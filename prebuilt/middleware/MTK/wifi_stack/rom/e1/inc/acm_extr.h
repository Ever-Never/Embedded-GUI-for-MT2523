/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	acm_extr.h

	Abstract:
	Used by other modules.

	Revision History:
	Who			When			What
	----------	----------		----------------------------------------------
	Sample Lin	06-18-2007		Create first
*/

#ifndef __ACM_EXTR_H__
#define __ACM_EXTR_H__


/* Porting Guide:
	.	ACMR_xx definitions

	.	EACM_WME_SETUP_RspSend()
	.	EACM_WME_TeardownSendToQSTA()
	.	EACM_WME_SETUP_ReqSend()
	.	EACM_WME_TeardownSendToQAP()

	.	ACM_Init()
	.	ACM_MSDU_Classify()

	.	ACM_APSD_Ctrl()
	.	ACM_TR_STM_Check()
	.	ACM_TR_STM_CheckDisable()
	.	ACM_TR_STM_CheckEnable()
	.	ACM_TR_TC_ReqCheck()
	.	ACM_TR_TC_ReqCheckDisable()
	.	ACM_TR_TC_ReqCheckEnable()
 */

/* -------------------------------------------------------------------------

	ACM			Adimission Control Mechanism
	ACM_CC		ACM Condition Compile

   ------------------------------------------------------------------------- */




/* ========================================================================== */
/* Definition */

#ifdef MODULE_ACM
	#define ACM_EXTERN
#else
	#define ACM_EXTERN extern
#endif // MODULE_ACM //


/* OS-dependence */ /* change it based on your environment */
#define ACM_CC_OS_LINUX					/* LINUX OS */
#define ACM_CC_FUNC_WMM					/* WMM ACM function */
#define ACM_CC_FUNC_SOFT_ACM			/* WMM software ACM function */

#define CONFIG_STA_SUPPORT_SIM			/* APC simulation (test use) */

#define ACM_LITTLE_ENDIAN
#define ACM_MEMORY_TEST

/* ex: OS_HZ = 100 means 100 ticks in a second, 1 jiffies = 1000000/100us */
#define ACM_JIFFIES_BASE				(1000000/OS_HZ)


/* Type Redefinition */
#ifndef UINT8
#define UINT8	unsigned char
#endif // UINT8 //

#ifndef UINT16
#define UINT16	unsigned short
#endif // UINT16 //

#ifndef UINT32
#define UINT32	unsigned int
#endif // UINT32 //

#ifndef INT8
#define INT8	signed char
#endif // INT8 //

#ifndef INT16
#define INT16	signed short
#endif // INT16 //

#ifndef INT32
#define INT32	signed int
#endif // INT32 //

#ifndef NULL
#define NULL	0
#endif // NULL //

//#define STATIC	static
#define STATIC


/* ACTION FRAME */
/* In WMMv1.1 these names are changed to ADDTS Req/Rsp DELTS */
#define ACM_CATEGORY_WME						17 /* WME Action */
#define ACM_ACTION_WME_SETUP_REQ            	0
#define ACM_ACTION_WME_SETUP_RSP				1
#define ACM_ACTION_WME_TEAR_DOWN			    2

/* public qos element */
#define ACM_ELM_TOTAL_LEN	(ACM_ELM_QBSS_LOAD_LEN+2+ACM_ELM_WME_PARAM_LEN+2)

#define ACM_ELM_QBSS_LOAD_ID					11
#define ACM_ELM_QBSS_LOAD_LEN					5
#define ACM_ELM_TSPEC_ID						13
#define ACM_ELM_TSPEC_LEN						63
#define ACM_ELM_TCLAS_ID						14
/* TCLASS element length is variable */
#define ACM_ELM_TCLAS_PROCESSING_ID				44
#define ACM_ELM_TCLAS_PROCESSING_LEN			1
#define ACM_ELM_QOS_ACTION_ID					45
/* QOS ACTION element length is variable */
#define ACM_ELM_QOS_CAP_ID						46
#define ACM_ELM_QOS_CAP_LEN						1

/* public wme element */
#define ACM_ELM_WME_ID							221
#define ACM_ELM_WME_INFO_LEN					7
#define ACM_ELM_WME_PARAM_LEN					24
#define ACM_ELM_WME_TSPEC_LEN					61
#define ACM_ELM_WME_TCLAS_LEN					61
#define ACM_ELM_WME_TCLAS_PROCESSING_LEN		7

#define ACM_NOT_FRAME_BODY_LEN					(4 + 2 + ACM_ELM_WME_TSPEC_LEN)

#define ACM_WME_OUI_SUBTYPE_INFO				0x00
#define ACM_WME_OUI_SUBTYPE_PARAM				0x01
#define ACM_WME_OUI_SUBTYPE_TSPEC				0x02
#define ACM_WSM_OUI_SUBTYPE_QOS_CAP				0x05
#define ACM_WSM_OUI_SUBTYPE_TCLAS				0x06
#define ACM_WSM_OUI_SUBTYPE_TCLAS_PROCESSING	0x07
#define ACM_WSM_OUI_SUBTYPE_TS_DELAY			0x08
#define ACM_WSM_OUI_SUBTYPE_SCHEDULE			0x09

/* element ID, length, OUI, OUI Type, OUI Subtype, Version = 8B */
#define ACM_WME_ELM_OFFSET						8

/* public QOS Return Status Code */
#define ACM_PARAM_IN
#define ACM_PARAM_OUT
#define ACM_PARAM_IN_OUT

#define ACM_FUNC_STATUS							INT32

#define ACM_RTN_OK								(0)
#define ACM_RTN_FAIL							(-1)
#define ACM_RTN_FATAL_ERR						(-2)
#define ACM_RTN_NO_FREE_TS						(-3)
#define ACM_RTN_ALLOC_ERR						(-4)
#define ACM_RTN_INSUFFICIENT_BD					(-5)
#define ACM_RTN_CAP_LIMIT_EXCEED				(-6)
#define ACM_RTN_NULL_POINTER					(-7)
#define ACM_RTN_INVALID_PARAM					(-8)
#define ACM_RTN_SEM_GET_ERR						(-9)
#define ACM_RTN_DISALLOW						(-10)
#define ACM_RTN_RENO_IN_REQ_LIST				(-11)
#define ACM_RTN_EXIST							(-12)
#define ACM_RTN_NOT_EXIST						(-13)
#define ACM_RTN_INSUFFICIENT_BD_BUT_DEL_AC		(-14)
#define ACM_RTN_TCLAS_IS_NEEDED					(-15)

/* STREAM Type */
#define ACM_QOS_TYPE_LEGACY						0x01	/* no use */
#define ACM_QOS_TYPE_DELTS						0x02	/* use */
#define ACM_QOS_TYPE_ADDTS_REQ					0x04	/* no use */
#define ACM_QOS_TYPE_ADDTS_RSP					0x08	/* no use */
#define ACM_QOS_TYPE_NULL						0x10	/* use */

/* STREAM Category */
#define ACM_SM_CATEGORY_REQ						0
#define ACM_SM_CATEGORY_ACT						1
#define ACM_SM_CATEGORY_CDB						2
#define ACM_SM_CATEGORY_ERR						3

/* WMM Related */
#define ACM_STA_TSPEC_MAX_NUM		4	/* for UP or DN, total 4 + 4 = 8 */
										/* must >= ACM_STA_NUM_OF_AC */
#define ACM_TSPEC_TCLAS_MAX_NUM		5	/* max 5 TCLAS maps to a TSPEC */
#define ACM_DEV_NUM_OF_AC			4	/* AC0 ~ AC3 */
#define ACM_STA_TID_MAX_NUM			8	/* 0 ~ 7 for EDCA, dont change it */

/* Traffic Classify */
#define ACM_CLSFY_NOT_ALLOW			0xFFFFFFFF




/* ========================================================================== */
/* MACRO */

/* MACRO: only for QAP or only for QSTA */

#ifdef CONFIG_AP_SUPPORT
#define ACMR_TXOP_BSS_GET(ad_p, ac_id)	(ad_p->ApCfg.BssEdcaParm.Txop[ac_id])

