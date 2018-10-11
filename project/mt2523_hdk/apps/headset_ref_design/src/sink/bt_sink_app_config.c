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

#include "bt_sink_srv.h"
#include "bt_sink_app_main.h"

/* fota need check if power on after update, decide to turn on visiable or not */
#include "fota_gprot.h"

static const bt_sink_srv_table_t g_bt_sink_app_mapping_table_master[] = {
#ifdef MTK_INEAR_NOODLES_HEADSET_ENABLE
/*Discoverable*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON |BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_DISCOVERABLE
    },
/* Reset paired list*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON,
        BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST
    },
/* Power off system*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_VERY_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON |BT_SINK_SRV_STATE_CONNECTED |BT_SINK_SRV_STATE_STREAMING      
        |BT_SINK_SRV_STATE_LOCAL_PLAYING  | BT_SINK_SRV_STATE_INCOMING |BT_SINK_SRV_STATE_OUTGOING    
        |BT_SINK_SRV_STATE_ACTIVE  | BT_SINK_SRV_STATE_TWC_INCOMING |BT_SINK_SRV_STATE_TWC_OUTGOING     
        |BT_SINK_SRV_STATE_HELD_ACTIVE  | BT_SINK_SRV_STATE_HELD_REMAINING,
        BT_SINK_SRV_ACTION_POWER_OFF
    },        
/* Call control*/
    {        
        //Answer the incoming call.
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_INCOMING,
        BT_SINK_SRV_ACTION_ANSWER
    },
    {
        //Reject the incoming call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_INCOMING,
        BT_SINK_SRV_ACTION_REJECT
    },
    {
        //Hung up the active/held/outgoing call.
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_ACTIVE |BT_SINK_SRV_STATE_OUTGOING |BT_SINK_SRV_STATE_HELD_ACTIVE |BT_SINK_SRV_STATE_HELD_REMAINING,
        BT_SINK_SRV_ACTION_HANG_UP
    },
    {
        //Hold the active call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_ACTIVE,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Retrived the held call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_HELD_REMAINING,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Swap the active and hold call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Hold the active call and accept incoming call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Reject the waitting call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD
    },
    {
        //End the active call and answer the incoming call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER
    },
    {
        //Switch the audio path
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_ACTIVE |BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH
    },     
    {
        //Redial the last dialed number.
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_DIAL_LAST
    },
    {
        //Call volume up
        BT_SINK_SRV_KEY_PREV,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_ACTIVE | BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_CALL_VOLUME_UP
    },
    {
        //Call volume down
        BT_SINK_SRV_KEY_NEXT,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_ACTIVE | BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN
    },
/*BT Music control*/

#ifndef __BT_SINK_SRV_MUSIC_MODE_SUPPORT__    
    {
        //Play BT Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_PLAY
    },
#else /*__BT_SINK_SRV_MUSIC_MODE_SUPPORT__*/
    {
        //Play BT Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON |BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_APP_ACTION_PLAY_BY_MODE
    },
#endif /*__BT_SINK_SRV_MUSIC_MODE_SUPPORT__*/
    {
        //Pause BT Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_PAUSE
    },
    {
        // Prev Song
        BT_SINK_SRV_KEY_PREV,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_PRE_TRACK
    },
    {
         // Next Song
        BT_SINK_SRV_KEY_NEXT,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_NEXT_TRACK
    },  
    {
        //Music volume up
        BT_SINK_SRV_KEY_PREV,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_VOLUME_UP
    },
    {
        //Music volume down
        BT_SINK_SRV_KEY_NEXT,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_VOLUME_DOWN
    },
#ifndef __BT_SINK_SRV_MUSIC_MODE_SUPPORT__    
    /*Local Music control*/  
    {
        //Play local Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON,
        AUDIO_PLAYER_ACTION_PLAY
    },
    {
        //Play local Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_POWER_ON,
        AUDIO_PLAYER_ACTION_PLAY
    },
    {
        //Play local Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_CONNECTED,
        AUDIO_PLAYER_ACTION_PLAY
    },
#endif /*__BT_SINK_SRV_MUSIC_MODE_SUPPORT__*/
    {
    //Pause local Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_PAUSE
    },
    {
        // Prev Song
        BT_SINK_SRV_KEY_PREV,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_PRE_TRACK
    },    
    {
        // Next Song
        BT_SINK_SRV_KEY_NEXT,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_NEXT_TRACK
    },  
    {
        //Music volume up
        BT_SINK_SRV_KEY_PREV,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_VOLUME_UP
    },
    {
        //Music volume down
        BT_SINK_SRV_KEY_NEXT,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_VOLUME_DOWN
    },
    #ifdef BT_SINK_APP_TEST_EXIT_MASS_STORAGE
    {
        //Exit USB mass storage mode, only internal used. 
        BT_SINK_SRV_KEY_NEXT,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_POWER_ON |BT_SINK_SRV_STATE_CONNECTED |BT_SINK_SRV_STATE_STREAMING      
        |BT_SINK_SRV_STATE_LOCAL_PLAYING  | BT_SINK_SRV_STATE_INCOMING |BT_SINK_SRV_STATE_OUTGOING    
        |BT_SINK_SRV_STATE_ACTIVE  | BT_SINK_SRV_STATE_TWC_INCOMING |BT_SINK_SRV_STATE_TWC_OUTGOING     
        |BT_SINK_SRV_STATE_HELD_ACTIVE  | BT_SINK_SRV_STATE_HELD_REMAINING,
        BT_SINK_APP_EXIT_MASS_STORGE_MODE
    },
    #endif
    {
        //Music mode switch
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_POWER_ON |BT_SINK_SRV_STATE_CONNECTED |BT_SINK_SRV_STATE_STREAMING      
        |BT_SINK_SRV_STATE_LOCAL_PLAYING,
        BT_SINK_SRV_ACTION_SWITCH_MODE
    },
#else /*MTK_INEAR_NOODLES_HEADSET_ENABLE*/
/*Discoverable*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_POWER_ON |BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_DISCOVERABLE
    },
/* Reset paired list*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_TRIPLE_CLICK,
        BT_SINK_SRV_STATE_POWER_ON,
        BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST
    },
/* Switch pass through mode*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON |BT_SINK_SRV_STATE_CONNECTED ,
        BT_SINK_SRV_ACTION_SWITCH_PASS_THROUGH_MODE
    },    
/* Power off system*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_VERY_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON |BT_SINK_SRV_STATE_CONNECTED |BT_SINK_SRV_STATE_STREAMING      
        |BT_SINK_SRV_STATE_LOCAL_PLAYING  | BT_SINK_SRV_STATE_INCOMING |BT_SINK_SRV_STATE_OUTGOING    
        |BT_SINK_SRV_STATE_ACTIVE  | BT_SINK_SRV_STATE_TWC_INCOMING |BT_SINK_SRV_STATE_TWC_OUTGOING     
        |BT_SINK_SRV_STATE_HELD_ACTIVE  | BT_SINK_SRV_STATE_HELD_REMAINING,
        BT_SINK_SRV_ACTION_POWER_OFF
    },        
/* Call control*/
    {        
        //Answer the incoming call.
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_INCOMING,
        BT_SINK_SRV_ACTION_ANSWER
    },
    {
        //Reject the incoming call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_INCOMING,
        BT_SINK_SRV_ACTION_REJECT
    },
    {
        //Hung up the active/held/outgoing call.
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_ACTIVE |BT_SINK_SRV_STATE_OUTGOING |BT_SINK_SRV_STATE_HELD_ACTIVE |BT_SINK_SRV_STATE_HELD_REMAINING,
        BT_SINK_SRV_ACTION_HANG_UP
    },
    {
        //Hold the active call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_ACTIVE,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Retrived the held call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_HELD_REMAINING,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Swap the active and hold call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Hold the active call and accept incoming call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Reject the waitting call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD
    },
    {
        //End the active call and answer the incoming call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER
    },
    {
        //Switch the audio path
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_TRIPLE_CLICK,
        BT_SINK_SRV_STATE_ACTIVE |BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH
    },     
    {
        //Redial the last dialed number.
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_TRIPLE_CLICK,
        BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_DIAL_LAST
    },
    {
        //Redial the missed call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_DIAL_MISSED
    },
    {
        //Call volume up
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_ACTIVE | BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_CALL_VOLUME_UP
    },
/*BT Music control*/    
    {
        //Play BT Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_PLAY
    },
    {
        //Pause BT Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_PAUSE
    },
    {
        // Next Song
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_NEXT_TRACK
    },
    {
        //Music volume up
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_VOLUME_UP
    },
    /*Local Music control*/  
 #if 0    /*Not support mp3 playback on master and A2dp streaming interrupt local mp3 playback.*/  
    {
        //Play local Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON,
        AUDIO_PLAYER_ACTION_PLAY
    },	
#endif
    {
    //Pause local Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_PAUSE
    },
    {
        // Next Song
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_NEXT_TRACK
    },
    {
        //Music volume up
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_VOLUME_UP
    },
#endif /*MTK_INEAR_NOODLES_HEADSET_ENABLE*/      
/* Add before this line */
    {
        BT_SINK_SRV_KEY_NONE,
        BT_SINK_SRV_KEY_ACT_NONE,
        BT_SINK_SRV_STATE_NONE,
        BT_SINK_SRV_ACTION_ALL
    }  
};

