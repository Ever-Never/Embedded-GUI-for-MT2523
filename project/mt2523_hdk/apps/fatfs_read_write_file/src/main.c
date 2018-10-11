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
#include "mt2523.h"

/* hal includes */
#include "hal.h"

#include "sys_init.h"
#include "task_def.h"
#include "ff.h"

#ifdef MTK_FATFS_ON_SERIAL_NAND
#include "serial_nand_flash.h"
#include "spi_nand_fdm.h"
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


FIL fdst;                      /* file target which must be a global variable name if it would be accessed with global scope. */
FATFS fatfs_0;                 /* fs target which must be a global variable name if it would be accessed with global scope. */
FATFS fatfs_1;                 /* fs target which must be a global variable name if it would be accessed with global scope. */
FATFS fatfs_2;                 /* fs target which must be a global variable name if it would be accessed with global scope. */

/* Private functions ---------------------------------------------------------*/
log_create_module(fatfs, PRINT_LEVEL_INFO);


/**
* @brief     write one file and then read the file to verify whether the file is written successfully.
* @param[in]   None.
* @return      None.
*/
static int  fatfs_test_by_partition(uint32_t partition_number )
{
    uint32_t temp_count;
    BYTE buffer_src[1024];
    BYTE buffer_dst[1024];
    FRESULT res;
    BYTE test_wrok[1024];
    UINT  length_written, length_read;

    if (partition_number > 2) {
        LOG_I(fatfs, "partition number is error[%d]\n\r", partition_number);
        return -10;
    }
    for (temp_count = 0; temp_count < 1000; temp_count++) {
        buffer_src[temp_count] = temp_count;
    }

    if (0 == partition_number) {
        res = f_mount(&fatfs_0, _T("0:"), 1);
    } else if (1 == partition_number) {
        res = f_mount(&fatfs_1, _T("1:"), 1);
    } else if (2 == partition_number) {
        res = f_mount(&fatfs_2, _T("2:"), 1);
    }
    if (FR_OK != res) {
        if (FR_NO_FILESYSTEM == res ) {
            if (0 == partition_number) {
                res = f_mkfs(_T("0:"), FM_ANY, 0, test_wrok, sizeof(test_wrok));
            } else if (1 == partition_number) {
                res = f_mkfs(_T("1:"), FM_ANY, 0, test_wrok, sizeof(test_wrok));
            } else if (2 == partition_number) {
                res = f_mkfs(_T("2:"), FM_ANY, 0, test_wrok, sizeof(test_wrok));
            }

            if (FR_OK != res) {
                LOG_I(fatfs, "Failed to f_mkfs the partition[%d], res=%d\n\r", partition_number, res);
                return -1;
            } else {
                LOG_I(fatfs, "Partition[%d] is formated successfully\n\r", partition_number);
            }

            if (0 == partition_number) {
                res = f_mount(&fatfs_0, _T("0:"), 1);
            } else if (1 == partition_number) {
                res = f_mount(&fatfs_1, _T("1:"), 1);
            } else if (2 == partition_number) {
                res = f_mount(&fatfs_2, _T("2:"), 1);
            }

            if (FR_OK != res) {
                LOG_I(fatfs, "Failed to mount the partition[%d], res=%d\n\r", partition_number, res);
                return -2;
            } else {
                LOG_I(fatfs, "Mount the partition[%d] OK!\n\r", partition_number);
            }
        } else {
            LOG_I(fatfs, "Failed to mount the partition[%d], res=%d\n\r", partition_number, res);
            return -3;
        }
    } else {
        LOG_I(fatfs, "Mount the partition[%d] OK!\n\r", partition_number);
    }

    /* Step1: Create a new file for writing and reading. */
    if (0 == partition_number) {
        res = f_open(&fdst, _T("0:/Test_W0.txt"), FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    } else if (1 == partition_number) {
        res = f_open(&fdst, _T("1:/Test_W1.txt"), FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    } else if (2 == partition_number) {
        res = f_open(&fdst, _T("2:/Test_W2.txt"), FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    }
    if (FR_OK != res) {
        LOG_I(fatfs, "[%d]Failed to create the file, res=%d\n\r", partition_number, res);
        return -4;
    }

    /* Step2: Write the file */
    res = f_write(&fdst, buffer_src, 1000, &length_written);
    if (FR_OK != res) {
        LOG_I(fatfs, "[%d]Failed to write the file, res=%d\n\r", partition_number, res);
        res = f_close(&fdst);
        return -5;
    }


    /* Close the open file object, flush the cached information of the file back to the volume.*/
    res = f_close(&fdst);
    if (FR_OK != res) {
        LOG_I(fatfs, "[%d]Failed to close the file, res=%d\n\r", partition_number, res);
        res = f_close(&fdst);
        return -6;
    }


    if (0 == partition_number) {
        res = f_open(&fdst, _T("0:/Test_W0.txt"), FA_OPEN_EXISTING | FA_READ | FA_WRITE);
    } else if (1 == partition_number) {
        res = f_open(&fdst, _T("1:/Test_W1.txt"), FA_OPEN_EXISTING | FA_READ | FA_WRITE);
    } else if (2 == partition_number) {
        res = f_open(&fdst, _T("2:/Test_W2.txt"), FA_OPEN_EXISTING | FA_READ | FA_WRITE);
    }
    if (FR_OK != res) {
        LOG_I(fatfs, "[%d]Failed to open the file, res=%d\n\r", partition_number, res);
        res = f_close(&fdst);
        return -7;
    }


    /* Step3: Read the file and verify whether the data is the same as written. */
    memset(buffer_dst, 0, 1000);
    res = f_read(&fdst, buffer_dst, 1000, &length_read);
    if (FR_OK != res) {
        LOG_I(fatfs, "[%d]Failed to read the file, res=%d\n\r", partition_number, res);
        res = f_close(&fdst);
        return -8;
    }
    res = f_close(&fdst);

    for (temp_count = 0; temp_count < 1000; temp_count++) {
        if (buffer_dst[temp_count] != buffer_src[temp_count]) {
            LOG_I(fatfs, "Failed: data read is different from the data written in partition[%d], res=%d\n\r", partition_number, res);
            res = f_close(&fdst);
            return -9;
        }
    }

    return 0;
}


/**
* @brief       test file read and written for every partition.
* @param[in]   None.
* @return      None.
*/
static void fatfs_read_write_file(void)
{
    int res_partition;

    res_partition = fatfs_test_by_partition(0);
    if (0 != res_partition) {
        LOG_I(fatfs, "partition[0] test fail, res=%d\n\r", res_partition);
        return;
    }
#ifdef MTK_FATFS_ON_SERIAL_NAND
    res_partition = fatfs_test_by_partition(1);
    if (0 != res_partition) {
        LOG_I(fatfs, "partition[1] test fail, res=%d\n\r", res_partition);
        return;
    }
#endif
    LOG_I(fatfs, "The file is written and read successfully!\n\r");
}

int main(void)
{

    /* Do system initialization, eg: hardware, nvdm. */
    system_init();

    /* system log initialization.
     * This is the simplest way to initialize system log, that just inputs three NULLs
     * as input arguments. User can use advanced feature of system log along with NVDM.
     * For more details, please refer to the log dev guide under /doc folder or projects
     * under project/mtxxxx_hdk/apps/.
     */
    log_init(NULL, NULL, NULL);

    __enable_irq();
    __enable_fault_irq();

#ifdef MTK_FATFS_ON_SERIAL_NAND
#define FREQUENCY (13000000)
    LOG_I(fatfs, "\r\n SPI Nand interface init \r\n");
    serial_nand_init(HAL_SPI_MASTER_2, FREQUENCY);
    LOG_I(fatfs, "\r\n FDM init \r\n");
    spi_nand_fdm_DAL_init();
    LOG_I(fatfs, "\r\n FDM init end \r\n");
#endif

    fatfs_read_write_file();

    for ( ;; );
}

