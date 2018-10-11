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

#include "bmi160.h"
#include "mems_bus.h"
#include "hal_eint.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "sensor_alg_interface.h"
#include "sensor_manager.h" /* cmd_event_t */
#include "stdio.h"

#define SPI_DMA_NON_CACHEABLE_ACCESS

#ifdef SPI_DMA_NON_CACHEABLE_ACCESS
#include "memory_attribute.h"
#endif /*SPI_DMA_NON_CACHEABLE_ACCESS*/

/* syslog */
#define LOGI(fmt,arg...)   LOG_I(sensor, "BMI160: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(sensor, "BMI160: "fmt,##arg)
#define LOGE(fmt,arg...)   LOG_E(sensor, "BMI160: "fmt,##arg)

/*printf*/
//#define LOGI(fmt,arg...)   printf("BMI160:INFO: "fmt,##arg)
//#define LOGE(fmt,arg...)   printf("BMI160:ERROR: "fmt,##arg)

//#define BMI160_LOG_MSG
#ifdef BMI160_LOG_MSG
#define LOGD(fmt,arg...)   LOG_I(sensor, "BMI160: "fmt,##arg)
#else
#define LOGD(fmt,arg...)
#endif

#define SPI_READ_CMD 0x80
#define SPI_WRITE_CMD 0x7F

#define BMI160_TIME_UP (655360L)

struct bmi160_t {
    int32_t enabled_sensors;
    uint32_t first_data_system_time;
    uint32_t first_data_sensor_time;
    uint32_t time_wrap_around_cnt;
    unsigned char chip_id;
    unsigned char acc_range;
    unsigned short step_cnt;
    unsigned int acc_delay;  // delay in ms.
    unsigned int gyro_delay; // delay in ms.
    unsigned int acc_timeout;
    unsigned int gyro_timeout;
    unsigned int int_en;
    unsigned char gyro_range;
    uint32_t fifo_watermark;
    uint32_t eint_num;
    uint32_t eint_status;
    uint32_t current_mcu_time_ms;
    uint32_t prev_acc_mcu_time_ms;
    uint32_t prev_gyro_mcu_time_ms;
};

#define MAX_FIFO_BYTE_COUNT 1024

#ifdef SPI_DMA_NON_CACHEABLE_ACCESS
#ifndef __ICCARM__
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) send_buffer[2];
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) bmi160_receive_buffer[8];
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) int_en[3]; /* one byte more for full-duplex */
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) int_map[3]; /* one byte more for full-duplex */
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) eint_status[5]; /* one byte more for full-duplex */
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) fifo_data_store[MAX_FIFO_BYTE_COUNT + 1]; /* one byte more for full-duplex */
#else
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t send_buffer[2];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t bmi160_receive_buffer[8];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t int_en[3]; /* one byte more for full-duplex */
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t int_map[3]; /* one byte more for full-duplex */
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t eint_status[5]; /* one byte more for full-duplex */
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t fifo_data_store[MAX_FIFO_BYTE_COUNT + 1]; /* one byte more for full-duplex */
#endif

#else
uint8_t fifo_data_store[MAX_FIFO_BYTE_COUNT + 1]; /* one byte more for full-duplex */
#endif /*SPI_DMA_NON_CACHEABLE_ACCESS*/

static struct bmi160_t t_bmi160;

unsigned short lastframe_index[3] = {0, 0, 0};
unsigned int fifo_time = 0;
unsigned int sensortime = 0;

unsigned short fifolength;

/*! FIFO 1024 byte, max fifo frame count not over 150, buffer latest amount */
#define FIFO_FRAME_CNT 16

uint32_t acc_frm_cnt = 0;/*0~146*/
struct bmi160acc_t acc_frame_arr[FIFO_FRAME_CNT];

uint32_t gyro_frm_cnt = 0;/*0~146*/
struct bmi160gyro_t gyro_frame_arr[FIFO_FRAME_CNT];

static void mt_device_usleep(int usec)
{
    /* user-written implementation-specific source code */
    vTaskDelay((usec + 999) / 1000 / portTICK_RATE_MS); // sleep in 1 ms unit
    // LOGI("\nCalled mt_device_usleep=%dusec)", usec);
}

static int bmi160_SPI_write(uint8_t addr, uint8_t value)
{
    int32_t ret;
#ifdef SPI_DMA_NON_CACHEABLE_ACCESS
    send_buffer[0] = addr;
    send_buffer[1] = value;

    ret = mems_spi_write(SPI_SELECT_BMI160, send_buffer, 2U);
#else
    uint8_t send[2];

    send[0] = addr;
    send[1] = value;

    ret = mems_spi_write(SPI_SELECT_BMI160, send, 2U);
#endif /*SPI_DMA_NON_CACHEABLE_ACCESS*/
    mt_device_usleep(2); // 2 us acoording to datasheet

    return ret;
}

static int bmi160_SPI_read(uint8_t addr, uint8_t *result)
{
    uint8_t send;
    int32_t ret = HAL_SPI_MASTER_STATUS_OK;

    send = (addr) | SPI_READ_CMD;

#ifdef SPI_DMA_NON_CACHEABLE_ACCESS
     ret = mems_spi_write_read(SPI_SELECT_BMI160, send, bmi160_receive_buffer, 1);
    *result = bmi160_receive_buffer[1];
#else
    uint8_t receive[2] = {0};
    ret = mems_spi_write_read(SPI_SELECT_BMI160, send, receive, 1);
    *result = receive[1];
#endif /*SPI_DMA_NON_CACHEABLE_ACCESS*/

    return ret;
}

static int bmi160_SPI_multipe_read(uint8_t addr, uint8_t *result, uint16_t len)
{
    uint8_t send;
    int32_t ret = HAL_SPI_MASTER_STATUS_OK;

    send = (addr) | SPI_READ_CMD;

    ret = mems_spi_write_read(SPI_SELECT_BMI160, send, result, len);

    return ret;
}

#if 1
static int bmi160_NVM_Prog(void)
{
    unsigned char receive[2] = {0};
    LOGD("bmi160_NVM_Prog start\n");
    /* NVM_CONF */
    bmi160_SPI_write(BMI160_USER_CONF_ADDR, 0x2);

    /* NVM Programming */
    bmi160_SPI_write(BMI160_CMD_COMMANDS_ADDR, CMD_PROG_NVM);

    /* read status */
    while ((receive[1] & BMI160_USER_STATUS_NVM_RDY__MSK) != BMI160_USER_STATUS_NVM_RDY__MSK) {
        bmi160_SPI_multipe_read(BMI160_USER_STATUS_ADDR, receive,2);
        mt_device_usleep(5000);
        LOGD("[1B]: 0x%X\n", receive[1]);
    }

    /* NVM_CONF */
    bmi160_SPI_write(BMI160_USER_CONF_ADDR, 0);
    LOGD("bmi160_NVM_Prog end\n");
    return 0;
}
#endif

