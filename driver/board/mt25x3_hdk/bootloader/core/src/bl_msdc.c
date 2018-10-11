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

#include "bl_msdc.h"

#if defined(BL_SD_MODULE_ENABLED)
#include "bl_clock.h"
#include "hal_pmu_internal.h"
#include "bl_pmic.h"

/*if define MSDC1_PAD_CAMERA,  it means MSDC1 pin is from GPIO24~GPIO29*/
#define MSDC1_PAD_CAMERA

#define BL_MSDC_SOURCE_CLOCK  (89100) /*KHz*/

msdc_config_t msdc_config[MSDC_PORT_MAX];


volatile msdc_io_parameter_t msdc_io_driving_capability = {
    MSDC_IO_8MA,
    MSDC_IO_8MA,
    MSDC_IO_SLEW_FAST,
    MSDC_IO_SLEW_FAST
};


void msdc_wait(uint32_t wait_ms)
{
    uint32_t counter = 0x1000;

    counter = counter * wait_ms;

    while (counter--);
}


void msdc_reset(msdc_port_t msdc_port)
{
    msdc_register_t *msdc_register_base;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    if (!(msdc_register_base->MSDC_CFG & MSDC_CFG_RST_MASK)) {
        msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_RST_MASK;
    }

    while (msdc_register_base->MSDC_CFG & MSDC_CFG_RST_MASK);
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

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    if (0 == msdc_clock) {
        return;
    }

    clock_config = ((BL_MSDC_SOURCE_CLOCK + msdc_clock - 1) / msdc_clock);
    clock_config = (clock_config >> 2) + (((clock_config & 3) != 0) ? 1 : 0);

    if (0 != clock_config) {
        msdc_config[msdc_port].output_clock = BL_MSDC_SOURCE_CLOCK / (4 * clock_config);
    }

    msdc_cfg = (clock_config << MSDC_CFG_SCLKF_OFFSET);

    /*disable serial interface*/
    msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG & (~SDC_CFG_SIEN_MASK);

    /*set output clock*/
    msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~(MSDC_CFG_SCLKF_MASK))) | msdc_cfg;

    /*update source clock mux select*/
    *(volatile uint32_t *)0xA21D0150 = *(volatile uint32_t *)0xA21D0150 | (1 << 10);
    while (*(volatile uint32_t *)0xA21D0150 & (1 << 10));


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
    if (is_power_on == MSDC_FALSE) { /*enable clock*/
        if (MSDC_PORT_0 == msdc_port) {
            *(volatile uint32_t *)0xA2010314 = 1 << 3; /*PDN_CLRD1,clear clock gating off */
        } else if (MSDC_PORT_1 == msdc_port) {
            *(volatile uint32_t *)0xA2010318 = 1 << 10; /*PDN_CLRD2,clear clock gating off */
        }
    } else {
        if (MSDC_PORT_0 == msdc_port) {
            *(volatile uint32_t *)0xA2010324 = 1 << 3;  /*PDN_SETD1,set clock gating off */
        } else if (MSDC_PORT_1 == msdc_port) {
            *(volatile uint32_t *)0xA2010328 = 1 << 10; /*PDN_SETD2,set clock gating off */
        }
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
    /*NVIC masked in bootloader*/
}

void msdc_interrupt_init(msdc_port_t msdc_port)
{
    msdc_register_t *msdc_register_base;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;

    msdc_nvic_set(msdc_port, MSDC_FALSE); /*need modify*/

    /*clear MSDC interrupt status, MSDC_INT is read-clear*/
    msdc_register_base->MSDC_INT;

    /*disable MSDC interrupt*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG & (~MSDC_CFG_INTEN_MASK);

    /*disable data request interrupt*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG & (~MSDC_CFG_DIRQEN_MASK);

    msdc_register_base->SDC_IRQMASK0 = 0xFFFFFFFF;

    /*enable MSDC interrupt*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_INTEN_MASK;

    msdc_nvic_set(msdc_port, MSDC_TRUE); /*need modify*/
}


bool msdc_card_is_present(msdc_port_t msdc_port)
{
    return (msdc_config[msdc_port].is_card_present);
}


void msdc_dma_wait_transfer_complete(msdc_port_t msdc_port)
{
}


void msdc_dma_init(msdc_port_t msdc_port)
{
}


void msdc_dma_config(msdc_port_t msdc_port, uint32_t address, uint32_t count, bool is_write)
{
}


void msdc_dma_enable(msdc_port_t msdc_port)
{
}

void msdc_dma_disable(msdc_port_t msdc_port)
{
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
    if (is_power_on) {
        bl_pmic_vmc_vr_control(MSDC_TRUE);
    } else {
        bl_pmic_vmc_vr_control(MSDC_FALSE);
    }

}

msdc_status_t msdc_init(msdc_port_t msdc_port)
{
    msdc_register_t *msdc_register_base;

    msdc_register_base = (MSDC_PORT_0 == msdc_port) ? MSDC0_REG : MSDC1_REG;


    /*if MSDC have initialized, return directly*/
    if (msdc_config[msdc_port].is_initialized) {
        return MSDC_INITIALIZED;
    }

    if (MSDC_PORT_0 == msdc_port) {
        *(volatile uint32_t *)0xA2020c34 = 0x55 << 24; /*MSDC_CLK & MSDC_CMD Set pin mode 5*/
        *(volatile uint32_t *)0xA2020c44 = 0x5555; /*MSDC_DA Set pin mode 5*/
    } else if (MSDC_PORT_1 == msdc_port) {
#ifdef MSDC1_PAD_CAMERA
        *(volatile uint32_t *)0xA2020c34 = 0x888888; /*Set pin mode 8*/

        *(volatile uint32_t *)0xa2010234 = *(volatile uint32_t *)0xa2010234 | 1; /*select CAMERA PAD for MSDC1*/
#else
        *(volatile uint32_t *)0xA2020c04 = 0x2222 << 16; /*Set pin mode 2*/
        *(volatile uint32_t *)0xA2020c14 = 0x22; /*Set pin mode 2*/
#endif
    }

    /*turn on MSDC clock*/
    msdc_power_set(msdc_port, MSDC_TRUE);

    msdc_config[msdc_port].msdc_clock = MSDC_CLOCK;

    /*enable MSDC and enable interrupt pin*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | (MSDC_CFG_MSDC_MASK | MSDC_CFG_PINEN_MASK);

    /*config MSDC io pin driving capability*/
    msdc_set_io_parameter(msdc_port, (msdc_io_parameter_t *)(&msdc_io_driving_capability));

    msdc_wait(1);

    /*config MSDC fifo threshold to 1 word*/
    msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_FIFOTHD_MASK)) |
                                   (MSDC_FIFO_THRESHOLD_1 << MSDC_CFG_FIFOTHD_OFFSET);

    msdc_config[msdc_port].is_card_present = MSDC_TRUE;

    /*turn on power for card VDD and VDDQ*/
    msdc_card_power_set(msdc_port, MSDC_TRUE);

    /*init MSDC interrupt*/
    msdc_interrupt_init(msdc_port);

    /*MSDC dma init*/
    msdc_dma_init(msdc_port);

    msdc_config[msdc_port].is_initialized = MSDC_TRUE;

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
}

#endif /*BL_SD_MODULE_ENABLED || HAL_SDIO_MODULE_ENABLED*/

