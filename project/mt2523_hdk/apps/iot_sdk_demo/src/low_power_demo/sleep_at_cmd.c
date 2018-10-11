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
#include "battery_management.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
extern atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd);
#endif
ATTR_RWDATA_IN_TCM extern bool FAST_WAKEUP_VERIFICATION;
extern uint8_t sdkdemo_sleep_handle;

#ifdef MTK_DEMO_PROJ_LOWPOWER_AT_CMD_ENABLE
#include "mt25x3_hdk_backlight.h"
#include "mt25x3_hdk_lcd.h"
#include "task_def.h"
#endif

int demo_input_1 = 0;
int demo_input_2 = 0;
char *demo_mid_pos = NULL;
char *demo_end_pos = NULL;
char demo_str[20] = { 0 };
extern void wf_app_task_enable_show(void);
extern void mtcmos_bt(void);
extern void mtcmos_mm(void);
extern void mtcmos_md2g(void);

static TimerHandle_t xTimerofTest = NULL;
void power_off_task(TimerHandle_t pxTimer){
hal_sleep_manager_enter_power_off_mode();
}
void force_to_sleep(TimerHandle_t pxTimer){
    extern sleep_driver_struct sleepdrv;
    sleepdrv.sleep_lock_index=0;
}
int sm_demo_htoi(char s[])
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
atci_status_t sleep_atci_reg_callback(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t sleep_rp = {{0}};
    sleep_rp.response_flag = 0;
    if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=0") != NULL) {
        strcpy((char *)sleep_rp.response_buf, "Test Command Pass!!\r\n");
        sleep_rp.response_len = strlen((char *) sleep_rp.response_buf);
        sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&sleep_rp);
    } else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=MTCMOS") != NULL) {
        demo_mid_pos = strchr(parse_cmd->string_ptr, ',');
        demo_mid_pos++;
        demo_end_pos = strchr(demo_mid_pos, ',');
        memcpy(demo_str, demo_mid_pos,strlen(demo_mid_pos) - strlen(demo_end_pos));
        demo_input_1 = sm_demo_htoi(demo_mid_pos);
        demo_end_pos++;
        demo_input_2 = sm_demo_htoi(demo_end_pos);
        demo_mid_pos = NULL;
        demo_end_pos = NULL;
        LOG_I(common,"[MTCMOS : %d %d ]", demo_input_1, demo_input_2);
        if (demo_input_1 > 2 || demo_input_2 > 1) {
            sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&sleep_rp);
            return ATCI_STATUS_ERROR;
        }
        sys_topsm_mtcmos_control((hal_sys_topsm_mtcmos_enum_t) demo_input_1,demo_input_2);
        sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&sleep_rp);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=SLEEP") != NULL) {
        sleep_rp.response_len = strlen((char *) sleep_rp.response_buf);
        demo_mid_pos = strchr(parse_cmd->string_ptr, ',');
        demo_mid_pos++;
        demo_end_pos = strchr(demo_mid_pos, ',');
        memcpy(demo_str, demo_mid_pos,strlen(demo_mid_pos) - strlen(demo_end_pos));
        demo_input_1 = sm_demo_htoi(demo_mid_pos);
        demo_end_pos++;
        demo_input_2 = sm_demo_htoi(demo_end_pos);
        demo_mid_pos = NULL;
        demo_end_pos = NULL;
        switch (demo_input_1) {
            case 1:
                if (demo_input_2 == 1) {
                    sys_topsm_debug_log(HAL_SYS_TOPSM_MTCMOS_DEBUG);
                    cm4_topsm_debug_option(true);
                } else if (demo_input_2 == 0) {
                    cm4_topsm_debug_option(false);
                } else {
                    sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(&sleep_rp);
                    return ATCI_STATUS_ERROR;
                }
                break;
            case 3:
                sleepdrv_get_lock_module();
                break;
            case 7:
                if (demo_input_2 == 0) {
                    LOG_I(common,"[BT MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_BT_MTCMOS();
                } else if (demo_input_2 == 1) {
                    LOG_I(common,"[MM MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_MM_MTCMOS();
                } else if (demo_input_2 == 2) {
                    LOG_I(common,"[MD2G MTCMOS lock in sleep]\n");
                    cm4_topsm_lock_MD2G_MTCMOS();
                } else if (demo_input_2 == 3) {
                    LOG_I(common,"[BT MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_BT_MTCMOS();
                } else if (demo_input_2 == 4) {
                    LOG_I(common,"[MM MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_MM_MTCMOS();
                } else if (demo_input_2 == 5) {
                    LOG_I(common,"[MD2G MTCMOS unlock in sleep]\n");
                    cm4_topsm_unlock_MD2G_MTCMOS();
                }
                break;
        }
        sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&sleep_rp);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=1") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=1,0") != NULL) {
            LOG_I(common,"[Eint Setting] \n");
            sleep_driver_fast_wakeup_eint_setting(HAL_GPIO_4,HAL_EINT_NUMBER_3);
        } else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=1,1") != NULL) {
            LOG_I(common,"[EVB Eint Setting] \n");
            sleep_driver_fast_wakeup_eint_setting(HAL_GPIO_3,HAL_EINT_NUMBER_14);
        } else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=1,3") != NULL) {
            LOG_I(common,"[Fast_wakeup]\n");
            FAST_WAKEUP_VERIFICATION = true;
            hal_dvfs_target_cpu_frequency(26000, HAL_DVFS_FREQ_RELATION_L);
            hal_dvfs_target_cpu_frequency(26000, HAL_DVFS_FREQ_RELATION_H);
            *((volatile uint32_t *) (0xA2020C18)) = 0xF0000000;
            *((volatile uint32_t *) (0xA2020004)) = 0x00008000;
            *((volatile uint32_t *) (0xA2020304)) = 0x00008000; //high
            hal_sleep_manager_unlock_sleep(sdkdemo_sleep_handle);
        } else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=1,4") != NULL) {
            LOG_I(common,"[Normail_wakeup] \n");
            FAST_WAKEUP_VERIFICATION = true;
            *((volatile uint32_t *) (0xA2020C18)) = 0xF0000000;
            *((volatile uint32_t *) (0xA2020004)) = 0x00008000;
            *((volatile uint32_t *) (0xA2020304)) = 0x00008000; //high
            hal_sleep_manager_unlock_sleep(sdkdemo_sleep_handle);
        }else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=1,5") != NULL) {
            FAST_WAKEUP_VERIFICATION = true;
        }else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=1,6") != NULL) {
            xTimerofTest = xTimerCreate("TimerofTest",       /* Just a text name, not used by the kernel. */
                                                  (5 *1000 / portTICK_PERIOD_MS),    /* The timer period in ticks. */
                                                  pdTRUE,        /* The timers will auto-reload themselves when they expire. */
                                                  NULL,   /* Assign each timer a unique id equal to its array index. */
                                                  force_to_sleep /* Each timer calls the same callback when it expires. */
                                                 );
                   xTimerStart(xTimerofTest, 0);

        }else{
            sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&sleep_rp);
            return ATCI_STATUS_ERROR;
        }
        sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&sleep_rp);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=3") != NULL) {
        sys_topsm_debug_log(HAL_SYS_TOPSM_MTCMOS_DEBUG);
        sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&sleep_rp);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=4") != NULL) {
        /*Sleep 5s*/
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=4,0") != NULL) {
            LOG_I(common,"[enter deep sleep]\n");
            SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
            __asm volatile("cpsid i");
            hal_sleep_manager_set_sleep_time(5000);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_DEEP_SLEEP);
            __asm volatile("cpsie i");
        } else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=4,1") != NULL) {
            LOG_I(common,"[enter sleep]\n");
            SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
            __asm volatile("cpsid i");
            hal_sleep_manager_set_sleep_time(5000);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);
            __asm volatile("cpsie i");
        } else {
            sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&sleep_rp);
            return ATCI_STATUS_ERROR;
        }
        sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&sleep_rp);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=5") != NULL) {
        /*Power off*/
        xTimerofTest = xTimerCreate("TimerofTest",       /* Just a text name, not used by the kernel. */
                                       (3 *1000 / portTICK_PERIOD_MS),    /* The timer period in ticks. */
                                       pdTRUE,        /* The timers will auto-reload themselves when they expire. */
                                       NULL,   /* Assign each timer a unique id equal to its array index. */
                                       power_off_task /* Each timer calls the same callback when it expires. */
                                      );
        xTimerStart(xTimerofTest, 0);
        sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&sleep_rp);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=6") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=6,0") != NULL) {
            hal_sleep_manager_lock_sleep(sdkdemo_sleep_handle);
        }
        else if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=6,1") != NULL) {
            hal_sleep_manager_unlock_sleep(sdkdemo_sleep_handle);
        }
        else if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=6,2") != NULL) {
