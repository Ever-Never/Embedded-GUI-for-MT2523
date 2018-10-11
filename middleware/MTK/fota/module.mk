
###############################################################################
# feature option dependency
###############################################################################

## MTK_FOTA_ENABLE
## Brief:       This option is to enable and disable FOTA.
## Usage:       If the value is "y", the MTK_FOTA_ENABLE compile option will be defined. You must also include the gva3\middleware\MTK\fota\module.mk in your Makefile before setting the option to "y". 
## Path:        middleware/MTK/fota
## Dependency:  HAL_FLASH_MODULE_ENABLED and HAL_WDT_MODULE_ENABLED must also defined in the hal_feature_config.h  under the project inc folder. If MTK_FOTA_CLI_ENABLE defined in the module.mk is "y", please also include the middleware/MTK/tftp/module.mk. 
## Notice:      Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/gcc/full_bin_fota/flash.ld for gcc build environment. 
##              Please use the driver/CMSIS/Device/MTK/mt2523/linkerscript/rvct/full_bin_fota/flash.sct for KEIL build environment. 

ifeq ($(MTK_FOTA_ENABLE),y)
CFLAGS += -DMTK_FOTA_ENABLE
endif

ifeq ($(MTK_FOTA_CMD_ENABLE),y)
CFLAGS += -DMTK_FOTA_CMD_ENABLE
endif

ifeq ($(MTK_FOTA_UBIN_ENABLE),y)
CFLAGS += -DMTK_FOTA_UBIN_ENABLE
endif


ifeq ($(MTK_FOTA_CM4_FS_ENABLE), y)
CFLAGS += -DFOTA_FS_ENABLE
endif

ifeq ($(IC_CONFIG), $(filter $(IC_CONFIG),mt7687 mt7697))
CFLAGS += -DMTK_FOTA_ON_7687

ifeq ($(MTK_FOTA_DUAL_IMAGE_ENABLE), y)
CFLAGS += -DMTK_FOTA_DUAL_IMAGE_ENABLE

ifeq ($(MTK_FOTA_DUAL_IMAGE_ONE_PACK_ENABLE), y)
CFLAGS += -DMTK_FOTA_DUAL_IMAGE_ONE_PACK
endif

endif
endif

#################################################################################
# source files
#################################################################################

FOTA_SRC = middleware/MTK/fota

C_FILES  += $(FOTA_SRC)/src/fota.c
ifneq ($(MTK_FOTA_CM4_FS_ENABLE), y)
C_FILES  += $(FOTA_SRC)/src/internal/fota_internal.c
C_FILES  += $(FOTA_SRC)/src/internal/fota_config.c
C_FILES  += $(FOTA_SRC)/src/internal/fota_port.c
endif
ifeq ($(IC_CONFIG), $(filter $(IC_CONFIG),mt7687 mt7697 mt7682 mt7686))
C_FILES  += $(FOTA_SRC)/src/internal/fota_cli.c
ifeq ($(MTK_FOTA_DUAL_IMAGE_ENABLE),y)
C_FILES  += $(FOTA_SRC)/src/internal/fota_dual_image.c
else
C_FILES  += $(FOTA_SRC)/src/internal/fota_download_interface.c
endif
endif

#################################################################################
# include path
#################################################################################

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/fota/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/fota/inc/internal
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/tftp/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/httpclient/inc

ifeq ($(MTK_BL_LOAD_ENABLE),y)
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/bootloader/src/mt7687
CFLAGS  += -DBL_FOTA_DEBUG
endif

ifeq ($(MTK_FOTA_DUAL_IMAGE_ENABLE),y)
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/mbedtls/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/httpclient/inc
endif
