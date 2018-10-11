
SERIAL_NAND_SRC = middleware/MTK/serial_nand

C_FILES  += $(SERIAL_NAND_SRC)/src/NAND_DAL.c
C_FILES  += $(SERIAL_NAND_SRC)/src/NAND_DevConfig.c
C_FILES  += $(SERIAL_NAND_SRC)/src/NAND_MTD_FDM50.c
C_FILES  += $(SERIAL_NAND_SRC)/src/port/spi_serial_nand_flash.c
C_FILES  += $(SERIAL_NAND_SRC)/src/spi_nand_fdm.c
C_FILES  += $(SERIAL_NAND_SRC)/src/fdm_port.c

#################################################################################
#include path
CFLAGS 	+= -Iinc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/serial_nand/inc

ifneq ($(wildcard $(strip $(SOURCE_DIR))/middleware/MTK/serial_nand_protected/),)
include $(SOURCE_DIR)/middleware/MTK/serial_nand_protected/src_protected/GCC/module.mk
else
include $(SOURCE_DIR)/prebuilt/middleware/MTK/serial_nand/module.mk
endif

#Enable the feature by configuring
CFLAGS += -DMTK_FATFS_ON_SERIAL_NAND