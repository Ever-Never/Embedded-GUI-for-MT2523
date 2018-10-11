
BOARD_SRC = driver/board/mt25x3_hdk
COMPONENT_SRC = driver/board/component

#eint key driver source
C_FILES  += $(COMPONENT_SRC)/eint_key/src/eint_key.c
C_FILES  += $(BOARD_SRC)/eint_key/src/eint_key_custom.c

#################################################################################
#include path
CFLAGS 	+= -Iinclude
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc

#eint key driver include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/eint_key/inc

#board confg include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/eint_key/inc

CFLAGS  += -DMTK_EINT_KEY_ENABLE

