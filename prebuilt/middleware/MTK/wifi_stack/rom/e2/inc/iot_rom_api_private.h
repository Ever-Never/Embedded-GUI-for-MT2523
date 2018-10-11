/*
** $Id: //Department/DaVinci/TRUNK/MT6620_WiFi_Firmware/mcu/wifi/include/wifi_rom_api_vctr.h#7 $
*/

/*! \file   "wifi_rom_api_vctr.h"
    \brief

    Detail description.
*/

/******************************************************************************
* Copyright (c) 2007 MediaTek Inc.
*
* All rights reserved. Copying, compilation, modification, distribution
* or any other use whatsoever of this material is strictly prohibited
* except in accordance with a Software License Agreement with
* MediaTek Inc.
*******************************************************************************
*/

/******************************************************************************
* LEGAL DISCLAIMER
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK
* SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
* PROVIDED TO BUYER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
* DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
* LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE
* ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY
* WHICH MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK
* SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY
* WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE
* FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION OR TO
* CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL
* BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT
* ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
* BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
* WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT
* OF LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING
* THEREOF AND RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN
* FRANCISCO, CA, UNDER THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE
* (ICC).
*******************************************************************************
*/

/*
** $Log: iot_rom_api_private.h $
**
** 06 17 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** CFG80211 patch support.
 *
 * 06 16 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Check-in not complete. Need to check-in more files
 * Add following things:
 * 1) Make Number of STA configurable in AP mode. Now sta entries are dynamically allocated.
 * 2) Make values of other macros configurable
 * 3) Fix STA powersave issue: STA sending PSPOLL in fast PSP
 * 4) Add fixes for opmode change.
**
** 06 14 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** WPA2PSK security verified
 *
 * 06 13 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Add repeater Support. Synced to JEDI revision: 127949
 * 1) DHCP working
 * 2) ARP working
 * 3) Ping working.
**
** 06 11 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** AP mode to STA mode configuration issue fixed + ASSOC IE event format modidied
 *
 * 06 11 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * 1) Added AMSDU Rx Support
 * 2) Fixed Issue of AP not able to send Broadcast frames.
 * 3) Fixed AP Frame forwarding issue.
 * 4) Added support for AP Powersave handling of BMC frames at DTIM.
**
** 06 10 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** ASSOC IE event sent to HOST.
**
** 06 10 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** OpMode configuration support added (STA+promiscus, AP).
** Assoc IE sent to hostapd-support added.
**
** 06 05 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** WOW Support added
**
** 06 04 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** 40MHz support added and auto-connection code added
**
** 06 04 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** 11n support added + Some inline functions changed to patchable function
 *
 * 05 27 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Fix AP and APCLI data path issues:
 * 1) tx_hw_header_len not filled properly
 * 2) removed some PCI related code
 * 3) APCLI data packet was getting dropped if bssidx in RxD was 1
 * 4) WDEV idx was getting stored wrongly.
 * 5) fixed compilation error if repeater mode is disabled and apcli mode is enabled
**
** 05 22 2015 michael.rong
** [BORA00004254] [MT7637E2][IOT][FW]
** 1. Fix compile error
** 2. Send the whole beacon frame to host for scan operation of supplicant
 *
 * 05 21 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Remove RAM header file dependancy in ROM code.
 * Created parallel header files for RAM files which were being used in ROM code.
**
** 05 15 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** IOT code with STA+promiscus mode and AP mode with APCLI and Repeater feature.
**
** 05 13 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** [APSTA] Patching support added.
 *
 * 05 09 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Integrated Branch for AP-STA.
 *
 * 04 28 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Integrated AP/APCLI code with the STA mode to use same rate related functions
 * Updated rate related functions to the latest ones and changed the corresponding STA code
 *
 * 04 28 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Integrated AP/APCLI code with the STA mode to use same rate related functions
 * Updated rate related functions to the latest ones and changed the corresponding STA code
 *
 * 04 27 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Added APCLI, Repeater and MAT codebase.
 *
 * 04 27 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Added APCLI, Repeater and MAT codebase.
 *
 * 04 27 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Added APCLI, Repeater and MAT codebase.
**
** 01 13 2015 Michael Rong
** Initial 
 *
**
*/

#ifndef _IOT_ROM_API_PRIVATE_H
#define _IOT_ROM_API_PRIVATE_H

/******************************************************************************
*                         C O M P I L E R   F L A G S
*******************************************************************************
*/

/******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
*******************************************************************************
*/

//#include "hif.h"
//#include "cos_perf.h"
//#include "config_rom.h"

#include "cmm_asic_mt.h"
#include "offload_routine.h"

#include "rt_os_util.h"
#include "rtmp_cmd.h"
#include "rom/patch_config.h"
/******************************************************************************
*                              C O N S T A N T S
*******************************************************************************
*/


/******************************************************************************
*                             D A T A   T Y P E S
*******************************************************************************
*/

/******************************************************************************
*                            P U B L I C   D A T A
*******************************************************************************
*/

/******************************************************************************
*                           P R I V A T E   D A T A
*******************************************************************************
*/

/******************************************************************************
*                                 M A C R O S
*******************************************************************************
*/

/******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************
*/

/******************************************************************************
*                              F U N C T I O N S
*******************************************************************************
*/
// ba_action.c
// dh_key.c
// evp_enc.c
// hmac.c
// md4.c

#if 1//md5.c
__romtext VOID  __MD5Transform(ULONG Buf[4], ULONG Mes[16]);

__romtext VOID  __SHATransform(ULONG Buf[5], ULONG Mes[20]);

__romtext VOID  __MD5Init(MD5_CTX *pCtx);

VOID __romtext __MD5Update(MD5_CTX *pCtx, UCHAR *pData, ULONG LenInBytes);

VOID __romtext __MD5Final(UCHAR Digest[16], MD5_CTX *pCtx);

VOID __romtext __MD5Transform(ULONG Buf[4], ULONG Mes[16]);

void __romtext __md5_mac(u8 *key, size_t key_len, u8 *data, size_t data_len, u8 *mac);

void __romtext __hmac_md5(u8 *key, size_t key_len, u8 *data, size_t data_len, u8 *mac);

VOID __romtext RT_HMAC_MD5(
    IN  UINT8 Key[],
    IN  UINT KeyLen,
    IN  UINT8 Message[],
    IN  UINT MessageLen,
    OUT UINT8 MAC[],
    IN  UINT MACLen);

VOID __romtext __SHAInit(SHA_CTX *pCtx);

UCHAR __romtext __SHAUpdate(SHA_CTX *pCtx, UCHAR *pData, ULONG LenInBytes);

VOID __romtext __SHAFinal(SHA_CTX *pCtx, UCHAR Digest[20]);

VOID __romtext __SHATransform(ULONG Buf[5], ULONG Mes[20]);

__romtext unsigned char *__wpa_aes_engine_init(void);

int	__romtext __rtmp_aes_set_key( aes_context *ctx, uint8 *key, int nbits );

void __romtext __rtmp_aes_encrypt(aes_context *ctx, uint8 input[16],	uint8 output[16] );

void __romtext __rtmp_aes_decrypt( aes_context *ctx,	uint8 input[16], uint8 output[16] );

VOID  __romtext AES_CMAC_GenerateSubKey (
    IN UINT8 Key[],
    IN UINT KeyLength,
    OUT UINT8 SubKey1[],
    OUT UINT8 SubKey2[]);

VOID  __romtext AES_CMAC (
    IN UINT8 PlainText[],
    IN UINT PlainTextLength,
    IN UINT8 Key[],
    IN UINT KeyLength,
    OUT UINT8 MACText[],
    INOUT UINT *MACTextLength);

INT __romtext AES_Key_Wrap(
    IN UINT8    *plaintext,
    IN UINT     p_len,
    IN UINT8    *key,
    IN UINT      key_len,
    OUT UINT8   *ciphertext,
    OUT UINT    *c_len);

INT	__romtext AES_Key_Unwrap(
    IN	UINT8	*ciphertext,
    IN	UINT    c_len,
    IN	UINT8	*key,
    IN  UINT     key_len,
    OUT	UINT8	*plaintext,
    OUT	UINT    *p_len);

VOID	__romtext  __HMAC_SHA1(
    IN	UCHAR	 *text,
    IN	UINT	text_len,
    IN	UCHAR	 *key,
    IN	UINT	key_len,
    IN	UCHAR	*digest);

VOID __romtext RT_HMAC_SHA1 (
    UINT_8 Key[],
    UINT_32 KeyLen,
    UINT_8 Message[],
    UINT_32 MessageLen,
    UINT_8 MAC[],
    UINT_32 MACLen
);

#endif

// rom_fun.c
// rtmp_ckipmic.c
// rtmp_tkip.c
// rtmp_wep.c
// sha2.c
// conn_sys_dev.c
void move_iot_rom_data_to_ram(void);
void exp_printf(const char *fmt, ...);
extern void (*iot_init)(void);
void iot_start(void);
extern error_t (*connsys_netif_setup)(struct netif *netif);
extern void (*netif_initialize)(void);
extern void (*iot_process)(void);
BOOLEAN connsys_inband_process(PUINT_8 prPacket);

// rt_main_dev.c
extern error_t (*wlan_netif_setup)(struct netif *netif);
extern error_t (*rt28xx_packet_xmit)(PNDIS_PACKET p);
extern void        (*rx_dma_done_process)(struct netif *netif, int queue);
extern void        (*RTMPHandleTxRingDmaDoneInterrupt)(struct netif *netif);
extern NDIS_STATUS (*rt2860_init)(struct netif *netif);
extern int         (*rt2860_open)(struct netif *netif);
extern int         (*rt2860_close)(struct netif *net_dev);
void init_profile(struct sys_cfg_t *profile);
/*START: New added */
extern VOID (*mt7636_chipCap_init)(VOID);
VOID mt7636_chipOp_init(VOID);
extern VOID (*mt7636_init)(RTMP_ADAPTER *pAd);
extern error_t (*rt28xx_send_packets)(
		IN struct netif 		*net_dev, 
		IN PNDIS_PACKET 		pPacket);

#ifdef CONFIG_STA_SUPPORT
#if WIRELESS_EXT >= 12
struct iw_statistics *rt28xx_get_wireless_stats(
		IN struct netif 	*net_dev);
#endif
#endif
/*END: New added */

// rtmp_init.c
extern NDIS_STATUS	(*RTMPAllocAdapterBlock)(
	OUT	PRTMP_ADAPTER	*ppAdapter);
extern VOID	(*RTMPFreeAdapter)(
	IN	VOID		*pAdSrc);
extern VOID	(*RTMPReadChannelPwr)(
	IN	PRTMP_ADAPTER	pAd);
NDIS_STATUS	NICReadRegParameters(
	IN	PRTMP_ADAPTER		pAd,
	IN	NDIS_HANDLE			WrapperConfigurationContext);
NDIS_STATUS	NICReadRegParameters(
	IN	PRTMP_ADAPTER		pAd,
	IN	NDIS_HANDLE			WrapperConfigurationContext);
extern VOID	(*NICReadEEPROMParameters)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			mac_addr);
extern VOID (*NICInitAsicFromEEPROM)(
	IN	PRTMP_ADAPTER	pAd);
extern NDIS_STATUS (*NICInitializeAdapter)(
		IN  PRTMP_ADAPTER   pAd,
		IN   BOOLEAN    bHardReset);
extern NDIS_STATUS	(*NICInitializeAsic)(
	IN	PRTMP_ADAPTER	pAd,
	IN  BOOLEAN		bHardReset);
extern VOID (*NICIssueReset)(
	IN	PRTMP_ADAPTER	pAd);
extern VOID (*NICDisableInterrupt)(
    IN  PRTMP_ADAPTER   pAd);
VOID NICEnableInterrupt(
    IN  PRTMP_ADAPTER   pAd);
extern VOID (*NICUpdateRawCounters)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*UserCfgInit)(
	IN	PRTMP_ADAPTER pAd);
extern UCHAR (*rom_BtoH)(char ch);
extern void (*rom_AtoH)(char * src, UCHAR * dest, int destlen);
#if 0 //declared in wifi_os.h
VOID RTMPInitTimer(
	IN	PRTMP_ADAPTER			pAd,
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	PVOID					pTimerFunc,
	IN	PVOID					pData,
	IN	BOOLEAN					Repeat,
	IN				const char * const pcTimerName);
VOID RTMPSetTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	ULONG					u4TimeoutMs);
VOID RTMPModTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	ULONG					Value);
VOID RTMPCancelTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	OUT	BOOLEAN					*pCancelled);
#endif
extern VOID (*RTMPSetLED)(
	IN PRTMP_ADAPTER 	pAd, 
	IN UCHAR			Status);
extern VOID (*RTMPSetSignalLED)(
	IN PRTMP_ADAPTER 	pAd, 
	IN NDIS_802_11_RSSI Dbm);
extern VOID (*RTMPEnableRxTx)(
	IN PRTMP_ADAPTER	pAd);

/*START: New added */
extern INT (*RtmpChipOpsRegister)(RTMP_ADAPTER *pAd, INT infType);
extern INT (*RtmpRaDevCtrlInit)(VOID *pAdSrc, int infType);
BOOLEAN RtmpRaDevCtrlExit(IN VOID *pAdSrc);
/*END: New added */


// rt_bora.c
VOID RTMP_SetPeriodicTimer(
		  IN  NDIS_MINIPORT_TIMER *pTimer, 
		  IN  unsigned long timeout);
VOID RTMP_OS_Init_Timer(
	   IN  PRTMP_ADAPTER pAd,
	   IN  NDIS_MINIPORT_TIMER *pTimer, 
	   IN  TIMER_FUNCTION function,
	   IN  PVOID data);
VOID RTMP_OS_Add_Timer(
	  IN  NDIS_MINIPORT_TIMER     *pTimer,
	  IN  unsigned long timeout);
VOID RTMP_OS_Mod_Timer(
	  IN  NDIS_MINIPORT_TIMER     *pTimer,
	  IN  unsigned long timeout);
VOID RTMP_OS_Del_Timer(
	  IN  NDIS_MINIPORT_TIMER     *pTimer,
	  OUT BOOLEAN                 *pCancelled);
extern VOID (*RTMPusecDelay)(IN  ULONG   usec);
void RTMP_GetCurrentSystemTime(LARGE_INTEGER *time);
extern NDIS_STATUS  (*os_alloc_mem)(
			IN  PRTMP_ADAPTER pAd,
			OUT PUCHAR *mem,
			IN  ULONG  size);
extern NDIS_STATUS (*os_free_mem)(
		   IN  PRTMP_ADAPTER pAd,
		   IN  PUCHAR mem);
extern NDIS_STATUS (*RTMPAllocateNdisPacket)(
	  IN  PRTMP_ADAPTER   pAd,
	  OUT PNDIS_PACKET   *ppPacket,
	  IN  PUCHAR          pHeader,
	  IN  UINT            HeaderLen,
	  IN  PUCHAR          pData,
	  IN  UINT            DataLen);
extern VOID (*RTMPFreeNdisPacket)(
	   IN PRTMP_ADAPTER pAd,
	   IN PNDIS_PACKET  pPacket);
extern NDIS_STATUS (*Sniff2BytesFromNdisBuffer)(
	 IN  PNDIS_BUFFER    pFirstBuffer,
	 IN  UCHAR           DesiredOffset,
	 OUT PUCHAR          pByte0,
	 OUT PUCHAR          pByte1);
extern void (*RTMP_QueryPacketInfo)(
	 IN  PNDIS_PACKET pPacket,
	 OUT PACKET_INFO  *pPacketInfo,
	 OUT PUCHAR       *pSrcBufVA,
	 OUT UINT         *pSrcBufLen);
extern PNET_DEV (*get_netdev_from_bssid)(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR           FromWhichBSSID);
extern PNDIS_PACKET (*duplicate_pkt_with_TKIP_MIC)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket);
extern PNDIS_PACKET (*ClonePacket)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PNDIS_PACKET    pPacket,    
	IN  PUCHAR          pData,
	IN  ULONG           DataSize);
void  update_os_packet_info(
	   IN  PRTMP_ADAPTER   pAd, 
	   IN  RX_BLK          *pRxBlk,
	   IN  UCHAR           FromWhichBSSID);
extern void (*wlan_802_11_to_802_3_packet)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  RX_BLK          *pRxBlk,
	IN  PUCHAR          pHeader802_3,
	IN  UCHAR           FromWhichBSSID);
extern void (*announce_802_3_packet)(
	  IN  PRTMP_ADAPTER   pAd, 
	  IN  PNDIS_PACKET    pPacket);

#ifdef IOT_PROMISCUS_MODE
extern void (*announce_802_11_packet)(
	  IN  PRTMP_ADAPTER   pAd, 
	  IN  PNDIS_PACKET    pPacket);
#endif /* IOT_PROMISCUS_MODE */

//void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);
extern VOID (*RTMPSendWirelessEvent)(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Event_flag,
	IN	PUCHAR 			pAddr,
	IN	UCHAR			BssIdx,
	IN	CHAR			Rssi);
extern void (*RTMP_IndicateMediaState)(
	IN	PRTMP_ADAPTER	pAd);
extern int (*bridge_rx_handle)(struct net_pkt_blk *pkt);
int RtmpOSWrielessEventSend(
	IN PNET_DEV pNetDev,
	IN UINT32 eventType,
	IN INT flags,
	IN PUCHAR pSrcMac,
	IN PUCHAR pData,
	IN UINT32 dataLen);

int RtmpOSWrielessEventSendExt(
	IN PNET_DEV pNetDev,
	IN UINT32 eventType,
	IN INT flags,
	IN PUCHAR pSrcMac,
	IN PUCHAR pData,
	IN UINT32 dataLen,
	IN UINT32 family);

// in_band_msg.c
extern void (*wireless_send_event)(
		IN struct netif *dev,
		IN UINT     cmd,
		IN union iwreq_data * wrq,
		IN PUCHAR    extra);
void MacResponse (struct netif *dev);

extern struct net_pkt_blk *(*CreateInBandFrame)(IN INT len);

extern INT (*RspByInBands)(
	   IN  USHORT cmd_id,
	   IN  CHAR *msg, 
	   IN  INT   total,
	   IN  USHORT cmd_seq,
	   IN  INT status);

extern VOID (*SendHT2040ToHost)(IN  CHAR bw);
extern VOID (*SendBeaconToHost)(
	IN  CHAR	*pMsg,
	IN  INT	MsgLen,
	IN  CHAR Channel,
	IN  CHAR Rssi);
VOID SendWpsPBCApSortToHost(IN  VOID);
VOID SendWpsPinApSortToHost(IN CHAR *ssid, IN INT ssidlen);

// rtmp_data.c
extern VOID (*APRxEAPOLFrameIndicate)(
	IN	PRTMP_ADAPTER	pAd,
	IN	MAC_TABLE_ENTRY	*pEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

extern VOID (*STARxEAPOLFrameIndicate)(
	IN	PRTMP_ADAPTER	pAd,
	IN	MAC_TABLE_ENTRY	*pEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern VOID (*STARxDataFrameAnnounce)(
	IN	PRTMP_ADAPTER	pAd,
	IN	MAC_TABLE_ENTRY	*pEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern VOID (*STAHandleRxDataFrame)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk);
extern VOID (*STAHandleRxMgmtFrame)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk);
extern VOID (*STAHandleRxControlFrame)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk);
extern NDIS_STATUS (*RTMPCheckRxError)(
	IN	PRTMP_ADAPTER		pAd, 
	IN	PHEADER_802_11		pHeader,
	IN	PRXINFO_STRUC 		pRxInfo);
