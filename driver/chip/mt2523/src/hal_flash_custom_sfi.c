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

#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED
#include "memory_attribute.h"
#include "hal_flash_custom_memorydevice.h"
#include "mt2523.h"
#include "hal_flash_sf.h"
#include "hal_flash_opt.h"
#include "hal_flash_combo_init.h"
#include "hal_flash_combo_sfi_defs.h"
#include "hal_flash_sfi_release.h"  // Get __SERIAL_FLASH_INIT_IN_EXTBOOTLOADER__ definition

#include "hal_flash_custom_sfi.h"


/*************************************************************************
* FUNCTION
*  custom_setSFI()
*
* DESCRIPTION
*   This routine aims to set SFI
*
* PARAMETERS
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/

//-----------------------------------------------------------------------------
// MCP Serial Flash EMI/SFI settings
//-----------------------------------------------------------------------------

// include type defines (to be modified by EMI/SFI owner)
ATTR_RODATA_IN_RAM
#define COMBO_MEM_HW_INST_NAME combo_mem_hw_list
#define COMBO_MEM_HW_TYPE_MODIFIER  static const
#include "hal_flash_combo_sfi_config.h"

/* SFC Driving registers */
#define SFIO_CFG0_DRV       ((volatile unsigned int *)(CONFIG_BASE+0x600))
#define SFIO_CFG1_DRV       ((volatile unsigned int *)(CONFIG_BASE+0x604))
#define SFIO_CFG2_DRV       ((volatile unsigned int *)(CONFIG_BASE+0x608))

/* SFC GPIO registers */
#define SFIO_CFG0           (CONFIG_BASE + 0x600)
#define SFIO_CFG1           (CONFIG_BASE + 0x604)
#define SFIO_CFG2           (CONFIG_BASE + 0x608)

#define SF_NORMAL_HIGHZ     (0xFFCFFFCF)

/* SFC SLT registers */
#define SF_SLT_MODE         (0x00100000)
#define SYSTEM_INFOD        ((volatile unsigned int *)(CONFIG_BASE+0x0224))

#define GPIO_RESEN0_1       (GPIO_BASE + 0xb10)
#define GPIO_RESEN1_1       (GPIO_BASE + 0xb30)
#define SF_SLT_HIGHZ        (0xFFFFF03F)

#define GPIO_DRV1           (GPIO_BASE + 0x820)
#define SF_SLT_DRIVING_CLR  (0xFF000FFF)
#define SF_SLT_DRIVING_12mA (0xFFAAAFFF)

int sfi_index, CS_COUNT_SFI;
#if !defined(__DUAL_SERIAL_FLASH_SUPPORT__)
ATTR_TEXT_IN_RAM signed char custom_setSFI(void)
{
    const CMEMEntrySFI *sfi;

    uint32_t tmp_driving;

    //Add GPIO, clock, pad switch configuration here in case only mini-bootloader init SF device
    /* GPIO high-Z Enable*/
    /* Normal MODE */
    SFI_WriteReg32(SFIO_CFG0, (SFI_ReadReg32(SFIO_CFG0) & (SF_NORMAL_HIGHZ)));
    SFI_WriteReg32(SFIO_CFG1, (SFI_ReadReg32(SFIO_CFG1) & (SF_NORMAL_HIGHZ)));
    SFI_WriteReg32(SFIO_CFG2, (SFI_ReadReg32(SFIO_CFG2) & (SF_NORMAL_HIGHZ)));
    /* SLT  MODE */
    if ((SFI_ReadReg32(SYSTEM_INFOD)&SF_SLT_MODE)) {
        SFI_WriteReg32(GPIO_RESEN0_1, (SFI_ReadReg32(GPIO_RESEN0_1) & (SF_SLT_HIGHZ)));
        SFI_WriteReg32(GPIO_RESEN1_1, (SFI_ReadReg32(GPIO_RESEN1_1) & (SF_SLT_HIGHZ)));
        SFI_WriteReg32(GPIO_DRV1, (SFI_ReadReg32(GPIO_DRV1) & (SF_SLT_DRIVING_CLR)) | SF_SLT_DRIVING_12mA);  //SLT driving 12mA
    }

    /*--------------------------------------------------------------------------
     * 1. Slow down SF_CLK to 26Mhz for META mode
     * At META mode, the PLL will be enabled
     * At non-META mode, bootrom will disable PLL
     *--------------------------------------------------------------------------*/



    sfi_index = CMEM_EMIINIT_Index(); // API CMCP_EMIINIT_Index() is defined in hal_flash_combo_init.h

    // if __COMBO_MCP_SUPPORT__ was not defined, CMEM_EMIINIT_Index() always retunrs 0.
    // if __COMBO_MCP_SUPPORT__ was defined, CMEM_EMIINIT_Index() returns the index to the sfi config structure.
    //    => if the returned index < 0, means the flash was not included in the mcp list

    if (sfi_index < 0)    {
        // Add error handler here
        while (1);
    }

    /*--------------------------------------------------------------------------
     * Step 1.
     * 1. Switch mode QPI/SPI Quad
     * 2. Set Burst/Wrap length
     *--------------------------------------------------------------------------*/
    sfi = &combo_mem_hw_list.List[sfi_index]; // the structure name "combo_mem_hw_list" can be renamed by SFI owner

#if defined(__SFI_CLK_78MHZ__) || defined(__SFI_CLK_80MHZ__)
    // Device Initialization
    SFI_Dev_Command_List(0, sfi->DevInit_78M);

    /*--------------------------------------------------------------------------
     * Step 2. Initialize Serial Flash Control Registers
     *--------------------------------------------------------------------------*/
    SFC->RW_SF_MAC_CTL = sfi->HWConf_78M[SFC_MAC_CTL];
    SFC->RW_SF_DIRECT_CTL = sfi->HWConf_78M[SFC_DR_CTL];

    SFC->RW_SF_MISC_CTL = sfi->HWConf_78M[SFC_MISC_CTL];
    SFC->RW_SF_MISC_CTL2 = sfi->HWConf_78M[SFC_MISC_CTL2];
    SFC->RW_SF_DLY_CTL2 = sfi->HWConf_78M[SFC_DLY_CTL2];
    SFC->RW_SF_DLY_CTL3 = sfi->HWConf_78M[SFC_DLY_CTL3];

    tmp_driving = *SFIO_CFG0_DRV & 0xFFF8FFF8;
    *SFIO_CFG0_DRV = tmp_driving | (sfi->HWConf_78M[SFC_Driving]);
    tmp_driving = *SFIO_CFG1_DRV & 0xFFF8FFF8;
    *SFIO_CFG1_DRV = tmp_driving | (sfi->HWConf_78M[SFC_Driving]);
    tmp_driving = *SFIO_CFG2_DRV & 0xFFF8FFF8;
    *SFIO_CFG2_DRV = tmp_driving | (sfi->HWConf_78M[SFC_Driving]);

#else
#error "Undefined BB chips of SFC 130MHz"
#endif /* __SFI_CLK_78MHZ__ */

    /*--------------------------------------------------------------------------
     * Step 3. EFuse Post process of IO driving/ Sample clk delay/ Input delay
     * Currently, only for MT6250 and MT6260
     *--------------------------------------------------------------------------*/

    return 0;

}

