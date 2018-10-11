

#sct key event  file
C_FILES  += middleware/MTK/key_event/common/src/sct_key_event.c
C_FILES  += middleware/MTK/key_event/common/src/sct_key_event_internal.c
C_FILES  += middleware/MTK/key_event/keypad/src/sct_keypad.c
C_FILES  += middleware/MTK/key_event/eint_key/src/sct_eint_key.c

#################################################################################
#include path
CFLAGS 	+= -Iinclude
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc

#driver include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/key_event/common/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/key_event/keypad/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/key_event/eint_key/inc

CFLAGS  += -DSCT_KEY_EVENT_ENABLE

