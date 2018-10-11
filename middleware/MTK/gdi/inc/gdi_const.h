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
#ifndef _GDI_CONST_H_
#define _GDI_CONST_H_
#include <stdint.h>
#include "gdi_features.h"

typedef enum
{
    GDI_CAPABILITY_NO_VIDEO     = (1 << 0), 
    GDI_CAPABILITY_NO_CAMERA    = (1 << 1),
    GDI_CAPABILITY_TOTAL
} gdi_capability_decoder_concurrency_enum;

typedef enum
{
    GDI_LCD_TYPE_NULL = 0,
    GDI_LCD_TYPE_LCD,
    GDI_LCD_TYPE_TVOUT
} gdi_lcd_type_enum;

typedef enum
{
    GDI_ANIM_POS_TYPE_INDEX,
    GDI_ANIM_POS_TYPE_TIME
} gdi_anim_pos_type_enum;

typedef enum
{
    GDI_LAYER_BLT_FLAG_NON_BLOCKING     = (1 << 0),
    GDI_LAYER_BLT_FLAG_IGNORE_BLT_LOCK  = (1 << 1),
    GDI_LAYER_BLT_FLAG_NO_GDI_LOCK      = (1 << 2),

    GDI_LAYER_BLT_FLAG_END_OF_ENUM
} gdi_layer_blt_flag_enum;

typedef enum
{
    GDI_LAYER_FLATTEN_FLAG_CLIP          = (1 << 0),
    GDI_LAYER_FLATTEN_FLAG_NON_BLOCKING  = (1 << 1),
    /* Valid only if GDI_FLATTEN_RESULT_DITHER_SUPPORT */
    GDI_LAYER_FLATTEN_FLAG_RESULT_DITHER = (1 << 2),

    GDI_FLATTEN_FLAG_END_OF_ENUM
} gdi_layer_flatten_flag_eunm;

/* Bit composition of gdi_layer_flatten_flag_eunm */
typedef uint32_t gdi_layer_flatten_flag_bitset;


/* Image Codec flag */
typedef enum
{
    GDI_IMAGE_CODEC_FLAG_DISABLE_SRC_KEY        =   (1<<0),
    GDI_IMAGE_CODEC_FLAG_IS_FILE                =   (1<<1),
    GDI_IMAGE_CODEC_FLAG_IS_MEM                 =   (1<<2),
    GDI_IMAGE_CODEC_FLAG_DISABLE_DRAW_BEFORE    =   (1<<3),
    GDI_IMAGE_CODEC_FLAG_DISABLE_DRAW           =   (1<<4),
    GDI_IMAGE_CODEC_FLAG_DISABLE_DRAW_AFTER     =   (1<<5),
    GDI_IMAGE_CODEC_FLAG_DISABLE_BLT            =   (1<<6),
    GDI_IMAGE_CODEC_FLAG_USE_SW_DECODE          =   (1<<7),
    GDI_IMAGE_CODEC_FLAG_DRAW_ONCE              =   (1<<8),
    GDI_IMAGE_CODEC_FLAG_RETRY                  =   (1<<9),
    GDI_IMAGE_CODEC_FLAG_IS_ANIMATION           =   (1<<10),
    GDI_IMAGE_CODEC_FLAG_DISABLE_SWITCH_STACK   =   (1<<11),
    GDI_IMAGE_CODEC_FLAG_BLEND_2_LAYERS         =   (1<<12),
    GDI_IMAGE_CODEC_FLAG_USE_LESS_BUF           =   (1<<13),
    GDI_IMAGE_CODEC_FLAG_FAST_GET_FRAME_BUFFER  =   (1<<14), /* just want to make sure this image will have >1 frame numbers */
    GDI_IMAGE_CODEC_FLAG_IS_IMGDEC              =   (1 << 15), /* IMGDEC identification flag */
    GDI_IMAGE_CODEC_FLAG_FORCE_DITHER           =   (1 << 16), /* Force to use dither */
    GDI_IMAGE_CODEC_FLAG_DISABLE_CACHE_SWITCH   =   (1 << 17), /* disable dynamic cacheable switch */
    GDI_IMAGE_CODEC_FLAG_DISABLE_DITHER         =   (1 << 18), /* disable do dither */
    GDI_IMAGE_CODEC_FLAG_ADJUST_ASPECT          =   (1 << 19), /* adjust the aspect, this flag is special for Non-blocking decode */
    GDI_IMAGE_CODEC_FLAG_FORCE_AB2_ALPHA_BLEND  =   (1 << 20), /* Force AB2 to do alpha blending with destination layer */
    GDI_IMAGE_CODEC_FLAG_FORCE_9SLICE_ALPHA_BLEND = (1 << 21), /* Force 9slice to do alpha blending with destination layer */
    GDI_IMAGE_CODEC_FLAG_END
} gdi_image_codec_flag_enum;

/* Resizer Enum */
typedef enum
{
    GDI_RESIZER_DEFAULT                 ,       /* GDI would decide which resizer to user for application */
    GDI_RESIZER_HW_QUALITY_HIGH         ,       
    GDI_RESIZER_HW_QUALITY_MEDIUM       ,
    GDI_RESIZER_HW_QUALITY_LOW          ,
    GDI_RESIZER_SW_QUALITY_HIGH         ,       
    GDI_RESIZER_SW_QUALITY_MEDIUM       ,       
    GDI_RESIZER_SW_QUALITY_LOW          ,       
    GDI_RESIZER_END
} gdi_resizer_enum;

typedef enum 
{
   GDI_LT_SAMPLE_MODE_TRUNCATED,
   GDI_LT_SAMPLE_MODE_NEAREST,
   GDI_LT_SAMPLE_MODE_BILINEAR_WITHOUT_EDGE_FILTER,
   GDI_LT_SAMPLE_MODE_BILINEAR_WITH_EDGE_FILTER_DULPLICATE,
   GDI_LT_SAMPLE_MODE_BILINEAR_WITH_EDGE_FILTER_DST_COLOR,
   GDI_LT_SAMPLE_MODE_BILINEAR_WITH_EDGE_FILTER_USER_SPEC_COLOR
} GDI_LT_SAMPLE_MODE_ENUM;

typedef enum 
{
   GDI_LT_SRC_KEY_EDGE_FILTER_DULPLICATE,
   GDI_LT_SRC_KEY_EDGE_FILTER_USER_SPEC_COLOR
} GDI_LT_SRC_KEY_EDGE_FILTER_ENUM;

typedef enum
{
    GDI_LT_FLAG_USE_INVERSE_MATRIX          =  (1<<0),
    GDI_LT_FLAG_NON_BLOCKING                =  (1<<1),
    GDI_LT_FLAG_USE_DIRECT_OUTPUT           =  (1<<2), 
    GDI_LT_FLAG_USE_RANDOM_ALGORITHM_DITHER =  (1<<3),
    GDI_LT_FLAG_USE_FIXED_PATTERN_DITHER    =  (1<<4),
    GDI_LT_FLAG_SOURCE_KEY_DISABLE          =  (1<<5),
    GDI_LT_FLAG_DISABLE_CACHE_SWITCH        =  (1<<6),
    
    GDI_LT_FLAG_END_OF_ENUM
} GDI_LT_FLAG_ENUM;

typedef enum 
{
   GDI_G2D_CAPABILITY_BITBLT = 0,
   GDI_G2D_CAPABILITY_RECT_FILL,
   GDI_G2D_CAPABILITY_NORMAL_FONT,
   GDI_G2D_CAPABILITY_AA_FONT,
   GDI_G2D_CAPABILITY_TILT_FONT,
   GDI_G2D_CAPABILITY_LT,
   GDI_G2D_CAPABILITY_LT_WITH_ALPHA_BLENDING,
   GDI_G2D_CAPABILITY_SAD,
   GDI_G2D_CAPABILITY_OVERLAY
} GDI_G2D_CAPABILITY_ENUM;

//#if defined(__GDI_BLT_WITH_DOUBLE_BUF_SUPPORT__)
typedef enum
{
    GDI_BLTDB_ENABLE_ONLY_WHEN_HW_UPDATE    =(1<<0),
    GDI_BLTDB_ENABLE_ALWAYS                 =(1<<1),
    GDI_BLTDB_ENABLE_END
} GDI_BLTDB_ENABLE_ENUM;
//#endif

typedef enum
{
    GDI_ROTATOR_HW,
    GDI_ROTATOR_SW,
    GDI_ROTATOR_DEFAULT
} GDI_ROTATOR_ENUM;

typedef enum
{
    GDI_BLT_FINISH    =(1<<0),
    GDI_LT_FINISH     =(1<<1),
    GDI_RESIZER_FINISH = (1<<2),
    GDD_EVENT_WAIT_DECODING = (1<<3),
    GDC_CODEC_EVENT_JOB_DONE = (1<<4),
    GDC_IMGDEC_EVENT_JOB_DONE = (1<<5),
    GDI_FLATTEN_FINISH     =(1<<6),
    GDI_2D_FILL_RECT_FINISH  = (1<<7),
    GDI_EVENTS_MAX
} GDI_EVENTS_ENUM;


/* CABC Enum */
typedef enum
{
    GDI_CABC_MODE_OFF,
    GDI_CABC_MODE_UI,
    GDI_CABC_MODE_STILL,
    GDI_CABC_MODE_MOVING
} gdi_cabc_mode_enum;

typedef enum 
{
   GDI_GIF_DISPOSAL_NO_ACTION = 0,
   GDI_GIF_DISPOSAL_NOT_DISPOSAL,
   GDI_GIF_DISPOSAL_RESTORE_BG,
   GDI_GIF_DISPOSAL_RESTORE_PRE,
   GDI_GIF_DISPOSAL_RSVD,
} GDI_GIF_DISPOSAL_ENUM;

#if 1
/* This default value can be customize by customer */
#define GDI_CABC_MODE_DEFAULT       GDI_CABC_MODE_UI

