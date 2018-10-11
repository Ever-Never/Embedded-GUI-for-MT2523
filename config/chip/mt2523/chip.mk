PRODUCT_VERSION=2523

# Default common setting
MTK_BUILD_SMT_LOAD ?= n

AR      = $(BINPATH)/arm-none-eabi-ar
CC      = $(BINPATH)/arm-none-eabi-gcc
CXX     = $(BINPATH)/arm-none-eabi-g++
OBJCOPY = $(BINPATH)/arm-none-eabi-objcopy
SIZE    = $(BINPATH)/arm-none-eabi-size
OBJDUMP = $(BINPATH)/arm-none-eabi-objdump


ALLFLAGS = -mlittle-endian -mthumb -mcpu=cortex-m4
FPUFLAGS = -Wdouble-promotion -mfpu=fpv4-sp-d16 -mfloat-abi=hard

#CFLAGS += $(ALLFLAGS) $(FPUFLAGS) -flto -ffunction-sections -fdata-sections -fno-builtin
CFLAGS += $(ALLFLAGS) $(FPUFLAGS) -ffunction-sections -fdata-sections -fno-builtin
CFLAGS += -gdwarf-2 -Os -fno-strict-aliasing -fno-common
CFLAGS += -Wimplicit-function-declaration -Wall -Werror=uninitialized -Wno-error=maybe-uninitialized -Werror=return-type
CFLAGS += -std=gnu99 -Wno-switch
CFLAGS += -DPCFG_OS=2 -D_REENT_SMALL
CFLAGS += -DPRODUCT_VERSION=$(PRODUCT_VERSION)

## MTK_GNSS_ENABLE
## Brief:       This option is to enable and disable GNSS.
## Usage:       To enable GNSS support in a project, use "include $(SOURCE_DIR)/middleware/MTK/gnss/module.mk" to include the middleware.
##              Additionally, switch on this option to make build script to copy GNSS firmware image.
## Path:        middleware/MTK/gnss
## Dependency:  Add "include $(SOURCE_DIR)/middleware/MTK/gnss/module.mk" in the Makefile to link GNSS middleware.
## Notice:      Keep the copy_firmware.sh statement in Makefile to copy GNSS firmware image.
## Relative doc:Find the GNSS section in API reference.
##
ifeq ($(MTK_GNSS_ENABLE),y)
CFLAGS         += -DMTK_GNSS_ENABLE
export MTK_GNSS_ENABLE=y
endif

##
## MTK_BT_AT_COMMAND_ENABLE
## Brief:       This option is to enable and disable BT AT commands.
## Usage:       If the value is "y", the MTK_BT_AT_COMMAND_ENABLE compile option will be defined. You must include some module in your Makefile before setting.
##              the option to "y"
##              include the /middleware/MTK/atci/module.mk
##              include the /middleware/MTK/bluetooth/module.mk
##              include the /middleware/MTK/bluetooth/lib/libbt.a
## Path:        middleware/MTK/atci/inc
## Dependency:  Add "include /middleware/MTK/bluetooth/module.mk" in the Makefile.
##
ifeq ($(MTK_BT_AT_COMMAND_ENABLE),y)
CFLAGS         += -DMTK_BT_AT_COMMAND_ENABLE
endif

##
## MTK_BUILD_IME_LOAD
## Brief:       This option is to enable i2s some code for project=bt_ime_command.
## Usage:       enable i2s loopback.
## Path:        project\mt2533_evb\test_loads\bt_ime_command
## Dependency:  .
## Notice:      .
## Relative doc:None
##
ifeq ($(MTK_BUILD_IME_LOAD),y)
CFLAGS         += -DMTK_BUILD_IME_LOAD
endif

##
## MTK_USB_DEMO_ENABLED
## Brief:       This option is to enable and disable USB module.
## Usage:       If the value is "y", the MTK_USB_DEMO_ENABLED compile option will be defined. You must include following module in your Makefile before setting.
##              the option to "y"
##              include the /middleware/MTK/usb/module.mk
## Path:        middleware/MTK/usb
## Dependency:  HAL_USB_MODULE_ENABLED must also defined in hal_feature_config.h under project inc folder.
##
ifeq ($(MTK_USB_DEMO_ENABLED),y)
CFLAGS         += -DMTK_USB_DEMO_ENABLED
endif

##
## MTK_USB_BOOT_ACM
## Brief:       This option is to enable and disable USB CDC-ACM class in booting.
## Path:        middleware/MTK/usb
## Dependency:  Must enable HAL_USB_MODULE_ENABLED, , and MTK_USB_DEMO_ENABLED
##
ifeq ($(MTK_USB_BOOT_ACM),y)
CFLAGS         += -DMTK_USB_BOOT_ACM
endif

