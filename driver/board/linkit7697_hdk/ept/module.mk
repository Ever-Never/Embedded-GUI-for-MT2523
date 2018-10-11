
EPT_SRC = driver/board/linkit7697_hdk/ept


C_FILES  += $(EPT_SRC)/src/bsp_gpio_ept_config.c


#################################################################################
#include path
#CFLAGS 	+= -Iinc
CFLAGS  += -I$(SOURCE_DIR)/driver/board/linkit7697_hdk/ept/inc