#define GDI_COLOR_FORMAT_1          0       /* 1-bit , black/white color */
#define GDI_COLOR_FORMAT_PALETTE_8  1       /* 8-bit , index color */
#define GDI_COLOR_FORMAT_16         2       /* 16-bit, rgb 565 */
#define GDI_COLOR_FORMAT_24         3       /* 24-bit, B,G,R       8,8,8 */
#define GDI_COLOR_FORMAT_32         4       /* 32-bit, B,G,R,A  8,8,8,8 */
#define GDI_COLOR_FORMAT_32_PARGB   5       /* 32-bit, PB,PG,PR,A  8,8,8,8 */
#define GDI_COLOR_FORMAT_UYVY422    6       /* 32-bit=2pixles, Y0,V0,Y1,U0 8,8,8,8 */
#define GDI_COLOR_FORMAT_A8         7       /* Alpha buffer, 8-bit */
#define GDI_COLOR_FORMAT_PARGB6666  8       /* 24-bit, (A:6 PR:6 PG:6 PB:6) */
#define GDI_COLOR_FORMAT_STANDARD_END 9     /* standard color format end marker */
#define GDI_COLOR_FORMAT_LCD0       9       /* this usually mapping to main lcd */
#define GDI_COLOR_FORMAT_LCD1       10      /* this usually mapping to sub lcd */
#define GDI_COLOR_FORMAT_END        11      /* Invalid color format */
#endif
 
#if !defined(LCD_LAYER_SOURCE_COLOR_ARGB8888)   /* in MNT should support those format */
#define GDI_HW_COLOR_FORMAT_8       0
#define GDI_HW_COLOR_FORMAT_16         1
#define GDI_HW_COLOR_FORMAT_24         2
#define GDI_HW_COLOR_FORMAT_32         3
#else /* !defined(LCD_LAYER_SOURCE_COLOR_ARGB8888) */ 
#define GDI_HW_COLOR_FORMAT_8       LCD_LAYER_SOURCE_COLOR_8BPP_INDEX
#define GDI_HW_COLOR_FORMAT_16         LCD_LAYER_SOURCE_COLOR_RGB565
#define GDI_HW_COLOR_FORMAT_24         LCD_LAYER_SOURCE_COLOR_RGB888
#define GDI_HW_COLOR_FORMAT_32         LCD_LAYER_SOURCE_COLOR_ARGB8888
#endif /* !defined(LCD_LAYER_SOURCE_COLOR_ARGB8888) */ 

#if !defined(LCD_LAYER_SOURCE_COLOR_PARGB8888)
#define GDI_HW_COLOR_FORMAT_32_PARGB   4
#else
#define GDI_HW_COLOR_FORMAT_32_PARGB   LCD_LAYER_SOURCE_COLOR_PARGB8888
#endif

#if !defined(LCD_LAYER_SOURCE_COLOR_UYVY422)
#define GDI_HW_COLOR_FORMAT_UYVY422    5
#else
#define GDI_HW_COLOR_FORMAT_UYVY422    LCD_LAYER_SOURCE_COLOR_UYVY422
#endif

// device bmp (PBM) transparent define
#define	DEVICE_BMP_TRANSPARENT_COLOR_16 (0x1234)
#define	DEVICE_BMP_TRANSPARENT_COLOR_24 (0x123456)
#define	DEVICE_BMP_TRANSPARENT_COLOR_32 (0x12345678)


#define GDI_COLOR_FORMAT_MAINLCD       GDI_COLOR_FORMAT_LCD0
#define GDI_COLOR_FORMAT_SUBLCD        GDI_COLOR_FORMAT_LCD1
#define GDI_IMAGE_TYPE_INVALID                  0
#define GDI_IMAGE_TYPE_BMP                      1
#define GDI_IMAGE_TYPE_BMP_SEQUENCE             2
#define GDI_IMAGE_TYPE_GIF                      3
#define GDI_IMAGE_TYPE_DEVICE_BITMAP            4
#define GDI_IMAGE_TYPE_DEVICE_BITMAP_SEQUENCE   5
#define GDI_IMAGE_TYPE_BMP_FILE                 6
#define GDI_IMAGE_TYPE_GIF_FILE                 7
#define GDI_IMAGE_TYPE_WBMP_FILE                8
#define GDI_IMAGE_TYPE_JPG                      9
#define GDI_IMAGE_TYPE_JPG_FILE                 10
#define GDI_IMAGE_TYPE_WBMP                     11
#define GDI_IMAGE_TYPE_AVI                      12
#define GDI_IMAGE_TYPE_AVI_FILE                 13
#define GDI_IMAGE_TYPE_3GP                      14
#define GDI_IMAGE_TYPE_3GP_FILE                 15
#define GDI_IMAGE_TYPE_MP4                      16
#define GDI_IMAGE_TYPE_MP4_FILE                 17
#define GDI_IMAGE_TYPE_JPG_SEQUENCE             18
#define GDI_IMAGE_TYPE_PNG                      19
#define GDI_IMAGE_TYPE_PNG_FILE                 20
#define GDI_IMAGE_TYPE_PNG_SEQUENCE             21
#define GDI_IMAGE_TYPE_DEVICE_BMP_FILE          22
#define GDI_IMAGE_TYPE_BMP_FILE_OFFSET          23
#define GDI_IMAGE_TYPE_GIF_FILE_OFFSET          24
#define GDI_IMAGE_TYPE_M3D                      25
#define GDI_IMAGE_TYPE_M3D_FILE                 26
#define GDI_IMAGE_TYPE_SVG                      27
#define GDI_IMAGE_TYPE_SVG_FILE                 28
#define GDI_IMAGE_TYPE_SWFLASH                  29
#define GDI_IMAGE_TYPE_SWFLASH_FILE             30

