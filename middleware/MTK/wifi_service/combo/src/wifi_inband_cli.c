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

#include <os.h>
#include <cli.h>
#include <toi.h>
#include <timer.h>
#include <os_util.h>

#include "wifi_inband.h"


#define _STEP (16)

#ifdef MTK_MINICLI_ENABLE

static uint8_t _efuse_get_free(uint8_t len, char *param[]);
static uint8_t _efuse_get(uint8_t len, char *param[]);
static uint8_t _efuse_set(uint8_t len, char *param[]);
static uint8_t _efuse_get_phy(uint8_t len, char *param[]);
static uint8_t _efuse_set_phy(uint8_t len, char *param[]);
static uint8_t _efuse_block_status(uint8_t len, char *param[]);
//static uint8_t _efuse_timer(uint8_t len, char *param[]);

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
static uint8_t _ie_set(uint8_t len, char *param[]);
static uint8_t _ie_del(uint8_t len, char *param[]);
static uint8_t _ie_clr(uint8_t len, char *param[]);
static uint8_t _ie_lst(uint8_t len, char *param[]);
#endif

static uint8_t _inband_debug_flag(uint8_t len, char *param[]);

const static cmd_t _efuse_phy_cmds[] = {
    { "get",    "show eFuse content",   _efuse_get_phy,     NULL },
    { "set",    "set eFuse content",    _efuse_set_phy,     NULL },
    { "status", "show block status",    _efuse_block_status, NULL },
    { NULL,     NULL,                   NULL,               NULL }
};


const static cmd_t _efuse_cmds[] = {
    { "free",   "show free blocks",     _efuse_get_free,    NULL },
    { "get",    "show eFuse content",   _efuse_get,         NULL },
    { "set",    "set eFuse content",    _efuse_set,         NULL },
    { "phy",    "physical access",      NULL, (cmd_t *) &_efuse_phy_cmds[0] },
    { NULL,     NULL,                   NULL,               NULL }
};

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
const static cmd_t _ie_cmds[] = {
    { "set",    "set IE: <type> <port> <id> <len> [bytes]", _ie_set, NULL },
    { "del",    "del IE: <type> <port>",                    _ie_del, NULL },
    { "clr",    "remove all IE",                            _ie_clr, NULL },
    { "lst",    "options",                                  _ie_lst, NULL },
    { NULL,     NULL,                                       NULL,    NULL }
};
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */


#ifndef __CC_ARM
const cmd_t inband_cmds_cli[] =
#else
cmd_t inband_cmds_cli[] =
#endif
{
    { "efuse",  "efuse access",         NULL, (cmd_t *) &_efuse_cmds[0] },
#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
    { "ie",     "information element",  NULL, (cmd_t *) &_ie_cmds[0] },
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */
    //{ "time",   "show timestamp",       _efuse_timer,       NULL },
    { "debug",  "show/set debug flags", _inband_debug_flag, NULL },
    { NULL,     NULL,                   NULL,               NULL }
};


static uint8_t _efuse_get_free(uint8_t len, char *param[])
{
    uint32_t free_blocks;

    if (wifi_inband_get_efuse_free(&free_blocks) < 0) {
        printf("API error\n");
        return 1;
    }

    printf("Available blocks: %u\n", (unsigned int)free_blocks);

    return 0;
}


static uint8_t _efuse_get(uint8_t len, char *param[])
{
    uint32_t    a;
    uint32_t    l;
    uint8_t     type;
    uint8_t     buf[16];
    uint16_t    s_addr;
    uint16_t    e_addr;
    int         i;

    if (len != 2) {
        printf("need address and length\n");
        return 1;
    }

    a = toi(param[0], &type);
    if (type == TOI_ERR || a & 0xF) {
        printf("invalid addr, must be multiple of 16.\n");
        return 2;
    }

    l = toi(param[1], &type);
    if (type == TOI_ERR || l & 0xF || l == 0) {
        printf("invalid length, must be a multiple of 16.\n");
        return 3;
    }

    if ((a + l) > 512) {
        printf("read range should be 0-512.\n");
        return 4;
    }

    s_addr = a;
    e_addr = a + l;

    while (s_addr < e_addr) {
        int32_t r = wifi_inband_get_efuse(EFUSE_ADDR_SPACE_LOGICAL, s_addr, buf);

        printf("%08x: ", s_addr);
        for (i = 0; i < _STEP; i++) {
            printf("%02x%c", buf[i], (i == _STEP - 1) ? '\n' : ' ');
        }

        if (r) {
            printf("read logical efuse err\n");
        }

        s_addr += _STEP;
    }

    return 0;
}


static uint8_t _efuse_get_phy(uint8_t len, char *param[])
{
    uint32_t    a;
    uint32_t    l;
    uint8_t     type;
    uint8_t     buf[16];
    uint16_t    s_addr;
    uint16_t    e_addr;
    int         i;

    if (len != 2) {
        printf("need address and length\n");
        return 1;
    }

    a = toi(param[0], &type);
    if (type == TOI_ERR || a & 0xF) {
        printf("invalid addr, must be multiple of 16.\n");
        return 2;
    }

    l = toi(param[1], &type);
    if (type == TOI_ERR || l & 0xF || l == 0) {
        printf("invalid length, must be a multiple of 16.\n");
        return 3;
    }

    if ((a + l) > 512) {
        printf("read range should be 0-512.\n");
        return 4;
    }

    s_addr = a;
    e_addr = a + l;

    while (s_addr < e_addr) {
        if (wifi_inband_get_efuse(EFUSE_ADDR_SPACE_PHYSICAL, s_addr, buf) != 0) {
            printf("read physical efuse err\n");
        } else {
            printf("%08x: ", s_addr);
            for (i = 0; i < _STEP; i++) {
                printf("%02x%c", buf[i], (i == _STEP - 1) ? '\n' : ' ');
            }
        }

        s_addr += _STEP;
    }

    return 0;
}


static uint8_t _efuse_set(uint8_t len, char *param[])
{
    uint32_t    a;
    uint32_t    magic;
    uint16_t    s_addr;
    uint8_t     type;
    uint8_t     data[16];
    int         i;
    uint8_t     ok;
    uint8_t     used;
    uint8_t     noop;
    uint8_t     reuse;
    uint32_t    free_blocks;

    if (len < 3) {
        printf("need address, magic, and at least 1 byte\n");
        return 1;
    }

    /* parse address */
    a = toi(param[0], &type);
    if (type == TOI_ERR || a >= 0x512) {
        printf("invalid addr!\n");
        return 1;
    }
    s_addr = (uint16_t)a;

    /* parse magic */
    magic = toi(param[1], &type);
    if (type == TOI_ERR) {
        printf("invalid magic!\n");
        return 2;
    }

    for (i = 2; i < len; i++) {
        a = toi(param[i], &type);
        if (type == TOI_ERR || a > 255) {
            printf("invalid: %s\n",param[i]);
            return 3;
        }
        data[i - 2] = (uint8_t)a;
    }

    if (wifi_inband_get_efuse_free(&free_blocks) < 0) {
        printf("API error\n");
        return 4;
    }

    if (wifi_inband_query_efuse(EFUSE_ADDR_SPACE_LOGICAL, s_addr, data,
                                len - 2, &ok, &used, &noop, &reuse) != 0) {
        printf("query write failed!\n");
        return 5;
    }

    if (!ok) {
        printf("no eFuse space!\n");
        return 6;
    }

    if (free_blocks < used) {
        printf("not enough blocks: need ");
        printf("%d",used);
        printf(" has ");
        printf("%d",(int)free_blocks);
        printf(".\n");
        return 6;
    }

    printf("eFuse write request: \n");
    printf("  will use: ");
    printf("%d",used);
    printf(" blocks\n");
    printf("are you sure (y/n)? ");

    if (1) {
        char c = getchar();
        if (c != 'Y') {
            printf("skip\n");
            return 0;
        }
    }

    if (wifi_inband_set_efuse(EFUSE_ADDR_SPACE_LOGICAL, s_addr, data, len - 2, magic) != 0) {
        printf("write efuse failed\n");
        return 7;
    }

    return 0;
}


