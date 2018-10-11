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

#ifndef _SFI_HW_H
#define _SFI_HW_H

#include "mt7686.h"

//-------------------------------------------------------
// Serial Flash Interface Access Macros
//-------------------------------------------------------

#define SFI_ReadReg8(addr)          *((volatile unsigned char *)(addr))
#define SFI_ReadReg16(addr)         *((volatile unsigned short *)(addr))
#define SFI_WriteReg16(addr, data)  *((volatile unsigned short *)(addr)) = (unsigned int)(data)
#define SFI_ReadReg32(addr)         *((volatile unsigned int *)(addr))
#define SFI_WriteReg32(addr, data)  *((volatile unsigned int *)(addr)) = (unsigned int)(data)

//-------------------------------------------------------
// Serial Flash Interface
//-------------------------------------------------------
// MT2523
//-------------------------------------------------------

// 4 bytes address support
#define __SFI_4BYTES_ADDRESS__

// SFI registers
#define RW_SFI_MAC_CTL              (SFC_BASE + 0x0000)
#define RW_SFI_DIRECT_CTL           (SFC_BASE + 0x0004)
#define RW_SFI_MISC_CTL             (SFC_BASE + 0x0008)
#define RW_SFI_MISC_CTL2            (SFC_BASE + 0x000C)
#define RW_SFI_MAC_OUTL             (SFC_BASE + 0x0010)
#define RW_SFI_MAC_INL              (SFC_BASE + 0x0014)
#define RW_SFI_RESET_CTL            (SFC_BASE + 0x0018)
#define RW_SFI_STA2_CTL             (SFC_BASE + 0x001C)
#define RW_SFI_DLY_CTL1             (SFC_BASE + 0x0020)
#define RW_SFI_DLY_CTL2             (SFC_BASE + 0x0024)
#define RW_SFI_DLY_CTL3             (SFC_BASE + 0x0028)
#define RW_SFI_DLY_CTL4             (SFC_BASE + 0x0030)
#define RW_SFI_DLY_CTL5             (SFC_BASE + 0x0034)
#define RW_SFI_DLY_CTL6             (SFC_BASE + 0x0038)
#define RW_SFI_DIRECT_CTL2          (SFC_BASE + 0x0040)
#define RW_SFI_MISC_CTL3            (SFC_BASE + 0x0044)
#define RW_SFI_GPRAM_DATA           (SFC_BASE + 0x0800)


// Performance Monitor
#define SF_PERF_MON1                (SFC_BASE + 0x0080)
#define SF_PERF_MON2                (SFC_BASE + 0x0084)
#define SF_PERF_MON3                (SFC_BASE + 0x0088)
#define SF_PERF_MON4                (SFC_BASE + 0x008C)
#define SF_PERF_MON5                (SFC_BASE + 0x0090)
#define SF_PERF_MON6                (SFC_BASE + 0x0094)
#define SF_PERF_MON7                (SFC_BASE + 0x0098)
#define SF_PERF_MON8                (SFC_BASE + 0x009C)
#define SF_PERF_MON9                (SFC_BASE + 0x00A0)
#define SF_PERF_MON10               (SFC_BASE + 0x00A4)
#define SF_PERF_MON11               (SFC_BASE + 0x00A8)
#define SF_PERF_MON12               (SFC_BASE + 0x00AC)
#define SF_PERF_MON13               (SFC_BASE + 0x00B0)

// RW_SF_MAC_CTL
#define SFI_WIP                     (0x00000001)   // b0
#define SFI_WIP_READY               (0x00000002)   // b1
#define SFI_TRIG                    (0x00000004)   // b2
#define SFI_MAC_EN                  (0x00000008)   // b3
#define SFI_MAC_SIO_SEL             (0x00000010)   // b4
#define SFI_RELEASE_MAC             (0x00010000)   // b16
#define SFI_MAC_SEL                 (0x10000000)   // b28

// RW_SF_DIRECT_CTL
#define SFI_QPI_EN                  (0x00000001)  // b0
#define SFI_CMD1_EXTADDR_EN         (0x00000002)  // b1
#define SFI_CMD2_EN                 (0x00000004)  // b2
#define SFI_CMD2_EXTADDR_EN         (0x00000008)  // b3
#define SFI_DR_MODE_MASK            (0x00000070)  // b4~b6
#define SFI_DR_CMD2_DUMMY_CYC_MASK  (0x00000F00)  // b8~b11
#define SFI_DR_CMD1_DUMMY_CYC_MASK  (0x0000F000)  // b12~b15
#define SFI_DR_CMD2_DUMMY_CYC_OFFSET         (8)
#define SFI_DR_CMD1_DUMMY_CYC_OFFSET        (12)

