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

/*******************************************************************************
 *
 * Filename:
 * ---------
 *       uart.c
 *
 * Project:
 * --------
 *    MTK6208 DOWNLOAD AGENT
 *
 * Description:
 * ------------
 *    This Module defines the uart driver.
 *
 * Author:
 * -------
 *       Jensen Hu
 *
 *******************************************************************************/
//#include "UART_HW.H"
//#include "UART.H"
//#include "BLOADER.H"
//#include "hw_config.h"
#include "gnss_uart.h"
#include "bl_common.h"
#if defined(BB_MT6291)
#include "da_uart_drv.h"
#include "usbdl_u3mac_reg.h"
#endif

//extern uint32 err_threshold;
static uint32 err_threshold;

static const uint16 baudrate[] = {
    1,        // UART_BAUD_921600
    2,        // UART_BAUD_460800
    4,        // UART_BAUD_230400
    7,        // UART_BAUD_115200
    14,        // UART_BAUD_57600
    21,        // UART_BAUD_38400
    42,        // UART_BAUD_19200
    85,        // UART_BAUD_9600
    169,    // UART_BAUD_4800
    339,    // UART_BAUD_2400
    677,    // UART_BAUD_1200
    2708,    // UART_BAUD_300
    7386    // UART_BAUD_110
};


void GNSS_UART_Delay(void)
{
    volatile uint32 index_loop;
    volatile uint32 index;

    for (index_loop = 0; index_loop < (20 * 2 / 4); index_loop++) {
        for (index = 0; index < 10000; index++) {
            ;
        }
    }
}

void GNSS_Delay_ms(uint16 ms)
{
    uint32 pre_tick = *GPT_TIMER4_COUNT;
    uint32 wait_ticks = ms * 13000; //  MT2513 real chip: 13MHz, 1ms = 13000 ticks

    volatile uint32 now_tick = *GPT_TIMER4_COUNT;
    while (1) {
        now_tick = *GPT_TIMER4_COUNT;
        if ((now_tick - pre_tick) > wait_ticks) {
            break;
        }
    }
}

void GNSS_UART_Init(void)    //this is called , if no bootrom issued!!
{
    uint16 tmp;

    /* use UART3 and pinmux to GPIO2 and GPIO3 */
    *(volatile int *)(0xA2020C00) &= 0xFFFF00FF;
    *(volatile int *)(0xA2020C00) |= 0x00003300;

    /* enable UART3's clock */
    *(volatile int *)(0xA2010324) |= 0x00010000;

    //Setup N81,(UART_WLS_8 | UART_NONE_PARITY | UART_1_STOP) = 0x03
    UART_WriteReg(UART3_LCR, 0x0003);
    //Set BaudRate = 115200;
    tmp = UART_ReadReg(UART3_LCR);        /* DLAB start */
    UART_WriteReg(UART3_LCR, (tmp | UART_LCR_DLAB));

#ifdef PLUTO_DA_INIT_MPLL_UART
    /* 115200  UART_WriteReg(UART1_DLL,0x0007);  */
    UART_WriteReg(UART3_DLL, 0x0007 * 4);
#else
    /* 19200 */
    //UART_WriteReg(UART3_DLL,0x002a);
    /* 115200/26M */
    UART_WriteReg(UART3_DLL, 0x0007 * 2);
#endif
    UART_WriteReg(UART3_DLH, 0x0000);
    UART_WriteReg(UART3_LCR, tmp);        /* DLAB End */
    //Set Baudrate
    UART_WriteReg(UART3_FCR, 0x0047);
    UART_WriteReg(UART3_MCR, (uint16)0x0003);

    UART_WriteReg(UART3_FCR, UART_FCR_Normal);
}

uint8 GNSS_UART_GetByte(void)
{
    uint8    g_uart_last_rx_data;
    uint16    g_uart_last_lsr;
    //uint8    rest_data[32];
    //uint32    rest_count;

    while (1) {
        g_uart_last_lsr = UART_ReadReg(UART3_LSR);

        if ( g_uart_last_lsr & UART_LSR_DR )    {
            g_uart_last_rx_data = (uint8)UART_ReadReg(UART3_RBR);
            return g_uart_last_rx_data;
        }
    }
}

bool GNSS_UART_GetByteStatus(uint8 *data)
{
    uint16 LSR;
    uint32 err_count;

    err_count = 0;
    while (1) {
        LSR = UART_ReadReg(UART3_LSR);

        if ((LSR & UART_LSR_FIFOERR) ||
                (LSR & UART_LSR_OE) ||
                (LSR & UART_LSR_PE) ) {
            err_count = 0;
        }

        if (LSR & UART_LSR_DR) {
            *data = (uint8)UART_ReadReg(UART3_RBR);
            return TRUE;
        } else {
            err_count++;
            if (err_count > err_threshold) {
                return FALSE;
            }
        }
    }

}

bool GNSS_UART_GetByte_NO_TIMEOUT(uint8 *data)
{
    uint16 LSR;

    LSR = UART_ReadReg(UART3_LSR);

    if (LSR & UART_LSR_DR) {
        *data = (uint8)UART_ReadReg(UART3_RBR);
        return TRUE;
    } else {
        return FALSE;
    }
}

