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
    rtmp_def.h

    Abstract:
    Miniport related definition header

    Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Paul Lin    08-01-2002    created
    John Chang  08-05-2003    add definition for 11g & other drafts
*/
#ifndef __RTMP_DEF_H__
#define __RTMP_DEF_H__
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#undef WSC_INCLUDED

#ifdef CONFIG_AP_SUPPORT
#ifdef WSC_AP_SUPPORT
#define WSC_INCLUDED
#endif // WSC_AP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
#ifdef WSC_STA_SUPPORT
#define WSC_INCLUDED
#endif // WSC_STA_SUPPORT //
#endif // CONFIG_STA_SUPPORT //

//
//  Debug information verbosity: lower values indicate higher urgency
//
#define RT_DEBUG_OFF        5 // force to remove this level msg out of ROM // 0
#define RT_DEBUG_ERROR      1
#define RT_DEBUG_WARN       2
#define RT_DEBUG_TRACE      3
#define RT_DEBUG_INFO       4
#define RT_DEBUG_LOUD       5

#define NIC_TAG             ((ULONG)'0682')
#define NIC_DBG_STRING      ("**RT28xx**")

//#define PACKED

//
// NDIS version in use by the NIC driver.
// The high byte is the major version. The low byte is the minor version.
//
#ifdef  NDIS51_MINIPORT
#define NIC_DRIVER_VERSION      0x0501
#else
#define NIC_DRIVER_VERSION      0x0500
#endif

//
// NDIS media type, current is ethernet, change if native wireless supported
//
#define NIC_MEDIA_TYPE          NdisMedium802_3
#define NIC_PCI_HDR_LENGTH      0xe2
#define NIC_MAX_PACKET_SIZE     2304
#define NIC_HEADER_SIZE         14
#define MAX_MAP_REGISTERS_NEEDED 32
#define MIN_MAP_REGISTERS_NEEDED 2   //Todo: should consider fragment issue.

//
// interface type, we use PCI
//
#define NIC_INTERFACE_TYPE      NdisInterfacePci
#define NIC_INTERRUPT_MODE      NdisInterruptLevelSensitive

//
// buffer size passed in NdisMQueryAdapterResources
// We should only need three adapter resources (IO, interrupt and memory),
// Some devices get extra resources, so have room for 10 resources
//                    UF_SIZE   (sizeof(NDIS_RESOURCE_LIST) + (10*sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)))


#define NIC_RESOURCE_B//
// IO space length
//
#define NIC_MAP_IOSPACE_LENGTH  sizeof(CSR_STRUC)

#define	RETRY_LIMIT	10

//
// Entry number for each DMA descriptor ring
//
#ifdef RT2860
#if 0
#ifndef TX_RING_SIZE
#define TX_RING_SIZE            96 //64
#endif
#ifndef MGMT_RING_SIZE
#define MGMT_RING_SIZE          128
#endif
#ifndef RX_RING_SIZE
#define RX_RING_SIZE            128 //64
#endif
#endif


//#define MAX_TX_PROCESS          TX_RING_SIZE //8
//#define MAX_DMA_DONE_PROCESS    TX_RING_SIZE
//#define MAX_TX_DONE_PROCESS     TX_RING_SIZE //8
#define LOCAL_TXBUF_SIZE        2
#endif // RT2860 //

#if 0 //def RT2870
#define TX_RING_SIZE            8 // 1
#define PRIO_RING_SIZE          8
#define MGMT_RING_SIZE          PRIO_RING_SIZE
#define RX_RING_SIZE            8
#define MAX_TX_PROCESS          4
#define LOCAL_TXBUF_SIZE        2048
#endif // RT2870 //

#ifdef MULTIPLE_CARD_SUPPORT
// MC: Multple Cards
#define MAX_NUM_OF_MULTIPLE_CARD		32
#endif // MULTIPLE_CARD_SUPPORT //

#if 0 //ndef MAX_RX_PROCESS
#define MAX_RX_PROCESS          128 //64 //32
#endif
#define NUM_OF_LOCAL_TXBUF      2
#define TXD_SIZE                16
#define TXWI_SIZE               32 // sizeof(HW_MAC_TX_DESC_T)  //MT7662=20, old chip= 16
#define RXD_SIZE               	16
#define RXWI_SIZE             	16 // sizeof(HW_MAC_RX_DESC_T)   // 28 //16
#define RXINFO_SIZE             0
// TXINFO_SIZE + TXWI_SIZE + 802.11 Header Size + AMSDU sub frame header
#define TX_DMA_1ST_BUFFER_SIZE  96    // only the 1st physical buffer is pre-allocated
#define MGMT_DMA_BUFFER_SIZE    512//1536 //2048
#define RX_BUFFER_AGGRESIZE     3840   //3904 //3968 //4096 //2048 //4096
#define RX_BUFFER_NORMSIZE      1536  // 3840 //3904 //3968 //4096 //2048 //4096
#define TX_BUFFER_NORMSIZE		RX_BUFFER_NORMSIZE
#define MAX_FRAME_SIZE          2346                    // Maximum 802.11 frame size
#define MAX_AGGREGATION_SIZE    3840 //3904 //3968 //4096
#define MAX_NUM_OF_TUPLE_CACHE  2
#define MAX_MCAST_LIST_SIZE     32
#define MAX_LEN_OF_VENDOR_DESC  64
#if 0
//#define MAX_SIZE_OF_MCAST_PSQ   (NUM_OF_LOCAL_TXBUF >> 2) // AP won't spend more than 1/4 of total buffers on M/BCAST PSQ
#define MAX_SIZE_OF_MCAST_PSQ               32

#define MAX_RX_PROCESS_CNT	(MAX_RX_PROCESS)
#endif
//#ifndef MAX_PACKETS_IN_QUEUE
#define MAX_PACKETS_IN_QUEUE(_pAd)				(_pAd->MaxPacketsInQueue) //(512)    // to pass WMM A5-WPAPSK
//#endif 
#define MAX_PACKETS_IN_MCAST_PS_QUEUE(_pAd)		(_pAd->MaxPacketsInMcastPsQueue)//32	 
#define MAX_PACKETS_IN_PS_QUEUE(_pAd)				(_pAd->MaxPacketsInPsQueue)//32
#define WMM_NUM_OF_AC                       4  /* AC0, AC1, AC2, and AC3 */

#ifdef CONFIG_AP_SUPPORT
#ifdef IGMP_SNOOP_SUPPORT
#define MAX_LEN_OF_MULTICAST_FILTER_TABLE 64
#define MAX_LEN_OF_MULTICAST_FILTER_HASH_TABLE ((MAX_LEN_OF_MULTICAST_FILTER_TABLE) * 2)
#define FREE_MEMBER_POOL_SIZE 64
#endif // IGMP_SNOOP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_AP_SUPPORT

#define HW_BEACON_MAX_NUM			1 // No MBSS support

#endif
#define BSSINFO_NUM_MAX HW_BSSID_MAX


#define VALID_WCID(_pAd,_wcid)	((_wcid) > 0 && (_wcid) < MAX_LEN_OF_MAC_TABLE(_pAd) )
#define VALID_UCAST_ENTRY_WCID(_pAd, _wcid) VALID_WCID(_pAd,_wcid)

#define VALID_MBSS(_pAd, _apidx)	((_apidx < MAX_MBSSID_NUM(_pAd)) && (_apidx < HW_BEACON_MAX_NUM))


#define MT7636_PSE_WTBL_2_ADDR	(PSE_BASE_ADDR+0x8000000) // 0xa8000000  /* get from CR setting of PSE.BUFS_DAA (0x164) */
#define MT7636_MT_WTBL_SIZE			32

// RxFilter
#define STANORMAL	 0x17f97
#define APNORMAL	 0x14f97
//

#ifdef IAPP_SUPPORT

#define MAX_RTSIG_QUEUE_SIZE					32
#define MAX_RTSIG_TIMEOUT					(15*OS_HZ)		// 15 secs

/* Definition of signal */
#define AP_SIG_NONE								0
#define AP_SIG_ASSOCIATION						1
#define AP_SIG_REASSOCIATION					2
#define AP_SIG_TERMINATE						3
#define AP_SIG_FORWARD_SKB						4
#define AP_SIG_FORWARD_BCAST_SKB				5
#define AP_SIG_STA_NOT_FOUND					6
#define AP_SIG_STA_FORWARD_START				7
#define AP_SIG_STA_FORWARD_END					8
#define AP_SIG_STA_MSSID_NUM					9

#define IAPP_SIG_ASK_FOR_FORWARD_SKB			10
#define IAPP_SIG_FORWARD_SKB					11

#endif // IAPP_SUPPORT //


//
//  RTMP_ADAPTER flags
//
#define fRTMP_ADAPTER_MAP_REGISTER          0x00000001
#define fRTMP_ADAPTER_INTERRUPT_IN_USE      0x00000002
//#define fRTMP_ADAPTER_HARDWARE_ERROR        0x00000004
//#define fRTMP_ADAPTER_SCATTER_GATHER        0x00000008
//#define fRTMP_ADAPTER_SEND_PACKET_ERROR     0x00000010
#define fRTMP_ADAPTER_MLME_RESET_IN_PROGRESS    0x00000020
#define fRTMP_ADAPTER_HALT_IN_PROGRESS      0x00000040
#define fRTMP_ADAPTER_RESET_IN_PROGRESS     0x00000080
//#define fRTMP_ADAPTER_REMOVE_IN_PROGRESS	0x00000100
//#define fRTMP_ADAPTER_TX_RING_ALLOCATED   0x00000200
//#define fRTMP_ADAPTER_BULKOUT_RESET         0x00000400
//#define fRTMP_ADAPTER_BULKIN_RESET          0x00000800
//#define fRTMP_ADAPTER_RX_RING_ALLOCATED   0x00001000
#define fRTMP_ADAPTER_INTERRUPT_ACTIVE      0x00002000
#define fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS  0x00004000
#define fRTMP_ADAPTER_MEDIA_STATE_CHANGE    0x00008000
//#define   fRTMP_ADAPTER_MEDIA_STATE_PENDING   0x00010000
#define fRTMP_ADAPTER_RADIO_OFF             0x00020000
#define fRTMP_ADAPTER_RADIO_MEASUREMENT     0x00040000
//#define fRTMP_ADAPTER_MIMORATE_INUSED		0x00080000
#define fRTMP_ADAPTER_NIC_NOT_EXIST         0x02000000
#define fRTMP_ADAPTER_START_UP         		0x04000000				//Devive already initialized and enabled Tx/Rx.


#define fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET  	0x00800000



//#define fRTMP_ADAPTER_RDG_ACTIVE			0x00100000	
#define fRTMP_ADAPTER_DYNAMIC_BE_TXOP_ACTIVE			0x00200000	

#define fRTMP_ADAPTER_IDLE_RADIO_OFF        0x40000000
// Lock bit for accessing different ring buffers
//#define fRTMP_ADAPTER_TX_RING_BUSY        0x80000000
//#define fRTMP_ADAPTER_MGMT_RING_BUSY      0x40000000
//#define fRTMP_ADAPTER_ATIM_RING_BUSY      0x20000000
//#define fRTMP_ADAPTER_RX_RING_BUSY        0x10000000

// Lock bit for accessing different queue
//#define   fRTMP_ADAPTER_TX_QUEUE_BUSY     0x08000000
//#define   fRTMP_ADAPTER_MGMT_QUEUE_BUSY   0x04000000

//
//  STA operation status flags
//
#define fOP_STATUS_INFRA_ON                 0x00000001
#define fOP_STATUS_ADHOC_ON                 0x00000002
#define fOP_STATUS_BG_PROTECTION_INUSED     0x00000004
#define fOP_STATUS_SHORT_SLOT_INUSED        0x00000008
#define fOP_STATUS_SHORT_PREAMBLE_INUSED    0x00000010
#define fOP_STATUS_RECEIVE_DTIM             0x00000020
#define fOP_STATUS_TX_RATE_SWITCH_ENABLED   0x00000040
#define fOP_STATUS_MEDIA_STATE_CONNECTED    0x00000080
#define fOP_STATUS_WMM_INUSED               0x00000100
#define fOP_STATUS_AGGREGATION_INUSED       0x00000200
#define fOP_STATUS_DOZE                     0x00000400  // debug purpose
#define fOP_STATUS_PIGGYBACK_INUSED         0x00000800  // piggy-back, and aggregation
#define fOP_STATUS_APSD_INUSED				0x00001000
#define fOP_STATUS_TX_AMSDU_INUSED			0x00002000
#define fOP_STATUS_MAX_RETRY_ENABLED		0x00004000
#define fOP_STATUS_WAKEUP_NOW           0x00008000
//#define fOP_STATUS_KEEP_SLEEP_NOW               0x00010000

#define fOP_AP_STATUS_MEDIA_STATE_CONNECTED	0x00200000


#define CCKSETPROTECT		0x1
#define OFDMSETPROTECT		0x2
#define MM20SETPROTECT		0x4
#define MM40SETPROTECT		0x8
#define GF20SETPROTECT		0x10
#define GR40SETPROTECT		0x20
#define ALLN_SETPROTECT		(GR40SETPROTECT | GF20SETPROTECT | MM40SETPROTECT | MM20SETPROTECT)

//
//  AP's client table operation status flags
//
#define fCLIENT_STATUS_WMM_CAPABLE          0x00000001  // CLIENT can parse QOS DATA frame
#define fCLIENT_STATUS_AGGREGATION_CAPABLE  0x00000002  // CLIENT can receive Ralink's proprietary TX aggregation frame
#define fCLIENT_STATUS_PIGGYBACK_CAPABLE    0x00000004  // CLIENT support piggy-back
#define fCLIENT_STATUS_AMSDU_INUSED			0x00000008
#define fCLIENT_STATUS_SGI20_CAPABLE		0x00000010
#define fCLIENT_STATUS_SGI40_CAPABLE		0x00000020
#define fCLIENT_STATUS_TxSTBC_CAPABLE		0x00000040
#define fCLIENT_STATUS_RxSTBC_CAPABLE		0x00000080
#define fCLIENT_STATUS_HTC_CAPABLE			0x00000100
#define fCLIENT_STATUS_RDG_CAPABLE			0x00000200
#define fCLIENT_STATUS_MCSFEEDBACK_CAPABLE  0x00000400
#define fCLIENT_STATUS_APSD_CAPABLE         0x00000800  /* UAPSD STATION */
#define fCLIENT_STATUS_RALINK_CHIPSET		0x00100000
#if 1 //def DOT11N_DRAFT3
#define fCLIENT_STATUS_BSSCOEXIST_CAPABLE	0x00001000
#endif // DOT11N_DRAFT3 //
#define fCLIENT_STATUS_VHT_RX_LDPC_CAPABLE      0x00800000
#define fCLIENT_STATUS_HT_RX_LDPC_CAPABLE       0x01000000
#define SUPPORT_CCK_MODE        1
#define SUPPORT_OFDM_MODE       2
#define SUPPORT_HT_MODE         4
#define SUPPORT_VHT_MODE        8



