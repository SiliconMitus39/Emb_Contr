/* Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Nucleo-F072RB board configuration */

#ifndef __CROS_EC_BOARD_H
#define __CROS_EC_BOARD_H


/* 48 MHz SYSCLK clock frequency */
#define CPU_CLOCK 48000000
#define  CONFIG_EC_EC_COMM_BATTERY
/* the UART console is on USART2 (PA14/PA15) */
#undef CONFIG_UART_CONSOLE
#define CONFIG_UART_CONSOLE 2
/*fsco*/
#define CONFIG_USB_PD_MAX_SINGLE_SOURCE_CURRENT TYPEC_RP_3A0

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


/* USB_Config */
#define DEDICATED_CHARGE_PORT	CHARGE_PORT_USB_C/*fsco*/
#define GPIO_EN_USB_C1_5V_OUT 1
#define GPIO_USB_C0_5V_EN   GPIO_EN_USB_C1_5V_OUT
#define GPIO_USB_C1_5V_EN	GPIO_EN_USB_C1_5V_OUT
#define GPIO_EN_USB_C0_3A   3
#define GPIO_EN_USB_C1_3A   3
#define GPIO_EN_USB_C0_CHARGE_L 2
#define GPIO_EN_USB_C1_CHARGE_L 2

#define GPIO_SLP_SUS_L_PMIC   5
#define GPIO_PMIC_SLP_SUS_L	GPIO_SLP_SUS_L_PMIC
#define GPIO_USB2_ID  0
#define GPIO_USB2_VBUSSENSE 1

/*
 * Require PD negotiation to be complete prior to booting Linux, but don't
 * care about how much power we negotiate.

#define CONFIG_CHARGER_LIMIT_POWER_THRESH_CHG_MW 15001*/

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


/* Optional features */
#define CONFIG_STM_HWTIMER32

#ifdef CTS_MODULE
#undef STM32_IRQ_EXT2_3_PRIORITY
#define STM32_IRQ_EXT2_3_PRIORITY	2
#ifdef CTS_MODULE_I2C
#define CONFIG_I2C
#define CONFIG_I2C_MASTER
#endif
#endif

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


/* Custom function to indicate if sourcing VBUS */
int board_is_sourcing_vbus(int port);

#endif /* !__ASSEMBLER__ */
#endif /* __CROS_EC_BOARD_H */
