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

/** @defgroup DriverIcm30xxxLogger Icm30xxx in-memory logger
 *  @brief Low-level function to access to Icm30xxx in-memory logger
 *  @ingroup  DriverIcm30xxx
 *  @{
 */

#ifndef _INV_ICM30XXX_LOGGER_H_
#define _INV_ICM30XXX_LOGGER_H_

#include "Invn/InvExport.h"

#ifdef __cplusplus
extern "C" {
#endif

/* forward declaration */
struct inv_icm30xxx;

// allowed level for logger
enum {
	INV_ICM30XXX_LOGGER_LVL_OFF       = 0,
	INV_ICM30XXX_LOGGER_LVL_ERROR     = 1,
	INV_ICM30XXX_LOGGER_LVL_WARNING   = 2,
	INV_ICM30XXX_LOGGER_LVL_NOTICE    = 3,
	INV_ICM30XXX_LOGGER_LVL_INFO      = 4,
	INV_ICM30XXX_LOGGER_LVL_VERBOSE   = 5,
	INV_ICM30XXX_LOGGER_LVL_DEBUG     = 6,
	INV_ICM30XXX_LOGGER_LVL_TIMESTAMP = 7,
	INV_ICM30XXX_LOGGER_LVL_ALL,
};

// setup RAM logger
// should be called before starting M0 FW
int INV_EXPORT inv_imc30xxx_logger_setup(struct inv_icm30xxx * s, unsigned level);

// user putchar callback definition
typedef void (*inv_imc30xxx_logger_putc_cb)(void * context, int c);

// retrieve logger data and print them using user provided function
int INV_EXPORT inv_imc30xxx_print_logger_data(struct inv_icm30xxx * s,
		inv_imc30xxx_logger_putc_cb putcb, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM30XXX_LOGGER_H_ */

/** @} */
