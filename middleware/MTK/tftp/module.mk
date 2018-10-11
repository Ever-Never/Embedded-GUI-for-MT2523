
TFTP_SRC  = middleware/MTK/tftp

C_FILES  += $(TFTP_SRC)/src/port.c \
            $(TFTP_SRC)/src/tftp.c \
            $(TFTP_SRC)/src/tftpc.c
			 

#################################################################################
#include path
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/tftp/inc
CFLAGS	+= -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include 
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt$(PRODUCT_VERSION)/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc 
