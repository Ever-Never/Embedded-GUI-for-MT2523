
SPI_EXTERNAL_FLASH_SRC = driver/board/mt25x3_hdk/spi_external_flash

C_FILES  += $(SPI_EXTERNAL_FLASH_SRC)/src/spi_external_flash.c

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/spi_external_flash/inc