#define PHY_CAP_2G(_x)		(((_x) & fPHY_CAP_24G) == fPHY_CAP_24G)
#define PHY_CAP_5G(_x)		(((_x) & fPHY_CAP_5G) == fPHY_CAP_5G)
#define PHY_CAP_N(_x)		(((_x) & fPHY_CAP_HT) == fPHY_CAP_HT)
#define PHY_CAP_AC(_x)		(((_x) & fPHY_CAP_VHT) == fPHY_CAP_VHT)

#ifndef WIFI_BUILD_RAM
enum {
	WMODE_INVALID = 0,
	WMODE_A = 1 << 0,
	WMODE_B = 1 << 1,
	WMODE_G = 1 << 2,
	WMODE_GN = 1 << 3,
	WMODE_AN = 1 << 4,
	WMODE_AC = 1 << 5,
	WMODE_COMP = 6,	/* total types of supported wireless mode, add this value once yow add new type */
};
#endif

#define WMODE_CAP_5G(_x)			(((_x) & (WMODE_A | WMODE_AN | WMODE_AC)) != 0)
#define WMODE_CAP_2G(_x)			(((_x) & (WMODE_B | WMODE_G | WMODE_GN)) != 0)
#define WMODE_CAP_N(_x)			(((_x) & (WMODE_GN | WMODE_AN)) != 0)
#define WMODE_CAP_AC(_x)		(((_x) & (WMODE_AC)) != 0)
#define WMODE_CAP(_x, _mode)	(((_x) & (_mode)) != 0)

#define WMODE_EQUAL(_x, _mode)	((_x) == (_mode))

#define WMODE_5G_ONLY(_x)		(((_x) & (WMODE_B | WMODE_G | WMODE_GN)) == 0)
#define WMODE_2G_ONLY(_x)		(((_x) & (WMODE_A | WMODE_AN | WMODE_AC)) == 0)
#define WMODE_HT_ONLY(_x)		(((_x) & (~(WMODE_GN | WMODE_AN | WMODE_AC))) == 0)
#define WMODE_VHT_ONLY(_x)		(((_x) & (~(WMODE_AC))) == 0)



//
//  STA configuration flags
//
//#define fSTA_CFG_ENABLE_TX_BURST          0x00000001

// 802.11n Operating Mode Definition. 0-3 also used in ASICUPdateProtect switch case 
#define HT_NO_PROTECT	0
#define HT_LEGACY_PROTECT	1
#define HT_40_PROTECT	2
#define HT_2040_PROTECT	3
#define HT_RTSCTS_6M	7
//following is our own definition in order to turn on our ASIC protection register in INFRASTRUCTURE.
#define HT_ATHEROS	8	// rt2860c has problem with atheros chip. we need to turn on RTS/CTS .
#define HT_FORCERTSCTS	9	// Force turn on RTS/CTS first. then go to evaluate if this force RTS is necessary.

//
// RX Packet Filter control flags. Apply on pAd->PacketFilter
//
#define fRX_FILTER_ACCEPT_DIRECT            NDIS_PACKET_TYPE_DIRECTED
#define fRX_FILTER_ACCEPT_MULTICAST         NDIS_PACKET_TYPE_MULTICAST
#define fRX_FILTER_ACCEPT_BROADCAST         NDIS_PACKET_TYPE_BROADCAST
#define fRX_FILTER_ACCEPT_ALL_MULTICAST     NDIS_PACKET_TYPE_ALL_MULTICAST

//
// Error code section
//
// NDIS_ERROR_CODE_ADAPTER_NOT_FOUND
#define ERRLOG_READ_PCI_SLOT_FAILED     0x00000101L
#define ERRLOG_WRITE_PCI_SLOT_FAILED    0x00000102L
#define ERRLOG_VENDOR_DEVICE_NOMATCH    0x00000103L

// NDIS_ERROR_CODE_ADAPTER_DISABLED
#define ERRLOG_BUS_MASTER_DISABLED      0x00000201L

// NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION
#define ERRLOG_INVALID_SPEED_DUPLEX     0x00000301L
#define ERRLOG_SET_SECONDARY_FAILED     0x00000302L

// NDIS_ERROR_CODE_OUT_OF_RESOURCES
#define ERRLOG_OUT_OF_MEMORY            0x00000401L
#define ERRLOG_OUT_OF_SHARED_MEMORY     0x00000402L
#define ERRLOG_OUT_OF_MAP_REGISTERS     0x00000403L
#define ERRLOG_OUT_OF_BUFFER_POOL       0x00000404L
#define ERRLOG_OUT_OF_NDIS_BUFFER       0x00000405L
#define ERRLOG_OUT_OF_PACKET_POOL       0x00000406L
#define ERRLOG_OUT_OF_NDIS_PACKET       0x00000407L
#define ERRLOG_OUT_OF_LOOKASIDE_MEMORY  0x00000408L

// NDIS_ERROR_CODE_HARDWARE_FAILURE
#define ERRLOG_SELFTEST_FAILED          0x00000501L
#define ERRLOG_INITIALIZE_ADAPTER       0x00000502L
#define ERRLOG_REMOVE_MINIPORT          0x00000503L

// NDIS_ERROR_CODE_RESOURCE_CONFLICT
#define ERRLOG_MAP_IO_SPACE             0x00000601L
#define ERRLOG_QUERY_ADAPTER_RESOURCES  0x00000602L
#define ERRLOG_NO_IO_RESOURCE           0x00000603L
#define ERRLOG_NO_INTERRUPT_RESOURCE    0x00000604L
#define ERRLOG_NO_MEMORY_RESOURCE       0x00000605L


// WDS definition
#define	MAX_WDS_ENTRY               4
#define WDS_PAIRWISE_KEY_OFFSET     60    // WDS links uses pairwise key#60 ~ 63 in ASIC pairwise key table

#define	WDS_DISABLE_MODE            0
#define	WDS_RESTRICT_MODE           1
#define	WDS_BRIDGE_MODE             2
#define	WDS_REPEATER_MODE           3
#define	WDS_LAZY_MODE               4


#ifdef APCLI_SUPPORT
#define MAX_APCLI_NUM(_pAd)				(_pAd->chipCap.MaxApCliNum)
#else
#define MAX_APCLI_NUM(_pAd)				0
#endif // APCLI_SUPPORT //

#ifdef MBSS_SUPPORT
#ifdef APCLI_SUPPORT
//#define MAX_MBSSID_NUM              7
#define MAX_MBSSID_NUM              8
#else
#define MAX_MBSSID_NUM(__pAd)              8
#endif // APCLI_SUPPORT //
#else
#define MAX_MBSSID_NUM(__pAd)              1
#endif /* !MBSS_SUPPORT */

/* sanity check for apidx */
#if 0
#define MBSS_MR_APIDX_SANITY_CHECK(apidx) \
    { if (apidx > MAX_MBSSID_NUM) { \
          printk("%s> Error! apidx = %d > MAX_MBSSID_NUM!\n", __FUNCTION__, apidx); \
	  apidx = MAIN_MBSSID; } }
#endif
#define MBSS_MR_APIDX_SANITY_CHECK(__pAd, apidx) \
    { if ((apidx >= MAX_MBSSID_NUM(__pAd)) || \
		(apidx >= HW_BEACON_MAX_NUM)) { \
          MTWF_LOG(DBG_CAT_AP, CATAP_MBSS, DBG_LVL_ERROR, ("%s> Error! apidx = %d > MAX_MBSSID_NUM!\n", __FUNCTION__, apidx)); \
	  apidx = MAIN_MBSSID; } }

#define MAIN_MBSSID                 0
#define FIRST_MBSSID                1


#define MAX_BEACON_SIZE				512
// If the MAX_MBSSID_NUM is larger than 6, 
// it shall reserve some WCID space(wcid 222~253) for beacon frames. 
// -	these wcid 238~253 are reserved for beacon#6(ra6).
// -	these wcid 222~237 are reserved for beacon#7(ra7).
#if defined(MAX_MBSSID_NUM) && (MAX_MBSSID_NUM == 8)
#define HW_RESERVED_WCID	222
#elif defined(MAX_MBSSID_NUM) && (MAX_MBSSID_NUM == 7)
#define HW_RESERVED_WCID	238
#else
#define HW_RESERVED_WCID	255
#endif

// Then dedicate wcid of DFS and Carrier-Sense.
#define DFS_CTS_WCID 		(HW_RESERVED_WCID - 1)
#define CS_CTS_WCID 		(HW_RESERVED_WCID - 2)
#define LAST_SPECIFIC_WCID	(HW_RESERVED_WCID - 2)

// If MAX_MBSSID_NUM is 8, the maximum available wcid for the associated STA is 211. 
// If MAX_MBSSID_NUM is 7, the maximum available wcid for the associated STA is 228.
#define MAX_AVAILABLE_CLIENT_WCID	(LAST_SPECIFIC_WCID - MAX_MBSSID_NUM - 1) 

// TX need WCID to find Cipher Key
// these wcid 212 ~ 219 are reserved for bc/mc packets if MAX_MBSSID_NUM is 8.
#define GET_GroupKey_WCID(_pAd,__wcid, __bssidx) \
	{										\
		__wcid = LAST_SPECIFIC_WCID - (MAX_MBSSID_NUM(_pAd)) + __bssidx;	\
	}

#define IsGroupKeyWCID(__wcid) (((__wcid) < LAST_SPECIFIC_WCID) && ((__wcid) >= (LAST_SPECIFIC_WCID - (MAX_MBSSID_NUM))))

// definition to support multiple BSSID
#define BSS0                            0
#define BSS1                            1
#define BSS2                            2
#define BSS3                            3
#define BSS4                            4
#define BSS5                            5
#define BSS6                            6
#define BSS7                            7


//============================================================
// Length definitions
#define PEER_KEY_NO                     2
#define MAC_ADDR_LEN                    6
#define TIMESTAMP_LEN                   8
#define MAX_LEN_OF_SUPPORTED_RATES      12    // 1, 2, 5.5, 11, 6, 9, 12, 18, 24, 36, 48, 54
#define MAX_LEN_OF_KEY                  32      // 32 octets == 256 bits, Redefine for WPA
#define MAX_NUM_OF_CHANNELS             49      // 14 channels @2.4G +  12@UNII + 4 @MMAC + 11 @HiperLAN2 + 7 @Japan + 1 as NULL termination
#define MAX_NUM_OF_11JCHANNELS             20      // 14 channels @2.4G +  12@UNII + 4 @MMAC + 11 @HiperLAN2 + 7 @Japan + 1 as NULL termination
// #define MAX_LEN_OF_SSID                 32
#define CIPHER_TEXT_LEN                 128

#define MAX_VIE_LEN                     384   // New for WPA cipher suite variable IE sizes.
#define MAX_SUPPORT_MCS             32    

//============================================================
// ASIC WCID Table definition.
//============================================================

#define APCLI_MCAST_WCID    (MAX_LEN_OF_MAC_TABLE(pAd) + HW_BEACON_MAX_NUM + MAX_APCLI_NUM(pAd))

#define BSSID_WCID		1	// in infra mode, always put bssid with this WCID 
#define MCAST_WCID	0x0	 
#define BSS0Mcast_WCID	0x0	 
#define BSS1Mcast_WCID	0xf8	 
#define BSS2Mcast_WCID	0xf9
#define BSS3Mcast_WCID	0xfa
#define BSS4Mcast_WCID	0xfb
#define BSS5Mcast_WCID	0xfc
#define BSS6Mcast_WCID	0xfd
#define BSS7Mcast_WCID	0xfe
#define RESERVED_WCID		0xff	

//============================================================
// Unify Mac Definition
//============================================================
#define STA_OWNMAC_IDX 0	// for Addr2
#define STA_BSSID_IDX  BSS0  // for Addr3

#define HW_BEACON_MAX_NUM				1
#define MAX_NUM_OF_ACL_LIST				64

#ifdef CONFIG_AP_SUPPORT
#define MAX_LEN_OF_MAC_TABLE(_pAd)            (_pAd->chipCap.MaxSTANum)   //IoT: only 4 clients to be supported //128 //32 // if MAX_MBSSID_NUM is 8, this value can't be larger than 211
#else
#define MAX_LEN_OF_MAC_TABLE(_pAd)            2  //16 //32
#endif
#define MAX_LEN_OF_TR_TABLE(_pAd) 	(MAX_LEN_OF_MAC_TABLE(_pAd) + HW_BEACON_MAX_NUM)

#if MAX_LEN_OF_MAC_TABLE>MAX_AVAILABLE_CLIENT_WCID
#error MAX_LEN_OF_MAC_TABLE can not be larger than MAX_AVAILABLE_CLIENT_WCID!!!!
#endif

#define HASH_TABLE_SIZE(_pAd)                 (MAX_LEN_OF_MAC_TABLE(_pAd) * 2) // 256

#define MAX_NUM_OF_WDS_LINK_PERBSSID	            3
//#define MAX_NUM_OF_WDS_LINK	            (MAX_NUM_OF_WDS_LINK_PERBSSID*MAX_MBSSID_NUM)
#define MAX_NUM_OF_EVENT                10  // entry # in EVENT table
#define WDS_LINK_START_WCID				(MAX_LEN_OF_MAC_TABLE(pAd)-1)	

