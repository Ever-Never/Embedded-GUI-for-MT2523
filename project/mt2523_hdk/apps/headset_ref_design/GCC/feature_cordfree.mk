IC_CONFIG                           = mt2523
BOARD_CONFIG                        = mt2523_hdk

# No PSRAM, shrink memory 
MTK_NO_PSRAM_ENABLE                   = n

# battery_management
MTK_SMART_BATTERY_ENABLE = y

# bt at command
MTK_BT_AT_COMMAND_ENABLE = y

# bt codec enable
MTK_BT_CODEC_ENABLED = y

# CTP module on
MTK_CTP_ENABLE = n

# debug level: none, error, warning, and info
MTK_DEBUG_LEVEL = info

# BT A2DP codec AAC support
MTK_BT_A2DP_AAC_ENABLE = y
MTK_BT_DUO_ENABLE = y

# bt module enable
MTK_BT_ENABLE                       = y
MTK_BLE_ONLY_ENABLE                 = n
MTK_BT_HFP_ENABLE                   = y
MTK_BT_AVRCP_ENABLE                 = y
MTK_BT_AVRCP_ENH_ENABLE             = y
MTK_BT_A2DP_ENABLE                  = y
MTK_BT_PBAP_ENABLE                  = y
MTK_BT_SPP_ENABLE                   = y
MTK_BT_AWS_ENABLE                   = y
# Support MTK audio tuning tool
MTK_AUDIO_TUNING_ENABLED = n

# System service debug feature for internal use
MTK_SUPPORT_HEAP_DEBUG              = n
MTK_HEAP_SIZE_GUARD_ENABLE          = n

# Support CPU utilization through at command
MTK_OS_CPU_UTILIZATION_ENABLE = y

# Support system at command
MTK_SYSTEM_AT_COMMAND_ENABLE = y

# Development board type: EVB, HDK
MTK_DEVELOPMENT_BOARD_TYPE = HDK

# SDK version
MTK_FW_VERSION                      = SDK_VER_BT_HEADSET_DEMO

#SDK version query cmd
MTK_QUERY_SDK_VERSION = y

#LED support
MTK_LED_ENABLE = y

# USB
MTK_USB_DEMO_ENABLED = y

# port service
MTK_PORT_SERVICE_ENABLE = y

# atci via port service
MTK_ATCI_VIA_PORT_SERVICE = y

#MD5 configration
MTK_MBEDTLS_CONFIG_FILE             = config-mtk-md5.h

MTK_NVDM_ENABLE          = y

# FOTA option
MTK_FOTA_ENABLE = y

# RTC XOSC mode select
MTK_XOSC_MODE = XOSC

#mp3 support
MTK_AUDIO_MP3_ENABLED = y

#Enable demo project delete all task at cmd
MTK_DEMO_PROJ_LOWPOWER_AT_CMD_ENABLE = y

# Audio pass through support
MTK_AUDIO_PASS_THROUGH_ENABLED = y

# prompt sound
MTK_PROMPT_SOUND_ENABLE = n

#wav support
MTK_AUDIO_WAV_ENABLED = n

# mp3 is on flash
MTK_MP3_ON_FLASH_ENABLE = y

# Enable the modification for cord-free headset
MTK_CORD_FREE_HEADSET_ENABLE = y

#BT external timer
MTK_BT_TIMER_EXTERNAL_ENABLE = y