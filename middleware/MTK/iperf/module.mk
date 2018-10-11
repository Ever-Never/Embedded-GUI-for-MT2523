
IPERF_SRC = middleware/MTK/iperf

C_FILES  += 	$(IPERF_SRC)/src/iperf_task.c \
		$(IPERF_SRC)/src/iperf_cli.c
			 

#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/iperf/inc
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/connsys/inc
