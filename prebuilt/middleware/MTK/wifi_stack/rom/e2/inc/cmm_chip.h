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
	cmm_chip.h

	Abstract:
	Ralink Wireless Chip HW related definition & structures

	Revision History:
	Who			When		  What
	--------	----------	  ----------------------------------------------
*/


#ifndef __CMM_CHIP_H__
#define __CMM_CHIP_H__

#ifndef MT_MAC
#define MT_MAC
#endif

struct _EXT_CMD_EFUSE_BUFFER_MODE_T;
struct _RTMP_ADAPTER;
struct _MAC_TABLE_ENTRY;
struct _CIPHER_KEY;
struct _MT_TX_COUNTER;
struct _EDCA_PARM;
struct _RTMP_REG_PAIR;
//struct _BANK_RF_REG_PAIR;
//struct _R_M_W_REG;
//struct _RF_R_M_W_REG;
//struct _CR_REG;
struct _REG_PAIR;
//struct MT_TX_PWR_CAP;
struct _EXT_CMD_CHAN_SWITCH_T;

struct _RSSI_SAMPLE;



enum ASIC_CAP {
    fASIC_CAP_RX_SC = 0x1,
    fASIC_CAP_CSO = 0x2,
    fASIC_CAP_TSO = 0x4,
    fASIC_CAP_MCS_LUT = 0x8,

    fASIC_CAP_PMF_ENC = 0x10,
};

enum PHY_CAP {
    fPHY_CAP_24G = 0x1,
    fPHY_CAP_5G = 0x2,

    fPHY_CAP_HT = 0x10,
    fPHY_CAP_VHT = 0x20,

    fPHY_CAP_TXBF = 0x100,
    fPHY_CAP_LDPC = 0x200,
};

enum HIF_TYPE {
    HIF_RTMP = 0x0,
    HIF_RLT = 0x1,
    HIF_MT = 0x2,
    HIF_MAX = HIF_MT,
};

enum MAC_TYPE {
    MAC_RTMP = 0x0,
    MAC_MT = 0x1,
};

enum RF_TYPE {
    RF_RT,
    RF_RLT,
    RF_MT76x2,
    RF_MT,
};

enum BBP_TYPE {
    BBP_RTMP = 0x0,
    BBP_RLT = 0x1,
    BBP_MT = 0x2,
};


enum EFUSE_TYPE {
	EFUSE_MT,
	EFUSE_MAX,
} ;

#define MBSSID_MODE0 	0
#define MBSSID_MODE1 	1	/* Enhance NEW MBSSID MODE mapping to mode 0 */



