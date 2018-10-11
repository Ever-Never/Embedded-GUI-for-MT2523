SMTCN_SRC := middleware/MTK/smtcn/src

C_FILES  += $(SMTCN_SRC)/smt_api.c \
			$(SMTCN_SRC)/ops_config.c \
            $(SMTCN_SRC)/smt_conn.c


ifeq ($(MTK_SMTCN_V5_ENABLE),y)
C_FILES  += $(SMTCN_SRC)/bsmtcn_ops.c
endif

ifeq ($(MTK_SMTCN_V4_ENABLE),y)
C_FILES  += $(SMTCN_SRC)/elian.c
endif

#################################################################################
#include path
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/smtcn/inc
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/smtcn/inc/internal
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc 
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/wifi_service/combo/inc
CFLAGS 	+= -I$(SOURCE_DIR)/driver/board/mt76x7_hdk/inc
