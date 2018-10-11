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

/** @defgroup DeviceIcm30xxx DeviceIcm30xxx
 *	@brief    Concrete implementation of the 'Device' interface for Icm30xxx devices
 *
 *            See @ref ExampleDeviceIcm30630EMD.c example.
 *
 *  @ingroup  Device
 *	@{
 */

#ifndef _INV_DEVICE_ICM30XXX_H_
#define _INV_DEVICE_ICM30XXX_H_

#include "Invn/InvExport.h"

#include "Invn/Devices/Device.h"
#include "Invn/Devices/SerifHal.h"

#include "Invn/Devices/Drivers/Icm30xxx/Icm30xxx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Enum describing possible type of image that can be loaded
 */
enum inv_device_icm30xxx_image_type {
	INV_DEVICE_ICM30XXX_IMAGE_TYPE_M0_FLASH,
	INV_DEVICE_ICM30XXX_IMAGE_TYPE_M0_SRAM,
	INV_DEVICE_ICM30XXX_IMAGE_TYPE_DMP3,
	INV_DEVICE_ICM30XXX_IMAGE_TYPE_DMP4,
	INV_DEVICE_ICM30XXX_IMAGE_TYPE_ZSP_IMEM,
	INV_DEVICE_ICM30XXX_IMAGE_TYPE_ZSP_DMEM,
};

/** @brief States for Icm30xxx device
 */
typedef struct inv_device_icm30xxx
{
	inv_device_t         base;
	struct inv_icm30xxx  icm30xxx_states;
	inv_bool_t           debug_en;
	inv_bool_t           reset_done;
	uint32_t             clock_rate;
	void                 (*irq_callback)(void * context, int int_num);
	void *               irq_callback_context;
	void                 (*dbg_hook_cb)(void * context);
	void *               dbg_hook_context;
	uint8_t              map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_MAX];
	struct
	{
		uint8_t level;
		void (* putc_cb)(void * context, int c);
		void *  context;
	} logger;
	inv_bool_t           ts_imu_started;
	inv_bool_t           hrgyr_started;
	inv_bool_t (*build_data)(uint8_t, uint8_t, const uint8_t *, uint16_t, inv_sensor_event_t *);
	inv_host_serif_t          legacy_serif; /* used for backward API compatibility */
	inv_host_serif_t          legacy_serif_slave; /* used for backward API compatibility */
	struct inv_fw_version     fw_version;
} inv_device_icm30xxx_t;


/** @brief Return handle to underlying driver states
 *
 *  @param[in] self         handle to device
 *  @return pointer to underlying driver states
 */
static inline struct inv_icm30xxx * inv_device_icm30xxx_get_driver_handle(inv_device_icm30xxx_t * self)
{
	return &self->icm30xxx_states;
}

/** @brief constructor-like function for Icm3063x device
 *
 *  Will initialize inv_device_icm30xxx_t object states to default value
 *  for ICM3063x chip variant from ICM30xxx family.
 *
 *  You must provide a static buffer to the driver. Buffer must be >= 4kB.
 *  Bigger buffer increases performance a bit by reducing number of request to
 *  the device for some actions.
 *
 *  @warning An option is provided to enable target debugging with JTAG/SWD.
 *  Attempting to debug the target whitout enabling this option can dammage it.
 *
 *  @param[in] self         handle to device
 *  @param[in] serif        reference to Serial Interface HAL object
 *  @param[in] listener     reference to Sensor Event Listener object
 *  @param[in] buffer       reference to static buffer
 *  @param[in] buffer_size  static buffer size (must be >= 4kB)
 *  @param[in] jtag_en      enable JTAG/SWD debugging capability
 */
void INV_EXPORT inv_device_icm3063x_init2(inv_device_icm30xxx_t * self,
		const inv_serif_hal_t * serif, const inv_sensor_listener_t * listener,
		uint8_t * buffer, uint32_t buffer_size, inv_bool_t jtag_en);

/** @brief constructor-like function for Icm3063x device
 *
 *  This function is identical to inv_device_icm3063x_init2() except it takes a
 *  reference to a legacy Host Serial Interface object
 *
 *  See inv_device_icm3063x_init2() for arguments description.
 *
 *  @deprecated Use inv_device_icm3063x_init2() instead.
 */
void INV_EXPORT inv_device_icm3063x_init(inv_device_icm30xxx_t * self,
		const inv_host_serif_t * serif, const inv_sensor_listener_t * listener,
		uint8_t * buffer, uint32_t buffer_size, inv_bool_t jtag_en);

