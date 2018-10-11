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

#include "mt2523.h"
#include "hal_msdc.h"
#include "hal_sd_define.h"
#include "hal_sd_internal.h"
#include "hal_sdio_define.h"

#if defined(HAL_SD_MODULE_ENABLED) || defined(HAL_SDIO_MODULE_ENABLED)
#include "hal.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_pdma_internal.h"
#include "hal_eint.h"
#include "hal_nvic.h"
#include "hal_log.h"
#include "assert.h"
#include "hal_dvfs_internal.h"
#include "hal_pmu_wrap_interface.h"
#include "msdc_custom_config.h"

msdc_config_t msdc_config[MSDC_PORT_MAX];

#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
static volatile bool msdc_card_detection_eint_polarity;
extern const unsigned char HAL_MSDC_EINT;  /*ept tool config*/
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
#define GPIO_MODE0_ADD (0xA2020C00)

#define MSDC0_1_BIT_BUS_WIDTH_SWITCH        (1 << 0)
#define MSDC0_4_BIT_BUS_WIDTH_SWITCH        (1 << 1)
#define MSDC1_PAD_A_1_BIT_BUS_WIDTH_SWITCH  (1 << 2)
#define MSDC1_PAD_A_4_BIT_BUS_WIDTH_SWITCH  (1 << 3)
#define MSDC1_PAD_B_1_BIT_BUS_WIDTH_SWITCH  (1 << 4)
#define MSDC1_PAD_B_4_BIT_BUS_WIDTH_SWITCH  (1 << 5)

static volatile uint32_t msdc_pinmux_switch_flag = 0;
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
bool dvfs_1p1v_need_sd_clock_tuning = MSDC_FALSE;
bool dvfs_1p3v_need_sd_clock_tuning = MSDC_FALSE;
uint32_t dvfs_save_sd_init_clock = 0;
static uint32_t msdc_clock_before_dvfs[2];
dvfs_notification_t msdc_desc = {
    "VCORE",
    "MSDC_CK",
    "MSDC_DVFS",
    {
        msdc_valid,
        msdc_prepare,
    }
};
#endif

const msdc_clock_t msdc_output_clock[7] = { {44570, 0, 1, 1},
    {39000, 0, 1, 2},
    {31200, 0, 1, 3},
    {22275, 1, 1, 1},
    {19500, 1, 1, 2},
    {15600, 1, 1, 3},
#ifdef CLK_USE_LFOSC
    {13000, 0, 0, 1},
#else
    {13000, 0, 0, 0},
#endif
};


#ifdef HAL_SDIO_MODULE_ENABLED
extern void sdio_isr(void);
#endif

void msdc_wait(uint32_t wait_ms)
{
    hal_gpt_delay_ms(wait_ms);
}


void msdc_reset(msdc_port_t msdc_port)
{
    msdc_register_t *msdc_register_base;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    if (!(msdc_register_base->MSDC_CFG & MSDC_CFG_RST_MASK)) {
        msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_RST_MASK;
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);
    while (msdc_register_base->MSDC_CFG & MSDC_CFG_RST_MASK) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
        hal_gpt_get_duration_count(start_count, end_count, &count);
        if (count >= MSDC_TIMEOUT_PERIOD_DATA) {
            log_hal_error("msdc reset timeout.\r\n");
            break;
        }
    }
}

void msdc_record_command_status(msdc_port_t msdc_port, uint32_t command_status)
{
    msdc_config[msdc_port].command_status = command_status;
}

void msdc_record_data_status(msdc_port_t msdc_port, uint32_t data_status)
{
    msdc_config[msdc_port].data_status = data_status;
}

void msdc_clock_init(msdc_port_t msdc_port)
{
    msdc_register_t *msdc_register_base;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    msdc_register_base->CLK_RED = msdc_register_base->CLK_RED & (~CLK_RED_CLKPAD_RED_MASK);

    /*in 50MHZ case, we should set 80 to have at least 100ms timeout,for initial read*/
    msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_DTOC_MASK)) |
                                  (MSDC_DATA_TIMEOUT_COUNT << SDC_CFG_DTOC_OFFSET);

    msdc_config[msdc_port].msdc_clock = MSDC_CLOCK;

    /*set source clock as HFOSC 89.1M*/
    msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_CLKSRC_MASK)) |
                                   (0x01 << MSDC_CFG_CLKSRC_OFFSET) |
                                   MSDC_CFG_CLKSRC_PAT_MASK;

    /*set output clock to 240KHz. The clock should <= 400KHz,*/
    msdc_set_output_clock(msdc_port, MSDC_INIT_CLOCK);

}


