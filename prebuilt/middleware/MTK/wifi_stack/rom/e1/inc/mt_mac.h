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
	mt_mac.h

	Abstract:
	Ralink Wireless Chip MAC related definition & structures

	Revision History:
	Who			When		  What
	--------	----------	  ----------------------------------------------
*/

#ifndef __MT_MAC_H__
#define __MT_MAC_H__
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#if 0
#include "mac/mac_mt/top.h"
#include "mac/mac_mt/smac/gpio.h"
#include "mac/mac_mt/smac/wf_agg.h"
#include "mac/mac_mt/smac/wf_aon.h"
#include "mac/mac_mt/smac/wf_arb.h"
#include "mac/mac_mt/smac/wf_cfgoff.h"
#include "mac/mac_mt/smac/wf_dma.h"
#include "mac/mac_mt/smac/wf_lpon_top.h"
#include "mac/mac_mt/smac/wf_mib.h"
#include "mac/mac_mt/smac/wf_pf.h"
#include "mac/mac_mt/smac/wf_rmac.h"
#include "mac/mac_mt/smac/wf_sec.h"
#include "mac/mac_mt/smac/wf_tmac.h"
#include "mac/mac_mt/smac/wf_trb.h"
#include "mac/mac_mt/smac/wf_wtbloff.h"
#include "mac/mac_mt/smac/wf_wtblon.h"
#include "mac/mac_mt/smac/wf_int_wakeup_top.h"
#include "mac/mac_mt/smac/client.h"
#include "mac/mac_mt/smac/wf_wtbl.h"
#include "mac/mac_mt/smac/wf_phy.h"
#include "mac/mac_mt/smac/pse.h"
#ifdef RTMP_MAC_PCI
#include "mac/mac_mt/mt_mac_pci.h"
#endif /* RTMP_MAC_PCI */

#ifndef COMPOS_WIN
#ifdef RTMP_MAC_USB
#include "mac/mac_mt/mt_mac_usb.h"
#endif /* RTMP_MAC_USB */
#endif /* COMPOS_WIN */

#ifdef RTMP_MAC_SDIO
#include "mac/mac_mt/mt_mac_sdio.h"
#endif /* RTMP_MAC_SDIO */

#endif
#include "wf_wtblon.h"

#include "wf_wtbl.h"
//#include "rtmp.h"
//#include "mac_rom.h"

/*
	TX / RX descriptor format

	TX:
		PCI/RBUS_DMA_Descriptor + TXD + 802.11

	Rx:
		PCI/RBUS/USB_DMA_Descripotr + RXD + 802.11 + RX_CSO_INFO
*/

#define FIFO_MGMT	0
#define FIFO_HCCA	1
#define FIFO_EDCA	2


#ifdef MAC_REPEATER_SUPPORT
#define MAX_EXT_MAC_ADDR_SIZE	16
//TODO: Carter, single driver case?
#endif /* MAC_REPEATER_SUPPORT */


/******************************************************************************
	LMAC TxD
	
	TMAC_TXD_0
	TMAC_TXD_1
	TMAC_TXD_2
	TMAC_TXD_3

******************************************************************************/

/******************************************************************************
	TMAC_TXD_0
******************************************************************************/
#ifdef RELEASE_EXCLUDE
/*
	TxByteCount[15:0]: This is the total byte count of the transmit data structure 
	EthTypeOffset[22:16]: This field indicates the offset of Ether-Type field from the fist byte of the transmit data structure
	IpChkSum[23:23]: Request to do IP checksum offload
	UTChkSum[24:24]: Request to do UDP/TCP checksum offload
	UNxV[25:25]: USB NextVLD (only valid when USB interface is selected)
	UTxB[26:26]: USB TxBURST (only valid when USB interface is selected)
	QIdx[30:27]: Indicate the destination Queue Index
	PIdx[31:31]: Indicate the destination port ID
*/
#endif /* RELEASE_EXCLUDE */

// +++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _TMAC_TXD_0 {
	/*  DWORD 0 */
	UINT32 PIdx:1;
	UINT32 QIdx:4;
	UINT32 UTxB:1;
	UINT32 UNxV:1;
	UINT32 UTChkSum:1;
	UINT32 IpChkSum:1;
	UINT32 EthTypeOffset:7;
	UINT32 TxByteCount:16;
} TMAC_TXD_0, *PTMAC_TXD_0;
#else
typedef struct GNU_PACKED _TMAC_TXD_0 {
	/* DWORD 0 */
	UINT32 TxByteCount:16;
	UINT32 EthTypeOffset:7;
	UINT32 IpChkSum:1;
	UINT32 UTChkSum:1;
	UINT32 UNxV:1;
	UINT32 UTxB:1;
	UINT32 QIdx:4;
	UINT32 PIdx:1;
} TMAC_TXD_0, *PTMAC_TXD_0;
#endif
//---UNIFIED_TXD_RXD

/* TMAC_TXD_0.PIdx */
#define P_IDX_LMAC	0
#define P_IDX_MCU	1

/* TMAC_TXD_0.QIdx */
#define Q_IDX_AC0	0x00
#define Q_IDX_AC1	0x01
#define Q_IDX_AC2	0x02
#define Q_IDX_AC3	0x03
#define Q_IDX_AC4	0x04
#define Q_IDX_AC5	0x05
#define Q_IDX_AC6	0x06
#define Q_IDX_BCN	0x07
#define Q_IDX_BMC	0x08
#define Q_IDX_AC10	0x09
#define Q_IDX_AC11	0x0a
#define Q_IDX_AC12	0x0b
#define Q_IDX_AC13	0x0c
#define Q_IDX_AC14	0x0d

#define Q_IDX_RQ0	0x00
#define Q_IDX_RQ1	0x01
#define Q_IDX_RQ2	0x02
#define Q_IDX_RQ3	0x03

/* TMAC_TXD_0.PIdx +  TMAC_TXD_0.QIdx */
#define PQ_IDX_LMAC_AC0	(P_IDX_LMAC << 4) | (Q_IDX_AC0)
#define PQ_IDX_LMAC_AC1	(P_IDX_LMAC << 4) | (Q_IDX_AC1)
#define PQ_IDX_LMAC_AC2	(P_IDX_LMAC << 4) | (Q_IDX_AC2)
#define PQ_IDX_LMAC_AC3	(P_IDX_LMAC << 4) | (Q_IDX_AC3)
#define PQ_IDX_LMAC_AC4	(P_IDX_LMAC << 4) | (Q_IDX_AC4)
#define PQ_IDX_LMAC_AC5	(P_IDX_LMAC << 4) | (Q_IDX_AC5)
#define PQ_IDX_LMAC_AC6	(P_IDX_LMAC << 4) | (Q_IDX_AC6)
#define PQ_IDX_LMAC_BCN	(P_IDX_LMAC << 4) | (Q_IDX_BCN)
#define PQ_IDX_LMAC_BMC	(P_IDX_LMAC << 4) | (Q_IDX_BMC)
#define PQ_IDX_LMAC_AC10	(P_IDX_LMAC << 4) | (Q_IDX_AC10)
#define PQ_IDX_LMAC_AC11	(P_IDX_LMAC << 4) | (Q_IDX_AC11)
#define PQ_IDX_LMAC_AC12	(P_IDX_LMAC << 4) | (Q_IDX_AC12)
#define PQ_IDX_LMAC_AC13	(P_IDX_LMAC << 4) | (Q_IDX_AC13)
#define PQ_IDX_LMAC_AC14	(P_IDX_LMAC << 4) | (Q_IDX_AC14)

#define PQ_IDX_MCU_RQ0		(P_IDX_MCU << 4) | (Q_IDX_RQ0)
#define PQ_IDX_MCU_RQ1		(P_IDX_MCU << 4) | (Q_IDX_RQ1)
#define PQ_IDX_MCU_RQ2		(P_IDX_MCU << 4) | (Q_IDX_RQ2)
#define PQ_IDX_MCU_RQ3		(P_IDX_MCU << 4) | (Q_IDX_RQ3)


#define WTBL_DW0_MUARIDX_MASK	0x3f0000
#define WTBL_DW0_MUARIDX_BIT		16
#define WTBL_SET_MUARIDX(_x, _y)	(_x = (_x & (~(0x3f<<16))) | (((_y) & 0x3f)<<16))
#define WTBL_SET_RC_A1(_x, _y)		(_x = (_x & (~(0x1<<22))) | (((_y) & 0x1)<<22))
#define WTBL_SET_KID(_x, _y)		(_x = (_x & (~(0x3<<23))) | (((_y) & 0x3)<<23))
#define WTBL_SET_RCID(_x, _y)		(_x = (_x & (~(0x1<<25))) | (((_y) & 0x1)<<25))
#define WTBL_SET_RKV(_x, _y)		(_x = (_x & (~(0x1<<26))) | (((_y) & 0x1)<<26))
#define WTBL_SET_IKV(_x, _y)		(_x = (_x & (~(0x1<<27))) | (((_y) & 0x1)<<27))
#define WTBL_SET_RV(_x, _y)			(_x = (_x & (~(0x1<<28))) | (((_y) & 0x1)<<28))
#define WTBL_SET_RC_A2(_x, _y)		(_x = (_x & (~(0x1<<29))) | (((_y) & 0x1)<<29))

#define WTBL_SET_PEER_ADDR4(_x, _y)	(_x = (_x & (~(0xff))) | (((_y) & 0xff)<<0))
#define WTBL_SET_PEER_ADDR5(_x, _y)	(_x = (_x & (~(0xff<<8))) | (((_y) & 0xff)<<8))
#define WTBL_SET_PEER_ADDR0_3(_x, _y)	((_x) = ((_y) & 0xffffffff))

#define WTBL_SET_DIS_RHTR(_x, _y)		(_x = (_x & (~(0x1<<1))) | (((_y) & 0x1)<<1))
#define WTBL_SET_WPI_EVEN(_x, _y)		(_x = (_x & (~(0x1<<2))) | (((_y) & 0x1)<<2))
#define WTBL_SET_AAD_OM(_x, _y)		(_x = (_x & (~(0x1<<3))) | (((_y) & 0x1)<<3))
#define WTBL_SET_CIPHER_SUIT(_x, _y)	(_x = (_x & (~(0xf<<4))) | (((_y) & 0xf)<<4))
#define WTBL_SET_PFMU_IDX(_x, _y)		(_x = (_x & (~(0xff<<8))) | (((_y) & 0xff)<<8))
#define WTBL_SET_PARTIAL_AID(_x, _y)	(_x = (_x & (~(0x1ff<<16))) | (((_y) & 0x1ff)<<16))
#define WTBL_SET_TIBF_HT(_x, _y)		(_x = (_x & (~(0x1<<25))) | (((_y) & 0x1)<<25))
#define WTBL_SET_TEBF_HT(_x, _y)		(_x = (_x & (~(0x1<<26))) | (((_y) & 0x1)<<26))
#define WTBL_SET_TIBF_VHT(_x, _y)		(_x = (_x & (~(0x1<<27))) | (((_y) & 0x1)<<27))
#define WTBL_SET_TEBF_VHT(_x, _y)		(_x = (_x & (~(0x1<<28))) | (((_y) & 0x1)<<28))
#define WTBL_SET_HT(_x, _y)				(_x = (_x & (~(0x1<<29))) | (((_y) & 0x1)<<29))
#define WTBL_SET_VHT(_x, _y)			(_x = (_x & (~(0x1<<30))) | (((_y) & 0x1)<<30))
#define WTBL_SET_MESH(_x, _y)			(_x = (_x & (~(0x1<<31))) | (((_y) & 0x1)<<31))

#define WTBL_SET_TXPWR_OFFSET(_x, _y)	(_x = ((_x) & (~0x0000001f)) | (((_y) & 0x1f)<<0))
#define WTBL_SET_FCAP(_x, _y)			(_x = ((_x) & (~0x00003000)) | (((_y) & 0x3)<<12))
#define WTBL_SET_LDPC(_x, _y)			(_x = ((_x) & (~0x00004000)) | (((_y) & 0x1)<<14))
#define WTBL_SET_LDPC_VHT(_x, _y)		(_x = ((_x) & (~0x00008000)) | (((_y) & 0x1)<<12))
#define WTBL_SET_MM(_x, _y)				(_x = ((_x) & (~0x00070000)) | (((_y) & 0x7)<<16))
#define WTBL_SET_QOS(_x, _y)			(_x = ((_x) & (~0x00080000)) | (((_y) & 0x1)<<19))
#define WTBL_SET_FD(_x, _y)				(_x = ((_x) & (~0x00100000)) | (((_y) & 0x1)<<20))
#define WTBL_SET_TD(_x, _y)				(_x = ((_x) & (~0x00200000)) | (((_y) & 0x1)<<21))
#define WTBL_SET_DYN_BW(_x, _y)		(_x = ((_x) & (~0x00400000)) | (((_y) & 0x1)<<22))


#define DMAC_WTBL_DW3_SET_BAF(_x, _y)		(_x = ((_x) & (~0x00001000)) | (((_y) & 0x1)<<12))
#define DMAC_WTBL_DW3_SET_RDGBA(_x, _y)	(_x = ((_x) & (~0x00004000)) | (((_y) & 0x1)<<14))
#define DMAC_WTBL_DW3_SET_R(_x, _y)		(_x = ((_x) & (~0x00008000)) | (((_y) & 0x1)<<15))
#define DMAC_WTBL_DW3_SET_SMPS(_x, _y)	(_x = ((_x) & (~0x00800000)) | (((_y) & 0x1)<<23))
#define DMAC_WTBL_DW3_SET_RTS(_x, _y)		(_x = ((_x) & (~0x00400000)) | (((_y) & 0x1)<<22))
#define DMAC_WTBL_DW3_SET_TXPS(_x, _y)	(_x = ((_x) & (~0x00200000)) | (((_y) & 0x1)<<21))
#define DMAC_WTBL_DW3_SET_AF(_x, _y)		(_x = ((_x) & (~0x07000000)) | (((_y) & 0x7)<<24))
#define DMAC_WTBL_DW3_SET_DU_IPSM(_x, _y)	(_x = ((_x) & (~0x10000000)) | (((_y) & 0x1)<<28))
#define DMAC_WTBL_DW3_SET_IPSM(_x, _y)		(_x = ((_x) & (~0x20000000)) | (((_y) & 0x1)<<29))
/******************************************************************************
	TMAC_TXD_1
******************************************************************************/
#ifdef RELEASE_EXCLUDE
/*
WlanIdx[7:0]: This field is used to identify the WLAN Table index when transmission.
HdrInfo[12:8]: Header information depend on HdrF
HdrF[14:13]: Header Format
TxDF[15:15]: Tx descriptor format
HdrPad[18:16]: Indicate the header padding byte count
NoAck[19:19]:No_ACK, this bit is used for indicating if HW MAC needs to wait a response frame for current packet.
Tid[22:20]:TID of QoS Data frame, otherwise it should be set to 0. HW can aggregate packets with the same TID when there is a valid RA/TID entry in BA table.
ProtectFrame[]:Protected Frame Field. Indicating this is a packet need to be encrypted.
Rsv24to25[25:24]: Reserve bits
OwnMacAddr[31:26]: Own MAC Address. This field indicates the Own MAC address of current packet.
*/
#endif /* RELEASE_EXCLUDE */

