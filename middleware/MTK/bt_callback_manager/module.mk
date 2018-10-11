###################################################
# Sources
###################################################

BT_CMGR_SOURCE = middleware/MTK/bt_callback_manager/src
BT_CMGR_FILES =  $(BT_CMGR_SOURCE)/bt_callback_manager.c


C_FILES += $(BT_CMGR_FILES)
###################################################
# include path
###################################################

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bt_callback_manager/inc
