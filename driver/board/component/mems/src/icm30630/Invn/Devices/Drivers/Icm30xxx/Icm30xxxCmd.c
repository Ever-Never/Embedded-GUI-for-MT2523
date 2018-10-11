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

#include "Icm30xxxCmd.h"
#include "Icm30xxx.h"

#include "Invn/FifoProtocol/FifoProtocol.h"
#include "Invn/FifoProtocol/FifoProtocolHelper.h"

#include "Invn/EmbUtils/Message.h"

#include <assert.h>

static int send_cmd(struct inv_icm30xxx * s, uint8_t sensor, uint8_t cmd,
		const uint8_t * payload, uint16_t len, inv_bool_t wait_response)
{
	int rc = 0;

	if(wait_response) {
		s->response.event = false;
	}

	if((rc = inv_icm30xxx_ctrl_send_command(s, sensor, cmd, payload, len)) != 0) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "Error %d while sending command '%d'"
				" (payload: %d bytes)", rc, cmd, len);
		return rc;
	}

	if(wait_response) {
		int timeout = 1000; /* 1s */

		/* poll FIFO until we received the answer */
		do {
			uint8_t fifo_mask;

			if((rc = inv_icm30xxx_read_int_status(s, s->poll_int_mask, &fifo_mask)) != 0)
				return rc;

			if((rc = inv_icm30xxx_ctrl_poll_sensor(s, fifo_mask)) != 0)
				return rc;

			inv_icm30xxx_sleep(2);
			timeout -= 2;

			if(timeout <= 0)
				return INV_ERROR_TIMEOUT;

		} while(!s->response.event);

		if(s->response.sensorid != sensor || s->response.command != cmd) {
			INV_MSG(INV_MSG_LEVEL_ERROR, "Unexpected response for command");
			INV_MSG(INV_MSG_LEVEL_DEBUG, "sensor %d != %d", s->response.sensorid,
					sensor);
			INV_MSG(INV_MSG_LEVEL_DEBUG, "command %d != %d", s->response.command,
					cmd);

			return INV_ERROR_UNEXPECTED;
		}
	}

	return rc;
}


int inv_icm30xxx_cmd_load(struct inv_icm30xxx * s, uint8_t who, uint8_t what,
		uint32_t arg)
{
	int rc;
	uint8_t payload[6];
	const uint16_t len = inv_fifoprotocol_helper_encode_load(who, what,
			arg, payload);

	if((rc = send_cmd(s, FIFOPROTOCOL_SENSOR_ID_META_DATA,
			FIFOPROTOCOL_CMD_LOAD, payload, len, true)) != 0)
		return rc;

	if(s->response.payload.load.who != who ||
				s->response.payload.load.what != what) {

		INV_MSG(INV_MSG_LEVEL_ERROR, "Unexpected response received.");
		INV_MSG(INV_MSG_LEVEL_DEBUG, "who:  received %d vs %d expected",
				s->response.payload.load.who, who);
		INV_MSG(INV_MSG_LEVEL_DEBUG, "what: received %d vs %d expected",
				s->response.payload.load.what, what);

		return INV_ERROR_UNEXPECTED;
	}

	return 0;
}

int inv_icm30xxx_cmd_get_fw_info(struct inv_icm30xxx * s, struct inv_icm30xxx_fw_version * fw)
{
	int rc;

	if((rc = send_cmd(s, FIFOPROTOCOL_SENSOR_ID_META_DATA,
			FIFOPROTOCOL_CMD_GETFIRMWAREINFO, 0, 0, true)) != 0)
		return rc;

	*fw = s->response.payload.fwversion;

	return 0;
}

int inv_icm30xxx_cmd_enable_sensor(struct inv_icm30xxx * s, uint8_t sensorid,
		inv_bool_t state)
{
	const int cmd = (state) ? FIFOPROTOCOL_CMD_SENSORON : FIFOPROTOCOL_CMD_SENSOROFF;

	return send_cmd(s, sensorid, cmd, 0, 0, false);
}

