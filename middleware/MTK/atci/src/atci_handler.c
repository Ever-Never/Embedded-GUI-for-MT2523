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

#include "atci.h"
#include "atci_main.h"
#include "atci_adapter.h"

#include <string.h>
#include "syslog.h"

#define LOGE(fmt,arg...)   LOG_E(atci, "ATCI hdl: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci, "ATCI hdl: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci ,"ATCI hdl: "fmt,##arg)

#define ATCI_INVALID_COMMAND_HASH_VALUE     (0xffff)

#define ATCI_IS_LOWER( alpha_char )		    ( ( (alpha_char >= ATCI_char_a) && (alpha_char <= ATCI_char_z) ) ?  1 : 0 )

#define ATCI_IS_UPPER( alpha_char )       ( ( (alpha_char >= ATCI_CHAR_A) && (alpha_char <= ATCI_CHAR_Z) ) ? 1 : 0 )

#define ATCI_IS_HEX_ALPHA( alpha_char )		( ( (alpha_char >= ATCI_CHAR_A) && (alpha_char <= ATCI_CHAR_F) ) ? 1 : 0 )

#define ATCI_IS_NUMBER( alpha_char )      ( ( (alpha_char >= ATCI_CHAR_0) && (alpha_char <= ATCI_CHAR_9) ) ? 1 : 0 )

#define ATCI_IS_ALPHA( alpha_char )       ( ( ATCI_IS_UPPER(alpha_char) || ATCI_IS_LOWER(alpha_char) ) ? 1 : 0 )

#define ATCI_IS_SYMBOL( alpha_char )                                           \
    ( ( (alpha_char == ATCI_CHAR_PLUS) || (alpha_char == ATCI_CHAR_STAR) ||    \
        (alpha_char == ATCI_CHAR_POUND) || (alpha_char == ATCI_COMMA) ||       \
        (alpha_char == ATCI_CHAR_DOT) || (alpha_char == ATCI_FORWARD_SLASH) || \
        (alpha_char == ATCI_COLON) || (alpha_char == ATCI_HAT) ||              \
        (alpha_char == ATCI_CHAR_MINUS) ||                                     \
        (alpha_char == ATCI_L_SQ_BRACKET) ||                                   \
        (alpha_char == ATCI_R_SQ_BRACKET) ||                                   \
        (alpha_char == ATCI_L_ANGLE_BRACKET) ||                                \
        (alpha_char == ATCI_CHAR_ULINE) ||                                     \
        (alpha_char == ATCI_SPACE) ||                                          \
        (alpha_char == ATCI_SEMICOLON) ||                                      \
        (alpha_char == ATCI_R_ANGLE_BRACKET)                                   \
      ) ? 1 : 0 )

#define ATCI_IS_EXT_CHAR( alpha_char )                                                      \
    ( ( (alpha_char == ATCI_HAT) || (alpha_char == ATCI_CHAR_TILDE) ||                      \
        (alpha_char == ATCI_L_SQ_BRACKET) || (alpha_char == ATCI_R_SQ_BRACKET) ||           \
        (alpha_char == ATCI_L_CURLY_BRACKET) || (alpha_char == ATCI_R_CURLY_BRACKET) ||     \
        (alpha_char == ATCI_CHAR_REVERSE_SOLIDUS) ||(alpha_char == ATCI_CHAR_VERTICAL_LINE) \
      ) ? 1 : 0 )

#define ATCI_SET_OUTPUT_PARAM_STRING( s, ptr, len, flag)	    \
    {                                                           \
        memcpy((void*)s->response_buf, (uint8_t*)ptr, len);       \
        s->response_len = (uint32_t)(len);                        \
        s->response_flag = (uint32_t)(flag);                      \
    }


