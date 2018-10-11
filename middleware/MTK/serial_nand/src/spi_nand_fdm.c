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

#ifdef MTK_FATFS_ON_SERIAL_NAND
#ifndef __SPI_NAND_FDM_H__
#define __SPI_NAND_FDM_H__

#ifndef __NAND_FDM_50__
#define __NAND_FDM_50__

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal.h"
#include "spi_nand_fdm.h"
#include "hal_nvic.h"
#include "NAND_FDM.h"
#include "fs_port.h"
#include "serial_nand_flash.h"

#define SPI_FREQUENCY (13000000)

NAND_FLASH_DRV_DATA  NANDFlashDriveData;

NAND_FLASH_DRV_DATA *NANDFDMData = &NANDFlashDriveData;

typedef SemaphoreHandle_t fmd_port_mutex_t;
static fmd_port_mutex_t g_xSemaphore_ms = NULL;
static void ms_mutex_create_lock(void)
{
    g_xSemaphore_ms = xSemaphoreCreateMutex();

    if (g_xSemaphore_ms == NULL) {
        assert(0);
    }
}

void ms_mutex_lock(void)
{
    //if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    if (SysInitStatus_Query() == 1) {
        if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == 0) {
            if (xSemaphoreTake(g_xSemaphore_ms, portMAX_DELAY) == pdFALSE) {
                log_hal_error("ms_mutex_lock error\r\n");
            }
        } else {
            //log_hal_error("take ms_mutex_lock in \r\n");
        }
    }
}

void ms_mutex_unlock(void)
{
    //if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    if (SysInitStatus_Query() == 1) {
        if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == 0) {
            if (xSemaphoreGive(g_xSemaphore_ms) == pdFALSE) {
                log_hal_error("ms_mutex_lock error\r\n");
            }
        }
    } else {
        //log_hal_error("take ms_mutex_lock in \r\n");
    }
}


uint32_t INT_GetCurrentTime(void)
{
    return (xTaskGetTickCount());
}

int32_t spi_nand_fdm_ReadSectors(uint32_t Sector, uint32_t Sector_length, void *Data)
{
    int32_t status;
    ms_mutex_lock();
    status = FDM5_ReadSectors(NANDFDMData, Sector, Sector_length, Data);
    ms_mutex_unlock();
    return status;
}

int32_t spi_nand_fdm_WriteSectors(uint32_t Sector, uint32_t Sector_length, void *Data)
{
    int32_t status;
    ms_mutex_lock();
    uint32_t sectors = Sector_length;
    status = FDM5_WriteSectors(NANDFDMData, Sector, sectors, Data);
    ms_mutex_unlock();
    return status;
}

int32_t spi_nand_fdm_LowLevelFormat(void)
{
    uint32_t block_index, status;

    ms_mutex_lock();
    //format Nand flash
    for (block_index = 0; block_index < 2048; block_index++) {
        status = serial_nand_erase((block_index << 18), 256 * 1024);
        if (status != 0) {
            ms_mutex_unlock();
            //log_hal_error("spi_nand_fdm_LowLevelFormat block_index = %d, status = %d \r\n", block_index, status );
            return (-1);

        }
    }

    //Should do SPI and FDM initiation before use File system
    serial_nand_init(HAL_SPI_MASTER_2, SPI_FREQUENCY);
    //FDM init
    status = FDM5_MountDevice(NANDFDMData, 1, 2, 0);
    if (status != 512) {
        ms_mutex_unlock();
        return status;
    }
    ms_mutex_unlock();
    return (status = 0);

}


int32_t spi_nand_GetDiskGeometry(DEVICE_INFO_T *block_size)
{
    uint8_t MediaDescriptor;
    int32_t status;

    ms_mutex_lock();
    FS_PartitionRecord DiskGeometry;
    status = FDM5_GetDiskGeometry(NANDFDMData, &DiskGeometry, &MediaDescriptor);
    if (status == 0 && MediaDescriptor) {
        block_size->sectors = DiskGeometry.Sectors;
        block_size->block_size = 256 * 1024;
        block_size->sector_size = 512;
    }
    ms_mutex_unlock();
    return status;
}

int32_t spi_nand_fdm_DAL_init(void)
{
    int32_t status;
    if (g_xSemaphore_ms == NULL) {
        ms_mutex_create_lock();
    }
    ms_mutex_lock();
    status = FDM5_MountDevice(NANDFDMData, 1, 2, 0);
    if (status != 512) {
        ms_mutex_unlock();
        return status;
    }
    ms_mutex_unlock();
    return (status = 0);
}

#endif
#endif
#endif
