Bluetooth module usage guide

Brief:          This module is the bluetooth stack implementation. It supports EDR/BLE stack on MT2523x and BLE only stack on MT7697x.
Usage:          GCC:  For EDR/BLE stack, include the module with 
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/bluetooth/module.mk
                      2) Module.mk provide different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         MTK_BT_HFP_ENABLE
                         MTK_BT_AVRCP_ENABLE
                         MTK_BT_AVRCP_ENH_ENABLE
                         MTK_BT_A2DP_ENABLE
                         MTK_BT_PBAP_ENAABLE
                         MTK_BT_SPP_ENABLE
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc 
                         CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/bluetooth/inc
                      4) copy the hci_log.h to the [project]/inc folder and hci_log.c to the [project]/src folder from the project 
                         under project folder with bt_ or ble_ prefix. Add the source file:
                            APP_FILES += $(APP_PATH_SRC)/hci_log.c
                      in your GCC project Makefile.
                      
                      
                      For BLE only stack,
                      1) Add the following moddule.mk 
                         include $(SOURCE_DIR)/middleware/MTK/bluetooth/module.mk
                      2) Set MTK_BLE_ONLY_ENABLE as "y" in specified GCC/feature.mk
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc 
                         CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/bluetooth/inc
                      4) copy the hci_log.h to the [project]/inc folder and hci_log.c to the [project]/src folder from the iot_sdk project.
                         Add the source file:
                            APP_FILES += $(APP_PATH_SRC)/hci_log.c
                      in your GCC project Makefile.

                KEIL: For EDR/BLE stack, 
                      1)Add the following libs, libbt_CM4_keil.lib and libbtdriver_2523_CM4_keil.lib are mandatory, but profile libs are optional:
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_CM4_keil.lib
                        prebuilt/middleware/MTK/bluetooth/lib/libbtdriver_2523_CM4_keil.lib
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_spp_CM4_keil.lib
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_pbapc_CM4_keil.lib
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_hfp_CM4_keil.lib
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_avrcp_eh_CM4_keil.lib
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_avrcp_CM4_keil.lib
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_a2dp_CM4_keil.lib
                      2)add all source files except bt_cli.c under the path "middleware/MTK/bluetooth/src" to your project 
                      3)add the path "..\..\..\..\..\middleware\MTK\bluetooth\inc" and "..\..\..\..\..\prebuilt\middleware\MTK\bluetooth\inc" to the "include paths" of the C/C++ options.
                      4)copy the hci_log.h to the [project]/inc folder and hci_log.c to the [project]/src folder from the 
                        project folder with bt_ or ble_ prefix. 
                        Add the source file hci_log.c to your project.
                      5)Add "MBEDTLS_CONFIG_FILE=<config-mtk-md5.h>" and "__BT_DEBUG__" into the Define of the Preprocessor symbols,
                        add the path "..\..\..\..\..\middleware\third_party\mbedtls\configs" and the path 
                        "..\..\..\..\..\middleware\third_party\mbedtls\include" to the "include paths" of the C/C++ options and
                        add the source file aes.c and md5.c under the folder middleware/third_party/mbedtls/library/ to your project.
                      6)Implement the APIs below and add #include "bt_system.h", #include "bt_gap_le.h" and #include "bt_gap.h" in your source file.
                        bt_gap_le_bonding_info_t *bt_gap_le_get_bonding_info(const bt_addr_t remote_addr);
                        bt_gap_le_local_config_req_ind_t *bt_gap_le_get_local_config(void);
                        bt_status_t bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind);
                        const bt_gap_config_t* bt_gap_get_local_configuration(void);

                      For BLE only stack, 
                      1)add these two libraries to your project:
                        prebuilt/middleware/MTK/bluetooth/lib/libble_CM4_keil.lib
                        prebuilt/middleware/MTK/bluetooth/lib/libbtdriver_7697_CM4_keil.lib
                      2)add all source files under the path "middleware/MTK/bluetooth/src" to your project 
                      3)add the path "..\..\..\..\..\middleware\MTK\bluetooth\inc" and "..\..\..\..\..\prebuilt\middleware\MTK\bluetooth\inc" to the "include paths" of the C/C++ options.
                      4)copy the hci_log.h to the [project]/inc folder and hci_log.c to the [project]/src folder from the iot_sdk project.
                        Add the source file hci_log.c to your project.
                      5)Add "MBEDTLS_CONFIG_FILE=<config-mtk-md5.h>" and "__BT_DEBUG__" into the Define of the Preprocessor symbols,
                        add the path "..\..\..\..\..\middleware\third_party\mbedtls\configs" and the path 
                        "..\..\..\..\..\middleware\third_party\mbedtls\include" to the "include paths" of the C/C++ options and
                        add the source file aes.c and md5.c under the folder middleware/third_party/mbedtls/library/ to your project.
                      6)Implement the APIs below, add #include "bt_system.h" and #include "bt_gap_le.h" in your source file.
                        bt_gap_le_bonding_info_t *bt_gap_le_get_bonding_info(const bt_addr_t remote_addr);
                        bt_gap_le_local_config_req_ind_t *bt_gap_le_get_local_config(void);
                        bt_status_t bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind);

                IAR:  For EDR/BLE stack, 
                      1)Add the following libs, libbt_iar.a and libbtdriver_2523_iar.a are mandatory, but profile libs are optional:
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_iar.a
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_driver_2523_iar.a
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_spp_iar.a
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_pbapc_iar.a
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_hfp_iar.a
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_avrcp_iar.a
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_avrcp_enhance_iar.a
                        prebuilt/middleware/MTK/bluetooth/lib/libbt_a2dp_iar.a
                      2)add all source files except bt_cli.c under the path "middleware/MTK/bluetooth/src" to your project 
                      3)add the path "$PROJ_DIR$\..\..\..\..\..\middleware\MTK\bluetooth\inc" and "$PROJ_DIR$\..\..\..\..\..\prebuilt\middleware\MTK\bluetooth\inc" to the "Additional include directories" of the preprocessor.
                      4)copy the hci_log.h to the [project]/inc folder and hci_log.c to the [project]/src folder from the 
                        project folder with bt_ or ble_ prefix. 
                        Add the source file hci_log.c to your project.
                      5)Add "MBEDTLS_CONFIG_FILE=<config-mtk-md5.h>" and "__BT_DEBUG__" into the Defined symbols in the C/C++ compiler,
                        add the path "$PROJ_DIR$\..\..\..\..\..\middleware\third_party\mbedtls\configs" and the path 
                        "$PROJ_DIR$\..\..\..\..\..\middleware\third_party\mbedtls\include" to the "additional include directories" of the C/C++ compiler and
                        add the source file aes.c and md5.c under the folder middleware/third_party/mbedtls/library/ to your project.
                      6)Implement the APIs below, add #include "bt_system.h" and #include "bt_gap_le.h" in your source file.
                        bt_gap_le_bonding_info_t *bt_gap_le_get_bonding_info(const bt_addr_t remote_addr);
                        bt_gap_le_local_config_req_ind_t *bt_gap_le_get_local_config(void);
                        bt_status_t bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind);
                        const bt_gap_config_t* bt_gap_get_local_configuration(void);
                      
                      For BLE only stack, 
                      1)add these two libraries to your project:
                        prebuilt/middleware/MTK/bluetooth/lib/libble_iar.a
                        prebuilt/middleware/MTK/bluetooth/lib/libbtdriver_7697_iar.a
                      2)add all source files under the path "middleware/MTK/bluetooth/src" to your project 
                      3)add the path "$PROJ_DIR$\..\..\..\..\..\middleware\MTK\bluetooth\inc" and "$PROJ_DIR$\..\..\..\..\..\prebuilt\middleware\MTK\bluetooth\inc" to the "Additional include directories" of the preprocessor.
                      4)copy the hci_log.h to the [project]/inc folder and hci_log.c to the [project]/src folder from the iot_sdk project.
                      Add the source file hci_log.c to your project.
                      5)Add "MBEDTLS_CONFIG_FILE=<config-mtk-md5.h>" and "__BT_DEBUG__" into the Defined symbols in the C/C++ compiler,
                        add the path "$PROJ_DIR$\..\..\..\..\..\middleware\third_party\mbedtls\configs" and the path 
                        "$PROJ_DIR$\..\..\..\..\..\middleware\third_party\mbedtls\include" to the "additional include directories" of the C/C++ compiler and
                        add the source file aes.c under the folder middleware/third_party/mbedtls/library/ to your project.
                      6)Implement the APIs below, add #include "bt_system.h" and #include "bt_gap_le.h" in your source file.
                        bt_gap_le_bonding_info_t *bt_gap_le_get_bonding_info(const bt_addr_t remote_addr);
                        bt_gap_le_local_config_req_ind_t *bt_gap_le_get_local_config(void);
                        bt_status_t bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind);
                     
Dependency:     Please define HAL_DVFS_MODULE_ENABLED and HAL_AES_MODULE_ENABLED in the hal_feature_config.h under the project inc folder. 
Notice:         1) The UART port which HCI log use is assigned in the file hci_log.c. Please make sure you have been initialized the UART port in your project main function 
                   or return directly in the function hci_log() in the file hci_log.c if no need to catcher HCI log.
                2) BT trace maybe turn off defaultly afer bootup, please try "log set BT on info" command to turn it on.
Relative doc:   For EDR/BLE stack, please refer to the bluetooth developer's guide on MT2523 under the doc folder for more detail.
                For BLE only stack, please refer to the bluetooth developer's guide on mt7697x under the doc folder for more detail.
Example project:For EDR/BLE stack, please find the project under project folder with bt_ or ble_ prefix.
                For BLE only stack, please find the iot_sdk project under the project folder.