//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _TMAC_TXD_1 {
	/* DWORD 1 */
	UINT32 OwnMacAddr:6;
	UINT32 Rsv24to25:2;
	UINT32 ProtectFrame:1;
	UINT32 Tid:3;
	UINT32 NoAck:1;
	UINT32 HdrPad:3;
	UINT32 TxDFmt:1;
	UINT32 HdrFmt:2;
	UINT32 HdrInfo:5;
	UINT32 WlanIdx:8;
} TMAC_TXD_1, *PTMAC_TXD_1;
#else
typedef struct GNU_PACKED _TMAC_TXD_1 {
	/* DWORD 1 */
	UINT32 WlanIdx:8;
	UINT32 HdrInfo:5;
	UINT32 HdrFmt:2;
	UINT32 TxDFmt:1;
	UINT32 HdrPad:3;
	UINT32 NoAck:1;
	UINT32 Tid:3;
	UINT32 ProtectFrame:1;
	UINT32 Rsv24to25:2;
	UINT32 OwnMacAddr:6;
} TMAC_TXD_1, *PTMAC_TXD_1;
#endif
//---UNIFIED_TXD_RXD

/* TMAC_TXD_1.HdrFmt */
#define TMI_HDR_FT_NON_80211	0x0
#define TMI_HDR_FT_CMD		0x1
#define TMI_HDR_FT_NOR_80211	0x2
#define TMI_HDR_FT_ENH_80211	0x3

/*
	TMAC_TXD_1.HdrInfo
*/
/* if TMAC_TXD_1.HdrFmt  == HDR_FORMAT_NON_80211 */
#define TMI_HDR_INFO_0_BIT_MRD		0
#define TMI_HDR_INFO_0_BIT_EOSP		1
#define TMI_HDR_INFO_0_BIT_RMVL		2
#define TMI_HDR_INFO_0_BIT_VLAN		3
#define TMI_HDR_INFO_0_BIT_ETYP		4
#define TMI_HDR_INFO_0_VAL(_mrd, _eosp, _rmvl, _vlan, _etyp)	\
			((((_mrd) ? 1 : 0) << TMI_HDR_INFO_0_BIT_MRD) | \
			(((_eosp) ? 1 : 0) << TMI_HDR_INFO_0_BIT_EOSP) |\
			(((_rmvl) ? 1 : 0) << TMI_HDR_INFO_0_BIT_RMVL) |\
			(((_vlan) ? 1 : 0) << TMI_HDR_INFO_0_BIT_VLAN) |\
			(((_etyp) ? 1 : 0) << TMI_HDR_INFO_0_BIT_ETYP))


/* if TMAC_TXD_1.HdrFmt  == HDR_FORMAT_CMD */
#define TMI_HDR_INFO_1_MASK_RSV		(0x1f)
#define TMI_HDR_INFO_1_VAL			0

/* if TMAC_TXD_1.HdrFmt  == HDR_FORMAT_NOR_80211 */
#define TMI_HDR_INFO_2_MASK_LEN	(0x1f)
#define TMI_HDR_INFO_2_VAL(_len)	(_len >> 1)

/* if TMAC_TXD_1.HdrFmt  == HDR_FORMAT_ENH_80211 */
#define TMI_HDR_INFO_3_BIT_EOSP	1
#define TMI_HDR_INFO_3_BIT_AMS	2
#define TMI_HDR_INFO_3_VAL(_eosp, _ams)	\
			((((_eosp) ? 1 : 0) << TMI_HDR_INFO_3_BIT_EOSP) | \
			(((_ams) ? 1 : 0) << TMI_HDR_INFO_3_BIT_AMS))

#define TMI_HDR_INFO_VAL(_fmt, _mrd, _eosp, _rmvl, _vlan, _etype, _len, _ams, _val)	\
		do{\
			switch (_fmt) {\
				case TMI_HDR_FT_NON_80211:\
					(_val) = TMI_HDR_INFO_0_VAL((_mrd), (_eosp), (_rmvl), (_vlan), (_etype));\
					break;\
				case TMI_HDR_FT_CMD:\
					(_val) = TMI_HDR_INFO_1_VAL;\
					break;\
				case TMI_HDR_FT_NOR_80211:\
					ASSERT((((_len) & 1) == 0));\
					(_val) = TMI_HDR_INFO_2_VAL((_len));\
					break;\
				case TMI_HDR_FT_ENH_80211:\
					(_val) = TMI_HDR_INFO_3_VAL((_eosp), (_ams));\
				default:\
					(_val) = 0;\
			}\
		}while(0)


/* TMAC_TXD_1.TxDFmt */
#define TMI_FT_SHORT	0
#define TMI_FT_LONG		1


/* TMAC_TXD_1.HdrPad */
#define TMI_HDR_PAD_BIT_MODE		2
#define TMI_HDR_PAD_MODE_TAIL		0
#define TMI_HDR_PAD_MODE_HEAD	1
#define TMI_HDR_PAD_BIT_LEN	0
#define TMI_HDR_PAD_MASK_LEN	0x3


/******************************************************************************
	TMAC_TXD_2
******************************************************************************/
#ifdef RELEASE_EXCLUDE
/*
SubType[3:0]: Frame Subtype Field 
FrmType[5:4]: Frame Type Field 
Ndp[6:6]: indicates the frame is a NDP
Ndpa[7:7]: indicates the frame is a NDP announcement frame (11n) or NDPA (11ac)
Sounding[8:8]: Sounding PPDU, when asserted, this frame is a sounding PPDU, it will not be aggregated with other frame.
Rts[9:9]: Used for BT over WiFi application. Firmware can set this bit to 1 and must ignore all other protection conditions while transmitting BT over WiFi packet.
BmcPkt[10:10]: Broadcast/Multicast packet. FW would parse the A1 of WLAN header and set this bit if A1 is a BMCAST address. When this bit is set, HW MAC would default treat this packet as No-ACK packet.
Bip[11:11]: This field indicates this packet should be BIP protected.
Duration[12:12]: This bit is used to control the Duration field content maintained by HW MAC or Firmware. This bit is useless when the packet is not in 802.11 formats.
HtcExist[13:13]: HTC Exist. Indicating this is a packet already contains HTC field.
Frag[15:14]: indicate the fragment packet position
MaxTxTime[23:16]: This field indicates the remaining life time in unit of 8TU used for this packet.
PwrOffset[28:24]: Per-packet TX power control While TXing packet, look up the power value from the FPRx mapping table. Add this power offset from the power value and find the mapping power code from PCMRN.
BaDisable[29:29]: indicate that the packet can NOT be aggregated to AMPDU
TimingMeasure[30:30]: Timing Measurement
FixRate[31:31]: Fixed Rate
*/
#endif /* RELEASE_EXCLUDE */


//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _TMAC_TXD_2 {
	/* DWORD 2 */
	UINT32 FixRate:1;
	UINT32 TimingMeasure:1;
	UINT32 BaDisable:1;
	UINT32 PwrOffset:5;
	UINT32 MaxTxTime:8;
	UINT32 Frag:2;
	UINT32 HtcExist:1;
	UINT32 Duration:1;
	UINT32 Bip:1;
	UINT32 BmcPkt:1;
	UINT32 Rts:1;
	UINT32 Sounding:1;
	UINT32 Ndpa:1;
	UINT32 Ndp:1;
	UINT32 FrmType:2;
	UINT32 SubType:4;
} TMAC_TXD_2, *PTMAC_TXD_2;
#else
typedef struct GNU_PACKED _TMAC_TXD_2 {
	/* DWORD 2 */
	UINT32 SubType:4;
	UINT32 FrmType:2;
	UINT32 Ndp:1;
	UINT32 Ndpa:1;
	UINT32 Sounding:1;
	UINT32 Rts:1;
	UINT32 BmcPkt:1;
	UINT32 Bip:1;
	UINT32 Duration:1;
	UINT32 HtcExist:1;
	UINT32 Frag:2;
	UINT32 MaxTxTime:8;
	UINT32 PwrOffset:5;
	UINT32 BaDisable:1;
	UINT32 TimingMeasure:1;
	UINT32 FixRate:1;
} TMAC_TXD_2, *PTMAC_TXD_2;
#endif
//---UNIFIED_TXD_RXD

/******************************************************************************
	TMAC_TXD_3
******************************************************************************/
#ifdef RELEASE_EXCLUDE
/*
TxCnt[10:6]: TX Count
RemainTxCnt[15:11]: Remaining TX Count
Sn[27:16]: sequence number
Pnvld[30:30]: PN in TX descriptor is valid
SnVld[31:31]: SN in TX descriptor is valid
*/
#endif /* RELEASE_EXCLUDE */

//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _TMAC_TXD_3 {
	/* DWORD 3 */
	UINT32 SnVld:1;
	UINT32 PnVld:1;
	UINT32 Rsv28to29:2;
	UINT32 Sn:12;
	UINT32 RemainTxCnt:5;
	UINT32 TxCnt:5;
	UINT32 Rsv0to5:6;
} TMAC_TXD_3, *PTMAC_TXD_3;
#else
typedef struct GNU_PACKED _TMAC_TXD_3 {
	/* DWORD 3 */
	UINT32 Rsv0to5:6;
	UINT32 TxCnt:5;
	UINT32 RemainTxCnt:5;
	UINT32 Sn:12;
	UINT32 Rsv28to29:2;
	UINT32 PnVld:1;
	UINT32 SnVld:1;
} TMAC_TXD_3, *PTMAC_TXD_3;
#endif
//---UNIFIED_TXD_RXD

/******************************************************************************
	TMAC_TXD_4
******************************************************************************/
#ifdef RELEASE_EXCLUDE
/*
	PktNumLow	[31 : 0]	Packet Number [31 : 0]. 
*/
#endif /* RELEASE_EXCLUDE */
//+++UNIFIED_TXD_RXD
typedef struct GNU_PACKED _TMAC_TXD_4 {
    UINT32   PktNumLow;
} TMAC_TXD_4, *PTMAC_TXD_4;
//---UNIFIED_TXD_RXD


/******************************************************************************
	TMAC_TXD_5
******************************************************************************/
#ifdef RELEASE_EXCLUDE
/*
	PktId		[7 : 0] 	Packet ID. the field is valid only when TXS2H or TXS2M is set.
	TxSFmt		[8] 		Tx status format. 0 : time stamp / front time mode. 1 : noisy / RCPI mode
	TxS2Mcu		[9]      	Tx status must return to MCU. 0 : no effect. 1 : this packet's status must return to MCU
	TxS2Host	[10]     	Tx status must return to Host. 0 : no effect. 1 : this packet's status must return to Host.
	DataSrcSelect	[11]     	data source selection. (valid for Tx translation only)
	BarSsnCtrl	[12]     	BAR SSN control. 0 : HW mode, the SSN is assigned by HW. 1 : SW mode, the SSN is assigned by SW.
	PwrMgmt		[13] 	    	power management. 0 : controlled by HW. 1 : controlled by SW. This bit is used to enable/disable the power management control of PM bit in frame control field.
	Rsv		[15 : 14]    	Reserve bits.
	PktNumHigh	[31 : 16]    	Packet Number [47 : 32].
*/
#endif /* RELEASE_EXCLUDE */
//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _TMAC_TXD_5 {
    UINT32   PktNumHigh:16;
    UINT32   Rsv:2;
    UINT32   PwrMgmt:1;
    UINT32   BarSsnCtrl:1;
    UINT32   DataSrcSelect:1;
    UINT32   TxS2Host:1;
    UINT32   TxS2Mcu:1;
    UINT32   TxSFmt:1;
    UINT32   PktId:8;
} TMAC_TXD_5, *PTMAC_TXD_5;
#else
typedef struct GNU_PACKED _TMAC_TXD_5 {
    UINT32   PktId:8;
    UINT32   TxSFmt:1;
    UINT32   TxS2Mcu:1;
    UINT32   TxS2Host:1;
    UINT32   DataSrcSelect:1;
    UINT32   BarSsnCtrl:1;
    UINT32   PwrMgmt:1;
    UINT32   Rsv:2;
    UINT32   PktNumHigh:16;
} TMAC_TXD_5, *PTMAC_TXD_5;
#endif
//---UNIFIED_TXD_RXD

/* TMAC_TXD_5.DataSrcSelect */
#define TMI_DAS_FROM_MPDU		0
#define TMI_DAS_FROM_WTBL		1

/* TMAC_TXD_5.BarSsnCtrl */
#define TMI_BSN_CFG_BY_HW	0
#define TMI_BSN_CFG_BY_SW	1

/* TMAC_TXD_5.PwrMgmt */
#define TMI_PM_BIT_CFG_BY_HW	0
#define TMI_PM_BIT_CFG_BY_SW	1


/******************************************************************************
	TMAC_TXD_6
******************************************************************************/
#ifdef RELEASE_EXCLUDE
/*
	FixedRateMode		[0]      	fixed rate mode. 0 : use the TX.Dscr's fixed rate to transmit current frame. 1 : use the CR's fixed rate to transmit current frame.
	Rsv			[1]      	Reserve bit.
	AntIdx			[7 : 2]		the smart antenna index..
	FixedBwMode		[10 : 8]	Fixed Bandwidth Mdoe.
	SpeExtEnable		[11]		Spetial Extenstion enable.
	AntPri			[14 : 12]	Antenna priority.
	DynamicBw		[15]		use dynamic bandwidth.
	ExplicitBf		[16]		Use explicit beamforming matrix to transmit packet.
	ImplicitBf		[17]		Use implicit beamforming matrix to transmit packet.
	RateToBeFixed		[29 : 18]	Rate to be fixed. To see spec.
	LDPC			[30]		LDPC. 0 : BCC. 1 : LDPC
	GI			[31]		Guard Interval mode of the Fixed Rate. 0 : normal. 1 : short GI
*/
#endif /* RELEASE_EXCLUDE */
//+++UNIFIED_TXD_RXD

#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _TMAC_TXD_6 {
    UINT32   GI:1;
    UINT32   LDPC:1;
    UINT32   RateToBeFixed:12;
    UINT32   ImplicitBf:1;
    UINT32   ExplicitBf:1;
    UINT32   DynamicBw:1;
    UINT32   AntPri:3;
    UINT32   SpeExtEnable:1;
    UINT32   FixedBwMode:3;
    UINT32   AntIdx:6;
    UINT32   Rsv:1;
    UINT32   FixedRateMode:1;
} TMAC_TXD_6, *PTMAC_TXD_6;
#else
typedef struct GNU_PACKED _TMAC_TXD_6 {
    UINT32   FixedRateMode:1;
    UINT32   Rsv:1;
    UINT32   AntIdx:6;
    UINT32   FixedBwMode:3;
    UINT32   SpeExtEnable:1;
    UINT32   AntPri:3;
    UINT32   DynamicBw:1;
    UINT32   ExplicitBf:1;
    UINT32   ImplicitBf:1;
    UINT32   RateToBeFixed:12;
    UINT32   LDPC:1;
    UINT32   GI:1;
} TMAC_TXD_6, *PTMAC_TXD_6;
#endif
//---UNIFIED_TXD_RXD


/* TMAC_TXD_6.fix_rate_mode */
#define TMI_FIX_RATE_BY_TXD	0
#define TMI_FIX_RATE_BY_CR		1

#define TMI_TX_RATE_BIT_STBC		11
#define TMI_TX_RATE_BIT_NSS		9
#define TMI_TX_RATE_MASK_NSS		0x3

#define TMI_TX_RATE_BIT_MODE		6
#define TMI_TX_RATE_MASK_MODE	0x7
#define TMI_TX_RATE_MODE_CCK		0
#define TMI_TX_RATE_MODE_OFDM	1
#define TMI_TX_RATE_MODE_HTMIX	2
#define TMI_TX_RATE_MODE_HTGF		3
#define TMI_TX_RATE_MODE_VHT		4

#define SHORT_PREAMBLE 0
#define LONG_PREAMBLE 1

#define TMI_TX_RATE_BIT_MCS		0
#define TMI_TX_RATE_MASK_MCS		0x3f
#define TMI_TX_RATE_CCK_1M_LP		0
#define TMI_TX_RATE_CCK_2M_LP		1
#define TMI_TX_RATE_CCK_5M_LP		2
#define TMI_TX_RATE_CCK_11M_LP	3

#define TMI_TX_RATE_CCK_2M_SP		5
#define TMI_TX_RATE_CCK_5M_SP		6
#define TMI_TX_RATE_CCK_11M_SP	7

#define TMI_TX_RATE_OFDM_6M		11
#define TMI_TX_RATE_OFDM_9M		15
#define TMI_TX_RATE_OFDM_12M		10
#define TMI_TX_RATE_OFDM_18M		14
#define TMI_TX_RATE_OFDM_24M		9
#define TMI_TX_RATE_OFDM_36M		13
#define TMI_TX_RATE_OFDM_48M		8
#define TMI_TX_RATE_OFDM_54M		12

#define TMI_TX_RATE_HT_MCS0		0
#define TMI_TX_RATE_HT_MCS1		1
#define TMI_TX_RATE_HT_MCS2		2
#define TMI_TX_RATE_HT_MCS3		3
#define TMI_TX_RATE_HT_MCS4		4
#define TMI_TX_RATE_HT_MCS5		5
#define TMI_TX_RATE_HT_MCS6		6
#define TMI_TX_RATE_HT_MCS7		7
#define TMI_TX_RATE_HT_MCS8		8
#define TMI_TX_RATE_HT_MCS9		9
#define TMI_TX_RATE_HT_MCS10		10
#define TMI_TX_RATE_HT_MCS11		11
#define TMI_TX_RATE_HT_MCS12		12
#define TMI_TX_RATE_HT_MCS13		13
#define TMI_TX_RATE_HT_MCS14		14
#define TMI_TX_RATE_HT_MCS15		15
#define TMI_TX_RATE_HT_MCS16		16
#define TMI_TX_RATE_HT_MCS17		17
#define TMI_TX_RATE_HT_MCS18		18
#define TMI_TX_RATE_HT_MCS19		19
#define TMI_TX_RATE_HT_MCS20		20
#define TMI_TX_RATE_HT_MCS21		21
#define TMI_TX_RATE_HT_MCS22		22
#define TMI_TX_RATE_HT_MCS23		23

#define TMI_TX_RATE_HT_MCS32		32

#define TMI_TX_RATE_VHT_MCS0		0
#define TMI_TX_RATE_VHT_MCS1		1
#define TMI_TX_RATE_VHT_MCS2		2
#define TMI_TX_RATE_VHT_MCS3		3
#define TMI_TX_RATE_VHT_MCS4		4
#define TMI_TX_RATE_VHT_MCS5		5
#define TMI_TX_RATE_VHT_MCS6		6
#define TMI_TX_RATE_VHT_MCS7		7
#define TMI_TX_RATE_VHT_MCS8		8
#define TMI_TX_RATE_VHT_MCS9		9


/******************************************************************************
	TMAC_TXD_7
******************************************************************************/
#ifdef RELEASE_EXCLUDE
/*
	SchdTxTime		[15 : 0]
	SwFieldReserve		[22 : 16]
	SwUseSegIdx		[26 : 23]
	SwUseUSB4ByteAlign	[28 : 27]
	SwUseAMSDU		[29]
	SwUseSegmentEnd		[30]
	SwUseNonQoS		[31]
*/
#endif /* RELEASE_EXCLUDE */

//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
// TODO: Lens, need to make more specific and clear definition for those fields!
typedef struct _TMAC_TXD_7
{
    UINT32   SwUseNonQoS:1; // remove
    UINT32   SwUseSegmentEnd:1; 
    UINT32   SwUseAMSDU:1; // remove
    UINT32   SwUseUSB4ByteAlign:2; // remove
    UINT32   SwUseSegIdx:6;
    UINT32   SwFieldReserve:5;
    UINT32   SchdTxTime:16;
} TMAC_TXD_7, *PTMAC_TXD_7;
#else
typedef struct _TMAC_TXD_7
{
    UINT32   SchdTxTime:16;
    UINT32   SwFieldReserve:5;
    UINT32   SwUseSegIdx:6;
    UINT32   SwUseUSB4ByteAlign:2;
    UINT32   SwUseAMSDU:1;
    UINT32   SwUseSegmentEnd:1;
    UINT32   SwUseNonQoS:1;
} TMAC_TXD_7, *PTMAC_TXD_7;
#endif
//---UNIFIED_TXD_RXD


/******************************************************************************
	TX Descriptor in Long Format (TMAC_TXD_1.FT = 1)
	which including TMAC_TXD_0~ TMAC_TXD_7
******************************************************************************/
//+++UNIFIED_TXD_RXD
typedef struct GNU_PACKED _TMAC_TXD_L {
	TMAC_TXD_0 TxD0;
	TMAC_TXD_1 TxD1;
	TMAC_TXD_2 TxD2;
	TMAC_TXD_3 TxD3;
	TMAC_TXD_4 TxD4;
	TMAC_TXD_5 TxD5;
	TMAC_TXD_6 TxD6;
	//TMAC_TXD_7 TxD7; Raghav : TXD only 7DW, no DMA scheduler
} TMAC_TXD_L, *PTMAC_TXD_L;
//---UNIFIED_TXD_RXD


/******************************************************************************
	TX Descriptor in Short Format (TMAC_TXD_1.FT = 0)
	which including TMAC_TXD_0, TMAC_TXD_1, and TMAC_TXD_7
******************************************************************************/
/*
	TX Descriptor in Short Format (TMAC_TXD_1.FT = 0)
*/
//+++UNIFIED_TXD_RXD
typedef struct GNU_PACKED _TMAC_TXD_S {
	TMAC_TXD_0 TxD0;
	TMAC_TXD_1 TxD1;
	TMAC_TXD_7 TxD7;
} TMAC_TXD_S, *PTMAC_TXD_S;
//---UNIFIED_TXD_RXD


typedef struct _TXV1_MAC2PHY {
	UCHAR txrate;
	UCHAR STBC;
	UCHAR Ldpc_Bcc;
	UCHAR Ness;
	UCHAR txmode;
	UCHAR bw;
	UCHAR txpower;
} TXV1_MAC2PHY;

typedef struct _TXV2_MAC2PHY {
	UCHAR agg;
	UCHAR gi;
	UCHAR smooth;
	UCHAR sounding;
	UCHAR txbf;
} TXV2_MAC2PHY;

typedef struct _TXV3_MAC2PHY {
	UCHAR bw_status;
	UCHAR mac[6];
} TXV3_MAC2PHY;


#ifdef RELEASE_EXCLUDE
/*
DWORD 0 ==>
	RxByteCnt(RX Byte count): in unit of byte
			The length includes RX status, Header padding, header, and payload
	EthTypeOffset(Ether-Type Offset): in unit of WORD
			Indicates the offset of Ether-Type field from the first byte of the
				tx data structure.
			This field is meaningful ONLY when UdpTcpChkSumOffload(UT) or IpChkSumOffload(I) field is 1
	IpChkSumOffload(I): Request to do IP checksum offload
			When set, this bit indicates the HW IP checksum check is needed and
			the result will pad in the last DW
	UdpTcpChkSumOffload(UT): Request to do UDP/TCP checksum offload
			When set, this bit indicates the HW UDP/TCP checksum check is needed
			and the result will pad in the last DW
	RfbGroupVld(GROUP_VLD): RFB Group valid indicators
			Bit[0] =>indicate GROUP 1(DW8~DW11)
			Bit[1] =>indicate GROUP 2(DW12~DW13)
			Bit[2] =>indicate GROUP 3(DW14~DW19)
			Bit[3] =>indicate GROUP 4(DW4~DW7)
			1'b0: the related GROUP is not present
			1'b1: the related GROUP is present
	pkt_type(PKT_TYPE):
			This field could be 3'h2(RX) or 3'h3(Duplicate RFB)

DWORD 1 ==>
	HTC(HTC): HTC field is valid
	UcastToMe(UC2ME): Unicast to ME frame
	Mcast(MC): Multicast frame
	Bcast(BC): Broadcast frame
	BcnWithBMcst(Beacon with BC/MC(BBM)):
			Indicate the beacon with BC/MC indication when
				MPTCR.BCN_PARSE_TIM_EN asserted
	BcnWithUCast(Beacon with Unicast (BU)):
			Indicate the beacon with UC indication when
				MPTCR. BCN_PARSE_TIM_EN asserted
	KeyId(Key ID (KID)): Indicates the matched Key ID
	ChFreq(Channel Frequency):
			Indicate the channel frequency for this packet. The coding rule will
			follow RF frequency register definition.
	MacHdrLen(MAC Header Length): in units of byte
			the header length
	HdrOffset(Header_Offset (HO)):
			This field indicates the padding byte count of header in unit of byte.
				1'b0: no padding
				1'b1: the padding length is 2 bytes
	HdrTranslation(H):
			1'b0: original 802.11 header
			1'b1: received frame is performed by HW RX header translation.
				If WF_DMA.DCR0.RX_HDR_TRANS_MODE is 0: Ethernet / 802.3 header.
				If WF_DMA.DCR0.RX_HDR_TRANS_MODE is 1: Enhanced 802.11 header (non-QoS Data)
	PayloadFmt(PF):
			2'b00: the payload is a MSDU
			2'b11: the payload is from the first sub frame of an A-MSDU
			2'b10: the payload is from a sub frame of an A-MSDU and it's neither the first nor the final.
			2'b01: the payload is from the final sub frame of an A-MSDU
	RxDBssidIdx(BSSID): indicate the matched BSSID
			6'h00: BSSID0 / InfraAP BSSID00
			6'h01: BSSID1
			6'h02: BSSID2
			6'h03: BSSID3
			6'h07: Not match any BSSID
			6'h11: InfraAP BSSID01
			...
			6'h1F: InfraAP BSSID0F
			6'h20: The RootAP BSSID of CloneMAC0 (for repeater mode only)
			6'h21: The RootAP BSSID of CloneMAC1 (for repeater mode only)
			...
			6'h2F: The RootAP BSSID of CloneMACF (for repeater mode only)
			Others: Reserved

DWORD 2 ==>
	RxDWlanIdx(WLAN Index):
			If HW needs to search the WTBL,
						report the matched index,
						if no entry was matched, report 255.
			If HW doesn't need to search the WTBL, always report 255.
			The WTBL search rules can refer to WTBL chapter.
	RxDTid(TID):
			RX packet is an QoS Data frame: = TID field in QoS control field.
			RX packet is not an QoS Data frame: = 0
	SecMode(SEC_mode): Security engine operation mode
			4'h0: None
			4'h1: WEP-40
			4'h2: TKIP w/ MIC
			4'h3: TKIP w/o MIC
			4'h4: CCMP w/ MIC
			4'h5: WEP-104
			4'h6: BIP
			4'h7: WEP-128
			4'h8: WPI
			4'h9: CCMP with CCX
			4'hA: GCMP
	SwBit(SWBIT): software assigned bit in WTBL
	FcsErr(FCS_Error (FC)): indicates the received frame contains FCS error
	format_err(Format_Error (FM)):
			it indicates the received frame containing frame format error
			 a). Length is too short for decryption or
			 b). cipher suit does not match (pf bit set but cipher suit = none or pf bit not set but cipher suit = cipher)).
			The WLAN Index, SEC_mode, Key ID should contain correct values
	IcvErr(ICV_Error (I)):
			Indicates the received frame contains ICV error or CCMP/BIP/WPI MIC error.
			This error might also combine with other error status bit
	mic_err(TKIPMIC_error (T)):
			indicates the received frame containing TKIP MIC error.
			If fragment frame is received, then MT6620will not check the MIC,
				and will not indicate this TKIP MIC error.
			This error might also combine with other error status bit.
	LenMis(LM):
			Length mismatch
	de_amsdu_err(DAMSDU Fail (DAF)):
			This bit indicates the DAMSDU is failed.
				Case #1: the sub-frame length is too short.
				Case #2: the sub-frame length is more than the remaining packet length.
			1 => the AMSDU sub-frame is abnormal.
			0 => the AMSDU sub-frame is correct and it can be De-AMSDU.
	ExMaxRxLen(Exceed MAX. RX length (EL)):
			If the RX packet exceeds the allowed maximum RX length, the packet payload
			will be drop, the packet header (ONLY when the header length is less than max RX length)
			and the RX status will be written to SYSRAM with this bit asserted.
	LlcMis(LLC-MIS):
			Indicate that the RX header translation can't find pre-defined
			LLC-SNAP pattern and the result packet type is 802.3.
			(this field is meaningful only when H field is 1)
				1'b0: the pre-defined LLC-SNAP pattern is matched
				1'b1: the pre-defined LLC-SNAP pattern is un-matched
	UdfVlanType(UDF_VLT): Undefined VLAN tagged Type
			1'b0: VLAN is followed by pre-defined LLC-SNAP
			1'b1: VLAN is followed by un-defined LLC-SNAP
	FragFrm(FRAG): Fragmented frame
	NullFrm(NULL): the frame is a NULL frame
	NonDataFrm(DATA): Non Data frame
	amsdu_sub_frm(A-MPDU Subframe (ASF)):
			0:  This is not an A-MPDU subframe
			1: This is an A-MPDU subframe
			If A-MPDU de-aggregation function was disabled, this field always is 0
	ampdu(AMP): AMPDU
			0: This is an MPDU frame
			1: This is an A-MPDU frame

DWORD 3 ==>
	rx_vector_seq(RX Vector Sequence No):
			The sequence NO. of the corresponding RX vector
			0: RX vector is invalid
			1~255: RX vector sequence number
	tsf_compare_loss(TSF Compare Loss (TCL)):
			In Ad-Hoc mode, indicates that the local TSF timer is less than the timestamp of Beacon or Probe Response.
			The compared TSF timer is selected by CR.
	pattern_drop_bit(DP): Pattern Filter Drop bit
			0: a wanted packet
			1: a drop packet
	cls(CLS): indicate the packet is a coalescing packet
			0: a normal frame
			1: a coalescing frame
	OFLD(OFLD): FW offload packet filter
			2'h0: not match
			2'h1: matched, the RX packet is an EAPOL packet
			2'h2: matched, the RX packet is an ARP or NS packet
			2'h3: matched, the RX packet is an TDLS packet
	magic_pkt(MGC): Wakeup magic packet
			0: not match
			1: matched, the RX packet is a wakeup magic packet
	wol(WOL): Wake-on-WLAN filter matching number
			{4'h0, 1'h0}: Not matched
			{4'h0, 1'h1}: match bitmap 0
			{4'h1, 1'h1}: match bitmap 1
			...
			{4'hF, 1'h1}: match bitmap 15
			Others: Reserved
	cls_bitmap(CLS_BITMAP): coalescing filter matching flag
			Bit[0]: indicate rule 0 matched
			Bit[1]: indicate rule 1 matched
			...
			Bit[9]: indicate rule 9 matched
	pf_mode(PF_MODE): The pattern filter mode
			0: White list
			1: Black list
	pf_sts(PF_STS): This is status report of packet filter
			00: filtering result is OK
				match vec = result (NO-fragment & NO unknown IPv6)
			01: fragment packet is detected
				match vec = result, ONLY when 1st  fragment is matched
							(the remaining fragments will be pasted with the same matched vector)
			10: non-fragment, but unknown IPv6 extenstion header is detected
				match vec = partial matched result, ONLY when header before unknown IPv6 ext.
							header are matched
			11: no filter enable
				match vec = invalid (all match result = non-match), ONLY when PF_EN = 0
*/
#endif /* RELEASE_EXCLUDE */

