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

/** @defgroup DriverIcm30xxxSetup Icm30xxx driver setup
 *  @brief Low-level function to setup an Icm30xxx device
 *  @ingroup  DriverIcm30xxx
 *  @{
 */

#ifndef _INV_ICM30XXX_SETUP_H_
#define _INV_ICM30XXX_SETUP_H_

#include "Invn/InvExport.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "Invn/InvBool.h"

/* forward declaration */
struct inv_icm30xxx;

/* Identification related functions */

// return WHOAM register value
int INV_EXPORT inv_icm30xxx_get_whoami(struct inv_icm30xxx * s, uint8_t * whoami);

/* Power related functions */

// reset state for lp inhibing feature
// you must call inv_icm30xxx_lp_enable() afterwise to ensure correct value of lp_en
void INV_EXPORT inv_icm30xxx_lp_inhibit_reset(struct inv_icm30xxx * s);

// inhibit from asserting lp_en until function is called again with false as argument
// will clear LP_EN bit if set, and restore its value afterwards
// support nested calls
int	INV_EXPORT inv_icm30xxx_lp_inhibit(struct inv_icm30xxx * s, inv_bool_t enable);

// set/clear LP_EN bit
int INV_EXPORT inv_icm30xxx_lp_enable(struct inv_icm30xxx * s, inv_bool_t enable);

// set SLEEP_REQ and DEEP_SLEEP bits
int INV_EXPORT inv_icm30xxx_set_deep_sleep(struct inv_icm30xxx * s);

// clear SLEEP_REQ bit and set DEEP_SLEEP and LP_EN bits if not inhibited
int INV_EXPORT inv_icm30xxx_set_sleep(struct inv_icm30xxx * s);

// clear SLEEP_REQ and DEEP_SLEEP bits
int INV_EXPORT inv_icm30xxx_wake_up(struct inv_icm30xxx * s);

// clear LP_EN bit
int INV_EXPORT inv_icm30xxx_power_off(struct inv_icm30xxx * s);

// return PWR_MGMT_1 register value
int INV_EXPORT inv_icm30xxx_get_power_mgmt(struct inv_icm30xxx * s, uint8_t * pwr);

// set PWR_MGMT_1 register value
int INV_EXPORT inv_icm30xxx_set_power_mgmt(struct inv_icm30xxx * s, uint8_t pwr);

/* Setup/boot related functions */

// set SOFT_RST bit and wait for it to be cleared
int INV_EXPORT inv_icm30xxx_soft_reset(struct inv_icm30xxx * s);

// enable M0 by writing M0_EN bit to MOD_EN register
// if serif is SPI will also set I2C_IF_DIS bit
// if debug is true will set set SKU_TYPE and DAP_EN bits to TEST_MODES register
// finally, set LP_EN bit
int INV_EXPORT inv_icm30xxx_start_m0(struct inv_icm30xxx * s, inv_bool_t debug);

// enable ZSP (Berlin only)
int INV_EXPORT inv_icm30xxx_start_zsp(struct inv_icm30xxx * s);

/* SRAM/FLASH memory related functions */

// read len bytes from memory at address addr
int INV_EXPORT inv_icm30xxx_read_mem(struct inv_icm30xxx * s, uint32_t addr,
		uint8_t * buf, uint32_t len);

// write len butes to memory at address addr
int INV_EXPORT inv_icm30xxx_write_mem(struct inv_icm30xxx * s, uint32_t addr,
		const uint8_t * buf, uint32_t len);

// intitiate DMA transfert to copy nbytes from source_addr tp dest_addr
// source_addr being in SRAM and dest_addr being in FLASH
// be sure PWMGT_1 is set to 0 before accessing the flash
int INV_EXPORT inv_icm30xxx_initiate_dma_transfer(struct inv_icm30xxx * s, uint8_t dma_channel,
		uint32_t source_addr, uint32_t dest_addr, uint32_t num_bytes);

// erase specified flash page
// if page is >= 32, will perform mass erase
// be sure PWMGT_1 is set to 0 before accessing the flash
int INV_EXPORT inv_icm30xxx_flash_erase(struct inv_icm30xxx * s, uint8_t page);

// write flash_size bytes to flash starting from FLASH_START address
// using SRAM as temporary buffer and mass erasing the flash
// will call the following functions in order:
// - inv_icm30xxx_lp_inhibit(true)
// - inv_icm30xxx_wake_up()
// - inv_icm30xxx_write_mem(@ICM30XXX_SRAM_START_ADDR)
// - inv_icm30xxx_flash_erase(32)
// - inv_icm30xxx_soft_reset()
// - inv_icm30xxx_initiate_dma_transfer(ICM30XXX_SRAM_START_ADDR, ICM30XXX_FLASH_START_ADDR)
// - inv_icm30xxx_lp_inhibit(false)
// be sure PWMGT_1 is set to 0 before accessing the flash
int INV_EXPORT inv_icm30xxx_flash_write(struct inv_icm30xxx * s, const uint8_t * flash_image,
		uint32_t flash_size);

// write flash_size bytes from flash starting from FLASH_START address
// and copy them to flash_image
// if diable_cache is true, will set FLASH_CACHE_BYPASS bit to FLASH_CFG
// and clear it afterwards
// be sure PWMGT_1 is set to 0 before accessing the flash
int INV_EXPORT inv_icm30xxx_flash_read(struct inv_icm30xxx * s, uint32_t offset,
		uint8_t * flash_image, uint32_t flash_size, inv_bool_t disable_cache);

/* Scratch interrupts related functions */

// return SCRATCH_INT0_STATUS register value
int INV_EXPORT inv_icm30xxx_get_scratch_int0_status(struct inv_icm30xxx * s, uint8_t * status);

// return SCRATCH_INT1_STATUS register value
int INV_EXPORT inv_icm30xxx_get_scratch_int1_status(struct inv_icm30xxx * s, uint8_t * status);

// return SCRATCH_INT2_STATUS register value
int INV_EXPORT inv_icm30xxx_get_scratch_int2_status(struct inv_icm30xxx * s, uint8_t * status);

// set value to SCRATCH_M0_INT register
int INV_EXPORT inv_icm30xxx_set_scratch_m0_int(struct inv_icm30xxx * s, uint8_t value);

/* Timer related functions */

// return current systick from M0
int INV_EXPORT inv_icm30xxx_get_cpu_ticks(struct inv_icm30xxx * s, uint32_t * ticks);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM30XXX_SETUP_H_ */

/** @} */
