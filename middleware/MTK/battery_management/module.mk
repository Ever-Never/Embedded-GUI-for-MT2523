
ifeq ($(IC_CONFIG), $(filter $(IC_CONFIG),mt2523 mt2523s))
###################################################

BMT_SRC = middleware/MTK/battery_management

BMT_FILES = $(BMT_SRC)/port/mt2523/src/battery_management_driver.c  \
			$(BMT_SRC)/port/mt2523/src/battery_management_meter.c   \
			$(BMT_SRC)/port/mt2523/src/battery_management_core.c

C_FILES += $(BMT_FILES) 

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/port/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/fuelgauge/inc

# include library
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/fuelgauge/lib/libfgauge.a

endif

ifeq ($(IC_CONFIG),mt2533)
###################################################

ifeq ($(MTK_EXTERNAL_PMIC), y)
# external gague files
ifeq ($(MTK_EXTERNAL_GAUGE), y)
# gague driver files
include $(SOURCE_DIR)/driver/board/component/gauge/module.mk
CFLAGS	+= -DMTK_EXTERNAL_GAUGE
endif
endif

BMT_SRC = middleware/MTK/battery_management

BMT_FILES = $(BMT_SRC)/src/battery_management.c \
			$(BMT_SRC)/port/mt2533/src/battery.c

C_FILES += $(BMT_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/battery_management/port/mt2533/inc

endif

