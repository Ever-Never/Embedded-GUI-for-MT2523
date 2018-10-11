
BT_CODEC_SRC = driver/board/linkit7697_hdk/bt_codec

C_FILES  += $(BT_CODEC_SRC)/src/bt_a2dp_codec.c
C_FILES  += $(BT_CODEC_SRC)/src/bt_hfp_codec.c

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc