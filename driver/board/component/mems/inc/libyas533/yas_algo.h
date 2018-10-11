/**
 * Header file of the MS-x algorithm API @file yas_algo.h
 *
 * Copyright (c) 2013-2014 Yamaha Corporation
 * CONFIDENTIAL
 */
#ifndef __YAS_ALGO_H__
#define __YAS_ALGO_H__

#include "yas.h"

#define YAS_MAG_CALIB_UPDATE (0x00000002) /*!< Magnetic calibration update */
#define YAS_GYRO_CALIB_UPDATE (0x00000004) /*!< Gyroscope calibration update */
#if YAS_SIGNIFICANT_MOTION_ENABLE
#define YAS_SIGNIFICANT_MOTION_UPDATE (0x00000008) /*!< Significant motion
						     update */
#endif
#if YAS_STEPCOUNTER_ENABLE
#define YAS_STEPCOUNTER_UPDATE (0x00000010) /*!< Step counter update */
#endif

/* ----------------------------------------------------------------------------
 *                          Struct Definition
 *--------------------------------------------------------------------------- */

/**
 * @enum sensor index
 * @brief array index of the sensors.
 */
enum {
#if YAS_ACC_DRIVER != YAS_ACC_DRIVER_NONE
	yas_accelerometer, /*!< acceleration sensor index */
#endif
	yas_magnetic, /*!< magnetic sensor index */
#if YAS_GYRO_DRIVER != YAS_GYRO_DRIVER_NONE
	yas_gyroscope, /*!< gyroscope sensor index */
#endif
	yas_num_sensors /*!< the number of the sensors */
};

/**
 * @struct yas_algo_state
 * @brief MS-x algorithm status to be saved/loaded at power on/off
 */
struct yas_algo_state {
	struct yas_vector offset[yas_num_sensors]; /*!< Calibration offset */
	uint8_t accuracy[yas_num_sensors]; /*!< Calibration offset accuracy */
#if YAS_MAG_CALIB_ELLIPSOID_ENABLE
	struct yas_matrix mag_dynamic_matrix; /*!< Magnetometer dynamic
						ellipsoid correction matrix */
#endif
};

/* ----------------------------------------------------------------------------
 *                          Global Functions
 *--------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the MS-x algorithm module
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_init(void);

/**
 * Terminates the MS-x algorithm module
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_term(void);

/**
 * Updates algorithm and obtains the algorithm data
 * @param[in|out] raw Measured sensor data as input, uncalibrated sensor data
 * as output.
 * @param[out] cal_and_filtered Calibrated and filterd sensor data
 * @param[in] num The number of measured sensor data
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_update(struct yas_data *raw, struct yas_data *cal_and_filtered,
		int num);

/**
 * Obtains the algorithm status to be saved/loaded at power on/off
 * @param[out] state Algorithm status
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_state(struct yas_algo_state *state);

/**
 * Sets the algorithm status to be saved/loaded at power on/off
 * @param[in] state Algorithm status
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_state(struct yas_algo_state *state);

/**
 * Reports the calibration status (Enabled or Disabled)
 * @param[in] type Sensor type
 * @retval 0 Disabled
 * @retval 1 Enabled
 * @retval Negative Failure
 */
int yas_algo_get_calib_enable(int32_t type);

/**
 * Enables or disables calibration
 * @param[in] type Sensor type
 * @param[in] enable The calibration of the sensor (0: Disable, 1: Enable)
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_calib_enable(int32_t type, int enable);

/**
 * Obtains the calibration configuration
 * @param[in] type Sensor type
 * @param[out] config Calibration configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_calib_config(int32_t type, void *config);

/**
 * Sets the calibration configuration
 * @param[in] type Sensor type
 * @param[in] config Calibration configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_calib_config(int32_t type, void *config);

/**
 * Obtains the detail of the last calibration result
 * @param[in] type Sensor type
 * @param[out] result Calibration result
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_calib_result(int32_t type, void *result);

/**
 * Resets the calibration
 * @param[in] type Sensor type
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_reset_calib(int32_t type);

/**
 * Reports the filter status (Enabled or Disabled)
 * @param[in] type Sensor type
 * @retval 0 Disabled
 * @retval 1 Enabled
 * @retval Negative Failure
 */
int yas_algo_get_filter_enable(int32_t type);

/**
 * Enables or disables filter
 * @param[in] type Sensor type
 * @param[in] enable The filter of the sensor (0: Disable, 1: Enable)
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_filter_enable(int32_t type, int enable);

/**
 * Obtains the filter configuration
 * @param[in] type Sensor type
 * @param[out] config Filter configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_filter_config(int32_t type, void *config);

/**
 * Sets the filter configuration
 * @param[in] type Sensor type
 * @param[in] config Filter configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_filter_config(int32_t type, void *config);

/**
 * Resets the filter
 * @param[in] type Sensor type
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_reset_filter(int32_t type);

#if YAS_FUSION_ENABLE
/**
 * Reports the sensor fusion status (Enabled or Disabled)
 * @retval 0 Disabled
 * @retval 1 Enabled
 * @retval Negative Failure
 */
