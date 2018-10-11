/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	wpa.h

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Name		Date			Modification logs
*/

#ifndef	__WPA_H__
#define	__WPA_H__
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
/* 	
	WFA recommend to restrict the encryption type in 11n-HT mode.
 	So, the WEP and TKIP shall not be allowed to use HT rate. 
 */
#define IS_INVALID_HT_SECURITY(_mode)		\
	(((_mode) == Ndis802_11Encryption1Enabled) || \
	 ((_mode) == Ndis802_11Encryption2Enabled))

#define IS_WPA_CAPABILITY(a)       (((a) >= Ndis802_11AuthModeWPA) && ((a) <= Ndis802_11AuthModeWPA1PSKWPA2PSK))



#define MIX_CIPHER_WPA_TKIP_ON(x)       (((x) & 0x08) != 0)
#define MIX_CIPHER_WPA_AES_ON(x)        (((x) & 0x04) != 0)
#define MIX_CIPHER_WPA2_TKIP_ON(x)      (((x) & 0x02) != 0)
#define MIX_CIPHER_WPA2_AES_ON(x)       (((x) & 0x01) != 0)



// EAPOL Key descripter frame format related length
#define LEN_KEY_DESC_NONCE			32
#define LEN_KEY_DESC_IV				16
#define LEN_KEY_DESC_RSC			8
#define LEN_KEY_DESC_ID				8
#define LEN_KEY_DESC_REPLAY			8
#define LEN_KEY_DESC_MIC			16

// The length is the EAPoL-Key frame except key data field. 
// Please refer to 802.11i-2004 ,Figure 43u in p.78
#define LEN_EAPOL_KEY_MSG			(sizeof(KEY_DESCRIPTER) - MAX_LEN_OF_RSNIE)	

// EAP Code Type.
#define EAP_CODE_REQUEST	1
#define EAP_CODE_RESPONSE	2
#define EAP_CODE_SUCCESS    3
#define EAP_CODE_FAILURE    4

// EAPOL frame Protocol Version
#define	EAPOL_VER					1
#define	EAPOL_VER2					2

// EAPOL-KEY Descriptor Type
#define	WPA1_KEY_DESC				0xfe
#define WPA2_KEY_DESC               0x02

// Key Descriptor Version of Key Information
#define	DESC_TYPE_TKIP				1
#define	DESC_TYPE_AES				2

#define LEN_MSG1_2WAY               0x7f
#define MAX_LEN_OF_EAP_HS           256

#define LEN_MASTER_KEY				32  

// EAPOL EK, MK
#define LEN_EAP_EK					16
#define LEN_EAP_MICK				16
#define LEN_EAP_KEY					((LEN_EAP_EK)+(LEN_EAP_MICK))
// TKIP key related
#define LEN_PMKID					16
#define LEN_TKIP_EK					16
#define LEN_TKIP_RXMICK				8
#define LEN_TKIP_TXMICK				8
#define LEN_AES_EK					16
#define LEN_AES_KEY					LEN_AES_EK
#define LEN_TKIP_KEY				((LEN_TKIP_EK)+(LEN_TKIP_RXMICK)+(LEN_TKIP_TXMICK))
#define TKIP_AP_TXMICK_OFFSET		((LEN_EAP_KEY)+(LEN_TKIP_EK))
#define TKIP_AP_RXMICK_OFFSET		(TKIP_AP_TXMICK_OFFSET+LEN_TKIP_TXMICK)
#define TKIP_GTK_LENGTH				((LEN_TKIP_EK)+(LEN_TKIP_RXMICK)+(LEN_TKIP_TXMICK))
//#define LEN_PTK						((LEN_EAP_KEY)+(LEN_TKIP_KEY))


// RSN IE Length definition
//#define MAX_LEN_OF_RSNIE         	90
//#define MIN_LEN_OF_RSNIE         	8

//EAP Packet Type
#define	EAPPacket		0
#define	EAPOLStart		1
#define	EAPOLLogoff		2
#define	EAPOLKey		3
#define	EAPOLASFAlert	4
#define	EAPTtypeMax		5

#define	EAPOL_MSG_INVALID	0
#define	EAPOL_PAIR_MSG_1	1
#define	EAPOL_PAIR_MSG_2	2
#define	EAPOL_PAIR_MSG_3	3
#define	EAPOL_PAIR_MSG_4	4
#define	EAPOL_GROUP_MSG_1	5
#define	EAPOL_GROUP_MSG_2	6

#define PAIRWISEKEY					1
#define GROUPKEY					0

// Retry timer counter initial value
#define PEER_MSG1_RETRY_TIMER_CTR           0
#define PEER_MSG3_RETRY_TIMER_CTR           10
#define GROUP_MSG1_RETRY_TIMER_CTR          20

