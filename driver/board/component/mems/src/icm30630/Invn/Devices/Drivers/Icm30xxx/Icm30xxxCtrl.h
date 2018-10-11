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

/** @defgroup DriverIcm30xxxCtrl Icm30xxx driver control
 *  @brief    Low-level driver functions to send command to and get data from ICM30xxx
 *  @ingroup  DriverIcm30xxx
 *  @{
 */

#ifndef _INV_ICM30XXX_CTRL_H_
#define _INV_ICM30XXX_CTRL_H_

#include "Invn/InvExport.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* forward declaration */
struct inv_icm30xxx;

int INV_EXPORT inv_icm30xxx_ctrl_retrieve_fifo_config(struct inv_icm30xxx * s);

int INV_EXPORT inv_icm30xxx_ctrl_send_command(struct inv_icm30xxx * s,	uint8_t sensorid,
		uint8_t command, const void * arg, uint16_t size);

int INV_EXPORT inv_icm30xxx_read_int_status(struct inv_icm30xxx * s,
		uint8_t poll_int_mask, uint8_t * fifo_mask);

int INV_EXPORT inv_icm30xxx_ctrl_poll_sensor(struct inv_icm30xxx * s, int fifo_mask);


/* Read data from the I2C slave interface and parse it.
 */
int INV_EXPORT inv_icm30xxx_read_and_parse_i2c(struct inv_icm30xxx * s);

/* Berlin only */

/* To read audio data from FIFO2
 */
int INV_EXPORT inv_icm30xxx_ctrl_poll_audio(struct inv_icm30xxx * s, int fifo_mask);

/* To indicate to the driver to stop polling audio FIFO
 * next time inv_icm30xxx_ctrl_poll() is called.
 * (Has no effect on the device)
 */
void INV_EXPORT inv_icm30xxx_ctrl_stop_poll_audio(struct inv_icm30xxx * s);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM30XXX_CTRL_H_ */

/** @} */
