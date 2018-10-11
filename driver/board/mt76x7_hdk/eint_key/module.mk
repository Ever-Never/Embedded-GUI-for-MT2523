
BOARD_EINT_KEY_SRC = driver/board/mt76x7_hdk
COMPONENT_EINT_KEY_SRC = driver/board/component

#eint key driver source
C_FILES  += $(COMPONENT_EINT_KEY_SRC)/eint_key/src/eint_key.c
C_FILES  += $(BOARD_EINT_KEY_SRC)/eint_key/src/eint_key_custom.c

#################################################################################
#include path
CFLAGS 	+= -Iinclude
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7687/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt7687/src/common/include

#eint key driver include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/eint_key/inc

#board confg include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/eint_key/inc