#define ACMR_WMM_IS_ENABLED(ad_p)		ad_p->ApCfg.MBSSID[BSS0].bWmmCapable

#define ACMR_AP_ADDR_GET(ad_p)			ad_p->MlmeAux.Bssid

#define ACMR_STA_ENTRY_GET(ad_p, mac_p)	MacTableLookup(ad_p, mac_p)

#define ACMR_IS_AP_MODE					1

#define ACMR_IS_ASSOCIATED(ad_p)		1

#define ACMR_AC_ACM_CTRL(ad_p, flg0, flg1, flg2, flg3)	\
		ad_p->ApCfg.BssEdcaParm.bACM[QID_AC_BE] = flg0;	\
		ad_p->ApCfg.BssEdcaParm.bACM[QID_AC_BK] = flg1;	\
		ad_p->ApCfg.BssEdcaParm.bACM[QID_AC_VI] = flg2;	\
		ad_p->ApCfg.BssEdcaParm.bACM[QID_AC_VO] = flg3;

#define ACMR_STA_IS_SPREAMBLE(ad_p, cdb_p)	(CAP_IS_SHORT_PREAMBLE_ON(cdb_p->CapabilityInfo))
#endif // CONFIG_AP_SUPPORT //


#ifdef CONFIG_STA_SUPPORT
#define ACMR_TXOP_BSS_GET(ad_p, ac_id)	ACMR_TXOP_AP_GET(ad_p, ac_id)

#define ACMR_WMM_IS_ENABLED(ad_p)		ad_p->MlmeAux.APEdcaParm.bValid

#define ACMR_AP_ADDR_GET(ad_p)			ad_p->MlmeAux.Bssid

	/*	only one entry of QAP in QSTA */
#define ACMR_STA_ENTRY_GET(ad_p, mac_p)		\
		ad_p->MacTab.Content[BSSID_WCID].ValidAsCLI ? (&ad_p->MacTab.Content[BSSID_WCID]) : NULL

#define ACMR_IS_AP_MODE					0

#define ACMR_IS_ASSOCIATED(ad_p)		(ad_p->IndicateMediaState == NdisMediaStateConnected)

#define ACMR_STA_IS_SPREAMBLE(ad_p, cdb_p)	(ad_p->CommonCfg.TxPreamble == Rt802_11PreambleShort)

	/*	temporarily active our station to send ADDTS req */
#define ACMR_STA_PS_MODE_ACTIVE(ad_p)	\
	{	ACMR_DB->flg_ps_is_addts_req_sent = TRUE;							\
		if (OPSTATUS_TEST_FLAG(ad_p, fOP_STATUS_DOZE)) {					\
			ACMR_DB->ps_mode_backup = ad_p->StaCfg.WindowsBatteryPowerMode;	\
			MlmeSetPsmBit(ad_p, PWR_ACTIVE);								\
			OPSTATUS_SET_FLAG(ad_p, fOP_STATUS_RECEIVE_DTIM);				\
			if (ad_p->StaCfg.bWindowsACCAMEnable == FALSE)					\
				ad_p->StaCfg.WindowsPowerMode = Ndis802_11PowerModeCAM;		\
			ad_p->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeCAM;	\
			AsicForceWakeup(ad_p);											\
			if (!(ad_p->CommonCfg.bAPSDCapable &&							\
				ad_p->CommonCfg.APEdcaParm.bAPSDCapable))					\
				RTMPSendNullFrame(ad_p, ad_p->CommonCfg.TxRate, FALSE);		\
			else RTMPSendNullFrame(ad_p, ad_p->CommonCfg.TxRate, TRUE);		\
		} else ACMR_DB->ps_mode_backup = Ndis802_11PowerModeCAM;	}

	/* recover the old PS mode */
#define ACMR_STA_PS_MODE_RECOVER(ad_p)	\
	{	if ((ACMR_DB->flg_ps_is_addts_req_sent == TRUE) &&					\
			(ACMR_DB->ps_mode_backup != Ndis802_11PowerModeCAM)) {			\
			if (ACMR_DB->ps_mode_backup == Ndis802_11PowerModeFast_PSP) {	\
				OPSTATUS_SET_FLAG(ad_p, fOP_STATUS_RECEIVE_DTIM);			\
				if (ad_p->StaCfg.bWindowsACCAMEnable == FALSE)				\
					ad_p->StaCfg.WindowsPowerMode = Ndis802_11PowerModeFast_PSP;	\
				ad_p->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeFast_PSP; \
				ad_p->StaCfg.DefaultListenCount = 3;						\
			} else if (ACMR_DB->ps_mode_backup == Ndis802_11PowerModeMAX_PSP) {		\
				OPSTATUS_SET_FLAG(ad_p, fOP_STATUS_RECEIVE_DTIM);			\
				if (ad_p->StaCfg.bWindowsACCAMEnable == FALSE)				\
					ad_p->StaCfg.WindowsPowerMode = Ndis802_11PowerModeMAX_PSP;		\
				ad_p->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeMAX_PSP;	\
				ad_p->StaCfg.DefaultListenCount = 5;						\
            } if (ACMR_DB->ps_mode_backup == Ndis802_11PowerModeLegacy_PSP) {		\
				OPSTATUS_SET_FLAG(ad_p, fOP_STATUS_RECEIVE_DTIM);			\
				if (ad_p->StaCfg.bWindowsACCAMEnable == FALSE)				\
					ad_p->StaCfg.WindowsPowerMode = Ndis802_11PowerModeLegacy_PSP;	\
				ad_p->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeLegacy_PSP; \
				ad_p->StaCfg.DefaultListenCount = 3; }						\
			if (!(ad_p->CommonCfg.bAPSDCapable &&							\
				ad_p->CommonCfg.APEdcaParm.bAPSDCapable))					\
				RTMPSendNullFrame(ad_p, ad_p->CommonCfg.TxRate, FALSE);		\
			else RTMPSendNullFrame(ad_p, ad_p->CommonCfg.TxRate, TRUE);		\
		ACMR_DB->flg_ps_is_addts_req_sent = FALSE; } }
#endif // CONFIG_STA_SUPPORT //


/* MACRO: others related */

	/* 	for debug information */
#define ACMR_DEBUG					IOT_DBGPRINT
#define ACMR_DEBUG_TRACE			RT_DEBUG_TRACE
#define ACMR_DEBUG_ERR				RT_DEBUG_ERROR

	/* check is the ACM function is enabled */
#define ACMR_IS_ENABLED(ad_p)		((ad_p != NULL) && \
									(ACMR_WMM_IS_ENABLED(ad_p)) && \
									(ACM_IsAnyACEnabled(ad_p) == ACM_RTN_OK))

	/* 	for Device Adapter Control Block & ACM Control Block of pAd */
#define ACMR_PWLAN_STRUC			PRTMP_ADAPTER
#define ACMR_DB						((ACM_CTRL_BLOCK *)ad_p->pACM_Ctrl_BK)
#define ACMR_ADAPTER_DB			    ad_p->pACM_Ctrl_BK

	/* 	for protection spin lock/unlock */
#define ACMR_LOCK_INIT				NdisAllocateSpinLock
#define ACMR_LOCK_FREE				NdisFreeSpinLock

	/* 	get self's MAC address */
#define ACMR_SELF_MAC_GET(ad_p)	    ad_p->PermanentAddress

	/*	get current timestamp low 32-bit value */
#define ACMR_TIMESTAMP_GET(ad_p, data)					\
	{	UINT32 csr;	UINT64 tmp;							\
		RTMP_IO_READ32(ad_p, TSF_TIMER_DW0, &csr);		\
		data = (UINT64)csr;								\
		RTMP_IO_READ32(ad_p, TSF_TIMER_DW1, &csr);		\
		tmp = (UINT64)csr;								\
		data |= (tmp << 32); }

	/* channel busy time, bit 0 must be set to 1 to enable */
