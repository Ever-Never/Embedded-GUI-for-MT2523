#ifndef	__PUBLIC_ROM_API__
#define	__PUBLIC_ROM_API__

#include "rom/wifi_fw_inband.h"
#include "wlan_def_iot_rom.h"
#include "hal_pwr_mgt_rom.h"
#include "iot_lib.h"
extern VOID (*ActHeaderInit)(
		IN	PRTMP_ADAPTER	pAd, 
		IN OUT PHEADER_802_11 pHdr80211, 
		IN PUCHAR Addr1, 
		IN PUCHAR Addr2,
		IN PUCHAR Addr3);
extern VOID (*ActionStateMachineInit)(
    IN	PRTMP_ADAPTER	pAd, 
    IN  STATE_MACHINE *S, 
    OUT STATE_MACHINE_FUNC Trans[]);
extern airplay_ie_t* (*airplay_ie_get)(void);
extern VOID (*Alloc_Frag_Frame)();
extern void (*announce_802_11_packet)(
	  IN  PRTMP_ADAPTER   pAd, 
	  IN  PNDIS_PACKET    pPacket);
extern void (*announce_802_3_packet)(
	  IN  PRTMP_ADAPTER   pAd, 
	  IN  PNDIS_PACKET    pPacket);
extern VOID (*Announce_or_Forward_802_3_Packet)(
	IN RTMP_ADAPTER 	*pAd,
	IN PNDIS_PACKET 	pPacket,
	IN UCHAR 			wdev_idx);
extern void (*Announce_Reordering_Packet)(IN PRTMP_ADAPTER pAd,
		IN struct reordering_mpdu *mpdu);
extern INT (*ApAllowToSendPacket)(
		IN RTMP_ADAPTER *pAd,
		IN struct wifi_dev *wdev,
		IN PNDIS_PACKET pPacket,
		IN UCHAR *pWcid);
extern VOID (*APAmpduFrameTx)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern VOID (*APAmsduFrameTx)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern VOID (*APAssocStateMachineInit)(
    IN RTMP_ADAPTER *pAd,
    IN STATE_MACHINE *S,
    OUT STATE_MACHINE_FUNC Trans[]);
extern void (*APAuthStateMachineInit)(
    IN PRTMP_ADAPTER pAd, 
    IN STATE_MACHINE *Sm, 
    OUT STATE_MACHINE_FUNC Trans[]);
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
extern PUCHAR (*APBuildAmsduFrameHeader)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern USHORT (*APBuildAssociation)(
    IN RTMP_ADAPTER *pAd,
    IN MAC_TABLE_ENTRY *pEntry,
    IN IE_LISTS *ie_list,
    IN UCHAR MaxSupportedRateIn500Kbps,
    OUT USHORT *pAid);
extern VOID (*APBuildCache802_11Header)(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK 		*pTxBlk,
	IN UCHAR 		*pHeader);
extern VOID (*APBuildCommon802_11Header)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern BOOLEAN (*ApCheckLongPreambleSTA)(RTMP_ADAPTER *pAd);
extern BOOLEAN (*APChkCls2Cls3Err)(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
extern VOID (*APCleanupPsQueue)(RTMP_ADAPTER *pAd, QUEUE_HEADER *pQueue);
extern VOID (*APCls2errAction)(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
extern VOID (*APCls3errAction)(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
extern VOID (*ap_cmm_peer_assoc_req_action)(
    IN RTMP_ADAPTER *pAd,
    IN MLME_QUEUE_ELEM *Elem,
	IN BOOLEAN isReassoc);
extern void (*ap_disconnect)(RTMP_ADAPTER *pAd);
extern VOID (*APFindCipherAlgorithm)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern VOID (*APFragmentFrameTx)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern INT (*ap_func_init)(RTMP_ADAPTER *pAd);
extern NDIS_STATUS (*APHardTransmit)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern INT (*ap_hw_tb_init)(RTMP_ADAPTER *pAd);
extern NDIS_STATUS (*APInitialize)(RTMP_ADAPTER *pAd);
extern VOID (*APLegacyFrameTx)(RTMP_ADAPTER *pAd, TX_BLK *pTxBlk);
extern PMAC_TABLE_ENTRY (*APMacTableInsertEntry)(
	IN RTMP_ADAPTER 	*pAd,
	IN UCHAR 			*pAddr,
	IN struct wifi_dev 	*wdev,
	IN UINT32 			ent_type,
	IN UCHAR 			OpMode,
	IN BOOLEAN 			CleanAll);
extern INT (*APMacTableResetEntry)(
	RTMP_ADAPTER 	*pAd, 
	MAC_TABLE_ENTRY *pEntry,
	BOOLEAN 		clean);
extern VOID (*APMlmeDeauthReqAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*APMlmeDisassocReqAction)(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
extern VOID (*ApMlmeInit)(RTMP_ADAPTER *pAd);
extern VOID (*APMlmeKickOutSta)(RTMP_ADAPTER *pAd, UCHAR *pStaAddr, UCHAR Wcid, USHORT Reason);
extern VOID (*APMlmePeriodicExec)(
    RTMP_ADAPTER *pAd);
extern INT (*ap_mlme_set_capability)(RTMP_ADAPTER *pAd, BSS_STRUCT *pMbss);
extern BOOLEAN (*APMsgTypeSubst)(
    IN PRTMP_ADAPTER pAd,
    IN PFRAME_802_11 pFrame,
    OUT INT *Machine,
    OUT INT *MsgType);
extern VOID (*APPeerAssocReqAction)(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
extern VOID (*APPeerAuthConfirmAction)(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*APPeerAuthReqAtIdleAction)(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
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
extern VOID (*APPeerBeaconAction)(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*APPeerDeauthReqAction)(
    IN PRTMP_ADAPTER pAd, 
    IN PMLME_QUEUE_ELEM Elem);
extern VOID (*APPeerDisassocReqAction)(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
extern VOID (*APPeerProbeReqAction)(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*APPeerReassocReqAction)(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem);
extern INT (*ap_phy_rrm_init)(RTMP_ADAPTER *pAd);
extern INT (*ap_phy_rrm_init_byRf)(RTMP_ADAPTER *pAd, UCHAR RfIC);
extern void (*AP_QueuePsActionPacket)(
		IN RTMP_ADAPTER 	*pAd,
		IN MAC_TABLE_ENTRY 	*pMacEntry,
		IN PNDIS_PACKET 	pPacket,
		IN BOOLEAN 			FlgIsDeltsFrame,
		IN BOOLEAN 			FlgIsLocked,
		IN UCHAR 			MgmtQid);
extern VOID (*APRxEAPOLFrameIndicate)(
	IN	PRTMP_ADAPTER	pAd,
	IN	MAC_TABLE_ENTRY	*pEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern INT (*APRxFowardHandle)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, PNDIS_PACKET pPacket);
extern INT (*APRxPktAllow)(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
extern INT (*APRxPsHandle)(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk);
extern INT (*ap_security_init)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, INT idx);
extern INT (*APSendPacket)(RTMP_ADAPTER *pAd, PNDIS_PACKET pPacket);
extern VOID (*APShutdown)(RTMP_ADAPTER *pAd);
extern VOID (*APStartUp)(RTMP_ADAPTER *pAd);
extern VOID (*APStartUpForMbss)(RTMP_ADAPTER *pAd,BSS_STRUCT *pMbss);
extern VOID (*APStop)(RTMP_ADAPTER *pAd);
extern VOID (*APSyncStateMachineInit)(
	IN RTMP_ADAPTER *pAd,
	IN STATE_MACHINE *Sm,
	OUT STATE_MACHINE_FUNC Trans[]);
extern VOID (*APUpdateAllBeaconFrame)(RTMP_ADAPTER *pAd);
extern VOID (*APUpdateBeaconFrame)(RTMP_ADAPTER *pAd, INT apidx);
extern VOID (*APUpdateCapabilityAndErpIe)(RTMP_ADAPTER *pAd);
extern VOID (*ApUpdateCapabilityAndErpleByRf)(RTMP_ADAPTER *pAd, UCHAR RfIC);
extern VOID (*APUpdateOperationMode)(RTMP_ADAPTER *pAd);
extern UINT	(*APValidateRSNIE)(
	IN PRTMP_ADAPTER    pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN PUCHAR			pRsnIe,
	IN UCHAR			rsnie_len);
extern __romtext UCHAR	(*ARCFOUR_BYTE)(
	IN	PARCFOURCONTEXT		Ctx);
extern __romtext VOID	(*ARCFOUR_ENCRYPT)(
	IN	PARCFOURCONTEXT	Ctx,
	IN	PUCHAR			pDest,
	IN	PUCHAR			pSrc,
	IN	UINT			Len);
extern __romtext VOID	(*ARCFOUR_INIT)(
	IN	PARCFOURCONTEXT	Ctx,
	IN	PUCHAR			pKey,
	IN	UINT			KeyLen);
extern VOID (*AsicAddRemoveKey)(
	IN RTMP_ADAPTER 	*pAd,
	IN UCHAR			isAdd,
	IN UCHAR			BssIdx,
	IN UCHAR			KeyIdx,
	IN UCHAR			Wcid,
	IN UCHAR 			KeyTabFlag,
	IN PCIPHER_KEY		pCipherKey,
	IN PUCHAR			PeerAddr);
extern VOID (*AsicAddSharedKeyEntry)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 BssIndex,
	IN UCHAR		 KeyIdx,
	IN PCIPHER_KEY   pCipherKey);
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
extern VOID (*AsicDelWcidTab)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR	Wcid);
extern INT32 (*AsicDevInfoUpdate)(
		RTMP_ADAPTER 		*pAd,
		UINT8 				OwnMacIdx,
		UINT8 				*OwnMacAddr,
		UINT8 				BandIdx,
		UINT8 				Active,
		UINT32 				EnableFeature);
extern VOID (*AsicDisableRDG)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*AsicDisableSync)(
	IN PRTMP_ADAPTER pAd,
	UCHAR HWBssidIdx);
extern VOID (*AsicEnableBssSync)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*AsicEnableRDG)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*AsicForceWakeup)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*asic_mcs_lut_update)(RTMP_ADAPTER *pAd, MAC_TABLE_ENTRY *pEntry);
extern INT_32 (*AsicPmStateCtrl)(RTMP_ADAPTER *pAd,UINT_8 WlanIdx, UINT_8 PmNumber, UINT_8 PmState);
extern VOID (*AsicRemovePairwiseKeyEntry)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 BssIdx,
	IN UCHAR		 Wcid);
extern VOID (*AsicRemoveSharedKeyEntry)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 BssIndex,
	IN UCHAR		 KeyIdx);
extern void (*AsicSendBcnFrame)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG FrameLen);
extern VOID (*AsicSetBssid)(
	IN PRTMP_ADAPTER pAd, 
	IN PUCHAR pBssid);
extern VOID (*AsicSetEdcaParm)(
	IN PRTMP_ADAPTER pAd,
	IN PEDCA_PARM	 pEdcaParm);
extern INT (*AsicSetRxFilter)(    IN RTMP_ADAPTER *pAd);
extern VOID (*AsicSetSlotTime)(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN bUseShortSlotTime);
extern VOID (*AsicSleepThenAutoWakeup)(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT TbttNumToNextWakeUp);
extern INT32 (*AsicStaRecUpdate)(
	RTMP_ADAPTER *pAd,
	UINT8 BssIndex,
	UINT8 WlanIdx,
	UINT32 ConnectionType,
	UINT8 ConnectionState,
	UINT32 EnableFeature);
extern void (*AsicStopBcnFrame)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG FrameLen);
extern VOID (*AsicSwitchChannel)(
	IN PRTMP_ADAPTER pAd, 
	IN	UCHAR			Channel,
	IN	BOOLEAN			bScan);
extern VOID (*AsicUpdateAutoFallBackTable)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			pRateTable);
extern VOID (*AsicUpdateBASession)(
	RTMP_ADAPTER 	*pAd, 
	UCHAR 			wcid, 
	UCHAR 			tid, 
	UINT16 			sn, 
	UCHAR 			basize, 
	BOOLEAN 		isAdd, 
	INT 				ses_type);
extern VOID (*AsicUpdateProtect)(
	IN		PRTMP_ADAPTER	pAd,
	IN 		USHORT			OperationMode,
	IN 		UCHAR			SetMask,
	IN		BOOLEAN			bDisableBGProtect,
	IN		BOOLEAN			bNonGFExist);
extern VOID (*AsicUpdateRxWCIDTable)(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		WCID,
	IN PUCHAR        pAddr);
extern VOID (*AsicUpdateWCIDIVEIV)(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		WCID,
	IN ULONG        uIV,
	IN ULONG        uEIV);
extern VOID (*AssocParmFill)(
	IN PRTMP_ADAPTER pAd, 
	IN OUT MLME_ASSOC_REQ_STRUCT *AssocReq, 
	IN PUCHAR                     pAddr, 
	IN USHORT                     CapabilityInfo, 
	IN ULONG                      Timeout, 
	IN USHORT                     ListenIntv);
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
extern VOID (*AssocStateMachineInit)(
	IN	PRTMP_ADAPTER	pAd, 
	IN  STATE_MACHINE *S, 
	OUT STATE_MACHINE_FUNC Trans[]);
extern VOID (*AssocTimeout)(IN PVOID SystemSpecific1, 
				 IN PVOID FunctionContext, 
				 IN PVOID SystemSpecific2, 
				 IN PVOID SystemSpecific3);
extern VOID (*AssocTimeoutAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern void (*rom_AtoH)(char * src, UCHAR * dest, int destlen);
extern VOID (*AuthRspStateMachineInit)(
    IN PRTMP_ADAPTER pAd, 
    IN PSTATE_MACHINE Sm, 
    IN STATE_MACHINE_FUNC Trans[]);
extern void (*AuthStateMachineInit)(
    IN PRTMP_ADAPTER pAd, 
    IN STATE_MACHINE *Sm, 
    OUT STATE_MACHINE_FUNC Trans[]);
extern VOID (*AuthTimeout)(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3);
extern VOID (*AuthTimeoutAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern struct reordering_mpdu * (*ba_dequeue)(struct reordering_list *list);
extern void (*ba_enqueue)(struct reordering_list *list, struct reordering_mpdu *mpdu_blk);
extern VOID (*ba_enqueue_reordering_packet)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PBA_REC_ENTRY	pBAEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern void (*ba_flush_reordering_timeout_mpdus)(
	 IN PRTMP_ADAPTER    pAd, 
	 IN PBA_REC_ENTRY    pBAEntry,
	 IN ULONG            Now32);
extern USHORT (*ba_indicate_reordering_mpdus_in_order)(
	   IN PRTMP_ADAPTER    pAd, 
	   IN PBA_REC_ENTRY    pBAEntry,
	   IN USHORT           StartSeq);
extern void (*ba_indicate_reordering_mpdus_le_seq)(
	   IN PRTMP_ADAPTER    pAd, 
	   IN PBA_REC_ENTRY    pBAEntry,
	   IN USHORT           Sequence);
extern struct reordering_mpdu * (*ba_mpdu_blk_alloc)(PRTMP_ADAPTER pAd);
extern void (*ba_mpdu_blk_free)(PRTMP_ADAPTER pAd, struct reordering_mpdu *mpdu_blk);
extern VOID (*BAOriSessionAdd)(
	IN PRTMP_ADAPTER    pAd, 
	IN MAC_TABLE_ENTRY  *pEntry,
	IN PFRAME_ADDBA_RSP pFrame);
extern VOID (*BAOriSessionSetUp)(
	  IN PRTMP_ADAPTER    pAd, 
	  IN MAC_TABLE_ENTRY  *pEntry,
	  IN UCHAR            TID,
	  IN USHORT           TimeOut,
	  IN ULONG            DelayTime,
	  IN BOOLEAN          isForced);
extern VOID (*BAOriSessionSetupTimeout)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*BAOriSessionTearDown)(
	IN OUT  PRTMP_ADAPTER   pAd, 
	IN      UCHAR           Wcid,
	IN      UCHAR           TID,
	IN      BOOLEAN         bPassive,
	IN      BOOLEAN         bForceSend);
extern BOOLEAN (*BARecSessionAdd)(
	IN PRTMP_ADAPTER    pAd, 
	IN MAC_TABLE_ENTRY  *pEntry,
	IN PFRAME_ADDBA_REQ pFrame);
extern VOID (*BARecSessionIdleTimeout)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*BARecSessionTearDown)(
	IN OUT  PRTMP_ADAPTER   pAd, 
	IN      UCHAR           Wcid,
	IN      UCHAR           TID,
	IN      BOOLEAN         bPassive);
extern void (*ba_refresh_reordering_mpdus)(
	   IN PRTMP_ADAPTER    pAd, 
	   PBA_REC_ENTRY       pBAEntry);
extern UINT (*BA_Reorder_AMSDU_Annnounce)(RTMP_ADAPTER *pAd,
	PNDIS_PACKET pPacket,
	UCHAR OpMode);
extern BOOLEAN (*ba_reordering_mpdu_insertsorted)(struct reordering_list *list, struct reordering_mpdu *mpdu);
extern struct reordering_mpdu  * (*ba_reordering_mpdu_probe)(struct reordering_list *list);
extern BOOLEAN (*ba_reordering_resource_init)(PRTMP_ADAPTER pAd, int num);
extern void (*ba_reordering_resource_release)(PRTMP_ADAPTER pAd);
extern VOID (*BarHeaderInit)(
		IN	PRTMP_ADAPTER	pAd, 
		IN OUT PFRAME_BAR pCntlBar, 
		IN PUCHAR pDA,
		IN PUCHAR pSA);
extern VOID (*BASessionTearDownALL)(
	IN OUT  PRTMP_ADAPTER pAd, 
	IN      UCHAR Wcid);
extern BA_ORI_ENTRY * (*BATableAllocOriEntry)(
  IN  PRTMP_ADAPTER   pAd,
  OUT USHORT          *Idx);
extern BA_REC_ENTRY * (*BATableAllocRecEntry)(
  IN  PRTMP_ADAPTER   pAd,
  OUT USHORT          *Idx);
extern VOID (*BATableFreeOriEntry)(
	IN  PRTMP_ADAPTER   pAd,
	IN  ULONG           Idx);
extern VOID (*BATableFreeRecEntry)(
	IN  PRTMP_ADAPTER   pAd,
	IN  ULONG           Idx);
extern VOID (*BATableInit)(
	IN PRTMP_ADAPTER pAd, 
    IN BA_TABLE *Tab);
extern INT (*bcn_buf_deinit)(RTMP_ADAPTER *pAd, BCN_BUF_STRUC *bcn_info);
extern INT (*bcn_buf_init)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev);
extern INT (*BcnTimUpdate)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, UCHAR *ptr);
extern VOID (*BeaconTimeout)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*BeaconTimeoutAtJoinAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*BeaconTransmitRequired)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev);
extern BOOLEAN (*BeaconTransmitRequired_from_jedi)(RTMP_ADAPTER *pAd, INT apidx, BSS_STRUCT *pMbss);
extern VOID (*bget_stats)(char * when);
extern int (*bridge_rx_handle)(struct net_pkt_blk *pkt);
extern VOID (*BssCipherParse_iotn9)(
	IN OUT	PBSS_ENTRY	pBss);
extern VOID
(*bssEnqBcnPktFromCmd) (
    IN UINT_8  ucOwnMacIdx,
    IN P_MSDU_INFO_T prMsduInfo
    );
extern ULONG (*BssSsidTableSearch)(
	IN BSS_TABLE *Tab, 
	IN PUCHAR	 pBssid,
	IN CHAR	     *pSsid,
	IN UCHAR	 SsidLen,
	IN UCHAR	 Channel);
extern VOID (*BssTableDeleteEntry)(
	IN OUT	BSS_TABLE *Tab, 
	IN		PUCHAR	  pBssid,
	IN		UCHAR	  Channel);
extern ULONG (*BssTableGetBSSID)(BSS_TABLE *Tab, CHAR *pSsid, UCHAR SsidLen, PUCHAR Bssid);
extern VOID (*BssTableInit)(
	IN BSS_TABLE *Tab);
extern ULONG (*BssTableSearch)(
	IN BSS_TABLE *Tab, 
	IN PUCHAR	 pBssid,
	IN UCHAR	 Channel);
extern VOID (*BssTableSortByRssi)(
	IN OUT BSS_TABLE *OutTab);
extern VOID (*BssTableSsidSort)(
	IN	PRTMP_ADAPTER	pAd, 
	OUT BSS_TABLE *OutTab, 
	IN	CHAR Ssid[], 
	IN	UCHAR SsidLen);
extern WLAN_STATUS (*bssUpdateBssInfoBasic) (	RTMP_ADAPTER *pAd,
	UCHAR OwnMacIdx,
	UINT8 BssIndex,
	UCHAR *Bssid,
	UINT8 BcMcWlanIdx,
	UINT32 NetworkType,
	UINT32 u4ConnectionType,
	UINT8 Active,
	UINT32 EnableFeature);
extern UCHAR (*rom_BtoH)(char ch);
extern void (*buf_blk_ref)(struct net_buf_blk *buf_blk);
extern VOID (*BuildChannelList)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*BuildEffectedChannelList)(
	IN PRTMP_ADAPTER pAd);
extern ULONG (*BuildIntolerantChannelRep)(
		IN	PRTMP_ADAPTER	pAd,
		IN	  PUCHAR  pDest);
extern UCHAR (*cfgmode_2_wmode)(UCHAR cfg_mode);
extern VOID (*ChangeToCellPowerLimit)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR         AironetCellPowerLimit);
extern UCHAR (*ChannelSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN UCHAR channel);
extern BOOLEAN (*CHAN_PropertyCheck)(RTMP_ADAPTER *pAd, UINT32 ChanNum);
extern PNDIS_PACKET (*ClonePacket)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PNDIS_PACKET    pPacket,    
	IN  PUCHAR          pData,
	IN  ULONG           DataSize);
extern VOID (*Cls2errAction)(
    IN PRTMP_ADAPTER pAd, 
    IN PUCHAR pAddr);
extern VOID (*cmm_assoc_reassoc_action)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem,
	IN BOOLEAN	isReassoc);
extern VOID (*cmm_invalid_state)(
		IN PRTMP_ADAPTER	pAd, 
		IN MLME_QUEUE_ELEM 	*Elem,
		IN ULONG 			MsgType);
extern VOID (*CmmRxnonRalinkFrameIndicate)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern VOID (*CntlChannelWidth)(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			PrimaryChannel,
	IN UCHAR			CentralChannel,	
	IN UCHAR			ChannelWidth,
	IN UCHAR			SecondaryChannelOffset);
extern BOOLEAN (*CntlEnqueueForRecv)(
	IN PRTMP_ADAPTER		pAd, 
	IN ULONG				Wcid, 
	IN ULONG				MsgLen, 
	IN PFRAME_BA_REQ		pMsg);
extern VOID (*CntlIdleProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlMlmeRoamingProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlOidRTBssidProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM * Elem);
extern VOID (*CntlOidScanProc)(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlOidSsidProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM * Elem);
extern VOID (*CntlWaitAssocProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitAuthProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitAuthProc2)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitDisassocProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitJoinProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitReassocProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*CntlWaitStartProc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*ComposeAPRsnIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern ULONG (*ComposeBcnPktHead)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, UCHAR *pBeaconFrame);
extern VOID (*ComposeBcnPktTail)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*ComposeNullFrame)(
		RTMP_ADAPTER *pAd,	
		PHEADER_802_11 pNullFrame,	
		UCHAR *pAddr1,	
		UCHAR *pAddr2,	
		UCHAR *pAddr3);
extern VOID (*ComposePsPoll)(
	IN	RTMP_ADAPTER *pAd,
	IN	PPSPOLL_FRAME pPsPollFrame,
	IN	USHORT	Aid,
	IN	UCHAR *pBssid,
	IN	UCHAR *pTa);
extern VOID (*ComposeRSNIE)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*ComposeStaRsnIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*ComposeWPSIE)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern int (*connsys_close_wifi)(IN struct netif *net_dev);
extern NDIS_STATUS (*connsys_init_wifi)(IN struct netif *netif);
extern error_t (*connsys_netif_setup)(struct netif *netif);
extern int (*connsys_open_wifi)(struct netif *netif);
extern void (*connsys_rx_handle)(struct netif *netif, int queue);
extern error_t (*connsys_send_packets)(
	IN struct netif 	*net_dev, 
	IN PNDIS_PACKET 	pPacket);
extern struct pbuf_boson *(*convert_from_net_pkt_to_pfuf)(struct net_pkt_blk *pkt);
extern struct net_pkt_blk *(*convert_from_pbuf_to_net_pkt)(struct pbuf_boson *new_pbuf_pkt);
extern BOOLEAN (*convert_reordering_packet_to_preAMSDU_or_802_3_packet)(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RX_BLK			*pRxBlk,
	IN  UCHAR			FromWhichBSSID);
extern CHAR (*ConvertToRssi)(
	IN PRTMP_ADAPTER pAd,
	IN CHAR Rssi,
	IN  UCHAR   RssiNumber);
extern struct net_pkt_blk *(*CreateInBandFrame)(IN INT len);
extern UINT (*deaggregate_AMSDU_announce)(
	IN RTMP_ADAPTER *pAd,
	PNDIS_PACKET pPacket,
	IN UCHAR *pData,
	IN ULONG DataSize,
	IN UCHAR OpMode);
extern INT (*deq_mgmt_frame)(
		RTMP_ADAPTER 	*pAd,
		PNDIS_PACKET 	pkt,
		UCHAR 			qIdx,
		BOOLEAN 		bLocked);
extern INT (*deq_packet_gatter)(
		RTMP_ADAPTER 		*pAd,
		struct dequeue_info *deq_info,
		TX_BLK 				*pTxBlk,
		BOOLEAN 			in_hwIRQ);
extern HAL_STATUS
(*__direct_access_halConfigBCNQueueBehavior) (
    IN ENUM_HW_OP_MODE_T eBcnQueueOperationMode,
    IN UINT_8 ucBssidIdx
    );
extern HAL_STATUS
(*__direct_access_halEnableExtBssTsfFunc)(
    IN UINT_8 ucEnable
    );
extern HAL_STATUS
(*__direct_access_halMacSetExtBss) (
    IN UINT_8 ucExtBssIdx,
    IN UINT_8 ucEnable,
    IN PUINT_8 pucBSSID
    );
extern VOID
(*__direct_access_halPmConfigTbttBasedTimer) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN UINT_16              u2BeaconPeriod, // unit of TU
    IN UINT_8               ucDtimPeriod,
    IN UINT_8               ucTbttWakePeriod,
    IN UINT_8               ucDtimWakePeriod,
    IN BOOLEAN              fgEnableTrigger
    );
extern VOID
(*__direct_access_halPmEnablePreTbttTimeupEvent) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );
extern VOID
(*__direct_access_halPmEnableTbttTimer) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable,
    IN BOOLEAN              fgReCalNextTbttTime
    );
extern VOID
(*__direct_access_halPmEnableTbttTimeupEvent) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );
extern VOID
(*__direct_access_halPmSetPreTbttInterval) (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN UINT_16  u2PreTbttIntvUs
    );
extern VOID
(*__direct_access_halPmSetTsfHardwareUpdateMode) (
    IN ENUM_LOCAL_TSF_T    eTsfId,
    IN HAL_TSF_HW_UPDATE_MODE eMode
    );
extern HAL_STATUS
(*__direct_access_halSetBSSID)(
    IN UINT_8       ucBssidIdx,
    IN BOOL         fgEnable,
    IN PUINT_8      pucBSSID
    );
extern HAL_STATUS
(*__direct_access_halSetExtBssTsfOffset)(
    IN UINT_8 ucExtBssIdx,
    IN UINT_32 u4TsfOffset
    );
extern HAL_STATUS
(*__direct_access_halSetOwnMACAddr)(
    IN UINT_8       ucIdx,
    IN BOOL         fgEnable,
    IN PUINT_8      pucMACAddr
    );
extern HAL_STATUS
(*__direct_access_halWtbl2ConfigAmpduCap)(
    IN UINT_8                ucEntryIdx,
    IN P_HAL_WTBL_PEER_CAP_T prWtblPeerCap
    );
extern HAL_STATUS
(*__direct_access_halWtbl2ConfigBaEntry)(
    IN UINT_8  ucWtblIdx,
    IN BOOLEAN fgEnable,
    IN UINT_8  ucTID,
    IN UINT_8  ucWinSizeEntryIdx
    );
extern HAL_STATUS
(*__direct_access_halWtbl2ConfigPeerCap)(
    IN UINT_8                      ucEntryIdx,
    IN P_HAL_WTBL_PEER_CAP_T       prWtblPeerCap,
    IN P_HAL_WTBL_PEER_RATE_INFO_T prWtblRateInfo
    );
extern HAL_STATUS
(*__direct_access_halWtbl2GetPeerCap)(
    IN UINT_8                      ucEntryIdx,
    IN P_HAL_WTBL_PEER_CAP_T       prWtblPeerCap,
    IN P_HAL_WTBL_PEER_RATE_INFO_T prWtblRateInfo
    );
extern HAL_STATUS
(*__direct_access_halWtbl2SetSN)(
    IN UINT_8                      ucEntryIdx,
    IN ENUM_WTBL2_SN_SOURCE_TYPE_T eSnSource,
    IN UINT_16                     u2SN
    );
extern HAL_STATUS
(*__direct_access_halWtblConfigTbl1EntryCap)(
    IN UINT_8                  ucEntryIdx,
    IN P_HAL_WTBL_TX_CONFIG_T  prWtblTxConfig,
    IN P_HAL_WTBL_FID_EID_T    prWtblFidEid,
    IN P_HAL_WTBL_SEC_CONFIG_T prWtblSecConfig
    );
extern HAL_STATUS
(*__direct_access_halWtblGetTbl1EntryCap)(
    IN UINT_8                   ucEntryIdx,
    OUT P_HAL_WTBL_TX_CONFIG_T  prWtblTxConfig,
    OUT P_HAL_WTBL_FID_EID_T    prWtblFidEid,
    OUT P_HAL_WTBL_SEC_CONFIG_T prWtblSecConfig
    );
extern VOID (*__direct_access_MtAsicUpdateProtect)(
	IN PRTMP_ADAPTER pAd,
	IN IOT_PROTECT_CTRL_T *Protect);
extern VOID (*__direct_access_MtAsicUpdateRTSThreshold)(PRTMP_ADAPTER pAd, MT_RTSCTS_CTRL_T RtsCtsCtrl);
extern WLAN_STATUS (*__direct_access_OffloadSlotTimeSet)(
    P_SLOT_TIME_SET_T prSlotTimeSet
    );
extern VOID (*__direct_access_Wtbl2RateTableUpdate)(RTMP_ADAPTER *pAd, UCHAR ucWcid, UINT32 u4Wtbl2D9, UINT32 *Rate);
extern VOID (*DisassocParmFill)(
	IN PRTMP_ADAPTER pAd, 
	IN OUT MLME_DISASSOC_REQ_STRUCT *DisassocReq, 
	IN PUCHAR pAddr, 
	IN USHORT Reason);
extern VOID (*DisassocTimeout)(IN PVOID SystemSpecific1, 
					IN PVOID FunctionContext, 
					IN PVOID SystemSpecific2, 
					IN PVOID SystemSpecific3);
extern VOID (*DisassocTimeoutAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern UCHAR (*dot11_2_ra_rate)(UCHAR MaxSupportedRateIn500Kbps);
extern VOID (*Drop)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern PNDIS_PACKET (*duplicate_pkt_with_TKIP_MIC)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket);
extern VOID (*EnqueuePsPoll)(
	IN PRTMP_ADAPTER pAd);
extern UCHAR (*FirstChannel)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*Free_Frag_Frame)();
extern void (*fw_os_ms_delay)(unsigned int ui4_delay);
extern UCHAR (*get_apidx_by_addr)(RTMP_ADAPTER *pAd, UCHAR *addr);
extern MAC_TABLE_ENTRY *(*GetAssociatedAPByWdev)(
	RTMP_ADAPTER 		*pAd,
	struct wifi_dev   	*pwdev);
extern INT32 (*GetBssIdx)(RTMP_ADAPTER *pAd);
extern UCHAR (*get_cent_ch_by_htinfo_iotn9)(	RTMP_ADAPTER *pAd,	ADD_HT_INFO_IE *ht_op,	HT_CAPABILITY_IE *ht_cap);
extern UINT8 (*GetCuntryMaxTxPwr)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR PhyMode,
	IN UINT8 channel);
extern UINT8 (*GetCuntryMaxTxPwr_from_jedi)(
	IN PRTMP_ADAPTER pAd,
	IN UINT8 channel);
extern INT (*get_ht_max_mcs)(
	RTMP_ADAPTER *pAd,
	UCHAR *desire_mcs,
	UCHAR *cap_mcs);
extern PNET_DEV (*get_netdev_from_bssid)(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR           FromWhichBSSID);
extern UCHAR (*get_nss_by_mcs)(
		UCHAR 	phy_mode,
		UCHAR 	mcs,
		BOOLEAN stbc);
extern UINT_32 (*get_rx_vector)(UINT_8 ucGroupVLD);
extern void (*get_target_port_bssid)(UINT_8 **target_bssid, UINT_8 target_port,RTMP_ADAPTER *pAd);
extern UINT_8* (*get_target_port_bw)(UINT_8 target_port,RTMP_ADAPTER *pAd);
extern UINT_8* (*get_target_port_ext_cha)(UINT_8 target_port,RTMP_ADAPTER *pAd);
extern BOOLEAN (*GetTimBit)(
    IN UCHAR *Ptr, 
    IN USHORT Aid, 
    OUT UCHAR *TimLen, 
    OUT UCHAR *BcastFlag, 
    OUT UCHAR *DtimCount, 
    OUT UCHAR *DtimPeriod,
    OUT UCHAR *MessageToMe);
extern struct wifi_dev *(*get_wdev_by_idx)(
		RTMP_ADAPTER 	*pAd, 
		INT 			idx);
extern HAL_STATUS
(*halConfigBCNQueueBehavior) (
    IN ENUM_HW_OP_MODE_T eBcnQueueOperationMode,
    IN UINT_8 ucBssidIdx
    );
extern VOID (*halConfigRifsEnable) (BOOLEAN fgIsRifsMode, IN BOOLEAN fgIsTxRifsEnable);
extern UINT_32 (*halMacTxFlushBcnBmcQueue) (UINT_8 ucQueueIdx, UINT_8 ucBssidIdx, BOOLEAN fgBlockingMode, UINT_32 u4Timeout);
extern UINT_32 (*halMacTxStartBcnBmcQueue) (UINT_8 ucQueueIdx, UINT_8 ucBssidIdx);
extern VOID
(*halPmConfigTbttBasedTimer) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN UINT_16              u2BeaconPeriod, // unit of TU
    IN UINT_8               ucDtimPeriod,
    IN UINT_8               ucTbttWakePeriod,
    IN UINT_8               ucDtimWakePeriod,
    IN BOOLEAN              fgEnableTrigger
    );
extern VOID
(*halPmDelayUs) (
    UINT_32 u4Delay
    );
extern UINT_16
(*halPmEnableCalculateNextTbtt) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );
extern VOID
(*halPmEnablePreDtimTrigger) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );
extern VOID
(*halPmEnablePreTbttTimeupEvent) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );
extern VOID
(*halPmEnablePreTbttTrigger) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );
extern VOID
(*halPmEnableTbttTimer) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable,
    IN BOOLEAN              fgReCalNextTbttTime
    );
extern VOID
(*halPmEnableTbttTimeupEvent) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );
extern UINT_16
(*halPmGetNextTbtt) (
    IN ENUM_LOCAL_TSF_T     eTsfId
    );
extern UINT_32
(*halPmGetWifiSysUpTime) (
    VOID
    );
extern VOID
(*halPmSetPreTbttInterval) (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN UINT_16  u2PreTbttIntvUs
    );
extern VOID
(*halPmSetTsfHardwareUpdateMode) (
    IN ENUM_LOCAL_TSF_T    eTsfId,
    IN HAL_TSF_HW_UPDATE_MODE eMode
    );
extern OUT UINT_16
(*halPseCifDeqSig)(
    IN UINT_8       ucDePortid,
    IN UINT_8       ucDeQueueid,
    IN BOOLEAN      fgGetReverse
    );
extern VOID (*halPseFlowCtrlFfaFreeCntRead) (
    OUT PUINT_16     pu2FfaCnt,
    OUT PUINT_16     pu2FreePageCnt
    );
extern OUT BOOLEAN
(*halPseGetQlenCtrl) (
    IN   UINT_8       ucPortid,
    IN   UINT_8       ucQueueid,
    IN   BOOLEAN      fgQlenInPage,
    OUT  PUINT_16     pu2QlenReturnValue
    );
extern OUT UINT_32
(*halPsePageIdxToPageSize) (
    IN UINT_32 u4PageIdx
    );
extern VOID
(*halPsePollPseBusy) ( VOID );
extern OUT UINT_16
(*halPsePortOutputQueueSanityCheck) (
    IN UINT_8       ucPortid,
    IN UINT_8       ucQueueid
    );
extern OUT UINT_16
(*halPsePortPhyQueueSanityCheck) (
    IN UINT_8       ucPortid,
    IN UINT_8       ucQueueid
    );
extern OUT BOOLEAN
(*halPseQLenRead) (
    IN UINT_8       ucPortid,
    IN UINT_8       ucQueueid,
    OUT PUINT_16     pu2QLenRead
    );
extern VOID (*Hal_REG_READ_WRITE32)(UCHAR Read,UINT32 Addr,UINT32 *Value);
extern HAL_STATUS (*halSetBSSID)(    
	IN UINT_8       ucBssidIdx,
    IN BOOL         fgEnable,
    IN PUINT_8      pucBSSID);
extern HAL_STATUS
(*halSetOwnMACAddr)(
		IN UINT_8		ucIdx,
		IN BOOL 		fgEnable,
		IN PUINT_8		pucMACAddr
		);
extern unsigned int (*halTxConfigIFSTime) (unsigned int rIFSType, unsigned short u2Time);
extern HAL_STATUS
		(*halWtbl2ClearCounter)(
			IN UINT_8						   ucEntryIdx,
			IN ENUM_WTBL2_CLEAR_COUNTER_TYPE_T eWtbl2ClearCnt
			);
extern HAL_STATUS
(*halWtbl2ConfigAmpduCap)(
    IN UINT_8                           ucEntryIdx,
    IN P_HAL_WTBL_PEER_CAP_T            prWtblPeerCap
    );
extern HAL_STATUS
(*halWtbl2ConfigBaEntry)(
    IN UINT_8  ucWtblIdx,
    IN BOOLEAN fgEnable,
    IN UINT_8  ucTID,
    IN UINT_8  ucWinSizeEntryIdx
    );
extern HAL_STATUS
(*halWtbl2GetAmpduCap)(
    IN UINT_8                ucEntryIdx,
    IN P_HAL_WTBL_PEER_CAP_T prWtblPeerCap
    );
extern HAL_STATUS
(*halWtbl2SetSN)(
    IN UINT_8                      ucEntryIdx,
    IN ENUM_WTBL2_SN_SOURCE_TYPE_T eSnSource,
    IN UINT_16                     u2SN
    );
extern HAL_STATUS 
(*halWtblConfigTbl1EntryCap)(
    IN UINT_8                  ucEntryIdx,
    IN P_HAL_WTBL_TX_CONFIG_T  prWtblTxConfig,
    IN P_HAL_WTBL_FID_EID_T    prWtblFidEid,
    IN P_HAL_WTBL_SEC_CONFIG_T prWtblSecConfig
    );
extern HAL_STATUS
(*halWtblGetTbl1EntryCap) (
    IN UINT_8                        ucEntryIdx,
    OUT P_HAL_WTBL_TX_CONFIG_T       prWtblTxConfig,
    OUT P_HAL_WTBL_FID_EID_T         prWtblFidEid,
    OUT P_HAL_WTBL_SEC_CONFIG_T      prWtblSecConfig
    );
extern HAL_STATUS
		(*halWtblSetIpsm)(
			IN UINT_8 ucIpsm
			);
extern HAL_STATUS
		(*halWtblSetPsm)(
			IN UINT_8 ucPsm
			);
extern VOID (*Handle_BSS_Width_Trigger_Events)(
	RTMP_ADAPTER *pAd,
	UCHAR Channel);
extern P_MSDU_INFO_T (*hemAllocBcnPkt)();
extern void (*hemBcnPktFree)(P_MSDU_INFO_T pAmsduInfo);
extern WLAN_STATUS
(*hemExtCmdBcnUpdate)(
    P_MSDU_INFO_T prMsduInfo
    );
extern void (*hemSendBcnPkt)(P_MSDU_INFO_T pAmsduInfo);
extern INT (*ht_mode_adjust)(
	RTMP_ADAPTER *pAd,
	MAC_TABLE_ENTRY *pEntry,
	HT_CAPABILITY_IE *peer,
	RT_HT_CAPABILITY *my);
extern void (*HTParametersHook)(
	IN	RTMP_ADAPTER 		*pAd,
	IN	RTMP_STRING 		*pValueStr,
	IN	RTMP_STRING		 	*pInput);
extern NTSTATUS (*HwCtrlUpdateProtect)(RTMP_ADAPTER *pAd, UINT_32 mode);
extern VOID (*Indicate_AMPDU_Packet)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern VOID (*Indicate_AMSDU_Packet)(
	RTMP_ADAPTER *pAd,
	RX_BLK *pRxBlk,
	UCHAR wdev_idx);
extern VOID (*Indicate_EAPOL_Packet)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern VOID (*Indicate_Legacy_Packet)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern VOID (*InitChannelRelatedValue)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*InvalidStateWhenAssoc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenAuth)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenDisassociate)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenJoin)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenReassoc)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenScan)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*InvalidStateWhenStart)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*IoT_APRxErrorHandle)(RTMP_ADAPTER *pAd, PHEADER_802_11	pHeader, RXINFO_STRUC *pRxInfo);
extern UCHAR (*iot_FillChList)(
	IN RTMP_ADAPTER *pAd,
	IN ch_desc_t *pChDesp,
	IN UCHAR Offset, 
	IN UCHAR increment);
extern void (*Iot_fill_wep_key)(PCIPHER_KEY pSharedKey,PUINT_8 pbuffer,UINT_8 ucLength);
extern void (*iot_init)(void);
extern VOID (*Iot_init_profile)(RTMP_ADAPTER *pAd);
extern void (*iot_process)(void);
extern void (*iot_ram_assert)(const char*,int);
extern VOID	(*IoT_RTMPReportMicError)(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PCIPHER_KEY 	pWpaKey);
extern UCHAR (*Is_Rcvd_Pkt_Data_Or_Mgmt)(PUCHAR rcvd_data_buf);
extern VOID (*IterateOnBssTab)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*IterateOnBssTab2)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*LinkDown)(
	IN PRTMP_ADAPTER pAd,
	IN  BOOLEAN      IsReqFromAP);
extern VOID (*LinkUp)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR BssType);
extern VOID (*MacAddrRandomBssid)(
	IN PRTMP_ADAPTER pAd, 
	OUT PUCHAR pAddr);
extern INT32 (*MacTableDelEntryFromHash)(
	RTMP_ADAPTER 		*pAd,
	MAC_TABLE_ENTRY 	*pEntry);
extern BOOLEAN (*MacTableDeleteEntry)(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT wcid,
	IN PUCHAR pAddr);
extern MAC_TABLE_ENTRY *(*MacTableLookup)(
	IN PRTMP_ADAPTER pAd, 
	PUCHAR pAddr);
extern VOID (*MacTableMaintenance)(RTMP_ADAPTER *pAd);
extern VOID (*MacTableReset)(RTMP_ADAPTER *pAd);
extern VOID (*MacTableSetEntryPhyCfg)(
	RTMP_ADAPTER 	*pAd, 
	MAC_TABLE_ENTRY *pEntry);
extern VOID (*MakeBeacon)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, BOOLEAN UpdateRoutine);
extern INT (*MakeChanSwitchData)(PVOID pData, CHAN_SWITCH_T *ChanSwitch, UCHAR Channel);
extern VOID (*MakeChReportIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*MakeCountryIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*MakeErpIE)(
            RTMP_ADAPTER *pAd,
            struct wifi_dev *wdev,
            ULONG *pFrameLen,
            UCHAR *pBeaconFrame);
extern VOID (*MakeExtCapIE)(RTMP_ADAPTER *pAd, BSS_STRUCT *pMbss, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*MakeExtSuppRateIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*MakeHTIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern __romtext ULONG (*MakeOutgoingFrame)(
	OUT UCHAR *Buffer, 
	OUT ULONG *FrameLen, ...);
extern VOID (*MakeWmmIe)(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, ULONG *pFrameLen, UCHAR *pBeaconFrame);
extern VOID (*MgmtTableSetMcastEntry)(
	RTMP_ADAPTER 	*pAd,
	UCHAR 			wcid);
extern VOID (*MgtMacHeaderInit)(
	IN	PRTMP_ADAPTER	pAd, 
	IN OUT PHEADER_802_11 pHdr80211, 
	IN UCHAR SubType, 
	IN UCHAR ToDs, 
	IN PUCHAR pDA, 
	IN UCHAR *pSA,
	IN PUCHAR pBssid);
extern NDIS_STATUS (*MiniportMMRequest)(
		 IN  PRTMP_ADAPTER   pAd,
		 IN  UCHAR           QueIdx,
		 IN  PUCHAR          pData,
		 IN  UINT            Length);
extern VOID (*MlmeADDBAAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*MlmeAddBAReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2);
extern VOID (*MlmeAssocReqAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*MlmeAssocReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pApAddr, 
    OUT USHORT *pCapabilityInfo, 
    OUT ULONG *pTimeout, 
    OUT USHORT *pListenIntv);
extern VOID (*MlmeAuthReqAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*MlmeAuthReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr, 
    OUT ULONG *pTimeout, 
    OUT USHORT *pAlg);
extern VOID (*MlmeAutoReconnectLastSSID)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeAutoScan)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeCalculateChannelQuality)(
	IN PRTMP_ADAPTER pAd,
	IN ULONG Now32);
