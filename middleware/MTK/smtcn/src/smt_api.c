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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "type_def.h"

#include "smt_conn.h"
#include "smt_api.h"

#include "wifi_api.h"
#include "wifi_scan.h"
#include "wifi_event.h"
#include "semphr.h"
#include "syslog.h"
#include "wifi_rx_desc.h"
#include "nvdm.h"
#include "misc.h"
#include "task_def.h"
#include "wifi_event_id.h"


log_create_module(smnt, PRINT_LEVEL_INFO);

#define smtcn_debug 1
#if (smtcn_debug == 1)
uint8_t channel_times[14] = {0};
#endif

#define SMTCN_RX_QUEUE_SIZE        4
#define USE_SMTCN_STATIC_BUF

#ifdef USE_SMTCN_STATIC_BUF
#define SMTCN_STATIC_BUF_COUNT     (SMTCN_RX_QUEUE_SIZE + 1)
#define SMTCN_STATIC_BUF_SIZE      64

#ifdef __ICCARM__
_Pragma("data_alignment=4") uint8_t  smtcn_rx_buf[SMTCN_STATIC_BUF_COUNT][SMTCN_STATIC_BUF_SIZE] = {{0}};
#else
uint8_t __attribute__((aligned(4))) smtcn_rx_buf[SMTCN_STATIC_BUF_COUNT][SMTCN_STATIC_BUF_SIZE] = {{0}};
#endif

uint8_t smtcn_rx_buf_idx = 0;
#endif

extern global_event_callback __process_global_event[MAX_INTERFACE_NUM];


SemaphoreHandle_t smart_config_mutex = NULL;
xQueueHandle smtcn_rx_queue = NULL;

typedef struct {
    uint8_t    *payload;
    uint32_t    len;
} raw_pkt_t, *praw_pkt_t;

smtcn_proto_ops *psmtcn_proto_ops;
smtcn_info   saved_smtcn_info;
wifi_smart_connection_callback_t smtcn_evt_cb = NULL;

#define switch_channel_timeticks (switch_channel_timems/portTICK_PERIOD_MS)

#if (use_smart_connection_filter == 0)
#else
static uint32_t origin_rxfilter = 0;
#endif
static uint8_t origin_opmode = 0;
static uint8_t origin_bw = 0;

uint8_t channel_list[] = {
    1, 6, 11, 2, 5, 7, 10, 12, 1, 6, 11, 3, 8, 13, 4, 9, 14
};


/*smart connection swtich channel timer */
static TimerHandle_t switch_channel_timer = NULL;

static TimerHandle_t time_out_timer = NULL;


static struct {
    uint16_t  search_idx;
    uint8_t   cur_chanl;
} smt_conn_st;

static TaskHandle_t smtcn_handle = NULL;
static bool   b_init = FALSE;

static bool   b_debug_on = FALSE;


