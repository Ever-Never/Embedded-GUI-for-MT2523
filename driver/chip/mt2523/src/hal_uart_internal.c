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

#include "hal_uart.h"

#ifdef HAL_UART_MODULE_ENABLED

#include "hal_pdma_internal.h"
#include "hal_nvic.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_uart_internal.h"
#include "hal_uart_internal_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

extern UART_REGISTER_T *g_uart_regbase[];

void uart_set_baudrate(UART_REGISTER_T *uartx, uint32_t actual_baudrate)
{
    uint32_t uart_clock, integer, remainder, fraction;
    uint32_t dll_dlm, sample_count, sample_point, temp_lcr;
    uint32_t fraction_L_mapping[] = {0x00, 0x00, 0x20, 0x90, 0xa8, 0x54, 0x6c, 0xba, 0xf6, 0xfe};
    uint32_t fraction_M_mapping[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01};

    uartx->RATEFIX_AD = 0x0;
    uartx->FRACDIV_L = 0x0;
    uartx->FRACDIV_M = 0x0;

    if (is_clk_use_lfosc() == true) {
        uart_clock = clock_get_freq_lfosc() * 1000;
    } else {
        uart_clock = UART_INPUT_CLOCK_DCXO;
    }

    integer = uart_clock / (actual_baudrate * 256);
    remainder = ((uart_clock * 10) / (actual_baudrate * 256)) % 10;
    if ((remainder != 0) || (integer == 0)) {
        integer += 1;
    }
    dll_dlm = integer;
    sample_count = uart_clock / (actual_baudrate * dll_dlm);

    while (sample_count > 256) {
        dll_dlm++;
        sample_count = uart_clock / (actual_baudrate * dll_dlm);
    }

    fraction = ((uart_clock * 10) / (actual_baudrate * dll_dlm)) % 10;

    sample_count -= 1;
    sample_point = (sample_count - 1) >> 1;

    uartx->HIGHSPEED = UART_HIGHSPEED_SPEED_MODE3;

    temp_lcr = uartx->LCR;
    uartx->LCR = temp_lcr | UART_LCR_DLAB_MASK;
    uartx->RBR_THR_DLL.DLL = dll_dlm & 0x00ff;
    uartx->IER_DLM.DLM = (dll_dlm >> 8) & 0x00ff;
    uartx->LCR = temp_lcr;

    uartx->SAMPLE_COUNT = sample_count;
    uartx->SAMPLE_POINT = sample_point;

    uartx->FRACDIV_L = fraction_L_mapping[fraction];
    uartx->FRACDIV_M = fraction_M_mapping[fraction];

    if (actual_baudrate >= 3000000) {
        uartx->GUARD = 0x12;    /* delay 2 bits per byte. */
    }
}

void uart_set_format(UART_REGISTER_T *uartx,
                     hal_uart_word_length_t word_length,
                     hal_uart_stop_bit_t stop_bit,
                     hal_uart_parity_t parity)
{
    uint16_t byte;

    /* DLAB start */
    byte = uartx->LCR;
    uartx->LCR = UART_LCR_DLAB_MASK;

    /* Setup wordlength */
    byte &= ~UART_LCR_WORD_MASK;
    switch (word_length) {
        case HAL_UART_WORD_LENGTH_5:
            byte |= UART_LCR_WORD_5BITS;
            break;
        case HAL_UART_WORD_LENGTH_6:
            byte |= UART_LCR_WORD_6BITS;
            break;
        case HAL_UART_WORD_LENGTH_7:
            byte |= UART_LCR_WORD_7BITS;
            break;
        case HAL_UART_WORD_LENGTH_8:
            byte |= UART_LCR_WORD_8BITS;
            break;
        default:
            break;
    }

    /* setup stop bit */
    byte &= ~UART_LCR_STB_MASK;
    switch (stop_bit) {
        case HAL_UART_STOP_BIT_1:
            byte |= UART_LCR_STB_1;
            break;
        case HAL_UART_STOP_BIT_2:
            byte |= UART_LCR_STB_2;
            break;
        default:
            break;
    }

    /* setup parity bit */
    byte &= ~UART_LCR_PARITY_MASK;
    switch (parity) {
        case HAL_UART_PARITY_NONE:
            byte &= ~UART_LCR_PEN_MASK;
            break;
        case HAL_UART_PARITY_ODD:
            byte |= UART_LCR_EPS_ODD;
            break;
        case HAL_UART_PARITY_EVEN:
            byte |= UART_LCR_EPS_EVEN;
            break;
        default:
            break;
    }

    /* DLAB End */
    uartx->LCR = byte;
}

