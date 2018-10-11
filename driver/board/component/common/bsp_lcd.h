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

#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__

//#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_display_lcd.h"
#include "hal_display_lcd_internal.h"

/* definition of LCM data output format */
#define LCM_8BIT_8_BPP_RGB332_1     0x00  /* RRRGGBB */
#define LCM_8BIT_8_BPP_RGB332_2     0x01  /* BBGGGRR */
#define LCM_8BIT_12_BPP_RGB444_1    0x08  /* RRRRGGGG, BBBBRRRR, GGGGBBBB */
#define LCM_8BIT_12_BPP_RGB444_2    0x0B  /* GGGGRRRR, RRRRBBBB, BBBBGGGG */
#define LCM_8BIT_16_BPP_RGB565_1    0x10  /* RRRRRGGG, GGGBBBBB */
#define LCM_8BIT_16_BPP_RGB565_2    0x12  /* GGGBBBBB, RRRRRGGG */
#define LCM_8BIT_16_BPP_BGR565_1    0x11  /* BBBBBGGG, GGGRRRRR */
#define LCM_8BIT_16_BPP_BGR565_2    0x13  /* GGGRRRRR, BBBBBGGG */
#define LCM_8BIT_18_BPP_RGB666_1    0x18  /* RRRRRRXX, GGGGGGXX, BBBBBBXX */
#define LCM_8BIT_18_BPP_RGB666_2    0x1C  /* XXRRRRRR, XXGGGGGG, XXBBBBBB */
#define LCM_8BIT_24_BPP_RGB888_1    0x20  /* RRRRRRRR, GGGGGGGG, BBBBBBBB */
#define LCM_16BIT_8_BPP_RGB332_1    0x40  /* RRRGGGBBRRRGGGBB, MSB first*/
#define LCM_16BIT_8_BPP_RGB332_2    0x42  /* RRRGGGBBRRRGGGBB, LSB first*/
#define LCM_16BIT_8_BPP_RGB332_3    0x41  /* BBGGGRRRBBGGGRRR, MSB first */
#define LCM_16BIT_8_BPP_RGB332_4    0x43  /* BBGGGRRRBBGGGRRR, LSB first */
#define LCM_16BIT_12_BPP_RGB444_1   0x4C  /* XXXXRRRRGGGGBBBB */
#define LCM_16BIT_12_BPP_RGB444_2   0x4D  /* XXXXBBBBGGGGRRRR */
#define LCM_16BIT_12_BPP_RGB444_3   0x48  /* RRRRGGGGBBBBXXXX */
#define LCM_16BIT_12_BPP_RGB444_4   0x49  /* BBBBGGGGRRRRXXXX */
#define LCM_16BIT_16_BPP_RGB565_1   0x50  /* RRRRRGGGGGGBBBBB */
#define LCM_16BIT_16_BPP_RGB565_2   0x52  /* GGGBBBBBRRRRRGGG */
#define LCM_16BIT_16_BPP_BGR565_1   0x51  /* BBBBBGGGGGGRRRRR */
#define LCM_16BIT_16_BPP_BGR565_2   0x53  /* GGGRRRRRBBBBBGGG */
#define LCM_16BIT_18_BPP_RGB666_1   0x5C  /* XXXXRRRRRRGGGGGG, XXXXBBBBBBRRRRRR, XXXXGGGGGGBBBBBB */
#define LCM_16BIT_18_BPP_RGB666_2   0x5F  /* XXXXGGGGGGRRRRRR, XXXXRRRRRRBBBBBB, XXXXBBBBBBGGGGGG */
#define LCM_16BIT_18_BPP_RGB666_3   0x58  /* RRRRRRGGGGGGXXXX, BBBBBBRRRRRRXXXX, GGGGGGBBBBBBXXXX */
#define LCM_16BIT_18_BPP_RGB666_4   0x5B  /* GGGGGGRRRRRRXXXX, RRRRRRBBBBBBXXXX, BBBBBBGGGGGGXXXX */
#define LCM_16BIT_24_BPP_RGB888_1   0x60  /* RRRRRRRRGGGGGGGG, BBBBBBBBRRRRRRRR, GGGGGGGGBBBBBBBB */
#define LCM_16BIT_24_BPP_RGB888_2   0x63  /* GGGGGGGGRRRRRRRR, RRRRRRRRBBBBBBBB, BBBBBBBBGGGGGGGG */