static uint8_t _efuse_set_phy(uint8_t len, char *param[])
{
    uint32_t    a;
    uint32_t    magic;
    uint16_t    s_addr;
    uint8_t     type;
    uint8_t     data[16];
    int         i;
    uint8_t     ok;
    uint8_t     used;
    uint8_t     noop;
    uint8_t     reuse;
    uint32_t    free_blocks;

    if (len < 3) {
        printf("need address, magic, and at least 1 byte\n");
        return 1;
    }

    /* parse address */
    a = toi(param[0], &type);
    if (type == TOI_ERR || a >= 0x512) {
        printf("invalid addr!\n");
        return 1;
    }
    s_addr = (uint16_t)a;

    /* parse magic */
    magic = toi(param[1], &type);
    if (type == TOI_ERR) {
        printf("invalid magic!\n");
        return 2;
    }

    for (i = 2; i < len; i++) {
        a = toi(param[i], &type);
        if (type == TOI_ERR || a > 255) {
            printf("invalid: %s\n",param[i]);
            return 3;
        }
        data[i - 2] = (uint8_t)a;
    }

    if (wifi_inband_get_efuse_free(&free_blocks) < 0) {
        printf("API error\n");
        return 4;
    }

    if (wifi_inband_query_efuse(EFUSE_ADDR_SPACE_PHYSICAL, s_addr, data,
                                len - 2, &ok, &used, &noop, &reuse) != 0) {
        printf("query write failed!\n");
        return 5;
    }

    if (!ok) {
        printf("query failed\n");
        return 6;
    }

    if (free_blocks < used) {
        printf("not enough blocks: need ");
        printf("%d",used);
        printf(" has ");
        printf("%d",(int)free_blocks);
        printf(".\n");
        return 6;
    }

    printf("eFuse write request: \n");
    printf("  will use: ");
    printf("%d",used);
    printf(" blocks\n");
    printf("are you sure (Y/n)? ");

    if (1) {
        char c = getchar();
        if (c != 'Y') {
            printf("no\n");
            return 0;
        }
        printf("Y\n");
    }

    if (wifi_inband_set_efuse(EFUSE_ADDR_SPACE_PHYSICAL, s_addr, data, len - 2, magic) != 0) {
        printf("write efuse failed\n");
        return 7;
    }

    return 0;
}


static uint8_t _efuse_block_status(uint8_t len, char *param[])
{
    uint32_t    block_no;
    uint8_t     type;
    uint8_t     buf[16];
    int         i;

    if (len != 1) {
        printf("block no err!\n");
        return 1;
    }

    block_no = toi(param[0], &type);
    if (type == TOI_ERR) {
        printf("block no err!\n");
        return 2;
    }

    if (wifi_inband_get_efuse(EFUSE_ADDR_SPACE_PHYSICAL, block_no << 4, buf) != 0) {
        printf("read physical efuse err!\n");
        return 3;
    }

    for (i = 0; i < sizeof(buf); i++) {
        if (buf[i] != 0) {
            break;
        }
    }

    printf((i != sizeof(buf)) ? "used\n" : "empty\n");

    return 0;
}
#if 0
//Not sure this cli used for.
static uint8_t _efuse_timer(uint8_t len, char *param[])
{
    static uint32_t start = 0;

    if (start == 0) {
        drvGPT4Init();
        TMR_Start(4);
        start = get_current_gpt4_count();
    } else {
        uint32_t stop = get_current_gpt4_count();
        TMR_Stop(4);

        printf("delta: ");
        printf("%d",(int)(stop - start) / 192);
        printf(" us\n");
        start = 0 ;
    }

    return 0;
}
#endif

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
static uint8_t _ie_set(uint8_t len, char *param[])
{
    uint32_t type;
    uint32_t port;
    uint32_t ie_id;
    uint32_t ie_len;
    uint8_t  *ie_body;
    uint8_t  e;
    uint16_t i;

    if (len < 1 || (type = toi(param[0], &e)) == 0 || e == TOI_ERR) {
        printf("<type> err!\n");
        return 1;
    }
    type--;

    if (len < 2 || (port = toi(param[1], &e)) == 0 || e == TOI_ERR) {
        printf("<port> err!\n");
        return 2;
    }
    port--;

    if (len < 4 || (ie_id  = toi(param[2], &e)) == 0xFFF || e == TOI_ERR ||
            (ie_len = toi(param[3], &e)) == 0xFFF || e == TOI_ERR) {
        printf("<IE> err!\n");
        return 3;
    }

    ie_id  &= 0xFF;
    ie_len &= 0xFF;

    if (ie_len + 4 != len) {
        printf("IE len not match err!\n");
        return 4;
    }

    ie_body    = os_malloc(ie_len + 2);
    ie_body[0] = ie_id;
    ie_body[1] = ie_len;

    for (i = 2; i < ie_len + 2; i++) {
        ie_body[i] = toi(param[i + 2], &e);
        if (e == TOI_ERR) {
            break;
        }
    }

    if (i != ie_len + 2) {
        printf("IE bytes parse err!\n");
        os_free(ie_body);
        return 5;
    }

    wifi_inband_set_ie((wifi_inband_packet_type_t)type,
                       (wifi_inband_port_id_t)port,
                       ie_len + 2,
                       ie_body);

    os_free(ie_body);

    return 0;
}

