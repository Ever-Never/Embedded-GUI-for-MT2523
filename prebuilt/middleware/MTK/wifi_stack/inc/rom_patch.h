/* Copyright Statement:
*
* (C) 2005-2017  MediaTek Inc. All rights reserved.
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

#ifndef __ROM_PATCH_H__
#define __ROM_PATCH_H__

#include "rt_config.h"

#ifdef WSC_FEATURE_SUPPORT
#include    "wsc.h"
#endif

#include "patch_config_ram.h"

#if 0
typedef unsigned short u16_t;
typedef unsigned char u8_t;
typedef unsigned int    UINT32;
typedef UINT32  wlan_status_t;
typedef unsigned int size_t;
#endif

#if 0
#define NET_BUF_128			0
#define NET_BUF_256			1
#define NET_BUF_512			2
#define NET_BUF_1024			3
#define NET_BUF_2048			4
#define NET_BUF_4096			5

struct pbuf_boson {
  /** next pbuf in singly linked pbuf chain */
  struct pbuf_boson *next;

  /** pointer to the actual data in the buffer */
  void *payload;

  /**
   * total length of this buffer and all next buffers in chain
   * belonging to the same packet.
   *
   * For non-queue packet chains this is the invariant:
   * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
   */
  unsigned short tot_len;

  /** length of this buffer */
  unsigned short len;

  /** pbuf_type as u8_t instead of enum to save space */
  unsigned char /*pbuf_type*/ type;

  /** misc flags */
  unsigned char flags;

  /**
   * the reference count always equals the number of pointers
   * that refer to this pbuf. This can be pointers from an application,
   * the stack itself, or pbuf->next pointers from a chain.
   */
  unsigned short ref;
};

struct net_buf_blk {
	/* These two member must be first */
	struct net_buf_blk*		 next;
	struct net_buf_blk_head* list;
	UINT32					size;
	UINT32					ref;
	UINT32					idx;
	UINT32					fid;	// FID of PSE buffer
	unsigned char* 			buf;
#if 1//Navaneeth IP stack integration
	struct pbuf_boson* pbuf_pkt;
#endif
};
#endif

typedef struct net_buf_blk * 	PBUF_BLK_OBJ;
typedef char *					PBUF_OBJ;

//char*	net_buf_blk_mem;

#define NET_PKT_CB_SIZE			20

#if (MT7682_WSAP00030486 == 1)
#if (MT7682_WSAP00031029 == 1)
#define SLEEP_COUNT_DOWN_TIME  5
#else
#define SLEEP_COUNT_DOWN_TIME  1
#endif
#endif

struct netif;
#if 0
struct net_pkt_blk {
	struct net_pkt_blk*		next;
	struct net_pkt_blk_head* list;
	struct netif*			dev;
	unsigned int			len;
	unsigned char*			head;	/* Head of buffer    */
	unsigned char*			data;	/* Data head pointer */
	unsigned char*			tail;	/* Tail pointer      */
	unsigned char*			end;	/* End pointer       */
	struct net_buf_blk* 	buf_blk;
	unsigned char			cb[NET_PKT_CB_SIZE];
};


struct net_pkt_blk_head {
	struct net_pkt_blk*	next;
	unsigned int 		qlen;
};


struct net_buf_blk_head {
	struct net_buf_blk*	next;
	unsigned int 		qlen;
};

struct net_buf_type  {
	UINT32 	nblk;
	char*	buf_mem;
	struct net_buf_blk_head freelist;
	struct net_pkt_blk_head pkt_list;
};
#endif
/****************************************************************************
 *
 * STATIC functions
 *
 ****************************************************************************/
#if 0

typedef enum {
  PBUF_TRANSPORT,
  PBUF_IP,
  PBUF_LINK,
  PBUF_RAW_TX,
  PBUF_RAW
} pbuf_layer;
typedef enum {
  /** pbuf data is stored in RAM, used for TX mostly, struct pbuf and its payload
      are allocated in one piece of contiguous memory (so the first payload byte
      can be calculated from struct pbuf)
      pbuf_alloc() allocates PBUF_RAM pbufs as unchained pbufs (although that might
      change in future versions) */
  PBUF_RAM,
  /** pbuf data is stored in ROM, i.e. struct pbuf and its payload are located in
      totally different memory areas. Since it points to ROM, payload does not
      have to be copied when queued for transmission. */
  PBUF_ROM,
  /** pbuf comes from the pbuf pool. Much like PBUF_ROM but payload might change
      so it has to be duplicated when queued before transmitting, depending on
      who has a 'ref' to it. */
  PBUF_REF,
  /** pbuf payload refers to RAM. This one comes from a pool and should be used
      for RX. Payload can be chained (scatter-gather RX) but like PBUF_RAM, struct
      pbuf and its payload are allocated in one piece of contiguous memory (so
      the first payload byte can be calculated from struct pbuf) */
  PBUF_POOL
} pbuf_type;
struct pbuf;
struct netif;
#endif