void msdc_set_output_clock(msdc_port_t msdc_port, uint32_t msdc_clock)
{
    msdc_register_t *msdc_register_base;
    uint32_t clock_config = 0;
    uint32_t msdc_cfg = 0;
    uint32_t i = 0;
    uint32_t msdc_mux_select = 0;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    if (0 == msdc_clock) {
        return;
    }

    for (i = 0; i < (sizeof(msdc_output_clock) / sizeof(msdc_output_clock[0])); i++) {
        if (msdc_output_clock[i].output_clock <= msdc_clock) {
            break;
        }
    }

    if (i < (sizeof(msdc_output_clock) / sizeof(msdc_output_clock[0]))) {
        msdc_config[msdc_port].output_clock = msdc_output_clock[i].output_clock;
        msdc_cfg = ((uint32_t)msdc_output_clock[i].sclkf << MSDC_CFG_SCLKF_OFFSET);
        msdc_mux_select = ((msdc_output_clock[i].clksrc_pat & 0x01) << 2) | (msdc_output_clock[i].clksrc & 0x03);
    } else {
        clock_config = ((MSDC_26M_SOURCE_CLOCK + msdc_clock - 1) / msdc_clock);
        clock_config = (clock_config >> 2) + (((clock_config & 3) != 0) ? 1 : 0);

        if (0 != clock_config) {
            msdc_config[msdc_port].output_clock = MSDC_26M_SOURCE_CLOCK / (4 * clock_config);
        }

        msdc_cfg = (clock_config << MSDC_CFG_SCLKF_OFFSET);
        msdc_mux_select = ((msdc_output_clock[i - 1].clksrc_pat & 0x01) << 2) | (msdc_output_clock[i - 1].clksrc & 0x03);
    }

    /*disable serial interface*/
    msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG & (~SDC_CFG_SIEN_MASK);

    /*set output clock*/
    msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~(MSDC_CFG_SCLKF_MASK))) | msdc_cfg;

    /*use clock common API select msdc clock source, so MSDC_CFG(CLKSRC_PAT & CLKSRC) not neet to config*/
    if (MSDC_PORT_0 == msdc_port) {
        clock_mux_sel(MSDC0_MUX_SEL, msdc_mux_select);
    } else if (MSDC_PORT_1 == msdc_port) {
        clock_mux_sel(MSDC1_MUX_SEL, msdc_mux_select);
    }

    /*enable serial interface fo MSDC*/
    msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG | SDC_CFG_SIEN_MASK;

    msdc_wait(1);

    /*config data and cmd latch edge*/
    if (msdc_config[msdc_port].output_clock < MSDC_LATCH_THRESHOLD) {
        msdc_register_base->CLK_RED = msdc_register_base->CLK_RED & (~CLK_RED_CLKPAD_RED_MASK);
    } else {
        msdc_register_base->CLK_RED = msdc_register_base->CLK_RED | CLK_RED_CLKPAD_RED_MASK;
    }
}

uint32_t msdc_get_output_clock(msdc_port_t msdc_port)
{
    return (msdc_config[msdc_port].output_clock);
}

/*MSDC PDN clock gating set*/
void msdc_power_set(msdc_port_t msdc_port, bool is_power_on)
{
    hal_clock_status_t ret = HAL_CLOCK_STATUS_ERROR;

    if (is_power_on == MSDC_TRUE) { /*enable clock*/
        if (MSDC_PORT_0 == msdc_port) {
            ret = hal_clock_enable(HAL_CLOCK_CG_MSDC0); /*PDN_CLRD1,clear clock gating off */
        } else if (MSDC_PORT_1 == msdc_port) {
            ret = hal_clock_enable(HAL_CLOCK_CG_MSDC1); /*PDN_CLRD2,clear clock gating off */
        }
    } else {
        if (MSDC_PORT_0 == msdc_port) {
            ret = hal_clock_disable(HAL_CLOCK_CG_MSDC0);  /*PDN_SETD1,set clock gating off */
        } else if (MSDC_PORT_1 == msdc_port) {
            ret = hal_clock_disable(HAL_CLOCK_CG_MSDC1); /*PDN_SETD2,set clock gating off */
        }
    }

    if (ret != HAL_CLOCK_STATUS_OK) {
        log_hal_error("msdc set clock gating error\r\n");
    }
}

void msdc_set_io_parameter(msdc_port_t msdc_port, msdc_io_parameter_t *io_config)
{
    msdc_register_t *msdc_register_base;
    uint32_t msdc_io_config = 0;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    msdc_io_config = (io_config->data_line_slew_rate << MSDC_IOCON_SRCFG1_OFFSET)                  |
                     (io_config->control_line_slew_rate << MSDC_IOCON_SRCFG0_OFFSET)               |
                     (io_config->data_line_output_driving_capability << MSDC_IOCON_ODCCFG1_OFFSET) |
                     (io_config->control_line_output_driving_capability << MSDC_IOCON_ODCCFG0_OFFSET);
    msdc_register_base->MSDC_IOCON = (msdc_register_base->MSDC_IOCON & (~ MSDC_IOCON_ODCCFG0_MASK)) | msdc_io_config;

    msdc_register_base->MSDC_IOCON = msdc_register_base->MSDC_IOCON | MSDC_IOCON_SAMPON_MASK | MSDC_IOCON_CMDSEL_MASK;

    /*pull up all pin 47k*/
    msdc_register_base->MSDC_IOCON1 = msdc_register_base->MSDC_IOCON1 | MSDC_IO_1_ALL_PULLUP_47K;
}

void msdc_nvic_set(msdc_port_t msdc_port, bool is_enable)
{
    IRQn_Type msdc_nvic_number;

    msdc_nvic_number = (MSDC_PORT_0 == msdc_port) ? MSDC0_IRQn : MSDC1_IRQn;

    /*should call nvic api to set msdc interrupt enable or disable*/
    if (is_enable) {
        hal_nvic_enable_irq(msdc_nvic_number);
    } else {
        hal_nvic_disable_irq(msdc_nvic_number);
    }
}

void msdc_isr(hal_nvic_irq_t irq_number)
{
    msdc_nvic_set(MSDC_PORT_0, MSDC_FALSE);
    msdc_config[MSDC_PORT_0].interrupt_register = MSDC0_REG->MSDC_INT;
    msdc_config[MSDC_PORT_0].data_status = MSDC0_REG->SDC_DATSTA;

#ifdef HAL_SD_MODULE_ENABLED
    if ((msdc_config[MSDC_PORT_0].data_status) & SDC_DATSTA_DATTO_MASK) {
        /*sd data transfer error*/
        if (msdc_config[MSDC_PORT_0].msdc_sd_dma_interrupt_callback) {
            /*call user callback to inform user error*/
            msdc_config[MSDC_PORT_0].msdc_sd_dma_interrupt_callback(HAL_SD_EVENT_DATA_TIMEOUT ,(void *)0);            
        }
    }
    else if ((msdc_config[MSDC_PORT_0].data_status) & SDC_DATSTA_DATCRCERR_MASK) {
       /*sd data transfer error*/
        if (msdc_config[MSDC_PORT_0].msdc_sd_dma_interrupt_callback) {
            /*call user callback to inform user error*/
            msdc_config[MSDC_PORT_0].msdc_sd_dma_interrupt_callback(HAL_SD_EVENT_CRC_ERROR ,(void *)0);            
        }
    }
#endif

#ifdef HAL_SDIO_MODULE_ENABLED
    /*if this is a SDIO interrupt*/
    if (msdc_config[MSDC_PORT_0].interrupt_register & MSDC_INT_SDIOIRQ_MASK) {
        sdio_isr();
    }
#endif

    msdc_nvic_set(MSDC_PORT_0, MSDC_TRUE);
}


void msdc1_isr(hal_nvic_irq_t irq_number)
{
    msdc_nvic_set(MSDC_PORT_1, MSDC_FALSE);
    msdc_config[MSDC_PORT_1].interrupt_register = MSDC1_REG->MSDC_INT;
    msdc_config[MSDC_PORT_1].data_status = MSDC0_REG->SDC_DATSTA;

#ifdef HAL_SD_MODULE_ENABLED
    if ((msdc_config[MSDC_PORT_1].data_status) & SDC_DATSTA_DATTO_MASK) {
        /*sd data transfer error*/
        if (msdc_config[MSDC_PORT_1].msdc_sd_dma_interrupt_callback) {
            /*call user callback to inform user error*/
            msdc_config[MSDC_PORT_1].msdc_sd_dma_interrupt_callback(HAL_SD_EVENT_DATA_TIMEOUT ,(void *)0);
        }
    }
    else if ((msdc_config[MSDC_PORT_1].data_status) & SDC_DATSTA_DATCRCERR_MASK) {
       /*sd data transfer error*/
        if (msdc_config[MSDC_PORT_1].msdc_sd_dma_interrupt_callback) {
            /*call user callback to inform user error*/
            msdc_config[MSDC_PORT_1].msdc_sd_dma_interrupt_callback(HAL_SD_EVENT_CRC_ERROR ,(void *)0);
        }
    }
#endif

#ifdef HAL_SDIO_MODULE_ENABLED
    /*if this is a SDIO interrupt*/
    if (msdc_config[MSDC_PORT_1].interrupt_register & MSDC_INT_SDIOIRQ_MASK) {
        sdio_isr();
    }
#endif

    msdc_nvic_set(MSDC_PORT_1, MSDC_TRUE);
}


void msdc_interrupt_init(msdc_port_t msdc_port)
{
    msdc_register_t *msdc_register_base;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    msdc_nvic_set(msdc_port, MSDC_FALSE);

    if (MSDC_PORT_0 == msdc_port) {
        hal_nvic_register_isr_handler(MSDC0_IRQn, msdc_isr);
    } else if (MSDC_PORT_1 == msdc_port) {
        hal_nvic_register_isr_handler(MSDC1_IRQn, msdc1_isr);
    }

    /*clear MSDC interrupt status, MSDC_INT is read-clear*/
    msdc_register_base->MSDC_INT;

    /*disable MSDC interrupt*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG & (~MSDC_CFG_INTEN_MASK);

    /*disable data request interrupt*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG & (~MSDC_CFG_DIRQEN_MASK);

    msdc_register_base->SDC_IRQMASK0 = 0xFFFFFFFF;

    /*enable MSDC interrupt*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_INTEN_MASK;

    msdc_nvic_set(msdc_port, MSDC_TRUE);
}

#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
void msdc_eint_isr(void *user_data)
{
    hal_eint_trigger_mode_t mode;
    hal_sd_card_event_t sd_event = HAL_SD_EVENT_CARD_INSERTED;

    hal_eint_mask((hal_eint_number_t)HAL_MSDC_EINT);

    msdc_config[MSDC_PORT_0].is_card_present = msdc_card_detection_eint_polarity ? MSDC_FALSE : MSDC_TRUE;

    msdc_card_detection_eint_polarity = msdc_card_detection_eint_polarity ? MSDC_FALSE : MSDC_TRUE;

    mode = msdc_card_detection_eint_polarity ? HAL_EINT_LEVEL_HIGH : HAL_EINT_LEVEL_LOW;
    hal_eint_set_trigger_mode((hal_eint_number_t)HAL_MSDC_EINT, mode);

    hal_eint_unmask((hal_eint_number_t)HAL_MSDC_EINT);

    if (msdc_config[MSDC_PORT_0].is_card_present == MSDC_FALSE) {
        msdc_config[MSDC_PORT_0].is_card_plug_out = MSDC_TRUE;
        sd_event = HAL_SD_EVENT_CARD_REMOVED;
        msdc_reset(MSDC_PORT_0);
        pdma_stop(PDMA_MSDC1);
        msdc_deinit(MSDC_PORT_0);
    } else {
        msdc_config[MSDC_PORT_0].is_card_plug_out = MSDC_FALSE;
        sd_event = HAL_SD_EVENT_CARD_INSERTED;
    }

    if (msdc_config[MSDC_PORT_0].msdc_card_detect_callback != NULL) {
        msdc_config[MSDC_PORT_0].msdc_card_detect_callback(sd_event, msdc_config[MSDC_PORT_0].card_detect_user_data);
    }
}

void msdc1_eint_isr(void *user_data)
{
    hal_eint_trigger_mode_t mode;
    hal_sd_card_event_t sd_event = HAL_SD_EVENT_CARD_INSERTED;

    hal_eint_mask((hal_eint_number_t)HAL_MSDC_EINT);

    msdc_config[MSDC_PORT_1].is_card_present = msdc_card_detection_eint_polarity ? MSDC_FALSE : MSDC_TRUE;

    msdc_card_detection_eint_polarity = msdc_card_detection_eint_polarity ? MSDC_FALSE : MSDC_TRUE;

    mode = msdc_card_detection_eint_polarity ? HAL_EINT_LEVEL_HIGH : HAL_EINT_LEVEL_LOW;
    hal_eint_set_trigger_mode((hal_eint_number_t)HAL_MSDC_EINT, mode);

    hal_eint_unmask((hal_eint_number_t)HAL_MSDC_EINT);

    if (msdc_config[MSDC_PORT_1].is_card_present == MSDC_FALSE) {
        msdc_config[MSDC_PORT_1].is_card_plug_out = MSDC_TRUE;
        sd_event = HAL_SD_EVENT_CARD_REMOVED;
        msdc_reset(MSDC_PORT_1);
        pdma_stop(PDMA_MSDC2);
        msdc_deinit(MSDC_PORT_1);
    } else {
        msdc_config[MSDC_PORT_1].is_card_plug_out = MSDC_FALSE;
        sd_event = HAL_SD_EVENT_CARD_INSERTED;
    }

    if (msdc_config[MSDC_PORT_1].msdc_card_detect_callback != NULL) {
        msdc_config[MSDC_PORT_1].msdc_card_detect_callback(sd_event, msdc_config[MSDC_PORT_1].card_detect_user_data);
    }
}

void msdc_eint_registration(msdc_port_t msdc_port)
{
    hal_eint_config_t config;

    /*HAL_MSDC_EINT is EPT tool config, the HAL_MSDC_EINT value is 0xff means the hot plug eint is not configured in EPT tool*/
    if (0xFF == HAL_MSDC_EINT) {
        assert(0);
    }

    msdc_card_detection_eint_polarity = MSDC_FALSE;

    config.debounce_time = 500;
    config.trigger_mode = HAL_EINT_LEVEL_LOW;

    hal_eint_mask((hal_eint_number_t)HAL_MSDC_EINT);
    hal_eint_init((hal_eint_number_t)HAL_MSDC_EINT, &config);

    if (msdc_port == MSDC_PORT_0) {
        hal_eint_register_callback((hal_eint_number_t)HAL_MSDC_EINT, msdc_eint_isr, NULL);
    } else if (msdc_port == MSDC_PORT_1) {
        hal_eint_register_callback((hal_eint_number_t)HAL_MSDC_EINT, msdc1_eint_isr, NULL);
    }

    hal_eint_unmask((hal_eint_number_t)HAL_MSDC_EINT);
}
#endif

