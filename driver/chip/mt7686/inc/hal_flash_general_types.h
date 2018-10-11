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

#ifndef __FLASH_TYPES_H
#define __FLASH_TYPES_H
#include "mt7686.h"

/*
 * Definitions for BOOLEAN
 */

#define FALSE          0
#define TRUE           1
#undef NULL
#define NULL           0

typedef char                    kal_char;
typedef unsigned short          kal_wchar;
typedef unsigned char           kal_uint8;
typedef signed char             kal_int8;
typedef unsigned short int      kal_uint16;
typedef signed short int        kal_int16;
typedef unsigned int            kal_uint32;
typedef signed int              kal_int32;
/* portable 64-bit unsigned integer */
typedef unsigned long long   kal_uint64;
/* portable 64-bit signed integer */
typedef signed long long     kal_int64;

typedef unsigned int  		  U32;
typedef unsigned short int    U16;
typedef unsigned char  		  U8;

typedef volatile unsigned char           *P_U8;
typedef volatile signed char             *P_S8;
typedef volatile unsigned short          *P_U16;
typedef volatile signed short            *P_S16;
typedef volatile unsigned int            *P_U32;
typedef volatile signed int              *P_S32;


//==============================================================================
// GFH type definition
//==============================================================================

typedef enum {

    GFH_FILE_INFO          = 0x0000
    , GFH_BL_INFO            = 0x0001
    , GFH_ANTI_CLONE         = 0x0002
    , GFH_BL_SEC_KEY         = 0x0003
    , GFH_SCTRL_CERT         = 0x0004
    , GFH_TOOL_AUTH          = 0x0005
    , GFH_MTK_RESERVED1      = 0x0006
    , GFH_BROM_CFG           = 0x0007
    , GFH_BROM_SEC_CFG       = 0x0008
    , GFH_MTK_RESERVED2      = 0x0009
    , GFH_MTK_RESERVED3      = 0x000a
    , GFH_ROOT_CERT          = 0x000b
    , GFH_EXP_CHK            = 0x000c
    , GFH_EPP_PARAM          = 0x000d
    , GFH_CHIP_VER           = 0x000e
    , GFH_MTK_RESERVED4      = 0x000f
    , GFH_MD_SEC_CFG         = 0x0010

    , GFH_EPP_INFO           = 0x0100
    , GFH_EMI_LIST           = 0x0101
    , GFH_CMEM_ID_INFO       = 0x0102
    , GFH_CMEM_NOR_INFO      = 0x0103
    , GFH_DSP_INFO           = 0x0104

    , GFH_MAUI_INFO          = 0x0200
    , GFH_MAUI_SEC           = 0x0201
    , GFH_MAUI_CODE_KEY      = 0x0202   //MAUI_SEC_KEY for code part
    , GFH_MAUI_SECURE_RO_KEY = 0x0203   //MAUI_SEC_KEY for secure ro part
    , GFH_MAUI_RESOURCE_KEY  = 0x0204   //MAUI_SEC_KEY for resource part

    , GFH_SECURE_RO_INFO     = 0x0205

    , GFH_DL_PACKAGE_INFO    = 0x0206
    , GFH_FLASH_INFO         = 0x0207
    , GFH_MACR_INFO          = 0x0208
    , GFH_ARM_BL_INFO        = 0x0209
    , GFH_EMMC_BOOTING_INFO  = 0x020A
    , GFH_FOTA_INFO          = 0x020B
    , GFH_CBR_RECORD_INFO    = 0x020C
    , GFH_CONFIDENTIAL_BIN_INFO = 0x020D
    , GFH_CBR_INFO           = 0x020E
    , GFH_MBA_INFO           = 0x020F
    , GFH_BINARY_LOCATION    = 0x0210

    , GFH_BOOT_CERT_CTRL_CONTENT = 0x0300

    , GFH_TYPE_NUM
    , GFH_TYPE_END           = (U16)0xffff
} GFH_TYPE;


//==============================================================================
// GFH header definition
//==============================================================================

#define GFH_HDR_MAGIC               0x004D4D4D  // "MMM"
#define GFH_HDR_MAGIC_END_MARK      0x45454545  // "EEEE"
#define GFH_GET_MAGIC(magic_ver)    ((magic_ver)&0x00FFFFFF)
#define GFH_GET_VER(magic_ver)      ((magic_ver)>>SFC_GENERIC_24_BIT_OFFSET)

// WARNING!! in order to guarantee all GFH structure is 4B aligned and size is multiple of 4B
// the following rules must be guaranteed!
// 1. GFH_Header structure size must be multiple of 4B
// 2. GFH_Header structure alignment must be 4B aligned
// so that use m_magic_ver(U32) to guarantee that
typedef struct GFH_Header {
    U32                 m_magic_ver;
    U16                 m_size;
#if defined(WIN32)
    U16                 m_type;
#else
    GFH_TYPE            m_type;
#endif
} GFH_Header;

typedef struct GFH_End {
    U32                 m_magic_end;
} GFH_End;

#define GFH_HEADER(type, ver) { (GFH_HDR_MAGIC|((ver)<<SFC_GENERIC_24_BIT_OFFSET)) , sizeof(type##_v##ver), type}

#endif