#define ATCI_SPACE                  (' ')
#define ATCI_EQUAL                  ('=')
#define ATCI_COMMA                  (')')
#define ATCI_SEMICOLON              (';')
#define ATCI_COLON                  (':')
#define ATCI_AT                     ('@')
#define ATCI_HAT                    ('^')
#define ATCI_DOUBLE_QUOTE           ('"')
#define ATCI_QUESTION_MARK          ('?')
#define ATCI_EXCLAMATION_MARK       ('!')
#define ATCI_FORWARD_SLASH          ('/')
#define ATCI_L_ANGLE_BRACKET        ('<')
#define ATCI_R_ANGLE_BRACKET        ('>')
#define ATCI_L_SQ_BRACKET           ('[')
#define ATCI_R_SQ_BRACKET           (']')
#define ATCI_L_CURLY_BRACKET        ('{')
#define ATCI_R_CURLY_BRACKET        ('}')
#define ATCI_CHAR_STAR              ('*')
#define ATCI_CHAR_POUND             ('#')
#define ATCI_CHAR_AMPSAND           ('&')
#define ATCI_CHAR_PERCENT           ('%')
#define ATCI_CHAR_PLUS              ('+')
#define ATCI_CHAR_MINUS             ('-')
#define ATCI_CHAR_DOT               ('.')
#define ATCI_CHAR_ULINE             ('_')
#define ATCI_CHAR_TILDE             ('~')
#define ATCI_CHAR_REVERSE_SOLIDUS   ('\\')
#define ATCI_CHAR_VERTICAL_LINE     ('|')
#define ATCI_END_OF_STRING_CHAR     ('\0')
#define ATCI_CHAR_0                 ('0')
#define ATCI_CHAR_1                 ('1')
#define ATCI_CHAR_2                 ('2')
#define ATCI_CHAR_3                 ('3')
#define ATCI_CHAR_4                 ('4')
#define ATCI_CHAR_5                 ('5')
#define ATCI_CHAR_6                 ('6')
#define ATCI_CHAR_7                 ('7')
#define ATCI_CHAR_8                 ('8')
#define ATCI_CHAR_9                 ('9')
#define ATCI_CHAR_A                 ('A')
#define ATCI_CHAR_B                 ('B')
#define ATCI_CHAR_C                 ('C')
#define ATCI_CHAR_D                 ('D')
#define ATCI_CHAR_E                 ('E')
#define ATCI_CHAR_F                 ('F')
#define ATCI_CHAR_G                 ('G')
#define ATCI_CHAR_H                 ('H')
#define ATCI_CHAR_I                 ('I')
#define ATCI_CHAR_J                 ('J')
#define ATCI_CHAR_K                 ('K')
#define ATCI_CHAR_L                 ('L')
#define ATCI_CHAR_M                 ('M')
#define ATCI_CHAR_N                 ('N')
#define ATCI_CHAR_O                 ('O')
#define ATCI_CHAR_P                 ('P')
#define ATCI_CHAR_Q                 ('Q')
#define ATCI_CHAR_R                 ('R')
#define ATCI_CHAR_S                 ('S')
#define ATCI_CHAR_T                 ('T')
#define ATCI_CHAR_U                 ('U')
#define ATCI_CHAR_V                 ('V')
#define ATCI_CHAR_W                 ('W')
#define ATCI_CHAR_X                 ('X')
#define ATCI_CHAR_Y                 ('Y')
#define ATCI_CHAR_Z                 ('Z')
#define ATCI_char_a                 ('a')
#define ATCI_char_b                 ('b')
#define ATCI_char_c                 ('c')
#define ATCI_char_d                 ('d')
#define ATCI_char_e                 ('e')
#define ATCI_char_f                 ('f')
#define ATCI_char_g                 ('g')
#define ATCI_char_h                 ('h')
#define ATCI_char_i                 ('i')
#define ATCI_char_j                 ('j')
#define ATCI_char_k                 ('k')
#define ATCI_char_l                 ('l')
#define ATCI_char_m                 ('m')
#define ATCI_char_n                 ('n')
#define ATCI_char_o                 ('o')
#define ATCI_char_p                 ('p')
#define ATCI_char_q                 ('q')
#define ATCI_char_r                 ('r')
#define ATCI_char_s                 ('s')
#define ATCI_char_t                 ('t')
#define ATCI_char_u                 ('u')
#define ATCI_char_v                 ('v')
#define ATCI_char_w                 ('w')
#define ATCI_char_x                 ('x')
#define ATCI_char_y                 ('y')
#define ATCI_char_z                 ('z')
#define ATCI_R_BRACKET              (')')
#define ATCI_L_BRACKET              ('(')
#define ATCI_MONEY                  ('$')


