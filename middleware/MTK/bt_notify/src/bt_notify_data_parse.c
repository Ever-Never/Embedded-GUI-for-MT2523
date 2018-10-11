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

/*****************************************************************************
 *
 * Description:
 * ------------
 * The file is for BT notification body parsing.
 *
 ****************************************************************************/


#include "stdbool.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "bt_notify_data_parse.h"
#include "bt_notify_conversion.h"
#include "bt_notify_private.h"

#include "xml_def.h"

#include "syslog.h"
log_create_module(NOTIFY_SRV, PRINT_LEVEL_INFO);

/*****************************************************************************
 * Typedef
 *****************************************************************************/

typedef bool (*bt_notify_call)(const int8_t *, const int8_t **, const int8_t *, int32_t);

typedef struct {
    int8_t *name;
    uint16_t deep;
    uint16_t type;
    bt_notify_call call_back;
} bt_notify_node_struct_t;

typedef struct {
    uint8_t stack;
    bool is_run;
    bt_notify_parse_para_t para_flag;
    bt_parse_xml_callback callback;
    void *data;
    void *user_data;
    uint32_t curr_action_id_index;
    uint32_t curr_action_name_index;
    uint32_t curr_page_indx;
} bt_notify_parser_struct_t;


/*****************************************************************************
 * Static function
 *****************************************************************************/

static bool bt_notify_node_category(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_subtype(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_msgid(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_action(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_page_number(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_page_indx(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_group_id(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_sender(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_appid(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_title(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_content(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_timestamp(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_number(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_action_number(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_action_id(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static bool bt_notify_node_action_name(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
#ifdef _ICON_SUPPORT_
static bool bt_notify_node_icon(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
#endif /*_ICON_SUPPORT_*/
static bool bt_notify_node_missed_call_count(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len);
static void bt_notify_elem_start_hdlr(void *data, const char *el, const char **attr, int32_t error);
static void bt_notify_elem_end_hdlr(void *data, const char *el, int32_t error);
static void bt_notify_data_hdlr(void *resv, const char *el, const char *data, int32_t len, int32_t error);

/*****************************************************************************
 * Variable
 *****************************************************************************/
static bt_notify_paser_t paser_buf[BT_NOTIFY_MAX_CHANNEL];
static bt_notify_callback_item_t g_callback_t[BT_NOTIFY_MAX_CHANNEL];
static bt_notify_data_msg_context_t bt_notify_data_msg_context[BT_NOTIFY_MAX_CHANNEL];

static bt_notify_parser_struct_t g_bt_notify_parser = {0};

static const char NOTIFY_REPORT[]               = "event_report";

static const char NOTIFY_CATEGORY[]             = "category";
static const char NOTIFY_SUBTYPE[]              = "subType";
static const char NOTIFY_MSGID[]                = "msgId";
static const char NOTIFY_ACTION[]               = "action";
static const char NOTIFY_SENDER[]               = "sender";
static const char NOTIFY_APPID[]                = "appId";
static const char NOTIFY_TITLE[]                = "title";
static const char NOTIFY_CONTENT[]              = "content";
static const char NOTIFY_TIMESTAMP[]            = "timestamp";
static const char NOTIFY_NUMBER[]               = "number";
#ifdef _ICON_SUPPORT_
static const char NOTIFY_ICON[]                 = "icon";
#endif
static const char NOTIFY_MISSED_CALL_COUNT[]    = "missed_call_count";
static const char NOTIFY_ACTION_NAME[]          = "action_name";

static const char NOTIFY_NOTI_ACTION[]          = "action_num";
static const char NOTIFY_ACTION_ID[]            = "action_id";
static const char NOTIFY_PAGE_NUM[]             = "page_num";
static const char NOTIFY_PAGE[]                 = "page";
static const char NOTIFY_PAGE_INDX[]            = "index";
static const char NOTIFY_GROUP_ID[]             = "group_id";

static const char NOTIFY_CATEGORY_NOTI[]        = "notification";
static const char NOTIFY_SUBTYPE_NOTI[]         = "text";
static const char NOTIFY_SUBTYPE_SMS[]          = "sms";
static const char NOTIFY_SUBTYPE_NOTI_DELETE[]  = "notification_delete";

static const char NOTIFY_CATEGORY_CALL[]        = "call";
static const char NOTIFY_SUBTYPE_MISSED_CALL[]  = "missed_call";

static const char NOTIFY_ACTION_ADD[]           = "add";
static const char NOTIFY_ACTION_DEL[]           = "delete";
static const char NOTIFY_ACTION_Del_ALL[]       = "deleteAll";
static const char NOTIFY_ACTION_UPDATE[]        = "update";

static bt_notify_node_struct_t bt_notify_node[] = {
    {(int8_t *) &NOTIFY_CATEGORY,  BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_category},
    {(int8_t *) &NOTIFY_SUBTYPE,   BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_subtype},
    {(int8_t *) &NOTIFY_MSGID,     BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_msgid},
    {(int8_t *) &NOTIFY_ACTION,    BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_action},
    {(int8_t *) &NOTIFY_SENDER,    BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_sender},
    {(int8_t *) &NOTIFY_APPID,     BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_appid},
    {(int8_t *) &NOTIFY_PAGE_NUM,      BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_page_number},
    {(int8_t *) &NOTIFY_PAGE,    BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_ATTR,   bt_notify_node_page_indx},
    {(int8_t *) &NOTIFY_GROUP_ID,    BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_group_id},
    {(int8_t *) &NOTIFY_NOTI_ACTION,      BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_action_number},
    {(int8_t *) &NOTIFY_ACTION_ID,      BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_action_id},
    {(int8_t *) &NOTIFY_ACTION_NAME,      BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_action_name},
    {(int8_t *) &NOTIFY_TITLE,     BT_NOTIFY_NODE_LEVEL_4,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_title},
    {(int8_t *) &NOTIFY_CONTENT,   BT_NOTIFY_NODE_LEVEL_4,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_content},
    {(int8_t *) &NOTIFY_TIMESTAMP, BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_timestamp},
    {(int8_t *) &NOTIFY_NUMBER,    BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_number},

#ifdef _ICON_SUPPORT_
    {(int8_t *) &NOTIFY_ICON,      BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_icon},
#endif
    {(int8_t *) &NOTIFY_MISSED_CALL_COUNT,      BT_NOTIFY_NODE_LEVEL_3,   BT_NOTIFY_TYPE_DATA,   bt_notify_node_missed_call_count},
};

static bool bt_notify_node_category(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_category start, data = %s\r\n", data);
    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_HEADER) {

        bt_notify_header_t *header = (bt_notify_header_t *)g_bt_notify_parser.data;

        if (strcmp((const char *)data, (const char *)&NOTIFY_CATEGORY_NOTI) == 0) {

            header->category = BT_NOTIFY_CATEGORY_NOTI;
        } else if (strcmp((const char *)data, (const char *)&NOTIFY_CATEGORY_CALL) == 0) {

            header->category = BT_NOTIFY_CATEGORY_CALL;
        } else {

            header->category = BT_NOTIFY_CATEGORY_NONE;
        }
        LOG_I(NOTIFY_SRV, "[Parse] node_category end, category = %d\r\n", header->category);
    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");

    }

    return true;
}

static bool bt_notify_node_subtype(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_subtype start, data = %s\r\n", data);
    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_HEADER) {

        bt_notify_header_t *header = (bt_notify_header_t *)g_bt_notify_parser.data;

        if (strcmp((const char *)data, (const char *)&NOTIFY_SUBTYPE_NOTI) == 0) {

            header->subtype = BT_NOTIFY_SUBTYPE_TEXT;

        } else if (strcmp((const char *)data, (const char *)&NOTIFY_SUBTYPE_SMS) == 0) {

            header->subtype = BT_NOTIFY_SUBTYPE_SMS;

        } else if (strcmp((const char *)data, (const char *)&NOTIFY_SUBTYPE_MISSED_CALL) == 0) {

            header->subtype = BT_NOTIFY_SUBTYPE_MISSED_CALL;

        } else if (strcmp((const char *)data, (const char *)&NOTIFY_SUBTYPE_NOTI_DELETE) == 0) {

            header->subtype = BT_NOTIFY_SUBTYPE_NOTI_DELETE;
        }
        LOG_I(NOTIFY_SRV, "[Parse] node_subtype end, category = %d\r\n", header->subtype);

    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");

    }

    return true;
}

static bool bt_notify_node_msgid(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_msgid start, data = %d\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;
        noti_body->msg_id = (uint32_t)atoi((const char *)data);
        LOG_I(NOTIFY_SRV, "[Parse] node_msgid end, msg_id = %d\r\n", noti_body->msg_id);
    } else if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_SMS_BODY) {

        bt_notify_sms_t *sms_body = (bt_notify_sms_t *)g_bt_notify_parser.data;
        sms_body->msg_id =  (uint32_t)atoi((const char *)data);
        LOG_I(NOTIFY_SRV, "[Parse] node_msgid end, msg_id = %d\r\n", sms_body->msg_id);

    } else if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_CALL_BODY) {

        bt_notify_missed_call_t *call_body = (bt_notify_missed_call_t *)g_bt_notify_parser.data;
        call_body->msg_id =  (uint32_t)atoi((const char *)data);
        LOG_I(NOTIFY_SRV, "[Parse] node_msgid end, msg_id = %d\r\n", call_body->msg_id);
    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");
    }

    return true;
}


static bool bt_notify_node_action(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_action start, data = %s\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_HEADER) {

        bt_notify_header_t *header = (bt_notify_header_t *)g_bt_notify_parser.data;

        if (strcmp((const char *)data, (const char *)&NOTIFY_ACTION_ADD) == 0) {

            header->action = BT_NOTIFY_ACTION_ADD;

        } else if (strcmp((const char *)data, (const char *)&NOTIFY_ACTION_DEL) == 0) {

            header->action = BT_NOTIFY_ACTION_DELETE;

        } else if (strcmp((const char *)data, (const char *)&NOTIFY_ACTION_Del_ALL) == 0) {

            header->action = BT_NOTIFY_ACTION_DELETE_ALL;

        } else if (strcmp((const char *)data, (const char *)&NOTIFY_ACTION_UPDATE) == 0) {

            header->action = BT_NOTIFY_ACTION_UPDATE;
        }
        LOG_I(NOTIFY_SRV, "[Parse] node_action end, category = %d\r\n", header->action);
    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");
    }

    return true;
}

static bool bt_notify_node_sender(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t sender_len = BT_NOTIFY_MAX_SENDER_LENGTH;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_sender start, data = %s\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;
        memset(noti_body->sender, 0, BT_NOTIFY_MAX_SENDER_LENGTH * ENCODING_LENGTH);

        if (len < BT_NOTIFY_MAX_SENDER_LENGTH) {
            sender_len = len + 1;
        }
#ifdef SHOW_UCS2
        utf8_to_ucs2_string((uint8_t *)noti_body->sender, sender_len * ENCODING_LENGTH, (uint8_t *)data);
#else
        memcpy((uint8_t *)noti_body->sender, (uint8_t *)data, sender_len);
#endif

        LOG_I(NOTIFY_SRV, "[Parse] sender = %s\r\n", noti_body->sender);

    } else if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_SMS_BODY) {
        bt_notify_sms_t *sms_body = (bt_notify_sms_t *)g_bt_notify_parser.data;
        memset(sms_body->sender, 0, BT_NOTIFY_MAX_SENDER_LENGTH * ENCODING_LENGTH);

        if (len < BT_NOTIFY_MAX_SENDER_LENGTH) {
            sender_len = len + 1;
        }
#ifdef SHOW_UCS2
        utf8_to_ucs2_string((uint8_t *)sms_body->sender, sender_len * ENCODING_LENGTH, (uint8_t *)data);
#else
        memcpy(sms_body->sender, data, sender_len);
#endif
        LOG_I(NOTIFY_SRV, "[Parse] sender = %s\r\n", sms_body->sender);
    } else if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_CALL_BODY) {
        bt_notify_missed_call_t *call_body = (bt_notify_missed_call_t *)g_bt_notify_parser.data;
        memset(call_body->sender, 0, BT_NOTIFY_MAX_SENDER_LENGTH * ENCODING_LENGTH);

        if (len < BT_NOTIFY_MAX_SENDER_LENGTH) {
            sender_len = len + 1;
        }
#ifdef SHOW_UCS2
        utf8_to_ucs2_string((uint8_t *)call_body->sender, sender_len * ENCODING_LENGTH, (uint8_t *)data);
#else
        memcpy(call_body->sender, data, sender_len);
#endif
        LOG_I(NOTIFY_SRV, "[Parse] sender = %s\r\n", call_body->sender);
    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");
    }

    return true;
}

static bool bt_notify_node_appid(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_appid start, data = %d\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;
        noti_body->app_id = (uint32_t)atoi((const char *)data);
        LOG_I(NOTIFY_SRV, "[Parse] node_appid end, appid = %d\r\n", noti_body->app_id);

    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");

    }

    return true;
}

static bool bt_notify_node_title(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t title_len = BT_NOTIFY_MAX_PAGE_TITLE_LENGTH;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_title start, data = %d\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        uint32_t i = 0;
        bt_notify_page_content_list_t *page_content;
        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;

        if (len < BT_NOTIFY_MAX_PAGE_TITLE_LENGTH) {
            title_len = len + 1;
        }

        page_content = noti_body->page_content;
        if (g_bt_notify_parser.curr_page_indx >= BT_NOTIFY_PAGE_MAX_NUM) {
            return true;
        }

        for (i = 0; i < g_bt_notify_parser.curr_page_indx; i++) {
            if (i < noti_body->page_num) {

                page_content = page_content->next;
            } else {
                return true;
            }
        }

        memset(page_content->title, 0, BT_NOTIFY_MAX_PAGE_TITLE_LENGTH * ENCODING_LENGTH);
#ifdef SHOW_UCS2
        utf8_to_ucs2_string((uint8_t *)page_content->title, title_len * ENCODING_LENGTH, (uint8_t *)data);
#else
        memcpy(page_content->title, data, title_len);
#endif

        LOG_I(NOTIFY_SRV, "[Parse] node_title end, title = %d\r\n", page_content->title);

    } else {

        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");

    }

    return true;
}

static bool bt_notify_node_content(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t content_len = BT_NOTIFY_MAX_PAGE_CONTENT_LENGTH;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_content start, data = %s\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        bt_notify_page_content_list_t *page_content;
        uint32_t i;
        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;

        if (len < BT_NOTIFY_MAX_PAGE_CONTENT_LENGTH) {
            content_len = len + 1;
        }

        if (g_bt_notify_parser.curr_page_indx >= BT_NOTIFY_PAGE_MAX_NUM) {
            return true;
        }
        page_content = noti_body->page_content;
        for (i = 0; i < g_bt_notify_parser.curr_page_indx; i++) {
            if (i < noti_body->page_num) {

                page_content = page_content->next;
            } else {
                return true;
            }
        }
        memset(page_content->content, 0, BT_NOTIFY_MAX_PAGE_CONTENT_LENGTH * ENCODING_LENGTH);
#ifdef SHOW_UCS2
        utf8_to_ucs2_string((uint8_t *)page_content->content, content_len * ENCODING_LENGTH, (uint8_t *)data);
#else
        memcpy(page_content->content, data, content_len);

#endif
        page_content = noti_body->page_content;

        LOG_I(NOTIFY_SRV, "[Parse] node_content end, page content = %s\r\n", page_content->content);

    } else if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_SMS_BODY) {

        bt_notify_sms_t *sms_body = (bt_notify_sms_t *)g_bt_notify_parser.data;
#ifdef WIN32_DEBUG
        sms_body->page_content = (bt_notify_page_content_list_t *)OS_MemAlloc(sizeof(bt_notify_page_content_list_t));
#else  /* WIN32_DEBUG */
        sms_body->page_content = (bt_notify_page_content_list_t *)pvPortMalloc(sizeof(bt_notify_page_content_list_t));

#endif /* WIN32_DEBUG */
        memset(sms_body->page_content->content, 0, BT_NOTIFY_MAX_PAGE_CONTENT_LENGTH * ENCODING_LENGTH);
        sms_body->page_content->next = NULL;
        if (len < BT_NOTIFY_MAX_PAGE_CONTENT_LENGTH) {
            content_len = len + 1;
        }
#ifdef SHOW_UCS2
        utf8_to_ucs2_string((uint8_t *)sms_body->page_content->content, content_len * ENCODING_LENGTH, (uint8_t *)data);
#else  /* SHOW_UCS2 */
        memcpy(sms_body->page_content->content, data, content_len);
#endif /* SHOW_UCS2 */
        LOG_I(NOTIFY_SRV, "[Parse] node_content end,sms content = %d\r\n", sms_body->page_content);

    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");

    }

    return true;
}

static bool bt_notify_node_timestamp(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_timestamp start, data = %d\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;
        noti_body->timestamp = atoi((const char *)data);
        LOG_I(NOTIFY_SRV, "[Parse]timestamp = %d\r\n", noti_body->timestamp);

    } else if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_SMS_BODY) {

        bt_notify_sms_t *sms_body = (bt_notify_sms_t *)g_bt_notify_parser.data;
        sms_body->timestamp = atoi((const char *)data);
        LOG_I(NOTIFY_SRV, "[Parse]timestamp = %d\r\n", sms_body->timestamp);

    } else if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_CALL_BODY) {

        bt_notify_missed_call_t *call_body = (bt_notify_missed_call_t *)g_bt_notify_parser.data;
        call_body->timestamp = atoi((const char *)data);
        LOG_I(NOTIFY_SRV, "[Parse]timestamp = %d\r\n", call_body->timestamp);

    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");

    }

    return true;
}

static bool bt_notify_node_number(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_number start, data= %d\r\n", data);
    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_SMS_BODY) {

        bt_notify_sms_t *sms_body = (bt_notify_sms_t *)g_bt_notify_parser.data;
        memset(sms_body->sender_number, 0, BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH * ENCODING_LENGTH);

        if (len < BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH) {
#ifdef SHOW_UCS2
            asc_to_ucs2((int8_t *)sms_body->sender_number, (int8_t *)data);
#else /* SHOW_UCS2 */
            memcpy(sms_body->sender_number, data, len + 1);
#endif /* SHOW_UCS2 */

        } else {
#ifdef SHOW_UCS2
            asc_n_to_ucs2((int8_t *)sms_body->sender_number, (int8_t *)data, BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH - 1);
#else /* SHOW_UCS2 */

            memcpy(sms_body->sender_number, data, BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH - 1);
#endif /* SHOW_UCS2 */
        }
        LOG_I(NOTIFY_SRV, "[Parse] sms number = %d\r\n", sms_body->sender_number);
    } else if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_CALL_BODY) {

        bt_notify_missed_call_t *call_body = (bt_notify_missed_call_t *)g_bt_notify_parser.data;
        memset(call_body->sender_number, 0, BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH * ENCODING_LENGTH);

        if (len < BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH) {
#ifdef SHOW_UCS2
            asc_to_ucs2((int8_t *)call_body->sender_number, (int8_t *)data);
#else /* SHOW_UCS2 */
            memcpy(call_body->sender_number, data, len + 1);
#endif /* SHOW_UCS2 */
        } else {
#ifdef SHOW_UCS2
            asc_n_to_ucs2((int8_t *)call_body->sender_number, (int8_t *)data, BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH - 1);
#else /* SHOW_UCS2 */
            memcpy(call_body->sender_number, data, BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH - 1);
#endif /* SHOW_UCS2 */
        }

        LOG_I(NOTIFY_SRV, "[Parse] call number = %d\r\n", call_body->sender_number);
    } else {

        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");
    }

    return true;
}

