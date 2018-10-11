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

// For Register AT command handler
//#include "atci.h"

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"

#ifdef HAL_SD_MODULE_ENABLED
#if defined(MTK_BUILD_SMT_LOAD) || defined(MTK_BUILD_HQA_LOAD)
#include "hal_sd.h"
#include "hal_msdc.h"
#include "hal_log.h"
#include "stdlib.h"
#include "memory_attribute.h"
#include "task_def.h"

/*
 * sample code
*/

#define MSDC0_MSDC_IOCON_REG_ADDR (0xA0020014)
#define MSDC1_MSDC_IOCON_REG_ADDR (0xA0030014)

#define TEST_BLOCK_NUMBER  (2)
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint32_t msdc_buf[128 * TEST_BLOCK_NUMBER];  /*512byte * TEST_BLOCK_NUMBER = 64K bytes*/

hal_sd_config_t sd_cfg = { HAL_SD_BUS_WIDTH_4,
                           45000
                         };

static volatile bool is_read_write_going = 0;
static volatile bool is_write;
static volatile uint32_t msdc_number;
static volatile bool is_stop_done;
static bool read_write_OK = false;

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_msdc(atci_parse_cmd_param_t *parse_cmd);

/*
AT+EMSDC=<op>,[<p1>[,<p2>,<p3>]]                  |   [+EMSDC:<p1>[,<p2>,<p3>]] OK / ERROR

*/
extern bool msdc_card_is_present(msdc_port_t msdc_port);


TaskHandle_t  atci_msdc_read_write_task_handle;

void atci_msdc_read_write_task_main(void *pvParameters)
{
    uint32_t i;
    uint32_t address;
    

    is_stop_done = false;

    while (is_read_write_going) {

        vTaskDelay(1000 / portTICK_RATE_MS); //delay and sleep

        for (i = 0; i < (128 * TEST_BLOCK_NUMBER); i++) {
            msdc_buf[i] = 0xA5A5A5A5;
        }

        if (is_write) {
            for (address = 0; address < 1024; address++) {
                if (0 > hal_sd_write_blocks_dma_blocking((hal_sd_port_t) msdc_number, msdc_buf, address, TEST_BLOCK_NUMBER)) {
                    log_hal_info("atci_msdc_read_write_task_main,WRITE ERROR!!!!!\r\n");
                    goto error;
                } else {
                    log_hal_info("write ok\r\n");
                }
            }
        } else {
            for (address = 0; address < 1024; address++) {
                if (0 > hal_sd_read_blocks_dma_blocking((hal_sd_port_t) msdc_number, msdc_buf, address, TEST_BLOCK_NUMBER)) {
                    log_hal_info("atci_msdc_read_write_task_main,READ ERROR!!!!!\r\n");
                    goto error;
                } else {
                    log_hal_info("read ok\r\n");
                }
            }
        }
        while(!read_write_OK);
        read_write_OK = false;
        log_hal_info("atci_msdc_read_write_task_main,READ OR WRITE OK \r\n");
    }

error:
    is_stop_done = true;
    log_hal_info("atci_msdc_read_write_task_main,TEST STOP \r\n");
    while (1);
}

void atci_msdc_read_write_task_init(void)
{
    xTaskCreate(atci_msdc_read_write_task_main, ATCI_MSDC_TASK_NAME, ATCI_MSDC_TASK_STACKSIZE / sizeof(StackType_t), NULL, ATCI_MSDC_TASK_PRIO, &atci_msdc_read_write_task_handle);
}

void atci_msdc_read_write_task_deinit(void)
{
    vTaskDelete(atci_msdc_read_write_task_handle);
}


void atci_msdc_modify_io_parameter(hal_sd_port_t sd_port, uint32_t io_config)
{
    if (HAL_SD_PORT_0 == sd_port) {
        *(volatile uint32_t *)MSDC0_MSDC_IOCON_REG_ADDR = ((*(volatile uint32_t *)MSDC0_MSDC_IOCON_REG_ADDR) & (~ MSDC_IOCON_ODCCFG0_MASK)) |
                (io_config & MSDC_IOCON_ODCCFG0_MASK);
    } else if (HAL_SD_PORT_1 == sd_port) {
        *(volatile uint32_t *)MSDC1_MSDC_IOCON_REG_ADDR = ((*(volatile uint32_t *)MSDC1_MSDC_IOCON_REG_ADDR) & (~ MSDC_IOCON_ODCCFG0_MASK)) |
                (io_config & MSDC_IOCON_ODCCFG0_MASK);
    }
}

void atci_msdc_get_io_parameter(hal_sd_port_t sd_port, uint32_t *io_config)
{
    if (HAL_SD_PORT_0 == sd_port) {
        *io_config = (*(volatile uint32_t *)MSDC0_MSDC_IOCON_REG_ADDR) & MSDC_IOCON_ODCCFG0_MASK;
    } else if (HAL_SD_PORT_1 == sd_port) {
        *io_config = (*(volatile uint32_t *)MSDC1_MSDC_IOCON_REG_ADDR) & MSDC_IOCON_ODCCFG0_MASK;
    }
}

// AT command handler
atci_status_t atci_cmd_hdlr_msdc(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    int param_val[4] = {0, 0, 0, 0};
    int i = 0;
    uint32_t msdc_iocon_driving;
    uint32_t clock = 0;
    char str[10];

    log_hal_info("atci_cmd_hdlr_msdc \r\n");

    memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            strcpy((char *)resonse.response_buf, "+EMSDC:(0~7),<p1>,<p2>,<p3>\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EMSDC=<op>  the handler need to parse the parameters

            param = strtok(parse_cmd->string_ptr, "=,\n\r");
            while (NULL != (param = strtok(NULL, ",\n\r"))) {
                param_val[i++] = atoi(param);
            }
            log_hal_info("atci_cmd_hdlr_msdc OP= %d, P1=%d, P2=%d, P3=%d \r\n", param_val[0], param_val[1], param_val[2], param_val[3]);

            if ((0 != param_val[1]) && (1 != param_val[1])) {
                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                atci_send_response(&resonse);
                break;
            }

            if (0 == param_val[0]) {
                if (NULL != atci_msdc_read_write_task_handle) {
                    atci_msdc_read_write_task_deinit();
                    atci_msdc_read_write_task_handle = NULL;
                }

                /*deinit msdc and sd card*/
                hal_sd_deinit((hal_sd_port_t) param_val[1]);
                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&resonse);
            } else if (1 == param_val[0]) {

                /*init msdc and sd card*/
                if (0 <= hal_sd_init((hal_sd_port_t) param_val[1], &sd_cfg)) {
                    resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                    atci_send_response(&resonse);
                } else {
                    resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(&resonse);
                }
            } else if (2 == param_val[0]) {
                /*set msdc driving strength*/
                msdc_iocon_driving = ((param_val[3] & 0x01) << MSDC_IOCON_SRCFG1_OFFSET) |
                                     ((param_val[3] & 0x01) << MSDC_IOCON_SRCFG0_OFFSET) |
                                     ((param_val[2] & 0x03) << MSDC_IOCON_ODCCFG1_OFFSET) |
                                     ((param_val[2] & 0x03) << MSDC_IOCON_ODCCFG0_OFFSET);

                atci_msdc_modify_io_parameter((hal_sd_port_t)param_val[1], msdc_iocon_driving);

                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&resonse);
            } else if (3 == param_val[0]) {
                /*query msdc driving strength*/
                log_hal_info("+EMSDC:");
                log_hal_info("%d,", param_val[1]);

                strcpy((char *)resonse.response_buf, "+EMSDC:");
                if (HAL_SD_PORT_0 == param_val[1]) {
                    strcat((char *)resonse.response_buf, "0,");
                } else if (HAL_SD_PORT_0 == param_val[1]) {
                    strcat((char *)resonse.response_buf, "1,");
                }

                atci_msdc_get_io_parameter((hal_sd_port_t)param_val[1], &msdc_iocon_driving);

                if (0 == (msdc_iocon_driving & 0x03)) {
                    log_hal_info("0,");
                    strcat((char *)resonse.response_buf, "0,");
                } else if (1 == (msdc_iocon_driving & 0x03)) {
                    log_hal_info("1,");
                    strcat((char *)resonse.response_buf, "1,");
                } else if (2 == (msdc_iocon_driving & 0x03)) {
                    log_hal_info("2,");
                    strcat((char *)resonse.response_buf, "2,");
                } else if (3 == (msdc_iocon_driving & 0x03)) {
                    log_hal_info("3,");
                    strcat((char *)resonse.response_buf, "3,");
                }

                if (0 == ((msdc_iocon_driving >> MSDC_IOCON_SRCFG0_OFFSET) & 0x01)) {
                    log_hal_info("0\r\nOK\r\n");
                    strcat((char *)resonse.response_buf, "0\r\n");
                } else if (1 == ((msdc_iocon_driving >> MSDC_IOCON_SRCFG0_OFFSET) & 0x01)) {
                    log_hal_info("1\r\nOK\r\n");
                    strcat((char *)resonse.response_buf, "1\r\n");
                }

                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (4 == param_val[0]) {
                /*set clock*/
                hal_sd_set_clock((hal_sd_port_t) param_val[1], param_val[2]);
                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&resonse);
            } else if (5 == param_val[0]) {
                log_hal_info("+EMSDC:");
                strcpy((char *)resonse.response_buf, "+EMSDC:");
                if (HAL_SD_PORT_0 == param_val[1]) {
                    strcat((char *)resonse.response_buf, "0,");
                } else if (HAL_SD_PORT_0 == param_val[1]) {
                    strcat((char *)resonse.response_buf, "1,");
                }
                /*get clock*/
                hal_sd_get_clock((hal_sd_port_t) param_val[1], &clock);
                log_hal_info("+EMSDC:%d,%d\r\nOK\r\n", param_val[1], clock);
                sprintf(str, "%d\r\n", (int)clock);
                strcat((char *)resonse.response_buf, str);

                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (6 == param_val[0]) {
                /*read data*/
                if (0 <= hal_sd_read_blocks((hal_sd_port_t) param_val[1], msdc_buf, 0, 1)) {
                    resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                    atci_send_response(&resonse);
                } else {
                    resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(&resonse);
                }
            } else if (7 == param_val[0]) {
                /*write data*/
                for (i = 0; i < 128; i++) {
                    msdc_buf[i] = (i << 24) | (i << 16) | (i << 8) | i;
                }
                if (0 <= hal_sd_write_blocks((hal_sd_port_t) param_val[1], msdc_buf, 0, 1)) {
                    memset(msdc_buf, 0, 128 * sizeof(unsigned int));
                    /*read & compare*/
                    if (0 <= hal_sd_read_blocks((hal_sd_port_t) param_val[1], msdc_buf, 0, 1)) {
                        for (i = 0; i < 128; i++) {
                            if (msdc_buf[i] != ((i << 24) | (i << 16) | (i << 8) | i)) {
                                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                                atci_send_response(&resonse);
                                return ATCI_STATUS_OK;
                            }
                        }
                        resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                        atci_send_response(&resonse);
                    } else {
                        resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        atci_send_response(&resonse);
                    }
                } else {
                    resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(&resonse);
                }
            } else if (8 == param_val[0]) {
                log_hal_info("+EMSDC:");
                log_hal_info("%d,", param_val[1]);
                strcpy((char *)resonse.response_buf, "+EMSDC:");
                if (HAL_SD_PORT_0 == param_val[1]) {
                    strcat((char *)resonse.response_buf, "0,");
                } else if (HAL_SD_PORT_0 == param_val[1]) {
                    strcat((char *)resonse.response_buf, "1,");
                }
                if (msdc_card_is_present((msdc_port_t) param_val[1])) {
                    log_hal_info("1\r\nOK\r\n");
                    strcat((char *)resonse.response_buf, "1\r\n");
                } else {
                    log_hal_info("0\r\nOK\r\n");
                    strcat((char *)resonse.response_buf, "0\r\n");
                }

                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (9 == param_val[0]) {
                if (is_read_write_going && (0 == param_val[3])) {
                    is_read_write_going = param_val[3]; //stop read or write

                    while (is_stop_done != true) {
                        vTaskDelay(1000 / portTICK_RATE_MS); //delay and sleep
                    }
                    if ((NULL != atci_msdc_read_write_task_handle)) {
                        atci_msdc_read_write_task_deinit();
                        atci_msdc_read_write_task_handle = NULL;
                    }
                } else if (!is_read_write_going) {
                    is_write = param_val[2];
                    msdc_number = param_val[1];
                    is_read_write_going = param_val[3]; // start read or write
                    atci_msdc_read_write_task_init();
                }

                resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }
            break;
        default :
            // others are invalid command format
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif
#endif

