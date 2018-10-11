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

#include "hal_uart_internal.h"

//#include "hal_pdma_internal.h"
#include "hal_nvic.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"

#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT
#include "hal_gpio.h"
#include "hal_eint.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

UART_REGISTER_T *const g_uart_regbase[] = {UART0, UART1, UART2};
const hal_clock_cg_id g_uart_port_to_pdn[] = {HAL_CLOCK_CG_UART0, HAL_CLOCK_CG_UART1, HAL_CLOCK_CG_UART2};
const hal_nvic_irq_t g_uart_port_to_irq_num[] = {UART0_IRQn, UART1_IRQn, UART2_IRQn};

#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT
/* extern GPIOs and EINTs var declare */
extern const unsigned char HAL_UART0_DTE_RI_EINT;
extern const unsigned char HAL_UART0_DTE_DCD_EINT;
extern const unsigned char HAL_UART0_DTE_DSR_EINT;
extern const char HAL_UART0_DTE_DCD_PIN;
extern const char HAL_UART0_DTE_DSR_PIN;
extern const char HAL_UART0_DTE_DTR_PIN;
extern const char HAL_UART0_DTE_RI_PIN;

extern const unsigned char HAL_UART1_DTE_RI_EINT;
extern const unsigned char HAL_UART1_DTE_DCD_EINT;
extern const unsigned char HAL_UART1_DTE_DSR_EINT;
extern const char HAL_UART1_DTE_DCD_PIN;
extern const char HAL_UART1_DTE_DSR_PIN;
extern const char HAL_UART1_DTE_DTR_PIN;
extern const char HAL_UART1_DTE_RI_PIN;

extern const unsigned char HAL_UART2_DTE_RI_EINT;
extern const unsigned char HAL_UART2_DTE_DCD_EINT;
extern const unsigned char HAL_UART2_DTE_DSR_EINT;
extern const char HAL_UART2_DTE_DCD_PIN;
extern const char HAL_UART2_DTE_DSR_PIN;
extern const char HAL_UART2_DTE_DTR_PIN;
extern const char HAL_UART2_DTE_RI_PIN;

void uart_dte_init_config(uart_dte_config_t *uart_dte_config)
{
    uart_dte_config[HAL_UART_0].dsr_pin = (hal_gpio_pin_t)HAL_UART0_DTE_DSR_PIN;
    uart_dte_config[HAL_UART_0].dcd_pin = (hal_gpio_pin_t)HAL_UART0_DTE_DCD_PIN;
    uart_dte_config[HAL_UART_0].dtr_pin = (hal_gpio_pin_t)HAL_UART0_DTE_DTR_PIN;
    uart_dte_config[HAL_UART_0].ri_pin = (hal_gpio_pin_t)HAL_UART0_DTE_RI_PIN;
    uart_dte_config[HAL_UART_0].dsr_eint = (hal_eint_number_t)HAL_UART0_DTE_DSR_EINT;
    uart_dte_config[HAL_UART_0].ri_eint = (hal_eint_number_t)HAL_UART0_DTE_RI_EINT;
    uart_dte_config[HAL_UART_0].dcd_eint = (hal_eint_number_t)HAL_UART0_DTE_DCD_EINT;
    uart_dte_config[HAL_UART_0].uart_port = HAL_UART_0;

    uart_dte_config[HAL_UART_1].dsr_pin = (hal_gpio_pin_t)HAL_UART1_DTE_DSR_PIN;
    uart_dte_config[HAL_UART_1].dcd_pin = (hal_gpio_pin_t)HAL_UART1_DTE_DCD_PIN;
    uart_dte_config[HAL_UART_1].dtr_pin = (hal_gpio_pin_t)HAL_UART1_DTE_DTR_PIN;
    uart_dte_config[HAL_UART_1].ri_pin = (hal_gpio_pin_t)HAL_UART1_DTE_RI_PIN;
    uart_dte_config[HAL_UART_1].dsr_eint = (hal_eint_number_t)HAL_UART1_DTE_DSR_EINT;
    uart_dte_config[HAL_UART_1].ri_eint = (hal_eint_number_t)HAL_UART1_DTE_RI_EINT;
    uart_dte_config[HAL_UART_1].dcd_eint = (hal_eint_number_t)HAL_UART1_DTE_DCD_EINT;
    uart_dte_config[HAL_UART_1].uart_port = HAL_UART_1;

    uart_dte_config[HAL_UART_2].dsr_pin = (hal_gpio_pin_t)HAL_UART2_DTE_DSR_PIN;
    uart_dte_config[HAL_UART_2].dcd_pin = (hal_gpio_pin_t)HAL_UART2_DTE_DCD_PIN;
    uart_dte_config[HAL_UART_2].dtr_pin = (hal_gpio_pin_t)HAL_UART2_DTE_DTR_PIN;
    uart_dte_config[HAL_UART_2].ri_pin = (hal_gpio_pin_t)HAL_UART2_DTE_RI_PIN;
    uart_dte_config[HAL_UART_2].dsr_eint = (hal_eint_number_t)HAL_UART2_DTE_DSR_EINT;
    uart_dte_config[HAL_UART_2].ri_eint = (hal_eint_number_t)HAL_UART2_DTE_RI_EINT;
    uart_dte_config[HAL_UART_2].dcd_eint = (hal_eint_number_t)HAL_UART2_DTE_DCD_EINT;
    uart_dte_config[HAL_UART_2].uart_port = HAL_UART_2;
}
#endif

vdma_channel_t uart_port_to_dma_channel(hal_uart_port_t uart_port, int32_t is_rx)
{
    vdma_channel_t dma_channel = VDMA_UART0RX;

    switch (uart_port) {
        case HAL_UART_0:
            if (is_rx) {
                dma_channel = VDMA_UART0RX;
            } else {
                dma_channel = VDMA_UART0TX;
            }
            break;
        case HAL_UART_1:
            if (is_rx) {
                dma_channel = VDMA_UART1RX;
            } else {
                dma_channel = VDMA_UART1TX;
            }
            break;
        case HAL_UART_2:
            if (is_rx) {
                dma_channel = VDMA_UART2RX;
            } else {
                dma_channel = VDMA_UART2TX;
            }
            break;
        default:
            break;
    }

    return dma_channel;
}

void uart_dma_channel_to_callback_data(vdma_channel_t dma_channel, uart_dma_callback_data_t *user_data)
{
    switch (dma_channel) {
        case VDMA_UART0RX:
            user_data->is_rx = true;
            user_data->uart_port = HAL_UART_0;
            break;
        case VDMA_UART1RX:
            user_data->is_rx = true;
            user_data->uart_port = HAL_UART_1;
            break;
        case VDMA_UART2RX:
            user_data->is_rx = true;
            user_data->uart_port = HAL_UART_2;
            break;
        case VDMA_UART0TX:
            user_data->is_rx = false;
            user_data->uart_port = HAL_UART_0;
            break;
        case VDMA_UART1TX:
            user_data->is_rx = false;
            user_data->uart_port = HAL_UART_1;
            break;
        case VDMA_UART2TX:
            user_data->is_rx = false;
            user_data->uart_port = HAL_UART_2;
            break;
        default:
            break;
    }
}

void uart_enable_dma(UART_REGISTER_T *uartx)
{
    uartx->DMA_CON_UNION.DMA_CON_CELLS.TX_DMA_EN = UART_DMA_CON_TX_DMA_EN_MASK;
    uartx->DMA_CON_UNION.DMA_CON_CELLS.RX_DMA_EN = UART_DMA_CON_RX_DMA_EN_MASK;
}

