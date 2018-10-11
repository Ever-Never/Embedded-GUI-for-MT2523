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

#ifndef __TFTP_H__
#define __TFTP_H__

#include <stdint.h>
#include "syslog.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TFTP_PORT           (69)

/**
 * 2 bytes opcode, 2 bytes block number, and 512 bytes data.
 */
#define TFTP_BLOCK_SIZE     (4 + 512)

typedef enum
{
    TFTP_OPCODE_RRQ = 1,
    TFTP_OPCODE_WRQ,
    TFTP_OPCODE_DATA,
    TFTP_OPCODE_ACK,
    TFTP_OPCODE_ERROR,
} tftp_opcode_t;


typedef enum {
    TFTP_ERRNO_OK,
    TFTP_ERRNO_FILE_NO_FOUND,
    TFTP_ERRNO_ACCESS_VIOLATION,
    TFTP_ERRNO_DISK_FULL_OR_ALLOCATION_EXCEEDED,
    TFTP_ERRNO_ILLEGAL_TFTP_OPERATION,
    TFTP_ERRNO_UNKNOWN_TRANSFER_ID,
    TFTP_ERRNO_FILE_ALREADY_EXISTS,
    TFTP_ERRNO_NO_SUCH_USER
} tftp_error_code_t;


typedef enum {
    TFTP_STATUS_SEND_ERROR, // sending failed
    TFTP_STATUS_SEND_DONE,  // sending done
    TFTP_STATUS_RCVD_ERROR, // receiving failed
    TFTP_STATUS_RPKT_ERROR, // received ERROR packet
    TFTP_STATUS_PROT_ERROR, // received unexpected opcode, sent error pkt
    TFTP_STATUS_RCVD_DONE,  // receive completed
    TFTP_STATUS_RECV_MORE   // more data to be received
} tftp_status_t;

typedef struct tftp_pkt_s {
    uint16_t    opcode;
} tftp_pkt_t;


typedef struct tftp_ack_pkt_s {
    uint16_t    opcode;
    uint16_t    block;
} tftp_ack_pkt_t;


typedef struct tftp_error_pkt_s {
    uint16_t    opcode;
    uint16_t    error_code;
} tftp_error_pkt_t;

#ifndef MTK_DEBUG_LEVEL_NONE
#define TFTP_DEBUG 1
#else
#define TFTP_DEBUG 0
#endif

#if TFTP_DEBUG
#define TFTP_FUNC_IN   LOG_I(tftp, "enter: %s %d\n", __FUNCTION__, __LINE__)
#define TFTP_FUNC_OUT   LOG_I(tftp, "exit: %s %d\n", __FUNCTION__, __LINE__)
#else
#define TFTP_FUNC_IN
#define TFTP_FUNC_OUT
#endif

/**
 * Create a RRQ packet and send it out.
 *
 * 1. Prepare read request.
 * 2. Send read request.
 * 3. Wait data block / error.
 * 4. if data block, send ack, if error then done.
 * 5. if data block is less than 512 bytes, done.
 *
 * @retval TFTP_STATUS_SEND_ERROR sending failed
 * @retval TFTP_STATUS_SEND_DONE  sending done
 */
tftp_status_t tftp_rrq(void *sock, uint8_t *buf, const char *filename);


/**
 * Receive a packet and process it according to its content.
 *
 * @retval TFTP_STATUS_RCVD_ERROR receiving failed
 * @retval TFTP_STATUS_RPKT_ERROR received ERROR packet
 * @retval TFTP_STATUS_PROT_ERROR received unexpected opcode, sent error pkt
 * @retval TFTP_STATUS_SEND_ERROR send ack packet error
 * @retval TFTP_STATUS_RCVD_DONE  receive completed
 * @retval TFTP_STATUS_RECV_MORE  more data to be received
 */
tftp_status_t tftp_rrq_recv(
    void                *sock,
    uint8_t             *buf,
    tftp_error_code_t   *err,
    uint16_t            *len,
    uint16_t            *block);


/** create and send an ACK packet. */
tftp_status_t tftp_ack(void *sock, uint16_t block);


/** create and send an ERROR packet. */
tftp_status_t tftp_error(void *sock, uint8_t *buf, tftp_error_code_t err);


#ifdef __cplusplus
}
#endif


#endif /* __TFTP_H__ */

