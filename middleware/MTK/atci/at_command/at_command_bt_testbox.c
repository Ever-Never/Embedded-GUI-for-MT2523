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
#ifdef MTK_BT_AT_COMMAND_ENABLE
#include <string.h>
#include <stdlib.h>
// For Register AT command handler
#include "at_command.h"
#include "at_command_bt.h"
#include "syslog.h"
#include <FreeRTOS.h>

#include "queue.h"
#include "task.h"
#include "timers.h"

//For bt head file
#include "bt_gap.h"
#include "bt_system.h"
#include "nvdm.h"
#include "hal_gpio.h"
#include "hal_i2c_master.h"





//extern function
extern bt_status_t bt_driver_power_on(void);
extern bt_status_t bt_driver_power_off(void);
extern uint8_t bt_hci_event_parser(uint8_t *param, uint16_t paramLen, atci_bt_hci_event *hci_event);
extern void bt_hci_log(const uint8_t in, const void * data, const uint32_t data_len);
extern uint8_t bt_driver_send_hci_command(uint16_t opCode, uint8_t cmd_len, uint8_t * cmd);
extern bool bt_driver_register_atci_rx_callback(void * rx_callback);
extern void bt_driver_deinit_atci_mode(void);
extern bool bt_enable_relay_mode(hal_uart_port_t port);



#if 1
#define atci_cmd_hdlr_bt_TASK_NAME "Testbox BT Task"
#define atci_cmd_hdlr_bt_TASK_STACK_SIZE 1024*2 //should be fine tune
#define atci_cmd_hdlr_bt_TASK_PRIORITY 3 //should be arrange by scenario
#define atci_cmd_hdlr_bt_QUEUE_SIZE 10

typedef struct {
    int message_id;
    int param1;
    uint8_t param2[257];
} atci_cmd_hdlr_bt_testbox_message_struct_t;

typedef enum {
    TextboxType_Unknown=0,
    TextboxType_Exit_BT=1,
    TextboxType_Enter_BT,
    TextboxType_GetBitErrorRate,
    TextboxType_SetBtAddressToNvdm,
    TextboxType_GetBtAddressFromNvdm,
    TextboxType_SetBtNameToNvdm,
    TextboxType_GetBtNameFromNvdm,
    TextboxType_SetTxPower=100,
    TextboxType_MeasureRSSI,
    TextboxType_GetLocalBtAddressFromController,
    TextboxType_SetLocalBtAddressToController,
    TextboxType_ResetBT,
    TextboxType_SetInquiryMode,
    TextboxType_SetDiscoverableConnectable,
    TextboxType_SetWhiten,
    TextboxType_SetPowerLevel,
    TextboxType_TestCommandRxStart,
    TextboxType_TestCommandRxStop,
    TextboxType_TestCommandTxStart,
    TextboxType_TestCommandTxStop,
    TextboxType_GetLocalBtNameFromController,
    TextboxType_SetLocalBtNameToController,
    TextboxType_TxTone,
    TextboxType_Methodd_SetBtAddr=1000,
    TextboxType_Methodd_GetBtAddr,
    TextboxType_Methodd_PowerOnBt,
    TextboxType_Methodd_PowerOffBt,
    TextboxType_Methodd_InitialBt,
    TextboxType_Methodd_DUTTX,
    TextboxType_Methodd_DUTRX,
    TextboxType_Methodd_TBTX_Start,
    TextboxType_Methodd_TBTX_Stop,
    TextboxType_Methodd_TBRX,
    TextboxType_Methodd_SetBtName,
    TextboxType_Methodd_GetBtName,
    TextboxType_Methodd_TxToneStart,
    TextboxType_Methodd_TxToneStop,
} atci_cmd_hdlr_bt_testbox_type;

typedef enum {
    TextboxType_hcicmd_SetTxPower = 0xFC79,
    TextboxType_hcicmd_MeasureRSSI = 0x0401,
    TextboxType_hcicmd_SetBtAddr = 0xFC1A,
    TextboxType_hcicmd_GetBtAddr = 0x1009,
    TextboxType_hcicmd_SetBtName = 0x0C13,
    TextboxType_hcicmd_GetBtName = 0x0C14,
    TextboxType_hcicmd_ResetBT = 0x0C03,
    TextboxType_hcicmd_SetInquiryMode = 0x0C45,
    TextboxType_hcicmd_SetDiscoverable = 0x0C1A,
    TextboxType_hcicmd_SetWhiten = 0xFC15,
    TextboxType_hcicmd_TestCommand = 0xFC0D,
    TextboxType_hcicmd_SetTxPowerLevel = 0xFC17,
    TextboxType_hcicmd_TxTone = 0xFCD5,
    TextboxType_hcicmd_TestCommandTxStop = 0x0C03,
} atci_cmd_hdlr_bt_testbox_hcicmd;


atci_cmd_hdlr_bt_testbox_type atci_cmd_hdlr_bt_testbox_CurrentTestboxType=TextboxType_Unknown;
TaskHandle_t atci_cmd_hdlr_bt_testbox_TaskHdlr;
QueueHandle_t atci_cmd_hdlr_bt_testbox_TaskQueue;
bool atci_cmd_hdlr_bt_testbox_PowerStatus=false;
bool atci_cmd_hdlr_bt_testbox_WhetherFoundOneDevice=false;
bt_bd_addr_t atci_cmd_hdlr_bt_testbox_LocalAddr;
bool atci_cmd_hdr_bt_testbox_HciCommandFinish=false;
uint32_t atci_cmd_hdr_bt_testbox_biterrorrate=0;
char atci_cmd_hdlr_bt_testbox_LocalName[255]= {0};
bool atci_cmd_hdlr_bt_testbox_NormalBtPowerOn=false;


uint8_t atci_cmd_hdlr_bt_testbox_CheckBtaddrValid(char *bt_address_str)
{
    uint8_t result = 0;
    uint8_t is_zero = 0;
    uint8_t i = 0;
    for (i = 0; i < 12; ++i) {
        uint8_t c = bt_address_str[i];
        if ((c >= '0' && c <= '9' ) ||(c >= 'A' && c<= 'F') || (c>= 'a' && c <= 'f') ) {
            if(c != '0') {
                is_zero = 1;
            }
            continue;
        } else {
            result = 1;
            break;
        }
    }

    if (result == 0) {
        if(is_zero == 0) result = 2;
    }
    return result;
}


atci_cmd_hdlr_bt_testbox_type atci_cmd_hdlr_bt_testbox_GetTextboxType(void)
{
    return atci_cmd_hdlr_bt_testbox_CurrentTestboxType;
}
void atci_cmd_hdlr_bt_testbox_SetTextboxType(atci_cmd_hdlr_bt_testbox_type type)
{
    atci_cmd_hdlr_bt_testbox_CurrentTestboxType = type;
}

void atci_cmd_hdlr_bt_testbox_RxCallback(uint8_t *data, uint32_t dataLen)
{
    atci_cmd_hdlr_bt_testbox_message_struct_t msg = {0,0,{0}};
    msg.message_id = ATCI_BT_COMMAND_HCI_EVENT;
    msg.param1 = dataLen;
    memcpy(msg.param2, data, dataLen);
    xQueueSendFromISR(atci_cmd_hdlr_bt_testbox_TaskQueue, &msg, 0);

    return;
}

uint8_t atci_cmd_hdlr_bt_testbox_HciEventParser(uint8_t *param, uint16_t paramLen, atci_bt_hci_event *hci_event)
{
    uint8_t data[260];
    uint16_t cmdOpcode;
    atci_response_t output = {{0}};
    hci_event->event = 0;
    hci_event->status = 0;
    hci_event->handle = 0;
    hci_event->parmslen = 0;
    memset(hci_event->parms, 0, 256);

    if (param[1] != (paramLen - 2))
    {
        return 1;
    }
    memcpy(data, param, paramLen);
    hci_event->event = data[0];
    hci_event->parmslen = data[1];
    memcpy(hci_event->parms, &data[2], hci_event->parmslen);

    switch(hci_event->event)
    {
    case ATCI_HCE_INQUIRY_COMPLETE:
        hci_event->status = data[2];
        if(false == atci_cmd_hdlr_bt_testbox_WhetherFoundOneDevice) {
            output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        } else {
            output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        }
        output.response_len = 0;
        atci_send_response(&output);
        atci_cmd_hdlr_bt_testbox_WhetherFoundOneDevice = false;
        atci_cmd_hdr_bt_testbox_HciCommandFinish = true;
        break;
    case ATCI_HCE_REMOTE_NAME_REQ_COMPLETE:
    case ATCI_HCE_COMMAND_STATUS:
    case ATCI_HCE_ROLE_CHANGE:
    case ATCI_HCE_SIMPLE_PAIRING_COMPLETE:
        hci_event->status = data[2];
        break;
    case ATCI_HCE_CONNECT_COMPLETE:
    case ATCI_HCE_DISCONNECT_COMPLETE:
    case ATCI_HCE_AUTH_COMPLETE:
    case ATCI_HCE_ENCRYPT_CHNG:
    case ATCI_HCE_CHNG_CONN_LINK_KEY_COMPLETE:
    case ATCI_HCE_MASTER_LINK_KEY_COMPLETE:
    case ATCI_HCE_READ_REMOTE_FEATURES_COMPLETE:
    case ATCI_HCE_READ_REMOTE_VERSION_COMPLETE:
    case ATCI_HCE_QOS_SETUP_COMPLETE:
    case ATCI_HCE_MODE_CHNG:
    case ATCI_HCE_READ_CLOCK_OFFSET_COMPLETE:
    case ATCI_HCE_CONN_PACKET_TYPE_CHNG:
    case ATCI_HCE_FLOW_SPECIFICATION_COMPLETE:
    case ATCI_HCE_READ_REMOTE_EXT_FEAT_COMPLETE:
    case ATCI_HCE_SYNC_CONNECT_COMPLETE:
    case ATCI_HCE_SYNC_CONN_CHANGED:
    case ATCI_HCE_SNIFF_SUBRATING_EVENT:
    case ATCI_HCE_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT:
        hci_event->status = data[2];
		hci_event->handle = *((uint16_t *)(&(data[3])));
        break;
    case ATCI_HCE_COMMAND_COMPLETE:
        hci_event->status = data[5];
        memcpy(&cmdOpcode, data + 3, 2);

        switch(cmdOpcode)
        {
        case ATCI_HCC_READ_LMP_HANDLE:
        case ATCI_HCC_ROLE_DISCOVERY:
        case ATCI_HCC_READ_LINK_POLICY:
        case ATCI_HCC_WRITE_LINK_POLICY:
        case ATCI_HCC_SNIFF_SUBRATING:
        case ATCI_HCC_FLUSH:
        case ATCI_HCC_READ_AUTO_FLUSH_TIMEOUT:
        case ATCI_HCC_WRITE_AUTO_FLUSH_TIMEOUT:
        case ATCI_HCC_READ_XMIT_POWER_LEVEL:
        case ATCI_HCC_READ_LINK_SUPERV_TIMEOUT:
        case ATCI_HCC_WRITE_LINK_SUPERV_TIMEOUT:
        case ATCI_HCC_READ_FAILED_CONTACT_COUNT:
        case ATCI_HCC_RESET_FAILED_CONTACT_COUNT:
        case ATCI_HCC_GET_LINK_QUALITY:
        case ATCI_HCC_READ_RSSI:
        case ATCI_HCC_READ_AFH_CHANNEL_MAP:
        case ATCI_HCC_READ_CLOCK:
        case ATCI_HCC_LE_READ_CHANNEL_MAP:
        case ATCI_HCC_LE_LONG_TERM_KEY_REQ_REPL:
        case ATCI_HCC_LE_LONG_TERM_KEY_REQ_NEG_REPL:
			hci_event->handle = *((uint16_t *)(&(data[6])));
            break;
        }
        switch(atci_cmd_hdlr_bt_testbox_GetTextboxType()) {
        case TextboxType_Methodd_DUTTX:
        case TextboxType_Methodd_TBTX_Start:
        case TextboxType_Methodd_TBTX_Stop:
        case TextboxType_Methodd_InitialBt:
        case TextboxType_ResetBT:
        case TextboxType_SetTxPower:
        case TextboxType_SetLocalBtAddressToController:
        case TextboxType_SetInquiryMode:
        case TextboxType_SetDiscoverableConnectable:
        case TextboxType_SetWhiten:
        case TextboxType_SetPowerLevel:
        case TextboxType_TestCommandRxStart:
        case TextboxType_TestCommandTxStart:
        case TextboxType_TestCommandTxStop:
        case TextboxType_SetLocalBtNameToController:
            if(TextboxType_hcicmd_ResetBT == cmdOpcode ||
                    TextboxType_hcicmd_SetTxPower == cmdOpcode ||
                    TextboxType_hcicmd_SetBtAddr == cmdOpcode ||
                    TextboxType_hcicmd_SetInquiryMode== cmdOpcode ||
                    TextboxType_hcicmd_SetDiscoverable== cmdOpcode ||
                    TextboxType_hcicmd_SetWhiten== cmdOpcode ||
                    TextboxType_hcicmd_SetTxPowerLevel== cmdOpcode ||
                    TextboxType_hcicmd_TestCommand == cmdOpcode ||
                    TextboxType_hcicmd_TestCommandTxStop == cmdOpcode ||
                    TextboxType_hcicmd_SetBtName == cmdOpcode) {
                sprintf((char *)output.response_buf, "+EBTTB:%d,%d\r\n",
                        atci_cmd_hdlr_bt_testbox_GetTextboxType(),hci_event->status);
                output.response_len = strlen((char *)output.response_buf);
                if(BT_STATUS_SUCCESS == hci_event->status) {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&output);
            }

            break;
        case TextboxType_TestCommandRxStop:
        case TextboxType_Methodd_DUTRX:
            if(TextboxType_hcicmd_TestCommand == cmdOpcode) {
                uint32_t bitErrorRate = (hci_event->parms[19]<<24  | hci_event->parms[18]<<16  |
                                         hci_event->parms[17]<<8  | hci_event->parms[16]<<0) / 100;
                atci_cmd_hdr_bt_testbox_biterrorrate = bitErrorRate;

                sprintf((char *)output.response_buf, "+EBTTB:%d,%ld\r\n",
                        atci_cmd_hdlr_bt_testbox_GetTextboxType(),bitErrorRate);
                output.response_len = strlen((char *)output.response_buf);
                if(BT_STATUS_SUCCESS == hci_event->status) {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&output);
            }

            break;
        case TextboxType_GetLocalBtAddressFromController:
            if(TextboxType_hcicmd_GetBtAddr == cmdOpcode) {
                sprintf((char *)output.response_buf, "+EBTTB:%d,%x:%x:%x:%x:%x:%x\r\n",
                        atci_cmd_hdlr_bt_testbox_GetTextboxType(),
                        hci_event->parms[9], hci_event->parms[8], hci_event->parms[7],
                        hci_event->parms[6], hci_event->parms[5], hci_event->parms[4]);
                output.response_len = strlen((char *)output.response_buf);
                if(BT_STATUS_SUCCESS == hci_event->status) {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&output);
            }

            break;
        case TextboxType_GetLocalBtNameFromController:
            if(TextboxType_hcicmd_GetBtName == cmdOpcode) {
                memcpy(atci_cmd_hdlr_bt_testbox_LocalName , hci_event->parms+4, hci_event->parmslen - 4);
                sprintf((char *)output.response_buf, "+EBTTB:%d,%s\r\n",
                        atci_cmd_hdlr_bt_testbox_GetTextboxType(),atci_cmd_hdlr_bt_testbox_LocalName);
                output.response_len = strlen((char *)output.response_buf);
                if(BT_STATUS_SUCCESS == hci_event->status) {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&output);
            }

            break;
        case TextboxType_Methodd_TxToneStart:
            if(TextboxType_hcicmd_TxTone== cmdOpcode) {
                sprintf((char *)output.response_buf, "+EBTTB:%d,%d\r\n",
                        atci_cmd_hdlr_bt_testbox_GetTextboxType(),hci_event->status);
                output.response_len = strlen((char *)output.response_buf);
                if(BT_STATUS_SUCCESS == hci_event->status) {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&output);
            }

            break;
        case TextboxType_Methodd_TxToneStop:
            if(TextboxType_hcicmd_ResetBT == cmdOpcode) {
                sprintf((char *)output.response_buf, "+EBTTB:%d,%d\r\n",
                        atci_cmd_hdlr_bt_testbox_GetTextboxType(),hci_event->status);
                output.response_len = strlen((char *)output.response_buf);
                if(BT_STATUS_SUCCESS == hci_event->status) {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&output);
            }

            break;
        default:
            break;
        }

        atci_cmd_hdr_bt_testbox_HciCommandFinish = true;
        break;
    case ATCI_HCE_FLUSH_OCCURRED:
    case ATCI_HCE_MAX_SLOTS_CHNG:
    case ATCI_HCE_QOS_VIOLATION:
    case ATCI_HCE_LINK_SUPERVISION_TIMEOUT_CHNG:
    case ATCI_HCE_ENHANCED_FLUSH_COMPLETE:
        hci_event->handle = *((uint16_t *)(&(data[2])));
        break;
    case ATCI_HCE_LE_META_EVENT:
        switch(data[2])
        {
        case ATCI_HCE_LE_CONNECT_COMPLETE:
            hci_event->status = data[3];
            hci_event->handle = *((uint16_t *)(&(data[4])));
            break;
        case ATCI_HCE_LE_CONNECT_UPDATE_COMPLETE:
            hci_event->status = data[3];
            hci_event->handle = *((uint16_t *)(&(data[4])));
            break;
        case ATCI_HCE_LE_READ_REMOTE_FEATURES_COMPLETE:
            hci_event->status = data[3];
            hci_event->handle = *((uint16_t *)(&(data[4])));
            break;
        case ATCI_HCE_LE_LONG_TERM_KEY_REQUEST_EVENT:
            hci_event->handle = *((uint16_t *)(&(data[3])));
            break;
        case ATCI_HCE_LE_ENHANCED_CONNECT_COMPLETE:
            hci_event->status = data[3];
            hci_event->handle = *((uint16_t *)(&(data[4])));
            break;
        }
        break;
    case ATCI_HCE_CONNECT_REQUEST:
        //No status or hci handle
        break;
    case ATCI_HCE_INQUIRY_RESULT:
        //No status or hci handle
        break;
    case ATCI_HCE_HARDWARE_ERROR:
        //no status/hci handle
        break;
    case ATCI_HCE_NUM_COMPLETED_PACKETS:
        //for acl data should not used
        break;
    case ATCI_HCE_RETURN_LINK_KEYS:
        //No status/hci handle
        break;
    case ATCI_HCE_PIN_CODE_REQ:
        //No status/hci handle
        break;
    case ATCI_HCE_LINK_KEY_REQ:
        //No status/hci handle
        break;
    case ATCI_HCE_LINK_KEY_NOTIFY:
        //No status/hci handle
        break;
    case ATCI_HCE_LOOPBACK_COMMAND:
        //No status/hci handle
        break;
    case ATCI_HCE_DATA_BUFFER_OVERFLOW:
        //for acl data should not used
        break;
    case ATCI_HCE_PAGE_SCAN_REPETITION_MODE:
        //No status/hci handle
        break;
    case ATCI_HCE_INQUIRY_RESULT_WITH_RSSI:
    case ATCI_HCE_INQUIRY_RESULT_WITH_EIR:
        //No status/hci handle
        LOG_I(atci_bt, "bt addr :%x:%x:%x:%x:%x:%x,RSSI=%ddbm",
              hci_event->parms[6], hci_event->parms[5], hci_event->parms[4],
              hci_event->parms[3], hci_event->parms[2], hci_event->parms[1],
              hci_event->parms[14]-256);
        sprintf((char *)output.response_buf, "+EBTTB:%d,%d,%x:%x:%x:%x:%x:%x\r\n",
                atci_cmd_hdlr_bt_testbox_GetTextboxType(),
                hci_event->parms[14]-256,
                hci_event->parms[6], hci_event->parms[5], hci_event->parms[4],
                hci_event->parms[3], hci_event->parms[2], hci_event->parms[1]);
        output.response_len = strlen((char *)output.response_buf);
        //output.response_flag = 0;
        output.response_flag = 0;
        atci_send_response(&output);
        atci_cmd_hdlr_bt_testbox_WhetherFoundOneDevice = true;
        break;
    case ATCI_HCE_IO_CAPABILITY_REQUEST:
        //No status/hci handle
        break;
    case ATCI_HCE_IO_CAPABILITY_RESPONSE:
        //No status/hci handle
        break;
    case ATCI_HCE_USER_CONFIRM_REQUSEST:
        //No status/hci handle
        break;
    case ATCI_HCE_USER_PASSKEY_REQUEST_EVENT:
        //No status/hci handle
        break;
    case ATCI_HCE_REMOTE_OOB_DATA_REQUEST_EVENT:
        //No status/hci handle
        break;
    case ATCI_HCE_USER_PASSKEY_NOTIFICATION:
        //No status/hci handle
        break;
    case ATCI_HCE_USER_KEYPRESS:
        //No status/hci handle
        break;
    case ATCI_HCE_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT:
        //No status/hci handle
        break;
    case ATCI_HCE_BLUETOOTH_LOGO:
        //may not used
        break;
    case ATCI_HCE_VENDOR_SPECIFIC:
        //may not used
        break;
    default:
        //Not supported event
        break;
    }

    return 0;
}

