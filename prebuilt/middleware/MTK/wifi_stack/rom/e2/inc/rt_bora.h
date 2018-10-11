/***********************************************************************/
/*                                                                     */
/*   Program:    rt_ucos.c                                             */
/*   Created:    4/21/2006 1:17:38 PM                                  */
/*   Author:     Wu Xi-Kun                                             */
/*   Comments:   Porting Routines for uCOS-II                          */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/* History:                                                            */
/*    Revision 1.1 4/21/2006 1:17:38 PM  xsikun                        */
/*    Initial revision                                                 */
/*                                                                     */
/***********************************************************************/
#ifndef __RT_BORA_H__
#define __RT_BORA_H__
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif

#include "rom/rt_pkt.h"
#include "wireless.h"

#if N9_PORTING

#include "hal_pwr_mgt_rom.h" // "delay.h"

//#include "regs.h"
#include "init.h"
#endif
#include "net.h"
#include "New_Prototype.h"
//#include "rom/connsys_profile.h"

extern unsigned int jiffies;

//Raghav#define CONFIG_5G_SUPPORT

//#define RTMP_INTERNAL_TX_ALC	/* enable iNIC Internal Tx ALC */
//extern unsigned char TX_RING_SIZE;
//extern unsigned char  MAX_TX_PROCESS;
//#define MAX_LEN_OF_MLME_QUEUE   5
//#define MGMT_DMA_BUFFER_SIZE    1024  move to rtmp_def.h

//#define CONFIG_RX_MEMCOPY       1  // RX memory copy for throughput optimization

#if 0
#define MGMT_RING_SIZE          1  //4 //64
#endif
//#define DEF_TX_RING_SIZE    2 // 16  //32 //12 //96
//#define RX_RING_SIZE            2  // 4   //32 //8 //32 //64
//#define MAX_RX_PROCESS          2 //32 //8 //64 //32
#define MAX_TX_PROCESS(_pAd)          (_pAd->MaxTxProcess)
#define TX_RING_SIZE(_pAd)            (_pAd->TxRingSize)


//#define RX_SCATTERED
#define RX_SCATTER_SIZE		(1024*2)
//#define CONFIG_CKIP_SUPPORT
// 

#define HZ              1024
#define OS_HZ			HZ

#define ETH_LENGTH_OF_ADDRESS	6

#define IN
#define OUT
#define INOUT


//#define TRUE        1
//#define FALSE       0
#define ERR_OK      0
#define ERR_MEM     -1      /* Out of memory error.     */

#define NDIS_STATUS                             INT
#define NDIS_STATUS_SUCCESS                     0x00
#define NDIS_STATUS_FAILURE                     0x01
#define NDIS_STATUS_INVALID_DATA				0x02
#define NDIS_STATUS_RESOURCES                   0x03
#define NDIS_STATUS_PENDING                     0x04

#define MIN_NET_DEVICE_FOR_AID			0x00		//0x00~0x3f
#define MIN_NET_DEVICE_FOR_MBSSID		0x00		//0x00,0x10,0x20,0x30
#define MIN_NET_DEVICE_FOR_WDS			0x10		//0x40,0x50,0x60,0x70
#define MIN_NET_DEVICE_FOR_APCLI		0x20

#ifdef CONFIG_STA_SUPPORT
#define NDIS_PACKET_TYPE_DIRECTED		0
#define NDIS_PACKET_TYPE_MULTICAST		1
#define NDIS_PACKET_TYPE_BROADCAST		2
#define NDIS_PACKET_TYPE_ALL_MULTICAST	3
#endif // CONFIG_STA_SUPPORT //

#define dma_addr_t UINT32

typedef struct netif		* PNET_DEV;
typedef void				* PNDIS_PACKET;
typedef char				NDIS_PACKET;
typedef PNDIS_PACKET		* PPNDIS_PACKET;
typedef	dma_addr_t			NDIS_PHYSICAL_ADDRESS;
typedef	dma_addr_t			* PNDIS_PHYSICAL_ADDRESS;

//typedef struct os_lock		NDIS_SPIN_LOCK;
typedef UINT32 NDIS_SPIN_LOCK;
typedef struct timer_list	NDIS_MINIPORT_TIMER;
#if N9_INIT_PORTING
#define init_timer(x,y,d1) cnmTimerInitTimer(x,y,d1)
#define add_timer(x,y)        cnmTimerStartTimer(x,y)
#define mod_timer(x,y)        cnmTimerStartTimer(x,y)
#define del_timer_sync(x)     cnmTimerStopTimer(x)
#define del_timer             del_timer_sync
#endif

#define udelay(x)				cos_delay_clock_ticks(x)
typedef void				* NDIS_HANDLE;
typedef char 				* PNDIS_BUFFER;
typedef unsigned int	NDIS_MEDIA_STATE;
typedef int             error_t;
typedef struct timer_list	RTMP_OS_TIMER;

#define CHIP_NAME "MT7637"

#ifdef CONFIG_STA_SUPPORT
#define NIC_DEVICE_NAME	CHIP_NAME "MT-WIFI STA"
#define DRIVER_VERSION          "0.0.0.1"
#endif // CONFIG_STA_SUPPORT //

#define OS_NTOHS(_Val) SWAP16(_Val)
#define OS_HTONS(_Val) SWAP16(_Val)
#define OS_NTOHL(_Val) SWAP32(_Val)
#define OS_HTONL(_Val) SWAP32(_Val)



#define skb_cloned(_x)		((_x->buf_blk->ref) > 1)
////////////////////////////////////////
// MOVE TO rtmp.h ?
/////////////////////////////////////////
#define PKTSRC_NDIS             0x7f
#define PKTSRC_DRIVER           0x0f
#define PRINT_MAC(addr)	\
	addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

//////////////////////////////////////////
//
//////////////////////////////////////////


#define NdisMIndicateStatus(_w, _x, _y, _z)
#define NdisMediaStateConnected		1
#define NdisMediaStateDisconnected	0

typedef struct  _RALINK_TIMER_STRUCT
{
	//TimerHandle_t       TimerObj;		// Ndis Timer object
	void                    *TimerObj;		// Ndis Timer object
	wifi_timer_token_t 	TimerToken;    
	ULONG               TimerValue;		// Timer value in milliseconds
/*
	BOOLEAN             Valid;			// Set to True when call RTMPInitTimer
	BOOLEAN             State;			// True if timer cancelled
	BOOLEAN             PeriodicType;	// True if timer is periodic timer 
	BOOLEAN             Repeat;			// timer do again
	ULONG               TimerValue;		// Timer value in milliseconds
	ULONG               cookie;			// os specific object
*/
}   RALINK_TIMER_STRUCT, *PRALINK_TIMER_STRUCT;

//typedef TimerHandle_t RALINK_TIMER_STRUCT, *PRALINK_TIMER_STRUCT;


#define timer_list _RALINK_TIMER_STRUCT

/**
 * container_of - cast a member of a structure out to the containing structure
 *
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})

/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */
#ifdef IAR_COMPILE
#define RTMP_TIME_AFTER(a,b)		\
	(((long)(b) - (long)(a) < 0))

#define RTMP_TIME_AFTER_EQ(a,b)	\
	 (((long)(a) - (long)(b) >= 0))

#else
#define typecheck(type,x) \
({	type __dummy; \
	typeof(x) __dummy2; \
	(void)(&__dummy == &__dummy2); \
	1; \
})

#define RTMP_TIME_AFTER(a,b)		\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(b) - (long)(a) < 0))
#define RTMP_TIME_BEFORE(a,b)	RTMP_TIME_AFTER_EQ(b,a)

#define RTMP_TIME_AFTER_EQ(a,b)	\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(a) - (long)(b) >= 0))
#endif//IAR_COMPILE


#define RTMP_TIME_BEFORE(a,b)	RTMP_TIME_AFTER_EQ(b,a)

//
//  MACRO for debugging information
//

#if 0 //def DBG
extern ULONG    RTDebugLevel;
#define DBGPRINT_RAW(Level, Fmt)    \
{                                   \
    if (Level <= RTDebugLevel)      \
    {                               \
        printk Fmt;               \
    }                               \
}

#define DBGPRINT(Level, Fmt)    DBGPRINT_RAW(Level, Fmt)


#define DBGPRINT_ERR(Fmt)           \
{                                   \
    printk("ERROR!!! ");          \
    printk Fmt;                  \
}

#define DBGPRINT_S(Status, Fmt)		\
{									\
	printk Fmt;					\
}
			

//#else
#define DBGPRINT(Level, Fmt)
#define DBGPRINT_RAW(Level, Fmt)
#define DBGPRINT_S(Status, Fmt)
#define DBGPRINT_ERR(Fmt)
#endif


//
//  spin_lock enhanced for Nested spin lock
//
#define NdisAllocateSpinLock(__pAd,__lock)

#define NdisFreeSpinLock(lock)          \
{                                       \
}


//  spin_lock_bh(&(__lock)->lock);              

#define LOCAL_IRQ_LOCK(x)		    // SYSirq_Disable_Interrupts_Save_Flags
#define LOCAL_IRQ_UNLOCK(x)			// SYSirq_Interrupts_Restore_Flags
#define OSSchedLock(x)             // LOCAL_IRQ_LOCK 
#define OSSchedUnlock(x)           // LOCAL_IRQ_UNLOCK

//  spin_unlock_bh(&(__lock)->lock);   


#define RTMP_IRQ_LOCK(__lock, __irqflag)					\
{													\
	LOCAL_IRQ_LOCK(__lock);	\
}

#define RTMP_IRQ_UNLOCK(__lock, __irqflag)						\
{														\
	LOCAL_IRQ_UNLOCK(*(__lock));	\
}

#define RTMP_INT_LOCK(__lock, __irqflag)					\
{													\
	LOCAL_IRQ_LOCK(__lock);	\
}

#define RTMP_INT_UNLOCK(__lock, __irqflag)						\
{														\
	LOCAL_IRQ_UNLOCK(*(__lock));	\
}

#define RTMP_SEM_LOCK(__lock)					\
{												\
    RTMP_IRQ_LOCK(__lock, 0);    \
}

#define RTMP_SEM_UNLOCK(__lock)					\
{												\
    RTMP_IRQ_UNLOCK(__lock, 0);    \
}

#if 0 //raghav
#define RTMP_IO_READ32(_A, _R, _pV)								\
{																\
	*_pV = REG32(BASE_MAC + _R);		\
}
#define RTMP_IO_WRITE32(_A, _R, _V)												\
{																\
	REG32(BASE_MAC + _R) = _V;							\
}
#endif

#define OS_WAIT(time) \
    cos_delay_clock_ticks(32*time);	//	OSTimeDly((time*HZ)/1000);

/* Modified by Wu Xi-Kun 4/21/2006 */
typedef void (*TIMER_FUNCTION)(unsigned int);

#ifndef WIFI_BUILD_RAM
// #define COPY_MAC_ADDR(Addr1, Addr2)             rom_memcpy((Addr1), (Addr2), MAC_ADDR_LEN)
#endif
#define MlmeAllocateMemory(_pAd, _ppVA) os_alloc_mem(_pAd, _ppVA, MGMT_DMA_BUFFER_SIZE)
#define MlmeFreeMemory(_pAd, _pVA)     os_free_mem(_pAd, _pVA)

#define GET_TIMER_FUNCTION(_func)				\
		_func							

#if N9_INIT_PORTING

#define BUILD_TIMER_FUNCTION(_func)												\
void bora_##_func(unsigned long data)											\
{																				\
	PRALINK_TIMER_STRUCT	pTimer = (PRALINK_TIMER_STRUCT) data;				\
																				\
	if (pTimer->Repeat)															\
		RTMP_OS_Add_Timer(&pTimer->TimerObj, pTimer->TimerValue);				\
	_func(NULL, (PVOID) pTimer->cookie, NULL, NULL); 							\
}


