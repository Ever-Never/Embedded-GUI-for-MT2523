/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "FreeRTOS.h"

/*------------------------------------------------------------------------
* Register Definitions of User start
*------------------------------------------------------------------------*/
#define BMI160_USER_CHIP_ID_ADDR                0x00
#define BMI160_USER_REV_ID_ADDR                 0x01
#define BMI160_USER_ERROR_ADDR                    0X02
#define BMI160_USER_PMU_STATUS_ADDR                0X03
#define BMI160_USER_DATA_0_ADDR                    0X04
#define BMI160_USER_DATA_1_ADDR                    0X05
#define BMI160_USER_DATA_2_ADDR                    0X06
#define BMI160_USER_DATA_3_ADDR                    0X07
#define BMI160_USER_DATA_4_ADDR                    0X08
#define BMI160_USER_DATA_5_ADDR                    0X09
#define BMI160_USER_DATA_6_ADDR                    0X0A
#define BMI160_USER_DATA_7_ADDR                    0X0B
#define BMI160_USER_DATA_8_ADDR                    0X0C
#define BMI160_USER_DATA_9_ADDR                    0X0D
#define BMI160_USER_DATA_10_ADDR                0X0E
#define BMI160_USER_DATA_11_ADDR                0X0F
#define BMI160_USER_DATA_12_ADDR                0X10
#define BMI160_USER_DATA_13_ADDR                0X11
#define BMI160_USER_DATA_14_ADDR                0X12
#define BMI160_USER_DATA_15_ADDR                0X13
#define BMI160_USER_DATA_16_ADDR                0X14
#define BMI160_USER_DATA_17_ADDR                0X15
#define BMI160_USER_DATA_18_ADDR                0X16
#define BMI160_USER_DATA_19_ADDR                0X17
#define BMI160_USER_SENSORTIME_0_ADDR            0X18
#define BMI160_USER_SENSORTIME_1_ADDR            0X19
#define BMI160_USER_SENSORTIME_2_ADDR            0X1A
#define BMI160_USER_STATUS_ADDR                    0X1B
#define BMI160_USER_INT_STATUS_0_ADDR            0X1C
#define BMI160_USER_INT_STATUS_1_ADDR            0X1D
#define BMI160_USER_INT_STATUS_2_ADDR            0X1E
#define BMI160_USER_INT_STATUS_3_ADDR            0X1F
#define BMI160_USER_TEMPERATURE_0_ADDR            0X20
#define BMI160_USER_TEMPERATURE_1_ADDR            0X21
#define BMI160_USER_FIFO_LENGTH_0_ADDR            0X22
#define BMI160_USER_FIFO_LENGTH_1_ADDR            0X23
#define BMI160_USER_FIFO_DATA_ADDR                0X24
#define BMI160_USER_ACC_CONF_ADDR                0X40
#define BMI160_USER_ACC_RANGE_ADDR              0X41
#define BMI160_USER_GYR_CONF_ADDR               0X42
#define BMI160_USER_GYR_RANGE_ADDR              0X43
#define BMI160_USER_MAG_CONF_ADDR                0X44
#define BMI160_USER_FIFO_DOWNS_ADDR             0X45
#define BMI160_USER_FIFO_CONFIG_0_ADDR          0X46
#define BMI160_USER_FIFO_CONFIG_1_ADDR          0X47
#define BMI160_USER_MAG_IF_0_ADDR                0X4B
#define BMI160_USER_MAG_IF_1_ADDR                0X4C
#define BMI160_USER_MAG_IF_2_ADDR                0X4D
#define BMI160_USER_MAG_IF_3_ADDR                0X4E
#define BMI160_USER_MAG_IF_4_ADDR                0X4F
#define BMI160_USER_INT_EN_0_ADDR                0X50
#define BMI160_USER_INT_EN_1_ADDR               0X51
#define BMI160_USER_INT_EN_2_ADDR               0X52
#define BMI160_USER_INT_OUT_CTRL_ADDR            0X53
#define BMI160_USER_INT_LATCH_ADDR                0X54
#define BMI160_USER_INT_MAP_0_ADDR                0X55
#define BMI160_USER_INT_MAP_1_ADDR                0X56
#define BMI160_USER_INT_MAP_2_ADDR                0X57
#define BMI160_USER_INT_DATA_0_ADDR                0X58
#define BMI160_USER_INT_DATA_1_ADDR                0X59
#define BMI160_USER_INT_LOWHIGH_0_ADDR            0X5A
#define BMI160_USER_INT_LOWHIGH_1_ADDR            0X5B
#define BMI160_USER_INT_LOWHIGH_2_ADDR            0X5C
#define BMI160_USER_INT_LOWHIGH_3_ADDR            0X5D
#define BMI160_USER_INT_LOWHIGH_4_ADDR            0X5E
#define BMI160_USER_INT_MOTION_0_ADDR            0X5F
#define BMI160_USER_INT_MOTION_1_ADDR            0X60
#define BMI160_USER_INT_MOTION_2_ADDR            0X61
#define BMI160_USER_INT_MOTION_3_ADDR            0X62
#define BMI160_USER_INT_TAP_0_ADDR                0X63
#define BMI160_USER_INT_TAP_1_ADDR                0X64
#define BMI160_USER_INT_ORIENT_0_ADDR            0X65
#define BMI160_USER_INT_ORIENT_1_ADDR            0X66
#define BMI160_USER_INT_FLAT_0_ADDR                0X67
#define BMI160_USER_INT_FLAT_1_ADDR                0X68
#define BMI160_USER_FOC_CONF_ADDR                0X69
#define BMI160_USER_CONF_ADDR                    0X6A
#define BMI160_USER_IF_CONF_ADDR                0X6B
#define BMI160_USER_PMU_TRIGGER_ADDR            0X6C
#define BMI160_USER_SELF_TEST_ADDR                0X6D
#define BMI160_USER_NV_CONF_ADDR                0x70
#define BMI160_USER_OFFSET_0_ADDR                0X71
#define BMI160_USER_OFFSET_1_ADDR                0X72
#define BMI160_USER_OFFSET_2_ADDR                0X73
#define BMI160_USER_OFFSET_3_ADDR                0X74
#define BMI160_USER_OFFSET_4_ADDR                0X75
#define BMI160_USER_OFFSET_5_ADDR                0X76
#define BMI160_USER_OFFSET_6_ADDR                0X77
#define BMI160_USER_STEP_CNT_0                   0x78
#define BMI160_USER_STEP_CNT_1                   0X79
#define BMI160_USER_STEP_CONF_0                  0X7A
#define BMI160_USER_STEP_CONF_1                  0X7B


