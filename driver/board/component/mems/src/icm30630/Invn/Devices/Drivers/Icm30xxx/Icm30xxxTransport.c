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

#include "Icm30xxxTransport.h"
#include "Icm30xxxSerif.h"
#include "Icm30xxx.h"

#define SPI_READ_BIT 0x80

static int inv_icm30xxx_set_bank_cache(struct inv_icm30xxx * s, uint8_t bank,
		uint8_t * register_addr)
{
	/* if we are using SPI and reg address is above 0x7F, we must set bit1
	   in REG_BANK_SEL as bit 0x80 is used to distinguish between SPI R/W */
	if(inv_icm30xxx_serif_is_spi(&s->serif)) {
		bank &= 1; /* ensure bank is 0 or 1 */
		if(*register_addr & SPI_READ_BIT) { /* if reg is above 0x80,
		                                    set ICM30XXX_SPI_ADDR_MSB_BIT
		                                    to distinguish from a read */
			bank |= ICM30XXX_SPI_ADDR_MSB_BIT;
			*register_addr &= ~SPI_READ_BIT; /* clear READ bit to avoid confusion */
		}
	}

	/* set bank only if different from cached value */
	if(bank != s->cache.bank) {
		int rc;
		if((rc = inv_icm30xxx_serif_write_reg(&s->serif, ICM30XXX_REG_BANK_SEL,
				&bank, 1)) != 0)
			return rc;
		s->cache.bank = bank;
	}

	return 0;
}

int inv_icm30xxx_read_reg(struct inv_icm30xxx * s, uint8_t bank, uint8_t reg,
		uint8_t * buf, uint32_t len)
{
	int rc;

	if((rc = inv_icm30xxx_set_bank_cache(s, bank, &reg)) != 0)
		return rc;

	return inv_icm30xxx_serif_read_reg(&s->serif, reg, buf, len);
}

int inv_icm30xxx_write_reg(struct inv_icm30xxx * s, uint8_t bank, uint8_t reg,
		const uint8_t * buf, uint32_t len)
{
	inv_bool_t do_twice = false;
	int rc;

	/* HW bug workaround, see below... */
	if(bank == ICM30XXX_REG_BANK_0 && reg == ICM30XXX_PWR_MGMT_1_B0)
		do_twice = true;

	if((rc = inv_icm30xxx_set_bank_cache(s, bank, &reg)) != 0)
		return rc;

	if((rc = inv_icm30xxx_serif_write_reg(&s->serif, reg, buf, len)) != 0)
		return rc;

	/* There is an ASIC bug such that when a PWR_MGMT_1 register write happens
	   in the same cycle as the chip is going to sleep, the write does not go through.
	   So if we write it back to back, for sure one of the two writes will propagate through
	   (chip takes a relatively long period of time to go into, come out of, and go back into sleep)
	*/
	if(do_twice) {
		if((rc = inv_icm30xxx_serif_write_reg(&s->serif, reg, buf, len)) != 0)
			return rc;
	}

	return 0;
}
