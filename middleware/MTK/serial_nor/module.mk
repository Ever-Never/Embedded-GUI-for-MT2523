
SERIAL_NOR_SRC = middleware/MTK/serial_nor

C_FILES  += $(SERIAL_NOR_SRC)/src/serial_nor_flash.c

#################################################################################
#include path
CFLAGS += -Iinc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/serial_nor/inc

#Enable the feature by configuring
CFLAGS += -DSERIAL_NOR_WITH_SPI_SUPPORT