
###################################################
BOARD_SRC = driver/board/mt25x3_hdk
BOARD_LED_SRC = $(BOARD_SRC)/led/src
BOARD_COMPONENT_LED_SRC = driver/board/component/led/src


C_FILES += $(BOARD_LED_SRC)/bsp_led_internal.c
C_FILES += $(BOARD_COMPONENT_LED_SRC)/bsp_led.c

###################################################
# include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/led/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/led/inc

#################################################################################
CFLAGS         += -DMTK_LED_ENABLE
