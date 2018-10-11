#ifndef	__WPA_ROM_H__
#define	__WPA_ROM_H__
#include "rt_config.h"

typedef void (*PWPASENDFUNC) (
	PUCHAR          pHeader802_3,
    UINT            HdrLen,
	PUCHAR          pData,
    UINT            DataLen,
    BOOLEAN			is4wayFrame);


typedef VOID (*PWPA2_PAIRMSG1_ACTION)(
	IN  WPACfg_t		*pStaWPACfg,	
	IN  UCHAR 			*Msg,
	IN	UCHAR			*CurrentAddress,
	IN	UCHAR			*Bssid);

typedef VOID (*PWPA2_PAIRMSG3_ACTION)(
	IN  WPACfg_t		*pStaWPACfg,
	IN	ApRSNIE_t		*pApRSNIE,
	IN  UCHAR			*Msg,
	IN	UCHAR			*CurrentAddress,
	IN	UCHAR			*Bssid);

typedef VOID (*PWPA_PAIRMSG1_ACTION)(
	IN  WPACfg_t		*pStaWPACfg,	
	IN  UCHAR 			*Msg,
	IN	UCHAR			*CurrentAddress,
	IN	UCHAR			*Bssid);

typedef VOID (*PWPA_PAIRMSG3_ACTION)(
	IN  WPACfg_t		*pStaWPACfg,
	IN	ApRSNIE_t		*pApRSNIE,
	IN	UCHAR			*Msg,
	IN	UCHAR			*CurrentAddress,
	IN	UCHAR			*Bssid);

typedef VOID (*PWPA_GROUPMSG1_ACTION)(
	IN  WPACfg_t		*pStaWPACfg,
	IN  ApRSNIE_t		*pApRSNIE,
	IN	UCHAR			*Msg,
	IN	UCHAR			*CurrentAddress,
	IN	UCHAR			*Bssid);


typedef BOOLEAN (*PWPA_PARSE_KEYDATA)(
	IN  WPACfg_t		*pStaWPACfg,
	IN	ApRSNIE_t		*pApRSNIE,    
	IN  PUCHAR          pKeyData,
	IN  UCHAR           KeyDataLen,
	IN	UCHAR			bPairewise);


typedef void (*PWPA_EAPOL_SEND_FUNC) (
	PUCHAR          pHeader802_3,
    UINT            HdrLen,
	PUCHAR          pData,
    UINT            DataLen,
    BOOLEAN			is4wayFrame);


typedef void (*PWPA_ASIC_KEY_FUNC)(
	IN  WPACfg_t		*pStaWPACfg,
	IN	UCHAR			KeyIdx,
	IN	UCHAR			Wcid);


typedef void (*PWPA_KEY_DONE_FUNC)(void);


typedef struct _WPA_CALLBACK_FUNC{
	PWPA_PAIRMSG1_ACTION	wpa_pairmsg1_action;
	PWPA_PAIRMSG3_ACTION	wpa_pairmsg3_action;
	PWPA_GROUPMSG1_ACTION	wpa_groupmsg1_action;
	PWPA_PARSE_KEYDATA		wpa_parse_keydata;
	PWPA2_PAIRMSG1_ACTION	wpa2_pairmsg1_action;
	PWPA2_PAIRMSG3_ACTION	wpa2_pairmsg3_action;
	PWPA_ASIC_KEY_FUNC		wpa_asic_sharedkey_updata;
	PWPA_KEY_DONE_FUNC		wpa_group_key_done_action;
	PWPA_KEY_DONE_FUNC		wpa2_key_done_action;
} WPA_CALLBACK_FUNC;


void WpaAsicSharedKeyUpdate(
	IN  WPACfg_t		*pStaWPACfg,
	IN	UCHAR			KeyIdx,
	IN	UCHAR			Wcid);


VOID    Test_RTMPToWirelessSta(
	IN  PUCHAR          pHeader802_3,
    IN  UINT            HdrLen,
	IN  PUCHAR          pData,
    IN  UINT            DataLen,
    IN	BOOLEAN			is4wayFrame);

#endif 