bool msdc_card_is_present(msdc_port_t msdc_port)
{
    return (msdc_config[msdc_port].is_card_present);
}


void msdc_dma_wait_transfer_complete(msdc_port_t msdc_port,uint32_t address)
{
    pdma_channel_t dma_channel;

    dma_channel = (msdc_port == MSDC_PORT_0) ? PDMA_MSDC1 : PDMA_MSDC2;
    /*polling wait pdma ready*/
    pdma_start_polling(dma_channel, address);
}

void msdc_set_dma_interrupt(msdc_port_t msdc_port,uint32_t address)
{
    pdma_channel_t dma_channel;

    dma_channel = (msdc_port == MSDC_PORT_0) ? PDMA_MSDC1 : PDMA_MSDC2;
    /*polling wait pdma ready*/
    pdma_start_interrupt(dma_channel, address);
}

void msdc_dma_isr(pdma_event_t msdc_event, void *user_data)
{
    msdc_config[MSDC_PORT_0].dma_status |= MSDC_DMA_TRANSFER_COMPLETE_MASK;
    if (MSDC_OWNER_SD == msdc_get_owner(MSDC_PORT_0)) {
            
            #ifdef HAL_SD_MODULE_ENABLED
                if(msdc_config[MSDC_PORT_0].msdc_sd_dma_interrupt_callback) {
                    if (HAL_SD_STATUS_OK != sd_wait_dma_interrupt_transfer_ready(HAL_SD_PORT_0)) {
                        log_hal_error("msdc:msdc_dma_isr wait sd ready err!\r\n");
                    }
                }
                
            #endif
        }
    else if (MSDC_OWNER_SDIO == msdc_get_owner(MSDC_PORT_0)) {
             #ifdef HAL_SDIO_MODULE_ENABLED
                if( msdc_config[MSDC_PORT_0].msdc_sdio_dma_interrupt_callback) {
                    if (HAL_SDIO_STATUS_OK != sdio_wait_dma_interrupt_transfer_ready(HAL_SDIO_PORT_0)) {
                    log_hal_error("msdc:msdc_dma_isr wait sdio ready err!\r\n");
                }
            }        
            #endif
    }
}

void msdc1_dma_isr(pdma_event_t msdc_event, void *user_data)
{
    msdc_config[MSDC_PORT_1].dma_status |= MSDC_DMA_TRANSFER_COMPLETE_MASK;
    if (MSDC_OWNER_SD == msdc_get_owner(MSDC_PORT_1)) {
            
            #ifdef HAL_SD_MODULE_ENABLED
                if(msdc_config[MSDC_PORT_1].msdc_sd_dma_interrupt_callback) {
                    if (HAL_SD_STATUS_OK != sd_wait_dma_interrupt_transfer_ready((hal_sd_port_t)MSDC_PORT_1)) {
                        log_hal_error("msdc:msdc_dma_isr wait sd ready err!\r\n");
                    }
                }
                
            #endif
        }
    else if (MSDC_OWNER_SDIO == msdc_get_owner(MSDC_PORT_1)) {
             #ifdef HAL_SDIO_MODULE_ENABLED
                if( msdc_config[MSDC_PORT_1].msdc_sdio_dma_interrupt_callback) {
                    if (HAL_SDIO_STATUS_OK != sdio_wait_dma_interrupt_transfer_ready((hal_sdio_port_t)MSDC_PORT_1)) {
                    log_hal_error("msdc:msdc_dma_isr wait sdio ready err!\r\n");
                }
            }        
            #endif
    }
}


void msdc_dma_init(msdc_port_t msdc_port)
{
    pdma_channel_t dma_channel;

    dma_channel = (msdc_port == MSDC_PORT_0) ? PDMA_MSDC1 : PDMA_MSDC2;

    msdc_config[msdc_port].dma_status = 0;

    pdma_init(dma_channel);

    if (msdc_port == MSDC_PORT_0) {
        pdma_register_callback(dma_channel, msdc_dma_isr, NULL);
    } else if (msdc_port == MSDC_PORT_1) {
        pdma_register_callback(dma_channel, msdc1_dma_isr, NULL);
    }
}


