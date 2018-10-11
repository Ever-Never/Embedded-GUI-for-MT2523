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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_platform.h"
#include <memory_attribute.h>
#include <hal_emi_internal.h>

#ifndef MTK_NO_PSRAM_ENABLE

ATTR_RWDATA_IN_TCM EMI_SETTINGS emi_settings[] = {
    {
        0x00000010,     /*EMI_GENA_VAL*/
        0x00000000,     /*EMI_RDCT_VAL*/
        0x00004000,     /*EMI_DSRAM_VAL*/
        0xA0261000,     /*EMI_MSRAM_VAL*/
        0x00000000,     /*EMI_IDL_C_VAL*/
        0x00000000,     /*EMI_IDL_D_VAL*/
        0x00000000,     /*EMI_IDL_E_VAL*/
        0x00000000,     /*EMI_ODL_C_VAL*/
        0x00000000,     /*EMI_ODL_D_VAL*/
        0x00000000,     /*EMI_ODL_E_VAL*/
        0x00000000,     /*EMI_ODL_F_VAL*/
        0x00020002,     /*EMI_IO_A_VAL*/
        0x00020002,     /*EMI_IO_B_VAL*/

    } ,
};

#define PSRAM_BASE 0x0

ATTR_RWDATA_IN_TCM uint32_t __EMI_CurSR0 = 0x0;
ATTR_RWDATA_IN_TCM uint32_t __EMI_CurSR1 = 0x0;
ATTR_RWDATA_IN_TCM uint32_t __EMI_CurSR2 = 0x0;
ATTR_ZIDATA_IN_TCM uint32_t EMI_GENA_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_RDCT_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_DSRAM_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_MSRAM_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_IDL_C_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_IDL_D_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_IDL_E_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_ODL_C_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_ODL_D_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_ODL_E_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_ODL_F_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_IO_A_VAL;
ATTR_ZIDATA_IN_TCM uint32_t EMI_IO_B_VAL;

#define CM4_BOOT_FROM_SLV    ((volatile uint32_t *)(0xA2110008))
ATTR_RWDATA_IN_TCM EMI_REGISTER_T *emi_register = (EMI_REGISTER_T *)EMI_BASE;
ATTR_ZIDATA_IN_TCM EMI_REGISTER_T emi_register_backup;


ATTR_TEXT_IN_TCM uint32_t __EMI_GetSR(uint32_t bank_no)
{
    uint32_t value;

    emi_register->EMI_MREG_RW = bank_no << EMI_MREG_BANK_OFFSET ; //set bank
    emi_register->EMI_MREG_RW = EMI_MRGE_EDGE_TRIGGER_MASK | (emi_register->EMI_MREG_RW);  //trigger start

    while ((emi_register->EMI_MREG_RW & EMI_MRGE_ACC_IDLE_MASK) == 0); //wait read data ready

    value = ((emi_register->EMI_MREG_RW & EMI_MREG_RDATA_MASK) >> EMI_MREG_RDATA_OFFSET);

    return value;
}


ATTR_TEXT_IN_TCM void __EMI_SetSR(uint32_t bank_no, uint32_t value)
{
#ifndef MTK_NO_PSRAM_ENABLE
    value = value & EMI_MR_DATA_MASK;
    emi_register->EMI_MREG_RW = (value << EMI_MRGE_WDATA_OFFSET) | (bank_no << EMI_MREG_BANK_OFFSET) | (1 << EMI_MRGE_W_OFFSET) ; //set write data¡Bbank and write access
    emi_register->EMI_MREG_RW = EMI_MRGE_EDGE_TRIGGER_MASK | (emi_register->EMI_MREG_RW);  //trigger start

    while ((emi_register->EMI_MREG_RW & EMI_MRGE_ACC_IDLE_MASK) == 0); //wait write data ready
#endif
    return ;
}

ATTR_TEXT_IN_TCM void __EMI_SetRegValfromCMCP(void)
{
#ifndef MTK_NO_PSRAM_ENABLE
    EMI_GENA_VAL = emi_settings[0].EMI_GENA_VAL;
    EMI_RDCT_VAL =  emi_settings[0].EMI_RDCT_VAL;
    EMI_DSRAM_VAL = emi_settings[0].EMI_DSRAM_VAL;
    EMI_MSRAM_VAL = emi_settings[0].EMI_MSRAM_VAL;
    EMI_IDL_C_VAL = emi_settings[0].EMI_IDL_C_VAL;
    EMI_IDL_D_VAL = emi_settings[0].EMI_IDL_D_VAL;
    EMI_IDL_E_VAL = emi_settings[0].EMI_IDL_E_VAL;

    EMI_ODL_C_VAL = emi_settings[0].EMI_ODL_C_VAL;
    EMI_ODL_D_VAL = emi_settings[0].EMI_ODL_D_VAL;
    EMI_ODL_E_VAL = emi_settings[0].EMI_ODL_E_VAL;
    EMI_ODL_F_VAL = emi_settings[0].EMI_ODL_F_VAL;

    EMI_IO_A_VAL = emi_settings[0].EMI_IO_A_VAL;
    EMI_IO_B_VAL = emi_settings[0].EMI_IO_B_VAL;
#endif
    return;

}

#define EMI_ARB_A_VAL                           0x00005431   // ARM  port, filter-length=1024 (max=4096)
#define EMI_ARB_B_VAL                           0x00005009   // DMA port
#define EMI_ARB_C_VAL                           0x00005046   // MM
ATTR_TEXT_IN_TCM int __EMI_EnableBandwidthLimiter(void)
{

    /* Set ARM port BW*/
    emi_register->EMI_ARBA = EMI_ARB_A_VAL;

    /* Set DMA port BW */
    emi_register->EMI_ARBB = EMI_ARB_B_VAL;

    /* Set GMC port BW */
    emi_register->EMI_ARBC = EMI_ARB_C_VAL;

    return 0;

}


ATTR_TEXT_IN_TCM int32_t TestCase_MBIST(void)
{
    uint32_t mbist_src, mbist_len, mbist_data[2] = {0x5AA5, 0xFFFF};
    uint32_t i, bist_data_inv,  bist_data_random;

    mbist_src = PSRAM_BASE;
    mbist_len = 0x4000; //16k Bytes

    emi_register->EMI_MBISTA = 0x0;
    for (bist_data_inv = 0; bist_data_inv <= 1; bist_data_inv++) {
        for (i = 0; i <= 1; i++) {
            for (bist_data_random = 0; bist_data_random <= 1; bist_data_random++) {
                // need to reset mbist everytime
                emi_register->EMI_MBISTA = 0x0;
                // config mbist source address and test length
                emi_register->EMI_MBISTB = ((mbist_src >> 8) << EMI_BIST_STR_ADDR_OFFSET) | ((mbist_len >> 8) - 1);
                // config mbist data pattern, data inverse, burst length and width
                emi_register->EMI_MBISTA = (mbist_data[i] << EMI_BIST_BG_DATA_OFFSET) | (bist_data_inv << EMI_BIST_DATA_INV_OFFSET)  | (bist_data_random << EMI_BIST_DATA_RANDOM_OFFSET);
                emi_register->EMI_MBISTA |= 0x325; // enable MBIST, write and read mode, 4 byte trans size, burst length = 16 byte
                // check if mbist finished
                while ((emi_register->EMI_MBISTD & EMI_BIST_END_MASK) == 0x0);
                // check mbist result
                if ((emi_register->EMI_MBISTD & EMI_BIST_FAIL_MASK) != 0) {
                    emi_register->EMI_MBISTA = 0x0;
                    return -1;
                }
            }
        }
    }

    emi_register->EMI_MBISTA = 0x0;
    return 0;
}
ATTR_TEXT_IN_TCM int32_t __EMI_DataAutoTrackingMbistTest(void)
{
    if (TestCase_MBIST() != 0) {
        return -1;
    }

    return 0;
}

#define DATA_TUNING_STEP 1

ATTR_TEXT_IN_TCM void __EMI_EnableDataAutoTracking(void)
{
    int32_t dqy_in_del = 0x1F;
    int32_t dqs_in_del = 0;

    emi_register->EMI_DLLV = 0x0;
    emi_register->EMI_IDLC = 0x0;
    emi_register->EMI_IDLD = 0x0;
    emi_register->EMI_IDLE = 0x0;

    for (dqy_in_del = 0x1F; dqy_in_del >= 0; dqy_in_del -= DATA_TUNING_STEP) {
        emi_register->EMI_IDLC = dqy_in_del << EMI_DQ7_IN_DEL_OFFSET | dqy_in_del << EMI_DQ6_IN_DEL_OFFSET | dqy_in_del << EMI_DQ5_IN_DEL_OFFSET | dqy_in_del;
        emi_register->EMI_IDLD = dqy_in_del << EMI_DQ3_IN_DEL_OFFSET | dqy_in_del << EMI_DQ2_IN_DEL_OFFSET | dqy_in_del << EMI_DQ1_IN_DEL_OFFSET | dqy_in_del;

        if (0 ==  __EMI_DataAutoTrackingMbistTest()) {
            break;
        }
    }

    if (dqy_in_del < 0) { //DQ_IN_DELAY can't find window
        for (dqs_in_del = 0x1; dqs_in_del <= 0x1F; dqs_in_del += DATA_TUNING_STEP) {
            emi_register->EMI_IDLE = dqs_in_del << EMI_DQS0_IN_DEL_OFFSET ;

            if (0 ==  __EMI_DataAutoTrackingMbistTest()) {
                break;
            }
        }
    }
    return;
}
#if 0
//#if (PRODUCT_VERSION == 2523S)
#ifdef MT2523S
uint32_t query_psram_is_1M_size(void)
{
/*
    MR2<2:0> Density
    -------------------
    001 	 32M
    011 	 16M
    101 	 8M
    111 	 4M
*/
    if( (__EMI_GetSR(2) & 0x07) == 0x3 )
        return 1;
     else
        return 0;
}
#endif
#endif


