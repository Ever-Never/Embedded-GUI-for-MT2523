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

/** @defgroup 	FifoProtocolCore	FifoProtocolCore
	@brief 		Reentrant API to parse/format FIFO Protocol packet
	@ingroup 	FifoProtocol
	@{ 
*/

#ifndef _INV_FIFO_PROTOCOL_CORE_H_
#define _INV_FIFO_PROTOCOL_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "FifoProtocolDefs.h"
#include "FifoProtocolSensorId.h"

/** @brief Packet structure definitions
*/
struct FifoProtocolPacket
{
	enum {
		FIFOPROTOCOL_PACKET_TYPE_DATA,
		FIFOPROTOCOL_PACKET_TYPE_COMMAND,
		FIFOPROTOCOL_PACKET_TYPE_ANSWER
	} type;                                /**< packet type among DATA, COMMAND, ANSWER */
	uint8_t sensorid;                      /**< sensor id */
	union {
		struct {
			uint8_t		status;            /**< data status */
			uint8_t		accuracy;          /**< data accuracy */
			uint32_t 	timestamp;         /**< data timestamp */
		} data;
		struct {
			uint8_t 	cmd;               /**< command id */
		} command;
	} u;
	uint8_t		arg[FIFOPROTOCOL_PAYLOAD_SIZE_MAX];  	/**< buffer for frame payload */
	uint16_t 	size;					/**< payload size */
	uint8_t 	protected_header_idx;			/**< byte index in the packet at which valid header was found */
	struct {
		uint8_t expected_packet_number;
		uint8_t packet_cnt;
	} states; /**< states field use by parser/formater */
};

/** @brief 		Specify expected packet count for EXT sensor
	@param[in]	sensorid    sensor id in extension range
	@param[in] 	number      expected packet count
	@return 	none
*/
void inv_fifo_protocol_data_packet_number_set(uint8_t sensorid, uint8_t number);

/** @brief 		Rentrant version of inv_fifo_protocol_data_packet_number_set()
	@param[in]	sensorid    sensor id in extension range
	@param[in] 	number      expected packet count
	@param[out]	exp_pkt_cnt pointer to array of size (FIFOPROTOCOL_SENSOR_ID_EXT_CNT
	                        + FIFOPROTOCOL_SENSOR_ID_CUST_CNT) that will hold expected
	                        packet count for EXT and CUST sensor
	@return 	none
*/
void inv_fifo_protocol_data_packet_number_set_r(uint8_t sensorid, uint8_t number,
		uint8_t * exp_pkt_cnt);

/** @brief 		Reset parser for data/answer
	@param[in]	packet 		handle to packet data structure
	@return 	none
*/
void inv_fifo_protocol_parse_packet_reset(struct FifoProtocolPacket *packet);

/** @brief 		Reset parser for command
	@param[in]	packet 		handle to packet data structure
	@return 	none
*/
void inv_fifo_protocol_parse_command_packet_reset(struct FifoProtocolPacket *packet);

/** @brief 		Parse a data or answer from a raw packet, looking for header FIFOPROTOCOL_PROTECTED_HEADER symbol
	@param[out]	packet 		handle to packet data structure, which will contain the index
					at which the expected header was found inside the packet
	@param[in]	raw_packet  	incoming raw packet buffer to be parsed
	@return 	- FIFOPROTOCOL_ERROR_INCOMPLETE if header was found but not at start of the packed,
			- FIFOPROTOCOL_ERROR_PARSE if no header was found in the whole packet,
			- FIFOPROTOCOL_ERROR_NO otherwise
*/
int inv_fifo_protocol_parse_header_packet(struct FifoProtocolPacket *packet,
	const uint8_t * raw_packet
);

/** @brief 		Reentrant version of inv_fifo_protocol_parse_header_packet()
	@param[out]	packet 		handle to packet data structure, which will contain the index
					at which the expected header was found inside the packet
	@param[in]	raw_packet  	incoming raw packet buffer to be parsed
	@param[in]	exp_pkt_cnt pointer to array of size (FIFOPROTOCOL_SENSOR_ID_EXT_CNT
	                        + FIFOPROTOCOL_SENSOR_ID_CUST_CNT) that holds expected
	                        packet count for EXT and CUST sensor
	@return 	- FIFOPROTOCOL_ERROR_INCOMPLETE if header was found but not at start of the packed,
			- FIFOPROTOCOL_ERROR_PARSE if no header was found in the whole packet,
			- FIFOPROTOCOL_ERROR_NO otherwise
*/
int inv_fifo_protocol_parse_packet_r(struct FifoProtocolPacket *packet,
	const uint8_t * raw_packet, uint16_t size, const uint8_t * exp_pkt_cnt
);

