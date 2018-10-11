/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    rtmp.h

    Abstract:
    Miniport generic portion header file

    Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Paul Lin    2002-08-01    created
    James Tan   2002-09-06    modified (Revise NTCRegTable)
    John Chang  2004-09-06    modified for RT2600     
*/
#ifndef __RTMP_H__
#define __RTMP_H__
#ifndef MT_MAC
#define MT_MAC
#endif
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#include "link_list_iot.h"
#include "link_rom.h"
#ifdef CONFIG_STA_SUPPORT
#include <aironet.h>
#endif // CONFIG_STA_SUPPORT //
#ifdef CONFIG_AP_SUPPORT
#include "ap_autoChSel.h"
#endif // CONFIG_AP_SUPPORT //

#ifdef WSC_INCLUDED
#include "wsc_rom.h"
#endif // WSC_INCLUDED //


//
//  NDIS Version definitions
//
#include "cmm_chip.h"
#include "mt_mac.h"
//#include "cmm_wpa.h"
#include "dot11i_wpa.h"

//#include "ra_ctrl.h"

#include "mat.h"

#ifdef IOT_CFG80211_SUPPORT
#include "cfg80211_cmm.h"
#include "cfg80211.h"
#endif /* IOT_CFG80211_SUPPORT */

#ifdef  NDIS51_MINIPORT
#define RTMP_NDIS_MAJOR_VERSION     5
#define RTMP_NDIS_MINOR_VERSION     1
#endif

#ifdef ASSERT
#undef ASSERT
#endif
extern void (*iot_ram_assert)(const char*,int);

#if 0
#define ASSERT(_exp) \
    { \
        if (!(_exp)) { \
			IOT_DBGPRINT(RT_DEBUG_ERROR,("ASSERT %s %s",__FUNCTION__,__LINE__));\
            /* halDumpAssertPoint((const PUINT_8)__FILE__, (UINT_16)__LINE__); */ \
            assert(0); \
        } \
    }
#else

#define ASSERT(_exp)\
	if(!(_exp))\
		 	iot_ram_assert(__FUNCTION__,__LINE__)\

#endif
#define LITE_ASSERT ASSERT

#define STA_DRIVER_VERSION			"3.0.0.0"
#define AP_DRIVER_VERSION			"3.0.0.0"

extern  char    NIC_VENDOR_DESC[];
extern  int     NIC_VENDOR_DESC_LEN;

extern  unsigned char   SNAP_AIRONET[];
extern  __romdata unsigned char   CipherSuiteCiscoCCKM[];
extern  __romdata unsigned char   CipherSuiteCiscoCCKMLen;
extern	__romdata unsigned char	CipherSuiteCiscoCCKM24[];
extern	__romdata unsigned char	CipherSuiteCiscoCCKM24Len;
extern  __romdata unsigned char   CipherSuiteCCXTkip[];
extern  __romdata unsigned char   CipherSuiteCCXTkipLen;
extern  unsigned char   CISCO_OUI[];
extern  UCHAR	BaSizeArray[4];

extern UCHAR BROADCAST_ADDR[MAC_ADDR_LEN];
extern UCHAR MULTICAST_ADDR[MAC_ADDR_LEN];
extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];
extern UCHAR array_BIT8[8];
extern char* CipherName[];
extern char* MCSToMbps[];
extern UCHAR	 RxwiMCSToOfdmRate[12];
extern __romdata UCHAR SNAP_802_1H[];
extern __romdata UCHAR SNAP_BRIDGE_TUNNEL[];
extern UCHAR SNAP_AIRONET[8];
extern UCHAR CKIP_LLC_SNAP[8];
extern UCHAR EAPOL_LLC_SNAP[8];
extern __romdata UCHAR EAPOL[2];
extern __romdata UCHAR IPX[2];
extern __romdata UCHAR APPLE_TALK[2];
extern UCHAR RateIdToPlcpSignal[12]; // see IEEE802.11a-1999 p.14
extern __romdata UCHAR	 OfdmRateToRxwiMCS[];
extern UCHAR OfdmSignalToRateId[16] ;
extern UCHAR default_cwmin[4];
extern UCHAR default_cwmax[4];
extern UCHAR default_sta_aifsn[4];
extern __romdata UCHAR MapUserPriorityToAccessCategory[8];

extern USHORT RateUpPER[];
extern USHORT RateDownPER[];
extern UCHAR  Phy11BNextRateDownward[];
extern UCHAR  Phy11BNextRateUpward[];
extern UCHAR  Phy11BGNextRateDownward[];
extern UCHAR  Phy11BGNextRateUpward[];
extern UCHAR  Phy11ANextRateDownward[];
extern UCHAR  Phy11ANextRateUpward[];
extern CHAR   RssiSafeLevelForTxRate[];
extern UCHAR  RateIdToMbps[];
extern USHORT RateIdTo500Kbps[];

extern UCHAR  CipherSuiteWpaNoneTkip[];
extern UCHAR  CipherSuiteWpaNoneTkipLen;

extern UCHAR  CipherSuiteWpaNoneAes[];
extern UCHAR  CipherSuiteWpaNoneAesLen;

extern UCHAR  SsidIe;
extern UCHAR  SupRateIe;
extern UCHAR  ExtRateIe;
extern UCHAR  HtCapIe;
extern UCHAR  AddHtInfoIe;
extern UCHAR  NewExtChanIe;
extern UCHAR  ErpIe;
extern UCHAR  DsIe;
extern UCHAR  TimIe;
extern UCHAR  WpaIe;
extern UCHAR  Wpa2Ie;
extern UCHAR  IbssIe;
extern UCHAR  Ccx2Ie;

#if 1 //def DOT11_N_SUPPORT
extern UCHAR  HtCapIe;
extern UCHAR  AddHtInfoIe;
extern UCHAR  NewExtChanIe;
extern UCHAR  BssCoexistIe;
extern UCHAR  ExtHtCapIe;
#endif // DOT11_N_SUPPORT //

extern __romdata UCHAR  WPA_OUI[];
extern __romdata UCHAR  RSN_OUI[];
extern __romdata UCHAR  WME_INFO_ELEM[];
extern __romdata UCHAR  WME_PARM_ELEM[];
extern UCHAR  Ccx2QosInfo[];
extern UCHAR  Ccx2IeInfo[];
extern __romdata UCHAR  RALINK_OUI[];
extern UCHAR  PowerConstraintIE[];


extern UCHAR  RateSwitchTable[];
extern UCHAR  RateSwitchTable11B[];
extern UCHAR  RateSwitchTable11G[];
extern UCHAR  RateSwitchTable11BG[];
extern UCHAR  RateSwitchTable11BGN1S[];
extern UCHAR  RateSwitchTable11BGN2S[];
extern UCHAR  RateSwitchTable11BGN2SForABand[];
extern UCHAR  RateSwitchTable11N1S[];
extern UCHAR  RateSwitchTable11N2S[];
extern UCHAR  RateSwitchTable11N2SForABand[];

extern UCHAR g_MaxApCliNum;
extern UCHAR g_MaxSTANum;
extern UCHAR g_MaxReorderMpduNum;
extern UCHAR g_MaxRepeaterNum;

extern USHORT g_SqEnqNormalMax;
extern USHORT g_MaxPacketsInMcastPsQueue;
extern USHORT g_MaxPacketsInPsQueue;
extern USHORT g_MaxPacketsInQueue;
extern USHORT g_MaxTxProcess;
extern USHORT g_TxRingSize;

extern UCHAR WMM_UP2AC_MAP[8];
#ifdef IOT_PROCESS_PORTING
#ifdef THROUGHPUT_PROFILING
typedef struct global_txrx_timing_info_s{
	UINT_32 iFace;
	UINT_32 reserved;
	UINT_32 timestamp[2];
} global_txrx_timing_info_t;
#endif/*THROUGHPUT_PROFILING*/
#endif/*IOT_PROCESS_PORTING*/

#define MAX_MCS_SET 8//IoT is 1x1 chip//16		/* From MCS 0 ~ MCS 15 */
#define	MAXSEQ		(0xFFF)
#ifdef CONFIG_STA_SUPPORT
extern UCHAR  __romdata PRE_N_HT_OUI[];
#endif // CONFIG_STA_SUPPORT //

#ifdef RTMP_INTERNAL_TX_ALC

#define EEPROM_TEMPRTATURE_STEP_NUM_G_BAND 0x6E
#define EEPROM_TEMPRTATURE_STEP_NUM_A_BAND 0xD4

#define EEPROM_REFERENCE_STEP_G_BAND (EEPROM_TEMPRTATURE_STEP_NUM_G_BAND+0x8)
#define EEPROM_REFERENCE_STEP_A_BAND (EEPROM_TEMPRTATURE_STEP_NUM_A_BAND+0x8)


//Created By Michael..20101215
#define NEG_THRESHOLDS_CNT	21 //8	
#define POS_THRESHOLDS_CNT	25
#define ALC_MID_IDX (NEG_THRESHOLDS_CNT-1) 
#define ALC_LOOKUP_TAB_SIZE  (NEG_THRESHOLDS_CNT+POS_THRESHOLDS_CNT)



//
// The number of channels for per-channel Tx power offset
//
//#define NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET	14

typedef struct  _EXT_TX_ALC_LOOKUP_TABLE_STRUCT
{
	CHAR       AlcCodeDelta;
	SHORT    BBP_R49_ShiftThreshold;
} EXT_TX_ALC_LOOKUP_TABLE_STRUCT, *PEXT_TX_ALC_LOOKUP_TABLE_STRUCT;


typedef struct _TX_ALC_WRAPPER_STRUCT
{
	EXT_TX_ALC_LOOKUP_TABLE_STRUCT	TemperatureCompensationLookUpTable[ALC_LOOKUP_TAB_SIZE];
	CHAR	AlcDeltaIdx;
//	CHAR	TemperatureRefStep;
//	CHAR	RefTemperature;
} TX_ALC_WRAPPER_STRUCT, *PTX_ALC_WRAPPER_STRUCT;

typedef struct  _INT_TX_ALC_TUNING_TABLE_STRUCT
{
        CHAR       EepromPower;
        UCHAR    RF_New_Value;
        CHAR       MAC_PowerDelta;
}   INT_TX_ALC_TUNING_TABLE_STRUCT, *PINT_TX_ALC_TUNING_TABLE_STRUCT;

//
// The Tx power control using the internal ALC
//
typedef struct _TX_POWER_CONTROL
{
	TX_ALC_WRAPPER_STRUCT	GbandTxALC;
	TX_ALC_WRAPPER_STRUCT	AbandTxALC;

#if 0	
	BOOLEAN bInternalTxALC; // Internal Tx ALC
	BOOLEAN bExtendedTssiMode; // The extended TSSI mode (each channel has different Tx power if needed)
	CHAR PerChTxPwrOffset[NUM_OF_CH_FOR_PER_CH_TX_PWR_OFFSET + 1]; // Per-channel Tx power offset	
	CHAR idxTxPowerTable; // The index of the Tx power table
	CHAR	RF_R12_Value; // RF R12[4:0]: Tx0 ALC
	CHAR	MAC_PowerDelta; // Tx power control over MAC 0x1314~0x1324
#endif	
} TX_POWER_CONTROL, *PTX_POWER_CONTROL;

#endif // RTMP_INTERNAL_TX_ALC //

typedef struct _UAPSD_INFO {
	BOOLEAN bAPSDCapable;
} UAPSD_INFO;

typedef struct  _QUEUE_ENTRY    {
	struct _QUEUE_ENTRY     *Next;
}   QUEUE_ENTRY, *PQUEUE_ENTRY;

// Queue structure
typedef struct  _QUEUE_HEADER   {
	PQUEUE_ENTRY    Head;
	PQUEUE_ENTRY    Tail;
	ULONG           Number;
}   QUEUE_HEADER, *PQUEUE_HEADER;




#ifdef CONFIG_STA_SUPPORT
extern UCHAR  __romdata PRE_N_HT_OUI[];
#endif // CONFIG_STA_SUPPORT //

#define	MAXSEQ		(0xFFF)


struct reordering_mpdu
{
	struct reordering_mpdu	*next;
	PNDIS_PACKET			pPacket;		/* coverted to 802.3 frame */
	int						Sequence;		/* sequence number of MPDU */
	BOOLEAN					bAMSDU;
};

struct reordering_list
{
	struct reordering_mpdu *next;
	int 	qlen;
};

struct reordering_mpdu_pool
{
	PVOID					mem;
	NDIS_SPIN_LOCK			lock;
	struct reordering_list 	freelist;
};

typedef struct 	_RSSI_SAMPLE {
	CHAR			LastRssi0;             // last received RSSI
	CHAR			LastRssi1;             // last received RSSI
	CHAR			LastRssi2;             // last received RSSI
	CHAR			AvgRssi0;
	CHAR			AvgRssi1;
	CHAR			AvgRssi2;
	SHORT			AvgRssi0X8;
	SHORT			AvgRssi1X8;
	SHORT			AvgRssi2X8;
} RSSI_SAMPLE;

//
//  Queue structure and macros
//

#define InitializeQueueHeader(QueueHeader)              \
{                                                       \
	(QueueHeader)->Head = (QueueHeader)->Tail = NULL;   \
	(QueueHeader)->Number = 0;                          \
}

#define RemoveHeadQueue(QueueHeader)                \
(QueueHeader)->Head;                                \
{                                                   \
	PQUEUE_ENTRY pNext;                             \
	if ((QueueHeader)->Head != NULL)				\
	{												\
		pNext = (QueueHeader)->Head->Next;          \
		(QueueHeader)->Head = pNext;                \
		if (pNext == NULL)                          \
			(QueueHeader)->Tail = NULL;             \
		(QueueHeader)->Number--;                    \
	}												\
}

#define InsertHeadQueue(QueueHeader, QueueEntry)            \
{                                                           \
		((PQUEUE_ENTRY)QueueEntry)->Next = (QueueHeader)->Head; \
		(QueueHeader)->Head = (PQUEUE_ENTRY)(QueueEntry);       \
		if ((QueueHeader)->Tail == NULL)                        \
			(QueueHeader)->Tail = (PQUEUE_ENTRY)(QueueEntry);   \
		(QueueHeader)->Number++;                                \
}

#define InsertTailQueue(QueueHeader, QueueEntry)                \
{                                                               \
	((PQUEUE_ENTRY)QueueEntry)->Next = NULL;                    \
	if ((QueueHeader)->Tail)                                    \
		(QueueHeader)->Tail->Next = (PQUEUE_ENTRY)(QueueEntry); \
	else                                                        \
		(QueueHeader)->Head = (PQUEUE_ENTRY)(QueueEntry);       \
	(QueueHeader)->Tail = (PQUEUE_ENTRY)(QueueEntry);           \
	(QueueHeader)->Number++;                                    \
}

#define InsertTailQueueAc(pAd, pEntry, QueueHeader, QueueEntry)			\
{																		\
	((PQUEUE_ENTRY)QueueEntry)->Next = NULL;							\
	if ((QueueHeader)->Tail)											\
		(QueueHeader)->Tail->Next = (PQUEUE_ENTRY)(QueueEntry);			\
	else																\
		(QueueHeader)->Head = (PQUEUE_ENTRY)(QueueEntry);				\
	(QueueHeader)->Tail = (PQUEUE_ENTRY)(QueueEntry);					\
	(QueueHeader)->Number++;											\
}


//
//  MACRO for debugging information
//
#ifdef IOT_DEBUG_PRINT
extern int (*iot_rom_printf)(const char *fmt, ...);
extern ULONG    RTDebugLevel;
#define IOT_DBGPRINT_RAW(Level, Fmt)    \
{                                   \
    if (Level <= RT_DEBUG_ERROR && RTDebugLevel >= Level)      \
    {                               \
    	if (iot_rom_printf != NULL)	\
        {	iot_rom_printf Fmt; }   \
    }                               \
}

#define IOT_DBGPRINT(Level, Fmt)    IOT_DBGPRINT_RAW(Level, Fmt)

#define IOT_DBGPRINT_ERR(Fmt)           \
{                                   \
    IOT_DBGPRINT_RAW(RT_DEBUG_ERROR, Fmt);                  \
}

#define IOT_DBGPRINT_S(Status, Fmt)                                         \
{                                                                       \
    ULONG   dbglevel;                                                   \
    if (Status == NDIS_STATUS_SUCCESS || Status == NDIS_STATUS_PENDING) \
        dbglevel = RT_DEBUG_TRACE;                                      \
    else                                                                \
        dbglevel = RT_DEBUG_ERROR;                                      \
    IOT_DBGPRINT(dbglevel, Fmt);                                            \
}
#define MTWF_LOG(Category, SubCategory, Level, Fmt)	\
do{	\
	IOT_DBGPRINT_RAW(Level, Fmt)\
}while(0)
#else
#define IOT_DBGPRINT(Level, Fmt)
#define IOT_DBGPRINT_RAW(Level, Fmt)
#define IOT_DBGPRINT_S(Status, Fmt)
#define IOT_DBGPRINT_ERR(Fmt)
#define MTWF_LOG(Category, SubCategory, Level, Fmt)	\
do{	\
	IOT_DBGPRINT_RAW(Level, Fmt)\
}while(0)
#endif

//#define STACFG_SET_FLAG(_pAd, _F)       ((_pAd)->StaCfg.StaCfgFlags |= (_F))
//#define STACFG_CLEAR_FLAG(_pAd, _F)     ((_pAd)->StaCfg.StaCfgFlags &= ~(_F))
//#define STACFG_TEST_FLAG(_pAd, _F)      (((_pAd)->StaCfg.StaCfgFlags & (_F)) != 0)

#define RX_FILTER_SET_FLAG(_pAd, _F)    ((_pAd)->CommonCfg.PacketFilter |= (_F))
#define RX_FILTER_CLEAR_FLAG(_pAd, _F)  ((_pAd)->CommonCfg.PacketFilter &= ~(_F))
#define RX_FILTER_TEST_FLAG(_pAd, _F)   (((_pAd)->CommonCfg.PacketFilter & (_F)) != 0)

#ifdef CONFIG_STA_SUPPORT
#define STA_NO_SECURITY_ON(_p)          (_p->StaCfg.wdev.WepStatus == Ndis802_11EncryptionDisabled)
#define STA_WEP_ON(_p)                  (_p->StaCfg.wdev.WepStatus == Ndis802_11Encryption1Enabled)
#define STA_TKIP_ON(_p)                 (_p->StaCfg.wdev.WepStatus == Ndis802_11Encryption2Enabled)
#define STA_AES_ON(_p)                  (_p->StaCfg.wdev.WepStatus == Ndis802_11Encryption3Enabled)

#define STA_TGN_WIFI_ON(_p)             (_p->StaCfg.bTGnWifiTest == TRUE)
#endif // CONFIG_STA_SUPPORT //

#define CKIP_KP_ON(_p)				((((_p)->StaCfg.CkipFlag) & 0x10) && ((_p)->StaCfg.bCkipCmicOn == TRUE))
#define CKIP_CMIC_ON(_p)			((((_p)->StaCfg.CkipFlag) & 0x08) && ((_p)->StaCfg.bCkipCmicOn == TRUE))



/* when adding any new type, please also add codes in LINUX_WEVENT_TRANSLATE */
#define RT_WLAN_EVENT_CUSTOM							0x01
#define RT_WLAN_EVENT_CGIWAP							0x02
#define RT_WLAN_EVENT_ASSOC_REQ_IE						0x03
#define RT_WLAN_EVENT_SCAN								0x04
#define RT_WLAN_EVENT_EXPIRED							0x05
#define RT_WLAN_EVENT_SHOWPIN							0x06
#define RT_WLAN_EVENT_PIN							0x07


#define INC_RING_INDEX(_idx, _RingSize)    \
{                                          \
    (_idx) = (_idx+1) % (_RingSize);       \
}


#define RING_PACKET_INIT(_TxRing, _idx)    \
{                                          \
    _TxRing->Cell[_idx].pNdisPacket = NULL;                              \
    _TxRing->Cell[_idx].pNextNdisPacket = NULL;                              \
}

#define TXDT_INIT(_TxD)    \
{                                          \
	NdisZeroMemory(_TxD, TXD_SIZE);	\
	_TxD->DMADONE = 1;                              \
}

//Set last data segment
#define RING_SET_LASTDS(_TxD, _IsSD0)    \
{                                          \
    if (_IsSD0) {_TxD->LastSec0 = 1;}     \
    else {_TxD->LastSec1 = 1;}     \
}



#define TR_ENQ_COUNT_INC(tr) \
{								\
	tr->enqCount++;				\
}

#define TR_ENQ_COUNT_DEC(tr) \
{								\
	tr->enqCount--;				\
}

#define TR_TOKEN_COUNT_INC(tr, qid) \
{								\
	tr->TokenCount[qid]++;				\
}

#define TR_TOKEN_COUNT_DEC(tr, qid) \
{								\
	tr->TokenCount[qid]--;				\
}


#define SQ_ENQ_NORMAL_MAX(_pAd) 	 _pAd->SqEnqNormalMax //32
#define SQ_ENQ_RESERVE_PERAC(_pAd) 	(SQ_ENQ_NORMAL_MAX(_pAd)/2)


// Increase TxTsc value for next transmission
// TODO: 
#if 0
// When i==6, means TSC has done one full cycle, do re-keying stuff follow specs
// Should send a special event microsoft defined to request re-key
#define INC_TX_TSC(_tsc)                                \
{                                                       \
    int i=0;                                            \
    while (++_tsc[i] == 0x0)                            \
    {                                                   \
        i++;                                            \
        if (i == 6)                                     \
            break;                                      \
    }                                                   \
}
#else
#define INC_TX_TSC(_tsc, _cnt)                          \
{                                                       \
    INT i=0;                                            \
	while (++_tsc[i] == 0x0)                            \
    {                                                   \
        i++;                                            \
		if (i == (_cnt))                                \
			break;                                      \
	}                                                   \
}
#endif

// StaActive.SupportedHtPhy.MCSSet is copied from AP beacon.  Don't need to update here.
#define COPY_HTSETTINGS_FROM_MLME_AUX_TO_ACTIVE_CFG(_pAd)                                 \
{                                                                                       \
	_pAd->StaActive.SupportedHtPhy.ChannelWidth = _pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth;      \
	_pAd->StaActive.SupportedHtPhy.MimoPs = _pAd->MlmeAux.HtCapability.HtCapInfo.MimoPs;      \
	_pAd->StaActive.SupportedHtPhy.GF = _pAd->MlmeAux.HtCapability.HtCapInfo.GF;      \
	_pAd->StaActive.SupportedHtPhy.ShortGIfor20 = _pAd->MlmeAux.HtCapability.HtCapInfo.ShortGIfor20;      \
	_pAd->StaActive.SupportedHtPhy.ShortGIfor40 = _pAd->MlmeAux.HtCapability.HtCapInfo.ShortGIfor40;      \
	_pAd->StaActive.SupportedHtPhy.TxSTBC = _pAd->MlmeAux.HtCapability.HtCapInfo.TxSTBC;      \
	_pAd->StaActive.SupportedHtPhy.RxSTBC = _pAd->MlmeAux.HtCapability.HtCapInfo.RxSTBC;      \
	_pAd->StaActive.SupportedHtPhy.ExtChanOffset = _pAd->MlmeAux.AddHtInfo.AddHtInfo.ExtChanOffset;      \
	_pAd->StaActive.SupportedHtPhy.RecomWidth = _pAd->MlmeAux.AddHtInfo.AddHtInfo.RecomWidth;      \
	_pAd->StaActive.SupportedHtPhy.OperaionMode = _pAd->MlmeAux.AddHtInfo.AddHtInfo2.OperaionMode;      \
	_pAd->StaActive.SupportedHtPhy.NonGfPresent = _pAd->MlmeAux.AddHtInfo.AddHtInfo2.NonGfPresent;      \
}

#define COPY_AP_HTSETTINGS_FROM_BEACON(_pAd, _pHtCapability)                                 \
{                                                                                       \
	_pAd->MacTab.Content[BSSID_WCID].AMsduSize = (UCHAR)(_pHtCapability->HtCapInfo.AMsduSize);	\
	_pAd->MacTab.Content[BSSID_WCID].MmpsMode= (UCHAR)(_pHtCapability->HtCapInfo.MimoPs);	\
	_pAd->MacTab.Content[BSSID_WCID].MaxRAmpduFactor = (UCHAR)(_pHtCapability->HtCapParm.MaxRAmpduFactor);	\
}


//
// MACRO for 32-bit PCI register read / write
//
// Usage : RTMP_IO_READ32(
//              PRTMP_ADAPTER pAd,
//              ULONG Register_Offset,
//              PULONG  pValue)
//
//         RTMP_IO_WRITE32(
//              PRTMP_ADAPTER pAd,
//              ULONG Register_Offset,
//              ULONG Value)
//


//
// BBP & RF are using indirect access. Before write any value into it.
// We have to make sure there is no outstanding command pending via checking busy bit.
//
#define MAX_BUSY_COUNT  100         // Number of retry before failing access BBP & RF indirect register
//
#if 0 //def RT2860
#define RTMP_RF_IO_WRITE32(_A, _V)                  \
{                                                   \
    PHY_CSR4_STRUC  Value;                          \
    ULONG           BusyCnt = 0;                    \
    do {                                            \
        RTMP_IO_READ32(_A, RF_CSR_CFG0, &Value.word);  \
        if (Value.field.Busy == IDLE)               \
            break;                                  \
        BusyCnt++;                                  \
    }   while (BusyCnt < MAX_BUSY_COUNT);           \
    if (BusyCnt < MAX_BUSY_COUNT)                   \
    {                                               \
        RTMP_IO_WRITE32(_A, RF_CSR_CFG0, _V);          \
    }                                               \
}

#define BBP_IO_READ8_BY_REG_ID(_A, _I, _pV)        \
{                                                       \
    BBP_CSR_CFG_STRUC  BbpCsr;                             \
    int             __i, __k;                               \
    for (__i=0; __i<MAX_BUSY_COUNT; __i++)                    \
    {                                                   \
        RTMP_IO_READ32(_A, BBP_CSR_CFG, &BbpCsr.word);     \
        if (BbpCsr.field.Busy == BUSY)                  \
        {                                               \
            continue;                                   \
        }                                               \
        BbpCsr.word = 0;                                \
        BbpCsr.field.fRead = 1;                         \
        BbpCsr.field.BBP_RW_MODE = 1;                         \
        BbpCsr.field.Busy = 1;                          \
        BbpCsr.field.RegNum = _I;                       \
        RTMP_IO_WRITE32(_A, BBP_CSR_CFG, BbpCsr.word);     \
        for (__k=0; __k<MAX_BUSY_COUNT; __k++)                \
        {                                               \
            RTMP_IO_READ32(_A, BBP_CSR_CFG, &BbpCsr.word); \
            if (BbpCsr.field.Busy == IDLE)              \
                break;                                  \
        }                                               \
        if ((BbpCsr.field.Busy == IDLE) &&              \
            (BbpCsr.field.RegNum == _I))                \
        {                                               \
            *(_pV) = (UCHAR)BbpCsr.field.Value;         \
            break;                                      \
        }                                               \
    }                                                   \
    if (BbpCsr.field.Busy == BUSY)                      \
    {                                                   \
        DBGPRINT_ERR(("DFS BBP read R%d fail\n", _I));      \
        *(_pV) = (_A)->BbpWriteLatch[_I];               \
    }                                                   \
}

#if 1 // RT2880 do not have MCU

#define RTMP_BBP_IO_READ8_BY_REG_ID 	BBP_IO_READ8_BY_REG_ID

#else // RT2880 do not have MCU
//#define RTMP_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV)    {}
// Read BBP register by register's ID. Generate PER to test BA
#define RTMP_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV)        \
{                                                       \
    BBP_CSR_CFG_STRUC  BbpCsr;                             \
    int             i, k;                               \
    if ((_A)->bPCIclkOff == FALSE)                     \
    {                                                   \
    for (i=0; i<MAX_BUSY_COUNT; i++)                    \
    {                                                   \
		RTMP_IO_READ32(_A, H2M_BBP_AGENT, &BbpCsr.word);				\
        if (BbpCsr.field.Busy == BUSY)                  \
        {                                               \
            continue;                                   \
        }                                               \
        BbpCsr.word = 0;                                \
        BbpCsr.field.fRead = 1;                         \
        BbpCsr.field.BBP_RW_MODE = 1;                         \
        BbpCsr.field.Busy = 1;                          \
        BbpCsr.field.RegNum = _I;                       \
		RTMP_IO_WRITE32(_A, H2M_BBP_AGENT, BbpCsr.word);				\
		AsicSendCommandToMcu(_A, 0x80, 0xff, 0x0, 0x0);					\
		RTMPusecDelay(1000);							\
        for (k=0; k<MAX_BUSY_COUNT; k++)                \
        {                                               \
			RTMP_IO_READ32(_A, H2M_BBP_AGENT, &BbpCsr.word);			\
            if (BbpCsr.field.Busy == IDLE)              \
                break;                                  \
        }                                               \
        if ((BbpCsr.field.Busy == IDLE) &&              \
            (BbpCsr.field.RegNum == _I))                \
        {                                               \
            *(_pV) = (UCHAR)BbpCsr.field.Value;         \
            break;                                      \
        }                                               \
    }                                                   \
    if (BbpCsr.field.Busy == BUSY)                      \
    {                                                   \
		DBGPRINT_ERR(("BBP read R%d=0x%x fail\n", _I, BbpCsr.word));	\
        *(_pV) = (_A)->BbpWriteLatch[_I];               \
		RTMP_IO_READ32(_A, H2M_BBP_AGENT, &BbpCsr.word);				\
		BbpCsr.field.Busy = 0;                          \
		RTMP_IO_WRITE32(_A, H2M_BBP_AGENT, BbpCsr.word);				\
    }                                                   \
    }                   \
}
#endif // RT2880 do not have MCU

#define BBP_IO_WRITE8_BY_REG_ID(_A, _I, _V)        \
{                                                       \
    BBP_CSR_CFG_STRUC  BbpCsr;                             \
    int             BusyCnt;                            \
    for (BusyCnt=0; BusyCnt<MAX_BUSY_COUNT; BusyCnt++)  \
    {                                                   \
        RTMP_IO_READ32(_A, BBP_CSR_CFG, &BbpCsr.word);     \
        if (BbpCsr.field.Busy == BUSY)                  \
            continue;                                   \
        BbpCsr.word = 0;                                \
        BbpCsr.field.fRead = 0;                         \
        BbpCsr.field.BBP_RW_MODE = 1;                         \
        BbpCsr.field.Busy = 1;                          \
        BbpCsr.field.Value = _V;                        \
        BbpCsr.field.RegNum = _I;                       \
        RTMP_IO_WRITE32(_A, BBP_CSR_CFG, BbpCsr.word);     \
        (_A)->BbpWriteLatch[_I] = _V;                   \
        break;                                          \
    }                                                   \
    if (BusyCnt == MAX_BUSY_COUNT)                      \
    {                                                   \
        DBGPRINT_ERR(("BBP write R%d fail\n", _I));     \
    }                                                   \
}

#if 1 // RT2880 do not have MCU

#define RTMP_BBP_IO_WRITE8_BY_REG_ID 	BBP_IO_WRITE8_BY_REG_ID

#else // RT2880 do not have MCU
// Write BBP register by register's ID & value
#define RTMP_BBP_IO_WRITE8_BY_REG_ID(_A, _I, _V)        \
{                                                       \
    BBP_CSR_CFG_STRUC  BbpCsr;                             \
    int             BusyCnt;                            \
    if ((_A)->bPCIclkOff == FALSE)                     \
    {                                                   \
    for (BusyCnt=0; BusyCnt<MAX_BUSY_COUNT; BusyCnt++)  \
    {                                                   \
		RTMP_IO_READ32(_A, H2M_BBP_AGENT, &BbpCsr.word);				\
        if (BbpCsr.field.Busy == BUSY)                  \
            continue;                                   \
        BbpCsr.word = 0;                                \
        BbpCsr.field.fRead = 0;                         \
        BbpCsr.field.BBP_RW_MODE = 1;                         \
        BbpCsr.field.Busy = 1;                          \
        BbpCsr.field.Value = _V;                        \
        BbpCsr.field.RegNum = _I;                       \
		RTMP_IO_WRITE32(_A, H2M_BBP_AGENT, BbpCsr.word);				\
		AsicSendCommandToMcu(_A, 0x80, 0xff, 0x0, 0x0);					\
            if (_A->OpMode == OPMODE_AP)                    \
		RTMPusecDelay(1000);							\
        (_A)->BbpWriteLatch[_I] = _V;                   \
        break;                                          \
    }                                                   \
    if (BusyCnt == MAX_BUSY_COUNT)                      \
    {                                                   \
		DBGPRINT_ERR(("BBP write R%d=0x%x fail\n", _I, BbpCsr.word));	\
		RTMP_IO_READ32(_A, H2M_BBP_AGENT, &BbpCsr.word);				\
		BbpCsr.field.Busy = 0;                          \
		RTMP_IO_WRITE32(_A, H2M_BBP_AGENT, BbpCsr.word);				\
    }                                                   \
    }                                                   \
}
#endif // RT2880 do not have MCU



#define RF_IO_READ8_BY_REG_ID(_A, _I, _pV)        \
{                                                       \
    RF_CSR_STRUC  RfCsr;                             \
    int             i, k;                               \
    for (i=0; i<MAX_BUSY_COUNT; i++)                    \
    {                                                   \
        RTMP_IO_READ32(_A, RF_CSR_CFG, &RfCsr.word);     \
        if (RfCsr.field.Busy == BUSY)                  \
        {                                               \
            continue;                                   \
        }                                               \
        RfCsr.word = 0;                                \
        RfCsr.field.Busy = 1;                          \
        RfCsr.field.fWrite = 0;                         \
        RfCsr.field.RegNum = _I;                       \
        RTMP_IO_WRITE32(_A, RF_CSR_CFG, RfCsr.word);     \
        for (k=0; k<MAX_BUSY_COUNT; k++)                \
        {                                               \
            RTMP_IO_READ32(_A, RF_CSR_CFG, &RfCsr.word); \
            if (RfCsr.field.Busy == IDLE)              \
                break;                                  \
        }                                               \
        if ((RfCsr.field.Busy == IDLE) &&              \
            (RfCsr.field.RegNum == _I))                \
        {                                               \
            *(_pV) = (UCHAR)RfCsr.field.Value;         \
            break;                                      \
        }                                               \
    }                                                   \
    if (RfCsr.field.Busy == BUSY)                      \
    {                                                   \
        DBGPRINT_ERR(("RF read R%d fail\n", RfCsr.word)); \
    }                                                   \
}

#define RF_IO_WRITE8_BY_REG_ID(_A, _I, _V)        \
{                                                       \
    RF_CSR_STRUC  RfCsr;                             \
    int             BusyCnt;                            \
    for (BusyCnt=0; BusyCnt<MAX_BUSY_COUNT; BusyCnt++)  \
    {                                                   \
        RTMP_IO_READ32(_A, RF_CSR_CFG, &RfCsr.word);     \
        if (RfCsr.field.Busy == BUSY)                  \
            continue;                                   \
        RfCsr.word = 0;                                \
        RfCsr.field.fWrite = 1;                         \
        RfCsr.field.Busy = 1;                          \
        RfCsr.field.Value = _V;                        \
        RfCsr.field.RegNum = _I;                       \
        RTMP_IO_WRITE32(_A, RF_CSR_CFG, RfCsr.word);     \
        break;                                          \
    }                                                   \
    if (BusyCnt == MAX_BUSY_COUNT)                      \
    {                                                   \
        DBGPRINT_ERR(("RF write R%d fail\n", RfCsr.word));\
    }                                                   \
}
#endif // RT2860 //

#define RTMP_BBP_IO_WRITE8_BY_REG_ID(_A, _I, _V)
#define RTMP_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV)

#define     MAP_CHANNEL_ID_TO_KHZ(ch, khz)  {               \
                switch (ch)                                 \
                {                                           \
                    case 1:     khz = 2412000;   break;     \
                    case 2:     khz = 2417000;   break;     \
                    case 3:     khz = 2422000;   break;     \
                    case 4:     khz = 2427000;   break;     \
                    case 5:     khz = 2432000;   break;     \
                    case 6:     khz = 2437000;   break;     \
                    case 7:     khz = 2442000;   break;     \
                    case 8:     khz = 2447000;   break;     \
                    case 9:     khz = 2452000;   break;     \
                    case 10:    khz = 2457000;   break;     \
                    case 11:    khz = 2462000;   break;     \
                    case 12:    khz = 2467000;   break;     \
                    case 13:    khz = 2472000;   break;     \
                    case 14:    khz = 2484000;   break;     \
                    case 36:  /* UNII */  khz = 5180000;   break;     \
                    case 40:  /* UNII */  khz = 5200000;   break;     \
                    case 44:  /* UNII */  khz = 5220000;   break;     \
                    case 48:  /* UNII */  khz = 5240000;   break;     \
                    case 52:  /* UNII */  khz = 5260000;   break;     \
                    case 56:  /* UNII */  khz = 5280000;   break;     \
                    case 60:  /* UNII */  khz = 5300000;   break;     \
                    case 64:  /* UNII */  khz = 5320000;   break;     \
                    case 149: /* UNII */  khz = 5745000;   break;     \
                    case 153: /* UNII */  khz = 5765000;   break;     \
                    case 157: /* UNII */  khz = 5785000;   break;     \
                    case 161: /* UNII */  khz = 5805000;   break;     \
                    case 165: /* UNII */  khz = 5825000;   break;     \
                    case 100: /* HiperLAN2 */  khz = 5500000;   break;     \
                    case 104: /* HiperLAN2 */  khz = 5520000;   break;     \
                    case 108: /* HiperLAN2 */  khz = 5540000;   break;     \
                    case 112: /* HiperLAN2 */  khz = 5560000;   break;     \
                    case 116: /* HiperLAN2 */  khz = 5580000;   break;     \
                    case 120: /* HiperLAN2 */  khz = 5600000;   break;     \
                    case 124: /* HiperLAN2 */  khz = 5620000;   break;     \
                    case 128: /* HiperLAN2 */  khz = 5640000;   break;     \
                    case 132: /* HiperLAN2 */  khz = 5660000;   break;     \
                    case 136: /* HiperLAN2 */  khz = 5680000;   break;     \
                    case 140: /* HiperLAN2 */  khz = 5700000;   break;     \
                    case 34:  /* Japan MMAC */   khz = 5170000;   break;   \
                    case 38:  /* Japan MMAC */   khz = 5190000;   break;   \
                    case 42:  /* Japan MMAC */   khz = 5210000;   break;   \
                    case 46:  /* Japan MMAC */   khz = 5230000;   break;   \
                    case 184: /* Japan */   khz = 4920000;   break;   \
                    case 188: /* Japan */   khz = 4940000;   break;   \
                    case 192: /* Japan */   khz = 4960000;   break;   \
                    case 196: /* Japan */   khz = 4980000;   break;   \
                    case 208: /* Japan, means J08 */   khz = 5040000;   break;   \
                    case 212: /* Japan, means J12 */   khz = 5060000;   break;   \
                    case 216: /* Japan, means J16 */   khz = 5080000;   break;   \
                    default:    khz = 2412000;   break;     \
                }                                           \
            }

#define     MAP_KHZ_TO_CHANNEL_ID(khz, ch)  {               \
                switch (khz)                                \
                {                                           \
                    case 2412000:    ch = 1;     break;     \
                    case 2417000:    ch = 2;     break;     \
                    case 2422000:    ch = 3;     break;     \
                    case 2427000:    ch = 4;     break;     \
                    case 2432000:    ch = 5;     break;     \
                    case 2437000:    ch = 6;     break;     \
                    case 2442000:    ch = 7;     break;     \
                    case 2447000:    ch = 8;     break;     \
                    case 2452000:    ch = 9;     break;     \
                    case 2457000:    ch = 10;    break;     \
                    case 2462000:    ch = 11;    break;     \
                    case 2467000:    ch = 12;    break;     \
                    case 2472000:    ch = 13;    break;     \
                    case 2484000:    ch = 14;    break;     \
                    case 5180000:    ch = 36;  /* UNII */  break;     \
                    case 5200000:    ch = 40;  /* UNII */  break;     \
                    case 5220000:    ch = 44;  /* UNII */  break;     \
                    case 5240000:    ch = 48;  /* UNII */  break;     \
                    case 5260000:    ch = 52;  /* UNII */  break;     \
                    case 5280000:    ch = 56;  /* UNII */  break;     \
                    case 5300000:    ch = 60;  /* UNII */  break;     \
                    case 5320000:    ch = 64;  /* UNII */  break;     \
                    case 5745000:    ch = 149; /* UNII */  break;     \
                    case 5765000:    ch = 153; /* UNII */  break;     \
                    case 5785000:    ch = 157; /* UNII */  break;     \
                    case 5805000:    ch = 161; /* UNII */  break;     \
                    case 5825000:    ch = 165; /* UNII */  break;     \
                    case 5500000:    ch = 100; /* HiperLAN2 */  break;     \
                    case 5520000:    ch = 104; /* HiperLAN2 */  break;     \
                    case 5540000:    ch = 108; /* HiperLAN2 */  break;     \
                    case 5560000:    ch = 112; /* HiperLAN2 */  break;     \
                    case 5580000:    ch = 116; /* HiperLAN2 */  break;     \
                    case 5600000:    ch = 120; /* HiperLAN2 */  break;     \
                    case 5620000:    ch = 124; /* HiperLAN2 */  break;     \
                    case 5640000:    ch = 128; /* HiperLAN2 */  break;     \
                    case 5660000:    ch = 132; /* HiperLAN2 */  break;     \
                    case 5680000:    ch = 136; /* HiperLAN2 */  break;     \
                    case 5700000:    ch = 140; /* HiperLAN2 */  break;     \
                    case 5170000:    ch = 34;  /* Japan MMAC */   break;   \
                    case 5190000:    ch = 38;  /* Japan MMAC */   break;   \
                    case 5210000:    ch = 42;  /* Japan MMAC */   break;   \
                    case 5230000:    ch = 46;  /* Japan MMAC */   break;   \
                    case 4920000:    ch = 184; /* Japan */  break;   \
                    case 4940000:    ch = 188; /* Japan */  break;   \
                    case 4960000:    ch = 192; /* Japan */  break;   \
                    case 4980000:    ch = 196; /* Japan */  break;   \
                    case 5040000:    ch = 208; /* Japan, means J08 */  break;   \
                    case 5060000:    ch = 212; /* Japan, means J12 */  break;   \
                    case 5080000:    ch = 216; /* Japan, means J16 */  break;   \
                    default:         ch = 1;     break;     \
                }                                           \
            }

//
// Common fragment list structure -  Identical to the scatter gather frag list structure
//
//#define RTMP_SCATTER_GATHER_ELEMENT         SCATTER_GATHER_ELEMENT 
//#define PRTMP_SCATTER_GATHER_ELEMENT        PSCATTER_GATHER_ELEMENT 
#define NIC_MAX_PHYS_BUF_COUNT              8

typedef struct _RTMP_SCATTER_GATHER_ELEMENT {
    PVOID		Address;
    ULONG		Length;
    PULONG		Reserved;
} RTMP_SCATTER_GATHER_ELEMENT, *PRTMP_SCATTER_GATHER_ELEMENT;


typedef struct _RTMP_SCATTER_GATHER_LIST {
    ULONG  NumberOfElements;
    PULONG Reserved;
    RTMP_SCATTER_GATHER_ELEMENT Elements[NIC_MAX_PHYS_BUF_COUNT];
} RTMP_SCATTER_GATHER_LIST, *PRTMP_SCATTER_GATHER_LIST;

//
//  Some utility macros
//
#ifndef min
#define min(_a, _b)     (((_a) < (_b)) ? (_a) : (_b))
#endif

#ifndef max
#define max(_a, _b)     (((_a) > (_b)) ? (_a) : (_b))
#endif

#ifdef CONFIG_5G_SUPPORT
#define GET_LNA_GAIN(_pAd)	((_pAd->LatchRfRegs.Channel <= 14) ? (_pAd->BLNAGain) : ((_pAd->LatchRfRegs.Channel <= 64) ? (_pAd->ALNAGain0) : ((_pAd->LatchRfRegs.Channel <= 128) ? (_pAd->ALNAGain1) : (_pAd->ALNAGain2))))
#else
#define GET_LNA_GAIN(_pAd)	(_pAd->BLNAGain)
#endif

//#define GET_LNA_GAIN(_pAd)	(_pAd->BLNAGain)

#define INC_COUNTER64(Val)          (Val.QuadPart++)
																				 
// value recorded inside pNdisPacket->MiniportReservedEx[x] via RTMP_SET_PACKET_SOURCE()/RTMP_GET_PACKET_SOURCE()
// this value is used to decide how to release this NDIS packet - either release to NDIS layer or release
// to driver's pre-allocated packet pool
//      0xff: NDIS PACKET is pass from Windows NDIS layer
//      0 ~ NUM_OF_PREALLOCATEDP_SHARED_MEMORY: NDIS PACKET is created internally

// if orginal Ethernet frame contains no LLC/SNAP, then an extra LLC/SNAP encap is required 
#define EXTRA_LLCSNAP_ENCAP_FROM_PKT_START(_pBufVA, _pExtraLlcSnapEncap)		\
{																\
	if (((*(_pBufVA + 12) << 8) + *(_pBufVA + 13)) > 1500)		\
	{															\
		_pExtraLlcSnapEncap = (PUCHAR)SNAP_802_1H;				\
		if (NdisEqualMemory(IPX, _pBufVA + 12, 2) || 			\
			NdisEqualMemory(APPLE_TALK, _pBufVA + 12, 2))		\
		{														\
			_pExtraLlcSnapEncap = (PUCHAR)SNAP_BRIDGE_TUNNEL;	\
		}														\
	}															\
	else														\
	{															\
		_pExtraLlcSnapEncap = NULL;								\
	}															\
}

// New Define for new Tx Path.
#define EXTRA_LLCSNAP_ENCAP_FROM_PKT_OFFSET(_pBufVA, _pExtraLlcSnapEncap)	\
{																\
	if (((*(_pBufVA) << 8) + *(_pBufVA + 1)) > 1500)			\
	{															\
		_pExtraLlcSnapEncap = (PUCHAR)SNAP_802_1H;				\
		if (NdisEqualMemory(IPX, _pBufVA, 2) || 				\
			NdisEqualMemory(APPLE_TALK, _pBufVA, 2))			\
		{														\
			_pExtraLlcSnapEncap = (PUCHAR)SNAP_BRIDGE_TUNNEL;	\
		}														\
	}															\
	else														\
	{															\
		_pExtraLlcSnapEncap = NULL;								\
	}															\
}


#define MAKE_802_3_HEADER(_p, _pMac1, _pMac2, _pType)                   \
{                                                                       \
    NdisMoveMemory(_p, _pMac1, MAC_ADDR_LEN);                           \
    NdisMoveMemory((_p + MAC_ADDR_LEN), _pMac2, MAC_ADDR_LEN);          \
    NdisMoveMemory((_p + MAC_ADDR_LEN * 2), _pType, LENGTH_802_3_TYPE); \
}

// if pData has no LLC/SNAP (neither RFC1042 nor Bridge tunnel), keep it that way.
// else if the received frame is LLC/SNAP-encaped IPX or APPLETALK, preserve the LLC/SNAP field 
// else remove the LLC/SNAP field from the result Ethernet frame
// Patch for WHQL only, which did not turn on Netbios but use IPX within its payload
// Note:
//     _pData & _DataSize may be altered (remove 8-byte LLC/SNAP) by this MACRO
//     _pRemovedLLCSNAP: pointer to removed LLC/SNAP; NULL is not removed
#define CONVERT_TO_802_3(_p8023hdr, _pDA, _pSA, _pData, _DataSize, _pRemovedLLCSNAP)      \
{                                                                       \
    char LLC_Len[2];                                                    \
                                                                        \
    _pRemovedLLCSNAP = NULL;                                            \
    if (NdisEqualMemory(SNAP_802_1H, _pData, 6)  ||                     \
        NdisEqualMemory(SNAP_BRIDGE_TUNNEL, _pData, 6))                 \
    {                                                                   \
        PUCHAR pProto = _pData + 6;                                     \
                                                                        \
        if ((NdisEqualMemory(IPX, pProto, 2) || NdisEqualMemory(APPLE_TALK, pProto, 2)) &&  \
            NdisEqualMemory(SNAP_802_1H, _pData, 6))                    \
        {                                                               \
            LLC_Len[0] = (UCHAR)(_DataSize / 256);                      \
            LLC_Len[1] = (UCHAR)(_DataSize % 256);                      \
            MAKE_802_3_HEADER(_p8023hdr, _pDA, _pSA, LLC_Len);          \
        }                                                               \
        else                                                            \
        {                                                               \
            MAKE_802_3_HEADER(_p8023hdr, _pDA, _pSA, pProto);           \
            _pRemovedLLCSNAP = _pData;                                  \
            _DataSize -= LENGTH_802_1_H;                                \
            _pData += LENGTH_802_1_H;                                   \
        }                                                               \
    }                                                                   \
    else                                                                \
    {                                                                   \
        LLC_Len[0] = (UCHAR)(_DataSize / 256);                          \
        LLC_Len[1] = (UCHAR)(_DataSize % 256);                          \
        MAKE_802_3_HEADER(_p8023hdr, _pDA, _pSA, LLC_Len);              \
    }                                                                   \
}

#define SWITCH_AB( _pAA, _pBB)    \
{                                                                           \
    PVOID pCC;                                                          \
    pCC = _pBB;                                                 \
    _pBB = _pAA;                                                 \
    _pAA = pCC;                                                 \
}

	

// Enqueue this frame to MLME engine
// We need to enqueue the whole frame because MLME need to pass data type
// information from 802.11 header

#if 0  // Michael : TSF Timer ??
#define REPORT_MGMT_FRAME_TO_MLME(_pAd, Wcid, _pFrame, _FrameSize, _Rssi0, _Rssi1, _Rssi2, _PlcpSignal)        \
{                                                                                       \
    UINT32 High32TSF, Low32TSF;                                                          \
    RTMP_IO_READ32(_pAd, TSF_TIMER_DW1, &High32TSF);                                       \
    RTMP_IO_READ32(_pAd, TSF_TIMER_DW0, &Low32TSF);                                        \
    MlmeEnqueueForRecv(_pAd, Wcid, High32TSF, Low32TSF, (UCHAR)_Rssi0, (UCHAR)_Rssi1,(UCHAR)_Rssi2,_FrameSize, _pFrame, (UCHAR)_PlcpSignal);   \
}
#else
#define REPORT_MGMT_FRAME_TO_MLME(_pAd, Wcid, _pFrame, _FrameSize, _Rssi0, _Rssi1, _Rssi2, _PlcpSignal)        \
    MlmeEnqueueForRecv(_pAd, Wcid, 0, 0, (UCHAR)_Rssi0, (UCHAR)_Rssi1,(UCHAR)_Rssi2,_FrameSize, _pFrame, (UCHAR)_PlcpSignal)
#endif // RT2860 //

// INFRA mode- Address 1 - AP, Address 2 - this STA, Address 3 - DA
// ADHOC mode- Address 1 - DA, Address 2 - this STA, Address 3 - BSSID
#if 0 //raghav: Not used
#ifdef CONFIG_AP_SUPPORT
#define MAKE_802_11_HEADER(_pAd, _80211hdr, _pDA)                         \
{                                                                               \
    NdisZeroMemory(&_80211hdr, sizeof(HEADER_802_11));                          \
    if (INFRA_ON(_pAd))                                                         \
    {                                                                           \
        COPY_MAC_ADDR(_80211hdr.Addr1, _pAd->ApCfg.MBSSID[apidx].Bssid);                  \
        COPY_MAC_ADDR(_80211hdr.Addr3, _pDA);                                   \
        _80211hdr.FC.ToDs = 1;                                                  \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        COPY_MAC_ADDR(_80211hdr.Addr1, _pDA);                                   \
        COPY_MAC_ADDR(_80211hdr.Addr3, _pAd->ApCfg.MBSSID[apidx].Bssid);                  \
    }                                                                           \
    COPY_MAC_ADDR(_80211hdr.Addr2, _pAd->CurrentAddress);                       \
    _80211hdr.FC.Type = BTYPE_DATA;                                             \
    if (_pAd->CommonCfg.bAPSDForcePowerSave)									\
    {																			\
    	_80211hdr.FC.PwrMgmt = PWR_SAVE;												\
    }																			\
    else																		\
    {																			\
    	_80211hdr.FC.PwrMgmt = (_pAd->StaCfg.Psm == PWR_SAVE);                  \
    }																			\
}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
#define MAKE_802_11_HEADER(_pAd, _80211hdr, _pDA)                         \
{                                                                               \
    NdisZeroMemory(&_80211hdr, sizeof(HEADER_802_11));                          \
    if (INFRA_ON(_pAd))                                                         \
    {                                                                           \
        COPY_MAC_ADDR(_80211hdr.Addr1, _pAd->CommonCfg.Bssid);                  \
        COPY_MAC_ADDR(_80211hdr.Addr3, _pDA);                                   \
        _80211hdr.FC.ToDs = 1;                                                  \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        COPY_MAC_ADDR(_80211hdr.Addr1, _pDA);                                   \
        COPY_MAC_ADDR(_80211hdr.Addr3, _pAd->CommonCfg.Bssid);                  \
    }                                                                           \
    COPY_MAC_ADDR(_80211hdr.Addr2, _pAd->CurrentAddress);                       \
    _80211hdr.FC.Type = BTYPE_DATA;                                             \
    if (_pAd->CommonCfg.bAPSDForcePowerSave)									\
    {																			\
    	_80211hdr.FC.PwrMgmt = PWR_SAVE;												\
    }																			\
    else																		\
    {																			\
    	_80211hdr.FC.PwrMgmt = (_pAd->StaCfg.Psm == PWR_SAVE);                  \
    }																			\
}
#endif // CONFIG_STA_SUPPORT //
#endif
#define NDIS_QUERY_BUFFER(_NdisBuf, _ppVA, _pBufLen)                    \
    NdisQueryBuffer(_NdisBuf, _ppVA, _pBufLen)
//#endif

#define MAC_ADDR_EQUAL(pAddr1,pAddr2)           RTMPEqualMemory((PVOID)(pAddr1), (PVOID)(pAddr2), MAC_ADDR_LEN)
#define SSID_EQUAL(ssid1, len1, ssid2, len2)    ((len1==len2) && (RTMPEqualMemory(ssid1, ssid2, len1)))

//
// Check if it is Japan W53(ch52,56,60,64) channel.
//
#define JapanChannelCheck(channel)  ((channel == 52) || (channel == 56) || (channel == 60) || (channel == 64))


//
// Register set pair for initialzation register set definition
//
typedef struct  _RTMP_REG_PAIR
{
	ULONG   Register;
	ULONG   Value;
} RTMP_REG_PAIR, *PRTMP_REG_PAIR;

typedef struct  _REG_PAIR
{
	UCHAR   Register;
	UCHAR   Value;
} REG_PAIR, *PREG_PAIR;

//
// Register set pair for initialzation register set definition
//
typedef struct  _RTMP_RF_REGS
{
	UCHAR   Channel;
	ULONG   R1;
	ULONG   R2;
	ULONG   R3;
	ULONG   R4;
} RTMP_RF_REGS, *PRTMP_RF_REGS;

typedef struct _FREQUENCY_ITEM {
	UCHAR	Channel;
	USHORT	N;
	UCHAR	R;
	ULONG	K;  // UCHAR K; To support SDM
} FREQUENCY_ITEM, *PFREQUENCY_ITEM;

//
//  Data buffer for DMA operation, the buffer must be contiguous physical memory
//  Both DMA to / from CPU use the same structure.
//
typedef struct  _RTMP_DMABUF
{
	ULONG                   AllocSize;
	PVOID                   AllocVa;            // TxBuf virtual address
	NDIS_PHYSICAL_ADDRESS   AllocPa;            // TxBuf physical address
} RTMP_DMABUF, *PRTMP_DMABUF;


typedef	union	_HEADER_802_11_SEQ{
#ifdef BIG_ENDAIN
    struct {
   	USHORT			Sequence:12;
	USHORT			Frag:4;
    }   field;
#else
    struct {
	USHORT			Frag:4;
	USHORT			Sequence:12;
    }   field;
#endif
    USHORT           value;
}	HEADER_802_11_SEQ, *PHEADER_802_11_SEQ;

//
//  Data buffer for DMA operation, the buffer must be contiguous physical memory
//  Both DMA to / from CPU use the same structure.
//
typedef struct  _RTMP_REORDERBUF
{
	BOOLEAN			IsFull;
	PVOID                   AllocVa;            // TxBuf virtual address
	UCHAR			Header802_3[14];
	HEADER_802_11_SEQ			Sequence;	//support compressed bitmap BA, so no consider fragment in BA
	UCHAR 		DataOffset;
	USHORT 		Datasize;
	ULONG                   AllocSize;
#ifdef RT2860
	NDIS_PHYSICAL_ADDRESS   AllocPa;            // TxBuf physical address
#endif // RT2860 //
}   RTMP_REORDERBUF, *PRTMP_REORDERBUF;

//
// Control block (Descriptor) for all ring descriptor DMA operation, buffer must be 
// contiguous physical memory. NDIS_PACKET stored the binding Rx packet descriptor
// which won't be released, driver has to wait until upper layer return the packet
// before giveing up this rx ring descriptor to ASIC. NDIS_BUFFER is assocaited pair
// to describe the packet buffer. For Tx, NDIS_PACKET stored the tx packet descriptor
// which driver should ACK upper layer when the tx is physically done or failed.
//
typedef struct _RTMP_DMACB
{
	ULONG                   AllocSize;          // Control block size
	PVOID                   AllocVa;            // Control block virtual address
	NDIS_PHYSICAL_ADDRESS   AllocPa;            // Control block physical address
	PNDIS_PACKET pNdisPacket;
	PNDIS_PACKET pNextNdisPacket;

	RTMP_DMABUF             DmaBuf;             // Associated DMA buffer structure
} RTMP_DMACB, *PRTMP_DMACB;

typedef struct _RTMP_TX_BUF
{
	PQUEUE_ENTRY    Next;
	UCHAR           Index;
	ULONG                   AllocSize;          // Control block size
	PVOID                   AllocVa;            // Control block virtual address
	NDIS_PHYSICAL_ADDRESS   AllocPa;            // Control block physical address
} RTMP_TXBUF, *PRTMP_TXBUF;

typedef struct _RTMP_RX_BUF
{
	BOOLEAN           InUse;
	ULONG           	ByBaRecIndex;
	RTMP_REORDERBUF	MAP_RXBuf[MAX_RX_REORDERBUF];
} RTMP_RXBUF, *PRTMP_RXBUF;
typedef struct _RTMP_TX_RING
{
	//RTMP_DMACB  Cell[DEF_TX_RING_SIZE]; // max TX_RING_SIZE
	UINT32		TxCpuIdx;	
	UINT32		TxDmaIdx;   
	UINT32		TxSwFreeIdx; 	// software next free tx index
} RTMP_TX_RING, *PRTMP_TX_RING;
#if 0
typedef struct _RTMP_RX_RING
{
	RTMP_DMACB  Cell[RX_RING_SIZE];
	UINT32		RxCpuIdx;
	UINT32		RxDmaIdx;
	INT32		RxSwReadIdx; 	// software next read index
} RTMP_RX_RING, *PRTMP_RX_RING;
#endif

#if 0
typedef struct _RTMP_MGMT_RING
{
	RTMP_DMACB  Cell[MGMT_RING_SIZE];
	UINT32		TxCpuIdx;
	UINT32		TxDmaIdx;
	UINT32		TxSwFreeIdx; // software next free tx index
} RTMP_MGMT_RING, *PRTMP_MGMT_RING;
#endif
//
//  Statistic counter structure
//
typedef struct _COUNTER_802_3
{
	// General Stats
	ULONG       GoodTransmits;
	ULONG       GoodReceives;
	ULONG       TxErrors;
	ULONG       RxErrors;
	ULONG       RxNoBuffer;

	// Ethernet Stats
	ULONG       RcvAlignmentErrors;
	ULONG       OneCollision;
	ULONG       MoreCollisions;

} COUNTER_802_3, *PCOUNTER_802_3;

typedef struct _COUNTER_802_11 {
	ULONG           Length;
	LARGE_INTEGER   LastTransmittedFragmentCount;
	LARGE_INTEGER   TransmittedFragmentCount;
	LARGE_INTEGER   MulticastTransmittedFrameCount;
	LARGE_INTEGER   FailedCount;
	LARGE_INTEGER   RetryCount;
	LARGE_INTEGER   MultipleRetryCount;
	LARGE_INTEGER   RTSSuccessCount;
	LARGE_INTEGER   RTSFailureCount;
	LARGE_INTEGER   ACKFailureCount;
	LARGE_INTEGER   FrameDuplicateCount;
	LARGE_INTEGER   ReceivedFragmentCount;
	LARGE_INTEGER   MulticastReceivedFrameCount;
	LARGE_INTEGER   FCSErrorCount;
} COUNTER_802_11, *PCOUNTER_802_11;

typedef struct _COUNTER_RALINK {
	ULONG           TransmittedByteCount;   // both successful and failure, used to calculate TX throughput
	ULONG           ReceivedByteCount;      // both CRC okay and CRC error, used to calculate RX throughput
	ULONG           BeenDisassociatedCount;
	ULONG           BadCQIAutoRecoveryCount;
	ULONG           PoorCQIRoamingCount;
	ULONG           MgmtRingFullCount;
	ULONG           RxCount;
	ULONG           RxRingErrCount;
	ULONG           KickTxCount;
	ULONG           TxRingErrCount;
	LARGE_INTEGER   RealFcsErrCount;
	ULONG           PendingNdisPacketCount;

	ULONG           OneSecOsTxCount[NUM_OF_TX_RING];
	ULONG           OneSecDmaDoneCount[NUM_OF_TX_RING];
	UINT32          OneSecTxDoneCount;
	ULONG           OneSecRxCount;
	UINT32           OneSecTxAggregationCount;
	UINT32           OneSecRxAggregationCount;

	UINT32   		OneSecFrameDuplicateCount;

	UINT32           OneSecTxNoRetryOkCount;
	UINT32           OneSecTxRetryOkCount;
	UINT32           OneSecTxFailCount;
	UINT32           OneSecFalseCCACnt;      // CCA error count, for debug purpose, might move to global counter
	UINT32           OneSecRxOkCnt;          // RX without error
	UINT32           OneSecRxOkDataCnt;      // unicast-to-me DATA frame count
	UINT32           OneSecRxFcsErrCnt;      // CRC error
	UINT32           OneSecBeaconSentCnt;
	UINT32           LastOneSecTotalTxCount; // OneSecTxNoRetryOkCount + OneSecTxRetryOkCount + OneSecTxFailCount
	UINT32           LastOneSecRxOkDataCnt;  // OneSecRxOkDataCnt
	ULONG		DuplicateRcv;
	ULONG		TxAggCount;
	ULONG		TxNonAggCount;
	ULONG		TxAgg1MPDUCount;
	ULONG		TxAgg2MPDUCount;
	ULONG		TxAgg3MPDUCount;
	ULONG		TxAgg4MPDUCount;
	ULONG		TxAgg5MPDUCount;
	ULONG		TxAgg6MPDUCount;
	ULONG		TxAgg7MPDUCount;
	ULONG		TxAgg8MPDUCount;
	ULONG		TxAgg9MPDUCount;
	ULONG		TxAgg10MPDUCount;
	ULONG		TxAgg11MPDUCount;
	ULONG		TxAgg12MPDUCount;
	ULONG		TxAgg13MPDUCount;
	ULONG		TxAgg14MPDUCount;
	ULONG		TxAgg15MPDUCount;
	ULONG		TxAgg16MPDUCount;

	LARGE_INTEGER       TransmittedOctetsInAMSDU; 
	LARGE_INTEGER       TransmittedAMSDUCount; 
	LARGE_INTEGER       ReceivedOctesInAMSDUCount; 
	LARGE_INTEGER       ReceivedAMSDUCount; 
	LARGE_INTEGER       TransmittedAMPDUCount;
	LARGE_INTEGER       TransmittedMPDUsInAMPDUCount;
	LARGE_INTEGER       TransmittedOctetsInAMPDUCount; 		
	LARGE_INTEGER       MPDUInReceivedAMPDUCount;
} COUNTER_RALINK, *PCOUNTER_RALINK;

typedef struct _PID_COUNTER {
	ULONG           TxAckRequiredCount;      // CRC error
	ULONG           TxAggreCount;
	ULONG           TxSuccessCount; // OneSecTxNoRetryOkCount + OneSecTxRetryOkCount + OneSecTxFailCount
	ULONG		LastSuccessRate;
} PID_COUNTER, *PPID_COUNTER;

typedef struct _COUNTER_DRS {
	// to record the each TX rate's quality. 0 is best, the bigger the worse.
	USHORT          TxQuality[MAX_STEP_OF_TX_RATE_SWITCH];
	UCHAR           PER[MAX_STEP_OF_TX_RATE_SWITCH];
	UCHAR           TxRateUpPenalty;      // extra # of second penalty due to last unstable condition
	ULONG           CurrTxRateStableTime; // # of second in current TX rate
	BOOLEAN         fNoisyEnvironment;
	BOOLEAN         fLastSecAccordingRSSI;
	UCHAR           LastSecTxRateChangeAction; // 0: no change, 1:rate UP, 2:rate down
	UCHAR			LastTimeTxRateChangeAction; //Keep last time value of LastSecTxRateChangeAction
	ULONG			LastTxOkCount;
} COUNTER_DRS, *PCOUNTER_DRS;


//
//  Arcfour Structure Added by PaulWu
//
typedef struct  _ARCFOUR
{
	UINT_32			X;
	UINT_32            Y;
	UCHAR           STATE[256];
} ARCFOURCONTEXT, *PARCFOURCONTEXT;


typedef struct {
    UINT_32 BlockIndex1;
    UINT_32 BlockIndex2;
    UINT_8 KeyBlock[256];
} ARC4_CTX_STRUC, *PARC4_CTX_STRUC;

#ifdef OLD
// MIMO Tx parameter, ShortGI, MCS, STBC, etc.  these are fields in TXWI. Don't change this definition!!!
typedef union  _HTTRANSMIT_SETTING {
#ifdef BIG_ENDIAN
	struct	{
	USHORT		MODE:2;	// Use definition MODE_xxx.  
	USHORT		rsv:3;	 
	USHORT		STBC:2;	//SPACE 
	USHORT		ShortGI:1;
	USHORT		BW:1;	//channel bandwidth 20MHz or 40 MHz
	USHORT   	MCS:7;                 // MCS
	}	field;
#else
	struct	{
	USHORT   	MCS:7;                 // MCS
	USHORT		BW:1;	//channel bandwidth 20MHz or 40 MHz
	USHORT		ShortGI:1;
	USHORT		STBC:2;	//SPACE 
	USHORT		rsv:3;	 
	USHORT		MODE:2;	// Use definition MODE_xxx.  
	}	field;
#endif
	USHORT		word;
 } HTTRANSMIT_SETTING, *PHTTRANSMIT_SETTING;
#else


/* MIMO Tx parameter, ShortGI, MCS, STBC, etc.  these are fields in TXWI. Don't change this definition!!! */
typedef union _HTTRANSMIT_SETTING {
#ifdef RT_BIG_ENDIAN
	struct {
		USHORT MODE:3;	/* Use definition MODE_xxx. */
		USHORT iTxBF:1;
		USHORT eTxBF:1;
		USHORT STBC:1;	/* only support in HT/VHT mode with MCS0~7 */
		USHORT ShortGI:1;
		USHORT BW:2;	/* channel bandwidth 20MHz/40/80 MHz */
		USHORT ldpc:1;
		USHORT MCS:6;	/* MCS */
	} field;
#else
	struct {
		USHORT MCS:6;
		USHORT ldpc:1;
		USHORT BW:2;
		USHORT ShortGI:1;
		USHORT STBC:1;
		USHORT eTxBF:1;
		USHORT iTxBF:1;
		USHORT MODE:3;
	} field;
#endif
	USHORT word;
} HTTRANSMIT_SETTING, *PHTTRANSMIT_SETTING;


#endif

// MIMO Tx parameter, ShortGI, MCS, STBC, etc.  these are fields in TXWI too. just copy to TXWI.
typedef struct  _RECEIVE_SETTING {
#ifdef BIG_ENDIAN
	USHORT		MIMO:1;
	USHORT		OFDM:1;
	USHORT		rsv:3;	
	USHORT		STBC:2;	//SPACE 
	USHORT		ShortGI:1;
	USHORT		Mode:2;	//channel bandwidth 20MHz or 40 MHz
	USHORT   	NumOfRX:2;                 // MIMO. WE HAVE 3R 
#else
	USHORT   	NumOfRX:2;                 // MIMO. WE HAVE 3R 
	USHORT		Mode:2;	//channel bandwidth 20MHz or 40 MHz
	USHORT		ShortGI:1;
	USHORT		STBC:2;	//SPACE 
	USHORT		rsv:3;	 
	USHORT		OFDM:1;
	USHORT		MIMO:1;
#endif
 } RECEIVE_SETTING, *PRECEIVE_SETTING;

// Shared key data structure
typedef struct  _WEP_KEY {
	UCHAR   KeyLen;                     // Key length for each key, 0: entry is invalid
	UCHAR   Key[MAX_LEN_OF_KEY];        // right now we implement 4 keys, 128 bits max
} WEP_KEY, *PWEP_KEY;

#if N9_INIT_PORTING

#ifndef CIPHER_KEY_DEFINED
typedef struct _CIPHER_KEY {
	UCHAR   Key[16];            // right now we implement 4 keys, 128 bits max
	UCHAR   RxMic[8];			// make alignment 
	UCHAR   TxMic[8];
	UCHAR   TxTsc[6];           // 48bit TSC value
	UCHAR   RxTsc[6];           // 48bit TSC value
	UCHAR   CipherAlg;          // 0-none, 1:WEP64, 2:WEP128, 3:TKIP, 4:AES, 5:CKIP64, 6:CKIP128
	UCHAR   KeyLen; 
#ifdef CONFIG_STA_SUPPORT
	UCHAR   BssId[6];
#endif  // CONFIG_STA_SUPPORT //
            // Key length for each key, 0: entry is invalid
	UCHAR   Type;               // Indicate Pairwise/Group when reporting MIC error
} CIPHER_KEY, *PCIPHER_KEY;
#define CIPHER_KEY_DEFINED
#endif
#endif
typedef struct _BBP_TUNING_STRUCT {
	BOOLEAN     Enable;
	UCHAR       FalseCcaCountUpperBound;  // 100 per sec
	UCHAR       FalseCcaCountLowerBound;  // 10 per sec
	UCHAR       R17LowerBound;            // specified in E2PROM
	UCHAR       R17UpperBound;            // 0x68 according to David Tung
	UCHAR       CurrentR17Value;
} BBP_TUNING, *PBBP_TUNING;

typedef struct _SOFT_RX_ANT_DIVERSITY_STRUCT {
	UCHAR EvaluatePeriod;	/* 0:not evalute status, 1: evaluate status, 2: switching status */
	UCHAR EvaluateStableCnt;
	UCHAR Pair1PrimaryRxAnt;	/* 0:Ant-E1, 1:Ant-E2 */
	UCHAR Pair1SecondaryRxAnt;	/* 0:Ant-E1, 1:Ant-E2 */
	SHORT Pair1AvgRssi[2];	/* AvgRssi[0]:E1, AvgRssi[1]:E2 */
	SHORT Pair2AvgRssi[2];	/* AvgRssi[0]:E3, AvgRssi[1]:E4 */
	SHORT Pair1LastAvgRssi;	/* */
	SHORT Pair2LastAvgRssi;	/* */
	ULONG RcvPktNumWhenEvaluate;
	BOOLEAN FirstPktArrivedWhenEvaluate;
} SOFT_RX_ANT_DIVERSITY, *PSOFT_RX_ANT_DIVERSITY;

typedef struct _LEAP_AUTH_INFO {
	BOOLEAN         Enabled;        //Ture: Enable LEAP Authentication
	BOOLEAN         CCKM;           //Ture: Use Fast Reauthentication with CCKM
	UCHAR           Reserve[2];
	UCHAR           UserName[256];  //LEAP, User name
	ULONG           UserNameLen;
	UCHAR           Password[256];  //LEAP, User Password
	ULONG           PasswordLen;
} LEAP_AUTH_INFO, *PLEAP_AUTH_INFO;

typedef struct {
	UCHAR        Addr[MAC_ADDR_LEN];
	UCHAR        ErrorCode[2];  //00 01-Invalid authentication type
								//00 02-Authentication timeout
								//00 03-Challenge from AP failed
								//00 04-Challenge to AP failed
	BOOLEAN      Reported;
} ROGUEAP_ENTRY, *PROGUEAP_ENTRY;

typedef struct {
	UCHAR               RogueApNr;
	//ROGUEAP_ENTRY       RogueApEntry[MAX_LEN_OF_BSS_TABLE];
	ROGUEAP_ENTRY       *RogueApEntry;
} ROGUEAP_TABLE, *PROGUEAP_TABLE;

typedef struct {
	BOOLEAN     Enable;
	UCHAR       Delta;
	BOOLEAN     PlusSign;
} CCK_TX_POWER_CALIBRATE, *PCCK_TX_POWER_CALIBRATE;

//
// Receive Tuple Cache Format
//
typedef struct  _TUPLE_CACHE    {
	BOOLEAN         Valid;
	UCHAR           MacAddress[MAC_ADDR_LEN];
	USHORT          Sequence; 
	USHORT          Frag;
} TUPLE_CACHE, *PTUPLE_CACHE;

//
// Fragment Frame structure
//
typedef struct  _FRAGMENT_FRAME {
//	UCHAR       Header802_3[LENGTH_802_3];
//	UCHAR       Header_LLC[LENGTH_802_1_H];
//	UCHAR       Buffer[LENGTH_802_3 + MAX_FRAME_SIZE];  // Add header to prevent NETBUEI continuous buffer isssue
	PNDIS_PACKET    pFragPacket;
	ULONG       	RxSize;
	USHORT      	Sequence;
	USHORT      	LastFrag;
	ULONG       	Flags;          // Some extra frame information. bit 0: LLC presented
} FRAGMENT_FRAME, *PFRAGMENT_FRAME;


//
// Packet information for NdisQueryPacket
//
typedef struct  _PACKET_INFO    {
	UINT            PhysicalBufferCount;    // Physical breaks of buffer descripor chained
	UINT            BufferCount ;           // Number of Buffer descriptor chained
	UINT            TotalPacketLength ;     // Self explained
	PNDIS_BUFFER    pFirstBuffer;           // Pointer to first buffer descriptor
} PACKET_INFO, *PPACKET_INFO;

//
// Tkip Key structure which RC4 key & MIC calculation
//
typedef struct  _TKIP_KEY_INFO  {
	UINT        nBytesInM;  // # bytes in M for MICKEY
	ULONG       IV16;
	ULONG       IV32;   
	ULONG       K0;         // for MICKEY Low
	ULONG       K1;         // for MICKEY Hig
	ULONG       L;          // Current state for MICKEY
	ULONG       R;          // Current state for MICKEY
	ULONG       M;          // Message accumulator for MICKEY
	UCHAR       RC4KEY[16];
	UCHAR       MIC[8];
} TKIP_KEY_INFO, *PTKIP_KEY_INFO;

// structure to define WPA Group Key Rekey Interval
#ifdef IAR_COMPILE
PACKED typedef struct  _RT_802_11_WPA_REKEY {
#else
typedef struct PACKED _RT_802_11_WPA_REKEY {
#endif
	ULONG ReKeyMethod;          // mechanism for rekeying: 0:disable, 1: time-based, 2: packet-based
	ULONG ReKeyInterval;        // time-based: seconds, packet-based: kilo-packets
} RT_WPA_REKEY,*PRT_WPA_REKEY, RT_802_11_WPA_REKEY, *PRT_802_11_WPA_REKEY;

//
// Private / Misc data, counters for driver internal use
//
typedef struct  __PRIVATE_STRUC {
	UINT       		FCSCRC32;
	ARCFOURCONTEXT  WEPCONTEXT;
	// Tkip stuff
	TKIP_KEY_INFO   Tx;
	TKIP_KEY_INFO   Rx;
	UINT       		SystemResetCnt;         // System reset counter
	UINT       		TxRingFullCnt;          // Tx ring full occurrance number
	UINT       		PhyRxErrCnt;            // PHY Rx error count, for debug purpose, might move to global counter
	// Variables for WEP encryption / decryption in rtmp_wep.c
} PRIVATE_STRUC, *PPRIVATE_STRUC;

// structure to tune BBP R66 (BBP TUNING)
typedef struct _BBP_R66_TUNING {
	BOOLEAN     bEnable;
	USHORT      FalseCcaLowerThreshold;  // default 100
	USHORT      FalseCcaUpperThreshold;  // default 512
	UCHAR       R66Delta;
	UCHAR       R66CurrentValue;
	BOOLEAN		R66LowerUpperSelect; //Before LinkUp, Used LowerBound or UpperBound as R66 value.
} BBP_R66_TUNING, *PBBP_R66_TUNING;

#if 1 //def DOT11_N_SUPPORT
#if 1 //def DOT11N_DRAFT3
#define EFFECTED_CH_SECONDARY 0x1
#define EFFECTED_CH_PRIMARY	0x2
#define EFFECTED_CH_LEGACY		0x4
#endif // DOT11N_DRAFT3 //
#endif // DOT11_N_SUPPORT //



// structure to store channel TX power
typedef struct _CHANNEL_TX_POWER {
	USHORT     RemainingTimeForUse;		//unit: sec
	UCHAR      Channel;
#if 1 //def DOT11N_DRAFT3
	BOOLEAN       bEffectedChannel;	// For BW 40 operating in 2.4GHz , the "effected channel" is the channel that is covered in 40Mhz.
#endif // DOT11N_DRAFT3 //	
	CHAR       Power;
	CHAR       Power2;
	UCHAR      MaxTxPwr;
	UCHAR      DfsReq;
} CHANNEL_TX_POWER, *PCHANNEL_TX_POWER;

// structure to store 802.11j channel TX power
typedef struct _CHANNEL_11J_TX_POWER {
	UCHAR      Channel;
	UCHAR      BW;	// BW_10 or BW_20
	CHAR       Power;
	CHAR       Power2;
	USHORT     RemainingTimeForUse;		//unit: sec
} CHANNEL_11J_TX_POWER, *PCHANNEL_11J_TX_POWER;

typedef enum _ABGBAND_STATE_ {
	UNKNOWN_BAND,
	BG_BAND,
	A_BAND,
} ABGBAND_STATE;

typedef struct _MLME_STRUCT {
#ifdef CONFIG_STA_SUPPORT
	// STA state machines
	STATE_MACHINE           CntlMachine;
	STATE_MACHINE           AssocMachine;
	STATE_MACHINE           AuthMachine;
	STATE_MACHINE           AuthRspMachine;
	STATE_MACHINE           SyncMachine;
	STATE_MACHINE           WpaPskMachine;
	//STATE_MACHINE           LeapMachine;
	//STATE_MACHINE           AironetMachine;
	STATE_MACHINE_FUNC      AssocFunc[ASSOC_FUNC_SIZE];
	STATE_MACHINE_FUNC      AuthFunc[AUTH_FUNC_SIZE];
	STATE_MACHINE_FUNC      AuthRspFunc[AUTH_RSP_FUNC_SIZE];
	STATE_MACHINE_FUNC      SyncFunc[SYNC_FUNC_SIZE];
	STATE_MACHINE_FUNC      WpaPskFunc[WPA_PSK_FUNC_SIZE];
	STATE_MACHINE_FUNC      AironetFunc[AIRONET_FUNC_SIZE];
#endif // CONFIG_STA_SUPPORT //
#ifdef CONFIG_AP_SUPPORT
		/* AP state machines */
		STATE_MACHINE ApAssocMachine;
		STATE_MACHINE ApAuthMachine;
		STATE_MACHINE ApSyncMachine;
		STATE_MACHINE_FUNC ApAssocFunc[AP_ASSOC_FUNC_SIZE];
	/*	STATE_MACHINE_FUNC		ApDlsFunc[DLS_FUNC_SIZE]; */
		STATE_MACHINE_FUNC ApAuthFunc[AP_AUTH_FUNC_SIZE];
		STATE_MACHINE_FUNC ApSyncFunc[AP_SYNC_FUNC_SIZE];
#ifdef APCLI_SUPPORT
		STATE_MACHINE ApCliAuthMachine;
		STATE_MACHINE ApCliAssocMachine;
		STATE_MACHINE ApCliCtrlMachine;
		STATE_MACHINE ApCliSyncMachine;
		STATE_MACHINE ApCliWpaPskMachine;
	
		STATE_MACHINE_FUNC ApCliAuthFunc[APCLI_AUTH_FUNC_SIZE];
		STATE_MACHINE_FUNC ApCliAssocFunc[APCLI_ASSOC_FUNC_SIZE];
		STATE_MACHINE_FUNC ApCliCtrlFunc[APCLI_CTRL_FUNC_SIZE];
		STATE_MACHINE_FUNC ApCliSyncFunc[APCLI_SYNC_FUNC_SIZE];
#endif /* APCLI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	STATE_MACHINE_FUNC      ActFunc[ACT_FUNC_SIZE];
	// Action 
	STATE_MACHINE           ActMachine;

#ifdef WSC_INCLUDED
    STATE_MACHINE			WscMachine;
    STATE_MACHINE_FUNC      WscFunc[WSC_FUNC_SIZE];
#endif // WSC_INCLUDED //


	ULONG                   ChannelQuality;  // 0..100, Channel Quality Indication for Roaming
	ULONG                   Now32;           // latch the value of NdisGetSystemUpTime()

	BOOLEAN                 bRunning;
	NDIS_SPIN_LOCK          TaskLock;
	MLME_QUEUE              Queue;

	UINT                    ShiftReg;

	RALINK_TIMER_STRUCT     PeriodicTimer;
	RALINK_TIMER_STRUCT     LinkDownTimer;
	RALINK_TIMER_STRUCT     LinkUpTimer;
	ULONG                   PeriodicRound;
	ULONG                   OneSecPeriodicRound;
#ifndef MLME_AGENT_SUPPORT	
	BOOLEAN			bEnableAutoAntennaCheck;
#endif

	UCHAR					RealRxPath;
	BOOLEAN					bLowThroughput;
	RALINK_TIMER_STRUCT		RxAntEvalTimer;
} MLME_STRUCT, *PMLME_STRUCT;

// structure for radar detection and channel switch
typedef struct _RADAR_DETECT_STRUCT {
    //BOOLEAN		IEEE80211H;			// 0: disable, 1: enable IEEE802.11h
	UCHAR		CSCount;			//Channel switch counter
	UCHAR		CSPeriod;			//Channel switch period (beacon count)
	UCHAR		RDCount;			//Radar detection counter
	UCHAR		RDMode;				//Radar Detection mode
	UCHAR		RDDurRegion;		//Radar detection duration region
	UCHAR		BBPR16;
	UCHAR		BBPR17;
	UCHAR		BBPR18;
	UCHAR		BBPR21;
	UCHAR		BBPR22;
	UCHAR		BBPR64;
	ULONG		InServiceMonitorCount; // unit: sec
	UINT8		DfsSessionTime;
	BOOLEAN		bFastDfs;
	UINT8		ChMovingTime;
	UINT8		LongPulseRadarTh;
} RADAR_DETECT_STRUCT, *PRADAR_DETECT_STRUCT;

#ifdef CARRIER_DETECTION_SUPPORT
typedef enum CD_STATE_n
{
	CD_NORMAL,
	CD_SILENCE,
	CD_MAX_STATE
} CD_STATE;

typedef struct CARRIER_DETECTION_s
{
	BOOLEAN					Enable;
	UINT8					CDSessionTime;
	UINT8					CDPeriod;
	CD_STATE				CD_State;
} CARRIER_DETECTION, *PCARRIER_DETECTION;
#endif // CARRIER_DETECTION_SUPPORT //

typedef enum _REC_BLOCKACK_STATUS
{	
    Recipient_NONE=0,
	Recipient_USED,
	Recipient_HandleRes,
    Recipient_Accept
} REC_BLOCKACK_STATUS, *PREC_BLOCKACK_STATUS;

typedef enum _ORI_BLOCKACK_STATUS
{
    Originator_NONE=0,
	Originator_USED,
    Originator_WaitRes,
    Originator_Done
} ORI_BLOCKACK_STATUS, *PORI_BLOCKACK_STATUS;


typedef struct _BA_ORI_ENTRY{
	UCHAR   Wcid;
	UCHAR   TID;
	UCHAR   BAWinSize;
	UCHAR   Token;
// Sequence is to fill every outgoing QoS DATA frame's sequence field in 802.11 header.
	USHORT	Sequence;
	USHORT	TimeOutValue;
	ORI_BLOCKACK_STATUS  ORI_BA_Status;
	RALINK_TIMER_STRUCT ORIBATimer;
	PVOID	pAdapter;
} BA_ORI_ENTRY, *PBA_ORI_ENTRY;

typedef struct _BA_REC_ENTRY {
	UCHAR   Wcid;
	UCHAR   TID;
	UCHAR   BAWinSize;	// 7.3.1.14. each buffer is capable of holding a max AMSDU or MSDU.
	//UCHAR	NumOfRxPkt;
	//UCHAR    Curindidx; // the head in the RX reordering buffer
	USHORT		LastIndSeq;
//	USHORT		LastIndSeqAtTimer;
	USHORT		TimeOutValue;
	RALINK_TIMER_STRUCT RECBATimer;
	ULONG		LastIndSeqAtTimer;
	ULONG		nDropPacket;
	ULONG		rcvSeq;
	REC_BLOCKACK_STATUS  REC_BA_Status;
//	UCHAR	RxBufIdxUsed;
	// corresponding virtual address for RX reordering packet storage.
	//RTMP_REORDERDMABUF MAP_RXBuf[MAX_RX_REORDERBUF]; 
	NDIS_SPIN_LOCK          RxReRingLock;                 // Rx Ring spinlock
//	struct _BA_REC_ENTRY *pNext;
	PVOID	pAdapter;
	struct reordering_list	list;
} BA_REC_ENTRY, *PBA_REC_ENTRY;


typedef struct {
	ULONG		numAsRecipient;		// I am recipient of numAsRecipient clients. These client are in the BARecEntry[]
	ULONG		numAsOriginator;	// I am originator of 	numAsOriginator clients. These clients are in the BAOriEntry[]
	BA_ORI_ENTRY       *BAOriEntry;//[MAX_LEN_OF_BA_ORI_TABLE];
	BA_REC_ENTRY       *BARecEntry;//[MAX_LEN_OF_BA_REC_TABLE];
} BA_TABLE, *PBA_TABLE;

//For QureyBATableOID use;
#ifdef IAR_COMPILE
PACKED typedef struct  _OID_BA_REC_ENTRY {
#else
typedef struct PACKED _OID_BA_REC_ENTRY {
#endif
	UCHAR   MACAddr[MAC_ADDR_LEN];
	UCHAR   BaBitmap;   // if (BaBitmap&(1<<TID)), this session with{MACAddr, TID}exists, so read BufSize[TID] for BufferSize
	UCHAR   rsv; 
	UCHAR   BufSize[8];
	REC_BLOCKACK_STATUS	REC_BA_Status[8];
} OID_BA_REC_ENTRY, *POID_BA_REC_ENTRY;

//For QureyBATableOID use;
#ifdef IAR_COMPILE
PACKED typedef struct  _OID_BA_ORI_ENTRY {
#else
typedef struct PACKED _OID_BA_ORI_ENTRY {
#endif
	UCHAR   MACAddr[MAC_ADDR_LEN];
	UCHAR   BaBitmap;  // if (BaBitmap&(1<<TID)), this session with{MACAddr, TID}exists, so read BufSize[TID] for BufferSize, read ORI_BA_Status[TID] for status
	UCHAR   rsv; 
	UCHAR   BufSize[8];
	ORI_BLOCKACK_STATUS  ORI_BA_Status[8];
} OID_BA_ORI_ENTRY, *POID_BA_ORI_ENTRY;

typedef struct _QUERYBA_TABLE{
	OID_BA_ORI_ENTRY       BAOriEntry[32];
	OID_BA_REC_ENTRY       BARecEntry[32];
	UCHAR   OriNum;// Number of below BAOriEntry
	UCHAR   RecNum;// Number of below BARecEntry
} QUERYBA_TABLE, *PQUERYBA_TABLE;

typedef	union	_BACAP_STRUC	{
#ifdef BIG_ENDIAN
	struct	{
		UINT32     :6;	
		UINT32     MMPSmode:2;	// MIMO power save more, 0:static, 1:dynamic, 2:rsv, 3:mimo enable
		UINT32     AmsduSize:1;	// 0:3839, 1:7935 bytes. UINT  MSDUSizeToBytes[]	= { 3839, 7935};
		UINT32     AmsduEnable:1;	//Enable AMSDU transmisstion
		UINT32		MpduDensity:3;	
		UINT32		Policy:2;	// 0: DELAY_BA 1:IMMED_BA  (//BA Policy subfiled value in ADDBA frame)   2:BA-not use
		UINT32		AutoBA:1;	// automatically BA  	
		UINT32		TxBAWinLimit:8;
		UINT32		RxBAWinLimit:8;
	}	field;
#else
	struct	{
		UINT32		RxBAWinLimit:8;
		UINT32		TxBAWinLimit:8;
		UINT32		AutoBA:1;	// automatically BA  	
		UINT32		Policy:2;	// 0: DELAY_BA 1:IMMED_BA  (//BA Policy subfiled value in ADDBA frame)   2:BA-not use
		UINT32		MpduDensity:3;	
		UINT32       	AmsduEnable:1;	//Enable AMSDU transmisstion
		UINT32       	AmsduSize:1;	// 0:3839, 1:7935 bytes. UINT  MSDUSizeToBytes[]	= { 3839, 7935};
		UINT32       	MMPSmode:2;	// MIMO power save more, 0:static, 1:dynamic, 2:rsv, 3:mimo enable
		UINT32       	:6;	
	}	field;
#endif
	UINT32			word;
} BACAP_STRUC, *PBACAP_STRUC;

//This structure is for all 802.11n card InterOptibilityTest action. Reset all Num every n second.  (Details see MLMEPeriodic)
typedef	struct	_IOT_STRUC	{
	UCHAR			Threshold[2];
#ifdef SHRINK
	UCHAR			ReorderTimeOutNum[MAX_LEN_OF_BA_REC_TABLE];	// compare with threshold[0]
	UCHAR			RefreshNum[MAX_LEN_OF_BA_REC_TABLE];	// compare with threshold[1]
#endif
	ULONG			OneSecInWindowCount;
	ULONG			OneSecFrameDuplicateCount;
	ULONG			OneSecOutWindowCount;
	UCHAR			DelOriAct;
	UCHAR			DelRecAct;
	UCHAR			RTSShortProt;
	UCHAR			RTSLongProt;
	BOOLEAN			bRTSLongProtOn;
#ifdef CONFIG_STA_SUPPORT 
	BOOLEAN			bLastAtheros;
    BOOLEAN			bCurrentAtheros;
    BOOLEAN         bNowAtherosBurstOn;
	BOOLEAN			bNextDisableRxBA;
    BOOLEAN			bToggle;
#endif // CONFIG_STA_SUPPORT //
} IOT_STRUC, *PIOT_STRUC;

// This is the registry setting for 802.11n transmit setting.  Used in advanced page.
typedef union _REG_TRANSMIT_SETTING {
#ifdef BIG_ENDIAN
 struct {
         UINT32  rsv:13;
		 UINT32  EXTCHA:2;
		 UINT32  HTMODE:1;
		 UINT32  TRANSNO:2; 
		 UINT32  STBC:1; //SPACE 
		 UINT32  ShortGI:1;
		 UINT32  BW:1; //channel bandwidth 20MHz or 40 MHz
		 UINT32  MCS:7;                 // MCS
         UINT32  PhyMode:4;                
    } field;
#else
 struct {
         UINT32  PhyMode:4;                
         UINT32  MCS:7;                 // MCS
         UINT32  BW:1; //channel bandwidth 20MHz or 40 MHz
         UINT32  ShortGI:1;
         UINT32  STBC:1; //SPACE 
         UINT32  TRANSNO:2; 
         UINT32  HTMODE:1;
         UINT32  EXTCHA:2;
         UINT32  rsv:13;
    } field;
#endif
 UINT32   word;
} REG_TRANSMIT_SETTING, *PREG_TRANSMIT_SETTING;

typedef union  _DESIRED_TRANSMIT_SETTING {
#ifdef RT_BIG_ENDIAN
	struct	{
			USHORT		rsv:3;	 
			USHORT		FixedTxMode:2;			// If MCS isn't AUTO, fix rate in CCK, OFDM or HT mode.
			USHORT		PhyMode:4;	
			USHORT   	MCS:7;                 // MCS
	}	field;
#else
	struct	{
			USHORT   	MCS:7;                 	// MCS
			USHORT		PhyMode:4;				
			USHORT	 	FixedTxMode:2;			// If MCS isn't AUTO, fix rate in CCK, OFDM or HT mode.
			USHORT		rsv:3;	 	 
	}	field;
#endif
	USHORT		word;
 } DESIRED_TRANSMIT_SETTING, *PDESIRED_TRANSMIT_SETTING;


//
//  For OID Query or Set about BA structure
//
typedef	struct	_OID_BACAP_STRUC	{
		UCHAR		RxBAWinLimit;
		UCHAR		TxBAWinLimit;
		UCHAR		Policy;	// 0: DELAY_BA 1:IMMED_BA  (//BA Policy subfiled value in ADDBA frame)   2:BA-not use. other value invalid
		UCHAR		MpduDensity;	// 0: DELAY_BA 1:IMMED_BA  (//BA Policy subfiled value in ADDBA frame)   2:BA-not use. other value invalid
		UCHAR       	AmsduEnable;	//Enable AMSDU transmisstion
		UCHAR       	AmsduSize;	// 0:3839, 1:7935 bytes. UINT  MSDUSizeToBytes[]	= { 3839, 7935};
		UCHAR       	MMPSmode;	// MIMO power save more, 0:static, 1:dynamic, 2:rsv, 3:mimo enable
		BOOLEAN		AutoBA;	// Auto BA will automatically	
} OID_BACAP_STRUC, *POID_BACAP_STRUC;



// This is OID setting structure. So only GF or MM as Mode. This is valid when our wirelss mode has 802.11n in use.
typedef struct {
	RT_802_11_PHY_MODE		PhyMode; 	// 
	UCHAR		TransmitNo;
	UCHAR		HtMode; 	//HTMODE_GF or HTMODE_MM
	UCHAR		ExtOffset;	//extension channel above or below
	UCHAR		MCS;
	UCHAR   	BW;
	UCHAR		STBC;
	UCHAR		SHORTGI;
	UCHAR		rsv;
} OID_SET_HT_PHYMODE, *POID_SET_HT_PHYMODE;

typedef struct {
	BOOLEAN		IsRecipient;
	UCHAR   MACAddr[MAC_ADDR_LEN];
	UCHAR   TID;
	UCHAR   nMSDU;
	USHORT   TimeOut;
	BOOLEAN bAllTid;  // If True, delete all TID for BA sessions with this MACaddr.
} OID_ADD_BA_ENTRY, *POID_ADD_BA_ENTRY;


typedef enum _WpaMixPairCipher {
	MIX_CIPHER_NOTUSE = 0x00,
	WPA_NONE_WPA2_TKIPAES = 0x03,	/* WPA2-TKIPAES */
	WPA_AES_WPA2_TKIP = 0x06,
	WPA_AES_WPA2_TKIPAES = 0x07,
	WPA_TKIP_WPA2_AES = 0x09,
	WPA_TKIP_WPA2_TKIPAES = 0x0B,
	WPA_TKIPAES_WPA2_NONE = 0x0C,	/* WPA-TKIPAES */
	WPA_TKIPAES_WPA2_AES = 0x0D,
	WPA_TKIPAES_WPA2_TKIP = 0x0E,
	WPA_TKIPAES_WPA2_TKIPAES = 0x0F,
} WPA_MIX_PAIR_CIPHER;


//
// Multiple SSID structure
//
#define WLAN_MAX_NUM_OF_TIM(_pAd)			((MAX_LEN_OF_MAC_TABLE(_pAd) >> 3) + 1) /* /8 + 1 */
#define WLAN_CT_TIM_BCMC_OFFSET		0 /* unit: 32B */

/* clear bcmc TIM bit */
#if 0
#define WLAN_MR_TIM_BCMC_CLEAR(apidx) \
	pAd->ApCfg.MBSSID[apidx].TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] &= ~array_BIT8[0];

/* set bcmc TIM bit */
#define WLAN_MR_TIM_BCMC_SET(apidx) \
	pAd->ApCfg.MBSSID[apidx].TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] |= array_BIT8[0];

/* clear a station PS TIM bit */
#define WLAN_MR_TIM_BIT_CLEAR(ad_p, apidx, wcid) \
	{	UCHAR tim_offset = wcid >> 3; \
		UCHAR bit_offset = wcid & 0x7; \
		ad_p->ApCfg.MBSSID[apidx].TimBitmaps[tim_offset] &= (~array_BIT8[bit_offset]); }

/* set a station PS TIM bit */
#define WLAN_MR_TIM_BIT_SET(ad_p, apidx, wcid) \
	{	UCHAR tim_offset = wcid >> 3; \
		UCHAR bit_offset = wcid & 0x7; \
		ad_p->ApCfg.MBSSID[apidx].TimBitmaps[tim_offset] |= array_BIT8[bit_offset]; }
#else

/* clear bcmc TIM bit */
#define WLAN_MR_TIM_BCMC_CLEAR(apidx) \
	pAd->ApCfg.MBSSID[apidx].wdev.bcn_buf.TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] &= ~array_BIT8[0];

/* set bcmc TIM bit */
#define WLAN_MR_TIM_BCMC_SET(apidx) \
	pAd->ApCfg.MBSSID[apidx].wdev.bcn_buf.TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] |= array_BIT8[0];

#define WLAN_MR_TIM_BCMC_GET(apidx) \
	(pAd->ApCfg.MBSSID[apidx].wdev.bcn_buf.TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] & array_BIT8[0])

/* clear a station PS TIM bit */
#define WLAN_MR_TIM_BIT_CLEAR(ad_p, apidx, _aid) \
	{	UCHAR tim_offset = _aid >> 3; \
		UCHAR bit_offset = _aid & 0x7; \
		ad_p->ApCfg.MBSSID[apidx].wdev.bcn_buf.TimBitmaps[tim_offset] &= (~array_BIT8[bit_offset]); }

/* set a station PS TIM bit */
#define WLAN_MR_TIM_BIT_SET(ad_p, apidx, _aid) \
	{	UCHAR tim_offset = _aid >> 3; \
		UCHAR bit_offset = _aid & 0x7; \
		ad_p->ApCfg.MBSSID[apidx].wdev.bcn_buf.TimBitmaps[tim_offset] |= array_BIT8[bit_offset]; }


#endif
#define MAX_RADIUS_SRV_NUM			2	  // 802.1x failover number

#define IFNAMSIZ	16
#ifdef IAR_COMPILE
PACKED typedef struct  _RADIUS_SRV_INFO {
#else
typedef struct PACKED _RADIUS_SRV_INFO {
#endif
	UINT32			radius_ip;
	UINT32			radius_port;
	UCHAR			radius_key[64];
	UCHAR			radius_key_len;
} RADIUS_SRV_INFO, *PRADIUS_SRV_INFO;

#ifdef IAR_COMPILE
PACKED typedef struct  _RADIUS_KEY_INFO {
#else
typedef struct PACKED _RADIUS_KEY_INFO {
#endif
	UCHAR			radius_srv_num;			
	RADIUS_SRV_INFO	radius_srv_info[MAX_RADIUS_SRV_NUM];
	UCHAR			ieee8021xWEP;		 // dynamic WEP
    UCHAR           key_index;           
    UCHAR           key_length;          // length of key in bytes
    UCHAR           key_material[13];    
} RADIUS_KEY_INFO, *PRADIUS_KEY_INFO;

// It's used by 802.1x daemon to require relative configuration
#if 0
typedef struct PACKED _RADIUS_CONF
{
    UINT32          Length;             // Length of this structure    
    UCHAR			mbss_num;			// indicate multiple BSS number 
	UINT32			own_ip_addr;	
	UINT32			retry_interval;
	UINT32			session_timeout_interval;
	UCHAR			EAPifname[IFNAMSIZ];
	UCHAR			EAPifname_len;
	UCHAR 			PreAuthifname[IFNAMSIZ];
	UCHAR			PreAuthifname_len;
	RADIUS_KEY_INFO	RadiusInfo[MAX_MBSSID_NUM];
} RADIUS_CONF, *PRADIUS_CONF;
#endif
#if 0 //not required
typedef struct _MULTISSID_STRUCT {
	UCHAR								Bssid[MAC_ADDR_LEN];
    UCHAR                               SsidLen;
    CHAR                                Ssid[MAX_LEN_OF_SSID];
    USHORT                              CapabilityInfo;
    
    PNET_DEV                   			MSSIDDev;

	NDIS_802_11_AUTHENTICATION_MODE     AuthMode;
	NDIS_802_11_WEP_STATUS              WepStatus;
	NDIS_802_11_WEP_STATUS				GroupKeyWepStatus;

	ULONG								TxCount;
	ULONG								RxCount;
	ULONG								ReceivedByteCount;
	ULONG								TransmittedByteCount;
	ULONG								RxErrorCount;
	ULONG								RxDropCount;

	
	//CIPHER_KEY                          SharedKey[SHARE_KEY_NUM]; // ref pAd->SharedKey[BSS][4]
	UCHAR                               DefaultKeyId;

	UCHAR								TxRate;       // RATE_1, RATE_2, RATE_5_5, RATE_11, ...
	UCHAR     							DesiredRates[MAX_LEN_OF_SUPPORTED_RATES];// OID_802_11_DESIRED_RATES
	UCHAR								DesiredRatesIndex;
	UCHAR     							MaxTxRate;            // RATE_1, RATE_2, RATE_5_5, RATE_11

//	ULONG           					TimBitmap;      // bit0 for broadcast, 1 for AID1, 2 for AID2, ...so on
//    ULONG           					TimBitmap2;     // b0 for AID32, b1 for AID33, ... and so on
	UCHAR								TimBitmaps[WLAN_MAX_NUM_OF_TIM];

    // WPA
    UCHAR                               GMK[32];
    UCHAR                               PMK[32];
	UCHAR								GTK[32];
    BOOLEAN                             IEEE8021X;
    BOOLEAN                             PreAuth;
    UCHAR                               GNonce[32];
    UCHAR                               PortSecured;
    NDIS_802_11_PRIVACY_FILTER          PrivacyFilter;
    UCHAR                               BANClass3Data;
    ULONG                               IsolateInterStaTraffic;

    UCHAR                               RSNIE_Len[2];
    UCHAR                               RSN_IE[2][MAX_LEN_OF_RSNIE];

    UCHAR                   			TimIELocationInBeacon;
    UCHAR                   			CapabilityInfoLocationInBeacon;

    NDIS_802_11_PMKID					PMKIDCache;

    // outgoing BEACON frame buffer and corresponding TXWI
	// PTXWI_STRUC                           BeaconTxWI; // 
    CHAR                                BeaconBuf[MAX_BEACON_SIZE]; // NOTE: BeaconBuf should be 4-byte aligned

    BOOLEAN                             bHideSsid;
	UINT16								StationKeepAliveTime; // unit: second

#ifdef UCOS
	BOOLEAN								bVLAN_Tag;
#endif 
    USHORT                              VLAN_VID;
    USHORT                              VLAN_Priority;

    RT_802_11_ACL						AccessControlList;

	// EDCA Qos
    BOOLEAN								bWmmCapable;	// 0:disable WMM, 1:enable WMM
    BOOLEAN								bDLSCapable;	// 0:disable DLS, 1:enable DLS

	UCHAR           					DlsPTK[64];		// Due to windows dirver count on meetinghouse to handle 4-way shake
	
	// For 802.1x daemon setting per BSS
	UCHAR								radius_srv_num;
	RADIUS_SRV_INFO						radius_srv_info[MAX_RADIUS_SRV_NUM];
	
#ifdef RTL865X_SOC
	unsigned int						mylinkid;
#endif 

#ifdef CONFIG_AP_SUPPORT
#ifdef WSC_AP_SUPPORT
	WSC_LV_INFO            WscIEBeacon;
   	WSC_LV_INFO            WscIEProbeResp;
	WSC_CTRL	           WscControl;
#endif // WSC_AP_SUPPORT //

#ifdef IGMP_SNOOP_SUPPORT
	BOOLEAN								IgmpSnoopEnable;	// 0: disable, 1: enable.
#endif // IGMP_SNOOP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

	UINT32					RcvdConflictSsidCount;					
	UINT32					RcvdSpoofedAssocRespCount;	
	UINT32					RcvdSpoofedReassocRespCount;	
	UINT32					RcvdSpoofedProbeRespCount;
	UINT32					RcvdSpoofedBeaconCount;
	UINT32					RcvdSpoofedDisassocCount;	
	UINT32					RcvdSpoofedAuthCount;
	UINT32					RcvdSpoofedDeauthCount;	
	UINT32					RcvdSpoofedUnknownMgmtCount;	
	UINT32					RcvdReplayAttackCount;

	CHAR					RssiOfRcvdConflictSsid;					
	CHAR					RssiOfRcvdSpoofedAssocResp;	
	CHAR					RssiOfRcvdSpoofedReassocResp;	
	CHAR					RssiOfRcvdSpoofedProbeResp;
	CHAR					RssiOfRcvdSpoofedBeacon;
	CHAR					RssiOfRcvdSpoofedDisassoc;	
	CHAR					RssiOfRcvdSpoofedAuth;
	CHAR					RssiOfRcvdSpoofedDeauth;	
	CHAR					RssiOfRcvdSpoofedUnknownMgmt;	
	CHAR					RssiOfRcvdReplayAttack;		
	
} MULTISSID_STRUCT, *PMULTISSID_STRUCT;
#endif

typedef enum _NDIS_HOSTAPD_STATUS {
	Hostapd_Diable = 0,
	Hostapd_EXT,
	Hostapd_CFG
} NDIS_HOSTAPD_STATUS, *PNDIS_HOSTAPD_STATUS;


/* This structure is extracted from struct RT_HT_CAPABILITY and RT_VHT_CAP */
typedef struct _RT_PHY_INFO{
	BOOLEAN		bHtEnable;	 /* If we should use ht rate. */
	BOOLEAN		bPreNHt;	 /* If we should use ht rate. */
	/*Substract from HT Capability IE */
	UCHAR		MCSSet[16];
} RT_PHY_INFO;

enum BCN_TX_STATE{
    BCN_TX_UNINIT = 0,
	BCN_TX_IDLE = 1,
    BCN_TX_WRITE_TO_DMA = 2,
    BCN_TX_DMA_DONE = 3,
    BCN_TX_TXS_DONE = 4,
	BCN_TX_STOP = 5
};

struct hw_setting{
	UCHAR prim_ch;
	UCHAR cent_ch;
	UINT8 bbp_bw;
	UCHAR rf_band;
	UCHAR cur_ch_pwr[3];
	CHAR lan_gain;
};
#ifdef MT_MAC
typedef struct _TIM_BUF_STRUCT {
	BOOLEAN bTimSntReq;	/* used in if beacon send or stop */
	UCHAR TimBufIdx;
	PNDIS_PACKET TimPkt;
//	PNDIS_PACKET TimPkt2;
} TIM_BUF_STRUC;

#endif

typedef struct _BCN_BUF_STRUCT {
	BOOLEAN bBcnSntReq;	/* used in if beacon send or stop */
	UCHAR BcnBufIdx;
	enum BCN_TX_STATE bcn_state;	/* Make sure if no packet pending in the Hardware */
//	NDIS_SPIN_LOCK bcn_lock;

#if 0
	CHAR BeaconBuf[MAX_BEACON_SIZE];	/* NOTE: BeaconBuf should be 4-byte aligned */
#else
	P_MSDU_INFO_T BeaconPkt;
#endif
	UCHAR cap_ie_pos;
    struct wifi_dev *pWdev;/* point to associated wdev.*/

    UCHAR *TimBitmaps;//[WLAN_MAX_NUM_OF_TIM];
    UCHAR TimIELocationInBeacon;
#ifdef MT_MAC
    TIM_BUF_STRUC tim_buf;
    UCHAR TimIELocationInTim;
#endif /* MT_MAC */

    NDIS_SPIN_LOCK BcnContentLock;
    UCHAR BcnUpdateMethod;
   /* VOID (*archUpdateBeaconToAsic)(
        struct _RTMP_ADAPTER *pAd,
        struct wifi_dev *wdev,
        ULONG FrameLen,
        ULONG UpdatePos,
        UCHAR UpdatePktType);
*/
} BCN_BUF_STRUC;

enum {
	OMAC_TYPE_AP,
	OMAC_TYPE_STA,
	OMAC_TYPE_ADHOC,
	OMAC_TYPE_WDS,
	OMAC_TYPE_MESH,
	OMAC_TYPE_GO,
	OMAC_TYPE_GC,
	OMAC_TYPE_APCLI,
	OMAC_TYPE_REPEATER,
	OMAC_TYPE_P2P_DEVICE,
};

#ifndef WIFI_BUILD_RAM
typedef struct _CMD_BSSINFO_PM_T
{
    UINT_16	u2Tag;		// Tag = 0x03
    UINT_16	u2Length;
    UINT_8	ucKeepAliveEn;
    UINT_8	ucKeepAlivePeriod;		//unit is second
    UINT_8	ucBeaconLossReportEn;
    UINT_8	ucBeaconLossCount;
    UINT_8	u2BcnSpState0Min;
    UINT_8	u2BcnSpState0Max;
    UINT_8	u2BcnSpState1Min;
    UINT_8	u2BcnSpState1Max;
    UINT_8	u2BcnSpState2Min;
    UINT_8	u2BcnSpState2Max;
    UINT_8	u2BcnSpState3Min;
    UINT_8	u2BcnSpState3Max;
    UINT_8	u2BcnSpState4Min;
    UINT_8	u2BcnSpState4Max;
    UINT_16	u2Reserve;
} CMD_BSSINFO_PM_T, *P_CMD_BSSINFO_PM_T;


#endif
typedef struct _BSS_INFO_ARGUMENT_T
{
    BOOLEAN fgInitialized;
    UCHAR *pOwnMacIdx;
    UINT8 ucBssIndex;
    UINT8 *Bssid;
    UINT8 ucBcMcWlanIdx;
	UINT8 ucPeerWlanIdx;
    UINT32 NetworkType;
    UINT32 u4ConnectionType;
    UINT8 CipherSuit;
    UINT8 Active;
    UINT32 u4BssInfoFeature;
	CMD_BSSINFO_PM_T rBssInfoPm;
	HTTRANSMIT_SETTING BcTransmit;
	HTTRANSMIT_SETTING McTransmit;	
	VOID *priv;
} BSS_INFO_ARGUMENT_T, *PBSS_INFO_ARGUMENT_T;

struct _TX_BLK;
struct _RX_BLK_;
struct wifi_dev{
	PNET_DEV if_dev;
	VOID *func_dev;
	VOID *sys_handle;

	CHAR wdev_idx;	/* index refer from pAd->wdev_list[] */
	CHAR BssIdx;
	CHAR func_idx; /* index refer to func_dev which pointer to */
	UCHAR tr_tb_idx; /* index refer to BSS which this device belong */
	UCHAR wdev_type;
	UCHAR PhyMode;
	UCHAR channel;
#ifdef CONFIG_MULTI_CHANNEL
	UCHAR CentralChannel;
	UCHAR extcha;
	UCHAR bw;
#endif /* CONFIG_MULTI_CHANNEL */
	UCHAR if_addr[MAC_ADDR_LEN];
	UCHAR bssid[MAC_ADDR_LEN];
	UCHAR hw_bssid_idx;

	/* security segment */
	NDIS_802_11_AUTHENTICATION_MODE AuthMode;
	NDIS_802_11_WEP_STATUS WepStatus;
	NDIS_802_11_WEP_STATUS GroupKeyWepStatus;
	WPA_MIX_PAIR_CIPHER WpaMixPairCipher;
	UCHAR DefaultKeyId;
	UCHAR PortSecured;
#if defined(DOT1X_SUPPORT) || defined(WPA_SUPPLICANT_SUPPORT)
	BOOLEAN IEEE8021X; /* Only indicate if we are running in dynamic WEP mode (WEP+802.1x) */
#endif /* DOT1X_SUPPORT */

	/* transmit segment */
	BOOLEAN allow_data_tx;
	BOOLEAN IgmpSnoopEnable; /* Only enabled for AP/WDS mode */
	RT_PHY_INFO DesiredHtPhyInfo;
	DESIRED_TRANSMIT_SETTING DesiredTransmitSetting;	/* Desired transmit setting. this is for reading registry setting only. not useful. */
	BOOLEAN bAutoTxRateSwitch;
	HTTRANSMIT_SETTING HTPhyMode, MaxHTPhyMode, MinHTPhyMode;	/* For transmit phy setting in TXWI. */

	/* 802.11 protocol related characters */
	BOOLEAN bWmmCapable;	/* 0:disable WMM, 1:enable WMM */
	/* UAPSD information: such as enable or disable, do not remove */
	UAPSD_INFO UapsdInfo;


	/* VLAN related */
	BOOLEAN bVLAN_Tag;
	USHORT VLAN_VID;
	USHORT VLAN_Priority;

	/* operations */
	INT (*tx_pkt_allowed)(struct _RTMP_ADAPTER *pAd, struct wifi_dev *wdev, PNDIS_PACKET pPacket, UCHAR *pWcid);
	INT (*tx_pkt_handle)(struct _RTMP_ADAPTER *pAd, PNDIS_PACKET pPacket);
	NDIS_STATUS (*wdev_hard_tx)(struct _RTMP_ADAPTER *pAd, struct _TX_BLK *pTxBlk);
	INT (*rx_pkt_allowed)(struct _RTMP_ADAPTER *pAd, struct _RX_BLK_*pRxBlk);
	INT (*rx_pkt_hdr_chk)(struct _RTMP_ADAPTER *pAd, struct _RX_BLK_*pRxBlk);
	INT (*rx_ps_handle)(struct _RTMP_ADAPTER *pAd, struct _RX_BLK_*pRxBlk);
	INT (*rx_pkt_foward)(struct _RTMP_ADAPTER *pAd, struct wifi_dev *wdev, PNDIS_PACKET pPacket);

	/* last received packet's SNR for each antenna */
	UCHAR LastSNR0;
	UCHAR LastSNR1;
#ifdef DOT11N_SS3_SUPPORT
	UCHAR LastSNR2;
	INT32 BF_SNR[3];	/* Last RXWI BF SNR. Units=0.25 dB */
#endif /* DOT11N_SS3_SUPPORT */
	RSSI_SAMPLE RssiSample;
	ULONG NumOfAvgRssiSample;
	BOOLEAN bLinkUpDone;
	BOOLEAN bGotEapolPkt;
	struct _RX_BLK *pEapolPktFromAP;

	/* 7636 psm */
	USHORT Psm;		/* power management mode   (PWR_ACTIVE|PWR_SAVE), Please use this value to replace  pAd->StaCfg.Psm in the future*/
	BOOLEAN bBeaconLost;
	UINT8	ucDtimPeriod;
	UINT8	ucBeaconPeriod;
	UINT8	OmacIdx;
#if defined(IOT_CFG80211_SUPPORT) || defined(HOSTAPD_SUPPORT)
	NDIS_HOSTAPD_STATUS Hostapd;
#endif	

#ifdef CONFIG_AP_SUPPORT
	BCN_BUF_STRUC bcn_buf;
	BOOLEAN BssInfoLinked;
	BSS_INFO_ARGUMENT_T bss_info_argument;
#endif
	UINT8 BcMcWcid;

};


#define RTMP_OS_NETDEV_GET_PHYADDR(_pNetDev)	((_pNetDev)->dev_addr)

typedef struct _STA_TR_ENTRY{
	UINT32 EntryType;
	struct wifi_dev *wdev;

	UCHAR wcid;
	/*
		func_tb_idx used to indicate following index:
			in ApCfg.ApCliTab
			in pAd->MeshTab
			in WdsTab.MacTab
	*/
	// TODO:shiang-usw, need to integrate this parameter to wdev_idx or something else!!
	UCHAR func_tb_idx;
	UCHAR Addr[MAC_ADDR_LEN];
	/*
		Tx Info
	*/
	USHORT NonQosDataSeq;
	USHORT TxSeq[NUM_OF_TID];

	QUEUE_HEADER tx_queue[WMM_QUE_NUM];
	QUEUE_HEADER ps_queue;
	UINT	enqCount;
	UINT	TokenCount[WMM_QUE_NUM];
#ifdef MT_PS
	BOOLEAN bEospNullSnd;
	UCHAR EospNullUp;
#endif /* MT_PS */
	INT		ps_qbitmap;
	UCHAR		ps_state;
	UCHAR		retrieve_start_state;
	UCHAR		token_enq_all_fail;

	BOOLEAN tx_pend_for_agg[WMM_QUE_NUM];
	NDIS_SPIN_LOCK txq_lock[WMM_QUE_NUM];
	NDIS_SPIN_LOCK ps_queue_lock;
	UINT deq_cnt;
	UINT deq_bytes;
	ULONG PsQIdleCount;

	BOOLEAN enq_cap;
	BOOLEAN deq_cap;

	/*
		STA status
	*/

	UCHAR bssid[MAC_ADDR_LEN];
	BOOLEAN bIAmBadAtheros;	/* Flag if this is Atheros chip that has IOT problem.  We need to turn on RTS/CTS protection. */
	BOOLEAN isCached;
	UCHAR PortSecured;
	UCHAR PsMode;
	UCHAR FlgPsModeIsWakeForAWhile; /* wake up for a while until a condition */
#if 1 // def WDS_SUPPORT
	BOOLEAN LockEntryTx;	/* TRUE = block to WDS Entry traffic, FALSE = not. */
#endif /* WDS_SUPPORT */

#ifdef WAC_SUPPORT
#ifdef WAC_QOS_PRIORITY
	BOOLEAN bSamsungForcePriority;
#endif /* WAC_QOS_PRIORITY */
#endif /* WAC_SUPPORT */

	UCHAR CurrTxRate;

#ifdef VENDOR_FEATURE1_SUPPORT
	/* total 128B, use UINT32 to avoid alignment problem */
	UINT32 HeaderBuf[32];	/* (total 128B) TempBuffer for TX_INFO + TX_WI + 802.11 Header + padding + AMSDU SubHeader + LLC/SNAP */
	UCHAR HdrPadLen;	/* recording Header Padding Length; */
	UCHAR MpduHeaderLen;
	UCHAR wifi_hdr_len;
	UINT16 Protocol;
#endif /* VENDOR_FEATURE1_SUPPORT */

#ifdef DOT11_N_SUPPORT
	UINT32 CachedBuf[16];	/* UINT (4 bytes) for alignment */

	USHORT RXBAbitmap;	/* fill to on-chip  RXWI_BA_BITMASK in 8.1.3RX attribute entry format */
	USHORT TXBAbitmap;	/* This bitmap as originator, only keep in software used to mark AMPDU bit in TXWI */
	USHORT TXAutoBAbitmap;
	USHORT BADeclineBitmap;
	USHORT BARecWcidArray[NUM_OF_TID];	/* The mapping wcid of recipient session. if RXBAbitmap bit is masked */
	USHORT BAOriWcidArray[NUM_OF_TID];	/* The mapping wcid of originator session. if TXBAbitmap bit is masked */
	USHORT BAOriSequence[NUM_OF_TID];	/* The mapping wcid of originator session. if TXBAbitmap bit is masked */

	UCHAR MpduDensity;
	UCHAR MaxRAmpduFactor;
	UCHAR AMsduSize;
	UCHAR MmpsMode;		/* MIMO power save mode. */
#endif /* DOT11_N_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
#ifdef MAC_REPEATER_SUPPORT
	BOOLEAN bReptCli;
	BOOLEAN bReptEthCli;
	UCHAR MatchReptCliIdx;
	UCHAR ReptCliAddr[MAC_ADDR_LEN];
	ULONG ReptCliIdleCount;
#endif /* MAC_REPEATER_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */


#ifdef TXBF_SUPPORT
	UCHAR			TxSndgType;
	NDIS_SPIN_LOCK	TxSndgLock;

/* ETxBF */
	UCHAR		bfState;
	UCHAR		sndgMcs;
	UCHAR		sndgBW;
	UCHAR		sndg0Mcs;
	INT			sndg0Snr0, sndg0Snr1, sndg0Snr2;

#ifdef ETXBF_EN_COND3_SUPPORT
	UCHAR		bestMethod;
	UCHAR		sndgRateIdx;
	UCHAR		bf0Mcs, sndg0RateIdx, bf0RateIdx;
	UCHAR		sndg1Mcs, bf1Mcs, sndg1RateIdx, bf1RateIdx;
	INT			sndg1Snr0, sndg1Snr1, sndg1Snr2;
#endif /* ETXBF_EN_COND3_SUPPORT */
	UCHAR		noSndgCnt;
	UCHAR		eTxBfEnCond;
	UCHAR		noSndgCntThrd, ndpSndgStreams;
	UCHAR		iTxBfEn;
	RALINK_TIMER_STRUCT eTxBfProbeTimer;

	BOOLEAN		phyETxBf;			/* True=>Set ETxBF bit in PHY rate */
	BOOLEAN		phyITxBf;			/* True=>Set ITxBF bit in PHY rate */
	UCHAR		lastNonBfRate;		/* Last good non-BF rate */
	BOOLEAN		lastRatePhyTxBf;	/* For Quick Check. True if last rate was BF */
	USHORT      BfTxQuality[MAX_TX_RATE_INDEX + 1];	/* Beamformed TX Quality */

	COUNTER_TXBF TxBFCounters;		/* TxBF Statistics */
	UINT LastETxCount;		/* Used to compute %BF statistics */
	UINT LastITxCount;
	UINT LastTxCount;
#endif /* TXBF_SUPPORT */

#ifdef VHT_TXBF_SUPPORT
	UINT8 snd_dialog_token;
#ifdef SOFT_SOUNDING
	BOOLEAN snd_reqired;
	HTTRANSMIT_SETTING snd_rate;
#endif /* SOFT_SOUNDING */
#endif /* VHT_TXBF_SUPPORT */


	/*
		Statistics related parameters
	*/
	UINT32 ContinueTxFailCnt;
	ULONG TimeStamp_toTxRing;
	ULONG NoDataIdleCount;

#ifdef CONFIG_AP_SUPPORT
	LARGE_INTEGER TxPackets;
	LARGE_INTEGER RxPackets;
	ULONG TxBytes;
	ULONG RxBytes;
#endif /* CONFIG_AP_SUPPORT */
	INT cacheSn[NUM_OF_UP];
#ifdef MT_SDIO_ADAPTIVE_TC_RESOURCE_CTRL
#if TC_PAGE_BASED_DEMAND
	INT32 TotalPageCount[WMM_QUE_NUM];
#endif
#endif
 /*
 	Used to ignore consecutive PS poll.
 	set: when we get a PS poll.
 	clear: when a PS data is sent or two period passed.
 */
	UINT8 PsDeQWaitCnt;

#ifdef MT_MAC
	UINT8 OmacIdx;
#endif
}STA_TR_ENTRY;


#if 1//def DBG
#ifdef MT_MAC
#define MAX_TIME_RECORD 5
#endif
#endif

#if 1
typedef struct _BSS_STRUCT {
	struct wifi_dev wdev;

	INT mbss_idx;

#ifdef HOSTAPD_SUPPORT
	NDIS_HOSTAPD_STATUS Hostapd;
	BOOLEAN HostapdWPS;
#endif

	CHAR Ssid[MAX_LEN_OF_SSID];
	UCHAR SsidLen;
	BOOLEAN bHideSsid;

	USHORT CapabilityInfo;

	UCHAR MaxStaNum;	/* Limit the STA connection number per BSS */
	UCHAR StaCount;
	UINT16 StationKeepAliveTime;	/* unit: second */

	/*
		Security segment
	*/
	UCHAR RSNIE_Len[2];
	UCHAR RSN_IE[2][MAX_LEN_OF_RSNIE];

	/* WPA */
	UCHAR GMK[32];
	UCHAR PMK[32];
	UCHAR GTK[32];
	UCHAR GNonce[32];
	NDIS_802_11_PRIVACY_FILTER PrivacyFilter;

	/* for Group Rekey, AP ONLY */
	RT_WPA_REKEY WPAREKEY;
	ULONG REKEYCOUNTER;
	RALINK_TIMER_STRUCT REKEYTimer;
	UCHAR REKEYTimerRunning;
	UINT8 RekeyCountDown;

	/* For PMK Cache using, AP ONLY */
	ULONG PMKCachePeriod;	/* unit : jiffies */
#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
#ifdef WPA_SUPPLICANT_SUPPORT
	NDIS_AP_802_11_PMKID PMKIDCache;
#endif/*WPA_SUPPLICANT_SUPPORT*/
#endif /* APCLI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef DOT1X_SUPPORT
	BOOLEAN PreAuth;

	/* For 802.1x daemon setting per BSS */
	UINT8 radius_srv_num;
	RADIUS_SRV_INFO radius_srv_info[MAX_RADIUS_SRV_NUM];
	UINT8 NasId[IFNAMSIZ];
	UINT8 NasIdLen;
#endif /* DOT1X_SUPPORT */

#ifdef WAPI_SUPPORT
	UCHAR WAPIPassPhrase[64];	/* WAPI PSK pass phrase */
	UINT WAPIPassPhraseLen;	/* the length of WAPI PSK pass phrase */
	UINT WapiPskType;	/* 0 - Hex, 1 - ASCII */
	UCHAR WAPI_BK[16];	/* WAPI base key */

	UCHAR NMK[LEN_WAPI_NMK];
	UCHAR key_announce_flag[LEN_WAPI_TSC];
	BOOLEAN sw_wpi_encrypt;	/* WPI data encrypt by SW */
#endif /* WAPI_SUPPORT */


	/*
		Transmitting segment
	*/
	UCHAR TxRate; /* RATE_1, RATE_2, RATE_5_5, RATE_11, ... */
	UCHAR DesiredRates[MAX_LEN_OF_SUPPORTED_RATES];	/* OID_802_11_DESIRED_RATES */
	UCHAR DesiredRatesIndex;
	UCHAR MaxTxRate; /* RATE_1, RATE_2, RATE_5_5, RATE_11 */


	/*
		Statistics segment
	*/
	/*MBSS_STATISTICS MbssStat;*/
	ULONG TxCount;
	ULONG RxCount;
	ULONG ReceivedByteCount;
	ULONG TransmittedByteCount;
	ULONG RxErrorCount;
	ULONG RxDropCount;

	ULONG TxErrorCount;
	ULONG TxDropCount;
	ULONG ucPktsTx;
	ULONG ucPktsRx;
	ULONG mcPktsTx;
	ULONG mcPktsRx;
	ULONG bcPktsTx;
	ULONG bcPktsRx;

	UCHAR BANClass3Data;
	ULONG IsolateInterStaTraffic;

#ifdef CONFIG_AP_SUPPORT
	/* outgoing BEACON frame buffer and corresponding TXWI */
	BCN_BUF_STRUC bcn_buf;
	UCHAR *TimBitmaps;//[WLAN_MAX_NUM_OF_TIM];
	UCHAR TimIELocationInBeacon;

#ifdef MT_MAC
	TIM_BUF_STRUC tim_buf;
	UCHAR TimIELocationInTim;
#endif /* MT_MAC */
#endif /* CONFIG_AP_SUPPORT */

#ifdef DOT11V_WNM_SUPPORT
	UCHAR DMSEntrycount;
	UCHAR totalDMScount;
#endif /* DOT11V_WNM_SUPPORT */
#ifdef IOT_ACL
	RT_802_11_ACL AccessControlList;
#endif
	/* EDCA QoS */
	/*BOOLEAN bWmmCapable;*/	/* 0:disable WMM, 1:enable WMM */
	BOOLEAN bDLSCapable;	/* 0:disable DLS, 1:enable DLS */
#ifdef QOS_DLS_SUPPORT
	UCHAR DlsPTK[64];	/* Due to windows dirver count on meetinghouse to handle 4-way shake */
#endif /* QOS_DLS_SUPPORT */

	/*
	   Why need the parameter: 2009/09/22

	   1. iwpriv ra0 set WmmCapable=0
	   2. iwpriv ra0 set WirelessMode=9
	   3. iwpriv ra0 set WirelessMode=0
	   4. iwpriv ra0 set SSID=SampleAP

	   After the 4 commands, WMM is still enabled.
	   So we need the parameter to recover WMM Capable flag.

	   No the problem in station mode.
	 */
	BOOLEAN bWmmCapableOrg;	/* origin Wmm Capable in non-11n mode */


	/*
`		WPS segment
	*/
#if defined (WSC_STA_SUPPORT) || defined (WSC_AP_SUPPORT)

	WSC_LV_INFO WscIEBeacon;
	WSC_LV_INFO WscIEProbeResp;
#endif
#ifdef WSC_AP_SUPPORT
	WSC_CTRL WscControl;
	WSC_SECURITY_MODE WscSecurityMode;
#endif /* WSC_AP_SUPPORT */

#ifdef IDS_SUPPORT
	UINT32 RcvdConflictSsidCount;
	UINT32 RcvdSpoofedAssocRespCount;
	UINT32 RcvdSpoofedReassocRespCount;
	UINT32 RcvdSpoofedProbeRespCount;
	UINT32 RcvdSpoofedBeaconCount;
	UINT32 RcvdSpoofedDisassocCount;
	UINT32 RcvdSpoofedAuthCount;
	UINT32 RcvdSpoofedDeauthCount;
	UINT32 RcvdSpoofedUnknownMgmtCount;
	UINT32 RcvdReplayAttackCount;

	CHAR RssiOfRcvdConflictSsid;
	CHAR RssiOfRcvdSpoofedAssocResp;
	CHAR RssiOfRcvdSpoofedReassocResp;
	CHAR RssiOfRcvdSpoofedProbeResp;
	CHAR RssiOfRcvdSpoofedBeacon;
	CHAR RssiOfRcvdSpoofedDisassoc;
	CHAR RssiOfRcvdSpoofedAuth;
	CHAR RssiOfRcvdSpoofedDeauth;
	CHAR RssiOfRcvdSpoofedUnknownMgmt;
	CHAR RssiOfRcvdReplayAttack;
#endif /* IDS_SUPPORT */

#ifdef DOT11R_FT_SUPPORT
	FT_CFG FtCfg;
#endif /* DOT11R_FT_SUPPORT */

#ifdef DOT11V_WNM_SUPPORT
	WNM_CONFIG WnmCfg;
#endif /* DOT11V_WNM_SUPPORT */

#ifdef DOT11K_RRM_SUPPORT
	RRM_CONFIG RrmCfg;
#endif /* DOT11K_RRM_SUPPORT */

#ifdef DOT11W_PMF_SUPPORT
	PMF_CFG PmfCfg;
#endif /* DOT11W_PMF_SUPPORT */

#ifdef EASY_CONFIG_SETUP
	EASY_CONFIG_INFO EasyConfigInfo;
#endif /* EASY_CONFIG_SETUP */

	/* YF@20120417: Avoid connecting to AP in Poor Env, value 0 fOr disable. */
	CHAR AssocReqRssiThreshold;
	CHAR RssiLowForStaKickOut;

#ifdef CONFIG_DOT11U_INTERWORKING
	GAS_CTRL GASCtrl;
#endif

#ifdef CONFIG_HOTSPOT
	HOTSPOT_CTRL HotSpotCtrl;
#endif

#ifdef CONFIG_DOT11V_WNM
	WNM_CTRL WNMCtrl;
#endif

#ifdef SPECIFIC_TX_POWER_SUPPORT
	CHAR TxPwrAdj;
#endif /* SPECIFIC_TX_POWER_SUPPORT */

#ifdef IOT_CFG80211_SUPPORT
    /* Extra IEs for (Re)Association Response provided by wpa_supplicant. E.g, WPS & P2P & WFD...etc */
    UCHAR AssocRespExtraIe[512];
    UINT32 AssocRespExtraIeLen;
#endif /* IOT_CFG80211_SUPPORT */

#if 1//def DBG
#ifdef MT_MAC
    ULONG WriteBcnDoneTime[MAX_TIME_RECORD];
    ULONG BcnDmaDoneTime[MAX_TIME_RECORD];
    UCHAR bcn_not_idle_time;
    UINT32 bcn_recovery_num;
    ULONG TXS_TSF[MAX_TIME_RECORD];
    ULONG TXS_SN[MAX_TIME_RECORD];
    UCHAR timer_loop;
#endif /* MT_MAC */
#endif
} BSS_STRUCT;
#endif

#if 0
typedef struct _MAC_TABLE_ENTRY {
	//Choose 1 from ValidAsWDS and ValidAsCLI  to validize.
	
	UINT32 EntryType;
	BOOLEAN         ValidAsCLI;		// Sta mode, set this TRUE after Linkup,too.
	BOOLEAN         ValidAsWDS;	// This is WDS Entry. only for AP mode.
	BOOLEAN         ValidAsApCli;   //This is a AP-Client entry, only for AP mode which enable AP-Client functions.
	BOOLEAN         bIAmBadAtheros;	// Flag if this is Atheros chip that has IOT problem.  We need to turn on RTS/CTS protection.

	UCHAR           apidx;			// MBSS number 
#ifdef IAPP_SUPPORT
	INT				IAPPidx;
#endif // IAPP_SUPPORT

	NDIS_802_11_WEP_STATUS              WepStatus;

	BOOLEAN         EnqueueStartForPSKTimerRunning;  // Enqueue EAPoL-Start for triggering PSK SM
	//jan for wpa
	// record which entry revoke MIC Failure , if it leaves the BSS itself, AP won't update aMICFailTime MIB
	UCHAR           CMTimerRunning;
	PVOID           pAd;

	UCHAR           RSNIE_Len;
	UCHAR           RSN_IE[MAX_LEN_OF_RSNIE];
	UCHAR           ANonce[LEN_KEY_DESC_NONCE];
	UCHAR           R_Counter[LEN_KEY_DESC_REPLAY];
	UCHAR           PTK[64];
	UCHAR           ReTryCounter;   
	RALINK_TIMER_STRUCT                 RetryTimer;
	RALINK_TIMER_STRUCT					EnqueueStartForPSKTimer;	// A timer which enqueue EAPoL-Start for triggering PSK SM
	NDIS_802_11_AUTHENTICATION_MODE     AuthMode;   // This should match to whatever microsoft defined
	AP_WPA_STATE    WpaState;
	GTK_STATE       GTKState;
	CIPHER_KEY      PairwiseKey;
	INT				PMKID_CacheIdx;
	UCHAR			PMKID[LEN_PMKID];
	NDIS_802_11_PRIVACY_FILTER  PrivacyFilter;      // PrivacyFilter enum for 802.1X
	AUTH_STATE      AuthState; // for SHARED KEY authentication state machine used only

	USHORT          CapabilityInfo;	
	UINT16			StationKeepAliveCount; // unit: second	
	SST             Sst;
	USHORT          PortSecured;

	UCHAR           Addr[MAC_ADDR_LEN];
	USHORT          Aid;
	UCHAR           LastRssi;
	ULONG           NoDataIdleCount;

	UCHAR           PsMode;
	ULONG           PsQIdleCount;
	QUEUE_HEADER    PsQueue;

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_AP_SUPPORT
    /* 0:AC_BK, 1:AC_BE, 2:AC_VI, 3:AC_VO */
    BOOLEAN			bAPSDCapablePerAC[4];
    UCHAR			MaxSPLength;

    BOOLEAN         bAPSDAllAC; /* 1: all AC are U-APSD */

    QUEUE_HEADER    UAPSDQueue[WMM_NUM_OF_AC]; /* queue for each U-APSD */
    USHORT          UAPSDQIdleCount;           /* U-APSD queue timeout */

    PQUEUE_ENTRY    pUAPSDEOSPFrame;    /* the last U-APSD frame */
    USHORT          UAPSDTxNum;         /* total U-APSD frame number */
    BOOLEAN         bAPSDFlagEOSPOK;    /* 1: EOSP is tx by ASIC */
    BOOLEAN         bAPSDFlagSPStart;   /* 1: SP is started */
#endif // UAPSD_AP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //


	BOOLEAN			bSendBAR;
	USHORT			NoBADataCountDown;

	BOOLEAN			isCached;
	UINT32   		CachedBuf[16];		// UINT (4 bytes) for alignment
	UINT			FIFOCount;
	UINT			DebugFIFOCount;
	UINT			DebugTxCount;
    BOOLEAN			bDlsInit;


//====================================================
//WDS entry needs these
// rt2860 add this. if ValidAsWDS==TRUE, MatchWDSTabIdx is the index in WdsTab.MacTab
	UINT			MatchWDSTabIdx;
	UCHAR           MaxSupportedRate;
	UCHAR           CurrTxRate;
	UCHAR           CurrTxRateIndex;
	// to record the each TX rate's quality. 0 is best, the bigger the worse.
	USHORT          TxQuality[MAX_STEP_OF_TX_RATE_SWITCH];
//	USHORT          OneSecTxOkCount;
	UINT32			OneSecTxNoRetryOkCount;
	UINT32          OneSecTxRetryOkCount;
	UINT32          OneSecTxFailCount;
	UINT32          CurrTxRateStableTime; // # of second in current TX rate
	UCHAR           TxRateUpPenalty;      // extra # of second penalty due to last unstable condition
//====================================================

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
	UINT			MatchAPCLITabIdx; // indicate the index in ApCfg.ApCliTab. 
#endif // APCLI_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

	BOOLEAN         fNoisyEnvironment;
	BOOLEAN			fLastSecAccordingRSSI;
	UCHAR           LastSecTxRateChangeAction; // 0: no change, 1:rate UP, 2:rate down
	CHAR			LastTimeTxRateChangeAction; //Keep last time value of LastSecTxRateChangeAction	
	ULONG			LastTxOkCount;
	UCHAR           PER[MAX_STEP_OF_TX_RATE_SWITCH];

	// a bitmap of BOOLEAN flags. each bit represent an operation status of a particular 
	// BOOLEAN control, either ON or OFF. These flags should always be accessed via
	// CLIENT_STATUS_TEST_FLAG(), CLIENT_STATUS_SET_FLAG(), CLIENT_STATUS_CLEAR_FLAG() macros.
	// see fOP_STATUS_xxx in RTMP_DEF.C for detail bit definition. fCLIENT_STATUS_AMSDU_INUSED
	ULONG           ClientStatusFlags;
	// HT EWC MIMO-N used parameters
	USHORT		RXBAbitmap;	// fill to on-chip  RXWI_BA_BITMASK in 8.1.3RX attribute entry format
	USHORT		TXAutoBAbitmap;
	USHORT		BARecWcidArray[NUM_OF_TID];	// The mapping wcid of recipient session. if RXBAbitmap bit is masked
	USHORT		BAOriSequence[NUM_OF_TID]; // The mapping wcid of originator session. if TXBAbitmap bit is masked
	USHORT		TXBAbitmap;	// This bitmap as originator, only keep in software used to mark AMPDU bit in TXWI 
	USHORT		BADeclineBitmap;
	USHORT		BAOriWcidArray[NUM_OF_TID]; // The mapping wcid of originator session. if TXBAbitmap bit is masked

	HTTRANSMIT_SETTING	HTPhyMode, MaxHTPhyMode, MinHTPhyMode;// For transmit phy setting in TXWI.
	// 802.11n features.
	UCHAR		MpduDensity;
	UCHAR		MaxRAmpduFactor;

	UCHAR		AMsduSize;
	UCHAR		MmpsMode;	// MIMO power save more. 
//	PID_COUNTER		Stafifoaccu[16];	//fifo stack can be record per {WCID , PID}
	UCHAR       RateLen;
	HT_CAPABILITY_IE		HTCapability;

#if 1 //def DOT11N_DRAFT3
	UCHAR		BSS2040CoexistenceMgmtSupport;
	BOOLEAN		bForty_Mhz_Intolerant;
#endif // DOT11N_DRAFT3 //
	
	struct _MAC_TABLE_ENTRY *pNext;
	USHORT      TxSeq[NUM_OF_TID];
	USHORT		NonQosDataSeq;

	RSSI_SAMPLE	RssiSample;

#ifdef CONFIG_AP_SUPPORT
#ifdef WSC_AP_SUPPORT
    BOOLEAN                 bWscCapable;
    BOOLEAN                 EnqueueEapolStartTimerForWscRunning; 
    RALINK_TIMER_STRUCT     EnqueueEapolStartTimerForWsc;
    UCHAR                   Receive_EapolStart_EapRspId;
#endif // WSC_AP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

#if 0
	UINT32			TXMCSExpected[16];
	UINT32			TXMCSSuccessful[16];
	UINT32			TXMCSFailed[16];
	UINT32			TXMCSAutoFallBack[16][16];
#endif
#ifdef CONFIG_STA_SUPPORT
	ULONG   		LastBeaconRxTime;
#endif // CONFIG_STA_SUPPORT //

} MAC_TABLE_ENTRY, *PMAC_TABLE_ENTRY;

#else


#define LEN_OUI_SUITE				4

typedef struct _MAC_TABLE_ENTRY {
	BOOLEAN         ValidAsCLI;		// Sta mode, set this TRUE after Linkup,too.
	BOOLEAN         ValidAsWDS;	// This is WDS Entry. only for AP mode.
	BOOLEAN         ValidAsApCli;   //This is a AP-Client entry, only for AP mode which enable AP-Client functions.

	UINT32 EntryType;
	struct wifi_dev *wdev;
	PVOID pAd;
	struct _MAC_TABLE_ENTRY *pNext;

	USHORT      TxSeq[NUM_OF_TID]; //raghav : moved to TR_ENTRy

	/*
		A bitmap of BOOLEAN flags. each bit represent an operation status of a particular
		BOOLEAN control, either ON or OFF. These flags should always be accessed via
		CLIENT_STATUS_TEST_FLAG(), CLIENT_STATUS_SET_FLAG(), CLIENT_STATUS_CLEAR_FLAG() macros.
		see fOP_STATUS_xxx in RTMP_DEF.C for detail bit definition. fCLIENT_STATUS_AMSDU_INUSED
	*/
	ULONG ClientStatusFlags;
	ULONG cli_cap_flags;

	HTTRANSMIT_SETTING HTPhyMode, MaxHTPhyMode;	/* For transmit phy setting in TXWI. */
	HTTRANSMIT_SETTING MinHTPhyMode;

/*
	wcid:

	tr_tb_idx:

	func_tb_idx used to indicate following index:
		in ApCfg.ApCliTab
		in pAd->MeshTab
		in WdsTab.MacTab

	apidx: should remove this
*/
	UCHAR wcid;
	UCHAR tr_tb_idx;
	UCHAR func_tb_idx;
	UCHAR apidx;		/* MBSS number */

	BOOLEAN isRalink;
	BOOLEAN bIAmBadAtheros;	/* Flag if this is Atheros chip that has IOT problem.  We need to turn on RTS/CTS protection. */

	UCHAR Addr[MAC_ADDR_LEN];
#ifdef CONFIG_AP_SUPPORT
	BSS_STRUCT *pMbss;
#endif /* CONFIG_AP_SUPPORT */

	/*
		STATE MACHINE Status
	*/
	USHORT Aid;	/* in range 1~2007, with bit 14~15 = b'11, e.g., 0xc001~0xc7d7 */
	SST Sst;
	
	UCHAR PortSecured;  //raghav : moved to TR_ENTRy
	AUTH_STATE AuthState;	/* for SHARED KEY authentication state machine used only */

#ifdef P2P_SUPPORT
	UINT32 P2PEntryType; /* 0:Invalid, 1:P2P_GO, 2:P2P_CLI */
	P2P_ENTRY_PARM P2pInfo;
	BOOLEAN bP2pClient;
	UCHAR DeviceName[P2P_DEVICE_NAME_LEN];
#endif /* P2P_SUPPORT */

	/* Rx status related parameters */
	RSSI_SAMPLE RssiSample;
	UINT32 LastTxRate;
	UINT32 LastRxRate;
	SHORT freqOffset;		/* Last RXWI FOFFSET */
	SHORT freqOffsetValid;	/* Set when freqOffset field has been updated */


#ifdef WAPI_SUPPORT
	UCHAR usk_id;		/* unicast key index for WPI */
#endif /* WAPI_SUPPORT */

	/* WPA/WPA2 4-way database */
	UCHAR EnqueueEapolStartTimerRunning;	/* Enqueue EAPoL-Start for triggering EAP SM */
	RALINK_TIMER_STRUCT EnqueueStartForPSKTimer;	/* A timer which enqueue EAPoL-Start for triggering PSK SM */


	/* record which entry revoke MIC Failure , if it leaves the BSS itself, AP won't update aMICFailTime MIB */
	UCHAR CMTimerRunning;
	UCHAR RSNIE_Len;
	UCHAR RSN_IE[MAX_LEN_OF_RSNIE];
	UCHAR ANonce[LEN_KEY_DESC_NONCE];
	UCHAR SNonce[LEN_KEY_DESC_NONCE];
	UCHAR R_Counter[LEN_KEY_DESC_REPLAY];
	UCHAR PTK[64];
	UCHAR ReTryCounter;
	RALINK_TIMER_STRUCT RetryTimer;
	NDIS_802_11_AUTHENTICATION_MODE AuthMode;	/* This should match to whatever microsoft defined */
	NDIS_802_11_WEP_STATUS WepStatus;
	NDIS_802_11_WEP_STATUS GroupKeyWepStatus;
	UINT8 WpaState;
	UINT8 GTKState;
	NDIS_802_11_PRIVACY_FILTER PrivacyFilter;	/* PrivacyFilter enum for 802.1X */
	CIPHER_KEY PairwiseKey;
	INT PMKID_CacheIdx;
	UCHAR PMKID[LEN_PMKID];
	UCHAR NegotiatedAKM[LEN_OUI_SUITE];	/* It indicate the negotiated AKM suite */

	UCHAR bssid[MAC_ADDR_LEN];
	BOOLEAN IsReassocSta;	/* Indicate whether this is a reassociation procedure */
	ULONG NoDataIdleCount;
	ULONG AssocDeadLine;
	UINT16 StationKeepAliveCount;	/* unit: second */
	USHORT CapabilityInfo;
	UCHAR PsMode;
	UCHAR FlgPsModeIsWakeForAWhile; /* wake up for a while until a condition */
	UCHAR VirtualTimeout; /* peer power save virtual timeout */



#ifdef CONFIG_AP_SUPPORT
#ifdef MAC_REPEATER_SUPPORT
	BOOLEAN bReptCli;
	BOOLEAN bReptEthCli;
	UCHAR MatchReptCliIdx;
	UCHAR ReptCliAddr[MAC_ADDR_LEN];
	ULONG ReptCliIdleCount;
#endif /* MAC_REPEATER_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	UINT32 StaConnectTime;	/* the live time of this station since associated with AP */
	UINT32 StaIdleTimeout;	/* idle timeout per entry */

#ifdef UAPSD_SUPPORT
	/* these UAPSD states are used on the fly */
	/* 0:AC_BK, 1:AC_BE, 2:AC_VI, 3:AC_VO */
	BOOLEAN bAPSDCapablePerAC[4];	/* for trigger-enabled */
	BOOLEAN bAPSDDeliverEnabledPerAC[4];	/* for delivery-enabled */


	UCHAR MaxSPLength;

	BOOLEAN bAPSDAllAC;	/* 1: all AC are delivery-enabled U-APSD */

	QUEUE_HEADER UAPSDQueue[WMM_NUM_OF_AC];	/* queue for each U-APSD */
	USHORT UAPSDQIdleCount;	/* U-APSD queue timeout */

	PQUEUE_ENTRY pUAPSDEOSPFrame;	/* the last U-APSD frame */
	USHORT UAPSDTxNum;	/* total U-APSD frame number */
	BOOLEAN bAPSDFlagEOSPOK;	/* 1: EOSP frame is tx by ASIC */
	BOOLEAN bAPSDFlagSPStart;	/* 1: SP is started */

	/* need to use unsigned long, because time parameters in OS is defined as
	   unsigned long */
	unsigned long UAPSDTimeStampLast;	/* unit: 1000000/OS_HZ */
	BOOLEAN bAPSDFlagSpRoughUse;	/* 1: use rough SP (default: accurate) */

	/* we will set the flag when PS-poll frame is received and
	   clear it when statistics handle.
	   if the flag is set when PS-poll frame is received then calling
	   statistics handler to clear it. */
	BOOLEAN bAPSDFlagLegacySent;	/* 1: Legacy PS sent but yet statistics handle */

#endif /* UAPSD_SUPPORT */

#ifdef STREAM_MODE_SUPPORT
	UINT32 StreamModeMACReg;	/* MAC reg used to control stream mode for this client. 0=>No stream mode */
#endif /* STREAM_MODE_SUPPORT */

	UINT FIFOCount;
	UINT DebugFIFOCount;
	UINT DebugTxCount;

#ifdef DOT11_N_SUPPORT
	USHORT NoBADataCountDown;
#endif /* DOT11_N_SUPPORT */

ULONG		 PsQIdleCount;
QUEUE_HEADER PsQueue;

/* ==================================================== */
	enum RATE_ADAPT_ALG rateAlg;
	// TODO: shiang-usw, use following parameters to replace "RateLen/MaxSupportedRate"
#if 0
	UINT8 rate_cap_cck;	/* 1/2/5.5/11 */
	UINT8 rate_cap_ofdm;	/* 6/9/12/18/24/36/48/54 */
#ifdef DOT11_N_SUPPORT
	UINT8 rate_cap_ht[5];	/* MCS0~7, 8~15, 16~23, 24~31, 32 */
#endif /* DOT11_N_SUPPORT */
#else
	UCHAR RateLen;
	UCHAR MaxSupportedRate;
#endif

	BOOLEAN bAutoTxRateSwitch;
	UCHAR CurrTxRate;
	UCHAR CurrTxRateIndex;
	UCHAR lastRateIdx;
	UCHAR *pTable;	/* Pointer to this entry's Tx Rate Table */

#ifdef NEW_RATE_ADAPT_SUPPORT
	UCHAR lowTrafficCount;
	UCHAR fewPktsCnt;
	BOOLEAN perThrdAdj;
	UCHAR mcsGroup;/* the mcs group to be tried */
#endif /* NEW_RATE_ADAPT_SUPPORT */

#ifdef AGS_SUPPORT
	AGS_CONTROL AGSCtrl;	/* AGS control */
#endif /* AGS_SUPPORT */

	/* to record the each TX rate's quality. 0 is best, the bigger the worse. */
	USHORT TxQuality[MAX_TX_RATE_INDEX + 1];
	BOOLEAN fLastSecAccordingRSSI;
	UCHAR LastSecTxRateChangeAction;	/* 0: no change, 1:rate UP, 2:rate down */
	CHAR LastTimeTxRateChangeAction;	/* Keep last time value of LastSecTxRateChangeAction */
	ULONG LastTxOkCount; /* TxSuccess count in last Rate Adaptation interval */
	UCHAR LastTxPER;	/* Tx PER in last Rate Adaptation interval */
	UCHAR PER[MAX_TX_RATE_INDEX + 1];
	UINT32 CurrTxRateStableTime;	/* # of second in current TX rate */
	UCHAR TxRateUpPenalty;	/* extra # of second penalty due to last unstable condition */

	UCHAR SupportRateMode; /* 1: CCK 2:OFDM 4: HT, 8:VHT */
	UINT8 SupportCCKMCS;
	UINT8 SupportOFDMMCS;
#ifdef DOT11_N_SUPPORT
	UINT32 SupportHTMCS;
#endif /* DOT11_N_SUPPORT */

#ifdef PEER_DELBA_TX_ADAPT
	BOOLEAN bPeerDelBaTxAdaptEn;
#endif /* PEER_DELBA_TX_ADAPT */



	UINT32 OneSecTxNoRetryOkCount;
	UINT32 OneSecTxRetryOkCount;
	UINT32 OneSecTxFailCount;
	UINT32 OneSecRxLGICount;		/* unicast-to-me Long GI count */
	UINT32 OneSecRxSGICount;      	/* unicast-to-me Short GI count */

#ifdef FIFO_EXT_SUPPORT
	UINT32 fifoTxSucCnt;
	UINT32 fifoTxRtyCnt;
#endif /* FIFO_EXT_SUPPORT */

	UINT32 ContinueTxFailCnt;
	ULONG TimeStamp_toTxRing;

/*==================================================== */
	EXT_CAP_INFO_ELEMENT ext_cap;

#ifdef DOT11_N_SUPPORT
	HT_CAPABILITY_IE HTCapability;

	USHORT RXBAbitmap;	/* fill to on-chip  RXWI_BA_BITMASK in 8.1.3RX attribute entry format */
	USHORT TXBAbitmap;	/* This bitmap as originator, only keep in software used to mark AMPDU bit in TXWI */
	USHORT TXAutoBAbitmap;
	USHORT BADeclineBitmap;
	USHORT BARecWcidArray[NUM_OF_TID];	/* The mapping wcid of recipient session. if RXBAbitmap bit is masked */
	USHORT BAOriWcidArray[NUM_OF_TID];	/* The mapping wcid of originator session. if TXBAbitmap bit is masked */
	USHORT BAOriSequence[NUM_OF_TID];	/* The mapping wcid of originator session. if TXBAbitmap bit is masked */

	UCHAR MpduDensity;
	UCHAR MaxRAmpduFactor;
	UCHAR AMsduSize;
	UCHAR MmpsMode;		/* MIMO power save mode. */

#ifdef DOT11N_DRAFT3
	UCHAR BSS2040CoexistenceMgmtSupport;
	BOOLEAN bForty_Mhz_Intolerant;
#endif /* DOT11N_DRAFT3 */

#endif /* DOT11_N_SUPPORT */


	BOOLEAN bWscCapable;
	UCHAR Receive_EapolStart_EapRspId;
#ifdef EASY_CONFIG_SETUP
	BOOLEAN bRaAutoWpsCapable;
#endif /* EASY_CONFIG_SETUP */
#ifdef WAC_SUPPORT
	BOOLEAN bSamsungAutoWpsCapable;
#endif /* WAC_SUPPORT */

	UINT32 TXMCSExpected[MAX_MCS_SET];
	UINT32 TXMCSSuccessful[MAX_MCS_SET];
	UINT32 TXMCSFailed[MAX_MCS_SET];
	UINT32 TXMCSAutoFallBack[MAX_MCS_SET][MAX_MCS_SET];

#ifdef CONFIG_STA_SUPPORT
	ULONG LastBeaconRxTime;
#endif /* CONFIG_STA_SUPPORT */

#ifdef WAPI_SUPPORT
	BOOLEAN WapiUskRekeyTimerRunning;
	RALINK_TIMER_STRUCT WapiUskRekeyTimer;
	UINT32 wapi_usk_rekey_cnt;
#endif /* WAPI_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
	LARGE_INTEGER TxPackets;
	LARGE_INTEGER RxPackets;
	ULONG TxBytes;
	ULONG RxBytes;
#endif /* CONFIG_AP_SUPPORT */



	ULONG ChannelQuality;	/* 0..100, Channel Quality Indication for Roaming */
#ifdef MT_PS
	UCHAR i_psm; /* 0: disable, 1: enable */
#endif /* MT_PS */
#if 0// defined(NEW_RATE_ADAPT_SUPPORT) || defined(RATE_ADAPT_AGBS_SUPPORT)

	RA_ENTRY_INFO_T RaEntry;
	RA_INTERNAL_INFO_T	RaInternal;
#endif
	UINT32	ConnectionType;
	UCHAR	ucMaxTxRetryCnt;
#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
		UINT			MatchAPCLITabIdx; // indicate the index in ApCfg.ApCliTab. 
#endif // APCLI_SUPPORT //
#endif // CONFIG_AP_SUPPORT //	
} MAC_TABLE_ENTRY, *PMAC_TABLE_ENTRY;

/*For KRACK*/
typedef struct _KRACK_PREVENT_T
{
    BOOLEAN AllowInsPTK;
    UCHAR   LastGroupKeyId;
    UCHAR   LastGTK[64];
} KRACK_PREVENT_T;



#endif

#if 0
typedef struct _APCLI_STRUCT {
	PNET_DEV				dev;
#ifdef RTL865X_SOC
	unsigned int            mylinkid;
#endif
	BOOLEAN                 Enable;	// Set it as 1 if the apcli interface was configured to "1"  or by iwpriv cmd "ApCliEnable"
	BOOLEAN                 Valid;	// Set it as 1 if the apcli interface associated success to remote AP.
	UCHAR					MacTabWCID;	//WCID value, which point to the entry of ASIC Mac table.
	UCHAR                   SsidLen;
	CHAR                    Ssid[MAX_LEN_OF_SSID];

	UCHAR                   CfgSsidLen;
	CHAR                    CfgSsid[MAX_LEN_OF_SSID];
	UCHAR                   CfgApCliBssid[ETH_LENGTH_OF_ADDRESS];
	UCHAR                   CurrentAddress[ETH_LENGTH_OF_ADDRESS];

	ULONG                   ApCliRcvBeaconTime;

	ULONG                   CtrlCurrState;
	ULONG                   SyncCurrState;
	ULONG                   AuthCurrState;
	ULONG                   AssocCurrState;
	ULONG					WpaPskCurrState;

	USHORT                  AuthReqCnt;
	USHORT                  AssocReqCnt;

	ULONG                   ClientStatusFlags;
	UCHAR                   MpduDensity;

	NDIS_802_11_AUTHENTICATION_MODE     AuthMode;   // This should match to whatever microsoft defined
	NDIS_802_11_WEP_STATUS              WepStatus;

	// Add to support different cipher suite for WPA2/WPA mode
	NDIS_802_11_ENCRYPTION_STATUS		GroupCipher;		// Multicast cipher suite
	NDIS_802_11_ENCRYPTION_STATUS		PairCipher;			// Unicast cipher suite
	BOOLEAN								bMixCipher;			// Indicate current Pair & Group use different cipher suites
	USHORT								RsnCapability;
	
	UCHAR		PSK[100];				// reserve PSK key material
	UCHAR       PSKLen;
	UCHAR       PMK[32];                // WPA PSK mode PMK
	//UCHAR       PTK[64];                // WPA PSK mode PTK
	UCHAR		GTK[32];				// GTK from authenticator

	//CIPHER_KEY		PairwiseKey;
	CIPHER_KEY      SharedKey[SHARE_KEY_NUM];
	UCHAR           DefaultKeyId;

	// WPA 802.1x port control, WPA_802_1X_PORT_SECURED, WPA_802_1X_PORT_NOT_SECURED
	//UCHAR       PortSecured;
    
	// store RSN_IE built by driver
	UCHAR		RSN_IE[MAX_LEN_OF_RSNIE];  // The content saved here should be convert to little-endian format.
	UCHAR		RSNIE_Len; 

	// For WPA countermeasures
	ULONG       LastMicErrorTime;   // record last MIC error time
	//ULONG       MicErrCnt;          // Should be 0, 1, 2, then reset to zero (after disassoiciation).
	BOOLEAN                 bBlockAssoc; // Block associate attempt for 60 seconds after counter measure occurred.

	// For WPA-PSK supplicant state
	//WPA_STATE   	WpaState;           // Default is SS_NOTUSE
	//UCHAR       	ReplayCounter[8];
	//UCHAR       	ANonce[32];         // ANonce for WPA-PSK from authenticator
	UCHAR       	SNonce[32];         // SNonce for WPA-PSK
	UCHAR			GNonce[32];			// GNonce for WPA-PSK from authenticator
	
#ifdef WSC_AP_SUPPORT
	WSC_CTRL	           WscControl;
#endif // WSC_AP_SUPPORT //	
} APCLI_STRUCT, *PAPCLI_STRUCT;
#else

#ifdef MAC_REPEATER_SUPPORT
#define MAX_IGNORE_AS_REPEATER_ENTRY_NUM    32
typedef struct _MBSS_TO_CLI_LINK_MAP_T
{
    struct wifi_dev *mbss_wdev;
    struct wifi_dev *cli_link_wdev;
} MBSS_TO_CLI_LINK_MAP_T;
typedef struct _REPEATER_CLIENT_ENTRY {
	//BOOLEAN bValid;
    BOOLEAN bBlockAssoc;    /* Block associate attempt for 60 seconds after counter measure occurred. */
	BOOLEAN CliEnable;
	BOOLEAN CliValid;
	BOOLEAN bEthCli;
	UCHAR MatchApCliIdx;
	UCHAR MatchLinkIdx;
	UCHAR MacTabWCID;
	UCHAR CliConnectState; /* 0: disconnect 1: connecting 2: connected */

    USHORT AuthReqCnt;
    USHORT AssocReqCnt;
    UCHAR BandIdx;/*link on which Apcli link bandidx.*/
    UCHAR OriginalAddress[MAC_ADDR_LEN];
    UCHAR CurrentAddress[MAC_ADDR_LEN];
	RALINK_TIMER_STRUCT ApCliAssocTimer;
    RALINK_TIMER_STRUCT ApCliAuthTimer;
	RALINK_TIMER_STRUCT ReptCliResetTimer;
	PVOID pAd;
	struct _REPEATER_CLIENT_ENTRY *pNext;
    struct wifi_dev *wdev;/*pointer to the linking Apcli interface wdev. */
	ULONG CtrlCurrState;
	ULONG SyncCurrState;
	ULONG AuthCurrState;
	ULONG AssocCurrState;


	ULONG CliTriggerTime;

	/* For WPA countermeasures */
	ULONG LastMicErrorTime; /* record last MIC error time */
} REPEATER_CLIENT_ENTRY, *PREPEATER_CLIENT_ENTRY;

typedef struct _REPEATER_CLIENT_ENTRY_MAP {
	PREPEATER_CLIENT_ENTRY pReptCliEntry;
	struct _REPEATER_CLIENT_ENTRY_MAP *pNext;
} REPEATER_CLIENT_ENTRY_MAP, *PREPEATER_CLIENT_ENTRY_MAP;

typedef struct _INVAILD_TRIGGER_MAC_ENTRY {
	UCHAR MacAddr[MAC_ADDR_LEN];
	BOOLEAN bInsert;
	struct _INVAILD_TRIGGER_MAC_ENTRY *pNext;
} INVAILD_TRIGGER_MAC_ENTRY, *PINVAILD_TRIGGER_MAC_ENTRY;

typedef struct _REPEATER_CTRL_STRUCT {
	INVAILD_TRIGGER_MAC_ENTRY IgnoreAsRepeaterEntry[MAX_IGNORE_AS_REPEATER_ENTRY_NUM];
	INVAILD_TRIGGER_MAC_ENTRY **IgnoreAsRepeaterHash;//[32];
	UCHAR IgnoreAsRepeaterEntrySize;
} REPEATER_CTRL_STRUCT, *PREPEATER_CTRL_STRUCT;
#endif /* MAC_REPEATER_SUPPORT */

/***************************************************************************
  *	AP APCLI related data structures
  **************************************************************************/

#ifdef WPA_SUPPLICANT_SUPPORT

typedef struct _WPA_SUPPLICANT_INFO{
	/*
		802.1x WEP + MD5 will set key to driver before assoc, but we need to apply the key to
		ASIC after get EAPOL-Success frame, so we use this flag to indicate that
	*/
	BOOLEAN IEEE8021x_required_keys;
	CIPHER_KEY DesireSharedKey[4];	/* Record user desired WEP keys */
	UCHAR DesireSharedKeyId;

	/* 0x00: driver ignores wpa_supplicant */
	/* 0x01: wpa_supplicant initiates scanning and AP selection */
	/* 0x02: driver takes care of scanning, AP selection, and IEEE 802.11 association parameters */
	/* 0x80: wpa_supplicant trigger driver to do WPS */
	UCHAR WpaSupplicantUP;
	UCHAR WpaSupplicantScanCount;
	BOOLEAN bRSN_IE_FromWpaSupplicant;
	BOOLEAN bLostAp;
	UCHAR *pWpsProbeReqIe;
	UINT WpsProbeReqIeLen;
	UCHAR *pWpaAssocIe;
	UINT WpaAssocIeLen;
}WPA_SUPPLICANT_INFO;
#endif /* WPA_SUPPLICANT_SUPPORT */

typedef struct _APCLI_STRUCT {
	struct wifi_dev wdev;

	BOOLEAN Enable;		/* Set it as 1 if the apcli interface was configured to "1"  or by iwpriv cmd "ApCliEnable" */
	BOOLEAN Valid;		/* Set it as 1 if the apcli interface associated success to remote AP. */

	MLME_AUX MlmeAux;			/* temporary settings used during MLME state machine */

	UCHAR MacTabWCID;	/*WCID value, which point to the entry of ASIC Mac table. */
	UCHAR SsidLen;
	CHAR Ssid[MAX_LEN_OF_SSID];

	UCHAR CfgSsidLen;
	CHAR CfgSsid[MAX_LEN_OF_SSID];
	UCHAR CfgApCliBssid[MAC_ADDR_LEN];

	ULONG ApCliRcvBeaconTime;
	ULONG ApCliLinkUpTime;
	USHORT ApCliBeaconPeriod;

	ULONG CtrlCurrState;
	ULONG SyncCurrState;
	ULONG AuthCurrState;
	ULONG AssocCurrState;
	ULONG WpaPskCurrState;

	USHORT AuthReqCnt;
	USHORT AssocReqCnt;

	UCHAR MpduDensity;


	/*
		Security segment
	*/
	/* Add to support different cipher suite for WPA2/WPA mode */
	NDIS_802_11_ENCRYPTION_STATUS GroupCipher;	/* Multicast cipher suite */
	NDIS_802_11_ENCRYPTION_STATUS PairCipher;	/* Unicast cipher suite */
	BOOLEAN bMixCipher;	/* Indicate current Pair & Group use different cipher suites */
	USHORT RsnCapability;

	UCHAR PSK[100];		/* reserve PSK key material */
	UCHAR PSKLen;
	UCHAR PMK[32];		/* WPA PSK mode PMK */
	UCHAR GTK[32];		/* GTK from authenticator */

	/*CIPHER_KEY            PairwiseKey; */
	CIPHER_KEY SharedKey[SHARE_KEY_NUM];

	/* store RSN_IE built by driver */
	UCHAR RSN_IE[MAX_LEN_OF_RSNIE];	/* The content saved here should be convert to little-endian format. */
	UCHAR RSNIE_Len;

	/* For WPA countermeasures */
	ULONG LastMicErrorTime;	/* record last MIC error time */
	BOOLEAN bBlockAssoc;	/* Block associate attempt for 60 seconds after counter measure occurred. */

	/* For WPA-PSK supplicant state */
	UCHAR SNonce[32];	/* SNonce for WPA-PSK */
	UCHAR GNonce[32];	/* GNonce for WPA-PSK from authenticator */

#ifdef WPA_SUPPLICANT_SUPPORT
	WPA_SUPPLICANT_INFO wpa_supplicant_info;

	BOOLEAN	 bScanReqIsFromWebUI;
	BSSID_INFO SavedPMK[PMKID_NO];
	UINT SavedPMKNum; /* Saved PMKID number */
	BOOLEAN bConfigChanged;
	NDIS_802_11_ASSOCIATION_INFORMATION AssocInfo;
	USHORT ReqVarIELen; /* Length of next VIE include EID & Length */
	UCHAR ReqVarIEs[MAX_VIE_LEN]; /* The content saved here should be little-endian format. */
	USHORT ResVarIELen; /* Length of next VIE include EID & Length */
	UCHAR ResVarIEs[MAX_VIE_LEN];
	UCHAR LastSsidLen;               /* the actual ssid length in used */
	CHAR LastSsid[MAX_LEN_OF_SSID]; /* NOT NULL-terminated */
	UCHAR LastBssid[MAC_ADDR_LEN];
#endif /* WPA_SUPPLICANT_SUPPORT */


	/*
		WPS segment
	*/
#ifdef WSC_AP_SUPPORT
	WSC_CTRL WscControl;
#endif /* WSC_AP_SUPPORT */

	/*
		Transmitting segment
	*/
	UCHAR RxMcsSet[16];

#ifdef P2P_SUPPORT
	BOOLEAN bP2pClient;
#endif /* P2P_SUPPORT */

	PSPOLL_FRAME PsPollFrame;
	HEADER_802_11 NullFrame;

#ifdef MAC_REPEATER_SUPPORT
	REPEATER_CLIENT_ENTRY RepeaterCli[MAX_EXT_MAC_ADDR_SIZE];
	REPEATER_CLIENT_ENTRY_MAP RepeaterCliMap[MAX_EXT_MAC_ADDR_SIZE];
#endif /* MAC_REPEATER_SUPPORT */
} APCLI_STRUCT, *PAPCLI_STRUCT;

#endif // New version of APCLI



#define AMSDU_SUBHEAD_LEN	14

#define TX_FRAG_ID_NO			0x0
#define TX_FRAG_ID_FIRST		0x1
#define TX_FRAG_ID_MIDDLE		0x2
#define TX_FRAG_ID_LAST			0x3
#if defined(DATA_PATH_86) || defined(DATA_PATH_87)

struct _MSDU_INFO_T
{
    /* ----------------MSDU_INFO and SW_RFB Common Fields------------------ */
    LINK_ENTRY_T                rLinkEntry;
    PUINT_8                     pucBuffer;                  /* Pointer to the associated buffer */
    UINT_16                     u2PseFid;                   /* FID of PSE */

    UINT_16                     u2CommonReserved;

    //4 /* -----------------------Non-Common ------------------------- */
    PFN_TX_DONE_HANDLER         pfTxDoneHandler;
    PFN_TX_DONE_HANDLER         pfTxDoneHandlerPs;          /* Trigger next MSDU */
    P_STA_RECORD_T              prStaRec;
    UINT_8                      ucPID;
    UINT_8                      ucBssInfoIdx;               /* BSSINFO index */
    UINT_16                     u2PayloadLength;
    UINT_8                      ucMacHeaderLength;
    PUINT_8                     pucMacHeader;               /* 802.11 header  */
    PUINT_8                     pucPayload;                 /* 802.11 payload */

    //4 /* -----------------------TXM------------------------- */
    /* TXM: For MAC TX Descriptor */
    OS_SYSTIME                  rArrivalTime;               /* System Timestamp (4) */
    UINT_32                     u4Option;                   /* Special option in bitmask, no ACK, etc... */
    UINT_32                     u4RemainingLifetimeInMs;    /* Remaining lifetime, in ms */
    UINT_32                     u4FixedRateOption;          /* The rate option, rate code, GI, etc... */
    UINT_32                     u4TimeStamp;                /* record the TX timestamp */
    /* TXM: For MAC TX Status */
    P_HW_MAC_TX_STATUS_T        prTxStatus;
    UINT_16                     u2SwSN;                     /* SW assigned sequence number */
    UINT_16                     u2FrameCtrl;                /* Store frame control for TxDone handler */
    BOOLEAN                     fgIsDescComposed;           /* Tx descriptor is composed in the buffer head */
    UINT_8                      ucWlanIdx;                  /* WLAN table index */
    UINT_8                      ucMacTxQueIdx;              /* MAC TX queue: AC0-AC6, BCM, or BCN */

    UINT_8                      ucPacketType;               /* Packet type: MGMT, DATA */

    UINT_8                      ucTID;                      /* Traffic Identification */

    BOOLEAN                     fgIs802_11Frame;            /* Set to TRUE for 802.11 frame */
    UINT_8                      ucHeaderFormat;             /* Header Format, 802.11/native 802.11/Ethernet */


    INT_8                       cPowerOffset;               /* Per-packet power offset, in 2's complement */

    UINT_8                      ucControlFlag;              /* For specify some Control Flags */

    UINT_8                      ucRetryLimit;               /* The retry limit */

    UINT_8                      ucRateMode;                 /* Rate mode: AUTO, MANUAL_DESC, MANUAL_CR */


    //4 /* -----------------------TODO area------------------------- */
    //UINT_8                      ucBufferSource;           /* HIF TX0, HIF TX1, MAC RX, or MNG Pool */
    //BOOLEAN                     fgIsBmcQueueEnd;          /* Set to true if this packet is the end of BMC */
    //BOOLEAN                     fgMoreDataBit;
    //BOOLEAN                     fgEOSPBit;

#if TXM_PROFILE
    UINT_16                     u2ACLSeq;                   /* u2LLH+u2ACLSeq for AM HCI flush ACL frame */
#endif

#if CFG_PROFILE_BUFFER_TRACING
    ENUM_BUFFER_ACTIVITY_TYPE_T eActivity[2];
    UINT_32                     rActivityTime[2];
#endif
#if DBG && CFG_BUFFER_FREE_CHK
    BOOLEAN                     fgBufferInSource;
#endif

    //4 /* -----------------------Non-Common ------------------------- */

    /* TODO: move flags to ucControlFlag */

    /* TXM: For TX Done handling, callback function & parameter (5) */
};
#endif

#ifndef WIFI_BUILD_RAM
#ifdef IOT_WOW_SUPPORT
/* Define STA record structure */
struct _STA_RECORD_T
{
    /*------------------------------------------------------------------------------------------*/
    /* Common station record
    	  *------------------------------------------------------------------------------------------*/
    LINK_ENTRY_T            rLinkEntry;                 /* StaRec link list in the same BSS network */
    P_BSS_INFO_T            prBssInfo;                  /* BSS_INFO_T pointer */
    BOOLEAN                 fgIsInUse;                  /* Indicate if this entry is in use or not */
    UINT_8                  ucIndex;                    /* Not modify it except initializing. It should be equal to Wlan Index */
    UINT_8                  ucHwBssIndex;               /* HW BSSID/TSF index */
    UINT_8                  ucWTEntry;                  /* WLAN index */
    /*ENUM_STA_STATE_T*/UINT_32        eStaState;                  /* state of STA , MU or SU */
    /*ENUM_STA_TYPE_T*/UINT_32         ePeerStaType;               /* Indicate the role of this peer STA in
                                                         * the network (for example, P2P GO) */
    BOOLEAN                 ucIsQosLink;                /* Indicate current link is Qos link */
    UINT_8		            ucIsVHT;				    /* Indicates if the STA supports VHT */
    UINT_16                 u2AssocId;                  /* For Infra Mode, to store AID from Association Resp(SAA).
                                                         * For AP Mode, to store the Assigned AID(AAA). */
    UINT_8                  aucMacAddr[MAC_ADDR_LEN];   /* MAC address. Peer STA MAC address */
    BOOLEAN                 fgIsPseRedirected;          /* for AP and infra station PowerSanving used */


    /*------------------------------------------------------------------------------------------*/
    /* 802.11n HT and 802.11ac VHT capabilities
     	  *------------------------------------------------------------------------------------------*/
    UINT_16                 u2HtCapInfo;                /* HT cap info field by HT cap IE */
    UINT_16                 u2HtExtendedCap;            /* HT extended cap field by HT cap IE */

    /*------------------------------------------------------------------------------------------*/
    /* Infra STA mode (peer is AP)
         *------------------------------------------------------------------------------------------*/
    BOOLEAN                 fgSetPwrMgtBit;             /* For Infra Station Mode, to indicate that outgoing frame need toggle
                                                         * the Pwr Mgt Bit in its Frame Control Field. */

    /*------------------------------------------------------------------------------------------*/
    /* AP mode (peer is client)
         *------------------------------------------------------------------------------------------*/
    BOOLEAN                 fgIsInPS;                   /* For AP Mode, to indicate the client PS state(PM).
                                                         * TRUE: In PS Mode; FALSE: In Active Mode. */

    /*------------------------------------------------------------------------------------------*/
    /* MU MIMO record
         *------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------------------*/
    /* TXBF
         *------------------------------------------------------------------------------------------*/
};

struct _SW_RFB_T
{
    /* ----------------MSDU_INFO and SW_RFB Common Fields------------------ */
    LINK_ENTRY_T    rLinkEntry;
    PUINT_8         pucBuffer;          /* Pointer to the associated buffer */
    UINT_16         u2PseFid;           /* FID of PSE */
    UINT_16         u2CommonReserved;

    //4 /* -----------------------Non-Common ------------------------- */

    /* For composing the HIF RX Header (TODO: move flags to ucControlFlag) */
    UINT_8          ucHifRxPortIndex;

    // Cache the Rx status
    UINT_8                  ucRxPktType;
    UINT_8                  ucGroupVLD;
    UINT_16                 u2RxStatusOffst;
    P_HW_MAC_RX_DESC_T        prRxStatus;
    P_HW_MAC_RX_STS_GROUP_1_T prRxStatusGroup1;
    P_HW_MAC_RX_STS_GROUP_2_T prRxStatusGroup2;
    P_HW_MAC_RX_STS_GROUP_3_T prRxStatusGroup3;
    P_HW_MAC_RX_STS_GROUP_4_T prRxStatusGroup4;

    UINT_8          ucBssInfoIdx;       /* BSSInfo index */
    UINT_8          ucTC;               /* 0 to 5 (used by HIF TX to increment the corresponding TC counter) */
    UINT_8          ucTID;              /* Traffic Identification */

    BOOLEAN         fgIs802_11Frame;    /* Set to TRUE for 802.11 frame */
    UINT_8          ucMacHeaderLength;
    UINT_16         u2PayloadLength;
    PUINT_8         pucMacHeader;       /* 802.11 header  */
    PUINT_8         pucPayload;         /* 802.11 payload */

    //OS_SYSTIME      rArrivalTime;       /* System Timestamp (4) */
    P_STA_RECORD_T  prStaRec;

    UINT_16     u2SequenceControl;

    /* For Header Translation Support */
    UINT_16     u2FrameCtrl;
    UINT_32     u4HTCtrl;               /* Valid only when fgIsHTCValid = TRUE */
    UINT_16     u2QoSCtrl;
    BOOLEAN     fgIsHTCValid;

    BOOLEAN     fgIsQoSData;
    UINT_8      fgIsAmsduSubframe;  /* Set to TRUE for A-MSDU Subframe */

    /* For HIF RX DMA Desc */
    BOOLEAN     fgTUChecksumCheckRequired;
    BOOLEAN     fgIPChecksumCheckRequired;
    UINT_8      ucEtherTypeOffset;

    /* Add for New HIF RX Header */
    BOOLEAN     fgIsMMPDU;

    BOOLEAN     fgTCL;
    BOOLEAN     fgIsHeaderTran;
    UINT_8      ucRCPI;
    UINT_8      ucHwChannelNum;
};
#endif /* IOT_WOW_SUPPORT */
#endif /* WIFI_BUILD_RAM */

//	Currently the sizeof(TX_BLK) is 148 bytes.
#if 1
typedef struct _TX_BLK
{
	UCHAR				QueIdx;
	UCHAR				TxFrameType;				// Indicate the Transmission type of the all frames in one batch
	UCHAR				TotalFrameNum;				// Total frame number want to send-out in one batch
	USHORT				TotalFragNum;				// Total frame fragments required in one batch
	USHORT				TotalFrameLen;				// Total length of all frames want to send-out in one batch

	QUEUE_HEADER		TxPacketList;
	MAC_TABLE_ENTRY		*pMacEntry;					// NULL: packet with 802.11 RA field is multicast/broadcast address
	STA_TR_ENTRY 		*tr_entry;
	HTTRANSMIT_SETTING	*pTransmit;
	
	// Following structure used for the characteristics of a specific packet.
	PNDIS_PACKET		pPacket;
	PUCHAR				pSrcBufHeader;				// Reference to the head of sk_buff->data
	PUCHAR				pSrcBufData;				// Reference to the sk_buff->data, will changed depends on hanlding progresss
	UINT				SrcBufLen;					// Length of packet payload which not including Layer 2 header
	PUCHAR				pExtraLlcSnapEncap;			// NULL means no extra LLC/SNAP is required
	UCHAR				HeaderBuf[96];				//80+16:  TempBuffer for TX_DESC (7DW=28 or 8DB=32) + 802.11 Header + padding(0~3) + AMSDU SubHeader + LLC/SNAP
	
	UCHAR				*wifi_hdr;
	
	UCHAR				wifi_hdr_len;
	UCHAR				HdrPadLen;					/* recording Header Padding Length; */
	UCHAR				MpduHeaderLen;				// 802.11 header length NOT including the padding
	UCHAR				apidx;						// The interface associated to this packet 
	UCHAR				Wcid;						// The MAC entry associated to this packet
	UCHAR				UserPriority;				// priority class of packet
	UCHAR				FrameGap;					// what kind of IFS this packet use
	UCHAR				MpduReqNum;					// number of fragments of this frame
	UCHAR				TxRate;						// TODO: Obsoleted? Should change to MCS?
	UCHAR				CipherAlg;					// cipher alogrithm
	PCIPHER_KEY			pKey;
	
	UCHAR				KeyIdx;						/* Indicate the transmit key index */
	UCHAR				OpMode;
	/*UCHAR				apidx;*/						/* The interface associated to this packet */
	UCHAR				wdev_idx;				// Used to replace apidx
	UCHAR				FragIdx;					/* refer to TX_FRAG_ID_xxxx */
	UINT8				Pid;

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
	UINT				ApCliIfidx;
	PAPCLI_STRUCT		pApCliEntry;
#endif // APCLI_SUPPORT //
#endif // CONFIG_AP_SUPPORT //
	struct wifi_dev *wdev;

	USHORT				Flags;						//See following definitions for detail.

	//YOU SHOULD NOT TOUCH IT! Following parameters are used for hardware-depended layer.
	ULONG				Priv;						// Hardware specific value saved in here.
	
	BOOLEAN				NeedTrans;	/* indicate the packet needs to do hw header translate */
} TX_BLK;




typedef struct dequeue_info{
	BOOLEAN inited;
	UCHAR start_q;
	UCHAR end_q;
	CHAR cur_q;
	UCHAR target_wcid;
	UCHAR target_que;
	UCHAR cur_wcid;
	USHORT q_max_cnt[WMM_QUE_NUM];
	INT pkt_bytes;
	INT pkt_cnt;
	INT deq_pkt_bytes;
	INT deq_pkt_cnt;
	INT status;
	BOOLEAN full_qid[WMM_QUE_NUM];
#ifdef DBG_DEQUE
	deq_log_struct deq_log;
#endif /* DBG_DEQUE */
}DEQUE_INFO;


typedef struct _PEER_PROBE_REQ_PARAM {
	UCHAR Addr2[MAC_ADDR_LEN];
	CHAR Ssid[MAX_LEN_OF_SSID];
	UCHAR SsidLen;
	BOOLEAN bRequestRssi;
} PEER_PROBE_REQ_PARAM, *PPEER_PROBE_REQ_PARAM;

#ifdef CONFIG_AP_SUPPORT
typedef struct  _AUTO_CH_CTRL{
	PCHANNELINFO pChannelInfo;
	PBSSINFO pBssInfoTab;
}AUTO_CH_CTRL;
#endif

typedef struct _RX_BLK_
{
//	RXD_STRUC		RxD; // sample
//	RT28XX_RXD_STRUC	RxD;
	RXINFO_STRUC	RxInfo;
    UINT_8          ucMacHeaderLength;
    UINT_16         u2PayloadLength;
    PUINT_8         pucMacHeader;       /* 802.11 header  */
    PUINT_8         pucPayload;         /* 802.11 payload */
    UINT_16         u2RxStatusOffst;

	PRXWI_STRUC			pRxWI;
	PHEADER_802_11		pHeader;
	PNDIS_PACKET		pRxPacket;
	UCHAR				*pData;
	USHORT				DataSize;
	USHORT				Flags;
	UCHAR				UserPriority;	// for calculate TKIP MIC using
#ifdef IOT_WOW_SUPPORT
	SW_RFB_T	SwRfb;
	STA_RECORD_T StaRec;
#endif

} RX_BLK;


#endif

enum WDEV_TYPE {
	WDEV_TYPE_AP = (1 << 0),
	WDEV_TYPE_STA = (1 << 1),
	WDEV_TYPE_ADHOC = (1 << 2),
	WDEV_TYPE_WDS = (1 << 3),
	WDEV_TYPE_MESH = (1 << 4),
	WDEV_TYPE_GO = (1 << 5),
	WDEV_TYPE_GC = (1 << 6),
	WDEV_TYPE_APCLI = (1 << 7),
	WDEV_TYPE_REPEATER = (1 << 8),
	WDEV_TYPE_P2P_DEVICE = (1 << 9),
};

// configuration common to OPMODE_AP as well as OPMODE_STA
#ifdef OLD
typedef struct _COMMON_CONFIG {

	BOOLEAN		bCountryFlag;
	UCHAR		CountryCode[3];
	UCHAR		Geography;
	UCHAR       CountryRegion;      // Enum of country region, 0:FCC, 1:IC, 2:ETSI, 3:SPAIN, 4:France, 5:MKK, 6:MKK1, 7:Israel
	UCHAR       CountryRegionForABand;	// Enum of country region for A band
	UCHAR       PhyMode;            // PHY_11A, PHY_11B, PHY_11BG_MIXED, PHY_ABG_MIXED
	UCHAR cfg_wmode;
	UCHAR SavedPhyMode;
	USHORT      Dsifs;              // in units of usec
	ULONG       PacketFilter;       // Packet filter for receiving

	CHAR        Ssid[MAX_LEN_OF_SSID]; // NOT NULL-terminated
	UCHAR       SsidLen;               // the actual ssid length in used
	UCHAR       LastSsidLen;               // the actual ssid length in used
	CHAR        LastSsid[MAX_LEN_OF_SSID]; // NOT NULL-terminated
	UCHAR		LastBssid[MAC_ADDR_LEN];

	UCHAR       Bssid[MAC_ADDR_LEN];
	USHORT      BeaconPeriod;
	UCHAR       Channel;            
	UCHAR       CentralChannel;    	// Central Channel when using 40MHz is indicating. not real channel.        

	BOOLEAN		HT_DisallowTKIP;		/* Restrict the encryption type in 11n HT mode */
	UCHAR		RxAntDiversity; 		// 0:Disable, 1:Enable HW Rx Antenna Diversity, 2: Fix Main, 3: Fix Aux
	UCHAR		SwAntDiversity; 		// 0:Disable, 1: not used,                               2: Fix Main, 3: Fix Aux

#if 1 //def DOT11N_DRAFT3
	BOOLEAN					bBssCoexEnable;
	/* 
		Following two paramters now only used for the initial scan operation. the AP only do 
			bandwidth fallback when BssCoexApCnt > BssCoexApCntThr
		By default, the "BssCoexApCntThr" is set as 0 in "UserCfgInit()".
	*/
	UCHAR					BssCoexApCntThr;	
	UCHAR					BssCoexApCnt;
	
	UCHAR					Bss2040CoexistFlag;		// bit 0: bBssCoexistTimerRunning, bit 1: NeedSyncAddHtInfo.
	RALINK_TIMER_STRUCT	Bss2040CoexistTimer;
#ifdef UCOS
	UCHAR					Bss2040NeedFallBack; 	// 1: Need Fall back to 20MHz
#endif // UCOS //

	//This IE is used for 20/40 BSS Coexistence.
	BSS_2040_COEXIST_IE		BSS2040CoexistInfo;
	// ====== 11n D3.0 =======================>
	USHORT					Dot11OBssScanPassiveDwell;				// Unit : TU. 5~1000
	USHORT					Dot11OBssScanActiveDwell;				// Unit : TU. 10~1000
	USHORT					Dot11BssWidthTriggerScanInt;			// Unit : Second
	USHORT					Dot11OBssScanPassiveTotalPerChannel;	// Unit : TU. 200~10000
	USHORT					Dot11OBssScanActiveTotalPerChannel;	// Unit : TU. 20~10000
	USHORT					Dot11BssWidthChanTranDelayFactor;
	USHORT					Dot11OBssScanActivityThre;				// Unit : percentage
	
	ULONG					Dot11BssWidthChanTranDelay;			// multiple of (Dot11BssWidthTriggerScanInt * Dot11BssWidthChanTranDelayFactor)
	ULONG					CountDownCtr;	// CountDown Counter from (Dot11BssWidthTriggerScanInt * Dot11BssWidthChanTranDelayFactor)
	
	BSS_2040_COEXIST_IE		LastBSSCoexist2040;
	BSS_2040_COEXIST_IE		BSSCoexist2040;
	TRIGGER_EVENT_TAB		TriggerEventTab;
	UCHAR					ChannelListIdx;
	// <====== 11n D3.0 =======================
	BOOLEAN					bOverlapScanning;
	BOOLEAN					bBssCoexNotify;
#endif // DOT11N_DRAFT3 //

	UCHAR       SupRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR       SupRateLen;
	UCHAR       ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR       ExtRateLen;
	UCHAR       DesireRate[MAX_LEN_OF_SUPPORTED_RATES];      // OID_802_11_DESIRED_RATES
	UCHAR       MaxDesiredRate;
	UCHAR       ExpectedACKRate[MAX_LEN_OF_SUPPORTED_RATES];

	ULONG       BasicRateBitmap;        // backup basic ratebitmap

	BOOLEAN		bAPSDCapable;
	BOOLEAN		bInServicePeriod;
	BOOLEAN		bAPSDAC_BE;
	BOOLEAN		bAPSDAC_BK;
	BOOLEAN		bAPSDAC_VI;
	BOOLEAN		bAPSDAC_VO;
	BOOLEAN		bNeedSendTriggerFrame;
	BOOLEAN		bAPSDForcePowerSave;	// Force power save mode, should only use in APSD-STAUT
	ULONG		TriggerTimerCount;
	UCHAR		MaxSPLength;
	UCHAR		BBPCurrentBW;	// BW_10, 	BW_20, BW_40
	HTTRANSMIT_SETTING	HTPhyMode, MaxHTPhyMode, MinHTPhyMode;// For transmit phy setting in TXWI.
	REG_TRANSMIT_SETTING        RegTransmitSetting; //registry transmit setting. this is for reading registry setting only. not useful.
	UCHAR		FixedMCS[1 /*MAX_MBSSID_NUM*/];
	UCHAR       FixedTxMode[1/*MAX_MBSSID_NUM*/];        // Fixed Tx Mode (CCK, OFDM), for HT fixed tx mode (GF, MIX) , refer to RegTransmitSetting.field.HTMode
	UCHAR       TxRate;                 // Same value to fill in TXD. TxRate is 6-bit 
	UCHAR       MaxTxRate;              // RATE_1, RATE_2, RATE_5_5, RATE_11
	UCHAR       TxRateIndex;            // Tx rate index in RateSwitchTable
	UCHAR       TxRateTableSize;        // Valid Tx rate table size in RateSwitchTable
	//BOOLEAN		bAutoTxRateSwitch;
	UCHAR       MinTxRate;              // RATE_1, RATE_2, RATE_5_5, RATE_11
	UCHAR       RtsRate;                // RATE_xxx
	HTTRANSMIT_SETTING	MlmeTransmit;   // MGMT frame PHY rate setting when operatin at Ht rate.
	UCHAR       MlmeRate;               // RATE_xxx, used to send MLME frames
	UCHAR       BasicMlmeRate;          // Default Rate for sending MLME frames

	USHORT      RtsThreshold;           // in unit of BYTE
	USHORT      FragmentThreshold;      // in unit of BYTE

	UCHAR       TxPower;                // in unit of mW
	ULONG       TxPowerPercentage;      // 0~100 %
	ULONG       TxPowerDefault;         // keep for TxPowerPercentage

	BACAP_STRUC        BACapability; //   NO USE = 0XFF  ;  IMMED_BA =1  ;  DELAY_BA=0
	BACAP_STRUC        REGBACapability; //   NO USE = 0XFF  ;  IMMED_BA =1  ;  DELAY_BA=0
	IOT_STRUC		IOTestParm;	// 802.11n InterOpbility Test Parameter;
	ULONG       TxPreamble;             // Rt802_11PreambleLong, Rt802_11PreambleShort, Rt802_11PreambleAuto
	BOOLEAN     bUseZeroToDisableFragment;     // Microsoft use 0 as disable 
	ULONG       UseBGProtection;        // 0: auto, 1: always use, 2: always not use
	BOOLEAN     bUseShortSlotTime;      // 0: disable, 1 - use short slot (9us)
	BOOLEAN     bEnableTxBurst;         // 1: enble TX PACKET BURST, 0: disable TX PACKET BURST
	BOOLEAN     bAggregationCapable;      // 1: enable TX aggregation when the peer supports it
	BOOLEAN     bPiggyBackCapable;		// 1: enable TX piggy-back according MAC's version
	BOOLEAN     bIEEE80211H;			// 1: enable IEEE802.11h spec.
	ULONG		DisableOLBCDetect;		// 0: enable OLBC detect; 1 disable OLBC detect 

	BOOLEAN				bRdg;
	BOOLEAN             bWmmCapable;        // 0:disable WMM, 1:enable WMM
	QOS_CAPABILITY_PARM APQosCapability;    // QOS capability of the current associated AP
	EDCA_PARM           APEdcaParm;         // EDCA parameters of the current associated AP
	QBSS_LOAD_PARM      APQbssLoad;         // QBSS load of the current associated AP
	UCHAR               AckPolicy[4];       // ACK policy of the specified AC. see ACK_xxx
#ifdef CONFIG_STA_SUPPORT
	BOOLEAN				bDLSCapable;		// 0:disable DLS, 1:enable DLS
#endif // CONFIG_STA_SUPPORT //
	// a bitmap of BOOLEAN flags. each bit represent an operation status of a particular 
	// BOOLEAN control, either ON or OFF. These flags should always be accessed via
	// OPSTATUS_TEST_FLAG(), OPSTATUS_SET_FLAG(), OP_STATUS_CLEAR_FLAG() macros.
	// see fOP_STATUS_xxx in RTMP_DEF.C for detail bit definition
	ULONG               OpStatusFlags;
	ULONG 		SetOpFlagLog;  // for record changes of OpStatusFlags
	ULONG 		ClearOpFlagLog;  // for record changes of OpStatusFlags

	BOOLEAN				NdisRadioStateOff; //For HCT 12.0, set this flag to TRUE instead of called MlmeRadioOff.       
	ABGBAND_STATE		BandState;		// For setting BBP used on B/G or A mode.

#ifdef ANT_DIVERSITY_SUPPORT
	//UCHAR bRxAntDiversity;	/* 0:disable, 1:enable Software Rx Antenna Diversity. */
#endif				/* ANT_DIVERSITY_SUPPORT */

	// IEEE802.11H--DFS.
	RADAR_DETECT_STRUCT	RadarDetect;

#ifdef CONFIG_AP_SUPPORT
#ifdef CARRIER_DETECTION_SUPPORT
	CARRIER_DETECTION		CarrierDetect;
#endif // CARRIER_DETECTION_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

	// HT
	UCHAR			BASize;		// USer desired BAWindowSize. Should not exceed our max capability
	BOOLEAN ht_ldpc;
	//RT_HT_CAPABILITY	SupportedHtPhy;
	RT_HT_CAPABILITY	DesiredHtPhy;
	HT_CAPABILITY_IE		HtCapability;	
	ADD_HT_INFO_IE		AddHTInfo;	// Useful as AP.
	//This IE is used with channel switch announcement element when changing to a new 40MHz.
	//This IE is included in channel switch ammouncement frames 7.4.1.5, beacons, probe Rsp.
	NEW_EXT_CHAN_IE	NewExtChanOffset;	//7.3.2.20A, 1 if extension channel is above the control channel, 3 if below, 0 if not present
    BOOLEAN                     bHTProtect;
    BOOLEAN                     bMIMOPSEnable;
    BOOLEAN						bBADecline;
	BOOLEAN						bDisableReordering;
	BOOLEAN						bForty_Mhz_Intolerant;
	BOOLEAN						bExtChannelSwitchAnnouncement;
	BOOLEAN						bRcvBSSWidthTriggerEvents;
	ULONG						LastRcvBSSWidthTriggerEventsTime;

	// Enable wireless event
	BOOLEAN				bWirelessEvent;		
	UCHAR				TxBASize;
	BOOLEAN				bWiFiTest;				// Enable this parameter for WiFi test		

	// Tx & Rx Stream number selection
	UCHAR				TxStream;
	UCHAR				RxStream;

	// transmit phy mode, trasmit rate for Multicast.
#ifdef MCAST_RATE_SPECIFIC
	UCHAR				McastTransmitMcs;
	UCHAR				McastTransmitPhyMode;
#endif // MCAST_RATE_SPECIFIC //

	BOOLEAN     bHardwareRadio;     // Hardware controlled Radio enabled
#ifdef RT2870
	BOOLEAN     		bMultipleIRP;       // Multiple Bulk IN flag
	UCHAR       		NumOfBulkInIRP;     // if bMultipleIRP == TRUE, NumOfBulkInIRP will be 4 otherwise be 1
 	RT_HT_CAPABILITY	SupportedHtPhy;
	ULONG						MaxPktOneTxBulk;
	UCHAR						TxBulkFactor;
	UCHAR						RxBulkFactor;
#endif // RT2870 //
	BOOLEAN		bRadioOff;
	UCHAR	VCORecalibrationThreshold;
	
#ifdef PS_ENHANCE
	BOOLEAN		bEPS_FeatureFlag;			/* the flag used to disgnose if the PS_ENHANCE is enabled */
	BOOLEAN		bEPS_TriggerFlag;			/* the flag used to disgnose if the throughput need to be trigger up/down */
	ULONG       EPS_TimeNow; 			
	
	
	ULONG		EPS_TriggerAliveThreshold;	/* if the traffic over than this threshold, trigger up, disable sleep, in PS mode */
	ULONG		EPS_TriggerAsleepThreshold;	/* if the traffic less than this threshold, trigger down, enable sleep, in PS mode */
	
	/* for average throughput caculate */
	ULONG		EPS_rxbyets;				/* the rx traffic count */
	ULONG		EPS_txbyets;				/* the tx traffic count */

	ULONG		EPS_last_rxbytes;			/* the last measure rx traffic count */
	ULONG		EPS_last_txbytes;			/* the last measure tx traffic count */
		
	/* for trend caculate */
	ULONG		EPS_rx_trend;				/* used to monitor the traffic trand for rx */
	ULONG		EPS_tx_trend;				/* used to monitor the traffic trand for tx */
	ULONG		EPS_rxtx_trend;

	BOOLEAN		EPS_UserCfg;				/* used to record the setting from profile and iwpriv(Set_PSMode_Proc) */

	RALINK_TIMER_STRUCT   EPS_PeriodTimer;

	USHORT		eps_shift_threshold_bit;	/* for caculate the average throughput in the interval */
	USHORT		eps_shift_traffic_bit;		/* for caculate the average throughput in the interval */
	ULONG		eps_timer_counter;			/* the counter for measure the eps timer */
#endif // PS_ENHANCE  // 		
	BOOLEAN bStopReadTemperature; /* avoid race condition between FW/driver */
	BOOLEAN bTXRX_RXV_ON;
	BOOLEAN ManualTxop;
	ULONG ManualTxopThreshold;
	UCHAR ManualTxopUpBound;
	UCHAR ManualTxopLowBound;

	UINT8 HT_Disable;
} COMMON_CONFIG, *PCOMMON_CONFIG;

#else


/* configuration common to OPMODE_AP as well as OPMODE_STA */
typedef struct _COMMON_CONFIG {
	BOOLEAN bCountryFlag;
	UCHAR CountryCode[4];
	UCHAR Geography;
#ifdef EXT_BUILD_CHANNEL_LIST

	UCHAR DfsType;
	PUCHAR pChDesp;
#endif /* EXT_BUILD_CHANNEL_LIST */
	UCHAR CountryRegion;	/* Enum of country region, 0:FCC, 1:IC, 2:ETSI, 3:SPAIN, 4:France, 5:MKK, 6:MKK1, 7:Israel */
	UCHAR CountryRegionForABand;	/* Enum of country region for A band */
	UCHAR PhyMode;
	UCHAR cfg_wmode;
	UCHAR SavedPhyMode;
	USHORT Dsifs;		/* in units of usec */
	ULONG PacketFilter;	/* Packet filter for receiving */
#ifdef DOT11K_RRM_SUPPORT
	UINT8 RegulatoryClass[MAX_NUM_OF_REGULATORY_CLASS];
#endif
	CHAR Ssid[MAX_LEN_OF_SSID];	/* NOT NULL-terminated */
	UCHAR SsidLen;		/* the actual ssid length in used */
	UCHAR LastSsidLen;	/* the actual ssid length in used */
	CHAR LastSsid[MAX_LEN_OF_SSID];	/* NOT NULL-terminated */
	UCHAR LastBssid[MAC_ADDR_LEN];

	UCHAR Bssid[MAC_ADDR_LEN];
	USHORT BeaconPeriod;
	UCHAR Channel;
	UCHAR CentralChannel;	/* Central Channel when using 40MHz is indicating. not real channel. */

	UCHAR SupRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR SupRateLen;
	UCHAR ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR ExtRateLen;
	UCHAR DesireRate[MAX_LEN_OF_SUPPORTED_RATES];	/* OID_802_11_DESIRED_RATES */
	UCHAR MaxDesiredRate;
	UCHAR ExpectedACKRate[MAX_LEN_OF_SUPPORTED_RATES];

	ULONG BasicRateBitmap;	/* backup basic ratebitmap */
	ULONG BasicRateBitmapOld;	/* backup basic ratebitmap */

	BOOLEAN bInServicePeriod;

#if 0 /* mbss uapsd */
	BOOLEAN bAPSDCapable;
#endif /* 0 */

	BOOLEAN bAPSDAC_BE;
	BOOLEAN bAPSDAC_BK;
	BOOLEAN bAPSDAC_VI;
	BOOLEAN bAPSDAC_VO;


	/* because TSPEC can modify the APSD flag, we need to keep the APSD flag
	   requested in association stage from the station;
	   we need to recover the APSD flag after the TSPEC is deleted. */
	BOOLEAN bACMAPSDBackup[4];	/* for delivery-enabled & trigger-enabled both */
	BOOLEAN bACMAPSDTr[4];	/* no use */
	UCHAR MaxSPLength;

	BOOLEAN bNeedSendTriggerFrame;
	BOOLEAN bAPSDForcePowerSave;	/* Force power save mode, should only use in APSD-STAUT */
	ULONG TriggerTimerCount;
	UCHAR BBPCurrentBW;	/* BW_10, BW_20, BW_40, BW_80 */
	REG_TRANSMIT_SETTING RegTransmitSetting;	/*registry transmit setting. this is for reading registry setting only. not useful. */
	UCHAR TxRate;		/* Same value to fill in TXD. TxRate is 6-bit */
	UCHAR MaxTxRate;	/* RATE_1, RATE_2, RATE_5_5, RATE_11 */
	UCHAR TxRateIndex;	/* Tx rate index in Rate Switch Table */
	UCHAR MinTxRate;	/* RATE_1, RATE_2, RATE_5_5, RATE_11 */
	UCHAR RtsRate;		/* RATE_xxx */
	HTTRANSMIT_SETTING MlmeTransmit;	/* MGMT frame PHY rate setting when operatin at Ht rate. */
	UCHAR MlmeRate;		/* RATE_xxx, used to send MLME frames */
	UCHAR BasicMlmeRate;	/* Default Rate for sending MLME frames */

    UINT32 RtsPktThreshold; /* pkt number */
	UINT32 RtsThreshold;	/* in unit of BYTE */
	USHORT FragmentThreshold;	/* in unit of BYTE */

	UCHAR TxPower;		/* in unit of mW */
	ULONG TxPowerPercentage;	/* 0~100 % */
	ULONG TxPowerDefault;	/* keep for TxPowerPercentage */
	UINT8 PwrConstraint;

#ifdef DOT11_N_SUPPORT
	BACAP_STRUC BACapability;	/*   NO USE = 0XFF  ;  IMMED_BA =1  ;  DELAY_BA=0 */
	BACAP_STRUC REGBACapability;	/*   NO USE = 0XFF  ;  IMMED_BA =1  ;  DELAY_BA=0 */
#endif /* DOT11_N_SUPPORT */

	BOOLEAN dbdc_mode;


	IOT_STRUC IOTestParm;	/* 802.11n InterOpbility Test Parameter; */
	ULONG TxPreamble;	/* Rt802_11PreambleLong, Rt802_11PreambleShort, Rt802_11PreambleAuto */
	BOOLEAN bUseZeroToDisableFragment;	/* Microsoft use 0 as disable */
	ULONG UseBGProtection;	/* 0: auto, 1: always use, 2: always not use */
	BOOLEAN bUseShortSlotTime;	/* 0: disable, 1 - use short slot (9us) */
	BOOLEAN bEnableTxBurst;	/* 1: enble TX PACKET BURST (when BA is established or AP is not a legacy WMM AP), 0: disable TX PACKET BURST */
	BOOLEAN bAggregationCapable;	/* 1: enable TX aggregation when the peer supports it */
	BOOLEAN bPiggyBackCapable;	/* 1: enable TX piggy-back according MAC's version */
	BOOLEAN bIEEE80211H;	/* 1: enable IEEE802.11h spec. */
	UCHAR RDDurRegion; /* Region of radar detection */
	ULONG DisableOLBCDetect;	/* 0: enable OLBC detect; 1 disable OLBC detect */

#ifdef DOT11_N_SUPPORT
	BOOLEAN bRdg;
#endif /* DOT11_N_SUPPORT */
	BOOLEAN bWmmCapable;	/* 0:disable WMM, 1:enable WMM */
	QOS_CAPABILITY_PARM APQosCapability;	/* QOS capability of the current associated AP */
	EDCA_PARM APEdcaParm;	/* EDCA parameters of the current associated AP */
	QBSS_LOAD_PARM APQbssLoad;	/* QBSS load of the current associated AP */
	UCHAR AckPolicy[4];	/* ACK policy of the specified AC. see ACK_xxx */
#ifdef CONFIG_STA_SUPPORT
	BOOLEAN bDLSCapable;	/* 0:disable DLS, 1:enable DLS */
#endif /* CONFIG_STA_SUPPORT */
	/* a bitmap of BOOLEAN flags. each bit represent an operation status of a particular */
	/* BOOLEAN control, either ON or OFF. These flags should always be accessed via */
	/* OPSTATUS_TEST_FLAG(), OPSTATUS_SET_FLAG(), OP_STATUS_CLEAR_FLAG() macros. */
	/* see fOP_STATUS_xxx in RTMP_DEF.C for detail bit definition */
	ULONG OpStatusFlags;

	BOOLEAN NdisRadioStateOff;	/*For HCT 12.0, set this flag to TRUE instead of called MlmeRadioOff. */
	ABGBAND_STATE BandState;        /* For setting BBP used on B/G or A mode. */


#ifdef DFS_SUPPORT
	/* IEEE802.11H--DFS. */
	RADAR_DETECT_STRUCT RadarDetect;
#endif /* DFS_SUPPORT */

#ifdef MT_DFS_SUPPORT
        DFS_PARAM DfsParameter;//Jelly20150322  
#endif

#ifdef DOT11_N_SUPPORT
	/* HT */
	RT_HT_CAPABILITY DesiredHtPhy;
	HT_CAPABILITY_IE HtCapability;
	ADD_HT_INFO_IE AddHTInfo;	/* Useful as AP. */
	/*This IE is used with channel switch announcement element when changing to a new 40MHz. */
	/*This IE is included in channel switch ammouncement frames 7.4.1.5, beacons, probe Rsp. */
	NEW_EXT_CHAN_IE NewExtChanOffset;	/*7.3.2.20A, 1 if extension channel is above the control channel, 3 if below, 0 if not present */

	EXT_CAP_INFO_ELEMENT ExtCapIE;	/* this is the extened capibility IE appreed in MGMT frames. Doesn't need to update once set in Init. */

#if 1//def DOT11N_DRAFT3
	BOOLEAN bBssCoexEnable;
	/*
	   Following two paramters now only used for the initial scan operation. the AP only do
	   bandwidth fallback when BssCoexApCnt > BssCoexApCntThr
	   By default, the "BssCoexApCntThr" is set as 0 in "UserCfgInit()".
	 */
	UCHAR BssCoexApCntThr;
	UCHAR BssCoexApCnt;

	UCHAR Bss2040CoexistFlag;	/* bit 0: bBssCoexistTimerRunning, bit 1: NeedSyncAddHtInfo. */
	RALINK_TIMER_STRUCT Bss2040CoexistTimer;
	UCHAR Bss2040NeedFallBack; 	/* 1: Need Fall back to 20MHz */

	/*This IE is used for 20/40 BSS Coexistence. */
	BSS_2040_COEXIST_IE BSS2040CoexistInfo;

	USHORT Dot11OBssScanPassiveDwell;	/* Unit : TU. 5~1000 */
	USHORT Dot11OBssScanActiveDwell;	/* Unit : TU. 10~1000 */
	USHORT Dot11BssWidthTriggerScanInt;	/* Unit : Second */
	USHORT Dot11OBssScanPassiveTotalPerChannel;	/* Unit : TU. 200~10000 */
	USHORT Dot11OBssScanActiveTotalPerChannel;	/* Unit : TU. 20~10000 */
	USHORT Dot11BssWidthChanTranDelayFactor;
	USHORT Dot11OBssScanActivityThre;	/* Unit : percentage */

	ULONG Dot11BssWidthChanTranDelay;	/* multiple of (Dot11BssWidthTriggerScanInt * Dot11BssWidthChanTranDelayFactor) */
	ULONG CountDownCtr;	/* CountDown Counter from (Dot11BssWidthTriggerScanInt * Dot11BssWidthChanTranDelayFactor) */

	BSS_2040_COEXIST_IE LastBSSCoexist2040;
	BSS_2040_COEXIST_IE BSSCoexist2040;
	TRIGGER_EVENT_TAB TriggerEventTab;
	UCHAR ChannelListIdx;

	BOOLEAN bOverlapScanning;
	BOOLEAN bBssCoexNotify;
#endif /* DOT11N_DRAFT3 */

	BOOLEAN bHTProtect;
	BOOLEAN bMIMOPSEnable;
	BOOLEAN bBADecline;
	BOOLEAN bDisableReordering;
	BOOLEAN bForty_Mhz_Intolerant;
	BOOLEAN bExtChannelSwitchAnnouncement;
	BOOLEAN bRcvBSSWidthTriggerEvents;
	ULONG LastRcvBSSWidthTriggerEventsTime;

	UCHAR TxBASize;

	BOOLEAN bRalinkBurstMode;
	UINT32 RestoreBurstMode;
	BOOLEAN ht_ldpc;
#endif /* DOT11_N_SUPPORT */


#if 1//def SYSTEM_LOG_SUPPORT
	/* Enable wireless event */
	BOOLEAN bWirelessEvent;
#endif /* SYSTEM_LOG_SUPPORT */

	BOOLEAN bWiFiTest;	/* Enable this parameter for WiFi test */

	/* Tx & Rx Stream number selection */
	UCHAR TxStream;
	UCHAR RxStream;

	/* transmit phy mode, trasmit rate for Multicast. */
#ifdef MCAST_RATE_SPECIFIC
	UCHAR McastTransmitMcs;
	UCHAR McastTransmitPhyMode;
#endif /* MCAST_RATE_SPECIFIC */

	BOOLEAN bHardwareRadio;	/* Hardware controlled Radio enabled */

#ifdef WSC_INCLUDED
	/* WSC hardware push button function 0811 */
	UINT8 WscHdrPshBtnCheckCount;
#endif /* WSC_INCLUDED */

#ifdef LATER
	NDIS_SPIN_LOCK MeasureReqTabLock;
	PMEASURE_REQ_TAB pMeasureReqTab;

	NDIS_SPIN_LOCK TpcReqTabLock;
	PTPC_REQ_TAB pTpcReqTab;
#endif
	/* transmit phy mode, trasmit rate for Multicast. */
#ifdef MCAST_RATE_SPECIFIC
	HTTRANSMIT_SETTING MCastPhyMode;
#endif /* MCAST_RATE_SPECIFIC */



#ifdef WAPI_SUPPORT
	COMMON_WAPI_INFO comm_wapi_info;

	/* rekey related parameter */
	/* USK update parameter */
	UINT8 wapi_usk_rekey_method;	/* 0:disable , 1:time, 2:packet */
	UINT32 wapi_usk_rekey_threshold;	/* the rekey threshold */

	/* MSK update parameter */
	UINT8 wapi_msk_rekey_method;	/* 0:disable , 1:time, 2:packet */
	UINT32 wapi_msk_rekey_threshold;	/* the rekey threshold */

	UINT32 wapi_msk_rekey_cnt;
	RALINK_TIMER_STRUCT WapiMskRekeyTimer;
	UCHAR WapiMskRekeyTimerRunning;
#endif /* WAPI_SUPPORT */


	BOOLEAN HT_DisallowTKIP;	/* Restrict the encryption type in 11n HT mode */

	BOOLEAN HT_Disable;	/* 1: disable HT function; 0: enable HT function */

#if 0// 1// defined(NEW_RATE_ADAPT_SUPPORT) || defined(RATE_ADAPT_AGBS_SUPPORT)
	USHORT	lowTrafficThrd;		/* Threshold for reverting to default MCS when traffic is low */
	SHORT	TrainUpRuleRSSI;	/* If TrainUpRule=2 then use Hybrid rule when RSSI < TrainUpRuleRSSI */
	USHORT	TrainUpLowThrd;		/* QuickDRS Hybrid train up low threshold */
	USHORT	TrainUpHighThrd;	/* QuickDRS Hybrid train up high threshold */
	BOOLEAN	TrainUpRule;		/* QuickDRS train up criterion: 0=>Throughput, 1=>PER, 2=> Throughput & PER */
#endif /* defined(NEW_RATE_ADAPT_SUPPORT) || defined(RATE_ADAPT_AGBS_SUPPORT) */



#ifdef DBG_CTRL_SUPPORT
	ULONG DebugFlags;	/* Temporary debug flags */
#endif /* DBG_CTRL_SUPPORT */


#ifdef WSC_INCLUDED
	BOOLEAN WscPBCOverlap;
	WSC_STA_PBC_PROBE_INFO WscStaPbcProbeInfo;
#endif /* WSC_INCLUDED */

/* TODO: need to integrate with MICROWAVE_OVEN_SUPPORT */

	BOOLEAN bStopReadTemperature; /* avoid race condition between FW/driver */
    BOOLEAN bTXRX_RXV_ON;
    BOOLEAN ManualTxop;
    ULONG ManualTxopThreshold;
    UCHAR ManualTxopUpBound;
    UCHAR ManualTxopLowBound;

} COMMON_CONFIG, *PCOMMON_CONFIG;

#endif








#ifdef CONFIG_STA_SUPPORT 
/* Modified by Wu Xi-Kun 4/21/2006 */
// STA configuration and status
#if 0 //raghav: Older version
typedef struct _STA_ADMIN_CONFIG {
	// GROUP 1 -
	//   User configuration loaded from Registry, E2PROM or OID_xxx. These settings describe
	//   the user intended configuration, but not necessary fully equal to the final 
	//   settings in ACTIVE BSS after negotiation/compromize with the BSS holder (either 
	//   AP or IBSS holder).
	//   Once initialized, user configuration can only be changed via OID_xxx
	struct wifi_dev wdev;
	UCHAR       BssType;              // BSS_INFRA or BSS_ADHOC
	USHORT      AtimWin;          // used when starting a new IBSS

	// GROUP 2 -
	//   User configuration loaded from Registry, E2PROM or OID_xxx. These settings describe
	//   the user intended configuration, and should be always applied to the final 
	//   settings in ACTIVE BSS without compromising with the BSS holder.
	//   Once initialized, user configuration can only be changed via OID_xxx
	UCHAR       RssiTrigger;
	UCHAR       RssiTriggerMode;      // RSSI_TRIGGERED_UPON_BELOW_THRESHOLD or RSSI_TRIGGERED_UPON_EXCCEED_THRESHOLD
	USHORT      DefaultListenCount;   // default listen count;
#ifndef MLME_AGENT_SUPPORT	
	ULONG       WindowsPowerMode;           // Power mode for AC power
	ULONG       WindowsBatteryPowerMode;    // Power mode for battery if exists
	BOOLEAN     bWindowsACCAMEnable;        // Enable CAM power mode when AC on
#endif // !MLME_AGENT_SUPPORT

#ifdef UCOS
	UCHAR bFreqSwitchOff;  	
	USHORT KeepScanPeriod;
#endif 

	BOOLEAN     bAutoReconnect;         // Set to TRUE when setting OID_802_11_SSID with no matching BSSID
	ULONG       WindowsPowerProfile;    // Windows power profile, for NDIS5.1 PnP

	// MIB:ieee802dot11.dot11smt(1).dot11StationConfigTable(1)
	USHORT      Psm;                  // power management mode   (PWR_ACTIVE|PWR_SAVE)
	USHORT      DisassocReason;
	UCHAR       DisassocSta[MAC_ADDR_LEN];
	USHORT      DeauthReason;
	UCHAR       DeauthSta[MAC_ADDR_LEN];
	USHORT      AuthFailReason;
	UCHAR       AuthFailSta[MAC_ADDR_LEN];

	NDIS_802_11_PRIVACY_FILTER          PrivacyFilter;  // PrivacyFilter enum for 802.1X
	NDIS_802_11_WEP_STATUS				OrigWepStatus;	// Original wep status set from OID

	// Add to support different cipher suite for WPA2/WPA mode
	NDIS_802_11_ENCRYPTION_STATUS		GroupCipher;		// Multicast cipher suite
	NDIS_802_11_ENCRYPTION_STATUS		PairCipher;			// Unicast cipher suite
	BOOLEAN								bMixCipher;			// Indicate current Pair & Group use different cipher suites
	USHORT								RsnCapability;
	
	BSSID_INFO	SavedPMK[PMKID_NO];
	UINT		SavedPMKNum;			// Saved PMKID number

	NDIS_802_11_AUTHENTICATION_MODE     AuthMode;       // This should match to whatever microsoft defined
	NDIS_802_11_WEP_STATUS              WepStatus;

	UCHAR		DefaultKeyId;			

	// WPA 802.1x port control, WPA_802_1X_PORT_SECURED, WPA_802_1X_PORT_NOT_SECURED
	UCHAR       PortSecured;

	// For WPA countermeasures
	ULONG       LastMicErrorTime;   // record last MIC error time
	ULONG       MicErrCnt;          // Should be 0, 1, 2, then reset to zero (after disassoiciation).
	BOOLEAN     bBlockAssoc;        // Block associate attempt for 60 seconds after counter measure occurred.

	// For WPA-PSK supplicant state
	WPA_STATE   WpaState;           // Default is SS_NOTUSE and handled by microsoft 802.1x
	UCHAR       ReplayCounter[8];
	UCHAR       ANonce[32];         // ANonce for WPA-PSK from aurhenticator
	UCHAR       SNonce[32];         // SNonce for WPA-PSK
    UCHAR       RSNIE_Len;
    UCHAR       RSN_IE[MAX_LEN_OF_RSNIE];	// The content saved here should be little-endian format.

	NDIS_802_11_WEP_STATUS              GroupKeyWepStatus;

    	// TODO: Only for supporting Windows/WinCE ... Michael
	//UCHAR		WpaPassPhrase[64];		// WPA PSK pass phrase		
	//UINT		WpaPassPhraseLen;		// the length of WPA PSK pass phrase    

	UCHAR		PMK[32];                // WPA PSK mode PMK
	UCHAR       PTK[64];                // WPA PSK mode PTK
	UCHAR		GTK[32];				// GTK from authenticator

	UCHAR       LastSNR0;             // last received BEACON's SNR
	UCHAR       LastSNR1;            // last received BEACON's SNR for 2nd  antenna
	RSSI_SAMPLE RssiSample;
	ULONG       NumOfAvgRssiSample;

	ULONG       LastBeaconRxTime;     // OS's timestamp of the last BEACON RX time
	ULONG       Last11bBeaconRxTime;  // OS's timestamp of the last 11B BEACON RX time
	ULONG       LastScanTime;       // Record last scan time for issue BSSID_SCAN_LIST
	ULONG       ScanCnt;            // Scan counts since most recent SSID, BSSID, SCAN OID request
	BOOLEAN     bSwRadio;           // Software controlled Radio On/Off, TRUE: On
	BOOLEAN     bHwRadio;           // Hardware controlled Radio On/Off, TRUE: On
	BOOLEAN     bRadio;             // Radio state, And of Sw & Hw radio state
	BOOLEAN     bHardwareRadio;     // Hardware controlled Radio enabled
	BOOLEAN     bShowHiddenSSID;    // Show all known SSID in SSID list get operation

    BOOLEAN		AdhocBOnlyJoined;	// Indicate Adhoc B Join.
    
	// New for WPA, windows want us to to keep association information and
	// Fixed IEs from last association response
	NDIS_802_11_ASSOCIATION_INFORMATION     AssocInfo;
	USHORT       ReqVarIELen;                // Length of next VIE include EID & Length
	UCHAR       ReqVarIEs[MAX_VIE_LEN];		// The content saved here should be little-endian format.
	USHORT       ResVarIELen;                // Length of next VIE include EID & Length
	UCHAR       ResVarIEs[MAX_VIE_LEN];

	//ULONG       AdhocMode;          // 0:WIFI mode (11b rates only), 1: b/g mixed, 2: 11g only

#ifdef CONFIG_CCX_SUPPORT
	// New variables used for CCX 1.0
	BOOLEAN             bCkipOn;
	BOOLEAN             bCkipCmicOn;
	UCHAR               CkipFlag;
	UCHAR               GIV[3];  //for CCX iv
	UCHAR               RxSEQ[4];
	UCHAR               TxSEQ[4];
	UCHAR               CKIPMIC[4];
	UCHAR               LeapAuthMode;
	LEAP_AUTH_INFO      LeapAuthInfo;   
	UCHAR               HashPwd[16];
	UCHAR               NetworkChallenge[8];
	UCHAR               NetworkChallengeResponse[24];
	UCHAR               PeerChallenge[8];
	
	UCHAR               PeerChallengeResponse[24];
	UCHAR               SessionKey[16]; //Network session keys (NSK)
	RALINK_TIMER_STRUCT LeapAuthTimer;
	ROGUEAP_TABLE       RogueApTab;   //Cisco CCX1 Rogue AP Detection

	// New control flags for CCX
	CCX_CONTROL         CCXControl;                 // Master administration state
	BOOLEAN             CCXEnable;                  // Actual CCX state
	UCHAR               CCXScanChannel;             // Selected channel for CCX beacon request
	USHORT              CCXScanTime;                // Time out to wait for beacon and probe response
	UCHAR               CCXReqType;                 // Current processing CCX request type
	BSS_TABLE           CCXBssTab;                  // BSS Table
	UCHAR               FrameReportBuf[2048];       // Buffer for creating frame report
	USHORT              FrameReportLen;             // Current Frame report length
	ULONG               CLBusyBytes;                // Save the total bytes received durning channel load scan time
	USHORT              RPIDensity[8];              // Array for RPI density collection
	// Start address of each BSS table within FrameReportBuf
	// It's important to update the RxPower of the corresponding Bss
	USHORT              BssReportOffset[MAX_LEN_OF_BSS_TABLE];
	USHORT              BeaconToken;                // Token for beacon report
	ULONG               LastBssIndex;               // Most current reported Bss index
	RM_REQUEST_ACTION   MeasurementRequest[16];     // Saved measurement request
	UCHAR               RMReqCnt;                   // Number of measurement request saved.
	UCHAR               CurrentRMReqIdx;            // Number of measurement request saved.
	BOOLEAN             ParallelReq;                // Parallel measurement, only one request performed,
													// It must be the same channel with maximum duration
	USHORT              ParallelDuration;           // Maximum duration for parallel measurement
	UCHAR               ParallelChannel;            // Only one channel with parallel measurement
	USHORT              IAPPToken;                  // IAPP dialog token
	UCHAR               CCXQosECWMin;               // Cisco QOS ECWMin for AC 0
	UCHAR               CCXQosECWMax;               // Cisco QOS ECWMax for AC 0
	// Hack for channel load and noise histogram parameters
	UCHAR               NHFactor;                   // Parameter for Noise histogram
	UCHAR               CLFactor;                   // Parameter for channel load

	UCHAR               KRK[16];        //Key Refresh Key.
	UCHAR               BTK[32];        //Base Transient Key
	BOOLEAN             CCKMLinkUpFlag;
	ULONG               CCKMRN;    //(Re)Association request number.
	LARGE_INTEGER       CCKMBeaconAtJoinTimeStamp;  //TSF timer for Re-assocaite to the new AP
	UCHAR               AironetCellPowerLimit;      //in dBm
	UCHAR               AironetIPAddress[4];        //eg. 192.168.1.1
	BOOLEAN             CCXAdjacentAPReportFlag;    //flag for determining report Assoc Lost time
	CHAR                CCXAdjacentAPSsid[MAX_LEN_OF_SSID]; //Adjacent AP's SSID report
	UCHAR               CCXAdjacentAPSsidLen;               // the actual ssid length in used    
	UCHAR               CCXAdjacentAPBssid[MAC_ADDR_LEN];         //Adjacent AP's BSSID report
	USHORT              CCXAdjacentAPChannel;
	ULONG               CCXAdjacentAPLinkDownTime;  //for Spec S32.
#endif 

	RALINK_TIMER_STRUCT	StaQuickResponeForRateUpTimer;
	BOOLEAN				StaQuickResponeForRateUpTimerRunning;
	
	UINT32		StaQuickResponseStart;
	UINT32		StaQuickResponseCounter;
	UINT32		StaQuickResponsePeriod;

	UCHAR           	DtimCount;      // 0.. DtimPeriod-1
	UCHAR           	DtimPeriod;     // default = 3

	//RT_802_11_DLS		DLSEntry[MAX_NUM_OF_DLS_ENTRY];
	////////////////////////////////////////////////////////////////////////////////////////
	// This is only for WHQL test.
	BOOLEAN				WhqlTest;
	////////////////////////////////////////////////////////////////////////////////////////
	
    RALINK_TIMER_STRUCT WpaDisassocAndBlockAssocTimer;

	BOOLEAN		        bAutoRoaming;       // 0:disable auto roaming by RSSI, 1:enable auto roaming by RSSI
	CHAR		        dBmToRoam;          // the condition to roam when receiving Rssi less than this value. It's negative value.
	
#ifdef WPA_SUPPLICANT_SUPPORT
    BOOLEAN             IEEE8021X;
    BOOLEAN             IEEE8021x_required_keys;
    CIPHER_KEY	        DesireSharedKey[4];	// Record user desired WEP keys	
    UCHAR               DesireSharedKeyId;
    
    // 0x00: driver ignores wpa_supplicant
    // 0x01: wpa_supplicant initiates scanning and AP selection
    // 0x02: driver takes care of scanning, AP selection, and IEEE 802.11 association parameters
    // 0x80: wpa_supplicant trigger driver to do WPS
    UCHAR               WpaSupplicantUP;
	UCHAR				WpaSupplicantScanCount;
	BOOLEAN				bRSN_IE_FromWpaSupplicant;
	BOOLEAN				bLostAp;
	UCHAR				*pWpsProbeReqIe;
	UINT				WpsProbeReqIeLen;
	UCHAR				*pWpaAssocIe;
	UINT				WpaAssocIeLen;
#endif // WPA_SUPPLICANT_SUPPORT //

#ifdef WSC_STA_SUPPORT
	WSC_LV_INFO			WpsIEBeacon;
   	WSC_LV_INFO			WpsIEProbeResp;
	WSC_CTRL	        WscControl;    
	// Patch of multiple crendentials: DO NOT ADD TO WscControl <== ROM code will access 
	BOOLEAN                 			WscProfileRetryTimerRunning;
	RALINK_TIMER_STRUCT		WscProfileRetryTimer;
	UINT			ApplyProfileIdx;  // add by johnli, fix WPS test plan 5.1.1
	
#endif // WSC_STA_SUPPORT //
    CHAR                dev_name[16];
    USHORT              OriDevType;

    BOOLEAN             bTGnWifiTest;
	BOOLEAN			    bScanReqIsFromWebUI;
	BOOLEAN				bNotFirstScan;
	BOOLEAN				bAutoConnectByBssid;

#ifndef MLME_AGENT_SUPPORT
    USHORT keepAliveCounter;
    USHORT keepAlivePeriod;    // period (multiple of listen interval) to issue null frame to AP for keeping alive during PSM (default 0=never send null frame)
#endif // !MLME_AGENT_SUPPORT
	HTTRANSMIT_SETTING				HTPhyMode, MaxHTPhyMode, MinHTPhyMode;// For transmit phy setting in TXWI.	
	DESIRED_TRANSMIT_SETTING       	DesiredTransmitSetting;
	RT_HT_PHY_INFO					DesiredHtPhyInfo;
	BOOLEAN							bAutoTxRateSwitch;
#if 1 //def DOT11N_DRAFT3
	UCHAR					RegClass; //IE_SUPP_REG_CLASS: 2009 PF#3: For 20/40 Intolerant Channel Report
#endif // DOT11N_DRAFT3 //
    
} STA_ADMIN_CONFIG, *PSTA_ADMIN_CONFIG;

#else

/* Modified by Wu Xi-Kun 4/21/2006 */
/* STA configuration and status */
typedef struct _STA_ADMIN_CONFIG {
	struct wifi_dev wdev;

	/*
		GROUP 1 -
		User configuration loaded from Registry, E2PROM or OID_xxx. These settings describe
		the user intended configuration, but not necessary fully equal to the final
		settings in ACTIVE BSS after negotiation/compromize with the BSS holder (either
		AP or IBSS holder).
		Once initialized, user configuration can only be changed via OID_xxx
	*/
	UCHAR BssType;		/* BSS_INFRA or BSS_ADHOC */

#ifdef MONITOR_FLAG_11N_SNIFFER_SUPPORT
#define MONITOR_FLAG_11N_SNIFFER		0x01
	UCHAR BssMonitorFlag;	/* Specific flag for monitor */
#endif /* MONITOR_FLAG_11N_SNIFFER_SUPPORT */

	USHORT AtimWin;		/* used when starting a new IBSS */

	/*
		GROUP 2 -
		User configuration loaded from Registry, E2PROM or OID_xxx. These settings describe
		the user intended configuration, and should be always applied to the final
		settings in ACTIVE BSS without compromising with the BSS holder.
		Once initialized, user configuration can only be changed via OID_xxx
	*/
	USHORT DefaultListenCount;	/* default listen count; */
	USHORT ThisTbttNumToNextWakeUp;
	ULONG WindowsPowerMode;	/* Power mode for AC power */
	ULONG WindowsBatteryPowerMode;	/* Power mode for battery if exists */
	BOOLEAN bWindowsACCAMEnable;	/* Enable CAM power mode when AC on */
	BOOLEAN bAutoReconnect;	/* Set to TRUE when setting OID_802_11_SSID with no matching BSSID */
	UCHAR RssiTrigger;
	UCHAR RssiTriggerMode;	/* RSSI_TRIGGERED_UPON_BELOW_THRESHOLD or RSSI_TRIGGERED_UPON_EXCCEED_THRESHOLD */

	ULONG WindowsPowerProfile;	/* Windows power profile, for NDIS5.1 PnP */

	BOOLEAN	 FlgPsmCanNotSleep; /* TRUE: can not switch ASIC to sleep */
	/* MIB:ieee802dot11.dot11smt(1).dot11StationConfigTable(1) */
	USHORT Psm;		/* power management mode   (PWR_ACTIVE|PWR_SAVE) */
	USHORT DisassocReason;
	UCHAR DisassocSta[MAC_ADDR_LEN];
	USHORT DeauthReason;
	UCHAR DeauthSta[MAC_ADDR_LEN];
	USHORT AuthFailReason;
	UCHAR AuthFailSta[MAC_ADDR_LEN];

	/*
		Security segment
	*/
	NDIS_802_11_PRIVACY_FILTER PrivacyFilter;	/* PrivacyFilter enum for 802.1X */
	NDIS_802_11_WEP_STATUS	OrigWepStatus;	/* Original wep status set from OID */

	/* Add to support different cipher suite for WPA2/WPA mode */
	NDIS_802_11_ENCRYPTION_STATUS GroupCipher;	/* Multicast cipher suite */
	NDIS_802_11_ENCRYPTION_STATUS PairCipher;	/* Unicast cipher suite */
	BOOLEAN bMixCipher;	/* Indicate current Pair & Group use different cipher suites */
	USHORT RsnCapability;

	UCHAR WpaPassPhrase[64];	/* WPA PSK pass phrase */
	UINT WpaPassPhraseLen;	/* the length of WPA PSK pass phrase */
	UCHAR PMK[LEN_PMK];	/* WPA PSK mode PMK */
	UCHAR PTK[LEN_PTK];	/* WPA PSK mode PTK */
	UCHAR GMK[LEN_GMK];	/* WPA PSK mode GMK */
	UCHAR GTK[MAX_LEN_GTK];	/* GTK from authenticator */
	UCHAR GNonce[32];	/* GNonce for WPA2PSK from authenticator */
	CIPHER_KEY TxGTK;
	BSSID_INFO SavedPMK[PMKID_NO];
	UINT SavedPMKNum;	/* Saved PMKID number */

	/* For WPA countermeasures */
	ULONG LastMicErrorTime;	/* record last MIC error time */
	ULONG MicErrCnt;	/* Should be 0, 1, 2, then reset to zero (after disassoiciation). */
	BOOLEAN bBlockAssoc;	/* Block associate attempt for 60 seconds after counter measure occurred. */
	/* For WPA-PSK supplicant state */
	UINT8 WpaState;		/* Default is SS_NOTUSE and handled by microsoft 802.1x */
	UCHAR ReplayCounter[8];
	UCHAR ANonce[32];	/* ANonce for WPA-PSK from aurhenticator */
	UCHAR SNonce[32];	/* SNonce for WPA-PSK */

	RSSI_SAMPLE RssiSample;
	ULONG NumOfAvgRssiSample;

	ULONG LastBeaconRxTime;	/* OS's timestamp of the last BEACON RX time */
	ULONG Last11bBeaconRxTime;  // OS's timestamp of the last 11B BEACON RX time

	ULONG LastScanTime;	/* Record last scan time for issue BSSID_SCAN_LIST */
	ULONG       ScanCnt;            // Scan counts since most recent SSID, BSSID, SCAN OID request
	BOOLEAN bNotFirstScan;	/* Sam add for ADHOC flag to do first scan when do initialization */
	BOOLEAN bSwRadio;	/* Software controlled Radio On/Off, TRUE: On */
	BOOLEAN bHwRadio;	/* Hardware controlled Radio On/Off, TRUE: On */
	BOOLEAN bRadio;		/* Radio state, And of Sw & Hw radio state */
	BOOLEAN bHardwareRadio;	/* Hardware controlled Radio enabled */
	BOOLEAN bShowHiddenSSID;	/* Show all known SSID in SSID list get operation */
	BOOLEAN AdhocBOnlyJoined;	 // Indicate Adhoc B Join.

	/* New for WPA, windows want us to to keep association information and */
	/* Fixed IEs from last association response */
	NDIS_802_11_ASSOCIATION_INFORMATION AssocInfo;
	USHORT ReqVarIELen;	/* Length of next VIE include EID & Length */
	UCHAR ReqVarIEs[MAX_VIE_LEN];	/* The content saved here should be little-endian format. */
	USHORT ResVarIELen;	/* Length of next VIE include EID & Length */
	UCHAR ResVarIEs[MAX_VIE_LEN];

	UCHAR RSNIE_Len;
	UCHAR RSN_IE[MAX_LEN_OF_RSNIE];	/* The content saved here should be little-endian format. */

	//ULONG CLBusyBytes;	/* Save the total bytes received durning channel load scan time */
	USHORT RPIDensity[8];	/* Array for RPI density collection */

	UCHAR RMReqCnt;		/* Number of measurement request saved. */
	UCHAR CurrentRMReqIdx;	/* Number of measurement request saved. */
	BOOLEAN ParallelReq;	/* Parallel measurement, only one request performed, */
	/* It must be the same channel with maximum duration */
	USHORT ParallelDuration;	/* Maximum duration for parallel measurement */
	UCHAR ParallelChannel;	/* Only one channel with parallel measurement */
	USHORT IAPPToken;	/* IAPP dialog token */
	/* Hack for channel load and noise histogram parameters */
	UCHAR NHFactor;		/* Parameter for Noise histogram */
	UCHAR CLFactor;		/* Parameter for channel load */

	RALINK_TIMER_STRUCT StaQuickResponeForRateUpTimer;
	BOOLEAN StaQuickResponeForRateUpTimerRunning;

	UCHAR DtimCount;	/* 0.. DtimPeriod-1 */
	UCHAR DtimPeriod;	/* default = 3 */

#ifdef CONFIG_AP_SUPPORT
	BCN_BUF_STRUC bcn_buf;
#endif


	RALINK_TIMER_STRUCT WpaDisassocAndBlockAssocTimer;
	/* Fast Roaming */
	BOOLEAN bAutoRoaming;	/* 0:disable auto roaming by RSSI, 1:enable auto roaming by RSSI */
	CHAR dBmToRoam;		/* the condition to roam when receiving Rssi less than this value. It's negative value. */

#ifdef WPA_SUPPLICANT_SUPPORT
	WPA_SUPPLICANT_INFO wpa_supplicant_info;
#endif /* WPA_SUPPLICANT_SUPPORT */

#ifdef WSC_STA_SUPPORT//STA_EASY_CONFIG_SETUP //raghav
	BOOLEAN bScanReqIsFromWebUI;
#endif /* STA_EASY_CONFIG_SETUP */

#ifdef WSC_STA_SUPPORT
	WSC_LV_INFO WpsIEBeacon;
	WSC_LV_INFO WpsIEProbeResp;
	WSC_CTRL WscControl;
	BOOLEAN 				WscProfileRetryTimerRunning;
	RALINK_TIMER_STRUCT 	WscProfileRetryTimer;
	UINT			ApplyProfileIdx;  // add by johnli, fix WPS test plan 5.1.1
#ifdef DPA_T
	BOOLEAN bPriorityCtrl;
	INT	WscPbcExtraScanCount;
	INT	WscPbcExtraRemainScanCount;
#endif /* DPA_T */
#ifdef IWSC_SUPPORT
	IWSC_INFO IWscInfo;
#endif /* IWSC_SUPPORT */
#endif /* WSC_STA_SUPPORT */
	CHAR dev_name[16];
	USHORT OriDevType;

	BOOLEAN bTGnWifiTest;
	BOOLEAN bSkipAutoScanConn;


#ifdef EXT_BUILD_CHANNEL_LIST
	UCHAR IEEE80211dClientMode;
	UCHAR StaOriCountryCode[3];
	UCHAR StaOriGeography;
#endif /* EXT_BUILD_CHANNEL_LIST */



#ifdef DOT11W_PMF_SUPPORT
	PMF_CFG PmfCfg;
#endif /* DOT11W_PMF_SUPPORT */

	BOOLEAN bAutoConnectByBssid;
	ULONG BeaconLostTime;	/* seconds */
	BOOLEAN bForceTxBurst;	/* 1: force enble TX PACKET BURST, 0: disable */
	BOOLEAN bAutoConnectIfNoSSID;
#ifdef DOT11_N_SUPPORT
#if 1 //def DOT11N_DRAFT3
	UCHAR RegClass;		/*IE_SUPP_REG_CLASS: 2009 PF#3: For 20/40 Intolerant Channel Report */
#endif /* DOT11N_DRAFT3 */
	BOOLEAN bAdhocN;
#endif /* DOT11_N_SUPPORT */
	BOOLEAN bAdhocCreator;	/*TRUE indicates divice is Creator. */

	/*
	   Enhancement Scanning Mechanism
	   To decrease the possibility of ping loss
	 */
	BOOLEAN bImprovedScan;
	BOOLEAN BssNr;
	UCHAR ScanChannelCnt;	/* 0 at the beginning of scan, stop at 7 */
	UCHAR LastScanChannel;
	/************************************/

	BOOLEAN bFastConnect;

/*connectinfo  for tmp store connect info from UI*/
	BOOLEAN Connectinfoflag;
	UCHAR   ConnectinfoBssid[MAC_ADDR_LEN];
	UCHAR   ConnectinfoChannel;
	UCHAR   ConnectinfoSsidLen;
	CHAR    ConnectinfoSsid[MAX_LEN_OF_SSID];
	UCHAR ConnectinfoBssType;
	PVOID pAssociatedAPEntry;

#ifndef MLME_AGENT_SUPPORT
    USHORT keepAliveCounter;
    USHORT keepAlivePeriod;    // period (multiple of listen interval) to issue null frame to AP for keeping alive during PSM (default 0=never send null frame)
#endif // !MLME_AGENT_SUPPORT

#ifdef EASY_CONFIG_SETUP
	EASY_CONFIG_INFO EasyConfigInfo;
#endif /* EASY_CONFIG_SETUP */


#ifdef CONFIG_DOT11U_INTERWORKING
	GAS_CTRL GASCtrl;
#endif

#ifdef CONFIG_HOTSPOT
	HOTSPOT_CTRL HotSpotCtrl;
#endif
} STA_ADMIN_CONFIG, *PSTA_ADMIN_CONFIG;


#endif //new version

// This data structure keep the current active BSS/IBSS's configuration that this STA
// had agreed upon joining the network. Which means these parameters are usually decided
// by the BSS/IBSS creator instead of user configuration. Data in this data structurre 
// is valid only when either ADHOC_ON(pAd) or INFRA_ON(pAd) is TRUE.
// Normally, after SCAN or failed roaming attempts, we need to recover back to
// the current active settings.
typedef struct _STA_ACTIVE_CONFIG {
	USHORT      Aid;
	USHORT      AtimWin;                // in kusec; IBSS parameter set element
	USHORT      CapabilityInfo;
	USHORT      CfpMaxDuration;
	USHORT      CfpPeriod;

	// Copy supported rate from desired AP's beacon. We are trying to match
	// AP's supported and extended rate settings.
	UCHAR       SupRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR       ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR       SupRateLen;
	UCHAR       ExtRateLen;
	// Copy supported ht from desired AP's beacon. We are trying to match
	RT_HT_CAPABILITY	SupportedHtPhy;
} STA_ACTIVE_CONFIG, *PSTA_ACTIVE_CONFIG;
#endif // CONFIG_STA_SUPPORT //

// ----------- start of AP --------------------------
// AUTH-RSP State Machine Aux data structure
typedef struct _AP_MLME_AUX {
	UCHAR               Addr[MAC_ADDR_LEN];
	USHORT              Alg;
	CHAR                Challenge[CIPHER_TEXT_LEN];
} AP_MLME_AUX, *PAP_MLME_AUX;

#if 0
typedef struct _MAC_TABLE {
	USHORT			Size;
	MAC_TABLE_ENTRY *Hash[HASH_TABLE_SIZE];
	MAC_TABLE_ENTRY Content[MAX_LEN_OF_MAC_TABLE];
	
	STA_TR_ENTRY tr_entry[MAX_LEN_OF_TR_TABLE];
	QUEUE_HEADER    McastPsQueue;
	ULONG           PsQIdleCount;
	BOOLEAN         fAnyStationInPsm;   
	//BOOLEAN         fAnyStationIsLegacy;	// Check if I use legacy rate to transmit to my BSS Station/
	//BOOLEAN         fAnyStationNonGF;		// Check if any Station can't support GF.
	//BOOLEAN         fAnyStation20Only;		// Check if any Station can't support GF.
	BOOLEAN         fAnyStationBadAtheros;	// Check if any Station is atheros 802.11n Chip.  We need to use RTS/CTS with Atheros 802,.11n chip. 
	BOOLEAN			fAnyStationMIMOPSDynamic; // Check if any Station is MIMO Dynamic
	BOOLEAN			fAnyTxOPForceDisable;	// Check if it is necessary to disable BE TxOP
	BOOLEAN         fAnyBASession;   // Check if there is BA session.  Force turn on RTS/CTS 
#ifdef IAPP_SUPPORT
	BOOLEAN			fAnyStaIAPPRoaming;
#endif // IAPP_SUPPORT //
} MAC_TABLE, *PMAC_TABLE;
#else


typedef enum _MAC_ENT_STATUS_{
	/* fAnyStationInPsm */
	MAC_TB_ANY_PSM = 0x1,
	/*
		fAnyStationBadAtheros
		Check if any Station is atheros 802.11n Chip.  We need to use RTS/CTS with Atheros 802,.11n chip.
	*/
	MAC_TB_ANY_ATH = 0x2,
	/*
		fAnyTxOPForceDisable
		Check if it is necessary to disable BE TxOP
	*/
	MAC_TB_FORCE_TxOP = 0x4,
	/*
		fAllStationAsRalink
		Check if all stations are ralink-chipset
	*/
	MAC_TB_ALL_RALINK = 0x8,
	/*
		fAnyStationIsLegacy
		Check if I use legacy rate to transmit to my BSS Station
	*/
	MAC_TB_ANY_LEGACY = 0x10,
	/*
		fAnyStationNonGF
		Check if any Station can't support GF
	*/
	MAC_TB_ANY_NON_GF = 0x20,
	/* fAnyStation20Only */
	MAC_TB_ANY_HT20 = 0x40,
	/*
		fAnyStationMIMOPSDynamic
		Check if any Station is MIMO Dynamic
	*/
	MAC_TB_ANY_MIMO_DYNAMIC = 0x80,
	/*
		fAnyBASession
		Check if there is BA session.  Force turn on RTS/CTS
	*/
	MAC_TB_ANY_BA = 0x100,
	/* fAnyStaFortyIntolerant */
	MAC_TB_ANY_40_INTOlERANT = 0x200,
	/*
		fAllStationGainGoodMCS
		Check if all stations more than MCS threshold
	*/
	MAC_TB_ALL_GOOD_MCS = 0x400,
	/*
		fAnyStationIsHT
		Check if still has any station set the Intolerant bit on!
	*/
	MAC_TB_ANY_HT = 0x800,
	/* fAnyWapiStation */
	MAC_TB_ANY_WAPI = 0x1000,
}MAC_ENT_STATUS;

typedef struct _MAC_TABLE {
	MAC_TABLE_ENTRY **Hash;//[HASH_TABLE_SIZE];
	MAC_TABLE_ENTRY *Content;//[MAX_LEN_OF_MAC_TABLE];
	UINT16 Size;
	STA_TR_ENTRY *tr_entry;//[MAX_LEN_OF_TR_TABLE];
	QUEUE_HEADER McastPsQueue;
	ULONG PsQIdleCount;
	MAC_ENT_STATUS sta_status;

	BOOLEAN fAnyStationInPsm;
	BOOLEAN fAnyStationBadAtheros;	/* Check if any Station is atheros 802.11n Chip.  We need to use RTS/CTS with Atheros 802,.11n chip. */
	BOOLEAN fAnyTxOPForceDisable;	/* Check if it is necessary to disable BE TxOP */
	BOOLEAN fAllStationAsRalink;	/* Check if all stations are ralink-chipset */
	BOOLEAN fCurrentStaBw40;        /* Check if only one STA w/ BW40 */
#ifdef DOT11_N_SUPPORT
	BOOLEAN fAnyStationIsLegacy;	/* Check if I use legacy rate to transmit to my BSS Station/ */
	BOOLEAN fAnyStationNonGF;	/* Check if any Station can't support GF. */
	BOOLEAN fAnyStation20Only;	/* Check if any Station can't support GF. */
	BOOLEAN fAnyStationMIMOPSDynamic;	/* Check if any Station is MIMO Dynamic */
	BOOLEAN fAnyBASession;	/* Check if there is BA session.  Force turn on RTS/CTS */
	BOOLEAN fAnyStaFortyIntolerant;	/* Check if still has any station set the Intolerant bit on! */
	BOOLEAN fAllStationGainGoodMCS; /* Check if all stations more than MCS threshold */

#ifdef CONFIG_AP_SUPPORT
	BOOLEAN fAnyStationIsHT;	/* Check if there is 11n STA.  Force turn off AP MIMO PS */
#endif /* CONFIG_AP_SUPPORT */
#endif /* DOT11_N_SUPPORT */

#ifdef WAPI_SUPPORT
	BOOLEAN fAnyWapiStation;
#endif /* WAPI_SUPPORT */

	USHORT MsduLifeTime; /* life time for PS packet */
} MAC_TABLE, *PMAC_TABLE;
#endif	/* #if 0 */

#ifdef IOT_PROMISCUS_MODE
#define MONITOR_MODE_OFF  0
#define MONITOR_MODE_REGULAR_RX  1
#define MONITOR_MODE_FULL 2

typedef struct _MONITOR_STRUCT
{
	//struct wifi_dev wdev;
	INT CurrentMonitorMode;
	BOOLEAN	bMonitorInitiated;
	BOOLEAN bMonitorOn;
}MONITOR_STRUCT;
#endif /*IOT_PROMISCUS_MODE */

#if 1 //def DOT11N_DRAFT3
typedef enum _BSS2040COEXIST_FLAG{
	BSS_2040_COEXIST_DISABLE = 0,
	BSS_2040_COEXIST_TIMER_FIRED  = 1,
	BSS_2040_COEXIST_INFO_SYNC = 2,
	BSS_2040_COEXIST_INFO_NOTIFY = 4,
}BSS2040COEXIST_FLAG;

typedef struct _BssCoexChRange_{
	UCHAR primaryCh;
	UCHAR secondaryCh;
	UCHAR effectChStart;
	UCHAR effectChEnd;
}BSS_COEX_CH_RANGE;
#endif // DOT11N_DRAFT3 //


#define IS_HT_STA(_pMacEntry)	\
	(1 /*_pMacEntry->MaxHTPhyMode.field.MODE >= MODE_HTMIX */)

#define IS_HT_RATE(_pMacEntry)	\
	(1 /*_pMacEntry->HTPhyMode.field.MODE >= MODE_HTMIX*/)

#define PEER_IS_HT_RATE(_pMacEntry)	\
	(_pMacEntry->HTPhyMode.field.MODE >= MODE_HTMIX)

typedef struct _WDS_ENTRY {
	BOOLEAN         Valid;
	UCHAR           Addr[MAC_ADDR_LEN];
	ULONG           NoDataIdleCount;
	struct _WDS_ENTRY *pNext;
} WDS_ENTRY, *PWDS_ENTRY;
#ifdef LATER
typedef struct  _WDS_TABLE_ENTRY {
	USHORT			Size;
	UCHAR           WdsAddr[MAC_ADDR_LEN];
	WDS_ENTRY       *Hash[HASH_TABLE_SIZE];
	WDS_ENTRY       Content[MAX_LEN_OF_MAC_TABLE];
	UCHAR           MaxSupportedRate;
	UCHAR           CurrTxRate;
	USHORT          TxQuality[MAX_LEN_OF_SUPPORTED_RATES];
	USHORT          OneSecTxOkCount;
	USHORT          OneSecTxRetryOkCount;
	USHORT          OneSecTxFailCount;
	ULONG           CurrTxRateStableTime; // # of second in current TX rate
	UCHAR           TxRateUpPenalty;      // extra # of second penalty due to last unstable condition
} WDS_TABLE_ENTRY, *PWDS_TABLE_ENTRY;
#endif
typedef struct _RT_802_11_WDS_ENTRY {
	PNET_DEV			dev;
	UCHAR				Valid;
	UCHAR				PhyMode;
	UCHAR				PeerWdsAddr[MAC_ADDR_LEN];
	UCHAR				MacTabMatchWCID;	// ASIC
	NDIS_802_11_WEP_STATUS  WepStatus;
	UCHAR					KeyIdx;
	CIPHER_KEY          	WdsKey;
} RT_802_11_WDS_ENTRY, *PRT_802_11_WDS_ENTRY;

typedef struct _WDS_TABLE {
	UCHAR               Mode;
	ULONG               Size;
	RT_802_11_WDS_ENTRY	WdsEntry[MAX_WDS_ENTRY];
} WDS_TABLE, *PWDS_TABLE;




#ifdef CONFIG_AP_SUPPORT

#if 0
typedef struct _BSS_STRUCT {
	struct wifi_dev wdev;

	INT mbss_idx;

#ifdef HOSTAPD_SUPPORT
	NDIS_HOSTAPD_STATUS Hostapd;
	BOOLEAN HostapdWPS;
#endif

	CHAR Ssid[MAX_LEN_OF_SSID];
	UCHAR SsidLen;
	BOOLEAN bHideSsid;

	USHORT CapabilityInfo;

	UCHAR MaxStaNum;	/* Limit the STA connection number per BSS */
	UCHAR StaCount;
	UINT16 StationKeepAliveTime;	/* unit: second */

	/*
		Security segment
	*/
	UCHAR RSNIE_Len[2];
	UCHAR RSN_IE[2][MAX_LEN_OF_RSNIE];

	/* WPA */
	UCHAR GMK[32];
	UCHAR PMK[32];
	UCHAR GTK[32];
	UCHAR GNonce[32];
	NDIS_802_11_PRIVACY_FILTER PrivacyFilter;

	/* for Group Rekey, AP ONLY */
	RT_WPA_REKEY WPAREKEY;
	ULONG REKEYCOUNTER;
	RALINK_TIMER_STRUCT REKEYTimer;
	UCHAR REKEYTimerRunning;
	UINT8 RekeyCountDown;

	/* For PMK Cache using, AP ONLY */
	ULONG PMKCachePeriod;	/* unit : jiffies */
	NDIS_AP_802_11_PMKID PMKIDCache;

#ifdef DOT1X_SUPPORT
	BOOLEAN PreAuth;

	/* For 802.1x daemon setting per BSS */
	UINT8 radius_srv_num;
	RADIUS_SRV_INFO radius_srv_info[MAX_RADIUS_SRV_NUM];
	UINT8 NasId[IFNAMSIZ];
	UINT8 NasIdLen;
#endif /* DOT1X_SUPPORT */

#ifdef WAPI_SUPPORT
	UCHAR WAPIPassPhrase[64];	/* WAPI PSK pass phrase */
	UINT WAPIPassPhraseLen;	/* the length of WAPI PSK pass phrase */
	UINT WapiPskType;	/* 0 - Hex, 1 - ASCII */
	UCHAR WAPI_BK[16];	/* WAPI base key */

	UCHAR NMK[LEN_WAPI_NMK];
	UCHAR key_announce_flag[LEN_WAPI_TSC];
	BOOLEAN sw_wpi_encrypt;	/* WPI data encrypt by SW */
#endif /* WAPI_SUPPORT */


	/*
		Transmitting segment
	*/
	UCHAR TxRate; /* RATE_1, RATE_2, RATE_5_5, RATE_11, ... */
	UCHAR DesiredRates[MAX_LEN_OF_SUPPORTED_RATES];	/* OID_802_11_DESIRED_RATES */
	UCHAR DesiredRatesIndex;
	UCHAR MaxTxRate; /* RATE_1, RATE_2, RATE_5_5, RATE_11 */


	/*
		Statistics segment
	*/
	/*MBSS_STATISTICS MbssStat;*/
	ULONG TxCount;
	ULONG RxCount;
	ULONG ReceivedByteCount;
	ULONG TransmittedByteCount;
	ULONG RxErrorCount;
	ULONG RxDropCount;

	ULONG TxErrorCount;
	ULONG TxDropCount;
	ULONG ucPktsTx;
	ULONG ucPktsRx;
	ULONG mcPktsTx;
	ULONG mcPktsRx;
	ULONG bcPktsTx;
	ULONG bcPktsRx;

	UCHAR BANClass3Data;
	ULONG IsolateInterStaTraffic;

	/* outgoing BEACON frame buffer and corresponding TXWI */
	BCN_BUF_STRUC bcn_buf;
	UCHAR TimBitmaps[WLAN_MAX_NUM_OF_TIM];
	UCHAR TimIELocationInBeacon;

#ifdef MT_MAC
	TIM_BUF_STRUC tim_buf;
	UCHAR TimIELocationInTim;
#endif /* MT_MAC */

#ifdef DOT11V_WNM_SUPPORT
	UCHAR DMSEntrycount;
	UCHAR totalDMScount;
#endif /* DOT11V_WNM_SUPPORT */

	RT_802_11_ACL AccessControlList;

	/* EDCA QoS */
	/*BOOLEAN bWmmCapable;*/	/* 0:disable WMM, 1:enable WMM */
	BOOLEAN bDLSCapable;	/* 0:disable DLS, 1:enable DLS */
#ifdef QOS_DLS_SUPPORT
	UCHAR DlsPTK[64];	/* Due to windows dirver count on meetinghouse to handle 4-way shake */
#endif /* QOS_DLS_SUPPORT */

	/*
	   Why need the parameter: 2009/09/22

	   1. iwpriv ra0 set WmmCapable=0
	   2. iwpriv ra0 set WirelessMode=9
	   3. iwpriv ra0 set WirelessMode=0
	   4. iwpriv ra0 set SSID=SampleAP

	   After the 4 commands, WMM is still enabled.
	   So we need the parameter to recover WMM Capable flag.

	   No the problem in station mode.
	 */
	BOOLEAN bWmmCapableOrg;	/* origin Wmm Capable in non-11n mode */


	/*
`		WPS segment
	*/
#if defined (WSC_STA_SUPPORT) || defined (WSC_AP_SUPPORT)

	WSC_LV_INFO WscIEBeacon;
	WSC_LV_INFO WscIEProbeResp;
#endif
#ifdef WSC_AP_SUPPORT
	WSC_CTRL WscControl;
	WSC_SECURITY_MODE WscSecurityMode;
#endif /* WSC_AP_SUPPORT */

#ifdef IDS_SUPPORT
	UINT32 RcvdConflictSsidCount;
	UINT32 RcvdSpoofedAssocRespCount;
	UINT32 RcvdSpoofedReassocRespCount;
	UINT32 RcvdSpoofedProbeRespCount;
	UINT32 RcvdSpoofedBeaconCount;
	UINT32 RcvdSpoofedDisassocCount;
	UINT32 RcvdSpoofedAuthCount;
	UINT32 RcvdSpoofedDeauthCount;
	UINT32 RcvdSpoofedUnknownMgmtCount;
	UINT32 RcvdReplayAttackCount;

	CHAR RssiOfRcvdConflictSsid;
	CHAR RssiOfRcvdSpoofedAssocResp;
	CHAR RssiOfRcvdSpoofedReassocResp;
	CHAR RssiOfRcvdSpoofedProbeResp;
	CHAR RssiOfRcvdSpoofedBeacon;
	CHAR RssiOfRcvdSpoofedDisassoc;
	CHAR RssiOfRcvdSpoofedAuth;
	CHAR RssiOfRcvdSpoofedDeauth;
	CHAR RssiOfRcvdSpoofedUnknownMgmt;
	CHAR RssiOfRcvdReplayAttack;
#endif /* IDS_SUPPORT */

#ifdef DOT11R_FT_SUPPORT
	FT_CFG FtCfg;
#endif /* DOT11R_FT_SUPPORT */

#ifdef DOT11V_WNM_SUPPORT
	WNM_CONFIG WnmCfg;
#endif /* DOT11V_WNM_SUPPORT */

#ifdef DOT11K_RRM_SUPPORT
	RRM_CONFIG RrmCfg;
#endif /* DOT11K_RRM_SUPPORT */

#ifdef DOT11W_PMF_SUPPORT
	PMF_CFG PmfCfg;
#endif /* DOT11W_PMF_SUPPORT */

#ifdef EASY_CONFIG_SETUP
	EASY_CONFIG_INFO EasyConfigInfo;
#endif /* EASY_CONFIG_SETUP */

	/* YF@20120417: Avoid connecting to AP in Poor Env, value 0 fOr disable. */
	CHAR AssocReqRssiThreshold;
	CHAR RssiLowForStaKickOut;

#ifdef CONFIG_DOT11U_INTERWORKING
	GAS_CTRL GASCtrl;
#endif

#ifdef CONFIG_HOTSPOT
	HOTSPOT_CTRL HotSpotCtrl;
#endif

#ifdef CONFIG_DOT11V_WNM
	WNM_CTRL WNMCtrl;
#endif

#ifdef SPECIFIC_TX_POWER_SUPPORT
	CHAR TxPwrAdj;
#endif /* SPECIFIC_TX_POWER_SUPPORT */

#ifdef IOT_CFG80211_SUPPORT
    /* Extra IEs for (Re)Association Response provided by wpa_supplicant. E.g, WPS & P2P & WFD...etc */
    UCHAR AssocRespExtraIe[512];
    UINT32 AssocRespExtraIeLen;
#endif /* IOT_CFG80211_SUPPORT */

#ifdef DBG
#ifdef MT_MAC
    ULONG WriteBcnDoneTime[MAX_TIME_RECORD];
    ULONG BcnDmaDoneTime[MAX_TIME_RECORD];
    UCHAR bcn_not_idle_time;
    UINT32 bcn_recovery_num;
    ULONG TXS_TSF[MAX_TIME_RECORD];
    ULONG TXS_SN[MAX_TIME_RECORD];
    UCHAR timer_loop;
#endif /* MT_MAC */
#endif
} BSS_STRUCT;
#endif
#if 0
typedef struct _AP_ADMIN_CONFIG {
	USHORT          CapabilityInfo;
	// Multiple SSID
    UCHAR               BssidNum;
	UCHAR				MacMask;	
    MULTISSID_STRUCT    MBSSID[MAX_MBSSID_NUM];
	ULONG				IsolateInterStaTrafficBTNBSSID;

#ifdef APCLI_SUPPORT
	UCHAR				ApCliInfRunned;				// Number of  ApClient interface which was running. value from 0 to MAX_APCLI_INTERFACE
#if raghav
	UINT8 ApCliNum;
	BOOLEAN FlgApCliIsUapsdInfoUpdated;
#endif //raghav
	APCLI_STRUCT		ApCliTab[MAX_APCLI_NUM];		//AP-client 
	
#ifdef APCLI_AUTO_CONNECT_SUPPORT
		BOOLEAN 	ApCliAutoConnectRunning;
		BOOLEAN 	ApCliAutoConnectChannelSwitching;
#endif /* APCLI_AUTO_CONNECT_SUPPORT */
#endif // APCLI_SUPPORT //
#ifdef MAC_REPEATER_SUPPORT
		NDIS_SPIN_LOCK ReptCliEntryLock;
		REPEATER_CLIENT_ENTRY *ReptCliHash[HASH_TABLE_SIZE];
		REPEATER_CLIENT_ENTRY_MAP *ReptMapHash[HASH_TABLE_SIZE];
		UCHAR BridgeAddress[MAC_ADDR_LEN];
		REPEATER_CTRL_STRUCT ReptControl;
#endif /* MAC_REPEATER_SUPPORT */

	// for wpa
// remove to MBSSID
//	UCHAR           GMK[32];
//	UCHAR           PMK[32];
//	UCHAR           GNonce[32];
//	UCHAR           PortSecured;  // WPA 802.1x port control, WPA_802_1X_PORT_SECURED, WPA_802_1X_PORT_NOT_SECURED
//	WPA_GTK_STATE   WpaGTKState;
//	ULONG           MICErrorCounter;
//	UCHAR           GKeyDoneStations; // number of entry left to update group key
	RALINK_TIMER_STRUCT         CounterMeasureTimer;

	UCHAR           CMTimerRunning;
	UCHAR           BANClass3Data;
	LARGE_INTEGER   aMICFailTime;
	LARGE_INTEGER   PrevaMICFailTime;
	ULONG           MICFailureCounter;

	// for Group Rekey 
#if 0 //raghav
	RT_WPA_REKEY    WPAREKEY;
	ULONG           REKEYCOUNTER;
	RALINK_TIMER_STRUCT     REKEYTimer;
	UCHAR           REKEYTimerRunning;
#endif

	RSSI_SAMPLE		RssiSample;
	ULONG           NumOfAvgRssiSample;

	BOOLEAN         bAutoChannelAtBootup;  // 0: disable, 1: enable
	BOOLEAN         bIsolateInterStaTraffic;
	BOOLEAN         bHideSsid;

	// temporary latch for Auto channel selection
	CHAR            AutoChannel_MaxRssi;  // max RSSI during Auto Channel Selection period
	UCHAR           AutoChannel_Channel;  // channel number during Auto Channel Selection

//	Remove to MBSSID
//	UCHAR           RSNIE_Len[2];
//	UCHAR           RSN_IE[2][MAX_LEN_OF_RSNIE];

	UCHAR           Key_Counter[32];  //jan for wpa

	ULONG			PMKCachePeriod;		

	UCHAR           TimIELocationInBeacon;
	UCHAR           CapabilityInfoLocationInBeacon;
	UCHAR           DtimCount;      // 0.. DtimPeriod-1
	UCHAR           DtimPeriod;     // default = 3
	UCHAR           ErpIeContent;
	ULONG           LastOLBCDetectTime;
	ULONG			LastNoneHTOLBCDetectTime;
	ULONG       	LastScanTime;       // Record last scan time for issue BSSID_SCAN_LIST

	// For 802.1x daemon setting
	UINT32			own_ip_addr;
	UINT32			retry_interval;
	UINT32			session_timeout_interval;
	UCHAR 			EAPifname[IFNAMSIZ];		// indicate as the binding interface for EAP negotiation.
	UCHAR			EAPifname_len;
	UCHAR 			PreAuthifname[IFNAMSIZ];	// indicate as the binding interface for WPA2 Pre-authentication.
	UCHAR			PreAuthifname_len;

	// for CCX 1.0
#if 0	// AP doesn't need CCX variables
	BOOLEAN         bCkipOn;
	UCHAR           CkipFlag;
	UCHAR           GIV[3];  //for CCX iv
	UCHAR           RxSEQ[4];
	UCHAR           TxSEQ[4];
	UCHAR           CKIPMIC[4];
#endif

	// EDCA parameters to be announced to its local BSS
	EDCA_PARM       BssEdcaParm;

	RALINK_TIMER_STRUCT		ApQuickResponeForRateUpTimer;
	BOOLEAN					ApQuickResponeForRateUpTimerRunning;	
	// rt2860c support unlimited piggyback. should delete this boolean.
	BOOLEAN					bOneClientUsePiggyBack;	// This parameter didn't need anymore in RT2860.

#ifdef IDS_SUPPORT
	// intrusion detection parameter	
	BOOLEAN					IdsEnable;
	UINT32					AuthFloodThreshold;		// Authentication frame flood threshold
	UINT32					AssocReqFloodThreshold;		// Association request frame flood threshold
	UINT32					ReassocReqFloodThreshold;	// Re-association request frame flood threshold
	UINT32					ProbeReqFloodThreshold;		// Probe request frame flood threshold
	UINT32					DisassocFloodThreshold;		// Disassociation frame flood threshold
	UINT32					DeauthFloodThreshold;		// Deauthentication frame flood threshold
	UINT32					EapReqFooldThreshold;		// EAP request frame flood threshold

	UINT32					RcvdAuthCount;	
	UINT32					RcvdAssocReqCount;		
	UINT32					RcvdReassocReqCount;	
	UINT32					RcvdProbeReqCount;		
	UINT32					RcvdDisassocCount;		
	UINT32					RcvdDeauthCount;		
	UINT32					RcvdEapReqCount;	

	RALINK_TIMER_STRUCT		IDSTimer;
	BOOLEAN					IDSTimerRunning;
#endif // IDS_SUPPORT //
			
#ifdef RT2870
	BEACON_SYNC_STRUCT		*pBeaconSync;
	RALINK_TIMER_STRUCT		BeaconUpdateTimer;
	UINT32					BeaconAdjust;
	UINT32					BeaconFactor;
	UINT32					BeaconRemain;
#endif // RT2870 //
} AP_ADMIN_CONFIG, *PAP_ADMIN_CONFIG;

#else



typedef struct _AP_ADMIN_CONFIG {
	USHORT CapabilityInfo;
	/* Multiple SSID */
	UCHAR BssidNum;
	UCHAR MacMask;
	BSS_STRUCT MBSSID[HW_BEACON_MAX_NUM];
	ULONG IsolateInterStaTrafficBTNBSSID;

#ifdef APCLI_SUPPORT
	UCHAR ApCliInfRunned;	/* Number of  ApClient interface which was running. value from 0 to MAX_APCLI_INTERFACE */
	UINT8 ApCliNum;
	BOOLEAN FlgApCliIsUapsdInfoUpdated;
	APCLI_STRUCT *ApCliTab;//[MAX_APCLI_NUM];	/*AP-client */
#ifdef APCLI_AUTO_CONNECT_SUPPORT
	BOOLEAN		ApCliAutoConnectRunning;
	BOOLEAN		ApCliAutoConnectChannelSwitching;
#endif /* APCLI_AUTO_CONNECT_SUPPORT */
#endif /* APCLI_SUPPORT */

#ifdef MAC_REPEATER_SUPPORT
    BOOLEAN bMACRepeaterEn;
    UCHAR MACRepeaterOuiMode;
    UINT8 EthApCliIdx;
    UCHAR RepeaterCliSize;
	NDIS_SPIN_LOCK ReptCliEntryLock;
	REPEATER_CLIENT_ENTRY **ReptCliHash;//[HASH_TABLE_SIZE];
	REPEATER_CLIENT_ENTRY_MAP **ReptMapHash;//[HASH_TABLE_SIZE];
	UCHAR BridgeAddress[MAC_ADDR_LEN];
	REPEATER_CTRL_STRUCT ReptControl;
    NDIS_SPIN_LOCK CliLinkMapLock;
    MBSS_TO_CLI_LINK_MAP_T  MbssToCliLinkMap[HW_BEACON_MAX_NUM];
    REPEATER_CLIENT_ENTRY *pRepeaterCliPool;
    REPEATER_CLIENT_ENTRY_MAP *pRepeaterCliMapPool;
	UINT32 RepeaterBitMap;  // Raghav:to configure in WTBL
#endif /* MAC_REPEATER_SUPPORT */

	/* for wpa */
	RALINK_TIMER_STRUCT CounterMeasureTimer;

	UCHAR CMTimerRunning;
	UCHAR BANClass3Data;
	LARGE_INTEGER aMICFailTime;
	LARGE_INTEGER PrevaMICFailTime;
	ULONG MICFailureCounter;

	RSSI_SAMPLE RssiSample;
	ULONG NumOfAvgRssiSample;

	BOOLEAN bAutoChannelAtBootup;	/* 0: disable, 1: enable */
	ChannelSel_Alg AutoChannelAlg;	/* Alg for selecting Channel */
#ifdef AP_SCAN_SUPPORT
	UINT32  ACSCheckTime;           /* Periodic timer to trigger Auto Channel Selection (unit: second) */
	UINT32  ACSCheckCount;          /* if  ACSCheckCount > ACSCheckTime, then do ACS check */
#endif /* AP_SCAN_SUPPORT */
	BOOLEAN bAvoidDfsChannel;	/* 0: disable, 1: enable */
	BOOLEAN bIsolateInterStaTraffic;
	BOOLEAN bHideSsid;

	/* temporary latch for Auto channel selection */
	ULONG ApCnt;		/* max RSSI during Auto Channel Selection period */
	UCHAR AutoChannel_Channel;	/* channel number during Auto Channel Selection */
	UCHAR current_channel_index;	/* current index of channel list */
	UCHAR AutoChannelSkipListNum;	/* number of rejected channel list */
	UCHAR AutoChannelSkipList[10];
	UCHAR DtimCount;	/* 0.. DtimPeriod-1 */
	UCHAR DtimPeriod;	/* default = 3 */
	UCHAR ErpIeContent;
	ULONG LastOLBCDetectTime;
	ULONG LastNoneHTOLBCDetectTime;
	ULONG LastScanTime;	/* Record last scan time for issue BSSID_SCAN_LIST */

#ifdef DOT1X_SUPPORT
	/* dot1x related parameter */
	UINT32 own_ip_addr;
	UINT32 retry_interval;
	UINT32 session_timeout_interval;
	UINT32 quiet_interval;
	UCHAR EAPifname[HW_BEACON_MAX_NUM][IFNAMSIZ];	/* indicate as the binding interface for EAP negotiation. */
	UCHAR EAPifname_len[HW_BEACON_MAX_NUM];
	UCHAR PreAuthifname[HW_BEACON_MAX_NUM][IFNAMSIZ];	/* indicate as the binding interface for WPA2 Pre-authentication. */
	UCHAR PreAuthifname_len[HW_BEACON_MAX_NUM];
#endif /* DOT1X_SUPPORT */

	/* EDCA parameters to be announced to its local BSS */
	EDCA_PARM BssEdcaParm;

	RALINK_TIMER_STRUCT ApQuickResponeForRateUpTimer;
	BOOLEAN ApQuickResponeForRateUpTimerRunning;

#ifdef IDS_SUPPORT
	/* intrusion detection parameter */
	BOOLEAN IdsEnable;
	UINT32 AuthFloodThreshold;	/* Authentication frame flood threshold */
	UINT32 AssocReqFloodThreshold;	/* Association request frame flood threshold */
	UINT32 ReassocReqFloodThreshold;	/* Re-association request frame flood threshold */
	UINT32 ProbeReqFloodThreshold;	/* Probe request frame flood threshold */
	UINT32 DisassocFloodThreshold;	/* Disassociation frame flood threshold */
	UINT32 DeauthFloodThreshold;	/* Deauthentication frame flood threshold */
	UINT32 EapReqFloodThreshold;	/* EAP request frame flood threshold */
	UINT32 DataFloodThreshold;		/* Malicious data frame flood threshold */

	UINT32 RcvdAuthCount;
	UINT32 RcvdAssocReqCount;
	UINT32 RcvdReassocReqCount;
	UINT32 RcvdProbeReqCount;
	UINT32 RcvdDisassocCount;
	UINT32 RcvdDeauthCount;
	UINT32 RcvdEapReqCount;
	UINT32 RcvdMaliciousDataCount;	/* Data Frame DDOS */

	RALINK_TIMER_STRUCT IDSTimer;
	BOOLEAN IDSTimerRunning;
#endif /* IDS_SUPPORT */

	/* Indicate the maximum idle timeout */
	UINT32 StaIdleTimeout;

	ULONG EntryLifeCheck;



#ifdef DOT11_N_SUPPORT
#ifdef GREENAP_SUPPORT
	UCHAR GreenAPLevel;
	BOOLEAN bGreenAPEnable;
	BOOLEAN bGreenAPActive;
#endif /* GREENAP_SUPPORT */
#endif

	UCHAR EntryClientCount;

} AP_ADMIN_CONFIG;

#endif

#ifdef IGMP_SNOOP_SUPPORT
typedef enum _IGMP_GROUP_TYPE
{
	MODE_IS_INCLUDE = 1,
	MODE_IS_EXCLUDE,
	CHANGE_TO_INCLUDE_MODE,
	CHANGE_TO_EXCLUDE_MODE,
	ALLOW_NEW_SOURCES,
	BLOCK_OLD_SOURCES
} IgmpGroupType;

typedef enum _MULTICAST_FILTER_ENTRY_TYPE
{
	MCAT_FILTER_STATIC = 0,
	MCAT_FILTER_DYNAMIC,
} MulticastFilterEntryType;

typedef struct _MEMBER_ENTRY
{
	struct _MEMBER_ENTRY *pNext;
//	UCHAR Addr[MAC_ADDR_LEN];
	USHORT Aid;
} MEMBER_ENTRY, *PMEMBER_ENTRY;

typedef struct _MULTICAST_FILTER_TABLE_ENTRY
{
	BOOLEAN Valid;
	MulticastFilterEntryType type; // 0: static, 1: dynamic.
	UINT  lastTime;
	PNET_DEV net_dev;
	UCHAR Addr[MAC_ADDR_LEN];
	LIST_HEADER MemberList;
	struct _MULTICAST_FILTER_TABLE_ENTRY *pNext;
} MULTICAST_FILTER_TABLE_ENTRY, *PMULTICAST_FILTER_TABLE_ENTRY;

typedef struct _MULTICAST_FILTER_TABLE
{
	UCHAR Size;
	PMULTICAST_FILTER_TABLE_ENTRY Hash[MAX_LEN_OF_MULTICAST_FILTER_HASH_TABLE];
	MULTICAST_FILTER_TABLE_ENTRY Content[MAX_LEN_OF_MULTICAST_FILTER_TABLE];
	NDIS_SPIN_LOCK MulticastFilterTabLock;
	NDIS_SPIN_LOCK FreeMemberPoolTabLock;
	MEMBER_ENTRY freeMemberPool[FREE_MEMBER_POOL_SIZE];
	LIST_HEADER freeEntryList;
} MULTICAST_FILTER_TABLE, *PMULTICAST_FILTER_TABLE;
#endif // IGMP_SNOOP_SUPPORT //

#endif // CONFIG_AP_SUPPORT //
// ----------- end of AP ----------------------------

#ifdef BLOCK_NET_IF
typedef struct _BLOCK_QUEUE_ENTRY
{
	BOOLEAN SwTxQueueBlockFlag;
	LIST_HEADER NetIfList;
} BLOCK_QUEUE_ENTRY, *PBLOCK_QUEUE_ENTRY;
#endif // BLOCK_NET_IF //

struct wificonf
{
	BOOLEAN	bShortGI;
	BOOLEAN bGreenField;
};


#ifdef ETH_CONVERT_SUPPORT

#define ETH_CONVERT_NODE_MAX 256

// Ethernet Convertor operation mode definitions.
typedef enum{
	ETH_CONVERT_MODE_DISABLE = 0,
	ETH_CONVERT_MODE_DONGLE = 1,	// Multiple client support, dispatch to AP by device mac address.
	ETH_CONVERT_MODE_CLONE = 2,		// Single client support, dispatch to AP by client's mac address.
	ETH_CONVERT_MODE_HYBRID = 3,	// Multiple client supprot, dispatch to AP by client's mac address.
}ETH_CONVERT_MODE;


typedef struct _ETH_CONVERT_STRUCT_
{
	UCHAR		EthCloneMac[MAC_ADDR_LEN];	//Only meanful when ECMode = Clone/Hybrid mode.
	UCHAR		ECMode;						// 0 = Disable, 1 = Dongle mode, 2 = Clone mode, 3 = Hybrid mode.
	BOOLEAN		CloneMacValid;				// 1 if the CloneMac is valid for connection. 0 if not valid.
	UINT32		nodeCount;					// the number of nodes which connect to Internet via us.
	CHAR       SSIDStr[MAX_LEN_OF_SSID];
	UCHAR		SSIDStrLen;
	BOOLEAN		macAutoLearn;				//0: disabled, 1: enabled.
}ETH_CONVERT_STRUCT;
#endif // ETH_CONVERT_SUPPORT //


typedef struct __WPACfg{
	NDIS_802_11_AUTHENTICATION_MODE     AuthMode;       // This should match to whatever microsoft defined
	NDIS_802_11_WEP_STATUS              WepStatus;

	UCHAR		DefaultKeyId;			

	// For WPA-PSK state
	WPA_STATE   WpaState;           	// Default is SS_NOTUSE and handled by microsoft 802.1x

	CIPHER_KEY		*pSharedKey;
	ARCFOURCONTEXT  *pWEPCONTEXT;

	UCHAR       ReplayCounter[8];
	UCHAR       ANonce[32];         	// ANonce for WPA-PSK from aurhenticator
	UCHAR       SNonce[32];         	// SNonce for WPA-PSK
	UCHAR		PMK[32];                // WPA PSK mode PMK
	UCHAR       PTK[64];                // WPA PSK mode PTK
	UCHAR		GTK[32];				// GTK from authenticator
    UCHAR       RSN_IE[MAX_LEN_OF_RSNIE];	// The content saved here should be little-endian format.
	UCHAR       RSNIE_Len;

	NDIS_802_11_WEP_STATUS              GroupKeyWepStatus;

	// WPA 802.1x port control, WPA_802_1X_PORT_SECURED, WPA_802_1X_PORT_NOT_SECURED
	UCHAR       PortSecured;

	// Add to support different cipher suite for WPA2/WPA mode
	NDIS_802_11_ENCRYPTION_STATUS		GroupCipher;		// Multicast cipher suite
	NDIS_802_11_ENCRYPTION_STATUS		PairCipher;			// Unicast cipher suite


	// For WPA countermeasures
	BOOLEAN     bBlockAssoc;        // Block associate attempt for 60 seconds after counter measure occurred.
	ULONG       LastMicErrorTime;   // record last MIC error time
	ULONG       MicErrCnt;          // Should be 0, 1, 2, then reset to zero (after disassoiciation).
} WPACfg_t;

typedef struct __ApRSNIE {
    UCHAR       RSNIE_Len;
    UCHAR       *RSN_IE;	// The content saved here should be little-endian format.
} ApRSNIE_t;

#ifdef IAR_COMPILE
PACKED typedef struct  _BA_SYNC_FRAME {
#else
typedef struct PACKED _BA_SYNC_FRAME {
#endif
	ULONG		numAsRecipient;		// I am recipient of numAsRecipient clients. These client are in the BARecEntry[]
	ULONG		numAsOriginator;	// I am originator of 	numAsOriginator clients. These clients are in the BAOriEntry[]
	UCHAR         	BATableIdx;
	UCHAR   		ORI_BAWinSize;

	//------ sync MAC_TABLE_ENTRY[Wcid]
	UCHAR		Wcid;
	USHORT		TXBAbitmap;	// This bitmap as originator, only keep in software used to mark AMPDU bit in TXWI 
	USHORT		BADeclineBitmap;
	USHORT		BAOriWcidArray[NUM_OF_TID]; // The mapping wcid of originator session. if TXBAbitmap bit is masked
} BA_SYNC_FRAME;

//
//  The miniport adapter structure
//


/* 802.11H */
typedef struct _DOT11_H {
	/* 802.11H and DFS related params */
	UCHAR CSCount;		/*Channel switch counter */
	UCHAR CSPeriod; 	/*Channel switch period (beacon count) */
	USHORT RDCount; 	/*Radar detection counter, if RDCount >  ChMovingTime, start to send beacons*/
	UCHAR RDMode;		/*Radar Detection mode */
	UCHAR org_ch;
	UCHAR new_channel;
	USHORT ChMovingTime;
	BOOLEAN bDFSIndoor;
	ULONG InServiceMonitorCount;	/* unit: sec */
	ULONG CalBufTime;	/* A Timing buffer for befroe calibrations which generates Tx signals */
} DOT11_H, *PDOT11_H;


typedef struct rtmp_mac_ctrl {
UINT8 wtbl_entry_cnt[4];
UINT16 wtbl_entry_size[4];
UINT32 wtbl_base_addr[4]; // base address for WTBL2/3/4
UINT32 wtbl_base_fid[4];
UINT32 page_size;
}RTMP_MAC_CTRL;


#define TX_SWQ_FIFO_LEN		512  //Max number of Packets
typedef struct tx_swq_fifo{
	UCHAR swq[TX_SWQ_FIFO_LEN]; // value 0 is used to indicate free to insert, value 1~127 used to incidate the WCID entry
	UINT enqIdx;
	UINT deqIdx;
	NDIS_SPIN_LOCK swq_lock;	/* spinlock for swq */
}TX_SWQ_FIFO;

#ifdef IOT_CFG80211_SUPPORT
typedef struct __CMD_RTPRIV_IOCTL_80211_CHAN {
	UINT8 ChanId;
	UINT8 IfType;
	UINT8 ChanType;

	UINT32 MonFilterFlag;

	PVOID chan;
} CMD_RTPRIV_IOCTL_80211_CHAN;

typedef struct _CFG80211_VIF_DEV
{
	struct _CFG80211_VIF_DEV *pNext;
	BOOLEAN isMainDev;
	UINT32 devType;
	PNET_DEV net_dev;
	UCHAR CUR_MAC[MAC_ADDR_LEN];

	/* ProbeReq Frame */
	BOOLEAN Cfg80211RegisterProbeReqFrame;
	CHAR Cfg80211ProbeReqCount;

	/* Action Frame */
	BOOLEAN Cfg80211RegisterActionFrame;
	CHAR Cfg80211ActionCount;
} CFG80211_VIF_DEV, *PCFG80211_VIF_DEV;

typedef struct _CFG80211_VIF_DEV_SET
{
#define MAX_CFG80211_VIF_DEV_NUM  2

	BOOLEAN inUsed;
	UINT32 vifDevNum;
	LIST_HEADER vifDevList;
	BOOLEAN isGoingOn; /* To check any vif in list */
} CFG80211_VIF_DEV_SET;

/* TxMmgt Related */
typedef struct _CFG80211_TX_PACKET
{
	struct _CFG80211_TX_PACKET *pNext;
	UINT32 TxStatusSeq;			  /* TxMgmt Packet ID from sequence */
	UCHAR *pTxStatusBuf;		  /* TxMgmt Packet buffer content */
	UINT32 TxStatusBufLen;		  /* TxMgmt Packet buffer Length */

} CFG80211_TX_PACKET, *PCFG80211_TX_PACKET;

/* CFG80211 Total CTRL Point */
typedef struct _CFG80211_CONTROL
{
	BOOLEAN FlgCfg8021Disable2040Scan;
	BOOLEAN FlgCfg80211Scanning;   /* Record it When scanReq from wpa_supplicant */
	BOOLEAN FlgCfg80211Connecting; /* Record it When ConnectReq from wpa_supplicant*/

	/* Scan Related */
    UINT32 *pCfg80211ChanList;    /* the channel list from from wpa_supplicant */
    UCHAR Cfg80211ChanListLen;    /* channel list length */
	UCHAR Cfg80211CurChanIndex;   /* current index in channel list when driver in scanning */

	UCHAR *pExtraIe;  /* Carry on Scan action from supplicant */
	UINT   ExtraIeLen;

	UCHAR Cfg_pending_Ssid[MAX_LEN_OF_SSID+1]; /* Record the ssid, When ScanTable Full */
   	UCHAR Cfg_pending_SsidLen;

	/* ROC Related */
	RALINK_TIMER_STRUCT Cfg80211RocTimer;
	CMD_RTPRIV_IOCTL_80211_CHAN Cfg80211ChanInfo;
	BOOLEAN Cfg80211RocTimerInit;
	BOOLEAN Cfg80211RocTimerRunning;

	/* Tx_Mmgt Related */
	UINT32 TxStatusSeq;			  /* TxMgmt Packet ID from sequence */
	UCHAR *pTxStatusBuf;		  /* TxMgmt Packet buffer content */
	UINT32 TxStatusBufLen;		  /* TxMgmt Packet buffer Length */
	BOOLEAN TxStatusInUsed;
	LIST_HEADER cfg80211TxPacketList;

	/* P2P Releated*/
	UCHAR P2PCurrentAddress[MAC_ADDR_LEN];	  /* User changed MAC address */
	BOOLEAN isCfgDeviceInP2p; 				  /* For BaseRate 6 */

	/* MainDevice Info. */
	CFG80211_VIF_DEV cfg80211MainDev;
#if defined(RT_CFG80211_P2P_CONCURRENT_DEVICE) || defined(CFG80211_MULTI_STA)
	/* For add_virtual_intf */
	CFG80211_VIF_DEV_SET Cfg80211VifDevSet;
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE || CFG80211_MULTI_STA */

#ifdef CFG80211_MULTI_STA
    PNET_DEV multi_sta_net_dev;
    BOOLEAN flg_cfg_multi_sta_init;
#endif /* CFG80211_MULTI_STA */

#ifdef RT_CFG80211_P2P_SUPPORT
	BOOLEAN bP2pCliPmEnable;

	BOOLEAN bPreKeepSlient;
	BOOLEAN	bKeepSlient;

	UCHAR MyGOwcid;
	UCHAR NoAIndex;
	UCHAR CTWindows; /* CTWindows and OppPS parameter field */

	P2PCLIENT_NOA_SCHEDULE GONoASchedule;
	RALINK_TIMER_STRUCT P2pCTWindowTimer;
	RALINK_TIMER_STRUCT P2pSwNoATimer;
	RALINK_TIMER_STRUCT P2pPreAbsenTimer;

	UCHAR P2pSupRate[MAX_LEN_OF_SUPPORTED_RATES];
    UCHAR P2pSupRateLen;
    UCHAR P2pExtRate[MAX_LEN_OF_SUPPORTED_RATES];
    UCHAR P2pExtRateLen;

#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
	/* Dummy P2P Device for ANDROID JB */
	PNET_DEV dummy_p2p_net_dev;
	struct wifi_dev dummy_p2p_wdev;
	BOOLEAN flg_cfg_dummy_p2p_init;
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */

#ifdef RT_CFG80211_P2P_SINGLE_DEVICE
	ULONG P2POpStatusFlags; /* P2P_CLI_UP / P2P_GO_UP*/
#endif /* RT_CFG80211_P2P_SINGLE_DEVICE */
#endif /* RT_CFG80211_P2P_SUPPORT */

	/* In AP Mode */
	UINT8 isCfgInApMode;    /* Is any one Device in AP Mode */
	UCHAR *beacon_tail_buf; /* Beacon buf from upper layer */
	UINT32 beacon_tail_len;

	UCHAR *pCfg80211ExtraIeAssocRsp;
	UINT32 Cfg80211ExtraIeAssocRspLen;

	/* for AdHoc Mode */
	UCHAR *BeaconExtraIe;
	UINT  BeaconExtraIeLen;

	/* Mcc Part */
	//BOOLEAN isMccOn;

	/* TODO: need fix it */
	UCHAR Cfg80211_Alpha2[2];
} CFG80211_CTRL, *PCFG80211_CTRL;
#endif /* IOT_CFG80211_SUPPORT */

/* For Wake on Wireless LAN */
#ifdef IOT_WOW_SUPPORT
typedef struct _WOW_CFG_STRUCT {
	BOOLEAN			bEnable;		/* Enable WOW function*/
	BOOLEAN			bWOWFirmware;	/* Enable WOW function, trigger to reload WOW-support firmware */
	BOOLEAN			bInBand;		/* use in-band signal to wakeup system */
	UINT8			nSelectedGPIO;	/* Side band signal to wake up system */
	UINT8			nDelay;			/* Delay number is multiple of 3 secs, and it used to postpone the WOW function */
	UINT32          nHoldTime;      /* GPIO pulse hold time, unit: 1us, 0 means hold forever.*/
	BOOLEAN			bWoWRunning;	/* WOW function is working */
	UINT8			nWakeupInterface; /* PCI:0 USB:1 GPIO:2 */
	UINT8			IPAddress[16];	/* Used for ARP response */
	UINT8			bGPIOHighLow;	/* 0: low to high, 1: high to low */
} WOW_CFG_STRUCT, *PWOW_CFG_STRUCT;

typedef enum {
	WOW_GPIO_LOW_TO_HIGH,
	WOW_GPIO_HIGH_TO_LOW
} WOW_GPIO_HIGH_LOW_T;

typedef enum {
	WOW_PKT_TO_HOST,
	WOW_PKT_TO_ANDES
} WOW_PKT_FLOW_T;

typedef enum {
	WOW_WAKEUP_BY_PCIE,
	WOW_WAKEUP_BY_USB,
	WOW_WAKEUP_BY_GPIO
} WOW_WAKEUP_METHOD_T;

typedef enum {
	WOW_ENABLE = 1,
	WOW_TRAFFIC = 3,
	WOW_WAKEUP = 4
} WOW_FEATURE_T;

typedef enum {
	WOW_MASK_CFG = 1,
	WOW_SEC_CFG,
	WOW_INFRA_CFG,
	WOW_P2P_CFG,
} WOW_CONFIG_T;

enum {
	WOW_MAGIC_PKT,
	WOW_BITMAP,
	WOW_IPV4_TCP_SYNC,
	WOW_IPV6_TCP_SYNC
};
#endif /* IOT_WOW_SUPPORT */


typedef struct _RTMP_ADAPTER
{
	PVOID				ram_cookie;	// reserved for RAM extension
	WPACfg_t			*pStaWPACfg;
	PNET_DEV			net_dev;

	MAC_TABLE				  MacTab;	  // ASIC on-chip WCID entry table.  At TX, ASIC always use key according to this on-chip table.

/***********************************************************/
/*      HIF  related parameters                                                           */
/***********************************************************/

	UINT					int_enable_reg;
	UINT					int_disable_mask;
	UINT					int_pending;
#ifdef SHRINK
	RTMP_DMABUF             TxBufSpace[NUM_OF_TX_RING]; // Shared memory of all 1st pre-allocated TxBuf associated with each TXD
	RTMP_DMABUF             RxDescRing;                 // Shared memory for RX descriptors
	RTMP_DMABUF             TxDescRing[NUM_OF_TX_RING]; 	// Shared memory for Tx descriptors
#endif
	RTMP_TX_RING            TxRing[NUM_OF_TX_RING];     	// AC0~4 + HCCA

	NDIS_SPIN_LOCK          irq_lock;

/*****************************************************************************************/
/*      Tx related parameters                                                           */
/*****************************************************************************************/
#ifdef SHRINK

	RTMP_DMABUF             MgmtDescRing;               	// Shared memory for MGMT descriptors
	RTMP_MGMT_RING          MgmtRing;
#endif
	NDIS_SPIN_LOCK          MgmtRingLock;               	// Prio Ring spinlock


/*****************************************************************************************/
/*      Rx related parameters                                                           */
/*****************************************************************************************/
#ifdef SHRINK

	RTMP_RX_RING            RxRing;
	NDIS_SPIN_LOCK          RxRingLock;                 // Rx Ring spinlock
#endif

/*****************************************************************************************/
/*      ASIC related parameters                                                          */
/*****************************************************************************************/
#ifdef SHRINK
	UINT32               	MACVersion;      	// MAC version. Record rt2860C(0x28600100) or rt2860D (0x28600101)..
#endif
	RTMP_CHIP_OP  chipOps;
    RTMP_CHIP_CAP chipCap;
   // PHY_OPS      *phy_op; // phy_op pointer
	struct hw_setting hw_cfg;
#ifdef MT_MAC
	   struct rtmp_mac_ctrl mac_ctrl;
	//   USHORT rx_pspoll_filter;
#endif /* MT_MAC */

	// ---------------------------
	// E2PROM
	// ---------------------------
	ULONG                   EepromVersion;          // byte 0: version, byte 1: revision, byte 2~3: unused
	UCHAR                   EEPROMAddressNum;       // 93c46=6  93c66=8
	USHORT                  EEPROMDefaultValue[NUM_EEPROM_BBP_PARMS];
	ULONG                   FirmwareVersion;        // byte 0: Minor version, byte 1: Major version, otherwise unused.
	RTMP_RF_REGS            LatchRfRegs;    // latch th latest RF programming value since RF IC doesn't support READ
	EEPROM_ANTENNA_STRUC    Antenna;                            // Since ANtenna definition is different for a & g. We need to save it for future reference.
	UCHAR                   BbpRssiToDbmDelta;
	CHANNEL_TX_POWER        TxPower[MAX_NUM_OF_CHANNELS];       // Store Tx power value for all channels.

	CHANNEL_TX_POWER        ChannelList[MAX_NUM_OF_CHANNELS];   // list all supported channels for site survey
	UCHAR                   ChannelListNum;                     // number of channel in ChannelList[]

	//+++For RT2870, the parameteres is start from BGRssiOffset1 ~ BGRssiOffset3
	CHAR		BGRssiOffset0;				// Store B/G RSSI#0 Offset value on EEPROM 0x46h
	CHAR		BGRssiOffset1;				// Store B/G RSSI#1 Offset value 
	CHAR		BGRssiOffset2;				// Store B/G RSSI#2 Offset value 
	//---

#ifdef CONFIG_5G_SUPPORT
		//+++For RT2870, the parameteres is start from ARssiOffset1 ~ ARssiOffset3	
		CHAR		ARssiOffset0;				// Store A RSSI#0 Offset value on EEPROM 0x4Ah
		CHAR		ARssiOffset1;				// Store A RSSI#1 Offset value 
		CHAR		ARssiOffset2;				// Store A RSSI#2 Offset value 
#endif // CONFIG_5G_SUPPORT //
		
		CHAR		BLNAGain;					// Store B/G external LNA#0 value on EEPROM 0x44h
#ifdef CONFIG_5G_SUPPORT
		CHAR		ALNAGain0;					// Store A external LNA#0 value for ch36~64
		CHAR		ALNAGain1;					// Store A external LNA#1 value for ch100~128
		CHAR		ALNAGain2;					// Store A external LNA#2 value for ch132~165
#endif // CONFIG_5G_SUPPORT //

#if 0
	UCHAR				RssiSingalstrengthOffet;
#endif
	

/*****************************************************************************************/
/*      802.11 related parameters                                                        */
/*****************************************************************************************/
	PSPOLL_FRAME            	PsPollFrame;
    HEADER_802_11               NullFrame;


//========AP===========
#ifdef CONFIG_AP_SUPPORT

	USHORT BeaconOffset[HW_BEACON_MAX_NUM];
	AP_ADMIN_CONFIG ApCfg;	/* user configuration when in AP mode */
	AP_MLME_AUX ApMlmeAux;

#ifdef APCLI_SUPPORT
	BOOLEAN flg_apcli_init;
#endif /* APCLI_SUPPORT */

/*#ifdef AUTO_CH_SELECT_ENHANCE */
#if 0 // Raghav
	PBSSINFO pBssInfoTab;
	PCHANNELINFO pChannelInfo;
#endif
/*#endif // AUTO_CH_SELECT_ENHANCE */

#ifdef MAT_SUPPORT
	MAT_STRUCT MatCfg;
#endif /* MAT_SUPPORT */


#endif



//=======STA===========
#ifdef CONFIG_STA_SUPPORT	
/* Modified by Wu Xi-Kun 4/21/2006 */
	// -----------------------------------------------
	// STA specific configuration & operation status
	// used only when pAd->OpMode == OPMODE_STA
	// -----------------------------------------------
	STA_ADMIN_CONFIG        StaCfg;           // user desired settings	
	STA_ACTIVE_CONFIG       StaActive;         // valid only when ADHOC_ON(pAd) || INFRA_ON(pAd)
	CHAR                    nickname[IW_ESSID_MAX_SIZE+1]; // nickname, only used in the iwconfig i/f 
	NDIS_MEDIA_STATE        PreMediaState;
#endif // CONFIG_STA_SUPPORT //



	UCHAR wmm_cw_min; /* CW_MIN_IN_BITS, actual CwMin = 2^CW_MIN_IN_BITS - 1 */
	UCHAR wmm_cw_max; /* CW_MAX_IN_BITS, actual CwMax = 2^CW_MAX_IN_BITS - 1 */


//=======Common===========
	// OP mode: either AP or STA
	UCHAR                   OpMode;                     // OPMODE_STA, OPMODE_AP
	
	struct wifi_dev **wdev_list;//[WDEV_NUM_MAX];
	UINT32 BssInfoIdxBitMap;/* mapping BssInfoIdx inside wdev struct with FW BssInfoIdx */
	
	NDIS_MEDIA_STATE        IndicateMediaState;			// Base on Indication state, default is NdisMediaStateDisConnected

#ifdef WSC_INCLUDED
#error"WSC Not Supported"
    unsigned int                   write_dat_file_pid;
    int                     time_to_die;
    //struct semaphore	    write_dat_file_semaphore;		/* to sleep thread on	*/
    //struct completion	    write_dat_file_notify;
    //RTMP_SEMAPHORE       	write_dat_file_semaphore;		/* to sleep thread on	*/
    //RTMP_COMPLETION  	    write_dat_file_notify;
	BOOLEAN             	WriteWscCfgToDatFile;
#endif // WSC_INCLUDED //

	// configuration: read from Registry & E2PROM
	BOOLEAN                 bLocalAdminMAC;             // Use user changed MAC
	UCHAR                   PermanentAddress[MAC_ADDR_LEN];    // Factory default MAC address
	UCHAR                   CurrentAddress[MAC_ADDR_LEN];      // User changed MAC address


	// ------------------------------------------------------
	// common configuration to both OPMODE_STA and OPMODE_AP
	// ------------------------------------------------------
	COMMON_CONFIG           CommonCfg;
	MLME_STRUCT             Mlme;

	// AP needs those vaiables for site survey feature.
	MLME_AUX                MlmeAux;           // temporary settings used during MLME state machine

#if 0// Raghav
	SCAN_CTRL ScanCtrl;
#endif

	//About MacTab, the sta driver will use #0 and #1 for multicast and AP.
	NDIS_SPIN_LOCK          MacTabLock;
	BSS_TABLE               ScanTab;           // store the latest SCAN result
	BSS_ENTRY               TmpEntry[2];           // Raghav: To avoid any corruption due to ScanTab overflow
	BA_TABLE			BATable;
	NDIS_SPIN_LOCK      BATabLock;
	RALINK_TIMER_STRUCT RECBATimer;
	// encryption/decryption KEY tables
		/* DOT11_H */
	DOT11_H Dot11_H;
	CIPHER_KEY              SharedKey[HW_BEACON_MAX_NUM /*+ MAX_P2P_NUM*/][4]; // STA always use SharedKey[0..3]
//  CIPHER_KEY              PairwiseKey[64];        // for AP only

		/* Maximum allowed tx software Queue length */
	UINT TxSwQMaxLen;
	struct tx_swq_fifo tx_swq[NUM_OF_TX_RING];

#ifdef MT_PS
	struct tx_swq_fifo apps_cr_q;
#endif



	// RX re-assembly buffer for fragmentation
	FRAGMENT_FRAME          FragFrame;                  // Frame storage for fragment frame
	
	// various Counters 
	COUNTER_802_3           Counters8023;               // 802.3 counters
	COUNTER_802_11          WlanCounters;               // 802.11 MIB counters
	COUNTER_RALINK          RalinkCounters;             // Ralink propriety counters
	COUNTER_DRS             DrsCounters;                // counters for Dynamic TX Rate Switching
	PRIVATE_STRUC           PrivateInfo;                // Private information & counters


	// flags, see fRTMP_ADAPTER_xxx flags
	ULONG           Flags;                      // Represent current device status
	ULONG 		SetFlagLog;  // for record changes of OpStatusFlags
	ULONG 		ClearFlagLog;  // for record changes of OpStatusFlags

	// current TX sequence #
	USHORT                  Sequence;
	//+++Used only for Station
	BOOLEAN                 bConfigChanged;         // Config Change flag for the same SSID setting
	ULONG                   ExtraInfo;              // Extra information for displaying status

	BOOLEAN bBroadComHT;

	struct reordering_mpdu_pool mpdu_blk_pool;

	ULONG					OneSecondnonBEpackets;		// record non BE packets per second 
	ULONG					OneSecondTxpackets;

#if WIRELESS_EXT >= 12
    struct iw_statistics    iw_stats;
#endif

	ULONG					TbttTickCount;

#ifdef WSC_INCLUDED
    // for multiple card
    UCHAR	                *pHmacData;
    UCHAR                   Wsc_Uuid_E[UUID_LEN_HEX];
    UCHAR                   Wsc_Uuid_Str[UUID_LEN_STR];
		
    RALINK_TIMER_STRUCT		EapolTimer;
    RALINK_TIMER_STRUCT		WscPBCTimer;
    RALINK_TIMER_STRUCT		WscScanTimer;
	RALINK_TIMER_STRUCT		Wsc2MinsTimer;
#endif // WSC_INCLUDED //

#ifdef IOT_CFG80211_SUPPORT
	CFG80211_CTRL cfg80211_ctrl;
	VOID *pCfg80211_CB;
#endif /* IOT_CFG80211_SUPPORT */

#ifdef IOT_PROMISCUS_MODE
	MONITOR_STRUCT monitor_ctrl;
#endif /* IOT_PROMISCUS_MODE */

#ifdef IOT_WOW_SUPPORT
	WOW_CFG_STRUCT WOW_Cfg; /* data structure for wake on wireless */
#endif /* (defined(WOW_SUPPORT) && defined(RTMP_MAC_USB)) || defined(NEW_WOW_SUPPORT) || defined(MT_WOW_SUPPORT) */

	USHORT SqEnqNormalMax;
	USHORT MaxPacketsInMcastPsQueue;
	USHORT MaxPacketsInPsQueue;
	USHORT MaxPacketsInQueue;
	USHORT MaxTxProcess;
	USHORT TxRingSize;
	/* MAT related parameters */
       PVOID   CookieExt; /* reserved for extension */
    
} RTMP_ADAPTER, *PRTMP_ADAPTER;
#ifndef WIFI_BUILD_RAM
typedef struct _EXT_CMD_BCN_UPDATE_T
{
    UINT_8 ucOwnMacIdx;
    UINT_8 ucEnable;
    UINT_8 ucWlanIdx;
    UINT_8 ucBandIdx;//0: band 0, 1: band 1

    UINT_8 ucPktType;//0: Bcn, 1: Tim Frame.
    UINT_8 aucReserve[3];

    UINT_16 u2PktLength;
    UINT_16 u2TimIePos;//Tim IE position in pkt.

    UINT_8 acPktContent[512];//whole pkt template length which include TXD, max shall not exceed 512 bytes.
} EXT_CMD_BCN_UPDATE_T, *P_EXT_CMD_BCN_UPDATE_T;
#endif

#ifdef MT_MAC
INT wdev_tim_buf_init(RTMP_ADAPTER *pAd, TIM_BUF_STRUC *tim_info);
#endif


//
//  Macros for flag and ref count operations
//

#define RTMP_SET_FLAG(_M, _F)       ((_M)->Flags |= (_F))
#define RTMP_CLEAR_FLAG(_M, _F)     ((_M)->Flags &= ~(_F))
#define RTMP_CLEAR_FLAGS(_M)        ((_M)->Flags = 0)
#define OPSTATUS_SET_FLAG(_pAd, _F)     ((_pAd)->CommonCfg.OpStatusFlags |= (_F))
#define OPSTATUS_CLEAR_FLAG(_pAd, _F)   ((_pAd)->CommonCfg.OpStatusFlags &= ~(_F))
#define CLIENT_STATUS_SET_FLAG(_pEntry,_F)      ((_pEntry)->ClientStatusFlags |= (_F))
#define CLIENT_STATUS_CLEAR_FLAG(_pEntry,_F)    ((_pEntry)->ClientStatusFlags &= ~(_F))
#define RTMP_TEST_FLAG(_M, _F)      (((_M)->Flags & (_F)) != 0)
#define RTMP_TEST_FLAGS(_M, _F)     (((_M)->Flags & (_F)) == (_F))
#define OPSTATUS_TEST_FLAG(_pAd, _F)    (((_pAd)->CommonCfg.OpStatusFlags & (_F)) != 0)
#ifndef WIFI_BUILD_RAM	// Workaround to fix RAM build issue: Raju
#define CLIENT_STATUS_TEST_FLAG(_pEntry,_F)     (((_pEntry)->ClientStatusFlags & (_F)) != 0)
#endif

#define RTMP_SET_MORE_FLAG(_M, _F)       ((_M)->MoreFlags |= (_F))
#define RTMP_TEST_MORE_FLAG(_M, _F)      (((_M)->MoreFlags & (_F)) != 0)
#define RTMP_CLEAR_MORE_FLAG(_M, _F)     ((_M)->MoreFlags &= ~(_F))



#define INFRA_ON(_p)                (OPSTATUS_TEST_FLAG(_p, fOP_STATUS_INFRA_ON))
#define ADHOC_ON(_p)                (OPSTATUS_TEST_FLAG(_p, fOP_STATUS_ADHOC_ON))

#ifdef IOT_PROMISCUS_MODE
#define MONITOR_ON(_p)              (((_p)->monitor_ctrl.bMonitorOn) == TRUE)
#else
#define MONITOR_ON(_p)              (((_p)->StaCfg.BssType) == BSS_MONITOR)
#endif /* IOT_PROMISCUS_MODE */

#define IDLE_ON(_p)                 (!INFRA_ON(_p) && !ADHOC_ON(_p))

// Check LEAP & CCKM flags
#define LEAP_ON(_p)                 (((_p)->StaCfg.LeapAuthMode) == CISCO_AuthModeLEAP)
#define LEAP_CCKM_ON(_p)            ((((_p)->StaCfg.LeapAuthMode) == CISCO_AuthModeLEAP) && ((_p)->StaCfg.LeapAuthInfo.CCKM == TRUE))

#if 0
//
// Cisco IAPP format
//
typedef struct  _CISCO_IAPP_CONTENT_
{
	USHORT     Length;        //IAPP Length
	UCHAR      MessageType;      //IAPP type
	UCHAR      FunctionCode;     //IAPP function type
	UCHAR      DestinaionMAC[MAC_ADDR_LEN];   
	UCHAR      SourceMAC[MAC_ADDR_LEN];
	USHORT     Tag;           //Tag(element IE) - Adjacent AP report
	USHORT     TagLength;     //Length of element not including 4 byte header
	UCHAR      OUI[4];           //0x00, 0x40, 0x96, 0x00
	UCHAR      PreviousAP[MAC_ADDR_LEN];       //MAC Address of access point
	USHORT     Channel;       
	USHORT     SsidLen;
	CHAR      Ssid[MAX_LEN_OF_SSID];
	USHORT     Seconds;          //Seconds that the client has been disassociated.
} CISCO_IAPP_CONTENT, *PCISCO_IAPP_CONTENT;
#endif

#define DELAYINTMASK		0x0003fffb
#define INTMASK				0x0003fffb
#define IndMask				0x0003fffc
#define RxINT				0x00000005	// Delayed Rx or indivi rx
#define TxDataInt			0x000000fa	// Delayed Tx or indivi tx
#define TxMgmtInt			0x00000102	// Delayed Tx or indivi tx 
#define TxCoherent			0x00020000	// tx coherent 
#define RxCoherent			0x00010000	// rx coherent
#define McuCommand			0x00000200	// mcu
#define PreTBTTInt			0x00001000	// Pre-TBTT interrupt
#define TBTTInt				0x00000800		// TBTT interrupt
#define BCN_DONE_INT            0x01000000           // BCN_DONE interrupt
#define BCN_DONE_CRC_INT    0x02000000           // BCN_DONE_CRC interrupt
#define BCN_NODTIM_INT        0x04000000           // BCN_NODTIM interrupt
#define BCN_NODTIM_CRC_INT 0x08000000          // BCN_NODTIM_CRC interrupt
#define BCN_START_INT           0x10000000           // BCN_START interrupt
#define MAIN_CCA_END_INT    0x20000000           // MAIN CCA END interrupt
#define GPTimeOutInt			0x00008000		// GPtimeout interrupt
#define AutoWakeupInt		0x00004000		// AutoWakeupInt interrupt
#define FifoStaFullInt			0x00002000	//  fifo statistics full interrupt




#define RX_BLK_SET_FLAG(_pRxBlk, _flag)		(_pRxBlk->Flags |= _flag)
#define RX_BLK_TEST_FLAG(_pRxBlk, _flag)	(_pRxBlk->Flags & _flag)
#define RX_BLK_CLEAR_FLAG(_pRxBlk, _flag)	(_pRxBlk->Flags &= ~(_flag))


typedef enum RX_BLK_FLAGS{
	fRX_AMPDU = (1 << 0),
	fRX_AMSDU = (1 << 1),
	fRX_ARALINK = (1 << 2),
	fRX_HTC = (1 << 3),
	fRX_PAD = (1 << 4),
	fRX_QOS = (1 << 5),
	fRX_EAP = (1 << 6),
	fRX_WPI = (1 << 7),
	fRX_AP = (1 << 8),			// Packet received from AP
	fRX_STA = (1 << 9),			// Packet received from Client(Infra mode)
	fRX_ADHOC = (1 << 10),		// packet received from AdHoc peer
	fRX_WDS = (1 << 11),		// Packet received from WDS
	fRX_MESH = (1 << 12),		// Packet received from MESH
	fRX_DLS = (1 << 13),		// Packet received from DLS peer
	fRX_TDLS = (1 << 14),		// Packet received from TDLS peer
	fRX_RETRIEVE = (1 << 15), 	// Packet received from mcu
	fRX_CMD_RSP = (1 << 16),	// Pakket received from mcu command response
	fRX_TXRX_RXV = (1 << 17),   // RxV received from Rx Ring1
	fRX_HDR_TRANS = (1 << 18),
}RX_BLK_FLAGS;

#define LENGTH_AMSDU_SUBFRAMEHEAD	14
#define LENGTH_ARALINK_SUBFRAMEHEAD	14
#define LENGTH_ARALINK_HEADER_FIELD	 2

#define TX_UNKOWN_FRAME			0x00
#define TX_MCAST_FRAME			0x01
#define TX_LEGACY_FRAME			0x02
#define TX_AMPDU_FRAME			0x04
#define TX_AMSDU_FRAME			0x08
#define TX_RALINK_FRAME			0x10
#define TX_FRAG_FRAME			0x20




#define fTX_bRtsRequired		0x0001	// Indicate if need send RTS frame for protection. Not used in RT2860/RT2870.
#define fTX_bAckRequired       	0x0002	// the packet need ack response
#define fTX_bPiggyBack     		0x0004	// Legacy device use Piggback or not
#define fTX_bHTRate         	0x0008	// allow to use HT rate
//#define fTX_bForceLowRate       0x0010	// force to use Low Rate 
#define fTX_bForceNonQoS       	0x0010	// force to transmit frame without WMM-QoS in HT mode 
#define fTX_bAllowFrag       	0x0020	// allow to fragment the packet, A-MPDU, A-MSDU, A-Ralink is not allowed to fragment
#define fTX_bMoreData			0x0040	// there are more data packets in PowerSave Queue
#define fTX_bWMM				0x0080	// QOS Data

#define fTX_bClearEAPFrame		0x0100

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
#define fTX_bApCliPacket		0x0200
#define	fTX_bClearEAPoLFrame	0x0400	// Used when APCLI_SUPPORT
#endif // APCLI_SUPPORT //

#ifdef UAPSD_AP_SUPPORT
#define	fTX_bWMM_UAPSD_EOSP		0x0800	// Used when UAPSD_SUPPORT
#endif // UAPSD_AP_SUPPORT //

#ifdef WDS_SUPPORT
#define fTX_bWDSEntry			0x1000	// Used when WDS_SUPPORT
#endif // WDS_SUPPORT

#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
#ifdef ETH_CONVERT_SUPPORT
#define	fTX_bDonglePkt			0x2000	// Used when ETH_CONVERT_SUPPORT
#endif // ETH_CONVERT_SUPPORT //
#endif // CONFIG_STA_SUPPORT //


#define TX_BLK_ASSIGN_FLAG(_pTxBlk, _flag, value)	\
		do {										\
			if (value) 								\
				(_pTxBlk->Flags |= _flag) 			\
			else 									\
				(_pTxBlk->Flags &= ~(_flag))		\
		}while(0)
		
#define TX_BLK_SET_FLAG(_pTxBlk, _flag)		(_pTxBlk->Flags |= _flag)
#define TX_BLK_TEST_FLAG(_pTxBlk, _flag)	(((_pTxBlk->Flags & _flag) == _flag) ? 1 : 0)
#define TX_BLK_CLEAR_FLAG(_pTxBlk, _flag)	(_pTxBlk->Flags &= ~(_flag))
	




//------------------------------------------------------------------------------------------


//
// Enable & Disable NIC interrupt via writing interrupt mask register
// Since it use ADAPTER structure, it have to be put after structure definition.
//
BOOLEAN RTMPCheckForHang(
	IN  NDIS_HANDLE MiniportAdapterContext
	);

VOID  RTMPHalt(
	IN  NDIS_HANDLE MiniportAdapterContext
	);


//
//  Private routines in rtmp_init.c
//

NDIS_STATUS RTMPAllocTxRxRingMemory(
	IN  PRTMP_ADAPTER   pAd
	);

NDIS_STATUS RTMPFindAdapter(
	IN  PRTMP_ADAPTER   pAd,
	IN  NDIS_HANDLE     WrapperConfigurationContext
	);

NDIS_STATUS	RTMPReadParametersHook(
	IN	PRTMP_ADAPTER pAd
	);


VOID RTMPRingCleanUp(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR           RingType);

VOID RxTest(
	IN  PRTMP_ADAPTER   pAd);

NDIS_STATUS DbgSendPacket(
	IN  PRTMP_ADAPTER   pAd,
	IN  PNDIS_PACKET    pPacket);


VOID NICResetFromError(
	IN  PRTMP_ADAPTER   pAd);

VOID NICEraseFirmware(
	IN PRTMP_ADAPTER pAd);

NDIS_STATUS NICLoadFirmware(
	IN  PRTMP_ADAPTER   pAd);

NDIS_STATUS NICLoadRateSwitchingParams(
	IN PRTMP_ADAPTER pAd);

BOOLEAN NICCheckForHang(
	IN  PRTMP_ADAPTER   pAd);

VOID RTMPPatchMacBbpBug(
	IN  PRTMP_ADAPTER   pAd);

VOID RTMPPatchCardBus(
	IN	PRTMP_ADAPTER	pAdapter);

VOID RTMPPatchRalinkCardBus(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	ULONG			Bus);

ULONG RTMPReadCBConfig(
	IN	ULONG	Bus,
	IN	ULONG	Slot,
	IN	ULONG	Func,
	IN	ULONG	Offset);

VOID RTMPWriteCBConfig(
	IN	ULONG	Bus,
	IN	ULONG	Slot,
	IN	ULONG	Func,
	IN	ULONG	Offset,
	IN	ULONG	Value);
//
// prototype in action.c
//
#if 0
VOID PeerAddBAReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID PeerAddBARspAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID PeerDelBAAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
#endif

VOID SendPSMPAction(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			Wcid,
	IN UCHAR			Psmp);

VOID PeerRMAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID PeerBSSTranAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

#ifdef CONFIG_AP_SUPPORT
#ifdef QOS_DLS_SUPPORT
VOID PeerDLSAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
#endif // QOS_DLS_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

VOID RECBATimerTimeout(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3);

BOOLEAN QosBADataParse(
	IN PRTMP_ADAPTER	pAd, 
	IN BOOLEAN bAMSDU,
	IN PUCHAR p8023Header,
	IN UCHAR	WCID,
	IN UCHAR	TID,
	IN USHORT Sequence,
	IN UCHAR DataOffset, 
	IN USHORT Datasize,
	IN UINT        CurRxIndex);
#if 0
BOOLEAN CntlEnqueueForRecv(
    IN	PRTMP_ADAPTER	pAd, 
	IN ULONG Wcid, 
    IN ULONG MsgLen, 
	IN PFRAME_BA_REQ pMsg);
#endif
VOID BaAutoManSwitch(
	IN	PRTMP_ADAPTER	pAd);

VOID HTIOTCheck(
	IN	PRTMP_ADAPTER	pAd,
	IN    UCHAR     BatRecIdx);

//
// Private routines in rtmp_data.c
//
BOOLEAN RTMPHandleRxDoneInterrupt(
	IN  PRTMP_ADAPTER   pAd);

#if 0 // not MT7662
BOOLEAN RTMPHandleTxRingDmaDoneInterrupt(
	IN  PRTMP_ADAPTER   pAd,
	IN  INT_SOURCE_CSR_STRUC TxRingBitmap);
#endif
VOID RTMPHandleMgmtRingDmaDoneInterrupt(
	IN  PRTMP_ADAPTER   pAd);

extern VOID (*RTMPHandleTBTTInterrupt)(
	IN  PRTMP_ADAPTER   pAd);

extern VOID (*RTMPHandlePreTBTTInterrupt)(
	IN  PRTMP_ADAPTER   pAd);
#if 0
void RTMPHandleTwakeupInterrupt(
	IN PRTMP_ADAPTER pAd);
#endif
VOID	RTMPHandleRxCoherentInterrupt(
	IN	PRTMP_ADAPTER	pAd);

BOOLEAN TxFrameIsAggregatible(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pPrevAddr1,
	IN  PUCHAR          p8023hdr);

BOOLEAN PeerIsAggreOn(
    IN  PRTMP_ADAPTER   pAd,
    IN  ULONG          TxRate,
    IN  PMAC_TABLE_ENTRY pMacEntry);

#if 0	// It's not be used
HTTRANSMIT_SETTING  *GetTxMode(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk);
#endif

NDIS_STATUS	RTMPHardTransmit(
	IN PRTMP_ADAPTER	pAd,
	IN PNDIS_PACKET		pPacket,
	IN  UCHAR			QueIdx,
	OUT	PULONG			pFreeTXDLeft);


NDIS_STATUS RTMPFreeTXDRequest(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR           RingType,
	IN  UCHAR           NumberRequired,
	IN 	PUCHAR          FreeNumberIs);

VOID RTMPSendDisassociationFrame(
	IN	PRTMP_ADAPTER	pAd);

VOID RTMPSendRTSFrame(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pDA,
	IN	unsigned int	NextMpduSize,
	IN  UCHAR           TxRate,
	IN  UCHAR           RTSRate,
	IN  USHORT          AckDuration,
	IN  UCHAR           QueIdx,
	IN  UCHAR			FrameGap);


PQUEUE_HEADER   RTMPCheckTxSwQueue(
	IN  PRTMP_ADAPTER   pAd,
	OUT UCHAR           *QueIdx);

BOOLEAN RTMPFreeTXDUponTxDmaDone(
	IN PRTMP_ADAPTER    pAd, 
	IN UCHAR            QueIdx);

BOOLEAN RTMPCheckDHCPFrame(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket);


VOID RTMPCckBbpTuning(
	IN	PRTMP_ADAPTER	pAd, 
	IN	UINT			TxRate);

//
// Private routines in rtmp_wep.c
//
#if 0
VOID RTMPInitWepEngine(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pKey,
	IN  UCHAR           KeyId,
	IN  UCHAR           KeyLen, 
	IN  PUCHAR          pDest);

VOID RTMPEncryptData(
	IN	PRIVATE_STRUC	*pPrivateInfo,
	IN  PUCHAR          pSrc,
	IN  PUCHAR          pDest,
	IN  UINT            Len);

BOOLEAN	RTMPDecryptData(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	PUCHAR			pSrc,
	IN	UINT			Len,
	IN	UINT			idx);

VOID RTMPSetICV(
	IN	PRIVATE_STRUC	*pPrivateInfo,
	IN  PUCHAR          pDest);

VOID ARCFOUR_INIT(
	IN  PARCFOURCONTEXT Ctx,
	IN  PUCHAR          pKey,
	IN  UINT            KeyLen);

UCHAR   ARCFOUR_BYTE(
	IN  PARCFOURCONTEXT     Ctx);

VOID ARCFOUR_DECRYPT(
	IN  PARCFOURCONTEXT Ctx,
	IN  PUCHAR          pDest, 
	IN  PUCHAR          pSrc,
	IN  UINT            Len);

VOID ARCFOUR_ENCRYPT(
	IN  PARCFOURCONTEXT Ctx,
	IN  PUCHAR          pDest,
	IN  PUCHAR          pSrc,
	IN  UINT            Len);

VOID WPAARCFOUR_ENCRYPT(
	IN  PARCFOURCONTEXT Ctx,
	IN  PUCHAR          pDest,
	IN  PUCHAR          pSrc,
	IN  UINT            Len);

UINT RTMP_CALC_FCS32(
	IN  UINT   Fcs,
	IN  PUCHAR  Cp,
	IN  INT     Len);
#endif
//
// MLME routines
//



/*
	==========================================================================
	Description:
		This function is required for 2421 only, and should not be used during
		site survey. It's only required after NIC decided to stay at a channel
		for a longer period.
		When this function is called, it's always after AsicSwitchChannel().

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	==========================================================================
 */

/*
VOID AsicLockChannel(
	IN PRTMP_ADAPTER pAd, 
	IN UCHAR Channel) ;
*/
#define AsicLockChannel(_x, _y)


VOID AsicSetMcastWC(
	IN PRTMP_ADAPTER pAd);


VOID BATableDeleteORIEntry(
	IN OUT	PRTMP_ADAPTER pAd, 
	IN		BA_ORI_ENTRY	*pBAORIEntry);

VOID BATableDeleteRECEntry(
	IN OUT	PRTMP_ADAPTER pAd, 
	IN		BA_REC_ENTRY	*pBARECEntry);

VOID BATableTearORIEntry(
	IN OUT	PRTMP_ADAPTER pAd, 
	IN		UCHAR TID, 
	IN		UCHAR Wcid, 
	IN		BOOLEAN bForceDelete, 
	IN		BOOLEAN ALL);

VOID BATableTearRECEntry(
	IN OUT	PRTMP_ADAPTER pAd, 
	IN		UCHAR TID, 
	IN		UCHAR WCID, 
	IN		BOOLEAN ALL);



//----------------------------------------------


VOID  Cls3errAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PUCHAR pAddr);


#ifdef RT2870
VOID MlmeCntlConfirm(
	IN PRTMP_ADAPTER pAd, 
	IN ULONG MsgType, 
	IN USHORT Msg);
#endif // RT2870 //

VOID PeerAuthSimpleRspGenAndSend(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PHEADER_802_11  pHdr80211, 
	IN  USHORT Alg, 
	IN  USHORT Seq, 
	IN  USHORT Reason, 
	IN  USHORT Status);

//
// Private routines in dls.c
//
VOID DLSStateMachineInit(
    IN	PRTMP_ADAPTER		pAd, 
    IN  STATE_MACHINE		*S, 
    OUT STATE_MACHINE_FUNC	Trans[]);

VOID PeerDlsReqAction(
    IN PRTMP_ADAPTER	pAd, 
    IN MLME_QUEUE_ELEM	*Elem);

VOID PeerDlsRspAction(
    IN PRTMP_ADAPTER	pAd, 
    IN MLME_QUEUE_ELEM	*Elem);

VOID PeerDlsTearDownAction(
    IN PRTMP_ADAPTER	pAd, 
    IN MLME_QUEUE_ELEM	*Elem);

//========================================



//=========================================

typedef struct __BeaconProbeRspStr
{
	UCHAR					Addr2[MAC_ADDR_LEN];
	UCHAR					Bssid[MAC_ADDR_LEN];
	CHAR 					Ssid[MAX_LEN_OF_SSID];
	UCHAR 					SsidLen;
	UCHAR 					BssType;
	USHORT 					BeaconPeriod;
	UCHAR					Channel;
	UCHAR					NewChannel;
	LARGE_INTEGER			Timestamp;
	CF_PARM					CfParm;
	USHORT					AtimWin;
	USHORT					CapabilityInfo;
	UCHAR					Erp;
	UCHAR					DtimCount;
	UCHAR					DtimPeriod;
	UCHAR					BcastFlag;
	UCHAR					MessageToMe;
	UCHAR					SupRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR					SupRateLen;
	UCHAR					ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR					ExtRateLen;
	UCHAR					CkipFlag;
	UCHAR					AironetCellPowerLimit;
	EDCA_PARM      			EdcaParm;
	QBSS_LOAD_PARM			QbssLoad;
	QOS_CAPABILITY_PARM 	QosCapability;
	ULONG				 	RalinkIe;
	UCHAR				 	HtCapabilityLen;
	UCHAR					PreNHtCapabilityLen;
	HT_CAPABILITY_IE		HtCapability;
	EXT_CAP_INFO_ELEMENT ExtCapInfo;
	UCHAR					AddHtInfoLen;
	ADD_HT_INFO_IE			AddHtInfo;
	UCHAR					NewExtChannelOffset;
	USHORT					LengthVIE;
	UCHAR					VarIE[MAX_VIE_LEN];
} BeaconProbeRspStr_t;

VOID RaiseClock(
	IN  PRTMP_ADAPTER   pAd,
	IN  UINT32 *x);

VOID LowerClock(
	IN  PRTMP_ADAPTER   pAd,
	IN  UINT32 *x);

USHORT ShiftInBits(
	IN  PRTMP_ADAPTER   pAd);

VOID ShiftOutBits(
	IN  PRTMP_ADAPTER   pAd,
	IN  USHORT data,
	IN  USHORT count);

VOID EEpromCleanup(
	IN  PRTMP_ADAPTER   pAd);

VOID EWDS(
	IN  PRTMP_ADAPTER   pAd);

VOID EWEN(
	IN  PRTMP_ADAPTER   pAd);

USHORT RTMP_EEPROM_READ16(
	IN  PRTMP_ADAPTER   pAd,
	IN  USHORT Offset);

VOID RTMP_EEPROM_WRITE16(
	IN  PRTMP_ADAPTER   pAd,
	IN  USHORT Offset,
	IN  USHORT Data);

#if 1
UCHAR eFuseReadRegisters(
	IN	PRTMP_ADAPTER	pAd, 
	IN	USHORT Offset, 
	IN	USHORT Length, 
	OUT	USHORT* pData);

UINT32 eFuseWriteRegisters(
	IN	PRTMP_ADAPTER	pAd, 
	IN	USHORT Offset, 
	IN	USHORT Length, 
	IN	USHORT* pData);
#endif
//
// Prototypes of function definition in rtmp_tkip.c
//
#if 0
VOID    RTMPInitTkipEngine(
	IN  PUCHAR          pTKey,
	IN  UCHAR           KeyId,
	IN  PUCHAR          pTA,
	IN  PUCHAR          pMICKey,
	IN  PUCHAR          pTSC,
	OUT PULONG          pIV16,
	OUT PULONG          pIV32);

VOID    RTMPInitMICEngine(
	IN	TKIP_KEY_INFO   	*pRx_Key,	
	IN  PUCHAR          pKey,
	IN  PUCHAR          pDA,
	IN  PUCHAR          pSA,
	IN  UCHAR           UserPriority,
	IN  PUCHAR          pMICKey);

BOOLEAN RTMPTkipCompareMICValue(
	IN  TKIP_KEY_INFO  	*pRx_Key,
	IN  PUCHAR          pSrc,
	IN  PUCHAR          pDA,
	IN  PUCHAR          pSA,
	IN  PUCHAR          pMICKey,
	IN	UCHAR			UserPriority,
	IN  UINT            Len);

VOID    RTMPCalculateMICValue(
	IN  TKIP_KEY_INFO   *pTx,
	IN  PNDIS_PACKET    pPacket,
	IN  PUCHAR          pEncap,
	IN  PCIPHER_KEY     pKey,
	IN	UCHAR			apidx);

BOOLEAN RTMPTkipCompareMICValueWithLLC(
	IN	TKIP_KEY_INFO  	*pRx_Key,
	IN  PUCHAR          pLLC,
	IN  PUCHAR          pSrc,
	IN  PUCHAR          pDA,
	IN  PUCHAR          pSA,
	IN  PUCHAR          pMICKey,
	IN  UINT            Len);

VOID    RTMPTkipAppendByte( 
	IN  PTKIP_KEY_INFO  pTkip,  
	IN  UCHAR           uChar);

VOID    RTMPTkipAppend( 
	IN  PTKIP_KEY_INFO  pTkip,  
	IN  PUCHAR          pSrc,
	IN  UINT            nBytes);

VOID    RTMPTkipGetMIC( 
	IN  PTKIP_KEY_INFO  pTkip);

BOOLEAN RTMPSoftDecryptTKIP(
	IN TKIP_KEY_INFO   	*pRx_Key,
	IN PUCHAR	pData,
	IN ULONG	DataByteCnt, 
	IN UCHAR    UserPriority,
	IN PCIPHER_KEY	pWpaKey);

BOOLEAN RTMPSoftDecryptAES(
	IN PUCHAR	pData,
	IN ULONG	DataByteCnt, 
	IN PCIPHER_KEY	pWpaKey);
#endif
#if 0	// removed by AlbertY
NDIS_STATUS RTMPWPAAddKeyProc(
	IN  PRTMP_ADAPTER   pAd,
	IN  PVOID           pBuf);
#endif

//
// Prototypes of function definition in cmm_info.c
//
VOID RTMPIoctlGetSiteSurvey(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq);
/*
VOID RTMPIoctlGetMacTable(
	IN PRTMP_ADAPTER pAd, 
	IN struct ifreq *wrq);
*/
VOID RTMPIndicateWPA2Status(
	IN  PRTMP_ADAPTER  pAdapter);

VOID	RTMPOPModeSwitching(
	IN	PRTMP_ADAPTER	pAd);

#ifdef CONFIG_STA_SUPPORT
VOID    RTMPAddBSSIDCipher(
    IN  PRTMP_ADAPTER   pAd,
	IN	UCHAR	Aid,
    IN  PNDIS_802_11_KEY    pKey,
    IN  UCHAR   CipherAlg);
#endif
VOID	NICUpdateCntlCounters(
	IN	PRTMP_ADAPTER	pAd,
	IN	PHEADER_802_11	pHeader,
	IN    UCHAR			SubType,
	IN	PRXWI_STRUC 	pRxWI);

extern VOID (*RtmpEnqueueNullFrame)(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR *pAddr,
	IN UCHAR TxRate,
	IN UCHAR AID,
	IN UCHAR apidx,
	IN BOOLEAN bQosNull,
	IN BOOLEAN bEOSP,
	IN UCHAR OldUP);



BOOLEAN ParseKeyData(
	IN  WPACfg_t		*pStaWPACfg,
	IN	ApRSNIE_t		*pApRSNIE,    
	IN  PUCHAR          pKeyData,
    IN  UCHAR           KeyDataLen,
	IN	UCHAR			bPairewise);


VOID    CCKMPRF(
	IN  UCHAR   *key,
	IN  INT     key_len,
	IN  UCHAR   *data,
	IN  INT     data_len,
	OUT UCHAR   *output,
	IN  INT     len);

//
// prototype in aironet.c
//
VOID    AironetStateMachineInit(
	IN  PRTMP_ADAPTER       pAd, 
	IN  STATE_MACHINE       *S, 
	OUT STATE_MACHINE_FUNC  Trans[]);

VOID    AironetMsgAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID    AironetRequestAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID    ChannelLoadRequestAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index);

VOID    NoiseHistRequestAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index);

VOID    BeaconRequestAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index);

VOID    AironetReportAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID    ChannelLoadReportAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index);

VOID    NoiseHistReportAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index);

VOID    AironetFinalReportAction(
	IN  PRTMP_ADAPTER   pAd);

VOID    BeaconReportAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index); 

VOID    AironetAddBeaconReport(
	IN  PRTMP_ADAPTER       pAd,
	IN  ULONG               Index,
	IN  PMLME_QUEUE_ELEM    pElem);

VOID    AironetCreateBeaconReportFromBssTable(
	IN  PRTMP_ADAPTER       pAd);

VOID    DBGPRINT_TX_RING(
	IN PRTMP_ADAPTER  pAd,
	IN UCHAR          QueIdx);

VOID DBGPRINT_RX_RING(
	IN PRTMP_ADAPTER  pAd);

VOID APAsicEvaluateRxAnt(
	IN PRTMP_ADAPTER	pAd);


VOID APAsicRxAntEvalTimeout(
	IN PRTMP_ADAPTER	pAd);

// 
// function prototype in cmm_wpa.c
//

// 
// function prototype in ap_wpa.c
//

BOOLEAN APWpaMsgTypeSubst(
	IN UCHAR    EAPType,
	OUT INT *MsgType) ;

VOID APWpaEAPPacketAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID APWpaEAPOLStartAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID APWpaEAPOLLogoffAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID APWpaEAPOLKeyAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID APWpaEAPOLASFAlertAction(
	IN  PRTMP_ADAPTER    pAd, 
	IN  MLME_QUEUE_ELEM  *Elem);

VOID HandleCounterMeasure(
	IN PRTMP_ADAPTER pAd, 
	IN MAC_TABLE_ENTRY  *pEntry);


VOID RTMPHandleSTAKey(
    IN PRTMP_ADAPTER    pAdapter, 
    IN MAC_TABLE_ENTRY  *pEntry,
    IN MLME_QUEUE_ELEM  *Elem);

#if 0 // merge into PeerPairMsg4Action
VOID Wpa1PeerPairMsg4Action(
	IN PRTMP_ADAPTER pAd, 
	IN MAC_TABLE_ENTRY  *pEntry,
	IN MLME_QUEUE_ELEM *Elem);

VOID Wpa2PeerPairMsg4Action(
	IN  PRTMP_ADAPTER    pAd, 
	IN  PMAC_TABLE_ENTRY pEntry,
	IN  MLME_QUEUE_ELEM  *Elem);
#endif // 0 //


#if 0	// replaced by WPAStart2WayGroupHS
NDIS_STATUS APWpaHardTransmit(
	IN  PRTMP_ADAPTER    pAd,
	IN  PMAC_TABLE_ENTRY pEntry);
#endif // 0 //

VOID PairDisAssocAction(
	IN  PRTMP_ADAPTER    pAd, 
	IN  PMAC_TABLE_ENTRY pEntry,
	IN  USHORT           Reason);

VOID DisAssocAction(
	IN  PRTMP_ADAPTER    pAd, 
	IN  PMAC_TABLE_ENTRY pEntry,
	IN  USHORT           Reason);

VOID    GetSmall(
	IN  PVOID   pSrc1,
	IN  PVOID   pSrc2,
	OUT PUCHAR  out,
	IN  ULONG   Length);

VOID    GetLarge(
	IN  PVOID   pSrc1,
	IN  PVOID   pSrc2,
	OUT PUCHAR  out,
	IN  ULONG   Length);

VOID APGenRandom(
	IN PRTMP_ADAPTER pAd, 
	OUT UCHAR       *random);

VOID    WpaSend(
    IN  PRTMP_ADAPTER   pAdapter,
    IN  PUCHAR          pPacket,
    IN  ULONG           Len);

VOID    APToWirelessSta(
	IN  PRTMP_ADAPTER   pAd,
	IN  MAC_TABLE_ENTRY *pEntry,
	IN  PUCHAR          pHeader802_3,
	IN  UINT            HdrLen,
	IN  PUCHAR          pData,
	IN  UINT            DataLen,
    IN	BOOLEAN			bClearFrame);

VOID RTMPAddPMKIDCache(
	IN  PRTMP_ADAPTER   		pAd,
	IN	INT						apidx,
	IN	PUCHAR				pAddr,
	IN	UCHAR					*PMKID,
	IN	UCHAR					*PMK);

INT RTMPSearchPMKIDCache(
	IN  PRTMP_ADAPTER   pAd,
	IN	INT				apidx,
	IN	PUCHAR		pAddr);

VOID RTMPDeletePMKIDCache(
	IN  PRTMP_ADAPTER   pAd,
	IN	INT				apidx,
	IN  INT				idx);

VOID RTMPMaintainPMKIDCache(
	IN  PRTMP_ADAPTER   pAd);

VOID	RTMPSendTriggerFrame(
	IN	PRTMP_ADAPTER	pAd,
	IN	PVOID			pBuffer,
	IN	ULONG			Length,
	IN  UCHAR           TxRate,
	IN	BOOLEAN			bQosNull);


//typedef void (*TIMER_FUNCTION)(unsigned long);


/* timeout -- ms */
#if 0
VOID Indicate_AMPDU_Packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
#endif
void RTMP_AllocateSharedMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

VOID RTMPFreeTxRxRingMemory(
    IN  PRTMP_ADAPTER   pAd);

NDIS_STATUS AdapterBlockAllocateMemory(
	OUT	PVOID	*ppAd);

void RTMP_AllocateTxDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	UINT	Index,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

void RTMP_AllocateFirstTxBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	UINT	Index,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

void RTMP_AllocateMgmtDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

void RTMP_AllocateRxDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

PNDIS_PACKET RTMP_AllocateRxPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

PNDIS_PACKET RTMP_AllocateTxPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress);

PNDIS_PACKET RTMP_AllocateFragPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length);

void RTMP_QueryNextPacketInfo(
	IN  PNDIS_PACKET *ppPacket,
	OUT PACKET_INFO  *pPacketInfo,
	OUT PUCHAR		 *pSrcBufVA,
	OUT	UINT		 *pSrcBufLen);


PRTMP_SCATTER_GATHER_LIST
rt_get_sg_list_from_packet(PNDIS_PACKET pPacket, RTMP_SCATTER_GATHER_LIST *sg);

#if 0
UINT BA_Reorder_AMSDU_Annnounce(
	IN	PRTMP_ADAPTER	pAd, 	
	IN	PNDIS_PACKET	pPacket);


UINT Handle_AMSDU_Packet(
	IN	PRTMP_ADAPTER	pAd, 	
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN  UCHAR			FromWhichBSSID);


void convert_802_11_to_802_3_packet(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket,
	IN	PUCHAR			p8023hdr,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN  UCHAR			FromWhichBSSID);

PNDIS_PACKET duplicate_pkt(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pHeader802_3,
    IN  UINT            HdrLen,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN	UCHAR			FromWhichBSSID);


PNDIS_PACKET duplicate_pkt_with_VLAN(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pHeader802_3,
    IN  UINT            HdrLen,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN	UCHAR			FromWhichBSSID);


UCHAR VLAN_8023_Header_Copy(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pHeader802_3,
	IN	UINT            HdrLen,
	OUT PUCHAR			pData,
	IN	UCHAR			FromWhichBSSID);


void ba_flush_reordering_timeout_mpdus(
	IN PRTMP_ADAPTER	pAd, 
	IN PBA_REC_ENTRY	pBAEntry,
	IN ULONG			Now32);


VOID BAOriSessionSetUp(
			IN PRTMP_ADAPTER    pAd, 
			IN MAC_TABLE_ENTRY	*pEntry,
			IN UCHAR			TID,
			IN USHORT			TimeOut,
			IN ULONG			DelayTime,
			IN BOOLEAN		isForced);

VOID BASessionTearDownALL(
	IN OUT	PRTMP_ADAPTER pAd, 
	IN		UCHAR Wcid);

BOOLEAN OS_Need_Clone_Packet(void);


VOID build_tx_packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	PUCHAR	pFrame,
	IN	ULONG	FrameLen);
VOID BAOriSessionTearDown(
	IN OUT	PRTMP_ADAPTER	pAd, 
	IN		UCHAR			Wcid,
	IN		UCHAR			TID,
	IN		BOOLEAN			bPassive,
	IN		BOOLEAN			bForceSend);
VOID BARecSessionTearDown(
	IN OUT	PRTMP_ADAPTER	pAd, 
	IN		UCHAR			Wcid,
	IN		UCHAR			TID,
	IN		BOOLEAN			bPassive);

BOOLEAN ba_reordering_resource_init(PRTMP_ADAPTER pAd, int num);
void ba_reordering_resource_release(PRTMP_ADAPTER pAd);
#endif
/*
ULONG AutoChBssInsertEntry(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pBssid,
	IN CHAR Ssid[],
	IN UCHAR SsidLen, 
	IN UCHAR ChannelNo,
	IN CHAR Rssi);
*/
/*
void AutoChBssTableInit(
	IN PRTMP_ADAPTER pAd);

void ChannelInfoInit(
	IN PRTMP_ADAPTER pAd);

void AutoChBssTableDestroy(
	IN PRTMP_ADAPTER pAd);

void ChannelInfoDestroy(
	IN PRTMP_ADAPTER pAd);
*/

UCHAR New_ApAutoSelectChannel(
	IN PRTMP_ADAPTER pAd);

INT Set_LED_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

#ifdef WSC_INCLUDED
INT	Set_WscConfMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_WscConfStatus_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_WscVendorPinCode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

#ifdef WSC_STA_SUPPORT
INT	Set_WscSsid_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
#endif // WSC_STA_SUPPORT //

INT	Set_WscMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_WscStatus_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_WscGetConf_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_WscPinCode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_WscOOB_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

VOID RTMPIoctlWscProfile(
	IN PRTMP_ADAPTER pAdapter, 
	IN struct iwreq *wrq);

INT	Set_WscGenPinCode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

//
// prototype in wsc.c
//
VOID WscEAPOLTimeOutAction(
    IN  PVOID SystemSpecific1, 
    IN  PVOID FunctionContext, 
    IN  PVOID SystemSpecific2, 
    IN  PVOID SystemSpecific3);

VOID Wsc2MinsTimeOutAction(
    IN  PVOID SystemSpecific1, 
    IN  PVOID FunctionContext, 
    IN  PVOID SystemSpecific2, 
    IN  PVOID SystemSpecific3);

VOID	WscSendEapReqAck(
	IN	PRTMP_ADAPTER		pAdapter, 
	IN	PMAC_TABLE_ENTRY	pEntry);
	
VOID	WscSendEapReqDone(
	IN	PRTMP_ADAPTER		pAdapter, 
	IN	PMLME_QUEUE_ELEM	pElem);

#ifdef CONFIG_STA_SUPPORT
VOID	WscPBCTimeOutAction(
    IN  PVOID SystemSpecific1, 
    IN  PVOID FunctionContext, 
    IN  PVOID SystemSpecific2, 
    IN  PVOID SystemSpecific3);

VOID	WscScanTimeOutAction(
    IN  PVOID SystemSpecific1,
    IN  PVOID FunctionContext,
    IN  PVOID SystemSpecific2,
    IN  PVOID SystemSpecific3);

#endif // CONFIG_STA_SUPPORT //

int __romtext BuildMessageM1(
	IN  PWSC_REG_DATA	pReg,
	IN  UCHAR			*pHmacData,
	OUT	VOID *pbuf);

int __romtext BuildMessageM3(
	IN  PWSC_REG_DATA	pReg,
	IN  UCHAR			*pHmacData,
	OUT	VOID *pbuf);

int __romtext BuildMessageM5(
	IN  PWSC_REG_DATA	pReg,
	IN  UCHAR			*pHmacData,
	OUT	VOID *pbuf);

int __romtext BuildMessageM7(
	IN  PWSC_REG_DATA	pReg,
	IN  UCHAR			*pHmacData,
	OUT	VOID *pbuf);

int __romtext BuildMessageDONE(
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int __romtext BuildMessageACK(
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int __romtext BuildMessageNACK(
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int __romtext ProcessMessageM2(
	IN	VOID *precv,
	IN	INT Length,
	IN  VOID 			*pSecKeyMem,
	IN  UCHAR			*pHmacData,
	OUT	PWSC_REG_DATA pReg);

int __romtext ProcessMessageM2D(
	IN	VOID *precv,
	IN	INT Length,
	OUT	PWSC_REG_DATA pReg);

int __romtext ProcessMessageM4(
	IN  PWSC_CTRL           pWscControl,
	IN	VOID *precv,
	IN	INT Length,
	IN  UCHAR			*pHmacData,
	OUT	PWSC_REG_DATA pReg);

int __romtext ProcessMessageM6(
	IN  PWSC_CTRL           pWscControl,
	IN	VOID *precv,
	IN	INT Length,
	IN  UCHAR			*pHmacData,
	OUT	PWSC_REG_DATA pReg);

int __romtext ProcessMessageM8(
	IN	VOID *precv,
	IN	INT Length,
	IN  PWSC_CTRL           pWscControl,
	IN  UCHAR			*pHmacData);

int ProcessMessageM8_Wrap(
	IN	VOID 		*precv,
	IN	INT 		Length,
	IN  PWSC_CTRL   pWscControl,
	IN  UCHAR		*pHmacData);

VOID WscLinkDown(
	IN  PWSC_CTRL       pWscControl);

#ifdef UCOS
void *WscSyncConfToDatFile(
    IN  PRTMP_ADAPTER pAd,
    IN  ULONG dummy1,
    IN  ULONG dummy2);
#endif
void    WscWriteConfToDatFile(
    IN  PRTMP_ADAPTER pAd);

void    start_write_dat_file_thread(
    IN  PRTMP_ADAPTER pAd);

#ifdef CONFIG_STA_SUPPORT
VOID    WscPBCBssTableSort(
	IN	PRTMP_ADAPTER	pAdapter);

VOID __romtext fnChangePriDevType(
	IN  UCHAR newPriDevType[]);
#endif // CONFIG_STA_SUPPORT //

void    WscWriteConfToPortCfg(
    IN  PWSC_CTRL       pWscControl,
    IN  PWSC_CREDENTIAL pCredential);

#ifdef CONFIG_AP_SUPPORT
VOID   WpsApSmProcess(
    IN PRTMP_ADAPTER        pAd,
    IN MLME_QUEUE_ELEM 	   *Elem);
#endif // CONFIG_AP_SUPPORT //


void    WscWriteConfToPortCfg(
    IN  PWSC_CTRL       pWscControl,
    IN  PWSC_CREDENTIAL pCredential);



#endif // WSC_INCLUDED //

#ifdef NINTENDO_AP
VOID	InitNINTENDO_TABLE(
	IN PRTMP_ADAPTER pAd);

UCHAR	CheckNINTENDO_TABLE(
	IN PRTMP_ADAPTER pAd, 
	PCHAR pDS_Ssid, 
	UCHAR DS_SsidLen, 
	PUCHAR pDS_Addr);

UCHAR	DelNINTENDO_ENTRY(
	IN	PRTMP_ADAPTER pAd,
	UCHAR * pDS_Addr);

VOID	RTMPIoctlNintendoCapable(
	IN	PRTMP_ADAPTER	pAd, 
	IN	struct iwreq	*wrq);

VOID	RTMPIoctlNintendoGetTable(
	IN	PRTMP_ADAPTER	pAd, 
	IN	struct iwreq	*wrq);

VOID	RTMPIoctlNintendoSetTable(
	IN	PRTMP_ADAPTER	pAd, 
	IN	struct iwreq	*wrq);

#endif // NINTENDO_AP //

BOOLEAN rtstrmactohex(
	IN char *s1,
	IN char *s2);

char *rtstrstruncasecmp(
	IN char *s1,
	IN char *s2);

char    *rtstrstr(
	IN	const char * s1,
	IN	const char * s2);

char *rstrtok(
	IN char * s,
	IN const char * ct);
	
////////// common ioctl functions //////////
INT	Set_BASetup_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_BADecline_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_BAOriTearDown_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_BARecTearDown_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtBw_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtMcs_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtGi_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtOpMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtStbc_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtHtc_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtExtcha_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtMpduDensity_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtBaWinSize_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtRdg_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtLinkAdapt_Proc(																																																																																																																																																																																																																																									
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtAmsdu_Proc(																																																																																																																																																																																																																																																																																																																			
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
	
INT	Set_HtAutoBa_Proc(																																																																																																																																																																																																																																																																																																																			
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
					
INT	Set_HtProtect_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

INT	Set_HtMimoPs_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

#ifdef CONFIG_AP_SUPPORT
INT	Set_HtTxStream_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtRxStream_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
#endif // CONFIG_AP_SUPPORT //

INT	Set_ForceShortGI_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

INT	Set_ForceGF_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

INT	Set_SendPSMPAction_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtMIMOPSmode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);


INT	Set_HtTxBASize_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

#ifdef RT2870
INT	Show_RxBulk_Proc(
	IN RTMP_ADAPTER *pAd, 
	IN PUCHAR	arg);

INT	Show_TxBulk_Proc(
	IN RTMP_ADAPTER *pAd, 
	IN PUCHAR	arg);
#endif // RT2870 //


#ifdef CONFIG_STA_SUPPORT
//Dls ,	kathy
VOID RTMPSendDLSTearDownFrame(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			pDA);

//Block ACK
VOID QueryBATABLE(
	IN  PRTMP_ADAPTER pAd,
	OUT PQUERYBA_TABLE pBAT);

#endif // CONFIG_STA_SUPPORT //

#ifdef CONFIG_AP_SUPPORT
VOID detect_wmm_traffic(
	IN	PRTMP_ADAPTER	pAd, 
	IN	UCHAR			UserPriority);

VOID dynamic_tune_be_tx_op(
	IN	PRTMP_ADAPTER	pAd,
	IN	ULONG			nonBEpackets);
#endif // CONFIG_AP_SUPPORT //


#ifdef CONFIG_AP_SUPPORT
void SendSingalToDaemon(
	IN INT              sig,
	ULONG               pid);

BOOLEAN IEEE8021X_L2_Disconnect_Frame_Send(
    IN  PRTMP_ADAPTER	pAd,
    IN  MAC_TABLE_ENTRY *pEntry);

#endif // CONFIG_AP_SUPPORT //

BOOLEAN APRxDoneInterruptHandle(
	IN	PRTMP_ADAPTER	pAd);


#if 0
#ifdef CONFIG_AP_SUPPORT
// remove LLC and get 802_3 Header
#define  RTMP_802_11_REMOVE_LLC_AND_CONVERT_TO_802_3(_pRxBlk, _pHeader802_3)	\
{																				\
	PUCHAR _pRemovedLLCSNAP = NULL, _pDA, _pSA;                                 \
																				\
	_pDA = _pRxBlk->pHeader->Addr3;                                         	\
																				\
	if (!RX_BLK_TEST_FLAG(_pRxBlk, fRX_WDS))                                    \
	{                                                                           \
		_pSA = _pRxBlk->pHeader->Addr2;                                         \
	}                                                                           \
	else                                                                        \
	{                                                                           \
		_pSA = (PUCHAR)_pRxBlk->pHeader + sizeof(HEADER_802_11);                \
	}                                                                           \
																				\
	CONVERT_TO_802_3(_pHeader802_3, _pDA, _pSA, _pRxBlk->pData, 				\
		_pRxBlk->DataSize, _pRemovedLLCSNAP);                                   \
}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
// remove LLC and get 802_3 Header
#define  RTMP_802_11_REMOVE_LLC_AND_CONVERT_TO_802_3(_pRxBlk, _pHeader802_3)	\
{																				\
	PUCHAR _pRemovedLLCSNAP = NULL, _pDA, _pSA;                                 \
																				\
	_pDA = _pRxBlk->pHeader->Addr1;                                         	\
																				\
	if (RX_BLK_TEST_FLAG(_pRxBlk, fRX_INFRA))                              		\
	{                                                                           \
		_pSA = _pRxBlk->pHeader->Addr3;                                         \
	}                                                                           \
	else                                                                        \
	{                                                                           \
		_pSA = _pRxBlk->pHeader->Addr2;                                         \
	}                                                                           \
																				\
	CONVERT_TO_802_3(_pHeader802_3, _pDA, _pSA, _pRxBlk->pData, 				\
		_pRxBlk->DataSize, _pRemovedLLCSNAP);                                   \
}
#endif // CONFIG_STA_SUPPORT //
#endif

#define  RTMP_802_11_REMOVE_LLC_AND_CONVERT_TO_802_3(_pRxBlk, _pHeader802_3)	\
{																						\
	PUCHAR _pRemovedLLCSNAP = NULL, _pDA, _pSA;                                 						\
																				\
	                                                                    											\
	if (RX_BLK_TEST_FLAG(_pRxBlk, fRX_AP))										\
	{                                                                           											\
		_pDA = _pRxBlk->pHeader->Addr1; 													\
		if (RX_BLK_TEST_FLAG(_pRxBlk, fRX_DLS))									\
			_pSA = _pRxBlk->pHeader->Addr2;										\
		else																	\
			_pSA = _pRxBlk->pHeader->Addr3;		\
	} \
	else if (RX_BLK_TEST_FLAG(_pRxBlk, fRX_STA))                              	\
	{                                                                       \
		_pDA = _pRxBlk->pHeader->Addr3;                                     \
		_pSA = _pRxBlk->pHeader->Addr2;                                     \
	}                                                                       \
	else                                                                        										\
	{    /* TODO: shiang-usw, where shall we go here?? */ \
		MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, ("%s():Un-assigned Peer's Role!\n", __FUNCTION__));\
		_pDA = _pRxBlk->pHeader->Addr3;                                         							\
		_pSA = _pRxBlk->pHeader->Addr2;                                         							\
	}                                                                           											\
																				\
	CONVERT_TO_802_3(_pHeader802_3, _pDA, _pSA, _pRxBlk->pData, 		\
		_pRxBlk->DataSize, _pRemovedLLCSNAP);                                  \
	if (!_pRemovedLLCSNAP)			\
	{ \
		MTWF_LOG(DBG_CAT_RX, DBG_SUBCAT_ALL, DBG_LVL_TRACE, ("%s():Null _pRemovedLLCSNAP\n", __FUNCTION__));\
	} \
}

BOOLEAN APFowardWirelessStaToWirelessSta(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	ULONG			FromWhichBSSID);

//#ifdef CONFIG_AP_SUPPORT
#define ANNOUNCE_OR_FORWARD_802_3_PACKET(_pAd, _pPacket, _FromWhichBSS)\
			Announce_or_Forward_802_3_Packet(_pAd, _pPacket, _FromWhichBSS);
//#endif // CONFIG_AP_SUPPORT //


#if 0
#ifdef CONFIG_STA_SUPPORT
#define ANNOUNCE_OR_FORWARD_802_3_PACKET(_pAd, _pPacket, _FromWhichBSS)\
			announce_802_3_packet(_pAd, _pPacket);
#endif // CONFIG_STA_SUPPORT //
#endif

PNDIS_PACKET DuplicatePacket(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket,
	IN	UCHAR			FromWhichBSSID);

extern PNDIS_PACKET (*RTMPDeFragmentDataFrame)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk);

////////////////////////////////////////


#ifdef CONFIG_AP_SUPPORT
typedef struct CountryCodeToCountryRegion {
	USHORT		CountryNum;
    UCHAR   	IsoName[3];
    UCHAR   	CountryName[40];
	BOOLEAN		SupportABand;
    ULONG		RegDomainNum11A;
    BOOLEAN  	SupportGBand;
    ULONG		RegDomainNum11G;
} COUNTRY_CODE_TO_COUNTRY_REGION;
#endif // CONFIG_AP_SUPPORT //

#ifndef WIFI_BUILD_RAM
#ifdef IOT_SECURITY_SUPPORT
typedef struct _CMD_802_11_KEY {
    UINT_8      ucAddRemove;
    UINT_8      ucTxKey;   /* 1 : Tx key */
    UINT_8      ucKeyType; /* 0 : group Key, 1 : Pairwise key */
    UINT_8      ucIsAuthenticator; /* 1 : authenticator */
    UINT_8      aucPeerAddr[6];
    UINT_8      ucBssIndex; /* the BSS index */
    UINT_8      ucAlgorithmId;
    UINT_8      ucKeyId;
    UINT_8      ucKeyLen;
    UINT_8      ucWlanIndex;
    UINT_8      ucReverved;
    UINT_8      aucKeyMaterial[32];
    UINT_8      aucKeyRsc[16];
} CMD_802_11_KEY, *P_CMD_802_11_KEY;
#endif
#endif

#ifdef CONFIG_STA_SUPPORT
enum {
	DIDmsg_lnxind_wlansniffrm		= 0x00000044,
	DIDmsg_lnxind_wlansniffrm_hosttime	= 0x00010044,
	DIDmsg_lnxind_wlansniffrm_mactime	= 0x00020044,
	DIDmsg_lnxind_wlansniffrm_channel	= 0x00030044,
	DIDmsg_lnxind_wlansniffrm_rssi		= 0x00040044,
	DIDmsg_lnxind_wlansniffrm_sq		= 0x00050044,
	DIDmsg_lnxind_wlansniffrm_signal	= 0x00060044,
	DIDmsg_lnxind_wlansniffrm_noise		= 0x00070044,
	DIDmsg_lnxind_wlansniffrm_rate		= 0x00080044,
	DIDmsg_lnxind_wlansniffrm_istx		= 0x00090044,
	DIDmsg_lnxind_wlansniffrm_frmlen	= 0x000A0044
};
enum {
	P80211ENUM_msgitem_status_no_value	= 0x00
};
enum {
	P80211ENUM_truth_false			= 0x00,
	P80211ENUM_truth_true			= 0x01
};

typedef struct {
        UINT32 did;
        UINT16 status;
        UINT16 len;
        UINT32 data;
} p80211item_uint32_t;

typedef struct {
        UINT32 msgcode;
        UINT32 msglen;
#define WLAN_DEVNAMELEN_MAX 16
        UINT8 devname[WLAN_DEVNAMELEN_MAX];
        p80211item_uint32_t hosttime;
        p80211item_uint32_t mactime;
        p80211item_uint32_t channel;
        p80211item_uint32_t rssi;
        p80211item_uint32_t sq;
        p80211item_uint32_t signal;
        p80211item_uint32_t noise;
        p80211item_uint32_t rate;
        p80211item_uint32_t istx;
        p80211item_uint32_t frmlen;
} wlan_ng_prism2_header;
#endif

static inline char* GetPhyMode(
	int Mode)
{
	switch(Mode)
	{
		case MODE_CCK:
			return "CCK";

		case MODE_OFDM:
			return "OFDM";

		case MODE_HTMIX:
			return "HTMIX";

		case MODE_HTGREENFIELD:
			return "GREEN";

		default:
			return "N/A";
	}
}


static inline char* GetBW(
	int BW)
{
	switch(BW)
	{
		case BW_10:
			return "10M";

		case BW_20:
			return "20M";

		case BW_40:
			return "40M";

		default:
			return "N/A";
	}
}


#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/
#define ETH_P_IPV6	0x86DD		/* IPv6 over bluebook		*/
#define ETH_P_ARP       0x0806          /* Address Resolution packet    */
static inline VOID ConvertMulticastIP2MAC(
	IN PUCHAR pIpAddr,
	IN PUCHAR *ppMacAddr, 
	IN UINT16 ProtoType)
{
	if (pIpAddr == NULL)
		return;

	if (ppMacAddr == NULL || *ppMacAddr == NULL)
		return;

	switch (ProtoType)
	{
		case ETH_P_IPV6:
//			rom_memset(*ppMacAddr, 0, ETH_LENGTH_OF_ADDRESS);
			*(*ppMacAddr) = 0x33;
			*(*ppMacAddr + 1) = 0x33;
			*(*ppMacAddr + 2) = pIpAddr[12];
			*(*ppMacAddr + 3) = pIpAddr[13];
			*(*ppMacAddr + 4) = pIpAddr[14];
			*(*ppMacAddr + 5) = pIpAddr[15];
			break;

		case ETH_P_IP:
		default:
//			rom_memset(*ppMacAddr, 0, ETH_LENGTH_OF_ADDRESS);
			*(*ppMacAddr) = 0x01;
			*(*ppMacAddr + 1) = 0x00;
			*(*ppMacAddr + 2) = 0x5e;
			*(*ppMacAddr + 3) = pIpAddr[1] & 0x7f;
			*(*ppMacAddr + 4) = pIpAddr[2];
			*(*ppMacAddr + 5) = pIpAddr[3];
			break;
	}

	return;
}


typedef VOID (*PRANDOM_BYTE)(
	IN PRTMP_ADAPTER pAd);


#ifdef IOT_CFG80211_SUPPORT
INT Set_DisableCfg2040Scan_Proc(RTMP_ADAPTER *pAd, UCHAR *arg);

/* command */
INT Set_Key1_Proc(RTMP_ADAPTER *pAd, CHAR *arg);
INT Set_Key2_Proc(RTMP_ADAPTER *pAd, CHAR *arg);
INT Set_Key3_Proc(RTMP_ADAPTER *pAd, CHAR *arg);
INT Set_Key4_Proc(RTMP_ADAPTER *pAd, CHAR *arg);
INT Set_WPAPSK_Proc(RTMP_ADAPTER *pAd, CHAR *arg);
INT Set_PSMode_Proc(RTMP_ADAPTER *pAd, CHAR *arg);

INT RTMPSetWEPKey(RTMP_ADAPTER *pAd, UCHAR *Key, UINT8 KeyType, UINT8 BSSIdx, UINT8 KeyIdx);
#endif /* IOT_CFG80211_SUPPORT */

extern VOID (*RTMPSetSupportMCS)(
    IN struct _RTMP_ADAPTER *pAd,
    IN UCHAR OpMode,
    IN struct _MAC_TABLE_ENTRY	*pEntry,
    IN UCHAR SupRate[],
    IN UCHAR SupRateLen,
    IN UCHAR ExtRate[],
    IN UCHAR ExtRateLen,
    IN struct _HT_CAPABILITY_IE *pHtCapability,
    IN UCHAR HtCapabilityLen
    );


#if 1//def IOT_CFG80211_SUPPORT

#define STA_EXTRA_SETTING(_pAd)
#ifdef DOT11R_FT_SUPPORT
#undef STA_EXTRA_SETTING
#define STA_EXTRA_SETTING(_pAd) \
{ \
	if ((_pAd)->StaCfg[0].Dot11RCommInfo.bFtSupport && \
		(_pAd)->MlmeAux.MdIeInfo.Len && \
		(_pAd)->StaCfg[0].AuthMode == Ndis802_11AuthModeWPA2PSK) \
		(_pAd)->StaCfg[0].Dot11RCommInfo.bInMobilityDomain = TRUE; \
}
#endif /* DOT11R_FT_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
#define STA_PORT_SECURED(_pAd) \
{ \
	BOOLEAN	Cancelled; \
	struct wifi_dev *pwdev = &((_pAd)->StaCfg.wdev);  \
 	pwdev->PortSecured = WPA_802_1X_PORT_SECURED; \
 	_pAd->IndicateMediaState = NdisMediaStateConnected; \
	RTMP_IndicateMediaState(_pAd); \
	_pAd->StaCfg.PrivacyFilter = Ndis802_11PrivFilterAcceptAll;\
	RTMPCancelTimer(&((_pAd)->Mlme.LinkDownTimer), &Cancelled);\
	STA_EXTRA_SETTING(_pAd); \
}

#if 0
#define STA_PORT_SECURED(_pAd) \
{ \
	BOOLEAN	Cancelled; \
	struct wifi_dev *pwdev = &((_pAd)->StaCfg.wdev);  \
	pwdev->PortSecured = WPA_802_1X_PORT_SECURED; \
	_pAd->IndicateMediaState = NdisMediaStateConnected); \
	_pAd->StaCfg.PrivacyFilter = Ndis802_11PrivFilterAcceptAll;\
	RTMPCancelTimer(&((_pAd)->Mlme.LinkDownTimer), &Cancelled);\
	STA_EXTRA_SETTING(_pAd); \
}
#endif
#endif/*CONFIG_STA_SUPPORT*/
#endif /* IOT_CFG80211_SUPPORT */

#ifdef DATA_PATH_87
extern void (*connsys_kickout_tx_data_87)(IN PRTMP_ADAPTER pAd, IN TX_BLK *pTxBlk);
extern void (*connsys_kickout_tx_mgmt_87)(IN	PRTMP_ADAPTER pAd, IN struct net_pkt_blk *pkt,BOOLEAN Keep_pkt);
#endif

#endif  // __RTMP_H__
