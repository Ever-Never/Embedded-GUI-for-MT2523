
EPT_SRC = driver/board/mt25x3_hdk

C_FILES  += $(EPT_SRC)/ept/src/bsp_gpio_ept_config.c

#################################################################################
#include path
CFLAGS 	+= -Iinclude
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/ept/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include