extern uint32_t g_atci_handler_semaphore;
extern uint8_t  g_atci_local_startup_flag;
extern uint32_t atci_input_commad_in_processing;

/*------ Private Typedef ------*/

/*------ Private function Pre-declare ------*/
static atci_status_t atci_init_hdlr_tbl_hash_value(atci_cmd_hdlr_item_t* hdlr_table, uint16_t hdlr_number);
static atci_status_t atci_add_to_cm4_hdlr_tbls(atci_cmd_hdlr_item_t* hdlr_table, uint16_t hdlr_number);
static atci_status_t atci_process_flag(uint8_t *str_ptr, uint16_t *str_len_ptr, uint16_t str_max_len, uint32_t flag);
static atci_status_t atci_local_parse_input_at_cmd(atci_parse_cmd_param_ex_t *parse_cmd);
static atci_status_t atci_local_parse_extend_cmd(atci_parse_cmd_param_ex_t *parse_cmd);
static atci_status_t atci_local_parse_basic_cmd(atci_parse_cmd_param_ex_t *parse_cmd);
static atci_status_t atci_local_parse_find_mode(atci_parse_cmd_param_ex_t *parse_cmd);
static atci_status_t atci_check_end_valid_char (char c);
static atci_status_t atci_local_dispatch_cm4_general_handler(atci_parse_cmd_param_ex_t *parse_cmd);
static uint16_t    atci_local_caculate_hash_value(uint8_t *at_name, uint32_t *hash_value1, uint32_t *hash_value2);
static atci_status_t atci_local_check_duplicate_cm4_handler(atci_cmd_hdlr_item_t *parse_cmd, int32_t hdlr_number);


/* External API */
extern uint8_t atci_check_startup_finish(void);

/* Register AT handler */
atci_status_t atci_register_handler(atci_cmd_hdlr_item_t* hdlr_table, int32_t hdlr_number)
{
    atci_status_t ret = ATCI_STATUS_REGISTRATION_FAILURE;

    //printf("atci_register_handler \r\n");
    //if(atci_check_startup_finish()) {
	if(1) {
   	     /* Init hash value */
   	     ret = atci_init_hdlr_tbl_hash_value(hdlr_table, hdlr_number);

		 ret = atci_local_check_duplicate_cm4_handler(hdlr_table, hdlr_number);
		 
		 if (ret == ATCI_STATUS_OK) {
		 	
			 /* lock g_atci_handler_mutex */
			 atci_mutex_lock(g_atci_handler_mutex);
			 
			 /* Add the handler table */
			 ret = atci_add_to_cm4_hdlr_tbls(hdlr_table, hdlr_number);
			 
			 /* unlock g_atci_handler_mutex */
			 atci_mutex_unlock(g_atci_handler_mutex);
		 }

		 if ( ret == ATCI_STATUS_OK) {
        
			 LOGI("atci handler register success \r\n");
		 } else {
			 LOGE("atci handler register fail \r\n");
			 ret = ATCI_STATUS_REGISTRATION_FAILURE;
		 }
    } else {
    
		LOGE("atci handler register fail, please call atci_init() frist \r\n");
    }

    return ret;
}

/* Send response to ATCI */
atci_status_t atci_send_response(atci_response_t *response)
{
    atci_status_t        ret = ATCI_STATUS_ERROR;
    atci_general_msg_t   msg_queue_item;
	uint8_t              *msg_data = NULL;
	uint16_t			notify_queue_msg_num = 0;

	if (g_atci_local_startup_flag != 1) {
		LOGW("atci_send_response() fail, atci does not ready!\r\n");
		return ret;
	}

	notify_queue_msg_num = atci_queue_get_item_num(g_atci_resonse_command_queue);

	if (notify_queue_msg_num >= ATCI_LOCAL_QUEUE_LENGTH)
	{
    	atci_process_flag((void*)response->response_buf, (uint16_t*)&(response->response_len),
                      ATCI_UART_TX_FIFO_BUFFER_SIZE, response->response_flag);
		
		if (ATCI_RESPONSE_FLAG_URC_FORMAT == (response->response_flag & ATCI_RESPONSE_FLAG_URC_FORMAT)) {
			LOGW("atci_send_response(), atci response queue full(%d), drop this urc data!\r\n", notify_queue_msg_num);
		} else {

			if(atci_input_commad_in_processing == 2)
				atci_input_commad_in_processing = 3;
			
			LOGW("atci_send_response(), atci response queue full(%d), drop this rsp data!\r\n", notify_queue_msg_num);
		}

//		LOGW("atci_send_response(), atci response queue full(%d), drop this rsp data!\r\n", notify_queue_msg_num);
		return ret;
	}

	
    /* send ATCI_SEND_CMD msg to ATCI queue */
    atci_process_flag((void*)response->response_buf, (uint16_t*)&(response->response_len),
                      ATCI_UART_TX_FIFO_BUFFER_SIZE, response->response_flag);

    msg_data = (uint8_t *)atci_mem_alloc(response->response_len + sizeof(response->response_len));
	
	if (msg_data == NULL) {
		LOGW("atci_send_response(), atci_mem_alloc fail \r\n");
		return ret;
	}

	memcpy((void*)(msg_data), (uint8_t*)(&(response->response_len)), sizeof(response->response_len));
    memcpy((void*)(msg_data + sizeof(response->response_len)), (uint8_t*)response->response_buf, response->response_len);

    if (ATCI_RESPONSE_FLAG_URC_FORMAT == (response->response_flag & ATCI_RESPONSE_FLAG_URC_FORMAT)) {
        msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_URC_NOTIFY_IND;
    } else {
        msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_RSP_NOTIFY_IND;
    }
    msg_queue_item.msg_data = (void *)msg_data;
    ret = atci_queue_send(g_atci_resonse_command_queue,(void*)&msg_queue_item);
    atci_semaphore_give(g_atci_handler_semaphore);
    return ret;
}

/* Internal API */
static atci_status_t atci_init_hdlr_tbl_hash_value(atci_cmd_hdlr_item_t* hdlr_table, uint16_t hdlr_number)
{
    uint16_t    i;
    uint8_t     *cmd_ptr;

    for (i = 0;i < hdlr_number;i++) {
        cmd_ptr = (uint8_t *)hdlr_table[i].command_head;

        /* caculate hash value of AT command name */
        atci_local_caculate_hash_value(cmd_ptr, &(hdlr_table[i].hash_value1),&(hdlr_table[i].hash_value2));
    }
    return ATCI_STATUS_OK;
}

static atci_status_t atci_add_to_cm4_hdlr_tbls(atci_cmd_hdlr_item_t* hdlr_table, uint16_t hdlr_number)
{
    if (g_atci_registered_table_number >= ATCI_MAX_GNENERAL_TABLE_NUM) {
        /* exceed the max registed table number */
        LOGE("atci handler register fail, hdlr table extend \r\n");
        return ATCI_STATUS_REGISTRATION_FAILURE;
    }
    g_atci_cm4_general_hdlr_tables[g_atci_registered_table_number].item_table = hdlr_table;
    g_atci_cm4_general_hdlr_tables[g_atci_registered_table_number].item_table_size = hdlr_number;
    g_atci_registered_table_number++;
    return ATCI_STATUS_OK;
}