static int bmi160_set_acc_pmu_mode(unsigned char mode)
{
    unsigned char receive[1] = {0};

    LOGI("bmi160_set_acc_pmu_mode (%d)\n", mode);

    bmi160_SPI_read(BMI160_USER_ACC_CONF_ADDR, receive);

    if (mode == ACC_PMU_LOW1) {
        // undersampling mode must be enabled for low power mode
        bmi160_SPI_write(BMI160_USER_ACC_CONF_ADDR, (receive[0] | BMI160_USER_ACC_US__MSK));
    } else if (mode == ACC_PMU_NORMAL) {
        bmi160_SPI_write(BMI160_USER_ACC_CONF_ADDR, (receive[0] & (~BMI160_USER_ACC_US__MSK)));
    }

    bmi160_SPI_write(BMI160_CMD_COMMANDS_ADDR, (CMD_ACC_SET_PMU_MODE | mode));

    receive[0] = 0;
    while (((receive[0] & ACC_PMU_MASK) >> ACC_PMU_POS) != mode) {
        bmi160_SPI_read(BMI160_USER_PMU_STATUS_ADDR, receive);
        LOGD("[0x03] 0x%X\n", receive[0]);
        mt_device_usleep(1000);
    }

    return 0;
}

static int bmi160_set_gyro_pmu_mode(unsigned char mode)
{
    unsigned char receive[1] = {0};

    LOGI("bmi160_set_gyro_pmu_mode (%d)\n", mode);

    bmi160_SPI_write(BMI160_CMD_COMMANDS_ADDR, (CMD_GYR_SET_PMU_MODE | mode));

    receive[0] = 0;
    while (((receive[0] & GYR_PMU_MASK) >> GYR_PMU_POS) != mode) {
        bmi160_SPI_read(BMI160_USER_PMU_STATUS_ADDR, receive);
        LOGD("[0x03] 0x%X\n", receive[0]);
        mt_device_usleep(1000);
    }

    return 0;
}

static void bmi160_FIFO_enable(void)
{
    unsigned char setting;

    /* disable FIFO first */
    setting = FIFO_HEADER_EN;
    bmi160_SPI_write(BMI160_USER_FIFO_CONFIG_1_ADDR, setting);

    /* fifo flush */
    bmi160_SPI_write(BMI160_CMD_COMMANDS_ADDR, CMD_FIFO_FLUSH);

    /* FIFO watermake */
    if (t_bmi160.fifo_watermark > 0) {
        bmi160_SPI_write(BMI160_USER_FIFO_CONFIG_0_ADDR, (uint8_t)(t_bmi160.fifo_watermark));
    }

    setting = FIFO_ACC_EN | FIFO_HEADER_EN | FIFO_TIME_EN;
    if (t_bmi160.enabled_sensors == ACC_GYRO) {
        setting |= FIFO_GYR_EN;
    }
    bmi160_SPI_write(BMI160_USER_FIFO_CONFIG_1_ADDR, setting);
}

static void bmi160_FIFO_disable(void)
{
    unsigned char setting;

    setting = FIFO_HEADER_EN;
    bmi160_SPI_write(BMI160_USER_FIFO_CONFIG_1_ADDR, setting);
}

uint8_t bmi160_get_acc_range()
{
    if (t_bmi160.acc_range == 0x5) {
        return 8; // +-4g
    } else if (t_bmi160.acc_range == 0x8) {
        return 16; // +-8g
    } else if (t_bmi160.acc_range == 0xC) {
        return 32; // +-16g
    } else {
        return 4; // +-2g
    }
}

// Convert bmi160 acc unit to mm/s^2
int bmi160_conv_acc_mm_s2(int input)
{
    int output;
    int divisor;

    configASSERT((input <= 32767) && (input >= -32768));

    divisor = (1 << 16) / bmi160_get_acc_range();
    output = input * 9800 / divisor;
    return output;
}

#if 0
void bmi160_pedometer_enable(unsigned int enable)
{
    unsigned char receive[1] = {0};

    bmi160_SPI_read(BMI160_USER_STEP_CONF_1, receive);

    if (enable != 0) {
        /* Soft reset */
        bmi160_SPI_write(BMI160_CMD_COMMANDS_ADDR, CMD_STEM_CNT_CLR);

        // enable
        if ((receive[0] & BMI160_USER_STEP_CNT_EN__MSK) == 0) {
            bmi160_SPI_write(BMI160_USER_STEP_CONF_1, receive[0] | BMI160_USER_STEP_CNT_EN__MSK);
        }
    } else {
        // disable
        if ((receive[0] & BMI160_USER_STEP_CNT_EN__MSK) != 0) {
            bmi160_SPI_write(BMI160_USER_STEP_CONF_1, receive[0] & (~BMI160_USER_STEP_CNT_EN__MSK));
        }
    }
}

unsigned short bmi160_pedometer_cnt(void)
{
    unsigned char receive[3] = {0}; /* one byte more for full-duplex */

    bmi160_SPI_multipe_read(BMI160_USER_STEP_CNT_0, receive, 2);
    t_bmi160.step_cnt = (unsigned short)
                        ((((unsigned short)receive[2]) << 8) | receive[1]);

    return t_bmi160.step_cnt;
}
#endif

unsigned short bmi160_fifo_length(void)
{
    unsigned short length = 0;

#ifdef SPI_DMA_NON_CACHEABLE_ACCESS
    bmi160_SPI_multipe_read(BMI160_USER_FIFO_LENGTH_0_ADDR, bmi160_receive_buffer, 2);
    length = ((((unsigned short)bmi160_receive_buffer[2]) & 0x7) << 8) | bmi160_receive_buffer[1];
#else
    unsigned char receive[3] = {0}; /* one byte more for full-duplex */
    bmi160_SPI_multipe_read(BMI160_USER_FIFO_LENGTH_0_ADDR, receive, 2);
    length = ((((unsigned short)receive[2]) & 0x7) << 8) | receive[1];
#endif /*SPI_DMA_NON_CACHEABLE_ACCESS*/

    return length;
}

