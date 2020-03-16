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


/* Outputs */


GPIO(LED_U, PIN(A, 5), GPIO_OUT_HIGH)

/* Unimplemented signals which we need to emulate for now */
UNIMPLEMENTED(ENTERING_RW)
UNIMPLEMENTED(WP_L)

ALTERNATE(PIN_MASK(A, 0x000C), 1, MODULE_UART,  0) /* USART2: PA2/PA3 */

GPIO(I2C0_SCL, PIN(B, 10), GPIO_ODR_HIGH) /* I2C port 0 SCL */
GPIO(I2C0_SDA, PIN(B, 11), GPIO_ODR_HIGH) /* I2C port 0 SDA */


/* I2C0: PB6/7*/
ALTERNATE(PIN_MASK(B, 0x0C00), GPIO_ALT_F1, MODULE_I2C, GPIO_PULL_UP)

