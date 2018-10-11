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

#ifndef	__WSC_H__
#define	__WSC_H__

#define WSC_VMALLOC_KEY_MEM(pubKey, secKey, Size) \
{ \
    pubKey = vmalloc(Size); \
	if (pubKey == NULL) \
		DBGPRINT(RT_DEBUG_ERROR, ("Allocate memory for WscControl.pPubKeyMem failed\n")); \
	secKey = vmalloc(Size); \
	if (secKey == NULL) \
		DBGPRINT(RT_DEBUG_ERROR, ("Allocate memory for WscControl.pSecKeyMem failed\n")); \
} \

#define WSC_VFREE_KEY_MEM(pubKey, secKey) \
{ \
    if (pubKey) \
		vfree(pubKey); \
	if (secKey) \
		vfree(secKey); \
} \

//Messages for the WSC state machine,
#define	WSC_MACHINE_BASE		34
#define	WSC_EAPOL_PACKET_MSG	34
#define	WSC_EAPOL_START_MSG		35
#define	WSC_EAPOL_UPNP_MSG		36

#define	MAX_WSC_MSG			    3

// WSC OUI SMI
#define WSC_OUI				0x0050f204
#define	WSC_SMI				0x00372A
#define	WSC_VENDOR_TYPE		0x00000001

// EAP code
#define	EAP_CODE_REQ		0x01
#define	EAP_CODE_RSP		0x02
#define	EAP_CODE_FAIL		0x04
#define EAP_TYPE_ID			0x01
#define EAP_TYPE_NOTIFY		0x02
#define	EAP_TYPE_WSC		0xfe

// WSC Opcode
#define	WSC_OPCODE_START		0x01
#define	WSC_OPCODE_ACK			0x02
#define	WSC_OPCODE_NACK			0x03
#define	WSC_OPCODE_MSG			0x04
#define	WSC_OPCODE_DONE			0x05
#define	WSC_OPCODE_FRAG_ACK		0x06

#define WSC_OPCODE_UPNP_MASK	0x10
#define WSC_OPCODE_UPNP_DATA	0x11
#define WSC_OPCODE_UPNP_MGMT	0x12
#define WSC_OPCODE_UPNP_CTRL	0x13

#define WSC_UPNP_MGMT_SUB_PROBE_REQ		0x01
#define WSC_UPNP_MGMT_SUB_CONFIG_REQ	0x02
#define WSC_UPNP_MGMT_SUB_REG_SELECT	0x03


//patch for Atheros External registrar
#define WSC_UPNP_DATA_SUB_INCLUDE_MAC	0x0100

#define WSC_UPNP_DATA_SUB_NORMAL		0x00
#define WSC_UPNP_DATA_SUB_TO_ALL		0x01
#define WSC_UPNP_DATA_SUB_TO_ONE		0x02
#define WSC_UPNP_DATA_SUB_ACK			0x03
#define WSC_UPNP_DATA_SUB_M1			0x04
#define WSC_UPNP_DATA_SUB_M2			0x05
#define WSC_UPNP_DATA_SUB_M2D			0x06
#define WSC_UPNP_DATA_SUB_M3			0x07
#define WSC_UPNP_DATA_SUB_M4			0x08
#define WSC_UPNP_DATA_SUB_M5			0x09
#define WSC_UPNP_DATA_SUB_M6			0x0A
#define WSC_UPNP_DATA_SUB_M7			0x0B
#define WSC_UPNP_DATA_SUB_M8			0x0C
#define WSC_UPNP_DATA_SUB_WSC_ACK		0x0D
#define WSC_UPNP_DATA_SUB_WSC_NACK		0x0E
#define WSC_UPNP_DATA_SUB_WSC_DONE		0x0F
#define WSC_UPNP_DATA_SUB_WSC_UNKNOWN	0xff