#ifdef _ICON_SUPPORT_
static bool bt_notify_node_icon(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint32_t icon_len = BT_NOTIFY_MAX_ICON_SIZE;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_icon start, data = %s\r\n", data);
    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;
        if (len < BT_NOTIFY_MAX_ICON_SIZE) {
            icon_len = len + 1;
        }
        memset(noti_body->icon, 0, sizeof(bt_notify_notification_t));
        memcpy(noti_body->icon->icon_buffer, data, icon_len);
        noti_body->icon->icon_size = icon_len;
        LOG_I(NOTIFY_SRV, "[Parse] node_icon end, icon = %s\r\n", noti_body->icon->icon_buffer);

    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");
    }
    return true;
}
#endif /*_ICON_SUPPORT_ */
static bool bt_notify_node_missed_call_count(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_missed_call_count start, data = %d\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_CALL_BODY) {

        bt_notify_missed_call_t *call_body = (bt_notify_missed_call_t *)g_bt_notify_parser.data;
        call_body->missed_call_count = atoi((const char *)data);
        LOG_I(NOTIFY_SRV, "[Parse] missed_call_count=%d\r\n", call_body->missed_call_count);

    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");

    }

    return true;
}


static bool bt_notify_node_page_number(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_page_number start, data = %d\r\n", data);
    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        uint32_t i;
        bt_notify_page_content_list_t *page_content;
        bt_notify_page_content_list_t *p_page_content;

        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;
        noti_body->page_num = atoi((const char *)data);
        if (noti_body->page_num > BT_NOTIFY_PAGE_MAX_NUM) {
            noti_body->page_num = BT_NOTIFY_PAGE_MAX_NUM;
        }

        if (noti_body->page_num == 0) {
            return true;
        }

        for (i = 0; i < noti_body->page_num; i++) {
            if (i == 0) {

#ifdef WIN32_DEBUG
                p_page_content = (bt_notify_page_content_list_t *)OS_MemAlloc(sizeof(bt_notify_page_content_list_t));
#else /* WIN32_DEBUG */
                p_page_content = (bt_notify_page_content_list_t *)pvPortMalloc(sizeof(bt_notify_page_content_list_t));
#endif /* WIN32_DEBUG */
                p_page_content->next = NULL;
                noti_body->page_content = p_page_content;

            } else {

#ifdef WIN32_DEBUG
                page_content = (bt_notify_page_content_list_t *)OS_MemAlloc(sizeof(bt_notify_page_content_list_t));
#else /* WIN32_DEBUG */
                page_content = (bt_notify_page_content_list_t *)pvPortMalloc(sizeof(bt_notify_page_content_list_t));
#endif /* WIN32_DEBUG */
                page_content->next = NULL;

                for (; p_page_content->next != NULL;) {
                    p_page_content = p_page_content->next;
                }

                p_page_content->next = page_content;
            }
        }
        LOG_I(NOTIFY_SRV, "[Parse]  parse page num: %d\r\n", noti_body->page_num);
    }  else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");
    }

    return true;
}

static bool bt_notify_node_page_indx(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_page_indx start, data = %d,%d\r\n", data, g_bt_notify_parser.para_flag);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {
        if (strcmp((const char *)attr[0],(const char *)NOTIFY_PAGE_INDX) == 0) {

            if (attr[1] != NULL) {
                g_bt_notify_parser.curr_page_indx = atoi((const char *)attr[1]);
            }

        }

    } else {
        LOG_I(NOTIFY_SRV, "[Parse]Don't need to parse this part.\r\n");
    }
    LOG_I(NOTIFY_SRV, "[Parse]%d\r\n", g_bt_notify_parser.curr_page_indx);

    return true;
}


static bool bt_notify_node_group_id(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_group_id start, data = %d\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        uint32_t group_len = BT_NOTIFY_MAX_GROUP_ID_LENGTH;
        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;

        memset(noti_body->group_id, 0, BT_NOTIFY_MAX_GROUP_ID_LENGTH);

        if (data == NULL) {
            return true;
        }

        if (len < BT_NOTIFY_MAX_GROUP_ID_LENGTH) {
            group_len = len + 1;
        }

        strncpy((char *)noti_body->group_id, (const char *)data, group_len);


    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");
    }
    return true;
}

static bool bt_notify_node_action_number(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_action_number start, data = %d\r\n", data);
    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;
        noti_body->action_number = atoi((const char *)data);

        if (noti_body->action_number > BT_NOTIFY_MAX_ACTION_NUM) {
            noti_body->action_number = BT_NOTIFY_MAX_ACTION_NUM;
        }
        LOG_I(NOTIFY_SRV, "[Parse] node_action_number end, action_number = %d\r\n", noti_body->action_number);
    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");
    }

    return true;
}


