BT sink module usage guide

Brief:          This module is the source service which integrates A2DP and AVRCP profiles.
                It works as a audio player, such as, playing and pausing music, moving to previous song and next song, and
                connection reestablishing when power on or link lost.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/bt_source/module.mk" in your GCC project makefile.
                KEIL: Drag the middleware/MTK/bt_source folder to your project.
                      Add middleware/bt_source/inc to include paths.
                IAR: Drag the middleware/MTK/bt_source folder to your project.
                     Add middleware/MTK/bt_source/inc to include paths.
Dependency:     This module has the dependency with Bluetooth. Please also make sure to include Bluetooth.
Relative doc:   None.
Example project: Please find the iot_sdk_demo project under project/mt2523_hdk/apps folder.
