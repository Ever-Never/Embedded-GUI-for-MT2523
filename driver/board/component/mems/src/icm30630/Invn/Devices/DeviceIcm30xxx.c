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

#include "DeviceIcm30xxx.h"

#include "Invn/InvExport.h"

#include "Invn/EmbUtils/Message.h"

#include "Invn/FifoProtocol/FifoProtocolDefs.h"
#include "Invn/FifoProtocol/FifoProtocolSensorId.h"
#include "Invn/FifoProtocol/FifoProtocolHelper.h"
#include "Invn/FifoProtocol/FifoProtocolCore.h"

static const inv_device_vt_t device_icm30xxx_vt = {
	inv_device_icm30xxx_whoami,
	inv_device_icm30xxx_reset,
	inv_device_icm30xxx_setup,
	inv_device_icm30xxx_cleanup,
	inv_device_icm30xxx_load,
	inv_device_icm30xxx_poll,
	inv_device_icm30xxx_self_test,
	inv_device_icm30xxx_get_fw_info,
	inv_device_icm30xxx_ping_sensor,
	inv_device_icm30xxx_set_running_state,
	inv_device_icm30xxx_enable_sensor,
	inv_device_icm30xxx_set_sensor_period_us,
	inv_device_icm30xxx_set_sensor_timeout,
	inv_device_icm30xxx_flush_sensor,
	inv_device_icm30xxx_set_sensor_bias,
	inv_device_icm30xxx_get_sensor_bias,
	inv_device_icm30xxx_set_sensor_mounting_matrix,
	inv_device_icm30xxx_get_sensor_data,
	inv_device_icm30xxx_set_sensor_config,
	inv_device_icm30xxx_get_sensor_config,
	inv_device_icm30xxx_write_mems_register,
	inv_device_icm30xxx_read_mems_register,
};

static const inv_device_vt_t device_berlin_vt = {
	inv_device_icm30xxx_whoami,
	inv_device_icm30xxx_reset,
	inv_device_icm30xxx_setup,
	inv_device_icm30xxx_cleanup,
	inv_device_icm30xxx_load,
	inv_device_berlin_poll,
	0,
	inv_device_icm30xxx_get_fw_info,
	inv_device_icm30xxx_ping_sensor,
	inv_device_icm30xxx_set_running_state,
	inv_device_icm30xxx_enable_sensor,
	inv_device_icm30xxx_set_sensor_period_us,
	inv_device_icm30xxx_set_sensor_timeout,
	inv_device_icm30xxx_flush_sensor,
	inv_device_icm30xxx_set_sensor_bias,
	inv_device_icm30xxx_get_sensor_bias,
	inv_device_icm30xxx_set_sensor_mounting_matrix,
	inv_device_icm30xxx_get_sensor_data,
	inv_device_berlin_set_sensor_config,
	0,
	0,
	0,
};

static void build_fifo_protocol_id_map_common(inv_device_icm30xxx_t * self);
static void build_fifo_protocol_id_map_icm3063x(inv_device_icm30xxx_t * self);
static void build_fifo_protocol_id_map_icm30670(inv_device_icm30xxx_t * self);
static void build_fifo_protocol_id_map_berlin(inv_device_icm30xxx_t * self);
static inv_bool_t build_data_icm3063x(uint8_t sensorid, uint8_t accuracy,
		const uint8_t * data, uint16_t len, inv_sensor_event_t * event);
static inv_bool_t build_data_icm30670(uint8_t sensorid, uint8_t accuracy,
		const uint8_t * data, uint16_t len, inv_sensor_event_t * event);
static inv_bool_t build_data_berlin(uint8_t sensorid, uint8_t accuracy,
		const uint8_t * data, uint16_t len, inv_sensor_event_t * event);
static int sensor_type_2_fifo_protocol_id(const inv_device_icm30xxx_t * self,
		int type);
static int fifo_protocol_id_2_sensor_type(const inv_device_icm30xxx_t * self,
		uint8_t sensorid);
static inv_bool_t build_sensor_event(void * context, uint8_t sensorid, uint32_t timestamp,
		uint8_t status, uint8_t accuracy, const uint8_t * data, uint16_t len,
		inv_sensor_event_t * event);
static void data_handler_wrapper(void * context, uint8_t sensorid,
		uint32_t timestamp, uint8_t status, uint8_t accuracy,
		const uint8_t * data, uint16_t size);

int inv_device_icm30xxx_register_interrupt_callback(inv_device_icm30xxx_t * self,
	   void (*interrupt_cb)(void * context, int int_num), void * context)
{
	(void)self, (void)interrupt_cb, (void)context;

	/*
	 * Just here to break existing code to compile but does nothing
	 * Will be removed in a future libIDD delivery which increments the major.
	 */

	return INV_ERROR_NIMPL;
}

void inv_device_icm30xxx_enable_logger(inv_device_icm30xxx_t * self,
	   uint8_t level, void (*putc_cb)(void * context, int c), void * context)

{
	assert(self);
	self->logger.level   = putc_cb ? level : INV_ICM30XXX_LOGGER_LVL_OFF;
	self->logger.putc_cb = putc_cb;
	self->logger.context = context;
}

void inv_device_icm30xxx_register_debugger_hook_callback(inv_device_icm30xxx_t * self,
	   void (*dbg_hook_cb)(void * context), void * context)
{
	assert(self);

	self->dbg_hook_cb      = dbg_hook_cb;
	self->dbg_hook_context = context;
}

static void device_base_init(inv_device_icm30xxx_t * self, 
		const inv_sensor_listener_t * listener,
		const inv_device_vt_t * pvt)
{
	assert(self && listener && pvt);

	memset(self, 0, sizeof(*self));

	/* build base */
	self->base.instance = self;
	self->base.vt       = pvt;
	self->base.listener = listener;
}

static void device_icm30xxx_init(inv_device_icm30xxx_t * self,
		const inv_serif_hal_t * serif, uint8_t * buffer, uint32_t buffer_size,
		inv_bool_t debug, int chip_variant)
{
	struct inv_icm30xxx_serif icm30xxx_serif;
	int poll_int_mask;

	assert(self && buffer);

	/* initialize icm30xxx serif structure */
	icm30xxx_serif.context   = serif->context;
	icm30xxx_serif.read_reg  = serif->read_reg;
	icm30xxx_serif.write_reg = serif->write_reg;
	icm30xxx_serif.max_read  = serif->max_read_transaction_size;
	icm30xxx_serif.max_write = serif->max_write_transaction_size;
	icm30xxx_serif.is_spi    = !!(serif->serif_type == INV_SERIF_HAL_TYPE_SPI);

	poll_int_mask = ICM30XXX_POLL_SCRATCH_INT0 | ICM30XXX_POLL_SCRATCH_INT1;

	if(chip_variant == INV_ICM30XXX_VARIANT_ICM30670) {
		/* For ICM30670, data interrupt are INT0 and INT2 */
		poll_int_mask = ICM30XXX_POLL_SCRATCH_INT0 | ICM30XXX_POLL_SCRATCH_INT2;
	}

	/* reset icm30xxx driver states */
	inv_icm30xxx_reset_states(&self->icm30xxx_states, chip_variant, &icm30xxx_serif,
		buffer, buffer_size, poll_int_mask, data_handler_wrapper, self);

	self->clock_rate = 1;
	self->debug_en   = debug;

	/* init cb and context */
	self->irq_callback         = 0;
	self->irq_callback_context = 0;
	self->dbg_hook_cb          = 0;
	self->dbg_hook_context     = 0;

	/* build map for type/sensor id conversion */
	memset(self->map_2sensortype, -1, sizeof(self->map_2sensortype));
	build_fifo_protocol_id_map_common(self);

	/* reset other states */
	self->reset_done     = false;
	self->ts_imu_started = false;
	self->hrgyr_started = false;
}

