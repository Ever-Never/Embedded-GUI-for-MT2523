###################################################
# Sources
###################################################


EXTERNAL_DSP_SRC = driver/board/component/external_dsp
EXTERNAL_DSP_FILES = $(EXTERNAL_DSP_SRC)/src/external_dsp_driver.c \
                     $(EXTERNAL_DSP_SRC)/src/external_dsp_application.c

C_FILES += $(EXTERNAL_DSP_FILES)

###################################################
# include path
###################################################

CFLAGS += -I$(SOURCE_DIR)/$(EXTERNAL_DSP_SRC)/inc
CFLAGS += -I$(SOURCE_DIR)/chip/mt2523/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc

