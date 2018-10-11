###################################################
# Sources
###################################################

BT_DEVICE_MGR_SOURCE = middleware/MTK/bluetooth_service/src
BT_DEVICE_MGR_FILES =  $(BT_DEVICE_MGR_SOURCE)/bt_device_manager.c


C_FILES += $(BT_DEVICE_MGR_FILES)
###################################################
# include path
###################################################

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/bluetooth_service/inc