#if 1
int32_t smtcn_raw_pkt_handler(uint8_t *payload, uint32_t len)
{
    P_HW_MAC_RX_DESC_T        pRxWI;
    uint8_t          ucRxPktType;
    uint8_t          ucMacHeaderLength;
    uint16_t         u2PayloadLength;
    uint8_t *pucMacHeader;       /* 802.11 header  */
    uint8_t *pucPayload;         /* 802.11 payload */
    uint16_t         u2RxStatusOffst;


    pRxWI   = (P_HW_MAC_RX_DESC_T)(payload);
    // =======  Process RX Vector (if any) ========

    // 1. ToDo: Cache RXStatus in SwRfb
    u2RxStatusOffst = sizeof(HW_MAC_RX_DESC_T);
    ucRxPktType = HAL_RX_STATUS_GET_PKT_TYPE(pRxWI);

    if ((ucRxPktType != RX_PKT_TYPE_RX_DATA) && (ucRxPktType != RX_PKT_TYPE_SW_DEFINED)) {
        return 0; // not handled
    }

    if (ucRxPktType == RX_PKT_TYPE_RX_DATA ||
            ucRxPktType == RX_PKT_TYPE_DUP_RFB) {
        uint16_t          u2GrpOffst;
        uint8_t              ucGroupVLD;

        // RX Status Group
        u2GrpOffst = sizeof(HW_MAC_RX_DESC_T);
        ucGroupVLD = HAL_RX_STATUS_GET_GROUP_VLD(pRxWI);
        if (ucGroupVLD & BIT(RX_GROUP_VLD_4)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_4_T);
        }

        if (ucGroupVLD & BIT(RX_GROUP_VLD_1)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_1_T);
        }

        if (ucGroupVLD & BIT(RX_GROUP_VLD_2)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_2_T);
        }

        if (ucGroupVLD & BIT(RX_GROUP_VLD_3)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_3_T);
        }

        u2RxStatusOffst = u2GrpOffst;
    }


    // Locate the MAC Header based on GroupVLD
    pucMacHeader = (uint8_t *)(((uint32_t)pRxWI + (HAL_RX_STATUS_GET_HEADER_OFFSET(pRxWI)) + u2RxStatusOffst) & 0xFFFFFFFE);
    ucMacHeaderLength = HAL_RX_STATUS_GET_HEADER_LEN(pRxWI);

    pucPayload = (uint8_t *)((uint32_t)(pucMacHeader + ucMacHeaderLength) & 0xFFFFFFFC);
    u2PayloadLength = (uint16_t)(HAL_RX_STATUS_GET_RX_BYTE_CNT(pRxWI) - ((uint32_t)pucPayload - (uint32_t)pRxWI));

    //LOG_HEXDUMP_I(smnt, "mac header begin", pucMacHeader, ucMacHeaderLength+u2PayloadLength);
    psmtcn_proto_ops->rx_handler((char *)pucMacHeader, ucMacHeaderLength + u2PayloadLength, pRxWI->ucChanFreq);

    return 1; /* handled */
}
#endif

#ifdef MTK_SMTCN_V5_ENABLE
kal_uint8 bsmtcn_adapter_is_channel_locked(void);
#endif

static void switch_next_channel(TimerHandle_t tmr)
{
    uint8_t channel_list_size;
	int32_t ret = 0;
	uint16_t pre_ch_idx = 0;

#ifdef MTK_SMTCN_V5_ENABLE
	if(bsmtcn_adapter_is_channel_locked()) {//workaround stop period time fail
        xTimerStop(switch_channel_timer, tmr_nodelay);
		return;
	}
#endif

    if (psmtcn_proto_ops && psmtcn_proto_ops->switch_channel_rst) {
        if (0 != psmtcn_proto_ops->switch_channel_rst()) {
            return;
        }
    }
	
	pre_ch_idx = smt_conn_st.search_idx;

    channel_list_size = sizeof(channel_list) / sizeof(channel_list[0]);
    if (++smt_conn_st.search_idx >= channel_list_size) {
        smt_conn_st.search_idx = 0;
    }

    smt_conn_st.cur_chanl = channel_list[smt_conn_st.search_idx];
    ret = wifi_config_set_channel(WIFI_PORT_STA, smt_conn_st.cur_chanl);

    if(ret != 0) {
		smt_conn_st.search_idx = pre_ch_idx;
        smt_conn_st.cur_chanl = channel_list[pre_ch_idx];
    }

#if (smtcn_debug == 1)
	channel_times[smt_conn_st.cur_chanl - 1]++;
    /*
        uint8_t channel;
        wifi_config_get_channel(WIFI_PORT_STA, &channel);
        sc_dbg("now switched to channel :%d\n", channel);
        */
#endif
}


static void time_out_cb(TimerHandle_t tmr)
{
    atomic_write_smtcn_flag(SMTCN_FLAG_TIME_OUT);
}


