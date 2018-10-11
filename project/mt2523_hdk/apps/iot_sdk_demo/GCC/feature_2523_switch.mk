IC_CONFIG                           = mt2523
BOARD_CONFIG                        = mt2523_hdk
MTK_FW_VERSION                      = SDK_VER_IOT_SDK_DEMO

########################################
#           Custom feature option      #
########################################

## part1: configure y/n

MTK_SYS_TRNG_ENABLE                 = n
MTK_LWIP_ENABLE                     =
MTK_WIFI_TGN_VERIFY_ENABLE          =
MTK_SUPPORT_APP_TEST_ENABLE         =
MTK_LWIP_DYNAMIC_DEBUG_ENABLE       =
MTK_LWIP_STATISTICS_ENABLE          =
MTK_OS_CPU_UTILIZATION_ENABLE       =
MTK_SYSTEM_AT_COMMAND_ENABLE        = 
MTK_HEAP_GUARD_ENABLE               =
MTK_LED_ENABLE                      = y
MTK_PING_OUT_ENABLE                 =
MTK_IPERF_ENABLE                    =
MTK_BSPEXT_ENABLE                   =
MTK_FOTA_ENABLE                     =
MTK_HAL_PLAIN_LOG_ENABLE            =
MTK_BLE_BQB_TEST_ENABLE             =
MTK_GNSS_ENABLE                     = y
MTK_BT_AT_COMMAND_ENABLE            = y
MTK_BT_NORMAL_MODE_ENABLE           =
MTK_CTP_ENABLE                      = y
MTK_SMART_BATTERY_ENABLE            = y
MTK_FATFS_ENABLE                    = n
MTK_USB_DEMO_ENABLED                =
MTK_BT_A2DP_AAC_ENABLE              = y
MTK_AUDIO_TUNING_ENABLE             = n
MTK_XOSC_MODE                       =


## part2: need to set specified value

MTK_SENSOR_ACCELEROMETER_USE        = BMA255    # BMA255, none
MTK_SENSOR_MAGNETIC_USE             = none    # YAS533, none
MTK_SENSOR_BAROMETER_USE            = none    # BMP280, none
MTK_SENSOR_PROXIMITY_USE            = none    # CM36672, none
MTK_SENSOR_BIO_USE                  = MT2511    # MT2511, none
FUSION_PEDOMETER_USE                = none    # M_INHOUSE_PEDOMETER, none
FUSION_SLEEP_TRACKER_USE            = none    # M_INHOUSE_SLEEP_TRACKER, none
FUSION_HEART_RATE_MONITOR_USE       = M_INHOUSE_HEART_RATE_MONITOR    # M_INHOUSE_HEART_RATE_MONITOR, none
FUSION_HEART_RATE_VARIABILITY_USE   = M_INHOUSE_HEART_RATE_VARIABILITY    # M_INHOUSE_HEART_RATE_VARIABILITY, none
FUSION_BLOOP_PRESSURE_USE           = none    # M_INHOUSE_BLOOD_PRESSURE, none
MTK_DEBUG_LEVEL                     = info    # info, warning, error, none
MTK_MBEDTLS_CONFIG_FILE             =         # 2523 project don't include Lwip, no need pay attention to verify it.

########################################
#           Internal use               #
########################################
MTK_MAIN_CONSOLE_UART2_ENABLE       = n
MTK_HEAP_SIZE_GUARD_ENABLE          = n
MTK_MET_TRACE_ENABLE                = n
MTK_CODE_COVERAGE_ENABLE            = n
MTK_MVG_FLASH_TEST_ENABLE           = n
MTK_SUPPORT_SFC_DVT_ENABLE          = n
MTK_DEVELOPMENT_BOARD_TYPE          = HDK
MTK_SYSLOG_DMA                      = n
MTK_NO_PSRAM_ENABLE                 = n
MTK_BLE_ONLY_ENABLE                 = n