typedef struct _RTMP_CHIP_OP {
	int (*sys_onoff)(struct _RTMP_ADAPTER *pAd, BOOLEAN on, BOOLEAN reser);

	/*  Calibration access related callback functions */
	int (*eeinit)(struct _RTMP_ADAPTER *pAd);
	BOOLEAN (*eeread)(struct _RTMP_ADAPTER *pAd, UINT16 offset, UINT16 *pValue);
	int (*eewrite)(struct _RTMP_ADAPTER *pAd, USHORT offset, USHORT value);

	/* ITxBf calibration */
	int (*fITxBfDividerCalibration)(struct _RTMP_ADAPTER *pAd, int calFunction, int calMethod, UCHAR *divPhase);
	void (*fITxBfLNAPhaseCompensate)(struct _RTMP_ADAPTER *pAd);
	int (*fITxBfCal)(struct _RTMP_ADAPTER *pAd, RTMP_STRING *arg);
	int (*fITxBfLNACalibration)(struct _RTMP_ADAPTER *pAd, int calFunction, int calMethod, BOOLEAN gBand);

	/* MCU related callback functions */
	int (*load_rom_patch)(struct _RTMP_ADAPTER *ad);
	int (*erase_rom_patch)(struct _RTMP_ADAPTER *ad);
	int (*loadFirmware)(struct _RTMP_ADAPTER *pAd);
	int (*eraseFirmware)(struct _RTMP_ADAPTER *pAd);
	int (*sendCommandToMcu)(struct _RTMP_ADAPTER *pAd, UCHAR cmd, UCHAR token, UCHAR arg0, UCHAR arg1, BOOLEAN FlgIsNeedLocked);	/* int (*sendCommandToMcu)(RTMP_ADAPTER *pAd, UCHAR cmd, UCHAR token, UCHAR arg0, UCHAR arg1); */
#ifdef CONFIG_ANDES_SUPPORT
	int (*sendCommandToAndesMcu)(struct _RTMP_ADAPTER *pAd, UCHAR QueIdx, UCHAR cmd, UCHAR *pData, USHORT DataLen, BOOLEAN FlgIsNeedLocked);
#endif

	void (*AsicRfInit)(struct _RTMP_ADAPTER *pAd);
	void (*AsicBbpInit)(struct _RTMP_ADAPTER *pAd);
	void (*AsicMacInit)(struct _RTMP_ADAPTER *pAd);

	void (*AsicRfTurnOn)(struct _RTMP_ADAPTER *pAd);
	void (*AsicRfTurnOff)(struct _RTMP_ADAPTER *pAd);
	void (*AsicReverseRfFromSleepMode)(struct _RTMP_ADAPTER *pAd, BOOLEAN FlgIsInitState);
	void (*AsicHaltAction)(struct _RTMP_ADAPTER *pAd);

	/* Power save */
	void (*EnableAPMIMOPS)(struct _RTMP_ADAPTER *pAd, IN BOOLEAN ReduceCorePower);
	void (*DisableAPMIMOPS)(struct _RTMP_ADAPTER *pAd);
	INT (*PwrSavingOP)(struct _RTMP_ADAPTER *pAd, UINT32 PwrOP, UINT32 PwrLevel,
							UINT32 ListenInterval, UINT32 PreTBTTLeadTime,
							UINT8 TIMByteOffset, UINT8 TIMBytePattern);

	/* Chip tuning */
	VOID (*RxSensitivityTuning)(IN struct _RTMP_ADAPTER *pAd);

	/* MAC */
	VOID (*BeaconUpdate)(struct _RTMP_ADAPTER *pAd, USHORT Offset, UINT32 Value, UINT8 Unit);

	/* BBP adjust */
	VOID (*ChipBBPAdjust)(IN struct _RTMP_ADAPTER *pAd);

	/* AGC */
	VOID (*ChipAGCInit)(struct _RTMP_ADAPTER *pAd, UCHAR bw);
	UCHAR (*ChipAGCAdjust)(struct _RTMP_ADAPTER *pAd, CHAR Rssi, UCHAR OrigR66Value);

	/* Channel */
#ifdef COMPOS_WIN
	VOID (*ChipSwitchChannel)(struct _RTMP_ADAPTER *pAd, struct _EXT_CMD_CHAN_SWITCH_T CmdChSwitch);
#else
	VOID (*ChipSwitchChannel)(struct _RTMP_ADAPTER *pAd, UCHAR ch, BOOLEAN bScan);
#endif

	/* EDCCA */
	VOID (*ChipSetEDCCA)(struct _RTMP_ADAPTER *pAd, BOOLEAN bOn);

	/* IQ Calibration */
	VOID (*ChipIQCalibration)(struct _RTMP_ADAPTER *pAd, UCHAR Channel);

	/* TX ALC */
	UINT32 (*TSSIRatio)(INT32 delta_power);
	VOID (*InitDesiredTSSITable)(IN struct _RTMP_ADAPTER *pAd);
	int (*ATETssiCalibration)(struct _RTMP_ADAPTER *pAd, RTMP_STRING *arg);
	int (*ATETssiCalibrationExtend)(struct _RTMP_ADAPTER *pAd, RTMP_STRING *arg);
	int (*ATEReadExternalTSSI)(struct _RTMP_ADAPTER *pAd, RTMP_STRING *arg);

	VOID (*AsicTxAlcGetAutoAgcOffset)(
				IN struct _RTMP_ADAPTER	*pAd,
				IN PCHAR				pDeltaPwr,
				IN PCHAR				pTotalDeltaPwr,
				IN PCHAR				pAgcCompensate,
				IN PCHAR 				pDeltaPowerByBbpR1);

	VOID (*AsicGetTxPowerOffset)(struct _RTMP_ADAPTER *pAd, ULONG *TxPwr);
	VOID (*AsicExtraPowerOverMAC)(struct _RTMP_ADAPTER *pAd);

	VOID (*AsicAdjustTxPower)(struct _RTMP_ADAPTER *pAd);

	/* Antenna */
	VOID (*AsicAntennaDefaultReset)(struct _RTMP_ADAPTER *pAd, union _EEPROM_ANTENNA_STRUC *pAntenna);
	VOID (*SetRxAnt)(struct _RTMP_ADAPTER *pAd, UCHAR Ant);

	/* EEPROM */
	VOID (*NICInitAsicFromEEPROM)(IN struct _RTMP_ADAPTER *pAd);

	/* Temperature Compensation */
	VOID (*InitTemperCompensation)(IN struct _RTMP_ADAPTER *pAd);
	VOID (*TemperCompensation)(IN struct _RTMP_ADAPTER *pAd);

	/* high power tuning */
	VOID (*HighPowerTuning)(struct _RTMP_ADAPTER *pAd, struct _RSSI_SAMPLE *pRssi);

	/* Others */
	VOID (*NetDevNickNameInit)(IN struct _RTMP_ADAPTER *pAd);
#ifdef CAL_FREE_IC_SUPPORT
	BOOLEAN (*is_cal_free_ic)(IN struct _RTMP_ADAPTER *pAd);
	VOID (*cal_free_data_get)(IN struct _RTMP_ADAPTER *pAd);
#endif /* CAL_FREE_IC_SUPPORT */

	/* The chip specific function list */

	VOID (*AsicResetBbpAgent)(IN struct _RTMP_ADAPTER *pAd);

	VOID (*CckMrcStatusCtrl)(struct _RTMP_ADAPTER *pAd);
	VOID (*RadarGLRTCompensate)(struct _RTMP_ADAPTER *pAd);
	VOID (*SecondCCADetection)(struct _RTMP_ADAPTER *pAd);

	/* MCU */
#if 0
	void (*MCUCtrlInit)(struct _RTMP_ADAPTER *ad);
	void (*MCUCtrlExit)(struct _RTMP_ADAPTER *ad);
	VOID (*FwInit)(struct _RTMP_ADAPTER *pAd);
	VOID (*FwExit)(struct _RTMP_ADAPTER *pAd);
	int (*BurstWrite)(struct _RTMP_ADAPTER *ad, UINT32 Offset, UINT32 *Data, UINT32 Cnt);
	int (*BurstRead)(struct _RTMP_ADAPTER *ad, UINT32 Offset, UINT32 Cnt, UINT32 *Data);
	int (*RandomRead)(struct _RTMP_ADAPTER *ad,struct  _RTMP_REG_PAIR *RegPair, UINT32 Num);
	int (*RFRandomRead)(struct _RTMP_ADAPTER *ad, struct _BANK_RF_REG_PAIR *RegPair, UINT32 Num);
	int (*ReadModifyWrite)(struct _RTMP_ADAPTER *ad, struct _R_M_W_REG *RegPair, UINT32 Num);
	int (*RFReadModifyWrite)(struct _RTMP_ADAPTER *ad, struct _RF_R_M_W_REG *RegPair, UINT32 Num);
	int (*RandomWrite)(struct _RTMP_ADAPTER *ad, struct _RTMP_REG_PAIR *RegPair, UINT32 Num);
	int (*RFRandomWrite)(struct _RTMP_ADAPTER *ad, struct _BANK_RF_REG_PAIR *RegPair, UINT32 Num);
	int (*sc_random_write)(struct _RTMP_ADAPTER *ad, struct _CR_REG *table, UINT32 num, UINT32 flags);
	int (*sc_rf_random_write)(struct _RTMP_ADAPTER *ad, struct _BANK_RF_CR_REG *table, UINT32 num, UINT32 flags);
	int (*DisableTxRx)(struct _RTMP_ADAPTER *ad, UCHAR Level);
	void (*AsicRadioOn)(struct _RTMP_ADAPTER *ad, UCHAR Stage);
	void (*AsicRadioOff)(struct _RTMP_ADAPTER *ad, UINT8 Stage);
	void (*rx_event_handler)(struct _RTMP_ADAPTER *ad, UCHAR *data);
#endif

#if (defined(WOW_SUPPORT) && defined(RTMP_MAC_USB)) || defined(NEW_WOW_SUPPORT)
	VOID (*AsicWOWEnable)(struct _RTMP_ADAPTER *ad);
	VOID (*AsicWOWDisable)(struct _RTMP_ADAPTER *ad);
#endif /* (defined(WOW_SUPPORT) && defined(RTMP_MAC_USB)) || defined(NEW_WOW_SUPPORT) */

	void (*usb_cfg_read)(struct _RTMP_ADAPTER *ad, UINT32 *value);
	void (*usb_cfg_write)(struct _RTMP_ADAPTER *ad, UINT32 value);
	void (*show_pwr_info)(struct _RTMP_ADAPTER *ad);
	void (*cal_test)(struct _RTMP_ADAPTER *ad, UINT32 type);
	//void (*bufferModeEfuseFill)(struct _RTMP_ADAPTER *ad, struct _EXT_CMD_EFUSE_BUFFER_MODE_T *pCmd);
//	INT32 (*MtCmdTx)(struct _RTMP_ADAPTER *pAd,struct cmd_msg *msg);
}RTMP_CHIP_OP;
enum MCU_TYPE {
	SWMCU = (1<<0),
	M8051 = (1<<1),
	ANDES = (1<<2),
    CR4 = (1<<3),
};


