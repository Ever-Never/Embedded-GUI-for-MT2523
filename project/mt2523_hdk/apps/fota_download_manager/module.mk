# source files

FOTA_APP_PATH = project/mt2523_hdk/apps/fota_download_manager
BT_NOTIFY_SRC = $(FOTA_APP_PATH)/src/btnotify_src
GNSS_DL_SRC = $(FOTA_APP_PATH)/src/gnss_src
C_FILES += $(FOTA_APP_PATH)/src/fota_main.c
C_FILES += $(GNSS_DL_SRC)/brom_base.c
C_FILES += $(GNSS_DL_SRC)/brom_mt3301.c
C_FILES += $(GNSS_DL_SRC)/da_cmd.c
C_FILES += $(GNSS_DL_SRC)/flashtool.c
C_FILES += $(GNSS_DL_SRC)/gnss_uart.c

# include files
CFLAGS += -I$(SOURCE_DIR)/$(FOTA_APP_PATH)/inc/fota_inc
CFLAGS += -I$(SOURCE_DIR)/$(FOTA_APP_PATH)/inc/btnotify_inc
CFLAGS += -I$(SOURCE_DIR)/$(FOTA_APP_PATH)/inc/gnss_inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc

