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

#ifndef __FLASH_CFI_INTERNAL_H
#define __FLASH_CFI_INTERNAL_H

#include "hal_flash_general_types.h"
#include "mt2523.h"
//-----------------------------------------------------------------------------
// Internal Functions / Predefines
//-----------------------------------------------------------------------------
#define CFI_QRYSTR     0x00595251 // ASCII 'Q', 'R', 'Y'
#define CFI_PRISTR     0x00495250 // ASCII 'P', 'R', 'I'
#define CFI_ALTSTR     0x00544C41 // ASCII 'A', 'L', 'T'

#define CFI_MILVOLT(x)   ((x) & SFC_GENERIC_0x0F_MASK )
#define CFI_VOLT(x)      (((x) & SFC_GENERIC_0xF0_MASK ) >> SFC_GENERIC_4_BIT_OFFSET)
#define CFI_BYTE(x)      ((x) & SFC_GENERIC_0xFF_MASK )
#define CFI_WORD(a)      (a[1] << SFC_GENERIC_8_BIT_OFFSET | a[0])
#define CFI_DWORD(a)     (a[3] << SFC_GENERIC_24_BIT_OFFSET | a[2] << SFC_GENERIC_16_BIT_OFFSET | a[1] << SFC_GENERIC_8_BIT_OFFSET | a[0])
#define CFI_EXP2(x)      (SFC_GENERIC_1_BIT_OFFSET << (x))

#define CFI_TEXT_BUFF_SIZE 100  // Size of text buffer for error messges (bytes)

//-----------------------------------------------------------------------------
// CFI ID Codes: Command Set (JEDEC JEP137B)
//-----------------------------------------------------------------------------
typedef enum {
    CFI_ALG_NONE          = 0x0000,  // None
    CFI_ALG_IntelSharpExt = 0x0001,  // Intel/Sharp Extemded Command Set
    CFI_ALG_AMDFujitsu    = 0x0002,  // AMD/Fujitsu Standard Command Set
    CFI_ALG_Intel         = 0x0003,  // Intel Standard Command Set
    CFI_ALG_AMDFujitsuExt = 0x0004,  // AMD/Fujitsu Extended Command Set
    CFI_ALG_Winbond       = 0x0006,  // Winbond Standard Command Set
    CFI_ALG_Mitsubishi    = 0x0100,  // Mistubishi Standard Command Set
    CFI_ALG_MitsubishiExt = 0x0101,  // Mistubishi Extended Command Set
    CFI_ALG_SST           = 0x0102,  // SST Page Write Command Set
    CFI_ALG_Intel_Perform = 0x0200,  // Intel Performance Code Command
    CFI_ALG_Intel_Data    = 0x0210,  // Intel Data Command Set
    CFI_ALG_NA            = 0xFFFF   // Not Allowed
} Enum_CFI_ALG;

//-----------------------------------------------------------------------------
// CFI ID Codes: Device Assignments (JEDEC JEP137B)
//-----------------------------------------------------------------------------
typedef enum {
    CFI_INT_X8     = 0x0000, // x8-only asynchronous interface
    CFI_INT_X16    = 0x0001, // x16-only asynchronous interface
    CFI_INT_X8X16  = 0x0002, // Supports x8/x16 via BYTE# with asynchronous interface
    CFI_INT_X32    = 0x0003, // x32-only asynchronous interface
    CFI_INT_X16X32 = 0x0004, // Supports x16/x32 via WORD# with asynchronous interface
    CFI_INT_NA     = 0xFFFF  // Not Allowed
} Enum_CFI_INT;

//-----------------------------------------------------------------------------
// Standard CFI Command Codes / Base Addr
//-----------------------------------------------------------------------------
#define CFI_BASE_ADDR  0x0010     // CFI mode data address
#define CFI_CMD_ADDR   0x0055     // CFI command address
#define CFI_CMD_ENTR   0x0098     // Enter CFI mode
#define CFI_CMD_EXIT1  0x00FF     // Leave CFI mode (Intel/Sibley)
#define CFI_CMD_EXIT2  0x00F0     // Leave CFI mode (AMD/Toshiba)

