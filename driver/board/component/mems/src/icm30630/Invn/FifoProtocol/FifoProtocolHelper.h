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

/** @defgroup 	FifoProtocolHelper	FifoProtocolHelper
 	@brief 		Helper function to code/decode command parameters or data
	@ingroup 	FifoProtocolAdapter
    @{
*/

#ifndef _INV_FIFO_PROTOCOL_HELPER_H_
#define _INV_FIFO_PROTOCOL_HELPER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "FifoProtocolUtils.h"

#include <stdint.h>
#include <string.h>


#define FIFOPROTOCOL_QUATERNION_ACCURACY_PRECISION				6		// hq6
#define FIFOPROTOCOL_QUATERNION_PRECISION						30		// q30
#define FIFOPROTOCOL_ACCELEROMETER_PRECISION					11		// hq11
#define FIFOPROTOCOL_MAGNETOMETER_PRECISION						4		// hq4
#define FIFOPROTOCOL_GYROSCOPE_PRECISION						4		// hq4
#define FIFOPROTOCOL_ORIENTATION_PRECISION						6		// hq6
#define FIFOPROTOCOL_RELATIVE_HUMIDITY_PRECISION				9		// hq9
#define FIFOPROTOCOL_AMBIENT_TEMPERATURE_PRECISION				8		// hq8
#define FIFOPROTOCOL_HEART_RATE_PRECISION						7		// hq7
#define FIFOPROTOCOL_CALIBRATION_GAINS_PRECISION				30		// q30
#define FIFOPROTOCOL_CALIBRATION_OFFSETS_PRECISION				16		// q16
#define FIFOPROTOCOL_REFERENCE_FRAME_ROTATION_MATRIX_PRECISION	30		// q30



/** @brief 		Decode ms delay from FIFOPROTOCOL_CMD_SETDELAY arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	in 	encoded data
	@param[in]	len length of encoded data
	@return 	delay in ms
*/
static inline uint32_t inv_fifo_protocol_helper_decode_delay(const void * in, uint16_t len)
{
	uint32_t delay;

	FIFOPROTOCOL_MEMCPY(&delay, in, sizeof(delay));

	(void)len;

	return FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(delay);
}

/** @brief 		Encode ms delay for FIFOPROTOCOL_CMD_SETDELAY arguments
	@warning 	This function assumes target architecture is little endian.
	@warning 	This function assumes target architecture is little endian.
	@param[in]	timeout timeout to encode
	@param[out]	out 	data buffer
	@return 	length of encoded data
*/
static inline uint16_t inv_fifo_protocol_helper_encode_delay(uint32_t delay, uint8_t * out)
{
	const uint32_t outDelay = FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(delay);

	FIFOPROTOCOL_MEMCPY(out, &outDelay, sizeof(outDelay));

	return 4;
}

/** @brief 		Decode ms timeout from FIFOPROTOCOL_CMD_BATCHON arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	in 	encoded data
	@param[in]	len length of encoded data
	@return 	timeout in ms
*/
static inline uint32_t inv_fifo_protocol_helper_decode_timeout(const void * in, uint16_t len)
{
	uint32_t timeout;

	FIFOPROTOCOL_MEMCPY(&timeout, in, sizeof(timeout));

	(void)len;

	return FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(timeout);
}

/** @brief 		Encode ms timeout for FIFOPROTOCOL_CMD_BATCHON arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	timeout timeout to encode
	@param[out]	out 	data buffer
	@return 	length of encoded data
*/
static inline uint16_t inv_fifo_protocol_helper_encode_timeout(uint32_t timeout, uint8_t * out)
{
	const uint32_t outTimeout = FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(timeout);

	FIFOPROTOCOL_MEMCPY(out, &outTimeout, sizeof(outTimeout));

	return 4;
}

/** @brief 		Decode AP power state value from FIFOPROTOCOL_CMD_SETPOWER arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	in 	encoded data
	@param[in]	len length of encoded data
	@return 	power state value
*/
static inline FifoProtocolApPowerMode inv_fifo_protocol_helper_decode_power(
        const void *in, uint16_t len)
{
	uint16_t power;

	FIFOPROTOCOL_MEMCPY(&power, in, sizeof(power));

	(void)len;

	return (FifoProtocolApPowerMode)FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(power);
}