#define NUM_OF_TID			8
#define MAX_AID_BA                    4
#ifdef CONFIG_AP_SUPPORT
#define MAX_LEN_OF_BA_REC_TABLE(_pAd)          ((NUM_OF_TID * MAX_LEN_OF_MAC_TABLE(_pAd))/2)//   (NUM_OF_TID*MAX_AID_BA + 32)	 //Block ACK recipient 
#define MAX_LEN_OF_BA_ORI_TABLE(_pAd)          ((NUM_OF_TID * MAX_LEN_OF_MAC_TABLE(_pAd))/2)//   (NUM_OF_TID*MAX_AID_BA + 32)   // Block ACK originator
#else // CONFIG_AP_SUPPORT
#define MAX_LEN_OF_BA_REC_TABLE(_pAd)          (4)//   (NUM_OF_TID*MAX_AID_BA + 32)	 //Block ACK recipient 
#define MAX_LEN_OF_BA_ORI_TABLE(_pAd)          (4)//   (NUM_OF_TID*MAX_AID_BA + 32)   // Block ACK originator
#endif 

extern int g_MAX_LEN_OF_BSS_TABLE;
#define MAX_LEN_OF_BSS_TABLE             g_MAX_LEN_OF_BSS_TABLE //4   // (64)
//#define MAX_LEN_OF_BSS_TABLE             4 //4   // (64)

#ifdef CONFIG_AP_SUPPORT
#define MAX_REORDERING_MPDU_NUM(_pAd)			 _pAd->chipCap.MaxReorderMpduNum
#else
#define MAX_REORDERING_MPDU_NUM(_pAd)			 32 //16
#endif

#define MAX_TX_POWER_LEVEL              100   /* mW */
#define MAX_RSSI_TRIGGER                -10    /* dBm */
#define MIN_RSSI_TRIGGER                -200   /* dBm */
#define MAX_FRAG_THRESHOLD              2346  /* byte count */
#define MIN_FRAG_THRESHOLD              256   /* byte count */
#define MAX_RTS_THRESHOLD               2347  /* byte count */

// key related definitions
#define SHARE_KEY_NUM                   4
#define MAX_LEN_OF_SHARE_KEY            16    // byte count
#define MAX_LEN_OF_PEER_KEY             16    // byte count
#define PAIRWISE_KEY_NUM                64    // in MAC ASIC pairwise key table
#define GROUP_KEY_NUM                   4
#define PMK_LEN                         32
#define WDS_PAIRWISE_KEY_OFFSET         60    // WDS links uses pairwise key#60 ~ 63 in ASIC pairwise key table
#define	PMKID_NO                        4     // Number of PMKID saved supported
#define MAX_LEN_OF_MLME_BUFFER          2048

// power status related definitions
#define PWR_ACTIVE                      0
#define PWR_SAVE                        1
#define PWR_MMPS                        2			//MIMO power save
//#define PWR_UNKNOWN                   2


/* For AP PS retrieve status */
#define APPS_RETRIEVE_IDLE       0
#define APPS_RETRIEVE_CR_PADDING	1
#define APPS_RETRIEVE_START_PS		2
#define APPS_RETRIEVE_GOING			3
#define APPS_RETRIEVE_WAIT_EVENT	4
#define APPS_RETRIEVE_DONE			5

// Auth and Assoc mode related definitions
//#define AUTH_MODE_OPEN                  0x00
//#define AUTH_MODE_KEY                   0x01
//#define AUTH_MODE_AUTO_SWITCH         0x03
//#define AUTH_MODE_DEAUTH              0x04
//#define AUTH_MODE_UPLAYER             0x05 // reserved for 802.11i use

// BSS Type definitions
#define BSS_ADHOC                       0  // = Ndis802_11IBSS
#define BSS_INFRA                       1  // = Ndis802_11Infrastructure
#define BSS_ANY                         2  // = Ndis802_11AutoUnknown
#define BSS_MONITOR			            3  // = Ndis802_11Monitor


// Reason code definitions
#define REASON_RESERVED                 0
#define REASON_UNSPECIFY                1
#define REASON_NO_LONGER_VALID          2
#define REASON_DEAUTH_STA_LEAVING       3
#define REASON_DISASSOC_INACTIVE        4
#define REASON_DISASSPC_AP_UNABLE       5
#define REASON_CLS2ERR                  6
#define REASON_CLS3ERR                  7
#define REASON_DISASSOC_STA_LEAVING     8
#define REASON_STA_REQ_ASSOC_NOT_AUTH   9
#define REASON_INVALID_IE               13
#define REASON_MIC_FAILURE              14
#define REASON_4_WAY_TIMEOUT            15
#define REASON_GROUP_KEY_HS_TIMEOUT     16
#define REASON_IE_DIFFERENT             17
#define REASON_MCIPHER_NOT_VALID        18
#define REASON_UCIPHER_NOT_VALID        19
#define REASON_AKMP_NOT_VALID           20
#define REASON_UNSUPPORT_RSNE_VER       21
#define REASON_INVALID_RSNE_CAP         22
#define REASON_8021X_AUTH_FAIL          23
#define REASON_CIPHER_SUITE_REJECTED    24
#define REASON_DECLINED                 37

#define REASON_QOS_UNSPECIFY              32	
#define REASON_QOS_LACK_BANDWIDTH         33	
#define REASON_POOR_CHANNEL_CONDITION     34
#define REASON_QOS_OUTSIDE_TXOP_LIMITION  35
#define REASON_QOS_QSTA_LEAVING_QBSS      36
#define REASON_QOS_UNWANTED_MECHANISM     37
#define REASON_QOS_MECH_SETUP_REQUIRED    38
#define REASON_QOS_REQUEST_TIMEOUT        39
#define REASON_QOS_CIPHER_NOT_SUPPORT     45

// Status code definitions
#define MLME_SUCCESS                    0
#define MLME_UNSPECIFY_FAIL             1
#define MLME_CANNOT_SUPPORT_CAP         10
#define MLME_REASSOC_DENY_ASSOC_EXIST   11 
#define MLME_ASSOC_DENY_OUT_SCOPE       12
#define MLME_ALG_NOT_SUPPORT            13
#define MLME_SEQ_NR_OUT_OF_SEQUENCE     14
#define MLME_REJ_CHALLENGE_FAILURE      15
#define MLME_REJ_TIMEOUT                  16
#define MLME_ASSOC_REJ_UNABLE_HANDLE_STA  17
#define MLME_ASSOC_REJ_DATA_RATE          18

#define MLME_ASSOC_REJ_NO_EXT_RATE        22
#define MLME_ASSOC_REJ_NO_EXT_RATE_PBCC   23
#define MLME_ASSOC_REJ_NO_CCK_OFDM        24

#define MLME_QOS_UNSPECIFY                32
#define MLME_REQUEST_DECLINED             37
#define MLME_REQUEST_WITH_INVALID_PARAM   38
#define MLME_INVALID_INFORMATION_ELEMENT  40
#define MLME_INVALID_GROUP_CIPHER   	  41
#define MLME_INVALID_PAIRWISE_CIPHER   	  42
#define MLME_INVALID_AKMP   			  43
#define MLME_NOT_SUPPORT_RSN_VERSION	  44
#define	MLME_INVALID_RSN_CAPABILITIES	  45
#define MLME_INVALID_SECURITY_POLICY      46 /* Cipher suite rejected because of security policy */
#define MLME_DLS_NOT_ALLOW_IN_QBSS        48
#define MLME_DEST_STA_NOT_IN_QBSS         49
#define MLME_DEST_STA_IS_NOT_A_QSTA       50

#define MLME_INVALID_FORMAT             0x51
#define MLME_FAIL_NO_RESOURCE           0x52
#define MLME_STATE_MACHINE_REJECT       0x53
#define MLME_MAC_TABLE_FAIL             0x54

// IE code
#define IE_SSID                         0
#define IE_SUPP_RATES                   1
#define IE_FH_PARM                      2
#define IE_DS_PARM                      3
#define IE_CF_PARM                      4
#define IE_TIM                          5
#define IE_IBSS_PARM                    6
#define IE_COUNTRY                      7     // 802.11d
#define IE_802_11D_REQUEST              10    // 802.11d
#define IE_QBSS_LOAD                    11    // 802.11e d9
#define IE_EDCA_PARAMETER               12    // 802.11e d9
#define IE_TSPEC                        13    // 802.11e d9 
#define IE_TCLAS                        14    // 802.11e d9
#define IE_SCHEDULE                     15    // 802.11e d9
#define IE_CHALLENGE_TEXT               16
#define IE_POWER_CONSTRAINT             32    // 802.11h d3.3
#define IE_POWER_CAPABILITY             33    // 802.11h d3.3
#define IE_TPC_REQUEST                  34    // 802.11h d3.3
#define IE_TPC_REPORT                   35    // 802.11h d3.3
#define IE_SUPP_CHANNELS                36    // 802.11h d3.3
#define IE_CHANNEL_SWITCH_ANNOUNCEMENT  37    // 802.11h d3.3
#define IE_MEASUREMENT_REQUEST          38    // 802.11h d3.3
#define IE_MEASUREMENT_REPORT           39    // 802.11h d3.3
#define IE_QUIET                        40    // 802.11h d3.3
#define IE_IBSS_DFS                     41    // 802.11h d3.3
#define IE_ERP                          42    // 802.11g
#define IE_TS_DELAY                     43    // 802.11e d9
#define IE_TCLAS_PROCESSING             44    // 802.11e d9
#define IE_QOS_CAPABILITY               46    // 802.11e d6
#define IE_HT_CAP                       45    // 802.11n d1. HT CAPABILITY. ELEMENT ID TBD
#define IE_AP_CHANNEL_REPORT			51    // 802.11k d6
#define IE_HT_CAP2                         52    // 802.11n d1. HT CAPABILITY. ELEMENT ID TBD
#define IE_RSN                          48    // 802.11i d3.0
#define IE_WPA2                         48    // WPA2

#define IE_EXT_SUPP_RATES               50    // 802.11g
#define IE_SUPP_REG_CLASS               59    // 802.11y. Supported regulatory classes.
#define IE_EXT_CHANNEL_SWITCH_ANNOUNCEMENT	60	// 802.11n 
#define IE_ADD_HT                         61    // 802.11n d1. ADDITIONAL HT CAPABILITY. ELEMENT ID TBD
#define IE_ADD_HT2                        53    // 802.11n d1. ADDITIONAL HT CAPABILITY. ELEMENT ID TBD
#define IE_NEW_EXT_CHA_OFFSET                         62    // 802.11n d1. New extension channel offset elemet
#define IE_EXT_CAPABILITY				127
#define IE_WPA                          221   // WPA
#define IE_VENDOR_SPECIFIC              221   // Wifi WMM (WME)
#ifdef WSC_STA_SUPPORT
#define	IE_WFA_WSC							221
#endif // WSC_STA_SUPPORT //

#if 1 //def DOT11N_DRAFT3
#define fOP_STATUS_SCAN_2040               	    0x00040000
#endif // DOT11N_DRAFT3 //
// For 802.11n D3.03
//#define IE_NEW_EXT_CHA_OFFSET             62    // 802.11n d1. New extension channel offset elemet
#define IE_SECONDARY_CH_OFFSET		62	// 802.11n D3.03	Secondary Channel Offset element
#define IE_WAPI							68		// WAPI information element. Same as Bss Ac Access Dealy Element.
#define IE_2040_BSS_COEXIST               72    // 802.11n D3.0.3
#define IE_2040_BSS_INTOLERANT_REPORT     73    // 802.11n D3.03
#define IE_OVERLAPBSS_SCAN_PARM           74    // 802.11n D3.03
#define IE_EXT_CAPABILITY                127   // 802.11n D3.03 



#define OUI_BROADCOM_HT              51   //  
#define OUI_BROADCOM_HTADD              52   //  
#define OUI_PREN_HT_CAP              51   //  
#define OUI_PREN_ADD_HT              52   //  

// CCX information
#define IE_AIRONET_CKIP                 133   // CCX1.0 ID 85H for CKIP 
#define IE_AP_TX_POWER                  150   // CCX 2.0 for AP transmit power
#define IE_MEASUREMENT_CAPABILITY       221   // CCX 2.0
#define IE_CCX_V2                       221
#define IE_AIRONET_IPADDRESS            149   // CCX ID 95H for IP Address
#define IE_AIRONET_CCKMREASSOC          156   // CCX ID 9CH for CCKM Reassociation Request element
#define CKIP_NEGOTIATION_LENGTH         30
#define AIRONET_IPADDRESS_LENGTH        10
#define AIRONET_CCKMREASSOC_LENGTH      24

// ========================================================
// MLME state machine definition
// ========================================================

// STA MLME state mahcines
#define ASSOC_STATE_MACHINE             1
#define AUTH_STATE_MACHINE              2
#define AUTH_RSP_STATE_MACHINE          3
#define SYNC_STATE_MACHINE              4
#define MLME_CNTL_STATE_MACHINE         5
#define WPA_PSK_STATE_MACHINE           6
#define LEAP_STATE_MACHINE              7
#define AIRONET_STATE_MACHINE           8
#define ACTION_STATE_MACHINE           9

// AP MLME state machines
#define AP_ASSOC_STATE_MACHINE          11
#define AP_AUTH_STATE_MACHINE           12
#define AP_AUTH_RSP_STATE_MACHINE       13
#define AP_SYNC_STATE_MACHINE           14
#define AP_CNTL_STATE_MACHINE           15
#define AP_WPA_STATE_MACHINE            16

#define WSC_STATE_MACHINE            17
#define WSC_UPNP_STATE_MACHINE		    18

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
#define APCLI_AUTH_STATE_MACHINE			19
#define APCLI_ASSOC_STATE_MACHINE			20
#define APCLI_SYNC_STATE_MACHINE			21
#define APCLI_CTRL_STATE_MACHINE			22
//#define APCLI_WPA_STATE_MACHINE				23
#endif // APCLI_SUPPORT //
#endif // CONFIG_AP_SUPPORT //


#define WPA_STATE_MACHINE            		23

//
// STA's CONTROL/CONNECT state machine: states, events, total function #
//
#define CNTL_IDLE                       0
#define CNTL_WAIT_DISASSOC              1
#define CNTL_WAIT_JOIN                  2
#define CNTL_WAIT_REASSOC               3
#define CNTL_WAIT_START                 4
#define CNTL_WAIT_AUTH                  5
#define CNTL_WAIT_ASSOC                 6
#define CNTL_WAIT_AUTH2                 7
#define CNTL_WAIT_OID_LIST_SCAN         8
#define CNTL_WAIT_OID_DISASSOC          9
#ifdef RT2870
#define CNTL_WAIT_SCAN_FOR_CONNECT      10
#endif // RT2870 //

