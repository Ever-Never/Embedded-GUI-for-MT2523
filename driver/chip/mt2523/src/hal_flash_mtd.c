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

#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED
#include "memory_attribute.h"
#include "hal_flash_custom_memorydevice.h"
#include "hal_flash_mtd_ut.h"
#include "hal_flash_drvflash.h"
#include "hal_flash_mtd_internal.h"
#include "mt2523.h"
//#include "us_timer.h"
#include "hal_flash_general_types.h"
//#include "kal_public_api.h"
#include "hal_flash_opt.h"
#if defined(__SERIAL_FLASH_EN__)
#include "hal_flash_mtd_sf_dal.h"
#endif
#if defined(__UBL__)
#include "bl_gpt.h"
#else
#include "hal_gpt.h"
#endif

#define CHECK_LOG_START(a)
#define CHECK_LOG_STOP(a)

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
uint32_t gNOR_ReturnReadyBegin   = 0;
uint32_t gNOR_ReturnReadyEnd     = NOR_DEFAULT_RETURN_READY_LATENCY;
#endif  /* __SINGLE_BANK_NOR_FLASH_SUPPORT__ */

#ifdef NOR_RESUME_SUSPEND_INTERVAL
extern uint32_t resume_time_g;  // to store suspend time (Qbit) to satisfy tERS
#endif

/*******************************************************************************
 * For NOR FDM Customized Setting
 *******************************************************************************/
#ifdef 	__NOR_IDLE_SWEEP__
void setIdleSweepThreshold(NOR_FLASH_DRV_Data *D, uint32_t MaxSectorsPerBlock)
{
    D->IdleSweepThreshold = 2 * MaxSectorsPerBlock;
}
#endif


/*******************************************************************************
 * For NOR FDM Unit test
 *******************************************************************************/
#if defined(__BASIC_LOAD_FLASH_TEST__)
#include "drvflash_ut.h"
//******************************************************************************
// FDM_UT_Item test mapping
// NOR_FDM_UT_RW_STRESS(0): 	R/W Stress Test
// NOR_FDM_UT_AUTO:				6 Hour Power Test (CFI test + 2 hr Normal Power On/Off testing + 2hr Random Power Loss test + 2hr Busy Power Loss test + Performance Profiling test)
// NOR_FDM_UT_AUTO_16HR:
// NOR_FDM_UT_NORMAL_POWER_ON_OFF: Watch dog reset after program/erase operation is done
// NOR_FDM_UT_RANDOM_POWER_LOSS:   Watch dog reset randomly
// NOR_FDM_UT_BUSY_POWER_LOSS:     Watch dog reset during program/erase busy
// NOR_FDM_UT_CFI:                 Check Geometry setting by CFI information
// NOR_FDM_UT_PROFILING:           Read/Write performance profiling
// NOR_FDM_UT_ATE:                 Map to NOR_FDM_UT_RANDOM_POWER_LOSS r/w pattern and disable watch dog reset
// NOR_FDM_UT_BOOTROM:
// NOR_FDM_UT_OTP:
// NOR_FDM_UT_API:
//******************************************************************************

//NOR_MTD_UT_DEVICE_TEST
const uint8_t FDM_UT_Item = NOR_MTD_UT_DEVICE_TEST;

#endif


#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
uint32_t NOR_Flash_Base_Address = 0;
#endif


ATTR_TEXT_IN_RAM void *BlockAddress(void *DriverData, uint32_t BlockIndex)
{
    NOR_Flash_MTD_Data *D = DriverData;
    uint32_t addr  = 0;
    uint32_t region = 0, BlockUpperBound = 0;

    BlockUpperBound = D->RegionInfo[0].RegionBlocks;
    while (BlockUpperBound <= BlockIndex) {
        addr += D->RegionInfo[region].RegionBlocks * D->RegionInfo[region].BlockSize;
        region++;
        BlockUpperBound += D->RegionInfo[region].RegionBlocks;
    }
    addr += (BlockIndex - (BlockUpperBound - D->RegionInfo[region].RegionBlocks)) * D->RegionInfo[region].BlockSize;

    return (D->BaseAddr + addr);
}


/*-----------------------------------*/
#define INVALID_BLOCK_ENTRY (0xFFFFFFFF)

bool NOR_FLASH_BUSY = false;
bool NOR_FLASH_SUSPENDED = false;