extern VOID (*MlmeCheckForFastRoaming)(
	IN	PRTMP_ADAPTER	pAd,
	IN	ULONG		Now);
extern VOID (*MlmeCheckPsmChange)(
	IN PRTMP_ADAPTER pAd,
	IN ULONG	Now32);
extern VOID (*MlmeCntlInit)(
	IN PRTMP_ADAPTER pAd, 
	IN STATE_MACHINE *S, 
	OUT STATE_MACHINE_FUNC Trans[]);
extern VOID (*MlmeCntlMachinePerformAction)(
	IN PRTMP_ADAPTER pAd, 
	IN STATE_MACHINE *S, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeDeAuthAction)(
    IN PRTMP_ADAPTER    pAd,
    IN MAC_TABLE_ENTRY  *pEntry,
	IN USHORT           Reason,
	IN BOOLEAN          bDataFrameFirst);
extern VOID (*MlmeDELBAAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*MlmeDelBAReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen);
extern BOOLEAN (*MlmeDequeue)(
	IN MLME_QUEUE *Queue, 
	OUT MLME_QUEUE_ELEM **Elem);
extern VOID (*MlmeDisassocReqAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeDLSAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
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
extern VOID (*MlmeHalt)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeHandler)(
	IN PRTMP_ADAPTER pAd);
extern NDIS_STATUS (*MlmeHardTransmit)(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR	QueIdx,
	IN	PNDIS_PACKET	pPacket);
extern NDIS_STATUS (*MlmeHardTransmitMgmtRing)(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR	QueIdx,
	IN	PNDIS_PACKET	pPacket);
extern NDIS_STATUS (*MlmeInit)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeInvalidAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeJoinReqAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmePeriodicExec)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*MlmeQOSAction)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*MlmeQueueFull)(
	IN MLME_QUEUE *Queue);
extern NDIS_STATUS (*MlmeQueueInit)(
	IN MLME_QUEUE *Queue);
extern VOID (*MlmeRadioOff)(IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeRadioOn)(IN PRTMP_ADAPTER pAd);
extern VOID (*MlmeReassocReqAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*MlmeResetRalinkCounters)(
	IN  PRTMP_ADAPTER   pAd);
extern VOID (*MlmeRestartStateMachine)(
	IN	PRTMP_ADAPTER	pAd);
extern VOID (*MlmeScanReqAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*MlmeScanReqSanity)(
	IN PRTMP_ADAPTER pAd, 
	IN VOID *Msg, 
	IN ULONG MsgLen, 
	OUT UCHAR *pBssType, 
	OUT CHAR Ssid[], 
	OUT UCHAR *pSsidLen, 
	OUT UCHAR *pScanType);
extern VOID (*MlmeSelectTxRateTable)(
	IN PRTMP_ADAPTER        pAd,
	IN PMAC_TABLE_ENTRY		pEntry,
	IN PUCHAR               *ppTable,
	IN PUCHAR               pTableSize,
	IN PUCHAR               pInitTxRateIdx);
extern VOID (*MlmeSetPsmBit)(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT psm);
extern VOID (*MlmeSetTxPreamble)(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT TxPreamble);
extern VOID (*MlmeStartReqAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*MlmeStartReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT CHAR Ssid[], 
    OUT UCHAR *pSsidLen);
extern VOID (*MlmeUpdateHtTxRates)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 Wcid);
extern VOID (*MlmeUpdateTxRates)(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN		 bLinkUp,
	IN UCHAR apidx);
extern BOOLEAN (*MlmeValidateSSID)(
	IN CHAR	*pSsid,
	IN UCHAR	SsidLen);
extern BOOLEAN (*MsgTypeSubst)(
	IN PRTMP_ADAPTER  pAd,
	IN PFRAME_802_11 pFrame, 
	OUT INT *Machine, 
	OUT INT *MsgType);
extern VOID (*mt7636_chipCap_init)(VOID);
extern VOID (*mt7636_init)(RTMP_ADAPTER *pAd);
extern VOID (*MtAsicAddRemoveKeyByDriver)(
	PRTMP_ADAPTER	pAd,
	MT_SECURITY_CTRL SecurityCtrl);
extern VOID (*MtAsicSetEdcaParm)(RTMP_ADAPTER *pAd, PEDCA_PARM pEdcaParm);
extern INT32 (*MtAsicSetMacTxRx)(RTMP_ADAPTER *pAd, INT32 TxRx, BOOLEAN Enable,UCHAR BandIdx);
extern VOID (*MtAsicSetMbssWdevIfAddr)(RTMP_ADAPTER *pAd, INT idx, UCHAR *if_addr, INT opmode);
extern INT (*MtAsicSetPreTbtt)(struct _RTMP_ADAPTER *pAd, BOOLEAN bEnable,UINT32 BssidIdx);
extern INT (*MtAsicSetRxFilter)(RTMP_ADAPTER *pAd,MT_RX_FILTER_CTRL_T RxFilter);
extern INT (*MtAsicSetRxStream)(RTMP_ADAPTER *pAd, UINT32 StreamNums,UCHAR BandIdx);
extern INT (*MtAsicSetTxStream)(RTMP_ADAPTER *pAd, UINT32 StreamNums,UCHAR BandIdx);
extern INT32 (*MtAsicUpdateBASessionByDriver)(
	RTMP_ADAPTER *pAd,
	MT_BA_CTRL_T BaCtrl);
extern VOID (*MtAsicUpdateProtect)(
	IN RTMP_ADAPTER *pAd,
	IN IOT_PROTECT_CTRL_T *Protect);
extern VOID (*MtAsicUpdateRTSThreshold)(
	PRTMP_ADAPTER pAd, MT_RTSCTS_CTRL_T RtsCtsCtrl);
extern VOID (*MtAsicUpdateRxWCIDTableByDriver)(
	RTMP_ADAPTER *pAd,
	MT_WCID_TABLE_INFO_T WtblInfo);
extern INT (*mt_hw_tb_init)(RTMP_ADAPTER *pAd, BOOLEAN bHardReset);
extern INT (*MtSmacAsicDisableBeacon)(RTMP_ADAPTER *pAd, VOID *wdev_void);
extern INT (*MtSmacAsicEnableBeacon)(RTMP_ADAPTER *pAd, VOID *wdev_void, UCHAR NumOfBcns);
extern BOOLEAN (*MtSmacAsicSetArbBcnQCR)(
    RTMP_ADAPTER 	*pAd,
    UINT8 			OmacIdx,
    BOOLEAN 		enable);
extern INT (*mt_wtbl_get_entry234)(
		RTMP_ADAPTER 		*pAd, 
		UCHAR 				widx, 
		struct wtbl_entry 	*ent);
extern INT (*mt_wtbl_init)(RTMP_ADAPTER *pAd);
extern struct net_pkt_blk * (*net_buf_join_pkt_blk)(struct net_buf_blk * buf_blk);
extern struct node_obj * (*net_dequeue)(struct head_list *list);
extern void (*net_enqueue)(struct head_list *list, struct node_obj *blk);
extern void (*net_enqueue_unlock)(struct head_list *list, struct node_obj *blk);
extern struct netif *
(*netif_add_wifi_n9)(struct netif *netif, void *adapter, int (* setup)(struct netif *netif));
extern void (*netif_initialize)(void);
extern int (*netlib_packet_buffer_init)(void);
extern struct net_pkt_blk * (*net_pkt_alloc)(int size_idx);
extern struct net_pkt_blk * (*net_pkt_clone)(struct net_pkt_blk *pkt);
extern void (*net_pkt_free)(struct net_pkt_blk * pkt_blk);
extern void (*net_pkt_free_evt)(void *ptr);
extern void (*net_pkt_free_keep_pse)(struct net_pkt_blk * pkt_blk);
extern void (*net_pkt_free_net_buf)(struct net_pkt_blk * pkt_blk);
extern struct net_pkt_blk * (*net_pkt_get)(int size_idx, UINT_16 u2PseFid);
extern struct net_pkt_blk * (*net_pkt_get_87)(int size_idx,void *buffer);
extern struct net_pkt_blk * (*net_pkt_get_from_pbuf)(int size_idx, UINT_16 u2PseFid,struct pbuf_boson *new_pbuf_pkt,UINT_16 reserved_headroom_len);
extern int (*net_pkt_init)(int num);
extern unsigned char * (*net_pkt_push)(struct net_pkt_blk *pkt, unsigned int len);
extern unsigned char * (*net_pkt_put)(struct net_pkt_blk *pkt, u32 len);
extern void (*net_pkt_reserve)(struct net_pkt_blk *pkt_blk, int len);
extern void (*net_pkt_show)(void);
extern UCHAR (*NextChannel)(
	IN PRTMP_ADAPTER pAd, 
	IN UCHAR channel);
extern VOID (*NICDisableInterrupt)(
    IN  PRTMP_ADAPTER   pAd);
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
extern VOID	(*NICReadEEPROMParameters)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			mac_addr);
extern VOID (*NICUpdateRawCounters)(
	IN PRTMP_ADAPTER pAd);
extern WLAN_STATUS (*OffloadChannelSwitch)(P_CHAN_SWITCH_T prChanSwitch);
extern WLAN_STATUS (*OffloadEdcaSet)(P_EDCA_SET_T prEdcaSet);
extern VOID (*OffloadSetTxPreamble)(RTMP_ADAPTER *Ad, USHORT TxPreamble);
extern WLAN_STATUS (*OffloadSlotTimeSet)(P_SLOT_TIME_SET_T prSlotTimeSet);
extern NDIS_STATUS  (*os_alloc_mem)(
			IN  PRTMP_ADAPTER pAd,
			OUT PUCHAR *mem,
			IN  ULONG  size);
extern NDIS_STATUS (*os_free_mem)(
		   IN  PRTMP_ADAPTER pAd,
		   IN  PUCHAR mem);
extern MAC_TABLE_ENTRY *(*PACInquiry)(RTMP_ADAPTER *pAd, UCHAR Wcid);
extern VOID (*PeerAddBAReqAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*PeerAddBAReqActionSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *pMsg, 
    IN ULONG MsgLen,
	OUT PUCHAR pAddr2);
extern VOID (*PeerAddBARspAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*PeerAddBARspActionSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *pMsg, 
    IN ULONG MsgLen);
extern BOOLEAN (*PeerAssocReqCmmSanity)(
	RTMP_ADAPTER *pAd, 
	BOOLEAN isReassoc,
	VOID *Msg,
	INT MsgLen,
	IE_LISTS *ie_lists);
extern VOID (*PeerAssocRspAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
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
extern VOID (*PeerAuthRspAtSeq2Action)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerAuthRspAtSeq4Action)(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*PeerAuthSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr, 
    OUT USHORT *pAlg, 
    OUT USHORT *pSeq, 
    OUT USHORT *pStatus, 
    UCHAR *pChlgText);
extern VOID (*PeerBAAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerBeacon)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*PeerBeaconAndProbeRspSanity)(
	IN PRTMP_ADAPTER pAd,
	IN VOID *Msg, 
	IN ULONG MsgLen, 
	IN UCHAR  MsgChannel,
	OUT BCN_IE_LIST *ie_list,
	OUT USHORT *LengthVIE,	
	OUT PNDIS_802_11_VARIABLE_IEs pVIE,
	IN BOOLEAN bGetDtim);
extern BOOLEAN (*PeerBeaconAndProbeRspSanity2)(
	IN PRTMP_ADAPTER pAd, 
	IN VOID *Msg, 
	IN ULONG MsgLen, 
	IN OVERLAP_BSS_SCAN_IE *BssScan,
	OUT UCHAR 	*RegClass);
extern VOID (*PeerBeaconAtJoinAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerBeaconAtScanAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerChSwAnnAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*PeerChSwAnnSanity)(
	IN PRTMP_ADAPTER pAd,
	IN VOID *pMsg,
	IN ULONG MsgLen,
	OUT PCH_SW_ANN_INFO pChSwAnnInfo);
extern VOID (*PeerDeauthAction)(
    IN PRTMP_ADAPTER pAd, 
    IN PMLME_QUEUE_ELEM Elem);
extern BOOLEAN (*PeerDeauthReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT	UINT16	*SeqNum, 
    OUT USHORT *Reason);
extern BOOLEAN (*PeerDeauthSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT USHORT *pReason);
extern VOID (*PeerDelBAAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*PeerDelBAActionSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN UCHAR Wcid, 
    IN VOID *pMsg, 
    IN ULONG MsgLen );
extern VOID (*PeerDisassocAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*PeerDisassocReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT	UINT16	*SeqNum,
    OUT USHORT *Reason);
extern BOOLEAN (*PeerDisassocSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT USHORT *pReason);
extern VOID (*PeerHTAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerProbeReqAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*PeerProbeReqSanity)(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2,
    OUT CHAR Ssid[], 
    OUT UCHAR *pSsidLen);
extern VOID (*PeerPublicAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerQOSAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerReassocRspAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*PeerSpectrumAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern OUT VOID *
(*PseFrameIDtoVmAddr) (
    IN UINT_16 u2FrameID
    );
extern OUT UINT_16
(*PseFreeQueueUsableCntNum) (
    VOID
    );
extern OUT BOOLEAN
(*PsegGetWtbl2to4Entry)(
	IN  UINT_8 		ucWlanIdx,
	
	OUT PUINT_16 	pu2Wtbl2Fid,
	OUT PUINT_8    	pucWtbl2Eid,
	OUT VOID**    	ppWtbl2VmAddr,
	
	OUT PUINT_16    pu2Wtbl3Fid,
	OUT PUINT_8     pucWtbl3Eid,
	OUT VOID**     	ppWtbl3VmAddr,
	
	OUT PUINT_16    pu2Wtbl4Fid,
	OUT PUINT_8     pucWtbl4Eid,
	OUT VOID**     	ppWtbl4VmAddr	
	);
extern UCHAR (*RandomByte)(
	IN PRTMP_ADAPTER pAd);
extern BOOLEAN (*Reason_Code_Validate)(UINT_8 *NEW_AP_BSSID,UINT_16 New_Reason_Code);
extern VOID (*ReassocTimeout)(IN PVOID SystemSpecific1, 
					IN PVOID FunctionContext, 
					IN PVOID SystemSpecific2, 
					IN PVOID SystemSpecific3);
extern VOID  (*ReassocTimeoutAction)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);
extern VOID (*ReservedAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern void (*resetopmode)();
extern VOID (*respond_ht_information_exchange_action)(
		IN PRTMP_ADAPTER pAd,
		IN MLME_QUEUE_ELEM *Elem);
extern BOOLEAN (*rlmSetChannel)(
    ENUM_BAND_T             eBand,
    UINT_8                  ucPriChannel,
    ENUM_CHNL_EXT_T         eExtend,
    UINT_8                  ucChannelS1,
    UINT_8                  ucChannelS2,
    ENUM_CHANNEL_WIDTH_T    eChannelWidth,
    ENUM_RECAL_TYPE         eReCal,
    UINT_32                 eDbdcBandIdx
    );
extern int (*rom_memcmp)(const void * cs, const void * ct, size_t count);
extern void *(*rom_memcpy)(void *dest, const void *src, size_t n);
extern void *(*rom_memmove)(void *dest, const void *src, size_t n);
extern void *(*rom_memset)(void *s, int c, size_t n);
extern int (*rom_strcmp)(const char *s1, const char *s2);
extern size_t (*rom_strlen)(const char *s);
extern INT (*RspByInBands)(
	   IN  USHORT cmd_id,
	   IN  CHAR *msg, 
	   IN  INT   total,
	   IN  USHORT cmd_seq,
	   IN  INT status);
extern error_t (*rt28xx_packet_xmit)(PNDIS_PACKET p);
extern error_t (*rt28xx_send_packets)(
		IN struct netif 		*net_dev, 
		IN PNDIS_PACKET 		pPacket);
extern VOID (*RT28xx_UpdateBeaconToAsic)(
	RTMP_ADAPTER *pAd,
	INT apidx,
	ULONG FrameLen,
	ULONG UpdatePos);
extern INT (*RT_CfgSetCountryRegion)(RTMP_ADAPTER *pAd, UINT_8 region, INT band);
extern INT (*RT_CfgSetMacAddress)(RTMP_ADAPTER *pAd, RTMP_STRING *arg);
extern VOID (*RTMPAddWcidAttributeEntry)(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			BssIdx,
	IN 	UCHAR		 	KeyIdx,
	IN 	UCHAR		 	CipherAlg,
	IN 	MAC_TABLE_ENTRY *pEntry);
extern NDIS_STATUS	(*RTMPAllocAdapterBlock)(
	OUT	PRTMP_ADAPTER	*ppAdapter);
extern NDIS_STATUS (*RTMPAllocateNdisPacket)(
	  IN  PRTMP_ADAPTER   pAd,
	  OUT PNDIS_PACKET   *ppPacket,
	  IN  PUCHAR          pHeader,
	  IN  UINT            HeaderLen,
	  IN  PUCHAR          pData,
	  IN  UINT            DataLen);
extern INT (*rtmp_ap_init)(RTMP_ADAPTER *pAd);
extern VOID (*RTMP_BASetup)(
		RTMP_ADAPTER 	*pAd,
		STA_TR_ENTRY 	*tr_entry,
		UINT8 			UPriority);
extern USHORT	(*RTMPCalcDuration)(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			Rate,
	IN	ULONG			Size);
extern __romtext UINT	(*RTMP_CALC_FCS32)(
	IN	UINT	Fcs,
	IN	PUCHAR	Cp,
	IN	INT		Len);
extern VOID  (*RTMPCalculateMICValue)(
	 IN  TKIP_KEY_INFO   *pTx,
	 IN  PNDIS_PACKET    pPacket,
	 IN  PUCHAR          pEncap,
	 IN  PCIPHER_KEY     pKey,
	 IN  UCHAR           apidx);
extern BOOLEAN (*RTMPCheckAKM)(PUCHAR sta_akm, PUCHAR ap_rsn_ie, INT iswpa2);
extern BOOLEAN (*RTMPCheckAUTH)(
    IN PRTMP_ADAPTER    pAd,
    IN PEID_STRUCT      eid_ptr,
    IN MAC_TABLE_ENTRY	*pEntry);
extern BOOLEAN (*RTMPCheckChannel)(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		CentralChannel,
	IN UCHAR		Channel);
extern BOOLEAN (*RTMPCheckEtherType)(
	IN PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket);
extern BOOLEAN (*RTMPCheckHt)(
	IN		PRTMP_ADAPTER	pAd,
	IN		UCHAR	Wcid,
	IN OUT	HT_CAPABILITY_IE			*pHtCapability,
	IN OUT	ADD_HT_INFO_IE			*pAddHtInfo);
extern BOOLEAN (*RTMPCheckMcast)(
    IN PRTMP_ADAPTER    pAd,
    IN PEID_STRUCT      eid_ptr,
    IN MAC_TABLE_ENTRY  *pEntry);
extern VOID (*RTMPCheckRates)(
	IN		PRTMP_ADAPTER	pAd,
	IN OUT	UCHAR			SupRate[],
	IN OUT	UCHAR			*SupRateLen);
extern NDIS_STATUS (*RTMPCheckRxError)(
	IN	PRTMP_ADAPTER		pAd, 
	IN	PHEADER_802_11		pHeader,
	IN	PRXINFO_STRUC 		pRxInfo);
extern BOOLEAN (*RTMPCheckUcast)(
    IN PRTMP_ADAPTER    pAd,
    IN PEID_STRUCT      eid_ptr,
    IN MAC_TABLE_ENTRY	*pEntry);
extern __romtext BOOLEAN (*RTMPCheckWPAframe)(
    IN PUCHAR           pData,
    IN ULONG            DataByteCount);
extern INT (*RtmpChipOpsRegister)(RTMP_ADAPTER *pAd, INT infType);
extern BOOLEAN (*rtmp_chk_tkip_mic)(
    IN RTMP_ADAPTER *pAd, 
    IN MAC_TABLE_ENTRY *pEntry, 
    IN RX_BLK *pRxBlk);
extern __romtext ULONG	(*RTMPCompareMemory)(
	IN	PVOID	pSrc1,
	IN	PVOID	pSrc2,
	IN	ULONG	Length);
extern PNDIS_PACKET (*RTMPDeFragmentDataFrame)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk);
extern INT (*rtmp_deq_report)(
		RTMP_ADAPTER 		*pAd,
		struct dequeue_info *info);
extern INT (*rtmp_deq_req)(
		RTMP_ADAPTER 		*pAd,
		INT 				cnt,
		struct dequeue_info *info);
extern VOID (*RTMPDeQueuePacket)(
		IN RTMP_ADAPTER *pAd,
		IN BOOLEAN 		in_hwIRQ,
		IN UCHAR 		QIdx,
		IN INT 			wcid,
		IN INT 			max_cnt);
extern VOID (*RTMPDisableDesiredHtInfo)(RTMP_ADAPTER *pAd);
extern VOID (*RTMPEnableRxTx)(
	IN PRTMP_ADAPTER	pAd);
extern __romtext VOID	(*RTMPEncryptData)(
	IN	PRIVATE_STRUC	*pPrivateInfo,	
	IN	PUCHAR			pSrc,
	IN	PUCHAR			pDest,
	IN	UINT			Len);
extern INT (*rtmp_enq_req)(
		RTMP_ADAPTER 		*pAd,
		PNDIS_PACKET 		pkt, 
		UCHAR 				qidx, 
		STA_TR_ENTRY 		*tr_entry,
		BOOLEAN 			FlgIsLocked,
		QUEUE_HEADER 		*pPktQueue);
extern VOID (*RtmpEnqueueNullFrame)(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR *pAddr,
	IN UCHAR TxRate,
	IN UCHAR AID,
	IN UCHAR apidx,
	IN BOOLEAN bQosNull,
	IN BOOLEAN bEOSP,
	IN UCHAR OldUP);
extern VOID (*RTMP_FillTxBlkInfo)(
	IN RTMP_ADAPTER *pAd, 
	IN TX_BLK *pTxBlk);
extern VOID	(*RTMPFreeAdapter)(
	IN	VOID		*pAdSrc);
extern VOID (*RTMPFreeNdisPacket)(
	   IN PRTMP_ADAPTER pAd,
	   IN PNDIS_PACKET  pPacket);
extern VOID (*RTMPHandlePreTBTTInterrupt)(
	IN  PRTMP_ADAPTER   pAd);
extern VOID (*RTMPHandleTBTTInterrupt)(
	IN  PRTMP_ADAPTER   pAd);
extern void (*RTMP_IndicateMediaState)(
	IN	PRTMP_ADAPTER	pAd);
