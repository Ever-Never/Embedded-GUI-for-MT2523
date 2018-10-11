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


#include "hci_log.h"
#include <stdio.h>
#include <string.h>
#include "memory_attribute.h"

#include "mt2523.h"
#define HCI_MAGIC_HI    0xab
#define HCI_MAGIC_LO    0xcd
#if defined(MTK_USB_DEMO_ENABLED) && defined(MTK_PORT_SERVICE_ENABLE)
#include "serial_port.h"
#define BT_HCILOG_PORT  SERIAL_PORT_DEV_USB_COM1
extern uint32_t g_atci_port_handle;
#else
#include "hal_uart.h"
#define BT_HCILOG_PORT  HAL_UART_1
#endif

#define MEDIA_HEAD_MAGIC_HI  (0x80)
#define MEDIA_HEAD_MAGIC_LOW_V1  (0x60)
#define MEDIA_HEAD_MAGIC_LOW_V2  (0x62)
#define L2CAP_MEDIA_HEAD_LEN (16)       // L2CAP:4, MEDIA:12
#define L2CAP_MEDIA_TOTAL_LEN (24)      // HEAD:16, payload: frame_num 1, frame header: 5, payload len:2
#define HCI_CONTINUE_TAG (0x10)

static unsigned char g_hci_log_buf[2048];
static unsigned char g_a2dp_omit_buf[28];
static unsigned char* hci_log_a2dp_streaming_data_omit(int32_t buf_len, unsigned char* buf, uint32_t *out_len)
{
    if (buf_len > 26 && buf[8] == MEDIA_HEAD_MAGIC_HI && (buf[9] == MEDIA_HEAD_MAGIC_LOW_V1 || buf[9] == MEDIA_HEAD_MAGIC_LOW_V2))
    {
        uint32_t l2cap_total_len = (buf[3]<<8) | buf[2];
        uint32_t media_payload_len = l2cap_total_len - L2CAP_MEDIA_HEAD_LEN;
        memcpy(g_a2dp_omit_buf, buf, 26);
        g_a2dp_omit_buf[2] = L2CAP_MEDIA_TOTAL_LEN;
        g_a2dp_omit_buf[3] = 0;
        g_a2dp_omit_buf[4] = L2CAP_MEDIA_TOTAL_LEN - 4;
        g_a2dp_omit_buf[5] = 0;
        g_a2dp_omit_buf[26] = media_payload_len >> 8;
        g_a2dp_omit_buf[27] = media_payload_len & 0xff;
        *out_len = 28;
        return g_a2dp_omit_buf;
    }
    else if (buf_len > 159 && (buf[1]&HCI_CONTINUE_TAG) == HCI_CONTINUE_TAG)
    {
        uint32_t media_payload_len = (buf[3]<<8) | buf[2];
        memcpy(g_a2dp_omit_buf, buf, 26);
        g_a2dp_omit_buf[2] = L2CAP_MEDIA_TOTAL_LEN;
        g_a2dp_omit_buf[3] = 0;
        g_a2dp_omit_buf[26] = media_payload_len >> 8;
        g_a2dp_omit_buf[27] = media_payload_len & 0xff;
        *out_len = 28;
        return g_a2dp_omit_buf;
    }
    else
    {
        memcpy(g_hci_log_buf, buf, buf_len);
        *out_len = buf_len;
        return g_hci_log_buf;
    }
}

#if defined(MTK_USB_DEMO_ENABLED) && defined(MTK_PORT_SERVICE_ENABLE)
static int32_t hci_log_usb_send(unsigned char type, unsigned char* buf, int32_t length)
{
    unsigned char head[8] = {'\0'};
    int32_t sent_len = 0;
    serial_port_write_data_t send_data;
    serial_port_status_t status = SERIAL_PORT_STATUS_OK;
    if (g_atci_port_handle == 0 || ((g_atci_port_handle >> 16) & 0x4D56) != 0x4D56) {
        return 1;
    }
    sprintf((char *)head, "%c%c%c%c%c", HCI_MAGIC_HI, HCI_MAGIC_LO, type, (uint8_t)((length & 0xff00)>>8), (uint8_t)(length & 0xff));

    send_data.data = head;
    send_data.size = 5;
    status = serial_port_control(g_atci_port_handle, SERIAL_PORT_CMD_WRITE_DATA, (serial_port_ctrl_para_t*)&send_data);
    sent_len = send_data.ret_size;
    sent_len = 0;
    while (sent_len < length)
    {
        send_data.data = buf + sent_len;
        send_data.size = length - sent_len;
        status = serial_port_control(g_atci_port_handle, SERIAL_PORT_CMD_WRITE_DATA, (serial_port_ctrl_para_t*)&send_data);
        if (status == SERIAL_PORT_STATUS_OK)
        {
            sent_len += send_data.ret_size;
        }
    }
    return sent_len;
}
#else
static int32_t hci_log_uart_send(unsigned char type, unsigned char* buf, int32_t length)
{
    unsigned char head[8] = {'\0'};
    int32_t ava_len, w_len, sent_len; 
    sent_len = 0;
    w_len = 0;

    sprintf((char *)head, "%c%c%c%c%c", HCI_MAGIC_HI, HCI_MAGIC_LO, type, (uint8_t)((length & 0xff00)>>8), (uint8_t)(length & 0xff));
    hal_uart_send_dma(BT_HCILOG_PORT, (const uint8_t *)head, 5);

    while (sent_len < length)
    {
        ava_len = hal_uart_get_available_send_space(BT_HCILOG_PORT);
        if (ava_len > 0)
        {
            w_len = (length-sent_len) < ava_len? (length-sent_len) : ava_len;
            sent_len += hal_uart_send_dma(BT_HCILOG_PORT, (const uint8_t *)(buf+sent_len), w_len);
        }
    }
    return sent_len;

}
#endif

static int32_t hci_log(unsigned char type, unsigned char* buf, int32_t length)
{
    int32_t send_len = 0;
    /*
    printf("[HCI LOG]Type[%02X]Length[%d]Data:",type, (int)length);
    for (i=0;i<length;i++)
    {
        printf("%02X",buf[i]);
    }
    
    printf("\n");
    */
    //printf("hci\n");
    __disable_irq();
#if defined(MTK_USB_DEMO_ENABLED) && defined(MTK_PORT_SERVICE_ENABLE)
    send_len = hci_log_usb_send(type, buf, length);
#else
    send_len = hci_log_uart_send(type, buf, length);
#endif
    /*
    hal_uart_put_char(BT_HCILOG_PORT, HCI_MAGIC_HI);
    hal_uart_put_char(BT_HCILOG_PORT, HCI_MAGIC_LO);
    hal_uart_put_char(BT_HCILOG_PORT, type);
    x = (length & 0xff00)>>8;
    hal_uart_put_char(BT_HCILOG_PORT, x);
    x = length & 0xff;
    hal_uart_put_char(BT_HCILOG_PORT, x);
    for (i=0;i<length;i++)
    {
        hal_uart_put_char(BT_HCILOG_PORT, buf[i]);
    }
    */
    __enable_irq();
    
    return send_len;
}

int32_t hci_log_cmd(unsigned char* buf, int32_t length)
{
    return hci_log(HCI_COMMAND, buf, length);
}

int32_t hci_log_event(unsigned char* buf, int32_t length)
{
    memcpy(g_hci_log_buf, buf, length);
    return hci_log(HCI_EVENT, buf, length);
}

int32_t hci_log_acl_out(unsigned char* buf, int32_t length)
{
    uint32_t out_len;
    unsigned char* omit_buf = hci_log_a2dp_streaming_data_omit(length, buf, &out_len);
    return hci_log(HCI_ACL_OUT, omit_buf, out_len);
}

int32_t hci_log_acl_in(unsigned char* buf, int32_t length)
{
    uint32_t out_len;
    unsigned char* omit_buf = hci_log_a2dp_streaming_data_omit(length, buf, &out_len);
    return hci_log(HCI_ACL_IN, omit_buf, out_len);
}