#define ACMR_CHAN_BUSY_DETECT_ENABLE(ad_p)	\
	{	RTMP_IO_WRITE32(ad_p, CH_TIME_CFG, 0x0000001F); }

#define ACMR_CHAN_BUSY_DETECT_DISABLE(ad_p)	\
	{	RTMP_IO_WRITE32(ad_p, CH_TIME_CFG, 0x00000000); }

#define ACMR_CHAN_BUSY_GET(ad_p, time)		\
	{	RTMP_IO_READ32(ad_p, CH_BUSY_STA, &time); }


/* MACRO: WMM */

	/* 	get the WMM TXOP parameters of QAP, not BSS (used for QAP mode) */
#define ACMR_TXOP_AP_GET(ad_p, ac_id)	(ad_p->CommonCfg.APEdcaParm.Txop[ac_id])

	/*	QID for AC0 ~ AC1 */
#define ACMR_QID_AC_BE					QID_AC_BE
#define ACMR_QID_AC_BK					QID_AC_BK
#define ACMR_QID_AC_VI					QID_AC_VI
#define ACMR_QID_AC_VO					QID_AC_VO


/* MACRO: device (ap & station) */

	/* 	for Device Entry & ACM Control Block of Device */
#define ACMR_STA_DB						MAC_TABLE_ENTRY
#define ACMR_STA_ACM_PARAM_INFO(cdb_p)	((ACM_ENTRY_INFO *)(&cdb_p->ACM_Info))

	/* 	1. check if the STA is QSTA;
		2. get Maximum number of device entry;
		3. check if the device entry is valid; */
#define ACMR_IS_QSTA(cdb_p)				ACMR_IS_WMM_STA(cdb_p)
#define ACMR_STA_MAX_NUM(_pAd)				MAX_LEN_OF_MAC_TABLE(_pAd)
#define ACMR_ENTRY_ID_VALID(cdb_p)		cdb_p->ValidAsCLI

	/*	1. get a device entry based on array index;
		2. check if the device entry is valid; */
#define ACMR_STA_GET(ad_p, cdb_p, i)	(&ad_p->MacTab.Content[i])
#define ACMR_STA_IS_VALID(cdb_p)			\
	((cdb_p != NULL) && (cdb_p->ValidAsCLI) && ((*(UINT32 *)ACMR_CLIENT_MAC(cdb_p) != 0)))

	/*	1. get MAC address of a device entry;
		2. get Current tx rate of a device entry; */
#define ACMR_CLIENT_MAC(cdb_p)			(cdb_p)->Addr
#define ACMR_CLIENT_PHY_MODE_MCS_GET(cdb_p, phy_mode, mcs)			\
	{	HTTRANSMIT_SETTING *tx_phy_mode_p = &cdb_p->HTPhyMode;		\
		phy_mode = tx_phy_mode_p->field.MODE;						\
		mcs = tx_phy_mode_p->field.MCS; }
#define ACMR_CLIENT_CCK_RATE_INDEX_GET(mcs, rate_index)				\
	{	if (mcs <= 3) rate_index = mcs;								\
		else if ((mcs >= 8) && (mcs <= 11)) rate_index = mcs - 8; }
#define ACMR_CLIENT_OFDM_RATE_INDEX_GET(mcs, rate_index)			\
	{	if (mcs <= 7) rate_index = mcs+4; }

	/*	1. check if the station associates to us (QAP mode);
		2. check if the station is a WMM device;
		3. check if the preamble of stations is short preamble; */
#define ACMR_STA_IS_ASSOC(cdb_p)		(cdb_p->Sst == SST_ASSOC)
#define ACMR_IS_WMM_STA(cdb_p)			CLIENT_STATUS_TEST_FLAG(cdb_p, fCLIENT_STATUS_WMM_CAPABLE)

	/*	get current associated station count */
#define ACMR_STA_CUR_COUNT(ad_p)		1	// yet implement


/* MACRO: packet */

	/*	1. packet structure pointer type;
		2. wlan header structure;
		3. get the type field of a frame;
		4. get the subtype field of a frame;
		5. get the qos field of a frame;
		6. the sub type definition of QoS Data;
		7. get legacy wlan header size;
		8. get QoS wlan header size; */
#define ACMR_MBUF						NDIS_PACKET
#define ACMR_WLAN_HEADER				HEADER_802_11
#define ACMR_FME_TYPE_GET(hdr_p)		(hdr_p->FC.Type)
#define ACMR_FME_SUBTYPE_GET(hdr_p)		(hdr_p->FC.SubType)
#define ACMR_FME_QOSCTRL_GET(hdr_p)		(*((UINT16 *)hdr_p->Octet))
#define ACMR_FME_SUB_TYPE_QOS_DATA		SUBTYPE_QDATA
#define ACMR_FME_LEG_HEADER_SIZE		(sizeof(HEADER_802_11))
#define ACMR_FME_QOS_HEADER_SIZE		(sizeof(HEADER_802_11)+2)

	/*	1. allocate a wlan packet;
		2. copy data to the wlan packet;
		3. free a wlan packet; */
#define ACMR_PKT_ALLOCATE(ad_p, mblk_p)								\
	{	mblk_p = (ACMR_MBUF *)dev_alloc_skb(MGMT_DMA_BUFFER_SIZE);	\
		if (mblk_p != NULL) RTPKT_TO_OSPKT(mblk_p)->dev = get_netdev_from_bssid(ad_p, BSS0); }

#define ACMR_PKT_COPY(mblk_p, buf, len)	rom_memcpy(skb_put(RTPKT_TO_OSPKT(mblk_p), len), buf, len)
#define ACMR_PKT_FREE(ad_p, mblk_p)		RTMPFreeNdisPacket(ad_p, mblk_p)

	/*	1. get the acm type of a packet;
		2. get the user priority of a packet;
		3. set the acm type of a packet;
		4. set the user priority of a packet; */
#define ACMR_PKT_QOS_TYPE_GET(pkt_p)			\
	(((ACM_PACKET_INFO *)(RTMP_GET_PACKET_ACM_PARAM_INFO(pkt_p)))->qos_type)
#define ACMR_PKT_UP_GET(pkt_p)					\
	(((ACM_PACKET_INFO *)(RTMP_GET_PACKET_ACM_PARAM_INFO(pkt_p)))->up)

#define ACMR_PKT_QOS_TYPE_SET(pkt_p, type)		\
	(((ACM_PACKET_INFO *)(RTMP_GET_PACKET_ACM_PARAM_INFO(pkt_p)))->qos_type) = type;
#define ACMR_PKT_UP_SET(pkt_p, up)				\
	(((ACM_PACKET_INFO *)(RTMP_GET_PACKET_ACM_PARAM_INFO(pkt_p)))->up) = up;

	/*	1. get the encrypt mode of a device entry;
		2. the WEP type;
		3. the TKIP type;
		4. the AES type; */
#define ACMR_STA_ENCRYPT_MODE_GET(cdb_p)	(cdb_p->WepStatus)
#define ACMR_ENCRYPT_WEP					Ndis802_11WEPEnabled
#define ACMR_ENCRYPT_TKIP					Ndis802_11Encryption2Enabled
#define ACMR_ENCRYPT_AES					Ndis802_11Encryption3Enabled

	/*	1. the ACTION sub type;
		2. get the data pointer of a packet;
		3. get the data length of a packet;
		4. get the destination address (Addr1) of a packet;
		5. get the source address (Addr2) of a packet;
		6. get the type/length field of a packet; */
#define ACMR_SUBTYPE_ACTION					SUBTYPE_ACTION
#define ACMR_WLAN_PKT_GET(mbuf_p)			GET_OS_PKT_DATAPTR(mbuf_p)
#define ACMR_WLAN_LEN_GET(mbuf_p)			GET_OS_PKT_LEN(mbuf_p)

#define ACMR_WLAN_PKT_DA_GET(mbuf_p, addr)	\
	{	ACMR_WLAN_HEADER *hdr_p = (ACMR_WLAN_HEADER *)mbuf_p;	\
		rom_memcpy(addr, hdr_p->Addr1, 6);	}

