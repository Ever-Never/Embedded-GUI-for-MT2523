
MT7686_CHIP = driver/chip/mt7686
COMPONENT = driver/board/component


CFLAGS   += $(FPUFLAGS) -DUSE_HAL_DRIVER

C_FILES  += $(MT7686_CHIP)/src/hal_i2s.c
C_FILES  += $(MT7686_CHIP)/src/hal_i2s_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_uart.c
C_FILES  += $(MT7686_CHIP)/src/hal_uart_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_log.c
C_FILES  += $(MT7686_CHIP)/src/hal_nvic.c
C_FILES  += $(MT7686_CHIP)/src/hal_nvic_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_flash_mtd_common.c
C_FILES  += $(MT7686_CHIP)/src/hal_flash_mtd_dal.c
C_FILES  += $(MT7686_CHIP)/src/hal_flash_custom_sfi.c
C_FILES  += $(MT7686_CHIP)/src/hal_flash_combo_init.c
C_FILES  += $(MT7686_CHIP)/src/hal_flash_cfi.c
C_FILES  += $(MT7686_CHIP)/src/hal_flash_custom.c
C_FILES  += $(MT7686_CHIP)/src/hal_flash_combo_nor.c
C_FILES  += $(MT7686_CHIP)/src/hal_flash_disk.c
C_FILES  += $(MT7686_CHIP)/src/hal_flash_mtd.c
C_FILES  += $(MT7686_CHIP)/src/hal_gdma.c
C_FILES  += $(MT7686_CHIP)/src/hal_gdma_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_pdma_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_emi.c
C_FILES  += $(MT7686_CHIP)/src/hal_gpt.c
C_FILES  += $(MT7686_CHIP)/src/hal_gpt_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_cache.c
C_FILES  += $(MT7686_CHIP)/src/hal_cache_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_mpu.c
C_FILES  += $(MT7686_CHIP)/src/hal_mpu_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_adc.c
C_FILES  += $(MT7686_CHIP)/src/hal_i2c_master.c
C_FILES  += $(MT7686_CHIP)/src/hal_i2c_master_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_clock.c
C_FILES  += $(MT7686_CHIP)/src/hal_dcxo.c
C_FILES  += $(MT7686_CHIP)/src/hal_msdc.c
C_FILES  += $(MT7686_CHIP)/src/hal_sd.c
C_FILES  += $(MT7686_CHIP)/src/hal_sd_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_sdio.c
C_FILES  += $(MT7686_CHIP)/src/hal_sdio_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_sdio_slave.c
C_FILES  += $(MT7686_CHIP)/src/hal_sdio_slave_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_eint.c
C_FILES  += $(MT7686_CHIP)/src/hal_eint_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_aes.c
C_FILES  += $(MT7686_CHIP)/src/hal_des.c
C_FILES  += $(MT7686_CHIP)/src/hal_md5.c
C_FILES  += $(MT7686_CHIP)/src/hal_sha.c
C_FILES  += $(MT7686_CHIP)/src/hal_spi_master.c
C_FILES  += $(MT7686_CHIP)/src/hal_spi_master_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_spi_slave.c
C_FILES  += $(MT7686_CHIP)/src/hal_spi_slave_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_rtc.c
C_FILES  += $(MT7686_CHIP)/src/hal_wdt.c
C_FILES  += $(MT7686_CHIP)/src/hal_wdt_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_trng.c
C_FILES  += $(MT7686_CHIP)/src/hal_trng_internal.c
C_FILES  += $(MT7686_CHIP)/src/hal_pmu.c
C_FILES  += $(MT7686_CHIP)/src/hal_pmu_wrap_interface.c
C_FILES  += $(MT7686_CHIP)/src/hal_pwm.c
C_FILES  += $(MT7686_CHIP)/src/hal_gpio.c
C_FILES  += $(MT7686_CHIP)/src/hal_sys.c
C_FILES  += $(MT7686_CHIP)/src/hal_dwt.c
C_FILES  += $(MT7686_CHIP)/src/hal_audio_internal_service.c
#################################################################################
#include path
CFLAGS  += -I../inc
CFLAGS  += -Iinc
CFLAGS  += -Isrc/common/include
CFLAGS 	+= -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/mt7686/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include

