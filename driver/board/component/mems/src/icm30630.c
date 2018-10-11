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

#include "icm30630.h"
#include "stdio.h"

#include "mems_bus.h"
#include "hal_eint.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "sensor_alg_interface.h"
#include "sensor_manager.h" /* cmd_event_t */
#include "memory_attribute.h"

/*icm driver include */
#include "DeviceIcm30xxx.h"

#if 1
/* syslog */
#define LOGI(fmt,arg...)   LOG_I(sensor, "ICM30630: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(sensor, "ICM30630:WARN: "fmt,##arg)
#define LOGE(fmt,arg...)   LOG_E(sensor, "ICM30630: "fmt,##arg)
#else
/* printf*/
#define LOGI(fmt,arg...)   printf("ICM30630:INFO: "fmt,##arg)
#define LOGW(fmt,arg...)   printf("ICM30630:WARN: "fmt,##arg)
#define LOGE(fmt,arg...)   printf("ICM30630:ERROR: "fmt,##arg)
#endif

/*----------------------------------------------------------------------------*/
struct icm30630_priv {
    uint32_t enable; 		/*enable mask*/
    uint32_t eint_num;
    cmd_event_t acc_cmd_event;
    uint32_t acc_delay;  // delay in ms.
    uint32_t timeout;
    uint32_t t_last_sample;
    uint32_t t_makeup_unit;
    uint32_t data_count;
    uint32_t acc_data_count_inbundle;
    uint8_t id;
    uint32_t step_count;
};
static struct icm30630_priv icm30630_obj;

#define APP_FIFO_MAX_COUNT 32

struct icm_acc_data_def {
    int32_t x;
    int32_t y;
    int32_t z;
};
static struct icm_acc_data_def icm_acc_data[APP_FIFO_MAX_COUNT];

#define ICM_TIME_DIFF_LOW 50L /*low threshold*/
#define ICM_TIME_DIFF_HIGH 1000L /*high threshold*/

#define MAX_WRITE_LENGTH 36

#ifndef __ICCARM__
static uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) icm30630_txbuffer[MAX_WRITE_LENGTH];
static uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) icm30630_rxbuffer[MAX_WRITE_LENGTH];
#else
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t icm30630_txbuffer[MAX_WRITE_LENGTH];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t icm30630_rxbuffer[MAX_WRITE_LENGTH];
#endif

static inv_device_t * device; /* just a handy variable to keep the handle to device object */

/*----------------------------------------------------------------------------*/
static int icm30630_write(uint8_t *buf, uint32_t len)
{
    int32_t ret = 0;
    ret = mems_i2c_write(ICM30630_I2C_ADDRESS, buf, len);

    if (ret < 0) {
        LOGE("Write error (%ld)\r\n", ret);
    }

    return ret;
}
/*----------------------------------------------------------------------------*/
static int icm30630_read(uint8_t reg, uint8_t *buf, uint32_t len)
{
    int32_t ret = 0;

    ret = mems_i2c_write_read_dma(ICM30630_I2C_ADDRESS, reg, icm30630_rxbuffer, len);

    if (ret < 0) {
        LOGE("Read error (%ld), reg(0x%X), len(%d)\r\n", ret, reg, len);
    } else {
        memcpy(buf, icm30630_rxbuffer, len);
    }

    return ret;
}

void icm30630_eint_set(uint32_t eint_num)
{
    icm30630_obj.eint_num = eint_num;
}

static uint32_t icm_counter;

void icm30630_eint_handler(void *parameter)
{
    BaseType_t xHigherPriorityTaskWoken;

    //printf("icm30630_eint_handler (%d) 0x%X\r\n", icm_counter, device);
    icm_counter++;

    hal_eint_mask((hal_eint_number_t)icm30630_obj.eint_num);
    hal_eint_unmask((hal_eint_number_t)icm30630_obj.eint_num);

    if (icm30630_obj.enable == 0){
        return;
    }

    icm30630_obj.acc_cmd_event.event = SM_EVENT_DATA_READY;
    icm30630_obj.acc_cmd_event.data_ready = (1U << SENSOR_TYPE_ACCELEROMETER);
    icm30630_obj.acc_cmd_event.delay = icm30630_obj.acc_delay; /* ACC delay */

    // Post the event.
    xQueueSendFromISR(sm_queue_handle, &(icm30630_obj.acc_cmd_event), &xHigherPriorityTaskWoken);

    // Now the buffer is empty we can switch context if necessary.
    if ( xHigherPriorityTaskWoken ) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    }
}