void uart_interrupt_handler(hal_nvic_irq_t irq_number)
{
    hal_uart_port_t uart_port;
    uart_interrupt_type_t type;
    UART_REGISTER_T *uartx = UART0;

    switch (irq_number) {
        case UART0_IRQn:
            uart_port = HAL_UART_0;
            break;
        case UART1_IRQn:
            uart_port = HAL_UART_1;
            break;
        case UART2_IRQn:
            uart_port = HAL_UART_2;
            break;
        default:
            uart_port = HAL_UART_0;
    }

    uartx = g_uart_regbase[uart_port];
    type = uart_query_interrupt_type(uartx);

    switch (type) {
        /* received data and timeout happen */
        case UART_INTERRUPT_RECEIVE_TIMEOUT:
            uart_receive_handler(uart_port, true);
            break;
        /* receive line status changed Any of BI/FE/PE/OE becomes set */
        case UART_INTERRUPT_RECEIVE_ERROR:
            uart_error_handler(uart_port);
            break;
        /* received data or received Trigger level reached */
        case UART_INTERRUPT_RECEIVE_AVAILABLE:
            //uart_receive_handler(uart_port, false);
            break;
        /* false interrupt detect */
        case UART_INTERRUPT_NONE:
            break;
        /* received break signal */
        case UART_INTERRUPT_RECEIVE_BREAK:
            uart_purge_fifo(uartx, 1);
            break;
        /* TX holding register is empty or the TX FIFO reduce to it's trigger level */
        case UART_INTERRUPT_SEND_AVAILABLE:
#ifdef HAL_SLEEP_MANAGER_ENABLED
            uart_send_handler(uart_port, true);
            break;
#endif
        /* detect hardware flow control request (CTS is high) */
        case UART_INTERRUPT_HARDWARE_FLOWCONTROL:
        /* an XOFF character has been received */
        case UART_INTERRUPT_SOFTWARE_FLOWCONTROL:
        default:
            UART_ASSERT();
    }
}


void uart_set_baudrate(UART_REGISTER_T *uartx, uint32_t actual_baudrate)
{
    uint32_t uart_clock, integer, remainder, fraction;
    uint32_t dll_dlm, sample_count, sample_point;
    uint32_t fraction_mapping[] = {0x00, 0x00, 0x20, 0x90, 0xa8, 0x154, 0x16c, 0x1ba, 0x1f6, 0x1fe};

    uartx->RATEFIX_UNION.RATEFIX = 0x0;
    uartx->FRACDIV = (~UART_FRACDIV_MASK);

    if (hal_clock_fxo_is_26m() == true) {
        uart_clock = 26000000;
    }
    else {
        uart_clock = 20000000;
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

    uartx->DLM_DLL = dll_dlm & 0x0000ffff;

    uartx->SAMPLE_REG_UNION.SAMPLE_REG = (sample_point << UART_SAMPLE_REG_SAMPLE_POINT_OFFSET) | sample_count;

    uartx->FRACDIV = fraction_mapping[fraction];

    if (actual_baudrate >= 3000000) {
        uartx->GUARD = 0x12;    /* delay 2 bits per byte. */
    }
}

void uart_set_format(UART_REGISTER_T *uartx,
                     hal_uart_word_length_t word_length,
                     hal_uart_stop_bit_t stop_bit,
                     hal_uart_parity_t parity)
{
    uint8_t byte = 0;

    /* Setup wordlength */
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
            byte |= UART_LCR_WORD_8BITS;
            break;
    }

    /* setup stop bit */
    switch (stop_bit) {
        case HAL_UART_STOP_BIT_1:
            byte |= UART_LCR_STB_1;
            break;
        case HAL_UART_STOP_BIT_2:
            byte |= UART_LCR_STB_2;
            break;
        default:
            byte |= UART_LCR_STB_1;
            break;
    }

    /* setup parity bit */
    switch (parity) {
        case HAL_UART_PARITY_NONE:
            byte |= UART_LCR_PAR_NONE;
            break;
        case HAL_UART_PARITY_ODD:
            byte |= UART_LCR_PAR_ODD;
            break;
        case HAL_UART_PARITY_EVEN:
            byte |= UART_LCR_PAR_EVEN;
            break;
        default:
            byte |= UART_LCR_PAR_NONE;
            break;
    }

    /* DLAB End */
    uartx->LCR_UNION.LCR_CELLS.PAR_STB_WLS = byte;
}

