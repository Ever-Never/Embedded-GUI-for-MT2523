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


#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#ifndef MTK_EXTERNAL_PMIC
#if (PRODUCT_VERSION == 2523 || PRODUCT_VERSION == 2533 )
#include "hal_pmu_internal.h"
#endif
#include "hal_pmu.h"
/*
 * sample code
*/


/*--- Function ---*/

atci_status_t atci_cmd_hdlr_vibrator(atci_parse_cmd_param_t *parse_cmd);

/*
AT+EVIB=<op>	            |   "OK"
AT+EVIB=?                   |   "+EVIB=(0,1)","OK"


*/
/* AT command handler  */
#if (PRODUCT_VERSION == 2523 || PRODUCT_VERSION == 2533 )

atci_status_t atci_cmd_hdlr_vibrator(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    char *param = NULL;
    char param_val;

    printf("atci_cmd_hdlr_vibrator \r\n");

    resonse.response_flag = 0; /*    Command Execute Finish.  */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    /* rec: AT+EVIB=?   */
            strcpy((char *)resonse.response_buf, "+EVIB=(\"0: turn off\",\"1: turn on 2.8V\",\"2: turn on 3.0V\",\"3: turn on 3.3V\")\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+ELED=<op>  the handler need to parse the parameters  */
            param = strtok(parse_cmd->string_ptr, ",\n\r");
            param = strtok(parse_cmd->string_ptr, "AT+EVIB=");
            param_val = atoi(param);


            if ((0 != param_val) && (1 != param_val) && (2 != param_val) && (3 != param_val)) {
                strcpy((char *)resonse.response_buf, "ERROR\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }

            if (0 == param_val) {

		  PMIC_VR_CONTROL(PMIC_VIBR, PMIC_VR_CTL_DISABLE);
				
            } else if (1 == param_val) {
                      
                     PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VIBR, PMIC_LDO_2P8V);
                     PMIC_VR_CONTROL(PMIC_VIBR, PMIC_VR_CTL_ENABLE);
               
            }
	#ifdef MTK_BUILD_SMT_LOAD
           else if (2 == param_val) {
                      
                     PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VIBR, PMIC_LDO_3P0V);
                     PMIC_VR_CONTROL(PMIC_VIBR, PMIC_VR_CTL_ENABLE);
               
            } else {
                      
                     PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VIBR, PMIC_LDO_3P3V);
                     PMIC_VR_CONTROL(PMIC_VIBR, PMIC_VR_CTL_ENABLE);
               
            }
         #else
             else{
                 ;
	 }			 
        #endif
            strcpy((char *)resonse.response_buf, "OK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        default :
            /* others are invalid command format */
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}
#else
 atci_status_t atci_cmd_hdlr_vibrator(atci_parse_cmd_param_t *parse_cmd)
{
        /*the chip does not support vibrator*/
	return ATCI_STATUS_ERROR;
}
#endif

#endif

