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

#include "pmic_mt6327.h"
#include "external_pmic_driver.h"
#include "hal_i2c_pmic.h"
#include "hal_i2c_master.h"
#include "hal_i2c_master_internal.h"
#include "hal_log.h"

void pmic_charger_restore_setup(void)
{
   

}

unsigned char pmic_charger_get_charging_status(void)
{
    unsigned char chg_status=0;
    

    return chg_status;
}

unsigned char pmic_charger_get_charger_exist(void)
{
    unsigned char chg_exist=0;
    

    return chg_exist;
}

void pmic_init_mt6327(void)
{
    unsigned char chipid;
	uint8_t status=0;
	uint8_t count=0;

    chipid = external_pmic_get_register_value(MT6327_DEVICE_ID, 0, 0xFF, 0);

    if (chipid == MT6327_CHIP_ID) {
        log_hal_info("Use MT6327 PMIC\r\n");
		 

		// actual can't disable,why???
		status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x57,0x01,0,0x00);count++;// Set VMD is controled by register,not SRCLKEN
//		status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x53,0x01,0,0x00);count++;//disable VMD ,default enable
		//status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x56,0x1f,0,0x14);count++;//adjust VMD voltage===1.2v   (0.75V~1.525V)

		// actual can't disable,why???
		status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x63,0x02,0,0x00);count++;// Set VRF18 is controled by register,not SRCLKEN
//		status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x63,0x01,0,0x00);count++;//disable VRF18,default disable
		//status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x65,0x1f,0,0x0e);count++;//adjust VRF18 voltage===1.8v   (1.45V~2.25V)

		// actual can't disable,why???
		//status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x82,0x01,0,0x00);count++;//disable V18,default enable

		// actual can't disable,why???
//		status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x84,0x01,0,0x00);count++;//disable VSIM,default disable
		//status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x84,0x10,0,0x01);count++;//adjust VSIM voltage===3.0v   (1.8V  3.0V)

		
		//status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x88,0x01,0,0x00);count++;//disable VMC,default enable
		status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x87,0x70,0,0x40);count++;//adjust VMC voltage===2.8v   (1.3V 1.5V 1.8V 2.5V 2.8V 3.0V 3.1V)

		
		//status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0x8a,0x31,0,0x21);count++;//default is disable and 1.2v, adjust VM voltage===1.8v   (1.2V 1.5V 1.8V),enable
		

		status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0xa1,0x02,0,0x00);count++;// Set VTCXO is controled by register,not SRCLKEN
//		status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0xa1,0x01,0,0x00);count++;//disable VTCXO,default enable    2.8V

		
//		status+=external_pmic_set_register_value(MT6327_DEVICE_ID,0xa4,0x01,0,0x00);count++;//disable VA,default enable   2.5V

		if(status!=count){
			log_hal_info("pmic_init_mt6327 Set Param Error\r\n");
		}
    } else {
        log_hal_info("Not Found MT6327 PMIC Chip ID\r\n");
        return;
    }
}

external_pmic_status_t pmic_ctrl_power_mt6327(external_pmic_mt6327_vr_t power_name, external_pmic_vr_ctl_t enable)
{
   	switch(power_name){
	case EXT_PMIC_MT6327_VMD:
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x53,0x01,0,enable);
		break;
	case EXT_PMIC_MT6327_VRF18:
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x63,0x01,0,enable);
		break;
	case EXT_PMIC_MT6327_VTCXO:
		external_pmic_set_register_value(MT6327_DEVICE_ID,0xa1,0x01,0,enable);
		break;
	case EXT_PMIC_MT6327_VA25:
		external_pmic_set_register_value(MT6327_DEVICE_ID,0xa4,0x01,0,enable);
		break;
	case EXT_PMIC_MT6327_VM:
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x8a,0x01,0,enable);
		break;
	case EXT_PMIC_MT6327_VSIM1:
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x84,0x01,0,enable);
		break;
	case EXT_PMIC_MT6327_VIO18:
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x82,0x01,0,enable);
		break;
	case EXT_PMIC_MT6327_VMC:
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x88,0x01,0,enable);
		break;
	}
    return (EXT_PMIC_STATUS_SUCCESS);
}

external_pmic_status_t pmic_vosel_config_mt6327(external_pmic_mt6327_vr_t power_name, external_pmic_voltage_t voltage)
{
    external_pmic_status_t status = EXT_PMIC_STATUS_SUCCESS;
    unsigned char vosel=0;
	

    switch(power_name){
	case EXT_PMIC_MT6327_VMD:
		if(voltage>EXT_PMIC_VOL_1P3V){
			voltage=EXT_PMIC_VOL_1P3V;
		}
		vosel=voltage*4;
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x56,0x1f,0,vosel);
		break;
	case EXT_PMIC_MT6327_VRF18:
		if(voltage<EXT_PMIC_VOL_1P5V){
			voltage=EXT_PMIC_VOL_1P5V;
		}else if(voltage>EXT_PMIC_VOL_2P0V){
			voltage=EXT_PMIC_VOL_2P0V;
		}
		vosel=2+(voltage-8)*4;
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x65,0x1f,0,vosel);
		break;
	case EXT_PMIC_MT6327_VTCXO:
		break;
	case EXT_PMIC_MT6327_VA25:
		break;
	case EXT_PMIC_MT6327_VM:
		if(voltage==EXT_PMIC_VOL_1P8V){
			vosel=0x00;
		}else if(voltage==EXT_PMIC_VOL_3P0V){
			vosel=0x01;
		}
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x8a,0x30,0,vosel);
		break;
	case EXT_PMIC_MT6327_VSIM1:
		if(voltage==EXT_PMIC_VOL_1P2V){
			vosel=0x00;
		}else if(voltage==EXT_PMIC_VOL_1P5V){
			vosel=0x01;
		}
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x84,0x10,0,vosel);
		break;
	case EXT_PMIC_MT6327_VIO18:
		break;
	case EXT_PMIC_MT6327_VMC:
		if(voltage==EXT_PMIC_VOL_2P8V){
			vosel=0x04;
		}else if(voltage==EXT_PMIC_VOL_3P0V){
			vosel=0x05;
		}else if(voltage==EXT_PMIC_VOL_3P1V){
			vosel=0x06;
		}
		external_pmic_set_register_value(MT6327_DEVICE_ID,0x87,0x70,0,vosel);
		break;
	}
    return (status);
}
