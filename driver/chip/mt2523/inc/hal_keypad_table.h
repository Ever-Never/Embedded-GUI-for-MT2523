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

/*------------------------------------------------------------------------------
 * Below this line, DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef __HAL_KEYPAD_TABLE_H__
#define __HAL_KEYPAD_TABLE_H__

#ifdef __cplusplus
extern "C"
{
#endif


#define   DEVICE_KEY_0             0
#define   DEVICE_KEY_1             1
#define   DEVICE_KEY_2             2
#define   DEVICE_KEY_3             3
#define   DEVICE_KEY_4             4
#define   DEVICE_KEY_5             5
#define   DEVICE_KEY_6             6
#define   DEVICE_KEY_7             7
#define   DEVICE_KEY_8             8
#define   DEVICE_KEY_9             9
#define   DEVICE_KEY_STAR          10
#define   DEVICE_KEY_HASH          11
#define   DEVICE_KEY_VOL_UP        12
#define   DEVICE_KEY_VOL_DOWN      13
#define   DEVICE_KEY_UP            14
#define   DEVICE_KEY_DOWN          15
#define   DEVICE_KEY_LEFT          16
#define   DEVICE_KEY_RIGHT         17
#define   DEVICE_KEY_MENU          18/*WAP, IP*/
#define   DEVICE_KEY_FUNCTION      19/*vol_c*//*Camera*/
#define   DEVICE_KEY_SK_LEFT       20
#define   DEVICE_KEY_SK_RIGHT      21
#define   DEVICE_KEY_SEND          22
#define   DEVICE_KEY_END           23
#define   DEVICE_KEY_POWER         24
#define   DEVICE_KEY_CLEAR         25/*soft_c*/
#define   DEVICE_KEY_EXT_FUNC1     26/*for  "go to xxx AP(ex, Calendar) shortcut"*/
#define   DEVICE_KEY_EXT_FUNC2     27/*for  "go to xxx AP(ex, Calendar) shortcut"*/
#define   DEVICE_KEY_MP3_PLAY_STOP 28
#define   DEVICE_KEY_MP3_FWD       29
#define   DEVICE_KEY_MP3_BACK      30
#define   DEVICE_KEY_EXT_A         31
#define   DEVICE_KEY_EXT_B         32
#define   DEVICE_KEY_A             33
#define   DEVICE_KEY_B             34
#define   DEVICE_KEY_C             35
#define   DEVICE_KEY_D             36
#define   DEVICE_KEY_E             37
#define   DEVICE_KEY_F             38
#define   DEVICE_KEY_G             39
#define   DEVICE_KEY_H             40
#define   DEVICE_KEY_I             41
#define   DEVICE_KEY_J             42
#define   DEVICE_KEY_K             43
#define   DEVICE_KEY_L             44
#define   DEVICE_KEY_M             45
#define   DEVICE_KEY_N             46
#define   DEVICE_KEY_O             47
#define   DEVICE_KEY_P             48
#define   DEVICE_KEY_Q             49
#define   DEVICE_KEY_R             50
#define   DEVICE_KEY_S             51
#define   DEVICE_KEY_T             52
#define   DEVICE_KEY_U             53
#define   DEVICE_KEY_V             54
#define   DEVICE_KEY_W             55
#define   DEVICE_KEY_X             56
#define   DEVICE_KEY_Y             57
#define   DEVICE_KEY_Z             58
#define   DEVICE_KEY_ENTER         59
#define   DEVICE_KEY_SPACE         60
#define   DEVICE_KEY_TAB           61
#define   DEVICE_KEY_DEL           62
#define   DEVICE_KEY_ALT           63
#define   DEVICE_KEY_CTRL          64
#define   DEVICE_KEY_WIN           65
#define   DEVICE_KEY_SHIFT         66
#define   DEVICE_KEY_QUESTION      67
#define   DEVICE_KEY_PERIOD        68    /* . */
#define   DEVICE_KEY_COMMA         69    /* , */
#define   DEVICE_KEY_EXCLAMATION   70    /* ! */
#define   DEVICE_KEY_APOSTROPHE    71    /* ' */
#define   DEVICE_KEY_AT            72    /* @ */
#define   DEVICE_KEY_SEND2         73

//below keys are define for qwerty
#define   DEVICE_KEY_BACKQUOTE     74    /* ` */
#define   DEVICE_KEY_DASH          75    /* - */
#define   DEVICE_KEY_EQUAL         76    /* = */
#define   DEVICE_KEY_BACKSPACE     77    /* <- */
#define   DEVICE_KEY_OPEN_PARENS   78    /* ( */
#define   DEVICE_KEY_CLOSE_PARENS  79    /* ) */
#define   DEVICE_KEY_OPEN_SQUARE   80    /* [ */
#define   DEVICE_KEY_CLOSE_SQUARE  81    /* ] */
#define   DEVICE_KEY_OPEN_BRACE    82    /* { */
#define   DEVICE_KEY_CLOSE_BRACE   83    /* } */
#define   DEVICE_KEY_BACKSLASH     84    /* \ */
#define   DEVICE_KEY_SEMICOLON     85    /* ; */
#define   DEVICE_KEY_SLASH         86    /* / */
#define   DEVICE_KEY_DOLLAR        87    /* $ */
#define   DEVICE_KEY_PERCENT       88    /* % */
#define   DEVICE_KEY_CARET         89    /* ^ */
#define   DEVICE_KEY_AND           90    /* & */
#define   DEVICE_KEY_QUOTE         91    /* " */
#define   DEVICE_KEY_PLUS          92    /* + */
#define   DEVICE_KEY_LESS_THAN     93    /* < */
#define   DEVICE_KEY_GREAT_THAN    94    /* > */
#define   DEVICE_KEY_UNDERSCORE    95    /* _ */
#define   DEVICE_KEY_PIPE          96    /* | */
#define   DEVICE_KEY_TILDE         97    /* ~ */
#define   DEVICE_KEY_CAPS_LOCK     98
#define   DEVICE_KEY_NUM_LOCK      99

#define   DEVICE_KEY_FN            100
#define   DEVICE_KEY_SYMBOL        101
#define   DEVICE_KEY_EMAIL         102
#define   DEVICE_KEY_MESSAGE       103
#define   DEVICE_KEY_CAMERA        104
#define   DEVICE_KEY_ESC           105
#define   DEVICE_KEY_QWERTY_MENU   106
#define   DEVICE_KEY_OK            107

#define   DEVICE_KEY_F1            111
#define   DEVICE_KEY_F2            112
#define   DEVICE_KEY_F3            113
#define   DEVICE_KEY_F4            114
#define   DEVICE_KEY_F5            115
#define   DEVICE_KEY_F6            116
#define   DEVICE_KEY_F7            117
#define   DEVICE_KEY_F8            118
#define   DEVICE_KEY_F9            119
#define   DEVICE_KEY_F10           120
#define   DEVICE_KEY_F11           121
#define   DEVICE_KEY_F12           122
#define   DEVICE_KEY_BACK          123
#define   DEVICE_KEY_HOME          124
#define   MAX_DEVICE_KEYS          125


#define   DEVICE_KEY_NONE          0xFE

#ifdef __cplusplus
}
#endif

#endif //_HAL_KEYPAD_TABLE_H