void uart_put_char_block(UART_REGISTER_T *uartx, uint8_t byte)
{
    uint32_t LSR;

    while (1) {
        LSR = uartx->LSR;
        if (LSR & UART_LSR_THRE_MASK) {
            uartx->THR = byte;
            break;
        }
    }
}

int uart_probe_char(UART_REGISTER_T *uartx)
{
    if (uartx->LSR & UART_LSR_DR_MASK) {
        return (int)uartx->RBR;
    }

    return -1;
}

uint8_t uart_get_char_block(UART_REGISTER_T *uartx)
{
    uint32_t LSR;
    uint8_t byte;

    while (1) {
        LSR = uartx->LSR;
        if (LSR & UART_LSR_DR_MASK) {
            byte = (uint8_t)uartx->RBR;
            break;
        }
    }

    return byte;
}

uint32_t uart_get_char_unblocking(UART_REGISTER_T *uartx)
{
    uint32_t LSR;
    uint32_t value;

    LSR = uartx->LSR;
    if (LSR & UART_LSR_DR_MASK) {
        value = uartx->RBR;
    } else {
        value = 0xffffffff;
    }

    return value;
}

void uart_set_hardware_flowcontrol(UART_REGISTER_T *uartx)
{
    uartx->ESCAPE_REG_UNION.ESCAPE_REG = (~UART_ESCAPE_REG_CHAR_MASK);
    uartx->MCR_UNION.MCR_CELLS.RTS = UART_MCR_RTS_MASK;
    uartx->EFR_UNION.EFR_CELLS.HW_FLOW_CONT = (UART_EFR_HW_TX_FLOWCTRL_MASK |
            UART_EFR_HW_RX_FLOWCTRL_MASK);
}

void uart_set_software_flowcontrol(UART_REGISTER_T *uartx,
                                   uint8_t xon,
                                   uint8_t xoff,
                                   uint8_t escape_character)
{
    uartx->XON_XOFF_UNION.XON_XOFF = (xon << UART_XON_XOFF_XONCHAR_OFFSET) |
                                     (xoff << UART_XON_XOFF_XOFFCHAR_OFFSET);
    uartx->ESCAPE_REG_UNION.ESCAPE_REG = (0x1 << UART_ESCAPE_REG_EN_OFFSET) |
                                         (escape_character << UART_ESCAPE_REG_CHAR_OFFSET);
    uartx->EFR_UNION.EFR_CELLS.SW_FLOW_CONT = (UART_EFR_SW_TX_FLOWCTRL_MASK |
            UART_EFR_SW_RX_FLOWCTRL_MASK);
}

void uart_disable_flowcontrol(UART_REGISTER_T *uartx)
{
    uartx->EFR_UNION.EFR_CELLS.SW_FLOW_CONT = 0x0;
}

