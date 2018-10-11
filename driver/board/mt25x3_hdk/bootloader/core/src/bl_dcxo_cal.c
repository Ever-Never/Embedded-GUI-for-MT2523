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

#include "bl_common.h"
#include "bl_mtk_bb_reg.h"
#include "bl_gpt.h"

//#define BL_DCXO_CAL_DEBUG
//#define BL_DCXO_DUMP_CAPID

#define BL_EFUSE_CAPID                  ((BL_P_UINT_32)(0xA2060180)) /* [8:0] CAPID0 [17:9] CAPID1 [26:18] CAPID2 */
#define BL_EFUSE_MAGIC                  ((BL_P_UINT_32)(0xA2060008))
#define BL_EFUSE_MAGIC_CODE             0xA07923B6
#define BL_EFUSE_REFRESH                ((BL_P_UINT_32)(0xA2060000))
#define BL_EFUSE_REFRESH_SET            0x4

#define BL_EFUSE_CAPID0_MASK            0x000001FF
#define BL_EFUSE_CAPID1_MASK            0x0003FE00
#define BL_EFUSE_CAPID2_MASK            0x07FC0000

#define BL_DCXO_CAP_ID                  ((BL_P_UINT_32)(0xA2110080)) /* [10:2] cal data [0] sel */
#define BL_DCXO_DEFAULT_VAL             431
#define BL_DCXO_GOLDEN_VAL              408

#define BL_MIXED_BASE                   0xA2040000
#define ABIST_FQMTR_BASE                (BL_MIXED_BASE + 0x400)
#define FQMTR_FCKSEL_XOSC_F32K_CK       6
#define FQMTR_TCKSEL_CSW_GP_26M_CK      7

typedef struct {
    __IO uint16_t ABIST_FQMTR_CON0;     /* Frequency Meter Control Register 0 */
    __IO uint16_t RESERVED1;
    __IO uint16_t ABIST_FQMTR_CON1;     /* Frequency Meter Control Register 1 */
    __IO uint16_t RESERVED2;
    __IO uint16_t ABIST_FQMTR_CON2;     /* Frequency Meter Control Register 2 */
    __IO uint16_t RESERVED3;
    __IO uint16_t ABIST_FQMTR_DATA;     /* Frequency Meter Data */
    __IO uint16_t RESERVED4;
    __IO uint16_t ABIST_FQMTR_DATA_MSB; /* Frequency Meter Data */
} ABIST_FQMTR_REGISTER_T;

static ABIST_FQMTR_REGISTER_T *abist_fqmtr_register = (ABIST_FQMTR_REGISTER_T *)ABIST_FQMTR_BASE;
uint32_t g_current_capid = BL_DCXO_DEFAULT_VAL;

