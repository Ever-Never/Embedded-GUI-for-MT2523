
BOARD_SRC = driver/board/mt25x3_hdk
COMPONENT_SRC = driver/board/component

#ctp driver source
C_FILES  += $(COMPONENT_SRC)/touch_panel/common/src/bsp_ctp.c
C_FILES  += $(COMPONENT_SRC)/touch_panel/common/src/ctp.c
C_FILES  += $(COMPONENT_SRC)/touch_panel/common/src/ctp_i2c.c
C_FILES  += $(COMPONENT_SRC)/touch_panel/ctp_goodix_gt9xxx/gt9137/ctp_goodix_gt9xxx.c
C_FILES  += $(COMPONENT_SRC)/touch_panel/ctp_ite_it7xxx/it7258/ctp_ite_it7xxx.c
C_FILES  += $(COMPONENT_SRC)/touch_panel/ctp_ite_it7xxx/it7258/ctp_ite_it7xxx_download.c
C_FILES  += $(BOARD_SRC)/touch_panel/it7258/touch_panel_custom_ite.c
C_FILES  += $(BOARD_SRC)/touch_panel/gt9137/touch_panel_custom_goodix.c

#################################################################################
#include path
CFLAGS 	+= -Iinclude
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/mt2523/inc
CFLAGS  += -I$(BOARD_SRC)/lcd

#ctp driver include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/common
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/touch_panel/common/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/touch_panel/ctp_goodix_gt9xxx/gt9137
CFLAGS  += -I$(SOURCE_DIR)/driver/board/component/touch_panel/ctp_ite_it7xxx/it7258

#board confg include path
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/touch_panel/gt9137
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/touch_panel/it7258
CFLAGS  += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/touch_panel/config
##
## MTK_CTP_ENABLE
## Brief:       This option is to enable BSP CTP APIs under driver/board/component/touch_panel folder.
## Usage:       The MTK_CTP_ENABLE compile option will be defined, the sources and header files under driver/board/component/touch_panel,
##				driver/board/component/common, driver/board/<board_name>/touch_panel will be included by driver/board/<board_name>/module.mk.
## Path:        driver/board/component/touch_panel, driver/board/component/common, driver/board/<board_name>/touch_panel.
## Dependency:  Must enable HAL_GPT_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED, HAL_EINT_MODULE_ENABLED, HAL_NVIC_MODULE_ENABLED, HAL_GPIO_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.
## Notice:      The default implementation of CTP under driver/board/<board_name>/touch_panel is none.
##              Please configure the GPIO for CTP by EPT tool. If you want to modify I2C parametr, please refer to driver/board/<board_name>/touch_panel.
## Relative doc:None
##
CFLAGS         += -DMTK_CTP_ENABLE

