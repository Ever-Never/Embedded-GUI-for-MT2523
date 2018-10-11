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

#include <stdio.h>
#include "string.h" /* memset */
#include "FreeRTOS.h"
#include "bma2x2.h"
#include "bma255_sensor_adaptor.h"
#include "sensor_alg_interface.h"
#include "sensor_manager.h" /* cmd_event_t */
#include "hal_i2c_master.h"
#include "hal_eint.h"

log_create_module(bma255_SM, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(bma255_SM, "[bma255]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(bma255_SM, "[bma255]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(bma255_SM, "[bma255]"fmt,##arg)

#ifdef BMA255_LOG_MSG
#define LOGD(fmt,arg...)   LOGI(fmt,arg...)
#else
#define LOGD(fmt,arg...)
#endif

#define SPI_BUFFER_LEN 5
#define FIFO_MAX_COUNT 32
#define APP_FIFO_MAX_COUNT 32

#define SENSOR_SPP_DATA_MAGIC       54321
#define SENSOR_SPP_DATA_RESERVED    12345

extern struct bma2x2_t bma2x2;
cmd_event_t bma255_event;
int print_count = 0;
uint32_t bma_eint_num;
static bool bma255_chip_init = false;

#define TIMESTAMP_COMPENSATE

#define BMA_TIME_DIFF_LOW 50L /*low threshold*/
#define BMA_TIME_DIFF_HIGH 1000L /*high threshold*/

struct bma255_time_t {
    uint32_t t_mcu_previous;
    uint32_t t_last_sample;
    uint32_t t_makeup_unit;
    uint32_t data_count;
    uint32_t acc_delay;
    uint32_t acc_timeout;
    uint32_t fifo_watermark;
};

static struct bma255_time_t t_bma_255;

#ifndef TIMESTAMP_COMPENSATE
static int32_t time_offset_compute(uint32_t current_tick, uint32_t pre_tick, int number_of_data)
{
    return (current_tick  - pre_tick)/(number_of_data + 1);
}
#endif

#ifndef BMA255_USE_POLLING
void bma255_eint_set(uint32_t eint_num)
{
    bma_eint_num = eint_num;
}

void bma255_eint_handler(void *parameter)
{
    BaseType_t xHigherPriorityTaskWoken;
    hal_eint_mask((hal_eint_number_t)bma_eint_num);
    hal_eint_unmask((hal_eint_number_t)bma_eint_num);

    bma255_event.event = SM_EVENT_DATA_READY;

    bma255_event.data_ready = (1U << SENSOR_TYPE_ACCELEROMETER);

    //bma255_event.timestamp = sensor_driver_get_ms_tick();
    bma255_event.delay = bma2x2.acc_odr; /* ACC delay */

    // Post the event.
    xQueueSendFromISR(sm_queue_handle, &bma255_event, &xHigherPriorityTaskWoken);

    // Now the buffer is empty we can switch context if necessary.
    if ( xHigherPriorityTaskWoken ) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    }

}
#endif

void compensate_data_from_8G(int16_t *data)
{
    if (!data) {
        return;
    }
    *data =(int)(*data * 3.9);
}

int32_t convert_to_mg(struct bma2x2_accel_data *sample_xyz)
{
    if (!sample_xyz) {
        return -1;
    }
    compensate_data_from_8G(&(sample_xyz->x));
    compensate_data_from_8G(&(sample_xyz->y));
    compensate_data_from_8G(&(sample_xyz->z));
    return 0;
}

int32_t bma_acc_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;
    if (bma255_chip_init == false) {
        if (actualout) {
            *actualout = 0;
        }
        err = -1;
        return err;
    }
    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("ACC Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("ACC SENSOR_DELAY ms (%ld) \r\n", value);
                #if 0
                int setting = valid_odr(value);
                if (setting > 0) {
                    bmi160_SPI_read(BMI160_USER_ACC_CONF_ADDR, receive);
                    receive[0] = (receive[0] & (~BMI160_USER_ACC_ODR__MSK)) | (uint8_t)setting;
                    bmi160_SPI_write(BMI160_USER_ACC_CONF_ADDR, receive[0]);
                    t_bmi160.acc_odr = 1000 / value;
                } else {
                    err = -1;
                    LOGE("ACC SENSOR_DELAY invalid frequency (%ld) \r\n", value);
                }
                #endif
                #if 0
                /* odr = 8 mean using 125Hz, odr = 16 mean using 62.5Hz*/
                if (value <= 200) {
                    bma2x2.acc_odr = 8;
                } else {
                    bma2x2.acc_odr = 16;
                }
                if (bma2x2.enabled) {
                    // set bandwidth
                    if (bma2x2.acc_odr == 8) { // set 125Hz
                        err = bma2x2_set_bw(0x0B);
                    } else { // set 62.5Hz
                        err = bma2x2_set_bw(0x0A);
                    }
                }
                t_bma_255.t_makeup_unit = bma2x2.acc_odr/10;
                if (t_bma_255.t_makeup_unit == 0) {
                    t_bma_255.t_makeup_unit = 1;
                }
                #else
                t_bma_255.acc_delay = value;
                #endif
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("ACC Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("ACC SENSOR_ENABLE (%ld) \r\n", value);

                if (value) {
#ifndef BMA255_USE_POLLING
                    uint8_t reg = 0;
#endif
                    err = bma2x2_set_power_mode(BMA2x2_MODE_NORMAL);
                    if ((SUCCESS != err)) {
                        LOGE("enable acc fail: %d \r\n", err);
                        return -1;
                    }

                    // set bandwidth
                    //err = bma2x2_set_bw(0x0B);
                    if (bma2x2.acc_odr == 8) { // set 125Hz
                        err = bma2x2_set_bw(0x0B);
                    } else { // set 62.5Hz
                        err = bma2x2_set_bw(0x0A);
                    }

                    LOGI("ACC bma2x2_set_bw err (%d) \r\n", err);
                    // 0x01 is fifo mode
                    err = bma2x2_set_fifo_mode(0x01);
                    LOGI("ACC bma2x2_set_fifo_mode err (%d) \r\n", err);
                    // 0x00 is toggle xyz
                    err = bma2x2_set_fifo_data_select(0x00);
                    LOGI("ACC bma2x2_set_fifo_data_select err (%d) \r\n", err);
#ifndef BMA255_USE_POLLING
                    // set watermark trigger level
                    err = bma2x2_set_fifo_wml_trig(8);
                    LOGI("ACC bma2x2_set_fifo_wml_trig err (%d) \r\n", err);
                    //1. set interrupt source as INT1 and output type
                    err = bma2x2_set_intr_output_type(0, 0x00);
                    LOGI("ACC bma2x2_set_intr_output_type err (%d) \r\n", err);
                    //2. set active level
                    err = bma2x2_set_intr_level(0, 1);
                    LOGI("ACC bma2x2_set_intr_level err (%d) \r\n", err);

                    // map fifo watermark interrupt source to intr1
                    err = bma2x2_set_intr1_fifo_wm(INTR_ENABLE);
                    LOGI("ACC bma2x2_set_intr1_fifo_wm err (%d) \r\n", err);

                    // 0x01 is enable fifo wm interrupt
                    err = bma2x2_set_intr_fifo_wm(INTR_ENABLE);
                    LOGI("ACC bma2x2_set_intr_fifo_wm err (%d) \r\n", err);

                    bma2x2_set_intr_fifo_full(0x1);
                    
                    /*workaround bit from FAE's suggestion*/
                    reg = 0x8C;
                    bma2x2.BMA2x2_BUS_WRITE_FUNC(bma2x2.dev_addr,
                                                BMA2x2_FIFO_MODE_ADDR,
                                                &reg, BMA2x2_GEN_READ_WRITE_LENGTH);                    
#endif
                    t_bma_255.t_last_sample = 0;
                    t_bma_255.t_mcu_previous = 0;
                    t_bma_255.data_count = 0;
                    bma2x2.enabled = 1;
                } else {
                    if ((SUCCESS != bma2x2_set_power_mode(BMA2x2_MODE_SUSPEND))) {
                        LOGE("disable acc fail: %d \r\n", err);
                        return -1;
                    }

                    // 0x01 is fifo mode
                    err = bma2x2_set_fifo_mode(0x00);

                    // set fifo watermark interrupt1 disable
                    bma2x2_set_intr1_fifo_wm(0x00);

                    bma2x2.enabled = 0;
                }
            }
            break;

        case SENSOR_GET_DATA: { // output unit is mm/s^2
#ifndef BMA255_USE_POLLING
            u8 reg = 0;
#endif
            u8 fifo_count = 0, i = 0;
            u8 fifo_overrun = 0;
            u32 current_tick = 0;
            struct bma2x2_accel_data sample_xyz[FIFO_MAX_COUNT];
            int32_t time_offset;

            print_count ++;
            memset(sample_xyz, 0, sizeof(sample_xyz));
#ifndef BMA255_USE_POLLING
            // get int status
            bma2x2.BMA2x2_BUS_READ_FUNC(
            bma2x2.dev_addr,
            BMA2x2_STAT2_ADDR,
            &reg, BMA2x2_GEN_READ_WRITE_LENGTH);

            //check interrupt source is fifo_irq
            if (reg) {
#endif
                // get fifo counter
                bma2x2_get_fifo_frame_count(&fifo_count);
                bma2x2_get_fifo_overrun(&fifo_overrun);

                current_tick = sensor_driver_get_ms_tick();
                //LOGI("fifo_count %d, reg 0x%x\r\n", fifo_count,reg);
                for (i = 0; i < fifo_count; i ++) {
                    bma2x2_read_accel_xyz( (sample_xyz + i));/* Read the accel XYZ data, reg:0x3F*/

                    /*convert raw data to offset compensation*/
                    convert_to_mg(sample_xyz + i);

                    #if 0
                    if((print_count % 50) == 0)
                    LOGI("(%d) x %d, y %d, z %d, time %u, print_count %d\r\n",
                        i, (s16)sample_xyz[i].x, (s16)sample_xyz[i].y,
                        (s16)sample_xyz[i].z, current_tick, print_count);
                    #endif
                }
#if 0
                // p.45, write 0x3E clear buffer content and reset fifo intr
                //bma2x2_set_fifo_mode(0x01);
                /*workaround bit from FAE's suggestion*/
                reg = 0x8C;
                bma2x2.BMA2x2_BUS_WRITE_FUNC(bma2x2.dev_addr,
                                            BMA2x2_FIFO_MODE_ADDR,
                                            &reg, BMA2x2_GEN_READ_WRITE_LENGTH);
#endif
#ifndef BMA255_USE_POLLING
                /* irq issue workaround from BOSCH*/
                reg = 0x40;
                bma2x2.BMA2x2_BUS_WRITE_FUNC(bma2x2.dev_addr,
                                            0x12,
                                            &reg, BMA2x2_GEN_READ_WRITE_LENGTH);

                reg = 0x80;
                bma2x2.BMA2x2_BUS_WRITE_FUNC(bma2x2.dev_addr,
                                            0x11,
                                            &reg, BMA2x2_GEN_READ_WRITE_LENGTH);

                reg = 0x4C;
                bma2x2.BMA2x2_BUS_WRITE_FUNC(bma2x2.dev_addr,
                                            0x3E,
                                            &reg, BMA2x2_GEN_READ_WRITE_LENGTH);


                reg = 0x4C;
                bma2x2.BMA2x2_BUS_WRITE_FUNC(bma2x2.dev_addr,
                                            0x3E,
                                            &reg, BMA2x2_GEN_READ_WRITE_LENGTH);


                reg = 0x08;
                bma2x2.BMA2x2_BUS_WRITE_FUNC(bma2x2.dev_addr,
                                            0x30,
                                            &reg, BMA2x2_GEN_READ_WRITE_LENGTH);


                reg = 0x00;
                bma2x2.BMA2x2_BUS_WRITE_FUNC(bma2x2.dev_addr,
                                            0x12,
                                            &reg, BMA2x2_GEN_READ_WRITE_LENGTH);


                reg = 0x00;
                bma2x2.BMA2x2_BUS_WRITE_FUNC(bma2x2.dev_addr,
                                            0x11,
                                            &reg, BMA2x2_GEN_READ_WRITE_LENGTH);

#endif

#ifndef BMA255_USE_POLLING
            }
#endif
#if 0
            // get int status
            bma2x2.BMA2x2_BUS_READ_FUNC(
            bma2x2.dev_addr,
            BMA2x2_STAT2_REG,
            &reg, C_BMA2x2_ONE_U8X);
            printf("%s():fifo_count %d, status 0x%x\r\n", __func__, fifo_count, reg);
#endif
            #if 1
            //put sensor data to sensor manager
            if ( fifo_count > 0) {
                uint32_t acc_frm_idx = 0;
                uint32_t acc_frm_buf_cnt; // the acc data stored in buffer
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                uint32_t out_buffer_cnt = size_out / sizeof(sensor_data_unit_t);

                // input buffer count at most FIFO_FRAME_CNT
                //acc_frm_buf_cnt = (acc_frm_cnt > FIFO_FRAME_CNT) ? FIFO_FRAME_CNT : acc_frm_cnt;
                acc_frm_buf_cnt = (fifo_count > APP_FIFO_MAX_COUNT) ? APP_FIFO_MAX_COUNT : fifo_count;
                // output buffer count at most out_buffer_cnt
                acc_frm_buf_cnt = (acc_frm_buf_cnt > out_buffer_cnt) ? out_buffer_cnt : acc_frm_buf_cnt;

                time_offset = bma2x2.acc_odr;
                uint32_t this_sample_time = current_tick;

                #ifdef TIMESTAMP_COMPENSATE
                if (t_bma_255.t_last_sample != 0) {
                    t_bma_255.data_count += fifo_count;

                    int32_t time_diff = current_tick - (t_bma_255.t_last_sample + fifo_count*bma2x2.acc_odr);
                    if ((time_diff <= BMA_TIME_DIFF_LOW) && (time_diff >= (-BMA_TIME_DIFF_LOW))) {
                        time_offset = bma2x2.acc_odr; /* sync*/
                        this_sample_time = t_bma_255.t_last_sample + fifo_count*bma2x2.acc_odr;
                    } else if (((time_diff > BMA_TIME_DIFF_LOW) && (time_diff <= BMA_TIME_DIFF_HIGH)) ||
                    ((time_diff < (-BMA_TIME_DIFF_LOW)) && (time_diff >= (-BMA_TIME_DIFF_HIGH)))) {
                        //LOGI("t::drift: %ld/%ld, %ld,%ld\r\n", time_diff, t_bma_255.data_count, bma2x2.acc_odr, t_bma_255.t_makeup_unit);

                        if (time_diff > 0) {
                            /* mcu clock quicker. */
                            time_offset += t_bma_255.t_makeup_unit;
                        } else {
                            /* mcu clock slower. */
                            time_offset -= t_bma_255.t_makeup_unit;
                        }
                        this_sample_time = t_bma_255.t_last_sample + fifo_count*time_offset;
                    } else if ((time_diff > BMA_TIME_DIFF_HIGH) && (time_diff < (-BMA_TIME_DIFF_HIGH))) {
                        LOGW("t::drift: %ld/%ld, mcu(%ld), sensor(%ld)\r\n", time_diff, t_bma_255.data_count, current_tick, t_bma_255.t_last_sample);
                        if (time_diff > 0) {
                            /* mcu clock quicker. */
                            this_sample_time = current_tick;
                        } else {
                            /* mcu clock slower. */
                            acc_frm_buf_cnt = 0;
                            this_sample_time = t_bma_255.t_last_sample;
                        }
                    }
                }

                #else
                if (t_bma_255.t_mcu_previous != 0 &&
                    t_bma_255.t_mcu_previous >= (current_tick - ((acc_frm_buf_cnt - 1) * bma2x2.acc_odr))) {
                    LOGI("current_tick %lu, bma_last_timestamp %lu",current_tick, t_bma_255.t_mcu_previous);
                    time_offset = time_offset_compute(current_tick, t_bma_255.t_mcu_previous, acc_frm_buf_cnt);
                }
                #endif

                *actualout = acc_frm_buf_cnt;

                //acc_frm_idx = (acc_frm_cnt - acc_frm_buf_cnt) % FIFO_FRAME_CNT;
                char bma_time_dump[80] = {0};
                uint32_t log_len = 0;

                while (acc_frm_buf_cnt != 0) {
                    #ifdef BMA_RAW_DATA
                    start->accelerometer_t.x = sample_xyz[acc_frm_idx].x;
                    start->accelerometer_t.y = sample_xyz[acc_frm_idx].y;
                    start->accelerometer_t.z = sample_xyz[acc_frm_idx].z;

                    #elif defined(BAND_ORIENTATION_OPT1)
                    start->accelerometer_t.x = bma255_conv_acc_mm_s2(0 - sample_xyz[acc_frm_idx].x);
                    start->accelerometer_t.y = bma255_conv_acc_mm_s2(0 - sample_xyz[acc_frm_idx].y);
                    start->accelerometer_t.z = bma255_conv_acc_mm_s2(sample_xyz[acc_frm_idx].z);

                    #elif defined(BAND_ORIENTATION_OPT2)
                    start->accelerometer_t.x = bma255_conv_acc_mm_s2(sample_xyz[acc_frm_idx].x);
                    start->accelerometer_t.y = bma255_conv_acc_mm_s2(sample_xyz[acc_frm_idx].y);
                    start->accelerometer_t.z = bma255_conv_acc_mm_s2(sample_xyz[acc_frm_idx].z);

                    #else
                    start->accelerometer_t.x = bma255_conv_acc_mm_s2(sample_xyz[acc_frm_idx].x);
                    start->accelerometer_t.y = bma255_conv_acc_mm_s2(0 - sample_xyz[acc_frm_idx].y);
                    start->accelerometer_t.z = bma255_conv_acc_mm_s2(0 - sample_xyz[acc_frm_idx].z);
                    #endif

                    start->time_stamp = this_sample_time - ((acc_frm_buf_cnt - 1) * time_offset);
                    if (acc_frm_buf_cnt == *actualout) {
                        
                        if ( t_bma_255.t_last_sample >= start->time_stamp) {
                            LOGI("first item time_stamp %lu, bma_last_timestamp %lu, time offset %d, fifo_count %d, current_tick %lu\r\n",
                                 start->time_stamp, t_bma_255.t_last_sample, time_offset, *actualout, this_sample_time);
                        }
                    }

                    acc_frm_idx = (acc_frm_idx + 1) % APP_FIFO_MAX_COUNT; // input index
                    start++; // outout index
                    acc_frm_buf_cnt--;

                    snprintf(bma_time_dump+log_len, 80-log_len, "%ld, ", time_offset);
                    log_len = strlen(bma_time_dump);
                }
                /*Data lost detection*/
#if 0
                if (bma2x2.acc_odr == 8) {
                    if (current_tick - t_bma_255.t_last_sample > 252 && fifo_count == 32) {
                        send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            982,
                            current_tick - t_bma_255.t_last_sample,
                            current_tick,
                            t_bma_255.t_last_sample,
                            fifo_count,
                            SENSOR_SPP_DATA_RESERVED);
                    }
                } else {
                    if (current_tick - t_bma_255.t_last_sample > 512 && fifo_count == 32) {
                        send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            983,
                            current_tick - t_bma_255.t_last_sample,
                            current_tick,
                            t_bma_255.t_last_sample,
                            fifo_count,
                            SENSOR_SPP_DATA_RESERVED);
                    }
                }
#else
                if (fifo_overrun) {
                    bma2x2_set_fifo_mode(0x01);
                    LOGI("%s():fifo_overrun 0x%x\r\n", __func__, fifo_overrun);
                    send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            982,
                            current_tick - t_bma_255.t_last_sample,
                            current_tick,
                            t_bma_255.t_last_sample,
                            fifo_count,
                            SENSOR_SPP_DATA_RESERVED);
                }
#endif
                t_bma_255.t_mcu_previous = current_tick;
                t_bma_255.t_last_sample = this_sample_time;
                //LOGI("t::%s\r\n", bma_time_dump);
            } else {
                *actualout = 0;
                send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            983,
                            0,
                            0,
                            0,
                            0,
                            SENSOR_SPP_DATA_RESERVED);
            }
            #endif
        }

        break;

        case SENSOR_CUST:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("ACC SENSOR_CUST parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("ACC SENSOR_CUST (%ld) \r\n", value);
                #if 0
                if ((value >= 0) && (value <= 1024)) {
                    bmi160_FIFO_disable();
                    t_bmi160.fifo_watermark = value / 4;
                    bmi160_FIFO_enable();
                } else {
                    err = -1;
                    LOGE("ACC SENSOR_CUST invalid FIFO watermark (%ld) \r\n", value);
                }
                #endif
            }
            break;
        case SENSOR_BATCH: 
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("ACC SENSOR_BATCH parameter error!\n");
                err = -1;
            } else {
                value = *(uint32_t *)buff_in;
                /* 1 ACC data of 7 bytes*/
                //t_bma_255.fifo_watermark = ((value/t_bmi160.acc_delay)*7)/4;
                t_bma_255.fifo_watermark = (value/t_bma_255.acc_delay);
                LOGI("acc timeout(%ld), delay(%ld), watermark(%ld)\r\n", value, t_bma_255.acc_delay, t_bma_255.fifo_watermark);

                t_bma_255.acc_timeout = value;

                if (t_bma_255.acc_timeout <= 200) {
                    bma2x2.acc_odr = 8;
                } else {
                    bma2x2.acc_odr = 16;
                }
                if (bma2x2.enabled) {
                    u32 current_tick = sensor_driver_get_ms_tick();
                    // set bandwidth
                    if (bma2x2.acc_odr == 8) { // set 125Hz
                        err = bma2x2_set_bw(0x0B);
                    } else { // set 62.5Hz
                        err = bma2x2_set_bw(0x0A);
                    }
                    bma2x2_set_fifo_mode(0x01);
                    t_bma_255.t_mcu_previous = current_tick;
                    t_bma_255.t_last_sample = current_tick;
                    LOGI("set sample rate err (%d)\r\n", err);
                }
                t_bma_255.t_makeup_unit = bma2x2.acc_odr/10;
                if (t_bma_255.t_makeup_unit == 0) {
                    t_bma_255.t_makeup_unit = 1;
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

BMA2x2_RETURN_FUNCTION_TYPE bma255_sensor_subsys_init()
{
    sensor_driver_object_t obj_acc;
    BMA2x2_RETURN_FUNCTION_TYPE err = ERROR;

    err = bma255_init();
    if (err == SUCCESS) {
        bma255_chip_init = true;
    }

    obj_acc.self = (void *)&obj_acc;
#ifndef BMA255_USE_POLLING
    obj_acc.polling = 0; // interupt mode
#else
    obj_acc.polling = 1; // polling mode
#endif

    err = bma2x2_set_power_mode(BMA2x2_MODE_SUSPEND);
    if ((SUCCESS != err)) {
        LOGE("disable acc fail: %d \r\n", err);
    }

    obj_acc.sensor_operate = bma_acc_operate;
    sensor_driver_attach(SENSOR_TYPE_ACCELEROMETER, &obj_acc);

    return err;
}


