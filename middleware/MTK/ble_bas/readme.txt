ble_bas module usage guide

Brief:          This module is the Battery Service (BAS) implementation. BAS exposes the Battery State and Battery Level to the peer device by a Bluetooth 
                low-energy link. This module supports Battery Level notification to the peer device and manages Battery Level related read events from the 
                peer device.
Usage:          GCC: 1. Include the module with "include $(SOURCE_DIR)/middleware/MTK/ble_bas/module.mk" in the GCC project Makefile
                KEIL: 1. Drag the middleware/MTK/ble_bas folder to the project.
                      2. Add middleware/MTK/ble_bas/inc to include paths.
                IAR: 1. Drag the middleware/MTK/ble_bas folder to the project.
                     2. Add middleware/MTK/ble_bas/inc to include paths.
Dependency:     This module depends on Bluetooth and Battery Management module. Please set MTK_BT_ENABLE and MTK_SMART_BATTERY_ENABLE to be "y" in feature.mk and
                include Bluetooth module.mk and battery_management module.mk in your project.
Notice:         None.
Relative doc:   None.
Example project:project/mt2523_hdk/apps/bt_headset_23alpha/ble_bas_app.