#define ACMR_WLAN_PKT_SA_GET(mbuf_p, addr)	\
	{	ACMR_WLAN_HEADER *hdr_p = (ACMR_WLAN_HEADER *)mbuf_p;	\
		rom_memcpy(addr, hdr_p->Addr2, 6);	}

#define ACMR_WLAN_PKT_TYPE_GET(mbuf_p, type)	\
	{	ACMR_WLAN_HEADER *hdr_p = (ACMR_WLAN_HEADER *)mbuf_p;			\
		if (hdr_p->FC.Type & SUBTYPE_QDATA) type = *(UINT16 *)&hdr_p->Octet[2];	\
		else type = *(UINT16 *)&hdr_p->Octet[0]; }

	/*	1. get RTS threshold;
		2. get Fragment threshold;
		3. get maximum packet size; */
#define ACMR_RTS_THRESH(ad_p)				ad_p->CommonCfg.RtsThreshold
#define ACMR_FRG_THRESH(ad_p)				ad_p->CommonCfg.FragmentThreshold
#define ACMR_MAX_BUF_SIZE					MGMT_DMA_BUFFER_SIZE


/* MACRO: memory */

	/*	1. allocate a memory block;
		2. free a memory block;
		3. copy a memory block;
		4. zero a memory block;
		5. copy MAC address;
		6. set value to a memory block; */
#ifndef ACM_MEMORY_TEST
#define ACMR_MEM_ALLOC(size)				kmalloc(size, GFP_ATOMIC)
#define ACMR_MEM_FREE(p)					kfree(p)
#else
#define ACMR_MEM_ALLOC(size)				kmalloc(size, GFP_ATOMIC); \
											uiACM_MEM_Alloc_Num ++;
#define ACMR_MEM_FREE(p)					{ kfree(p); \
											uiACM_MEM_Free_Num ++; }
#endif // ACM_MEMORY_TEST //

#define ACMR_MEM_COPY(d, s, len)			rom_memcpy(d, s, len)
#define ACMR_MEM_ZERO(p, len)				rom_memset(p, 0, len)
#define ACMR_MEM_MAC_COPY(dst, src)			rom_memcpy(dst, src, 6)
#define ACMR_MEM_SET(d, c, len)				rom_memset(d, c, len)


/* MACRO: utility */

	/*	atoi & atoh */
#define ACMR_ARG_ATOI(argv_p)				simple_strtol(argv_p, 0, 10)
#define ACMR_ARG_ATOH(buf, h)				rom_AtoH(buf, h, 1)


/* MACRO: rate */

	/* phy mode */
#define ACMR_PHY_CCK						MODE_CCK
#define ACMR_PHY_OFDM						MODE_OFDM
#define ACMR_PHY_NONE						0xFF

#define ACMR_PHY_MODE_COMPARE(src, dst)		(src < dst)

	/* if same PHY Mode, we need to check if mcs < min_mcs;
		if cur phy = OFDM and min phy = CCK, mcs = 0 (6M) and mcs = 1 (9M) <
		min_mcs = 3 or 11 (11M) */
#define ACMR_MCS_COMPARE(phy, min_phy, mcs, min_mcs, flg_fail)		\
	{	if (phy == min_phy) { if (mcs < min_mcs) flg_fail = 1; }	\
		else { if ((phy == MODE_OFDM) && (min_phy == MODE_CCK))		\
		{ if ((mcs <= 1) && ((min_mcs == 3) || (min_mcs == 11)))	\
			flg_fail = 1; } } }

	/* rate 54 ~ 1M ID, reference to other modules */
#define ACMR_RATE_DRV_54M					RATE_54
#define ACMR_RATE_DRV_48M					RATE_48
#define ACMR_RATE_DRV_36M					RATE_36
#define ACMR_RATE_DRV_24M					RATE_24
#define ACMR_RATE_DRV_18M					RATE_18
#define ACMR_RATE_DRV_12M					RATE_12
#define ACMR_RATE_DRV_9M					RATE_9
#define ACMR_RATE_DRV_6M					RATE_6
#define ACMR_RATE_DRV_11M					RATE_11
#define ACMR_RATE_DRV_5_5M					RATE_5_5
#define ACMR_RATE_DRV_2M					RATE_2
#define ACMR_RATE_DRV_1M					RATE_1

#define ACM_RATE_MAX_NUM					12




/* ========================================================================== */
/* Structure */

/* ----- ACM Information ----- */
/* STA Entry ACM Information, put the entry to your client entry structure */
typedef struct _ACM_ENTRY_INFO {

	/* Important: If you add any extra parameters or change declaration type
		here, you need to change the WMM_STA_ACM_INFO_SIZE in rtmp.h */

	/*  for QAP, these TSPECs are down link TSPEC;
		for QSTA, these TSPECs are up link TSPEC */
	UINT8	*ac_out_sm [ACM_STA_TID_MAX_NUM]; /* output AC streams */

	/*  for QAP, these TSPECs are non-down link TSPEC;
		for QSTA, these TSPECs are non-up link TSPEC */
	UINT8	*ac_in_sm [ACM_STA_TID_MAX_NUM]; /* input AC streams */

} ACM_ENTRY_INFO;


/* Packet ACM Information, put the entry to your packet structure */
typedef struct _ACM_PACKET_INFO {

	UINT8	qos_type;	/* ACM_QOS_TYPE_LEGACY ~ ACM_QOS_TYPE_UAPSD */
	UINT8	up;			/* used for QoS Null frame */

} ACM_PACKET_INFO;


/* TCLASS element */
#define ACM_TCLAS_PROCESSING_MAX			1
#define ACM_TCLAS_MAX_NUM					5

typedef struct _ACM_TCLAS {

    UINT8  user_priority;			/* user priority */

#define ACM_TCLAS_TYPE_ETHERNET          	0
#define ACM_TCLAS_TYPE_IP_V4             	1
#define ACM_TCLAS_TYPE_8021DQ            	2
#define ACM_TCLAS_TYPE_MAX               	ACM_TCLAS_TYPE_8021DQ

#define ACM_TCLAS_TYPE_ETHERNET_LEN 		17
#define ACM_TCLAS_TYPE_IP_V4_LEN			18
#define ACM_TCLAS_TYPE_8021DQ_LEN		    5

#define ACM_TCLAS_TYPE_WME_ETHERNET_LEN	    (17+6) /* 6 is OUI header length */
#define ACM_TCLAS_TYPE_WME_IP_V4_LEN		(18+6) /* 6 is OUI header length */
#define ACM_TCLAS_TYPE_WME_8021DQ_LEN		(5+6)  /* 6 is OUI header length */
    UINT8  classifier_type;

    UINT8  classifier_mask;		    /* maximum 8 fields mapping */

    union
    {
        struct
        {
            UINT8   src_addr[6];	/* source MAC address */
            UINT8   dst_addr[6];	/* destination MAC address */
            UINT16  type;			/* type/length */
            UINT8   reserved[28];
        } __attribute__ ((packed)) ethernet;

        struct
        {
            UINT8   version;
            UINT32  source_ip;
            UINT32  dest_ip;
            UINT16  source_port;
            UINT16  dest_port;
            UINT8   dscp;
            UINT8   protocol;
            UINT8   reserved[27];
        } __attribute__ ((packed)) ipv4;

        struct
        {
            UINT16  tag_type;		/* VLAN tag (2B) */
            UINT8   reserved[40];
        } __attribute__ ((packed)) ieee8021q;
    } clasifier;
} __attribute__ ((packed)) ACM_TCLAS;


/* ----- TSPEC ----- */
/* provided to other modules */

/*     *head_p	|											 | *tail_p	*/
/*			    |											 |		    */
/*		 TSPEC1 <--> TSPEC2 <--> TSPEC3 <--> ... <--> TSPECn		    */
/*																        */
/* 1. TSPEC is dnlink or bidirection link for QAP.						*/
/* 2. TSPEC is uplink or direct link for QSTA.							*/
/* 3. TSPEC request is sent by QSTA.									*/
/* 4. QAP will handle the request only when QoS is enabled,				*/
/*    is_TSPEC_allowed == 1 and ACM bit == 1 for EDCA.					*/