/*
 * For backward compatibility
 */
#define inv_device_icm30630_init  inv_device_icm3063x_init
#define inv_device_icm30631_init  inv_device_icm3063x_init
#define inv_device_icm30632_init  inv_device_icm3063x_init
#define inv_device_icm30630_init2 inv_device_icm3063x_init2
#define inv_device_icm30631_init2 inv_device_icm3063x_init2
#define inv_device_icm30632_init2 inv_device_icm3063x_init2

/** @brief constructor-like function for Icm30670 device
 *
 *  See inv_device_icm3063x_init() for arguments description.
 */
void INV_EXPORT inv_device_icm30670_init2(inv_device_icm30xxx_t * self,
		const inv_serif_hal_t * serif, const inv_sensor_listener_t * listener,
		uint8_t * buffer, uint32_t buffer_size, inv_bool_t jtag_en);

/** @brief constructor-like function for Icm30670 device
 *
 *  This function is identical to inv_device_icm30670_init2() except it takes a
 *  reference to a legacy Host Serial Interface object
 *
 *  See inv_device_icm3063x_init2() for arguments description.
 *
 *  @deprecated Use inv_device_icm30670_init2() instead.
 */
void INV_EXPORT inv_device_icm30670_init(inv_device_icm30xxx_t * self,
		const inv_host_serif_t * serif, const inv_sensor_listener_t * listener,
		uint8_t * buffer, uint32_t buffer_size, inv_bool_t jtag_en);

/** @brief constructor-like function for Berlin device
 *
 *  See inv_device_icm3063x_init() for arguments description.
 */
void INV_EXPORT inv_device_berlin_init(inv_device_icm30xxx_t * self,
		const inv_serif_hal_t * serif, const inv_sensor_listener_t * listener,
		uint8_t * buffer, uint32_t buffer_size, inv_bool_t jtag_en);

/** @brief initialize i2c slave serif
 *
 */
void INV_EXPORT inv_device_icm30670_init_serif_i2cslave(inv_device_icm30xxx_t * self, const inv_host_serif_t * serif_i2cslave);

/** @brief initialize i2c slave serif - backwards compatible
 *
 */
void INV_EXPORT inv_device_icm30670_init_serif_i2cslave2(inv_device_icm30xxx_t * self, const inv_serif_hal_t * serif_i2cslave);

/** @brief Helper function to get handle to base object
 */
static inline inv_device_t * inv_device_icm30xxx_get_base(inv_device_icm30xxx_t * self)
{
	if(self)
		return &self->base;

	return 0;
}

/** @brief Register secondary SERIF object for I2C Slave access
 *  Must be called after inv_icm30670_reset_states()
 *  @param[in] s handle to driver states structure
 *  @param[in] serif handle to SERIF object for underlying register access to OIS
 */
static inline void inv_icm30670_reset_states_serif_i2cslave(struct inv_icm30xxx * s,
        const struct inv_icm30xxx_serif * serif_i2cslave)
{
	if(serif_i2cslave) {
	    s->serif_i2cslave = *serif_i2cslave;
	    s->use_serif_i2cslave = 1;
	}
}
/** @brief  Helper function to register an interrupt interrupt callback
 *
 *  This method is deprecated and does nothing. It is only there to not break legacy code.
 */
int INV_EXPORT inv_device_icm30xxx_register_interrupt_callback(inv_device_icm30xxx_t * self,
		void (*interrupt_cb)(void * context, int int_num), void * context);

/** @brief  Function to enable device logger
 *
 *  This method should be call after inv_device_icm3063x_init()
 *
 *  Will enable device logger upon setup and check for messages when calling poll()
 *
 *  @warning registered callback will be called in the same context than
 *  Host Serif callbak.
 *  @param[in] self          handle to device icm30xxx object
 *  @param[in] level         logger level from 0 (no messages) to 7 (all messages)
 *  @param[in] putc_cb       user defined printf function
 *  @param[in] context       some pointer passed to the callback
 */
void INV_EXPORT inv_device_icm30xxx_enable_logger(inv_device_icm30xxx_t * self,
	   uint8_t level, void (*putc_cb)(void * context, int c), void * context);