void msdc_dma_config(msdc_port_t msdc_port, uint32_t address, uint32_t count, bool is_write)
{
    pdma_config_t dma_config;
    msdc_register_t *msdc_register_base;
    pdma_channel_t dma_channel;

    dma_channel = (msdc_port == MSDC_PORT_0) ? PDMA_MSDC1 : PDMA_MSDC2;
    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    memset(&dma_config, 0, sizeof(pdma_config_t));

    msdc_config[msdc_port].dma_status &= (~MSDC_DMA_TRANSFER_COMPLETE_MASK);

    if (!(address & 0x03)) {
        msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_FIFOTHD_MASK)) |
                                       (MSDC_FIFO_THRESHOLD_4  << MSDC_CFG_FIFOTHD_OFFSET);
        msdc_register_base->MSDC_IOCON = (msdc_register_base->MSDC_IOCON & (~MSDC_IOCON_DMABURST_MASK)) |
                                         (MSDC_DMA_4_BEAT_INCREMENT_BUSRT << MSDC_IOCON_DMABURST_OFFSET);
        dma_config.burst_mode = MSDC_TRUE;
        dma_config.size = PDMA_WORD;
        dma_config.count = count >> 2;
        dma_config.master_type = is_write ? PDMA_TX : PDMA_RX;

    } else {
        msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_FIFOTHD_MASK)) |
                                       (MSDC_FIFO_THRESHOLD_1  << MSDC_CFG_FIFOTHD_OFFSET);
        msdc_register_base->MSDC_IOCON = (msdc_register_base->MSDC_IOCON & (~MSDC_IOCON_DMABURST_MASK)) |
                                         (MSDC_DMA_SINGLE_MODE << MSDC_IOCON_DMABURST_OFFSET);
        dma_config.burst_mode = MSDC_TRUE;
        dma_config.size = PDMA_BYTE;
        dma_config.count = count;
        dma_config.master_type = is_write ? PDMA_TX : PDMA_RX;
    }

    /*if the count is bigger than MSDC_DMA_MAX_COUNT(65024), then must be ivided into several times to transfer data*/
    if (dma_config.count > MSDC_DMA_MAX_COUNT) {

        if (!(address & 0x03)) {
            msdc_config[msdc_port].dma_count = (dma_config.count - MSDC_DMA_MAX_COUNT) << 2;
            msdc_config[msdc_port].dma_address = address + (MSDC_DMA_MAX_COUNT << 2);
        } else {
            msdc_config[msdc_port].dma_count = dma_config.count - MSDC_DMA_MAX_COUNT;
            msdc_config[msdc_port].dma_address = address + MSDC_DMA_MAX_COUNT;
        }

        msdc_config[msdc_port].is_dma_write = is_write;
        dma_config.count = MSDC_DMA_MAX_COUNT;

    } else {
        msdc_config[msdc_port].dma_count = 0;
        msdc_config[msdc_port].dma_address = 0;
        msdc_config[msdc_port].is_dma_write = MSDC_FALSE;
    }

    if (0 > pdma_configure(dma_channel, &dma_config)) {
        log_hal_error("msdc config pdma error!\r\n");
    }
}


void msdc_dma_enable(msdc_port_t msdc_port)
{
    msdc_register_t *msdc_register_base;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_DMAEN_MASK;
}

void msdc_dma_disable(msdc_port_t msdc_port)
{
    msdc_register_t *msdc_register_base;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG & (~MSDC_CFG_DMAEN_MASK);
}


msdc_owner_t msdc_get_owner(msdc_port_t msdc_port)
{
    return (msdc_config[msdc_port].owner);
}

void msdc_set_owner(msdc_port_t msdc_port, msdc_owner_t owner)
{
    msdc_config[msdc_port].owner = owner;
}

/*this function used to turn on power for card VDD and VDDQ*/
void msdc_card_power_set(msdc_port_t msdc_port, bool is_power_on)
{
    pmu_ctrl_driver_power(DRIVER_MSDC, (pmic_power_ctrl_t)is_power_on);
}

void msdc_pad_mux_select(msdc_port_t msdc_port)
{
    if ((MSDC_PORT_1 == msdc_port) && (HAL_GPIO_26_MC1_B_CM0 == ((*(volatile uint32_t *)MSDC1_PAD_GPIO26_MODE_ADDRESS & MSDC1_PAD_GPIO26_MODE_MASK) >> MSDC1_PAD_GPIO26_MODE_SHIFT))) {
        *(volatile uint32_t *)MSDC1_PAD_SELECT_ADDRESS = *(volatile uint32_t *)MSDC1_PAD_SELECT_ADDRESS | MSDC1_PAD_SELECT_BIT_MASK;
    }
}