##
## MTK_USB_BOOT_MSC
## Brief:       This option is to enable and disable USB MASS-STORAGE class in booting.
## Path:        middleware/MTK/usb
## Dependency:  Must enable HAL_USB_MODULE_ENABLED, , and MTK_USB_DEMO_ENABLED
##
ifeq ($(MTK_USB_BOOT_MSC),y)
CFLAGS         += -DMTK_USB_BOOT_MSC
endif

##
## MTK_MINICLI_ENABLE
## Brief:       This option is to enable and disable CLI (command line interface) engine.
## Usage:       If the value is "y", the MTK_MINICLI_ENABLE compile option will be defined. You must also include the gva3\middleware\MTK\minicli\module.mk in your Makefile before setting the option to "y".
## Path:        middleware/MTK/minicli
## Dependency:  HAL_UART_MODULE_ENABLED must also defined in hal_feature_config.h under project inc folder.
## Notice:      N/A
## Related doc: N/A
##
ifeq ($(MTK_MINICLI_ENABLE),y)
CFLAGS         += -DMTK_MINICLI_ENABLE
endif

##
## MTK_FOTA_ENABLE
## Brief:       This option is to enable and disable FOTA.
## Usage:       If the value is "y", the MTK_FOTA_ENABLE compile option will be defined. You must also include the \middleware\MTK\fota\module.mk in your Makefile before setting the option to "y". 
## Path:        middleware/MTK/fota
## Dependency:  HAL_FLASH_MODULE_ENABLED and HAL_WDT_MODULE_ENABLED must also defined in the hal_feature_config.h  under the project inc folder. If MTK_FOTA_CLI_ENABLE defined in the module.mk is "y", please also include the middleware/MTK/tftp/module.mk. 
## Notice:      Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/gcc/full_bin_fota/flash.ld for gcc build environment. 
##              Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/rvct/full_bin_fota/flash.sct for KEIL build environment. 
##              Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/iar/full_bin_fota/flash.icf for IAR build environment.
## Relative doc:Please refer to the firmware update developer's guide under the doc folder for more detail.
##
#ifeq ($(MTK_FOTA_ENABLE),y)
#CFLAGS += -DMTK_FOTA_ENABLE
#endif

##
## MTK_OS_CPU_UTILIZATION_ENABLE
## Brief:       This option is to enable and disable cpu utilization function.
## Usage:     If the value is "y", the MTK_OS_CPU_UTILIZATION_ENABLE compile option will be defined and supports the 'os cpu' MTK CLI commands to show/get statistics of CM4 CPU utilizations of all the tasks running on.You must also include the \kernel\service\module.mk in your Makefile before setting the option to "y". 
## Path:       kernel/service
## Dependency:  MTK_MINICLI_ENABLE must be enabled in this file.
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_OS_CPU_UTILIZATION_ENABLE),y)
CFLAGS += -DMTK_OS_CPU_UTILIZATION_ENABLE
endif

##
## MTK_SYSTEM_AT_COMMAND_ENABLE
## Brief:       This option is to enable and disable at commands for system service, include show task information, heap infomation, and force system crash to get memory dump
## Usage:     If the value is "y", the MTK_SYSTEM_AT_COMMAND_ENABLE compile option will be defined. You must also include the \kernel\service\module.mk in your Makefile before setting the option to "y". 
## Path:       kernel/service
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_SYSTEM_AT_COMMAND_ENABLE),y)
CFLAGS += -DMTK_SYSTEM_AT_COMMAND_ENABLE
endif

##
## MTK_SENSOR_ACCELEROMETER_USE
## Brief:        This option is to enable and select accelerometer driver under driver/board/component/mems folder.
## Usage:        If the value is in the supported list (e.g. BMA255), the MTK_SENSOR_ACCELEROMETER_USE and correspoding selection (e.g. MTK_SENSOR_ACCELEROMETER_USE_BMA255) compile options will be defined, 
##               and the corresponding sources and header files under driver/board/component/mems will be included by driver/board/component/mems/module.mk.
## Path:         driver/board/component/mems
## Dependency:   Must enable HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED 
##               in the hal_feature_config.h under the project inc folder.
## Notice:       None
## Relative doc: None
##
ifeq ($(MTK_SENSOR_ACCELEROMETER_USE),BMA255)
  CFLAGS += -DMTK_SENSOR_ACCELEROMETER_USE
  CFLAGS += -DMTK_SENSOR_ACCELEROMETER_USE_BMA255
endif

ifeq ($(MTK_SENSOR_ACCELEROMETER_USE),BMI160)
  CFLAGS += -DMTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE
  CFLAGS += -DMTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160
endif

ifeq ($(MTK_SENSOR_ACCELEROMETER_USE),ICM30630)
  CFLAGS += -DMTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE
  CFLAGS += -DMTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_ICM30630
endif