#define RMAC_RXD_0_PKT_TYPE_MASK		0xe0000000
#define RMAC_RX_PKT_TYPE_RX_TXS			0x00
#define RMAC_RX_PKT_TYPE_RX_TXRXV		0x01
#define RMAC_RX_PKT_TYPE_RX_NORMAL		0x02
#define RMAC_RX_PKT_TYPE_RX_DUP_RFB		0x03
#define RMAC_RX_PKT_TYPE_RX_TMR			0x04
#define RMAC_RX_PKT_TYPE_RETRIEVE		0x05
#define RMAC_RX_PKT_TYPE_RX_EVENT		0x07
#define RMAC_RX_PKT_TYPE(_x)			(((_x) & RMAC_RXD_0_PKT_TYPE_MASK) >> 29)
#define RMAC_RXD_0_PKT_RX_BYTE_COUNT_MASK 0x0000ffff
#define RMAC_RX_PKT_RX_BYTE_COUNT(_x)	(((_x) & RMAC_RXD_0_PKT_RX_BYTE_COUNT_MASK))

//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RMAC_RXD_0_TXS {
	UINT32 TxSPktType:3;
	UINT32 TxSRsv:8;
	UINT32 TxSCnt:5;
	UINT32 TxSRxByteCnt:16;
} RMAC_RXD_0_TXS, *PRMAC_RXD_0_TXS;
#else
typedef struct GNU_PACKED _RMAC_RXD_0_TXS {
	UINT32 TxSRxByteCnt:16;
	UINT32 TxSCnt:5;
	UINT32 TxSRsv:8;
	UINT32 TxSPktType:3;
} RMAC_RXD_0_TXS, *PRMAC_RXD_0_TXS;
#endif
//---UNIFIED_TXD_RXD


//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RMAC_RXD_0_TXRXV {
	UINT32 PktType:3;
	UINT32 Rsv:8;
	UINT32 RxvCnt:5;
	UINT32 RxByteCnt:16;
} RMAC_RXD_0_TXRXV, *PRMAC_RXD_0_TXRXV;
#else
typedef struct GNU_PACKED _RMAC_RXD_0_TXRXV {
	UINT32 RxByteCnt:16;
	UINT32 RxvCnt:5;
	UINT32 Rsv:8;
	UINT32 PktType:3;
} RMAC_RXD_0_TXRXV, *PRMAC_RXD_0_TXRXV;
#endif
//---UNIFIED_TXD_RXD


/******************************************************************************
	LMAC RxD
	
	RMAC_RXD_0
	RMAC_RXD_1
	RMAC_RXD_2
	RMAC_RXD_3
	
******************************************************************************/

/******************************************************************************
	RMAC_RXD_0
******************************************************************************/
//+++UNIFIED_TXD_RXD
#ifdef RELEASE_EXCLUDE
/*
Rx DWORD 0
 [15 : 0]     Rx Byte Count (byte)
 [22 : 16]    Ether-Type Offset (word)
 [23]         IP : IP checksum offload
 [24]         UT : UDP/TCP checksum offload
 [28 : 25]    Group VLD : RFB(Receive Frame Buffer) Group valid indicators
 [31 : 29]    Packet Type
*/
#endif /* RELEASE_EXCLUDE */
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RMAC_RXD_0 {
    UINT32   PktType:3;
    UINT32   RfbGroupVld:4;
    UINT32   UdpTcpChkSumOffload:1;
    UINT32   IpChkSumOffload:1;
    UINT32   EthTypeOffset:7;       
    UINT32   RxByteCnt:16;                                          
} RMAC_RXD_0, *PRMAC_RXD_0;
#else
typedef struct GNU_PACKED _RMAC_RXD_0 {
    UINT32   RxByteCnt:16;                             
    UINT32   EthTypeOffset:7;                      
    UINT32   IpChkSumOffload:1;                    
    UINT32   UdpTcpChkSumOffload:1;                
    UINT32   RfbGroupVld:4;              
    UINT32   PktType:3;                           
} RMAC_RXD_0, *PRMAC_RXD_0;
#endif
//---UNIFIED_TXD_RXD

/******************************************************************************
	RMAC_RXD_1
******************************************************************************/
//+++UNIFIED_TXD_RXD
#ifdef RELEASE_EXCLUDE
/*
Rx DWORD 1
 [0]          HTC : Four bytes HTC are padded after MAC header
 [1]          UC2ME : Unicast to Me
 [2]          MC : Multicast
 [3]          BC : Broadcast
 [4]          BBM : Beacon with Broadcast/Multicast
 [5]          BU : Beacon with Unicast
 [7  : 6]     KID : Key ID
 [15 : 8]     CH Frequency : Channel Frequency
 [21 : 16]    MAC Header Length
 [22]         HO : Header Offset
 [23]         H : Header Translation --> 0 : original 802.11 header ; 1 : received frame is performed by HW RX header translation(for Linux)
 [25 : 24]    PF : Payload Format
 [31 : 26]    RxDBssidIdx : indicate the matched BSSID
*/
#endif /* RELEASE_EXCLUDE */
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RMAC_RXD_1 {
    UINT32   RxDBssidIdx:6; 
    UINT32   PayloadFmt:2; 
    UINT32   HdrTranslation:1; 
    UINT32   HdrOffset:1;  
    UINT32   MacHdrLen:6; 
    UINT32   ChFreq:8;  
    UINT32   KeyId:2;
    UINT32   BcnWithUCast:1;
    UINT32   BcnWithBMcst:1;
    UINT32   Bcast:1;
    UINT32   Mcast:1;
    UINT32   UcastToMe:1;
    UINT32   HTC:1;                                                                                        
} RMAC_RXD_1, *PRMAC_RXD_1;
#else
typedef struct GNU_PACKED _RMAC_RXD_1 {
    UINT32   HTC:1;                                  
    UINT32   UcastToMe:1;                          
    UINT32   Mcast:1;                                
    UINT32   Bcast:1;                                
    UINT32   BcnWithBMcst:1;         
    UINT32   BcnWithUCast:1;                    
    UINT32   KeyId:2;                                
    UINT32   ChFreq:8;                     
    UINT32   MacHdrLen:6;                      
    UINT32   HdrOffset:1;                         
    UINT32   HdrTranslation:1;                        
    UINT32   PayloadFmt:2;                        
    UINT32   RxDBssidIdx:6;                                                        
} RMAC_RXD_1, *PRMAC_RXD_1;
#endif
//---UNIFIED_TXD_RXD

/******************************************************************************
	RMAC_RXD_2
******************************************************************************/
//+++UNIFIED_TXD_RXD
#ifdef RELEASE_EXCLUDE
/*
//Rx DWORD 2
// [7  : 0] WLAN index
// [11 : 8] TID : copy from the TID field in QoS control field, if the Rx packet is an QoS Data frame. It will be 0, if the Rx packet is not an QoS Data frame.
// [15 : 12]SEC_mode : Security engine operation mode.
// [16]     SWBIT : Software assigned bit in WTBL(WLan Table)
// [17]     FC : FCS Error. If this bit is set, the received frame contains FCS error.
// [18]     CM : Cipher Mismatch.
// [19]     CLM : Cipher Length Mismatch.
// [20]     I : ICV Error
// [21]     T : TKIPMIC Error
// [22]     LM : Length Mismatch
// [23]     DAF : DAMSDU Fail. --> 0 : DAMSDU is not failed ; 1 : DAMSDU is failed
// [24]     EL : Exceed maxium Rx Length
// [25]     LLC-MIS : LLC Mismatched. --> 0 : the pre-defined LLC-SNAP pattern is matched. ; 1 : the pre-defined LLC-SNAP pattern is un-matched.
// [26]     UdfVlanType : Undefined VLAN Tagged Type. --> 0 : VLAN is followed by pre-defined LLC-SNAP. ; 1 : VLAN is followed by un-defined LLC-SNAP.
// [27]     FRAG : Fragmented Frame
// [28]     NULL : NULL Frame
// [29]     NonDataFrm : Non Data Frame
// [30]     NASF : A-MPDU Subframe. --> 0 : this is an A-MPDU subframe ; 1 : this is not an A-MPDU subframe. If A-MPDU de-aggregation function was disabled, this field always be 0.
// [31]     NAMP : A-MPDU Frame. --> 0 : this is an A-MPDU frame ; 1 : this is a MPDU frame.
*/
#endif /* RELEASE_EXCLUDE */
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RMAC_RXD_2 {
    UINT32   NonAmpduFrm:1; 
    UINT32   NonAmpduSfrm:1;
    UINT32   NonDataFrm:1;  
    UINT32   NullFrm:1;  
    UINT32   FragFrm:1;  
    UINT32   UdfVlanType:1;  
    UINT32   LlcMis:1; 
    UINT32   ExMaxRxLen:1;  
    UINT32   DeAmsduFail:1;  
    UINT32   LenMis:1;   
    UINT32   TkipMicErr:1;  
    UINT32   IcvErr:1;   
    UINT32   CipherLenMis:1;  
    UINT32   CipherMis:1;
    UINT32   FcsErr:1;    
    UINT32   SwBit:1;  
    UINT32   SecMode:4;
    UINT32   RxDTid:4;   
    UINT32   RxDWlanIdx:8;                                                                                                
} RMAC_RXD_2, *PRMAC_RXD_2;
#else
typedef struct GNU_PACKED _RMAC_RXD_2 {
    UINT32   RxDWlanIdx:8;                            
    UINT32   RxDTid:4;                                  
    UINT32   SecMode:4;                             
    UINT32   SwBit:1;                                
    UINT32   FcsErr:1;                             
    UINT32   CipherMis:1;                       
    UINT32   CipherLenMis:1;                 
    UINT32   IcvErr:1;                             
    UINT32   TkipMicErr:1;                         
    UINT32   LenMis:1;                       
    UINT32   DeAmsduFail:1;                               
    UINT32   ExMaxRxLen:1;                    
    UINT32   LlcMis:1;                        
    UINT32   UdfVlanType:1;          
    UINT32   FragFrm:1;                      
    UINT32   NullFrm:1;                            
    UINT32   NonDataFrm:1;                            
    UINT32   NonAmpduSfrm:1;                 
    UINT32   NonAmpduFrm:1;                                                                             
} RMAC_RXD_2, *PRMAC_RXD_2;
#endif
//---UNIFIED_TXD_RXD

