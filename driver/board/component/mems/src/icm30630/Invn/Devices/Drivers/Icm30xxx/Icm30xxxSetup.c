/*
 * ________________________________________________________________________________________________________
 * Copyright (c) 2015-2015 InvenSense Inc. All rights reserved.
 *
 * This software, related documentation and any modifications thereto (collectively “Software”) is subject
 * to InvenSense and its licensors' intellectual property rights under U.S. and international copyright
 * and other intellectual property rights laws.
 *
 * InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
 * and any use, reproduction, disclosure or distribution of the Software without an express license agreement
 * from InvenSense is strictly prohibited.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE SOFTWARE IS
 * PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * INVENSENSE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 * ________________________________________________________________________________________________________
 */

#include "Icm30xxxSetup.h"
#include "Icm30xxx.h"

#include "Invn/EmbUtils/DataConverter.h"
#include "Invn/EmbUtils/Message.h"

#include <assert.h>

/* Identification related functions */

int inv_icm30xxx_get_whoami(struct inv_icm30xxx * s, uint8_t * whoami)
{
	return inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_WHOAMI_B0, whoami);
}

/* Power related functions */

void inv_icm30xxx_lp_inhibit_reset(struct inv_icm30xxx * s)
{
	s->lp_inhibit.cnt      = 0;
	s->lp_inhibit.lpen_val = 0;
}

int	inv_icm30xxx_lp_inhibit(struct inv_icm30xxx * s, inv_bool_t enable)
{
	int rc;

	assert(s->lp_inhibit.cnt >= 0);

	if(enable) {
		++s->lp_inhibit.cnt;

		if(s->lp_inhibit.cnt == 1) {
			if((rc = inv_icm30xxx_get_reg_bits(s, ICM30XXX_REG_BANK_0,
					ICM30XXX_PWR_MGMT_1_B0, ICM30XXX_LP_EN_BIT,
					&s->lp_inhibit.lpen_val)) != 0) {
				return rc;
			}
			if(s->lp_inhibit.lpen_val) {
				if((rc = inv_icm30xxx_clear_reg_bits(s, ICM30XXX_REG_BANK_0,
						ICM30XXX_PWR_MGMT_1_B0, ICM30XXX_LP_EN_BIT)) != 0) {
					return rc;
				}
			}
		}
	} else {
		--s->lp_inhibit.cnt;

		if(s->lp_inhibit.cnt == 0) {
			if(s->lp_inhibit.lpen_val) {
				if((rc = inv_icm30xxx_set_reg_bits(s, ICM30XXX_REG_BANK_0,
						ICM30XXX_PWR_MGMT_1_B0, ICM30XXX_LP_EN_BIT)) != 0) {
					return rc;
				}
			}
		}
	}

	return 0;
}

int inv_icm30xxx_lp_enable(struct inv_icm30xxx * s, inv_bool_t enable)
{
	if(s->lp_inhibit.cnt == 0) {
		int rc;
		if(enable) {
			if((rc = inv_icm30xxx_set_reg_bits(s, ICM30XXX_REG_BANK_0,
					ICM30XXX_PWR_MGMT_1_B0, ICM30XXX_LP_EN_BIT)) != 0) {
				return rc;
			}
		} else {
			if((rc = inv_icm30xxx_clear_reg_bits(s, ICM30XXX_REG_BANK_0,
					ICM30XXX_PWR_MGMT_1_B0, ICM30XXX_LP_EN_BIT)) != 0) {
				return rc;
			}
		}
	}

	return 0;
}

int inv_icm30xxx_set_deep_sleep(struct inv_icm30xxx * s)
{
	return inv_icm30xxx_set_reg_bits(s, ICM30XXX_REG_BANK_0, ICM30XXX_PWR_MGMT_1_B0,
			ICM30XXX_DEEP_SLEEP_EN_BIT | ICM30XXX_SLEEP_REQ_BIT);
}

int inv_icm30xxx_set_sleep(struct inv_icm30xxx * s)
{
	int rc;
	uint8_t reg_value;

	if((rc = inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0, ICM30XXX_PWR_MGMT_1_B0,
			&reg_value)) != 0) {
		return rc;
	}

	reg_value &= ~ICM30XXX_SLEEP_REQ_BIT;
	reg_value |= ICM30XXX_DEEP_SLEEP_EN_BIT;

	if(s->lp_inhibit.cnt == 0) {
		reg_value |= ICM30XXX_LP_EN_BIT;
	}

	if((rc = inv_icm30xxx_write_reg_one(s, ICM30XXX_REG_BANK_0, ICM30XXX_PWR_MGMT_1_B0,
			reg_value)) != 0) {
		return rc;
	}

	return 0;
}

int inv_icm30xxx_wake_up(struct inv_icm30xxx * s)
{
	if(inv_icm30xxx_clear_reg_bits(s, ICM30XXX_REG_BANK_0, ICM30XXX_PWR_MGMT_1_B0,
			ICM30XXX_SLEEP_REQ_BIT | ICM30XXX_DEEP_SLEEP_EN_BIT) != 0) {
		return INV_ERROR_TRANSPORT;
	}

	inv_icm30xxx_sleep(10);

	return 0;
}

int inv_icm30xxx_power_off(struct inv_icm30xxx * s)
{
	return inv_icm30xxx_lp_enable(s, false);
}

int inv_icm30xxx_get_power_mgmt(struct inv_icm30xxx * s, uint8_t * pwr)
{
	return inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_PWR_MGMT_1_B0, pwr);
}

int inv_icm30xxx_set_power_mgmt(struct inv_icm30xxx * s, uint8_t pwr)
{
	return inv_icm30xxx_write_reg_one(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_PWR_MGMT_1_B0, pwr);
}


/* Setup/boot related functions */

int inv_icm30xxx_soft_reset(struct inv_icm30xxx * s)
{
	int rc;
	int timeout = 2000; /* 2s */

	if((rc = inv_icm30xxx_write_reg_one(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_PWR_MGMT_1_B0, ICM30XXX_SOFT_RESET_BIT)) != 0) {
		return rc;
	}

	/* Small delay before accessing the SPI interface after a soft reset */
	inv_icm30xxx_sleep(10);

	/* wait for reset to be complete */
	do {
		uint8_t reg_val;

		if((rc = inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0,
				ICM30XXX_PWR_MGMT_1_B0, &reg_val)) != 0) {
			return rc;
		}

		if(!(reg_val & ICM30XXX_SOFT_RESET_BIT))
			break;

		inv_icm30xxx_sleep(100);
		timeout -= 100;

		if(timeout <= 0)
			return INV_ERROR_TIMEOUT;
	} while(1);

	return 0;
}

int inv_icm30xxx_start_m0(struct inv_icm30xxx * s, inv_bool_t enable_dbg)
{
	int rc;
	uint8_t reg_value = ICM30XXX_M0_EN_BIT;

	if((rc = inv_icm30xxx_wake_up(s)) != 0)
		return rc;

	if((rc = inv_icm30xxx_lp_enable(s, false)) != 0)
		return rc;

	if(inv_icm30xxx_serif_is_spi(&s->serif))
		reg_value |= ICM30XXX_I2C_IF_DIS_BIT;

	if((rc = inv_icm30xxx_set_reg_bits(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_MOD_EN_B0, reg_value)) != 0) {
		return INV_ERROR_TRANSPORT;
	}

	if(enable_dbg) {
		/* set SKU_TYPE and DAP_EN to TEST_MODES register to enable JTAG */
		if((rc != inv_icm30xxx_write_reg_one(s, ICM30XXX_REG_BANK_0,
				ICM30XXX_TEST_MODES_B0, ICM30XXX_TEST_MODES_SKU_TYPE_BIT
				| ICM30XXX_TEST_MODES_DAP_EN_BIT)) != 0) {
			return INV_ERROR_TRANSPORT;
		}
	}

	if((rc = inv_icm30xxx_lp_enable(s, true)) != 0)
		return rc;

	inv_icm30xxx_sleep(100); /* wait some time to allow FW to perform basic
	                            initialization and get ready for action! */

	return 0;
}

int inv_icm30xxx_start_zsp(struct inv_icm30xxx * s)
{
	assert(s->variant == INV_ICM30XXX_VARIANT_BERLIN);

	return inv_icm30xxx_write_reg_one(s, ICM30XXX_REG_BANK_0, BERLIN_ZSP_SCONFIG_1_B0,
			BERLIN_IBOOT_BIT | BERLIN_INT0_BIT | BERLIN_ZSPSYSRSTN_BIT |
			BERLIN_ZSPCORERSTN_BIT | BERLIN_HRESETN_BIT);
}

/* SRAM/FLASH memory related functions */

int inv_icm30xxx_read_mem(struct inv_icm30xxx * s, uint32_t addr,
		uint8_t * buf, uint32_t len)
{
	int rc;
	uint8_t mem_addr_buf[4];
	uint32_t bytes_read = 0;

	if((rc = inv_icm30xxx_lp_inhibit(s, true)) != 0)
		return rc;

	inv_dc_int32_to_big8(addr, mem_addr_buf);

	/* Set memory address we whish to access */
	if((rc = inv_icm30xxx_write_reg(s, ICM30XXX_REG_BANK_0, ICM30XXX_MEM_ADDR_SEL_0_B0,
			mem_addr_buf, sizeof(mem_addr_buf))) != 0)
		return rc;

	/* Read SRAM */
	while(bytes_read < len) {
		uint32_t this_len = (len - bytes_read);

		if(this_len > inv_icm30xxx_serif_max_read(&s->serif)) {
			this_len = inv_icm30xxx_serif_max_read(&s->serif);
		}

		if((rc = inv_icm30xxx_read_reg(s, ICM30XXX_REG_BANK_0, ICM30XXX_MEM_R_W_B0,
				&buf[bytes_read], this_len)) != 0) {
			return rc;
		}

		bytes_read += this_len;
	}

	if((rc = inv_icm30xxx_lp_inhibit(s, false)) != 0)
		return rc;

	return 0;
}

int inv_icm30xxx_write_mem(struct inv_icm30xxx * s, uint32_t addr,
		const uint8_t * buf, uint32_t len)
{
	int rc;
	uint8_t mem_addr_buf[4];
	uint32_t bytes_written = 0;

	if((rc = inv_icm30xxx_lp_inhibit(s, true)) != 0)
		return rc;

	inv_dc_int32_to_big8(addr, mem_addr_buf);

	/* Set memory address we whish to access */
	if((rc = inv_icm30xxx_write_reg(s, ICM30XXX_REG_BANK_0, ICM30XXX_MEM_ADDR_SEL_0_B0,
			mem_addr_buf, sizeof(mem_addr_buf))) != 0)
		return rc;

	/* Write SRAM */
	while(bytes_written < len) {
		uint32_t this_len = (len - bytes_written);

		if(this_len > inv_icm30xxx_serif_max_write(&s->serif)) {
			this_len = inv_icm30xxx_serif_max_write(&s->serif);
		}

		if((rc = inv_icm30xxx_write_reg(s, ICM30XXX_REG_BANK_0, ICM30XXX_MEM_R_W_B0,
				&buf[bytes_written], this_len)) != 0) {
			return rc;
		}

		bytes_written += this_len;
	}

	if((rc = inv_icm30xxx_lp_inhibit(s, false)) != 0)
		return rc;

	return 0;
}

int inv_icm30xxx_initiate_dma_transfer(struct inv_icm30xxx * s, uint8_t channel,
		uint32_t source_addr, uint32_t dest_addr, uint32_t len)
{
	int rc;

	/* NOTE DMA Register descriptions are found in Section 5:
	   DMA Registers of the Icm30xxx A1 Register Map document */

	/* DNA registers are written LSB first */
	// TODO verify that these are the proper values for all DMAs
	const uint8_t dma_ctr_reg_bytes[4] = {
		ICM30XXX_DMA_CONTROL_REGISTER_BYTE_0_WORD_SIZE_BITS,
		ICM30XXX_DMA_CONTROL_REGISTER_BYTE_1_MAX_BURST_BITS,
		(ICM30XXX_DMA_CONTROL_REGISTER_BYTE_2_CHG_BIT |
			ICM30XXX_DMA_CONTROL_REGISTER_BYTE_2_STRT_BIT),
		(ICM30XXX_DMA_CONTROL_REGISTER_BYTE_3_INT_BIT |
			ICM30XXX_DMA_CONTROL_REGISTER_BYTE_3_TC_BIT |
			ICM30XXX_DMA_CONTROL_REGISTER_BYTE_3_SINC_BIT |
			ICM30XXX_DMA_CONTROL_REGISTER_BYTE_3_DINC_BIT)
	};

	const uint32_t dma_addr = ICM30XXX_DMA_CH_0_START_ADDR +
			channel * ICM30XXX_DMA_CHANNEL_ADDRESS_OFFSET;

	uint8_t dma_addr_buf[8] = {0};
	uint8_t dma_len_buf[4] = {0};
	uint8_t int_status;
	int timeout = 5000; /* 5s */

    /* write source and dest addresses to dma registers */
	inv_dc_int32_to_little8(source_addr,dma_addr_buf);	   /* put source address LSB first */
	inv_dc_int32_to_little8(dest_addr, &dma_addr_buf[4]); /* append dest address LSB first */

	if((rc = inv_icm30xxx_write_mem(s, dma_addr, dma_addr_buf, 8)) != 0) {
        return rc;
	}

	/* write the length (dma_addr + 0x0C) */
	inv_dc_int32_to_little8(len, dma_len_buf);

	if((rc = inv_icm30xxx_write_mem(s, dma_addr+ICM30XXX_DMA_TRANSFER_COUNT_OFFSET,
			dma_len_buf, 4)) != 0) {
		return rc;
	}

	/* write the dma control register (NOTE: must write the 3rd byte (arm DMA) last) */

	/* the following writes could more efficiently be done individually...
	    per Andy's code, just need to write MEM_ADDR_SEL_3 and then the bytes */
	if((rc = inv_icm30xxx_write_mem(s, dma_addr + ICM30XXX_DMA_CONTROL_REGISTER_BYTE_0_OFFSET,
			&dma_ctr_reg_bytes[0], 2)) != 0) {
		return rc;
	}

	if((rc = inv_icm30xxx_write_mem(s, dma_addr + ICM30XXX_DMA_CONTROL_REGISTER_BYTE_3_OFFSET,
			&dma_ctr_reg_bytes[3], 1)) != 0) {
		return rc;
	}

	if((rc = inv_icm30xxx_write_mem(s, dma_addr + ICM30XXX_DMA_CONTROL_REGISTER_BYTE_2_OFFSET,
			&dma_ctr_reg_bytes[2], 1)) != 0) {
		return rc;
	}

	/* verify DMA is working */
	do {
		inv_icm30xxx_sleep(10);
        timeout -= 10;

        if(timeout <= 0)
			return INV_ERROR_TIMEOUT;

		if((rc = inv_icm30xxx_read_mem(s, ICM30XXX_DMA_INTERRUPT_REGISTER,
				&int_status, 1)) != 0) {
			return rc;
		}
	} while(!(int_status & (1 << channel)));

	return 0;
}

int inv_icm30xxx_flash_erase(struct inv_icm30xxx * s, uint8_t page)
{
	int rc;
	int timeout = 10000; /* 10s */

	/* there are 32 2K pages of flash, addressed with bits 4:0 of
	   the FLASH_ERASE register.
       in the following program, specifying page > 32 will erase all.
       otherwise, just a page will be erased (page erase TO DO) */

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Attempting to erase FLASH page 0x%x...", page);

	if((rc = inv_icm30xxx_lp_inhibit(s, true)) != 0) {
		INV_MSG(INV_MSG_LEVEL_DEBUG, "Inhibiting LP_EN returned %d", rc);
		return rc;
	}

    /* set flash_ifrn_dis=1 in flash config.  This protects the OTP? */
	if((rc = inv_icm30xxx_set_reg_bits(s, ICM30XXX_REG_BANK_0, ICM30XXX_FLASH_CFG_B0,
			ICM30XXX_FLASH_IFRN_DIS_BIT)) != 0) {
		INV_MSG(INV_MSG_LEVEL_DEBUG, "Setting FLASH_IFRN_DIS in FLASH_CFG register returned %d", rc);
		return rc;
	}

	/* erase all flash */
	if(page > ICM30XXX_MAX_FLASH_PAGE_ADDRESS) {
		/* Don't think we really want to OR this -- for mass erase, clear the page enable and pages */
		if((rc = inv_icm30xxx_set_reg_bits(s, ICM30XXX_REG_BANK_0, ICM30XXX_FLASH_ERASE_B0,
				ICM30XXX_FLASH_ERASE_MASS_EN_BIT)) != 0) {
			INV_MSG(INV_MSG_LEVEL_DEBUG, "setting FLASH_ERASE_MASS bit in FLASH_ERASE returned %d", rc);
			return rc;
		}

		/* wait until the MASS_EN BIT clears */
		do {
			uint8_t reg_value;

			if((rc = inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0,
					ICM30XXX_FLASH_ERASE_B0, &reg_value)) != 0) {
				return rc;
			}

			if(!(reg_value & ICM30XXX_FLASH_ERASE_MASS_EN_BIT))
				break;

			inv_icm30xxx_sleep(1);		/* sleep for a bit */
			timeout -= 1;

			if(timeout <= 0) {
				INV_MSG(INV_MSG_LEVEL_DEBUG, "Timeout while waiting for MASS ERASE to complete");
				return INV_ERROR_TIMEOUT;
			}
		} while(1);
	} else { /* erase only one page */
		/* there are 32 2K pages of flash, addressed with bits 4:0 of the FLASH_ERASE register */
		assert(0); // TODO
		return INV_ERROR_NIMPL;
	}

	/* Restore lp en to its original value */
	if((rc = inv_icm30xxx_lp_inhibit(s, false)) != 0) {
		INV_MSG(INV_MSG_LEVEL_DEBUG, "Restoring LP_EN returned %d", rc);
		return rc;
	}

	return 0;
}

static inline uint32_t get_flash_size(struct inv_icm30xxx * s)
{
	switch(s->variant) {
	case INV_ICM30XXX_VARIANT_ICM3063x:
	case INV_ICM30XXX_VARIANT_BERLIN:
		return ICM30630_FLASH_SIZE;

	case INV_ICM30XXX_VARIANT_ICM30670:
		return ICM30670_FLASH_SIZE;

	default:
		assert(0);
		return 0;
	}
}

static inline uint32_t get_flash_start_addr(struct inv_icm30xxx * s)
{
	switch(s->variant) {
	case INV_ICM30XXX_VARIANT_ICM3063x:
	case INV_ICM30XXX_VARIANT_BERLIN:
		return ICM30630_FLASH_START_ADDR;

	case INV_ICM30XXX_VARIANT_ICM30670:
		return ICM30670_FLASH_START_ADDR;

	default:
		assert(0);
		return 0;
	}
}

static inline uint32_t get_sram_size(struct inv_icm30xxx * s)
{
	switch(s->variant) {
	case INV_ICM30XXX_VARIANT_ICM3063x:
	case INV_ICM30XXX_VARIANT_BERLIN:
		return ICM30630_SRAM_SIZE;

	case INV_ICM30XXX_VARIANT_ICM30670:
		return ICM30670_SRAM_SIZE;

	default:
		assert(0);
		return 0;
	}
}

static inline uint32_t get_sram_start_addr(struct inv_icm30xxx * s)
{
	switch(s->variant) {
	case INV_ICM30XXX_VARIANT_ICM3063x:
	case INV_ICM30XXX_VARIANT_BERLIN:
		return ICM30630_SRAM_START_ADDR;

	case INV_ICM30XXX_VARIANT_ICM30670:
		return ICM30670_SRAM_START_ADDR;

	default:
		assert(0);
		return 0;
	}
}

int inv_icm30xxx_flash_write(struct inv_icm30xxx * s, const uint8_t * flash_image,
		uint32_t flash_size)
{
	int rc;
	int timeout = 10000; /* 10s */

	const unsigned ICM30XXX_FLASH_SIZE       = get_flash_size(s);
	const unsigned ICM30XXX_SRAM_SIZE        = get_sram_size(s);
	const unsigned ICM30XXX_SRAM_START_ADDR  = get_sram_start_addr(s);
	const unsigned ICM30XXX_FLASH_START_ADDR = get_flash_start_addr(s);;

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Checking if image fits in memory (limits: %d < %d and < %d)...",
			flash_size, ICM30XXX_FLASH_SIZE, ICM30XXX_SRAM_SIZE);

	/* sanity check */
	if(flash_size > ICM30XXX_FLASH_SIZE || flash_size > ICM30XXX_SRAM_SIZE) {
		rc = INV_ERROR_SIZE;
		goto error;
	}

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Writing %lu bytes of image to SRAM @ 0x%x...",
			flash_size, ICM30XXX_SRAM_START_ADDR);

	/* write the flash image to SRAM */
	if((rc = inv_icm30xxx_write_mem(s, ICM30XXX_SRAM_START_ADDR, flash_image, flash_size)) != 0)
		goto error;

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Mass erasing flash...");

	/* Perform mass erase */
	if((rc = inv_icm30xxx_flash_erase(s, 0xFF /* page > 64 will perform mass erase */)) != 0)
		goto error;

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Iniating DMA transfer of %lu bytes from 0x%x to 0x%x...",
			flash_size, ICM30XXX_SRAM_START_ADDR, ICM30XXX_FLASH_START_ADDR);

	/* Program the DMA controller to transfer the program data from SRAM to FLASH
		ONLY channels 0 and 1 are available to the serial interface.  Do not use other channels! */
	if((rc = inv_icm30xxx_initiate_dma_transfer(s, 1 /* channel */,
			ICM30XXX_SRAM_START_ADDR, ICM30XXX_FLASH_START_ADDR, flash_size)) != 0)
		goto error;

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Waiting for IDLE bit to be set...");

	/* check the idle status register */
	do {
		uint8_t reg_value;

		if((rc = inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0, ICM30XXX_IDLE_STATUS_B0,
				&reg_value)) != 0)
            goto error;

		if(reg_value & ICM30XXX_FLASH_IDLE_BIT)
			break;

		inv_icm30xxx_sleep(10);
		timeout -= 10;

		if(timeout <= 0) {
			rc = INV_ERROR_TIMEOUT;
			goto error;
		}
	} while(1);

	return 0;
error:
	INV_MSG(INV_MSG_LEVEL_DEBUG, "Error %d !", rc);
	return rc;
}

int inv_icm30xxx_flash_read(struct inv_icm30xxx * s, uint32_t offset,
		uint8_t * flash_data, uint32_t flash_size, inv_bool_t disable_cache)
{
	int rc;

	unsigned ICM30XXX_FLASH_START_ADDR = 0;
	
	if ((s->variant) == INV_ICM30XXX_VARIANT_ICM3063x)
	{
		ICM30XXX_FLASH_START_ADDR = ICM30630_FLASH_START_ADDR;
	}
	else 
	{
		ICM30XXX_FLASH_START_ADDR = ICM30670_FLASH_START_ADDR;
	}

	/* disable flash cache before reading */
	if(disable_cache) {
		if((rc = inv_icm30xxx_set_reg_bits(s, ICM30XXX_REG_BANK_0,
				ICM30XXX_FLASH_CFG_B0, ICM30XXX_FLASH_CACHE_BYPASS_BIT)) != 0) {
			return rc;
		}
	}

	/* sleep for a bit... */
	inv_icm30xxx_sleep(100);

	if((rc = inv_icm30xxx_read_mem(s, ICM30XXX_FLASH_START_ADDR + offset,
			flash_data, flash_size)) != 0)
		return rc;

	/* re-enable flash cache before reading */
	if(disable_cache) {
		if((rc = inv_icm30xxx_clear_reg_bits(s, ICM30XXX_REG_BANK_0,
				ICM30XXX_FLASH_CFG_B0, ICM30XXX_FLASH_CACHE_BYPASS_BIT)) != 0) {
			return rc;
		}
	}

	return rc;
}

/* Scratch interrupts related functions */

int inv_icm30xxx_get_scratch_int0_status(struct inv_icm30xxx * s, uint8_t * int_status)
{
	return inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_SCRATCH_INT0_STATUS_B0, int_status);
}

int inv_icm30xxx_get_scratch_int1_status(struct inv_icm30xxx * s, uint8_t * int_status)
{
	return inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_SCRATCH_INT1_STATUS_B0, int_status);
}

int inv_icm30xxx_get_scratch_int2_status(struct inv_icm30xxx * s, uint8_t * int_status)
{
	return inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_SCRATCH_INT2_STATUS_B0, int_status);
}

int inv_icm30xxx_set_scratch_m0_int(struct inv_icm30xxx * s, uint8_t value)
{
	return inv_icm30xxx_set_reg_bits(s, ICM30XXX_REG_BANK_1,
			ICM30XXX_SCRATCH_M0_INT_STATUS_B1, value);
}

/* Timer related functions */

int inv_icm30xxx_get_cpu_ticks(struct inv_icm30xxx * s, uint32_t * ticks)
{
	int rc;
	uint8_t buffer[4];

	if((rc = inv_icm30xxx_read_mem(s, ICM30XXX_TIMER_BASE_ADDR +
			ICM30XXX_EXT_TIME_STAMP_TIMER_VALUE, buffer, 4)) != 0) {
		return rc;
	}

	*ticks = inv_dc_little8_to_int32(buffer);
	/* counter starts counting from 0xFFFFFFFF and decrements */
	*ticks = (0xFFFFFFFF - *ticks);
	return 0;
}