#else  // !defined(__DUAL_SERIAL_FLASH_SUPPORT__)

//No dual SF support for MT2523 currently

#endif // !defined(__DUAL_SERIAL_FLASH_SUPPORT__)

#define CLK_CONDB               ((volatile unsigned int *)(CONFIG_BASE+0x0104))
#define ACFG_CLK_UPDATE         ((volatile unsigned short *)(ANA_CFGSYS_BASE+0x0150))

#define RG_VCORE_VOSEL   ((volatile unsigned short *)(PMU_SD_base + 0x230))
#define RG_VSF_VOSEL     ((volatile unsigned short *)(PMU_SD_base + 0x1D0))


#include "hal_flash_custom_sfi_clock.h"

ATTR_TEXT_IN_RAM int custom_setSFIExt()
{
#if defined(__SERIAL_FLASH_STT_EN__)
// 6261 Vcore can not change from external re-work, for STT test, set Vcore from 1.3 to 1.175V
//    uint16_t VcoreVoltage =0;
//    VcoreVoltage = *RG_VCORE_VOSEL & 0xFE0F;
//    *RG_VCORE_VOSEL = VcoreVoltage | 0x0030; //1.175V

#if 0
    uint16_t VcoreVoltage = 0; //Disable Vcore and VSF, use external source
    uint16_t VsfVoltage = 0;
    VcoreVoltage = *RG_VCORE_VOSEL & 0xFBFE;
    *RG_VCORE_VOSEL = VcoreVoltage;

    VsfVoltage = *RG_VSF_VOSEL & 0xFBFE;
    *RG_VSF_VOSEL = VsfVoltage;
#endif

#endif //defined(__SERIAL_FLASH_STT_EN__)

    // init SFI & SF device (QPI / wrap ...etc)
    custom_setSFI();

#if !defined(__SERIAL_FLASH_STT_EN__) // Not set PLL for Basic STT test
// init SFI clock

#if defined(__SFI_CLK_78MHZ__)
#if 1 //HFOSC 78MHz
    *CLK_CONDB &= 0xFFFFC3FF;
    *CLK_CONDB |= 0x00001C00; //CLK_CONDB[13:10] = 7: HFOSC_DIV4_CK 312MHz/4 =  78MHz
    *ACFG_CLK_UPDATE |= 0x0008; //set ACFG_CLK_UPDATE bit[3] = 1
    while (*ACFG_CLK_UPDATE & 0x0008); //polling switch clock ready by: ACFG_CLK_UPDATE bit[3] = 0
#endif
    /* MPLL 78MHz */
#if 0
    *CLK_CONDB &= 0xFFFFC3FF;
    *CLK_CONDB |= 0x00001000; //CLK_CONDB[13:10] = 4: MPLL =  78MHz
    *ACFG_CLK_UPDATE |= 0x0008; //set ACFG_CLK_UPDATE bit[3] = 1
    while (*ACFG_CLK_UPDATE & 0x0008); //polling switch clock ready by: ACFG_CLK_UPDATE bit[3] = 0
#endif
#endif
#endif //!defined(__SERIAL_FLASH_STT_EN__)

    return 0;
}


#else   /* ! HAL_FLASH_MODULE_ENABLED*/

//#include "kal_release.h"
//#include "pll.h"
//#define ATTR_TEXT_IN_RAM    __attribute__ ((section(".ram_code")))
#include "memory_attribute.h"
ATTR_TEXT_IN_RAM signed char custom_setSFI(void)
{
    return 0;
}

ATTR_TEXT_IN_RAM int custom_setSFIExt()
{
    return 0;
}

#endif //#ifdef HAL_FLASH_MODULE_ENABLED
