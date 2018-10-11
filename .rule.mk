OS_VERSION := $(shell uname)
ifneq ($(filter MINGW%,$(OS_VERSION)),)
  $(DRV_CHIP_PATH)_EXTRA    := -j1
  $(MID_MBEDTLS_PATH)_EXTRA := -j1
endif

ifeq ($(MAKELEVEL),0)
M :=
else
M := -
endif

include $(SOURCE_DIR)/middleware/MTK/verno/module.mk

.PHONY: cleanlog

cleanlog:
ifeq ($(TARGET_PATH),)
	rm -f $(OUTPATH)/*.log
else
	@echo "trigger by build.sh, skip cleanlog"
endif


$(TARGET_LIB).a: $(C_OBJS) $(CXX_OBJS) $(S_OBJS)
	@echo Gen $(TARGET_LIB).a
	@echo Gen $(TARGET_LIB).a >>$(BUILD_LOG)
	$(Q)if [ -e "$(OUTPATH)/$@" ]; then rm -f "$(OUTPATH)/$@"; fi
	$(Q)if [ -e "$(OUTPATH)/lib/$@" ]; then rm -f "$(OUTPATH)/lib/$@"; fi
	$(Q)$(M)$(AR) -r $(OUTPATH)/$@ $(C_OBJS) $(CXX_OBJS) $(S_OBJS) >>$(BUILD_LOG) 2>>$(ERR_LOG); \
	if [ "$$?" != "0" ]; then \
		echo "MODULE BUILD $@ FAIL"; \
		echo "MODULE BUILD $@ FAIL" >> $(BUILD_LOG); \
		exit 1;\
	else \
		echo "MODULE BUILD $@ PASS"; \
		echo "MODULE BUILD $@ PASS" >> $(BUILD_LOG); \
	fi;

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo Build... $$(basename $@)
	@echo Build... $@ >> $(BUILD_LOG)
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -n "$(OVERRIDE_CFLAGS)" ]; then \
		echo $(CC) $(OVERRIDE_CFLAGS) $@ >> $(BUILD_LOG); \
		$(CC) $(OVERRIDE_CFLAGS) -c $< -o $@ 2>>$(ERR_LOG); \
	else \
		echo $(CC) $(CFLAGS) $@ >> $(BUILD_LOG); \
		$(CC) $(CFLAGS) -c $< -o $@ 2>>$(ERR_LOG); \
	fi; \
	if [ "$$?" != "0" ]; then \
		echo "Build... $$(basename $@) FAIL"; \
		echo "Build... $@ FAIL" >> $(BUILD_LOG); \
	else \
		echo "Build... $$(basename $@) PASS"; \
		echo "Build... $@ PASS" >> $(BUILD_LOG); \
	fi;

$(BUILD_DIR)/%.d: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	export D_FILE="$@"; \
	export B_NAME=`echo $$D_FILE | sed 's/\.d//g'`; \
	if [ -n "$(OVERRIDE_CFLAGS)" ]; then \
		$(CC) -MM $(OVERRIDE_CFLAGS) $< > $@.$$$$; \
	else \
		$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	fi; \
	sed 's@\(.*\)\.o@'"$$B_NAME\.o $$B_NAME\.d"'@g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo Build... $$(basename $@)
	@echo Build... $@ >> $(BUILD_LOG)
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -n "$(OVERRIDE_CFLAGS)" ]; then \
		echo $(CXX) $(OVERRIDE_CFLAGS) $@ >> $(BUILD_LOG); \
		$(CXX) $(OVERRIDE_CFLAGS) -c $< -o $@ 2>>$(ERR_LOG); \
	else \
		echo $(CXX) $(CXXFLAGS) $@ >> $(BUILD_LOG); \
		$(CXX) $(CXXFLAGS) -c $< -o $@ 2>>$(ERR_LOG); \
	fi; \
	if [ "$$?" != "0" ]; then \
		echo "Build... $$(basename $@) FAIL"; \
		echo "Build... $@ FAIL" >> $(BUILD_LOG); \
	else \
		echo "Build... $$(basename $@) PASS"; \
		echo "Build... $@ PASS" >> $(BUILD_LOG); \
	fi;

$(BUILD_DIR)/%.d: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	export D_FILE="$@"; \
	export B_NAME=`echo $$D_FILE | sed 's/\.d//g'`; \
	if [ -n "$(OVERRIDE_CFLAGS)" ]; then \
		$(CXX) -MM $(OVERRIDE_CFLAGS) $< > $@.$$$$; \
	else \
		$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	fi; \
	sed 's@\(.*\)\.o@'"$$B_NAME\.o $$B_NAME\.d"'@g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.s
	@mkdir -p $(dir $@)
	@echo Build... $$(basename $@)
	@echo Build... $@ >> $(BUILD_LOG)
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -n "$(OVERRIDE_CFLAGS)" ]; then \
		$(CC) $(OVERRIDE_CFLAGS) -c $< -o $@; \
	else \
		$(CC) $(CFLAGS) -c $< -o $@; \
	fi; \
	if [ "$$?" != "0" ]; then \
		echo "Build... $$(basename $@) FAIL"; \
		echo "Build... $@ FAIL" >> $(BUILD_LOG); \
	else \
		echo "Build... $$(basename $@) PASS"; \
		echo "Build... $@ PASS" >> $(BUILD_LOG); \
	fi;

ifneq ($(MAKECMDGOALS),clean)
-include $(C_OBJS:.o=.d)
-include $(CXX_OBJS:.o=.d)
endif
