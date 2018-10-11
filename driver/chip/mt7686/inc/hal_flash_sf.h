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

#ifndef _FLASH_SF_H
#define _FLASH_SF_H

#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED
#include "hal_flash_general_types.h"
#include "hal_flash_sfi_hw.h"

/*-------------------------------------------------------
 *          Serial Flash Device Parameters
 *-------------------------------------------------------*/

// Parameters
#define SF_MAX_PAGE_PROG_SIZE   (256)
#define SF_ADDRESS_MASK         (0x07FFFFFF)

typedef enum {
    SF_SR_BUSY  = 0
    , SF_SR_WEL
    , SF_SR_WSE
    , SF_SR_WSP
    , SF_SR_BP
    , SF_SR_SRWD
    , SF_SR_OTPLOCK
    , SF_SR_FAIL
    , SF_SR_COUNT    // Number of SR map entries
} SF_Status_Enum;

typedef enum {
    SF_SST_SR_WEL          = 0x02      // Write Enable Latch
    , SF_SST_SR_WSE          = 0x04     // Write Suspend Erase Status
    , SF_SST_SR_WSP          = 0x08     // Write Suspend Program Status
    , SF_SST_SR_WPLD         = 0x10     // Write Protection Lockdown Status (Resets to 0 after a power cycle)
    , SF_SST_SR_SEC          = 0x20     // Security ID status (Once set by Lock Security ID (0x85h) it can not be reset)
    , SF_SST_SR_BUSY         = 0x80     // Write operation status (Write In Progress Bit)
    , SF_SST_SR_ALL          = 0xFF
} SF_SST_Status_Indication_Enum;

typedef enum {
    SF_MXIC_SR_BUSY        = 0x01      // Write operation status (Write In Progress Bit)
    , SF_MXIC_SR_WEL         = 0x02     // Write Enable Latch
    , SF_MXIC_SR_WSE         = 0x08     // Write Suspend Erase Status (in security register)
    , SF_MXIC_SR_WSP         = 0x04     // Write Suspend Program Status (in security register)
    , SF_MXIC_SR_BP          = 0x3C     // Block protection (MXIC only)
    , SF_MXIC_SR_SRWD        = 0x80     // Status register write disable (MXIC only)
    , SF_MXIC_SR_ALL         = 0xFF
} SF_MXIC_Status_Indication_Enum;

typedef enum {
    SF_MXIC_SSR_OTP        = 0x01
    , SF_MXIC_SSR_LDSO       = 0x02
    , SF_MXIC_SSR_WSP        = 0x04
    , SF_MXIC_SSR_WSE        = 0x08
    , SF_MXIC_SSR_PFAIL      = 0x20
    , SF_MXIC_SSR_EFAIL      = 0x40
    , SF_MXIC_SSR_WPSEL      = 0x80
} SF_MXIC_Security_Indication_Enum;

typedef enum {
    SF_EON_SSR_WSE         = 0x04
    , SF_EON_SSR_WSP         = 0x08
} SF_EON_Suspend_Indication_Enum;

typedef enum {
    SF_WINBOND_SSR_SUS     = 0x80
    , SF_WINBOND_SSR_OTP     = 0x3C      //Security register lock bits (LB0, LB1, LB2, LB3)
} SF_WINBOND_Suspend_Indication_Enum;

typedef enum {
    SF_NUMONYX_SSR_WSP     = 0x04
    , SF_NUMONYX_SSR_WSE     = 0x40
} SF_NUMONYX_Suspend_Indication_Enum;

typedef enum {
    SF_UNDEF = 0
    , SPI      = 1
    , SPIQ     = 2
    , QPI      = 3
} SF_MODE_Enum;

#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
#define CS_COUNT 2
#else
#define CS_COUNT 1
#endif
typedef uint8_t SF_Status;

/*-------------------------------------------------------
 *             Serial Flash Device Commands
 *-------------------------------------------------------*/

