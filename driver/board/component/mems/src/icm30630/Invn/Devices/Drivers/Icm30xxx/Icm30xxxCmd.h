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

/** @defgroup DriverIcm30xxxCmd Icm30xxx driver command
 *  @brief    Low-level driver functions to send specific commands to ICM30xxx device
 *  @ingroup  DriverIcm30xxx
 *  @{
 */

#ifndef _INV_ICM30XXX_CMD_H_
#define _INV_ICM30XXX_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "Invn/InvExport.h"
#include "Invn/InvBool.h"

/* forward declaration */
struct inv_icm30xxx;
struct inv_icm30xxx_fw_version;

int INV_EXPORT inv_icm30xxx_cmd_load(struct inv_icm30xxx * s, uint8_t who, uint8_t what, uint32_t arg);

int INV_EXPORT inv_icm30xxx_cmd_get_fw_info(struct inv_icm30xxx * s, struct inv_icm30xxx_fw_version * fw);

int INV_EXPORT inv_icm30xxx_cmd_enable_sensor(struct inv_icm30xxx * s, uint8_t sensorid, inv_bool_t state);

int INV_EXPORT inv_icm30xxx_cmd_set_i2c_sklave_mode(struct inv_icm30xxx * s, inv_bool_t state);

int INV_EXPORT inv_icm30xxx_cmd_set_running_state(struct inv_icm30xxx * s, inv_bool_t state);

int INV_EXPORT inv_icm30xxx_cmd_set_sensor_period(struct inv_icm30xxx * s, uint8_t sensorid, uint32_t us_period);

int INV_EXPORT inv_icm30xxx_cmd_set_sensor_timeout(struct inv_icm30xxx * s, uint8_t sensorid, uint32_t ms_timeout);

int INV_EXPORT inv_icm30xxx_cmd_flush_sensor(struct inv_icm30xxx * s, uint8_t sensorid);

int INV_EXPORT inv_icm30xxx_cmd_set_sensor_mounting_matrix(struct inv_icm30xxx * s, uint8_t sensorid, const float matrix[9]);

int INV_EXPORT inv_icm30xxx_cmd_set_sensor_bias(struct inv_icm30xxx * s, uint8_t sensorid, const float bias[3]);

int INV_EXPORT inv_icm30xxx_cmd_get_sensor_bias(struct inv_icm30xxx * s, uint8_t sensorid, float bias[3]);

int INV_EXPORT inv_icm30xxx_cmd_set_sensor_gain(struct inv_icm30xxx * s, uint8_t sensorid, const float gain[3]);

int INV_EXPORT inv_icm30xxx_cmd_get_sensor_gain(struct inv_icm30xxx * s, uint8_t sensorid, float gain[3]);

int INV_EXPORT inv_icm30xxx_cmd_get_clock_rate(struct inv_icm30xxx * s, uint32_t * clock_rate);

int INV_EXPORT inv_icm30xxx_cmd_get_last_sensor_data(struct inv_icm30xxx * s, uint8_t sensorid,	uint32_t * timestamp, uint8_t * data, uint16_t * len);

int INV_EXPORT inv_icm30xxx_cmd_ping_sensor(struct inv_icm30xxx * s, uint8_t sensorid, uint8_t * ping_value);

int INV_EXPORT inv_icm30xxx_cmd_reset_sensor(struct inv_icm30xxx * s, uint8_t sensorid);

/** @brief Allowed config settings
 */
enum inv_icm30670_config_setting {
	INV_ICM30670_CONFIG_FSR               = 1, /**< configure full-scale range for sensor */
	INV_ICM30670_CONFIG_BW                = 2, /**< configure bandwith for sensor */
	INV_ICM30670_CONFIG_CUSTOM            = 3, /**< configure custom sensor */
	INV_ICM30670_CONFIG_TSIMU_TEMPERATURE = 4, /**< configure PID stable temperature for TSIMU */
	INV_ICM30670_CONFIG_TSIMU_PROP        = 5, /**< configure PID proportional paramter for TSIMU */
	INV_ICM30670_CONFIG_TSIMU_INTG        = 6, /**< configure PID integral parameter for TSIMU */
	INV_ICM30670_CONFIG_TSIMU_DERI        = 7, /**< configure PID derivative parameter for TSIMU */
	INV_ICM30670_CONFIG_TSIMU_RPID        = 8, /**< configure enable/disable PID status report for TSIMU */
};