#define DECLARE_TIMER_FUNCTION(_func)			\
void bora_##_func(unsigned long data)			

#define GET_TIMER_FUNCTION(_func)				\
		bora_##_func							

#ifdef CONFIG_AP_SUPPORT
DECLARE_TIMER_FUNCTION(APDetectOverlappingExec);
#endif // CONFIG_AP_SUPPORT //
DECLARE_TIMER_FUNCTION(MlmePeriodicExec);
DECLARE_TIMER_FUNCTION(MlmeRssiReportExec);
DECLARE_TIMER_FUNCTION(AsicRxAntEvalTimeout);

#ifdef CONFIG_AP_SUPPORT
DECLARE_TIMER_FUNCTION(GREKEYPeriodicExec);
DECLARE_TIMER_FUNCTION(CMTimerExec);
DECLARE_TIMER_FUNCTION(APQuickResponeForRateUpExec);
DECLARE_TIMER_FUNCTION(WPARetryExec);
DECLARE_TIMER_FUNCTION(EnqueueStartForPSKExec);
DECLARE_TIMER_FUNCTION(APScanTimeout);
#ifdef IDS_SUPPORT
DECLARE_TIMER_FUNCTION(RTMPIdsPeriodicExec);
#endif // IDS_SUPPORT //
#ifdef WSC_AP_SUPPORT
DECLARE_TIMER_FUNCTION(WscEAPOLTimeOutAction);
DECLARE_TIMER_FUNCTION(Wsc2MinsTimeOutAction);
DECLARE_TIMER_FUNCTION(WscEnqueueEapolStart);
#endif // WSC_AP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
DECLARE_TIMER_FUNCTION(BeaconTimeout);
DECLARE_TIMER_FUNCTION(ScanTimeout);
DECLARE_TIMER_FUNCTION(AuthTimeout);
DECLARE_TIMER_FUNCTION(AssocTimeout);
DECLARE_TIMER_FUNCTION(ReassocTimeout);
DECLARE_TIMER_FUNCTION(DisassocTimeout);
DECLARE_TIMER_FUNCTION(LinkDownExec);
#ifdef LEAP_SUPPORT
DECLARE_TIMER_FUNCTION(LeapAuthTimeout);
#endif
DECLARE_TIMER_FUNCTION(StaQuickResponeForRateUpExec);
DECLARE_TIMER_FUNCTION(WpaDisassocApAndBlockAssoc);
#ifdef WSC_STA_SUPPORT
DECLARE_TIMER_FUNCTION(WscEAPOLTimeOutAction);
DECLARE_TIMER_FUNCTION(Wsc2MinsTimeOutAction);
DECLARE_TIMER_FUNCTION(WscPBCTimeOutAction);
DECLARE_TIMER_FUNCTION(WscScanTimeOutAction);
DECLARE_TIMER_FUNCTION(WscProfileRetryTimeout);
#endif // WSC_STA_SUPPORT //

#ifdef PS_ENHANCE
DECLARE_TIMER_FUNCTION(EPS_CoreDecisonMaker);
#endif // PS_ENHANCE //

#endif // CONFIG_STA_SUPPORT //
#endif
/*
 * packet helper 
 * 	- convert internal rt packet to os packet or 
 *             os packet to rt packet
 */      
#define RTPKT_TO_OSPKT(_p)		((struct net_pkt_blk *)(_p))
#define OSPKT_TO_RTPKT(_p)		((PNDIS_PACKET)(_p))

#define SET_OS_PKT_NETDEV(_pkt, _pNetDev)	\
		(RTPKT_TO_OSPKT(_pkt)->dev) = (_pNetDev)
#define GET_OS_PKT_DATAPTR(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->data)
#define SET_OS_PKT_DATAPTR(_pkt, _dataPtr)	\
			(RTPKT_TO_OSPKT(_pkt)->data) = (_dataPtr)

#define GET_OS_PKT_LEN(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->len)
#define SET_OS_PKT_LEN(_pkt, _len)	\
			(RTPKT_TO_OSPKT(_pkt)->len) = (_len)


#define GET_OS_PKT_DATATAIL(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->tail)
#define SET_OS_PKT_DATATAIL(_pkt, _start, _len)	\
			((RTPKT_TO_OSPKT(_pkt))->tail) = (PUCHAR)((_start) + (_len))

#define GET_OS_PKT_HEAD(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->head)

#define GET_OS_PKT_END(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->end)

#define GET_OS_PKT_NETDEV(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->dev)


#define GET_OS_PKT_TYPE(_pkt) \
		(RTPKT_TO_OSPKT(_pkt))

#define GET_OS_PKT_NEXT(_pkt) \
		(RTPKT_TO_OSPKT(_pkt)->next)

/* statistics counter */
#define STATS_INC_RX_PACKETS(_pAd, _dev)
#define STATS_INC_TX_PACKETS(_pAd, _dev)

#define STATS_INC_RX_BYTESS(_pAd, _dev, len)
#define STATS_INC_TX_BYTESS(_pAd, _dev, len)

#define STATS_INC_RX_ERRORS(_pAd, _dev)
#define STATS_INC_TX_ERRORS(_pAd, _dev)

#define STATS_INC_RX_DROPPED(_pAd, _dev)
#define STATS_INC_TX_DROPPED(_pAd, _dev)


#define CB_OFF  0


//   check DDK NDIS_PACKET data structure and find out only MiniportReservedEx[0..7] can be used by our driver without
//   ambiguity. Fields after pPacket->MiniportReservedEx[8] may be used by other wrapper layer thus crashes the driver
//

// User Priority
#define RTMP_SET_PACKET_UP(_p, _prio)			(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+0] = _prio)
#define RTMP_GET_PACKET_UP(_p)					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+0])

// Fragment #
#define RTMP_SET_PACKET_FRAGMENTS(_p, _num)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+1] = _num)   
#define RTMP_GET_PACKET_FRAGMENTS(_p)			(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+1])

