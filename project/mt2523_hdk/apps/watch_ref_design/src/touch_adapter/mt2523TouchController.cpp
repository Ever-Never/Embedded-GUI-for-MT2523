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
 
#include "mt2523TouchController.hpp"
#include "hal_log.h"
#include "bsp_ctp.h"
#include "ctp.h"

using namespace touchgfx;

#define TOUCH_ADAPTER_EVENT_UP          false
#define TOUCH_ADAPTER_EVENT_DOWN        true

typedef struct {
    int32_t x;
    int32_t y;
    bool state;
} touch_adapter_point_t;

touch_adapter_point_t touch_adapter_point;

void touch_adapter_callback(void) 
{

}
void touch_adapter_init(void)
{
    bsp_ctp_gpt_sample_period_t config;
    
    bsp_ctp_init();
    
    touch_adapter_point.state       = TOUCH_ADAPTER_EVENT_UP;

    /*set to 20ms polling hardware*/
    config.high_sample_period       = 20;
    config.low_sample_period        = 20;
    config.is_used_hardware_polling = true;
    bsp_ctp_set_sample_period(&config);
    
    
    bsp_ctp_register_callback((bsp_ctp_callback_t)touch_adapter_callback, NULL);

    log_hal_info("[CTP][adapter] touch_adapter_init \r\n");
}

bool touch_adapter_get_data(int32_t *point_x, int32_t* point_y)
{
    bsp_ctp_multiple_event_t touch_event;   
    bsp_ctp_status_t         ret;
    bool                     update_flag = false;
    static bool is_power_on = false;

    /*once the touchGFX to get data, it will get all of point data out*/

    if (ctp_context.is_power_on == false) {
        *point_x = touch_adapter_point.x;
        *point_y = touch_adapter_point.y;
        if (is_power_on == false) {
            bsp_ctp_flush_buffer();
            is_power_on = true;
        }
        return touch_adapter_point.state;
    }
    else {
        is_power_on = false;
    }

    
    while(1) {
        ret = bsp_ctp_get_event_data(&touch_event);
        if (ret == BSP_CTP_OK) { 

            /*if have get up or down, do not update point, waiting to all point to be get out*/
            if (update_flag == false) {
                touch_adapter_point.x = (int32_t)touch_event.points[0].x;
                touch_adapter_point.y = (int32_t)touch_event.points[0].y;
            }
            
            /*if there is up or abort, release event*/
            if (touch_event.points[0].event == CTP_PEN_DOWN ){
                /*if there is down, press event*/
                touch_adapter_point.state = TOUCH_ADAPTER_EVENT_DOWN;
                update_flag = true;
                break;
            }
            else if ((touch_event.points[0].event == CTP_PEN_UP)) {
                touch_adapter_point.state = TOUCH_ADAPTER_EVENT_UP;
                update_flag = true;
                break;
            }
            else if (touch_event.points[0].event == CTP_PEN_ABORT) {
                bsp_ctp_flush_buffer();
                break;
            }
            else {
                /*others, while loop to get point until the all points are get out. */
            }
        }
        else { /* no point in buffer, keep last point data and unchange state*/
            break;
        }
    }

    *point_x = touch_adapter_point.x;
    *point_y = touch_adapter_point.y;

    /*log_hal_info("[CTP][adapter]get point.x = %x, point.y = %x, state = %d\r\n", \
        touch_adapter_point.x, \
        touch_adapter_point.y, \
        touch_adapter_point.state);*/
    
    return touch_adapter_point.state;
    
}

void mt2523TouchController::init()
{
    touch_adapter_init();
}

bool mt2523TouchController::sampleTouch(int32_t& x, int32_t& y)
{
    bool    ret = false;
    int32_t ret_x = 0;
    int32_t ret_y = 0;

    ret = touch_adapter_get_data(&ret_x, &ret_y);

    x = ret_x;
    y = ret_y;
    
    return ret;
}