void inv_device_icm3063x_init2(inv_device_icm30xxx_t * self,
		const inv_serif_hal_t * serif, const inv_sensor_listener_t * listener,
		uint8_t * buffer, uint32_t buffer_size, inv_bool_t jtag_en)
{
	device_base_init(self, listener, &device_icm30xxx_vt);
	device_icm30xxx_init(self, serif, buffer, buffer_size, jtag_en,
			INV_ICM30XXX_VARIANT_ICM3063x);

	build_fifo_protocol_id_map_icm3063x(self);

	self->build_data = build_data_icm3063x;

	/* Set data packet number for raw ppg */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_0,
			1, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for hrv */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_4,
			2, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for sleep analysis */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_6,
			4, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for BAC Extended */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_8,
			1, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for BAC Statistics */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_9,
			5, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for Floor Climbed Counter */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_10,
			2, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for Energy Expenditure */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_11,
			2, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for Distance */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_12,
			2, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for Shake */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_13,
			1, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for Double Tap */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_14,
			1, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for Sendentary Reminder */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_15,
			1, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for Data Encryption */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_16,
			4, self->icm30xxx_states.fifop_exp_pkt_cnt);
}

void inv_device_icm30670_init2(inv_device_icm30xxx_t * self,
		const inv_serif_hal_t * serif, const inv_sensor_listener_t * listener,
		uint8_t * buffer, uint32_t buffer_size, inv_bool_t jtag_en)
{
	device_base_init(self, listener, &device_icm30xxx_vt);
	device_icm30xxx_init(self, serif, buffer, buffer_size, jtag_en,
			INV_ICM30XXX_VARIANT_ICM30670);

	build_fifo_protocol_id_map_icm30670(self);

	self->build_data = build_data_icm30670;

	/* Set data packet number for EIS gyroscope */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_4,
			2, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for EIS gyroscope */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_4,
			2, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for raw accelerometer */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_1,
			1, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for raw gyroscope */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_5,
			1, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for raw magnetometer */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_3,
			1, self->icm30xxx_states.fifop_exp_pkt_cnt);
	/* Set data packet number for raw temperature */
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_2,
			1, self->icm30xxx_states.fifop_exp_pkt_cnt);	
	inv_fifo_protocol_data_packet_number_set_r(FIFOPROTOCOL_SENSOR_ID_EXT_8,
			1, self->icm30xxx_states.fifop_exp_pkt_cnt);	
}


/* For BW compatibility
 * wrapper function to adapt prototype of read_reg() as defined in inv_host_serif
 * to expected prototype for inv_serif_hal
 */
static int host_serif_read_reg_legacy(void * context, uint8_t reg, uint8_t * data, uint32_t len)
{
	inv_host_serif_t * serif = (inv_host_serif_t *)context;

	return serif->read_reg(reg, data, len);
}

/* For BW compatibility
 * wrapper function to adapt prototype of write_reg() as defined in inv_host_serif
 * to expected prototype for inv_serif_hal
 */
static int host_serif_write_reg_legacy(void * context, uint8_t reg, const uint8_t * data, uint32_t len)
{
	inv_host_serif_t * serif = (inv_host_serif_t *)context;

	return serif->write_reg(reg, data, len);
}

void inv_device_icm3063x_init(inv_device_icm30xxx_t * self,
		const inv_host_serif_t * serif, const inv_sensor_listener_t * listener,
		uint8_t * buffer, uint32_t buffer_size, inv_bool_t jtag_en)
{
	/* create an a inv_serif_hal_t object from a inv_host_serif_t */
	const inv_serif_hal_t serif_hal = {
		host_serif_read_reg_legacy, host_serif_write_reg_legacy, /* use small wrappers to adapt prototype */
		serif->max_read_size, serif->max_write_size,
		serif->serif_type, &self->legacy_serif
	};

	/* call the 'constructor' */
	inv_device_icm3063x_init2(self, &serif_hal, listener, buffer, buffer_size, jtag_en);
	/* keep a copy of the user inv_host_serif_t (used in the _legacy callbacks) */
	self->legacy_serif = *serif;
}

void inv_device_icm30670_init(inv_device_icm30xxx_t * self,
		const inv_host_serif_t * serif, const inv_sensor_listener_t * listener,
		uint8_t * buffer, uint32_t buffer_size, inv_bool_t jtag_en)
{
	/* create an a inv_serif_hal_t object from a inv_host_serif_t */
	const inv_serif_hal_t serif_hal = {
		host_serif_read_reg_legacy, host_serif_write_reg_legacy, /* use small wrappers to adapt prototype */
		serif->max_read_size, serif->max_write_size,
		serif->serif_type, &self->legacy_serif
	};

	/* call the 'constructor' */
	inv_device_icm30670_init2(self, &serif_hal, listener, buffer, buffer_size, jtag_en);
	/* keep a copy of the user inv_host_serif_t (used in the _legacy callbacks) */
	self->legacy_serif = *serif;
}

void inv_device_berlin_init(inv_device_icm30xxx_t * self,
		const inv_serif_hal_t * serif, const inv_sensor_listener_t * listener,
		uint8_t * buffer, uint32_t buffer_size, inv_bool_t jtag_en)
{
	device_base_init(self, listener, &device_berlin_vt);
	device_icm30xxx_init(self, serif, buffer, buffer_size, jtag_en,
			INV_ICM30XXX_VARIANT_BERLIN);

	build_fifo_protocol_id_map_berlin(self);

	self->build_data = build_data_berlin;
}

void inv_device_icm30670_init_serif_i2cslave(inv_device_icm30xxx_t * self, const inv_host_serif_t * serif_i2cslave)
{
	struct inv_icm30xxx_serif icm30670_serif_i2cslave;

	assert(self);

	icm30670_serif_i2cslave.context   = &self->legacy_serif_slave;
	icm30670_serif_i2cslave.read_reg  = host_serif_read_reg_legacy;
	icm30670_serif_i2cslave.write_reg = host_serif_write_reg_legacy;
	icm30670_serif_i2cslave.max_read  = serif_i2cslave->max_read_size;
	icm30670_serif_i2cslave.max_write = serif_i2cslave->max_write_size;
	icm30670_serif_i2cslave.is_spi    = 0;

	/* reset icm30670 driver states */
	inv_icm30670_reset_states_serif_i2cslave(&self->icm30xxx_states, &icm30670_serif_i2cslave);
        self->legacy_serif_slave = *serif_i2cslave;

}

void inv_device_icm30670_init_serif_i2cslave2(inv_device_icm30xxx_t * self, const inv_serif_hal_t * serif_i2cslave)
{
	struct inv_icm30xxx_serif icm30670_serif_i2cslave;

	assert(self);

	icm30670_serif_i2cslave.context   = serif_i2cslave->context;
	icm30670_serif_i2cslave.read_reg  = serif_i2cslave->read_reg;
	icm30670_serif_i2cslave.write_reg = serif_i2cslave->write_reg;
	icm30670_serif_i2cslave.max_read  = serif_i2cslave->max_read_transaction_size;
	icm30670_serif_i2cslave.max_write = serif_i2cslave->max_write_transaction_size;
	icm30670_serif_i2cslave.is_spi    = !!(serif_i2cslave->serif_type == INV_SERIF_HAL_TYPE_SPI);

	/* reset icm30670 driver states */
	inv_icm30670_reset_states_serif_i2cslave(&self->icm30xxx_states, &icm30670_serif_i2cslave);
}

static int poll_device_common(inv_device_icm30xxx_t * self, uint8_t * fifo_mask)
{
	int rc = 0;

	if(self->logger.level) {
		rc = inv_imc30xxx_print_logger_data(&self->icm30xxx_states,
				self->logger.putc_cb, self->logger.context);

		if(rc)
			return rc;
	}

	rc = inv_icm30xxx_read_int_status(&self->icm30xxx_states,
			self->icm30xxx_states.poll_int_mask, fifo_mask);
	if(!rc && *fifo_mask)
	    rc = inv_icm30xxx_ctrl_poll_sensor(&self->icm30xxx_states, *fifo_mask);
        
    if (self->icm30xxx_states.use_serif_i2cslave)		// If we have started I2C Slave Mode
		inv_icm30xxx_read_and_parse_i2c(&self->icm30xxx_states);		
	
		return rc;
}

