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

#include <stdio.h>
#include <string.h>

#include "tftpc.h"
#include "port.h"


/****************************************************************************
 *
 * Static Functions.
 *
 ****************************************************************************/
static tftpc_status_t _tftpc_read_more(tftpc_t *p, uint16_t *len)
{
    tftp_status_t   ts;

    TFTP_FUNC_IN;

    ts = tftp_rrq_recv(p->sock, p->buf, &p->error_code, len, &p->block);

    if (ts == TFTP_STATUS_RCVD_ERROR || ts == TFTP_STATUS_RPKT_ERROR ||
        ts == TFTP_STATUS_PROT_ERROR || ts == TFTP_STATUS_SEND_ERROR) {
        TFTP_FUNC_OUT;
        return TFTPC_STATUS_ERR;
    }

    if (ts == TFTP_STATUS_RCVD_DONE) {
        TFTP_FUNC_OUT;
        return TFTPC_STATUS_DONE;
    }

    TFTP_FUNC_OUT;
    return TFTPC_STATUS_MORE;
}


/**
 * @retval TFTPC_STATUS_ERR read failed.
 * @retval TFTPC_STATUS_MORE more data to read.
 */
static tftpc_status_t _tftpc_read_idle(tftpc_t *p)
{
    tftp_status_t       ts;

    TFTP_FUNC_IN;

    ts = tftp_rrq(p->sock, p->buf, p->filename);

    if (ts == TFTP_STATUS_SEND_ERROR) {
        TFTP_FUNC_OUT;
        return TFTPC_STATUS_ERR;
    }

    TFTP_FUNC_OUT;
    return TFTPC_STATUS_MORE;
}


/****************************************************************************
 *
 * API Functions.
 *
 ****************************************************************************/


tftpc_t *tftpc_read_init(const char      *raddr,
                         unsigned short  rport,
                         unsigned short  lport,
                         const char      *filename)
{
    tftpc_t             *tftpc;
    tftpc_status_t      st;
    struct timeval      tv;
    tftp_sock_t         *sock_t_ptr;
    int                 set_sock_r;

    TFTP_FUNC_IN;

    if (!raddr || !filename || filename[0] == '\0') {
        tftpc = NULL;
        goto tftpc_init_end;
    }

    tftpc = (tftpc_t *)pvPortMalloc(sizeof(tftpc_t));

    if (!tftpc) {
        goto tftpc_init_end;
    }

    os_memset(tftpc, 0, sizeof(tftpc_t));

    if (lport == 0) {
        lport = TFTP_PORT;
    }

    tftpc->state    = TFTPC_STATE_IDLE;

    tftpc->sock     = tftp_sock_open(raddr, rport, lport);
    if (tftpc->sock == 0) {
        vPortFree(tftpc);
        tftpc = NULL;
        goto tftpc_init_end;
    }

    tftpc->filename = pvPortMalloc(strlen(filename) + 1);

    if (tftpc->filename == NULL) {
        tftp_sock_close(tftpc->sock);
        vPortFree(tftpc);
        tftpc = NULL;
        goto tftpc_init_end;
    }

    tv.tv_sec  = 10;
    tv.tv_usec = 0;
    sock_t_ptr = (tftp_sock_t *)tftpc->sock;
    set_sock_r = setsockopt(sock_t_ptr->s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    printf("set socket result: %d\n", set_sock_r);

    os_memcpy(tftpc->filename, filename, strlen(filename) + 1);

    st = _tftpc_read_idle(tftpc);

    if (st != TFTPC_STATUS_MORE) {
        tftp_sock_close(tftpc->sock);
        vPortFree(tftpc->filename);
        vPortFree(tftpc);
        tftpc = NULL;
    }

tftpc_init_end:
    TFTP_FUNC_OUT;
    return tftpc;
}


void tftpc_read_done(tftpc_t *tftpc)
{
    TFTP_FUNC_IN;

    if (tftpc == NULL) {
        TFTP_FUNC_OUT;
        return;
    }

    if (tftpc->filename) {
        vPortFree(tftpc->filename);
    }

    tftp_sock_close(tftpc->sock);

    vPortFree(tftpc);

    TFTP_FUNC_OUT;
}


tftpc_status_t tftpc_read_data(tftpc_t *tftpc, uint16_t *len)
{
    tftpc_status_t status;

    TFTP_FUNC_IN;

    if (!tftpc || !len) {
        TFTP_FUNC_OUT;
        return TFTPC_STATUS_ERR;
    }

    status = _tftpc_read_more(tftpc, len);

    TFTP_FUNC_OUT;

    return status;
}