int32_t sc_save_smtcn_info(void)
{
    
#ifdef MTK_NVDM_ENABLE
    int ret = -1;

    do
    {
        char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
        sprintf(buf, "%d", WIFI_MODE_STA_ONLY);
        if(NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode",
                             NVDM_DATA_ITEM_TYPE_STRING,
                             (uint8_t *)buf, strlen(buf)))
        {
            break;
        }

        memset(buf, 0, sizeof(buf));
        memcpy(buf, saved_smtcn_info.ssid, saved_smtcn_info.ssid_len);
        if(NVDM_STATUS_OK !=nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Ssid",
                             NVDM_DATA_ITEM_TYPE_STRING,
                             (uint8_t *)buf, strlen(buf)))
        {
            break;
        }

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d", saved_smtcn_info.ssid_len);
        if(NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SsidLen",
                             NVDM_DATA_ITEM_TYPE_STRING,
                             (uint8_t *)buf, strlen(buf)))
        {
            break;
        }

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d", saved_smtcn_info.auth_mode);
        if(NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "AuthMode",
                             NVDM_DATA_ITEM_TYPE_STRING,
                             (uint8_t *)buf, strlen(buf)))
        {
            break;
        }

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d", saved_smtcn_info.encrypt_type);
        if(NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "EncrypType",
                             NVDM_DATA_ITEM_TYPE_STRING,
                             (uint8_t *)buf, strlen(buf)))
        {
            break;
        }

        if(saved_smtcn_info.auth_mode != WIFI_AUTH_MODE_OPEN) {
            memset(buf, 0, sizeof(buf));
            memcpy(buf, saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);
            if(NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPsk",
                                 NVDM_DATA_ITEM_TYPE_STRING,
                                 (uint8_t *)buf, strlen(buf)))
            {
                break;
            }

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%d", saved_smtcn_info.pwd_len);
            if(NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPskLen",
                                 NVDM_DATA_ITEM_TYPE_STRING,
                                 (uint8_t *)buf, strlen(buf)))
            {
                break;
            }
        }

        if((saved_smtcn_info.auth_mode == WIFI_AUTH_MODE_OPEN) &&
                (saved_smtcn_info.encrypt_type == WIFI_ENCRYPT_TYPE_WEP_ENABLED)) {
            if (saved_smtcn_info.pwd_len == 5 ||
                    saved_smtcn_info.pwd_len == 13 ||
                    saved_smtcn_info.pwd_len == 10 ||
                    saved_smtcn_info.pwd_len == 26) {
                wifi_wep_key_t wep_key;
                wep_key.wep_tx_key_index = 0;
                wep_key.wep_key_length[0] = saved_smtcn_info.pwd_len;
                memcpy(wep_key.wep_key[0], saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);

                memset(buf, 0, sizeof(buf));
                memcpy(buf, wep_key.wep_key[0], wep_key.wep_key_length[0]);
                buf[wep_key.wep_key_length[0]] = ',';
                if(NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKey",
                                     NVDM_DATA_ITEM_TYPE_STRING,
                                     (uint8_t *)buf, strlen(buf)))
                {
                    break;
                }

                memset(buf, 0, sizeof(buf));
                sprintf(buf, "%d,", wep_key.wep_key_length[0]);
                if(NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKeyLen",
                                     NVDM_DATA_ITEM_TYPE_STRING,
                                     (uint8_t *)buf, strlen(buf)))
                {
                    break;
                }

                memset(buf, 0, sizeof(buf));
                sprintf(buf, "%d", wep_key.wep_tx_key_index);
                if(NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "DefaultKeyId",
                                     NVDM_DATA_ITEM_TYPE_STRING,
                                     (uint8_t *)buf, strlen(buf)))
                {
                    break;
                }
            }
        }
        ret = 0;
    }while(0);
    return ret;
#else
    return 0;
#endif

}



int32_t smtcn_rx_handler(uint8_t *payload, uint32_t len)
{
    raw_pkt_t     pkt;

#ifdef USE_SMTCN_STATIC_BUF
    //int32_t  freecount=0;
    pkt.len = len;
    pkt.payload  = smtcn_rx_buf[smtcn_rx_buf_idx];
    memcpy(pkt.payload, payload, SMTCN_STATIC_BUF_SIZE);
    if (xQueueSendToBack(smtcn_rx_queue, (void *)&pkt, (TickType_t)5) == pdPASS) {
        smtcn_rx_buf_idx = (smtcn_rx_buf_idx + 1) % SMTCN_STATIC_BUF_COUNT;
    }
#else
    /* assign pkt */
    pkt.len = len;
    if ((pkt.payload = pvPortMalloc(len)) == NULL) {
        LOG_E(smnt, "%s:%d,pvPortMalloc failed\n", __FILE__, __LINE__);
        return 1;
    }
    memcpy(pkt.payload, payload, len);

    if (xQueueSendToBack(smtcn_rx_queue, (void *)&pkt, (TickType_t)5) != pdPASS) {
        vPortFree(pkt.payload);
        //printf("can't add a job to smart connect queue\n");
    }
#endif

    return 1;
}

