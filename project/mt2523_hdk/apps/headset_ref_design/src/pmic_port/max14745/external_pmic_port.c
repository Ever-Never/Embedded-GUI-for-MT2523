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

#include "external_pmic_port.h"
#include "pmic_max14745.h"
#include "external_pmic_driver.h"
#include "hal_eint.h"
#include "hal_log.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_gpio.h"

#ifdef MTK_SMART_BATTERY_ENABLE
extern void battery_charger_detect_handler(void);
extern void battery_charger_status_handler(void);
#endif

void external_pmic_init(void)
{
    external_pmic_i2c_init();

    pmic_init_maxim();

    external_pmic_eint_init();

    /*default USB Power off*/
    pmic_ctrl_power_maxim(EXT_PMIC_MAXIM_LDO3, EXT_PMIC_VR_DISABLE);
}

void external_pmic_ctrl_power(unsigned char driver_name, unsigned char enable)
{
    switch (driver_name) {
        case DRIVER_BT:
            /*BT use MAX14745 LDO2-2.8V*/
            break;

        case DRIVER_AUDIO:
            /*Audio use MAX14745 LDO2-2.8V*/
            break;

        case DRIVER_USB:
            /*USB use MAX14745 LDO3-3.3V*/
            pmic_ctrl_power_maxim(EXT_PMIC_MAXIM_LDO3, (external_pmic_vr_ctl_t)enable);
            break;

        case DRIVER_CAMERA:

            break;

        case DRIVER_MSDC:
            /*MSDC use external LDO,Need Use GPIO Control LDO Power*/
            if (enable) {
                hal_gpio_set_output(HAL_GPIO_45, HAL_GPIO_DATA_HIGH);
            } else {
                hal_gpio_set_output(HAL_GPIO_45, HAL_GPIO_DATA_LOW);
            }
            break;

        case DRIVER_ACCDET:

            break;

        case DRIVER_ADC:

            break;

        case DRIVER_DAC:

            break;

        default :
            log_hal_info("EXTERNAL PMIC ERROR: No matching Power Control!\n");
            break;
    }
}

external_pmic_status_t external_pmic_vosel_config(platform_device_t driver_name, external_pmic_voltage_t voltage)
{
    external_pmic_status_t status = EXT_PMIC_STATUS_SUCCESS;
    switch (driver_name) {
        case DRIVER_DVFS:
            /*Vcore use Buck1*/
            if (pmic_vosel_config_maxim(EXT_PMIC_MAXIM_BUCK1, voltage) != EXT_PMIC_STATUS_SUCCESS) {
                status = EXT_PMIC_STATUS_ERROR;
            }
            break;

        default :
            log_hal_info("EXTERNAL PMIC ERROR: No matching Power Control!\n");
            break;
    }

    return (status);
}

void external_pmic_eint_init(void)
{
    hal_eint_config_t config;
    config.trigger_mode = HAL_EINT_EDGE_RISING;
    config.debounce_time = 0;

    hal_eint_init(HAL_EINT_PMIC, &config);    /*set EINT trigger mode and debounce time.*/
    hal_eint_register_callback(HAL_EINT_PMIC, external_pmic_eint_handler, NULL); /*register a user callback.*/

    hal_eint_unmask(HAL_EINT_PMIC);

    /*IntMaskA Register (0x07)*/
    /*unmask UsbOk interrupt*/
    external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x07, 0x01, 3, 1);
    /*unmask ChgStatInt interrupt*/
    external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x07, 0x01, 6, 1);

    external_pmic_get_register_value(MAX14745_DEVICE_ID, 0x05, 0xFF, 0);
}
#ifdef MTK_USB_DEMO_ENABLED
void usb_cable_detect(void);
#endif
void external_pmic_eint_handler(void *parameter)
{
    unsigned int mask;
    unsigned char interrupt_status;

    hal_eint_mask(HAL_EINT_PMIC);
    mask = save_and_set_interrupt_mask();

    /*IntA Register (0x05)*/
    interrupt_status = external_pmic_get_register_value(MAX14745_DEVICE_ID, 0x05, 0xFF, 0);

    /*UsbOk : Change in UsbOk caused interrupt.*/
    if ((interrupt_status & 0x08) == 0x08) {
#ifdef MTK_SMART_BATTERY_ENABLE
        battery_charger_detect_handler();
#endif
        /*Read StatusB Register (0x03) - Check USB(CHGIN Input Status)*/
        if (external_pmic_get_register_value(MAX14745_DEVICE_ID, 0x03, 0x1, 3) == 1) {
            /*CHGIN Input is present and valid*/
            #ifdef MTK_USB_DEMO_ENABLED
            usb_cable_detect();
            #endif            
        } else {
            /*CHGIN Input is not present or outside of valid range*/
            #ifdef MTK_USB_DEMO_ENABLED
            usb_cable_detect();
            #endif  
        }
    }
    /*ChgStatInt : Change in ChgStat caused interrupt, or first detection complete after POR.*/
    if ((interrupt_status & 0x40) == 0x40) {
#ifdef MTK_SMART_BATTERY_ENABLE
        battery_charger_status_handler();
#endif
    }
    restore_interrupt_mask(mask);
    hal_eint_unmask(HAL_EINT_PMIC);
}

unsigned char external_pmic_set_vcore_voltage(unsigned char lock, unsigned char vcore_vosel)
{
    static unsigned char Vcore_Resource_Ctrl[7], init = 0;
    static external_pmic_voltage_t origin_voltage, old_vcore;
    int vol_index;

    if (init == 0) {
        old_vcore = (external_pmic_voltage_t)external_pmic_get_vcore_voltage();
        old_vcore = origin_voltage;
        init = 1;
    }
    /* parameter check */
    if ((vcore_vosel > EXT_PMIC_VOL_1P3V) || (vcore_vosel < EXT_PMIC_VOL_0P9V)) {
        return (EXT_PMIC_VOL_ERROR);
    }

    if (lock == EXT_PMIC_VCORE_LOCK) {
        Vcore_Resource_Ctrl[vcore_vosel]++;
    } else {
        if (Vcore_Resource_Ctrl[vcore_vosel] != 0) {
            Vcore_Resource_Ctrl[vcore_vosel]--;
        }
    }
    //Find Highest Vcore Voltage
    for (vol_index = EXT_PMIC_VOL_1P3V; vol_index >= EXT_PMIC_VOL_0P7V; vol_index--) {
        if (Vcore_Resource_Ctrl[vol_index] != 0) {
            break;
        }
    }
    if (vol_index < EXT_PMIC_VOL_0P7V) {
        //Cna't find any Vcore Ctrl request
        vol_index = origin_voltage;
    }

    if (vol_index != old_vcore) {
        old_vcore = (external_pmic_voltage_t)vol_index;
        /*Set Vcore Voltege*/
        external_pmic_vosel_config(DRIVER_DVFS, (external_pmic_voltage_t)vol_index);
    }
    return (vol_index);
}

unsigned char external_pmic_get_vcore_voltage(void)
{
    unsigned char vcore;

    /*Buck1VSet[5:0]*/
    vcore = external_pmic_get_register_value(MAX14745_DEVICE_ID, 0x0E, 0x3F, 0);
    /*Linear Scale from 0.8V to 2.375V in 25mV increments*/
    vcore = (vcore / 4) + 1;

    return (vcore);
}

void external_pmic_audio_lock_voltage(void)
{
    //Lock Vcore Voltage 1.0V
    external_pmic_set_vcore_voltage(EXT_PMIC_VCORE_LOCK, EXT_PMIC_VOL_1P0V);
}

void external_pmic_audio_unlock_voltage(void)
{
    //Unlock Vcore Voltage 1.0V
    external_pmic_set_vcore_voltage(EXT_PMIC_VCORE_UNLOCK, EXT_PMIC_VOL_1P0V);
}

unsigned char external_pmic_get_usb_input_status(void)
{
    unsigned char ret;
    
    //read max14745 usb status
    ret = external_pmic_get_register_value(MAX14745_DEVICE_ID, 0x03, 0x1, 3);   

    return(ret);
}

void external_pmic_enter_power_off(void)
{
    external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x1F, 0xFF,0 , 0xB2);    
}

void external_pmic_reset(void)
{
    external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x1F, 0xFF,0 , 0xC3);   
}
