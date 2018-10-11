ifeq ($(MTK_BT_CODEC_ENABLED), y)
  LIBS += $(SOURCE_DIR)/prebuilt/driver/board/component/bt_codec/lib/arm_cm4/libcvsd_codec.a
  LIBS += $(SOURCE_DIR)/prebuilt/driver/board/component/bt_codec/lib/arm_cm4/libmsbc_codec.a
  LIBS += $(SOURCE_DIR)/prebuilt/driver/board/component/bt_codec/lib/arm_cm4/libplc.a
endif

CFLAGS += -I$(SOURCE_DIR)/prebuilt/driver/board/component/bt_codec/inc
