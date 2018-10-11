
DHCPD_SRC = middleware/MTK/dhcpd

C_FILES  += 	$(DHCPD_SRC)/src/dhcpd.c
			 

#################################################################################
#include path
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/dhcpd/inc
CFLAGS	+= -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
