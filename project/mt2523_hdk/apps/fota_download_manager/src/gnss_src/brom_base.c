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
 *  brom_base.cpp
 *
 * Project:
 * --------
 *  BootRom Library
 *
 * Description:
 * ------------
 *  BootRom base class with basic bootrom command implementation.
 *
 *******************************************************************************/
#include "DOWNLOAD.H"
#include "brom_base.h"
#include "GNSS_DL_api.h"
#include "flashtool.h"
#include "gnss_uart.h"
#include <string.h>
#include "bl_common.h"

// BootRom Base BaudRate
#define BOOT_ROM_BASE_BAUDRATE   9600

// BootRom Command
#define BOOT_ROM_WRITE_CMD    0xA1
#define BOOT_ROM_CHECKSUM_CMD 0xA4
#define BOOT_ROM_JUMP_CMD     0xA8
#define BOOT_ROM_WRITE32_CMD  0xAE


//------------------------------------------------------------------------------
// bootrom command
//------------------------------------------------------------------------------
static int BRom_CmdSend(GNSS_Download_Arg arg, unsigned char cmd)
{
    unsigned char result;
    unsigned char *p_result;
    //DWORD err;
    //int ret;

    p_result = &result;

    if ( NULL != arg.m_p_bootstop && BOOT_STOP == (*(arg.m_p_bootstop)) ) {
        //MTRACE_WARN(g_hBROM_DEBUG, "BRom_StartCmd: BOOT_STOP!, m_p_bootstop(0x%08X)=%u.", m_p_bootstop, *m_p_bootstop);
        return (BROM_CMD_START_FAIL + 6);
    }

    GNSS_UART_PutByte(cmd);
    //com_driver.m_cb_WriteData(arg.m_da_cmd, arg.m_connCOM, &cmd, 1, TIMEOUT);
    //sprintf(Dbgbuf, "BRom_Base::BRom_CmdSend(0x%02X)\n",cmd);
    //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_CmdSend(0x%02X)\n"), DbgFILE);

    // yield for context switch before read command
    //Sleep(1);

    *p_result = GNSS_UART_GetByte();
    //com_driver.m_cb_ReadData(arg.m_da_cmd, arg.m_connCOM, p_result, 1, TIMEOUT);
    //sprintf(Dbgbuf, "BRom_Base::BRom_CmdSend,Read(0x%02X)\n",*p_result);
    //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_CmdSend,Read(0x%02X)\n"), DbgFILE);

    if ( cmd != *p_result ) {
        //sprintf(Dbgbuf, "BRom_Base::BRom_CmdSend(0x%02X): bootrom response is incorrect!, cmd(0x%02X) != result(0x%02X).\n", cmd, cmd, *p_result);
        //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_CmdSend(0x%02X): bootrom response is incorrect!, cmd(0x%02X) != result(0x%02X).\n"), DbgFILE);

        //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_CmdSend(0x%02X): bootrom response is incorrect!, cmd(0x%02X) != result(0x%02X).", cmd, cmd, *p_result);
        return 4;
    } else {
        return 0;
    }
}


static int BRom_OutData(GNSS_Download_Arg arg, unsigned int data)
{
    unsigned int tmp32;
    //DWORD err;

    if ( NULL != arg.m_p_bootstop && BOOT_STOP == (*(arg.m_p_bootstop)) ) {
        //MTRACE_WARN(g_hBROM_DEBUG, "BRom_StartCmd: BOOT_STOP!, m_p_bootstop(0x%08X)=%u.", m_p_bootstop, *m_p_bootstop);
        return (BROM_CMD_START_FAIL + 6);
    }

    //com_driver.m_cb_WriteData32(arg.m_da_cmd, arg.m_connCOM, data, TIMEOUT);
    GNSS_UART_PutData32(data);

    // yield for context switch before read command
    //Sleep(4);

    //com_driver.m_cb_ReadData32(arg.m_da_cmd, arg.m_connCOM, tmp32, TIMEOUT);
    tmp32 = GNSS_UART_GetData32();

    if (tmp32 != data) {
        return 4;
    }

    return 0;
}