static void atci_cmd_hdlr_bt_testbox_TaskMsgHandler(atci_cmd_hdlr_bt_testbox_message_struct_t *message1)
{
    //uint8_t result = 0;
    atci_bt_hci_event hci_event;
    //atci_response_t output = {{0}};
    //uint8_t string[526+100] = {0};
    uint8_t *temp_str;
    atci_cmd_hdlr_bt_testbox_message_struct_t message_temp=*message1;
    atci_cmd_hdlr_bt_testbox_message_struct_t *message=&message_temp;
    uint8_t temp_len=message->param1;

    LOG_I(atci_bt, "msglen :0x%x", message->param1);
    temp_str = message->param2;
    if(message->param1>32) {
        temp_len=32;
    }
    for (int i = 0 ; i < temp_len; i++) {
        LOG_I(atci_bt, "msg_handler--msg:%x", *(temp_str + i));
    }
    //hci_log_event(message->param2, message->param1);
    atci_cmd_hdlr_bt_testbox_HciEventParser((uint8_t *)message->param2, message->param1, &hci_event);


}


static void atci_cmd_hdlr_bt_testbox_TaskMain(void *pvParameters)
{

    atci_cmd_hdlr_bt_testbox_message_struct_t queue_item;
    LOG_I(atci_bt, "enter task main.");

    atci_cmd_hdlr_bt_testbox_TaskQueue = xQueueCreate(atci_cmd_hdlr_bt_QUEUE_SIZE, sizeof(atci_cmd_hdlr_bt_testbox_message_struct_t));
    while (1) {
        if (xQueueReceive(atci_cmd_hdlr_bt_testbox_TaskQueue, &queue_item, portMAX_DELAY)) {
            LOG_I(atci_bt, "enter massge handler.");
            atci_cmd_hdlr_bt_testbox_TaskMsgHandler(&queue_item);
        }
    }

}

static void atci_cmd_hdlr_bt_testbox_TaskInit(void)
{
    LOG_I(atci_bt, "");

    xTaskCreate(atci_cmd_hdlr_bt_testbox_TaskMain,
                atci_cmd_hdlr_bt_TASK_NAME,
                atci_cmd_hdlr_bt_TASK_STACK_SIZE,
                NULL,
                atci_cmd_hdlr_bt_TASK_PRIORITY, &atci_cmd_hdlr_bt_testbox_TaskHdlr);
}

static void atci_cmd_hdlr_bt_testbox_TaskDeinit(void)
{
    vTaskDelete(atci_cmd_hdlr_bt_testbox_TaskHdlr);
}

#if 1
int atci_cmd_hdlr_bt_testbox_doing(atci_cmd_hdlr_bt_testbox_type type,char* req);

static int atci_cmd_hdlr_bt_testbox_HciCmd_SetTxPower(int power) {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_SetTxPower;//reset BT
    hci_cmd.cmd_len=6;
    hci_cmd.cmd[0]=0x07;//initial bt tx power
    hci_cmd.cmd[1]=0x80;//unused
    hci_cmd.cmd[2]=0x00;//unused
    hci_cmd.cmd[3]=0x06;//unused
    hci_cmd.cmd[4]=0x05;//ble tx power
    hci_cmd.cmd[5]=power;//bt max tx power
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}

static int atci_cmd_hdlr_bt_testbox_HciCmd_MearsureRssi() {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_MeasureRSSI;
    hci_cmd.cmd_len=5;
    hci_cmd.cmd[0]=0x33;
    hci_cmd.cmd[1]=0x8B;
    hci_cmd.cmd[2]=0x9E;
    hci_cmd.cmd[3]=0x04;//time = 04*1.28s=5.12s
    hci_cmd.cmd[4]=0x20;//count=32
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}
static int atci_cmd_hdlr_bt_testbox_HciCmd_GetAddrFromController() {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_GetBtAddr;
    hci_cmd.cmd_len=0x00;
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}
static int atci_cmd_hdlr_bt_testbox_HciCmd_SetAddrToController() {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_SetBtAddr;
    hci_cmd.cmd_len=0x06;
    hci_cmd.cmd[0]=atci_cmd_hdlr_bt_testbox_LocalAddr[5];
    hci_cmd.cmd[1]=atci_cmd_hdlr_bt_testbox_LocalAddr[4];
    hci_cmd.cmd[2]=atci_cmd_hdlr_bt_testbox_LocalAddr[3];
    hci_cmd.cmd[3]=atci_cmd_hdlr_bt_testbox_LocalAddr[2];
    hci_cmd.cmd[4]=atci_cmd_hdlr_bt_testbox_LocalAddr[1];
    hci_cmd.cmd[5]=atci_cmd_hdlr_bt_testbox_LocalAddr[0];
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);


    return result;
}

static int atci_cmd_hdlr_bt_testbox_HciCmd_GetNameFromController() {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_GetBtName;
    hci_cmd.cmd_len=0x00;
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}
static int atci_cmd_hdlr_bt_testbox_HciCmd_SetNameToController() {
    int result=0;
    atci_bt_hci_cmd hci_cmd= {0};

    hci_cmd.op_code=TextboxType_hcicmd_SetBtName;
    hci_cmd.cmd_len=248;
    strcpy((char *)(&(hci_cmd.cmd)), atci_cmd_hdlr_bt_testbox_LocalName);
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}


static int atci_cmd_hdlr_bt_testbox_HciCmd_ResetBt() {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_ResetBT;//reset BT
    hci_cmd.cmd_len=0;
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);


    return result;
}

static int atci_cmd_hdlr_bt_testbox_HciCmd_TxTone(int channel) { //channel: 0~79
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_TxTone;
    hci_cmd.cmd_len=5;
    hci_cmd.cmd[0]=channel;//channel: 0~79
    hci_cmd.cmd[1]=0;//tone type : 0 is DC,1 is 250K, 2 is 400K, 3 is 1M, 4 is Modulation
    hci_cmd.cmd[2]=0;//when tone type =4, this meaning  0: BT,  1 is BLE
    hci_cmd.cmd[3]=0;//when tone type=4, this meaning 0: 1M,  1 is 2M,  2 is 3M
    hci_cmd.cmd[4]=0;//when tone type=4, this meaning 0: PRBS9, 1 is PRBS15
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);


    return result;
}


static int atci_cmd_hdlr_bt_testbox_HciCmd_SetInquiryMode() {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_SetInquiryMode;//set bt inquiry mode
    hci_cmd.cmd_len=1;
    hci_cmd.cmd[0]=0x02;
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}

static int atci_cmd_hdlr_bt_testbox_HciCmd_SetDiscoverableConnectable() {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_SetDiscoverable;//set scan mode=discoverable only
    hci_cmd.cmd_len=1;
    hci_cmd.cmd[0]=0x03;// connectable + discoverable
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}

static int atci_cmd_hdlr_bt_testbox_HciCmd_SetWhiten() {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_SetWhiten;//set scan mode=discoverable only
    hci_cmd.cmd_len=1;
    hci_cmd.cmd[0]=0x00;
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}

static int atci_cmd_hdlr_bt_testbox_HciCmd_SetTestCommandTxStart() {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_TestCommand;//set scan mode=discoverable only
    hci_cmd.cmd_len=0x17;
    hci_cmd.cmd[0]=0x00;
    hci_cmd.cmd[1]=0x00;
    hci_cmd.cmd[2]=0x03;
    hci_cmd.cmd[3]=0x00;
    hci_cmd.cmd[4]=0x00;
    hci_cmd.cmd[5]=0x00;
    hci_cmd.cmd[6]=0x00;
    hci_cmd.cmd[7]=0x02;
    hci_cmd.cmd[8]=0x0A;
    hci_cmd.cmd[9]=0x40;
    hci_cmd.cmd[10]=0x00;
    hci_cmd.cmd[11]=0x02;
    hci_cmd.cmd[12]=0x00;
    hci_cmd.cmd[13]=0x01;
    hci_cmd.cmd[14]=0x00;
    hci_cmd.cmd[15]=0x00;
    hci_cmd.cmd[16]=0x00;
    hci_cmd.cmd[17]=0x00;
    hci_cmd.cmd[18]=0x00;
    hci_cmd.cmd[19]=0x00;
    hci_cmd.cmd[20]=0x00;
    hci_cmd.cmd[21]=0x00;
    hci_cmd.cmd[22]=0x00;
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}

static int atci_cmd_hdlr_bt_testbox_HciCmd_SetTestCommandTxStop() {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_TestCommandTxStop;//set scan mode=discoverable only
    hci_cmd.cmd_len=0x00;
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}


static int atci_cmd_hdlr_bt_testbox_HciCmd_SetTxPowerLevel(uint8_t power) {
    int result=0;
    atci_bt_hci_cmd hci_cmd;

    hci_cmd.op_code=TextboxType_hcicmd_SetTxPowerLevel;//set scan mode=discoverable only
    hci_cmd.cmd_len=0x4;
    hci_cmd.cmd[0]=0x00;
    hci_cmd.cmd[1]=0x00;
    hci_cmd.cmd[2]=power;
    hci_cmd.cmd[3]=0x00;
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}

static int atci_cmd_hdlr_bt_testbox_HciCmd_SetTestCommandRxStart(char* req) {
    int result=0;
    atci_bt_hci_cmd hci_cmd;
    bt_bd_addr_t addr;
    int i;
    char tmp_buf[3]= {0};

    for (i = 0; i < 6; ++i) {
        tmp_buf[0] = req[2 * i];
        tmp_buf[1] = req[2 * i + 1];
        addr[i] = (uint8_t)strtoul((char *)tmp_buf, NULL, 16);
    }

    hci_cmd.op_code=TextboxType_hcicmd_TestCommand;//set scan mode=discoverable only
    hci_cmd.cmd_len=0x17;
    hci_cmd.cmd[0]=0x00;
    hci_cmd.cmd[1]=0x03;
    hci_cmd.cmd[2]=0x0B;
    hci_cmd.cmd[3]=0x00;
    hci_cmd.cmd[4]=0x00;
    hci_cmd.cmd[5]=0x00;
    hci_cmd.cmd[6]=0x00;
    hci_cmd.cmd[7]=0x01;
    hci_cmd.cmd[8]=0x0A;
    hci_cmd.cmd[9]=0x00;
    hci_cmd.cmd[10]=0x00;
    hci_cmd.cmd[11]=0x02;
    hci_cmd.cmd[12]=0x00;
    hci_cmd.cmd[13]=0x01;
    hci_cmd.cmd[14]=0x00;
    hci_cmd.cmd[15]=addr[0]&0x00;
    hci_cmd.cmd[16]=addr[1]&0x00;
    hci_cmd.cmd[17]=addr[2];
    hci_cmd.cmd[18]=addr[3];
    hci_cmd.cmd[19]=addr[4];
    hci_cmd.cmd[20]=addr[5];
    hci_cmd.cmd[21]=0x00;
    hci_cmd.cmd[22]=0x00;
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}

static int atci_cmd_hdlr_bt_testbox_HciCmd_SetTestCommandRxStop(char* req) {
    int result=0;
    atci_bt_hci_cmd hci_cmd;
    bt_bd_addr_t addr;
    int i;
    char tmp_buf[3]= {0};
    if (req == NULL) {
        return result;
	}
	
    for (i = 0; i < 6; ++i) {
        tmp_buf[0] = req[2 * i];
        tmp_buf[1] = req[2 * i + 1];
        addr[i] = (uint8_t)strtoul((char *)tmp_buf, NULL, 16);
    }

    hci_cmd.op_code=TextboxType_hcicmd_TestCommand;//set scan mode=discoverable only
    hci_cmd.cmd_len=0x17;
    hci_cmd.cmd[0]=0x00;
    hci_cmd.cmd[1]=0x03;
    hci_cmd.cmd[2]=0xFF;
    hci_cmd.cmd[3]=0x00;
    hci_cmd.cmd[4]=0x00;
    hci_cmd.cmd[5]=0x00;
    hci_cmd.cmd[6]=0x00;
    hci_cmd.cmd[7]=0x01;
    hci_cmd.cmd[8]=0x0A;
    hci_cmd.cmd[9]=0x00;
    hci_cmd.cmd[10]=0x00;
    hci_cmd.cmd[11]=0x02;
    hci_cmd.cmd[12]=0x00;
    hci_cmd.cmd[13]=0x01;
    hci_cmd.cmd[14]=0x00;
    hci_cmd.cmd[15]=addr[0]&0x00;
    hci_cmd.cmd[16]=addr[1]&0x00;
    hci_cmd.cmd[17]=addr[2];
    hci_cmd.cmd[18]=addr[3];
    hci_cmd.cmd[19]=addr[4];
    hci_cmd.cmd[20]=addr[5];
    hci_cmd.cmd[21]=0x00;
    hci_cmd.cmd[22]=0x00;
    bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);

    return result;
}


static int atci_cmd_hdlr_bt_testbox_SetAddrToNvdm(char* req) {
    //int result=0;
    //atci_bt_hci_cmd hci_cmd;
    atci_response_t output = {{0}};
    nvdm_status_t status = NVDM_STATUS_OK;
    bt_bd_addr_t addr;
    int8_t i;
    //uint8_t buffer[18] = {0};
    uint32_t size = 12;
    uint8_t tmp_buf[3] = {0};

    if(0 != atci_cmd_hdlr_bt_testbox_CheckBtaddrValid(req)) {
        LOG_I(atci_bt, "[BT]invalid address.\n");
        output.response_len = 0;
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    } else {
        status = nvdm_write_data_item("BT", "address", NVDM_DATA_ITEM_TYPE_STRING, (const uint8_t*)req, size);
        for (i = 0; i < 6; ++i) {
            tmp_buf[0] = req[2 * i];
            tmp_buf[1] = req[2 * i + 1];
            addr[i] = (uint8_t)strtoul((char *)tmp_buf, NULL, 16);
        }
        if (NVDM_STATUS_OK != status) {
            LOG_I(atci_bt, "[BT]Failed to store address.\n");
            output.response_len = 0;
            output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

        } else {
            LOG_I(atci_bt, "[BT]Successfully store address to NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\n", addr[0],
                  addr[1], addr[2], addr[3], addr[4], addr[5]);
            memcpy(atci_cmd_hdlr_bt_testbox_LocalAddr,addr,sizeof(addr));

            sprintf((char *)output.response_buf, "+EBTTB:%d,%d\r\n",atci_cmd_hdlr_bt_testbox_GetTextboxType(),status);
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        }

    }

    atci_send_response(&output);
    return status;
}

static int atci_cmd_hdlr_bt_testbox_GetAddrFromNvdm() {
    //int result=0;
    //atci_bt_hci_cmd hci_cmd;
    atci_response_t output = {{0}};
    nvdm_status_t status;
    bt_bd_addr_t addr;
    int8_t i;
    uint8_t buffer[18] = {0};
    uint32_t size = 12;
    uint8_t tmp_buf[3] = {0};


    status = nvdm_read_data_item("BT", "address", buffer, &size);
    if (NVDM_STATUS_OK == status) {
        LOG_I(atci_bt, "[BT]Read from NVDM:%s\n", buffer);
        for (i = 0; i < 6; ++i) {
            tmp_buf[0] = buffer[2 * i];
            tmp_buf[1] = buffer[2 * i + 1];
            addr[i] = (uint8_t)strtoul((char *)tmp_buf, NULL, 16);
        }

        LOG_I(atci_bt, "[BT]Read address from NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\n", addr[0],
              addr[1], addr[2], addr[3], addr[4], addr[5]);

        memcpy(atci_cmd_hdlr_bt_testbox_LocalAddr,addr,sizeof(addr));

        sprintf((char *)output.response_buf, "+EBTTB:%d,%s\r\n",
                atci_cmd_hdlr_bt_testbox_GetTextboxType(),buffer);
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    } else {
        LOG_I(atci_bt, "[BT]Failed to Read from NVDM:%d.\n", status);
        output.response_len = 0;
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    }

    atci_send_response(&output);
    return status;
}

