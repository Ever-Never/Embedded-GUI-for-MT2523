IC_CONFIG                             = mt2523
BOARD_CONFIG                             = mt2523_hdk

# combo sensor of accelerometer and gyroscope
#MTK_SENSOR_ACCELEROMETER_USE = BMI160
#MTK_SENSOR_MAGNETIC_USE = YAS533
#MTK_SENSOR_BAROMETER_USE = BMP280
#MTK_SENSOR_PROXIMITY_USE = CM36672
MTK_SENSOR_BIO_USE = MT2511
MTK_SENSOR_ACCELEROMETER_USE = BMA255
MT2511_E1 = y
MT2511_INTERFACE = SPI
HR_PPG_128HZ = y

# fusion algorithm
#FUSION_PEDOMETER_USE = M_INHOUSE_PEDOMETER
#FUSION_SLEEP_TRACKER_USE = M_INHOUSE_SLEEP_TRACKER
FUSION_HEART_RATE_MONITOR_USE = M_INHOUSE_HEART_RATE_MONITOR
# Note HRV and B.P. are "exclusive"!!!
#FUSION_HEART_RATE_VARIABILITY_USE = M_INHOUSE_HEART_RATE_VARIABILITY
FUSION_BLOOD_PRESSURE_USE = M_INHOUSE_BLOOD_PRESSURE

# gnss
MTK_GNSS_ENABLE = y

# battery_management
MTK_SMART_BATTERY_ENABLE = y

# bt at command
MTK_BT_AT_COMMAND_ENABLE = n

# bt module enable
MTK_BT_ENABLE                       = y
MTK_BLE_ONLY_ENABLE                 = n
MTK_BT_HFP_ENABLE                   = y
MTK_BT_AVRCP_ENABLE                 = y
MTK_BT_AVRCP_ENH_ENABLE             = y
MTK_BT_A2DP_ENABLE                  = y
MTK_BT_PBAP_ENABLE                  = y
MTK_BT_SPP_ENABLE                   = y

# bt codec enable
MTK_BT_CODEC_ENABLED = y


# CTP module on
MTK_CTP_ENABLE = y
# debug level: none, error, warning, and info
MTK_DEBUG_LEVEL = info

# BT A2DP codec AAC support
MTK_BT_A2DP_AAC_ENABLE = y
MTK_BT_DUO_ENABLE = y

# MVG flash test support
MTK_MVG_FLASH_TEST_ENABLE = n

MTK_SENSOR_AT_COMMAND_ENABLE = y

# Support MTK audio tuning tool
MTK_AUDIO_TUNING_ENABLED = n

# USB
MTK_USB_DEMO_ENABLED = y

# System service debug feature for internal use
MTK_SUPPORT_HEAP_DEBUG              = n
MTK_HEAP_SIZE_GUARD_ENABLE          = n

# Support CPU utilization through at command
MTK_OS_CPU_UTILIZATION_ENABLE = y

# Support system at command
MTK_SYSTEM_AT_COMMAND_ENABLE = y

# Development board type: EVB, HDK
MTK_DEVELOPMENT_BOARD_TYPE = HDK


#SDK version query cmd
MTK_QUERY_SDK_VERSION = y

#LED support
MTK_LED_ENABLE = n


# port service
MTK_PORT_SERVICE_ENABLE = y

# atci via port service
MTK_ATCI_VIA_PORT_SERVICE = y

#MD5 configration
MTK_MBEDTLS_CONFIG_FILE             = config-mtk-md5.h

MTK_NVDM_ENABLE           = y

__BT_ENABLE_BA__   =  n

#Enable demo project delete all task at cmd
MTK_DEMO_PROJ_LOWPOWER_AT_CMD_ENABLE = y

# TE enable, enable this feature will avoid tearing effect
MTK_TE_ENABLE = y