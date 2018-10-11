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

#ifndef __HAL_G2D_INTERNAL_H__
#define __HAL_G2D_INTERNAL_H__

#ifdef HAL_G2D_MODULE_ENABLED

#define G2D_MAX_LAYER 4

#define G2D_RGB888_TO_RGB565(R, G, B)        ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | ((B & 0xF8) >> 3)
#define G2D_RGB888_PACKED(R, G, B)           (((R << 16) | (G << 8) | B) & 0x00FFFFFF)
#define G2D_BGR888_PACKED(R, G, B)           (((B << 16) | (G << 8) | R) & 0x00FFFFFF)
#define G2D_ARGB8888_PACKED(A, R, G, B)      ((A << 24) | (R << 16) | (G << 8) | B)
#define G2D_ARGB8888_TO_ARGB8565(A, R, G, B) (((A << 16) | (G2D_RGB888_TO_RGB565(R, G, B))) & 0x00FFFFFF)
#define G2D_ARGB8888_TO_ARGB6666(A, R, G, B) ((((A >> 2) << 18) | ((R >> 2) << 12) | ((G >> 2) << 6) | (B >> 2) ) & 0x00FFFFFF)

#define G2D_POLLING_TIMEOUT (2000000) /* 2s */\

#define G2D_START_START 0x00000001

#define G2D_RESET_WRST 0x00000001
#define G2D_RESET_HRST 0x00000002

#define G2D_STATUS_BUSY_BIT 0x00000001
#define G2D_STATUS_TBUSY_BIT 0x00000002

#define G2D_IRQ_FLAG0_IRQ_EN 0x00000001
#define G2D_IRQ_FLAG0 0x00010000

#define G2D_SLOW_DOWN_EN 0x80000000
#define G2D_SLOW_DOWN_SLOW_CNT_MASK 0x000000FF

#define G2D_ROI_CON_EN0_OFFSET 31
#define G2D_ROI_CON_EN1_OFFSET 30
#define G2D_ROI_CON_EN2_OFFSET 29
#define G2D_ROI_CON_EN3_OFFSET 28
#define G2D_ROI_CON_CLR_REP_EN_OFFSET 21
#define G2D_ROI_CON_DIS_BG_OFFSET 19
#define G2D_ROI_CON_TILE_SIZE_OFFSET 18
#define G2D_ROI_CON_FORCE_TS_OFFSET 17
#define G2D_ROI_CON_CLP_EN_OFFSET 16
#define G2D_ROI_CON_OUT_ALPHA_OFFSET 8
#define G2D_ROI_CON_OUT_ALP_EN_OFFSET 7
#define G2D_ROI_CON_CLRFMT_OFFSET 0

#define G2D_X_OFFSET 16
#define G2D_Y_OFFSET 0
#define G2D_OFFSET_MASK 0x00000FFF

#define G2D_WIDTH_OFFSET 16
#define G2D_HEIGHT_OFFSET 0

#define G2D_DI_CON_DI_R_OFFSET 12
#define G2D_DI_CON_DI_G_OFFSET 8
#define G2D_DI_CON_DI_B_OFFSET 4
#define G2D_DI_CON_DI_MODE_OFFSET 0

#define G2D_LX_CON_FONT_EN_OFFSET 30
#define G2D_LX_CON_IDX_OFFSET 28
#define G2D_LX_CON_SKEY_EN_OFFSET 23
#define G2D_LX_CON_RECT_EN_OFFSET 22
#define G2D_LX_CON_ROT_OFFSET 16
#define G2D_LX_CON_ALPHA_OFFSET 8
#define G2D_LX_CON_ALP_EN_OFFSET 7
#define G2D_LX_CON_CLRFMT_OFFSET 0

