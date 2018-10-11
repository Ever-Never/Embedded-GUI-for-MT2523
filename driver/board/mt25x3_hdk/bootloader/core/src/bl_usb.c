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

#include <bl_usb.h>
#include <bl_gpt.h>
#include "bl_pmic.h"
#include <bl_clock.h>
#include "mt2523.h"
#include "bl_common.h"

#define PDN_USB           0x0004
#define PDN_USB_BUS_CG    0x2000
#define PDN_USB_DMA_CG    0x1000

static volatile USBPHY_REGISTER_T *musb_phy = (USBPHY_REGISTER_T*)USB_SIFSLV_BASE;

void hal_usbphy_poweron_initialize(void)
{
	//A2D LVSH ENABLE
	USB_DRV_SetBits32(&musb_phy->u2phyac0, U2PHYAC0_RG_USB_LVSH_EN);
	//switch to USB function. (system register, force ip into usb mode)
	USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_uart_en);
	USB_DRV_ClearBits32(&musb_phy->u2phydtm1, U2PHYDTM1_RG_UART_EN);
	USB_DRV_ClearBits32(&musb_phy->u2phyacr4, U2PHYACR4_rg_usb20_gpio_ctl);
	USB_DRV_ClearBits32(&musb_phy->u2phyacr4, U2PHYACR4_usb20_gpio_mode);
	//DP/DM BC1.1 path Disable
	USB_DRV_ClearBits32(&musb_phy->u2phyacr3, U2PHYACR3_RG_USB20_PHY_REV_7);
	//dp_100k disable
	//dm_100k disable
	USB_DRV_ClearBits32(&musb_phy->u2phyacr4, U2PHYACR4_RG_USB20_DP_100K_EN);
	USB_DRV_ClearBits32(&musb_phy->u2phyacr4, U2PHYACR4_RG_USB20_DM_100K_EN);
	//OTG Enable
	USB_DRV_SetBits32(&musb_phy->u2phyacr2, U2PHYACR2_RG_USB20_OTG_VBUSCMP_EN);
	//Release force suspendm. „³ (force_suspendm=0) (let suspendm=1, enable usb 480MHz pll)
	USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_suspendm);
	//Wait 800 usec
	bl_gpt_delay_ms(1);
}


void hal_usbphy_save_current()
{
	//switch to USB function. (system register, force ip into usb mode)
	USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_uart_en);
	USB_DRV_ClearBits32(&musb_phy->u2phydtm1, U2PHYDTM1_RG_UART_EN);
	USB_DRV_ClearBits32(&musb_phy->u2phyacr4, U2PHYACR4_rg_usb20_gpio_ctl);
	USB_DRV_ClearBits32(&musb_phy->u2phyacr4, U2PHYACR4_usb20_gpio_mode);
	//(let suspendm=1, enable usb 480MHz pll)
	USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_SUSPENDM);
	//(force_suspendm=1)
	USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_suspendm);
	//Wait 2 ms. Wait USBPLL stable.
	bl_gpt_delay_ms(2);
	//save current
	USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_DPPULLDOWN);
	USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_DMPULLDOWN);
	USB_DRV_WriteReg32(&musb_phy->u2phydtm0, (USB_DRV_Reg32(&musb_phy->u2phydtm0)&(~(0x3<<U2PHYDTM0_RG_XCVRSEL_1_0_OFST)))|U2PHYDTM0_RG_XCVRSEL_1_0);
	USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_TERMSEL);
	USB_DRV_WriteReg32(&musb_phy->u2phydtm0, USB_DRV_Reg32(&musb_phy->u2phydtm0)&(~(0xF<<U2PHYDTM0_RG_DATAIN_3_0_OFST)));
	USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_dp_pulldown);
	USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_dm_pulldown);
	USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_xcvrsel);
	USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_termsel);
	USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_datain);
	//DP/DM BC1.1 path enable
	USB_DRV_SetBits32(&musb_phy->u2phyacr3, U2PHYACR3_RG_USB20_PHY_REV_7);
	//OTG Disable
	USB_DRV_ClearBits32(&musb_phy->u2phyacr2, U2PHYACR2_RG_USB20_OTG_VBUSCMP_EN);
	//wait 800us
	bl_gpt_delay_ms(1);
	//(let suspendm=0, set utmi into analog power down )
	USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_SUSPENDM);
	//wait 1us
	bl_gpt_delay_ms(1);
	//A2D LVSH ENABLE
	USB_DRV_ClearBits32(&musb_phy->u2phyac0, U2PHYAC0_RG_USB_LVSH_EN);
}

void bl_usb_low_power_setting(void)
{
    volatile unsigned int pll_rg = *BL_CLK_CONDD;

    /*enable clock, VUSB is default on*/
    bl_print(LOG_DEBUG, "bl_usb_low_power_setting start\n\r");
    USB_DRV_SetBits(0xa20107a0, 0x01); /*usbsif_rstb bit*/
    *BL_CLK_CONDD = 0x10000fff;
    *BL_PDN_CLRD1 = PDN_USB;
    *BL_PDN_CLRD2 = PDN_USB_BUS_CG | PDN_USB_DMA_CG;

    /*usb save current mode*/
    hal_usbphy_poweron_initialize();
    hal_usbphy_save_current();

    /*disable clock and VUSB*/
    USB_DRV_ClearBits(0xa20107a0, 0x01); /*usbsif_rstb bit*/
    *BL_CLK_CONDD = pll_rg;
    *BL_PDN_SETD1 = PDN_USB;
    *BL_PDN_SETD2 = PDN_USB_BUS_CG | PDN_USB_DMA_CG;
	bl_pmic_vusb_vr_control(0);	//disable VUSB LDO;
}
