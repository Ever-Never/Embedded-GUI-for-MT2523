BT_NOTIFY_SRC := middleware/MTK/bt_notify/src

C_FILES  += $(BT_NOTIFY_SRC)/bt_notify_conversion.c \
            $(BT_NOTIFY_SRC)/bt_notify_data_parse.c \
            $(BT_NOTIFY_SRC)/bt_notify_list.c \
            $(BT_NOTIFY_SRC)/xml_main.c \
            $(BT_NOTIFY_SRC)/bt_notify_task.c \
            $(BT_NOTIFY_SRC)/dogp/ble_dogp_adp_service.c \
            $(BT_NOTIFY_SRC)/dogp/ble_dogp_common.c \
            $(BT_NOTIFY_SRC)/dogp/ble_dogp_service.c


ifeq ($(MTK_BLE_ONLY_ENABLE), y)
C_FILES +=  $(BT_NOTIFY_SRC)/bt_spp_dummy.c
endif
#################################################################################
#include path
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/bt_notify/inc
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/bt_notify/inc/dogp
CFLAGS 	+= -I$(SOURCE_DIR)/prebuilt/middleware/MTK/bluetooth/inc
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc 

ifneq ($(wildcard $(strip $(SOURCE_DIR))/middleware/MTK/bt_notify_protected/),)
include $(SOURCE_DIR)/middleware/MTK/bt_notify_protected/src_protected/GCC/module.mk
else
include $(SOURCE_DIR)/prebuilt/middleware/MTK/bt_notify/module.mk
endif

CFLAGS += -DMTK_BT_NOTIFY_ENABLE
