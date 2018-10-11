
/*
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
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
#ifndef __CTP_ITE_IT7XXX_DOWNLOAD_H__
#define __CTP_ITE_IT7XXX_DOWNLOAD_H__

#ifdef MTK_CTP_ENABLE

#include "touch_panel_custom_config.h"
#ifdef CTP_IT7258_SUPPORTED

bool fnFirmwareDownload(void) ;
bool fnDownloadFirmware(uint32_t unFirmwareLength, void *pFirmware) ;
bool fnVerifyFirmware(uint32_t unFirmwareLength, void *pFirmware) ;
bool fnDownloadConfig(uint32_t unConfigLength, void *pConfig) ;
bool fnVerifyConfig(uint16_t unConfigLength, void *pConfig) ;
bool fnEnterFirmwareUpgradeMode(void) ;
bool fnExitFirmwareUpgradeMode(void) ;
bool fnGetFlashSize(uint16_t *pwFlashSize) ;
bool fnSetStartOffset(uint16_t wOffset) ;
bool fnFirmwareReinitialize(void) ;
bool fnWriteFlash(uint32_t unLength, void *pData) ;
bool fnAdvanceWriteFlash(uint32_t unLength, void *pData) ;
bool fnReadFlash(uint32_t unLength, uint32_t unOffset, void *pData) ;
bool fnAdvanceReadFlash(uint32_t unLength, uint32_t unStartOffset, void *pData) ;
bool fnWriteBuffer(uint8_t ucBuffeIndex, uint32_t unLength, void *pData) ;
bool fnReadBuffer(uint8_t ucBuffeIndex, uint32_t unLength, void *pData) ;
bool fnCompareResult(uint32_t unFileLength, uint32_t unStartOffset, void *pFile);

#endif /*CTP_IT7258_SUPPORTED*/
#endif /*MTK_CTP_ENABLE*/

#endif