/** @brief  Register a debugger hook
 *
 *  This method should be call after inv_device_icm3063x_init()
 *
 *  Will call registered callback after device reset and before performing
 *  device initialization to help debugging early stage FW execution.
 *
 *  Hook can block calling process, letting time to attach a debugger.
 *
 *  @param[in] self          handle to device icm30xxx object
 *  @param[in] interrupt_cb  callback called at early stage of device setup.
 *  @param[in] context       some pointer passed to the callback
 */
void INV_EXPORT inv_device_icm30xxx_register_debugger_hook_callback(inv_device_icm30xxx_t * self,
	   void (*dbg_hook_cb)(void * context), void * context);

/** @brief Checks if the device firmware is running properly
 *
 *  This method sends a request for a heartbeat to the icm30670 device 
 *  and if it doesn't receive an answer within a certain amount of time
 *  it resets the device.
 * 
 *  @warning It falls in the host's responsibility to call this method 
 *  as often as it seems fit for the desired application.
 *
 *  @param[in] self     handle to device icm30xxx object
 *  @return             0 on success
 *                      INV_ERROR_TRANSPORT in case of low level serial error
 *                      INV_ERROR_TIMEOUT if device does not respond in time
 *                      INV_ERROR if sensor is not supported by the device
 *                      INV_ERROR_BAD_ARG if sensor is not supported by the implementation
 *                      INV_ERROR_WATCHDOG in case of response timeout
 */
int INV_EXPORT inv_device_icm30670_watchdog_poll(inv_device_icm30xxx_t * self);

/*
 * Functions below are described in Device.h
 */

int INV_EXPORT inv_device_icm30xxx_whoami(void * context, uint8_t * whoami);

int INV_EXPORT inv_device_icm30xxx_reset(void * context);

int INV_EXPORT inv_device_icm30xxx_setup(void * context);

int INV_EXPORT inv_device_icm30xxx_cleanup(void * context);

int INV_EXPORT inv_device_icm30xxx_load(void * context, int what,
		const uint8_t * image, uint32_t size, inv_bool_t verify, inv_bool_t force);

int INV_EXPORT inv_device_icm30xxx_get_fw_info(void * context,
		struct inv_fw_version * version);

int INV_EXPORT inv_device_icm30xxx_set_running_state(void * context, inv_bool_t state);

int INV_EXPORT inv_device_icm30xxx_ping_sensor(void * context, int sensor);

int INV_EXPORT inv_device_icm30xxx_enable_sensor(void * context, int sensor, inv_bool_t en);

int INV_EXPORT inv_device_icm30xxx_set_sensor_period_us(void * context,
		int sensor, uint32_t period);

int INV_EXPORT inv_device_icm30xxx_set_sensor_timeout(void * context,
		int sensor, uint32_t timeout);

int INV_EXPORT inv_device_icm30xxx_flush_sensor(void * context, int sensor);

int INV_EXPORT inv_device_icm30xxx_set_sensor_bias(void * context, int sensor,
		const float bias[3]);

int INV_EXPORT inv_device_icm30xxx_get_sensor_bias(void * context, int sensor,
		float bias[3]);

int INV_EXPORT inv_device_icm30xxx_set_sensor_mounting_matrix(void * context,
		int sensor, const float matrix[9]);

int INV_EXPORT inv_device_icm30xxx_get_sensor_data(void * context, int sensor,
		inv_sensor_event_t * event);

int INV_EXPORT inv_device_icm30xxx_poll(void * context);

int INV_EXPORT inv_device_icm30xxx_self_test(void * context, int sensor);

/** @brief Allowed settings value for set_sensor_config() method
 */
