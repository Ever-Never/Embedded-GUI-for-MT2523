/* Copyright Statement:
 *
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

#ifndef __TOUCH_PANEL_CUSTOM_ITE_H__
#define __TOUCH_PANEL_CUSTOM_ITE_H__

#ifdef MTK_CTP_ENABLE

#include "ctp.h"
#include "hal_eint.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*resolution ratio configuration */
#define ITE_LCD_WIDTH                   (320)
#define ITE_LCD_HEIGHT                  (320)

/*software definition size*/
#define CTP_ITE_COORD_X_START           (0)
#define CTP_ITE_COORD_X_END             (ITE_LCD_WIDTH - 1)
#define CTP_ITE_COORD_Y_START           (0)
#define CTP_ITE_COORD_Y_END             (ITE_LCD_HEIGHT - 1)

/*calibration parameter, provide by vendor*/
#define CTP_ITE_COORD_VENDOR_X_START    (0)
#define CTP_ITE_COORD_VENDOR_X_END      (ITE_LCD_WIDTH - 1)
#define CTP_ITE_COORD_VENDOR_Y_START    (0)
#define CTP_ITE_COORD_VENDOR_Y_END      (ITE_LCD_HEIGHT - 1)


#define ITE_PEN_MOVE_OFFSET             8
#define ITE_PEN_LONGTAP_OFFSET          10

/*enable ctp atuomatic update firmware*/
#define CTP_ITE_UPGRADE_FIRMWARE

/*eint trriger definition*/
#define ITE_EINT_TRIGGER_LEVEL_LOW      HAL_EINT_LEVEL_LOW
#define ITE_EINT_TRIGGER_LEVEL_HIGH     HAL_EINT_LEVEL_HIGH

#define CTP_ITE_EINT_TRIGGER_TYPE       HAL_EINT_LEVEL_LOW
/********* varible extern *************/

/******** funtion extern **************/
extern void touch_panel_ite_custom_data_init(void);

#ifdef __cplusplus
}
#endif

#endif /*MTK_CTP_ENABLE*/

#endif /*__TOUCH_PANEL_CUSTOM_ITE_H__*/