int yas_algo_get_fusion_enable(void);

/**
 * Enables or disables sensor fusion
 * @param[in] enable The sensor fusion (0: Disable, 1: Enable)
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_fusion_enable(int enable);

/**
 * Obtains the sensor fusion configuration
 * @param[out] config Sensor fusion configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_fusion_config(struct yas_fusion_config *config);

/**
 * Sets the sensor fusion configuration
 * @param[in] config Sensor fusion configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_fusion_config(struct yas_fusion_config *config);

/**
 * Obtains the detail of the last sensor fusion result
 * @param[in] type Sensor type
 * @param[out] result Sensor fusion result
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_fusion_result(struct yas_fusion_result *result);

/**
 * Resets the sensor fusion
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_reset_fusion(void);
#endif

#if YAS_SOFTWARE_GYROSCOPE_ENABLE
/**
 * Reports the software gyroscope status (Enabled or Disabled)
 * @retval 0 Disabled
 * @retval 1 Enabled
 * @retval Negative Failure
 */
int yas_algo_get_swgyro_enable(void);

/**
 * Enables or disables software gyroscope
 * @param[in] enable The software gyroscope (0: Disable, 1: Enable)
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_swgyro_enable(int enable);

/**
 * Obtains the software gyroscope configuration
 * @param[out] config software gyroscope configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_swgyro_config(struct yas_swgyro_config *config);

/**
 * Sets the software gyroscope configuration
 * @param[in] config software gyroscope configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_swgyro_config(struct yas_swgyro_config *config);

/**
 * Obtains the detail of the last software gyroscope result
 * @param[in] type Sensor type
 * @param[out] result software gyroscope result
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_swgyro_result(struct yas_swgyro_result *result);

/**
 * Resets the software gyroscope
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_reset_swgyro(void);
#endif

#if YAS_STEPCOUNTER_ENABLE
/**
 * Reports the step counter status (Enabled or Disabled)
 * @retval 0 Disabled
 * @retval 1 Enabled
 * @retval Negative Failure
 */
int yas_algo_get_stepcounter_enable(void);

/**
 * Enables or disables step counter
 * @param[in] enable The step counter (0: Disable, 1: Enable)
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_stepcounter_enable(int enable);

/**
 * Obtains the step counter configuration
 * @param[out] config step counter configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_stepcounter_config(struct yas_stepcounter_config *config);

/**
 * Sets the step counter configuration
 * @param[in] config step counter configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_stepcounter_config(struct yas_stepcounter_config *config);

/**
 * Obtains the detail of the last step counter result
 * @param[in] type Sensor type
 * @param[out] result step counter result
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_stepcounter_result(struct yas_stepcounter_result *result);

/**
 * Resets the step counter
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_reset_stepcounter(void);
#endif

#if YAS_SIGNIFICANT_MOTION_ENABLE
/**
 * Reports the step counter status (Enabled or Disabled)
 * @retval 0 Disabled
 * @retval 1 Enabled
 * @retval Negative Failure
 */
int yas_algo_get_sfm_enable(void);

/**
 * Enables or disables step counter
 * @param[in] enable The step counter (0: Disable, 1: Enable)
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_sfm_enable(int enable);

/**
 * Obtains the step counter configuration
 * @param[out] config step counter configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_sfm_config(struct yas_sfm_config *config);

/**
 * Sets the step counter configuration
 * @param[in] config step counter configuration
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_sfm_config(struct yas_sfm_config *config);

/**
 * Obtains the detail of the last step counter result
 * @param[in] type Sensor type
 * @param[out] result step counter result
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_get_sfm_result(struct yas_sfm_result *result);

/**
 * Resets the step counter
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_reset_sfm(void);
#endif

#if YAS_LOG_ENABLE
/**
 * Initializes the MS-x driver log function
 * @param[in] log User-written callback Functions
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_log_init(struct yas_log *log);

/**
 * Reports the log status (Enabled or Disabled)
 * @retval 0 Disabled
 * @retval 1 Enabled
 * @retval Negative Failure
 */
int yas_algo_get_log_enable(void);

/**
 * Enables or disables log
 * @param[in] enable The status of log (0:Disable, 1:Enable)
 * @retval #YAS_NO_ERROR Success
 * @retval Negative Failure
 */
int yas_algo_set_log_enable(int enable);
#endif

#ifdef __cplusplus
}
#endif

#endif
