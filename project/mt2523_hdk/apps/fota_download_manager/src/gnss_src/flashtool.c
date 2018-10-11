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
 *  flashtool.cpp
 *
 * Project:
 * --------
 *  Flash Download/Format Library.
 *
 * Description:
 * ------------
 *  Exported C interface APIs for FlashTool Library.
 *
 *******************************************************************************/
#include "flashtool.h"
#include "da_cmd.h"
#include "brom_base.h"
#include "DOWNLOAD.H"
#include "GNSS_DL_api.h"
#include <stdio.h>
#include <string.h>
#include "brom.h"
//#include "all_in_one_da.h"
#include "bl_common.h"

#define DEFAULT_READ_TIMEOUT     5000  // default read timeout 
#define DEFAULT_WRITE_TIMEOUT    1500  // default write timeout 




extern kal_uint32 Image$$EXT_BL_IOT_FOTA_IMAGE_BUF$$ZI$$Base;
extern kal_uint32 Image$$EXT_BL_IOT_FOTA_IMAGE_BUF$$ZI$$Length;

//------------------------------------------------------------------------------
// SyncWithDA
//------------------------------------------------------------------------------
static int SyncWithDA(GNSS_Download_Arg arg, s_DA_REPORT_T *p_da_report, s_FlashDeviceKey *p_FlashDeviceKey)
{
    unsigned char   buf[32];
    s_FlashDeviceKey   flash_id;

    if ( NULL == p_da_report ) {
        return FT_INVALID_ARGUMENTS;
    }

    // initialize first
    memset(p_da_report, 0, sizeof(s_DA_REPORT_T));
    p_da_report->expected_da_major_ver = DA_MAJOR_VER;
    p_da_report->expected_da_minor_ver = DA_MINOR_VER;
    p_da_report->flash_device_id = DEVICE_UNKNOWN;

    // get SYNC_CHAR
    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): wait SYNC_CHAR.");
    //sprintf(Dbgbuf, "SyncWithDA(): wait SYNC_CHAR.\n");
    //fwrite(Dbgbuf, 1, strlen("SyncWithDA(): wait SYNC_CHAR.\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]SyncWithDA(): wait SYNC_CHAR\n\r");
    if (ReadData(arg, buf, 1)) {
        //sprintf(Dbgbuf, "SYNC_CHAR no response\n");
        //fwrite(Dbgbuf, 1, strlen("SYNC_CHAR no response\n"), DbgFILE);
        //MTRACE_ERR(g_hBROM_DEBUG, "SyncWithDA(): SYNC_CHAR no response.");
        BL_PRINT(LOG_DEBUG, "[FUNET]SyncWithDA(): SYNC_CHAR no response.\n\r");
        return FT_DA_NO_RESPONSE;
    }
    //sprintf(Dbgbuf, "SYNC_CHAR get:%x\n",buf[0]);
    //fwrite(Dbgbuf, 1, strlen("SYNC_CHAR get:%x\n"), DbgFILE);
    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): SYNC_CHAR Get:%02X.", buf[0]);
    BL_PRINT(LOG_DEBUG, "[FUNET]SyncWithDA(): SYNC_CHAR Get:%x.\n\r", buf[0]);

    if ( SOC_FAIL == buf[0] ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "SyncWithDA(): SOC_FAIL(0x%02X) received from DA.", buf[0]);
        BL_PRINT(LOG_DEBUG, "[FUNET]SyncWithDA(): SOC_FAIL(%x) received from DA.\n\r", buf[0]);
        return FT_DA_SOC_CHECK_FAIL;
    } else if ( HW_ERROR == buf[0] ) {
        return FT_DA_HW_ERROR;
    } else if ( SYNC_CHAR != buf[0] ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "SyncWithDA(): non-SYNC_CHAR(0x%02X) received from DA.", buf[0]);
        BL_PRINT(LOG_DEBUG, "[FUNET]SyncWithDA(): non-SYNC_CHAR(%x) received from DA.\n\r", buf[0]);
        return FT_DA_SYNC_INCORRECT;
    }

    // get DA_MAJOR_VER, DA_MINOR_VER
    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): wait DA_MAJOR_VER, DA_MINOR_VER.");

    BL_PRINT(LOG_DEBUG, "[FUNET]SyncWithDA(): wait DA_MAJOR_VER, DA_MINOR_VER.\n\r");
    //sprintf(Dbgbuf, "SyncWithDA(): wait DA_MAJOR_VER, DA_MINOR_VER.\n");
    //fwrite(Dbgbuf, 1, strlen("SyncWithDA(): wait DA_MAJOR_VER, DA_MINOR_VER.\n"), DbgFILE);
    if (ReadData(arg, buf, 2)) {
        return FT_DA_NO_RESPONSE;
    }

    p_da_report->da_major_ver = buf[0];
    p_da_report->da_minor_ver = buf[1];
    if ( DA_MAJOR_VER != p_da_report->da_major_ver || DA_MINOR_VER != p_da_report->da_minor_ver ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "SyncWithDA(): DA_v%u.%u was expired, expect DA_v%u.%u ", p_da_report->da_major_ver, p_da_report->da_minor_ver, DA_MAJOR_VER, DA_MINOR_VER);
        //sprintf(Dbgbuf, "SyncWithDA(): DA_v%u.%u was expired, expect DA_v%u.%u\n", p_da_report->da_major_ver, p_da_report->da_minor_ver, DA_MAJOR_VER, DA_MINOR_VER);
        //fwrite(Dbgbuf, 1, strlen("SyncWithDA(): DA_v%uuu.%uuu was expired, expect DA_v%uuuu.%uuuu\n"), DbgFILE);
        BL_PRINT(LOG_DEBUG, "[FUNET]SyncWithDA(): DA_v%d.%d was expired, expect DA_v%d.%d .\n\r",  p_da_report->da_major_ver, p_da_report->da_minor_ver, DA_MAJOR_VER, DA_MINOR_VER);
        return FT_DA_VERSION_INCORRECT;
    }

    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): DA_v%u.%u ", p_da_report->da_major_ver, p_da_report->da_minor_ver);
    //sprintf(Dbgbuf, "SyncWithDA(): DA_v%u.%u\n", p_da_report->da_major_ver, p_da_report->da_minor_ver);
    //fwrite(Dbgbuf, 1, strlen("SyncWithDA(): DA_v%u.%u\n"), DbgFILE);
    // get DEVICE_INFO
    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): wait DEVICE_INFO.");
    //sprintf(Dbgbuf, "SyncWithDA(): wait DEVICE_INFO.\n");
    //fwrite(Dbgbuf, 1, strlen("SyncWithDA(): wait DEVICE_INFO.\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]SyncWithDA(): DA_v%d.%d.\n\r",  p_da_report->da_major_ver, p_da_report->da_minor_ver);
    BL_PRINT(LOG_DEBUG, "[FUNET]SyncWithDA(): wait DEVICE_INFO.\n\r");
    if (ReadData(arg, buf, 1)) {
        return FT_DA_NO_RESPONSE;
    }
    p_da_report->flash_device_id = (DEVICE_INFO)buf[0];
    if ( DEVICE_UNKNOWN == p_da_report->flash_device_id ) {
        //MTRACE_ERR(g_hBROM_DEBUG, "SyncWithDA(): DEVICE_INFO(0x%02X)=\"Unknown Flash Device\".", p_da_report->flash_device_id);
        //sprintf(Dbgbuf, "SyncWithDA(): DEVICE_INFO(0x%02X)=\"Unknown Flash Device\"\n", p_da_report->flash_device_id);
        //fwrite(Dbgbuf, 1, strlen("SyncWithDA(): DEVICE_INFO(0x%02X)=\"Unknown Flash Device\"\n"), DbgFILE);
        return FT_DA_UNKNOWN_FLASH_DEVICE;
    }

    // get flash size, manufacture id and device code and ext sram size
    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): wait FLASH_SIZE, MANUFACTURE_ID, DEVICE_CODE and EXT_SRAM_SIZE.");
    //sprintf(Dbgbuf, "SyncWithDA(): wait FLASH_SIZE, MANUFACTURE_ID, DEVICE_CODE and EXT_SRAM_SIZE.\n");
    //fwrite(Dbgbuf, 1, strlen("SyncWithDA(): wait FLASH_SIZE, MANUFACTURE_ID, DEVICE_CODE and EXT_SRAM_SIZE.\n"), DbgFILE);
    if (ReadData(arg, buf, 16)) {
        return FT_DA_NO_RESPONSE;
    }
    // get flash size
    p_da_report->flash_size = ((buf[0] << 24) & 0xFF000000) | ((buf[1] << 16) & 0x00FF0000) | ((buf[2] << 8) & 0x0000FF00) | ((buf[3]) & 0x000000FF);
    // get flash manufacture id and device code
    flash_id.m_ManufactureId   = ((buf[4] << 8) & 0xFF00) | ((buf[5]) & 0x00FF);
    flash_id.m_DeviceCode      = ((buf[6] << 8) & 0xFF00) | ((buf[7]) & 0x00FF);
    flash_id.m_ExtDeviceCode1   = ((buf[8] << 8) & 0xFF00) | ((buf[9]) & 0x00FF);
    flash_id.m_ExtDeviceCode2   = ((buf[10] << 8) & 0xFF00) | ((buf[11]) & 0x00FF);
    if ( NULL != p_FlashDeviceKey ) {
        *p_FlashDeviceKey = flash_id;
    }
    // get external sram size
    p_da_report->ext_sram_size = ((buf[12] << 24) & 0xFF000000) | ((buf[13] << 16) & 0x00FF0000) | ((buf[14] << 8) & 0x0000FF00) | ((buf[15]) & 0x000000FF);

    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): DEVICE_INFO(0x%02X)=\"%s\".\t\t\t\t\t\t\t\t\t\t\t", p_da_report->flash_device_id, GetFlashDeviceNameByTypeId(p_da_report->flash_device_id));
    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): FLASH_SIZE(0x%08X).\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", p_da_report->flash_size);
    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): FLASH_MANU_ID(0x%04X), FLASH_DEV_CODE={ 0x%04X, 0x%04X, 0x%04X }.\t", flash_id.m_ManufactureId, flash_id.m_DeviceCode, flash_id.m_ExtDeviceCode1, flash_id.m_ExtDeviceCode2);
    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): EXT_SRAM_SIZE(0x%08X).\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", p_da_report->ext_sram_size);
    //MTRACE(g_hBROM_DEBUG, "SyncWithDA(): FAT_ADDR(0x%08X), FAT_LEN(0x%08X).\t\t", p_da_report->fat_begin_addr, p_da_report->fat_length);
    //sprintf(Dbgbuf, "SyncWithDA(): SYNC ok.\n");
    //fwrite(Dbgbuf, 1, strlen("SyncWithDA(): SYNC ok.\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]SyncWithDA(): SYNC ok.\n\r");
    return FT_OK;
}

//------------------------------------------------------------------------------
// flash download
//------------------------------------------------------------------------------
static int FlashDownload_Internal(
    GNSS_DA *GNSS_da,
    GNSS_Image_List *GNSS_image_list,
    FLASHTOOL_ARG  *p_arg,
    GNSS_Download_Arg arg)
{
    int ret;//status;
    //int *p_stopflag = arg.m_p_bootstop;
    s_BOOT_FLASHTOOL_ARG boot_flashtool_arg;
    ROM_HANDLE_T dl_handle, *p_dl_handle;
    s_DA_REPORT_T      da_report;
    s_DA_REPORT_T      *pDA_Report;
    s_FlashDeviceKey   flash_id;
    p_dl_handle = &dl_handle;


    #ifndef FOTA_FS_ENABLE
    p_dl_handle->m_buf = GNSS_image_list->m_image_list[0].m_image;
    #else
    p_dl_handle->pImageFileHandle = GNSS_image_list->m_image_list[0].pImageFileHandle;
    #endif
    p_dl_handle->m_len = GNSS_image_list->m_image_list[0].m_size;
    p_dl_handle->m_begin_addr = 0x00000000;
    p_dl_handle->m_end_addr = p_dl_handle->m_begin_addr + p_dl_handle->m_len - 1;
    p_dl_handle->m_packet_length = 1024;


    // check arguments
    if ( NULL == GNSS_image_list || NULL == p_arg || NULL == GNSS_da) {
        BL_PRINT(LOG_ERROR, "[FUNET]Input parameters error \n\r");
        return FT_INVALID_ARGUMENTS;
    }

    // fill boot flashtool arg

    boot_flashtool_arg.m_da_start_addr           = 0x00000C00;
    boot_flashtool_arg.m_da_buf                  = GNSS_da->m_image;
    boot_flashtool_arg.m_da_len                  = GNSS_da->m_size;

    // if disable bbchip eco version check, use bbchip type as return value of PC side bbchip name
    //p_arg->p_bbchip_name = BRom_Base::BBChipTypeToName(p_arg->bbchip);
    // assign NULL to disable eco version check in Boot_FlashTool
    boot_flashtool_arg.m_p_bbchip_name = NULL;

    BL_PRINT(LOG_DEBUG, "[FUNET]Enter Boot_FlashTool \n\r");

    // boot target to flashtool mode
    //MTRACE(g_hBROM_DEBUG, "Enter Boot_FlashTool.");
    ret = Boot_FlashTool(&boot_flashtool_arg, arg);

    // check return value
    if ( BROM_OK != ret ) {
        // boot up failed!
        //MTRACE_ERR(g_hBROM_DEBUG, "FlashDownload_Internal(): BRom_AutoBoot::Boot_FlashTool() fail! , Err(0x%06X).", ret);
        BL_PRINT(LOG_DEBUG, "[FUNET]FlashDownload_Internal(): BRom_AutoBoot::Boot_FlashTool() fail! , Err(%x).\n\r", ret);
        return (FT_BROM_ERROR | ret);
    }
    // reset stop flag
    //*p_stopflag = 0;



    //DA_FinishCmd_Sentry  da_finish_sentry(com.GetHandle(), da_cmd);

    //strcpy(Dbgbuf, "Sync With DA\n");
    //fwrite(Dbgbuf, 1, strlen("Sync With DA\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]Sync With DA \n\r");

    // sync with DA
    pDA_Report = &da_report;
    if ( FT_OK != (ret = SyncWithDA(arg, pDA_Report, &flash_id)) ) {
        return ret;
    }

    // Doesn't format
    pDA_Report->fat_begin_addr = 0;
    pDA_Report->fat_length = 0;

    //sprintf(Dbgbuf, "CMD_SetMemBlock\n");
    //fwrite(Dbgbuf, 1, strlen("CMD_SetMemBlock\n"), DbgFILE);
    //MTRACE(g_hBROM_DEBUG, "CMD_SetMemBlock.");
    BL_PRINT(LOG_DEBUG, "[FUNET]CMD_SetMemBlock.\n\r");
    // set memory block
    if (CMD_SetMemBlock(arg, p_dl_handle)) {
        return FT_DA_SET_DOWNLOAD_BLOCK_FAIL;
    }
    //sprintf(Dbgbuf, "da_cmd.CMD_WriteData\n");
    //fwrite(Dbgbuf, 1, strlen("da_cmd.CMD_WriteData\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]da_cmd.CMD_WriteData.\n\r");
    // write to flash by memory block
    if (CMD_WriteData(arg, p_dl_handle, p_arg->m_cb_download_conn_init, p_arg->m_cb_download_conn_init_arg, p_arg->m_cb_download_conn, p_arg->m_cb_download_conn_arg)) {
        return FT_DA_DOWNLOAD_FAIL;
    }

    // set ret to FT_OK
    ret = FT_OK;

    //MTRACE(g_hBROM_DEBUG, "FlashDownload_Internal(): Success!");
    //sprintf(Dbgbuf, "FlashDownload_Internal(): Success!\n");
    //fwrite(Dbgbuf, 1, strlen("FlashDownload_Internal(): Success!\n"), DbgFILE);
    BL_PRINT(LOG_DEBUG, "[FUNET]FlashDownload_Internal(): Success!\n\r");

    // Finish Cmd
    CMD_Finish(arg);

    return ret;
}


//------------------------------------------------------------------------------
// Exported Functions: FlashDownload
//------------------------------------------------------------------------------
int  FlashDownload(
    GNSS_DA *GNSS_da,
    GNSS_Image_List *GNSS_image_list,
    GNSS_Download_Arg arg)
{
    FLASHTOOL_ARG  flashtool_arg, *p_arg;
    p_arg = &flashtool_arg;

    // check arguments
    if ( NULL == GNSS_image_list || NULL == GNSS_da) {
        BL_PRINT(LOG_ERROR, "[FUNET]Input parameters error \n\r");
        return FT_INVALID_ARGUMENTS;
    }

    p_arg->m_cb_download_conn_init = arg.m_cb_download_conn_init;
    p_arg->m_cb_download_conn_init_arg = arg.m_cb_download_conn_init_arg;
    p_arg->m_cb_download_conn = arg.m_cb_download_conn;
    p_arg->m_cb_download_conn_arg = arg.m_cb_download_conn_arg;
    p_arg->m_cb_download_conn_da_init = arg.m_cb_download_conn_da_init;
    p_arg->m_cb_download_conn_da_init_arg = arg.m_cb_download_conn_da_init_arg;
    p_arg->m_cb_download_conn_da = arg.m_cb_download_conn_da;
    p_arg->m_cb_download_conn_da_arg = arg.m_cb_download_conn_da_arg;
    p_arg->ext_clock = 2;
    p_arg->max_start_cmd_retry_count = 1;
    p_arg->p_bbchip_name = "MT3301";
    p_arg->ms_boot_timeout = 0xFFFFFFF;

    BL_PRINT(LOG_DEBUG, "[FUNET]Enter FlashDownload_Internal \n\r");
    return FlashDownload_Internal(GNSS_da, GNSS_image_list, p_arg, arg);
}