/*//do unify
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED rmac_rxd_3_normal {
	UINT32 pf_sts:2;
	UINT32 pf_mode:1;
	UINT32 cls_bitmap:10;
	UINT32 wol:5;
	UINT32 magic_pkt:1;
	UINT32 OFLD:2;
	UINT32 cls:1;
	UINT32 pattern_drop_bit:1;
	UINT32 tsf_compare_loss:1;
	UINT32 rx_vector_seq:8;
} RMAC_RXD_3_NORMAL;
#else
typedef struct GNU_PACKED rmac_rxd_3_normal {
	UINT32 rx_vector_seq:8;
	UINT32 tsf_compare_loss:1;
	UINT32 pattern_drop_bit:1;
	UINT32 cls:1;
	UINT32 OFLD:2;
	UINT32 magic_pkt:1;
	UINT32 wol:5;
	UINT32 cls_bitmap:10;
	UINT32 pf_mode:1;
	UINT32 pf_sts:2;
} RMAC_RXD_3_NORMAL;
#endif
*/


#ifdef RELEASE_EXCLUDE
/*
	RMAC_RXD_0_TMR[15:0]
		RX Byte Count: the total byte count of TMR packet
	RMAC_RXD_0_TMR[17:16]
		Reserved
	RMAC_RXD_0_TMR[18]
		NC: Noisy channel. When NC = 1, means the measurement report is inaccuracy.
	RMAC_RXD_0_TMR[19]
		TMF: Timing measurement fail
	RMAC_RXD_0_TMR[23:20]
		Sub-type: (valid when IR = 1)
	RMAC_RXD_0_TMR[25:24]
		Type: (valid when IR = 1)
	RMAC_RXD_0_TMR[26]
		ToA_VLD: indicate the ToA is vaild
	RMAC_RXD_0_TMR[27]
		ToD_VLD: indicate the ToD is vaild
	RMAC_RXD_0_TMR[28]
		IR: Initiator or Responder
		1'b0: Initiator
		1'b1: Responder
	RMAC_RXD_0_TMR[31:29]
		PktType: This field is always 3'h4 (Timing Measurement report)
*/
#endif /* RELEASE_EXCLUDE */
//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RMAC_RXD_0_TMR {
	UINT32 PktType:3;
	UINT32 IR:1;
	UINT32 TodVld:1;
	UINT32 ToaVld:1;
	UINT32 Type:2;
	UINT32 SubType:4;
	UINT32 Tmf:1;
	UINT32 Nc:1;
	UINT32 Rsv16:2;
	UINT32 RxByteCnt:16;
} RMAC_RXD_0_TMR, *PRMAC_RXD_0_TMR;
#else
typedef struct GNU_PACKED _RMAC_RXD_0_TMR {
	UINT32 RxByteCnt:16;
	UINT32 Rsv16:2;
	UINT32 Nc:1;
	UINT32 Tmf:1;
	UINT32 SubType:4;
	UINT32 Type:2;
	UINT32 ToaVld:1;
	UINT32 TodVld:1;
	UINT32 IR:1;
	UINT32 PktType:3;
} RMAC_RXD_0_TMR, *PRMAC_RXD_0_TMR;
#endif
//---UNIFIED_TXD_RXD

typedef union GNU_PACKED _RMAC_RXD_0_UNION {
	struct _RMAC_RXD_0_TXS TxSD0;
	struct _RMAC_RXD_0 RxD0;
	struct _RMAC_RXD_0_TXRXV RxvD0;
	struct _RMAC_RXD_0_TMR TmrD0;
	UINT32 word;
} RMAC_RXD_0_UNION, *PRMAC_RXD_0_UNION;

#ifdef RELEASE_EXCLUDE
/*
    RxVSeq[7 : 0]:
	RX Vector Sequence No: The sequence NO. of the corresponding RX vector
	0: RX vector is invalid
	1~255: RX vector sequence number
    Tcl[8]:
	TSF Compare Loss (TCL): In Ad-Hoc mode, if this bit is set to 1, it indicates that the local TSF timer is less than the timestamp of Beacon or Probe Response.
	The compared TSF timer is selected by CR.
    Drop[9]:
	DP: Pattern Filter Drop bit
	0: a wanted packet
	1: a drop packet
    ClsPkt[10]:
	CLS: indicate the packet is a coalescing packet
	0: a normal frame
	1: a coalescing frame
    OfldPf[12 : 11]:
	OFLD: FW offload packet filter
	2¡¦h0: not match
	2¡¦h1: matched, the RX packet is an EAPOL packet
	2¡¦h2: matched, the RX packet is an ARP or NS packet
	2¡¦h3: matched, the RX packet is an TDLS packet
    MgcPkt[13]:
	MGC: Wakeup magic packet
	0: not match
	1: matched, the RX packet is a wakeup magic packet
    WoWlan[18 : 14]:
	WOL: Wake-on-WLAN filter matching number
	{4¡¦h0, 1¡¦h0}: Not matched
	{4¡¦h0, 1¡¦h1}: match bitmap 0
	{4¡¦h1, 1¡¦h1}: match bitmap 1
	¡K
	{4¡¦hF, 1¡¦h1}: match bitmap 15
	Others: Reserved
    ClsBitMap[28 : 19]:
	CLS_BITMAP: coalescing filter matching flag
	Bit[0]: indicate rule 0 matched
	Bit[1]: indicate rule 1 matched
	¡K
	Bit[9]: indicate rule 9 matched
    PfMode[29]:
	PF_MODE: The pattern filter mode
	1: White list
	0: Black list
    PfStatus[31 : 30]:
	PF_STS: This is status report of packet filter
	00: filtering result is OK
„»	match vec = result (NO-fragment & NO unknown IPv6)
	01: fragment packet is detected
„»	match vec = result, ONLY when 1st  fragment is matched (the remaining fragments will be pasted with the same matched vector)
	10: non-fragment, but unknown IPv6 extenstion header is detected
„»	match vec = partial matched result, ONLY when header before unknown IPv6 ext. header are matched
	11: no filter enable
	match vec = invalid (all match result = non-match), ONLY when PF_EN = 0

*/
#endif /* RELEASE_EXCLUDE */
//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RMAC_RXD_3 {
    UINT32   PfStatus:2;
    UINT32   PfMode:1;
    UINT32   ClsBitMap:10;
    UINT32   WoWlan:5;
    UINT32   MgcPkt:1;  
    UINT32   OfldPf:2;
    UINT32   ClsPkt:1;
    UINT32   Drop:1;
    UINT32   Tcl:1;
    UINT32   RxVSeq:8;
} RMAC_RXD_3, *PRMAC_RXD_3;
#else
typedef struct GNU_PACKED _RMAC_RXD_3 {
    UINT32   RxVSeq:8;
    UINT32   Tcl:1;
    UINT32   Drop:1;
    UINT32   ClsPkt:1;
    UINT32   OfldPf:2;
    UINT32   MgcPkt:1;            
    UINT32   WoWlan:5;
    UINT32   ClsBitMap:10;
    UINT32   PfMode:1;
    UINT32   PfStatus:2;
} RMAC_RXD_3, *PRMAC_RXD_3;
#endif
//---UNIFIED_TXD_RXD


//+++UNIFIED_TXD_RXD
/*
	Rx Normal Data frame Format
*/
typedef struct GNU_PACKED _RXD_BASE_STRUCT{
	/* DWORD 0 */
	struct _RMAC_RXD_0 RxD0;
	/* DWORD 1 */
	struct _RMAC_RXD_1 RxD1;
	/* DWORD 2 */
	struct _RMAC_RXD_2 RxD2;
	/* DWORD 3 */
	struct _RMAC_RXD_3 RxD3;
	//struct rmac_rxd_3_normal RxD3;//do unify
}RXD_BASE_STRUCT, *PRXD_BASE_STRUCT;
//---UNIFIED_TXD_RXD


#define RXS_GROUP1 (1 << 0)
#define RXS_GROUP2 (1 << 1)
#define RXS_GROUP3 (1 << 2)
#define RXS_GROUP4 (1 << 3)

#define RMAC_INFO_BASE_SIZE 	16
#define RMAC_INFO_GRP_1_SIZE    16
#define RMAC_INFO_GRP_2_SIZE    8
#define RMAC_INFO_GRP_3_SIZE    24
#define RMAC_INFO_GRP_4_SIZE    16


#ifdef RELEASE_EXCLUDE
/*
	DW4~5
	TA[47:0] (present only when H = 1)
        FrmCtrl[15:0]Frame Control Field (present only when H = 1)
	
*/
#endif /* RELEASE_EXCLUDE */
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RMAC_RXD_4 {

    // DW4
    UINT32   TA0:16;
    UINT32   FrmCtrl:16;
} RMAC_RXD_4, *PRMAC_RXD_4;
#else
typedef struct GNU_PACKED _RMAC_RXD_4 {

    // DW4
    UINT32   FrmCtrl:16;
    UINT32   TA0:16;
} RMAC_RXD_4, *PRMAC_RXD_4;
#endif


#ifdef RELEASE_EXCLUDE
/*
	TA1[31 : 0]: TA (present only when H = 1)
*/
#endif /* RELEASE_EXCLUDE */

typedef struct GNU_PACKED _RMAC_RXD_5 {
    UINT32   TA1;
} RMAC_RXD_5, *PRMAC_RXD_5;


#ifdef RELEASE_EXCLUDE
/*
    Frag[3:0]: Fragment Number [] (present only when H = 1)
    Seq[15:4]: Sequence Number (present only when H = 1)
    Qos[31:16]: QoS Control Field (present only when H = 1)    
*/
#endif /* RELEASE_EXCLUDE */
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RMAC_RXD_6 {
    UINT32   QoS:16;    
    UINT32   Seq:12;
    UINT32   Frag:4;
} RMAC_RXD_6, *PRMAC_RXD_6;
#else
typedef struct GNU_PACKED _RMAC_RXD_6 {
    UINT32   Frag:4;
    UINT32   Seq:12;
    UINT32   QoS:16;
} RMAC_RXD_6, *PRMAC_RXD_6;
#endif


#ifdef RELEASE_EXCLUDE
/*
	HT HT Control Field (present only when H = 1)
*/
#endif /* RELEASE_EXCLUDE */

typedef struct GNU_PACKED _RMAC_RXD_7 {
    UINT32   HTCtrl;
} RMAC_RXD_7, *PRMAC_RXD_7;


#ifdef RELEASE_EXCLUDE
/*
	Present when RXD_BASE_STRUCT.HdrTranslation == 1

DWORD 4 ==>
	frm_ctrl(Frame Control Field):
	ta_low(TA[15:0]):

DWORD 5 ==>
	ta_high(TA[47:16]):

DWORD 6 ==>
	frag_num(Fragment Number):
	seq_num(Sequence Number):
	qos_ctrl(QoS Control Field):

DWORD 7 ==>
	htc_ctrl(HT Control Field):
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RXD_GRP4_STRUCT{
	/* DWORD 4 */
	struct _RMAC_RXD_4 rxd_4;

	/* DWORD 5 */	
	struct _RMAC_RXD_5 rxd_5;

	/* DWORD 6 */
	struct _RMAC_RXD_6 rxd_6;

	/* DWORD 7 */
	struct _RMAC_RXD_7 rxd_7;
} RXD_GRP4_STRUCT;


#ifdef RELEASE_EXCLUDE
/*
	for DW8~11
        RscPn0[47:0]: TKIP_RSC[47:0] / CCMP_PN[47:0] / GCMP_PN[47:0] 
	WPIPn0[127:0]: WPI_PN
*/
#endif /* RELEASE_EXCLUDE */
typedef union GNU_PACKED _RMAC_RXD_8 {
    UINT32   RscPn0;
    UINT32   WPIPn0;
    UINT32   Word;
} RMAC_RXD_8, *PRMAC_RXD_8;


#ifdef RT_BIG_ENDIAN
typedef union GNU_PACKED _RMAC_RXD_9 {
    struct {
        UINT32 Rsv:16;
        UINT32 PN:16;
    }RscPn1;
    UINT32   WPIPn1;
    UINT32 Word;
} RMAC_RXD_9, *PRMAC_RXD_9;
#else
typedef union GNU_PACKED _RMAC_RXD_9 {
    struct {
        UINT32 PN:16;
        UINT32 Rsv:16;
    }RscPn1;
    UINT32   WPIPn1;
    UINT32 Word;
} RMAC_RXD_9, *PRMAC_RXD_9;
#endif


typedef struct GNU_PACKED _RMAC_RXD_10 {
    UINT32   WpiPn2;
} RMAC_RXD_10, *PRMAC_RXD_10;


typedef struct GNU_PACKED _RMAC_RXD_11 {
     UINT32   WpiPn3;
} RMAC_RXD_11, *PRMAC_RXD_11;


#ifdef RELEASE_EXCLUDE
/*
	Present when CR setting HIF_CLIENT.RXINF.RXGV[2:0]

DWORD 8 ==>
	sec_pn_32: TKIP_RSC[31:0] / CCMP_PN[31:0] / GCMP_PN[31:0] / WPI_PN[31:0]

DWORD 9 ==>
	sec_pn_48: TKIP_RSC[47:32] / CCMP_PN[47:32] / GCMP_PN[47:32] / WPI_PN[47:32]
	wpi_pn_64: WPI_PN[63:48]

DWORD 10 ==>
	wpi_pn_96: WPI_PN[95:64]

DWORD 11 ==>
	wpi_pn_128: WPI_PN[127:96]
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RXD_GRP1_STRUCT{
	/* DWORD 8 */ 
	union  _RMAC_RXD_8 rxd_8 ;

	/* DWORD 9 */
	union  _RMAC_RXD_9 rxd_9;	
	
	/* DWORD 10 */ 
	struct  _RMAC_RXD_10 rxd_10;

	/* DWORD 11 */
	struct  _RMAC_RXD_11 rxd_11;
} RXD_GRP1_STRUCT;


#ifdef RELEASE_EXCLUDE
/*
    Timestamp: Frame reception timestamp (low 32-bits) that indicates the value of the local TSF timer value at the time the first bit of the MAC header in the received frame (PPDU unit) arriving at the MAC.
    For A-MPDU within in the same PPDU, the Timestamp will be the same for each de-aggregated MPDU.
    If RX packet matches Own BSSID, the local TSF is the BSSID related TSF timer.
    If RX packet un-matches Own BSSID, the local TSF is assigned by CR.
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RMAC_RXD_12 {
    UINT32   Timestamp;
} RMAC_RXD_12, *PRMAC_RXD_12;


#ifdef RELEASE_EXCLUDE
/*
	Crc: CRC32 value of this MPDU	
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RMAC_RXD_13 {
    UINT32   Crc;
} RMAC_RXD_13, *PRMAC_RXD_13;


#ifdef RELEASE_EXCLUDE
/*
	Present when CR setting HIF_CLIENT.RXINF.RXGV[2:0]

DWORD 12 ==>
	timestamp(Timestamp):
			Frame reception timestamp (low 32-bits) that indicates the value of
			the local TSF timer value at the time the first bit of the MAC header
			in the received frame (PPDU unit) arriving at the MAC.

			For A-MPDU within in the same PPDU, the Timestamp will be the
			same for each de-aggregated MPDU

			If RX packet matches Own BSSID, the local TSF is the BSSID related TSF timer.
			If RX packet un-matches Own BSSID, the local TSF is assigned by CR

DWORD 13 ==>
	crc(CRC): CRC32 value of this MPDU
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RXD_GRP2_STRUCT{
	/* DWORD 12 */
	struct _RMAC_RXD_12 rxd_12;

	/* DWORD 13 */
	struct _RMAC_RXD_13 rxd_13;
} RXD_GRP2_STRUCT;


