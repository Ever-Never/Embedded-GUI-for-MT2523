ifeq ($(MTK_AUDIO_PASS_THROUGH_ENABLED), y)
  LIBS += $(SOURCE_DIR)/prebuilt/driver/board/component/audio/lib/arm_cm4/libblisrc.a
endif

CFLAGS += -I$(SOURCE_DIR)/prebuilt/driver/board/component/audio/inc