int inv_device_icm30xxx_poll(void * context)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	uint8_t fifo_mask;

	return poll_device_common(self, &fifo_mask);
}

int inv_device_berlin_poll(void * context)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	uint8_t fifo_mask;
	int rc;

	assert(self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_BERLIN);

	if((rc = poll_device_common(self, &fifo_mask)) != 0)
		return rc;

	return inv_icm30xxx_ctrl_poll_audio(&self->icm30xxx_states, fifo_mask);
}

#define HEARTBEAT_REQ_MASK      0x40 /* mask indicating the hearbeat request bit */
#define HEARTBEAT_NOTIF_MASK    0x40 /* mask indicating the heartbeat notify bit */

int inv_device_icm30670_watchdog_poll(inv_device_icm30xxx_t * self)
{
	int rc;
	int timeout = 1000; /* 1s */	
	void * context = (void *)(self);
	inv_icm30xxx_t * s = &self->icm30xxx_states;
	inv_bool_t watchdog_notif = false;
	
	/* Prevent device from entering sleep */
	if((rc = inv_icm30xxx_lp_enable(s, false)) != 0)
		return rc;

	/* Send heartbeat request to the device */
	if((rc = inv_icm30xxx_set_scratch_m0_int(s, HEARTBEAT_REQ_MASK)) != 0)
		return rc;
	
	do { /* Read interrupt status to check heartbeat notification */
		uint8_t int_status;

		if((rc = inv_icm30xxx_read_int_status(s, ICM30XXX_POLL_SCRATCH_INT1, &int_status)) != 0)
			return rc;

		if(int_status & HEARTBEAT_NOTIF_MASK) { 
			/* Heartbeat notification received */
			watchdog_notif = true;
		} else {
			inv_icm30xxx_sleep(10);
			timeout -= 10;

			if(timeout <= 0) { /* Device did not respond in time, reset it !!! */
				if((rc = inv_device_icm30xxx_reset(context)) != 0) {
					return rc;
				} else {
					/* Reset successful */
					return INV_ERROR_WATCHDOG;
				}
			}
		}

	} while(!watchdog_notif);

	/* Allow device to enter sleep */
	rc = inv_icm30xxx_lp_enable(s, false);
	return rc;
}

int inv_device_icm30xxx_whoami(void * context, uint8_t * whoami)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;

	assert(whoami);

	return inv_icm30xxx_get_whoami(&self->icm30xxx_states, whoami);
}

int inv_device_icm30xxx_reset(void * context)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	int rc;

	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Reseting device...");

	inv_icm30xxx_lp_inhibit_reset(&self->icm30xxx_states);

	if((rc = inv_icm30xxx_soft_reset(&self->icm30xxx_states)) != 0) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "Icm30xxx soft reset returned %d", rc);
		return rc;
	}

	self->reset_done = true;
	self->ts_imu_started = false;
	self->hrgyr_started = false;
	
	return 0;
}

int inv_device_icm30xxx_setup(void * context)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	int rc;
	uint8_t whoami, ping_value;

	INV_MSG(INV_MSG_LEVEL_INFO, "Booting up Icm30xxx...");

	/* reset if not already done */
	if(!self->reset_done) {
		if((rc = inv_device_icm30xxx_reset(context)) != 0)
			return rc;
	}

	/* Check WHOAMI */
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Reading WHOAMI...");
	if((rc = inv_device_icm30xxx_whoami(self, &whoami)) != 0) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "Error %d when reading WHOAMI value", rc);
		return rc;
	}

	if(whoami == 0 || whoami == 0xff) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "Unexpected WHOAMI value 0x%x. Aborting setup.", whoami);
		return INV_ERROR;
	} else {
		INV_MSG(INV_MSG_LEVEL_INFO, "WHOAMI value: 0x%x", whoami);
	}

	/* start M0 */
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Starting M0 (debug=%s)", (self->debug_en) ? "on" : "off");
	if((rc = inv_icm30xxx_start_m0(&self->icm30xxx_states, self->debug_en)) != 0)
		goto error;

	/* set default power mode */
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Putting Icm30xxx in sleep mode...");
	if((rc = inv_icm30xxx_set_sleep(&self->icm30xxx_states)) != 0)
		goto error;	

	/* configure logger */
	if((rc = inv_imc30xxx_logger_setup(&self->icm30xxx_states, self->logger.level)) != 0)
		goto error;

	if(self->dbg_hook_cb) {
		INV_MSG(INV_MSG_LEVEL_VERBOSE, "Calling debugger hook...");
		self->dbg_hook_cb(self->dbg_hook_context);
	}

	/* caching FIFOs config */
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Retrieving FIFOs configuration...");
	if((rc = inv_icm30xxx_ctrl_retrieve_fifo_config(&self->icm30xxx_states)) != 0)
		goto error;

	/* start ZSP (for Berlin) */
	if(self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_BERLIN) {
		INV_MSG(INV_MSG_LEVEL_VERBOSE, "Starting ZSP...");
		if((rc = inv_icm30xxx_start_zsp(&self->icm30xxx_states)) != 0)
			goto error;

		/* in case of Berlin, inhibit LP_EN (due to current HW limitation on FPGA) */
		if((rc != inv_icm30xxx_lp_inhibit(&self->icm30xxx_states, true)) != 0)
			goto error;
	}

	/* ping system to see if it responding */
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Ping system to see if it is responding...");
	if((rc = inv_icm30xxx_cmd_ping_sensor(&self->icm30xxx_states, 0, &ping_value)) != 0)
		goto error;

	if(!ping_value) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "System not responding. Something went wrong.");
		rc = INV_ERROR;
		goto error;
	} else {
		INV_MSG(INV_MSG_LEVEL_INFO, "System responded OK to ping");
	}

	/* retrieve clock rate to convert received data timestamp to standard unit */
	// FIXME Will fail if we do this before loading DMP image because of FW bug
	INV_MSG(INV_MSG_LEVEL_INFO, "Getting clock rate...");
	if((rc = inv_icm30xxx_cmd_get_clock_rate(&self->icm30xxx_states, &self->clock_rate)) != 0)
		goto error;

	INV_MSG(INV_MSG_LEVEL_VERBOSE, "System clock rate is %lu ns/tick", self->clock_rate);

	/* reading firmware version */
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Reading firmware version...");
	if((rc = inv_device_icm30xxx_get_fw_info(context, &self->fw_version)) != 0) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "Error %d when reading firmware version", rc);
		goto error;
	} else {
		INV_MSG(INV_MSG_LEVEL_VERBOSE, "Firmware version: %d.%d.%d%s", self->fw_version.major, 
				self->fw_version.minor, self->fw_version.patch, self->fw_version.suffix);
	}

	/* we should be good to go ! */
	INV_MSG(INV_MSG_LEVEL_VERBOSE, "We're good to go !");

	return 0;
error:
	INV_MSG(INV_MSG_LEVEL_ERROR, "Error %d while setting-up device.", rc);

	return rc;
}

int inv_device_icm30xxx_cleanup(void * context)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;

	self->reset_done = false;

	return inv_icm30xxx_soft_reset(&self->icm30xxx_states);
}

