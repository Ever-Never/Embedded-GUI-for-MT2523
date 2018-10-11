
###################################################

MDP_DRIVER_SRC = driver/board/component/mdp
MDP_DRIVER_FILES = $(MDP_DRIVER_SRC)/src/idp_core.c \
                   $(MDP_DRIVER_SRC)/src/mt6252/idp_engines_mt6252.c \
                   $(MDP_DRIVER_SRC)/src/mt6252/idp_scen_common_mt6252.c \
                   $(MDP_DRIVER_SRC)/src/mt6252/idp_scen_cp_mt6252.c \
                   $(MDP_DRIVER_SRC)/src/mt6252/idp_hw/idp_imgdma_mt6252.c \
                   $(MDP_DRIVER_SRC)/src/mt6252/idp_hw/idp_imgdma_rotdma0_mt6252.c \
                   $(MDP_DRIVER_SRC)/src/mt6252/idp_hw/idp_resz_crz_mt6252.c \
                   $(MDP_DRIVER_SRC)/src/mt6252/idp_hw/idp_resz_mt6252.c \


C_FILES += $(MDP_DRIVER_FILES)
###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(MDP_DRIVER_SRC)/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/camera/interface/camera

