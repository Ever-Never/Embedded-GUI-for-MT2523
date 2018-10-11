/*
 * ________________________________________________________________________________________________________
 * Copyright (c) 2016 InvenSense Inc. All rights reserved.
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


#include "Icm30xxx.h"

#include "Invn/EmbUtils/Message.h"
#include "Invn/EmbUtils/DataConverter.h"

#define BYTES_PER_SENSOR                         6
#define DEF_ST_PRECISION                         1000
#define THREE_AXES                               3

/* Accel and gyro LPF bits */
#define DEF_ST_ACCEL_LPF                         2
#define DEF_ST_GYRO_LPF                          2
/* Accel and gyro FSR bits */
#define DEF_ST_6500_GYRO_FS                      (0 << 3)
#define DEF_ST_6500_ACCEL_FS                     (0 << 3)
/* Sample rate */
#define DEF_ST_SAMPLE_RATE                       0
/* Number of samples used to average the shift codes */
#define DEF_ST_SAMPLES                           200

/*---- Self Test Pass/Fail Criteria ----*/
/* Criteria A thresholds */
#define DEF_6500_ACCEL_ST_SHIFT_DELTA_LOW        500
#define DEF_6500_ACCEL_ST_SHIFT_DELTA_HIGH       1500
#define DEF_6500_GYRO_CT_SHIFT_DELTA             500
/* Criteria B thresholds */
#define DEF_GYRO_ST_AL_MIN                       60  /* dps */
#define DEF_ACCEL_ST_AL_MIN                      225 /* mg */
#define DEF_ACCEL_ST_AL_MAX                      675 /* mg */
/* Criteria C thresholds */
#define DEF_GYRO_OFFSET_MAX                      20  /* dps */

/* Scorpion registers definitions */
#define REG_6500_SAMPLE_RATE_DIV                 0x19
#define REG_6500_CONFIG                          0x1A

#define REG_6500_GYRO_CONFIG                     0x1B
#define BITS_6500_SELF_TEST_EN                   0xE0

#define REG_6500_ACCEL_CONFIG                    0x1C

#define REG_6500_ACCEL_CONFIG2                   0x1D
#define BIT_6500_FIFO_SIZE_1K                    0x40

#define REG_6500_FIFO_EN                         0x23
#define BIT_6500_ACCEL_OUT                       0x08
#define BITS_6500_GYRO_OUT                       0x70

#define REG_6500_INT_ENABLE                      0x38

#define REG_6500_USER_CTRL                       0x6A
#define BIT_6500_FIFO_RST                        0x04
#define BIT_6500_FIFO_EN                         0x40

#define REG_6500_PWR_MGMT_1                      0x6B
#define BIT_6500_H_RESET                         0x80
#define BITS_6500_CLKSEL                         (1 << 0)

#define REG_6500_PWR_MGMT_2                      0x6C
#define BIT_6500_LP_DIS                          0x80
#define BIT_6500_DMP_LP_DIS                      0x40

#define REG_6500_FIFO_COUNT_H                    0x72
#define REG_6500_FIFO_R_W                        0x74

#define REG_6500_XG_ST_DATA                      0x00
#define REG_6500_XA_ST_DATA                      0x0D

/* Look-up-table implementing: y = 2620 * (1.01 ^ x) */
static const uint16_t cST_OTP_LookUpTable[256] = {
	2620, 2646, 2672, 2699, 2726, 2753, 2781, 2808,
	2837, 2865, 2894, 2923, 2952, 2981, 3011, 3041,
	3072, 3102, 3133, 3165, 3196, 3228, 3261, 3293,
	3326, 3359, 3393, 3427, 3461, 3496, 3531, 3566,
	3602, 3638, 3674, 3711, 3748, 3786, 3823, 3862,
	3900, 3939, 3979, 4019, 4059, 4099, 4140, 4182,
	4224, 4266, 4308, 4352, 4395, 4439, 4483, 4528,
	4574, 4619, 4665, 4712, 4759, 4807, 4855, 4903,
	4953, 5002, 5052, 5103, 5154, 5205, 5257, 5310,
	5363, 5417, 5471, 5525, 5581, 5636, 5693, 5750,
	5807, 5865, 5924, 5983, 6043, 6104, 6165, 6226,
	6289, 6351, 6415, 6479, 6544, 6609, 6675, 6742,
	6810, 6878, 6946, 7016, 7086, 7157, 7229, 7301,
	7374, 7448, 7522, 7597, 7673, 7750, 7828, 7906,
	7985, 8065, 8145, 8227, 8309, 8392, 8476, 8561,
	8647, 8733, 8820, 8909, 8998, 9088, 9178, 9270,
	9363, 9457, 9551, 9647, 9743, 9841, 9939, 10038,
	10139, 10240, 10343, 10446, 10550, 10656, 10763, 10870,
	10979, 11089, 11200, 11312, 11425, 11539, 11654, 11771,
	11889, 12008, 12128, 12249, 12371, 12495, 12620, 12746,
	12874, 13002, 13132, 13264, 13396, 13530, 13666, 13802,
	13940, 14080, 14221, 14363, 14506, 14652, 14798, 14946,
	15096, 15247, 15399, 15553, 15709, 15866, 16024, 16184,
	16346, 16510, 16675, 16842, 17010, 17180, 17352, 17526,
	17701, 17878, 18057, 18237, 18420, 18604, 18790, 18978,
	19167, 19359, 19553, 19748, 19946, 20145, 20347, 20550,
	20756, 20963, 21173, 21385, 21598, 21814, 22033, 22253,
	22475, 22700, 22927, 23156, 23388, 23622, 23858, 24097,
	24338, 24581, 24827, 25075, 25326, 25579, 25835, 26093,
	26354, 26618, 26884, 27153, 27424, 27699, 27976, 28255,
	28538, 28823, 29112, 29403, 29697, 29994, 30294, 30597,
	30903, 31212, 31524, 31839, 32157, 32479, 32804
};

static int32_t inv_abs(int32_t a)
{
	if(a < 0) 
		a = -a;
	return a;
}

static inline int set_secondary_interface_start_address(struct inv_icm30xxx * st, uint32_t address)
{
	uint8_t temp[4] = {0};

	/* need to convert the 4 bytes of the actual address into 3 bytes: */
	/* Secondary interface channel 0 program start adress Bits {[31:28],
		[19:16]} 0xAB CD EF 12... we want AD..
		2000 4000... should get 20 40 00 */
	temp[0] = (address >> 24) & 0xF0;
	/* this gets bits [31:28]] */
	temp[0] |= (address >> 16) & 0x0F;
	/* should get bits [19:16] */
	/* Secondary interface channel 0 program start adress Bits {[15:8]} */
	temp[1] = (address >> 8) & 0xFF;
	/* Secondary interface channel 0 program start adress Bits {[7:0]} */
	temp[2] = (address & 0xFF);

	/* so specifying 20 40 00 really means 20 00 40  00 */
	return inv_icm30xxx_write_reg(st, 0, ICM30XXX_SEC_INTF_PRGM_START_ADDR_0_B0, temp, 3);

}

/** @brief Writes registers of the scorpion bypassing the M0
 *  @param[in] st                device context handler
 *  @param[in] reg               register address
 *  @param[in] length            number of data bytes to be written
 *  @param[in] data              data buffer holding the data to be written
 *  @return                      0 in case of succes
 */
static int write_scorpion_reg(struct inv_icm30xxx * st, uint8_t reg, int length, uint8_t *data)
{
	int rc = 0;
	int i = 0, j = 0;
	uint8_t spi_program[32] = {0};
	uint8_t temp[2] = {0};

	if (length > 16)
		return INV_ERROR_SIZE;

	spi_program[i++] = SPI_CLK_FREQ_PCLK_DIV_8 | SPI_SLAVE_0_ID;
	spi_program[i++] = (SPI_MASTER_WRITE_COMMAND | (length - 1));
	spi_program[i++] = reg;
	for (j = 0; j < length; j++)
		spi_program[j+i] = data[j];
	spi_program[j+i] = SPI_MASTER_STOP_COMMAND;

	/* write the spi program to SRAM */
	rc += inv_icm30xxx_write_mem(st, ICM30670_SPI_SCRIPT_START_ADDR, spi_program, 32);

	/* set the starting address for the SPI transaction */
	rc += set_secondary_interface_start_address(st, ICM30670_SPI_SCRIPT_START_ADDR);
	/* enable */
	rc += inv_icm30xxx_read_reg(st, 0, ICM30XXX_SEC_INTF_CH0_CONFIG_B0, temp, 1);
	temp[0] |= ICM30XXX_SEC_INTF_CH0_EN_BIT;
	rc += inv_icm30xxx_write_reg(st, 0, ICM30XXX_SEC_INTF_CH0_CONFIG_B0, temp, 1);
	/* kick off the SPI transaction */
	temp[0] = SEC_INTF_CH0_RUN_BIT;
	rc += inv_icm30xxx_write_reg(st, 0, ICM30XXX_MOD_RUN_ONCE_0_B0, temp, 1);
	/*  disable */
	rc += inv_icm30xxx_read_reg(st, 0, ICM30XXX_SEC_INTF_CH0_CONFIG_B0, temp, 1);
	temp[0] &=  ~ICM30XXX_SEC_INTF_CH0_EN_BIT;
	rc = inv_icm30xxx_write_reg(st, 0, ICM30XXX_SEC_INTF_CH0_CONFIG_B0, temp, 1);

	return (rc != 0) ? INV_ERROR : 0;
}