extern __romtext VOID	(*RTMPInitMICEngine)(
	IN	TKIP_KEY_INFO  	*pTx_Key,
	IN	PUCHAR			pKey,
	IN	PUCHAR			pDA,
	IN	PUCHAR			pSA,
	IN  UCHAR           UserPriority,
	IN	PUCHAR			pMICKey);
extern VOID (*RTMPInitWepEngine)(
	IN	PRTMP_ADAPTER	pAd,	
	IN	PUCHAR			pKey,
	IN	UCHAR			KeyId,
	IN	UCHAR			KeyLen, 
	IN OUT	PUCHAR		pDest);
extern __romtext VOID (*RTMPMakeRSNIE)(
	IN  PRTMP_ADAPTER   pAd,	
	IN  UINT            AuthMode,
	IN  UINT            WepStatus,
	IN	UCHAR		apidx);
extern __romtext VOID (*RTMPMakeRsnIeAKM)(	
	IN  PRTMP_ADAPTER   pAd,	
	IN	UCHAR			ElementID,	
	IN	UINT			AuthMode,
	IN	UCHAR			apidx,
	OUT	PUCHAR			pRsnIe,
	OUT	UCHAR			*rsn_len);
extern __romtext VOID (*RTMPMakeRsnIeCipher)(
	IN  PRTMP_ADAPTER   pAd,
	IN	UCHAR			ElementID,	
	IN	UINT			WepStatus,
	IN	UCHAR			apidx,
	IN	BOOLEAN			bMixCipher,
	IN	UCHAR			FlexibleCipher,
	OUT	PUCHAR			pRsnIe,
	OUT	UCHAR			*rsn_len);
extern CHAR (*RTMPMaxRssi)(
	IN PRTMP_ADAPTER	pAd,
	IN CHAR				Rssi0,
	IN CHAR				Rssi1,
	IN CHAR				Rssi2);
extern VOID (*RTMPOffloadPm)(RTMP_ADAPTER *pAd, UINT_8 WlanIdx, UINT_8 PmNumber, UINT_8 PmState);
extern void (*RTMP_QueryPacketInfo)(
	 IN  PNDIS_PACKET pPacket,
	 OUT PACKET_INFO  *pPacketInfo,
	 OUT PUCHAR       *pSrcBufVA,
	 OUT UINT         *pSrcBufLen);
extern INT (*RtmpRaDevCtrlInit)(VOID *pAdSrc, int infType);
extern void (*rtmp_read_ap_wmm_parms_from_file)(
RTMP_ADAPTER 	*pAd,
RTMP_STRING 	*tmpbuf,
RTMP_STRING 	*buffer);
extern VOID	(*RTMPReadChannelPwr)(
	IN	PRTMP_ADAPTER	pAd);
extern VOID (*RTMPResumeMsduTransmission)(
	IN	PRTMP_ADAPTER	pAd);
extern VOID (*RTMPSendNullFrame)(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			TxRate,
	IN	BOOLEAN 		bQosNull);
extern VOID (*RTMPSendWirelessEvent)(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Event_flag,
	IN	PUCHAR 			pAddr,
	IN	UCHAR			BssIdx,
	IN	CHAR			Rssi);
extern VOID (*RTMPSetAGCInitValue)(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			BandWidth);
extern void (*RTMPSetCountryCode)(
	RTMP_ADAPTER 	*pAd,
	RTMP_STRING 	*CountryCode);
extern VOID (*RTMPSetHT)(
	IN	PRTMP_ADAPTER	pAd,
	IN	OID_SET_HT_PHYMODE *pHTPhyMode);
extern __romtext VOID	(*RTMPSetICV)(
	IN	PRIVATE_STRUC	*pPrivateInfo,
	IN	PUCHAR	pDest);
extern VOID (*RTMPSetIndividualHT)(RTMP_ADAPTER *pAd, UCHAR apidx);
extern VOID (*RTMPSetLED)(
	IN PRTMP_ADAPTER 	pAd, 
	IN UCHAR			Status);
extern VOID (*RTMPSetPhyMode)(
	IN	PRTMP_ADAPTER	pAd,
	IN	ULONG phymode);
extern VOID (*RTMPSetPiggyBack)(
    IN PRTMP_ADAPTER    pAd,
    IN BOOLEAN          bPiggyBack);
extern NDIS_STATUS (*RTMPSetProfileParameters)(
		IN RTMP_ADAPTER 	*pAd,
		IN RTMP_STRING 		*_pBuffer);
extern VOID (*RTMPSetSignalLED)(
	IN PRTMP_ADAPTER 	pAd, 
	IN NDIS_802_11_RSSI Dbm);
extern void (*RTMPSetSTASSID)(
	RTMP_ADAPTER 		*pAd, 
	RTMP_STRING 		*SSID);
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
extern VOID (*RTMPSetSupportMCS_STA)(
    IN PRTMP_ADAPTER pAd,
    IN UCHAR OpMode,
    IN PMAC_TABLE_ENTRY	pEntry,
    IN UCHAR SupRate[],
    IN UCHAR SupRateLen,
    IN UCHAR ExtRate[],
    IN UCHAR ExtRateLen,
    IN HT_CAPABILITY_IE *pHtCapability,
    IN UCHAR HtCapabilityLen
    );
extern VOID (*RTMPSuspendMsduTransmission)(
	IN	PRTMP_ADAPTER	pAd);
extern __romtext VOID	(*RTMPTkipAppend)( 
	IN	PTKIP_KEY_INFO	pTkip,	
	IN	PUCHAR			pSrc,
	IN	UINT			nBytes);
extern __romtext VOID	(*RTMPTkipAppendByte)( 
	IN	PTKIP_KEY_INFO	pTkip,	
	IN	UCHAR 			uChar);
extern __romtext BOOLEAN	(*RTMPTkipCompareMICValue)(
	TKIP_KEY_INFO   	*pRx_Key,
	IN	PUCHAR			pSrc,
	IN	PUCHAR			pDA,
	IN	PUCHAR			pSA,
	IN	PUCHAR			pMICKey,
	IN	UCHAR			UserPriority,
	IN	UINT			Len);
extern __romtext VOID	(*RTMPTkipGetMIC)( 
	IN	PTKIP_KEY_INFO	pTkip);
extern __romtext ULONG	(*RTMPTkipGetUInt32)( 	
	IN	PUCHAR	pMICKey);
extern __romtext VOID	(*RTMPTkipPutUInt32)(
	IN OUT	PUCHAR		pDst,
	IN		ULONG		val);
extern __romtext VOID (*RTMPTkipSetMICKey)(  
	IN	PTKIP_KEY_INFO	pTkip,	
	IN	PUCHAR			pMICKey);
extern INT (*rtmp_tx_swq_exit)(
		RTMP_ADAPTER 	*pAd,
		UCHAR 			wcid);
extern INT (*rtmp_tx_swq_init)(RTMP_ADAPTER *pAd);
extern VOID (*RTMPUpdateHTIE)(
	  IN  RT_HT_CAPABILITY    *pRtHt,
	  OUT     HT_CAPABILITY_IE *pHtCapability,
	  OUT     ADD_HT_INFO_IE      *pAddHtInfo);
extern VOID (*RTMPUpdateLegacyTxSetting)(
	UCHAR fixed_tx_mode,
	PMAC_TABLE_ENTRY pEntry);
extern VOID (*RTMPUpdateMlmeRate)(
	IN PRTMP_ADAPTER	pAd);
extern VOID (*RTMPusecDelay)(IN  ULONG   usec);
extern INT32 (*rtmp_wdev_idx_reg)(
		RTMP_ADAPTER 		*pAd,
		struct wifi_dev 	*wdev);
extern VOID	(*RTMPWPARemoveAllKeys)(
	IN	PRTMP_ADAPTER	pAd);
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
extern VOID (*RTMPWriteTxWI_Cache)(
	IN	PRTMP_ADAPTER		pAd,
	IN	OUT PTXWI_STRUC		pTxWI,
	IN	TX_BLK				*pTxBlk);
extern VOID (*RTMPWriteTxWI_Data)(
	IN	PRTMP_ADAPTER		pAd,
	IN	OUT PTXWI_STRUC		pTxWI,
	IN	TX_BLK				*pTxBlk);
extern VOID (*RxFilterCfg2Row)(UINT32 FilterMask,UINT32 *RowFilterMask);
extern VOID (*ScanNextChannel)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*ScanParmFill)(
	IN PRTMP_ADAPTER pAd, 
	IN OUT MLME_SCAN_REQ_STRUCT *ScanReq, 
	IN CHAR Ssid[], 
	IN UCHAR SsidLen, 
	IN UCHAR BssType, 
	IN UCHAR ScanType);
extern VOID (*ScanTimeout)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*ScanTimeoutAction)(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
extern VOID (*SecurityAddKey)(
	RTMP_ADAPTER 	*pAd,
	UCHAR 			OperMode,
	UCHAR 			CipherAlg,
	CMD_802_11_KEY CmdKey);
extern VOID (*SecurityInbandReq2)(UINT_8 ucWlanIndex, UINT32 *pKey, UINT32 ucKeyLen, UINT32 addr,
		UINT32 *word0, UINT32 *word1, UINT32 *word2, UINT_8 add);
extern VOID (*SecurityInbandReq3)(UINT8 arg0, UINT32 arg1, UINT32 arg2, UINT32 arg3);
extern VOID (*SecurityRemoveKey)(
	PRTMP_ADAPTER 	pAd,
	UCHAR 			CipherAlg,
	CMD_802_11_KEY CmdKey);
extern VOID (*Send2040CoexistAction)(
		IN	PRTMP_ADAPTER	pAd,
		IN	  UCHAR  Wcid,
		IN	BOOLEAN bAddIntolerantCha);
extern VOID (*SendBeaconRequest)(
				   IN PRTMP_ADAPTER		pAd,
				   IN UCHAR				Wcid);
extern VOID (*SendBeaconToHost)(
	IN  CHAR	*pMsg,
	IN  INT	MsgLen,
	IN  CHAR Channel,
	IN  CHAR Rssi);
extern VOID (*SendHT2040ToHost)(IN  CHAR bw);
extern VOID (*SendNotifyBWActionFrame)(RTMP_ADAPTER *pAd, UCHAR Wcid, UCHAR apidx);
extern void (*SendProbeReqToHost)( VOID *Msg, ULONG MsgLen);
extern INT	(*SetCommonHT)(IN PRTMP_ADAPTER	pAd);
extern VOID (*set_default_ap_edca_param)(RTMP_ADAPTER *pAd);
extern VOID (*set_default_sta_edca_param)(RTMP_ADAPTER *pAd);
extern INT (*set_ht_fixed_mcs)(RTMP_ADAPTER *pAd, MAC_TABLE_ENTRY *pEntry, UCHAR fixed_mcs, UCHAR mcs_bound);
extern INT (*Set_MonitorMode_Proc)(RTMP_ADAPTER *pAd, UINT8 arg);
extern VOID (*setOpMode)(UINT_32 OpMode);
extern VOID (*set_sta_ht_cap)(RTMP_ADAPTER *pAd, MAC_TABLE_ENTRY *ent, HT_CAPABILITY_IE *ht_ie);
extern NDIS_STATUS (*Sniff2BytesFromNdisBuffer)(
	 IN  PNDIS_BUFFER    pFirstBuffer,
	 IN  UCHAR           DesiredOffset,
	 OUT PUCHAR          pByte0,
	 OUT PUCHAR          pByte1);
extern BOOLEAN (*StaAddMacTableEntry)(
	IN  PRTMP_ADAPTER		pAd,
	IN  PMAC_TABLE_ENTRY	pEntry,
	IN  UCHAR				MaxSupportedRateIn500Kbps,
	IN  HT_CAPABILITY_IE	*pHtCapability,
	IN  UCHAR				HtCapabilityLen,
	IN  ADD_HT_INFO_IE		*pAddHtInfo,
	IN  UCHAR				AddHtInfoLen,
	IN  USHORT        		CapabilityInfo);
extern VOID (*STA_AMPDU_Frame_Tx)(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk);
extern VOID (*STA_AMSDU_Frame_Tx)(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk);
extern PUCHAR (*STA_Build_AMSDU_Frame_Header)(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK		*pTxBlk);
extern VOID (*STABuildCache802_11Header)(
	IN RTMP_ADAPTER		*pAd,
	IN TX_BLK			*pTxBlk,
	IN UCHAR			*pHeader);
extern VOID (*STABuildCommon802_11Header)(
	IN  PRTMP_ADAPTER   pAd,
	IN  TX_BLK          *pTxBlk);


#if (MT7687_BORA00007143 == 1)
extern void (*sta_connect) (UINT_8 Modified__first_channel);
#else
extern void (*sta_connect)();
#endif
extern void (*sta_disconnect)();
extern VOID (*STAFindCipherAlgorithm)(
	IN PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk);
extern VOID (*STA_Fragment_Frame_Tx)(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK		*pTxBlk);
extern void (*sta_get_bssid)(CHAR *pssid, UCHAR ssidlen, UCHAR *pbssid);
extern VOID (*STAHandleRxControlFrame)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk);
extern VOID (*STAHandleRxDataFrame)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk);
extern VOID (*STAHandleRxMgmtFrame)(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk);
extern NDIS_STATUS (*STAHardTransmit)(
	IN	PRTMP_ADAPTER	pAd, 
	IN TX_BLK 			*pTxBlk,
	IN	UCHAR			QueIdx);
