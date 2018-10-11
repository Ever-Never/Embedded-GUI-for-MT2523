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

#include "bl_pmic.h"
#include "bl_mt6327.h" 

unsigned int bl_mt6327_init(void)
{
	unsigned char chip_id;
	/*Find pmic chip id*/
	/*read MT6327 pmic chip id = 0x27*/
	chip_id = bl_pmic_get_register_value(BL_PMIC_I2C_ADDR_MT6327,1,MT6327_CID0,0xFF,0);
	if(chip_id == 0x27)
	{
		/* Set Vcore Voltage to 1.3V*/
		//bl_mt6327_vcore_set_1p3v();
		/* Set VMC Voltage to 2.8V*/
		//bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x87,0x07,4,0x04);


		// actual can't disable,why???
		bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x57,0x01,0,0x00);// Set VMD is controled by register,not SRCLKEN
	//	bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x53,0x01,0,0x00);//disable VMD
		//bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x56,0x1f,0,0x14);//adjust VMD voltage===1.2v   (0.75V~1.525V)

		// actual can't disable,why???
		bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x63,0x02,0,0x00);// Set VRF18 is controled by register,not SRCLKEN
	//	bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x63,0x01,0,0x00);//disable VRF18
		//bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x65,0x1f,0,0x0e);//adjust VRF18 voltage===1.8v   (1.45V~1.25V)

		// actual can't disable,why???
		//bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x82,0x01,0,0x00);//disable V18

		// actual can't disable,why???
	//	bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x84,0x01,0,0x00);//disable VSIM
		//bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x84,0x10,0,0x01);//adjust VSIM voltage===3.0v   (1.8V  3.0V)

		
		//bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x88,0x01,0,0x00);//disable VMC
		bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x87,0x70,0,0x40);//adjust VMC voltage===2.8v   (1.3V 1.5V 1.8V 2.5V 2.8V 3.0V 3.1V)

		
		bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0x8a,0x31,0,0x21);//default is disable and 1.2v,adjust VM voltage===1.8v   (1.2V 1.5V 1.8V), enable VM
		

		bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0xa1,0x02,0,0x00);// Set VTCXO is controled by register,not SRCLKEN
	//	bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0xa1,0x01,0,0x00);//disable VTCXO

		
	//	bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,0xa4,0x01,0,0x00);//disable VA
		
		return (0);
	}	
	else{
		return (1);
	}
}

void bl_mt6327_vcore_set_1p3v(void)
{
    // psi_mode : OVR/HP/LP/S1
	//PMIC_VCORE_1P3V = 6,
   bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6327,1,MT6327_VMD_CON5,0x1F,0,0x1F);
}

void bl_mt6327_vusb_vr_control(unsigned char enable)
{
    
}

void bl_mt6327_vmc_vr_control(unsigned char enable)
{
    
}