uint8_t smtcn_current_channel(void)
{
    return smt_conn_st.cur_chanl;
}
void smtcn_stop_switch(void)
{

	xTimerStop(switch_channel_timer, tmr_nodelay);
    if(smtcn_evt_cb) {
        (*smtcn_evt_cb)(WIFI_SMART_CONNECTION_EVENT_CHANNEL_LOCKED, NULL);
    }
#if (smtcn_debug == 1)
    LOG_I(smnt, "channel locked at %d, scaned %d times\n",
          smt_conn_st.cur_chanl,
          channel_times[smt_conn_st.cur_chanl - 1]);
#endif
}

void smtcn_continue_switch(void)
{
    switch_next_channel(NULL);
    xTimerStart(switch_channel_timer, tmr_nodelay);
}


int32_t smtcn_done(void)
{
#if (use_smart_connection_filter == 0)
    wifi_config_set_opmode(origin_opmode);
#else
    wifi_config_set_rx_filter(origin_rxfilter);
    wifi_config_set_smart_connection_filter(0);
#endif
    wifi_config_set_bandwidth(WIFI_PORT_STA, origin_bw);
    wifi_config_unregister_rx_handler();

#ifndef USE_SMTCN_STATIC_BUF
	raw_pkt_t pkt;

    while (xQueueReceive(smtcn_rx_queue, (void *)&pkt, (TickType_t)0) == pdPASS) {

        vPortFree(pkt.payload);
    }
#endif

    return 0;
}

uint8_t atomic_read_smtcn_flag(void)
{
    return saved_smtcn_info.smtcn_flag;;
}

void atomic_write_smtcn_flag(uint8_t flag_value)
{
    saved_smtcn_info.smtcn_flag = flag_value;
}



static void smtcn_rcv_pkt_thread(void *pvParameters)
{
    LOG_I(smnt,"\n>>smt begin <<\n");
    
    memset((void *)&saved_smtcn_info, 0, sizeof(saved_smtcn_info));

    uint8_t smtcn_flag = 0;
    while ((SMTCN_FLAG_FIN != smtcn_flag) &&
            (SMTCN_FLAG_EXIT != smtcn_flag) &&
            (SMTCN_FLAG_FAIL != smtcn_flag) &&
            (SMTCN_FLAG_TIME_OUT != smtcn_flag)) {
        raw_pkt_t pkt;
        if (xQueueReceive(smtcn_rx_queue, (void *)&pkt, portMAX_DELAY) == pdPASS) {
            /* process this raw packet */
            smtcn_raw_pkt_handler(pkt.payload, pkt.len);
#ifndef USE_SMTCN_STATIC_BUF
            vPortFree(pkt.payload);
#endif
        }

        smtcn_flag = atomic_read_smtcn_flag();
    }

    smtcn_done();
    LOG_I(smnt,"\n>>smt end<<\n");
    if((SMTCN_FLAG_EXIT == smtcn_flag) || (SMTCN_FLAG_FAIL == smtcn_flag)) {
        wifi_smart_connection_deinit();
        goto thread_exit;
    }
        
    if( SMTCN_FLAG_TIME_OUT == smtcn_flag) {
        if(smtcn_evt_cb) {
            (*smtcn_evt_cb)(WIFI_SMART_CONNECTION_EVENT_TIMEOUT, NULL);
        }
    } else {
        if(smtcn_evt_cb) {
            (*smtcn_evt_cb)(WIFI_SMART_CONNECTION_EVENT_INFO_COLLECTED, NULL);
        }
    }

thread_exit:
    smtcn_handle = NULL;
    vTaskDelete(NULL);
}

bool smtcn_is_debug_on(void)
{
    return b_debug_on;
}

void smtcn_enable_debug(bool on)
{
    b_debug_on = on;
}

void wifi_smart_connection_deinit(void)
{
    if (smtcn_rx_queue != NULL) {
        vQueueDelete(smtcn_rx_queue);
        smtcn_rx_queue = NULL;
    }

    if (switch_channel_timer != NULL) {
        xTimerDelete(switch_channel_timer, tmr_nodelay);
        switch_channel_timer = NULL;
    }

    if(time_out_timer != NULL) {
        xTimerDelete(time_out_timer, tmr_nodelay);
        time_out_timer = NULL;
    }
    if (psmtcn_proto_ops && psmtcn_proto_ops->cleanup) {
        psmtcn_proto_ops->cleanup();
    }

    smtcn_evt_cb = NULL;
    psmtcn_proto_ops = NULL;
    b_init = FALSE;
}

