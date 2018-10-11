SOURCE_DIR = ../../..
BINPATH = ~/gcc-arm-none-eabi/bin
PROJ_PATH ?= ../../../project/mt7687_hdk/apps/iot_sdk/GCC
CONFIG_PATH ?= .

CFLAGS += -I$(PROJ_PATH)/../inc
CFLAGS += -I../../../$(CONFIG_PATH)

FEATURE    ?= feature.mk

include $(PROJ_PATH)/$(FEATURE)

# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk

# Project name
TARGET_LIB=librtos

BUILD_DIR = Build
OUTPATH = Build

# suppress all building warning in this module
CFLAGS += -w

# Sources
include module.mk

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)


.PHONY: $(TARGET_LIB).a

all: $(TARGET_LIB).a
	@echo Build $< Done

include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)/$(TARGET_LIB).a
	rm -rf $(BUILD_DIR)