/** @brief 		Parse a data or answer from a raw packet, looking for footer FIFOPROTOCOL_PROTECTED_FOOTER symbol
				in the last byte of the raw packet
	@param[out]	packet 		handle to packet data structure, which will contain the index
					at which the expected header might be found inside the packet
	@param[in]	raw_packet  	incoming raw packet buffer to be parsed
	@return 	- FIFOPROTOCOL_ERROR_PARSE if no footer was found in the whole packet, and then packet is updated
			- FIFOPROTOCOL_ERROR_NO otherwise
*/
int inv_fifo_protocol_parse_footer_packet(struct FifoProtocolPacket *packet,
    const uint8_t * raw_packet
);

/** @brief 		Try to parse a command from a raw packet
	@param[in]	packet 		handle to packet data structure
	@param[in]	raw_packet  	raw packet buffer
	@param[in]	size  		raw packet buffer size (expected to be FIFOPROTOCOL_PACKET_SIZE)
	@return 	FifoProtocolError
*/
int inv_fifo_protocol_parse_command_packet(struct FifoProtocolPacket *packet,
	const uint8_t * raw_packet, uint16_t size);

/** @brief 		Try to parse a data or answer from a raw packet
				To be called again if FIFOPROTOCOL_ERROR_INCOMPLETE is returned.
	@param[in]	packet 		handle to packet data structure
	@param[in]	raw_packet  raw packet buffer
	@param[in]	size  		raw packet buffer size (expected to be equal to FIFOPROTOCOL_PACKET_SIZE)
	@return 	FifoProtocolError
*/
int inv_fifo_protocol_parse_packet(struct FifoProtocolPacket *packet,
	const uint8_t * raw_packet, uint16_t size
);

/** @brief 		Fill packet structure with command
	@param[out]	packet 		handle to packet data structure
	@param[in]	sensorid  	sensor id for command
	@param[in]	cmd  		command id
	@param[in]	arg  		pointer to payload buffer
	@param[in]	len  		length of payload buffer
	@return 	none
*/
void inv_fifo_protocol_command_packet_set(struct FifoProtocolPacket * packet, 
	int sensorid, enum FifoProtocolCmd cmd, const void * arg, uint16_t size
);

/** @brief 		Fill packet structure with answer
	@param[out]	packet 		handle to packet data structure
	@param[in]	sensorid  	sensor id for answer
	@param[in]	cmd  		command id for answer
	@param[in]	arg  		pointer to payload buffer
	@param[in]	len  		length of payload buffer
	@return 	none
*/
void inv_fifo_protocol_answer_packet_set(struct FifoProtocolPacket * packet, 
	int sensorid, enum FifoProtocolCmd cmd, const void * arg, uint16_t size
);

/** @brief 		Fill packet structure with data
	@param[out]	packet 		handle to packet data structure
	@param[in]	sensorid 	sensor id
	@param[in]	timestamp 	timestamp of the data
	@param[in]	accuracy 	accuracy of the data
	@param[in]	status	 	status	of the data
	@param[in]	data 		sensor data payload
	@param[in]	len  		length of payload buffer
	@return 	none
*/
void inv_fifo_protocol_data_packet_set(struct FifoProtocolPacket * packet, 
	int sensorid, uint32_t timestamp, uint8_t accuracy, uint8_t status, const void * data, uint16_t size
);

/** @brief 		Reset internal states for packet formater
	@param[in]	packet 		handle to packet data structure
	@return 	none
*/
void inv_fifo_protocol_format_packet_reset(struct FifoProtocolPacket * packet);

/** @brief 		Generate raw packet from a packet structure
				To be called again if FIFOPROTOCOL_ERROR_INCOMPLETE is returned.
	@param[in]	packet 		handle to packet data structure
	@param[out]	raw_packet 	pointer to raw_packet place holder
	@param[out]	size 		raw_packet size (expected to be equal to FIFOPROTOCOL_PACKET_SIZE)
	@return 	FifoProtocolError
*/
int inv_fifo_protocol_format_packet(struct FifoProtocolPacket * packet,
	uint8_t * raw_packet, uint16_t * size
);

/** @brief 		Rentrant version of inv_fifo_protocol_format_packet()
				To be called again if FIFOPROTOCOL_ERROR_INCOMPLETE is returned.
	@param[in]	packet 		handle to packet data structure
	@param[in]	packet 		handle to packet data structure
	@param[out]	raw_packet 	pointer to raw_packet place holder
	@param[out]	size 		raw_packet size (expected to be equal to FIFOPROTOCOL_PACKET_SIZE)
	@param[in]	exp_pkt_cnt pointer to array of size (FIFOPROTOCOL_SENSOR_ID_EXT_CNT
	                        + FIFOPROTOCOL_SENSOR_ID_CUST_CNT) that holds expected
	                        packet count for EXT and CUST sensor
	@return 	FifoProtocolError
*/
int inv_fifo_protocol_format_packet_r(struct FifoProtocolPacket *packet,
	uint8_t * raw_packet, uint16_t * size, const uint8_t * ext_exp_data_pkt_cnt
);

#ifdef __cplusplus
}
#endif

#endif /* _INV_FIFO_PROTOCOL_CORE_H_ */

/** @} */
