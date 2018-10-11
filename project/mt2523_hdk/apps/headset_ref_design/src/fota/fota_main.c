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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* project configure header */
#include "memory_map.h"
#include "task_def.h"

/* kernel service header */
#include "FreeRTOS.h"
#include "syslog.h"
#include "timers.h"
#include "queue.h"

/* HAL module header */
#include "hal_flash.h"
#include "hal_wdt.h"

/* FOTA related header */
#include "bl_fota.h"
#include "fota.h"
#include "fota_gprot.h"
#include "fota_bt_common.h"


/* bluetooth module header */
#include "bt_type.h"
#include "bt_notify.h"
#include "bt_callback_manager.h"

//#define DEBUG_USE_PRINT
//#define FOTA_DM_UT

#ifndef DEBUG_USE_PRINT
#define LOGI(fmt, arg...)          LOG_I(fota_dl_m, fmt, ##arg);
#define LOGW(fmt, arg...)          LOG_W(fota_dl_m, fmt, ##arg);
#define LOGE(fmt, arg...)          LOG_E(fota_dl_m, fmt, ##arg);

#else
#define LOGI(fmt, arg...)          printf(fmt, ##arg);
#define LOGW(fmt, arg...)          printf(fmt, ##arg);
#define LOGE(fmt, arg...)          printf(fmt, ##arg);
#endif

/*****************************************************************************
 * define
 *****************************************************************************/
#define UPDATE_INFO_SIZE     sizeof(bl_fota_update_info_t) //32
#define FLASH_BLOCK_SIZE     (4096)

/*****************************************************************************
 * typedef
 *****************************************************************************/
typedef uint32_t bt_event_t;
typedef struct {
    bt_event_t event_id;
    void *data_ptr;
} fota_queue_struct_t;


/*****************************************************************************
 * static function
 *****************************************************************************/

static void fota_recv_data_start(bt_notify_event_data_t *p_data);
static void fota_recv_data_pack(bt_notify_event_data_t *p_data);
static int16_t fota_recv_data_end();
static void fota_init_mem_info(fota_update_enum_t udpate_type);
void fota_dl_init();
void fota_btnotify_msg_hdlr(void *data);
bool fota_is_power_on_after_update();

/*****************************************************************************
 * global variable
 *****************************************************************************/
log_create_module(fota_dl_m, PRINT_LEVEL_INFO);


/* mem info struct to store flash info, bt address to store remote bt address */
bt_bd_addr_t g_fota_bt_addr;
fota_mem_info_t g_fota_mem_info;
fota_mem_info_t gnss_fota_mem_info;
fota_update_enum_t update_type_enum;
static TimerHandle_t fota_reboot_timer = NULL;

/* flow control global variable */
#ifdef FOTA_DM_UT
uint32_t data_pack_handled;
#endif

/*****************************************************************************
 * utility
 *****************************************************************************/

/**
 * @brief             reply error code to remote end via bt
 * @param[IN]    sender is the id of the entity which send the error code
 * @param[IN]    receiver is the id of the entity which receive the error code
 * @param[IN]    reply code is the exact error code would be sent
 * @return
 */
void fota_reply_int_value(char *sender, char *receiver, int32_t reply_code)
{
    char data[50] = {0};
    char error[5] = {0};
    uint32_t write_len;
    
    LOGI("[FOTA] fota_reply_int_value sender = %s, receiver = %s, reply_code = %d\r\n", sender, receiver, (int)reply_code);
    if (reply_code >= 0) {
        sprintf(error, "%d", (int)reply_code);
        sprintf(data, "%s %s %d %d %s",
                sender,
                receiver,
                0,
                strlen(error),
                error);
    } else {
        reply_code = 0 - reply_code;
        sprintf(error, "%d", (int)reply_code);
        sprintf(data, "%s %s %d %d -%s",
                sender,
                receiver,
                0,
                strlen(error) + 1,
                error);
    }

    write_len = bt_notify_send_data(&g_fota_bt_addr, data, strlen(data), true);
    if (write_len != strlen(data)) {
        LOGI("[FOTA] send data unfinished, left = %d\r\n", (int)(strlen(data) - write_len));
    }
}

/**
 * @brief             reply error according to current udate type
 * @param[IN]    update_type is the type of update file current been transftered
 * @param[IN]    reply code is the exact error code would be sent
 * @return
 */
