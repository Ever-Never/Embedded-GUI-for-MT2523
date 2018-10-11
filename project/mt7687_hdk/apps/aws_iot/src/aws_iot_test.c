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
#include "sample_common_header.h"
#include "aws_iot_test_integration_common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "syslog.h"
#include "wifi_lwip_helper.h"
#include "task_def.h"

//#define TEST

int already_started = 0;
const char *aws_iot_task_name = "AWS_IOT";
static const int AWS_IOT_TASK_STACK_SIZE = 1024 * 10;

void aws_iot_testing_task(void *param)
{
#ifdef TEST
    int basic_conn_ret = 0;
    int auto_reconnect_ret = 0;
    int multiple_clients_ret = 0;
    int multi_thread_validation_ret = 0;
    int shadow_sample_console_ret = 0;
    int sub_pub_lib_sample_ret = 0;
    int sub_pub_sample = 0;
#endif
    int shadow_sample_ret = 0;

    printf(".....AWS IoT Test Task Running.....");
    
#ifdef TEST

    printf("\n=========================================================\n");
    printf( "= Start To Run Basic Connectivity\n");
    printf( "=========================================================\n");
    basic_conn_ret = aws_iot_mqtt_tests_basic_connectivity();
    printf( "\n---------------------------------------------------------\n");
    printf( "- Basic Connectivity Result = %d\n", basic_conn_ret);
    printf( "---------------------------------------------------------\n");

    vTaskDelay(1000);
    printf("\n=========================================================\n");
    printf( "= Start To Run Auto Reconnect\n");
    printf( "=========================================================\n");
    auto_reconnect_ret = aws_iot_mqtt_tests_auto_reconnect();
    printf( "\n---------------------------------------------------------\n");
    printf( "- Auto Reconnect Result = %d\n", auto_reconnect_ret);
    printf( "---------------------------------------------------------\n");

    vTaskDelay(1000);

    printf("\n=========================================================\n");
    printf( "= Start To Run Multi Clients\n");
    printf( "=========================================================\n");
    multiple_clients_ret = aws_iot_mqtt_tests_multiple_clients();
    printf( "\n---------------------------------------------------------\n");
    printf( "- Multi Clients Result = %d\n", multiple_clients_ret);
    printf( "---------------------------------------------------------\n");

    vTaskDelay(1000);

    printf("\n=========================================================\n");
    printf( "= Start To Run Multi Threading Validation\n");
    printf( "=========================================================\n");
    multi_thread_validation_ret = aws_iot_mqtt_tests_multi_threading_validation();
    printf( "\n---------------------------------------------------------\n");
    printf( "- Multi Threading Validation Result = %d\n", multi_thread_validation_ret);
    printf( "---------------------------------------------------------\n");
#endif
    vTaskDelay(1000);

    printf("\n=========================================================\n");
    printf( "= Start To Run Shadow Sample\n");
    printf( "=========================================================\n");
    shadow_sample_ret = aws_iot_mqtt_tests_basic_connectivity();//shadow_sample_main();
    printf( "\n---------------------------------------------------------\n");
    printf( "- Shadow Sample Result = %d\n", shadow_sample_ret);
    printf( "---------------------------------------------------------\n");
    
#ifdef TEST

    vTaskDelay(1000);

    printf( "\n=========================================================\n");
    printf( "= Start To Run Shadow Sample Console Echo\n");
    printf( "=========================================================\n");
    shadow_sample_console_ret = shadow_sample_console_echo_main();
    printf( "\n---------------------------------------------------------\n");
    printf( "- Shadow Sample Console Echo Result = %d\n", shadow_sample_console_ret);
    printf( "---------------------------------------------------------\n");

    vTaskDelay(1000);

    printf( "\n=========================================================\n");
    printf( "- Start To Run Subscrible Publish Library Sample\n");
    printf( "=========================================================\n");
    sub_pub_lib_sample_ret = subscribe_publish_library_sample_main();
    printf( "\n---------------------------------------------------------\n");
    printf( "- Subscrible Publish Library Sample Result = %d\n", sub_pub_lib_sample_ret);
    printf( "---------------------------------------------------------\n");

    vTaskDelay(1000);

    printf( "\n=========================================================\n");
    printf( "= Start To Run Subscribe Publish Sample\n");
    printf( "=========================================================\n");
    sub_pub_sample = subscribe_publish_sample_main();
    printf( "\n---------------------------------------------------------\n");
    printf( "- Subscribe Publish Sample Result = %d\n", sub_pub_sample);
    printf( "---------------------------------------------------------\n");


    printf("*********************************************************************\n");
    printf("* Sample Result:\n");
    printf("* Basic Connectivity Result             : %s\n", (basic_conn_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Auto Reconnect Result                 : %s\n", (auto_reconnect_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Multi Clients Result                  : %s\n", (multiple_clients_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Multi Thread Validation Result        : %s\n", (multi_thread_validation_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Shadow Sample Result                  : %s\n", (shadow_sample_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Shadow Console Echo Result            : %s\n", (shadow_sample_console_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Subscrbe Publish Library Result       : %s\n", (sub_pub_lib_sample_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("* Subscrbe Publish Result               : %s\n", (sub_pub_sample == 0 ? "SUCCESS" : "FAILURE"));
    printf("*********************************************************************\n");
#else
    printf("*********************************************************************\n");
    printf("* Result:\n");
    printf("* shadow sample                  : %s\n", (shadow_sample_ret == 0 ? "SUCCESS" : "FAILURE"));
    printf("*********************************************************************\n");
    printf("example project test success.");
#endif
    vTaskDelay(1000);

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS); // release CPU
    }

}

void ip_ready_task(void *param)
{
    lwip_net_ready();

    vTaskDelay(1000);

    if (already_started == 0) {
        printf(".....Start AWS IoT Test Task.....\n");
        already_started = 1;
        xTaskCreate(aws_iot_testing_task, aws_iot_task_name,
                        AWS_IOT_TASK_STACK_SIZE / ((uint32_t)sizeof(StackType_t)), NULL, TASK_PRIORITY_NORMAL, NULL);
    } else {
        printf(".....AWS IoT Testing Task Already Started.....\n");
    }

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS); // release CPU
    }
}

void start_aws_iot_tests(void) 
{
    xTaskCreate(ip_ready_task, "ip_ready_task",
                        1024 / ((uint32_t)sizeof(StackType_t)), NULL, TASK_PRIORITY_NORMAL, NULL);
}

