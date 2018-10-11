
.PHONY: FORCE all

MTK_FW_VERSION ?= undefined
ifeq ($(MTK_FW_VERSION),)
MTK_FW_VERSION := undefined
endif

BOARD_CONFIG ?= undefined


CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/verno/inc

# Pretend the symbol is undefined, to force linking it
LDFLAGS += -u build_date_time_str -u sw_verno_str -u hw_verno_str

# let "all" on top to be the default target.
all:

# generate verno information object and add verno.o to .elf prerequisite.
$(OUTPATH)/$(PROJ_NAME).elf: $(BUILD_DIR)/middleware/MTK/verno/verno.o

$(BUILD_DIR)/middleware/MTK/verno/verno.o: $(BUILD_DIR)/middleware/MTK/verno/verno.c
	@mkdir -p $(dir $@)
	@echo Build... $$(basename $@)
	@echo Build... $@ >> $(BUILD_LOG)
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@echo $(CC) $(CFLAGS) $@ >> $(BUILD_LOG)
	@-$(CC) $(CFLAGS) -c $< -o $@ 2>>$(ERR_LOG); \
	if [ "$$?" != "0" ]; then \
		echo "Build... $$(basename $@) FAIL"; \
		echo "Build... $@ FAIL" >> $(BUILD_LOG); \
	else \
		echo "Build... $$(basename $@) PASS"; \
		echo "Build... $@ PASS" >> $(BUILD_LOG); \
	fi;

# generate verno.c based on template.
$(BUILD_DIR)/middleware/MTK/verno/verno.c: FORCE
	@mkdir -p $(dir $@)
	@$(SOURCE_DIR)/middleware/MTK/verno/gen_verno.sh $(SOURCE_DIR)/middleware/MTK/verno/verno.template $@ $(MTK_FW_VERSION) $(BOARD_CONFIG)

FORCE:

