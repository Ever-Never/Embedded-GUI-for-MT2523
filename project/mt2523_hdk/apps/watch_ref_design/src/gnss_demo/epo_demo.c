/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation (MediaTek Software) are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. (MediaTek) and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek (License Agreement) and been granted explicit permission to do so within
 * the License Agreement (Permitted User).  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN AS-IS BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
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


#include <string.h>
#include <stdio.h>

#include "gnss_app.h"

#ifdef GNSS_SUPPORT_EPO

#include "gnss_api.h"
#include "epo_file.h"

#define MTKEPO_SV_NUMBER 32
#define MTKEPO_RECORD_SIZE 72
#define MTKEPO_SEGMENT_NUM (30 * 4)

int32_t epo_read_data(void* buf, int32_t buf_len, int32_t start_record, int32_t record_num)
{
    static int8_t* epo_data;
    if (buf_len < record_num * MTKEPO_RECORD_SIZE) {
        return 0;
    }
    if (epo_data == NULL) {
        //init epo data
        epo_data = (int8_t*) g_epo_data;
    }
    memcpy( buf, epo_data + MTKEPO_RECORD_SIZE * start_record, record_num * MTKEPO_RECORD_SIZE);
    return record_num * MTKEPO_RECORD_SIZE;
}

static uint8_t calc_nmea_checksum (const int8_t* sentence)
{
    uint8_t checksum = 0;
    while (*sentence) {
        checksum ^= (uint8_t)*sentence++;
    }
    return checksum;
}

// translate UTC to GNSS_Hour
int utc_to_gnss_hour (int iYr, int iMo, int iDay, int iHr)
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

void epo_demo_send_assistance_data (int iYr, int iMo, int iDay, int iHr)
{
    int i, segment, epo_gnss_hour, current_gnss_hour;
    unsigned epobuf[MTKEPO_RECORD_SIZE/sizeof(unsigned)];
    int8_t strbuf[200], outbuf[200];

    // read the header (assume EPO file has passed integrity check)
    epo_read_data(epobuf, sizeof(epobuf), 0, 1);
    memcpy(&epo_gnss_hour, epobuf, sizeof(epo_gnss_hour));
    epo_gnss_hour &= 0x00FFFFFF;
    // determine the segment to use
    current_gnss_hour = utc_to_gnss_hour(iYr, iMo, iDay, iHr);
    segment = (current_gnss_hour - epo_gnss_hour) / 6;
    if ((segment < 0) || (segment >= MTKEPO_SEGMENT_NUM)) {
        return;
    }

    // read binary EPO data and sent it to GNSS chip

    for (i = 0; i < MTKEPO_SV_NUMBER; i++) {
        epo_read_data(epobuf, sizeof(epobuf), segment*(MTKEPO_SV_NUMBER) + i, 1);
        // assume host system is little-endian
        sprintf((char*) strbuf,
        "PMTK721,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X",
        i+1,
        epobuf[0], epobuf[1], epobuf[2], epobuf[3], epobuf[4], epobuf[5],
        epobuf[6], epobuf[7], epobuf[8], epobuf[9], epobuf[10], epobuf[11],
        epobuf[12], epobuf[13], epobuf[14], epobuf[15], epobuf[16], epobuf[17]);
        //sprintf((char*) outbuf, "$%s*%02X\r\n", (char*) strbuf, calc_nmea_checksum(strbuf));
        // send them by UART
        //gnss_send_command(outbuf, strlen((char*)outbuf));
        gnss_app_send_cmd(strbuf, strlen(strbuf));
        //GNSSLOGD("%s", outbuf); // for demo
    }
}

#endif