// Serial Flash commands (General for SPI)
#define SF_CMD_WRITE_SR     (0x01)
#define SF_CMD_PAGE_PROG    (0x02)
#define SF_CMD_READ         (0x03)
#define SF_CMD_NORMAL_READ  (0x00030000)  // for direct mode usage
#define SF_CMD_READ_SR      (0x05)
#define SF_CMD_WREN         (0x06)
#define SF_CMD_FAST_READ    (0x000B0000)  // for direct mode usage
#define SF_CMD_RESUME       (0x30)
#define SF_CMD_WBPR         (0x42)
#define SF_CMD_RSTEN        (0x66)
#define SF_CMD_RBPR         (0x72)
#define SF_CMD_RST          (0x99)
#define SF_CMD_SUSPEND      (0xB0)
#define SF_CMD_ERASE_CHIP   (0xC7)
#define SF_CMD_ERASE_BLOCK  (0xD8)
#define SF_CMD_ERASE_SMALL_BLOCK (0x52)
#define SF_CMD_ERASE_SECTOR (0x20)
#define SF_CMD_READ_ID      (0x9F)
#define SF_CMD_ENTER_DPD    (0xB9)
#define SF_CMD_LEAVE_DPD    (0xAB)


// Serial Flash commands (General for QPI)
#define SF_CMD_READ_ID_QPI  (0xAF)

// Serial Flash commands (SST specific)
#define SF_CMD_SST_QPIEN                    (0x38)        // for SST only
#define SF_CMD_SST_QPIRST                   (0xFF)
#define SF_CMD_SST_SET_BURST_LENGTH         (0xC0)
#define SF_CMD_SST_BURST_LENGTH_32_BYTE     (0x02)
#define SF_CMD_SST_FAST_READ                (0x0B)
#define SF_CMD_SST_FAST_READ_WRAP           (0x0C)
#define SF_CMD_SST_READ_SECURITY_ID         (0x88)    // SST: Read OTP registers
#define SF_CMD_SST_PROG_SECURITY_ID         (0xA5)    // SST: Program OTP registers
#define SF_CMD_SST_LOCK_SECURITY_ID         (0x85)    // SST: Lock OTP registers


// Serial Flash commands (MXIC specific)
#define SF_CMD_MXIC_QPIEN                   (0x35)
#define SF_CMD_MXIC_RSTQPI                  (0xF5)
#define SF_CMD_MXIC_READ_SEC_SR             (0x2B)  // OTP Read status
#define SF_CMD_MXIC_WRITE_SEC_SR            (0x2F)  // OTP write status
#define SF_CMD_MXIC_SET_BURST_LENGTH        (0xC0)
#define SF_CMD_MXIC_BURST_LENGTH_32_BYTE    (0x02)
#define SF_CMD_MXIC_BURST_LENGTH_DISABLE    (0x10)
#define SF_CMD_MXIC_FAST_READ_WRAP          (0x0B)  // for direct mode usage
#define SF_CMD_MXIC_FAST_READ_ENH           (0xEB)  // for direct mode usage
#define SF_CMD_MXIC_ENTER_SECURED_OTP       (0xB1)  // Enter OTP Access
#define SF_CMD_MXIC_EXIT_SECURED_OTP        (0xC1)  // Exit OTP Access

// Serial Flash commands (EON specific)
#define SF_CMD_EON_READ_SUSPEND_SR          (0x09)  // Suspend status register

// Serial Flash commands (WINBOND specific)
#define SF_CMD_WINBOND_READ_SUSPEND_SR      (0x35)  // Suspend status register
#define SF_CMD_WINBOND_SUSPEND              (0x75)  // Suspend
#define SF_CMD_WINBOND_RESUME               (0x7A)  // Resume
#define SF_CMD_WINBOND_READ_SECURITY_REG    (0x48)  // Winbond: Read OTP registers
#define SF_CMD_WINBOND_PROG_SECURITY_REG    (0x42)  // Winbond: Program OTP registers
#define SF_CMD_WINBOND_READ_UNIQUE_ID       (0x4B)  // Read unique ID number (4 dummy bytes)

