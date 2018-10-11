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

/**
 * @file
 *
 * Neighbor discovery and stateless address autoconfiguration for IPv6.
 * Aims to be compliant with RFC 4861 (Neighbor discovery) and RFC 4862
 * (Address autoconfiguration).
 */
/*
 * Copyright (c) 2010 Inico Technologies Ltd.
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
 * Author: Ivan Delamer <delamer@inicotech.com>
 *
 *
 * Please coordinate changes and requests with Ivan Delamer
 * <delamer@inicotech.com>
 */

#include "lwip/opt.h"

#if LWIP_IPV6  /* don't build if not configured for use in lwipopts.h */

#include "lwip/nd6.h"
#include "lwip/pbuf.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/inet_chksum.h"
#include "lwip/netif.h"
#include "lwip/icmp6.h"
#include "lwip/mld6.h"
#include "lwip/ip.h"
#include "lwip/stats.h"

#include <string.h>

#define REDIRECT_ENABLE  1
/* Router tables. */
struct nd6_neighbor_cache_entry neighbor_cache[LWIP_ND6_NUM_NEIGHBORS];
struct nd6_destination_cache_entry destination_cache[LWIP_ND6_NUM_DESTINATIONS];
struct nd6_prefix_list_entry prefix_list[LWIP_ND6_NUM_PREFIXES];
struct nd6_prefix_list_entry autocfg_prefix_list[LWIP_ND6_NUM_AUTOCFG_PREFIXES];
struct nd6_router_list_entry default_router_list[LWIP_ND6_NUM_ROUTERS];

/* Default values, can be updated by a RA message. */
u32_t reachable_time = LWIP_ND6_REACHABLE_TIME;
u32_t retrans_timer = LWIP_ND6_RETRANS_TIMER; /* TODO implement this value in timer */
u8_t current_hop_limit = LWIP_ICMP6_HL; /* TODO implement this value in timer */

/* Index for cache entries. */
static u8_t nd6_cached_neighbor_index;
static u8_t nd6_cached_destination_index;

/* Multicast address holder. */
static ip6_addr_t multicast_address;

/* Static buffer to parse RA packet options (size of a prefix option, biggest option) */
static u8_t nd6_ra_buffer[sizeof(struct prefix_option)];

/* Forward declarations. */
static s8_t nd6_find_neighbor_cache_entry(const ip6_addr_t * ip6addr);
static s8_t nd6_new_neighbor_cache_entry(void);
static void nd6_free_neighbor_cache_entry(s8_t i);
static s8_t nd6_find_destination_cache_entry(const ip6_addr_t * ip6addr);
static s8_t nd6_new_destination_cache_entry(void);
static s8_t nd6_is_prefix_in_netif(const ip6_addr_t * ip6addr, struct netif * netif);
static s8_t nd6_get_router(const ip6_addr_t * router_addr, struct netif * netif);
static s8_t nd6_new_router(const ip6_addr_t * router_addr, struct netif * netif);
static s8_t nd6_get_onlink_prefix(ip6_addr_t * prefix, u8_t prefix_len, struct netif * netif);
static s8_t nd6_get_autocfg_prefix(ip6_addr_t * prefix, u8_t prefix_len, struct netif * netif);
static s8_t nd6_new_onlink_prefix(ip6_addr_t * prefix, u8_t prefix_len, struct netif * netif);
static s8_t nd6_new_autocfg_prefix(ip6_addr_t * prefix, u8_t prefix_len, struct netif * netif);

#define ND6_SEND_FLAG_MULTICAST_DEST 0x01
#define ND6_SEND_FLAG_ALLNODES_DEST 0x02
static void nd6_send_ns(struct netif * netif, const ip6_addr_t * target_addr, u8_t flags);
static void nd6_send_na(struct netif * netif, const ip6_addr_t * target_addr, u8_t flags);
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
static err_t nd6_send_rs(struct netif * netif);
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */

#if LWIP_ND6_QUEUEING
static void nd6_free_q(struct nd6_q_entry *q);
#else /* LWIP_ND6_QUEUEING */
#define nd6_free_q(q) pbuf_free(q)
#endif /* LWIP_ND6_QUEUEING */
static void nd6_send_q(s8_t i);
static void nd6_disc_def_rt_list(u8_t def_rt_indx);
static s8_t nd6_find_default_router_by_neighbor(struct nd6_neighbor_cache_entry * neighbor);
static s8_t nd6_icmp_ra_prefix_info_chk(struct prefix_option *prefix_opt);
static void nd6_icmp_na_input(struct pbuf *p, struct netif *inp);
static void nd6_icmp_ns_input(struct pbuf *p, struct netif *inp);
static void nd6_icmp_ra_input(struct pbuf *p, struct netif *inp);
static void nd6_icmp_rd_input(struct pbuf *p, struct netif *inp);
static void nd6_trace_addr(char * addr);
static s8_t nd6_match_prefix(const ip6_addr_t *addr, struct nd6_prefix_list_entry *prefix);

s8_t nd6_icmp_code_chk(enum icmp6_type type, u8_t code)
{
    switch (type)
    {
        case ICMP6_TYPE_RS:
        case ICMP6_TYPE_RA:
        case ICMP6_TYPE_NS:
        case ICMP6_TYPE_NA:
        case ICMP6_TYPE_RD:
        {
            if (code)
            {
                //kal_prompt_trace(MOD_TCPIP, "Err: ND ICMP code is not 0! code:%d", code);
                return ND6_ERR_VIOLATE_PROTOCAL;
            }
            
            return ND6_ERR_OK;
        }
        
        default:
        {
            break;
        }    
    }

    return ND6_ERR_INVALID_PARAM;
}

s8_t nd6_icmp_hoplimit_chk(void)
{
    u8_t hop_limt = ip6_current_hop_limit();
    
    if (255 != hop_limt)
    {
        //kal_prompt_trace(MOD_TCPIP, "Err: ND ICMP hop limit is not 255! hop limit:%d", hop_limt);
        return ND6_ERR_VIOLATE_PROTOCAL;
    }

    return ND6_ERR_OK;
}

s8_t nd6_icmp_option_len_chk(struct pbuf *p)
{
    u8_t *buffer, opt_len = 0;
    u16_t offset;
    u8_t msg_type = *((u8_t *)p->payload);

    switch (msg_type) 
    {
        case ICMP6_TYPE_NS:
        {
            offset = sizeof(struct ns_header);
            break;
        }            
        case ICMP6_TYPE_NA:
        {
            offset = sizeof(struct na_header);
            break;
        }    
        case ICMP6_TYPE_RS: 
        {
            offset = sizeof(struct rs_header);
            break;
        } 
        case ICMP6_TYPE_RA:
        {
            offset = sizeof(struct ra_header);
            break;
        } 
        case ICMP6_TYPE_RD:
        {
            offset = sizeof(struct redirect_header);
            break;
        }
        default:
        {
            return ND6_ERR_INVALID_PARAM;
        }
    }
    
    while ((p->tot_len - offset) > 0) 
    {
      /* Extract one option once a time. */  
        if (p->len == p->tot_len) 
        {
            /* no need to copy from contiguous pbuf */
            buffer = &((u8_t*)p->payload)[offset];
        } 
        else 
        {
            buffer = nd6_ra_buffer;
            pbuf_copy_partial(p, buffer, 2, offset);
        }
        
        #if 0
        switch (buffer[0]) 
        {
            case ND6_OPTION_TYPE_SOURCE_LLADDR:
            case ND6_OPTION_TYPE_TARGET_LLADDR:
            case ND6_OPTION_TYPE_MTU:      
            case ND6_OPTION_TYPE_PREFIX_INFO:      
            case ND6_OPTION_TYPE_REDIR_HDR:
            {
                opt_len = buffer[1];
                if (!opt_len)
                {
                    //kal_prompt_trace(MOD_TCPIP, "Err: ND ICMP option len is not 0. opt_len: %d", opt_len);
                    return ND6_ERR_VIOLATE_PROTOCAL;
                }
                break;
            }            

            default:
            {
                /* The type may not be recognized with length of 0. 
                 * If not return, this case will cause infinit loop. */
                return ND6_ERR_INVALID_PARAM;
                //TODO: unrecognizable option should be silently ignored.
            }
        }
        #endif
        
        opt_len = buffer[1];
        if (!opt_len)
        {
            //kal_prompt_trace(MOD_TCPIP, "Err: ND ICMP option len is not 0. opt_len: %d", opt_len);
            return ND6_ERR_VIOLATE_PROTOCAL;
        }
        
        offset += 8 * ((u16_t)buffer[1]);
    }

    return ND6_ERR_OK;
}

s8_t nd6_icmp_ndmsg_commvali_chk(struct pbuf *p)
{    
    if (!p)
    {
        return ND6_ERR_INVALID_PARAM;
    }
    
    if (nd6_icmp_code_chk(*((u8_t*)p->payload),*((u8_t*)p->payload + 1)) < 0 ||
        nd6_icmp_hoplimit_chk() < 0 ||
        nd6_icmp_option_len_chk(p) < 0)
    {
        return ND6_ERR_VIOLATE_PROTOCAL;
    }

    return ND6_ERR_OK;
}

s8_t nd6_icmp_ra_validation_chk(struct pbuf *p)
{
    // struct ra_header * ra_hdr;
    
    if (!p)
    {
        return ND6_ERR_INVALID_PARAM;
    }    

    // ra_hdr = (struct ra_header *)p->payload;
    
    if (!ip6_addr_islinklocal(ip6_current_src_addr()))
    {
        return ND6_ERR_VIOLATE_PROTOCAL;
    }

    if (icmp6_get_icmp_len() < 16 || p->len < 16)
    {
        return ND6_ERR_VIOLATE_PROTOCAL;
    }
    
    return ND6_ERR_OK;        
}


s8_t nd6_icmp_na_validation_chk(struct pbuf *p)
{    
    struct na_header * na_hdr = NULL;

    if (!p)
    {
        return ND6_ERR_INVALID_PARAM;
    }

    if (icmp6_get_icmp_len() < 24 || p->len < 24)
    {
        return ND6_ERR_VIOLATE_PROTOCAL;
    }
    
    na_hdr = (struct na_header *)p->payload;

    if (ip6_addr_ismulticast(&(na_hdr->target_address)))
    {
        return ND6_ERR_VIOLATE_PROTOCAL;
    }

    if (ip6_addr_ismulticast(ip6_current_dest_addr()) &&
        (na_hdr->flags & ND6_FLAG_SOLICITED))
    {
        return ND6_ERR_VIOLATE_PROTOCAL;
    }
    
    return ND6_ERR_OK;        
}


s8_t nd6_icmp_ns_validation_chk(struct pbuf *p)
{    
    struct ns_header * ns_hdr = NULL;

    if (!p)
    {
        return ND6_ERR_INVALID_PARAM;
    }

    if (icmp6_get_icmp_len() < 24 || p->len < 24)
    {
        return ND6_ERR_VIOLATE_PROTOCAL;
    }
    
    ns_hdr = (struct ns_header *)p->payload;

    if (ip6_addr_ismulticast(&(ns_hdr->target_address)))
    {
        return ND6_ERR_VIOLATE_PROTOCAL;
    }

    if (ip6_addr_isany(ip6_current_src_addr()) &&
        !ip6_addr_issolinodemultiaddr(ip6_current_dest_addr()))
    {
        return ND6_ERR_VIOLATE_PROTOCAL;
    }

    return ND6_ERR_OK;        
}


#if REDIRECT_ENABLE
s8_t nd6_icmp_rd_validation_chk(struct pbuf *p, struct netif *inp)
{
  struct redirect_header *rd_hdr = NULL;
  s8_t i = 0;
  ip6_addr_t tmp_addr;
  
  if (!p)
  {
    return ND6_ERR_INVALID_PARAM;
  }

  rd_hdr = (struct redirect_header *)p->payload;
  if (!ip6_addr_islinklocal(ip6_current_src_addr()) ||
    ((icmp6_get_icmp_len() < 40 || p->len < 40)) ||
    ip6_addr_ismulticast(&rd_hdr->destination_address) ||
    !(ip6_addr_islinklocal(&rd_hdr->target_address) ||
    ip6_addr_cmp(&rd_hdr->target_address, &rd_hdr->destination_address)))
  {
    return ND6_ERR_VIOLATE_PROTOCAL;
  }

  /* RFC 4861 8.1 The IP source addresss of the Redirect is the same as the current first-hop router
    * for the specified ICMP Destination Address.
    */
  ip6_addr_set(&tmp_addr, &rd_hdr->destination_address);
  i = nd6_get_next_hop_entry(&tmp_addr, inp);
  if (i < 0 || !ip6_addr_cmp(ip6_current_src_addr(), &neighbor_cache[i].next_hop_address))
  {
    return ND6_ERR_VIOLATE_PROTOCAL;
  }

  return ND6_ERR_OK;
}
#endif

s8_t nd6_icmp_ra_prefix_info_chk(struct prefix_option *prefix_opt)
{
    if (prefix_opt == NULL)
    {
        return ND6_ERR_INVALID_PARAM;
    }

    if (prefix_opt->prefix_length < 0 || prefix_opt->prefix_length > 128)
    {
        return ND6_ERR_INVALID_PARAM;
    } 
        
    if (ip6_addr_islinklocal(&(prefix_opt->prefix)) ||
        ip6_addr_ismulticast(&(prefix_opt->prefix)))
    {
        return ND6_ERR_INVALID_PARAM;
    }

    if (ntohl(prefix_opt->preferred_lifetime) > ntohl(prefix_opt->valid_lifetime))
    {
        return ND6_ERR_INVALID_PARAM;
    }
    
    return ND6_ERR_OK;        
}


void nd6_icmp_na_input(struct pbuf *p, struct netif *inp)
{
    s8_t i, lla_dif = 0;
    struct na_header * na_hdr;
    struct lladdr_option * lladdr_opt = NULL;
    
    if (nd6_icmp_na_validation_chk(p) < ND6_ERR_OK) {
        ND6_STATS_INC(nd6.lenerr);
        ND6_STATS_INC(nd6.drop);
        return;
    }

    na_hdr = (struct na_header *)p->payload;    

    lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct na_header));
    if ((p->len < (sizeof(struct na_header) + sizeof(struct lladdr_option))) ||
        (lladdr_opt->type != ND6_OPTION_TYPE_TARGET_LLADDR))
    {
        // TODO: should traversal the option lists to check if there's TLL option.
        lladdr_opt = NULL;
    }
    
    if (ip6_addr_ismulticast(ip6_current_dest_addr())) 
    {          
        //kal_prompt_trace(MOD_TCPIP, "Muticasted NA received.");

        /* Override ip6_current_dest_addr() so that we have an aligned copy. */
        ip6_addr_set(ip6_current_dest_addr(), &(na_hdr->target_address));

    #if LWIP_IPV6_DUP_DETECT_ATTEMPTS
        /* If the target address matches this netif, it is a DAD response. */
        for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) 
        {
            if (ip6_addr_cmp(ip6_current_dest_addr(), netif_ip6_addr(inp, i))) 
            {
                /* We are using a duplicate address. */
                netif_ip6_addr_set_state(inp, i, IP6_ADDR_INVALID);
                if (ip6_addr_isglobal(netif_ip6_addr(inp, i))) 
                { 
                    /*kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_INPUT_NA_DAD_FAIL, i,
                        inp->ip6_addr[0].addr[0], inp->ip6_addr[0].addr[1], inp->ip6_addr[0].addr[2],
                        inp->ip6_addr[0].addr[3], 0, 0); */

                    if (inp->ip6_autoconfig_enabled == IPV6_AUTOCONFIG_ENABLED) 
                    {
                        //tcpip_get_ipv6_addr_cnf((const u8_t *)(netif_ip6_addr(inp, i))->addr, inp->profile_id, ERR_CONF);
                    } 
                    else if (inp->ip6_autoconfig_enabled == IPV6_AUTOCONFIG_STOP) 
                    {
                        //tcpip_ipv6_connect_ind((const u8_t *)(netif_ip6_addr(inp, i))->addr, inp->profile_id, ERR_CONF);
                    }
                }

            #if LWIP_IPV6_AUTOCONFIG
                /* Check to see if this address was autoconfigured. */
                if (!ip6_addr_islinklocal(ip6_current_dest_addr())) 
                {
                    i = nd6_get_autocfg_prefix(ip6_current_dest_addr(), ND6_AUTOCFG_PREFIX_LEN, inp);
                    if (i >= 0) 
                    {
                        /* Mark this prefix as duplicate, so that we don't use it
                         * to generate this address again. */
                        autocfg_prefix_list[i].flags |= ND6_PREFIX_AUTOCONFIG_ADDRESS_DUPLICATE;
                    }
                }
                else
                {
                    //kal_prompt_trace(MOD_TCPIP, "NA: Duplicated link local address. System error!");
                }
            #endif /* LWIP_IPV6_AUTOCONFIG */

                return;
            }
        }
    #endif /* LWIP_IPV6_DUP_DETECT_ATTEMPTS */
    }

    /* No matter solicated or unsolicated */
    
    /* Override ip6_current_dest_addr() so that we have an aligned copy. */
    ip6_addr_set(ip6_current_dest_addr(), &(na_hdr->target_address));

    i = nd6_find_neighbor_cache_entry(ip6_current_dest_addr());
    /*kal_prompt_trace(MOD_TCPIP, "NCE find result:%d, state:%d", i,
                     i >= 0 ? neighbor_cache[i].state : -1);*/

    if (i >= 0) 
    {  
        if (lladdr_opt && memcmp(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len))
        {
            lla_dif = 1;
        }

        // TODO: what about ND6_NO_ENTRY, though state will not ND6_NO_ENTRY here.
        if (neighbor_cache[i].state == ND6_INCOMPLETE)
        {
            if (lladdr_opt)
            {
                /* ND6_FLAG_OVERRIDE is ignored if the NCE is in INCOMPLETE state. */
                //kal_prompt_trace(MOD_TCPIP, "LLA update.");
                MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
                                
                if (na_hdr->flags & ND6_FLAG_SOLICITED)
                {
                    neighbor_cache[i].state = ND6_REACHABLE;               
                    //neighbor_cache[i].counter.stale_time = 0;
                    neighbor_cache[i].counter.reachable_time = reachable_time;
                }
                else
                {
                    neighbor_cache[i].state = ND6_STALE;               
                    neighbor_cache[i].counter.stale_time = 0;
                }

                if (na_hdr->flags & ND6_FLAG_ROUTER && !neighbor_cache[i].isrouter)
                {
                    neighbor_cache[i].isrouter = 1;
                }
                else if (!(na_hdr->flags & ND6_FLAG_ROUTER) && neighbor_cache[i].isrouter)
                {
                    s8_t rt_indx;

                    rt_indx = nd6_find_default_router_by_neighbor(&(neighbor_cache[i]));
                    if (rt_indx >= 0)
                    {
                        nd6_disc_def_rt_list(rt_indx);
                    }
                }

                /* Send queued packets, if any. */
                if (neighbor_cache[i].q != NULL) 
                {
                    nd6_send_q(i);
                }
            }
            else
            {
                /* else: If NCE is in INCOMPLETE state, Link layer has addresses and Target LLA is                  
                 * not included, silently discard the NA. */
                //kal_prompt_trace(MOD_TCPIP, "INCOMPLETE state, no Target LLA. Silently discard the NA.");
                ND6_STATS_INC(nd6.err);
                ND6_STATS_INC(nd6.drop);
                return;
            }            
            // TODO: how to determine link layer has addresses?            
        }
        else
        {
            if (!(na_hdr->flags & ND6_FLAG_OVERRIDE) && lla_dif)
            {
                if (neighbor_cache[i].state == ND6_REACHABLE)
                {
                    neighbor_cache[i].state = ND6_STALE;
                    neighbor_cache[i].counter.stale_time = 0;
                }
                else
                {
                    /* ND6_FLAG_OVERRIDE is clear and LLA is the different, and ignore the NA. */
                    //kal_prompt_trace(MOD_TCPIP, "overwrite is clr and LLA is dif. Silently discard the NA.");
                    ND6_STATS_INC(nd6.err);
                    ND6_STATS_INC(nd6.drop);
                    return;    
                }
            }
            else
            {
                if (lla_dif)
                {
                    //kal_prompt_trace(MOD_TCPIP, "LLA update.");
                    MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
                }
                
                if (na_hdr->flags & ND6_FLAG_SOLICITED)
                {
                    neighbor_cache[i].state = ND6_REACHABLE;               
                    //neighbor_cache[i].counter.stale_time = 0;
                    neighbor_cache[i].counter.reachable_time = reachable_time;
                }
                else if (lla_dif)
                {
                    neighbor_cache[i].state = ND6_STALE;               
                    neighbor_cache[i].counter.stale_time = 0;
                }
            }

            if (na_hdr->flags & ND6_FLAG_ROUTER && !neighbor_cache[i].isrouter)
            {
                neighbor_cache[i].isrouter = 1;
            }
            else if (!(na_hdr->flags & ND6_FLAG_ROUTER) && neighbor_cache[i].isrouter)
            {
                s8_t rt_indx;

                rt_indx = nd6_find_default_router_by_neighbor(&(neighbor_cache[i]));
                if (rt_indx >= 0)
                {
                    nd6_disc_def_rt_list(rt_indx);
                }
            }
        }            
    }
    else 
    {
        /* NCE is not found. Silently discard the NA. */
        //kal_prompt_trace(MOD_TCPIP, "NCE is not found. Silently discard the NA.");
        ND6_STATS_INC(nd6.err);
        ND6_STATS_INC(nd6.drop);
        return;
    }    

    /*kal_prompt_trace(MOD_TCPIP, "NCE PROCESSED idx:%d, state:%d", i,
    i >= 0 ? neighbor_cache[i].state : -1);*/
}


void nd6_icmp_ns_input(struct pbuf *p, struct netif *inp)
{
    s8_t i;
    struct ns_header * ns_hdr;
    struct lladdr_option * lladdr_opt;
    u8_t has_option = 1;

    /* Check that ns header fits in packet. */
    if (nd6_icmp_ns_validation_chk(p) < 0) 
    {
        /* TODO debug message */
        ND6_STATS_INC(nd6.lenerr);
        ND6_STATS_INC(nd6.drop);
        return;
    }

    ns_hdr = (struct ns_header *)p->payload;   

    /* Check if there is a link-layer address provided. Only point to it if in this buffer. */
    lladdr_opt = NULL;
    if (p->len >= (sizeof(struct ns_header) + sizeof(struct lladdr_option))) {
      lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct ns_header));
    }

    /* Check if the target address is configured on the receiving netif. */
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
      if ((ip6_addr_isvalid(netif_ip6_addr_state(inp, i)) ||
           (ip6_addr_istentative(netif_ip6_addr_state(inp, i)) &&
            ip6_addr_isany(ip6_current_src_addr()))) &&
          ip6_addr_cmp(&(ns_hdr->target_address), netif_ip6_addr(inp, i))) {
        break;
      }
    }

    /* NS not for us? */
    if (LWIP_IPV6_NUM_ADDRESSES == i) { 
        //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_INPUT_NS_ACCEPTED, 0);
      return;
    }
    
    //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_INPUT_NS_ACCEPTED, 1);

    /* Check for ANY address in src (DAD algorithm). */
    if (ip6_addr_isany(ip6_current_src_addr())) {
      if (lladdr_opt != NULL && lladdr_opt->type == ND6_OPTION_TYPE_SOURCE_LLADDR) {
        /* if the source is ANYADDRESS, the OPTION MUST BE NULL drop it! */
        ND6_STATS_INC(nd6.proterr);
        ND6_STATS_INC(nd6.drop);
        return;
      }
      
      /* Sender is validating this address. */
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
        if (ip6_addr_cmp(&(ns_hdr->target_address), netif_ip6_addr(inp, i))) {
          /* Send a NA back so that the sender does not use this address. */
          //kal_prompt_trace(MOD_TCPIP, "ns input, state %d", netif_ip6_addr_state(inp, i));
          // TODO: In all cases, a node MUST NOT respond to a Neighbor Solicitation for a tentative address.
          //      RFC 4862 5.4.3 P15
          
          // TODO: Override flag SHOULD NOT be set in solicited advertisements for anycast addrresses.
          //      RFC 4681 4.4 P24          
          if (ip6_addr_isvalid(netif_ip6_addr_state(inp, i)))
            nd6_send_na(inp, netif_ip6_addr(inp, i), ND6_FLAG_OVERRIDE | ND6_SEND_FLAG_ALLNODES_DEST);
          
          if (ip6_addr_istentative(netif_ip6_addr_state(inp, i))) {
            /* We shouldn't use this address either. */
            netif_ip6_addr_set_state(inp, i, IP6_ADDR_INVALID);
            if (ip6_addr_isglobal(netif_ip6_addr(inp, i))) {  
              /*kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_INPUT_NS_DAD_FAIL, i,
                inp->ip6_addr[0].addr[0], inp->ip6_addr[0].addr[1], inp->ip6_addr[0].addr[2],
                inp->ip6_addr[0].addr[3], inp->ip6_addr[0].addr[4], inp->ip6_addr[0].addr[5]);*/
              
              if (inp->ip6_autoconfig_enabled == IPV6_AUTOCONFIG_ENABLED) {
                //tcpip_get_ipv6_addr_cnf((const u8_t *)(netif_ip6_addr(inp, i))->addr, inp->profile_id, ERR_CONF);
              } 
              else if (inp->ip6_autoconfig_enabled == IPV6_AUTOCONFIG_STOP) {
                //tcpip_ipv6_connect_ind((const u8_t *)(netif_ip6_addr(inp, i))->addr, inp->profile_id, ERR_CONF);
              }
            }
          }
        }
      }
    }
    else {
      /* Sender is trying to resolve our address. */
      /* Verify that they included their own link-layer address. */

      /* Multicast NS must with LLA option */
      if (ip6_addr_ismulticast(ip6_current_dest_addr()) && lladdr_opt == NULL) {
        /* Not a valid message. */
        ND6_STATS_INC(nd6.proterr);
        ND6_STATS_INC(nd6.drop);
        return;
      }

      i = nd6_find_neighbor_cache_entry(ip6_current_src_addr());
      if (i >= 0) {
        /* We already have a record for the solicitor. */
        if ((lladdr_opt != NULL) && (neighbor_cache[i].state == ND6_INCOMPLETE)) {
          neighbor_cache[i].netif = inp;
          MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);

          /* Delay probe in case we get confirmation of reachability from upper layer (TCP). */
          neighbor_cache[i].state = ND6_STALE;
          neighbor_cache[i].counter.stale_time = 0;
        }
      }
      else
      {
        /* Add their IPv6 address and link-layer address to neighbor cache.
         * We will need it at least to send a unicast NA message, but most
         * likely we will also be communicating with this node soon. */
        i = nd6_new_neighbor_cache_entry();
        if (i < 0) {
          /* We couldn't assign a cache entry for this neighbor.
           * we won't be able to reply. drop it. */
          ND6_STATS_INC(nd6.memerr);
          return;
        }
        neighbor_cache[i].netif = inp;   
        
        if (lladdr_opt == NULL) {
          neighbor_cache[i].state = ND6_INCOMPLETE;
          neighbor_cache[i].counter.probes_sent = 0; 
        }
        else {
          MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
          /* Receiving a message does not prove reachability: only in one direction.
                * Delay probe in case we get confirmation of reachability from upper layer (TCP). */      
          //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_INPUT_NS_STATE_CHANGE_1);
          neighbor_cache[i].state = ND6_STALE;
          neighbor_cache[i].counter.stale_time = 0;
        }
        ip6_addr_set(&(neighbor_cache[i].next_hop_address), ip6_current_src_addr());
      }

      /* Override ip6_current_dest_addr() so that we have an aligned copy. */
      ip6_addr_set(ip6_current_dest_addr(), &(ns_hdr->target_address));

      /* different SLLA, update NCE and state change to STALE */      
      if ((i >= 0) && 
          (i < LWIP_ND6_NUM_NEIGHBORS) && 
          (lladdr_opt != NULL) && 
          (memcmp(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len))) {        
        //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_INPUT_NS_STATE_CHANGE_2);        
        MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
        neighbor_cache[i].state = ND6_STALE;
        neighbor_cache[i].counter.stale_time = 0;
      }

      /* Send back a NA for us. Allocate the reply pbuf. */
      if (has_option) {
        nd6_send_na(inp, ip6_current_dest_addr(), ND6_FLAG_SOLICITED | ND6_FLAG_OVERRIDE);
      }
      else {        
        nd6_send_na(inp, ip6_current_dest_addr(), ND6_FLAG_SOLICITED | ND6_FLAG_OVERRIDE);
        //nd6_send_na(inp, ip6_current_dest_addr(), ND6_FLAG_SOLICITED | ND6_FLAG_NO_OPTION);
      }
    }
}


void nd6_icmp_ra_input(struct pbuf *p, struct netif *inp)
{
    s8_t rt_idx;
    struct ra_header * ra_hdr;
    u8_t * buffer; /* Used to copy options. */
    u16_t offset;
    u32_t tmp_value = 0; 
        
    /* Check that RA header fits in packet. */
    if (nd6_icmp_ra_validation_chk(p) < 0) 
    {
      /* TODO debug message */
      ND6_STATS_INC(nd6.lenerr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    ra_hdr = (struct ra_header *)p->payload;
    
    /* If we are sending RS messages, stop. */
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
    inp->rs_count = 0;
    inp->rs_interval = 0;
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */

    /* Get the matching default router entry. */
    rt_idx = nd6_get_router(ip6_current_src_addr(), inp);
    
    if (rt_idx < 0) 
    {
      /* Create a new router entry. */
      rt_idx = nd6_new_router(ip6_current_src_addr(), inp);
    }
    
    if (rt_idx < 0) 
    {
        // Memory is not enough, or router life time is zero && no rt entry exists. 
      ND6_STATS_INC(nd6.memerr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    if (default_router_list[rt_idx].neighbor_entry)
    {
        /* If NCE for routter exists, isrouter must be set to TRUE. */
        default_router_list[rt_idx].neighbor_entry->isrouter = 1;        
    }

    /* Update the parameter/option which has only one value, with current valid RA received, 
     * even when no default Router entry for current RA exists. */
    if (ra_hdr->current_hop_limit)
    {
        current_hop_limit = ra_hdr->current_hop_limit;
        //kal_prompt_trace(MOD_TCPIP, "Current hop limit update:%d", current_hop_limit);
    }

    /* Re-set default timer values. */
#if LWIP_ND6_ALLOW_RA_UPDATES
    tmp_value = ntohl(ra_hdr->reachable_time);
    if (tmp_value) 
    {
        // TODO: Generate new random reachable time value. P54 RFC4861
        // TODO: In most cases reachable time value barely change, then new random reachable time
        // should be generated at least once every few hours.
        reachable_time = tmp_value;
        //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_INPUT_RA_RECHABLE_UPDATE, reachable_time); 
    }

    tmp_value = ntohl(ra_hdr->retrans_timer);
    if (tmp_value) 
    { 
        retrans_timer = tmp_value;
        //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_INPUT_RA_RETRANS_UPDATE, retrans_timer);        
    }        
#endif /* LWIP_ND6_ALLOW_RA_UPDATES */    

    /* Update flags in local entry (incl. preference). */
    default_router_list[rt_idx].flags = ra_hdr->flags;

    /* Offset to options. */
    offset = sizeof(struct ra_header);

    /* Process each option. */
    while ((p->tot_len - offset) > 0) {
      /* Extract one option once a time. */  
      if (p->len == p->tot_len) {
        /* no need to copy from contiguous pbuf */
        buffer = &((u8_t*)p->payload)[offset];
      } else {
        buffer = nd6_ra_buffer;
        pbuf_copy_partial(p, buffer, sizeof(struct prefix_option), offset);
      }
      switch (buffer[0]) {
      case ND6_OPTION_TYPE_SOURCE_LLADDR:
      {
        struct lladdr_option * lladdr_opt;
        lladdr_opt = (struct lladdr_option *)buffer;
        
        if (default_router_list[rt_idx].neighbor_entry)
        {           
            /* NCE for router is just created or its link-layer address is different and should be updated */            
            if ((default_router_list[rt_idx].neighbor_entry->state == ND6_NO_ENTRY) ||
                (default_router_list[rt_idx].neighbor_entry->state == ND6_INCOMPLETE) ||
                memcmp(default_router_list[rt_idx].neighbor_entry->lladdr, lladdr_opt->addr, inp->hwaddr_len))
            {
                SMEMCPY(default_router_list[rt_idx].neighbor_entry->lladdr, lladdr_opt->addr, inp->hwaddr_len);
                default_router_list[rt_idx].neighbor_entry->state = ND6_STALE;
                default_router_list[rt_idx].neighbor_entry->counter.stale_time = 0;
            }
            
            /*kal_prompt_trace(MOD_TCPIP, "Default router's NCE:%x %x, state:%d", 
                ((char*)&(default_router_list[rt_idx].neighbor_entry->next_hop_address))[14], 
                ((char*)&(default_router_list[rt_idx].neighbor_entry->next_hop_address))[15],
                default_router_list[rt_idx].neighbor_entry->state);*/
        }
        else
        {
            /* Each default_router_list entry is associated with a NCE. */
            // TODO: create a NCE for the current router
            //kal_prompt_trace(MOD_TCPIP, "Default router entry without a related NCE.");
            // LWIP_ASSERT("Default router entry without a related NCE.", 0);
        }
        break;
      }
      case ND6_OPTION_TYPE_MTU:
      {
        struct mtu_option * mtu_opt;
        mtu_opt = (struct mtu_option *)buffer;
        // TODO: when mtu >= minimum link MTU && mtu <= maximum LinkMTU, update mtu
        if (ntohl(mtu_opt->mtu) >= 1280) {
#if LWIP_ND6_ALLOW_RA_UPDATES
          inp->mtu = ntohl(mtu_opt->mtu);
#endif /* LWIP_ND6_ALLOW_RA_UPDATES */
        }
        break;
      }
      case ND6_OPTION_TYPE_PREFIX_INFO:
      {
        struct prefix_option * prefix_opt;
        s8_t prefix_idx;
        prefix_opt = (struct prefix_option *)buffer;

        if (nd6_icmp_ra_prefix_info_chk(prefix_opt) == ND6_ERR_OK)
        {
          if (prefix_opt->flags & ND6_PREFIX_FLAG_ON_LINK)
          {
          /* Add to on-link prefix list. */

          /* Get a memory-aligned copy of the prefix. */
          ip6_addr_set(ip6_current_dest_addr(), &(prefix_opt->prefix));

          /* find cache entry for this prefix. */
          prefix_idx = nd6_get_onlink_prefix(ip6_current_dest_addr(), prefix_opt->prefix_length, inp);
          tmp_value = ntohl(prefix_opt->valid_lifetime);
          if (!tmp_value)
          {
            if (prefix_idx >= 0)
            {
                /* Time out the prefix immediately */
                prefix_list[prefix_idx].invalidation_timer = 0;
            }
          }
          else
          {
              if (prefix_idx < 0)
              {
                /* Create a new cache entry. */
                prefix_idx = nd6_new_onlink_prefix(ip6_current_dest_addr(), 
                                                   prefix_opt->prefix_length, 
                                                   inp);
              }
                            
              if (prefix_idx >= 0) 
              {
                prefix_list[prefix_idx].invalidation_timer = tmp_value;
              
                //TODO: What about preferred lifetime?  
               }                
             }
           }
                
          if ((prefix_opt->flags & ND6_PREFIX_FLAG_AUTONOMOUS) && 
              (ND6_AUTOCFG_PREFIX_LEN == prefix_opt->prefix_length))
          {
              /* If the sum of the prefix length and interface identifier length does
                        * not equal 128 bits, ignore the option.*/
               
              /* refer to page 445, too strict */
              if (ip6_addr_isglobal(&(prefix_opt->prefix)) && prefix_opt->prefix_length != 64)
              {
                  //kal_prompt_trace(MOD_TCPIP, "Autocfg prefix len is %d, which is not 64!", prefix_opt->prefix_length);
                  break;
              }
               
              /* Get a memory-aligned copy of the prefix. */
              ip6_addr_set(ip6_current_dest_addr(), &(prefix_opt->prefix));
  
              /* find cache entry for this prefix. */
              prefix_idx = nd6_get_autocfg_prefix(ip6_current_dest_addr(), prefix_opt->prefix_length, inp);
              tmp_value = ntohl(prefix_opt->valid_lifetime);
              
              if (prefix_idx >= 0)    /* Prefix exists. */
              {
                  /* The prefix is received earlier and an address is generated from it. */
                  if (autocfg_prefix_list[prefix_idx].flags & ND6_PREFIX_AUTOCONFIG_ADDRESS_GENERATED)
                  {
                      autocfg_prefix_list[prefix_idx].preferred_timer = ntohl(prefix_opt->preferred_lifetime);
                      if (tmp_value > 7200 || tmp_value > autocfg_prefix_list[prefix_idx].invalidation_timer)
                      {
                          autocfg_prefix_list[prefix_idx].invalidation_timer = (tmp_value > 7200) ? (tmp_value - ND6_TMR_DALAY) : tmp_value;
                      }
                      else if (autocfg_prefix_list[prefix_idx].invalidation_timer > 7200)
                      {
                          autocfg_prefix_list[prefix_idx].invalidation_timer = 7200 - ND6_TMR_DALAY;
                      }
                  }
                  else
                  {
                    autocfg_prefix_list[prefix_idx].invalidation_timer = tmp_value;
                    autocfg_prefix_list[prefix_idx].preferred_timer = ntohl(prefix_opt->preferred_lifetime);
                  }
                  //kal_prompt_trace(MOD_TCPIP, "the current timer is %d", autocfg_prefix_list[prefix_idx].invalidation_timer);
                }
                else    /* Prefix doesn't exist. */
                {
                    /* Create a new cache entry. */
                    prefix_idx = nd6_new_autocfg_prefix(ip6_current_dest_addr(), 
                                                       prefix_opt->prefix_length, 
                                                       inp);
          
                    if (prefix_idx >= 0) 
                    {
                        autocfg_prefix_list[prefix_idx].invalidation_timer = tmp_value;
                        autocfg_prefix_list[prefix_idx].preferred_timer = ntohl(prefix_opt->preferred_lifetime);
          
                        autocfg_prefix_list[prefix_idx].flags = 0;
                        #if 0
                        /* Mark prefix as autonomous, so that address autoconfiguration can take place.
                         * Only OR flag, so that we don't over-write other flags (such as ADDRESS_DUPLICATE)*/
                        autocfg_prefix_list[prefix_idx].flags |= ND6_PREFIX_AUTOCONFIG_AUTONOMOUS;
                        #endif
                    }
                }                    
            }
        }

        break;
      }
        
      case ND6_OPTION_TYPE_ROUTE_INFO:
      {
        /* TODO implement preferred routes.
        struct route_option * route_opt;
        route_opt = (struct route_option *)buffer;*/

        break;
      }
      default:
        /* Unrecognized option, abort. */
        ND6_STATS_INC(nd6.proterr);
        break;
      }
      offset += 8 * ((u16_t)buffer[1]);
    }

#if 0
    /* Re-set invalidation timer. */
    if (ntohs(ra_hdr->router_lifetime) > 0) {
      default_router_list[rt_idx].invalidation_timer = ntohs(ra_hdr->router_lifetime);
      //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_INPUT_RA_INVALID_UPDATE, default_router_list[rt_idx].invalidation_timer); 
    }
    else if (ra_hdr->router_lifetime == 0)
    {
        nd6_disc_def_rt_list(rt_idx);
    }
#endif    

    tmp_value = ntohs(ra_hdr->router_lifetime);
    LWIP_DEBUGF(IP6_DEBUG , ("nd6_icmp_ra_input: rt_idx=%"U16_F", router_lifetime=%"U16_F".\n", rt_idx, tmp_value));
    if (tmp_value > 0)
    {
        default_router_list[rt_idx].invalidation_timer = tmp_value;               
    }
    else
    {
        /* Time out the Default Router List Entry. */
        nd6_disc_def_rt_list(rt_idx);
    }
}


#if REDIRECT_ENABLE
void nd6_icmp_rd_input(struct pbuf *p, struct netif *inp)
{
    s8_t i;
    struct redirect_header * redir_hdr;
    struct lladdr_option * lladdr_opt = NULL;
    u8_t *buffer;
    u16_t offset;
  u8_t rd_opt_buf[sizeof(struct prefix_option)] = {0};

  //kal_prompt_trace(MOD_TCPIP, "nd6_icmp_rd_input()");
  
    /* Check that Redir header fits in packet. */
    if (nd6_icmp_rd_validation_chk(p, inp) < 0) {
      //kal_prompt_trace(MOD_TCPIP, "RD validation failed");
      /* TODO debug message */
      ND6_STATS_INC(nd6.lenerr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    redir_hdr = (struct redirect_header *)p->payload;
   
    /* Copy original destination address to current source address, to have an aligned copy. */
    ip6_addr_set(ip6_current_src_addr(), &(redir_hdr->destination_address));

    /* Find dest address in cache */
    i = nd6_find_destination_cache_entry(ip6_current_src_addr());
    if (i < 0) 
  {
    i = nd6_new_destination_cache_entry();
    if (i < 0)
    {
      return;
    }
    destination_cache[i].pmtu = inp->mtu;
    ip6_addr_set(&(destination_cache[i].destination_addr), &(redir_hdr->destination_address));
    }

    /* Set the new target address. */
    ip6_addr_set(&(destination_cache[i].next_hop_addr), &(redir_hdr->target_address));

  /* Process Options */
  offset = sizeof(struct redirect_header);
  while ((p->tot_len - offset) > 0)
  {
    /* Extract one option once a time */
    if (p->len == p->tot_len)
    {
      /* No need to copy from continous pbuf */
      buffer = & ((u8_t*)p->payload)[offset];
    }
    else
    {
      buffer = rd_opt_buf;
      pbuf_copy_partial(p, buffer, sizeof(struct lladdr_option), offset);      
    }

    /* buffer[0] is the option type */
    switch (buffer[0])
    {
      case ND6_OPTION_TYPE_TARGET_LLADDR:
      {
        if (!lladdr_opt)
        {
          lladdr_opt = (struct lladdr_option *)buffer;
        }
        break;
      }

      /* Didn't process */
      case ND6_OPTION_TYPE_REDIR_HDR:
      {
        break;
      }

      default:
        break;
    }

    offset += 8 * ((u16_t)buffer[1]);
  }

  /* If Link-layer address of other router is given, try to add to neighbor cache. */
  if (lladdr_opt)
  {
    ip6_addr_set(ip6_current_src_addr(), &(redir_hdr->target_address));
    i = nd6_find_neighbor_cache_entry(ip6_current_src_addr());
    if (i < 0)
    {
      i = nd6_new_neighbor_cache_entry();
      if (i < 0)
      {
        ND6_STATS_INC(nd6.memerr);
        return;
      }

      ip6_addr_set(&(neighbor_cache[i].next_hop_address), ip6_current_src_addr());
      neighbor_cache[i].netif = inp;
      neighbor_cache[i].state = ND6_INCOMPLETE;
    }

    if (neighbor_cache[i].state == ND6_INCOMPLETE ||
      memcmp(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len))
    {
      //kal_prompt_trace(MOD_TCPIP, "Update Target LLA");
      MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
      neighbor_cache[i].state = ND6_STALE;
      neighbor_cache[i].counter.stale_time = 0;
    }
  }
  
  if (ip6_addr_cmp(&redir_hdr->target_address, &redir_hdr->destination_address))
  {
    // TODO: Treate the target as on-link
    /* Target address has been set as the next hop address in Destination cache. In such way,
      * the target is treated as on-link?
      */
  }
  else
  {
    ip6_addr_set(ip6_current_src_addr(), &(redir_hdr->target_address));
    i = nd6_find_neighbor_cache_entry(ip6_current_src_addr());
    if (i > 0)
    {
      neighbor_cache[i].isrouter = 1;
    }
    // TODO: if new NCE should be created?
    /* I think no need. If created, NS will be sent to query the LLA of the target address. 
      * And when needed, NCE will be created automatically.
      */
  }  
}
#else
void nd6_icmp_rd_input(struct pbuf *p, struct netif *inp)
{
    s8_t i;
    struct redirect_header * redir_hdr;
    struct lladdr_option * lladdr_opt;

    /* Check that Redir header fits in packet. */
    if (p->len < sizeof(struct redirect_header)) {
      /* TODO debug message */
      ND6_STATS_INC(nd6.lenerr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    redir_hdr = (struct redirect_header *)p->payload;


    lladdr_opt = NULL;
    if (p->len >= (sizeof(struct redirect_header) + sizeof(struct lladdr_option))) {
      lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct redirect_header));
    }

    /* Copy original destination address to current source address, to have an aligned copy. */
    ip6_addr_set(ip6_current_src_addr(), &(redir_hdr->destination_address));

    /* Find dest address in cache */
    i = nd6_find_destination_cache_entry(ip6_current_src_addr());
    if (i < 0) {
      /* Destination not in cache, drop packet. */
      /* Never send any packet to the destination IP */
      return;
    }

    /* Set the new target address. */
    ip6_addr_set(&(destination_cache[i].next_hop_addr), &(redir_hdr->target_address));

    /* If Link-layer address of other router is given, try to add to neighbor cache. */
    if (lladdr_opt != NULL) {
      if (lladdr_opt->type == ND6_OPTION_TYPE_TARGET_LLADDR) {
        /* Copy target address to current source address, to have an aligned copy. */
        ip6_addr_set(ip6_current_src_addr(), &(redir_hdr->target_address));

        i = nd6_find_neighbor_cache_entry(ip6_current_src_addr());
        if (i < 0) {
          i = nd6_new_neighbor_cache_entry();
          if (i >= 0) {
            neighbor_cache[i].netif = inp;
            MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
            ip6_addr_set(&(neighbor_cache[i].next_hop_address), ip6_current_src_addr());

            /* Receiving a message does not prove reachability: only in one direction.
             * Delay probe in case we get confirmation of reachability from upper layer (TCP). */
            // TODO: STALE in fact
            neighbor_cache[i].state = ND6_DELAY;
            neighbor_cache[i].counter.delay_time = LWIP_ND6_DELAY_FIRST_PROBE_TIME;
          }
        }
        if (i >= 0) {
          if (neighbor_cache[i].state == ND6_INCOMPLETE) {
            MEMCPY(neighbor_cache[i].lladdr, lladdr_opt->addr, inp->hwaddr_len);
            /* Receiving a message does not prove reachability: only in one direction.
             * Delay probe in case we get confirmation of reachability from upper layer (TCP). */
            // TODO: STALE in fact
            neighbor_cache[i].state = ND6_DELAY;
            neighbor_cache[i].counter.delay_time = LWIP_ND6_DELAY_FIRST_PROBE_TIME;
          }
        }
      }
    }
}
#endif



void nd6_icmp_ptb_input(struct pbuf *p, struct netif *inp)
{
    s8_t i;
    struct icmp6_hdr *icmp6hdr; /* Packet too big message */
    struct ip6_hdr * ip6hdr; /* IPv6 header of the packet which caused the error */

    /* Check that ICMPv6 header + IPv6 header fit in payload */
    if (p->len < (sizeof(struct icmp6_hdr) + IP6_HLEN)) {
      /* drop short packets */
      ND6_STATS_INC(nd6.lenerr);
      ND6_STATS_INC(nd6.drop);
      return;
    }

    icmp6hdr = (struct icmp6_hdr *)p->payload;
    ip6hdr = (struct ip6_hdr *)((u8_t*)p->payload + sizeof(struct icmp6_hdr));

    /* Copy original destination address to current source address, to have an aligned copy. */
    ip6_addr_set(ip6_current_src_addr(), &(ip6hdr->dest));

    /* Look for entry in destination cache. */
    i = nd6_find_destination_cache_entry(ip6_current_src_addr());
    if (i < 0) {
      /* Destination not in cache, drop packet. */
      if (ip6_addr_ismulticast(ip6_current_src_addr())) {
        if (ntohl(icmp6hdr->data) < inp->mtu && !(ntohl(icmp6hdr->data) < 1280)) {
          inp->mtu = ntohl(icmp6hdr->data);
        }
      }
      return;
    }
    if (destination_cache[i].pmtu != 0 &&
        ntohl(icmp6hdr->data) > destination_cache[i].pmtu && 
        destination_cache[i].age < 600)
        return;
    
    /* Change the Path MTU. */
    destination_cache[i].pmtu = ntohl(icmp6hdr->data);
    //nd6_send_soc_mtu_update(ip6hdr, destination_cache[i].destination_addr, ntohl(icmp6hdr->data));
}

/**
 * Process an incoming neighbor discovery message
 *
 * @param p the nd packet, p->payload pointing to the icmpv6 header
 * @param inp the netif on which this packet was received
 */
void
nd6_input(struct pbuf *p, struct netif *inp)
{
  u8_t msg_type;

  ND6_STATS_INC(nd6.recv);

  msg_type = *((u8_t *)p->payload);

  if (nd6_icmp_ndmsg_commvali_chk(p) < 0)
  {
    //kal_prompt_trace(MOD_TCPIP, "ICMP hop limit or option len error. hop limit:%d", ip6_current_hop_limit());
    /* TODO debug message */
    pbuf_free(p);
    ND6_STATS_INC(nd6.err);
    ND6_STATS_INC(nd6.drop);
    return;
  }

  switch (msg_type) {
  case ICMP6_TYPE_NA: /* Neighbor Advertisement. */
  {
    nd6_icmp_na_input(p, inp);
    break;
  }
  case ICMP6_TYPE_NS: /* Neighbor solicitation. */
  {
    nd6_icmp_ns_input(p, inp);
    break;
  }
  case ICMP6_TYPE_RA: /* Router Advertisement. */
  {
    nd6_icmp_ra_input(p, inp);
    break;
  }
  case ICMP6_TYPE_RD: /* Redirect */
  {
    nd6_icmp_rd_input(p, inp);
    break;
  }

  default:
    ND6_STATS_INC(nd6.proterr);
    ND6_STATS_INC(nd6.drop);
    break; /* default */
  }
  
  pbuf_free(p);
  return;
}

/* Default router entry is to be deleted. The destination entries the next hop addr of which is the
 * router to be deleted must perform next-hop determination again. */
void nd6_update_des_cache(u8_t def_rt_indx)
{
    u8_t i;
    for (i = 0; i < LWIP_ND6_NUM_DESTINATIONS; i++) 
    {    
            nd6_trace_addr((char*)&(destination_cache[i].destination_addr));
        
        if (ip6_addr_cmp(&(destination_cache[i].next_hop_addr), \
            &(default_router_list[def_rt_indx].neighbor_entry->next_hop_address)))
        {
            // TODO: Do next hop address determination again for destination address.
            /*kal_prompt_trace(MOD_TCPIP, "DC updated:%d, %x %x", i, 
            ((char*)&(destination_cache[i].next_hop_addr))[14],
            ((char*)&(destination_cache[i].next_hop_addr))[15]);  */          
            
            memset(&destination_cache[i], 0, sizeof(struct nd6_destination_cache_entry));
        }
        
            nd6_trace_addr((char*)&(destination_cache[i].destination_addr));
    }
}

/* Discard default router list */
static void nd6_disc_def_rt_list(u8_t def_rt_indx)
{
    s8_t i;
    //kal_prompt_trace(MOD_TCPIP, "nd6_disc_def_rt_list(), router index:%d", def_rt_indx);
    LWIP_DEBUGF(IP6_DEBUG , ("nd6_disc_def_rt_list, router index=%"U16_F".\n", def_rt_indx));

    for(i = 0; i < LWIP_ND6_NUM_ROUTERS; i++)
    {
        /*kal_prompt_trace(MOD_TCPIP, "nd6_disc_def_rt_list():%d, %x %x", i, 
            ((char*)&(default_router_list[i].neighbor_entry->next_hop_address))[14], 
            ((char*)&(default_router_list[i].neighbor_entry->next_hop_address))[15]);*/
        
        if ((i != def_rt_indx) &&
            ip6_addr_cmp(&(default_router_list[i].neighbor_entry->next_hop_address), 
            &(default_router_list[def_rt_indx].neighbor_entry->next_hop_address)))
        {
            //kal_prompt_trace(MOD_TCPIP, "Warning: Duplicate default router entries. %d %d", i, def_rt_indx);
            LWIP_DEBUGF(IP6_DEBUG , ("nd6_disc_def_rt_list, %"U16_F", %"U16_F".\n", i, def_rt_indx));
            nd6_update_des_cache(i);
            default_router_list[i].neighbor_entry->isrouter = 0;
            default_router_list[i].neighbor_entry = NULL;
            default_router_list[i].invalidation_timer = 0;
            default_router_list[i].flags = 0;
            // LWIP_ASSERT("Two or more default router entries have the same NCE.", 0);
        }
    }
    
    nd6_update_des_cache(def_rt_indx);
    default_router_list[def_rt_indx].neighbor_entry->isrouter = 0;
    default_router_list[def_rt_indx].neighbor_entry = NULL;
    default_router_list[def_rt_indx].invalidation_timer = 0;
    default_router_list[def_rt_indx].flags = 0;
}


void nd6_tmr_onlink_prefix(void)
{
    s8_t i;
    
    for (i = 0; i < LWIP_ND6_NUM_PREFIXES; i++) 
    {
        /* Skip empty onlink prefix */
        if (!prefix_list[i].invalidation_timer && !prefix_list[i].netif)
        {
            continue;
        }
        
        /* 0xFFFFFFFF means infinity. */
        if (0xFFFFFFFF != prefix_list[i].invalidation_timer)
        {
            if (prefix_list[i].invalidation_timer <= (u32_t)(ND6_TMR_INTERVAL / 1000))
            {
                prefix_list[i].invalidation_timer = 0;
                /*kal_prompt_trace(MOD_TCPIP, "onlink prefix_list[%d] invalid, netif: %x", 
                                 i, prefix_list[i].netif); */
            }
            else
            {
                prefix_list[i].invalidation_timer -= (u32_t)(ND6_TMR_INTERVAL / 1000);
            }
        }
        
        if (!prefix_list[i].invalidation_timer) 
        {
            prefix_list[i].flags = 0;
            prefix_list[i].netif = NULL;             
        }
    }
}


void nd6_tmr_addr_dad(void)
{
    u8_t i;
    struct netif * netif;
    
    /* Process our own addresses, if DAD configured. */
    for (netif = netif_list; netif != NULL; netif = netif->next) 
    {
        for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) 
        {
            if (ip6_addr_istentative(netif->ip6_addr_state[i])) 
            {
                if ((netif->ip6_addr_state[i] & 0x07) >= LWIP_IPV6_DUP_DETECT_ATTEMPTS) 
                {        
                    /* No NA received in response. Mark address as valid. */
                    netif->ip6_addr_state[i] = IP6_ADDR_PREFERRED;
                  #if LWIP_IPV6_MLD
                    if((netif->ip6_addr_state[i] & 0x07) == 0) {
                      ip6_addr_set_solicitednode(&multicast_address, netif_ip6_addr(netif, i)->addr[3]);
                      mld6_joingroup(netif_ip6_addr(netif, i), &multicast_address);
                    }
                  #endif
                    #if 0
                    if (!ip6_addr_islinklocal(netif_ip6_addr(netif, i)))
                    {
                        for (j = 0; j < LWIP_ND6_NUM_AUTOCFG_PREFIXES; j++)
                        {
                            if ((netif == autocfg_prefix_list[j].netif) &&
                                !nd6_match_prefix(netif_ip6_addr(autocfg_prefix_list[j].netif, i), \
                                &(autocfg_prefix_list[j]))
                            {
                                if (!autocfg_prefix_list[j].preferred_timer)
                                {
                                    netif->ip6_addr_state[i] = IP6_ADDR_DEPRECATED;
                                }
                                break;
                            }
                        }
                    }
                    #endif
                    
                    if (ip6_addr_isglobal(netif_ip6_addr(netif, i))) 
                    {
                        if (netif->ip6_autoconfig_enabled == IPV6_AUTOCONFIG_ENABLED) 
                        {                
                            //tcpip_get_ipv6_addr_cnf((const u8_t *)(netif_ip6_addr(netif, i))->addr, netif->profile_id, ERR_OK);
                        } 
                        else if (netif->ip6_autoconfig_enabled == IPV6_AUTOCONFIG_STOP) 
                        {
                            //tcpip_ipv6_connect_ind((const u8_t *)(netif_ip6_addr(netif, i))->addr, netif->profile_id, ERR_OK);
                        }
                        /*kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_INPUT_NS_DAD_SUCCESS, i,
                        netif->ip6_addr[0].addr[0], netif->ip6_addr[0].addr[1], netif->ip6_addr[0].addr[2],
                        netif->ip6_addr[0].addr[3], 0, 0);*/
                    }
                } 
                else if (netif->flags & NETIF_FLAG_UP) 
                {            
            #if 0
                #if LWIP_IPV6_MLD
                    if ((netif->ip6_addr_state[i] & 0x07) == 0) 
                    {
                        /* Join solicited node multicast group. */
                        ip6_addr_set_solicitednode(&multicast_address, netif_ip6_addr(netif, i)->addr[3]);
                        mld6_joingroup(netif_ip6_addr(netif, i), &multicast_address);
                    }
                #endif /* LWIP_IPV6_MLD */
            #endif       
                    /* Send a NS for this address. */
                    //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_TMR_SEND_NS_3, i);

                    nd6_send_ns(netif, netif_ip6_addr(netif, i), ND6_SEND_FLAG_MULTICAST_DEST | ND6_SEND_FLAG_NS_NO_OPTION | ND6_SEND_FLAG_SRC_ANY_ADDR);
                    //kal_prompt_trace(MOD_TCPIP, "ip6_addr_state[%d]:%d", i, netif->ip6_addr_state[i]); 
                    (netif->ip6_addr_state[i])++;
                }
            }
        }
    }
}


void nd6_tmr_autocfg_prefix(void)
{
    s8_t i, j;

    for (i = 0; i < LWIP_ND6_NUM_AUTOCFG_PREFIXES; i++)
    {
        /* Skip empty autocfg prefix */
        if (!autocfg_prefix_list[i].invalidation_timer && !autocfg_prefix_list[i].netif)
        {
            continue;
        }
        
        /* 0xFFFFFFFF means infinity. */
        if (0xFFFFFFFF != autocfg_prefix_list[i].invalidation_timer)
        {
            if (autocfg_prefix_list[i].invalidation_timer <= (u32_t)(ND6_TMR_INTERVAL / 1000))
            {
                autocfg_prefix_list[i].invalidation_timer = 0;
                /*kal_prompt_trace(MOD_TCPIP, "autocfg_prefix_list[%d] invalid, netif: %x", 
                                 i, autocfg_prefix_list[i].netif);*/
            }
            else
            {
                autocfg_prefix_list[i].invalidation_timer -= (u32_t)(ND6_TMR_INTERVAL / 1000);
            }
        }

        /* 0xFFFFFFFF means infinity. */
        if (0xFFFFFFFF != autocfg_prefix_list[i].preferred_timer)
        {
            if (autocfg_prefix_list[i].preferred_timer <= (u32_t)(ND6_TMR_INTERVAL / 1000) ||
                !autocfg_prefix_list[i].invalidation_timer)
            {
                autocfg_prefix_list[i].preferred_timer = 0;
                /*kal_prompt_trace(MOD_TCPIP, "autocfg_prefix_list[%d] perferred time is 0, netif: %x", 
                                 i, autocfg_prefix_list[i].netif);*/
            }
            else
            {
                autocfg_prefix_list[i].preferred_timer -= (u32_t)(ND6_TMR_INTERVAL / 1000);
            }
        }

        
        if (!autocfg_prefix_list[i].invalidation_timer && autocfg_prefix_list[i].netif) 
        {
            /* Check if there's valid autocfg prefix in the same netif */
            for (j = 0; j < LWIP_ND6_NUM_AUTOCFG_PREFIXES; j++) 
            {
                  if ((i != j) && 
                      (autocfg_prefix_list[j].netif == autocfg_prefix_list[i].netif) && 
                      autocfg_prefix_list[j].invalidation_timer)
                      break;
            }
              
            if (LWIP_ND6_NUM_AUTOCFG_PREFIXES == j) 
            {
                /* No valid prefix for the net interface. Notify bearer disconnection. */
                //tcpip_ipv6_disconnect_ind(autocfg_prefix_list[i].netif->profile_id, ERR_ABRT);
                
                /* Skip link local address, and invalidate all other v6 addresses. */  
                for (j = 1; j < LWIP_IPV6_NUM_ADDRESSES; j++) 
                {
                    netif_ip6_addr_state(autocfg_prefix_list[i].netif, j) = IP6_ADDRESS_STATE_INVALID;
                  #if LWIP_IPV6_MLD
                    ip6_addr_set_solicitednode(&multicast_address, netif_ip6_addr(autocfg_prefix_list[i].netif, j)->addr[3]);
                    mld6_leavegroup(netif_ip6_addr(autocfg_prefix_list[i].netif, j), &multicast_address);
                  #endif
                    memset(netif_ip6_addr(autocfg_prefix_list[i].netif, j), 0, sizeof(ip6_addr_t));
                }
                autocfg_prefix_list[i].netif->ip6_autoconfig_enabled = IPV6_AUTOCONFIG_STOP;                
                //autocfg_prefix_list[i].netif->ip6_autoconfig_now = 1;
                //autocfg_prefix_list[i].flags |= ~ND6_PREFIX_AUTOCONFIG_ADDRESS_GENERATED;
                //autocfg_prefix_list[i].flags &= ~ND6_PREFIX_AUTOCONFIG_ADDRESS_GENERATED;
                //autocfg_prefix_list[i].flags = 0;
                //autocfg_prefix_list[i].netif = NULL;
            }
            else
            {
                /* Only invalidate v6 addresses generated from the autocfg prefix */
                for (j = 1; j < LWIP_IPV6_NUM_ADDRESSES; j++)
                {
                    if (!nd6_match_prefix(netif_ip6_addr(autocfg_prefix_list[i].netif, j), \
                        &(autocfg_prefix_list[i])))
                    {
                        netif_ip6_addr_state(autocfg_prefix_list[i].netif, j) = IP6_ADDRESS_STATE_INVALID;
                      #if LWIP_IPV6_MLD
                        ip6_addr_set_solicitednode(&multicast_address, netif_ip6_addr(autocfg_prefix_list[i].netif, j)->addr[3]);
                        mld6_leavegroup(netif_ip6_addr(autocfg_prefix_list[i].netif, j), &multicast_address);
                      #endif
                        memset(netif_ip6_addr(autocfg_prefix_list[i].netif, j), 0, sizeof(ip6_addr_t));
                    }
                }
            }

            autocfg_prefix_list[i].flags = 0;
            autocfg_prefix_list[i].netif = NULL;
        }
        else if (autocfg_prefix_list[i].invalidation_timer && autocfg_prefix_list[i].netif)
        {
            /* Initiate address autoconfiguration for this prefix, if conditions are met. */
            if ((autocfg_prefix_list[i].netif->ip6_autoconfig_enabled != IPV6_AUTOCONFIG_DISENABLED) &&
                //(autocfg_prefix_list[i].flags & ND6_PREFIX_AUTOCONFIG_AUTONOMOUS) &&
                !(autocfg_prefix_list[i].flags & ND6_PREFIX_AUTOCONFIG_ADDRESS_GENERATED))
            {
                /* Try to get an address on this netif that is invalid.
                * Skip 0 index (link-local address) */

                /*if (autocfg_prefix_list[i].netif->ip6_autoconfig_enabled == IPV6_AUTOCONFIG_STOP && 
                    autocfg_prefix_list[i].netif->ip6_autoconfig_now == 1) 
                {
                    // TODO: Link local address is generated in activation stage. And its valid time is 
                    // infinit.
                    // netif_create_ip6_linklocal_address(prefix_list[i].netif, 1, NULL);
                    autocfg_prefix_list[i].netif->ip6_autoconfig_now = 0;
                }*/

                /* Check if the link local address for the netif is valid. */            
                if (ip6_addr_isvalid(netif_ip6_addr_state(autocfg_prefix_list[i].netif, 0)) &&
                    ip6_addr_islinklocal(netif_ip6_addr(autocfg_prefix_list[i].netif, 0)))
                {                   
                    for (j = 1; j < LWIP_IPV6_NUM_ADDRESSES; j++) 
                    {
                        if (netif_ip6_addr_state(autocfg_prefix_list[i].netif, j) == IP6_ADDRESS_STATE_INVALID) 
                        {
                            /* Generate an address using this prefix and interface ID from link-local address. */                        
                            netif_ip6_addr(autocfg_prefix_list[i].netif, j)->addr[0] = autocfg_prefix_list[i].prefix.addr[0];
                            netif_ip6_addr(autocfg_prefix_list[i].netif, j)->addr[1] = autocfg_prefix_list[i].prefix.addr[1];
                            netif_ip6_addr(autocfg_prefix_list[i].netif, j)->addr[2] = netif_ip6_addr(autocfg_prefix_list[i].netif, 0)->addr[2];
                            netif_ip6_addr(autocfg_prefix_list[i].netif, j)->addr[3] = netif_ip6_addr(autocfg_prefix_list[i].netif, 0)->addr[3];

                            /* Mark it as tentative (DAD will be performed if configured). */
                            //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_TMR_SET_STATE);
                            netif_ip6_addr_set_state(autocfg_prefix_list[i].netif, j, IP6_ADDR_TENTATIVE);

                            /* Mark this prefix as ADDRESS_GENERATED, so that we don't try again. */
                            autocfg_prefix_list[i].flags |= ND6_PREFIX_AUTOCONFIG_ADDRESS_GENERATED;                
                            break;
                        }
                    }
                }
                else
                {
                    // TODO: Check if link local address should be generated.
                    // netif_create_ip6_linklocal_address(autocfg_prefix_list[i].netif, 1, NULL);
                    //kal_prompt_trace(MOD_TCPIP, "Link loacal address is invalid. Autocfg cancel.");
                }
            }
            else if ((autocfg_prefix_list[i].flags & ND6_PREFIX_AUTOCONFIG_ADDRESS_GENERATED) &&
                      !(autocfg_prefix_list[i].flags & ND6_PREFIX_AUTOCONFIG_ADDRESS_DUPLICATE) &&
                      !(autocfg_prefix_list[i].flags & ND6_PREFIX_AUTOCONFIG_ADDRESS_DEPRECATED) &&                      
                      !autocfg_prefix_list[i].preferred_timer)
            {
                for (j = 1; j < LWIP_IPV6_NUM_ADDRESSES; j++)
                {
                    if (!nd6_match_prefix(netif_ip6_addr(autocfg_prefix_list[i].netif, j), \
                        &(autocfg_prefix_list[i])))
                    {
                        //kal_prompt_trace(MOD_TCPIP, "Global address is deprecated.");
                        netif_ip6_addr_set_state(autocfg_prefix_list[i].netif, j, IP6_ADDR_DEPRECATED);                        
                      #if LWIP_IPV6_MLD
                        ip6_addr_set_solicitednode(&multicast_address, netif_ip6_addr(autocfg_prefix_list[i].netif, j)->addr[3]);
                        mld6_leavegroup(netif_ip6_addr(autocfg_prefix_list[i].netif, j), &multicast_address);
                      #endif
                    }
                }

                autocfg_prefix_list[i].flags |= ND6_PREFIX_AUTOCONFIG_ADDRESS_DEPRECATED;
            }
        }
    }
}


s8_t nd6_gen_ns_src_flag(u8_t nce_idx)
{
    struct ip6_hdr *ip6hdr;
    s8_t addr_idx = -1;

    if (nce_idx >= LWIP_ND6_NUM_NEIGHBORS)
    {
         return ND6_ERR_INVALID_PARAM;
    }

    if ((ND6_INCOMPLETE != neighbor_cache[nce_idx].state) &&
        (ND6_PROBE != neighbor_cache[nce_idx].state))
    {
        return ND6_ERR_MISC_ERR;
    }

    neighbor_cache[nce_idx].src_flag &= ~ND6_SEND_FLAG_SRC_MARK_ALL;
    if (neighbor_cache[nce_idx].q && neighbor_cache[nce_idx].netif)
    {
        //nd6_trace_addr((char *)(&(ip6hdr->src)));
    #if LWIP_ND6_QUEUEING
        ip6hdr = (struct ip6_hdr *)(neighbor_cache[nce_idx].q->p->payload);
    #else
        ip6hdr = (struct ip6_hdr *)(neighbor_cache[nce_idx].q->payload);
    #endif
        for (addr_idx = 0; addr_idx < LWIP_IPV6_NUM_ADDRESSES; addr_idx++) 
        {
          if (ip6_addr_cmp(&(ip6hdr->src), netif_ip6_addr(neighbor_cache[nce_idx].netif, addr_idx)) &&
              ip6_addr_isvalid(netif_ip6_addr_state(neighbor_cache[nce_idx].netif, addr_idx)))
          {
              break;
          }
        }

        /* unspecific address: addr_idx == LWIP_IPV6_NUM_ADDRESSES */
        neighbor_cache[nce_idx].src_flag |= ND6_SEND_FLAG_SRC_LINK_LOCAL << addr_idx;       
    }
    else
    {
        neighbor_cache[nce_idx].src_flag |= ND6_SEND_FLAG_SRC_LINK_LOCAL;
    }
    
    /*kal_prompt_trace(MOD_TCPIP, "neighbor_cache[%d].src_flag: %x, addr_idx: %d", 
                     nce_idx, neighbor_cache[nce_idx].src_flag, addr_idx);    */    

    return ND6_ERR_OK;
}


void nd6_snd_ns_immediately(u8_t nce_idx)
{
    /*kal_prompt_trace(MOD_TCPIP, "nd6_snd_ns_immediately(): NCE index: %d, state: %d", 
        nce_idx, 
        nce_idx < LWIP_ND6_NUM_NEIGHBORS ? neighbor_cache[nce_idx].state : -1);*/

    if (nce_idx >= LWIP_ND6_NUM_NEIGHBORS)
    {
         return;
    }

    if (ND6_INCOMPLETE == neighbor_cache[nce_idx].state)
    {
        if (nd6_gen_ns_src_flag(nce_idx) != ND6_ERR_OK)
        {
            return;
        }
        
        nd6_send_ns(neighbor_cache[nce_idx].netif, 
                  &(neighbor_cache[nce_idx].next_hop_address), 
                  neighbor_cache[nce_idx].src_flag | ND6_SEND_FLAG_MULTICAST_DEST);        
    }
    else if (ND6_PROBE == neighbor_cache[nce_idx].state)
    {
        nd6_send_ns(neighbor_cache[nce_idx].netif, &(neighbor_cache[nce_idx].next_hop_address), 0);
    }
    else
    {
        return;
    }
    
    neighbor_cache[nce_idx].counter.probes_sent++;        
    neighbor_cache[nce_idx].retrans_interval = 0;
}


/**
 * Periodic timer for Neighbor discovery functions:
 *
 * - Update neighbor reachability states
 * - Update destination cache entries age
 * - Update invalidation timers of default routers and on-link prefixes
 * - Perform duplicate address detection (DAD) for our addresses
 * - Send router solicitations
 */
void
nd6_tmr(void)
{
  s8_t i, j, snd_rs = 0;
  struct netif * netif;

  /* Process router entries. */
  for (i = 0; i < LWIP_ND6_NUM_ROUTERS; i++) {
    if (default_router_list[i].neighbor_entry != NULL) {
      /* Active entry. */
      if (default_router_list[i].invalidation_timer > 0) {
        default_router_list[i].invalidation_timer -= ND6_TMR_INTERVAL / 1000;
      }
      if (default_router_list[i].invalidation_timer < ND6_TMR_INTERVAL / 1000) {
        /* Less than 1 second remainig. Clear this entry. */
        
        /*nd6_update_des_cache(i);
            default_router_list[i].neighbor_entry->isrouter = 0;
            default_router_list[i].neighbor_entry = NULL;
            default_router_list[i].invalidation_timer = 0;
            default_router_list[i].flags = 0;*/
        nd6_disc_def_rt_list(i);
      }
    }
  }

  /* Process neighbor entries. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    switch (neighbor_cache[i].state) {
    case ND6_INCOMPLETE:
      if (neighbor_cache[i].counter.probes_sent >= LWIP_ND6_MAX_MULTICAST_SOLICIT) {
        /* Retries exceeded. */
      #if REDIRECT_ENABLE
        //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_TMR_FREE_NEIGHBOR, i);
        for (j = 0; j < LWIP_ND6_NUM_DESTINATIONS; j++)
        {
          if (ip6_addr_cmp(&destination_cache[j].next_hop_addr, &neighbor_cache[i].next_hop_address))
          {
            //kal_prompt_trace(MOD_TCPIP, "1Free Destination Cache with next hop address deleted j=%d.", j);
            nd6_trace_addr((char*)&destination_cache[j].destination_addr);
            nd6_trace_addr((char*)&destination_cache[j].next_hop_addr);        
            memset(&destination_cache[j], 0, sizeof(struct nd6_destination_cache_entry));  
          }
        }
      #endif
        nd6_free_neighbor_cache_entry(i);
      } else {
        /* Send a NS for this entry. */
        neighbor_cache[i].retrans_interval += ND6_TMR_INTERVAL;
        if (ND6_ABS((s32_t)neighbor_cache[i].retrans_interval - (s32_t)retrans_timer) <= ND6_TMR_TOLERATE_BIAS) {
          nd6_snd_ns_immediately(i);
        }
      }
      break;
    case ND6_REACHABLE:
      /* Send queued packets, if any are left. Should have been sent already. */
      if (neighbor_cache[i].q != NULL) {
        nd6_send_q(i);
      }
      if (neighbor_cache[i].counter.reachable_time <= ND6_TMR_INTERVAL) {
        /* Change to stale state. */
        neighbor_cache[i].state = ND6_STALE;
        neighbor_cache[i].counter.stale_time = 0;
      }
      else {
        neighbor_cache[i].counter.reachable_time -= ND6_TMR_INTERVAL;
      }
      break;
    case ND6_STALE:
      neighbor_cache[i].counter.stale_time += ND6_TMR_INTERVAL;
      /* Send queued packets, if any are left. Should have been sent already. */
      if (neighbor_cache[i].q != NULL) {
        nd6_send_q(i);
      }
      break;
    case ND6_DELAY:
      if (neighbor_cache[i].q != NULL) {
        nd6_send_q(i);
      }
      if (neighbor_cache[i].counter.delay_time <= ND6_TMR_INTERVAL) {
        /* Change to PROBE state. */
        neighbor_cache[i].state = ND6_PROBE;
        neighbor_cache[i].counter.probes_sent = 0;
        nd6_snd_ns_immediately(i);
      } else {
        neighbor_cache[i].counter.delay_time -= ND6_TMR_INTERVAL;
      }
      break;
    case ND6_PROBE:
      if (neighbor_cache[i].counter.probes_sent >= LWIP_ND6_MAX_MULTICAST_SOLICIT) {
         /* Retries exceeded. */
         //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_TMR_FREE_NEIGHBOR, i);        
      #if REDIRECT_ENABLE
        for (j = 0; j < LWIP_ND6_NUM_DESTINATIONS; j++)
        {
          if (ip6_addr_cmp(&destination_cache[j].next_hop_addr, &neighbor_cache[i].next_hop_address))
          {
            //kal_prompt_trace(MOD_TCPIP, "5Free Destination Cache with next hop address deleted j=%d.", j);
            nd6_trace_addr((char*)&destination_cache[j].destination_addr);
            nd6_trace_addr((char*)&destination_cache[j].next_hop_addr);        
            memset(&destination_cache[j], 0, sizeof(struct nd6_destination_cache_entry));  
          }
        }
      #endif    
        nd6_free_neighbor_cache_entry(i);
      } else {
        /* Send a NS for this entry. */
        neighbor_cache[i].retrans_interval += ND6_TMR_INTERVAL;
        if (ND6_ABS((s32_t)neighbor_cache[i].retrans_interval - (s32_t)retrans_timer) <= ND6_TMR_TOLERATE_BIAS)
        {
          nd6_snd_ns_immediately(i);
        }
      }
      break;
    case ND6_NO_ENTRY:
    default:
      /* Do nothing. */
      break;
    }
  }

  


  /* Process destination entries. */
  for (i = 0; i < LWIP_ND6_NUM_DESTINATIONS; i++) {
    // TODO: risk of overflow
      destination_cache[i].age++;
    }
    
    /* Process onlink prefix entries. */
    nd6_tmr_onlink_prefix();

    /* Process autocfg prefix entries. */
    nd6_tmr_autocfg_prefix();  

    nd6_tmr_addr_dad();

#if LWIP_IPV6_SEND_ROUTER_SOLICIT
  /* Send router solicitation messages, if necessary. */ 
  for (netif = netif_list; netif != NULL; netif = netif->next) {
    u32_t k;
        snd_rs = -1;
    for (k = 0; k < LWIP_IPV6_NUM_ADDRESSES; k++)
    {
        if (ip6_addr_islinklocal(netif_ip6_addr(netif, k)) && (ip6_addr_isvalid(netif_ip6_addr_state(netif, k))))
        {
            snd_rs = 0;
            break;
    }
  }

    if (snd_rs == 0 && (netif->rs_count > 0) && (netif->flags & NETIF_FLAG_UP)) {  
        netif->rs_interval -= ND6_TMR_INTERVAL;
        if (netif->rs_interval <= 0){
          //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_TMR_SEND_RS);
          nd6_send_rs(netif);
        netif->rs_count--;
          netif->rs_interval = netif->rs_count ? LWIP_ND6_PTR_SOLICITATION_INTERVAL : 0;
      }
    }
  }
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */
}

/**
 * Send a neighbor solicitation message
 *
 * @param netif the netif on which to send the message
 * @param target_addr the IPv6 target address for the ND message
 * @param flags one of ND6_SEND_FLAG_*
 */
static void
nd6_send_ns(struct netif * netif, const ip6_addr_t * target_addr, u8_t flags)
{
  struct ns_header * ns_hdr;
  struct lladdr_option * lladdr_opt;
  struct pbuf * p;
  const ip6_addr_t * src_addr = IP6_ADDR_ANY6;
  u16_t lladdr_opt_len = 0;
  u8_t no_need_option = flags & ND6_SEND_FLAG_NS_NO_OPTION;

    if (flags & ND6_SEND_FLAG_SRC_ANY_ADDR)
    {
        src_addr = IP6_ADDR_ANY6;
        no_need_option = 1;
        //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_SEND_NS_ANY);
    }
    else
    {
        u8_t addr_idx = 0; /* Link local address by default */
        
        if (flags & ND6_SEND_FLAG_SRC_ADDR1)
        {
            addr_idx = 1;
        }
        else if (flags & ND6_SEND_FLAG_SRC_ADDR2)
        {
            addr_idx = 2;
        }

        if (ip6_addr_isvalid(netif_ip6_addr_state(netif,addr_idx)))
        {
            src_addr = netif_ip6_addr(netif, addr_idx);  
        }
    }

    //kal_prompt_trace(MOD_TCPIP, "SND_NS src_flag:%x", flags);
#if 0    
  if (ip6_addr_isvalid(netif_ip6_addr_state(netif,0)) && ((flags & ND6_SEND_FLAG_SRC_ANY_ADDR) == 0)) {
    /* Use link-local address as source address. */
    src_addr = netif_ip6_addr(netif, 0);
  } else {
    src_addr = IP6_ADDR_ANY;
    no_need_option = 1;
    //kal_trace(LWIP_TRACE_G7_ICMPV6, ICMP6_ND6_SEND_NS_ANY);
  }
#endif

  /* Allocate a packet. */
  if (!no_need_option) {
    lladdr_opt_len = ((netif->hwaddr_len + 2) >> 3) + (((netif->hwaddr_len + 2) & 0x07) ? 1 : 0);
  }
  p = pbuf_alloc(PBUF_IP, sizeof(struct ns_header) + (lladdr_opt_len << 3), PBUF_RAM);
  if ((p == NULL) || (p->len < (sizeof(struct ns_header) + (lladdr_opt_len << 3)))) {
    /* We couldn't allocate a suitable pbuf for the ns. drop it. */
    if (p != NULL) {
      pbuf_free(p);
    }
    ND6_STATS_INC(nd6.memerr);
    return;
  }

  /* Set fields. */
  ns_hdr = (struct ns_header *)p->payload;
  if (!no_need_option) {
    lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct ns_header));
  }

  ns_hdr->type = ICMP6_TYPE_NS;
  ns_hdr->code = 0;
  ns_hdr->chksum = 0;
  ns_hdr->reserved = 0;
  ip6_addr_set(&(ns_hdr->target_address), target_addr);

  if (!no_need_option) {
    lladdr_opt->type = ND6_OPTION_TYPE_SOURCE_LLADDR;
    lladdr_opt->length = (u8_t)lladdr_opt_len;
    SMEMCPY(lladdr_opt->addr, netif->hwaddr, netif->hwaddr_len);
  }

  /* Generate the solicited node address for the target address. */
  if (flags & ND6_SEND_FLAG_MULTICAST_DEST) {
    ip6_addr_set_solicitednode(&multicast_address, target_addr->addr[3]);
    target_addr = &multicast_address;
  }

#if CHECKSUM_GEN_ICMP6
  ns_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, src_addr,
    target_addr);
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Send the packet out. */
  ND6_STATS_INC(nd6.xmit);
  ip6_output_if(p, (src_addr == IP6_ADDR_ANY6) ? NULL : src_addr, target_addr,
      LWIP_ICMP6_HL, 0, IP6_NEXTH_ICMP6, netif);
  pbuf_free(p);
}

/**
 * Send a neighbor advertisement message
 *
 * @param netif the netif on which to send the message
 * @param target_addr the IPv6 target address for the ND message
 * @param flags one of ND6_SEND_FLAG_*
 */
static void
nd6_send_na(struct netif * netif, const ip6_addr_t * target_addr, u8_t flags)
{
  struct na_header * na_hdr;
  struct lladdr_option * lladdr_opt;
  struct pbuf * p;
  const ip6_addr_t * src_addr;
  const ip6_addr_t * dest_addr;
  u16_t lladdr_opt_len = 0;
  u8_t no_need_option = 0;

  /* Use link-local address as source address. */
  /* src_addr = &(netif->ip6_addr[0]); */
  /* Use target address as source address. */
  src_addr = target_addr;

  no_need_option = flags & ND6_FLAG_NO_OPTION;
  /* Allocate a packet. */
  if (no_need_option != ND6_FLAG_NO_OPTION) {
    lladdr_opt_len = ((netif->hwaddr_len + 2) >> 3) + (((netif->hwaddr_len + 2) & 0x07) ? 1 : 0);
  }
  p = pbuf_alloc(PBUF_IP, sizeof(struct na_header) + (lladdr_opt_len << 3), PBUF_RAM);
  if ((p == NULL) || (p->len < (sizeof(struct na_header) + (lladdr_opt_len << 3)))) {
    /* We couldn't allocate a suitable pbuf for the ns. drop it. */
    if (p != NULL) {
      pbuf_free(p);
    }
    ND6_STATS_INC(nd6.memerr);
    return;
  }

  /* Set fields. */
  na_hdr = (struct na_header *)p->payload;
  if (no_need_option != ND6_FLAG_NO_OPTION) {
    lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct na_header));
  }

  na_hdr->type = ICMP6_TYPE_NA;
  na_hdr->code = 0;
  na_hdr->chksum = 0;
  na_hdr->flags = flags & 0xe0; /* R, S, O */
  na_hdr->reserved[0] = 0;
  na_hdr->reserved[1] = 0;
  na_hdr->reserved[2] = 0;
  ip6_addr_set(&(na_hdr->target_address), target_addr);

  if (no_need_option != ND6_FLAG_NO_OPTION) {
    lladdr_opt->type = ND6_OPTION_TYPE_TARGET_LLADDR;
    lladdr_opt->length = (u8_t)lladdr_opt_len;
    SMEMCPY(lladdr_opt->addr, netif->hwaddr, netif->hwaddr_len);
  }

  /* Generate the solicited node address for the target address. */
  if (flags & ND6_SEND_FLAG_MULTICAST_DEST) {
    ip6_addr_set_solicitednode(&multicast_address, target_addr->addr[3]);
    dest_addr = &multicast_address;
  }
  else if (flags & ND6_SEND_FLAG_ALLNODES_DEST) {
    ip6_addr_set_allnodes_linklocal(&multicast_address);
    dest_addr = &multicast_address;
  }
  else {
    dest_addr = ip6_current_src_addr();
  }

#if CHECKSUM_GEN_ICMP6
  na_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, src_addr,
    dest_addr);
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Send the packet out. */
  ND6_STATS_INC(nd6.xmit);
  ip6_output_if(p, src_addr, dest_addr,
      LWIP_ICMP6_HL, 0, IP6_NEXTH_ICMP6, netif);
  pbuf_free(p);
}

#if LWIP_IPV6_SEND_ROUTER_SOLICIT
/**
 * Send a router solicitation message
 *
 * @param netif the netif on which to send the message
 */
static err_t
nd6_send_rs(struct netif * netif)
{
  struct rs_header * rs_hdr;
  struct lladdr_option * lladdr_opt;
  struct pbuf * p;
  const ip6_addr_t * src_addr;
  err_t err;
  u16_t lladdr_opt_len = 0;

  /* Link-local source address, or unspecified address? */
  if (ip6_addr_isvalid(netif_ip6_addr_state(netif, 0))) {
    src_addr = netif_ip6_addr(netif, 0);
  }
  else {
    src_addr = IP6_ADDR_ANY6;
  }

  /* Generate the all routers target address. */
  ip6_addr_set_allrouters_linklocal(&multicast_address);

  /* Allocate a packet. */
  if (src_addr != IP6_ADDR_ANY6) {
    lladdr_opt_len = ((netif->hwaddr_len + 2) >> 3) + (((netif->hwaddr_len + 2) & 0x07) ? 1 : 0);
  }
  p = pbuf_alloc(PBUF_IP, sizeof(struct rs_header) + (lladdr_opt_len << 3), PBUF_RAM);
  if ((p == NULL) || (p->len < (sizeof(struct rs_header) + (lladdr_opt_len << 3)))) {
    /* We couldn't allocate a suitable pbuf for the ns. drop it. */
    if (p != NULL) {
      pbuf_free(p);
    }
    ND6_STATS_INC(nd6.memerr);
    return ERR_BUF;
  }

  /* Set fields. */
  rs_hdr = (struct rs_header *)p->payload;

  rs_hdr->type = ICMP6_TYPE_RS;
  rs_hdr->code = 0;
  rs_hdr->chksum = 0;
  rs_hdr->reserved = 0;

  if (src_addr != IP6_ADDR_ANY6) {
    /* Include our hw address. */
    lladdr_opt = (struct lladdr_option *)((u8_t*)p->payload + sizeof(struct rs_header));
    lladdr_opt->type = ND6_OPTION_TYPE_SOURCE_LLADDR;
    lladdr_opt->length = (u8_t)lladdr_opt_len;
    SMEMCPY(lladdr_opt->addr, netif->hwaddr, netif->hwaddr_len);
  }

#if CHECKSUM_GEN_ICMP6
  rs_hdr->chksum = ip6_chksum_pseudo(p, IP6_NEXTH_ICMP6, p->len, src_addr,
    &multicast_address);
#endif /* CHECKSUM_GEN_ICMP6 */

  /* Send the packet out. */
  ND6_STATS_INC(nd6.xmit);

  err = ip6_output_if(p, (src_addr == IP6_ADDR_ANY6) ? NULL : src_addr, &multicast_address,
      LWIP_ICMP6_HL, 0, IP6_NEXTH_ICMP6, netif);
  pbuf_free(p);

  return err;
}
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */

/**
 * Search for a neighbor cache entry
 *
 * @param ip6addr the IPv6 address of the neighbor
 * @return The neighbor cache entry index that matched, -1 if no
 * entry is found
 */
static s8_t
nd6_find_neighbor_cache_entry(const ip6_addr_t * ip6addr)
{
  s8_t i;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (ip6_addr_cmp(ip6addr, &(neighbor_cache[i].next_hop_address))) {
      if (neighbor_cache[i].state == ND6_NO_ENTRY)
        neighbor_cache[i].state = ND6_INCOMPLETE;
      return i;
    }
  }
  return -1;
}

static s8_t
nd6_find_nce(const ip6_addr_t * ip6addr,  struct netif * netif)
{
  s8_t i;
  
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (ip6_addr_cmp(ip6addr, &(neighbor_cache[i].next_hop_address)) &&
        (neighbor_cache[i].netif == NULL || neighbor_cache[i].netif == netif))  {
        return i;
    }
  }
    
  return -1;
}


/**
 * Create a new neighbor cache entry.
 *
 * If no unused entry is found, will try to recycle an old entry
 * according to ad-hoc "age" heuristic.
 *
 * @return The neighbor cache entry index that was created, -1 if no
 * entry could be created
 */
static s8_t
nd6_new_neighbor_cache_entry(void)
{
  s8_t i;
  s8_t j;
  u32_t time;


  /* First, try to find an empty entry. */
    for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) 
    {
        if (neighbor_cache[i].state == ND6_NO_ENTRY &&
            !neighbor_cache[i].isrouter) 
        {
            nd6_free_neighbor_cache_entry(i);
            return i;
        }
     }

  /* We need to recycle an entry. in general, do not recycle if it is a router. */

  /* Next, try to find a Stale entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_STALE) &&
        (!neighbor_cache[i].isrouter)) {
      nd6_free_neighbor_cache_entry(i);
      return i;
    }
  }

  /* Next, try to find a Probe entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_PROBE) &&
        (!neighbor_cache[i].isrouter)) {
      nd6_free_neighbor_cache_entry(i);
      return i;
    }
  }

  /* Next, try to find a Delayed entry. */
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_DELAY) &&
        (!neighbor_cache[i].isrouter)) {
      nd6_free_neighbor_cache_entry(i);
      return i;
    }
  }

  /* Next, try to find the oldest reachable entry. */
  time = 0xfffffffful;
  j = -1;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_REACHABLE) &&
        (!neighbor_cache[i].isrouter)) {
      if (neighbor_cache[i].counter.reachable_time < time) {
        j = i;
        time = neighbor_cache[i].counter.reachable_time;
      }
    }
  }
  if (j >= 0) {
    nd6_free_neighbor_cache_entry(j);
    return j;
  }

  /* Next, find oldest incomplete entry without queued packets. */
  time = 0;
  j = -1;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if (
        (neighbor_cache[i].q == NULL) &&
        (neighbor_cache[i].state == ND6_INCOMPLETE) &&
        (!neighbor_cache[i].isrouter)) {
      if (neighbor_cache[i].counter.probes_sent >= time) {
        j = i;
        time = neighbor_cache[i].counter.probes_sent;
      }
    }
  }
  if (j >= 0) {
    nd6_free_neighbor_cache_entry(j);
    return j;
  }

  /* Next, find oldest incomplete entry with queued packets. */
  time = 0;
  j = -1;
  for (i = 0; i < LWIP_ND6_NUM_NEIGHBORS; i++) {
    if ((neighbor_cache[i].state == ND6_INCOMPLETE) &&
        (!neighbor_cache[i].isrouter)) {
      if (neighbor_cache[i].counter.probes_sent >= time) {
        j = i;
        time = neighbor_cache[i].counter.probes_sent;
      }
    }
  }
  if (j >= 0) {
    nd6_free_neighbor_cache_entry(j);
    return j;
  }

  /* No more entries to try. */
  return -1;
}

/**
 * Will free any resources associated with a neighbor cache
 * entry, and will mark it as unused.
 *
 * @param i the neighbor cache entry index to free
 */
static void
nd6_free_neighbor_cache_entry(s8_t i)
{
    s8_t rt_indx = 0;
        
  if ((i < 0) || (i >= LWIP_ND6_NUM_NEIGHBORS)) {
    return;
  }
  rt_indx = nd6_find_default_router_by_neighbor(&(neighbor_cache[i]));
    if (rt_indx >= 0)
    {
        #ifdef DISCART_RT_WHEN_ITS_NCE_TIMEOUT
        nd6_disc_def_rt_list(rt_indx);
        #else
        neighbor_cache[i].state = ND6_NO_ENTRY;
        neighbor_cache[i].q = NULL;
        neighbor_cache[i].counter.probes_sent = 0;
        //neighbor_cache[i].netif = netif; 
        neighbor_cache[i].isrouter = 1;
        #endif
  }
  
  /* Free any queued packets. */
  if (neighbor_cache[i].q != NULL) {
    nd6_free_q(neighbor_cache[i].q);
    neighbor_cache[i].q = NULL;
  }

  neighbor_cache[i].state = ND6_NO_ENTRY;
  neighbor_cache[i].isrouter = 0;
  neighbor_cache[i].netif = NULL;
  neighbor_cache[i].counter.reachable_time = 0;
  neighbor_cache[i].retrans_interval = 0;
  ip6_addr_set_zero(&(neighbor_cache[i].next_hop_address));
  neighbor_cache[i].src_flag = 0;
}

/**
 * Search for a destination cache entry
 *
 * @param ip6addr the IPv6 address of the destination
 * @return The destination cache entry index that matched, -1 if no
 * entry is found
 */
static s8_t
nd6_find_destination_cache_entry(const ip6_addr_t * ip6addr)
{
  s8_t i;
  for (i = 0; i < LWIP_ND6_NUM_DESTINATIONS; i++) {
    if (ip6_addr_cmp(ip6addr, &(destination_cache[i].destination_addr))) {
      return i;
    }
  }
  return -1;
}

/**
 * Create a new destination cache entry. If no unused entry is found,
 * will recycle oldest entry.
 *
 * @return The destination cache entry index that was created, -1 if no
 * entry was created
 */
static s8_t
nd6_new_destination_cache_entry(void)
{
  s8_t i, j;
  u32_t age;

  /* Find an empty entry. */
  for (i = 0; i < LWIP_ND6_NUM_DESTINATIONS; i++) {
    if (ip6_addr_isany(&(destination_cache[i].destination_addr))) {
      return i;
    }
  }

  /* Find oldest entry. */
  age = 0;
  j = LWIP_ND6_NUM_DESTINATIONS - 1;
  for (i = 0; i < LWIP_ND6_NUM_DESTINATIONS; i++) {
    if (destination_cache[i].age > age) {
      j = i;
    }
  }

  return j;
}

/**
 * Determine whether an address matches an on-link prefix.
 *
 * @param ip6addr the IPv6 address to match
 * @return 1 if the address is on-link, 0 otherwise
 */
static s8_t
nd6_is_prefix_in_netif(const ip6_addr_t * ip6addr, struct netif * netif)
{
  s8_t i;
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; i++) {
    if ((prefix_list[i].netif == netif) &&
        (prefix_list[i].invalidation_timer > 0) &&
        !nd6_match_prefix(ip6addr, &(prefix_list[i])))
    {
      return 1;
    }
  }

  return 0;
}

/**
 * Select a default router for a destination.
 *
 * @param ip6addr the destination address
 * @param netif the netif for the outgoing packet, if known
 * @return the default router entry index, or -1 if no suitable
 *         router is found
 */
s8_t
nd6_select_router(const ip6_addr_t * ip6addr, struct netif * netif)
{
  s8_t i;
  /* last_router is used for round-robin router selection (as recommended
   * in RFC). This is more robust in case one router is not reachable,
   * we are not stuck trying to resolve it. */
  static s8_t last_router;
  (void)ip6addr; /* TODO match preferred routes!! (must implement ND6_OPTION_TYPE_ROUTE_INFO) */

  /* TODO: implement default router preference */

  /* Look for reachable routers. */
  for (i = 0; i < LWIP_ND6_NUM_ROUTERS; i++) {
    if (++last_router >= LWIP_ND6_NUM_ROUTERS) {
      last_router = 0;
    }
    if ((default_router_list[i].neighbor_entry != NULL) &&
        (netif != NULL ? netif == default_router_list[i].neighbor_entry->netif : 1) &&
        (default_router_list[i].invalidation_timer > 0) &&
        (default_router_list[i].neighbor_entry->state == ND6_REACHABLE)) {
      return i;
    }
  }

  /* Look for router in other reachability states, but still valid according to timer. */
  for (i = 0; i < LWIP_ND6_NUM_ROUTERS; i++) {
    if (++last_router >= LWIP_ND6_NUM_ROUTERS) {
      last_router = 0;
    }
    if ((default_router_list[i].neighbor_entry != NULL) &&
        (netif != NULL ? netif == default_router_list[i].neighbor_entry->netif : 1) &&
        (default_router_list[i].invalidation_timer > 0)) {
      return i;
    }
  }

  /* Look for any router for which we have any information at all. */
  for (i = 0; i < LWIP_ND6_NUM_ROUTERS; i++) {
    if (++last_router >= LWIP_ND6_NUM_ROUTERS) {
      last_router = 0;
    }
    if (default_router_list[i].neighbor_entry != NULL &&
        (netif != NULL ? netif == default_router_list[i].neighbor_entry->netif : 1)) {
      return i;
    }
  }

  /* no suitable router found. */
  return -1;
}

/**
 * Find an entry for a default router.
 *
 * @param router_addr the IPv6 address of the router
 * @param netif the netif on which the router is found, if known
 * @return the index of the router entry, or -1 if not found
 */
static s8_t
nd6_get_router(const ip6_addr_t * router_addr, struct netif * netif)
{
  s8_t i;

  /* Look for router. */
  for (i = 0; i < LWIP_ND6_NUM_ROUTERS; i++) {
    if ((default_router_list[i].neighbor_entry != NULL) &&
        ((netif != NULL) ? netif == default_router_list[i].neighbor_entry->netif : 1) &&
        ip6_addr_cmp(router_addr, &(default_router_list[i].neighbor_entry->next_hop_address))) {
      return i;
    }
  }

  /* router not found. */
  return -1;
}

/**
 * Create a new entry for a default router.
 *
 * @param router_addr the IPv6 address of the router
 * @param netif the netif on which the router is connected, if known
 * @return the index on the router table, or -1 if could not be created
 */
static s8_t
nd6_new_router(const ip6_addr_t * router_addr, struct netif * netif)
{
  s8_t router_index;
  s8_t neighbor_index;

  /* Do we have a neighbor entry for this router? */
  // neighbor_index = nd6_find_neighbor_cache_entry(router_addr);
  neighbor_index = nd6_find_nce(router_addr, netif);  

  if (neighbor_index < 0) {
    /* Create a neighbor entry for this router. */
    neighbor_index = nd6_new_neighbor_cache_entry();
    if (neighbor_index < 0) {
      /* Could not create neighbor entry for this router. */
      return -1;
    }
    ip6_addr_set(&(neighbor_cache[neighbor_index].next_hop_address), router_addr);
    neighbor_cache[neighbor_index].state = ND6_NO_ENTRY;
  }
  
  if (neighbor_cache[neighbor_index].state == ND6_NO_ENTRY) 
  {   
    neighbor_cache[neighbor_index].q = NULL;
    /* RFC4861 P54: If a NCE is created for the router, its 
     * reachability state MUST be set to STALE. */
    // neighbor_cache[neighbor_index].state = ND6_NO_ENTRY;
    // neighbor_cache[neighbor_index].state = ND6_STALE;
    neighbor_cache[neighbor_index].counter.probes_sent = 0;
  }

  neighbor_cache[neighbor_index].netif = netif;

  /* Mark neighbor as router. */
  neighbor_cache[neighbor_index].isrouter = 1;

  /* Look for empty entry. */
    for (router_index = 0; router_index < LWIP_ND6_NUM_ROUTERS; router_index++) 
    {
        if (!default_router_list[router_index].neighbor_entry ||
            !default_router_list[router_index].invalidation_timer) 
        {
      default_router_list[router_index].neighbor_entry = &(neighbor_cache[neighbor_index]);
      return router_index;
    }
  }

  /* Could not create a router entry. */

  /* Mark neighbor entry as not-router. Entry might be useful as neighbor still. */
  neighbor_cache[neighbor_index].isrouter = 0;

  /* router not found. */
  return -1;
}

/**
 * Find the cached entry for an on-link prefix.
 *
 * @param prefix the IPv6 prefix that is on-link
 * @param netif the netif on which the prefix is on-link
 * @return the index on the prefix table, or -1 if not found
 */
static s8_t
nd6_get_onlink_prefix(ip6_addr_t * prefix, u8_t prefix_len, struct netif * netif)
{
  s8_t i;

  /* Look for prefix in list. */
    for (i = 0; i < LWIP_ND6_NUM_PREFIXES; ++i) 
    {
        if ((prefix_list[i].prefix_len == prefix_len) &&
            (prefix_list[i].netif == netif) &&
            !nd6_match_prefix(prefix, &(prefix_list[i]))) 
        {
            return i;
        }
    }

    /* Entry not available. */
    return -1;
}


static s8_t
nd6_get_autocfg_prefix(ip6_addr_t * prefix, u8_t prefix_len, struct netif * netif)
{
    s8_t i;

    /* Look for prefix in list. */
    for (i = 0; i < LWIP_ND6_NUM_AUTOCFG_PREFIXES; i++) 
    {
        if ((autocfg_prefix_list[i].prefix_len == prefix_len) &&
            (autocfg_prefix_list[i].netif == netif) &&
            !nd6_match_prefix(prefix, &(autocfg_prefix_list[i]))) 
        {
      return i;
    }
  }
  /* Entry not available. */
  return -1;
}


/**
 * Creates a new entry for an on-link prefix.
 *
 * @param prefix the IPv6 prefix that is on-link
 * @param netif the netif on which the prefix is on-link
 * @return the index on the prefix table, or -1 if not created
 */
static s8_t
nd6_new_onlink_prefix(ip6_addr_t * prefix, u8_t prefix_len, struct netif * netif)
{
  s8_t i;

  /* Create new entry. */
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; ++i) {
    if ((prefix_list[i].netif == NULL) ||
        (prefix_list[i].invalidation_timer == 0)) {
      /* Found empty prefix entry. */
      prefix_list[i].netif = netif;
      ip6_addr_set(&(prefix_list[i].prefix), prefix);
#if LWIP_IPV6_AUTOCONFIG
      prefix_list[i].flags = 0;
#endif /* LWIP_IPV6_AUTOCONFIG */
      prefix_list[i].prefix_len = prefix_len;
      return i;
    }
  }

  /* Entry not available. */
  return -1;
}


/**
 * Creates a new entry for an auto addr cfg prefix.
 *
 * @param prefix the IPv6 prefix that is auto cfg
 * @param netif the netif on which the prefix is auto cfg
 * @return the index on the prefix table, or -1 if not created
 */
static s8_t
nd6_new_autocfg_prefix(ip6_addr_t * prefix, u8_t prefix_len, struct netif * netif)
{
  s8_t i;

  /* Create new entry. */
  for (i = 0; i < LWIP_ND6_NUM_AUTOCFG_PREFIXES; i++) 
  {
    if ((autocfg_prefix_list[i].netif == NULL) ||
        (autocfg_prefix_list[i].invalidation_timer == 0)) 
    {
      /* Found empty auto cfg prefix entry. */
      autocfg_prefix_list[i].netif = netif;
      ip6_addr_set(&(autocfg_prefix_list[i].prefix), prefix);
      autocfg_prefix_list[i].flags = 0;
      autocfg_prefix_list[i].prefix_len = prefix_len;
      return i;
    }
  }

  /* Entry not available. */
  return -1;
}


/**
 * Determine the next hop for a destination. Will determine if the
 * destination is on-link, else a suitable on-link router is selected.
 *
 * The last entry index is cached for fast entry search.
 *
 * @param ip6addr the destination address
 * @param netif the netif on which the packet will be sent
 * @return the neighbor cache entry for the next hop, ERR_RTE if no
 *         suitable next hop was found, ERR_MEM if no cache entry
 *         could be created
 */
s8_t
nd6_get_next_hop_entry(const ip6_addr_t * ip6addr, struct netif * netif)
{
  s8_t i;

#if LWIP_NETIF_HWADDRHINT
  if (netif->addr_hint != NULL) {
    /* per-pcb cached entry was given */
    u8_t addr_hint = *(netif->addr_hint);
    if (addr_hint < LWIP_ND6_NUM_DESTINATIONS) {
      nd6_cached_destination_index = addr_hint;
    }
  }
#endif /* LWIP_NETIF_HWADDRHINT */

  /* Look for ip6addr in destination cache. */
  if (ip6_addr_cmp(ip6addr, &(destination_cache[nd6_cached_destination_index].destination_addr))) {
    /* the cached entry index is the right one! */
    /* do nothing. */
    ND6_STATS_INC(nd6.cachehit);
  } else {
    /* Search destination cache. */
    i = nd6_find_destination_cache_entry(ip6addr);
    if (i >= 0) {
      /* found destination entry. make it our new cached index. */
      nd6_cached_destination_index = i;
    }
    else {
      /* Not found. Create a new destination entry. */
      i = nd6_new_destination_cache_entry();
      if (i >= 0) {
        /* got new destination entry. make it our new cached index. */
        nd6_cached_destination_index = i;
      } else {
        /* Could not create a destination cache entry. */
        return ERR_MEM;
      }

      /* Copy dest address to destination cache. */
      ip6_addr_set(&(destination_cache[nd6_cached_destination_index].destination_addr), ip6addr);

      /* Now find the next hop. is it a neighbor? */
      if (ip6_addr_islinklocal(ip6addr) ||
          nd6_is_prefix_in_netif(ip6addr, netif)) {
        /* Destination in local link. */
        destination_cache[nd6_cached_destination_index].pmtu = netif->mtu;
        ip6_addr_copy(destination_cache[nd6_cached_destination_index].next_hop_addr, destination_cache[nd6_cached_destination_index].destination_addr);
      }
      else {
        /* We need to select a router. */
        i = nd6_select_router(ip6addr, netif);
        if (i < 0) {
          /* No router found. */
          ip6_addr_set_any(&(destination_cache[nd6_cached_destination_index].destination_addr));
          return ERR_RTE;
        }
        if (i < LWIP_ND6_NUM_ROUTERS) {
          /*kal_prompt_trace(MOD_TCPIP, "Router selected:%d, %x %x", i, 
            ((char*)&(default_router_list[i].neighbor_entry->next_hop_address))[14], 
            ((char*)&(default_router_list[i].neighbor_entry->next_hop_address))[15]);*/
        }
        destination_cache[nd6_cached_destination_index].pmtu = netif->mtu; /* Start with netif mtu, correct through ICMPv6 if necessary */
        ip6_addr_copy(destination_cache[nd6_cached_destination_index].next_hop_addr, default_router_list[i].neighbor_entry->next_hop_address);
      }
    }
  }

#if LWIP_NETIF_HWADDRHINT
  if (netif->addr_hint != NULL) {
    /* per-pcb cached entry was given */
    *(netif->addr_hint) = nd6_cached_destination_index;
  }
#endif /* LWIP_NETIF_HWADDRHINT */

  /* Look in neighbor cache for the next-hop address. */
  if (ip6_addr_cmp(&(destination_cache[nd6_cached_destination_index].next_hop_addr),
                   &(neighbor_cache[nd6_cached_neighbor_index].next_hop_address))) {
    /* Cache hit. */
    /* Do nothing. */
    ND6_STATS_INC(nd6.cachehit);
  } else {
    i = nd6_find_neighbor_cache_entry(&(destination_cache[nd6_cached_destination_index].next_hop_addr));
    if (i >= 0) {
      /* Found a matching record, make it new cached entry. */
      nd6_cached_neighbor_index = i;
    }
    else {
      /* Neighbor not in cache. Make a new entry. */
      i = nd6_new_neighbor_cache_entry();
      if (i >= 0) {
        /* got new neighbor entry. make it our new cached index. */
        nd6_cached_neighbor_index = i;
      } else {
        /* Could not create a neighbor cache entry. */
        return ERR_MEM;
      }

      /* Initialize fields. */
      ip6_addr_copy(neighbor_cache[i].next_hop_address,
                   destination_cache[nd6_cached_destination_index].next_hop_addr);
      neighbor_cache[i].isrouter = 0;
      neighbor_cache[i].netif = netif;
      neighbor_cache[i].state = ND6_INCOMPLETE;
      neighbor_cache[i].counter.probes_sent = 0;
    }
  }

  /* Reset this destination's age. */
  destination_cache[nd6_cached_destination_index].age = 0;

  return nd6_cached_neighbor_index;
}

/**
 * Queue a packet for a neighbor.
 *
 * @param neighbor_index the index in the neighbor cache table
 * @param q packet to be queued
 * @return ERR_OK if succeeded, ERR_MEM if out of memory
 */
err_t
nd6_queue_packet(s8_t neighbor_index, struct pbuf * q)
{
  err_t result = ERR_MEM;
  struct pbuf *p;
  int copy_needed = 0;
#if LWIP_ND6_QUEUEING
  struct nd6_q_entry *new_entry, *r;
#endif /* LWIP_ND6_QUEUEING */

  if ((neighbor_index < 0) || (neighbor_index >= LWIP_ND6_NUM_NEIGHBORS)) {
    return ERR_ARG;
  }

  /* IF q includes a PBUF_REF, PBUF_POOL or PBUF_RAM, we have no choice but
   * to copy the whole queue into a new PBUF_RAM (see bug #11400)
   * PBUF_ROMs can be left as they are, since ROM must not get changed. */
  p = q;
  while (p) {
    if(p->type != PBUF_ROM) {
      copy_needed = 1;
      break;
    }
    p = p->next;
  }
  if(copy_needed) {
    /* copy the whole packet into new pbufs */
    p = pbuf_alloc(PBUF_LINK, q->tot_len, PBUF_RAM);
    while ((p == NULL) && (neighbor_cache[neighbor_index].q != NULL)) {
      /* Free oldest packet (as per RFC recommendation) */
#if LWIP_ND6_QUEUEING
      r = neighbor_cache[neighbor_index].q;
      neighbor_cache[neighbor_index].q = r->next;
      r->next = NULL;
      nd6_free_q(r);
#else /* LWIP_ND6_QUEUEING */
      pbuf_free(neighbor_cache[neighbor_index].q);
      neighbor_cache[neighbor_index].q = NULL;
#endif /* LWIP_ND6_QUEUEING */
      p = pbuf_alloc(PBUF_LINK, q->tot_len, PBUF_RAM);
    }
    if(p != NULL) {
      if (pbuf_copy(p, q) != ERR_OK) {
        pbuf_free(p);
        p = NULL;
      }
    }
  } else {
    /* referencing the old pbuf is enough */
    p = q;
    pbuf_ref(p);
  }
  /* packet was copied/ref'd? */
  if (p != NULL) {
    /* queue packet ... */
#if LWIP_ND6_QUEUEING
    /* allocate a new nd6 queue entry */
    new_entry = (struct nd6_q_entry *)memp_malloc(MEMP_ND6_QUEUE);
    if ((new_entry == NULL) && (neighbor_cache[neighbor_index].q != NULL)) {
      /* Free oldest packet (as per RFC recommendation) */
      r = neighbor_cache[neighbor_index].q;
      neighbor_cache[neighbor_index].q = r->next;
      r->next = NULL;
      nd6_free_q(r);
      new_entry = (struct nd6_q_entry *)memp_malloc(MEMP_ND6_QUEUE);
    }
    if (new_entry != NULL) {
      new_entry->next = NULL;
      new_entry->p = p;
      if(neighbor_cache[neighbor_index].q != NULL) {
        /* queue was already existent, append the new entry to the end */
        r = neighbor_cache[neighbor_index].q;
        while (r->next != NULL) {
          r = r->next;
        }
        r->next = new_entry;
      } else {
        /* queue did not exist, first item in queue */
        neighbor_cache[neighbor_index].q = new_entry;
      }
      LWIP_DEBUGF(LWIP_DBG_TRACE, ("ipv6: queued packet %p on neighbor entry %"S16_F"\n", (void *)p, (s16_t)neighbor_index));
      result = ERR_OK;
    } else {
      /* the pool MEMP_ND6_QUEUE is empty */
      pbuf_free(p);
      LWIP_DEBUGF(LWIP_DBG_TRACE, ("ipv6: could not queue a copy of packet %p (out of memory)\n", (void *)p));
      /* { result == ERR_MEM } through initialization */
    }
#else /* LWIP_ND6_QUEUEING */
    /* Queue a single packet. If an older packet is already queued, free it as per RFC. */
    if (neighbor_cache[neighbor_index].q != NULL) {
      pbuf_free(neighbor_cache[neighbor_index].q);
    }
    neighbor_cache[neighbor_index].q = p;
    LWIP_DEBUGF(LWIP_DBG_TRACE, ("ipv6: queued packet %p on neighbor entry %"S16_F"\n", (void *)p, (s16_t)neighbor_index));
    result = ERR_OK;
#endif /* LWIP_ND6_QUEUEING */
  } else {
    LWIP_DEBUGF(LWIP_DBG_TRACE, ("ipv6: could not queue a copy of packet %p (out of memory)\n", (void *)q));
    /* { result == ERR_MEM } through initialization */
  }

  return result;
}

#if LWIP_ND6_QUEUEING
/**
 * Free a complete queue of nd6 q entries
 *
 * @param q a queue of nd6_q_entry to free
 */
static void
nd6_free_q(struct nd6_q_entry *q)
{
  struct nd6_q_entry *r;
  LWIP_ASSERT("q != NULL", q != NULL);
  LWIP_ASSERT("q->p != NULL", q->p != NULL);
  while (q) {
    r = q;
    q = q->next;
    LWIP_ASSERT("r->p != NULL", (r->p != NULL));
    pbuf_free(r->p);
    memp_free(MEMP_ND6_QUEUE, r);
  }
}
#endif /* LWIP_ND6_QUEUEING */

/**
 * Send queued packets for a neighbor
 *
 * @param i the neighbor to send packets to
 */
static void
nd6_send_q(s8_t i)
{
  struct ip6_hdr *ip6hdr;
#if LWIP_ND6_QUEUEING
  struct nd6_q_entry *q;
#endif /* LWIP_ND6_QUEUEING */

  if ((i < 0) || (i >= LWIP_ND6_NUM_NEIGHBORS)) {
    return;
  }

#if LWIP_ND6_QUEUEING
  while (neighbor_cache[i].q != NULL) {
    /* remember first in queue */
    q = neighbor_cache[i].q;
    /* pop first item off the queue */
    neighbor_cache[i].q = q->next;
    /* Get ipv6 header. */
    ip6hdr = (struct ip6_hdr *)(q->p->payload);
    /* Override ip6_current_dest_addr() so that we have an aligned copy. */
    ip6_addr_set(ip6_current_dest_addr(), &(ip6hdr->dest));
    /* send the queued IPv6 packet */
    (neighbor_cache[i].netif)->output_ip6(neighbor_cache[i].netif, q->p, ip6_current_dest_addr());
    /* free the queued IP packet */
    pbuf_free(q->p);
    /* now queue entry can be freed */
    memp_free(MEMP_ND6_QUEUE, q);
  }
#else /* LWIP_ND6_QUEUEING */
  if (neighbor_cache[i].q != NULL) {
    /* Get ipv6 header. */
    ip6hdr = (struct ip6_hdr *)(neighbor_cache[i].q->payload);
    /* Override ip6_current_dest_addr() so that we have an aligned copy. */
    ip6_addr_set(ip6_current_dest_addr(), &(ip6hdr->dest));
    /* send the queued IPv6 packet */
    (neighbor_cache[i].netif)->output_ip6(neighbor_cache[i].netif, neighbor_cache[i].q, ip6_current_dest_addr());
    /* free the queued IP packet */
    pbuf_free(neighbor_cache[i].q);
    neighbor_cache[i].q = NULL;
  }
#endif /* LWIP_ND6_QUEUEING */
}


/**
 * Get the Path MTU for a destination.
 *
 * @param ip6addr the destination address
 * @param netif the netif on which the packet will be sent
 * @return the Path MTU, if known, or the netif default MTU
 */
u16_t
nd6_get_destination_mtu(const ip6_addr_t * ip6addr, struct netif * netif)
{
  s8_t i;

  i = nd6_find_destination_cache_entry(ip6addr);
  if (i >= 0) {
    if (destination_cache[i].pmtu > 0 && destination_cache[i].pmtu < netif->mtu) {
      return destination_cache[i].pmtu;
    }
  }

  if (netif != NULL && netif->mtu > 0) {
    return netif->mtu;
  }

  return 1280; /* Minimum MTU */
}


#if LWIP_ND6_TCP_REACHABILITY_HINTS
/**
 * Provide the Neighbor discovery process with a hint that a
 * destination is reachable. Called by tcp_receive when ACKs are
 * received or sent (as per RFC). This is useful to avoid sending
 * NS messages every 30 seconds.
 *
 * @param ip6addr the destination address which is know to be reachable
 *                by an upper layer protocol (TCP)
 */
void
nd6_reachability_hint(const ip6_addr_t * ip6addr)
{
  s8_t i;

  /* Find destination in cache. */
  if (ip6_addr_cmp(ip6addr, &(destination_cache[nd6_cached_destination_index].destination_addr))) {
    i = nd6_cached_destination_index;
    ND6_STATS_INC(nd6.cachehit);
  }
  else {
    i = nd6_find_destination_cache_entry(ip6addr);
  }
  if (i < 0) {
    return;
  }

  /* Find next hop neighbor in cache. */
  if (ip6_addr_cmp(&(destination_cache[i].next_hop_addr), &(neighbor_cache[nd6_cached_neighbor_index].next_hop_address))) {
    i = nd6_cached_neighbor_index;
    ND6_STATS_INC(nd6.cachehit);
  }
  else {
    i = nd6_find_neighbor_cache_entry(&(destination_cache[i].next_hop_addr));
  }
  if (i < 0) {
    return;
  }

  /* For safety: don't set as reachable if we don't have a LL address yet. Misuse protection. */
  if (neighbor_cache[i].state == ND6_INCOMPLETE || neighbor_cache[i].state == ND6_NO_ENTRY) {
    return;
  }

  /* Set reachability state. */
  neighbor_cache[i].state = ND6_REACHABLE;
  neighbor_cache[i].counter.reachable_time = reachable_time;
}
#endif /* LWIP_ND6_TCP_REACHABILITY_HINTS */

/* Clear all prefix associated to the netif */
void nd6_clear_onlink_prefix_by_netif(struct netif* inp)
{
    s8_t i;
    for (i = 0; i < LWIP_ND6_NUM_PREFIXES; i++)
    {
        if (prefix_list[i].netif == inp)
        {
            // TODO: Why netif remained?
            memset(&(prefix_list[i]), 0, sizeof(struct nd6_prefix_list_entry));
            prefix_list[i].netif = inp;
        }
    }
}


/* Clear all prefix associated to the netif */
void nd6_clear_autocfg_prefix_by_netif(struct netif* inp)
{
    s8_t i;
    for (i = 0; i < LWIP_ND6_NUM_AUTOCFG_PREFIXES; i++)
    {
        if (autocfg_prefix_list[i].netif == inp)
        {
            // TODO: Why netif remained?
            memset(&(autocfg_prefix_list[i]), 0, sizeof(struct nd6_prefix_list_entry));
            autocfg_prefix_list[i].netif = inp;
        }
    }
}



s8_t nd6_find_prefix_by_netif(struct netif* inp)
{
  s8_t i = 0;
  for (i = 0; i < LWIP_ND6_NUM_PREFIXES; i++) {
    if (prefix_list[i].netif == inp)
        return i;
  }
  return -1;
}


void nd6_reset_prefix(s8_t index, struct netif* inp)
{
  memset(&(prefix_list[index]), 0, sizeof(struct nd6_prefix_list_entry));
  prefix_list[index].netif = inp;
#if 0    
  prefix_list[index].flags = 0;
  prefix_list[index].invalidation_timer = 0;   
  prefix_list[index].netif = inp;
  memset(&(prefix_list[index].prefix), 0, sizeof(ip6_addr_t));  
#endif  
}


s8_t nd6_find_autocfg_prefix_by_netif(struct netif* inp)
{
  s8_t i = 0;
  for (i = 0; i < LWIP_ND6_NUM_AUTOCFG_PREFIXES; i++) {
    if (autocfg_prefix_list[i].netif == inp)
        return i;
  }
  return -1;
}


void nd6_reset_autocfg_prefix(s8_t index, struct netif* inp)
{
  memset(&(autocfg_prefix_list[index]), 0, sizeof(struct nd6_prefix_list_entry));
  autocfg_prefix_list[index].netif = inp;
}


void nd6_reset_cache(void)
{
  memset(&neighbor_cache, 0, sizeof(struct nd6_neighbor_cache_entry) * LWIP_ND6_NUM_NEIGHBORS);
  memset(&destination_cache, 0, sizeof(struct nd6_destination_cache_entry) * LWIP_ND6_NUM_DESTINATIONS);
  memset(&prefix_list, 0, sizeof(struct nd6_prefix_list_entry)* LWIP_ND6_NUM_PREFIXES);
  memset(&autocfg_prefix_list, 0, sizeof(struct nd6_prefix_list_entry)* LWIP_ND6_NUM_AUTOCFG_PREFIXES);
  memset(&default_router_list, 0, sizeof(struct nd6_router_list_entry) * LWIP_ND6_NUM_ROUTERS);
}

u8_t nd6_get_cur_hop_limit(void)
{
  return current_hop_limit;
}


s8_t nd6_find_default_router_by_neighbor(struct nd6_neighbor_cache_entry * neighbor)
{
  s8_t j = -1;
    
  if (neighbor->isrouter) {
    for (j = 0; j < LWIP_ND6_NUM_ROUTERS; j++) {
      if (default_router_list[j].neighbor_entry &&
        default_router_list[j].neighbor_entry == neighbor) {
        break;
      }
    }
  }

  return j == LWIP_ND6_NUM_ROUTERS ? -1 : j;
}


static s8_t nd6_match_prefix(const ip6_addr_t *addr, struct nd6_prefix_list_entry *prefix)
{
  s8_t remain_bits, i;
  u8_t max_4bytes;
  u8_t max_index;
  u32_t mark_bits = 0xFFFFFFFF;
    
  if (addr && prefix) {
    LWIP_ASSERT("prefix->prefix_len out of range: [0, 128]", (prefix->prefix_len >= 0 && prefix->prefix_len <= 128));

    max_4bytes = prefix->prefix_len / 32; /* 8*4 */
    remain_bits = prefix->prefix_len % 32;
    max_index = (max_4bytes < 4) ? max_4bytes : 4;
    for (i = 0; i < max_index; i++) {
      if (addr->addr[i] != prefix->prefix.addr[i]) {
        return -1;
      }
    }

    if (remain_bits) {
      mark_bits >>= (32 - remain_bits);
      mark_bits <<= (32 - remain_bits);

      if ((addr->addr[i] & mark_bits) != (prefix->prefix.addr[i] & mark_bits))  {
        return -1;
      }
    }

    return 0;        
  }    
  return -1;
}


static void nd6_trace_addr(char *addr)
{
#if 0
  //kal_prompt_trace(MOD_TCPIP, "Address: %x %x %x %x %x %x %x %x",
                    addr[0], addr[1], addr[2], addr[3],
                    addr[4], addr[5], addr[6], addr[7],
                    addr[8], addr[9], addr[10], addr[11],
                    addr[12], addr[13], addr[14], addr[15]);
#endif
}
#endif /* LWIP_IPV6 */
