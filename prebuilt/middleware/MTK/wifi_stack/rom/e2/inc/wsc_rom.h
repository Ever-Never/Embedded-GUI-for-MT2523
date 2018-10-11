/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	aironet.h

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Name		Date			Modification logs
	Paul Lin	06-08-08		Initial
*/

#ifndef __WSC_ROM_H__
#define __WSC_ROM_H__

#include    "rt_config.h"

/* UUID related definition */
#define UUID_LEN_HEX 16  /* 128 bits => 16 bytes */
#define UUID_LEN_STR 37  /* hex to string, plus 4 dash, plus 1 '\0' */
#define UUID_VERSION 1   /* We currently just support version 1 */


#define WSC_DISABLE		    0x0
#define WSC_ENROLLEE     0x1
#define WSC_PROXY        0x2
#define WSC_REGISTRAR    0x4
#define WSC_ENROLLEE_PROXY           (WSC_ENROLLEE | WSC_PROXY)
#define WSC_ENROLLEE_REGISTRAR       (WSC_ENROLLEE | WSC_REGISTRAR)
#define WSC_PROXY_REGISTRAR          (WSC_PROXY | WSC_REGISTRAR)
#define WSC_ENROLLEE_PROXY_REGISTRAR (WSC_ENROLLEE | WSC_PROXY | WSC_REGISTRAR)

/*Messages for the WSC state machine, */
#define	WSC_MACHINE_BASE		34
#define	WSC_EAPOL_PACKET_MSG	34
#define	WSC_EAPOL_START_MSG		35
#define	WSC_EAPOL_UPNP_MSG		36


/* define OpMode for WscSendMessage */
#undef     AP_MODE         
#undef     AP_CLIENT_MODE  
#undef     STA_MODE        

#define     AP_MODE         	0x00
#define     AP_CLIENT_MODE  	0x01
#define     STA_MODE        	0x02

/* For WSC state machine states. */
/* We simplified it */
typedef	enum	_WscState
{
	WSC_STATE_OFF,
	WSC_STATE_INIT,
	WSC_STATE_START,
	WSC_STATE_FAIL,	
   	WSC_STATE_CONFIGURED,
   	WSC_STATE_LINK_UP,
   	WSC_STATE_SEND_EAPOL_START,
	WSC_STATE_WAIT_EAPOL_START,
	WSC_STATE_WAIT_UPNP_START,
	WSC_STATE_WAIT_REQ_ID,
	WSC_STATE_WAIT_RESP_ID,
	WSC_STATE_WAIT_WSC_START,
	WSC_STATE_WAIT_M1,
	WSC_STATE_SENT_M1,
	WSC_STATE_SENT_M2D,
	WSC_STATE_WAIT_M2,
	WSC_STATE_RX_M2D,	
	WSC_STATE_WAIT_PIN,
	WSC_STATE_WAIT_M3,
	WSC_STATE_WAIT_M4,
	WSC_STATE_WAIT_M5,
	WSC_STATE_WAIT_M6,
	WSC_STATE_WAIT_M7,
	WSC_STATE_WAIT_M8,
	WSC_STATE_WAIT_DONE,
	WSC_STATE_WAIT_ACK,
	WSC_STATE_WAIT_EAPFAIL,
	WSC_STATE_WAIT_DISCONN
}	WSC_STATE;


#define WSC_PIN_ATTACK_CHECK	600
#define WSC_V2_VERSION			0x20
#define		TLV_ASCII	0
#define		TLV_HEX		1

typedef	struct	_WSC_TLV {
	USHORT	TlvTag;
	USHORT	TlvLen;
	PUCHAR	pTlvData;
	UCHAR	TlvType;	/* 0: ASCII, 1: Hex */
} WSC_TLV, *PWSC_TLV;

typedef	struct	_WSC_V2_INFO {
	WSC_TLV			ExtraTlv;
	BOOLEAN			bWpsEnable;		/* FALSE: disable WSC , TRUE: enable WSC */
	BOOLEAN			bEnableWpsV2;	/* FALSE: not support WSC 2.0, TRUE: support WSC 2.0 */
#ifdef CONFIG_STA_SUPPORT
    BOOLEAN			bForceSetAP; /* FALSE: do not change AP's setting when AP is configured, TRUE: force to change AP's setting when AP is configured */
#endif /* CONFIG_STA_SUPPORT */
} WSC_V2_INFO, *PWSC_V2_INFO;


#define MAX_PBC_STA_TABLE_SIZE	4
typedef struct _WSC_STA_PBC_PROBE_INFO {
	ULONG				ReciveTime[MAX_PBC_STA_TABLE_SIZE];
    UCHAR				WscPBCStaProbeCount;
    UCHAR   			StaMacAddr[MAX_PBC_STA_TABLE_SIZE][MAC_ADDR_LEN];
	UCHAR				Valid[MAX_PBC_STA_TABLE_SIZE];
} WSC_STA_PBC_PROBE_INFO, *PWSC_STA_PBC_PROBE_INFO;


/* WSC configured credential */
typedef struct _WSC_CREDENTIAL {
	NDIS_802_11_SSID SSID;	/* mandatory */
	USHORT AuthType;	/* mandatory, 1: open, 2: wpa-psk, 4: shared, 8:wpa, 0x10: wpa2, 0x20: wpa2-psk */
	USHORT EncrType;	/* mandatory, 1: none, 2: wep, 4: tkip, 8: aes */
	UCHAR Key[64];		/* mandatory, Maximum 64 byte */
	USHORT				KeyLength;
	UCHAR MacAddr[6];	/* mandatory, AP MAC address */
	UCHAR KeyIndex;		/* optional, default is 1 */
	UCHAR bFromUPnP;	/* TRUE: This credential is from external UPnP registrar */
	UCHAR Rsvd[2];		/* Make alignment */
}	WSC_CREDENTIAL, *PWSC_CREDENTIAL;

/* WSC configured profiles */
typedef struct _WSC_PROFILE {
	UINT			ProfileCnt;
	UINT ApplyProfileIdx;	/* add by johnli, fix WPS test plan 5.1.1 */
	WSC_CREDENTIAL Profile[8];	/* Support up to 8 profiles */
}	WSC_PROFILE, *PWSC_PROFILE;


#define MAX_2ND_DEV_TYPE_LIST	2
#define MAX_2ND_DEV_TYPE_LIST_BUFFER	(1+(8*MAX_2ND_DEV_TYPE_LIST))
/* Data structure to hold Enrollee and Registrar information */
typedef struct	_WSC_DEV_INFO
{
	UCHAR	Version;
	UCHAR	Version2;
	UCHAR	Uuid[16];
	UCHAR	MacAddr[6];
	UCHAR	DeviceName[32];
	UCHAR	PriDeviceType[8];
	UCHAR	SecDevTypList[MAX_2ND_DEV_TYPE_LIST_BUFFER]; /* 2nd Device Type List, ref. P2P Spec. v1.1 Table 29*/
	USHORT	AuthTypeFlags;
	USHORT	EncrTypeFlags;
	UCHAR	ConnTypeFlags;
	USHORT	ConfigMethods;
	UCHAR	ScState;
	UCHAR	Manufacturer[64];
	UCHAR	ModelName[32];
	UCHAR	ModelNumber[32];
	UCHAR	SerialNumber[32];
	UCHAR	RfBand;
	UINT	OsVersion;
	UINT	FeatureId;
	USHORT	AssocState;
	USHORT	DevPwdId;
	USHORT	ConfigError;
	UCHAR	Ssid[32];
    UCHAR	NewKey[64 + 1]; /* not sure sprintf would add '\0' or not, add one byte for \0' */
    INT     NewKeyLen;
    UCHAR   NewKeyIndex;
}	WSC_DEV_INFO, *PWSC_DEV_INFO;

/* WSC saved message */
typedef	struct	_WSC_MESSAGE
{
	INT	    Length;			/* Length of saved message */
	UCHAR	Data[2048];		/* Contents */
}	WSC_MESSAGE, *PWSC_MESSAGE;