//-----------------------------------------------------------------------------
// Read ID Command Codes
//-----------------------------------------------------------------------------
#define CFI_RDID_UNLOCK_ADR1 0x555
#define CFI_RDID_UNLOCK_DAT1 0xAA
#define CFI_RDID_UNLOCK_ADR2 0x2AA
#define CFI_RDID_UNLOCK_DAT2 0x55
#define CFI_RDID_ADR         0x555
#define CFI_RDID_CMD         0x90

//-----------------------------------------------------------------------------
// Standard CFI Query
//-----------------------------------------------------------------------------
typedef enum {
    CFI_QRY    = 0x010, // 10h: Query unique ASCII string "QRY"
    CFI_PRIALG = 0x013, // 13h: Primary algorithm
    CFI_PRIEXT = 0x015, // 15h: Offset to the Primary algorithm extended query
    CFI_ALTALG = 0x017, // 17h: Alternative algorithm
    CFI_ALTEXT = 0x019, // 19h: Offset to the Alternative algorithm extended query
    // CFI Device System Interface  (1Bh)
    CFI_VCCMIN = 0x01B, // 1Bh: Vcc Logic Supply Minimum Voltage, bits 7~4: BCD in vlots, bits 3~0: BCD in 100 millivolts
    CFI_VCCMAX = 0x01C, // 1Ch: Vcc Logic Supply Maximum Voltage, bits 7~4: BCD in vlots, bits 3~0: BCD in 100 millivolts
    CFI_VPPMIN = 0x01D, // 1Dh: Vpp Supply Minimum Voltage (Factory Mode), bits 7~4: HEX in vlots, bits 3~0: BCD in 100 millivolts
    CFI_VPPMAX = 0x01E, // 1Eh: Vpp Supply Maximum Voltage (Factory Mode), bits 7~4: HEX in vlots, bits 3~0: BCD in 100 millivolts
    CFI_WPRGTT = 0x01F, // 1Fh: Typical timeout per single byte/word/D-word program, 2^N us
    CFI_BPRGTT = 0x020, // 20h: Typical timeout for maximum-size multi-byte program 2^N us, 0=not supported
    CFI_BLKETT = 0x021, // 21h: Typical timeout per individual block erase, 2^N ms
    CFI_CHPETT = 0x022, // 22h: Typical timeout for full chip erase
    CFI_WPRGMT = 0x023, // 23h: Maximum timeout per single byte/word/D-word program, 2^N us
    CFI_BPRGMT = 0x024, // 24h: Maximum timeout for maximum-size multi-byte program 2^N us, 0=not supported
    CFI_BLKEMT = 0x025, // 25h: Maximum timeout per individual block erase, 2^N ms
    CFI_CHPEMT = 0x026, // 26h: Maximum timeout for full chip erase
    CFI_SIZE   = 0x027, // 27h: Device Size: 2^N bytes
    CFI_DEVINT = 0x028, // 28h: Device interface
    CFI_WBSIZE = 0x02A, // 2Ah: Maximum number of bytes in multi-byte program
    CFI_REGCNT = 0x02C, // 2Ch: Number of Erase Block Regions within the device
    CFI_REGINF = 0x02D  // 2Dh: Erase Block Region Information
} Enum_CFI;

//-----------------------------------------------------------------------------
// Extended CFI Query
//-----------------------------------------------------------------------------
typedef enum {
    CFI_EXTQRY  = 0x000, // P+00h: Query unique ASCII string "PRI"
    CFI_EXTVER1 = 0x003, // P+03h: Major version number, ASCII
    CFI_EXTVER2 = 0x004  // P+04h: Minor version number, ASCII
} Enum_CFI_EXT;


