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

#include "uart.h"
#include "uart_hw.h"
#include "mem_util.h"
#include "timer.h"

#include "nvic.h"

//#include "printf.h"
#include "top.h"

#include "type_def.h"
#include "dma_hw.h"
#include "dma_sw.h"

#define    UART_BAUDRATE        115200        /* default baudrate */

#define UART_BASE_ADDR(port_no) \
    ((port_no == UART_PORT0) ? CM4_UART1_BASE : CM4_UART2_BASE)

uint32_t gUartClkFreq;

#ifdef HAL_SLEEP_MANAGER_ENABLED
const char *const g_uart_sleep_handler_name[] = {"uart0", "uart1"};
const hal_nvic_irq_t g_uart_port_to_irq_num[] = {CM4_UART1_IRQ, CM4_UART2_IRQ};
#endif

/*****************************/
/* Internal used function */
/*****************************/
/*
 *     get character from SW FIFO (interrupt based)
 *     (ROM code shall call getc_nowait() instead of this function)
 */
int getc(void)
{
    //int rc = rb_get_and_zap();
    int rc = getc_nowait();
    return rc;
}

/*****************************/

/*
 *    get character from HW directly
 */
int getc_nowait(void)
{
    char c;

    if (HAL_REG_32(CONSOLE + UART_LSR) & 0x1) { //  DataReady bit
        c = HAL_REG_32(CONSOLE + UART_RBR);
        return c;
    } else {
        return -1;
    }
}

/* MT7687 CM4 */
void uart_output_char(UART_PORT port_no, unsigned char c)
{
    unsigned int base = UART_BASE_ADDR(port_no);

    while (!(HAL_REG_32(base + UART_LSR) & 0x20 /* THRE bit */))
        ;
    HAL_REG_32(base + UART_RBR) = c;

    return;
}


/* MT7687 CM4 */
uint8_t uart_input_char(UART_PORT port_no)
{
    unsigned int base = UART_BASE_ADDR(port_no);
    char         c;

    while (!(HAL_REG_32(base + UART_LSR) & 0x1)) //  DataReady bit
        ;

    c = HAL_REG_32(base + UART_RBR);

    return c;
}

/* MT7687 CM4 */
uint32_t uart_input_char_unblocking(UART_PORT port_no)
{
    unsigned int base = UART_BASE_ADDR(port_no);
    char         c;

    if (HAL_REG_32(base + UART_LSR) & 0x1) { //  DataReady bit
        c = HAL_REG_32(base + UART_RBR);
        return (uint32_t)c;
    } else {
        return 0xffffffff;
    }
}

/*
 * Initial UART hardware
 */
void halUART_HWInit(UART_PORT u_port)
{

    halUART_SetFormat(u_port, UART_BAUDRATE, UART_WLS_8, UART_NONE_PARITY, UART_1_STOP);
    if (u_port == UART_PORT0) {
        //UART_HWInit(CM4_UART1_BASE);    /*temp*/
        HAL_REG_32(CM4_UART1_BASE + UART_VFIFO_EN_REG) |= 0x1;

        /* ISR */
        //NVIC_EnableIRQ(CM4_UART1_IRQ);
        //NVIC_SetPriority(CM4_UART1_IRQ, CM4_UART1_PRI);
    } else if (u_port == UART_PORT1) {
        //UART_HWInit(CM4_UART2_BASE);    /*temp*/
        HAL_REG_32(CM4_UART2_BASE + UART_VFIFO_EN_REG) |= 0x1;
        /* ISR */
        //NVIC_EnableIRQ(CM4_UART2_IRQ);
        //NVIC_SetPriority(CM4_UART2_IRQ, CM4_UART2_PRI);
    } else {
    }
}


