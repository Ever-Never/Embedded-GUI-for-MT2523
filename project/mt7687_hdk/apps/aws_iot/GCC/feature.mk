IC_CONFIG                           = mt7687
BOARD_CONFIG                        = mt7687_hdk

MTK_HAL_LOWPOWER_ENABLE             = y
MTK_LWIP_ENABLE                     = y
MTK_BSPEXT_ENABLE                   = y
MTK_MINISUPP_ENABLE                 = y
MTK_WIFI_TGN_VERIFY_ENABLE          = y
MTK_MAIN_CONSOLE_UART2_ENABLE       = n
MTK_BSP_LOOPBACK_ENABLE             = n
MTK_OS_CPU_UTILIZATION_ENABLE       = n
MTK_MBEDTLS_CONFIG_FILE             = config-aws-iot.h
MTK_NVDM_ENABLE                     = y

########################################################
# Add for AWS IOT Feature
AWS_IOT_SUPPORT                     = y
# AWS IoT Log level define
AWS_IOT_DEBUG_ENABLE                = y
AWS_IOT_ERROR_ENABLE                = y
AWS_IOT_TRACE_ENABLE                = n
AWS_IOT_INFO_ENABLE                 = y
AWS_IOT_WARN_ENABLE                 = y