void bmi160_fifo_data(unsigned char *data, unsigned short length)
{
    // For read outs of the FIFO the sensor has to be set to normal mode
    unsigned char pmu_mode = 0xFF;
    unsigned char receive[1] = {0};

    /* To be in PMU normal mode */
    bmi160_SPI_read(BMI160_USER_PMU_STATUS_ADDR, receive);
    if (((receive[0] & ACC_PMU_MASK) >> ACC_PMU_POS) != ACC_PMU_NORMAL) {
        LOGE("PMU not in normal mode [%X]\n", receive[0]);
        pmu_mode = ((receive[0] & ACC_PMU_MASK) >> ACC_PMU_POS);

        /* Set accelerometer to PMU normal mode */
        bmi160_set_acc_pmu_mode(ACC_PMU_NORMAL);
    }

    bmi160_SPI_multipe_read(BMI160_USER_FIFO_DATA_ADDR, data, length);

    if (pmu_mode != 0xFF) {
        /* Set accelerometer back to PMU original mode */
        bmi160_set_acc_pmu_mode(pmu_mode);
    }
}

int bmi_fifo_analysis_handle(unsigned char *fifo_data, unsigned short fifo_length)
{
    unsigned char frame_head;/* every frame head*/
    unsigned short fifo_index = 0;/* fifo data buff index*/
    int last_return_st = 0;
    uint32_t acc_frm_idx, gyro_frm_idx;

    acc_frm_cnt = 0;
    gyro_frm_cnt = 0;
    for (fifo_index = 0; fifo_index < fifo_length;) {
        frame_head = fifo_data[fifo_index];
        //LOGI("frame_head(0x%X)\n", frame_head);
        switch (frame_head) {

            case FIFO_HEAD_SKIP_FRAME: {
                fifo_index = fifo_index + 1;
                if (fifo_index + 1 > fifo_length) {
                    last_return_st = FIFO_SKIP_OVER_LEN;
                    break;
                }

                fifo_index = fifo_index + 1;
                break;
            }

            case FIFO_HEAD_FIFO_INPUT_CONFIG: {
                fifo_index = fifo_index + 1;
                if (fifo_index + 1 > fifo_length) {
                    last_return_st = FIFO_INPUT_CONFIG_RETURN;
                    break;
                }

                LOGD("FIFO input config = 0x%X\r\n", fifo_data[fifo_index]);
                //The FIFO input config frame contains one byte of data
                fifo_index = fifo_index + 1;
                break;
            }

            case FIFO_HEAD_A: {
                fifo_index = fifo_index + 1;

                if (fifo_index + 6 > fifo_length) {
                    last_return_st = FIFO_A_OVER_LEN;
                    break;
                }
                acc_frm_idx = acc_frm_cnt % FIFO_FRAME_CNT;
                acc_frame_arr[acc_frm_idx].x =
                    fifo_data[fifo_index + 1] << 8 |
                    fifo_data[fifo_index + 0];
                acc_frame_arr[acc_frm_idx].y =
                    fifo_data[fifo_index + 3] << 8 |
                    fifo_data[fifo_index + 2];
                acc_frame_arr[acc_frm_idx].z =
                    fifo_data[fifo_index + 5] << 8 |
                    fifo_data[fifo_index + 4];

                acc_frm_cnt++;
                lastframe_index[0] = fifo_index;/*A save*/
                /*fifo A data frame index + 6*/
                fifo_index = fifo_index + 6;

                break;
            }

            case FIFO_HEAD_G: {
                fifo_index = fifo_index + 1;

                if (fifo_index + 6 > fifo_length) {
                    last_return_st = FIFO_G_OVER_LEN;
                    break;
                }
                gyro_frm_idx = gyro_frm_cnt % FIFO_FRAME_CNT;
                gyro_frame_arr[gyro_frm_idx].x =
                    fifo_data[fifo_index + 1] << 8 |
                    fifo_data[fifo_index + 0];
                gyro_frame_arr[gyro_frm_idx].y =
                    fifo_data[fifo_index + 3] << 8 |
                    fifo_data[fifo_index + 2];
                gyro_frame_arr[gyro_frm_idx].z =
                    fifo_data[fifo_index + 5] << 8 |
                    fifo_data[fifo_index + 4];

                gyro_frm_cnt++;
                lastframe_index[1] = fifo_index;/*G save*/
                /*fifo G data frame index + 6*/
                fifo_index = fifo_index + 6;

                break;
            }

            /*G & A */
            case FIFO_HEAD_G_A: {
                /*fifo data frame index + 1*/
                fifo_index = fifo_index + 1;

                if (fifo_index + 12 > fifo_length) {
                    last_return_st = FIFO_G_A_OVER_LEN;
                    break;
                }

                gyro_frm_idx = gyro_frm_cnt % FIFO_FRAME_CNT;
                gyro_frame_arr[gyro_frm_idx].x =
                    fifo_data[fifo_index + 1] << 8 |
                    fifo_data[fifo_index + 0];
                gyro_frame_arr[gyro_frm_idx].y =
                    fifo_data[fifo_index + 3] << 8 |
                    fifo_data[fifo_index + 2];
                gyro_frame_arr[gyro_frm_idx].z =
                    fifo_data[fifo_index + 5] << 8 |
                    fifo_data[fifo_index + 4];

                acc_frm_idx = acc_frm_cnt % FIFO_FRAME_CNT;
                acc_frame_arr[acc_frm_idx].x =
                    fifo_data[fifo_index + 7] << 8 |
                    fifo_data[fifo_index + 6];
                acc_frame_arr[acc_frm_idx].y =
                    fifo_data[fifo_index + 9] << 8 |
                    fifo_data[fifo_index + 8];
                acc_frame_arr[acc_frm_idx].z =
                    fifo_data[fifo_index + 11] << 8 |
                    fifo_data[fifo_index + 10];

                gyro_frm_cnt++;
                acc_frm_cnt++;

                lastframe_index[1] = fifo_index;/*G save*/
                lastframe_index[0] = fifo_index + 6;/*A save*/
                /*fifo AG, data frame index + 12(6+6)*/
                fifo_index = fifo_index + 12;

                break;
            }

            /* sensor time frame*/
            case FIFO_HEAD_SENSOR_TIME: {
                fifo_index = fifo_index + 1;

                if (fifo_index + 3 > fifo_length) {
                    last_return_st = FIFO_SENSORTIME_RETURN;
                    break;
                }
                fifo_time = fifo_data[fifo_index + 2] << 16 |
                            fifo_data[fifo_index + 1] << 8 |
                            fifo_data[fifo_index + 0];

                /*fifo sensor time frame index + 3*/
                fifo_index = fifo_index + 3;
                break;
            }

            default:
                LOGE("frame_head(0x%X), index(%d)\n", frame_head, fifo_index);
                break;

        }
    }

    return last_return_st;
}

// offset compensation should NOT be used in combination with the low power mode
int bmi160_sensor_Cali_Offset(int *x_offset, int *y_offset, int *z_offset)
{
    unsigned char receive[8] = {0}; /* one byte more for full-duplex */
    int ret = 1;

    unsigned char pmu_mode = 0xFF;

    LOGD("bmi160_sensor_Cali_Offset\r\n");

    /* To be in PMU normal mode */
    bmi160_SPI_read(BMI160_USER_PMU_STATUS_ADDR, receive);
    if (((receive[0] & ACC_PMU_MASK) >> ACC_PMU_POS) != ACC_PMU_NORMAL) {
        LOGE("PMU not in normal mode [%X]\n", receive[0]);
        pmu_mode = ((receive[0] & ACC_PMU_MASK) >> ACC_PMU_POS);

        /* Set accelerometer to PMU normal mode */
        bmi160_set_acc_pmu_mode(ACC_PMU_NORMAL);
    }

    receive[0] = 0x3D; // +1 g for z axis
    if (t_bmi160.enabled_sensors == ACC_GYRO) {
        receive[0] |= BMI160_USER_FOC_GYR_EN__MSK;
    }

    /* FOC_CONF */
    bmi160_SPI_write(BMI160_USER_FOC_CONF_ADDR, receive[0]);

    /* Fast Offset Calibration */
    bmi160_SPI_write(BMI160_CMD_COMMANDS_ADDR, CMD_START_FOC);

    /* read status */
    receive[0] = 0;
    while ((receive[0] & BMI160_USER_STATUS_FOC_RDY__MSK) != BMI160_USER_STATUS_FOC_RDY__MSK) {
        bmi160_SPI_read(BMI160_USER_STATUS_ADDR, receive);
        mt_device_usleep(1000);
        LOGD("[1B]: 0x%X\r\n", receive[0]);
    }

    uint16_t len = 3;

    if (t_bmi160.enabled_sensors == ACC_GYRO) {
        len = 7;
    }

    /* multiple read */
    bmi160_SPI_multipe_read(BMI160_USER_OFFSET_0_ADDR, receive, len);
    //LOGD("OFFSET: X(%d) mg, Y(%d) mg, Z(%d) mg\n",
    //    (int)((char)receive[0])*39/10, (int)((char)receive[1])*39/10, (int)((char)receive[2])*39/10);

    LOGD("ACC OFFSET X(%d), Y(%d), Z(%d) mg\r\n",
         ((int)((signed char)receive[1])) * 39 / 10,
         ((int)((signed char)receive[2])) * 39 / 10,
         ((int)((signed char)receive[3])) * 39 / 10);
    *x_offset = (int)((((signed char)receive[1])) * 39 / 10);
    *y_offset = (int)((((signed char)receive[2])) * 39 / 10);
    *z_offset = (int)((((signed char)receive[3])) * 39 / 10);


    /*convert offset data from mg to mm/s^2 */
    *x_offset = (int) (*x_offset * 9.8);
    *y_offset = (int) (*y_offset * 9.8);
    *z_offset = (int) (*z_offset * 9.8);
    LOGD("ACC OFFSET X(%d), Y(%d), Z(%d) mm/s^2\r\n", *x_offset,
         *y_offset,
         *z_offset);

    int16_t gyro_off_x, gyro_off_y, gyro_off_z;

    gyro_off_x = receive[4] | (((uint16_t)(receive[7] & 0x3)) << 8);
    if (gyro_off_x & 0x200) {
        gyro_off_x = -(0x200 - (gyro_off_x & 0x1FF));
    }
    gyro_off_y = receive[5] | (((uint16_t)(receive[7] & 0xC)) << 6);
    if (gyro_off_y & 0x200) {
        gyro_off_y = -(0x200 - (gyro_off_y & 0x1FF));
    }
    gyro_off_z = receive[6] | (((uint16_t)(receive[7] & 0x30)) << 4);
    if (gyro_off_z & 0x200) {
        gyro_off_z = -(0x200 - (gyro_off_z & 0x1FF));
    }

    if (t_bmi160.enabled_sensors == ACC_GYRO) {
        LOGD("GYRO OFFSET X(%d), Y(%d), Z(%d) *61/%d mili-deg/s\r\n",
             gyro_off_x, gyro_off_y, gyro_off_z, (1 << t_bmi160.gyro_range));
    }

    if (pmu_mode != 0xFF) {
        /* Set accelerometer back to PMU original mode */
        bmi160_set_acc_pmu_mode(pmu_mode);
    }
    return ret;
}

#if 0
int bmi160_read_acc_xyz(struct bmi160acc_t *acc)
{
    unsigned char receive[7] = {0}; /* one byte more for full-duplex */

    /* multiple read */
    bmi160_SPI_multipe_read(BMI160_USER_DATA_14_ADDR, receive, 6);

    /* Data X */
    acc->x = (signed short)
             ((((signed short)((signed char)receive[2])) << 8) | (receive[1]));
    /* Data Y */
    acc->y = (signed short)
             ((((signed short)((signed char)receive[4])) << 8) | (receive[3]));
    /* Data Z */
    acc->z = (signed short)
             ((((signed short)((signed char)receive[6])) << 8) | (receive[5]));

    uint8_t acc_range = bmi160_get_acc_range();
    (void)acc_range;
    LOGI("ACC X(%ld), Y(%ld), Z(%ld) mg\n",
         (int32_t)acc_range * 1000 * acc->x / 65536,
         (int32_t)acc_range * 1000 * acc->y / 65536,
         (int32_t)acc_range * 1000 * acc->z / 65536);

    return 0;
}

int bmi160_read_gyro_xyz(struct bmi160gyro_t *gyro)
{
    unsigned char receive[7] = {0}; /* one byte more for full-duplex */

    /* multiple read */
    bmi160_SPI_multipe_read(BMI160_USER_DATA_8_ADDR, receive, 6);

    /* Data X */
    gyro->x = (signed short)
              ((((signed short)((signed char)receive[2])) << 8) | (receive[1]));
    /* Data Y */
    gyro->y = (signed short)
              ((((signed short)((signed char)receive[4])) << 8) | (receive[3]));
    /* Data Z */
    gyro->z = (signed short)
              ((((signed short)((signed char)receive[6])) << 8) | (receive[5]));

    LOGD("GYRO X(%d), Y(%d), Z(%d) *61/%d mili-deg/s\n",
         gyro->x,
         gyro->y,
         gyro->z,
         (1 << t_bmi160.gyro_range));

    return 0;
}

void bmi160_dump_reg(void)
{
    unsigned char receive[9] = {0}; /* one byte more for full-duplex */

    bmi160_SPI_multipe_read(BMI160_USER_FIFO_DOWNS_ADDR, receive, 3);
    LOGI("Reg_0x45: 0x%X, 0x%X, 0x%X\n", receive[1], receive[2], receive[3]);

    bmi160_SPI_multipe_read(BMI160_USER_INT_EN_0_ADDR, receive, 8);
    LOGI("Reg_0x50: 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n", receive[1], receive[2], receive[3],
           receive[4], receive[5], receive[6],
           receive[7], receive[8]);
}
#endif

static void bmi160_int_disable(void)
{
    uint8_t addr;

    /* In addition to enable int_flat_en, you need to map flat int function into physical int pins, i.e.  int1 or int2,
            ex: if flat is mapped to int1 => set reg0x55 = 0x80.
            Also, set reg0x53 = 0x0A (i.e. set int1_output_en =1 and int1_lvl =1).
            In this way, interrupt pin will go from low to high while the flat condition is fulfilled.*/

    if (t_bmi160.int_en != 0) {
        /* reset BMI160_USER_INT_OUT_CTRL_ADDR(0x53)
                            BMI160_USER_INT_EN_0_ADDR(0x50)
                            BMI160_USER_INT_MAP_0_ADDR(0x55)
                            BMI160_USER_INT_EN_1_ADDR(0x51)
                            BMI160_USER_INT_MAP_1_ADDR(0x56) */
        addr = BMI160_USER_INT_OUT_CTRL_ADDR;
        bmi160_SPI_write(addr, 0);
        addr = BMI160_USER_INT_EN_0_ADDR;
        bmi160_SPI_write(addr, 0);
        addr = BMI160_USER_INT_MAP_0_ADDR;
        bmi160_SPI_write(addr, 0);
        addr = BMI160_USER_INT_EN_1_ADDR;
        bmi160_SPI_write(addr, 0);
        addr = BMI160_USER_INT_MAP_1_ADDR;
        bmi160_SPI_write(addr, 0);
        t_bmi160.int_en = 0;
    }
}

static void bmi160_int_enable(unsigned int which)
{
#ifndef SPI_DMA_NON_CACHEABLE_ACCESS
    unsigned char int_en[3] = {0}; /* one byte more for full-duplex */
    unsigned char int_map[3] = {0}; /* one byte more for full-duplex */
#endif
    unsigned char addr, value;

    /* In addition to enable int_flat_en, you need to map flat int function into physical int pins, i.e.  int1 or int2,
            ex: if flat is mapped to int1 => set reg0x55 = 0x80.
            Also, set reg0x53 = 0x0A (i.e. set int1_output_en =1 and int1_lvl =1).
            In this way, interrupt pin will go from low to high while the flat condition is fulfilled.*/

    if (t_bmi160.int_en == 0) {
        // set reg0x53 = 0x0A (i.e. set int1_output_en =1 and int1_lvl =1).
        addr = BMI160_USER_INT_OUT_CTRL_ADDR;
        value = INT1_OUTPUT_EN | INT1_LVL;
        bmi160_SPI_write(addr, value);
        t_bmi160.int_en |= which;
    }

    // INT_EN
    addr = BMI160_USER_INT_EN_0_ADDR;
    bmi160_SPI_multipe_read(addr, int_en, 2);

    // INT_MAP
    addr = BMI160_USER_INT_MAP_0_ADDR;
    bmi160_SPI_multipe_read(addr, int_map, 2);


    if (which & INT_FLAT_EN_POS) {
        // INT_EN
        int_en[1] |= INT_FLAT_EN_POS;

        // flat is mapped to int1
        int_map[1] |= INT_FLAT_MAP1_POS;

    }
    if (which & INT_FWM_EN_POS) {
        // INT_EN
        int_en[2] |= INT_FWM_EN_POS;

        // flat is mapped to int1
        int_map[2] |= INT_FWM_MAP1_POS;
    }

    if (int_en[1] != 0) {
        addr = BMI160_USER_INT_EN_0_ADDR;
        bmi160_SPI_write(addr, int_en[1]);
    }
    if (int_en[2] != 0) {
        addr = BMI160_USER_INT_EN_1_ADDR;
        bmi160_SPI_write(addr, int_en[2]);
    }
    if (int_map[1] != 0) {
        addr = BMI160_USER_INT_MAP_0_ADDR;
        bmi160_SPI_write(addr, int_map[1]);
    }
    if (int_map[2] != 0) {
        addr = BMI160_USER_INT_MAP_1_ADDR;
        bmi160_SPI_write(addr, int_map[2]);
    }

}

cmd_event_t bmi160_event;

void bmi160_eint_set(uint32_t eint_num)
{
    t_bmi160.eint_num = eint_num;
}

void bmi160_eint_handler(void *parameter)
{
    BaseType_t xHigherPriorityTaskWoken;

    hal_eint_mask((hal_eint_number_t)t_bmi160.eint_num);
    hal_eint_unmask((hal_eint_number_t)t_bmi160.eint_num);

    if (t_bmi160.enabled_sensors == 0) {
        return;
    }

    bmi160_event.event = SM_EVENT_DATA_READY;

    bmi160_event.data_ready = (1U << SENSOR_TYPE_ACCELEROMETER);
    if (t_bmi160.enabled_sensors == ACC_GYRO) {
        bmi160_event.data_ready |= (1U << SENSOR_TYPE_GYROSCOPE);
    }
    //bmi160_event.timestamp = sensor_driver_get_ms_tick();
    bmi160_event.delay = t_bmi160.acc_delay; /* ACC delay */

    // Post the event.
    xQueueSendFromISR(sm_queue_handle, &bmi160_event, &xHigherPriorityTaskWoken);

    // Now the buffer is empty we can switch context if necessary.
    if ( xHigherPriorityTaskWoken ) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    }
}