##
## MTK_SENSOR_MAGNETIC_USE
## Brief:        This option is to enable and select magnetic field sensor driver under driver/board/component/mems folder.
## Usage:        If the value is in the supported list (e.g. YAS33), the MTK_SENSOR_MAGNETIC_USE and correspoding selection (e.g. MTK_SENSOR_MAGNETIC_USE_YAS533) compile options will be defined, 
##               and the corresponding sources and header files under driver/board/component/mems will be included by driver/board/component/mems/module.mk.
## Path:         driver/board/component/mems
## Dependency:   Must enable HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED 
##               in the hal_feature_config.h under the project inc folder.
## Notice:       None
## Relative doc: None
##
ifeq ($(MTK_SENSOR_MAGNETIC_USE),YAS533)
  CFLAGS += -DMTK_SENSOR_MAGNETIC_USE
  CFLAGS += -DMTK_SENSOR_MAGNETIC_USE_YAS533
endif

##
## MTK_SENSOR_BAROMETER_USE
## Brief:        This option is to enable and select barometric sensor driver under driver/board/component/mems folder.
## Usage:        If the value is in the supported list (e.g. BMP280), the MTK_SENSOR_BAROMETER_USE and correspoding selection (e.g. MTK_SENSOR_BAROMETER_USE_BMP280) compile options will be defined, 
##               and the corresponding sources and header files under driver/board/component/mems will be included by driver/board/component/mems/module.mk.
## Path:         driver/board/component/mems
## Dependency:   Must enable HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED 
##               in the hal_feature_config.h under the project inc folder.
## Notice:       None
## Relative doc: None
##
ifeq ($(MTK_SENSOR_BAROMETER_USE),BMP280)
  CFLAGS += -DMTK_SENSOR_BAROMETER_USE
  CFLAGS += -DMTK_SENSOR_BAROMETER_USE_BMP280
endif

##
## MTK_SENSOR_PROXIMITY_USE
## Brief:        This option is to enable and select proximity sensor driver under driver/board/component/mems folder.
## Usage:        If the value is in the supported list (e.g. CM36672), the MTK_SENSOR_PROXIMITY_USE and correspoding selection (e.g. MTK_SENSOR_PROXIMITY_USE_CM36672) compile options will be defined, 
##               and the corresponding sources and header files under driver/board/component/mems will be included by driver/board/component/mems/module.mk.
## Path:         driver/board/component/mems
## Dependency:   Must enable HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED 
##               in the hal_feature_config.h under the project inc folder.
## Notice:       None
## Relative doc: None
##
ifeq ($(MTK_SENSOR_PROXIMITY_USE),CM36672)
  CFLAGS += -DMTK_SENSOR_PROXIMITY_USE
  CFLAGS += -DMTK_SENSOR_PROXIMITY_USE_CM36672
endif

##
## MTK_SENSOR_BIO_USE
## Brief:        This option is to enable and select biosensor driver under driver/board/component/biosensor folder.
## Usage:        If the value is in the supported list (e.g. MT2511), the MTK_SENSOR_BIO_USE and correspoding selection (e.g. MTK_SENSOR_BIO_USE_MT2511) compile options will be defined, 
##               and the corresponding sources and header files under driver/board/component/mems will be included by driver/board/component/biosensor/mt2511/module.mk.
## Path:         driver/board/component/biosensor
## Dependency:   Must enable HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED 
##               in the hal_feature_config.h under the project inc folder.
## Notice:       None
## Relative doc: None
##
ifeq ($(MTK_SENSOR_BIO_USE),MT2511)
  CFLAGS += -DMTK_SENSOR_BIO_USE
  CFLAGS += -DMTK_SENSOR_BIO_USE_MT2511
endif


##
## FUSION_HEART_RATE_MONITOR_USE
## Brief:        This option is to enable MediaTek heart rate monitor fusion algorithm library under middleware/MTK/sensor_subsys/lib/fusion_algo folder.
## Usage:        If the value is M_INHOUSE_HEART_RATE_MONITOR, the FUSION_HEART_RATE_MONITOR_USE compile option will be defined as M_INHOUSE_HEART_RATE_MONITOR, 
##               and the corresponding library and header files under middleware/MTK/sensor_subsys/lib/fusion_algo will be included by middleware/MTK/sensor_subsys/src/module.mk.
## Path:         middleware/MTK/sensor_subsys/lib/fusion_algo
## Dependency:   Must enable MTK_SENSOR_BIO_USE and MTK_SENSOR_ACCELEROMETER_USE in this file.
## Notice:       None
## Relative doc: None
##
ifeq ($(FUSION_HEART_RATE_MONITOR_USE),M_INHOUSE_HEART_RATE_MONITOR)
  CFLAGS += -DFUSION_HEART_RATE_MONITOR_USE=M_INHOUSE_HEART_RATE_MONITOR
endif