void fota_reply_int_value_ex(fota_update_enum_t update_type, int32_t reply_code)
{
    LOGI("[FOTA] update_type = %d, reply_code = %d\r\n", update_type, (int)reply_code);
    //vTaskDelay(10000);
    if (FOTA_UPDATE_FBIN == update_type) {
        fota_reply_int_value(FOTA_EXTCMD_UPDATE_BIN_SENDER, FOTA_EXTCMD_UPDATE_BIN_RECEIVER, reply_code);
    } else if (FOTA_UPDATE_GNSS == update_type) {
        fota_reply_int_value(FOTA_EXTCMD_GNSS_UPDATE_SENDER, FOTA_EXTCMD_GNSS_UPDATE_RECEIVER, reply_code);
    } else {
        LOGE("[FOTA] no current update\r\n");
    }
}


/**
 * @brief             init specific memory info for different update type
 * @param[IN]    update_type is the type of update file current been transftered
 * @return
 */
static void fota_init_mem_info(fota_update_enum_t update_type)
{
    LOGI("[FOTA] init mem info, udpate_type = %d\r\n", update_type);
    if (FOTA_UPDATE_FBIN == update_type) {
        g_fota_mem_info.start_address = FOTA_RESERVED_BASE - BL_BASE;  //0x00190000
        g_fota_mem_info.end_address = FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - BL_BASE;
        g_fota_mem_info.reserved_size = FOTA_RESERVED_LENGTH;  // 1536KB
        g_fota_mem_info.block_count = FOTA_RESERVED_LENGTH >> 12;
        g_fota_mem_info.block_size = FLASH_BLOCK_SIZE;
        g_fota_mem_info.block_type = HAL_FLASH_BLOCK_4K;
        g_fota_mem_info.total_received = 0;
        g_fota_mem_info.write_ptr = 0;
        g_fota_mem_info.ubin_pack_count = 0;
    } else if (FOTA_UPDATE_GNSS == update_type) {
        gnss_fota_mem_info.start_address = FOTA_RESERVED_BASE - BL_BASE;
        gnss_fota_mem_info.end_address = FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - BL_BASE;
        gnss_fota_mem_info.reserved_size = FOTA_RESERVED_LENGTH;
        gnss_fota_mem_info.block_count = FOTA_RESERVED_LENGTH >> 12;
        gnss_fota_mem_info.block_size = FLASH_BLOCK_SIZE;
        gnss_fota_mem_info.block_type = HAL_FLASH_BLOCK_4K;
        gnss_fota_mem_info.total_received = 0;
        gnss_fota_mem_info.write_ptr = 0;
        gnss_fota_mem_info.ubin_pack_count = 0;
    } else {
        LOGE("[FOTA] init mem info, param error\r\n");
    }
}


/**
 * @brief             fota init operation
 * @return
 */

void fota_download_manager_init()
{
    bt_notify_result_t reg_ret;
    LOGI("[FOTA]fota init\r\n");

    fota_init_mem_info(FOTA_UPDATE_FBIN);
    update_type_enum = FOTA_UPDATE_NONE;

    hal_flash_init();
    //bt_notify_init();
    LOGI("[FOTA] begin register extend command callback hdlr\r\n");
    reg_ret = bt_notify_register_callback(NULL, FOTA_EXTCMD_UPDATE_BIN_SENDER, fota_btnotify_msg_hdlr);
    LOGI("[FOTA] register result: %d\r\n", reg_ret);

    /* bt callback manager registration */
    //bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_SYSTEM, (void*)fota_bt_app_event_callback); 

#if 0
    bt_callback_manager_register_callback(bt_callback_type_gap_get_local_configuration, 0, (void*)fota_bt_gap_get_local_configuration);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_bonding_info, 0, (void*)fota_bt_gap_le_get_bonding_info);    
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_pairing_config, 0, (void*)fota_bt_gap_le_get_pairing_config);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_local_cofig, 0, (void*)fota_bt_gap_le_get_local_config);
#endif
}



/**
 * @brief             handle the start pack of fota transfer
 * @param[IN]    event_data is the ptr of data body
 * @return
 */
static void fota_recv_data_start(bt_notify_event_data_t *event_data)
{
    fota_mem_info_t *mem_info_ptr = NULL;
    if (!strcmp(event_data->sender_id, FOTA_EXTCMD_UPDATE_BIN_SENDER) &&
            !strcmp(event_data->receiver_id, FOTA_EXTCMD_UPDATE_BIN_SENDER)) {
        LOGI("[FOTA] FOTA_UPDATE_FBIN\r\n");
        mem_info_ptr = &g_fota_mem_info;
        update_type_enum = FOTA_UPDATE_FBIN;
    } else if (!strcmp(event_data->sender_id, FOTA_EXTCMD_GNSS_UPDATE_SENDER) &&
               !strcmp(event_data->receiver_id, FOTA_EXTCMD_GNSS_UPDATE_SENDER)) {
        LOGI("[FOTA] FOTA_UPDATE_GNSS\r\n");
        mem_info_ptr = &gnss_fota_mem_info;
        update_type_enum = FOTA_UPDATE_GNSS;
    } else {
        LOGE("[FOTA] error_sender = %s or error_receiver = %s\r\n", event_data->sender_id, event_data->receiver_id);
    }

    if (mem_info_ptr != NULL) {
        mem_info_ptr->write_ptr = mem_info_ptr->start_address;
        mem_info_ptr->ubin_pack_count = atoi((const char*)event_data->data);
        LOGI("[FOTA] write_ptr = %x, data_pack_count = %u\r\n",
              (unsigned int)mem_info_ptr->write_ptr, (unsigned int)mem_info_ptr->ubin_pack_count);
    #ifdef FOTA_DM_UT
        data_pack_handled = 0;
    #endif
    }
}

/**
 * @brief             handle the pure data pack of fota transfer
 * @param[IN]    event_data is the ptr of data body
 * @return
 */
static void fota_recv_data_pack(bt_notify_event_data_t *event_data)
{
    hal_flash_status_t ret = HAL_FLASH_STATUS_OK;
    fota_mem_info_t *mem_info_ptr = NULL;

    if (update_type_enum == FOTA_UPDATE_FBIN) {
        mem_info_ptr = &g_fota_mem_info;
    } else if (update_type_enum == FOTA_UPDATE_GNSS) {
        mem_info_ptr = &gnss_fota_mem_info;
    } else {
        LOGE("[FOTA] end error, not start yet\r\n");
        return;
    }

    LOGI("[FOTA] write_ptr = 0x%x, start_addr = 0x%x, end_addr = 0x%x\r\n", (unsigned int)mem_info_ptr->write_ptr, (unsigned int)mem_info_ptr->start_address, (unsigned int)mem_info_ptr->end_address);

    if (mem_info_ptr->write_ptr >= mem_info_ptr->start_address
            && mem_info_ptr->write_ptr < mem_info_ptr->end_address) {
        if (mem_info_ptr->write_ptr + event_data->length < mem_info_ptr->end_address) {
            uint32_t erase_addr = mem_info_ptr->write_ptr;
            while (erase_addr < mem_info_ptr->write_ptr + event_data->length) {
                if (!(erase_addr % FLASH_BLOCK_SIZE)) {
                    ret = hal_flash_erase(erase_addr, mem_info_ptr->block_type);
                    LOGI("[FOTA] erase flash, ret = %d, address = 0x%x\r\n", ret, (unsigned int)erase_addr);
                }
                erase_addr = (erase_addr + FLASH_BLOCK_SIZE) & (~(FLASH_BLOCK_SIZE - 1));
            }

            LOGI("[FOTA] write flash, write_ptr: 0x%x, data_addr: 0x%x, len: %u\r\n", (unsigned int)mem_info_ptr->write_ptr, (unsigned int)event_data->data, event_data->length);
            ret = hal_flash_write(mem_info_ptr->write_ptr, event_data->data, event_data->length);
            if (HAL_FLASH_STATUS_OK == ret) {
                mem_info_ptr->ubin_pack_count--;
                mem_info_ptr->write_ptr = mem_info_ptr->write_ptr + event_data->length;
                mem_info_ptr->total_received = mem_info_ptr->total_received + event_data->length;
                LOGI("[FOTA] current received = %d\r\n", event_data->length);
                LOGI("[FOTA] total received = %u\r\n", (unsigned int)mem_info_ptr->total_received);
                LOGI("[FOTA] pack left = %u\r\n", (unsigned int)mem_info_ptr->ubin_pack_count);
            } else {
                LOGE("[FOTA] flash write error %d\r\n", ret);
                fota_reply_int_value_ex(update_type_enum, FOTA_WRITE_FLASH_FAIL);
                fota_init_mem_info(update_type_enum);
            }
        } else {
            LOGE("[FOTA] file over reserved size\r\n");
            fota_reply_int_value_ex(update_type_enum, FOTA_UBIN_OVERSIZE);
            fota_init_mem_info(update_type_enum);
        }
    }
}

