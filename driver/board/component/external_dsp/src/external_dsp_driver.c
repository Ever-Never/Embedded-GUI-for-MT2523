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

/* Includes ------------------------------------------------------------------*/
//#include <stdint.h>
//#include <stdio.h>
#include <string.h>


#include "hal.h"
#include "hal_log.h"
#include "memory_attribute.h"
#include "external_dsp_driver.h"
#include "hal_platform.h"

#define  DSP_SPI_PORT    HAL_SPI_MASTER_3
#define  DSP_SPI_BIT_ORDER  HAL_SPI_MASTER_MSB_FIRST

#define HIGH_SPEED

#ifdef HIGH_SPEED
#define  DSP_SPI_FREQUENCY          (13000000)
#else
#define  DSP_SPI_FREQUENCY          (1000000)
#endif

#define  DSP_SPI_CMD_DM_WR             (0x05)
#define  DSP_SPI_CMD_DM_RD             (0x01)
#define  DSP_SPI_CMD_IM_WR             (0x04)
#define  DSP_SPI_CMD_IM_RD             (0x00)
#define  DSP_SPI_CMD_REG_WR            (0x06)
#define  DSP_SPI_CMD_REG_RD            (0x02)


#define  DSP_I2C_SPI_REG                (0x12)
#define  DSP_RESET_REG                  (0x0)

#define  DSP_SPI_BUF_LEN               (39040)

#define  DSP_SPI_SEND_LEN              (180*1024)      // for receive data, this can be set to 0
#define  DSP_SPI_RECEIVE_LEN           (180*1024)
#define  F_DSP_SPI_SEND_LEN            (DSP_SPI_BUF_LEN + 6)
#define  F_DSP_SPI_RECEIVE_LEN         (DSP_SPI_BUF_LEN + 6)

volatile uint32_t dsp_spi_dma_flag = 0;

/** @brief DSP firmware type */
typedef enum {

    DSP_FIRMWARE = 0,
    EFT_FIRMWARE = 1
} dsp_firmware_typ_t;

#ifndef __ICCARM__
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__((aligned(4))) dsp_send_buffer[F_DSP_SPI_SEND_LEN];
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__((aligned(4))) dsp_send_buffer_bak[F_DSP_SPI_SEND_LEN];
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__((aligned(4))) dsp_receive_buffer[F_DSP_SPI_RECEIVE_LEN];
#else
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  dsp_send_buffer[F_DSP_SPI_SEND_LEN];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  dsp_send_buffer_bak[F_DSP_SPI_SEND_LEN];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  dsp_receive_buffer[F_DSP_SPI_RECEIVE_LEN];
#endif



static void dsp_spi_dma_complete(void)
{
    dsp_spi_dma_flag = 1;
}

external_dsp_status_t external_dsp_spi_init(void)
{
    hal_spi_master_config_t spi_config;
    hal_spi_master_status_t spi_status;
#ifdef HIGH_SPEED
    hal_spi_master_advanced_config_t advanced_config;
#endif

    spi_config.bit_order = (hal_spi_master_bit_order_t)DSP_SPI_BIT_ORDER;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = DSP_SPI_FREQUENCY;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(DSP_SPI_PORT, &spi_config)) {
        log_hal_error("SPI master init fail");
        return  EXTERNAL_DSP_STATUS_ERROR;
    }

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_set_macro_selection(DSP_SPI_PORT, HAL_SPI_MASTER_MACRO_GROUP_B)) {
        log_hal_error("SPI master set macro fail");
        return  EXTERNAL_DSP_STATUS_ERROR;
    }
#ifdef HIGH_SPEED
    log_hal_info("Speed is %d", DSP_SPI_FREQUENCY);
    advanced_config.get_tick = HAL_SPI_MASTER_GET_TICK_DELAY1;
    advanced_config.byte_order = HAL_SPI_MASTER_LITTLE_ENDIAN;
    advanced_config.chip_polarity = HAL_SPI_MASTER_CHIP_SELECT_LOW;
    advanced_config.sample_select = HAL_SPI_MASTER_SAMPLE_POSITIVE;
    spi_status = hal_spi_master_set_advanced_config(DSP_SPI_PORT, &advanced_config);
    if (HAL_SPI_MASTER_STATUS_OK != spi_status) {
        log_hal_error("SPI master advanced config fail, reason: %d", spi_status);
        return  EXTERNAL_DSP_STATUS_ERROR;
    }