static bool bt_notify_node_action_id(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint32_t id_len = BT_NOTIFY_MAX_ACTION_ID_LENGTH;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_action_id start data = %d\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;

        if (g_bt_notify_parser.curr_action_id_index >= BT_NOTIFY_MAX_ACTION_NUM) {
            return true;
        }
        memset(noti_body->action_content[g_bt_notify_parser.curr_action_id_index].action_id, 0, BT_NOTIFY_MAX_ACTION_ID_LENGTH * ENCODING_LENGTH);

        if (len < BT_NOTIFY_MAX_ACTION_ID_LENGTH) {
            id_len = len + 1;
        } else {
            return true;
        }
#ifdef SHOW_UCS2

        utf8_to_ucs2_string((uint8_t *)noti_body->action_content[g_bt_notify_parser.curr_action_id_index].action_id, id_len * ENCODING_LENGTH, (uint8_t *)data);
#else /* SHOW_UCS2 */
        memcpy(noti_body->action_content[g_bt_notify_parser.curr_action_id_index].action_id, data, id_len);
#endif /* SHOW_UCS2 */
        g_bt_notify_parser.curr_action_id_index++;
    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");
    }

    return true;
}


static bool bt_notify_node_action_name(const int8_t *elem, const int8_t **attr, const int8_t *data, int32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint32_t name_len = BT_NOTIFY_MAX_ACTION_NAME_LENGTH;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Parse] node_action_name:%s\r\n", data);

    if (g_bt_notify_parser.para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

        bt_notify_notification_t *noti_body = (bt_notify_notification_t *)g_bt_notify_parser.data;

        if (g_bt_notify_parser.curr_action_name_index >= BT_NOTIFY_MAX_ACTION_NUM) {
            return true;
        }

        memset(noti_body->action_content[g_bt_notify_parser.curr_action_name_index].action_name, 0, BT_NOTIFY_MAX_ACTION_NAME_LENGTH * ENCODING_LENGTH);

        if (len < BT_NOTIFY_MAX_ACTION_NAME_LENGTH) {
            name_len = len + 1;
#ifdef SHOW_UCS2
            utf8_to_ucs2_string(
                (uint8_t *)noti_body->action_content[g_bt_notify_parser.curr_action_name_index].action_name,
                name_len * ENCODING_LENGTH,
                (uint8_t *)data);
#else /* SHOW_UCS2 */
            memcpy((uint8_t *)noti_body->action_content[g_bt_notify_parser.curr_action_name_index].action_name, (uint8_t *)data, name_len);

#endif /* SHOW_UCS2 */
            LOG_I(NOTIFY_SRV, "[Parse] idx: %d; name:%s\r\n", g_bt_notify_parser.curr_action_name_index, noti_body->action_content[g_bt_notify_parser.curr_action_name_index].action_name);
        } else {
            name_len = BT_NOTIFY_MAX_ACTION_NAME_LENGTH-3;
#ifdef SHOW_UCS2
            utf8_to_ucs2_string(
                (uint8_t *)noti_body->action_content[g_bt_notify_parser.curr_action_name_index].action_name,
                name_len * ENCODING_LENGTH,
                (uint8_t *)data);
#else /* SHOW_UCS2 */
            memcpy((uint8_t *)noti_body->action_content[g_bt_notify_parser.curr_action_name_index].action_name, (uint8_t *)data, name_len);

#endif /* SHOW_UCS2 */
            strcat((char *)noti_body->action_content[g_bt_notify_parser.curr_action_name_index].action_name, "...");
            LOG_I(NOTIFY_SRV, "[Parse] idx: %d; name: %s\r\n", g_bt_notify_parser.curr_action_name_index, noti_body->action_content[g_bt_notify_parser.curr_action_name_index].action_name);
        }

        g_bt_notify_parser.curr_action_name_index++;
    } else {
        LOG_I(NOTIFY_SRV, "[Parse] Don't need to parse this part.\r\n");
    }

    return true;
}

