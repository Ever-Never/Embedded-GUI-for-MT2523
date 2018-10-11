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
 *  da_cmd.cpp
 *
 * Project:
 * --------
 *  Flash Download/Format Library.
 *
 * Description:
 * ------------
 *  DA(Download Agent) handshake command.
 *
 *******************************************************************************/
#include "brom_base.h"
#include "da_cmd.h"
#include "gnss_uart.h"
#include <string.h>
#include "bl_common.h"

#define UART_BAUDRATE_SYNC_RETRY      50
#define PACKET_RE_TRANSMISSION_TIMES   3
#define DA_FLASH_ERASE_WAITING_TIME      1000


static int WriteData(GNSS_Download_Arg arg, const void *write_buf, unsigned int write_len)
{

    unsigned int BytesOfWriiten = 0;
    unsigned int rest_of_bytes = 0;
    //unsigned long wbytes = 0;
    //unsigned short RetryCount = 0;

    if ( NULL == write_buf || 0 >= write_len ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::WriteData(): invalid arguments, write_buf(0x%08X), write_len(%lu).", write_buf, write_len);
        return 1;
    }

    //RetryCount = 0;
    BytesOfWriiten = 0;
    while ( BytesOfWriiten < write_len ) {

        // check stop flag
        if ( NULL != arg.m_p_bootstop && BOOT_STOP == *(arg.m_p_bootstop) ) {
            //MTRACE(g_hBROM_DEBUG, "DA_cmd::WriteData(): m_stopflag(0x%08X)=%lu, force to stop!", m_stopflag, *m_stopflag);
            return 2;
        }

        //wbytes = 0;
        rest_of_bytes = write_len - BytesOfWriiten;
        //if(!WriteFile(hCOM, ((char *)write_buf)+BytesOfWriiten, rest_of_bytes, &wbytes, NULL)) {
        //   ret = GetLastError();
        //   MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::WriteData(): Retry(%u): WriteFile(%lu/%lu): Err(%u), total=(%lu/%lu).", RetryCount, wbytes, rest_of_bytes, ret, BytesOfWriiten, write_len);
        //   return 3;
        //}

        GNSS_UART_PutByte_Buffer((uint32 *) write_buf, rest_of_bytes);
        //ret = com_driver.m_cb_WriteData(arg.m_da_cmd, arg.m_connCOM, ((char *)write_buf)+BytesOfWriiten, rest_of_bytes, TIMEOUT);

        // update write bytes
        BytesOfWriiten += rest_of_bytes;

    }

    // A thread can relinquish the remainder of its time slice by calling Sleep() function
    // with a sleep time of zero milliseconds.
    // Add Sleep(0) to force context switch to flush UART TX data.
    //Sleep(0);

    //MTRACE(g_hBROM_DEBUG, "DA_cmd::WriteData(): OK. total=(%lu/%lu).", BytesOfWriiten, write_len);
    return 0;
}

int ReadData(GNSS_Download_Arg arg, void *read_buf, unsigned int read_len)
{
    unsigned int BytesOfRead = 0;
    unsigned int rest_of_bytes = 0;
    //unsigned long rbytes = 0;
    //unsigned short RetryCount = 0;

    if ( NULL == read_buf || 0 >= read_len ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::ReadData(): invalid arguments, read_buf(0x%08X), read_len(%lu).", read_buf, read_len);
        return 1;
    }

    // initialize read buffer
    memset(read_buf, '\0' , read_len);

    //RetryCount = 0;
    BytesOfRead = 0;
    while ( BytesOfRead < read_len ) {
        // check stop flag
        if ( NULL != arg.m_p_bootstop && BOOT_STOP == *(arg.m_p_bootstop) ) {
            //MTRACE(g_hBROM_DEBUG, "DA_cmd::ReadData(): m_stopflag(0x%08X)=%lu, force to stop!", m_stopflag, *m_stopflag);
            return 2;
        }

        //rbytes = 0;
        rest_of_bytes = read_len - BytesOfRead;

        GNSS_UART_GetByte_Buffer((uint32 *) read_buf, rest_of_bytes);
        //com_driver.m_cb_ReadData(arg.m_da_cmd, arg.m_connCOM, ((char *)read_buf)+BytesOfRead, rest_of_bytes, TIMEOUT);

        BytesOfRead += rest_of_bytes;

    }

    //MTRACE(g_hBROM_DEBUG, "DA_cmd::ReadData(): OK. total=(%lu/%lu).", BytesOfRead, read_len);
    return 0;
}