#define GDI_IMAGE_TYPE_JPG_FILE_OFFSET          31
#define GDI_IMAGE_TYPE_PNG_FILE_OFFSET          32
#define GDI_IMAGE_TYPE_DEVICE_BMP_FILE_OFFSET   33
#define GDI_IMAGE_TYPE_WBMP_FILE_OFFSET         34
#define GDI_IMAGE_TYPE_M3D_FILE_OFFSET          35
#define GDI_IMAGE_TYPE_SVG_FILE_OFFSET          36
#define GDI_IMAGE_TYPE_SWFLASH_FILE_OFFSET      37
#define GDI_IMAGE_TYPE_AVATAR                   38

#define GDI_IMAGE_TYPE_ABM_FILE_OFFSET          39
#define GDI_IMAGE_TYPE_ABM                      40
#define GDI_IMAGE_TYPE_ABM_SEQUENCE             41
#define GDI_IMAGE_TYPE_ABM_FILE                 42

#define GDI_IMAGE_TYPE_MPG                      43
#define GDI_IMAGE_TYPE_MPG_FILE                 44
#define GDI_IMAGE_TYPE_MPG_FILE_OFFSET          45

#define GDI_IMAGE_TYPE_3G2                      46
#define GDI_IMAGE_TYPE_3G2_FILE                 47
#define GDI_IMAGE_TYPE_3G2_FILE_OFFSET          48

#define GDI_IMAGE_TYPE_VIS                      49
#define GDI_IMAGE_TYPE_VIS_FILE                 50

#define GDI_IMAGE_TYPE_BMP_MEM                  51

#define GDI_IMAGE_TYPE_AB2                      52
#define GDI_IMAGE_TYPE_AB2_SEQUENCE             53
#define GDI_IMAGE_TYPE_AB2_FILE                 54
#define GDI_IMAGE_TYPE_AB2_FILE_OFFSET          55

#define GDI_IMAGE_TYPE_BMP_SEQUENCE_FILE_OFFSET 56
#define GDI_IMAGE_TYPE_DEVICE_BITMAP_SEQUENCE_FILE_OFFSET 57
#define GDI_IMAGE_TYPE_JPG_SEQUENCE_FILE_OFFSET 58
#define GDI_IMAGE_TYPE_PNG_SEQUENCE_FILE_OFFSET 59
#define GDI_IMAGE_TYPE_ABM_SEQUENCE_FILE_OFFSET 60
#define GDI_IMAGE_TYPE_AB2_SEQUENCE_FILE_OFFSET 61

#define GDI_IMAGE_TYPE_9SLICE                   62
#define GDI_IMAGE_TYPE_9SLICE_FILE              63
#define GDI_IMAGE_TYPE_9SLICE_FILE_OFFSET       64

#define GDI_IMAGE_TYPE_RM_FILE                  65
#define GDI_IMAGE_TYPE_RA_FILE                  66

#define GDI_IMAGE_TYPE_KTX                      67
#define GDI_IMAGE_TYPE_KTX_FILE                 68

#define GDI_IMAGE_TYPE_MAV                      69
#define GDI_IMAGE_TYPE_MAV_FILE                 70

#define GDI_IMAGE_TYPE_SUM                      71
#define GDI_IMAGE_TYPE_DEV_BMP                  80

#define GDI_ERROR_HANDLE                        (0)
#define GDI_NULL_HANDLE                         (0)
/////////////////////////////////////////////////////////////////////////////////////////////////
// GDI return code
/////////////////////////////////////////////////////////////////////////////////////////////////
#define GDI_IMAGE_IS_STILL_IMAGE                1
#define GDI_IMAGE_OUT_OF_CLIP_REGION            2
#define GDI_SUCCEED_NO_CALLBACK                 3

#define GDI_GIF_LAST_FRAME                      1    /* this will be remove after 05B */
#define GDI_JPEG_SUCCEED                        GDI_SUCCEED  /* this will be remove after 05B */
#define GDI_GIF_ERR_OUT_OF_CLIP_REGION          GDI_SUCCEED  /* this will be remove after 05B */
#define GDI_BMP_ERR_OUT_OF_CLIP_REGION          GDI_SUCCEED  /* this will be remove after 05B */
#define GDI_IMAGE_SUCCEED                       GDI_SUCCEED  /* this will be remove after 05B */
#define GDI_LAYER_SUCCEED                       GDI_SUCCEED  /* this will be remove after 05B */
#define GDI_GIF_SUCCEED                         GDI_SUCCEED     /* this will be remove after 05B */


