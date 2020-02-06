/* Copyright 2018 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "charger.h"
#include "console.h"
#include "gpio.h"
#include "system.h"
#include "timer.h"
#include "usb_mux.h"
#include "usb_pd.h"
#include "usb_pd_policy.h"
#include "util.h"

#define CPRINTF(format, args...) cprintf(CC_USBPD, format, ## args)
#define CPRINTS(format, args...) cprints(CC_USBPD, format, ## args)

static int board_get_polarity(int port)
{
	/* Krane's aux mux polarity is reversed. Workaround to flip it back. */
	if (IS_ENABLED(BOARD_KRANE) && board_get_version() == 3)
		return !pd_get_polarity(port);

	return pd_get_polarity(port);
}

static uint8_t vbus_en;

int board_vbus_source_enabled(int port)
{
	return vbus_en;
}

int board_is_sourcing_vbus(int port)
{
	if (IS_ENABLED(BOARD_KUKUI) && board_get_version() <= 1)
		return charger_is_sourcing_otg_power(port);
	else
		return board_vbus_source_enabled(port);
}

int pd_set_power_supply_ready(int port)
{
	if (port != CHARGE_PORT_USB_C)
		return EC_ERROR_INVAL;

	pd_set_vbus_discharge(port, 0);
	/* Provide VBUS */
	vbus_en = 1;

	charger_enable_otg_power(1);

	gpio_set_level(GPIO_EN_USBC_CHARGE_L, 1);
	gpio_set_level(GPIO_EN_PP5000_USBC, 1);

	/* notify host of power info change */
	pd_send_host_event(PD_EVENT_POWER_CHANGE);

	return EC_SUCCESS; /* we are ready */
}

void pd_power_supply_reset(int port)
{
	int prev_en;

	if (port != CHARGE_PORT_USB_C)
		return;

	prev_en = vbus_en;
	/* Disable VBUS */
	vbus_en = 0;
	/* Enable discharge if we were previously sourcing 5V */
	if (prev_en)
		pd_set_vbus_discharge(port, 1);

	charger_enable_otg_power(0);
	gpio_set_level(GPIO_EN_PP5000_USBC, 0);

	/* notify host of power info change */
	pd_send_host_event(PD_EVENT_POWER_CHANGE);
}

int pd_check_vconn_swap(int port)
{
	/*
	 * VCONN is provided directly by the battery (PPVAR_SYS)
	 * but use the same rules as power swap.
	 */
	return pd_get_dual_role(port) == PD_DRP_TOGGLE_ON ? 1 : 0;
}

/* ----------------- Vendor Defined Messages ------------------ */
#ifdef CONFIG_USB_PD_ALT_MODE_DFP
__overridable int board_has_virtual_mux(void)
{
	return IS_ENABLED(CONFIG_USB_MUX_VIRTUAL);
}

__override void svdm_safe_dp_mode(int port)
{
	/* make DP interface safe until configure */
	dp_flags[port] = 0;
	dp_status[port] = 0;
	usb_mux_set(port, TYPEC_MUX_NONE,
		    USB_SWITCH_CONNECT, board_get_polarity(port));
}

__override int svdm_enter_dp_mode(int port, uint32_t mode_caps)
{
	/* Kukui/Krane doesn't support superspeed lanes. */
	const uint32_t support_pin_mode = board_has_virtual_mux() ?
		(MODE_DP_PIN_C | MODE_DP_PIN_E) : MODE_DP_PIN_ALL;

	/**
	 * Only enter mode if device is DFP_D (and PIN_C/E for Kukui/Krane)
	 * capable
	 */
	if ((mode_caps & MODE_DP_SNK) &&
	    (mode_caps & (support_pin_mode << MODE_DP_DFP_PIN_SHIFT))) {
		svdm_safe_dp_mode(port);
		return 0;
	}

	CPRINTS("ERR:DP mode SNK or C&E missing! 0x%x", mode_caps);
	return -1;
}