int inv_icm30xxx_cmd_set_running_state(struct inv_icm30xxx * s, inv_bool_t state)
{
	uint8_t payload[4];
	FifoProtocolApPowerMode power = (state) ?
			FIFOPROTOCOL_APPOWERMODE_ON : FIFOPROTOCOL_APPOWERMODE_SUSPEND;
	const uint16_t len = inv_fifo_protocol_helper_encode_power(power, payload);

	return send_cmd(s, FIFOPROTOCOL_SENSOR_ID_META_DATA,
			FIFOPROTOCOL_CMD_SETPOWER, payload, len, false);
}

int inv_icm30xxx_cmd_set_sensor_period(struct inv_icm30xxx * s, uint8_t sensorid,
		uint32_t us_period)
{
	uint8_t payload[4];
	const uint16_t len = inv_fifo_protocol_helper_encode_delay(us_period, payload);

	return send_cmd(s, sensorid, FIFOPROTOCOL_CMD_SETDELAY, payload, len, false);
}

int inv_icm30xxx_cmd_set_sensor_timeout(struct inv_icm30xxx * s, uint8_t sensorid,
		uint32_t ms_timeout)
{
	uint8_t payload[4];
	const uint16_t len = inv_fifo_protocol_helper_encode_timeout(ms_timeout, payload);

	return send_cmd(s, sensorid, FIFOPROTOCOL_CMD_BATCHON, payload, len, false);
}

int inv_icm30xxx_cmd_flush_sensor(struct inv_icm30xxx * s, uint8_t sensorid)
{
	return send_cmd(s, sensorid, FIFOPROTOCOL_CMD_FLUSH, 0, 0, false);
}

int inv_icm30xxx_cmd_set_sensor_mounting_matrix(struct inv_icm30xxx * s, uint8_t sensorid,
		const float matrix[9])
{
	uint8_t payload[9*4];
	// cast because of const attribute missing in prototy of below function
	const uint16_t len = inv_fifo_protocol_helper_encode_referenceframe(
			(float *)matrix, payload);

	return send_cmd(s, sensorid, FIFOPROTOCOL_CMD_SETREFERENCEFRAME,
			payload, len, false);
}

int inv_icm30xxx_cmd_set_sensor_bias(struct inv_icm30xxx * s, uint8_t sensorid,
		const float bias[3])
{
	uint8_t payload[3*4];
	// cast because of const attribute missing in prototy of below function
	const uint16_t len = inv_fifo_protocol_helper_encode_calibrationoffsets(
			(float *)bias, 3*sizeof(float), payload);

	return send_cmd(s, sensorid, FIFOPROTOCOL_CMD_SETCALIBRATIONOFFSETS,
			payload, len, false);
}

int inv_icm30xxx_cmd_get_sensor_bias(struct inv_icm30xxx * s, uint8_t sensorid,
		float bias[3])
{
	int rc;

	if((rc = send_cmd(s, sensorid, FIFOPROTOCOL_CMD_GETCALIBRATIONOFFSETS,
			0, 0, true)) != 0)
		return rc;

	memcpy(bias, s->response.payload.bias, 3*sizeof(float));

	return 0;
}

int inv_icm30xxx_cmd_set_sensor_gain(struct inv_icm30xxx * s, uint8_t sensorid,
		const float gain[9])
{
	uint8_t payload[9*4];
	// cast because of const attribute missing in prototy of below function
	const uint16_t len = inv_fifo_protocol_helper_encode_calibrationgains(
			(float *)gain, 3*sizeof(float), payload);

	return send_cmd(s, sensorid, FIFOPROTOCOL_CMD_SETCALIBRATIONGAINS,
			payload, len, false);
}

int inv_icm30xxx_cmd_get_sensor_gain(struct inv_icm30xxx * s, uint8_t sensorid,
		float gain[9])
{
	int rc;

	if((rc = send_cmd(s, sensorid, FIFOPROTOCOL_CMD_GETCALIBRATIONGAINS,
			0, 0, true)) != 0)
		return rc;

	memcpy(gain, s->response.payload.gain, 9*sizeof(float));

	return 0;
}