/** @brief Reads registers of the scorpion bypassing the M0
 *  @param[in] st                device context handler
 *  @param[in] reg               register address
 *  @param[in] length            number of data bytes to be read
 *  @warning Maximum length is limited at 16 bytes
 *  @param[in] data              data buffer where the read data will be stored
 *  @return                      0 in case of succes
 */
static int read_scorpion_reg(struct inv_icm30xxx * st , uint8_t reg, int length, uint8_t *data)
{
	int rc = 0;
	uint8_t spi_program[33] = {0};
	int i = 0;
	uint8_t regData[2] = {0};
	uint8_t temp[2] = {0};

	if (length > 16)
		return INV_ERROR_SIZE;

	spi_program[i++] = SPI_CLK_FREQ_PCLK_DIV_8 | SPI_SLAVE_0_ID;
	spi_program[i++] = (SPI_MASTER_READ_COMMAND | (length-1));
	spi_program[i++] = reg;
	spi_program[i++] = SPI_MASTER_READ_UPDATE_COMMAND;
	spi_program[i++] = SPI_MASTER_STOP_COMMAND;

	rc += inv_icm30xxx_write_mem(st, ICM30670_SPI_SCRIPT_START_ADDR, spi_program, 32);
	/* set the starting address for the SPI transaction */
	rc += set_secondary_interface_start_address(st, ICM30670_SPI_SCRIPT_START_ADDR);
	/* enable */
	rc += inv_icm30xxx_read_reg(st, 0, ICM30XXX_SEC_INTF_CH0_CONFIG_B0, temp, 1);
	temp[0] |= ICM30XXX_SEC_INTF_CH0_EN_BIT;

	rc += inv_icm30xxx_write_reg(st, 0, ICM30XXX_SEC_INTF_CH0_CONFIG_B0, temp, 1);

	/* kick off the SPI transaction */
	regData[0] = SEC_INTF_CH0_RUN_BIT;
	rc += inv_icm30xxx_write_reg(st, 0, ICM30XXX_MOD_RUN_ONCE_0_B0, regData, 1);
	/* and read/return the data */
	rc += inv_icm30xxx_read_reg(st, 0, ICM30XXX_EXT_SLV_SENS_DATA_00_B0, data, length);
	/* disable */
	rc += inv_icm30xxx_read_reg(st, 0, ICM30XXX_SEC_INTF_CH0_CONFIG_B0, temp, 1);
	temp[0] &= ~ICM30XXX_SEC_INTF_CH0_EN_BIT;
	rc += inv_icm30xxx_write_reg(st, 0, ICM30XXX_SEC_INTF_CH0_CONFIG_B0, temp, 1);

	return (rc != 0) ? INV_ERROR : 0;
}

/** @brief Reads registers of the scorpion bypassing the M0
 *  @param[in] st                device context handler
 *  @param[in] reg               register address
 *  @param[in] len               number of data bytes to be read
 *  @param[in] data              data buffer where the read data will be stored
 *  @return                      0 in case of succes
 */