#ifdef RELEASE_EXCLUDE
/*
	RX Vector Group 1 1th cycle
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RMAC_RXD_14 {
    UINT32   RxVCycle1;
} RMAC_RXD_14, *PRMAC_RXD_14;


#ifdef RELEASE_EXCLUDE
/*
	RX Vector Group 1 2th cycle	
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RMAC_RXD_15 {
    UINT32   RxVCycle2;
} RMAC_RXD_15, *PRMAC_RXD_15;


#ifdef RELEASE_EXCLUDE
/*
	RX Vector Group 1 3th cycle
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RMAC_RXD_16 {
    UINT32   RxVCycle3;
} RMAC_RXD_16, *PRMAC_RXD_16;


#ifdef RELEASE_EXCLUDE
/*
	RX Vector Group 1 4th cycle
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RMAC_RXD_17 {
    UINT32   RxVCycle4;
} RMAC_RXD_17, *PRMAC_RXD_17;


#ifdef RELEASE_EXCLUDE
/*
	RX Vector Group 1 5th cycle
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RMAC_RXD_18 {
    UINT32   RxVCycle5;
} RMAC_RXD_18, *PRMAC_RXD_18;


#ifdef RELEASE_EXCLUDE
/*
	RX Vector Group 1 6th cycle	
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RMAC_RXD_19 {
    UINT32   RxVCycle6;
} RMAC_RXD_19, *PRMAC_RXD_19;


#ifdef RELEASE_EXCLUDE
/*
	Present when CR setting HIF_CLIENT.RXINF.RXGV[2:0]

DWORD 14 ==>
	rxv_1: RX Vector Group 1 1st cycle, The value should refer to "MT6620 WiFi BB MAC interface" document.
DWORD 15 ==>
	rxv_2: RX Vector Group 1 2nd cycle
DWORD 16 ==>
	rxv_3: RX Vector Group 1 3rd cycle
DWORD 17 ==>
	rxv_4: RX Vector Group 1 4th cycle
DWORD 18 ==>
	rxv_5: RX Vector Group 1 5th cycle
DWORD 19 ==>
	rxv_6: RX Vector Group 1 6th cycle
*/
#endif /* RELEASE_EXCLUDE */
typedef struct GNU_PACKED _RXD_GRP3_STRUCT{
	/* DWORD 13 */
	struct _RMAC_RXD_13 rxd_13;

	/* DWORD 14 */
	struct _RMAC_RXD_14 rxd_14;

	/* DWORD 15 */
	struct _RMAC_RXD_15 rxd_15;

	/* DWORD 16 */
	struct _RMAC_RXD_16 rxd_16;

	/* DWORD 17 */
	struct _RMAC_RXD_17 rxd_17;

	/* DWORD 18 */
	struct _RMAC_RXD_18 rxd_18;

	/* DWORD 19 */
	struct _RMAC_RXD_19 rxd_19;
	
}RXD_GRP3_STRUCT;


#ifdef RELEASE_EXCLUDE
/*
	ChksumStatus(Checksum Status (CS_Status)):
		Indicates the checking result of packet checksum.
			Bit3: This bit is used for UDP checksum result indication and it is
				valid only when HWFCR. RX_UDP_CS_OFLD_EN is asserted.
				0: The UDP checksum result is correct or it doesn't need to check.
						If UDP checksum value is 0, then this field will be 0.
				1: The UDP checksum result is incorrect.
			Bit2: This bit is used for TCP checksum result indication and it is
				valid only when HWFCR. RX_TCP_CS_OFLD_EN is asserted.
				0: The TCP checksum is correct or it doesn't need to check.
					If TCP checksum value is 0, then this field will be 0.
				1: The TCP checksum is incorrect.
			Bit1: Reserved.
			Bit0: This bit is used for IP checksum result indication and it is
				valid only when HWFCR. RX_IPV4_CS_OFLD_EN is asserted.
				0: The IP checksum is correct.
				1: The IP checksum is incorrect.

	ChksumType(Checksum Type (CS Type)): Indicates the packet type.
			Bit7: this bit indicates this data structure is an UDP packet format
				=>valid only when HWFCR. RX_UDP_CS_OFLD_EN is asserted.
			Bit6: this bit indicates this data structure is a TCP packet format
				=>valid only when HWFCR. RX_TCP_CS_OFLD_EN is asserted.
			Bit5: this bit indicates this data structure is an IPv6 packet format
				=>valid only when HWFCR. RX_IPV6_CS_OFLD_EN is asserted.
			Bit4: this bit indicates this data structure is an IPv4 packet format
				=>valid only when HWFCR. RX_IPV4_CS_OFLD_EN is asserted.

	IpLenMismatch(IP Datagram Length Mismatch (C)):
			indicates that the received IP length does not match with the received
			frame length. Refer chapter 8.4 "Rule for checksum offload in TX/RX"
			for details.

			=> This field only valid when at least one bit of HWFCR.[7:5] is asserted

	IpFrag(IP Fragmentation Packet (F)):
			it indicates that the received frame is an IP fragmented packet.

			=> This field is only valid when at least one bit of HWFCR.[7:5] is asserted

	UnknownNextHdr(Unknown Next Header (UN)):
			If the "next header" field of IPV6 header is not equal to
			TCP(NextHeader=6), UDP (NextHeader=17), or NO_NEXT_HEADER (NextHeader=59),
			this bit will be set to 1.

			=> This field is only valid when HWFCR. RX_IPV6_CS_OFLD_EN is asserted.

	Rsv11to31: Reserved bits
*/
#endif /* RELEASE_EXCLUDE */

#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RX_CSO_STRUCT {
	UINT32 Rsv11to31:21;
	UINT32 UnknownNextHdr:1;
	UINT32 IpFrag:1;
	UINT32 IpLenMismatch:1;
	UINT32 ChksumType:4;
	UINT32 ChksumStatus:4;
} RX_CSO_STRUCT;
#else
typedef struct GNU_PACKED _RX_CSO_STRUCT {
	UINT32 ChksumStatus:4;
	UINT32 ChksumType:4;
	UINT32 IpLenMismatch:1;
	UINT32 IpFrag:1;
	UINT32 UnknownNextHdr:1;
	UINT32 Rsv11to31:21;
} RX_CSO_STRUCT;
#endif


/******************************************************************************
	LMAC TxS Structure
	
	TXS_D_0
	TXS_D_1
	TXS_D_2
	TXS_D_3
	TXS_D_4
******************************************************************************/

/******************************************************************************
	TXS_D_0
******************************************************************************/
//+++UNIFIED_TXD_RXD
/* TX Status Report Format */
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _TXS_D_0 {
	UINT32 AntId:6;
	UINT32 Tid:3;
	UINT32 BAFail:1;
	UINT32 PSBit:1;
	UINT32 TxOp:1;
	UINT32 BE:1;
	UINT32 LE:1;
	UINT32 RE:1;
	UINT32 ME:1;
	UINT32 TxS2H:1;
	UINT32 TxS2M:1;
	UINT32 TxSFmt:1;
	UINT32 TxS_FR:1;
	UINT32 TxRate:12;
} TXS_D_0, *PTXS_D_0;
#else
typedef struct GNU_PACKED _TXS_D_0 {
	UINT32 TxRate:12;
	UINT32 TxS_FR:1;
	UINT32 TxSFmt:1;
	UINT32 TxS2M:1;
	UINT32 TxS2H:1;
	UINT32 ME:1;
	UINT32 RE:1;
	UINT32 LE:1;
	UINT32 BE:1;
	UINT32 TxOp:1;
	UINT32 PSBit:1;
	UINT32 BAFail:1;
	UINT32 Tid:3;
	UINT32 AntId:6;
} TXS_D_0, *PTXS_D_0;
#endif
//---UNIFIED_TXD_RXD


/******************************************************************************
	TXS_D_1
******************************************************************************/
//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef union GNU_PACKED _TXS_D_1 {
	struct {
		UINT32 Rsv:8;
		UINT32 Noise2:8;
		UINT32 Noise1:8;
		UINT32 Noise0:8;
	} FieldNoise;

	UINT32 TimeStamp;

	UINT32 Word;
} TXS_D_1, *PTXS_D_1;
#else
typedef union GNU_PACKED _TXS_D_1 {
	struct {
		UINT32 Noise0:8;
		UINT32 Noise1:8;
		UINT32 Noise2:8;
		UINT32 Rsv:8;
	} FieldNoise;

	UINT32 TimeStamp;

	UINT32 Word;
} TXS_D_1, *PTXS_D_1;
#endif
//---UNIFIED_TXD_RXD


/******************************************************************************
	TXS_D_2
******************************************************************************/
#ifdef RT_BIG_ENDIAN
typedef union GNU_PACKED _TXS_D_2 {
	struct {
		UINT32 TxPwrdBm:7;
		UINT32 FrontTime:25;
	} field_ft;

	struct {
		UINT32 TxPwrdBm:7;
		UINT32 Rsv:1;
		UINT32 RCPI2:8;
		UINT32 RCPI1:8;
		UINT32 RCPI0:8;
	} field_rcpi;

	UINT32 Word;
} TXS_D_2, *PTXS_D_2;
#else
typedef union GNU_PACKED _TXS_D_2 {
	struct {
		UINT32 FrontTime:25;
		UINT32 TxPwrdBm:7;
	} field_ft;

	struct {
		UINT32 RCPI0:8;
		UINT32 RCPI1:8;
		UINT32 RCPI2:8;
		UINT32 Rsv:1;
		UINT32 TxPwrdBm:7;
	} field_rcpi;

	UINT32 Word;
} TXS_D_2, *PTXS_D_2;
#endif


/******************************************************************************
	TXS_D_3
******************************************************************************/
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _TXS_D_3 {
	UINT32 TxS_WlanIdx:8;
	UINT32 RXV_SN:8;
	UINT32 TxDelay:16;
} TXS_D_3, *PTXS_D_3;
#else
typedef struct GNU_PACKED _TXS_D_3 {
	UINT32 TxDelay:16;
	UINT32 RXV_SN:8;
	UINT32 TxS_WlanIdx:8;
} TXS_D_3, *PTXS_D_3;
#endif


/******************************************************************************
	TXS_D_4
******************************************************************************/
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _TXS_D_4 {
	UINT32 TxS_LastTxMcsIdx:3;
	UINT32 TxS_MpduTxCnt:5;
	UINT32 TxS_AMPDU:1;
	UINT32 TxS_FianlMPDU:1;
	UINT32 TxS_Pid:8;
	UINT32 TxS_TxBW:2;
	UINT32 TxS_SN_TSSI:12;
} TXS_D_4, *PTXS_D_4;
#else
typedef struct GNU_PACKED _TXS_D_4 {
	UINT32 TxS_SN_TSSI:12;
	UINT32 TxS_TxBW:2;
	UINT32 TxS_Pid:8;
	UINT32 TxS_FianlMPDU:1;
	UINT32 TxS_AMPDU:1;
	UINT32 TxS_MpduTxCnt:5;
	UINT32 TxS_LastTxMcsIdx:3;
} TXS_D_4, *PTXS_D_4;
#endif


/******************************************************************************
	TXS_STRUC
******************************************************************************/
typedef struct GNU_PACKED _TXS_STRUC{
	TXS_D_0 TxSD0;
	TXS_D_1 TxSD1;
	TXS_D_2 TxSD2;
	TXS_D_3 TxSD3;
	TXS_D_4 TxSD4;
} TXS_STRUC;


/******************************************************************************
	TXS Frame Format
******************************************************************************/
typedef struct GNU_PACKED _TXS_FRM_STRUC{
	struct _RMAC_RXD_0_TXS TxSD0;
	TXS_STRUC txs_info[0];
} TXS_FRM_STRUC;



#ifdef RELEASE_EXCLUDE
/*
	DW0[31:0], DW1[15:0]
		TA[47:0]
	DW1
		[22:16]: RXV_SN: RX Vector sequence number
				8'h0: No VEC
				8'h1~8'hFF: VEC sequence number
		[23] TR: indicates the RX vector is for TX status or RX frame
				1'h0: RX VEC
				1'h1: TX Status VEC
	DW2
		RX Vector1 1st Cycle
	DW3
		RX Vector1 2nd Cycle
	DW4
		RX Vector1 3rd Cycle
	DW5
		RX Vector1 4th Cycle
	DW6 
		RX Vector1 5th Cycle
	DW7
		RX Vector1 6th Cycle
	DW8
		RX Vector2 DW0
	DW9 
		RX Vector2 DW1
	DW10
		RX Vector2 DW2
*/
#endif /* RELEASE_EXCLUDE */

#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RX_VECTOR1_1ST_CYCLE {
	UINT32 VHTA1_B5_B4:2;
	UINT32 VHTA2_B8_B1:8;
	UINT32 HtNoSound:1;
	UINT32 HtSmooth:1;
	UINT32 HtShortGi:1;
	UINT32 HtAggregation:1;
	UINT32 VHTA1_B22:1;
	UINT32 FrMode:2;
	UINT32 TxMode:3;
	UINT32 HtExtltf:2;
	UINT32 HtAdCode:1;
	UINT32 HtStbc:2;
	UINT32 TxRate:7;
} RX_VECTOR1_1ST_CYCLE;
#else
typedef struct GNU_PACKED _RX_VECTOR1_1ST_CYCLE {
	UINT32 TxRate:7;
	UINT32 HtStbc:2;
	UINT32 HtAdCode:1;
	UINT32 HtExtltf:2;
	UINT32 TxMode:3;
	UINT32 FrMode:2;
	UINT32 VHTA1_B22:1;
	UINT32 HtAggregation:1;
	UINT32 HtShortGi:1;
	UINT32 HtSmooth:1;
	UINT32 HtNoSound:1;
	UINT32 VHTA2_B8_B1:8;
	UINT32 VHTA1_B5_B4:2;
} RX_VECTOR1_1ST_CYCLE;
#endif


