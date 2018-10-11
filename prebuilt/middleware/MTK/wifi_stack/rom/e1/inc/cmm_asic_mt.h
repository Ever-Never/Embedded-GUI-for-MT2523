/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering	the source code	is stricitly prohibited, unless	the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	cmm_asic_mt.h

	Abstract:
	Ralink Wireless Chip HW related definition & structures

	Revision History:
	Who			When		  What
	--------	----------	  ----------------------------------------------
*/


#ifndef __CMM_ASIC_MT_H__
#define __CMM_ASIC_MT_H__
#ifndef MT_MAC
#define MT_MAC
#endif

struct _RTMP_ADAPTER;
struct _MAC_TABLE_ENTRY;
struct _CIPHER_KEY;
struct _MT_TX_COUNTER;
struct _EDCA_PARM;
struct _EXT_CMD_CHAN_SWITCH_T;
//struct _BCTRL_INFO_T;

/* SIFS time, 20us, and assume bcn len is 512 byte, tx by 1Mbps.*/
#define BCN_TRANSMIT_ESTIMATE_TIME  (4096 + 20)

typedef enum {
	MT_BSS_MODE_AP=0,
	MT_BSS_MODE_ADHOC,
	MT_BSS_MODE_STA,
}MT_BSS_OP_MODE_T;

typedef struct {
 UINT8 BssSet; 						/*HW Bssid Set: 0~3*/
 UINT8 MbssidIdx;					/*Virtual Multi BSSID index: 0~15*/
	UINT8 	PreTbttInterval;
	USHORT BeaconPeriod;
	USHORT DtimPeriod;
	MT_BSS_OP_MODE_T BssOpMode;
	UCHAR BandIdx;
} MT_BSS_SYNC_CTRL_T;


typedef enum {
 NO_PROTECT=0,
 PROTECT_MIX_MODE=1,
 PROTECT_HT40_ONLY=2,
 PROTECT_HT40_HT20=3,
 PROTECT_MODE_END=8
} PROTECT_OP_MODE_T;

#define ERP_DISABLE 0
#define ERP_OMAC0   (1 << 0)
#define ERP_OMAC1   (1 << 1)
#define ERP_OMAC2   (1 << 2)
#define ERP_OMAC3   (1 << 3)
#define ERP_OMAC4   (1 << 4)
#define ERP_OMAC_ALL    ((ERP_OMAC0)|(ERP_OMAC1)|(ERP_OMAC2)|(ERP_OMAC3)|(ERP_OMAC4))


typedef struct {
 UCHAR SetMask;
 BOOLEAN bDisableBGProtect;
 BOOLEAN bNonGFExist;
 BOOLEAN bLongNAV; 					/*Note: When TX_RIFS_EN of RCR is enabled, HW will force switch to long NAV mode*/
 PROTECT_OP_MODE_T OperationMode;
 UINT32 ERPProtectMask; 					/*[0] for OM0/[1] for OM1/[2] for OM2/[3] for OM3/[4] for OM4*/
} MT_PROTECT_CTRL_T ;


typedef struct {
    UINT32 RTSThreshold;
    UINT32 RTSPacketNumThreshold;
    UINT8 Rsv[4];
} MT_RTSCTS_CTRL_T;



typedef struct _MT_RTS_THRESHOLD_T
{
    UINT8   BandIdx;
    UINT32  PacketLenThreshold;
    UINT32  PacketNumThreshold;
} MT_RTS_THRESHOLD_T, *P_MT_RTS_THRESHOLD_T;


#define RDG_DISABLE     0x0
#define RDG_INITIATOR   (1 << 0)
#define RDG_RESPONDER   (1 << 1)
#define RDG_BOTH        (RDG_INITIATOR|RDG_RESPONDER)
typedef struct _MT_RDG_CTRL_T
{
    UINT32  Txop;
    UINT8   Role;
    UINT8   LongNav;
    UINT8   WlanIdx;
    UINT8   BandIdx;
} MT_RDG_CTRL_T, *P_MT_RDG_CTRL_T;


typedef struct _MT_SWITCH_CHANNEL_CFG{
	UCHAR ControlChannel;
	/*Only used for 80+80 case*/
	UCHAR ControlChannel2;
	UCHAR CentralChannel;
	UCHAR Bw;
	UCHAR TxStream;
	UCHAR RxStream;
	BOOLEAN bScan;
#ifdef COMPOS_TESTMODE_WIN
	BOOLEAN isMCC;
#endif
#ifdef MT_DFS_SUPPORT
    DFS_PARAM DfsParam;//Jelly20150305
#endif
	UCHAR BandIdx;
}MT_SWITCH_CHANNEL_CFG;

