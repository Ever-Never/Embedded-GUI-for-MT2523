ifeq ($(MTK_BT_DUO_ENABLE), y)
# BT sink source files
BT_SINK_SRV_SRC = middleware/MTK/bt_sink/src
BT_SINK_SRV_FILES = $(BT_SINK_SRV_SRC)/bt_sink_srv.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_action.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_atci_cmd.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_conmgr.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_custom_db.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_common.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_db.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_hf.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_hf_call_manager.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_hf_multipoint.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_state_notify.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_utils.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_a2dp.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_avrcp.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_music.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_pbapc.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_spp.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_audio_setting.c \
                    $(BT_SINK_SRV_SRC)/bt_sink_srv_aws.c

ifeq ($(MTK_BT_AWS_ENABLE), y)
BT_SINK_SRV_FILES += $(BT_SINK_SRV_SRC)/bt_sink_srv_audio_sync.c
endif
C_FILES += $(BT_SINK_SRV_FILES)

BT_AM_SRC = middleware/MTK/bt_sink/src
BT_AM_FILES = $(BT_AM_SRC)/bt_sink_srv_am_task.c \
              $(BT_AM_SRC)/bt_sink_srv_ami.c \
              $(BT_AM_SRC)/bt_sink_srv_am_med_mgr.c
C_FILES += $(BT_AM_FILES)

AUDIO_TUNNING_SRC = middleware/MTK/bt_sink/src
AUDIO_TUNNING_FILES = $(AUDIO_TUNNING_SRC)/bt_sink_srv_audio_tunning.c
C_FILES += $(AUDIO_TUNNING_FILES)

# BT callback manager module
include $(SOURCE_DIR)/middleware/MTK/bt_callback_manager/module.mk

# BT device manager module
include $(SOURCE_DIR)/middleware/MTK/bluetooth_service/bt_device_manager_module.mk

# Include bt sink path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth_service/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_sink/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/bt_codec/inc
CFLAGS += -mno-unaligned-access
CFLAGS += -D__MUSIC_MODE_SWITCH_SUPPORT__

endif
