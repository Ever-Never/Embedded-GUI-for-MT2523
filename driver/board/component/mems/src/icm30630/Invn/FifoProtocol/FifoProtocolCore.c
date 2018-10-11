/*
* ________________________________________________________________________________________________________
* Copyright (c) 2014-2015 InvenSense Inc. Portions Copyright (c) 2014-2015 Movea. All rights reserved.
*
* This software, related documentation and any modifications thereto (collectively "Software") is subject
* to InvenSense and its licensors' intellectual property rights under U.S. and international copyright and
* other intellectual property rights laws.
* 
* InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
* and any use, reproduction, disclosure or distribution of the Software without an express license
* agreement from InvenSense is strictly prohibited.
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

#include "FifoProtocolCore.h"

#include <string.h>

#ifndef ASSERT
	#include <assert.h>
	#define ASSERT(...) assert(__VA_ARGS__)
#else
	#undef 	ASSERT
	#define ASSERT(...) (void)0
#endif

static const uint8_t sCommandPacketNumber[FIFOPROTOCOL_CMD_MAX] = {
	/* FIFOPROTOCOL_CMD_SENSOROFF 				*/	1,
    /* FIFOPROTOCOL_CMD_SENSORON 				*/	1,
    /* FIFOPROTOCOL_CMD_SETPOWER 				*/	1,
    /* FIFOPROTOCOL_CMD_BATCHON 				*/	1,
    /* FIFOPROTOCOL_CMD_FLUSH 					*/	1,
    /* FIFOPROTOCOL_CMD_SETDELAY 				*/	1,
    /* FIFOPROTOCOL_CMD_SETCALIBRATIONGAINS 	*/	4,
    /* FIFOPROTOCOL_CMD_GETCALIBRATIONGAINS 	*/	1,
    /* FIFOPROTOCOL_CMD_SETCALIBRATIONOFFSETS 	*/	2,
    /* FIFOPROTOCOL_CMD_GETCALIBRATIONOFFSETS 	*/	1,
    /* FIFOPROTOCOL_CMD_SETREFERENCEFRAME 		*/	4,
    /* FIFOPROTOCOL_CMD_GETFIRMWAREINFO 		*/ 	1,
	/* FIFOPROTOCOL_CMD_GETDATA					*/ 	1,
	/* FIFOPROTOCOL_CMD_GETCLOCKRATE			*/ 	1,
	/* FIFOPROTOCOL_CMD_PING					*/	1,
	/* FIFOPROTOCOL_CMD_RESET					*/	1,
	/* FIFOPROTOCOL_CMD_LOAD                    */  2,
	/* FIFOPROTOCOL_CMD_SETCONFIG   			*/  7,
	/* FIFOPROTOCOL_CMD_GETCONFIG  	 			*/  7,
	/* FIFOPROTOCOL_CMD_SETDATAINJECTION        */  1,
	/* FIFOPROTOCOL_CMD_AUDIO			        */  2,
};

static const uint8_t sAnswerPacketNumber[FIFOPROTOCOL_CMD_MAX] = {
	/* FIFOPROTOCOL_CMD_SENSOROFF 				*/	1,
    /* FIFOPROTOCOL_CMD_SENSORON 				*/	1,
    /* FIFOPROTOCOL_CMD_SETPOWER 				*/	1,
    /* FIFOPROTOCOL_CMD_BATCHON 				*/	1,
    /* FIFOPROTOCOL_CMD_FLUSH 					*/	1,
    /* FIFOPROTOCOL_CMD_SETDELAY 				*/	1,
    /* FIFOPROTOCOL_CMD_SETCALIBRATIONGAINS 	*/	1,
    /* FIFOPROTOCOL_CMD_GETCALIBRATIONGAINS 	*/	4,
    /* FIFOPROTOCOL_CMD_SETCALIBRATIONOFFSETS 	*/	1,
    /* FIFOPROTOCOL_CMD_GETCALIBRATIONOFFSETS 	*/	2,
    /* FIFOPROTOCOL_CMD_SETREFERENCEFRAME 		*/	1,
    /* FIFOPROTOCOL_CMD_GETFIRMWAREINFO 		*/ 	2,
	/* FIFOPROTOCOL_CMD_GETDATA					*/ 	1,
	/* FIFOPROTOCOL_CMD_GETCLOCKRATE			*/ 	1,
	/* FIFOPROTOCOL_CMD_PING					*/	1,
	/* FIFOPROTOCOL_CMD_RESET					*/	1,
	/* FIFOPROTOCOL_CMD_LOAD                    */  2,
	/* FIFOPROTOCOL_CMD_SETCONFIG   			*/  0,
	/* FIFOPROTOCOL_CMD_GETCONFIG  	 			*/  7,
	/* FIFOPROTOCOL_CMD_SETDATAINJECTION        */  1,
	/* FIFOPROTOCOL_CMD_AUDIO			        */  0,
};

static const uint8_t sDataPacketNumber[FIFOPROTOCOL_SENSOR_ID_MAX] = 
{
	/* FIFOPROTOCOL_SENSOR_ID_META_DATA 				*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_ACCELEROMETER 			*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_MAGNETOMETER 				*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_ORIENTATION 				*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_GYROSCOPE 				*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_LIGHT 					*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_PRESSURE 					*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_TEMPERATURE 				*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_PROXIMITY 				*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_GRAVITY 					*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_LINEAR_ACCELERATION 		*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_ROTATION_VECTOR 			*/ 2,
	/* FIFOPROTOCOL_SENSOR_ID_HUMIDITY 					*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_AMBIENT_TEMPERATURE 		*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_UNCAL_MAGNETOMETER 		*/ 2,
	/* FIFOPROTOCOL_SENSOR_ID_GAME_ROTATION_VECTOR 		*/ 2,
	/* FIFOPROTOCOL_SENSOR_ID_UNCAL_GYROSCOPE 			*/ 2,
	/* FIFOPROTOCOL_SENSOR_ID_SMD 						*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_STEP_DETECTOR 			*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_STEP_COUNTER 				*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_GEOMAG_ROTATION_VECTOR 	*/ 2,
	/* FIFOPROTOCOL_SENSOR_ID_HEART_RATE 				*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_TILT_DETECTOR 			*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_WAKE_GESTURE 				*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_GLANCE_GESTURE 			*/ 1,
	/* FIFOPROTOCOL_SENSOR_ID_PICK_UP_GESTURE 			*/ 1,
	/* empty 											*/ 0,
	/* empty 											*/ 0,
	/* empty 											*/ 0,
	/* empty 											*/ 0,
	/* empty 											*/ 0,
	/* FIFOPROTOCOL_SENSOR_ID_ACTIVITY_CLASSIFIER 		*/ 1,
};

/* Array that contains expected packet count for 'EXT' and 'CUST' sensors
   as this may vary from one chip to another depending on which is embedded in the application
   by default, a value of 0 means 'max packet count'
*/
static uint8_t sDataPacketNumberExt[FIFOPROTOCOL_SENSOR_ID_EXT_CNT+FIFOPROTOCOL_SENSOR_ID_CUST_CNT];

void inv_fifo_protocol_data_packet_number_set_r(uint8_t sensorid, uint8_t number,
		uint8_t * ext_exp_data_pkt_cnt)
{
	sensorid &= ~FIFOPROTOCOL_SENSOR_ID_WAKEUP_FLAG;

	ASSERT(sensorid >= FIFOPROTOCOL_SENSOR_ID_EXT_BASE &&
			sensorid < FIFOPROTOCOL_SENSOR_ID_CUST_MAX);
	ASSERT(number > 0 && number <= FIFOPROTOCOL_NUMBER_MAX);

	ext_exp_data_pkt_cnt[sensorid - FIFOPROTOCOL_SENSOR_ID_EXT_BASE] = number;
}

void inv_fifo_protocol_data_packet_number_set(uint8_t sensorid, uint8_t number)
{
	inv_fifo_protocol_data_packet_number_set_r(sensorid, number, sDataPacketNumberExt);
}

static uint8_t get_expected_packet_number_data(uint8_t sensorid, const uint8_t * ext_exp_data_pkt_cnt)
{
	uint8_t packet_number = 0;
	sensorid &= ~FIFOPROTOCOL_SENSOR_ID_WAKEUP_FLAG;

	if(sensorid < FIFOPROTOCOL_SENSOR_ID_MAX) {
		packet_number = sDataPacketNumber[sensorid];
	}
	else if(sensorid < FIFOPROTOCOL_SENSOR_ID_CUST_MAX) {
		packet_number = ext_exp_data_pkt_cnt[sensorid - FIFOPROTOCOL_SENSOR_ID_EXT_BASE];
	}

	if(packet_number == 0)
		packet_number = FIFOPROTOCOL_NUMBER_MAX;

	return packet_number;
}

static uint8_t get_expected_packet_number_answer(uint8_t command)
{
	uint8_t packet_number = 0;
	
	if(command < FIFOPROTOCOL_CMD_MAX) {
		packet_number = sAnswerPacketNumber[command];
	}

	return packet_number;
}

static uint8_t get_expected_packet_number_command(uint8_t command)
{
	uint8_t packet_number = 0;
	
	if(command < FIFOPROTOCOL_CMD_MAX) {
		packet_number = sCommandPacketNumber[command];
	}

	return packet_number;
}

/*************************************************************/

void inv_fifo_protocol_parse_packet_reset(struct FifoProtocolPacket *packet)
{
	packet->states.packet_cnt = 0;
}

int inv_fifo_protocol_parse_header_packet(struct FifoProtocolPacket *packet,
	const uint8_t * raw_packet
)
{    
	while( raw_packet[packet->protected_header_idx] != FIFOPROTOCOL_PROTECTED_HEADER ) {
		packet->protected_header_idx++;
		if(packet->protected_header_idx == FIFOPROTOCOL_PACKET_SIZE) {
			return FIFOPROTOCOL_ERROR_PARSE;
		}
	}
	       
	if(packet->protected_header_idx != 0) {
		return FIFOPROTOCOL_ERROR_INCOMPLETE;
	}
	
	return FIFOPROTOCOL_ERROR_NO;
}

int inv_fifo_protocol_parse_footer_packet(struct FifoProtocolPacket *packet, const uint8_t * raw_packet)
{    
	if( raw_packet[FIFOPROTOCOL_PACKET_SIZE-1] != (uint8_t)FIFOPROTOCOL_PROTECTED_FOOTER ) {
		/* since footer is not ok, indicate that next valid header can only be starting from next byte */
		packet->protected_header_idx++;
		return FIFOPROTOCOL_ERROR_PARSE;
	}
	
	return FIFOPROTOCOL_ERROR_NO;
}

int inv_fifo_protocol_parse_packet_r(struct FifoProtocolPacket *packet,
	const uint8_t * raw_packet, uint16_t size, const uint8_t * ext_exp_data_pkt_cnt
)
{
	int rc =  FIFOPROTOCOL_ERROR_NO;
	uint8_t sensorid, packet_number, answer;

	/* sanity check */
	if(size < FIFOPROTOCOL_PACKET_SIZE) {
		rc = FIFOPROTOCOL_ERROR_SIZE;
		goto end;
	}

	sensorid = (raw_packet[1] & ~FIFOPROTOCOL_ANSWER_MASK); /* clear answer bit */
	packet_number = (raw_packet[2] & FIFOPROTOCOL_NUMBER_MASK) >> FIFOPROTOCOL_NUMBER_SHIFT;
	answer = !!(raw_packet[1] & FIFOPROTOCOL_ANSWER_MASK); /* get answer bit */

	if(packet->states.packet_cnt == 0) {
		/* sanity check */
		if(packet_number != 0)
			return FIFOPROTOCOL_ERROR_PARSE;

		packet->sensorid = sensorid;
		
		/* as there is still no way to distinguish command from answer or data
			if packet->type is already set, we expect command
		*/
		if(packet->type == FIFOPROTOCOL_PACKET_TYPE_COMMAND) {
			/* sanity check */
			if(!answer)
				return FIFOPROTOCOL_ERROR_PARSE;

			packet->u.command.cmd = raw_packet[FIFOPROTOCOL_HEADER_SIZE];
			packet->states.expected_packet_number = get_expected_packet_number_command(packet->u.command.cmd);
		} else { /* we check for answer flag in sensor id */
			if(answer) {
				packet->type = FIFOPROTOCOL_PACKET_TYPE_ANSWER;
				packet->u.command.cmd = raw_packet[FIFOPROTOCOL_HEADER_SIZE];
				packet->states.expected_packet_number = get_expected_packet_number_answer(packet->u.command.cmd);
			} else {
				packet->type = FIFOPROTOCOL_PACKET_TYPE_DATA;
				packet->u.data.accuracy = (raw_packet[2] & FIFOPROTOCOL_ACCURACY_MASK) >> FIFOPROTOCOL_ACCURACY_SHIFT;
				packet->u.data.status = (raw_packet[2] & FIFOPROTOCOL_STATUS_MASK) >> FIFOPROTOCOL_STATUS_SHIFT;
				packet->u.data.timestamp = (uint32_t)(raw_packet[FIFOPROTOCOL_HEADER_SIZE]);
				packet->u.data.timestamp |= (uint32_t)(raw_packet[FIFOPROTOCOL_HEADER_SIZE + 1] << 8);
				packet->u.data.timestamp |= (uint32_t)(raw_packet[FIFOPROTOCOL_HEADER_SIZE + 2] << 16);
				packet->u.data.timestamp |= (uint32_t)(raw_packet[FIFOPROTOCOL_HEADER_SIZE + 3] << 24);
				packet->states.expected_packet_number = get_expected_packet_number_data(sensorid, ext_exp_data_pkt_cnt);
			}
		}

		memcpy(&packet->arg[0], &raw_packet[FIFOPROTOCOL_HEADER_SIZE + FIFOPROTOCOL_TIMESTAMP_SIZE],
			FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_TIMESTAMP_SIZE - FIFOPROTOCOL_FOOTER_SIZE);
		packet->size = FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_TIMESTAMP_SIZE - FIFOPROTOCOL_FOOTER_SIZE;

		if(packet->states.expected_packet_number == 0 || 
			packet->states.expected_packet_number > FIFOPROTOCOL_NUMBER_MAX) {
			rc = FIFOPROTOCOL_ERROR_SIZE;
			goto end;
		}
	} else {
		const uint16_t idx = (FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_TIMESTAMP_SIZE - FIFOPROTOCOL_FOOTER_SIZE)
			+ (packet->states.packet_cnt-1)*(FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_FOOTER_SIZE);

		/* sanity checks */
		if(sensorid != packet->sensorid || packet_number != (packet->states.packet_cnt) ||
			(answer && packet->type == FIFOPROTOCOL_PACKET_TYPE_DATA) ||
			(!answer && packet->type != FIFOPROTOCOL_PACKET_TYPE_DATA)) {
			rc = FIFOPROTOCOL_ERROR_PARSE;
			goto end;
		}

		memcpy(&packet->arg[idx], &raw_packet[FIFOPROTOCOL_HEADER_SIZE], FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_FOOTER_SIZE);
		packet->size += FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_FOOTER_SIZE;
	}

	if(++packet->states.packet_cnt < packet->states.expected_packet_number)
		return FIFOPROTOCOL_ERROR_INCOMPLETE;

end:
	inv_fifo_protocol_parse_packet_reset(packet);
	return rc;
}