/* data structure to store info of the instance of Registration protocol */
typedef	struct	_WSC_REG_DATA
{
	/* filled in by device self */
	WSC_DEV_INFO	SelfInfo;        
	/* filled in by wps peer */
	WSC_DEV_INFO	PeerInfo;    

	/*Diffie Hellman parameters */
/*	BIGNUM		*DH_PubKey_Peer; //peer's pub key stored in bignum format */
/*	DH			*DHSecret;       //local key pair in bignum format */
	UCHAR		EnrolleeRandom[192];	/* Saved random byte for public key generation */
	
	UCHAR		ReComputePke;
	UCHAR		Pke[192];		/*enrollee's raw pub key */
	UCHAR		Pkr[192];		/*registrar's raw pub key */

	UCHAR		SecretKey[192];	/* Secret key calculated by enrollee */
	
	UCHAR		StaEncrSettings[128];		/* to be sent in M2/M8 by reg & M7 by enrollee */
	UCHAR		ApEncrSettings[1024];

	/* Saved Message content for authenticator calculation */
	WSC_MESSAGE	LastTx;
	WSC_MESSAGE	LastRx;

	/* Device password */
	UCHAR		PIN[8];
	UCHAR		PinCodeLen;
	
	/* From KDF Key */
	UCHAR		AuthKey[32];
	UCHAR		KeyWrapKey[16];
	UCHAR		Emsk[32];
	
	USHORT		EnrolleePwdId;
	UCHAR		EnrolleeNonce[16];		/*N1, from enrollee */
	UCHAR		RegistrarNonce[16];		/*N2, from registrar */
	UCHAR		SelfNonce[16];

	UCHAR		Psk1[16];
	UCHAR		Psk2[16];

	UCHAR		EHash1[32];
	UCHAR		EHash2[32];
	UCHAR		Es1[16];
	UCHAR		Es2[16];

	UCHAR		RHash1[32];
	UCHAR		RHash2[32];
	UCHAR		Rs1[16];
	UCHAR		Rs2[16];
}	WSC_REG_DATA, *PWSC_REG_DATA;

/* WSC UPnP node info. */
typedef struct _WSC_UPNP_NODE_INFO{
	BOOLEAN				bUPnPInProgress;
	BOOLEAN				bUPnPMsgTimerRunning;
	BOOLEAN				bUPnPMsgTimerPending;
	UINT				registrarID;
	RALINK_TIMER_STRUCT   UPnPMsgTimer;
}WSC_UPNP_NODE_INFO, *PWSC_UPNP_NODE_INFO;

typedef struct PACKED _WSC_PEER_DEV_INFO {
	UCHAR	WscPeerDeviceName[32];
	UCHAR	WscPeerManufacturer[64];
	UCHAR	WscPeerModelName[32];
	UCHAR	WscPeerModelNumber[32];
	UCHAR	WscPeerSerialNumber[32];
	UCHAR	WscPeerMAC[6];
} WSC_PEER_DEV_INFO, *PWSC_PEER_DEV_INFO;