/* TS Info field */
typedef struct _ACM_TS_INFO {

	/*	1 for a periodic traffic pattern (e.g. isochronous traffic stream of
		MSDUs, with constant or variable sizes, that are originated at fixed
		rate), or is set to 0 for an aperiodic, or unspecified traffic pattern
		(e.g. asynchronous traffic stream of low-duty cycles. */
#define ACM_TRAFFIC_TYPE_PERIODIC		1
#define ACM_TRAFFIC_TYPE_APERIODIC	0
	UINT8  traffic_type;

	/* 0 ~ 7, UP for prioritized QoS (TC) */
	/*	The same TSID may be used for multiple traffic streams at different
		non-AP QSTAs.  A non-AP QSTA may use the TSID value for a downlink
		TSPEC and either an uplink or a direct link TSPEC at the same time.
		A non-AP QSTA shall not use the same TSID for both uplink and direct
		link TS. */
	UINT8  TSID;

	/* 00: uplink; 01: downlink; 10: direct link; 11: bidirectional link */
#define ACM_DIRECTION_MAX				4
#define ACM_DIRECTION_UP_LINK			0x00
#define ACM_DIRECTION_DOWN_LINK		    0x01
#define ACM_DIRECTION_DIRECT_LINK		0x02
#define ACM_DIRECTION_BIDIREC_LINK	    0x03
    UINT8  direction;

	/* 00: reserved; 01: EDCA; 10: HCCA; 11: EDCA+HCCA */
#define ACM_ACCESS_POLICY_EDCA		    0x01
#define ACM_ACCESS_POLICY_HCCA		    0x02
#define ACM_ACCESS_POLICY_MIX			0x03
	UINT8  access_policy;

	/*	valid only when access method is HCCA or when the access method is EDCA
		and the schedule subfiled is set to 1.  It is set to 1 by a non-AP QSTA
		to indicate that an aggregate schedule is required. */
#define ACM_AGGREGATION_ENABLE		    1
#define ACM_AGGREGATION_DISABLE		    0
    UINT8  aggregation;

	/*	1 indicate that APSD is to be used for the traffic associated with the
		TSPEC. */
#define ACM_APSD_ENABLE				    1
#define ACM_APSD_DISABLE				0
	UINT8  APSD;

	/*	indicates the actual value of the UP to be used for the transport of
		MSDUs belonging to this traffic stream in cases where relative
		prioritization is required.  When the TCLAS element is present in the
		request, the user priority subfield in TSInfo field of the TSPEC
		element is reserved. */
#define ACM_UP_UNKNOWN				    0xFF
	UINT8  UP;

	/* 00: normal ACK; 01: No ACK; 10: reserved; 11: Block ACK */
#define ACM_ACK_POLICY_NORMAL			0x00
#define ACM_ACK_POLICY_NO				0x01
#define ACM_ACK_POLICY_BLOCK			0x03
	UINT8  ack_policy;

	/*	APSD vs. schedule ==> 00: no schedule; 01: unscheduled APSD;
		10: reserved; 11: scheduled APSD */
	/*	When the access policy is set to any value other than EDCA, the schedule
		subfiled is reserved. */
#define ACM_SCHEDULE_NO				    0x00
#define ACM_SCHEDULE_UN_APSD			0x01
#define ACM_SCHEDULE_APSD				0x03
	UINT8  schedule;

} PACKED ACM_TS_INFO;

/* Unspecified parameter sin these fields as indicated by a zero value indicate
   dont care. */
typedef struct _ACM_TSPEC {

	ACM_TS_INFO  ts_info; /* 9B */

	/*	specify the nominal size, if bit15 == 1 means the size, in octets, of
		the MSDU is fixed and is indicated by bit0 ~ bit14; If bit15 == 0 means
		the size of the MSDU might not be fixed and the size indicates the
		nominal MSDU size */
#define ACM_NOM_MSDU_SIZE_CHECK_BIT		    ((UINT16)0x8000)
	UINT16  nominal_msdu_size;

	/*	specify the maximum size, in octets, of MSDUs belonging to the TS under
		this traffic specification */
	UINT16  max_msdu_size;

	/*	specify the minimum interval, in units of microseconds, between the
		start of two successive SPs. */
#define ACM_TSPEC_MIN_SERV_INT_LIMIT		((UINT32)10000)
	UINT32  min_serv_int;

	/*	specify the maximum interval, in units of microseconds, between the
		start of two successive SPs. */
	UINT32  max_serv_int;

	/*	specify the maximum amount of time in units of microseconds that may
		elapse without arrival or transfer of an MSDU belonging to the TS
		before this TS is deleted by the MAC entity at the HC. */
#define ACM_TSPEC_INACTIVITY_DISABLE		((UINT32)0)
#define ACM_TSPEC_INACTIVITY_MIN			((UINT32)1000000)
	UINT32  inactivity_int;

	/*	specify the maximum amount of time in units of microseconds that may
		elapse without arrival or transfer of an MSDU belonging to the TS before
		the generation of successive QoS (+)CF-Poll is stopped for this TS. */
	/*	A value of 4294967295 (=2^32 - 1) disables the suspension interval,
		indicating that polling for ths TS is not to be interrupted based on
		inactivity. */
#define ACM_TSPEC_SUSPENSION_DISABLE		((UINT32)0xFFFFFFFF)
#define ACM_TSPEC_SUSPENSION_MIN			((UINT32)500000)
	UINT32  suspension_int;

	/*	indicates the time, expressed in microseconds, when the SP starts.
		If APSD subfield is set to 0, this field is also set to 0
		(unspecified). */
	UINT32  service_start_time;

	/*	specify the lowest data rate specified at the MAC_SAP, in units of
		bits per second */
	UINT32  min_data_rate;

	/*	specify the average data rate specified at the MAC_SAP, in units of
		bits per second */
	UINT32  mean_data_rate;

	/*	specify the maximum data rate specified at the MAC_SAP, in units of
		bits per second */
	UINT32  peak_data_rate;

	/*	specify the maximum burst, in units of octets, of the MSDUs belonging
		to this TS that arrive at the MAC SAP at the peak data rate. */
	UINT32  max_burst_size;

	/*	specify the maximum amount of time, in units of microseconds, allowed
		to transport an MSDU belonging to the TS in this TSPEC, measured between
		the time marking the arrival of the MSDU at the local MAC sublayer from
		the local MAC SAP and the time of completion of the successful
		transmission or retransmission of the MSDU to the destination. */
	UINT32  delay_bound;

	/*	specify the desired minimum PHY rate to use for this TS, in units of
		bits per second (bps). */
	UINT32  min_phy_rate;

	/*	specify the excess allocation of time (and bandwidth) over and above the
		stated application rates required to transport an MSDU belonging to the
		TS in this TSPEC. */
	/*	The thirteen least significant bits indicate the decimal part while the
		three most significant bits indicate the integer part of the number. */
	/*	The field takes into account retransmissions, as the rate information
		does not include retransmissions. */
#define ACM_SURPLUS_INT_BIT_NUM			3
#define ACM_SURPLUS_DEC_BIT_NUM			13
#define ACM_SURPLUS_DEC_BITMAP			0x1FFF
#define ACM_SURPLUS_DEC_VALID_NUM		2
#define ACM_SURPLUS_DEC_BASE			100
	UINT16  surplus_bandwidth_allowance;

	/*	contains the amount of time admitted to access the medium, in units of
		32 microsecond periods per second. */
	UINT32  medium_time;

} PACKED ACM_TSPEC;

