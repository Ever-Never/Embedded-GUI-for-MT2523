
LIBSENSORSREEN   = project/mt2523_hdk/apps/iot_sdk_demo/src/sensor_demo

C_FILES  += $(LIBSENSORSREEN)/sensor_demo_screen.c \
            $(LIBSENSORSREEN)/sensor_demo.c

CFLAGS  += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_sdk_demo/inc/sensor_demo