bool wifi_smart_running()
{
    return  (smtcn_handle == NULL ? FALSE:TRUE);
}

wifi_smart_connection_status_t wifi_smart_connection_init (const uint8_t *key, const uint8_t key_length, wifi_smart_connection_callback_t  callback)
{
    taskENTER_CRITICAL();
    
    if (smart_config_mutex == NULL) {
        smart_config_mutex = xSemaphoreCreateMutex();
        if (smart_config_mutex == NULL) {
            configASSERT(0);
        }
    }
    taskEXIT_CRITICAL();

    xSemaphoreTake(smart_config_mutex, portMAX_DELAY);
    
    if(b_init) {
        LOG_E(smnt,"already init.\n");
        xSemaphoreGive(smart_config_mutex);
        return WIFI_SMART_CONNECTION_ERROR;
    }


    if(psmtcn_proto_ops == NULL) {
         psmtcn_proto_ops = smt_get_proto_ops();
         if(psmtcn_proto_ops == NULL)
         {
            LOG_E(smnt, "ops is NULL.\n");
            goto fail;
         }
    }

    if (smtcn_rx_queue == NULL) {
        smtcn_rx_queue = xQueueCreate(SMTCN_RX_QUEUE_SIZE, sizeof(raw_pkt_t));
        if (smtcn_rx_queue == NULL) {
            LOG_E(smnt, "rx_queue create failed.\n");
            goto fail;
        }
        configASSERT(smtcn_rx_queue);
        vQueueAddToRegistry(smtcn_rx_queue, "smart connect");
    }

    if(switch_channel_timer == NULL){
        switch_channel_timer = xTimerCreate("switch_channel_timer",
                                            switch_channel_timeticks,
                                            pdTRUE,
                                            NULL,
                                            switch_next_channel);
        if (switch_channel_timer == NULL) {
            LOG_E(smnt, "ch timer create fail.\n");
            goto fail;
        }
    }
    
    smtcn_evt_cb = callback;
    if (psmtcn_proto_ops && psmtcn_proto_ops->init)
        if (psmtcn_proto_ops->init(key, key_length) != 0) {
            goto fail;
        }
    b_init = TRUE;
    xSemaphoreGive(smart_config_mutex);
    return WIFI_SMART_CONNECTION_OK;


fail:
    wifi_smart_connection_deinit();
    xSemaphoreGive(smart_config_mutex);
    return WIFI_SMART_CONNECTION_ERROR;
}

wifi_smart_connection_status_t wifi_smart_connection_start (uint16_t timeout_seconds)
{
    eTaskState smtcn_state;
    wifi_smart_connection_status_t ret = WIFI_SMART_CONNECTION_OK;
    
    xSemaphoreTake(smart_config_mutex, portMAX_DELAY);

    if(smtcn_handle != NULL) {
        smtcn_state = eTaskGetState(smtcn_handle);
        LOG_E(smnt,"smtcn task state:%d, cannot create it\n", smtcn_state);
        xSemaphoreGive(smart_config_mutex);
        return WIFI_SMART_CONNECTION_ERROR;
    }

    do
    {
        wifi_config_get_opmode(&origin_opmode);

        if (origin_opmode == WIFI_MODE_AP_ONLY) {
            LOG_W(smnt, "opmode:[%d], smt fail\n",
                  origin_opmode);
            ret = WIFI_SMART_CONNECTION_ERROR;
            break;
        }

        if(!b_init)
        {
            ret = WIFI_SMART_CONNECTION_ERROR;
            break;
        }

        if (xTaskCreate(smtcn_rcv_pkt_thread,
                        UNIFY_SMTCN_TASK_NAME,
                        UNIFY_SMTCN_TASK_STACKSIZE / 4,
                        NULL,
                        UNIFY_SMTCN_TASK_PRIO,
                        &smtcn_handle) != pdPASS) {
            LOG_E(smnt,"thread create failed\n");
            ret = WIFI_SMART_CONNECTION_ERROR;
            break;
        }

#if (use_smart_connection_filter == 0)
        wifi_config_set_opmode(WIFI_MODE_MONITOR);
#else
        uint32_t rx_filter;
        wifi_config_get_rx_filter(&origin_rxfilter);
    	rx_filter = origin_rxfilter;
        rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_NOT_MY_BSSID);
        rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_NOT_UC2ME);
        rx_filter |= BIT(WIFI_RX_FILTER_DROP_PROBE_REQ);
        rx_filter |= BIT(WIFI_RX_FILTER_DROP_BSSID_BCN);

        