#define MT2_ASSOC_CONF                  34
#define MT2_AUTH_CONF                   35
#define MT2_DEAUTH_CONF                 36
#define MT2_DISASSOC_CONF               37
#define MT2_REASSOC_CONF                38
#define MT2_PWR_MGMT_CONF               39
#define MT2_JOIN_CONF                   40
#define MT2_SCAN_CONF                   41
#define MT2_START_CONF                  42
#define MT2_GET_CONF                    43
#define MT2_SET_CONF                    44
#define MT2_RESET_CONF                  45
#define MT2_MLME_ROAMING_REQ            52

#define CNTL_FUNC_SIZE                  1

//
// STA's ASSOC state machine: states, events, total function #
//
#define ASSOC_IDLE                      0
#define ASSOC_WAIT_RSP                  1
#define REASSOC_WAIT_RSP                2
#define DISASSOC_WAIT_RSP               3
#define MAX_ASSOC_STATE                 4

#define ASSOC_MACHINE_BASE              0
#define MT2_MLME_ASSOC_REQ              0
#define MT2_MLME_REASSOC_REQ            1
#define MT2_MLME_DISASSOC_REQ           2  
#define MT2_PEER_DISASSOC_REQ           3
#define MT2_PEER_ASSOC_REQ              4
#define MT2_PEER_ASSOC_RSP              5
#define MT2_PEER_REASSOC_REQ            6
#define MT2_PEER_REASSOC_RSP            7
#define MT2_DISASSOC_TIMEOUT            8
#define MT2_ASSOC_TIMEOUT               9
#define MT2_REASSOC_TIMEOUT             10
#define MAX_ASSOC_MSG                   11

#define ASSOC_FUNC_SIZE                 (MAX_ASSOC_STATE * MAX_ASSOC_MSG)

//
// ACT state machine: states, events, total function #
//
#define ACT_IDLE                      0
#define MAX_ACT_STATE                 1

#define ACT_MACHINE_BASE              0
//Category 
#define MT2_PEER_SPECTRUM_CATE              0
#define MT2_PEER_QOS_CATE              1
#define MT2_PEER_DLS_CATE             2
#define MT2_PEER_BA_CATE             3
#define MT2_PEER_PUBLIC_CATE      4
#define MT2_PEER_RM_CATE             5 // 4
#define MT2_PEER_BSSTRANS_CATE             5
#define MT2_PEER_HT_CATE             7	//	7.4.7
#define MAX_PEER_CATE_MSG                   7
#define MT2_MLME_ADD_BA_CATE             8
#define MT2_MLME_ORI_DELBA_CATE             9
#define MT2_MLME_REC_DELBA_CATE             10
#define MT2_MLME_QOS_CATE              11
#define MT2_MLME_DLS_CATE             12
#define MT2_ACT_INVALID             13
#define MAX_ACT_MSG                   14

//Public's  Action field value in Public Category.  Some in 802.11y and some in 11n
#define ACTION_BSS_2040_COEXIST				0	// 11n
#define ACTION_DSE_ENABLEMENT					1	// 11y D9.0
#define ACTION_DSE_DEENABLEMENT				2	// 11y D9.0
#define ACTION_DSE_REG_LOCATION_ANNOUNCE	3	// 11y D9.0
#define ACTION_EXT_CH_SWITCH_ANNOUNCE		4	// 11y D9.0
#define ACTION_DSE_MEASUREMENT_REQ			5	// 11y D9.0
#define ACTION_DSE_MEASUREMENT_REPORT		6	// 11y D9.0
#define ACTION_MEASUREMENT_PILOT_ACTION		7  	// 11y D9.0
#define ACTION_DSE_POWER_CONSTRAINT			8	// 11y D9.0
#define ACTION_VENDOR_USAGE					221

//Category field
#define CATEGORY_SPECTRUM	0
#define CATEGORY_QOS		1
#define CATEGORY_DLS		2
#define CATEGORY_BA			3
#define CATEGORY_PUBLIC		4
#define CATEGORY_RM			5
#define CATEGORY_FT			6
#define CATEGORY_HT			7
#define CATEGORY_PMF			8	/* defined in IEEE 802.11w-D8.0 7.3.1.11*/
#ifdef DOT11Z_TDLS_SUPPORT
#define CATEGORY_TDLS		10
#endif // DOT11Z_TDLS_SUPPORT //


// DLS Action frame definition
#define ACTION_DLS_REQUEST			0
#define ACTION_DLS_RESPONSE			1
#define ACTION_DLS_TEARDOWN			2

//Spectrum  Action field value 802.11h 7.4.1
#define SPEC_MRQ	0	// Request
#define SPEC_MRP	1	//Report
#define SPEC_TPCRQ	2
#define SPEC_TPCRP	3
#define SPEC_CHANNEL_SWITCH	4


//BA  Action field value
#define ADDBA_REQ	0
#define ADDBA_RESP	1
#define DELBA   2


//HT  Action field value
#define NOTIFY_BW_ACTION	0
#define SMPS_ACTION		1
#define PSMP_ACTION   2
#define SETPCO_ACTION	3
#define MIMO_CHA_MEASURE_ACTION		4
#define MIMO_N_BEACONFORM		5
#define MIMO_BEACONFORM		6
#define ANTENNA_SELECT			7
#define HT_INFORMATION_EXCHANGE_ACTION	8

#define ACT_FUNC_SIZE                 (MAX_ACT_STATE * MAX_ACT_MSG)
//
// STA's AUTHENTICATION state machine: states, evvents, total function #
//
#define AUTH_REQ_IDLE                   0
#define AUTH_WAIT_SEQ2                  1
#define AUTH_WAIT_SEQ4                  2
#define MAX_AUTH_STATE                  3

#define AUTH_MACHINE_BASE               0
#define MT2_MLME_AUTH_REQ               0
#define MT2_PEER_AUTH_EVEN              1
#define MT2_AUTH_TIMEOUT                2
#define MAX_AUTH_MSG                    3

#define AUTH_FUNC_SIZE                  (MAX_AUTH_STATE * MAX_AUTH_MSG)

//
// STA's AUTH_RSP state machine: states, events, total function #
//
#define AUTH_RSP_IDLE                   0
#define AUTH_RSP_WAIT_CHAL              1
#define MAX_AUTH_RSP_STATE              2

#define AUTH_RSP_MACHINE_BASE           0
#define MT2_AUTH_CHALLENGE_TIMEOUT      0
#define MT2_PEER_AUTH_ODD               1
#define MT2_PEER_DEAUTH                 2
#define MAX_AUTH_RSP_MSG                3

#define AUTH_RSP_FUNC_SIZE              (MAX_AUTH_RSP_STATE * MAX_AUTH_RSP_MSG)

//
// STA's SYNC state machine: states, events, total function #
//
#define SYNC_IDLE                       0  // merge NO_BSS,IBSS_IDLE,IBSS_ACTIVE and BSS in to 1 state
#define JOIN_WAIT_BEACON                1
#define SCAN_LISTEN                     2
#define MAX_SYNC_STATE                  3

#define SYNC_MACHINE_BASE               0
#define MT2_MLME_SCAN_REQ               0
#define MT2_MLME_JOIN_REQ               1
#define MT2_MLME_START_REQ              2
#define MT2_PEER_BEACON                 3
#define MT2_PEER_PROBE_RSP              4
#define MT2_PEER_ATIM                   5
#define MT2_SCAN_TIMEOUT                6
#define MT2_BEACON_TIMEOUT              7
#define MT2_ATIM_TIMEOUT                8
#define MT2_PEER_PROBE_REQ              9
#define MAX_SYNC_MSG                    10

#define SYNC_FUNC_SIZE                  (MAX_SYNC_STATE * MAX_SYNC_MSG)

//Messages for the DLS state machine
#define DLS_IDLE						0
#define MAX_DLS_STATE					1

#define DLS_MACHINE_BASE				0
#define MT2_PEER_DLS_REQ				0
#define MT2_PEER_DLS_RSP				1
#define MT2_PEER_DLS_TEAR_DOWN			2
#define MAX_DLS_MSG						3

#define DLS_FUNC_SIZE					(MAX_DLS_STATE * MAX_DLS_MSG)

//
// STA's WPA-PSK State machine: states, events, total function #
// 
#define WPA_PSK_IDLE					0
#define MAX_WPA_PSK_STATE				1

#define WPA_MACHINE_BASE                0
#define MT2_EAPPacket                   0
#define MT2_EAPOLStart                  1
#define MT2_EAPOLLogoff                 2
#define MT2_EAPOLKey                    3
#define MT2_EAPOLASFAlert               4
#define MAX_WPA_PSK_MSG                 5

#define	WPA_PSK_FUNC_SIZE				(MAX_WPA_PSK_STATE * MAX_WPA_PSK_MSG)

//
// STA's CISCO-AIRONET State machine: states, events, total function #
//
#define AIRONET_IDLE					0
#define	AIRONET_SCANNING				1
#define MAX_AIRONET_STATE				2

#define AIRONET_MACHINE_BASE		    0
#define MT2_AIRONET_MSG				    0
#define MT2_AIRONET_SCAN_REQ		    1
#define MT2_AIRONET_SCAN_DONE		    2
#define MAX_AIRONET_MSG				    3

#define	AIRONET_FUNC_SIZE				(MAX_AIRONET_STATE * MAX_AIRONET_MSG)

// 
// WSC State machine: states, events, total function #
//
#ifdef WSC_INCLUDED
//Messages for the WSC State machine
#define	WSC_IDLE						0
#define	MAX_WSC_STATE					1
#define	WSC_FUNC_SIZE					(MAX_WSC_STATE * MAX_WSC_MSG)
#if 0
/* hdr->data:
	    WscPBCBssCount(4)
	    Bssid(6)
	    Channel(1)
	    SsidLen(1)
	    Ssid(n)
 */
#define 	PBC_RSP_BSSID_OFFSET 4
#define 	PBC_RSP_CHANNEL_OFFSET (PBC_RSP_BSSID_OFFSET+MAC_ADDR_LEN)
#define 	PBC_RSP_SSIDLEN_OFFSET (PBC_RSP_CHANNEL_OFFSET+1)
#define 	PBC_RSP_SSID_OFFSET 	   (PBC_RSP_SSIDLEN_OFFSET+1)
#endif // if 0

#endif // WSC_INCLUDED //

// 
// AP's CONTROL/CONNECT state machine: states, events, total function #
// 
#define AP_CNTL_FUNC_SIZE               1

//
// AP's ASSOC state machine: states, events, total function #
//
#define AP_ASSOC_IDLE                   0
#define AP_MAX_ASSOC_STATE              1

#define AP_ASSOC_MACHINE_BASE           0
#define APMT2_MLME_DISASSOC_REQ         0
#define APMT2_PEER_DISASSOC_REQ         1
#define APMT2_PEER_ASSOC_REQ            2
#define APMT2_PEER_REASSOC_REQ          3
#define APMT2_CLS3ERR                   4
#define AP_MAX_ASSOC_MSG                5

#define AP_ASSOC_FUNC_SIZE              (AP_MAX_ASSOC_STATE * AP_MAX_ASSOC_MSG)

//
// AP's AUTHENTICATION state machine: states, events, total function #
//
#define AP_AUTH_REQ_IDLE                0
#define AP_MAX_AUTH_STATE               1

#define AP_AUTH_MACHINE_BASE            0
#define APMT2_MLME_DEAUTH_REQ           0
#define APMT2_CLS2ERR                   1
#define APMT2_PEER_DEAUTH				2
#define APMT2_PEER_AUTH_REQ				3
#define APMT2_PEER_AUTH_CONFIRM			4
#define AP_MAX_AUTH_MSG                 5


#define AP_AUTH_FUNC_SIZE               (AP_MAX_AUTH_STATE * AP_MAX_AUTH_MSG)

//
// AP's AUTH-RSP state machine: states, events, total function #
//
#define AP_AUTH_RSP_IDLE                0
#define AP_MAX_AUTH_RSP_STATE           1

#define AP_AUTH_RSP_MACHINE_BASE        0
#define APMT2_AUTH_CHALLENGE_TIMEOUT    0
#define APMT2_PEER_AUTH_ODD             1
#define APMT2_PEER_DEAUTH               2
#define AP_MAX_AUTH_RSP_MSG             3

#define AP_AUTH_RSP_FUNC_SIZE           (AP_MAX_AUTH_RSP_STATE * AP_MAX_AUTH_RSP_MSG)

//
// AP's SYNC state machine: states, events, total function #
//
#define AP_SYNC_IDLE                    0
#define AP_SCAN_LISTEN					1
#define AP_MAX_SYNC_STATE               2

#define AP_SYNC_MACHINE_BASE            0
#define APMT2_PEER_PROBE_REQ            0
#define APMT2_PEER_BEACON               1
#define APMT2_MLME_SCAN_REQ				2
#define APMT2_PEER_PROBE_RSP			3
#define APMT2_SCAN_TIMEOUT				4
#define AP_MAX_SYNC_MSG                 5

#define AP_SYNC_FUNC_SIZE               (AP_MAX_SYNC_STATE * AP_MAX_SYNC_MSG)

//
// AP's WPA state machine: states, events, total function #
//
#define AP_WPA_PTK                      0
#define AP_MAX_WPA_PTK_STATE            1

#define AP_WPA_MACHINE_BASE             0
#define APMT2_EAPPacket                 0
#define APMT2_EAPOLStart                1
#define APMT2_EAPOLLogoff               2
#define APMT2_EAPOLKey                  3
#define APMT2_EAPOLASFAlert             4
#define AP_MAX_WPA_MSG                  5

#define AP_WPA_FUNC_SIZE                (AP_MAX_WPA_PTK_STATE * AP_MAX_WPA_MSG)

#ifdef APCLI_SUPPORT
//ApCli authentication state machine
#define APCLI_AUTH_REQ_IDLE                0
#define APCLI_AUTH_WAIT_SEQ2               1
#define APCLI_AUTH_WAIT_SEQ4               2
#define APCLI_MAX_AUTH_STATE               3

#define APCLI_AUTH_MACHINE_BASE            0
#define APCLI_MT2_MLME_AUTH_REQ            0
#define APCLI_MT2_MLME_DEAUTH_REQ          1
#define APCLI_MT2_PEER_AUTH_EVEN           2
#define APCLI_MT2_PEER_DEAUTH              3
#define APCLI_MT2_AUTH_TIMEOUT             4
#define APCLI_MAX_AUTH_MSG                 5

#define APCLI_AUTH_FUNC_SIZE               (APCLI_MAX_AUTH_STATE * APCLI_MAX_AUTH_MSG)

//ApCli association state machine
#define APCLI_ASSOC_IDLE                   0
#define APCLI_ASSOC_WAIT_RSP               1
#define APCLI_MAX_ASSOC_STATE              2

#define APCLI_ASSOC_MACHINE_BASE           0
#define APCLI_MT2_MLME_ASSOC_REQ           0
#define APCLI_MT2_MLME_DISASSOC_REQ        1  
#define APCLI_MT2_PEER_DISASSOC_REQ        2
#define APCLI_MT2_PEER_ASSOC_RSP           3
#define APCLI_MT2_ASSOC_TIMEOUT            4
#define APCLI_MAX_ASSOC_MSG                5

#define APCLI_ASSOC_FUNC_SIZE              (APCLI_MAX_ASSOC_STATE * APCLI_MAX_ASSOC_MSG)

//ApCli sync state machine
#define APCLI_SYNC_IDLE                   0  // merge NO_BSS,IBSS_IDLE,IBSS_ACTIVE and BSS in to 1 state
#define APCLI_JOIN_WAIT_PROBE_RSP         1
#define APCLI_MAX_SYNC_STATE              2

#define APCLI_SYNC_MACHINE_BASE           0
#define APCLI_MT2_MLME_PROBE_REQ          0
#define APCLI_MT2_PEER_PROBE_RSP          1
#define APCLI_MT2_PEER_BEACON             2
#define APCLI_MT2_PROBE_TIMEOUT           3
#define APCLI_MAX_SYNC_MSG                4

#define APCLI_SYNC_FUNC_SIZE              (APCLI_MAX_SYNC_STATE * APCLI_MAX_SYNC_MSG)

//ApCli ctrl state machine
#define APCLI_CTRL_DISCONNECTED           0  // merge NO_BSS,IBSS_IDLE,IBSS_ACTIVE and BSS in to 1 state
#define APCLI_CTRL_PROBE                  1
#define APCLI_CTRL_AUTH                   2
#define APCLI_CTRL_AUTH_2                 3
#define APCLI_CTRL_ASSOC                  4
#define APCLI_CTRL_DEASSOC                5
#define APCLI_CTRL_CONNECTED              6
#define APCLI_MAX_CTRL_STATE              7

#define APCLI_CTRL_MACHINE_BASE           0
#define APCLI_CTRL_JOIN_REQ               0
#define APCLI_CTRL_PROBE_RSP              1
#define APCLI_CTRL_AUTH_RSP               2
#define APCLI_CTRL_DISCONNECT_REQ         3
#define APCLI_CTRL_PEER_DISCONNECT_REQ    4
#define APCLI_CTRL_ASSOC_RSP              5
#define APCLI_CTRL_DEASSOC_RSP            6
#define APCLI_CTRL_JOIN_REQ_TIMEOUT       7
#define APCLI_CTRL_AUTH_REQ_TIMEOUT       8
#define APCLI_CTRL_ASSOC_REQ_TIMEOUT      9
#define APCLI_CTRL_MT2_AUTH_REQ			  10
#define APCLI_MAX_CTRL_MSG                11

#define APCLI_CTRL_FUNC_SIZE              (APCLI_MAX_CTRL_STATE * APCLI_MAX_CTRL_MSG)

#if 0	// remove those variables by AlbertY
// ApCli WPA state machine
#define APCLI_WPA_PSK_IDLE				0
#define APCLI_MAX_WPA_PSK_STATE			1

// ApCli WPA MSG Type
#define APCLI_WPA_MACHINE_BASE			0
#define APCLI_MT2_EAPPacket				0
#define APCLI_MT2_EAPOLStart			1
#define APCLI_MT2_EAPOLLogoff			2
#define APCLI_MT2_EAPOLKey				3
#define APCLI_MT2_EAPOLASFAlert			4
#define APCLI_MAX_WPA_PSK_MSG			5

#define	APCLI_WPA_PSK_FUNC_SIZE			(APCLI_MAX_WPA_PSK_STATE * APCLI_MAX_WPA_PSK_MSG)
#endif // end - 0 //

#endif	// APCLI_SUPPORT //

// =============================================================================

// value domain of 802.11 header FC.Tyte, which is b3..b2 of the 1st-byte of MAC header
#define BTYPE_MGMT                  0
#define BTYPE_CNTL                  1
#define BTYPE_DATA                  2

/* value domain of 802.11 header FC.Tyte, which is b3..b2 of the 1st-byte of MAC header */
#define FC_TYPE_MGMT	0
#define FC_TYPE_CNTL	1
#define FC_TYPE_DATA	2
#define FC_TYPE_RSVED	3

// value domain of 802.11 MGMT frame's FC.subtype, which is b7..4 of the 1st-byte of MAC header
#define SUBTYPE_ASSOC_REQ           0
#define SUBTYPE_ASSOC_RSP           1
#define SUBTYPE_REASSOC_REQ         2
#define SUBTYPE_REASSOC_RSP         3
#define SUBTYPE_PROBE_REQ           4
#define SUBTYPE_PROBE_RSP           5
#define SUBTYPE_BEACON              8
#define SUBTYPE_ATIM                9
#define SUBTYPE_DISASSOC            10
#define SUBTYPE_AUTH                11
#define SUBTYPE_DEAUTH              12
#define SUBTYPE_ACTION              13
#define SUBTYPE_ACTION_NO_ACK              14

// value domain of 802.11 CNTL frame's FC.subtype, which is b7..4 of the 1st-byte of MAC header
#define SUBTYPE_WRAPPER       	7
#define SUBTYPE_BLOCK_ACK_REQ       8
#define SUBTYPE_BLOCK_ACK           9
#define SUBTYPE_PS_POLL             10
#define SUBTYPE_RTS                 11
#define SUBTYPE_CTS                 12
#define SUBTYPE_ACK                 13
#define SUBTYPE_CFEND               14
#define SUBTYPE_CFEND_CFACK         15

// value domain of 802.11 DATA frame's FC.subtype, which is b7..4 of the 1st-byte of MAC header
#define SUBTYPE_DATA                0
#define SUBTYPE_DATA_CFACK          1
#define SUBTYPE_DATA_CFPOLL         2
#define SUBTYPE_DATA_CFACK_CFPOLL   3
#define SUBTYPE_NULL_FUNC           4
#define SUBTYPE_CFACK               5
#define SUBTYPE_CFPOLL              6
#define SUBTYPE_CFACK_CFPOLL        7
#define SUBTYPE_QDATA               8
#define SUBTYPE_QDATA_CFACK         9
#define SUBTYPE_QDATA_CFPOLL        10
#define SUBTYPE_QDATA_CFACK_CFPOLL  11
#define SUBTYPE_QOS_NULL            12
#define SUBTYPE_QOS_CFACK           13
#define SUBTYPE_QOS_CFPOLL          14
#define SUBTYPE_QOS_CFACK_CFPOLL    15

// ACK policy of QOS Control field bit 6:5
#define NORMAL_ACK                  0x00  // b6:5 = 00
#define NO_ACK                      0x20  // b6:5 = 01
#define NO_EXPLICIT_ACK             0x40  // b6:5 = 10
#define BLOCK_ACK                   0x60  // b6:5 = 11

//
// rtmp_data.c use these definition
//
#define LENGTH_802_11               24
#define LENGTH_802_11_AND_H         30
#define LENGTH_802_11_CRC_H         34
#define LENGTH_802_11_CRC           28
#define LENGTH_802_11_WITH_ADDR4    30
#define LENGTH_802_3                14
#define LENGTH_802_3_TYPE           2
#define LENGTH_802_1_H              8
#define LENGTH_EAPOL_H              4
#define LENGTH_WMMQOS_H				2
#define LENGTH_CRC                  4
#define MAX_SEQ_NUMBER              0x0fff
#define LENGTH_802_3_NO_TYPE		12
#define LENGTH_802_1Q				4 /* VLAN related */

// STA_CSR4.field.TxResult
#define TX_RESULT_SUCCESS           0
#define TX_RESULT_ZERO_LENGTH       1
#define TX_RESULT_UNDER_RUN         2
#define TX_RESULT_OHY_ERROR         4
#define TX_RESULT_RETRY_FAIL        6

// All PHY rate summary in TXD
// Preamble MODE in TxD
#define MODE_CCK	0
#define MODE_OFDM   1
#define MODE_HTMIX	2
#define MODE_HTGREENFIELD	3
// MCS for CCK.  BW.SGI.STBC are reserved
#define MCS_LONGP_RATE_1                      0	 // long preamble CCK 1Mbps
#define MCS_LONGP_RATE_2                      1	// long preamble CCK 1Mbps
#define MCS_LONGP_RATE_5_5                    2
#define MCS_LONGP_RATE_11                     3
#define MCS_SHORTP_RATE_1                      4	 // long preamble CCK 1Mbps. short is forbidden in 1Mbps
#define MCS_SHORTP_RATE_2                      5	// short preamble CCK 2Mbps
#define MCS_SHORTP_RATE_5_5                    6
#define MCS_SHORTP_RATE_11                     7
// To send duplicate legacy OFDM. set BW=BW_40.  SGI.STBC are reserved
#define MCS_RATE_6                      0   // legacy OFDM
#define MCS_RATE_9                      1   // OFDM
#define MCS_RATE_12                     2   // OFDM
#define MCS_RATE_18                     3   // OFDM
#define MCS_RATE_24                     4  // OFDM
#define MCS_RATE_36                     5   // OFDM
#define MCS_RATE_48                     6  // OFDM
#define MCS_RATE_54                     7 // OFDM	
// HT
#define MCS_0       0           /* 1S */
#define MCS_1		1
#define MCS_2		2
#define MCS_3		3
#define MCS_4		4
#define MCS_5		5
#define MCS_6		6
#define MCS_7		7
#define MCS_8       8           /* 2S */
#define MCS_9		9
#define MCS_10		10
#define MCS_11		11
#define MCS_12		12
#define MCS_13		13
#define MCS_14		14
#define MCS_15		15
#define MCS_16                  16          /* 3S */
#define MCS_17                  17
#define MCS_18                  18
#define MCS_19                  19
#define MCS_20                  20
#define MCS_21                  21
#define MCS_22                  22
#define MCS_23                  23
#define MCS_24                  24          /* 4S */
#define MCS_25                  25
#define MCS_26                  26
#define MCS_27                  27
#define MCS_28                  28
#define MCS_29                  29
#define MCS_30                  30
#define MCS_31                  31
#define MCS_32		32
#define MCS_AUTO		33

// OID_HTPHYMODE
// Extension channel offset
#define EXTCHA_NONE	0	 
#define EXTCHA_ABOVE	0x1
#define EXTCHA_BELOW	0x3
// MODE
#define HTMODE_MM	0	 
#define HTMODE_GF	1

/* Fixed Tx MODE - HT, CCK or OFDM */
#define FIXED_TXMODE_HT	0
#define FIXED_TXMODE_CCK	1
#define FIXED_TXMODE_OFDM 	2
#define FIXED_TXMODE_VHT	3

// BW
#define BW_20	0
#define BW_40	1
#define BW_BOTH		2
#ifndef WIFI_BUILD_RAM
#define BW_10		3	// 802.11j has 10MHz. This definition is for internal usage. doesn't fill in the IE or other field.
#endif

// SHORTGI
#define GI_400	1	// only support in HT mode
#define GI_800	0	
#define GI_BOTH		2
// STBC
#define STBC_NONE	0
#define STBC_USE	1	// limited use in rt2860b phy
#define RXSTBC_ONE	1	// rx support of one spatial stream
#define RXSTBC_TWO	2	// rx support of 1 and 2 spatial stream
#define RXSTBC_THR	3	// rx support of 1~3 spatial stream
// MCS FEEDBACK
#define MCSFBK_NONE	0  // not support mcs feedback /
#define MCSFBK_RSV	1	// reserved
#define MCSFBK_UNSOLICIT	2	// only support unsolict mcs feedback
#define MCSFBK_MRQ	3	// response to both MRQ and unsolict mcs feedback

// MIMO power safe 
#define	MMPS_STATIC	0
#define	MMPS_DYNAMIC		1
#define   MMPS_RSV		2
#define MMPS_ENABLE		3


// A-MSDU size
#define	AMSDU_0	0
#define	AMSDU_1		1

// MCS use 7 bits 
#define TXRATEMIMO		0x80
#define TXRATEMCS		0x7F
#define TXRATEOFDM		0x7F
#define RATE_1                      0
#define RATE_2                      1
#define RATE_5_5                    2
#define RATE_11                     3
#define RATE_6                      4   // OFDM
#define RATE_9                      5   // OFDM
#define RATE_12                     6   // OFDM
#define RATE_18                     7   // OFDM
#define RATE_24                     8   // OFDM
#define RATE_36                     9   // OFDM
#define RATE_48                     10  // OFDM
#define RATE_54                     11  // OFDM
#define RATE_FIRST_OFDM_RATE        RATE_6
#define RATE_LAST_OFDM_RATE        	RATE_54
#define RATE_6_5                    12  // HT mix
#define RATE_13                     13  // HT mix
#define RATE_19_5                   14  // HT mix
#define RATE_26                     15  // HT mix
#define RATE_39                     16  // HT mix
#define RATE_52                     17  // HT mix
#define RATE_58_5                   18  // HT mix
#define RATE_65                     19  // HT mix
#define RATE_78                     20  // HT mix
#define RATE_104                    21  // HT mix
#define RATE_117                    22  // HT mix
#define RATE_130                    23  // HT mix
//#define RATE_AUTO_SWITCH            255 // for StaCfg.FixedTxRate only
#define HTRATE_0                      12
#define RATE_FIRST_MM_RATE        HTRATE_0
#define RATE_FIRST_HT_RATE        HTRATE_0
#define RATE_LAST_HT_RATE        HTRATE_0