typedef struct MT_TX_PWR_CAP {
#define INTERNAL_PA 0
#define EXTERNAL_PA 1
	UINT8 pa_type;
#define TSSI_TRIGGER_STAGE 0
#define TSSI_COMP_STAGE 1
#define TSSI_CAL_STAGE 2
	UINT8 tssi_stage;
#define TSSI_0_SLOPE_G_BAND_DEFAULT_VALUE 0x84
#define TSSI_1_SLOPE_G_BAND_DEFAULT_VALUE 0x83
	UINT8 tssi_0_slope_g_band;
	UINT8 tssi_1_slope_g_band;
#define TSSI_0_OFFSET_G_BAND_DEFAULT_VALUE 0x0A
#define TSSI_1_OFFSET_G_BAND_DEFAULT_VALUE 0x0B
	UINT8 tssi_0_offset_g_band;
	UINT8 tssi_1_offset_g_band;
#define TX_TARGET_PWR_DEFAULT_VALUE 0x26
	CHAR tx_0_target_pwr_g_band;
	CHAR tx_1_target_pwr_g_band;
	CHAR tx_0_chl_pwr_delta_g_band[3];
	CHAR tx_1_chl_pwr_delta_g_band[3];
	CHAR delta_tx_pwr_bw40_g_band;

	CHAR tx_pwr_cck_1_2;
	CHAR tx_pwr_cck_5_11;
	CHAR tx_pwr_g_band_ofdm_6_9;
	CHAR tx_pwr_g_band_ofdm_12_18;
	CHAR tx_pwr_g_band_ofdm_24_36;
	CHAR tx_pwr_g_band_ofdm_48;
	CHAR tx_pwr_g_band_ofdm_54;
	CHAR tx_pwr_ht_bpsk_mcs_0_8;
	CHAR tx_pwr_ht_bpsk_mcs_32;
	CHAR tx_pwr_ht_qpsk_mcs_1_2_9_10;
	CHAR tx_pwr_ht_16qam_mcs_3_4_11_12;
	CHAR tx_pwr_ht_64qam_mcs_5_13;
	CHAR tx_pwr_ht_64qam_mcs_6_14;
	CHAR tx_pwr_ht_64qam_mcs_7_15;
}MT_TX_PWR_CAP_T;



typedef struct _RTMP_CHIP_CAP {
	/* ------------------------ packet --------------------- */
	UINT8 TXWISize;	// TxWI or LMAC TxD max size
	UINT8 RXWISize; // RxWI or LMAC RxD max size
	UINT8 tx_hw_hdr_len;	// Tx Hw meta info size which including all hw info fields
	UINT8 rx_hw_hdr_len;	// Rx Hw meta info size

	enum ASIC_CAP asic_caps;
	enum PHY_CAP phy_caps;
	enum HIF_TYPE hif_type;
	enum MAC_TYPE mac_type;
	enum BBP_TYPE bbp_type;
	enum MCU_TYPE MCUType;
	enum RF_TYPE rf_type;
	enum EFUSE_TYPE efuse_type;

	/* register */
	struct _REG_PAIR *pRFRegTable;
	struct _REG_PAIR *pBBPRegTable;
	UCHAR bbpRegTbSize;

	UINT32 MaxNumOfRfId;
	UINT32 MaxNumOfBbpId;

#define RF_REG_WT_METHOD_NONE			0
#define RF_REG_WT_METHOD_STEP_ON		1
	UCHAR RfReg17WtMethod;

	/* beacon */
	BOOLEAN FlgIsSupSpecBcnBuf;	/* SPECIFIC_BCN_BUF_SUPPORT */
	UINT8 BcnMaxNum;	/* software use */
	UINT8 BcnMaxHwNum;	/* hardware limitation */
	UINT8 WcidHwRsvNum;	/* hardware available WCID number */
	UINT32 WtblHwNum;		/* hardware WTBL number */
	UINT32 WtblPseAddr;		/* */
	UINT16 BcnMaxHwSize;	/* hardware maximum beacon size */
	UINT16 BcnBase[HW_BEACON_MAX_NUM];	/* hardware beacon base address */

	/* function */
	/* use UINT8, not bit-or to speed up driver */
	BOOLEAN FlgIsHwWapiSup;

	/* VCO calibration mode */
	UINT8 VcoPeriod; /* default 10s */
#define VCO_CAL_DISABLE		0	/* not support */
#define VCO_CAL_MODE_1		1	/* toggle RF7[0] */
#define VCO_CAL_MODE_2		2	/* toggle RF3[7] */
#define VCO_CAL_MODE_3		3	/* toggle RF4[7] or RF5[7] */
	UINT8	FlgIsVcoReCalMode;

	BOOLEAN FlgIsHwAntennaDiversitySup;
	BOOLEAN Flg7662ChipCap;
#ifdef FIFO_EXT_SUPPORT
	BOOLEAN FlgHwFifoExtCap;
#endif /* FIFO_EXT_SUPPORT */

	UCHAR ba_max_cnt;


	BOOLEAN temp_tx_alc_enable;
	INT32 temp_25_ref; /* a quantification value of temperature, but not J */
	INT32 current_temp; 
#ifdef RTMP_TEMPERATURE_TX_ALC
	UINT32 high_temp_slope_a_band; /* slope with unit (J /dB) */
	UINT32 low_temp_slope_a_band; /* slope with unit (J /dB) */
	UINT32 high_temp_slope_g_band; /* slope with unit (J /dB) */
	UINT32 low_temp_slope_g_band; /* slope with unit (J /dB) */
	INT32 tc_upper_bound_a_band; /* unit dB */
	INT32 tc_lower_bound_a_band; /* unit dB */
	INT32 tc_upper_bound_g_band; /* unit dB */
	INT32 tc_lower_bound_g_band; /* unit dB */	
#endif /* RTMP_TEMPERATURE_TX_ALC */



	/* ---------------------------- signal ---------------------------------- */
#define SNR_FORMULA1		0	/* ((0xeb     - pAd->StaCfg.wdev.LastSNR0) * 3) / 16; */
#define SNR_FORMULA2		1	/* (pAd->StaCfg.wdev.LastSNR0 * 3 + 8) >> 4; */
#define SNR_FORMULA3		2	/* (pAd->StaCfg.wdev.LastSNR0) * 3) / 16; */
#define SNR_FORMULA4		3	/* for MT7603 */
	UINT8 SnrFormula;

	UINT8 max_nss;			/* maximum Nss, 3 for 3883 or 3593 */
	UINT8 max_vht_mcs;		/* Maximum Vht MCS */


	BOOLEAN bTempCompTxALC;
	BOOLEAN rx_temp_comp;

	BOOLEAN bLimitPowerRange; /* TSSI compensation range limit */




	/* ---------------------------- others ---------------------------------- */
#ifdef RTMP_EFUSE_SUPPORT
	UINT16 EFUSE_USAGE_MAP_START;
	UINT16 EFUSE_USAGE_MAP_END;
	UINT8 EFUSE_USAGE_MAP_SIZE;
	UINT8 EFUSE_RESERVED_SIZE;
#endif /* RTMP_EFUSE_SUPPORT */

	UCHAR *EEPROM_DEFAULT_BIN;
	UINT16 EEPROM_DEFAULT_BIN_SIZE;

#ifdef RTMP_FLASH_SUPPORT
	BOOLEAN ee_inited;
#endif /* RTMP_FLASH_SUPPORT */
#ifdef DFS_SUPPORT
	UINT8 DfsEngineNum;
#endif /* DFS_SUPPORT */

	/*
		Define the burst size of WPDMA of PCI
		0 : 4 DWORD (16bytes)
		1 : 8 DWORD (32 bytes)
		2 : 16 DWORD (64 bytes)
		3 : 32 DWORD (128 bytes)
	*/
	UINT8 WPDMABurstSIZE;

	/*
 	 * 0: MBSSID_MODE0
 	 * (The multiple MAC_ADDR/BSSID are distinguished by [bit2:bit0] of byte5)
 	 * 1: MBSSID_MODE1
 	 * (The multiple MAC_ADDR/BSSID are distinguished by [bit4:bit2] of byte0)
 	 */
	UINT8 MBSSIDMode;





#ifdef CONFIG_ANDES_SUPPORT
	UINT32 WlanMemmapOffset;
	UINT32 InbandPacketMaxLen; /* must be 48 multible */
	UINT8 CmdRspRxRing;
	BOOLEAN IsComboChip;
	BOOLEAN need_load_fw;
	BOOLEAN need_load_rom_patch;
	UINT8 ram_code_protect;
	UINT8 rom_code_protect;
	UINT8 load_iv;
	UINT32 ilm_offset;
	UINT32 dlm_offset;
	UINT32 rom_patch_offset;
	UINT8 DownLoadType;
#endif

	UINT8 cmd_header_len;
	UINT8 cmd_padding_len;



#ifdef CONFIG_SWITCH_CHANNEL_OFFLOAD
	UINT16 ChannelParamsSize;
	UCHAR *ChannelParam;
	INT XtalType;
#endif

	UCHAR load_code_method;
	UCHAR *FWImageName;
	UCHAR *fw_header_image;
	UCHAR *fw_bin_file_name;
	UCHAR *rom_patch;
	UCHAR *rom_patch_header_image;
	UCHAR *rom_patch_bin_file_name;
	UINT32 rom_patch_len;
	UINT32 fw_len;
	UCHAR *MACRegisterVer;
	UCHAR *BBPRegisterVer;
	UCHAR *RFRegisterVer;


	BOOLEAN tssi_enable;
	BOOLEAN ed_cca_enable;
	
#ifdef MT_MAC
	struct MT_TX_PWR_CAP MTTxPwrCap;
	UCHAR TmrEnable;
	UINT8 OmacNums;
	UINT8 BssNums;
	UINT8 MBSSStartIdx;
    UINT8 MaxRepeaterNum;
	UINT8 ExtMbssOmacStartIdx;
	UINT8 RepeaterStartIdx;
#endif



	UINT8 TxBAWinSize;
	UINT8 RxBAWinSize;
	UINT8 AMPDUFactor;
#ifdef RACTRL_FW_OFFLOAD_SUPPORT
    BOOLEAN fgRateAdaptFWOffload;
#endif /* RACTRL_FW_OFFLOAD_SUPPORT */

    UINT32  CurrentTxOP;
	UINT8 MaxApCliNum;
	UINT8 MaxSTANum;
	UINT16 MaxReorderMpduNum;
}RTMP_CHIP_CAP;

#endif


