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


/** @defgroup DriverIcm30xxxFifo Icm30xxx driver FIFO
 *  @brief    Low-level functions to configure and access HW FIFO of ICM30xxx devices
 *  @ingroup  DriverIcm30xxx
 *  @{
 */

#ifndef _INV_ICM30XXX_FIFO_H_
#define _INV_ICM30XXX_FIFO_H_

#include "Invn/InvExport.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Forward declarations */
struct inv_icm30xxx;

/** @brief FIFO ID definitions
 */
enum inv_icm30xxx_fifo_id {
	INV_ICM30XXX_FIFO_ID_0 = 0,
	INV_ICM30XXX_FIFO_ID_1 = 1,
	INV_ICM30XXX_FIFO_ID_2 = 2,
	INV_ICM30XXX_FIFO_ID_3 = 3,
	INV_ICM30XXX_FIFO_ID_MAX
};

/** @brief Helper macro to convert FIFO id to mask
 */
#define INV_ICM30XXX_FIFO_ID_2_MASK(fifo_id) 	(1 << (fifo_id))

/** @brief Set FIFO reset bit
 *  Assert and hold to set FIFO count to 0
 *  While reset is asserted enables to FIFO write should be disabled so as not
 *  to corrupt FIFOs that are not in reset.
 */
int INV_EXPORT inv_icm30xxx_fifo_reset_assert(struct inv_icm30xxx * s, int fifo_id);

/** @brief Clear FIFO reset bit
 */
int INV_EXPORT inv_icm30xxx_fifo_reset_release(struct inv_icm30xxx * s, int fifo_id);

/** @brief Set and clear FIFO reset bit
 */
int INV_EXPORT inv_icm30xxx_fifo_reset(struct inv_icm30xxx * s, int fifo_id);

/** @brief Helper macro to get factor value from size register value
 */
#define INV_ICM30XXX_FIFO_FACTOR_VAL(size)            ((size) & 0x1F)

/** @brief Helper macro to get mult value from size register value
 */
#define INV_ICM30XXX_FIFO_MULT_VAL(size)              (((size) & 0xE0) >> 5)

/** @brief Helper macro to compute size register value from mult and factor value
 */
#define INV_ICM30XXX_FIFO_SIZE_VAL(mult, factor)      (((mult) << 5) | (factor))

/** @brief Helper macro to compute FIFO size in byte from mult, factor and packet_size value
 *  Formula: (2^fifo_size[7:5]) x (fifo_size[4:0]+1) x (fifo_pkt_size+1)-1
 */
#define INV_ICM30XXX_FIFO_COMPUTE_SIZE_BYTE(size, packet) \
	inv_icm30xxx_fifo_compute_size_byte(size, packet)

static inline uint16_t inv_icm30xxx_fifo_compute_size_byte(uint8_t size, uint8_t packet)
{
	const uint16_t mult = INV_ICM30XXX_FIFO_MULT_VAL(size);
	const uint16_t factor = INV_ICM30XXX_FIFO_FACTOR_VAL(size);

	return (1 << mult)*(factor + 1)*(packet+1) - 1;
}

/** @brief Return current FIFO packet register value
 */
int INV_EXPORT inv_icm30xxx_fifo_get_pkt_size(struct inv_icm30xxx * s, int fifo_id,
		uint8_t *pkt_size);

/** @brief Return current FIFO size register value
 */
int INV_EXPORT inv_icm30xxx_fifo_get_size(struct inv_icm30xxx * s, int fifo_id, uint8_t *size);

/** @brief Set FIFO watermark register value
 */
int INV_EXPORT inv_icm30xxx_fifo_set_wm(struct inv_icm30xxx * s, int fifo_id, uint8_t wm);

/** @brief Return FIFO watermark register value
 */
int INV_EXPORT inv_icm30xxx_fifo_get_wm(struct inv_icm30xxx * s, int fifo_id, uint8_t *wm);

/** @brief Set FIFO index (FIFO impacted by R/W access)
 */
int INV_EXPORT inv_icm30xxx_fifo_set_index(struct inv_icm30xxx * s, int fifo_id);

/** @brief Return current FIFO count (number of packet in the FIFO)
 */
int INV_EXPORT inv_icm30xxx_fifo_get_count(struct inv_icm30xxx * s, int fifo_id,
		uint16_t * length);

/** @brief Read bytes from FIFO
	Will always read length bytes no matter actual FIFO count
 */
int INV_EXPORT inv_icm30xxx_fifo_read(struct inv_icm30xxx * s, int fifo_id, uint8_t *data,
		uint32_t length);

/** @brief Read bytes from FIFO
 *  Will always write length bytes no matter actual FIFO count
 *  Depending on FIFO mode, oldest element will be overwritten or newsest element discarded
 */
int INV_EXPORT inv_icm30xxx_fifo_write(struct inv_icm30xxx * s, int fifo_id,
		const uint8_t * data, uint32_t length);

/** @brief Extract FIFO configuration information for FIFO fifo_idx and fill in s with those
 */
int INV_EXPORT inv_icm30xxx_fifo_buffer_setup(struct inv_icm30xxx * s, uint8_t fifo_idx);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM30XXX_FIFO_H_ */

/** @} */