int inv_device_icm30xxx_load(void * context, int what,
		const uint8_t * image, uint32_t size, inv_bool_t verify, inv_bool_t force)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;

	switch(what) {
		case INV_DEVICE_ICM30XXX_IMAGE_TYPE_M0_FLASH:
			INV_MSG(INV_MSG_LEVEL_INFO,
					"Attempting to load %u bytes into FLASH...", size);
			return inv_icm30xxx_load_flash(&self->icm30xxx_states, image,
					size, verify, force);

		case INV_DEVICE_ICM30XXX_IMAGE_TYPE_M0_SRAM:
			INV_MSG(INV_MSG_LEVEL_INFO,
					"Attempting to load %u bytes into SRAM...", size);
			return inv_icm30xxx_load_ram(&self->icm30xxx_states, image, size, verify);

		case INV_DEVICE_ICM30XXX_IMAGE_TYPE_DMP3:
			INV_MSG(INV_MSG_LEVEL_INFO,
					"Attempting to load %u bytes into DMP3...", size);
			return inv_icm30xxx_load_other(&self->icm30xxx_states,
					FIFOPROTOCOL_LOAD_WHO_DMP3_FW, image, size);

		case INV_DEVICE_ICM30XXX_IMAGE_TYPE_DMP4:
			INV_MSG(INV_MSG_LEVEL_INFO,
					"Attempting to load %u bytes into DMP4...", size);
			return inv_icm30xxx_load_other(&self->icm30xxx_states,
					FIFOPROTOCOL_LOAD_WHO_DMP4_FW, image, size);

		case INV_DEVICE_ICM30XXX_IMAGE_TYPE_ZSP_IMEM:
			INV_MSG(INV_MSG_LEVEL_INFO,
					"Attempting to load %u bytes into ZSP IMEM...", size);
			return inv_icm30xxx_load_zsp_imem(&self->icm30xxx_states, image, size);

		case INV_DEVICE_ICM30XXX_IMAGE_TYPE_ZSP_DMEM:
			INV_MSG(INV_MSG_LEVEL_INFO,
					"Attempting to load %u bytes into ZSP DMEM...", size);
			return inv_icm30xxx_load_zsp_dmem(&self->icm30xxx_states, image, size);

		default:
			return INV_ERROR_BAD_ARG;
	}
}

int inv_device_icm30xxx_get_fw_info(void * context,
		struct inv_fw_version * version)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	int rc;
	struct inv_icm30xxx_fw_version icm30xxx_version;

	assert(version);

	if((rc = inv_icm30xxx_cmd_get_fw_info(&self->icm30xxx_states,
			&icm30xxx_version)) != 0)
		return rc;

	version->major = icm30xxx_version.major;
	version->minor = icm30xxx_version.minor;
	version->patch = icm30xxx_version.patch;
	version->crc   = icm30xxx_version.crc;
	memset(version->suffix, '\0', sizeof(version->suffix));
	strncpy(version->suffix, icm30xxx_version.hash, sizeof(icm30xxx_version.hash));

	return 0;
}

int inv_device_icm30xxx_set_running_state(void * context, inv_bool_t state)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;

	return inv_icm30xxx_cmd_set_running_state(&self->icm30xxx_states, state);
}

int inv_device_icm30xxx_ping_sensor(void * context, int sensor)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	int rc;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;
	uint8_t ping_value;

	if(id == -1)
		return INV_ERROR_BAD_ARG;

	if((rc = inv_icm30xxx_cmd_ping_sensor(&self->icm30xxx_states, sid,
			&ping_value)) != 0)
		return rc;

	if(ping_value == 0)
		return INV_ERROR;

	return 0;
}

int inv_device_icm30xxx_enable_sensor(void * context, int sensor, inv_bool_t en)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	if(id == -1)
		return INV_ERROR_BAD_ARG;

	if(self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_BERLIN
			&& !en && INV_SENSOR_ID_TO_TYPE(sensor) == INV_SENSOR_TYPE_KWR) {
		inv_icm30xxx_ctrl_stop_poll_audio(&self->icm30xxx_states);
	}
	if(self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_ICM30670) {
		if (INV_SENSOR_ID_TO_TYPE(sensor) == INV_SENSOR_TYPE_TSIMU) {
			if((en && !self->ts_imu_started) || (!en && self->ts_imu_started)) {
				self->ts_imu_started = en;
				inv_icm30xxx_lp_inhibit(&self->icm30xxx_states, en);
			}
		}
		if (INV_SENSOR_ID_TO_TYPE(sensor) == INV_SENSOR_TYPE_HIGH_RATE_GYRO) {
			if((en && !self->hrgyr_started) || (!en && self->hrgyr_started)) {
				self->hrgyr_started = en;
				inv_icm30xxx_lp_inhibit(&self->icm30xxx_states, en);
			}
		}
	}
	return inv_icm30xxx_cmd_enable_sensor(&self->icm30xxx_states, sid, en);
}

int inv_device_icm30xxx_set_sensor_period_us(void * context,
		int sensor, uint32_t period)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	if(id == -1)
		return INV_ERROR_BAD_ARG;

	/** Firmware versions more recent than 5.0.0 (including 5.0.0) expect microseconds 
	 *  wereas older versions expect milliseconds 
	 */
	if (self->fw_version.major < 5) {
		period /= 1000;
	}

	return inv_icm30xxx_cmd_set_sensor_period(&self->icm30xxx_states, sid, period);
}

int inv_device_icm30xxx_set_sensor_timeout(void * context,
		int sensor, uint32_t timeout)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	if(id == -1)
		return INV_ERROR_BAD_ARG;

	return inv_icm30xxx_cmd_set_sensor_timeout(&self->icm30xxx_states, sid, timeout);
}

int inv_device_icm30xxx_flush_sensor(void * context, int sensor)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	if(id == -1)
		return INV_ERROR_BAD_ARG;

	return inv_icm30xxx_cmd_flush_sensor(&self->icm30xxx_states, sid);
}

int inv_device_icm30xxx_set_sensor_bias(void * context, int sensor,
		const float bias[3])
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	if(id == -1)
		return INV_ERROR_BAD_ARG;

	return inv_icm30xxx_cmd_set_sensor_bias(&self->icm30xxx_states, sid, bias);
}

int inv_device_icm30xxx_get_sensor_bias(void * context, int sensor,
		float bias[3])
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	if(id == -1)
		return INV_ERROR_BAD_ARG;

	return inv_icm30xxx_cmd_get_sensor_bias(&self->icm30xxx_states, sid, bias);
}

int inv_device_icm30xxx_set_sensor_mounting_matrix(void * context,
		int sensor, const float matrix[9])
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	if(id == -1)
		return INV_ERROR_BAD_ARG;

	return inv_icm30xxx_cmd_set_sensor_mounting_matrix(&self->icm30xxx_states, sid,
			matrix);
}

int inv_device_icm30xxx_get_sensor_data(void * context, int sensor,
		inv_sensor_event_t * event)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	int rc;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;
	uint8_t data[64];
	uint16_t size;
	uint32_t timestamp;

	if(id == -1)
		return INV_ERROR_BAD_ARG;

	if((rc = inv_icm30xxx_cmd_get_last_sensor_data(&self->icm30xxx_states, sid,
			&timestamp, data, &size)) != 0)
		return rc;

	if(!build_sensor_event(context, sid, timestamp, FIFOPROTOCOL_STATUS_POLL, 0,
			data, size, event))
		return INV_ERROR;

	return 0;
}

static int set_kwr_sensor_config(inv_device_icm30xxx_t * self,
	int settings, const union inv_device_berlin_kwr_settings_value * value)
{
	switch(settings)
	{
	case INV_DEVICE_BERLIN_KWR_SETTINGS_KWR_EN:
		return inv_icm30xxx_cmd_enable_kwr(&self->icm30xxx_states, value->enable);
	case INV_DEVICE_BERLIN_KWR_SETTINGS_AUDIO_EN:
		return inv_icm30xxx_cmd_enable_audio_detect(&self->icm30xxx_states, value->enable);
	case INV_DEVICE_BERLIN_KWR_SETTINGS_STREAMING_MODE:
		return inv_icm30xxx_cmd_enable_audio_stream(&self->icm30xxx_states, value->stream_mode);
	case INV_DEVICE_BERLIN_KWR_SETTINGS_TIMESTAMP_EN:
		return inv_icm30xxx_cmd_enable_audio_timestamp(&self->icm30xxx_states, value->enable);
	case INV_DEVICE_BERLIN_KWR_SETTINGS_UPDATE_MODEL_SENSORY:
		return inv_icm30xxx_cmd_configure_audio_update_kwr_sensory_model(&self->icm30xxx_states,
				value->kwr_model_sensory.grammar_model, value->kwr_model_sensory.grammar_model_size,
				value->kwr_model_sensory.net_model, value->kwr_model_sensory.net_model_size);
	case INV_DEVICE_BERLIN_KWR_SETTINGS_UPDATE_MODEL_GOOGLE:
		return inv_icm30xxx_cmd_configure_audio_update_kwr_google_model(&self->icm30xxx_states,
				value->kwr_model_google.model, value->kwr_model_google.model_size);
	default:
		return INV_ERROR_BAD_ARG;
	}
}

static int set_sensor_config_berlin(inv_device_icm30xxx_t * self, int sensor, int settings,
		const void * pvalue, unsigned size)
{
	(void)size;

	switch(sensor) {
	case INV_SENSOR_TYPE_KWR:
		assert(size >= sizeof(union inv_device_berlin_kwr_settings_value));
		return set_kwr_sensor_config(self, settings,
				(const union inv_device_berlin_kwr_settings_value *)pvalue);
	default:
		return INV_ERROR_BAD_ARG;
	}
}

int inv_device_icm30xxx_set_sensor_config(void * context, int sensor, int setting,
		const void * value, unsigned size)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	if (id == INV_SENSOR_TYPE_STEP_COUNTER && setting == INV_SENSOR_CONFIG_RESET) {
		return inv_icm30xxx_cmd_reset_sensor(&self->icm30xxx_states, sid);
	}
	else if((self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_ICM30670) || (self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_ICM3063x)) {
		if(setting >= INV_SENSOR_CONFIG_CUSTOM) {
			setting -= INV_SENSOR_CONFIG_CUSTOM; /* to avoid breaking existing code */
		}

		return inv_icm30xxx_cmd_set_sensor_config(&self->icm30xxx_states, sid,
				setting, value, size);
	}
	else {
		return INV_ERROR_NIMPL;
	}
}

int inv_device_berlin_set_sensor_config(void * context, int sensor, int setting,
		const void * value, unsigned size)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;

	assert(self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_BERLIN);

	/* treat indifferently WU or NL sensor */
	sensor = INV_SENSOR_ID_TO_TYPE(sensor);

	return set_sensor_config_berlin(self, sensor, setting, value, size);
}

int	inv_device_icm30xxx_get_sensor_config(void * context, int sensor, int setting,
		void *value_out, unsigned size)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	if((self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_ICM30670) || (self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_ICM3063x)) {
		if(setting >= INV_SENSOR_CONFIG_CUSTOM)
			setting -= INV_SENSOR_CONFIG_CUSTOM; /* to avoid breaking existing code */

		return inv_icm30xxx_cmd_get_sensor_config(&self->icm30xxx_states, sid, setting, value_out, size);
	}
	else
		return INV_ERROR_NIMPL;
}

int inv_device_icm30xxx_write_mems_register(void * context, int sensor, uint16_t reg_addr,
		const void * data, unsigned size)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	return inv_icm30xxx_cmd_write_mems_register(&self->icm30xxx_states, sid, reg_addr, data, size);
}

int inv_device_icm30xxx_read_mems_register(void * context, int sensor, uint16_t reg_addr,
		void * data, unsigned size)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	const int id = sensor_type_2_fifo_protocol_id(self, sensor);
	const uint8_t sid = (uint8_t)id;

	return inv_icm30xxx_cmd_read_mems_register(&self->icm30xxx_states, sid, reg_addr, data, size);
}

#define ICM30XXX_OSC_TOL 5 /* oscillator tolerance in % */

int inv_device_icm30xxx_check_osc_freq(inv_device_icm30xxx_t * self)
{
	int rc;
	uint32_t tick1, tick2, delta, upper_margin, lower_margin;

	INV_MSG(INV_MSG_LEVEL_VERBOSE, "Checking oscillator frequency. Expected value: %d Hz with %d%s tolerance", 
		ICM30XXX_OSC_FREQ, ICM30XXX_OSC_TOL, "%");

	/* Read timestamp register */
	if ((rc = inv_icm30xxx_get_cpu_ticks(&self->icm30xxx_states, &tick1)) != 0)
		return rc;
	inv_icm30xxx_sleep(1000);
	if ((rc = inv_icm30xxx_get_cpu_ticks(&self->icm30xxx_states, &tick2)) != 0)
		return rc;

	/* Compute delta taking into account possible timestamp register overflow */
	if (tick2 > tick1)
		delta = tick2 - tick1;
	else
		delta = 0xFFFFFFFF + tick2 - tick1;

	/* Compute valid frequency range */
	upper_margin = ICM30XXX_OSC_FREQ + (ICM30XXX_OSC_FREQ * ICM30XXX_OSC_TOL / 100);
	lower_margin = ICM30XXX_OSC_FREQ - (ICM30XXX_OSC_FREQ * ICM30XXX_OSC_TOL / 100);

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Measured oscillator frequency: %d Hz", delta);

	/* Evaluate measured frequency */
	if ((delta < upper_margin) && (delta > lower_margin)) {
		return 0;
	} else {
		return INV_ERROR;
	}
}

int inv_device_icm30xxx_self_test(void * context, int sensor)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;

	INV_MSG(INV_MSG_LEVEL_DEBUG, "Self Tests ICM30xxx sensor %s ",
		inv_sensor_str(sensor));

	switch(sensor) {
	case INV_SENSOR_TYPE_RESERVED:
		return inv_device_icm30xxx_check_osc_freq(self);
	case INV_SENSOR_TYPE_ACCELEROMETER:
	case INV_SENSOR_TYPE_GYROSCOPE:
	case INV_SENSOR_TYPE_RAW_ACCELEROMETER:
	case INV_SENSOR_TYPE_RAW_GYROSCOPE:
		if(self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_ICM30670) {
			return inv_icm30670_do_self_test(&self->icm30xxx_states);
		} else {
			return INV_ERROR_NIMPL;
		}
	default:
		return INV_ERROR_NIMPL;
	}
}

/******************************************************************************/

static void data_handler_wrapper(void * context, uint8_t sensorid,
		uint32_t timestamp, uint8_t status, uint8_t accuracy,
		const uint8_t * data, uint16_t size)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	inv_sensor_event_t event;

	if(build_sensor_event(context, sensorid, timestamp, status, accuracy,
			data, size, &event)) {
		inv_sensor_listener_notify(self->base.listener, &event);
	}
}