/*****************************************************************************
 * FUNCTION
 *  bt_notify_elem_start_hdlr
 * DESCRIPTION
 *  Process element start.
 * PARAMETERS
 *  data  [IN]      data.
 *  el    [IN]      element.
 *  attr  [IN]      element attribute.
 *  error [IN]      error code.
 * RETURNS
 *  void.
 *****************************************************************************/
static void bt_notify_elem_start_hdlr(void *data, const char *el, const char **attr, int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i = 0, tab_num;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    tab_num = sizeof(bt_notify_node) / sizeof(bt_notify_node[0]);

    g_bt_notify_parser.stack++;
    LOG_I(NOTIFY_SRV, "[Prase]elem_start, el = %s\r\n", el);

    for (i = 0; i < tab_num; i++) {
        if (bt_notify_node[i].type != BT_NOTIFY_TYPE_ATTR ||
                strcmp((const char *)el, (const char *)bt_notify_node[i].name) != 0 ||
                bt_notify_node[i].deep != g_bt_notify_parser.stack) {
            continue;
        }

        bt_notify_node[i].call_back((const int8_t *)el, (const int8_t **)attr, NULL, 0);
        break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_notify_elem_end_hdlr
 * DESCRIPTION
 *  Process element end.
 * PARAMETERS
 *  data  [IN]      data.
 *  el    [IN]      element.
 *  error [IN]      error code.
 * RETURNS
 *  void.
 *****************************************************************************/
static void bt_notify_elem_end_hdlr(void *data, const char *el, int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool ret_val = error == XML_NO_ERROR ? true : false;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[Prase]elem_end, el = %s, error = %d\r\n", el, error);
    if (g_bt_notify_parser.stack > 0) {
        g_bt_notify_parser.stack--;
    }

    if (false == ret_val || strcmp((const char *)el, (const char *)&NOTIFY_REPORT) == 0) {

        g_bt_notify_parser.is_run = false;

        if (g_bt_notify_parser.callback != NULL) {
            g_bt_notify_parser.callback(g_bt_notify_parser.para_flag, g_bt_notify_parser.data, g_bt_notify_parser.user_data, ret_val);
        }

        g_bt_notify_parser.callback = NULL;
        g_bt_notify_parser.para_flag = BT_NOTIFY_PARSE_NONE;
        g_bt_notify_parser.data = NULL;
        g_bt_notify_parser.user_data = NULL;
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_notify_data_hdlr
 * DESCRIPTION
 *  Process element data.
 * PARAMETERS
 *  resv  [IN]      xml data.
 *  el    [IN]      element name.
 *  data  [IN]      element data.
 *  len   [IN]      length of data.
 *  error [IN]      error code.
 * RETURNS
 *  void.
 *****************************************************************************/
static void bt_notify_data_hdlr(void *resv, const char *el, const char *data, int32_t len, int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i, tab_num;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    tab_num = sizeof(bt_notify_node) / sizeof(bt_notify_node[0]);
    LOG_I(NOTIFY_SRV, "[Prase]data_hdlr, el = %s, data = %s, error = %d\r\n", el, data, error);

    if (error) {
        xml_stop_parse((XML_PARSER_STRUCT *)resv);
        return;
    }

    for (i = 0; i < tab_num; i++) {
        if (strcmp((const char *)el, (const char *)bt_notify_node[i].name) != 0 ||
                bt_notify_node[i].deep != g_bt_notify_parser.stack) {
            continue;
        }
        bt_notify_node[i].call_back((const int8_t *)el, NULL, (const int8_t *)data, len);
        break;
    }
}

int32_t bt_notify_xml_parse(uint8_t *buffer, bt_notify_parse_para_t para_flag, bt_parse_xml_callback callback, void *data, void *user_data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    int32_t ret = XML_NO_ERROR;
    XML_PARSER_STRUCT *xml_parser = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(NOTIFY_SRV, "[BT notify]xml_parse!is_run=%d, para_flag=%d, callback=%x, data=%s\r\n", g_bt_notify_parser.is_run, para_flag, callback, data);


    if (g_bt_notify_parser.is_run == true) {
        return -1;
    }

    do {
        g_bt_notify_parser.is_run = true;
        g_bt_notify_parser.para_flag = para_flag;
        g_bt_notify_parser.callback = callback;
        g_bt_notify_parser.data = data;
        g_bt_notify_parser.user_data = user_data;
        g_bt_notify_parser.stack = 0;
        g_bt_notify_parser.curr_action_id_index = 0;
        g_bt_notify_parser.curr_action_name_index = 0;


#ifdef WIN32_DEBUG
        xml_parser = (XML_PARSER_STRUCT *)OS_MemAlloc(sizeof(XML_PARSER_STRUCT));
#else /* WIN32_DEBUG */

        xml_parser = (XML_PARSER_STRUCT *)pvPortMalloc(sizeof(XML_PARSER_STRUCT));

#endif /* WIN32_DEBUG */

        ret = xml_new_parser(xml_parser);
        LOG_I(NOTIFY_SRV, "[Prase]ret = %d\r\n", ret);
        if (ret != XML_NO_ERROR) {
            break;
        }

        xml_register_element_handler(xml_parser, bt_notify_elem_start_hdlr, bt_notify_elem_end_hdlr);
        xml_register_data_handler(xml_parser, bt_notify_data_hdlr);
        ret = xml_parse_buffer(xml_parser, (const char *)buffer, BT_NOTIFY_MIN_PARSER_BUF_SIZE);
        LOG_I(NOTIFY_SRV, "[Prase]ret = %d\r\n", ret);
    } while (0);

    xml_close_parser(xml_parser);

#ifdef WIN32_DEBUG
    OS_MemFree(xml_parser);
#else /* WIN32_DEBUG */
    vPortFree(xml_parser);

#endif /* WIN32_DEBUG */


    /* error happened, clear state */
    if (ret != XML_NO_ERROR) {
        g_bt_notify_parser.is_run = false;
        g_bt_notify_parser.para_flag = BT_NOTIFY_PARSE_NONE;
        g_bt_notify_parser.callback = NULL;
        g_bt_notify_parser.data = NULL;
        g_bt_notify_parser.user_data = NULL;
        return -1;
    }

    return 0;
}

void bt_notify_itoa(int8_t *buf, uint32_t i, int32_t base)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    int8_t *s;
#define LEN   20
    int32_t rem;
    int8_t rev[LEN + 1] = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    rev[LEN] = 0;
    if (i == 0) {
        buf[0] = '0';
        ++buf;
        return;
    }
    s = &rev[LEN];
    while (i) {
        rem = i % base;
        if (rem < 10) {
            *--s = rem + '0';
        }
        i /= base;
    }
    while (*s) {
        buf[0] = *s++;
        ++buf;
    }
}

void bt_notify_data_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    memset(&bt_notify_data_msg_context, 0, sizeof(bt_notify_data_msg_context_t));

    memset(&paser_buf, 0, sizeof(bt_notify_paser_t));

    memset(&g_callback_t, 0, sizeof(bt_notify_callback_item_t));
    LOG_I(NOTIFY_SRV, "[data]data init\r\n");

}

static void bt_notify_data_parse_xml_callback(bt_notify_parse_para_t para_flag, void *data, void *user_data, bool ret_val)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    bt_notify_notification_t *noti;
    bt_notify_sms_t *sms;
    bt_notify_missed_call_t *call;
    uint32_t user_para = (uint32_t)user_data;
    uint32_t action = user_para >> 16;
    uint32_t cntx_chnl = (user_para & 0xFFFF);
    bt_notify_callback_data_t *callback_struct = bt_notify_get_callback_struct(cntx_chnl);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    LOG_I(NOTIFY_SRV, "[data prase]xml_callback,cntx_chnl = %d, para_flag = %d, data = %d, user_data = %d, ret_val = %d\r\n", cntx_chnl,
          para_flag,
          data,
          user_data,
          ret_val);

    bt_notify_data_msg_context[cntx_chnl].is_waiting_parser_callback = false;
    LOG_I(NOTIFY_SRV, "1is_waiting_parser_callback=fase\r\n");

    bt_notify_reset_parse_status(cntx_chnl);

    if (false == ret_val) {
        LOG_I(NOTIFY_SRV, "[data]parse error!\r\n");
        return ;
    }

    if (para_flag == BT_NOTIFY_PARSE_NOTI_BODY || (int)user_data == BT_NOTIFY_ACTION_DELETE) {

        noti = (bt_notify_notification_t *)data;
        callback_struct->evt_id = BT_NOTIFY_EVENT_NOTIFICATION;
        noti->action = (bt_notify_action_type_t)action;
        callback_struct->notification = *noti;
        bt_notify_callback_app(cntx_chnl);

#ifdef WIN32_DEBUG

        OS_MemFree(callback_struct->sms.page_content);
#else /*WIN32_DEBUG*/
        {

            int i, j;
            bt_notify_page_content_list_t *page_content;
            bt_notify_page_content_list_t *pre_page_content;


            for (i = callback_struct->notification.page_num; i > 0; i--) {
                page_content = callback_struct->notification.page_content;
                pre_page_content = page_content;
                for (j = 0; j < i - 1; j++) {
                    pre_page_content = page_content;
                    page_content = page_content->next;
                }

                if (page_content) {
                    pre_page_content->next = NULL;
                    vPortFree(page_content);
                    page_content = NULL;
                }
            }
        }

#endif /*WIN32_DEBUG*/
        memset(callback_struct, 0, sizeof(bt_notify_callback_data_t));


    } else if (para_flag == BT_NOTIFY_PARSE_SMS_BODY) {

        sms = (bt_notify_sms_t *)data;
        callback_struct->evt_id = BT_NOTIFY_EVENT_SMS;

        callback_struct->sms = *sms;
        bt_notify_callback_app(cntx_chnl);

        memset(callback_struct, 0, sizeof(bt_notify_callback_data_t));

#ifdef WIN32_DEBUG
        OS_MemFree(callback_struct->sms.page_content);

#else /*WIN32_DEBUG*/
        vPortFree(callback_struct->sms.page_content);

#endif /*WIN32_DEBUG*/


    } else if (para_flag == BT_NOTIFY_PARSE_CALL_BODY) {

        call = (bt_notify_missed_call_t *)data;
        callback_struct->evt_id = BT_NOTIFY_EVENT_MISSED_CALL;

        callback_struct->missed_call = *call;
        bt_notify_callback_app(cntx_chnl);

        memset(callback_struct, 0, sizeof(bt_notify_callback_data_t));

    }

}

void bt_notify_data_handle_new_msg(int32_t cntx_chnl, uint8_t *paser_buf, bt_notify_header_t *header)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint16_t content[BT_NOTIFY_MAX_PAGE_CONTENT_LENGTH];
#ifdef _ICON_SUPPORT_
    bt_notify_data_icon_t icon_t;
#endif /*_ICON_SUPPORT_*/

    bt_notify_parse_para_t para_flag = BT_NOTIFY_PARSE_NONE;
    bt_notify_body_union_t *body;
    int32_t parse_ret  = -1;
    uint32_t user_para;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(content, 0, BT_NOTIFY_MAX_PAGE_CONTENT_LENGTH);
    LOG_I(NOTIFY_SRV, "[data]handle_new, cntx_chnl = %d\r\n", cntx_chnl);

    if (false == bt_notify_check_channel_valid(cntx_chnl)) {
        return;
    }

    if (bt_notify_data_msg_context[cntx_chnl].is_waiting_parser_callback == true) {
        LOG_I(NOTIFY_SRV, "[data]is_waiting_parser_callback\r\n");
        return;
    }

    if (paser_buf) {


#ifdef WIN32_DEBUG
        body = OS_MemAlloc(sizeof(bt_notify_body_union_t));
#else /* WIN32_DEBUG */
        body = pvPortMalloc(sizeof(bt_notify_body_union_t));
#endif /* WIN32_DEBUG */
        memset(body, 0, sizeof(bt_notify_body_union_t));

        /* parse the body from xml file */
        if (header->subtype == BT_NOTIFY_SUBTYPE_TEXT || header->subtype == BT_NOTIFY_SUBTYPE_NOTI_DELETE ) {
            para_flag = BT_NOTIFY_PARSE_NOTI_BODY;
        } else if (header->subtype == BT_NOTIFY_SUBTYPE_SMS) {
            para_flag = BT_NOTIFY_PARSE_SMS_BODY;
        } else if (header->subtype == BT_NOTIFY_SUBTYPE_MISSED_CALL) {
            para_flag = BT_NOTIFY_PARSE_CALL_BODY;
        }

        if (para_flag == BT_NOTIFY_PARSE_NOTI_BODY) {

#ifdef _ICON_SUPPORT_
            body->noti.icon = &icon_t;
#endif /*_ICON_SUPPORT_*/

        }
        LOG_I(NOTIFY_SRV, "[data]%s\r\n", body);
        bt_notify_data_msg_context[cntx_chnl].is_waiting_parser_callback = true;
        LOG_I(NOTIFY_SRV, "is_waiting_parser_callback=true\r\n");
        user_para = ((uint16_t)header->action) << 16 | (uint16_t)cntx_chnl;
        parse_ret = bt_notify_xml_parse(paser_buf,
                                        para_flag,
                                        bt_notify_data_parse_xml_callback,
                                        (void *)body,
                                        (void *)user_para);
        LOG_I(NOTIFY_SRV, "[data]parse_ret: %d\r\n", parse_ret);

        /*free*/
#ifdef WIN32_DEBUG
        OS_MemFree(body);
#else /* WIN32_DEBUG */
        vPortFree(body);
#endif /* WIN32_DEBUG */
        if (parse_ret == -1) {

            bt_notify_data_msg_context[cntx_chnl].is_waiting_parser_callback = false;

            return;
        }
    }
}

void bt_notify_deinit_data(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    memset(&bt_notify_data_msg_context, 0, sizeof(bt_notify_data_msg_context_t));
    memset(&paser_buf, 0, sizeof(bt_notify_paser_t));
    memset(&g_callback_t, 0, sizeof(bt_notify_callback_item_t));
}

bt_notify_paser_t *bt_notify_get_parser_buffer(uint8_t cntx_chnl)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    return &paser_buf[cntx_chnl];
}

bt_notify_callback_data_t *bt_notify_get_callback_struct(uint8_t cntx_chnl)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    return &g_callback_t[cntx_chnl].callback_struct;
}

