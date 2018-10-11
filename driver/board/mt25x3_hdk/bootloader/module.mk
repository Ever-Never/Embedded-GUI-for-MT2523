# Sources
# Bootloader project files

# RTOS source files
RTOS_SRC = kernel/rtos/FreeRTOS/Source
# EMI source files
EMI_SRC = driver/board/mt25x3_hdk/emi
# MTD source files
MTD_SRC = driver/chip/mt2523/
# MBEDTLS source files
MBEDTLS_SRC = middleware/third_party/mbedtls

#Bootloader source files
BOOTLOADER_SRC = driver/board/mt25x3_hdk/bootloader

BOOTLOADER_FILES = $(BOOTLOADER_SRC)/core/src/bl_main.c \
				   $(BOOTLOADER_SRC)/core/src/bl_dbgprint.c \
				   $(BOOTLOADER_SRC)/core/src/bl_uart.c \
				   $(BOOTLOADER_SRC)/core/src/bl_clock.c \
				   $(BOOTLOADER_SRC)/core/src/custom_blconfig.c \
				   $(BOOTLOADER_SRC)/core/src/bl_gpt.c \
				   $(BOOTLOADER_SRC)/core/src/bl_misc.c \
				   $(BOOTLOADER_SRC)/core/src/bl_dcxo_cal.c \
				   $(BOOTLOADER_SRC)/core/src/bl_pmic.c \
				   $(BOOTLOADER_SRC)/core/src/bl_usb.c \
				   $(BOOTLOADER_SRC)/core/src/pmic/bl_mt6385.c \
				   $(BOOTLOADER_SRC)/core/src/pmic/bl_max14745.c \
				   $(BOOTLOADER_SRC)/core/src/bl_wdt.c

#Driver source files
DRIVER_FILES =            $(EMI_SRC)/src/emi_init.c \
                          $(EMI_SRC)/src/memory_test.c \
                          $(MTD_SRC)/src/hal_i2c_pmic.c  \
                          $(MTD_SRC)/src/hal_log.c  \
                          $(MTD_SRC)/src/hal_i2c_master_internal.c  \
                          $(MTD_SRC)/src/hal_flash_mtd_common.c	\
                          $(MTD_SRC)/src/hal_flash_mtd_dal.c	\
                          $(MTD_SRC)/src/hal_flash_custom_sfi.c	\
                          $(MTD_SRC)/src/hal_flash_combo_init.c	\
                          $(MTD_SRC)/src/hal_flash_cfi.c	\
                          $(MTD_SRC)/src/hal_flash_custom.c	\
                          $(MTD_SRC)/src/hal_flash_combo_nor.c	\
                          $(MTD_SRC)/src/hal_flash_disk.c	\
                          $(MTD_SRC)/src/hal_flash_mtd.c \
                          $(MTD_SRC)/src/hal_cache.c \
                          $(MTD_SRC)/src/hal_cache_internal.c \
                          $(MTD_SRC)/src/hal_nvic_internal.c \

ifeq ($(MTK_FOTA_FS_ENABLE),y)
BOOTLOADER_FILES += $(BOOTLOADER_SRC)/core/src/bl_msdc.c
BOOTLOADER_FILES += $(BOOTLOADER_SRC)/core/src/bl_sd_internal.c
BOOTLOADER_FILES += $(BOOTLOADER_SRC)/core/src/bl_sd.c
endif

ifeq ($(MTK_FOTA_ENABLE),y)
ifeq ($(MTK_BL_FOTA_CUST_ENABLE),y)
BOOTLOADER_FILES += $(APP_PATH)/src/custom_fota.c
else
BOOTLOADER_FILES += $(BOOTLOADER_SRC)/core/src/bl_fota.c
DRIVER_FILES += $(MBEDTLS_SRC)/library/sha1.c
CFLAGS += -DMBEDTLS_CONFIG_FILE='<config-mtk-sha1.h>'
endif
endif

ifeq ($(MTK_BL_DCXO_KVALUE_SW),y)
CFLAGS += -DBL_DCXO_KVALUE_SW
endif

ifeq ($(MTK_CAL_DCXO_CAPID),0)
CFLAGS += -DBL_RUN_DCXO_CAL
  ifneq ($(MTK_BL_DCXO_KVALUE_SW),y)
  CFLAGS += -DBL_CAL_DCXO_CAPID0
  endif
endif

ifeq ($(MTK_CAL_DCXO_CAPID),1)
CFLAGS += -DBL_RUN_DCXO_CAL
  ifneq ($(MTK_BL_DCXO_KVALUE_SW),y)
  CFLAGS += -DBL_CAL_DCXO_CAPID1
  endif
endif

ifeq ($(MTK_CAL_DCXO_CAPID),2)
CFLAGS += -DBL_RUN_DCXO_CAL
  ifneq ($(MTK_BL_DCXO_KVALUE_SW),y)
  CFLAGS += -DBL_CAL_DCXO_CAPID2
  endif
endif

ifeq ($(MTK_EXTERNAL_PMIC),y)
  ifeq ($(MTK_EXTERNAL_PMIC_TYPE),MT6327)
  BOOTLOADER_FILES += $(BOOTLOADER_SRC)/core/src/pmic/bl_mt6327.c
  endif
endif



###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(RTOS_SRC)/portable/GCC/ARM_CM4F
CFLAGS += -I$(SOURCE_DIR)/$(BOOTLOADER_SRC)/core/inc
CFLAGS += -I$(SOURCE_DIR)/$(BOOTLOADER_SRC)/gfh/include
CFLAGS += -I$(SOURCE_DIR)/$(EMI_SRC)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc #common driver include path
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt2523/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/$(MBEDTLS_SRC)/include
CFLAGS += -I$(SOURCE_DIR)/$(MBEDTLS_SRC)/configs
CFLAGS += -I$(SOURCE_DIR)/$(BOOTLOADER_SRC)/core/inc/pmic