// pTxWI->txop
#define IFS_HTTXOP                 0	// The txop will be handles by ASIC.
#define IFS_PIFS                    1
#define IFS_SIFS                    2
#define IFS_BACKOFF                 3

// pTxD->RetryMode 
#define LONG_RETRY                  1
#define SHORT_RETRY                 0

// Country Region definition
#define REGION_MINIMUM_BG_BAND            0
#define REGION_0_BG_BAND                  0       // 1-11
#define REGION_1_BG_BAND                  1       // 1-13
#define REGION_2_BG_BAND                  2       // 10-11
#define REGION_3_BG_BAND                  3       // 10-13
#define REGION_4_BG_BAND                  4       // 14
#define REGION_5_BG_BAND                  5       // 1-14
#define REGION_6_BG_BAND                  6       // 3-9
#define REGION_7_BG_BAND                  7       // 5-13
#define REGION_MAXIMUM_BG_BAND            7

#define REGION_MINIMUM_A_BAND             0
#define REGION_0_A_BAND                   0       // 36, 40, 44, 48, 52, 56, 60, 64, 149, 153, 157, 161, 165
#define REGION_1_A_BAND                   1       // 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140
#define REGION_2_A_BAND                   2       // 36, 40, 44, 48, 52, 56, 60, 64
#define REGION_3_A_BAND                   3       // 52, 56, 60, 64, 149, 153, 157, 161
#define REGION_4_A_BAND                   4       // 149, 153, 157, 161, 165
#define REGION_5_A_BAND                   5       // 149, 153, 157, 161
#define REGION_6_A_BAND                   6       // 36, 40, 44, 48
#define REGION_7_A_BAND                   7       // 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 149, 153, 157, 161, 165
#define REGION_8_A_BAND                   8       // 52, 56, 60, 64
#define REGION_9_A_BAND                   9       // 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 132, 136, 140, 149, 153, 157, 161, 165
#define REGION_10_A_BAND                  10	  // 36, 40, 44, 48, 149, 153, 157, 161, 165	
#define REGION_11_A_BAND                  11	  // 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116, 120, 149, 153, 157, 161
#define REGION_MAXIMUM_A_BAND             11

// pTxD->CipherAlg
#if 0
#define CIPHER_NONE                 0
#define CIPHER_WEP64                1
#define CIPHER_WEP128               2
#define CIPHER_TKIP                 3
#define CIPHER_AES                  4
#define CIPHER_CKIP64               5
#define CIPHER_CKIP128              6
#define CIPHER_CKIP152              7
#define CIPHER_WEP152              8

#define CIPHER_TKIP_NO_MIC          9       // MIC appended by driver: not a valid value in hardware key table
#endif
// value domain of pAd->RfIcType
#define RFIC_2820                   1       // 2.4G 2T3R
#define RFIC_2850                   2       // 2.4G/5G 2T3R
#define RFIC_2720                   3       // 2.4G 1T2R
#define RFIC_2750                   4       // 2.4G/5G 1T2R
#define RFIC_3020                   5       // 2.4G 1T1R
#define RFIC_3022                   8       // 2.4G 2T2R

// LED Status.
#define LED_LINK_DOWN               0
#define LED_LINK_UP                 1
#define LED_RADIO_OFF               2
#define LED_RADIO_ON                3
#define LED_HALT                    4

#define LED_FORCE_OFF				20

// value domain of pAd->LedCntl.LedMode and E2PROM
#define LED_MODE_DEFAULT            0
#define LED_MODE_TWO_LED			1
#define LED_MODE_SIGNAL_STREGTH		8  // EEPROM define =8

// RC4 init value, used fro WEP & TKIP
#define PPPINITFCS32                0xffffffff   /* Initial FCS value */

// value domain of pAd->StaCfg.PortSecured. 802.1X controlled port definition
#define WPA_802_1X_PORT_SECURED     1
#define WPA_802_1X_PORT_NOT_SECURED 2

#define PAIRWISE_KEY                1
#define GROUP_KEY                   2

#define PCI_CFG_ADDR_PORT           0xcf8
#define PCI_CFG_DATA_PORT           0xcfc

// vendor ID
#define RICOH                       0x1180
#define O2MICRO                     0x1217
#define TI                          0x104c
#define RALINK                      0x1814
#define TOSHIBA                     0x1179
#define ENE                         0x1524
#define UNKNOWN                     0xffff

#define CARD_BRIDGE_CLASS           0x0607          // CardBus bridge class & subclass

#define MAX_PCI_DEVICE              32      // support up to 32 devices per bus
#define MAX_PCI_BUS                 32      // support 10 buses
#define MAX_FUNC_NUM                4
//definition of DRS
#define MAX_STEP_OF_TX_RATE_SWITCH	32


// pre-allocated free NDIS PACKET/BUFFER poll for internal usage
#define MAX_NUM_OF_FREE_NDIS_PACKET 128

#define DEFAULT_VCO_RECALIBRATION_THRESHOLD	1

/*definition of DRS */
#define MAX_TX_RATE_INDEX			33		/* Maximum Tx Rate Table Index value */



//Block ACK
#define MAX_TX_REORDERBUF   64
#define MAX_RX_REORDERBUF   64
#define DEFAULT_TX_TIMEOUT   30
#define DEFAULT_RX_TIMEOUT   30
#ifdef CONFIG_AP_SUPPORT
#define MAX_BARECI_SESSION   8 
#endif // CONFIG_AP_SUPPORT //

// definition of Recipient or Originator
#define I_RECIPIENT                  TRUE
#define I_ORIGINATOR                   FALSE

#define DEFAULT_BBP_TX_POWER        0
#define DEFAULT_RF_TX_POWER         5

#define MAX_INI_BUFFER_SIZE			4096
#define MAX_PARAM_BUFFER_SIZE		512 //IoT//(2048) // enough for ACL (18*64)
											//18 : the length of Mac address acceptable format "01:02:03:04:05:06;")
											//64 : MAX_NUM_OF_ACL_LIST
// definition of pAd->OpMode
#define OPMODE_STA                  0
#define OPMODE_AP                   1
//#define OPMODE_L3_BRG               2       // as AP and STA at the same time

#ifdef BIG_ENDIAN
#define DIR_READ                    0
#define DIR_WRITE                   1
#define TYPE_TXD                    0
#define TYPE_RXD                    1
#define TYPE_TXINFO					0
#define TYPE_RXINFO					1
#define TYPE_TXWI					0
#define TYPE_RXWI					1
#endif

// ========================= AP rtmp_def.h ===========================
// value domain for pAd->EventTab.Log[].Event
#define EVENT_RESET_ACCESS_POINT    0 // Log = "hh:mm:ss   Restart Access Point"
#define EVENT_ASSOCIATED            1 // Log = "hh:mm:ss   STA 00:01:02:03:04:05 associated"
#define EVENT_DISASSOCIATED         2 // Log = "hh:mm:ss   STA 00:01:02:03:04:05 left this BSS"
#define EVENT_AGED_OUT              3 // Log = "hh:mm:ss   STA 00:01:02:03:04:05 was aged-out and removed from this BSS"
#define EVENT_COUNTER_M             4
#define EVENT_INVALID_PSK           5
#define EVENT_MAX_EVENT_TYPE        6
// ==== end of AP rtmp_def.h ============

// definition RSSI Number
#define RSSI_0					0
#define RSSI_1					1
#define RSSI_2					2

// definition of radar detection
#define RD_NORMAL_MODE				0	// Not found radar signal
#define RD_SWITCHING_MODE			1	// Found radar signal, and doing channel switch
#define RD_SILENCE_MODE				2	// After channel switch, need to be silence a while to ensure radar not found

//Driver defined cid for mapping status and command.
#define  SLEEPCID	0x11
#define  WAKECID	0x22
#define  QUERYPOWERCID	0x33
#define  OWNERMCU	0x1
#define  OWNERCPU	0x0

// MBSSID definition
#define ENTRY_NOT_FOUND             0xFF


/* After Linux 2.6.9, 
 * VLAN module use Private (from user) interface flags (netdevice->priv_flags). 
 * #define IFF_802_1Q_VLAN 0x1         --    802.1Q VLAN device.  in if.h     
 * ref to ip_sabotage_out() [ out->priv_flags & IFF_802_1Q_VLAN ] in br_netfilter.c
 * 
 * For this reason, we MUST use EVEN value in priv_flags
 */
#define INT_MAIN                    0x0100
#define INT_MBSSID                  0x0200
#define INT_WDS                     0x0300
#define INT_APCLI                   0x0400


#define ENTRY_CAT_NONE			0x000
#define ENTRY_CAT_STA			0x001
#define ENTRY_CAT_AP			0x002
#define ENTRY_CAT_WDS			0x004
#define ENTRY_CAT_MESH			0x008
#define ENTRY_CAT_MCAST		0x400
#define ENTRY_CAT_WDEV			0x800
#define ENTRY_CAT_MASK			0xfff

typedef struct _WIFI_NODE_TYPE {
	UINT32 rsv:26;
	UINT32 type: 3;
	UINT32 cat:3;
}WIFI_NODE_TYPE;

#define ENTRY_AP			ENTRY_CAT_AP
#define ENTRY_GO			(ENTRY_CAT_AP | 0x01000)


#define ENTRY_INFRA			ENTRY_CAT_STA
#define ENTRY_GC			(ENTRY_CAT_STA | 0x01000)
#define ENTRY_ADHOC		(ENTRY_CAT_STA | 0x02000)
#define ENTRY_APCLI			(ENTRY_CAT_STA | 0x04000)
#define ENTRY_DLS			(ENTRY_CAT_STA | 0x08000)
#define ENTRY_TDLS			(ENTRY_CAT_STA | 0x10000)
#define ENTRY_CLIENT		(ENTRY_CAT_STA | 0x20000)
#define ENTRY_REPEATER      (ENTRY_CAT_STA | 0x40000)

#define ENTRY_WDS			ENTRY_CAT_WDS
#define ENTRY_MESH			ENTRY_CAT_MESH

#define ENTRY_NONE			ENTRY_CAT_NONE


#ifdef P2P_SUPPORT
#define P2P_ENTRY_NONE		0
#define P2P_GO_ENTRY		1
#define P2P_CLI_ENTRY		2
#endif /* P2P_SUPPORT */

#define IS_ENTRY_NONE(_x)		((_x)->EntryType == ENTRY_CAT_NONE)
#define IS_ENTRY_CLIENT(_x)		((_x)->EntryType == ENTRY_CLIENT)
#define IS_ENTRY_WDS(_x)		((_x)->EntryType & ENTRY_CAT_WDS)
#define IS_ENTRY_APCLI(_x)		((_x)->EntryType == ENTRY_APCLI)
#define IS_ENTRY_REPEATER(_x)   ((_x)->EntryType == ENTRY_REPEATER)
#define IS_ENTRY_ADHOC(_x)		((_x)->EntryType & ENTRY_ADHOC)
#define IS_ENTRY_AP(_x)			((_x)->EntryType & ENTRY_CAT_AP)
#define IS_ENTRY_MESH(_x)		((_x)->EntryType & ENTRY_CAT_MESH)
#define IS_ENTRY_DLS(_x)		((_x)->EntryType == ENTRY_DLS)
#define IS_ENTRY_TDLS(_x)		((_x)->EntryType == ENTRY_TDLS)
#ifdef CLIENT_WDS
#define IS_ENTRY_CLIWDS(_x)		CLIENT_STATUS_TEST_FLAG((_x), fCLIENT_STATUS_CLI_WDS)
#endif /* CLIENT_WDS */
#ifdef P2P_SUPPORT
#define IS_ENTRY_P2PCLI(_x)		CLIENT_STATUS_TEST_FLAG((_x), fCLIENT_STATUS_P2P_CLI)
#define IS_P2P_ENTRY_NONE(_x)	((_x)->P2PEntryType == P2P_ENTRY_NONE)
#define IS_P2P_GO_ENTRY(_x)		((_x)->P2PEntryType == P2P_GO_ENTRY)
#define IS_P2P_CLI_ENTRY(_x)	((_x)->P2PEntryType == P2P_CLI_ENTRY)
#endif /* P2P_SUPPORT */

#define IS_VALID_ENTRY(_x)		(((_x) != NULL) && ((_x)->EntryType != ENTRY_NONE))
#define VALID_TR_WCID(_pAd,_wcid)		((_wcid) < MAX_LEN_OF_TR_TABLE(_pAd))

#define SET_ENTRY_NONE(_x)		((_x)->EntryType = ENTRY_NONE)
#define SET_ENTRY_CLIENT(_x)	((_x)->EntryType = ENTRY_CLIENT)
#define SET_ENTRY_WDS(_x)		((_x)->EntryType = ENTRY_WDS)
#define SET_ENTRY_APCLI(_x)		((_x)->EntryType = ENTRY_APCLI)
#define SET_ENTRY_AP(_x)		((_x)->EntryType = ENTRY_AP)
#define SET_ENTRY_ADHOC(_x)                ((_x)->EntryType = ENTRY_ADHOC)
#define SET_ENTRY_MESH(_x)		((_x)->EntryType = ENTRY_MESH)
#define SET_ENTRY_DLS(_x)		((_x)->EntryType = ENTRY_DLS)
#define SET_ENTRY_TDLS(_x)		((_x)->EntryType = ENTRY_TDLS)
#define SET_ENTRY_REPEATER(_x)  ((_x)->EntryType = ENTRY_REPEATER)
#ifdef CLIENT_WDS
#define SET_ENTRY_CLIWDS(_x)	CLIENT_STATUS_SET_FLAG((_x), fCLIENT_STATUS_CLI_WDS)
#endif /* CLIENT_WDS */
#ifdef P2P_SUPPORT
#define SET_ENTRY_P2PCLI(_x)	CLIENT_STATUS_SET_FLAG((_x), fCLIENT_STATUS_P2P_CLI)
#define SET_P2P_GO_ENTRY(_x)	((_x)->P2PEntryType = P2P_GO_ENTRY)
#define SET_P2P_CLI_ENTRY(_x)	((_x)->P2PEntryType = P2P_CLI_ENTRY)
#define SET_P2P_ENTRY_NONE(_x)	((_x)->P2PEntryType = P2P_ENTRY_NONE)
#endif /* P2P_SUPPORT */
//CFG_TODO
#define SET_PKT_OPMODE_AP(_x)		((_x)->OpMode = OPMODE_AP)
#define SET_PKT_OPMODE_STA(_x)		((_x)->OpMode = OPMODE_STA)
#define IS_PKT_OPMODE_AP(_x)		((_x)->OpMode == OPMODE_AP)
#define IS_PKT_OPMODE_STA(_x)		((_x)->OpMode == OPMODE_STA)