// AMD
typedef enum {
    CFI_AMD_BOOTFLAG = 0x000F, // P+0Fh: boot flag, 02=bottom, 03=top boot
    CFI_AMD_BNKCNT = 0x017,    // P+17h: Bank organization, number of banks
    CFI_AMD_BNKINF = 0x018     // P+18h ~ : Bank Region Information (number of sectors in each bank)
} Enum_CFI_AMD;

// Numonyx
typedef enum {
    CFI_NUMONYX_BNKCNT = 0x022, // P+22h: Number of device hardware prtition regions
    CFI_NUMONYX_REGINF = 0x023  // P+23h: Region Info Start
} Enum_CFI_NUMONYX;

#ifndef FLASH_CELL
typedef uint16_t FLASH_CELL;
#endif // FLASH_CELL

//-----------------------------------------------------------------------------
// Exported CFI Geometry Infomation
//-----------------------------------------------------------------------------
#define CFI_FLASH_ID_LENGTH   4
#define CFI_REGIONINFO_SIZE  16   // Maximum entries in Region Info
#define CFI_BANKINFO_SIZE     8   // Maximum entries in Bank Info
#define CFI_BLOCKINFO_SIZE   16   // Maximum entries in Block Info

// Basetype of RegionInfo and BankInfo
typedef struct MEM_DEV_GEOMETRY {
    uint32_t  Size;
    uint16_t  Count;
} MEM_DEV_GEOMETRY_T;

typedef struct MEM_DEV_BLOCK {
    uint32_t  Offset;
    uint32_t  Size;
} MEM_DEV_BLOCK_T;

// CFI Geometry info structure
// The structure is used by CFI_QueryGeometry(), used by .\mcu\init\info.c in Engineer Mode
typedef struct MEM_DEV_CFI_STRUCT {
    uint16_t          FlashID[CFI_FLASH_ID_LENGTH];     // Flash ID (0x90)
    uint32_t          Size;           // Size of NOR flash in bytes
    uint16_t          CommandSet;     // Primary Algorithm Code (refer to CFI spec.)
    uint8_t           RegionCount;    // Number of Regions
    uint8_t           BankCount;      // Number of Banks
    uint16_t          TotalBlocks;        // Number of Blocks (of all sizes)
    uint16_t          ProgramBufferSize;  // Size of Page Program Buffer (i.e maximum program length of bytes at a time)
    MEM_DEV_GEOMETRY_T  RegionInfo[CFI_REGIONINFO_SIZE];  // Region Info, in format of {region size, region count}
    MEM_DEV_GEOMETRY_T  BankInfo[CFI_BANKINFO_SIZE];      // Bank Info, in format of {bank size, bank count}
    MEM_DEV_BLOCK_T     BlockInfo[CFI_BLOCKINFO_SIZE];    // Block Info, in format of {offset, block size}
    MEM_DEV_GEOMETRY_T  FAT_RegionInfo[CFI_REGIONINFO_SIZE];  // Region Info, in format of {region size, region count}
    MEM_DEV_GEOMETRY_T  FAT_BankInfo[CFI_BANKINFO_SIZE];      // Bank Info, in format of {bank size, bank count}

    int8_t            CheckResults;   // Check Results
    uint32_t          CheckData1;
    uint32_t          CheckData2;
} MEM_DEV_CFI_T;

// CFI Query Error Codes
typedef enum {
    CFIErr_NoError = 0,
    CFIErr_FATBeginUnalign,
    CFIErr_FATEndUnalign,
    CFIErr_FATSizeExceed,
    CFIErr_ProgBufExceed,
    CFIErr_NotAvailable,
    CFIErr_RegionInfoError,
    CFIErr_RegionInfoBlkCnt,
    CFIErr_RegionInfoBlkSize,
    CFIErr_BankInfoBnkCnt,
    CFIErr_BankInfoBnkSize,
    CFIErr_Count
} Enum_CFIErrCode;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
// Main Funtion
void CFI_NOR_Main(void);

