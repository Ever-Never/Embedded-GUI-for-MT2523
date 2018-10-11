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

#include "hal.h"
#include "hal_msdc.h"
#include "hal_gpio.h"
#if defined(HAL_SD_MODULE_ENABLED) || defined(HAL_SDIO_MODULE_ENABLED)
#include "hal_log.h"
#include "hal_clock_internal.h"
#include "hal_gpio_internal.h"
#include "hal_eint.h"
#include "assert.h"
#include "msdc_custom_config.h"
#include "hal_sd_internal.h"
#include "hal_sdio_internal.h"

msdc_config_t msdc_config;
extern volatile uint32_t msdc_interrupt_status;

#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
static volatile bool msdc_card_detection_eint_polarity;
extern const unsigned char HAL_MSDC_EINT;  /*ept tool config*/
#endif

void msdc_wait(uint32_t wait_ms)
{
    hal_gpt_delay_us(wait_ms * 500);
}


void msdc_power_set(bool is_power_on)
{
    if (is_power_on) {
        hal_clock_enable(HAL_CLOCK_CG_SDIOMST_BUS);
    } else {
        hal_clock_disable(HAL_CLOCK_CG_SDIOMST_BUS);
    }
}

void msdc_reset(void)
{
    if (!(MSDC_REG->MSDC_CFG & MSDC_CFG_RST)) {
        MSDC_REG->MSDC_CFG = MSDC_REG->MSDC_CFG | MSDC_CFG_RST;
    }

    while (MSDC_REG->MSDC_CFG & MSDC_CFG_RST);
}

void msdc_clear_fifo(void)
{
    MSDC_REG->MSDC_FIFOCS = MSDC_REG->MSDC_FIFOCS | MSDC_FIFOCS_CLR;

    while (MSDC_REG->MSDC_FIFOCS & MSDC_FIFOCS_CLR);
}

void msdc_set_bus_width(msdc_bus_width_t bus_width)
{
    MSDC_REG->SDC_CFG = (MSDC_REG->SDC_CFG & (~SDC_CFG_BUSWIDTH)) | (bus_width << SDC_CFG_BUSWIDTH_OFFSET);
}

void msdc_set_output_clock(uint32_t clock)
{
    uint32_t clock_config = 0;

    /*disable msdc clock*/
    hal_clock_disable(HAL_CLOCK_CG_SDIOMST);

    if (clock >= MSDC_CLOCK_SOURCE) {
        MSDC_REG->MSDC_CFG |= MSDC_CFG_CCKMD;
        msdc_config.output_clock = MSDC_CLOCK_SOURCE;
    } else if (clock >= (MSDC_CLOCK_SOURCE >> 1)) {
        MSDC_REG->MSDC_CFG &= ~MSDC_CFG_CCKMD;
        MSDC_REG->MSDC_CFG &= ~MSDC_CFG_CKDIV;
        msdc_config.output_clock = MSDC_CLOCK_SOURCE >> 1;
    } else {
        clock_config = ((MSDC_CLOCK_SOURCE + clock - 1) / clock);
        clock_config = (clock_config >> 2) + (((clock_config & 3) != 0) ? 1 : 0);
        msdc_config.output_clock = MSDC_CLOCK_SOURCE / (4 * clock_config);

        MSDC_REG->MSDC_CFG = (MSDC_REG->MSDC_CFG & (~MSDC_CFG_CKDIV)) | ((clock_config & 0xff) << 8);
    }

    /*config cmd response sample edge.*/
    if (msdc_config.output_clock > SDIO_DEFAULT_MAX_SPEED) {
        MSDC_REG->MSDC_IOCON &= ~MSDC_IOCON_RSPL;
    } else {
        MSDC_REG->MSDC_IOCON |= MSDC_IOCON_RSPL;
    }

    log_hal_info("setting clock is %d, MSDC_CFG = %x \r\n", clock, MSDC_REG->MSDC_CFG);

    /*enable msdc clock*/
    hal_clock_enable(HAL_CLOCK_CG_SDIOMST);

    msdc_wait(15);
}


uint32_t msdc_get_output_clock(void)
{
    return msdc_config.output_clock;
}


void msdc_sdio_interrupt_set(bool enable)
{
    if (enable) {
        MSDC_REG->MSDC_INTEN |= MSDC_INT_SDIOIRQ;
    } else {
        MSDC_REG->MSDC_INTEN &= ~MSDC_INT_SDIOIRQ;
    }
}