##
## FUSION_HEART_RATE_VARIABILITY_USE
## Brief:        This option is to enable MediaTek heart rate variability monitor fusion algorithm library under middleware/MTK/sensor_subsys/lib/fusion_algo folder.
## Usage:        If the value is M_INHOUSE_HEART_RATE_VARIABILITY, the FUSION_HEART_RATE_VARIABILITY_USE compile option will be defined as M_INHOUSE_HEART_RATE_VARIABILITY, 
##               and the corresponding library and header files under middleware/MTK/sensor_subsys/lib/fusion_algo will be included by middleware/MTK/sensor_subsys/src/module.mk.
## Path:         middleware/MTK/sensor_subsys/lib/fusion_algo
## Dependency:   Must enable MTK_SENSOR_BIO_USE in this file.
## Notice:       None
## Relative doc: None
##
ifeq ($(FUSION_HEART_RATE_VARIABILITY_USE),M_INHOUSE_HEART_RATE_VARIABILITY)
  CFLAGS += -DFUSION_HEART_RATE_VARIABILITY_USE=M_INHOUSE_HEART_RATE_VARIABILITY
endif


##
## MTK_MBEDTLS_CONFIG_FILE
## Brief:       This option is to configure mbedTLS features.
## Usage:       If the value is "*.h", mbedTLS module will use *.h as the configuration file. For example, if its value is "config-mtk-basic.h",
##              config-mtk-basic.h will be used as the configuration file. MTK_MBEDTLS_CONFIG_FILE compile option will be defined. You must also
##              include the /middleware/third_party/mbedtls/module.mk in your Makefile before setting the option to "*.h".
## Path:        middleware/third_party/mbedtls
## Dependency:  LWIP module must be enabled.
## Relative doc:Please refer to internet and open source software guide under the doc folder for more detail.
##
ifneq ($(MTK_MBEDTLS_CONFIG_FILE),)
CFLAGS += -DMBEDTLS_CONFIG_FILE=\"$(MTK_MBEDTLS_CONFIG_FILE)\"
endif

##
## MTK_HTTPCLIENT_SSL_ENABLE
## Brief:       This option is to switch SSL/TLS support in SSL client module.
## Usage:       To enable HTTP client support in a project, use "LIBS += $(OUTPATH)/libhttpclient.a" to include the middleware.
##              Additionally, switch this option to configure SSL support in HTTP client.
## Path:        middleware/third_party/httpclient
## Dependency:  LWIP and mbedTLS module must be enabled.
## Relative doc:Please refer to internet and open source software guide under the doc folder for more detail.
##
ifeq ($(MTK_HTTPCLIENT_SSL_ENABLE),y)
CFLAGS += -DMTK_HTTPCLIENT_SSL_ENABLE
endif

##
## MTK_WEBSOCKET_SSL_ENABLE
## Brief:       This option is to switch SSL/TLS support in websocket client module.
## Usage:       To enable websocket client support in a project, use "include $(SOURCE_DIR)/middleware/third_party/websocket/module.mk" to include the middleware.
##              Additionally, switch this option to configure SSL support in websocket client.
## Path:        middleware/third_party/websocket
## Dependency:  LWIP and mbedTLS module must be enabled.
## Related doc: Please refer to internet and open source software guide under the doc folder for more detail.
##
ifeq ($(MTK_WEBSOCKET_SSL_ENABLE),y)
CFLAGS += -DMTK_WEBSOCKET_SSL_ENABLE
endif

##
## MTK_DEBUG_LEVEL
## Brief:       This option is to configure system log debug level.
## Usage:       The valid values are empty, error, warning, info, debug, and none.
##              The setting will determine whether a debug log will be compiled.
##              However, the setting has no effect on the prebuilt library.
##              empty   : All debug logs are compiled.
##              error   : Only error logs are compiled.
##              warning : Only warning and error logs are compiled.
##              info    : Only info, warning, and error logs are compiled.
##              debug   : All debug logs are compiled.
##              none    : All debugs are disabled.
## Path:        kernel/service
## Dependency:  None
## Notice:      None
## Realted doc: Please refer to doc/LinkIt_for_RTOS_System_Log_Developers_Guide.pdf
##
ifeq ($(MTK_DEBUG_LEVEL),)
  CFLAGS += -DMTK_DEBUG_LEVEL_DEBUG
  CFLAGS += -DMTK_DEBUG_LEVEL_INFO
  CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
  CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),error)
  CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),warning)
  CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
  CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),info)
  CFLAGS += -DMTK_DEBUG_LEVEL_INFO
  CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
  CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),debug)
  CFLAGS += -DMTK_DEBUG_LEVEL_DEBUG
  CFLAGS += -DMTK_DEBUG_LEVEL_INFO
  CFLAGS += -DMTK_DEBUG_LEVEL_WARNING
  CFLAGS += -DMTK_DEBUG_LEVEL_ERROR
endif

ifeq ($(MTK_DEBUG_LEVEL),none)
  CFLAGS += -DMTK_DEBUG_LEVEL_NONE
endif