// Wsc EAP Messges type
#define	WSC_MSG_EAP_RSP_ID			0x21	
#define	WSC_MSG_EAP_REG_RSP_ID      0x22
#define	WSC_MSG_EAP_ENR_RSP_ID      0x23
#define	WSC_MSG_EAP_UPNP_RSP_ID     0x24
#define	WSC_MSG_EAP_REQ_ID			0x25
#define	WSC_MSG_EAP_REQ_START		0x26
#define	WSC_MSG_EAP_FAIL			0x27
#define	WSC_MSG_PROB_RSP			0x01
#define WSC_MSG_EAPOL_START         0x02
#define	WSC_MSG_M1					0x04
#define	WSC_MSG_M2					0x05
#define	WSC_MSG_M2D					0x06
#define	WSC_MSG_M3					0x07
#define	WSC_MSG_M4					0x08
#define	WSC_MSG_M5					0x09
#define	WSC_MSG_M6					0x0A
#define	WSC_MSG_M7					0x0B
#define	WSC_MSG_M8					0x0C
#define	WSC_MSG_WSC_ACK				0x0D
#define	WSC_MSG_WSC_NACK			0x0E
#define	WSC_MSG_WSC_DONE			0x0F
#define	WSC_MSG_UNKNOWN				0xff

// WSC connection mode
#define	WSC_PIN_MODE		        1
#define	WSC_PBC_MODE		        2

// Value of WSC_IE_DEV_PASS_ID 0x1012
#define	DEV_PASS_ID_PIN				0x0000
#define	DEV_PASS_ID_USER			0x0001
#define	DEV_PASS_ID_MACHINE			0x0002
#define	DEV_PASS_ID_REKEY			0x0003
#define	DEV_PASS_ID_PBC				0x0004
#define	DEV_PASS_ID_REG				0x0005

// Common definition
#define	WSC_VERSION					0x10
#define WSC_CONFIG_METHODS			0x008A

// Wsc status code
#define	STATUS_WSC_NOTUSED						0
#define	STATUS_WSC_IDLE							1
#define STATUS_WSC_FAIL        			        2		// WSC Process Fail
#define	STATUS_WSC_LINK_UP						3		// Start WSC Process
#define	STATUS_WSC_EAPOL_START_RECEIVED			4		// Received EAPOL-Start
#define	STATUS_WSC_EAP_REQ_ID_SENT				5		// Sending EAP-Req(ID)
#define	STATUS_WSC_EAP_RSP_ID_RECEIVED			6		// Receive EAP-Rsp(ID)
#define	STATUS_WSC_EAP_RSP_WRONG_SMI			7		// Receive EAP-Req with wrong WSC SMI Vendor Id
#define	STATUS_WSC_EAP_RSP_WRONG_VENDOR_TYPE	8		// Receive EAPReq with wrong WSC Vendor Type
#define	STATUS_WSC_EAP_REQ_WSC_START			9		// Sending EAP-Req(WSC_START)
#define	STATUS_WSC_EAP_M1_SENT					10		// Send M1
#define	STATUS_WSC_EAP_M1_RECEIVED				11		// Received M1
#define	STATUS_WSC_EAP_M2_SENT					12		// Send M2
#define	STATUS_WSC_EAP_M2_RECEIVED				13		// Received M2
#define	STATUS_WSC_EAP_M2D_RECEIVED				14		// Received M2D
#define	STATUS_WSC_EAP_M3_SENT					15		// Send M3
#define	STATUS_WSC_EAP_M3_RECEIVED				16		// Received M3
#define	STATUS_WSC_EAP_M4_SENT					17		// Send M4
#define	STATUS_WSC_EAP_M4_RECEIVED				18		// Received M4
#define	STATUS_WSC_EAP_M5_SENT					19		// Send M5
#define	STATUS_WSC_EAP_M5_RECEIVED				20		// Received M5
#define	STATUS_WSC_EAP_M6_SENT					21		// Send M6
#define	STATUS_WSC_EAP_M6_RECEIVED				22		// Received M6
#define	STATUS_WSC_EAP_M7_SENT					23		// Send M7
#define	STATUS_WSC_EAP_M7_RECEIVED				24		// Received M7
#define	STATUS_WSC_EAP_M8_SENT					25		// Send M8
#define	STATUS_WSC_EAP_M8_RECEIVED				26		// Received M8
#define	STATUS_WSC_EAP_RAP_RSP_ACK				27		// Processing EAP Response (ACK)
#define	STATUS_WSC_EAP_RAP_REQ_DONE_SENT		28		// Processing EAP Request (Done)
#define	STATUS_WSC_EAP_RAP_RSP_DONE_SENT		29		// Processing EAP Response (Done)
#define STATUS_WSC_EAP_FAIL_SENT                30      // Sending EAP-Fail
#define STATUS_WSC_ERROR_HASH_FAIL              31      // WSC_ERROR_HASH_FAIL
#define STATUS_WSC_ERROR_HMAC_FAIL              32      // WSC_ERROR_HMAC_FAIL
#define STATUS_WSC_ERROR_DEV_PWD_AUTH_FAIL      33      // WSC_ERROR_DEV_PWD_AUTH_FAIL
#define STATUS_WSC_CONFIGURED					34
#define	STATUS_WSC_SCAN_AP						35		// Scanning AP
#define	STATUS_WSC_EAPOL_START_SENT				36
#define STATUS_WSC_EAP_RSP_DONE_SENT			37
#define STATUS_WSC_WAIT_PIN_CODE                38
#define STATUS_WSC_START_ASSOC					39

// All error message dtarting from 0x0100
#define	STATUS_WSC_PBC_TOO_MANY_AP				0x0101		// Too many PBC AP avaliable
#define	STATUS_WSC_PBC_NO_AP					0x0102		// No PBC AP avaliable
#define	STATUS_WSC_EAP_FAIL_RECEIVED			0x0103		// Received EAP-FAIL
#define	STATUS_WSC_EAP_NONCE_MISMATCH			0x0104		// Receive EAP with wrong NONCE
#define	STATUS_WSC_EAP_INVALID_DATA				0x0105		// Receive EAP without integrity (Hmac mismatch)
#define STATUS_WSC_PASSWORD_MISMATCH			0x0106		// Error PIN Code (R-Hash mismatch)
#define	STATUS_WSC_EAP_REQ_WRONG_SMI			0x0107		// Receive EAP-Req with wrong WPS SMI Vendor Id
#define	STATUS_WSC_EAP_REQ_WRONG_VENDOR_TYPE	0x0108		// Receive EAPReq with wrong WPS Vendor Type


#define WSC_DISABLE		0x0
#define WSC_ENROLLEE	0x1
#define WSC_PROXY		0x2
#define WSC_REGISTRAR	0x4

// Device request/response type
#define WSC_MSGTYPE_ENROLLEE_INFO_ONLY		0x00
#define WSC_MSGTYPE_ENROLLEE_OPEN_8021X		0x01
#define WSC_MSGTYPE_REGISTRAR				0x02
#define WSC_MSGTYPE_AP_WLAN_MGR				0x03

// RF Band
#define WSC_RFBAND_24GHZ    0x01
#define WSC_RFBAND_50GHZ    0x02

// Simple Config state
#define WSC_SCSTATE_UNCONFIGURED	0x01
#define WSC_SCSTATE_CONFIGURED		0x02

// Common definition
#define WSC_EAP_TIMER	1
#define WSC_2MINS_TIMER	2
#define WSC_PBC_TIMER	3
#define WSC_SCAN_TIMER	4

// Time-Out, param for timer func, count by micro-sec, not ticks
#define WSC_EAPOL_START_TIME_OUT    2000 
#define WSC_EAP_ID_TIME_OUT         5000
#define WSC_EAP_MSG_TIME_OUT        5000
#define WSC_EAP_EAP_FAIL_TIME_OUT   1000
#define WSC_TWO_MINS_TIME_OUT       120000
#define	WSC_PROFILE_RETRY_TIME_OUT	10000

#define WSC_INIT_ENTRY_APIDX        0xFF
#define WSC_MAX_DATA_LEN            1024

#define WSC_ENTRY_GET_EAPOL_START   0x1
#define WSC_ENTRY_GET_EAP_RSP_ID    0x2

// General used field

// UUID related definition
#define UUID_LEN_HEX 16  // 128 bits => 16 bytes
#define UUID_LEN_STR 37  // hex to string, plus 4 dash, plus 1 '\0'
#define UUID_VERSION 1   // We currently just support version 1

typedef struct _WSC_UUID_T{
	UINT32 timeLow;
	UINT16 timeMid;
	UINT16 timeHi_Version;
	UCHAR  clockSeqHi_Var;
	UCHAR  clockSeqLow;
	UCHAR  node[6];
}WSC_UUID_T;

extern UCHAR Wsc_Uuid_E[UUID_LEN_HEX];
extern UCHAR Wsc_Uuid_Str[UUID_LEN_STR];