/*------------------------------------------------------------------------
* End of Register Definitions of User
*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------
* Start of Register Definitions of CMD
*------------------------------------------------------------------------*/
#define BMI160_CMD_COMMANDS_ADDR                0X7E
#define BMI160_CMD_EXT_MODE_ADDR                0X7F
/*------------------------------------------------------------------------
* End of Register Definitions of CMD
*------------------------------------------------------------------------*/

/* Status Description - Reg Addr --> 0x1B, Bit --> 3 */
#define BMI160_USER_STATUS_FOC_RDY__POS          3
#define BMI160_USER_STATUS_FOC_RDY__LEN          1
#define BMI160_USER_STATUS_FOC_RDY__MSK          0x08
#define BMI160_USER_STATUS_FOC_RDY__REG          BMI160_USER_STATUS_ADDR

/* Status Description - Reg Addr --> 0x1B, Bit --> 4 */
#define BMI160_USER_STATUS_NVM_RDY__POS          4
#define BMI160_USER_STATUS_NVM_RDY__LEN          1
#define BMI160_USER_STATUS_NVM_RDY__MSK          0x10
#define BMI160_USER_STATUS_NVM_RDY__REG          BMI160_USER_STATUS_ADDR

/* ACC ODR - Reg Addr --> 0x40, Bit --> 3:0 */
#define BMI160_USER_ACC_ODR__POS          3
#define BMI160_USER_ACC_ODR__LEN          4
#define BMI160_USER_ACC_ODR__MSK          0xF
#define BMI160_USER_ACC_ODR__REG          BMI160_USER_ACC_CONF_ADDR