ATTR_TEXT_IN_TCM void EMI_PowerOn_Init(void)
{
    /* boot slave default Enable and Make EMI_base + 0 and  EMI_base + 4 mapping to
    boot vector. we need to disable it to make EMI_base map to real PSRAM*/
    *(CM4_BOOT_FROM_SLV) = 0x0;

    emi_register->EMI_HFSLP = EMI_HFSLP_EXIT_REQ_MASK;
    while ((emi_register->EMI_HFSLP & EMI_HFSLP_EXIT_ACK_MASK) >> EMI_HFSLP_EXIT_ACK_OFFSET == 0);
    emi_register->EMI_HFSLP = 0x0;
    while ((emi_register->EMI_HFSLP & EMI_HFSLP_EXIT_ACK_MASK) >> EMI_HFSLP_EXIT_ACK_OFFSET == 1);

    *((volatile uint32_t *)(PSRAM_BASE)) = 0x5a5a5a5a;
    *((volatile uint32_t *)(PSRAM_BASE + 0x4)) = 0x5a5a5a5a;

    while ((emi_register->EMI_MREG_RW & EMI_MRGE_ACC_IDLE_MASK) == 0);

    __EMI_SetSR(0x0, 0xf);
    #ifdef MT2523S
       __EMI_SetSR(0x4, 0xa);
    #else
       __EMI_SetSR(0x4, 0x8);
    #endif
    while ((emi_register->EMI_MREG_RW & EMI_MRGE_ACC_IDLE_MASK) == 0);

    #if (PRODUCT_VERSION == 2523)
    if( *((volatile uint32_t *)0xA2000008) == 0x2523 ){
        if((*((volatile uint32_t *)0xA2060184) & 0x0f) == 0x2){
            *((volatile uint32_t *)0xA0800044) = (*((volatile uint32_t *)0xA0800044) &(~0x7f)) | 0x1;
        }
        else{
            *((volatile uint32_t *)0xA0800044) = (*((volatile uint32_t *)0xA0800044) &(~0x7f)) | 0x7;
        }
    }
    #endif

}


#endif


ATTR_TEXT_IN_TCM int8_t custom_setEMI(void)
{
#ifndef MTK_NO_PSRAM_ENABLE

    EMI_PowerOn_Init();

    __EMI_SetRegValfromCMCP();

    __EMI_CurSR0 = __EMI_GetSR(0);
    __EMI_CurSR1 = __EMI_GetSR(1);
    __EMI_CurSR2 = __EMI_GetSR(2);
#endif

    return 0;
}



ATTR_TEXT_IN_TCM int8_t custom_setAdvEMI(void)
{
#ifndef MTK_NO_PSRAM_ENABLE
    /**
        * Switch the EMI register into corresponding modes.
        */

    emi_register->EMI_IOA = EMI_IO_A_VAL;
    emi_register->EMI_IOB = EMI_IO_B_VAL;

    emi_register->EMI_GENA = EMI_GENA_VAL;

    emi_register->EMI_RDCT = EMI_RDCT_VAL;

    emi_register->EMI_DSRAM = EMI_DSRAM_VAL;
    emi_register->EMI_MSRAM = EMI_MSRAM_VAL;

    /**
      * set driving & delay
      */
    emi_register->EMI_IDLC = EMI_IDL_C_VAL;
    emi_register->EMI_IDLD = EMI_IDL_D_VAL;
    emi_register->EMI_IDLE = EMI_IDL_E_VAL;

    emi_register->EMI_ODLC = EMI_ODL_C_VAL;
    emi_register->EMI_ODLD = EMI_ODL_D_VAL;
    emi_register->EMI_ODLE = EMI_ODL_E_VAL;
    emi_register->EMI_ODLF = EMI_ODL_F_VAL;

    __EMI_EnableDataAutoTracking();
    /**
        * Enable 1/5 DLL
        */
    emi_register->EMI_DLLV = 0x300f0000; //dll_cal_init = 1
    while (((emi_register->EMI_DLLV & EMI_CAL_DONE_MASK) >> EMI_CAL_DONE_OFFSET) != 1);  //wait_cal_done = 1
    emi_register->EMI_DLLV = 0x700f0000; // enable dll soft update
    while (((emi_register->EMI_DLLV & EMI_CAL_DONE_MASK) >> EMI_CAL_DONE_OFFSET) != 0);  //wait_cal_done = 0
    emi_register->EMI_DLLV = 0x100f0000; //initialization finish

    __EMI_EnableBandwidthLimiter();
#endif
    return 0;
}