static int BRom_CheckSumCmd(GNSS_Download_Arg arg, unsigned int ulBaseAddr, unsigned int num_of_word, unsigned short *result)
{
    int ret;

    if (BRom_CmdSend(arg, BOOT_ROM_CHECKSUM_CMD )) {
        return 1;
    }

    if ((ret = BRom_OutData(arg, ulBaseAddr))) {
        //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_CheckSumCmd(): BRom_OutData(0x%08X): send base address fail!, Err(%d).", ulBaseAddr, ret);
        BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_CheckSumCmd(): BRom_OutData(%x): send base address fail!, Err(%d). \n\r", ulBaseAddr, ret);
        return 2;
    }

    if ((ret = BRom_OutData(arg, num_of_word))) {
        //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_CheckSumCmd(): BRom_OutData(%lu): send number of word fail!, Err(%d).", num_of_word, ret);
        BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_CheckSumCmd(): BRom_OutData(%d): send number of word fail!, Err(%d). \n\r", num_of_word, ret);
        return 3;
    }

    *result = GNSS_UART_GetData16();
    //com_driver.m_cb_ReadData16(arg.m_da_cmd, arg.m_connCOM, *result, TIMEOUT);

    return 0;
}


int BRom_JumpCmd(GNSS_Download_Arg arg, unsigned int addr)
{
    int ret;

    if (BRom_CmdSend(arg, BOOT_ROM_JUMP_CMD )) {
        return 1;
    }

    if ((ret = BRom_OutData(arg, addr))) {
        //sprintf(Dbgbuf, "BRom_Base::BRom_JumpCmd(): BRom_OutData(0x%08X): send jump address fail!, Err(%d).\n", addr, ret);
        //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_JumpCmd(): BRom_OutData(0xXXXXXXXX): send jump address fail!, Err(%d).\n"), DbgFILE);
        //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_JumpCmd(): BRom_OutData(0x%08X): send jump address fail!, Err(%d).", addr, ret);
        BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_JumpCmd(): BRom_OutData(%x): send jump address fail!, Err(%d). \n\r", addr, ret);
        return 2;
    }
    //sprintf(Dbgbuf, "BRom_Base::BRom_JumpCmd(): Jump succeed \n");
    //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_JumpCmd(): Jump succeed \n"), DbgFILE);
    return 0;
}


int BRom_WriteBuf(
    GNSS_Download_Arg arg,
    unsigned int ulBaseAddr,
    const unsigned char *buf_in, unsigned int num_of_byte)
{
    const unsigned char *buf = (const unsigned char *)buf_in;
    unsigned short data;
    unsigned int num_of_word = (num_of_byte + 1) / 2;
    int err;
    unsigned char rate;
    unsigned int finish_rate = 0;
    //unsigned int rate;
    unsigned int i = 0;
    unsigned short checksum = 0;
    unsigned short brom_checksum = 0;
    unsigned int accuracy;
    unsigned char write_buf[10];
    unsigned char read_buf[10];
    int j;

    if ( NULL == buf || 0 >= num_of_byte ) {
        return 1;
    }

    //Callback function of DA download initialize
    if (arg.m_cb_download_conn_da_init != NULL) {
        arg.m_cb_download_conn_da_init(arg.m_cb_download_conn_da_init_arg);
    }

    if (BRom_CmdSend(arg, BOOT_ROM_WRITE_CMD )) {
        return 2;
    }

    //sprintf(Dbgbuf, "BRom_Base::BRom_WriteBuf(): Send BaseAddr\n");
    //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_WriteBuf(): Send BaseAddr\n"), DbgFILE);
    //MTRACE(g_hBROM_DEBUG, "BRom_Base::BRom_WriteBuf(): Send BaseAddr");
    BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_WriteBuf(): Send BaseAddr \n\r");

    if ((err = BRom_OutData(arg, ulBaseAddr))) {
        //sprintf(Dbgbuf, "BRom_Base::BRom_WriteBuf(): BRom_OutData(0x%08X): send base address fail!, Err(%d)\n",ulBaseAddr, err);
        //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_WriteBuf(): BRom_OutData(0xXXXXXXXX): send base address fail!, Err(%d)\n"), DbgFILE);
        //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_WriteBuf(): BRom_OutData(0x%08X): send base address fail!, Err(%d).", ulBaseAddr, err);
        BL_PRINT(LOG_ERROR, "[FUNET]BRom_Base::BRom_WriteBuf(): BRom_OutData(%x): send base address fail!, Err(%d).\n\r", ulBaseAddr, err );
        return 3;
    }

    if ((err = BRom_OutData(arg, num_of_word))) {
        //sprintf(Dbgbuf, "BRom_Base::BRom_WriteBuf(): BRom_OutData(%lu): send number of word fail!, num_of_byte(%lu), Err(%d).\n",num_of_word, num_of_byte, err);
        //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_WriteBuf(): BRom_OutData(0xXXXXXXXX): send base address fail!, Err(%d)\n"), DbgFILE);
        //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_WriteBuf(): BRom_OutData(%lu): send number of word fail!, num_of_byte(%lu), Err(%d).", num_of_word, num_of_byte, err);
        BL_PRINT(LOG_ERROR, "[FUNET]BRom_Base::BRom_WriteBuf(): BRom_OutData(%d): send number of word fail!, num_of_byte(%d), Err(%d).\n\r", num_of_word, num_of_byte, err );
        return 4;
    }


    accuracy = ACCURACY_1_100;
    //MTRACE(g_hBROM_DEBUG, "BRom_Base::BRom_WriteBuf(): DA download Start");
    BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_WriteBuf(): DA download Start \n\r");

    while ( i < num_of_byte ) {
        if ( NULL != arg.m_p_bootstop && BOOT_STOP == (*(arg.m_p_bootstop)) ) {
            //sprintf(Dbgbuf, "BRom_Base::WriteData(): BOOT_STOP!, m_p_bootstop(0x%08X)=%u.\n", m_p_bootstop, *m_p_bootstop);
            //fwrite(Dbgbuf, 1, strlen("BRom_Base::WriteData(): BOOT_STOP!, m_p_bootstop(0x%08XXXXXXX)=%u.\n"), DbgFILE);
            //MTRACE_WARN(g_hBROM_DEBUG, "BRom_Base::WriteData(): BOOT_STOP!, m_p_bootstop(0x%08X)=%u.", m_p_bootstop, *m_p_bootstop);
            //BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::WriteData(): BOOT_STOP!, m_p_bootstop(0x%08X)=%u. \n\r", arg.m_p_bootstop, *(arg.m_p_bootstop));
            return 1;
        }
        // if PC is Windows NT, 2000, XP series, send 10 bytes each time to increase the download speed!

        // copy from buf to write_buf by swap order
        for (j = 0; j < 10; j += 2) {
            write_buf[j] = buf[i + j + 1];
            write_buf[j + 1] = buf[i + j];

            data = (((unsigned short)write_buf[j]) << 8) & 0xFF00;
            data |= ((unsigned short)write_buf[j + 1]) & 0x00FF;

            // update checksum
            checksum ^= data;
        }

        // write
        GNSS_UART_PutByte_Buffer((uint32 *) write_buf, 10);
        //com_driver.m_cb_WriteData(arg.m_da_cmd, arg.m_connCOM, write_buf, 10, TIMEOUT);


        // read bootrom echo to verify
        GNSS_UART_GetByte_Buffer((uint32 *) read_buf, 10);
        //com_driver.m_cb_ReadData(arg.m_da_cmd, arg.m_connCOM, read_buf, 10, TIMEOUT);

        //sprintf(Dbgbuf, "BRom_Base::BRom_WriteBuf(): Progress(%d%),already:%d, to_do:%d\n",(int)((float)i/(float)num_of_byte*100.0f),i,num_of_byte);
        //sfwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_WriteBuf(): Progress(ddd%),already:%ddddddddd, to_do:%ddddddddd\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_WriteBuf(): Progress(%d%%),already:%d, to_do:%d \n\r", (int)((float)i / (float)num_of_byte * 100.0f), i, num_of_byte);

        if (memcmp(write_buf, read_buf, 10)) {
            //sprintf(Dbgbuf, "BRom_Base::BRom_WriteBuf(): write_buf={ 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X }. \n",
            //	     write_buf[0], write_buf[1], write_buf[2], write_buf[3], write_buf[4], write_buf[5], write_buf[6], write_buf[7], write_buf[8], write_buf[9]);
            //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_WriteBuf(): write_buf={ 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X }.\n"), DbgFILE);
            //sprintf(Dbgbuf, "BRom_Base::BRom_WriteBuf(): read_buf={ 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X }. \n",
            //	     read_buf[0], read_buf[1], read_buf[2], read_buf[3], read_buf[4], read_buf[5], read_buf[6], read_buf[7], read_buf[8], read_buf[9]);
            //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_WriteBuf(): write_buf={ 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X, 0x%X }.\n"), DbgFILE);
            //sprintf(Dbgbuf, "BRom_Base::BRom_WriteBuf(): write_buf != read_buf\n");
            //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_WriteBuf(): write_buf != read_buf\n"), DbgFILE);
            //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_WriteBuf(): write_buf={ 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X }. ",
            //         write_buf[0], write_buf[1], write_buf[2], write_buf[3], write_buf[4], write_buf[5], write_buf[6], write_buf[7], write_buf[8], write_buf[9]);
            //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_WriteBuf():  read_buf={ 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X }. ",
            //         read_buf[0], read_buf[1], read_buf[2], read_buf[3], read_buf[4], read_buf[5], read_buf[6], read_buf[7], read_buf[8], read_buf[9]);
            //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_WriteBuf(): write_buf != read_buf  ");
            BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_WriteBuf(): write_buf={ %x, %x, %x, %x, %x, %x, %x, %x, %x, %x }. \n\r", write_buf[0], write_buf[1], write_buf[2], write_buf[3], write_buf[4], write_buf[5], write_buf[6], write_buf[7], write_buf[8], write_buf[9]);
            BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_WriteBuf():  read_buf={ %x, %x, %x, %x, %x, %x, %x, %x, %x, %x }. \n\r", read_buf[0], read_buf[1], read_buf[2], read_buf[3], read_buf[4], read_buf[5], read_buf[6], read_buf[7], read_buf[8], read_buf[9]);
            BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_WriteBuf(): write_buf != read_buf  \n\r");
            return 8;
        }

        // increase by 10, because we send 5 WORDs each time
        i += 10;

        //Callback function of DA download
        if (arg.m_cb_download_conn_da != NULL) {
            if ( accuracy < (rate = (unsigned int)(((float)i / num_of_byte) * accuracy)) ) {
                rate = accuracy;
            }

            if ( 0 < (rate - finish_rate) ) {
                finish_rate = rate;
                // progress callback
                arg.m_cb_download_conn_da((unsigned char)finish_rate, i, num_of_byte, arg.m_cb_download_conn_da_arg);
            }
        }

    }

//0805
    // perform checksum verification
    if ((err = BRom_CheckSumCmd(arg, ulBaseAddr, num_of_word, &brom_checksum))) {
        //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_WriteBuf(): BRom_CheckSumCmd() fail!, Err(%d).", err);
        //sprintf(Dbgbuf, "BRom_Base::BRom_WriteBuf(): BRom_CheckSumCmd() fail!, Err(%d).\n", err);
        //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_WriteBuf(): BRom_CheckSumCmd() fail!, Err(%d).\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_WriteBuf(): BRom_CheckSumCmd() fail!, Err(%d). \n\r", err);
        return 9;
    }

    // compare checksum
    if ( checksum != brom_checksum ) {
        //sprintf(Dbgbuf, "BRom_Base::BRom_WriteBuf(): checksum error!, checksum(0x%04X) != brom_checksum(0x%04X).\n", checksum, brom_checksum);
        //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_WriteBuf(): checksum error!, checksum(0xXXXX) != brom_checksum(0xXXXX).\n"), DbgFILE);
        //MTRACE_ERR(g_hBROM_DEBUG, "BRom_Base::BRom_WriteBuf(): checksum error!, checksum(0x%04X) != brom_checksum(0x%04X).", checksum, brom_checksum );
        BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_WriteBuf(): checksum error!, checksum(%x) != brom_checksum(%x) \n\r", checksum, brom_checksum);
        return 10;
    } else {
        //sprintf(Dbgbuf, "BRom_Base::BRom_WriteBuf(): checksum ok!, checksum(0x%04X) == brom_checksum(0x%04X).\n", checksum, brom_checksum);
        //fwrite(Dbgbuf, 1, strlen("BRom_Base::BRom_WriteBuf(): checksum ok!, checksum(0xXXXX) == brom_checksum(0xXXXX).\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]BRom_Base::BRom_WriteBuf(): checksum ok!, checksum(%x) == brom_checksum(%x). \n\r", checksum, brom_checksum);
        //MTRACE(g_hBROM_DEBUG, "BRom_Base::BRom_WriteBuf(): checksum ok!, checksum(0x%04X) == brom_checksum(0x%04X).", checksum, brom_checksum );
    }

    return 0;
}

