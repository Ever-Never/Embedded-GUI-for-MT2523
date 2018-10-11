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

/**
 * @file smt_conn.h
 *
 * MTK Smart Connection interfaces
 *
 * @author  Chang Yong
 * @version 2015/10/24 v0.9
 */
#ifndef __SMT_CONN_H__
#define __SMT_CONN_H__
#include <stdint.h>
#include <stdbool.h>

#include "wifi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup Smart_Connection
 * @{
 * This document introduces MTK smart connection interfaces for upper applications.
 * It has wrapped the base interfaces of core.h, which supplies core functionality
 * of MTK elian protocol. MTK elian protocol is confidential and it doesn't
 * release its source code. The file smt_conn.c shows how to use these elian
 * APIs and WiFi APIs, such as switch channel, set rx filter, set raw packet
 * handler and so on.
 * You can replace MTK elian protocol and develop your own smart
 * config protocol easily.
 */



/**@brief Specifies timeout value that can remain in each channel to listen
 * packets from smart phone.
 */
#define switch_channel_timems   (200)          /* elian 200ms */
/**@brief Specifies timeout value that can receive data packets in which
 * channel has sniffered sync packets.
 */
#define locked_channel_timems   (5 * 1000)     /* 5s */
#define use_smart_connection_filter  1


/**@brief For FreeRTOS compatibility.
 */
#define tmr_nodelay         ( TickType_t ) 0

/**@brief After gained smart connection information, it will start a scan
 * process, this value specifies the maximum times to scan.
 */
#define max_scan_loop_count (5)

/**@brief Smart connection finish flag.
 */
#define SMTCN_FLAG_FIN  (0xAA)

/**@brief Smart connection time out flag.
 */
#define SMTCN_FLAG_TIME_OUT  (0xAB)

/**@brief Smart connection fail flag.
 */
#define SMTCN_FLAG_FAIL  (0xAE)

/**@brief Smart connection stop flag.
 */
#define SMTCN_FLAG_EXIT  (0xAF)

/**@brief Scan finished flag, which means successfully scan the target AP
 * information.
 */
#define SCAN_FLAG_FIN (0xBB)

/**@brief Scan finish flag, which means not found the target AP information.
 */
#define SCAN_FLAG_NOT_FOUND (0xCC)

/**@brief for init purpose
 */
#define SCAN_FLAG_NONE   (0x00)

/**
 *@}
 */
#define BSMTCN_MAX_CUSTOM_LEN (640)


#define USE_MC_PROTOCOL 0
#define USE_BC_PROTOCOL 1
#define USE_BOTH 2

/**@defgroup SMTCN_STRUCT Structure
*@{
*/

/** @brief smart connection result information.
*/
typedef struct {
    uint8_t                   pwd[WIFI_LENGTH_PASSPHRASE]; /**< store the password you got */
    uint8_t                   ssid[WIFI_MAX_LENGTH_OF_SSID]; /**< store the ssid you got */
    uint8_t                   pmk[WIFI_LENGTH_PMK]; /**< store the PMK if you have any */
    uint8_t                   tlv_data[BSMTCN_MAX_CUSTOM_LEN];
    int32_t                   tlv_data_len;
    wifi_auth_mode_t          auth_mode; /**< deprecated */
    uint8_t                   ssid_len; /**< ssid length */
    uint8_t                   pwd_len;  /**< password length */

    wifi_encrypt_type_t       encrypt_type; /**< Not used */
    uint8_t                   channel;      /**< Not used */
    uint8_t                   smtcn_flag;   /**< Flag to spicify whether smart connection finished. */
    uint8_t                   scan_flag;    /**< Flag to spicify whether scan finished. */
} smtcn_info;

/** @brief smart connection operations to adapt to specific protocol.
*/
typedef struct {
    int32_t (*init)(const uint8_t *key, const uint8_t key_length);     /**< protocol specific initialize */
    void (*cleanup)(void); /**< protocol specific cleanup */
    int32_t (*switch_channel_rst)(void);  /**< protocol specific reset operation when switch to next channel */
    int32_t (*rx_handler)(char *, int32_t,uint8_t);   /**< protocol specific packet handler */
} smtcn_proto_ops;

typedef enum
{
      EVT_BC_SYNC,
      EVT_MC_SYNC,
      EVT_SYNC_TIME_OUT,
      EVT_FINISHED
}e_multi_protocol_evt_type;

typedef void (* multi_config_evt_cb)(e_multi_protocol_evt_type type);


/**
 *@}
 */

/**
* @brief When have received the sync packets, you can use this function to stop
* switching channel. It means locked at the channel.
*
* @param[in]  None
*
* @return  None
*/
void smtcn_stop_switch(void);

/**
* @brief When locked channel timeout, you can use this function to continue
* to switch channel.
*
* @param[in]  None
*
* @return  None
*/
void smtcn_continue_switch(void);

/**
* @brief This function can be called to create a thread to process smart connection..
*
* @param[in]  None
*
* @return  0 means success, <0 means fail
*/
int32_t mtk_smart_connect(void);

void mtk_smart_set_key(uint8_t key[],uint8_t key_len);

uint8_t smtcn_current_channel(void);

/**
* @brief This function can be called to stop smart connection.
*
* @param[in]  None
*
* @return  0 means success, <0 means fail
*/
int32_t mtk_smart_exit(void);

int32_t mtk_smart_stop(void);

void atomic_write_smtcn_flag(uint8_t flag_value);

bool smtcn_is_debug_on(void);

void smtcn_enable_debug(bool on);
smtcn_proto_ops * smt_get_proto_ops(void);

void bcsmtcn_register_evt_cb(multi_config_evt_cb cb);

void elian_register_evt_cb(multi_config_evt_cb cb);

#if defined(MTK_SMTCN_V4_ENABLE) && defined(MTK_SMTCN_V5_ENABLE)
void smt_ops_set_protocol(int32_t protocol);
#endif

void smtcn_aes_decrypt(uint8_t *cipher_blk, uint32_t cipher_blk_size,
                              uint8_t *key, uint8_t key_len,
                              uint8_t *plain_blk, uint32_t *plain_blk_size);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif
