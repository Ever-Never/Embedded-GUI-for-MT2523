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

#ifndef __BL_UART_HW_H__
#define __BL_UART_HW_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BL_LOG_VIA_UART0
#define   BL_UART0_BASE    0xA00D0000
#else
#define   BL_UART0_BASE    0xa00f0000
#endif

#define   BL_UART0_RBR     (BL_UART0_BASE+0x0)    /* Read only */
#define   BL_UART0_THR     (BL_UART0_BASE+0x0)    /* Write only */
#define   BL_UART0_DLL     (BL_UART0_BASE+0x0)
#define   BL_UART0_IER     (BL_UART0_BASE+0x4)
#define   BL_UART0_DLH     (BL_UART0_BASE+0x4)
#define   BL_UART0_IIR     (BL_UART0_BASE+0x8)    /* Read only */
#define   BL_UART0_FCR     (BL_UART0_BASE+0x8)    /* Write only */
#define   BL_UART0_EFR     (BL_UART0_BASE+0x8)    /* Only when LCR = 0xbf */
#define   BL_UART0_LCR     (BL_UART0_BASE+0xc)
#define   BL_UART0_MCR     (BL_UART0_BASE+0x10)
#define   BL_UART0_LSR     (BL_UART0_BASE+0x14)
#define   BL_UART0_SCR     (BL_UART0_BASE+0x1c)
#define   BL_UART0_SPEED   (BL_UART0_BASE+0x24)
#define   BL_UART0_SAMPLE_COUNT   (BL_UART0_BASE+0x28)
#define   BL_UART0_SAMPLE_POINT   (BL_UART0_BASE+0x2c)
#define   BL_UART0_FRACDIV_L      (BL_UART0_BASE+0x54)
#define   BL_UART0_FRACDIV_M      (BL_UART0_BASE+0x58)

/* IER */
#define   BL_UART_IER_ERBFI            0x0001
#define   BL_UART_IER_ETBEI            0x0002
#define   BL_UART_IER_ELSI             0x0004
#define   BL_UART_IER_EDSSI            0x0008
#define   BL_UART_IER_XOFFI            0x0020
#define   BL_UART_IER_RTSI             0x0040
#define   BL_UART_IER_CTSI             0x0080
#define   BL_IER_HW_NORMALINTS         0x000d
#define   BL_IER_HW_ALLINTS            0x000f
#define   BL_IER_SW_NORMALINTS         0x002d
#define   BL_IER_SW_ALLINTS            0x002f
#define   BL_UART_IER_ALLOFF           0x0000


/* FCR */
#define   BL_UART_FCR_FIFOEN            0x0001
#define   BL_UART_FCR_CLRR              0x0002
#define   BL_UART_FCR_CLRT              0x0004
#define   BL_UART_FCR_FIFOINI           0x0007
#define   BL_UART_FCR_RX_1BYTE_LEVEL    0x0000
#define   BL_UART_FCR_RX_6BYTE_LEVEL    0x0040
#define   BL_UART_FCR_RX_12BYTE_LEVEL   0x0080
#define   BL_UART_FCR_TX_1BYTE_LEVEL    0x0000
#define   BL_UART_FCR_TX_4BYTE_LEVEL    0x0010
#define   BL_UART_FCR_TX_8BYTE_LEVEL    0x0020
#define   BL_UART_FCR_TX_14BYTE_LEVEL   0x0030
#define   BL_UART_FCR_NORMAL           (BL_UART_FCR_TX_4BYTE_LEVEL|BL_UART_FCR_RX_12BYTE_LEVEL|BL_UART_FCR_FIFOINI)

#if 0
/* IIR */
#define   BL_UART_IIR_INT_INVALID      0x0001
#define   BL_UART_IIR_RLS              0x0006  /* Receiver Line Status */
#define   BL_UART_IIR_RDA              0x0004  /* Receive Data Available */
#define   BL_UART_IIR_CTI              0x000C  /* Character Timeout Indicator */
#define   BL_UART_IIR_THRE             0x0002  /* Transmit Holding Register Empty */
#define   BL_UART_IIR_MS               0x0000  /* Check Modem Status Register */
#define   BL_UART_IIR_SW_FLOW_CTRL     0x0010  /* Receive XOFF characters */
#define   BL_UART_IIR_HW_FLOW_CTRL     0x0020  /* CTS or RTS Rising Edge */
#define   BL_UART_IIR_FIFOS_ENABLED    0x00c0
#define   BL_UART_IIR_NO_INTERRUPT_PENDING   0x0001
#define   BL_UART_IIR_INT_MASK         0x003f
#endif

/* LCR */
#define   BL_UART_WLS_8                0x0003
#define   BL_UART_WLS_7                0x0002
#define   BL_UART_WLS_6                0x0001
#define   BL_UART_WLS_5                0x0000
#define   BL_UART_DATA_MASK            0x0003
#define   BL_UART_NONE_PARITY          0x0000
#define   BL_UART_ODD_PARITY           0x0008
#define   BL_UART_EVEN_PARITY          0x0018
#define   BL_UART_MARK_PARITY          0x0028
#define   BL_UART_SPACE_PARITY         0x0038
#define   BL_UART_PARITY_MASK          0x0038
#define   BL_UART_1_STOP               0x0000
#define   BL_UART_1_5_STOP             0x0004  /* Only valid for 5 data bits */
#define   BL_UART_2_STOP               0x0004
#define   BL_UART_STOP_MASK            0x0004
#define   BL_UART_LCR_DLAB             0x0080

/* MCR */
#define   BL_UART_MCR_RTS              0x0002
#define   BL_UART_MCR_LOOPB            0x0010
#define   BL_UART_MCR_XOFF             0x0080


/* LSR */
#define   BL_UART_LSR_DR               0x0001
#define   BL_UART_LSR_OE               0x0002
#define   BL_UART_LSR_PE               0x0004
#define   BL_UART_LSR_FE               0x0008
#define   BL_UART_LSR_BI               0x0010
#define   BL_UART_LSR_THRE             0x0020
#define   BL_UART_LSR_TEMT             0x0040
#define   BL_UART_LSR_FIFOERR          0x0080

#if 0
/* EFR */
#define   BL_UART_EFR_AUTO_CTS         0x0080
#define   BL_UART_EFR_AUTO_RTS         0x0040
#define   BL_UART_EFR_ENCHANCE         0x0010
#define   BL_UART_EFR_SW_CTRL_MASK     0x000f
#define   BL_UART_EFR_NO_SW_FLOW_CTRL  0x0000
#define   BL_UART_EFR_ALL_OFF          0x0000
#define   BL_UART_EFR_AUTO_RTS_CTS     0x00c0
#define   BL_UART_EFR_TX_SW_FLOW_CTRL  0x0008
#define   BL_UART_EFR_RX_SW_FLOW_CTRL  0x0002
#endif

/* HIGHSPEED */
#define   BL_UART_SPEED_DIV_16         0x0000
#define   BL_UART_SPEED_DIV_8          0x0001
#define   BL_UART_SPEED_DIV_4          0x0002
#define   BL_UART_SPEED_DIV_CUSTOM     0x0003


#ifdef __cplusplus
}
#endif

#endif /* __BL_UART_HW_H__ */
