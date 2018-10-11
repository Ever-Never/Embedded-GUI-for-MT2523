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

#include "hal_pmu.h"

#ifdef HAL_PMU_MODULE_ENABLED
#ifndef MTK_EXTERNAL_PMIC
#include "hal_i2c_pmic.h"
#include "hal_i2c_master.h"
#include "hal_i2c_master_internal.h"
#include "hal_eint.h"
#include "hal_log.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_pmu_auxadc.h"

#define PMU_SWBAT_ALWAYS_ON
#define LONG_PRESS_SHUTDOWN_ENABLE
#define PMIC_SLAVE_ADDR (0x6B)

#define HAL_PMU_DEEBUG_ENABLE
#ifdef  HAL_PMU_DEEBUG_ENABLE
#define log_hal_debug(_message,...) log_hal_info(_message, ##__VA_ARGS__)
#else
#define log_hal_debug(_message,...)
#endif

static unsigned char pmu_power_mode_init_setting_E1[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 6	, 6	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP      , PMIC_LDO_PMOD_LP      , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 4	, 4	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 3	, 3	},	//VBT
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 2	, 2	},	//VA28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	}	//VMC
};

static unsigned char pmu_power_mode_init_setting_E2[10][8] = {
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                     , 0	, 0	},	//VCLDO
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                     , 0	, 0	},	//VIO18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_ULP	, PMIC_LDO_PMOD_ULP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VSF
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VIO28
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_LP      , PMIC_LDO_PMOD_LP      , PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, 0                     , 0	, 0	},	//VDIG18
    {	PMIC_VR_CTL_ENABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_HW_CTL	, 0	, 0	},	//VA18
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VUSB
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 1	, 1	},	//VBT
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	},	//VA28
    {	PMIC_VR_CTL_DISABLE	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_NORMAL	, PMIC_LDO_PMOD_LP	, PMIC_LDO_PMOD_LP	, PMIC_SLP_WKUP_SW_CTL	, 0	, 0	}	//VMC
};

unsigned char Wrap_D2D_I2C_Write(unsigned char *ptr_send);
unsigned char Wrap_D2D_I2C_Read(unsigned char *ptr_send, unsigned char *ptr_read);
void pmu_eint_init(void);
void pmu_eint_handler(void *parameter);

pmu_function_t pmu_function_table[PMU_INT_MAX];