/* Stream information */
typedef struct _ACM_STREAM_INFO {

	ACM_TSPEC	tspec;
	ACM_TCLAS	tclas[ACM_TCLAS_MAX_NUM];

#define ACM_TCLAS_PROCESSING_NOT_EXIST	0xFF
	UINT8		tclas_processing;

#define ACM_STREAM_TYPE_11E				0x00	/* 11e D8.0 */
#define ACM_STREAM_TYPE_WIFI			0x01	/* WME/WSM */
	UINT8		stream_type;
	UINT8		up;			/* user priority */
	UINT8		reserved1;

	UINT8		sta_mac[6];	/* QSTA MAC */

#define TSPEC_STATUS_REQUEST			0x0000	/* requesting */
#define TSPEC_STATUS_ACTIVE				0x0001	/* active */
#define TSPEC_STATUS_ACTIVE_SUSPENSION	0x0002	/* active but suspended */
#define TSPEC_STATUS_REQ_DELETING		0x0003	/* deleting the request */
#define TSPEC_STATUS_ACT_DELETING		0x0004	/* deleting the active */
#define TSPEC_STATUS_RENEGOTIATING		0x0005	/* renegotiating */

#define TSPEC_STATUS_HANDLING			0x0006	/* handle request */

#define TSPEC_STATUS_FAIL				0x0007	/* active or request fail */
	UINT16  status;			/* current status */

#define TSPEC_CAUSE_UNKNOWN				0x0000	/* unknown cause */
#define TSPEC_CAUSE_REQ_TIMEOUT			0x0001	/* fail due to request timeout */
#define TSPEC_CAUSE_SUGGESTED_TSPEC		0x0002	/* fail due to suggested TSPEC */
#define TSPEC_CAUSE_REJECTED			0x0003	/* rejected by QAP */
#define TSPEC_CAUSE_UNKNOWN_STATUS		0x0004	/* unknown rsp status code */
#define TSPEC_CAUSE_INACTIVITY_TIMEOUT	0x0005	/* inactivity timeout */
#define TSPEC_CAUSE_DELETED_BY_QAP		0x0006	/* deleted by QAP */
#define TSPEC_CAUSE_DELETED_BY_QSTA		0x0007	/* deleted by QSTA */
#define TSPEC_CAUSE_BANDWIDTH			0x0008	/* lower priority deletion */
#define TSPEC_CAUSE_REJ_MANY_TS			0x0009	/* only one TS for a AC */
#define TSPEC_CASUE_REJ_INVALID_PARAM	0x000a	/* invaild parameteres */
	UINT16  cause;			/* cause that makes the stream to be stopped */
							/* valid only when status == TSPEC_STATUS_FAIL */

#define TSPEC_PHY_TSID_DISABLE			0xFF
	UINT8  phy_tsid;		/* physical TS ID for EDCA or HCCA */
	UINT8  phy_tsid2;		/* used when dir = bidirectional, it will be copied
                               to phy_tsid of the duplicate TSPEC */

	UINT8  flg_is_main_link;/* 1: in active table; 0: in client data base */
	UINT8  reserved2[3];

	UINT32  inactivity_cur, suspension_cur;	/* variable, miscroseconds */

} ACM_STREAM_INFO;

/* Bandwidth information */
typedef struct _ACM_BANDWIDTH_INFO {

	UINT32 acm_used_time;	/* current total used time for ACM streams */

	UINT32 ac_used_time;	/* the allocated EDCA time */

	BOOLEAN flg_are_multiple_AP_checked;
	UINT32 acm_time_from_other_AP; /* time from other APs */

	UINT32 req_link_num;	/* requested link number */

	UINT32 ac_link_num_up;	/* the number of EDCA uplink streams */
	UINT32 ac_link_num_dn;	/* the number of EDCA dnlink streams */
	UINT32 ac_link_num_di;	/* the number of EDCA direct streams */
	UINT32 ac_link_num_bi;	/* the number of EDCA bidirectional streams */

	UINT16 chan_util;		/* channel utilization */
	UINT32 chan_busy_time;	/* channel busy time */

} ACM_BANDWIDTH_INFO;

/* ACM control parameters information */
typedef struct _ACM_CTRL_INFO {

	UINT8  flg_enable[ACM_DEV_NUM_OF_AC];	/* ACM is enabled or disabled */
	UINT8  downgrade_ac_num[ACM_DEV_NUM_OF_AC];	/* not-zero: downgrade AC ID */

	/* currently, CP % = BEK % */
	UINT32 min_CP_nu, min_CP_de;			/* Contention Period % */
	UINT32 min_BEK_nu, min_BEK_de;			/* BE/BK % */

	UINT32 acm_total_time;					/* total ACM used time */
	UINT32 acm_main_time[ACM_DEV_NUM_OF_AC];/* ACM main time for different AC */
	UINT32 acm_ac_time[ACM_DEV_NUM_OF_AC];	/* ACM used time for different AC */

	/* the number for uplink, dnlink, bidirectional link, or direct link */
	UINT32 link_num_up, link_num_dn, link_num_bi, link_num_di;

	/* dynamic ATL */
	UINT8  flg_datl; 						/* 1: enable DATL */
	UINT8  datl_bw_min[ACM_DEV_NUM_OF_AC];	/* unit: 1/100 */
	UINT8  datl_bw_max[ACM_DEV_NUM_OF_AC];	/* unit: 1/100 */

	UINT32	datl_bor_ac_bw[ACM_DEV_NUM_OF_AC][ACM_DEV_NUM_OF_AC];

} ACM_CTRL_INFO;


/* general public function (QAP) */
/*
========================================================================
Routine Description:
	Initialize the ACM Module.

Arguments:
	ad_p			- WLAN control block pointer
	flg_acm0			- the ACM flag for AC0
	flg_acm1			- the ACM flag for AC1
	flg_acm2			- the ACM flag for AC2
	flg_acm3			- the ACM flag for AC3
	flg_datl			- the Dynamic ATL flag

Return Value:
	ACM_RTN_OK		- init OK
	ACM_RTN_FAIL	- init fail

Note:
========================================================================
*/
ACM_FUNC_STATUS ACM_Init(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p,
	ACM_PARAM_IN	UINT8					flg_acm0,
	ACM_PARAM_IN	UINT8					flg_acm1,
	ACM_PARAM_IN	UINT8					flg_acm2,
	ACM_PARAM_IN	UINT8					flg_acm3,
	ACM_PARAM_IN	UINT8					flg_datl);

/*
========================================================================
Routine Description:
	Release the ACM Resource.

Arguments:
	ad_p			- WLAN control block pointer

Return Value:
	ACM_RTN_OK		- release OK
	ACM_RTN_FAIL	- release fail

Note:
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_Release(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p);

/*
========================================================================
Routine Description:
	ReInit ASIC ACM function.

Arguments:
	ad_p			- WLAN control block pointer

Return Value:
	ACM_RTN_OK		- init OK
	ACM_RTN_FAIL	- init fail

Note:
	Must be called after ACM_Init().
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_ReInit(
	ACM_PARAM_IN ACMR_PWLAN_STRUC		ad_p);

/*
========================================================================
Routine Description:
	Initialize the ACM ASIC setting.

Arguments:
	ad_p			- WLAN control block pointer

Return Value:
	None

Note:
	Must be called after ACM_Init() is called.
========================================================================
*/
ACM_EXTERN void ACM_ASIC_Init(
	ACM_PARAM_IN ACMR_PWLAN_STRUC		ad_p);

/*
========================================================================
Routine Description:
	Get the allocated ACM Time.

Arguments:
	ad_p			- WLAN control block pointer

Return Value:
	the allocated time (unit: 32us)

Note:
	Maximum allocated time is 1000000/32 = 31250 32-us.
========================================================================
*/
ACM_EXTERN UINT16 ACM_AllocatedTimeGet(
	ACM_PARAM_IN ACMR_PWLAN_STRUC	ad_p);

