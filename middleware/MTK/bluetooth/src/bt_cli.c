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

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

//#include "os.h"

#include "bt_type.h"

bool is_cli_bqb_mode = false;//default is cli mode


#ifdef MTK_BLE_BQB_CLI_ENABLE
bt_status_t bt_app_io_callback(void *input, void *output);


uint8_t bt_cli_bqb(uint8_t len, char *param[])
{
    int32_t i;
    uint8_t cmd[140];
    uint32_t tt = 0;
    for (i = 0; i < len; i++) {
        memcpy(cmd + tt, param[i], strlen(param[i]));
        tt += strlen(param[i]);
        *(cmd + tt) = ' ';
        tt += 1;
    }
    *(cmd + tt - 1) = '\0';
    printf("CMD: %s\n", cmd);
    is_cli_bqb_mode = true;
    return bt_app_io_callback(cmd, NULL);

}

#endif /* MTK_BLE_BQB_CLI_ENABLE */

#ifdef MTK_BLE_CLI_ENABLE
bt_status_t bt_app_io_callback(void *input, void *output);

uint8_t bt_cli_ble(uint8_t len, char *param[])
{
    int32_t i;
    uint8_t cmd[140];
    uint32_t tt = 0;
    for (i = 0; i < len; i++) {
        memcpy(cmd + tt, param[i], strlen(param[i]));
        tt += strlen(param[i]);
        *(cmd + tt) = ' ';
        tt += 1;
    }
    *(cmd + tt - 1) = '\0';
    printf("CMD: %s\n", cmd);
    is_cli_bqb_mode = false;
    return bt_app_io_callback(cmd, NULL);

}
#endif /* MTK_BLE_CLI_ENABLE */

