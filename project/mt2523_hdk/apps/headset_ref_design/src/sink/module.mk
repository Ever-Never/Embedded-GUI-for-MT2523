# BT sink source files
BT_SINK_FILES = $(APP_PATH_SRC)/sink/bt_sink_app_event.c \
                $(APP_PATH_SRC)/sink/bt_sink_app_config.c \
                $(APP_PATH_SRC)/sink/bt_sink_app_main.c \
                $(APP_PATH_SRC)/sink/bt_sink_app_cmd.c \
                $(APP_PATH_SRC)/sink/bt_sink_app_keypad.c

C_FILES += $(BT_SINK_FILES)

ifeq ($(MTK_LED_ENABLE),y)
C_FILES +=   $(APP_PATH_SRC)/sink/bt_sink_app_led.c \
             $(APP_PATH_SRC)/sink/bt_sink_app_led_config.c
endif

ifeq ($(MTK_PROMPT_SOUND_ENABLE),y)
C_FILES +=   $(APP_PATH_SRC)/sink/bt_sink_app_voice_prompt.c
endif

# Include bt sink path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_sink/inc
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/sink
CFLAGS += -mno-unaligned-access
