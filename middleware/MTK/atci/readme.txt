ATCI module usage guide

Brief:          This module is the AT command interface.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/atci/module.mk" in your GCC project Makefile.
                KEIL: Drag the middleware/MTK/atci folder to your project. Add middleware/MTK/atci/inc to include paths.
                IAR: Drag the middleware/MTK/atci folder to your project. Add middleware/MTK/atci/inc to include paths.
Dependency:     Please define HAL_UART_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.  
Notice:         To disable the Mediatek default AT command provided under middleware/MTK/atci/at_command folder, please define MTK_AT_CMD_DISABLE.  
Relative doc:   Please refer to the API reference under the doc folder for more detail.
Example project:Please find the project under project folder with atci_ prefix.
