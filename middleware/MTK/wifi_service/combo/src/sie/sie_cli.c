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
#include <stdlib.h>

#include "sie.h"
#include "sie_cli.h"

static uint8_t _sie_unit_test(uint8_t len, char *param[]);

const cmd_t sie_cmds[] = {
    { "test", "unit test", _sie_unit_test, NULL },
    { NULL, NULL, NULL, NULL }
};

static uint8_t _sie_unit_test(uint8_t len, char *param[])
{
    int             i;
    sie_request_t   *request = sie_request_init();
    sie_response_t  *response;
    char            payload[4] = "abcd";
    int             l;
    sie_validate_t  v;

    printf("** create request\n");
    if (NULL == request) {
        printf("request allocation failed\n");
        return 1;
    }

    for (i = 0; i < sizeof(*request); i++) {
        printf("%02x ", ((char *)request)[i] & 0xFF);
    }
    printf("\n");

    printf("** create response\n");
    response = sie_response_init(request, (uint8_t *)&payload[0], sizeof(payload));
    l        = sizeof(*response) + sizeof(payload);

    if (NULL == response) {
        printf("response allocation failed\n");
        return 2;
    }

    for (i = 0; i < l; i++) {
        printf("%02x ", ((char *)response)[i] & 0xFF);
    }
    printf("\n");

    printf("** decrypt response\n");

    l = sizeof(payload) + sizeof(*response);
    v = sie_response_decrypt(response,
                             l,
                             request);
    printf("result: %s\n", v == SIE_VALIDATE_SUCCESS ? "success" : "failure");

    for (i = 0; i < l; i++) {
        printf("%02x ", ((char *)response)[i] & 0xFF);
    }
    printf("\n");

    sie_request_deinit(request);
    sie_response_deinit(response, l);

    return 0;
}