// 802.1x authentication format
typedef	struct	_IEEE8021X_FRAME	{
	UCHAR	Version;					// 1.0
	UCHAR	Type;						// 0 = EAP Packet
	USHORT	Length;
}	IEEE8021X_FRAME, *PIEEE8021X_FRAME;

// EAP frame format
typedef	struct PACKED	_EAP_FRAME	{
	UCHAR	Code;						// 1 = Request, 2 = Response
	UCHAR	Id;
	USHORT	Length;
	UCHAR	Type;						// 1 = Identity, 0xfe = reserved, used by WSC
}	EAP_FRAME, *PEAP_FRAME;

// WSC fixed information within EAP
typedef	struct PACKED	_WSC_FRAME	{
	UCHAR	SMI[3];
	UINT	VendorType;
	UCHAR	OpCode;
	UCHAR	Flags;
}	WSC_FRAME, *PWSC_FRAME;

// For WSC state machine states.
// We simplified it
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

// WSC IE structure
typedef	struct PACKED	_WSC_IE
{
	USHORT	Type;
	USHORT	Length;
	UCHAR	Data[1];	// variable length data
}	WSC_IE, *PWSC_IE;

// WSC saved message
typedef	struct	_WSC_MESSAGE
{
	INT	    Length;			// Length of saved message
	UCHAR	Data[2048];		// Contents
}	WSC_MESSAGE, *PWSC_MESSAGE;

// Data structure to hold Enrollee and Registrar information
typedef struct	_WSC_DEV_INFO
{
	UCHAR	Version;
	UCHAR	Uuid[16];
	UCHAR	MacAddr[6];
	UCHAR	DeviceName[32];
	UCHAR	PriDeviceType[8];
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
    UCHAR	NewKey[64 + 1]; // not sure sprintf would add '\0' or not, add one byte for \0'
    INT     NewKeyLen;
    UCHAR   NewKeyIndex;
}	WSC_DEV_INFO, *PWSC_DEV_INFO;

