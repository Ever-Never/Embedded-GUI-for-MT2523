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
 *  brom_mt3301.cpp
 *
 * Project:
 * --------
 *  BootRom Library
 *
 * Description:
 * ------------
 *  BootRom class for MT3301
 *
 *******************************************************************************/
#include <stdio.h>
#include "flashtool.h"
#include "gnss_uart.h"
#include "brom_base.h"
#include "bl_common.h"



// MT3301 BootRom Start Command
#define MT3301_BOOT_ROM_START_CMD1      0xA0
#define MT3301_BOOT_ROM_START_CMD2      0x0A
#define MT3301_BOOT_ROM_START_CMD3      0x50
#define MT3301_BOOT_ROM_START_CMD4      0x05

#define NMEA_START_CMD1         '$'
#define NMEA_START_CMD2         'P'
#define NMEA_START_CMD3         'M'
#define NMEA_START_CMD4         'T'
#define NMEA_START_CMD5         'K'
#define NMEA_START_CMD6         '1'
#define NMEA_START_CMD7         '8'
#define NMEA_START_CMD8         '0'
#define NMEA_START_CMD9         '*'
#define NMEA_START_CMD10        '3'
#define NMEA_START_CMD11        'B'
#define NMEA_START_CMD12      0x0D
#define NMEA_START_CMD13      0x0A

#define BAUD_CHECK_UPPER_BOUND   7


static const unsigned char MT3301_BOOT_ROM_START_CMD[] = {
    MT3301_BOOT_ROM_START_CMD1,
    MT3301_BOOT_ROM_START_CMD2,
    MT3301_BOOT_ROM_START_CMD3,
    MT3301_BOOT_ROM_START_CMD4
};

static unsigned char NMEA_START_CMD[] = {
    NMEA_START_CMD1,
    NMEA_START_CMD2,
    NMEA_START_CMD3,
    NMEA_START_CMD4,
    NMEA_START_CMD5,
    NMEA_START_CMD6,
    NMEA_START_CMD7,
    NMEA_START_CMD8,
    NMEA_START_CMD9,
    NMEA_START_CMD10,
    NMEA_START_CMD11,
    NMEA_START_CMD12,
    NMEA_START_CMD13
};

//Baud support list by COM API , please check MSDN
//currently we support 115200 , 57600, 38400, 19200, 14400, 9600, 4800
// use for special baud rate

// MT3301 BootRom Base BaudRate
#define MT3301_BOOT_ROM_BASE_BAUDRATE   115200

// MT3301 Register Address
#define MT3301_REG_EMI_CON0         0x80010000   // config Bank0 memory 
#define MT3301_REG_EMI_CON1         0x80010004   // config Bank1 memory 
#define MT3301_REG_EMI_CON4         0x80010010   // config memory re-mapping mechanism 
#define MT3301_REG_GPIO_DOUT        0x80120004  // config FT mode 
#define MT3301_REG_RTC_BBPU         0x80210000   // config baseband chip power up 
#define MT3301_REG_RTC_POWERKEY1    0x80210050   // Real-Time-Clock PowerKey 1 
#define MT3301_REG_RTC_POWERKEY2    0x80210054   // Real-Time-Clock PowerKey 2 
#define MT3301_REG_UART_RATE_STEP   0x80130020   // UART highspeed rate step 

// MT3301 Target Start Address
#define MT3301_TARGET_START_ADDR    0x00000000


#define	MAX_SPECIAL_BAUD_RATE	3
#define	DEFAULT_BAUD_RATE		115200