/** @brief Allowed FSR setting value for accelerometer
 */
enum inv_icm30670_fsr_acc {
	INV_ICM30670_FSR_ACC_2G   = 2,  /**< accelerometer FSR +/- 2 g */
	INV_ICM30670_FSR_ACC_4G   = 4,  /**< accelerometer FSR +/- 4 g */
	INV_ICM30670_FSR_ACC_8G   = 8,  /**< accelerometer FSR +/- 8 g */
	INV_ICM30670_FSR_ACC_16G  = 16, /**< accelerometer FSR +/- 16 g */
};

/** @brief Allowed FSR setting value for gyroscope
 */
enum inv_icm30670_fsr_gyr {
	INV_ICM30670_FSR_GYR_250  = 250,  /**< gyroscope FSR +/- 250 dps */
	INV_ICM30670_FSR_GYR_500  = 500,  /**< gyroscope FSR +/- 500 dps */
	INV_ICM30670_FSR_GYR_1000 = 1000, /**< gyroscope FSR +/- 1000 dps */
	INV_ICM30670_FSR_GYR_2000 = 2000, /**< gyroscope FSR +/- 2000 dps */
};

/** @brief Allowed BW setting value for accelerometer
 */
enum inv_icm30670_bw_acc {
	INV_ICM30670_BW_ACC_420   = 420, /**< BW f0 = 420 Hz */
	INV_ICM30670_BW_ACC_218   = 218, /**< BW f0 = 218 Hz */
	INV_ICM30670_BW_ACC_99    = 99,  /**< BW f0 = 99 Hz */
	INV_ICM30670_BW_ACC_45    = 45,  /**< BW f0 = 45 Hz */
	INV_ICM30670_BW_ACC_21    = 21,  /**< BW f0 = 21 Hz */
	INV_ICM30670_BW_ACC_10    = 10,  /**< BW f0 = 10 Hz */
	INV_ICM30670_BW_ACC_5     = 5,   /**< BW f0 = 5 Hz */
};

/** @brief Allowed BW setting value for gyroscope
 */
enum inv_icm30670_bw_gyr {
	INV_ICM30670_BW_GYR_176   = 176,  /**< BW f0 = 176 Hz */
	INV_ICM30670_BW_GYR_92    = 92,   /**< BW f0 = 92 Hz */
	INV_ICM30670_BW_GYR_41    = 41,   /**< BW f0 = 41 Hz */
	INV_ICM30670_BW_GYR_20    = 20,   /**< BW f0 = 20 Hz */
	INV_ICM30670_BW_GYR_10    = 10,   /**< BW f0 = 10 Hz */
	INV_ICM30670_BW_GYR_5     = 5,    /**< BW f0 = 5 Hz */
};

/** @brief Send command to configure sensor
 *  @param sensorid expected to FIFOPROTOCOL_SENSOR_ID_ACCELEROMETER or FIFOPROTOCOL_SENSOR_ID_GYROSCOPE
 *  @param setting expected to be a value of @sa enum inv_icm30670_config_setting
 *  @param value   expected to be a pointer to a uint16_t containing a value of:
 *                 - @sa enum inv_icm30670_fsr_acc
 *                 - @sa enum inv_icm30670_fsr_gyr
 *                 - @sa enum inv_icm30670_bw_acc
 *                 - @sa enum inv_icm30670_bw_gyr
 */
int INV_EXPORT inv_icm30xxx_cmd_set_sensor_config(struct inv_icm30xxx * s, uint8_t sensorid,
		int setting, const void * data, uint16_t size);

/** @brief Send command to configure sensor
 *  @param sensorid expected to FIFOPROTOCOL_SENSOR_ID_ACCELEROMETER or FIFOPROTOCOL_SENSOR_ID_GYROSCOPE
 *  @param setting  expected to be a value of @sa enum inv_icm30670_config_setting
 *  @param value    expected to be a pointer to a uint16_t placeholder that will
 *                  contain the returned value as of:
 *                 - @sa enum inv_icm30670_fsr_acc
 *                 - @sa enum inv_icm30670_fsr_gyr
 *                 - @sa enum inv_icm30670_bw_acc
 *                 - @sa enum inv_icm30670_bw_gyr
 *  @return        size of returned value in byte
 */