/* Image 1400, -1400 */
#define GDI_IMAGE_ERR_CANT_FIND_IMG_BY_ID          -1402
#define GDI_IMAGE_ERR_NULL_IMG_PTR                 -1403
#define GDI_IMAGE_ERR_INVALID_IMG_TYPE             -1404
#define GDI_IMAGE_ERR_NO_VALID_ANIMATOR            -1405
#define GDI_IMAGE_ERR_FILE_EXT_NOT_SUPPOTR         -1406
#define GDI_IMAGE_ERR_INVALID_FILE                 -1407
#define GDI_IMAGE_ERR_DECODE_FAILED                -1408
#define GDI_IMAGE_ERR_IMAGE_TOO_LARGE              -1409
#define GDI_IMAGE_ERR_OPEN_FILE_FAILED             -1410
#define GDI_IMAGE_ERR_NO_DECODER_SUPPORT           -1411
#define GDI_IMAGE_ERR_FRAME_BUFFER_NOT_ENOUGH			GDI_IMAGE_ERR_MEMORY_NOT_AVAILABLE	
#define GDI_IMAGE_ERR_FILE_FORMAT_WRONG					-1413
#define GDI_IMAGE_ERR_NO_DECODER					    -1414
#define GDI_IMAGE_ERR_INVALID_IMAGE_SIZE				-1415	
#define GDI_IMAGE_ERR_DECODE_TIME_OUT					-1416
#define GDI_IMAGE_ERR_MEMORY_NOT_AVAILABLE              -1417
#define GDI_IMAGE_ERR_RESOURCE_NOT_AVAILABLE            -1418
#define GDI_IMAGE_ERR_UNSUPPORTED_FORMAT                -1419
#define GDI_IMAGE_ERR_9SLICE_MARGIN_ZERO                -1420
#define GDI_IMAGE_ERR_DECODE_NO_FRAME					-1421

/* JPEG Map to Image */
#define GDI_JPEG_ERR_FRAME_BUFFER_NOT_ENOUGH			GDI_IMAGE_ERR_FRAME_BUFFER_NOT_ENOUGH	
#define GDI_JPEG_ERR_OPEN_FILE_FAILED					GDI_IMAGE_ERR_OPEN_FILE_FAILED	
#define GDI_JPEG_ERR_FILE_FORMAT_WRONG					GDI_IMAGE_ERR_FILE_FORMAT_WRONG
#define GDI_JPEG_ERR_NO_DECODER							GDI_IMAGE_ERR_NO_DECODER
#define GDI_JPEG_ERR_INVALID_IMAGE_SIZE			    	GDI_IMAGE_ERR_INVALID_IMAGE_SIZE	
#define GDI_JPEG_ERR_IMAGE_TOO_LARGE					GDI_IMAGE_ERR_IMAGE_TOO_LARGE
#define GDI_JPEG_ERR_DECODE_TIME_OUT					GDI_IMAGE_ERR_DECODE_TIME_OUT


/* Layer 1500, -1500 */
#define GDI_LAYER_DOUBLE_BUFFER_LOCKED             -1501
#define GDI_LAYER_ERR_NO_VALID_LAYER_HANDLE        -1502
#define GDI_LAYER_ERR_ALLOCATE_MEMORY_FAIL         -1503
#define GDI_LAYER_ERR_INVALID_BLT_REGION           -1504
#define GDI_LAYER_ERR_NO_VALID_DOUBLE_LAYER_ENTRY  -1505
#define GDI_LAYER_ERR_HANDLE_OUT_OF_VALID_RANGE    -1506
#define GDI_LAYER_ERR_TOO_MANY_LAYER_INPUT         -1507

/* Image Decode 1700, - 1700 */
#define GDI_IMAGE_DECODER_ERR_WRONG_PARA           -1701
#define GDI_IMAGE_DECODER_ERR_NO_ITEM_ENTRY        -1702
#define GDI_IMAGE_DECODER_ERR_FILE_BUF_NOT_ENOUGH  -1703
#define GDI_IMAGE_DECODER_ERR_FRAME_BUF_NOT_ENOUGH GDI_IMAGE_ERR_MEMORY_NOT_AVAILABLE
#define GDI_IMAGE_DECODER_ERR_OPEN_FILE_FAILED     -1705
#define GDI_IMAGE_DECODER_ERR_INVALID_IMG_TYPE     -1706
#define GDI_IMAGE_DECODER_ERR_WRONG_HANDLE         -1707
#define GDI_IMAGE_DECODER_ERR_LIST_IS_EMPTY        -1708