static int atci_cmd_hdlr_bt_testbox_SetNameToNvdm(char* req) {
    //int result=0;
    //atci_bt_hci_cmd hci_cmd;
    atci_response_t output = {{0}};
    nvdm_status_t status;
    //int8_t i;
    //uint8_t buffer[18] = {0};
    //uint32_t size = sizeof(buffer);
    //uint8_t tmp_buf[3] = {0};


    status = nvdm_write_data_item("BT", "name", NVDM_DATA_ITEM_TYPE_STRING, (const uint8_t *)req, strlen((char *)req));

    if (NVDM_STATUS_OK != status) {
        LOG_I(atci_bt, "[BT]Failed to store [len=%d,name=%s]\n",strlen((char *)req),req);
        output.response_len = 0;
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    } else {
        LOG_I(atci_bt, "[BT]Successfully store name to NVDM [len=%d,name=%s]\n", strlen((char *)req),req);
        strcpy(atci_cmd_hdlr_bt_testbox_LocalName,req);

        sprintf((char *)output.response_buf, "+EBTTB:%d,%d\r\n",atci_cmd_hdlr_bt_testbox_GetTextboxType(),status);
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    }

    atci_send_response(&output);

    return status;
}

static int atci_cmd_hdlr_bt_testbox_GetNameFromNvdm() {
    //int result=0;
    //atci_bt_hci_cmd hci_cmd;
    atci_response_t output = {{0}};
    nvdm_status_t status;
    //int8_t i;
    uint8_t buffer[18] = {0};
    uint32_t size = sizeof(buffer);
    //uint8_t tmp_buf[3] = {0};


    status = nvdm_read_data_item("BT", "name", buffer, &size);
    if (NVDM_STATUS_OK == status) {
        LOG_I(atci_bt, "[BT]Read from NVDM:%s\n", buffer);

        LOG_I(atci_bt, "[BT]Read name from NVDM [len=%d,name=%s]\n", strlen((char *)buffer),buffer);
        memcpy(atci_cmd_hdlr_bt_testbox_LocalName,buffer,size);

        sprintf((char *)output.response_buf, "+EBTTB:%d,%s\r\n",
                atci_cmd_hdlr_bt_testbox_GetTextboxType(),atci_cmd_hdlr_bt_testbox_LocalName);
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    } else {
        LOG_I(atci_bt, "[BT]Failed to Read from NVDM:%d.\n", status);
        output.response_len = 0;
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    }

    atci_send_response(&output);

    return status;
}

void atci_cmd_hdlr_bt_testbox_SetBtPowerOnFlag(bool flag) {
    atci_cmd_hdlr_bt_testbox_NormalBtPowerOn = flag;
}

static int atci_cmd_hdlr_bt_testbox_EnterBT() {
    int result=0;
    //atci_bt_hci_cmd hci_cmd;
    atci_response_t output = {{0}};
    //nvdm_status_t status;
    //bt_bd_addr_t addr;
    //int8_t i;
    //uint8_t buffer[18] = {0};
    //uint32_t size = sizeof(buffer);
    //uint8_t tmp_buf[3] = {0};


    if (false == atci_cmd_hdlr_bt_testbox_PowerStatus) {
        if(true == atci_cmd_hdlr_bt_testbox_NormalBtPowerOn) {
            LOG_I(atci_bt, "BT has powered on, need to power off");
            bt_power_off();
            while(true == atci_cmd_hdlr_bt_testbox_NormalBtPowerOn) {
                vTaskDelay(50);
            };
        }
        result = bt_driver_power_on();
        if (result == BT_STATUS_SUCCESS) {
            atci_cmd_hdlr_bt_testbox_PowerStatus = true;
            bt_driver_register_atci_rx_callback((void*)atci_cmd_hdlr_bt_testbox_RxCallback);
            atci_cmd_hdlr_bt_testbox_TaskInit();
        }
    } else {
        result = BT_STATUS_SUCCESS;
    }

    if(BT_STATUS_SUCCESS == result) {
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    } else {
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    }
    output.response_len = 0;
    atci_send_response(&output);

    return result;
}

static int atci_cmd_hdlr_bt_testbox_ExitBT() {
    int result=0;
    //atci_bt_hci_cmd hci_cmd;
    atci_response_t output = {{0}};
    //nvdm_status_t status;
    //bt_bd_addr_t addr;
    //int8_t i;
    //uint8_t buffer[18] = {0};
    //uint32_t size = sizeof(buffer);
    //uint8_t tmp_buf[3] = {0};

    if (true == atci_cmd_hdlr_bt_testbox_PowerStatus) {
        result = bt_driver_power_off();
        bt_driver_deinit_atci_mode();
        atci_cmd_hdlr_bt_testbox_TaskDeinit();
        atci_cmd_hdlr_bt_testbox_PowerStatus = false;
    } else {
        result = BT_STATUS_SUCCESS;
    }

    if(BT_STATUS_SUCCESS == result) {
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    } else {
        output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    }
    output.response_len = 0;
    atci_send_response(&output);
    return result;
}

static int atci_cmd_hdlr_bt_testbox_GetBitErrorRate() {
    int result=0;
    //atci_bt_hci_cmd hci_cmd;
    atci_response_t output = {{0}};
    //nvdm_status_t status;
    //bt_bd_addr_t addr;
    //int8_t i;
    //uint8_t buffer[18] = {0};
    //uint32_t size = sizeof(buffer);
    //uint8_t tmp_buf[3] = {0};

    sprintf((char *)output.response_buf, "+EBTTB:%d,%d\r\n",atci_cmd_hdlr_bt_testbox_GetTextboxType(),100);
    output.response_len = strlen((char *)output.response_buf);
    output.response_flag = 0;
    atci_send_response(&output);

    return result;
}



#endif

#if 1
#define atci_cmd_hdlr_bt_biterrorrate_duration (1000*5)

bool atci_cmd_hdlr_bt_testbox_CheckHciCommandFinish() {
    while(false == atci_cmd_hdr_bt_testbox_HciCommandFinish) {
        vTaskDelay(1);
    }
    return atci_cmd_hdr_bt_testbox_HciCommandFinish;
}

void atci_cmd_hdlr_bt_testbox_Method_SetBtAddr(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_SetBtAddr);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_SetBtAddressToNvdm,req);

}

void atci_cmd_hdlr_bt_testbox_Method_GetBtAddr(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_GetBtAddr);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_GetBtAddressFromNvdm,NULL);

}


void atci_cmd_hdlr_bt_testbox_Method_SetBtName(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_SetBtName);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_SetBtNameToNvdm,req);

}

void atci_cmd_hdlr_bt_testbox_Method_GetBtName(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_GetBtName);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_GetBtNameFromNvdm,NULL);

}

void atci_cmd_hdlr_bt_testbox_Method_TxToneStart(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_doing(TextboxType_ResetBT,NULL);
    atci_cmd_hdlr_bt_testbox_CheckHciCommandFinish();
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_TxToneStart);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_TxTone,req);

}
void atci_cmd_hdlr_bt_testbox_Method_TxToneStop(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_TxToneStop);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_ResetBT,NULL);

}


void atci_cmd_hdlr_bt_testbox_Method_PowerOn(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_PowerOnBt);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_Enter_BT,NULL);

}

void atci_cmd_hdlr_bt_testbox_Method_PowerOff(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_PowerOffBt);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_Exit_BT,NULL);

}

