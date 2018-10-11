#ifndef	__IOT_LIB_H__
#define	__IOT_LIB_H__

//#include "rt_config.h"
//#include "hal_pwr_mgt_rom.h"

#define MLME_TASK_EXEC_INTV_1SEC         1000


typedef struct _PARTIAL_SCAN_CH_LIST{
	UCHAR Channel;
	UCHAR ScanType;
}PARTIAL_SCAN_CH_LIST;
/* Rx-Zero copy in intra-bss forwarding to improve throughput */
extern UCHAR g_TxdPadding;
#define TXD_PADDING					g_TxdPadding

typedef struct _RA_PHY_CFG_T {
    UINT_8  MODE;
    UINT_8  iTxBF;
    UINT_8  eTxBF;
    UINT_8  STBC;
    UINT_8  ShortGI;
    UINT_8  BW;
    UINT_8  ldpc;
    UINT_8  MCS;
    UINT_8  VhtNss;
}RA_PHY_CFG_T;

typedef struct _RA_ENTRY_INFO_T {
    BOOL    fgRaValid;
    UINT_8  ucWcid;
    BOOL    fgAutoTxRateSwitch;

    BOOL    fgDisableCCK;

    BOOL    fgHtCapMcs32;
    BOOL    fgHtCapInfoGF;
    UCHAR   aucHtCapMCSSet[4];
    UCHAR   ucMmpsMode;

    INT_8   AvgRssiSample[4];
    UINT_8  ucCERMSD;

    UCHAR   RateLen;
    UCHAR   ucSupportRateMode;
    UINT_8  ucSupportCCKMCS;
    UINT_8  ucSupportOFDMMCS;
#ifdef DOT11_N_SUPPORT
    UINT_32 u4SupportHTMCS;
#ifdef DOT11_VHT_AC
    UINT_16 u2SupportVHTMCS1SS;
    UINT_16 u2SupportVHTMCS2SS;
    UINT_16 u2SupportVHTMCS3SS;
    UINT_16 u2SupportVHTMCS4SS;
    BOOL    force_op_mode;
    UINT_8  vhtOpModeChWidth;
    UINT_8  vhtOpModeRxNss;
    UINT_8  vhtOpModeRxNssType;
#endif /* DOT11_VHT_AC */
#endif /* DOT11_N_SUPPORT */

    UINT_32 ClientStatusFlags;

    RA_PHY_CFG_T MaxPhyCfg;
    RA_PHY_CFG_T TxPhyCfg;

    BOOL    fgAuthWapiMode;                 /* Security hardware limitation workaround */

#ifdef WIFI_BUILD_RAM
#if (PRODUCT_VERSION == 7615)
    BOOL    fgRaCtrlExec;
    BOOL    fgQuickResponse;
    INT_8   ucRaTimeSlot;
    INT_8   ucRaQuickTimeSlot;
    INT_8   ucRaQuickTimeRatio;
    UINT_32 u4TxCount;
    UINT_16 u2Rate1TxCnt;
    UINT_16 u2Rate1FailCnt;
    UINT_16 u2Rate2OkCnt;
    UINT_16 u2Rate3OkCnt;
#endif
    UINT_32 u4TotalTxCount;
    UINT_32 u4TotalTxFailCount;
#endif /* WIFI_BUILD_RAM */
}RA_ENTRY_INFO_T, *P_RA_ENTRY_INFO_T;


typedef struct _rssi_sample{
	UINT_8 ucLastRssi0;
	UINT_8 ucReserved;
	UINT_16 u2AvgRssi0X8;
}rssi_sample_t;

typedef struct _station_list{
	UINT_32 u4last_tx_rate;
	UINT_32 u4last_rx_rate;
	rssi_sample_t rssi_sample;
	UINT_8 ucaddr[MAC_ADDR_LEN];
	UINT_8 ucPsMode;
	UINT_8 ucBw;
	UINT_8 ucKeepAlive;
}station_list_t;
#define CFG_STA_REC_NUM                             12

#ifndef N9_HAL_PORTING_UNPATCH

typedef enum _ENUM_RX_FILTER_EXT_T {
    RXFILTER_EXT_DROP_NOT_IN_MC_TABLE_BSSID1 = 0,
    RXFILTER_EXT_DROP_NOT_IN_MC_TABLE_BSSID2,
    RXFILTER_EXT_DROP_NOT_IN_MC_TABLE_BSSID3,
    RXFILTER_EXT_DROP_NOT_IN_MC_TABLE_BSSIDX,
    RXFILTER_EXT_SMART_CONNECTION_EN         = 18
} ENUM_RX_FILTER_EXT_T ;

#endif/*N9_HAL_PORTING_UNPATCH*/

typedef struct {    
    UINT8  band_idx;
    UINT8  long_nav;
    UINT8  mix_mode;
    UINT8  gf;
    UINT8  rifs;
    UINT8  bw40;
    UINT8  bw80;
    UINT8  bw160;
    UINT16 erp_mask;
} IOT_PROTECT_CTRL_T;
#ifndef IOT_PROCESS_PORTING
#if 0
typedef enum _ENUM_TSF_T {
    ENUM_LOCAL_TSF_0,
    ENUM_LOCAL_TSF_1,
    ENUM_LOCAL_TSF_2,
    ENUM_LOCAL_TSF_3,
    ENUM_LOCAL_TSF_NUM
} ENUM_LOCAL_TSF_T, *P_ENUM_LOCAL_TSF_T;


typedef enum {
    ENUM_PM_BSSID_0,
    ENUM_PM_BSSID_1,
    ENUM_PM_BSSID_2,
    ENUM_PM_BSSID_3,
    ENUM_PM_BSSID_NUM
} ENUM_PM_BSS_ID_T;

typedef enum
{
    HAL_TSF_HW_UPDATE_BY_TICK_AND_RECEIVED_FRAME,
    HAL_TSF_HW_UPDATE_BY_TICK_ONLY,
    HAL_TSF_HW_UPDATE_BY_RECEIVED_FRAME_ONLY,
    HAL_TSF_HW_UPDATE_BY_TICK_AND_RECEIVED_FRAME_AD_HOC
} HAL_TSF_HW_UPDATE_MODE;
#endif
#endif
/* MAC TX Queue Operation Parameters */
#ifndef TXM_FLUSH_QUEUE_MODE_BLOCKING
#define TXM_FLUSH_QUEUE_MODE_BLOCKING                       TRUE
#endif
#ifndef TXM_FLUSH_QUEUE_MODE_NONBLOCKING
#define TXM_FLUSH_QUEUE_MODE_NONBLOCKING                    FALSE
#endif

#ifndef TXM_FLUSH_QUEUE_TIMEOUT_LEN
#define TXM_FLUSH_QUEUE_TIMEOUT_LEN                         20000   /* in us */
#endif

#ifndef N9_HAL_PORTING_UNPATCH

typedef enum _ENUM_HW_OP_MODE_T {
    HW_OP_MODE_STA = 0,
    HW_OP_MODE_AP,
    HW_OP_MODE_ADHOC,
    HW_OP_MODE_NUM
} ENUM_HW_OP_MODE_T;
#endif
#ifdef N9_HAL_PORTING_UNPATCH
#define BSS_DEFAULT_PRETBTT_INTERVAL_BEACON_USEC        5000 /* This is also Wakeup Guard Time value */
#define SET_PROTECT(x)  (1 << (x))
#define fRTMP_ADAPTER_RDG_ACTIVE					0x00100000

typedef enum _ENUM_RECAL_TYPE
{
    RECAL_DEFAULT,          /* Used for Normal Tx and Rx */
    RECAL_NON,
    RECAL_DELAY_TONE_CAL,
    RECAL_SIMPLY_CAL,       /* Used for Scan only */

#if (CFG_MCC_SUPPORT == 1)
    RECAL_MCC_CAL,          /* Used for MCC only */
#endif

    RECAL_TYPE_NUM
} ENUM_RECAL_TYPE, *P_ENUM_RECAL_TYPE;
enum _protection {
    /* 11n */
    NO_PROTECTION,
    NON_MEMBER_PROTECT,
    HT20_PROTECT,
    NON_HT_MIXMODE_PROTECT,
    _NOT_DEFINE_HT_PROTECT,
    /* b/g */
    ERP,
    /* vendor */
    LONG_NAV_PROTECT,
    RDG_PROTECT=LONG_NAV_PROTECT,
    GREEN_FIELD_PROTECT,
    RIFS_PROTECT,
    RDG,
    FORCE_RTS_PROTECT,
    _NOT_DEFINE_VENDOR_PROTECT,
    _END_PROTECT
};
#define EXT_CMD_CBW_20MHZ                           0x00
#define EXT_CMD_CBW_40MHZ                           0x01
#define EXT_CMD_CBW_80MHZ                           0x02
#define EXT_CMD_CBW_160MHZ                          0x03
#define EXT_CMD_CBW_10MHZ                           0x04
#define EXT_CMD_CBW_5MHZ                            0x05
#define EXT_CMD_CBW_8080MHZ                         0x06

