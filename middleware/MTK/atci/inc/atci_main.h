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

#ifndef ATCI_TASK_MAIN_H
#define ATCI_TASK_MAIN_H

#include "hal_uart.h"
#include "atci.h"

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
#include "serial_port.h"
#endif

// ATCI config setting
// Queue Define
#define ATCI_QUEUE_ITEM_SIZE           (1)
#define ATCI_LOCAL_QUEUE_LENGTH        (30)

// General Table
#define ATCI_MAX_GNENERAL_TABLE_NUM    (20)

#define ATCI_HASH_TABLE_ROW            (37)
#define ATCI_HASH_TABLE_SPAN           (5)
#define ATCI_MAX_CMD_NAME_LEN          (2*ATCI_HASH_TABLE_SPAN)
#define ATCI_MAX_CMD_HEAD_LEN          (ATCI_MAX_CMD_NAME_LEN+3)


/* UART related */
#define ATCI_UART_RX_FIFO_ALERT_SIZE        (50)
#define ATCI_UART_RX_FIFO_THRESHOLD_SIZE    (128)
#define ATCI_UART_TX_FIFO_THRESHOLD_SIZE    (51) 

#if defined (MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)

#define ATCI_UART_RX_FIFO_BUFFER_SIZE      SERIAL_PORT_RECEIVE_BUFFER_SIZE
#define ATCI_TX_BUFFER_SIZE                SERIAL_PORT_SEND_BUFFER_SIZE
#else

// AT Command input data length
#define ATCI_UART_RX_FIFO_BUFFER_SIZE       (1024)
#define ATCI_TX_BUFFER_SIZE                 (1024)
#endif


#define ATCI_CR_C 13
#define ATCI_LF_C 10


typedef enum {
    MSG_ID_ATCI_LOCAL_SEND_CMD_IND = 2000,
	MSG_ID_ATCI_LOCAL_WRITE_CMD_IND,
    MSG_ID_ATCI_LOCAL_URC_NOTIFY_IND,
    MSG_ID_ATCI_LOCAL_RSP_NOTIFY_IND,

    MSG_ID_ATCI_END
} atci_msg_id_t;


typedef struct {
    atci_msg_id_t msg_id;
    uint8_t*            msg_data;
} atci_general_msg_t;


/* MSG_ID_ATCI_LOCAL_SEND_CMD_IND message content structure */
typedef struct {
    char     input_buf[ATCI_UART_RX_FIFO_BUFFER_SIZE];
    uint16_t input_len; 
    uint32_t flag;

} atci_send_input_cmd_msg_t;



typedef struct {
    /* the beginning structure need to be the same with atci_parse_cmd_param_t */
    uint8_t               *string_ptr;
    uint32_t              string_len;
    uint32_t              name_len;      /* AT command name length. ex. In "AT+EXAMPLE=1,2,3", name_len = 10 (not include = symbol) */ 
    uint32_t              parse_pos;     /* parse_pos means the length after detecting AT command mode */
    atci_cmd_mode_t mode;

    uint32_t              hash_value1;
    uint32_t              hash_value2;

} atci_parse_cmd_param_ex_t;



/* ATCI main body related */
extern atci_status_t  atci_uart_response_callback(atci_response_t *response_msg);

extern atci_status_t atci_input_command_handler(atci_send_input_cmd_msg_t* cmd_msg);
extern atci_status_t atci_deinit(hal_uart_port_t port);
uint8_t atci_check_startup_finish(void);


/* */
extern uint32_t g_atci_handler_mutex;
extern uint32_t g_atci_input_command_queue;
extern uint32_t g_atci_resonse_command_queue;
extern uint32_t g_atci_registered_table_number;
extern uint32_t atci_input_commad_in_processing;
extern atci_cmd_hdlr_table_t g_atci_cm4_general_hdlr_tables[];





#endif