void pmu_init_mt6385(void)
{
    static unsigned char init = 0;
    uint8_t  id_l, id_h;
    pmic_i2c_config_t config;

    if (init != 0) {
        return;
    }
    init = 1;

    config.transfer_frequency = 2500;
    pmic_i2c_init(PMIC_I2C_MASTER, &config);

    pmu_set_register_value(PMU_I2C_CONFIG_ADDR  , PMU_I2C_CONFIG_MASK    , PMU_I2C_CONFIG_SHIFT   , 1);

    id_l = pmu_get_register_value(PMU_HWCID_L_ADDR  , PMU_HWCID_L_MASK    , PMU_HWCID_L_SHIFT);
    id_h = pmu_get_register_value(PMU_HWCID_H_ADDR  , PMU_HWCID_H_MASK    , PMU_HWCID_H_SHIFT);
    if ((id_l == 0x10) && (id_h == 0x85)) {
        log_hal_debug("\n\rPMU Version : E1\n\r");
    }
    if ((id_l == 0x11) && (id_h == 0x85)) {
        log_hal_debug("\n\rPMU Version : E2\n\r");
    }

    //PMU E1 Init Setting
    if (id_l == 0x10) {
        pmu_set_register_value(PMU_RG_PKEY_LPEN_ADDR  , PMU_RG_PKEY_LPEN_MASK    , PMU_RG_PKEY_LPEN_SHIFT   , 0);

        // RG_XRST_EN[3:3] = 0x1 => 11/05:seven; If ECO change default value
        pmu_set_register_value(PMU_RG_XRST_EN_ADDR  , PMU_RG_XRST_EN_MASK    , PMU_RG_XRST_EN_SHIFT   , 1);

        // RG_PKEY_LPEN[5:5] = 0x1 => 11/5:seven, Efuse =1
        //pmu_set_register_value(PMU_RG_PKEY_LPEN_ADDR  ,PMU_RG_PKEY_LPEN_MASK    ,PMU_RG_PKEY_LPEN_SHIFT   ,1);

        // RG_UVLRST_SYNC[4:4] = 0x0 => 11/5:seven, Efuse =0
        pmu_set_register_value(PMU_RG_UVLRST_SYNC_ADDR  , PMU_RG_UVLRST_SYNC_MASK    , PMU_RG_UVLRST_SYNC_SHIFT   , 0);

        // RG_SKIP_OTP_OUT[0:0] = 0x0
        pmu_set_register_value(PMU_RG_SKIP_OTP_OUT_ADDR  , PMU_RG_SKIP_OTP_OUT_MASK    , PMU_RG_SKIP_OTP_OUT_SHIFT   , 0);

        // AUXADC_AVG_NUM_SEL[7:0] = 0x83
        pmu_set_register_value(PMU_AUXADC_CON2  , 0xFF    , 0   , 0x83);

        // AUXADC_TRIM_CH4_SEL[1:0] = 0x1
        pmu_set_register_value(PMU_AUXADC_TRIM_CH4_SEL_ADDR  , PMU_AUXADC_TRIM_CH4_SEL_MASK    , PMU_AUXADC_TRIM_CH4_SEL_SHIFT   , 1);

        //unlock key
        pmu_set_vrc_key_mt6385(PMIC_UNLOCK_KEY);

        pmu_set_register_value(PMU_RG_PWRHOLD_ADDR          , PMU_RG_PWRHOLD_MASK            , PMU_RG_PWRHOLD_SHIFT           , 1);
        pmu_set_register_value(PMU_RG_OFF_VDIG18_PMOD_ADDR  , PMU_RG_OFF_VDIG18_PMOD_MASK    , PMU_RG_OFF_VDIG18_PMOD_SHIFT   , 2);
        pmu_set_ovr_mode_mt6385(PMIC_LEAVE_OVR_MODE);
        
        //open voltage buffer for auxadc ch4
        pmu_set_register_value(0x026B   , 0xFF   , 0  , 0x0C);

        pmu_init_power_mode(pmu_power_mode_init_setting_E1);
    }

    //PMU E2 Init Setting
    if (id_l == 0x11) {
#ifdef LONG_PRESS_SHUTDOWN_ENABLE
        pmu_set_register_value(PMU_RG_PKEY_LPEN_ADDR    , PMU_RG_PKEY_LPEN_MASK      , PMU_RG_PKEY_LPEN_SHIFT     , 1);
#else
        pmu_set_register_value(PMU_RG_PKEY_LPEN_ADDR    , PMU_RG_PKEY_LPEN_MASK      , PMU_RG_PKEY_LPEN_SHIFT     , 0);
#endif

        pmu_set_register_value(PMU_I2C_CONFIG_ADDR     , PMU_I2C_CONFIG_MASK     , PMU_I2C_CONFIG_SHIFT       , 1);

        pmu_set_register_value(PMU_RG_RST_DRVSEL_ADDR  , PMU_RG_RST_DRVSEL_MASK  , PMU_RG_RST_DRVSEL_SHIFT    , 1);

        pmu_set_register_value(PMU_RG_XRST_EN_ADDR     , PMU_RG_XRST_EN_MASK     , PMU_RG_XRST_EN_SHIFT       , 1);

        pmu_set_register_value(PMU_RG_UVLRST_SYNC_ADDR , PMU_RG_UVLRST_SYNC_MASK , PMU_RG_UVLRST_SYNC_SHIFT   , 0);

        pmu_set_register_value(PMU_RG_VCORE_PFM_RIP_ADDR           , PMU_RG_VCORE_PFM_RIP_MASK           , PMU_RG_VCORE_PFM_RIP_SHIFT             , 1);

        pmu_set_register_value(PMU_AUXADC_CK_AON_ADDR              , PMU_AUXADC_CK_AON_MASK              , PMU_AUXADC_CK_AON_SHIFT                , 0);

        pmu_set_register_value(PMU_AUXADC_AVG_NUM_SEL_LSB_ADDR     , PMU_AUXADC_AVG_NUM_SEL_LSB_MASK    , PMU_AUXADC_AVG_NUM_SEL_LSB_SHIFT        , 0x83);

        pmu_set_register_value(PMU_AUXADC_AVG_NUM_SEL_WAKEUP_ADDR  , PMU_AUXADC_AVG_NUM_SEL_WAKEUP_MASK , PMU_AUXADC_AVG_NUM_SEL_WAKEUP_SHIFT     , 1);

        pmu_set_register_value(PMU_AUXADC_TRIM_CH4_SEL_ADDR        , PMU_AUXADC_TRIM_CH4_SEL_MASK       , PMU_AUXADC_TRIM_CH4_SEL_SHIFT           , 1);

        //open voltage buffer for auxadc ch4
        pmu_set_register_value(0x026B   , 0xFF   , 0  , 0x0C);

        pmu_init_power_mode(pmu_power_mode_init_setting_E2);

        pmu_set_vrc_key_mt6385(PMIC_UNLOCK_KEY);

        pmu_set_register_value(PMU_RG_PWRHOLD_ADDR         , PMU_RG_PWRHOLD_MASK        , PMU_RG_PWRHOLD_SHIFT       , 1);
        pmu_set_ovr_mode_mt6385(PMIC_LEAVE_OVR_MODE);

        pmu_set_vrc_key_mt6385(PMIC_LOCK_KEY);
    }

    //Disable MTCMOS ForceON setting
    pmu_set_register_value(PMU_RG_MTCTL_FORCE_ON_ADDR     , PMU_RG_MTCTL_FORCE_ON_MASK , PMU_RG_MTCTL_FORCE_ON_SHIFT, 0);

    //Automatic poweup mtcmos and ,ivgen From S0,S1 to HP,LP.
    pmu_set_register_value(PMU_PPCPSI  , 0xFF    , 0   , 0x0);

    pmu_auxadc_init();

    pmu_eint_init();
}

