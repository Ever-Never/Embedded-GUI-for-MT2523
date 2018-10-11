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

#ifndef __BT_SINK_SRV_H__
#define __BT_SINK_SRV_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "bt_system.h"
#include "bt_type.h"
#include "bt_hfp.h"
#include "bt_a2dp.h"
#include "bt_gap.h"

#ifdef MTK_BT_AWS_ENABLE
#define __BT_AWS_SUPPORT__
#endif

#ifdef __BT_AWS_SUPPORT__
#define __BT_AWS_A2DP_SUPPORT__
#endif

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup Services
 * @{
 * The Sink service is a Bluetooth service which integrates HFP, A2DP, AVRCP and PBAPC profiles.
 * It implements most functions of these Bluetooth profiles and provides the interface which is easier to use.
 * The Sink service works as a Bluetooth headset and contains many usual functions such as answer or reject incoming call,
 * get contact name of incoming call, play or pause music, move to previous or next song,
 * reconnection when power on or link lost.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SRV                        |Service, a common service which based on Bluetooth profile. |
 * |\b BT_ADDR                    | Bluetooth address. |
 * |\b DB                         | Database, use to strore remote device information. |
 * |\b CM                         | Connection manger. |
 * |\b ACT                        | Action, define user action to control remote device . |
 * |\b TWC                        | Three-way call, a state of sink status. |
 * |\b AG                         | Audio Gateway, the remote device.|
 * |\b DTMF                       | Dual Tone Multi-Frequency, is an in-band telecommunication signaling system using the voice-frequency band over telephone lines. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Dual-tone_multi-frequency_signaling">Dual tone multi-frequency signaling</a> in Wikipedia. |
 *
 * @section bt_sink_srv_api_usage How to use this module
 *  - Step1: Mandatory, implement #bt_sink_srv_get_mapping_table() to get the mapping relation of user input and sink service status
 *   - Sample code:
 *    @code
 *       static const bt_sink_srv_table_t g_bt_sink_app_mapping_table[] = {
 *            // Call control
 *           {
 *                // ANSWER INCOMING
 *                BT_SINK_SRV_KEY_FUNC,
 *                BT_SINK_SRV_KEY_ACT_PRESS_UP,
 *                BT_SINK_SRV_STATE_INCOMING,
 *                BT_SINK_SRV_ACTION_ANSWER
 *            },
 *            {
 *                // REJECT INCOMING
 *                BT_SINK_SRV_KEY_FUNC,
 *                BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
 *                BT_SINK_SRV_STATE_INCOMING,
 *                BT_SINK_SRV_ACTION_REJECT
 *            }
 *       };
 *
 *       const bt_sink_srv_table_t *bt_sink_srv_get_mapping_table(void)
 *       {
 *            return g_bt_sink_app_mapping_table;
 *       }
 *    @endcode
 *
 *  - Step2: Mandatory, implement #bt_sink_srv_event_callback() to handle the sink events, such as status changed, connection information, caller information etc.
 *   - Sample code:
 *    @code
 *       void bt_sink_srv_event_callback(bt_sink_srv_event_t event_id, bt_sink_srv_event_param_t *param)
 *       {
 *           switch (event_id)
 *           {
 *              case BT_SINK_SRV_EVENT_STATE_CHANGE:
 *                  printf("State changed, previous:0x%x, now:0x%x", param->state_change.previous, param->state_change.now);
 *                  break;
 *
 *              case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE:
 *                  printf("Connection information, profile:0x%x", event->connection_info.profile_type);
 *                  break;
 *
 *              case BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION:
 *                  bt_sink_app_report("Caller information, number:%s, name:%s",
 *                              event->caller_info.number,
 *                              event->caller_info.name);
 *                  break;
 *
 *              case BT_SINK_SRV_EVENT_HF_MISSED_CALL:
 *                  bt_sink_app_report("Missed call, number:%s, name:%s",
 *                              event->caller_info.number,
 *                              event->caller_info.name);
 *                  break;
 *
 *              case BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE:
 *                  bt_sink_app_report("Speaker volume, %d", event->volume_change.current_volume);
 *                  break;
 *
 *              default:
 *                  break;
 *            }
 *        }
 *    @endcode
 *
 *  - Step3: Mandatory, receive Bluetooth profile event from stack callback or Bluetooth callback manager.
 *   - Sample code:
 *    @code
 *       const bt_gap_config_t* bt_gap_get_local_configuration(void)
 *       {
 *           return bt_sink_srv_gap_config();
 *       }
 *
 *       void bt_gap_get_link_key(bt_gap_link_key_notification_ind_t* key_information)
 *       {
 *           bt_sink_srv_cm_reply_link_key(key_information);
 *       }
 *
 *       bt_status_t bt_hfp_get_init_params(bt_hfp_init_params_t *init_params)
 *       {
 *           return bt_sink_srv_hf_get_init_params(init_params);
 *       }
 *
 *       bt_status_t bt_a2dp_get_init_params(bt_a2dp_init_params_t *param)
 *       {
 *           return bt_sink_srv_a2dp_get_init_params(param);
 *       }
 *
 *       bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           return bt_sink_srv_common_callback(msg, status, buff);
 *       }
 *    @endcode
 *
 *  - Step4: Mandatory, initialize Bluetooth sink service when system initialization.
 *   - Sample code:
 *    @code
 *           bt_sink_srv_features_config_t config;
 *
 *           config.features = BT_SINK_SRV_FEATURE_POWER_ON_RECONNECT;
 *           bt_sink_srv_init(&config);
 *    @endcode
 *
 *  - Step5: Optional, according to mapping table and current sink service status, send user event to control remote device.
 *   - Sample code:
 *    @code
 *           // if current sink status is BT_SINK_SRV_STATE_INCOMING
 *           // and mapping table which get from bt_sink_srv_get_mapping_table() include following item.
 *           // {
 *           //     BT_SINK_SRV_KEY_FUNC,
 *           //     BT_SINK_SRV_KEY_ACT_PRESS_UP,
 *           //     BT_SINK_SRV_STATE_INCOMING,
 *           //     BT_SINK_SRV_ACTION_ANSWER
 *           // }
 *           bt_sink_srv_key_action(BT_SINK_SRV_KEY_FUNC, BT_SINK_SRV_KEY_ACT_PRESS_UP);
 *    @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Bluetooth_sink_srv_define Define
 * @{
 * Define sink service data types and values.
 */

#ifndef NULL
#define NULL ((void *)0)       /**<Default value of the pointer.*/
#endif

#define BT_SINK_SRV_MAX_PHONE_NUMBER BT_HFP_PHONE_NUMBER_MAX_LEN /**<This value is the max phone number size.*/
#define BT_SINK_SRV_MAX_NAME         (64)                        /**<This value is the max contact name size.*/

/**
 *  @brief This structure define sink service actions.
 */
typedef uint32_t bt_sink_srv_action_t;
#define BT_SINK_SRV_ACTION_BASE 0x40000000                                                                           /**< Sink service actions beginning. */

#define BT_SINK_SRV_ACTION_INTERNAL_RANGE 0x100                                                                      /**< Internal used action range. */
#define BT_SINK_SRV_ACTION_INTERNAL_START BT_SINK_SRV_ACTION_BASE                                                    /**< Internal used action beginning. */
#define BT_SINK_SRV_ACTION_INTERNAL_END   (BT_SINK_SRV_ACTION_INTERNAL_START + BT_SINK_SRV_ACTION_INTERNAL_RANGE - 1)/**< Internal used action end. */

#define BT_SINK_SRV_ACTION_COMMON_RANGE  0x100                                                                        /**< Common action range. */
#define BT_SINK_SRV_ACTION_COMMON_START  (BT_SINK_SRV_ACTION_INTERNAL_END + 1)                                        /**< Common action beginning. */
#define BT_SINK_SRV_ACTION_COMMON_END    (BT_SINK_SRV_ACTION_COMMON_START + BT_SINK_SRV_ACTION_COMMON_RANGE - 1)      /**< Common action end. */

#define BT_SINK_SRV_ACTION_CM_RANGE      0x100                                                                       /**< Connection manager action range. */
#define BT_SINK_SRV_ACTION_CM_START      (BT_SINK_SRV_ACTION_COMMON_END + 1)                                         /**< Connection manager action beginning. */
#define BT_SINK_SRV_ACTION_CM_END        (BT_SINK_SRV_ACTION_CM_START + BT_SINK_SRV_ACTION_CM_RANGE - 1)             /**< Connection manager action end. */

#define BT_SINK_SRV_ACTION_HF_RANGE      0x100                                                                       /**< HFP action range. */
#define BT_SINK_SRV_ACTION_HF_START      (BT_SINK_SRV_ACTION_CM_END + 1)                                             /**< HFP action beginning. */
#define BT_SINk_SRV_ACTION_HF_END        (BT_SINK_SRV_ACTION_HF_START + BT_SINK_SRV_ACTION_HF_RANGE - 1)             /**< HFP action end. */

#define BT_SINK_SRV_ACTION_A2DP_RANGE    0x100                                                                       /**< A2DP action range. */
#define BT_SINK_SRV_ACTION_A2DP_START    (BT_SINk_SRV_ACTION_HF_END + 1)                                             /**< A2DP action beginning. */
#define BT_SINK_SRV_ACTION_A2DP_END      (BT_SINK_SRV_ACTION_A2DP_START + BT_SINK_SRV_ACTION_A2DP_RANGE - 1)         /**< A2DP action end. */

#define BT_SINK_SRV_ACTION_AVRCP_RANGE   0x100                                                                       /**< AVRCP action range. */
#define BT_SINK_SRV_ACTION_AVRCP_START   (BT_SINK_SRV_ACTION_A2DP_END + 1)                                           /**< AVRCP action beginning. */
#define BT_SINK_SRV_ACTION_AVRCP_END     (BT_SINK_SRV_ACTION_AVRCP_START + BT_SINK_SRV_ACTION_AVRCP_RANGE - 1)       /**< AVRCP action end. */

#define BT_SINK_SRV_ACTION_PBAPC_RANGE   0x100                                                                       /**< PBAPC action range. */
#define BT_SINK_SRV_ACTION_PBAPC_START   (BT_SINK_SRV_ACTION_AVRCP_END + 1)                                          /**< PBAPC action beginning. */
#define BT_SINK_SRV_ACTION_PBAPC_END     (BT_SINK_SRV_ACTION_PBAPC_START + BT_SINK_SRV_ACTION_PBAPC_RANGE - 1)       /**< PBAPC action end. */

#define BT_SINK_SRV_ACTION_AWS_RANGE   0x100                                                                       /**< AWS action range. */
#define BT_SINK_SRV_ACTION_AWS_START   (BT_SINK_SRV_ACTION_PBAPC_END + 1)                                          /**< AWS action beginning. */
#define BT_SINK_SRV_ACTION_AWS_END     (BT_SINK_SRV_ACTION_AWS_START + BT_SINK_SRV_ACTION_AWS_RANGE - 1)       /**< AWS action end. */

#define BT_SINK_SRV_ACTION_USER_RANGE   0x100                                                                        /**< User defined action range. */
#define BT_SINK_SRV_ACTION_USER_START   (BT_SINK_SRV_ACTION_AWS_END + 1)                                           /**< User defined action beginning. */
#define BT_SINK_SRV_ACTION_USER_END     (BT_SINK_SRV_ACTION_USER_START + BT_SINK_SRV_ACTION_USER_RANGE - 1)          /**< User defined action end. */


/* Internal use start */
#define BT_SINK_SRV_ACTION_PROFILE_INIT       (BT_SINK_SRV_ACTION_INTERNAL_START)     /**< This action indicates the profile to initialize. */
#define BT_SINK_SRV_ACTION_PROFILE_CONNECT    (BT_SINK_SRV_ACTION_INTERNAL_START + 1) /**< This action indicates the profile to create connection. */
#define BT_SINK_SRV_ACTION_PROFILE_DISCONNECT (BT_SINK_SRV_ACTION_INTERNAL_START + 2) /**< This action indicates the profile to disconnect remote device. */
#define BT_SINK_SRV_ACTION_PROFILE_DEINIT     (BT_SINK_SRV_ACTION_INTERNAL_START + 3) /**< This action indicates the profile to de-initialize. */
/* Internal use end */

/* CM action start */
#define BT_SINK_SRV_ACTION_DISCOVERABLE       (BT_SINK_SRV_ACTION_CM_START)     /**< This action send a request to make sink service discoverable. */
#define BT_SINK_SRV_ACTION_CONNECT            (BT_SINK_SRV_ACTION_CM_START + 1) /**< This action send a request to create a connection. */
#define BT_SINK_SRV_ACTION_DISCONNECT         (BT_SINK_SRV_ACTION_CM_START + 2) /**< This action send a request to disconnect a device. */
#define BT_SINK_SRV_ACTION_INQUIRY            (BT_SINK_SRV_ACTION_CM_START + 3) /**< This action send a request to inquiry remote devices. */
#define BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST (BT_SINK_SRV_ACTION_CM_START + 4) /**< This action indicates the profile to reset trusted device list. */
#define BT_SINK_SRV_ACTION_POWER_OFF          (BT_SINK_SRV_ACTION_CM_START + 5) /**< This action send a request to power off the whole system. */
/* CM action end */

/* HF action start */
#define BT_SINK_SRV_ACTION_ANSWER                             (BT_SINK_SRV_ACTION_HF_START)       /**< This action send a request to answer an incoming call. */
#define BT_SINK_SRV_ACTION_REJECT                             (BT_SINK_SRV_ACTION_HF_START + 1)   /**< This action send a request to reject an incoming call. */
#define BT_SINK_SRV_ACTION_HANG_UP                            (BT_SINK_SRV_ACTION_HF_START + 2)   /**< This action send a request to hang up an outgoing call or an active call. */
#define BT_SINK_SRV_ACTION_DIAL_LAST                          (BT_SINK_SRV_ACTION_HF_START + 3)   /**< This action send a request to dial the last dialed number. */
#define BT_SINK_SRV_ACTION_DIAL_MISSED                        (BT_SINK_SRV_ACTION_HF_START + 4)   /**< This action send a request to dial the missed call number. */
#define BT_SINK_SRV_ACTION_QUERY_CALL_LIST                    (BT_SINK_SRV_ACTION_HF_START + 5)   /**< This action send a request to query current call list. */
#define BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH                  (BT_SINK_SRV_ACTION_HF_START + 6)   /**< This action send a request to switch audio path. */
#define BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD              (BT_SINK_SRV_ACTION_HF_START + 7)   /**< This action send a request to release all held calls or sets User Determined User Busy (UDUB) for a waiting call. */
#define BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER   (BT_SINK_SRV_ACTION_HF_START + 8)   /**< This action send a request to release all active calls and accepts the other held or waiting call. */
#define BT_SINK_SRV_ACTION_3WAY_RELEASE_SPECIAL               (BT_SINK_SRV_ACTION_HF_START + 9)   /**< This action send a request to release a specific call. */
#define BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER      (BT_SINK_SRV_ACTION_HF_START + 10)  /**< This action send a request to place all active calls on hold and accepts the other held or waiting call. */
#define BT_SINK_SRV_ACTION_3WAY_HOLD_SPECIAL                  (BT_SINK_SRV_ACTION_HF_START + 11)  /**< This action send a request to place a specific call on hold. */
#define BT_SINK_SRV_ACTION_3WAY_ADD_HELD_CALL_TO_CONVERSATION (BT_SINK_SRV_ACTION_HF_START + 12)  /**< This action send a request to add a held call to the conversation. */
#define BT_SINK_SRV_ACTION_3WAY_EXPLICIT_CALL_TRANSFER        (BT_SINK_SRV_ACTION_HF_START + 13)  /**< This action send a request to connect the two calls and disconnects the AG from both calls. */
#define BT_SINK_SRV_ACTION_SWITCH_AUDIO_DEVICE                (BT_SINK_SRV_ACTION_HF_START + 14)  /**< This action send a request to switch audio between two devices. */
#define BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE         (BT_SINK_SRV_ACTION_HF_START + 15)  /**< This action send a request to activate voice recognition. */
#define BT_SINK_SRV_ACTION_DTMF                               (BT_SINK_SRV_ACTION_HF_START + 16)  /**< This action send a request to instruct the AG to transmit a specific DTMF code to its network connection. */
#define BT_SINK_SRV_ACTION_DIAL_NUMBER                        (BT_SINK_SRV_ACTION_HF_START + 17)  /**< This action send a request to instruct the AG to dial a number. */
#define BT_SINK_SRV_ACTION_HF_REPORT_BATTERY                  (BT_SINK_SRV_ACTION_HF_START + 18)  /**< This action report a battery level (0~9) to an  iOS device. */
#define BT_SINK_SRV_ACTION_CALL_VOLUME_UP                     (BT_SINK_SRV_ACTION_HF_START + 19)  /**< This action send a request to increase the call volume one level. */
#define BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN                   (BT_SINK_SRV_ACTION_HF_START + 20)  /**< This action send a request to decrease the call volume one level. */
#define BT_SINK_SRV_ACTION_CALL_VOLUME_MAX                    (BT_SINK_SRV_ACTION_HF_START + 21)  /**< This action send a request to increase the call volume to the MAX. */
#define BT_SINK_SRV_ACTION_CALL_VOLUME_MIN                    (BT_SINK_SRV_ACTION_HF_START + 22)  /**< This action send a request to decrease the call volume to the MIN. */
/* HF action end */

/* A2DP action start */
#define BT_SINK_SRV_ACTION_SWITCH_PASS_THROUGH_MODE  (BT_SINK_SRV_ACTION_A2DP_START)  /**< This action send a request to switch pass through mode. */
/* A2DP action end */

/* AVRCP action start */
#define BT_SINK_SRV_ACTION_PLAY           (BT_SINK_SRV_ACTION_AVRCP_START)      /**< This action send a request to play a BT music. */
#define BT_SINK_SRV_ACTION_PAUSE          (BT_SINK_SRV_ACTION_AVRCP_START + 1)  /**< This action send a request to pause a music. */
#define BT_SINK_SRV_ACTION_NEXT_TRACK     (BT_SINK_SRV_ACTION_AVRCP_START + 2)  /**< This action send a request to switch to the next song. */
#define BT_SINK_SRV_ACTION_PRE_TRACK      (BT_SINK_SRV_ACTION_AVRCP_START + 3)  /**< This action send a request to switch to the previous song. */
#define BT_SINK_SRV_ACTION_VOLUME_UP      (BT_SINK_SRV_ACTION_AVRCP_START + 4)  /**< This action send a request to increase the music volume one level. */
#define BT_SINK_SRV_ACTION_VOLUME_DOWN    (BT_SINK_SRV_ACTION_AVRCP_START + 5)  /**< This action send a request to decrease the music volume one level. */
#define BT_SINK_SRV_ACTION_VOLUME_MAX     (BT_SINK_SRV_ACTION_AVRCP_START + 6)  /**< This action send a request to increase the music volume one level. */
#define BT_SINK_SRV_ACTION_VOLUME_MIN     (BT_SINK_SRV_ACTION_AVRCP_START + 7)  /**< This action send a request to decrease the music volume one level. */
#define BT_SINK_SRV_ACTION_SWITCH_MODE    (BT_SINK_SRV_ACTION_AVRCP_START + 8)  /**< This action send a request to switch music mode. */

/* AVRCP action end */

/* AWS action start */
#define BT_SINK_SRV_ACTION_AWS_TRANSFER_SINK_ACTION    (BT_SINK_SRV_ACTION_AWS_START + 1)      /**< This action send a sink action to remote by AWS. */
/* AWS action end */

#define BT_SINK_SRV_ACTION_ALL (BT_SINK_SRV_ACTION_USER_END + 1)            /**< This action means the end of action and receiver want to receive any actions. */

/**
 *  @brief This structure define sink service events.
 */
typedef uint32_t bt_sink_srv_event_t;
#define BT_SINK_SRV_EVENT_BASE          0x50000000                                                            /**< Sink service events beginning. */

#define BT_SINK_SRV_EVENT_COMMON_RANGE  0x100                                                                 /**< Common event range. */
#define BT_SINK_SRV_EVENT_COMMON_START  BT_SINK_SRV_EVENT_BASE                                                /**< Common event beginning. */
#define BT_SINK_SRV_EVENT_COMMON_END    (BT_SINK_SRV_EVENT_COMMON_START + BT_SINK_SRV_EVENT_COMMON_RANGE - 1) /**< Common event end. */

#define BT_SINK_SRV_EVENT_CM_RANGE      0x100                                                                 /**< Connection manager event range. */
#define BT_SINK_SRV_EVENT_CM_START      (BT_SINK_SRV_EVENT_COMMON_END + 1)                                    /**< Connection manager event beginning. */
#define BT_SINK_SRV_EVENT_CM_END        (BT_SINK_SRV_EVENT_CM_START + BT_SINK_SRV_EVENT_CM_RANGE - 1)         /**< Connection manager event end. */

#define BT_SINK_SRV_EVENT_HF_RANGE      0x100                                                                 /**< HFP event range. */
#define BT_SINK_SRV_EVENT_HF_START      (BT_SINK_SRV_EVENT_CM_END + 1)                                        /**< HFP event beginning. */
#define BT_SINk_SRV_EVENT_HF_END        (BT_SINK_SRV_EVENT_HF_START + BT_SINK_SRV_EVENT_HF_RANGE - 1)         /**< HFP event end. */

#define BT_SINK_SRV_EVENT_A2DP_RANGE    0x100                                                                 /**< A2DP event range. */
#define BT_SINK_SRV_EVENT_A2DP_START    (BT_SINk_SRV_EVENT_HF_END + 1)                                        /**< A2DP event beginning. */
#define BT_SINK_SRV_EVENT_A2DP_END      (BT_SINK_SRV_EVENT_A2DP_START + BT_SINK_SRV_EVENT_A2DP_RANGE - 1)     /**< A2DP event end. */

#define BT_SINK_SRV_EVENT_AVRCP_RANGE   0x100                                                                 /**< AVRCP event range. */
#define BT_SINK_SRV_EVENT_AVRCP_START   (BT_SINK_SRV_EVENT_A2DP_END + 1)                                      /**< AVRCP event beginning. */
#define BT_SINK_SRV_EVENT_AVRCP_END     (BT_SINK_SRV_EVENT_AVRCP_START + BT_SINK_SRV_EVENT_AVRCP_RANGE - 1)   /**< AVRCP event end. */

#define BT_SINK_SRV_EVENT_PBAPC_RANGE   0x100                                                                 /**< PBAPC event range. */
#define BT_SINK_SRV_EVENT_PBAPC_START   (BT_SINK_SRV_EVENT_AVRCP_END + 1)                                     /**< PBAPC event beginning. */
#define BT_SINK_SRV_EVENT_PBAPC_END     (BT_SINK_SRV_EVENT_PBAPC_START + BT_SINK_SRV_EVENT_PBAPC_RANGE - 1)   /**< PBAPC event end. */

#define BT_SINK_SRV_EVENT_AWS_RANGE    0x100                                                                  /**< AWS event range. */
#define BT_SINK_SRV_EVENT_AWS_START    (BT_SINK_SRV_EVENT_PBAPC_END + 1)                                      /**< AWS event beginning. */
#define BT_SINK_SRV_EVENT_AWS_END      (BT_SINK_SRV_EVENT_AWS_START + BT_SINK_SRV_EVENT_AWS_RANGE - 1)        /**< AWS event end. */

#define BT_SINK_SRV_EVENT_USER_RANGE   0x100                                                                  /**< User defined event range. */
#define BT_SINK_SRV_EVENT_USER_START   (BT_SINK_SRV_EVENT_AWS_END + 1)                                        /**< User defined event beginning. */
#define BT_SINK_SRV_EVENT_USER_END     (BT_SINK_SRV_EVENT_USER_START + BT_SINK_SRV_EVENT_USER_RANGE - 1)      /**< User defined event end. */

#define BT_SINK_SRV_EVENT_STATE_CHANGE (BT_SINK_SRV_EVENT_COMMON_START)  /**< This event indicates the status of sink service has changed. */
#define BT_SINK_SRV_EVENT_REMOTE_SINK_ACTION_RECEIVED (BT_SINK_SRV_EVENT_COMMON_START + 1)
#define BT_SINK_SRV_EVENT_POWER_OFF (BT_SINK_SRV_EVENT_COMMON_START + 2) /**< This action send a request to power off the whole system. */
#define BT_SINK_SRV_EVENT_VOICE_PROMPT (BT_SINK_SRV_EVENT_COMMON_START + 3) /**< This action send a request to play voice prompt tone. */

#define BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE    (BT_SINK_SRV_EVENT_CM_START )      /**< This event indicates the profile connection information of current link. */
#define BT_SINK_SRV_EVENT_VISIBILITY_STATE_UPDATE      (BT_SINK_SRV_EVENT_CM_START + 1)   /**< This event indicates the status of visibility. */
#define BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE       (BT_SINK_SRV_EVENT_CM_START + 2 )  /**< This event indicates the connection information of current link. */
#define BT_SINK_SRV_EVENT_SCAN_MODE_CHANGED            (BT_SINK_SRV_EVENT_CM_START + 3 )  /**< This event indicates the status of scan mode has changed. */
#define BT_SINK_SRV_EVENT_ROLE_CHANGED_IND             (BT_SINK_SRV_EVENT_CM_START + 4)   /**< This event indicates the current GAP role */
#define BT_SINK_SRV_EVENT_START_RECONNECTION           (BT_SINK_SRV_EVENT_CM_START + 5)      /**< This event indicates the sink service start to reconnect the remote device. */

#define BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION        (BT_SINK_SRV_EVENT_HF_START)       /**< This event indicates the caller information of current incoming call. */
#define BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE     (BT_SINK_SRV_EVENT_HF_START + 1)   /**< This event indicates the in-call volume has changed. */
#define BT_SINK_SRV_EVENT_HF_MISSED_CALL               (BT_SINK_SRV_EVENT_HF_START + 2)   /**< This event indicates a missed call. */
#define BT_SINK_SRV_EVENT_HF_VOICE_RECOGNITION_CHANGED (BT_SINK_SRV_EVENT_HF_START + 3)   /**< This event indicates the status of voice recognition. */
#define BT_SINK_SRV_EVENT_HF_CALL_LIST_INFORMATION     (BT_SINK_SRV_EVENT_HF_START + 4)   /**< This event indicates the call list information. */
#define BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE        (BT_SINK_SRV_EVENT_HF_START + 5)   /**< This event indicates the status changed of hf SCO link . */

#define BT_SINK_SRV_EVENT_AWS_PLAY_READY               (BT_SINK_SRV_EVENT_AWS_START)      /**< This event indicates the MP3 + AWS play */
#define BT_SINK_SRV_EVENT_AWS_SUSPEND_CNF              (BT_SINK_SRV_EVENT_AWS_START + 1)  /**< This event indicates the MP3 + AWS suspend confirm event */
#define BT_SINK_SRV_EVENT_AWS_MP3_DATA_REQ             (BT_SINK_SRV_EVENT_AWS_START + 2)  /**< This event indicates the MP3 + AWS data request from dsp */
#define BT_SINK_SRV_EVENT_AWS_MP3_DATA_IND             (BT_SINK_SRV_EVENT_AWS_START + 3)  /**< This event indicates the MP3 + AWS data indication from AWS source */
#define BT_SINK_SRV_EVENT_AWS_MP3_SUSPEND_IND          (BT_SINK_SRV_EVENT_AWS_START + 4)  /**< This event indicates the MP3 + AWS suspend indication from AWS source */
#define BT_SINK_SRV_EVENT_AWS_MP3_RESUME_IND           (BT_SINK_SRV_EVENT_AWS_START + 5)  /**< This event indicates the MP3 + AWS resume indication from AWS source */
#define BT_SINK_SRV_EVENT_AWS_MP3_STOP_IND             (BT_SINK_SRV_EVENT_AWS_START + 6)  /**< This event indicates the MP3 + AWS stop indication from AWS source */
#define BT_SINK_SRV_EVENT_AWS_MP3_CHANGE_TRACK_IND     (BT_SINK_SRV_EVENT_AWS_START + 7)  /**< This event indicates the MP3 + AWS change track indication from AWS source */
#define BT_SINK_SRV_EVENT_AWS_MP3_SNK_INTERRUPT        (BT_SINK_SRV_EVENT_AWS_START + 9)  /**< This event indicates the MP3 + AWS receive interrupt */
#define BT_SINK_SRV_EVENT_AWS_MP3_SNK_RESUME           (BT_SINK_SRV_EVENT_AWS_START + 10) /**< This event indicates the MP3 + AWS receive resume */
#define BT_SINK_SRV_EVENT_AWS_MP3_SNK_INTERRUPT_IND    (BT_SINK_SRV_EVENT_AWS_START + 11) /**< This event indicates the MP3 + AWS receive SNK interrupt */
#define BT_SINK_SRV_EVENT_AWS_MP3_SNK_RESUME_IND       (BT_SINK_SRV_EVENT_AWS_START + 12) /**< This event indicates the MP3 + AWS receive SNK resume */
#define BT_SINK_SRV_EVENT_AWS_MP3_KEY_PRESS            (BT_SINK_SRV_EVENT_AWS_START + 13) /**< This action notify AWS source that sink trigger a key action */
#define BT_SINK_SRV_EVENT_AWS_MP3_RESYNC               (BT_SINK_SRV_EVENT_AWS_START + 14) /**< This action notify AWS source that sink ask for starting resync */
#define BT_SINK_SRV_EVENT_AWS_MP3_RESYNC_IND           (BT_SINK_SRV_EVENT_AWS_START + 15) /**< This action notify AWS sink received resync request */
#define BT_SINK_SRV_EVENT_AWS_VOLUME_UP                (BT_SINK_SRV_EVENT_AWS_START + 16)
#define BT_SINK_SRV_EVENT_AWS_VOLUME_DOWN              (BT_SINK_SRV_EVENT_AWS_START + 17)
#define BT_SINK_SRV_EVENT_AWS_MP3_INIT_SYNC            (BT_SINK_SRV_EVENT_AWS_START + 18) /**< This action notify AWS sink init sync has come */

#define BT_SINK_SRV_EVENT_USER                         (BT_SINK_SRV_EVENT_USER_START)    /**< This event means the start of the customized event. */
#define BT_SINK_SRV_EVENT_MIXER_TONE_PLAY              (BT_SINK_SRV_EVENT_USER_START + 10) /**< This event indicates play tone. */
#define BT_SINK_SRV_EVENT_MIXER_TONE_STOP              (BT_SINK_SRV_EVENT_USER_START + 11) /**< This event indicates the tone should be stopped. */

#define BT_SINK_SRV_EVENT_HFP_ON                       (BT_SINK_SRV_EVENT_USER_START + 12) /**< This event notify mixer that HFP is on */
#define BT_SINK_SRV_EVENT_HFP_OFF                      (BT_SINK_SRV_EVENT_USER_START + 13) /**< This event notify mixer that HFP is off */
#define BT_SINK_SRV_EVENT_MP3_PLAY                     (BT_SINK_SRV_EVENT_USER_START + 14)


/* Add new event before this line */
#define BT_SINK_SRV_EVENT_ALL (BT_SINK_SRV_EVENT_USER_END + 1)                       /**< This event means the end of the events and receiver want to receive all the events. */

/**
 *  @brief Define for the feature configuration of sink service.
 */
#define  BT_SINK_SRV_FEATURE_NONE                                (0x0000)   /**< None. */
#define  BT_SINK_SRV_FEATURE_POWER_ON_RECONNECT                  (0x0001)   /**< Device will reconnect the trusted devices when Bluetooth switch on. */
#define  BT_SINK_SRV_FEATURE_VISIBILITY                          (0x0002)   /**< Device will be discoverable. */
#define  BT_SINK_SRV_FEATURE_IOS_HFP_CUSTOM_AT_COMMAND           (0x0004)   /**< Device will enable Apple custom AT command when HFP is connected. */
#define  BT_SINK_SRV_FEATURE_RECONNECT_ON_LINK_LOSS              (0x0008)   /**< Device will reconnect the remote device when Link lost happened. */
#define  BT_SINK_SRV_FEATURE_AUTO_CONNECT_PBAPC                  (0x0010)   /**< Device will connect pabpc to the remote device when HFP is connected.*/
#define  BT_SINK_SRV_FEATURE_SYNC_POWER_OFF                      (0x0020)   /**< Device will Power off.*/
#define  BT_SINK_SRV_FEATURE_INQUIRY_AND_CONNECT                 (0x0040)   /**< Device will inquiry the nearby Bluetooth device by EIR and initiate the AWS connection. */
#define  BT_SINK_SRV_FEATURE_LOCK_AWS_DEVICE                          (0x0080)   /**< Device will lock AWS device. */
typedef uint16_t bt_sink_srv_configurable_feature_t;                        /**<The feature configuration of sink service. */

/**
 *  @brief Define for remote device type.
 */
 #define BT_SINK_SRV_DEVICE_UNKNOWN      (0x00)     /**< unknown type */
#define BT_SINK_SRV_DEVICE_AWS_HEADSET   (0x01)     /**< Headset supported AWS. */
#define BT_SINK_SRV_DEVICE_PHONE         (0x02)     /**< Phone type. */
typedef uint8_t bt_sink_srv_device_type_t;          /**< The type of the remote device. */

/**
 *  @brief Define for Bluetooth GAP role.
 */
#define BT_SINK_SRV_GAP_ROLE_MASTER   BT_ROLE_MASTER   /**<Master. */
#define BT_SINK_SRV_GAP_ROLE_SLAVE    BT_ROLE_SLAVE    /**< Slave. */
#define BT_SINK_SRV_GAP_ROLE_UNKNOWN  0xFF             /**< Unknown role */
typedef uint8_t bt_sink_srv_gap_role_type_t;           /**< Define the gap role type. */

/**
 *  @brief Define for ios device supported hfp feature.
 */
#define BT_SINK_SRV_IOS_HF_NONE              (0x0001) /** <Reserved. */
#define BT_SINK_SRV_IOS_HF_BATTERY_REPORT    (0x0002) /**< Battery reporting. */
#define BT_SINK_SRV_IOS_HF_DOCKED            (0x0004) /**< Is docked or powered. */
#define BT_SINK_SRV_IOS_HF_SIRI_REPORT       (0x0008) /**< Siri status reporting */
#define BT_SINK_SRV_IOS_HF_NR_REPORT         (0x0010) /**< Noise reduction(NR) status reporting. */
typedef uint16_t bt_sink_srv_ios_hf_features_t;       /**< The supported features of the ios device. */

/**
 *  @brief Define for the profile type.
 */
#define BT_SINK_SRV_PROFILE_NONE           (0x00)  /**< Profile type: None. */
#define BT_SINK_SRV_PROFILE_HFP            (0x01)  /**< Profile type: Hands-free Profile(HFP). */
#define BT_SINK_SRV_PROFILE_A2DP_SINK      (0x02)  /**< Profile type: Advanced Audio Distribution Profile(A2DP) as sink. */
#define BT_SINK_SRV_PROFILE_AVRCP          (0x04)  /**< Profile type: Audio/Video Remote Control Profile(AVRCP). */
#define BT_SINK_SRV_PROFILE_PBAPC          (0x08)  /**< Profile type: Audio/Video Remote Control Profile(PBAPC). */
#define BT_SINK_SRV_PROFILE_AWS            (0x10)  /**< Profile type: Advanced Wireless Stero Profile(AWS). */
#define BT_SINK_SRV_PROFILE_A2DP_SOURCE    (0x20)  /**< Profile type: Advanced Audio Distribution Profile(A2DP)  as source. */
typedef uint8_t bt_sink_srv_profile_type_t;        /**<The feature configuration of sink service. */

/**
 *  @brief Define for the profile connection state.
 */
#define BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED  (0x00)
#define BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTING    (0x01)
#define BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED     (0x02)
#define BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTING (0x03)
typedef uint8_t bt_sink_srv_profile_connection_state_t;    /**<The profile connection state. */

/**
 *  @brief Define for the sco link state.
 */
#define BT_SINK_SRV_SCO_LINK_STATE_DISCONNECTED  (0x00)
#define BT_SINK_SRV_SCO_LINK_STATE_CONNECTED     (0x01)
typedef uint8_t bt_sink_srv_sco_link_state_t;     /**<The SCO link state. */

/**
 *  @brief Define for the the state of the sink service.
 */
#define BT_SINK_SRV_STATE_NONE              (0x0000) /**<Bluetooth is powered off. */
#define BT_SINK_SRV_STATE_POWER_ON          (0x0001) /**< Bluetooth is powered on. */
#define BT_SINK_SRV_STATE_CONNECTED         (0x0002) /**< Device is connected.*/
#define BT_SINK_SRV_STATE_STREAMING         (0x0004) /**< BT music is playing.  */
#define BT_SINK_SRV_STATE_LOCAL_PLAYING     (0x0008) /**< Local music is playing.  */
#define BT_SINK_SRV_STATE_INCOMING          (0x0010) /**< There is an incoming call. */
#define BT_SINK_SRV_STATE_OUTGOING          (0x0020) /**< There is an outgoing call. */
#define BT_SINK_SRV_STATE_ACTIVE            (0x0040) /**<There is an active call only.*/
#define BT_SINK_SRV_STATE_TWC_INCOMING      (0x0080) /**<There is an active call and a waiting incoming call. */
#define BT_SINK_SRV_STATE_TWC_OUTGOING      (0x0100) /**< There is a held call and a outgoing call. */
#define BT_SINK_SRV_STATE_HELD_ACTIVE       (0x0200) /**<There is an active call and a held call. */
#define BT_SINK_SRV_STATE_HELD_REMAINING    (0x0400) /**< There is a held call only. */
#define BT_SINK_SRV_STATE_MULTIPARTY        (0x0800) /**< There is a conference call. */
typedef uint16_t bt_sink_srv_state_t;     /**<the the state of the sink service. */

/**
 *  @brief Define for the key value.
 */
#define BT_SINK_SRV_KEY_NONE             (0x00) /**< Invalid key. */
#define BT_SINK_SRV_KEY_FUNC              (0x01)/**< Function key. */
#define BT_SINK_SRV_KEY_NEXT              (0x02) /**< Next key. */
#define BT_SINK_SRV_KEY_PREV              (0x03)/**< Previous key. */
#define BT_SINK_SRV_KEY_VOL_DOWN  (0x04)/**< Volume down key. */
#define BT_SINK_SRV_KEY_VOL_UP         (0x05) /**< Volume up key. */
typedef uint8_t bt_sink_srv_key_value_t;   

/**
 *  @brief Define for the key action.
 */
#define BT_SINK_SRV_KEY_ACT_NONE                                             (0x00)    /**< Invalid key action. */
#define BT_SINK_SRV_KEY_ACT_PRESS_DOWN                            (0x01)    /**< Press down. */
#define BT_SINK_SRV_KEY_ACT_PRESS_UP                                   (0x02)    /**< Press up. */
#define BT_SINK_SRV_KEY_ACT_LONG_PRESS_DOWN              (0x03)    /**< Long press down. */
#define BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP                      (0x04)   /**< Long press up. */
#define BT_SINK_SRV_KEY_ACT_LONG_LONG_PRESS_DOWN (0x05)   /**< Long long press down. */
#define BT_SINK_SRV_KEY_ACT_LONG_LONG_PRESS_UP        (0x06)   /**< Long long press up. */
#define BT_SINK_SRV_KEY_ACT_VERY_LONG_PRESS_DOWN (0x07)   /**< Very long press up. */
#define BT_SINK_SRV_KEY_ACT_VERY_LONG_PRESS_UP        (0x08)   /**< Very long press up. */
#define BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK                          (0x09)   /**< Double click. */
#define BT_SINK_SRV_KEY_ACT_TRIPLE_CLICK                             (0x0A)   /**< Triple click. */
typedef uint8_t bt_sink_srv_key_action_t;     /**<the the state of the sink key action. */


/**
 * @}
 */

/**
 * @defgroup Bluetooth_sink_srv_enum Enum
 * @{
 */

/**
 *  @brief This enum is the sink service status.
 */
typedef enum {
    BT_SINK_SRV_STATUS_SUCCESS        =     0,    /**< The sink service status: success. */
    BT_SINK_SRV_STATUS_FAIL           =    -1,    /**< The sink service status: fail. */
    BT_SINK_SRV_STATUS_PENDING        =    -2,    /**< The sink service status: operation is pending. */
    BT_SINK_SRV_STATUS_INVALID_PARAM  =    -3,    /**< The sink service status: invalid parameters. */
    BT_SINK_SRV_STATUS_DB_NOT_FOUND   =    -4,    /**< The sink service status: database is not found. */
    BT_SINK_SRV_STATUS_EVENT_STOP     =    -5,    /**< The sink service status: event stop looping. */
    BT_SINK_SRV_STATUS_NO_REQUEST     =    -6,    /**< The sink service status: no request is found. */
    BT_SINK_SRV_STATUS_LINK_EXIST     =    -7,    /**< The sink service status: link is already existed. */
    BT_SINK_SRV_STATUS_MAX_LINK       =    -8,    /**< The sink service status: reach the max link number. */
    BT_SINK_SRV_STATUS_NEED_RETRY     =    -9,    /**< The sink service status: the request need to be retried. */
    BT_SINK_SRV_STATUS_REQUEST_EXIST  =    -10,   /**< The sink service status: the request is already existed. */
    BT_SINK_SRV_STATUS_INVALID_STATUS =    -11,   /**< The sink service status: invalid status. */
    BT_SINK_SRV_STATUS_USER_CANCEL    =    -12    /**< The sink service status: user cancel the action. */
} bt_sink_srv_status_t;

/**
 * @}
 */


/**
 * @defgroup Bluetooth_sink_srv_struct Struct
 * @{
 * Define structures for sink service APIs and events.
 */

/**
 *  @brief This structure defines the parameters for the features to initiate reconnection after powered on or link loss ouccured .
 */
typedef struct {
    bt_sink_srv_device_type_t remote;   /** <Remote device type.*/
    uint8_t attampts;                   /** <The number of reconnect before the reconnection is established.*/
} bt_sink_srv_feature_reconnect_params_t;

/**
 *  @brief This structure defines the parameters for the features to initiate reconnection after powered on or link loss ouccured .
 */
typedef struct {
    uint8_t inquiry_duration;        /**<The maximum time of the inquiry. */
    uint8_t max_count;               /**<The maximum device count of the search, stops the inquiry, if the number of searched devices reaches the max_count.*/
} bt_sink_srv_feature_inquiry_and_connect_params_t;

/**
 *  @brief This structure defines the parameters for the features to initiate reconnection after powered on or link loss ouccured .
 */
typedef struct {
    uint32_t visibility_duration;    /**<The duration . */
    bool power_on_be_visible_once;  /**< If the device will inquiry the search for nearby devices when Bluetooth switch on. */
} bt_sink_srv_feature_visibility_params_t;

/**
 *  @brief This structure defines the parameters for the features to initiate reconnection after powered on or link loss ouccured .
 */
typedef struct {
    bt_sink_srv_ios_hf_features_t feature;
} bt_sink_srv_feature_ios_hf_custom_at_command_params_t;

/**
 * @brief This structure defines the EIR information used in the function #bt_sink_srv_get_eir().
 */
typedef struct {
    const uint8_t*    uuid_128bit;          /**< The eir data. */
    uint8_t      rssi;                        /**< The RSSI. */
}bt_sink_srv_eir_information_t;

/**
 *  @brief This structure is service table definition which get from #bt_sink_srv_get_mapping_table.
 */
typedef struct {
    bt_sink_srv_key_value_t  key_value;    /**<  Key value, which key is pressed. */
    bt_sink_srv_key_action_t key_action;   /**<  Key action, the state of the key. */
    bt_sink_srv_state_t       sink_state;  /**<  Sink state, on which state of sink service to handle the action. */
    bt_sink_srv_action_t     sink_action;  /**<  Sink action, which action of sink service will be executed. */
} bt_sink_srv_table_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_STATE_CHANGE) which indicates sink service state is changed.
 */
