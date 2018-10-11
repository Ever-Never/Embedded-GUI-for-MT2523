
EPT_SRC = driver/board/mt76x7_hdk/ept


C_FILES  += $(EPT_SRC)/src/bsp_gpio_ept_config.c


#################################################################################
#include path
#CFLAGS 	+= -Iinc
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/ept/inc

