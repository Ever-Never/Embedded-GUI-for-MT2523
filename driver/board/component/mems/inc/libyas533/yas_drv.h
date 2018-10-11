/**
 * Header file of the MS-x driver API @file yas_drv.h
 *
 * Copyright (c) 2013 Yamaha Corporation
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */
#ifndef __YAS_DRV_H__
#define __YAS_DRV_H__

#include "yas.h"

/* ----------------------------------------------------------------------------
 *                            Structure definition
 *--------------------------------------------------------------------------- */

#if YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS532 \
	|| YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS533 \
	|| YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS535
/**
 * @struct yas_driver_state
 * @brief Driver status to be saved/loaded at power on/off
 */
struct yas_driver_state {
	int8_t mag_hard_offset[3]; /*!< magnetic hardware offset */
};
#endif

/* ----------------------------------------------------------------------------
 *                          Global Functions
 *--------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the MS-x driver
 * @param[in] machdep User-written callback Functions
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_driver_init(struct yas_driver_callback *cbk);

/**
 * Terminates the MS-x driver
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_driver_term(void);

/**
 * Reports the sensor status (Enabled or Disabled)
 * @param[in] type Sensor type
 * @retval 0 Disabled
 * @retval 1 Enabled
 * @retval Negative Failure
 */
int yas_driver_get_enable(int32_t type);

/**
 * Enables or disables sensors
 * @param[in] type Sensor type
 * @param[in] enable The status of the sensor (0:Disable, 1:Enable)
 * @retval Non-Negative The bit or of the sensor type successfully
 * enabled/disabled.
 * @retval Negative Failure
 */
int32_t yas_driver_set_enable(int32_t type, int enable);

/**
 * Obtains the sensor position
 * @param[in] type Sensor type
 * @retval 0-7 The position of the sensor
 * @retval Negative Failure
 */
int yas_driver_get_position(int32_t type);

/**
 * Sets the sensor position
 * @param[in] type Sensor type
 * @param[in] position The position of the sensor (0-7)
 * @retval Non-Negative The bit or of the sensor type successfully
 * position changed.
 * @retval Negative Failure
 */
int32_t yas_driver_set_position(int32_t type, int position);

/**
 * Obtains measurment period in milli-seconds
 * @param[in] type Sensor type
 * @retval Non-Negative Measurement period in milli-seconds
 * @retval Negative Failure
 */
int yas_driver_get_delay(int32_t type);

/**
 * Sets measurment period in milli-seconds
 * @param[in] type Sensor type
 * @param[in] delay Measurement period in milli-seconds
 * @retval Non-Negative The bit or of the sensor type successfully
 * delay changed.
 * @retval Negative Failure
 */
int32_t yas_driver_set_delay(int32_t type, int delay);

#if YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS532 \
	|| YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS533 \
	|| YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS535
/**
 * Obtains the sensor status to be saved/loaded at power on/off
 * @param[out] state Driver status
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_driver_get_state(struct yas_driver_state *state);

/**
 * Sets the sensor status to be saved/loaded at power on/off
 * @param[in] state Driver status
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_driver_set_state(struct yas_driver_state *state);
#endif

#if YAS_LOG_ENABLE
/**
 * Initializes the MS-x driver log function
 * @param[in] log User-written callback Functions
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_driver_log_init(struct yas_log *log);

/**
 * Reports the log status (Enabled or Disabled)
 * @retval 0 Disabled
 * @retval 1 Enabled
 * @retval Negative Failure
 */
int yas_driver_get_log_enable(void);

/**
 * Enables or disables log
 * @param[in] enable The status of log (0:Disable, 1:Enable)
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_driver_set_log_enable(int enable);
#endif

/**
 * Extension command execution specific to the part number
 * @param[in] type Sensor type
 * @param[in] cmd Extension command id
 * @param[out] result Extension command result
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_driver_ext(int32_t type, int32_t cmd, void *p);

/**
 * Measures the sensor
 * @param[in] type Sensor type
 * @param[out] data Measured sensor data
 * @param[in] num The number of the measured sensor data
 * @retval Non-Negative The number of the measured sensor data
 * @retval Negative Failure
 */
int yas_driver_measure(int32_t type, struct yas_data *data, int num);

#ifdef __cplusplus
}
#endif

#endif