msdc_status_t msdc_init(msdc_port_t msdc_port)
{
    msdc_register_t *msdc_register_base;
    msdc_io_parameter_t msdc_io_driving;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    /*if MSDC have initialized, return directly*/
    if (msdc_config[msdc_port].is_initialized) {
        return MSDC_INITIALIZED;
    }

#ifdef HAL_SLEEP_MANAGER_ENABLED
    /*switch pinmux to msdc mode because pinmux mode have been switch to gpio mode when sd/sdio init fail.*/
    msdc_switch_pinmux_to_msdc_mode();
#endif
    msdc_pad_mux_select(msdc_port);

    /*turn on MSDC clock*/
    msdc_power_set(msdc_port, MSDC_TRUE);

    msdc_config[msdc_port].msdc_clock = MSDC_CLOCK;

    /*enable MSDC and enable interrupt pin*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | (MSDC_CFG_MSDC_MASK | MSDC_CFG_PINEN_MASK);

    if (MSDC_PORT_0 == msdc_port) {
        msdc_io_driving.control_line_output_driving_capability = (msdc_driving_capability_t)MSDC_0_CONTROL_LINE_DRIVING_CAPABILITY;
        msdc_io_driving.data_line_output_driving_capability = (msdc_driving_capability_t)MSDC_0_DATA_LINE_DRIVING_CAPABILITY;
        msdc_io_driving.control_line_slew_rate = (msdc_slew_rate_t)MSDC_0_CONTROL_LINE_SLEW_RATE;
        msdc_io_driving.data_line_slew_rate = (msdc_slew_rate_t)MSDC_0_DATA_LINE_SLEW_RATE;
    } else {
        msdc_io_driving.control_line_output_driving_capability = (msdc_driving_capability_t)MSDC_1_CONTROL_LINE_DRIVING_CAPABILITY;
        msdc_io_driving.data_line_output_driving_capability = (msdc_driving_capability_t)MSDC_1_DATA_LINE_DRIVING_CAPABILITY;
        msdc_io_driving.control_line_slew_rate = (msdc_slew_rate_t)MSDC_1_CONTROL_LINE_SLEW_RATE;
        msdc_io_driving.data_line_slew_rate = (msdc_slew_rate_t)MSDC_1_DATA_LINE_SLEW_RATE;
    }

    /*config MSDC io pin driving capability*/
    msdc_set_io_parameter(msdc_port, (msdc_io_parameter_t *)(&msdc_io_driving));

    msdc_wait(1);

    /*config MSDC fifo threshold to 1 word*/
    msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_FIFOTHD_MASK)) |
                                   (MSDC_FIFO_THRESHOLD_1 << MSDC_CFG_FIFOTHD_OFFSET);

    msdc_config[msdc_port].is_card_present = MSDC_TRUE;

    /*turn on power for card VDD and VDDQ*/
    msdc_card_power_set(msdc_port, MSDC_TRUE);

    /*init MSDC interrupt*/
    msdc_interrupt_init(msdc_port);

#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
    /*card detection eint registration*/
    msdc_eint_registration(msdc_port);
#endif

    msdc_config[msdc_port].is_initialized = MSDC_TRUE;

#ifdef HAL_DVFS_MODULE_ENABLED
    dvfs_register_notification(&msdc_desc);
#endif

    return MSDC_OK;
}


void msdc_deinit(msdc_port_t msdc_port)
{
    msdc_config[msdc_port].is_initialized = MSDC_FALSE;

    msdc_config[msdc_port].owner = MSDC_OWNER_NONE;

    msdc_config[msdc_port].is_card_present = MSDC_FALSE;

    /*turn off MSDC power*/
    msdc_power_set(msdc_port, MSDC_FALSE);

    /*turn off power for card VDD and VDDQ*/
    msdc_card_power_set(msdc_port, MSDC_FALSE);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    /*switch pinmux to gpio mode when sd/sdio init fail.*/
    msdc_switch_pinmux_to_gpio_mode();
#endif
#ifdef HAL_DVFS_MODULE_ENABLED
    dvfs_deregister_notification(&msdc_desc);
#endif
}


unsigned char msdc_get_vcore_voltage(void)
{
    unsigned char vcore_voltage;

    vcore_voltage = pmu_get_vcore_voltage();

    return vcore_voltage;
}


#ifdef HAL_DVFS_MODULE_ENABLED
bool msdc_valid(uint32_t voltage, uint32_t frequency)
{
    return MSDC_TRUE;
}

