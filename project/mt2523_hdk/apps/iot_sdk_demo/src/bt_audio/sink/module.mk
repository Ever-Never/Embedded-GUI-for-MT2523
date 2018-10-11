# BT sink source files
BT_SINK_FILES = $(APP_PATH_SRC)/bt_audio/sink/bt_sink_app_event.c \
                $(APP_PATH_SRC)/bt_audio/sink/bt_sink_app_mapping_table.c \
                $(APP_PATH_SRC)/bt_audio/sink/bt_sink_app_main.c \
                $(APP_PATH_SRC)/bt_audio/sink/bt_sink_app_cmd.c

C_FILES += $(BT_SINK_FILES)

# Include bt sink path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_sink/inc
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/bt_audio/sink
CFLAGS += -mno-unaligned-access