/*
========================================================================
Routine Description:
	Get bandwidth information.

Arguments:
	ad_p			- WLAN control block pointer
	*info_p			- the bandwidth information

Return Value:
	ACM_RTN_OK		- get ok
	ACM_RTN_FAIL	- get fail

Note:
	Used in QAP.
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_BandwidthGet(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_OUT	ACM_BANDWIDTH_INFO  *info_p);

/*
========================================================================
Routine Description:
	Get current EDCA ACM Information.

Arguments:
	ad_p			- WLAN control block pointer
	*info_p			- the EDCA ACM information

Return Value:
	ACM_RTN_OK		- get ok
	ACM_RTN_FAIL	- get fail

Note:
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_ControlInfomationGet(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_CTRL_INFO		*info_p);

/*
========================================================================
Routine Description:
	Handle something when a QoS data or null frame is received.

Arguments:
	ad_p			- WLAN control block pointer
	*cdb_p			- the source QSTA
	header_p			- the WLAN MAC header

Return Value:
	None

Note:
	1. Only for QAP.
	2. The frame shall be uplink.
	3. For EDCA, we shall reset activity timeout for QoS data frames.
	4. In LINUX, the function must be called in a tasklet.
========================================================================
*/
ACM_EXTERN void ACM_DataNullHandle(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB			*cdb_p,
	ACM_PARAM_IN	ACMR_WLAN_HEADER	*header_p);

/*
========================================================================
Routine Description:
	Enable or disable Dynamic ATL function.

Arguments:
	ad_p			- WLAN control block pointer
	flg_enable		- 1: enable; 0: disable
	*datl_bw_min_p	- new minimum bandwidth threshold
	*datl_bw_max_p	- new maximum bandwidth threshold

Return Value:
	None

Note:
	if you dont want to change bandwidth threshold, you can input NULL.
	datl_bw_min_p = NULL or datl_bw_max_p = NULL
========================================================================
*/
ACM_EXTERN void ACM_DATL_Ctrl(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				flg_enable,
	ACM_PARAM_IN	UINT8				*datl_bw_min_p,
	ACM_PARAM_IN	UINT8				*datl_bw_max_p);

/*
========================================================================
Routine Description:
	Inform us that tx compleletion interrupt occurred.

Arguments:
	ad_p			- WLAN control block pointer
	*mac_p			- the destination MAC
	*ts_info_p		- the TS Info of the packet
	is_err			- if the frame tx is error

Return Value:
	None

Note:
	1. Responsible for DELTS ACK frame check.
========================================================================
*/
ACM_EXTERN void ACM_DeltsFrameACK(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				*mac_p,
	ACM_PARAM_IN	UINT8				*ts_info_p,
	ACM_PARAM_IN	UINT8				is_err);

/*
========================================================================
Routine Description:
	Append the QBSS Load element to the beacon frame.

Arguments:
	ad_p			- WLAN control block pointer
	*pkt_p			- the beacon frame

Return Value:
	the element total length

Note:
========================================================================
*/
ACM_EXTERN UINT32 ACM_Element_QBSS_LoadAppend(
 	ACM_PARAM_IN		ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN_OUT	UINT8				*pkt_p);

/*
========================================================================
Routine Description:
	Reset current ACM Flag for each AC.

Arguments:
	ad_p			- WLAN control block pointer
	flg_acm0		- the ACM flag for AC0
	flg_acm1		- the ACM flag for AC1
	flg_acm2		- the ACM flag for AC2
	flg_acm3		- the ACM flag for AC3

Return Value:
	None

Note:
========================================================================
*/
ACM_EXTERN void ACM_FlagReset(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p,
	ACM_PARAM_IN	UINT8					flg_acm0,
	ACM_PARAM_IN	UINT8					flg_acm1,
	ACM_PARAM_IN	UINT8					flg_acm2,
	ACM_PARAM_IN	UINT8					flg_acm3);

/*
========================================================================
Routine Description:
	Return TRUE if the ACM of all AC are enabled.

Arguments:
	ad_p			- WLAN control block pointer

Return Value:
	ACM_RTN_OK		- the ACM of all AC is enabled
	ACM_RTN_FAIL	- the ACM of one AC is disabled

Note:
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_IsAllACEnabled(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p);

/*
========================================================================
Routine Description:
	Return TRUE if the ACM of any AC is enabled.

Arguments:
	ad_p			- WLAN control block pointer

Return Value:
	ACM_RTN_OK		- the ACM of any AC is enabled
	ACM_RTN_FAIL	- the ACM of all AC is disabled

Note:
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_IsAnyACEnabled(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p);

/*
========================================================================
Routine Description:
	Handle the management action frame.

Arguments:
	ad_p			- WLAN control block pointer
	*cdb_p			- the source QSTA
	subtype			- the subtype of the frame
	mblk_p			- the received frame
	phy_rate		- the physical tx rate for the frame

Return Value:
	ACM_RTN_OK		- mblk_p is released or forwarded
	ACM_RTN_FAIL	- handle ok and mblk_p is not released

Note:
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_ManagementHandle(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB		    *cdb_p,
	ACM_PARAM_IN	UINT32				subtype,
	ACM_PARAM_IN	UINT8				*mblk_p,
	ACM_PARAM_IN	UINT32				pkt_len,
	ACM_PARAM_IN	UINT32				phy_rate);

/*
========================================================================
Routine Description:
	Classify the QoS frame to a AC queue.

Arguments:
	ad_p			- WLAN control block pointer
	*cdb_p			- the source QSTA
	mbuf_p			- the received frame
	flag_force_high_ac	- 1: force the packet to AC3

Return Value:
	Queue Type: AC0 ~ AC3
	not AC0 ~ AC3: can not transmit

Note:
	1.	For QSTA or QAP.  For QSTA, TS ID is from 0 ~ 7.  For QAP,
		TS ID is from 0 ~ 15.
	2.	For QSTA, the frame shall be uplink; for QAP, the frame
		shall be dnlink.
	3.	Maybe use AC0 (BE) to transmit the frame if no TSPEC is matched.
========================================================================
*/
ACM_EXTERN UINT32 ACM_MSDU_Classify(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB			*cdb_p,
	ACM_PARAM_IN	ACMR_MBUF			*mbuf_p,
	ACM_PARAM_IN	UINT8				flag_force_high_ac);

/*
========================================================================
Routine Description:
	Enable or disable multiple AP ACM function.

Arguments:
	ad_p			- WLAN control block pointer
	flg_enable		- 1: enable; 0: disable

Return Value:
	None

Note:
========================================================================
*/
ACM_EXTERN void ACM_Multiple_AP_ACM_Ctrl(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	BOOLEAN				flg_enable);

/*
========================================================================
Routine Description:
	Check if the current tx PHY Mode and MCS > minimum PHY Mode and MCS.

Arguments:
	ad_p			- WLAN control block pointer
	*mbuf_p			- the frame expected to transmit
	phy_mode		- the PHY Mode expected to use
	mcs				- the MCS expected to use

Return Value:
	ACM_RTN_OK		- current Mode & MCS is allowed
	ACM_RTN_FAIL	- current Mode & MCS is not allowed

Note:
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_PacketPhyModeMCSCheck(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_MBUF			*mbuf_p,
	ACM_PARAM_IN	UINT8				phy_mode,
	ACM_PARAM_IN	UINT8				mcs);

/*
========================================================================
Routine Description:
	Delete a QSTA due to deauthentication or deassociation, etc.

Arguments:
	ad_p			- WLAN control block pointer
	*cdb_p			- the QSTA

Return Value:
	None

Note:
	Used in QAP.
========================================================================
*/
ACM_EXTERN void ACM_QSTA_Delete(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB			*cdb_p);