void msdc_data_interrupt_handle(uint32_t status)
{
    #if defined(HAL_SD_MODULE_ENABLED)
        if (MSDC_OWNER_SD == msdc_get_owner()) {
            if( status & MSDC_INT_XFER_COMPL ) {
                sd_wait_dma_interrupt_transfer_ready();
            }
            else if ((status & MSDC_INT_DATTMO) || (status & MSDC_INT_DATCRCERR)) {
                 if (msdc_config.msdc_sd_callback) {
                    /*call user callback to inform transfer error*/
                    if (status & MSDC_INT_DATTMO) {
                        msdc_config.msdc_sd_callback(HAL_SD_EVENT_DATA_TIMEOUT,(void *)0);
                    }
                    else if (status & MSDC_INT_DATCRCERR) {
                        msdc_config.msdc_sd_callback(HAL_SD_EVENT_CRC_ERROR,(void *)0);
                    }
                }
            }
       }
    #endif

    #if defined(HAL_SDIO_MODULE_ENABLED)
        if (MSDC_OWNER_SDIO == msdc_get_owner()) {
             if( status & MSDC_INT_XFER_COMPL ) {
                sdio_wait_dma_interrupt_transfer_ready();
            }
            else if ((status & MSDC_INT_DATTMO) || (status & MSDC_INT_DATCRCERR)) {
                 if (msdc_config.msdc_sdio_dma_interrupt_callback) {
                     /*call user callback to inform transfer error*/
                     if (status & MSDC_INT_DATCRCERR) {
                         msdc_config.msdc_sdio_dma_interrupt_callback(HAL_SDIO_EVENT_CRC_ERROR,(void *)0);
                     }
                     else if (status & MSDC_INT_DATTMO) {
                         msdc_config.msdc_sdio_dma_interrupt_callback(HAL_SDIO_EVENT_DATA_TIMEOUT,(void *)0);
                     }
                 }
            }
        }
    #endif
}

void msdc_command_interrupt_handle(uint32_t status)
{

}

void msdc_isr(hal_nvic_irq_t irq_number)
{
    uint32_t command_status  = MSDC_INT_CMDRDY | MSDC_INT_CMDTMO | MSDC_INT_RSPCRCERR;
    uint32_t acommand_status = MSDC_INT_ACMDRDY | MSDC_INT_ACMDTMO | MSDC_INT_ACMDCRCERR | MSDC_INT_ACMD19_DONE;
    uint32_t data_status  = MSDC_INT_XFER_COMPL | MSDC_INT_DXFER_DONE | MSDC_INT_DATCRCERR | MSDC_INT_DATTMO;
    uint32_t msdc_int;
    uint32_t msdc_inten;
    uint32_t interrupt_status;

    msdc_nvic_set(false);

    msdc_int = MSDC_REG->MSDC_INT;
    msdc_inten = MSDC_REG->MSDC_INTEN;
    interrupt_status = msdc_int & msdc_inten;

    MSDC_REG->MSDC_INT |= interrupt_status;

#if defined(HAL_SDIO_MODULE_ENABLED)
    msdc_interrupt_status = msdc_int;
#endif

    //log_hal_info("irq handler got msdc_int=%08X msdc_inten=%08X => %08X (in interrupt)\r\n", (unsigned int)msdc_int, (unsigned int)msdc_inten, (unsigned int)interrupt_status);

    /* transfer complete interrupt */
    if (interrupt_status & data_status) {
        msdc_data_interrupt_handle(interrupt_status);
    }

    /* command interrupts */
    if (interrupt_status & (command_status | acommand_status)) {
        msdc_command_interrupt_handle(interrupt_status);
    }

    /* sdio interrupt */
    if (interrupt_status & MSDC_INT_SDIOIRQ) {
        log_hal_info("SDIOIRQ interrupt\r\n");
    }

    msdc_nvic_set(true);
}

void msdc_nvic_set(bool enable)
{
    /*should call nvic api to set msdc interrupt enable or disable*/
    if (enable) {
        hal_nvic_enable_irq(SDIO_MST_IRQn);
    } else {
        hal_nvic_disable_irq(SDIO_MST_IRQn);
    }
}

void msdc_interrupt_init(void)
{
    uint32_t reg_value = 0;

    msdc_nvic_set(false);

    hal_nvic_register_isr_handler(SDIO_MST_IRQn, msdc_isr);

    /* Disable and clear all interrupts */
    reg_value = MSDC_REG->MSDC_INTEN;
    MSDC_REG->MSDC_INTEN &= ~reg_value;
    reg_value = MSDC_REG->MSDC_INT;
    MSDC_REG->MSDC_INT |= reg_value;

    msdc_nvic_set(true);
}