#if 0
static void acc_thread_entry( void *pvParameters )
{
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 10000 / portTICK_PERIOD_MS;

    LOGI("acc_thread_entry\n");

    while (1) {

        // Wait a maximum of 10000ms for either bit 0 to be set within
        // the event group.  Clear the bits before exiting.
        uxBits = xEventGroupWaitBits(
                     xBmi160EventGroup,    // The event group being tested.
                     TILT_BIT | FWM_BIT, // The bits within the event group to wait for.
                     pdTRUE,         // TILT_BIT should be cleared before returning.
                     pdFALSE,        // Don't wait for both bits, either bit will do.
                     xTicksToWait ); // Wait a maximum of 100ms for either bit to be set.
        if (uxBits & TILT_BIT) {
            LOGI("Tilt wake-up\n");
        }
        if (uxBits & FWM_BIT) {
            unsigned short fifo_length = 0;
            unsigned char *buffer = NULL;
            struct bmi160acc_t *acc_frame;
            unsigned short acc_data_count = 0;
            struct bmi160gyro_t *gyro_frame;
            unsigned short gyro_data_count = 0;
            uint32_t sensortime = 0;
            uint32_t system_time = 0;

            bmi160_get_fifo_data(&buffer, &fifo_length, &sensortime);
            LOGD("FIFO watermark count(%d), length(%d), time(%ld) ms\n",
                 fifocounter, fifo_length, sensortime);

            bmi_fifo_analysis_handle(buffer, fifo_length);
            bmi160_get_acc_data(&acc_frame, &acc_data_count);
            bmi160_get_gyro_data(&gyro_frame, &gyro_data_count);

            uint32_t i = 0;
            hwm_sensor_data data = {0};
            uint32_t time_inc = 1000 / t_bmi160.odr;
            LOGD("acc_data_count(%d), gyro_data_count(%d) \n", acc_data_count, gyro_data_count);

            // Initialize system time and corresponding sensor time at first FIFO data
            if (t_bmi160.first_data_sensor_time == 0) {
                t_bmi160.first_data_sensor_time = sensortime;
                t_bmi160.first_data_system_time = mt_device_current_time();
                LOGI("First system time(%ld), sensor time(%ld) ms\n", \
                     t_bmi160.first_data_system_time, t_bmi160.first_data_sensor_time);
            }

            // Wrap around handling, BMI160 sensor time up to 655360 ms
            // Check if wrap around happening in this FIFO data
            if (((acc_data_count * time_inc) > sensortime) ||
                    ((gyro_data_count * time_inc) > sensortime)) {
                t_bmi160.time_wrap_around_cnt++;
                LOGI("Sensor time wrap around. Curr(%ld), time_inc(%ld), ACC(%d), GYRO(%d)\n", \
                     sensortime, time_inc, acc_data_count, gyro_data_count);
            }

            system_time = t_bmi160.first_data_system_time + \
                          (t_bmi160.time_wrap_around_cnt * BMI160_TIME_UP) + \
                          sensortime - t_bmi160.first_data_sensor_time - \
                          (time_inc * acc_data_count);

            data.time = system_time;
            for (i = 0; i < acc_data_count; i++) {
                data.values[0] = acc_frame->x;
                data.values[1] = acc_frame->y;
                data.values[2] = acc_frame->z;
                sensor_driver_set_interrupt_data(SENSOR_TYPE_ACCELEROMETER, &data, acc_data_count);
                data.time += time_inc;
                acc_frame++;
            }

            if (gyro_data_count != 0) {
                system_time = t_bmi160.first_data_system_time + \
                              (t_bmi160.time_wrap_around_cnt * BMI160_TIME_UP) + \
                              sensortime - t_bmi160.first_data_sensor_time - \
                              (time_inc * gyro_data_count);
                data.time = system_time;
                for (i = 0; i < gyro_data_count; i++) {
                    data.values[0] = gyro_frame->x;
                    data.values[1] = gyro_frame->y;
                    data.values[2] = gyro_frame->z;
                    sensor_driver_set_interrupt_data(SENSOR_TYPE_GYROSCOPE, &data, gyro_data_count);
                    data.time += time_inc;
                    gyro_frame++;
                }
            }

            //bmi160_dump_reg();
        }

    }

}

#endif

static int valid_odr(int delay)
{
    int ret = 0;

    if (delay >= ((80+40)/2)) {
        ret = 5; // 12.5 Hz
    } else if (delay >= ((40+20)/2)) {
        ret = 6; // 25 Hz
    } else if (delay >= ((20+10)/2)) {
        ret = 7; // 50 Hz
    } else if (delay >= ((10+5)/2)) {
        ret = 8; // 100 Hz
    } else if (delay >= ((5+(5/2))/2)) {
        ret = 9; // 200 Hz
    } else {
        ret = 10; // 400 Hz
    }
    return ret;
}