#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RX_VECTOR1_2ND_CYCLE {
	UINT32 VHTA1_B16_B6:11;
	UINT32 Length:21;
} RX_VECTOR1_2ND_CYCLE;
#else
typedef struct GNU_PACKED _RX_VECTOR1_2ND_CYCLE {
	UINT32 Length:21;
	UINT32 VHTA1_B16_B6:11;
} RX_VECTOR1_2ND_CYCLE;
#endif


#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RX_VECTOR1_3TH_CYCLE {
	UINT32 Fagc1CalGain:3;
	UINT32 Fgac1EqCal:1;
	UINT32 Rcpi1:8;
	UINT32 Fagc0CalGain:3;
	UINT32 Fagc0EqCal:1;
	UINT32 Rcpi0:8;
	UINT32 SelAnt:1;
	UINT32 ACI_DETx:1;
	UINT32 OFDMFreqTransDet:1;
	UINT32 VHTA1_B21_B17:5;
} RX_VECTOR1_3TH_CYCLE;
#else
typedef struct GNU_PACKED _RX_VECTOR1_3TH_CYCLE {
	UINT32 VHTA1_B21_B17:5;
	UINT32 OFDMFreqTransDet:1;
	UINT32 ACI_DETx:1;
	UINT32 SelAnt:1;
	UINT32 Rcpi0:8;
	UINT32 Fagc0EqCal:1;
	UINT32 Fgac0CalGain:3;
	UINT32 Rcpi1:8;
	UINT32 Fagc1EqCal:1;
	UINT32 Fgac1CalGain:3;
} RX_VECTOR1_3TH_CYCLE;
#endif


#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RX_VECTOR1_4TH_CYCLE {
	UINT32 FgacCalGain:3;
	UINT32 Fagc2EqCal:1;
	UINT32 IBRssi1:8;
	UINT32 FagcLpfGainx:4;
	UINT32 WBRssix:8;
	UINT32 IBRssi0:8;
} RX_VECTOR1_4TH_CYCLE;
#else
typedef struct GNU_PACKED _RX_VECTOR1_4TH_CYCLE {
	UINT32 IBRssi0:8;
	UINT32 WBRssix:8;
	UINT32 FagcLpfGainx:4;
	UINT32 IBRssi1:8;
	UINT32 Fagc2EqCal:1;
	UINT32 FgacCalGain:3;
} RX_VECTOR1_4TH_CYCLE;
#endif


#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RX_VECTOR1_5TH_CYCLE {
	UINT32 LTF_SNR0:6;
	UINT32 LTF_ProcTime:7;
	UINT32 FoE:12; //FreqOffSer from Rx
	UINT32 CagcState:3;
	UINT32 FagcLnaGain1:2;
	UINT32 FagcLnaGain0:2;
} RX_VECTOR1_5TH_CYCLE;
#else
typedef struct GNU_PACKED _RX_VECTOR1_5TH_CYCLE {
	UINT32 FagcLnaGain0:2;
    UINT32 FagcLnaGain1:2;
	UINT32 CagcState:3;
    UINT32 FoE:12; //FreqOffSer from Rx
    UINT32 LTF_ProcTime:7;
    UINT32 LTF_SNR0:6;
} RX_VECTOR1_5TH_CYCLE;
#endif


#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RX_VECTOR1_6TH_CYCLE {
	UINT32 Reserved:1;
	UINT32 CagcState:3;
	UINT32 NsTsField:3;
	UINT32 RxValidIndicator:1;
	UINT32 Nf2:8;
	UINT32 Nf1:8;
	UINT32 Nf0:8;
} RX_VECTOR1_6TH_CYCLE;
#else
typedef struct GNU_PACKED _RX_VECTOR1_6TH_CYCLE {
	UINT32 Nf0:8;
	UINT32 Nf1:8;
	UINT32 Nf2:8;
	UINT32 RxValidIndicator:1;
	UINT32 NsTsField:3;
	UINT32 CagcState:3;
	UINT32 Reserved:1;
} RX_VECTOR1_6TH_CYCLE;
#endif


#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RX_VECTOR2_1ST_CYCLE {
	UINT32 Ofdm:28;
	UINT32 BtEnv:1;
	UINT32 Sec40ItfrEnv:1;
	UINT32 SecItfrEnv:1;
	UINT32 PrimItfrEnv:1;
} RX_VECTOR2_1ST_CYCLE;
#else
typedef struct GNU_PACKED _RX_VECTOR2_1ST_CYCLE {
	UINT32 PrimItfrEnv:1;
	UINT32 SecItfrEnv:1;
	UINT32 Sec40ItfrEnv:1;
	UINT32 BtEnv:1;
	UINT32 Ofdm:28;
} RX_VECTOR2_1ST_CYCLE;
#endif

#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RX_VECTOR2_2ND_CYCLE {
	UINT32 RxScramblingSeed:3;
	//UINT32 DumpSelect:8;
	UINT32 Reserved2:2;
    UINT32 OfdmLtfSNR1:6;
	UINT32 BtdNoTchLoc:7;
	UINT32 Ofdm:14;
} RX_VECTOR2_2ND_CYCLE;
#else
typedef struct GNU_PACKED _RX_VECTOR2_2ND_CYCLE {
	UINT32 Ofdm:14;
	UINT32 BtdNoTchLoc:7;
	//UINT32 DumpSelect:8;
	UINT32 OfdmLtfSNR1:6;
    UINT32 Reserved2:2;
	UINT32 RxScramblingSeed:3;
} RX_VECTOR2_2ND_CYCLE;
#endif

#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RX_VECTOR2_3TH_CYCLE {
	UINT32 FcsErr:1;
	UINT32 OfdmDewModeDet:1;
	UINT32 OfdmCeLtfComb:1;
	UINT32 OfdmCeGicEnb:1;
	UINT32 OfdmCeRmsdId:3;
	UINT32 Reserved1:1;
	UINT32 BgAgcWbRssix:8;
	UINT32 BfAgcIbRssix:8;
	UINT32 BfAgcLpfGain:4;
	UINT32 Reserved0:1;
	UINT32 BfAgcLnaGainx:2;
	UINT32 HtStfDet:1;
} RX_VECTOR2_3TH_CYCLE;
#else
typedef struct GNU_PACKED _RX_VECTOR2_3TH_CYCLE {
	UINT32 HtStfDet:1;
	UINT32 BfAgcLnaGainx:2;
	UINT32 Reserved0:1;
	UINT32 BfAgcLpfGain:4;
	UINT32 BfAgcIbRssix:8;
	UINT32 BgAgcWbRssix:8;
	UINT32 Reserved1:1;
	UINT32 OfdmCeRmsdId:3;
	UINT32 OfdmCeGicEnb:1;
	UINT32 OfdmCeLtfComb:1;
	UINT32 OfdmDewModeDet:1;
	UINT32 FcsErr:1;
} RX_VECTOR2_3TH_CYCLE;
#endif


typedef struct _RXV_INFO {
	INT32 Rssi0;
	INT32 Rssi1;
} RXV_INFO;

//+++UNIFIED_TXD_RXD
typedef struct GNU_PACKED _RXV_DWORD0 {
	UINT32 TA_0_31;
} RXV_DWORD0;
//---UNIFIED_TXD_RXD


//+++UNIFIED_TXD_RXD
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RXV_DWORD1 {
	UINT32 Reserved:7;
	UINT32 TR:1;
	UINT32 RxvSn:8;
	UINT32 TA_32_47:16;
} RXV_DWORD1;
#else
typedef struct GNU_PACKED _RXV_DWORD1 {
	UINT32 TA_32_47:16;
	UINT32 RxvSn:8;
	UINT32 TR:1;
	UINT32 Reserved:7;
} RXV_DWORD1;
#endif
//---UNIFIED_TXD_RXD


//+++UNIFIED_TXD_RXD
/* RX/TX-Status Vector Format */
#ifdef RT_BIG_ENDIAN
typedef struct GNU_PACKED _RXV_STRUC{
	UINT32 Ta0;
	UINT32 Rsv25To31:7;
	UINT32 Tr:1;
	UINT32 RxVSn:8;
	UINT32 Ta32:16;
	UINT32 Vector1[6];
	UINT32 Vector2[3];
} RXV_STRUC, *PRXV_STRUC;
#else
typedef struct GNU_PACKED _RXV_STRUC{
	UINT32 Ta0;
	UINT32 Ta32:16;
	UINT32 RxVSn:8;
	UINT32 Tr:1;
	UINT32 Rsv25To31:7;
	UINT32 Vector1[6];
	UINT32 Vector2[3];
} RXV_STRUC, *PRXV_STRUC;
#endif
//---UNIFIED_TXD_RXD


//+++UNIFIED_TXD_RXD
typedef struct GNU_PACKED _RXV_FRM_STRUC{
	struct _RMAC_RXD_0_TXRXV RxVD0;
	RXV_STRUC RxVInfo[0];
} RXV_FRM_STRUC, *PRXV_FRM_STRUC;
//---UNIFIED_TXD_RXD


/******************************************************************************
	LMAC TMR Structure
	
	TMR_D_1
	TMR_D_2
	TMR_D_6
******************************************************************************/

/******************************************************************************
	TMR_D_1
******************************************************************************/
//+++UNIFIED_TXD_RXD
/*
	Timing Measurement Report Format
*/
#ifdef RT_BIG_ENDIAN
typedef union GNU_PACKED _TMR_D_1 {
	struct {
		UINT32 Rsv29:3;
		UINT32 RxVFrmMode:2;
		UINT32 RxVTxMode:3;
		UINT32 Rsv21:3;
		UINT32 TxVFrmMode:2;
		UINT32 TxVTxMode:3;
		UINT32 Rsv8:8;
		UINT32 Pid:8;
	} field_init;

	struct {
		UINT32 Rsv29:3;
		UINT32 RxVFrmMode:2;
		UINT32 RxVTxMode:3;
		UINT32 Rsv21:3;
		UINT32 TxVFrmMode:2;
		UINT32 TxVTxMode:3;
		UINT32 Rsv8:8;
		UINT32 RxVSN:8;
	} field_resp;

	UINT32 word;
} TMR_D_1, *PTMR_D_1;
#else
typedef union GNU_PACKED _TMR_D_1 {
	struct {
		UINT32 Pid:8;
		UINT32 Rsv8:8;
		UINT32 TxVTxMode:3;
		UINT32 TxVFrmMode:2;
		UINT32 Rsv21:3;
		UINT32 RxVTxMode:3;
		UINT32 RxVFrmMode:2;
		UINT32 Rsv29:3;
	} field_init;

	struct {
		UINT32 RxVSN:8;
		UINT32 Rsv8:8;
		UINT32 TxVTxMode:3;
		UINT32 TxVFrmMode:2;
		UINT32 Rsv21:3;
		UINT32 RxVTxMode:3;
		UINT32 RxVFrmMode:2;
		UINT32 Rsv29:3;
	} field_resp;

	UINT32 word;
} TMR_D_1, *PTMR_D_1;
#endif
//---UNIFIED_TXD_RXD


/******************************************************************************
	TMR_D_2
******************************************************************************/
//+++UNIFIED_TXD_RXD
#ifdef RELEASE_EXCLUDE
/*
	SnField - Sequence Number Field: (valid when IR = 1)
	Ta0 - TA: (valid when IR = 1)
*/
#endif /* RELEASE_EXCLUDE */
#ifdef RT_BIG_ENDIAN
typedef union GNU_PACKED _TMR_D_2 {
	struct {
		UINT32 SnField:16;
		UINT32 Ta0:16;
	} field;

	UINT32 word;
} TMR_D_2, *PTMR_D_2;
#else
typedef union GNU_PACKED _TMR_D_2 {
	struct {
		UINT32 Ta0:16;
		UINT32 SnField:16;
	} field;

	UINT32 word;
} TMR_D_2, *PTMR_D_2;
#endif
//---UNIFIED_TXD_RXD


/******************************************************************************
	TMR_D_6
******************************************************************************/
//+++UNIFIED_TXD_RXD
#ifdef RELEASE_EXCLUDE
/*
	ToD: Time of departure (in unit of 0.1ns)
	ToA: Time of arrival (in unit of 0.1ns)
*/
#endif /* RELEASE_EXCLUDE */
#ifdef RT_BIG_ENDIAN
typedef union GNU_PACKED _TMR_D_6 {
	struct {
		UINT32 ToA32:16;
		UINT32 ToD32:16;
	} field;

	UINT32 word;
} TMR_D_6, *PTMR_D_6;
#else
typedef union GNU_PACKED _TMR_D_6 {
	struct {
		UINT32 ToD32:16;
		UINT32 ToA32:16;
	} field;

	UINT32 word;
} TMR_D_6, *PTMR_D_6;
#endif
//---UNIFIED_TXD_RXD


/******************************************************************************
	TMR_FRM_STRUC
******************************************************************************/
//+++UNIFIED_TXD_RXD
typedef struct GNU_PACKED _TMR_FRM_STRUC{
	struct _RMAC_RXD_0_TMR TmrD0;
	TMR_D_1 TmrD1;
	TMR_D_2 TmrD2;
	UINT32 Ta16;
	UINT32 ToD0;
	UINT32 ToA0;
	TMR_D_6 TmrD6;
}TMR_FRM_STRUC, *PTMR_FRM_STRUC;
//---UNIFIED_TXD_RXD


typedef struct wtbl_entry {
	UINT8 wtbl_idx;

	struct wtbl_1_struc wtbl_1;
	struct wtbl_2_struc wtbl_2;
	union wtbl_3_struc wtbl_3;
	struct wtbl_4_struc wtbl_4;

	UINT32 wtbl_addr[4];
	UINT16 wtbl_fid[4];
	UINT16 wtbl_eid[4];
}WTBL_ENTRY;


#if 0
#if defined(MT7603) || defined(MT7628)
#define MT7603_PSE_WTBL_2_ADDR	0xa5000000  /* get from CR setting of PSE.BUFS_DAA (0x164) */
#define MT7603_MT_WTBL_SIZE			128
#endif /* MT7603 */