int inv_fifo_protocol_parse_packet(struct FifoProtocolPacket *packet,
	const uint8_t * raw_packet, uint16_t size
)
{
	return inv_fifo_protocol_parse_packet_r(packet, raw_packet, size, sDataPacketNumberExt);
}

int inv_fifo_protocol_parse_command_packet(struct FifoProtocolPacket *packet, const uint8_t * raw_packet, uint16_t size)
{
	ASSERT(packet);

	packet->type = FIFOPROTOCOL_PACKET_TYPE_COMMAND;

	return inv_fifo_protocol_parse_packet(packet, raw_packet, size);
}

/**************************************************/

static void format_command_answer_set(struct FifoProtocolPacket * packet, 
	int sensorid, enum FifoProtocolCmd cmd, const void * arg, uint16_t size
)
{
	ASSERT(size <= FIFOPROTOCOL_PAYLOAD_SIZE_MAX);

	packet->sensorid = sensorid;
	packet->u.command.cmd = cmd;
	packet->size = size;
	memcpy(packet->arg, arg, size);
	packet->states.packet_cnt = 0;
}

void inv_fifo_protocol_command_packet_set(struct FifoProtocolPacket * packet, 
	int sensorid, enum FifoProtocolCmd cmd, const void * arg, uint16_t size
)
{
	ASSERT(packet);

	format_command_answer_set(packet, sensorid, cmd, arg, size);
	packet->type = FIFOPROTOCOL_PACKET_TYPE_COMMAND;
}

void inv_fifo_protocol_answer_packet_set(struct FifoProtocolPacket * packet, 
	int sensorid, enum FifoProtocolCmd cmd, const void * arg, uint16_t size
)
{
	ASSERT(packet);

	format_command_answer_set(packet, sensorid, cmd, arg, size);
	packet->type = FIFOPROTOCOL_PACKET_TYPE_ANSWER;
}

void inv_fifo_protocol_data_packet_set(struct FifoProtocolPacket * packet, 
	int sensorid, uint32_t timestamp, uint8_t accuracy, uint8_t status, const void * data, uint16_t size
)
{
	ASSERT(packet);
	ASSERT(size <= FIFOPROTOCOL_PAYLOAD_SIZE_MAX);

	packet->type = FIFOPROTOCOL_PACKET_TYPE_DATA;
	packet->sensorid = sensorid;
	packet->u.data.timestamp = timestamp;
	packet->u.data.accuracy = accuracy;
	packet->u.data.status = status;
	memcpy(packet->arg, data, size);
	packet->size = size;
	packet->states.packet_cnt = 0;
}

void inv_fifo_protocol_format_packet_reset(struct FifoProtocolPacket *packet)
{
	packet->states.packet_cnt = 0;
}

int inv_fifo_protocol_format_packet_r(struct FifoProtocolPacket *packet,
	uint8_t * raw_packet, uint16_t * size, const uint8_t * ext_exp_data_pkt_cnt
)
{
	int rc = FIFOPROTOCOL_ERROR_NO;

	ASSERT(raw_packet && size);
	ASSERT(packet->type == FIFOPROTOCOL_PACKET_TYPE_ANSWER ||
		packet->type == FIFOPROTOCOL_PACKET_TYPE_DATA ||
		packet->type == FIFOPROTOCOL_PACKET_TYPE_COMMAND
	);
    
	raw_packet[0] = FIFOPROTOCOL_PROTECTED_HEADER;
	raw_packet[1] = packet->sensorid;
	raw_packet[2] = 0;

	if(packet->type != FIFOPROTOCOL_PACKET_TYPE_DATA) {
		raw_packet[1] |= FIFOPROTOCOL_ANSWER_MASK;
	}

	if(packet->states.packet_cnt == 0) {
		if(packet->type == FIFOPROTOCOL_PACKET_TYPE_DATA) {
			raw_packet[2] |= (packet->u.data.accuracy << FIFOPROTOCOL_ACCURACY_SHIFT) & FIFOPROTOCOL_ACCURACY_MASK;
			raw_packet[2] |= (packet->u.data.status << FIFOPROTOCOL_STATUS_SHIFT) & FIFOPROTOCOL_STATUS_MASK;
			raw_packet[FIFOPROTOCOL_HEADER_SIZE] = (packet->u.data.timestamp & 0xFF);
			raw_packet[FIFOPROTOCOL_HEADER_SIZE+1] = (packet->u.data.timestamp & 0xFF00) >> 8;
			raw_packet[FIFOPROTOCOL_HEADER_SIZE+2] = (packet->u.data.timestamp & 0xFF0000) >> 16;
			raw_packet[FIFOPROTOCOL_HEADER_SIZE+3] = (packet->u.data.timestamp & 0xFF000000) >> 24;
			packet->states.expected_packet_number = get_expected_packet_number_data(packet->sensorid, ext_exp_data_pkt_cnt);
		} else {
			raw_packet[FIFOPROTOCOL_HEADER_SIZE] = packet->u.command.cmd;
			raw_packet[FIFOPROTOCOL_HEADER_SIZE+1] = 0;
			raw_packet[FIFOPROTOCOL_HEADER_SIZE+2] = 0;
			raw_packet[FIFOPROTOCOL_HEADER_SIZE+3] = 0;
			if(packet->type == FIFOPROTOCOL_PACKET_TYPE_COMMAND) {
				packet->states.expected_packet_number = get_expected_packet_number_command(packet->u.command.cmd);
			} else {
				packet->states.expected_packet_number = get_expected_packet_number_answer(packet->u.command.cmd);
			}
		}
		
		memcpy(&raw_packet[FIFOPROTOCOL_HEADER_SIZE + FIFOPROTOCOL_TIMESTAMP_SIZE], packet->arg, 
			FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_TIMESTAMP_SIZE - FIFOPROTOCOL_FOOTER_SIZE);
        
		if(packet->states.expected_packet_number == 0 ||
			packet->states.expected_packet_number > FIFOPROTOCOL_NUMBER_MAX) {
			rc = FIFOPROTOCOL_ERROR_SIZE;
			goto end;
		}		
	}
	else {
		const uint16_t idx = (FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_TIMESTAMP_SIZE - FIFOPROTOCOL_FOOTER_SIZE)
			+ (packet->states.packet_cnt-1)*(FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_FOOTER_SIZE);

		memcpy(&raw_packet[FIFOPROTOCOL_HEADER_SIZE], &packet->arg[idx], FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_FOOTER_SIZE);        
	}

	*size = FIFOPROTOCOL_PACKET_SIZE;
	raw_packet[ FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_FOOTER_SIZE ] = FIFOPROTOCOL_PROTECTED_FOOTER;
	raw_packet[2] |= packet->states.packet_cnt << FIFOPROTOCOL_NUMBER_SHIFT;
	++packet->states.packet_cnt;

	if(packet->states.packet_cnt < packet->states.expected_packet_number)
		return FIFOPROTOCOL_ERROR_INCOMPLETE;

end:
	inv_fifo_protocol_format_packet_reset(packet);
	return rc;
}

int inv_fifo_protocol_format_packet(struct FifoProtocolPacket *packet,
	uint8_t * raw_packet, uint16_t * size
)
{
	return inv_fifo_protocol_format_packet_r(packet, raw_packet, size, sDataPacketNumberExt);
}