/*--------------------------------------------------------------------------------*/
int32_t acc_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;
    unsigned char receive[1] = {0};

    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("ACC Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("ACC SENSOR_DELAY ms (%ld) \r\n", value);
                int setting = valid_odr(value);
                if (setting > 0) {
                    bmi160_SPI_read(BMI160_USER_ACC_CONF_ADDR, receive);
                    receive[0] = (receive[0] & (~BMI160_USER_ACC_ODR__MSK)) | (uint8_t)setting;
                    bmi160_SPI_write(BMI160_USER_ACC_CONF_ADDR, receive[0]);
                } else {
                    err = -1;
                    LOGE("ACC SENSOR_DELAY invalid frequency (%ld) \r\n", value);
                }
                int32_t real_delay;

                if(setting <= 8) {
                    real_delay = 10 * (1<<(8-setting));
                } else {
                    real_delay = 10 / (1<<(setting-8));
                }
                t_bmi160.acc_delay = real_delay;
                if (real_delay != value) {
                    LOGW("acc set delay(%ld). real delay(%ld)\r\n", value, real_delay);
                }
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("ACC Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                if (value) {
                    if ((err = bmi160_set_acc_pmu_mode(ACC_PMU_NORMAL))) {
                        LOGE("enable acc fail: %d \r\n", err);
                        return -1;
                    }
                    t_bmi160.enabled_sensors = ACC;
                    bmi160_int_enable(INT_FWM_EN_POS);
                    bmi160_FIFO_enable();
                } else {
                    configASSERT(t_bmi160.enabled_sensors == ACC);
                    if ((err = bmi160_set_acc_pmu_mode(ACC_PMU_SUSPEND))) {
                        LOGE("disable acc fail: %d \r\n", err);
                        return -1;
                    }
                    bmi160_FIFO_disable();
                    bmi160_int_disable();
                    t_bmi160.acc_timeout = 0;
                    t_bmi160.enabled_sensors = 0;
                }
            }
            break;

        case SENSOR_GET_DATA: { // output unit is mm/s^2
        #ifndef SPI_DMA_NON_CACHEABLE_ACCESS
            uint8_t eint_status[5] = {0}; /* one byte more for full-duplex */
        #endif /*SPI_DMA_NON_CACHEABLE_ACCESS*/

            bmi160_SPI_multipe_read(BMI160_USER_INT_STATUS_0_ADDR, eint_status, 4);
            t_bmi160.eint_status = (unsigned int) ((eint_status[4] << 24)
                                                   | (eint_status[3] << 16)
                                                   | (eint_status[2] << 8)
                                                   | (eint_status[1]));

            if ((t_bmi160.eint_status & FLAT_INT) && ((t_bmi160.eint_status & FLAT_STATUS) == 0)) {

            } else if (t_bmi160.eint_status & FWM_INT) {
                fifolength = bmi160_fifo_length();

                bmi160_fifo_data(fifo_data_store, fifolength + 4); // +4 to get sensor time
                fifo_time = (unsigned int) ((((unsigned int)fifo_data_store[fifolength+4]) << 16)
                                                         |(((unsigned int)fifo_data_store[fifolength+3]) << 8)
                                                         | (fifo_data_store[fifolength+2]));

            }
        }

        sensortime = (fifo_time >> 4) * 10 / 16;

        t_bmi160.current_mcu_time_ms = sensor_driver_get_ms_tick();

        bmi_fifo_analysis_handle(fifo_data_store + 1, fifolength);
        if ((buff_out != NULL) && (acc_frm_cnt > 0)) {
            uint32_t acc_frm_idx = 0;
            uint32_t acc_frm_buf_cnt; // the acc data stored in buffer
            sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
            uint32_t out_buffer_cnt = size_out / sizeof(sensor_data_unit_t);

            // input buffer count at most FIFO_FRAME_CNT
            acc_frm_buf_cnt = (acc_frm_cnt > FIFO_FRAME_CNT) ? FIFO_FRAME_CNT : acc_frm_cnt;
            // output buffer count at most out_buffer_cnt
            acc_frm_buf_cnt = (acc_frm_buf_cnt > out_buffer_cnt) ? out_buffer_cnt : acc_frm_buf_cnt;
#if 1
            /* timestamp check */
            uint32_t time_period = t_bmi160.current_mcu_time_ms - t_bmi160.prev_acc_mcu_time_ms;

            if ( time_period < ((acc_frm_buf_cnt - 1) * t_bmi160.acc_delay)) {
                LOGW("timestamp back (%ld, %ld, %ld)\r\n", t_bmi160.current_mcu_time_ms,  t_bmi160.prev_acc_mcu_time_ms, acc_frm_buf_cnt);
                acc_frm_buf_cnt = time_period / t_bmi160.acc_delay;
                if ( (time_period%t_bmi160.acc_delay) > (t_bmi160.acc_delay/2) ) {
                    acc_frm_buf_cnt += 1;
                }
                LOGW("acc_frm_buf_cnt changed to %ld\r\n", acc_frm_buf_cnt);
            }
            t_bmi160.prev_acc_mcu_time_ms = t_bmi160.current_mcu_time_ms;

#endif
            *actualout = acc_frm_buf_cnt;
            acc_frm_idx = (acc_frm_cnt - acc_frm_buf_cnt) % FIFO_FRAME_CNT;
            while (acc_frm_buf_cnt != 0) {
                start->accelerometer_t.x = bmi160_conv_acc_mm_s2(acc_frame_arr[acc_frm_idx].x);
                start->accelerometer_t.y = bmi160_conv_acc_mm_s2(acc_frame_arr[acc_frm_idx].y);
                start->accelerometer_t.z = bmi160_conv_acc_mm_s2(acc_frame_arr[acc_frm_idx].z);
                start->time_stamp = t_bmi160.current_mcu_time_ms - ((acc_frm_buf_cnt - 1) * t_bmi160.acc_delay);

                acc_frm_idx = (acc_frm_idx + 1) % FIFO_FRAME_CNT; // input index
                start++; // outout index
                acc_frm_buf_cnt--;
            }
        }
        break;

        case SENSOR_BATCH:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("ACC SENSOR_BATCH parameter error!\n");
                err = -1;
            } else {
                value = *(uint32_t *)buff_in;
                /* 1 ACC data of 7 bytes*/
                t_bmi160.fifo_watermark = ((value/t_bmi160.acc_delay)*7)/4;
                LOGI("acc timeout(%ld), delay(%ld), watermark(%ld)\r\n", value, t_bmi160.acc_delay, t_bmi160.fifo_watermark);

                if ((t_bmi160.fifo_watermark > 0) && (t_bmi160.fifo_watermark <= 256)) {
                    bmi160_FIFO_disable();
                    bmi160_FIFO_enable();
                    t_bmi160.acc_timeout = value;
                } else {
                    err = -1;
                    LOGE("ACC SENSOR_BATCH invalid FIFO watermark (%ld) \r\n", value);
                }
            }
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

/*--------------------------------------------------------------------------------*/
int32_t gyro_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                     void *buff_in, int32_t size_in)
{
    int err = 0;
    int value = 0;
    unsigned char receive[1] = {0};
    int cal_x,cal_y,cal_z;

    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("GYRO Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int *)buff_in;
                LOGI("GYRO SENSOR_DELAY ms (%d) \r\n", value);
                int setting = valid_odr(value);
                if (setting > 0) {
                    bmi160_SPI_read(BMI160_USER_GYR_CONF_ADDR, receive);
                    receive[0] = (receive[0] & (~BMI160_USER_GYR_ODR__MSK)) | (uint8_t)setting;
                    bmi160_SPI_write(BMI160_USER_GYR_CONF_ADDR, receive[0]);
                } else {
                    err = -1;
                    LOGE("GYRO SENSOR_DELAY invalid frequency (%d) \r\n", value);
                }
                int32_t real_delay;

                if(setting <= 8) {
                    real_delay = 10 * (1<<(8-setting));
                } else {
                    real_delay = 10 / (1<<(setting-8));
                }
                t_bmi160.gyro_delay = real_delay;
                if (real_delay != value) {
                    LOGW("gyro set delay(%ld). real delay(%ld)\r\n", value, real_delay);
                }
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int *)buff_in;
                if (value) {
                    configASSERT(t_bmi160.enabled_sensors == ACC);
                    if ((err = bmi160_set_gyro_pmu_mode(GYR_PMU_NORMAL))) {
                        LOGE("enable gyro fail: %d \r\n", err);
                        return -1;
                    }
                    t_bmi160.enabled_sensors = ACC_GYRO;
                    bmi160_FIFO_enable();
                } else {
                    configASSERT(t_bmi160.enabled_sensors == ACC_GYRO);
                    if ((err = bmi160_set_gyro_pmu_mode(GYR_PMU_SUSPEND))) {
                        LOGE("disable gyro fail: %d \r\n", err);
                        return -1;
                    }
                    t_bmi160.gyro_timeout = 0;
                    t_bmi160.enabled_sensors = ACC;
                    bmi160_FIFO_enable(); /* To re-config FIFO*/
                }
            }
            break;

        case SENSOR_GET_DATA: // // output unit is mili-degree/s
            // suppose hw fifo is read in acc data acquisition
            if ((buff_out != NULL) && (gyro_frm_cnt > 0)) {
                uint32_t gyro_frm_idx = 0;
                uint32_t gyro_frm_buf_cnt; // the acc data stored in buffer
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                uint32_t out_buffer_cnt = size_out / sizeof(sensor_data_unit_t);

                // input buffer count at most FIFO_FRAME_CNT
                gyro_frm_buf_cnt = (gyro_frm_cnt > FIFO_FRAME_CNT) ? FIFO_FRAME_CNT : gyro_frm_cnt;
                // output buffer count at most out_buffer_cnt
                gyro_frm_buf_cnt = (gyro_frm_buf_cnt > out_buffer_cnt) ? out_buffer_cnt : gyro_frm_buf_cnt;
    #if 1
                /* timestamp check */
                uint32_t time_period = t_bmi160.current_mcu_time_ms - t_bmi160.prev_gyro_mcu_time_ms;

                if ( time_period < ((gyro_frm_buf_cnt - 1) * t_bmi160.gyro_delay)) {
                    LOGW("timestamp back (%ld, %ld, %ld)\r\n", t_bmi160.current_mcu_time_ms,  t_bmi160.prev_gyro_mcu_time_ms, gyro_frm_buf_cnt);
                    gyro_frm_buf_cnt = time_period / t_bmi160.gyro_delay;
                    if ( (time_period%t_bmi160.gyro_delay) > (t_bmi160.gyro_delay/2) ) {
                        gyro_frm_buf_cnt += 1;
                    }
                    LOGW("gyro_frm_buf_cnt changed to %ld\r\n", gyro_frm_buf_cnt);
                }
                t_bmi160.prev_gyro_mcu_time_ms = t_bmi160.current_mcu_time_ms;
    #endif
                *actualout = gyro_frm_buf_cnt;
                gyro_frm_idx = (gyro_frm_cnt - gyro_frm_buf_cnt) % FIFO_FRAME_CNT;
                while (gyro_frm_buf_cnt != 0) {
                    start->gyroscope_t.x = (int)(gyro_frame_arr[gyro_frm_idx].x) * 61;
                    start->gyroscope_t.y = (int)(gyro_frame_arr[gyro_frm_idx].y) * 61;
                    start->gyroscope_t.z = (int)(gyro_frame_arr[gyro_frm_idx].z) * 61;
                    start->time_stamp = t_bmi160.current_mcu_time_ms - ((gyro_frm_buf_cnt - 1) * t_bmi160.gyro_delay);

                    gyro_frm_idx = (gyro_frm_idx + 1) % FIFO_FRAME_CNT; // input index
                    start++; // outout index
                    gyro_frm_buf_cnt--;
                }
            }
            break;
        case SENSOR_CUST:
            value = *(int *)buff_in;
            LOGD("GYRO CUST , input = %d \r\n", value);
            bmi160_sensor_Cali_Offset(&cal_x,&cal_y,&cal_z);
            bmi160_NVM_Prog();
            break;

        case SENSOR_BATCH:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("acc+gyro SENSOR_BATCH parameter error!\n");
                err = -1;
            } else {
                value = *(uint32_t *)buff_in;

                if (value < t_bmi160.acc_timeout) {
                    value = t_bmi160.acc_timeout;
                }

                /* 1 ACC data of 7 bytes*/
                t_bmi160.fifo_watermark =( ((value/t_bmi160.acc_delay)*7)+(((value/t_bmi160.gyro_delay)*7)))/4;

                LOGI("acc+gyro timeout(%ld), delay(%ld,%ld), watermark(%ld)\r\n", value, t_bmi160.acc_delay, t_bmi160.gyro_delay, t_bmi160.fifo_watermark);

                if ((t_bmi160.fifo_watermark > 0) && (t_bmi160.fifo_watermark <= 256)) {
                    bmi160_FIFO_disable();
                    bmi160_FIFO_enable();
                } else {
                    err = -1;
                    LOGE("ACC SENSOR_BATCH invalid FIFO watermark (%ld) \r\n", value);
                }
            }
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

/*----------------------------------------------------------------------------*/
/* acc + gyro sensor init */
void ags_init(void)
{
    sensor_driver_object_t obj_acc, obj_gyro;
    uint8_t receive[2] = {0};

    /* Soft reset */
    bmi160_SPI_write(BMI160_CMD_COMMANDS_ADDR, CMD_SOFTRESET);

    /* Structure initialization */
    t_bmi160.enabled_sensors = 0;
    t_bmi160.chip_id = 0xD1;
    // mg unit: acc_range*9800*acc_frame->x/65536;
    t_bmi160.acc_range = 0x5;
    t_bmi160.step_cnt = 0;
    t_bmi160.acc_delay = 10;
    t_bmi160.gyro_delay= 10;
    t_bmi160.int_en = 0;
    // 16 bit for +/-2000 degrees per second. Unit: gyro_range/65536 = 0.061 degree per second
    t_bmi160.gyro_range = 0;
    t_bmi160.first_data_system_time = 0;
    t_bmi160.first_data_sensor_time = 0;
    t_bmi160.time_wrap_around_cnt = 0;
    t_bmi160.eint_status = 0;
    t_bmi160.fifo_watermark = 0;
    t_bmi160.current_mcu_time_ms = 0;
    t_bmi160.prev_acc_mcu_time_ms = 0;
    t_bmi160.prev_gyro_mcu_time_ms = 0;

    int fail_cout = 0;
    while (receive[0] != t_bmi160.chip_id) {
        /* read Chip Id */
        bmi160_SPI_read(BMI160_USER_CHIP_ID_ADDR, receive);
        LOGI("chip id == 0x%X\n", receive[0]);
        if (receive[0] == t_bmi160.chip_id) {
            break;
        }
        fail_cout++;
        if(fail_cout > 5) {
            return;
        }
        mt_device_usleep(1000);
    }

    bmi160_SPI_read(BMI160_USER_ACC_RANGE_ADDR, receive);
    receive[0] = ((receive[0] & (~BMI160_USER_ACC_RANGE__MSK)) | t_bmi160.acc_range);
    bmi160_SPI_write(BMI160_USER_ACC_RANGE_ADDR, receive[0]);

    bmi160_SPI_read(BMI160_USER_GYR_RANGE_ADDR, &(t_bmi160.gyro_range));

    //+++
    //LOGD("acc gyro offset en \r\n");
    //bmi160_SPI_read(BMI160_USER_OFFSET_6_ADDR, receive);
    //bmi160_SPI_write(BMI160_USER_OFFSET_6_ADDR, receive[0] | 0xC0); //0x80 GYRO_OFF_EN , 0x40 ACC_OFF_EN. 0x80|0x40 = 0xC0
    //---

    obj_acc.self = (void *)&obj_acc;
    obj_acc.polling = 0; // interupt mode
    obj_acc.sensor_operate = acc_operate;
    sensor_driver_attach(SENSOR_TYPE_ACCELEROMETER, &obj_acc);

    obj_gyro.self = (void *)&obj_gyro;
    obj_gyro.polling = 0; // polling mode
    obj_gyro.sensor_operate = gyro_operate;
    sensor_driver_attach(SENSOR_TYPE_GYROSCOPE, &obj_gyro);
}