#ifdef MT7628
#define MT7628_PSE_WTBL_2_ADDR	0xa5000000  /* get from CR setting of PSE.BUFS_DAA (0x164) */
#define MT7628_MT_WTBL_SIZE			128
#ifdef MT7628_FPGA
#undef MT7628_MT_WTBL_SIZE
#define MT7628_MT_WTBL_SIZE			20
#endif /* MT7628_FPGA */
#endif /* MT7628 */

#ifdef MT7636
#define MT7636_PSE_WTBL_2_ADDR	0xa8000000  /* get from CR setting of PSE.BUFS_DAA (0x164) */
#define MT7636_MT_WTBL_SIZE			32
#ifdef MT7636_FPGA
#undef MT7636_MT_WTBL_SIZE
#define MT7636_MT_WTBL_SIZE			20
#endif /* MT7636_FPGA */
#endif /* MT7636 */


#define MT_PSE_BASE_ADDR		0xa0000000
#define MCU_CFG_BASE		0x2000

#define MCU_PCIE_REMAP_1	(MCU_CFG_BASE + 0x500)
#define REMAP_1_OFFSET_MASK (0x3ffff)
#define GET_REMAP_1_OFFSET(p) (((p) & REMAP_1_OFFSET_MASK))
#define REMAP_1_BASE_MASK	(0x3fff << 18)
#define GET_REMAP_1_BASE(p) (((p) & REMAP_1_BASE_MASK) >> 18)
#define MCU_PCIE_REMAP_2	(MCU_CFG_BASE + 0x504)
#define REMAP_2_OFFSET_MASK (0x7ffff)
#define GET_REMAP_2_OFFSET(p) (((p) & REMAP_2_OFFSET_MASK))
#define REMAP_2_BASE_MASK (0x1fff << 19)
#define GET_REMAP_2_BASE(p) (((p) & REMAP_2_BASE_MASK) >> 19)

#define TOP_CFG_BASE        0x0000

#define XTAL_CTL4           (TOP_CFG_BASE + 0x1210)
#define XTAL_CTL13          (TOP_CFG_BASE + 0x1234)
#define XTAL_CTL14          (TOP_CFG_BASE + 0x1238)
#define DA_XO_C2_MASK (0x7f << 8)
#define DA_XO_C2(p) (((p) & 0x7f) << 8)
#endif

#if defined(MT7603_FPGA) || defined(MT7628_FPGA) || defined(MT7636_FPGA)
#define MT_PSE_PAGE_SIZE		256
#else
#define MT_PSE_PAGE_SIZE		128
#endif /* MT7603_FPGA */


#if defined(MT7603) || defined(MT7628) || defined(MT7636)
#define DMA_SCH_LMAC		0
#define DMA_SCH_BYPASS		1
#define DMA_SCH_HYBRID		2
#endif /* defined(MT7603) || defined(MT7628) || defined(MT7636) */

#define MT_PCI_REMAP_ADDR_1		0x40000
#define MT_PCI_REMAP_ADDR_2		0x80000



// TODO: shiang-7603, this is a dummy data structure and need to revise to adapative for MT7603 series

#define WMM_QUE_NUM		4 /* each sta has 4 Queues to mapping to each WMM AC */

/* value domain of pTxD->HostQId (4-bit: 0~15) */
#define QID_AC_BK               1   /* meet ACI definition in 802.11e */
#define QID_AC_BE               0   /* meet ACI definition in 802.11e */
#define QID_AC_VI               2
#define QID_AC_VO               3
#define QID_HCCA                4
//#define NUM_OF_TX_RING		4
#define NUM_OF_WMM1_TX_RING	1

#define QID_BMC                 8

#define NUM_OF_RX_RING		1
#ifdef CONFIG_ANDES_SUPPORT
#undef NUM_OF_RX_RING
#define NUM_OF_RX_RING		2
#endif /* CONFIG_ANDES_SUPPORT */

#define QID_MGMT                13
#define QID_RX                  14
#define QID_OTHER               15
#ifdef CONFIG_ANDES_SUPPORT
#define QID_CTRL				16
#endif /* CONFIG_ANDES_SUPPORT */

#define QID_BCN				17

#define SHAREDKEYTABLE			0
#define PAIRWISEKEYTABLE		1

#define PID_NULL_FRAME_PWR_ACTIVE	0x0a
#define PID_NULL_FRAME_PWR_SAVE		0x0b
/* Indicate the txs report to Host or MCU */
typedef enum {
	TXS2MCU = 0,
	TXS2HOST = 1,
} TXS_TARGET_PORT;

#define TXS2MCU_AGGNUMS 31
#define TXS2HOST_AGGNUMS 31

enum {
	TXS2M_QID0,
	TXS2M_QID1,
	TXS2M_QID2,
	TXS2M_QID3,
};

enum {
	TXS2H_QID0,
	TXS2H_QID1,
};

enum {
	TXS_FORMAT0,
	TXS_FORMAT1,
};

#ifdef CONFIG_TSO_SUPPORT
#define TSO_SIZE		4
#else
#define TSO_SIZE		0
#endif /* CONFIG_TSO_SUPPORT */

#define EXP_ACK_TIME	0x1380

#if 0
#ifdef RT_BIG_ENDIAN
typedef	union GNU_PACKED _TX_STA_CNT0_STRUC {
	struct {
	    UINT16  TxBeaconCount;
	    UINT16  TxFailCount;
	} field;
	UINT32 word;
} TX_STA_CNT0_STRUC;
#else
typedef	union GNU_PACKED _TX_STA_CNT0_STRUC {
	struct {
	    UINT16  TxFailCount;
	    UINT16  TxBeaconCount;
	} field;
	UINT32 word;
} TX_STA_CNT0_STRUC;
#endif

#ifdef RT_BIG_ENDIAN
typedef	union GNU_PACKED _TX_STA_CNT1_STRUC {
	struct {
	    UINT16  TxRetransmit;
	    UINT16  TxSuccess;
	} field;
	UINT32 word;
} TX_STA_CNT1_STRUC;
#else
typedef	union GNU_PACKED _TX_STA_CNT1_STRUC {
	struct {
	    UINT16  TxSuccess;
	    UINT16  TxRetransmit;
	} field;
	UINT32 word;
} TX_STA_CNT1_STRUC;
#endif
#endif
typedef struct __TX_CNT_INFO {
	union WTBL_2_DW5 wtbl_2_d5;
	union WTBL_2_DW6 wtbl_2_d6;
	union WTBL_2_DW7 wtbl_2_d7;
	union WTBL_2_DW8 wtbl_2_d8;
#ifndef WIFI_BUILD_RAM	// Workaround to fix RAM build issue: Raju
	union WTBL_2_DW9 wtbl_2_d9;
#endif
} TX_CNT_INFO;


#define E2PROM_CSR          0x0004
#define GPIO_CTRL_CFG	0x0228
#define WSC_HDR_BTN_GPIO_0			((UINT32)0x00000001) /* bit 0 for RT2860/RT2870 */
#define WSC_HDR_BTN_GPIO_3			((UINT32)0x00000008) /* bit 3 for RT2860/RT2870 */

#define LEGACY_BASIC_RATE	0x1408

//#define HW_BEACON_MAX_SIZE(__pAd)      ((__pAd)->chipCap.BcnMaxHwSize)

#ifndef WIFI_BUILD_RAM
enum 
{
	BSS_INFO_OWN_MAC = 0,
	BSS_INFO_BASIC = 1,
	BSS_INFO_RF_CH = 2,
	BSS_INFO_PM = 3,
	BSS_INFO_UAPSD = 4,
	BSS_INFO_ROAM_DETECTION = 5,
 	BSS_INFO_EXT_BSS = 7,
 	BSS_INFO_BROADCAST_INFO =8,
    BSS_INFO_SYNC_MODE = 9,
    BSS_INFO_MAX_NUM
};

enum
{
	BSS_INFO_OWN_MAC_FEATURE = (1 << BSS_INFO_OWN_MAC),
	BSS_INFO_BASIC_FEATURE = (1 << BSS_INFO_BASIC),
	BSS_INFO_RF_CH_FEATURE = (1 << BSS_INFO_RF_CH),
	BSS_INFO_PM_FEATURE = (1 << BSS_INFO_PM),
	BSS_INFO_UAPSD_FEATURE = (1 << BSS_INFO_UAPSD),
	BSS_INFO_ROAM_DETECTION_FEATURE = (1 << BSS_INFO_ROAM_DETECTION),
    BSS_INFO_EXT_BSS_FEATURE = (1 << BSS_INFO_EXT_BSS),
    BSS_INFO_SYNC_MODE_FEATURE = (1 << BSS_INFO_SYNC_MODE),
	BSS_INFO_MAX_NUM_FEATURE = (1 << BSS_INFO_MAX_NUM)
};

#else
#define BSS_INFO_SYNC_MODE 9
enum
{
	BSS_INFO_OWN_MAC_FEATURE = (1 << BSS_INFO_CONN_OWN_DEV),
	BSS_INFO_BASIC_FEATURE = (1 << BSS_INFO_BASIC),
	BSS_INFO_RF_CH_FEATURE = (1 << BSS_INFO_RF_CH),
	BSS_INFO_PM_FEATURE = (1 << BSS_INFO_PM),
	BSS_INFO_UAPSD_FEATURE = (1 << BSS_INFO_UAPSD),
	BSS_INFO_ROAM_DETECTION_FEATURE = (1 << BSS_INFO_RSSI_RM),
    BSS_INFO_EXT_BSS_FEATURE = (1 << BSS_INFO_EXT_BSS),
	BSS_INFO_MAX_NUM_FEATURE = (1 << BSS_INFO_EXT_CMD_TAG_MAX_NUM),
    BSS_INFO_SYNC_MODE_FEATURE = (1 << BSS_INFO_SYNC_MODE),
};
#endif

#ifndef WIFI_BUILD_RAM

enum
{
	STA_REC_BASIC_STA_RECORD = 0,
	STA_REC_RA = 1,
	STA_REC_RA_COMMON_INFO = 2,
	STA_REC_RA_UPDATE = 3,
	STA_REC_BF = 4,
	STA_REC_AMSDU = 5,
	STA_REC_BA = 6,
	STA_REC_RED = 7,
	STA_REC_TX_PROC = 8,
	STA_REC_BASIC_HT_INFO = 9,
	STA_REC_BASIC_VHT_INFO = 10,
	STA_REC_AP_PS = 11,
	STA_REC_MAX_NUM
};

enum
{
	STA_REC_BASIC_STA_RECORD_FEATURE = (1 << STA_REC_BASIC_STA_RECORD),
	STA_REC_RA_FEATURE = (1 << STA_REC_RA),
	STA_REC_RA_COMMON_INFO_FEATURE = (1 << STA_REC_RA_COMMON_INFO),
	STA_REC_RA_UPDATE_FEATURE = (1 << STA_REC_RA_UPDATE),
	STA_REC_BF_FEATURE = (1 << STA_REC_BF),
	STA_REC_AMSDU_FEATURE = (1 << STA_REC_AMSDU),
	STA_REC_BA_FEATURE = (1 << STA_REC_BA),
	STA_REC_RED_FEATURE = (1 << STA_REC_RED),
	STA_REC_TX_PROC_FEATURE = (1 << STA_REC_TX_PROC),
	STA_REC_BASIC_HT_INFO_FEATURE  = (1 << STA_REC_BASIC_HT_INFO),
	STA_REC_BASIC_VHT_INFO_FEATURE = (1 << STA_REC_BASIC_VHT_INFO),
	STA_REC_AP_PS_FEATURE = (1 << STA_REC_AP_PS),
	STA_REC_MAX_NUM_FEATURE = (1 << STA_REC_MAX_NUM)
};

#else
#if 0
/*  STA record TLV tag */
typedef enum _EXT_ENUM_STAREC_TAG_HANDLE_T
{
    STA_REC_BASIC               = 0x00,
    STA_REC_RA                  = 0x01,
    STA_REC_RA_COMMON_INFO      = 0x02,
    STA_REC_RA_UPDATE           = 0x03,
    STA_REC_BF                  = 0x04,
    STA_REC_BA                  = 0x06,
    STA_REC_HT_BASIC		    = 0x09,
    STA_REC_VHT_BASIC		    = 0x0a,
    STA_REC_MAX_NUM
} EXT_ENUM_TAG_STAREC_HANDLE_T;

#endif

enum
{
	STA_REC_BASIC_STA_RECORD_FEATURE = (1 << STA_REC_BASIC),
	STA_REC_RA_FEATURE = (1 << STA_REC_RA),
	STA_REC_RA_COMMON_INFO_FEATURE = (1 << STA_REC_RA_COMMON_INFO),
	STA_REC_RA_UPDATE_FEATURE = (1 << STA_REC_RA_UPDATE),
	STA_REC_BF_FEATURE = (1 << STA_REC_BF),
	//STA_REC_AMSDU_FEATURE = (1 << STA_REC_AMSDU),
	STA_REC_BA_FEATURE = (1 << STA_REC_BA),
	//STA_REC_RED_FEATURE = (1 << STA_REC_RED),
	//STA_REC_TX_PROC_FEATURE = (1 << STA_REC_TX_PROC),
	STA_REC_BASIC_HT_INFO_FEATURE  = (1 << STA_REC_HT_BASIC),
	STA_REC_BASIC_VHT_INFO_FEATURE = (1 << STA_REC_VHT_BASIC),
	//STA_REC_AP_PS_FEATURE = (1 << STA_REC_AP_PS),
	//STA_REC_MAX_NUM_FEATURE = (1 << STA_REC_MAX_NUM)
};


#endif

struct _RTMP_ADAPTER;

VOID dump_mt_mac_cr(struct _RTMP_ADAPTER *pAd);

INT mt_mac_init(struct _RTMP_ADAPTER *pAd);

VOID dump_wtbl_entry(struct _RTMP_ADAPTER *pAd, struct wtbl_entry *ent);
VOID dump_wtbl_info(struct _RTMP_ADAPTER *pAd, UINT wtbl_idx);
VOID dump_wtbl_base_info(struct _RTMP_ADAPTER *pAd);

INT mt_mac_set_ctrlch(struct _RTMP_ADAPTER *pAd, UINT8 extch);

#define HIF_PORT 1
#define MCU_PORT 2
#define MT_TX_RETRY_UNLIMIT		0x1f
#define MT_TX_SHORT_RETRY		0x0f
#define MT_TX_LONG_RETRY		0x0f

#endif /* __MT_MAC_H__ */


