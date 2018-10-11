include $(SOURCE_DIR)/driver/chip/mt7686/src/GCC/module.mk

ifneq ($(wildcard $(strip $(SOURCE_DIR))/driver/chip/mt7686/src_core/),)
include $(SOURCE_DIR)/driver/chip/mt7686/src_core/GCC/module.mk
else
LIBS += $(SOURCE_DIR)/prebuilt/driver/chip/mt7686/lib/libhal_core_CM4_GCC.a
endif

ifneq ($(wildcard $(strip $(SOURCE_DIR))/driver/chip/mt7686/src_protected/),)
include $(SOURCE_DIR)/driver/chip/mt7686/src_protected/GCC/module.mk
else
LIBS += $(SOURCE_DIR)/prebuilt/driver/chip/mt7686/lib/libhal_protected_CM4_GCC.a
endif

ifeq ($(MTK_SECURE_BOOT_ENABLE),y)
ifneq ($(wildcard $(strip $(SOURCE_DIR))/milddleware/MTK/secure_boot/),)
include $(SOURCE_DIR)/middleware/MTK/secure_boot/module.mk
else
LIBS += $(SOURCE_DIR)/prebuilt/driver/chip/mt7686/lib/libsboot_CM4_GCC.a
endif
endif

