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

SOURCE_DIR = ../../../../..

PWD= $(shell pwd)
SDK_PATH    = $(abspath $(PWD)/$(SOURCE_DIR))
FEATURE ?= feature.mk
include $(FEATURE)
# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk
# EPT Config
-include $(SOURCE_DIR)/driver/board/mt25x3_hdk/ept/module.mk
# Keypad Config
-include $(SOURCE_DIR)/driver/board/mt25x3_hdk/keypad/module.mk
# Led module
ifeq ($(MTK_LED_ENABLE),y)
-include $(SOURCE_DIR)/driver/board/mt25x3_hdk/led/module.mk
endif

RAM_BOOTING=0

DEBUG = 0
FLOAT_TYPE = hard
BUILD_DIR = $(PWD)/Build

# Project name
PROJ_NAME       = $(notdir $(abspath $(dir $(PWD))))
PROJ_PATH = $(PWD)
OUTPATH = $(PWD)/Build

###################################################
# Sources or Lib
# RTOS source files
include $(SOURCE_DIR)/kernel/rtos/FreeRTOS/module.mk

# mems driver files
include $(SOURCE_DIR)/driver/board/component/mems/module.mk
include $(SOURCE_DIR)/driver/board/mt25x3_hdk/mems_init/module.mk

# sensor subsys files
include $(SOURCE_DIR)/middleware/MTK/sensor_subsys/module.mk
# fatfs files
include $(SOURCE_DIR)/middleware/third_party/fatfs/module.mk

# HAL driver files
-include $(SOURCE_DIR)/driver/board/mt25x3_hdk/module.mk
-include $(SOURCE_DIR)/driver/chip/mt2523/module.mk

# NVDM files
ifeq ($(MTK_NVDM_ENABLE), y)
include $(SOURCE_DIR)/middleware/MTK/nvdm/module.mk
endif

# BT codec files
include $(SOURCE_DIR)/prebuilt/driver/board/component/bt_codec/module.mk

# audio library files
include $(SOURCE_DIR)/prebuilt/driver/board/component/audio/module.mk

# audio middleware files
include $(SOURCE_DIR)/middleware/MTK/audio/module.mk

# kernel service files
include $(SOURCE_DIR)/kernel/service/module.mk

ifeq ($(MTK_PORT_SERVICE_ENABLE), y)
# add PORT SERVICE files
include $(SOURCE_DIR)/middleware/MTK/port_service/module.mk
endif

ifeq ($(MTK_USB_DEMO_ENABLED), y)
# add USB CLASS files
include $(SOURCE_DIR)/middleware/MTK/usb/module.mk
endif

ifeq ($(MTK_USB_CONNECTED_STOP_ALL), y)
  CFLAGS	+= -DMTK_USB_CONNECTED_STOP_ALL
endif

# add ATCI files
include $(SOURCE_DIR)/middleware/MTK/atci/module.mk

CFLAGS += -DBT_DEBUG

# Bluetooth module
include $(SOURCE_DIR)/middleware/MTK/bluetooth/module.mk

# Keypad module
include $(SOURCE_DIR)/middleware/MTK/key_event/module.mk

# add CFW files
#CFLAGS	+= -D__CFW_CONFIG_MODE__
#include $(SOURCE_DIR)/middleware/MTK/config_framework/module.mk

# BT Sink module
include $(SOURCE_DIR)/middleware/MTK/sink/module.mk

# BT notify module
include $(SOURCE_DIR)/middleware/MTK/bt_notify/module.mk


# FOTA files
include $(SOURCE_DIR)/middleware/MTK/fota/module.mk

# for pbpac
#include mbedtls
include $(SOURCE_DIR)/middleware/third_party/mbedtls/module.mk

# BT callback manager
include $(SOURCE_DIR)/middleware/MTK/bt_callback_manager/module.mk

# bt components le module
include $(SOURCE_DIR)/middleware/MTK/bluetooth_service/module.mk

# ANCS module
ifneq ($(wildcard $(strip $(SOURCE_DIR))/middleware/MTK/ble_ancs/src_protected/),)
include $(SOURCE_DIR)/middleware/MTK/ble_ancs/module.mk
else
include $(SOURCE_DIR)/prebuilt/middleware/MTK/ble_ancs/module.mk
endif


# ble battery service module
include $(SOURCE_DIR)/middleware/MTK/ble_bas/module.mk