#define LCM_9BIT_8_BPP_RGB332_1     0x80  /* RRRGGGBBX */
#define LCM_9BIT_8_BPP_RGB332_2     0x81  /* BBGGGRRRX */
#define LCM_9BIT_12_BPP_RGB444_1    0x88  /* RRRRGGGGX, BBBBRRRRX, GGGGBBBBX */
#define LCM_9BIT_12_BPP_RGB444_2    0x8B  /* GGGGRRRRX, RRRRBBBBX, BBBBGGGGX */
#define LCM_9BIT_16_BPP_RGB565_1    0x90  /* RRRRRGGGX, GGGBBBBBX */
#define LCM_9BIT_16_BPP_RGB565_2    0x93  /* GGGRRRRRX, BBBBBGGGX */
#define LCM_9BIT_18_BPP_RGB666_1    0x98  /* RRRRRRGGG, GGGBBBBBB */
#define LCM_9BIT_18_BPP_RGB666_2    0x9B  /* GGGRRRRRR, BBBBBBGGG */
#define LCM_9BIT_24_BPP_RGB888_1    0xA0  /* RRRRRRRRX, GGGGGGGGX, BBBBBBBBX */
#define LCM_18BIT_8_BPP_RGB332_1    0xC0  /* RRRGGGBBRRRGGGBBXX, MSB first */
#define LCM_18BIT_8_BPP_RGB332_2    0xC2  /* RRRGGGBBRRRGGGBBXX, LSB first */
#define LCM_18BIT_8_BPP_RGB332_3    0xC1  /* BBGGGRRRBBGGGRRRXX, MSB first */
#define LCM_18BIT_8_BPP_RGB332_4    0xC3  /* BBGGGRRRBBGGGRRRXX, LSB first */
#define LCM_18BIT_12_BPP_RGB444_1   0xCC  /* XXXXXXRRRRGGGGBBBB */
#define LCM_18BIT_12_BPP_RGB444_2   0xCD  /* XXXXXXBBBBGGGGRRRR */
#define LCM_18BIT_12_BPP_RGB444_3   0xC8  /* RRRRGGGGBBBBXXXXXX */
#define LCM_18BIT_12_BPP_RGB444_4   0xC9  /* BBBBGGGGRRRRXXXXXX */
#define LCM_18BIT_16_BPP_RGB565_1   0xD0  /* RRRRRGGGGGGBBBBBXX */
#define LCM_18BIT_16_BPP_RGB565_2   0xD1  /* BBBBBGGGGGGRRRRRXX */
#define LCM_18BIT_18_BPP_RGB666_1   0xD8  /* RRRRRRGGGGGGBBBBBB */
#define LCM_18BIT_18_BPP_RGB666_2   0xD9  /* BBBBBBGGGGGGRRRRRR */
#define LCM_18BIT_24_BPP_RGB888_1   0xE0  /* RRRRRRRRGGGGGGGGXX, BBBBBBBBRRRRRRRRXX, GGGGGGGGBBBBBBBBXX */
#define LCM_18BIT_24_BPP_RGB888_2   0xE3  /* GGGGGGGGRRRRRRRRXX, RRRRRRRRBBBBBBBBXX, BBBBBBBBGGGGGGGGXX */

///TODO for 24 bit interface