uint32_t f32k_measure_clock(uint16_t fixed_clock, uint16_t tested_clock, uint16_t window_setting)
{
    uint32_t fqmtr_data;

    /* 1) PLL_ABIST_FQMTR_CON0 = 0xCXXX */
    abist_fqmtr_register->ABIST_FQMTR_CON0 |= 0xC000;
    bl_gpt_delay_us(1000);
    while ((abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0);
    /* 2) PLL_ABIST_FQMTR_CON1 = 0x0 */
    abist_fqmtr_register->ABIST_FQMTR_CON1 = 0;
    /* 3) PLL_ABIST_FQMTR_CON1 = 0x0601 */
    abist_fqmtr_register->ABIST_FQMTR_CON1 = (fixed_clock << 8) | tested_clock;
    abist_fqmtr_register->ABIST_FQMTR_CON2 = 0;
    /* 4) PLL_ABIST_FQMTR_CON0 = 0x8009 */
    abist_fqmtr_register->ABIST_FQMTR_CON0 = 0x8000 | (window_setting-1);
    bl_gpt_delay_us(1000);

#ifdef BL_DCXO_CAL_DEBUG
    /* dump registers */
    bl_print(LOG_DEBUG,"ABIST_FQMTR_CON0 = %u \r\n",abist_fqmtr_register->ABIST_FQMTR_CON0);
    bl_print(LOG_DEBUG,"ABIST_FQMTR_CON1 = %u \r\n",abist_fqmtr_register->ABIST_FQMTR_CON1);
    bl_print(LOG_DEBUG,"ABIST_FQMTR_CON2 = %u \r\n",abist_fqmtr_register->ABIST_FQMTR_CON2);
    bl_print(LOG_DEBUG,"ABIST_FQMTR_DATA = %u \r\n",abist_fqmtr_register->ABIST_FQMTR_DATA);
    bl_print(LOG_DEBUG,"ABIST_FQMTR_DATA_MSB = %u \r\n",abist_fqmtr_register->ABIST_FQMTR_DATA_MSB);
#endif

    /* 5) Wait PLL_ABIST_FQMTR_CON1 & 0x8000 == 0x8000 */
    while ((abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0);
    /* 6) Read PLL_ABIST_FQMTR_DATA */
    fqmtr_data = ((uint32_t)abist_fqmtr_register->ABIST_FQMTR_DATA_MSB << 16) | abist_fqmtr_register->ABIST_FQMTR_DATA;

#ifdef BL_DCXO_CAL_DEBUG
    /* dump registers */
    bl_print(LOG_DEBUG,"ABIST_FQMTR_CON0 = %u \r\n",abist_fqmtr_register->ABIST_FQMTR_CON0);
    bl_print(LOG_DEBUG,"ABIST_FQMTR_CON1 = %u \r\n",abist_fqmtr_register->ABIST_FQMTR_CON1);
    bl_print(LOG_DEBUG,"ABIST_FQMTR_CON2 = %u \r\n",abist_fqmtr_register->ABIST_FQMTR_CON2);
    bl_print(LOG_DEBUG,"ABIST_FQMTR_DATA = %u \r\n",abist_fqmtr_register->ABIST_FQMTR_DATA);
    bl_print(LOG_DEBUG,"ABIST_FQMTR_DATA_MSB = %u \r\n",abist_fqmtr_register->ABIST_FQMTR_DATA_MSB);
    bl_print(LOG_DEBUG,"fqmtr_data = %u \r\n",fqmtr_data);
#endif

    return fqmtr_data;
}

void set_capid(uint32_t target_capid,uint32_t current_capid)
{
    uint32_t dcxo_cap_id     = *BL_DCXO_CAP_ID;
    int32_t  duration        = target_capid - current_capid;
    int32_t temp_capid      = current_capid;

    BL_PRINT(LOG_DEBUG, " temp_capid = %d \n\r",temp_capid);

    if(duration>0){
        /* increase CAPID */
        BL_PRINT(LOG_DEBUG, " increase CAPID from %d to %d \n\r",current_capid,target_capid);
        while (duration>0) {
            /* calculate temp capid */
            duration -= 4;
            temp_capid +=4;
            if(temp_capid>target_capid){
                temp_capid = target_capid;
            }

            /* wait 4 us */
            bl_gpt_delay_us(4);
            /* set new val */
            dcxo_cap_id = *BL_DCXO_CAP_ID & 0xFFFFF803;
            *BL_DCXO_CAP_ID =  ((temp_capid<<2) | dcxo_cap_id);
            /* dump for debug */
            dcxo_cap_id = *BL_DCXO_CAP_ID;
#ifdef BL_DCXO_CAL_DEBUG
            BL_PRINT(LOG_DEBUG, " dump BL_DCXO_CAP_ID = %u \n\r",dcxo_cap_id);
#endif
        }
    }else{
        /* decrease CAPID */
        BL_PRINT(LOG_DEBUG, " decrease CAPID from %d to %d \n\r",current_capid,target_capid);
        while (duration<0) {
            /* calculate temp capid */
            duration += 4;
            temp_capid -=4;
            if(temp_capid<target_capid){
                temp_capid = target_capid;
            }

            if( temp_capid<0 ) {
                temp_capid = (int32_t)target_capid;
            }
            /* wait 4 us */
            bl_gpt_delay_us(4);
            /* set new val */
            dcxo_cap_id = *BL_DCXO_CAP_ID & 0xFFFFF803;
            *BL_DCXO_CAP_ID =  ((temp_capid<<2) | dcxo_cap_id);
            /* dump for debug */
            dcxo_cap_id = *BL_DCXO_CAP_ID;
#ifdef BL_DCXO_CAL_DEBUG
            BL_PRINT(LOG_DEBUG, " dump temp_capid = %d , BL_DCXO_CAP_ID = %u \n\r",temp_capid,dcxo_cap_id);
#endif
        }
    }
    /* dump final config */
    dcxo_cap_id = *BL_DCXO_CAP_ID;
    BL_PRINT(LOG_DEBUG, " BL_DCXO_CAP_ID = %u \n\r",dcxo_cap_id);
    /* record current capid */
    g_current_capid = target_capid;
}

void write_efuse_capid(uint32_t cal_capid)
{
    uint32_t efuse_capid  = *BL_EFUSE_CAPID;
    uint32_t efuse_capid0 = efuse_capid & BL_EFUSE_CAPID0_MASK;
    uint32_t efuse_capid1 = efuse_capid & BL_EFUSE_CAPID1_MASK;
    uint32_t efuse_capid2 = efuse_capid & BL_EFUSE_CAPID2_MASK;
    uint32_t temp = 0;
    int32_t  efuse_check = 0;

    (void)temp;
    (void)efuse_check;

    efuse_capid1 = efuse_capid1>>9;
    efuse_capid2 = efuse_capid2>>18;
    BL_PRINT(LOG_DEBUG, " EFUSE CAPID = %u \n\r",efuse_capid);
    BL_PRINT(LOG_DEBUG, " EFUSE CAPID0 = %u \n\r",efuse_capid0);
    BL_PRINT(LOG_DEBUG, " EFUSE CAPID1 = %u \n\r",efuse_capid1);
    BL_PRINT(LOG_DEBUG, " EFUSE CAPID2 = %u \n\r",efuse_capid2);
    BL_PRINT(LOG_DEBUG, " target cal_capid = %u \n\r",cal_capid);

#ifdef BL_CAL_DCXO_CAPID0
    if( efuse_capid0 == 0 ){
        BL_PRINT(LOG_DEBUG, " write EFUSE CAPID0 \n\r");
        *BL_EFUSE_MAGIC = BL_EFUSE_MAGIC_CODE;
        temp = *BL_EFUSE_MAGIC;
        BL_PRINT(LOG_DEBUG, " BL_EFUSE_MAGIC = %u \n\r",temp);
        *BL_EFUSE_CAPID = (cal_capid & BL_EFUSE_CAPID0_MASK);
        temp = *BL_EFUSE_CAPID;
        BL_PRINT(LOG_DEBUG, " BL_EFUSE_CAPID = %u \n\r",temp);
        *BL_EFUSE_REFRESH = BL_EFUSE_REFRESH_SET;
        BL_PRINT(LOG_DEBUG, " CAPID Calibration DONE!!!! EFUSE CAPID0 write (calibration result = %d )\n\r",cal_capid);
    } else {
        BL_PRINT(LOG_DEBUG, " CAPID Calibrated!!!!! \n\r");
        if(efuse_capid0 != cal_capid) {
            efuse_check = cal_capid - efuse_capid0;
            if( efuse_check>5||efuse_check<(-5) ){
                BL_PRINT(LOG_DEBUG, " WARNING!!!! EFUSE CAPID0 is not match calibration result ( efuse_capid0 = %d , calibration result = %d )\n\r",efuse_capid0,cal_capid);
            }
        }
        BL_PRINT(LOG_DEBUG, "efuse_capid0 = %d \n\r",efuse_capid0);
    }
    BL_PRINT(LOG_DEBUG, " Please re-download the normal bootloader \r\n");
    while(1){}
#endif
#ifdef BL_CAL_DCXO_CAPID1
    if( efuse_capid1 == 0 ){
        BL_PRINT(LOG_DEBUG, " write EFUSE CAPID1 \n\r");
        *BL_EFUSE_MAGIC = BL_EFUSE_MAGIC_CODE;
        temp = *BL_EFUSE_MAGIC;
        BL_PRINT(LOG_DEBUG, " BL_EFUSE_MAGIC = %u \n\r",temp);
        *BL_EFUSE_CAPID = ( (cal_capid<<9) & BL_EFUSE_CAPID1_MASK);
        temp = *BL_EFUSE_CAPID;
        BL_PRINT(LOG_DEBUG, " BL_EFUSE_CAPID = %u \n\r",temp);
        *BL_EFUSE_REFRESH = BL_EFUSE_REFRESH_SET;
        BL_PRINT(LOG_DEBUG, " CAPID Calibration DONE!!!! EFUSE CAPID1 write (calibration result = %d )\n\r",cal_capid);
    } else {
        BL_PRINT(LOG_DEBUG, " CAPID Calibrated!!!!! \n\r");
        if(efuse_capid1 != cal_capid) {
            efuse_check = cal_capid - efuse_capid1;
            if( efuse_check>5||efuse_check<(-5) ){
                BL_PRINT(LOG_DEBUG, " WARNING!!!! EFUSE CAPID1 is not match calibration result ( efuse_capid1 = %d , calibration result = %d )\n\r",efuse_capid1,cal_capid);
            }
        }
        BL_PRINT(LOG_DEBUG, "efuse_capid1 = %d \n\r",efuse_capid1);
    }
    BL_PRINT(LOG_DEBUG, " Please re-download the normal bootloader \r\n");
    while(1){}
#endif
#ifdef BL_CAL_DCXO_CAPID2
    if( efuse_capid2 == 0 ){
        BL_PRINT(LOG_DEBUG, " write EFUSE CAPID2 \n\r");
        *BL_EFUSE_MAGIC = BL_EFUSE_MAGIC_CODE;
        temp = *BL_EFUSE_MAGIC;
        BL_PRINT(LOG_DEBUG, " BL_EFUSE_MAGIC = %u \n\r",temp);
        *BL_EFUSE_CAPID = ( (cal_capid<<18) & BL_EFUSE_CAPID2_MASK);
        temp = *BL_EFUSE_CAPID;
        BL_PRINT(LOG_DEBUG, " BL_EFUSE_CAPID = %u \n\r",temp);
        *BL_EFUSE_REFRESH = BL_EFUSE_REFRESH_SET;
        BL_PRINT(LOG_DEBUG, " CAPID Calibration DONE!!!! EFUSE CAPID2 write (calibration result = %d )\n\r",cal_capid);
    } else {
        BL_PRINT(LOG_DEBUG, " CAPID Calibrated!!!!! \n\r");
        if(efuse_capid2 != cal_capid) {
            efuse_check = cal_capid - efuse_capid2;
            if( efuse_check>5||efuse_check<(-5) ){
                BL_PRINT(LOG_DEBUG, " WARNING!!!! EFUSE CAPID2 is not match calibration result ( efuse_capid2 = %d , calibration result = %d )\n\r",efuse_capid2,cal_capid);
            }
        }
        BL_PRINT(LOG_DEBUG, "efuse_capid2 = %d \n\r",efuse_capid2);
    }
    BL_PRINT(LOG_DEBUG, " Please re-download the normal bootloader \r\n");
    while(1){}
#endif
}

uint32_t read_efuse_capid()
{
    uint32_t efuse_capid  = *BL_EFUSE_CAPID;
    uint32_t efuse_capid0 = efuse_capid & BL_EFUSE_CAPID0_MASK;
    uint32_t efuse_capid1 = efuse_capid & BL_EFUSE_CAPID1_MASK;
    uint32_t efuse_capid2 = efuse_capid & BL_EFUSE_CAPID2_MASK;
    efuse_capid1 = efuse_capid1>>9;
    efuse_capid2 = efuse_capid2>>18;
    if(efuse_capid2!=0){
        BL_PRINT(LOG_DEBUG, " read EFUSE CAPID2 %d \n\r",efuse_capid2);
        return efuse_capid2;
    }else if(efuse_capid1!=0){
        BL_PRINT(LOG_DEBUG, " read EFUSE CAPID1 %d \n\r",efuse_capid1);
        return efuse_capid1;
    }else if(efuse_capid0!=0){
        BL_PRINT(LOG_DEBUG, " read EFUSE CAPID0 %d \n\r",efuse_capid0);
        return efuse_capid0;
    }else {
        BL_PRINT(LOG_DEBUG, " EFUSE CAPID not exists , read golden value\n\r");
        return BL_DCXO_GOLDEN_VAL;
    }
}

void bl_dcxo_factory_calibration(void)
{
    uint32_t cal_capid = 0;
    int i = 8;
    uint32_t fqmtr_data;

#ifdef BL_DCXO_DUMP_CAPID
    /* check 1 ~ 511 freq meter */
    uint32_t test_val = 0;
    for(test_val = 1;test_val<=511;test_val+=10) {
        set_capid(test_val,g_current_capid);
        fqmtr_data = f32k_measure_clock(FQMTR_FCKSEL_XOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 1200);
        BL_PRINT(LOG_DEBUG, " dcxo check capid = %u (%d) , fqmtr = %d \n\r",test_val,test_val,fqmtr_data);
    }
#endif

    for(i=8;i>=0;i--){
        cal_capid = cal_capid + (1<<i);
        set_capid(cal_capid,g_current_capid);

        fqmtr_data = f32k_measure_clock(FQMTR_FCKSEL_XOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 1200);
        BL_PRINT(LOG_DEBUG, " dump capid = %u (%d) , fqmtr = %d \n\r",cal_capid,cal_capid,fqmtr_data);
        if (fqmtr_data == 952148) {
            BL_PRINT(LOG_DEBUG, " target fqmtr 952148 found \n\r");
            break;
        }

        if (fqmtr_data > 952148) {
            cal_capid = cal_capid - (1<<i);
        }
        BL_PRINT(LOG_DEBUG, " dump capid result = %u (%d) \n\r",cal_capid,cal_capid);
        /* wait 1 ms */
        bl_gpt_delay_us(1000);
    }

    BL_PRINT(LOG_DEBUG, " final cal capid = %d , fqmtr = %d \n\r",cal_capid,fqmtr_data);

    if(cal_capid==0){
        BL_PRINT(LOG_DEBUG, " final cal capid = 0 , change to 1 \n\r");
        cal_capid = 1;
    }

    /* write cal_capid to EFUSE */
#ifdef BL_DCXO_KVALUE_SW
    BL_PRINT(LOG_DEBUG, " Please write the calibrated CAPID (%d) in SW, re-build, and re-download the normal bootloader \r\n", cal_capid);
    while(1){}
#else
    write_efuse_capid(cal_capid);
#endif
}


void bl_dcxo_load_calibration(void)
{
    /* read calibration from e fuse or golden K val */
    uint32_t dcxo_cap_id     = *BL_DCXO_CAP_ID;
  #ifdef BL_DCXO_KVALUE_SW
    uint32_t efuse_capid     = BL_DCXO_GOLDEN_VAL;
    BL_PRINT(LOG_DEBUG, " BL_DCXO_CAP_ID = %u , SW K value = %u \r\n",dcxo_cap_id,efuse_capid);
  #else
    uint32_t efuse_capid     = read_efuse_capid();
    BL_PRINT(LOG_DEBUG, " BL_DCXO_CAP_ID = %u , efuse capid =%u \r\n",dcxo_cap_id,efuse_capid);
  #endif
    if (efuse_capid == 0) {
        BL_PRINT(LOG_DEBUG, "Error!!! CAPID==0!!!\r\n");
        while(1){}
    }
    set_capid(efuse_capid,g_current_capid);
}

void bl_dcxo_capid_init(void)
{
    uint32_t dcxo_cap_id     = *BL_DCXO_CAP_ID;
    uint32_t traget_cal_data = BL_DCXO_DEFAULT_VAL;
    /* set sel = 1 */
    *BL_DCXO_CAP_ID =  (0x00000001 | dcxo_cap_id);
    /* set default val */
    dcxo_cap_id = *BL_DCXO_CAP_ID & 0xFFFFF803;
    *BL_DCXO_CAP_ID =  ((traget_cal_data<<2) | dcxo_cap_id);
    dcxo_cap_id = *BL_DCXO_CAP_ID;
    BL_PRINT(LOG_DEBUG, " BL_DCXO_CAP_ID = %u \r\n",dcxo_cap_id);
    g_current_capid = BL_DCXO_DEFAULT_VAL;

}
