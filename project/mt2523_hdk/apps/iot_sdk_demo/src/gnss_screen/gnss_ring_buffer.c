/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation (MediaTek Software) are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. (MediaTek) and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek (License Agreement) and been granted explicit permission to do so within
 * the License Agreement (Permitted User).  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN AS-IS BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
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



#include "ring_buf.h"
#include <string.h>

// this implementation can not be used by multi-task. if you want, you need use mutex.
// but this implementation can be read by one task, and write by the other task.

void ring_buf_init(ring_buf_struct_t *ring_buf, int8_t* buf, int16_t buf_size)
{
    ring_buf->buf_size = buf_size;
    ring_buf->buf = buf;
    ring_buf->start_ind = ring_buf->end_ind = 0;
}


int32_t is_empty(ring_buf_struct_t *ring_buf)
{
    return ring_buf->end_ind == ring_buf->start_ind;
}


int32_t put_data(ring_buf_struct_t *ring_buf, int8_t* buf, int32_t buf_len)
{
    if (buf_len + ring_buf->end_ind >= ring_buf->buf_size){
        memcpy(ring_buf->buf + ring_buf->end_ind, buf, ring_buf->buf_size - ring_buf->end_ind);
        memcpy(ring_buf->buf, buf + ring_buf->buf_size - ring_buf->end_ind, buf_len - (ring_buf->buf_size - ring_buf->end_ind));
        ring_buf->end_ind = ring_buf->end_ind + buf_len - ring_buf->buf_size;
    } else {
        memcpy(ring_buf->buf + ring_buf->end_ind, buf, buf_len);
        ring_buf->end_ind += buf_len;
    }
    return buf_len;
}

int32_t consume_data(ring_buf_struct_t *ring_buf, int8_t* buf, int32_t buf_len)
{
    int32_t consume_len;
    int32_t end_ind  = ring_buf->end_ind;
    if (end_ind == ring_buf->start_ind) {
        return 0;
    }

    if (ring_buf->start_ind < end_ind) {
        if (end_ind - ring_buf->start_ind > buf_len) {
            consume_len = buf_len;
            memcpy(buf, ring_buf->buf, buf_len);
            ring_buf->start_ind += buf_len;
        } else {
            consume_len = end_ind - ring_buf->start_ind;
            memcpy(buf, ring_buf->buf, consume_len);
            ring_buf->start_ind = end_ind;
        }
    } else {
        if (ring_buf->buf_size - ring_buf->start_ind > buf_len) {
            consume_len = buf_len;
            memcpy(buf, ring_buf->buf, buf_len);
            ring_buf->start_ind += buf_len;
        } else {
            int32_t copyed_len = ring_buf->buf_size - ring_buf->start_ind;
            memcpy(buf, ring_buf->buf + ring_buf->start_ind, copyed_len);
            ring_buf->start_ind = 0;
            if (end_ind > buf_len - copyed_len) {
                consume_len = buf_len;
                memcpy(buf + copyed_len, ring_buf->buf, buf_len - copyed_len);
                ring_buf->start_ind += buf_len;
            } else {
                consume_len = end_ind;
                memcpy(buf + copyed_len, ring_buf->buf, end_ind);
                ring_buf->start_ind = end_ind;
            }
        }
    }
    return consume_len;
}