int inv_icm30xxx_cmd_get_clock_rate(struct inv_icm30xxx * s, uint32_t * clock_rate)
{
	int rc;

	if((rc = send_cmd(s, FIFOPROTOCOL_SENSOR_ID_META_DATA,
			FIFOPROTOCOL_CMD_GETCLOCKRATE, 0, 0, true)) != 0)
		return rc;

	*clock_rate = s->response.payload.clock_rate;

	return 0;
}

int inv_icm30xxx_cmd_get_last_sensor_data(struct inv_icm30xxx * s, uint8_t sensorid,
		 uint32_t * timestamp, uint8_t * data, uint16_t * len)
{
	int rc;

	if((rc = send_cmd(s, sensorid, FIFOPROTOCOL_CMD_GETDATA,
			0, 0, true)) != 0)
		return rc;

	*len = s->response.payload.sensor_data.len;
	memcpy(data, s->response.payload.sensor_data.data, *len);

	*timestamp = s->response.payload.sensor_data.timestamp;

	return 0;
}

int inv_icm30xxx_cmd_ping_sensor(struct inv_icm30xxx * s, uint8_t sensorid,
		uint8_t * ping_value)
{
	int rc;

	if((rc = send_cmd(s, sensorid, FIFOPROTOCOL_CMD_PING, 0, 0, true)) != 0)
		return rc;

	*ping_value = s->response.payload.ping;

	return 0;
}

int inv_icm30xxx_cmd_reset_sensor(struct inv_icm30xxx * s, uint8_t sensorid)
{
	int rc;

	if((rc = send_cmd(s, sensorid, FIFOPROTOCOL_CMD_RESET, 0, 0, false)) != 0)
		return rc;

	return 0;
}

int inv_icm30xxx_cmd_set_sensor_config(struct inv_icm30xxx * s, uint8_t sensorid,
		int setting, const void * data, uint16_t size)
{
	int rc;
	unsigned char buffer[64] = {0};

	if(size > sizeof(buffer) - 4 /* 4 corresponds to config command header */)
		return INV_ERROR_SIZE;

	buffer[0] = (uint8_t)(setting & 0xFF);
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = (size & 0xFF);
	memcpy(&buffer[4], data, size);

	if((rc = send_cmd(s, sensorid, FIFOPROTOCOL_CMD_SETCONFIG,
			buffer, size + 4 , false)) != 0)
		return rc;

	return 0;
}

int inv_icm30xxx_cmd_get_sensor_config(struct inv_icm30xxx * s, uint8_t sensorid,
		int setting, void * data, uint16_t size)
{
	int rc;
	unsigned char buffer[4] = {0};
	uint8_t buffer_size_received;
	
	if(size < 2)
		return INV_ERROR_SIZE;

	buffer[0] = (uint8_t)(setting & 0xFF);
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = (size & 0xFF);

	if((rc = send_cmd(s, sensorid, FIFOPROTOCOL_CMD_GETCONFIG, buffer, 4, true)) != 0)
		return rc;

	buffer_size_received = (uint8_t)(s->response.payload.config_data[3]);

	memcpy(data, &s->response.payload.config_data[4], buffer_size_received);

	return buffer_size_received;
}

int inv_icm30xxx_cmd_write_mems_register(struct inv_icm30xxx * s, uint8_t sensorid,
		uint16_t reg_addr, const void * data, uint16_t size)
{
	int rc;
	unsigned char buffer[64] = {0};

	if(size > sizeof(buffer) - 4 /* 4 corresponds to config command header */)
		return INV_ERROR_SIZE;

	buffer[0] = 0;
	buffer[1] = (uint8_t)(reg_addr & 0xFF);
	buffer[2] = (uint8_t)((reg_addr & 0xFF00) >> 8);
	buffer[3] = (uint8_t)size;
	memcpy(&buffer[4], data, size);

	if((rc = send_cmd(s, sensorid, FIFOPROTOCOL_CMD_SETCONFIG,
			buffer, size + 4 , false)) != 0)
		return rc;

	return 0;
}

