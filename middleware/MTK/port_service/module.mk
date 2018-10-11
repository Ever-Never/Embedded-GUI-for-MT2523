
FEATURE ?= feature.mk
include $(FEATURE)

###################################################
# Sources
PORT_SERVICE_SRC = middleware/MTK/port_service
PORT_SERVICE_FILES = $(PORT_SERVICE_SRC)/src/serial_port_uart.c \
             $(PORT_SERVICE_SRC)/src/serial_port_usb.c \
             $(PORT_SERVICE_SRC)/src/serial_port.c \
			 $(PORT_SERVICE_SRC)/src/serial_port_para.c

C_FILES += $(PORT_SERVICE_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/port_service/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/usb/inc
CFLAGS += -D__MTK_TARGET__
