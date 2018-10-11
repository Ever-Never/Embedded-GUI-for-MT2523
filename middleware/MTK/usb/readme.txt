USB module usage guide

Brief:          This module is the USB class interface.
Usage:          GCC:  Include the module with "include $(SOURCE_DIR)/middleware/MTK/usb/module.mk" in your GCC project Makefile.
                      Please add "MTK_USB_DEMO_ENABLED = y" in feature.mk under the project GCC folder if USB support is needed.
                      Please add "include $(SOURCE_DIR)/middleware/MTK/usb/module.mk" and wrap them with "ifeq ($(MTK_USB_DEMO_ENABLED), y)" and "endif" in Makefile under the project GCC folder.
                KEIL: Drag the middleware/MTK/usb folder to your project.                       
                      Please define the MTK_USB_DEMO_ENABLED macro in KEIL if USB support is needed.
                      Add middleware/MTK/usb/inc to include paths.                
                IAR:  Drag the middleware/MTK/usb folder to your project. Add middleware/MTK/usb/inc to include paths.
                      Please define the MTK_USB_DEMO_ENABLED macro in IAR if USB support is needed.
                      Please add middleware/MTK/usb/inc to include paths if USB support is needed.
Dependency:     Please define HAL_USB_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.  
Notice:         CDC-ACM class was enabled in default. you can add other class in module.mk, for example USB_MASS_STORAGE_ENABLE
Relative doc:   Please refer to the API reference under the doc folder for more detail.
Example project:CDC-ACM class:
                      Please find the project iot_sdk_demo under project folder of mt2523_hdk.
                      Please find the project bt_headset_gui_ref_design under project folder of mt2533_evb.
                Mass Storage class:
                      Please find the project watch_demo under project folder of mt2523_watch.
