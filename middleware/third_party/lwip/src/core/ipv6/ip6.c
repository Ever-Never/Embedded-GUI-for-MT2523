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
 * IPv6 layer.
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

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/netif.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/ip6_frag.h"
#include "lwip/icmp6.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcp_impl.h"
#include "lwip/dhcp6.h"
#include "lwip/nd6.h"
#include "lwip/mld6.h"
#include "lwip/debug.h"
#include "lwip/stats.h"

#if !LWIP_IPV4
/** Global data for IPv6 only */
struct ip_globals ip_data;
#endif /* !LWIP_IPV4 */

/**
 * Finds the appropriate network interface for a given IPv6 address. It tries to select
 * a netif following a sequence of heuristics:
 * 1) if there is only 1 netif, return it
 * 2) if the destination is a link-local address, try to match the src address to a netif.
 *    this is a tricky case because with multiple netifs, link-local addresses only have
 *    meaning within a particular subnet/link.
 * 3) tries to match the destination subnet to a configured address
 * 4) tries to find a router
 * 5) tries to match the source address to the netif
 * 6) returns the default netif, if configured
 *
 * @param src the source IPv6 address, if known
 * @param dest the destination IPv6 address for which to find the route
 * @return the netif on which to send to reach dest
 */
struct netif *
ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest)
{
  struct netif *netif;
  s8_t i;

#ifdef LWIP_HOOK_IP6_ROUTE
  netif = LWIP_HOOK_IP6_ROUTE(src, dest);
  if (netif != NULL) {
    return netif;
  }
#endif

  /* If single netif configuration, fast return. */
  if ((netif_list != NULL) && (netif_list->next == NULL)) {
    if (!netif_is_up(netif_list) || !netif_is_link_up(netif_list)) {
      return NULL;
    }
    return netif_list;
  }

  /* Special processing for link-local addresses. */
  if (ip6_addr_islinklocal(dest)) {
    if (ip6_addr_isany(src)) {
      /* Use default netif, if Up. */
      if (!netif_is_up(netif_default) || !netif_is_link_up(netif_default)) {
        return NULL;
      }
      return netif_default;
    }

    /* Try to find the netif for the source address, checking that link is up. */
    for(netif = netif_list; netif != NULL; netif = netif->next) {
      if (!netif_is_up(netif) || !netif_is_link_up(netif)) {
        continue;
      }
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
            ip6_addr_cmp(src, netif_ip6_addr(netif, i))) {
          return netif;
        }
      }
    }

    /* netif not found, use default netif, if up */
    if (!netif_is_up(netif_default) || !netif_is_link_up(netif_default)) {
      return NULL;
    }
    return netif_default;
  }

  /* See if the destination subnet matches a configured address. */
  for(netif = netif_list; netif != NULL; netif = netif->next) {
    if (!netif_is_up(netif) || !netif_is_link_up(netif)) {
      continue;
    }
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_netcmp(dest, netif_ip6_addr(netif, i))) {
        return netif;
      }
    }
  }

  /* Get the netif for a suitable router. */
  i = nd6_select_router(dest, NULL);
  if (i >= 0) {
    if (default_router_list[i].neighbor_entry != NULL) {
      if (default_router_list[i].neighbor_entry->netif != NULL) {
        if (netif_is_up(default_router_list[i].neighbor_entry->netif) && netif_is_link_up(default_router_list[i].neighbor_entry->netif)) {
          return default_router_list[i].neighbor_entry->netif;
        }
      }
    }
  }

  /* try with the netif that matches the source address. */
  if (!ip6_addr_isany(src)) {
    for(netif = netif_list; netif != NULL; netif = netif->next) {
      if (!netif_is_up(netif) || !netif_is_link_up(netif)) {
        continue;
      }
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
            ip6_addr_cmp(src, netif_ip6_addr(netif, i))) {
          return netif;
        }
      }
    }
  }

#if LWIP_NETIF_LOOPBACK && !LWIP_HAVE_LOOPIF
  /* loopif is disabled, loopback traffic is passed through any netif */
  if (ip6_addr_isloopback(dest)) {
    /* don't check for link on loopback traffic */
    if (netif_is_up(netif_default)) {
      return netif_default;
    }
    /* default netif is not up, just use any netif for loopback traffic */
    for (netif = netif_list; netif != NULL; netif = netif->next) {
      if (netif_is_up(netif)) {
        return netif;
      }
    }
    return NULL;
  }
#endif /* LWIP_NETIF_LOOPBACK && !LWIP_HAVE_LOOPIF */

  /* no matching netif found, use default netif, if up */
  if (!netif_is_up(netif_default) || !netif_is_link_up(netif_default)) {
    return NULL;
  }
  return netif_default;
}

/**
 * Select the best IPv6 source address for a given destination
 * IPv6 address. Loosely follows RFC 3484. "Strong host" behavior
 * is assumed.
 *
 * @param netif the netif on which to send a packet
 * @param dest the destination we are trying to reach
 * @return the most suitable source address to use, or NULL if no suitable
 *         source address is found
 */
ip6_addr_t *
ip6_select_source_address(struct netif *netif, const ip6_addr_t * dest)
{
  ip6_addr_t * src = NULL;
  u8_t i;

  /* If dest is link-local, choose a link-local source. */
  if (ip6_addr_islinklocal(dest) || ip6_addr_ismulticast_linklocal(dest) || ip6_addr_ismulticast_iflocal(dest)) {
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_islinklocal(netif_ip6_addr(netif, i))) {
        return netif_ip6_addr(netif, i);
      }
    }
  }

  /* Choose a site-local with matching prefix. */
  if (ip6_addr_issitelocal(dest) || ip6_addr_ismulticast_sitelocal(dest)) {
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_issitelocal(netif_ip6_addr(netif, i)) &&
          ip6_addr_netcmp(dest, netif_ip6_addr(netif, i))) {
        return netif_ip6_addr(netif, i);
      }
    }
  }

  /* Choose a unique-local with matching prefix. */
  if (ip6_addr_isuniquelocal(dest) || ip6_addr_ismulticast_orglocal(dest)) {
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_isuniquelocal(netif_ip6_addr(netif, i)) &&
          ip6_addr_netcmp(dest, netif_ip6_addr(netif, i))) {
        return netif_ip6_addr(netif, i);
      }
    }
  }

  /* Choose a global with best matching prefix. */
  if (ip6_addr_isglobal(dest) || ip6_addr_ismulticast_global(dest)) {
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_isglobal(netif_ip6_addr(netif, i))) {
        if (src == NULL) {
          src = netif_ip6_addr(netif, i);
        }
        else {
          /* Replace src only if we find a prefix match. */
          /* TODO find longest matching prefix. */
          if ((!(ip6_addr_netcmp(src, dest))) &&
              ip6_addr_netcmp(netif_ip6_addr(netif, i), dest)) {
            src = netif_ip6_addr(netif, i);
          }
        }
      }
    }
    if (src != NULL) {
      return src;
    }
  }

  /* Last resort: see if arbitrary prefix matches. */
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
        ip6_addr_netcmp(dest, netif_ip6_addr(netif, i))) {
      return netif_ip6_addr(netif, i);
    }
  }

  return NULL;
}

#if LWIP_IPV6_FORWARD
/**
 * Forwards an IPv6 packet. It finds an appropriate route for the
 * packet, decrements the HL value of the packet, and outputs
 * the packet on the appropriate interface.
 *
 * @param p the packet to forward (p->payload points to IP header)
 * @param iphdr the IPv6 header of the input packet
 * @param inp the netif on which this packet was received
 */
static void
ip6_forward(struct pbuf *p, struct ip6_hdr *iphdr, struct netif *inp)
{
  struct netif *netif;

  /* do not forward link-local addresses */
  if (ip6_addr_islinklocal(ip6_current_dest_addr())) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: not forwarding link-local address.\n"));
    IP6_STATS_INC(ip6.rterr);
    IP6_STATS_INC(ip6.drop);
    return;
  }

  /* Find network interface where to forward this IP packet to. */
  netif = ip6_route(IP6_ADDR_ANY6, ip6_current_dest_addr());
  if (netif == NULL) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: no route for %"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F"\n",
        IP6_ADDR_BLOCK1(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK2(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK3(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK4(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK5(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK6(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK7(ip6_current_dest_addr()),
        IP6_ADDR_BLOCK8(ip6_current_dest_addr())));
#if LWIP_ICMP6
    /* Don't send ICMP messages in response to ICMP messages */
    if (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6) {
      icmp6_dest_unreach(p, ICMP6_DUR_NO_ROUTE);
    }
#endif /* LWIP_ICMP6 */
    IP6_STATS_INC(ip6.rterr);
    IP6_STATS_INC(ip6.drop);
    return;
  }
  /* Do not forward packets onto the same network interface on which
   * they arrived. */
  if (netif == inp) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: not bouncing packets back on incoming interface.\n"));
    IP6_STATS_INC(ip6.rterr);
    IP6_STATS_INC(ip6.drop);
    return;
  }

  /* decrement HL */
  IP6H_HOPLIM_SET(iphdr, IP6H_HOPLIM(iphdr) - 1);
  /* send ICMP6 if HL == 0 */
  if (IP6H_HOPLIM(iphdr) == 0) {
#if LWIP_ICMP6
    /* Don't send ICMP messages in response to ICMP messages */
    if (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6) {
      icmp6_time_exceeded(p, ICMP6_TE_HL);
    }
#endif /* LWIP_ICMP6 */
    IP6_STATS_INC(ip6.drop);
    return;
  }

  if (netif->mtu && (p->tot_len > netif->mtu)) {
#if LWIP_ICMP6
    /* Don't send ICMP messages in response to ICMP messages */
    if (IP6H_NEXTH(iphdr) != IP6_NEXTH_ICMP6) {
      icmp6_packet_too_big(p, netif->mtu);
    }
#endif /* LWIP_ICMP6 */
    IP6_STATS_INC(ip6.drop);
    return;
  }

  LWIP_DEBUGF(IP6_DEBUG, ("ip6_forward: forwarding packet to %"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F"\n",
      IP6_ADDR_BLOCK1(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK2(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK3(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK4(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK5(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK6(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK7(ip6_current_dest_addr()),
      IP6_ADDR_BLOCK8(ip6_current_dest_addr())));

  /* transmit pbuf on chosen interface */
  netif->output_ip6(netif, p, ip6_current_dest_addr());
  IP6_STATS_INC(ip6.fw);
  IP6_STATS_INC(ip6.xmit);
  return;
}
#endif /* LWIP_IPV6_FORWARD */


/**
 * This function is called by the network interface device driver when
 * an IPv6 packet is received. The function does the basic checks of the
 * IP header such as packet size being at least larger than the header
 * size etc. If the packet was not destined for us, the packet is
 * forwarded (using ip6_forward).
 *
 * Finally, the packet is sent to the upper layer protocol input function.
 *
 * @param p the received IPv6 packet (p->payload points to IPv6 header)
 * @param inp the netif on which this packet was received
 * @return ERR_OK if the packet was processed (could return ERR_* if it wasn't
 *         processed, but currently always returns ERR_OK)
 */
err_t
ip6_input(struct pbuf *p, struct netif *inp)
{
  struct ip6_hdr *ip6hdr;
  struct netif *netif;
  u8_t nexth;
  u16_t hlen; /* the current header length */
  u8_t i;
#if 0 /*IP_ACCEPT_LINK_LAYER_ADDRESSING*/
  @todo
  int check_ip_src=1;
#endif /* IP_ACCEPT_LINK_LAYER_ADDRESSING */

  /* For extension header */
  u32_t plen; // payload length in IPv6 header
  u32_t rtalert = ~0;
  s32_t proto = 0;
  u8_t nnexth; // next to next head
  u8_t has_exthead = 0; 
  /* For extension header */

  IP6_STATS_INC(ip6.recv);

  /* identify the IP header */
  ip6hdr = (struct ip6_hdr *)p->payload;
  if (IP6H_V(ip6hdr) != 6) {
    LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_WARNING, ("IPv6 packet dropped due to bad version number %"U32_F"\n",
        IP6H_V(ip6hdr)));
    pbuf_free(p);
    IP6_STATS_INC(ip6.err);
    IP6_STATS_INC(ip6.drop);
    return ERR_OK;
  }

#ifdef LWIP_HOOK_IP6_INPUT
  if (LWIP_HOOK_IP6_INPUT(p, inp)) {
    /* the packet has been eaten */
    return ERR_OK;
  }
#endif

  /* header length exceeds first pbuf length, or ip length exceeds total pbuf length? */
  if ((IP6_HLEN > p->len) || ((IP6H_PLEN(ip6hdr) + IP6_HLEN) > p->tot_len)) {
    if (IP6_HLEN > p->len) {
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
        ("IPv6 header (len %"U16_F") does not fit in first pbuf (len %"U16_F"), IP packet dropped.\n",
            IP6_HLEN, p->len));
    }
    if ((IP6H_PLEN(ip6hdr) + IP6_HLEN) > p->tot_len) {
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
        ("IPv6 (plen %"U16_F") is longer than pbuf (len %"U16_F"), IP packet dropped.\n",
            IP6H_PLEN(ip6hdr) + IP6_HLEN, p->tot_len));
    }
    /* free (drop) packet pbufs */
    pbuf_free(p);
    IP6_STATS_INC(ip6.lenerr);
    IP6_STATS_INC(ip6.drop);
    return ERR_OK;
  }

  /* Trim pbuf. This should have been done at the netif layer,
   * but we'll do it anyway just to be sure that its done. */
  pbuf_realloc(p, IP6_HLEN + IP6H_PLEN(ip6hdr));

  /* copy IP addresses to aligned ip6_addr_t */
  ip_addr_copy_from_ip6(ip_data.current_iphdr_dest, ip6hdr->dest);
  ip_addr_copy_from_ip6(ip_data.current_iphdr_src, ip6hdr->src);

  /* current header pointer. */
  ip_data.current_ip6_header = ip6hdr;

  /* In netif, used in case we need to send ICMPv6 packets back. */
  ip_data.current_netif = inp;
  ip_data.current_input_netif = inp;

  /* match packet against an interface, i.e. is this packet for us? */
  if (ip6_addr_ismulticast(ip6_current_dest_addr())) {
    /* Always joined to multicast if-local and link-local all-nodes group. */
    if (ip6_addr_isallnodes_iflocal(ip6_current_dest_addr()) ||
        ip6_addr_isallnodes_linklocal(ip6_current_dest_addr())) {
      netif = inp;
    }
#if LWIP_IPV6_MLD
    else if (mld6_lookfor_group(inp, ip6_current_dest_addr())) {
      netif = inp;
    }
#else /* LWIP_IPV6_MLD */
    else if (ip6_addr_issolicitednode(ip6_current_dest_addr())) {
      /* Filter solicited node packets when MLD is not enabled
       * (for Neighbor discovery). */
      netif = NULL;
      for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(inp, i)) &&
            ip6_addr_cmp_solicitednode(ip6_current_dest_addr(), netif_ip6_addr(inp, i))) {
          netif = inp;
          LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: solicited node packet accepted on interface %c%c\n",
              netif->name[0], netif->name[1]));
          break;
        }
      }
    }
#endif /* LWIP_IPV6_MLD */
    else {
      netif = NULL;
    }
  }
  else {
    /* start trying with inp. if that's not acceptable, start walking the
       list of configured netifs.
       'first' is used as a boolean to mark whether we started walking the list */
    int first = 1;
    netif = inp;
    do {
      /* interface is up? */
      if (netif_is_up(netif)) {
        /* unicast to this interface address? address configured? */
        for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
          if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
              ip6_addr_cmp(ip6_current_dest_addr(), netif_ip6_addr(netif, i))) {
            /* exit outer loop */
            goto netif_found;
          }
        }
      }
      if (ip6_addr_islinklocal(ip6_current_dest_addr())) {
        /* Do not match link-local addresses to other netifs. */
        netif = NULL;
        break;
      }
      if (first) {
        first = 0;
        netif = netif_list;
      } else {
        netif = netif->next;
      }
      if (netif == inp) {
        netif = netif->next;
      }
    } while(netif != NULL);
netif_found:
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet accepted on interface %c%c\n",
        netif ? netif->name[0] : 'X', netif? netif->name[1] : 'X'));
  }

  /* "::" packet source address? (used in duplicate address detection) */
  if (ip6_addr_isany(ip6_current_src_addr()) &&
      (!ip6_addr_issolicitednode(ip6_current_dest_addr()))) {
    /* packet source is not valid */
    /* free (drop) packet pbufs */
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with src ANY_ADDRESS dropped\n"));
    pbuf_free(p);
    IP6_STATS_INC(ip6.drop);
    goto ip6_input_cleanup;
  }

  if(ip6_addr_isloopback(ip6_current_dest_addr())) {

    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_cmp(ip6_current_src_addr(), netif_ip6_addr(netif, i))) {
        /* exit outer loop */
        break;
      }
    }
    if(i >= LWIP_IPV6_NUM_ADDRESSES) {
        LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with dest loopback but src addr is not mine dropped\n"));
        pbuf_free(p);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
    }
  }

  /* packet not for us? */
  if (netif == NULL) {
    /* packet not for us, route or discard */
    LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_TRACE, ("ip6_input: packet not for us.\n"));
#if LWIP_IPV6_FORWARD
    /* non-multicast packet? */
    if (!ip6_addr_ismulticast(ip6_current_dest_addr())) {
      /* try to forward IP packet on (other) interfaces */
      ip6_forward(p, ip6hdr, inp);
    }
#endif /* LWIP_IPV6_FORWARD */
    pbuf_free(p);
    goto ip6_input_cleanup;
  }

  /* current netif pointer. */
  ip_data.current_netif = netif;

  /* Save next header type. */
  nexth = IP6H_NEXTH(ip6hdr);

  /* Save payload length */  
  plen = IP6H_PLEN(ip6hdr);

  /* Init header length. */
  hlen = ip_data.current_ip_header_tot_len = IP6_HLEN;

  /* Move to payload. */
  pbuf_header(p, -IP6_HLEN);

  /* Cases need to handle
    1)  Next header is TCP or normal header (Pass)
    2)  Next header is NONE (Pass)
    3)  Next header is error type (Pass)
    4)  Next header is extension header
        4.1)  Extension header's next header is TCP or normal header
        4.2)  Extension header's next header is NONE
        4.3)  Extension header's next header is error type */
  /* Process known option extension headers, if present. */
  while (nexth != IP6_NEXTH_NONE)
  {
    switch (nexth) {
    case IP6_NEXTH_HOPBYHOP:
    case IP6_NEXTH_ROUTING:   
    case IP6_NEXTH_FRAGMENT:  
    case IP6_NEXTH_DESTOPTS: 
        {                        
            has_exthead = 1;
            
            /* Get next to next header type. */
            nnexth = *((u8_t *)p->payload);    
            switch (nnexth) {
            case IP6_NEXTH_HOPBYHOP:  
                /* hop-by-hop option header must follow with IPv6 header */
                /* p points to IPv6 header again. */
                pbuf_header(p, ip_data.current_ip_header_tot_len);       
                icmp6_param_problem(p, ICMP6_PP_HEADER, ip_data.current_ip_header_tot_len);      
                pbuf_free(p);
                goto ip6_input_cleanup;
                break;
            case IP6_NEXTH_ROUTING:   
            case IP6_NEXTH_FRAGMENT:  
            case IP6_NEXTH_DESTOPTS: 
                // continue procee nexth extension headers
                break;
            case IP6_NEXTH_TCP:      
            case IP6_NEXTH_UDP:       
            case IP6_NEXTH_ENCAPS:    
            case IP6_NEXTH_ICMP6:    
            case IP6_NEXTH_NONE:      
            case IP6_NEXTH_UDPLITE:   
                // continue procee nexth extension headers
                break;
            default:        
                // case 1) and 3)
                goto options_done;
                break;
            }
        }
        break;
    default:
        break;
    }
    
    switch (nexth) {
    case IP6_NEXTH_HOPBYHOP:
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Hop-by-Hop options header\n"));
      /* Get next header type. */
      nexth = *((u8_t *)p->payload);

      /* Get the header length. */
      hlen = 8 * (1 + *((u8_t *)p->payload + 1));
      //ip_data.current_ip_header_tot_len += hlen;

      /* Skip over this header. */
      if (hlen > p->len) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IPv6 options header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 packet dropped.\n",
              hlen, p->len));
        /* free (drop) packet pbufs */
        pbuf_free(p);
        IP6_STATS_INC(ip6.lenerr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }

      /* handle hop-by-hop option header */
      if(ip6_hopopts_input(&plen, &rtalert, &p, (s32_t *)(&ip_data.current_ip_header_tot_len)) < 0)
      {
         pbuf_free(p);
         goto ip6_input_cleanup;     
      }

      pbuf_header(p, -(s16_t)hlen);
      break;
    case IP6_NEXTH_DESTOPTS:
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Destination options header\n"));
      /* Get next header type. */
      nexth = *((u8_t *)p->payload);

      /* Get the header length. */
      hlen = 8 * (1 + *((u8_t *)p->payload + 1));
      //ip_data.current_ip_header_tot_len += hlen;

      /* Skip over this header. */
      if (hlen > p->len) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IPv6 options header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 packet dropped.\n",
              hlen, p->len));
        /* free (drop) packet pbufs */
        pbuf_free(p);
        IP6_STATS_INC(ip6.lenerr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }

      /* handle destination option header */
      if(dest6_input(&p, (s32_t *)(&ip_data.current_ip_header_tot_len), proto) < 0)
      {
         pbuf_free(p);
         goto ip6_input_cleanup;     
      }

      pbuf_header(p, -(s16_t)hlen);
      break;
    case IP6_NEXTH_ROUTING:
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Routing header\n"));
      /* Get next header type. */
      nexth = *((u8_t *)p->payload);

      /* Get the header length. */
      hlen = 8 * (1 + *((u8_t *)p->payload + 1));
      //ip_data.current_ip_header_tot_len += hlen;

      /* Skip over this header. */
      if (hlen > p->len) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IPv6 options header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 packet dropped.\n",
              hlen, p->len));
        /* free (drop) packet pbufs */
        pbuf_free(p);
        IP6_STATS_INC(ip6.lenerr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
      }

      /* handle routing header */
      if(route6_input(&p, (s32_t *)(&ip_data.current_ip_header_tot_len), proto) < 0)
      {
         pbuf_free(p);
         goto ip6_input_cleanup;     
      }

      pbuf_header(p, -(s16_t)hlen);
      break;

    case IP6_NEXTH_FRAGMENT:
    {
      struct ip6_frag_hdr * frag_hdr;
      LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Fragment header\n"));

      frag_hdr = (struct ip6_frag_hdr *)p->payload;

      /* Get next header type. */
      nexth = frag_hdr->_nexth;

      /* Fragment Header length. */
      hlen = 8;
      //ip_data.current_ip_header_tot_len += hlen;

      /* Make sure this header fits in current pbuf. */
      if (hlen > p->len) {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS,
          ("IPv6 options header (hlen %"U16_F") does not fit in first pbuf (len %"U16_F"), IPv6 packet dropped.\n",
              hlen, p->len));
        /* free (drop) packet pbufs */
        pbuf_free(p);
        IP6_FRAG_STATS_INC(ip6_frag.lenerr);
        IP6_FRAG_STATS_INC(ip6_frag.drop);
        goto ip6_input_cleanup;
      }

      /* handle fragment header */
      if(frag6_input(&p, (s32_t *)(&ip_data.current_ip_header_tot_len), proto) < 0)
      {
         pbuf_free(p);
         goto ip6_input_cleanup;     
      }   

      /* Offset == 0 and more_fragments == 0? */
      if (((ntohs(frag_hdr->_fragment_offset) & IP6_FRAG_OFFSET_MASK) == 0) &&
          ((ntohs(frag_hdr->_fragment_offset) & IP6_FRAG_MORE_FLAG) == 0)) {

        /* This is a 1-fragment packet, usually a packet that we have
         * already reassembled. Skip this header anc continue. */
        pbuf_header(p, -(s16_t)hlen);
      } else {
#if LWIP_IPV6_REASS

        /* reassemble the packet */
        p = ip6_reass(p);
        /* packet not fully reassembled yet? */
        if (p == NULL) {
          goto ip6_input_cleanup;
        }

        /* Returned p point to IPv6 header.
         * Update all our variables and pointers and continue. */
        ip6hdr = (struct ip6_hdr *)p->payload;
        nexth = IP6H_NEXTH(ip6hdr);
        hlen = ip_data.current_ip_header_tot_len = IP6_HLEN;
        pbuf_header(p, -IP6_HLEN);

#else /* LWIP_IPV6_REASS */
        /* free (drop) packet pbufs */
        LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: packet with Fragment header dropped (with LWIP_IPV6_REASS==0)\n"));
        pbuf_free(p);
        IP6_STATS_INC(ip6.opterr);
        IP6_STATS_INC(ip6.drop);
        goto ip6_input_cleanup;
#endif /* LWIP_IPV6_REASS */
      }
      break;
    }
    default:
      goto options_done;
      break;
    }
  }
options_done:

  /* p points to IPv6 header again. */
  /* @todo: this does not work for PBUF_REF pbufs */
  pbuf_header(p, ip_data.current_ip_header_tot_len);

  /* send to upper layers */
  LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: \n"));
  ip6_debug_print(p);
  LWIP_DEBUGF(IP6_DEBUG, ("ip6_input: p->len %"U16_F" p->tot_len %"U16_F"\n", p->len, p->tot_len));

#if LWIP_RAW
  /* raw input did not eat the packet? */
  if (raw_input(p, inp) == 0)
#endif /* LWIP_RAW */
  {
    switch (nexth) {
    case IP6_NEXTH_NONE:
      pbuf_free(p);
      break;
#if LWIP_UDP
    case IP6_NEXTH_UDP:
#if LWIP_UDPLITE
    case IP6_NEXTH_UDPLITE:
#endif /* LWIP_UDPLITE */
      /* Point to payload. */
      pbuf_header(p, -(s16_t)ip_data.current_ip_header_tot_len);
      udp_input(p, inp);
      break;
#endif /* LWIP_UDP */
#if LWIP_TCP
    case IP6_NEXTH_TCP:
      /* Point to payload. */
      pbuf_header(p, -(s16_t)ip_data.current_ip_header_tot_len);
      tcp_input(p, inp);
      break;
#endif /* LWIP_TCP */
#if LWIP_ICMP6
    case IP6_NEXTH_ICMP6:
      /* Point to payload. */
      pbuf_header(p, -(s16_t)ip_data.current_ip_header_tot_len);
      icmp6_input(p, inp);
      break;
#endif /* LWIP_ICMP */
    default:
#if LWIP_ICMP6
      /* send ICMP parameter problem unless it was a multicast or ICMPv6 */
      if ((!ip6_addr_ismulticast(ip6_current_dest_addr())) &&
          (IP6H_NEXTH(ip6hdr) != IP6_NEXTH_ICMP6)) 
      {
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ip6_input: send parameter problem! %"U16_F"\n", has_exthead));
        if(1 == has_exthead)
        {
            icmp6_param_problem(p, ICMP6_PP_HEADER, ip_data.current_ip_header_tot_len);
        }
        else
        {
            icmp6_param_problem(p, ICMP6_PP_HEADER, sizeof(ip6hdr->_v_tc_fl) + sizeof(ip6hdr->_plen));
        }
      }
#endif /* LWIP_ICMP */
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ip6_input: Unsupported transport protocol %"U16_F", %"U16_F"\n", IP6H_NEXTH(ip6hdr), nnexth));
      pbuf_free(p);
      IP6_STATS_INC(ip6.proterr);
      IP6_STATS_INC(ip6.drop);
      break;
    }
  }

ip6_input_cleanup:
  ip_data.current_netif = NULL;
  ip_data.current_input_netif = NULL;
  ip_data.current_ip6_header = NULL;
  ip_data.current_ip_header_tot_len = 0;
  ip6_addr_set_zero(ip6_current_src_addr());
  ip6_addr_set_zero(ip6_current_dest_addr());

  return ERR_OK;
}


/**
 * Sends an IPv6 packet on a network interface. This function constructs
 * the IPv6 header. If the source IPv6 address is NULL, the IPv6 "ANY" address is
 * used as source (usually during network startup). If the source IPv6 address it
 * IP6_ADDR_ANY, the most appropriate IPv6 address of the outgoing network
 * interface is filled in as source address. If the destination IPv6 address is
 * IP_HDRINCL, p is assumed to already include an IPv6 header and p->payload points
 * to it instead of the data.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
            protocol header; if dest == IP_HDRINCL, p already includes an
            IPv6 header and p->payload points to that IPv6 header)
 * @param src the source IPv6 address to send from (if src == IP6_ADDR_ANY, an
 *         IP address of the netif is selected and used as source address.
 *         if src == NULL, IP6_ADDR_ANY is used as source)
 * @param dest the destination IPv6 address to send the packet to
 * @param hl the Hop Limit value to be set in the IPv6 header
 * @param tc the Traffic Class value to be set in the IPv6 header
 * @param nexth the Next Header to be set in the IPv6 header
 * @param netif the netif on which to send this packet
 * @return ERR_OK if the packet was sent OK
 *         ERR_BUF if p doesn't have enough space for IPv6/LINK headers
 *         returns errors returned by netif->output
 */
err_t
ip6_output_if(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
             u8_t hl, u8_t tc,
             u8_t nexth, struct netif *netif)
{
  const ip6_addr_t *src_used = src;
  if (dest != IP_HDRINCL) {
    if (src != NULL && ip6_addr_isany(src)) {
      src = ip6_select_source_address(netif, dest);
      if ((src == NULL) || ip6_addr_isany(src)) {
        /* No appropriate source address was found for this packet. */
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ip6_output: No suitable source address for packet.\n"));
        IP6_STATS_INC(ip6.rterr);
        return ERR_RTE;
      }
    }
  }
  return ip6_output_if_src(p, src_used, dest, hl, tc, nexth, netif);
}

/**
 * Same as ip6_output_if() but 'src' address is not replaced by netif address
 * when it is 'any'.
 */
err_t
ip6_output_if_src(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
             u8_t hl, u8_t tc,
             u8_t nexth, struct netif *netif)
{
  struct ip6_hdr *ip6hdr;
  ip6_addr_t dest_addr;

  LWIP_IP_CHECK_PBUF_REF_COUNT_FOR_TX(p);

  /* Should the IPv6 header be generated or is it already included in p? */
  if (dest != IP_HDRINCL) {
    /* generate IPv6 header */
    if (pbuf_header(p, IP6_HLEN)) {
      LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ip6_output: not enough room for IPv6 header in pbuf\n"));
      IP6_STATS_INC(ip6.err);
      return ERR_BUF;
    }

    ip6hdr = (struct ip6_hdr *)p->payload;
    LWIP_ASSERT("check that first pbuf can hold struct ip6_hdr",
               (p->len >= sizeof(struct ip6_hdr)));

    IP6H_HOPLIM_SET(ip6hdr, hl);
    IP6H_NEXTH_SET(ip6hdr, nexth);

    /* dest cannot be NULL here */
    ip6_addr_copy(ip6hdr->dest, *dest);

    IP6H_VTCFL_SET(ip6hdr, 6, tc, 0);
    IP6H_PLEN_SET(ip6hdr, p->tot_len - IP6_HLEN);

    if (src == NULL) {
      src = IP6_ADDR_ANY6;
    }
    else if (ip6_addr_isany(src)) {
      src = ip6_select_source_address(netif, dest);
      if ((src == NULL) || ip6_addr_isany(src)) {
        /* No appropriate source address was found for this packet. */
        LWIP_DEBUGF(IP6_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("ip6_output: No appropriate source address was found for this packet\n"));
        IP6_STATS_INC(ip6.rterr);
        return ERR_RTE;
      }
    }
    /* src cannot be NULL here */
    ip6_addr_copy(ip6hdr->src, *src);

  } else {
    /* IP header already included in p */
    ip6hdr = (struct ip6_hdr *)p->payload;
    ip6_addr_copy(dest_addr, ip6hdr->dest);
    dest = &dest_addr;
  }

  IP6_STATS_INC(ip6.xmit);

  LWIP_DEBUGF(IP6_DEBUG, ("ip6_output_if: %c%c%"U16_F"\n", netif->name[0], netif->name[1], netif->num));
  ip6_debug_print(p);

#if ENABLE_LOOPBACK
  {
    int i;
#if !LWIP_HAVE_LOOPIF
    if (ip6_addr_isloopback(dest)) {
      return netif_loop_output(netif, p);
    }
#endif /* !LWIP_HAVE_LOOPIF */
    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
      if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
          ip6_addr_cmp(dest, netif_ip6_addr(netif, i))) {
        /* Packet to self, enqueue it for loopback */
        LWIP_DEBUGF(IP6_DEBUG, ("netif_loop_output()\n"));
        return netif_loop_output(netif, p);
      }
    }
  }
#endif /* ENABLE_LOOPBACK */
#if LWIP_IPV6_FRAG
  /* don't fragment if interface has mtu set to 0 [loopif] */
  if (netif->mtu && (p->tot_len > nd6_get_destination_mtu(dest, netif))) {
    return ip6_frag_ex(p, netif, dest);
  }
#endif /* LWIP_IPV6_FRAG */

  LWIP_DEBUGF(IP6_DEBUG, ("netif->output_ip6()\n"));
  return netif->output_ip6(netif, p, dest);
}

/**
 * Simple interface to ip6_output_if. It finds the outgoing network
 * interface and calls upon ip6_output_if to do the actual work.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
            protocol header; if dest == IP_HDRINCL, p already includes an
            IPv6 header and p->payload points to that IPv6 header)
 * @param src the source IPv6 address to send from (if src == IP6_ADDR_ANY, an
 *         IP address of the netif is selected and used as source address.
 *         if src == NULL, IP6_ADDR_ANY is used as source)
 * @param dest the destination IPv6 address to send the packet to
 * @param hl the Hop Limit value to be set in the IPv6 header
 * @param tc the Traffic Class value to be set in the IPv6 header
 * @param nexth the Next Header to be set in the IPv6 header
 *
 * @return ERR_RTE if no route is found
 *         see ip_output_if() for more return values
 */
err_t
ip6_output(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
          u8_t hl, u8_t tc, u8_t nexth)
{
  struct netif *netif;
  struct ip6_hdr *ip6hdr;
  ip6_addr_t src_addr, dest_addr;

  LWIP_IP_CHECK_PBUF_REF_COUNT_FOR_TX(p);

  if (dest != IP_HDRINCL) {
    netif = ip6_route(src, dest);
  } else {
    /* IP header included in p, read addresses. */
    ip6hdr = (struct ip6_hdr *)p->payload;
    ip6_addr_copy(src_addr, ip6hdr->src);
    ip6_addr_copy(dest_addr, ip6hdr->dest);
    netif = ip6_route(&src_addr, &dest_addr);
  }

  if (netif == NULL) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_output: no route for %"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F"\n",
        IP6_ADDR_BLOCK1(dest),
        IP6_ADDR_BLOCK2(dest),
        IP6_ADDR_BLOCK3(dest),
        IP6_ADDR_BLOCK4(dest),
        IP6_ADDR_BLOCK5(dest),
        IP6_ADDR_BLOCK6(dest),
        IP6_ADDR_BLOCK7(dest),
        IP6_ADDR_BLOCK8(dest)));
    IP6_STATS_INC(ip6.rterr);
    return ERR_RTE;
  }

  return ip6_output_if(p, src, dest, hl, tc, nexth, netif);
}


#if LWIP_NETIF_HWADDRHINT
/** Like ip6_output, but takes and addr_hint pointer that is passed on to netif->addr_hint
 *  before calling ip6_output_if.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
            protocol header; if dest == IP_HDRINCL, p already includes an
            IPv6 header and p->payload points to that IPv6 header)
 * @param src the source IPv6 address to send from (if src == IP6_ADDR_ANY, an
 *         IP address of the netif is selected and used as source address.
 *         if src == NULL, IP6_ADDR_ANY is used as source)
 * @param dest the destination IPv6 address to send the packet to
 * @param hl the Hop Limit value to be set in the IPv6 header
 * @param tc the Traffic Class value to be set in the IPv6 header
 * @param nexth the Next Header to be set in the IPv6 header
 * @param addr_hint address hint pointer set to netif->addr_hint before
 *        calling ip_output_if()
 *
 * @return ERR_RTE if no route is found
 *         see ip_output_if() for more return values
 */
err_t
ip6_output_hinted(struct pbuf *p, const ip6_addr_t *src, const ip6_addr_t *dest,
          u8_t hl, u8_t tc, u8_t nexth, u8_t *addr_hint)
{
  struct netif *netif;
  struct ip6_hdr *ip6hdr;
  ip6_addr_t src_addr, dest_addr;
  err_t err;

  LWIP_IP_CHECK_PBUF_REF_COUNT_FOR_TX(p);

  if (dest != IP_HDRINCL) {
    netif = ip6_route(src, dest);
  } else {
    /* IP header included in p, read addresses. */
    ip6hdr = (struct ip6_hdr *)p->payload;
    ip6_addr_copy(src_addr, ip6hdr->src);
    ip6_addr_copy(dest_addr, ip6hdr->dest);
    netif = ip6_route(&src_addr, &dest_addr);
  }

  if (netif == NULL) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_output: no route for %"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F":%"X16_F"\n",
        IP6_ADDR_BLOCK1(dest),
        IP6_ADDR_BLOCK2(dest),
        IP6_ADDR_BLOCK3(dest),
        IP6_ADDR_BLOCK4(dest),
        IP6_ADDR_BLOCK5(dest),
        IP6_ADDR_BLOCK6(dest),
        IP6_ADDR_BLOCK7(dest),
        IP6_ADDR_BLOCK8(dest)));
    IP6_STATS_INC(ip6.rterr);
    return ERR_RTE;
  }

  NETIF_SET_HWADDRHINT(netif, addr_hint);
  err = ip6_output_if(p, src, dest, hl, tc, nexth, netif);
  NETIF_SET_HWADDRHINT(netif, NULL);

  return err;
}
#endif /* LWIP_NETIF_HWADDRHINT*/

#if LWIP_IPV6_MLD
/**
 * Add a hop-by-hop options header with a router alert option and padding.
 *
 * Used by MLD when sending a Multicast listener report/done message.
 *
 * @param p the packet to which we will prepend the options header
 * @param nexth the next header protocol number (e.g. IP6_NEXTH_ICMP6)
 * @param value the value of the router alert option data (e.g. IP6_ROUTER_ALERT_VALUE_MLD)
 * @return ERR_OK if hop-by-hop header was added, ERR_* otherwise
 */
err_t
ip6_options_add_hbh_ra(struct pbuf * p, u8_t nexth, u8_t value)
{
  struct ip6_hbh_hdr * hbh_hdr;

  /* Move pointer to make room for hop-by-hop options header. */
  if (pbuf_header(p, sizeof(struct ip6_hbh_hdr))) {
    LWIP_DEBUGF(IP6_DEBUG, ("ip6_options: no space for options header\n"));
    IP6_STATS_INC(ip6.err);
    return ERR_BUF;
  }

  hbh_hdr = (struct ip6_hbh_hdr *)p->payload;

  /* Set fields. */
  hbh_hdr->_nexth = nexth;
  hbh_hdr->_hlen = 0;
  hbh_hdr->_ra_opt_type = IP6_ROUTER_ALERT_OPTION;
  hbh_hdr->_ra_opt_dlen = 2;
  hbh_hdr->_ra_opt_data = value;
  hbh_hdr->_padn_opt_type = IP6_PADN_ALERT_OPTION;
  hbh_hdr->_padn_opt_dlen = 0;

  return ERR_OK;
}
#endif /* LWIP_IPV6_MLD */

#if IP6_DEBUG
/* Print an IPv6 header by using LWIP_DEBUGF
 * @param p an IPv6 packet, p->payload pointing to the IPv6 header
 */
void
ip6_debug_print(struct pbuf *p)
{
  struct ip6_hdr *ip6hdr = (struct ip6_hdr *)p->payload;

  LWIP_DEBUGF(IP6_DEBUG, ("IPv6 header:\n"));
  LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(IP6_DEBUG, ("| %2"U16_F" |  %3"U16_F"  |      %7"U32_F"     | (ver, class, flow)\n",
                    IP6H_V(ip6hdr),
                    IP6H_TC(ip6hdr),
                    IP6H_FL(ip6hdr)));
  LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(IP6_DEBUG, ("|     %5"U16_F"     |  %3"U16_F"  |  %3"U16_F"  | (plen, nexth, hopl)\n",
                    IP6H_PLEN(ip6hdr),
                    IP6H_NEXTH(ip6hdr),
                    IP6H_HOPLIM(ip6hdr)));
  LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(IP6_DEBUG, ("|  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |  %4"X32_F" | (src)\n",
                    IP6_ADDR_BLOCK1(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK2(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK3(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK4(&(ip6hdr->src))));
  LWIP_DEBUGF(IP6_DEBUG, ("|  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |\n",
                    IP6_ADDR_BLOCK5(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK6(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK7(&(ip6hdr->src)),
                    IP6_ADDR_BLOCK8(&(ip6hdr->src))));
  LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));
  LWIP_DEBUGF(IP6_DEBUG, ("|  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |  %4"X32_F" | (dest)\n",
                    IP6_ADDR_BLOCK1(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK2(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK3(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK4(&(ip6hdr->dest))));
  LWIP_DEBUGF(IP6_DEBUG, ("|  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |  %4"X32_F" |\n",
                    IP6_ADDR_BLOCK5(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK6(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK7(&(ip6hdr->dest)),
                    IP6_ADDR_BLOCK8(&(ip6hdr->dest))));
  LWIP_DEBUGF(IP6_DEBUG, ("+-------------------------------+\n"));
}
#endif /* IP6_DEBUG */

/*
 * Unknown option processing.
 * The third argument `off' is the offset from the IPv6 header to the option,
 * which is necessary if the IPv6 header the and option header and IPv6 header
 * is not continuous in order to return an ICMPv6 error.
 */
s32_t ip6_unknown_opt(u8_t *optp, struct pbuf *p, s32_t off)
{
  // struct ip6_hdr *ip6;

  switch (IP6OPT_TYPE(*optp)) {
    case IP6OPT_TYPE_SKIP: /* ignore the option */
      return((s32_t)*(optp + 1));
    case IP6OPT_TYPE_DISCARD: /* silently discard */
      return(-1);
    case IP6OPT_TYPE_FORCEICMP: /* send ICMP even if multicasted */
      /* p points to IPv6 header again. */
      pbuf_header(p, ip_data.current_ip_header_tot_len);
      icmp6_param_problem(p, ICMP6_PP_OPTION, off); 
      return(-1);
    case IP6OPT_TYPE_ICMP: /* send ICMP if not multicasted */
      if (ip6_addr_ismulticast(ip6_current_dest_addr())) {
        // do nothing
       } else {      
        /* p points to IPv6 header again. */
        pbuf_header(p, ip_data.current_ip_header_tot_len);
        icmp6_param_problem(p, ICMP6_PP_OPTION, off); 
      }
      return(-1);
    }

  return(-1);
}

/*
 * Hop-by-Hop options header processing. If a valid jumbo payload option is
 * included, the real payload length will be stored in plenp. 
 * rtalertp: should be stored more smart way 
 */
s32_t ip6_hopopts_input(u32_t *plenp, u32_t *rtalertp, struct pbuf **pp, s32_t *offp)
{
  struct pbuf *p = *pp;
  s32_t off = *offp, hbhlen;
  struct ip6_hbh_hdr *hbh;

  /* validation of the length of the header */
#if 0
#ifndef PULLDOWN_TEST
  IP6_EXTHDR_CHECK(m, off, sizeof(*hbh), -1);
  hbh = (struct ip6_hbh *)(mtod(m, caddr_t) + off);
  hbhlen = (hbh->ip6h_len + 1) << 3;
  
  IP6_EXTHDR_CHECK(m, off, hbhlen, -1);
  hbh = (struct ip6_hbh *)(mtod(m, caddr_t) + off);
#else
  IP6_EXTHDR_GET(hbh, struct ip6_hbh *, m,
  sizeof(struct ip6_hdr), sizeof(struct ip6_hbh));
  if (hbh == NULL) {
    ip6stat.ip6s_tooshort++;
    return -1;
  }
  hbhlen = (hbh->ip6h_len + 1) << 3;
  IP6_EXTHDR_GET(hbh, struct ip6_hbh *, m, sizeof(struct ip6_hdr),
    hbhlen);
  if (hbh == NULL) {
    ip6stat.ip6s_tooshort++;
    return -1;
  }
#endif
#endif
  hbh = (struct ip6_hbh_hdr *)p->payload;
  hbhlen = (hbh->_hlen + 1) << 3;
  off += hbhlen;
  // hop-by-hop option data len
  hbhlen -= IPV6_HBH_HSIZE;

  if (ip6_process_hopopts(p, (u8_t *)hbh + IPV6_HBH_HSIZE, hbhlen, rtalertp, plenp) < 0) {   
    return(-1);
  }

  *offp = off;
  *pp = p;
  return(0);
}

/*
 * Search header for all Hop-by-hop options and process each option.
 * This function is separate from ip6_hopopts_input() in order to
 * handle a case where the sending node itself process its hop-by-hop
 * options header. In such a case, the function is called from ip6_output().
 *
 * The function assumes that hbh header is located right after the IPv6 header
 * (RFC2460 p7), opthead is pointer into data content in m, and opthead to
 * opthead + hbhlen is located in continuous memory region.
 */
s32_t ip6_process_hopopts(struct pbuf *p, u8_t *opthead, s32_t hbhlen, u32_t *rtalertp, u32_t *plenp)
{
  // struct ip6_hdr *ip6;
  s32_t optlen = 0;
  u8_t *opt = opthead;
  const s32_t erroff = IP6_HLEN + IPV6_HBH_HSIZE;

  for (; hbhlen > 0; hbhlen -= optlen, opt += optlen) {
    switch (*opt) {
      case IP6OPT_PAD1:
        optlen = 1;
        break;
      case IP6OPT_PADN:
        if (hbhlen < IP6OPT_MINLEN) {
          goto bad;
        }
        optlen = *(opt + 1) + 2;
        break;
    #if 0
      case IP6OPT_RTALERT:
        /* XXX may need check for alignment */
        if (hbhlen < IP6OPT_RTALERT_LEN) {
          ip6stat.ip6s_toosmall++;
          goto bad;
        }
        if (*(opt + 1) != IP6OPT_RTALERT_LEN - 2) {
          /* XXX stat */
          icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER, erroff + opt + 1 - opthead);
          return(-1);
        }
        optlen = IP6OPT_RTALERT_LEN;
        bcopy((caddr_t)(opt + 2), (caddr_t)&rtalert_val, 2);
        *rtalertp = ntohs(rtalert_val);
        break;
      case IP6OPT_JUMBO:
        /* XXX may need check for alignment */
        if (hbhlen < IP6OPT_JUMBO_LEN) {
          ip6stat.ip6s_toosmall++;
          goto bad;
        }
        if (*(opt + 1) != IP6OPT_JUMBO_LEN - 2) {
          /* XXX stat */
          icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER, erroff + opt + 1 - opthead);
          return(-1);
        }
        optlen = IP6OPT_JUMBO_LEN;

        /*
         * IPv6 packets that have non 0 payload length
         * must not contain a jumbo payload option.
         */
        ip6 = mtod(m, struct ip6_hdr *);
        if (ip6->ip6_plen) {
          ip6stat.ip6s_badoptions++;
          icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER, erroff + opt - opthead);
          return(-1);
        }

        /*
         * We may see jumbolen in unaligned location, so
         * we'd need to perform bcopy().
         */
        bcopy(opt + 2, &jumboplen, sizeof(jumboplen));
        jumboplen = (u_int32_t)htonl(jumboplen);

      #if 1
        /*
         * if there are multiple jumbo payload options,
         * *plenp will be non-zero and the packet will be
         * rejected.
         * the behavior may need some debate in ipngwg -
         * multiple options does not make sense, however,
         * there's no explicit mention in specification.
         */
        if (*plenp != 0) {
          ip6stat.ip6s_badoptions++;
          icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER, erroff + opt + 2 - opthead);
          return(-1);
        }
      #endif

        /*
         * jumbo payload length must be larger than 65535.
         */
        if (jumboplen <= IPV6_MAXPACKET) {
          ip6stat.ip6s_badoptions++;
          icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER, erroff + opt + 2 - opthead);
          return(-1);
        }
        *plenp = jumboplen;
        break;
      #endif                  
      default:    /* unknown option */
        if (hbhlen < IP6OPT_MINLEN) {
          goto bad;
        }
        optlen = ip6_unknown_opt(opt, p, erroff + opt - opthead);
        if (optlen == -1) {         
          return(-1);
        }
        optlen += 2;
        break;
      }
    }
  return(0);

bad:
  return(-1);
}

/*
 * Destination options header processing.
 */
s32_t dest6_input(struct pbuf **pp, s32_t *offp, s32_t proto)
{
  struct pbuf *p = *pp;
  s32_t off = *offp, dstoptlen, optlen;
  struct ip6_dest_hdr *dstopts;
  //struct mbuf *n;
  // struct ip6_opt_home_address *haopt = NULL;
  // struct ip6aux *ip6a = NULL;
  u8_t *opt;
  // struct ip6_hdr *ip6;

  // ip6 = mtod(m, struct ip6_hdr *);

  /* validation of the length of the header */
#if 0
  #ifndef PULLDOWN_TEST
  IP6_EXTHDR_CHECK(m, off, sizeof(*dstopts), IPPROTO_DONE);
  dstopts = (struct ip6_dest *)(mtod(m, caddr_t) + off);
  #else
  IP6_EXTHDR_GET(dstopts, struct ip6_dest *, m, off, sizeof(*dstopts));
  if (dstopts == NULL)
    return IPPROTO_DONE;
  #endif
  dstoptlen = (dstopts->ip6d_len + 1) << 3;

  #ifndef PULLDOWN_TEST
  IP6_EXTHDR_CHECK(m, off, dstoptlen, IPPROTO_DONE);
  dstopts = (struct ip6_dest *)(mtod(m, caddr_t) + off);
  #else
  IP6_EXTHDR_GET(dstopts, struct ip6_dest *, m, off, dstoptlen);
  if (dstopts == NULL)
    return IPPROTO_DONE;
  #endif
#endif
  dstopts = (struct ip6_dest_hdr *)p->payload;
  dstoptlen = (dstopts->_hlen + 1) << 3;
  off += dstoptlen;
  // destination option data len
  dstoptlen -= IPV6_DEST_HSIZE;
  opt = (u8_t *)dstopts + IPV6_DEST_HSIZE;

  /* search header for all options. */
  for (optlen = 0; dstoptlen > 0; dstoptlen -= optlen, opt += optlen) {
    if (*opt != IP6OPT_PAD1 && (dstoptlen < IP6OPT_MINLEN || *(opt + 1) + 2 > dstoptlen))  {
      goto bad;
    }

    switch (*opt) {
      case IP6OPT_PAD1:
        optlen = 1;
        break;
      case IP6OPT_PADN:
        optlen = *(opt + 1) + 2;
        break;
    #if 0
      case IP6OPT_HOME_ADDRESS:
        /*
         * XXX we assume that home address option appear after
         * AH.  if the assumption does not hold, the validation
         * of AH will fail due to the address swap.
         */
        #if 0
        /* be picky about alignment: 8n+6 */
        if ((opt - (u_int8_t *)dstopts) % 8 != 6)
          goto bad;
        #endif

        /* HA option must appear only once */
        n = ip6_addaux(m);
        if (!n) {
          /* not enough core */
          goto bad;
        }
        ip6a = mtod(n, struct ip6aux *);
        if ((ip6a->ip6a_flags & IP6A_HASEEN) != 0) {
          /* XXX icmp6 paramprob? */
          goto bad;
        }

        haopt = (struct ip6_opt_home_address *)opt;
        optlen = haopt->ip6oh_len + 2;

        /*
         * don't complain even if it is larger,
         * we don't support suboptions at this moment.
         */
        if (optlen < sizeof(*haopt)) {
          ip6stat.ip6s_toosmall++;
          goto bad;
        }

        /* XXX check header ordering */

        bcopy(haopt->ip6oh_addr, &ip6a->ip6a_home, 
            sizeof(ip6a->ip6a_home));
        bcopy(&ip6->ip6_src, &ip6a->ip6a_careof, 
            sizeof(ip6a->ip6a_careof));
        ip6a->ip6a_flags |= IP6A_HASEEN;

        /*
         * reject invalid home-addresses
         */
        /* XXX linklocal-address is not supported */
        if (IN6_IS_ADDR_MULTICAST(&ip6a->ip6a_home) ||
            IN6_IS_ADDR_LINKLOCAL(&ip6a->ip6a_home) ||
            IN6_IS_ADDR_V4MAPPED(&ip6a->ip6a_home)  ||
            IN6_IS_ADDR_UNSPECIFIED(&ip6a->ip6a_home) ||
            IN6_IS_ADDR_LOOPBACK(&ip6a->ip6a_home)) {
            ip6stat.ip6s_badscope++;
            goto bad;
        }

        /*
         * Currently, no valid sub-options are
         * defined for use in a Home Address option.
         */

        break;

      #ifdef MIP6
      case IP6OPT_BINDING_UPDATE:
      case IP6OPT_BINDING_ACK:
      case IP6OPT_BINDING_REQ:
        if (mip6_process_destopt(m, dstopts, opt, dstoptlen)
            == -1)
          goto bad;
          optlen = *(opt + 1) + 2;
          break;
      #endif /* MIP6 */
    #endif
      default:      /* unknown option */
        optlen = ip6_unknown_opt(opt, p, (u8_t *)opt - (u8_t *)dstopts + *offp);
        if (optlen == -1) {            
          return (-1);
          //return (IPPROTO_DONE);
        }
        optlen += 2;
        break;
    }
  }

#if 0
  /* if haopt is non-NULL, we are sure we have seen fresh HA option */
  if (haopt && ip6a &&
     (ip6a->ip6a_flags & (IP6A_HASEEN | IP6A_SWAP)) == IP6A_HASEEN) {
    /* XXX should we do this at all?  do it now or later? */
    /* XXX interaction with 2292bis IPV6_RECVDSTOPT */
    /* XXX interaction with ipsec - should be okay */
    /* XXX icmp6 responses is modified - which is bad */
    bcopy(&ip6a->ip6a_careof, haopt->ip6oh_addr,
          sizeof(haopt->ip6oh_addr));
    bcopy(&ip6a->ip6a_home, &ip6->ip6_src,
          sizeof(ip6->ip6_src));
    #if 0
    /* XXX linklocal address is (currently) not supported */
    if (IN6_IS_SCOPE_LINKLOCAL(&ip6->ip6_src))
        ip6->ip6_src.s6_addr16[1]
        = htons(m->m_pkthdr.rcvif->if_index);
    #endif
    ip6a->ip6a_flags |= IP6A_SWAP;
  }
#endif
    
  *offp = off;
  return (0);

bad:
  return (-1);
  // return (IPPROTO_DONE);
}

/*
 * Routing header processing.
 */
s32_t route6_input(struct pbuf **pp, s32_t *offp, s32_t proto)
{
  // struct ip6_hdr *ip6;
  struct pbuf *p = *pp;
  struct ip6_rt_hdr *rh;
  int off = *offp, rhlen = 0;
  // struct mbuf *n;

#if 0
  n = ip6_findaux(m);
  if (n) {
    struct ip6aux *ip6a = mtod(n, struct ip6aux *);
    /* XXX reject home-address option before rthdr */
    if (ip6a->ip6a_flags & IP6A_SWAP) {
      ip6stat.ip6s_badoptions++;
      m_freem(m);
      return IPPROTO_DONE;
    }
  }
#endif

#if 0
  #ifndef PULLDOWN_TEST
  IP6_EXTHDR_CHECK(m, off, sizeof(*rh), IPPROTO_DONE);
  ip6 = mtod(m, struct ip6_hdr *);
  rh = (struct ip6_rthdr *)((caddr_t)ip6 + off);
  #else
  ip6 = mtod(m, struct ip6_hdr *);
  IP6_EXTHDR_GET(rh, struct ip6_rthdr *, m, off, sizeof(*rh));
  if (rh == NULL) {
    ip6stat.ip6s_tooshort++;
    return IPPROTO_DONE;
  }
  #endif
#endif
    
  rh = (struct ip6_rt_hdr *)p->payload;

  switch (rh->_type) {
  #if 0 /* Modified after FreeBSD7 (Eddy ) */
  case IPV6_RTHDR_TYPE_0:
    rhlen = (rh->ip6r_len + 1) << 3;
    #ifndef PULLDOWN_TEST
    /*
     * note on option length:
     * due to IP6_EXTHDR_CHECK assumption, we cannot handle
     * very big routing header (max rhlen == 2048).
     */
    IP6_EXTHDR_CHECK(m, off, rhlen, IPPROTO_DONE);
    #else
    /*
     * note on option length:
     * maximum rhlen: 2048
     * max mbuf m_pulldown can handle: MCLBYTES == usually 2048
     * so, here we are assuming that m_pulldown can handle
     * rhlen == 2048 case.  this may not be a good thing to
     * assume - we may want to avoid pulling it up altogether.
     */
    IP6_EXTHDR_GET(rh, struct ip6_rthdr *, m, off, rhlen);
    if (rh == NULL) {
      ip6stat.ip6s_tooshort++;
      return IPPROTO_DONE;
    }
    #endif
    if (ip6_rthdr0(m, ip6, (struct ip6_rthdr0 *)rh))
      return(IPPROTO_DONE);
    break;
  #endif /* Disable route header 0 */    
  default:
    /* unknown routing type */
    if (rh->_segleft == 0) {
      rhlen = (rh->_hlen + 1) << 3;
      break;  /* Final dst. Just ignore the header. */
    }
    /* p points to IPv6 header again. */
    pbuf_header(p, ip_data.current_ip_header_tot_len);
    icmp6_param_problem(p, ICMP6_PP_FIELD, off + sizeof(rh->_nexth) + sizeof(rh->_hlen)); 
    // icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER, (caddr_t)&rh->ip6r_type - (caddr_t)ip6);       
    return (-1);
    // return(IPPROTO_DONE);
  }

  *offp += rhlen;   
  return (0);
  // return(rh->ip6r_nxt);
}

/*
 * In RFC2460, fragment and reassembly rule do not agree with each other,
 * in terms of next header field handling in fragment header.
 * While the sender will use the same value for all of the fragmented packets,
 * receiver is suggested not to check the consistency.
 *
 * fragment rule (p20):
 *  (2) A Fragment header containing:
 *  The Next Header value that identifies the first header of
 *  the Fragmentable Part of the original packet.
 *    -> next header field is same for all fragments
 *
 * reassembly rule (p21):
 *  The Next Header field of the last header of the Unfragmentable
 *  Part is obtained from the Next Header field of the first
 *  fragment's Fragment header.
 *    -> should grab it from the first fragment only
 *
 * The following note also contradicts with fragment rule - noone is going to
 * send different fragment with different next header field.
 *
 * additional note (p22):
 *  The Next Header values in the Fragment headers of different
 *  fragments of the same original packet may differ.  Only the value
 *  from the Offset zero fragment packet is used for reassembly.
 *    -> should grab it from the first fragment only
 *
 * There is no explicit reason given in the RFC.  Historical reason maybe?
 */
/*
 * Fragment input
 */
s32_t frag6_input(struct pbuf **pp, s32_t *offp, s32_t proto)
{
#if 0
  struct mbuf *m = *mp, *t;
  struct ip6_hdr *ip6;
  struct ip6_frag_hdr *ip6f;
  struct ip6q *q6;
  struct ip6asfrag *af6, *ip6af, *af6dwn;
  int offset = *offp, nxt, i, next;
  int first_frag = 0;
  int fragoff, frgpartlen;  /* must be larger than u_int16_t */
  struct ifnet *dstifp;
  #ifdef IN6_IFSTAT_STRICT
  #ifdef NEW_STRUCT_ROUTE
  static struct route ro;
  #else
  static struct route_in6 ro;
  #endif
  struct sockaddr_in6 *dst;
  #endif
#endif
  struct pbuf *p = *pp;
  struct ip6_hdr *ip6;
  struct ip6_frag_hdr *ip6f;
  s32_t off = *offp;
    
#if 0
  ip6 = mtod(m, struct ip6_hdr *);
  #ifndef PULLDOWN_TEST
  IP6_EXTHDR_CHECK(m, offset, sizeof(struct ip6_frag), IPPROTO_DONE);
  ip6f = (struct ip6_frag *)((caddr_t)ip6 + offset);
  #else
  IP6_EXTHDR_GET(ip6f, struct ip6_frag *, m, offset, sizeof(*ip6f));
  if (ip6f == NULL)
    return IPPROTO_DONE;
  #endif

  dstifp = NULL;
  #ifdef IN6_IFSTAT_STRICT
  /* find the destination interface of the packet. */
  dst = (struct sockaddr_in6 *)&ro.ro_dst;
  if (ro.ro_rt
   && ((ro.ro_rt->rt_flags & RTF_UP) == 0
    || !IN6_ARE_ADDR_EQUAL(&dst->sin6_addr, &ip6->ip6_dst))) {
    RTFREE(ro.ro_rt);
    ro.ro_rt = (struct rtentry *)0;
  }
  if (ro.ro_rt == NULL) {
    bzero(dst, sizeof(*dst));
    dst->sin6_family = AF_INET6;
    dst->sin6_len = sizeof(struct sockaddr_in6);
    dst->sin6_addr = ip6->ip6_dst;
  }
  #ifndef __bsdi__
  rtalloc((struct route *)&ro);
  #else
  rtcalloc((struct route *)&ro);
  #endif
  if (ro.ro_rt != NULL && ro.ro_rt->rt_ifa != NULL)
    dstifp = ((struct in6_ifaddr *)ro.ro_rt->rt_ifa)->ia_ifp;
  #else
  /* we are violating the spec, this is not the destination interface */
  if ((m->m_flags & M_PKTHDR) != 0)
    dstifp = m->m_pkthdr.rcvif;
  #endif
#endif

  ip6f = (struct ip6_frag_hdr *)p->payload;
  off += 8; /* Fragment head is 8 btyes */

#if 0
  /* jumbo payload can't contain a fragment header */
  if (ip6->_plen == 0) {
    icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER, offset);
    in6_ifstat_inc(dstifp, ifs6_reass_fail);
    return IPPROTO_DONE;
  }
#endif
    
  /*
   * check whether fragment packet's fragment length is
   * multiple of 8 octets.
   * sizeof(struct ip6_frag) == 8
   * sizeof(struct ip6_hdr) = 40
   */
  if ((ntohs(ip6f->_fragment_offset) & IP6_FRAG_MORE_FLAG) &&
      (ntohs(ip6_current_payload_len())%8 != 0)) {
        /* p points to IPv6 header again. */
    pbuf_header(p, ip_data.current_ip_header_tot_len);
    icmp6_param_problem(p, ICMP6_PP_FIELD, sizeof(ip6->_v_tc_fl)); 
    return -1;
  }
        
#if 0
  ip6stat.ip6s_fragments++;
  in6_ifstat_inc(dstifp, ifs6_reass_reqd);
  
  /* offset now points to data portion */
  offset += sizeof(struct ip6_frag);

  frag6_doing_reass = 1;

  for (q6 = ip6q.ip6q_next; q6 != &ip6q; q6 = q6->ip6q_next)
    if (ip6f->ip6f_ident == q6->ip6q_ident &&
        IN6_ARE_ADDR_EQUAL(&ip6->ip6_src, &q6->ip6q_src) &&
        IN6_ARE_ADDR_EQUAL(&ip6->ip6_dst, &q6->ip6q_dst))
      break;

  if (q6 == &ip6q) {
    /*
     * the first fragment to arrive, create a reassembly queue.
     */
    first_frag = 1;

    /*
     * Enforce upper bound on number of fragmented packets
     * for which we attempt reassembly;
     * If maxfrag is 0, never accept fragments.
     * If maxfrag is -1, accept all fragments without limitation.
     */
    if (ip6_maxfragpackets < 0)
      ;
    else if (frag6_nfragpackets >= (u_int)ip6_maxfragpackets)
      goto dropfrag;
    frag6_nfragpackets++;
    q6 = (struct ip6q *)malloc(sizeof(struct ip6q), M_FTABLE,
      M_DONTWAIT);
    if (q6 == NULL)
      goto dropfrag;
    bzero(q6, sizeof(*q6));

    frag6_insque(q6, &ip6q);

    /* ip6q_nxt will be filled afterwards, from 1st fragment */
    q6->ip6q_down  = q6->ip6q_up = (struct ip6asfrag *)q6;
  #ifdef notyet
    q6->ip6q_nxtp  = (u_char *)nxtp;
  #endif
    q6->ip6q_ident  = ip6f->ip6f_ident;
    q6->ip6q_arrive = 0; /* Is it used anywhere? */
    q6->ip6q_ttl   = IPV6_FRAGTTL;
    q6->ip6q_src  = ip6->ip6_src;
    q6->ip6q_dst  = ip6->ip6_dst;
    q6->ip6q_unfrglen = -1;  /* The 1st fragment has not arrived. */
  }

  /*
   * If it's the 1st fragment, record the length of the
   * unfragmentable part and the next header of the fragment header.
   */
  fragoff = ntohs(ip6f->ip6f_offlg & IP6F_OFF_MASK);
  if (fragoff == 0) {
    q6->ip6q_unfrglen = offset - sizeof(struct ip6_hdr)
      - sizeof(struct ip6_frag);
    q6->ip6q_nxt = ip6f->ip6f_nxt;
  }

  /*
   * Check that the reassembled packet would not exceed 65535 bytes
   * in size.
   * If it would exceed, discard the fragment and return an ICMP error.
   */
  frgpartlen = sizeof(struct ip6_hdr) + ntohs(ip6->ip6_plen) - offset;
  if (q6->ip6q_unfrglen >= 0) {
    /* The 1st fragment has already arrived. */
    if (q6->ip6q_unfrglen + fragoff + frgpartlen > IPV6_MAXPACKET) {
      icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER,
            offset - sizeof(struct ip6_frag) +
          offsetof(struct ip6_frag, ip6f_offlg));
      frag6_doing_reass = 0;
      return(IPPROTO_DONE);
    }
  }
  else if (fragoff + frgpartlen > IPV6_MAXPACKET) {
    icmp6_error(m, ICMP6_PARAM_PROB, ICMP6_PARAMPROB_HEADER,
          offset - sizeof(struct ip6_frag) +
        offsetof(struct ip6_frag, ip6f_offlg));
    frag6_doing_reass = 0;
    return(IPPROTO_DONE);
  }
  /*
   * If it's the first fragment, do the above check for each
   * fragment already stored in the reassembly queue.
   */
  if (fragoff == 0) {
    for (af6 = q6->ip6q_down; af6 != (struct ip6asfrag *)q6;
         af6 = af6dwn) {
      af6dwn = af6->ip6af_down;

      if (q6->ip6q_unfrglen + af6->ip6af_off + af6->ip6af_frglen >
          IPV6_MAXPACKET) {
        struct mbuf *merr = IP6_REASS_MBUF(af6);
        struct ip6_hdr *ip6err;
        int erroff = af6->ip6af_offset;

        /* dequeue the fragment. */
        frag6_deq(af6);
        free(af6, M_FTABLE);

        /* adjust pointer. */
        ip6err = mtod(merr, struct ip6_hdr *);

        /*
         * Restore source and destination addresses
         * in the erroneous IPv6 header.
         */
        ip6err->ip6_src = q6->ip6q_src;
        ip6err->ip6_dst = q6->ip6q_dst;

        icmp6_error(merr, ICMP6_PARAM_PROB,
              ICMP6_PARAMPROB_HEADER,
              erroff - sizeof(struct ip6_frag) +
            offsetof(struct ip6_frag, ip6f_offlg));
      }
    }
  }

  ip6af = (struct ip6asfrag *)malloc(sizeof(struct ip6asfrag), M_FTABLE,
      M_DONTWAIT);
  if (ip6af == NULL)
    goto dropfrag;
  bzero(ip6af, sizeof(*ip6af));
  ip6af->ip6af_head = ip6->ip6_flow;
  ip6af->ip6af_len = ip6->ip6_plen;
  ip6af->ip6af_nxt = ip6->ip6_nxt;
  ip6af->ip6af_hlim = ip6->ip6_hlim;
  ip6af->ip6af_mff = ip6f->ip6f_offlg & IP6F_MORE_FRAG;
  ip6af->ip6af_off = fragoff;
  ip6af->ip6af_frglen = frgpartlen;
  ip6af->ip6af_offset = offset;
  IP6_REASS_MBUF(ip6af) = m;

  if (first_frag) {
    af6 = (struct ip6asfrag *)q6;
    goto insert;
  }

  /*
   * Find a segment which begins after this one does.
   */
  for (af6 = q6->ip6q_down; af6 != (struct ip6asfrag *)q6;
       af6 = af6->ip6af_down)
    if (af6->ip6af_off > ip6af->ip6af_off)
      break;

  #if 0
  /*
   * If there is a preceding segment, it may provide some of
   * our data already.  If so, drop the data from the incoming
   * segment.  If it provides all of our data, drop us.
   */
  if (af6->ip6af_up != (struct ip6asfrag *)q6) {
    i = af6->ip6af_up->ip6af_off + af6->ip6af_up->ip6af_frglen
      - ip6af->ip6af_off;
    if (i > 0) {
      if (i >= ip6af->ip6af_frglen)
        goto dropfrag;
      m_adj(IP6_REASS_MBUF(ip6af), i);
      ip6af->ip6af_off += i;
      ip6af->ip6af_frglen -= i;
    }
  }

  /*
   * While we overlap succeeding segments trim them or,
   * if they are completely covered, dequeue them.
   */
  while (af6 != (struct ip6asfrag *)q6 &&
         ip6af->ip6af_off + ip6af->ip6af_frglen > af6->ip6af_off) {
    i = (ip6af->ip6af_off + ip6af->ip6af_frglen) - af6->ip6af_off;
    if (i < af6->ip6af_frglen) {
      af6->ip6af_frglen -= i;
      af6->ip6af_off += i;
      m_adj(IP6_REASS_MBUF(af6), i);
      break;
    }
    af6 = af6->ip6af_down;
    m_freem(IP6_REASS_MBUF(af6->ip6af_up));
    frag6_deq(af6->ip6af_up);
  }
  #else
  /*
   * If the incoming framgent overlaps some existing fragments in
   * the reassembly queue, drop it, since it is dangerous to override
   * existing fragments from a security point of view.
   */
  if (af6->ip6af_up != (struct ip6asfrag *)q6) {
    i = af6->ip6af_up->ip6af_off + af6->ip6af_up->ip6af_frglen
      - ip6af->ip6af_off;
    if (i > 0) {
  #if 0        /* suppress the noisy log */
      log(LOG_ERR, "%d bytes of a fragment from %s "
          "overlaps the previous fragment\n",
          i, ip6_sprintf(&q6->ip6q_src));
  #endif
      free(ip6af, M_FTABLE);
      goto dropfrag;
    }
  }
  if (af6 != (struct ip6asfrag *)q6) {
    i = (ip6af->ip6af_off + ip6af->ip6af_frglen) - af6->ip6af_off;
    if (i > 0) {
  #if 0        /* suppress the noisy log */
      log(LOG_ERR, "%d bytes of a fragment from %s "
          "overlaps the succeeding fragment",
          i, ip6_sprintf(&q6->ip6q_src));
  #endif
      free(ip6af, M_FTABLE);
      goto dropfrag;
    }
  }
  #endif

insert:

  /*
   * Stick new segment in its place;
   * check for complete reassembly.
   * Move to front of packet queue, as we are
   * the most recently active fragmented packet.
   */
  frag6_enq(ip6af, af6->ip6af_up);
  #if 0 /* xxx */
  if (q6 != ip6q.ip6q_next) {
    frag6_remque(q6);
    frag6_insque(q6, &ip6q);
  }
  #endif
  next = 0;
  for (af6 = q6->ip6q_down; af6 != (struct ip6asfrag *)q6;
       af6 = af6->ip6af_down) {
    if (af6->ip6af_off != next) {
      frag6_doing_reass = 0;
      return IPPROTO_DONE;
    }
    next += af6->ip6af_frglen;
  }
  if (af6->ip6af_up->ip6af_mff) {
    frag6_doing_reass = 0;
    return IPPROTO_DONE;
  }

  /*
   * Reassembly is complete; concatenate fragments.
   */
  ip6af = q6->ip6q_down;
  t = m = IP6_REASS_MBUF(ip6af);
  af6 = ip6af->ip6af_down;
  frag6_deq(ip6af);
  while (af6 != (struct ip6asfrag *)q6) {
    af6dwn = af6->ip6af_down;
    frag6_deq(af6);
    while (t->m_next)
      t = t->m_next;
    t->m_next = IP6_REASS_MBUF(af6);
    m_adj(t->m_next, af6->ip6af_offset);
    free(af6, M_FTABLE);
    af6 = af6dwn;
  }

  /* adjust offset to point where the original next header starts */
  offset = ip6af->ip6af_offset - sizeof(struct ip6_frag);
  free(ip6af, M_FTABLE);
  ip6 = mtod(m, struct ip6_hdr *);
  ip6->ip6_plen = htons((u_short)next + offset - sizeof(struct ip6_hdr));
  ip6->ip6_src = q6->ip6q_src;
  ip6->ip6_dst = q6->ip6q_dst;
  nxt = q6->ip6q_nxt;
  #ifdef notyet
  *q6->ip6q_nxtp = (u_char)(nxt & 0xff);
  #endif

  /*
   * Delete frag6 header with as a few cost as possible.
   */
  if (offset < m->m_len) {
    ovbcopy((caddr_t)ip6, (caddr_t)ip6 + sizeof(struct ip6_frag),
      offset);
    m->m_data += sizeof(struct ip6_frag);
    m->m_len -= sizeof(struct ip6_frag);
  } else {
    /* this comes with no copy if the boundary is on cluster */
    if ((t = m_split(m, offset, M_DONTWAIT)) == NULL) {
      frag6_remque(q6);
      free(q6, M_FTABLE);
      frag6_nfragpackets--;
      goto dropfrag;
    }
    m_adj(t, sizeof(struct ip6_frag));
    m_cat(m, t);
  }

  /*
   * Store NXT to the original.
   */
  {
    char *prvnxtp = ip6_get_prevhdr(m, offset); /* XXX */
    *prvnxtp = nxt;
  }

  frag6_remque(q6);
  free(q6, M_FTABLE);
  frag6_nfragpackets--;

  if (m->m_flags & M_PKTHDR) { /* Isn't it always true? */
    int plen = 0;
    for (t = m; t; t = t->m_next)
      plen += t->m_len;
    m->m_pkthdr.len = plen;
  }
  
  ip6stat.ip6s_reassembled++;
  in6_ifstat_inc(dstifp, ifs6_reass_ok);

  /*
   * Tell launch routine the next header
   */

  *mp = m;
  *offp = offset;

  frag6_doing_reass = 0;
  return nxt;

 dropfrag:
  in6_ifstat_inc(dstifp, ifs6_reass_fail);
  ip6stat.ip6s_fragdropped++;
  m_freem(m);
  frag6_doing_reass = 0;
  return IPPROTO_DONE;
#endif
    
  *pp = p;    
  *offp = off;
  return 0;
}

#endif /* LWIP_IPV6 */
