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

/*******************************************************************************
 * Filename:
 * ---------
 *  brom.h
 *
 * Project:
 * --------
 *  BootRom Library
 *
 * Description:
 * ------------
 *  Exported C interface APIs for BootRom library.
 *
 *******************************************************************************/
#ifndef _BROM_H_
#define _BROM_H_


#ifdef   __cplusplus
extern "C" {
#endif

// The magic value to stop boot process
#define BOOT_STOP 9876

// boot infinitely
#define BOOT_INFINITE   0xFFFFFFF

// default start cmd retry count
#define DEFAULT_BROM_START_CMD_RETRY_COUNT   1

//------------------------------------------------------------------------------
// Boolean
//------------------------------------------------------------------------------
typedef enum {
    _FALSE = 0,
    _TRUE = 1
} _BOOL;

//------------------------------------------------------------------------------
// return code
//------------------------------------------------------------------------------

#define BROM_RET(ret)   (ret&0x00FF0000)

#define BROM_OK                           0x000000
#define BROM_ERROR                        0x010000
#define BROM_NO_MEMORY                    0x020000
#define BROM_INVALID_ARGUMENTS            0x030000
#define BROM_SET_COM_STATE_FAIL           0x040000
#define BROM_PURGE_COM_FAIL               0x050000
#define BROM_SET_META_REG_FAIL            0x060000
#define BROM_SET_FLASHTOOL_REG_FAIL       0x070000
#define BROM_SET_REMAP_REG_FAIL           0x080000
#define BROM_SET_MEM_WAIT_STATE_FAIL      0x090000
#define BROM_DOWNLOAD_DA_FAIL             0x0A0000
#define BROM_CMD_START_FAIL               0x0B0000
#define BROM_CMD_JUMP_FAIL                0x0C0000
#define BROM_CMD_WRITE16_MEM_FAIL         0x0D0000
#define BROM_CMD_READ16_MEM_FAIL          0x0E0000
#define BROM_CMD_WRITE16_REG_FAIL         0x0F0000
#define BROM_CMD_READ16_REG_FAIL          0x100000
#define BROM_CMD_CHKSUM16_MEM_FAIL        0x110000
#define BROM_CMD_WRITE32_MEM_FAIL         0x120000
#define BROM_CMD_READ32_MEM_FAIL          0x130000
#define BROM_CMD_WRITE32_REG_FAIL         0x140000
#define BROM_CMD_READ32_REG_FAIL          0x150000
#define BROM_CMD_CHKSUM32_MEM_FAIL        0x160000
#define BROM_WR16_RD16_MEM_RESULT_DIFF    0x170000
#define BROM_WR16_RD16_REG_RESULT_DIFF    0x180000
#define BROM_WR32_RD32_MEM_RESULT_DIFF    0x190000
#define BROM_WR32_RD32_REG_RESULT_DIFF    0x1A0000
#define BROM_CHKSUM16_MEM_RESULT_DIFF     0x1B0000
#define BROM_CHKSUM32_MEM_RESULT_DIFF     0x1C0000
#define BROM_BBCHIP_HW_VER_INCORRECT      0x1D0000
#define BROM_FAIL_TO_GET_BBCHIP_HW_VER    0x1E0000
#define BROM_SKIP_BBCHIP_HW_VER_CHECK     0x1F0000
#define BROM_UNKNOWN_BBCHIP               0x200000
#define BROM_UNKNOWN_TGT_BBCHIP           0x210000
#define BROM_BBCHIP_DSP_VER_INCORRECT     0x220000
#define BROM_MULTIPLE_BAUDRATE_FAIL       0x230000
#define BROM_JUMP_TO_NFB_DETECTION_FAIL   0x240000


//------------------------------------------------------------------------------
// boot FlashTool download mode
//------------------------------------------------------------------------------
typedef struct BOOT_FLASHTOOL_ARG {

    // [IN] the PC side bbchip revsion name that is used to compare with target bbchip
    // eg: "MT6218B_EN", if this field is NULL it will skip comparison with target bbchip
    const char     *m_p_bbchip_name;

    // Download Agent
    unsigned int         m_da_start_addr;     // DA start address
    const unsigned char  *m_da_buf;           // buffer stored DA code
    unsigned int         m_da_len;            // length of DA buffer

    // This callback function will be invoke after BootROM start cmd is passed.
    // You can issue other BootROM command by brom_handle and hCOM which provides callback arguments,
    // or do whatever you want otherwise.
    //CALLBACK_IN_BROM_STAGE  m_cb_in_brom_stage;
    //void  *m_cb_in_brom_stage_arg;

} s_BOOT_FLASHTOOL_ARG;


#ifdef   __cplusplus
}
#endif

#endif


