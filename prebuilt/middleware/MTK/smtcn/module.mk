ifeq ($(MTK_SMTCN_V4_ENABLE),y)
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/smtcn/lib/libmc_smtcn_protected.a
endif

ifeq ($(MTK_SMTCN_V5_ENABLE),y)
LIBS += $(SOURCE_DIR)/prebuilt/middleware/MTK/smtcn/lib/libbc_smtcn_protected.a
endif