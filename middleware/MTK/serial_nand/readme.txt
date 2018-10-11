Serial NAND module usage guide

Brief:          This module is the flash disk management implementation specialized for the serial NAND flash device.
Usage:          GCC: Please add "include $(SOURCE_DIR)/middleware/MTK/serial_nand/module.mk" in your GCC project Makefile.
                KEIL: Please drag the prebuilt/middleware/MTK/serial_nand/lib/lib_serial_nand_xxx_KEIL.lib and middleware/MTK/serial_nand/src folder to your project. Please define the macro MTK_FATFS_ON_SERIAL_NAND in KEIL, and add middleware/MTK/serial_nand/inc to include paths.
                IAR: Please drag the prebuilt/middleware/MTK/serial_nand/lib/lib_serial_nand_xxx_IAR.a and middleware/MTK/serial_nand/src folder to your project. Please define the macro MTK_FATFS_ON_SERIAL_NAND in IAR, and add middleware/MTK/serial_nand/inc to include paths.
Dependency:     Please define HAL_SPI_MASTER_MODULE_ENABLED and HAL_TRNG_MODULE_ENABLED in hal_feature_config.h under the project inc folder.
                Besides OS need also be enabled to use this feature.
                Please should do SPI and FDM initiation by serial_nand_init and spi_nand_fdm_DAL_init before do File System operation, it's better to do FMD initiation at task level.
Notice:         When porting to a different serial NAND flash device, please implement all functions in the serial_nand_flash.h.
Relative doc:   Please refer to the section of serial_nand in LinkIt SDK for xxxx API Reference Manual.html under the doc folder for more detail.
Example project:none.