// 0x0 ~0x7f: TX to AP's own BSS which has the specified AID. if AID>127, set bit 7 in RTMP_SET_PACKET_EMACTAB too. 
//(this value also as MAC(on-chip WCID) table index)
// 0x80~0xff: TX to a WDS link. b0~6: WDS index
#define RTMP_SET_PACKET_WCID(_p, _wdsidx)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+2] = _wdsidx)
#define RTMP_GET_PACKET_WCID(_p)          		((UCHAR)(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+2]))

// 0xff: PKTSRC_NDIS, others: local TX buffer index. This value affects how to a packet
#define RTMP_SET_PACKET_SOURCE(_p, _pktsrc)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+3] = _pktsrc)
#define RTMP_GET_PACKET_SOURCE(_p)       		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+3])  

// RTS/CTS-to-self protection method
#define RTMP_SET_PACKET_RTS(_p, _num)      		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+4] = _num)
#define RTMP_GET_PACKET_RTS(_p)          		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+4])  
// see RTMP_S(G)ET_PACKET_EMACTAB

// TX rate index
#define RTMP_SET_PACKET_TXRATE(_p, _rate)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+5] = _rate)
#define RTMP_GET_PACKET_TXRATE(_p)		  		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+5])

// From which Interface 
#define RTMP_SET_PACKET_IF(_p, _ifdx)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+6] = _ifdx)
#define RTMP_GET_PACKET_IF(_p)		  		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+6])

#define RTMP_SET_PACKET_NET_DEVICE_MBSSID(_p, _bss)		RTMP_SET_PACKET_IF((_p), (_bss))
#define RTMP_SET_PACKET_NET_DEVICE_WDS(_p, _bss)		RTMP_SET_PACKET_IF((_p), ((_bss) + MIN_NET_DEVICE_FOR_WDS))
#define RTMP_SET_PACKET_NET_DEVICE_APCLI(_p, _idx)   	RTMP_SET_PACKET_IF((_p), ((_idx) + MIN_NET_DEVICE_FOR_APCLI))
#define RTMP_GET_PACKET_NET_DEVICE_MBSSID(_p)			RTMP_GET_PACKET_IF((_p))
#define RTMP_GET_PACKET_NET_DEVICE(_p)					RTMP_GET_PACKET_IF((_p))

#define RTMP_SET_PACKET_MOREDATA(_p, _morebit)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+7] = _morebit)
#define RTMP_GET_PACKET_MOREDATA(_p)				(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+7])

//#define RTMP_SET_PACKET_NET_DEVICE_MBSSID(_p, _bss)	(RTPKT_TO_OSPKT(_p)->cb[8] = _bss)
//#define RTMP_GET_PACKET_NET_DEVICE_MBSSID(_p)		(RTPKT_TO_OSPKT(_p)->cb[8])

/* [CB_OFF + 3]  */
/* From which Interface, wdev index */
#define RTMP_SET_PACKET_WDEV(_p, _wdev_idx)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+8] = _wdev_idx)
#define RTMP_GET_PACKET_WDEV(_p)					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+8])



/* [CB_OFF + 12]  */
#define RTMP_SET_PACKET_TXTYPE(_p, _mode)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+12] = _mode)
#define RTMP_GET_PACKET_TXTYPE(_p)			(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+12])


/* use bit3 of cb[CB_OFF+16] */
#define RTMP_SET_PACKET_MGMT_PKT(_p, _flg)	     (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+13] = (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+13]&0xF7)|((_flg & 0x01) << 3)) 
#define RTMP_GET_PACKET_MGMT_PKT(_p)				\
		(((RTPKT_TO_OSPKT(_p)->cb[CB_OFF+13]) & 0x08) >> 3)

/* use bit0 of cb[CB_OFF+20] */
#define RTMP_SET_PACKET_MGMT_PKT_DATA_QUE(_p, _flg)	\
        (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+14] = (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+14] & 0xFE) | (_flg & 0x01))
#define RTMP_GET_PACKET_MGMT_PKT_DATA_QUE(_p)		\
		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+14] & 0x01)


/* [CB_OFF + 13]  */
/* TX rate index */
//#define RTMP_SET_PACKET_TXRATE(_p, _rate)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+13] = _rate)
//#define RTMP_GET_PACKET_TXRATE(_p)			(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+13])


/* [CB_OFF + 14]  */
#ifdef MAC_REPEATER_SUPPORT
#define RTMP_SET_PKT_MAT_FREE(_p, _flg)	(PACKET_CB(_p, 14) = (_flg))
#define RTMP_GET_PKT_MAT_FREE(_p)			(PACKET_CB(_p, 14))
#endif /* MAC_REPEATER_SUPPORT */

#define RELEASE_NDIS_PACKET(_pAd, _pPacket, _Status)                    \
{                                                                       \
        RTMPFreeNdisPacket(_pAd, _pPacket);                             \
}


//
//	Sepcific Pakcet Type definition
//
#define RTMP_PACKET_SPECIFIC_CB_OFFSET	11

#define RTMP_PACKET_SPECIFIC_DHCP		0x01
#define RTMP_PACKET_SPECIFIC_EAPOL		0x02
#define RTMP_PACKET_SPECIFIC_IPV4		0x04
#define RTMP_PACKET_SPECIFIC_VLAN		0x10
#define RTMP_PACKET_SPECIFIC_LLCSNAP	0x20

//Specific
#define RTMP_SET_PACKET_SPECIFIC(_p, _flg)	   	(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11] = _flg)												

//DHCP
#define RTMP_SET_PACKET_DHCP(_p, _flg)   													\
			do{																				\
				if (_flg)																	\
					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_DHCP);		\
				else																		\
					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) &= (!RTMP_PACKET_SPECIFIC_DHCP);	\
			}while(0)
#define RTMP_GET_PACKET_DHCP(_p)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_DHCP)

//EAPOL
#define RTMP_SET_PACKET_EAPOL(_p, _flg)   													\
			do{																				\
				if (_flg)																	\
					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_EAPOL);		\
				else																		\
					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) &= (!RTMP_PACKET_SPECIFIC_EAPOL);	\
			}while(0)
