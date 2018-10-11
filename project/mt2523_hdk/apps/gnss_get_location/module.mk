# source files

GNSS_APP_PATH = project/mt2523_hdk/apps/gnss_get_location
C_FILES += $(GNSS_APP_PATH)/src/epo_demo.c
C_FILES += $(GNSS_APP_PATH)/src/epo_download.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_app.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_bridge_task.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_demo.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_ring_buffer.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_timer.c
C_FILES += $(GNSS_APP_PATH)/src/gnss_uart_bridge.c

# include files

CFLAGS += -I$(SOURCE_DIR)/$(GNSS_APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc

