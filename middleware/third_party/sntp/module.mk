
SNTP_SRC = middleware/third_party/sntp

C_FILES  += 	$(SNTP_SRC)/src/sntp.c 
			 

#################################################################################
#include path
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/third_party/sntp/inc
CFLAGS	+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
