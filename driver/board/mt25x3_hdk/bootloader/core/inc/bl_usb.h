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

#ifndef BL_USB_H_
#define BL_USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DRV_WriteReg(addr,data)     ((*(volatile unsigned short *)(addr)) = (unsigned short)(data))
#define DRV_Reg(addr)               (*(volatile unsigned short *)(addr))
#define DRV_WriteReg32(addr,data)     ((*(volatile unsigned int *)(addr)) = (unsigned int)(data))
#define DRV_Reg32(addr)               (*(volatile unsigned int *)(addr))
#define DRV_WriteReg8(addr,data)     ((*(volatile unsigned char *)(addr)) = (unsigned char)(data))
#define DRV_Reg8(addr)               (*(volatile unsigned char *)(addr))
#define USB_DRV_WriteReg(addr,data)     ((*(volatile unsigned short *)(addr)) = (unsigned short)(data))
#define USB_DRV_Reg(addr)               (*(volatile unsigned short *)(addr))
#define USB_DRV_WriteReg32(addr,data)     ((*(volatile unsigned int *)(addr)) = (unsigned int)(data))
#define USB_DRV_Reg32(addr)               (*(volatile unsigned int *)(addr))
#define USB_DRV_WriteReg8(addr,data)     ((*(volatile unsigned char *)(addr)) = (unsigned char)(data))
#define USB_DRV_Reg8(addr)               (*(volatile unsigned char *)(addr))

#define USB_DRV_ClearBits(addr,data)     {\
   unsigned short temp;\
   temp = DRV_Reg(addr);\
   temp &=~(data);\
   DRV_WriteReg(addr,temp);\
}

#define USB_DRV_SetBits(addr,data)     {\
   unsigned short temp;\
   temp = DRV_Reg(addr);\
   temp |= (data);\
   DRV_WriteReg(addr,temp);\
}

#define USB_DRV_SetData(addr, bitmask, value)     {\
   unsigned short temp;\
   temp = (~(bitmask)) & DRV_Reg(addr);\
   temp |= ((value) & (bitmask));\
   DRV_WriteReg(addr,temp);\
}

#define USB_DRV_ClearBits32(addr,data)     {\
   unsigned int temp;\
   temp = DRV_Reg32(addr);\
   temp &=~(data);\
   DRV_WriteReg32(addr,temp);\
}

#define USB_DRV_SetBits32(addr,data)     {\
   unsigned int temp;\
   temp = DRV_Reg32(addr);\
   temp |= (data);\
   DRV_WriteReg32(addr,temp);\
}

#define USB_DRV_SetData32(addr, bitmask, value)     {\
   unsigned int temp;\
   temp = (~(bitmask)) & DRV_Reg32(addr);\
   temp |= ((value) & (bitmask));\
   DRV_WriteReg32(addr,temp);\
}

#define USB_DRV_ClearBits8(addr,data)     {\
   unsigned char temp;\
   temp = DRV_Reg8(addr);\
   temp &=~(data);\
   DRV_WriteReg8(addr,temp);\
}

#define USB_DRV_SetBits8(addr,data)     {\
   unsigned char temp;\
   temp = DRV_Reg8(addr);\
   temp |= (data);\
   DRV_WriteReg8(addr,temp);\
}

#define USB_DRV_SetData8(addr, bitmask, value)     {\
   unsigned char temp;\
   temp = (~(bitmask)) & DRV_Reg8(addr);\
   temp |= ((value) & (bitmask));\
   DRV_WriteReg8(addr,temp);\
}	



#define U2PHYAC0_RG_USB_LVSH_EN		(1U<<31)
#define	U2PHYDTM0_force_uart_en		(1<<26)
#define	U2PHYDTM1_RG_UART_EN		(1<<16)
#define	U2PHYACR4_rg_usb20_gpio_ctl	(1<<9)
#define	U2PHYACR4_usb20_gpio_mode	(1<<8)
#define	U2PHYACR3_RG_USB20_PHY_REV_7	(1<<7)
#define	U2PHYACR4_RG_USB20_DP_100K_EN	(1<<16)
#define	U2PHYACR4_RG_USB20_DM_100K_EN	(1<<17)
#define	U2PHYACR2_RG_USB20_OTG_VBUSCMP_EN	(1<<27)
#define	U2PHYDTM0_force_suspendm	(1<<18)
#define	U2PHYDTM0_RG_SUSPENDM		(1<<3)
#define	U2PHYDTM0_RG_DPPULLDOWN		(1<<6)
#define	U2PHYDTM0_RG_DMPULLDOWN		(1<<7)
#define	U2PHYDTM0_RG_XCVRSEL_1_0	(1<<4)
#define	U2PHYDTM0_RG_XCVRSEL_1_0_OFST	(4)
#define	U2PHYDTM0_RG_TERMSEL		(1<<2)
#define	U2PHYDTM0_RG_DATAIN_3_0		(1<<10)
#define	U2PHYDTM0_RG_DATAIN_3_0_OFST		(10)
#define	U2PHYDTM0_force_dp_pulldown	(1<<20)
#define	U2PHYDTM0_force_dm_pulldown	(1<<21)
#define	U2PHYDTM0_force_xcvrsel		(1<<19)
#define	U2PHYDTM0_force_termsel		(1<<17)
#define	U2PHYDTM0_force_datain		(1<<23)

#define	U2PHYACR0_RG_USB20_HSTX_SRCAL_EN (1<<23)
#define	U2PHYACR0_RG_USB20_HSTX_SRCTRL_2_0 (1<16)
#define U2PHYACR0_RG_USB20_HSTX_SRCTRL_CLR      (0x7<<16)
#define USB_FMMONR1_RG_FRCK_EN (1<<8)
typedef struct {
	/*0x00*/
	unsigned int	revered[512];	//
	unsigned int	u2phyac0;     //0x800
	unsigned int	u2phyac1;     //0x804
	unsigned int	u2phyac2;     //0x808
	unsigned int	revered1;      //
	unsigned int	u2phyacr0;    //0x810
	unsigned int	u2phyacr1;    //0x814
	unsigned int	u2phyacr2;    //0x818
	unsigned int	u2phyacr3;    //0x81c
	unsigned int	u2phyacr4;    //0x820
	unsigned int	revered2[15];  //
	unsigned int	u2phydcr0;    //0x860
	unsigned int	u2phydcr1;    //0x864
	unsigned int	u2phydtm0;    //0x868
	unsigned int	u2phydtm1;    //0x86c
	unsigned int	u2phydmon0;   //0x870
	unsigned int	u2phydmon1;   //0x874
	unsigned int	u2phydmon2;   //0x878
	unsigned int	revered3[17];  //
	unsigned int	u1phycr0;     //0x8c0
	unsigned int	u1phycr1;     //0x8c4
	unsigned int	u1phycr2;     //0x8c8
	unsigned int	revered4[5];   //
	unsigned int	regfppc;      //0x8e0
	unsigned int	revered5[3];   //
	unsigned int	versionc;     //0x8f0
	unsigned int	revered6[2];   //
	unsigned int	regfcom;			//0x8fc
	unsigned int	revered7[384];
	unsigned int	fmcr0;				//0xf00
	unsigned int	fmcr1;				//0xf04
	unsigned int	fmcr2;				//0xf08
	unsigned int	fmmonr0;			//0xf0c
	unsigned int	fmmonr1;			//0xf10
} USBPHY_REGISTER_T;

void bl_usb_low_power_setting(void);

#ifdef __cplusplus
}
#endif

#endif /* BL_USB_H_ */
