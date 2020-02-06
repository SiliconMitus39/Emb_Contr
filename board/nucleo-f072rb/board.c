/* Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "common.h"
#include "battery.h"
#include "extpower.h"
#include "board.h"
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
#include "util.h"
#include "tcpm.h"
#include "charge_manager.h"
#include "virtual_battery.h"
#include "panic.h"
#include "charger.h"


/* extern global var */
	extern int gpio_bat_present_l;
	extern int temp_batt_set;
	extern int statecharge;
	extern int voltagebatt;

	extern int  remcap;
	extern int  fullcap;
	extern int  status;// 0x80, 0x40, 0x20, 0x00  ;
	extern int battflag; //BATT_FLAG_WANT_CHARGE;//BATT_FLAG_RESPONSIVE;//

/* adding francesco */
#define I2C_PORT_MASTER		0
/* Console output macros */
#define CPUTS(outstr) cputs(CC_CHARGER, outstr)
#define CPRINTS(format, args...) cprints(CC_CHARGER, format, ## args)
#define CPRINTF(format, args...) cprintf(CC_CHARGER, format, ## args)
void button_event(enum gpio_signal signal)
{
	gpio_set_level(GPIO_LED_U, 1);
}

#ifdef CTS_MODULE
/*
 * Dummy interrupt handler. It's supposed to be overwritten by each suite
 * if needed.
 */
__attribute__((weak)) void cts_irq1(enum gpio_signal signal) {}
__attribute__((weak)) void cts_irq2(enum gpio_signal signal) {}
#endif

#include "gpio_list.h"

void tick_event(void)
{
	static int count;

	gpio_set_level(GPIO_LED_U, (count & 0x07) == 0);

	count++;
}
DECLARE_HOOK(HOOK_TICK, tick_event, HOOK_PRIO_DEFAULT);

#ifdef CTS_MODULE_I2C
const struct i2c_port_t i2c_ports[]  = {
	{"test", STM32_I2C1_PORT, 100, GPIO_I2C1_SCL, GPIO_I2C1_SDA},
};

const unsigned int i2c_ports_used = ARRAY_SIZE(i2c_ports);
#endif

/******************************************************************************
 * Initialize board.
 */
//static int force_discharge;


/* I2C ports */
const struct i2c_port_t i2c_ports[] = {
	{"master",
	 I2C_PORT_MASTER,
	 100,
	 GPIO_I2C0_SCL,
	 GPIO_I2C0_SDA},

};

/*
USB_fsco*/
#define CONFIG_USB_PD_PORT_MAX_COUNT 2
//#define I2C_PORT_TCPC                0

const unsigned int i2c_ports_used = ARRAY_SIZE(i2c_ports);

//const struct tcpc_config_t tcpc_config[CONFIG_USB_PD_PORT_MAX_COUNT] = {
//	{
//		.bus_type = EC_BUS_TYPE_I2C,
//		.i2c_info = {
//			.port = I2C_PORT_TCPC,
//			.addr_flags = 0x30,
//		},
//		/*.drv = &anx7447_tcpm_drv,*/
//	},
//};


static int charger_derate(int current)
{
	return current * 95 / 100;
}

static void board_init(void)
{
	gpio_enable_interrupt(GPIO_USER_BUTTON);
	charger_set_input_current(charger_derate(PD_MAX_CURRENT_MA));
	CPRINTS("set input current %d",PD_MAX_CURRENT_MA);

}
DECLARE_HOOK(HOOK_INIT, board_init, HOOK_PRIO_DEFAULT);



int board_set_active_charge_port(int port)
{
	int is_real_port = (port >= 0 &&
			    port < CONFIG_USB_PD_PORT_MAX_COUNT);
//	int i;
//	int rv;

	if (!is_real_port && port != CHARGE_PORT_NONE)
		return EC_ERROR_INVAL;

	CPRINTS("New chg p%d", port);

//	if (port == CHARGE_PORT_NONE) {
//		/* Disable all ports. */
//		for (i = 0; i < CONFIG_USB_PD_PORT_MAX_COUNT; i++) {
//			rv = board_vbus_sink_enable(i, 0);
//			if (rv) {
//				CPRINTS("Disabling p%d sink path failed.", i);
//				return rv;
//			}
//		}
//
//		return EC_SUCCESS;
//	}

	/* Check if the port is sourcing VBUS. */
	if (board_is_sourcing_vbus(port)) {
		CPRINTF("Skip enable p%d", port);
		return EC_ERROR_INVAL;
	}

	/*
	 * Turn off the other ports' sink path FETs, before enabling the
	 * requested charge port.
//	 */
//	for (i = 0; i < CONFIG_USB_PD_PORT_MAX_COUNT; i++) {
//		if (i == port)
//			continue;
//
//		if (board_vbus_sink_enable(i, 0))
//			CPRINTS("p%d: sink path disable failed.", i);
//	}

//	/* Enable requested charge port. */
//	if (board_vbus_sink_enable(port, 1)) {
//		CPRINTS("p%d: sink path enable failed.", port);
//		return EC_ERROR_UNKNOWN;
//	}

	return EC_SUCCESS;
}
//int board_set_active_charge_port(int charge_port)
//{
//	CPRINTS("New chg p%d", charge_port);
//
//	/* ignore all request when discharge mode is on */
//	if (force_discharge)
//		return EC_SUCCESS;
//
//	switch (charge_port) {
//	case CHARGE_PORT_USB_C:
//		/* Don't charge from a source port */
//		if (board_vbus_source_enabled(charge_port))
//			return -1;
//		break;
//	case CHARGE_PORT_NONE:
//		/*
//		 * To ensure the fuel gauge (max17055) is always powered
//		 * even when battery is disconnected, keep VBAT rail on but
//		 * set the charging current to minimum.
//		 */
//		charger_set_current(0);
//		break;
//	default:
//		panic("Invalid charge port\n");
//		break;
//	}
//
//	return EC_SUCCESS;
//}



int extpower_is_present(void)
{

	 /* The charger will indicate VBUS presence if we're sourcing 5V,
	 * so exclude such ports.*/

	int usb_c_extpower_present;
	CPRINTS("EXTERNAL_PWR");
	if (board_vbus_source_enabled(CHARGE_PORT_USB_C))
		usb_c_extpower_present = 0;
	else
		usb_c_extpower_present = 1; //tcpm_get_vbus_level(CHARGE_PORT_USB_C);

	return usb_c_extpower_present;

}







/******************************************************************************
 * Set the command of the Board.
 */

static int setbatt_pres(int argc, char **argv) {

	static int set_batt = 0;
	enum present{
		BATT_NOT_PRESENT,
		BATT_PRESENT
	};

	int rv = EC_SUCCESS;
	char *end;
	if (argc != 2)
		rv = EC_ERROR_PARAM_COUNT;

	set_batt = strtoi(argv[1], &end, 0);
	switch (set_batt) {
	case BATT_PRESENT:
		gpio_bat_present_l = 1;
		break;
	case BATT_NOT_PRESENT:
		gpio_bat_present_l = 0;
		break;
	default:
		rv = EC_ERROR_PARAM1;
		break;
		return rv;
	}
	return rv;
}

DECLARE_CONSOLE_COMMAND(sbp,setbatt_pres, "Give the possibility to set the battery presence", "help");


static int setbatt_temp(int argc, char **argv) {


	int rv = EC_SUCCESS;
	char *end;
	if (argc != 2)
		rv = EC_ERROR_PARAM_COUNT;

	temp_batt_set = strtoi(argv[1], &end, 0);
	if ((temp_batt_set > 5000) && (temp_batt_set) < -1 && (argc == 2) )
		rv = EC_ERROR_PARAM1;

	return rv;
}

DECLARE_CONSOLE_COMMAND(tempbattset,setbatt_temp, "Give the possibility to set the battery temperature", "help");


static int st_charge(int argc, char **argv) {


	int rv = EC_SUCCESS;
	char *end;
	if (argc != 2)
		rv = EC_ERROR_PARAM_COUNT;

	statecharge = strtoi(argv[1], &end, 0);
	if ((temp_batt_set > -1) && (temp_batt_set) < 5000 && (argc == 2) )
		rv = EC_ERROR_PARAM1;

	return rv;
}

DECLARE_CONSOLE_COMMAND(statecharge,st_charge, "Give the possibility to set the status of the charge", "help");


static int voltage_batt(int argc, char **argv) {


	int rv = EC_SUCCESS;
	char *end;
	if (argc != 2)
		rv = EC_ERROR_PARAM_COUNT;

	voltagebatt = strtoi(argv[1], &end, 0);
	if ((voltagebatt > -1) && (voltagebatt) < 8000 && (argc == 2) )
		rv = EC_ERROR_PARAM1;

	return rv;
}

DECLARE_CONSOLE_COMMAND(voltagebatt,voltage_batt, "Give the possibility to set voltage", "help");







static int rem_cap(int argc, char **argv) {


	int rv = EC_SUCCESS;
	char *end;
	if (argc != 2)
		rv = EC_ERROR_PARAM_COUNT;

	remcap = strtoi(argv[1], &end, 0);
	if ((remcap > -1) && (remcap) < 9000 && (argc == 2) )
		rv = EC_ERROR_PARAM1;

	return rv;
}

DECLARE_CONSOLE_COMMAND(remcap,rem_cap, "Give the possibility to set the remain capacity", "help");


static int full_cap(int argc, char **argv) {


	int rv = EC_SUCCESS;
	char *end;
	if (argc != 2)
		rv = EC_ERROR_PARAM_COUNT;

	fullcap = strtoi(argv[1], &end, 0);
	if ((fullcap > -1) && (fullcap) < 9000 && (argc == 2) )
		rv = EC_ERROR_PARAM1;

	return rv;
}

DECLARE_CONSOLE_COMMAND(fullcap,full_cap, "Give the possibility to set the full capacity", "help");


static int batt_status(int argc, char **argv) {


	int rv = EC_SUCCESS;
	char *end;
	if (argc != 2)
		rv = EC_ERROR_PARAM_COUNT;

	status = strtoi(argv[1], &end, 0);
	if ((!((status != 0x80) || (status != 0x40) ||(status != 0x20) || (status != 0x00)))&& (argc == 2) )
		rv = EC_ERROR_PARAM1;

	return rv;
}

DECLARE_CONSOLE_COMMAND(battstatus,batt_status, "Give the possibility to set the battery status", "help");


static int batt_flag(int argc, char **argv) {


	int rv = EC_SUCCESS;
	char *end;
	if (argc != 2)
		rv = EC_ERROR_PARAM_COUNT;

	battflag = strtoi(argv[1], &end, 0);
	if ((!((battflag != 0x01) || (status != 0x02)))&& (argc == 2) )
		rv = EC_ERROR_PARAM1;

	return rv;
}

DECLARE_CONSOLE_COMMAND(battflag,batt_flag, "Give the possibility to set the batt_flag", "help");