# IAS service module
include $(SOURCE_DIR)/middleware/MTK/ble_ias/module.mk

# Main APP files
APP_PATH        = $(patsubst $(SDK_PATH)/%,%,$(abspath $(dir $(PWD))))
APP_PATH_SRC = project/mt2523_hdk/apps/headset_ref_design/src

APP_FILES = $(APP_PATH_SRC)/main.c \
            $(APP_PATH_SRC)/regions_init.c \
            $(APP_PATH)/GCC/syscalls.c \
            $(APP_PATH_SRC)/hci_log.c \
            $(APP_PATH_SRC)/task_def.c \
            $(APP_PATH_SRC)/system_mt2523.c \
            $(APP_PATH_SRC)/bt_init.c \
            $(APP_PATH_SRC)/fota/fota_main.c \
            $(APP_PATH_SRC)/fota/fota_bt_common.c \
            $(APP_PATH_SRC)/gatt_service.c \
            $(APP_PATH_SRC)/bt_gattc_discovery.c \
            $(APP_PATH_SRC)/AudioPlayer/audio_player_main.c \
            $(APP_PATH_SRC)/AudioPlayer/audio_file_manager/ucs2_util.c \
            $(APP_PATH_SRC)/AudioPlayer/audio_file_manager/audio_file_manager.c \
            $(APP_PATH_SRC)/bt_app_common.c \
            $(APP_PATH_SRC)/ble_bas_app/ble_bas_app.c \
            $(APP_PATH_SRC)/ble_find_me_server/ble_find_me_server.c

ifeq ($(MTK_PROMPT_SOUND_ENABLE), y)
APP_FILES += $(APP_PATH_SRC)/voice_prompt_mix.c \
             $(APP_PATH_SRC)/atci_cmd.c
endif

#Battery related file
APP_FILES += $(APP_PATH_SRC)/battery_message.c

APP_FILES += $(APP_PATH_SRC)/low_power_demo/sleep_at_cmd.c
APP_FILES += $(APP_PATH_SRC)/low_power_demo/bt_at_cmd.c
APP_FILES += $(APP_PATH_SRC)/low_power_demo/local_at_cmd.c

# BT audio app
include $(SOURCE_DIR)/project/mt2523_hdk/apps/headset_ref_design/src/sink/module.mk

# EPT files selection
ifeq ($(MTK_CORD_FREE_HEADSET_ENABLE), y)
  CFLAGS	+= -DMTK_CORD_FREE_HEADSET_ENABLE
  
  APP_FILES += $(APP_PATH_SRC)/ept_gpio_var_cordfree.c \
               $(APP_PATH_SRC)/ept_eint_var_cordfree.c
else
  ifeq ($(MTK_INEAR_NOODLES_HEADSET_ENABLE), y)
    CFLAGS	+= -DMTK_INEAR_NOODLES_HEADSET_ENABLE
  
    APP_FILES += $(APP_PATH_SRC)/ept_gpio_var_inear.c \
                 $(APP_PATH_SRC)/ept_eint_var_inear.c
  else
    APP_FILES += $(APP_PATH_SRC)/ept_gpio_var_hdk.c \
                 $(APP_PATH_SRC)/ept_eint_var_hdk.c
  endif
endif

# MTK_MP3_ON_FLASH_ENABLE define
ifeq ($(MTK_MP3_ON_FLASH_ENABLE), y)
  CFLAGS	+= -DMTK_MP3_ON_FLASH_ENABLE
endif

C_FILES += $(APP_FILES)
C_FILES += $(APP_PATH_SRC)/Notification/bt_notify_app.c
C_FILES += $(APP_PATH_SRC)/ble_ancs_ios_notification/ble_ancs_app.c
C_FILES += $(APP_PATH_SRC)/ble_ancs_ios_notification/bt_ancs_common.c

#C_FILES += $(APP_PATH_SRC)/Notification/bt_notify_test.c
C_FILES += driver/board/mt25x3_hdk/ept/src/bsp_gpio_ept_config.c
# MTK_SMART_BATTERY_ENABLE define
ifeq ($(MTK_SMART_BATTERY_ENABLE), y)
CFLAGS	+= -DMTK_SMART_BATTERY_ENABLE
endif
#CFLAGS	+= -DBMT_DEBUG_ENABLE

