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

#include <bt_hci_log.h>
#include "hal_uart.h"
//#include "hci_log.h"

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_hci_log_enabled=_default_bt_hci_log_enabled")
#pragma comment(linker, "/alternatename:_hci_log_cmd=_default_hci_log_cmd")
#pragma comment(linker, "/alternatename:_hci_log_event=_default_hci_log_event")
#pragma comment(linker, "/alternatename:_hci_log_acl_out=_default_hci_log_acl_out")
#pragma comment(linker, "/alternatename:_hci_log_acl_in=_default_hci_log_acl_in")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#pragma weak bt_hci_log_enabled = default_bt_hci_log_enabled
#pragma weak hci_log_cmd = default_hci_log_cmd
#pragma weak hci_log_event = default_hci_log_event
#pragma weak hci_log_acl_out = default_hci_log_acl_out
#pragma weak hci_log_acl_in = default_hci_log_acl_in
#else
#error "Unsupported Platform"
#endif

extern bool bt_hci_log_enabled(void);
extern int32_t hci_log_cmd(unsigned char* buf, int32_t length);
extern int32_t hci_log_event(unsigned char* buf, int32_t length);
extern int32_t hci_log_acl_out(unsigned char* buf, int32_t length);
extern int32_t hci_log_acl_in(unsigned char* buf, int32_t length);

bool default_bt_hci_log_enabled(void)
{
    return FALSE;
}

int32_t default_hci_log_cmd(unsigned char* buf, int32_t length)
{
    return 0;
}

int32_t default_hci_log_event(unsigned char* buf, int32_t length)
{
    return 0;
}

int32_t default_hci_log_acl_out(unsigned char* buf, int32_t length)
{
    return 0;
}

int32_t default_hci_log_acl_in(unsigned char* buf, int32_t length)
{
    return 0;
}

void bt_hci_log(const uint8_t in, const void * data, const uint32_t data_len)
{
    const uint8_t * p = (const uint8_t *)data;

    const uint8_t h4type = *p++;
    uint8_t logtype;
    
    if (!bt_hci_log_enabled()) {
        return;
    }
    if (in == 2) {
        logtype = BT_HCI_LOG_MSG;
        --p;
    } else if (in) {
        if (h4type == 2)        logtype = BT_HCI_LOG_ACL_IN;
        else if (h4type == 3)   logtype = BT_HCI_LOG_SCO_IN;
        else                    logtype = BT_HCI_LOG_EVENT;
    } else {
        if (h4type == 2)        logtype = BT_HCI_LOG_ACL_OUT;
        else if (h4type == 3)   logtype = BT_HCI_LOG_SCO_OUT;
        else                    logtype = BT_HCI_LOG_CMD;
    }
    if (logtype == BT_HCI_LOG_CMD) {
        hci_log_cmd((unsigned char *)p, data_len - 1);
    } 
    else if (logtype == BT_HCI_LOG_EVENT){
        hci_log_event((unsigned char *)p, data_len - 1);
    }
    else if (logtype == BT_HCI_LOG_ACL_OUT){
        hci_log_acl_out((unsigned char *)p, data_len - 1);
    }
    else if (logtype == BT_HCI_LOG_ACL_IN){
        hci_log_acl_in((unsigned char *)p, data_len - 1);
    }  

}

