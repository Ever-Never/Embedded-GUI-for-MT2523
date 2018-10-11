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

#include "hal.h"
#include "usbms_msdc.h"
#include "spi_nand_fdm.h"

#ifdef MTK_FATFS_ON_SERIAL_NAND
static bool usbms_read_nand(void *data, uint32_t LBA, uint16_t sec_len)
{
    int32_t status;
    ms_mutex_lock();
    status = spi_nand_fdm_ReadSectors(LBA, sec_len, data);
    ms_mutex_unlock();
    return (0 == status ? true : false);
}

static bool usbms_write_nand(void *data, uint32_t LBA, uint16_t sec_len)
{
    int32_t status;
    ms_mutex_lock();
    status = spi_nand_fdm_WriteSectors(LBA, sec_len, data);
    ms_mutex_unlock();
    return (0 == status ? true : false);
}


static bool usbms_checkmedia_exist_nand(void)
{
    return (true); 
}


static bool usbms_format_nand(void)
{
    int32_t status;
    ms_mutex_lock();
    status = spi_nand_fdm_LowLevelFormat();
    ms_mutex_unlock();
    return (0 == status ? true : false);
}


static bool usbms_prevmedia_removal_nand(bool enable)
{
    return true;
}


static bool usbms_read_capacity_nand(uint32_t *max_lba, uint32_t *sec_len)
{
    uint32_t status;
    DEVICE_INFO_T device_info;

    ms_mutex_lock();
    status = spi_nand_GetDiskGeometry(&device_info);
    *max_lba = device_info.sectors - 1;
    *sec_len = device_info.sector_size;
    ms_mutex_unlock();

    return true;
}


static usbms_dev_type_t usbms_read_dev_type_nand(void)
{
    return (USB_STORAGE_DEV_NAND);
}


usbms_msdc_driver_t usbms_spinand_driver = {
    usbms_read_nand,
    usbms_write_nand,
    usbms_format_nand,
    usbms_checkmedia_exist_nand,
    usbms_prevmedia_removal_nand,
    usbms_read_capacity_nand,
    usbms_read_dev_type_nand,
};
#endif