#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
void msdc_eint_isr(void *user_data)
{
    hal_eint_trigger_mode_t mode;
    hal_sd_card_event_t sd_event = HAL_SD_EVENT_CARD_INSERTED;

    hal_eint_mask((hal_eint_number_t)HAL_MSDC_EINT);

    msdc_config.is_card_present = msdc_card_detection_eint_polarity ? false : true;

    msdc_card_detection_eint_polarity = msdc_card_detection_eint_polarity ? false : true;

    mode = msdc_card_detection_eint_polarity ? HAL_EINT_LEVEL_HIGH : HAL_EINT_LEVEL_LOW;
    hal_eint_set_trigger_mode((hal_eint_number_t)HAL_MSDC_EINT, mode);

    hal_eint_unmask((hal_eint_number_t)HAL_MSDC_EINT);

    if (msdc_config.is_card_present == false) {
        msdc_config.is_card_plug_out = true;
        sd_event = HAL_SD_EVENT_CARD_REMOVED;
        msdc_reset();
        msdc_dma_disable();
        msdc_deinit();
    } else {
        msdc_config.is_card_plug_out = false;
        sd_event = HAL_SD_EVENT_CARD_INSERTED;
    }

    if (msdc_config.msdc_card_detect_callback != NULL) {
        msdc_config.msdc_card_detect_callback(sd_event, msdc_config.card_detect_user_data);
    }
}

void msdc_eint_registration(void)
{
    hal_eint_config_t config;

    /*HAL_MSDC_EINT is EPT tool config, the HAL_MSDC_EINT value is 0xff means the hot plug eint is not configured in EPT tool*/
    if (0xFF == HAL_MSDC_EINT) {
        assert(0);
    }

    msdc_card_detection_eint_polarity = false;

    config.debounce_time = 500;
    config.trigger_mode = HAL_EINT_LEVEL_LOW;
  
    hal_eint_mask((hal_eint_number_t)HAL_MSDC_EINT);

    if(HAL_EINT_STATUS_OK != hal_eint_init((hal_eint_number_t)HAL_MSDC_EINT, &config))
    {
        log_hal_error("ERROR:hal_eint_init error!\r\n");
    }
    if(HAL_EINT_STATUS_OK != hal_eint_register_callback ((hal_eint_number_t)HAL_MSDC_EINT, msdc_eint_isr, NULL))
    {
        log_hal_error("ERROR:hal_eint_register_callback error!\r\n");
    }

    hal_eint_unmask((hal_eint_number_t)HAL_MSDC_EINT);

}
#endif