extern VOID (*STARxDoneInterruptHandle)(
	IN	PRTMP_ADAPTER	pAd, 
	IN  PNDIS_PACKET	pRxPacket);
extern VOID (*STASendPackets)(
	IN	NDIS_HANDLE		MiniportAdapterContext,
	IN	PPNDIS_PACKET	ppPacketArray,
	IN	UINT			NumberOfPackets);
extern NDIS_STATUS (*STASendPacket)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket);
extern VOID (*RTMPSendNullFrame)(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			TxRate,
	IN	BOOLEAN 		bQosNull);
extern VOID (*STAFindCipherAlgorithm)(
	IN PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk);
extern VOID (*STABuildCommon802_11Header)(
	IN  PRTMP_ADAPTER   pAd,
	IN  TX_BLK          *pTxBlk);
extern VOID (*STABuildCache802_11Header)(
	IN RTMP_ADAPTER		*pAd,
	IN TX_BLK			*pTxBlk,
	IN UCHAR			*pHeader);
extern PUCHAR (*STA_Build_AMSDU_Frame_Header)(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK		*pTxBlk);
extern VOID (*STA_AMPDU_Frame_Tx)(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk);

extern VOID (*STA_AMSDU_Frame_Tx)(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk);
extern VOID (*STA_Legacy_Frame_Tx)(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk);
extern VOID (*STA_Fragment_Frame_Tx)(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK		*pTxBlk);
extern NDIS_STATUS (*STAHardTransmit)(
	IN	PRTMP_ADAPTER	pAd, 
	IN TX_BLK 			*pTxBlk,
	IN	UCHAR			QueIdx);
INT WpaCheckEapCode(
	IN  PRTMP_ADAPTER   		pAd,
	IN  PUCHAR				pFrame,
	IN  USHORT				FrameLen,
	IN  USHORT				OffSet);
VOID WpaSendMicFailureToWpaSupplicant(
    IN  PRTMP_ADAPTER    pAd,
    IN  BOOLEAN          bUnicast);

// cmm_data.c
VOID PseKickOutTx(
	IN	PRTMP_ADAPTER	pAd,
	IN  TX_BLK *pTxBlk,	
	IN	UCHAR			QueIdx);

extern NDIS_STATUS (*MiniportMMRequest)(
		 IN  PRTMP_ADAPTER   pAd,
		 IN  UCHAR           QueIdx,
		 IN  PUCHAR          pData,
		 IN  UINT            Length);

extern NDIS_STATUS (*MlmeHardTransmit)(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR	QueIdx,
	IN	PNDIS_PACKET	pPacket);
extern NDIS_STATUS (*MlmeHardTransmitMgmtRing)(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR	QueIdx,
	IN	PNDIS_PACKET	pPacket);
extern UCHAR (*TxPktClassification)(
	IN RTMP_ADAPTER *pAd, 
	IN PNDIS_PACKET  pPacket);
extern VOID (*RTMP_FillTxBlkInfo)(
	IN RTMP_ADAPTER *pAd, 
	IN TX_BLK *pTxBlk);

extern USHORT	(*RTMPCalcDuration)(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			Rate,
	IN	ULONG			Size);
extern VOID (*RTMPWriteTxWI_Data)(
	IN	PRTMP_ADAPTER		pAd,
	IN	OUT PTXWI_STRUC		pTxWI,
	IN	TX_BLK				*pTxBlk);
extern VOID (*RTMPWriteTxWI_Cache)(
	IN	PRTMP_ADAPTER		pAd,
	IN	OUT PTXWI_STRUC		pTxWI,
	IN	TX_BLK				*pTxBlk);
extern VOID (*RTMPSuspendMsduTransmission)(
	IN	PRTMP_ADAPTER	pAd);
extern VOID (*RTMPResumeMsduTransmission)(
	IN	PRTMP_ADAPTER	pAd);
extern MAC_TABLE_ENTRY *(*MacTableLookup)(
	IN PRTMP_ADAPTER pAd, 
	PUCHAR pAddr);
extern VOID (*STAMacTableReset)(
	IN  PRTMP_ADAPTER  pAd);
MAC_TABLE_ENTRY *MacTableInsertEntry(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PUCHAR			pAddr,
	IN	UCHAR			apidx,
	IN BOOLEAN	CleanAll) ;
extern VOID (*AssocParmFill)(
	IN PRTMP_ADAPTER pAd, 
	IN OUT MLME_ASSOC_REQ_STRUCT *AssocReq, 
	IN PUCHAR                     pAddr, 
	IN USHORT                     CapabilityInfo, 
	IN ULONG                      Timeout, 
	IN USHORT                     ListenIntv);
extern VOID (*DisassocParmFill)(
	IN PRTMP_ADAPTER pAd, 
	IN OUT MLME_DISASSOC_REQ_STRUCT *DisassocReq, 
	IN PUCHAR pAddr, 
	IN USHORT Reason);
extern BOOLEAN (*MacTableDeleteEntry)(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT wcid,
	IN PUCHAR pAddr);
extern BOOLEAN (*RTMPCheckEtherType)(
	IN PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket);
extern VOID (*Update_Rssi_Sample)(
	IN PRTMP_ADAPTER	pAd,
	IN RSSI_SAMPLE		*pRssi,
	IN PRXWI_STRUC		pRxWI);

extern VOID (*Indicate_Legacy_Packet)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern VOID (*CmmRxnonRalinkFrameIndicate)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern VOID (*Indicate_EAPOL_Packet)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern VOID (*RTMPWriteTxWI)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PTXWI_STRUC 	pOutTxWI,	
	IN	BOOLEAN			FRAG,	
	IN	BOOLEAN			CFACK,
	IN	BOOLEAN			InsTimestamp,
	IN	BOOLEAN 		AMPDU,
	IN	BOOLEAN 		Ack,
	IN	BOOLEAN 		NSeq,		// HW new a sequence.
	IN	UCHAR			BASize,
	IN	UCHAR			WCID,
	IN	ULONG			Length,
	IN	UCHAR 			PID,
	IN	UCHAR			TID,
	IN	UCHAR			TxRate,
	IN	UCHAR			Txopmode,	
	IN	BOOLEAN			CfAck,	
	IN	HTTRANSMIT_SETTING	*pTransmit);
/* START: New added */
extern void (*AP_QueuePsActionPacket)(
		IN RTMP_ADAPTER 	*pAd,
		IN MAC_TABLE_ENTRY 	*pMacEntry,
		IN PNDIS_PACKET 	pPacket,
		IN BOOLEAN 			FlgIsDeltsFrame,
		IN BOOLEAN 			FlgIsLocked,
		IN UCHAR 			MgmtQid);
extern VOID (*tr_tb_reset_entry)(
		RTMP_ADAPTER 	*pAd, 
		UCHAR 			tr_tb_idx);
VOID tr_tb_set_entry(
		RTMP_ADAPTER 	*pAd, 
		UCHAR 			tr_tb_idx, 
		MAC_TABLE_ENTRY *pEntry);
VOID tr_tb_set_mcast_entry(
		RTMP_ADAPTER 	*pAd,
		UCHAR 			tr_tb_idx,
		struct wifi_dev *wdev);
VOID mgmt_tb_set_mcast_entry(
		RTMP_ADAPTER 	*pAd, 
		UCHAR 			wcid);
VOID set_entry_phy_cfg(
		RTMP_ADAPTER 	*pAd,
		MAC_TABLE_ENTRY *pEntry);
extern VOID (*RTMP_BASetup)(
		RTMP_ADAPTER 	*pAd,
		STA_TR_ENTRY 	*tr_entry,
		UINT8 			UPriority);
#ifdef CONFIG_AP_SUPPORT
extern INT (*deq_mgmt_frame)(
		RTMP_ADAPTER 	*pAd,
		PNDIS_PACKET 	pkt,
		UCHAR 			qIdx,
		BOOLEAN 		bLocked);
#endif
VOID rtmp_sta_txq_dump(
		RTMP_ADAPTER 	*pAd,
		STA_TR_ENTRY 	*tr_entry,
		INT 			qidx);
VOID rtmp_tx_swq_dump(
		RTMP_ADAPTER 	*pAd,
		INT 			qidx);
extern INT (*rtmp_tx_swq_init)(RTMP_ADAPTER *pAd);
extern INT (*rtmp_tx_swq_exit)(
		RTMP_ADAPTER 	*pAd,
		UCHAR 			wcid);
extern INT (*rtmp_enq_req)(
		RTMP_ADAPTER 		*pAd,
		PNDIS_PACKET 		pkt, 
		UCHAR 				qidx, 
		STA_TR_ENTRY 		*tr_entry,
		BOOLEAN 			FlgIsLocked,
		QUEUE_HEADER 		*pPktQueue);
extern INT (*rtmp_deq_req)(
		RTMP_ADAPTER 		*pAd,
		INT 				cnt,
		struct dequeue_info *info);
extern INT (*rtmp_deq_report)(
		RTMP_ADAPTER 		*pAd,
		struct dequeue_info *info);
BOOLEAN CanDoAggregateTransmit(
		RTMP_ADAPTER 	*pAd,
		NDIS_PACKET 	*pPacket,
		TX_BLK 			*pTxBlk);
extern INT (*deq_packet_gatter)(
		RTMP_ADAPTER 		*pAd,
		struct dequeue_info *deq_info,
		TX_BLK 				*pTxBlk,
		BOOLEAN 			in_hwIRQ);
/*Entry point for DeQueuePacket*/
extern VOID (*RTMPDeQueuePacket)(
		IN RTMP_ADAPTER *pAd,
		IN BOOLEAN 		in_hwIRQ,
		IN UCHAR 		QIdx,
		IN INT 			wcid,
		IN INT 			max_cnt);
INT sta_rx_fwd_hnd(
		RTMP_ADAPTER 	*pAd, 
		struct wifi_dev *wdev,
		PNDIS_PACKET 	pPacket);
INT sta_rx_pkt_allow(
		RTMP_ADAPTER 	*pAd, 
		RX_BLK 			*pRxBlk);

/*END: New added */


// wsc_tlv_rom.c

// sync.c
extern VOID (*MlmeScanReqAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeJoinReqAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeStartReqAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerBeaconAtScanAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerBeaconAtJoinAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerBeacon)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerProbeReqAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*BeaconTimeoutAtJoinAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*ScanTimeoutAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenScan)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenJoin)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenStart)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*SyncStateMachineInit)(
	IN PRTMP_ADAPTER pAd, 
	IN STATE_MACHINE *Sm, 
	OUT STATE_MACHINE_FUNC Trans[]);
extern VOID (*BeaconTimeout)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*ScanTimeout)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*EnqueuePsPoll)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*BuildEffectedChannelList)(
	IN PRTMP_ADAPTER pAd);
VOID DeleteEffectedChannelList(
	IN PRTMP_ADAPTER pAd);
extern VOID (*CntlChannelWidth)(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			PrimaryChannel,
	IN UCHAR			CentralChannel,	
	IN UCHAR			ChannelWidth,
	IN UCHAR			SecondaryChannelOffset);

// sanity.c
extern BOOLEAN (*MlmeStartReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT CHAR Ssid[], 
    OUT UCHAR *pSsidLen);
extern BOOLEAN (*PeerAssocRspSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *pMsg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT USHORT *pCapabilityInfo, 
    OUT USHORT *pStatus, 
    OUT USHORT *pAid, 
    OUT UCHAR SupRate[], 
    OUT UCHAR *pSupRateLen,
    OUT UCHAR ExtRate[], 
    OUT UCHAR *pExtRateLen,
    OUT HT_CAPABILITY_IE		*pHtCapability,
    OUT ADD_HT_INFO_IE		*pAddHtInfo,	// AP might use this additional ht info IE 
    OUT UCHAR			*pHtCapabilityLen,
    OUT UCHAR			*pAddHtInfoLen,
    OUT UCHAR			*pNewExtChannelOffset,
    OUT PEDCA_PARM pEdcaParm,
    OUT UCHAR *pCkipFlag);
extern BOOLEAN (*PeerProbeReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2,
    OUT CHAR Ssid[], 
    OUT UCHAR *pSsidLen);

// connect.c
extern VOID (*MlmeCntlInit)(
	IN PRTMP_ADAPTER pAd, 
	IN STATE_MACHINE *S, 
	OUT STATE_MACHINE_FUNC Trans[]);
extern VOID (*MlmeCntlMachinePerformAction)(
	IN PRTMP_ADAPTER pAd, 
	IN STATE_MACHINE *S, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlIdleProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlOidScanProc)(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlOidSsidProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM * Elem);
VOID CntlWscSsidProc(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM * Elem);
VOID CntlWscIterate(
	IN PRTMP_ADAPTER pAd);
extern VOID (*CntlOidRTBssidProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM * Elem);
extern VOID (*CntlMlmeRoamingProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitDisassocProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitJoinProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitStartProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitAuthProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitAuthProc2)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitAssocProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitReassocProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
//VOID AdhocTurnOnQos(
//	IN  PRTMP_ADAPTER pAd);
extern VOID (*LinkUp)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR BssType);
extern VOID (*LinkDown)(
	IN PRTMP_ADAPTER pAd,
	IN  BOOLEAN      IsReqFromAP);
extern VOID (*IterateOnBssTab)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*IterateOnBssTab2)(
	IN PRTMP_ADAPTER pAd);
VOID JoinParmFill(
	IN PRTMP_ADAPTER pAd, 
	IN OUT MLME_JOIN_REQ_STRUCT *JoinReq, 
	IN ULONG BssIdx);
extern VOID (*ScanParmFill)(
	IN PRTMP_ADAPTER pAd, 
	IN OUT MLME_SCAN_REQ_STRUCT *ScanReq, 
	IN CHAR Ssid[], 
	IN UCHAR SsidLen, 
	IN UCHAR BssType, 
	IN UCHAR ScanType);
extern VOID (*StartParmFill)(
	IN PRTMP_ADAPTER pAd, 
	IN OUT MLME_START_REQ_STRUCT *StartReq, 
	IN CHAR Ssid[], 
	IN UCHAR SsidLen);
VOID AuthParmFill(
	IN PRTMP_ADAPTER pAd, 
	IN OUT MLME_AUTH_REQ_STRUCT *AuthReq, 
	IN PUCHAR pAddr, 
	IN USHORT Alg);
extern VOID (*ComposePsPoll)(
	IN	RTMP_ADAPTER *pAd,
	IN	PPSPOLL_FRAME pPsPollFrame,
	IN	USHORT	Aid,
	IN	UCHAR *pBssid,
	IN	UCHAR *pTa);
extern VOID (*ComposeNullFrame)(
		RTMP_ADAPTER *pAd,	
		PHEADER_802_11 pNullFrame,	
		UCHAR *pAddr1,	
		UCHAR *pAddr2,	
		UCHAR *pAddr3);
extern VOID (*InitChannelRelatedValue)(
	IN PRTMP_ADAPTER pAd);

#if 1 // cmm_wpa.c
__romtext VOID WpaDisassocApAndBlockAssoc(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3);

__romtext VOID	RTMPReportMicError(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PCIPHER_KEY 	pWpaKey);

__romtext void inc_byte_array(UCHAR *counter, int len);

extern __romtext VOID (*WpaMicFailureReportFrame)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR *Msg,
	IN UCHAR *CurrentAddress,
	IN UCHAR *Bssid);

extern __romtext VOID (*RTMPMakeRsnIeCipher)(
	IN  PRTMP_ADAPTER   pAd,
	IN	UCHAR			ElementID,	
	IN	UINT			WepStatus,
	IN	UCHAR			apidx,
	IN	BOOLEAN			bMixCipher,
	IN	UCHAR			FlexibleCipher,
	OUT	PUCHAR			pRsnIe,
	OUT	UCHAR			*rsn_len);

extern __romtext VOID (*RTMPMakeRsnIeAKM)(	
	IN  PRTMP_ADAPTER   pAd,	
	IN	UCHAR			ElementID,	
	IN	UINT			AuthMode,
	IN	UCHAR			apidx,
	OUT	PUCHAR			pRsnIe,
	OUT	UCHAR			*rsn_len);

__romtext VOID RTMPMakeRsnIeCap(	
	IN  PRTMP_ADAPTER   pAd,	
	IN	UCHAR			ElementID,
	IN	UCHAR			apidx,
	OUT	PUCHAR			pRsnIe,
	OUT	UCHAR			*rsn_len);


__romtext  void inc_iv_byte(UCHAR *iv, UINT len, UINT cnt);

__romtext BOOLEAN WpaMsgTypeSubst(
	IN UCHAR EAPType,
	OUT INT *MsgType);

__romtext VOID WpaEAPPacketAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem); 

__romtext VOID WpaEAPOLASFAlertAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem); 

__romtext VOID WpaEAPOLLogoffAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem); 

__romtext VOID WpaEAPOLStartAction(
    IN PRTMP_ADAPTER    pAd, 
    IN MLME_QUEUE_ELEM  *Elem);

__romtext VOID WpaEAPOLKeyAction(
    IN PRTMP_ADAPTER    pAd, 
    IN MLME_QUEUE_ELEM  *Elem);

__romtext VOID PeerPairMsg1Action(
	IN PRTMP_ADAPTER    pAd, 
    IN MAC_TABLE_ENTRY  *pEntry,
    IN MLME_QUEUE_ELEM  *Elem) ;

__romtext VOID PeerPairMsg2Action(
	IN PRTMP_ADAPTER    pAd, 
    IN MAC_TABLE_ENTRY  *pEntry,
    IN MLME_QUEUE_ELEM  *Elem) ;

__romtext VOID PeerPairMsg3Action(
	IN PRTMP_ADAPTER    pAd, 
    IN MAC_TABLE_ENTRY  *pEntry,
    IN MLME_QUEUE_ELEM  *Elem) ;

__romtext VOID PeerPairMsg4Action(
	IN PRTMP_ADAPTER    pAd, 
    IN MAC_TABLE_ENTRY  *pEntry,
    IN MLME_QUEUE_ELEM  *Elem) ;

__romtext VOID PeerGroupMsg1Action(
        IN PRTMP_ADAPTER    pAd, 
        IN MAC_TABLE_ENTRY  *pEntry,
        IN MLME_QUEUE_ELEM  *Elem) ;
__romtext VOID	PeerGroupMsg2Action(
    IN PRTMP_ADAPTER    pAd, 
    IN MAC_TABLE_ENTRY  *pEntry,
    IN VOID             *Msg,
    IN UINT             MsgLen) ;

__romtext VOID	ConstructEapolMsg(
	IN 	PMAC_TABLE_ENTRY	pEntry,
    IN 	UCHAR				GroupKeyWepStatus,
    IN 	UCHAR				MsgType,  
    IN	UCHAR				DefaultKeyIdx,
	IN 	UCHAR				*KeyNonce,
	IN	UCHAR				*TxRSC,
	IN	UCHAR				*GTK,
	IN	UCHAR				*RSNIE,
	IN	UCHAR				RSNIE_Len,
    OUT PEAPOL_PACKET       pMsg);

__romtext VOID WpaDerivePTK(
	IN	PRTMP_ADAPTER	pAd, 
	IN	UCHAR	*PMK,
	IN	UCHAR	*ANonce,
	IN	UCHAR	*AA,
	IN	UCHAR	*SNonce,
	IN	UCHAR	*SA,
	OUT	UCHAR	*output,
	IN	UINT	len);

__romtext VOID WpaDeriveGTK(
    IN  UCHAR   *GMK,
    IN  UCHAR   *GNonce,
    IN  UCHAR   *AA,
    OUT UCHAR   *output,
    IN  UINT    len);

__romtext VOID RTMPInsertRSNIE(
	IN PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN PUINT8 rsnie_ptr,
	IN UINT8  rsnie_len,
	IN PUINT8 pmkid_ptr,
	IN UINT8  pmkid_len);
 
__romtext VOID WPASetKeyToDriver(PRTMP_ADAPTER    pAd,UINT_8 keyIndex,UCHAR keyType,PCIPHER_KEY pCipherKey,UINT_8 *Addr);


__romtext VOID WPAInstallPairwiseKey(
	PRTMP_ADAPTER		pAd,
	PMAC_TABLE_ENTRY	pEntry,
	BOOLEAN				bAE);

__romtext VOID WPAInstallSharedKey(
	PRTMP_ADAPTER	    pAd,
	UINT8				GroupCipher,
	UINT8				BssIdx,
	UINT8				KeyIdx,
	BOOLEAN				bAE,
	PUINT8				pGtk,
	UINT8				GtkLen);

extern __romtext VOID (*WpaStateMachineInit)(
    PRTMP_ADAPTER   pAd, 
    IN  STATE_MACHINE *S, 
    OUT STATE_MACHINE_FUNC Trans[]) ;

__romtext VOID	ConstructEapolKeyData(
	IN	PMAC_TABLE_ENTRY	pEntry,
	IN	UCHAR			GroupKeyWepStatus,	
	IN	UCHAR			keyDescVer,
	IN 	UCHAR			MsgType,
	IN	UCHAR			DefaultKeyIdx,
	IN	UCHAR			*GTK,
	IN	UCHAR			*RSNIE,
	IN	UCHAR			RSNIE_LEN,
	OUT PEAPOL_PACKET   pMsg);

__romtext VOID	PRF(
	IN	UCHAR	*key,
	IN	INT		key_len,
	IN	UCHAR	*prefix,
	IN	INT		prefix_len,
	IN	UCHAR	*data,
	IN	INT		data_len,
	OUT	UCHAR	*output,
	IN	INT		len);

__romtext VOID	GenRandom(
	IN	UCHAR			*macAddr,
	OUT	UCHAR			*random);

__romtext VOID WPA_ConstructKdeHdr(
	IN 	UINT8	data_type,	
	IN 	UINT8 	data_len,
	OUT PUCHAR 	pBuf);

__romtext PSTRING GetEapolMsgType(CHAR msg);

extern __romtext VOID (*RTMPMakeRSNIE)(
	IN  PRTMP_ADAPTER   pAd,	
	IN  UINT            AuthMode,
	IN  UINT            WepStatus,
	IN	UCHAR		apidx);

extern __romtext BOOLEAN (*RTMPCheckWPAframe)(
    IN PUCHAR           pData,
    IN ULONG            DataByteCount);


BOOLEAN RTMPParseEapolKeyData(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pKeyData,
	IN  UCHAR           KeyDataLen,
	IN	UCHAR			GroupKeyIndex,
	IN	UCHAR			MsgType,
	IN	BOOLEAN			bWPA2,
	IN  MAC_TABLE_ENTRY *pEntry);

__romtext VOID	CalculateMIC(
	IN	UCHAR			PeerWepStatus,
	IN	UCHAR			*PTK,
	OUT PEAPOL_PACKET   pMsg);

__romtext UCHAR	RTMPExtractKeyIdxFromIVHdr(	
	IN	PUCHAR			pIV,
	IN	UINT8			CipherAlg);

__romtext VOID   RTMPToWirelessSta(
    IN  PRTMP_ADAPTER   	pAd,
    IN  PMAC_TABLE_ENTRY 	pEntry,
    IN  PUCHAR          	pHeader802_3,
    IN  UINT            	HdrLen,
    IN  PUCHAR          	pData,
    IN  UINT            	DataLen,
    IN	BOOLEAN				bClearFrame);

__romtext VOID WPAStart4WayHS(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MAC_TABLE_ENTRY *pEntry,
	IN	ULONG			TimeInterval);

__romtext VOID WPAStart2WayGroupHS(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MAC_TABLE_ENTRY *pEntry);

extern __romtext VOID (*EnqueueStartForPSKExec)(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3); 

/* START: New added */
extern VOID (*MlmeDeAuthAction)(
    IN PRTMP_ADAPTER    pAd,
    IN MAC_TABLE_ENTRY  *pEntry,
	IN USHORT           Reason,
	IN BOOLEAN          bDataFrameFirst);

/* END: New added */
#endif


// cmm_sync.c
extern VOID (*BuildChannelList)(
	IN PRTMP_ADAPTER pAd);
extern UCHAR (*FirstChannel)(
	IN PRTMP_ADAPTER pAd);
extern UCHAR (*NextChannel)(
	IN PRTMP_ADAPTER pAd, 
	IN UCHAR channel);
extern VOID (*ChangeToCellPowerLimit)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR         AironetCellPowerLimit);
extern CHAR (*ConvertToRssi)(
	IN PRTMP_ADAPTER pAd,
	IN CHAR Rssi,
	IN  UCHAR   RssiNumber);
extern VOID (*ScanNextChannel)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*Handle_BSS_Width_Trigger_Events)(
	RTMP_ADAPTER *pAd,
	UCHAR Channel);

// cmm_sanity.c
extern BOOLEAN (*MlmeAddBAReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2);

extern BOOLEAN (*MlmeDelBAReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen);

extern BOOLEAN (*PeerAddBAReqActionSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *pMsg, 
    IN ULONG MsgLen,
	OUT PUCHAR pAddr2);
extern BOOLEAN (*PeerAddBARspActionSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *pMsg, 
    IN ULONG MsgLen);
extern BOOLEAN (*PeerDelBAActionSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN UCHAR Wcid, 
    IN VOID *pMsg, 
    IN ULONG MsgLen );
extern BOOLEAN (*MlmeScanReqSanity)(
	IN PRTMP_ADAPTER pAd, 
	IN VOID *Msg, 
	IN ULONG MsgLen, 
	OUT UCHAR *pBssType, 
	OUT CHAR Ssid[], 
	OUT UCHAR *pSsidLen, 
	OUT UCHAR *pScanType);
extern UCHAR (*ChannelSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN UCHAR channel);
extern BOOLEAN (*PeerDeauthSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT USHORT *pReason);
extern BOOLEAN (*PeerAuthSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr, 
    OUT USHORT *pAlg, 
    OUT USHORT *pSeq, 
    OUT USHORT *pStatus, 
    UCHAR *pChlgText);
extern BOOLEAN (*MlmeAuthReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr, 
    OUT ULONG *pTimeout, 
    OUT USHORT *pAlg);
extern BOOLEAN (*MlmeAssocReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pApAddr, 
    OUT USHORT *pCapabilityInfo, 
    OUT ULONG *pTimeout, 
    OUT USHORT *pListenIntv);
extern BOOLEAN (*PeerDisassocSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT USHORT *pReason);

NDIS_802_11_NETWORK_TYPE NetworkTypeInUseSanity(
    IN PBSS_ENTRY pBss);

BOOLEAN MyPeerBeaconAndProbeRspSanity(
    IN PRTMP_ADAPTER pAd,
    IN VOID *Msg, 
    IN ULONG MsgLen, 
	IN OUT BeaconProbeRspStr_t *pRsp);
extern BOOLEAN (*PeerBeaconAndProbeRspSanity2)(
	IN PRTMP_ADAPTER pAd, 
	IN VOID *Msg, 
	IN ULONG MsgLen, 
	IN OVERLAP_BSS_SCAN_IE *BssScan,
	OUT UCHAR 	*RegClass);
/* START: New added */
extern BOOLEAN (*PeerBeaconAndProbeRspSanity)(
	IN PRTMP_ADAPTER pAd,
	IN VOID *Msg, 
	IN ULONG MsgLen, 
	IN UCHAR  MsgChannel,
	OUT BCN_IE_LIST *ie_list,
	OUT USHORT *LengthVIE,	
	OUT PNDIS_802_11_VARIABLE_IEs pVIE,
	IN BOOLEAN bGetDtim);
/* END: New added */

// auth.c
extern void (*AuthStateMachineInit)(
    IN PRTMP_ADAPTER pAd, 
    IN STATE_MACHINE *Sm, 
    OUT STATE_MACHINE_FUNC Trans[]);
extern VOID (*MlmeAuthReqAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenAuth)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerAuthRspAtSeq2Action)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*AuthTimeoutAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerAuthRspAtSeq4Action)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
VOID MlmeDeauthReqAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern void (*AuthStateMachineInit)(
    IN PRTMP_ADAPTER pAd, 
    IN STATE_MACHINE *Sm, 
    OUT STATE_MACHINE_FUNC Trans[]);
extern VOID (*Cls2errAction)(
    IN PRTMP_ADAPTER pAd, 
    IN PUCHAR pAddr);
extern VOID (*AuthTimeout)(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3);

// auth_rsp.c
extern VOID (*AuthRspStateMachineInit)(
    IN PRTMP_ADAPTER pAd, 
    IN PSTATE_MACHINE Sm, 
    IN STATE_MACHINE_FUNC Trans[]);
extern VOID (*PeerDeauthAction)(
    IN PRTMP_ADAPTER pAd, 
    IN PMLME_QUEUE_ELEM Elem);

// assoc.c
extern VOID (*MlmeDisassocReqAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeAssocReqAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeReassocReqAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerAssocRspAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerReassocRspAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerDisassocAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*DisassocTimeoutAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*AssocTimeoutAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID  (*ReassocTimeoutAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);


extern VOID (*AssocStateMachineInit)(
	IN	PRTMP_ADAPTER	pAd, 
	IN  STATE_MACHINE *S, 
	OUT STATE_MACHINE_FUNC Trans[]); 
extern VOID (*AssocTimeout)(IN PVOID SystemSpecific1, 
				 IN PVOID FunctionContext, 
				 IN PVOID SystemSpecific2, 
				 IN PVOID SystemSpecific3);
extern VOID (*ReassocTimeout)(IN PVOID SystemSpecific1, 
					IN PVOID FunctionContext, 
					IN PVOID SystemSpecific2, 
					IN PVOID SystemSpecific3);
extern VOID (*DisassocTimeout)(IN PVOID SystemSpecific1, 
					IN PVOID FunctionContext, 
					IN PVOID SystemSpecific2, 
					IN PVOID SystemSpecific3);
extern VOID (*cmm_assoc_reassoc_action)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem,
	IN BOOLEAN	isReassoc);
extern VOID (*AssocPostProc)(
	IN PRTMP_ADAPTER pAd, 
	IN PUCHAR pAddr2, 
	IN USHORT CapabilityInfo, 
	IN USHORT Aid, 
	IN UCHAR SupRate[], 
	IN UCHAR SupRateLen,
	IN UCHAR ExtRate[],
	IN UCHAR ExtRateLen,
	IN PEDCA_PARM pEdcaParm,
	IN HT_CAPABILITY_IE		*pHtCapability,
	IN UCHAR HtCapabilityLen, 
	IN ADD_HT_INFO_IE		*pAddHtInfo);
extern VOID (*InvalidStateWhenAssoc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenReassoc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenDisassociate)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*StaAddMacTableEntry)(
	IN  PRTMP_ADAPTER		pAd,
	IN  PMAC_TABLE_ENTRY	pEntry,
	IN  UCHAR				MaxSupportedRateIn500Kbps,
	IN  HT_CAPABILITY_IE	*pHtCapability,
	IN  UCHAR				HtCapabilityLen,
	IN  ADD_HT_INFO_IE		*pAddHtInfo,
	IN  UCHAR				AddHtInfoLen,
	IN  USHORT        		CapabilityInfo);
/*START: New added */
extern VOID (*cmm_invalid_state)(
		IN PRTMP_ADAPTER	pAd, 
		IN MLME_QUEUE_ELEM 	*Elem,
		IN ULONG 			MsgType);
#ifdef WPA_SUPPLICANT_SUPPORT
#ifndef NATIVE_WPA_SUPPLICANT_SUPPORT
VOID SendAssocIEsToWpaSupplicant( 
	    IN  PRTMP_ADAPTER 	pAd);
#endif
#endif
#ifdef WPA_SUPPLICANT_SUPPORT
int wext_notify_event_assoc(
	IN PNET_DEV pNetDev,
	IN UCHAR *ReqVarIEs,
	IN UINT32 ReqVarIELen);
#endif
/*END: New added */


// action.c
extern VOID (*ActionStateMachineInit)(
    IN	PRTMP_ADAPTER	pAd, 
    IN  STATE_MACHINE *S, 
    OUT STATE_MACHINE_FUNC Trans[]);

extern VOID (*PeerSpectrumAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

extern VOID (*PeerPublicAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

extern VOID (*PeerBAAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

extern VOID (*PeerHTAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem); 

extern VOID (*MlmeADDBAAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeDELBAAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerQOSAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeQOSAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeDLSAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeInvalidAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem); 
/* STAR: New added */
extern BOOLEAN (*PeerChSwAnnSanity)(
	IN PRTMP_ADAPTER pAd,
	IN VOID *pMsg,
	IN ULONG MsgLen,
	OUT PCH_SW_ANN_INFO pChSwAnnInfo);
extern VOID (*PeerChSwAnnAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
#ifdef CONFIG_AP_SUPPORT
#ifdef QOS_DLS_SUPPORT
VOID PeerDLSAction(
		IN PRTMP_ADAPTER pAd,
		IN MLME_QUEUE_ELEM *Elem);
#endif
#endif
extern VOID (*ReservedAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*respond_ht_information_exchange_action)(
		IN PRTMP_ADAPTER pAd,
		IN MLME_QUEUE_ELEM *Elem); 
#ifdef CONFIG_AP_SUPPORT
#ifdef DOT11N_DRAFT3
extern VOID (*SendNotifyBWActionFrame)(RTMP_ADAPTER *pAd, UCHAR Wcid, UCHAR apidx);
//VOID ApPublicAction(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID SendBSS2040CoexistMgmtAction(
		IN RTMP_ADAPTER *pAd,
		IN UCHAR Wcid,
		IN UCHAR apidx,
		IN UCHAR InfoReq);
#endif
#endif
VOID ORIBATimerTimeout(
	IN	PRTMP_ADAPTER	pAd);
VOID SendRefreshBAR(
		IN	PRTMP_ADAPTER	pAd,
		IN	MAC_TABLE_ENTRY *pEntry);
extern VOID (*ActHeaderInit)(
		IN	PRTMP_ADAPTER	pAd, 
		IN OUT PHEADER_802_11 pHdr80211, 
		IN PUCHAR Addr1, 
		IN PUCHAR Addr2,
		IN PUCHAR Addr3);
extern VOID (*BarHeaderInit)(
		IN	PRTMP_ADAPTER	pAd, 
		IN OUT PFRAME_BAR pCntlBar, 
		IN PUCHAR pDA,
		IN PUCHAR pSA); 
extern VOID (*StaPublicAction)(
		IN PRTMP_ADAPTER pAd, 
		IN BSS_2040_COEXIST_IE *pBssCoexIE);
extern ULONG (*BuildIntolerantChannelRep)(
		IN	PRTMP_ADAPTER	pAd,
		IN	  PUCHAR  pDest);
extern VOID (*Update2040CoexistFrameAndNotify)(
		IN	PRTMP_ADAPTER	pAd,
		IN	  UCHAR  Wcid,
		IN	BOOLEAN bAddIntolerantCha);
extern VOID (*Send2040CoexistAction)(
		IN	PRTMP_ADAPTER	pAd,
		IN	  UCHAR  Wcid,
		IN	BOOLEAN bAddIntolerantCha);
extern VOID (*UpdateBssScanParm)(
		IN	PRTMP_ADAPTER	pAd,
		IN	OVERLAP_BSS_SCAN_IE APBssScan);
/* END: New added */


// cmm_info.c
extern VOID	(*RTMPWPARemoveAllKeys)(
	IN	PRTMP_ADAPTER	pAd);
extern VOID (*RTMPSetPhyMode)(
	IN	PRTMP_ADAPTER	pAd,
	IN	ULONG phymode);
extern VOID (*RTMPSetHT)(
	IN	PRTMP_ADAPTER	pAd,
	IN	OID_SET_HT_PHYMODE *pHTPhyMode);
extern VOID (*RTMPUpdateHTIE)(
	  IN  RT_HT_CAPABILITY    *pRtHt,
	  OUT     HT_CAPABILITY_IE *pHtCapability,
	  OUT     ADD_HT_INFO_IE      *pAddHtInfo);
extern VOID (*RTMPAddWcidAttributeEntry)(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			BssIdx,
	IN 	UCHAR		 	KeyIdx,
	IN 	UCHAR		 	CipherAlg,
	IN 	MAC_TABLE_ENTRY *pEntry);
extern INT	(*SetCommonHT)(IN PRTMP_ADAPTER	pAd);
void  getRate(HTTRANSMIT_SETTING HTSetting, ULONG* fLastTxRxRate);

VOID RTMPSetWcidSecurityInfo(
	RTMP_ADAPTER		*pAd,
	UINT8				BssIdx,
	UINT8				KeyIdx,
	UINT8				CipherAlg,
	UINT8				Wcid,
	UINT8				KeyTabFlag);

#ifdef IOT_PROMISCUS_MODE
extern INT (*Set_MonitorMode_Proc)(RTMP_ADAPTER *pAd, UINT8 arg);
#endif

// mlme.c
extern NDIS_STATUS (*MlmeInit)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeHandler)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeHalt)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeResetRalinkCounters)(
	IN  PRTMP_ADAPTER   pAd);
extern VOID (*MlmeSelectTxRateTable)(
	IN PRTMP_ADAPTER        pAd,
	IN PMAC_TABLE_ENTRY		pEntry,
	IN PUCHAR               *ppTable,
	IN PUCHAR               pTableSize,
	IN PUCHAR               pInitTxRateIdx);
extern VOID (*MlmePeriodicExec)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID AsicStaBbpTuning(IN PRTMP_ADAPTER pAd);
extern VOID (*STAMlmePeriodicExec)(PRTMP_ADAPTER pAd);
VOID LinkDownExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*MlmeAutoScan)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeAutoReconnectLastSSID)(
	IN PRTMP_ADAPTER pAd);
extern BOOLEAN (*MlmeValidateSSID)(
	IN CHAR	*pSsid,
	IN UCHAR	SsidLen);
extern VOID (*MlmeCheckForFastRoaming)(
	IN	PRTMP_ADAPTER	pAd,
	IN	ULONG		Now);
extern VOID (*MlmeCalculateChannelQuality)(
	IN PRTMP_ADAPTER pAd,
	IN ULONG Now32);
extern VOID (*MlmeCheckPsmChange)(
	IN PRTMP_ADAPTER pAd,
	IN ULONG	Now32);
VOID MlmeSetTxRate(
  IN PRTMP_ADAPTER        pAd,
  IN PMAC_TABLE_ENTRY     pEntry,
  IN RTMP_RA_LEGACY_TB *pTxRate);
