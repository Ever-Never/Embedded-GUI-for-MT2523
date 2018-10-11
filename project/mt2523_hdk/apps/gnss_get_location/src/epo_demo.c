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

/* add include file here */

#include "gnss_app.h"

//Enable this option to download EPO via bt notification service.
//If you don't need it, you can disable this option in gnss_app.h
#ifdef GNSS_SUPPORT_EPO_DOWNLOAD_BY_BT
#include "FreeRTOS.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "hal_flash.h"
#include "gnss_log.h"
#include "gnss_timer.h"
#include "epo_data.h"
#include "hal_rtc.h"
/***************************************************************************** 
* define
*****************************************************************************/

typedef struct
{
    uint32_t start_address;
    int32_t end_address;
    uint32_t block_count;
    int32_t block_size;
    hal_flash_block_t block_type;
}epo_demo_mem_info_t;


#define MTKEPO_SV_NUMBER 32
#define MTKEPO_RECORD_SIZE 72
#define MTKEPO_SEGMENT_NUM (7 * 4)
#define MTKEPO_ASSIST_EPO_INTERVAL 25
#define RTC_BASE_YEAR 2000
static epo_demo_mem_info_t g_epo_demo_mem_info = {0};
static int32_t emp_unit, emp_unit_end;
static uint32_t sv_buf[(MTKEPO_SV_NUMBER * MTKEPO_RECORD_SIZE)/sizeof(uint32_t)];
static void epo_demo_send_assistance_data_int(int32_t timer_id); 

/**
* @brief       This function used init EPO context by EPO block information.
* @return      void
*/
static void epo_demo_init_mem_info()
{
    GNSSLOGD( "[EPO] init mem info\n");
    g_epo_demo_mem_info.start_address = 0x003F0000;
    g_epo_demo_mem_info.end_address = 0x00400000;
    g_epo_demo_mem_info.block_size = 4*1024;
    g_epo_demo_mem_info.block_type = HAL_FLASH_BLOCK_4K;
    g_epo_demo_mem_info.block_count = 16;
}

/**
* @brief       This function used to erase data on EPO block.
* @return      void
*/
static void epo_demo_flash_erase()
{
    uint32_t erase_address = g_epo_demo_mem_info.start_address;
    uint32_t block_idx;
    int32_t ret;
    GNSSLOGD( "[EPO] erase all fota reserved flash blocks, block_count = %d\n", g_epo_demo_mem_info.block_count);
    for(block_idx = 0; block_idx < g_epo_demo_mem_info.block_count; block_idx++)
    {
        ret = hal_flash_erase(erase_address, g_epo_demo_mem_info.block_type);
        GNSSLOGD( "[EPO] erase ret:%d\n", ret);
        erase_address = erase_address + g_epo_demo_mem_info.block_size;
    }
}

/**
* @brief       Init EPO context & erase relate flash
* @return      void
*/
void epo_demo_memory_init()
{
    int32_t i = 0;
    int32_t ret;
    GNSSLOGD("[EPO_INIT] epo_download_init\n");
    epo_demo_init_mem_info();
    epo_demo_flash_erase();
    for (i = 0; i < MTKEPO_SEGMENT_NUM; i++){
        uint32_t file_offset = i * MTKEPO_SV_NUMBER * MTKEPO_RECORD_SIZE;
        ret = hal_flash_write(g_epo_demo_mem_info.start_address + file_offset, (uint8_t*)(g_epo_data + file_offset), MTKEPO_SV_NUMBER*MTKEPO_RECORD_SIZE);
        GNSSLOGD("[EPO_INIT] hal_flash_write,i:%d, ret:%d\n", i, ret);
    }
}

/**
* @brief       This function used init EPO context by EPO block information.
* @param[in]   buf: buffer pointer
* @param[in]   buf_len: buffer length
* @param[in]   start_record: The started SV number
* @param[in]   record_num: Should be read SV number
* @return      int32_t: read data length
*/
int32_t epo_demo_read_data(void* buf, int32_t buf_len, int32_t start_record, int32_t record_num)
{
    int32_t ret;
    GNSSLOGD("epo_read_data:%d,%d\n", start_record, record_num);
    if (buf_len < record_num * MTKEPO_RECORD_SIZE) {
        return 0;
    }
    ret = hal_flash_read(g_epo_demo_mem_info.start_address + MTKEPO_RECORD_SIZE * start_record, buf, record_num * MTKEPO_RECORD_SIZE);
    GNSSLOGD("epo_read_data:%d,%d\n", ret, record_num * MTKEPO_RECORD_SIZE);
    return record_num * MTKEPO_RECORD_SIZE;
}

