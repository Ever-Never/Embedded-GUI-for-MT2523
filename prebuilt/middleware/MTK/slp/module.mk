#ifeq ($(MTK_DEBUG_LEVEL),none)
#LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/slp/lib/libslp_CM4_GCC_release.a
#else
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/slp/lib/libslp_CM4_GCC.a
#endif

CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/slp/inc/
