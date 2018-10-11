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

#include "hal_usb.h"
#include "hal_usb_internal.h"
#include "hal_eint.h"
#include "hal_log.h"
#include "hal_gpt.h"

#ifdef HAL_USB_MODULE_ENABLED

static volatile USBPHY_REGISTER_T *musb_phy = (USBPHY_REGISTER_T *)USB_SIFSLV_BASE;

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
    //Release force suspendm. ?³ (force_suspendm=0) (let suspendm=1, enable usb 480MHz pll)
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_suspendm);
    //Wait 800 usec
    hal_gpt_delay_ms(1);
}


void hal_usbphy_save_current(void)
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
    hal_gpt_delay_ms(2);
    //save current
    USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_DPPULLDOWN);
    USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_DMPULLDOWN);
    USB_DRV_WriteReg32(&musb_phy->u2phydtm0, (USB_DRV_Reg32(&musb_phy->u2phydtm0) & (~(0x3 << U2PHYDTM0_RG_XCVRSEL_1_0_OFST))) | U2PHYDTM0_RG_XCVRSEL_1_0);
    USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_TERMSEL);
    USB_DRV_WriteReg32(&musb_phy->u2phydtm0, USB_DRV_Reg32(&musb_phy->u2phydtm0) & (~(0xF << U2PHYDTM0_RG_DATAIN_3_0_OFST)));
    USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_dp_pulldown);
    USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_dm_pulldown);
    USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_xcvrsel);
    USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_termsel);
    USB_DRV_SetBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_datain);
    //DP/DM BC1.1 path Disable
    USB_DRV_ClearBits32(&musb_phy->u2phyacr3, U2PHYACR3_RG_USB20_PHY_REV_7);
    //OTG Disable
    USB_DRV_ClearBits32(&musb_phy->u2phyacr2, U2PHYACR2_RG_USB20_OTG_VBUSCMP_EN);
    //wait 800us
    hal_gpt_delay_ms(1);
    //(let suspendm=0, set utmi into analog power down )
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_SUSPENDM);
    //wait 1us
    hal_gpt_delay_ms(1);
    //Turn off internal 48Mhz PLL if there is no other hardware module is using  the 48Mhz clock -the control register is in clock document
    //@@@@@@@@@@@
    //A2D LVSH ENABLE
    USB_DRV_ClearBits32(&musb_phy->u2phyac0, U2PHYAC0_RG_USB_LVSH_EN);
    //Turn off AVDD33_USB(PHY 3.3v power) ?the control register is in PMIC document
    //@@@@@@@@@@@

}


void hal_usbphy_recover(void)
{
    //A2D LVSH ENABLE
    USB_DRV_SetBits32(&musb_phy->u2phyac0, U2PHYAC0_RG_USB_LVSH_EN);
    //switch to USB function. (system register, force ip into usb mode)
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_uart_en);
    USB_DRV_ClearBits32(&musb_phy->u2phydtm1, U2PHYDTM1_RG_UART_EN);
    USB_DRV_ClearBits32(&musb_phy->u2phyacr4, U2PHYACR4_rg_usb20_gpio_ctl);
    USB_DRV_ClearBits32(&musb_phy->u2phyacr4, U2PHYACR4_usb20_gpio_mode);
    //Release force suspendm. ? (force_suspendm=0) (let suspendm=1, enable usb 480MHz pll)
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_suspendm);
    //recover
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_DPPULLDOWN);
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_DMPULLDOWN);
    USB_DRV_WriteReg32(&musb_phy->u2phydtm0, USB_DRV_Reg32(&musb_phy->u2phydtm0) & (~(0x3 << U2PHYDTM0_RG_XCVRSEL_1_0_OFST)));
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_RG_TERMSEL);
    USB_DRV_WriteReg32(&musb_phy->u2phydtm0, USB_DRV_Reg32(&musb_phy->u2phydtm0) & (~(0xF << U2PHYDTM0_RG_DATAIN_3_0_OFST)));
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_dp_pulldown);
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_dm_pulldown);
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_xcvrsel);
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_termsel);
    USB_DRV_ClearBits32(&musb_phy->u2phydtm0, U2PHYDTM0_force_datain);
    //DP/DM BC1.1 path Disable
    USB_DRV_ClearBits32(&musb_phy->u2phyacr3, U2PHYACR3_RG_USB20_PHY_REV_7);
    //OTG Enable
    USB_DRV_SetBits32(&musb_phy->u2phyacr2, U2PHYACR2_RG_USB20_OTG_VBUSCMP_EN);
    //Wait 800 usec
    hal_gpt_delay_ms(1);
    USB_DRV_WriteReg32(&musb_phy->u2phydtm1, 0x00003E2E);
}



