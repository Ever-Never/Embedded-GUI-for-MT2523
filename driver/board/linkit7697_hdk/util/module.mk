
UTIL_SRC = driver/board/linkit7697_hdk/util

ifeq ($(MTK_MINICLI_ENABLE),y)
C_FILES  += $(UTIL_SRC)/src/board_cli.c
C_FILES  += $(UTIL_SRC)/src/gpio_cli.c
endif
#C_FILES  += $(UTIL_SRC)/src/io_def.c

#################################################################################
#include path
CFLAGS 	+= -Iinc
CFLAGS  += -I$(SOURCE_DIR)/driver/board/linkit7697_hdk/util/inc

