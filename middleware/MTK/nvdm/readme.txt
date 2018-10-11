NVDM module usage guide

Brief:          This module is the non-volatile data management implementation.
Usage:          GCC: Please add "include $(SOURCE_DIR)/middleware/MTK/nvdm/module.mk" in your GCC project Makefile.
                KEIL: Please drag the prebuilt/middleware/MTK/nvdm/lib/libnvdm_xxx_KEIL.lib and middleware/MTK/nvdm/src folder to your project. Please define the macro PRODUCT_VERSION and MTK_NVDM_ENABLE in KEIL, and add middleware/MTK/nvdm/inc to include paths.
                IAR: Please drag the prebuilt/middleware/MTK/nvdm/lib/libnvdm_xxx_IAR.a and middleware/MTK/nvdm/src folder to your project. Please define the macro PRODUCT_VERSION and MTK_NVDM_ENABLE in IAR, and add middleware/MTK/nvdm/inc to include paths.
Dependency:     Please define HAL_FLASH_MODULE_ENABLED in hal_feature_config.h under the project inc folder.
Notice:         To customize the nvdm setting, please configure nvdm_port.c in middleware/MTK/nvdm/src folder.
                The users also should include their code with option "MTK_NVDM_ENABLE".
Relative doc:   Please refer to the section of NVDM in LinkIt SDK for xxxx API Reference Manual.html under the doc folder for more detail.
Example project:none.