typedef struct {
    bt_sink_srv_state_t previous;        /**<  Previous sink service state. */
    bt_sink_srv_state_t now;             /**<  Current sink service state. */
} bt_sink_srv_state_change_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE) which indicates connection status and information.
 */
typedef struct {
    bt_bd_addr_t               bt_addr;       /**<  The remote device address. */
    bt_sink_srv_profile_type_t profile_type;  /**<  The connected profile type of this device. */
} bt_sink_srv_connection_information_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE) which indicates connection status and information.
 */
    typedef struct {
        bt_bd_addr_t address;                           /**<  The remote device address */
        bt_sink_srv_profile_type_t profile;             /**< Profile type*/
        bt_sink_srv_profile_connection_state_t state;   /**<Profile connection state */
    } bt_sink_srv_profile_connection_state_update_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE) which indicates SCO link state.
 */
typedef struct {
    bt_bd_addr_t                 address;  /**<  The remote device address. */
    bt_sink_srv_sco_link_state_t state;    /**<  The sco link state. */
    bt_hfp_audio_codec_type_t codec_type;  /**<  The codec type. */
} bt_sink_srv_hf_sco_state_update_t;

/**
  *  @brief This structure is the parameters of #BT_SINK_SRV_EVENT_VISIBILITY_STATE_UPDATE  which indicates the device visibility state.
*/
typedef struct {
    bool visibility_on;                 /**<  The visibility is on or off. */
} bt_sink_srv_visibility_state_update_t;