void msdc_prepare(uint32_t voltage, uint32_t frequency)
{
    uint32_t msdc_max_source_clock;
    uint32_t msdc_max_output_clock;

    msdc_max_source_clock = frequency;
    msdc_max_output_clock = msdc_max_source_clock / 2;

    if (MSDC_OWNER_NONE == msdc_config[MSDC_PORT_0].owner) {
        /*do nothing*/
    } else if (msdc_config[MSDC_PORT_0].output_clock > msdc_max_output_clock) {
        /* frequency down.*/
        /*save the max output clock*/
        if (msdc_clock_before_dvfs[MSDC_PORT_0] < msdc_config[MSDC_PORT_0].output_clock) {
            msdc_clock_before_dvfs[MSDC_PORT_0] = msdc_config[MSDC_PORT_0].output_clock;
        }
        msdc_set_output_clock(MSDC_PORT_0, msdc_max_output_clock);
    } else if ((1300000 == voltage) &&
               ((MSDC_TRUE == dvfs_1p1v_need_sd_clock_tuning) || (MSDC_TRUE == dvfs_1p3v_need_sd_clock_tuning)) &&
               (MSDC_OWNER_SD == msdc_config[MSDC_PORT_0].owner)) {
        sd_output_clock_tuning((hal_sd_port_t)MSDC_PORT_0, dvfs_save_sd_init_clock);
        dvfs_1p1v_need_sd_clock_tuning = MSDC_FALSE;
        dvfs_1p3v_need_sd_clock_tuning = MSDC_FALSE;
    } else if ((1100000 == voltage) &&
               (MSDC_TRUE == dvfs_1p1v_need_sd_clock_tuning) &&
               (MSDC_OWNER_SD == msdc_config[MSDC_PORT_0].owner)) {
        sd_output_clock_tuning((hal_sd_port_t)MSDC_PORT_0, dvfs_save_sd_init_clock);
        dvfs_1p1v_need_sd_clock_tuning = MSDC_FALSE;
    } else if ((msdc_config[MSDC_PORT_0].output_clock < msdc_max_output_clock) &&
               (msdc_clock_before_dvfs[MSDC_PORT_0] > msdc_config[MSDC_PORT_0].output_clock)) {
        /*frequency up.*/
        if (msdc_clock_before_dvfs[MSDC_PORT_0] >= msdc_max_output_clock) {
            msdc_set_output_clock(MSDC_PORT_0, msdc_max_output_clock);
        }
    }

    if (MSDC_OWNER_NONE == msdc_config[MSDC_PORT_1].owner) {
        /*do nothing*/
    } else if (msdc_config[MSDC_PORT_1].output_clock > msdc_max_output_clock) {
        /* frequency down.*/
        /*save the max output clock*/
        if (msdc_clock_before_dvfs[MSDC_PORT_1] < msdc_config[MSDC_PORT_1].output_clock) {
            msdc_clock_before_dvfs[MSDC_PORT_1] = msdc_config[MSDC_PORT_1].output_clock;
        }

        msdc_set_output_clock(MSDC_PORT_1, msdc_max_output_clock);
    } else if ((1300000 == voltage) &&
               ((MSDC_TRUE == dvfs_1p1v_need_sd_clock_tuning) || (MSDC_TRUE == dvfs_1p3v_need_sd_clock_tuning)) &&
               (MSDC_OWNER_SD == msdc_config[MSDC_PORT_1].owner)) {
        sd_output_clock_tuning((hal_sd_port_t)MSDC_PORT_1, dvfs_save_sd_init_clock);
        dvfs_1p1v_need_sd_clock_tuning = MSDC_FALSE;
        dvfs_1p3v_need_sd_clock_tuning = MSDC_FALSE;
    } else if ((1100000 == voltage) &&
               (MSDC_TRUE == dvfs_1p1v_need_sd_clock_tuning) &&
               (MSDC_OWNER_SD == msdc_config[MSDC_PORT_1].owner)) {
        sd_output_clock_tuning((hal_sd_port_t)MSDC_PORT_1, dvfs_save_sd_init_clock);
        dvfs_1p1v_need_sd_clock_tuning = MSDC_FALSE;
    } else if ((msdc_config[MSDC_PORT_1].output_clock < msdc_max_output_clock) &&
               (msdc_clock_before_dvfs[MSDC_PORT_1] > msdc_config[MSDC_PORT_1].output_clock)) {
        /*frequency up.*/
        if (msdc_clock_before_dvfs[MSDC_PORT_1] >= msdc_max_output_clock) {
            msdc_set_output_clock(MSDC_PORT_1, msdc_max_output_clock);
        }
    }
}
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
uint8_t msdc_get_pinmux(hal_gpio_pin_t pin_number)
{
    uint32_t no;
    uint32_t remainder;
    uint32_t temp;
    uint8_t function_index;

    /* get the register number corresponding to the pin as one register can control 8 pins*/
    no = pin_number / 8;

    /* get the bit offset within the register as one register can control 8 pins*/
    remainder = pin_number % 8;
    temp = *(volatile unsigned int *)(GPIO_MODE0_ADD + (no * 0x10));

    temp &= (0xF << (4 * remainder));
    function_index = (temp >> (4 * remainder));

    return function_index;
}


void msdc_switch_pinmux_to_gpio_mode(void)
{
    /*MSDC0*/
    if (HAL_GPIO_30_MC0_CK == msdc_get_pinmux(HAL_GPIO_30)) {
        hal_pinmux_set_function(HAL_GPIO_30, HAL_GPIO_30_GPIO30);
        hal_pinmux_set_function(HAL_GPIO_31, HAL_GPIO_31_GPIO31);
        hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_GPIO32);

        /*disable pull*/
        hal_gpio_set_pupd_register(HAL_GPIO_30, 0, 0, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_31, 0, 0, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_32, 0, 0, 0);

        msdc_pinmux_switch_flag |= MSDC0_1_BIT_BUS_WIDTH_SWITCH;

        if (HAL_GPIO_35_MC0_DA3 == msdc_get_pinmux(HAL_GPIO_35)) {
            hal_pinmux_set_function(HAL_GPIO_33, HAL_GPIO_33_GPIO33);
            hal_pinmux_set_function(HAL_GPIO_34, HAL_GPIO_34_GPIO34);
            hal_pinmux_set_function(HAL_GPIO_35, HAL_GPIO_35_GPIO35);

            hal_gpio_set_pupd_register(HAL_GPIO_33, 0, 0, 0);
            hal_gpio_set_pupd_register(HAL_GPIO_34, 0, 0, 0);
            hal_gpio_set_pupd_register(HAL_GPIO_35, 0, 0, 0);

            msdc_pinmux_switch_flag |= MSDC0_4_BIT_BUS_WIDTH_SWITCH;
        }
    }

    /*MSDC1 PAD_B*/
    if (HAL_GPIO_27_MC1_B_CK == msdc_get_pinmux(HAL_GPIO_27)) {
        hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_GPIO26);
        hal_pinmux_set_function(HAL_GPIO_27, HAL_GPIO_27_GPIO27);
        hal_pinmux_set_function(HAL_GPIO_28, HAL_GPIO_28_GPIO28);

        hal_gpio_set_pupd_register(HAL_GPIO_26, 0, 0, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_27, 0, 0, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_28, 0, 0, 0);

        msdc_pinmux_switch_flag |= MSDC1_PAD_B_1_BIT_BUS_WIDTH_SWITCH;

        if (HAL_GPIO_24_MC1_B_DA3 == msdc_get_pinmux(HAL_GPIO_24)) {
            hal_pinmux_set_function(HAL_GPIO_24, HAL_GPIO_24_GPIO24);
            hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_GPIO25);
            hal_pinmux_set_function(HAL_GPIO_29, HAL_GPIO_29_GPIO29);

            hal_gpio_set_pupd_register(HAL_GPIO_24, 0, 0, 0);
            hal_gpio_set_pupd_register(HAL_GPIO_25, 0, 0, 0);
            hal_gpio_set_pupd_register(HAL_GPIO_29, 0, 0, 0);

            msdc_pinmux_switch_flag |= MSDC1_PAD_B_4_BIT_BUS_WIDTH_SWITCH;
        }
    }

    /*MSDC1 PAD_A*/
    if (HAL_GPIO_4_MC1_A_CK == msdc_get_pinmux(HAL_GPIO_4)) {
        hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_GPIO4);
        hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_GPIO5);
        hal_pinmux_set_function(HAL_GPIO_6, HAL_GPIO_6_GPIO6);

        hal_gpio_set_pupd_register(HAL_GPIO_4, 0, 0, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_5, 0, 0, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_6, 0, 0, 0);

        msdc_pinmux_switch_flag |= MSDC1_PAD_A_1_BIT_BUS_WIDTH_SWITCH;

        if (HAL_GPIO_9_MC1_A_DA3 == msdc_get_pinmux(HAL_GPIO_9)) {
            hal_pinmux_set_function(HAL_GPIO_7, HAL_GPIO_7_GPIO7);
            hal_pinmux_set_function(HAL_GPIO_8, HAL_GPIO_8_GPIO8);
            hal_pinmux_set_function(HAL_GPIO_9, HAL_GPIO_9_GPIO9);

            hal_gpio_set_pupd_register(HAL_GPIO_7, 0, 0, 0);
            hal_gpio_set_pupd_register(HAL_GPIO_8, 0, 0, 0);
            hal_gpio_set_pupd_register(HAL_GPIO_9, 0, 0, 0);

            msdc_pinmux_switch_flag |= MSDC1_PAD_A_4_BIT_BUS_WIDTH_SWITCH;
        }
    }
}