static atci_status_t atci_process_flag(uint8_t *str_ptr, uint16_t *str_len_ptr, uint16_t str_max_len, uint32_t flag)
{
    uint16_t    str_len = (*str_len_ptr);
    int32_t     i;

    if (ATCI_RESPONSE_FLAG_APPEND_OK == (flag & ATCI_RESPONSE_FLAG_APPEND_OK)) {
        /* append 'O','K',CR,LF */
        if (str_len + 4 > str_max_len) {
            return ATCI_STATUS_ERROR;
        }
        str_ptr[str_len] = ATCI_CHAR_O;
        str_ptr[str_len+1] = ATCI_CHAR_K;
        str_ptr[str_len+2] = ATCI_CR_C;
        str_ptr[str_len+3] = ATCI_LF_C;
        str_len +=4;
    }
    if (ATCI_RESPONSE_FLAG_APPEND_ERROR == (flag & ATCI_RESPONSE_FLAG_APPEND_ERROR)) {
        /* append 'E','R','R','O','R',CR,LF */
        if (str_len + 7 > str_max_len) {
            return ATCI_STATUS_ERROR;
        }
        str_ptr[str_len] = ATCI_CHAR_E;
        str_ptr[str_len+1] = ATCI_CHAR_R;
        str_ptr[str_len+2] = ATCI_CHAR_R;
        str_ptr[str_len+3] = ATCI_CHAR_O;
        str_ptr[str_len+4] = ATCI_CHAR_R;
        str_ptr[str_len+5] = ATCI_CR_C;
        str_ptr[str_len+6] = ATCI_LF_C;
        str_len +=7;
    }
    if (ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR == (flag & ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR)) {
        if (str_len + 2 > str_max_len) {
            return ATCI_STATUS_ERROR;
        }
        /* APPEND BACK CR LF */
        str_ptr[str_len] = ATCI_CR_C;
        str_ptr[str_len+1] = ATCI_LF_C;
        str_len += 2;
    }
    if (ATCI_RESPONSE_FLAG_QUOTED_WITH_LF_CR == (flag & ATCI_RESPONSE_FLAG_QUOTED_WITH_LF_CR)) {
        if (str_len + 4 > str_max_len) {
            return ATCI_STATUS_ERROR;
        }
        /* Left shift 2 position */
        for (i = str_len-1;i>=0;i--) {
            str_ptr[i+2] = str_ptr[i];
        }
        /* APPEND FRONT CR LF */
        str_ptr[0] = ATCI_CR_C;
        str_ptr[1] = ATCI_LF_C;
        str_len += 2;
        /* APPEND BACK CR LF */
        str_ptr[str_len] = ATCI_CR_C;
        str_ptr[str_len+1] = ATCI_LF_C;
        str_len += 2;
    }

    /* add null terminal at end of string buffer */
    if (str_max_len > (str_len)) {
        str_ptr[str_len] = ATCI_END_OF_STRING_CHAR;
    }
    (*str_len_ptr) = str_len;
    
    return ATCI_STATUS_OK;
}


/* Parse AT command */
atci_status_t atci_input_command_handler(atci_send_input_cmd_msg_t* cmd_msg)
{
    atci_status_t                 ret;
    atci_parse_cmd_param_ex_t  parse_cmd[1];
    atci_response_t   response[1];// = {{0}};

   // char    ok_str[] = {'O', 'K', '\r', '\n', '\0'};
   // char    error_str[] = {'E','R','R','O','R','\r','\n','\0'};
	//char	ok_str[]="OK\r\n";
	//char	error_str[] = "ERROR\r\n";
  

    LOGI("atci_input_command_handler, input_buf: %s\n", cmd_msg->input_buf);
    
    parse_cmd->string_ptr = ( uint8_t *)cmd_msg->input_buf;
    parse_cmd->string_len = strlen(cmd_msg->input_buf);

    if (((ATCI_CHAR_A == parse_cmd->string_ptr[0] || ATCI_char_a == parse_cmd->string_ptr[0]) &&
        (ATCI_CHAR_T == parse_cmd->string_ptr[1] || ATCI_char_t == parse_cmd->string_ptr[1]) &&
        (ATCI_CR_C == parse_cmd->string_ptr[2])) || (ATCI_LF_C == parse_cmd->string_ptr[2]) || ('\0' == parse_cmd->string_ptr[2])) {

        /* AT command format is 'AT' and returns "OK" directly. Set Output Buffer Param */
        ATCI_SET_OUTPUT_PARAM_STRING(response, "OK\r\n", strlen("OK\r\n"), cmd_msg->flag);
        atci_send_response(response);
        return ATCI_STATUS_OK;
    }
    /* Parse AT command and calculate hash value*/
    ret = atci_local_parse_input_at_cmd(parse_cmd);
    
    if (ATCI_STATUS_ERROR != ret) {
        /* Dispatch AT command */
        ret = atci_local_dispatch_cm4_general_handler(parse_cmd); 
    }

    if (ATCI_STATUS_OK != ret) {
        /* AT command format is invalid and returns "ERROR" directly. Set Output Buffer Param */
        ATCI_SET_OUTPUT_PARAM_STRING(response, "ERROR\r\n", strlen("ERROR\r\n"), cmd_msg->flag);
        atci_send_response(response);
    }

    return ret;
}