static void build_fifo_protocol_id_map_common(inv_device_icm30xxx_t * self)
{
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_META_DATA]              = INV_SENSOR_TYPE_RESERVED;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_ACCELEROMETER]          = INV_SENSOR_TYPE_ACCELEROMETER;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_MAGNETOMETER]           = INV_SENSOR_TYPE_MAGNETOMETER;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_ORIENTATION]            = INV_SENSOR_TYPE_ORIENTATION;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_GYROSCOPE]              = INV_SENSOR_TYPE_GYROSCOPE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_LIGHT]                  = INV_SENSOR_TYPE_LIGHT;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_PRESSURE]               = INV_SENSOR_TYPE_PRESSURE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_TEMPERATURE]            = INV_SENSOR_TYPE_TEMPERATURE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_PROXIMITY]              = INV_SENSOR_TYPE_PROXIMITY;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_GRAVITY]                = INV_SENSOR_TYPE_GRAVITY;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_LINEAR_ACCELERATION]    = INV_SENSOR_TYPE_LINEAR_ACCELERATION;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_ROTATION_VECTOR]        = INV_SENSOR_TYPE_ROTATION_VECTOR;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_HUMIDITY]               = INV_SENSOR_TYPE_HUMIDITY;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_AMBIENT_TEMPERATURE]    = INV_SENSOR_TYPE_AMBIENT_TEMPERATURE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_UNCAL_MAGNETOMETER]     = INV_SENSOR_TYPE_UNCAL_MAGNETOMETER;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_GAME_ROTATION_VECTOR]   = INV_SENSOR_TYPE_GAME_ROTATION_VECTOR;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_UNCAL_GYROSCOPE]        = INV_SENSOR_TYPE_UNCAL_GYROSCOPE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_SMD]                    = INV_SENSOR_TYPE_SMD;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_STEP_DETECTOR]          = INV_SENSOR_TYPE_STEP_DETECTOR;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_STEP_COUNTER]           = INV_SENSOR_TYPE_STEP_COUNTER;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_GEOMAG_ROTATION_VECTOR] = INV_SENSOR_TYPE_GEOMAG_ROTATION_VECTOR;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_HEART_RATE]             = INV_SENSOR_TYPE_HEART_RATE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_TILT_DETECTOR]          = INV_SENSOR_TYPE_TILT_DETECTOR;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_WAKE_GESTURE]           = INV_SENSOR_TYPE_WAKE_GESTURE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_GLANCE_GESTURE]         = INV_SENSOR_TYPE_GLANCE_GESTURE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_PICK_UP_GESTURE]        = INV_SENSOR_TYPE_PICK_UP_GESTURE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_ACTIVITY_CLASSIFIER]    = INV_SENSOR_TYPE_BAC;
}

static void build_fifo_protocol_id_map_icm3063x(inv_device_icm30xxx_t * self)
{
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_0]  = INV_SENSOR_TYPE_RAW_PPG;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_4]  = INV_SENSOR_TYPE_HRV;	
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_6]  = INV_SENSOR_TYPE_SLEEP_ANALYSIS;	
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_7]  = INV_SENSOR_TYPE_B2S;
	
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_8]  = INV_SENSOR_TYPE_BAC_EXTENDED;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_9]  = INV_SENSOR_TYPE_BAC_STATISTICS;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_10]  = INV_SENSOR_TYPE_FLOOR_CLIMB_COUNTER;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_11]  = INV_SENSOR_TYPE_ENERGY_EXPENDITURE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_12]  = INV_SENSOR_TYPE_DISTANCE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_13]  = INV_SENSOR_TYPE_SHAKE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_14]  = INV_SENSOR_TYPE_DOUBLE_TAP;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_15]  = INV_SENSOR_TYPE_SEDENTARY_REMIND;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_16]  = INV_SENSOR_TYPE_DATA_ENCRYPTION;
	
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_0] = INV_SENSOR_TYPE_CUSTOM0;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_1] = INV_SENSOR_TYPE_CUSTOM1;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_2] = INV_SENSOR_TYPE_CUSTOM2;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_3] = INV_SENSOR_TYPE_CUSTOM3;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_4] = INV_SENSOR_TYPE_CUSTOM4;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_5] = INV_SENSOR_TYPE_CUSTOM5;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_6] = INV_SENSOR_TYPE_CUSTOM6;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_7] = INV_SENSOR_TYPE_CUSTOM7;
}

static void build_fifo_protocol_id_map_icm30670(inv_device_icm30xxx_t * self)
{
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_0]  = INV_SENSOR_TYPE_TSIMU;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_1]  = INV_SENSOR_TYPE_RAW_ACCELEROMETER;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_2]  = INV_SENSOR_TYPE_RAW_TEMPERATURE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_3]  = INV_SENSOR_TYPE_RAW_MAGNETOMETER;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_4]  = INV_SENSOR_TYPE_EIS;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_5]  = INV_SENSOR_TYPE_RAW_GYROSCOPE;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_6]  = INV_SENSOR_TYPE_OIS;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_7]  = INV_SENSOR_TYPE_B2S;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_EXT_8]  = INV_SENSOR_TYPE_HIGH_RATE_GYRO;


	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_0] = INV_SENSOR_TYPE_CUSTOM0;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_1] = INV_SENSOR_TYPE_CUSTOM1;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_2] = INV_SENSOR_TYPE_CUSTOM2;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_3] = INV_SENSOR_TYPE_CUSTOM3;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_4] = INV_SENSOR_TYPE_CUSTOM4;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_5] = INV_SENSOR_TYPE_CUSTOM5;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_6] = INV_SENSOR_TYPE_CUSTOM6;
	self->map_2sensortype[FIFOPROTOCOL_SENSOR_ID_CUST_7] = INV_SENSOR_TYPE_CUSTOM7;
}

static void build_fifo_protocol_id_map_berlin(inv_device_icm30xxx_t * self)
{
	self->map_2sensortype[FIFOPROTOCOL_ZSP_AUDIO_SENSOR] = INV_SENSOR_TYPE_KWR;
}

/*  Return corresponding fifo protocol id from a sensor type
 */
static int sensor_type_2_fifo_protocol_id(const inv_device_icm30xxx_t * self,
		int type)
{
	if(INV_SENSOR_IS_VALID(type)) {
		unsigned i;
		for(i = 0; i < sizeof(self->map_2sensortype); ++i) {
			if(self->map_2sensortype[i] == INV_SENSOR_ID_TO_TYPE(type)) {
				return (INV_SENSOR_IS_WU(type)) ? i | FIFOPROTOCOL_SENSOR_ID_WAKEUP_FLAG : i;
			}
		}
	}

	return -1;
}

/*  Return corresponding sensor type from a fifo protocol id
 */
static int fifo_protocol_id_2_sensor_type(const inv_device_icm30xxx_t * self,
		uint8_t sensorid)
{
	uint8_t type;
	const inv_bool_t wakeup = !!(sensorid & FIFOPROTOCOL_SENSOR_ID_WAKEUP_FLAG);
	sensorid &= ~FIFOPROTOCOL_SENSOR_ID_WAKEUP_FLAG;

	if(sensorid < FIFOPROTOCOL_SENSOR_ID_CUST_MAX) {
		type = self->map_2sensortype[sensorid];

		if(type != UINT8_MAX) {
			if(wakeup) {
				return (int)type | INV_SENSOR_TYPE_WU_FLAG;
			} else {
				return type;
			}
		}
	}

	return -1;
}

