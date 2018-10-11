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

#include "ctp.h"
#include "bsp_ctp.h"
#include "hal_log.h"

bool  ctp_is_force_upgrade_firmware = false;       /*force update firmware*/
bsp_ctp_status_t bsp_ctp_init(void)
{
    if (ctp_config_info.is_external_initialized != false) {

        return BSP_CTP_ERROR;
    }

    if (touch_panel_init() == true) {
        ctp_config_info.is_external_initialized = true;
        log_hal_info("[CTP][BSP]initialize touch [OK]\r\n");

        return BSP_CTP_OK;
    } else {
        ctp_config_info.is_external_initialized = false;
        log_hal_info("[CTP][BSP]initialize touch [fail]\r\n");
        return BSP_CTP_ERROR;
    }
}

bsp_ctp_status_t bsp_ctp_deinit(void)
{
    if (ctp_context.send_touch_ilm != true) {   // the buffer have date not to be get out
        return BSP_CTP_ERROR;
    }

    if (ctp_hw_i2c_deinit() == true) {

        touch_panel_disable();
        touch_panel_reset_data();

        return BSP_CTP_OK;
    } else {
        return BSP_CTP_ERROR;
    }
}


bsp_ctp_status_t bsp_ctp_register_callback(bsp_ctp_callback_t ctp_callback, void *user_data)
{

    if (ctp_callback == NULL)   {
        return BSP_CTP_INVALID_PARAMETER;
    }

    if (ctp_config_info.is_external_initialized  != true) {

        return BSP_CTP_ERROR;
    }
    ctp_context.user_callback.callback = ctp_callback;
    ctp_context.user_callback.pdata    = user_data;

    /*umask eint and open nvic eint irq*/
    touch_panel_enable();

    return BSP_CTP_OK;
}

bsp_ctp_status_t bsp_ctp_get_event_data(bsp_ctp_multiple_event_t *touch_event)
{
    bool        ret = false;


    if (touch_event == NULL) {
        return BSP_CTP_INVALID_PARAMETER;
    }

    ret = touch_panel_capacitive_get_event(touch_event);

    /*check if data padding is right */
    if (touch_event->padding != CTP_PATTERN) {
        return BSP_CTP_ERROR;
    }

    /*when ret is false, buffer is empty */
    if (ret == false) {
        ctp_debug_printf("[CTP][BSP]no data in buffer\r\n");
        return BSP_CTP_EVENT_EMPTY;
    }

    return BSP_CTP_OK;
}

bsp_ctp_status_t bsp_ctp_flush_buffer(void)
{
    touch_panel_multiple_touch_flush_buffer();

    return BSP_CTP_OK;
}

bsp_ctp_status_t bsp_ctp_set_event_time(bsp_ctp_event_time_t *event_time)
{
    if (event_time == NULL) {
        return BSP_CTP_INVALID_PARAMETER;
    }

    ctp_context.pen_longtap_time = event_time->pen_longtap_time;
    ctp_context.pen_repeat_time  = event_time->pen_repeat_time;

    return BSP_CTP_OK;
}

bsp_ctp_status_t bsp_ctp_set_sample_period(bsp_ctp_gpt_sample_period_t *sample_period)
{
    if (sample_period == NULL) {
        return BSP_CTP_INVALID_PARAMETER;
    }

    ctp_context.timer_sample_rate.high_sample_period = sample_period->high_sample_period;

    ctp_context.timer_sample_rate.low_sample_period = sample_period->low_sample_period;

    ctp_context.is_sample_rate_configured = sample_period->is_used_hardware_polling;

    log_hal_info("[CTP][BSP]Set high_sample_period = %d, low_sample_period = %d, hardware_polling=%d\r\n", \
                 sample_period->high_sample_period, \
                 sample_period->low_sample_period, \
                 sample_period->is_used_hardware_polling);

    return BSP_CTP_OK;

}

bsp_ctp_status_t bsp_ctp_power_on(bool on)
{
    touch_panel_capacitive_power_on(on);

    return BSP_CTP_OK;

}

bsp_ctp_status_t bsp_ctp_force_upgrade(bool is_force_upgrade)
{
    ctp_is_force_upgrade_firmware =  is_force_upgrade;

    return BSP_CTP_OK;
}


#endif /*MTK_CTP_ENABLE*/


