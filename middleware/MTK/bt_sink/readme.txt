BT sink module usage guide

Brief:          This module is the sink service which integrates HFP, A2DP, AVRCP and PBAPC profiles.
                It works as a Bluetooth headset and supports the headset features, such as, answering or rejecting incoming call,
                getting contact name of the incoming call, playing and pausing music, moving to previous song and next song, and
                connection reestablishing when power on or link lost.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/bt_sink/module.mk" in your GCC project makefile.
                KEIL: Drag the middleware/MTK/bt_sink folder to your project.
                      Add middleware/bt_sink/inc to include paths.
                IAR: Drag the middleware/MTK/bt_sink folder to your project.
                     Add middleware/MTK/bt_sink/inc to include paths.
Dependency:     This module has the dependency with Bluetooth. Please also make sure to include Bluetooth.
Relative doc:   None.
Example project: Please find the iot_sdk_demo project under project/mt2523_hdk/apps folder.
