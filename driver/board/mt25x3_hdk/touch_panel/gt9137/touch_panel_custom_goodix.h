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

#ifndef _TOUCH_PANEL_CUSTOM_GOODIX_H_
#define _TOUCH_PANEL_CUSTOM_GOODIX_H_

#ifdef MTK_CTP_ENABLE

#include "ctp.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GOODIX_LCD_WIDTH                   (240)
#define GOODIX_LCD_HEIGHT                  (240)

/*software definition size*/
#define CTP_GOODIX_COORD_X_START           (0)
#define CTP_GOODIX_COORD_X_END             (GOODIX_LCD_WIDTH - 1)
#define CTP_GOODIX_COORD_Y_START           (0)
#define CTP_GOODIX_COORD_Y_END             (GOODIX_LCD_HEIGHT - 1)

/*calibration parameter, provide by vendor*/
#define CTP_GOODIX_COORD_VENDOR_X_START    (0)
#define CTP_GOODIX_COORD_VENDOR_X_END      (GOODIX_LCD_WIDTH - 1)
#define CTP_GOODIX_COORD_VENDOR_Y_START    (0)
#define CTP_GOODIX_COORD_VENDOR_Y_END      (GOODIX_LCD_HEIGHT -1)


#define GOODIX_CTP_CONFIG_LENGTH           106

#define GOODIX_PEN_MOVE_OFFSET             8
#define GOODIX_PEN_LONGTAP_OFFSET          10

/********* varible extern *************/


/******** funtion extern **************/
extern void touch_panel_goodix_custom_data_init(void);

#ifdef __cplusplus
}
#endif

#endif /*MTK_CTP_ENABLE*/

#endif /*_TOUCH_PANEL_CUSTOM_GOODIX_H_*/

