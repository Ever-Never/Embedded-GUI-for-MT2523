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
#include "bl_mt6385.h" 

unsigned int bl_mt6385_init(void)
{
	unsigned char chip_id;
	/*Find pmic chip id*/
	/*read MT6385 pmic chip id = 0x85*/
	chip_id = bl_pmic_get_register_value(BL_PMIC_I2C_ADDR_MT6385,2,1,0xFF,0);
	if(chip_id == 0x85)
	{
		/* latch power key */
		bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PPCCTL0	,0x01	,0  ,1);//RG_PWRHOLD
		/* Set Vcore Voltage to 1.3V*/
		bl_mt6385_vcore_set_1p3v();
		return (0);
	}	
	else{
		return (1);
	}
}

void bl_mt6385_vcore_set_1p3v(void)
{
    // psi_mode : OVR/HP/LP/S1
	//PMIC_VCORE_1P3V = 6,
    int vcbuck_voval[7]={2,4,6,8,10,12,14};
    int vc_ldo_vosel[7]={0,2,4,6, 8,10,12};

    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY0	,0xFF	,0	,0xFC);
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY1	,0xFF   ,0  ,0xDA);
	//PMU_RG_PSIOVR
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICTL		,0x01   ,0  ,0x1);  // switch to PSIOVR

    //case PMIC_PSI_HP:
	//RG_HP_VC_BUCK_VOVAL
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_VCBUCKPSI0	,0x0F   ,4  ,vcbuck_voval[6]);
	//RG_HP_VC_LDO_VOSEL
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_VCLDOPSI0	,0x0F   ,4  ,vc_ldo_vosel[6]);
	
    //case PMIC_PSI_LP:
	//RG_LP_VC_BUCK_VOVAL
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_VCBUCKPSI1	,0x0F   ,4  ,vcbuck_voval[6]);
	//RG_LP_VC_LDO_VOSEL
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_VCLDOPSI2 	,0x0F   ,4  ,vc_ldo_vosel[6]);

	//PMU_RG_PSIOVR
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICTL   	,0x01   ,0  ,0x0);  // switch to PSI Bus
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY0 	,0xFF   ,0  ,0x0);
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY1 	,0xFF   ,0  ,0x0);
}

void bl_mt6385_vusb_vr_control(unsigned char enable)
{
    int poll_stb_b=1;

    if(enable)  // polling STB status
        poll_stb_b = 0;

    // Fill protection key
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY0	,0xFF   ,0  ,0xFC);
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY1 	,0xFF   ,0  ,0xDA);
	
	//case PMIC_VUSB:
	//PMU_RG_VUSB_EN
	bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_VUSBCTL0	,0x01	,0  ,enable);
	while(poll_stb_b==0)
	{
		//PMU_RGS_VUSB_STBSTS
		poll_stb_b = bl_pmic_get_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_VUSBCTL0	,0x01	,6);
	}

    // Clear protection key
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY0	,0xFF	,0  ,0x0);
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY1	,0xFF	,0  ,0x0);
}

void bl_mt6385_vmc_vr_control(unsigned char enable)
{
    int poll_stb_b=1;

    if(enable)  // polling STB status
        poll_stb_b = 0;
        
    // Fill protection key
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY0	,0xFF   ,0  ,0xFC);
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY1 	,0xFF   ,0  ,0xDA);

    //PMU_RG_VMC_EN
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_VMCCTL0	    ,0x01	,0  ,enable);
    while (poll_stb_b == 0) {        
        poll_stb_b = bl_pmic_get_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_VMCCTL0	,0x01	,6);
    }       

    // Clear protection key
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY0	,0xFF	,0  ,0x0);
    bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_MT6385	,2	,MT6385_PSICKEY1	,0xFF	,0  ,0x0);
}
