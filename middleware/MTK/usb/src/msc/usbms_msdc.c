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
#include "hal_msdc.h"
#include "usbms_msdc.h"
#include "msdc_custom_config.h"


#define USBMS_MSDC_PORT ((hal_sd_port_t)MASS_STORAGE_USE_MSDC_PORT_NUMBER)
#define USBMS_MSDC_BUSWIDTH ((hal_sd_bus_width_t)MASS_STORAGE_MSDC_BUS_WIDTH)
#define USBMS_MSDC_BUS_CLOCK MASS_STORAGE_MSDC_BUS_CLOCK /*KHz*/

hal_sd_config_t sd_config = {USBMS_MSDC_BUSWIDTH, USBMS_MSDC_BUS_CLOCK};


static bool usbms_read_msdc(void *data, uint32_t LBA, uint16_t sec_len)
{
    hal_sd_status_t status;
    uint32_t retry = 0;

    while (retry < 3) {
        status = hal_sd_read_blocks(USBMS_MSDC_PORT, data, LBA, sec_len);
        if (HAL_SD_STATUS_OK == status) {
            return true;
        } else {
            status = hal_sd_init(USBMS_MSDC_PORT, &sd_config);
            if (HAL_SD_STATUS_OK == status) {
                retry++;
            } else {
                return false;
            }
        }
    }

    return false;
}

static bool usbms_write_msdc(void *data, uint32_t LBA, uint16_t sec_len)
{
    hal_sd_status_t status;
    uint32_t retry = 0;

    while (retry < 3) {
        status = hal_sd_write_blocks(USBMS_MSDC_PORT, data, LBA, sec_len);
        if (HAL_SD_STATUS_OK == status) {
            return true;
        } else {
            status = hal_sd_init(USBMS_MSDC_PORT, &sd_config);
            if (HAL_SD_STATUS_OK == status) {
                retry++;
            } else {
                return false;
            }
        }
    }

    return false;
}


static bool usbms_checkmedia_exist_msdc(void)
{
    return (true); //didn't support hot plug
}


static bool usbms_format_msdc(void)
{
    hal_sd_status_t status;

    status = hal_sd_init(USBMS_MSDC_PORT, &sd_config);

    return (HAL_SD_STATUS_OK == status ? true : false);
}


static bool usbms_prevmedia_removal_msdc(bool enable)
{
    return true;
}


static bool usbms_read_capacity_msdc(uint32_t *max_lba, uint32_t *sec_len)
{
    hal_sd_status_t status;
    uint64_t card_capacity;

    status = hal_sd_get_capacity(USBMS_MSDC_PORT, &card_capacity);

    if (HAL_SD_STATUS_OK != status) {
        status = hal_sd_init(USBMS_MSDC_PORT, &sd_config);
        if (HAL_SD_STATUS_OK == status) {
            status = hal_sd_get_capacity(USBMS_MSDC_PORT, &card_capacity);
            if (HAL_SD_STATUS_OK != status) {
                return false;
            }
        } else {
            return false;
        }
    }

    *max_lba = (card_capacity / 512) - 1;
    *sec_len = 512;


    return true;
}


static usbms_dev_type_t usbms_read_dev_type_msdc(void)
{
    return (USB_STORAGE_DEV_CARD);
}


usbms_msdc_driver_t usbms_msdc_driver = {
    usbms_read_msdc,
    usbms_write_msdc,
    usbms_format_msdc,
    usbms_checkmedia_exist_msdc,
    usbms_prevmedia_removal_msdc,
    usbms_read_capacity_msdc,
    usbms_read_dev_type_msdc,
};








