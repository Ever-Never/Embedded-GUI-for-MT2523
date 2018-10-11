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
// System head file

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"

#include "hal_gpio.h"
#include "hal_gpt.h"
#include <stdlib.h>

#ifdef HAL_GPIO_MODULE_ENABLED
/*
 * sample code
 * AT+EGPIO=GPIO_GET
 * AT+EGPIO=GPIO_SET:<pin number>:<mode><dir><pull><output>
 * AT+EGPIO=GPIO_SET_MODE:<pin number>:<mode>
 * ... ...
*/


/*--- Function ---*/
atci_status_t atci_cmd_hdlr_gpio(atci_parse_cmd_param_t *parse_cmd);


#define GPIO_MODE0_ADD 0xA2020C00
#define GPIO_PULLEN0_ADD 0xA2020100
#define GPIO_PULLSEL0_ADD 0xA2020500
#define GPIO_PUPD0_ADD 0xA2020A00
#define GPIO_R0_ADD 0xA2020B00
#define GPIO_R1_ADD 0xA2020B20

typedef struct {
    uint8_t pupd;
    uint8_t r0;
    uint8_t r1;
} pull_state_t;

uint32_t if_pin_with_pullsel(hal_gpio_pin_t gpio_pin)
{
    uint32_t i;
    uint32_t pin_number_with_pullsel[5] = {0, 1, 2, 3, 10}; /* pin number which has only one pull-up resister and one pull-down resister, it is different on different chips */

    for (i = 0; i < 5; i++) {
        if (pin_number_with_pullsel[i] == gpio_pin) {
            return 1;
        }
    }
    return 0;
}


hal_gpio_status_t hal_gpio_get_pinmux(hal_gpio_pin_t pin_number, uint8_t *function_index)
{
    uint32_t no;
    uint32_t remainder;
    uint32_t temp;

    /* get the register number corresponding to the pin as one register can control 8 pins*/
    no = pin_number / 8;

    /* get the bit offset within the register as one register can control 8 pins*/
    remainder = pin_number % 8;
    temp = *(volatile unsigned int *)( GPIO_MODE0_ADD + (no * 0x10) );

    temp &= (0xF << ( 4 * remainder ));
    *function_index = (temp >> ( 4 * remainder ));
    return HAL_GPIO_STATUS_OK;
}


hal_gpio_status_t hal_gpio_get_pull(hal_gpio_pin_t pin_number, pull_state_t *pull_state)
{
    uint8_t no;
    uint8_t remainder;
    uint32_t temp1, temp2, temp3;

    /* get the register number corresponding to the pin as one register can control 8 pins*/
    no = pin_number / 32;

    /* get the bit offset within the register as one register can control 8 pins*/
    remainder = pin_number % 32;

    if (if_pin_with_pullsel(pin_number)) {
        temp1 = *(volatile unsigned int *)(GPIO_PULLEN0_ADD + (0x10 * no));
        temp1 &= (0x1 << remainder);
        temp1 = temp1 >> remainder;
        pull_state->pupd = temp1;

        temp2 = *(volatile unsigned int *)(GPIO_PULLSEL0_ADD + (0x10 * no));
        temp2 &= (0x1 << remainder);
        temp2 = temp2 >> remainder;
        pull_state->r0 = temp2;

    } else {
        temp1 = *(volatile unsigned int *)(GPIO_PUPD0_ADD + (0x10 * no));
        temp1 &= (0x1 << remainder);
        temp1 = temp1 >> remainder;
        pull_state->pupd = temp1;

        temp2 = *(volatile unsigned int *)(GPIO_R0_ADD + (0x10 * no));
        temp2 &= (0x1 << remainder);
        temp2 = temp2 >> remainder;
        pull_state->r0 = temp2;

        temp3 = *(volatile unsigned int *)(GPIO_R1_ADD + (0x10 * no));
        temp3 &= (0x1 << remainder);
        temp3 = temp3 >> remainder;
        pull_state->r1 = temp3;

    }
    return HAL_GPIO_STATUS_OK;
}


