###################################################
# Sources
###################################################

BLE_BAS_SOURCE = middleware/MTK/ble_bas/src

C_FILES  += $(BLE_BAS_SOURCE)/ble_bas.c

###################################################
# include path
###################################################
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/ble_bas/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_callback_manager/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