// data structure to store info of the instance of Registration protocol
typedef	struct	_WSC_REG_DATA
{
	// enrollee endpoint - filled in by the Registrar, NULL for Enrollee
	WSC_DEV_INFO	EnrolleeInfo;        
	// Registrar endpoint - filled in by the Enrollee, NULL for Registrar
	WSC_DEV_INFO	RegistrarInfo;    

	//Diffie Hellman parameters
	UCHAR		EnrolleeRandom[192];	// Saved random byte for public key generation
	
	UCHAR		ReComputePke;
	UCHAR		Pke[192];		//enrollee's raw pub key
	UCHAR		Pkr[192];		//registrar's raw pub key

	UCHAR		SecretKey[192];	// Secret key calculated by enrollee
	
	UCHAR		StaEncrSettings[128];		// to be sent in M2/M8 by reg & M7 by enrollee
	UCHAR		ApEncrSettings[1024];

	// Saved Message content for authenticator calculation
	WSC_MESSAGE	LastTx;
	WSC_MESSAGE	LastRx;

	// Device password
	UCHAR		PIN[8];
	UCHAR		PinCodeLen;
	
	// From KDF Key
	UCHAR		AuthKey[32];
	UCHAR		KeyWrapKey[16];
	UCHAR		Emsk[32];
	
	USHORT		EnrolleePwdId;
	UCHAR		EnrolleeNonce[16];		//N1, from enrollee
	UCHAR		RegistrarNonce[16];		//N2, from registrar
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

// WSC configured credential
typedef	struct	_WSC_CREDENTIAL
{
	NDIS_802_11_SSID	SSID;				// mandatory
	USHORT				AuthType;			// mandatory, 1: open, 2: wpa-psk, 4: shared, 8:wpa, 0x10: wpa2, 0x20: wpa2-psk
	USHORT				EncrType;			// mandatory, 1: none, 2: wep, 4: tkip, 8: aes
	UCHAR				Key[64];				// mandatory, Maximum 64 byte
	USHORT				KeyLength;
	UCHAR				MacAddr[6];			// mandatory, AP MAC address
	UCHAR				KeyIndex;			// optional, default is 1
	UCHAR				Rsvd[3];			// Make alignment
}	WSC_CREDENTIAL, *PWSC_CREDENTIAL;

// WSC configured profiles
typedef	struct	_WSC_PROFILE
{
	UINT			ProfileCnt;
	WSC_CREDENTIAL	Profile[8];				// Support up to 8 profiles
}	WSC_PROFILE, *PWSC_PROFILE;
 
// WSC control block
typedef	struct	_WSC_CTRL
{
	INT             WscConfMode;    // 0 Wsc not enable; 1 un-configure AP ; 3 un-configure AP with Proxy ; 
	                                // 5 un-configure AP with Registrar ; 7 un-configure AP with proxy and Registrar
	INT             WscMode;        // 1 PIN ;2 PBC set from UI dynamically
	INT             WscStatus;      // for user to monitor the status
	INT             WscState;    	// WSC Protocl State: M1 to M8
	UINT            WscPinCode;     // record the UI's PIN code input when we are registrar
	UINT            WscEnrolleePinCode; // recored Device own PIN code.
	INT             WscSelReg;     // record the UI's PIN code input when we are registrar
	NDIS_802_11_SSID	    WscSsid;		        // select a desired ssid to connect for PIN mode 
	UCHAR				    WscBssid[MAC_ADDR_LEN];	// select a desired bssid to connect for PIN mode
	BOOLEAN				    WscEnAssociateIE;	    // Add WSC IE on Associate frame.
	BOOLEAN				    WscEnProbeReqIE;	    // Add WSC IE on Probe-Req frame.
	WSC_REG_DATA	RegData;		// Registrar pair data	
	UCHAR           lastId;
	BOOLEAN         EapMsgRunning;  // already recived Eap-Rsp(Identity) and sent M1 or Eap-Req(Start)
	UCHAR           WscRetryCount;
	/* 0x00: disable, 0x01: Auto Connect first credential only, 0x02: Auto Connect and rotate all crentials */	
	BOOLEAN				WscDriverAutoConnect;
	WSC_PROFILE			WscProfile;		// Saved WSC profile after M8
	WSC_PROFILE		        WscM7Profile;	// Saved WSC profile from AP Settings in M7
	BOOLEAN			        bConfiguredAP;	// True: AP is in the configured state. FALSE: others
    UCHAR				WpaPsk[64];
    INT					WpaPskLen;
    BOOLEAN				bWscTrigger;        // TRUE: AP-Enrollee & AP-Registrar work, FALSE: AP-Enrollee & AP-Registrar stop working
    void				*pPubKeyMem;
	void				*pSecKeyMem;
    PVOID				pAd;
    UINT				WscLastPinFromEnrollee;
    BOOLEAN				WscRejectSamePinFromEnrollee;
	UCHAR				WscActionMode;
	UCHAR				StaMacAddr[MAC_ADDR_LEN]; // Station's MAC Address

	BOOLEAN				EapolTimerRunning; 
    BOOLEAN				EapolTimerPending;
	BOOLEAN				WscPBCTimerRunning;
	BOOLEAN				WscScanTimerRunning;
	BOOLEAN				Wsc2MinsTimerRunning;

	UCHAR           WscConfStatus;  // 1 un-configured; 2 configured; need to update to .dat
	UCHAR			WscPinCodeLen;	// record the UI's PIN code input length when we are registrar
	UCHAR			WscEnrolleePinCodeLen; // recored Device own PIN code length
	UCHAR				WscPBCBssCount;			// Count of PBC activated APs.
}	WSC_CTRL, *PWSC_CTRL;

// structure to store Simple Config Attributes Info
typedef struct PACKED _WSC_LV_INFO {
    USHORT  ValueLen;
    UCHAR   Value[512];
} WSC_LV_INFO;

typedef struct PACKED _WSC_IE_HEADER {
	UCHAR elemId;
	UCHAR length;
	UCHAR oui[4];
} WSC_IE_HEADER;

typedef struct PACKED _WSC_CONFIGURED_VALUE {
	USHORT WscConfigured; // 1 un-configured; 2 configured
	UCHAR	WscSsid[32 + 1];
	USHORT WscAuthMode;	// mandatory, 0x01: open, 0x02: wpa-psk, 0x04: shared, 0x08:wpa, 0x10: wpa2, 0x20: wpa2-psk
	USHORT	WscEncrypType;	// 0x01: none, 0x02: wep, 0x04: tkip, 0x08: aes
	UCHAR	DefaultKeyIdx;
	UCHAR	WscWPAKey[64 + 1];
} WSC_CONFIGURED_VALUE;
  
/* 
	Ralink specific WSC Mesage Header definition. 
*/
#define RTMP_WSC_MSG_HDR_LEN		12	//msgType(2) + msgSubType(2) + ipAddr(4) + len(4)
typedef struct PACKED _RTMP_WSC_MSG_HDR{
	USHORT	msgType;
	USHORT	msgSubType;
	UINT   ipAddr;
	UINT   msgLen;		//Not include this header.
}RTMP_WSC_MSG_HDR;
 
// callback function 
typedef int (*PWPS_BUILD_M1_3_5_7_FUNC)(
	IN  PWSC_REG_DATA	pReg,
	IN  UCHAR			*pHmacData,
	OUT	VOID 			*pbuf);

typedef int (*PWPS_BUILD_DONE_ACK_NACKFUNC)(
	IN  PWSC_CTRL       pWscControl,
	OUT	VOID 			*pbuf);

typedef int (*PWPS_PROCESS_M2FUNC)(
	IN	VOID 			*precv,	
	IN	INT 			Length,
	IN  VOID 			*pSecKeyMem,
	IN  UCHAR			*pHmacData,
	OUT	PWSC_REG_DATA 	pReg);

typedef int (*PWPS_PROCESS_M2DFUNC)(
	IN	VOID 			*precv,
	IN	INT 			Length,
	OUT	PWSC_REG_DATA 	pReg);

typedef int (*PWPS_PROCESS_M4_6FUNC)(
	IN  PWSC_CTRL       pWscControl,
	IN	VOID 			*precv,
	IN	INT 			Length,
	IN  UCHAR			*pHmacData,
	OUT	PWSC_REG_DATA 	pReg);

typedef int (*PWPS_PROCESS_M8FUNC)(
	IN	VOID 			*precv,
	IN	INT 			Length,
	IN  PWSC_CTRL   	pWscControl,
	IN  UCHAR			*pHmacData);

typedef VOID (*PWPS_SET_TIMER)(
	IN	UCHAR		TimerType,
	IN	ULONG		Value);

typedef VOID (*PWPS_CANCEL_TIMER)(
	IN	UCHAR		TimerType);

typedef VOID (*PWPS_MOD_TIMER)(
	IN	UCHAR		TimerType,
	IN	ULONG		Value);

typedef VOID (*PWPS_WRITE_CONF_TO_PORTCFG)(
    IN  PWSC_CTRL       pWscControl,
    IN  PWSC_CREDENTIAL pCredential);

typedef VOID (*PWPS_LINK_DOWN)(
	IN  PWSC_CTRL       pWscControl);

typedef UCHAR (*PWPS_RANDOM_BYTE)(
	IN  PWSC_CTRL       pWscControl);

typedef struct _WPS_CALLBACK_FUNC{
	UCHAR  							StaWscPriDevType[8];
	PWPS_BUILD_M1_3_5_7_FUNC		WpsBuildM1;
	PWPS_BUILD_M1_3_5_7_FUNC		WpsBuildM3;
	PWPS_BUILD_M1_3_5_7_FUNC		WpsBuildM5;
	PWPS_BUILD_M1_3_5_7_FUNC		WpsBuildM7;
	PWPS_BUILD_DONE_ACK_NACKFUNC	WpsBuildDone;
	PWPS_BUILD_DONE_ACK_NACKFUNC	WpsBuildAck;
	PWPS_BUILD_DONE_ACK_NACKFUNC	WpsBuildNAck;
	PWPS_PROCESS_M2FUNC				WpsProcessM2;
	PWPS_PROCESS_M2DFUNC			WpsProcessM2D;
	PWPS_PROCESS_M4_6FUNC			WpsProcessM4;
	PWPS_PROCESS_M4_6FUNC			WpsProcessM6;
	PWPS_PROCESS_M8FUNC				WpsProcessM8;	
	PWPS_SET_TIMER					WpsSetTimer;
	PWPS_CANCEL_TIMER				WpsCancelTimer;
	PWPS_MOD_TIMER					WpsModTimer;
	PWPS_WRITE_CONF_TO_PORTCFG		WpsWriteConfToPortCfg;
	PWPS_LINK_DOWN					WpsLinkDown;
	PWPS_RANDOM_BYTE				WpsRandomByte;
}WPS_CALLBACK_FUNC;
  
#endif	// __WSC_H__