/* AT command handler  */
atci_status_t atci_cmd_hdlr_gpio(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};

    char *param1 = NULL;
    char *param2 = NULL;
    char *param3 = NULL;
    hal_gpio_data_t input_gpio_data = HAL_GPIO_DATA_LOW;
    hal_gpio_data_t output_gpio_data = HAL_GPIO_DATA_LOW;
    hal_gpio_direction_t gpio_dir;
    hal_gpio_pin_t gpio_number;
    uint8_t i, gpio_function_index;
    pull_state_t pull_state;
    uint8_t config_index[7];
    uint32_t printf_count, i_count;
    uint32_t group_number;

    printf("atci_cmd_hdlr_gpio \r\n");

    resonse.response_flag = 0; /*    Command Execute Finish.  */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            strcpy((char *)resonse.response_buf, "+EGPIO=(\"GPIO_GET: get all GPIO information\",\"GPIO_SET: set several configuration to one pin\",\"GPIO_SET_MODE: set mode to one pin\",\"GPIO_SET_DIR: set direction to one pin\",\"GPIO_SET_PULL: set pull to one pin\",\"GPIO_SET_OD: set output data to one pin\")\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION:
            strcpy((char *)resonse.response_buf, "OK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            group_number = (41 * 49 ) / ATCI_UART_TX_FIFO_BUFFER_SIZE + 1;
            printf_count = 49 / group_number;
			if (strncmp(parse_cmd->string_ptr, "AT+EGPIO=GPIO_GET:", strlen("AT+EGPIO=GPIO_GET:")) == 0) {
				gpio_number = (hal_gpio_pin_t)atoi(parse_cmd->string_ptr+strlen("AT+EGPIO=GPIO_GET:"));

                hal_gpio_get_pinmux((hal_gpio_pin_t)gpio_number, &gpio_function_index);
                hal_gpio_get_direction((hal_gpio_pin_t)gpio_number, &gpio_dir);
                if (HAL_GPIO_DIRECTION_OUTPUT == gpio_dir) {
					hal_gpio_get_output((hal_gpio_pin_t)gpio_number, &output_gpio_data);
                } else {
                	hal_gpio_get_input((hal_gpio_pin_t)gpio_number, &input_gpio_data);
                }
                hal_gpio_get_pull((hal_gpio_pin_t)gpio_number, &pull_state);
				sprintf((char *)resonse.response_buf, "+EGPIO:");
                resonse.response_len = strlen((char *)resonse.response_buf);

                if (if_pin_with_pullsel((hal_gpio_pin_t)gpio_number)) {
					sprintf(((char *)(resonse.response_buf + resonse.response_len)), "%d %d %d%d* %d %d\r\n", 
						gpio_function_index, gpio_dir, pull_state.pupd, pull_state.r0, output_gpio_data, input_gpio_data);
                } else {
                	sprintf(((char *)(resonse.response_buf + resonse.response_len)), "%d %d %d%d%d %d %d\r\n", 
						gpio_function_index, gpio_dir, pull_state.pupd, pull_state.r1, pull_state.r0, output_gpio_data, input_gpio_data);
                }
                resonse.response_len = strlen((char *)resonse.response_buf);

                resonse.response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);


                break;
            }
            if (strncmp(parse_cmd->string_ptr, "AT+EGPIO=GPIO_GET", strlen("AT+EGPIO=GPIO_GET")) == 0) {
                sprintf((char *)resonse.response_buf, "CMD: GPIO_GET\r\nPIN: [mode] [dir] [pull] [output] [input]\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
                atci_send_response(&resonse);
                i = 0;
                for (i_count = 0; i_count <= group_number; i_count++) {
                    resonse.response_len = 0;

                    for (; (i < printf_count * (i_count + 1)) && (i < 49); i ++) {
                        hal_gpio_get_pinmux((hal_gpio_pin_t)i, &gpio_function_index);
                        hal_gpio_get_direction((hal_gpio_pin_t)i, &gpio_dir);
                        if (HAL_GPIO_DIRECTION_OUTPUT == gpio_dir) {
                            hal_gpio_get_output((hal_gpio_pin_t)i, &output_gpio_data);
                        } else {
                            hal_gpio_get_input((hal_gpio_pin_t)i, &input_gpio_data);
                        }

                        hal_gpio_get_pull((hal_gpio_pin_t)i, &pull_state);

                        if (if_pin_with_pullsel((hal_gpio_pin_t)i)) {
                            sprintf(((char *)(resonse.response_buf + ((i - printf_count * i_count) * 41))), "%-7d  %-3d   %-3d  %d%d*      %-7d  %d\r\n", i, gpio_function_index, gpio_dir, pull_state.pupd, pull_state.r0, output_gpio_data, input_gpio_data);
                        } else {
                            sprintf(((char *)(resonse.response_buf + ((i - printf_count * i_count) * 41))), "%-7d  %-3d   %-3d  %d%d%d      %-7d  %d\r\n", i, gpio_function_index, gpio_dir, pull_state.pupd, pull_state.r1, pull_state.r0, output_gpio_data, input_gpio_data);
                        }
                        resonse.response_len += 41;
                    }

                    resonse.response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&resonse);
                }

                strcpy((char *)resonse.response_buf, "pull:PUPD R1 R0 or pull:PULLEN PULLSEL *  \r\ndone\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);

                break;
            }

            param1 = strtok(parse_cmd->string_ptr, ":");
            param2 = strtok(NULL, ":");
            param3 = strtok(NULL, ":");
            gpio_number = (hal_gpio_pin_t)atoi(param2);

            /*  set GPIO mode. */
            if (strncmp(param1, "AT+EGPIO=GPIO_SET_MODE", strlen("AT+EGPIO=GPIO_SET_MODE")) == 0) {
                config_index[0] = param3[0] - 48;
                if ((gpio_number > 48) || (config_index[0] > 15)) {
                    strcpy((char *)resonse.response_buf, "ERROR\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }
                hal_pinmux_set_function(gpio_number, config_index[0]);
                strcpy((char *)resonse.response_buf, "mode configure done\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }
            /*  set GPIO direction. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET_DIR", strlen("AT+EGPIO=GPIO_SET_DIR")) == 0) {
                config_index[0] = param3[0] - 48;

                if ((gpio_number > 48) || (config_index[0] > 1)) {
                    strcpy((char *)resonse.response_buf, "ERROR\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }

                hal_gpio_set_direction(gpio_number, (hal_gpio_direction_t)config_index[0]);
                strcpy((char *)resonse.response_buf, " dir configure done\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }
            /*  set GPIO pull state. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET_PULL", strlen("AT+EGPIO=GPIO_SET_PULL")) == 0) {
                config_index[0] = param3[0] - 48; //  pupd
                config_index[1] = param3[1] - 48; //  r0
                config_index[2] = param3[2] - 48; //  r1

                if ((gpio_number > 48) || (config_index[0] > 1) || (config_index[1] > 1)) {
                    strcpy((char *)resonse.response_buf, "ERROR\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }

                if (if_pin_with_pullsel(gpio_number)) {
                    /*  pupd:r0:r1:
                      0**: disable pull
                      10*: pull  down
                      11*: pull up
                     */
                    if (config_index[0] == 0) { /*disabl pull*/
                        hal_gpio_disable_pull(gpio_number);
                    } else if (1 == config_index[1]) {
                        hal_gpio_pull_up(gpio_number);
                    } else if (0 == config_index[1]) {
                        hal_gpio_pull_down(gpio_number);
                    }
                } else {
                    if (config_index[2] > 1) {
                        strcpy((char *)resonse.response_buf, "ERROR\r\n");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        atci_send_response(&resonse);
                        break;
                    }
                    hal_gpio_set_pupd_register(gpio_number, config_index[0], config_index[1], config_index[2]);
                }
                strcpy((char *)resonse.response_buf, "configure done\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }

            /*  set GPIO output data. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET_OD", strlen("AT+EGPIO=GPIO_SET_OD")) == 0) {
                config_index[0] = param3[0] - 48; //  NEED TO CHANGE
                if ((gpio_number > 48) || (config_index[0] > 1)) {
                    strcpy((char *)resonse.response_buf, "ERROR\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }
                hal_gpio_set_output(gpio_number, (hal_gpio_data_t)config_index[0]);
                strcpy((char *)resonse.response_buf, "configure done\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }
            /* Set serveral configurations to one GPIO. */
            else if (strncmp(param1, "AT+EGPIO=GPIO_SET", strlen("AT+EGPIO=GPIO_SET")) == 0) {
                config_index[0] = param3[0] - 48; //  mode
                config_index[1] = param3[1] - 48; //  dir
                config_index[2] = param3[2] - 48; //  pupd
                config_index[3] = param3[3] - 48; //  r0
                config_index[4] = param3[4] - 48; //  r1
                config_index[5] = param3[5] - 48; //  od

                if ((gpio_number > 48) || (config_index[0] > 15) || config_index[1] > 1 || config_index[2] > 2 || config_index[3] > 1 || config_index[4] > 1 || config_index[5] > 1) {
                    strcpy((char *)resonse.response_buf, "ERROR\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }
                /* set pinmux */
                hal_pinmux_set_function(gpio_number, config_index[0]);

                /*set direction */
                hal_gpio_set_direction(gpio_number, (hal_gpio_direction_t)config_index[1]);

                /*set pull state */
                if (if_pin_with_pullsel(gpio_number)) {
                    if (config_index[2] == 0) { /*disabl pull*/
                        hal_gpio_disable_pull(gpio_number);
                    } else if (1 == config_index[3]) {
                        hal_gpio_pull_up(gpio_number);
                    } else if (0 == config_index[3]) {
                        hal_gpio_pull_down(gpio_number);
                    }
                } else {
                    hal_gpio_set_pupd_register(gpio_number, config_index[2], config_index[3], config_index[4]);
                }

                /*set output data */
                hal_gpio_set_output(gpio_number, (hal_gpio_data_t)config_index[5]);
                strcpy((char *)resonse.response_buf, "configure done\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }

        default :
            /* others are invalid command format */
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif

