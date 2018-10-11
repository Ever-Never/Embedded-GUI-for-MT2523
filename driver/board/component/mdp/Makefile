# Copyright Statement:
#
# (C) 2005-2016  MediaTek Inc. All rights reserved.
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
# Without the prior written permission of MediaTek and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
# You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
# if you have agreed to and been bound by the applicable license agreement with
# MediaTek ("License Agreement") and been granted explicit permission to do so within
# the License Agreement ("Permitted User").  If you are not a Permitted User,
# please cease any access or use of MediaTek Software immediately.
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
# ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#

SOURCE_DIR = ../../../..
BINPATH = ~/gcc-arm-none-eabi/bin

-include $(SOURCE_DIR)/.config

CC = $(BINPATH)/arm-none-eabi-gcc
AR = $(BINPATH)/arm-none-eabi-ar

# Project name
TARGET_LIB=libdrv_idp

DEBUG=1
FLOAT_TYPE=hard

BUILD_DIR = Build
OUTPATH=Build

include $(SOURCE_DIR)/kernel/rtos/FreeRTOS/module.mk

# mdp driver files
SOURCE_SRC = driver/board/component/mdp/src
SOURCE_FILES = $(SOURCE_SRC)/idp_core.c \
               $(SOURCE_SRC)/mt6252/idp_engines_mt6252.c \
               $(SOURCE_SRC)/mt6252/idp_scen_common_mt6252.c \
               $(SOURCE_SRC)/mt6252/idp_scen_cp_mt6252.c \
               $(SOURCE_SRC)/mt6252/idp_hw/idp_imgdma_mt6252.c \
               $(SOURCE_SRC)/mt6252/idp_hw/idp_imgdma_rotdma0_mt6252.c \
               $(SOURCE_SRC)/mt6252/idp_hw/idp_resz_crz_mt6252.c \
               $(SOURCE_SRC)/mt6252/idp_hw/idp_resz_mt6252.c \

C_FILES = $(SOURCE_FILES)
C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)

###################################################

# Check for valid float argument
# NOTE that you have to run make clan after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

###################################################
# CC Flags
ifeq ($(DEBUG), 1)
ALLFLAGS = -g -O0
else
ALLFLAGS = -Os
endif
ALLFLAGS += -Wall -mlittle-endian -mthumb -mcpu=cortex-m4
CFLAGS += $(ALLFLAGS) -flto -ffunction-sections -fdata-sections -fno-builtin

ifeq ($(FLOAT_TYPE), hard)
FPUFLAGS = -fsingle-precision-constant -Wdouble-promotion
FPUFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
else
FPUFLAGS = -fsingle-precision-constant -Wdouble-promotion
FPUFLAGS += -msoft-float
endif

# Definitions
CFLAGS += -D_REENT_SMALL
CFLAGS += -DARM_MATH_CM4
CFLAGS += -D__FPU_PRESENT
CFLAGS += $(FPUFLAGS)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(SOURCE_SRC)/../inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/camera/interface/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/camera/Camera_common/inc
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_sdk_dev/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/driver/board/component/common
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/lcd

.PHONY: $(TARGET_LIB).a

all: $(TARGET_LIB).a
	mv ./$(TARGET_LIB).a $(SOURCE_DIR)/middleware/MTK/camera/lib/$(TARGET_LIB).a

$(TARGET_LIB).a: $(C_OBJS)
	@echo Gen $(TARGET_LIB).a
	@$(AR) -r $@ $(C_OBJS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo Build... $@
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.d: $(SOURCE_DIR)/%.c
	@echo GenDep... $@
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	export D_FILE="$@"; \
	export B_NAME=`echo $$D_FILE | sed 's/\.d//g'`; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's@\(.*\)\.o@'"$$B_NAME\.o $$B_NAME\.d"'@g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	rm -rf $(OUTPATH) $(TARGET_LIB).a $(SOURCE_DIR)/middleware/MTK/camera/lib/$(TARGET_LIB).a

ifneq ($(MAKECMDGOALS),clean)
-include $(C_OBJS:.o=.d)
endif
