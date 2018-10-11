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


#ifndef __MLME_LOG_H_
#define __MLME_LOG_H_

/* MLME_DBG_ON  this is controled by each c file which wanted to print the log

    #define LOG_PRE_FIX   "[module name]"
    #define MLME_DBG_INFO_ON 1
    #define MLME_DBG_API_ON 1
    #define MLME_DBG_ERR_ON 1
    #include "mlme_log.h"
    
*/

#define useSYSLOG

#ifdef useSYSLOG

#ifndef LOG_PRE_FIX
#define  LOG_PRE_FIX "[MLME]"
#endif

#if(MLME_DBG_INFO_ON == 1) 
    #define MLME_DBG_INFO(_fmt,...)    LOG_I(fw_interface,LOG_PRE_FIX""_fmt,##__VA_ARGS__)
#else
    #define MLME_DBG_INFO(_fmt,...)
#endif

#if(MLME_DBG_API_ON == 1) 
    #define MLME_DBG_API(_fmt,...)     LOG_I(fw_interface,LOG_PRE_FIX""_fmt,##__VA_ARGS__)
#else
    #define MLME_DBG_API(_fmt,...)
#endif

#if(MLME_DBG_ERR_ON == 1) 
    #define MLME_DBG_ERR(_fmt,...)     LOG_E(fw_interface,LOG_PRE_FIX""_fmt,##__VA_ARGS__)
#else
    #define MLME_DBG_ERR(_fmt,...)
#endif


#if(MLME_RAW_DBG_ON == 1) 
    #define MLME_RAW_DBG(_fmt,...)     printf(_fmt,##__VA_ARGS__)
#else
    #define MLME_RAW_DBG(_fmt,...)
#endif

#else


extern int g_mlme_current_dbg_level;

#ifndef LOG_PRE_FIX
#define  LOG_PRE_FIX "[MLME]"
#endif

#define MLME_DBG_LEVEL_API   1
#define MLME_DBG_LEVEL_INFO  2
#define MLME_DBG_LEVEL_ERR   3
#define MLME_DBG_LEVEL_NONE  4

#if(MLME_DBG_INFO_ON == 1) 
    #define MLME_DBG_INFO(_fmt,...)     MLME_DBG_LEVEL_INFO < g_mlme_current_dbg_level? 0 : printf(LOG_PRE_FIX"%s,%d [INFO] "_fmt,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
    #define MLME_DBG_INFO(_fmt,...)
#endif

#if(MLME_DBG_API_ON == 1) 
    #define MLME_DBG_API(_fmt,...)     MLME_DBG_LEVEL_API < g_mlme_current_dbg_level? 0 : printf(LOG_PRE_FIX"%s,%d [API] "_fmt,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
    #define MLME_DBG_API(_fmt,...)
#endif

#if(MLME_DBG_ERR_ON == 1) 
    #define MLME_DBG_ERR(_fmt,...)     MLME_DBG_LEVEL_ERR < g_mlme_current_dbg_level? 0 : printf(LOG_PRE_FIX"%s,%d [ERROR!!!] "_fmt,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
    #define MLME_DBG_ERR(_fmt,...)
#endif
#endif //ifdef useSYSLOG
#endif