/**
 *  @brief This structure is the parameters of #BT_SINK_SRV_ACTION_PROFILE_CONNECT or #BT_SINK_SRV_ACTION_PROFILE_DISCONNECT 
 *            which indicates the profile to create or disconnect connection.
 */
typedef struct {
    bt_bd_addr_t                address;                    /**<  The remote device address */
    bt_sink_srv_profile_type_t  profile_connection_mask;    /**<  The mask indicates which profile the device will connect */ 
} bt_sink_srv_profile_connection_action_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION) which indicates caller information of the incoming call.
 */
typedef struct {
    bt_bd_addr_t          address;                                  /**<  The remote device address. */
    uint16_t              num_size;                                 /**<  The phone number size of the call. */
    uint8_t               number[BT_SINK_SRV_MAX_PHONE_NUMBER + 1]; /**<  The phone number of the call. */
    uint8_t               name[BT_SINK_SRV_MAX_NAME + 1];           /**<  The contact name of the call. */
    uint8_t               type;                                     /**<  The type of the call. */
    bool                  waiting;                                  /**<  Waiting incoming or not. */
} bt_sink_srv_caller_information_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE) which indicates in-call volume is changed.
 */
typedef struct {
    bt_bd_addr_t        address;             /**<  The remote device address. */
    uint8_t             current_volume;      /**<  The current speech volume. */
} bt_sink_srv_volume_change_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_HF_VOICE_RECOGNITION_CHANGED) which indicates the status of voice recognition is changed.
 */