/**
 * @brief             handle the end pack of fota transfer
 * @return
 * #FOTA_PACK_END_CORRECT   data transfer end correct
 * #FOTA_PACK_END_WRONG      data transfer end wrong
 */
static int16_t fota_recv_data_end()
{
    fota_mem_info_t *mem_info_ptr = NULL;
    if (update_type_enum == FOTA_UPDATE_FBIN) {
        mem_info_ptr = &g_fota_mem_info;
    } else if (update_type_enum == FOTA_UPDATE_GNSS) {
        mem_info_ptr = &gnss_fota_mem_info;
    } else {
        LOGE("\n[FOTA]end error, not start yet\r\n");
    }

    if (mem_info_ptr != NULL) {
        if (mem_info_ptr->write_ptr >= mem_info_ptr->start_address
                && mem_info_ptr->write_ptr < mem_info_ptr->end_address) {
            mem_info_ptr->write_ptr = 0;
            if (mem_info_ptr->ubin_pack_count == 0) {
                LOGI("[FOTA]end correct\r\n");
                fota_reply_int_value_ex(update_type_enum, FOTA_PACK_END_CORRECT);
                return FOTA_PACK_END_CORRECT;
            } else {
                LOGE("[FOTA]end wrong, end pack come early\r\n");
                fota_reply_int_value_ex(update_type_enum, FOTA_PACK_END_WRONG);
                return FOTA_PACK_END_WRONG;
            }
        } else {
            LOGE("[FOTA]end wrong, write pointer wrong\r\n");
            return FOTA_PACK_END_WRONG;
        }
    }
    
    LOGE("\n[FOTA]mem_info_ptr is null\r\n");
    return FOTA_PACK_END_WRONG;
}


static void fota_reboot_timer_hdlr(TimerHandle_t timer_id)
{
    LOGI("reboot timer expired\r\n");
    hal_wdt_status_t ret;
    hal_wdt_config_t wdt_config;
    wdt_config.mode = HAL_WDT_MODE_RESET;
    wdt_config.seconds = 1;
    hal_wdt_init(&wdt_config);
    ret = hal_wdt_software_reset();
    LOGI("[FOTA] wdt software reset result: %d\r\n", ret);
}


static void fota_reboot_device(uint32_t delay_msec)
{
    if (fota_reboot_timer && (xTimerIsTimerActive(fota_reboot_timer) != pdFALSE)) {
        xTimerStop(fota_reboot_timer, 0);
    } else {
        LOGI("create reboot timer\r\n");
        fota_reboot_timer = xTimerCreate( "fota_reboot_timer",
                            (delay_msec/portTICK_PERIOD_MS),
                            pdFALSE,
                            NULL,
                            fota_reboot_timer_hdlr);
    }

    xTimerStart(fota_reboot_timer, 0);
}