void msdc_io_config(msdc_bus_width_t bus_width)
{
    if (MSDC_BUS_WIDTH_1BITS == bus_width) {
        hal_pinmux_set_function(HAL_GPIO_11, 4);// MC0_CK
        hal_pinmux_set_function(HAL_GPIO_12, 4);// MC0_CM0
        hal_pinmux_set_function(HAL_GPIO_13, 4);// MC0_DA0

        /*PU/PD setting.*/
        hal_gpio_set_pupd_register(HAL_GPIO_11, 1, 0, 1);
        hal_gpio_set_pupd_register(HAL_GPIO_12, 0, 0, 1);
        hal_gpio_set_pupd_register(HAL_GPIO_13, 0, 0, 1);

        /*IO driving setting.*/
        hal_gpio_set_driving_current(HAL_GPIO_11, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_GPIO_12, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_GPIO_13, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
    }
    else if (MSDC_SDIO_BUS_WIDTH_4BITS == bus_width) {
        hal_pinmux_set_function(HAL_GPIO_11, 4);// MC0_CK
        hal_pinmux_set_function(HAL_GPIO_12, 4);// MC0_CM0
        hal_pinmux_set_function(HAL_GPIO_13, 4);// MC0_DA0
        hal_pinmux_set_function(HAL_GPIO_14, 4);// MC0_DA1
        hal_pinmux_set_function(HAL_GPIO_15, 4);// MC0_DA2
        hal_pinmux_set_function(HAL_GPIO_16, 4);// MC0_DA3

        /*PU/PD setting.*/
        hal_gpio_set_pupd_register(HAL_GPIO_11, 1, 0, 1);
        hal_gpio_set_pupd_register(HAL_GPIO_12, 0, 0, 1);
        hal_gpio_set_pupd_register(HAL_GPIO_13, 0, 0, 1);
        hal_gpio_set_pupd_register(HAL_GPIO_14, 0, 0, 1);
        hal_gpio_set_pupd_register(HAL_GPIO_15, 0, 0, 1);
        hal_gpio_set_pupd_register(HAL_GPIO_16, 0, 0, 1);

        /*IO driving setting.*/
        hal_gpio_set_driving_current(HAL_GPIO_11, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_GPIO_12, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_GPIO_13, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_GPIO_14, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_GPIO_15, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
        hal_gpio_set_driving_current(HAL_GPIO_16, (hal_gpio_driving_current_t)MSDC_DATA_LINE_DRIVING_CAPABILITY);
    }
    else {
        log_hal_error("msdc io config error!\r\n");
    }
}

msdc_status_t msdc_init(msdc_bus_width_t bus_width)
{
    msdc_status_t status = MSDC_OK;
    uint32_t reg_value = 0;

    msdc_power_set(true);

    msdc_io_config(bus_width);

    clock_mux_sel(CLK_SDIOMST_SEL, 2); /*0:xo_ck(20M), 1:bbpll2_d5_d8(24M), 2:bbpll2_d5_d4(48M)*/

    msdc_config.msdc_clock = MSDC_CLOCK_SOURCE;

    msdc_set_output_clock(240);

    /* Reset */
    msdc_reset();
    msdc_clear_fifo();

    /* Disable card detection */
    MSDC_REG->MSDC_PS = MSDC_REG->MSDC_PS & (~MSDC_PS_CDEN);

    /* Disable and clear all interrupts */
    reg_value = MSDC_REG->MSDC_INTEN;
    MSDC_REG->MSDC_INTEN &= ~reg_value;
    reg_value = MSDC_REG->MSDC_INT;
    MSDC_REG->MSDC_INT |= reg_value;

    msdc_interrupt_init();

    /* Configure to PIO mode */
    MSDC_REG->MSDC_CFG = MSDC_REG->MSDC_CFG | MSDC_CFG_PIO;

    /* Configure to MMC/SD mode */
    MSDC_REG->MSDC_CFG = MSDC_REG->MSDC_CFG | MSDC_CFG_MODE;

    /* Configure to enable SDIO mode. it's must otherwise sdio command failed */
    MSDC_REG->SDC_CFG = MSDC_REG->SDC_CFG | SDC_CFG_SDIO;
    MSDC_REG->SDC_CFG = MSDC_REG->SDC_CFG | SDC_CFG_SDIOIDE;

    /* write crc timeout detection */
    MSDC_REG->PATCH_BIT0 = MSDC_REG->PATCH_BIT0 | (1 << 30);

    /*switch INCR1 to single burst.*/
    MSDC_REG->PATCH_BIT1 |= (1 << 16);

    /* Configure to default data timeout */
    MSDC_REG->SDC_CFG = (MSDC_REG->SDC_CFG & (~SDC_CFG_DTOC)) | (80 << 24);

    msdc_set_bus_width(MSDC_BUS_WIDTH_1BITS);

    /*set block number.*/
    MSDC_REG->SDC_BLK_NUM = 1;

    msdc_config.is_card_present = true;

#if defined(HAL_SD_MODULE_ENABLED) && defined(HAL_SD_CARD_DETECTION)
    /*card detection eint registration*/
    msdc_eint_registration();
    log_hal_info("msdc eint register done\r\n");
#endif

    log_hal_info("init hardware done\r\n");

    return status;
}

void msdc_deinit(void)
{
    uint32_t reg_value = 0;

    /* Disable and clear all interrupts */
    reg_value = MSDC_REG->MSDC_INTEN;
    MSDC_REG->MSDC_INTEN &= ~reg_value;
    reg_value = MSDC_REG->MSDC_INT;
    MSDC_REG->MSDC_INT |= reg_value;
    msdc_power_set(false);

    msdc_config.is_card_present = false;

    log_hal_info("deinit hardware done");
}


bool msdc_card_is_present(void)
{
    return msdc_config.is_card_present;
}

void msdc_dma_enable(void)
{
    MSDC_REG->MSDC_CFG &= ~MSDC_CFG_PIO;
}

void msdc_dma_disable(void)
{
    MSDC_REG->MSDC_CFG |= MSDC_CFG_PIO;
}

msdc_owner_t msdc_get_owner(void)
{
    return (msdc_config.owner);
}

void msdc_set_owner(msdc_owner_t owner)
{
    msdc_config.owner = owner;
}

/*this function used to turn on power for card VDD and VDDQ*/
void msdc_card_power_set(bool is_power_on)
{
    /*card power set.*/
}

void msdc_clock_init(void)
{
    /*in 50MHZ case, we should set 80 to have at least 100ms timeout,for initial read*/
    MSDC_REG->SDC_CFG = (MSDC_REG->SDC_CFG & (~SDC_CFG_DTOC)) |
                        (MSDC_DATA_TIMEOUT_COUNT << SDC_CFG_DTOC_OFFSET);

    msdc_config.msdc_clock = MSDC_CLOCK;

    /*set output clock to 240KHz. The clock should <= 400KHz,*/
    msdc_set_output_clock(MSDC_INIT_CLOCK);

}

void msdc_set_burst_type(msdc_burst_type_t burst_type)
{
    MSDC_REG->DMA_CTRL = (MSDC_REG->DMA_CTRL & (~MSDC_DMA_CTRL_BRUSTSZ)) | (burst_type << 12);
}

#endif /*HAL_SD_MODULE_ENABLED || HAL_SDIO_MODULE_ENABLED*/

