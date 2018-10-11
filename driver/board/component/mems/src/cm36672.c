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

#include "cm36672.h"
#include "stdio.h"

#include "mems_bus.h"
#include "hal_eint.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "sensor_alg_interface.h"
#include "sensor_manager.h" /* cmd_event_t */
#include "memory_attribute.h"

/* syslog */
#define LOGI(fmt,arg...)   LOG_I(sensor, "CM36672: "fmt,##arg)
#define LOGE(fmt,arg...)   LOG_E(sensor, "CM36672: "fmt,##arg)
/* printf*/
//#define LOGI(fmt,arg...)   printf("CM36672:INFO: "fmt,##arg)
//#define LOGE(fmt,arg...)   printf("CM36672:ERROR: "fmt,##arg)

#define PS_HIGH_THRESHOLD 13
#define PS_LOW_THRESHOLD 10

/*----------------------------------------------------------------------------*/
struct cm36672_priv {
    uint32_t enable; 		/*enable mask*/
    int32_t distance; /* INT_Flag*/
    uint32_t eint_num;
    cmd_event_t ps_cmd_event;
    uint8_t id;
};

static struct cm36672_priv cm36672_obj;

/*----------------------------------------------------------------------------*/
static int cm36672_write(uint8_t *buf, uint32_t len)
{
    int32_t ret = 0;

    ret = mems_i2c_write(CM36672_I2C_ADDRESS, buf, len);

    if (ret < 0) {
        LOGE("cm36672_write error (%ld)\r\n", ret);
    }

    return ret;
}
/*----------------------------------------------------------------------------*/
static int cm36672_read(uint8_t reg, uint8_t *buf, uint32_t len)
{
    int32_t ret = 0;

    ret = mems_i2c_write_read(CM36672_I2C_ADDRESS, reg, buf, len);

    if (ret < 0) {
        LOGE("cm36672_read error (%ld)\r\n", ret);
    }

    return ret;
}

void cm36672_eint_set(uint32_t eint_num)
{
    cm36672_obj.eint_num = eint_num;
}

void cm36672_eint_handler(void *parameter)
{
    BaseType_t xHigherPriorityTaskWoken;

    hal_eint_mask(cm36672_obj.eint_num);
    hal_eint_unmask(cm36672_obj.eint_num);

    if (cm36672_obj.enable == 0){
        return;
    }

    cm36672_obj.ps_cmd_event.event = SM_EVENT_DATA_READY;

    cm36672_obj.ps_cmd_event.data_ready = (1U << SENSOR_TYPE_PROXIMITY);

    //cm36672_obj.ps_cmd_event.timestamp = sensor_driver_get_ms_tick();
    cm36672_obj.ps_cmd_event.length = 1; /* for debug */

    // Post the event.
    xQueueSendFromISR(sm_queue_handle, &cm36672_obj.ps_cmd_event, &xHigherPriorityTaskWoken);

    // Now the buffer is empty we can switch context if necessary.
    if ( xHigherPriorityTaskWoken ) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    }
}

uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) cm36672_txbuffer[3];
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) cm36672_rxbuffer[3];


