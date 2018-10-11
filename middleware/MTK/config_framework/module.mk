###################################################
# Sources
###################################################
CFLAGS	+= -D__CFW_CONFIG_SUPPORT__
CFW_SOURCE = middleware/MTK/config_framework/src
CFW_FILES =  $(CFW_SOURCE)/cfw.c
CFW_FILES +=  $(CFW_SOURCE)/cfw_data.c
CFW_FILES +=  $(CFW_SOURCE)/at_command_cfw.c


C_FILES += $(CFW_FILES)
###################################################
# include path
###################################################

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/config_framework/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/config_framework/src
