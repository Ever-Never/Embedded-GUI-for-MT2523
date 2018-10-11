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

#include "mt25x3_hdk_lcd.h"

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED

#include "hal_display_lcd.h"
#include "hal_display_dsi.h"
#include "hal_display_dsi_internal.h"
#include "hal_cm4_topsm.h"
#include "hal_gpt.h"
#include "hal_log.h"

#ifdef FREERTOS_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
SemaphoreHandle_t lcd_sema = NULL;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_POLLING_TIMEOUT (20000)

volatile LCD_Funcs  *MainLCD = NULL;
extern volatile LCD_REGISTER_T	*lcd_register_ptr;
#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
extern volatile LCD_DSI_REGISTER_T	*lcd_dsi_register_ptr;
#endif
uint32_t isInited = 0;
lcm_config_para_t lcm_setting;
bsp_lcd_callback_t bsp_lcd_cb = NULL;

extern LCD_Funcs LCD_func_ST7789H2;
extern LCD_Funcs LCD_func_RM69032;

volatile bool isUpdate = false;

LCD_Funcs *lcm_list[] = {
    &LCD_func_ST7789H2,
    &LCD_func_RM69032
};

void bsp_lcd_callback(void)
{
    bsp_lcd_set_layer_to_default();
    bsp_lcd_power_off();
    bsp_lcd_unlock();

    if(bsp_lcd_cb != NULL) {
        bsp_lcd_cb(NULL);}
    isUpdate = false;
}

bsp_lcd_status_t bsp_lcd_init(uint16_t bgcolor)
{
    uint32_t frame_rate, back_porch, front_porch, width, height;
    uint32_t i;
    bool checkLCM = false;
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    if(isInited){
        return BSP_LCD_STATUS_OK;}

#ifdef FREERTOS_ENABLE
    if(lcd_sema == NULL) {
        vSemaphoreCreateBinary(lcd_sema);
        if(lcd_sema != NULL) {
            log_hal_info("lcd_sema create complete\n");
        } else {
            log_hal_info("Can't creare lcd_sema create\n");
            return BSP_LCD_STATUS_ERROR;
        }
    }
#endif

    bsp_lcd_power_on();

    for (i = 0; i < (sizeof(lcm_list) / sizeof(LCD_Funcs *)); i++) {
        MainLCD = lcm_list[i];
        MainLCD->IOCTRL(LCM_IOCTRL_QUERY__LCM_SETTING_PARAM, &lcm_setting);

        hal_display_lcd_init(lcm_setting.main_command_address, lcm_setting.main_data_address, lcm_setting.main_lcd_output);

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
        if (LCM_INTERFACE_TYPE_DSI == lcm_setting.type) {
            hal_display_dsi_init(false);
        }
#endif

        MainLCD->Init_lcd_interface();
        checkLCM = MainLCD->CheckID();

        if (true == checkLCM){
            break;}
    }

    MainLCD->Init(bgcolor);
    MainLCD->IOCTRL(LCM_IOCTRL_QUERY__FRAME_RATE, &frame_rate);
    MainLCD->IOCTRL(LCM_IOCTRL_QUERY__BACK_PORCH, &back_porch);
    MainLCD->IOCTRL(LCM_IOCTRL_QUERY__FRONT_PORCH, &front_porch);
    MainLCD->IOCTRL(LCM_IOCTRL_QUERY__LCM_WIDTH, &width);
    MainLCD->IOCTRL(LCM_IOCTRL_QUERY__LCM_HEIGHT, &height);

    hal_display_lcd_init_te(frame_rate, back_porch, front_porch, width, height, lcm_setting.main_lcd_output);

    hal_display_lcd_register_callback((hal_display_lcd_callback_t) bsp_lcd_callback);
    bsp_lcd_power_off();
    isInited = true;

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    if (LCM_INTERFACE_TYPE_DSI == lcm_setting.type){
        bsp_lcd_clear_screen(bgcolor);}
#endif

    return ret;
}