#define SFI_DR_CMD2_MASK            (0x00FF0000)  // b16~b23
#define SFI_DR_CMD1_MASK            (0xFF000000)  // b24~b31
#define SFI_DR_CMD2_OFFSET                  (16)
#define SFI_DR_CMD1_OFFSET                  (24)

// RW_SF_DIRECT_CTL: Serial flash direct read mode
#define SFI_DR_SPI_NORMAL_READ_MODE (0x00000000)
#define SFI_DR_SPI_FAST_READ_MODE   (0x00000010)
#define SFI_DR_QPI_FAST_READ_MODE   (0x00000070)

// RW_SF_MISC_CTL
#define SFI_DEL_LATCH_MASK          (0x00000003)  // b0~01
#define SFI_DEL_LATCH_OFFSET        (0)
#define SFI_REQ_IDLE                (0x00000100)  // b8
#define SFI_BOOT_REMAP              (0x00000200)  // b9
#define SFI_SYNC_EN                 (0x00020000)  // b17
#define SFI_NO_RELOAD               (0x00800000)  // b23
#define SFI_CS_EXT_MASK             (0xF0000000)  // b28~b31

// RW_SF_MISC_CTL2
#define SFI_WRAP_ACC_EN             (0x00000001)  // b0
#define SFI_DDR                     (0x00000002)  // b1

// RW_SF_MISC_CTL3
#define SFI_DEL_LATCH_MASK2         (0x00030000)  // b16~017
#define SFI_DEL_LATCH_OFFSET2       (16)
#define SFI_SF2_EN                  (0x00000001)    // b0
#define SFI_SF1_SIZE_MASK           (0x00000070)    // b4~6
#define SFI_SF1_SIZE_OFFSET         (4)             // b4~6
#define SFI_CH1_TRANS_MASK          (0x00000100)    // b8
#define SFI_CH2_TRANS_MASK          (0x00000200)    // b9
#define SFI_CH3_TRANS_MASK          (0x00000400)    // b10
#define SFI_CH1_TRANS_IDLE          (0x00001000)    // b12
#define SFI_CH2_TRANS_IDLE          (0x00002000)    // b13
#define SFI_CH3_TRANS_IDLE          (0x00004000)    // b14

// RW_SF_STA2_CTL
#define SFI_KEEP_READ_SETTING       (0x80000000)  // b31


#define SFI_DevMode (SFC->RW_SF_DIRECT_CTL&SFI_QPI_EN)
#define SFI_DevMode_CS1 (SFC->RW_SF_DIRECT_CTL2&SFI_QPI_EN)

#define SFI_SetDevMode(CS,x)  do {	\
        if(CS == 0x0) { \
	        if(x != 0) \
		        SFC->RW_SF_DIRECT_CTL = (SFC->RW_SF_DIRECT_CTL | (SFI_QPI_EN) ); \
	        else \
		        SFC->RW_SF_DIRECT_CTL = (SFC->RW_SF_DIRECT_CTL & (~(SFI_QPI_EN)) ); \
        } \
        else { \
             if(x != 0) \
                SFC->RW_SF_DIRECT_CTL2 = (SFC->RW_SF_DIRECT_CTL2 | (SFI_QPI_EN) ); \
             else \
                SFC->RW_SF_DIRECT_CTL2 = (SFC->RW_SF_DIRECT_CTL2 & (~(SFI_QPI_EN)) ); \
        } \
	} while(0)
#define SFI_GetWrapMode (SFC->RW_SF_MISC_CTL2&SFI_WRAP_ACC_EN)

#if !defined(__UBL__) // Bootloader should not toggle Code compressor
#define SFI_MaskAhbChannel(x)  do {	\
	if(x != 0) \
		SFC->RW_SF_MISC_CTL3 = (SFC->RW_SF_MISC_CTL3 | (SFI_CH2_TRANS_MASK) ); \
	else \
		SFC->RW_SF_MISC_CTL3 = (SFC->RW_SF_MISC_CTL3 & (~(SFI_CH2_TRANS_MASK)) ); \
	} while(0)

#else
#define SFI_MaskAhbChannel(...)
#endif

#endif /* _SFI_HW_H */