/********************************************************************/
int cm36672_enable_ps(int enable)
{
    struct cm36672_priv *obj = &cm36672_obj;
    int ret;

    if (enable != 0) {

        // Set PS_THDL
        cm36672_txbuffer[0] = CM36672_REG_PS_LOW_THD;
        cm36672_txbuffer[1] = (uint8_t)PS_LOW_THRESHOLD;
        cm36672_txbuffer[2] = 0;
        ret = cm36672_write(cm36672_txbuffer, 3);
        if (ret < 0) {
            LOGE("cm36672_write err (%d) \r\n", ret);
            goto ENABLE_PS_EXIT_ERR;
        }

        // Set PS_THDH
        cm36672_txbuffer[0] = CM36672_REG_PS_HIGH_THD;
        cm36672_txbuffer[1] = (uint8_t)PS_HIGH_THRESHOLD;
        cm36672_txbuffer[2] = 0;
        ret = cm36672_write(cm36672_txbuffer, 3);
        if (ret < 0) {
            LOGE("cm36672_write err (%d)\r\n", ret);
            goto ENABLE_PS_EXIT_ERR;
        }

        ret = cm36672_read(CM36672_REG_PS_CONF1_2, cm36672_rxbuffer, 0x2);
        if (ret < 0) {
            LOGE("cm36672_read err (%d)\r\n", ret);
            goto ENABLE_PS_EXIT_ERR;
        }
        LOGI("CM36672_REG_PS_CONF1_2 0x%X0x%X\r\n", cm36672_rxbuffer[0], cm36672_rxbuffer[1]);

        //PS power on
        cm36672_txbuffer[0] = cm36672_rxbuffer[0] & 0xFE;
        // Enable PS_INT and Set PS_PERS to 2
        cm36672_txbuffer[1] = cm36672_rxbuffer[1] | 0x3;
        cm36672_txbuffer[0] = (cm36672_txbuffer[0] & 0xCF) | 0x10;

        cm36672_txbuffer[2] = cm36672_txbuffer[1];
        cm36672_txbuffer[1] = cm36672_txbuffer[0];
        cm36672_txbuffer[0] = CM36672_REG_PS_CONF1_2;

        ret = cm36672_write(cm36672_txbuffer, 3);
        if (ret < 0) {
            goto ENABLE_PS_EXIT_ERR;
        }

        ret = cm36672_read(CM36672_REG_PS_CONF1_2, cm36672_rxbuffer, 0x2);
        if (ret < 0) {
            goto ENABLE_PS_EXIT_ERR;
        }
        LOGI("CM36672_REG_PS_CONF1_2 0x%X0x%X\r\n", cm36672_rxbuffer[0], cm36672_rxbuffer[1]);

        /* LED_I default b'000 for 50 mA */
        ret = cm36672_read(CM36672_REG_PS_CONF3_MS, cm36672_rxbuffer, 0x2);
        if (ret < 0) {
            goto ENABLE_PS_EXIT_ERR;
        }
        LOGI("CM36672_REG_PS_CONF3_MS 0x%X0x%X\r\n", cm36672_rxbuffer[0], cm36672_rxbuffer[1]);

        ret = cm36672_read(CM36672_REG_PS_HIGH_THD, cm36672_rxbuffer, 0x2);
        if (ret < 0) {
            goto ENABLE_PS_EXIT_ERR;
        }
        LOGI("CM36672_REG_PS_HIGH_THD 0x%X0x%X\r\n", cm36672_rxbuffer[0], cm36672_rxbuffer[1]);

        ret = cm36672_read(CM36672_REG_PS_LOW_THD, cm36672_rxbuffer, 0x2);
        if (ret < 0) {
            goto ENABLE_PS_EXIT_ERR;
        }
        LOGI("CM36672_REG_PS_LOW_THD 0x%X0x%X\r\n", cm36672_rxbuffer[0], cm36672_rxbuffer[1]);

        obj->enable = 1;
    } else {
        LOGI("cm36672_enable_ps disable_ps\n");

        ret = cm36672_read(CM36672_REG_PS_CONF1_2, cm36672_rxbuffer, 0x2);
        if (ret < 0) {
            goto ENABLE_PS_EXIT_ERR;
        }

        cm36672_txbuffer[1] = cm36672_rxbuffer[1];
        cm36672_txbuffer[0] = cm36672_rxbuffer[0] | 0x01;
        cm36672_txbuffer[2] = cm36672_txbuffer[1];
        cm36672_txbuffer[1] = cm36672_txbuffer[0];
        cm36672_txbuffer[0] = CM36672_REG_PS_CONF1_2;

        ret = cm36672_write(cm36672_txbuffer, 3);
        if (ret < 0) {
            LOGE("cm36672_write err (%d)\r\n", ret);
            goto ENABLE_PS_EXIT_ERR;
        }

        obj->enable = 0;

    }

    return 0;
ENABLE_PS_EXIT_ERR:
    return ret;
}

/*--------------------------------------------------------------------------------*/
int32_t ps_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                   void *buff_in, int32_t size_in)
{
    int err = 0;
    int value;

    switch (command) {
        case SENSOR_DELAY:
            break;
        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("Enable sensor parameter error! \r\n");
                err = -1;
            } else {
                value = *(int *)buff_in;
                LOGI("cm36672 ps enable command (%d)! \r\n", value);
                if (value) {
                    if ((err = cm36672_enable_ps(1))) {
                        LOGE("enable ps fail: %d \r\n", err);
                        return -1;
                    }
                } else {
                    if ((err = cm36672_enable_ps(0))) {
                        LOGE("disable ps fail: %d \r\n", err);
                        return -1;
                    }
                }
            }

            break;
        case SENSOR_GET_DATA:
            if ((buff_out == NULL) || size_out != sizeof(sensor_data_unit_t)) {
                LOGE("SENSOR_GET_DATA parameter error \r\n");
                err = -1;
            } else {
                *actualout = sizeof(sensor_data_unit_t);
                /* polling mode to read PS_Data
                cm36672_read(CM36672_REG_PS_DATA, (uint8_t *)buff_out, sizeof(uint16_t)); */
                /* interrupt mode to read int flag*/

                cm36672_read(CM36672_REG_INT_FLAG, cm36672_rxbuffer, 2);
                if (cm36672_rxbuffer[1] & 1) {
                    /* PS_IF_AWAY */
                    cm36672_obj.distance = 1;
                } else if (cm36672_rxbuffer[1] & 2) {
                    /* PS_IF_CLOSE */
                    cm36672_obj.distance = 0;
                }
                sensor_data_unit_t *pdata = (sensor_data_unit_t *)buff_out;
                pdata->distance = cm36672_obj.distance;
            }
            break;
        case SENSOR_CUST:
            break;
        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

/*----------------------------------------------------------------------------*/
void ps_init(void)
{
    sensor_driver_object_t obj_ps;

    obj_ps.self = (void *)&obj_ps;
    obj_ps.polling = 0;	//default value 0
    obj_ps.sensor_operate = ps_operate;

    cm36672_obj.id = 0x86;

    int fail_cout = 0;
    cm36672_rxbuffer[0] = 0;
    while (cm36672_rxbuffer[0] != cm36672_obj.id) {
        /* read Chip Id */
        cm36672_read(CM36672_REG_ID_MODE, cm36672_rxbuffer, 0x2);
        LOGI("chip id == 0x%X\n", cm36672_rxbuffer[0]);
        if (cm36672_rxbuffer[0] == cm36672_obj.id) {
            break;
        }
        fail_cout++;
        if(fail_cout > 5) {
            return;
        }
        vTaskDelay(1000);
    }

    obj_ps.self = (void *)&obj_ps;
    obj_ps.polling = 0;	//default value 0
    obj_ps.sensor_operate = ps_operate;

    sensor_driver_attach(SENSOR_TYPE_PROXIMITY, &obj_ps);
}

