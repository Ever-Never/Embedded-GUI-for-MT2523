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

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* device.h includes */
#include "mt7687.h"
#include "system_mt7687.h"

#include "sys_init.h"
#include "task_def.h"

#include "ff.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
    FIL fdst;                        /* file target which must be a global variable name if it would be accessed with global scope. */
    FATFS fatfs;                     /* fs target which must be a global variable name if it would be accessed with global scope. */
/* Private functions ---------------------------------------------------------*/
log_create_module(fatfs, PRINT_LEVEL_INFO);


/**
* @brief       write one file and then read the file to verify whether the file is written successfully.
* @param[in]   None.
* @return      None.
*/
static void fatfs_read_write_file(void)
{
    FRESULT res;                     /* fs status infor*/
    UINT temp_count, length_written, length_read;
    BYTE buffer_src[1024];
    BYTE buffer_dst[1024];

    for (temp_count = 0; temp_count < 1000; temp_count++) {
        buffer_src[temp_count] = temp_count;
    }

    /* Mount the partition before it is accessed, and need only do it once. */
    res = f_mount(&fatfs, _T("0:"), 1);
    if (FR_OK != res) {
        LOG_I(fatfs, "Failed to mount the partition!\n\r");
        return;
    }

    /* Step1: Create a new file for writing and reading. */
    res = f_open(&fdst, _T("SD:/Test_W.txt"), FA_CREATE_ALWAYS);
    if (FR_OK != res) {
        LOG_I(fatfs, "Failed to create the file!\n\r");
        return;
    }

    res = f_open(&fdst, _T("SD:/Test_W.txt"), FA_OPEN_EXISTING | FA_WRITE | FA_READ);
    if (FR_OK != res) {
        LOG_I(fatfs, "Failed to open the file!\n\r");
        return;
    }

    /* Step2: Write the file */
    res = f_write(&fdst, buffer_src, 1000, &length_written);
    if (FR_OK != res) {
        LOG_I(fatfs, "Failed to write the file!\n\r");
        res = f_close(&fdst);
        return;
    }

    /* Close the open file object, flush the cached information of the file back to the volume.*/
    res = f_close(&fdst);
    if (FR_OK != res) {
        LOG_I(fatfs, "Failed to close the file!\n\r");
        res = f_close(&fdst);
        return;
    }

    res = f_open(&fdst, _T("SD:/Test_W.txt"), FA_OPEN_EXISTING | FA_READ);
    if (FR_OK != res) {
        LOG_I(fatfs, "Failed to open the file!\n\r");
        res = f_close(&fdst);
        return;
    }

    /* Step3: Read the file and verify whether the data is the same as written. */
    res = f_read(&fdst, buffer_dst, 1000, &length_read);
    if (FR_OK != res) {
        LOG_I(fatfs, "Failed to read the file!\n\r");
        res = f_close(&fdst);
        return;
    }

    for (temp_count = 0; temp_count < 1000; temp_count++) {
        if (buffer_dst[temp_count] != buffer_src[temp_count]) {
            LOG_I(fatfs, "Failed: data read is different from the data written!\n\r");
            res = f_close(&fdst);
            return;
        }
    }
    res = f_close(&fdst);
    LOG_I(fatfs, "The file is written and read successfully!\n\r");
}



int main(void)
{

    /* Do system initialization, eg: hardware, nvdm and random seed. */
    system_init();

    /* system log initialization.
     * This is the simplest way to initialize system log, that just inputs three NULLs
     * as input arguments. User can use advanved feature of system log along with NVDM.
     * For more details, please refer to the log dev guide under /doc folder or projects
     * under project/mtxxxx_hdk/apps/.
     */
    log_init(NULL, NULL, NULL);

    fatfs_read_write_file();
    for ( ;; );
}

