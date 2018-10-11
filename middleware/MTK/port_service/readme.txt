Port service module usage guide

Brief:          This module is the port service implementation. It bases on different serial device ports and offers user with unified interface to use.
Usage:          GCC:  Please add "MTK_PORT_SERVICE_ENABLE = y" in feature.mk under the project GCC folder.
                      Please add "MTK_USB_DEMO_ENABLED = y" in feature.mk under the project GCC folder if USB support is needed.
                      Please add "APP_FILES += $(APP_PATH_SRC)/at_command_serial_port.c" and wrap them with "ifeq ($(MTK_PORT_SERVICE_ENABLE), y)" and "endif" in Makefile under the project GCC folder if AT command for port service is used.
                      Please add "include $(SOURCE_DIR)/middleware/MTK/port_service/module.mk" and wrap them with "ifeq ($(MTK_PORT_SERVICE_ENABLE), y)" and "endif" in Makefile under the project GCC folder.
                      Please add "include $(SOURCE_DIR)/middleware/MTK/usb/module.mk" and wrap them with "ifeq ($(MTK_USB_DEMO_ENABLED), y)" and "endif" in Makefile under the project GCC folder.
                KEIL: Please drag the middleware/MTK/port_service/src and middleware/MTK/usb/src folder to your project.
                      Please drag $(APP_PATH_SRC)/at_command_serial_port.c to your project if AT command for port service is used.
                      Please define the MTK_PORT_SERVICE_ENABLE macro in KEIL.
                      Please define the MTK_USB_DEMO_ENABLED macro in KEIL if USB support is needed.
                      Please add middleware/MTK/port_service/inc to include paths.
                      Please add middleware/MTK/usb/inc to include paths if USB support is needed.
                IAR:  Please drag the middleware/MTK/port_service/src and middleware/MTK/usb/src folder to your project.
                      Please drag $(APP_PATH_SRC)/at_command_serial_port.c to your project if AT command for port service is used.
                      Please define the MTK_PORT_SERVICE_ENABLE macro in IAR.
                      Please define the MTK_USB_DEMO_ENABLED macro in IAR if USB support is needed.
                      Please add middleware/MTK/port_service/inc to include paths.
                      Please add middleware/MTK/usb/inc to include paths if USB support is needed.
Dependency:     Please define HAL_UART_MODULE_ENABLED in hal_feature_config.h under the project inc folder.
Notice:         None.
Relative doc:   Please refer to the "Port Service Develop Guide" document from ACS for more detail.
Example project:none.
