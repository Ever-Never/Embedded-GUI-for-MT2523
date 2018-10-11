
###################################################

AUDIO_SRC = middleware/MTK/audio

ifeq ($(MTK_TEMP_REMOVE), y)
else
  ifeq ($(MTK_NVDM_ENABLE), y)
    AUDIO_FILES = $(AUDIO_SRC)/src/audio_middleware_api.c
    AUDIO_FILES += $(AUDIO_SRC)/src/audio_dsp_fd216_db_to_gain_value_mapping_table.c

    ifeq ($(IC_CONFIG),mt2533)
      AUDIO_FILES += $(AUDIO_SRC)/port/mt2533/src/audio_nvdm.c
    else
      AUDIO_FILES += $(AUDIO_SRC)/port/mt2523/src/audio_nvdm.c
    endif

  endif
endif

# temp for build pass
ifeq ($(MTK_WAV_DECODER_ENABLE), y)
    AUDIO_FILES += $(AUDIO_SRC)/src/audio_codec.c
endif

ifeq ($(MTK_AUDIO_MP3_ENABLED), y)
  ifeq ($(IC_CONFIG),mt7687)
    AUDIO_FILES += $(AUDIO_SRC)/mp3_codec/src/mp3_codec_7687.c
    LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/mp3_codec/lib/arm_cm4/libmp3dec.a
  else
    ifeq ($(IC_CONFIG),mt7682)
      AUDIO_FILES += $(AUDIO_SRC)/mp3_codec/src/mp3_codec_7682.c
      LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/mp3_codec/lib/arm_cm4/libmp3dec.a
    else ifeq ($(IC_CONFIG),mt7686)
      AUDIO_FILES += $(AUDIO_SRC)/mp3_codec/src/mp3_codec_7682.c
      LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/mp3_codec/lib/arm_cm4/libmp3dec.a
    else
      AUDIO_FILES += $(AUDIO_SRC)/mp3_codec/src/mp3_codec.c
      LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/mp3_codec/lib/arm_cm4/libmp3dec.a
    endif
  endif
  # temp for build pass
  AUDIO_FILES += $(AUDIO_SRC)/src/audio_codec.c
endif

ifeq ($(MTK_AUDIO_AMR_ENABLED), y)
ifeq ($(IC_CONFIG),mt7687)
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_encoder.c
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_decoder.c
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/amr_codec/lib/arm_cm4/libamr.a
endif
ifeq ($(IC_CONFIG),mt7682)
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_encoder_7682.c
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_decoder_7682.c
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/amr_codec/lib/arm_cm4/libamr.a
endif
ifeq ($(IC_CONFIG),mt7686)
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_encoder_7682.c
AUDIO_FILES += $(AUDIO_SRC)/amr_codec/src/amr_decoder_7682.c
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/amr_codec/lib/arm_cm4/libamr.a
endif
endif


ifeq ($(MTK_AUDIO_AAC_DECODER_ENABLED),y)
    ifeq ($(IC_CONFIG),mt7682)
      AUDIO_FILES  += $(AUDIO_SRC)/aac_codec/src/aac_decoder_api_7686.c
    else ifeq ($(IC_CONFIG),mt7686)
      AUDIO_FILES  += $(AUDIO_SRC)/aac_codec/src/aac_decoder_api_7686.c
    else
      AUDIO_FILES  += $(AUDIO_SRC)/aac_codec/src/aac_decoder_api.c
    endif
    LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/aac_codec/lib/libheaac_decoder.a
endif

ifeq ($(MTK_PROMPT_SOUND_ENABLE), y)
  LIBS += $(SOURCE_DIR)/prebuilt/driver/board/component/audio/lib/arm_cm4/libblisrc.a

  AUDIO_FILES += $(AUDIO_SRC)/audio_mixer/src/audio_mixer.c
  AUDIO_FILES += $(AUDIO_SRC)/prompt_control/src/prompt_control.c
endif

ifeq ($(MTK_WAV_DECODER_ENABLE), y)
    AUDIO_FILES += $(AUDIO_SRC)/wav_codec/src/wav_codec.c
    LIBS += $(SOURCE_DIR)/middleware/MTK/audio/wav_codec/lib/arm_cm4/libwav_codec.a
endif

ifeq ($(MTK_BT_A2DP_SOURCE_ENABLE), y)
    AUDIO_FILES += $(AUDIO_SRC)/sbc_codec/src/sbc_codec.c
    LIBS += $(SOURCE_DIR)/middleware/MTK/audio/sbc_codec/lib/arm_cm4/libsbc_encoder.a
    LIBS += $(SOURCE_DIR)/prebuilt/driver/board/component/audio/lib/arm_cm4/libblisrc.a
endif

C_FILES += $(AUDIO_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/inc

ifeq ($(IC_CONFIG),mt2533)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/mt2533/inc
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/mt2533/inc/mt2533_external_dsp_profile
else
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/mt2523/inc
endif



ifeq ($(MTK_AUDIO_MP3_ENABLED), y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/mp3_codec/inc
endif

ifeq ($(MTK_AUDIO_AMR_ENABLED), y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/amr_codec/inc
endif

ifeq ($(MTK_AUDIO_AAC_DECODER_ENABLED), y)
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/aac_codec/inc
endif

ifeq ($(MTK_PROMPT_SOUND_ENABLE), y)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/audio_mixer/inc
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/prompt_control/inc
  CFLAGS += -I$(SOURCE_DIR)/prebuilt/driver/board/component/audio/inc
endif

ifeq ($(MTK_WAV_DECODER_ENABLE), y)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/wav_codec/inc
endif

ifeq ($(MTK_BT_A2DP_SOURCE_ENABLE), y)
  CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/sbc_codec/inc
  CFLAGS += -I$(SOURCE_DIR)/prebuilt/driver/board/component/audio/inc
endif
