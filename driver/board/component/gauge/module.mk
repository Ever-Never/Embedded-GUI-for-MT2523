
###################################################

GAUGE_DRIVER_SRC = driver/board/component/gauge
#GAUGE_DRIVER_FILES = $(GAUGE_DRIVER_SRC)/external_pmic_driver.c

ifeq ($(MTK_EXTERNAL_GAUGE_TYPE), max17048)
GAUGE_DRIVER_FILES += $(GAUGE_DRIVER_SRC)/max17048/max17048_driver.c
GAUGE_DRIVER_FILES += $(GAUGE_DRIVER_SRC)/max17048/max17048.c
GAUGE_DRIVER_FILES += $(GAUGE_DRIVER_SRC)/max17048/external_ntc.c

CFLAGS += -I$(SOURCE_DIR)/$(GAUGE_DRIVER_SRC)/max17048/
endif


C_FILES += $(GAUGE_DRIVER_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(GAUGE_DRIVER_SRC)/