int my_adapter_open(void)
{
        //Implementation of SPI/I2c open
        //return error code
        return 0;//Serial_open_low_level_driver();
}
int my_adapter_close(void)
{
        //Implementation of SPI/I2c close
        //return error code
        return 0;//Serial_close_low_level_driver();
}

int my_adapter_read_reg(uint8_t reg, uint8_t * rbuffer, uint32_t rlen)
{
    int rc=0;
    //LOGI("R reg(0x%X) len(%d)\r\n", reg, rlen);
    rc = icm30630_read(reg, rbuffer, rlen);
    return rc;
}

int my_adapter_write_reg(uint8_t reg, const uint8_t * wbuffer, uint32_t wlen)
{
    uint8_t reg_len = wlen;
    uint8_t *ptxbuffer = icm30630_txbuffer;
    int32_t ret;

    //LOGI("W reg(0x%X) len(%d)\r\n", reg, wlen);

    if (wlen > (MAX_WRITE_LENGTH-1)) {
        LOGE("I2C Max Write length exceeds!\n");
        return -1;
    }

    ptxbuffer[0] = reg;
    ptxbuffer++;

    while (reg_len != 0) {
        // Register Data
        *ptxbuffer = *wbuffer;
        wbuffer++;
        ptxbuffer++;
        reg_len--;
    }

    ret = icm30630_write(icm30630_txbuffer, (wlen+1));

    if ( HAL_I2C_STATUS_OK != ret) {
        LOGE("I2C Trasmit error(%ld), reg(0x%X), len(%d)\n", ret, reg, wlen);
        return -1;
    }
    return ret;
}

int my_adapter_register_interrupt_callback(void (*interrupt_cb)(void * context, int int_num), void * context)
{
        int rc=0;
        return rc;
}

void inv_icm30xxx_sleep(int ms)
{
    /*
     * You must provide a sleep function that blocks the current programm
     * execution for the specified amount of ms
     */
    vTaskDelay(ms);
}

/*
 * States for icm30xxx device object
 */
static inv_device_icm30xxx_t device_icm30xxx;
/*
 * Some memory to be used by the icm30xxx driver (must be >= 4k)
 */
static uint8_t device_buffer[1024*4];
/*
 * Icm30630 device requires a FW and additionnal DMP images to be loaded on init
 * Provides such images by mean of a byte array
 */
static const uint8_t flash_image[] = {
#include "icm30631_img.flash.m0.h"
};
#if 1
static const uint8_t sram_image[] = {
	#include "icm30631_img.sram.m0.h"
};
#endif
static const uint8_t dmp3_image[] = {
#include "icm30631_img.sram.dmp3.h"
};
static const uint8_t dmp4_image[] = {
#include "icm30631_img.sram.dmp4.h"
};

#if 0
const inv_serif_hal_t serif_instance = {
        my_adapter_read_reg,  /* user read_register() function that reads a register over the serial interface */
        my_adapter_write_reg, /* user write_register() function that writes a register over the serial interface */
        32,                /* maximum number of bytes allowed per transaction */
        32,				/* maximum number of bytes allowed per transaction */
        INV_SERIF_HAL_TYPE_I2C, /* type of the serial interface used between SPI or I2C */
        0xDEAD
};
#endif

const inv_host_serif_t serif_instance = {
        my_adapter_open,      /* user open() function that setups the serial interface */
        my_adapter_close,     /* user close() function that cleanups the serial interface */
        my_adapter_read_reg,  /* user read_register() function that reads a register over the serial interface */
        my_adapter_write_reg, /* user write_register() function that writes a register over the serial interface */
        my_adapter_register_interrupt_callback, /* user my_serif_register_interrupt_callback() function register an interupt callback */
        15,                /* maximum number of bytes allowed per transaction */
        7,				/* maximum number of bytes allowed per transaction */
        INV_HOST_SERIF_TYPE_I2C /* type of the serial interface used between SPI or I2C */
};

static uint32_t icm_acc_count;

static void sensor_event_cb(const inv_sensor_event_t * event, void * arg)
{
    int32_t tempx = 0;
    int32_t tempy = 0;
    int32_t tempz = 0;

    //LOGI("sensor_event_cb E(%d) S(%d)\r\n", event->sensor, event->status);

    switch(event->sensor)
    {
        case INV_SENSOR_TYPE_ACCELEROMETER:
        {
            if (event->status == INV_SENSOR_STATUS_DATA_UPDATED) {
                tempx = (int32_t)(event->data.acc.vect[0]*9800);
                tempy = (int32_t)(event->data.acc.vect[1]*9800);
                tempz = (int32_t)(event->data.acc.vect[2]*9800);

                if (icm30630_obj.acc_data_count_inbundle < APP_FIFO_MAX_COUNT) {
                    icm_acc_data[icm30630_obj.acc_data_count_inbundle].x = tempx;
                    icm_acc_data[icm30630_obj.acc_data_count_inbundle].y = tempy;
                    icm_acc_data[icm30630_obj.acc_data_count_inbundle].z = tempz;
                    icm30630_obj.acc_data_count_inbundle++;
                }

                if ((icm_acc_count%250) == 0) {
                    //LOGI("Ax = %d,Ay = %d,Az = %d, At %d\r\n", tempx, tempy, tempz, (uint32_t)(event->timestamp));
                }
                icm_acc_count++;
            }
        }
        break;

        case INV_SENSOR_TYPE_STEP_COUNTER:
            if (event->status == INV_SENSOR_STATUS_DATA_UPDATED) {
                icm30630_obj.step_count = (uint32_t)event->data.step.count;
                LOGI("STEP_COUNTER = %d\r\n", icm30630_obj.step_count);

                icm30630_obj.acc_cmd_event.event = SM_EVENT_DATA_READY;
                icm30630_obj.acc_cmd_event.data_ready = (1U << SENSOR_TYPE_PEDOMETER);

                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                // Post the event.
                xQueueSendFromISR(sm_queue_handle, &(icm30630_obj.acc_cmd_event), &xHigherPriorityTaskWoken);

                // Now the buffer is empty we can switch context if necessary.
                if ( xHigherPriorityTaskWoken ) {
                    // Actual macro used here is port specific.
                    portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
                }
            }
            break;
        }
}

/*
 * A listener onject will handle sensor events
 */
static inv_sensor_listener_t sensor_listener = {
        sensor_event_cb, /* callback that will receive sensor events */
        (void *)0xDEAD   /* some pointer passed to the callback */
};

uint32_t icm_30630_get_step_count(void)
{
    return icm30630_obj.step_count;
}

void icm_30630_enable_step_count(uint32_t enable)
{
    int32_t rc = 0;
    if (enable) {
    /*
     * Start step counter. How to set sampling rate and timeout?
     */
        rc += inv_device_ping_sensor(device, INV_SENSOR_TYPE_STEP_COUNTER);
        LOGI("ping step counter, rc(%d)\r\n", rc);
        rc += inv_device_set_sensor_period(device, INV_SENSOR_TYPE_STEP_COUNTER, 40);
        rc += inv_device_set_sensor_timeout(device, INV_SENSOR_TYPE_STEP_COUNTER, 0);
        rc += inv_device_start_sensor(device, INV_SENSOR_TYPE_STEP_COUNTER);
        icm30630_obj.enable |= (1U << SENSOR_TYPE_PEDOMETER);
    } else {
        rc += inv_device_flush_sensor(device, INV_SENSOR_TYPE_STEP_COUNTER);
        rc += inv_device_stop_sensor(device, INV_SENSOR_TYPE_STEP_COUNTER);
        icm30630_obj.enable &= (~(1U << SENSOR_TYPE_PEDOMETER));
    }
    LOGI("step count enable(%X), rc(%d)\r\n", icm30630_obj.enable, rc);
}

int32_t icm30630_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                   void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;
    int32_t rc = 0;

    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("ACC Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("ACC SENSOR_DELAY ms (%ld) \r\n", value);
                icm30630_obj.acc_delay = value; // 125 Hz
                if (icm30630_obj.acc_delay != value) {
                    LOGW("acc set delay(%ld). real delay(%ld)\r\n", value, icm30630_obj.acc_delay);
                }
            }
            break;

        case SENSOR_ENABLE:
            value = *(int32_t *)buff_in;
            LOGI("ACC SENSOR_ENABLE (%ld) \r\n", value);

            if (value) {
                /*
                 * Check if Accelerometer is available sensor
                 * if rc value is 0, it means sensor is available,
                 * if rc value is INV_ERROR or INV_ERROR_BAD_ARG, sensor is NA
                 */
                rc += inv_device_ping_sensor(device, INV_SENSOR_TYPE_ACCELEROMETER);
                LOGI( "inv_device_ping_sensor ACC  rc= %d \r\n",rc);

                /*
                * Start accelerometer sensor sample at acc_delay interval with a batch timeout
                */
                rc += inv_device_set_sensor_period(device, INV_SENSOR_TYPE_ACCELEROMETER, icm30630_obj.acc_delay);
                rc += inv_device_set_sensor_timeout(device, INV_SENSOR_TYPE_ACCELEROMETER, icm30630_obj.timeout);
                rc += inv_device_start_sensor(device, INV_SENSOR_TYPE_ACCELEROMETER);
                icm30630_obj.enable |= (1U << SENSOR_TYPE_ACCELEROMETER);
                LOGI( "inv_device_start_sensor ACC  rc= %d \r\n",rc);

                icm30630_obj.t_last_sample = 0;
                icm30630_obj.data_count = 0;
            } else {
                rc += inv_device_flush_sensor(device, INV_SENSOR_TYPE_ACCELEROMETER);
                rc += inv_device_stop_sensor(device, INV_SENSOR_TYPE_ACCELEROMETER);
                icm30630_obj.enable &= (~(1U << SENSOR_TYPE_ACCELEROMETER));
            }
            LOGI("Enable(%X)\r\n", icm30630_obj.enable);
            break;

        case SENSOR_GET_DATA: { // output unit is mm/s^2
            uint32_t fifo_count = 0;
            uint32_t current_tick;

            icm30630_obj.acc_data_count_inbundle = 0;
            current_tick = sensor_driver_get_ms_tick();
            rc += inv_device_poll(device); /* sensor_event_cb called inside */
            if (rc != 0) {
                LOGE("inv_device_poll rc(%d)\r\n", rc);
            } else {
                fifo_count = icm30630_obj.acc_data_count_inbundle;
                //LOGI("%d:%d\r\n", icm30630_obj.acc_data_count_inbundle, current_tick);
            }

            if ( fifo_count > 0) {
                uint32_t acc_frm_idx = 0;
                uint32_t acc_frm_buf_cnt; // the acc data stored in buffer
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                uint32_t out_buffer_cnt = size_out / sizeof(sensor_data_unit_t);
                int32_t time_offset;

                // input buffer count at most FIFO_FRAME_CNT
                //acc_frm_buf_cnt = (acc_frm_cnt > FIFO_FRAME_CNT) ? FIFO_FRAME_CNT : acc_frm_cnt;
                acc_frm_buf_cnt = (fifo_count > APP_FIFO_MAX_COUNT) ? APP_FIFO_MAX_COUNT : fifo_count;
                // output buffer count at most out_buffer_cnt
                acc_frm_buf_cnt = (acc_frm_buf_cnt > out_buffer_cnt) ? out_buffer_cnt : acc_frm_buf_cnt;

                time_offset = icm30630_obj.acc_delay;
                uint32_t this_sample_time = current_tick;

                char icm_time_dump[80] = {0};
                uint32_t log_len = 0;

                if (icm30630_obj.t_last_sample != 0) {
                    icm30630_obj.data_count += fifo_count;

                    int32_t time_diff = current_tick - (icm30630_obj.t_last_sample + fifo_count*icm30630_obj.acc_delay);

                    log_len = strlen(icm_time_dump);

                    if ((time_diff <= ICM_TIME_DIFF_LOW) && (time_diff >= (-ICM_TIME_DIFF_LOW))) {
                        time_offset = icm30630_obj.acc_delay; /* sync*/
                        this_sample_time = icm30630_obj.t_last_sample + fifo_count*icm30630_obj.acc_delay;
                    } else if (((time_diff > ICM_TIME_DIFF_LOW) && (time_diff <= ICM_TIME_DIFF_HIGH)) ||
                    ((time_diff < (-ICM_TIME_DIFF_LOW)) && (time_diff >= (-ICM_TIME_DIFF_HIGH)))) {
                        //LOGI("t::drift: %ld/%ld, %ld,%ld\r\n", time_diff, t_bma_255.data_count, bma2x2.acc_odr, t_bma_255.t_makeup_unit);

                        if (time_diff > 0) {
                            /* mcu clock quicker. */
                            time_offset += icm30630_obj.t_makeup_unit;
                        } else {
                            /* mcu clock slower. */
                            time_offset -= icm30630_obj.t_makeup_unit;
                        }
                        this_sample_time = icm30630_obj.t_last_sample + fifo_count*time_offset;
                    } else if ((time_diff > ICM_TIME_DIFF_HIGH) || (time_diff < (-ICM_TIME_DIFF_HIGH))) {
                        LOGW("t::drift: %ld/%ld, mcu(%ld), sensor(%ld)\r\n", time_diff, icm30630_obj.data_count, current_tick, icm30630_obj.t_last_sample);
                        if (time_diff > 0) {
                            /* mcu clock quicker. */
                            this_sample_time = current_tick;
                        } else {
                            /* mcu clock slower. */
                            acc_frm_buf_cnt = 0;
                            this_sample_time = icm30630_obj.t_last_sample;
                        }
                    }
                }

                *actualout = acc_frm_buf_cnt;

                while (acc_frm_buf_cnt != 0) {
                    start->accelerometer_t.x = icm_acc_data[acc_frm_idx].x;
                    start->accelerometer_t.y = icm_acc_data[acc_frm_idx].y;
                    start->accelerometer_t.z = icm_acc_data[acc_frm_idx].z;

                    start->time_stamp = this_sample_time - ((acc_frm_buf_cnt - 1) * time_offset);
                    if (acc_frm_buf_cnt == *actualout) {

                        if ( icm30630_obj.t_last_sample >= start->time_stamp) {
                            LOGI("first item time_stamp %lu, icm_last_timestamp %lu, time offset %d, fifo_count %d, current_tick %lu\r\n",
                                 start->time_stamp, icm30630_obj.t_last_sample, time_offset, *actualout, this_sample_time);
                        }
                    }

                    acc_frm_idx = (acc_frm_idx + 1) % APP_FIFO_MAX_COUNT; // input index
                    start++; // outout index
                    acc_frm_buf_cnt--;

                    snprintf(icm_time_dump+log_len, 80-log_len, "%ld, ", time_offset);
                    log_len = strlen(icm_time_dump);
                }
                icm30630_obj.t_last_sample = this_sample_time;
                //LOGI("t::%s\r\n", icm_time_dump);
            } else {
                *actualout = 0;
            }
        }

        break;

        case SENSOR_CUST:
        break;

        case SENSOR_BATCH:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("acc SENSOR_BATCH parameter error!\n");
                err = -1;
            } else {
                value = *(uint32_t *)buff_in;
                icm30630_obj.timeout = value;
            }
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

void icm30630_init(void)
{
    int32_t rc = 0;

    uint8_t whoami;
    inv_fw_version_t fw_version;

    /*
     * Create Icm30630 Device
     * Pass to the driver:
     * - reference to serial interface object,
     * - reference to listener that will catch sensor events,
     * - a static buffer for the driver to use as a temporary buffer
     * - various driver option
     */
    inv_device_icm3063x_init(&device_icm30xxx, &serif_instance, &sensor_listener,device_buffer, sizeof(device_buffer), false /* disable debug */);
    //inv_device_icm3063x_init2(&device_icm30xxx, &serif_instance, &sensor_listener,device_buffer, sizeof(device_buffer), false /* disable debug */);
    LOGI( "inv_device_icm3063x_init\r\n");

    /*
     * Simply get generic device handle from Icm30xxx Device
     */
    device = inv_device_icm30xxx_get_base(&device_icm30xxx);
    /*
     * Just get the whoami
     */
    rc += inv_device_whoami(device, &whoami);
    if (rc != 0) {
        icm30630_obj.id = 0xFF;
        LOGE("chip id read error. Correctly mounted?");
        return;
    }
    icm30630_obj.id = whoami;
    LOGI( "inv_device_whoami = %X\r\n",whoami);

    /*
     * Flash device FW (FW must be flashed before calling setup())
     * This step is not mandatory if Icm30630 device was already flashed
     */
    rc += inv_device_load(device, INV_DEVICE_ICM30XXX_IMAGE_TYPE_M0_FLASH, flash_image, sizeof(flash_image), true /* verify */, false /* force */);
    LOGI( "inv_device_load m0 rc= %d\r\n",rc);

#if 1
    rc += inv_device_load(device, INV_DEVICE_ICM30XXX_IMAGE_TYPE_M0_SRAM, sram_image,
    sizeof(sram_image), true /* verify */, false /* force */);
    LOGI("RC=%d,INV_DEVICE_ICM30XXX_IMAGE_TYPE_M0_SRAM\r\n ",rc);
#endif

    /*
     * Now that FW is loaded, configure and initialize the Icm30xxx device
     */
    rc += inv_device_setup(device);
    if (rc != 0) {
        icm30630_obj.id = 0xFE;
        LOGE("device setup error. I2C transmission error?");
        return;
    }
    LOGI( "inv_device_setup rc= %d \r\n",rc);
    /*
     * Now that Icm30630 device was inialized, we can proceed with DMP image loading
     * This step is mandatory as DMP image are not store in non volatile memory
     */
    rc += inv_device_load(device, INV_DEVICE_ICM30XXX_IMAGE_TYPE_DMP3, dmp3_image,
                    sizeof(dmp3_image), true /* verify */, false /* force */);
    LOGI( "inv_device_load dmp3 rc= %d\r\n",rc);
    rc += inv_device_load(device, INV_DEVICE_ICM30XXX_IMAGE_TYPE_DMP4, dmp4_image,
                    sizeof(dmp4_image), true /* verify */, false /* force */);
    LOGI( "inv_device_load dmp4 rc= %d\r\n",rc);
    /*
     * Get device FW the version
     */
    rc += inv_device_get_fw_info(device, &fw_version);
    /* ... do something with fw_version */
    LOGI("rc = %d,version number = V%d.%d.%d\r\n version suffix string = %s\r\n FW checksum = 0x%x\r\n",
        rc,fw_version.major,fw_version.minor,fw_version.patch,fw_version.suffix,fw_version.crc);

    /* EINT is initialized in mems_init.c*/

	/*
         * Now that device is ready, you must call inv_device_poll() function
         * periodically or upon interrupt.
         * The poll function will check for sensor events, and notify, if any,
         * by means of the callback from the listener that was provided on device init.
         */

    /* Mounting matrix configuration (apply for both Acc/Gyr) */
     inv_sensor_config_mounting_mtx_t cfg_mmatrix = {{ 0.f, -1.f, 0.f,
                                                       1.f, 0.f, 0.f,
                                                       0.f, 0.f, 1.f }};
    rc = inv_device_set_sensor_mounting_matrix(device, INV_SENSOR_TYPE_ACCELEROMETER, cfg_mmatrix.matrix);
    LOGI("inv_sensor_config_mounting_mtx_t-ACCELEROMETER rc = %d\r\n",rc);

    icm30630_obj.t_makeup_unit = 1;

    sensor_driver_object_t obj_acc;

    obj_acc.self = (void *)&obj_acc;
    obj_acc.polling = 0; // interupt mode
    obj_acc.sensor_operate = icm30630_operate;
    sensor_driver_attach(SENSOR_TYPE_ACCELEROMETER, &obj_acc);

#if 0
	/*
         * Stop accelerometer sensor
         */
        rc += inv_device_flush_sensor(device, INV_SENSOR_TYPE_ACCELEROMETER);
        rc += inv_device_stop_sensor(device, INV_SENSOR_TYPE_ACCELEROMETER);
		invn_print( "Stop accelerometer sensor  rc= %d ",rc);
        /*
         * Shutdown everything.
         */
        rc += inv_device_cleanup(device);
		invn_print( "inv_device_cleanup  rc= %d ",rc);
        /*
         * Close serial interface link
         */
        rc += inv_host_serif_close(&serif_instance);
#endif

        return;
}