static uint8_t _ie_del(uint8_t len, char *param[])
{
    uint32_t type;
    uint32_t port;
    uint8_t  e;

    if (len < 1 || (type = toi(param[0], &e)) == 0 || e == TOI_ERR) {
        printf("<type> err!\n");
        return 1;
    }
    type--;

    if (len < 2 || (port = toi(param[1], &e)) == 0 || e == TOI_ERR) {
        printf("<port> err!\n");
        return 2;
    }
    port--;

    wifi_inband_set_ie((wifi_inband_packet_type_t)type,
                       (wifi_inband_port_id_t)port,
                       0,
                       NULL);
    return 0;
}

static uint8_t _ie_clr(uint8_t len, char *param[])
{
    uint32_t port;
    uint8_t  e;

    if (len < 1 || (port = toi(param[0], &e)) == 0 || e == TOI_ERR) {
        printf("<port> err!\n");
        return 2;
    }
    port--;

    wifi_inband_set_ie(WIFI_PACKET_TYPE_CLEAR_ALL_ID,
                       (wifi_inband_port_id_t)port,
                       0,
                       NULL);
    return 0;
}

static uint8_t _ie_lst(uint8_t len, char *param[])
{
#define IE_REQUEST_TYPES "<type>\n" \
                         "  1: beacon\n" \
                         "  2: probe request\n" \
                         "  3: probe response\n" \
                         "  4: assoication request\n" \
                         "  5: clear assoication response\n" \
                         "  6: clear all packets\n" \
                         "<port>\n" \
                         "  1: APCLI/STA\n" \
                         "  2: AP\n" \

    printf(IE_REQUEST_TYPES);
    return 0;
}
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */

static uint8_t _inband_debug_flag(uint8_t len, char *param[])
{
    extern uint32_t g_inband_debug_feature;

    if (len == 0) {
        printf("connsys inband is %d(0x%x)\n",
               (int)g_inband_debug_feature,
               (unsigned int)g_inband_debug_feature);

        printf("debug setting are bitwise\n");
        printf("BIT 0:INBAND_DBG_MAIN\n");
    } else {
        uint32_t flags;
        uint8_t  type;

        flags = toi(param[0], &type);

        if (type == TOI_ERR) {
            return 1;
        }

        printf("Set inband debug from %d (0x%x) to %d (0x%x)\n",
               (int)g_inband_debug_feature,
               (unsigned int)g_inband_debug_feature,
               (int)flags,
               (unsigned int)flags);

        g_inband_debug_feature = flags;
    }

    return 0;
}

#endif
