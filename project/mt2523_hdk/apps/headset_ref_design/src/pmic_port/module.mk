# PMIC Porting source files
###################################################
        
ifeq ($(MTK_EXTERNAL_PMIC_TYPE), MAX14745)
PMIC_PORT_FILES = $(APP_PATH_SRC)/pmic_port/max14745/external_pmic_port.c
endif
        
C_FILES += $(PMIC_PORT_FILES)

###################################################



