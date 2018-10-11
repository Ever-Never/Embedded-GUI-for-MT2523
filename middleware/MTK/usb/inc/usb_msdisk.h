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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *    usb_msdisk.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This file is intends for usb mass storage disk related information.
 *
 * Author:
 * -------
 *  Jensen Hu
 *
 ****************************************************************************/
#ifndef USB_MSDISK_H
#define USB_MSDISK_H

#include "kal_data_types.h"

#define LOGIC_DRIVE_COUNT             1
#define LOGIC_DRIVE_MS                0
#define LOGIC_DRIVE_CDROM             1

typedef enum 
{
	USB_STORAGE_DEV_STATUS_OK = 0,
	USB_STORAGE_DEV_STATUS_MEDIA_CHANGE,
	USB_STORAGE_DEV_STATUS_NOMEDIA,
	USB_STORAGE_DEV_STATUS_WP,
	USB_STORAGE_DEV_STATUS_MAX
} USB_STORAGE_DEV_STATUS;


typedef enum
{
	USB_STORAGE_DEV_NOR,
	USB_STORAGE_DEV_NAND,
	USB_STORAGE_DEV_CARD,
	USB_STORAGE_DEV_CDROM,
	USB_STORAGE_DEV_NONE
}USB_STORAGE_DEV_TYPE;

//FS_GetDevType Return Value
//FS_GetDevStatus Paramter, Cannot overlap with 'A'~'Z' 0x41~0x5f
typedef enum
{
    FS_DEVICE_TYPE_UNKNOWN  = 0,
    FS_DEVICE_TYPE_NOR      = 1,
    FS_DEVICE_TYPE_NAND,
    FS_DEVICE_TYPE_CARD,
    FS_DEVICE_TYPE_EXTERNAL,
    FS_DEVICE_TYPE_SIMPLUS,
    FS_DEVICE_TYPE_EMMC,
    FS_DEVICE_TYPE_MAX_VALUE
} FS_DEVICE_TYPE_ENUM;


typedef struct
{
	bool (* usbms_read)(void *data, uint32_t LBA, uint16_t sec_len);
	bool (* usbms_write)(void *data, uint32_t LBA, uint16_t sec_len);
	bool (* usbms_format)(void);
	USB_STORAGE_DEV_STATUS (* usbms_checkmedia_exist)(void);
	bool (* usbms_prevmedia_removal)(bool enable);
	bool (* usbms_read_capacity)(uint32_t *max_lba, uint32_t *sec_len);
//	bool (* usbms_read_formatcapacity)(uint32_t *no_of_blks, uint32_t *blk_len);
	USB_STORAGE_DEV_TYPE	(* usbms_dev_type)(void);
}USB_DiskDriver_STRUCT;


#if ( (defined(__MSDC_MS__)) || (defined(__MSDC_SD_MMC__)) || (defined(__MSDC_MSPRO__)) || (defined(__MSDC2_SD_MMC__))  )
extern USB_DiskDriver_STRUCT USB_MSDC_drv;
#endif

//uint8_t usbms_register_diskdriver(USB_DiskDriver_STRUCT *disk_api);
extern uint8_t USB_Ms_Register_DiskDriver(USB_DiskDriver_STRUCT *disk_api);
extern uint8_t USB_Ms_Set_Current_Driver(uint32_t drv);
extern uint32_t USB_Ms_Get_Current_Driver(void);

#endif /* USB_MSDISK_H */                    