##
## MTK_BT_A2DP_AAC_ENABLE
## Brief:       This option is to enable Middleware BT A2DP AAC decoder function under gva/driver/board/mt25x3_hdk/bt_codec folder.
## Usage:       If the value is "y", the MTK_BT_A2DP_AAC_ENABLE compile option will be defined, the sources and header files under gva/driver/board/mt25x3_hdk/bt_codec/src and gva/driver/board/mt25x3_hdk/bt_codec/inc/ will be included by gva/driver/board/mt25x3_hdk/bt_codec/module.mk.
## Path:        gva/driver/board/mt25x3_hdk/bt_codec
## Dependency:  DSP, HAL_AUDIO_MODULE_ENABLED.
## Notice:      None. 
## Relative doc:None
##
ifeq ($(MTK_BT_A2DP_AAC_ENABLE),y)
CFLAGS   += -DMTK_BT_A2DP_AAC_ENABLE
endif

##
## MTK_AUDIO_TUNING_ENABLED
## Brief:       This option is to enable Middleware audio tuning tool functions under middleware/MTK/atci/at_command/ folder.
## Usage:       If the value is "y", the MTK_AUDIO_TUNING_ENABLED compile option will be defined, the audio tuning tool sources and header files under middleware/MTK/atci/at_command/ and middleware/MTK/atci/inc/ will be included by middleware/MTK/atci/module.mk.
## Path:        middleware/MTK/atci/at_command/, middleware/MTK/atci/inc/
## Dependency:  Must enable MTK_AUDIO_TUNING_ENABLED in the hal_feature_config.h under the project inc folder.
## Notice:      The default implementation of audio tuning tool under middleware/MTK/atci/at_command/ is none. 
## Relative doc:None
##
ifeq ($(MTK_AUDIO_TUNING_ENABLED), y)
CFLAGS += -DMTK_AUDIO_TUNING_ENABLED
endif

##
## MTK_AUDIO_MP3_ENABLED
## Brief:       This option is to enable middleware audio mp3 functions under middleware/MTK/audio/mp3_codec/ folder.
## Usage:       If the value is "y", the MTK_AUDIO_MP3_ENABLED compile option will be defined, the MP3 sources files, header files and library under middleware/MTK/audio/mp3_codec/src, middleware/MTK/audio/mp3_codec/inc and middleware/MTK/audio/mp3_codec/lib  will be included by middleware/MTK/audio/module.mk.
##              You must also include the /middleware/MTK/audio/module.mk, /middleware/third_party/fatfs/module.mk and /middleware/MTK/audio/mp3_codec/lib/arm_cm4/libmp3dec.a in your Makefile before setting the option to "y".
## Path:        middleware/MTK/audio/mp3_codec/
## Dependency:  Must enable MTK_FATFS_ENABLE in the feature.mk under the project GCC folder..
## Notice:      The default implementation of audio mp3 functions under middleware/MTK/audio/ is none.
## Relative doc:None
##
ifeq ($(MTK_AUDIO_MP3_ENABLED), y)
CFLAGS += -DMTK_AUDIO_MP3_ENABLED
endif

##
## MTK_AUDIO_AAC_DECODER_ENABLED
## Brief:       This option is to enable middleware audio AAC functions under middleware/MTK/audio/aac_codec/ folder.
## Usage:       If the value is "y", the MTK_AUDIO_AAC_DECODER_ENABLED compile option will be defined, the AAC sources files, header files and library under middleware/MTK/audio/aac_codec/src, middleware/MTK/audio/aac_codec/inc and prebuilt/middleware/MTK/audio/aac_codec/lib/  will be included by middleware/MTK/audio/module.mk.
##              You must also include the gva/middleware/MTK/audio/module.mk, gva/middleware/third_party/fatfs/module.mk and gva/prebuilt/middleware/MTK/audio/aac_codec/lib/libheaac_decoder.a in your Makefile before setting the option to "y".
## Path:        middleware/MTK/audio/aac_codec/
## Dependency:  Must enable MTK_FATFS_ENABLE in the feature.mk under the project GCC folder..
## Notice:      The default implementation of audio AAC functions under middleware/MTK/audio/ is none.
## Relative doc:None
##
ifeq ($(MTK_AUDIO_AAC_DECODER_ENABLED), y)
CFLAGS	+= -DMTK_AUDIO_AAC_DECODER_ENABLED
endif