bool pmu_set_register_value_mt6385(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value)
{
    unsigned char send_buffer[3], receive_buffer[1];
    unsigned int mask_pri;

    send_buffer[1] = (address) & 0x00FF  ;
    send_buffer[0] = ((address >> 8) & 0x00FF) | 0x40;

    mask_pri = save_and_set_interrupt_mask();

    Wrap_D2D_I2C_Read(send_buffer, receive_buffer);

    receive_buffer[0] &= (~(mask << shift));
    send_buffer[2] = receive_buffer[0] | (value << shift); //data value

    if (Wrap_D2D_I2C_Write(send_buffer) == 0) {
        restore_interrupt_mask(mask_pri);
        return true;
    } else {
        restore_interrupt_mask(mask_pri);
        return false;
    }
}

unsigned char pmu_get_register_value_mt6385(unsigned short int address, unsigned short int mask, unsigned short int shift)
{
    unsigned char send_buffer[2], receive_buffer[1];
    unsigned int mask_pri;

    send_buffer[1] = (address) & 0x00FF;
    send_buffer[0] = ((address >> 8) & 0x00FF) | 0x40;  //Addr_H[7:4] = 01xx : read 1B

    mask_pri = save_and_set_interrupt_mask();
    Wrap_D2D_I2C_Read(send_buffer, receive_buffer);
    restore_interrupt_mask(mask_pri);
    return ((receive_buffer[0] >> shift)&mask);
}

//internal use
unsigned char Wrap_D2D_I2C_Write(unsigned char *ptr_send)
{
    unsigned char retry_cnt = 0, result_read;

    *(ptr_send) = *(ptr_send) | 0x40;

    do {
        result_read = pmic_i2c_send_polling(PMIC_I2C_MASTER, PMIC_SLAVE_ADDR, ptr_send, 3);
        retry_cnt++;
    } while ((result_read != 0) && (retry_cnt <= 60));

    retry_cnt--;
    return (retry_cnt);
}

//internal use
unsigned char Wrap_D2D_I2C_Read(unsigned char *ptr_send, unsigned char *ptr_read)
{
    pmic_i2c_send_to_receive_config_t config;
    unsigned char retry_cnt = 0, result_read;

    *(ptr_send) = *(ptr_send) | 0x40;
    config.slave_address      = PMIC_SLAVE_ADDR;
    config.send_data          = ptr_send;
    config.send_length        = 2;
    config.receive_buffer     = ptr_read;
    config.receive_length     = 1;

    do {
        result_read = pmic_i2c_send_to_receive_polling(PMIC_I2C_MASTER, &config);
        retry_cnt++;
    } while ((result_read != 0) && (retry_cnt <= 60));

    retry_cnt--;
    return (retry_cnt);
}

void pmu_eint_init()
{
    hal_eint_config_t config;
    config.trigger_mode = HAL_EINT_EDGE_FALLING;
    config.debounce_time = 1;

    hal_eint_init(HAL_EINT_PMIC, &config);    //set EINT trigger mode and debounce time.
    hal_eint_register_callback(HAL_EINT_PMIC, pmu_eint_handler, NULL); // register a user callback.

    hal_eint_unmask(HAL_EINT_PMIC);
}