static int scorpion_read(struct inv_icm30xxx * st, uint8_t reg, int len, uint8_t *data)
{
	int total, r_l, r;
	uint8_t *d;

	d = data;
	total = len;
	while (total > 0) {
		if (total > 16)
			r_l = 16;
		else
			r_l = total;
		if ((r = read_scorpion_reg(st, reg, r_l, d)) != 0)
			return r;
		d += r_l;
		total -= r_l;
	}

	return 0;
}

static inline int write_scorpion_reg_one(struct inv_icm30xxx * st, uint8_t reg, uint8_t v)
{
	return write_scorpion_reg(st, reg, 1, &v);
}

/** @brief Evaluates if the self test criterias for the gyroscope are fullfiled
 *  @param[in] reg_avg          average value of normal test
 *  @param[in] st_avg           average value of self test
 *  @return                      0 in case of succes
 */
static int check_gyro_self_test(struct inv_icm30xxx * st, uint32_t *reg_avg, uint32_t *st_avg)
{
	int rc;
	int otp_value_zero = 0;
	uint32_t st_shift_prod[3], st_shift_cust[3], st_shift_ratio[3], i;
	uint32_t low_threshold, high_threshold;
	uint8_t regs[3];

	if ((rc = scorpion_read(st, REG_6500_XG_ST_DATA, 3, regs)) != 0)
		return rc;
	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test gyro shift_code - %02x %02x %02x", regs[0], regs[1], regs[2]);

	for (i = 0; i < 3; i++) {
		if (regs[i] != 0) {
			st_shift_prod[i] = (uint32_t)cST_OTP_LookUpTable[regs[i] - 1];
		} else {
			st_shift_prod[i] = 0;
			otp_value_zero = 1;
		}
	}
	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test gyro st_shift_prod - %+d %+d %+d", st_shift_prod[0], st_shift_prod[1], st_shift_prod[2]);

	for (i = 0; i < 3; i++) {
		st_shift_cust[i] = st_avg[i] - reg_avg[i];
		if (!otp_value_zero) {
			/* Self Test Pass/Fail Criteria A */
			st_shift_ratio[i] = st_shift_cust[i] / st_shift_prod[i];
			if (st_shift_ratio[i] < DEF_6500_GYRO_CT_SHIFT_DELTA)
				rc = INV_ERROR;
		} else {
			/* Self Test Pass/Fail Criteria B */
			low_threshold = (DEF_GYRO_ST_AL_MIN * 32768 / 250) * DEF_ST_PRECISION;
			if (st_shift_cust[i] < low_threshold)
				rc = INV_ERROR;
		}
	}
	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test gyro st_shift_cust - %+d %+d %+d", st_shift_cust[0], st_shift_cust[1], st_shift_cust[2]);
	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test gyro st_shift_ratio - %+d %+d %+d", st_shift_ratio[0], st_shift_ratio[1], st_shift_ratio[2]);

	if (rc == 0) {
		/* Self Test Pass/Fail Criteria C */
		for (i = 0; i < 3; i++) {
			high_threshold = (DEF_GYRO_OFFSET_MAX * 32768 / 250) * DEF_ST_PRECISION;
			if (inv_abs(reg_avg[i]) > (int)high_threshold)
				rc = INV_ERROR;
		}
	}

	return rc;
}

/** @brief Evaluates if the self test criterias for the accelerometer are fullfiled
 *  @param[in] reg_avg          average value of normal test
 *  @param[in] st_avg           average value of self test
 *  @return                      0 in case of succes
 */