##
## MTK_AUDIO_AAC_DECODER_FEATURE_HEAAC
## Brief:       This option is to enable middleware audio HE-AAC functions under middleware/MTK/audio/aac_codec/ folder.
## Usage:       If the value is "y", the MTK_AUDIO_AAC_DECODER_FEATURE_HEAAC compile option will be defined, the AAC sources files, header files and library under middleware/MTK/audio/aac_codec/src, middleware/MTK/audio/aac_codec/inc and prebuilt/middleware/MTK/audio/aac_codec/lib/  will be included by middleware/MTK/audio/module.mk.
##              You must also include the gva/middleware/MTK/audio/module.mk, gva/middleware/third_party/fatfs/module.mk and gva/prebuilt/middleware/MTK/audio/aac_codec/lib/libheaac_decoder.a in your Makefile before setting the option to "y".
## Path:        middleware/MTK/audio/aac_codec/
## Dependency:  Must enable MTK_FATFS_ENABLE and MTK_AUDIO_AAC_DECODER_ENABLED in the feature.mk under the project GCC folder..
## Notice:      The default implementation of audio AAC functions under middleware/MTK/audio/ is none.
## Relative doc:None
##
ifeq ($(MTK_AUDIO_AAC_DECODER_FEATURE_HEAAC), y)
CFLAGS	+= -DMTK_AUDIO_AAC_DECODER_FEATURE_HEAAC
endif

##
## MTK_PROMPT_SOUND_ENABLE
## Brief:       This option is to enable prompt sound feature.
## Usage:       If the value is "y",  the MTK_PROMPT_SOUND_ENABLE and MTK_AUDIO_MIXER_SUPPORT compile options will be defined.
## Path:        middleware/MTK/audio/audio_mixer/
## Dependency:  Must enable HAL_AUDIO_MODULE_ENABLED feature.mk under the project GCC folder..
## Notice:      None.
## Relative doc:None
##
ifeq ($(MTK_PROMPT_SOUND_ENABLE), y)
  CFLAGS += -DMTK_PROMPT_SOUND_ENABLE
  CFLAGS += -DMTK_AUDIO_MIXER_SUPPORT
endif

##
## MTK_DEVELOPMENT_BOARD_TYPE 
## Brief:     It is an option to specify the type of development board. 
## Usage:     Can be one of EVB, HDK, and FPGA.
## Path:      Files of board level drivers
## Dependency:None
##

ifeq ($(MTK_DEVELOPMENT_BOARD_TYPE),)
  CFLAGS += -DMTK_DEVELOPMENT_BOARD_HDK
else
  ifeq ($(MTK_DEVELOPMENT_BOARD_TYPE),EVB)
    CFLAGS += -DMTK_DEVELOPMENT_BOARD_EVB
  else ifeq ($(MTK_DEVELOPMENT_BOARD_TYPE),HDK)
    CFLAGS += -DMTK_DEVELOPMENT_BOARD_HDK
  else ifeq ($(MTK_DEVELOPMENT_BOARD_TYPE),FPGA)
    CFLAGS += -DMTK_DEVELOPMENT_BOARD_FPGA
  else
    CFLAGS += -DMTK_DEVELOPMENT_BOARD_HDK
  endif
endif

##
## MTK_XOSC_MODE
## Brief:       This option is to enable or disable XOSC mode in the RTC module.
## Usage:       If the value is "XOSC", the MTK_XOSC_MODE compile option will be defined, and the RTC module will use XOSC mode instead of SCXO mode.
## Path:        driver/chip/mt2523
## Dependency:  Must enable HAL_RTC_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.
## Notice:      The default implementation of RTC under driver/chip/mt2523 is using SCXO mode. 
##              Please configure MTK_XOSC_MODE value as XOSC in project/mt2523_hdk/apps/<project>/gcc/feature.mk if user must use XOSC mode.
## Relative doc:None
##
ifeq ($(MTK_XOSC_MODE),XOSC)
  CFLAGS += -DUSE_XOSC_MODE
endif

##
## MTK_PORT_SERVICE_ENABLE
## Brief:       This option is to enable port service feature.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/port_service
## Dependency:  NVDM module must be enabled.
## Notice:      None
## Relative doc:None
##
ifeq ($(MTK_PORT_SERVICE_ENABLE),y)
  CFLAGS += -DMTK_PORT_SERVICE_ENABLE
endif


##
## MTK_ATCI_VIA_PORT_SERVICE
## Brief:       This option is to enable ATCI through port service feature for data transmission in mt2523.
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/atci/src/, middleware/MTK/atci/inc/
## Dependency:  Must enable the MTK_PORT_SERVICE_ENABLE for port service feature
## Notice:      The default implementation of ATCI for data transmission is using HAL UART directly.  
## Relative doc:None
##
ifeq ($(MTK_PORT_SERVICE_ENABLE),y)
ifeq ($(MTK_ATCI_VIA_PORT_SERVICE),y)
  CFLAGS += -DMTK_ATCI_VIA_PORT_SERVICE
endif
endif


##
## MTK_AT_CMD_DISABLE
## Brief:       This option is to disable the default AT command under middleware/MTK/atci/at_command/
## Usage:       Enable the feature by configuring it as y.
## Path:        middleware/MTK/atci/at_command/, middleware/MTK/atci/inc/
## Dependency:  None.
## Notice:      None.  
## Relative doc:None
##

ifeq ($(MTK_AT_CMD_DISABLE),y)
COM_CFLAGS         += -DMTK_AT_CMD_DISABLE
endif

