# -*- makefile -*-
# Copyright 2019 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Board specific files build

# the IC is STmicro STM32F072CBU6TR
CHIP:=stm32
CHIP_FAMILY:=stm32f0
CHIP_VARIANT:=stm32f07x

# Use coreboot-sdk
$(call set-option,CROSS_COMPILE_arm,\
	$(CROSS_COMPILE_coreboot_sdk_arm),\
	/home/francesco/environment_of_develop/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-)

board-y=board.o