/* ByteStream 1800, -1800 */
#define GDI_BYTESTREAM_ERR_OPEN_FILE_FAILED        -1801
#define GDI_BYTESTREAM_ERR_OPEN_BUFFER_FAILED      -1802

/* Image Encode 1900, - 1900 */
#define GDI_IMAGE_ENCODER_ERR_MEMORY_NOT_ENOUGH    -1901
#define GDI_IMAGE_ENCODER_ERR_DISK_FULL         -1902
#define GDI_IMAGE_ENCODER_ERR_WRITE_PROTECTION     -1903
#define GDI_IMAGE_ENCODER_ERR_NO_DISK        -1904
#define GDI_IMAGE_ENCODER_ERR_SIZE_LARGER_THAN_EXPECTATION  -1905
#define GDI_IMAGE_ENCODER_ERR_ROOT_DIR_FULL  -1906

/* Non-blocking -2000 */
#define GDI_ERR_NB_ABORT                            -2000

#define GDI_MIN(A,B)    ((A)<(B)?(A):(B))
#define GDI_MAX(A,B)    ((A)>(B)?(A):(B))

#define GDI_LCD_LAYER_ROTATE_0            0x00
#define GDI_LCD_LAYER_ROTATE_90           0x01
#define GDI_LCD_LAYER_ROTATE_180          0x02
#define GDI_LCD_LAYER_ROTATE_270       0x03
#define GDI_LCD_LAYER_ROTATE_0_MIRROR        0x80
#define GDI_LCD_LAYER_ROTATE_90_MIRROR          0x81
#define GDI_LCD_LAYER_ROTATE_180_MIRROR         0x82
#define GDI_LCD_LAYER_ROTATE_270_MIRROR         0x83
#define GDI_LCD_LAYER_ROTATE_ALL_MASK        0x83
#define GDI_LCD_LAYER_ROTATE_MIRROR_MASK     0x80
#define GDI_LCD_LAYER_ROTATE_ANGLE_MASK         0x03



#define GDI_IMAGE_ENCODING_FLAG_JPEG_YUV_440    0     // default value
#define GDI_IMAGE_ENCODING_FLAG_JPEG_YUV_GRAY   (1<<0)
#define GDI_IMAGE_ENCODING_FLAG_JPEG_YUV_442    (1<<1)

//////////////////////////////////////////////////////////////////////////////////
// main lcd layers
//////////////////////////////////////////////////////////////////////////////////
#define GDI_MAINLCD_MAX_LAYERS                     1

#if defined(__MMI_CAMERA__) && GDI_MAINLCD_MAX_LAYERS       <2
#undef GDI_MAINLCD_MAX_LAYERS
#define GDI_MAINLCD_MAX_LAYERS                  2
#endif /* defined(__MMI_CAMERA__) && GDI_MAINLCD_MAX_LAYERS       <2 */ 

/* tmp, wait leo fix :P */
#if defined(__GDI_MEMORY_PROFILE_2__) && GDI_MAINLCD_MAX_LAYERS   <2
#undef GDI_MAINLCD_MAX_LAYERS
#define GDI_MAINLCD_MAX_LAYERS                  2
#endif /* defined(__GDI_MEMORY_PROFILE_2__) && GDI_MAINLCD_MAX_LAYERS   <2 */ 

//////////////////////////////////////////////////////////////////////////////////
// sub lcd layers
//////////////////////////////////////////////////////////////////////////////////
#ifdef __MMI_SUBLCD__
#define GDI_SUBLCD_MAX_LAYERS                   1
#else 
#define GDI_SUBLCD_MAX_LAYERS                   0
#endif 


#if defined(WAP_SUPPORT) && defined(OBIGO_Q05A)
    #define GDI_MAX_RESIZE_WIDTH     1280
#elif defined(WAP_SUPPORT) && defined(JPG_ENCODE)
    #define  GDI_MAX_RESIZE_WIDTH     640
#else
    #define  GDI_MAX_RESIZE_WIDTH		0
#endif

#if GDI_MAX_RESIZE_WIDTH         < GDI_LCD_WIDTH
#undef GDI_MAX_RESIZE_WIDTH
#define GDI_MAX_RESIZE_WIDTH GDI_LCD_WIDTH
#endif /* GDI_MAX_RESIZE_WIDTH         < GDI_LCD_WIDTH */ 
#if GDI_MAX_RESIZE_WIDTH         < GDI_LCD_HEIGHT
#undef GDI_MAX_RESIZE_WIDTH
#define GDI_MAX_RESIZE_WIDTH GDI_LCD_HEIGHT
#endif /* GDI_MAX_RESIZE_WIDTH         < GDI_LCD_HEIGHT */ 

#ifdef __MMI_SUBLCD__
#if GDI_MAX_RESIZE_WIDTH         < GDI_SUBLCD_WIDTH
#undef GDI_MAX_RESIZE_WIDTH
#define GDI_MAX_RESIZE_WIDTH GDI_SUBLCD_WIDTH
#endif /* GDI_MAX_RESIZE_WIDTH         < GDI_SUBLCD_WIDTH */ 

