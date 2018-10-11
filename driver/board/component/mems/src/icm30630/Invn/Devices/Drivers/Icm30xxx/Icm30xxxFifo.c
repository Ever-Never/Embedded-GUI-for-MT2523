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

#include "Icm30xxxFifo.h"
#include "Icm30xxx.h"

#include "Invn/EmbUtils/Message.h"

int inv_icm30xxx_fifo_reset_assert(struct inv_icm30xxx * s, int fifo_id)
{
	return inv_icm30xxx_set_reg_bits(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_FIFO_RST_B0, INV_ICM30XXX_FIFO_ID_2_MASK(fifo_id));
}

int inv_icm30xxx_fifo_reset_release(struct inv_icm30xxx * s, int fifo_id)
{
	return inv_icm30xxx_clear_reg_bits(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_FIFO_RST_B0, INV_ICM30XXX_FIFO_ID_2_MASK(fifo_id));
}

int inv_icm30xxx_fifo_reset(struct inv_icm30xxx * s, int fifo_id)
{
	int rc;

	if((rc = inv_icm30xxx_fifo_reset_assert(s, fifo_id)) != 0)
		return rc;

	if((rc = inv_icm30xxx_fifo_reset_release(s, fifo_id)) != 0)
		return rc;

	return 0;
}

int inv_icm30xxx_fifo_get_pkt_size(struct inv_icm30xxx * s, int fifo_id,
		uint8_t * pkt_size)
{
	const uint8_t reg[] = {
		ICM30XXX_FIFO_0_PKT_SIZE_B0,
		ICM30XXX_FIFO_1_PKT_SIZE_B0,
		ICM30XXX_FIFO_2_PKT_SIZE_B0,
		ICM30XXX_FIFO_3_PKT_SIZE_B0,
	};

	if(fifo_id >= INV_ICM30XXX_FIFO_ID_MAX)
		return INV_ERROR_BAD_ARG;

	return inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0, reg[fifo_id], pkt_size);
}

int inv_icm30xxx_fifo_get_size(struct inv_icm30xxx * s, int fifo_id, uint8_t *size)
{
	const uint8_t reg[] = {
		ICM30XXX_FIFO_0_SIZE_B0,
		ICM30XXX_FIFO_1_SIZE_B0,
		ICM30XXX_FIFO_2_SIZE_B0,
		ICM30XXX_FIFO_3_SIZE_B0,
	};

	if(fifo_id >= INV_ICM30XXX_FIFO_ID_MAX)
		return INV_ERROR_BAD_ARG;

	return inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0, reg[fifo_id], size);
}

int inv_icm30xxx_fifo_get_wm(struct inv_icm30xxx * s, int fifo_id, uint8_t *wm)
{
	const uint8_t reg[] = {
		ICM30XXX_FIFO_0_WM_B0,
		ICM30XXX_FIFO_1_WM_B0,
		ICM30XXX_FIFO_2_WM_B0,
		ICM30XXX_FIFO_3_WM_B0,
	};

	if(fifo_id >= INV_ICM30XXX_FIFO_ID_MAX)
		return INV_ERROR_BAD_ARG;

	return inv_icm30xxx_read_reg_one(s, ICM30XXX_REG_BANK_0, reg[fifo_id], wm);
}

int inv_icm30xxx_fifo_set_wm(struct inv_icm30xxx * s, int fifo_id, uint8_t wm)
{
	const uint8_t reg[] = {
		ICM30XXX_FIFO_0_WM_B0,
		ICM30XXX_FIFO_1_WM_B0,
		ICM30XXX_FIFO_2_WM_B0,
		ICM30XXX_FIFO_3_WM_B0,
	};

	if(fifo_id >= INV_ICM30XXX_FIFO_ID_MAX)
		return INV_ERROR_BAD_ARG;

	return inv_icm30xxx_write_reg_one(s, ICM30XXX_REG_BANK_0, reg[fifo_id], wm);
}

static int inv_icm30xxx_fifo_set_index_cache(struct inv_icm30xxx * s, int fifo_id)
{
	if(s->cache.fifo_idx != fifo_id) {
		return inv_icm30xxx_fifo_set_index(s, fifo_id);
	}

	return 0;
}

int inv_icm30xxx_fifo_set_index(struct inv_icm30xxx * s, int fifo_id)
{
	int rc;

	if(fifo_id >= INV_ICM30XXX_FIFO_ID_MAX)
		return INV_ERROR_BAD_ARG;

	if((rc = inv_icm30xxx_write_reg_one(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_FIFO_INDEX_B0, fifo_id)) != 0)
		return rc;

	s->cache.fifo_idx = fifo_id;

	return 0;
}

int inv_icm30xxx_fifo_get_count(struct inv_icm30xxx * s, int fifo_id, uint16_t * cnt)
{
	int rc;
	uint8_t temp[2];

	if((rc = inv_icm30xxx_fifo_set_index_cache(s, fifo_id)) != 0)
		return rc;

	if((rc = inv_icm30xxx_read_reg(s, ICM30XXX_REG_BANK_0,
			ICM30XXX_FIFO_COUNTH_B0, temp, 2)) != 0)
		return rc;

	*cnt  = (temp[0] << 8);
	*cnt |= (temp[1]);

    return 0;
}

int inv_icm30xxx_fifo_read(struct inv_icm30xxx * s, int fifo_id,
		uint8_t *data, uint32_t len)
{
	int rc;
	uint32_t bytes_read = 0;

	if((rc = inv_icm30xxx_fifo_set_index_cache(s, fifo_id)) != 0)
		return rc;

	while(bytes_read < len) {
		uint32_t this_len = (len - bytes_read);

		if(this_len > inv_icm30xxx_serif_max_read(&s->serif)) {
			this_len = inv_icm30xxx_serif_max_read(&s->serif);
		}

		if((rc = inv_icm30xxx_read_reg(s, ICM30XXX_REG_BANK_0,
				ICM30XXX_FIFO_R_W_B0, &data[bytes_read], this_len)) != 0)
			return rc;

		bytes_read += this_len;
	}

	return 0;
}

int inv_icm30xxx_fifo_write(struct inv_icm30xxx * s, int fifo_id,
		const uint8_t * data, uint32_t len)
{
	int rc;
	uint32_t bytes_written = 0;

 	if((rc = inv_icm30xxx_fifo_set_index_cache(s, fifo_id)) != 0)
		return rc;

	while(bytes_written < len) {
		uint32_t this_len = (len - bytes_written);

		if(this_len > inv_icm30xxx_serif_max_write(&s->serif)) {
			this_len = inv_icm30xxx_serif_max_write(&s->serif);
		}

		if((rc = inv_icm30xxx_write_reg(s, ICM30XXX_REG_BANK_0,
				ICM30XXX_FIFO_R_W_B0, &data[bytes_written], this_len)) != 0)
			return rc;

		bytes_written += this_len;
	}

	return 0;
}

int inv_icm30xxx_fifo_buffer_setup(struct inv_icm30xxx * s, uint8_t fifo_idx)
{
	int rc;
	struct inv_icm30xxx_fifo_state * fifo;

	assert(s && fifo_idx < INV_ICM30XXX_FIFO_ID_MAX);

	/* keep fifo idx */
	fifo = &s->fifo_states[fifo_idx];
	fifo->fifo_idx = fifo_idx;

	/* read HW fifo config */
	if((rc = inv_icm30xxx_fifo_get_pkt_size(s, fifo_idx, &fifo->pkt_size)) != 0)
		return rc;
	if((rc = inv_icm30xxx_fifo_get_size(s, fifo_idx, &fifo->size)) != 0)
		return rc;
	if((rc = inv_icm30xxx_fifo_get_wm(s, fifo_idx, &fifo->wm)) != 0)
		return rc;

	if(fifo->pkt_size_byte == (uint8_t)0xFF) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "fifo %d pkt_size value=%u (maximum supported value is 254).\n"
			"\tThis was unexpected and something is probaly wrong.", (int)fifo_idx, (unsigned)fifo->pkt_size);
		return -1;
	}

	fifo->pkt_size_byte = fifo->pkt_size + 1;
	fifo->size_byte = inv_icm30xxx_fifo_compute_size_byte(fifo->size,
			fifo->pkt_size);
	fifo->wm_byte   = inv_icm30xxx_fifo_compute_size_byte(fifo->wm,
			fifo->pkt_size);

	INV_MSG(INV_MSG_LEVEL_DEBUG, "fifo %d config: pkt_size=%d size=%d wm=%d",
			(int)fifo_idx, (int)fifo->pkt_size_byte, (int)fifo->size_byte, (int)fifo->wm_byte);

	return 0;
}