static int check_accel_self_test(struct inv_icm30xxx * st, uint32_t *reg_avg, uint32_t *st_avg)
{
	int rc;
	int otp_value_zero = 0;
	uint32_t st_shift_prod[3], st_shift_cust[3], st_shift_ratio[3], i;
	uint32_t low_threshold, high_threshold;
	uint8_t regs[3];

	if ((rc = scorpion_read(st, REG_6500_XA_ST_DATA, 3, regs)) != 0)
		return rc;
	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test accel shift_code - %02x %02x %02x", regs[0], regs[1], regs[2]);

	for (i = 0; i < 3; i++) {
		if (regs[i] != 0) {
			st_shift_prod[i] = (uint32_t)cST_OTP_LookUpTable[regs[i] - 1];
		} else {
			st_shift_prod[i] = 0;
			otp_value_zero = 1;
		}
	}
	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test accel st_shift_prod - %+d %+d %+d", st_shift_prod[0], st_shift_prod[1], st_shift_prod[2]);

	if (!otp_value_zero) {
		/* Self Test Pass/Fail Criteria A */
		for (i = 0; i < 3; i++) {
			st_shift_cust[i] = st_avg[i] - reg_avg[i];
			st_shift_ratio[i] = st_shift_cust[i] / st_shift_prod[i];
			if ((st_shift_ratio[i] <= DEF_6500_ACCEL_ST_SHIFT_DELTA_LOW) || (st_shift_ratio[i] >= DEF_6500_ACCEL_ST_SHIFT_DELTA_HIGH))
				rc = INV_ERROR;
		}
	} else {
		/* Self Test Pass/Fail Criteria B */
		for (i = 0; i < 3; i++) {
			st_shift_cust[i] = inv_abs(st_avg[i] - reg_avg[i]);
			low_threshold = (DEF_ACCEL_ST_AL_MIN * 32768 / 2000) * DEF_ST_PRECISION;
			high_threshold = (DEF_ACCEL_ST_AL_MAX * 32768 / 2000) * DEF_ST_PRECISION;
			if (st_shift_cust[i] < low_threshold || st_shift_cust[i] > high_threshold)
				rc = INV_ERROR;
		}
	}
	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test accel st_shift_cust - %+d %+d %+d", st_shift_cust[0], st_shift_cust[1], st_shift_cust[2]);
	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test accel st_shift_ratio - %+d %+d %+d", st_shift_ratio[0], st_shift_ratio[1], st_shift_ratio[2]);

	return rc;
}

/** @brief Computes raw acc and raw gyr average over @sa DEF_ST_SAMPLES samples
 *  @param[in]  st               device context handler
 *  @param[in]  self_test_flag   indicates if self test should be enabled
 *  @param[out] gyro_result      gyr average
 *  @param[out] accel_result     acc average
 *  @return                      0 in case of success
 */
static int self_test_get_data(struct inv_icm30xxx * st, int self_test_flag, uint32_t *gyro_result, uint32_t *accel_result)
{
	int i, j;
	int rc = 0;
	int fifo_count, packet_count, ind, sample_cnt;
	uint8_t data[BYTES_PER_SENSOR * 2];

	rc += write_scorpion_reg_one(st, REG_6500_INT_ENABLE, 0);
	/* disable the sensor output to FIFO */
	rc += write_scorpion_reg_one(st, REG_6500_FIFO_EN, 0);
	/* disable fifo reading */
	rc += write_scorpion_reg_one(st, REG_6500_USER_CTRL, 0);
	/* clear FIFO */
	rc += write_scorpion_reg_one(st, REG_6500_USER_CTRL, BIT_6500_FIFO_RST);
	/* setup parameters */
	rc += write_scorpion_reg_one(st, REG_6500_CONFIG, DEF_ST_GYRO_LPF);
	/* config accel LPF register */
	rc += write_scorpion_reg_one(st, REG_6500_ACCEL_CONFIG2, DEF_ST_ACCEL_LPF | BIT_6500_FIFO_SIZE_1K);
	rc += write_scorpion_reg_one(st, REG_6500_SAMPLE_RATE_DIV, DEF_ST_SAMPLE_RATE);
	/* wait for the sampling rate change to stabilize */
	inv_icm30xxx_sleep(50);
	rc += write_scorpion_reg_one(st, REG_6500_GYRO_CONFIG, self_test_flag | DEF_ST_6500_GYRO_FS);
	rc += write_scorpion_reg_one(st, REG_6500_ACCEL_CONFIG, self_test_flag | DEF_ST_6500_ACCEL_FS);
	/* wait for the output to get stable */
	if (self_test_flag)
		inv_icm30xxx_sleep(30);
	/* enable FIFO reading */
	rc += write_scorpion_reg_one(st, REG_6500_USER_CTRL, BIT_6500_FIFO_EN);

	for (i = 0; i < THREE_AXES; i++) {
		gyro_result[i] = 0;
		accel_result[i] = 0;
	}
	sample_cnt = 0;
	while (sample_cnt < DEF_ST_SAMPLES) {
		/* enable sensor output to FIFO */
		rc += write_scorpion_reg_one(st, REG_6500_FIFO_EN, BITS_6500_GYRO_OUT | BIT_6500_ACCEL_OUT);

		inv_icm30xxx_sleep(10);
		/* disable sensor output to FIFO */
		rc += write_scorpion_reg_one(st, REG_6500_FIFO_EN, 0);
		/* read the number of available bytes in the FIFO */
		rc += scorpion_read(st, REG_6500_FIFO_COUNT_H, 2, data);
		fifo_count = inv_dc_big16_to_int16(&data[0]);
		packet_count = fifo_count / (BYTES_PER_SENSOR * 2);
		/* read the content of the FIFO */
		i = 0;
		while ((i < packet_count) && (sample_cnt < DEF_ST_SAMPLES)) {
			short vals[3];
			/* read a sample for each sensor (accel and gyro) */
			rc += scorpion_read(st, REG_6500_FIFO_R_W, BYTES_PER_SENSOR * 2, data);
			ind = 0;
			for (j = 0; j < THREE_AXES; j++) {
				vals[j] = (short)inv_dc_big16_to_int16(&data[ind + 2 * j]);
				accel_result[j] += vals[j];
			}
			ind += BYTES_PER_SENSOR;

			for (j = 0; j < THREE_AXES; j++) {
				vals[j] = (short)inv_dc_big16_to_int16(&data[ind + 2 * j]);
				gyro_result[j] += vals[j];
			}
			sample_cnt++;
			i++;
		}
	}

	/* compute the average of the collected samples */
	for (j = 0; j < THREE_AXES; j++) {
		accel_result[j] = accel_result[j] / sample_cnt;
		accel_result[j] *= DEF_ST_PRECISION;
	}
	for (j = 0; j < THREE_AXES; j++) {
		gyro_result[j] = gyro_result[j] / sample_cnt;
		gyro_result[j] *= DEF_ST_PRECISION;
	}

	return (rc != 0) ? INV_ERROR : 0;
}