#ifdef MTK_DEMO_PROJ_LOWPOWER_AT_CMD_ENABLE
#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED
            bsp_lcd_display_off();
            bsp_backlight_deinit();
#endif
            task_def_delete_wo_curr_task();
#endif
        }else{
            sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&sleep_rp);
            return ATCI_STATUS_ERROR;
        }
        sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&sleep_rp);
    } else if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=8") != NULL) {
        if (strstr((char *) parse_cmd->string_ptr, "AT+SM_DEMO=8,0") != NULL) {
            mtcmos_bt();
        } else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=8,1") != NULL) {
            mtcmos_mm();
        } else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=8,2") != NULL) {
            wf_app_task_enable_show();
        } else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=8,3") != NULL) {
            LOG_I(common,"[battery_management_deinit]\n");
            battery_management_deinit();
        }else if (strstr((char *) parse_cmd->string_ptr,"AT+SM_DEMO=8,4") != NULL) {
            LOG_I(common,"[VA28 LDO Off]\n");
            PMIC_VR_CONTROL(PMIC_VA28,PMIC_VR_CTL_DISABLE);
        }else {
            sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&sleep_rp);
            return ATCI_STATUS_ERROR;
        }
        sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&sleep_rp);
    } else {
        strcpy((char *) sleep_rp.response_buf, "Not Support\n");
        sleep_rp.response_len = strlen((char *) sleep_rp.response_buf);
        sleep_rp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(&sleep_rp);
        return ATCI_STATUS_ERROR;
    }
    return ATCI_STATUS_OK;
}

static atci_cmd_hdlr_item_t sleep_at_cmd[] = {
{"AT+SM_DEMO", sleep_atci_reg_callback, 0, 0},
};

#define SLEEP_ATCI_COMMAND_COUNT (sizeof(sleep_at_cmd)/sizeof(atci_cmd_hdlr_item_t))

void sleep_atci_init(void)
{
    atci_status_t ret = atci_register_handler(&sleep_at_cmd[0], SLEEP_ATCI_COMMAND_COUNT);
    if (ret == ATCI_STATUS_OK) {
        LOG_I(common, "sleep at cmd success");
    } else {
        LOG_W(common, "sleep at cmd fail");
    }
}