enum inv_device_icm30670_config_settings {
	INV_DEVICE_ICM30670_CONFIG_FSR               = INV_SENSOR_CONFIG_CUSTOM + (int)INV_ICM30670_CONFIG_FSR,               /**< configure full-scale range for sensor */
	INV_DEVICE_ICM30670_CONFIG_BW                = INV_SENSOR_CONFIG_CUSTOM + (int)INV_ICM30670_CONFIG_BW,                /**< configure bandwith for sensor */
	INV_DEVICE_ICM30670_CONFIG_CUSTOM            = INV_SENSOR_CONFIG_CUSTOM + (int)INV_ICM30670_CONFIG_CUSTOM,            /**< configure custom sensor */
	INV_DEVICE_ICM30670_CONFIG_TSIMU_TEMPERATURE = INV_SENSOR_CONFIG_CUSTOM + (int)INV_ICM30670_CONFIG_TSIMU_TEMPERATURE, /**< configure temperature for TS-IMU sensor */ 
	INV_DEVICE_ICM30670_CONFIG_TSIMU_PROP        = INV_SENSOR_CONFIG_CUSTOM + (int)INV_ICM30670_CONFIG_TSIMU_PROP,        /**< configure PID proportional paramter for TS-IMU sensor */ 
	INV_DEVICE_ICM30670_CONFIG_TSIMU_INTG        = INV_SENSOR_CONFIG_CUSTOM + (int)INV_ICM30670_CONFIG_TSIMU_INTG,        /**< configure PID integral parameter for TS-IMU sensor */ 
	INV_DEVICE_ICM30670_CONFIG_TSIMU_DERI        = INV_SENSOR_CONFIG_CUSTOM + (int)INV_ICM30670_CONFIG_TSIMU_DERI,        /**< configure PID derivative parameter for TS-IMU sensor */ 
	INV_DEVICE_ICM30670_CONFIG_TSIMU_RPID        = INV_SENSOR_CONFIG_CUSTOM + (int)INV_ICM30670_CONFIG_TSIMU_RPID,        /**< configure enable/disable PID status report for TS-IMU sensor */ 
	INV_DEVICE_ICM30670_CONFIG_MAX
};

/** @brief Configure a sensor
 *
 *  @param sensor   the sensor must be set 
 *  @param setting  the configuration must be set
 *  @param value    the data's buffer must be set 
 *  @param size     the size of buffer
 *  @return         size of returned value in byte
 *
 *  @sa Allowed sensor value for ICM3063x device:
 *  -  settings for INV_SENSOR_TYPE_ENERGY_EXPENDITURE are :
 *  @sa - setting is expected to be 0
 *  @sa - value is expected to inv_sensor_config_energy_expenditure_t  
 *  @sa - size is expected to size of inv_sensor_config_energy_expenditure_t 
 *  - settings for INV_SENSOR_TYPE_DISTANCE are :
 *  @sa - setting is expected to be 0
 *  @sa - value is expected to inv_sensor_config_distance_t  
 *  @sa - size is expected to size of inv_sensor_config_distance_t 
 *  -  settings for INV_SENSOR_TYPE_SHAKE are :
 *  @sa - setting is expected to be 0
 *  @sa - value is expected to inv_sensor_config_shake_wrist_t  
 *  @sa - size is expected to size of inv_sensor_config_shake_wrist_t
 *  -  settings for INV_SENSOR_TYPE_DOUBLE_TAP are :
 *  @sa - setting is expected to be 0
 *  @sa - value is expected to inv_sensor_config_double_tap_t  
 *  @sa - size is expected to size of inv_sensor_config_double_tap_t 
 *  @sa Allowed sensor value for ICM30670 device:
 *  - INV_SENSOR_TYPE_GYROSCOPE:     @sa settings is expected to be enum inv_device_icm30670_config_settings
 *                                   @sa pvalue is expected to to a uint16_t containing of value:
 *                                    @sa - enum inv_icm30670_fsr_gyr
 *                                    @sa - enum inv_icm30670_bw_gyr
 *  - INV_SENSOR_TYPE_ACCELEROMETER: @sa settings is expected to be  enum inv_icm30670_config_setting
 *                                   @sa pvalue is expected to to a uint16_t containing of value:
 *                                    @sa - enum inv_icm30670_fsr_acc
 *                                    @sa - enum inv_icm30670_bw_acc
 */

int INV_EXPORT inv_device_icm30xxx_set_sensor_config(void * context, int sensor, int setting,
		const void * value, unsigned size);