void halUART_SetFormat(UART_PORT u_port, uint32_t baudrate, uint16_t databit, uint16_t parity, uint16_t stopbit)
{
    uint16_t control_word;
    uint32_t UART_BASE = CM4_UART1_BASE;
    uint32_t data, uart_lcr, high_speed_div, sample_count, sample_point, fraction;
    uint16_t  fraction_L_mapping[] = {0x00, 0x10, 0x44, 0x92, 0x29, 0xaa, 0xb6, 0xdb, 0xad, 0xff, 0xff};
    uint16_t  fraction_M_mapping[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03};
    int32_t status = 0;
    gUartClkFreq = top_xtal_freq_get();

    if (u_port == UART_PORT0) {
        UART_BASE = CM4_UART1_BASE;
    } else if (u_port == UART_PORT1) {
        UART_BASE = CM4_UART2_BASE;
    } else {
    }

    // 100, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
    HAL_REG_32(UART_BASE + UART_RATE_STEP) = 0x3; // based on sampe_count * baud_pulse, baud_rate = system clock frequency / sampe_count
    uart_lcr = HAL_REG_32(UART_BASE + UART_LCR);     /* DLAB start */
    HAL_REG_32(UART_BASE + UART_LCR) = (uart_lcr | UART_LCR_DLAB);
    data = gUartClkFreq / baudrate;
    high_speed_div = (data >> 8) + 1; // divided by 256
    {
        sample_count = data / high_speed_div - 1;
        if (sample_count == 3) {
            sample_point = 0;
        } else {
            sample_point = sample_count / 2 - 1;
        }
        if (!(data >= 4)) {
            status = -1;
        }
    }
    fraction = (gUartClkFreq * 10 / baudrate / data - (sample_count + 1) * 10) % 10;
    HAL_REG_32(UART_BASE + UART_DLL) = (high_speed_div & 0x00ff);
    HAL_REG_32(UART_BASE + UART_DLH) = ((high_speed_div >> 8) & 0x00ff);
    HAL_REG_32(UART_BASE + UART_STEP_COUNT) = sample_count;
    HAL_REG_32(UART_BASE + UART_SAMPLE_COUNT) = sample_point;
    HAL_REG_32(UART_BASE + UART_FRACDIV_M) = fraction_M_mapping[fraction];
    HAL_REG_32(UART_BASE + UART_FRACDIV_L) = fraction_L_mapping[fraction];
    HAL_REG_32(UART_BASE + UART_LCR) = (uart_lcr);   /* DLAB end */
    HAL_REG_32(UART_BASE + UART_FCR) = 0x47;

    control_word = DRV_Reg16(UART_BASE + UART_LCR);     /* DLAB start */
    control_word &= ~UART_DATA_MASK;
    control_word |= databit;
    control_word &= ~UART_STOP_MASK;
    control_word |= stopbit;
    control_word &= ~UART_PARITY_MASK;
    control_word |= parity;
    DRV_WriteReg16(UART_BASE + UART_LCR , control_word);           /* DLAB End */
    if (status != 0) {
        return;
    }
}

void halUART_Get_VFIFO_Length(UART_PORT u_port, int32_t is_rx, uint32_t *length)
{
    if (u_port == UART_PORT0) {
        if (is_rx) {
            *length = DMA_GetVFIFO_Avail(VDMA_UART1RX_CH);
        } else {
            *length = DMA_GetVFIFO_FFSIZE(VDMA_UART1TX_CH);
            *length -= DMA_GetVFIFO_Avail(VDMA_UART1TX_CH);
        }
    } else {
        if (is_rx) {
            *length = DMA_GetVFIFO_Avail(VDMA_UART2RX_CH);
        } else {
            *length = DMA_GetVFIFO_FFSIZE(VDMA_UART2TX_CH);
            *length -= DMA_GetVFIFO_Avail(VDMA_UART2TX_CH);
        }
    }

    return;
}

void halUART_Read_VFIFO_Data(UART_PORT u_port, uint8_t *data, uint32_t length)
{
    UINT32 idx = 0;
    for (idx = 0; idx < length; idx++) {
        if (u_port == UART_PORT0) {
            data[idx] = (UINT8)(*(volatile UINT8 *)DMA_VPORT(VDMA_UART1RX_CH));
        } else if (u_port == UART_PORT1) {
            data[idx] = (UINT8)(*(volatile UINT8 *)DMA_VPORT(VDMA_UART2RX_CH));
        } else {
        }
    }

    return;
}

void halUART_Write_VFIFO_Data(UART_PORT u_port, const uint8_t *data, uint32_t length)
{
    UINT32 idx = 0;
    for (idx = 0; idx < length; idx++) {
        if (u_port == UART_PORT0) {
            *(volatile UINT8 *)DMA_VPORT(VDMA_UART1TX_CH) = data[idx];
        } else if (u_port == UART_PORT1) {
            *(volatile UINT8 *)DMA_VPORT(VDMA_UART2TX_CH) = data[idx];
        } else {
        }
    }

    return;
}