#if defined (MTK_SMTCN_V4_ENABLE) && defined (MTK_SMTCN_V5_ENABLE)
        rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_BC_FRAME);
        rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_MC_FRAME);
#elif defined (MTK_SMTCN_V5_ENABLE)
        rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_BC_FRAME);
        rx_filter |= BIT(WIFI_RX_FILTER_DROP_MC_FRAME);
#elif defined(MTK_SMTCN_V4_ENABLE)
        rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_MC_FRAME);
        rx_filter |= BIT(WIFI_RX_FILTER_DROP_BC_FRAME);
#endif
        wifi_config_set_rx_filter(rx_filter);
#endif
        wifi_config_get_bandwidth(WIFI_PORT_STA, &origin_bw);
        wifi_config_set_bandwidth(WIFI_PORT_STA, WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ);

        wifi_config_register_rx_handler((wifi_rx_handler_t)smtcn_rx_handler);

        memset((void *)&smt_conn_st, 0, sizeof(smt_conn_st));
        smt_conn_st.cur_chanl = channel_list[0];

        wifi_config_set_channel(WIFI_PORT_STA, smt_conn_st.cur_chanl);
#if (smtcn_debug == 1)
        memset(channel_times, 0, sizeof(channel_times));
        channel_times[smt_conn_st.cur_chanl - 1]++;
#endif
        xTimerStart(switch_channel_timer, tmr_nodelay);



        if(timeout_seconds != 0) {
            time_out_timer = xTimerCreate("timer_out_timer",
                                          timeout_seconds * 1000 / portTICK_PERIOD_MS,
                                          pdFALSE,
                                          NULL,
                                          time_out_cb);
            if (time_out_timer == NULL) {
                LOG_E(smnt, "time_out_timer create fail.\n");
                ret = WIFI_SMART_CONNECTION_ERROR;
                break;
            }
            
            xTimerStart(time_out_timer, tmr_nodelay);
        }
    }
    while(0);

    if(b_init && (ret == WIFI_SMART_CONNECTION_ERROR))
    {
        wifi_smart_connection_deinit();
    }
    
    xSemaphoreGive(smart_config_mutex);

    
    return ret;
}


wifi_smart_connection_status_t wifi_smart_connection_stop(void)
{
    atomic_write_smtcn_flag(SMTCN_FLAG_EXIT);
    return WIFI_SMART_CONNECTION_OK;
}

wifi_smart_connection_status_t wifi_smart_connection_get_result (uint8_t *ssid, uint8_t *ssid_length,
        uint8_t *password, uint8_t *password_length,
        uint8_t *tlv_data, uint8_t *tlv_data_length)

{
    if(saved_smtcn_info.ssid_len > 0) {
        if(ssid != NULL) {
            memcpy(ssid, saved_smtcn_info.ssid, saved_smtcn_info.ssid_len);
        }
        if(ssid_length != NULL) {
            *ssid_length = saved_smtcn_info.ssid_len;
        }
    }

    if(saved_smtcn_info.pwd_len > 0) {
        if(password != NULL) {
            memcpy(password, saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);
        }
        if(password_length != NULL) {
            *password_length = saved_smtcn_info.pwd_len;
        }
    }

    if(saved_smtcn_info.tlv_data_len > 0) {
        if(tlv_data != NULL) {
            memcpy(tlv_data, saved_smtcn_info.tlv_data, saved_smtcn_info.tlv_data_len);
        }
        if(tlv_data_length != NULL) {
            *tlv_data_length = saved_smtcn_info.tlv_data_len;
        }
    }

    return WIFI_SMART_CONNECTION_OK;
}