static const bt_sink_srv_table_t g_bt_sink_app_mapping_table_slave[] = {
/*Inquiry*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_POWER_ON,
        BT_SINK_SRV_ACTION_INQUIRY
    },
/* Reset paired list*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_TRIPLE_CLICK,
        BT_SINK_SRV_STATE_POWER_ON,
        BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST
    },  
/* Switch pass through mode*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON | BT_SINK_SRV_STATE_CONNECTED ,
        BT_SINK_SRV_ACTION_SWITCH_PASS_THROUGH_MODE
    },       
/* Power off system*/
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_VERY_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON|BT_SINK_SRV_STATE_CONNECTED|BT_SINK_SRV_STATE_STREAMING      
        |BT_SINK_SRV_STATE_LOCAL_PLAYING|BT_SINK_SRV_STATE_INCOMING|BT_SINK_SRV_STATE_OUTGOING    
        |BT_SINK_SRV_STATE_ACTIVE|BT_SINK_SRV_STATE_TWC_INCOMING|BT_SINK_SRV_STATE_TWC_OUTGOING     
        |BT_SINK_SRV_STATE_HELD_ACTIVE|BT_SINK_SRV_STATE_HELD_REMAINING,
        BT_SINK_SRV_ACTION_POWER_OFF
    },