typedef enum {
    LCM_IOCTRL_QUERY__FRAME_RATE, /**< Storage type: uint32_t
                                      This attribute is mandatory.
                                      Query the frame rate of LCM HW.
                                      Return status: LCM_IOCTRL_OK: when support.
                                                     LCM_IOCTRL_NOT_SUPPORT: shall not happen.
                                      */

    LCM_IOCTRL_QUERY__BACK_PORCH,  /**< Storage type: uint32
                                       This attribute is mandatory.
                                       Query the back porch of LCM HW.
                                       Return status: LCM_IOCTRL_OK: when support.
                                                      LCM_IOCTRL_NOT_SUPPORT: shall not happen.
                                       */

    LCM_IOCTRL_QUERY__FRONT_PORCH, /**< Storage type: uint32
                                       This attribute is optional.
                                       Query the front porch of LCM HW.
                                       Return status: LCM_IOCTRL_OK: when support.
                                                      LCM_IOCTRL_NOT_SUPPORT: shall not happen.
                                       */

    LCM_IOCTRL_QUERY__IF_CS_NUMBER, /**< Storage type: LCD_IF_ENUM
                                        This attribute is mandatory.
                                        Query the chip select occupied by the LCM.
                                        Return status: LCM_IOCTRL_OK: when support.
                                                       LCM_IOCTRL_NOT_SUPPORT: shall not happen.
                                        */

    LCM_IOCTRL_QUERY__LCM_WIDTH, /**< Storage type: uint32_t
                                     This attribute is mandatory.
                                     Query the real LCM width.
                                     Return status: LCM_IOCTRL_OK: when support.
                                                    LCM_IOCTRL_NOT_SUPPORT: shall not happen.
                                     */

    LCM_IOCTRL_QUERY__LCM_HEIGHT,/**< Storage type: uint32_t
                                     This attribute is mandatory.
                                     Query the real LCM height.
                                     Return status: LCM_IOCTRL_OK: when support.
                                                    LCM_IOCTRL_NOT_SUPPORT: shall not happen.
                                     */

    LCM_IOCTL_QUERY__LCM_SETTING_PARAM, /* @deprecated Please use #LCM_IOCTRL_QUERY__LCM_SETTING_PARAM instead.*/
                                    /**< storage type: NONE Get LCM setting parameters. */

    LCM_IOCTRL_QUERY__LCM_SETTING_PARAM, /**< storage type: NONE
                                 Get LCM setting parameters.
                                    */

    LCM_IOCTRL_ESD_CHECK, /**< storage type: LCM_ESD_INFO_STRUCT*
                         LCM ESD feature control.
                         Return status: LCM_IOCTRL_OK: Operation success
                                        Others: Operation fail
                               */

    LCM_IOCTRL_ESD_RESET,/**< storage type: NONE
                         LCM ESD feature control.
                         Return status: LCM_IOCTRL_OK: Operation success
                                        Others: Operation fail
                               */
#ifdef MTK_TE_ENABLE
    LCM_IOCTRL_SET_TE, /* Enable TE or not
                         */
#endif

    LCM_IOCTRL_RSVD = 0xFF
} LCM_IOCTRL_ID_ENUM;

typedef struct {
    void (* Init)(uint16_t background);
    void (* Init_lcd_interface)(void);
    void (* BlockWrite)(uint16_t startx, uint16_t starty, uint16_t endx, uint16_t endy);
    void (* EnterSleepMode)(void);
    void (* ExitSleepMode)(void);
    void (* EnterIdleMode)(void);
    void (* ExitIdleMode)(void);
    void (* ClearScreen)(uint16_t color);
    void (* ClearScreenBW)(void);
    void (* IOCTRL)(LCM_IOCTRL_ID_ENUM ID, void *Parameters);
    bool (* CheckID)(void);
} LCD_Funcs;

typedef struct {
    uint32_t cmd;
    uint32_t count;
    uint32_t para_list[64];
} LCM_setting_table;

typedef enum {
    LCM_INTERFACE_TYPE_DBI = 0,								/**< The output type is DBI */
    LCM_INTERFACE_TYPE_DSI,									/**< The output type is DSI. */
    LCM_INTERFACE_TYPE_NUM											/**< The output type number (invalid). */
} lcm_interface_type_t;

typedef enum {
    BACKLIGHT_TYPE_ISINK = 0,								/**< The output type is DBI */
    BACKLIGHT_TYPE_DISPLAY_PWM,									/**< The output type is DSI. */
    BACKLIGHT_TYPE_LCM_BRIGHTNESS,
    BACKLIGHT_TYPE_NUM											/**< The output type number (invalid). */
} backlight_interface_type_t;

typedef struct {
    lcm_interface_type_t type;
    backlight_interface_type_t backlight_type;
    uint32_t    main_command_address;
    uint32_t    main_data_address;
    uint32_t    main_lcd_output;
    uint32_t    output_pixel_width;
#ifdef MTK_TE_ENABLE
    bool        te_enable;
#endif
} lcm_config_para_t;

extern volatile LCD_Funcs  *MainLCD;

#ifdef __cplusplus
}
#endif


#endif /* __BSP_LCD_H__ */