#define RTMP_GET_PACKET_EAPOL(_p)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_EAPOL)

#define RTMP_GET_PACKET_LOWRATE(_p)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11] & (RTMP_PACKET_SPECIFIC_EAPOL | RTMP_PACKET_SPECIFIC_DHCP))

//VLAN
#define RTMP_SET_PACKET_VLAN(_p, _flg)   													\
			do{																				\
				if (_flg)																	\
					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_VLAN);		\
				else																		\
					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) &= (!RTMP_PACKET_SPECIFIC_VLAN);	\
			}while(0)
#define RTMP_GET_PACKET_VLAN(_p)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_VLAN)

//LLC/SNAP
#define RTMP_SET_PACKET_LLCSNAP(_p, _flg)   												\
			do{																				\
				if (_flg)																	\
					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_LLCSNAP);	\
				else																		\
					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) &= (!RTMP_PACKET_SPECIFIC_LLCSNAP);	\
			}while(0)
			
#define RTMP_GET_PACKET_LLCSNAP(_p)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_LLCSNAP)

// IP
#define RTMP_SET_PACKET_IPV4(_p, _flg)														\
			do{																				\
				if (_flg)																	\
					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) |= (RTMP_PACKET_SPECIFIC_IPV4);		\
				else																		\
					(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11]) &= (!RTMP_PACKET_SPECIFIC_IPV4);	\
			}while(0)
			
#define RTMP_GET_PACKET_IPV4(_p)		(RTPKT_TO_OSPKT(_p)->cb[CB_OFF+11] & RTMP_PACKET_SPECIFIC_IPV4)


// If this flag is set, it indicates that this EAPoL frame MUST be clear. 
#define RTMP_SET_PACKET_CLEAR_EAP_FRAME(_p, _flg)   (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+12] = _flg)
#define RTMP_GET_PACKET_CLEAR_EAP_FRAME(_p)         (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+12])

// Note cb was declare only have size = 16, cb[15] is the max number
#define RTMP_SET_PACKET_UCOS(_p, _flg)   (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+15] = _flg)
#define RTMP_GET_PACKET_UCOS(_p)         (RTPKT_TO_OSPKT(_p)->cb[CB_OFF+15])

#define RTMP_GET_PACKET_5VT(_p) 0

#define NDIS_SET_PACKET_STATUS(_p, _status)


#define GET_SG_LIST_FROM_PACKET(_p, _sc)	\
    rt_get_sg_list_from_packet(_p, _sc)


//#define RT2860_CSR_ADDR 0xB0180000

#define	IFF_UP		0x1		/* interface is up		*/

#define MEM_ALLOC_FLAG	0
#ifndef GFP_ATOMIC
#define GFP_ATOMIC		0
#endif
#define GFP_KERNEL		0
#define GFP_DMA			0

#define kfree iot_free
#define kmalloc(x, y) iot_malloc(x)
#define copy_to_user(x,y,z) (!rom_memcpy(x,y,z))
//#define copy_to_user(x,y,z) IoctlResponse((PUCHAR)x, (PUCHAR)y, z)

#define vmalloc(x) iot_malloc(x)
#define vfree iot_free


#define skb_copy(_x, _y) 		net_pkt_copy(_x)
#define skb_put(_x, _y)			net_pkt_put(_x, _y)
#define skb_clone(_x, _y)		net_pkt_clone(_x)
#define skb_push(_x, _y)		net_pkt_push(_x, _y)
#define skb_reserve(_x, _y)      net_pkt_reserve(_x, _y)

#define netif_stop_queue(dev)
#define netif_start_queue(dev)

#define NdisMoveMemory(Destination, Source, Length) rom_memmove(Destination, Source, Length)
#define NdisZeroMemory(Destination, Length)         rom_memset(Destination, 0, Length)
#define NdisFillMemory(Destination, Length, Fill)   rom_memset(Destination, Fill, Length)
#define NdisEqualMemory(Source1, Source2, Length)   (!rom_memcmp(Source1, Source2, Length))
#define NdisCopyMemory(Source1, Source2, Length)   (!rom_memcpy(Source1, Source2, Length))


#define RTMPEqualMemory(Source1, Source2, Length)	(!rom_memcmp(Source1, Source2, Length))
#define RTMPMoveMemory(Destination, Source, Length) rom_memmove(Destination, Source, Length)
#define RTMPZeroMemory(Destination, Length)			rom_memset(Destination, 0, Length)

#define RTMP_INC_REF(_A)		0
#define RTMP_DEC_REF(_A)		0
#define RTMP_GET_REF(_A)		0


#define APCLI_IF_UP_CHECK(pAd, ifidx) ((pAd)->ApCfg.ApCliTab[(ifidx)].wdev.if_dev != NULL)



/*
 * ULONG
 * RTMP_GetPhysicalAddressLow(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress);
 */
#define RTMP_GetPhysicalAddressLow(PhysicalAddress)		(PhysicalAddress)

/*
 * ULONG
 * RTMP_GetPhysicalAddressHigh(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress);
 */
#define RTMP_GetPhysicalAddressHigh(PhysicalAddress)		(0)

/*
 * VOID
 * RTMP_SetPhysicalAddressLow(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress,
 *   IN ULONG  Value);
 */
#define RTMP_SetPhysicalAddressLow(PhysicalAddress, Value)	\
			PhysicalAddress = Value;

/*
 * VOID
 * RTMP_SetPhysicalAddressHigh(
 *   IN NDIS_PHYSICAL_ADDRESS  PhysicalAddress,
 *   IN ULONG  Value);
 */
#define RTMP_SetPhysicalAddressHigh(PhysicalAddress, Value)


//CONTAINING_RECORD(pEntry, NDIS_PACKET, MiniportReservedEx);
#define QUEUE_ENTRY_TO_PACKET(pEntry) \
	(PNDIS_PACKET)(pEntry)

