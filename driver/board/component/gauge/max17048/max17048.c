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

#include "max17048.h"
#include "max17048_driver.h"
#include "max17048_common.h"
#include "external_ntc.h"
#include "syslog.h"
#include "hal_i2c_pmic.h"
#include "hal_i2c_master.h"
#include "hal_i2c_master_internal.h"
#include "hal_gpt.h"
#include "cust_max17048.h"

#ifdef  MAX17048_DEBUG_ENABLE
log_create_module(max17048, PRINT_LEVEL_INFO);
#else
log_create_module(max17048, PRINT_LEVEL_WARNING);
#endif

#define MAX17048_DEVICE_ID 0x36

/* MAXIM MAX17048 REG address */
#define MAX17048_REG_VCELL      0x02
#define MAX17048_REG_SOC        0x04
#define MAX17048_REG_MODE       0x06
#define MAX17048_REG_VERSION    0x08
#define MAX17048_REG_HIBRT      0x0A
#define MAX17048_REG_CONFIG     0x0C
#define MAX17048_REG_OCV        0x0E
#define MAX17048_REG_VALRT      0x14
#define MAX17048_REG_CRATE      0x16
#define MAX17048_REG_VRESET_ID  0x18
#define MAX17048_REG_STATUS     0x1A
#define MAX17048_REG_UNLOCK     0x3E
#define MAX17048_REG_TABLE      0x40
#define MAX17048_REG_RCOMPSEG1  0x80
#define MAX17048_REG_RCOMPSEG2  0x90
#define MAX17048_REG_CMD        0xFF

static unsigned char original_ocv_1, original_ocv_2;

static void unlock_model_access(void)
{
    max17048_write_word(MAX17048_DEVICE_ID, MAX17048_REG_UNLOCK, 0x4A, 0x57);
}

static void lock_model_access(void)
{
    max17048_write_word(MAX17048_DEVICE_ID, MAX17048_REG_UNLOCK, 0x00, 0x00);
}

static void prepare_to_load_model(void)
{

    do {
        unlock_model_access();
        max17048_read_word(MAX17048_DEVICE_ID, MAX17048_REG_OCV, &original_ocv_1, &original_ocv_2);
        MAX17048_DBG("Original OCV %02X %02X\r\n", original_ocv_1, original_ocv_2);
    } while ((original_ocv_1 == 0xFF) && (original_ocv_2 == 0xFF));

}

static void load_model(void)
{
    unsigned char index;
    unsigned char rcomp_seg_msb = (MAX17048_RCOMP_SEG >> 8) & 0xFF;
    unsigned char rcomp_seg_lsb = MAX17048_RCOMP_SEG & 0xFF;

    /* Write the Model */
    for (index = 0; index < 0x40; index = index + 2) {
        max17048_write_word(MAX17048_DEVICE_ID, MAX17048_REG_TABLE + index, max17048_model_data[index], max17048_model_data[index + 1]);
    }

    /* Write RCOMPSeg */
    for (index = 0; index < 0x20; index = index + 2) {
        max17048_write_word(MAX17048_DEVICE_ID, MAX17048_REG_RCOMPSEG1 + index, rcomp_seg_msb, rcomp_seg_lsb);
    }

}

static bool verify_model_is_correct(void)
{
    unsigned char ocv_test_msb = (MAX17048_OCV_TEST >> 8) & 0xFF;
    unsigned char ocv_test_lsb = MAX17048_OCV_TEST & 0xFF;
    unsigned char soc_msb = 0;
    unsigned char soc_lsb = 0;

    unsigned char hib_thr;
    unsigned char act_thr;

    hal_gpt_delay_ms(200);

    /* Write OCV Test Value */
    max17048_write_word(MAX17048_DEVICE_ID, MAX17048_REG_OCV, ocv_test_msb, ocv_test_lsb);

    /* Read Hibernate */
    max17048_read_word(MAX17048_DEVICE_ID, MAX17048_REG_HIBRT, &hib_thr, &act_thr);

    MAX17048_DBG("HibThr = %02X Act_Thr = %02X \r\n", hib_thr, act_thr);

    /* Diable Hibernate */
    max17048_write_word(MAX17048_DEVICE_ID, MAX17048_REG_HIBRT, 0x00, 0x00);
    /* lock_model_access */
    lock_model_access();

    hal_gpt_delay_ms(200);

    /* Read SOC and compare */
    max17048_read_word(MAX17048_DEVICE_ID, MAX17048_REG_SOC, &soc_msb, &soc_lsb);

    MAX17048_DBG("SOC = %02X %02X (SOC_CAHCK_A = %02X SOC_CAHCK_B = %02X )\r\n", \
                 soc_msb, soc_lsb, MAX17048_SOC_CAHECK_A, MAX17048_SOC_CAHECK_B);

    if (soc_msb >= MAX17048_SOC_CAHECK_A && soc_msb <= MAX17048_SOC_CAHECK_B) {
        return true;
    } else {
        return false;
    }
}