#endif

typedef struct _WPS_IE_VAL
{
	UINT_8 *wps_becon_ie;
	UINT_8 wps_beacon_ie_len;

	UINT_8 *wps_probereq_ie;
	UINT_8 wps_probereq_ie_len;
	UINT_8 *wps_proberesp_ie;
	UINT_8 wps_proberesp_ie_len;

	UINT_8 *wps_assocreq_ie;
	UINT_8 wps_assocreq_ie_len;
	UINT_8 *wps_assocresp_ie;
	UINT_8 wps_assocresp_ie_len;

	UINT_8  port;
} WPS_IE, *PWPS_IE;

#if (MT7687_BORA00006617 == 1)
extern UINT_16 glbl_deauth_reason_code;
extern UINT_16 glbl_assoc_reason_code;
extern UINT_16 glbl_auth_reason_code;
extern UINT_16 glbl_disassoc_reason_code;
#endif

#if (MT7687_BORA00006617 == 1)
#define EVENT_ID_IOT_MGMT_WITH_REASON_CODE 0x52
#endif


typedef struct _CF_IE_VAL
{
	UINT_8 *cf_probereq_ie;
	UINT_8 cf_probereq_ie_len;
	UINT_8 *cf_proberesp_ie;
	UINT_8 cf_proberesp_ie_len;

	UINT_8  port;
} CF_IE, *PCF_IE;

struct ieee80211_channel_sw_ie {
	UINT8 mode;
	UINT8 new_ch_num;
	UINT8 count;
} PACKED;

struct ieee80211_msrment_ie {
	UINT8 token;
	UINT8 mode;
	UINT8 type;
	UINT8 request[0];
} PACKED;
#define WLAN_SA_QUERY_TR_ID_LEN 2

struct ieee80211_mgmt_iot {
	UINT16 frame_control;
	UINT16 duration;
	UINT8 da[6];
	UINT8 sa[6];
	UINT8 bssid[6];
	UINT16 seq_ctrl;
	union {
		struct {
			UINT16 auth_alg;
			UINT16 auth_transaction;
			UINT16 status_code;
			/* possibly followed by Challenge text */
			u8 variable[0];
		} PACKED auth;
		struct {
			UINT16 reason_code;
		}PACKED deauth;
		struct {
			UINT16 capab_info;
			UINT16 listen_interval;
			/* followed by SSID and Supported rates */
			UINT8 variable[0];
		} PACKED assoc_req;
		struct {
			UINT16 capab_info;
			UINT16 status_code;
			UINT16 aid;
			/* followed by Supported rates */
			UINT8 variable[0];
		} PACKED assoc_resp, reassoc_resp;
		struct {
			UINT16 capab_info;
			UINT16 listen_interval;
			UINT8 current_ap[6];
			/* followed by SSID and Supported rates */
			UINT8 variable[0];
		}PACKED reassoc_req;
		struct {
			UINT16 reason_code;
		} PACKED disassoc;
		struct {
			ULONG timestamp;
			UINT16 beacon_int;
			UINT16 capab_info;
			/* followed by some of SSID, Supported rates,
			 * FH Params, DS Params, CF Params, IBSS Params, TIM */
			UINT8 variable[0];
		}PACKED beacon;
		struct {
			/* only variable items: SSID, Supported rates */
			UINT8 variable[0];
		} PACKED probe_req;
		struct {
			ULONG timestamp;
			UINT16 beacon_int;
			UINT16 capab_info;
			/* followed by some of SSID, Supported rates,
			 * FH Params, DS Params, CF Params, IBSS Params */
			u8 variable[0];
		} PACKED probe_resp;
		struct {
			UINT8 category;
			union {
				struct {
					UINT8 action_code;
					UINT8 dialog_token;
					UINT8 status_code;
					UINT8 variable[0];
				} PACKED wme_action;
				struct{
					UINT8 action_code;
					UINT8 element_id;
					UINT8 length;
					struct ieee80211_channel_sw_ie sw_elem;
				} PACKED chan_switch;
				struct{
					UINT8 action_code;
					UINT8 dialog_token;
					UINT8 element_id;
					UINT8 length;
					struct ieee80211_msrment_ie msr_elem;
				} PACKED measurement;
				struct{
					UINT8 action_code;
					UINT8 dialog_token;
					UINT16 capab;
					UINT16 timeout;
					UINT16 start_seq_num;
				} PACKED addba_req;
				struct{
					UINT8 action_code;
					UINT8 dialog_token;
					UINT16 status;
					UINT16 capab;
					UINT16 timeout;
				} PACKED addba_resp;
				struct{
					UINT8 action_code;
					UINT16 params;
					UINT16 reason_code;
				}PACKED delba;
				struct {
					UINT8 action_code;
					UINT8 variable[0];
				} PACKED self_prot;
				struct{
					UINT8 action_code;
					UINT8 variable[0];
				} PACKED mesh_action;
				struct {
					UINT8 action;
					UINT8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
				} PACKED sa_query;
				struct {
					UINT8 action;
					UINT8 smps_control;
				} PACKED ht_smps;
				struct {
					UINT8 action_code;
					UINT8 dialog_token;
					UINT16 capability;
					UINT8 variable[0];
				} PACKED tdls_discover_resp;
			} u;
		} PACKED action;
	} u;
} PACKED;