#ifdef CONFIG_AP_SUPPORT 
// WPA mechanism retry timer interval
#ifdef WIN_NDIS
#define PEER_MSG1_RETRY_EXEC_INTV           1024        		// 1024 ms = 1 sec
#define PEER_MSG3_RETRY_EXEC_INTV           5120        		// 5120 ms = 5 sec
#define GROUP_KEY_UPDATE_EXEC_INTV          10240
#else
#define PEER_MSG1_RETRY_EXEC_INTV           1000        		// 1 sec
#define PEER_MSG3_RETRY_EXEC_INTV           3000        		// 3 sec
#define GROUP_KEY_UPDATE_EXEC_INTV          1000				// 1 sec
#endif

#define ENQUEUE_EAPOL_START_TIMER			200					// 200 ms

// group rekey interval
#define TIME_REKEY                          0
#define PKT_REKEY                           1
#define DISABLE_REKEY                       2
#define MAX_REKEY                           2

#define MAX_REKEY_INTER                     0x3ffffff
#endif // CONFIG_AP_SUPPORT //	

#define EAPOL_START_DISABLE					0
#define EAPOL_START_PSK						1
#define EAPOL_START_1X							2

#define ROUND_UP(__x, __y) \
	(((ULONG)((__x)+((__y)-1))) & ((ULONG)~((__y)-1)))

#define	ADD_ONE_To_64BIT_VAR(_V)		\
{										\
	UCHAR	cnt = LEN_KEY_DESC_REPLAY;	\
	do									\
	{									\
		cnt--;							\
		_V[cnt]++;						\
		if (cnt == 0)					\
			break;						\
	}while (_V[cnt] == 0);				\
}
#define TX_EAPOL_BUFFER			1500

#if 0
// EAPOL Key Information definition within Key descriptor format
typedef	struct PACKED _KEY_INFO
{
#ifdef BIG_ENDIAN
	UCHAR	KeyAck:1;
    UCHAR	Install:1;
    UCHAR	KeyIndex:2;
    UCHAR	KeyType:1;
    UCHAR	KeyDescVer:3;
    UCHAR	Rsvd:3;
    UCHAR	EKD_DL:1;		// EKD for AP; DL for STA
    UCHAR	Request:1;
    UCHAR	Error:1;
    UCHAR	Secure:1;
    UCHAR	KeyMic:1;
#else
	UCHAR	KeyMic:1;
	UCHAR	Secure:1;
	UCHAR	Error:1;
	UCHAR	Request:1;
	UCHAR	EKD_DL:1;       // EKD for AP; DL for STA
	UCHAR	Rsvd:3;
	UCHAR	KeyDescVer:3;
	UCHAR	KeyType:1;
	UCHAR	KeyIndex:2;
	UCHAR	Install:1;
	UCHAR	KeyAck:1;
#endif	
}	KEY_INFO, *PKEY_INFO;

// EAPOL Key descriptor format
typedef	struct PACKED _KEY_DESCRIPTER
{
	UCHAR		Type;
	KEY_INFO	KeyInfo;
	UCHAR		KeyLength[2];
	UCHAR		ReplayCounter[LEN_KEY_DESC_REPLAY];
	UCHAR		KeyNonce[LEN_KEY_DESC_NONCE];
	UCHAR		KeyIv[LEN_KEY_DESC_IV];
	UCHAR		KeyRsc[LEN_KEY_DESC_RSC];
	UCHAR		KeyId[LEN_KEY_DESC_ID];
	UCHAR		KeyMic[LEN_KEY_DESC_MIC];
	UCHAR		KeyDataLen[2];	   
	UCHAR		KeyData[MAX_LEN_OF_RSNIE]; // ==> Michael, Sync with the latest definition
}	KEY_DESCRIPTER, *PKEY_DESCRIPTER;

typedef	struct PACKED _EAPOL_PACKET
{
	UCHAR	 			ProVer;
	UCHAR	 			ProType;
	UCHAR	 			Body_Len[2];
	KEY_DESCRIPTER		KeyDesc;
}	EAPOL_PACKET, *PEAPOL_PACKET;
#endif

//802.11i D10 page 83
#ifdef IAR_COMPILE
PACKED typedef struct  _GTK_ENCAP
#else
typedef struct PACKED _GTK_ENCAP
#endif
{
#ifndef BIG_ENDIAN
    UCHAR               Kid:2;
    UCHAR               tx:1;
    UCHAR               rsv:5;
    UCHAR               rsv1;
#else
    UCHAR               rsv:5;
    UCHAR               tx:1;
    UCHAR               Kid:2;
    UCHAR               rsv1;    	
#endif
    UCHAR               GTK[TKIP_GTK_LENGTH];
}   GTK_ENCAP, *PGTK_ENCAP;