void halUART_Tx_VFIFO_Initialize(UART_PORT u_port, uint8_t *vfifo_buffer, uint32_t buffer_length, uint32_t threshold)
{
    if (u_port == UART_PORT0) {
        DMA_Vfifo_SetAdrs((UINT32)vfifo_buffer, buffer_length, VDMA_UART1TX_CH, 0 /* alert length */, threshold /* threshold */, 0 /* timeout */);
        DMA_Vfifo_Flush(VDMA_UART1TX_CH);
    } else if (u_port == UART_PORT1) {
        DMA_Vfifo_SetAdrs((UINT32)vfifo_buffer, buffer_length, VDMA_UART2TX_CH, 0 /* alert length */, threshold /* threshold */, 0 /* timeout */);
        DMA_Vfifo_Flush(VDMA_UART2TX_CH);
    } else {
    }
}

void halUART_Rx_VFIFO_Initialize(UART_PORT u_port, uint8_t *vfifo_buffer, uint32_t buffer_length, uint32_t alert_length, uint32_t threshold, uint32_t timeout)
{
    if (u_port == UART_PORT0) {
        DMA_Vfifo_SetAdrs((UINT32)vfifo_buffer, buffer_length, VDMA_UART1RX_CH, alert_length /* alert length */, threshold /* threshold */, timeout /* timeout */);
        DMA_Vfifo_Flush(VDMA_UART1RX_CH);
    } else if (u_port == UART_PORT1) {
        DMA_Vfifo_SetAdrs((UINT32)vfifo_buffer, buffer_length, VDMA_UART2RX_CH, alert_length /* alert length */, threshold /* threshold */, timeout /* timeout */);
        DMA_Vfifo_Flush(VDMA_UART2RX_CH);
    } else {
    }
}

void halUART_VDMA_Rx_Register_Callback(UART_PORT u_port, VOID_FUNC func)
{
    if (u_port == UART_PORT0) {
        DMA_Vfifo_Register_Callback(VDMA_UART1RX_CH, func);
    } else if (u_port == UART_PORT1) {
        DMA_Vfifo_Register_Callback(VDMA_UART2RX_CH, func);
    } else {
    }
}

void halUART_VDMA_Rx_Register_TO_Callback(UART_PORT u_port, VOID_FUNC func)
{
    if (u_port == UART_PORT0) {
        DMA_Vfifo_Register_TO_Callback(VDMA_UART1RX_CH, func);
    } else if (u_port == UART_PORT1) {
        DMA_Vfifo_Register_TO_Callback(VDMA_UART2RX_CH, func);
    } else {
    }
}

void uart_set_hardware_flowcontrol(UART_PORT u_port)
{
    unsigned int base = UART_BASE_ADDR(u_port);
    uint16_t EFR, LCR;

    LCR = HAL_REG_32(base + UART_LCR);

    HAL_REG_32(base + UART_LCR) = 0xbf;
    EFR = HAL_REG_32(base + UART_EFR);
    EFR |= 0x00d0;
    HAL_REG_32(base + UART_EFR) = EFR;

    HAL_REG_32(base + UART_ESCAPE_EN) = 0;

    HAL_REG_32(base + UART_LCR) = 0x00;
    HAL_REG_32(base + UART_MCR) = 0x02;

    HAL_REG_32(base + UART_LCR) = LCR;
}

void uart_set_software_flowcontrol(UART_PORT u_port, uint8_t xon, uint8_t xoff, uint8_t escape_character)
{
    unsigned int base = UART_BASE_ADDR(u_port);
    uint16_t EFR, LCR;

    LCR = HAL_REG_32(base + UART_LCR);

    HAL_REG_32(base + UART_LCR) = 0xbf;
    HAL_REG_32(base + UART_XON1) = xon;
    HAL_REG_32(base + UART_XON2) = xon;
    HAL_REG_32(base + UART_XOFF1) = xoff;
    HAL_REG_32(base + UART_XOFF2) = xoff;

    EFR = HAL_REG_32(base + UART_EFR);
    EFR |= 0x000A;
    HAL_REG_32(base + UART_EFR) = EFR;

    HAL_REG_32(base + UART_LCR) = LCR;

    HAL_REG_32(base + UART_ESCAPE_DATA) = escape_character;
    HAL_REG_32(base + UART_ESCAPE_EN) = 1;
}

