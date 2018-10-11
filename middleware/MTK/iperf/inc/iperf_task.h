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

#ifndef __IPERF_TASK_H__
#define __IPERF_TASK_H__

// Includes --------------------------------------------------------------------
#include <stdint.h> //uint8_t and friends
#include "FreeRTOS.h"
//#include "timer.h"
#include "lwip/api.h" //netconn API
#include "lwip/sockets.h" //socket API


// Private typedef -------------------------------------------------------------
typedef struct count_s {
    uint64_t Bytes;
    unsigned KBytes;
    unsigned MBytes;
    unsigned GBytes;
    unsigned times;
} count_t;

// used to reference the 4 byte ID number we place in UDP datagrams
// use int32_t if possible, otherwise a 32 bit bitfield (e.g. on J90)
typedef struct UDP_datagram {
    int32_t id;
    unsigned int tv_sec;
    unsigned int tv_usec;
} UDP_datagram;

/*
 * The client_hdr structure is sent from clients
 * to servers to alert them of things that need
 * to happen. Order must be perserved in all
 * future releases for backward compatibility.
 * 1.7 has flags, num_threads, port, and buffer_len
 */
typedef struct client_hdr {
    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no
     * information bits are set then the header is ignored.
     * The lowest order diferentiates between dualtest and
     * tradeoff modes, wheither the speaker needs to start
     * immediately or after the audience finishes.
     */
    int32_t flags;
    int32_t num_threads;
    int32_t port;
    int32_t buffer_len;
    int32_t win_band;
    int32_t amount;
} client_hdr;

/*
 * The server_hdr structure facilitates the server
 * report of jitter and loss on the client side.
 * It piggy_backs on the existing clear to close
 * packet.
 */
typedef struct server_hdr {
    /*
     * flags is a bitmap for different options
     * the most significant bits are for determining
     * which information is available. So 1.7 uses
     * 0x80000000 and the next time information is added
     * the 1.7 bit will be set and 0x40000000 will be
     * set signifying additional information. If no
     * information bits are set then the header is ignored.
     */
    int32_t flags;
    int32_t total_len1;
    int32_t total_len2;
    int32_t stop_sec;
    int32_t stop_usec;
    int32_t error_cnt;
    int32_t outorder_cnt;
    int32_t datagrams;
    int32_t jitter1;
    int32_t jitter2;
} server_hdr;

typedef struct iperf_result_struct {
    int32_t data_size;
    int32_t send_time;
    char total_len[20];
    char result[20];
    char report_title[20];
} iperf_result_t;


// Private macro ---------------------------------------------------------------
#define IPERF_DEFAULT_PORT  5001 //Port to listen
#define IPERF_USE_PBUF      1    //Set to nonzero to use pbuf (faster) instead of netbuf (safer)

#define IPERF_HEADER_VERSION1 0x80000000
#define IPERF_DEFAULT_UDP_RATE (1024 * 1024)
#define IPERF_TEST_BUFFER_SIZE (1460)
#define IPERF_COMMAND_BUFFER_NUM (18)
#define IPERF_COMMAND_BUFFER_SIZE (20) // 4 bytes align

#define DBGPRINT_IPERF(FEATURE, _Fmt)            \
        {                                        \
            if (g_iperf_debug_feature & FEATURE) \
            {                                    \
                printf _Fmt;                     \
            }                                    \
        }

#define IPERF_DEBUG_RECEIVE     (1<<0)
#define IPERF_DEBUG_SEND        (1<<1)
#define IPERF_DEBUG_REPORT      (1<<2)

typedef void (* iperf_callback_t)(iperf_result_t* iperf_result);


// Private function prototypes -------------------------------------------------
void iperf_udp_run_server(char *parameters[]);
void iperf_tcp_run_server(char *parameters[]);
void iperf_udp_run_client(char *parameters[]);
void iperf_tcp_run_client(char *parameters[]);
void iperf_set_debug_mode(uint32_t debug);
void iperf_register_callback(iperf_callback_t callback);
int iperf_format_transform(char *param);

#endif /* __IPERF_TASK_H__ */