void uart_put_char_block(UART_REGISTER_T *uartx, uint8_t byte)
{
    uint16_t LSR;

    while (1) {
        LSR = uartx->LSR_XON2.LSR;
        if (LSR & UART_LSR_THRE_MASK) {
            uartx->RBR_THR_DLL.THR = byte;
            break;
        }
    }
}

uint8_t uart_get_char_block(UART_REGISTER_T *uartx)
{
    uint16_t LSR;
    uint8_t byte;

    while (1) {
        LSR = uartx->LSR_XON2.LSR;
        if (LSR & UART_LSR_DR_MASK) {
            byte = (uint8_t)uartx->RBR_THR_DLL.RBR;
            break;
        }
    }

    return byte;
}

uint32_t uart_get_char_unblocking(UART_REGISTER_T *uartx)
{
    uint16_t LSR;
    uint32_t value;

    LSR = uartx->LSR_XON2.LSR;
    if (LSR & UART_LSR_DR_MASK) {
        value = uartx->RBR_THR_DLL.RBR;
    } else {
        value = 0xffffffff;
    }

    return value;
}

void uart_set_hardware_flowcontrol(UART_REGISTER_T *uartx)
{
    uint16_t EFR, LCR;

    LCR = uartx->LCR;

    uartx->LCR = 0xbf;
    EFR = uartx->IIR_FCR_EFR.EFR;
    EFR |= UART_EFR_AUTO_CTS_MASK | UART_EFR_AUTO_RTS_MASK;
    uartx->IIR_FCR_EFR.EFR = EFR;
    uartx->ESCAPE_EN = 0;

    uartx->LCR = 0x00;
    uartx->MCR_XON1.MCR = UART_MCR_RTS_MASK;

    uartx->LCR = LCR;
}

void uart_set_software_flowcontrol(UART_REGISTER_T *uartx,
                                   uint8_t xon,
                                   uint8_t xoff,
                                   uint8_t escape_character)
{
    uint16_t EFR, LCR;

    LCR = uartx->LCR;

    uartx->LCR = 0xbf;
    uartx->MCR_XON1.XON1 = xon;
    uartx->XOFF1 = xoff;
    EFR = uartx->IIR_FCR_EFR.EFR;
    EFR |= UART_EFR_ENABLE_E_MASK | UART_EFR_SW_FLOW_TX_ON | UART_EFR_SW_FLOW_RX_ON;
    uartx->IIR_FCR_EFR.EFR = EFR;
    uartx->ESCAPE_DAT = escape_character;
    uartx->ESCAPE_EN = UART_ESCAPE_EN_MASK;

    uartx->LCR = LCR;
}

void uart_disable_flowcontrol(UART_REGISTER_T *uartx)
{
    uint16_t LCR;

    LCR = uartx->LCR;

    uartx->LCR = 0xbf;
    uartx->IIR_FCR_EFR.EFR = 0x0;

    uartx->LCR = 0x00;
    uartx->LCR = LCR;
}

void uart_set_fifo(UART_REGISTER_T *uartx)
{
    uint16_t EFR, LCR;

    LCR = uartx->LCR;

    uartx->LCR = 0xBF;
    EFR = uartx->IIR_FCR_EFR.EFR;
    EFR |= UART_EFR_ENABLE_E_MASK;
    uartx->IIR_FCR_EFR.EFR = EFR;

    uartx->LCR = 0x00;
    uartx->IIR_FCR_EFR.FCR = UART_FCR_TXTRIG_1 |
                             UART_FCR_RXTRIG_12 |
                             UART_FCR_CLRT_MASK |
                             UART_FCR_CLRR_MASK |
                             UART_FCR_FIFOE_MASK;

    uartx->LCR = LCR;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void uart_set_sleep_mode(UART_REGISTER_T *uartx)
{
    uartx->SLEEP_EN = 0x01;
}

void uart_unmask_send_interrupt(UART_REGISTER_T *uartx)
{
    uint16_t IER, LCR;

    LCR = uartx->LCR;

    uartx->LCR &= ~UART_LCR_DLAB_MASK;
    IER = uartx->IER_DLM.IER;
    IER |= UART_IER_ETBEI_MASK;
    uartx->IER_DLM.IER = IER;

    uartx->LCR &= LCR;
}
#endif

void uart_unmask_receive_interrupt(UART_REGISTER_T *uartx)
{
    uint16_t IER, LCR;

    LCR = uartx->LCR;

    uartx->LCR &= ~UART_LCR_DLAB_MASK;
    IER = uartx->IER_DLM.IER;
    IER |= (UART_IER_ERBFI_MASK | UART_IER_ELSI_MASK);
    uartx->IER_DLM.IER = IER;

    uartx->LCR &= LCR;
}

void uart_purge_fifo(UART_REGISTER_T *uartx, int32_t is_rx)
{
    uint16_t FCR;

    FCR = UART_FCR_TXTRIG_4 | UART_FCR_RXTRIG_12 | UART_FCR_FIFOE_MASK;

    if (is_rx) {
        FCR |= UART_FCR_CLRR_MASK;
    } else {
        FCR |= UART_FCR_CLRT_MASK;
    }

    uartx->IIR_FCR_EFR.FCR = FCR;
}

uart_interrupt_type_t uart_query_interrupt_type(UART_REGISTER_T *uartx)
{
    uint16_t IIR, LSR;
    uart_interrupt_type_t type = UART_INTERRUPT_NONE;

    IIR = uartx->IIR_FCR_EFR.IIR;
    if (IIR & UART_IIR_NONE) {
        return type;
    }

    switch (IIR & UART_IIR_ID_MASK) {
        /* received data and timeout happen */
        case UART_IIR_RDT:
            type = UART_INTERRUPT_RECEIVE_TIMEOUT;
            break;
        /* receive line status changed Any of BI/FE/PE/OE becomes set */
        case UART_IIR_LSR:
            LSR = uartx->LSR_XON2.LSR;
            if (LSR & UART_LSR_BI_MASK) {
                type = UART_INTERRUPT_RECEIVE_BREAK;
            } else {
                type = UART_INTERRUPT_RECEIVE_ERROR;
            }
            break;
        /* TX Holding Register Empty */
        case UART_IIR_THRE:
            type = UART_INTERRUPT_SEND_AVAILABLE;
            break;
        default:
            break;
    }

    return type;
}

int32_t uart_verify_error(UART_REGISTER_T *uartx)
{
    uint16_t LSR;
    int32_t ret = 0;

    LSR = uartx->LSR_XON2.LSR;
    if (!(LSR & (UART_LSR_OE_MASK | UART_LSR_FE_MASK | UART_LSR_PE_MASK))) {
        ret = -1;
    }

    return ret;
}

void uart_clear_timeout_interrupt(UART_REGISTER_T *uartx)
{
    uint16_t DMA_EN;

    DMA_EN = uartx->DMA_EN;
    DMA_EN = DMA_EN;
}

void uart_reset_default_value(UART_REGISTER_T *uartx)
{
    uartx->LCR = 0xbf;
    uartx->IIR_FCR_EFR.EFR = 0x10;
    uartx->MCR_XON1.XON1 = 0x00;
    uartx->XOFF1 = 0x00;

    uartx->LCR = 0x80;
    uartx->RBR_THR_DLL.DLL = 0x00;
    uartx->IER_DLM.DLM = 0x00;

    uartx->LCR = 0x00;
    uartx->IER_DLM.IER = 0x00;
    uartx->IIR_FCR_EFR.FCR = 0x00;

    uartx->LCR = 0xbf;
    uartx->IIR_FCR_EFR.EFR = 0x00;
    uartx->LCR = 0x00;

    uartx->MCR_XON1.MCR = 0x00;
    uartx->SCR_XOFF2.SCR = 0x00;
    uartx->AUTOBAUD_EN = 0x00;
    uartx->HIGHSPEED = 0x00;
    uartx->SAMPLE_COUNT = 0x00;
    uartx->SAMPLE_POINT = 0x00;
    uartx->RATEFIX_AD = 0x00;
    uartx->AUTOBAUDSAMPLE = 0x00;
    uartx->GUARD = 0x00;
    uartx->ESCAPE_DAT = 0x00;
    uartx->ESCAPE_EN = 0x00;
    uartx->SLEEP_EN = 0x00;
    uartx->DMA_EN = 0x00;
    uartx->RATEFIX_AD = 0x00;
    uartx->FRACDIV_L = 0x00;
    uartx->FRACDIV_M = 0x00;
}

void uart_query_empty(UART_REGISTER_T *uartx)
{
    while (!((uartx->LSR_XON2.LSR) & 0x40));
}


#ifdef __cplusplus
}
#endif

#endif