ATTR_TEXT_IN_TCM void EMI_Setting_Save(void)
{
#ifndef MTK_NO_PSRAM_ENABLE
    emi_register_backup.EMI_CONM = emi_register->EMI_CONM;
    emi_register_backup.EMI_GENA = emi_register->EMI_GENA;
    emi_register_backup.EMI_RDCT = emi_register->EMI_RDCT;
    emi_register_backup.EMI_IDLC = emi_register->EMI_IDLC;
    emi_register_backup.EMI_IDLD = emi_register->EMI_IDLD;
    emi_register_backup.EMI_IDLE = emi_register->EMI_IDLE;
    emi_register_backup.EMI_ODLC = emi_register->EMI_ODLC;
    emi_register_backup.EMI_ODLD = emi_register->EMI_ODLD;
    emi_register_backup.EMI_ODLE = emi_register->EMI_ODLE;
    emi_register_backup.EMI_ODLF = emi_register->EMI_ODLF;
    emi_register_backup.EMI_IOA  = emi_register->EMI_IOA;
    emi_register_backup.EMI_IOB  = emi_register->EMI_IOB;
    emi_register_backup.EMI_DSRAM = emi_register->EMI_DSRAM;
    emi_register_backup.EMI_MSRAM = emi_register->EMI_MSRAM;
    emi_register_backup.EMI_ARBA = emi_register->EMI_ARBA;
    emi_register_backup.EMI_ARBB = emi_register->EMI_ARBB;
    emi_register_backup.EMI_ARBC = emi_register->EMI_ARBC;
    emi_register_backup.EMI_SLCT = emi_register->EMI_SLCT;
    emi_register_backup.EMI_ABCT = emi_register->EMI_ABCT;
    emi_register_backup.EMI_BMEN = emi_register->EMI_BMEN;
#endif
}

ATTR_TEXT_IN_TCM void EMI_Setting_restore(void)
{
#ifndef MTK_NO_PSRAM_ENABLE
    while ((emi_register->EMI_CONM & (EMI_IDLE_MASK | EMI_SRAM_IDLE_MASK)) != 0x11);
    emi_register->EMI_BMEN = emi_register_backup.EMI_BMEN;
    emi_register->EMI_ABCT = emi_register_backup.EMI_ABCT;
    emi_register->EMI_SLCT = emi_register_backup.EMI_SLCT;
    emi_register->EMI_ARBC = emi_register_backup.EMI_ARBC;
    emi_register->EMI_ARBB = emi_register_backup.EMI_ARBB;
    emi_register->EMI_ARBA = emi_register_backup.EMI_ARBA;
    emi_register->EMI_MSRAM = emi_register_backup.EMI_MSRAM;
    emi_register->EMI_DSRAM = emi_register_backup.EMI_DSRAM;
    emi_register->EMI_IOB  = emi_register_backup.EMI_IOB;
    emi_register->EMI_IOA  = emi_register_backup.EMI_IOA;
    emi_register->EMI_ODLF = emi_register_backup.EMI_ODLF;
    emi_register->EMI_ODLE = emi_register_backup.EMI_ODLE;
    emi_register->EMI_ODLD = emi_register_backup.EMI_ODLD;
    emi_register->EMI_ODLC = emi_register_backup.EMI_ODLC;
    emi_register->EMI_IDLE = emi_register_backup.EMI_IDLE;
    emi_register->EMI_IDLD = emi_register_backup.EMI_IDLD;
    emi_register->EMI_IDLC = emi_register_backup.EMI_IDLC;
    emi_register->EMI_RDCT = emi_register_backup.EMI_RDCT;
    emi_register->EMI_GENA = emi_register_backup.EMI_GENA;
#endif
}