int inv_icm30xxx_cmd_read_mems_register(struct inv_icm30xxx * s, uint8_t sensorid,
		uint16_t reg_addr, void * data, uint16_t size)
{
	int rc;
	unsigned char buffer[4] = {0};

	/* max payload for command is 64 including for 4 for header, hence 60 */
	if(size > 60)
		return INV_ERROR_SIZE;

	buffer[0] = 0;
	buffer[1] = (uint8_t)(reg_addr & 0xFF);
	buffer[2] = (uint8_t)((reg_addr & 0xFF00) >> 8);
	buffer[3] = (uint8_t)size;

	if((rc = send_cmd(s, sensorid, FIFOPROTOCOL_CMD_GETCONFIG, buffer, 4, true)) != 0)
		return rc;

	memcpy(data, &s->response.payload.config_data[4], size);

	return 0;
}

int inv_icm30xxx_cmd_configure_audio(struct  inv_icm30xxx * s,
		uint8_t cmd, uint8_t arg)
{
	const uint8_t payload[4] = { arg, (uint8_t)cmd, 0, 0};

	assert(s->variant == INV_ICM30XXX_VARIANT_BERLIN);

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Configuring audio (cmd=0x%x, arg=0x%x)", cmd, arg);

	return send_cmd(s, FIFOPROTOCOL_ZSP_AUDIO_SENSOR,
			FIFOPROTOCOL_CMD_AUDIO, payload, sizeof(payload), false);
}

int inv_icm30xxx_cmd_enable_kwr(struct  inv_icm30xxx * s, inv_bool_t en)
{
	return inv_icm30xxx_cmd_configure_audio(s, INV_ICM30XXX_AUDIO_CMD_KWR_EN, !!(en));
}

int inv_icm30xxx_cmd_enable_audio_detect(struct  inv_icm30xxx * s, inv_bool_t en)
{
	return inv_icm30xxx_cmd_configure_audio(s, INV_ICM30XXX_AUDIO_CMD_AUDIO_EN, !!(en));
}

int inv_icm30xxx_cmd_enable_audio_stream(struct  inv_icm30xxx * s, int mode)
{
	if(mode == INV_ICM30XXX_AUDIO_CMD_STREAM_OPEN_MIC
			|| mode == INV_ICM30XXX_AUDIO_CMD_STREAM_DUMP) {
		s->audio_flag |= INV_ICM30XXX_POLL_AUDIO_FLAG;
	} else {
		s->audio_flag &= ~INV_ICM30XXX_POLL_AUDIO_FLAG;
	}

	return inv_icm30xxx_cmd_configure_audio(s, INV_ICM30XXX_AUDIO_CMD_STREAMING_MODE, mode);
}

int inv_icm30xxx_cmd_enable_audio_timestamp(struct  inv_icm30xxx * s, inv_bool_t en)
{
	if(!en) {
		s->audio_flag |= INV_ICM30XXX_AUDIO_CFG_FLAG_TIMESTAMP_OFF;
	} else {
		s->audio_flag &= ~INV_ICM30XXX_AUDIO_CFG_FLAG_TIMESTAMP_OFF;
	}

	return inv_icm30xxx_cmd_configure_audio(s, INV_ICM30XXX_AUDIO_CMD_TIMESTAMP_EN, !!(en));
}

#define	ZSP_GRAMMAR_MAX_SIZE     2000        // The max size of the Grammer on the ZSP
#define	ZSP_NET_MAX_SIZE         30000       // The max size of the model on the ZSP
#define	ZSP_GOOGLE_MAX_SIZE      42000       // The max size of the model on the ZSP
#define	ZSP_GRAMMAR_START_ADDR   0x60010000  // This is the start of the temporary audio buffer on the ZSP
#define	ZSP_NET_START_ADDR       0x600107D0  // This is an offset from the same ZSP audio buffer
#define	ZSP_GOOGLE_START_ADDR    0x60010000  // This is an offset from the same ZSP audio buffer