/*
========================================================================
Routine Description:
	Clear failed stream information.

Arguments:
	ad_p			- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
ACM_EXTERN void ACM_StreamFailClear(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);

/*
========================================================================
Routine Description:
	Get some streams information.

Arguments:
	ad_p			- WLAN control block pointer
	category		-	ACM_STM_CATEGORY_REQ,
						ACM_STM_CATEGORY_ACT,
						ACM_STM_CATEGORY_CDB,
						ACM_STM_CATEGORY_ERR
	type			- ACM_ACCESS_POLICY_EDCA
	*num_p			- the number of streams, must > 0
	*sta_mac_p		- the QSTA MAC
	*stream_p		- the stream information

Return Value:
	ACM_RTN_OK		- get ok
	ACM_RTN_FAIL	- no more stream

Note:
	1. if stream_p->tspec_p == NULL, the function will not copy
		TSPEC information.
	2. if stream_p->tclas_p[i] == NULL, the function will not
		copy TCLAS information.
	3. If you want to get all stream information, you shall call
		ACM_StreamNumGet() first.
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_StreamsGet(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT32				category,
	ACM_PARAM_IN	UINT32				type,
	ACM_PARAM_IN	UINT32				*num_p,
	ACM_PARAM_IN	UINT8				*sta_mac_p,
	ACM_PARAM_OUT	ACM_STREAM_INFO		*stream_p);

/*
========================================================================
Routine Description:
	Get the number of streams.

Arguments:
	ad_p			- WLAN control block pointer
	category		-	ACM_STM_CATEGORY_REQ,
						ACM_STM_CATEGORY_ACT,
						ACM_STM_CATEGORY_CDB,
						ACM_STM_CATEGORY_ERR
	type			- ACM_ACCESS_POLICY_EDCA
	*sta_mac_p		- the QSTA MAC

Return Value:
	current number of streams

Note:
========================================================================
*/
ACM_EXTERN UINT32 ACM_StreamNumGet(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT32				category,
	ACM_PARAM_IN	UINT32				type,
	ACM_PARAM_IN	UINT8				*sta_mac_p);

/*
========================================================================
Routine Description:
	Delete all activated TSPECs.

Arguments:
	ad_p				- WLAN control block pointer

Return Value:
	None

Note:
	1. Send a DELTS to the QSTA or QAP.
	2. Insert the activated TSPEC to the requested list.
	3. The TSPEC will be moved to the failed list when DELTS ACK
		is received or retry count is reached.
========================================================================
*/
void ACM_TC_DeleteAll(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);

#ifdef CONFIG_STA_SUPPORT
/*
========================================================================
Routine Description:
	Send a renegotiated TSPEC request to the QAP.

Arguments:
	ad_p				- WLAN control block pointer
	*cdb_p				- our STATION entry
	*src_tspec_p		- the requested TSPEC pointer
	tclas_num			- the number of TCLASS, max 5
	*src_tclas_p		- the requested TCLASS array pointer
	tclas_processing	- 1: must match all TCLAS
	stream_type			- the stream type: WME stream

Return Value:
	ACM_RTN_OK				- request is accepted
	ACM_RTN_FAIL			- semaphore lock fail or others
	ACM_RTN_NULL_POINTER	- null pointer
	ACM_RTN_NOT_EXIST		- the old TSPEC does not exist
	ACM_RTN_INVALID_PARAM	- invalid input parameters
	ACM_RTN_SEM_GET_ERR		- get semaphore fail
	ACM_RTN_FATAL_ERR		- can not call the func in error mode
	ACM_RTN_NO_FREE_TS		- no free TS ID or same TSID & direction
	ACM_RTN_ALLOC_ERR		- TSPEC request structure allocation fail

Note:
	1. Only for non-IBSS Station Mode.
	2. DLP is not allowed.
========================================================================
*/
STATIC ACM_FUNC_STATUS ACM_TC_Renegotiate(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB			*cdb_p,
	ACM_PARAM_IN	ACM_TSPEC			*src_tspec_p,
	ACM_PARAM_IN	UINT32				tclas_num,
    ACM_PARAM_IN	ACM_TCLAS			*src_tclas_p,
	ACM_PARAM_IN	UINT8				tclas_processing,
	ACM_PARAM_IN	UINT8				stream_type);
#endif // CONFIG_STA_SUPPORT //

/* general public function (QSTA) */
#if defined(CONFIG_STA_SUPPORT_SIM) || defined(CONFIG_STA_SUPPORT) /* !!!!!!!!! */
/*
========================================================================
Routine Description:
	Send a TSPEC request to the QAP.

Arguments:
	ad_p				- WLAN control block pointer
	*cdb_p				- our STATION entry
	*src_tspec_p		- the requested TSPEC pointer
	tclas_num			- the number of TCLASS, max 5
	*src_tclas_p		- the requested TCLASS array pointer
	tclas_processing	- 1: must match all TCLAS
	stream_type			- the stream type: WME stream

Return Value:
	ACM_RTN_OK				- request is accepted
	ACM_RTN_FAIL			- semaphore lock fail or others
	ACM_RTN_NULL_POINTER	- null pointer
	ACM_RTN_INVALID_PARAM	- invalid input parameters
	ACM_RTN_SEM_GET_ERR		- get semaphore fail
	ACM_RTN_FATAL_ERR		- can not call the func in error mode
	ACM_RTN_NO_FREE_TS		- no free TS ID or same TSID & direction
	ACM_RTN_ALLOC_ERR		- TSPEC request structure allocation fail

Note:
	1. Only for non-IBSS Station Mode.
	2. src_tclas_p is limited by ACM_TCLAS_MAX_NUM.
	3. DLP TSPEC is not allowed but DLP is allowed.
	4. *src_tspec_p & *src_tclas_p[ ] can not be freed in calling function.
	5. For WMM STA, the used TSPEC is the same.
========================================================================
*/
ACM_FUNC_STATUS ACM_WME_TC_Request(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB		    *cdb_p,
	ACM_PARAM_IN	ACM_TSPEC			*src_tspec_p,
	ACM_PARAM_IN	UINT32				tclas_num,
	ACM_PARAM_IN	ACM_TCLAS			*src_tclas_p,
	ACM_PARAM_IN	UINT8				tclas_processing,
	ACM_PARAM_IN	UINT8				stream_type);
#endif /* CONFIG_STA_SUPPORT_SIM || CONFIG_STA_SUPPORT */


/* EDCA public function (QAP) */
/*
========================================================================
Routine Description:
	Change current EDCA Information.

Arguments:
	ad_p			- WLAN control block pointer
	CPnu			- the numerator of Contention Period,
						if 0, no any update for CPnu
	CPde			- the denominator of Contention Period
						if 0, no any update for CPde
	BEnu			- the numerator of Best Effort percentage,
						if 0, no any update for BEnu
	BEde			- the denominator of Best Effort percentage
						if 0, no any update for BEde

Return Value:
	ACM_RTN_OK		- change ok
	ACM_RTN_FAIL	- change fail

Note:
	1. CPnu/CPde is the percentage of EDCA in Service Interval,
		only valid when HCCA is enabled.
	2. BEnu/BEde is the percentage of Best Effort streams in 1 second.
	3. The function will not delete any stream if residunt
		bandwidth is not enough for (CPnu/CPde)*SI or (BEnu/BEde).
	4. New (CPnu/CPde) or (BEnu/BEde) will be valid after bandwidth is enough.
	5. If the old ACM is enabled and the new ACM is disabled,
		the function will not deleted these streams use the AC.
	6. If flg_acm = 0xFF, it means old ACM flag is kept.
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_EDCA_InfomationChange(
	ACM_PARAM_IN ACMR_PWLAN_STRUC		ad_p,
	ACM_PARAM_IN UINT16   				CPnu,
	ACM_PARAM_IN UINT16   				CPde,
	ACM_PARAM_IN UINT16   				BEnu,
	ACM_PARAM_IN UINT16   				BEde);

/*
========================================================================
Routine Description:
	Check whether the element is WME Information.

Arguments:
	ad_p				- WLAN control block pointer
	*elm_p				- the element
	subtype				- the sub type

Return Value:
	ACM_RTN_OK			- check ok
	ACM_RTN_FAIL		- check fail

Note:
========================================================================
*/
ACM_EXTERN ACM_FUNC_STATUS ACM_WME_ELM_Check(
	ACM_PARAM_IN	UINT8				*elm_p,
	ACM_PARAM_IN	UINT8				subtype);

/*
========================================================================
Routine Description:
	Test command.

Arguments:
	ad_p				- WLAN control block pointer
	argv_p				- input parameters

Return Value:
	TRUE				- check ok
	FALSE				- check fail

Note:
========================================================================
*/
ACM_EXTERN INT ACM_Ioctl(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC 	ad_p,
	ACM_PARAM_IN	PUCHAR				argv_p);

#endif // __ACM_EXTR_H__ //

/* End of acm_extr.h */

