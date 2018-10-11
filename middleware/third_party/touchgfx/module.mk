# add module dependent project source file here.
MODULE_DEP_CXX_FILES += $(APPSRC_DIR)/main_menu_screen/SwipeContainer.cpp

# let "all" on top to be the default target.
all:

# before generating c++ dependency file (.d), the touchgfx_module.mk must be generated first.
$(addprefix $(SOURCE_DIR)/,$(MODULE_DEP_CXX_FILES)): $(OUTPATH)/generated/touchgfx_module.mk


$(OUTPATH)/generated/touchgfx_module.mk:
	@$(MAKE) -C $(TOUCHGFX_PATH) $(MOD_EXTRA) SCREEN_CONFIG=$(SCREEN_CONFIG)

ifneq ($(MAKECMDGOALS),clean)
-include $(OUTPATH)/generated/touchgfx_module.mk
endif

LIBS += $(SOURCE_DIR)/middleware/third_party/touchgfx/lib/core/cortex_m4f/gcc/libtouchgfx.a