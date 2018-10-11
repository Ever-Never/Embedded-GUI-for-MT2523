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

#ifdef MTK_CTP_ENABLE

#include <string.h>
#include "ctp_i2c.h"
#include "ctp.h"
#include "assert.h"
#include "hal_log.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "memory_attribute.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"


//#define CTP_I2C_DELAY ctp_delay_us(1)     //set sw i2c delay 10us
#define CTP_I2C_DELAY                   ctp_sw_i2c_delay(1)

#define CTP_I2C_DATA_NUMBER_POLLING     8           /* if tx/rx data > CTP_I2C_DATA_NUMBER_POLLING use DMA mode*/
#define CTP_I2C_DATA_NUMBER_DMA         15          /* if tx/rx data > CTP_I2C_DATA_NUMBER_DMA, DMA transaction will be cut to several times*/

#define CTP_I2C_BUFFER_SIZE             16          /* tx/rx data size*/

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t ctp_i2c_tx_buffer[CTP_I2C_BUFFER_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t ctp_i2c_rx_buffer[CTP_I2C_BUFFER_SIZE];

ctp_i2c_config_t *ctp_i2c_state;

xSemaphoreHandle g_ctp_i2c_dma_semaphore = NULL;

bool ctp_i2c_dma_event_state;

extern uint8_t i2c_lock_sleep_handle[HAL_I2C_MASTER_MAX];



/***********************************************************************************
hardware I2C
***********************************************************************************/

bool ctp_hw_i2c_init(ctp_i2c_config_t *config)
{
    hal_i2c_config_t i2c_config;
    hal_i2c_status_t ret;

    ctp_i2c_state = config;                                     //pass configuration information to state

    if (ctp_i2c_state->has_initialized != true) {

        if (ctp_i2c_state->i2c_type == CTP_USE_HARDWARE_I2C) {

            i2c_config.frequency = ctp_i2c_state->speed;
            ret = hal_i2c_master_init(ctp_i2c_state->i2c_port, &i2c_config); // initialize i2c master.

            if (ret == HAL_I2C_STATUS_OK) {
                ctp_i2c_state->has_initialized = true;
                return true;
            } else {
                log_hal_info("[CTP][I2C]init I2C%d fail\r\n", ctp_i2c_state->i2c_port);
            }
        } else if (ctp_i2c_state->i2c_type == CTP_USE_SOFTWARE_I2C) {
            hal_gpio_set_direction(ctp_i2c_state->scl_pin, HAL_GPIO_DIRECTION_OUTPUT);
            hal_gpio_set_direction(ctp_i2c_state->sda_pin, HAL_GPIO_DIRECTION_OUTPUT);
            ctp_i2c_state->has_initialized = true;
            return true;

        } else {
            log_hal_info("[CTP][I2C]ctp_i2c_state->i2c_type error:%d\r\n", ctp_i2c_state->i2c_type);
            assert(0);

        }

    }

    ctp_i2c_state->has_initialized = false;

    return false;

}

//release hw i2c
bool ctp_hw_i2c_deinit(void)
{
    hal_i2c_status_t ret;

    if (ctp_i2c_state->has_initialized == true) {

        if (ctp_i2c_state->i2c_type == CTP_USE_HARDWARE_I2C) {

            ret = hal_i2c_master_deinit(ctp_i2c_state->i2c_port);
            if (HAL_I2C_STATUS_OK != ret) {
                ctp_debug_printf("[CTP][I2C]deinit I2C%d fail\r\n", ctp_i2c_state->i2c_port);
                return false;
            }
        } else if (ctp_i2c_state->i2c_type == CTP_USE_SOFTWARE_I2C) {
            hal_gpio_get_direction(ctp_i2c_state->scl_pin, HAL_GPIO_DIRECTION_INPUT);
            hal_gpio_get_direction(ctp_i2c_state->sda_pin, HAL_GPIO_DIRECTION_INPUT);
        } else {
            log_hal_info("[CTP][I2C]ctp_i2c_state->i2c_type error:%d\r\n", ctp_i2c_state->i2c_type);
            assert(0);
        }

    }

    ctp_i2c_state->has_initialized = false;

    return true;
}
/***********************************************************************************
software I2C
***********************************************************************************/

void ctp_sw_i2c_delay(uint32_t count)
{
    while (count > 0) {
        count--;
    }

}
void gpio_sda_set_output(uint8_t high)
{
    hal_gpio_pin_t gpio_port;

    gpio_port = ctp_i2c_state->sda_pin;

    hal_gpio_set_direction(gpio_port, HAL_GPIO_DIRECTION_OUTPUT);

    if (high != 0) {
        hal_gpio_set_output(gpio_port, HAL_GPIO_DATA_HIGH);
    } else {
        hal_gpio_set_output(gpio_port, HAL_GPIO_DATA_LOW);
    }
}

void gpio_sda_set_input(void)
{
    hal_gpio_pin_t  gpio_port;

    gpio_port = ctp_i2c_state->sda_pin;

    hal_gpio_set_direction(gpio_port, HAL_GPIO_DIRECTION_INPUT);
}

uint8_t gpio_sda_get_input(void)
{
    hal_gpio_data_t ret;
    hal_gpio_get_input(ctp_i2c_state->sda_pin, &ret);
    return (uint8_t)ret;

}


void gpio_scl_set_output(uint8_t high)
{
    hal_gpio_pin_t gpio_port;

    gpio_port = ctp_i2c_state->scl_pin;

    hal_gpio_set_direction(gpio_port, HAL_GPIO_DIRECTION_OUTPUT);

    if (high != 0) {
        hal_gpio_set_output(gpio_port, HAL_GPIO_DATA_HIGH);
    } else {
        hal_gpio_set_output(gpio_port, HAL_GPIO_DATA_LOW);
    }
}


void ctp_sw_i2c_start(void)
{
    gpio_sda_set_output(1);
    gpio_scl_set_output(1);
    CTP_I2C_DELAY;
    gpio_sda_set_output(0);
    CTP_I2C_DELAY;
    gpio_scl_set_output(0);
    CTP_I2C_DELAY;
}

void ctp_sw_i2c_stop(void)
{
    gpio_scl_set_output(0);
    gpio_sda_set_output(0);
    CTP_I2C_DELAY;
    gpio_scl_set_output(1);
    CTP_I2C_DELAY;
    gpio_sda_set_output(1);
    CTP_I2C_DELAY;
}

uint8_t ctp_sw_i2c_read_ack(void)
{
    uint8_t ret;

    gpio_sda_set_input();

    gpio_scl_set_output(1);
    CTP_I2C_DELAY;
    ret = gpio_sda_get_input();

    gpio_scl_set_output(0);
    CTP_I2C_DELAY;

    return ret;
}

void ctp_sw_i2c_send_ack(uint8_t ack)
{
    if (ack != 0) {
        gpio_sda_set_output(1);
    } else {
        gpio_sda_set_output(0);
    }
    CTP_I2C_DELAY;
    gpio_scl_set_output(1);
    CTP_I2C_DELAY;
    gpio_scl_set_output(0);
    CTP_I2C_DELAY;
}

uint8_t ctp_sw_i2c_send_byte(uint8_t send_byte)
{
    uint8_t ret      = 0;
    uint8_t i;
    uint8_t out_mask = 0x80;

    for (i = 0; i < 8; i++) {

        if (send_byte & out_mask) {
            gpio_sda_set_output(1);
        } else {
            gpio_sda_set_output(0);
        }
        CTP_I2C_DELAY;
        gpio_scl_set_output(1);
        CTP_I2C_DELAY;
        gpio_scl_set_output(0);
        CTP_I2C_DELAY;

        out_mask = out_mask >> 1;
    }

    gpio_sda_set_output(1);

    ret = ctp_sw_i2c_read_ack();

    return ret;
}

void ctp_sw_i2c_read_byte(uint8_t *buffer, uint8_t ack)
{
    uint8_t ret = 0;
    uint8_t i;
    uint8_t data = 0;

    gpio_sda_set_input();

    for (i = 0 ; i < 8; i++) {
        gpio_scl_set_output(1);
        CTP_I2C_DELAY;

        ret  = gpio_sda_get_input();
        data = data << 1;
        if (ret != 0) {
            data |= 0x01;
        }
        gpio_scl_set_output(0);
        CTP_I2C_DELAY;
    }

    ctp_sw_i2c_send_ack(ack);

    *buffer = data;
}

bool ctp_sw_i2c_write(uint8_t slave_adress, uint16_t reg_address, uint8_t *data, uint32_t length)
{
    uint8_t  ret = 0;
    uint32_t i;

    ctp_sw_i2c_start();

    ret |= ctp_sw_i2c_send_byte(slave_adress & 0xfe);
    ret |= ctp_sw_i2c_send_byte((reg_address >> 8) & 0xff);
    ret |= ctp_sw_i2c_send_byte(reg_address & 0xff);

    if ((data == NULL) || (length == 0)) {
        ctp_sw_i2c_stop();
        return false;
    }

    for (i = 0; i < length; i++) {

        ret |= ctp_sw_i2c_send_byte(*data);
        data++;
    }

    ctp_sw_i2c_stop();

    if (ret != 0) {

        return false;
    } else {
        return true;
    }
}


bool ctp_sw_i2c_read(uint8_t slave_adress, uint16_t reg_address, uint8_t *data, uint32_t length)
{
    uint8_t  ret = 0;
    uint32_t i;

    ctp_sw_i2c_start();

    ret |= ctp_sw_i2c_send_byte(slave_adress & 0xfe);
    ret |= ctp_sw_i2c_send_byte((reg_address >> 8) & 0xff);
    ret |= ctp_sw_i2c_send_byte(reg_address & 0xff);
    ctp_sw_i2c_start();
    ret |= ctp_sw_i2c_send_byte(slave_adress | 0x01);

    if ((data == NULL) || (length == 0)) {
        ctp_sw_i2c_stop();
        return false;
    }

    for (i = 0; i < length; i++) {

        ctp_sw_i2c_read_byte(data, !(length - i - 1)); //!(length - i - 1) ensure the last byte is nACK.
        data++;
    }
    ctp_sw_i2c_stop();

    if (ret != 0) {

        return false;
    } else {
        return true;
    }
}

/***********************************************************************************
I2C interface for CTP use
************************************************************************************/
void ctp_i2c_dma_complete(uint8_t slave_address, hal_i2c_callback_event_t event, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken;

    //log_hal_info("enter i2c callback\r\n");
    if (HAL_I2C_EVENT_SUCCESS == event) {
        ctp_i2c_dma_event_state = true;
    } else {
        ctp_i2c_dma_event_state = false;
    }

    xSemaphoreGiveFromISR(g_ctp_i2c_dma_semaphore, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void ctp_i2c_translate_address_to_array(uint16_t reg_addr, uint8_t *array)
{
    /*get send data*/
    if (ctp_config_info.reg_address_byte == 2) {
        array[0] = (reg_addr >> 8) & 0xFF;
        array[1] = reg_addr & 0xFF;
    } else if (ctp_config_info.reg_address_byte == 1) {
        array[0] = reg_addr & 0xFF;
    } else {
        log_hal_info("[CTP][I2C]read ctp_config_info.reg_address_byte =%d error\r\n", ctp_config_info.reg_address_byte);
        assert(0);
    }
}

bool ctp_i2c_bottom_write(uint8_t device_addr, uint16_t ctp_reg_addr, uint8_t *data_buffer, uint32_t data_length)
{
    bool        ret;
    uint8_t     write_buf[10];
    uint8_t     pkt_len;
    uint32_t    offset = 0;
    uint32_t    i;
    hal_i2c_status_t status;


    if (ctp_i2c_state->i2c_type == CTP_USE_HARDWARE_I2C) {   //HW i2c
        if (ctp_i2c_state->has_initialized == true) {
            while (offset <= data_length) {
                if (ctp_config_info.reg_address_byte == 2) {
                    write_buf[0] = ((ctp_reg_addr + offset) >> 8) & 0xFF;
                    write_buf[1] = (ctp_reg_addr + offset) & 0xFF;
                } else if (ctp_config_info.reg_address_byte == 1) {
                    write_buf[0] = (ctp_reg_addr + offset) & 0xFF;
                } else {
                    log_hal_info("[CTP][I2C]write ctp_config_info.reg_address_byte =%d error\r\n", ctp_config_info.reg_address_byte);
                    assert(0);
                }


                if (data_length - offset > (8 - ctp_config_info.reg_address_byte)) {
                    pkt_len = (8 - ctp_config_info.reg_address_byte);
                } else {
                    pkt_len = data_length - offset;
                }

                memcpy(&write_buf[ctp_config_info.reg_address_byte], &data_buffer[offset], pkt_len);
                offset += pkt_len;

                for (i = 0; i < 10; i++) {
                    status = hal_i2c_master_send_polling(ctp_i2c_state->i2c_port, (device_addr >> 1), write_buf, pkt_len + ctp_config_info.reg_address_byte); //send data
                    if (status == HAL_I2C_STATUS_ERROR_BUSY) {
                        ctp_delay_ms(2);
                        continue;
                    } else {
                        break;
                    }
                }

                if (status != HAL_I2C_STATUS_OK) {
                    log_hal_info("[CTP][I2C]return err status = %d\r\n", status);
                    return false;
                }
                if (offset == data_length) {
                    break;
                }
            }
        }
    } else if (ctp_i2c_state->i2c_type == CTP_USE_SOFTWARE_I2C) {
        ret = ctp_sw_i2c_write(device_addr, ctp_reg_addr, data_buffer, data_length);
        if (ret != true) {
            return false;
        }
    } else {

        return false;
    }

    return true;
}

bool ctp_i2c_bottom_read(uint8_t device_addr, uint16_t ctp_reg_addr, uint8_t *data_buffer, uint32_t data_length)
{
    uint16_t reg_addr;
    uint32_t offset = 0;
    uint8_t  pkt_len;
    hal_i2c_status_t status;
    uint32_t i;
    bool    ret;

    hal_i2c_send_to_receive_config_t i2c_send_to_receive_config;
    hal_i2c_send_to_receive_config_ex_t ex_config;

    reg_addr = ctp_reg_addr;

    if (ctp_i2c_state->i2c_type == CTP_USE_HARDWARE_I2C) {    //HW i2c
        if (ctp_i2c_state->has_initialized == true) {

            if (data_length <= 8) { /* data length <= 8 use I2C polling mode*/

                /* convert register adress to array*/
                ctp_i2c_translate_address_to_array(ctp_reg_addr, (uint8_t *)(&ctp_i2c_tx_buffer));

                i2c_send_to_receive_config.receive_buffer = ctp_i2c_rx_buffer;
                i2c_send_to_receive_config.receive_length = data_length;
                i2c_send_to_receive_config.send_data      = ctp_i2c_tx_buffer;
                i2c_send_to_receive_config.send_length    = ctp_config_info.reg_address_byte;
                i2c_send_to_receive_config.slave_address  = (device_addr >> 1);

                for (i = 0; i < 10; i++) {
                    status = hal_i2c_master_send_to_receive_polling(ctp_i2c_state->i2c_port, &i2c_send_to_receive_config);
                    if (status == HAL_I2C_STATUS_ERROR_BUSY) {
                        ctp_delay_ms(2);
                        continue;
                    } else {
                        break;
                    }
                }

                if ((status != HAL_I2C_STATUS_OK)) {
                    log_hal_info("[CTP][I2C]return err status = %d\r\n", status);
                    return false;
                }

                memcpy(data_buffer, &ctp_i2c_rx_buffer, data_length);

            } else { /**/
                while (offset < data_length) {

                    /* convert register adress to array*/
                    ctp_i2c_translate_address_to_array(reg_addr, (uint8_t *)(&ctp_i2c_tx_buffer));

                    /* one transaction 15 bytes*/
                    if (data_length - offset > 15) {
                        pkt_len = 15;
                    } else {
                        pkt_len = data_length - offset;
                    }

                    ex_config.slave_address = device_addr >> 1;
                    ex_config.send_data = ctp_i2c_tx_buffer;
                    ex_config.receive_buffer = ctp_i2c_rx_buffer;
                    ex_config.send_packet_length = 1;
                    ex_config.send_bytes_in_one_packet = ctp_config_info.reg_address_byte;
                    ex_config.receive_packet_length = 1;
                    ex_config.receive_bytes_in_one_packet = pkt_len;

                    if (HAL_I2C_STATUS_OK != hal_i2c_master_register_callback(ctp_i2c_state->i2c_port, ctp_i2c_dma_complete, NULL)) {
                        log_hal_info("[CTP][I2C]register callback fail\r\n");
                        return false;
                    }

                    if (HAL_I2C_STATUS_OK != hal_i2c_master_send_to_receive_dma_ex(ctp_i2c_state->i2c_port, &ex_config)) {
                        log_hal_info("[CTP][I2C]hal_i2c_master_send_to_receive_dma_ex fail\r\n");
                        return false;
                    }

                    /* wait 50ms timeout to receive i2c data*/
                    if (pdFALSE == xSemaphoreTake(g_ctp_i2c_dma_semaphore, (TickType_t)(50 / portTICK_PERIOD_MS))) {
                        log_hal_info("[CTP][I2C]xSemaphoreTake timeout.\r\n");
						hal_sleep_manager_unlock_sleep(i2c_lock_sleep_handle[0]);
                        return false;
                    }

                    if (ctp_i2c_dma_event_state == false) {
                        return false;
                    }
                    memcpy(data_buffer, &ctp_i2c_rx_buffer, pkt_len);

                    offset   += pkt_len;
                    reg_addr  = ctp_reg_addr + offset;
                }

            }

        }
    } else if (ctp_i2c_state->i2c_type == CTP_USE_SOFTWARE_I2C) {
        ret = ctp_sw_i2c_read(device_addr, ctp_reg_addr, data_buffer, data_length);
        if (ret != true) {
            return false;
        }
    } else {

        return false;
    }

    return true;
}

#endif /*MTK_CTP_ENABLE*/