##
## MTK_WIFI_CHIP_USE_MT5931
## Brief:       This option is to enable Wi-Fi using a MT5931 Wi-Fi chip
## Usage:       Enable the feature by configuring it as y
## Path:        middleware/MTK/minorsupc/src/, driver/board/component/wifi/mt5931, driver/board/mt25x3_hdk/wifi/mt5931
## Dependency:  TBD
## Notice:      TBD
## Relative doc:TBD
##

ifeq ($(MTK_WIFI_CHIP_USE_MT5931),y)
CFLAGS         += -DMTK_WIFI_CHIP_USE_MT5931
endif


##
## MTK_WIFI_CHIP_USE_MT5932
## Brief:       This option is to enable Wi-Fi using a MT5932 Wi-Fi chip
## Usage:       Enable the feature by configuring it as y
## Path:        middleware/MTK/wifi_service/stub_conf, middleware/MTK/wifi_service/wifi_host
## Dependency:  TBD
## Notice:      TBD
## Relative doc:TBD
##

ifeq ($(MTK_WIFI_CHIP_USE_MT5932),y)
CFLAGS         += -DMTK_WIFI_CHIP_USE_MT5932
endif

###############################################################################
##
## The following makefile options are not configurable and may be removed in the future. 
## Please do not set the makefile options to "y" in your GCC feature configuration.
##

ifeq ($(__BT_ENABLE_BA__),y)
  CFLAGS += -D__BT_ENABLE_BA__
endif

ifeq ($(MTK_SYSLOG_DMA),y)
  CFLAGS += -DSYSLOG_DMA_ENABLE
endif

ifeq ($(MTK_MVG_FLASH_TEST_ENABLE), y)
CFLAGS += -D__NOR_FLASH_HARDWARE_TEST__
MVG_SRC = project/mt2523_hdk/test_loads/HQA/hqa_flash_test
CFLAGS += -I$(SOURCE_DIR)/$(MVG_SRC)/inc/mvg
endif

ifeq ($(MTK_SUPPORT_SFC_DVT_ENABLE),y)
  CFLAGS += -D__SF_DVT__
endif

ifeq ($(MTK_BUILD_SMT_LOAD),y)
CFLAGS         += -DMTK_BUILD_SMT_LOAD
endif

ifeq ($(MTK_BUILD_HQA_LOAD),y)
CFLAGS	+= -DMTK_BUILD_HQA_LOAD
endif

##
## MTK_BLE_BQB_TEST_ENABLE
## Brief:       This option is to enable and disable BLE BQB test.
## Usage:       If the value is "y", the MTK_BT_BQB_ENABLE compile option will be defined. You must include some module in your Makefile before setting.
##              the option to "y"
##              include the /middleware/MTK/bluetooth/module.mk
##              include the /middleware/MTK/bluetooth/lib/libble.a
## Path:        middleware/MTK/bluetooth/inc
## Dependency:  Add "include /middleware/MTK/bluetooth/module.mk" in the Makefile.
##
ifeq ($(MTK_BLE_BQB_TEST_ENABLE),y)
CFLAGS += -DMTK_BLE_BQB_ENABLE
endif

##
## MTK_BT_BQB_TEST_ENABLE
## Brief:       This option is to enable and disable BT BQB test.
## Usage:       If the value is "y", the MTK_BT_BQB_ENABLE compile option will be defined. You must include some module in your Makefile before setting.
##              the option to "y"
##              include the /middleware/MTK/bluetooth/module.mk
##              include the /middleware/MTK/bluetooth/lib/libbt.a
## Path:        middleware/MTK/bluetooth/inc
## Dependency:  Add "include /middleware/MTK/bluetooth/module.mk" in the Makefile.
##
ifeq ($(MTK_BT_BQB_TEST_ENABLE),y)
CFLAGS += -DMTK_BT_BQB_ENABLE
endif

ifeq ($(MTK_BT_NORMAL_MODE_ENABLE),y)
CFLAGS += -DMTK_BT_NORMAL_MODE_ENABLE
CFLAGS += -DBT_SINK_DEMO
CFLAGS += -DSUPPORT_BT_SINK_TEST_CASE
endif

ifeq ($(MTK_HEAP_GUARD_ENABLE),y)
ALLFLAGS       += -Wl,-wrap=pvPortMalloc -Wl,-wrap=vPortFree
CFLAGS         += -DHEAP_GUARD_ENABLE
endif

ifeq ($(MTK_SUPPORT_HEAP_DEBUG),y)
CFLAGS         += -DMTK_SUPPORT_HEAP_DEBUG
endif

ifeq ($(MTK_HEAP_SIZE_GUARD_ENABLE),y)
ALLFLAGS       += -Wl,-wrap=pvPortMalloc -Wl,-wrap=vPortFree
CFLAGS         += -DMTK_HEAP_SIZE_GUARD_ENABLE
endif

ifeq ($(MTK_MET_TRACE_ENABLE),y)
CFLAGS         += -DMET_TRACE_ENABLE
endif

