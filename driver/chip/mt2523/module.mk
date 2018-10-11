include $(SOURCE_DIR)/driver/chip/mt2523/src/GCC/module.mk

ifneq ($(wildcard $(strip $(SOURCE_DIR))/driver/chip/mt2523/src_core/),)
include $(SOURCE_DIR)/driver/chip/mt2523/src_core/GCC/module.mk
else
LIBS += $(SOURCE_DIR)/prebuilt/driver/chip/mt2523/lib/libhal_core_CM4_GCC.a
endif

ifneq ($(wildcard $(strip $(SOURCE_DIR))/driver/chip/mt2523/src_protected/),)
include $(SOURCE_DIR)/driver/chip/mt2523/src_protected/GCC/module.mk
else
LIBS += $(SOURCE_DIR)/prebuilt/driver/chip/mt2523/lib/libhal_protected_CM4_GCC.a
endif