void atci_cmd_hdlr_bt_testbox_Method_InitialBt(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_doing(TextboxType_ResetBT,NULL);
    atci_cmd_hdlr_bt_testbox_CheckHciCommandFinish();

    atci_cmd_hdlr_bt_testbox_doing(TextboxType_SetLocalBtAddressToController,NULL);
    atci_cmd_hdlr_bt_testbox_CheckHciCommandFinish();

    atci_cmd_hdlr_bt_testbox_doing(TextboxType_SetLocalBtNameToController,NULL);
    atci_cmd_hdlr_bt_testbox_CheckHciCommandFinish();

    atci_cmd_hdlr_bt_testbox_doing(TextboxType_SetInquiryMode,NULL);
    atci_cmd_hdlr_bt_testbox_CheckHciCommandFinish();

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_InitialBt);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_SetDiscoverableConnectable,NULL);
}

void atci_cmd_hdlr_bt_testbox_Method_RssiDutAsTx(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_DUTTX);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_SetTxPower,req);

}

void atci_cmd_hdlr_bt_testbox_Method_RssiTextBoxAsRx(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_TBRX);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_MeasureRSSI,NULL);
}

void atci_cmd_hdlr_bt_testbox_Method_BitErrorRateDutAsRx(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_doing(TextboxType_TestCommandRxStart,req);
    vTaskDelay(atci_cmd_hdlr_bt_biterrorrate_duration);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_DUTRX);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_TestCommandRxStop,req);
}

void atci_cmd_hdlr_bt_testbox_Method_BitErrorRateTestBoxAsTxStart(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_doing(TextboxType_SetWhiten,NULL);
    atci_cmd_hdlr_bt_testbox_CheckHciCommandFinish();

    atci_cmd_hdlr_bt_testbox_doing(TextboxType_TestCommandTxStart,NULL);
    atci_cmd_hdlr_bt_testbox_CheckHciCommandFinish();

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_TBTX_Start);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_SetPowerLevel,req);
}

void atci_cmd_hdlr_bt_testbox_Method_BitErrorRateTestBoxAsTxStop(char *req)
{
    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Unknown);

    atci_cmd_hdlr_bt_testbox_SetTextboxType(TextboxType_Methodd_TBTX_Stop);
    atci_cmd_hdlr_bt_testbox_doing(TextboxType_TestCommandTxStop,NULL);
}


#endif

int atci_cmd_hdlr_bt_testbox_doing(atci_cmd_hdlr_bt_testbox_type type,char* req) {
    int result=0;
    //atci_bt_hci_cmd hci_cmd;
    //atci_response_t output = {{0}};
    //bool register_result=false;
    //nvdm_status_t status;
    //bt_bd_addr_t addr;
    //int8_t i;
    //uint32_t random_seed;
    //uint8_t buffer[18] = {0};
    //uint32_t size = sizeof(buffer);
    //uint8_t tmp_buf[3] = {0};
    //static int once_flag=0;

    if(NULL != req) {
        LOG_I(atci_bt, "type:%d, req:%s", type, req);
    } else {
        LOG_I(atci_bt, "type:%d, req:NULL", type);
    }
    atci_cmd_hdr_bt_testbox_HciCommandFinish = false;

    switch(type) {
    case TextboxType_Exit_BT://AT+EBTTB=1
        atci_cmd_hdlr_bt_testbox_ExitBT();
        break;
    case TextboxType_Enter_BT://AT+EBTTB=2
        atci_cmd_hdlr_bt_testbox_EnterBT();
        break;
    case TextboxType_GetBitErrorRate://AT+EBTTB=3
        atci_cmd_hdlr_bt_testbox_GetBitErrorRate();
        break;
    case TextboxType_SetBtAddressToNvdm://AT+EBTTB=4,001122334455
        atci_cmd_hdlr_bt_testbox_SetAddrToNvdm(req);
        break;
    case TextboxType_GetBtAddressFromNvdm://AT+EBTTB=5
        atci_cmd_hdlr_bt_testbox_GetAddrFromNvdm();
        break;
    case TextboxType_SetBtNameToNvdm://AT+EBTTB=6,xxxx
        atci_cmd_hdlr_bt_testbox_SetNameToNvdm(req);
        break;
    case TextboxType_GetBtNameFromNvdm://AT+EBTTB=7
        atci_cmd_hdlr_bt_testbox_GetNameFromNvdm();
        break;

    case TextboxType_SetTxPower://AT+EBTTB=100,xx
        atci_cmd_hdlr_bt_testbox_HciCmd_SetTxPower(atoi(req));
        break;
    case TextboxType_MeasureRSSI://AT+EBTTB=101
        atci_cmd_hdlr_bt_testbox_HciCmd_MearsureRssi();
        break;
    case TextboxType_GetLocalBtAddressFromController://AT+EBTTB=102
        atci_cmd_hdlr_bt_testbox_HciCmd_GetAddrFromController();
        break;
    case TextboxType_SetLocalBtAddressToController://AT+EBTTB=103
        atci_cmd_hdlr_bt_testbox_HciCmd_SetAddrToController();
        break;
    case TextboxType_ResetBT://AT+EBTTB=104
        atci_cmd_hdlr_bt_testbox_HciCmd_ResetBt();
        break;
    case TextboxType_SetInquiryMode://AT+EBTTB=105
        atci_cmd_hdlr_bt_testbox_HciCmd_SetInquiryMode();
        break;
    case TextboxType_SetDiscoverableConnectable://AT+EBTTB=106
        atci_cmd_hdlr_bt_testbox_HciCmd_SetDiscoverableConnectable();
        break;
    case TextboxType_SetWhiten://AT+EBTTB=107
        atci_cmd_hdlr_bt_testbox_HciCmd_SetWhiten();
        break;
    case TextboxType_SetPowerLevel://AT+EBTTB=108,XX
        atci_cmd_hdlr_bt_testbox_HciCmd_SetTxPowerLevel(atoi(req));
        break;
    case TextboxType_TestCommandRxStart://AT+EBTTB=109,xxx
        atci_cmd_hdlr_bt_testbox_HciCmd_SetTestCommandRxStart(req);
        break;
    case TextboxType_TestCommandRxStop://AT+EBTTB=110,xxx
        atci_cmd_hdlr_bt_testbox_HciCmd_SetTestCommandRxStop(req);
        break;
    case TextboxType_TestCommandTxStart://AT+EBTTB=111
        atci_cmd_hdlr_bt_testbox_HciCmd_SetTestCommandTxStart();
        break;
    case TextboxType_TestCommandTxStop://AT+EBTTB=112
        atci_cmd_hdlr_bt_testbox_HciCmd_SetTestCommandTxStop();
        break;
    case TextboxType_GetLocalBtNameFromController://AT+EBTTB=113
        atci_cmd_hdlr_bt_testbox_HciCmd_GetNameFromController();
        break;
    case TextboxType_SetLocalBtNameToController://AT+EBTTB=114
        atci_cmd_hdlr_bt_testbox_HciCmd_SetNameToController();
        break;
    case TextboxType_TxTone://AT+EBTTB=115,xx
        atci_cmd_hdlr_bt_testbox_HciCmd_TxTone(atoi(req));
        break;

    case TextboxType_Methodd_SetBtAddr://AT+EBTTB=1000,XXXX
        atci_cmd_hdlr_bt_testbox_Method_SetBtAddr(req);
        break;
    case TextboxType_Methodd_GetBtAddr://AT+EBTTB=1001
        atci_cmd_hdlr_bt_testbox_Method_GetBtAddr(req);
        break;
    case TextboxType_Methodd_PowerOnBt://AT+EBTTB=1002
        atci_cmd_hdlr_bt_testbox_Method_PowerOn(req);
        break;
    case TextboxType_Methodd_PowerOffBt://AT+EBTTB=1003
        atci_cmd_hdlr_bt_testbox_Method_PowerOff(req);
        break;
    case TextboxType_Methodd_InitialBt://AT+EBTTB=1004
        atci_cmd_hdlr_bt_testbox_Method_InitialBt(req);
        break;
    case TextboxType_Methodd_DUTTX://AT+EBTTB=1005,XXX
        atci_cmd_hdlr_bt_testbox_Method_RssiDutAsTx(req);
        break;
    case TextboxType_Methodd_DUTRX://AT+EBTTB=1006,XXXX
        atci_cmd_hdlr_bt_testbox_Method_BitErrorRateDutAsRx(req);
        break;
    case TextboxType_Methodd_TBTX_Start://AT+EBTTB=1007,XX
        atci_cmd_hdlr_bt_testbox_Method_BitErrorRateTestBoxAsTxStart(req);
        break;
    case TextboxType_Methodd_TBTX_Stop://AT+EBTTB=1008
        atci_cmd_hdlr_bt_testbox_Method_BitErrorRateTestBoxAsTxStop(req);
        break;
    case TextboxType_Methodd_TBRX://AT+EBTTB=1009
        atci_cmd_hdlr_bt_testbox_Method_RssiTextBoxAsRx(req);
        break;
    case TextboxType_Methodd_SetBtName://AT+EBTTB=1010,XXXX
        atci_cmd_hdlr_bt_testbox_Method_SetBtName(req);
        break;
    case TextboxType_Methodd_GetBtName://AT+EBTTB=1011
        atci_cmd_hdlr_bt_testbox_Method_GetBtName(req);
        break;
    case TextboxType_Methodd_TxToneStart://AT+EBTTB=1012,xx
        atci_cmd_hdlr_bt_testbox_Method_TxToneStart(req);
        break;
    case TextboxType_Methodd_TxToneStop://AT+EBTTB=1013
        atci_cmd_hdlr_bt_testbox_Method_TxToneStop(req);
        break;
    default:
        break;
    }


    return result;
}




