/* Copyright Statement:
*
* (C) 2005-2017  MediaTek Inc. All rights reserved.
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
* Without the prior written permission of MediaTek and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
* You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
* if you have agreed to and been bound by the applicable license agreement with
* MediaTek ("License Agreement") and been granted explicit permission to do so within
* the License Agreement ("Permitted User").  If you are not a Permitted User,
* please cease any access or use of MediaTek Software immediately.
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
* ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
* WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*/

#ifndef _HAL_PATCH_H
#define _HAL_PATCH_H

#include "rt_config.h"

#define LP_HWIS0R			(WIFI_INT_BASE + 0x003C)
#define LP_HWIE0R			(WIFI_INT_BASE + 0x0040)
#define LP_HWIS1R			(WIFI_INT_BASE + 0x0044)
#define LP_HWIE1R			(WIFI_INT_BASE + 0x0048)
#define LP_HWIS2R			(WIFI_INT_BASE + 0x004C)
#define LP_HWIE2R			(WIFI_INT_BASE + 0x0050)
#define LP_HWIS3R			(WIFI_INT_BASE + 0x0054)
#define LP_HWIE3R			(WIFI_INT_BASE + 0x0058)
#define LP_HWIS4R			(WIFI_INT_BASE + 0x004C)
#define LP_HWIE4R			(WIFI_INT_BASE + 0x0060)



#define TXM_ALL_TX_Q_BITMAP \
    BITS(TX_Q_AC0, TX_Q_AC14)

typedef struct _TX_AC_PARAM_T
{
    UINT_8 ucAcNum;
    UINT_8 ucVaildBit;
    UINT_8 ucAifs;
    UINT_8 ucWinMin;
    UINT_16 u2WinMax;
    UINT_16 u2Txop;
}TX_AC_PARAM_T,*P_TX_AC_PARAM_T;
#define MASK_AIFS_SET   BIT(0)
#define MASK_WINMIN_SET BIT(1)
#define MASK_WINMAX_SET BIT(2)
#define MASK_TXOP_SET   BIT(3)

#if 0
VOID
txmStartQueue(
    IN UINT_32 u4QueIdxBitmap,
    IN UINT_8 ucOwnMacIdx
    );
VOID
txmStopQueue(
    IN UINT_32 u4QueIdxBitmap,
    IN UINT_8 ucOwnMacIdx,
    IN BOOLEAN fgAbort
    );
#endif

HAL_STATUS halTxConfigACAifs(
		IN UINT_8 ucQueueIdx,
		IN UINT_8 ucAifs
		);
HAL_STATUS halTxConfigACWinMin(
    IN UINT_8 ucQueueIdx,
    IN UINT_8 ucWinMin
    );

HAL_STATUS halTxConfigACWinMax(
    IN UINT_8 ucQueueIdx,
    IN UINT_16 u2WinMax
    );
HAL_STATUS halTxConfigACTxop(
    IN UINT_8 ucQueueIdx,
    IN UINT_16 u2Txop
    );


extern VOID
__original_halSeTbttInt (
    UINT_8 eTsfId,
    UINT_8 fgEnable
    );

extern VOID
(*halSeTbttInt) (
    UINT_8 eTsfId,
    UINT_8 fgEnable
    ) ;

extern VOID
__original___direct_access_halSeTbttInt (
    UINT_8 eTsfId,
    UINT_8 fgEnable
    );

extern VOID
(*__direct_access_halSeTbttInt) (
    UINT_8 eTsfId,
    UINT_8 fgEnable
    ) ;

extern HAL_STATUS
__original_halSetPreTbttIntPin(
    IN UINT_8 ucBssidIdx,
    IN BOOL fgEnable
    );
extern HAL_STATUS
(*halSetPreTbttIntPin)(
    IN UINT_8 ucBssidIdx,
    IN BOOL fgEnable
    );
extern HAL_STATUS
__original___direct_access_halSetPreTbttIntPin(
    IN UINT_8 ucBssidIdx,
    IN BOOL fgEnable
    );
extern HAL_STATUS
(*__direct_access_halSetPreTbttIntPin)(
    IN UINT_8 ucBssidIdx,
    IN BOOL fgEnable
    ) ;

#endif

