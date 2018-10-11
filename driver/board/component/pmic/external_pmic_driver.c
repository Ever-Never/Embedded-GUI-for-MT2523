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

#include "external_pmic_driver.h"
#include "hal_i2c_pmic.h"
#include "hal_i2c_master.h"
#include "hal_i2c_master_internal.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal.h"

/*internal use*/
unsigned char external_pmic_i2c_read(unsigned char i2c_address, unsigned char *ptr_send, unsigned char *ptr_read)
{
    pmic_i2c_send_to_receive_config_t config;
    unsigned char retry_cnt = 0, result_read;

    config.slave_address      = i2c_address;
    config.send_data          = ptr_send;
    config.send_length        = 1;
    config.receive_buffer     = ptr_read;
    config.receive_length     = 1;

    do {
        result_read = pmic_i2c_send_to_receive_polling(PMIC_I2C_MASTER, &config);
        retry_cnt++;
    } while ((result_read != 0) && (retry_cnt <= 10));

    retry_cnt--;
    return (retry_cnt);
}

/*internal use*/
unsigned char external_pmic_i2c_write(unsigned char i2c_address, unsigned char *ptr_send)
{
    unsigned char retry_cnt = 0, result_read;

    do {
        result_read = pmic_i2c_send_polling(PMIC_I2C_MASTER, i2c_address, ptr_send, 2);
        retry_cnt++;
    } while ((result_read != 0) && (retry_cnt <= 10));

    retry_cnt--;
    return (retry_cnt);
}

void external_pmic_i2c_init(void)
{
    pmic_i2c_config_t config;
    config.transfer_frequency = 400;
    pmic_i2c_init(PMIC_I2C_MASTER, &config);
}

bool external_pmic_set_register_value(unsigned char i2c_address, unsigned char address, unsigned char mask, unsigned char shift, unsigned char value)
{
    unsigned char send_buffer[2], receive_buffer[1];
    unsigned int mask_pri;

    send_buffer[0] = address;

    mask_pri = save_and_set_interrupt_mask();

    external_pmic_i2c_read(i2c_address, send_buffer, receive_buffer);

    receive_buffer[0] &= (~(mask << shift));
    send_buffer[1] = receive_buffer[0] | (value << shift); //data value

    if (external_pmic_i2c_write(i2c_address, send_buffer) == 0) {
        restore_interrupt_mask(mask_pri);
        return true;
    } else {
        restore_interrupt_mask(mask_pri);
        return false;
    }
}

unsigned char external_pmic_get_register_value(unsigned char i2c_address, unsigned char address, unsigned char mask, unsigned char shift)
{
    unsigned char send_buffer[1], receive_buffer[1];
    unsigned int mask_pri;

    send_buffer[0] = address;

    mask_pri = save_and_set_interrupt_mask();
    external_pmic_i2c_read(i2c_address, send_buffer, receive_buffer);
    restore_interrupt_mask(mask_pri);
    return ((receive_buffer[0] >> shift)&mask);
}