typedef enum _SCAN_MODE
{
	NORMAL_SCAN = 0,
	FORCE_PASSIVE_MODE = 1,
	FORCE_ACTIVE_MODE = 2,
}SCAN_MODE;

typedef struct _CMD_IOT_START_SCAN_S {
	
	UCHAR partial;		/* 0: Full scan, 1: Partial scan */
	UCHAR scan_mode;	/* 0: Active scan(Passive scan in regulatory channels) */
						/* 1: Force Passive scan in all channel */
						/* 2: Force Active scan in all channel */
	UCHAR partial_bcn_cnt;
	UCHAR periodic_bcn_cnt;		/* 0x00: SCAN for once (default) */
								/* 0xFF: continous SCAN */
								/* x: Re-SCAN after x Beacon interval*/
	UCHAR enable_bssid;			/* 0: disable 1: enable */
	UCHAR BSSID[MAC_ADDR_LEN];	/*BSSID in STA mode OR BSSID[0-1] used Channel Stay time in SoftAP mode for SmartConnection procedure */
	UCHAR enable_ssid; 			/* bit7 0:disable, 1:enable, bit[6]: 1:SmartConnet Scan(enable Rx raw filter) and bit[5:0] is length of SSID */
	UCHAR SSID[32];
	ch_list_t channel_list;
	
}CMD_IOT_START_SCAN_T;

#define REGION_31_BG_BAND                 31	/* 5-13 */
#define REGION_32_BG_BAND                 32	/* 1 - 13 */
#define REGION_33_BG_BAND                 33	/* 1 - 14 */

#define REGION_22_A_BAND 				22
#define REGION_30_A_BAND 				30
#define REGION_37_A_BAND 				37


extern sys_cfg_t *IoTSysCfg;
#ifdef APCLI_SUPPORT
#define PORT_APCLI   0
#endif
#define PORT_AP    1

#define MAX_WEP_KEY_SIZE_CM4 32

extern void (*SysCfg_Init)();
extern void (*SysCfg_Set_SysMacAddr)(UCHAR *mac);
void SysCfg_Set_MacSync(BOOLEAN status);
VOID dump_IoT_profile();
extern VOID (*Iot_init_profile)(RTMP_ADAPTER *pAd);
#ifdef IOT_PROCESS_PORTING
VOID PseFreeFrameId(UINT_16 u2Fid);
void FreeRxPacket(int port,int queue);
#endif/*IOT_PROCESS_PORTING*/


/* Define MAC TX control save and restore macros */

#define MAC_SAVE_AND_TXRX_DISABLE(_scr) \
    do { \
        (_scr) = HAL_REG_32(ARB_SCR); \
        HAL_REG_32(ARB_SCR) |= (SCR_TX_DISABLE | SCR_RX_DISABLE); \
    } while (0)

#define MAC_SAVE_AND_TX_DISABLE(_scr) \
    do { \
        (_scr) = HAL_REG_32(ARB_SCR); \
        HAL_REG_32(ARB_SCR) |= SCR_TX_DISABLE; \
    } while (0)

#define MAC_TXRX_RESTORE(_scr) \
    do { \
        HAL_REG_32(ARB_SCR) = \
            (HAL_REG_32(ARB_SCR) & ~(SCR_TX_DISABLE | SCR_RX_DISABLE)) | \
            ((_scr) & (SCR_TX_DISABLE | SCR_RX_DISABLE)); \
    } while (0)

#endif

