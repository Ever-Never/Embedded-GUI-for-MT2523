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


/** @defgroup DriverIcm30xxxLoad Icm30xxx driver fw loading 
 *  @brief    Low-level functions related to FW and image loading for ICM30xxx devices
 *  @ingroup  DriverIcm30xxx
 *  @{
 */

#ifndef _INV_ICM30XXX_LOAD_H_
#define _INV_ICM30XXX_LOAD_H_

#include "Invn/InvExport.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "Invn/InvBool.h"

/* forward declaration */
struct inv_icm30xxx;

int INV_EXPORT inv_icm30xxx_load_flash(struct inv_icm30xxx * s, const uint8_t * image,
		uint32_t size, inv_bool_t verify, inv_bool_t force);

int INV_EXPORT inv_icm30xxx_load_ram(struct inv_icm30xxx * s, const uint8_t * image,
		uint32_t size, inv_bool_t verify);

int INV_EXPORT inv_icm30xxx_load_other(struct inv_icm30xxx * s, int who, const uint8_t * image,
		uint32_t size);

// write to ZSP memory
// caller must take care of reseting ZSP and waking-up ICM
int INV_EXPORT inv_icm30xxx_zsp_mem_write(struct inv_icm30xxx * s, const uint8_t * buffer,
		uint32_t size, uint32_t dest_addr);

int INV_EXPORT inv_icm30xxx_load_zsp_imem(struct inv_icm30xxx * s, const uint8_t * iimage,
		uint32_t isize);

int INV_EXPORT inv_icm30xxx_load_zsp_dmem(struct inv_icm30xxx * s, const uint8_t * dimage,
		uint32_t dsize);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM30XXX_LOAD_H_ */

/** @} */