void pmu_eint_handler(void *parameter)
{
    static unsigned int index;
    static unsigned int int_sts[7], trigger_staus;
    unsigned int mask;

    hal_eint_mask(HAL_EINT_PMIC);
    mask = save_and_set_interrupt_mask();
    //read pmu interrupt status INT_STS0(ADDR:0x1E) ~ INT_STS6(ADDR:0x24)
    for (index = 0; index < 7; index++) {
        int_sts[index] = pmu_get_register_value(PMU_INTSTS0 + index, 0xFF, 0);
    }

    //INT_STS0 : PMU_INT_ALDO_OC,PMU_INT_ALDO_PG,PMU_INT_DLDO_OC,PMU_INT_DLDO_PG(index 0~3)
    for (index = 0; index < 4; index++) {
        trigger_staus = (int_sts[0] >> (index * 2)) & 0x03;
        if ((trigger_staus != 0) && (pmu_function_table[index].init_status == PMU_INIT)) {
            pmu_function_table[index].pmu_callback(trigger_staus, pmu_function_table[index].user_data);
        }
    }

    //INT_STS1 : PMU_INT_VC_BUCK_OC,PMU_INT_VCORE_PG,PMU_INT_VC_LDO_OC,PMU_INT_PKEYLP(index 4~7)
    for (index = 0; index < 4; index++) {
        trigger_staus = (int_sts[1] >> (index * 2)) & 0x03;
        if ((trigger_staus != 0) && (pmu_function_table[index + 4].init_status == PMU_INIT)) {
            pmu_function_table[index + 4].pmu_callback(trigger_staus, pmu_function_table[index + 4].user_data);
        }
    }

    //INT_STS2 : PMU_INT_THM1,PMU_INT_THM2,PMU_INT_AXPKEY,PMU_INT_PWRKEY(index 8~11)
    for (index = 0; index < 4; index++) {
        trigger_staus = (int_sts[2] >> (index * 2)) & 0x03;
        if ((trigger_staus != 0) && (pmu_function_table[index + 8].init_status == PMU_INIT)) {
            pmu_function_table[index + 8].pmu_callback(trigger_staus, pmu_function_table[index + 8].user_data);
        }
    }

    //INT_STS3 : PMU_INT_CHGOV,PMU_INT_CHRDET,PMU_INT_THR_H,PMU_INT_THR_L(index 12~15)
    for (index = 0; index < 4; index++) {
        trigger_staus = (int_sts[3] >> (index * 2)) & 0x03;
        if ((trigger_staus != 0) && (pmu_function_table[index + 12].init_status == PMU_INIT)) {
            pmu_function_table[index + 12].pmu_callback(trigger_staus, pmu_function_table[index + 12].user_data);
        }
    }

    //INT_STS4 : PMU_INT_OVER110,PMU_INT_OVER40,PMU_INT_PSW_PG(index 16~18)
    for (index = 0; index < 3; index++) {
        trigger_staus = (int_sts[4] >> (index * 2)) & 0x03;
        if ((trigger_staus != 0) && (pmu_function_table[index + 16].init_status == PMU_INIT)) {
            pmu_function_table[index + 16].pmu_callback(trigger_staus, pmu_function_table[index + 16].user_data);
        }
    }

    //INT_STS5 : PMU_INT_BAT_H...(index 19~26) : level trigger
    for (index = 0; index < 8; index++) {
        trigger_staus = (int_sts[5] >> (index * 1)) & 0x01;
        if ((trigger_staus != 0) && (pmu_function_table[index + 19].init_status == PMU_INIT)) {
            pmu_function_table[index + 19].pmu_callback(trigger_staus, pmu_function_table[index + 19].user_data);
        }
    }

    //INT_STS6 : PMU_INT_COOL_LV...(index 27~31) : level trigger
    for (index = 0; index < 5; index++) {
        trigger_staus = (int_sts[6] >> (index * 1)) & 0x01;
        if ((trigger_staus != 0) && (pmu_function_table[index + 27].init_status == PMU_INIT)) {
            pmu_function_table[index + 27].pmu_callback(trigger_staus, pmu_function_table[index + 27].user_data);
        }
    }

    for (index = 0; index < 7; index++) {
        pmu_set_register_value(PMU_INTSTS0 + index, 0xFF, 0, 0xFF); //Write 1 Clear interrupt Status
    }
    for (index = 0; index < 7; index++) {
        int_sts[index] = pmu_get_register_value(PMU_INTSTS0 + index, 0xFF, 0);
        if (int_sts[index] != 0) {
            pmu_set_register_value(PMU_INTSTS0 + index, 0xFF, 0, 0xFF); //Write 1 Clear interrupt Status
        }
    }
    restore_interrupt_mask(mask);
    hal_eint_unmask(HAL_EINT_PMIC);
}