/* Call control*/
    {        
    //Answer the incoming call.
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_INCOMING,
        BT_SINK_SRV_ACTION_ANSWER
    },
    {
    //Reject the incoming call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_INCOMING,
        BT_SINK_SRV_ACTION_REJECT
    },
    {
        //Hung up the active/outgoing call.
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_ACTIVE |BT_SINK_SRV_STATE_OUTGOING |BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_HANG_UP
    },
    {
        //Hold the active call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_ACTIVE,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Retrived the held call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_HELD_REMAINING,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
     {
        //Swap the active and hold call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Hold the active call and accept incoming call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        //Reject the waitting call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD
    },
    {
        //End the active call and answer the incoming call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER
    },
    {
        //Switch the audio path
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_TRIPLE_CLICK,
        BT_SINK_SRV_STATE_ACTIVE |BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH
    },     
    {
        //Redial the last dialed number.
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_TRIPLE_CLICK,
        BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_DIAL_LAST
    },
    {
        //Redial the missed call
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_DIAL_MISSED
    },
    {
        //Call volume down
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_ACTIVE | BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN
    },
    /*BT Music control*/    
    {
        //Pause Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_PAUSE
    },
    {
        // Previous Song
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_PRE_TRACK
    },
    {
        //Music volume down
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_VOLUME_DOWN
    },

/*Local Music control*/    
    {
        //Play Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_POWER_ON |BT_SINK_SRV_STATE_CONNECTED,
        AUDIO_PLAYER_ACTION_PLAY
    },
    {
        //Pause Music
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_PAUSE
    },
    {
        //Previous Song
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_PRE_TRACK
    },
    {
        //Music volume down
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_LOCAL_PLAYING,
        AUDIO_PLAYER_ACTION_VOLUME_DOWN
    },   
/* Add before this line */
    {
        BT_SINK_SRV_KEY_NONE,
        BT_SINK_SRV_KEY_ACT_NONE,
        BT_SINK_SRV_STATE_NONE,
        BT_SINK_SRV_ACTION_ALL
    }
};