#endif
    return EXTERNAL_DSP_STATUS_OK;

}

external_dsp_status_t external_dsp_spi_deinit(void)
{
    hal_spi_master_status_t status = hal_spi_master_deinit(DSP_SPI_PORT);
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_error(" SPI master deinit fail");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_read_register(uint8_t register_address, uint8_t *value)
{
    hal_spi_master_status_t spi_status = HAL_SPI_MASTER_STATUS_OK;
    uint8_t write_buf[2];
    uint8_t read_buf[3];
    hal_spi_master_send_and_receive_config_t cfg;

    write_buf[0] = DSP_SPI_CMD_REG_RD;
    write_buf[1] = register_address & 0xff;

    cfg.send_data = write_buf;
    cfg.send_length = 2;
    cfg.receive_buffer = read_buf;
    cfg.receive_length = 3;

    spi_status = hal_spi_master_send_and_receive_polling(DSP_SPI_PORT, &cfg);
    if (spi_status != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("[dsp_spi_read_reg] fail reason: %d", spi_status);
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    *value = read_buf[2];

    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_write_register(uint8_t register_address, uint8_t value)
{
    hal_spi_master_status_t spi_status = HAL_SPI_MASTER_STATUS_OK;
    uint8_t write_buf[3];

    write_buf[0] = DSP_SPI_CMD_REG_WR;
    write_buf[1] = register_address;
    write_buf[2] = value;

    spi_status = hal_spi_master_send_polling(DSP_SPI_PORT, write_buf, 3);
    if (spi_status != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("[dsp_spi_write_reg] fail reason: %d", spi_status);
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_read_dram(uint32_t address, uint32_t *data)
{
    hal_spi_master_status_t spi_status = HAL_SPI_MASTER_STATUS_OK;
    uint8_t i;
    uint8_t write_buf[6];
    uint8_t read_buf[10];
    uint8_t spi_cmd = DSP_SPI_CMD_IM_RD;
    uint8_t addr_msb;
    uint8_t temp_data[4] = {0};
    hal_spi_master_send_and_receive_config_t cfg;

    addr_msb = (address >> 24) & 0xff;
    if (addr_msb == 0x10) {
        spi_cmd = DSP_SPI_CMD_IM_RD;
    } else if (addr_msb == 0xF) {
        spi_cmd = DSP_SPI_CMD_DM_RD;
    }

    write_buf[0] = spi_cmd;
    write_buf[1] = address & 0xFF;
    write_buf[2] = (address >> 8) & 0xFF;
    write_buf[3] = (address >> 16) & 0xFF;
    write_buf[4] = 2; //(4>>1)
    write_buf[5] = 0;

    cfg.send_data = write_buf;
    cfg.send_length = 6;
    cfg.receive_buffer = read_buf;
    cfg.receive_length = 10;

    spi_status = hal_spi_master_send_and_receive_polling(DSP_SPI_PORT, &cfg);
    if (spi_status != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("[dsp_spi_read_dram] fail reason: %d", spi_status);
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    for (i = 0; i < 4; i++) {
        temp_data[i] = *(read_buf + 6 + i);
    }
    *data = 0;
    *data = temp_data[0] | (temp_data[1] << 8) | (temp_data[2] << 16) | (temp_data[3] << 24);
//    log_hal_info("[dsp_spi_read_dram] data(0~3) is %x, %x, %x, %x", temp_data[0],temp_data[1],temp_data[2], temp_data[3] );

    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_write_dram(uint32_t address, uint32_t data)
{
    hal_spi_master_status_t spi_status = HAL_SPI_MASTER_STATUS_OK;
    uint8_t buf[10];

    buf[0] =  DSP_SPI_CMD_DM_WR;
    buf[1] =  address & 0xFF;
    buf[2] = (address >> 8) & 0xFF;
    buf[3] = (address >> 16) & 0xFF;
    buf[4] =  2; //(4>>1)
    buf[5] =  0;

    buf[6] =  data & 0xFF;
    buf[7] = (data >> 8) & 0xFF;
    buf[8] = (data >> 16) & 0xFF;
    buf[9] = (data >> 24) & 0xFF;

    spi_status = hal_spi_master_send_polling(DSP_SPI_PORT, buf, 10);
    if (spi_status != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("[dsp_spi_write_dram] fail reason: %d", spi_status);
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    return EXTERNAL_DSP_STATUS_OK;
}

static external_dsp_status_t external_dsp_burst_internal_write(uint32_t address, const uint8_t *tx_buffer, uint32_t length, dsp_firmware_typ_t firmware_type)
{
    hal_spi_master_status_t spi_status = HAL_SPI_MASTER_STATUS_OK;
    uint8_t spi_cmd = DSP_SPI_CMD_IM_RD;
    uint8_t addr_msb;
    uint8_t *local_buf, *p_send_buffer;
    uint32_t i;
    uint32_t local_len;
    uint32_t end;
    uint32_t offset = 0;
    int32_t local_status = 0, buffer_select;

    local_buf = (uint8_t *)tx_buffer;

    if (length > DSP_SPI_SEND_LEN) {
        local_status = -10;
        log_hal_error("[dsp_spi_burst_write] fail reason: %d", local_status);
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    local_len = (length % 8) ? (((length / 8) + 1) * 8) : length;

    addr_msb = (address >> 24) & 0xff;
    if (addr_msb == 0x10) {
        spi_cmd = DSP_SPI_CMD_IM_WR;
    } else if (addr_msb == 0xF) {
        spi_cmd = DSP_SPI_CMD_DM_WR;
    }

    hal_spi_master_register_callback(DSP_SPI_PORT, (hal_spi_master_callback_t)dsp_spi_dma_complete, NULL);

    dsp_send_buffer[0] = spi_cmd;
    dsp_send_buffer_bak[0] = spi_cmd;
    buffer_select = 0;
    dsp_spi_dma_flag = 1;
    while (offset < local_len) {
        if (offset + DSP_SPI_BUF_LEN <= local_len) {
            end = DSP_SPI_BUF_LEN;
        } else {
            end = local_len % DSP_SPI_BUF_LEN;

        }

        if (buffer_select == 0) {
            p_send_buffer = dsp_send_buffer;
            buffer_select = 1;
        } else {
            p_send_buffer = dsp_send_buffer_bak;
            buffer_select = 0;
        }

        p_send_buffer[1] = ((address + offset) & 0x000000ff) >> 0;
        p_send_buffer[2] = ((address + offset) & 0x0000ff00) >> 8;
        p_send_buffer[3] = ((address + offset) & 0x00ff0000) >> 16;
        p_send_buffer[4] = ((end / 2) & 0x00ff) >> 0;       //The word counter low byte.
        p_send_buffer[5] = ((end / 2) & 0xff00) >> 8;       //Assign the high order word counter, since it is fixed.

        if (firmware_type == DSP_FIRMWARE) {
            for (i = 0; i < end; i += 8) {
                p_send_buffer[i + 13] = local_buf[offset + i + 0];
                p_send_buffer[i + 12] = local_buf[offset + i + 1];
                p_send_buffer[i + 11] = local_buf[offset + i + 2];
                p_send_buffer[i + 10] = local_buf[offset + i + 3];
                p_send_buffer[i + 9] = local_buf[offset + i + 4];
                p_send_buffer[i + 8] = local_buf[offset + i + 5];
                p_send_buffer[i + 7] = local_buf[offset + i + 6];
                p_send_buffer[i + 6] = local_buf[offset + i + 7];
            }
        } else if (firmware_type == EFT_FIRMWARE) {
            for (i = 0; i < end; i += 8) {
                p_send_buffer[i + 6] = local_buf[offset + i + 0];
                p_send_buffer[i + 7] = local_buf[offset + i + 1];
                p_send_buffer[i + 8] = local_buf[offset + i + 2];
                p_send_buffer[i + 9] = local_buf[offset + i + 3];
                p_send_buffer[i + 10] = local_buf[offset + i + 4];
                p_send_buffer[i + 11] = local_buf[offset + i + 5];
                p_send_buffer[i + 12] = local_buf[offset + i + 6];
                p_send_buffer[i + 13] = local_buf[offset + i + 7];
            }
        }

        while (dsp_spi_dma_flag == 0);
        dsp_spi_dma_flag = 0;

        //hal_gpt_delay_ms(5);

        spi_status = hal_spi_master_send_dma(DSP_SPI_PORT, p_send_buffer, (end + 6));
        if (spi_status != HAL_SPI_MASTER_STATUS_OK) {
            log_hal_error("[dsp_spi_burst_write] fail reason: %d", spi_status);
            return EXTERNAL_DSP_STATUS_ERROR;
        }

        offset += DSP_SPI_BUF_LEN;
    }

    while (dsp_spi_dma_flag == 0);
    dsp_spi_dma_flag = 0;

    return EXTERNAL_DSP_STATUS_OK;
}

/**
 * external_dsp_burst_read - Read data from SPI by dsp dsp memory address.
 * @addr: Start address.
 * @rxbuf: Data Buffer for reading.
 * @len: Data length, it must be a multiple of 8.
 *
 * Returns true for success.
 */
external_dsp_status_t external_dsp_burst_read(uint32_t address, uint8_t *rx_buffer, uint32_t length)
{
    hal_spi_master_status_t spi_status = HAL_SPI_MASTER_STATUS_OK;
    uint8_t spi_cmd = DSP_SPI_CMD_IM_RD;
    uint8_t addr_msb;
    hal_spi_master_send_and_receive_config_t cfg;
    uint32_t end;
    uint32_t offset = 0;
    int32_t local_status = 0;

    if (length > DSP_SPI_RECEIVE_LEN) {
        local_status = -10;
        log_hal_error("[dsp_spi_burst_read_dram] fail reason: %d", local_status);
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    addr_msb = (address >> 24) & 0xff;
    if (addr_msb == 0x10) {
        spi_cmd = DSP_SPI_CMD_IM_RD;
    } else if (addr_msb == 0xF) {
        spi_cmd = DSP_SPI_CMD_DM_RD;
    }

    hal_spi_master_register_callback(DSP_SPI_PORT, (hal_spi_master_callback_t)dsp_spi_dma_complete, NULL);
    dsp_send_buffer[0] = spi_cmd;
    while (offset < length) {
        if (offset + DSP_SPI_BUF_LEN <= length) {
            end = DSP_SPI_BUF_LEN;
        } else {
            end = length % DSP_SPI_BUF_LEN;
        }

        dsp_send_buffer[1] = ((address + offset) & 0x000000ff) >> 0;
        dsp_send_buffer[2] = ((address + offset) & 0x0000ff00) >> 8;
        dsp_send_buffer[3] = ((address + offset) & 0x00ff0000) >> 16;
        dsp_send_buffer[4] = (end >> 1) & 0xff;
        dsp_send_buffer[5] = (end >> (1 + 8)) & 0xff;

        cfg.send_data = dsp_send_buffer;
        cfg.send_length = 6;
        cfg.receive_buffer = dsp_receive_buffer;
        cfg.receive_length = (6 + end);

        spi_status = hal_spi_master_send_and_receive_dma(DSP_SPI_PORT, &cfg);
        if (spi_status != HAL_SPI_MASTER_STATUS_OK) {
            log_hal_error("[dsp_spi_burst_read_dram] fail reason: %d", spi_status);
            return EXTERNAL_DSP_STATUS_ERROR;
        }

#if 0
        log_hal_info("before while [read]dsp_spi_dma_flag: %x", dsp_spi_dma_flag); //keep this for status check
#endif

        while (dsp_spi_dma_flag == 0);

        dsp_spi_dma_flag = 0;
#if 0
        log_hal_info("after while [read]dsp_spi_dma_flag: %x", dsp_spi_dma_flag); //keep this for status check
#endif
        hal_gpt_delay_ms(5);

        memcpy(&(rx_buffer[offset]), &(dsp_receive_buffer[6]), end);
        memset(dsp_receive_buffer, 0 , F_DSP_SPI_RECEIVE_LEN);

        offset += DSP_SPI_BUF_LEN;
    }
    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_download_firmware_binary(uint32_t address, const uint8_t *tx_buffer, uint32_t length)
{
    external_dsp_status_t status;

    if (length % 8) {
        log_hal_error(" firmware binary size isn't the multiple of 8");
    }
    status = external_dsp_burst_internal_write(address, tx_buffer, length, DSP_FIRMWARE);
    return status;
}

/**
 * external_dsp_burst_write - Write data to SPI by dsp dsp memory address.
 * @addr: Start address.
 * @txbuf: Data Buffer for writng.
 * @len: Data length, it must be a multiple of 8.
 * @type: Firmware type, MSB and LSB, the iM501 firmware is in MSB, but the EFT firmware is in LSB.
 *
 * Returns true for success.
 */

external_dsp_status_t external_dsp_burst_write(uint32_t address, const uint8_t *tx_buffer, uint32_t length)
{
    external_dsp_status_t status;

    status = external_dsp_burst_internal_write(address, tx_buffer, length, EFT_FIRMWARE);
    return status;
}

external_dsp_status_t  external_dsp_exit_download_mode(void)
{
    /* put DSP on run state*/
    uint8_t read_data = 0;
    external_dsp_status_t status = external_dsp_write_register(DSP_RESET_REG, 0x04);
    if (status != EXTERNAL_DSP_STATUS_OK) {
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    status = external_dsp_read_register(DSP_RESET_REG, &read_data);
    if (status != EXTERNAL_DSP_STATUS_OK) {
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    log_hal_info("read DSP hold status is %x, 0x04 is expected ", read_data);
    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_enter_download_mode(void)
{
    uint8_t  read_data;
// warm reset
    external_dsp_status_t status = external_dsp_write_register(DSP_RESET_REG, 0x07);
    if (status != EXTERNAL_DSP_STATUS_OK) {
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    status = external_dsp_write_register(DSP_RESET_REG, 0x05);
    if (status != EXTERNAL_DSP_STATUS_OK) {
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    hal_gpt_delay_ms(10);
    external_dsp_read_register(DSP_RESET_REG, &read_data);
    log_hal_info("read DSP hold status is %x", read_data);
    return EXTERNAL_DSP_STATUS_OK;
}

void external_dsp_power_on(bool on)
{
#ifndef MTK_AUTOMOTIVE_SUPPORT //GPIO22 is used by uart for ime project
    hal_gpio_status_t   gpio_status;
    hal_pinmux_status_t status;
    hal_gpio_pin_t      gpio_pin = (hal_gpio_pin_t)EXT_DSP_POWER_SW_PIN;

    log_hal_info("gpio=%d, on=%x", gpio_pin, on);

    gpio_status = hal_gpio_init(gpio_pin);
    if (gpio_status != HAL_GPIO_STATUS_OK) {
        return;
    }
    status = hal_pinmux_set_function(gpio_pin, EXT_DSP_POWER_SW_PIN_M_GPIO);
    if (status != HAL_PINMUX_STATUS_OK) {
        return;
    }
    hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_OUTPUT);
    if (on) {
        hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_HIGH);
    } else {
        hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_LOW);
    }
#endif
}

void external_dsp_clk_sw_config(bool pdm_clk)
{
    hal_gpio_status_t   gpio_status;
    hal_pinmux_status_t status;
    hal_gpio_pin_t      gpio_pin = (hal_gpio_pin_t)EXT_DSP_CLK_SW_PIN;

    log_hal_info("gpio=%d, pdm_clk=%x", gpio_pin, pdm_clk);

    gpio_status = hal_gpio_init(gpio_pin);
    if (gpio_status != HAL_GPIO_STATUS_OK) {
        return;
    }
    status = hal_pinmux_set_function(gpio_pin, EXT_DSP_CLK_SW_PIN_M_GPIO);
    if (status != HAL_PINMUX_STATUS_OK) {
        return;
    }
    hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_OUTPUT);
    if (pdm_clk) {
        hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_HIGH);
    } else {
        hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_LOW);
    }
}

void external_dsp_rtc_sw_config(bool rtc_clk)
{
    hal_gpio_status_t   gpio_status;
    hal_pinmux_status_t status;
    hal_gpio_pin_t      gpio_pin = (hal_gpio_pin_t)EXT_DSP_RTC_CLK_SW_PIN;

    log_hal_info("gpio=%d, rtc_clk=%x", gpio_pin, rtc_clk);
    gpio_status = hal_gpio_init(gpio_pin);
    if (gpio_status != HAL_GPIO_STATUS_OK) {
        return;
    }
    status = hal_pinmux_set_function(gpio_pin, EXT_DSP_RTC_CLK_SW_PIN_M_GPIO);
    if (status != HAL_PINMUX_STATUS_OK) {
        return;
    }
    hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_OUTPUT);
    if (rtc_clk) {
        hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_HIGH);
    } else {
        hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_LOW);
    }
}

#ifdef MTK_AUTOMOTIVE_SUPPORT
void external_dsp_echo_ref_sw_config(bool bt_device)
{
    hal_gpio_status_t   gpio_status;
    hal_pinmux_status_t status;
    hal_gpio_pin_t      gpio_pin = HAL_GPIO_4;
    uint8_t             function_index = HAL_GPIO_4_GPIO4;

    log_hal_info("gpio=%d, bt_device=%x", gpio_pin, bt_device);
    gpio_status = hal_gpio_init(gpio_pin);
    if (gpio_status != HAL_GPIO_STATUS_OK) {
        return;
    }
    status = hal_pinmux_set_function(gpio_pin, function_index);
    if (status != HAL_PINMUX_STATUS_OK) {
        return;
    }

    hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_OUTPUT);
    if (bt_device) {
        hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_HIGH);
    } else {
        hal_gpio_set_output(gpio_pin, HAL_GPIO_DATA_LOW);
    }

}
#endif

void external_dsp_spi_pinmux_switch(bool spi_mode)
{
    hal_gpio_status_t   gpio_status;

    log_hal_info("spi_mode=%x", spi_mode);
    gpio_status = hal_gpio_init((hal_gpio_pin_t)HAL_SPI_MASTER_3_CS_PIN);
    gpio_status |= hal_gpio_init((hal_gpio_pin_t)HAL_SPI_MASTER_3_SCK_PIN);
    gpio_status |= hal_gpio_init((hal_gpio_pin_t)HAL_SPI_MASTER_3_MOSI_PIN);
    gpio_status |= hal_gpio_init((hal_gpio_pin_t)HAL_SPI_MASTER_3_MISO_PIN);
    if (gpio_status != HAL_GPIO_STATUS_OK) {
        return;
    }
    if (spi_mode) {
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_SPI_MASTER_3_CS_PIN, (uint8_t)HAL_SPI_MASTER_3_CS_PIN_M_MA_SPI3_B_CS);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_SPI_MASTER_3_SCK_PIN, (uint8_t)HAL_SPI_MASTER_3_SCK_PIN_M_MA_SPI3_B_SCK);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_SPI_MASTER_3_MOSI_PIN, (uint8_t)HAL_SPI_MASTER_3_MOSI_PIN_M_MA_SPI3_B_MOSI);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_SPI_MASTER_3_MISO_PIN, (uint8_t)HAL_SPI_MASTER_3_MISO_PIN_M_MA_SPI3_B_MISO);
    } else {
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_SPI_MASTER_3_CS_PIN, (uint8_t)HAL_SPI_MASTER_3_CS_PIN_M_GPIO);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_SPI_MASTER_3_SCK_PIN, (uint8_t)HAL_SPI_MASTER_3_SCK_PIN_M_GPIO);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_SPI_MASTER_3_MOSI_PIN, (uint8_t)HAL_SPI_MASTER_3_MOSI_PIN_M_GPIO);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_SPI_MASTER_3_MISO_PIN, (uint8_t)HAL_SPI_MASTER_3_MISO_PIN_M_GPIO);
    }
}

void external_dsp_rtc_pinmux_switch(bool rtc_mode)
{
    hal_gpio_status_t   gpio_status;
    hal_gpio_pin_t      gpio_pin = (hal_gpio_pin_t)EXT_DSP_32K_CLK_PIN;
    uint8_t             function_index;

    log_hal_info("gpio=%d, rtc_mode=%x", gpio_pin, rtc_mode);
    gpio_status = hal_gpio_init(gpio_pin);
    if (gpio_status != HAL_GPIO_STATUS_OK) {
        return;
    }
    if (rtc_mode) {
        function_index = EXT_DSP_32K_CLK_PIN_M_CLKO;
    } else {
        function_index = EXT_DSP_32K_CLK_PIN_M_GPIO;
    }
    hal_pinmux_set_function(gpio_pin, function_index);
}

