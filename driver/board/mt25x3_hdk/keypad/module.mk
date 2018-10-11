
BOARD_SRC = driver/board/mt25x3_hdk
COMPONENT_SRC = driver/board/component

#keypad custome file
C_FILES  += $(BOARD_SRC)/keypad/src/keypad_custom.c

#################################################################################
#include path
CFLAGS 	+= -Iinclude
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc

#ctp driver include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/keypad/inc

CFLAGS  += -DMTK_KEYPAD_ENABLE
