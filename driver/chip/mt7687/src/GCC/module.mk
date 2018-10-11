
MT7687_CHIP = driver/chip/mt7687
MT7686_CHIP = driver/chip/mt7686

CFLAGS   += $(FPUFLAGS) -DUSE_HAL_DRIVER
CFLAGS   += -DDATA_PATH_87
			 
C_FILES  += $(MT7687_CHIP)/src/common/cos_api.c              \
            $(MT7687_CHIP)/src/common/dma.c                  \
            $(MT7687_CHIP)/src/common/flash_sfc.c            \
            $(MT7687_CHIP)/src/common/gpt.c                  \
            $(MT7687_CHIP)/src/common/hal_I2C.c              \
            $(MT7687_CHIP)/src/common/hal_i2s_internal.c     \
            $(MT7687_CHIP)/src/common/low_hal_irtx.c         \
            $(MT7687_CHIP)/src/common/low_hal_pwm.c          \
            $(MT7687_CHIP)/src/common/hal_gpc.c              \
            $(MT7687_CHIP)/src/common/hal_gpio_7687.c        \
            $(MT7687_CHIP)/src/common/hal_irrx.c             \
            $(MT7687_CHIP)/src/common/hal_lp.c               \
            $(MT7687_CHIP)/src/common/hal_pcnt.c             \
            $(MT7687_CHIP)/src/common/hal_pinmux.c           \
            $(MT7687_CHIP)/src/common/hal_spim.c             \
            $(MT7687_CHIP)/src/common/hal_sys.c              \
            $(MT7687_CHIP)/src/common/hal_trng.c             \
            $(MT7687_CHIP)/src/common/hal_uid.c              \
            $(MT7687_CHIP)/src/common/i2c.c                  \
            $(MT7687_CHIP)/src/common/ir_tx.c                \
            $(MT7687_CHIP)/src/common/low_hal_gpio.c         \
            $(MT7687_CHIP)/src/common/low_hal_irrx.c         \
            $(MT7687_CHIP)/src/common/low_hal_rtc.c          \
            $(MT7687_CHIP)/src/common/low_hal_wdt.c          \
            $(MT7687_CHIP)/src/common/nvic.c                 \
            $(MT7687_CHIP)/src/common/pinmux.c               \
            $(MT7687_CHIP)/src/common/pwm.c                  \
            $(MT7687_CHIP)/src/common/sfc.c                  \
            $(MT7687_CHIP)/src/common/spi_flash.c            \
            $(MT7687_CHIP)/src/common/spim.c                 \
            $(MT7687_CHIP)/src/common/timer.c                \
            $(MT7687_CHIP)/src/common/top.c                  \
            $(MT7687_CHIP)/src/common/uart.c                 \
            $(MT7687_CHIP)/src/hal_log.c                     \
            $(MT7687_CHIP)/src/hal_adc.c                     \
            $(MT7687_CHIP)/src/hal_aes.c                     \
            $(MT7687_CHIP)/src/hal_cache.c                   \
            $(MT7687_CHIP)/src/hal_cache_internal.c          \
            $(MT7687_CHIP)/src/hal_des.c                     \
            $(MT7687_CHIP)/src/hal_eint.c                    \
            $(MT7687_CHIP)/src/hal_nvic.c                    \
            $(MT7687_CHIP)/src/hal_nvic_internal.c           \
            $(MT7687_CHIP)/src/hal_flash.c                   \
            $(MT7687_CHIP)/src/hal_gpio.c                    \
            $(MT7687_CHIP)/src/hal_i2c_master.c              \
            $(MT7687_CHIP)/src/hal_i2s.c                     \
            $(MT7687_CHIP)/src/hal_irtx.c                    \
            $(MT7687_CHIP)/src/hal_gdma.c                    \
            $(MT7687_CHIP)/src/hal_gpt.c                     \
            $(MT7687_CHIP)/src/hal_md5.c                     \
            $(MT7687_CHIP)/src/hal_pwm.c                     \
            $(MT7687_CHIP)/src/hal_rtc.c                     \
            $(MT7687_CHIP)/src/hal_sha.c                     \
            $(MT7687_CHIP)/src/hal_spi_master.c              \
            $(MT7687_CHIP)/src/hal_spi_slave.c               \
            $(MT7687_CHIP)/src/hal_uart.c                    \
            $(MT7687_CHIP)/src/hal_audio_internal_service.c  \
            $(MT7687_CHIP)/src/hal_wdt.c                     \
            $(MT7687_CHIP)/src/hal_misc.c                    \
            $(MT7687_CHIP)/src/hal_dwt.c

#################################################################################
#include path
CFLAGS  += -Iinc
CFLAGS  += -I../inc
CFLAGS  += -Isrc/common/include
CFLAGS 	+= -I$(SOURCE_DIR)/kernel/service/inc

# workaround for hal need to include board's external flash
ifeq ($(DRV_BOARD_CONFIG),)
-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk
endif

CFLAGS 	+= -I$(SOURCE_DIR)/driver/board/$(DRV_BOARD_CONFIG)/external_flash/inc

include $(SOURCE_DIR)/driver/board/$(DRV_BOARD_CONFIG)/external_flash/module.mk

