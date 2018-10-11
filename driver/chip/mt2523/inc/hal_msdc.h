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

#ifndef __HAL_MSDC_H__
#define __HAL_MSDC_H__

#include "hal_platform.h"
#include "hal_sd.h"
#include "hal_sdio.h"
#if defined(HAL_SD_MODULE_ENABLED) || defined(HAL_SDIO_MODULE_ENABLED)
#include "hal_pdma_internal.h"
#include <string.h>

#define    MSDC_FALSE      false
#define    MSDC_TRUE       true

#define MSDC0_REG  ((msdc_register_t *)MSDC0_BASE) /*defined in mt2523.h*/
#define MSDC1_REG  ((msdc_register_t *)MSDC1_BASE)

#define MSDC1_PAD_SELECT_ADDRESS (0xa2010234)
#define MSDC1_PAD_SELECT_BIT_MASK (1)
#define MSDC1_PAD_GPIO26_MODE_ADDRESS  (0xA2020C30)
#define MSDC1_PAD_GPIO26_MODE_SHIFT    (8)
#define MSDC1_PAD_GPIO26_MODE_MASK     (0x0f << MSDC1_PAD_GPIO26_MODE_SHIFT)

#define MSDC_CLOCK           89140 /*(kHz) 89140*/
#define MSDC_OUTPUT_CLOCK    15300
#define MSDC_INIT_CLOCK      240 /*KHz*/

#define MSDC_26M_SOURCE_CLOCK  (26000) /*KHz*/

#define MSDC_MAX_BLOCK_LENGTH (2048)

#define MSDC_IO_1_ALL_PULLUP_47K              (0x22222)     /*config MSDC_IOCON_1,pull up io 47K*/

#define MSDC_DATA_TIMEOUT_COUNT               (80)

#define MSDC_DMA_MAX_COUNT   (65024)   /*the dma count register is 16 bit, so the max count is 2^16 - 511 = 65024*/

#define MSDC_LATCH_THRESHOLD (25000)  /*for register CLK_REG, if output clock biger than 25000, CLKPAD_RED BIT should be set to 1, else should be set to 0*/

/*defined for msdc dma transcation status*/
#define MSDC_DMA_TRANSFER_COMPLETE_OFFSET   (0)
#define MSDC_DMA_TRANSFER_COMPLETE_MASK     (1 << MSDC_DMA_TRANSFER_COMPLETE_OFFSET)

/*MSDC port, sd and sdio header file should defined as below*/
typedef enum {
    MSDC_PORT_0 = 0,
    MSDC_PORT_1 = 1,
    MSDC_PORT_MAX
} msdc_port_t;

typedef enum {
    MSDC_FAIL = -1,
    MSDC_OK = 0,
    MSDC_INITIALIZED = 1,
} msdc_status_t;

typedef enum {
    MSDC_OWNER_NONE = 0,
    MSDC_OWNER_SD   = 1,
    MSDC_OWNER_SDIO
} msdc_owner_t;

typedef struct {
    volatile msdc_owner_t    owner;
    uint32_t                 msdc_clock;
    uint32_t                 output_clock;
    uint32_t                 interrupt_register;
    volatile bool            is_card_present;
    volatile bool            is_initialized;
    volatile bool            is_timeout;
    volatile bool            is_card_plug_out;
    uint32_t                 command_status;
    uint32_t                 data_status;
    volatile uint32_t        dma_status;
    volatile uint32_t        dma_count;
    volatile uint32_t        dma_address;
    volatile bool            is_dma_write;
#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
    void (*msdc_card_detect_callback)(hal_sd_card_event_t event, void *user_data);
    void                     *card_detect_user_data;
#endif
#if defined(HAL_SD_MODULE_ENABLED)
    void (*msdc_sd_dma_interrupt_callback)(hal_sd_callback_event_t event,void *user_data);
#endif
#if defined(HAL_SDIO_MODULE_ENABLED)
    void (*msdc_sdio_dma_interrupt_callback)(hal_sdio_callback_event_t event,void *user_data);
#endif
} msdc_config_t;

typedef struct {
    uint32_t output_clock;
    uint8_t sclkf;
    uint8_t clksrc_pat;
    uint8_t clksrc;
} msdc_clock_t;

typedef enum {
    MSDC_FIFO_THRESHOLD_0     = 0,
    MSDC_FIFO_THRESHOLD_1     = 1,
    MSDC_FIFO_THRESHOLD_2     = 2,
    MSDC_FIFO_THRESHOLD_3     = 3,
    MSDC_FIFO_THRESHOLD_4     = 4,
    MSDC_FIFO_THRESHOLD_5     = 5,
    MSDC_FIFO_THRESHOLD_6     = 6,
    MSDC_FIFO_THRESHOLD_7     = 7,
    MSDC_FIFO_THRESHOLD_8     = 8,
} msdc_fifo_threshold_t;

typedef enum {
    MSDC_SOURCE_CLOCK_94_5M = 0,
    MSDC_SOURCE_CLOCK_74_3M = 1,
    MSDC_SOURCE_CLOCK_65M   = 2,
    MSDC_SOURCE_CLOCK_60_6M = 3
} msdc_source_clock_t;



typedef enum {
    MSDC_IO_4MA  = 0,
    MSDC_IO_8MA  = 1,
    MSDC_IO_12MA = 2,
    MSDC_IO_16MA = 3,
} msdc_driving_capability_t;

typedef enum {
    MSDC_IO_SLEW_FAST    = 0,
    MSDC_IO_SLEW_SLOW    = 1
} msdc_slew_rate_t;

typedef enum {
    MSDC_DMA_SINGLE_MODE = 0,
    MSDC_DMA_4_BEAT_INCREMENT_BUSRT = 1,
    MSDC_DMA_8_BEAT_INCREMENT_BUSRT = 2,
    MSDC_DMA_BURST_MODE_MAX
} msdc_burst_mode_t;


/** @brief   SD/MMC IO driving capability structure */
typedef struct {
    msdc_driving_capability_t control_line_output_driving_capability;                /**< SD/MMC control line output driving capability */
    msdc_driving_capability_t data_line_output_driving_capability;                   /**< SD/MMC data line output driving capability */
    msdc_slew_rate_t          control_line_slew_rate;                                /**< SD/MMC control line output slew rate */
    msdc_slew_rate_t          data_line_slew_rate;                                   /**< SD/MMC data line output slew rate */
} msdc_io_parameter_t;


void msdc_nvic_set(msdc_port_t msdc_port, bool is_enable);
void msdc_wait(uint32_t wait_ms);
void msdc_reset(msdc_port_t msdc_port);
void msdc_record_command_status(msdc_port_t msdc_port, uint32_t command_status);
void msdc_record_data_status(msdc_port_t msdc_port, uint32_t data_status);
void msdc_set_output_clock(msdc_port_t msdc_port, uint32_t msdc_clock);
void msdc_clock_init(msdc_port_t msdc_port);
uint32_t msdc_get_output_clock(msdc_port_t msdc_port);
void msdc_power_set(msdc_port_t msdc_port, bool is_power_on);
void msdc_set_io_parameter(msdc_port_t msdc_port, msdc_io_parameter_t *io_config);
void msdc_isr(hal_nvic_irq_t irq_number);
void msdc1_isr(hal_nvic_irq_t irq_number);
void msdc_interrupt_init(msdc_port_t msdc_port);
void msdc_eint_isr(void *user_data);
void msdc1_eint_isr(void *user_data);
void msdc_eint_registration(msdc_port_t msdc_port);
bool msdc_card_is_present(msdc_port_t msdc_port);
void msdc_dma_wait_transfer_complete(msdc_port_t msdc_port,uint32_t address);
void msdc_set_dma_interrupt(msdc_port_t msdc_port,uint32_t address);
void msdc_dma_isr(pdma_event_t msdc_event, void *user_data);
void msdc1_dma_isr(pdma_event_t msdc_event, void *user_data);
void msdc_dma_init(msdc_port_t msdc_port);
void msdc_dma_config(msdc_port_t msdc_port, uint32_t address, uint32_t count, bool is_write);
void msdc_dma_enable(msdc_port_t msdc_port);
void msdc_dma_disable(msdc_port_t msdc_port);
msdc_owner_t msdc_get_owner(msdc_port_t msdc_port);
void msdc_set_owner(msdc_port_t msdc_port, msdc_owner_t owner);
void msdc_card_power_set(msdc_port_t msdc_port, bool is_power_on);
msdc_status_t msdc_init(msdc_port_t msdc_port);
void msdc_deinit(msdc_port_t msdc_port);
uint32_t msdc_read_fifo(msdc_port_t msdc_port);
void msdc_write_fifo(msdc_port_t msdc_port, uint32_t data);
void msdc_set_irqmask0(msdc_port_t msdc_port,uint32_t mask);
unsigned char msdc_get_vcore_voltage(void);
#ifdef HAL_DVFS_MODULE_ENABLED
bool msdc_valid(uint32_t voltage, uint32_t frequency);
void msdc_prepare(uint32_t voltage, uint32_t frequency);
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
void msdc_switch_pinmux_to_gpio_mode(void);
void msdc_switch_pinmux_to_msdc_mode(void);
#endif

#endif /*HAL_SD_MODULE_ENABLED || HAL_SDIO_MODULE_ENABLED*/
#endif