static void cleanup_model_load(void)
{
    unlock_model_access();

    /* Restore Config and OCV */
    max17048_set_register_value(MAX17048_DEVICE_ID, MAX17048_REG_CONFIG, 0xFF, 0x00, MAX17048_RCOMP0);
    max17048_write_word(MAX17048_DEVICE_ID, MAX17048_REG_OCV, original_ocv_1, original_ocv_2);
    max17048_write_word(MAX17048_DEVICE_ID, MAX17048_REG_HIBRT, MAX17048_HIB_THR, MAX17048_ACT_THR);

    lock_model_access();

    hal_gpt_delay_ms(200);

}

static void default_init_setting(void)
{
    /* Write MODE Register */
    max17048_write_word(MAX17048_DEVICE_ID, MAX17048_REG_MODE, MAX17048_MODE, 0x00);
    /* Disble analog comparator */
    max17048_set_register_value(MAX17048_DEVICE_ID, MAX17048_REG_VRESET_ID, 0x01, 0x00, 0x01);
}

static bool handle_model(void)
{
    bool model_load_ok = false;
    unsigned char retry_cnt = 0;

    do {
        prepare_to_load_model();
        load_model();
        model_load_ok = verify_model_is_correct();
        retry_cnt++;
    } while ((!model_load_ok) && (retry_cnt <= 10));

    cleanup_model_load();
    default_init_setting();

    if (!model_load_ok) {
        return false;
    }
    return true;
}

bool max17048_verify_model(void)
{
    /* Check the RI (reset indicator) */
    if (max17048_get_register_value(MAX17048_DEVICE_ID, MAX17048_REG_STATUS, 0x01, 0x00) == 1) {
        MAX17048_DBG("Reload MAX17048 model\r\n");
        if (!handle_model()) {
            MAX17048_ERR("Load MAX17048 model fail!!\r\n");
            return false;
        }
        max17048_set_register_value(MAX17048_DEVICE_ID, MAX17048_REG_STATUS, 0x01, 0x00, 0x00);
    } else {
        MAX17048_DBG("No need reload MAX17048 model\r\n");
    }
    return true;
}

void max17048_update_temperature(int temperature)
{
    float used_temp_co;
    int result;
    unsigned char new_rcomp;

    used_temp_co = temperature > 20 ? MAX17048_TEMP_CO_UP : MAX17048_TEMP_CO_DOWN;
    result = (int)(MAX17048_RCOMP0 + (temperature - 20) * used_temp_co);
    new_rcomp = (unsigned char)(result >= 0xFF ? 0xFF : (result <= 0 ? 0 : result));

    MAX17048_DBG("MAX17048 Temp = %d result  = %d new_rcomp  = 0x%02X\r\n", (int)temperature, (int)result, new_rcomp);

    max17048_set_register_value(MAX17048_DEVICE_ID, MAX17048_REG_CONFIG, 0xFF, 0x00, new_rcomp);

    MAX17048_DBG("MAX17048 read new_rcomp  = 0x%02X\r\n", max17048_get_register_value(MAX17048_DEVICE_ID, MAX17048_REG_CONFIG, 0xFF, 0x00));

}

unsigned int max17048_get_soc(void)
{
    unsigned char soc_msb = 0;
    unsigned char soc_lsb = 0;
    unsigned int soc_percent;
    /* Read SOC */
    max17048_read_word(MAX17048_DEVICE_ID, MAX17048_REG_SOC, &soc_msb, &soc_lsb);

#if (MAX17048_BITS == 18)
    soc_percent = ((soc_msb << 8) + soc_lsb) / 256;
#elif(MAX17048_BITS == 19)
    soc_percent = ((soc_msb << 8) + soc_lsb) / 512;
#else
    soc_percent = ((soc_msb << 8) + soc_lsb) / 256;
#endif

    return soc_percent;
}

unsigned int max17048_get_vcell(void)
{
    unsigned char vcell_msb;
    unsigned char vcell_lsb;
    unsigned int vcell;

    /* Read  Vcell */
    max17048_read_word(MAX17048_DEVICE_ID, MAX17048_REG_VCELL, &vcell_msb, &vcell_lsb);
    /* One step is 78.125uV */
    vcell = ((unsigned int)((vcell_msb << 8) + vcell_lsb) * 10) >> 7;

    return vcell;
}

bool max17048_init(void)
{
    unsigned char chip_ver_msb;
    unsigned char chip_ver_lsb;
    bool ret;

    external_ntc_init();

    max17048_i2c_init();

    ret = max17048_read_word(MAX17048_DEVICE_ID, MAX17048_REG_VERSION, &chip_ver_msb, &chip_ver_lsb);

    if (ret == false) {

        MAX17048_ERR("MAX17048 not detected !!");
        return false;
    }

    MAX17048_DBG("MAX17048 Ver = 0x%02X%02X\r\n", chip_ver_msb, chip_ver_lsb);

    ret = max17048_verify_model();

    default_init_setting();

    if (ret == false) {

        MAX17048_ERR("MAX17048 init fail!!");
        return false;
    }

    return true;

}