int inv_icm30670_do_self_test(struct inv_icm30xxx * st)
{
	int rc = 0;
	uint8_t temp;
	uint32_t gyro_bias_st[THREE_AXES], gyro_bias_regular[THREE_AXES];
	uint32_t accel_bias_st[THREE_AXES], accel_bias_regular[THREE_AXES];

	/* stop M0 and DMP4 */
	rc += inv_icm30xxx_read_reg(st, 0, ICM30XXX_MOD_EN_B0, &temp, 1);
	temp &= ~ICM30XXX_DMP_EN_BIT & ~ICM30XXX_M0_EN_BIT;
	rc += inv_icm30xxx_write_reg(st, 0, ICM30XXX_MOD_EN_B0, &temp, 1);

	/* reset scorpion */
	rc += write_scorpion_reg_one(st, REG_6500_PWR_MGMT_1, BIT_6500_H_RESET);

	inv_icm30xxx_sleep(100);
	rc += write_scorpion_reg_one(st, REG_6500_PWR_MGMT_1, BITS_6500_CLKSEL);
	rc += write_scorpion_reg_one(st, REG_6500_PWR_MGMT_2, BIT_6500_LP_DIS | BIT_6500_DMP_LP_DIS);

	/* get data without selftest enabled */
	rc += self_test_get_data(st, 0, gyro_bias_regular, accel_bias_regular);

	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test accel bias_regular - %+d %+d %+d", accel_bias_regular[0], accel_bias_regular[1], accel_bias_regular[2]);
	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test gyro bias_regular - %+d %+d %+d", gyro_bias_regular[0], gyro_bias_regular[1], gyro_bias_regular[2]);

	/* get data with selftest enabled */
	rc += self_test_get_data(st, BITS_6500_SELF_TEST_EN, gyro_bias_st, accel_bias_st);

	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test accel bias_st - %+d %+d %+d", accel_bias_st[0], accel_bias_st[1], accel_bias_st[2]);
	INV_MSG(INV_MSG_LEVEL_DEBUG, "self_test gyro bias_st - %+d %+d %+d", gyro_bias_st[0], gyro_bias_st[1], gyro_bias_st[2]);

	/* evaluate selftest criterias */
	rc += check_accel_self_test(st, accel_bias_regular, accel_bias_st);
	rc += check_gyro_self_test(st, gyro_bias_regular, gyro_bias_st);

	return (rc != 0) ? INV_ERROR : 0;
}
