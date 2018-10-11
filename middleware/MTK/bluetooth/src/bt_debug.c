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

#include "bt_type.h"
#include "bt_debug.h"
#include "syslog.h"

#ifdef __BT_DEBUG__
log_create_module(BT, PRINT_LEVEL_INFO);
log_create_module(BTMM, PRINT_LEVEL_INFO);
log_create_module(BTHCI, PRINT_LEVEL_INFO);
log_create_module(BTL2CAP, PRINT_LEVEL_INFO);
log_create_module(BTRFCOMM, PRINT_LEVEL_INFO);
log_create_module(BTSPP, PRINT_LEVEL_INFO);
//log_create_module(BTIF, PRINT_LEVEL_INFO);
log_control_block_t log_control_block_BTIF={
	"BTIF",
	(DEBUG_LOG_OFF),
	(PRINT_LEVEL_INFO),
	print_module_log,
	dump_module_buffer
};

//#define BT_DEBUG_NO_BTIF
#define BT_DEBUG_BUFF_SIZE  150
static char bt_debug_buff[BT_DEBUG_BUFF_SIZE];
void bt_debug_log(const char *format, ...)
{
    va_list arg;

    if ( 0
#ifdef BT_DEBUG_NO_MM
        || strstr(format, "[MM]")
#endif
#ifdef BT_DEBUG_NO_TIMER
        || strstr(format, "[TIMER]")
#endif
#ifdef BT_DEBUG_NO_HCI
        || strstr(format, "[HCI]")
#endif
#ifdef BT_DEBUG_NO_BTIF
        || strstr(format, "BTIF")
#endif
#ifdef BT_DEBUG_NO_GAP
        || strstr(format, "[GAP]")
#endif
#ifdef BT_DEBUG_NO_A2DP
        || strstr(format, "[A2DP]")
#endif
#ifdef BT_DEBUG_NO_AVDTP
        || strstr(format, "[AVDTP]")
#endif

#ifdef BT_DEBUG_NO_I
        || strstr(format, "[I]")
#endif
#ifdef BT_DEBUG_NO_D
        || strstr(format, "[D]")
#endif
        ) {
        return;
    }

    va_start(arg, format);
    vsnprintf(bt_debug_buff, 150, format, arg);
    va_end(arg);
    if (strstr(bt_debug_buff, "[MM]")) {
        LOG_I(BTMM, "%s", bt_debug_buff);
    } else if (strstr(bt_debug_buff, "[RFCOMM]")) {
        LOG_I(BTRFCOMM, "%s", bt_debug_buff);
    } else if (strstr(bt_debug_buff, "[SPP]")) {
        LOG_I(BTSPP, "%s", bt_debug_buff);
    } else if (strstr(bt_debug_buff, "[L2CAP]")) {
        LOG_I(BTL2CAP, "%s", bt_debug_buff);
    } else if (strstr(bt_debug_buff, "[HCI]")) {
        LOG_I(BTHCI, "%s", bt_debug_buff);
    }  else if (strstr(bt_debug_buff, "[BTIF]")) {
        LOG_I(BTIF, "%s", bt_debug_buff);
    } 
    else {
        LOG_I(BT, "%s", bt_debug_buff);
    }
}

const char *bt_debug_bd_addr2str(const bt_bd_addr_t addr)
{
    snprintf(bt_debug_buff, BT_DEBUG_BUFF_SIZE, "%02x-%02x-%02x-%02x-%02x-%02x",
            addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    return bt_debug_buff;
}

const char *bt_debug_bd_addr2str2(const bt_bd_addr_t addr)
{
    snprintf(bt_debug_buff, BT_DEBUG_BUFF_SIZE, "LAP: %02x-%02x-%02x, UAP: %02x, NAP: %02x-%02x",
            addr[2], addr[1], addr[0], addr[3], addr[5], addr[4]);
    return bt_debug_buff;
}

const char *bt_debug_addr2str(const bt_addr_t *p)
{
    bt_bd_addr_ptr_t addr = p->addr;
    snprintf(bt_debug_buff, BT_DEBUG_BUFF_SIZE, "[%s%s] %02x-%02x-%02x-%02x-%02x-%02x",
            (p->type & 0x01) == BT_ADDR_PUBLIC ? "PUBLIC" : "RANDOM",
             p->type >=2 ?"_IDENTITY":"",
            addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    return bt_debug_buff;
}

const char *bt_debug_addr2str2(const bt_addr_t *p)
{
    bt_bd_addr_ptr_t addr = p->addr;
    snprintf(bt_debug_buff, BT_DEBUG_BUFF_SIZE, "[%s%s] LAP: %02x-%02x-%02x, UAP: %02x, NAP: %02x-%02x",
            (p->type & 0x01) == BT_ADDR_PUBLIC ? "PUBLIC" : "RANDOM",
             p->type >=2 ?"_IDENTITY":"",
            addr[2], addr[1], addr[0], addr[3], addr[5], addr[4]);
    return bt_debug_buff;
}

#endif /* BT_DEBUG */