#define MODEL_UPDATE_BEGIN        0
#define MODEL_UPDATE_LOAD_GRAMMAR 1
#define MODEL_UPDATE_LOAD_NET     2
#define MODEL_UPDATE_LOAD_GOOGLE  5
#define MODEL_UPDATE_RESET        3
#define MODEL_UPDATE_END          4

struct kwr_model_info {
	uint32_t        type;
	uint32_t        addr;
	uint32_t        max_size;
	const uint8_t * buffer;
	uint32_t        size;
};

static int cmd_configure_audio_update_kwr_model(struct  inv_icm30xxx * s,
		const struct kwr_model_info * models, unsigned nb_models)
{
	int rc = 0;
	uint8_t pwr_mgmt1;

	assert(s->variant == INV_ICM30XXX_VARIANT_BERLIN);

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Updating ZSP KWR model...");

	/* backup current power sate and wake-up chip */
	rc += inv_icm30xxx_get_power_mgmt(s, &pwr_mgmt1);
	rc += inv_icm30xxx_wake_up(s);

	/* start model update procedure */
	rc += inv_icm30xxx_cmd_configure_audio(s, INV_ICM30XXX_AUDIO_CMD_UPDATE_MODEL,
			MODEL_UPDATE_BEGIN);

	/* load files */
	if(models) {
		unsigned int i;
		for(i = 0; i < nb_models; ++i) {
			if(models[i].size > models[i].max_size)
				return INV_ERROR_SIZE;

			rc += inv_icm30xxx_zsp_mem_write(s, models[i].buffer, models[i].size, models[i].addr);
			rc += inv_icm30xxx_cmd_configure_audio(s, INV_ICM30XXX_AUDIO_CMD_UPDATE_MODEL,
					models[i].type);
		}
	}

	/* reset audio processing */
	rc += inv_icm30xxx_cmd_configure_audio(s, INV_ICM30XXX_AUDIO_CMD_UPDATE_MODEL,
			MODEL_UPDATE_RESET);
	rc += inv_icm30xxx_cmd_configure_audio(s, INV_ICM30XXX_AUDIO_CMD_UPDATE_MODEL,
			MODEL_UPDATE_END);

	/* restore power management */
	rc += inv_icm30xxx_set_power_mgmt(s, pwr_mgmt1);

	if(rc) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "Some error occured while updating ZSP KWR model.");
	}

	return rc;
}

int inv_icm30xxx_cmd_configure_audio_update_kwr_sensory_model(struct  inv_icm30xxx * s,
		const uint8_t * grammar_model, uint32_t size_grammar_model,
		const uint8_t * net_model, uint32_t net_size_model)
{
#define SAMPLE_SIZE_SENSORY INV_ICM30XXX_AUDIO_SAMPLE_SIZE

	struct kwr_model_info models[] = {
		{ MODEL_UPDATE_LOAD_GRAMMAR, ZSP_GRAMMAR_START_ADDR, ZSP_GRAMMAR_MAX_SIZE ,
				grammar_model, size_grammar_model },
		{ MODEL_UPDATE_LOAD_NET, ZSP_NET_START_ADDR, ZSP_NET_MAX_SIZE,
				net_model, net_size_model },
	};

	/* update audio sample size according to model */
	s->audio_buffer.sample_size = SAMPLE_SIZE_SENSORY;

	return cmd_configure_audio_update_kwr_model(s, models, sizeof(models)/sizeof(models[0]));
}

int inv_icm30xxx_cmd_configure_audio_update_kwr_google_model(struct  inv_icm30xxx * s,
		const uint8_t * model, uint32_t size_model)
{
#define SAMPLE_SIZE_GOOGLE (10*16*2) /* 10 ms of audio at 16 k and 16 bits */

	struct kwr_model_info models[] = {
		{ MODEL_UPDATE_LOAD_GOOGLE, ZSP_GOOGLE_START_ADDR, ZSP_GOOGLE_MAX_SIZE,
				model, size_model },
	};

	/* update audio sample size according to model */
	s->audio_buffer.sample_size = SAMPLE_SIZE_GOOGLE;

	return cmd_configure_audio_update_kwr_model(s, models, sizeof(models)/sizeof(models[0]));
}