void uart_set_fifo(UART_REGISTER_T *uartx)
{
    uartx->FCR_UNION.FCR = UART_FCR_TXTRIG_1 |
                           UART_FCR_RXTRIG_12 |
                           UART_FCR_CLRT_MASK |
                           UART_FCR_CLRR_MASK |
                           UART_FCR_FIFOE_MASK;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void uart_set_sleep_mode(UART_REGISTER_T *uartx)
{
    uartx->SLEEP_REG= UART_SLEEP_EN_MASK;
}

void uart_unmask_send_interrupt(UART_REGISTER_T *uartx)
{
    uartx->IER_UNION.IER_CELLS.ETBEI = UART_IER_ETBEI_MASK;
}
#endif

void uart_unmask_receive_interrupt(UART_REGISTER_T *uartx)
{
    uartx->IER_UNION.IER_CELLS.ELSI_ERBFI = (UART_IER_ELSI_MASK | UART_IER_ERBFI_MASK);
}

void uart_mask_receive_interrupt(UART_REGISTER_T *uartx)
{
    uartx->IER_UNION.IER_CELLS.ELSI_ERBFI = ~(UART_IER_ELSI_MASK | UART_IER_ERBFI_MASK);
}

void uart_purge_fifo(UART_REGISTER_T *uartx, int32_t is_rx)
{
    uint32_t FCR = 0;

    FCR = UART_FCR_TXTRIG_4 | UART_FCR_RXTRIG_12 | UART_FCR_FIFOE_MASK;

    if (is_rx) {
        FCR |= UART_FCR_CLRR_MASK;
    } else {
        FCR |= UART_FCR_CLRT_MASK;
    }

    uartx->FCR_UNION.FCR = FCR;
}

uart_interrupt_type_t uart_query_interrupt_type(UART_REGISTER_T *uartx)
{
    uint32_t IIR, LSR;
    uart_interrupt_type_t type = UART_INTERRUPT_NONE;

    IIR = uartx->IIR;
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
            LSR = uartx->LSR;
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
/* if error happened, return 0*/
int32_t uart_verify_error(UART_REGISTER_T *uartx)
{
    uint32_t LSR;
    int32_t ret = 0;

    LSR = uartx->LSR;
    if (!(LSR & (UART_LSR_OE_MASK | UART_LSR_FE_MASK | UART_LSR_PE_MASK))) {
        ret = -1;
    }

    return ret;
}

void uart_clear_timeout_interrupt(UART_REGISTER_T *uartx)
{
    uint32_t DMA_CON;

    DMA_CON = uartx->DMA_CON_UNION.DMA_CON;
    DMA_CON = DMA_CON;
}

void uart_set_timeout_value(UART_REGISTER_T *uartx, uint32_t value)
{
    uartx->RX_TO_CON_UNION.RX_TO_CON_CELLS.RX_TO_MODE = 0x1;

    uartx->RX_TOC_DEST = value;
}

void uart_reset_default_value(UART_REGISTER_T *uartx)
{
    uartx->LCR_UNION.LCR = 0x00;
    uartx->XON_XOFF_UNION.XON_XOFF = 0x00;
    uartx->DLM_DLL = 0x00;
    uartx->IER_UNION.IER = 0x00;
    uartx->FCR_UNION.FCR = 0x00;
    uartx->EFR_UNION.EFR = 0x00;
    uartx->MCR_UNION.MCR = 0x00;
    //uartx->MCR_UNION.MCR_CELLS.RTS = 0x00;
    //uartx->MCR_UNION.MCR_CELLS.LOOP = 0x00;
    uartx->SCR = 0x00;
    //uartx->AUTOBAUD_REG_UNION.AUTOBAUD_REG = 0x00;
    uartx->HIGHSPEED = 0x00;
    uartx->SAMPLE_REG_UNION.SAMPLE_REG = 0x00;
    uartx->RATEFIX_UNION.RATEFIX = 0x00;
    uartx->AUTOBAUD_CON_UNION.AUTOBAUD_CON = 0x00;
    uartx->GUARD = 0x00;
    uartx->ESCAPE_REG_UNION.ESCAPE_REG = 0x00;
    uartx->SLEEP_REG = 0x00;
    uartx->DMA_CON_UNION.DMA_CON = 0x00;
    uartx->RXTRIG = 0x00;
    uartx->FRACDIV = 0x00;
    uartx->RX_TOC_DEST = 0x00;
    uartx->RX_TO_CON_UNION.RX_TO_CON = 0x00;
}

void uart_query_empty(UART_REGISTER_T *uartx)
{
    while (!((uartx->LSR) & UART_LSR_TEMT_MASK));
}


#ifdef __cplusplus
}
#endif

#endif

