Bluetooth callback manager module usage guide

Brief:           This module is to manage the callback function of the Bluetooth stack. User can register and deregister the 
                 EDR/BLE callback function to handle different callback functions. It supports EDR/BLE stack on 
                 MT2523x and BLE only stack on MT7697x.
Usage:           GCC: 
                      1) Add the following module.mk for include path and source file:
                         include $(SOURCE_DIR)/middleware/MTK/bt_callback_manager/module.mk
                         in your GCC project Makefile.

                 KEIL: 
                       1) Add all source files under the path "middleware/MTK/bt_callback_manager/src" to your
                          project.
                       2) Add the path "..\..\..\..\..\middleware\MTK\bt_callback_manager\inc" to the "include paths" of the C/C++ options.

                 IAR: 
                      1) Add all source files under the path "middleware/MTK/bt_callback_manager/src" to your project. 
                      2) Add the path "$PROJ_DIR$\..\..\..\..\..\middleware\MTK\bt_callback_manager\inc" to the "Additional include directories" of the preprocessor.

Dependency:      This module has the dependency with Bluetooth. Please also make sure to include Bluetooth.
                 MTK_BT_ENABLE must set to 'y' in your project.
Notice:          None.
Relative doc:    None.
Example project: Please find the projects under project/mt2523_hdk/apps folder with bt_ or ble_ prefix.
