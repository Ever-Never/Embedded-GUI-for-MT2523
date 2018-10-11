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

#ifndef __HAL_DISPLAY_PWM_INTERNAL_H__
#define __HAL_DISPLAY_PWM_INTERNAL_H__

#ifdef HAL_DISPLAY_PWM_MODULE_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    uint32_t	value;
    struct {
        uint32_t PWM_EN 						: 1; 	/*! Bit 0 */
        uint32_t reserved						: 31;	/*! reserved */
    } field;
} DISP_PWM_EN_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t PWM_RST						: 1; 	/*! Bit 0 */
        uint32_t reserved						: 31;	/*! reserved */
    } field;
} DISP_PWM_RST_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t PWM_COMMIT 					: 1; 	/*! Bit 0 */
        uint32_t reserved						: 31;	/*! reserved */
    } field;
} DISP_PWM_COMMIT_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t PWM_AUTO_MODE					: 1; 	/*! Bit 0 */
        uint32_t reserved2						: 1; 	/*! reserved */
        uint32_t PWM_POLARITY					: 1; 	/*! Bit 2 */
        uint32_t PWM_FRAME_SYNC 				: 1; 	/*! Bit 3 */
        uint32_t PWM_UPDATE_SEL 				: 2; 	/*! Bit 4..5 */
        uint32_t TE_EDGE_SEL					: 1; 	/*! Bit 6 */
        uint32_t PWM_DONT_UPDATE				: 1; 	/*! Bit 7 */
        uint32_t reserved1						: 8; 	/*! reserved */
        uint32_t PWM_CLKDIV 					: 10;	/*! Bit 16..25 */
        uint32_t reserved						: 6;		/*! reserved */
    } field;
} DISP_PWM_CON_0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t PWM_PERIOD 					: 12;	/*! Bit 0..11 */
        uint32_t reserved1						: 4; 	/*! reserved */
        uint32_t PWM_HIGH_WIDTH 				: 13;	/*! Bit 16..28 */
        uint32_t reserved						: 3;		/*! reserved */
    } field;
} DISP_PWM_CON_1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t PWM_GRADUAL_EN 				: 1; 	/*! Bit 0 */
        uint32_t reserved2						: 7; 	/*! reserved */
        uint32_t PWM_GRADUAL_FRAME				: 6; 	/*! Bit 8..13 */
        uint32_t reserved1						: 2; 	/*! reserved */
        uint32_t PWM_GRADUAL_STEP				: 8; 	/*! Bit 16..23 */
        uint32_t reserved						: 8;		/*! reserved */
    } field;
} DISP_PWM_GRADUAL_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t PWM_HIGH_WIDTH_GRAD			: 13;	/*! Bit 0..12 */
        uint32_t reserved						: 19;	/*! reserved */
    } field;
} DISP_PWM_GRADUAL_RO_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DOUBLE_BUFFER_DIS				: 1; 	/*! Bit 0 */
        uint32_t PWM_DOUBLE_BUFFER_DIS			: 1; 	/*! Bit 1 */
        uint32_t reserved						: 30;	/*! reserved */
    } field;
} DISP_PWM_DEBUG_REGISTER_T;

typedef struct {
    DISP_PWM_EN_REGISTER_T			DISP_PWM_EN_REGISTER;			// 0x00
    DISP_PWM_RST_REGISTER_T			DISP_PWM_RST_REGISTER;			// 0x04
    DISP_PWM_COMMIT_REGISTER_T		DISP_PWM_COMMIT_REGISTER;		// 0x08
    uint32_t							reseved;
    DISP_PWM_CON_0_REGISTER_T		DISP_PWM_CON_0_REGISTER;		// 0x10
    DISP_PWM_CON_1_REGISTER_T		DISP_PWM_CON_1_REGISTER;		// 0x14
    DISP_PWM_GRADUAL_REGISTER_T		DISP_PWM_GRADUAL_REGISTER;		// 0x18
    DISP_PWM_GRADUAL_RO_REGISTER_T	DISP_PWM_GRADUAL_RO_REGISTER;	// 0x1C
    DISP_PWM_DEBUG_REGISTER_T		DISP_PWM_DEBUG_REGISTER;		// 0x20
} DISP_PWM_REGISTER_T;

hal_display_pwm_status_t display_pwm_init(hal_display_pwm_clock_t clock);
hal_display_pwm_status_t display_pwm_deinit(void);
hal_display_pwm_status_t display_pwm_set_clock(hal_display_pwm_clock_t source_clock);
hal_display_pwm_status_t display_pwm_set_duty(uint8_t percent);
hal_display_pwm_status_t display_pwm_set_width(uint32_t width);

#ifdef __cplusplus
}
#endif

#endif

#endif //__HAL_DISPLAY_PWM_INTERNAL_H__