typedef	struct	_WSC_CTRL
{
    INT             WscConfMode;    /* 0 Wsc not enable; 1 un-configure AP ; 3 un-configure AP with Proxy ; */
                                    /* 5 un-configure AP with Registrar ; 7 un-configure AP with proxy and Registrar */
    INT             WscMode;        /* 1 PIN ;2 PBC set from UI dynamically */
    UCHAR           WscConfStatus;  /* 1 un-configured; 2 configured; need to update to .dat */
    USHORT          WscConfigMethods;  /* Registrar support list. The List is bitwise. PBC:0x0080 Lable:0x0004 Display:0x0008 */
#ifdef UCOS
    BOOLEAN         bWscProfileConfMethods; /* Used profile configure methods */
#endif /* UCOS */
    INT             WscStatus;      /* for user to monitor the status */
    INT             WscState;       /* WSC Protocl State: M1 to M8 */
    UINT            WscPinCode;     /* record the UI's PIN code input when we are registrar */
    UCHAR           WscPinCodeLen;  /* record the UI's PIN code input length when we are registrar */
    BOOLEAN         WscEnrollee4digitPinCode;  /* flag to use 4 or 8 digit Device own PIN code. */
    UINT            WscEnrolleePinCode; /* recored Device own PIN code. */
    UCHAR           WscEnrolleePinCodeLen; /* recored Device own PIN code length */
    INT             WscSelReg;     /* record the UI's PIN code input when we are registrar */
    NDIS_802_11_SSID        WscSsid;                /* select a desired ssid to connect for PIN mode */
    UCHAR                   WscPBCBssCount;         /* Count of PBC activated APs. */
    UCHAR                   WscBssid[MAC_ADDR_LEN]; /* select a desired bssid to connect */
#ifdef CONFIG_STA_SUPPORT
    BOOLEAN                 WscEnAssociateIE;       /* Add WSC IE on Associate frame. */
    BOOLEAN                 WscEnProbeReqIE;        /* Add WSC IE on Probe-Req frame. */
    UCHAR                   WscPeerMAC[MAC_ADDR_LEN];   /* peer Mac Address */
#endif /* CONFIG_STA_SUPPORT */
    WSC_REG_DATA    RegData;        /* Registrar pair data */
    UCHAR           lastId;
    UCHAR           WscUseUPnP;
    BOOLEAN         EapMsgRunning;  /* already recived Eap-Rsp(Identity) and sent M1 or Eap-Req(Start) */
    UCHAR           WscRetryCount;
    UCHAR           EntryIfIdx;
    UCHAR           EntryAddr[MAC_ADDR_LEN];
    BOOLEAN         Wsc2MinsTimerRunning;
    RALINK_TIMER_STRUCT   Wsc2MinsTimer;
    WSC_PROFILE         WscProfile;     /* Saved WSC profile after M8 */
#ifdef CONFIG_STA_SUPPORT
    WSC_PROFILE             WscM7Profile;   /* Saved WSC profile from AP Settings in M7 */
    BOOLEAN                 bConfiguredAP;  /* True: AP is in the configured state. FALSE: others */
#endif /* CONFIG_STA_SUPPORT */
    WSC_UPNP_NODE_INFO  WscUPnPNodeInfo;    /*Use to save UPnP node related info. */

    BOOLEAN             EapolTimerRunning; 
    BOOLEAN             EapolTimerPending;
    RALINK_TIMER_STRUCT   EapolTimer;

    BOOLEAN                 WscPBCTimerRunning;
    RALINK_TIMER_STRUCT     WscPBCTimer;
    BOOLEAN                 WscScanTimerRunning;
    RALINK_TIMER_STRUCT     WscScanTimer;
    BOOLEAN                 WscProfileRetryTimerRunning;
    RALINK_TIMER_STRUCT     WscProfileRetryTimer;
#ifdef CONFIG_STA_SUPPORT
    /* 0x00: disable, 0x01: Auto Connect first credential only, 0x02: Auto Connect and rotate all crentials */
    UCHAR                   WscDriverAutoConnect;       
#endif /* CONFIG_STA_SUPPORT */
    UCHAR               WpaPsk[64];
    INT                 WpaPskLen;
    BOOLEAN             bWscTrigger;        /* TRUE: AP-Enrollee & AP-Registrar work, FALSE: AP-Enrollee & AP-Registrar stop working */
    PVOID                   pAd;
    UINT                WscLastPinFromEnrollee;
    BOOLEAN             WscRejectSamePinFromEnrollee;
#ifdef CONFIG_AP_SUPPORT
    NDIS_802_11_SSID        WscDefaultSsid;     /* Default WPS SSID after WPS process complete with Enrollee when AP is un-configured Registrar. */
    BOOLEAN                 bWCNTest;
#endif /* CONFIG_AP_SUPPORT */
    INT                 WscKeyASCII;        /*WscKeyASCII (0:Hex, 1:ASCII(random length), others: ASCII length(8~63, default 8)) */
    INT                 WscActionMode;
    UCHAR                   Wsc_Uuid_E[UUID_LEN_HEX];
    UCHAR                   Wsc_Uuid_Str[UUID_LEN_STR];

    UCHAR               WpsApBand; /* Preferred WPS AP PHY type. Ref: PREFERRED_WPS_AP_PHY_TYPE */
    UCHAR Flags;

    WSC_PEER_DEV_INFO   WscPeerInfo;
    BOOLEAN             bCheckMultiByte;

    WSC_V2_INFO             WscV2Info;

    RALINK_TIMER_STRUCT WscUpdatePortCfgTimer;
    BOOLEAN             WscUpdatePortCfgTimerRunning;
#ifdef CONFIG_AP_SUPPORT
    RALINK_TIMER_STRUCT WscSetupLockTimer;
    BOOLEAN             WscSetupLockTimerRunning;
    RALINK_TIMER_STRUCT WscPinAttackCountCheckTimer;
    BOOLEAN             bSetupLock;
    UCHAR               PinAttackCount;
    UCHAR               MaxPinAttack;
    UINT                SetupLockTime; /* unit: minute */
#endif /* CONFIG_AP_SUPPORT */
    BOOLEAN             bWscAutoTigeer; 
    BOOLEAN             bWscFragment;
    PUCHAR              pWscRxBuf;
    INT                 WscRxBufLen;
    USHORT              WscFragSize;
    INT                 WscTxBufLen;
    PUCHAR              pWscTxBuf;
    BOOLEAN             bWscLastOne;
    BOOLEAN             bWscFirstOne;
    PUCHAR              pWscCurBufIdx;
    NDIS_SPIN_LOCK      WscPeerListSemLock;
    LIST_HEADER         WscPeerList;
    RALINK_TIMER_STRUCT M2DTimer;
    BOOLEAN             bM2DTimerRunning;
    INT                 M2DACKBalance;
    CHAR  *             pExtraData;
}WSC_CTRL, *PWSC_CTRL;


