BT notify module usage guide

Brief:          This module is a Bluetooth service.
                It can receive the notification from the APK by BT SPP and BLE GATT connection.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/bt_notify/module.mk" in your GCC project Makefile.
                KEIL: Drag the middleware/MTK/bt_notify folder to your project.
                      Include the middleware/MTK/bt_notify/lib/libbtnotify_Keil.lib in your project. 
                      Add middleware/bt_notify/inc and middleware/bt_notify/inc/dogp/ to include paths.
                IAR: Drag the middleware/MTK/bt_notify folder to your project.
                     Include the middleware/MTK/bt_notify/lib/libbtnotify_IAR.a in your project. 
                     Add middleware/MTK/bt_notify/inc and middleware/MTK/bt_notify/inc/dogp to include paths.
Dependency:     This module has the dependency with Bluetooth. Please also make sure to include Bluetooth.
                MTK_BT_ENABLE,MTK_BT_SPP_ENABLE must set to 'y' in your project.
Notice:         Get the APK in SDK packet in tools/smart_device and install it on Android Smartphone.
Relative doc:   None.
Example project:Please find the project under project folder bt_android_notification.

