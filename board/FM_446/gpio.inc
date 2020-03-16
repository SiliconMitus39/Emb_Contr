/* -*- mode:c -*-
 *
 * Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Declare symbolic names for all the GPIOs that we care about.
 * Note: Those with interrupt handlers must be declared first. */

/* Inputs */
//GPIO_INT(CHARGER_INT_L, PIN(C, 13)	PIN(B, 3), GPIO_INT_RISING, sm5803_vbus_interrupt)		/* CHARGER_EC_INT_ODL from sm5803 */
GPIO_INT(USER_BUTTON, PIN(C, 7), GPIO_INT_FALLING, button_event)



GPIO(LED_U, PIN(A, 5), GPIO_OUT_LOW)
/* Unimplemented signals which we need to emulate for now */
UNIMPLEMENTED(ENTERING_RW)
UNIMPLEMENTED(WP_L)



/* Outputs */
GPIO(I2C1_SCL,			PIN(B, 6), GPIO_INPUT)
GPIO(I2C1_SDA,			PIN(B, 7), GPIO_INPUT)

ALTERNATE(PIN_MASK(B, 0x00C0), GPIO_ALT_I2C,    MODULE_I2C,    GPIO_PULL_UP) /* I2C MASTER:PB6/PB7 */


/* USART3 TX/RX */
GPIO(MCU_UART1_TX,		PIN(A, 9), GPIO_OUTPUT )
GPIO(MCU_UART1_RX,		PIN(A, 10), GPIO_OUTPUT )


ALTERNATE(PIN_MASK(A, 0x000C), 1, MODULE_UART, 0) /* USART2: PA2/PA3 */
ALTERNATE(PIN_MASK(A, 0x0600), 7, MODULE_UART, 0)/* USART1: PA9/PA10 - Console */





/* I2C0: PB6/7*/
/*ALTERNATE(PIN_MASK(B, 0x00C0), GPIO_ALT_F1, MODULE_I2C, GPIO_PULL_UP)*/
/*ALTERNATE(PIN_MASK(B, 0x0300), GPIO_ALT_I2C, MODULE_I2C,GPIO_PULL_UP)  I2C1: PB8-9 */

