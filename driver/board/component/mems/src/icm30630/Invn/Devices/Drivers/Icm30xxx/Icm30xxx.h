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

/** @defgroup DriverIcm30xxx Icm30xxx driver
 *  @brief    Low-level driver for ICM30xxx devices
 *  @ingroup  Drivers
 *  @{
 */

#ifndef _INV_ICM30XXX_H_
#define _INV_ICM30XXX_H_

#include "Invn/InvExport.h"
#include "Invn/InvBool.h"
#include "Invn/InvError.h"

#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxxCmd.h"
#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxxCtrl.h"
#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxxDefs.h"
#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxxFifo.h"
#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxxLoad.h"
#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxxLogger.h"
#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxxSelfTest.h"
#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxxSetup.h"
#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxxSerif.h"
#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxxTransport.h"

#include "Invn/FifoProtocol/FifoProtocolCore.h"

#include <assert.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief ICM30xxx variant identifier
 */
enum inv_icm30xxx_variant {
	INV_ICM30XXX_VARIANT_ICM3063x,
	INV_ICM30XXX_VARIANT_ICM30670,
	INV_ICM30XXX_VARIANT_BERLIN,
};

/*
 * Various flags and definitions
 */

#define ICM30XXX_POLL_SCRATCH_INT0 ((unsigned)0x01) /**< to read SCRATCH INT0 status reg */
#define ICM30XXX_POLL_SCRATCH_INT1 ((unsigned)0x02) /**< to read SCRATCH INT1 status reg */
#define ICM30XXX_POLL_SCRATCH_INT2 ((unsigned)0x04) /**< to read SCRATCH INT2 status reg */
#define ICM30XXX_POLL_SCRATCH_INT3 ((unsigned)0x08) /**< to read SCRATCH INT3 status reg */

#define INV_ICM30XXX_AUDIO_SAMPLE_SIZE    (15*16*2) /**< 15 ms of audio at 16 k and 16 bits */

#define INV_ICM30XXX_POLL_AUDIO_FLAG      0x80      /**< flag to indicate to poll audio fifo */

/** @brief Data handler prototype definition
 *  @param[in] context   some context handler
 *  @param[in] sensorid  FIFO Protocol sensor id
 *  @param[in] timestamp sensor event timestamp in tick
 *  @param[in] status    FIFO Protocol status value
 *  @param[in] accurcay  FIFO Protocol accuracy value
 *  @param[in] data      pointer to FIFO Protocol sensor data buffer
 *  @param[in] size      size of FIFO Protocol sensor data buffer
 */
typedef void (*inv_icm30xxx_data_handler_t)(void * context, uint8_t sensorid,
		uint32_t timestamp, uint8_t status, uint8_t accuracy,
		const uint8_t * data, uint16_t size);

/** @brief ICM30xxx FW version strcuture
 */
typedef struct inv_icm30xxx_fw_version {
	uint8_t major, minor, patch;
	char    hash[8];
	uint32_t crc;
} inv_icm30xxx_fw_version_t;

/** @brief ICM30xxx driver states definition
 */
typedef struct inv_icm30xxx {
	struct inv_icm30xxx_serif serif;
	struct inv_icm30xxx_serif serif_i2cslave;
	uint8_t use_serif_i2cslave;
	int                       variant;
	uint8_t                   fifop_exp_pkt_cnt[FIFOPROTOCOL_SENSOR_ID_EXT_CNT
									+FIFOPROTOCOL_SENSOR_ID_CUST_CNT];
	uint8_t                   load_state;    /* to store some states related to loading */
	uint8_t                   poll_int_mask; /* to indicate which scratch reg
	                                            to read when polling device */
	uint8_t                   audio_flag;    /* keep track of current audio config and other stuff to do */
	uint8_t                   audio_fifo_empty_count; /* to keep track how many times audio FIFO was empty */
	struct {
		uint8_t bank;
		uint8_t fifo_idx;
	} cache;
	struct {
		int8_t     cnt;
		uint8_t    lpen_val;
	} lp_inhibit;
	/** @brief HW Fifo states structure */
	struct inv_icm30xxx_fifo_state {
		uint8_t   fifo_idx;
		uint8_t   pkt_size;
		uint8_t   pkt_size_byte;
		uint8_t   size;
		uint16_t  size_byte;
		uint8_t   wm;
		uint16_t  wm_byte;
		uint8_t * buffer;
		uint32_t  buffer_max;
		uint32_t  buffer_len;
	} fifo_states[INV_ICM30XXX_FIFO_ID_MAX];
	struct {
		uint8_t * buffer;
		uint32_t  buffer_len;
	} memory;
	struct {
		inv_icm30xxx_data_handler_t data;
		void *                    context;
	} handler;
	struct {
		uint8_t    sensorid;
		uint8_t    command;
		inv_bool_t event;
		union {
			struct inv_icm30xxx_fw_version fwversion;
			float bias[3];
			float gain[9];
			uint32_t clock_rate;
			struct {
				uint8_t who, what;
				int32_t arg;
			} load;
			uint8_t ping;
			struct {
				uint32_t timestamp;
				uint8_t  data[64];
				uint16_t len;
			} sensor_data;
			uint8_t config_data[64];
		} payload;
	} response;
	/** @brief reference to buffer for audio data */
	struct inv_icm30xxx_audio_buffer {
		uint8_t * buffer;
		uint32_t  max_size;
		uint32_t  cur_size;
		uint32_t  sample_size;
	} audio_buffer;
} inv_icm30xxx_t;


/** @brief Hook for low-level system sleep() function to be implemented by upper layer
 *  @param[in] ms number of millisecond the calling thread should sleep
 */
extern void inv_icm30xxx_sleep(int ms);


/** @brief Reset and initialize driver states
 *  @param[in] s             handle to driver states structure
 *  @param[in] variant       ICM30xxx variant id (must be a value from enum inv_icm30xxx_variant)
 *  @param[in] buffer        pointer to static buffer used by the driver to temporary store data.
 *  @param[in] buffer_len    pointer to static buffer used by the driver to temporary store data.
 *  @param[in] poll_int_mask mask indicating which scratch interrupt register to read for checking
 *                           for data availibility
 *  @param[in] data_handler  handler called when sensor data are parsed from one of the FIFO
 *  @param[in] context       some context pointer passed to the data_handler
 */
static inline void inv_icm30xxx_reset_states(struct inv_icm30xxx * s,
		int variant, const struct inv_icm30xxx_serif * serif,
		uint8_t * buffer, uint32_t  buffer_len, int poll_int_mask,
		inv_icm30xxx_data_handler_t data_handler, void * context
)
{
	assert(buffer_len >= 4096);

	memset(s, 0, sizeof(*s));
	s->variant           = variant;
	s->serif             = *serif;
	s->poll_int_mask     = (uint8_t)poll_int_mask;
	s->cache.bank        = 0xFF;
	s->cache.fifo_idx    = 0xFF;
	s->memory.buffer     = buffer;
	s->memory.buffer_len = buffer_len;
	s->handler.data      = data_handler;
	s->handler.context   = context;
	s->use_serif_i2cslave = 0;
	inv_icm30xxx_lp_inhibit_reset(s);
}

static inline void inv_icm30xxx_register_audio_buffer(struct inv_icm30xxx * s,
	uint8_t * buffer, uint32_t buffer_size)
{
	assert(buffer_size >= (2*16*1024));

	memset(&s->audio_buffer, 0, sizeof(s->audio_buffer));

	if(buffer_size >= INV_ICM30XXX_AUDIO_SAMPLE_SIZE) {
		s->audio_buffer.buffer      = buffer;
		s->audio_buffer.max_size    = buffer_size;
		s->audio_buffer.sample_size = INV_ICM30XXX_AUDIO_SAMPLE_SIZE;
	}
}

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM30XXX_H_ */

/** @} */
