
WIFI_HOST_SRC = middleware/MTK/wifi_service/wifi_host

C_FILES  += $(WIFI_HOST_SRC)/src/wifi_host_init.c
C_FILES  += $(WIFI_HOST_SRC)/src/wifi_host_api.c
C_FILES  += $(WIFI_HOST_SRC)/src/wifi_host_event.c
C_FILES  += $(WIFI_HOST_SRC)/src/wifi_host_os_api.c
C_FILES  += $(WIFI_HOST_SRC)/src/host_os_utils.c

#################################################################################
#include path
CFLAGS 	+= -Iinc
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/wifi_service/stub_conf/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/wifi_service/wifi_host/inc

