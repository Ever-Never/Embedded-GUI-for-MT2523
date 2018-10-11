###################################################
# Sources
###################################################

BLE_IAS_SOURCE = middleware/MTK/ble_ias/src

C_FILES  += $(BLE_IAS_SOURCE)/ble_ias.c

###################################################
# include path
###################################################
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/ble_ias/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_callback_manager/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
