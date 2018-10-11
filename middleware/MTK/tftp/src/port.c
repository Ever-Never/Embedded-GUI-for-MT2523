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
//#include <unistd.h>
#include <errno.h>

#include "port.h"

//#define I_O_DEBUG

#ifdef I_O_DEBUG
#define I   printf("enter: %s %d\n", __FUNCTION__, __LINE__)
#define O   printf("exit: %s %d\n", __FUNCTION__, __LINE__)
#else
#define I
#define O
#endif

#define SA_TYPE struct sockaddr
#define IN_TYPE struct sockaddr_in

/****************************************************************************
 *
 * Static Functions.
 *
 ****************************************************************************/


static int32_t to_saddr(IN_TYPE *saddr, const char *addr, unsigned short port)
{
    I;

    os_memset(saddr, 0, sizeof(*saddr));

    /* family */
    saddr->sin_family = AF_INET;

    /* IP address */
    if (addr) {
        if (inet_aton(addr, &saddr->sin_addr) == 0) {
            printf("%s %s\n", addr, strerror(errno));
            O;
            return -1;
        }
    } else {
        saddr->sin_addr.s_addr = INADDR_ANY;
    }

    /* port */
    saddr->sin_port = htons(port);

    O;
    return 0;
}


/****************************************************************************
 *
 * API Functions.
 *
 ****************************************************************************/


/**
 * convert local and remote addresses.
 * create udp socket.
 * bind udp socket.
 */
void *tftp_sock_open(const char      *raddr,
                     unsigned short  rport,
                     unsigned short  lport)
{
    tftp_sock_t *tsock = NULL;
    IN_TYPE     sa_laddr;

    I;

    tsock = os_malloc(sizeof(*tsock));
    if (!tsock) {
        goto tftp_sock_open_err;
    }

    os_memset(tsock, 0, sizeof(*tsock));

    if ((to_saddr(&tsock->sa_raddr, raddr, rport) < 0) ||
        (to_saddr(&sa_laddr,       NULL,  lport) < 0)) {
        goto tftp_sock_open_err;
    }

    tsock->s = socket(AF_INET, SOCK_DGRAM, 0);

    if (tsock->s < 0) {
        printf("%s %s\n", __FUNCTION__, strerror(errno));
        goto tftp_sock_open_err;
    }

    if (bind(tsock->s, (SA_TYPE *)&sa_laddr, sizeof(SA_TYPE)) < 0)
    {
        printf("%s:%u %s %d %d\n", __FUNCTION__, __LINE__, strerror(errno), tsock->s, lport);
        goto tftp_sock_open_err;
    }

    goto tftp_sock_open_end;

tftp_sock_open_err:
	if (tsock)
	{
		tftp_sock_close(tsock);
	    tsock = NULL;
	}    

tftp_sock_open_end:
    O;
    return tsock;
}


void tftp_sock_close(void *sock)
{
    I;
	if (!sock)
	{
		return;
	}
	
    tftp_sock_t *tsock = (tftp_sock_t *)sock;
    close(tsock->s);
    os_free(tsock);
    O;
}


int16_t tftp_sock_send(void *sock, const uint8_t *buf, uint16_t len)
{
    tftp_sock_t *tsock = sock;

    I;

    if (sendto(tsock->s, buf, len, 0, (const SA_TYPE *)&tsock->sa_raddr, sizeof(tsock->sa_raddr)) < 0) {
        printf("%s %s\n", __FUNCTION__, strerror(errno));

        O;

        return -1;
    }

    O;

    return len;
}


int16_t tftp_sock_recv(void *sock, uint8_t *buf, uint16_t len)
{
    int     size;
    tftp_sock_t *tsock = sock;
    socklen_t   sock_len = sizeof(tsock->sa_raddr);

    I;

    size = recvfrom(tsock->s, buf, len, 0,
                    (SA_TYPE *)&tsock->sa_raddr, &sock_len);

    O;

    return (int16_t)size;
}