VOID MlmeDynamicTxRateSwitching(
	IN PRTMP_ADAPTER pAd);
extern VOID (*StaQuickResponeForRateUpExec)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*MlmeSetPsmBit)(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT psm);
extern VOID (*MlmeSetTxPreamble)(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT TxPreamble);
extern VOID (*UpdateBasicRateBitmap)(
    IN  PRTMP_ADAPTER   pAdapter);
extern BOOLEAN (*MlmeEnqueue)(
	IN	PRTMP_ADAPTER	pAd,
	IN ULONG Machine, 
	IN ULONG MsgType, 
	IN ULONG MsgLen, 
	IN VOID *Msg,
	IN ULONG priv);
extern BOOLEAN (*MlmeEnqueueForRecv)(
	IN	PRTMP_ADAPTER	pAd, 
	IN ULONG Wcid, 
	IN ULONG TimeStampHigh, 
	IN ULONG TimeStampLow,
	IN UCHAR Rssi0, 
	IN UCHAR Rssi1, 
	IN UCHAR Rssi2, 
	IN ULONG MsgLen, 
	IN VOID *Msg,
	IN UCHAR Signal);
ULONG MakeIbssBeacon(
	IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeUpdateTxRates)(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN		 bLinkUp,
	IN UCHAR apidx);
extern VOID (*MlmeUpdateHtTxRates)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 Wcid);
extern VOID (*BssTableInit)(
	IN BSS_TABLE *Tab);
extern VOID (*BATableInit)(
	IN PRTMP_ADAPTER pAd, 
    IN BA_TABLE *Tab);
extern ULONG (*BssTableSearch)(
	IN BSS_TABLE *Tab, 
	IN PUCHAR	 pBssid,
	IN UCHAR	 Channel);
extern ULONG (*BssSsidTableSearch)(
	IN BSS_TABLE *Tab, 
	IN PUCHAR	 pBssid,
	IN CHAR	     *pSsid,
	IN UCHAR	 SsidLen,
	IN UCHAR	 Channel);
ULONG BssTableSearchWithSSID(
	IN BSS_TABLE *Tab, 
	IN PUCHAR	 Bssid,
	IN CHAR	     *pSsid,
	IN UCHAR	 SsidLen,
	IN UCHAR	 Channel);
extern VOID (*BssTableDeleteEntry)(
	IN OUT	BSS_TABLE *Tab, 
	IN		PUCHAR	  pBssid,
	IN		UCHAR	  Channel);
VOID BssEntrySet(
	IN PRTMP_ADAPTER	pAd, 
	OUT BSS_ENTRY *pBss, 
	IN PUCHAR pBssid, 
	IN CHAR Ssid[], 
	IN UCHAR SsidLen, 
	IN UCHAR BssType, 
	IN USHORT BeaconPeriod, 
	IN PCF_PARM pCfParm, 
	IN USHORT AtimWin, 
	IN USHORT CapabilityInfo, 
	IN UCHAR SupRate[], 
	IN UCHAR SupRateLen,
	IN UCHAR ExtRate[], 
	IN UCHAR ExtRateLen,
	IN HT_CAPABILITY_IE *pHtCapability,
	IN ADD_HT_INFO_IE *pAddHtInfo,	// AP might use this additional ht info IE 
	IN UCHAR			HtCapabilityLen,
	IN UCHAR			AddHtInfoLen,
	IN UCHAR			NewExtChanOffset,
	IN UCHAR Channel,
	IN CHAR Rssi,
	IN LARGE_INTEGER TimeStamp,
	IN UCHAR CkipFlag,
	IN PEDCA_PARM pEdcaParm,
	IN PQOS_CAPABILITY_PARM pQosCapability,
	IN PQBSS_LOAD_PARM pQbssLoad,
	IN USHORT LengthVIE,	
	IN PNDIS_802_11_VARIABLE_IEs pVIE);
ULONG BssTableSetEntry(
	IN	PRTMP_ADAPTER	pAd, 
	OUT BSS_TABLE *Tab, 
	IN PUCHAR pBssid, 
	IN CHAR Ssid[], 
	IN UCHAR SsidLen, 
	IN UCHAR BssType, 
	IN USHORT BeaconPeriod, 
	IN CF_PARM *CfParm, 
	IN USHORT AtimWin, 
	IN USHORT CapabilityInfo, 
	IN UCHAR SupRate[],
	IN UCHAR SupRateLen,
	IN UCHAR ExtRate[],
	IN UCHAR ExtRateLen,
	IN HT_CAPABILITY_IE *pHtCapability,
	IN ADD_HT_INFO_IE *pAddHtInfo,	// AP might use this additional ht info IE 
	IN UCHAR			HtCapabilityLen,
	IN UCHAR			AddHtInfoLen,
	IN UCHAR			NewExtChanOffset,
	IN UCHAR ChannelNo,
	IN CHAR Rssi,
	IN LARGE_INTEGER TimeStamp,
	IN UCHAR CkipFlag,
	IN PEDCA_PARM pEdcaParm,
	IN PQOS_CAPABILITY_PARM pQosCapability,
	IN PQBSS_LOAD_PARM pQbssLoad,
	IN USHORT LengthVIE,	
	IN PNDIS_802_11_VARIABLE_IEs pVIE);
extern VOID (*BssTableSsidSort)(
	IN	PRTMP_ADAPTER	pAd, 
	OUT BSS_TABLE *OutTab, 
	IN	CHAR Ssid[], 
	IN	UCHAR SsidLen);
extern VOID (*BssTableSortByRssi)(
	IN OUT BSS_TABLE *OutTab);
extern VOID (*BssCipherParse_iotn9)(
	IN OUT	PBSS_ENTRY	pBss);
extern VOID (*MacAddrRandomBssid)(
	IN PRTMP_ADAPTER pAd, 
	OUT PUCHAR pAddr);
extern NDIS_STATUS (*MlmeQueueInit)(
	IN MLME_QUEUE *Queue);
BOOLEAN MlmeEnqueueForWsc(
	IN	PRTMP_ADAPTER	pAd,
	IN ULONG eventID,
	IN LONG senderID,
	IN ULONG Machine, 
	IN ULONG MsgType, 
	IN ULONG MsgLen, 
	IN VOID *Msg);
extern BOOLEAN (*MlmeDequeue)(
	IN MLME_QUEUE *Queue, 
	OUT MLME_QUEUE_ELEM **Elem);
extern VOID (*MlmeRestartStateMachine)(
	IN	PRTMP_ADAPTER	pAd);
BOOLEAN MlmeQueueEmpty(
	IN MLME_QUEUE *Queue);
extern BOOLEAN (*MlmeQueueFull)(
	IN MLME_QUEUE *Queue);
VOID MlmeQueueDestroy(
	IN MLME_QUEUE *pQueue);
extern VOID (*StateMachineInit)(
	IN STATE_MACHINE *S, 
	IN STATE_MACHINE_FUNC Trans[], 
	IN ULONG StNr, 
	IN ULONG MsgNr, 
	IN STATE_MACHINE_FUNC DefFunc, 
	IN ULONG InitState, 
	IN ULONG Base);
extern VOID (*StateMachineSetAction)(
	IN STATE_MACHINE *S, 
	IN ULONG St, 
	IN ULONG Msg, 
	IN STATE_MACHINE_FUNC Func);
extern VOID (*StateMachinePerformAction)(
	IN	PRTMP_ADAPTER	pAd, 
	IN STATE_MACHINE *S, 
	IN MLME_QUEUE_ELEM *Elem,
	IN ULONG CurrState);
extern VOID (*Drop)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*MgtMacHeaderInit)(
	IN	PRTMP_ADAPTER	pAd, 
	IN OUT PHEADER_802_11 pHdr80211, 
	IN UCHAR SubType, 
	IN UCHAR ToDs, 
	IN PUCHAR pDA, 
	IN UCHAR *pSA,
	IN PUCHAR pBssid);
extern BOOLEAN (*GetTimBit)(
    IN UCHAR *Ptr, 
    IN USHORT Aid, 
    OUT UCHAR *TimLen, 
    OUT UCHAR *BcastFlag, 
    OUT UCHAR *DtimCount, 
    OUT UCHAR *DtimPeriod,
    OUT UCHAR *MessageToMe);
extern UCHAR (*RandomByte)(
	IN PRTMP_ADAPTER pAd);
extern BOOLEAN (*MsgTypeSubst)(
	IN PRTMP_ADAPTER  pAd,
	IN PFRAME_802_11 pFrame, 
	OUT INT *Machine, 
	OUT INT *MsgType);
extern VOID (*AsicUpdateAutoFallBackTable)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			pRateTable);
extern VOID (*AsicUpdateProtect)(
	IN		PRTMP_ADAPTER	pAd,
	IN 		USHORT			OperationMode,
	IN 		UCHAR			SetMask,
	IN		BOOLEAN			bDisableBGProtect,
	IN		BOOLEAN			bNonGFExist);
VOID AsicEvaluateRxAnt(
	IN PRTMP_ADAPTER	pAd);
extern VOID (*AsicSwitchChannel)(
	IN PRTMP_ADAPTER pAd, 
	IN	UCHAR			Channel,
	IN	BOOLEAN			bScan);
VOID AsicAdjustTxPower(
	IN PRTMP_ADAPTER pAd);
extern VOID (*AsicSleepThenAutoWakeup)(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT TbttNumToNextWakeUp);
extern VOID (*AsicSetBssid)(
	IN PRTMP_ADAPTER pAd, 
	IN PUCHAR pBssid);
extern VOID (*AsicDelWcidTab)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR	Wcid);
extern VOID (*AsicEnableRDG)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*AsicDisableRDG)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*AsicDisableSync)(
	IN PRTMP_ADAPTER pAd,
	UCHAR HWBssidIdx);
extern VOID (*AsicEnableBssSync)(
	IN PRTMP_ADAPTER pAd);
VOID AsicEnableIbssSync(
	IN PRTMP_ADAPTER pAd);
extern VOID (*AsicSetEdcaParm)(
	IN PRTMP_ADAPTER pAd,
	IN PEDCA_PARM	 pEdcaParm);
extern VOID (*AsicSetSlotTime)(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN bUseShortSlotTime);
extern VOID (*AsicAddSharedKeyEntry)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 BssIndex,
	IN UCHAR		 KeyIdx,
	IN PCIPHER_KEY   pCipherKey);
extern VOID (*AsicRemoveSharedKeyEntry)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 BssIndex,
	IN UCHAR		 KeyIdx);
VOID AsicUpdateWCIDAttribute(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		WCID,
	IN UCHAR		BssIndex,
	IN UCHAR        CipherAlg,
	IN BOOLEAN		bUsePairewiseKeyTable);
extern VOID (*AsicUpdateRxWCIDTable)(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		WCID,
	IN PUCHAR        pAddr);
VOID AsicAddPairwiseKeyEntry(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR        pAddr,
	IN UCHAR		WCID,
	IN CIPHER_KEY		 *pCipherKey);
extern VOID (*AsicRemovePairwiseKeyEntry)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 BssIdx,
	IN UCHAR		 Wcid);
extern VOID (*RTMPCheckRates)(
	IN		PRTMP_ADAPTER	pAd,
	IN OUT	UCHAR			SupRate[],
	IN OUT	UCHAR			*SupRateLen);
extern BOOLEAN (*RTMPCheckChannel)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		CentralChannel,
	IN UCHAR		Channel);
extern BOOLEAN (*RTMPCheckHt)(
	IN		PRTMP_ADAPTER	pAd,
	IN		UCHAR	Wcid,
	IN OUT	HT_CAPABILITY_IE			*pHtCapability,
	IN OUT	ADD_HT_INFO_IE			*pAddHtInfo);
BOOLEAN RTMPCheckAddHtInfoIe(
	IN		PRTMP_ADAPTER	pAd,
	IN OUT	ADD_HT_INFO_IE	*pAddHTInfo);
extern VOID (*RTMPUpdateMlmeRate)(
	IN PRTMP_ADAPTER	pAd);
extern CHAR (*RTMPMaxRssi)(
	IN PRTMP_ADAPTER	pAd,
	IN CHAR				Rssi0,
	IN CHAR				Rssi1,
	IN CHAR				Rssi2);
extern VOID (*RTMPSetPiggyBack)(
    IN PRTMP_ADAPTER    pAd,
    IN BOOLEAN          bPiggyBack);
BOOLEAN RTMPCheckEntryEnableAutoRateSwitch(
	IN PRTMP_ADAPTER    pAd,
	IN PMAC_TABLE_ENTRY	pEntry);
VOID AsicSetStaAid(
	IN PRTMP_ADAPTER pAd, 
	IN UINT16 Aid);
extern VOID (*RTMPUpdateLegacyTxSetting)(
	UCHAR fixed_tx_mode,
	PMAC_TABLE_ENTRY pEntry);
extern VOID (*RTMPSetAGCInitValue)(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			BandWidth);
extern VOID (*AsicUpdateWCIDIVEIV)(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		WCID,
	IN ULONG        uIV,
	IN ULONG        uEIV);
extern VOID (*SwitchBetweenWepAndCkip)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*AsicForceWakeup)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeRadioOn)(IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeRadioOff)(IN PRTMP_ADAPTER pAd);
extern VOID (*MtAsicUpdateRxWCIDTableByDriver)(
	RTMP_ADAPTER *pAd,
	MT_WCID_TABLE_INFO_T WtblInfo);
extern INT32 (*MtAsicUpdateBASessionByDriver)(
	RTMP_ADAPTER *pAd,
	MT_BA_CTRL_T BaCtrl);
extern VOID (*AsicUpdateBASession)(
	RTMP_ADAPTER 	*pAd, 
	UCHAR 			wcid, 
	UCHAR 			tid, 
	UINT16 			sn, 
	UCHAR 			basize, 
	BOOLEAN 		isAdd, 
	INT 				ses_type);
#ifdef IOT_SECURITY_SUPPORT
extern VOID (*SecurityAddKey)(
	RTMP_ADAPTER 	*pAd,
	UCHAR 			OperMode,
	UCHAR 			CipherAlg,
	CMD_802_11_KEY CmdKey);
extern VOID (*SecurityRemoveKey)(
	PRTMP_ADAPTER 	pAd,
	UCHAR 			CipherAlg,
	CMD_802_11_KEY CmdKey);
extern VOID (*MtAsicAddRemoveKeyByDriver)(
	PRTMP_ADAPTER	pAd,
	MT_SECURITY_CTRL SecurityCtrl);
#endif
// rt_pkt.c
extern int (*net_pkt_init)(int num);
extern int (*netlib_packet_buffer_init)(void);
struct net_buf_blk * net_buf_blk_get(int idx, int size);
void net_buf_blk_put(struct net_buf_blk * buf_blk);
extern struct net_pkt_blk * (*net_pkt_clone)(struct net_pkt_blk *pkt);
int net_pkt_cloned(struct net_pkt_blk *pkt);
extern struct net_pkt_blk * (*net_pkt_alloc)(int size_idx);
extern struct net_pkt_blk * (*net_pkt_get)(int size_idx, UINT_16 u2PseFid);
extern void (*net_pkt_free)(struct net_pkt_blk * pkt_blk);
extern void (*net_pkt_free_keep_pse)(struct net_pkt_blk * pkt_blk);
extern void (*net_pkt_reserve)(struct net_pkt_blk *pkt_blk, int len);
struct net_pkt_blk * net_pkt_copy(struct net_pkt_blk *pkt_blk);
extern struct net_pkt_blk * (*net_buf_join_pkt_blk)(struct net_buf_blk * buf_blk);
extern unsigned char * (*net_pkt_put)(struct net_pkt_blk *pkt, u32 len);
extern unsigned char * (*net_pkt_push)(struct net_pkt_blk *pkt, unsigned int len);
unsigned char * net_pkt_pull(struct net_pkt_blk *pkt, unsigned int len);
extern void (*net_pkt_show)(void);
extern void (*net_enqueue_unlock)(struct head_list *list, struct node_obj *blk);
extern void (*net_enqueue)(struct head_list *list, struct node_obj *blk);
extern struct node_obj * (*net_dequeue)(struct head_list *list);
extern void (*buf_blk_ref)(struct net_buf_blk *buf_blk);

// rom_func.c
extern __romtext ULONG (*MakeOutgoingFrame)(
	OUT UCHAR *Buffer, 
	OUT ULONG *FrameLen, ...);
extern __romtext ULONG	(*RTMPCompareMemory)(
	IN	PVOID	pSrc1,
	IN	PVOID	pSrc2,
	IN	ULONG	Length);

// rtmp_tkip.c
extern __romtext ULONG	(*RTMPTkipGetUInt32)( 	
	IN	PUCHAR	pMICKey);
extern __romtext VOID	(*RTMPTkipPutUInt32)(
	IN OUT	PUCHAR		pDst,
	IN		ULONG		val);
extern __romtext VOID (*RTMPTkipSetMICKey)(  
	IN	PTKIP_KEY_INFO	pTkip,	
	IN	PUCHAR			pMICKey);
extern __romtext VOID	(*RTMPTkipAppendByte)( 
	IN	PTKIP_KEY_INFO	pTkip,	
	IN	UCHAR 			uChar);
extern __romtext VOID	(*RTMPTkipAppend)( 
	IN	PTKIP_KEY_INFO	pTkip,	
	IN	PUCHAR			pSrc,
	IN	UINT			nBytes);
extern __romtext VOID	(*RTMPTkipGetMIC)( 
	IN	PTKIP_KEY_INFO	pTkip);
__romtext VOID	RTMPInitTkipEngine(
	IN	PUCHAR			pKey,
	IN	UCHAR			KeyId,
	IN	PUCHAR			pTA,
	IN	PUCHAR			pMICKey,
	IN	PUCHAR			pTSC,
	OUT	PULONG			pIV16,
	OUT	PULONG			pIV32);
extern __romtext VOID	(*RTMPInitMICEngine)(
	IN	TKIP_KEY_INFO  	*pTx_Key,
	IN	PUCHAR			pKey,
	IN	PUCHAR			pDA,
	IN	PUCHAR			pSA,
	IN  UCHAR           UserPriority,
	IN	PUCHAR			pMICKey);
extern __romtext BOOLEAN	(*RTMPTkipCompareMICValue)(
	TKIP_KEY_INFO   	*pRx_Key,
	IN	PUCHAR			pSrc,
	IN	PUCHAR			pDA,
	IN	PUCHAR			pSA,
	IN	PUCHAR			pMICKey,
	IN	UCHAR			UserPriority,
	IN	UINT			Len);
extern VOID  (*RTMPCalculateMICValue)(
	 IN  TKIP_KEY_INFO   *pTx,
	 IN  PNDIS_PACKET    pPacket,
	 IN  PUCHAR          pEncap,
	 IN  PCIPHER_KEY     pKey,
	 IN  UCHAR           apidx);
__romtext UINT tkip_sbox(UINT index);
__romtext UINT rotr1(UINT a);
__romtext VOID RTMPTkipMixKey(
	UCHAR *key, 
	UCHAR *ta, 
	ULONG pnl, /* Least significant 16 bits of PN */
	ULONG pnh, /* Most significant 32 bits of PN */ 
	UCHAR *rc4key, 
	UINT *p1k);
__romtext void construct_mic_header1(
	unsigned char *mic_header1,
	int header_length,
	unsigned char *mpdu);