typedef struct {
    bt_bd_addr_t        address;      /**<  The remote device address. */
    bool                enable;       /**<  The voice recognition status is enabled or not. */
} bt_sink_srv_voice_recognition_status_t;

/**
 *  @brief This structure is the parameters of #BT_SINK_SRV_EVENT_SCAN_MODE_CHANGED
 *            which indicates the device accessible mode is changed.
 */
typedef struct {
    bt_gap_scan_mode_t mode;                    /**<  The device accessible mode. */
} bt_sink_srv_scan_mode_changed_ind_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_HF_CALL_LIST_INFORMATION) which indicates the status of voice recognition is changed.
 */
typedef struct {
    bt_bd_addr_t          address;                                  /**<  The remote device address. */
    uint8_t               index;                                    /**<  Index of the call on the audio gateway. */
    uint8_t               director;                                 /**<  The director of the call, 0 - Mobile Originated, 1 - Mobile Terminated. */
    bt_hfp_call_status_t  state;                                    /**<  The call state of the call. */
    bt_hfp_call_mode_t    mode;                                     /**<  The call mode of the call. */
    uint8_t               multiple_party;                           /**<  If the call is multiple party, 0 - Not Multiparty, 1 - Multiparty. */
    uint16_t              num_size;                                 /**<  The phone number size of the call. */
    uint8_t               number[BT_SINK_SRV_MAX_PHONE_NUMBER];     /**<  The phone number of the call. */
    bt_hfp_phone_num_format_t   type;                               /**<  The address type of the call. */
    bool                  final;                                    /**<  The end of the call information. */
} bt_sink_srv_call_list_information_t;

