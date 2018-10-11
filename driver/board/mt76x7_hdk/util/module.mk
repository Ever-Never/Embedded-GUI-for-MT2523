
UTIL_SRC = driver/board/mt76x7_hdk/util

ifeq ($(MTK_MINICLI_ENABLE),y)
C_FILES  += $(UTIL_SRC)/src/board_cli.c
C_FILES  += $(UTIL_SRC)/src/gpio_cli.c
endif

#
# The critical API functions in io_def.c are declared as weak symbols and can be
# overridden by other implementations. Hence, the files is not enclosed by the
# condition MTK_MINICLI_ENABLE=y.
# 
C_FILES  += $(UTIL_SRC)/src/io_def.c

CFLAGS   += -Iinc
CFLAGS   += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/util/inc

