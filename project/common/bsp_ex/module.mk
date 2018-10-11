
BSPEXT_SRC = project/common/bsp_ex


C_FILES  += $(BSPEXT_SRC)/src/lp_ex_sleep.c

#################################################################################
#include path
CFLAGS 	+= -I./inc
CFLAGS 	+= -I$(SOURCE_DIR)/project/common/bsp_ex/inc
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/wifi_service/combo/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/ping/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc

ifeq ($(MTK_MINISUPP_ENABLE),y)
CFLAGS  += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/minisupp/inc
endif