static inv_bool_t build_data_icm3063x(uint8_t sensorid, uint8_t accuracy,
		const uint8_t * data, uint16_t len, inv_sensor_event_t * event)
{
	switch(sensorid) {
	case FIFOPROTOCOL_SENSOR_ID_EXT_0: // Raw PPG
		memcpy(&(event->data.rawppg.ppg_value), &data[0], sizeof(event->data.rawppg.ppg_value));
		memcpy(&(event->data.rawppg.touch_status), &data[4], sizeof(event->data.rawppg.touch_status));
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_4: // HRV
		memcpy(&(event->data.hrv.rr_count), &data[0], sizeof(event->data.hrv.rr_count));
		memcpy(&(event->data.hrv.rr_interval), &data[2], sizeof(event->data.hrv.rr_interval));
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_6: // Sleep Analysis
		memcpy(&(event->data.sleepanalysis.sleep_phase), &data[0], sizeof(event->data.sleepanalysis.sleep_phase));
		memcpy(&(event->data.sleepanalysis.timestamp), &data[4], sizeof(event->data.sleepanalysis.timestamp));
		memcpy(&(event->data.sleepanalysis.sleep_onset), &data[8], sizeof(event->data.sleepanalysis.sleep_onset));
		memcpy(&(event->data.sleepanalysis.sleep_latency), &data[12], sizeof(event->data.sleepanalysis.sleep_latency));
		memcpy(&(event->data.sleepanalysis.time_in_bed), &data[16], sizeof(event->data.sleepanalysis.time_in_bed));
		memcpy(&(event->data.sleepanalysis.total_sleep_time), &data[20], sizeof(event->data.sleepanalysis.total_sleep_time));
		memcpy(&(event->data.sleepanalysis.sleep_efficiency), &data[24], sizeof(event->data.sleepanalysis.sleep_efficiency));
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_7: // B2S
		event->data.event = true;
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_8: // BAC EXTENDED
		event->data.bacext.event = (data[0] & 0x80) ? -(data[0] & 0x7f) : data[0];
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_9: // BAC STATISTICS
		memcpy(&(event->data.bacstat.durationWalk), &data[0], sizeof(event->data.bacstat.durationWalk));
		memcpy(&(event->data.bacstat.durationRun), &data[4], sizeof(event->data.bacstat.durationRun));
		memcpy(&(event->data.bacstat.durationTransportSit), &data[8], sizeof(event->data.bacstat.durationTransportSit));
		memcpy(&(event->data.bacstat.durationTransportStand), &data[12], sizeof(event->data.bacstat.durationTransportStand));
		memcpy(&(event->data.bacstat.durationBiking), &data[16], sizeof(event->data.bacstat.durationBiking));
		memcpy(&(event->data.bacstat.durationStillSit), &data[20], sizeof(event->data.bacstat.durationStillSit));
		memcpy(&(event->data.bacstat.durationStillStand), &data[24], sizeof(event->data.bacstat.durationStillStand));
		memcpy(&(event->data.bacstat.durationTotalSit), &data[28], sizeof(event->data.bacstat.durationTotalSit));
		memcpy(&(event->data.bacstat.durationTotalStand), &data[32], sizeof(event->data.bacstat.durationTotalStand));
		memcpy(&(event->data.bacstat.stepWalk), &data[36], sizeof(event->data.bacstat.stepWalk));
		memcpy(&(event->data.bacstat.stepRun), &data[40], sizeof(event->data.bacstat.stepRun));
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_10: // FLOOR_CLIMB_COUNTER
		memcpy(&(event->data.floorclimb.floorsUp), &data[0], sizeof(event->data.floorclimb.floorsUp));
		memcpy(&(event->data.floorclimb.floorsDown), &data[4], sizeof(event->data.floorclimb.floorsDown));
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_11: // ENERGY_EXPENDITURE
		memcpy(&(event->data.energyexp.instantEEkcal), &data[0], sizeof(event->data.energyexp.instantEEkcal));
		memcpy(&(event->data.energyexp.instantEEmets), &data[4], sizeof(event->data.energyexp.instantEEmets));
		memcpy(&(event->data.energyexp.cumulativeEEkcal), &data[8], sizeof(event->data.energyexp.cumulativeEEkcal));
		memcpy(&(event->data.energyexp.cumulativeEEmets), &data[12], sizeof(event->data.energyexp.cumulativeEEmets));
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_12: // DISTANCE
		memcpy(&(event->data.distance.distanceWalk), &data[0], sizeof(event->data.distance.distanceWalk));
		memcpy(&(event->data.distance.distanceRun), &data[4], sizeof(event->data.distance.distanceRun));		
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_13: // SHAKE
		event->data.event = true;
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_14: // DOUBLE TAP
		event->data.event = true;
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_15: // SEDENTARY REMINDER
		event->data.event = true;
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_16: // DATA ENCRYPTION
		memcpy(&(event->data.dataencryption.table), &data[0], sizeof(event->data.dataencryption.table));
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_1:
	case FIFOPROTOCOL_SENSOR_ID_EXT_2:
	case FIFOPROTOCOL_SENSOR_ID_EXT_3:
	case FIFOPROTOCOL_SENSOR_ID_EXT_5:
	case FIFOPROTOCOL_SENSOR_ID_CUST_0:
	case FIFOPROTOCOL_SENSOR_ID_CUST_1:
	case FIFOPROTOCOL_SENSOR_ID_CUST_2:
	case FIFOPROTOCOL_SENSOR_ID_CUST_3:
	case FIFOPROTOCOL_SENSOR_ID_CUST_4:
	case FIFOPROTOCOL_SENSOR_ID_CUST_5:
	case FIFOPROTOCOL_SENSOR_ID_CUST_6:
	case FIFOPROTOCOL_SENSOR_ID_CUST_7:
		if (len > sizeof(event->data.reserved))
			memcpy(event->data.reserved, data, sizeof(event->data.reserved));
		else
			memcpy(event->data.reserved, data, len);
		break;
	default:
		return false;
	}

	(void)accuracy;

	return true;
}

static inv_bool_t build_data_icm30670(uint8_t sensorid, uint8_t accuracy,
		const uint8_t * data, uint16_t len, inv_sensor_event_t * event)
{
	switch(sensorid) {
	case FIFOPROTOCOL_SENSOR_ID_EXT_0: // TS-IMU sensor (status reported on GetLastData event)
			memcpy(&(event->data.tsimu_status), data, sizeof(event->data.tsimu_status));
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_5: // Raw Gyr
	case FIFOPROTOCOL_SENSOR_ID_EXT_1: // Raw Acc
	case FIFOPROTOCOL_SENSOR_ID_EXT_3: // Raw Mag
	case FIFOPROTOCOL_SENSOR_ID_EXT_8: // High Rate Raw Gyr
		inv_fifo_protocol_helper_decode_raw_data(data, len, &event->data.raw3d.vect[0],
			&event->data.raw3d.vect[1], &event->data.raw3d.vect[2]);
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_2: // Raw Temp
		memcpy(&(event->data.rawtemp.raw), data, sizeof(event->data.rawtemp.raw));
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_7: // B2S
		event->data.event = true;
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_4: // EIS
		inv_fifo_protocol_helper_decode_gyr_data(data, len, &event->data.eis.vect[0],
			&event->data.gyr.vect[1], &event->data.gyr.vect[2]);
		inv_fifo_protocol_helper_decode_gyr_bias_data(data, len, &event->data.eis.bias[0],
			&event->data.gyr.bias[1], &event->data.gyr.bias[2]);
		inv_fifo_protocol_helper_decode_eis_gyr_deltats_data(data, len,
				&event->data.eis.delta_ts);
		break;
	case FIFOPROTOCOL_SENSOR_ID_EXT_6: // OIS
		/* nothing to decode */
		break;

	case FIFOPROTOCOL_SENSOR_ID_CUST_0:
	case FIFOPROTOCOL_SENSOR_ID_CUST_1:
	case FIFOPROTOCOL_SENSOR_ID_CUST_2:
	case FIFOPROTOCOL_SENSOR_ID_CUST_3:
	case FIFOPROTOCOL_SENSOR_ID_CUST_4:
	case FIFOPROTOCOL_SENSOR_ID_CUST_5:
	case FIFOPROTOCOL_SENSOR_ID_CUST_6:
	case FIFOPROTOCOL_SENSOR_ID_CUST_7:
		if (len > sizeof(event->data.reserved))
			memcpy(event->data.reserved, data, sizeof(event->data.reserved));
		else
			memcpy(event->data.reserved, data, len);
		break;
	default:
		return false;
	}

	(void)accuracy;

	return true;
}

static inv_bool_t build_data_berlin(uint8_t sensorid, uint8_t accuracy,
		const uint8_t * data, uint16_t len, inv_sensor_event_t * event)
{
	(void)data, (void)len;

	switch(sensorid) {
	case FIFOPROTOCOL_ZSP_AUDIO_SENSOR:
	{
		struct inv_icm30xxx_audio_buffer * audio_buffer = (struct inv_icm30xxx_audio_buffer *)data;

		assert(len >= sizeof(struct inv_icm30xxx_audio_buffer));

		event->data.audio_buffer.buffer = audio_buffer->buffer;
		event->data.audio_buffer.size   = audio_buffer->cur_size;
		break;
	}
	default:
		return false;
	}

	(void)accuracy;

	return true;
}

