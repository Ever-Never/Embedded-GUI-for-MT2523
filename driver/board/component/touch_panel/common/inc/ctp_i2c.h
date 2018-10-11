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

#ifndef _CTP_I2C_H_
#define _CTP_I2C_H_

#ifdef MTK_CTP_ENABLE

#include <stdint.h>
#include <stdbool.h>
#include "hal_i2c_master.h"
#include "hal_gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    CTP_USE_HARDWARE_I2C = 0,
    CTP_USE_SOFTWARE_I2C,
} ctp_i2c_type_t;


typedef struct {
    bool 		   		has_initialized;
    hal_i2c_frequency_t speed;
    ctp_i2c_type_t 		i2c_type;
    hal_i2c_port_t		i2c_port;
    hal_gpio_pin_t		sda_pin;
    hal_gpio_pin_t 		scl_pin;
} ctp_i2c_config_t;

/********* varible extern *************/
extern ctp_i2c_config_t *ctp_i2c_state;

/******** funtion extern **************/
bool ctp_hw_i2c_init(ctp_i2c_config_t *i2c_config);
bool ctp_hw_i2c_deinit(void);
bool ctp_i2c_bottom_write(uint8_t device_addr, uint16_t ctp_reg_addr, uint8_t *data_buffer, uint32_t data_length);
bool ctp_i2c_bottom_read(uint8_t device_addr, uint16_t ctp_reg_addr, uint8_t *data_buffer, uint32_t data_length);
bool ctp_sw_i2c_read(uint8_t slave_adress, uint16_t reg_address, uint8_t *data, uint32_t length);
bool ctp_sw_i2c_write(uint8_t slave_adress, uint16_t reg_address, uint8_t *data, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /*MTK_CTP_ENABLE*/

#endif /*_CTP_I2C_H_*/

