-include $(SOURCE_DIR)/middleware/MTK/smtcn/src/module.mk

ifneq ($(wildcard $(strip $(SOURCE_DIR))/middleware/MTK/smtcn_protected),)
include $(SOURCE_DIR)/middleware/MTK/smtcn_protected/src_protected/GCC/module.mk
else
include $(SOURCE_DIR)/prebuilt/middleware/MTK/smtcn/module.mk
endif