/**
 *  @brief This enum is the muisc mode.
 */
typedef enum {
    BT_SINK_SRV_MODE_LOCAL_MUSIC,       /**<  The local music mode. */
    BT_SINK_SRV_MODE_BT_MUSIC,          /**<  The BT music mode. */

    BT_SINK_SRV_MODE_END
} bt_sink_srv_music_mode_t;

/**
 *  @brief This structure is the callback parameters of event, which indicated aws custom data received.
 */
typedef struct {
    uint8_t *buf;
    uint16_t len;
} bt_sink_srv_aws_information_t;

/**
 *  @brief This structure is the callback parameters of event, which indicated gap role
 */
typedef struct {
    bt_bd_addr_t address;   /**<  The remote device address. */
    bt_sink_srv_gap_role_type_t role;
} bt_sink_srv_gap_role_information_t;

/**
 *  @brief This structure is the callback parameters of event, which indicated aws remote triggered key action
 */
typedef struct {
    bt_sink_srv_key_value_t value;
    bt_sink_srv_key_action_t action;
} bt_sink_srv_key_information_t;

/**
 *  @brief This structure is the parameters to voice prompt tone.
 */
typedef struct {
    bt_sink_srv_action_t action;
} bt_sink_srv_voice_prompt_t;

/**
 *  @brief This structure is the callback parameters of #bt_sink_srv_event_callback, it is the union of all the events.
 */