atci_status_t atci_cmd_hdlr_bt_testbox(atci_parse_cmd_param_t *parse_cmd)
{
    //int8_t result;
    //atci_bt_hci_cmd hci_cmd;
    char *parse_pos = NULL;
    atci_response_t output = {{0}};


    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);



    switch (parse_cmd->mode) {
    case ATCI_CMD_MODE_EXECUTION: { //AT+EBTTB=<type,req>
        LOG_I(atci_bt, "cmd_len:%d, name_len:%d, string_len:%d", strlen(parse_cmd->string_ptr), parse_cmd->name_len, parse_cmd->string_len);

        //int tb_req=0;
        //int result=0;
        parse_pos = strchr(parse_cmd->string_ptr, '=');
        parse_pos++;
        atci_cmd_hdlr_bt_testbox_SetTextboxType((atci_cmd_hdlr_bt_testbox_type)atoi(parse_pos));
        parse_pos = strchr(parse_pos, ',');
        if(NULL == parse_pos) {

        } else {
            parse_pos++;
        }
        atci_cmd_hdlr_bt_testbox_doing(atci_cmd_hdlr_bt_testbox_GetTextboxType(),parse_pos);

    }

    break;

    case ATCI_CMD_MODE_TESTING:
    case ATCI_CMD_MODE_READ:
    case ATCI_CMD_MODE_ACTIVE:
    default: {
        strcpy((char *)output.response_buf, "Not Support\n");
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(&output);
    }
    break;
    }

    return ATCI_STATUS_OK;
}



atci_status_t atci_cmd_hdlr_bt_testbox_btaddr(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    //char *saveptr = NULL;
    //char *bt_address_str = NULL;
    nvdm_status_t status;
    bt_bd_addr_t addr;
    int8_t i;
    //uint32_t random_seed;
    //uint8_t buffer[18] = {0};
    //uint32_t size = sizeof(buffer);
    uint8_t tmp_buf[3] = {0};
    //static int once_flag=0;

    LOG_I(atci_bt, "[BT_ATCI] mode:%d, CMD:%s", parse_cmd->mode, parse_cmd->string_ptr);

    if (parse_cmd->mode == ATCI_CMD_MODE_EXECUTION) {/*AT+EBTADDR=<op>*/
        if (strstr((char *)parse_cmd->string_ptr, "AT+EBTADDR=0") != NULL) {
            /* read data item from NVDM */
            uint8_t buffer[13] = {0};
            uint32_t size = 12;
            status = nvdm_read_data_item("BT", "address",  buffer, &size);
            if (NVDM_STATUS_OK != status) {
                LOG_I(atci_bt, "nvdm_read_data_item status = %d", status);
                output.response_flag = 0 |ATCI_RESPONSE_FLAG_APPEND_ERROR;
                goto END;
            }
            snprintf ((char *)output.response_buf, sizeof(output.response_buf), "+EBTADDR:%s\r\n", (char *)buffer);
            LOG_I(atci_bt, "response string:%s", output.response_buf);
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 |ATCI_RESPONSE_FLAG_APPEND_OK;
        } else if (strstr((char *)parse_cmd->string_ptr, "AT+EBTADDR=1") != NULL) {
            /* write data item into NVDM */
            //char* input_string = parse_cmd->string_ptr;
            //uint8_t buffer[13] = {0};
            uint32_t size = 12;
            /* get bt address*/
            if (parse_cmd->string_ptr[12]  !=  ',') {
                LOG_I(atci_bt, "input is not valid: %c", parse_cmd->string_ptr[11]);
                output.response_flag = 0 |ATCI_RESPONSE_FLAG_APPEND_ERROR;
                goto END;
            }
            char* bt_address_str = parse_cmd->string_ptr + 13;
            /*check that input is valid or not*/
            LOG_I(atci_bt, "[BT]address:%s", bt_address_str);
            /*check that bt address is valid or not */
            uint8_t is_invalid = atci_cmd_hdlr_bt_testbox_CheckBtaddrValid(bt_address_str);
            if (is_invalid != 0) {
                LOG_I(atci_bt, "bt address is invalid: %d", is_invalid);
                output.response_flag = 0 |ATCI_RESPONSE_FLAG_APPEND_ERROR;
                goto END;
            }
            status = nvdm_write_data_item("BT", "address", NVDM_DATA_ITEM_TYPE_STRING, (const uint8_t*)bt_address_str, size);
            if (NVDM_STATUS_OK != status) {
                LOG_I(atci_bt, "[BT]Failed to store address:%d", status);
                output.response_flag = 0 |ATCI_RESPONSE_FLAG_APPEND_ERROR;
                goto END;
            }
            output.response_flag = 0 |ATCI_RESPONSE_FLAG_APPEND_OK;

            for (i = 0; i < 6; ++i) {
                tmp_buf[0] = bt_address_str[2 * i];
                tmp_buf[1] = bt_address_str[2 * i + 1];
                addr[i] = (uint8_t)strtoul((char *)tmp_buf, NULL, 16);
            }
            memcpy(atci_cmd_hdlr_bt_testbox_LocalAddr,addr,sizeof(addr));



            goto END;
        } else {
            output.response_flag = 0 |ATCI_RESPONSE_FLAG_APPEND_ERROR;
            goto END;
        }
    } else {
        strcpy((char *)output.response_buf, "Not Support\n");
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        goto END;
    }

END:
    atci_send_response(&output);
    return ATCI_STATUS_OK;
}





#endif

#endif
/*---ATA Command handler Function End---*/




