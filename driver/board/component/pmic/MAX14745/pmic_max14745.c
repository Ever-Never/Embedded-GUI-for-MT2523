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

#include "pmic_max14745.h"
#include "external_pmic_driver.h"
#include "hal_i2c_pmic.h"
#include "hal_i2c_master.h"
#include "hal_i2c_master_internal.h"
#include "hal_log.h"
#include "hal_gpt.h"

void pmic_charger_restore_setup(void)
{
    /* Restore charger register setup
       The register is reset to default value upon CHGIN rising edge
    */

    /* Register lLimCntl */
    /* SysMin 3.6V  lLimCntl 500mA */
    external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x09, 0xFF, 0x00, 0x02);
    /* Register ChgCntlA */
    /* BatReChg -120mV  BatReg 4.2V  ChgEn Enable */
    external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x0A, 0xFF, 0x00, 0x27);
    /* Register ChgCntlB */
    /* VPChg 3.0V  IPChg 0.2*I_FChg  ChgDone 0.2*I_FChg */
    external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x0B, 0xFF, 0x00, 0x6A);
    /* Register ChTmr */
    /* ChgAutoStp Enable  ChgAutoReSta Enable  MtChgTmr 30min  FChgTmr 300min  PChgTmr 30min */
    external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x0C, 0xFF, 0x00, 0xE8);
    /* Register ThrmCfg */
    /* T1T2IFchg 1*I_FChg  T2T3IFchg 1*I_FChg  ThermEn Charging enable between T1~T3 */
    external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x18, 0xFF, 0x00, 0xFD);
    /* Register ThrmCfg */
    /* T3T4IFchg 1*I_FChg */
    external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x19, 0xFF, 0x00, 0x07);

}

unsigned char pmic_charger_get_charging_status(void)
{
    unsigned char chg_status;
    /* Register StatusA */
    /* Read ChgStat */
    chg_status = external_pmic_get_register_value(MAX14745_DEVICE_ID, 0x02, 0x07, 0x00);

    return chg_status;
}

unsigned char pmic_charger_get_charger_exist(void)
{
    unsigned char chg_exist;
    /* Register StatusB */
    /* Read UsbOk */
    chg_exist = external_pmic_get_register_value(MAX14745_DEVICE_ID, 0x03, 0x01, 0x03);

    return chg_exist;
}

void pmic_init_maxim(void)
{
    unsigned char chipid;

    chipid = external_pmic_get_register_value(MAX14745_DEVICE_ID, 0, 0xFF, 0);

    if (chipid == MAX14745_CHIP_ID) {
        log_hal_info("Use Maxim 14745 PMIC\r\n");
    } else {
        log_hal_info("Not Found Maxim PMIC Chip ID\r\n");
        return;
    }
}

external_pmic_status_t pmic_ctrl_power_maxim(external_pmic_maxim_vr_t power_name, external_pmic_vr_ctl_t enable)
{
    switch (power_name) {
        case EXT_PMIC_MAXIM_BUCK1:
            /*Buck1En[1:0]*/
            external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x0D, 0x03, 3, enable);
            break;

        case EXT_PMIC_MAXIM_BUCK2:
            /*Buck2En[1:0]*/
            external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x0F, 0x03, 3, enable);
            break;

        case EXT_PMIC_MAXIM_LDO1:
            /*LDO1En[1:0]*/
            external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x12, 0x03, 1, enable);
            break;

        case EXT_PMIC_MAXIM_LDO2:
            /*LDO2En[1:0]*/
            external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x14, 0x03, 1, enable);
            break;

        case EXT_PMIC_MAXIM_LDO3:
            /*LDO3En[1:0]*/
            external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x16, 0x03, 1, enable);
            break;

        default:
            log_hal_info("PMIC ERROR : No matching LDO!\n");
            return (EXT_PMIC_STATUS_ERROR);
    }
    return (EXT_PMIC_STATUS_SUCCESS);
}

external_pmic_status_t pmic_vosel_config_maxim(external_pmic_maxim_vr_t power_name, external_pmic_voltage_t voltage)
{
    external_pmic_status_t status = EXT_PMIC_STATUS_SUCCESS;
    unsigned char vosel;

    switch (power_name) {
        case EXT_PMIC_MAXIM_BUCK1:
            /*Buck1VSet[5:0]*/
            if ((voltage >= EXT_PMIC_VOL_0P8V) || (voltage <= EXT_PMIC_VOL_2P3V)) {
                vosel = (voltage - 1) * 4; /*Linear Scale from 0.8V to 2.375V in 25mV increments*/
                external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x0E, 0x3F, 0, vosel);
                hal_gpt_delay_ms(1);
                hal_gpt_delay_us(400); 
            } else {
                log_hal_info("PMIC ERROR : Illegal voltage Setting!\n");
                status = EXT_PMIC_STATUS_INVALID_PARAMETER;
            }
            break;

        case EXT_PMIC_MAXIM_BUCK2:
            /*Buck2VSet[5:0]*/
            if ((voltage >= EXT_PMIC_VOL_0P8V) || (voltage <= EXT_PMIC_VOL_3P3V)) {
                vosel = (voltage - 1) * 2; /*Linear Scale from 0.8V to 3.95V in 50mV increments*/
                external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x10, 0x3F, 0, vosel);
            } else {
                log_hal_info("PMIC ERROR : Illegal voltage Setting!\n");
                status = EXT_PMIC_STATUS_INVALID_PARAMETER;
            }
            break;

        case EXT_PMIC_MAXIM_LDO1:
            /*LDO1VSet[4:0]*/
            if ((voltage >= EXT_PMIC_VOL_0P8V) || (voltage <= EXT_PMIC_VOL_3P3V)) {
                /*Linear Scale from 0.8V to 3.6V in 100mV increments*/
                vosel = (voltage - 1);
                external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x13, 0x1F, 0, vosel);
            } else {
                log_hal_info("PMIC ERROR : Illegal voltage Setting!\n");
                status = EXT_PMIC_STATUS_INVALID_PARAMETER;
            }
            break;

        case EXT_PMIC_MAXIM_LDO2:
            /*LDO2VSet[4:0]*/
            if ((voltage >= EXT_PMIC_VOL_0P9V) || (voltage <= EXT_PMIC_VOL_3P3V)) {
                vosel = voltage - 2; /*Linear Scale from 0.9V to 4.0V in 100mV increments*/
                external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x15, 0x1F, 0, vosel);
            } else {
                log_hal_info("PMIC ERROR : Illegal voltage Setting!\n");
                status = EXT_PMIC_STATUS_INVALID_PARAMETER;
            }
            break;

        case EXT_PMIC_MAXIM_LDO3:
            /*LDO3VSet[4:0]*/
            if ((voltage >= EXT_PMIC_VOL_0P9V) || (voltage <= EXT_PMIC_VOL_3P4V)) {
                vosel = voltage - 2; /*Linear Scale from 0.9V to 4.0V in 100mV increments*/
                external_pmic_set_register_value(MAX14745_DEVICE_ID, 0x17, 0x1F, 0, vosel);
            } else {
                log_hal_info("PMIC ERROR : Illegal voltage Setting!\n");
                status = EXT_PMIC_STATUS_INVALID_PARAMETER;
            }
            break;

        default:
            log_hal_info("PMIC ERROR: No matching LDO!\n");
            status = EXT_PMIC_STATUS_ERROR;
            break;
    }
    return (status);
}