ATTR_TEXT_IN_TCM void mtk_psram_half_sleep_exit(void)
{
#ifndef MTK_NO_PSRAM_ENABLE
    emi_register->EMI_IOB = emi_register->EMI_IOB & 0xFFDFFFDF;
    /* TINFO=" ----- exit half sleep mode begin ----- " */
    //exit half sleep
    emi_register->EMI_HFSLP = 0x1;
    while ((emi_register->EMI_HFSLP & EMI_HFSLP_EXIT_ACK_MASK) >> EMI_HFSLP_EXIT_ACK_OFFSET == 0);
    emi_register->EMI_HFSLP = 0x0;
    while ((emi_register->EMI_HFSLP & EMI_HFSLP_EXIT_ACK_MASK) >> EMI_HFSLP_EXIT_ACK_OFFSET == 1);

    EMI_Setting_restore();
    emi_register->EMI_DLLV = 0x300f0000; //300f0000 for real chip
    while (((emi_register->EMI_DLLV & EMI_CAL_DONE_MASK) >> EMI_CAL_DONE_OFFSET) != 1);
    emi_register->EMI_DLLV = 0x700f0000;
    while (((emi_register->EMI_DLLV & EMI_CAL_DONE_MASK) >> EMI_CAL_DONE_OFFSET) == 1);
    emi_register->EMI_DLLV = 0x100f0000;

    emi_register->EMI_CONM = emi_register_backup.EMI_CONM;
    /* TINFO=" ----- exit half sleep mode end ----- " */
#endif

}

ATTR_TEXT_IN_TCM void mtk_psram_half_sleep_enter(void)
{
#ifndef MTK_NO_PSRAM_ENABLE
    EMI_Setting_Save();
    /* TINFO= " ----- enter half sleep mode begin ----- " */
    //enter half sleep
    while ((emi_register->EMI_CONM & (EMI_IDLE_MASK | EMI_SRAM_IDLE_MASK)) != 0x11);
    emi_register->EMI_CONM = (emi_register->EMI_CONM | 0x00000700);
    emi_register->EMI_MREG_RW = 0xf0670000;
    while ((emi_register->EMI_MREG_RW & EMI_MRGE_ACC_IDLE_MASK) == 0);
    emi_register->EMI_DLLV = 0x00000000;
    while ((emi_register->EMI_HFSLP & EMI_HFSLP_ENT_STA_MASK) >> EMI_HFSLP_ENT_STA_OFFSET == 0);
    /* TINFO= " ----- enter half sleep mode end  ----- " */
    emi_register->EMI_IOB = emi_register->EMI_IOB | 0x200020;
#endif

}

ATTR_TEXT_IN_TCM int32_t EMI_DynamicClockSwitch(emi_clock clock)
{
#ifdef MTK_NO_PSRAM_ENABLE
    return -1;
#else
    volatile uint32_t delay;
    if (clock == EMI_CLK_LOW_TO_HIGH) {
//             while((emi_register->EMI_CONM & 0x01) == 0);   //wait EMI idle
//             emi_register->EMI_CONM = emi_register->EMI_CONM | 0x700;   //mask request

        /*
        * Enable 1/5 DLL
        */
        emi_register->EMI_DLLV = 0x0;
        emi_register->EMI_DLLV = 0x300f0000; //dll_cal_init = 1
        while (((emi_register->EMI_DLLV & EMI_CAL_DONE_MASK) >> EMI_CAL_DONE_OFFSET) != 1); //wait_cal_done = 1
        emi_register->EMI_DLLV = 0x700f0000; // enable dll soft update
        while (((emi_register->EMI_DLLV & EMI_CAL_DONE_MASK) >> EMI_CAL_DONE_OFFSET) != 0); //wait_cal_done = 0
        emi_register->EMI_DLLV = 0x100f0000; //initialization finish

        for (delay = 0; delay < 0xFF; delay++);

        //          emi_register->EMI_CONM = emi_register->EMI_CONM & (~0x700);   //unmask request
    } else if (clock == EMI_CLK_HIGH_TO_LOW) {
        return 0;
    } else {
        return -1;
    }

    return 0;
#endif
}

ATTR_RWDATA_IN_TCM volatile uint32_t dvfs_dma_runing_status = 0x0;

ATTR_TEXT_IN_TCM void emi_mask_master(void)
{
#ifndef MTK_NO_PSRAM_ENABLE
    emi_register->EMI_CONM = emi_register->EMI_CONM | EMI_REQ_MASK_MASK;   //mask request
#endif
}

ATTR_TEXT_IN_TCM void emi_unmask_master(void)
{
#ifndef MTK_NO_PSRAM_ENABLE
    emi_register->EMI_CONM = emi_register->EMI_CONM & (~EMI_REQ_MASK_MASK);   //unmask request
#endif

}