pmu_status_t pmu_register_callback(pmu_int_ch_t pmu_int_ch, pmu_int_trigger_mode_t trigger_mode, pmu_callback_t callback, void *user_data)
{
    pmu_status_t status = PMU_STATUS_ERROR;
    unsigned int mask;

    if (pmu_int_ch >= PMU_INT_MAX || callback == NULL) {
        status = PMU_STATUS_INVALID_PARAMETER;
        return status;
    }

    mask = save_and_set_interrupt_mask();

    pmu_function_table[pmu_int_ch].pmu_callback = callback;
    pmu_function_table[pmu_int_ch].user_data = user_data;
    pmu_function_table[pmu_int_ch].trigger_mode = trigger_mode;
    pmu_function_table[pmu_int_ch].init_status = PMU_INIT;

    pmu_enable_interrupt(pmu_int_ch, trigger_mode, 1);

    restore_interrupt_mask(mask);

    status = PMU_STATUS_SUCCESS;
    return status;
}

pmu_status_t pmu_disable_interrupt(pmu_int_ch_t int_ch, pmu_int_trigger_mode_t trigger_mode)
{
    pmu_status_t status = PMU_STATUS_ERROR;
    unsigned int mask;

    mask = save_and_set_interrupt_mask();

    pmu_enable_interrupt(int_ch, trigger_mode, 0);          //Disable pmu interrupt source
    pmu_function_table[int_ch].init_status = PMU_INIT;  //Set deinit status

    restore_interrupt_mask(mask);

    status = PMU_STATUS_SUCCESS;
    return status;
}

