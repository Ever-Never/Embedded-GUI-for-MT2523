FOTA module usage guide

Brief:          This module is the firmware update implementation.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/fota/module.mk" in your GCC project Makefile and set MTK_FOTA_ENABLE to "y" in feature.mk.
                KEIL: Drag the middleware/MTK/fota folder to your project and define the MTK_FOTA_ENABLE macro. Add middleware/MTK/fota/inc to INCLUDE_PATH.
                IAR: Drag the middleware/MTK/fota folder to your project and define the MTK_FOTA_ENABLE macro. Add middleware/MTK/fota/inc to "additional include directories" in IAR options setting.
Dependency:     HAL_FLASH_MODULE_ENABLED and HAL_WDT_MODULE_ENABLED must also defined in the hal_feature_config.h  under the project inc folder. If MTK_FOTA_CLI_ENABLE defined in the module.mk is "y", please also include the middleware/MTK/tftp/module.mk for tftp client module and middleware/third_party/httpclient for http client module.
Notice:         Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/gcc/full_bin_fota/flash.ld for gcc build environment. 
                Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/rvct/full_bin_fota/flash.sct for KEIL build environment. 
Relative doc:   Please refer to the firmware update developer guide under the doc folder for more detail.
Example project:Please find the project under project folder with fota_ prefix. 