typedef enum {
 RX_STBC_BCN_BC_MC=1<<0,
 RX_FCS_ERROR = 1 <<1,
 RX_PROTOCOL_VERSION = 1 <<2 , /*drop 802.11 protocol version not is 0*/
 RX_PROB_REQ = 1 <<3,
 RX_MC_ALL =  1 << 4,     /*drop all mcast frame*/
 RX_BC_ALL = 1 << 5,
 RX_MC_TABLE = 1 <<6,     /*drop mcast frame that is not in the mcast table*/
 RX_BC_MC_OWN_MAC_A3 = 1 << 7, /*drop bc/mc packet matches the following condition: ToDS=0,FromDS=1,A3=OwnMAC0 or OwnMAC1*/
 RX_BC_MC_DIFF_BSSID_A3 = 1<< 8, /*drop bc/mc packet matches the following condition: ToDS=0,FromDS=0,A3!=BSSID0 or BSSID1*/
 RX_BC_MC_DIFF_BSSID_A2 =  1 << 9,/*drop bc/mc packet matches the following condition: ToDS=0,FromDS=1,A2!=BSSID0 or BSSID1*/
 RX_BCN_DIFF_BSSID =  1<< 10,  /*drop bcn packet and A3!=BSSID0 or BSSID1*/
 RX_CTRL_RSV = 1 << 11,    /*drop control packets with reserve type*/
 RX_CTS = 1 << 12,
 RX_RTS = 1 <<13,
 RX_DUPLICATE_FRAME = 1 << 14,   /*drop duplicate frame, BA session not includign in this filter*/
 RX_NOT_OWN_BSSID = 1 <<15,  /*drop not my BSSID0/1/2/3 if enabled*/
 RX_NOT_OWN_UCAST= 1 << 16,  /*drop uncast packet not to OWN MAC 0/1/2/3/4*/
 RX_NOT_OWN_BTIM= 1<<17,  /*drop diff bassid TIM broadcast*/
 RX_NDPA= 1<<18,     /*drop NDPA control frame*/
} MT_RX_FILTER_T;

typedef struct {
 UINT32  filterMask;
 BOOLEAN bPromiscuous;
 BOOLEAN bFrameReport;
 UCHAR BandIdx;
}MT_RX_FILTER_CTRL_T;


typedef enum _BA_SESSION_TYPE{
 BA_SESSION_INV = 0,
 BA_SESSION_ORI = 1,
 BA_SESSION_RECP = 2,
}BA_SESSION_TYPE;


typedef struct _MT_BA_CTRL_T{
 UCHAR Wcid;
 UCHAR Tid;
 UCHAR BaWinSize;
 BOOLEAN isAdd;
 UINT8 band_idx;
 BA_SESSION_TYPE BaSessionType;
 UCHAR PeerAddr[MAC_ADDR_LEN];
 UINT16 Sn;							/*Sequence number for a specific TID*/
} MT_BA_CTRL_T, *P_MT_BA_CTRL_T;


typedef enum {
 MT_WCID_TYPE_AP,
 MT_WCID_TYPE_CLI,
 MT_WCID_TYPE_APCLI,
 MT_WCID_TYPE_BMCAST,
 MT_WCID_TYPE_APCLI_MCAST,
 MT_WCID_TYPE_REPEATER,
 MT_WCID_TYPE_WDS,
}MT_WCID_TYPE_T;

#define WCID_ALL		0xff



typedef struct {
 USHORT Wcid;
 USHORT Aid;
 UINT8 BssidIdx;
 UINT8 MacAddrIdx;
 UINT8 SmpsMode;
 UINT8 MaxRAmpduFactor;
 UINT8 MpduDensity;
 MT_WCID_TYPE_T WcidType;
 UCHAR Addr[MAC_ADDR_LEN];
 UINT32 CipherSuit;
 UCHAR  PfmuId;
 BOOLEAN SupportHT;
 BOOLEAN SupportVHT;
 BOOLEAN SupportRDG;
 BOOLEAN SupportQoS;
}MT_WCID_TABLE_INFO_T;