__override int svdm_dp_config(int port, uint32_t *payload)
{
	int opos = pd_alt_mode(port, USB_SID_DISPLAYPORT);
	int status = dp_status[port];
	int mf_pref = PD_VDO_DPSTS_MF_PREF(dp_status[port]);
	int pin_mode;

	/* Kukui doesn't support multi-function mode, mask it out. */
	if (board_has_virtual_mux())
		status &= ~PD_VDO_DPSTS_MF_MASK;

	pin_mode = pd_dfp_dp_get_pin_mode(port, status);

	if (!pin_mode)
		return 0;

	if (board_has_virtual_mux())
		usb_mux_set(port, TYPEC_MUX_DP, USB_SWITCH_CONNECT,
			    board_get_polarity(port));
	else
		usb_mux_set(port, mf_pref ? TYPEC_MUX_DOCK : TYPEC_MUX_DP,
			    USB_SWITCH_CONNECT, board_get_polarity(port));

	payload[0] = VDO(USB_SID_DISPLAYPORT, 1,
			 CMD_DP_CONFIG | VDO_OPOS(opos));
	payload[1] = VDO_DP_CFG(pin_mode,      /* pin mode */
				1,             /* DPv1.3 signaling */
				2);            /* UFP connected */
	return 2;
};

/*
 * timestamp of the next possible toggle to ensure the 2-ms spacing
 * between IRQ_HPD.
 */
static uint64_t hpd_deadline[CONFIG_USB_PD_PORT_MAX_COUNT];

__override void svdm_dp_post_config(int port)
{
	const struct usb_mux * const mux = &usb_muxes[port];

	dp_flags[port] |= DP_FLAGS_DP_ON;
	if (!(dp_flags[port] & DP_FLAGS_HPD_HI_PENDING))
		return;

	gpio_set_level(GPIO_USB_C0_HPD_OD, 1);
#ifdef VARIANT_KUKUI_DP_MUX_GPIO
	board_set_dp_mux_control(1, board_get_polarity(port));
#endif

	/* set the minimum time delay (2ms) for the next HPD IRQ */
	hpd_deadline[port] = get_time().val + HPD_USTREAM_DEBOUNCE_LVL;
	mux->hpd_update(port, 1, 0);
}

__override int svdm_dp_attention(int port, uint32_t *payload)
{
	int cur_lvl = gpio_get_level(GPIO_USB_C0_HPD_OD);
	int lvl = PD_VDO_DPSTS_HPD_LVL(payload[1]);
	int irq = PD_VDO_DPSTS_HPD_IRQ(payload[1]);
	const struct usb_mux * const mux = &usb_muxes[port];

	dp_status[port] = payload[1];

	/* Its initial DP status message prior to config */
	if (!(dp_flags[port] & DP_FLAGS_DP_ON)) {
		if (lvl)
			dp_flags[port] |= DP_FLAGS_HPD_HI_PENDING;
		return 1;
	}

	mux->hpd_update(port, lvl, irq);

	if (irq & cur_lvl) {
		uint64_t now = get_time().val;
		/* wait for the minimum spacing between IRQ_HPD if needed */
		if (now < hpd_deadline[port])
			usleep(hpd_deadline[port] - now);

		/* generate IRQ_HPD pulse */
		gpio_set_level(GPIO_USB_C0_HPD_OD, 0);
		usleep(HPD_DSTREAM_DEBOUNCE_IRQ);
		gpio_set_level(GPIO_USB_C0_HPD_OD, 1);

#ifdef VARIANT_KUKUI_DP_MUX_GPIO
		board_set_dp_mux_control(1, board_get_polarity(port));
#endif

		/* set the minimum time delay (2ms) for the next HPD IRQ */
		hpd_deadline[port] = get_time().val + HPD_USTREAM_DEBOUNCE_LVL;
	} else if (irq & !lvl) {
		CPRINTF("ERR:HPD:IRQ&LOW\n");
		return 0; /* nak */
	} else {
		gpio_set_level(GPIO_USB_C0_HPD_OD, lvl);
#ifdef VARIANT_KUKUI_DP_MUX_GPIO
		board_set_dp_mux_control(lvl, board_get_polarity(port));
#endif
		/* set the minimum time delay (2ms) for the next HPD IRQ */
		hpd_deadline[port] = get_time().val + HPD_USTREAM_DEBOUNCE_LVL;
	}

	/* ack */
	return 1;
}

__override void svdm_exit_dp_mode(int port)
{
	const struct usb_mux * const mux = &usb_muxes[port];

	svdm_safe_dp_mode(port);
	gpio_set_level(GPIO_USB_C0_HPD_OD, 0);
#ifdef VARIANT_KUKUI_DP_MUX_GPIO
	board_set_dp_mux_control(0, 0);
#endif
	mux->hpd_update(port, 0, 0);
}
#endif /* CONFIG_USB_PD_ALT_MODE_DFP */