bsp_lcd_status_t bsp_lcd_set_layer_to_default(void)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    if (0 == isInited){
        bsp_lcd_init(0x0);}

    bsp_lcd_power_on();
    hal_display_lcd_set_layer_to_default();
    bsp_lcd_power_off();

    return ret;
}

bsp_lcd_status_t bsp_lcd_display_on(void)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    if (0 == isInited){
        bsp_lcd_init(0x0);}

    bsp_lcd_lock();
    bsp_lcd_power_on();
    MainLCD->ExitSleepMode();
    bsp_lcd_power_off();
    bsp_lcd_unlock();

    return ret;
}

bsp_lcd_status_t bsp_lcd_display_off(void)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    if (0 == isInited){
        bsp_lcd_init(0x0);}

    bsp_lcd_lock();
    bsp_lcd_power_on();
    MainLCD->EnterSleepMode();
    bsp_lcd_power_off();
    bsp_lcd_unlock();

    return ret;
}

bsp_lcd_status_t bsp_lcd_enter_idle(void)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    if (0 == isInited){
        bsp_lcd_init(0x0);}

    bsp_lcd_lock();
    bsp_lcd_power_on();
    MainLCD->EnterIdleMode();
    bsp_lcd_power_off();

#ifdef MTK_TE_ENABLE
{
    bool enable_TE = false;
    MainLCD->IOCTRL(LCM_IOCTRL_SET_TE, &enable_TE);
}
#endif
    bsp_lcd_unlock();

    return ret;
}

bsp_lcd_status_t bsp_lcd_exit_idle(void)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    if (0 == isInited){
        bsp_lcd_init(0x0);}

    bsp_lcd_lock();
    bsp_lcd_power_on();
    MainLCD->ExitIdleMode();
    bsp_lcd_power_off();

#ifdef MTK_TE_ENABLE
{
    bool enable_TE = true;
    MainLCD->IOCTRL(LCM_IOCTRL_SET_TE, &enable_TE);
}
#endif
    bsp_lcd_unlock();

    return ret;
}

bsp_lcd_status_t bsp_lcd_update_screen(uint32_t start_x,  uint32_t start_y, uint32_t end_x, uint32_t end_y)
{
    uint32_t width, height;
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    if (0 == isInited){
        bsp_lcd_init(0x0);}

    bsp_lcd_lock();

    isUpdate = true;

    bsp_lcd_power_on();
    MainLCD->IOCTRL(LCM_IOCTRL_QUERY__LCM_WIDTH, &width);
    MainLCD->IOCTRL(LCM_IOCTRL_QUERY__LCM_HEIGHT, &height);
    hal_display_lcd_calculate_te(width, height);
    hal_display_lcd_apply_setting();

    MainLCD->BlockWrite(start_x, start_y, end_x, end_y);

    return ret;
}