/** @brief 		Encode AP power state value for FIFOPROTOCOL_CMD_SETPOWER arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	ps 		power state value to encode
	@param[out]	out 	data buffer
	@return 	length of encoded data
*/
static inline uint16_t inv_fifo_protocol_helper_encode_power(
        FifoProtocolApPowerMode ps, uint8_t *out)
{
	const uint16_t outPower = FIFOPROTOCOL_TO_LITTLE_ENDIAN_16((uint16_t)ps);

	FIFOPROTOCOL_MEMCPY(out, &outPower, sizeof(outPower));

	return 2;
}

/** @brief 		Encode the data for FIFOPROTOCOL_CMD_GETCLOCKRATE arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	rate 	clock rate
	@param[out]	out 	data buffer
	@return 	length of encoded data
*/
static inline uint16_t inv_fifo_protocol_helper_encode_clockrate(uint32_t rate,
                uint8_t *out)
{
	const uint32_t outRate = FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(rate);

	FIFOPROTOCOL_MEMCPY(out, &outRate, sizeof(outRate));

	return 4;
}

/** @brief 		Decode clock rate value from FIFOPROTOCOL_CMD_GETCLOCKRATE arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	in 	encoded data
	@param[in]	len length of encoded data
	@return 	clock rate
*/
static inline uint32_t inv_fifo_protocol_helper_decode_clockrate(const void *in,
                uint16_t len)
{
	uint32_t rate;

	FIFOPROTOCOL_MEMCPY(&rate, in, sizeof(rate));

	(void)len;

	return FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(rate);
}


/** @brief 		Encode the data for FIFOPROTOCOL_CMD_PING arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	supported 	sensor is supported (0: no, other: yes)
	@param[out]	out 		data buffer
	@return 	length of encoded data
*/
static inline uint16_t inv_fifo_protocol_helper_encode_ping(uint8_t supported,
                uint8_t *out)
{
	out[0] =  supported;

	return 1;
}

/** @brief 		Decode the data from FIFOPROTOCOL_CMD_PING arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	in 	encoded data
	@param[in]	len length of encoded data
	@return 	sensor is supported (0: no, other: yes)
*/
static inline uint8_t inv_fifo_protocol_helper_decode_ping(const void *in,
                uint16_t len)
{
	const uint8_t tmp = *((const uint8_t *) in);

	(void)len;

	return tmp;
}

/** @brief 		Encode the data for FIFOPROTOCOL_CMD_FIRMWARE arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	major 		major version of the FW in binary format
	@param[in]	minor 		minor version of the FW in binary format
	@param[in]	patch 		patch version of the FW in binary format
	@param[in]	hash 		git commit hash in ASCII format
	@param[in]	checksum 	checksum of the generated and loaded binary in CRC32
	@param[out]	out 		data buffer
	@return 	length of encoded data
*/
static inline uint16_t inv_fifo_protocol_helper_encode_firmware(
        uint8_t 			major,
        uint8_t 			minor,
        uint8_t 			patch,
        unsigned char 	*hash,
        uint32_t 			checksum,
        uint8_t 			*out
)
{
	// version
	out[0] = major;
	out[1] = minor;
	out[2] = patch;

	// hash
	FIFOPROTOCOL_MEMCPY(&out[3], hash, 8);

	// checksum
	checksum = FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(checksum);
	FIFOPROTOCOL_MEMCPY(&out[11], &checksum, sizeof(checksum));

	return 15;
}

/** @brief 		Decode the data from FIFOPROTOCOL_CMD_FIRMWARE arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	in 	encoded data
	@param[in]	len length of encoded data
	@return 	clock rate
*/
static inline void inv_fifo_protocol_helper_decode_firmware(
        const void 		*in,
        uint16_t 			len,
        uint8_t   			*major,
        uint8_t 			*minor,
        uint8_t 			*patch,
        unsigned char 	*hash,
        uint32_t 			*checksum
)
{
	const uint8_t *buf = (const uint8_t *) in;

	// version
	*major = buf[0];
	*minor = buf[1];
	*patch = buf[2];

	// hash
	FIFOPROTOCOL_MEMCPY(hash, &buf[3], 8);

	// checksum
	FIFOPROTOCOL_MEMCPY(checksum, &buf[11], sizeof(*checksum));
	*checksum = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(*checksum);

	(void)len;
}

