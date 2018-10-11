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

#include <stdint.h>
#include "bl_uart_hw.h"

#define BL_UART_BAUDRATE 115200

/*
 * uart write data to TBR
 */
static void bl_uart_write_reg(uint32_t addr, uint16_t data)
{
    *(volatile uint8_t *) addr = (uint8_t)data;
}

/*
 * uart read data from RHR
 */
static uint16_t bl_uart_read_reg(uint32_t addr)
{
    volatile uint8_t tmp;
    tmp = *(volatile uint8_t *) addr;
    return (uint16_t)tmp;
}

/*
 * low-level uart api used by bl_print_internal()
 */
void bl_uart_put_byte(uint8_t data)
{
    volatile uint16_t lsr;

    while (1) {
        lsr = bl_uart_read_reg(BL_UART0_LSR);
        if ( lsr & BL_UART_LSR_THRE ) {
            bl_uart_write_reg(BL_UART0_THR, (uint16_t)data);
            break;
        }
    }

}

/*
 * wait until all data sent out
 */
void bl_uart_check_send_end(void)
{
    uint16_t lsr;
    uint16_t timeout = 0xffff;

    while (timeout--) {
        lsr = bl_uart_read_reg(BL_UART0_LSR);
        if ( lsr & BL_UART_LSR_TEMT ) {
            break;
        }
    }
}

void bl_uart_init(uint32_t uart_clk)
{
    uint32_t integer, remainder, fraction;
    uint32_t dll_dlm, sample_count, sample_point, temp_lcr;
    uint32_t fraction_L_mapping[] = {0x00, 0x00, 0x20, 0x90, 0xa8, 0x54, 0x6c, 0xba, 0xf6, 0xfe};
    uint32_t fraction_M_mapping[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01};

    /* UART2 GPIO pinmux setting */
    *(volatile uint32_t *)0xA2020C08 = 0x000000ff;
    *(volatile uint32_t *)0xA2020C04 = 0x00000033;

    /* UART2 PDN turn on */
    *(volatile uint32_t *)0xA2010324 |= 0x00008000;

    integer = uart_clk / (BL_UART_BAUDRATE * 256);
    remainder = ((uart_clk * 10) / (BL_UART_BAUDRATE * 256)) % 10;
    if ((remainder != 0) || (integer == 0)) {
        integer += 1;
    }
    dll_dlm = integer;
    sample_count = uart_clk / (BL_UART_BAUDRATE * dll_dlm);

    while (sample_count > 256) {
        dll_dlm++;
        sample_count = uart_clk / (BL_UART_BAUDRATE * dll_dlm);
    }

    fraction = ((uart_clk * 10) / (BL_UART_BAUDRATE * dll_dlm)) % 10;

    sample_count -= 1;
    sample_point = (sample_count - 1) >> 1;

    bl_uart_write_reg(BL_UART0_SPEED, BL_UART_SPEED_DIV_CUSTOM);

    temp_lcr = bl_uart_read_reg(BL_UART0_LCR);
    bl_uart_write_reg(BL_UART0_LCR, temp_lcr | BL_UART_LCR_DLAB);
    bl_uart_write_reg(BL_UART0_DLL, dll_dlm & 0x00ff);
    bl_uart_write_reg(BL_UART0_DLH, (dll_dlm >> 8) & 0x00ff);
    bl_uart_write_reg(BL_UART0_LCR, temp_lcr);

    bl_uart_write_reg(BL_UART0_SAMPLE_COUNT, sample_count);
    bl_uart_write_reg(BL_UART0_SAMPLE_POINT, sample_point);

    bl_uart_write_reg(BL_UART0_FRACDIV_L, fraction_L_mapping[fraction]);
    bl_uart_write_reg(BL_UART0_FRACDIV_M, fraction_M_mapping[fraction]);

    bl_uart_write_reg(BL_UART0_LCR, BL_UART_WLS_8 | BL_UART_NONE_PARITY | BL_UART_1_STOP);
    bl_uart_write_reg(BL_UART0_FCR, BL_UART_FCR_NORMAL);
    bl_uart_write_reg(BL_UART0_IER, BL_UART_IER_ALLOFF);
}