#define PACKET_TO_QUEUE_ENTRY(pPacket) \
	(PQUEUE_ENTRY)(pPacket)


#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field)			\
((type *)((PCHAR)(address) - offsetof(type, field)))
#endif


#define NdisWriteErrorLogEntry(_a, _b, _c, _d)
#define NdisMAllocateMapRegisters(_a, _b, _c, _d, _e)		NDIS_STATUS_SUCCESS


#define NdisAcquireSpinLock		RTMP_SEM_LOCK
#define NdisReleaseSpinLock		RTMP_SEM_UNLOCK

//pPacket = CONTAINING_RECORD(pEntry, NDIS_PACKET, MiniportReservedEx);
#define QUEUE_ENTRY_TO_PKT(pEntry) \
		((PNDIS_PACKET) (pEntry))


#define NETDEV_TO_RTMP_ADAPTER(x)	((x)->adapter)
//#define NETIF_IS_UP(x) netif_is_up(x)

#define GET_IOCTL_RQ_DATAPTR(x)	(x);
#define GET_IOCTL_RQ_DATALEN(x)     (rom_strlen(x) + 1)
#define access_ok(x, y, z)		(TRUE)

extern struct _RTMP_ADAPTER	rtmp_Adapter;

#define FLAG_BIG_ENDIAN (1 << 0)



#define DEV_TYPE_APCLI_FLAG 0x8000
typedef enum _IW_TYPE
{
    IW_NAME_TYPE=0,
    IW_POINT_TYPE,
    IW_PARAM_TYPE,
    IW_FREQ_TYPE,
    IW_MODE_TYPE,
    IW_QUALITY_TYPE,
    IW_SOCKADDR_TYPE,
	IW_U32_TYPE,
} IW_TYPE;

#define MAX_FEEDBACK_LEN							1024 //(1500 - 12)
#define ETH_HLEN                14 // DA(6) + SA(6) + PROTO(2)
#define RACFG_HEADER_SIZE       sizeof(struct racfghdr) // MAGIC(4) + CMD_TYPE(2) + CMD_ID(2) + CMD_LEN(2) + CMD
#define MAGIC_NUMBER            0x18142880



typedef struct addr_value
{
    UINT32 addr;
    UINT32 value;
    UINT8  isRead;
} AddrValue;

#define RACFG_SIGUSR1 1
#define RACFG_SIGUSR2 2
// will be remapped according to  the definition of the host side's platform
#define SIGUSR1			RACFG_SIGUSR1
#define SIGUSR2		    RACFG_SIGUSR2

/* Routing table calls. */
#define SIOCADDRT	0x890B		/* add routing table entry	*/
#define SIOCDELRT	0x890C		/* delete routing table entry	*/
#define SIOCRTMSG	0x890D		/* call to routing system	*/

/* Socket configuration controls. */
#define SIOCGIFNAME	0x8910		/* get iface name		*/
#define SIOCSIFLINK	0x8911		/* set iface channel		*/
#define SIOCGIFCONF	0x8912		/* get iface list		*/
#define SIOCGIFFLAGS	0x8913		/* get flags			*/
#define SIOCSIFFLAGS	0x8914		/* set flags			*/
#define SIOCGIFADDR	0x8915		/* get PA address		*/
#define SIOCSIFADDR	0x8916		/* set PA address		*/
#define SIOCGIFDSTADDR	0x8917		/* get remote PA address	*/
#define SIOCSIFDSTADDR	0x8918		/* set remote PA address	*/
#define SIOCGIFBRDADDR	0x8919		/* get broadcast PA address	*/
#define SIOCSIFBRDADDR	0x891a		/* set broadcast PA address	*/
#define SIOCGIFNETMASK	0x891b		/* get network PA mask		*/
#define SIOCSIFNETMASK	0x891c		/* set network PA mask		*/
#define SIOCGIFMETRIC	0x891d		/* get metric			*/
#define SIOCSIFMETRIC	0x891e		/* set metric			*/
#define SIOCGIFMEM	0x891f		/* get memory address (BSD)	*/
#define SIOCSIFMEM	0x8920		/* set memory address (BSD)	*/
#define SIOCGIFMTU	0x8921		/* get MTU size			*/
#define SIOCSIFMTU	0x8922		/* set MTU size			*/
#define SIOCSIFNAME	0x8923		/* set interface name */
#define	SIOCSIFHWADDR	0x8924		/* set hardware address 	*/
#define SIOCGIFENCAP	0x8925		/* get/set encapsulations       */
#define SIOCSIFENCAP	0x8926		
#define SIOCGIFHWADDR	0x8927		/* Get hardware address		*/
#define SIOCGIFSLAVE	0x8929		/* Driver slaving support	*/
#define SIOCSIFSLAVE	0x8930
#define SIOCADDMULTI	0x8931		/* Multicast address lists	*/
#define SIOCDELMULTI	0x8932
#define SIOCGIFINDEX	0x8933		/* name -> if_index mapping	*/
#define SIOGIFINDEX	SIOCGIFINDEX	/* misprint compatibility :-)	*/
#define SIOCSIFPFLAGS	0x8934		/* set/get extended flags set	*/
#define SIOCGIFPFLAGS	0x8935
#define SIOCDIFADDR	0x8936		/* delete PA address		*/
#define	SIOCSIFHWBROADCAST	0x8937	/* set hardware broadcast addr	*/
#define SIOCGIFCOUNT	0x8938		/* get number of devices */

#define SIOCGIFBR	0x8940		/* Bridging support		*/
#define SIOCSIFBR	0x8941		/* Set bridging options 	*/

#define SIOCGIFTXQLEN	0x8942		/* Get the tx queue length	*/
#define SIOCSIFTXQLEN	0x8943		/* Set the tx queue length 	*/

#define SIOCGIFDIVERT	0x8944		/* Frame diversion support */
#define SIOCSIFDIVERT	0x8945		/* Set frame diversion options */

#define SIOCETHTOOL	0x8946		/* Ethtool interface		*/

