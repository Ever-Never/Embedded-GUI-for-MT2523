ble_ancs module usage guide

Brief:          This module is the ble_ancs middleware implementation. Apple Notification Center Service (ANCS) is a way to access the notifications 
                generated on iOS devices by a Bluetooth low-energy link. It can be connected with iOS devices only.
Usage:          GCC: 1. Include the module with "include $(SOURCE_DIR)/middleware/MTK/ble_ancs/module.mk" in the GCC project Makefile
                KEIL: 1. Drag the middleware/MTK/ble_ancs folder to the project.
                      2. Add middleware/MTK/ble_ancs/inc to include paths.
                IAR: 1. Drag the middleware/MTK/ble_ancs folder to the project.
                     2. Add middleware/MTK/ble_ancs/inc to include paths.
Dependency:     This module depends on Bluetooth. Please set MTK_BT_ENABLE to be "y" in feature.mk and include Bluetooth module.mk in your project.
Notice:         None.
Relative doc:   None.
Example project:project/mt2523_hdk/apps/ble_ancs_ios_notification.