/* structure to store Simple Config Attributes Info */
typedef struct PACKED _WSC_LV_INFO {
    USHORT  ValueLen;
    UCHAR   Value[512];
} WSC_LV_INFO;

typedef enum _WscSecurityMode{
	WPA2PSKAES,
	WPA2PSKTKIP,
	WPAPSKAES,
	WPAPSKTKIP,
}WSC_SECURITY_MODE;


typedef struct _WSC_COMMON_CONFIG_T
{
    UINT8					WscHdrPshBtnCheckCount;
	BOOLEAN					WscPBCOverlap;
	WSC_STA_PBC_PROBE_INFO	WscStaPbcProbeInfo;
}WSC_COMMON_CONFIG_T;

typedef struct _WSC_STA_CONFIG_T
{
	WSC_LV_INFO			WpsIEBeacon;
   	WSC_LV_INFO			WpsIEProbeResp;
	WSC_CTRL	        WscControl;    
}WSC_STA_CONFIG_T;

typedef struct _WSC_MULTI_BSS_T
{
    WSC_CTRL	         WscControl;
	WSC_LV_INFO          WscIEBeacon;
   	WSC_LV_INFO          WscIEProbeResp;
	WSC_SECURITY_MODE    WscSecurityMode; 
}WSC_MULTI_BSS_T;

typedef struct _WSC_AP_CONFIG_T
{
    WSC_MULTI_BSS_T MBSSID[4];
}WSC_AP_CONFIG_T;



/*Messages for the WSC State machine */
#define	MAX_WSC_MSG			    3
#define	WSC_IDLE						0
#define	MAX_WSC_STATE					1
#define	WSC_FUNC_SIZE					(MAX_WSC_STATE * MAX_WSC_MSG)

typedef struct _WSC_MLME_T
{
    STATE_MACHINE			WscMachine;
    STATE_MACHINE_FUNC      WscFunc[WSC_FUNC_SIZE];
}WSC_MLME_T;

typedef struct _WSC_ADAPTER_T
{
    WSC_COMMON_CONFIG_T CommonCfg;
    WSC_STA_CONFIG_T    StaCfg;
    WSC_AP_CONFIG_T     ApCfg;
    WSC_MLME_T          Mlme;
    UCHAR	            *pHmacData;
    CHAR                *pExtraData;
}WSC_ADAPTER_T;


extern WSC_ADAPTER_T * (*WscGetAdapter)();

extern VOID (*WscBuildAssocReqIE)(
	/*IN	PRTMP_ADAPTER	pAd,*/
	IN  PWSC_CTRL	pWscControl,
    OUT	PUCHAR			pOutBuf,
    OUT	PUCHAR			pIeLen);
extern VOID (*WscBuildProbeReqIE)(
        IN  PRTMP_ADAPTER   pAd,
        IN  UCHAR           CurOpMode,
        OUT PUCHAR          pOutBuf,
        OUT PUCHAR          pIeLen);
extern VOID    (*WscSendEapolStart)(
    IN  PRTMP_ADAPTER   pAdapter,
    IN  PUCHAR          pBssid,
    IN  UCHAR			CurOpMode);

extern VOID    (*WscStateMachineInit)(
    IN	PRTMP_ADAPTER		pAd,
    IN  STATE_MACHINE       *S,
    OUT STATE_MACHINE_FUNC  Trans[]);

extern BOOL (*WscIsSelectedRegistrar)(
   void *ie_buf);

extern  VOID  (*WpsSmProcess)(
        IN PRTMP_ADAPTER        pAd,
        IN MLME_QUEUE_ELEM     *Elem);

extern BOOLEAN (*WscMsgTypeSubst)(
    IN  UCHAR   EAPType,
    IN  UCHAR   EAPCode,
    OUT INT     *MsgType);

#endif