/** @brief      Decode the data for FIFOPROTOCOL_CMD_LOAD arguments
	@warning    This function assumes target architecture is little endian.
	@param[in]  data    FIFO data
	@param[in]  len 	length of FIFO data
	@param[out] who
	@param[out] what
	@param[out] inarg
	@return     length of encoded data
*/
static inline uint16_t inv_fifoprotocol_helper_decode_load(
        const void *in,
        uint16_t     len,
        uint8_t     *who,
        uint8_t     *what,
        uint32_t    *arg
)
{
	const uint8_t *buf = (const uint8_t *) in;
	uint32_t  myInArg;

	(void) len;

	*who = buf[0];
	*what = buf[1];

	FIFOPROTOCOL_MEMCPY(&myInArg, &buf[2], sizeof(myInArg));

	*arg = FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(myInArg);

	return 6;
}


/** @brief 		Encode the data for FIFOPROTOCOL_CMD_LOAD answer
	@warning 	This function assumes target architecture is little endian.
	@param[in]	who
	@param[in]	what
	@param[in]	response
	@param[out]	encoded data
	@return 	length of encoded data
*/
static inline uint16_t inv_fifoprotocol_helper_encode_load(
        uint8_t    who,
        uint8_t    what,
        uint32_t   response,
        uint8_t   *out
)
{
	out[0] = who;
	out[1] = what;

	response = FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(response);
	FIFOPROTOCOL_MEMCPY(&out[2], &response, sizeof(response));

	return 6;
}


/** @brief 		Encode the data for FIFOPROTOCOL_CMD_SETREFERENCEFRAME arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	rotation_matrix 		mounting matrix
	@param[out]	out 		data buffer
	@return 	length of encoded data
*/
static inline uint16_t inv_fifo_protocol_helper_encode_referenceframe(
        float 		rotation_matrix[9],
        uint8_t 	*out
)
{
	int i;

	// rotation matrix
	for (i = 0; i < 9; i++) {
		int32_t elt;

		elt = FIFOPROTOCOL_FLT_TO_SFIX(rotation_matrix[i],
		                               FIFOPROTOCOL_REFERENCE_FRAME_ROTATION_MATRIX_PRECISION);
		elt = FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(elt);

		FIFOPROTOCOL_MEMCPY(&out[sizeof(int32_t)*i], &elt, sizeof(elt));
	}

	return 36;
}

/** @brief 		Decode the data from FIFOPROTOCOL_CMD_SETREFERENCEFRAME arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	in 			encoded data
	@param[in]	len 		length of encoded data
	@return 	clock rate
*/
static inline void inv_fifo_protocol_helper_decode_referenceframe(
        const void 	*in,
        uint16_t 		len,
        float 			*rotation_matrix
)
{
	const uint8_t *buf = (const uint8_t *) in;
	int i;

	// rotation matrix
	for (i = 0; i < 9; i++) {
		int32_t elt;

		FIFOPROTOCOL_MEMCPY(&elt, &buf[sizeof(int32_t)*i], sizeof(int32_t));

		elt = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(elt);

		rotation_matrix[i] = FIFOPROTOCOL_SFIX_TO_FLT(elt,
		                     FIFOPROTOCOL_REFERENCE_FRAME_ROTATION_MATRIX_PRECISION);
	}

	(void)len;
}

/** @brief 		Encode the data for FIFOPROTOCOL_CMD_SETCALIBRATIONOFFSETS arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	data 		sensor offset vector[3]
	@param[in]	unsused 	unsued argument, should be removed
	@param[out]	out 		data buffer
	@return 	length of encoded data
*/
static inline uint16_t inv_fifo_protocol_helper_encode_calibrationoffsets(
        float 	*data,
        uint16_t 	unsused,
        uint8_t 	*out
)
{
	int i;

	for (i = 0; i < 3; ++i) {
		int32_t elt;

		elt = FIFOPROTOCOL_FLT_TO_SFIX(data[i],
		                               FIFOPROTOCOL_CALIBRATION_OFFSETS_PRECISION);
		elt = FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(elt);

		FIFOPROTOCOL_MEMCPY(&out[sizeof(int32_t)*i], &elt, sizeof(elt));
	}

	(void)unsused;

	return 12;
}

