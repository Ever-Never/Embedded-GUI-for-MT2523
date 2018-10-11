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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal_cache.h"
#include "hal_pmu.h"
#include "hal_sleep_manager.h"
#include "usbacm_drv.h"
#include "usb_resource.h"

static bool usb_initial = false;
static uint8_t usb_sleep_handler = 0;

#define BOOT_TIMEOUT_CNT 103000

bool is_vusb_ready(void)
{
	bool ret;
	// Fill protection key
	pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR ,PMU_RG_VRC_CFG_KEY0_MASK ,PMU_RG_VRC_CFG_KEY0_SHIFT, 0xFC);
	pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR ,PMU_RG_VRC_CFG_KEY1_MASK ,PMU_RG_VRC_CFG_KEY1_SHIFT, 0xDA);

	ret = pmu_get_register_value(PMU_RGS_CHRDET_ADDR, PMU_RGS_CHRDET_MASK, PMU_RGS_CHRDET_SHIFT);

	// Clear protection key
	pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR, PMU_RG_VRC_CFG_KEY0_MASK, PMU_RG_VRC_CFG_KEY0_SHIFT  , 0x0);
	pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR, PMU_RG_VRC_CFG_KEY1_MASK, PMU_RG_VRC_CFG_KEY1_SHIFT  , 0x0);

	return ret;
}

const USB_DEVICE_PARAM *USB_GetDeviceParam(void);
const USB_ACM_PARAM *USB_GetAcmParam(void);

bool usb_init(void)
{
	if (usb_initial == true)
		return false;

	usb_initial = true;

	/*LOG_I(hal,"++++++++++ usb lcok sleep\n");*/
	hal_sleep_manager_lock_sleep(usb_sleep_handler);
	hal_usb_init();

	///* Get customize parameters */
	gUsbDevice.device_param = USB_GetDeviceParam();
	g_UsbACM_Comm.acm_param = USB_GetAcmParam();

	USB_Register_CreateFunc("ACM COMMU1",
				USB_Acm_CommIf_Create,
				USB_Acm_CommIf_Reset,
				USB_Acm_CommIf_Enable,
				USB_Acm_CommIf_Speed_Reset,
				NULL);

	USB_Register_CreateFunc("ACM DATA1",
				USB_Acm_DataIf_Create,
				USB_Acm_DataIf_Reset,
				USB_Acm_DataIf_Enable,
				USB_Acm_DataIf_Speed_Reset,
				NULL);

	USB_Register_Device_Code(USB_ACM_DEVICE_CODE,
													USB_ACM_SUBCLASS_CODE,
													USB_ACM_PROTOCOL_CODE,
													0x0003/*g_UsbACM_Comm.acm_param->desc_product*/);

	USB_Init(USB_CDC_ACM);

	return true;
}

bool usb_de_init(void)
{
	if (usb_initial == false)
		return false;

	usb_initial = false;

	USB_Release_Type();

	hal_sleep_manager_unlock_sleep(usb_sleep_handler);
	/*LOG_I(hal,"--------- usb unlcok sleep\n");*/

	return true;
}
void usb_cable_detect(void)
{
	if (usb_sleep_handler == 0)
		usb_sleep_handler = hal_sleep_manager_set_sleep_handle("USBDemo");

	if (is_vusb_ready()) {
		/*LOG_I(hal, "USB init\n");*/
		usb_init();
		printf("usb_init()\n\r");
	} else {
		/*LOG_I(hal, "USB de-init\n");*/
		usb_de_init();
	}
}

extern void usb_hisr(void);

void usb_boot_init(void)
{
	int i=0;

	if (is_vusb_ready())
	{
		usb_init();
		do
		{
			usb_hisr();
			if(i++ >= BOOT_TIMEOUT_CNT)
				break;
		}while((USB_Get_Device_State() != DEVSTATE_CONFIG));
		printf("USBCOM init, hisr cnt%d\n\r", i);
	}

}
