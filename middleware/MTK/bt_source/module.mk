ifeq ($(MTK_BT_DUO_ENABLE), y)
# BT Source source files
BT_SOURCE_SRV_SRC = middleware/MTK/bt_source/src
BT_SOURCE_SRV_FILES = $(BT_SOURCE_SRV_SRC)/bt_source_srv_a2dp.c \
                    $(BT_SOURCE_SRV_SRC)/bt_source_srv_avrcp.c \
                    $(BT_SOURCE_SRV_SRC)/bt_source_srv.c \
                    $(BT_SOURCE_SRV_SRC)/bt_source_srv_utils.c \
                    $(BT_SOURCE_SRV_SRC)/bt_source_srv_cntx.c
C_FILES += $(BT_SOURCE_SRV_FILES)

# BT callback manager module
include $(SOURCE_DIR)/middleware/MTK/bt_callback_manager/module.mk

# BT device manager module
include $(SOURCE_DIR)/middleware/MTK/bluetooth_service/bt_device_manager_module.mk

# Include bt sink path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth_service/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bt_source/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/bluetooth/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/bt_codec/inc
CFLAGS += -mno-unaligned-access

endif