int CMD_SetMemBlock(GNSS_Download_Arg arg, ROM_HANDLE_T  *dl_handle)
{
    unsigned char buf[4];
    unsigned int begin_addr;
    unsigned int end_addr;

    // check arguments
    if ( NULL == dl_handle ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_SetMemBlock(): invalid arguments! dl_handle(0x%08X).", dl_handle);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_SetMemBlock(): invalid arguments! dl_handle(%x).\n\r", (unsigned int)dl_handle);
        return 1;
    }

    // send mem block command
    buf[0] = DA_MEM_CMD;
    buf[1] = 0x01;
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_SetMemBlock(): send DA_MEM_CMD(0x%02X) + MEM_BLOCK_COUNT(0x%02X).", buf[0], buf[1]);
    //sprintf(Dbgbuf, "DA_cmd::CMD_SetMemBlock(): send DA_MEM_CMD(0x%02X) + MEM_BLOCK_COUNT(0x%02X).\n", buf[0], buf[1]);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_SetMemBlock(): send DA_MEM_CMD(0x%02X) + MEM_BLOCK_COUNT(0x%02X).\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_SetMemBlock(): send DA_MEM_CMD(%x) + MEM_BLOCK_COUNT(%x).\n\r",  buf[0], buf[1]);
    if (WriteData(arg, buf, 2)) {
        return 2;
    }

    // send MEM begin addr, end addr
    // MOD with 0x08000000(bank1 start address), because our maui_sw remap flash to bank1 on MT6218B series project.
    begin_addr = dl_handle->m_begin_addr % 0x08000000;
    end_addr = dl_handle->m_end_addr % 0x08000000;

    // send begin addr, high byte first
    buf[0] = (unsigned char)((begin_addr >> 24) & 0x000000FF);
    buf[1] = (unsigned char)((begin_addr >> 16) & 0x000000FF);
    buf[2] = (unsigned char)((begin_addr >> 8) & 0x000000FF);
    buf[3] = (unsigned char)((begin_addr)    & 0x000000FF);
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_SetMemBlock(): send MEM_BEGIN_ADDR(0x%02X%02X%02X%02X).", buf[0], buf[1], buf[2], buf[3]);
    //sprintf(Dbgbuf, "DA_cmd::CMD_SetMemBlock(): send MEM_BEGIN_ADDR(0x%02X%02X%02X%02X\n", buf[0], buf[1], buf[2], buf[3]);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_SetMemBlock(): send MEM_BEGIN_ADDR(0x%02X%02X%02X%02X\n\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_SetMemBlock(): send MEM_BEGIN_ADDR( %x%x%x%x).\n\r", buf[0], buf[1], buf[2], buf[3]);
    if (WriteData(arg, buf, 4)) {
        return 3;
    }

    // send end addr, high byte first
    buf[0] = (unsigned char)((end_addr >> 24) & 0x000000FF);
    buf[1] = (unsigned char)((end_addr >> 16) & 0x000000FF);
    buf[2] = (unsigned char)((end_addr >> 8) & 0x000000FF);
    buf[3] = (unsigned char)((end_addr)    & 0x000000FF);
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_SetMemBlock(): send MEM_END_ADDR(0x%02X%02X%02X%02X).", buf[0], buf[1], buf[2], buf[3]);
    //sprintf(Dbgbuf, "DA_cmd::CMD_SetMemBlock(): send MEM_END_ADDR(0x%02X%02X%02X%02X)\n", buf[0], buf[1], buf[2], buf[3]);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_SetMemBlock(): send MEM_END_ADDR(0x%02X%02X%02X%02X)\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_SetMemBlock(): send MEM_END_ADDR(%x%x%x%x).\n\r", buf[0], buf[1], buf[2], buf[3]);
    if (WriteData(arg, buf, 4)) {
        return 4;
    }
    // Because DA receive data approach is polling and no RX buffer,
    // if the size of transmitting data is large, RX buffer of DA will overflow and miss data.
    // wait for a while
    //Sleep(100);

    // read ack
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_SetMemBlock(): wait for ACK.");
    //sprintf(Dbgbuf, "DA_cmd::CMD_SetMemBlock(): wait for ACK.\n");
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_SetMemBlock(): wait for ACK.\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_SetMemBlock(): wait for ACK..\n\r");
    if (ReadData(arg, buf, 1)) {
        return 5;
    }

    if ( ACK != buf[0] ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_SetMemBlock(): non-ACK(0x%02X) return.", buf[0]);
        //sprintf(Dbgbuf, "DA_cmd::CMD_SetMemBlock(): non-ACK(0x%02X) return.\n", buf[0]);
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_SetMemBlock(): non-ACK(0x%02X) return.\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_SetMemBlock(): non-ACK(%x) return.\n\r", buf[0]);
        return 6;
    }
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_SetMemBlock(): ACK(0x%02X) OK!", buf[0]);
    //sprintf(Dbgbuf, "DA_cmd::CMD_SetMemBlock(): ACK(0x%02X) OK!\n", buf[0]);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_SetMemBlock(): ACK(0x%02X) OK!\n"), DbgFILE);
    // read the number of the unchanged data blocks
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_SetMemBlock(): read the number of the unchanged data blocks.");
    //sprintf(Dbgbuf, "DA_cmd::CMD_SetMemBlock(): read the number of the unchanged data blocks.\n");
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_SetMemBlock(): read the number of the unchanged data blocks.\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_SetMemBlock(): ACK(%x) OK!\n\r", buf[0]);
    if (ReadData(arg, buf, 1)) {
        return 7;
    }
    dl_handle->m_num_of_unchanged_data_blocks = buf[0];
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_SetMemBlock(): UNCHANED_DATA_BLOCKS=(0x%02X).", dl_handle->m_num_of_unchanged_data_blocks);
    //sprintf(Dbgbuf, "DA_cmd::CMD_SetMemBlock(): UNCHANED_DATA_BLOCKS=(0x%02X).\n", dl_handle->m_num_of_unchanged_data_blocks);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_SetMemBlock(): UNCHANED_DATA_BLOCKS=(0x%02X).\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_SetMemBlock(): UNCHANED_DATA_BLOCKS=( %x)\n\r", dl_handle->m_num_of_unchanged_data_blocks);
    return 0;
}