# battery_management files
include $(SOURCE_DIR)/middleware/MTK/battery_management/module.mk

# add startup file to build
S_FILES += $(APP_PATH)/GCC/startup_mt2523.s

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
CXX_OBJS        = $(CXX_FILES:%.cpp=$(BUILD_DIR)/%.o)
S_OBJS = $(S_FILES:%.s=$(BUILD_DIR)/%.o)

# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
LDFLAGS += -Wl,-wrap=malloc -Wl,-wrap=calloc -Wl,-wrap=realloc -Wl,-wrap=free
ifeq ($(RAM_BOOTING), 1)
  CFLAGS+= -DRAM_BOOTING 
  LDFLAGS += -Wl,-T$(patsubst $(SDK_PATH)/%,%,$(PWD))/ram.ld -Wl,--gc-sections
else
  ifeq ($(MTK_MP3_ON_FLASH_ENABLE), y)
    LDFLAGS += -Wl,-T$(patsubst $(SDK_PATH)/%,%,$(PWD))/flash_mp3.ld -Wl,--gc-sections
  else
    ifeq ($(MTK_INEAR_NOODLES_HEADSET_ENABLE), y)
      LDFLAGS += -Wl,-T$(patsubst $(SDK_PATH)/%,%,$(PWD))/flash_inear.ld -Wl,--gc-sections
    else
      LDFLAGS += -Wl,-T$(patsubst $(SDK_PATH)/%,%,$(PWD))/flash.ld -Wl,--gc-sections
    endif
  endif
endif
ifeq ($(DEBUG_IO), 1)
LDFLAGS += --specs=rdimon.specs -lrdimon
CFLAGS += -DDEBUG_IO
endif
#LDFLAGS = $(CFLAGS)


ifeq ($(MTK_QUERY_SDK_VERSION),y)
CFLAGS += -DMTK_QUERY_SDK_VERSION
endif

ifeq ($(MTK_DEMO_PROJ_LOWPOWER_AT_CMD_ENABLE),y)
CFLAGS += -DMTK_DEMO_PROJ_LOWPOWER_AT_CMD_ENABLE
endif

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/headset_ref_design/inc
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/headset_ref_design/inc/notification
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/headset_ref_design/inc/fota
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/headset_ref_design/inc/ble_ancs_ios_notification
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/headset_ref_design/inc/AudioPlayer
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/headset_ref_design/inc/AudioPlayer/audio_file_manager
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/headset_ref_design/inc/ble_find_me_server
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/headset_ref_design/inc/ble_bas_app
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/headset_ref_design/inc/voice_tone
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/bootloader/core/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/include
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_notify/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/ept/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_commonents_le/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_callback_manager/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/ble_ias/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/ble_bas/inc
#CFLAGS += -fwide-exec-charset=UCS2
###################################################

###################################################
# Rules
.PHONY: proj clean $(MODULE_PATH)

all: proj
	@echo $(APP_PATH)
	@mkdir -p $(BUILD_DIR)
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	@$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD)

MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH)

$(LIBS): $(MODULE_PATH)

$(MODULE_PATH):
	@echo $(APP_PATH)
	@+make -C $@ $(MOD_EXTRA) $($@_EXTRA)
	make -C $(MOD_EXTRA) CONFIG_PATH=$(APP_PATH)/inc

proj: $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(C_OBJS) $(CXX_OBJS) $(S_OBJS) $(LIBS)
	@echo Linking...
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).map" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).map"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).dis" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).dis"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).hex" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).hex"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	@echo $(patsubst $(SOURCE_DIR)/%,%,$(patsubst $(SDK_PATH)/%,%, $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -Wl,-Map=$(patsubst $(SDK_PATH)/%,%,$(OUTPATH)/$(PROJ_NAME).map) -lm -lstdc++ -o $@)) > $(OUTPATH)/link_option.tmp
	@cd $(SOURCE_DIR) && \
	$(patsubst $(SOURCE_DIR)/%,%,$(CC)) @$(OUTPATH)/link_option.tmp 2>>$(ERR_LOG)
	@rm -f $(OUTPATH)/link_option.tmp
	@$(OBJDUMP) -D $(OUTPATH)/$(PROJ_NAME).elf > $(OUTPATH)/$(PROJ_NAME).dis
	@$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	@$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	@echo Done

include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)
