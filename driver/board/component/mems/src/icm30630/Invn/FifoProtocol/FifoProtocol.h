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

/** @defgroup 	FifoProtocol	FifoProtocol
 	@brief 		Main interface to FIFO Protocol
	@ingroup 	FifoProtocolAdapter
    @{ 
*/

#ifndef _INV_FIFO_PROTOCOL_H_
#define _INV_FIFO_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "FifoProtocolDefs.h"
#include "FifoProtocolSensorId.h"
#include "FifoProtocolCore.h"

/*	Methods to be called by the Sensor Hub */

/** @brief 		Pop a single protocol command packet from the COMMAND FIFO.
                The decoding of the command packet will be done in this function as well.
	@return 	FifoProtocolError
*/
int inv_fifo_protocol_pop_command_packet(FifoProtocolType fifo);

/** @brief 		Notify a sub-system of a new command.
	@warning 	Hook function to be implemented by the sub-system that calls FIFO Protocol APIs.
    @details    This API must be implemented by a sub-system that uses the FIFO Protocol to
                communicate with the host. For example, the FIFO_Protocol_Front_End should implement
                this function allowing it to be notfied when a new command is receieved.
	@param[in]	sensorid 	sensor id
	@param[in] 	command 	command id
	@param[in]	data 		command payload
	@param[in]	len 		length of command payload
	@return 	none
*/
extern void inv_fifo_protocol_new_command_handler(int sensorid,
                FifoProtocolCmd command, const void *data, uint16_t len);

/** @brief      Push sensor data to the specified DATA FIFO.
                The encodfing of the data to meet the protocol definition will be done in this function as well.
	@param[in]	fifo 		fifo to push the packet to
	@param[in]	sensorid 	sensor id
	@param[in]	status	 	status	of the data
	@param[in]	timestamp 	timestamp of the data
	@param[in]	data 		sensor data payload
	@param[in]	len 		length of sensor data payload
	@return 	FifoProtocolError
*/
int inv_fifo_protocol_push_data_packet(FifoProtocolType fifo, int sensorid,
                                       FifoProtocolStatus status, uint32_t timestamp,
                                       FifoProtocolAccuracy accuracy, const void *data, uint16_t len);

/** @brief      Push an answer to the specified FIFO.
                The encodfing of answer packet will be done in this function as well.
	@param[in]	fifo 		fifo to push the packet to
	@param[in]	sensorid	sensor id
	@param[in]	command 	command id
	@param[in]	data 		answer payload
	@param[in]	len 		length of data payload
	@return 	FifoProtocolError
*/
int inv_fifo_protocol_push_answer_packet(FifoProtocolType fifo, int sensorid,
                FifoProtocolCmd command, const void *data, uint16_t len);

/** @brief Trigger an explicit flush
	@param[in]	fifo 		fifo to flush
	@param[in]	mustFlushNow the flush must happen quickly (in case of FIFO overflow for instance) or not
	@return 	FifoProtocolError
*/
int inv_fifo_protocol_trigger_flush(FifoProtocolType fifo,
                                    uint8_t mustFlushNow);


/*	Methods to be called by the Host */

/** @brief 		Pop a single protocol data packet from the specified DATA FIFO.
                The decoding of the data packet will be done in this function as well.
    @param[in]	fifo 		fifo to push the packet to
	@return 	FifoProtocolError
*/
int inv_fifo_protocol_pop_data_packet(FifoProtocolType fifo);

/** @brief 		Notify a sub-system of a new data packet.
	@warning 	Hook function to be implemented by the sub-system that calls FIFO Protocol APIs.
    @details    This API must be implemented by a sub-system that uses the FIFO Protocol to
                communicate with the Sensor Hub. For example, the Sensor Hub kernel driver should
                implement this function allowing it to be notfied when a new data packet is receieved.
	@param[in]	sensorid 	sensor id
	@param[in]	status	 	status	of the data
	@param[in]	timestamp 	timestamp of the data
	@param[in]	data 		sensor data payload
	@param[in]	len 		length of sensor data payload
	@return 	none
*/
extern void inv_fifo_protocol_new_data_handler(int sensorid,
                FifoProtocolStatus status, uint32_t timestamp,
                FifoProtocolAccuracy accuracy, const void *data, uint16_t len);

extern void inv_fifo_protocol_new_answer_handler(int sensorid,
                FifoProtocolCmd command, const void *data, uint16_t len);

/** @brief      Push a command to the COMMAND FIFO.
                The encodfing of the command to meet the protocol definition will be done in this function as well.
	@param[in]	fifo 		fifo to push the packet to
	@param[in]	sensorid 	sensor id
	@param[in] 	command 	command id
	@param[in]	data 		command payload
	@param[in]	len 		length of command payload
	@return 	FifoProtocolError
*/
int inv_fifo_protocol_push_command_packet(FifoProtocolType fifo, int sensorid,
                FifoProtocolCmd command, const void *data, uint16_t len);


/*	Common methods that can be called either by the sensor hub or the host */

/**	@brief Update configuration of a FIFO.
	@param[in]	fifo 		fifo to configure
	@param[in]	opt 		option flag
	@param[in]	size 		requested size of the FIFO
	@param[in]	waterlark	watermark threshold
	@return  	FifoProtocolError
*/
int inv_fifo_protocol_set_fifo_config(FifoProtocolType fifo,
                                      FifoProtocolOpt opt, unsigned size, unsigned watermark);

/**	@brief Return configuration of a FIFO.
	@param[in]	fifo 		fifo to get configuration from
	@param[out]	opt 		current option flag
	@param[out]	size 		current size of the FIFO
	@param[out]	waterlark	current watermark threshold
	@retrun  	FifoProtocolError
*/
int inv_fifo_protocol_get_fifo_config(FifoProtocolType fifo,
                                      FifoProtocolOpt *opt, unsigned *size, unsigned *watermark);

#ifdef __cplusplus
}
#endif

#endif /* _INV_FIFO_PROTOCOL_H_ */

/** @} */