void uart_disable_flowcontrol(UART_PORT u_port)
{
    unsigned int base = UART_BASE_ADDR(u_port);
    uint16_t LCR;

    LCR = HAL_REG_32(base + UART_LCR);

    HAL_REG_32(base + UART_LCR) = 0xbf;

    HAL_REG_32(base + UART_EFR) = 0x0;

    HAL_REG_32(base + UART_LCR) = 0x00;

    HAL_REG_32(base + UART_LCR) = LCR;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void uart_set_sleep_mode(UART_PORT u_port)
{
    unsigned int base = UART_BASE_ADDR(u_port);

    HAL_REG_32(base + UART_SLEEP_EN) = 0x01;
}

void uart_unmask_send_interrupt(UART_PORT u_port)
{
    unsigned int base = UART_BASE_ADDR(u_port);
    uint16_t IER, LCR;

    LCR = HAL_REG_32(base + UART_LCR);
    HAL_REG_32(base + UART_LCR) = 0x00;

    IER = HAL_REG_32(base + UART_IER);
    IER |= 0x02;
    HAL_REG_32(base + UART_IER) = IER;

    HAL_REG_32(base + UART_LCR) = LCR;

    HAL_REG_32(base + UART_SLEEP_EN) = 0x01;
}

void uart_mask_send_interrupt(UART_PORT u_port)
{
    unsigned int base = UART_BASE_ADDR(u_port);
    uint16_t IER, LCR;

    LCR = HAL_REG_32(base + UART_LCR);
    HAL_REG_32(base + UART_LCR) = 0x00;

    IER = HAL_REG_32(base + UART_IER);
    IER &= ~0x02;
    HAL_REG_32(base + UART_IER) = IER;

    HAL_REG_32(base + UART_LCR) = LCR;

    HAL_REG_32(base + UART_SLEEP_EN) = 0x00;
}

void uart_interrupt_handler(hal_nvic_irq_t irq_number)
{
    UART_PORT u_port;
    unsigned int base;
    uint16_t IIR;

    if (irq_number == CM4_UART1_IRQ) {
        u_port = UART_PORT0;
        base = UART_BASE_ADDR(u_port);
    } else {
        u_port = UART_PORT1;
        base = UART_BASE_ADDR(u_port);
    }

    IIR = HAL_REG_32(base + UART_IIR);
    switch (IIR & 0x3F) {
        case 0x02:
            uart_send_handler(u_port);
            break;
        default:
            break;
    }
}
#endif

void uart_reset_default_value(UART_PORT u_port)
{
    unsigned int base = UART_BASE_ADDR(u_port);

    HAL_REG_32(base + UART_LCR) = 0xbf;
    HAL_REG_32(base + UART_EFR) = 0x10;
    HAL_REG_32(base + UART_XON1) = 0x00;
    HAL_REG_32(base + UART_XOFF1) = 0x00;

    HAL_REG_32(base + UART_LCR) = 0x80;
    HAL_REG_32(base + UART_DLL) = 0x00;
    HAL_REG_32(base + UART_DLH) = 0x00;

    HAL_REG_32(base + UART_LCR) = 0x00;
    HAL_REG_32(base + UART_IER) = 0x00;
    HAL_REG_32(base + UART_FCR) = 0x00;

    HAL_REG_32(base + UART_LCR) = 0xbf;
    HAL_REG_32(base + UART_EFR) = 0x00;
    HAL_REG_32(base + UART_LCR) = 0x00;

    HAL_REG_32(base + UART_MCR) = 0x00;
    HAL_REG_32(base + UART_SCR) = 0x00;
    HAL_REG_32(base + UART_RATE_STEP) = 0x00;
    HAL_REG_32(base + UART_STEP_COUNT) = 0x00;
    HAL_REG_32(base + UART_SAMPLE_COUNT) = 0x00;
    HAL_REG_32(base + UART_RATE_FIX_REG) = 0x00;
    HAL_REG_32(base + UART_GUARD) = 0x00;
    HAL_REG_32(base + UART_ESCAPE_DATA) = 0x00;
    HAL_REG_32(base + UART_ESCAPE_EN) = 0x00;
    HAL_REG_32(base + UART_SLEEP_EN) = 0x00;
    HAL_REG_32(base + UART_VFIFO_EN_REG) = 0x00;
    HAL_REG_32(base + UART_RX_TRIGGER_ADDR) = 0x00;
    HAL_REG_32(base + UART_FRACDIV_L) = 0x00;
    HAL_REG_32(base + UART_FRACDIV_M) = 0x00;
    HAL_REG_32(base + UART_TX_ACTIVE_EN) = 0x00;
}

void uart_query_empty(UART_PORT u_port)
{
    unsigned int base = UART_BASE_ADDR(u_port);

    while (!(HAL_REG_32(base + UART_LSR) & 0x40));
}

