# -*- makefile -*-
# Copyright 2016 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Board specific files build

# the IC is STmicro STM32F072RBT6
CHIP:=stm32
CHIP_FAMILY:=stm32f0
CHIP_VARIANT:=stm32f07x

board-y=board.o usb_pd_policy.o 
#common-$(CONFIG_I2C_MASTER)+=i2c_master.o
#common-$(HAS_TASK_HOSTCMD)+=host_command.o ec_features.o
#common-$(CONFIG_CHARGER)+=charger.o charge_state_v2.o
#common-$(CONFIG_HOSTCMD_EVENTS)+=host_event_commands.o
board-$(CONFIG_BATTERY_SMART)+=battery.o
