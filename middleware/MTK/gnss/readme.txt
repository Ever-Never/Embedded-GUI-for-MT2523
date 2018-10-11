GNSS module usage guide

Brief:          This module is the GNSS middleware implementation providing GNSS APIs.
Usage:          GCC: 1. Include the module with "include $(SOURCE_DIR)/middleware/MTK/gnss/module.mk" in the GCC project Makefile
                     2. Set MTK_GNSS_ENABLE to "y" in the feature.mk in the same directory.
                KEIL: 1. Drag the middleware/MTK/gnss folder to the project and define the MTK_GNSS_ENABLE macro.
                      2. Add middleware/MTK/gnss/inc to include paths.
                IAR: 1. Drag the middleware/MTK/gnss folder to the project and define the MTK_GNSS_ENABLE macro.
                     2. Add middleware/MTK/gnss/inc to include paths.
Dependency:     None.
Notice:         For GCC, keep the copy_firmware.sh statement in the Makefile under GCC folder in a project to copy GNSS firmware image.
                For KEIL and IAR, users need to download GNSS firmware image to target device manually by flashing image prebuilt/middleware/MTK/gnss/firmware/gnss_chip_fw.bin with MediaTek Flashtool.
Relative doc:   Find the GNSS section in the 2523 API reference under the <sdk_root>/doc folder.
Example project:Please find the project under project folder with "gnss_" prefix.