ATTR_TEXT_IN_RAM static uint32_t LookupAddress(void *DriveData, uint32_t BlockIndex)
{
    NOR_Flash_MTD_Data *D = DriveData;
    uint32_t  addr = 0;
    static uint32_t replaced = 0;
    static uint32_t look_up_hit = 0;
    static uint32_t look_up_miss = 0;
    if (D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BlockIndex != INVALID_BLOCK_ENTRY) {
        if (D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BlockIndex != BlockIndex) {
            if (D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BlockIndex != INVALID_BLOCK_ENTRY) {
                if (D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BlockIndex != BlockIndex) {
                    look_up_miss++;
                    if (replaced & (SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1)))) {
                        D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BlockIndex = BlockIndex;
                        addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BaseAddress = (uint32_t)BlockAddress(D, BlockIndex);
                        replaced &= ~(SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1))); /* zero first */
                    } else {
                        D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BlockIndex = BlockIndex;
                        addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BaseAddress = (uint32_t)BlockAddress(D, BlockIndex);
                        replaced |= SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1)); /* one first */
                    }
                } else {
                    look_up_hit++;
                    addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BaseAddress;
                    replaced &= ~(SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1))); /* zero first */
                }
            } else {
                look_up_miss++;
                D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BlockIndex = BlockIndex;
                addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BaseAddress = (uint32_t)BlockAddress(D, BlockIndex);
                replaced &= ~(SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1))); /* zero first */
            }
        } else {
            look_up_hit++;
            addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BaseAddress;
            replaced |= SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1)); /* one first */
        }
    } else {
        look_up_miss++;
        D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BlockIndex = BlockIndex;
        addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BaseAddress = (uint32_t)BlockAddress(D, BlockIndex);
        replaced |= SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1)); /* one first */
    }
    return addr;
}
ATTR_TEXT_IN_RAM void *MapWindow(void *DriveData, uint32_t BlockIndex, uint32_t WindowIndex)
{
    NOR_Flash_MTD_Data *D = DriveData;

    /* MapWindow: window index greater zero */
    ASSERT_RET(WindowIndex == 0, 0);
    D->CurrAddr = (void *)LookupAddress(D, BlockIndex);

    return D->CurrAddr;
}

ATTR_TEXT_IN_RAM uint32_t BlockSize(void *DriveData, uint32_t BlockIndex)
{
    NOR_Flash_MTD_Data *D = DriveData;
    uint32_t region = 0, BlockUpperBound = 0;

    BlockUpperBound = D->RegionInfo[region].RegionBlocks;
    while (BlockUpperBound <= BlockIndex) {
        region++;
        BlockUpperBound += D->RegionInfo[region].RegionBlocks;
    }

    return (D->RegionInfo[region].BlockSize);
}

ATTR_TEXT_IN_RAM uint32_t BlockIndex(void *DriveData, uint32_t blk_addr)
{
    NOR_Flash_MTD_Data *D = DriveData;
    uint32_t     region = 0, BlockUpperAddr = 0;
    uint32_t     blk_idx = 0;

    BlockUpperAddr = D->RegionInfo[region].RegionBlocks * D->RegionInfo[region].BlockSize;

    while ((BlockUpperAddr < blk_addr) && (D->RegionInfo[region].BlockSize)) {
        blk_idx += D->RegionInfo[region].RegionBlocks;
        blk_addr -= BlockUpperAddr;
        region++;
        BlockUpperAddr = D->RegionInfo[region].RegionBlocks * D->RegionInfo[region].BlockSize;
    }

    if (BlockUpperAddr >= blk_addr) {
        blk_idx += blk_addr / D->RegionInfo[region].BlockSize;
    } else {
        blk_idx = INVALID_BLOCK_INDEX;
    }
    return blk_idx;
}



#if defined(__SERIAL_FLASH__)

uint32_t get_ROM_BASE(void)
{
#if (defined(__FUE__) && defined(__FOTA_DM__)) || defined(__UBL__)
    return 0;
#else

    return (uint32_t)(SFC_GENERIC_FLASH_BANK_MASK);

#endif
}

uint32_t get_NOR_FLASH_BASE_ADDRESS(void)
{
    return NOR_FLASH_BASE_ADDRESS;
}


uint32_t get_Dual_NOR_FLASH_SIZE(void)
{
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    return DUAL_NOR_FLASH_SIZE; // In hal_flash_opt_gen.h if Dual SF is enabled
#else
    return 0;
#endif
}


uint32_t get_NOR_ALLOCATED_FAT_SPACE(void)
{
    return NOR_ALLOCATED_FAT_SPACE;
}