__romtext void construct_mic_header2(
	unsigned char *mic_header2,
	unsigned char *mpdu,
	int a4_exists,
	int qc_exists);
__romtext void construct_mic_iv(
	unsigned char *mic_iv,
	int qc_exists,
	int a4_exists,
	unsigned char *mpdu,
	unsigned int payload_length,
	unsigned char *pn_vector);
__romtext void bitwise_xor(unsigned char *ina, unsigned char *inb, unsigned char *out);
//__romtext void aes128k128d(unsigned char *key, unsigned char *data, unsigned char *ciphertext);
__romtext void construct_ctr_preload(
	unsigned char *ctr_preload,
	int a4_exists,
	int qc_exists,
	unsigned char *mpdu,
	unsigned char *pn_vector,
	int c);
#if 0
__romtext BOOLEAN RTMPSoftDecryptTKIP(
	IN	TKIP_KEY_INFO  	*pTx_Key,
	IN PUCHAR	pData,
	IN ULONG	DataByteCnt, 
	IN UCHAR    UserPriority,
	IN PCIPHER_KEY	pWpaKey);
__romtext BOOLEAN RTMPSoftDecryptAES(
	IN PUCHAR	pData,
	IN ULONG	DataByteCnt, 
	IN PCIPHER_KEY	pWpaKey);
#endif
__romtext VOID xor_128(
	IN  PUCHAR  a,
	IN  PUCHAR  b,
	OUT PUCHAR  out);
__romtext VOID next_key(
	IN  PUCHAR  key,
	IN  INT     round);
__romtext VOID xor_32(
	IN  PUCHAR  a,
	IN  PUCHAR  b,
	OUT PUCHAR  out);
__romtext VOID byte_sub(
	IN  PUCHAR  in,
	OUT PUCHAR  out);
__romtext UCHAR RTMPCkipSbox(
	IN  UCHAR   a);
__romtext VOID shift_row(
	IN  PUCHAR  in,
	OUT PUCHAR  out);
__romtext VOID mix_column(
	IN  PUCHAR  in,
	OUT PUCHAR  out);


#if 1// rtmp_wep.c
VOID  ARCFOUR_Compute (
    ARC4_CTX_STRUC *pARC4_CTX,
    UINT_8 InputBlock[],
    UINT_32 InputBlockSize,
    UINT_8 OutputBlock[]);

extern VOID (*RTMPInitWepEngine)(
	IN	PRTMP_ADAPTER	pAd,	
	IN	PUCHAR			pKey,
	IN	UCHAR			KeyId,
	IN	UCHAR			KeyLen, 
	IN OUT	PUCHAR		pDest);
extern __romtext VOID	(*RTMPEncryptData)(
	IN	PRIVATE_STRUC	*pPrivateInfo,	
	IN	PUCHAR			pSrc,
	IN	PUCHAR			pDest,
	IN	UINT			Len);
extern __romtext VOID	(*ARCFOUR_INIT)(
	IN	PARCFOURCONTEXT	Ctx,
	IN	PUCHAR			pKey,
	IN	UINT			KeyLen);
extern __romtext UCHAR	(*ARCFOUR_BYTE)(
	IN	PARCFOURCONTEXT		Ctx);
__romtext VOID	ARCFOUR_DECRYPT(
	IN	PARCFOURCONTEXT	Ctx,
	IN	PUCHAR			pDest, 
	IN	PUCHAR			pSrc,
	IN	UINT			Len);
extern __romtext VOID	(*ARCFOUR_ENCRYPT)(
	IN	PARCFOURCONTEXT	Ctx,
	IN	PUCHAR			pDest,
	IN	PUCHAR			pSrc,
	IN	UINT			Len);

__romtext VOID	WPAARCFOUR_ENCRYPT(
	IN	PARCFOURCONTEXT	Ctx,
	IN	PUCHAR			pDest,
	IN	PUCHAR			pSrc,
	IN	UINT			Len);
extern __romtext UINT	(*RTMP_CALC_FCS32)(
	IN	UINT	Fcs,
	IN	PUCHAR	Cp,
	IN	INT		Len);
extern __romtext VOID	(*RTMPSetICV)(
	IN	PRIVATE_STRUC	*pPrivateInfo,
	IN	PUCHAR	pDest);

extern __romtext VOID (*TKIP_GTK_KEY_WRAP)( 
    IN UCHAR    *key,
    IN UCHAR	*iv,
    IN UCHAR    *input_text,
    IN UINT32    input_len,
    OUT UCHAR   *output_text);

extern __romtext VOID (*TKIP_GTK_KEY_UNWRAP)( 
    IN UCHAR    *key,
    IN UCHAR	*iv,
    IN UCHAR    *input_text,
    IN UINT32    input_len,
    OUT UCHAR   *output_text);

__romtext VOID ARC4_Discard_KeyLength (
    IN ARC4_CTX_STRUC *pARC4_CTX,
    IN UINT Length) ;

__romtext VOID ARC4_INIT (
    IN ARC4_CTX_STRUC *pARC4_CTX,
    IN PUCHAR pKey,
	IN UINT KeyLength);
#endif


#if 0
// rtmp_ckipmic.c
VOID CKIP_key_permute
    (
     OUT UCHAR  *PK,           /* output permuted key */
     IN UCHAR *CK,           /* input CKIP key */
     IN UCHAR  toDsFromDs,    /* input toDs/FromDs bits */
     IN UCHAR *piv           /* input pointer to IV */
     );
VOID RTMPCkipMicInit(
    IN  PMIC_CONTEXT        pContext,
    IN  PUCHAR              CK);
VOID RTMPMicUpdate(
    IN  PMIC_CONTEXT        pContext,
    IN  PUCHAR              pOctets,
    IN  INT                 len);
ULONG RTMPMicGetCoefficient(
    IN  PMIC_CONTEXT         pContext);
VOID xor_128(
    IN  PUCHAR  a,
    IN  PUCHAR  b,
    OUT PUCHAR  out);
UCHAR RTMPCkipSbox(
    IN  UCHAR   a);
VOID xor_32(
    IN  PUCHAR  a,
    IN  PUCHAR  b,
    OUT PUCHAR  out);

VOID next_key(
    IN  PUCHAR  key,
    IN  INT     round);
VOID byte_sub(
    IN  PUCHAR  in,
    OUT PUCHAR  out);
VOID shift_row(
    IN  PUCHAR  in,
    OUT PUCHAR  out);
VOID mix_column(
    IN  PUCHAR  in,
    OUT PUCHAR  out);
VOID RTMPAesEncrypt(
    IN  PUCHAR  key,
    IN  PUCHAR  data,
    IN  PUCHAR  ciphertext);
VOID RTMPMicFinal(
    IN  PMIC_CONTEXT    pContext,
    OUT UCHAR           digest[4]);
VOID RTMPCkipInsertCMIC(
    IN  PRTMP_ADAPTER   pAd,
    OUT PUCHAR          pMIC,
    IN  PUCHAR          p80211hdr,
    IN  PNDIS_PACKET    pPacket,
    IN  PCIPHER_KEY     pKey,
    IN  PUCHAR          mic_snap);
#endif
// ba_action.h
extern void (*Announce_Reordering_Packet)(IN PRTMP_ADAPTER pAd,
		IN struct reordering_mpdu *mpdu);
extern BOOLEAN (*ba_reordering_mpdu_insertsorted)(struct reordering_list *list, struct reordering_mpdu *mpdu);
extern void (*ba_enqueue)(struct reordering_list *list, struct reordering_mpdu *mpdu_blk);
extern struct reordering_mpdu * (*ba_dequeue)(struct reordering_list *list);
struct reordering_mpdu  * ba_reordering_mpdu_dequeue(struct reordering_list *list);
extern struct reordering_mpdu  * (*ba_reordering_mpdu_probe)(struct reordering_list *list);
extern void (*ba_reordering_resource_release)(PRTMP_ADAPTER pAd);
extern BOOLEAN (*ba_reordering_resource_init)(PRTMP_ADAPTER pAd, int num);
extern struct reordering_mpdu * (*ba_mpdu_blk_alloc)(PRTMP_ADAPTER pAd);
extern void (*ba_mpdu_blk_free)(PRTMP_ADAPTER pAd, struct reordering_mpdu *mpdu_blk);
extern USHORT (*ba_indicate_reordering_mpdus_in_order)(
	   IN PRTMP_ADAPTER    pAd, 
	   IN PBA_REC_ENTRY    pBAEntry,
	   IN USHORT           StartSeq);
extern void (*ba_indicate_reordering_mpdus_le_seq)(
	   IN PRTMP_ADAPTER    pAd, 
	   IN PBA_REC_ENTRY    pBAEntry,
	   IN USHORT           Sequence);
extern void (*ba_refresh_reordering_mpdus)(
	   IN PRTMP_ADAPTER    pAd, 
	   PBA_REC_ENTRY       pBAEntry);
extern void (*ba_flush_reordering_timeout_mpdus)(
	 IN PRTMP_ADAPTER    pAd, 
	 IN PBA_REC_ENTRY    pBAEntry,
	 IN ULONG            Now32);
extern VOID (*BAOriSessionSetUp)(
	  IN PRTMP_ADAPTER    pAd, 
	  IN MAC_TABLE_ENTRY  *pEntry,
	  IN UCHAR            TID,
	  IN USHORT           TimeOut,
	  IN ULONG            DelayTime,
	  IN BOOLEAN          isForced);
extern VOID (*BAOriSessionAdd)(
	IN PRTMP_ADAPTER    pAd, 
	IN MAC_TABLE_ENTRY  *pEntry,
	IN PFRAME_ADDBA_RSP pFrame);
extern BOOLEAN (*BARecSessionAdd)(
	IN PRTMP_ADAPTER    pAd, 
	IN MAC_TABLE_ENTRY  *pEntry,
	IN PFRAME_ADDBA_REQ pFrame);
extern BA_REC_ENTRY * (*BATableAllocRecEntry)(
  IN  PRTMP_ADAPTER   pAd,
  OUT USHORT          *Idx);
extern BA_ORI_ENTRY * (*BATableAllocOriEntry)(
  IN  PRTMP_ADAPTER   pAd,
  OUT USHORT          *Idx);
extern VOID (*BATableFreeOriEntry)(
	IN  PRTMP_ADAPTER   pAd,
	IN  ULONG           Idx);
extern VOID (*BATableFreeRecEntry)(
	IN  PRTMP_ADAPTER   pAd,
	IN  ULONG           Idx);
extern VOID (*BAOriSessionTearDown)(
	IN OUT  PRTMP_ADAPTER   pAd, 
	IN      UCHAR           Wcid,
	IN      UCHAR           TID,
	IN      BOOLEAN         bPassive,
	IN      BOOLEAN         bForceSend);
extern VOID (*BARecSessionTearDown)(
	IN OUT  PRTMP_ADAPTER   pAd, 
	IN      UCHAR           Wcid,
	IN      UCHAR           TID,
	IN      BOOLEAN         bPassive);
extern VOID (*BASessionTearDownALL)(
	IN OUT  PRTMP_ADAPTER pAd, 
	IN      UCHAR Wcid);
extern VOID (*BAOriSessionSetupTimeout)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*BARecSessionIdleTimeout)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*PeerAddBAReqAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerAddBARspAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerDelBAAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
/* START: New added */
#ifdef CONFIG_AP_SUPPORT
extern VOID (*SendBeaconRequest)(
				   IN PRTMP_ADAPTER		pAd,
				   IN UCHAR				Wcid);
#endif
/* END: New added */

extern BOOLEAN (*CntlEnqueueForRecv)(
	IN PRTMP_ADAPTER		pAd, 
	IN ULONG				Wcid, 
	IN ULONG				MsgLen, 
	IN PFRAME_BA_REQ		pMsg);
extern VOID (*ba_enqueue_reordering_packet)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PBA_REC_ENTRY	pBAEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern BOOLEAN (*convert_reordering_packet_to_preAMSDU_or_802_3_packet)(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RX_BLK			*pRxBlk,
	IN  UCHAR			FromWhichBSSID);
extern VOID (*Indicate_AMPDU_Packet)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern UINT (*BA_Reorder_AMSDU_Annnounce)(RTMP_ADAPTER *pAd,
	PNDIS_PACKET pPacket,
	UCHAR OpMode);

extern UINT (*deaggregate_AMSDU_announce)(
	IN RTMP_ADAPTER *pAd,
	PNDIS_PACKET pPacket,
	IN UCHAR *pData,
	IN ULONG DataSize,
	IN UCHAR OpMode);

VOID RtmpOsPktInit(
	IN	PNDIS_PACKET		pNetPkt,
	IN	PNET_DEV			pNetDev,
	IN	UCHAR				*pData,
	IN	USHORT				DataSize);

extern VOID (*Indicate_AMSDU_Packet)(
	RTMP_ADAPTER *pAd,
	RX_BLK *pRxBlk,
	UCHAR wdev_idx);
extern UINT16 (*tx_rate_to_tmi_rate)(
    IN UINT_8 mode,
    IN UINT_8 mcs,
    IN UINT_8 nss,
    IN BOOL stbc,
    IN UINT_8 preamble);


/*START: New patch support added */

//cmm_info.c
extern VOID (*RTMPSetIndividualHT)(RTMP_ADAPTER *pAd, UCHAR apidx);

#ifdef ETH_CONVERT_SUPPORT
INT Set_EthConvertMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	char *arg);

INT Set_EthCloneMac_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
#endif // ETH_CONVERT_SUPPORT //

RTMP_STRING *GetEncryptType(CHAR enc);
RTMP_STRING *GetAuthMode(CHAR auth);
extern INT (*ht_mode_adjust)(
	RTMP_ADAPTER *pAd,
	MAC_TABLE_ENTRY *pEntry,
	HT_CAPABILITY_IE *peer,
	RT_HT_CAPABILITY *my);
extern INT (*get_ht_max_mcs)(
	RTMP_ADAPTER *pAd,
	UCHAR *desire_mcs,
	UCHAR *cap_mcs);
extern INT (*set_ht_fixed_mcs)(RTMP_ADAPTER *pAd, MAC_TABLE_ENTRY *pEntry, UCHAR fixed_mcs, UCHAR mcs_bound);
extern UCHAR (*get_cent_ch_by_htinfo_iotn9)(	RTMP_ADAPTER *pAd,	ADD_HT_INFO_IE *ht_op,	HT_CAPABILITY_IE *ht_cap);


extern VOID (*set_sta_ht_cap)(RTMP_ADAPTER *pAd, MAC_TABLE_ENTRY *ent, HT_CAPABILITY_IE *ht_ie);

//mlme/mlme.c
VOID MlmeLpEnter(RTMP_ADAPTER *pAd);
VOID MlmeLpExit(RTMP_ADAPTER *pAd);
extern UCHAR (*dot11_2_ra_rate)(UCHAR MaxSupportedRateIn500Kbps);
extern VOID (*MtAsicSetEdcaParm)(RTMP_ADAPTER *pAd, PEDCA_PARM pEdcaParm);
#ifdef CONFIG_AP_SUPPORT
extern VOID (*set_default_ap_edca_param)(RTMP_ADAPTER *pAd);
extern VOID (*set_default_sta_edca_param)(RTMP_ADAPTER *pAd);
#ifdef APCLI_SUPPORT
extern VOID (*ApCliMlmeInit)(RTMP_ADAPTER *pAd);
extern VOID (*ApCliRTMPSendNullFrame)(
	IN	PRTMP_ADAPTER	pAd,	
	IN	UCHAR			TxRate,
	IN	BOOLEAN 		bQosNull,
	IN 	PMAC_TABLE_ENTRY pMacEntry,	
	IN	USHORT			PwrMgmt);

extern BOOLEAN (*isValidApCliIf)(RTMP_ADAPTER *pAd,SHORT if_idx);

extern BOOLEAN (*preCheckMsgTypeSubset)(
	IN PRTMP_ADAPTER  pAd,
	IN PFRAME_802_11 pFrame,
	OUT INT *Machine,
	OUT INT *MsgType);

extern BOOLEAN (*ApCliMsgTypeSubst)(
	IN PRTMP_ADAPTER pAd,
	IN PFRAME_802_11 pFrame,
	OUT INT *Machine,
	OUT INT *MsgType);

extern VOID (*RT28xx_ApCli_Init)(VOID *pAd, PNET_DEV main_dev_p);

extern INT (*ApCli_VirtualIF_Open)(PNET_DEV dev_p);

extern VOID (*ApCliIfMonitor)(RTMP_ADAPTER *pAd);

extern VOID (*ApCliIfUp)(RTMP_ADAPTER *pAd);
extern 	VOID (*ApCliIfDown)(RTMP_ADAPTER *pAd);

#ifdef MAT_SUPPORT

extern BOOLEAN (*MATPktRxNeedConvert)(
		IN PRTMP_ADAPTER	pAd, 
		IN PNET_DEV			net_dev);
extern PUCHAR (*MATEngineRxHandle)(
	IN PRTMP_ADAPTER	pAd,
	IN PNDIS_PACKET		pPkt,
	IN UINT				infIdx);
extern NDIS_STATUS (*MATEngineExit)(
	IN RTMP_ADAPTER *pAd);
extern PUCHAR (*MATEngineTxHandle)(
	IN PRTMP_ADAPTER	pAd,
	IN PNDIS_PACKET	    pPkt,
	IN UINT				ifIdx,
	IN UCHAR    OpMode);
extern NDIS_STATUS (*MATEngineInit)(
	IN RTMP_ADAPTER *pAd);
#endif


#ifdef MAC_REPEATER_SUPPORT
INT MtAsicSetMacAddrExt(RTMP_ADAPTER *pAd, BOOLEAN bEnable);
VOID MtAsicInsertRepeaterEntry(
	IN PRTMP_ADAPTER pAd,	
	IN UCHAR CliIdx,	
	IN PUCHAR pAddr);
VOID MtAsicRemoveRepeaterEntry(RTMP_ADAPTER *pAd, UCHAR CliIdx);
VOID MtAsicInsertRepeaterRootEntry(	
	IN PRTMP_ADAPTER pAd,
	IN UCHAR Wcid,	
	IN UCHAR *pAddr,
	IN UCHAR ReptCliIdx);
INT AsicSetMacAddrExt(RTMP_ADAPTER *pAd, BOOLEAN enable);
VOID AsicInsertRepeaterEntry(RTMP_ADAPTER *pAd, UCHAR CliIdx, UCHAR *pAddr);
VOID AsicRemoveRepeaterEntry(RTMP_ADAPTER *pAd, UCHAR CliIdx);
VOID AsicInsertRepeaterRootEntry(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR Wcid,	
	IN UCHAR *pAddr,
	IN UCHAR ReptCliIdx);
INT32 GetFirstAvailableRepeaterOmacIdx(RTMP_ADAPTER *pAd);

INT AsicSetReptFuncEnable(RTMP_ADAPTER *pAd, BOOLEAN enable);
VOID RepeaterCtrlInit(RTMP_ADAPTER *pAd);
VOID RepeaterCtrlExit(RTMP_ADAPTER *pAd);

INT MtAsicSetReptFuncEnableByDriver(RTMP_ADAPTER *pAd, BOOLEAN bEnable);

VOID RepeaterLinkMonitor(RTMP_ADAPTER *pAd);

UINT32 ReptTxPktCheckHandler(
    RTMP_ADAPTER *pAd,
    IN struct wifi_dev *cli_link_wdev,
    IN PNDIS_PACKET pPacket,
    OUT UCHAR *pWcid);

