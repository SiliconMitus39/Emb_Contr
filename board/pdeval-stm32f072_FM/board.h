/* Copyright 2015 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* STM32F072-discovery board based USB PD evaluation configuration */

#ifndef __BOARD_H
#define __BOARD_H

/* 48 MHz SYSCLK clock frequency */
#define CPU_CLOCK 48000000

/* the UART console is on USART2 (PA14/PA15) */
#undef CONFIG_UART_CONSOLE
#define CONFIG_UART_CONSOLE 2

/* Optional features */
#define CONFIG_HW_CRC
#define CONFIG_I2C
#define CONFIG_I2C_MASTER
#define CONFIG_STM_HWTIMER32


/* I2C interfaces */
#include "../driver/charger/sm5803.h"
#define I2C_PORT_BATTERY  I2C_PORT_CHARGER

/* I2C Bus Configuration */
#define CONFIG_I2C
#define CONFIG_I2C_MASTER
#define CONFIG_I2C_DEBUG
/*command fsco*/

#define CHARGE_PORT_USB_C     0/* fsco*/

#define HAS_TASK_HOSTCMD
#define CONFIG_HOSTCMD_EVENTS
#define CONFIG_BATTERY
/* Charger inserted */
#define CONFIG_CHARGER
#define CONFIG_CHARGER_SM5803

/* Common charger defines */

#define CONFIG_CHARGER_DISCHARGE_ON_AC
#define CONFIG_CHARGER_INPUT_CURRENT 1000 /* Allow low-current USB charging */
#define CONFIG_CHARGER_MIN_BAT_PCT_FOR_POWER_ON 1
#define CONFIG_CHARGER_NARROW_VDC
#define CONFIG_CHARGER_SENSE_RESISTOR 10
#define CONFIG_CHARGER_SENSE_RESISTOR_AC 10



/* Define typical operating power and max power. */
#define PD_MAX_VOLTAGE_MV     20000
#define PD_MAX_CURRENT_MA     1000
#define PD_MAX_POWER_MW       45000
#define PD_OPERATING_POWER_MW 15000
#define PD_VCONN_SWAP_DELAY   5000 /* us */

/* Battery */
#define CONFIG_BATTERY_CUT_OFF
#define CONFIG_BATTERY_DEVICE_CHEMISTRY "LION"
#define CONFIG_BATTERY_LEVEL_NEAR_FULL 95
#define CONFIG_BATTERY_HW_PRESENT_CUSTOM
#define CONFIG_BATTERY_PRESENT_CUSTOM
#define CONFIG_BATTERY_SMART
/* battery briefly requests V=0, A=0 when woken up */
#define CONFIG_BATTERY_REQUESTS_NIL_WHEN_DEAD

/* USB Power Delivery configuration */
#define CONFIG_USB_PD_TCPC_LOW_POWER
#define CONFIG_USB_POWER_DELIVERY
#define CONFIG_USB_PD_ALT_MODE
#define CONFIG_USB_PD_ALT_MODE_DFP
#define CONFIG_USB_PD_CUSTOM_PDO
#define CONFIG_USB_PD_DUAL_ROLE
#define CONFIG_USB_PD_PORT_MAX_COUNT 1
#define CONFIG_USB_PD_TCPM_TCPCI
#define CONFIG_USB_PD_VBUS_DETECT_TCPC
/*#define CONFIG_USB_PD_TCPM_ANX7447*/
#define CONFIG_USB_PD_TCPM_FUSB302
/*#define CONFIG_USB_PD_TCPM_MUX*/






/* fake board specific type-C power constants */
#define PD_POWER_SUPPLY_TURN_ON_DELAY  30000  /* us */
#define PD_POWER_SUPPLY_TURN_OFF_DELAY 650000 /* us */

/* Define typical operating power and max power */
#define PD_OPERATING_POWER_MW 15000
/*
#define PD_MAX_POWER_MW       60000
#define PD_MAX_CURRENT_MA     3000
*/
#define PD_MAX_VOLTAGE_MV     20000

/* I2C master port connected to the TCPC */
#define I2C_PORT_TCPC 0
#define I2C_PORT_PD_MCU 0

/* Timer selection */

#define CONFIG_USBC_VCONN
#define CONFIG_USBC_VCONN_SWAP
/* delay to turn on/off vconn */
#define PD_VCONN_SWAP_DELAY 5000 /* us */

/* USB Configuration */
#define CONFIG_USB
#define CONFIG_USB_PID 0x500f
#define CONFIG_USB_CONSOLE

/* USB interface indexes (use define rather than enum to expand them) */
#define USB_IFACE_CONSOLE 0
#define USB_IFACE_COUNT   1

/* USB endpoint indexes (use define rather than enum to expand them) */
#define USB_EP_CONTROL 0
#define USB_EP_CONSOLE 1
#define USB_EP_COUNT   2

#undef CONFIG_WATCHDOG_HELP
#undef CONFIG_LID_SWITCH

/*
 * Allow dangerous commands all the time, since we don't have a write protect
 * switch.
 */
#define CONFIG_SYSTEM_UNLOCKED

#ifndef __ASSEMBLER__

/* Timer selection */
#define TIM_CLOCK32 2

#include "gpio_signal.h"

/* USB string indexes */
enum usb_strings {
	USB_STR_DESC = 0,
	USB_STR_VENDOR,
	USB_STR_PRODUCT,
	USB_STR_VERSION,
	USB_STR_CONSOLE_NAME,

	USB_STR_COUNT
};

void board_reset_pd_mcu(void);

#endif /* !__ASSEMBLER__ */
#endif /* __BOARD_H */
