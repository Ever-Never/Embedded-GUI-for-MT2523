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
 * Filename:
 * ---------
 *      download.h
 *
 * Project:
 * --------
 *   MTK3301 DOWNLOAD AGENT
 *
 * Description:
 * ------------
 *   This file is intends for download agent specific definition
 *
 *******************************************************************************/
#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

/* DA Version */
#define DA_MAJOR_VER    0x04
#define DA_MINOR_VER    0x00

/* Max Number of Load Sections */
#define MAX_LOAD_SECTIONS     10

// Max Die Number of MCP 
#define MAX_DIE_IN_MCP        2

/* RETURN VALUE */
#define HW_ERROR              0x1c
#define HW_RAM_OK             0xE0
#define HW_RAM_FLOARTING      0xE1
#define HW_RAM_UNACCESSABLE   0xE2
#define HW_RAM_ERROR          0xE3
#define SOC_FAIL              0x0c
#define SYNC_CHAR             0xc0
#define CONT_CHAR             0x69
#define STOP_CHAR             0x96
#define ACK                   0x5a
#define NACK                  0xa5
#define UNKNOWN_CMD           0xbb

/* FLASH OPERATION STATUS */
typedef enum {
    S_DONE = 0
   ,S_PGM_FAILED
   ,S_ERASE_FAILED
   ,S_TIMEOUT
   ,S_IN_PROGRESS
   ,S_CMD_ERR
   ,S_BLOCK_LOCKED_ERR
   ,S_BLOCK_UNSTABLE
   ,S_VPP_RANGE_ERR
   ,S_ERASE_ADDR_ERR
   ,S_ERASE_RANGE_ERR
   ,S_PGM_AT_ODD_ADDR
   ,S_PGM_WITH_ODD_LENGTH
   ,S_BUFPGM_NO_SUPPORT
   ,S_UNKNOWN_ERR
} STATUS_E;

/* COMMANDS */
#define DA_EXT_CLOCK_CMD      0xD0
#define DA_BBCHIP_TYPE_CMD      0xD1
#define DA_SPEED_CMD         0xD2
#define DA_MEM_CMD            0xD3
#define DA_FORMAT_CMD         0xD4
#define DA_WRITE_CMD         0xD5
#define DA_READ_CMD            0xD6
#define DA_WRITE_REG16_CMD      0xD7
#define DA_READ_REG16_CMD      0xD8
#define DA_FINISH_CMD         0xD9
#define DA_GET_DSP_VER_CMD      0xDA
#define DA_ENABLE_WATCHDOG_CMD   0xDB

/* SPEED_PARA */
typedef enum {
   UART_BAUD_921600  = 0x01,
   UART_BAUD_460800  = 0x02,
   UART_BAUD_230400  = 0x03,
   UART_BAUD_115200  = 0x04,
   UART_BAUD_57600   = 0x05,
   UART_BAUD_38400   = 0x06,
   UART_BAUD_19200   = 0x07,
   UART_BAUD_14400   = 0x08,
   UART_BAUD_9600    = 0x09,
   UART_BAUD_4800    = 0x0a,
   UART_BAUD_2400    = 0x0b,
   UART_BAUD_1200    = 0x0c,
   UART_BAUD_300     = 0x0d,
   UART_BAUD_110     = 0x0e,
   UART_BAUD_AUTO    = 0x0f,	
   
}UART_BAUDRATE;

typedef enum
{
   TIMEOUT_DATA = 0,
   CKSUM_ERROR,
   RX_BUFFER_FULL,
   TIMEOUT_CKSUM_LSB,
   TIMEOUT_CKSUM_MSB,
   ERASE_TIMEOUT,
   PROGRAM_TIMEOUT,
   RECOVERY_BUFFER_FULL,
   UNKNOWN_ERROR
}eRX_error;

/* DEVICE_INFO */
typedef enum
{
   DEVICE_TV0057A003AABD,
   DEVICE_LAST = DEVICE_TV0057A003AABD,
   DEVICE_UNKNOWN = 0xFF         // Unknown Device 
}DEVICE_INFO;

#endif