#if 0
typedef enum _ENUM_CIPHER_SUIT_T
{
    CIPHER_SUIT_NONE = 0,
    CIPHER_SUIT_WEP_40,
    CIPHER_SUIT_TKIP_W_MIC,
    CIPHER_SUIT_TKIP_WO_MIC,
    CIPHER_SUIT_CCMP_W_MIC, //for DFP or 802.11w MFP
    CIPHER_SUIT_WEP_104,
    CIPHER_SUIT_BIP,
    CIPHER_SUIT_WEP_128 = 7,
    CIPHER_SUIT_WPI,
    CIPHER_SUIT_CCMP_W_MIC_FOR_CCX = 9,  //for DFP or CCX MFP
    CIPHER_SUIT_CCMP_256,
    CIPHER_SUIT_GCMP_128,
    CIPHER_SUIT_GCMP_256
} ENUM_CIPHER_SUIT_T256, *P_ENUM_CIPHER_SUIT_T;
#endif

/* The security mode definition in MAC register */
typedef enum {
CIPHER_NONE            = 0,
CIPHER_WEP64           = 1,
CIPHER_WEP128         = 2,
CIPHER_TKIP               = 3,
CIPHER_AES                 = 4,
CIPHER_CKIP64           = 5,
CIPHER_CKIP128         = 6,
CIPHER_CKIP152         = 7,
CIPHER_SMS4		   = 8,
CIPHER_WEP152           = 9,
CIPHER_BIP	                   = 10,
CIPHER_WAPI             = 11,
CIPHER_TKIP_NO_MIC = 12,
} MT_SEC_CIPHER_SUITS_T;
//#endif

#if 1
typedef struct {
 BOOLEAN isAdd;
 UCHAR BssIdx;
 UCHAR Wcid;
 MT_SEC_CIPHER_SUITS_T KeyType;
 UCHAR KeyIdx;
 MT_BSS_OP_MODE_T OpMode;
 struct _CIPHER_KEY *pCipherKey;
 UCHAR PeerAddr[MAC_ADDR_LEN];
}MT_SECURITY_CTRL;
#endif

#ifndef WIFI_BUILD_RAM 
typedef struct _MT_TX_COUNTER
{
	UINT32 TxCount;
	UINT32 TxFailCount;
	UINT16 Rate1TxCnt;
	UINT16 Rate1FailCnt;
	UINT16 Rate2OkCnt;
	UINT16 Rate3OkCnt;
	UCHAR Rate2TxCnt;
	UCHAR Rate3TxCnt;
	UCHAR Rate4TxCnt;
	UCHAR Rate5TxCnt;
	UCHAR RateIndex;
} MT_TX_COUNTER;
#endif

typedef enum _MCU_STAT{
	METH1_RAM_CODE,
	METH1_ROM_CODE,
	METH3_FW_RELOAD,
	METH3_INIT_STAT,
	METH3_FW_PREPARE_LOAD,
	METH3_FW_RUN,
	MCU_STAT_END
} MCU_STAT;


INT32 __AsicStaRecUpdate(
	RTMP_ADAPTER *pAd,
	UINT8 BssIndex,
	UINT8 WlanIdx,
	UINT32 ConnectionType,
	UINT8 ConnectionState,
	UINT32 EnableFeature);

typedef struct {
	INT mode;						/*0: LMAC,1: BYPASS, 2: HyBrid",*/
	BOOLEAN bBeaconSpecificGroup; 	/*is want to allocate a specific group for beacon frame*/
}MT_DMASCH_CTRL_T;


/* mapping table entry for tttt cr and HW_bssid_idx*/
typedef struct _TTTT_CR_BSSID_IDX_MAP_T
{
    UINT32             u4HwBssidIdx;
    UINT32             u4TTTTEnableCr;
    UINT32             u4TTTTIntervalCr;
} TTTT_CR_BSSID_IDX_MAP_T, *PTTTT_CR_BSSID_IDX_MAP_T;

typedef enum _HWBSSID_TABLE
{
    HW_BSSID_0 = 0,
    HW_BSSID_1,
    HW_BSSID_2,
    HW_BSSID_3,
    HW_BSSID_MAX
} HWBSSID_TABLE;


#define RMAC_RMACDR                     (WIFI_RMAC_BASE + 0x0078)

#define ASIC_MAC_TX			1
#define ASIC_MAC_RX			2
#define ASIC_MAC_TXRX		3
#define ASIC_MAC_TXRX_RXV	4
#define ASIC_MAC_RXV		5
#define ASIC_MAC_RX_RXV		6

#endif /* __CMM_ASIC_MT_H__ */


