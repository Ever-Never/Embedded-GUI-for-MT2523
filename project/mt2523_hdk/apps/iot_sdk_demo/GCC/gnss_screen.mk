
LIBGNSSSREEN   = project/mt2523_hdk/apps/iot_sdk_demo/src/gnss_screen

C_FILES  += $(LIBGNSSSREEN)/gnss_app.c    \
            $(LIBGNSSSREEN)/epo_demo.c    \
            $(LIBGNSSSREEN)/gnss_demo_screen.c    \
            $(LIBGNSSSREEN)/gnss_timer.c    \
            $(LIBGNSSSREEN)/gnss_ring_buffer.c


CFLAGS  += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_sdk_demo/inc/gnss_screen
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/gnss/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/lcm/ST7789H2
