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



#ifdef MTK_FATFS_ON_SERIAL_NAND
#define __NAND_FDM_50__
#define NAND_SUPPORT

#include "NAND_DAL.h"
#include "NAND_DAL_internal.h"
#include "NAND_DevConfig.h"

///TODO:: api & device define
#if 0
#if defined(FLASH_DISKDRV_DEBUG) && !defined(__MTK_TARGET__)
#include <assert.h>
#define ASSERT    assert
#define EXT_ASSERT(e, x, y, z)  assert(e)
#define NAND_LOGGING
#define kal_mem_set  memset
#define kal_mem_cpy memcpy
#define kal_mem_cmp memcmp
#define NAND_GPIO_ReturnMode(a) (user_def_cs_num)
extern kal_uint32 DummyNFI_Reg[1]; /*For simulator*/
extern  uint8_t user_def_cs_num;
#endif
#endif

//#if ( defined(NAND_SUPPORT)&&defined(__NAND_FDM_50__) && (defined(__NFI_VERSION2__)||defined (__NFI_VERSION3_1__)))
#if (defined(NAND_SUPPORT)&& defined(__NAND_FDM_50__))


/*single page cache support, usually apply to old type NAND*/

NAND_MTD_SUB_DRIVER GD_SUB_DRIVER = {
    single_Page_Program,    /*ProgramPage*/
    single_Page_Read,       /*ReadPage*/
    NULL,                   /*CopyPage*/
    single_Block_Erase,     /*EraseBlock*/
    ///TODO::: start?
    NULL,    //GD_twoPlane_Program, /*TwoPlaneProgram*/
    NULL,    //GD_twoDie_Program, /*TwoDieProgram*/
    NULL,    //GD_fourPlane_Program, /*FourPlaneProgram(same as Toshiba)*/
    NULL,    //GD_twoDie_twoPlane_Program, /*TwoDieTwoPlaneProgram*/
    ///TODO::: end?
    NULL, /*CacheProgram*/
    NULL, /*TwoPlaneCacheProgram*/
    NULL, /*TwoDieCacheProgram*/
    NULL, /*FourPlaneCacheProgram*/
    NULL, /*TwoDieTwoPlaneCacheProgram*/
    ///TODO::: start?
    NULL,    //GD_twoPlane_Erase, /*TwoPlaneErase*/
    NULL,    //GD_twoDie_Erase, /*TwoDieErase*/
    NULL,    //GD_fourPlane_Erase, /*FourPlaneErase (same as Toshiba)*/
    NULL,    //GD_twoDie_twoPlane_Erase, /*TwoDieTwoPlaneErase*/
    ///TODO::: start?
#ifdef __SECURITY_OTP__
    NAND_OTPAccess,
    NAND_OTPQueryLength,
#else // __SECURITY_OTP__
    NULL,
    NULL,
#endif // __SECURITY_OTP__
};


/*******************************************************************************
 *  Global variable
 *******************************************************************************/
//#define NFI_EVENT   0x1

flash_list Default_ID_Table = {
    simu_NAND_1/*DevName_enum*/,
    {'D', 'E', 'F', 'A', 'U', 'L', 'T', 0xFF}/*ID_info*/,
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}/*ID_valid_mask*/,
    AUTO_NAND_DETECT/*IOWidth*/,
    AUTO_NAND_DETECT/*address_cycle*/,
    AUTO_NAND_DETECT/*planeNum*/,
    AUTO_NAND_DETECT/*dieNum*/,
    AUTO_NAND_DETECT/*pageSize*/,
    AUTO_NAND_DETECT/*spareSize*/,
    AUTO_NAND_DETECT/*blockPage*/,
    AUTO_NAND_DETECT/*deviceSize (MByte)*/,
    AUTO_NAND_DETECT/*supportMPType*/,
    AUTO_NAND_DETECT/*supportMEType*/,
    AUTO_NAND_DETECT/*CopyBackType*/,
    (NAND_MTD_SUB_DRIVER *)AUTO_NAND_DETECT /*driverSet*/
};

const uint8_t ID_512_Mask[8] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t ID_2K_Mask[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0x7C, 0x00, 0x00, 0x00};

/*[Notice] NAND_ID_Table should sort by
1. manufacture ID group;
2 ID valid bite (from large -> small)*/

const flash_list NAND_ID_Table[] = {
    /*Simulation (FAKE NAND)*/
    /*ID_byte == 2*/
    /* Fake NAND for simulation(refer to K9F1208U0M) */
    {
        simu_NAND_0/*DevName_enum*/,
        {'0', '1', 'F', 'A', 'K', 'E', 0xFF, 0xFF}/*ID_info*/,
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00}/*ID_valid_mask*/,
        8/*IOWidth*/,
        4/*address_cycle*/,
        4/*planeNum*/,
        1/*dieNum*/,
        512/*pageSize*/,
        16/*spareSize*/,
        32/*blockPage*/,
        64/*deviceSize (MByte)*/,
        FOUR_PLANE_OP | PLANE_INTERLACED/*supportMPType*/,
        FOUR_PLANE_OP | PLANE_INTERLACED/*supportMEType*/,
        AUTO_NAND_DETECT/*CopyBackType*/,
        (NAND_MTD_SUB_DRIVER *)AUTO_NAND_DETECT /*driverSet*/
    },


    /*GD*/
    /*ID_byte == 3*/
    /* K9F1G08U0A */
    {
        GD_5F4GQ4UBYIG/*DevName_enum*/,
        {0xC8, 0xC4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}/*ID_info*/, ///TODO:: check if need   : [Y]
        {0xFF, 0xFF, 0x80, 0xFF, 0x7C/*0x7C*/, 0x00, 0x00, 0x00}/*ID_valid_mask*/, ///TODO:: check if needed  : [Y]
        AUTO_NAND_DETECT/*IOWidth*/,
        AUTO_NAND_DETECT/*address_cycle*/,
        AUTO_NAND_DETECT/*planeNum*/,
        AUTO_NAND_DETECT/*dieNum*/,
        4096/*pageSize*/,
        128/*spareSize*/,
        64/*blockPage*/,
        AUTO_NAND_DETECT/*deviceSize (MByte)*/,
        0/*supportMPType*/,
        0/*supportMEType*/,
        AUTO_NAND_DETECT/*CopyBackType*/,
        (NAND_MTD_SUB_DRIVER *)AUTO_NAND_DETECT /*driverSet*/
    },

    /*Last one*/
    {
        MAX_NAND_NAME/*DevName_enum*/,
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}/*ID_info*/,
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}/*ID_valid_mask*/,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        (NAND_MTD_SUB_DRIVER *)NULL
    }
};

#endif /*( defined(__NAND_FDM_50__) )*/
#endif
