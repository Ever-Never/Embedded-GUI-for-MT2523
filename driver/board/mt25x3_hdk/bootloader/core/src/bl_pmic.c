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

#include "bl_pmic.h"
#include "bl_common.h"

void (*bl_pmic_vusb_vr_control)(unsigned char enable);
void (*bl_pmic_vmc_vr_control)(unsigned char enable);

void bl_pmic_init(void)
{
    pmic_i2c_config_t config;
    volatile unsigned int wdt_status = 0;

    /*D2D I2C init*/
    config.transfer_frequency = 100;
    pmic_i2c_init(PMIC_I2C_MASTER, &config);

    /*Find pmic chip id*/
    if(bl_mt6385_init() == 0) {
        bl_print(LOG_DEBUG, "\n\rFind MT6385 PMIC CHIP ID \n\r");
        bl_pmic_vusb_vr_control = bl_mt6385_vusb_vr_control;
        bl_pmic_vmc_vr_control = bl_mt6385_vmc_vr_control;
        return;
    } else if(bl_max14745_init() == 0) {
        bl_print(LOG_DEBUG, "\n\rFind MAX14745 PMIC CHIP ID \n\r");
        bl_pmic_vusb_vr_control = bl_max14745_vusb_vr_control;
        bl_pmic_vmc_vr_control = bl_max14745_vmc_vr_control;
        /*Workaround for MT2533 WDT issue: If Vcore!=1.2V, WDT reset will make system hang, fail rate nearly 1% */
        /*0xa205000c is the register address for WDT status register on MT2533*/
        wdt_status = *(volatile unsigned int *)(0xa205000c);
        if (wdt_status != 0) {
            //This time system reset is caused by wdt, need PMIC reset to solve WDT issue.
            bl_print(LOG_DEBUG, "system reset by WDT \n\r");
            bl_pmic_set_register_value(BL_PMIC_I2C_ADDR_max14745  ,1  ,0x1f   ,0xff   ,0  ,0xc3);
        } else {
            bl_print(LOG_DEBUG, "system reset not by WDT \n\r");
        }
        return;
    }
#ifdef MTK_EXTERNAL_PMIC_TYPE_MT6327
    else if(bl_mt6327_init() == 0) {
        bl_print(LOG_DEBUG, "\n\rFind MT6327 PMIC CHIP ID \n\r");
        bl_pmic_vusb_vr_control = bl_mt6327_vusb_vr_control;
        return;
    }
#endif
    else {
        bl_print(LOG_DEBUG, "\n\rERROR : not Find any PMIC CHIP ID \n\r");
        bl_pmic_vusb_vr_control = bl_null_vusb_vr_control;
        return;
    }
}

bool bl_pmic_set_register_value(uint8_t i2c_address, uint8_t reg_addr_len, uint16_t reg_addr, uint8_t mask, uint8_t shift, uint8_t value)
{
    unsigned char send_buffer[3], receive_buffer[1];

    if(reg_addr_len == 1) {
        send_buffer[0] = reg_addr;
    }
    if(reg_addr_len == 2) {
        send_buffer[1] = (reg_addr)&0x00FF  ;
        send_buffer[0] = ((reg_addr >> 8)&0x00FF) | 0x40;
    }

    bl_pmic_i2c_read(i2c_address, reg_addr_len, send_buffer, receive_buffer);

    receive_buffer[0] &= (~(mask << shift));
    send_buffer[reg_addr_len] = receive_buffer[0] | (value << shift); //data value

    if(bl_pmic_i2c_write(i2c_address, reg_addr_len, send_buffer) == 0) {
        return true;
    } else {
        return false;
    }
}

unsigned char bl_pmic_get_register_value(uint8_t i2c_address, uint8_t reg_addr_len, uint16_t reg_addr, uint8_t mask, uint8_t shift)
{
    unsigned char send_buffer[2],receive_buffer[1];

    if(reg_addr_len == 1) {
        send_buffer[0] = reg_addr;
    }
    if(reg_addr_len == 2) {
        send_buffer[1] = (reg_addr)&0x00FF  ;
        send_buffer[0] = ((reg_addr >> 8)&0x00FF) | 0x40;
    }

    bl_pmic_i2c_read(i2c_address, reg_addr_len, send_buffer, receive_buffer);

    return ((receive_buffer[0] >> shift)&mask);
}


/*internal use*/
unsigned char bl_pmic_i2c_read(unsigned char i2c_address, unsigned char reg_addr_len, unsigned char *ptr_send, unsigned char *ptr_read)
{
    pmic_i2c_send_to_receive_config_t config;
    unsigned char retry_cnt = 0, result_read;

    config.slave_address      = i2c_address;
    config.send_data          = ptr_send;
    config.send_length        = reg_addr_len;
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
unsigned char bl_pmic_i2c_write(unsigned char i2c_address, unsigned char reg_addr_len, unsigned char *ptr_send)
{
    unsigned char retry_cnt = 0, result_read;

    do {
        result_read = pmic_i2c_send_polling(PMIC_I2C_MASTER, i2c_address, ptr_send, reg_addr_len+1);
        retry_cnt++;
    } while ((result_read != 0) && (retry_cnt <= 10));

    retry_cnt--;
    return (retry_cnt);
}

void bl_null_vusb_vr_control(unsigned char enable)
{
    return;
}