/**
* @brief       Translate utc to gnss hours.
* @param[in]   iYr: years
* @param[in]   iMo: months
* @param[in]   iDay: days
* @param[in]   iHr: hours
* @return      int32_t: GNSS hours
*/
static int utc_to_gnss_hour (int iYr, int iMo, int iDay, int iHr)
{
    int iYearsElapsed; // Years since 1980
    int iDaysElapsed; // Days elapsed since Jan 6, 1980
    int iLeapDays; // Leap days since Jan 6, 1980
    int i;
    // Number of days into the year at the start of each month (ignoring leap years)
    const unsigned short doy[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
    iYearsElapsed = iYr - 1980;
    i = 0;
    iLeapDays = 0;
    while (i <= iYearsElapsed) {
        if ((i % 100) == 20) {
            if ((i % 400) == 20) {
                iLeapDays++;
            }
        } else if ((i % 4) == 0) {
            iLeapDays++;
        }
        i++;
    }
    if ((iYearsElapsed % 100) == 20) {
        if (((iYearsElapsed % 400) == 20) && (iMo <= 2)) {
            iLeapDays--;
        }
    } else if (((iYearsElapsed % 4) == 0) && (iMo <= 2)) {
        iLeapDays--;
    }
    iDaysElapsed = iYearsElapsed * 365 + (int)doy[iMo - 1] + iDay + iLeapDays - 6;
    // Convert time to GNSS weeks and seconds
    return (iDaysElapsed * 24 + iHr);
}

/**
* @brief       Read current epo segment (6 hours).
* @param[in]   iYr: years
* @param[in]   iMo: months
* @param[in]   iDay: days
* @param[in]   iHr: hours
* @param[in]   epobuf: buffer to receive epo data
* @param[in]   epobuf_len: buffer length
* @return      int32_t: read length
*/
static int32_t epo_demo_get_segment(int iYr, int iMo, int iDay, int iHr, uint8_t *epobuf, int32_t epobuf_len){
    
    int segment, epo_gnss_hour, current_gnss_hour;
    
    // read the header (assume EPO file has passed integrity check)
    epo_demo_read_data(epobuf, epobuf_len, 0, 1);
    memcpy(&epo_gnss_hour, epobuf, sizeof(epo_gnss_hour));
    GNSSLOGD("epo_gnss_hour:%x\n", epo_gnss_hour);
    epo_gnss_hour &= 0x00FFFFFF;
    // determine the segment to use
    current_gnss_hour = utc_to_gnss_hour(iYr, iMo, iDay, iHr);
    segment = (current_gnss_hour - epo_gnss_hour) / 6;
    GNSSLOGD("epo_demo_get_segment:%d,%d,%d\n", segment,epo_gnss_hour,current_gnss_hour);
    if ((segment < 0) || (segment >= MTKEPO_SEGMENT_NUM)) {
        return -1;
    }
    
    return segment;
}

/**
* @brief       Aiding EPO data to GNSS chip.
* @param[in]   iYr: years
* @param[in]   iMo: months
* @param[in]   iDay: days
* @param[in]   iHr: hours
* @return      void
*/
void epo_demo_send_assistance_data (int iYr, int iMo, int iDay, int iHr)
{
    int segment;
    uint32_t epobuf[MTKEPO_RECORD_SIZE/sizeof(uint32_t)];
    //int8_t strbuf[200], outbuf[200];
    hal_rtc_time_t rtc_time = {0};
    char temp_buf[100] = {0};
    
    // note: please set correct UTC time to rtc before here, you can call hal_rtc_set_time() to set rtc.
    hal_rtc_get_time(&rtc_time);
    sprintf(temp_buf, 
            "PMTK740,%4d,%02d,%02d,%02d,%02d,%02d", 
            rtc_time.rtc_year + RTC_BASE_YEAR, 
            rtc_time.rtc_mon, 
            rtc_time.rtc_day,
            (rtc_time.rtc_hour),
            rtc_time.rtc_min,
            rtc_time.rtc_sec);
    gnss_app_send_cmd((int8_t*) temp_buf, strlen(temp_buf));

    // read binary EPO data and sent it to GNSS chip
    GNSSLOGD("epo_demo_send_assistance_data:%d,%d,%d,%d\n", iYr, iMo, iDay, iHr);
    epo_demo_init_mem_info();
    segment = epo_demo_get_segment(iYr, iMo, iDay, iHr, (uint8_t *) &epobuf, sizeof(epobuf));
    if (segment == -1){
        return;
    }

    emp_unit = segment*(MTKEPO_SV_NUMBER);
    emp_unit_end = emp_unit + MTKEPO_SV_NUMBER;
    GNSSLOGD("start aiding. emp_unit:%d, emp_unit_end:%d\n", emp_unit, emp_unit_end);
    epo_demo_read_data(sv_buf, sizeof(sv_buf), emp_unit, MTKEPO_SV_NUMBER);
    gnss_start_repeat_timer(MTKEPO_ASSIST_EPO_INTERVAL, epo_demo_send_assistance_data_int);
}

/**
* @brief       Send EPO data(single SV) to GNSS chip.
* @param[in]   timer_id: The timer id
* @return      void
*/
void epo_demo_send_assistance_data_int(int32_t timer_id)
{
    unsigned int *epobuf;
    int8_t strbuf[200];
    if (emp_unit == emp_unit_end) {
        return;
    }
    
    epobuf = (unsigned int*) (sv_buf + ((emp_unit + MTKEPO_SV_NUMBER - emp_unit_end) * MTKEPO_RECORD_SIZE) / 4);
    // assume host system is little-endian
    sprintf((char*) strbuf,
        "PMTK721,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X",
        (unsigned int) (MTKEPO_SV_NUMBER + emp_unit - emp_unit_end + 1),
        epobuf[0], epobuf[1], epobuf[2], epobuf[3], epobuf[4], epobuf[5],
        epobuf[6], epobuf[7], epobuf[8], epobuf[9], epobuf[10], epobuf[11],
        epobuf[12], epobuf[13], epobuf[14], epobuf[15], epobuf[16], epobuf[17]);
    gnss_app_send_cmd(strbuf, strlen((char*)strbuf));

    emp_unit++;
    if (emp_unit == emp_unit_end) {
        GNSSLOGD("Finished aiding");
        gnss_stop_timer(timer_id);
    }
}

#endif /*GNSS_SUPPORT_EPO_DOWNLOAD_BY_BT*/


