
WIFI_STUB_CONF_SRC = middleware/MTK/wifi_service/stub_conf


ifeq ($(MTK_WIFI_STUB_CONF_SPI_ENABLE),y)
  ifeq ($(MTK_WIFI_STUB_CONF_SPIM_ENABLE),y)
    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/wfcm_spi.c
    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/wfcm_stub.c
    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/xboot_over_spi.c
  else
    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/wfc_stub.c
    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/wfc_spi.c
  endif
else
  ifeq ($(MTK_WIFI_STUB_CONF_SDIO_MSDC_ENABLE),y)
    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/wfcm_sdio.c
    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/wfcm_stub.c
    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/xboot_over_sdio.c
  else
    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/wfc_stub.c
    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/wfc_sdio.c

    ifeq ($(MTK_WFC_AT_SLAVE_ENABLE),y)
      C_FILES  += $(WIFI_STUB_CONF_SRC)/src/wfc_at_cmd.c
    endif

    C_FILES  += $(WIFI_STUB_CONF_SRC)/src/wfc_at_api.c
  endif
endif


#################################################################################
#include path
CFLAGS 	+= -Iinc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/wifi_service/combo/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/wifi_service/stub_conf/inc

