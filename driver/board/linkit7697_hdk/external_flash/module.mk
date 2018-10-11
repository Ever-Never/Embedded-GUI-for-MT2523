
EX_FLASH_SRC = driver/board/linkit7697_hdk/external_flash
C_FILES  += $(EX_FLASH_SRC)/src/bsp_flash_config.c


#################################################################################
#include path
#CFLAGS 	+= -Iinc
CFLAGS  += -I$(EX_FLASH_SRC)/driver/board/linkit7697_hdk/external_flash/inc

#################################################################################
#Enable the feature by configuration
CFLAGS += -DMTK_EXTERNAL_FLASH_ENABLE