#define G2D_ROTATE_0                                 0x0
#define G2D_ROTATE_FLIP_90                           0x1
#define G2D_ROTATE_FLIP_0                            0x2
#define G2D_ROTATE_90                                0x3
#define G2D_ROTATE_FLIP_180                          0x4
#define G2D_ROTATE_270                               0x5
#define G2D_ROTATE_180                               0x6
#define G2D_ROTATE_FLIP_270                          0x7

#define G2D_CLRFMT_RGB565                            0x01
#define G2D_CLRFMT_UYVY                              0x02
#define G2D_CLRFMT_RGB888                            0x03
#define G2D_CLRFMT_ARGB8888                          0x08
#define G2D_CLRFMT_ARGB8565                          0x09
#define G2D_CLRFMT_ARGB6666                          0x0A
#define G2D_CLRFMT_PARGB8888                         0x0C
#define G2D_CLRFMT_PARGB8565                         0x0D
#define G2D_CLRFMT_PARGB6666                         0x0E
#define G2D_CLRFMT_BGR888                            0x13
#define G2D_CLRFMT_MASK                              0x1F

typedef struct {
    volatile uint32_t CON;
    volatile uint32_t ADDR;
    volatile uint32_t PITCH;
    volatile uint32_t OFS;
    volatile uint32_t SIZE;
    volatile uint32_t SRCKEY;
    volatile uint32_t reserved[10];
} G2D_LAYER_REGISTER_T;

typedef struct {
    volatile uint32_t G2D_START;
    volatile uint32_t G2D_MODE_CON;
    volatile uint32_t G2D_RESET;
    volatile uint32_t G2D_STATUS;
    volatile uint32_t G2D_IRQ;
    volatile uint32_t G2D_SLOW_DOWN;
    volatile uint32_t reserved0[10];
    volatile uint32_t G2D_ROI_CON;
    volatile uint32_t G2D_W2M_ADDR;
    volatile uint32_t G2D_W2M_PITCH;
    volatile uint32_t G2D_ROI_OFS;
    volatile uint32_t G2D_ROI_SIZE;
    volatile uint32_t G2D_ROI_BGCLR;
    volatile uint32_t G2D_CLP_MIN;
    volatile uint32_t G2D_CLP_MAX;
    volatile uint32_t G2D_AVO_CLR;
    volatile uint32_t G2D_REP_CLR;
    volatile uint32_t G2D_W2M_MOFS;
    volatile uint32_t reserved1;
    volatile uint32_t G2D_MW_INIT;
    volatile uint32_t G2D_MZ_INIT;
    volatile uint32_t G2D_DI_CON;
    volatile uint32_t reserved2;
    volatile G2D_LAYER_REGISTER_T G2D_L[G2D_MAX_LAYER];
} G2D_REGISTER_T;

typedef struct {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
} hal_g2d_area_t;

typedef struct {
    int32_t x;
    int32_t y;
} hal_g2d_coordinate_t;

typedef struct {
    bool enable;
    bool enable_alpha;
    bool enable_color_key;
    hal_g2d_overlay_layer_function_t function;
    hal_g2d_color_format_t color_format;
    hal_g2d_rotate_angle_t rotate;
    hal_g2d_font_format_t font_format;
    uint8_t *address;
    hal_g2d_area_t area;
    uint32_t pitch;
    uint32_t alpha;
    uint32_t color;
} g2d_layer_t;

typedef struct {
    bool is_blocking;
    bool enable_clipping;
    bool enable_color_replace;
    uint32_t avoid_color;
    uint32_t replaced_color;
    uint32_t speed;
    hal_g2d_callback_t callback;
    void *user_data;
    hal_g2d_coordinate_t clipping_min;
    hal_g2d_coordinate_t clipping_max;
    hal_g2d_coordinate_t destination_offset;
    g2d_layer_t destination;
    g2d_layer_t source[G2D_MAX_LAYER];
} g2d_context_t;

typedef struct {
    hal_g2d_dithering_mode_t mode;
    uint8_t red_bit;
    uint8_t green_bit;
    uint8_t blue_bit;
} g2d_dither_context_t;

#endif
#endif
