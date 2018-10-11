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

//-----------------------------------------------------------------------------
// MCP Serial Flash HW settings (for ComboMEM only, do not include this header)
//-----------------------------------------------------------------------------
COMBO_MEM_HW_TYPE_MODIFIER CMEMEntrySFIList COMBO_MEM_HW_INST_NAME = {   // (to be renamed by SFI owner)
    "COMBO_MEM_SFI",
    COMBO_SFI_VER,           // SFI structure version
    SFI_COMBO_COUNT,   // defined in custom_Memorydevice.h
    {

        {
            // MTKSIP_2523_SF_32_01
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x18000018,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x06, SPI, 2, 0x01, 0x42, SPI,
                2, 0x05, 0x01, SPI, 2, 0xC0, 0x02, SF_UNDEF,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MTKSIP_2523_SF_32_02
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x18000018,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x06, SPI, 2, 0x01, 0x42, SPI,
                2, 0x05, 0x01, SPI, 2, 0xC0, 0x02, SF_UNDEF,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MTKSIP_2523_SF_32_03
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x18000018,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x06, SPI, 2, 0x01, 0x42, SPI,
                2, 0x05, 0x01, SPI, 2, 0xC0, 0x02, SF_UNDEF,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B5770,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MTKSIP_2523_SF_32_04
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0x0C0B7771,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x17000017,         // 1st SFI_DLY_CTL_3
                0x00040004,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                SPI, 1, 0x38, QPI, 2, 0xC0, 0x32, SF_UNDEF,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0x0C0B7771,  // SFI_DIRECT_CTL
                0x52F80310,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        },
        {
            // MTKSIP_2523_SF_32_05
            {
                // HW config 78Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B57F0,  // SFI_DIRECT_CTL
                0x52F80010,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x17000017,         // 1st SFI_DLY_CTL_3
                0x00000200,           // DRIVING
                0,                  // Reserved
                0,  // 2nd SFI_DLY_CTL_4
                0  // 2nd SFI_DLY_CTL_5
            },  // HW config End
            {
                SPI, 1, 0x50, SPI, 3, 0x01, 0x02, 0x02,
                QPI, 8, 0x01, 0x11, 0x01, 0x11, 0x00, 0x00,
                0x00, 0x40, SF_UNDEF, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            {
                // HW config 26Mhz Start
                0x00010000,  // SFI_MAC_CTL
                0xEB0B57F0,  // SFI_DIRECT_CTL
                0x52F80000,     //  SFI_MISC_CTL
                0x00000001,     //  SFI_MISC_CTL2
                0x0,     // 1st SFI_DLY_CTL_2
                0x0,         // 1st SFI_DLY_CTL_3
                0x0,           // DRIVING
                0                 // Reserved
            }   // HW config End
        }

    }
};

