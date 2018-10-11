
###################################################

PMIC_DRIVER_SRC = driver/board/component/pmic
PMIC_DRIVER_FILES = $(PMIC_DRIVER_SRC)/external_pmic_driver.c

ifeq ($(MTK_EXTERNAL_PMIC_TYPE), MAX14745)
PMIC_DRIVER_FILES += $(PMIC_DRIVER_SRC)/MAX14745/pmic_max14745.c
CFLAGS += -I$(SOURCE_DIR)/$(PMIC_DRIVER_SRC)/MAX14745/
endif

ifeq ($(MTK_EXTERNAL_PMIC_TYPE), MT6327)
PMIC_DRIVER_FILES += $(PMIC_DRIVER_SRC)/MT6327/pmic_mt6327.c
CFLAGS += -I$(SOURCE_DIR)/$(PMIC_DRIVER_SRC)/MT6327/
endif

C_FILES += $(PMIC_DRIVER_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(PMIC_DRIVER_SRC)/

