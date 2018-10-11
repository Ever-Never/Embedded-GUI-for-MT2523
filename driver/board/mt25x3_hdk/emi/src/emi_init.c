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
#include <stdbool.h>
#include "sw_types.h"
#include <custom_emi.h>
#include <emi_hw_internal.h>
#include "bl_common.h"


#ifndef MTK_NO_PSRAM_ENABLE

#define PSRAM_BASE 0x0

uint32_t __EMI_CurSR0 = 0x0;
uint32_t __EMI_CurSR1 = 0x0;
uint32_t __EMI_CurSR2 = 0x0;
uint32_t EMI_GENA_VAL;
uint32_t EMI_RDCT_VAL;
uint32_t EMI_DSRAM_VAL;
uint32_t EMI_MSRAM_VAL;
uint32_t EMI_IDL_C_VAL;
uint32_t EMI_IDL_D_VAL;
uint32_t EMI_IDL_E_VAL;
uint32_t EMI_ODL_C_VAL;
uint32_t EMI_ODL_D_VAL;
uint32_t EMI_ODL_E_VAL;
uint32_t EMI_ODL_F_VAL;
uint32_t EMI_IO_A_VAL;
uint32_t EMI_IO_B_VAL;

uint32_t  __EMI_GetSR(uint32_t bank_no)
{
    uint32_t value;

    *EMI_MREG_RW = bank_no << 20 ; //set bank
    *EMI_MREG_RW = 0x00010000 | (*EMI_MREG_RW);  //trigger start

    while ((*EMI_MREG_RW & 0x01) == 0);	//wait read data ready

    value = ((*EMI_MREG_RW & 0xFF00) >> 8);

    return value;
}


void  __EMI_SetSR(uint32_t bank_no, uint32_t value)
{

    value = 	value & 0xFF;
    *EMI_MREG_RW = (value << 24) | (bank_no << 20) | (1 << 17) ; //set write data¡Bbank and write access
    *EMI_MREG_RW = 0x00010000 | (*EMI_MREG_RW);  //trigger start

    while ((*EMI_MREG_RW & 0x01) == 0);	//wait write data ready

    return ;
}


void __EMI_SetRegValfromCMCP(void)
{

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

    return;

}
void dump_EMI_Setting(void)
{
    bl_print(LOG_DEBUG, "Dump EMI Setting\n\r");
    bl_print(LOG_DEBUG, "EMI_CONM = %x\n\r", *EMI_CONM);
    bl_print(LOG_DEBUG, "EMI_GENA = %x\n\r", *EMI_GENA);
    bl_print(LOG_DEBUG, "EMI_RDCT = %x\n\r", *EMI_RDCT);
    bl_print(LOG_DEBUG, "EMI_DLLV = %x\n\r", *EMI_DLLV);
    bl_print(LOG_DEBUG, "EMI_IDLC = %x\n\r", *EMI_IDLC);
    bl_print(LOG_DEBUG, "EMI_IDLD = %x\n\r", *EMI_IDLD);
    bl_print(LOG_DEBUG, "EMI_IDLE = %x\n\r", *EMI_IDLE);
    bl_print(LOG_DEBUG, "EMI_ODLC = %x\n\r", *EMI_ODLC);
    bl_print(LOG_DEBUG, "EMI_ODLD = %x\n\r", *EMI_ODLD);
    bl_print(LOG_DEBUG, "EMI_ODLE = %x\n\r", *EMI_ODLE);
    bl_print(LOG_DEBUG, "EMI_ODLF = %x\n\r", *EMI_ODLF);
    bl_print(LOG_DEBUG, "EMI_IOA = %x\n\r", *EMI_IOA);
    bl_print(LOG_DEBUG, "EMI_IOB = %x\n\r", *EMI_IOB);
    bl_print(LOG_DEBUG, "EMI_DSRAM = %x\n\r", *EMI_DSRAM);
    bl_print(LOG_DEBUG, "EMI_MSRAM = %u\n\r", *EMI_MSRAM);
    bl_print(LOG_DEBUG, "EMI_MREG_RW = %x\n\r", *EMI_MREG_RW);
    bl_print(LOG_DEBUG, "MR0 = %x\n\r", __EMI_GetSR(0));
    bl_print(LOG_DEBUG, "MR1 = %x\n\r", __EMI_GetSR(1));
    bl_print(LOG_DEBUG, "MR2 = %x\n\r", __EMI_GetSR(2));
    bl_print(LOG_DEBUG, "MR4 = %x\n\r", __EMI_GetSR(4));
    bl_print(LOG_DEBUG, "MR6 = %x\n\r", __EMI_GetSR(6));
}

