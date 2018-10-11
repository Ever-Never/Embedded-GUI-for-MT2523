Serial NOR module usage guide

Brief:          This module is for access the serial NOR Flash by SPI interface.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/serial_nor/module.mk" in your GCC project Makefile.
                KEIL: Drag the middleware/MTK/serial_nor folder to your project. Add middleware/MTK/serial_nor/inc to include paths.
                IAR: Drag the middleware/MTK/serial_nor folder to your project. Add middleware/MTK/serial_nor/inc to include paths.
Dependency:     Please define HAL_SPI_MASTER_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.  
Notice:         When porting to a different serial NOr flash device, please implement all functions in the serial_nor_flash.h;
                Advice to configure the GPIO and Pinmux with EPT Tool, otherwise configure it in serial_nor_init.   
Relative doc:   Please refer to the section of serial_nor in LinkIt SDK for xxxx API Reference Manual.html under the doc folder for more detail.
Example project:none.