bool GNSS_UART_GetByte_Buffer(uint32 *buf, uint32 length)
{
    bool ret;
    uint32 i;
    uint8 *buf8 = (uint8 *)buf;

    for (i = 0; i < length; i++) {
        ret = GNSS_UART_GetByteStatus(buf8 + i);
        if (!ret) {
            return FALSE;
        }
    }

    return TRUE;
}

void GNSS_UART_PutByte(uint8 data)
{
    uint16 LSR;

    while (1) {
        LSR = UART_ReadReg(UART3_LSR);
        if ( LSR & UART_LSR_THRE ) {
            UART_WriteReg(UART3_THR, (uint16)data);
            break;
        }
    }
}

void GNSS_UART_PutByte_Buffer(uint32 *buf, uint32 length)
{
    uint32 i;
    uint8 *tmp_buf = (uint8 *)buf;

#ifdef    HIGHSPEED_UART_PUT_DELAY
    uint16         delay;
    uint16         delay_max;

    extern UART_BAUDRATE        g_baudrate;
    // calculate the max delay by external clock.
    delay_max = 350 * g_HW_DevCfg.m_clk_cfg.m_ext_clock;
#endif

    for (i = 0; i < length; i++) {
#ifdef    HIGHSPEED_UART_PUT_DELAY
        // in order to prevent PC lost data at high speed baudrate, every 14 bytes must have a delay.
        // 14 is the max size of standard RX FIFO on Windows.
        if ( UART_BAUD_230400 > g_baudrate && 0 == i % 14 ) {
            for (delay = 0; delay < delay_max; delay++);
        }
#endif

        GNSS_UART_PutByte(*(tmp_buf + i));
    }
}

void GNSS_UART_PutByte_Complete(uint8 data)
{
    GNSS_UART_PutByte(data);
}

uint16 GNSS_UART_GetData16(void)            //ok, high byte is first
{
    uint8 tmp, index;
    uint16 tmp16;
    uint16 result = 0;
    for (index = 0; index < 2; index++) {
        tmp = GNSS_UART_GetByte();
        tmp16 = (uint16)tmp;
        result |= (tmp16 << (8 - 8 * index));
    }
    return result;
}

void GNSS_UART_PutData16(uint16 data)        //ok, high byte is first
{
    uint8 tmp, index;
    uint16 tmp16;

    for (index = 0; index < 2; index++) {
        tmp16 = (data >> (8 - 8 * index));
        tmp = (uint8)tmp16;
        GNSS_UART_PutByte(tmp);
    }
}

uint32 GNSS_UART_GetData32(void)            //ok, high byte is first
{
    uint8 tmp, index;
    uint32 tmp32;
    uint32 result = 0;
    for (index = 0; index < 4; index++) {
        tmp = GNSS_UART_GetByte();
        tmp32 = (uint32)tmp;
        result |= (tmp32 << (24 - 8 * index));
    }
    return result;
}

void GNSS_UART_PutData32(uint32 data)        //ok, high byte is first
{
    uint8 tmp, index;
    uint32 tmp32;

    for (index = 0; index < 4; index++) {
        tmp32 = (data >> (24 - 8 * index));
        tmp = (uint8)tmp32;
        GNSS_UART_PutByte(tmp);
    }
}

void GNSS_UART_PurgeFIFO(void)
{
    GNSS_UART_Delay();
    //#if defined(BB_MT6261) || defined(BB_MT2523)

    UART_WriteReg(UART3_FCR, 0x0 | UART_FCR_RX62Byte_Level | UART_FCR_TX62Byte_Level );
    UART_WriteReg(UART3_FCR, 0x6 | UART_FCR_RX62Byte_Level | UART_FCR_TX62Byte_Level );
    UART_WriteReg(UART3_FCR, 0x1 | UART_FCR_RX62Byte_Level | UART_FCR_TX62Byte_Level );

    //#else
    //UART_WriteReg(UART3_FCR, UART_FCR_MaxFIFO);
    //#endif
    GNSS_UART_Delay();
}

void GNSS_UART_SetBaudRate(uint8 baud_rate)
{
#ifdef PLUTO_DA_INIT_MPLL_UART
    // if MT6208 do not change baudrate, just increase timeout threshold
    err_threshold = 0x3FFFFC;
    return;
#else
    uint16 tmp;
    uint16 LCR;
    uint16 divisor;
    //uint16 rate_step;
    //uint16 sample_count;
    //uint16 sample_point;
    int32 ext_clock;

    /* kailing supply API */
    ext_clock = 2;

    // wait for sent data flush out
    GNSS_UART_Delay();

    //normal speed
    // multiple by external clock

    divisor = baudrate[baud_rate - 1] * ext_clock;

    LCR = UART_ReadReg(UART3_LCR);        /* DLAB start */
    UART_WriteReg(UART3_LCR, (LCR | UART_LCR_DLAB));

    tmp = (divisor & 0x00ff);
    UART_WriteReg(UART3_DLL, tmp);
    tmp = (divisor & 0xff00);
    tmp >>= 8;
    UART_WriteReg(UART3_DLH, tmp);
    UART_WriteReg(UART3_LCR, LCR);        /* DLAB End */

    err_threshold = 0xFFFFF;

    // setup TX/RX to max FIFO and purge FIFO
    UART_WriteReg(UART3_FCR, UART_FCR_MaxFIFO);

    // wait for baudrate change
    GNSS_UART_Delay();
#endif

}