#endif //__SERIAL_FLASH__ 


#if (defined(__AMD_SERIES_NOR__) || defined(__SERIAL_FLASH__))

ATTR_TEXT_IN_RAM void DelayAWhile_UST(uint32_t delay)
{
#if defined(__UBL__)
    bl_gpt_delay_us(delay);
#else
    hal_gpt_status_t ret;
    ret = hal_gpt_delay_us((uint32_t)delay);
    if (ret != HAL_GPT_STATUS_OK) {
        ASSERT(0);
    }
#endif
}

#endif /* __AMD_SERIES_NOR__ || __SERIAL_FLASH__ */

#if defined(__BASIC_LOAD_FLASH_TEST__)
char mtd_msg_g[200];
char mtd_inst_g[200];

/*********************************************************************
 * Show readable NOR flash related error message
 *
 * @par Catagory:
 * NOR MTD
 *
 * @note
 * 1. Make sure flash is ready before this function is called.
 *    Because print function is not located in RAM!
 **********************************************************************/

ATTR_TEXT_IN_RAM void MTD_PrintAssertInfo()
{
    printf("\n============ NOR Flash Fatal Error ============\n");
    printf("%s\n", mtd_msg_g);
    printf("%s\n", mtd_inst_g);
    printf("===============================================\n");
}

/*********************************************************************
 * Generate readable NOR flash related error message
 *
 * @par Catagory:
 * NOR MTD
 *
 * @note
 * 1. This function is only enabled in basic load.
 * 2. Make sure to put this function on RAM because of flash busy issue.
 **********************************************************************/
ATTR_TEXT_IN_RAM void MTD_GenerateAssertInfo(uint32_t err_code)
{
    uint32_t  instruction = 1;

    /* generate assert description */
    switch (err_code) {
        case NOR_MTD_ERASE_FAIL:
            sprintf(mtd_msg_g, "ERROR: Erase block failed!\n");
            break;

        case NOR_MTD_ERASE_POLL_BUSY_FAIL:
            sprintf(mtd_msg_g, "ERROR: Erase block failed. DQ6 stops toggling but DQ7 is always ZERO!\n");
            instruction = 3;
            break;

        case NOR_MTD_ERASE_TIMEOUT:
            sprintf(mtd_msg_g, "ERROR: Erase block timeout!\n");
            instruction = 3;
            break;

        case NOR_MTD_ERASE_COMP_DATA_FAIL:
            sprintf(mtd_msg_g, "ERROR: Erase block is not successful. Data compare failed!\n");
            instruction = 3;
            break;

        case NOR_MTD_ERASE_COMP_BLOCK_DATA_FAIL:
            sprintf(mtd_msg_g, "ERROR: Erase block is not successful. Non-0xFF data is found in erasing block!\n");
            instruction = 3;
            break;

        case NOR_MTD_PROGRAM_COMP_DATA_FAIL:
            sprintf(mtd_msg_g, "ERROR: Program is not successful. Data compare failed!\n");
            break;

        case NOR_MTD_PROGRAM_WRITE_BUFFER_FAIL:
            sprintf(mtd_msg_g, "ERROR: Program is not successful. Write to buffer failed!\n");
            break;

        case NOR_MTD_DATA_CORRUPTED:
            sprintf(mtd_msg_g, "ERROR: Signature mismatch. MTD data is corrupted!\n");
            break;

        case NOR_MTD_RAM_ROM_CONTENTS_UNMATCH:
            sprintf(mtd_msg_g, "ERROR: The contents of ROM and RAM are different. Flash ready test failed!\n");
            break;

        case NOR_MTD_BANK_REGION_INFO_UNMATCH:
            sprintf(mtd_msg_g, "ERROR: BankInfo is unmatched to RegionInfo!\n");
            instruction = 2;
            break;

        default:
            break;
    }

    /* generate instructions */
    if (instruction == 1) {
        sprintf(mtd_inst_g, "INSTRUCTION: Please read back raw image, then send it with Hyper Terminal log to WCT/SE2/CS5 for further analysis, thanks.\n");

    } else if (instruction == 2) {
        sprintf(mtd_inst_g, "INSTRUCTION: Please check custom files and memory database again, thanks.\n");

    } else if (instruction == 3) {
        sprintf(mtd_inst_g, "INSTRUCTION: Please contact NOR flash vendor, thanks.\n");
    }
}
#endif // __BASIC_LOAD_FLASH_TEST__ 

#endif //#ifdef HAL_FLASH_MODULE_ENABLED