extern VOID (*STA_Legacy_Frame_Tx)(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk);
extern VOID (*STAMacTableReset)(
	IN  PRTMP_ADAPTER  pAd);
extern VOID (*STAMlmePeriodicExec)(PRTMP_ADAPTER pAd);
extern VOID (*StaPublicAction)(
		IN PRTMP_ADAPTER pAd, 
		IN BSS_2040_COEXIST_IE *pBssCoexIE);
extern VOID (*StaQuickResponeForRateUpExec)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*StartParmFill)(
	IN PRTMP_ADAPTER pAd, 
	IN OUT MLME_START_REQ_STRUCT *StartReq, 
	IN CHAR Ssid[], 
	IN UCHAR SsidLen);
extern VOID (*STARxDataFrameAnnounce)(
	IN	PRTMP_ADAPTER	pAd,
	IN	MAC_TABLE_ENTRY	*pEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern VOID (*STARxDoneInterruptHandle)(
	IN	PRTMP_ADAPTER	pAd, 
	IN  PNDIS_PACKET	pRxPacket);
extern VOID (*STARxEAPOLFrameIndicate)(
	IN	PRTMP_ADAPTER	pAd,
	IN	MAC_TABLE_ENTRY	*pEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
extern NDIS_STATUS (*STASendPacket)(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket);
extern VOID (*STASendPackets)(
	IN	NDIS_HANDLE		MiniportAdapterContext,
	IN	PPNDIS_PACKET	ppPacketArray,
	IN	UINT			NumberOfPackets);
extern void (*sta_set_auth_mode)(char auth_mode);
extern void (*sta_set_bw)(unsigned char bw);
extern void (*sta_set_encr_type)(char encr_type);
extern void (*sta_set_mac_addr)(unsigned char *mac_addr);
extern void (*sta_site_survey)(char enable);
extern VOID (*StateMachineInit)(
	IN STATE_MACHINE *S, 
	IN STATE_MACHINE_FUNC Trans[], 
	IN ULONG StNr, 
	IN ULONG MsgNr, 
	IN STATE_MACHINE_FUNC DefFunc, 
	IN ULONG InitState, 
	IN ULONG Base);
extern VOID (*StateMachinePerformAction)(
	IN	PRTMP_ADAPTER	pAd, 
	IN STATE_MACHINE *S, 
	IN MLME_QUEUE_ELEM *Elem,
	IN ULONG CurrState);
extern VOID (*StateMachineSetAction)(
	IN STATE_MACHINE *S, 
	IN ULONG St, 
	IN ULONG Msg, 
	IN STATE_MACHINE_FUNC Func);
extern VOID (*SupportRate)(
	IN PUCHAR SupRate,
	IN UCHAR SupRateLen,
	IN PUCHAR ExtRate,
	IN UCHAR ExtRateLen,
	OUT PUCHAR *ppRates,
	OUT PUCHAR RatesLen,
	OUT PUCHAR pMaxSupportRate);
extern VOID (*SwitchBetweenWepAndCkip)(
	IN PRTMP_ADAPTER pAd);
extern VOID (*SyncStateMachineInit)(
	IN PRTMP_ADAPTER pAd, 
	IN STATE_MACHINE *Sm, 
	OUT STATE_MACHINE_FUNC Trans[]);
extern void (*SysCfg_Init)();
extern void (*SysCfg_Set_SysMacAddr)(UCHAR *mac);
extern VOID  (*TriEventInit)(IN	PRTMP_ADAPTER pAd);
extern INT (*TriEventTableSetEntry)(
	IN	PRTMP_ADAPTER	pAd, 
	OUT TRIGGER_EVENT_TAB *Tab, 
	IN PUCHAR pBssid, 	
	IN HT_CAPABILITY_IE *pHtCapability,	
	IN UCHAR			HtCapabilityLen,
	IN UCHAR			RegClass,	
	IN UCHAR ChannelNo);
extern VOID (*TRTableInsertEntry)(
	RTMP_ADAPTER 	*pAd,
	UCHAR 			tr_tb_idx, 
	MAC_TABLE_ENTRY *pEntry);
extern VOID (*TRTableInsertMcastEntry)(
	RTMP_ADAPTER 	*pAd,
	UCHAR 			tr_tb_idx, 
	struct wifi_dev *wdev);
extern VOID (*TRTableResetEntry)(
	RTMP_ADAPTER 	*pAd,
	UCHAR 			tr_tb_idx);
extern VOID (*tr_tb_reset_entry)(
		RTMP_ADAPTER 	*pAd, 
		UCHAR 			tr_tb_idx);
extern UCHAR (*TxPktClassification)(
	IN RTMP_ADAPTER *pAd, 
	IN PNDIS_PACKET  pPacket);
extern UINT16 (*tx_rate_to_tmi_rate)(
    IN UINT_8 mode,
    IN UINT_8 mcs,
    IN UINT_8 nss,
    IN BOOL stbc,
    IN UINT_8 preamble);
extern void (*uninitialize_airplay_ie)();
extern void (*uninitialize_ext_ie_val)();
extern VOID (*Update2040CoexistFrameAndNotify)(
		IN	PRTMP_ADAPTER	pAd,
		IN	  UCHAR  Wcid,
		IN	BOOLEAN bAddIntolerantCha);
extern USHORT (*update_associated_mac_entry)(
	IN RTMP_ADAPTER *pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN IE_LISTS *ie_list,
	IN UCHAR MaxSupportedRate);
extern VOID (*UpdateBasicRateBitmap)(
    IN  PRTMP_ADAPTER   pAdapter);
extern VOID (*UpdateBeaconHandler)(
        RTMP_ADAPTER *pAd,
        struct wifi_dev *wdev,
        UCHAR BCN_UPDATE_REASON);
extern VOID (*updateBeaconRoutineCase)(RTMP_ADAPTER *pAd, BOOLEAN UpdateAfterTim);
extern VOID (*UpdateBssScanParm)(
		IN	PRTMP_ADAPTER	pAd,
		IN	OVERLAP_BSS_SCAN_IE APBssScan);
extern void (*update_profile_ssid)(char *ssid);
extern VOID (*Update_Rssi_Sample)(
	IN PRTMP_ADAPTER	pAd,
	IN RSSI_SAMPLE		*pRssi,
	IN PRXWI_STRUC		pRxWI);
extern VOID (*UserCfgInit)(
	IN	PRTMP_ADAPTER pAd);
extern UCHAR (*VLAN_8023_Header_Copy)(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pHeader802_3,
	IN	UINT            HdrLen,
	OUT PUCHAR			pData,
	IN	UCHAR			FromWhichBSSID);
extern INT32 (*wdev_init)(
		RTMP_ADAPTER 	*pAd, 
		struct wifi_dev *wdev,
		enum WDEV_TYPE 	WdevType, 
		PNET_DEV 		IfDev, 
		INT8			func_idx,
		UINT8 			tr_tb_idx,
		VOID 			*func_dev, 
		VOID 			*sys_handle);
extern struct wifi_dev *(*WdevSearchByAddress)(
		RTMP_ADAPTER 	*pAd, 
		UCHAR 			*Address);
extern struct wifi_dev *(*WdevSearchByOmacIdx)(
		RTMP_ADAPTER 	*pAd,
		UINT8 			BssIndex);
extern int8_t (*wifi_fw_inband_early_eject)(uint8_t cmd_id);
extern void (*wireless_send_event)(
		IN struct netif *dev,
		IN UINT     cmd,
		IN union iwreq_data * wrq,
		IN PUCHAR    extra);
extern void (*wlan_802_11_to_802_3_packet)(
	IN  PRTMP_ADAPTER   pAd, 
	IN  RX_BLK          *pRxBlk,
	IN  PUCHAR          pHeader802_3,
	IN  UCHAR           FromWhichBSSID);
extern error_t (*wlan_netif_setup)(struct netif *netif);
extern void        (*rx_dma_done_process)(struct netif *netif, int queue);
extern void        (*RTMPHandleTxRingDmaDoneInterrupt)(struct netif *netif);
extern NDIS_STATUS (*rt2860_init)(struct netif *netif);
extern int         (*rt2860_open)(struct netif *netif);
extern int         (*rt2860_close)(struct netif *net_dev);
extern RT_802_11_PHY_MODE (*wmode_2_cfgmode)(UCHAR wmode);
extern UCHAR *(*wmode_2_str)(UCHAR wmode);
extern BOOLEAN (*wmode_band_equal)(UCHAR smode, UCHAR tmode);
extern VOID (*WowArpOffloadCfg)(IN RTMP_ADAPTER *pAd, UCHAR *IpAddr);
extern VOID (*WowBitmapPatternCfg)(UCHAR *BmMask, UCHAR *BmPattern);
extern VOID (*WowEnable)(IN RTMP_ADAPTER *pAd, BOOLEAN enable);
extern VOID (*WowMagicPktCfg)(UINT_32 Bssid, BOOLEAN enable);
extern VOID (*WowReKeyCfg)(IN RTMP_ADAPTER *pAd, UCHAR *pWowData);
extern VOID (*write_tmac_info_beacon)(RTMP_ADAPTER *pAd, INT apidx, UCHAR *tmac_buf, HTTRANSMIT_SETTING *BeaconTransmit, ULONG frmLen);
extern VOID (*Wtbl2RateTableUpdate)(RTMP_ADAPTER *pAd, UCHAR ucWcid, UINT32 u4Wtbl2D9, UINT32 *Rate);
extern VOID (*WPARetryExec)(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);
extern VOID (*GREKEYPeriodicExec)(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3); 
extern VOID (*CMTimerExec)(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3) ;
extern VOID	(*WPA_APSetGroupRekeyAction)(
	IN  PRTMP_ADAPTER   pAd);
extern void (*RTMPResetAdapter)(void *pAd);
#endif /* __PUBLIC_ROM_API__ */