/** @brief 		Decode the data from FIFOPROTOCOL_CMD_SETCALIBRATIONOFFSETS arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	in 	encoded data
	@param[in]	len length of encoded data
	@param[out]	data 	offset vector[3]
	@return 	always 0
*/
static inline uint16_t inv_fifo_protocol_helper_decode_calibrationoffsets(
        const void 	*in,
        uint16_t 		len,
        float 		*data
)
{
	const uint8_t *buf = (const uint8_t *) in;
	int i;

	for (i = 0; i < 3; i++) {
		int32_t elt;

		FIFOPROTOCOL_MEMCPY(&elt, &buf[sizeof(int32_t)*i], sizeof(int32_t));

		elt = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(elt);

		data[i] = FIFOPROTOCOL_SFIX_TO_FLT(elt,
		                                   FIFOPROTOCOL_CALIBRATION_OFFSETS_PRECISION);
	}

	(void)len;

	return 0;
}

/** @brief 		Encode the data for FIFOPROTOCOL_CMD_SETCALIBRATIONGAINS arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	data 		3x3 matrix to be encoded
	@param[in]	unsused 	unsused argument (should be removed)
	@param[out]	out 		data buffer
	@return 	length of encoded data
*/
static inline uint16_t inv_fifo_protocol_helper_encode_calibrationgains(
        float 	*data,
        uint16_t 	len,
        uint8_t 	*out
)
{
	int i;

	for (i = 0; i < 9; i++) {
		int32_t elt;

		elt = FIFOPROTOCOL_FLT_TO_SFIX(data[i],
		                               FIFOPROTOCOL_CALIBRATION_GAINS_PRECISION);
		elt = FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(elt);

		FIFOPROTOCOL_MEMCPY(&out[sizeof(int32_t)*i], &elt, sizeof(elt));
	}

	(void)len;

	return 36;
}

/** @brief 		Decode calibration matrix from FIFOPROTOCOL_CMD_GETCALIBRATIONGAINS arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	in 	encoded data
	@param[in]	len length of encoded data
	@param[out]	data 	calibration matrix
	@return 	always 0
*/
static inline uint16_t inv_fifo_protocol_helper_decode_calibrationgains(
        const void 	*in,
        uint16_t 		len,
        float 		*data
)
{
	const uint8_t *buf = (const uint8_t *) in;
	int i;

	for (i = 0; i < 9; i++) {
		int32_t elt;

		FIFOPROTOCOL_MEMCPY(&elt, &buf[sizeof(int32_t)*i], sizeof(int32_t));

		elt = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(elt);

		data[i] = FIFOPROTOCOL_SFIX_TO_FLT(elt,
		                                   FIFOPROTOCOL_CALIBRATION_GAINS_PRECISION);
	}

	(void)len;

	return 0;
}

/** @brief 		Encode data injection mode for FIFOPROTOCOL_CMD_SETDATAINJECTION arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	datainjectionmode 0: disable data injection mode; 1: enable data injection mode
	@param[out]	out 	data buffer
	@return 	length of encoded data
*/
static inline uint16_t inv_fifo_protocol_helper_encode_datainjection(uint32_t datainjectionmode, uint8_t * out)
{
	const uint16_t outDatainjectionMode = FIFOPROTOCOL_TO_LITTLE_ENDIAN_16((uint16_t)datainjectionmode);

	FIFOPROTOCOL_MEMCPY(out, &outDatainjectionMode, sizeof(outDatainjectionMode));

	return 2;
}

/** @brief 		Decode data injection mode for FIFOPROTOCOL_CMD_SETDATAINJECTION arguments
	@warning 	This function assumes target architecture is little endian.
	@param[in]	in 	encoded data
	@param[in]	len length of encoded data
	@return 	data injection mode
*/
static inline uint16_t inv_fifo_protocol_helper_decode_datainjection(
        const void *in, uint16_t len)
{
	uint16_t dataInjectionMode;

	FIFOPROTOCOL_MEMCPY(&dataInjectionMode, in, sizeof(dataInjectionMode));

	(void)len;

	return FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(dataInjectionMode);
}

// decode ACC data: expected values are 16 bits, with FIFOPROTOCOL_ACCELEROMETER_PRECISION
static inline void inv_fifo_protocol_helper_decode_acc_data(const void *in,
                uint16_t len, float *x, float *y, float *z)
{
	const uint8_t *buf = (const uint8_t *)in;
	int16_t acc;

	FIFOPROTOCOL_MEMCPY(&acc, &buf[0], sizeof(acc));
	*x = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(acc),
	                                FIFOPROTOCOL_ACCELEROMETER_PRECISION);
	FIFOPROTOCOL_MEMCPY(&acc, &buf[2], sizeof(acc));
	*y = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(acc),
	                                FIFOPROTOCOL_ACCELEROMETER_PRECISION);
	FIFOPROTOCOL_MEMCPY(&acc, &buf[4], sizeof(acc));
	*z = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(acc),
	                                FIFOPROTOCOL_ACCELEROMETER_PRECISION);

	(void) len;
}

// decode GYR data: expected values are 16 bits, with FIFOPROTOCOL_GYROSCOPE_PRECISION
static inline void inv_fifo_protocol_helper_decode_gyr_data(const void *in,
                uint16_t len, float *x, float *y, float *z)
{
	const uint8_t *buf = (const uint8_t *)in;
	int16_t gyr;

	FIFOPROTOCOL_MEMCPY(&gyr, &buf[0], sizeof(gyr));
	*x = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(gyr),
	                                FIFOPROTOCOL_GYROSCOPE_PRECISION);
	FIFOPROTOCOL_MEMCPY(&gyr, &buf[2], sizeof(gyr));
	*y = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(gyr),
	                                FIFOPROTOCOL_GYROSCOPE_PRECISION);
	FIFOPROTOCOL_MEMCPY(&gyr, &buf[4], sizeof(gyr));
	*z = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(gyr),
	                                FIFOPROTOCOL_GYROSCOPE_PRECISION);

	(void) len;
}

// decode GYR bias data: expected values are 16 bits, in 16q4 format
static inline void inv_fifo_protocol_helper_decode_gyr_bias_data(const void *in,
                uint16_t len, float *bx, float *by, float *bz)
{
	const uint8_t *buf = (const uint8_t *)in;
	int16_t gyr;

	FIFOPROTOCOL_MEMCPY(&gyr, &buf[6], sizeof(gyr));
	*bx = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(gyr),
	                                 FIFOPROTOCOL_GYROSCOPE_PRECISION);
	FIFOPROTOCOL_MEMCPY(&gyr, &buf[8], sizeof(gyr));
	*by = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(gyr),
	                                 FIFOPROTOCOL_GYROSCOPE_PRECISION);
	FIFOPROTOCOL_MEMCPY(&gyr, &buf[10], sizeof(gyr));
	*bz = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(gyr),
	                                 FIFOPROTOCOL_GYROSCOPE_PRECISION);

	(void) len;
}

static inline void inv_fifo_protocol_helper_decode_eis_gyr_deltats_data(
		const void * in, uint16_t len, int16_t * delta_ts)
{
	const uint8_t * buf = (const uint8_t *)in;

	FIFOPROTOCOL_MEMCPY(delta_ts, &buf[12], sizeof(int16_t));

	(void) len;
}


// decode MAG data: expected values are 16 bits, in 16q4 format
static inline void inv_fifo_protocol_helper_decode_mag_data(const void *in,
                uint16_t len, float *x, float *y, float *z)
{
	const uint8_t *buf = (const uint8_t *)in;
	int16_t mag;

	FIFOPROTOCOL_MEMCPY(&mag, &buf[0], sizeof(mag));
	*x = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(mag),
	                                FIFOPROTOCOL_MAGNETOMETER_PRECISION);
	FIFOPROTOCOL_MEMCPY(&mag, &buf[2], sizeof(mag));
	*y = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(mag),
	                                FIFOPROTOCOL_MAGNETOMETER_PRECISION);
	FIFOPROTOCOL_MEMCPY(&mag, &buf[4], sizeof(mag));
	*z = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(mag),
	                                FIFOPROTOCOL_MAGNETOMETER_PRECISION);

	(void) len;
}

// decode MAG bias data: expected values are 16 bits, in 16q4 format
static inline void inv_fifo_protocol_helper_decode_mag_bias_data(const void *in,
                uint16_t len, float *bx, float *by, float *bz)
{
	const uint8_t *buf = (const uint8_t *)in;
	int16_t mag;

	FIFOPROTOCOL_MEMCPY(&mag, &buf[6], sizeof(mag));
	*bx = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(mag),
	                                 FIFOPROTOCOL_MAGNETOMETER_PRECISION);
	FIFOPROTOCOL_MEMCPY(&mag, &buf[8], sizeof(mag));
	*by = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(mag),
	                                 FIFOPROTOCOL_MAGNETOMETER_PRECISION);
	FIFOPROTOCOL_MEMCPY(&mag, &buf[10], sizeof(mag));
	*bz = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(mag),
	                                 FIFOPROTOCOL_MAGNETOMETER_PRECISION);

	(void) len;
}

