###################################################
# Sources
###################################################

AUDIO_MANAGER_SRC= middleware/MTK/audio_manager/src

AUDIO_MANAGER_SRV_FILES  = $(AUDIO_MANAGER_SRC)/audio_src_srv.c \
                                                            $(AUDIO_MANAGER_SRC)/audio_src_srv_state_mgr.c
                                                            
C_FILES += $(AUDIO_MANAGER_SRV_FILES)
###################################################
# include path
###################################################
CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/audio_manager/inc

