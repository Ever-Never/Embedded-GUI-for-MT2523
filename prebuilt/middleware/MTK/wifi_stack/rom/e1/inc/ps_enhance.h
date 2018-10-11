/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
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
 ***************************************************************************/

/****************************************************************************
	Abstract:

***************************************************************************/

#ifndef	__PS_ENHANCE_H__
#define	__PS_ENHANCE_H__

#ifdef PS_ENHANCE

#define EN_POWER_SAVE_USER_CFG(_pAd)				( (_pAd)->CommonCfg.EPS_UserCfg )
#define EN_POWER_SAVE_USER_CFG_CAM_TEST(_pAd)		( (_pAd)->CommonCfg.EPS_UserCfg == Ndis802_11PowerModeCAM )
#define EN_POWER_SAVE_USER_CFG_SLEEP_TEST(_pAd)		( (_pAd)->CommonCfg.EPS_UserCfg != Ndis802_11PowerModeCAM )


void rtmp_read_ps_enhance_from_file(
	IN PRTMP_ADAPTER 		pAd,
	PSTRING 				tmpbuf,
	PSTRING 				pBuffer);

int EnhancePS_Init(IN PRTMP_ADAPTER pAd);

void EnhancePS_Release(IN PRTMP_ADAPTER pAd);

INT	Set_EnhancePS_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);
	
INT	Set_EPS_TraficTriggerAlive_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_EPS_TraficTriggerAsleep_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);	

void EnhancePS_RxCount(
	IN PRTMP_ADAPTER pAd, 
	UINT32	MPDUtotalByteCount);
	
void EnhancePS_TxCount(
	IN PRTMP_ADAPTER pAd, 
	UINT32	MPDUtotalByteCount);	

void EPS_TrafficLoadDecisionMaker(
	IN PRTMP_ADAPTER pAd);
	
void EPS_UserPowerSaveCfg(
	IN PRTMP_ADAPTER pAd, 
	IN BOOLEAN EPS_UserCfg);	
	
//void EPS_CoreDecisonMaker(
//	IN PRTMP_ADAPTER pAd);	
	
void EPS_CoreDecisonMaker(	IN PVOID SystemSpecific1, 
											IN PVOID FunctionContext, 
											IN PVOID SystemSpecific2, 
											IN PVOID SystemSpecific3);	

#endif // PS_ENHANCE //

#endif	// __PS_ENHANCE_H__ //