void msdc_switch_pinmux_to_msdc_mode(void)
{
    /*MSDC0*/
    if (msdc_pinmux_switch_flag & MSDC0_4_BIT_BUS_WIDTH_SWITCH) {
        hal_pinmux_set_function(HAL_GPIO_30, HAL_GPIO_30_MC0_CK);
        hal_pinmux_set_function(HAL_GPIO_31, HAL_GPIO_31_MC0_CM0);
        hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_MC0_DA0);
        hal_pinmux_set_function(HAL_GPIO_33, HAL_GPIO_33_MC0_DA1);
        hal_pinmux_set_function(HAL_GPIO_34, HAL_GPIO_34_MC0_DA2);
        hal_pinmux_set_function(HAL_GPIO_35, HAL_GPIO_35_MC0_DA3);

        hal_gpio_set_pupd_register(HAL_GPIO_30, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_31, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_32, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_33, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_34, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_35, 0, 1, 0);

    } else if (msdc_pinmux_switch_flag & MSDC0_1_BIT_BUS_WIDTH_SWITCH) {
        hal_pinmux_set_function(HAL_GPIO_30, HAL_GPIO_30_MC0_CK);
        hal_pinmux_set_function(HAL_GPIO_31, HAL_GPIO_31_MC0_CM0);
        hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_MC0_DA0);

        hal_gpio_set_pupd_register(HAL_GPIO_30, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_31, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_32, 0, 1, 0);
    }


    /*MSDC1 PAD_B*/
    if (msdc_pinmux_switch_flag & MSDC1_PAD_B_4_BIT_BUS_WIDTH_SWITCH) {
        hal_pinmux_set_function(HAL_GPIO_24, HAL_GPIO_24_MC1_B_DA3);
        hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_MC1_B_DA2);
        hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_MC1_B_CM0);
        hal_pinmux_set_function(HAL_GPIO_27, HAL_GPIO_27_MC1_B_CK);
        hal_pinmux_set_function(HAL_GPIO_28, HAL_GPIO_28_MC1_B_DA0);
        hal_pinmux_set_function(HAL_GPIO_29, HAL_GPIO_29_MC1_B_DA1);

        hal_gpio_set_pupd_register(HAL_GPIO_24, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_25, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_26, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_27, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_28, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_29, 0, 1, 0);
    } else if (msdc_pinmux_switch_flag & MSDC1_PAD_B_1_BIT_BUS_WIDTH_SWITCH) {
        hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_MC1_B_CM0);
        hal_pinmux_set_function(HAL_GPIO_27, HAL_GPIO_27_MC1_B_CK);
        hal_pinmux_set_function(HAL_GPIO_28, HAL_GPIO_28_MC1_B_DA0);

        hal_gpio_set_pupd_register(HAL_GPIO_26, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_27, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_28, 0, 1, 0);
    }

    /*MSDC1 PAD_A*/
    if (msdc_pinmux_switch_flag & MSDC1_PAD_A_4_BIT_BUS_WIDTH_SWITCH) {
        hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_MC1_A_CK);
        hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_MC1_A_CM0);
        hal_pinmux_set_function(HAL_GPIO_6, HAL_GPIO_6_MC1_A_DA0);
        hal_pinmux_set_function(HAL_GPIO_7, HAL_GPIO_7_MC1_A_DA1);
        hal_pinmux_set_function(HAL_GPIO_8, HAL_GPIO_8_MC1_A_DA2);
        hal_pinmux_set_function(HAL_GPIO_9, HAL_GPIO_9_MC1_A_DA3);

        hal_gpio_set_pupd_register(HAL_GPIO_4, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_5, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_6, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_7, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_8, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_9, 0, 1, 0);
    } else if (msdc_pinmux_switch_flag & MSDC1_PAD_A_1_BIT_BUS_WIDTH_SWITCH) {
        hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_MC1_A_CK);
        hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_MC1_A_CM0);
        hal_pinmux_set_function(HAL_GPIO_6, HAL_GPIO_6_MC1_A_DA0);

        hal_gpio_set_pupd_register(HAL_GPIO_4, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_5, 0, 1, 0);
        hal_gpio_set_pupd_register(HAL_GPIO_6, 0, 1, 0);
    }

    msdc_pinmux_switch_flag = 0;
}
#endif

void msdc_set_irqmask0(msdc_port_t msdc_port,uint32_t mask)
{
    msdc_register_t *msdc_register_base;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    msdc_nvic_set(msdc_port, MSDC_FALSE);

    msdc_register_base->SDC_IRQMASK0 = mask;

    msdc_nvic_set(msdc_port, MSDC_TRUE);
}
#endif /*HAL_SD_MODULE_ENABLED || HAL_SDIO_MODULE_ENABLED*/