int INV_EXPORT inv_icm30xxx_cmd_get_sensor_config(struct inv_icm30xxx * s, uint8_t sensorid,
		int setting, void * data, uint16_t size);

int INV_EXPORT inv_icm30xxx_cmd_write_mems_register(struct inv_icm30xxx * s, uint8_t sensorid,
		uint16_t reg_addr, const void * data, uint16_t size);

int INV_EXPORT inv_icm30xxx_cmd_read_mems_register(struct inv_icm30xxx * s, uint8_t sensorid,
		uint16_t reg_addr, void * data, uint16_t size);

/**************** Berlin only *************************************************/

/* Some flags use to track current audio states that impact the driver behavior */

// value may looks strange but they allow to have .audio_flag = 0 corresponding to default config

#define INV_ICM30XXX_AUDIO_CFG_FLAG_KWR_OFF       0x01
#define INV_ICM30XXX_AUDIO_CFG_FLAG_AUDIO_OFF     0x02
#define INV_ICM30XXX_AUDIO_CFG_FLAG_TIMESTAMP_OFF 0x08

/* Specific command for configure audio sensor */

enum inv_icm30xxx_audio_cmd
{
	INV_ICM30XXX_AUDIO_CMD_KWR_EN                   = 1, /* enable KWR processing at ZSP level */
	INV_ICM30XXX_AUDIO_CMD_AUDIO_EN                 = 2, /* enable audio processing at ZSP level */
	INV_ICM30XXX_AUDIO_CMD_STREAMING_MODE           = 3, /* configure streaming of audio data */
	INV_ICM30XXX_AUDIO_CMD_TIMESTAMP_EN             = 5, /* add timestamp to audio samples */
	INV_ICM30XXX_AUDIO_CMD_UPDATE_MODEL             = 6, /* update KWR model */
};

// generic function to send audio command
// should be used with care as .audio_cfg_flag is not updated from this
int INV_EXPORT inv_icm30xxx_cmd_configure_audio(struct  inv_icm30xxx * s,
		uint8_t cmd, uint8_t arg);

/* Send command to configure audio */

int INV_EXPORT inv_icm30xxx_cmd_enable_kwr(struct  inv_icm30xxx * s, inv_bool_t en);

int INV_EXPORT inv_icm30xxx_cmd_enable_audio_detect(struct  inv_icm30xxx * s, inv_bool_t en);

enum inv_icm30xxx_audio_cmd_stream_mode {
	INV_ICM30XXX_AUDIO_CMD_STREAM_FIFO      = 0x01,
	INV_ICM30XXX_AUDIO_CMD_STREAM_M0        = 0x02,
	INV_ICM30XXX_AUDIO_CMD_STREAM_STORE     = 0x04,
	INV_ICM30XXX_AUDIO_CMD_STREAM_OPEN_MIC  = 0x08,
	INV_ICM30XXX_AUDIO_CMD_STREAM_STOP      = 0x10,
	INV_ICM30XXX_AUDIO_CMD_STREAM_DUMP      = 0x20,
};

int INV_EXPORT inv_icm30xxx_cmd_enable_audio_stream(struct  inv_icm30xxx * s, int mode);

int INV_EXPORT inv_icm30xxx_cmd_enable_audio_timestamp(struct  inv_icm30xxx * s, inv_bool_t en);

int INV_EXPORT inv_icm30xxx_cmd_configure_audio_update_kwr_sensory_model(struct  inv_icm30xxx * s,
		const uint8_t * grammar_model, uint32_t size_grammar_model,
		const uint8_t * net_model, uint32_t net_size_model);

int INV_EXPORT inv_icm30xxx_cmd_configure_audio_update_kwr_google_model(struct  inv_icm30xxx * s,
		const uint8_t * model, uint32_t size_model);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM30XXX_CMD_H_ */

/** @} */
