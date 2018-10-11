/*
 * Copyright (c) 2007-2009 Fr�d�ric Bernon, Simon Goldschmidt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Fr�d�ric Bernon, Simon Goldschmidt
 *
 */
#ifndef LWIP_SNTP_H
#define LWIP_SNTP_H

#include "lwip/opt.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SNTP_MAX_SERVERS 5
/** The maximum number of SNTP servers that can be set */
#ifndef SNTP_MAX_SERVERS
#define SNTP_MAX_SERVERS           LWIP_DHCP_MAX_NTP_SERVERS
#endif

/** Set this to 1 to implement the callback function called by dhcp when
 * NTP servers are received. */
#ifndef SNTP_GET_SERVERS_FROM_DHCP
#define SNTP_GET_SERVERS_FROM_DHCP LWIP_DHCP_GET_NTP_SRV
#endif

/* Set this to 1 to support DNS names (or IP address strings) to set sntp servers */
#ifndef SNTP_SERVER_DNS
#define SNTP_SERVER_DNS            1
#endif

/** One server address/name can be defined as default if SNTP_SERVER_DNS == 1:*/
//#define SNTP_SERVER_ADDRESS "pool.ntp.org"
 

extern void sntp_init(void);
extern void sntp_stop(void);

extern void sntp_setserver(u8_t idx, const ip_addr_t *addr);
extern ip_addr_t sntp_getserver(u8_t idx);

#if SNTP_SERVER_DNS
extern void sntp_setservername(u8_t idx, char *server);
extern char *sntp_getservername(u8_t idx);
#endif /* SNTP_SERVER_DNS */

#if SNTP_GET_SERVERS_FROM_DHCP
extern void sntp_servermode_dhcp(int set_servers_from_dhcp);
#else /* SNTP_GET_SERVERS_FROM_DHCP */
#define sntp_servermode_dhcp(x)
#endif /* SNTP_GET_SERVERS_FROM_DHCP */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_SNTP_H */