#define SIOCGMIIPHY	0x8947		/* Get address of MII PHY in use. */
#define SIOCGMIIREG	0x8948		/* Read MII PHY register.	*/
#define SIOCSMIIREG	0x8949		/* Write MII PHY register.	*/

#define SIOCWANDEV	0x894A		/* get/set netdev parameters	*/

/* ARP cache control calls. */
		    /*  0x8950 - 0x8952  * obsolete calls, don't re-use */
#define SIOCDARP	0x8953		/* delete ARP table entry	*/
#define SIOCGARP	0x8954		/* get ARP table entry		*/
#define SIOCSARP	0x8955		/* set ARP table entry		*/

/* RARP cache control calls. */
#define SIOCDRARP	0x8960		/* delete RARP table entry	*/
#define SIOCGRARP	0x8961		/* get RARP table entry		*/
#define SIOCSRARP	0x8962		/* set RARP table entry		*/

/* Driver configuration calls */

#define SIOCGIFMAP	0x8970		/* Get device parameters	*/
#define SIOCSIFMAP	0x8971		/* Set device parameters	*/

/* DLCI configuration calls */

#define SIOCADDDLCI	0x8980		/* Create new DLCI device	*/
#define SIOCDELDLCI	0x8981		/* Delete DLCI device		*/

#define SIOCGIFVLAN	0x8982		/* 802.1Q VLAN support		*/
#define SIOCSIFVLAN	0x8983		/* Set 802.1Q VLAN options 	*/

/* bonding calls */

#define SIOCBONDENSLAVE	0x8990		/* enslave a device to the bond */
#define SIOCBONDRELEASE 0x8991		/* release a slave from the bond*/
#define SIOCBONDSETHWADDR      0x8992	/* set the hw addr of the bond  */
#define SIOCBONDSLAVEINFOQUERY 0x8993   /* rtn info about slave state   */
#define SIOCBONDINFOQUERY      0x8994	/* rtn info about bond state    */
#define SIOCBONDCHANGEACTIVE   0x8995   /* update to a new active slave */
			
/* bridge calls */
#define SIOCBRADDBR     0x89a0		/* create new bridge device     */
#define SIOCBRDELBR     0x89a1		/* remove bridge device         */
#define SIOCBRADDIF	0x89a2		/* add interface to bridge      */
#define SIOCBRDELIF	0x89a3		/* remove interface from bridge */

/* Device private ioctl calls */

/*
 *	These 16 ioctls are available to devices via the do_ioctl() device
 *	vector. Each device should include this file and redefine these names
 *	as their own. Because these are device dependent it is a good idea
 *	_NOT_ to issue them to random objects and hope.
 *
 *	THESE IOCTLS ARE _DEPRECATED_ AND WILL DISAPPEAR IN 2.5.X -DaveM
 */
 
#define SIOCDEVPRIVATE	0x89F0	/* to 89FF */

/*
 *	These 16 ioctl calls are protocol private
 */
 
#define SIOCPROTOPRIVATE 0x89E0 /* to 89EF */

#if 0
// reserved for the future
#define RTMP_SEMAPHORE              OS_EVENT *
#define RTMP_COMPLETION             UCHAR        // not used
#define RTMP_INIT_MUTEX_LOCKED(x)   { x = OSSemCreate(0); }
#define RTMP_SEM_UP(x)              OSSemPost(x)
#define RTMP_SEM_DOWN(x)            { UCHAR err; OSSemPend(x, 0, &err); }

typedef enum {
	ACTIVE_QUEUE,
	PASSIVE_QUEUE	
} MSG_QUEUE_IDX;
#endif

//extern struct syscfg *SysCfg;
extern struct sys_cfg_t *SysCfg;

//USHORT get_two_byte_align(PUCHAR buff);
//ULONG   get_four_byte_align(PUCHAR buff);

#if 0
#define INIC_PREFIX_KEYWORD  "__SkipIfNotINIC__"
#define INIC_PREFIX(x) INIC_PREFIX_KEYWORD x

#define ether_setup(dev)  //netif_set_up(dev), in UCOS, mbss don't opened at init phase...
#define GET_OS_DEV_ADDR(dev)   (dev->hwaddr)
#define GET_OS_DEV_XMIT(dev)   (dev->pkt_xmit)
#endif
	extern struct netif *netif_list_wifi_n9;
	extern struct netif *netif_default_wifi_n9;


//extern int rt2860_close(IN struct netif *net_dev);
#if 0
#define alloc_netdev(size, name, setup) alloc_etherdev(size)
#define dev_put(dev) 
#endif

#define HEART_BEAT_PERIOD     1000
#define CONSOLE_FLUSH_PERIOD   300  // 300 ms


#define E64_TO_CPU(x)  (x)
#define CPU_TO_E64(x)  (x)
#define E32_TO_CPU(x)  (x)
#define CPU_TO_E32(x)  (x)
#define E16_TO_CPU(x)  (x)
#define CPU_TO_E16(x)  (x)

#if 0 
struct FRAG_BLK
{
	int 				cmd;
	int 				seq;
	struct net_pkt_blk	*pkt;
	//BOOLEAN				done; ==> useless
};
extern UINT8 NetJobAdd(void *fun, ...);
extern UINT8 NetJobUrgentAdd(void *fun, ...);
#endif


#define MAX_LEN_OF_SSID 32
#ifndef UCHAR
#define UCHAR unsigned char
#endif


#ifndef CIPHER_KEY_DEFINED
#if 0
typedef struct _CIPHER_KEY {
	UCHAR   Key[16];            // right now we implement 4 keys, 128 bits max
	UCHAR   RxMic[8];			// make alignment 
	UCHAR   TxMic[8];
	UCHAR   TxTsc[6];           // 48bit TSC value
	UCHAR   RxTsc[6];           // 48bit TSC value
	UCHAR   CipherAlg;          // 0-none, 1:WEP64, 2:WEP128, 3:TKIP, 4:AES, 5:CKIP64, 6:CKIP128
	UCHAR   KeyLen; 
	UCHAR   BssId[6];
	UCHAR   Type;               // Indicate Pairwise/Group when reporting MIC error
} CIPHER_KEY, *PCIPHER_KEY;
#endif
#define CIPHER_KEY_DEFINED
#endif

