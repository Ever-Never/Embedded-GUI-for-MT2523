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

#include "Invn/FifoProtocol/FifoProtocol.h"
#include "Invn/FifoProtocol/FifoProtocolDriverWrapper.h"

int inv_fifo_protocol_driver_wrapper_fifo_write(FifoProtocolType fifo,
		const uint8_t * data, uint16_t len)
{
	(void)fifo, (void)data, (void)len;

	return 0;
}

int inv_fifo_protocol_driver_wrapper_fifo_read(FifoProtocolType fifo,
		uint8_t * data, uint16_t len)
{
	(void)fifo, (void)data, (void)len;

	return 0;
}

int inv_fifo_protocol_driver_wrapper_fifo_size(FifoProtocolType fifo, uint16_t * len)
{
	(void)fifo, (void)len;

	return 0;
}

void inv_fifo_protocol_new_data_handler(int sensorid, FifoProtocolStatus status,
		uint32_t timestamp, FifoProtocolAccuracy accuracy,
		const void * data, uint16_t len)
{
	(void)sensorid, (void)status, (void)timestamp;
	(void)accuracy, (void)data, (void)len;
}

void inv_fifo_protocol_new_answer_handler(int sensorid, FifoProtocolCmd command,
		const void * data, uint16_t len)
{
	(void)sensorid, (void)command, (void)data, (void)len;
}

int inv_fifo_protocol_driver_wrapper_trigger_flush(FifoProtocolType fifo,
		uint8_t now)
{
	(void)fifo, (void)now;

    return 0;
}

int inv_fifo_protocol_driver_wrapper_get_fifo_config(FifoProtocolType fifo,
		FifoProtocolOpt * opt, unsigned * size, unsigned * watermark)
{
	(void)fifo, (void)opt, (void)size, (void)watermark;

	return FIFOPROTOCOL_ERROR_NO;
}

int inv_fifo_protocol_driver_wrapper_set_fifo_config(FifoProtocolType fifo,
		FifoProtocolOpt opt, unsigned size, unsigned watermark)
{
	(void)fifo, (void)opt, (void)size, (void)watermark;

	return FIFOPROTOCOL_ERROR_NO;
}

void inv_fifo_protocol_new_command_handler(int sensorid, FifoProtocolCmd command,
		const void * data, uint16_t len)
{
	(void)sensorid, (void)command, (void)data, (void)len;
}