// General functions
int32_t CFI_Query(volatile FLASH_CELL *BaseAddr, uint16_t offset, uint8_t length);
int32_t CFI_QueryAll(volatile FLASH_CELL *BaseAddr);
void CFI_DumpInfo(MEM_DEV_CFI_T *CFIInfo);

// Reserved For EM
int32_t CFI_QueryGeometry(volatile FLASH_CELL *BaseAddr, MEM_DEV_CFI_T *CFIInfo);
int32_t CFI_CheckSettings(MEM_DEV_CFI_T *CFIInfo);
char *CFI_GetErrorMessage(MEM_DEV_CFI_T *CFIInfo, char *str, int len);
const char *CFI_GetCommandSetName(uint16_t id);
void NOR_ReadID(const uint16_t CS, volatile FLASH_CELL *BaseAddr, uint16_t *flashid);
int32_t CFI_ReadID(volatile FLASH_CELL *BaseAddr, MEM_DEV_CFI_T *CFIInfo);

//-----------------------------------------------------------------------------
// Exported Macros
//-----------------------------------------------------------------------------
#define CFI_NOR_BASE$ ((volatile FLASH_CELL *)(custom_get_NORFLASH_Base()+custom_get_fat_addr()))

#define CFI_QRY$     CFI_Query(CFI_NOR_BASE$, CFI_QRY,    3)
#define CFI_PRIALG$  CFI_Query(CFI_NOR_BASE$, CFI_PRIALG, 2)
#define CFI_PRIEXT$  CFI_Query(CFI_NOR_BASE$, CFI_PRIEXT, 2)
#define CFI_ALTALG$  CFI_Query(CFI_NOR_BASE$, CFI_ALTALG, 2)
#define CFI_ALTEXT$  CFI_Query(CFI_NOR_BASE$, CFI_ALTEXT, 2)
#define CFI_DEVINT$  CFI_Query(CFI_NOR_BASE$, CFI_DEVINT, 2)
#define CFI_SIZE$    CFI_Query(CFI_NOR_BASE$, CFI_SIZE,   1)
#define CFI_WBSIZE$  CFI_Query(CFI_NOR_BASE$, CFI_WBSIZE, 2)
#define CFI_VCCMIN$  CFI_Query(CFI_NOR_BASE$, CFI_VCCMIN, 1)
#define CFI_VCCMAX$  CFI_Query(CFI_NOR_BASE$, CFI_VCCMAX, 1)
#define CFI_VPPMIN$  CFI_Query(CFI_NOR_BASE$, CFI_VPPMIN, 1)
#define CFI_VPPMAX$  CFI_Query(CFI_NOR_BASE$, CFI_VPPMAX, 1)
#define CFI_WPRGTT$  CFI_Query(CFI_NOR_BASE$, CFI_WPRGTT, 1)
#define CFI_BPRGTT$  CFI_Query(CFI_NOR_BASE$, CFI_BPRGTT, 1)
#define CFI_BLKETT$  CFI_Query(CFI_NOR_BASE$, CFI_BLKETT, 1)
#define CFI_CHPETT$  CFI_Query(CFI_NOR_BASE$, CFI_CHPETT, 1)
#define CFI_WPRGMT$  CFI_Query(CFI_NOR_BASE$, CFI_WPRGMT, 1)
#define CFI_BPRGMT$  CFI_Query(CFI_NOR_BASE$, CFI_BPRGMT, 1)
#define CFI_BLKEMT$  CFI_Query(CFI_NOR_BASE$, CFI_BLKEMT, 1)
#define CFI_CHPEMT$  CFI_Query(CFI_NOR_BASE$, CFI_CHPEMT, 1)
#define CFI_REGCNT$  CFI_Query(CFI_NOR_BASE$, CFI_REGCNT, 1)

#endif /* end of __FLASH_CFI_H  */