#if GDI_MAX_RESIZE_WIDTH         < GDI_SUBLCD_HEIGHT
#undef GDI_MAX_RESIZE_WIDTH
#define GDI_MAX_RESIZE_WIDTH GDI_SUBLCD_HEIGHT
#endif /* GDI_MAX_RESIZE_WIDTH         < GDI_SUBLCD_HEIGHT */ 
#endif /* __MMI_SUBLCD__ */ 


#if defined(__GOVL_SUPPORT__)
#define GDI_LAYER_FLATTEN_X_MIN     (-(GOVL_ROI_MAX_WIDTH / 2))
#define GDI_LAYER_FLATTEN_X_MAX     (GOVL_ROI_MAX_WIDTH / 2)
#define GDI_LAYER_FLATTEN_Y_MIN     (-(GOVL_ROI_MAX_HEIGHT / 2))
#define GDI_LAYER_FLATTEN_Y_MAX     (GOVL_ROI_MAX_HEIGHT / 2)
#else /* LCD flatten */
#define GDI_LAYER_FLATTEN_X_MIN     (-(LCD_ROI_MAX_WIDTH / 2))
#define GDI_LAYER_FLATTEN_X_MAX     (LCD_ROI_MAX_WIDTH / 2)
#define GDI_LAYER_FLATTEN_Y_MIN     (-(LCD_ROI_MAX_HEIGHT / 2))
#define GDI_LAYER_FLATTEN_Y_MAX     (LCD_ROI_MAX_HEIGHT / 2)
#endif

#define GDI_LAYER_BLT_X_MIN     (-(LCD_ROI_MAX_WIDTH / 2))
#define GDI_LAYER_BLT_X_MAX     (LCD_ROI_MAX_WIDTH / 2)
#define GDI_LAYER_BLT_Y_MIN     (-(LCD_ROI_MAX_HEIGHT / 2))
#define GDI_LAYER_BLT_Y_MAX     (LCD_ROI_MAX_HEIGHT / 2)

#define GDI_LAYER_X_MIN     GDI_MAX(GDI_LAYER_BLT_X_MIN, GDI_LAYER_FLATTEN_X_MIN)
#define GDI_LAYER_X_MAX     GDI_MIN(GDI_LAYER_BLT_X_MAX, GDI_LAYER_FLATTEN_X_MAX)
#define GDI_LAYER_Y_MIN     GDI_MAX(GDI_LAYER_BLT_Y_MIN, GDI_LAYER_FLATTEN_Y_MIN)
#define GDI_LAYER_Y_MAX     GDI_MIN(GDI_LAYER_BLT_Y_MAX, GDI_LAYER_FLATTEN_Y_MAX)

#if defined(__HW_G2D_RECT_FILL_SUPPORT__)
#define GDI_LAYER_FILLRECT_WIDTH_MIN    G2D_LAYER_MIN_WIDTH
#define GDI_LAYER_FILLRECT_HEIGHT_MIN   G2D_LAYER_MIN_HEIGHT
#define GDI_LAYER_FILLRECT_WIDTH_MAX    G2D_LAYER_MAX_WIDTH
#define GDI_LAYER_FILLRECT_HEIGHT_MAX   G2D_LAYER_MAX_HEIGHT

#define GDI_LAYER_FILLRECT_X_MIN    G2D_LAYER_MIN_OFFSETX
#define GDI_LAYER_FILLRECT_Y_MIN    G2D_LAYER_MIN_OFFSETY
#define GDI_LAYER_FILLRECT_X_MAX    G2D_LAYER_MAX_OFFSETX
#define GDI_LAYER_FILLRECT_Y_MAX    G2D_LAYER_MAX_OFFSETY
#endif

#define GDI_IMAGE_JPEG_USE_LESS_BUF_SIZE    (50*1024) /* 50KB */

/* Main LCD scanline will use the buffer of GDI_BLTDB_BUF_SIZE */
#if defined(GDI_MAINLCD_SCANLINE_ROTATION)
    #if !defined(__GDI_BLT_WITH_DOUBLE_BUF_SUPPORT__)
        #error "please define __GDI_BLT_WITH_DOUBLE_BUF_SUPPORT__ in scanline."
#endif
#endif /* GDI_MAINLCD_SCANLINE_ROTATION */

#if defined(GDI_SUBLCD_SCANLINE_ROTATION)
    /* At present, we not have feature to play video and camera in sublCD.
     * Sub LCD will only flatten and rotate the layer.
     */
    #define GDI_SUBLCD_SCANLINE_ROTATION_BUF_SIZE (GDI_SUBLCD_WIDTH*GDI_SUBLCD_WIDTH*4)  /* 4 = 2(bpp)*2(double buffer) */
#endif /* GDI_MAINLCD_SCANLINE_ROTATION */