#if N9_INIT_PORTING
struct PACKED syscfg
{
	UCHAR bMacSync;
	UCHAR bDisableHeartBeat;
	//UCHAR bBigEndianHost;
	//UCHAR bCsumOffload;
	UCHAR bConsole;
	//UCHAR bLED;
	UCHAR bBADecline;
	UCHAR bLocalAdminMAC;
	/*--- 8 ---*/
	UCHAR MacAddr[6];
	char Ssid[MAX_LEN_OF_SSID];
	/*---46----*/
	UCHAR SsidLen;
	UCHAR BssType;
	UCHAR Channel;
	UCHAR WirelessMode;
	UCHAR AuthMode;
	UCHAR EncrypType;
	/*--- 52 ---*/
	UCHAR WpaPsk[64]; // for WSC
	UCHAR WpaPskLen;  // for WSC
	UCHAR PMK[32];
	/*---149---*/
	//UCHAR WepStatus;
	//UCHAR AdhocMode;
	UCHAR PairCipher;
	UCHAR GroupCipher;
	UCHAR DefaultKeyId;
	/*--- 154 ---*/
	CIPHER_KEY SharedKey[4];
	/*--- 154+53*4=366--- */
	UCHAR SharedKeyIdx;
	UCHAR AutoBA;
	UCHAR BW;
	UCHAR MCS;
	UCHAR DbgLevel;
	UCHAR PSMode;
	//UCHAR bQAControl;
	//UCHAR reserved;
	/* --- 374 --- */
	USHORT KeepAlivePeriod;
	/* --- 376 ---*/
	UCHAR bWmmCapable;
	UCHAR bAPSDCapable;
	UCHAR AckPolicy[4];
	UCHAR bAPSDAC_BE;
	UCHAR bAPSDAC_BK;
	UCHAR bAPSDAC_VI;
	UCHAR bAPSDAC_VO;
	/* ----386 ---*/
	UCHAR shortRetryLimit;
	UCHAR longRetryLimit;	
	//UCHAR TxRingSize;
	//UCHAR NumRxDesc;
	/* --- 390 --- */
	//UCHAR Gpio0HwRadio;
	//UCHAR Gpio0Polarity;	
	UCHAR CountryRegion;
	UCHAR CountryRegionABand;
	UCHAR FixedTxMode;	
	UCHAR ShortGI;
	//UCHAR bTxAggregate;
	UCHAR DisallowTKIP;
	UCHAR BSSCoexistence;
	//UCHAR AntDivertEnable;	
	UCHAR HtMode; 
	UCHAR bAutoRoaming;
	CHAR  dBmToRoam;   
	//UCHAR bRxAggregate;
	//UCHAR RxAggregateMax;
	//UCHAR SwAntDiversity;
#if 0 //def WOW_SUPPORT
	UCHAR bWowSupport;
	UCHAR bWowInputGpioSupport;
	UCHAR WowHoldTimePeriod;
	UCHAR WowInbandInterval; /* the WOW host heart-beat timer interval, and also iNIC monitor interval */	
#endif // WOW_SUPPORT		
	UCHAR IEEE80211H;  
	UCHAR bRadioOff;  
	//UCHAR bFreqSwitchOff;  	
	USHORT KeepScanPeriod;
	USHORT built_in_bridge;
	//UCHAR bEPS_FeatureFlag;  	
	//ULONG EPS_TriggerAliveThreshold;
	//ULONG EPS_TriggerAsleepThreshold;
};
#endif


#define add_netdevice(dev, pAd, setup)           \
{                                               \
    netif_add_wifi_n9(dev, pAd, setup);				    \
}
//void *malloc (size_t __size);
//void free (void *__ptr);
int copy_from_user(char *to, char *from, int len);
void tbtt_mcast_process(unsigned long data);


// ======== FOR FW TESTING ===========
#define NULL_FRAME_ID 0xFFF

extern BOOLEAN halHifDownloadTxPacket(PUCHAR prPacket);
extern void halHifDownloadTxReturnBuffer(UINT16 u2PseFid);
extern void exp_printf(const char *fmt, ...);
#ifndef WIFI_BUILD_RAM
extern UINT_8 wifi_get_hif_mode(void);
#endif

extern void (*enqueue_rx_pkt_lwip_boson)(void *p, unsigned char op_mode);
extern unsigned char g_inband_87_enable;
extern unsigned char g_beacon_86_enable;    
extern UINT_32 g_WIFI_CONFG_BASE;   
extern UINT_32 g_WIFI_MAC_BASE;     
extern UINT_32 g_WIFI_PHY_BASE;     
extern UINT_32 g_WIFI_LP_BASE;      
extern UINT_32 g_WIFI_LPAON_BASE;   
extern UINT_32 g_WIFI_INT_BASE;     
extern UINT_32 g_WIFI_WTBL_BASE;    
extern UINT_32 g_WIFI_MIB_BASE;     
extern UINT_32 g_WIFI_AON_BASE;     
extern UINT_32 g_TOP_SADC_BASE;     
extern UINT_32 g_TOP_THERM_BASE;    
extern UINT_32 g_WIFI_PDA_BASE;     
extern UINT_32 g_MCU_CONFIG_BASE;   
extern UINT_32 g_TOP_RGU_BASE;      
extern UINT_32 g_TOP_CFG_AON_BASE;  
extern UINT_32 g_WIFI_CMDBT_ON_BASE;
extern UINT_32 g_WIFI_CMDBT_BASE;   
extern UINT_32 g_TOP_GPIO_BASE;     
extern UINT_32 g_WF_WTBL_ON_BASE;
extern UINT_32 g_WF_MAC_BASE;
extern UINT_32 g_HIFCR_BASE;
extern UINT_32 g_PSE_BASE_ADDR;
#endif /*__RT_BORA_H__ */