/*------ Private function Entity ------*/
static atci_status_t atci_local_parse_basic_cmd(atci_parse_cmd_param_ex_t *parse_cmd)
{
    uint32_t name_len = 2;   /* AT command begins "AT" */
    uint8_t  *string_ptr = parse_cmd->string_ptr;
    uint32_t length = parse_cmd->string_len;

    while (name_len < length) {
        if ((string_ptr[name_len]  == ATCI_CR_C) || (string_ptr[name_len]  == ATCI_LF_C || (string_ptr[name_len]  == '\0'))) {
            break;
        }
        name_len++;
    }
    parse_cmd->name_len = name_len;
    parse_cmd->parse_pos = name_len;
    return ATCI_STATUS_OK;
}

static atci_status_t atci_local_parse_extend_cmd(atci_parse_cmd_param_ex_t *parse_cmd)
{
    uint32_t name_len = 0;
    uint8_t* at_name = parse_cmd->string_ptr;

    name_len = atci_local_caculate_hash_value(at_name,&(parse_cmd->hash_value1),&(parse_cmd->hash_value2));
    parse_cmd->name_len = name_len;
    parse_cmd->parse_pos = name_len;
    if (0 == name_len) {
        return ATCI_STATUS_ERROR;
    } else {
        return ATCI_STATUS_OK;
    }
}
static atci_status_t atci_local_parse_find_mode(atci_parse_cmd_param_ex_t *parse_cmd)
{
    atci_cmd_mode_t mode = ATCI_CMD_MODE_INVALID;
    uint16_t         index = parse_cmd->name_len;
    char             *string_ptr = (char *)parse_cmd->string_ptr;

    if (string_ptr[index] == ATCI_QUESTION_MARK)
    {
        index++;
        if (atci_check_end_valid_char(string_ptr[index]) == ATCI_STATUS_OK) {
            mode = ATCI_CMD_MODE_READ;			
        }
    } else if (string_ptr[index] == ATCI_EQUAL) {
        index++;
        if (string_ptr[index] == ATCI_QUESTION_MARK) {
            index++;
            if (atci_check_end_valid_char(string_ptr[index]) == ATCI_STATUS_OK) {
                mode = ATCI_CMD_MODE_TESTING;
             }
        } else {
            mode = ATCI_CMD_MODE_EXECUTION;
        }
    } else if (atci_check_end_valid_char(string_ptr[index]) == ATCI_STATUS_OK) {
        mode = ATCI_CMD_MODE_ACTIVE;
    }
    parse_cmd->parse_pos = index;
    parse_cmd->mode = mode;
    return ATCI_STATUS_OK;
}

static atci_status_t atci_check_end_valid_char(char c) {
   if ((c  == ATCI_CR_C) || (c  == ATCI_LF_C) || (c  == '\0')) {
       return ATCI_STATUS_OK;
    } 
    return ATCI_STATUS_ERROR;
        
}