#define IS_OPMODE_AP(_x)		((_x)->OpMode == OPMODE_AP)
#define IS_OPMODE_STA(_x)		((_x)->OpMode == OPMODE_STA)




// Use bitmap to allow coexist of ATE_TXFRAME and ATE_RXFRAME(i.e.,to support LoopBack mode)
#ifdef RALINK_ATE
#define	ATE_START                   0x00   // Start ATE
#define	ATE_STOP                    0x80   // Stop ATE
#define	ATE_TXCONT                  0x05   // Continuous Transmit
#define	ATE_TXCARR                  0x09   // Transmit Carrier
#define	ATE_TXCARRSUPP              0x11   // Transmit Carrier Suppression
#define	ATE_TXFRAME                 0x01   // Transmit Frames
#define	ATE_RXFRAME                 0x02   // Receive Frames
#ifdef RALINK_28xx_QA
#define ATE_TXSTOP                  0xfe   // Stop Transmition
#define ATE_RXSTOP					0xfd   // Stop receiving Frames
#define	BBP22_TXFRAME     			0x00   // Transmit Frames
#define	BBP22_TXCONT_OR_CARRSUPP    0x80   // Continuous Transmit or Carrier Suppression
#define	BBP22_TXCARR                0xc1   // Transmit Carrier
#define	BBP24_TXCONT                0x00   // Continuous Transmit
#define	BBP24_CARRSUPP              0x01   // Carrier Suppression
#endif // RALINK_28xx_QA //
#endif // RALINK_ATE //

// WEP Key TYPE
#define WEP_HEXADECIMAL_TYPE    0
#define WEP_ASCII_TYPE          1



// WIRELESS EVENTS definition
/* Max number of char in custom event, refer to wireless_tools.28/wireless.20.h */
#define IW_CUSTOM_MAX_LEN				  			255	/* In bytes */	

// For system event - start 
#define	IW_SYS_EVENT_FLAG_START                     0x0200
#define	IW_ASSOC_EVENT_FLAG                         0x0200
#define	IW_DISASSOC_EVENT_FLAG                      0x0201
#define	IW_DEAUTH_EVENT_FLAG                      	0x0202
#define	IW_AGEOUT_EVENT_FLAG                      	0x0203
#define	IW_COUNTER_MEASURES_EVENT_FLAG              0x0204
#define	IW_REPLAY_COUNTER_DIFF_EVENT_FLAG           0x0205
#define	IW_RSNIE_DIFF_EVENT_FLAG           			0x0206
#define	IW_MIC_DIFF_EVENT_FLAG           			0x0207
#define IW_ICV_ERROR_EVENT_FLAG						0x0208
#define IW_MIC_ERROR_EVENT_FLAG						0x0209
#define IW_GROUP_HS_TIMEOUT_EVENT_FLAG				0x020A
#define	IW_PAIRWISE_HS_TIMEOUT_EVENT_FLAG			0x020B
#define IW_RSNIE_SANITY_FAIL_EVENT_FLAG				0x020C
#define IW_SET_KEY_DONE_WPA1_EVENT_FLAG				0x020D
#define IW_SET_KEY_DONE_WPA2_EVENT_FLAG				0x020E
#define IW_STA_LINKUP_EVENT_FLAG					0x020F
#define IW_STA_LINKDOWN_EVENT_FLAG					0x0210
#define IW_SCAN_COMPLETED_EVENT_FLAG				0x0211
// if add new system event flag, please upadte the IW_SYS_EVENT_FLAG_END
#define	IW_SYS_EVENT_FLAG_END                       0x0211
#define	IW_SYS_EVENT_TYPE_NUM						(IW_SYS_EVENT_FLAG_END - IW_SYS_EVENT_FLAG_START + 1)
// For system event - end 

// For spoof attack event - start
#define	IW_SPOOF_EVENT_FLAG_START                   0x0300
#define IW_CONFLICT_SSID_EVENT_FLAG					0x0300
#define IW_SPOOF_ASSOC_RESP_EVENT_FLAG				0x0301
#define IW_SPOOF_REASSOC_RESP_EVENT_FLAG			0x0302
#define IW_SPOOF_PROBE_RESP_EVENT_FLAG				0x0303
#define IW_SPOOF_BEACON_EVENT_FLAG					0x0304
#define IW_SPOOF_DISASSOC_EVENT_FLAG				0x0305
#define IW_SPOOF_AUTH_EVENT_FLAG					0x0306
#define IW_SPOOF_DEAUTH_EVENT_FLAG					0x0307
#define IW_SPOOF_UNKNOWN_MGMT_EVENT_FLAG			0x0308
#define IW_REPLAY_ATTACK_EVENT_FLAG					0x0309
// if add new spoof attack event flag, please upadte the IW_SPOOF_EVENT_FLAG_END
#define	IW_SPOOF_EVENT_FLAG_END                     0x0309
#define	IW_SPOOF_EVENT_TYPE_NUM						(IW_SPOOF_EVENT_FLAG_END - IW_SPOOF_EVENT_FLAG_START + 1)
// For spoof attack event - end 

// For flooding attack event - start
#define	IW_FLOOD_EVENT_FLAG_START                   0x0400
#define IW_FLOOD_AUTH_EVENT_FLAG					0x0400
#define IW_FLOOD_ASSOC_REQ_EVENT_FLAG				0x0401
#define IW_FLOOD_REASSOC_REQ_EVENT_FLAG				0x0402
#define IW_FLOOD_PROBE_REQ_EVENT_FLAG				0x0403
#define IW_FLOOD_DISASSOC_EVENT_FLAG				0x0404
#define IW_FLOOD_DEAUTH_EVENT_FLAG					0x0405
#define IW_FLOOD_EAP_REQ_EVENT_FLAG					0x0406
// if add new flooding attack event flag, please upadte the IW_FLOOD_EVENT_FLAG_END
#define	IW_FLOOD_EVENT_FLAG_END                   	0x0406
#define	IW_FLOOD_EVENT_TYPE_NUM						(IW_FLOOD_EVENT_FLAG_END - IW_FLOOD_EVENT_FLAG_START + 1)
// For flooding attack - end 

#ifdef WSC_INCLUDED
// For WSC wireless event - start
#define	IW_WSC_EVENT_FLAG_START                   	0x0500
#define IW_WSC_PBC_SESSION_OVERLAP					0x0500
#define IW_WSC_REGISTRAR_SUPPORT_PBC				0x0501
#define IW_WSC_REGISTRAR_SUPPORT_PIN				0x0502
#define	IW_WSC_STATUS_SUCCESS              			0x0503
#define	IW_WSC_STATUS_FAIL             				0x0504
#define	IW_WSC_2MINS_TIMEOUT           				0x0505
#define	IW_WSC_SEND_EAPOL_START    					0x0506
#define	IW_WSC_SEND_WSC_START      					0x0507
#define	IW_WSC_SEND_M1           					0x0508
#define	IW_WSC_SEND_M2           					0x0509
#define	IW_WSC_SEND_M3           					0x050a
#define	IW_WSC_SEND_M4           					0x050b
#define	IW_WSC_SEND_M5           					0x050c
#define	IW_WSC_SEND_M6           					0x050d
#define	IW_WSC_SEND_M7           					0x050e
#define	IW_WSC_SEND_M8           					0x050f
#define	IW_WSC_SEND_DONE           					0x0510
#define	IW_WSC_SEND_ACK           					0x0511
#define	IW_WSC_SEND_NACK           					0x0512
#define	IW_WSC_RECEIVE_WSC_START   					0x0513
#define	IW_WSC_RECEIVE_M1          					0x0514
#define	IW_WSC_RECEIVE_M2          					0x0515
#define	IW_WSC_RECEIVE_M3          					0x0516
#define	IW_WSC_RECEIVE_M4          					0x0517
#define	IW_WSC_RECEIVE_M5          					0x0518
#define	IW_WSC_RECEIVE_M6          					0x0519
#define	IW_WSC_RECEIVE_M7          					0x051a
#define	IW_WSC_RECEIVE_M8          					0x051b
#define	IW_WSC_RECEIVE_DONE        					0x051c
#define	IW_WSC_RECEIVE_ACK         					0x051d
#define	IW_WSC_RECEIVE_NACK        					0x051e
#define	IW_WSC_MANY_CANDIDATE 						0x051f
#define	IW_WSC_REGISTRAR_PIN_FAIL           		0x0520
#define	IW_WSC_EVENT_FLAG_END                   	0x0520
#define	IW_WSC_EVENT_TYPE_NUM						(IW_WSC_EVENT_FLAG_END - IW_WSC_EVENT_FLAG_START + 1)
// For WSC wireless event - end
#endif // WSC_INCLUDED //

// End - WIRELESS EVENTS definition

#ifdef CONFIG_STA_SUPPORT
// definition for DLS, kathy
#define	MAX_NUM_OF_INIT_DLS_ENTRY   1
#define	MAX_NUM_OF_DLS_ENTRY        4

// DLS Action frame definition
#define ACTION_DLS_REQUEST			0
#define ACTION_DLS_RESPONSE			1
#define ACTION_DLS_TEARDOWN			2

//Block ACK , rt2860, kathy
#define MAX_TX_REORDERBUF		64
#define MAX_RX_REORDERBUF		64
#define DEFAULT_TX_TIMEOUT		30
#define DEFAULT_RX_TIMEOUT		30
#define MAX_BARECI_SESSION		8

#ifndef IW_ESSID_MAX_SIZE
/* Maximum size of the ESSID and pAd->nickname strings */
#define IW_ESSID_MAX_SIZE   		32
#endif
#endif // CONFIG_STA_SUPPORT //

#ifdef MCAST_RATE_SPECIFIC
#define MCAST_DISABLE	0
#define MCAST_CCK		1
#define MCAST_OFDM		2
#endif // MCAST_RATE_SPECIFIC //

// For AsicRadioOff/AsicRadioOn function
#define DOT11POWERSAVE		0
#define GUIRADIO_OFF		1
#define RTMP_HALT		    2
#define GUI_IDLE_POWER_SAVE		3
// --
#define WAKE_MCU_CMD				0x31
#define SLEEP_MCU_CMD					0x30
#define RFOFF_MCU_CMD				0x35

//
// The upperbound/lowerbound of the frequency offset
//
#define UPPERBOUND_OF_FREQUENCY_OFFSET		127
#define LOWERBOUND_OF_FREQUENCY_OFFSET	-127

//
// The trigger point of decreasng/increasing the frequency offset
//
#define DECREASE_FREQUENCY_OFFSET			3
#define INCREASE_FREQUENCY_OFFSET			-3

//
// Invalid frequency offset
//
#define INVALID_FREQUENCY_OFFSET			-128

//
// Frequency calibration period
//
#define FREQUENCY_CALIBRATION_PERIOD		10

//
// The frequency calibration control
//
typedef struct _FREQUENCY_CALIBRATION_CONTROL
{
	BOOLEAN bEnableFrequencyCalibration; // Enable the frequency calibration algorithm

	BOOLEAN bSkipFirstFrequencyCalibration; // Avoid calibrating frequency at the time the STA is just link-up

	CHAR AdaptiveFreqOffset; // Adaptive frequency offset
	CHAR LatestFreqOffsetOverBeacon; // Latest frequency offset from the beacon
} FREQUENCY_CALIBRATION_CONTROL, *PFREQUENCY_CALIBRATION_CONTROL;

/* definition for WpaSupport flag */
#define WPA_SUPPLICANT_DISABLE				0x00
#define WPA_SUPPLICANT_ENABLE				0x01
#define	WPA_SUPPLICANT_ENABLE_WITH_WEB_UI	0x02
#define	WPA_SUPPLICANT_ENABLE_WPS			0x80


enum ANT_DIVERSITY_TYPE {
    ANT_DIVERSITY_DISABLE = 0,
    ANT_DIVERSITY_ENABLE = 1,
    ANT_FIX_ANT1 = 2,		// Main
    ANT_FIX_ANT2 = 3	,		// Aux
    ANT_SW_CONTROLL = 4
};

#define MAX_EEPROM_BIN_FILE_SIZE				1024
//#define BOOLEAN UCHAR
#define NTSTATUS UINT32
#define STATUS_SUCCESS	0
#if 0
typedef enum _HWBSSID_TABLE
{
    HW_BSSID_0 = 0,
    HW_BSSID_1,
    HW_BSSID_2,
    HW_BSSID_3,
    HW_BSSID_MAX
} HWBSSID_TABLE;
#endif
#define HW_BEACON_MAX_NUM			1  // No MBSS support:q
#define MAX_MESH_NUM				0

#define NUM_OF_UP 9 /*number of user priority: 2set WMM+ non QoS*/

#define WDEV_NUM_MAX(_pAd)		(HW_BEACON_MAX_NUM + MAX_APCLI_NUM(_pAd))

#define MAC_IO_WRITE32(_A, _R, _V)  (HAL_REG_32(_R) = _V)
#define MAC_IO_READ32(_A, _R, _pV)  (*_pV = HAL_REG_32(_R))

#define RTMP_IO_WRITE32(_A, _R, _V) MAC_IO_WRITE32(_A, _R, _V)
#define RTMP_IO_READ32(_A, _R, _pV) MAC_IO_READ32(_A, _R, _pV)


#define HW_IO_READ32(_A, _R, _pV)			RTMP_IO_READ32(_A, _R, _pV)
#define HW_IO_WRITE32(_A, _R, _V)			RTMP_IO_WRITE32(_A, _R, _V)


#define HIF_IO_READ32(_A, _R, _pV)			RTMP_IO_READ32(_A, _R, _pV)
#define HIF_IO_WRITE32(_A, _R, _V)			RTMP_IO_WRITE32(_A, _R, _V)

#define PHY_IO_READ32(_A, _R, _pV)			RTMP_IO_READ32(_A, _R, _pV)
#define PHY_IO_WRITE32(_A, _R, _V)			RTMP_IO_WRITE32(_A, _R, _V)