int CMD_Finish(GNSS_Download_Arg arg)
{
    unsigned char buf[2];
    //char Dbgbuf[100];

    // send DA_FINISH_CMD command
    buf[0] = DA_FINISH_CMD;
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_Finish(): send DA_FINISH_CMD(0x%02X).", buf[0]);
    //if(WriteData(hCOM, buf, 1))
    //{
    //   return 1;
    //}
    GNSS_UART_PutByte(buf[0]);
    //com_driver.m_cb_WriteData8(arg.m_da_cmd, arg.m_connCOM, buf[0], TIMEOUT);

    //sprintf(Dbgbuf, "DA_cmd::CMD_Finish(): OK!\n");
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_Finish(): OK!\n"), DbgFILE);
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_Finish(): OK!");
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_Finish(): OK!\n\r");
    return 0;
}

int CMD_WriteData(
    GNSS_Download_Arg arg,
    ROM_HANDLE_T  *dl_handle,
    CALLBACK_DOWNLOAD_PROGRESS_INIT  cb_download_flash_init,  void *cb_download_flash_init_arg,
    CALLBACK_DOWNLOAD_PROGRESS  cb_download_flash,  void *cb_download_flash_arg)
{
    static const char ErrAckTable[][64] = {
        "TIMEOUT_DATA",
        "CKSUM_ERROR",
        "RX_BUFFER_FULL",
        "TIMEOUT_CKSUM_LSB",
        "TIMEOUT_CKSUM_MSB",
        "ERASE_TIMEOUT",
        "PROGRAM_TIMEOUT",
        "RECOVERY_BUFFER_FULL",
        "UNKNOWN_ERROR"
    };
    unsigned char buf[5];
    unsigned int finish_rate = 0;
    unsigned int total_bytes = 0;
    unsigned int total_sent_bytes = 0;
    unsigned int accuracy;
    int ret;
    unsigned int sent_bytes;
    unsigned int retry_count = 0;
    unsigned int j;
    unsigned int rate;
    unsigned short checksum;
    unsigned int frame_bytes;
    #ifdef FOTA_FS_ENABLE
    unsigned char* pSendBuffer = NULL;
    FRESULT res;
    UINT length_read = 0U;
    #endif

    // check arguments
    if ( NULL == dl_handle ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): invalid arguments! dl_handle(0x%08X).", dl_handle);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): invalid arguments! dl_handle( %x)..\n\r", (unsigned int)dl_handle);
        return 1;
    }

    total_bytes = dl_handle->m_len;

    // send write command + packet length
    buf[0] = DA_WRITE_CMD;
    buf[1] = (unsigned char)((dl_handle->m_packet_length >> 24) & 0x000000FF);
    buf[2] = (unsigned char)((dl_handle->m_packet_length >> 16) & 0x000000FF);
    buf[3] = (unsigned char)((dl_handle->m_packet_length >> 8)  & 0x000000FF);
    buf[4] = (unsigned char)((dl_handle->m_packet_length)     & 0x000000FF);
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): send DA_WRITE_CMD(0x%02X), PACKET_LENGTH(0x%02X%02X%02X%02X)=%u.", buf[0], buf[1], buf[2], buf[3], buf[4], dl_handle->m_packet_length);
    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): send DA_WRITE_CMD(0x%02X), PACKET_LENGTH(0x%02X%02X%02X%02X)=%u.\n", buf[0], buf[1], buf[2], buf[3], buf[4], dl_handle->m_packet_length);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): send DA_WRITE_CMD(0x%02X), PACKET_LENGTH(0x%02X%02X%02X%02X)=%u.\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): send DA_WRITE_CMD( %x), PACKET_LENGTH( %x%x%x%x)=%u..\n\r", buf[0], buf[1], buf[2], buf[3], buf[4], dl_handle->m_packet_length);
    if (WriteData(arg, buf, 5)) {
        return 3;
    }

    // initialization callback
    if ( NULL != cb_download_flash_init ) {
        //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): CALLBACK: cb_download_flash_init().");
        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): CALLBACK: cb_download_flash_init().\n");
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): CALLBACK: cb_download_flash_init().\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): CALLBACK: cb_download_flash_init().\n\r");
        cb_download_flash_init(cb_download_flash_init_arg);
    }

    accuracy = ACCURACY_1_100;

    // wait for DA to save unchanged data is done
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): wait for DA to save unchanged data.");
    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): wait for DA to save unchanged data.\n");
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): wait for DA to save unchanged data.\n"), DbgFILE);
    // delay to wait for DA to save unchanged data is done
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): wait for DA to save unchanged data.\n\r");
    if (dl_handle->m_num_of_unchanged_data_blocks) {
        // delay_time = N x (32x1024)x(70ns) = N x (32x1024)x7/100000 ms
        // N -> number of unchanged data blocks
        // 32x1024 -> each sector has max 32K word data
        // 70ns -> 70x10-9 the single word read time
        //delay_time = dl_handle->m_num_of_unchanged_data_blocks*32*1024*7/100000;
        //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Sleep(%lums)=UNCHANED_DATA_BLOCKS(%d)x32KWx70ns.", delay_time, dl_handle->m_num_of_unchanged_data_blocks);
        //Sleep(delay_time);
    }

    if ( 0 != (ret = ReadData(arg, buf, 1)) ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)", ret);

        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n", ret);
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d).\n\r", ret);
        return 4;
    }

    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): read ack(0x%02X).\n", buf[0]);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): read ack(0x%02X).\n"), DbgFILE);
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): read ack(0x%02X).", buf[0]);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): read ack( %x).\n\r", buf[0]);

    if ( ACK != buf[0] ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): %s(0x%02X): fail to save all the unchanged data from flash!", buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): %s(0x%02X): fail to save all the unchanged data from flash!\n", buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): %s(0x%02X): fail to save all the unchanged data from flash!\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): %s( %x): fail to save all the unchanged data from flash!\n\r", buf[0] > UNKNOWN_ERROR ? "UNKNOWN_ACK" : ErrAckTable[buf[0]], buf[0]);
        return 5;
    }

    // wait for 1st sector erase done
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): wait for 1st sector erase done.");
    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): wait for 1st sector erase done.\n", ret);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): wait for 1st sector erase done.\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): wait for 1st sector erase done.\n\r");
    if ( 0 != (ret = ReadData(arg, buf, 1)) ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)", ret);
        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n", ret);
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n\r", ret);
        return 6;
    }

    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): read ack(0x%02X).\n", buf[0]);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): read ack(0x%02X).\n"), DbgFILE);
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): read ack(0x%02X).", buf[0]);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): read ack( %x).\n\r", buf[0]);

    if ( ACK != buf[0] ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): %s(0x%02X): fail to erase the 1st sector!", buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n", ret);
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): %s( %x): fail to erase the 1st sector!.\n\r", buf[0] > UNKNOWN_ERROR ? "UNKNOWN_ACK" : ErrAckTable[buf[0]], buf[0]);
        return 7;
    }

    // send all rom files
    finish_rate = 0;
    total_sent_bytes = 0;
    //for(i=0; i<dl_handle->m_rom_list.size(); i++)
    //{
    // send each rom file
    sent_bytes = 0;
    retry_count = 0;
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): %lu bytes, total_sent_bytes=%lu/%lu.", dl_handle->m_len, total_sent_bytes, total_bytes);
    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): %lu bytes, total_sent_bytes=%lu/%lu.\n", dl_handle->m_len, total_sent_bytes, total_bytes);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): %luuuuuu bytes, total_sent_bytes=%luuuuuu/%luuuuuu.\n"), DbgFILE);

    #ifdef FOTA_FS_ENABLE
    /*allocate the tx buffer !*/
    pSendBuffer = (unsigned char*)pvPortMalloc(dl_handle->m_packet_length);
    if(NULL == pSendBuffer){
       BL_PRINT(LOG_DEBUG, "CMD_WriteData::Failed to allocate tx buffer !!");
       return FT_ERROR;
    }
    #endif

    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): %d bytes, total_sent_bytes=%d/%d..\n\r", dl_handle->m_len, total_sent_bytes, total_bytes);
    while ( sent_bytes < dl_handle->m_len ) {

    #ifdef FOTA_FS_ENABLE
    res = f_read(dl_handle->pImageFileHandle, pSendBuffer ,dl_handle->m_packet_length, &length_read);
    if (FR_OK != res){
        BL_PRINT(LOG_DEBUG, "CMD_WriteData::read buffer error, error=%d !!", res);
        vPortFree(pSendBuffer);
        pSendBuffer = NULL;
        return FT_ERROR;
    }
    frame_bytes = length_read;
    #endif

re_transmission:
        // reset the frame checksum
        checksum = 0;
        #ifndef FOTA_FS_ENABLE
        // if the last frame is less than PACKET_LENGTH bytes
        if (dl_handle->m_packet_length > (dl_handle->m_len - sent_bytes)) {
            frame_bytes = dl_handle->m_len - sent_bytes;
        } else {
            // the normal frame
            frame_bytes = dl_handle->m_packet_length;
        }
        #endif
        //sprintf(Dbgbuf, "Send frame,send bytes:%d,frame_bytes:%d\n", sent_bytes,frame_bytes);
        //fwrite(Dbgbuf, 1, strlen("Send frame,send bytes:%dddddddddd,frame_bytes:%dddddddddd\n"), DbgFILE);

        // send frame
        //if(tmp = WriteData(com_driver, arg, dl_handle->m_buf+sent_bytes, frame_bytes))
        /*if (tmp = com_driver.m_cb_WriteData(arg.m_da_cmd, arg.m_connCOM, dl_handle->m_buf+sent_bytes, frame_bytes, 500))
        {
        if (tmp == 1)
         {
        	sprintf(Dbgbuf, "Send frame failed:1\n");
        	fwrite(Dbgbuf, 1, strlen("Send frame failed:1\n"), DbgFILE);
        	sprintf(Dbgbuf, "Send buf(0x%08X),length(%d)\n",dl_handle->m_buf+sent_bytes,frame_bytes);
        	fwrite(Dbgbuf, 1, strlen("Send buf(0x%08xxxxxxX),length(%dddddd)\n"), DbgFILE);
        	}
        if (tmp == 2)
         {
         sprintf(Dbgbuf, "Send frame failed:2\n");
         fwrite(Dbgbuf, 1, strlen("Send frame failed:2\n"), DbgFILE);
        	}
        if (tmp == 3)
         {
         sprintf(Dbgbuf, "Send frame failed:3\n");
         fwrite(Dbgbuf, 1, strlen("Send frame failed:3\n"), DbgFILE);
         sprintf(Dbgbuf, "Send buf(0x%08X),length(%d)\n",dl_handle->m_buf+sent_bytes,frame_bytes);
         fwrite(Dbgbuf, 1, strlen("Send buf(0x%08xxxxxxX),length(%dddddd)\n"), DbgFILE);
        	}
        MTRACE_ERR(g_hBROM_DEBUG, "Send frame failed.");
        sprintf(Dbgbuf, "Send frame failed:%d\n",tmp);
        fwrite(Dbgbuf, 1, strlen("Send frame failed:%d\n"), DbgFILE);
        sprintf(Dbgbuf, "Send buf(0x%08X),length(%d)\n",dl_handle->m_buf+sent_bytes,frame_bytes);
        fwrite(Dbgbuf, 1, strlen("Send buf(0x%08xxxxxxX),length(%dddddd)\n"), DbgFILE);
        goto read_cont_char;
        }*/
        #ifndef FOTA_FS_ENABLE
        GNSS_UART_PutByte_Buffer((uint32 *) (dl_handle->m_buf + sent_bytes), frame_bytes);
        // calculate checksum
        for (j = 0; j < frame_bytes; j++) {
            // WARNING: MUST make sure it unsigned value to do checksum
            checksum += dl_handle->m_buf[sent_bytes + j];
        }
        #else
        GNSS_UART_PutByte_Buffer((uint32 *) (pSendBuffer), frame_bytes);
        // calculate checksum
        for (j = 0; j < frame_bytes; j++) {
            // WARNING: MUST make sure it unsigned value to do checksum
            checksum += pSendBuffer[j];
        }
        #endif

        // send 2 bytes checksum, high byte first
        buf[0] = (unsigned char)((checksum >> 8) & 0x000000FF);
        buf[1] = (unsigned char)((checksum)    & 0x000000FF);

        //sprintf(Dbgbuf, "Send checksum:%02X,%02X\n", buf[1],buf[0]);
        //fwrite(Dbgbuf, 1, strlen("Send checksum:%XXX,%XXX\n"), DbgFILE);
        if (WriteData(arg, buf, 2)) {
            goto read_cont_char;
        }

read_cont_char:
        // read CONT_CHAR
        buf[0] = 0xEE;
        if ( 0 != (ret = ReadData(arg, buf, 1)) ) {
            /*MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)", ret);
            MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): wait for CONT_CHAR(0x%02X) timeout!", CONT_CHAR);
            MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
            sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n", ret);
            fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n"), DbgFILE);
            sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): wait for CONT_CHAR(0x%02X) timeout!\n", CONT_CHAR);
            fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): wait for CONT_CHAR(0x%02X) timeout!\n"), DbgFILE);
            sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.\n", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
            fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%luuuuuu/%luuuuuu.\n"), DbgFILE);	  */
            BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d).\n\r", ret);
            #ifdef FOTA_FS_ENABLE
            vPortFree(pSendBuffer);
            pSendBuffer = NULL;
            #endif
            return 8;
        }
        //sprintf(Dbgbuf, "CONT_CHART Received(0x%02X)\n", buf[0]);
        //fwrite(Dbgbuf, 1, strlen("CONT_CHART Received(0x%XX)\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]CONT_CHART Received( %x).\n\r", buf[0]);

        switch (buf[0]) {
            case CONT_CHAR:
                // sent ok!, reset retry_count

                //sprintf(Dbgbuf, "CONT_CHART Ok(0x%X)\n", buf[0]);
                //fwrite(Dbgbuf, 1, strlen("CONT_CHART Ok(0x%X)\n"), DbgFILE);

                BL_PRINT(LOG_DEBUG, "[FUNET]CONT_CHART Ok( %x).\n\r", buf[0]);
                retry_count = 0;
                break;
            case ERASE_TIMEOUT:
                // flash erase timeout abort transmission
                //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): %s(0x%02X): flash erase timeout abort transmission!", buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
                //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): %s(0x%02X): flash erase timeout abort transmission!\n", buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
                //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): %s(0x%02X): flash erase timeout abort transmission!\n"), DbgFILE);
                //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.\n", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%luuuuuu/%luuuuuu.\n"), DbgFILE);
                #ifdef FOTA_FS_ENABLE
                vPortFree(pSendBuffer);
                pSendBuffer = NULL;
                #endif
                return 9;
            case PROGRAM_TIMEOUT:
                // program timeout abort transmission
                //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): %s(0x%02X): program timeout abort transmission!", buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
                //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): %s(0x%02X): program timeout abort transmission!\n",  buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
                //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): %s(0x%02X): program timeout abort transmission!\n"), DbgFILE);
                //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.\n", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %luuuuu bytes sent, total_bytes=%luuuuu/%luuuuu.\n"), DbgFILE);
                #ifdef FOTA_FS_ENABLE
                vPortFree(pSendBuffer);
                pSendBuffer = NULL;
                #endif
                return 10;
            case RECOVERY_BUFFER_FULL:
                // recovery buffer is not large enough to backup all the unchanged data before erase
                //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): %s(0x%02X): recovery buffer is not large enough to backup all the unchanged data before erase!", buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
                //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): %s(0x%02X): recovery buffer is not large enough to backup all the unchanged data before erase\n",  buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
                //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): %s(0x%02X): recovery buffer is not large enough to backup all the unchanged data before erase\n"), DbgFILE);
                //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.\n", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %luuuuu bytes sent, total_bytes=%luuuuu/%luuuuu.\n"), DbgFILE);
                #ifdef FOTA_FS_ENABLE
                vPortFree(pSendBuffer);
                pSendBuffer = NULL;
                #endif
                return 11;
            case RX_BUFFER_FULL:
            // target RX buffer is full, add delay to wait for flash erase done
            //Sleep(DA_FLASH_ERASE_WAITING_TIME);
            case CKSUM_ERROR:
            case TIMEOUT_DATA:
            case TIMEOUT_CKSUM_LSB:
            case TIMEOUT_CKSUM_MSB:
            default:
                // check retry times
                if ( PACKET_RE_TRANSMISSION_TIMES > retry_count ) {
                    retry_count++;
                    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): %s(0x%02X) received, start to re-transmit\n", retry_count, buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
                    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): %sssssssssss(0x%02X) received, start to re-transmit\n"), DbgFILE);
                    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): %s(0x%02X) received, start to re-transmit.", retry_count, buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
                } else {
                    // fail to re-transmission
                    //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): stop to re-transmit! retry %u times fail!", retry_count, retry_count);
                    //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                    // send NACK to wakeup DA to stop
                    buf[0] = NACK;
                    //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): send NACK(0x%02X) to wakeup DA to stop!", retry_count, NACK);
                    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): send NACK(0x%02X) to wakeup DA to stop!\n", retry_count, NACK);
                    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): send NACK(0x%02X) to wakeup DA to stop!\n"), DbgFILE);
                    if (WriteData(arg, buf, 1)) {
                        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.\n", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%luuuuuu/%luuuuuu.\n"), DbgFILE);
                        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                    }
                    #ifdef FOTA_FS_ENABLE
                    vPortFree(pSendBuffer);
                    pSendBuffer = NULL;
                    #endif
                    return 12;
                }

                // wait for DA clean RX buffer
                //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): wait for DA clean it's RX buffer.", retry_count);
                //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Wrong SYNC_CHAR(%02X) Retry(%u): wait for DA clean it's RX buffer.\n", buf[0], retry_count);
                //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Wrong SYNC_CHAR(%02X) Retry(%u): wait for DA clean it's RX buffer.\n"), DbgFILE);
                if ( 0 != (ret = ReadData(arg, buf, 1)) ) {
                    //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)", ret);
                    //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n", ret);
                    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n"), DbgFILE);
                    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.\n", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%luuuuuu/%luuuuuu.\n"), DbgFILE);
                    #ifdef FOTA_FS_ENABLE
                    vPortFree(pSendBuffer);
                    pSendBuffer = NULL;
                    #endif
                    return 13;
                }

                //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Read ack(0x%02X)\n", buf[0]);
                //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Read ack(0x%02X)\n"), DbgFILE);
                if ( ACK != buf[0] ) {
                    //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): wrong ack(0x%02X) return!", retry_count, buf[0]);
                    //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): wrong ack(0x%02X) return!\n", retry_count, buf[0]);
                    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): wrong ack(0x%02X) return!\n"), DbgFILE);
                    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.\n",  retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%luuuuu/%luuuuu.\n"), DbgFILE);
                    #ifdef FOTA_FS_ENABLE
                    vPortFree(pSendBuffer);
                    pSendBuffer = NULL;
                    #endif
                    return 14;
                }

                // send CONT_CHAR to wakeup DA to start recieving again
                buf[0] = CONT_CHAR;
                //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): send CONT_CHAR to wakeup DA to start recieving again.", retry_count);
                //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): send CONT_CHAR to wakeup DA to start recieving again.\n", retry_count);
                //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): send CONT_CHAR to wakeup DA to start recieving again.\n"), DbgFILE);
                if (WriteData(arg, buf, 1)) {
                    //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.", retry_count, (unsigned short)(((float)finish_rate/accuracy)*100), sent_bytes, total_sent_bytes, total_bytes);
                    #ifdef FOTA_FS_ENABLE
                    vPortFree(pSendBuffer);
                    pSendBuffer = NULL;
                    #endif
                    return 15;
                }

                // re-transmission this frame
                //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): Retry(%u): re-transmission this frame, offset(%lu).", retry_count, sent_bytes);
                //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): Retry(%u): re-transmission this frame, offset(%lu).\n", retry_count, sent_bytes);
                //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): Retry(%u): re-transmission this frame, offset(%lu).\n"), DbgFILE);
                goto re_transmission;

                //break;
        }

        // update progress state
        sent_bytes += frame_bytes;
        total_sent_bytes += frame_bytes;

        // calculate finish rate
        if ( accuracy < (rate = (unsigned int)(((float)total_sent_bytes / total_bytes) * accuracy)) ) {
            rate = accuracy;
        }
        //sprintf(Dbgbuf, "Rate:%d,total_sent_bytes:%d,total_bytes:%d\n",rate,total_sent_bytes,total_bytes);
        //fwrite(Dbgbuf, 1, strlen("Rate:%d,total_sent_bytes:%ddddd,total_bytes:%ddddd\n"), DbgFILE);

        if ( 0 < (rate - finish_rate) ) {
            finish_rate = rate;
            //sprintf(Dbgbuf, "Finish_rate:%d%%,rate:%d%%\n",rate,finish_rate);
            //fwrite(Dbgbuf, 1, strlen("Finish_rate:%d%%,rate:%d%%\n"), DbgFILE);
            // calling callback
            BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): (%d%%): %d bytes sent, total_bytes=%d/%d.\n\r", (unsigned char)finish_rate, sent_bytes, total_sent_bytes, total_bytes);
            if ( NULL != cb_download_flash ) {
                //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): CALLBACK: cb_download_flash().");
                BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): CALLBACK: cb_download_flash().\n\r");
                cb_download_flash((unsigned char)finish_rate, total_sent_bytes, total_bytes, cb_download_flash_arg);
            }
        }
    }
    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.\n", (unsigned char)finish_rate, sent_bytes, total_sent_bytes, total_bytes);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): (%u%%): %luuuuu bytes sent, total_bytes=%luuuuu/%luuuuu.\n"), DbgFILE);
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): (%u%%): %lu bytes sent, total_bytes=%lu/%lu.", (unsigned char)finish_rate, sent_bytes, total_sent_bytes, total_bytes);
    //}
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): (%d%%): %d bytes sent, total_bytes=%d/%d.\n\r", (unsigned char)finish_rate, sent_bytes, total_sent_bytes, total_bytes);

    // wait for recovery done ack
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): wait for DA to perform unchanged data recovery.");
    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): wait for DA to perform unchanged data recovery.\n");
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): wait for DA to perform unchanged data recovery.\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): wait for DA to perform unchanged data recovery.\n\r");
    if ( 0 != (ret = ReadData(arg, buf, 1)) ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)", ret);
        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n", ret);
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n\r", ret);
        #ifdef FOTA_FS_ENABLE
        vPortFree(pSendBuffer);
        pSendBuffer = NULL;
        #endif
        return 16;
    }

    if ( ACK != buf[0] ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): %s(0x%02X): fail to recover all the unchanged data to flash!", buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): %s(0x%02X): fail to recover all the unchanged data to flash!\n", buf[0]>UNKNOWN_ERROR?"UNKNOWN_ACK":ErrAckTable[buf[0]], buf[0]);
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): %s(0x%02X): fail to recover all the unchanged data to flash!\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): %s( %x): fail to recover all the unchanged data to flash!\n\r", buf[0] > UNKNOWN_ERROR ? "UNKNOWN_ACK" : ErrAckTable[buf[0]], buf[0]);
        #ifdef FOTA_FS_ENABLE
        vPortFree(pSendBuffer);
        pSendBuffer = NULL;
        #endif
        return 17;
    }

    // wait for checksum ack
    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): wait for DA to perform flash checksum.");
    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): wait for DA to perform flash checksum.\n");
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): wait for DA to perform flash checksum.\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): wait for DA to perform flash checksum.!\n\r");
    if ( 0 != (ret = ReadData(arg, buf, 1)) ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)", ret);
        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n", ret);
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d)\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): ReadData(): fail, Err(%d).!\n\r", ret);
        #ifdef FOTA_FS_ENABLE
        vPortFree(pSendBuffer);
        pSendBuffer = NULL;
        #endif
        return 18;
    }

    if ( NACK == buf[0] ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): NACK(0x%02X) return, flash checksum error!", buf[0]);
        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): NACK(0x%02X) return, flash checksum error!\n", buf[0]);
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): NACK(0x%02X) return, flash checksum error!\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): NACK( %x) return, flash checksum error!\n\r", buf[0]);
        #ifdef FOTA_FS_ENABLE
        vPortFree(pSendBuffer);
        pSendBuffer = NULL;
        #endif
        return 19;
    } else if ( ACK != buf[0] ) {
        //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): non-ACK(0x%02X) return.\n", buf[0]);
        //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): non-ACK(0x%02X) return.\n"), DbgFILE);
        //MTRACE_ERR(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): non-ACK(0x%02X) return.", buf[0]);
        BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): non-ACK( %x) return.\n\r", buf[0]);
        #ifdef FOTA_FS_ENABLE
        vPortFree(pSendBuffer);
        pSendBuffer = NULL;
        #endif
        return 20;
    }

    //MTRACE(g_hBROM_DEBUG, "DA_cmd::CMD_WriteData(): ACK(0x%02X): checksum OK!", buf[0]);
    //sprintf(Dbgbuf, "DA_cmd::CMD_WriteData(): ACK(0x%02X): checksum OK!\n", buf[0]);
    //fwrite(Dbgbuf, 1, strlen("DA_cmd::CMD_WriteData(): ACK(0x%02X): checksum OK!\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]DA_cmd::CMD_WriteData(): ACK( %x): checksum OK!\n\r", buf[0]);
    #ifdef FOTA_FS_ENABLE
    vPortFree(pSendBuffer);
    pSendBuffer = NULL;
    #endif
    return 0;
}