static atci_status_t atci_local_parse_input_at_cmd(atci_parse_cmd_param_ex_t *parse_cmd)
{
    atci_status_t ret;
    char        *string_ptr = (char *)parse_cmd->string_ptr;
    uint32_t    length = parse_cmd->string_len;
    uint16_t    index = 0;

    LOGI("atci_local_parse_input_at_cmd, string: %s\n", parse_cmd->string_ptr);
    
    /* Check the data valid*/
    /* AT<CR><LF> is the shortest length */
    if (length < 4) {
        return ATCI_STATUS_ERROR;
    }

    /* Skip not AT */
    while(index < length-1) {
        if (ATCI_CHAR_A == string_ptr[index] || ATCI_char_a == string_ptr[index]) {
            if (ATCI_CHAR_T == string_ptr[index+1] || ATCI_char_t == string_ptr[index+1]) {
                break;
            }
        }
        index++;
    }
    if (index >= length-1) {
        return ATCI_STATUS_ERROR;
    }
    index += 2; /*Skip AT*/

    /* Calculate has value and extend AT command: AT+, AT#, AT%, AT*, AT^, AT$ */
    if (ATCI_CHAR_PLUS == string_ptr[index] || ATCI_CHAR_POUND == string_ptr[index] || ATCI_CHAR_PERCENT == string_ptr[index] ||
        ATCI_CHAR_STAR == string_ptr[index] || ATCI_HAT == string_ptr[index] || ATCI_MONEY == string_ptr[index]) {
        //parse_cmd->parse_pos = index + 1;
        ret = atci_local_parse_extend_cmd(parse_cmd);
        atci_local_parse_find_mode(parse_cmd);
    } else {
        parse_cmd->parse_pos = index;
        ret = atci_local_parse_basic_cmd(parse_cmd);
    }

    return ret;
}

atci_status_t atci_local_find_cm4_handler(atci_cmd_hdlr_item_t *parse_cmd, atci_cmd_hdlr_item_t *handler_item)
{

	atci_status_t			 ret = ATCI_STATUS_ERROR;
	uint32_t			  item_table_size;
	uint16_t			  i, j;
	//atci_cmd_hdlr_item_t *handler_item = NULL;

	/* lock g_atci_handler_mutex */
	atci_mutex_lock(g_atci_handler_mutex);

	handler_item = NULL;

	for(i = 0;i < g_atci_registered_table_number; i++) {
		
		item_table_size = g_atci_cm4_general_hdlr_tables[i].item_table_size;
		
		if (ret == ATCI_STATUS_OK) {
		 break;
		}

		for(j = 0;j < item_table_size;j++) {
			handler_item = &(g_atci_cm4_general_hdlr_tables[i].item_table[j]);

			if (parse_cmd->hash_value1 == handler_item->hash_value1 && parse_cmd->hash_value2 == handler_item->hash_value2 &&
				  !strcmp(parse_cmd->command_head, handler_item->command_head)) {
				ret = ATCI_STATUS_OK;
				break;
			}

			handler_item = NULL;
		}
	}

	/* unlock g_atci_handler_mutex */
	atci_mutex_unlock(g_atci_handler_mutex);

	return ret;

}

atci_status_t atci_local_check_duplicate_cm4_handler(atci_cmd_hdlr_item_t *parse_cmd, int32_t hdlr_number)
{
    atci_status_t           ret = ATCI_STATUS_ERROR;
	atci_status_t           hdlr_ret = ATCI_STATUS_ERROR;
    uint16_t              i = 0;
	uint16_t              j = 0;
	uint16_t              dup_num = 0;
    atci_cmd_hdlr_item_t *handler_item = NULL;


	for (i = 0; i < hdlr_number; i++) {

		/* check current AT cmd whether duplicate  in current cmd table*/

		for (j = 0; j < i; j++) {
			
			if ((parse_cmd + i)->hash_value1 == (parse_cmd + j)->hash_value1 && (parse_cmd + i)->hash_value2 == (parse_cmd + j)->hash_value2 &&
				  !strcmp((parse_cmd + i)->command_head, (parse_cmd + j)->command_head)) {
				hdlr_ret = ATCI_STATUS_OK;

				dup_num ++;
				LOGW("duplicate AT command in current cmd table:%s \r\n", (parse_cmd + i)->command_head);
				break;
			}
		}

		if (hdlr_ret == ATCI_STATUS_ERROR) {

			/* check current AT cmd is already  in register table*/
			hdlr_ret = atci_local_find_cm4_handler(parse_cmd + i, handler_item);

			if (hdlr_ret == ATCI_STATUS_OK) {
				
				dup_num ++;
				LOGW("duplicate AT command in register cmd table:%s \r\n", (parse_cmd + i)->command_head);
			}	
		}
	}

    if (dup_num == 0) {

		ret = ATCI_STATUS_OK;
	}


	return ret;

}


