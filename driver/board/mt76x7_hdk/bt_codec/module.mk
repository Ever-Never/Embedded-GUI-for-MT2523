
BT_CODEC_SRC = driver/board/mt76x7_hdk/bt_codec

C_FILES  += $(BT_CODEC_SRC)/src/bt_a2dp_codec.c
C_FILES  += $(BT_CODEC_SRC)/src/bt_hfp_codec.c

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/$(BT_CODEC_SRC)/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc