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

#include "Icm30xxxCtrl.h"
#include "Icm30xxx.h"

#include "Invn/FifoProtocol/FifoProtocol.h"
#include "Invn/FifoProtocol/FifoProtocolCore.h"
#include "Invn/FifoProtocol/FifoProtocolHelper.h"

#include "Invn/EmbUtils/Message.h"

#include <assert.h>

int inv_icm30xxx_ctrl_retrieve_fifo_config(struct inv_icm30xxx * s)
{
	int rc, i;

	memset(s->fifo_states, 0, sizeof(s->fifo_states));

	for(i = 0; i < INV_ICM30XXX_FIFO_ID_MAX; ++i) {
		if((rc = inv_icm30xxx_fifo_buffer_setup(s, i)) != 0)
			return rc;
	}

	/* set data FIFO ring buffer to some location from the global memory buffer */
	s->fifo_states[INV_ICM30XXX_FIFO_ID_1].buffer     = &s->memory.buffer[0];
	s->fifo_states[INV_ICM30XXX_FIFO_ID_1].buffer_max = s->memory.buffer_len / 2;
	s->fifo_states[INV_ICM30XXX_FIFO_ID_1].buffer_len = 0;
	s->fifo_states[INV_ICM30XXX_FIFO_ID_3].buffer     = &s->memory.buffer[s->memory.buffer_len / 2];
	s->fifo_states[INV_ICM30XXX_FIFO_ID_3].buffer_max = s->memory.buffer_len / 2;
	s->fifo_states[INV_ICM30XXX_FIFO_ID_3].buffer_len = 0;

	return 0;
}

static int push_command_down(struct inv_icm30xxx * s, const uint8_t * buffer,
		uint16_t len)
{
	int rc = 0;
	const struct inv_icm30xxx_fifo_state * fifo;
	uint16_t nbpkt_to_push;
	int timeout = 1000; /* 1s */

	assert(s && buffer);

	fifo = &s->fifo_states[INV_ICM30XXX_FIFO_ID_0];

	/* pkt size is null, this is wrong, an error should have been caught this earlier */
	assert(fifo->pkt_size_byte != 0);
	/* nb byte is not aligned on pkt size, this is wrong... */
	assert(len%fifo->pkt_size_byte == 0);
	/* too many bytes to push at once. this is also wrong... */
	assert(len <= fifo->size_byte);

	nbpkt_to_push = len/fifo->pkt_size_byte;

	/* device must be in on */
	if((rc = inv_icm30xxx_lp_enable(s, false)) != 0)
		return rc;

	do {
		uint16_t current_cnt;

		/* retrieve current hw fifo count */
		if((rc = inv_icm30xxx_fifo_get_count(s, fifo->fifo_idx, &current_cnt)) != 0)
			return rc;

		if(s->variant == INV_ICM30XXX_VARIANT_BERLIN) {
			/* For Berlin, FIFO COUNT holds the number of byte */
			current_cnt /= fifo->pkt_size_byte;
		}

		/* check if there is space in the fifo */
		if(nbpkt_to_push < (fifo->size_byte/fifo->pkt_size_byte) - current_cnt)
			break;

		/* sleep for a bit, letting device process pending command */
		inv_icm30xxx_sleep(10);
		timeout -= 10;

		if(timeout <= 0) {
			rc = INV_ERROR_TIMEOUT;
			goto end;
		}
	} while(1);

	/* write all bytes to fifo */
	if((rc = inv_icm30xxx_fifo_write(s, fifo->fifo_idx, buffer, len)) != 0) {
		return rc;
	}

	/* trigger scratch int */
	if((rc = inv_icm30xxx_set_scratch_m0_int(s, 0x80)) != 0) // FIXME need to create a define for this
			goto end;

end:
	inv_icm30xxx_lp_enable(s, true);
	return rc;
}

int inv_icm30xxx_ctrl_send_command(struct inv_icm30xxx * s,	uint8_t sensorid,
		uint8_t command, const void * arg, uint16_t size)
{
	struct FifoProtocolPacket packet;
	uint8_t buffer[128];
	uint16_t idx = 0;
	int rc;

	inv_fifo_protocol_command_packet_set(&packet, sensorid,
			(FifoProtocolCmd)command, arg, size);

	do {
		uint16_t size;
		rc = inv_fifo_protocol_format_packet_r(&packet, &buffer[idx], &size,
				s->fifop_exp_pkt_cnt);
		idx += size;
		assert(idx <= sizeof(buffer));
	} while(rc == FIFOPROTOCOL_ERROR_INCOMPLETE);

	return push_command_down(s, buffer, idx);
}

int inv_icm30xxx_read_int_status(struct inv_icm30xxx * s, uint8_t poll_int_mask,
		uint8_t * fifo_mask)
{
	int rc = 0;
	uint8_t int_status;

	assert(s && fifo_mask);

	*fifo_mask = 0;

	if(poll_int_mask & ICM30XXX_POLL_SCRATCH_INT0) {
		if((rc = inv_icm30xxx_get_scratch_int0_status(s, &int_status)) != 0)
			return rc;
		*fifo_mask |= int_status;
	}

	if(poll_int_mask & ICM30XXX_POLL_SCRATCH_INT1) {
		if((rc = inv_icm30xxx_get_scratch_int1_status(s, &int_status)) != 0)
			return rc;
		*fifo_mask |= int_status;
	}

	if(poll_int_mask & ICM30XXX_POLL_SCRATCH_INT2) {
		if((rc = inv_icm30xxx_get_scratch_int2_status(s, &int_status)) != 0)
			return rc;
		*fifo_mask |= int_status;
	}

	return rc;
}

static void response_handler(struct inv_icm30xxx * s, uint8_t sensorid,
	uint8_t command, const uint8_t * data, uint16_t size)
{
	INV_MSG(INV_MSG_LEVEL_DEBUG, "Receive response (id=%d, command=%d, payload=%d)",
		sensorid, command, size);

	s->response.sensorid = sensorid;
	s->response.command  = command;

	switch(command) {
	case FIFOPROTOCOL_CMD_GETFIRMWAREINFO:
		inv_fifo_protocol_helper_decode_firmware(data, size,
				&s->response.payload.fwversion.major,
				&s->response.payload.fwversion.minor,
				&s->response.payload.fwversion.patch,
				(uint8_t *)s->response.payload.fwversion.hash,
				&s->response.payload.fwversion.crc);
		break;

	case FIFOPROTOCOL_CMD_LOAD:
		s->response.payload.load.who   = data[0];
		s->response.payload.load.what  = data[1];
		s->response.payload.load.arg   = data[2];
		s->response.payload.load.arg  |= data[3] << 8;
		s->response.payload.load.arg  |= data[4] << 16;
		s->response.payload.load.arg  |= data[5] << 24;
		break;

	case FIFOPROTOCOL_CMD_GETCALIBRATIONOFFSETS:
		inv_fifo_protocol_helper_decode_calibrationoffsets(data, size,
				s->response.payload.bias);
		break;

	case FIFOPROTOCOL_CMD_GETCALIBRATIONGAINS:
		inv_fifo_protocol_helper_decode_calibrationgains(data, size,
				s->response.payload.gain);
		break;

	case FIFOPROTOCOL_CMD_PING:
		s->response.payload.ping = (data[0] != 0);
		break;

	case FIFOPROTOCOL_CMD_GETCLOCKRATE:
		s->response.payload.clock_rate =
				inv_fifo_protocol_helper_decode_clockrate(data, size);
		break;

	case FIFOPROTOCOL_CMD_GETCONFIG:
		if(size > sizeof(s->response.payload.config_data)) {
			size = sizeof(s->response.payload.config_data);
		}
		memcpy(s->response.payload.config_data, data, size);
		break;
	default:
		/* unexpected command id */
		INV_MSG(INV_MSG_LEVEL_WARNING, "Unexpected response data frame "
				"received for command id %d. Ignored.", command);
		return;
	}

	/* signal the response */
	s->response.event = true;
}

static void data_handler(struct inv_icm30xxx * s, uint8_t sensorid,
		uint32_t timestamp, uint8_t status, uint8_t accuracy,
		const uint8_t * data, uint16_t size)
{
	/* correspond to a GETDATA command */
	if(status == FIFOPROTOCOL_STATUS_POLL) {
		/* copy response back */
		s->response.sensorid = sensorid;
		s->response.command  = FIFOPROTOCOL_CMD_GETDATA;
		s->response.payload.sensor_data.timestamp = timestamp;
		/* Check response size */
		if (size > sizeof(s->response.payload.sensor_data.data))
		{
			INV_MSG(INV_MSG_LEVEL_WARNING, "Response from command longer "
					"than expected. Dropping %d bytes", size - sizeof(s->response.payload.sensor_data.data));
			size = sizeof(s->response.payload.sensor_data.data);
		}
		memcpy(s->response.payload.sensor_data.data, data, size);
		s->response.payload.sensor_data.len = size;
		/* signal the response */
		s->response.event = true;
	} else { 	/* asynchronous sensor event */
		/* notify upper layer */
		s->handler.data(s->handler.context, sensorid,
				timestamp, status, accuracy, data, size);
	}
}

static int parse_fifo(struct inv_icm30xxx * s, const uint8_t * buffer, uint32_t len)
{
	int rc = 0;
	struct FifoProtocolPacket packet;
	uint32_t index = 0;
	int32_t  remaining = (int32_t)len;

	inv_fifo_protocol_parse_packet_reset(&packet);

	packet.type = FIFOPROTOCOL_PACKET_TYPE_DATA;

	while(remaining >= FIFOPROTOCOL_PACKET_SIZE) {

		/* check header / footer */
		if(buffer[index] != FIFOPROTOCOL_PROTECTED_HEADER
				|| buffer[index+FIFOPROTOCOL_PACKET_SIZE-1] != FIFOPROTOCOL_PROTECTED_FOOTER) {
			INV_MSG(INV_MSG_LEVEL_VERBOSE, "Bad header/footer in fifo packet, dropping one byte...");
			index     += 1;
			remaining -= 1;
			continue;
		}

		/* header/footer ok - parse packet */
		rc = inv_fifo_protocol_parse_packet_r(&packet, &buffer[index],
				FIFOPROTOCOL_PACKET_SIZE, s->fifop_exp_pkt_cnt);

		/* retry if parsing error with last packet */
		if(rc == FIFOPROTOCOL_ERROR_PARSE) {
			INV_MSG(INV_MSG_LEVEL_VERBOSE, "Error parsing fifo packet. One packet lost."
					" Retrying with current packet...");
			rc = inv_fifo_protocol_parse_packet_r(&packet, &buffer[index],
					FIFOPROTOCOL_PACKET_SIZE, s->fifop_exp_pkt_cnt);
		}

		/* invalid packet, skip it */
		if(rc == FIFOPROTOCOL_ERROR_SIZE || rc == FIFOPROTOCOL_ERROR_PARSE) {
			INV_MSG(INV_MSG_LEVEL_VERBOSE, "Error parsing fifo packet. Dropping full packet...");
		}
		/* valid packet, call handlers to notify upper layer */
		else if(rc == FIFOPROTOCOL_ERROR_NO) {
			if(packet.type == FIFOPROTOCOL_PACKET_TYPE_ANSWER) {
				response_handler(s, packet.sensorid,
						(FifoProtocolCmd)packet.u.command.cmd,
						packet.arg, packet.size);
			} else {
				data_handler(s, packet.sensorid,
						packet.u.data.timestamp, packet.u.data.status,
						packet.u.data.accuracy, packet.arg, packet.size);
			}
		}
		else {
			/* incomplete packet */
		}

		/* parse next packet */
		index     += FIFOPROTOCOL_PACKET_SIZE;
		remaining -= FIFOPROTOCOL_PACKET_SIZE;
	}

	/* If we did not receive all packets needed for current sensor id to be fully parsed, then make
		sure the already analyzed packet(s) will be part of next call to this function */
	if (rc == FIFOPROTOCOL_ERROR_INCOMPLETE) {
		// INV_MSG(INV_MSG_LEVEL_DEBUG, "Incomplete fifo packet, rewind fifo index.");
		remaining += packet.states.packet_cnt * FIFOPROTOCOL_PACKET_SIZE;
	}

	/* return number of remaining bytes from input buffer */
	return remaining;
}

static int read_and_parse_fifo(struct inv_icm30xxx * s, uint8_t fifo_idx)
{
	int rc = 0;
	uint16_t count;
	uint32_t total_bytes;
	uint32_t bytes_read;
	struct inv_icm30xxx_fifo_state * fifo;
	inv_bool_t reset_wm = false;

	assert(fifo_idx < INV_ICM30XXX_FIFO_ID_MAX);

	fifo = &s->fifo_states[fifo_idx];

	/* pkt size is null, this is wrong, an error should have been caught this earlier */
	assert(fifo->pkt_size_byte != 0);

	if((rc = inv_icm30xxx_lp_enable(s, false)) != 0)
		return rc;

	/* retrieve current hw fifo count */
	if((rc = inv_icm30xxx_fifo_get_count(s, fifo->fifo_idx, &count)) != 0) {
		/* something bad happen, better reset the fifo to
		   maybe be able to recover later... */
		fifo->buffer_len = 0;
		INV_MSG(INV_MSG_LEVEL_DEBUG, "inv_icm30xxx_fifo_get_count() = %d", rc);
		goto end;
	}

	/* compute total bytes */
	if(s->variant == INV_ICM30XXX_VARIANT_BERLIN) {
		/* for Berlin it holds the number of bytes (FPGA issue ?) */
		total_bytes = count;
	} else {
		/* for ICM30630 and 30670, FIFO CNT holds the number of packet */
		total_bytes = count*fifo->pkt_size_byte;
	}
	bytes_read = 0;

	/* disable WM if above threshold to avoid multiple-interrupts */
	if(total_bytes >= fifo->wm_byte) {
		if((rc = inv_icm30xxx_fifo_set_wm(s, fifo_idx, 0xFF)) != 0) {
			INV_MSG(INV_MSG_LEVEL_DEBUG, "inv_icm30xxx_fifo_set_wm() = %d", rc);
			/* something bad happen, better reset the fifo to
			   maybe be able to recover later... */
			fifo->buffer_len = 0;
			goto end;
		}
		reset_wm = true;
	}

	while(bytes_read < total_bytes) {
		const uint32_t max_len = fifo->buffer_max - fifo->buffer_len;
		uint32_t this_len = (total_bytes - bytes_read);
		uint8_t * buffer = &fifo->buffer[fifo->buffer_len];
		const uint32_t max_rx_size = inv_icm30xxx_serif_max_read(&s->serif);

		if(this_len > max_len) {
			INV_MSG(INV_MSG_LEVEL_VERBOSE, "FIFO miror is full. Reading only %d/%d",
					this_len, max_len);
			this_len = max_len;
		}

		if(max_len < FIFOPROTOCOL_PACKET_SIZE) {
			INV_MSG(INV_MSG_LEVEL_VERBOSE, "FIFO miror remaining size below < PACKET_SIZE."
					" Something bad happen. Resetting buffer.");
		}

		/* ensure we only read complete packet */
		if(this_len >= max_rx_size) {
			this_len = max_rx_size - (max_rx_size % FIFOPROTOCOL_PACKET_SIZE);
		}

		if((rc = inv_icm30xxx_fifo_read(s, fifo->fifo_idx, buffer, this_len)) != 0) {
			INV_MSG(INV_MSG_LEVEL_DEBUG, "Error (%d) while reading HW FIFO. Resetting buffer.", rc);
			/* something bad happen, better reset the fifo to
			   maybe be able to recover later... */
			fifo->buffer_len = 0;
			goto end;
		}

		fifo->buffer_len += this_len;
		rc = parse_fifo(s, fifo->buffer, fifo->buffer_len);

		if(rc < 0) {
			/* something bad happen, reset the fifo to maybe be able to recover later... */
			fifo->buffer_len = 0;
			goto end;
		}
		else if(rc > 0) {
			/* incomplete packet, move bytes at the top of the buffer */
			memmove(fifo->buffer, buffer + this_len - rc, rc);
			fifo->buffer_len = rc;
			rc = 0;
		} else {
			/* no remaining byte, no incomplete packet, reset buffer */
			fifo->buffer_len = 0;
		}

		bytes_read += this_len;
	}

	/* reset back WM value */
	if(reset_wm) {
		if((rc = inv_icm30xxx_fifo_set_wm(s, fifo_idx, fifo->wm)) != 0){
			/* something bad happen, better reset the fifo to
			   maybe be able to recover later... */
			fifo->buffer_len = 0;
			INV_MSG(INV_MSG_LEVEL_DEBUG, "inv_icm30xxx_fifo_set_wm() = %d", rc);
			goto end;
		}
	}

end:
	inv_icm30xxx_lp_enable(s, true);

	return rc;
}

