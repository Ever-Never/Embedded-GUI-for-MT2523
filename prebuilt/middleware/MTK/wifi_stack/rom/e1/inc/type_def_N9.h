/* Copyright Statement:
 *
 * @2015 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek Inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE.
 */

/**
 * @file type_def_N9.h
 *
 *  TYPE definition.
 *
 */

#ifndef __TYPE_DEF_N9_H__
#define __TYPE_DEF_N9_H__
#include "rom/type_def.h"

#define __romdata	const
#define __init		
#define __initdata	
#define __exitdata	
#define __exit_call	

#define __romtext	
#define __rom_x_data
#define __rom_bss_data
#define __rom_ex_api


/* specially for register access */
#define HAL_REG_64(reg)         (*((volatile unsigned long long *)(reg)))
#define HAL_REG_32(reg)         (*((volatile unsigned int *)(reg)))
#define HAL_REG_16(reg)         (*((volatile unsigned short *)(reg)))

#define HAL_P_REG_64(reg)       ((volatile unsigned long long *)(reg))
#define HAL_P_REG_32(reg)       ((volatile unsigned int *)(reg))
#define HAL_P_REG_16(reg)       ((volatile unsigned short *)(reg))

#define HAL_READ32(_reg_)           (*((volatile UINT_32*)(_reg_)))
#define HAL_WRITE32(_reg_, _val_)   (*((volatile UINT_32*)(_reg_)) = (_val_))
typedef UINT_32         NDIS_STATUS;

typedef union _LARGE_INTEGER {
    struct {
        UINT_32  LowPart;
        INT_32   HighPart;
    } u;
    INT_64       QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct {
        UINT_32  LowPart;
        UINT_32  HighPart;
    } u;
    UINT_64      QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

#ifdef _lint
typedef struct _NDIS_PACKET {
    UCHAR MiniportReservedEx[16];
} NDIS_PACKET, *PNDIS_PACKET;

#endif
#endif
