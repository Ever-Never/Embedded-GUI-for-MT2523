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

#include "hal_platform.h"

#ifdef HAL_PMU_MODULE_ENABLED
#include "hal_pmu_wrap_interface.h"
#include "hal_log.h"

#ifdef MTK_EXTERNAL_PMIC
/*use external pmic*/
#include "external_pmic_port.h"
#else
/*use internal pmu*/
#include "hal_pmu.h"
#include "hal_pmu_internal.h"
#endif

#ifndef MTK_EXTERNAL_PMIC
/*use internal pmu porting function*/
static void internal_pmu_ctrl_power(unsigned char driver_name, unsigned char enable);
static void internal_pmu_audio_lock_voltage(void);
#endif

#ifdef MTK_EXTERNAL_PMIC
/*use external pmic*/
static pmu_wrap_api_t pmu_wrap_api = {
    external_pmic_init,                 /* 1.external pmic init function */
    null_pmu_set_register_value,        /* 2.external pmic null pmu set register function */
    null_pmu_get_register_value,        /* 3.external pmic null pmu get register function */
    null_function,                      /* 4.null function */
    external_pmic_get_vcore_voltage,    /* 5.external pmic get Vcore voltage setting function */
    external_pmic_set_vcore_voltage,    /* 6.external pmic set Vcore voltage setting function */
    null_function,                      /* 7.null function */
    external_pmic_ctrl_power,           /* 8.external pmic driver user power control function */
    external_pmic_audio_lock_voltage,   /* 9.external pmic Audio lock Vcore voltage before Deep Sleep function */
    external_pmic_audio_unlock_voltage, /* 10.external pmic Audio unlock Vcore voltage enter Deep Sleep function */
    external_pmic_get_usb_input_status, /* 11.Get USB input status for USB detect */
    external_pmic_enter_power_off,      /* 12.Enter PMU Power OFF Mode */
    external_pmic_reset                 /* 13.PMU/PMIC hard reset */
};
#else
/*use internal pmu*/
static pmu_wrap_api_t pmu_wrap_api = {
    pmu_init_mt6385,                    /* 1.MT2523 internal pmu init function */
    pmu_set_register_value_mt6385,      /* 2.MT2523 internal pmu set register function */
    pmu_get_register_value_mt6385,      /* 3.MT2523 internal pmu get register function */
    pmu_control_mtcmos_mt6385,          /* 4.MT2523 internal pmu mtcmos control function */
    pmu_get_vcore_voltage_mt6385,       /* 5.MT2523 internal pmu get Vcore voltage setting function */
    pmu_set_vcore_voltage_mt6385,       /* 6.MT2523 internal pmu set Vcore voltage setting function */
    pmu_set_vcore_s1_buck_mt6385,       /* 7.MT2523 internal pmu set S1 mode Vcore Buck Voltage */
    internal_pmu_ctrl_power,            /* 8.MT2523 internal pmu driver user power control function */
    internal_pmu_audio_lock_voltage,    /* 9.MT2523 internal pmu Audio lock Vcore voltage before Deep Sleep function */
    null_function,                      /* 10.null function */
    pmu_get_usb_input_status_mt6385,    /* 11.Get USB input status for USB detect */
    pmu_enter_power_off_mt6385,         /* 12.Enter PMU Power OFF Mode */
    pmu_reset_mt6385                    /* 13.PMU/PMIC hard reset */
};
#endif

/* 1.init function */
void pmu_init(void)
{
#ifdef MTK_EXTERNAL_PMIC
    log_hal_info("\n\ruse external pmic \n\r");
#else
    log_hal_info("\n\ruse internal pmic \n\r");
#endif
    pmu_wrap_api.init();
}

/* 2.Only For MT2523 PMU Set Reg function */
bool pmu_set_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value)
{
    return (pmu_wrap_api.set_reg_value(address, mask, shift, value));
}

/* 3.Only For MT2523 PMU get Reg function */
unsigned char pmu_get_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift)
{
    return (pmu_wrap_api.get_reg_value(address, mask, shift));
}

/* 4.Only For MT2523 Sleep Manager use */
void pmu_control_mtcmos(void)
{
    pmu_wrap_api.ctrl_mtcmos();
}

/* 5.For MT2523 or MT2533 Get Vcore Voltage Setting */
/*Driver Use : MSDC*/
unsigned char pmu_get_vcore_voltage()
{
    return (pmu_wrap_api.get_vcore_vol());
}

/* 6.For MT2523 or MT2533 Set Vcore Voltage Setting */
/*Driver Use : DVFS*/
/*Driver Use : USB*/
unsigned char pmu_set_vcore_voltage(unsigned char lock, unsigned char vcore)
{
    return (pmu_wrap_api.set_vcore_vol(lock, vcore));
}

/* 7.Only For MT2523 Sleep Manager */
void pmu_set_vcore_s1_buck(void)
{
    pmu_wrap_api.set_vcore_s1_buck();
}

/* 8.For MT2523 or MT2533 Driver Power Control */
void pmu_ctrl_driver_power(unsigned char driver_name, unsigned char enable)
{
    pmu_wrap_api.ctrl_driver_power(driver_name, enable);
}

/* 9.For MT2523 or MT2533 Sleep Manager use */
void pmu_audio_lock_voltage(void)
{
    pmu_wrap_api.audio_lock_voltage();
}

/* 10.Only For MT2533 Sleep Manager use */
void pmu_audio_unlock_voltage(void)
{
    pmu_wrap_api.audio_unlock_voltage();
}

/* 11.Get USB input status for USB detect */
unsigned char pmu_get_usb_input_status(void)
{
    return (pmu_wrap_api.get_usb_input_status());
}

/* 12.Enter PMU Power OFF Mode */
void pmu_enter_power_off(void)
{
    pmu_wrap_api.enter_power_off();
}

/* 13.PMU/PMIC hard reset */
void pmu_reset(void)
{
    pmu_wrap_api.reset();
}

unsigned int pmu_get_type()
{
#ifdef MTK_EXTERNAL_PMIC
    return(0);  /* Use External PMIC*/
#else
    return(1);  /* Use Internal PMU*/
#endif    
}

#ifndef MTK_EXTERNAL_PMIC
/*use internal pmu porting function*/
static void internal_pmu_ctrl_power(unsigned char driver_name, unsigned char enable)
{
    switch (driver_name) {
        case DRIVER_BT:
            pmu_ctrl_vbt_ldo_mt6385(enable);
            break;

        case DRIVER_AUDIO:
            pmu_ctrl_va28_ldo_mt6385(enable);
            break;

        case DRIVER_USB:
            PMIC_VR_CONTROL(PMIC_VUSB, (PMIC_VR_CTL)enable);
            break;

        case DRIVER_CAMERA:
            PMIC_VR_CONTROL(PMIC_VCAMA, (PMIC_VR_CTL)enable);
            break;

        case DRIVER_MSDC:
            PMIC_VR_CONTROL(PMIC_VMC, (PMIC_VR_CTL)enable);
            break;

        case DRIVER_ACCDET:
            pmu_ctrl_va28_ldo_mt6385(enable);
            break;

        case DRIVER_ADC:
            pmu_ctrl_va28_ldo_mt6385(enable);
            break;

        case DRIVER_DAC:
            pmu_ctrl_va28_ldo_mt6385(enable);
            break;

        default :
            log_hal_info("EXTERNAL PMU ERROR: No matching Power Control!\n");
            break;
    }
}

static void internal_pmu_audio_lock_voltage(void)
{
    PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_LP, PMIC_VCORE_1P1V);
    PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_S1, PMIC_VCORE_0P9V);
}
#endif

/*null function don't use*/
bool null_pmu_set_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value)
{
    return (true);
}

/*null function don't use*/
unsigned char null_pmu_get_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift)
{
    return (0);
}

/*null function don't use*/
void null_function(void)
{
}
#endif /* HAL_PMU_MODULE_ENABLED */