bsp_lcd_status_t bsp_lcd_clear_screen(uint16_t color)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    if (0 == isInited){
        bsp_lcd_init(0x0);}

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    uint32_t width, height;
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;
    hal_display_lcd_roi_output_t roi_para;
    uint32_t bg_color;

    if (LCM_INTERFACE_TYPE_DSI == lcm_setting.type) {
        MainLCD->IOCTRL(LCM_IOCTRL_QUERY__LCM_WIDTH, &width);
        MainLCD->IOCTRL(LCM_IOCTRL_QUERY__LCM_HEIGHT, &height);

        bg_color = ((color&(0xF800)) << 8) | ((color&(0x7E0)) << 5) | ((color&(0x1F)) << 3);
        roi_para.target_start_x = 0;
        roi_para.target_start_y = 0;
        roi_para.target_end_x = width - 1;
        roi_para.target_end_y = height - 1;
        roi_para.roi_offset_x = 0;
        roi_para.roi_offset_y = 0;
        roi_para.roi_background_color = bg_color;
        roi_para.main_lcd_output = LCM_8BIT_24_BPP_RGB888_1;

        bsp_lcd_config_roi(&roi_para);

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
        while(isUpdate) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
            hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
            if ((gpt_duration_count > LCD_POLLING_TIMEOUT) && isUpdate) {
                log_hal_info("Previous frame isn't complete\n");
                return BSP_LCD_STATUS_ERROR;
            }
        }

        isUpdate = true;

        bsp_lcd_power_on();
        hal_display_lcd_calculate_te(width, height);
        hal_display_lcd_apply_setting();
        MainLCD->BlockWrite(roi_para.target_start_x , roi_para.target_start_y , roi_para.target_end_x, roi_para.target_end_y);

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
        while(lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.FRAME_DONE_INT_FLAG == 0) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
            hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
            if ((gpt_duration_count > LCD_POLLING_TIMEOUT) && (lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.FRAME_DONE_INT_FLAG == 0)) {
                log_hal_info("bsp_lcd_clear_screen timeout\n");
                break;
            }
        }
        if(isUpdate)
        {
            bsp_lcd_set_layer_to_default();
            bsp_lcd_power_off();
            isUpdate = false;
        }
    } else
#endif
    {
        hal_display_lcd_turn_on_mtcmos();
        MainLCD->ClearScreen(color);
        hal_display_lcd_turn_off_mtcmos();
    }

    return ret;
}

bsp_lcd_status_t bsp_lcd_clear_screen_bw(void)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    if (0 == isInited){
        bsp_lcd_init(0x0);}

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    uint32_t width, height;
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;
    hal_display_lcd_roi_output_t roi_para;

    if (LCM_INTERFACE_TYPE_DSI == lcm_setting.type) {
        MainLCD->IOCTRL(LCM_IOCTRL_QUERY__LCM_WIDTH, &width);
        MainLCD->IOCTRL(LCM_IOCTRL_QUERY__LCM_HEIGHT, &height);

        roi_para.target_start_x = 0;
        roi_para.target_start_y = 0;
        roi_para.target_end_x = width - 1;
        roi_para.target_end_y = height - 1;
        roi_para.roi_offset_x = 0;
        roi_para.roi_offset_y = 0;
        roi_para.roi_background_color = 0xFFFFFF;
        roi_para.main_lcd_output = LCM_8BIT_24_BPP_RGB888_1;

        bsp_lcd_config_roi(&roi_para);

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
        while(isUpdate) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
            hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
            if ((gpt_duration_count > LCD_POLLING_TIMEOUT) && isUpdate) {
                log_hal_info("Previous frame isn't complete\n");
                return BSP_LCD_STATUS_ERROR;
            }
        }

        isUpdate = true;

        bsp_lcd_power_on();
        hal_display_lcd_calculate_te(width, height);
        hal_display_lcd_apply_setting();
        MainLCD->BlockWrite(roi_para.target_start_x , roi_para.target_start_y , roi_para.target_end_x, roi_para.target_end_y);

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
        while(lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.FRAME_DONE_INT_FLAG == 0) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
            hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
            if ((gpt_duration_count > LCD_POLLING_TIMEOUT) && (lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.FRAME_DONE_INT_FLAG == 0)) {
                log_hal_info("bsp_lcd_clear_screenBW timeout\n");
                break;
            }
        }

        roi_para.target_start_x = 0;
        roi_para.target_start_y = height/2;
        roi_para.target_end_x = width - 1;
        roi_para.target_end_y = height - 1;
        roi_para.roi_offset_x = 0;
        roi_para.roi_offset_y = 0;
        roi_para.roi_background_color = 0x0;
        roi_para.main_lcd_output = LCM_8BIT_24_BPP_RGB888_1;

        bsp_lcd_config_roi(&roi_para);

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
        while(isUpdate) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
            hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
            if ((gpt_duration_count > LCD_POLLING_TIMEOUT) && isUpdate) {
                log_hal_info("Previous frame isn't complete\n");
                return BSP_LCD_STATUS_ERROR;
            }
        }

        hal_display_lcd_apply_setting();
        MainLCD->BlockWrite(roi_para.target_start_x , roi_para.target_start_y , roi_para.target_end_x, roi_para.target_end_y);

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
        while(lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.FRAME_DONE_INT_FLAG == 0) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
            hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
            if ((gpt_duration_count > LCD_POLLING_TIMEOUT) && (lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.FRAME_DONE_INT_FLAG == 0)) {
                log_hal_info("bsp_lcd_clear_screen timeout\n");
                break;
            }
        }

        if(isUpdate)
        {
            bsp_lcd_set_layer_to_default();
            bsp_lcd_power_off();
            isUpdate = false;
        }
    } else
