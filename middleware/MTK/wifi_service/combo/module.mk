
WIFI_SERVICE_SRC = middleware/MTK/wifi_service/combo

#include connsys -- included in main Makefile
#include $(SOURCE_DIR)/middleware/MTK/connsys/module.mk

# wifi src protected
WIFI_LIB_FOLDER=
ifeq ($(IC_CONFIG), mt7682)
WIFI_LIB_FOLDER=wifi_inic
else ifeq ($(IC_CONFIG), mt7686)
WIFI_LIB_FOLDER=wifi_inic
else ifeq ($(IC_CONFIG), mt5932)
WIFI_LIB_FOLDER=wifi_inic
else ifeq ($(IC_CONFIG), mt7687)
WIFI_LIB_FOLDER=wifi_supp
else ifeq ($(IC_CONFIG), mt7697)
WIFI_LIB_FOLDER=wifi_supp
endif

$(warning WIFI_LIB_FOLDER=$(WIFI_LIB_FOLDER))

ifneq ($(wildcard $(strip $(SOURCE_DIR))/middleware/MTK/wifi_service_protected/),)
include $(SOURCE_DIR)/middleware/MTK/wifi_service_protected/combo/src_protected/GCC/module.mk
else
include $(SOURCE_DIR)/prebuilt/middleware/MTK/wifi_service/combo/module.mk
endif

C_FILES  += $(WIFI_SERVICE_SRC)/src/get_profile_string.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/misc.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_os_ext/os_util.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_os_ext/os_utils.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_profile.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_wps.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/init/wifi_init.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_os_api.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/init/wifi_default_config.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/inband_queue_option.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_repeater.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_cm4_scan.c
ifeq ($(MTK_WIFI_CONFIGURE_FREE_ENABLE),y)
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_config_free.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/sie/sie.c
endif

ifeq ($(MTK_MINICLI_ENABLE),y)
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_cli.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_private_cli.c
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_inband_cli.c
ifeq ($(MTK_WIFI_CONFIGURE_FREE_ENABLE),y)
C_FILES  += $(WIFI_SERVICE_SRC)/src/sie/sie_cli.c
endif
ifeq ($(MTK_ATCI_ENABLE),y)
ifeq ($(MTK_WIFI_AT_COMMAND_ENABLE),y)
C_FILES  += $(WIFI_SERVICE_SRC)/src/at_command_wifi.c
CFLAGS += -DMTK_WIFI_AT_COMMAND_ENABLE
endif
endif

endif

ifeq ($(MTK_ANT_DIV_ENABLE),y)
C_FILES  += $(WIFI_SERVICE_SRC)/src/wifi_ant_div.c
endif
#################################################################################
#include path
CFLAGS 	+= -Iinc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/wifi_stack/inc

