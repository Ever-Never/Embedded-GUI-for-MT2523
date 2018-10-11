#ifndef __CMM_SECURITY_H__
#define __CMM_SECURITY_H__
#include "rt_config.h"
#include "typedef_iot_rom.h"

#define WTBL_UPDATE_SECURITY	0x7

enum
{
	RESET_WTBL_AND_SET 		= 1,
	SET_WTBL 				= 2,
	QUERY_WTBL 				= 3,
	RESET_ALL_WTBL			= 4,
};

/* WTBL Update - Security */
typedef struct _WTBL_SECURITY_T
{
    UINT_16 u2Tag;               /* Tag = 0x07 */
    UINT_16 u2Length;
    UINT_8  ucAddRemove;         /* 0: add, 1: remove */
    UINT_8  ucRkv;
    UINT_8  ucIkv;
    UINT_8  ucAlgorithmId;       /* refer to ENUM_CIPHER_SUIT_T256 */
    UINT_8  ucKeyId;
    UINT_8  ucKeyLen;
    UINT_8  aucReserved[2];
    UINT_8  aucKeyMaterial[32];
} WTBL_SECURITY_T, *P_WTBL_SECURITY_T;


extern VOID (*AsicAddRemoveKey)(
	IN RTMP_ADAPTER 	*pAd,
	IN UCHAR			isAdd,
	IN UCHAR			BssIdx,
	IN UCHAR			KeyIdx,
	IN UCHAR			Wcid,
	IN UCHAR 			KeyTabFlag,
	IN PCIPHER_KEY		pCipherKey,
	IN PUCHAR			PeerAddr);

WLAN_STATUS
wtblUpdateSecurity(
    UINT_8       ucWlanIdx,
    UINT_8       ucOperation,
    UINT_32      handle
    );

#endif /* __CMM_SECURITY_H__ */