#endif
    {
        hal_display_lcd_turn_on_mtcmos();
        MainLCD->ClearScreenBW();
        hal_display_lcd_turn_off_mtcmos();
    }

    return ret;
}

bsp_lcd_status_t bsp_lcd_get_parameter(LCM_IOCTRL_ID_ENUM ID, void *parameters)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    if (0 == isInited){
        bsp_lcd_init(0x0);}

    MainLCD->IOCTRL(ID, parameters);

    return ret;
}

bsp_lcd_status_t bsp_lcd_config_roi(hal_display_lcd_roi_output_t *roi_para)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    bsp_lcd_lock();

    hal_display_lcd_config_roi(roi_para);

    bsp_lcd_unlock();

    return ret;
}

bsp_lcd_status_t bsp_lcd_config_layer(hal_display_lcd_layer_input_t *layer_data)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    bsp_lcd_lock();

    hal_display_lcd_config_layer(layer_data);

    bsp_lcd_unlock();

    return ret;
}

bsp_lcd_status_t bsp_lcd_set_index_color_table(uint32_t *index_table)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    hal_display_lcd_set_index_color_table(index_table);

    return ret;
}

bsp_lcd_status_t bsp_lcd_register_callback(bsp_lcd_callback_t bsp_lcd_callback)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    bsp_lcd_cb = bsp_lcd_callback;

    return ret;
}

bsp_lcd_status_t bsp_lcd_power_on(void)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

    hal_display_lcd_turn_on_mtcmos();
#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    if(LCM_INTERFACE_TYPE_DSI == lcm_setting.type){
        hal_display_dsi_restore_callback();}
#endif

    return ret;
}
bsp_lcd_status_t bsp_lcd_power_off(void)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    if(LCM_INTERFACE_TYPE_DSI == lcm_setting.type){
            hal_display_dsi_deinit();}
#endif
    hal_display_lcd_turn_off_mtcmos();

    return ret;
}

bsp_lcd_status_t bsp_lcd_lock(void)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

#if defined (FREERTOS_ENABLE)
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        if(xSemaphoreTake(lcd_sema, portMAX_DELAY) != pdTRUE) {
            log_hal_info("Take Semaphore isn't failed\n");
            return BSP_LCD_STATUS_ERROR;
        }
    }
#else
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
    while(isUpdate) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
        hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
        if ((gpt_duration_count > LCD_POLLING_TIMEOUT) && isUpdate) {
            log_hal_info("Previous frame isn't complete\n");
            return BSP_LCD_STATUS_ERROR;
        }
    }
#endif

    return ret;
}

bsp_lcd_status_t bsp_lcd_unlock(void)
{
    bsp_lcd_status_t ret = BSP_LCD_STATUS_OK;

#if defined (FREERTOS_ENABLE)
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        if(xSemaphoreGiveFromISR(lcd_sema, &xHigherPriorityTaskWoken) != pdTRUE) {
            log_hal_info("Give Semaphore failed\n");
            return BSP_LCD_STATUS_ERROR;
        }
        if(xHigherPriorityTaskWoken){
            portYIELD_FROM_ISR (xHigherPriorityTaskWoken);}
    }
#endif

    return ret;
}
#ifdef __cplusplus
}
#endif

#endif