#ifdef IAR_COMPILE
PACKED typedef struct  _KDE_ENCAP {
#else
  typedef struct PACKED _KDE_ENCAP {
#endif
    UCHAR               Type;
    UCHAR               Len;
    UCHAR               OUI[3];
    UCHAR               DataType;
    GTK_ENCAP      GTKEncap;
}   KDE_ENCAP, *PKDE_ENCAP;
// For WPA1
#ifdef IAR_COMPILE
PACKED typedef struct  _RSNIE {
#else
typedef struct PACKED _RSNIE {
#endif
    UCHAR   oui[4];
    USHORT  version;
    UCHAR   mcast[4];
    USHORT  ucount;
#ifdef IAR_COMPILE
    PACKED struct {
#else
    struct PACKED {
#endif
        UCHAR oui[4];
    }ucast[1];
} RSNIE, *PRSNIE;

// For WPA2
#ifdef IAR_COMPILE
PACKED typedef struct  _RSNIE2 {
#else
typedef struct PACKED _RSNIE2 {
#endif
    USHORT  version;
    UCHAR   mcast[4];
    USHORT  ucount;
#ifdef IAR_COMPILE
     PACKED struct  {
#else
    struct PACKED {
#endif
        UCHAR oui[4];
    }ucast[1];
} RSNIE2, *PRSNIE2;

// AKM Suite
#ifdef IAR_COMPILE
PACKED typedef struct  _RSNIE_AUTH {
#else
typedef struct PACKED _RSNIE_AUTH {
#endif
    USHORT acount;
#ifdef IAR_COMPILE
     PACKED struct  {
#else
    struct PACKED {
#endif
        UCHAR oui[4];
    }auth[1];
} RSNIE_AUTH,*PRSNIE_AUTH;

#ifdef IAR_COMPILE
PACKED typedef union  RSN_CAPABILITIES	{
#else
typedef union PACKED RSN_CAPABILITIES	{
#endif
#ifdef IAR_COMPILE
     PACKED struct  {
#else
    struct PACKED {
#endif
#ifdef BIG_ENDIAN
        USHORT		Rsvd:10;
        USHORT		GTKSA_R_Counter:2;
        USHORT		PTKSA_R_Counter:2;
        USHORT		No_Pairwise:1;
		USHORT		PreAuth:1;
#else
        USHORT		PreAuth:1;
		USHORT		No_Pairwise:1;
		USHORT		PTKSA_R_Counter:2;
		USHORT		GTKSA_R_Counter:2;
		USHORT		Rsvd:10;
#endif
	}	field;
	USHORT			word;
}	RSN_CAPABILITIES, *PRSN_CAPABILITIES;

#ifdef IAR_COMPILE
PACKED typedef struct  _EAP_HDR {
#else
typedef struct PACKED _EAP_HDR {
#endif
    UCHAR   ProVer;
    UCHAR   ProType;
    UCHAR   Body_Len[2];
    UCHAR   code;
    UCHAR   identifier;
    UCHAR   length[2]; // including code and identifier, followed by length-2 octets of data
} EAP_HDR, *PEAP_HDR;

#ifdef IAR_COMPILE
PACKED typedef struct  _KDE_HDR {
#else
typedef struct PACKED _KDE_HDR {
#endif
    UCHAR               Type;
    UCHAR               Len;
    UCHAR               OUI[3];
    UCHAR               DataType;
    UCHAR               octet[0];
}KDE_HDR, *PKDE_HDR;

#ifdef IAR_COMPILE
PACKED typedef struct _GTK_KDE {
#else
typedef struct PACKED _GTK_KDE {
#endif
    UCHAR               Kid:2;
    UCHAR               tx:1;
    UCHAR               rsv:5;
    UCHAR               rsv1;
    UCHAR               GTK[0];
}GTK_KDE, *PGTK_KDE;

// For supplicant state machine states. 802.11i Draft 4.1, p. 97
// We simplified it
typedef	enum	_WpaState
{
	SS_NOTUSE,				// 0
	SS_START,				// 1
	SS_WAIT_MSG_3,			// 2
	SS_WAIT_GROUP,			// 3
	SS_FINISH,  			// 4
	SS_KEYUPDATE,			// 5
}	WPA_STATE;

VOID RTMPConstructWEPIVHdr(
	IN UINT8 key_idx,
	IN UCHAR *pn,
	OUT UCHAR *iv_hdr);

//struct RTMP_ADAPTER;
BOOLEAN	RTMPSoftEncryptWEP(
	IN 		struct _RTMP_ADAPTER 	*pAd,
	IN 		PUCHAR			pIvHdr,
	IN 		PCIPHER_KEY		pKey,
	INOUT 	PUCHAR			pData,
	IN 		ULONG			DataByteCnt);


#endif