void fota_btnotify_msg_hdlr(void *data)
{

    bt_notify_callback_data_t *p_data = (bt_notify_callback_data_t *)data;

    LOGI("[FOTA] msg hdlr evt_id = %d!\r\n", p_data->evt_id);
    switch (p_data->evt_id) {
        case BT_NOTIFY_EVENT_CONNECTION: {
            memcpy(g_fota_bt_addr, p_data->bt_addr, 6);
            /*connected with the remote device*/

            bl_fota_update_info_t* update_info_ptr;
            uint8_t update_info[UPDATE_INFO_SIZE];
            bool updated = false;
            hal_flash_status_t flash_op_result;
            uint32_t update_result;

            flash_op_result = hal_flash_read(FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - BL_FOTA_UPDATE_INFO_RESERVE_SIZE - BL_BASE,
                           update_info, sizeof(bl_fota_update_info_t));
            if (flash_op_result != HAL_FLASH_STATUS_OK) {
                LOGE("[FOTA] fail to read update result, flash_op_result: %d\r\n", flash_op_result);
                break;
            }
            for (int i = 0; i < UPDATE_INFO_SIZE; i++) {
                if (update_info[i] != 0xFF) {
                    updated = true;
                    break;
                }
            }
            if (updated == true) {
                update_info_ptr = (bl_fota_update_info_t *)update_info;
                update_result = update_info_ptr->m_error_code;
                if (update_result == BL_FOTA_ERROR_NONE) {
                    fota_reply_int_value_ex(FOTA_UPDATE_FBIN, FOTA_UPDATE_SUCCESS);
                } else {
                    fota_reply_int_value_ex(FOTA_UPDATE_FBIN, FOTA_UPDATE_COMMON_ERROR);
                    LOGE("[FOTA] fota update fail, error_code = %u\n\n", (unsigned int)update_result);
                }
                flash_op_result = hal_flash_erase(FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - FLASH_BLOCK_SIZE - BL_BASE, g_fota_mem_info.block_type);
                if (flash_op_result != HAL_FLASH_STATUS_OK) {
                    LOGE("[FOTA] erase result fail, error_code = %d\n\n", flash_op_result);
                } else {
                    LOGI("[FOTA] erase result success, error_code\n\n");
                }
            }
        }
        break;
        case BT_NOTIFY_EVENT_DISCONNECTION: {
            memset(g_fota_bt_addr, 0, 6);
            /*disconnected with the remote device*/
            fota_mem_info_t *mem_info_ptr = NULL;
            if (FOTA_UPDATE_FBIN == update_type_enum) {
            mem_info_ptr = &g_fota_mem_info;
            } else if (FOTA_UPDATE_GNSS == update_type_enum) {
                mem_info_ptr = &gnss_fota_mem_info;
            }

            if (mem_info_ptr != NULL) {
                if (mem_info_ptr->write_ptr >= mem_info_ptr->start_address
                        && mem_info_ptr->write_ptr < mem_info_ptr->end_address
                        && mem_info_ptr->ubin_pack_count != 0) {
                    LOGE("[FOTA] BT disconnect during data transfer\n");
                    fota_init_mem_info(update_type_enum);
                }
            }
        }
        break;
        case BT_NOTIFY_EVENT_SEND_IND: {
            /*send  new/the rest data flow start*/
            LOGI("[FOTA] send ind\r\n");

        }
        break;
        case BT_NOTIFY_EVENT_DATA_RECEIVED: {
            /*receive data*/
            if (p_data->event_data.error_code == FOTA_FILE_DATA_BEGIN) {
                LOGI("[FOTA] fota_recv_data_start\r\n");
                fota_recv_data_start(&(p_data->event_data));
                return;
            } else if (p_data->event_data.error_code == FOTA_FILE_DATA_PACK) {
            #ifdef FOTA_DM_UT
                data_pack_handled++;
                LOGI("[FOTA] pack_data_handled = %d\r\n", data_pack_handled);
            #endif
                fota_recv_data_pack(&(p_data->event_data));
            } else if (p_data->event_data.error_code == FOTA_FILE_DATA_END) {
                LOGI("[FOTA] end pack has come\r\n");
                if (fota_recv_data_end() == FOTA_PACK_END_CORRECT) {
                    if (update_type_enum == FOTA_UPDATE_FBIN) {
                        //trigger update here
                        fota_ret_t err;
                        err = fota_trigger_update();
                        LOGI("[FOTA] fota trigger update result: %d\r\n", err);

                        fota_reboot_device(3000);

                    } else {
                        LOGE("[FOTA] gnss firmware update not support on mt2533.\r\n");
                    }
                } else {
                    //don't erase first, check flash write result.
                    fota_init_mem_info(update_type_enum);
                    LOGE("[FOTA] data pack end error\r\n");
                }
            } else {
                LOGE("[FOTA] error data pack type\r\n");
            }
                
        }
        break;
        case BT_NOTIFY_EVENT_NOTIFICATION: 
        case BT_NOTIFY_EVENT_MISSED_CALL: 
        case BT_NOTIFY_EVENT_SMS: 
            break;
        default:
            break;
    }

}


bool fota_is_power_on_after_update()
{
    uint8_t update_info[UPDATE_INFO_SIZE];
    hal_flash_status_t fret;
    bool updated = false;
    if ((fret = hal_flash_read(FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - BL_FOTA_UPDATE_INFO_RESERVE_SIZE - BL_BASE,
               update_info, sizeof(bl_fota_update_info_t))) != HAL_FLASH_STATUS_OK) {
        LOG_E(fota_dl_m, "fail to read flash, fret: %d\r\n", fret);
        return false;
    }

    for (int i = 0; i < UPDATE_INFO_SIZE; i++) {
        if (update_info[i] != 0xFF) {
            updated = true;
            break;
        }
    }

    return updated;
}