static inv_bool_t build_data_common(void * context, uint8_t sensorid, uint8_t accuracy,
		const uint8_t * data, uint16_t len, inv_sensor_event_t * event)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;
	
	switch(sensorid) {
	case FIFOPROTOCOL_SENSOR_ID_META_DATA:
		break;

	case FIFOPROTOCOL_SENSOR_ID_ACCELEROMETER:
	case FIFOPROTOCOL_SENSOR_ID_GRAVITY:
	case FIFOPROTOCOL_SENSOR_ID_LINEAR_ACCELERATION:
		event->data.acc.accuracy_flag = accuracy;
		inv_fifo_protocol_helper_decode_acc_data(data, len, &event->data.acc.vect[0],
				&event->data.acc.vect[1], &event->data.acc.vect[2]);
		break;

	case FIFOPROTOCOL_SENSOR_ID_MAGNETOMETER:
		event->data.mag.accuracy_flag = accuracy;
		event->data.mag.bias[0] = 0;
		event->data.mag.bias[1] = 0;
		event->data.mag.bias[2] = 0;
		inv_fifo_protocol_helper_decode_mag_data(data, len, &event->data.mag.vect[0],
				&event->data.mag.vect[1], &event->data.mag.vect[2]);
		break;

	case FIFOPROTOCOL_SENSOR_ID_UNCAL_MAGNETOMETER:
		event->data.mag.accuracy_flag = accuracy;
		inv_fifo_protocol_helper_decode_mag_data(data, len, &event->data.mag.vect[0],
				&event->data.mag.vect[1], &event->data.mag.vect[2]);
		inv_fifo_protocol_helper_decode_mag_bias_data(data, len, &event->data.mag.bias[0],
				&event->data.mag.bias[1], &event->data.mag.bias[2]);
		break;

	case FIFOPROTOCOL_SENSOR_ID_GYROSCOPE:
		event->data.gyr.accuracy_flag = accuracy;
		event->data.gyr.bias[0] = 0;
		event->data.gyr.bias[1] = 0;
		event->data.gyr.bias[2] = 0;
		inv_fifo_protocol_helper_decode_gyr_data(data, len, &event->data.gyr.vect[0],
				&event->data.gyr.vect[1], &event->data.gyr.vect[2]);
		break;

	case FIFOPROTOCOL_SENSOR_ID_UNCAL_GYROSCOPE:
		event->data.gyr.accuracy_flag = accuracy;
		inv_fifo_protocol_helper_decode_gyr_data(data, len, &event->data.gyr.vect[0],
				&event->data.gyr.vect[1], &event->data.gyr.vect[2]);
		inv_fifo_protocol_helper_decode_gyr_bias_data(data, len, &event->data.gyr.bias[0],
				&event->data.gyr.bias[1], &event->data.gyr.bias[2]);
		break;

	case FIFOPROTOCOL_SENSOR_ID_ORIENTATION:
		event->data.orientation.accuracy_flag = accuracy;
		inv_fifo_protocol_helper_decode_orientation_data(data, len,
				&event->data.orientation.x, &event->data.orientation.y,
				&event->data.orientation.z);
		break;

	case FIFOPROTOCOL_SENSOR_ID_ROTATION_VECTOR:
	case FIFOPROTOCOL_SENSOR_ID_GEOMAG_ROTATION_VECTOR:
	case FIFOPROTOCOL_SENSOR_ID_GAME_ROTATION_VECTOR:
		event->data.quaternion.accuracy = 0;
		inv_fifo_protocol_helper_decode_q30_quat(data, len, &event->data.quaternion.quat[1],
			&event->data.quaternion.quat[2], &event->data.quaternion.quat[3],
			&event->data.quaternion.quat[0], &event->data.quaternion.accuracy);
		break;

	case FIFOPROTOCOL_SENSOR_ID_STEP_COUNTER:
		event->data.step.count  = (uint32_t)(data[0]);
		event->data.step.count |= (uint32_t)(data[1] << 8U);
		event->data.step.count |= (uint32_t)(data[2] << 16U);
		event->data.step.count |= (uint32_t)(data[3] << 24U);
		break;

	case FIFOPROTOCOL_SENSOR_ID_HEART_RATE:
		inv_fifo_protocol_helper_decode_heart_rate_data(data, len,
			&event->data.hrm.bpm);
		if(self->icm30xxx_states.variant == INV_ICM30XXX_VARIANT_ICM3063x) {
			/* currently, with latest ICM30631 FW, heart rate data encoding is different
			   compare to ICM30630 */
			event->data.hrm.confidence = data[2];
			event->data.hrm.sqi = data[3];
		}
		break;

	case FIFOPROTOCOL_SENSOR_ID_LIGHT:
		inv_fifo_protocol_helper_decode_light_data(data, len,
				&event->data.light.level);
		break;

	case FIFOPROTOCOL_SENSOR_ID_PRESSURE:
		inv_fifo_protocol_helper_decode_pressure_data(data, len,
			&event->data.pressure.pressure);
		break;

	case FIFOPROTOCOL_SENSOR_ID_TEMPERATURE:
	case FIFOPROTOCOL_SENSOR_ID_AMBIENT_TEMPERATURE:
		inv_fifo_protocol_helper_decode_ambient_temperature_data(data, len,
			&event->data.temperature.tmp);
		break;

	case FIFOPROTOCOL_SENSOR_ID_PROXIMITY:
		event->data.proximity.distance  = data[0];
		event->data.proximity.distance |= data[1] << 8;
		break;

	case FIFOPROTOCOL_SENSOR_ID_SMD:
	case FIFOPROTOCOL_SENSOR_ID_STEP_DETECTOR:
	case FIFOPROTOCOL_SENSOR_ID_TILT_DETECTOR:
	case FIFOPROTOCOL_SENSOR_ID_WAKE_GESTURE:
	case FIFOPROTOCOL_SENSOR_ID_GLANCE_GESTURE:
	case FIFOPROTOCOL_SENSOR_ID_PICK_UP_GESTURE:
		event->data.event = true;
		break;

	case FIFOPROTOCOL_SENSOR_ID_ACTIVITY_CLASSIFIER:
		event->data.bac.event = (data[0] & 0x7f);
		if(data[0] & 0x80)
			event->data.bac.event = -event->data.bac.event;
		break;

	case FIFOPROTOCOL_SENSOR_ID_HUMIDITY:
		inv_fifo_protocol_helper_decode_relative_humidity_data(data, len,
				&event->data.humidity.percent);
		break;

	default:
		return false;
	}

	return true;
}

static inv_bool_t build_sensor_event(void * context, uint8_t sensorid, uint32_t timestamp,
		uint8_t status, uint8_t accuracy, const uint8_t * data, uint16_t len,
		inv_sensor_event_t * event)
{
	inv_device_icm30xxx_t * self = (inv_device_icm30xxx_t *)context;

	inv_bool_t wakeup = !!(sensorid & FIFOPROTOCOL_SENSOR_ID_WAKEUP_FLAG);

	sensorid &= ~FIFOPROTOCOL_SENSOR_ID_WAKEUP_FLAG;

	assert(event);

	memset(event, 0, sizeof(*event));

	if(status == FIFOPROTOCOL_STATUS_DATAUPDATED || status == FIFOPROTOCOL_STATUS_POLL) {
		inv_bool_t decoded = false;

		/* handle specific sensors of ICM30630 vs ICM30670 */
		if(self->build_data) {
			decoded = self->build_data(sensorid, accuracy, data, len, event);
		}

		if(!decoded && !build_data_common(context, sensorid, accuracy, data, len, event)) {
			/* unexpected sensor id */
			INV_MSG(INV_MSG_LEVEL_WARNING,
					"Unexpected data frame received for sensor id %d. Ignored.",
					sensorid);
			return false;
		}
	}

	/* finish up building event */
	event->sensor    = fifo_protocol_id_2_sensor_type(self, sensorid);

	event->timestamp = timestamp;
	event->timestamp *= self->clock_rate; /* to ns */
	event->timestamp /= 1000;             /* to us */

	event->status    = status;
	if(wakeup)
		event->sensor |= INV_SENSOR_TYPE_WU_FLAG;

	return true;
}