#define USB_HS_SLEW_RATE_CAL_TIME_WINDOW 0x400
#define USB_HS_SLEW_RATE_CAL_A 32
#define USB_HS_SLEW_RATE_CAL_FRA (1000)

void hal_usbphy_slew_rate_calibration(void)
{
    double freq, x;
    uint32_t data = 0;
    uint32_t timeout = 0;
    uint32_t b = 0;

    /*printf("\r\n\r\n\r\n\r\n\r\n");*/
    /*printf("hal_usbphy_slew_rate_calibration - before 0x%x = 0x%.8x\r\n", &musb_phy->u2phyacr0, USB_DRV_Reg32(&musb_phy->u2phyacr0));*/

    //enable USB ring oscillator
    USB_DRV_SetBits32(&musb_phy->u2phyacr0, U2PHYACR0_RG_USB20_HSTX_SRCAL_EN);
    hal_gpt_delay_ms(1);
    //Enable free run clock
    USB_DRV_SetBits32(&musb_phy->fmmonr1, USB_FMMONR1_RG_FRCK_EN);
    //RG_MONCLK_SEL = 00, 60MHz clock
    USB_DRV_WriteReg32(&musb_phy->fmcr0, (1 << 24) | (USB_HS_SLEW_RATE_CAL_TIME_WINDOW));

    //check USB_FM_VLD
    //while ((USB_DRV_Reg32(&musb_phy->fmmonr1) & 0x1) != 0x1) {
    hal_gpt_delay_ms(5);
    while ((USB_DRV_Reg8(&musb_phy->fmmonr1)) != 0x1) {
        timeout++;
        if (timeout == 500) {
            log_hal_error("USB slew rate calibration timeout\n");
            break;
        }
    }

    data = USB_DRV_Reg32(&musb_phy->fmmonr0);
    freq = (double)((48 * USB_HS_SLEW_RATE_CAL_TIME_WINDOW) / (double)data);
    x = USB_HS_SLEW_RATE_CAL_A * freq / USB_HS_SLEW_RATE_CAL_FRA;
    b = (int)(x + (double)0.5);

    /*printf("hal_usbphy_slew_rate_calibration - data 0x%x\r\n", data);*/
    /*printf("hal_usbphy_slew_rate_calibration - 48* (%d/%d) * 0.0%d = %d\r\n",
    						USB_HS_SLEW_RATE_CAL_TIME_WINDOW,
    						data,
    						USB_HS_SLEW_RATE_CAL_A,
    						b);*/
    if (b > 0x7) {
        b = 0x07;
        log_hal_error("USB slew rate calibration overflow - fix value to 0x07\r\n");
    }

    //Disable free run clock
    USB_DRV_WriteReg32(&musb_phy->fmcr0, USB_DRV_Reg32(&musb_phy->fmcr0) & 0xFEFFFFFF);
    USB_DRV_ClearBits32(&musb_phy->fmmonr1, USB_FMMONR1_RG_FRCK_EN);
    USB_DRV_WriteReg32(&musb_phy->u2phyacr0, (USB_DRV_Reg32(&musb_phy->u2phyacr0) & ~U2PHYACR0_RG_USB20_HSTX_SRCTRL_CLR) | ((b & 0x7) << 16));
    //Disable USB ring oscillator
    USB_DRV_ClearBits32(&musb_phy->u2phyacr0, U2PHYACR0_RG_USB20_HSTX_SRCAL_EN);
    /*printf("hal_usbphy_slew_rate_calibration - after 0x%x = 0x%.8x\r\n", &musb_phy->u2phyacr0, USB_DRV_Reg32(&musb_phy->u2phyacr0));*/
    /*printf("\r\n\r\n\r\n\r\n\r\n");*/
}


#endif /*HAL_USB_MODULE_ENABLED*/