// parse_i2c - This routine is modeled off of parse_fifo(), and maintains some of the FIFOness, which isn't necessarily
// appropriate for I2C, but which works and also helps to maintain the similiarity between the two.
static int parse_i2c(struct inv_icm30xxx * s, const uint8_t * buffer, uint32_t len, struct FifoProtocolPacket *packet)
{
	int rc = 0;
	uint32_t index = 0;
	int32_t  remaining = (int32_t)len;

	while(remaining >= FIFOPROTOCOL_PACKET_SIZE) {

		/* check header / footer */
		if(buffer[index] != FIFOPROTOCOL_PROTECTED_HEADER
				|| buffer[index+FIFOPROTOCOL_PACKET_SIZE-1] != FIFOPROTOCOL_PROTECTED_FOOTER) {
			INV_MSG(INV_MSG_LEVEL_VERBOSE, "Bad header/footer in fifo packet, dropping one byte...");
			index     += 1;
			remaining -= 1;
			continue;
		}

		/* header/footer ok - parse packet */
		rc = inv_fifo_protocol_parse_packet_r(packet, &buffer[index],
				FIFOPROTOCOL_PACKET_SIZE, s->fifop_exp_pkt_cnt);

		/* retry if parsing error with last packet */
		if(rc == FIFOPROTOCOL_ERROR_PARSE) {
			INV_MSG(INV_MSG_LEVEL_VERBOSE, "Error parsing fifo packet. One packet lost."
					" Retrying with current packet...");
			rc = inv_fifo_protocol_parse_packet_r(packet, &buffer[index],
					FIFOPROTOCOL_PACKET_SIZE, s->fifop_exp_pkt_cnt);
		}

		/* invalid packet, skip it */
		if(rc == FIFOPROTOCOL_ERROR_SIZE || rc == FIFOPROTOCOL_ERROR_PARSE) {
			INV_MSG(INV_MSG_LEVEL_VERBOSE, "Error parsing fifo packet. Dropping full packet...");
		}
		/* valid packet, call handlers to notify upper layer */
		else if(rc == FIFOPROTOCOL_ERROR_NO) {
			if(packet->type == FIFOPROTOCOL_PACKET_TYPE_ANSWER) {
				response_handler(s, packet->sensorid,
						(FifoProtocolCmd)packet->u.command.cmd,
						packet->arg, packet->size);
			} else {
				data_handler(s, packet->sensorid,
						packet->u.data.timestamp, packet->u.data.status,
						packet->u.data.accuracy, packet->arg, packet->size);
			}
		}
		else {
			/* incomplete packet */
		}

		/* parse next packet */
		index     += FIFOPROTOCOL_PACKET_SIZE;
		remaining -= FIFOPROTOCOL_PACKET_SIZE;
	}

	/* If we did not receive all packets needed for current sensor id to be fully parsed, then make
		sure the already analyzed packet(s) will be part of next call to this function */
	if (rc == FIFOPROTOCOL_ERROR_INCOMPLETE) {
		// INV_MSG(INV_MSG_LEVEL_DEBUG, "Incomplete fifo packet, rewind fifo index.");
		remaining += packet->states.packet_cnt * FIFOPROTOCOL_PACKET_SIZE;
	}

	/* return number of remaining bytes from input buffer */
	return remaining;
}

// read_and_parse_i2c - This routine is modeled off of read_and_parse_fifo(), and maintains some of the FIFOness, which isn't necessarily
// appropriate for I2C, but which works and also helps to maintain the similiarity between the two.
int inv_icm30xxx_read_and_parse_i2c(struct inv_icm30xxx * s)
{
	int rc = 0;
	uint8_t data_in[FIFOPROTOCOL_PACKET_SIZE + 2];			// Add 1 for the number of bytes read and another for the i2c reg addr
	struct FifoProtocolPacket packet;

	inv_fifo_protocol_parse_packet_reset(&packet);	// Unlike the fifo routines, we maintain the packet structure one level up for two
    packet.type = FIFOPROTOCOL_PACKET_TYPE_DATA;	// packet data samples since we haven't read the second packet yet.

	do {
		if ((rc = inv_icm30xxx_serif_read_reg(&s->serif_i2cslave, 0, data_in, FIFOPROTOCOL_PACKET_SIZE + 1)) != 0)
			break;
												// As serif_read_reg does not return the num bytes read. We're cheating
		if (data_in[0])							//  and putting the num bytes in the first byte of the buffer
		    rc = parse_i2c(s, &data_in[2], FIFOPROTOCOL_PACKET_SIZE, &packet);		// Skip the first byte which is the number of bytes read
			                                                                        // and the second bytes with the the I2C reg addr
	} while (rc > 0);                                                               // Loop until we've read the complete data sample.

	return rc;
}

int inv_icm30xxx_ctrl_poll_sensor(struct inv_icm30xxx * s, int fifo_mask)
{
	int rc;

	if(fifo_mask & INV_ICM30XXX_FIFO_ID_2_MASK(INV_ICM30XXX_FIFO_ID_1)) {
		if((rc = read_and_parse_fifo(s, INV_ICM30XXX_FIFO_ID_1)) != 0)
			return rc;
	}

	if(fifo_mask & INV_ICM30XXX_FIFO_ID_2_MASK(INV_ICM30XXX_FIFO_ID_3)) {
		if((rc = read_and_parse_fifo(s, INV_ICM30XXX_FIFO_ID_3)) != 0)
			return rc;
	}

	return 0;
}

static int do_poll_audio_data(struct inv_icm30xxx * s, uint8_t fifo_idx)
{
#define TIMESTAMP_SIZE      4

	int rc;
	uint16_t fifo_count;
	uint32_t count;
	int nb_sample;
	struct inv_icm30xxx_fifo_state * fifo;
	/* compute frame size (expected to be one audio sample + timestamp if enabled) */
	const inv_bool_t timestamp_en = !(s->audio_flag & INV_ICM30XXX_AUDIO_CFG_FLAG_TIMESTAMP_OFF);
	const int sample_size = s->audio_buffer.sample_size + ((timestamp_en) ? TIMESTAMP_SIZE : 0);

	assert(s->variant == INV_ICM30XXX_VARIANT_BERLIN);
	assert(fifo_idx < INV_ICM30XXX_FIFO_ID_MAX);
	assert(s->audio_buffer.buffer && s->audio_buffer.sample_size != 0 &&
			s->audio_buffer.sample_size <= s->audio_buffer.max_size);

	fifo = &s->fifo_states[fifo_idx];

	/* pkt size is null, this is wrong, an error should have been caught this earlier */
	assert(fifo->pkt_size_byte != 0);

	if((rc = inv_icm30xxx_fifo_get_count(s, fifo_idx, &fifo_count)) != 0)
		return rc;

	/* count is expected to be in byte for berlin,
	   so round count on a FIFO packet boundary) */

	fifo_count = (fifo_count / fifo->pkt_size_byte); /* convert to packet count */
	count = fifo_count * fifo->pkt_size_byte; /* convert back to byte */


	/* read what we have in the FIFO */
	nb_sample = count / sample_size;

	/* read all samples, one at a time */
	while(nb_sample) {
		uint32_t timestamp = 0;

		/* read timestamp if configured */
		if(timestamp_en) {
			if((rc = inv_icm30xxx_fifo_read(s, fifo_idx, (uint8_t *)&timestamp,
					sizeof(timestamp))) != 0) {
				return rc;
			}
		}

		/* read audio data */
		if((rc = inv_icm30xxx_fifo_read(s, fifo_idx,
				&s->audio_buffer.buffer[0], s->audio_buffer.sample_size)) != 0) {
			return rc;
		}

		s->audio_buffer.cur_size = s->audio_buffer.sample_size;

		/* notify upper layer */
		s->handler.data(s->handler.context, FIFOPROTOCOL_SENSOR_ID_AUDIO_0, timestamp,
				FIFOPROTOCOL_STATUS_DATAUPDATED, FIFOPROTOCOL_ACCURACY_UNRELIABLE,
				(const uint8_t *)&s->audio_buffer, sizeof(struct inv_icm30xxx_audio_buffer));

		--nb_sample;
	}

	return 0;
}

void inv_icm30xxx_ctrl_stop_poll_audio(struct inv_icm30xxx * s)
{
	assert(s->variant == INV_ICM30XXX_VARIANT_BERLIN);

	s->audio_flag &= ~INV_ICM30XXX_POLL_AUDIO_FLAG;

	inv_icm30xxx_cmd_enable_audio_stream(s, INV_ICM30XXX_AUDIO_CMD_STREAM_STOP);
}

int inv_icm30xxx_ctrl_poll_audio(struct inv_icm30xxx * s, int fifo_mask)
{
	assert(s->variant == INV_ICM30XXX_VARIANT_BERLIN);

	if(fifo_mask & INV_ICM30XXX_FIFO_ID_2_MASK(INV_ICM30XXX_FIFO_ID_2)) {
		/* Set flat to indicate to poll FIFO for audio data */
		INV_MSG(INV_MSG_LEVEL_DEBUG, "Start polling audio fifo...");
		s->audio_flag |= INV_ICM30XXX_POLL_AUDIO_FLAG;
	}

	/* Poll audio FIFO if flag is set or when in streaming mode */
	if(s->audio_flag & INV_ICM30XXX_POLL_AUDIO_FLAG) {
		return do_poll_audio_data(s, INV_ICM30XXX_FIFO_ID_2);
	}

	/* Polling will stop upon explicit request from upper later
	   by calling inv_icm30xxx_ctrl_stop_poll_audio() */

	return 0;
}