typedef union {
    bt_sink_srv_state_change_t             state_change;            /**<  State change. */
    bt_sink_srv_caller_information_t       caller_info;             /**<  Caller infromation. */
    bt_sink_srv_connection_information_t   connection_info;         /**<  Connection infromation. */
    bt_sink_srv_volume_change_t            volume_change;           /**<  Volume change. */
    bt_sink_srv_voice_recognition_status_t voice_recognition;       /**<  Voice recognition status. */
    bt_sink_srv_call_list_information_t    call_list;               /**<  Call list information. */
    bt_sink_srv_aws_information_t          aws_data_info;           /**<  AWS received data. */
    bt_sink_srv_gap_role_information_t    role_info;               /**<  Gap Role information*/
    bt_sink_srv_key_information_t          key_info;                /**<  Key Press information*/
    bt_sink_srv_visibility_state_update_t  visibility_state;           /**<  visibility state*/
    bt_sink_srv_hf_sco_state_update_t   hf_sco_state;             /**<  HF SCO state*/
    bt_sink_srv_profile_connection_state_update_t profile_state;  /**<  Profile connection state*/
    bt_sink_srv_action_t remote_sink_action;                               /**<  A sink action from remote device*/
    bt_sink_srv_voice_prompt_t voice_prompt;                               /**<  Play voice prompt tone*/
} bt_sink_srv_event_param_t;