#define MCU_IO_READ32(_A, _R, _pV)			RTMP_IO_READ32(_A, _R, _pV)
#define MCU_IO_WRITE32(_A, _R, _V)			RTMP_IO_WRITE32(_A, _R, _V)

#define isxdigit(_char)  \
	(('0' <= (_char) && (_char) <= '9') ||\
	 ('a' <= (_char) && (_char) <= 'f') || \
	 ('A' <= (_char) && (_char) <= 'F') \
	)

#define isspace(ch) (ch == ' ')
#define isdigit(ch) \
	((ch >= '0') && (ch <= '9'))

#define islower(ch) ((ch >= 'a' && (ch) <= 'z'))
#define tolower(ch) ((ch >= 'A' && (ch) <= 'Z') ? (ch) - ('A' - 'a') : (ch))

#define toupper(ch) ((ch >= 'a' && (ch) <= 'z') ? (ch) + ('A' - 'a') : (ch))

#if 0
#define EINVAL					(-3000)
#define EOPNOTSUPP      		(-3001)
#define EFAULT					(-3002)
#define ENETDOWN				(-3003)
#define E2BIG					(-3004)
#define ENOMEM					(-3005)
#define EAGAIN					(-3006)
#define ENOTCONN				(-3007)
#endif

/* Station role */
#define STA_TYPE_STA		BIT(0)
#define STA_TYPE_AP		BIT(1)
#define STA_TYPE_ADHOC	BIT(2)
#define STA_TYPE_TDLS		BIT(3)
#define STA_TYPE_WDS		BIT(4)

/* Network type */
#define NETWORK_INFRA	BIT(16)
#define NETWORK_P2P		BIT(17)
#define NETWORK_IBSS		BIT(18)
#define NETWORK_MESH	BIT(19)
#define NETWORK_BOW		BIT(20)
#define NETWORK_WDS		BIT(21)


#define DROP_STBC_BCN_BC_MC (1 << 0)
#define DROP_FCS_ERROR_FRAME (1 << 1)
#define RX_PROMISCUOUS_MODE (1 << 2)
#define DROP_VERSION_NO_0 (1 << 3)
#define DROP_PROBE_REQ (1 << 4)
#define DROP_MC_FRAME (1 << 5)
#define DROP_BC_FRAME (1 << 6)
#define DROP_NOT_IN_MC_TABLE (1 << 7)
#define DROP_ADDR3_OWN_MAC (1 << 8)
#define DROP_DIFF_BSSID_A3 (1 << 9)
#define DROP_DIFF_BSSID_A2 (1 << 10)
#define DROP_DIFF_BSSID_BCN (1 << 11)
#define RM_FRAME_REPORT_EN (1 << 12)
#define DROP_CTRL_RSV (1 << 13)
#define DROP_CTS (1 << 14)
#define DROP_RTS (1 << 15)
#define DROP_DUPLICATE (1 << 16)
#define DROP_NOT_MY_BSSID (1 << 17)
#define DROP_NOT_UC2ME (1 << 18)
#define DROP_DIFF_BSSID_BTIM (1 << 19)
#define DROP_NDPA (1 << 20)
#define RX_CTRL_FRAME (1 << 21)

/* Suggested in RMAC Control register datasheet */
#define IOT_MONITOR_MODE 	RM_FRAME_REPORT_EN | DROP_FCS_ERROR_FRAME 

/* Connection type */
/*
#define CONNECTION_INFRA_STA		(STA_TYPE_STA|NETWORK_INFRA)
#define CONNECTION_INFRA_AP		(STA_TYPE_AP|NETWORK_INFRA)
#define CONNECTION_P2P_GC			(STA_TYPE_STA|NETWORK_P2P)
#define CONNECTION_P2P_GO			(STA_TYPE_AP|NETWORK_P2P)
#define CONNECTION_MESH_STA		(STA_TYPE_STA|NETWORK_MESH)
#define CONNECTION_MESH_AP		(STA_TYPE_AP|NETWORK_MESH)
#define CONNECTION_IBSS_ADHOC		(STA_TYPE_ADHOC|NETWORK_IBSS)
#define CONNECTION_TDLS			(STA_TYPE_STA|NETWORK_INFRA|STA_TYPE_TDLS)
#define CONNECTION_WDS			(STA_TYPE_WDS|NETWORK_WDS)
*/

enum
{
	DEVINFO_ACTIVE = 0,
	DEVINFO_MAX_NUM = 1,
};

enum 
{
	DEVINFO_ACTIVE_FEATURE = (1 << DEVINFO_ACTIVE),
	DEVINFO_MAX_NUM_FEATURE = (1 << DEVINFO_MAX_NUM)
};


#ifdef MT_PS
#define I_PSM_ENABLE	1
#define I_PSM_DISABLE	0
#endif /* MT_PS */

/**********************Default Profile Parameters********************************/
#if 0 //def IOT_SECURITY_SUPPORT
#define IOT_MAC_ADDRESS			"00:0c:43:76:62:42"				/* Hardcoded in WPASupllicant with WPA2PSK */
#else
#define IOT_MAC_ADDRESS			"00:0c:43:76:62:02"
#endif
#define IOT_APCLI_MAC_ADDRESS	"00:0c:43:76:61:01" //Hack!!  CM4 uses fixed MAC ADDRESS. change it to 00:0c:43:76:62:02
#define IOT_COUNTRY_REGION		0
#define IOT_COUNTRY_CODE		"WO "
#define IOT_CHANNEL_GEOGRAPHY   BOTH
#define IOT_BSID_NUM			1
#define IOT_AP_SSID				"IoT_AP"
#if 0//def IOT_SECURITY_SUPPORT
#define IOT_STA_SSID			"233_0"
#else
#define IOT_STA_SSID			"IoT_AP2"
#endif
#define IOT_CHANNEL				1
#define IOT_PHY_MODE			PHY_11BGN_MIXED
#define IOT_BEACON_PERIOD		100
#define IOT_TX_POWER			100
#define IOT_BG_PROTECTION		0
#define IOT_DISABLE_OLBC_DETECT 1
#define IOT_TX_PREAMBLE			Rt802_11PreambleAuto
#define IOT_THRESHOLD			MAX_RTS_THRESHOLD
#define IOT_FRAG_THRESHOLD		MAX_FRAG_THRESHOLD

/****** COMMON WMM PARAM *****/
#define IOT_WMM_CAPABLE			1

/**********AP WMM PARAMETERS **************/
/****** COMMON WMM PARAM *****/
#define IOT_WMM_CAPABLE			1
#define IOT_ACK_POLICY0			0
#define IOT_ACK_POLICY1			0
#define IOT_ACK_POLICY2			0
#define IOT_ACK_POLICY3			0

/**********AP WMM PARAMETERS **************/
#define IOT_DLS_CAPABLE			0

#define IOT_AP_AIFSN0			3
#define IOT_AP_AIFSN1			7
#define IOT_AP_AIFSN2			1
#define IOT_AP_AIFSN3			1

#define IOT_AP_CW_MIN0			4
#define IOT_AP_CW_MIN1			4
#define IOT_AP_CW_MIN2			3
#define IOT_AP_CW_MIN3			2

#define IOT_AP_CW_MAX0			6
#define IOT_AP_CW_MAX1			10
#define IOT_AP_CW_MAX2			4
#define IOT_AP_CW_MAX3			3

#define IOT_AP_TxOP0			0
#define IOT_AP_TxOP1			0
#define IOT_AP_TxOP2			94
#define IOT_AP_TxOP3			47

#define IOT_AP_ACM0				0
#define IOT_AP_ACM1				0
#define IOT_AP_ACM2				0
#define IOT_AP_ACM3				0


#define IOT_BSS_AIFSN0			3
#define IOT_BSS_AIFSN1			7
#define IOT_BSS_AIFSN2			2
#define IOT_BSS_AIFSN3			2

#define IOT_BSS_CW_MIN0			4
#define IOT_BSS_CW_MIN1			4
#define IOT_BSS_CW_MIN2			3
#define IOT_BSS_CW_MIN3			2

#define IOT_BSS_CW_MAX0			10
#define IOT_BSS_CW_MAX1			10
#define IOT_BSS_CW_MAX2			4
#define IOT_BSS_CW_MAX3			3

#define IOT_BSS_TxOP0			0
#define IOT_BSS_TxOP1			0
#define IOT_BSS_TxOP2			94
#define IOT_BSS_TxOP3			47

#define IOT_BSS_ACM0			0
#define IOT_BSS_ACM1			0
#define IOT_BSS_ACM2			0
#define IOT_BSS_ACM3			0




#define IOT_MAX_STA_NUM			4
#define IOT_IDLE_TIMEOUT		600
#define IOT_HIDE_SSID			0
#define IOT_STA_KEEP_ALIVE		60
#define IOT_AUTH_MODE			Ndis802_11AuthModeOpen
#define IOT_ENCRYP_TYPE			Ndis802_11WEPDisabled
#define IOT_WPA_MIX_PAIR_CIPHER	MIX_CIPHER_NOTUSE
#define IOT_REKEY_METHOD		DISABLE_REKEY		
#define IOT_MAC_ENTRY_LIFE_CHECK_CNT	1024


/******** APCLI *****************/
#define IOT_APCLI_ENABLE		1
#define IOT_APCLI_SSID			"IoT_AP1"
#define IOT_APCLI_BSSID			""
#define IOT_APCLI_AUTH_MODE		Ndis802_11AuthModeOpen
#define IOT_APCLI_ENCYP_TYPE	Ndis802_11WEPDisabled
#define IOT_APCLI_WPAPSK		"12345678"
#define IOT_APCLI_TXMODE		FIXED_TXMODE_OFDM
#define IOT_APCLI_TXMCS			MCS_AUTO
#define IOT_APCLI_NUM			1

#define IOT_HT_PROTECT			1
#define IOT_HT_BA_DECLINE		0
#define IOT_HT_AUTO_BA			1
#define IOT_HT_BA_POLICY		IMMED_BA
#define IOT_HT_AMSDU			0
#define IOT_HT_MPDU_DENSITY		0
#define IOT_HT_BA_WIN_SIZE		16
#define IOT_HT_GI				0
#define IOT_HT_LDPC				0
#define IOT_HT_OPMODE			HTMODE_MM
#define IOT_HT_FIXED_TXMODE		FIXED_TXMODE_HT
#define IOT_HT_BW				BW_20
#define IOT_HT_EXTCHA			EXTCHA_NONE
#define IOT_HT_MCS				MCS_3
#define IOT_HT_STBC				0
#define IOT_HT_40MHZ_INTOLERANT	0
#define IOT_HT_TxSTREAM			1
#define IOT_HT_RxSTREAM			1
#define IOT_HT_DISALLOW_TKIP	1
#define IOT_HT_OBSS_SCAN_PARAM	""
#define IOT_HT_BURST_MODE		0
#define IOT_TXRX_RXV_ON			0

#define IOT_REPEATER_EN			1
#define IOT_REPEATER_OUI_MODE	0

/*************STA MODE*************/
#define IOT_PS_MODE					Ndis802_11PowerModeCAM
#define IOT_BATTERY_MODE			Ndis802_11PowerModeCAM
#define IOT_AUTO_ROAMING			0
#define IOT_ROAM_THRESHOLD			70
#define IOT_BEACON_LOST_TIME		20
#define IOT_AUTO_CONNECT			0


/*****************************************************/

#define STATE_DISCONNECT 0
#define STATE_CONNECTED 1
#define STATE_PORT_SECURE 2
extern void (*fw_os_ms_delay)(unsigned int ui4_delay);

#define os_msec_delay(_V) fw_os_ms_delay(_V)

#define LPON_BTEIR LP_BEIR
#define LPON_PISR LP_PISR

#define RMACDR_MBSSID_MASK (0x3 << 24)
#define RMACDR_MBSSID(p) (((p) & 0x3) << 24)
#define CHK_HTC_BY_RATE (1 << 26)
#define SELECT_RXMAXLEN_20BIT (1 << 30)

#define __FUNCTION__ __func__

#define RTMP_STA_ENTRY_MAC_RESET(pAd, Wcid)	\
	AsicDelWcidTab(pAd, Wcid);
#define RTMP_UPDATE_PROTECT(_pAd, _OperationMode, _SetMask, _bDisableBGProtect, _bNonGFExist)	\
	AsicUpdateProtect(_pAd, _OperationMode, _SetMask, _bDisableBGProtect, _bNonGFExist);

#define RTMP_STA_ENTRY_KEY_DEL(pAd, BssIdx, Wcid)	\
	AsicRemovePairwiseKeyEntry(pAd, BssIdx, (UCHAR)Wcid);

#define RTMP_STA_ENTRY_ADD(pAd, Wcid,Addr)		\
	AsicUpdateRxWCIDTable(pAd, Wcid, Addr);

#define RTMP_REMOVE_PAIRWISE_KEY_ENTRY(_pAd, _Wcid)\
	AsicRemovePairwiseKeyEntry(_pAd, BSS0,_Wcid)
	
#define RTMP_ADDREMOVE_KEY(_pAd, _AddRemove, _BssIdx, _KeyIdx, _Wcid, _KeyTabFlag, _pCipherKey, _PeerAddr)\
		AsicAddRemoveKey(_pAd, _AddRemove, _BssIdx, _KeyIdx, _Wcid, _KeyTabFlag, _pCipherKey, _PeerAddr)

#define RTMP_SET_TR_ENTRY(pAd, pEntry)   \
	TRTableInsertEntry(pAd,pEntry->wcid, pEntry);


/* Set Asic Shared key table */
#define RTMP_ASIC_SHARED_KEY_TABLE(_pAd, _BssIndex, _KeyIdx, _pCipherKey) \
	//AsicAddSharedKeyEntry(_pAd, _BssIndex, _KeyIdx, _pCipherKey) : Not supported for MT

/* Set Asic WCID Attribute table */
#define RTMP_SET_WCID_SEC_INFO(_pAd, _BssIdx, _KeyIdx, _CipherAlg, _Wcid, _KeyTabFlag)	\
	//RTMPSetWcidSecurityInfo(_pAd, _BssIdx, _KeyIdx, _CipherAlg, _Wcid, _KeyTabFlag)



#endif  // __RTMP_DEF_H__


