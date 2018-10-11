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

#if (PRODUCT_VERSION == 2523 || PRODUCT_VERSION == 2533 )
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include "syslog.h"
#include "hal_cm4_topsm.h"
#include "hal_sys_topsm.h"
#include "hal_bsi.h"
#include "hal_sleep_manager.h"
#include "hal_pmu_internal.h"
#include "hal_dsp_topsm.h"
#include "hal_gpt.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_sleep_driver.h"
#include "hal_nvic_internal.h"
#include "hal_pmu.h"
#include "memory_attribute.h"
#include "hal_feature_config.h"
#include <ctype.h>
#include "hal_nvic.h"
#include "hal_dvfs.h"
#include "timers.h"
#ifdef MTK_SMART_BATTERY_ENABLE
#include "battery_management.h"
#endif
#ifdef HAL_SLEEP_MANAGER_ENABLED
extern atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd);
#endif
ATTR_RWDATA_IN_TCM extern bool FAST_WAKEUP_VERIFICATION;
extern uint8_t sleep_manager_handle;

#ifdef MTK_DEMO_PROJ_LOWPOWER_AT_CMD_ENABLE
#include "mt25x3_hdk_backlight.h"
#include "mt25x3_hdk_lcd.h"
#include "task_def.h"
#endif

char *end_pos = NULL;
int input_value = 0;
int input_1 = 0;
int input_2 = 0;
char *mid_pos = NULL;
char str[20] = { 0 };

extern void mtcmos_bt(void);
extern void mtcmos_mm(void);
extern void mtcmos_md2g(void);

ATTR_RWDATA_IN_TCM extern bool FAST_WAKEUP_VERIFICATION;

extern uint8_t sleep_manager_handle;
#define LOGE(fmt,arg...)   LOG_E(atcmd, "ATCMD: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "ATCMD: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd ,"ATCMD: "fmt,##arg)


static TimerHandle_t xTimerofTest = NULL;

void at_power_off_task(TimerHandle_t pxTimer){
hal_sleep_manager_enter_power_off_mode();
}

void at_force_to_sleep(TimerHandle_t pxTimer){
    extern sleep_driver_struct sleepdrv;
    sleepdrv.sleep_lock_index=0;
}

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd);

int sm_htoi(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X')){
        i = 2;
    }else{
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i){
        if (tolower((unsigned char)s[i]) > '9'){
            n = 16 * n + (10 + tolower((unsigned char)s[i]) - 'a');
        }else{
            n = 16 * n + (tolower((unsigned char)s[i]) - '0');
        }
    }
    return n;
}

atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    response.response_flag = 0;
    if(strstr((char *) parse_cmd->string_ptr, "AT+SM=0") != NULL){
        strcpy((char *)response.response_buf, "Test Command Pass!!\r\n");
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=MTCMOS") != NULL) {
        mid_pos = strchr(parse_cmd->string_ptr, ',');
        mid_pos++;
        end_pos = strchr(mid_pos, ',');
        memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
        input_1 = sm_htoi(mid_pos);
        end_pos++;
        input_2 = sm_htoi(end_pos);
        mid_pos = NULL;
        end_pos = NULL;
        LOGW("[MTCMOS : %d %d ]",input_1,input_2);
        if (input_1 > 2 || input_2 > 1) {
            LOGW("[Error Conmand]\n");
            return ATCI_STATUS_OK;
        }
        sys_topsm_mtcmos_control((hal_sys_topsm_mtcmos_enum_t) input_1,input_2);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=SLEEP") != NULL) {
        mid_pos = strchr(parse_cmd->string_ptr, ',');
        mid_pos++;
        end_pos = strchr(mid_pos, ',');
        memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
        input_1 = sm_htoi(mid_pos);
        end_pos++;
        input_2 = sm_htoi(end_pos);
        mid_pos = NULL;
        end_pos = NULL;
        switch (input_1) {
            case 1:
                if (input_2 == 1) {
                    sys_topsm_debug_log(HAL_SYS_TOPSM_MTCMOS_DEBUG);
                    cm4_topsm_debug_option(true);
                } else if (input_2 == 0) {
                    cm4_topsm_debug_option(false);
                } else {
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(&response);
                    return ATCI_STATUS_ERROR;
                }
                break;
            case 3:
                sleepdrv_get_lock_module();
                break;
            case 7:
                if (input_2 == 0) {
                    LOGW("[BT MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_BT_MTCMOS();
                } else if (input_2 == 1) {
                    LOGW("[MM MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_MM_MTCMOS();
                } else if (input_2 == 2) {
                    LOGW("[MD2G MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_MD2G_MTCMOS();
                } else if (input_2 == 3) {
                    LOGW("[BT MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_BT_MTCMOS();
                } else if (input_2 == 4) {
                    LOGW("[MM MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_MM_MTCMOS();
                } else if (input_2 == 5) {
                    LOGW("[MD2G MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_MD2G_MTCMOS();
                }
                break;
        }
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,0") != NULL) {
            LOGW("[Eint Setting] \n");
            sleep_driver_fast_wakeup_eint_setting(HAL_GPIO_4, HAL_EINT_NUMBER_3);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,1") != NULL) {
            LOGW("[EVB Eint Setting] \n");
            sleep_driver_fast_wakeup_eint_setting(HAL_GPIO_3,HAL_EINT_NUMBER_14);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,3") != NULL) {
            LOGW("[Fast_wakeup]\n");
            FAST_WAKEUP_VERIFICATION = true;
            hal_dvfs_target_cpu_frequency(26000, HAL_DVFS_FREQ_RELATION_L);
            hal_dvfs_target_cpu_frequency(26000, HAL_DVFS_FREQ_RELATION_H);
            *((volatile uint32_t *) (0xA2020C18)) = 0xF0000000;
            *((volatile uint32_t *) (0xA2020004)) = 0x00008000;
            *((volatile uint32_t *) (0xA2020304)) = 0x00008000; //high
            hal_sleep_manager_unlock_sleep(sleep_manager_handle);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,4") != NULL) {
            LOGW("[Normail_wakeup] \n");
            FAST_WAKEUP_VERIFICATION = true;
            *((volatile uint32_t *) (0xA2020C18)) = 0xF0000000;
            *((volatile uint32_t *) (0xA2020004)) = 0x00008000;
            *((volatile uint32_t *) (0xA2020304)) = 0x00008000; //high
            hal_sleep_manager_unlock_sleep(sleep_manager_handle);
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,5") != NULL) {
            FAST_WAKEUP_VERIFICATION = true;
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=1,6") != NULL) {
            xTimerofTest = xTimerCreate("TimerofTest", /* Just a text name, not used by the kernel. */
            (5 * 1000 / portTICK_PERIOD_MS), /* The timer period in ticks. */
            pdTRUE, /* The timers will auto-reload themselves when they expire. */
            NULL, /* Assign each timer a unique id equal to its array index. */
            at_force_to_sleep /* Each timer calls the same callback when it expires. */
            );
            xTimerStart(xTimerofTest, 0);

        } else {
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&response);
            return ATCI_STATUS_ERROR;
        }
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=3") != NULL) {
        sys_topsm_debug_log(HAL_SYS_TOPSM_MTCMOS_DEBUG);
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=4") != NULL) {
        /*Sleep 15s*/
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=4,0") != NULL) {
            SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
            __asm volatile("cpsid i");
            hal_sleep_manager_set_sleep_time(5000);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_DEEP_SLEEP);
            __asm volatile("cpsie i");
        } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=4,1") != NULL) {
            SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
            __asm volatile("cpsid i");
            hal_sleep_manager_set_sleep_time(5000);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);
            __asm volatile("cpsie i");
        } else {
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&response);
            return ATCI_STATUS_ERROR;
        }
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    }  else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=5") != NULL) {
        /*Power off*/
        xTimerofTest = xTimerCreate("TimerofTest",       /* Just a text name, not used by the kernel. */
                                       (3 *1000 / portTICK_PERIOD_MS),    /* The timer period in ticks. */
                                       pdTRUE,        /* The timers will auto-reload themselves when they expire. */
                                       NULL,   /* Assign each timer a unique id equal to its array index. */
                                       at_power_off_task /* Each timer calls the same callback when it expires. */
                                      );
        xTimerStart(xTimerofTest, 0);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6,0") != NULL) {
            hal_sleep_manager_lock_sleep(sleep_manager_handle);
        }
        else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6,1") != NULL) {
            hal_sleep_manager_unlock_sleep(sleep_manager_handle);
        }
        else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=6,2") != NULL) {
#ifdef MTK_DEMO_PROJ_LOWPOWER_AT_CMD_ENABLE
#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED
            bsp_lcd_display_off();
            bsp_backlight_deinit();
#endif
            task_def_delete_wo_curr_task();
#endif
        }else{
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&response);
            return ATCI_STATUS_ERROR;
        }
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,0") != NULL) {
            mtcmos_bt();
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,1") != NULL) {
            mtcmos_mm();
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,2") != NULL) {
            mtcmos_md2g();
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,3") != NULL) {
#ifdef MTK_SMART_BATTERY_ENABLE
            battery_management_deinit();
#endif
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,4") != NULL) {
            PMIC_VR_CONTROL(PMIC_VA28,PMIC_VR_CTL_DISABLE);
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,5") != NULL) {
            uint8_t A_sleep_manager_handle;
            A_sleep_manager_handle = hal_sleep_manager_set_sleep_handle("A_sleep_manager_handle_array");
            hal_sleep_manager_lock_sleep(A_sleep_manager_handle);
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,6") != NULL) {
        }else if (strstr((char *) parse_cmd->string_ptr, "AT+SM=8,7") != NULL) {
            sleepdrv_get_lock_module();
        }else {
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&response);
            return ATCI_STATUS_ERROR;
        }
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&response);
    } else {
        strcpy((char *) response.response_buf, "Not Support\n");
        response.response_len = strlen((char *) response.response_buf);
        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(&response);
        return ATCI_STATUS_ERROR;
    }
    return ATCI_STATUS_OK;
}
#endif