static inline void inv_fifo_protocol_helper_decode_q30_quat(const void *in,
                uint16_t len, float *x, float *y, float *z, float *w, float *a)
{
	const uint8_t *buf = (const uint8_t *)in;
	int32_t quat;
	int16_t prec;

	FIFOPROTOCOL_MEMCPY(&quat, &buf[0], sizeof(quat)-1);
	*x = FIFOPROTOCOL_SFIX_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(quat << 8), FIFOPROTOCOL_QUATERNION_PRECISION);

	FIFOPROTOCOL_MEMCPY(&quat, &buf[3], sizeof(quat)-1);
	*y = FIFOPROTOCOL_SFIX_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(quat << 8), FIFOPROTOCOL_QUATERNION_PRECISION);

	FIFOPROTOCOL_MEMCPY(&quat, &buf[6], sizeof(quat)-1);
	*z = FIFOPROTOCOL_SFIX_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(quat << 8), FIFOPROTOCOL_QUATERNION_PRECISION);

	FIFOPROTOCOL_MEMCPY(&quat, &buf[9], sizeof(quat)-1);
	*w = FIFOPROTOCOL_SFIX_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(quat << 8), FIFOPROTOCOL_QUATERNION_PRECISION);

	if(a)
	{
		FIFOPROTOCOL_MEMCPY(&prec, &buf[12], sizeof(prec));
		*a = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(prec), FIFOPROTOCOL_QUATERNION_ACCURACY_PRECISION);
	}

	(void)len;
}

static inline void inv_fifo_protocol_helper_decode_orientation_data(
        const void  *in, uint16_t len, float *x, float *y, float *z)
{
	const uint8_t *buf = (const uint8_t *)in;
	int16_t ori;

	FIFOPROTOCOL_MEMCPY(&ori, &buf[0], sizeof(ori));
	*x = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(ori),
	                                FIFOPROTOCOL_ORIENTATION_PRECISION);
	FIFOPROTOCOL_MEMCPY(&ori, &buf[2], sizeof(ori));
	*y = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(ori),
	                                FIFOPROTOCOL_ORIENTATION_PRECISION);
	FIFOPROTOCOL_MEMCPY(&ori, &buf[4], sizeof(ori));
	*z = FIFOPROTOCOL_SFIX16_TO_FLT(FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(ori),
	                                FIFOPROTOCOL_ORIENTATION_PRECISION);

	(void) len;
}

static inline void inv_fifo_protocol_helper_decode_grv_data(const void *in,
                uint16_t len, float *x, float *y, float *z, float *w)
{
	inv_fifo_protocol_helper_decode_q30_quat(in, len, x, y, z, w, 0);
}

static inline void inv_fifo_protocol_helper_decode_rv_data(const void *in,
                uint16_t len, float *x, float *y, float *z, float *w, float *a)
{
	inv_fifo_protocol_helper_decode_q30_quat(in, len, x, y, z, w, a);
}

static inline void inv_fifo_protocol_helper_decode_georv_data(const void *in,
                uint16_t len, float *x, float *y, float *z, float *w, float *a)
{
	inv_fifo_protocol_helper_decode_q30_quat(in, len, x, y, z, w, a);
}


// Light is in Lux
static inline void inv_fifo_protocol_helper_decode_light_data(const void *in,
                uint16_t len, uint32_t *light)
{
	FIFOPROTOCOL_MEMCPY(light, in, sizeof(uint32_t));
	*light = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(*light);

	(void) len;
}

// Pressure is in Pa
static inline void inv_fifo_protocol_helper_decode_pressure_data(const void *in,
                uint16_t len, uint32_t *pressure)
{
	FIFOPROTOCOL_MEMCPY(pressure, in, sizeof(uint32_t));
	*pressure = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(*pressure);

	(void) len;
}

// Distance is in mm
static inline void inv_fifo_protocol_helper_decode_proximity_data(
        const void *in, uint16_t len, uint16_t *distance)
{
	FIFOPROTOCOL_MEMCPY(distance, in, sizeof(uint16_t));
	*distance = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(*distance);

	(void) len;
}