/* Dispatch Handler */
static atci_status_t atci_local_dispatch_cm4_general_handler(atci_parse_cmd_param_ex_t *parse_cmd)
{
    atci_status_t           ret = ATCI_STATUS_ERROR;
    uint32_t              item_table_size;
    uint16_t              i,j;
    atci_cmd_hdlr_item_t *handler_item = NULL;

    /* lock g_atci_handler_mutex */
    atci_mutex_lock(g_atci_handler_mutex);

    for(i = 0;i < g_atci_registered_table_number;i++) {
        item_table_size = g_atci_cm4_general_hdlr_tables[i].item_table_size;
		
		if (ret == ATCI_STATUS_OK) {
		 break;
		}

        for(j = 0;j < item_table_size;j++) {
            handler_item = &(g_atci_cm4_general_hdlr_tables[i].item_table[j]);

            if (parse_cmd->hash_value1 == handler_item->hash_value1 && parse_cmd->hash_value2 == handler_item->hash_value2 &&
            	  !strncmp((char *)parse_cmd->string_ptr, handler_item->command_head, parse_cmd->name_len)) {
                ret = ATCI_STATUS_OK;
                break;
            }

            handler_item = NULL;
        }
    }

    /* unlock g_atci_handler_mutex */
    atci_mutex_unlock(g_atci_handler_mutex);

    /* executing AT command handler */
    if ((NULL != handler_item) && (ret == ATCI_STATUS_OK)) {
        handler_item->command_hdlr((atci_parse_cmd_param_t*)parse_cmd);
    } else {

		LOGE("find command handler fail \r\n");
	}

    return ret;
}


uint16_t atci_local_caculate_hash_value(uint8_t *at_name,uint32_t *hash_value1,uint32_t *hash_value2)
{
    uint16_t i = 0,counter = 0;
    char     ascii_char = 0;
    uint32_t value1 = 0;
    uint32_t value2 = 0;

    (*hash_value1) = ATCI_INVALID_COMMAND_HASH_VALUE;
    (*hash_value2) = ATCI_INVALID_COMMAND_HASH_VALUE;
    if ((ATCI_CHAR_A == at_name[0] && ATCI_CHAR_T == at_name[1]) ||
        (ATCI_char_a == at_name[0] && ATCI_char_t == at_name[1])) {

        /* Only support 'AT+XXX' 'AT#XXX' 'AT%XXX' 'AT*XXX'*/
        if (ATCI_CHAR_PLUS == at_name[2] || ATCI_CHAR_POUND == at_name[2]) {

            /* caculate hash value after ("AT+" or "AT#") until entering ('=' or '?' or CR/LF/NULL) */
            i = 3;
            counter = 3;
            while ((at_name[i] != ATCI_EQUAL) && (at_name[i] != ATCI_QUESTION_MARK) &&
                    (at_name[i] != ATCI_CR_C) &&(at_name[i] != ATCI_LF_C) && (at_name[i] != '\0')) {

                if (ATCI_IS_UPPER(at_name[i])) {
                    ascii_char = at_name[i] - ATCI_CHAR_A;
                } else if (ATCI_IS_LOWER(at_name[i])) {
                    ascii_char = at_name[i] - ATCI_char_a;
                } else if (ATCI_IS_NUMBER(at_name[i])) {
                    ascii_char = at_name[i] - ATCI_CHAR_0;
                }
                if (counter < (ATCI_HASH_TABLE_SPAN + 3)) {
                    value1 = value1*(ATCI_HASH_TABLE_ROW+1)+(ascii_char+1); /* 0 ~ 4*/
                } else if (counter < ATCI_MAX_CMD_NAME_LEN + 3){
                    value2 = value2*(ATCI_HASH_TABLE_ROW+1)+(ascii_char+1); /* 5 ~ 9*/
                }

                counter++;
                i++;
            }
        }
    }

    (*hash_value1) = value1;
    (*hash_value2) = value2;
    return counter;
}