VOID UpdateMbssCliLinkMap(
    RTMP_ADAPTER *pAd,
    UCHAR MbssIdx,
    struct wifi_dev *cli_link_wdev,
    struct wifi_dev *mbss_link_wdev);
VOID InsertIgnoreAsRepeaterEntryTable(
    IN RTMP_ADAPTER *pAd,
    IN UCHAR *pAddr);

BOOLEAN RepeaterRemoveIngoreEntry(
    IN RTMP_ADAPTER *pAd,
    IN UCHAR idx,
    IN UCHAR *pAddr);
VOID CliLinkMapInit(RTMP_ADAPTER *pAd);
#endif /* MAC_REPEATER_SUPPORT */
#endif /* APCLI_SUPPORT */
extern VOID (*ApMlmeInit)(RTMP_ADAPTER *pAd);
extern VOID (*ApBssEntrySet)(
	IN RTMP_ADAPTER *pAd,
	OUT BSS_ENTRY *pBss,
	IN BCN_IE_LIST *ie_list,
	IN CHAR Rssi,
	IN USHORT LengthVIE,
	IN PNDIS_802_11_VARIABLE_IEs pVIE);
extern ULONG (*ApBssTableSetEntry)(
	IN PRTMP_ADAPTER pAd,
	OUT BSS_TABLE *Tab,
	IN BCN_IE_LIST *ie_list,
	IN CHAR Rssi,
	IN USHORT LengthVIE,
	IN PNDIS_802_11_VARIABLE_IEs pVIE);

#endif /* CONFIG_AP_SUPPORT */
extern VOID  (*TriEventInit)(IN	PRTMP_ADAPTER pAd);
extern INT (*TriEventTableSetEntry)(
	IN	PRTMP_ADAPTER	pAd, 
	OUT TRIGGER_EVENT_TAB *Tab, 
	IN PUCHAR pBssid, 	
	IN HT_CAPABILITY_IE *pHtCapability,	
	IN UCHAR			HtCapabilityLen,
	IN UCHAR			RegClass,	
	IN UCHAR ChannelNo);

extern VOID (*RxFilterCfg2Row)(UINT32 FilterMask,UINT32 *RowFilterMask);
extern INT (*MtAsicSetRxFilter)(RTMP_ADAPTER *pAd,MT_RX_FILTER_CTRL_T RxFilter);
extern INT (*AsicSetRxFilter)(    IN RTMP_ADAPTER *pAd);

#ifdef IOT_WOW_SUPPORT
extern VOID (*WowEnable)(IN RTMP_ADAPTER *pAd, BOOLEAN enable);
extern VOID (*WowMagicPktCfg)(UINT_32 Bssid, BOOLEAN enable);
extern VOID (*WowBitmapPatternCfg)(UCHAR *BmMask, UCHAR *BmPattern);
extern VOID (*WowArpOffloadCfg)(IN RTMP_ADAPTER *pAd, UCHAR *IpAddr);
extern VOID (*WowReKeyCfg)(IN RTMP_ADAPTER *pAd, UCHAR *pWowData);
VOID WowCfg(IN RTMP_ADAPTER *pAd, UINT8 CmdType, UCHAR *pWowData);

#endif /* IOT_WOW_SUPPORT */

//mlme/offload_routine.c
extern WLAN_STATUS (*OffloadChannelSwitch)(P_CHAN_SWITCH_T prChanSwitch);
#ifdef IOT_OFFLOAD_ROUTINE
extern WLAN_STATUS (*OffloadEdcaSet)(P_EDCA_SET_T prEdcaSet);
extern WLAN_STATUS (*OffloadSlotTimeSet)(P_SLOT_TIME_SET_T prSlotTimeSet);
WLAN_STATUS OffloadRdgCtrl(P_RDG_CTRL_T prRdgCtrl);
VOID LowPowerDebug(RTMP_ADAPTER *pAd);
INT_32 SetPwrMgtBitWifi(RTMP_ADAPTER *pAd, UINT_8 WlanIdx, UINT_8 PwrMgtBit);
extern INT_32 (*AsicPmStateCtrl)(RTMP_ADAPTER *pAd,UINT_8 WlanIdx, UINT_8 PmNumber, UINT_8 PmState);
extern VOID (*RTMPOffloadPm)(RTMP_ADAPTER *pAd, UINT_8 WlanIdx, UINT_8 PmNumber, UINT_8 PmState);
extern WLAN_STATUS (*OffloadPmStateCtrl)(P_PM_STATE_CTRL_T prPmStateCtrl);
//WLAN_STATUS (*OffloadPwrMgtBitUpdate)(P_PWR_MGT_BIT_T prMultiPS);
extern VOID (*OffloadSetTxPreamble)(RTMP_ADAPTER *Ad, USHORT TxPreamble);
#endif

#ifdef CONFIG_AP_SUPPORT
//ap.c
extern UCHAR (*get_apidx_by_addr)(RTMP_ADAPTER *pAd, UCHAR *addr);
INT set_wdev_if_addr(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, INT opmode);
extern INT (*ap_mlme_set_capability)(RTMP_ADAPTER *pAd, BSS_STRUCT *pMbss);
extern NDIS_STATUS (*APInitialize)(RTMP_ADAPTER *pAd);
extern VOID (*APShutdown)(RTMP_ADAPTER *pAd);
#ifdef DOT11W_PMF_SUPPORT
INT ap_pmf_init(RTMP_ADAPTER *pAd, BSS_STRUCT *pMbss, INT idx);
#endif
extern INT (*ap_security_init)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, INT idx);
//INT ap_key_tb_init(RTMP_ADAPTER *pAd);
extern INT (*ap_hw_tb_init)(RTMP_ADAPTER *pAd);
extern INT (*ap_phy_rrm_init_byRf)(RTMP_ADAPTER *pAd, UCHAR RfIC);
extern INT (*ap_phy_rrm_init)(RTMP_ADAPTER *pAd);
extern INT (*ap_func_init)(RTMP_ADAPTER *pAd);
extern VOID (*APStartUpForMbss)(RTMP_ADAPTER *pAd,BSS_STRUCT *pMbss);
extern VOID (*APStartUp)(RTMP_ADAPTER *pAd);
extern VOID (*APStop)(RTMP_ADAPTER *pAd);
extern VOID (*APCleanupPsQueue)(RTMP_ADAPTER *pAd, QUEUE_HEADER *pQueue);
extern VOID (*MacTableMaintenance)(RTMP_ADAPTER *pAd);
UINT32 MacTableAssocStaNumGet(RTMP_ADAPTER *pAd);
#if 0
MAC_TABLE_ENTRY *APSsPsInquiry(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR *pAddr,
	OUT SST *Sst,
	OUT USHORT *Aid,
	OUT UCHAR *PsMode,
	OUT UCHAR *Rate);
#endif
VOID ApLogEvent(RTMP_ADAPTER *pAd, UCHAR *pAddr, USHORT Event);
extern VOID (*APUpdateOperationMode)(RTMP_ADAPTER *pAd);
extern BOOLEAN (*ApCheckLongPreambleSTA)(RTMP_ADAPTER *pAd);
extern VOID (*ApUpdateCapabilityAndErpleByRf)(RTMP_ADAPTER *pAd, UCHAR RfIC);
extern VOID (*APUpdateCapabilityAndErpIe)(RTMP_ADAPTER *pAd);
BOOLEAN ApCheckAccessControlList(RTMP_ADAPTER *pAd, UCHAR *pAddr, UCHAR Apidx);
VOID ApUpdateAccessControlList(RTMP_ADAPTER *pAd, UCHAR Apidx);
//INT GetBssCoexEffectedChRange(
//	IN RTMP_ADAPTER *pAd,
//	IN BSS_COEX_CH_RANGE *pCoexChRange,
//	IN UCHAR Channel);
VOID APOverlappingBSSScan(RTMP_ADAPTER *pAd, UCHAR PhyMode, UCHAR Channel);
BOOLEAN DOT1X_InternalCmdAction(
    IN  PRTMP_ADAPTER	pAd,
    IN  MAC_TABLE_ENTRY *pEntry,
    IN UINT8 cmd);
extern INT (*rtmp_ap_init)(RTMP_ADAPTER *pAd);
VOID rtmp_ap_exit(RTMP_ADAPTER *pAd);

//apcli_assoc.c
VOID ApCliAssocTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

#ifdef MAC_REPEATER_SUPPORT
VOID ApCliAssocTimeoutExt(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
#endif /* MAC_REPEATER_SUPPORT */
VOID ApcliSendAssocIEsToWpaSupplicant( 
    IN RTMP_ADAPTER *pAd,
    IN UINT ifIndex);
VOID ApCliMlmeAssocReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID ApCliMlmeDisassocReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID ApCliPeerAssocRspAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID ApCliPeerDisassocAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID ApCliAssocTimeoutAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID ApCliInvalidStateWhenAssoc(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID ApCliInvalidStateWhenDisassociate(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID ApCliAssocPostProc(
	IN PRTMP_ADAPTER pAd, 
	IN PUCHAR pAddr2, 
	IN USHORT CapabilityInfo, 
	IN USHORT IfIndex, 
	IN UCHAR SupRate[], 
	IN UCHAR SupRateLen,
	IN UCHAR ExtRate[],
	IN UCHAR ExtRateLen,
	IN PEDCA_PARM pEdcaParm,
	IN HT_CAPABILITY_IE *pHtCapability,
	IN UCHAR HtCapabilityLen, 
	IN ADD_HT_INFO_IE *pAddHtInfo);
VOID ApCliAssocStateMachineInit(
	IN	RTMP_ADAPTER	*pAd, 
	IN  STATE_MACHINE *S, 
	OUT STATE_MACHINE_FUNC Trans[]) ;


//apcli_auth.c
VOID ApCliAuthTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
VOID ApCliAuthTimeoutExt(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
VOID ApCliMlmeAuthReqAction(
	IN RTMP_ADAPTER *pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliPeerAuthRspAtSeq2Action(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM * Elem);
VOID ApCliPeerAuthRspAtSeq4Action(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID ApCliPeerDeauthAction(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID ApCliAuthTimeoutAction(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID ApCliInvalidStateWhenAuth(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID ApCliMlmeDeauthReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliAuthStateMachineInit(
	IN RTMP_ADAPTER *pAd,
	IN STATE_MACHINE *Sm,
	OUT STATE_MACHINE_FUNC Trans[]);

//apcli_ctrl.c
VOID ApCliCtrlJoinReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlJoinReqTimeoutAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlProbeRspAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlAuthRspAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlAuth2RspAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlAuthReqTimeoutAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlAuth2ReqTimeoutAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlAssocRspAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlDeAssocRspAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlAssocReqTimeoutAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlDisconnectReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlPeerDeAssocReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlDeAssocAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlDeAuthAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliCtrlStateMachineInit(
	IN PRTMP_ADAPTER pAd,
	IN STATE_MACHINE *Sm,
	OUT STATE_MACHINE_FUNC Trans[]);

//apcli_sync.c
VOID ApCliSyncStateMachineInit(
	IN PRTMP_ADAPTER pAd,
	IN STATE_MACHINE *Sm,
	OUT STATE_MACHINE_FUNC Trans[]);
VOID ApCliProbeTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
VOID ApCliMlmeProbeReqAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliPeerProbeRspAtJoinAction(
	IN RTMP_ADAPTER *pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliProbeTimeoutAtJoinAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliInvalidStateWhenJoin(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
VOID ApCliEnqueueProbeRequest(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR SsidLen,
	OUT PCHAR Ssid,
	IN USHORT ifIndex);

//ap_apcli.c
BOOLEAN ApCliWaitProbRsp(PRTMP_ADAPTER pAd, USHORT ifIndex);
//BOOLEAN isValidApCliIf(RTMP_ADAPTER *pAd,SHORT if_idx);
VOID ApCliMgtMacHeaderInit(
    IN RTMP_ADAPTER *pAd,
    INOUT HEADER_802_11 *pHdr80211,
    IN UCHAR SubType,
    IN UCHAR ToDs,
    IN UCHAR *pDA,
    IN UCHAR *pBssid,
    IN USHORT ifIndex);
BOOLEAN ApCliCheckHt(
	IN RTMP_ADAPTER *pAd,
	IN USHORT IfIndex,
	INOUT HT_CAPABILITY_IE *pHtCapability,
	INOUT ADD_HT_INFO_IE *pAddHtInfo);
BOOLEAN ApCliLinkUp(RTMP_ADAPTER *pAd, UCHAR ifIndex);
VOID ApCliLinkDown(RTMP_ADAPTER *pAd, UCHAR ifIndex);
//VOID ApCliIfUp(RTMP_ADAPTER *pAd);
//VOID ApCliIfDown(RTMP_ADAPTER *pAd);
//VOID ApCliIfMonitor(RTMP_ADAPTER *pAd);
/*BOOLEAN ApCliMsgTypeSubst(
	IN PRTMP_ADAPTER pAd,
	IN PFRAME_802_11 pFrame,
	OUT INT *Machine,
	OUT INT *MsgType);
BOOLEAN preCheckMsgTypeSubset(
	IN PRTMP_ADAPTER  pAd,
	IN PFRAME_802_11 pFrame,
	OUT INT *Machine,
	OUT INT *MsgType);*/
BOOLEAN ApCliPeerAssocRspSanity(
    IN PRTMP_ADAPTER pAd,
    IN VOID *pMsg,
    IN ULONG MsgLen,
    OUT PUCHAR pAddr2,
    OUT USHORT *pCapabilityInfo,
    OUT USHORT *pStatus,
    OUT USHORT *pAid,
    OUT UCHAR SupRate[],
    OUT UCHAR *pSupRateLen,
    OUT UCHAR ExtRate[],
    OUT UCHAR *pExtRateLen,
    OUT HT_CAPABILITY_IE *pHtCapability,
    OUT ADD_HT_INFO_IE *pAddHtInfo,	/* AP might use this additional ht info IE */
    OUT UCHAR *pHtCapabilityLen,
    OUT UCHAR *pAddHtInfoLen,
    OUT UCHAR *pNewExtChannelOffset,
    OUT PEDCA_PARM pEdcaParm,
    OUT UCHAR *pCkipFlag,
    OUT IE_LISTS *ie_list);
MAC_TABLE_ENTRY *ApCliTableLookUpByWcid(RTMP_ADAPTER *pAd, UCHAR wcid, UCHAR *pAddrs);
INT ApCliAllowToSendPacket(
	IN RTMP_ADAPTER *pAd,
	IN struct wifi_dev *wdev,
	IN PNDIS_PACKET pPacket,
	OUT UCHAR *pWcid);
BOOLEAN ApCliValidateRSNIE(
	IN RTMP_ADAPTER *pAd,
	IN PEID_STRUCT pEid_ptr,
	IN USHORT eid_len,
	IN USHORT idx);

#ifdef IOT_SECURITY  //LATER when interfaced with WPA supplicant
VOID APCliInstallPairwiseKey(
	IN  PRTMP_ADAPTER   pAd,
	IN  MAC_TABLE_ENTRY *pEntry);
BOOLEAN APCliInstallSharedKey(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pKey,
	IN  UCHAR           KeyLen,
	IN	UCHAR			DefaultKeyIdx,
	IN  MAC_TABLE_ENTRY *pEntry);
#endif /* IOT_SECURITY */
VOID ApCliUpdateMlmeRate(RTMP_ADAPTER *pAd, USHORT ifIndex);
VOID APCli_Init(RTMP_ADAPTER *pAd, PNET_DEV main_dev_p);
VOID ApCli_Remove(RTMP_ADAPTER *pAd);
BOOLEAN ApCli_Open(RTMP_ADAPTER *pAd, PNET_DEV dev_p);
BOOLEAN ApCli_Close(RTMP_ADAPTER *pAd, PNET_DEV dev_p);
#ifdef APCLI_AUTO_CONNECT_SUPPORT
BOOLEAN ApCliAutoConnectExec(
	IN  PRTMP_ADAPTER   pAd);
VOID ApCliSwitchCandidateAP(
	IN PRTMP_ADAPTER pAd);
BOOLEAN ApcliCompareAuthEncryp(
	IN PAPCLI_STRUCT pApCliEntry,
	IN NDIS_802_11_AUTHENTICATION_MODE 	AuthMode,
	IN NDIS_802_11_AUTHENTICATION_MODE 	AuthModeAux,
	IN NDIS_802_11_WEP_STATUS			WEPstatus,
	IN CIPHER_SUITE 					WPA);
#endif
BOOLEAN ValidApCliEntry(RTMP_ADAPTER *pAd, INT apCliIdx);

//ap_apcli_inf.c
//VOID RT28xx_ApCli_Init(VOID *pAd, PNET_DEV main_dev_p);
//INT ApCli_VirtualIF_Open(PNET_DEV dev_p);
//INT ApCli_VirtualIF_Close(PNET_DEV dev_p);
VOID RT28xx_ApCli_Remove(VOID *pAd);

//ap_assoc.c
extern USHORT (*update_associated_mac_entry)(
	IN RTMP_ADAPTER *pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN IE_LISTS *ie_list,
	IN UCHAR MaxSupportedRate);
extern USHORT (*APBuildAssociation)(
    IN RTMP_ADAPTER *pAd,
    IN MAC_TABLE_ENTRY *pEntry,
    IN IE_LISTS *ie_list,
    IN UCHAR MaxSupportedRateIn500Kbps,
    OUT USHORT *pAid);
#ifdef IAPP_SUPPORT
BOOLEAN IAPP_L2_Update_Frame_Send(RTMP_ADAPTER *pAd, UINT8 *mac, INT wdev_idx);
#endif
extern BOOLEAN (*PeerAssocReqCmmSanity)(
	RTMP_ADAPTER *pAd, 
	BOOLEAN isReassoc,
	VOID *Msg,
	INT MsgLen,
	IE_LISTS *ie_lists);
extern VOID (*ap_cmm_peer_assoc_req_action)(
    IN RTMP_ADAPTER *pAd,
    IN MLME_QUEUE_ELEM *Elem,
	IN BOOLEAN isReassoc);
extern VOID (*APPeerAssocReqAction)(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
extern VOID (*APPeerReassocReqAction)(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
extern VOID (*APPeerDisassocReqAction)(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
extern VOID (*APMlmeDisassocReqAction)(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID MbssKickOutStas(RTMP_ADAPTER *pAd, INT apidx, USHORT Reason);
extern VOID (*APMlmeKickOutSta)(RTMP_ADAPTER *pAd, UCHAR *pStaAddr, UCHAR Wcid, USHORT Reason);
extern BOOLEAN (*PeerDisassocReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT	UINT16	*SeqNum,
    OUT USHORT *Reason);

extern VOID (*APCls3errAction)(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
extern VOID (*APAssocStateMachineInit)(
    IN RTMP_ADAPTER *pAd,
    IN STATE_MACHINE *S,
    OUT STATE_MACHINE_FUNC Trans[]);

//ap_auth.c
extern BOOLEAN (*PeerDeauthReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT	UINT16	*SeqNum, 
    OUT USHORT *Reason);
extern VOID (*APMlmeDeauthReqAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*APPeerDeauthReqAction)(
    IN PRTMP_ADAPTER pAd, 
    IN PMLME_QUEUE_ELEM Elem);
extern VOID (*APPeerAuthReqAtIdleAction)(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
extern VOID (*APPeerAuthConfirmAction)(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*APPeerAuthSanity)(
    IN RTMP_ADAPTER *pAd,
    IN VOID *Msg,
    IN ULONG MsgLen,
    IN AUTH_FRAME_INFO *auth_info);
extern VOID (*APPeerAuthSimpleRspGenAndSend)(
    IN PRTMP_ADAPTER pAd, 
    IN PHEADER_802_11 pHdr, 
    IN USHORT Alg, 
    IN USHORT Seq, 
    IN USHORT StatusCode);
extern VOID (*APCls2errAction)(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
extern void (*APAuthStateMachineInit)(
    IN PRTMP_ADAPTER pAd, 
    IN STATE_MACHINE *Sm, 
    OUT STATE_MACHINE_FUNC Trans[]);

//ap_cfg.c
#ifdef APCLI_SUPPORT
INT Set_ApCli_Ssid_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_ApCli_Bssid_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_ApCli_DefaultKeyID_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_ApCli_WPAPSK_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_ApCli_Key1_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_ApCli_Key2_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_ApCli_Key3_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_ApCli_Key4_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_ApCli_TxMode_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_ApCli_TxMcs_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
#ifdef APCLI_AUTO_CONNECT_SUPPORT
INT Set_ApCli_AutoConnect_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
#endif /* APCLI_AUTO_CONNECT_SUPPORT */

#ifdef WPA_SUPPLICANT_SUPPORT
INT Set_ApCli_Wpa_Support(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT	Set_ApCli_IEEE8021X_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
#endif/*WPA_SUPPLICANT_SUPPORT*/

#ifdef MAC_REPEATER_SUPPORT
INT Set_ReptMode_Enable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
#endif /* MAC_REPEATER_SUPPORT */

#ifdef WSC_AP_SUPPORT
INT Set_AP_WscSsid_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
#endif /* WSC_AP_SUPPORT */
#endif /* APCLI_SUPPORT */
INT	ApCfg_Set_AuthMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	INT				apidx,
	IN	NDIS_802_11_AUTHENTICATION_MODE AuthMode);
//ap_connect.c
#if 0	//Raju: Not used
BOOLEAN BeaconTransmitRequired(RTMP_ADAPTER *pAd, INT apidx, BSS_STRUCT *pMbss);
VOID write_tmac_info_beacon(RTMP_ADAPTER *pAd, INT apidx, UCHAR *tmac_buf, HTTRANSMIT_SETTING *BeaconTransmit, ULONG frmLen);
VOID asic_write_bcn_buf(RTMP_ADAPTER *pAd, UCHAR *tmac_info, INT info_len, UCHAR *bcn_buf, INT buf_len, UINT32 hw_addr);
VOID APMakeBssBeacon(RTMP_ADAPTER *pAd, INT apidx);
VOID APUpdateBeaconFrame(RTMP_ADAPTER *pAd, INT apidx);
UCHAR GetBcnNum(RTMP_ADAPTER *pAd);
VOID APMakeAllBssBeacon(RTMP_ADAPTER *pAd);
VOID APUpdateAllBeaconFrame(RTMP_ADAPTER *pAd);
#endif

//ap_mlme.c
VOID Bss2040CoexistTimeOut(
	IN PVOID SystemSpecific1,
	IN PVOID FunctionContext,
	IN PVOID SystemSpecific2,
	IN PVOID SystemSpecific3);

VOID APDetectOverlappingExec(
	IN PVOID SystemSpecific1,
	IN PVOID FunctionContext,
	IN PVOID SystemSpecific2,
	IN PVOID SystemSpecific3);
extern VOID (*APMlmePeriodicExec)(
    RTMP_ADAPTER *pAd);
extern BOOLEAN (*APMsgTypeSubst)(
    IN PRTMP_ADAPTER pAd,
    IN PFRAME_802_11 pFrame,
    OUT INT *Machine,
    OUT INT *MsgType);

//ap_repeater.c
#ifdef MAC_REPEATER_SUPPORT
REPEATER_CLIENT_ENTRY *RTMPLookupRepeaterCliEntry(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN bRealMAC,
	IN PUCHAR pAddr);
VOID RTMPInsertRepeaterEntry(
	PRTMP_ADAPTER pAd,
	struct wifi_dev *wdev,
	PUCHAR pAddr);
VOID RTMPRemoveRepeaterEntry(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR func_tb_idx,
	IN UCHAR CliIdx);
MAC_TABLE_ENTRY *RTMPInsertRepeaterMacEntry(
	IN  RTMP_ADAPTER *pAd,
	IN  UCHAR *pAddr,
	IN struct wifi_dev *wdev,
	IN  UCHAR apIdx,
	IN  UCHAR cliIdx,
	IN BOOLEAN CleanAll);
VOID RTMPRepeaterReconnectionCheck(
	IN PRTMP_ADAPTER pAd);
BOOLEAN RTMPRepeaterVaildMacEntry(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pAddr);
INVAILD_TRIGGER_MAC_ENTRY *RepeaterInvaildMacLookup(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pAddr);
/*
VOID RTMPRepeaterInsertInvaildMacEntry(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pAddr);
BOOLEAN RTMPRepeaterRemoveInvaildMacEntry(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR idx,
	IN PUCHAR pAddr);
*/
#endif

//ap_sync.c
extern VOID (*APPeerProbeReqAction)(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*APPeerBeaconAction)(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
#ifdef AP_SCAN_SUPPORT
VOID APInvalidStateWhenScan(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID APScanTimeout(
	IN PVOID SystemSpecific1,
	IN PVOID FunctionContext,
	IN PVOID SystemSpecific2,
	IN PVOID SystemSpecific3);
VOID APScanTimeoutAction(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID APMlmeScanReqAction(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID APPeerBeaconAtScanAction(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID APScanCnclAction(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
VOID ApSiteSurvey(
	IN	PRTMP_ADAPTER  		pAd,
	IN	PNDIS_802_11_SSID	pSsid,
	IN	UCHAR				ScanType,
	IN	BOOLEAN				ChannelSel);
#endif
BOOLEAN ApScanRunning(RTMP_ADAPTER *pAd);
extern VOID (*APSyncStateMachineInit)(
	IN RTMP_ADAPTER *pAd,
	IN STATE_MACHINE *Sm,
	OUT STATE_MACHINE_FUNC Trans[]);
extern VOID (*SupportRate)(
	IN PUCHAR SupRate,
	IN UCHAR SupRateLen,
	IN PUCHAR ExtRate,
	IN UCHAR ExtRateLen,
	OUT PUCHAR *ppRates,
	OUT PUCHAR RatesLen,
	OUT PUCHAR pMaxSupportRate);

UCHAR get_regulatory_class(RTMP_ADAPTER *pAd, UCHAR Channel);
void build_ext_channel_switch_ie(
	IN PRTMP_ADAPTER pAd,
	IN HT_EXT_CHANNEL_SWITCH_ANNOUNCEMENT_IE *pIE,
	IN UCHAR Channel);

//ap_wpa.c
extern MAC_TABLE_ENTRY *(*PACInquiry)(RTMP_ADAPTER *pAd, UCHAR Wcid);
extern BOOLEAN (*RTMPCheckMcast)(
    IN PRTMP_ADAPTER    pAd,
    IN PEID_STRUCT      eid_ptr,
    IN MAC_TABLE_ENTRY  *pEntry);
extern BOOLEAN (*RTMPCheckUcast)(
    IN PRTMP_ADAPTER    pAd,
    IN PEID_STRUCT      eid_ptr,
    IN MAC_TABLE_ENTRY	*pEntry);
extern BOOLEAN (*RTMPCheckAKM)(PUCHAR sta_akm, PUCHAR ap_rsn_ie, INT iswpa2);
extern BOOLEAN (*RTMPCheckAUTH)(
    IN PRTMP_ADAPTER    pAd,
    IN PEID_STRUCT      eid_ptr,
    IN MAC_TABLE_ENTRY	*pEntry);
extern UINT	(*APValidateRSNIE)(
	IN PRTMP_ADAPTER    pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN PUCHAR			pRsnIe,
	IN UCHAR			rsnie_len);

//Bcn.c
extern BOOLEAN (*BeaconTransmitRequired)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev);
extern P_MSDU_INFO_T (*hemAllocBcnPkt)();
extern void (*hemBcnPktFree)(P_MSDU_INFO_T pAmsduInfo);
VOID* hemGetBcnPkt(P_MSDU_INFO_T pAmsduInfo);
P_EXT_CMD_BCN_UPDATE_T hemGetBcnCmd(P_MSDU_INFO_T prPktMsduInfo);
extern void (*hemSendBcnPkt)(P_MSDU_INFO_T pAmsduInfo);
extern INT (*bcn_buf_init)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev);
extern INT (*bcn_buf_deinit)(RTMP_ADAPTER *pAd, BCN_BUF_STRUC *bcn_info);
extern void (*AsicStopBcnFrame)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG FrameLen);
extern void (*AsicSendBcnFrame)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG FrameLen);
extern VOID (*MakeBeacon)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, BOOLEAN UpdateRoutine);
extern VOID (*ComposeAPRsnIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*ComposeStaRsnIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*ComposeRSNIE)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*ComposeWPSIE)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*MakeErpIE)(
            RTMP_ADAPTER *pAd,
            struct wifi_dev *wdev,
            ULONG *pFrameLen,
            UCHAR *pBeaconFrame);
VOID MakeChSwitchAnnounceIEandExtend(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*MakeHTIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
VOID MakeHotSpotIE(BSS_STRUCT *pMbss, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*MakeExtCapIE)(RTMP_ADAPTER *pAd, BSS_STRUCT *pMbss, ULONG *pFrameLen, UCHAR *pBeaconFrame);
#ifdef IOT_WMM
extern VOID (*MakeWmmIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
#endif
extern UINT8 (*GetCuntryMaxTxPwr)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR PhyMode,
	IN UINT8 channel);
extern VOID (*MakeCountryIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*MakeChReportIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*MakeExtSuppRateIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
#if defined (A_BAND_SUPPORT) && defined (CONFIG_AP_SUPPORT)
VOID MakePwrConstraintIe(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
#endif
extern VOID (*ComposeBcnPktTail)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
VOID MakeSupRateIe(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*updateBeaconRoutineCase)(RTMP_ADAPTER *pAd, BOOLEAN UpdateAfterTim);
extern VOID (*UpdateBeaconHandler)(
        RTMP_ADAPTER *pAd,
        struct wifi_dev *wdev,
        UCHAR BCN_UPDATE_REASON);
extern INT (*BcnTimUpdate)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, UCHAR *ptr);
extern ULONG (*ComposeBcnPktHead)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, UCHAR *pBeaconFrame);

//ap_data.c
extern BOOLEAN (*APChkCls2Cls3Err)(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
extern INT (*APRxPktAllow)(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
extern INT (*APRxPsHandle)(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
extern INT (*APRxFowardHandle)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, PNDIS_PACKET pPacket);
extern INT (*ApAllowToSendPacket)(
		IN RTMP_ADAPTER *pAd,
		IN struct wifi_dev *wdev,
		IN PNDIS_PACKET pPacket,
		IN UCHAR *pWcid);
extern INT (*APSendPacket)(RTMP_ADAPTER *pAd, PNDIS_PACKET pPacket);
extern PUCHAR (*APBuildAmsduFrameHeader)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern VOID (*APAmpduFrameTx)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern VOID (*APAmsduFrameTx)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern VOID (*APLegacyFrameTx)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern VOID (*APFragmentFrameTx)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern NDIS_STATUS (*APHardTransmit)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
#if 0
BOOLEAN APCheckVaildDataFrame(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
NDIS_STATUS APInsertPsQueue(
		IN PRTMP_ADAPTER pAd,
		IN PNDIS_PACKET pPacket,
		IN STA_TR_ENTRY *tr_entry,
		IN UCHAR QueIdx);
#endif
extern VOID (*APFindCipherAlgorithm)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern VOID (*APBuildCache802_11Header)(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK 		*pTxBlk,
	IN UCHAR 		*pHeader);
extern VOID (*APBuildCommon802_11Header)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
BOOLEAN BuildHtcField(
	IN RTMP_ADAPTER 	*pAd,
	IN TX_BLK 			*pTxBlk,
	IN  MAC_TABLE_ENTRY *pMacEntry, 
	IN PUCHAR 			pHeaderBufPtr);

#endif /* CONFIG_AP_SUPPORT */


//mgmt_entrytb.c
#ifdef CONFIG_STA_SUPPORT
PSTA_ADMIN_CONFIG GetStaCfgByWdev(
	RTMP_ADAPTER 		*pAd,
	struct wifi_dev 	*pwdev);
extern MAC_TABLE_ENTRY *(*GetAssociatedAPByWdev)(
	RTMP_ADAPTER 		*pAd,
	struct wifi_dev   	*pwdev);
#endif
extern INT32 (*MacTableDelEntryFromHash)(
	RTMP_ADAPTER 		*pAd,
	MAC_TABLE_ENTRY 	*pEntry);
extern VOID (*MacTableReset)(RTMP_ADAPTER *pAd);
extern VOID (*TRTableInsertEntry)(
	RTMP_ADAPTER 	*pAd,
	UCHAR 			tr_tb_idx, 
	MAC_TABLE_ENTRY *pEntry);
extern VOID (*MgmtTableSetMcastEntry)(
	RTMP_ADAPTER 	*pAd,
	UCHAR 			wcid);
extern VOID (*TRTableInsertMcastEntry)(
	RTMP_ADAPTER 	*pAd,
	UCHAR 			tr_tb_idx, 
	struct wifi_dev *wdev);
#ifdef CONFIG_AP_SUPPORT
extern INT (*APMacTableResetEntry)(
	RTMP_ADAPTER 	*pAd, 
	MAC_TABLE_ENTRY *pEntry,
	BOOLEAN 		clean);
extern PMAC_TABLE_ENTRY (*APMacTableInsertEntry)(
	IN RTMP_ADAPTER 	*pAd,
	IN UCHAR 			*pAddr,
	IN struct wifi_dev 	*wdev,
	IN UINT32 			ent_type,
	IN UCHAR 			OpMode,
	IN BOOLEAN 			CleanAll);
extern VOID (*MacTableSetEntryPhyCfg)(
	RTMP_ADAPTER 	*pAd, 
	MAC_TABLE_ENTRY *pEntry);
#endif /* CONFIG_AP_SUPPORT */
extern VOID (*TRTableResetEntry)(
	RTMP_ADAPTER 	*pAd,
	UCHAR 			tr_tb_idx);
extern BOOLEAN (*MacTableDeleteEntry)(
	RTMP_ADAPTER 	*pAd,
	USHORT 			wcid,
	UCHAR 			*pAddr);


// cmm_asic_mt.c
#ifdef CONFIG_AP_SUPPORT
extern BOOLEAN (*MtSmacAsicSetArbBcnQCR)(
    RTMP_ADAPTER 	*pAd,
    UINT8 			OmacIdx,
    BOOLEAN 		enable);
extern INT (*MtSmacAsicEnableBeacon)(RTMP_ADAPTER *pAd, VOID *wdev_void, UCHAR NumOfBcns);
extern INT (*MtSmacAsicDisableBeacon)(RTMP_ADAPTER *pAd, VOID *wdev_void);
INT AsicSetPreTbtt(RTMP_ADAPTER *pAd, BOOLEAN bEnable, UCHAR HwBssidIdx);
extern VOID (*MtAsicSetMbssWdevIfAddr)(RTMP_ADAPTER *pAd, INT idx, UCHAR *if_addr, INT opmode);
#endif /* CONFIG_AP_SUPPORT */
extern INT32 (*MtAsicSetMacTxRx)(RTMP_ADAPTER *pAd, INT32 TxRx, BOOLEAN Enable,UCHAR BandIdx);
extern INT (*MtAsicSetPreTbtt)(struct _RTMP_ADAPTER *pAd, BOOLEAN bEnable,UINT32 BssidIdx);
INT MtAsicSetMacMaxLen(RTMP_ADAPTER *pAd);
VOID MtAsicEnableBssSyncByDriver(
			RTMP_ADAPTER 	*pAd,
			USHORT 			BeaconPeriod,
			UCHAR 			HWBssidIdx,
			UCHAR 			OPMode);
extern INT (*MtAsicSetTxStream)(RTMP_ADAPTER *pAd, UINT32 StreamNums,UCHAR BandIdx);
extern INT (*MtAsicSetRxStream)(RTMP_ADAPTER *pAd, UINT32 StreamNums,UCHAR BandIdx);
extern INT32 (*AsicDevInfoUpdate)(
		RTMP_ADAPTER 		*pAd,
		UINT8 				OwnMacIdx,
		UINT8 				*OwnMacAddr,
		UINT8 				BandIdx,
		UINT8 				Active,
		UINT32 				EnableFeature);

extern INT32 (*AsicBssInfoUpdate)(
		RTMP_ADAPTER 	*pAd,
		UCHAR 			OwnMacIdx,
		UINT8 			BssIndex,
		UCHAR 			*Bssid,
		UINT8 			BcMcWlanIdx,
		UINT32 			NetworkType,
		UINT32 			u4ConnectionType,
		UINT8 			Active,
		UINT32 			EnableFeature);

INT MtAsicSetBW(RTMP_ADAPTER *pAd, INT bw);
extern VOID (*Wtbl2RateTableUpdate)(RTMP_ADAPTER *pAd, UCHAR ucWcid, UINT32 u4Wtbl2D9, UINT32 *Rate);
extern VOID (*asic_mcs_lut_update)(RTMP_ADAPTER *pAd, MAC_TABLE_ENTRY *pEntry);
BOOLEAN WtblClearCounter(RTMP_ADAPTER *pAd,UINT8 Wcid);
BOOLEAN WtblWaitIdle(RTMP_ADAPTER *pAd, UINT32 WaitCnt,  UINT32 WaitDelay);

// cmm_cfg.c
BOOLEAN RT_isLegalCmdBeforeInfUp(RTMP_STRING *SetCmd);
#if ( MT7687_BORA00006349 == 1)
extern INT (*RT_CfgSetCountryRegion)(RTMP_ADAPTER *pAd, UINT_8 region, INT band);
#else
extern INT (*RT_CfgSetCountryRegion)(RTMP_ADAPTER *pAd, RTMP_STRING *arg, INT band);
#endif
extern UCHAR *(*wmode_2_str)(UCHAR wmode);
extern RT_802_11_PHY_MODE (*wmode_2_cfgmode)(UCHAR wmode);
extern UCHAR (*cfgmode_2_wmode)(UCHAR cfg_mode);
extern BOOLEAN (*wmode_band_equal)(UCHAR smode, UCHAR tmode);
INT RT_CfgSetWirelessMode(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
#ifdef CONFIG_AP_SUPPORT
#ifdef MBSS_SUPPORT
BOOLEAN wmode_valid(RTMP_ADAPTER *pAd, enum WIFI_MODE wmode);
UCHAR RT_CfgMbssWirelessModeMaxGet(RTMP_ADAPTER *pAd);
INT RT_CfgSetMbssWirelessMode(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
#endif /* MBSS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
BOOLEAN RT_isLegalCmdBeforeInfUp(RTMP_STRING *SetCmd);
INT RT_CfgSetShortSlot(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT RT_CfgSetWepKey(
		IN	PRTMP_ADAPTER	pAd,
		IN	RTMP_STRING 	*keyString,
		IN	CIPHER_KEY		*pSharedKey,
		IN	INT 			keyIdx);
INT RT_CfgSetWPAPSKKey(
		IN RTMP_ADAPTER *pAd,
		IN RTMP_STRING *keyString,
		IN INT			keyStringLen,
		IN UCHAR		*pHashStr,
		IN INT			hashStrLen,
		OUT PUCHAR		pPMKBuf);
INT RT_CfgSetFixedTxPhyMode(RTMP_STRING *arg);
extern INT (*RT_CfgSetMacAddress)(RTMP_ADAPTER *pAd, RTMP_STRING *arg);

INT	RT_CfgSetTxMCSProc(RTMP_STRING *arg, BOOLEAN *pAutoRate);
INT RTMP_COM_IoctlHandle(
		IN	VOID					*pAdSrc,
		IN	RTMP_IOCTL_INPUT_STRUCT *wrq,
		IN	INT 					cmd,
		IN	USHORT					subcmd,
		IN	VOID					*pData,
		IN	ULONG					Data);
#ifdef MAC_REPEATER_SUPPORT
#ifdef MULTI_MAC_ADDR_EXT_SUPPORT
INT Set_EnMultiMacAddrExt_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_MultiMacAddrExt_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
#endif /* MULTI_MAC_ADDR_EXT_SUPPORT */
#endif /* MAC_REPEATER_SUPPORT */
INT set_tssi_enable(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
INT Set_RadioOn_Proc(
		IN	PRTMP_ADAPTER		pAd,
		IN	RTMP_STRING 		*arg);
#ifdef MT_MAC
INT set_get_fid(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
#endif

// cmm_profile.c
BOOLEAN rtstrcasecmp(
	RTMP_STRING 	*s1,
	RTMP_STRING 	*s2);
unsigned long simple_strtoul(
	const char 		*cp,
	char 			**endp,
	unsigned int 	base);
long simple_strtol(
	const char 		*cp,
	char 			**endp,
	unsigned int 	base);
#ifdef IOT_SECURITY
int rtmp_parse_key_buffer_from_file(
	IN  RTMP_ADAPTER 	*pAd,
	IN  RTMP_STRING 	*buffer,
	IN  ULONG 			KeyType, 
	IN  INT 			BSSIdx, 
	IN  INT 			KeyIdx);
void rtmp_read_key_parms_from_file(
	RTMP_ADAPTER 	*pAd, 
	RTMP_STRING 	*tmpbuf,
	RTMP_STRING 	*buffer);
int rtmp_parse_wpapsk_buffer_from_file(
	RTMP_ADAPTER 	*pAd,
	RTMP_STRING 	*buffer,
	INT 			BSSIdx);
#endif
#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
#if 1//def LATER // NOT to be read from File
	void rtmp_read_ap_client_from_file(
		IN PRTMP_ADAPTER 	pAd,
		IN RTMP_STRING 		*_tmpbuf,
		IN RTMP_STRING 		*buffer);
#endif
#endif
#ifdef IOT_WMM
extern void (*rtmp_read_ap_wmm_parms_from_file)(
RTMP_ADAPTER 	*pAd,
RTMP_STRING 	*tmpbuf,
RTMP_STRING 	*buffer);
#endif
#ifdef DOT1X_SUPPORT
void rtmp_read_radius_parms_from_file(
RTMP_ADAPTER 	*pAd,
RTMP_STRING 	*tmpbuf,
RTMP_STRING 	*buffer);
#endif
#endif

#ifdef IOT_WMM
#ifdef CONFIG_STA_SUPPORT
void rtmp_read_sta_wmm_parms_from_file(
	IN  PRTMP_ADAPTER 	pAd,
	char 				*tmpbuf,
	char 				*buffer);
#endif
#endif
#ifdef DOT11_N_SUPPORT
extern void (*HTParametersHook)(
	IN	RTMP_ADAPTER 		*pAd,
	IN	RTMP_STRING 		*pValueStr,
	IN	RTMP_STRING		 	*pInput);
#endif
#ifdef CONFIG_STA_SUPPORT
extern void (*RTMPSetSTASSID)(
	RTMP_ADAPTER 		*pAd, 
	RTMP_STRING 		*SSID);
void RTMPSetSTAPassPhrase(
	RTMP_ADAPTER 		*pAd,
	RTMP_STRING 		*PassPh);
#ifdef  CREDENTIAL_STORE
NDIS_STATUS RecoverConnectInfo(
	IN  RTMP_ADAPTER 	*pAd);
NDIS_STATUS StoreConnectInfo(
	IN	RTMP_ADAPTER 	*pAd);
#endif
#endif
extern void (*RTMPSetCountryCode)(
	RTMP_ADAPTER 	*pAd,
	RTMP_STRING 	*CountryCode);
	extern NDIS_STATUS (*RTMPSetProfileParameters)(
		IN RTMP_ADAPTER 	*pAd,
		IN RTMP_STRING 		*_pBuffer);
#ifdef WSC_INCLUDED
void rtmp_read_wsc_user_parms(
	PWSC_CTRL 		pWscControl,
	RTMP_STRING 	*tmpbuf,
	RTMP_STRING 	*buffer);
void rtmp_read_wsc_user_parms_from_file(
	IN PRTMP_ADAPTER 	pAd,
	char 				*tmpbuf, 
	char 				*buffer);
#endif

// mt_ps.c
#ifdef MT_PS
VOID MtReadPseRdTabAccessReg(
		RTMP_ADAPTER 	*pAd, 
		UCHAR 			wcid, 
		UINT32 			*p_rPseRdTabAccessReg);
VOID MtClearPseRdTab(
		RTMP_ADAPTER 	*pAd, 
		UCHAR 			wcid );
#ifdef CONFIG_WTBL_TLV_MODE	
#else
VOID MtSetIgnorePsmByDriver(
		RTMP_ADAPTER 	*pAd,
		MAC_TABLE_ENTRY *pEntry,
		UCHAR 			value);
#endif
VOID MtSetIgnorePsm(
		RTMP_ADAPTER 	*pAd,
		MAC_TABLE_ENTRY *pEntry,
		UCHAR			 value);
VOID CheckSkipTX(
		RTMP_ADAPTER 	*pAd,
		MAC_TABLE_ENTRY *pEntry);
#endif
#if 0
VOID MtHandleRxPsPoll(
		RTMP_ADAPTER 	*pAd,
		UCHAR 			*pAddr,
		USHORT 			wcid,
		BOOLEAN 		isActive);

#endif
BOOLEAN MtPsIndicate(
		RTMP_ADAPTER 	*pAd,
		UCHAR 			*pAddr,
		UCHAR 			wcid,
		UCHAR 			Psm);
#ifdef MT_PS
VOID MtPsRedirectDisableCheck(
		RTMP_ADAPTER 	*pAd,
		UCHAR 			wcid);
VOID MtPsSendToken(
		RTMP_ADAPTER 	*pAd,
		UINT32 			WlanIdx);
VOID MtPsRecovery(
		RTMP_ADAPTER 	*pAd);
VOID MtEnqTxSwqFromPsQueue(
		RTMP_ADAPTER 	*pAd,
		UCHAR 			qidx, 
		STA_TR_ENTRY 	*tr_entry);
#endif

// mt_mac.c
extern INT (*mt_wtbl_get_entry234)(
		RTMP_ADAPTER 		*pAd, 
		UCHAR 				widx, 
		struct wtbl_entry 	*ent);
extern INT (*mt_wtbl_init)(RTMP_ADAPTER *pAd);
extern INT (*mt_hw_tb_init)(RTMP_ADAPTER *pAd, BOOLEAN bHardReset);
VOID NicResetRawCounters(RTMP_ADAPTER *pAd);
extern UINT16 (*tx_rate_to_tmi_rate)(
		UINT8 	mode,
		UINT8 	mcs, 
		UINT8	nss,
		BOOL 	stbc, 
		UINT8 	preamble);
extern UCHAR (*get_nss_by_mcs)(
		UCHAR 	phy_mode,
		UCHAR 	mcs,
		BOOLEAN stbc);
#ifdef UNIFIED_TXD_RXD
VOID MtWriteTMacInfo(
		IN RTMP_ADAPTER 	*pAd,
		IN UCHAR 			*buf,
		IN TMAC_INFO 		*TxInfo);
#endif
#ifdef COMPOS_TESTMODE_WIN
VOID MtWriteTMacInfoTestMode(
		IN RTMP_ADAPTER 		*pAd,
		IN UCHAR 				*buf,
		IN struct TM_TMAC_INFO 	*TxInfo);
#endif
#ifdef COMPOS_WIN
VOID XmitWriteTxD(
		IN PMP_ADAPTER		  pAd,
		IN PMP_PORT 		  pPort,
		IN PTMAC_TXD_S		  pShortFormatTxD,
		IN PTMAC_TXD_L		  pLongFormatTxD,
		IN HARD_TRANSMIT_INFO TxInfo);
#endif
VOID write_tmac_info(
		IN RTMP_ADAPTER 		*pAd,
		IN UCHAR 				*tmac_info,
		IN MAC_TX_INFO 			*info,
		IN HTTRANSMIT_SETTING 	*pTransmit);
VOID write_tmac_info_Data(
		RTMP_ADAPTER 	*pAd,
		UCHAR 			*buf,
		TX_BLK 			*pTxBlk);
VOID write_tmac_info_Cache(
		RTMP_ADAPTER	*pAd,
		UCHAR 			*buf, 
		TX_BLK 			*pTxBlk);

// sta_cfg.c
INT Set_SSID_Proc(
		RTMP_ADAPTER 	*pAd,
		CHAR 			*arg);
INT	Set_WmmCapable_Proc(
		RTMP_ADAPTER 	*pAd,
		CHAR 			*arg);
INT Set_NetworkType_Proc(
		RTMP_ADAPTER 	*pAd,
		CHAR 			*arg);
INT Set_AuthMode_Proc(
		RTMP_ADAPTER 	*pAd,
		CHAR 			*arg);
INT Set_EncrypType_Proc(
		RTMP_ADAPTER 	*pAd,
		CHAR 			*arg);
INT Set_DefaultKeyID_Proc(
		RTMP_ADAPTER 	*pAdapter,
		UINT8 			KeyIdx);
INT Set_Wep_Key_Proc(
	    IN  RTMP_ADAPTER   *pAdapter,
	    IN  CHAR 			*Key,
	    IN  INT             KeyLen,
	    IN  INT             KeyId);
#ifdef ETH_CONVERT_SUPPORT
INT Set_FragTest_Proc(
		RTMP_ADAPTER 	*pAdapter,
		UCHAR 			*arg);
#endif
VOID StaSiteSurvey(
	IN	PRTMP_ADAPTER  		pAd,
	IN	PNDIS_802_11_SSID	pSsid,
	IN	UCHAR				ScanType);
#ifdef WIDI_SUPPORT
INT Set_WiDiEnable_Proc(
		RTMP_ADAPTER 	*pAd, 
		UCHAR	 		*arg);
#endif
#ifdef IOT_WOW_SUPPORT
int rtinet_aton(
		UCHAR 			*cp,
		unsigned int 	*addr);
INT Set_WOW_Enable(
		RTMP_ADAPTER 	*pAd,
		BOOLEAN 		enable);
INT Set_WOW_GPIO(
		RTMP_ADAPTER 	*pAd, 
		UINT8 			Value);
INT Set_WOW_Delay(
		RTMP_ADAPTER 	*pAd,
		UINT8 			Value);
INT Set_WOW_Hold(
		RTMP_ADAPTER 	*pAd,
		UINT32 			Value);
INT Set_WOW_InBand(
		RTMP_ADAPTER 	*pAd, 
		UINT8 			Value);
INT Set_WOW_Interface(
		RTMP_ADAPTER 	*pAd,
		UINT8 			Value);
INT Set_WOW_IPAddress(
		RTMP_ADAPTER 	*pAd,
		UCHAR 			*arg);
INT Set_WOW_GPIOHighLow(
		RTMP_ADAPTER 	*pAd, 
		UINT8 			Value);
#endif /* IOT_WOW_SUPPORT */

INT RtmpIoctl_rt_ioctl_siwauth(
		IN	RTMP_ADAPTER	*pAd,
		IN	VOID			*pData,
		IN	ULONG			Data);
void fnSetCipherKey(
	    IN  PRTMP_ADAPTER   pAd,
	    IN  INT             keyIdx,
	    IN  UCHAR           CipherAlg,
	    IN  BOOLEAN         bGTK,
	    IN  UCHAR			*pKey,
	    IN  struct wifi_dev *wdev);
INT RtmpIoctl_rt_ioctl_siwencodeext(
		RTMP_ADAPTER 	*pAd, 
		VOID 			*pData,
		ULONG 			Data,
		struct wifi_dev *wdev);
INT RtmpIoctl_rt_ioctl_siwpmksa(
		IN	RTMP_ADAPTER		*pAd,
		IN	VOID				*pData,
		IN	ULONG				Data);
INT RTMP_STA_IoctlHandle(
		IN	VOID					*pAdSrc,
		IN	RTMP_IOCTL_INPUT_STRUCT	*pRequest,
		IN	INT						Command,
		IN	USHORT					Subcmd,
		IN	VOID					*pData,
		IN  ULONG					Data,
		IN  USHORT                  priv_flags);

// wdev.c
INT rtmp_wdev_idx_unreg(
		RTMP_ADAPTER 		*pAd,
		struct wifi_dev 	*wdev);
extern INT32 (*rtmp_wdev_idx_reg)(
		RTMP_ADAPTER 		*pAd,
		struct wifi_dev 	*wdev);
extern INT32 (*GetBssIdx)(RTMP_ADAPTER *pAd);
VOID ReleaseBssIdx(
		RTMP_ADAPTER 	*pAd,
		UINT32 			BssIdx);
extern INT32 (*wdev_init)(
		RTMP_ADAPTER 	*pAd, 
		struct wifi_dev *wdev,
		enum WDEV_TYPE 	WdevType, 
		PNET_DEV 		IfDev, 
		INT8			func_idx,
		UINT8 			tr_tb_idx,
		VOID 			*func_dev, 
		VOID 			*sys_handle);
INT32 wdev_deinit(
		RTMP_ADAPTER 		*pAd,
		struct wifi_dev 	*wdev);
extern struct wifi_dev *(*WdevSearchByAddress)(
		RTMP_ADAPTER 	*pAd, 
		UCHAR 			*Address);
extern struct wifi_dev *(*get_wdev_by_idx)(
		RTMP_ADAPTER 	*pAd, 
		INT 			idx);
struct wifi_dev *get_wdev_by_type(
		RTMP_ADAPTER 	*pAd, 
		enum WDEV_TYPE 	wdev_type, 
		UINT32 func_index);
extern struct wifi_dev *(*WdevSearchByOmacIdx)(
		RTMP_ADAPTER 	*pAd,
		UINT8 			BssIndex);

// rt_bora.c
extern void (*SysCfg_Init)();

PUCHAR SysCfg_Get_SysMacAddr(void);

extern void (*SysCfg_Set_SysMacAddr)(UCHAR *mac);
BOOLEAN SysCfg_Get_MacSync(void);
void SysCfg_Set_MacSync(BOOLEAN status);
BOOLEAN SysCfg_Get_HeartBeatState(void);
VOID SysCfg_Toggle_HeartBeatState(void);
void dump_profile();

#ifdef CONFIG_AP_SUPPORT
PNDIS_PACKET duplicate_pkt_with_VLAN(
		IN  PRTMP_ADAPTER   pAd, 
		IN  PUCHAR          pHeader802_3,
		IN  UINT            HdrLen,
		IN  PUCHAR          pData,
		IN  ULONG           DataSize,
		IN  UCHAR           FromWhichBSSID);
extern UCHAR (*VLAN_8023_Header_Copy)(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pHeader802_3,
	IN	UINT            HdrLen,
	OUT PUCHAR			pData,
	IN	UCHAR			FromWhichBSSID);

#endif
extern VOID (*Announce_or_Forward_802_3_Packet)(
	IN RTMP_ADAPTER 	*pAd,
	IN PNDIS_PACKET 	pPacket,
	IN UCHAR 			wdev_idx);
UINT32 RtmpOSWirelessEventTranslate(IN UINT32 eventType);
NDIS_STATUS IPMacTableUpdate(
	IN MAT_STRUCT		*pMatCfg,
	IN PUCHAR			pMacAddr,
	IN UINT				ipAddr);
PUCHAR IPMacTableLookUp(
	IN MAT_STRUCT	*pMatCfg,
	IN	UINT		ipAddr);
NDIS_STATUS IPMacTable_RemoveAll(
	IN MAT_STRUCT *pMatCfg);
NDIS_STATUS IPMacTable_init(
	IN MAT_STRUCT *pMatCfg);
NDIS_STATUS MATProto_IP_Init(MAT_STRUCT *pMatCfg);
NDIS_STATUS MATProto_IP_Exit(MAT_STRUCT *pMatCfg);
PUCHAR MATProto_IP_Rx(
	MAT_STRUCT 		*pMatCfg,
	PNDIS_PACKET 	pSkb, 
	PUCHAR 			pLayerHdr, 
	PUCHAR 			pMacAddr);
PUCHAR MATProto_IP_Tx(
	MAT_STRUCT 		*pMatCfg,
	PNDIS_PACKET 	pSkb,
	PUCHAR 			pLayerHdr,
	PUCHAR 			pMacAddr);

NDIS_STATUS MATProto_ARP_Init(MAT_STRUCT *pMatCfg);
NDIS_STATUS MATProto_ARP_Exit(MAT_STRUCT *pMatCfg);
PUCHAR MATProto_ARP_Rx(
	MAT_STRUCT 		*pMatCfg,
	PNDIS_PACKET 	pSkb,
	PUCHAR 			pLayerHdr,
	PUCHAR 			pMacAddr);
PUCHAR MATProto_ARP_Tx(
	MAT_STRUCT 		*pMatCfg,
	PNDIS_PACKET 	pSkb,
	PUCHAR 			pLayerHdr,
	PUCHAR 			pMacAddr);
NDIS_STATUS MATDBEntryFree(
	IN MAT_STRUCT 	*pMatStruct, 
	IN PUCHAR 		NodeEntry);
PUCHAR MATDBEntryAlloc(
	IN MAT_STRUCT 	*pMatStruct,
	IN UINT32 		size);

#if 0
PUCHAR MATEngineTxHandle(
		IN PRTMP_ADAPTER	pAd,
		IN PNDIS_PACKET 	pPkt,
		IN UINT 			ifIdx,
		IN UCHAR			OpMode);
PUCHAR MATEngineRxHandle(
		IN PRTMP_ADAPTER	pAd,
		IN PNDIS_PACKET 	pPkt,
		IN UINT 			infIdx);
BOOLEAN MATPktRxNeedConvert(
			IN PRTMP_ADAPTER	pAd, 
			IN PNET_DEV 		net_dev);
NDIS_STATUS MATEngineExit(
		IN RTMP_ADAPTER 	*pAd);
NDIS_STATUS MATEngineInit(
		IN RTMP_ADAPTER 	*pAd);

#endif
VOID CFG80211OS_NewSta(
	IN PNET_DEV pNetDev,
	IN const PUCHAR mac_addr,
	IN const PUCHAR assoc_frame,
	IN UINT32 assoc_len);

#endif /* _IOT_ROM_API_PRIVATE_H */
