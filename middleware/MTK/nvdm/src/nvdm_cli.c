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

#ifdef MTK_NVDM_ENABLE

#include <stdlib.h>
#include <string.h>

#include "nvdm.h"

#include "nvdm_cli.h"

#define READ_HELP "config read <group_name> <data_item_name>"
#define WRITE_HELP "config write <group_name> <data_item_name> <value>"
#define RESET_HELP "config reset <group_name>"
#define SHOW_HELP "config show <group_name>"

extern void user_data_item_reset_to_default(char *group_name);
extern void user_data_item_show_value(char *group_name);


static unsigned char data_item_read(uint8_t len, char *param[])
{
#ifdef MTK_SINGLE_SKU_SUPPORT
    //single_SKU item is large than 798 bytes.
    char tmp[800];
#else
    char tmp[256];
#endif
    int  nvdm_len = sizeof(tmp);
    nvdm_status_t status;

    if (len == 2) {
        status = nvdm_read_data_item((const char *)param[0],
                                     (const char *)param[1],
                                     (uint8_t *)tmp,
                                     (uint32_t *)&nvdm_len);
        if (status == NVDM_STATUS_OK) {
            cli_puts(param[1]);
            cli_puts(" = ");
            cli_puts(tmp);
            cli_putln();
        } else {
            cli_puts("the data item is not exist");
            cli_putln();
        }
    } else {
        cli_puts(READ_HELP);
        cli_putln();
    }

    return 0;
}


static unsigned char data_item_write(uint8_t len, char *param[])
{
    nvdm_status_t status;

    if (len == 3) {
        cli_puts(param[0]);
        cli_puts("-");
        cli_puts(param[1]);
        cli_puts(" = ");
        cli_puts(param[2]);
        cli_putln();
        status = nvdm_write_data_item((const char *)param[0],
                                      (const char *) param[1],
                                      NVDM_DATA_ITEM_TYPE_STRING,
                                      (const uint8_t *)param[2],
                                      (uint32_t)strlen(param[2]));
        if (status != NVDM_STATUS_OK) {
            cli_puts("write data item error");
            cli_putln();
        } else {
            cli_puts("write data item ok");
            cli_putln();
        }
    } else {
        cli_puts(WRITE_HELP);
        cli_putln();
    }

    return 0;
}


static unsigned char data_item_reset(uint8_t len, char *param[])
{
    if (len == 0) {
        cli_puts("reset all group ");
        cli_putln();
        user_data_item_reset_to_default(NULL);
    } else if (len == 1) {
        cli_puts("reset group ");
        cli_puts(param[0]);
        cli_putln();
        user_data_item_reset_to_default(param[0]);
    } else {
        cli_puts(RESET_HELP);
        cli_putln();
    }

    return 0;
}


static unsigned char data_item_show(uint8_t len, char *param[])
{
    if (len == 0) {
        cli_puts("show all group ");
        cli_putln();
        user_data_item_show_value(NULL);
    } else if (len == 1) {
        cli_puts("show group ");
        cli_puts(param[0]);
        cli_putln();
        user_data_item_show_value(param[0]);
    } else {
        cli_puts(RESET_HELP);
        cli_putln();
    }

    return 0;
}


cmd_t nvdm_cli[] = {
    { "read", READ_HELP, data_item_read, NULL },
    { "write", WRITE_HELP, data_item_write, NULL },
    { "reset", RESET_HELP, data_item_reset, NULL },
    { "show", SHOW_HELP, data_item_show, NULL },
    { NULL, NULL, NULL, NULL }
};

#endif