const bt_sink_srv_table_t *bt_sink_srv_get_mapping_table(void)
{
    if (BT_SINK_APP_DEVICE_MASTER == bt_sink_app_get_device_role()) {
        return g_bt_sink_app_mapping_table_master;
    } else {
        return g_bt_sink_app_mapping_table_slave;
    }
}

const static bt_sink_srv_feature_reconnect_params_t master_link_loss_reconnect_params = {
    .remote = BT_SINK_SRV_DEVICE_PHONE,
    .attampts = 3
};

const static bt_sink_srv_feature_reconnect_params_t slave_link_loss_reconnect_params = {
    .remote = BT_SINK_SRV_DEVICE_AWS_HEADSET,
    .attampts = 3
};

const bt_sink_srv_feature_reconnect_params_t *  bt_sink_srv_get_link_loss_reconnect_params(void)
{
    if (BT_SINK_APP_DEVICE_MASTER == bt_sink_app_get_device_role()) {
        return &master_link_loss_reconnect_params;
    } else {
        return &slave_link_loss_reconnect_params;
    }
}

const static bt_sink_srv_feature_reconnect_params_t master_power_on_reconnect_params = {
    .remote = BT_SINK_SRV_DEVICE_PHONE,
    .attampts = 3
};

const static bt_sink_srv_feature_reconnect_params_t slave_power_on_reconnect_params = {
    .remote = BT_SINK_SRV_DEVICE_AWS_HEADSET,
    .attampts = 3
};

const bt_sink_srv_feature_reconnect_params_t *  bt_sink_srv_get_power_on_reconnect_params(void)
{
    if (BT_SINK_APP_DEVICE_MASTER == bt_sink_app_get_device_role()) {
        return &master_power_on_reconnect_params;
    } else {
        return &slave_power_on_reconnect_params;
    }
}

bt_sink_srv_ios_hf_features_t master_feature = BT_SINK_SRV_IOS_HF_SIRI_REPORT | BT_SINK_SRV_IOS_HF_BATTERY_REPORT;
bt_sink_srv_ios_hf_features_t slave_feature = BT_SINK_SRV_IOS_HF_NONE;
bt_sink_srv_ios_hf_features_t   bt_sink_srv_get_ios_hf_features_params(void)
{
    if (BT_SINK_APP_DEVICE_MASTER == bt_sink_app_get_device_role()) {
        return master_feature ;
    } else {
        return slave_feature;
    }
}

const static bt_sink_srv_feature_visibility_params_t normal_visibility_params = {
    .visibility_duration = 90000,
    .power_on_be_visible_once = false
};

const static bt_sink_srv_feature_visibility_params_t fota_visibility_params = {
    .visibility_duration = 90000,
    .power_on_be_visible_once = true
};
const bt_sink_srv_feature_visibility_params_t * bt_sink_srv_get_visibility_params(void)
{
    if (fota_is_power_on_after_update()) {
        return &fota_visibility_params;
    } else {
        return &normal_visibility_params;
    }
}