/**
 *  @brief This structure is the parameters of #bt_sink_srv_init which configure the features of sink service.
 */
typedef struct {
    bt_sink_srv_configurable_feature_t features;    /**<  Sink service features. */
} bt_sink_srv_features_config_t;

/**
 *  @brief This structure is the parameters to send the DTMF.
 */
typedef struct {
    bt_bd_addr_t          address;   /**<  The remote device address. */
    uint8_t               code;      /**<  The DTMF number ('0' ~ '9') or '*' or '#'. */
} bt_sink_srv_send_dtmf_t;

/**
 *  @brief This structure is the parameters to voice prompt tone.
 */
typedef struct {
    uint8_t tone_type;
    uint8_t *tone_buffer;
    uint32_t tone_size;
} bt_sink_srv_voice_prompt_tone_t;

/**
 * @}
 */
 
/**
  * @brief                             This function is to get parameters for link loss reconnection. Implementation is required.
  * @param[in] action           Sink srv action.
  * param[out] params         is the tone to play.
  * @return                           #BT_STATUS_SUCCESS, get paramter successfully.
 *                                         #BT_STATUS_FAIL, get paramter failed.
  */
bt_sink_srv_status_t bt_sink_srv_get_tone_parameter(bt_sink_srv_action_t action, bt_sink_srv_voice_prompt_tone_t *params);

/**
 * @brief                             This function is to get parameters for link loss reconnection. Implementation is required.
 * @param[in] params         None.
 * @return                           A pointer to the reconnect parameters.
 */
const bt_sink_srv_feature_reconnect_params_t *  bt_sink_srv_get_link_loss_reconnect_params(void);

/**
 * @brief                              This function is to get parameters for power on reconnection. Implementation is required.
 * @param[in] features         None.
 * @return                            A pointer to the reconnect parameters.
 */
const bt_sink_srv_feature_reconnect_params_t *  bt_sink_srv_get_power_on_reconnect_params(void);

/**
 * @brief                              This function is to get parameters for visibility. Implementation is required.
 * @param[in] features         None.
 * @return                            A pointer to the visibility parameters.
 */
const bt_sink_srv_feature_visibility_params_t * bt_sink_srv_get_visibility_params(void);

/**
 * @brief                             This function is to get parameters for inquiry and connect AWS. Implementation is required.
 * @param[in] features        None.
 * @return                           A pointer to the inquiry and connect parameters.
 */
const bt_sink_srv_feature_inquiry_and_connect_params_t* bt_sink_srv_get_inquiry_and_connect_aws_params(void);

/**
 * @brief                             This function is to get parameters for ios hf features.
 * @param[in] params         None.
 * @return                           The ios hf features parameters.
 */
bt_sink_srv_ios_hf_features_t   bt_sink_srv_get_ios_hf_features_params(void);

/**
 * @brief                             This function is to get parameters for the EIR information. Implementation is required.
 * @param[in] features         is the initialization parameter. The value is provided once it is called by sink service.
 * @return                            A pointer to the EIR data.
 */
const bt_sink_srv_eir_information_t* bt_sink_srv_get_eir(void);

/**
 * @brief                          This function get the mapping table of key and action.
 * @return                        The mapping table supplied by user.
 */
const bt_sink_srv_table_t *bt_sink_srv_get_mapping_table(void);

/**
 * @brief     It is used to configure the Bluetooth information.
 * @return    the default configuration information of sink service.
 */
const bt_gap_config_t *bt_sink_srv_gap_config(void);

/**
 * @brief                             Get the default HFP initialize parameters from sink service.
 * @param[out] param    is the initialization parameters.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_sink_srv_hf_get_init_params(bt_hfp_init_param_t *param);

/**
 * @brief                             Get the default A2DP initialize parameters from sink service.
 * @param[out] param          is the initialization parameters.
 * @return                          #BT_STATUS_SUCCESS, A2DP is enabled successfully.
 *                                      #BT_STATUS_FAIL, A2DP is enabled failed.
 */
bt_status_t bt_sink_srv_a2dp_get_init_params(bt_a2dp_init_params_t *param);

/**
 * @brief                         This function is a static callback for the application to listen to the event. Provide a user-defined callback.
 * @param[in] event_id     is the callback event ID.
 * @param[in] param        is the parameter of the callback message.
 * @return                       None.
 */
void bt_sink_srv_event_callback(bt_sink_srv_event_t event_id, bt_sink_srv_event_param_t *param);

/**
 * @brief                              This function initialze the sink service.
 * @param[in] features          is the feature configuration of sink service.
 * @return                            None.
 */
void bt_sink_srv_init(bt_sink_srv_features_config_t *features);

/**
 * @brief                  This function send a key value and key action,
 *                         sink service will control remote device according the mapping table which get from #bt_sink_srv_get_mapping_table.
 * @param[in] key_value    is current input key value.
 * @param[in] key_action   is current input key action.
 * @return                 #BT_SINK_SRV_STATUS_SUCCESS, send the action successfully.
 *                         #BT_SINK_SRV_STATUS_FAIL, fail to send the action, there is no mapping action in current status.
 */
bt_sink_srv_status_t bt_sink_srv_action_send(bt_sink_srv_action_t action, void *parameters);

/**
 * @brief                      This function get current sink state.
 * @param[in]                  None.
 * @return                     Sink current state.
 */
bt_sink_srv_state_t bt_sink_srv_state_query(void);

/**
 * @brief             This function send a key value and key action,
 *                         sink service will control remote device according the mapping table which get from #bt_sink_srv_get_mapping_table.
 * @param[in] key_value    is current input key value.
 * @param[in] key_action   is current input key action.
 * @return                 #BT_SINK_SRV_STATUS_SUCCESS, send the action successfully.
 *                         #BT_SINK_SRV_STATUS_FAIL, fail to send the action, there is no mapping action in current status.
 */
bt_sink_srv_status_t bt_sink_srv_key_action(bt_sink_srv_key_value_t key_value,
        bt_sink_srv_key_action_t key_action);

/**
 * @brief            Gets the number of connected devices.
 * @return           The number of connected devices.
 */
uint8_t bt_sink_srv_get_connected_devices_number(void);

/**
 * @brief                     Receive Bluetooth protocol stack events.
 * @param[in] msg             is the callback message type.
 * @param[in] status          is the status of the callback message.
 * @param[in] buffer          is the payload of the callback message.
 * @return                    The status of this operation returned from the callback.
 */
bt_status_t bt_sink_srv_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

#ifdef __cplusplus
}
#endif
/**
 * @}
 * @}
 */
#endif /* __BT_SINK_SRV_H__ */