//------------------------------------------------------------------------------
// bootrom start command
static int BRom_StartCmd(GNSS_Download_Arg arg)
{
    //example for com driver
    unsigned char data8;
    //char buf[1024] = {0};
    unsigned char fail_retry = 0;
    unsigned long i;
    //unsigned long Uart_Baud[] = { 115200 , 57600, 38400, 19200, 14400, 9600, 4800};
    //volatile unsigned long image_len = 0;
    //volatile unsigned long send_bytes = 0;
    //volatile unsigned long frame_bytes = 0;
    //volatile unsigned long packet_length = 0;
    unsigned char tmp8;
    unsigned long cnt = 0;
    unsigned char debug = 0;

    BL_PRINT(LOG_DEBUG, "[FUNET]Driver Debug \n\r");
    //Set 2523 GPIO10 output high, enable GNSS LDO
    *(volatile int *)(0xA2020C18) = 0xF00; //set GPIO mode
    *(volatile int *)(0xA2020004) = 0x400; //set dir as output
    *(volatile int *)(0xA2020304) = 0x400; //set output HIGH
    BL_PRINT(LOG_DEBUG, "[FUNET]GNSS LDO enable ok \n\r");
    //Set 2523 GPIO13 output 32K clock
    *(volatile int *)(0xA2020C18) = 0x700000;
    *(volatile int *)(0xA2020C14) = 0x200000; //set clout mode
    *(volatile int *)(0xA2020E30) = 0x4; //set 32K frequency
    BL_PRINT(LOG_DEBUG, "[FUNET]output 32K clock \n\r");

    GNSS_UART_Init();
    GNSS_UART_SetBaudRate(4);

    BL_PRINT(LOG_DEBUG, "[FUNET]Enable Schmitt trigger \n\r");
    *(volatile int *)(0xa0020604) = 0x00021000;  //Enable Schmitt trigger

    BL_PRINT(LOG_DEBUG, "[FUNET]PurgeFIFO \n\r");
    GNSS_UART_PurgeFIFO();

    BL_PRINT(LOG_DEBUG, "[FUNET]Delay2 \n\r");

    //Wait 3333 FW initial
    for (i = 0; i < 50; ++i) {
        GNSS_UART_Delay();
    }


    BL_PRINT(LOG_DEBUG, "[FUNET]Send PMTK180 to force 3333 into boot rom\n\r");
    //Send PMTK180 to force 3333 into boot rom
    for (i = 1; i < 8; ++i) {
        //	GNSS_UART_SetBaudRate(i, MT6261);
        GNSS_UART_PutByte_Buffer((uint32*)NMEA_START_CMD, sizeof(NMEA_START_CMD));

        BL_PRINT(LOG_DEBUG, "[FUNET]DELAY3\n\r");
        GNSS_UART_Delay();
    }

    BL_PRINT(LOG_DEBUG, "[FUNET]DELAY4\n\r");
    //delay, wait for system restart.
    for (i = 0; i < 10; ++i) {
        GNSS_UART_Delay();
    }
    BL_PRINT(LOG_DEBUG, "[FUNET]GNSS_UART_PurgeFIFO\n\r");
    GNSS_UART_PurgeFIFO();

    BL_PRINT(LOG_DEBUG, "[FUNET]SEND SYNC CHAR\n\r");
    cnt = 0;
    while (1) {

        GNSS_UART_PutByte(MT3301_BOOT_ROM_START_CMD[0]); //First start command sync char
        if (1 == GNSS_UART_GetByte_NO_TIMEOUT(&data8)) {
            tmp8 = 0x5F;
            BL_PRINT(LOG_DEBUG, "[FUNET]Received data:%x \n\r", data8);
            if (tmp8 == data8) {
                //connCOM.m_COM_PutByte(data8);
                GNSS_UART_PurgeFIFO();
                goto SECOND_CHAR;
            } else {
                //while(1);
                //connCOM.m_COM_PutByte(data8);
            }
        }

        cnt++;
        data8 = 0xDD;
        //connCOM.m_COM_PutByte(data8);
        if (cnt > 10000) {
            data8 = 0xCC;
            //connCOM.m_COM_PutByte(data8);
            break;
        }
    }

    //Use Reset way to sync boot rom
    *(volatile int *)(0xA2020C18) = 0x700000; //Close 32K clock to avoid non-sync char received
    GNSS_UART_PurgeFIFO();
    debug = 0xC1;
    cnt = 0;
    for (fail_retry = 0; fail_retry < 5; ++fail_retry) {
        //Set 2523 GPIO10 output low, disable GNSS LDO
        *(volatile int *)(0xA2020C18) = 0xF00; //set GPIO mode
        *(volatile int *)(0xA2020004) = 0x400; //set dir as output
        *(volatile int *)(0xA2020308) = 0x400; //set output LOW

        for (i = 0; i < 300; ++i) {
            GNSS_UART_Delay();
        }

        //Set 2523 GPIO10 output high, enable GNSS LDO
        *(volatile int *)(0xA2020C18) = 0xF00; //set GPIO mode
        *(volatile int *)(0xA2020004) = 0x400; //set dir as output
        *(volatile int *)(0xA2020304) = 0x400; //set output HIGH

        i = 0;
        while (1) {

            GNSS_UART_PutByte(MT3301_BOOT_ROM_START_CMD[0]); //First start command sync char
            if (1 == GNSS_UART_GetByte_NO_TIMEOUT(&data8)) {
                tmp8 = 0x5F;
                if (tmp8 == data8) {
                    //connCOM.m_COM_PutByte(data8);
                    GNSS_UART_PurgeFIFO();
                    //break;
                    goto SECOND_CHAR;
                } else {
                    //while(1);
                    //connCOM.m_COM_PutByte(data8);
                }
            }
            cnt++;
            //connCOM.m_COM_PutByte(debug);
            if (cnt > 50000) {
                //connCOM.m_COM_PutByte(fail_retry);
                debug++;
                cnt = 0;
                if (fail_retry == 4) {
                    while (1);
                } else {
                    break;
                }
            }
        }
    }

SECOND_CHAR:
    BL_PRINT(LOG_DEBUG, "[FUNET]Sync second char \n\r");

    i = 1;
    GNSS_UART_PutByte(MT3301_BOOT_ROM_START_CMD[i]); //2nd sync char
    tmp8 = 0xF5; // ~MT3301_BOOT_ROM_START_CMD[i]
    data8 = GNSS_UART_GetByte();
    if (tmp8 != data8) {
        //connCOM.m_COM_PutByte(data8);
        while (1);
    }

    i = 2;
    GNSS_UART_PutByte(MT3301_BOOT_ROM_START_CMD[i]); //3rd sync char
    tmp8 = 0xAF;
    data8 = GNSS_UART_GetByte();
    if (tmp8 != data8) {
        //connCOM.m_COM_PutByte(data8);
        while (1);
    }

    i = 3;
    GNSS_UART_PutByte(MT3301_BOOT_ROM_START_CMD[i]); //4th sync char
    tmp8 = 0xFA;
    data8 = GNSS_UART_GetByte();
    if (tmp8 != data8) {
        //connCOM.m_COM_PutByte(data8);
        while (1);
    }

    return 0;
}
//------------------------------------------------------------------------------
// boot FlashTool download mode
//------------------------------------------------------------------------------
int Boot_FlashTool(const s_BOOT_FLASHTOOL_ARG  *p_arg, GNSS_Download_Arg arg)
{
    // check data
    if ( NULL == p_arg ) {
        return BROM_INVALID_ARGUMENTS;
    }

    //MTRACE(g_hBROM_DEBUG, "Enter BRom_StartCmd");
    BL_PRINT(LOG_DEBUG, "[FUNET]Enter BRom_StartCmd \n\r");

    // send start command
    if ( BRom_StartCmd(arg) ) {
        return BROM_CMD_START_FAIL;
    }
    BL_PRINT(LOG_DEBUG, "[FUNET]Start Command Sync Done \n\r");

    //MTRACE(g_hBROM_DEBUG, "Start Command Sync Done.");

    // wait for 200ms, because of H/W chip limitation

    // download DA to target address m_da_start_addr
    //MTRACE(g_hBROM_DEBUG, "Boot_FlashTool: BRom_WriteBuf() ...");

    // 0805 who fill these value (p_arg->m_da_start_addr, p_arg->m_da_buf) ???????????

    //Sleep(200);

    //strcpy(Dbgbuf, "BRom_WriteBuf\n");
    //fwrite(Dbgbuf, 1, strlen("BRom_WriteBuf\n"), DbgFILE);
    if (BRom_WriteBuf(arg, p_arg->m_da_start_addr, p_arg->m_da_buf, p_arg->m_da_len)) {
        return BROM_DOWNLOAD_DA_FAIL;
    }
    //MTRACE(g_hBROM_DEBUG, "Boot_FlashTool: BRom_WriteBuf() Pass!");
    BL_PRINT(LOG_DEBUG, "[FUNET]Boot_FlashTool: BRom_WriteBuf() Pass! \n\r");

    // jump to m_da_start_addr to execute DA code on Internal SRAM
    //MTRACE(g_hBROM_DEBUG, "Boot_FlashTool: BRom_JumpCmd() ...");
    if (BRom_JumpCmd( arg, p_arg->m_da_start_addr )) {
        return BROM_CMD_JUMP_FAIL;
    }
    //MTRACE(g_hBROM_DEBUG, "Boot_FlashTool: BRom_JumpCmd() Pass!");
    //strcpy(Dbgbuf, "Boot_FlashTool: BRom_JumpCmd() Pass!\n");
    //fwrite(Dbgbuf, 1, strlen("Boot_FlashTool: BRom_JumpCmd() Pass!\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]Boot_FlashTool: BRom_JumpCmd() Pass! \n\r");
    return BROM_OK;
}
//------------------------------------------------------------------------------