// Serial Flash commands (NUMONYX specific)
#define SF_CMD_NUMONYX_READ_FLAG_SR         (0x70)  // Numonyx: Read flag status register
#define SF_CMD_NUMONYX_WRITE_FLAG_SR        (0x50)  // Numonyx: Write flag status register
#define SF_CMD_NUMONYX_READ_LOCK_SR         (0xE8)  // Numonyx: Read lock status register
#define SF_CMD_NUMONYX_WRITE_LOCK_SR        (0xE5)  // Numonyx: Write lock status register
#define SF_CMD_NUMONYX_READ_CONFIG_SR       (0x85)  // Numonyx: Read volatile config status register
#define SF_CMD_NUMONYX_WRITE_CONFIG_SR      (0x81)  // Numonyx: Write volatile config status register

// Serial Flash commands (GIGADEVICE specific)
#define SF_CMD_GIGADEVICE_HPM_ENTER         (0xA3000000)  // GIGA Device: Enter high performance mode + 3 dummy
#define SF_CMD_GIGADEVICE_HPM_ENTER_CMD     (0xA3)        // GIGA Device: Enter high performance mode cmd
#define SF_CMD_GIGADEVICE_HPM_EXIT          (0xAB)        // GIGA Device: Exit high performance mode

// Serial flash device characteriscs
#define SF_WRAP_ENABLE                       (1)      // wrap burst read should be enabled by default
#define SF_FLASH_ID_LENGTH                   (3)      // The number of bytes for flash ID



/*-------------------------------------------------------
 *              Serial Flash Common APIs
 *        API Body is in "flash_mtd_sf_common.c"
 *-------------------------------------------------------*/
extern void         SFI_BlankCheck(void *MTDData, uint32_t BlockIndex);
extern void         SFI_MacEnable(const uint16_t CS);
extern void         SFI_MacWaitReady(const uint16_t CS);
extern uint32_t   SFI_ReverseByteOrder(uint32_t num);
extern void         SFI_ReadDeviceID(const uint16_t CS, uint8_t *id, uint8_t cmd);
extern void         SFI_ReadDeviceID_SPI(const uint16_t CS, uint8_t *id);
// Device Command functions
extern void         SFI_Dev_Command(const uint16_t CS, const uint32_t cmd);
extern void         SFI_Dev_CommandAddress(const uint16_t CS, const uint8_t cmd, const uint32_t address, const uint32_t address_bytes);
extern void         SFI_Dev_Command_List(const uint16_t CS, const uint8_t *cmdlist);
extern void         SFI_Dev_Command_Ext(const uint16_t CS, const uint8_t *cmd, uint8_t *data, const uint16_t outl, const uint16_t inl);
#if !defined(__UBL__)
void SF_DAL_DEV_Enter_DPD(void);
void SF_DAL_DEV_Leave_DPD(void);
void SF_DAL_DEV_SWITCH_TO_LOW_FQ(void);
void SF_DAL_DEV_SWITCH_TO_HIGH_FQ(void);
#endif

// GPRAM access functions
extern void SFI_GPRAM_Write(uint32_t gpram_offset, void *buff, int32_t length);
extern uint32_t SFI_GPRAM_Write_C1A3(const uint32_t cmd, const uint32_t address);
extern uint32_t SFI_GPRAM_Write_C1A4(const uint32_t cmd, const uint32_t address, uint8_t **p_data, uint32_t *length);
#else	

void SF_DAL_DEV_SWITCH_TO_LOW_FQ(void);
void SF_DAL_DEV_SWITCH_TO_HIGH_FQ(void);
#endif

#endif /* _FLASH_SF_H */