/* ACC upsampling - Reg Addr --> 0x40, Bit --> 7 */
#define BMI160_USER_ACC_US__POS          7
#define BMI160_USER_ACC_US__LEN          1
#define BMI160_USER_ACC_US__MSK          0x80
#define BMI160_USER_ACC_US__REG          BMI160_USER_ACC_CONF_ADDR

/* Acc_Range Description - Reg Addr --> 0x41, Bit --> 0...3 */
#define BMI160_USER_ACC_RANGE__POS               0
#define BMI160_USER_ACC_RANGE__LEN               4
#define BMI160_USER_ACC_RANGE__MSK               0x0F
#define BMI160_USER_ACC_RANGE__REG               BMI160_USER_ACC_RANGE_ADDR

/* GYR ODR - Reg Addr --> 0x42, Bit --> 3:0 */
#define BMI160_USER_GYR_ODR__POS          3
#define BMI160_USER_GYR_ODR__LEN          4
#define BMI160_USER_GYR_ODR__MSK          0xF
#define BMI160_USER_GYR_ODR__REG          BMI160_USER_GYR_CONF_ADDR

/* Foc_Conf Description - Reg Addr --> 0x69, Bit --> 6 */
#define BMI160_USER_FOC_GYR_EN__POS               6
#define BMI160_USER_FOC_GYR_EN__LEN               1
#define BMI160_USER_FOC_GYR_EN__MSK               0x40
#define BMI160_USER_FOC_GYR_EN__REG               BMI160_USER_FOC_CONF_ADDR

/* Step count enable - Reg Addr --> 0x7B, Bit --> 3 */
#define BMI160_USER_STEP_CNT_EN__POS          3
#define BMI160_USER_STEP_CNT_EN__LEN          1
#define BMI160_USER_STEP_CNT_EN__MSK          0x08
#define BMI160_USER_STEP_CNT_EN__REG          BMI160_USER_STEP_CONF_1

/* PMU Status */
#define ACC_PMU_POS     4
#define ACC_PMU_LEN     2
#define ACC_PMU_MASK 0x30
#define ACC_PMU_SUSPEND 0
#define ACC_PMU_NORMAL  1
#define ACC_PMU_LOW1    2
#define ACC_PMU_LOW2    3

#define GYR_PMU_POS     2
#define GYR_PMU_LEN     2
#define GYR_PMU_MASK  0xC
#define GYR_PMU_SUSPEND 0
#define GYR_PMU_NORMAL  1
#define GYR_PMU_RESV    2
#define GYR_PMU_FAST_UP 3

/* Cmd */
#define CMD_START_FOC        0x03
#define CMD_ACC_SET_PMU_MODE 0x10
#define CMD_GYR_SET_PMU_MODE 0x14
#define CMD_PROG_NVM         0xA0
#define CMD_FIFO_FLUSH       0xB0
#define CMD_STEM_CNT_CLR     0xB2
#define CMD_SOFTRESET        0xB6

/* INT_OUT_CTRL */
#define INT1_OUTPUT_EN 0x8
#define INT1_OD        0x4
#define INT1_LVL       0x2
#define INT1_EDGE_CRTL 0x1

/* interrupt definition */
#define INT_FLAT_EN_POS       (1U<<7)
#define INT_FWM_EN_POS        (1U<<6)

#define INT_FLAT_MAP1_POS     (1U<<7)
#define INT_FWM_MAP1_POS      (1U<<6)

/* 0x1F bit 7 */
#define FLAT_INT    (1U<<7)
#define FLAT_STATUS (1U<<31)
#define FWM_INT     (1U<<14)

/* 0x47 FIFO_CONFIG */
#define FIFO_GYR_EN      0x80
#define FIFO_ACC_EN      0x40
#define FIFO_MAG_EN      0x20
#define FIFO_HEADER_EN   0x10
#define FIFO_TAG_INT1_EN 0x08
#define FIFO_TAG_INT2_EN 0x04
#define FIFO_TIME_EN     0x02

/*! FIFO Head definition*/
#define FIFO_HEAD_A        0x84
#define FIFO_HEAD_G        0x88
#define FIFO_HEAD_M        0x90

#define FIFO_HEAD_G_A        (FIFO_HEAD_G | FIFO_HEAD_A)
#define FIFO_HEAD_M_A        (FIFO_HEAD_M | FIFO_HEAD_A)
#define FIFO_HEAD_M_G        (FIFO_HEAD_M | FIFO_HEAD_G)

#define FIFO_HEAD_M_G_A         (FIFO_HEAD_M | FIFO_HEAD_G | FIFO_HEAD_A)

#define FIFO_HEAD_FIFO_INPUT_CONFIG  0x48
#define FIFO_HEAD_SENSOR_TIME        0x44
#define FIFO_HEAD_SKIP_FRAME        0x40
#define FIFO_HEAD_OVER_READ_LSB       0x80
#define FIFO_HEAD_OVER_READ_MSB       0x00

#define FIFO_TIME_HEAD       0xFFFB
#define ACC_FIFO_HEAD       0xFFFC
#define GYRO_FIFO_HEAD     0xFFFD
#define MAG_FIFO_HEAD         0xFFFE

/** bmi160 accelerometer data
* brief Structure containing accelerometer values for x,y and
* z-axis in signed short
*/
struct bmi160acc_t {
    int16_t x,
            y,
            z;
};

/** bmi160 gyro data
* brief Structure containing gyro values for x,y and
* z-axis in signed short
*/
struct bmi160gyro_t {
    int16_t x,
            y,
            z;
};

/*bmi fifo analyse return err status*/
enum BMI_FIFO_ANALYSE_RETURN_T {
    FIFO_INPUT_CONFIG_RETURN = -11,
    FIFO_OVER_READ_RETURN = -10,
    FIFO_SENSORTIME_RETURN = -9,
    FIFO_SKIP_OVER_LEN = -8,
    FIFO_M_G_A_OVER_LEN = -7,
    FIFO_M_G_OVER_LEN = -6,
    FIFO_M_A_OVER_LEN = -5,
    FIFO_G_A_OVER_LEN = -4,
    FIFO_M_OVER_LEN = -3,
    FIFO_G_OVER_LEN = -2,
    FIFO_A_OVER_LEN = -1
};

/*enabling which sensors */
enum BMI_SENSORS_ENABLE_T {
    ACC = 1,
    ACC_GYRO
};


/********************* Function Declarations***************************/
uint8_t bmi160_get_acc_range();
int bmi160_conv_acc_mm_s2(int input);
void bmi160_pedometer_enable(unsigned int enable);
unsigned short bmi160_pedometer_cnt(void);
unsigned short bmi160_fifo_length(void);

// time in ms unit
void bmi160_get_fifo_data(unsigned char **data, unsigned short *length, uint32_t *lasttime);
int bmi_fifo_analysis_handle(unsigned char *fifo_data, unsigned short fifo_length);

// input of BMI_SENSORS_ENABLE_T
int bmi160_sensor_Cali_Offset(int *x_offset, int *y_offset, int *z_offset);
int bmi160_read_acc_xyz(struct bmi160acc_t *acc);
int bmi160_read_gyro_xyz(struct bmi160gyro_t *gyro);
void bmi160_eint_set(uint32_t eint_num);
void bmi160_eint_handler(void *parameter);

void ags_init(void);