/** @brief Send command to retrieve sensor configuration
 *
 *  @param sensor   the sensor must be get 
 *  @param setting  the configuration must be get
 *  @param value    the data's buffer must be received 
 *  @param size     the size of buffer received
 *  @return         size of returned value in byte
 *  @sa Allowed sensor value for ICM3063x device:
 *  -  settings for INV_SENSOR_TYPE_ENERGY_EXPENDITURE are :
 *  @sa - setting is expected to be 0
 *  @sa - value is expected to inv_sensor_config_energy_expenditure_t  
 *  @sa - size is expected to size of inv_sensor_config_energy_expenditure_t 
 *  - settings for INV_SENSOR_TYPE_DISTANCE are :
 *  @sa - setting is expected to be 0
 *  @sa - value is expected to inv_sensor_config_distance_t  
 *  @sa - size is expected to size of inv_sensor_config_distance_t
 *  -  settings for INV_SENSOR_TYPE_SHAKE are :
 *  @sa - setting is expected to be 0
 *  @sa - value is expected to inv_sensor_config_shake_wrist_t  
 *  @sa - size is expected to size of inv_sensor_config_shake_wrist_t
 *  -  settings for INV_SENSOR_TYPE_DOUBLE_TAP are :
 *  @sa - setting is expected to be 0
 *  @sa - value is expected to inv_sensor_config_double_tap_t  
 *  @sa - size is expected to size of inv_sensor_config_double_tap_t 
 *  @sa Allowed sensor value for ICM30670 device:
 *  @sa sensor   expected to INV_SENSOR_TYPE_ACCELEROMETER or INV_SENSOR_TYPE_GYROSCOPE
 *  @sa setting  expected to be a value of enum inv_icm30670_config_setting
 *  @sa value    expected to be a pointer to a uint16_t placeholder that will
 *                  contain the returned value as of:
 *                 @sa - enum inv_icm30670_fsr_acc
 *                 @sa - enum inv_icm30670_fsr_gyr
 *                 @sa - enum inv_icm30670_bw_acc
 *                 @sa - enum inv_icm30670_bw_gyr
 *
 */
int INV_EXPORT inv_device_icm30xxx_get_sensor_config(void * context, int sensor, int setting,
		void *value_out, unsigned size);

int INV_EXPORT inv_device_icm30xxx_write_mems_register(void * context, int sensor, uint16_t reg_addr,
		const void * data, unsigned size);

int INV_EXPORT inv_device_icm30xxx_read_mems_register(void * context, int sensor, uint16_t reg_addr,
		void * data, unsigned size);

/** @brief Checks if the device's oscillator has the right frequency
 *  @warning Blocking function which takes one second to execute
 * 
 *  @param[in]  self         handle to device icm30xxx object
 *  @return                  0 if oscillator frequency is within parameters
 *                           INV_ERROR otherwise
 */
int INV_EXPORT inv_device_icm30xxx_check_osc_freq(inv_device_icm30xxx_t * self);


/********** Berlin specific *************/

enum inv_device_berlin_kwr_settings {
	INV_DEVICE_BERLIN_KWR_SETTINGS_KWR_EN = INV_SENSOR_CONFIG_CUSTOM, /* enable KWR processing at ZSP level */
	INV_DEVICE_BERLIN_KWR_SETTINGS_AUDIO_EN,             /* enable audio processing at ZSP level */
	INV_DEVICE_BERLIN_KWR_SETTINGS_STREAMING_MODE,       /* configure streaming of audio data */
	INV_DEVICE_BERLIN_KWR_SETTINGS_TIMESTAMP_EN,         /* add timestamp to audio samples */
	INV_DEVICE_BERLIN_KWR_SETTINGS_UPDATE_MODEL_SENSORY, /* update Sensory KWR model */
	INV_DEVICE_BERLIN_KWR_SETTINGS_UPDATE_MODEL_GOOGLE,  /* update Google KWR model */
};

/** @brief Expected config value for INV_SENSOR_TYPE_KWR sensor settings
 */
union inv_device_berlin_kwr_settings_value {
	inv_bool_t enable; /**< ON/OFF value for KWR_EN, AUDIO_EN, TIMESTAMP_EN settings */
	int stream_mode;   /**< expected to be \sa inv_icm30xxx_audio_cmd_stream_mode */
	struct {
		const uint8_t * grammar_model;
		uint32_t        grammar_model_size;
		const uint8_t * net_model;
		uint32_t        net_model_size;
	} kwr_model_sensory; /**< reference to Sensory KWR model for UPDATE_MODEL_SENSORY */
	struct {
		const uint8_t * model;
		uint32_t        model_size;
	} kwr_model_google; /**< reference to Sensory KWR model for UPDATE_MODEL_GOOGLE */
};

/** @brief Configure a sensor
 *
 *  Allowed sensor value for Berlin device:
 *  - INV_SENSOR_TYPE_KWR: settings is expected to be \sa inv_device_berlin_kwr_settings
 *                         pvalue is expected to point to \sa inv_device_icm30xxx_kwr_settings_value
 *
 */
int INV_EXPORT inv_device_berlin_set_sensor_config(void * context, int sensor, int setting,
		const void * value, unsigned size);

int INV_EXPORT inv_device_berlin_poll(void * context);

#ifdef __cplusplus
}
#endif

#endif /* _INV_DEVICE_ICM30XXX_H_ */


/** @} */