/* GDI_MEM_SIZE_DECODER */
/* Currently, GDI_MEM_SIZE_DECODER would be MAX(PARGB_LCD_SIZE, JPEG_SW_DEC_EXT_MIN_MEM_SIZE, JPEG_HW_DEC_EXT_MIN_MEM_SIZE)  */
#define GDI_MEM_SIZE_DECODER       (GDI_LCD_WIDTH*GDI_LCD_HEIGHT*4)

/* reduce HVGA cosmos slim ASM anonymous by 120 KB to save RAM usage */
#if (GDI_LCD_WIDTH == 320) && (GDI_LCD_HEIGHT == 480) && defined(__LOW_COST_SUPPORT_COMMON__) && defined(__COSMOS_MMI__)
    #undef GDI_MEM_SIZE_DECODER
    #define GDI_MEM_SIZE_DECODER    (GDI_LCD_WIDTH*GDI_LCD_HEIGHT*4 - 120*1024)
#endif


/* reduce cosmos slim ASM anonymous to 200 KB to save RAM usage */
#if defined(__LOW_COST_SUPPORT_COMMON__) && defined(__COSMOS_MMI__) && defined(__COSMOS_SLIM_ASM3X_PROFILE__)
    #undef GDI_MEM_SIZE_DECODER
    #define GDI_MEM_SIZE_DECODER    (200*1024)
#endif

/*Reduce Pluto ASM anonymous memory to 1/2 LCD Size if SSE not enable*/
#if !defined(__COSMOS_MMI__) && !defined(__MMI_FTE_SUPPORT__) && !defined(__MMI_SCREEN_SWITCH_EFFECT__)
    #undef GDI_MEM_SIZE_DECODER
    #define GDI_MEM_SIZE_DECODER    (GDI_LCD_WIDTH*GDI_LCD_HEIGHT)
#endif

//Rolltech
//#include "jpeg_mem.h"

#if defined(JPEG_SW_DEC_EXT_MIN_MEM_SIZE)
    #if GDI_MEM_SIZE_DECODER < JPEG_SW_DEC_EXT_MIN_MEM_SIZE
        #undef GDI_MEM_SIZE_DECODER
        #define GDI_MEM_SIZE_DECODER    JPEG_SW_DEC_EXT_MIN_MEM_SIZE
    #endif
#endif

#if defined(JPEG_HW_DEC_EXT_MIN_MEM_SIZE)
    #if GDI_MEM_SIZE_DECODER < JPEG_HW_DEC_EXT_MIN_MEM_SIZE
        #undef GDI_MEM_SIZE_DECODER
        #define GDI_MEM_SIZE_DECODER    JPEG_HW_DEC_EXT_MIN_MEM_SIZE
    #endif
#endif

/* GDI_MEM_SIZE_BLTDB */
#if defined(__GDI_BLT_WITH_DOUBLE_BUF_SUPPORT__)&& !defined(GDI_MAINLCD_SCANLINE_ROTATION)
    #define GDI_MEM_SIZE_BLTDB  GDI_BLTDB_BUF_SIZE
#else
    #define GDI_MEM_SIZE_BLTDB  (0)
#endif

/* GDI_MEM_SIZE_TOTAL is the total memory size that gdi needed. */
#define GDI_MEM_SIZE_TOTAL    ((GDI_MEM_SIZE_DECODER)+GDI_MEM_SIZE_BLTDB)

/* When GDI_USING_ASM, alloc mem from ASM, otherwise, alloc mem from Media buffer. */
/* GDI_ASM_MEM_SIZE is used by ASM to config gdi memory size, GDI_MED_MEM_SIZE is used by Media buffer. */
#if defined(GDI_USING_ASM)
    #define GDI_ASM_MEM_SIZE        GDI_MEM_SIZE_TOTAL
    #define GDI_MED_MEM_SIZE        (0)
#else   /* GDI_USING_ASM */
    #define GDI_ASM_MEM_SIZE        (0)
    #define GDI_MED_MEM_SIZE        GDI_MEM_SIZE_TOTAL
#endif  /* GDI_USING_ASM */

#define GDI_LCD_HW_UPDATE_LAYER_MAX    2

/* Flag of gdi util mode. User can use it as flags. */
typedef enum {
   GDI_UTIL_MODE_NO_RESIZE_OR_LONG_SIDE_FIT     = (1<<0),
   GDI_UTIL_MODE_NO_RESIZE_OR_SHORT_SIDE_FIT    = (1<<1),
   GDI_UTIL_MODE_LONG_SIDE_FIT                  = (1<<2),
   GDI_UTIL_MODE_SHORT_SIDE_FIT                 = (1<<3),
   GDI_UTIL_MODE_CENTER_ALIGN                   = (1<<4),
   GDI_UTIL_MODE_TOP_ALIGN                      = (1<<5),
   GDI_UTIL_MODE_BOTTOM_ALIGN                   = (1<<6),
   GDI_UTIL_MODE_LEFT_ALIGN                     = (1<<7),
   GDI_UTIL_MODE_RIGHT_ALIGN                    = (1<<8)
} gdi_util_mode_enum;

#endif /* _GDI_CONST_H_ */ 

