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

#include "Icm30xxxLogger.h"
#include "Icm30xxx.h"

typedef uint8_t buf_idx_t;

#define LOGGER_BUF_ADDR  (uint32_t)0x20009F00
#define LOGGER_BUF_SIZE  (uint32_t)0x00000100
#define LOGGER_LVL_ADDR  (LOGGER_BUF_ADDR-3*sizeof(buf_idx_t))
#define LOGGER_WIDX_ADDR (LOGGER_BUF_ADDR-2*sizeof(buf_idx_t))
#define LOGGER_RIDX_ADDR (LOGGER_BUF_ADDR-sizeof(buf_idx_t))

int inv_imc30xxx_logger_setup(struct inv_icm30xxx * s, unsigned level)
{
	const buf_idx_t clevel = level;

	return inv_icm30xxx_write_mem(s, LOGGER_LVL_ADDR, &clevel, sizeof(clevel));
}

int inv_imc30xxx_print_logger_data(struct inv_icm30xxx * s,
		inv_imc30xxx_logger_putc_cb putc_cb, void * arg)
{
	int rc = 0;
	uint8_t buffer[LOGGER_BUF_SIZE];
	unsigned nb_char = 0;

	struct {
		buf_idx_t w, r;
	} idx;

	if((rc = inv_icm30xxx_read_mem(s, LOGGER_WIDX_ADDR, (uint8_t *)&idx, sizeof(idx))) != 0)
		return rc;

	if(idx.r == idx.w) {
		return 0; /* buffer empty */
	}

	/* read buffer */
	if(idx.r < idx.w) {
		nb_char = idx.w - idx.r;
		if((rc = inv_icm30xxx_read_mem(s, LOGGER_BUF_ADDR+idx.r, buffer, nb_char)) != 0)
			return rc;
	} else {
		const unsigned nb = LOGGER_BUF_SIZE - idx.r;

		if((rc = inv_icm30xxx_read_mem(s, LOGGER_BUF_ADDR+idx.r, buffer, nb)) != 0)
			return rc;

		if((rc = inv_icm30xxx_read_mem(s, LOGGER_BUF_ADDR, &buffer[nb], idx.w)) != 0)
			return rc;

		nb_char = nb + idx.w;
	}

	/* update read index */
	idx.r = idx.w;
	if((rc = inv_icm30xxx_write_mem(s, LOGGER_RIDX_ADDR, &idx.r, sizeof(idx.r))) != 0)
		return rc;

	/* print data */
	if(putc_cb) {
		unsigned i;
		for(i = 0; i < nb_char; ++i) {
			putc_cb(arg, (int)(unsigned int)buffer[i]);
		}
	}

	return 0;
}