pmu_status_t pmu_enable_interrupt(pmu_int_ch_t int_ch, pmu_int_trigger_mode_t trigger_mode, unsigned char enable)
{
    unsigned char Channel = (unsigned char)int_ch;

    if (Channel >= PMU_INT_MAX) {
        return (PMU_STATUS_ERROR);
    }

    switch (Channel) {
        case PMU_INT_ALDO_OC:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_ALDO_OC_R_ADDR, 1, PMU_INTE_ALDO_OC_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_ALDO_OC_F_ADDR, 1, PMU_INTE_ALDO_OC_F_SHIFT, enable);
            }
            break;

        case PMU_INT_ALDO_PG:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_ALDO_PG_R_ADDR, 1, PMU_INTE_ALDO_PG_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_ALDO_PG_F_ADDR, 1, PMU_INTE_ALDO_PG_F_SHIFT, enable);
            }
            break;

        case PMU_INT_DLDO_OC:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_DLDO_OC_R_ADDR, 1, PMU_INTE_DLDO_OC_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_DLDO_OC_F_ADDR, 1, PMU_INTE_DLDO_OC_F_SHIFT, enable);
            }
            break;

        case PMU_INT_DLDO_PG:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_DLDO_PG_R_ADDR, 1, PMU_INTE_DLDO_PG_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_DLDO_PG_F_ADDR, 1, PMU_INTE_DLDO_PG_F_SHIFT, enable);
            }
            break;

        case PMU_INT_VC_BUCK_OC:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_VC_BUCK_OC_R_ADDR, 1, PMU_INTE_VC_BUCK_OC_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_VC_BUCK_OC_F_ADDR, 1, PMU_INTE_VC_BUCK_OC_F_SHIFT, enable);
            }
            break;

        case PMU_INT_VCORE_PG:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_VCORE_PG_R_ADDR, 1, PMU_INTE_VCORE_PG_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_VCORE_PG_F_ADDR, 1, PMU_INTE_VCORE_PG_F_SHIFT, enable);
            }
            break;

        case PMU_INT_VC_LDO_OC:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_VC_LDO_OC_R_ADDR, 1, PMU_INTE_VC_LDO_OC_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_VC_LDO_OC_F_ADDR, 1, PMU_INTE_VC_LDO_OC_F_SHIFT, enable);
            }
            break;

        case PMU_INT_PKEYLP:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_PKEYLP_R_ADDR, 1, PMU_INTE_PKEYLP_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_PKEYLP_F_ADDR, 1, PMU_INTE_PKEYLP_F_SHIFT, enable);
            }
            break;

        case PMU_INT_THM1:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_THM1_R_ADDR, 1, PMU_INTE_THM1_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_THM1_F_ADDR, 1, PMU_INTE_THM1_F_SHIFT, enable);
            }
            break;

        case PMU_INT_THM2:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_THM2_R_ADDR, 1, PMU_INTE_THM2_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_THM2_F_ADDR, 1, PMU_INTE_THM2_F_SHIFT, enable);
            }
            break;

        case PMU_INT_AXPKEY:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_AXPKEY_R_ADDR, 1, PMU_INTE_AXPKEY_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_AXPKEY_F_ADDR, 1, PMU_INTE_AXPKEY_F_SHIFT, enable);
            }
            break;

        case PMU_INT_PWRKEY:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_PWRKEY_R_ADDR, 1, PMU_INTE_PWRKEY_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_PWRKEY_F_ADDR, 1, PMU_INTE_PWRKEY_F_SHIFT, enable);
            }
            break;

        case PMU_INT_CHGOV:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_CHGOV_R_ADDR, 1, PMU_INTE_CHGOV_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_CHGOV_F_ADDR, 1, PMU_INTE_CHGOV_F_SHIFT, enable);
            }
            break;

        case PMU_INT_CHRDET:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_CHRDET_R_ADDR, 1, PMU_INTE_CHRDET_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_CHRDET_F_ADDR, 1, PMU_INTE_CHRDET_F_SHIFT, enable);
            }
            break;

        case PMU_INT_THR_H:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_THR_H_R_ADDR, 1, PMU_INTE_THR_H_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_THR_H_F_ADDR, 1, PMU_INTE_THR_H_F_SHIFT, enable);
            }
            break;

        case PMU_INT_THR_L:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_THR_L_R_ADDR, 1, PMU_INTE_THR_L_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_THR_L_F_ADDR, 1, PMU_INTE_THR_L_F_SHIFT, enable);
            }
            break;

        case PMU_INT_OVER110:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_OVER110_R_ADDR, 1, PMU_INTE_OVER110_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_OVER110_F_ADDR, 1, PMU_INTE_OVER110_F_SHIFT, enable);
            }
            break;

        case PMU_INT_OVER40:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_OVER40_R_ADDR, 1, PMU_INTE_OVER40_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_OVER40_F_ADDR, 1, PMU_INTE_OVER40_F_SHIFT, enable);
            }
            break;

        case PMU_INT_PSW_PG:
            if ((trigger_mode & 0x01) == 0x01) {
                pmu_set_register_value(PMU_INTE_PSW_PG_R_ADDR, 1, PMU_INTE_PSW_PG_R_SHIFT, enable);
            }
            if ((trigger_mode & 0x02) == 0x02) {
                pmu_set_register_value(PMU_INTE_PSW_PG_F_ADDR, 1, PMU_INTE_PSW_PG_F_SHIFT, enable);
            }
            break;

        case PMU_INT_BAT_H:
            pmu_set_register_value(PMU_INTE_BAT_H_LV_ADDR, 1, PMU_INTE_BAT_H_LV_SHIFT, enable);
            break;

        case PMU_INT_BAT_L:
            pmu_set_register_value(PMU_INTE_BAT_L_LV_ADDR, 1, PMU_INTE_BAT_L_LV_SHIFT, enable);
            break;

        case PMU_INT_VBATON_HV_LV:
            pmu_set_register_value(PMU_INTE_VBATON_HV_LV_ADDR, 1, PMU_INTE_VBATON_HV_LV_SHIFT, enable);
            break;

        case PMU_INT_VBAT_UNDET_LV:
            pmu_set_register_value(PMU_INTE_VBAT_UNDET_LV_ADDR, 1, PMU_INTE_VBAT_UNDET_LV_SHIFT, enable);
            break;

        case PMU_INT_BVALID_DET_LV:
            pmu_set_register_value(PMU_INTE_BVALID_DET_LV_ADDR, 1, PMU_INTE_BVALID_DET_LV_SHIFT, enable);
            break;

        case PMU_INT_CHRWDT_LV:
            pmu_set_register_value(PMU_INTE_CHRWDT_LV_ADDR, 1, PMU_INTE_CHRWDT_LV_SHIFT, enable);
            break;

        case PMU_INT_HOT_LV:
            pmu_set_register_value(PMU_INTE_HOT_LV_ADDR, 1, PMU_INTE_HOT_LV_SHIFT, enable);
            break;

        case PMU_INT_WARM_LV:
            pmu_set_register_value(PMU_INTE_WARM_LV_ADDR, 1, PMU_INTE_WARM_LV_SHIFT, enable);
            break;

        case PMU_INT_COOL_LV:
            pmu_set_register_value(PMU_INTE_COOL_LV_ADDR, 1, PMU_INTE_COOL_LV_SHIFT, enable);
            break;

        case PMU_INT_COLD_LV:
            pmu_set_register_value(PMU_INTE_COLD_LV_ADDR, 1, PMU_INTE_COLD_LV_SHIFT, enable);
            break;

        case PMU_INT_IMP_LV:
            pmu_set_register_value(PMU_INTE_IMP_LV_ADDR, 1, PMU_INTE_IMP_LV_SHIFT, enable);
            break;

        case PMU_INT_NAG_C_LV:
            pmu_set_register_value(PMU_INTE_NAG_C_LV_ADDR, 1, PMU_INTE_NAG_C_LV_SHIFT, enable);
            break;

        case PMU_INT_AD_LBAT_LV:
            pmu_set_register_value(PMU_INTE_AD_LBAT_LV_ADDR, 1, PMU_INTE_AD_LBAT_LV_SHIFT, enable);
            break;
    }

    return (PMU_STATUS_SUCCESS);
}

void pmu_init_power_mode(unsigned char power_mode_setting[10][8])
{
    unsigned int i;

    //cofing ldo psi power mode
    for (i = 0; i < 4; i++) {
        PMIC_VR_PSI_PMOD_CONFIG(PMIC_VCLDO  , (PMIC_PSI)(PMIC_PSI_HP + i), (PMIC_LDO_PMOD)power_mode_setting[0][1 + i]);
        PMIC_VR_PSI_PMOD_CONFIG(PMIC_VIO18  , (PMIC_PSI)(PMIC_PSI_HP + i), (PMIC_LDO_PMOD)power_mode_setting[1][1 + i]);
        PMIC_VR_PSI_PMOD_CONFIG(PMIC_VSF    , (PMIC_PSI)(PMIC_PSI_HP + i), (PMIC_LDO_PMOD)power_mode_setting[2][1 + i]);
        PMIC_VR_PSI_PMOD_CONFIG(PMIC_VIO28  , (PMIC_PSI)(PMIC_PSI_HP + i), (PMIC_LDO_PMOD)power_mode_setting[3][1 + i]);
        PMIC_VR_PSI_PMOD_CONFIG(PMIC_VDIG18 , (PMIC_PSI)(PMIC_PSI_HP + i), (PMIC_LDO_PMOD)power_mode_setting[4][1 + i]);
        PMIC_VR_PSI_PMOD_CONFIG(PMIC_VA18   , (PMIC_PSI)(PMIC_PSI_HP + i), (PMIC_LDO_PMOD)power_mode_setting[5][1 + i]);
        PMIC_VR_PSI_PMOD_CONFIG(PMIC_VUSB   , (PMIC_PSI)(PMIC_PSI_HP + i), (PMIC_LDO_PMOD)power_mode_setting[6][1 + i]);
        PMIC_VR_PSI_PMOD_CONFIG(PMIC_VBT    , (PMIC_PSI)(PMIC_PSI_HP + i), (PMIC_LDO_PMOD)power_mode_setting[7][1 + i]);
        PMIC_VR_PSI_PMOD_CONFIG(PMIC_VA28   , (PMIC_PSI)(PMIC_PSI_HP + i), (PMIC_LDO_PMOD)power_mode_setting[8][1 + i]);
        PMIC_VR_PSI_PMOD_CONFIG(PMIC_VMC    , (PMIC_PSI)(PMIC_PSI_HP + i), (PMIC_LDO_PMOD)power_mode_setting[9][1 + i]);
    }
    PMIC_VR_PSI_PMOD_CONFIG(PMIC_VCAMA  , PMIC_PSI_HP, PMIC_LDO_PMOD_NORMAL);
    PMIC_VR_PSI_PMOD_CONFIG(PMIC_VIBR   , PMIC_PSI_HP, PMIC_LDO_PMOD_NORMAL);

    //Config Wakeup control mode
    for (i = 0; i < 2; i++) {
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VSF      , (PMIC_PSI)(PMIC_PSI_S0 + i), (PMIC_SLP_WKUP_MODE)power_mode_setting[2][5], (int)power_mode_setting[2][i + 6]);
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VIO28    , (PMIC_PSI)(PMIC_PSI_S0 + i), (PMIC_SLP_WKUP_MODE)power_mode_setting[3][5], (int)power_mode_setting[3][i + 6]);
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VA18     , (PMIC_PSI)(PMIC_PSI_S0 + i), (PMIC_SLP_WKUP_MODE)power_mode_setting[5][5], (int)power_mode_setting[5][i + 6]);
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VUSB     , (PMIC_PSI)(PMIC_PSI_S0 + i), (PMIC_SLP_WKUP_MODE)power_mode_setting[6][5], (int)power_mode_setting[6][i + 6]);
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VBT      , (PMIC_PSI)(PMIC_PSI_S0 + i), (PMIC_SLP_WKUP_MODE)power_mode_setting[7][5], (int)power_mode_setting[7][i + 6]);
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VA28     , (PMIC_PSI)(PMIC_PSI_S0 + i), (PMIC_SLP_WKUP_MODE)power_mode_setting[8][5], (int)power_mode_setting[8][i + 6]);
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VMC      , (PMIC_PSI)(PMIC_PSI_S0 + i), (PMIC_SLP_WKUP_MODE)power_mode_setting[9][5], (int)power_mode_setting[9][i + 6]);
    }
    PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_SWXM     , PMIC_PSI_S0, PMIC_SLP_WKUP_SW_CTL, 1);
    PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_SWDP     , PMIC_PSI_S0, PMIC_SLP_WKUP_SW_CTL, 1);
    PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_SWXM     , PMIC_PSI_S1, PMIC_SLP_WKUP_SW_CTL, 1);
    PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_SWDP     , PMIC_PSI_S1, PMIC_SLP_WKUP_SW_CTL, 1);

    //Config SW Control
    PMIC_VR_CONTROL(PMIC_VSF    , (PMIC_VR_CTL)power_mode_setting[2][0]);
    PMIC_VR_CONTROL(PMIC_VIO28  , (PMIC_VR_CTL)power_mode_setting[3][0]);
    PMIC_VR_CONTROL(PMIC_VA18   , (PMIC_VR_CTL)power_mode_setting[5][0]);
    PMIC_VR_CONTROL(PMIC_VUSB   , (PMIC_VR_CTL)power_mode_setting[6][0]);
    PMIC_VR_CONTROL(PMIC_VBT    , (PMIC_VR_CTL)power_mode_setting[7][0]);
    PMIC_VR_CONTROL(PMIC_VA28   , (PMIC_VR_CTL)power_mode_setting[8][0]);
    PMIC_VR_CONTROL(PMIC_VMC    , (PMIC_VR_CTL)power_mode_setting[9][0]);
    PMIC_VR_CONTROL(PMIC_VCAMA  , PMIC_VR_CTL_DISABLE);
    PMIC_VR_CONTROL(PMIC_VIBR   , PMIC_VR_CTL_DISABLE);
    PMIC_VR_CONTROL(PMIC_SWXM   , PMIC_VR_CTL_DISABLE);
    PMIC_VR_CONTROL(PMIC_SWDP   , PMIC_VR_CTL_DISABLE);
    PMIC_VR_CONTROL(PMIC_SWMP   , PMIC_VR_CTL_DISABLE);

    pmu_set_vrc_key_mt6385(PMIC_UNLOCK_KEY);;
    pmu_set_ovr_mode_mt6385(PMIC_ENTER_OVR_MODE);

    //RG_S0_IVGEN_OFF_EN = 1,S0 Disable IVGEN
    pmu_set_register_value(0x42  , 0x01    , 0x02   , 1);
    //RG_S1_IVGEN_OFF_EN = 1,S1 Disable IVGEN
    pmu_set_register_value(0x42  , 0x01    , 0x03   , 1);

#ifdef PMU_SWBAT_ALWAYS_ON
    //RG_S1,S0_VBATSW_OFF_EN = 0, PMU MTCMOS Always ON
    pmu_set_register_value(0x42  , 0x01    , 0x05   , 0);
    pmu_set_register_value(0x42  , 0x01    , 0x04   , 0);
#else
    //RG_S1,S0_VBATSW_OFF_EN = 1, PMU MTCMOS ON/OFF By LDO Condition
    pmu_set_register_value(0x42  , 0x01    , 0x05   , 1);
    pmu_set_register_value(0x42  , 0x01    , 0x04   , 1);
#endif

    pmu_set_ovr_mode_mt6385(PMIC_LEAVE_OVR_MODE);
    pmu_set_vrc_key_mt6385(PMIC_LOCK_KEY);

    PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VSF, PMIC_PSI_S0, PMIC_SLP_WKUP_AON, 0);
    PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VSF, PMIC_PSI_S1, PMIC_SLP_WKUP_AON, 0);
}
#endif /* MTK_EXTERNAL_PMIC */
#endif /* HAL_PMU_MODULE_ENABLED */