#if 0
//#if (PRODUCT_VERSION == 2523S)
//#ifdef MT2523S
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
//#endif
#endif

void EMI_PowerOn_Init(void)
{
    /* boot slave default Enable and Make EMI_base + 0 and  EMI_base + 4 mapping to
    boot vector. we need to disable it to make EMI_base map to real PSRAM*/
    *(CM4_BOOT_FROM_SLV) = 0x0;

    *EMI_HFSLP = 0x1;
    while ((*EMI_HFSLP & 0x2) >> 1 == 0);
    *EMI_HFSLP = 0x0;
    while ((*EMI_HFSLP & 0x2) >> 1 == 1);

    *((volatile uint32_t *)(PSRAM_BASE)) = 0x5a5a5a5a;
    *((volatile uint32_t *)(PSRAM_BASE + 0x4)) = 0x5a5a5a5a;

    while ((*EMI_MREG_RW & 0x01) == 0);

    *EMI_MREG_RW = 0xf030000; //Set PSRAM Latency and DS
    #ifdef MT2523S
        *EMI_MREG_RW = 0x0a430000; // for MT2523s only have 1M bytes
    #else
        *EMI_MREG_RW = 0x08430000;
    #endif
    while ((*EMI_MREG_RW & 0x01) == 0);


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


#define EMI_ARB_A_VAL                           0x00005431   // ARM  port, filter-length=1024 (max=4096)
#define EMI_ARB_B_VAL                           0x00005009   // DMA port
#define EMI_ARB_C_VAL                           0x00005046   // MM



int __EMI_EnableBandwidthLimiter(void)
{

    /* Set ARM port BW*/
    *EMI_ARBA = EMI_ARB_A_VAL;

    /* Set DMA port BW */
    *EMI_ARBB = EMI_ARB_B_VAL;

    /* Set GMC port BW */
    *EMI_ARBC = EMI_ARB_C_VAL;

    return 0;

}



int32_t TestCase_MBIST(void)
{
    uint32_t mbist_src, mbist_len, mbist_data[2] = {0x5AA5, 0xFFFF};
    uint32_t i, bist_data_inv,  bist_data_random;

    mbist_src = PSRAM_BASE;
    mbist_len = 0x4000; //16k Bytes

    *EMI_MBISTA = 0x0;
    for (bist_data_inv = 0; bist_data_inv <= 1; bist_data_inv++) {
        for (i = 0; i <= 1; i++) {
            for (bist_data_random = 0; bist_data_random <= 1; bist_data_random++) {
                // need to reset mbist everytime
                *EMI_MBISTA = 0x0;
                // config mbist source address and test length
                *EMI_MBISTB = ((mbist_src >> 8) << 16) | ((mbist_len >> 8) - 1);
                // config mbist data pattern, data inverse, burst length and width
                *EMI_MBISTA = (mbist_data[i] << 16) | (bist_data_inv << 12)  | (bist_data_random << 13);
                *EMI_MBISTA |= 0x325; // enable MBIST, write and read mode, 4 byte trans size, burst length = 16 byte
                // check if mbist finished
                while ((*EMI_MBISTD & 0x2) == 0x0);
                // check mbist result
                if ((*EMI_MBISTD & 0x1) != 0) {
                    *EMI_MBISTA = 0x0;
                    return -1;
                }
            }
        }
    }

    *EMI_MBISTA = 0x0;
    return 0;
}

int32_t __EMI_DataAutoTrackingMbistTest(void)
{
    if (TestCase_MBIST() != 0) {
        return -1;
    }

    return 0;
}

#define DATA_TUNING_STEP 1

void  __EMI_EnableDataAutoTracking(void)
{
    int32_t dqy_in_del = 0x1F;
    int32_t dqs_in_del = 0;

    *EMI_DLLV = 0x0;
    *EMI_IDLC = 0x0;
    *EMI_IDLD = 0x0;
    *EMI_IDLE = 0x0;

    for (dqy_in_del = 0x1F; dqy_in_del >= 0; dqy_in_del -= DATA_TUNING_STEP) {
        *EMI_IDLC = *EMI_IDLD =
                        dqy_in_del << 24 | dqy_in_del << 16 | dqy_in_del << 8 | dqy_in_del;

        if (0 ==  __EMI_DataAutoTrackingMbistTest()) {
            break;
        }
    }

    if (dqy_in_del < 0) { //DQ_IN_DELAY can't find window
        for (dqs_in_del = 0x1; dqs_in_del <= 0x1F; dqs_in_del += DATA_TUNING_STEP) {
            *EMI_IDLE = dqs_in_del << 16 ;

            if (0 ==  __EMI_DataAutoTrackingMbistTest()) {
                break;
            }
        }
    }
    return;
}

#endif

int8_t custom_setEMI(void)
{
#ifdef MTK_NO_PSRAM_ENABLE
    return 0;
#else


#if 0
    uint32_t vend_id = 0;
    uint32_t density = 0;
    uint32_t rev     = 0;
#endif
    EMI_PowerOn_Init();

    __EMI_SetRegValfromCMCP();

    __EMI_CurSR0 = __EMI_GetSR(0);
    __EMI_CurSR1 = __EMI_GetSR(1);
    __EMI_CurSR2 = __EMI_GetSR(2);

#if 0
    /*
    *Configure TX Driving to 2 if this DRAM Type is 64Mb from APTIC
    *MR1[6:5]=0x01, MR2[2:0]=0x001 means 64Mb
    *MR1[6:5]=0x00, MR2[2:0]=0x010 means 64Mb
    *MR1[6:5]=0x00, MR2[2:0]=0x001 means 32Mb
    *MR1[6:5]=0x00, MR2[2:0]=0x000 means 16Mb
    */
    vend_id = __EMI_CurSR1 & 0x1f;
    density = __EMI_CurSR2 & 0x7;
    rev     = (__EMI_CurSR1 >> 5) & 0x3;
    if (vend_id == 0xD && density == 0x1 && rev == 0x1) {
        EMI_IO_A_VAL = 0x00020002;
        EMI_IO_B_VAL = 0x00020002;
    }
#endif

    return 0;
#endif

}




extern bool memory_btmt_test(void);

int8_t custom_setAdvEMI(void)
{
    /**
        * Switch the EMI register into corresponding modes.
        */
    bool test_result;

#ifdef MTK_NO_PSRAM_ENABLE
    return 0;
#else


    *EMI_IOA = EMI_IO_A_VAL;
    *EMI_IOB = EMI_IO_B_VAL;

    *EMI_GENA = EMI_GENA_VAL;

    *EMI_RDCT = EMI_RDCT_VAL;

    *EMI_DSRAM = EMI_DSRAM_VAL;
    *EMI_MSRAM = EMI_MSRAM_VAL;

    /**
      * set driving & delay
      */
    *EMI_IDLC = EMI_IDL_C_VAL;
    *EMI_IDLD = EMI_IDL_D_VAL;
    *EMI_IDLE = EMI_IDL_E_VAL;

    *EMI_ODLC = EMI_ODL_C_VAL;
    *EMI_ODLD = EMI_ODL_D_VAL;
    *EMI_ODLE = EMI_ODL_E_VAL;
    *EMI_ODLF = EMI_ODL_F_VAL;

    __EMI_EnableDataAutoTracking();

    /**
        * Enable 1/5 DLL
        */
    *EMI_DLLV = 0x300f0000; //dll_cal_init = 1
    while (((*EMI_DLLV & 0x80) >> 7) != 1);  //wait_cal_done = 1
    *EMI_DLLV = 0x700f0000; // enable dll soft update
    while (((*EMI_DLLV & 0x80) >> 7) != 0);  //wait_cal_done = 0
    *EMI_DLLV = 0x100f0000; //initialization finish

    __EMI_EnableBandwidthLimiter();

    dump_EMI_Setting();
    test_result =  memory_btmt_test();
    if (test_result == true) {
        bl_print(LOG_DEBUG, "PSRAM Memory Test Pass!!");
        return -1;
    } else {
        bl_print(LOG_DEBUG, "PSRAM Memory Test Failed!!");
        return 0;
    }
#endif

}

typedef enum emi_clock_enum_t {
    EMI_CLK_LOW_TO_HIGH = 0,
    EMI_CLK_HIGH_TO_LOW  = 1,
} emi_clock;

int32_t EMI_DynamicClockSwitch(emi_clock clock)
{
    volatile uint32_t delay;
#ifdef MTK_NO_PSRAM_ENABLE
    return -1 ;
#else
    if (clock == EMI_CLK_LOW_TO_HIGH) {
        while ((*EMI_CONM & 0x01) == 0);  //wait EMI idle
        *EMI_CONM = *EMI_CONM | 0x700;   //mask request

        /*
        * Enable 1/5 DLL
        */
        *EMI_DLLV = 0x0;
        *EMI_DLLV = 0x300f0000; //dll_cal_init = 1
        while (((*EMI_DLLV & 0x80) >> 7) != 1); //wait_cal_done = 1
        *EMI_DLLV = 0x700f0000; // enable dll soft update
        while (((*EMI_DLLV & 0x80) >> 7) != 0); //wait_cal_done = 0
        *EMI_DLLV = 0x100f0000; //initialization finish

        for (delay = 0; delay < 0xFF; delay++);

        *EMI_CONM = *EMI_CONM & (~0x700);   //unmask request
    } else if (clock == EMI_CLK_HIGH_TO_LOW) {
        return 0;
    } else {
        return -1;
    }

    return 0;
#endif
}

void mtk_psram_half_sleep_exit(void)
{
#ifndef MTK_NO_PSRAM_ENABLE
    /* TINFO=" ----- exit half sleep mode begin ----- " */
    //exit half sleep
    *EMI_HFSLP = 0x1;
    while ((*EMI_HFSLP & 0x2) >> 1 == 0);
    *EMI_HFSLP = 0x0;
    while ((*EMI_HFSLP & 0x2) >> 1 == 1);

    *EMI_DLLV = 0x300f0000; //300f0000 for real chip
    while (((*EMI_DLLV & 0x00000080) >> 7) != 1);
    *EMI_DLLV = 0x700f0000;
    while (((*EMI_DLLV & 0x00000080) >> 7) == 1);
    *EMI_DLLV = 0x100f0000;

    *EMI_CONM = (*EMI_CONM & 0xfffff0ff);
    /* TINFO=" ----- exit half sleep mode end ----- " */
#endif

}

void mtk_psram_half_sleep_enter(void)
{
#ifndef MTK_NO_PSRAM_ENABLE
    /* TINFO= " ----- enter half sleep mode begin ----- " */
    //enter half sleep
    while ((*EMI_CONM & 0x11) != 0x11);
    *EMI_CONM = (*EMI_CONM | 0x00000700);
    *EMI_MREG_RW = 0xf0670000;
    while ((*EMI_MREG_RW & 0x01) == 0);
    *EMI_DLLV = 0x00000000;

    while ((*EMI_HFSLP & 0x10) >> 4 == 0);
    /* TINFO= " ----- enter half sleep mode end  ----- " */
#endif
}




