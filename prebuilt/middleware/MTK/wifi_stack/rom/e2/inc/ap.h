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
    ap.h

    Abstract:
    Miniport generic portion header file

    Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
*/
#ifndef __AP_H__
#define __AP_H__

#ifdef DOT11R_FT_SUPPORT
#include "ft_cmm.h"
#endif /* DOT11R_FT_SUPPORT */



/* ============================================================= */
/*      Common definition */
/* ============================================================= */
#if 0
#define MBSS_VLAN_INFO_GET(__pAd, __VLAN_VID, __VLAN_Priority, __func_idx) \
{																		\
	if ((__func_idx < __pAd->ApCfg.BssidNum) &&					\
		(__func_idx < HW_BEACON_MAX_NUM) &&						\
		(__pAd->ApCfg.MBSSID[__func_idx].wdev.VLAN_VID != 0))			\
	{																	\
		__VLAN_VID = __pAd->ApCfg.MBSSID[__func_idx].wdev.VLAN_VID;	\
		__VLAN_Priority = __pAd->ApCfg.MBSSID[__func_idx].wdev.VLAN_Priority; \
	}																	\
}

#define WDEV_VLAN_INFO_GET(__pAd, __VLAN_VID, __VLAN_Priority, __wdev) \
{																		\
	if ((__wdev->VLAN_VID != 0))			\
	{																	\
		__VLAN_VID = __wdev->VLAN_VID;	\
		__VLAN_Priority = __wdev->VLAN_Priority; \
	}																	\
}
#endif

typedef struct _AUTH_FRAME_INFO{
	UCHAR addr1[MAC_ADDR_LEN];
	UCHAR addr2[MAC_ADDR_LEN];
	USHORT auth_alg;
	USHORT auth_seq;
	USHORT auth_status;
	CHAR Chtxt[CIPHER_TEXT_LEN];
}AUTH_FRAME_INFO;


/* ============================================================= */
/*      Function Prototypes */
/* ============================================================= */


#endif  /* __AP_H__ */


