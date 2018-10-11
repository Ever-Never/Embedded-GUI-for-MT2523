
ifeq ($(IC_CONFIG), mt7682)
WIFI_LIB_FOLDER=wifi_inic
else ifeq ($(IC_CONFIG), mt7686)
WIFI_LIB_FOLDER=wifi_inic
else ifeq ($(IC_CONFIG), mt5932)
WIFI_LIB_FOLDER=wifi_inic
else ifeq ($(IC_CONFIG), mt7687)
WIFI_LIB_FOLDER=wifi_supp
else ifeq ($(IC_CONFIG), mt7697)
WIFI_LIB_FOLDER=wifi_supp
endif


$(info Prebuilt WIFI_LIB_FOLDER=$(WIFI_LIB_FOLDER))

LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/wifi_service/combo/lib/$(WIFI_LIB_FOLDER)/libwifi.a