// decode GRAVITY data: expected values are 16 bits, with FIFOPROTOCOL_ACCELEROMETER_PRECISION
static inline void inv_fifo_protocol_helper_decode_gravity_data(const void *in,
                uint16_t len, float *x, float *y, float *z)
{
	inv_fifo_protocol_helper_decode_acc_data(in, len, x, y, z);
}

// decode LINEAR_ACC data: expected values are 16 bits, with FIFOPROTOCOL_ACCELEROMETER_PRECISION
static inline void inv_fifo_protocol_helper_decode_linear_acc_data(
        const void *in, uint16_t len, float *x, float *y, float *z)
{
	inv_fifo_protocol_helper_decode_acc_data(in, len, x, y, z);
}

// decode RELATIVE_HUMIDITY data: expected values are 16 bits, with FIFOPROTOCOL_RELATIVE_HUMIDITY_PRECISION, in %
static inline void inv_fifo_protocol_helper_decode_relative_humidity_data(
        const void *in, uint16_t len, float *relative_humidity)
{
	uint16_t tmp;

	FIFOPROTOCOL_MEMCPY(&tmp, in, sizeof(tmp));
	tmp = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(tmp);
	*relative_humidity = FIFOPROTOCOL_UFIX16_TO_FLT(tmp,
	                     FIFOPROTOCOL_RELATIVE_HUMIDITY_PRECISION);

	(void) len;
}

// decode AMBIENT_TEMPERATURE data: expected values are 16 bits, with FIFOPROTOCOL_AMBIENT_TEMPERATURE_PRECISION, in %
static inline void inv_fifo_protocol_helper_decode_ambient_temperature_data(
        const void *in, uint16_t len, float *ambient_temperature)
{
	uint16_t tmp;

	FIFOPROTOCOL_MEMCPY(&tmp, in, sizeof(tmp));
	tmp = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(tmp);
	*ambient_temperature = FIFOPROTOCOL_UFIX16_TO_FLT(tmp,
	                       FIFOPROTOCOL_AMBIENT_TEMPERATURE_PRECISION);

	(void) len;
}

// decode step counter, Q0
static inline void inv_fifo_protocol_helper_decode_step_counter_data(
        const void  *in, uint16_t len, uint32_t *step_counter)
{
	FIFOPROTOCOL_MEMCPY(step_counter, in, sizeof(uint32_t));
	*step_counter = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(*step_counter);

	(void) len;
}

// decode HEART_RATE data: expected values are 16 bits, with FIFOPROTOCOL_HEART_RATE_PRECISION, in bpm
static inline void inv_fifo_protocol_helper_decode_heart_rate_data(
        const void *in, uint16_t len, float *bpm)
{
	uint16_t tmp;

	FIFOPROTOCOL_MEMCPY(&tmp, in, sizeof(tmp));
	tmp = FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(tmp);
	*bpm = FIFOPROTOCOL_UFIX16_TO_FLT(tmp, FIFOPROTOCOL_HEART_RATE_PRECISION);

	(void)len;
}

// decode AAC data:
// event = 0 -> activity starts
// event = 1 -> activity ends
static inline void inv_fifo_protocol_helper_decode_aac_data(const void *in,
                uint16_t len, uint8_t *activity, uint8_t *event)
{
	const uint8_t data = *(const uint8_t *)in;

	*event 		= !!(data & 0x80);
	*activity 	= (data & 0x7F);

	(void) len;
}

// decode RAW data: expected values are 16 bits
static inline void inv_fifo_protocol_helper_decode_raw_data(
        const void *in, uint16_t len, int32_t *x, int32_t *y, int32_t *z)
{
	const uint8_t *rcvbuf = (const uint8_t *)in;
	int16_t temp[3];

	FIFOPROTOCOL_MEMCPY(&temp[0], &rcvbuf[0], sizeof(temp[0]));
	FIFOPROTOCOL_MEMCPY(&temp[1], &rcvbuf[2], sizeof(temp[1]));
	FIFOPROTOCOL_MEMCPY(&temp[2], &rcvbuf[4], sizeof(temp[2]));
	*x = (int32_t)temp[0];
	*y = (int32_t)temp[1];
	*z = (int32_t)temp[2];
	
	(void)len;
}

#ifdef __cplusplus
}
#endif

#endif /* _INV_FIFO_PROTOCOL_HELPER_H_ */

/** @} */
