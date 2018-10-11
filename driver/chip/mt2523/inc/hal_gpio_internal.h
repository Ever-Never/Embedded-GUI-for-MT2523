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

#ifndef __HAL_GPIO_INTERNAL_H__
#define __HAL_GPIO_INTERNAL_H__

#include "hal_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_GPIO_MODULE_ENABLED

#define GPIO_REG_ONE_BIT_SET_CLR            0x1
#define GPIO_REG_TWO_BIT_SET_CLR            0x3
#define GPIO_REG_FOUR_BIT_SET_CLR           0xF
#define GPIO_MODE_MIN_NUMBER                0       /*  minimum function index of GPIO  */
#define GPIO_MODE_MAX_NUMBER                16      /*  maximum function index of GPIO  */
#define GPIO_REG_CTRL_PIN_NUM_OF_32         32      /*  pin munber controlled by one register is 32  */
#define GPIO_MODE_FUNCTION_CTRL_BITS        4       /*  mode function is controlled by 4 bits  */
#define GPIO_TDSEL_FUNCTION_CTRL_BITS       2       /*  TDSEL function is controlled by 2 bits  */
#define GPIO_DRV_FUNCTION_CTRL_BITS         2       /*  DRV function is controlled by 2 bits  */
#define GPIO_DIR_REG_CTRL_PIN_NUM           32      /*  pin munber controlled by direction register is 32  */
#define GPIO_MODE_REG_CTRL_PIN_NUM          8       /*  pin munber controlled by mode register is 8  */
#define GPIO_DIN_REG_CTRL_PIN_NUM           32      /*  pin munber controlled by inputd data register is 32  */
#define GPIO_DOUT_REG_CTRL_PIN_NUM          32      /*  pin munber controlled by output data register is 32  */
#ifdef HAL_GPIO_FEATURE_INVERSE
#define GPIO_DINV_REG_CTRL_PIN_NUM          32      /*  pin munber controlled by inputd data inverse register is 32  */
#endif
#define GPIO_PULL_CTRL_REG_CTRL_PIN_NUM     32      /*  pin munber controlled by pull control register is 32  */
#ifdef HAL_GPIO_FEATURE_PUPD
#define GPIO_PUPD_CTRL_REG_CTRL_PIN_NUM     32      /*  pin munber controlled by PUPD control register is 32  */
#endif
#define GPIO_TDSEL_REG_CTRL_PIN_NUM         16      /*  pin munber controlled by TDSEL register is 16  */
#define GPIO_IES_REG_CTRL_PIN_NUM           32      /*  pin munber controlled by IES register is 32  */
#define GPIO_SR_REG_CTRL_PIN_NUM            32      /*  pin munber controlled by SR register is 32  */
#define GPIO_DRV_REG_CTRL_PIN_NUM           16      /*  pin munber controlled by DRV register is 16  */

#ifdef __cplusplus
}
#endif

#endif /*HAL_GPIO_MODULE_ENABLED*/
#endif /*__HAL_GPIO_INTERNAL_H__*/