ifeq ($(MTK_NO_PSRAM_ENABLE),y)
CFLAGS         += -DMTK_NO_PSRAM_ENABLE
endif

ifeq ($(MTK_CODE_COVERAGE_ENABLE),y)
CC = $(BINPATH)/arm-none-eabi-cov-gcc
CXX= $(BINPATH)/arm-none-eabi-cov-g++
AR = $(BINPATH)/arm-none-eabi-cov-ar
export GCOV_DIR=$(SOURCE_DIR)
endif

ifeq ($(MTK_MAIN_CONSOLE_UART2_ENABLE),y)
CFLAGS         += -DMTK_MAIN_CONSOLE_UART2_ENABLE
endif

ifeq ($(FUSION_PEDOMETER_USE),M_INHOUSE_PEDOMETER)
  CFLAGS += -DFUSION_PEDOMETER_USE=M_INHOUSE_PEDOMETER
endif

ifeq ($(FUSION_PEDOMETER_USE),ICM_PEDOMETER)
  CFLAGS += -DFUSION_PEDOMETER_USE=ICM_PEDOMETER
endif

ifeq ($(FUSION_SLEEP_TRACKER_USE),M_INHOUSE_SLEEP_TRACKER)
  CFLAGS += -DFUSION_SLEEP_TRACKER_USE=M_INHOUSE_SLEEP_TRACKER
endif

ifeq ($(FUSION_SLEEP_STAGING_USE),M_INHOUSE_SLEEP_STAGING)
  CFLAGS += -DFUSION_SLEEP_STAGING_USE=M_INHOUSE_SLEEP_STAGING
endif

ifeq ($(FUSION_BLOOD_PRESSURE_USE),M_INHOUSE_BLOOD_PRESSURE)
  CFLAGS += -DFUSION_BLOOD_PRESSURE_USE=M_INHOUSE_BLOOD_PRESSURE
endif

ifeq ($(MTK_VM_LOG_ENABLE),y)
  CFLAGS += -DMTK_VM_LOG_ENABLE
endif

ifeq ($(MTK_BT_HFP_CODEC_DEBUG),y)
  CFLAGS += -DMTK_BT_HFP_CODEC_DEBUG
  CFLAGS += -DMTK_VM_LOG_ENABLE
endif


###############################################################################

#Incldue Path
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/nghttp2/lib/includes
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/nghttp2/lib/includes/nghttp2
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/xml/inc
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS += -I$(SOURCE_DIR)/kernel/service/src_protected/inc

#Middleware Module Path
MID_TFTP_PATH 		= $(SOURCE_DIR)/middleware/MTK/tftp
MID_GNSS_PATH 		= $(SOURCE_DIR)/middleware/MTK/gnss
MID_LWIP_PATH 		= $(SOURCE_DIR)/middleware/third_party/lwip
MID_CJSON_PATH 		= $(SOURCE_DIR)/middleware/third_party/cjson
MID_DHCPD_PATH 		= $(SOURCE_DIR)/middleware/MTK/dhcpd
MID_HTTPCLIENT_PATH = $(SOURCE_DIR)/middleware/third_party/httpclient
MID_MBEDTLS_PATH 	= $(SOURCE_DIR)/middleware/third_party/mbedtls

MID_MQTT_PATH 		= $(SOURCE_DIR)/middleware/third_party/mqtt
MID_NGHTTP2_PATH 	= $(SOURCE_DIR)/middleware/third_party/nghttp2
MID_SNTP_PATH 		= $(SOURCE_DIR)/middleware/third_party/sntp
MID_XML_PATH 		= $(SOURCE_DIR)/middleware/third_party/xml
MID_HTTPD_PATH 		= $(SOURCE_DIR)/middleware/third_party/httpd
MID_PING_PATH 		= $(SOURCE_DIR)/middleware/third_party/ping
MID_IPERF_PATH 		= $(SOURCE_DIR)/middleware/MTK/iperf
MID_FATFS_PATH 		= $(SOURCE_DIR)/middleware/third_party/fatfs
DRV_CHIP_PATH 		= $(SOURCE_DIR)/driver/chip/mt2523
DRV_BSP_PATH 		= $(SOURCE_DIR)/driver/board/mt25x3_hdk
DRV_WNDRV_PATH          = $(SOURCE_DIR)/driver/board/component/wifi/mt5931
DRV_MINORSUPC_PATH      = $(SOURCE_DIR)/middleware/MTK/minorsupc
KRL_OS_PATH 		= $(SOURCE_DIR)/kernel/rtos/FreeRTOS
MID_LZMA_PATH           = $(SOURCE_DIR)/middleware/third_party/lzma_decoder
MID_SLP_PATH 		= $(SOURCE_DIR)/middleware/MTK/slp
MID_BT_NOTIFY_PATH       = $(SOURCE_DIR)/middleware/MTK/bt_notify
