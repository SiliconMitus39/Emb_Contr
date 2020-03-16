/* Copyright 2015 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
/* STM32F072-discovery board based USB PD evaluation configuration */
#include "../pdeval-stm32f072_FM/board.h"

#include "common.h"
#include "battery.h"
#include "extpower.h"
#include "../driver/charger/sm5803.h"
#include "gpio.h"
#include "hooks.h"
#include "registers.h"
#include "task.h"
#include "i2c.h"
#include "timer.h"
#include "host_command.h"
#include "console.h"
#include "math_util.h"
#include "uart.h"
#include "ec_version.h"

#include "tcpm.h"
#include "charge_manager.h"
#include "virtual_battery.h"
#include "panic.h"
#include "charger.h"
#include "fusb302.h"
#include "usb_descriptor.h"
#include "usb_pd.h"
#include "usb_pd_tcpm.h"
#include "util.h"
#include "usb_hw.h"


/* adding francesco */
#define I2C_PORT_MASTER		0

/* Console output macros */
#define CPUTS(outstr) cputs(CC_CHARGER, outstr)
#define CPRINTS(format, args...) cprints(CC_CHARGER, format, ## args)
#define CPRINTF(format, args...) cprintf(CC_CHARGER, format, ## args)



void button_event(enum gpio_signal signal);

void alert_event(enum gpio_signal signal)
{
	/* Exchange status with PD MCU. */
	host_command_pd_send_status(PD_CHARGE_NO_CHANGE);
}

#include "gpio_list.h"

const void *const usb_strings[] = {
	[USB_STR_DESC]         = usb_string_desc,
	[USB_STR_VENDOR]       = USB_STRING_DESC("Google Inc."),
	[USB_STR_PRODUCT]      = USB_STRING_DESC("PDeval-stm32f072"),
	[USB_STR_VERSION]      = USB_STRING_DESC(CROS_EC_VERSION32),
	[USB_STR_CONSOLE_NAME] = USB_STRING_DESC("Shell"),
};

BUILD_ASSERT(ARRAY_SIZE(usb_strings) == USB_STR_COUNT);

static int charger_derate(int current)
{
	return current * 95 / 100;
}



/* Initialize board. */
static void board_init(void)
{
	gpio_enable_interrupt(GPIO_USER_BUTTON);
	gpio_enable_interrupt(GPIO_PD_MCU_INT);
	charger_set_input_current(charger_derate(PD_MAX_CURRENT_MA));
	CPRINTS("set input current %d",PD_MAX_CURRENT_MA);

}
DECLARE_HOOK(HOOK_INIT, board_init, HOOK_PRIO_DEFAULT);

void board_reset_pd_mcu(void)
{
}

/* I2C ports */
const struct i2c_port_t i2c_ports[] = {
	{"tcpc", I2C_PORT_TCPC, 100 /* kHz */, GPIO_I2C0_SCL, GPIO_I2C0_SDA}
};
const unsigned int i2c_ports_used = ARRAY_SIZE(i2c_ports);

const struct tcpc_config_t tcpc_config[CONFIG_USB_PD_PORT_MAX_COUNT] = {
	{
		.bus_type = EC_BUS_TYPE_I2C,
		.i2c_info = {
			.port = I2C_PORT_TCPC,
			.addr_flags = FUSB302_I2C_SLAVE_ADDR_FLAGS,
		},
		.drv = &fusb302_tcpm_drv,
	},
};

uint16_t tcpc_get_alert_status(void)
{
	uint16_t status = 0;

	if (!gpio_get_level(GPIO_PD_MCU_INT)) {
		status = PD_STATUS_TCPC_ALERT_0;
	}

	return status;
}


int extpower_is_present(void)
{

	 /* The charger will indicate VBUS presence if we're sourcing 5V,
	 * so exclude such ports.*/

	int usb_c_extpower_present;
	CPRINTS("EXTERNAL_PWR");
	if (/*board_vbus_source_enabled(CHARGE_PORT_USB_C)*/0)
		usb_c_extpower_present = 0;
	else
		usb_c_extpower_present = 1; /*tcpm_get_vbus_level(CHARGE_PORT_USB_C);*/

	return usb_c_extpower_present;

}

