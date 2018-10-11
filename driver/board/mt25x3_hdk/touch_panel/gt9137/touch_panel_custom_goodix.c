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

#ifdef MTK_CTP_ENABLE
#include "touch_panel_custom_goodix.h"
#include "ctp.h"
#include "assert.h"
#include "hal_log.h"

extern const unsigned char BSP_CTP_EINT;
extern const char BSP_CTP_RST_PIN;
extern const char BSP_CTP_EINT_PIN;
extern const char BSP_CTP_EINT_PIN_M_GPIO;
extern const char BSP_CTP_EINT_PIN_M_EINT;
//extern const char BSP_CTP_SW_I2C_SCL_PIN;
//extern const char BSP_CTP_SW_I2C_SCL_SDA;


void touch_panel_goodix_custom_data_init(void)
{
    ctp_context.x_coord_start            = CTP_GOODIX_COORD_X_START;
    ctp_context.x_coord_end              = CTP_GOODIX_COORD_X_END;
    ctp_context.y_coord_start            = CTP_GOODIX_COORD_Y_START;
    ctp_context.y_coord_end              = CTP_GOODIX_COORD_Y_END;

    ctp_context.verndor_x_coord_start    = CTP_GOODIX_COORD_VENDOR_X_START;
    ctp_context.verndor_x_coord_end      = CTP_GOODIX_COORD_VENDOR_X_END;
    ctp_context.verndor_y_coord_start    = CTP_GOODIX_COORD_VENDOR_Y_START;
    ctp_context.verndor_y_coord_end      = CTP_GOODIX_COORD_VENDOR_Y_END;

    ctp_context.pen_move_offset          = GOODIX_PEN_MOVE_OFFSET;
    ctp_context.pen_longtap_offset       = GOODIX_PEN_LONGTAP_OFFSET;

    if ((BSP_CTP_RST_PIN == 0xff)  || \
            (BSP_CTP_EINT_PIN == 0xff) || \
            (BSP_CTP_EINT == 0xff)) {
        log_hal_info("[CTP] ept tool pin setting error\r\n");
        log_hal_info("[CTP]BSP_CTP_RST_PIN= %x, BSP_CTP_EINT_PIN=%x, BSP_CTP_EINT=%x\r\n", \
                     BSP_CTP_RST_PIN, BSP_CTP_EINT_PIN, BSP_CTP_EINT);
        assert(0);
    }

    if (ctp_config_info.is_external_initialized == false) {
        if (ctp_config_info.ctp_i2c.has_initialized != true) {
            /* i2c conifg information */
            ctp_config_info.ctp_i2c.i2c_type                   = CTP_USE_HARDWARE_I2C;
            //ctp_config_info.ctp_i2c.scl_pin                    = (hal_gpio_pin_t)BSP_CTP_SW_I2C_SCL_PIN;
            //ctp_config_info.ctp_i2c.sda_pin                    = (hal_gpio_pin_t)BSP_CTP_SW_I2C_SCL_SDA;
            ctp_config_info.ctp_i2c.speed                      = HAL_I2C_FREQUENCY_400K;
            ctp_config_info.ctp_i2c.i2c_port                   = HAL_I2C_MASTER_0;
            ctp_config_info.reg_address_byte                   = 2;
        }

        if (ctp_config_info.ctp_eint.has_initialized != true) {
            /* eint conifg information */
            ctp_config_info.ctp_eint.eint_pin                  = (hal_gpio_pin_t)BSP_CTP_EINT_PIN;
            ctp_config_info.ctp_eint.eint_num                  = (hal_eint_number_t)BSP_CTP_EINT;
            ctp_config_info.ctp_eint.eint_mode                 = (uint8_t)BSP_CTP_EINT_PIN_M_EINT;
            ctp_config_info.ctp_eint.gpio_mode                 = (uint8_t)BSP_CTP_EINT_PIN_M_GPIO;
            ctp_config_info.ctp_eint.init_config.trigger_mode  = HAL_EINT_EDGE_RISING;
            ctp_config_info.ctp_eint.init_config.debounce_time = 0;
            ctp_config_info.ctp_eint.has_initialized           = true;
        }

        /* reset conifg information */
        ctp_config_info.reset_pin = (hal_gpio_pin_t)BSP_CTP_RST_PIN;
    }
    log_hal_info("[CTP]get ept tool pin setting parameter \r\n");

}

#endif /*MTK_CTP_ENABLE*/