/* definition of pAd->OpMode */
#define OPMODE_STA                  0
#define OPMODE_AP                   1


extern void (*connsys_kickout_tx_data_87)(IN PRTMP_ADAPTER pAd, IN TX_BLK *pTxBlk);
extern void (*connsys_kickout_tx_mgmt_87)(IN	PRTMP_ADAPTER pAd, IN struct net_pkt_blk *pkt,BOOLEAN Keep_pkt);
extern int rom_init(void);


#if 1
extern VOID __patch_WpaStateMachineInit(
    IN  PRTMP_ADAPTER   pAd,
    IN  STATE_MACHINE *S,
    OUT STATE_MACHINE_FUNC Trans[]) ;
extern VOID __patch_WpaMicFailureReportFrame(
IN  PRTMP_ADAPTER   pAd,
IN	UCHAR			*Msg,
IN	UCHAR			*CurrentAddress,
IN	UCHAR			*Bssid) ;
extern VOID __patch_EnqueueStartForPSKExec(
    IN PVOID SystemSpecific1,
    IN PVOID FunctionContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3) ;
extern VOID __patch_WPARetryExec(
    IN PVOID SystemSpecific1,
    IN PVOID FunctionContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3) ;
extern VOID __patch_CMTimerExec(
    IN PVOID SystemSpecific1,
    IN PVOID FunctionContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3) ;
extern VOID __patch_GREKEYPeriodicExec(
    IN PVOID SystemSpecific1,
    IN PVOID FunctionContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3) ;
extern VOID	__patch_WPA_APSetGroupRekeyAction(
	IN  PRTMP_ADAPTER   pAd);

extern VOID __patch_APStartUpForMbss(RTMP_ADAPTER *pAd,BSS_STRUCT *pMbss);

extern VOID __patch_APMlmeDeauthReqAction(
    IN PRTMP_ADAPTER pAd,
    IN MLME_QUEUE_ELEM *Elem) ;

BOOLEAN
__patch_rlmSetChannel(
    ENUM_BAND_T             eBand,
    UINT_8                  ucPriChannel,
    ENUM_CHNL_EXT_T         eExtend,
    UINT_8                  ucChannelS1,
    UINT_8                  ucChannelS2,
    ENUM_CHANNEL_WIDTH_T    eChannelWidth,
    ENUM_RECAL_TYPE         eReCal,
    UINT_32                 eDbdcBandIdx
    );

#endif

#define IOT_PID_AP_PSPOLL						0x0f

#ifndef E2_ROM_7682
#if (MT7687_BORA00005151 == 1)
#define OBSS_SCAN_TIME						200 	/* 200 miliseconds */
#endif
#endif
#ifdef WSC_FEATURE_SUPPORT

WSC_ADAPTER_T * __patch_WscGetAdapter();

VOID __patch_WscBuildAssocReqIE(
	/*IN	PRTMP_ADAPTER	pAd,*/
	IN  PWSC_CTRL	pWscControl,
    OUT	PUCHAR			pOutBuf,
    OUT	PUCHAR			pIeLen);

VOID __patch_WscBuildProbeReqIE(
        IN  PRTMP_ADAPTER   pAd,
        IN  UCHAR           CurOpMode,
        OUT PUCHAR          pOutBuf,
        OUT PUCHAR          pIeLen);

VOID    __patch_WscSendEapolStart(
    IN  PRTMP_ADAPTER   pAdapter,
    IN  PUCHAR          pBssid,
    IN  UCHAR           CurOpMode);

VOID    __patch_WscStateMachineInit(
    IN	PRTMP_ADAPTER		pAd,
    IN  STATE_MACHINE       *S,
    OUT STATE_MACHINE_FUNC  Trans[]);


BOOL __patch_WscIsSelectedRegistrar(
   void *ie_buf);


VOID  __patch_WpsSmProcess(
        IN PRTMP_ADAPTER        pAd,
        IN MLME_QUEUE_ELEM     *Elem);

BOOLEAN __patch_WscMsgTypeSubst(
    IN  UCHAR   EAPType,
    IN  UCHAR   EAPCode,
    OUT INT     *MsgType);

#endif

VOID __patch_APPeerAuthConfirmAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

VOID __patch_APPeerAuthReqAtIdleAction(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
#endif
