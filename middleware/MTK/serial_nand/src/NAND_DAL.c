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


#ifdef MTK_FATFS_ON_SERIAL_NAND
#define __NAND_FDM_50__
#define NAND_SUPPORT

#include "hal_define.h"
#include "NAND_DAL.h"
#include "NAND_DAL_internal.h"
#include "NAND_DAL_Version.h"
#include "NAND_adapt.h"
#include "hal_nvic.h"
#include "hal_flash_fs_type.h"
#include "fs_port.h"

static void *kal_mem_cpy(void *dest, const void *src, uint32_t size)
{
    //return memcpy ( dest, src, size );
    char *d = dest;
    const char *s = src;
    while (size--) {
        *d++ = *s++;
    }
    return dest;
}

static void *kal_mem_set(void *dest, int32_t value, uint32_t size)
{
    //return memset ( dest, value, size );
    char *p = dest;
    while (size--) {
        *p++ = value;
    }
    return dest;
}

#if 0
static int32_t kal_mem_cmp(const void *src1, const void *src2, uint32_t size)
{
    //return memcmp ( src1, src2, size );
    const unsigned char *p1 = src1, *p2 = src2;
    if (size == 0) {
        return 0;
    }

    while (*p1 == *p2) {
        p1++;
        p2++;
        size--;
        if (size == 0) {
            return 0;
        }
    }
    return *p1 - *p2;
}
#endif

#if ( defined(NAND_SUPPORT)&& defined(__NAND_FDM_50__))

#if defined (__NAND_LMT__)
extern const uint8_t bWholeImage;
#endif // defined (__NAND_LMT__)


static struct_map_cache MapCache[MAX_MULTIPAGE_NUM][2] = {{0, 0}};
static uint8_t MapCacheCount[MAX_MULTIPAGE_NUM] = {0};

const uint8_t ZEROBIT[256] = {
    8, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 4,
    7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,
    7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3, 2, 2, 1,
    7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3, 2, 2, 1,
    6, 5, 5, 4, 5, 4, 4, 3, 5, 4, 4, 3, 4, 3, 3, 2,
    5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3, 2, 2, 1,
    5, 4, 4, 3, 4, 3, 3, 2, 4, 3, 3, 2, 3, 2, 2, 1,
    4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0
};


static uint32_t DAL_spare_buff[128 / 4]; /*DAL buff (for Copy)*/

char NAND_text_buff[100]; /*For debug message*/

uint8_t DAL_is_initialized = FALSE;
uint8_t CurDiskIndex = 0;
flash_info_2 Flash_Info;
add_convert Add_Cnvt;
extern mtd_if_para Mtd_para;

int8_t get_MTD_lock(void);
int8_t free_MTD_lock(void);

///TODO:change to fs mutex
static int MTD_SW_LOCK = 1; //hal layer can not use OS mutex, therefore, implement by flash driver.

int8_t init_MTD_lock(void)
{
    return 0;
}

int8_t get_MTD_lock(void)
{
    uint32_t savedMask;
    hal_nvic_save_and_set_interrupt_mask(&savedMask);
    if (MTD_SW_LOCK == 1) {
        MTD_SW_LOCK--;
        hal_nvic_restore_interrupt_mask(savedMask);
        return 0;
    } else if (MTD_SW_LOCK == 0) {
        hal_nvic_restore_interrupt_mask(savedMask);
        return -2;
    } else {
        hal_nvic_restore_interrupt_mask(savedMask);
        ASSERT_RET(0, -1);
    }
}
int8_t free_MTD_lock(void)
{
    uint32_t savedMask;
    hal_nvic_save_and_set_interrupt_mask(&savedMask);
    if (MTD_SW_LOCK == 0) {
        MTD_SW_LOCK++;
        hal_nvic_restore_interrupt_mask(savedMask);
        return 0;
    } else {
        hal_nvic_restore_interrupt_mask(savedMask);
        ASSERT_RET(0, -1);
    }
}


#define devicePage(ceIdx) (Flash_Info.devicePage[ceIdx]) /*page number of the device*/
#define zonePage(ceIdx) (devicePage(ceIdx)/Flash_Info.zone_CE[ceIdx]) /*page number of the zone*/
#define diePage(ceIdx) (Flash_Info.diePage[ceIdx]) /*page number of the die*/
#define planePage(ceIdx) (Flash_Info.planePage[ceIdx]) /*page number of plane*/
#define devDriver(ceIdx) (Flash_Info.deviceInfo_CE[ceIdx].driverSet)

//#define DAL_VERIFICATION
//#define DAL_DEBUG_TEXT

#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__) )
// Basic load in MAUI
uint32_t D_Buff[_DAL_MAX_PAGE_SIZE_ / 4 * 8 + 8];


#if defined(__MTK_TARGET__)
__attribute__((zero_init, section("NONCACHEDZI"))) uint32_t D_Buff_NC[_DAL_MAX_PAGE_SIZE_ / 4 * 8 + 8];
#else
uint32_t D_Buff_NC[_DAL_MAX_PAGE_SIZE_ / 4 * 8 + 8];
#endif

uint32_t S_Buff[512 / 4 * 8];
uint8_t Mtd_Fail_Offset = 0xFF;
uint8_t Fail_Bank = 0xFF;

#else // ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__) )
// Basic load in bootloader
// MAUI load
#if ( defined(__EMULATE_NFB_BIT_ERROE__) )
uint32_t D_Buff[_DAL_MAX_PAGE_SIZE_ / 4 * 2 + 8];
uint32_t S_Buff[_DAL_MAX_SPARE_SIZE_ / 4 * 2];
#endif //( defined(__EMULATE_NFB_BIT_ERROE__) )

#endif // ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__) )

#if ( defined(ECC_BIT_ERROR_TEST) || defined(__EMULATE_NFB_BIT_ERROE__) )

uint8_t ECCDisable = FALSE;

uint32_t EmulateBitError(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint16_t PageAddr, uint8_t SectorNumPerPage, uint32_t *Data, uint32_t *Spare)
{

    uint32_t retVal, SectorNum, BitOffset, Offset;
    uint8_t Empty;

    retVal = DAL_ReadPage(Acc_Type, ZoneNo, Bank, PageAddr, 1, ((uint8_t *)&Data[_DAL_MAX_PAGE_SIZE_ / 4]) + 4, (Acc_Type == CODE_ACC) ? NULL : (uint8_t *)&Spare[_DAL_MAX_SPARE_SIZE_ / 4]) & 0xffff0000;
    if ( retVal != MTD_STATUS_NO_ERROR ) {
        return retVal;
    }

    DAL_ReadPage(Acc_Type, ZoneNo, Bank, PageAddr, 1, (uint8_t *)Data, (uint8_t *)Spare);

    Empty = TRUE;
    for (Offset = 0; Offset < SectorNumPerPage * 0x10 / 4; Offset++) {
        if (Spare[Offset] != 0xffffffff) {
            Empty = FALSE;
            break;
        }
    }

    if (Empty == TRUE) {
        return MTD_STATUS_ERROR;  // Return this to bypass this page.
    }

    for (SectorNum = 0; SectorNum < SectorNumPerPage; SectorNum++) {
        if (1) {
            for (Offset = (SectorNum * 0x200) * 8; Offset < (SectorNum * 0x200 + 256) * 8; Offset++) {
                BitOffset = Offset & 0x07;
                if (((uint8_t *)Data)[Offset >> 3] & (0x01 << BitOffset)) {
                    ((uint8_t *)Data)[Offset >> 3] &= ~((0x01 << BitOffset));
                    break;
                }
            }
            for (Offset = (SectorNum * 0x200 + 511) * 8; Offset >= (SectorNum * 0x200 + 256) * 8; Offset--) {
                BitOffset = Offset & 0x07;
                if (((uint8_t *)Data)[Offset >> 3] & (0x01 << BitOffset)) {
                    ((uint8_t *)Data)[Offset >> 3] &= ~((0x01 << BitOffset));
                    break;
                }
            }
            for (Offset = (SectorNum * 0x10 + (PageAddr & 0x07)) * 8; Offset < (SectorNum * 0x10 + 0x8) * 8; Offset++) {
                BitOffset = Offset & 0x07;
                if (((uint8_t *)Spare)[Offset >> 3] & (0x01 << BitOffset)) {
                    ((uint8_t *)Spare)[Offset >> 3] &= ~((0x01 << BitOffset));
                    break;
                }
            }
        }  else {
            // Here NFI will not return error when ECC code error. So Do the following two times may cause ECC failure.
            // ECC code error
            uint32_t StartAddr, EndAddr;

            if (Acc_Type == CODE_ACC) {
                StartAddr = (SectorNum * 0x10 + 0x08 + (rand() % 6)) * 8;
                EndAddr = (SectorNum * 0x10 + 0x0e) * 8;
            } else {
                StartAddr = (SectorNum * 0x10 + 0x08 + (PageAddr & 0x07)) * 8;
                EndAddr = (SectorNum * 0x10 + 0x10) * 8;
            }
            for (Offset = StartAddr; Offset < EndAddr; Offset++) {
                BitOffset = Offset & 0x07;
                if (((uint8_t *)Spare)[Offset >> 3] & (0x01 << BitOffset)) {
                    ((uint8_t *)Spare)[Offset >> 3] &= ~((0x01 << BitOffset));
                    break;
                }
            }
        }
    }
    ECCDisable = TRUE;
    retVal = DAL_ProgramPage(Acc_Type, ZoneNo, Bank, PageAddr, 1, (uint8_t *)Data, (uint8_t *)Spare);
    ECCDisable = FALSE;

    return MTD_STATUS_NO_ERROR;
}

#endif // #if ( defined(ECC_BIT_ERROR_TEST) || defined(__EMULATE_NFB_BIT_ERROE__) )


/****************************************************************************
 * Function:
 *
 * Parameters:
 *
 * Returns:
 *
 * Description: It will change Flash_Info.multiProgram
 *                             Flash_Info.zone_CE
 *                             Flash_Info.bank_CE
 *                             Flash_Info.supportMPType
 *              according to the Flash_Info.deviceInfo_CE[ceIdx].driverSet and Flash_Info.supportMPType
 ****************************************************************************/
uint32_t init_multiProgram ( void )
{
    uint8_t   m_idx, ce_idx;
    uint16_t  supportMPType_ce[2] = {0, 0};
    uint32_t  bank_len_zone;
    kal_mem_set( Flash_Info.multiProgram, 0x00, sizeof(Flash_Info.multiProgram));
    for (ce_idx = 0; ce_idx < 2; ce_idx++) {
        for (m_idx = 0; m_idx < MAX_MULTIPAGE_FUNCTION_NUM; m_idx++) {
            Flash_Info.multiProgram[ce_idx][m_idx].startAlign = 0xFF;
        }
    }

    /*Operation function setting for each CE device*/
    for (ce_idx = 0; ce_idx < 2; ce_idx++) {
        if (Flash_Info.valid_CE[ce_idx] == FALSE) {
            continue;
        }
        /*The CACHE_PROGRAM, TWO_DIE_INTERLEAVE_OP, TWO_PLANE_OP, FOUR_PLANE_OP flags need to be redefined*/
        supportMPType_ce[ce_idx] = Flash_Info.supportMPType[ce_idx] & (~(CACHE_PROGRAM | TWO_DIE_INTERLEAVE_OP | TWO_PLANE_OP | FOUR_PLANE_OP));
        m_idx = 1; /*Flash_Info.multiProgram[ce_idx][0] reserve to cache function*/
        Flash_Info.bank_CE[ce_idx] = 1;
        /*non-support operation */
        ASSERT_RET((Flash_Info.supportMPType[ce_idx] & (TWO_DIE_INTERLEAVE_OP | FOUR_PLANE_OP)) != (TWO_DIE_INTERLEAVE_OP | FOUR_PLANE_OP), MTD_STATUS_ERROR);
        ASSERT_RET((Flash_Info.supportMPType[ce_idx] & (TWO_PLANE_OP | FOUR_PLANE_OP)) != (TWO_PLANE_OP | FOUR_PLANE_OP), MTD_STATUS_ERROR);

        /*multiple 4 check */
        if (((Flash_Info.supportMPType[ce_idx] & (TWO_DIE_INTERLEAVE_OP | TWO_PLANE_OP)) == (TWO_DIE_INTERLEAVE_OP | TWO_PLANE_OP))
                && (devDriver(ce_idx)->TwoDieTwoPlaneProgram != NULL)) {
            ASSERT_RET(Flash_Info.deviceInfo_CE[ce_idx].dieNum == 2, MTD_STATUS_ERROR);
            ASSERT_RET(Flash_Info.deviceInfo_CE[ce_idx].planeNum == 2, MTD_STATUS_ERROR);

            if ((Flash_Info.supportMPType[ce_idx] & CACHE_PROGRAM)
                    && (devDriver(ce_idx)->TwoDieTwoPlaneCacheProgram != NULL)) {
                Flash_Info.multiProgram[ce_idx][0].func_p = devDriver(ce_idx)->TwoDieTwoPlaneCacheProgram ;
                Flash_Info.multiProgram[ce_idx][0].multiPageNum = 0xFF;
                Flash_Info.multiProgram[ce_idx][0].startAlign = 4;
                supportMPType_ce[ce_idx] |= CACHE_PROGRAM;
            }
            Flash_Info.multiProgram[ce_idx][m_idx].func_p = devDriver(ce_idx)->TwoDieTwoPlaneProgram ;
            Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum = 4;
            Flash_Info.multiProgram[ce_idx][m_idx].startAlign = 4;
            supportMPType_ce[ce_idx] |= (TWO_DIE_INTERLEAVE_OP | TWO_PLANE_OP);
            if (m_idx == 1) {
                Flash_Info.bank_CE[ce_idx] = 4;
            }
            m_idx++;
        } else if (( (Flash_Info.supportMPType[ce_idx] & FOUR_PLANE_OP) == (FOUR_PLANE_OP))
                   && (devDriver(ce_idx)->FourPlaneProgram != NULL)) {
            ASSERT_RET(Flash_Info.deviceInfo_CE[ce_idx].planeNum == 4, MTD_STATUS_ERROR);
            if ( (Flash_Info.supportMPType[ce_idx] & CACHE_PROGRAM)
                    && (devDriver(ce_idx)->FourPlaneProgram != NULL)
                    && (Flash_Info.multiProgram[ce_idx][0].func_p == NULL)) {
                Flash_Info.multiProgram[ce_idx][0].func_p = devDriver(ce_idx)->FourPlaneCacheProgram ;
                Flash_Info.multiProgram[ce_idx][0].multiPageNum = 0xFF;
                Flash_Info.multiProgram[ce_idx][0].startAlign = 4;
                supportMPType_ce[ce_idx] |= CACHE_PROGRAM;
            }
            Flash_Info.multiProgram[ce_idx][m_idx].func_p = devDriver(ce_idx)->FourPlaneProgram ;
            Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum = 4;
            Flash_Info.multiProgram[ce_idx][m_idx].startAlign = 4;
            supportMPType_ce[ce_idx] |= (FOUR_PLANE_OP);
            if (m_idx == 1) {
                Flash_Info.bank_CE[ce_idx] = 4;
            }
            m_idx++;
        }
        /*multiple 2 check */
        if (((Flash_Info.supportMPType[ce_idx] & TWO_PLANE_OP) == TWO_PLANE_OP)
                && (devDriver(ce_idx)->TwoPlaneProgram != NULL)) {
            ASSERT_RET(Flash_Info.deviceInfo_CE[ce_idx].planeNum == 2, MTD_STATUS_ERROR);
            if ( (Flash_Info.multiProgram[ce_idx][0].func_p == NULL)
                    && (Flash_Info.supportMPType[ce_idx] & CACHE_PROGRAM)
                    && (devDriver(ce_idx)->TwoPlaneCacheProgram != NULL)) {
                Flash_Info.multiProgram[ce_idx][0].func_p = devDriver(ce_idx)->TwoPlaneCacheProgram ;
                Flash_Info.multiProgram[ce_idx][0].multiPageNum = 0xFF;
                Flash_Info.multiProgram[ce_idx][0].startAlign = 2;
                supportMPType_ce[ce_idx] |= CACHE_PROGRAM;
            }

            Flash_Info.multiProgram[ce_idx][m_idx].func_p = devDriver(ce_idx)->TwoPlaneProgram ;
            Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum = 2;
            Flash_Info.multiProgram[ce_idx][m_idx].startAlign = 2;
            supportMPType_ce[ce_idx] |= TWO_PLANE_OP;
            if (m_idx == 1) {
                Flash_Info.bank_CE[ce_idx] = 2;
            }
            m_idx++;
        } else if (((Flash_Info.supportMPType[ce_idx] & TWO_DIE_INTERLEAVE_OP) == TWO_DIE_INTERLEAVE_OP)
                   && (devDriver(ce_idx)->TwoDieProgram != NULL)) {
            //ASSERT(Flash_Info.deviceInfo_CE[ce_idx].dieNum == 2);
            ASSERT_RET(Flash_Info.deviceInfo_CE[ce_idx].planeNum == 2, MTD_STATUS_ERROR);
            if ((Flash_Info.multiProgram[ce_idx][0].func_p == NULL)
                    && (Flash_Info.supportMPType[ce_idx] & CACHE_PROGRAM)
                    && (devDriver(ce_idx)->TwoDieCacheProgram != NULL)) {
                Flash_Info.multiProgram[ce_idx][0].func_p = devDriver(ce_idx)->TwoDieCacheProgram ;
                Flash_Info.multiProgram[ce_idx][0].multiPageNum = 0xFF;
                Flash_Info.multiProgram[ce_idx][0].startAlign = 2;
                supportMPType_ce[ce_idx] |= CACHE_PROGRAM;
            }

            Flash_Info.multiProgram[ce_idx][m_idx].func_p = devDriver(ce_idx)->TwoDieProgram ;
            Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum = 2;
            Flash_Info.multiProgram[ce_idx][m_idx].startAlign = 2;
            supportMPType_ce[ce_idx] |= TWO_DIE_INTERLEAVE_OP;
            if (m_idx == 1) {
                Flash_Info.bank_CE[ce_idx] = 2;
            }
            m_idx++;
        }
        /*1 page operation check */
        if ((devDriver(ce_idx)->ProgramPage != NULL)) {
            if ((Flash_Info.multiProgram[ce_idx][0].func_p == NULL)
                    && (Flash_Info.supportMPType[ce_idx] & CACHE_PROGRAM)
                    && (devDriver(ce_idx)->CacheProgram != NULL)) {
                Flash_Info.multiProgram[ce_idx][0].func_p = devDriver(ce_idx)->CacheProgram ;
                Flash_Info.multiProgram[ce_idx][0].multiPageNum = 0xFF;
                Flash_Info.multiProgram[ce_idx][0].startAlign = 1; /*any position % 1 == 0*/
                supportMPType_ce[ce_idx] |= CACHE_PROGRAM;
            }

            Flash_Info.multiProgram[ce_idx][m_idx].func_p = devDriver(ce_idx)->ProgramPage ;
            Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum = 1;
            Flash_Info.multiProgram[ce_idx][m_idx].startAlign = 1; /*any position % 1 == 0*/
            if (m_idx == 1) {
                Flash_Info.bank_CE[ce_idx] = 1;
            }
            m_idx++;
        }
        /*set Flash_Info.supportMPType[ce_idx]*/
        Flash_Info.supportMPType[ce_idx] = supportMPType_ce[ce_idx];
        /*set Flash_Info.zone_CE[ce_idx]*/
        /*bank_len_zone must be smaller than planePage(ce_idx)*/
        bank_len_zone = devicePage(ce_idx) / Flash_Info.bank_CE[ce_idx] < planePage(ce_idx) ? devicePage(ce_idx) / Flash_Info.bank_CE[ce_idx] : planePage(ce_idx);
        Flash_Info.zone_CE[ce_idx] = devicePage(ce_idx) / (bank_len_zone * Flash_Info.bank_CE[ce_idx]);
    }
    return MTD_STATUS_NO_ERROR;
}
/****************************************************************************
 * Function:
 *
 * Parameters:
 *
 * Returns:
 *
 * Description: It will change Flash_Info.multiErase
 *                             Flash_Info.supportMEType
 *              according to the Flash_Info.deviceInfo_CE[ceIdx].driverSet and Flash_Info.supportMEType
 ****************************************************************************/
uint32_t init_multiErase ( void )
{
    uint8_t   m_idx, ce_idx;
    uint16_t  supportMEType_ce[2] = {0, 0};

    kal_mem_set( Flash_Info.multiErase, 0x00, sizeof(Flash_Info.multiErase));
    for (ce_idx = 0; ce_idx < 2; ce_idx++) {
        for (m_idx = 0; m_idx < MAX_MULTIPAGE_FUNCTION_NUM; m_idx++) {
            Flash_Info.multiErase[ce_idx][m_idx].startAlign = 0xFF;
        }
    }
    /*Operation function setting for each CE device*/
    for (ce_idx = 0; ce_idx < 2; ce_idx++) {
        if (Flash_Info.valid_CE[ce_idx] == FALSE) {
            continue;
        }

        Flash_Info.supportMEType[ce_idx] &= Flash_Info.supportMPType[ce_idx]; /*Multi Block erase support must follow (or less than) Multi-Page erase support*/
        /*The TWO_DIE_INTERLEAVE_OP, TWO_PLANE_OP, FOUR_PLANE_OP flags need to be redefined*/
        supportMEType_ce[ce_idx] = Flash_Info.supportMEType[ce_idx] & (~(TWO_DIE_INTERLEAVE_OP | TWO_PLANE_OP | FOUR_PLANE_OP));
        m_idx = 0;
        /*multiple 4 check */
        if ( ((Flash_Info.supportMEType[ce_idx] & (TWO_DIE_INTERLEAVE_OP | TWO_PLANE_OP)) == (TWO_DIE_INTERLEAVE_OP | TWO_PLANE_OP))
                && (devDriver(ce_idx)->TwoDieTwoPlaneErase != NULL) ) {
            ASSERT_RET(Flash_Info.deviceInfo_CE[ce_idx].dieNum == 2, MTD_STATUS_ERROR);
            ASSERT_RET(Flash_Info.deviceInfo_CE[ce_idx].planeNum == 2, MTD_STATUS_ERROR);

            Flash_Info.multiErase[ce_idx][m_idx].func_p = devDriver(ce_idx)->TwoDieTwoPlaneErase ;
            Flash_Info.multiErase[ce_idx][m_idx].multiPageNum = 4;
            Flash_Info.multiErase[ce_idx][m_idx].startAlign = 4;
            supportMEType_ce[ce_idx] |= (TWO_DIE_INTERLEAVE_OP | TWO_PLANE_OP);
            m_idx++;
        } else if ( ((Flash_Info.supportMEType[ce_idx] & FOUR_PLANE_OP) == FOUR_PLANE_OP)
                    && (devDriver(ce_idx)->FourPlaneErase != NULL) ) {
            ASSERT_RET(Flash_Info.deviceInfo_CE[ce_idx].planeNum == 4, MTD_STATUS_ERROR);
            Flash_Info.multiErase[ce_idx][m_idx].func_p = devDriver(ce_idx)->FourPlaneErase ;
            Flash_Info.multiErase[ce_idx][m_idx].multiPageNum = 4;
            Flash_Info.multiErase[ce_idx][m_idx].startAlign = 4;
            supportMEType_ce[ce_idx] |= (FOUR_PLANE_OP);
            m_idx++;
        }
        /*multiple 2 check */
        if ( ((Flash_Info.supportMEType[ce_idx] & TWO_PLANE_OP) == TWO_PLANE_OP)
                && (devDriver(ce_idx)->TwoPlaneErase != NULL) ) {
            ASSERT_RET(Flash_Info.deviceInfo_CE[ce_idx].planeNum == 2, MTD_STATUS_ERROR);
            Flash_Info.multiErase[ce_idx][m_idx].func_p = devDriver(ce_idx)->TwoPlaneErase ;
            Flash_Info.multiErase[ce_idx][m_idx].multiPageNum = 2;
            Flash_Info.multiErase[ce_idx][m_idx].startAlign = 2;
            supportMEType_ce[ce_idx] |= TWO_PLANE_OP;
            m_idx++;
        } else if ( ((Flash_Info.supportMEType[ce_idx] & TWO_DIE_INTERLEAVE_OP) == TWO_DIE_INTERLEAVE_OP)
                    && (devDriver(ce_idx)->TwoDieErase != NULL) ) {
            ASSERT_RET(Flash_Info.deviceInfo_CE[ce_idx].dieNum == 2, MTD_STATUS_ERROR);

            Flash_Info.multiErase[ce_idx][m_idx].func_p = devDriver(ce_idx)->TwoDieErase ;
            Flash_Info.multiErase[ce_idx][m_idx].multiPageNum = 2;
            Flash_Info.multiErase[ce_idx][m_idx].startAlign = 2;
            supportMEType_ce[ce_idx] |= TWO_DIE_INTERLEAVE_OP;
            m_idx++;
        }
        /*1 page operation check */
        if ( (devDriver(ce_idx)->EraseBlock != NULL) ) {
            Flash_Info.multiErase[ce_idx][m_idx].func_p = devDriver(ce_idx)->EraseBlock ;
            Flash_Info.multiErase[ce_idx][m_idx].multiPageNum = 1;
            Flash_Info.multiErase[ce_idx][m_idx].startAlign = 1; /*any position % 1 == 0*/
            m_idx++;
        }
        /*set Flash_Info.supportMEType[ce_idx]*/
        Flash_Info.supportMEType[ce_idx] = supportMEType_ce[ce_idx];
    }
    return MTD_STATUS_NO_ERROR;
}

uint32_t init_CE_interleave (void)
{
#if defined(ENABLE_ADVANCE_OPERATION)
    uint8_t m_idx, ce_idx;
#endif  // defined(ENABLE_ADVANCE_OPERATION)

    Flash_Info.CE_interleave_Flag = FALSE;
    Flash_Info.CE_cache_interleave_Flag = FALSE;

#if defined(ENABLE_ADVANCE_OPERATION)
    if (Flash_Info.valid_CE[1] == FALSE) {
        return FS_NAND_DEVICE_NOT_SUPPORTED;
    }
    /*FDM request the total bank number is order of 2*/
    if ( Flash_Info.bank_CE[0] != Flash_Info.bank_CE[1]) {
        return FS_NAND_DEVICE_NOT_SUPPORTED;
    }
    /*zone number of each CE must be the same then CE_interleave can be support*/
    if ( Flash_Info.zone_CE[0] != Flash_Info.zone_CE[1] ) {
        return FS_NAND_DEVICE_NOT_SUPPORTED;
    }
    /*bank size in zone of each CE must be the same then CE_interleave can be support*/
    if ( devicePage(0) / Flash_Info.zone_CE[0] / Flash_Info.bank_CE[0] != devicePage(1) / Flash_Info.zone_CE[1] / Flash_Info.bank_CE[1]) {
        return FS_NAND_DEVICE_NOT_SUPPORTED;
    }

    if ( (Flash_Info.valid_CE[0] == TRUE && Flash_Info.valid_CE[1] == TRUE)
#if 1 /*Limit CE interleave exists only at same device or not*/
            && kal_mem_cmp(&Flash_Info.deviceInfo_CE[0], &Flash_Info.deviceInfo_CE[1], sizeof(Flash_Info.deviceInfo_CE[0]) ) == 0 /*could loose this condition*/
#endif
       ) {
        for (ce_idx = 0; ce_idx < 2; ce_idx++) {
            Flash_Info.CE_cache_interleave_mOP_Idx[ce_idx] = 0xFF;
            Flash_Info.CE_interleave_mOP_Idx[ce_idx] = 0xFF;
            for (m_idx = 0; m_idx < MAX_MULTIPAGE_FUNCTION_NUM; m_idx++) {
                if ( Flash_Info.multiProgram[ce_idx][m_idx].func_p != NULL && Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum == 0xFF) { /* cache function*/
                    Flash_Info.CE_cache_interleave_mOP_Idx[ce_idx] = m_idx;
                    break;
                }
            }
            for (m_idx = 0; m_idx < MAX_MULTIPAGE_FUNCTION_NUM; m_idx++) {
                if ( Flash_Info.multiProgram[ce_idx][m_idx].func_p != NULL && Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum != 0xFF) { /* non-cache function*/
                    Flash_Info.CE_interleave_mOP_Idx[ce_idx] = m_idx;
                    break;
                }
            }
        }
        if (Flash_Info.CE_cache_interleave_mOP_Idx[0] != 0xFF && Flash_Info.CE_cache_interleave_mOP_Idx[1] != 0xFF) {
            Flash_Info.CE_cache_interleave_Flag = TRUE;
        }
        if (Flash_Info.CE_interleave_mOP_Idx[0] != 0xFF && Flash_Info.CE_interleave_mOP_Idx[1] != 0xFF) {
            Flash_Info.CE_interleave_Flag = TRUE;
        }
    }
#endif // defined(ENABLE_ADVANCE_OPERATION)
    return MTD_STATUS_NO_ERROR;
}

#if 0
#if defined(__AUDIO_DSP_LOWPOWER__)
void DAL_MP3InitVariable(void)
{
    NFI_ConfigforMP3.nfi_ACCCON_reg = *NFI_ACCCON;
    NFI_ConfigforMP3.nfi_page_size = Flash_Info.pageSize;
    NFI_ConfigforMP3.nfi_spare_size = 16;
    NFI_ConfigforMP3.nfi_pages_per_block = Flash_Info.blockPage;
    NFI_ConfigforMP3.nfi_fdm_size = 8;// FDM size fixed to 8 Bytes
    NFI_ConfigforMP3.nfi_sector_num = (Flash_Info.pageSize / 512);
    NFI_ConfigforMP3.nfi_data_sectors = (Flash_Info.pageSize / 512);
    if (Flash_Info.pageSize == 512) {
        NFI_ConfigforMP3.nfi_col_num = 1;
        NFI_ConfigforMP3.nfi_row_num = (Flash_Info.deviceInfo_CE[0].address_cycle - 1);
    } else {
        NFI_ConfigforMP3.nfi_col_num = 2;
        NFI_ConfigforMP3.nfi_row_num = (Flash_Info.deviceInfo_CE[0].address_cycle - 2);
    }
    if (Flash_Info.deviceInfo_CE[0].IOWidth == 16) {
        NFI_ConfigforMP3.nfi_io_width = 1;
    } else {
        NFI_ConfigforMP3.nfi_io_width = 0;
    }
}
#endif //#if defined(__AUDIO_DSP_LOWPOWER__)
#endif

/****************************************************************************
 * Function:
 *
 * Parameters:  void
 *
 * Returns:     FS_NO_ERROR
 *              FS_NAND_DEVICE_NOT_SUPPORTED
 *              FS_FLASH_MOUNT_ERROR
 * Description:
 *
 ****************************************************************************/
/*Note: Please DO NOT modify the function name, because append2.pl use this function name to identify
* __NAND_FDM_50__ and apply the MTK_ROMInfo_v06_ST */

uint32_t DAL_init (void)
{
    //uint8_t m_idx, ce_idx, i;
    uint8_t ce_idx;
    uint8_t bank_num;
    uint32_t bank_len_zone;  /*bank length in zone, unit by page*/
    uint32_t retVal;
    get_MTD_lock();
    kal_mem_set(&Flash_Info, 0x00, sizeof(Flash_Info));

    /*Get CE# ID and set dev_driver, devicePage*/ /*2 CE must : same block size, same page size, same EXTENDED_BLOCK or not*/ /*same PLANE_INTERLACED or PLANE_CONTINUOUS ??*/
    retVal = MTD_MountDevice();
    if (retVal != FS_NO_ERROR) {
        DAL_is_initialized = TRUE;
        free_MTD_lock();
        return retVal;
    }
    if (Flash_Info.valid_CE[1] == TRUE) {
        ASSERT_RET(Flash_Info.deviceInfo_CE[0].pageSize == Flash_Info.deviceInfo_CE[1].pageSize, MTD_STATUS_ERROR);
        ASSERT_RET(Flash_Info.deviceInfo_CE[0].spareSize == Flash_Info.deviceInfo_CE[1].spareSize, MTD_STATUS_ERROR);
        ASSERT_RET(Flash_Info.deviceInfo_CE[0].blockPage == Flash_Info.deviceInfo_CE[1].blockPage, MTD_STATUS_ERROR);
        ASSERT_RET(Flash_Info.deviceInfo_CE[0].IOWidth == Flash_Info.deviceInfo_CE[1].IOWidth, MTD_STATUS_ERROR);
        ASSERT_RET((Flash_Info.deviceInfo_CE[0].supportMPType & EXTENDED_BLOCK) == (Flash_Info.deviceInfo_CE[1].supportMPType & EXTENDED_BLOCK), MTD_STATUS_ERROR);
        ASSERT_RET((Flash_Info.deviceInfo_CE[0].supportMPType & PLANE_INTERLACED) == (Flash_Info.deviceInfo_CE[1].supportMPType & PLANE_INTERLACED), MTD_STATUS_ERROR);

        if (Flash_Info.deviceInfo_CE[0].supportMPType & EXTENDED_BLOCK) { /**only support 2 same NAND with EXTENDED_BLOCK*/
            ASSERT_RET(Flash_Info.deviceInfo_CE[0].supportMPType & PLANE_CONTINUOUS, MTD_STATUS_ERROR);
        }
    }
    Flash_Info.pageSize = Flash_Info.deviceInfo_CE[0].pageSize;
    Flash_Info.spareSize = Flash_Info.deviceInfo_CE[0].spareSize;
    //#if !(defined __UBL__)
    //force_ASSERT( sizeof(DAL_spare_buff) >= Flash_Info.spareSize,sizeof(DAL_spare_buff),Flash_Info.spareSize,0);
    //#endif
    ASSERT_RET(sizeof(DAL_spare_buff) >= Flash_Info.spareSize, MTD_STATUS_ERROR);
    Flash_Info.blockPage = Flash_Info.deviceInfo_CE[0].blockPage;
    /*Operation function setting for each CE device*/
    Flash_Info.supportMPType[0] = Flash_Info.deviceInfo_CE[0].supportMPType;
    Flash_Info.supportMPType[1] = (Flash_Info.valid_CE[1] == TRUE) ?  Flash_Info.deviceInfo_CE[1].supportMPType : 0;

    init_multiProgram();

    Flash_Info.supportMEType[0] = Flash_Info.deviceInfo_CE[0].supportMEType;
    Flash_Info.supportMEType[1] = Flash_Info.deviceInfo_CE[1].supportMEType;
    init_multiErase();

    /* Set Flash_Info.CE_interleave parameters*/
    init_CE_interleave();

    /*shrink Zone size if both CE have device and different zone size*/
    if (Flash_Info.valid_CE[0] == TRUE && Flash_Info.valid_CE[1] == TRUE) {
        if ( Flash_Info.CE_interleave_Flag == FALSE ) {
            bank_num = Flash_Info.bank_CE[0] < Flash_Info.bank_CE[1] ? Flash_Info.bank_CE[0] : Flash_Info.bank_CE[1] ;
            while (!(bank_num == Flash_Info.bank_CE[0] && bank_num == Flash_Info.bank_CE[1])) {
                for (ce_idx = 0; ce_idx < 2; ce_idx++) {
                    if ( Flash_Info.bank_CE[ce_idx] >  bank_num) {
                        if ((Flash_Info.supportMPType[ce_idx]&TWO_DIE_INTERLEAVE_OP) != 0) {
                            Flash_Info.supportMPType[ce_idx] &= (~TWO_DIE_INTERLEAVE_OP);
                            Flash_Info.bank_CE[ce_idx] /= 2;
                        } else if ((Flash_Info.supportMPType[ce_idx]&FOUR_PLANE_OP) != 0) {
                            Flash_Info.supportMPType[ce_idx] &= (~FOUR_PLANE_OP);
                            Flash_Info.bank_CE[ce_idx] /= 4;
                        } else if ((Flash_Info.supportMPType[ce_idx]&TWO_PLANE_OP) != 0) {
                            Flash_Info.supportMPType[ce_idx] &= (~TWO_PLANE_OP);
                            Flash_Info.bank_CE[ce_idx] /= 2;
                        }
                        bank_num = bank_num < Flash_Info.bank_CE[ce_idx] ? bank_num : Flash_Info.bank_CE[ce_idx];
                        ASSERT_RET(bank_num > 0, MTD_STATUS_ERROR);
                    }
                }
            }
            /*after setting of Flash_Info.supportMPType[ce_idx], re-assign Flash_Info.multiProgram*/
            init_multiProgram ( );
            init_multiErase ();
            ASSERT_RET(Flash_Info.bank_CE[0] == Flash_Info.bank_CE[1], MTD_STATUS_ERROR);
        }
        bank_len_zone = (devicePage(0) / Flash_Info.zone_CE[0] / Flash_Info.bank_CE[0] < devicePage(1) / Flash_Info.zone_CE[1] / Flash_Info.bank_CE[1] ) ?
                        devicePage(0) / Flash_Info.zone_CE[0] / Flash_Info.bank_CE[0] : devicePage(1) / Flash_Info.zone_CE[1] / Flash_Info.bank_CE[1];

        Flash_Info.zone_CE[0] = devicePage(0) / (bank_len_zone * Flash_Info.bank_CE[0]);
        Flash_Info.zone_CE[1] = devicePage(1) / (bank_len_zone * Flash_Info.bank_CE[1]);
    }
    /*else is single CE case, Flash_Info.zone_CE[0] is set by init_multiProgram*/

    Flash_Info.bankNum = (Flash_Info.CE_interleave_Flag == FALSE ? (Flash_Info.bank_CE[0]) : (Flash_Info.bank_CE[0] + Flash_Info.bank_CE[1]));
    Flash_Info.zoneNum = (Flash_Info.CE_interleave_Flag == FALSE ? (Flash_Info.zone_CE[0] + Flash_Info.zone_CE[1]) : (Flash_Info.zone_CE[0]));
    ASSERT_RET(MAX_MULTIPAGE_NUM >= Flash_Info.bankNum, MTD_STATUS_ERROR);
    if (Flash_Info.deviceInfo_CE[0].supportMPType & EXTENDED_BLOCK) { /*if using EXTENDED_BLOCK NAND, must full support multi-page operation*/
        ASSERT_RET( Flash_Info.zoneNum == 1, MTD_STATUS_ERROR);
    }
    kal_mem_set(MapCache, 0xFF, sizeof(MapCache));
    kal_mem_set(MapCacheCount, 0xFF, sizeof(MapCacheCount));

    DAL_is_initialized = TRUE;
#if defined (__AUDIO_DSP_LOWPOWER__)
    DAL_MP3InitVariable();
#endif //__AUDIO_DSP_LOWPOWER__
    free_MTD_lock();
    return FS_NO_ERROR;
}
/****************************************************************************
 * Function:
 *
 * Parameters:  void
 *
 * Returns:     FS_NO_ERROR
 *
 * Description:
 *
 ****************************************************************************/
int32_t DAL_ShutDown (void)
{
    int32_t retVal = 0;
    get_MTD_lock();
    retVal = MTD_ShutDown();
    free_MTD_lock();
    return retVal;
}

uint32_t DAL_FDM_Flash_Info_Init (FLASH_INFO *fdm_flash_info, uint32_t DiskIndex)
{
    uint32_t FAT_start_position;
    uint32_t FAT_length;
    uint32_t zone_len;
    uint16_t i;
    uint8_t  FAT_start_z, FAT_end_z;
    uint32_t sectionPage = 0;

    get_MTD_lock();

    /*fill fdm_flash_info->BankNumber*/
    fdm_flash_info->BankNumber = Flash_Info.bankNum;
    /*fill fdm_flash_info->SectorNumPerPage*/
    fdm_flash_info->SectorNumPerPage = Flash_Info.pageSize / 512;
    /*fill fdm_flash_info->PageNumPerSection*/
    fdm_flash_info->PageNumPerSection = fdm_flash_info->BankNumber * Flash_Info.blockPage;
    /*fill fdm_flash_info->PageNumPerSection*/
    sectionPage = fdm_flash_info->PageNumPerSection;
    for (i = 0; i < 32; i++) {
        if (sectionPage == (uint32_t)(1 << i)) {
            break;
        }
    }

    ASSERT_RET(i < 32, MTD_STATUS_ERROR);
    fdm_flash_info->SectionShiftNum = i;

    /*fill fdm_flash_info->Plane_PagePair*/
    //if ((Flash_Info.deviceInfo_CE[0].driverSet->TwoPlaneProgram == samsung_twoPlane_Program) &&
    //    (Flash_Info.supportMPType[0] & TWO_PLANE_OP))
    if ( 0 && (Flash_Info.supportMPType[0] & TWO_PLANE_OP)) {
        fdm_flash_info->Plane_PagePair = TRUE;
    } else {
        fdm_flash_info->Plane_PagePair = FALSE;
    }


#if (defined (_NAND_FLASH_BOOTING_) || defined(__NANDFDM_MULTI_INSTANCE__) || (defined(__UP_PKG_ON_NAND__) && defined(NAND_SUPPORT)) ) && (!defined(__UBL__)||(defined(__UBL__)&&defined(__NAND_SUPPORT_RAW_DISK__)))
#if !defined(__NANDFDM_MULTI_INSTANCE__)
    FAT_start_position = NFB_BASE_ADDRESS;
    FAT_length = NFB_ALLOCATED_FAT_SPACE;
#else // !defined(__NANDFDM_MULTI_INSTANCE__)
    FAT_start_position = NFB_BASE_ADDRESS[DiskIndex];
    FAT_length = NFB_ALLOCATED_FAT_SPACE[DiskIndex];
#endif // !defined(__NANDFDM_MULTI_INSTANCE__)


    FAT_start_position = (FAT_start_position + (Flash_Info.pageSize - 1)) / Flash_Info.pageSize ; /*move to page aligned and unit by page*/
    FAT_start_position = (FAT_start_position + (sectionPage - 1)) / sectionPage * sectionPage; /*move to section aligned*/
    FAT_length /= Flash_Info.pageSize; /*reduce size to page aligned and unit by page*/
    FAT_length = FAT_length / sectionPage * sectionPage; /*reduce size to section aligned*/
#else

///TODO::: add default value 0, check the default value is 0 or not?
    FAT_start_position = 0;  /*unit by page*/
    FAT_length = devicePage(0) + (Flash_Info.valid_CE[1] == TRUE ? devicePage(1) : 0); /*full NAND size is used for FAT */
    FAT_length = (Flash_Info.supportMPType[0] & EXTENDED_BLOCK) ? FAT_length / 1024 * (1024 + 16) : FAT_length; /*consider EXTENDED_BLOCK size*/
    FAT_length = FAT_length / sectionPage * sectionPage; /*reduce size to section aligned*/
#endif /* _NAND_FLASH_BOOTING_ || defined(__NANDFDM_MULTI_INSTANCE__) || (__UP_PKG_ON_NAND__ && NAND_SUPPORT) */


    /*calculate zone_len*/
    if (Flash_Info.CE_interleave_Flag == FALSE) {
        if ( Flash_Info.valid_CE[1] == TRUE ) {
            //ASSERT(zonePage(0) == zonePage(1));
            ASSERT_RET(zonePage(0) == zonePage(1), MTD_STATUS_ERROR);
        }
        zone_len = zonePage(0);
    } else {
        zone_len = zonePage(0) + zonePage(1);
    }

    /*fill fdm_flash_info->ExtendBlock*/
    if ( (Flash_Info.supportMPType[0]& EXTENDED_BLOCK) == EXTENDED_BLOCK) {
        zone_len = zone_len / 1024 * (1024 + 16);
        fdm_flash_info->ExtendBlock = TRUE;
    } else {
        fdm_flash_info->ExtendBlock = FALSE;
    }
    FAT_start_z = FAT_start_position / zone_len;
    FAT_end_z = ( FAT_start_position + FAT_length - 1 ) / zone_len;
    /*fill fdm_flash_info->TotalZoneNum*/
    fdm_flash_info->TotalZoneNum = FAT_end_z - FAT_start_z + 1;
    ASSERT_RET(fdm_flash_info->TotalZoneNum <= NUM_OF_ZONES, MTD_STATUS_ERROR);
    /*fill fdm_flash_info->PSectionNumInZone*/
    /*first one*/
    fdm_flash_info->PSectionNumInZone[0] =
        (zone_len - (FAT_start_position % zone_len)) < FAT_length ? (zone_len - (FAT_start_position % zone_len)) / sectionPage : FAT_length / sectionPage;
    for (i = FAT_start_z + 1; i < FAT_end_z; i++) {
        fdm_flash_info->PSectionNumInZone[i - FAT_start_z] = zone_len / sectionPage;
    }
#if defined(__NANDFDM_MULTI_INSTANCE__)
    fdm_flash_info->TotalPSectionNumInZone = zone_len / sectionPage;
#endif // defined(__NANDFDM_MULTI_INSTANCE__)
    if (FAT_end_z > FAT_start_z) {
        fdm_flash_info->PSectionNumInZone[FAT_end_z - FAT_start_z] = ( (( FAT_start_position + FAT_length - 1)  % zone_len) + 1) / sectionPage; /*last one*/
    }
#if 0
#if defined (_NAND_FLASH_BOOTING_) || defined(__NANDFDM_MULTI_INSTANCE__) || (defined(__UP_PKG_ON_NAND__) && defined(NAND_SUPPORT))
    /*Fill the global variables FAT_Start_ZoneNum and FAT_Start_Section_Offset*/
#if !defined(__NANDFDM_MULTI_INSTANCE__)
    FAT_Start_ZoneNum = FAT_start_z;
    FAT_Start_Section_Offset = (FAT_start_position % zone_len) / sectionPage;
#else // !defined(__NANDFDM_MULTI_INSTANCE__)
    FAT_Start_ZoneNum[DiskIndex] = FAT_start_z;
    FAT_Start_Section_Offset[DiskIndex] = (FAT_start_position % zone_len) / sectionPage;
#endif //!defined(__NANDFDM_MULTI_INSTANCE__)
#if defined (__NAND_LMT__)

    {
        extern uint32_t IM_FAT_Start_Section;
        extern uint32_t IM_FAT_Start_Zone;

        IM_FAT_Start_Section = FAT_Start_Section_Offset;
        IM_FAT_Start_Zone = FAT_Start_ZoneNum;
        if (!bWholeImage) {

            FAT_Start_Section_Offset = 0;
            FAT_Start_ZoneNum = 0;
        }
    }

#endif // defined (__NAND_LMT__)

#endif /* _NAND_FLASH_BOOTING_ || defined(__NANDFDM_MULTI_INSTANCE__) || (__UP_PKG_ON_NAND__ && NAND_SUPPORT) */
#endif

    free_MTD_lock();
    return MTD_STATUS_NO_ERROR;
}


uint32_t readMapCache(add_convert *AddInfo, uint8_t *ce_p, uint32_t *Zoffset_p, uint32_t *Boffset_p )
{
    uint8_t bIdx = AddInfo->pageIdx % Flash_Info.bankNum;
    if ( AddInfo->bankArry[bIdx] == MapCache[bIdx][0].blockNum ) {
        if (AddInfo->zoneNum == MapCache[bIdx][0].zoneNum ) {
            *Zoffset_p = MapCache[bIdx][0].zoneOffset;
            *Boffset_p = MapCache[bIdx][0].bankOffset;
            *ce_p = MapCache[bIdx][0].ce;
            return TRUE;
        }
    } else if ( AddInfo->bankArry[bIdx] == MapCache[bIdx][1].blockNum ) {
        if (AddInfo->zoneNum == MapCache[bIdx][1].zoneNum ) {
            *Zoffset_p = MapCache[bIdx][1].zoneOffset;
            *Boffset_p = MapCache[bIdx][1].bankOffset;
            *ce_p = MapCache[bIdx][1].ce;
            return TRUE;
        }
    }
    return FALSE;
}

uint32_t writeMapCache(add_convert *AddInfo, uint32_t Zoffset, uint32_t Boffset )
{
    uint8_t bIdx = AddInfo->pageIdx % Flash_Info.bankNum;
    uint8_t cnt = MapCacheCount[bIdx] % 2;

    MapCache[bIdx][cnt].zoneNum = AddInfo->zoneNum;
    MapCache[bIdx][cnt].blockNum = AddInfo->bankArry[bIdx];

    MapCache[bIdx][cnt].ce = AddInfo->ce;
    MapCache[bIdx][cnt].zoneOffset = Zoffset;
    MapCache[bIdx][cnt].bankOffset = Boffset;

    MapCacheCount[bIdx] = cnt ? 0 : 1;
    return TRUE;
}
/*Can not support EXTENDED_BLOCK now*/
uint32_t addressMap(add_convert *AddInfo)
{

    uint8_t  ce_idx, zoneNum;
    uint32_t zoneOffset;
    uint8_t  cacheHitFlag;
    uint8_t  bk_idx_die; /*bank index refer to the die*/
    uint32_t bankOffset, pageOffset;
    uint16_t blk_num;
    uint32_t tmp, bank_len_zone;

    ce_idx = BYTE_INVALID;
    zoneOffset = DWORD_INVALID;
    bankOffset = DWORD_INVALID;
    cacheHitFlag = readMapCache(AddInfo, &ce_idx, &zoneOffset, &bankOffset);
    /*Get the zoneOffset (unit by page) -- Start*/
    if ( zoneOffset == DWORD_INVALID) {
#if defined(__NANDFDM_TOTAL_BBM__)
        bk_idx_die = AddInfo->pageIdx % Flash_Info.bankNum; /*now bk_idx_die is bk_idx of all Flash*/
        blk_num = AddInfo->bankArry[bk_idx_die];
        if (blk_num & BBM_GLOBAL_ADDR) {
            zoneNum = 0;
        } else {
            zoneNum = AddInfo->zoneNum;
        }
#else  //#if defined(__NANDFDM_TOTAL_BBM__)
        zoneNum = AddInfo->zoneNum;
#endif //#if defined(__NANDFDM_TOTAL_BBM__)
        /*Get the CE of zone*/
        if (Flash_Info.CE_interleave_Flag == FALSE) {
            if (zoneNum < Flash_Info.zone_CE[0]) {
                ce_idx = 0;
            } else {
                ce_idx = 1;
                zoneNum -= Flash_Info.zone_CE[0];
            }
            ASSERT_RET( zoneNum <= Flash_Info.zone_CE[ce_idx], MTD_STATUS_ERROR);
        } else {
            if (AddInfo->pageIdx % Flash_Info.bankNum < Flash_Info.bank_CE[0]) {
                ce_idx = 0;
            } else {
                ce_idx = 1;
            }
            ASSERT_RET( zoneNum <= Flash_Info.zone_CE[0], MTD_STATUS_ERROR);
        }

#if 0
        tmp =  zoneNum / ( (planePage(ce_idx) * Flash_Info.bank_CE[ce_idx]) / zonePage(ce_idx) ); /* tmp = zone number aligne  n die (ex: 1 zone = 1/2 die, tmp = zoneNum/2 */
        if ( (Flash_Info.supportMPType[ce_idx]& EXTENDED_BLOCK) == EXTENDED_BLOCK) {
            zoneOffset = tmp * (planePage(ce_idx) * Flash_Info.bank_CE[ce_idx]) / 1024 * (1024 + 16) ;    /*here zoneOffset is page that part of die aligned */
        } else {
            zoneOffset = tmp * (planePage(ce_idx) * Flash_Info.bank_CE[ce_idx]) ;    /*here zoneOffset is page that part of die aligned */
        }
        zoneNum -= tmp * ( (planePage(ce_idx) * Flash_Info.bank_CE[ce_idx]) / zonePage(ce_idx) ); /*here zoneNum is reminder part of !(die aligned)*/
#else
        tmp = planePage(ce_idx) * Flash_Info.bank_CE[ce_idx]; /* tmp = n die size, n = bank_CE[ce_idx]/die plane Numer (deviceInfo_CE[ceIdx].planeNum) */
        zoneOffset =  tmp * (zoneNum / (tmp / zonePage(ce_idx)))  ; /*here zoneOffset is page that part of die aligned */
        zoneNum -= (zoneOffset / zonePage(ce_idx)); /*here zoneNum is reminder part of !(die aligned)*/
        if ( (Flash_Info.supportMPType[ce_idx]& EXTENDED_BLOCK) == EXTENDED_BLOCK) {
            zoneOffset = zoneOffset / 1024 * (1024 + 16) ;
        }
#endif

        if ( (Flash_Info.supportMPType[ce_idx]& EXTENDED_BLOCK) == EXTENDED_BLOCK) {
            ASSERT_RET(zoneNum == 0, MTD_STATUS_ERROR); /*in EXTENDED_BLOCK case, zone should be die aligned*/
        }
        bank_len_zone = zonePage(ce_idx) / Flash_Info.bank_CE[ce_idx];
        tmp = bank_len_zone * zoneNum; /* tmp = bank_len_zone * zoneNum */

        if ((Flash_Info.supportMPType[ce_idx] & (TWO_PLANE_OP | PLANE_INTERLACED)) == (TWO_PLANE_OP | PLANE_INTERLACED)) {
            zoneOffset += tmp * 2;
        } else if ((Flash_Info.supportMPType[ce_idx] & (FOUR_PLANE_OP | PLANE_INTERLACED)) == (FOUR_PLANE_OP | PLANE_INTERLACED)) {
            zoneOffset += tmp * 4;
        } else {
            zoneOffset += tmp;
        }
        /*here get the correct zoneOffset*/
    }
    /*Get the zoneOffset (unit by page) -- End*/

    /*Get the bankOffset (unit by page) -- Start*/
    if (bankOffset == DWORD_INVALID) {
        bk_idx_die = AddInfo->pageIdx % Flash_Info.bankNum; /*now bk_idx_die is bk_idx of all Flash*/
        blk_num = AddInfo->bankArry[bk_idx_die];
#if defined(__NANDFDM_TOTAL_BBM__)
        if (blk_num & BBM_GLOBAL_ADDR) {
            blk_num &= (~BBM_GLOBAL_ADDR);
        }
#endif
        if ( Flash_Info.CE_interleave_Flag == TRUE && ce_idx == 1) {
            bk_idx_die -= Flash_Info.bank_CE[0];
        }
        /*now bk_idx_die is bk_idx of ce_idx*/
        bankOffset = 0;
        if ((Flash_Info.supportMPType[ce_idx]& TWO_DIE_INTERLEAVE_OP) == TWO_DIE_INTERLEAVE_OP) {
            if (bk_idx_die >= (Flash_Info.bank_CE[ce_idx] / 2)) {
                if ( (Flash_Info.supportMPType[ce_idx]& EXTENDED_BLOCK) == EXTENDED_BLOCK) {
                    bankOffset += diePage(ce_idx) / 1024 * (1024 + 16) ; /*here zone offset is the page that part of 1st die*/
                } else {
                    bankOffset += diePage(ce_idx);
                }
                //bk_idx_die /= 2;
                bk_idx_die -= (Flash_Info.bank_CE[ce_idx] / 2);
            }
        }
        /*now bk_idx_die is bk_idx of die*/
        if ((Flash_Info.supportMPType[ce_idx] & (TWO_PLANE_OP | PLANE_INTERLACED)) == (TWO_PLANE_OP | PLANE_INTERLACED)) {
            bankOffset += (blk_num * 2 + bk_idx_die) * Flash_Info.blockPage;
        } else if ((Flash_Info.supportMPType[ce_idx] & (FOUR_PLANE_OP | PLANE_INTERLACED)) == (FOUR_PLANE_OP | PLANE_INTERLACED)) {
            bankOffset += (blk_num * 4 + bk_idx_die) * Flash_Info.blockPage;
        } else { /*PLANE_CONTINUOUS */
            if (Flash_Info.supportMPType[ce_idx]& EXTENDED_BLOCK) {
                if ( blk_num < planePage(ce_idx) / Flash_Info.blockPage ) { /*not inside the extended block area case*/
                    bankOffset += bk_idx_die * planePage(ce_idx) + blk_num * Flash_Info.blockPage;
                } else {
                    /*only EXTENDED_BLOCK is possible to run here*/
                    ASSERT_RET(Flash_Info.supportMPType[ce_idx]& EXTENDED_BLOCK, MTD_STATUS_ERROR);
                    bankOffset += diePage(ce_idx); /* + regular block area page*/
                    bankOffset += bk_idx_die * planePage(ce_idx) / 1024 * 16; /* + full extended blocks of planes*/
                    bankOffset += ( blk_num - planePage(ce_idx) / Flash_Info.blockPage ) * Flash_Info.blockPage; /* + extended block offset*/
                }
            } else {
                /*!EXTENDED_BLOCK case*/
                bankOffset += bk_idx_die * planePage(ce_idx) + blk_num * Flash_Info.blockPage;
            }
        }
        /*here get the correct bankOffset*/
    }
    /*Get the bankOffset (unit by page) -- End*/

    /*Get the pageOffset (unit by page) -- Start*/
    ASSERT_RET( AddInfo->pageIdx < Flash_Info.bankNum * Flash_Info.blockPage, MTD_STATUS_ERROR);
    pageOffset = AddInfo->pageIdx / Flash_Info.bankNum;
    /*Get the bankOffset (unit by page) -- End*/
    /*fill to AddInfo*/
    AddInfo->phyPage = zoneOffset + bankOffset + pageOffset;
    AddInfo->ce = ce_idx;
#if defined(__NANDFDM_TOTAL_BBM__)
    if (AddInfo->phyPage >= Flash_Info.devicePage[0]) {
        ASSERT_RET(Flash_Info.valid_CE[1] == TRUE, MTD_STATUS_ERROR);
        AddInfo->ce = 1;
        AddInfo->phyPage = (AddInfo->phyPage - Flash_Info.devicePage[0]);
    }
#endif

    ASSERT_RET(AddInfo->phyPage < Flash_Info.devicePage[AddInfo->ce], MTD_STATUS_ERROR);
    //AddInfo->phyPage, Flash_Info.devicePage[AddInfo->ce], AddInfo->ce);
    if (cacheHitFlag == FALSE) {
        writeMapCache(AddInfo, zoneOffset, bankOffset);
    }

    return MTD_STATUS_NO_ERROR;
}


/****************************************************************************
 * Function:
 *
 * Parameters:  Spare - spare buffer, only FDM Data (about 8 bytes) inside
 *                      if Spare == NULL, mean the spare data is careless
 *
 * Returns:     Low 2 byte: error Page
 *              High 2 byte: error code
 * Description:
 *
 ****************************************************************************/
uint32_t DAL_ProgramPage(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint16_t PageAddr, uint16_t PageLength, uint8_t *Data, uint8_t *Spare)
{
    uint8_t  m_idx = 0;
    uint8_t  bk_idx_ce;
    uint8_t  ce_idx = 0;
    uint16_t writingLen, writenLen,  i;
    uint32_t (*mtd_fp) (mtd_if_para *);
    uint32_t retVal = MTD_STATUS_NO_ERROR;
#if 0
#if defined(__DEMAND_PAGING__) && !defined(__FUE__) && !defined(__UBL__)
    uint32_t LockLength = PageLength * Flash_Info.pageSize;
#endif

    // Get demand lock before MTD lock to keep to avoid potential dead lock.
    // eg. Get MTD but can not get demand lock.
#if defined(__DEMAND_PAGING__) && !defined(__FUE__) && !defined(__UBL__)
    if (Acc_Type == FAT_ACC) {
        demp_lock_pages(DEMP_LOCK_NAND, (uint32_t)Data, LockLength);
    }
#endif
#endif
    get_MTD_lock();

#if defined (_NAND_FLASH_BOOTING_) || defined(__NANDFDM_MULTI_INSTANCE__) || (defined(__UP_PKG_ON_NAND__) && defined(NAND_SUPPORT))
    if (Acc_Type == FAT_ACC) {
        for (i = 0; i < Flash_Info.bankNum; i++) {
#if defined(__NANDFDM_TOTAL_BBM__)
            if (Bank[i]&BBM_GLOBAL_ADDR) {
                DAL_Bank_Array[i] = Bank[i];
            } else
#endif //#if defined(__NANDFDM_TOTAL_BBM__)
                if (ZoneNo == 0) {
#if !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset;
#else // !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset[CurDiskIndex];
#endif //!defined(__NANDFDM_MULTI_INSTANCE__)
                } else {
                    DAL_Bank_Array[i] = Bank[i];
                }
        }
        Bank = DAL_Bank_Array;
#if !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum;
#else // !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum[CurDiskIndex];
#endif // !defined(__NANDFDM_MULTI_INSTANCE__)
    }
#else
    if (Acc_Type != FAT_ACC) {
        //ASSERT_RET(0, MTD_STATUS_ERROR);
#if defined (_NAND_FLASH_BOOTING_)
        if (Acc_Type == FAT_ACC) {
            demp_unlock_pages(DEMP_LOCK_NAND, (uint32_t)Data, LockLength);
        }
#endif
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }
#endif /* _NAND_FLASH_BOOTING_ || defined(__NANDFDM_MULTI_INSTANCE__) || (__UP_PKG_ON_NAND__ && NAND_SUPPORT))*/
    /*boundary check*/
    if ( (ZoneNo >= Flash_Info.zoneNum) || ( PageAddr + PageLength > Flash_Info.bankNum * Flash_Info.blockPage ) ) {
        //ASSERT_RET(0, MTD_STATUS_ERROR);
#if defined(__DEMAND_PAGING__) && !defined(__FUE__) && !defined(__UBL__)
        if (Acc_Type == FAT_ACC) {
            demp_unlock_pages(DEMP_LOCK_NAND, (uint32_t)Data, LockLength);
        }
#endif
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }

    writenLen = 0;
    kal_mem_set(&Mtd_para, 0x00, sizeof(Mtd_para));
    Add_Cnvt.zoneNum = ZoneNo ;
    Add_Cnvt.bankArry = Bank;
    while (PageLength) {
        /*Get the mtd_fp, m_idx & writingLen*/
        mtd_fp = NULL;
        writingLen = 0;
        if ( Flash_Info.CE_interleave_Flag == TRUE ) {
            if ( (PageAddr % Flash_Info.bankNum) == 0) {
                if (PageLength >= Flash_Info.bankNum * 2 &&
                        Flash_Info.CE_cache_interleave_Flag == TRUE) {
                    writingLen = Flash_Info.bankNum * (PageLength / Flash_Info.bankNum);
                    writingLen = writingLen < 16 ? writingLen : 16; /*Maximun number to MTD is 16*/
                    m_idx = Flash_Info.CE_cache_interleave_mOP_Idx[0];
                    mtd_fp = MTD_CE_Interleave_CacheProgram;
                } else if ( PageLength >= Flash_Info.bankNum ) {
                    writingLen = Flash_Info.bankNum ;
                    m_idx = Flash_Info.CE_interleave_mOP_Idx[0];
                    mtd_fp = MTD_CE_Interleave_Program;
                }
            }
        }
        if (mtd_fp == NULL) {
            Add_Cnvt.pageIdx = PageAddr;
            addressMap(&Add_Cnvt);
            ce_idx = Add_Cnvt.ce;
            bk_idx_ce = PageAddr % Flash_Info.bankNum;
            if ( Flash_Info.CE_interleave_Flag == TRUE && ce_idx == 1) {
                bk_idx_ce -= Flash_Info.bank_CE[0];
            }
            for (m_idx = 0; m_idx < MAX_MULTIPAGE_FUNCTION_NUM; m_idx++) {
                if (Flash_Info.multiProgram[ce_idx][m_idx].func_p != NULL
                        && ((bk_idx_ce % Flash_Info.multiProgram[ce_idx][m_idx].startAlign) == 0)) {
                    /*cache program check*/
                    if ( Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum == 0xFF
                            && (PageLength >= Flash_Info.multiProgram[ce_idx][m_idx].startAlign * 2)
                            && Flash_Info.CE_interleave_Flag == FALSE ) {
                        writingLen = (Flash_Info.multiProgram[ce_idx][m_idx].startAlign) * (PageLength / (Flash_Info.multiProgram[ce_idx][m_idx].startAlign));
                        writingLen = writingLen < 16 ? writingLen : 16; /*Maximun number to MTD is 16*/
                        mtd_fp = MTD_CacheProgram;
                        break;
                    } else if (PageLength >= Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum) {
                        writingLen = (Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum);
                        mtd_fp = MTD_Program;
                        break;
                    }
                }
            }
            /*here m_idx point to the corresponding multiProgram function */
        }
        ASSERT_RET(writingLen <= PageLength, MTD_STATUS_ERROR);
        ASSERT_RET(mtd_fp != NULL, MTD_STATUS_ERROR);
        //writingLen = writingLen < 16 ? writingLen : 16; /*limited by MTD return*/
        /* Fill Mtd_para*/
        Mtd_para.buff = &Data[(writenLen * Flash_Info.pageSize)];
        Mtd_para.spare = &Spare[0];
        Mtd_para.m_idx = m_idx;
        Mtd_para.length = writingLen;
        if (mtd_fp == MTD_CacheProgram || mtd_fp == MTD_CE_Interleave_CacheProgram) {
            /*cache program case*/
            for ( i = 0; i < Flash_Info.bankNum; i++) {
                if (Acc_Type == RAW_ACC) {
                    uint32_t ZoneLen;

                    ZoneLen = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
                    Mtd_para.pageIdx[i] = ((ZoneNo * ZoneLen) + (Bank[0] * Flash_Info.bankNum)) * Flash_Info.blockPage + PageAddr + i;
                    Mtd_para.ce = 0;
                } else {
                    Add_Cnvt.pageIdx = PageAddr + i;
                    addressMap(&Add_Cnvt);
                    Mtd_para.pageIdx[i] = Add_Cnvt.phyPage;
                    Mtd_para.ce = Add_Cnvt.ce;
                }
                if (Flash_Info.deviceInfo_CE[Add_Cnvt.ce].deviceSize * 1024 * 1024 <= Flash_Info.pageSize * Add_Cnvt.phyPage) {
                    //ASSERT_RET(0, MTD_STATUS_ERROR);
#if defined(__DEMAND_PAGING__) && !defined(__FUE__) && !defined(__UBL__)
                    if (Acc_Type == FAT_ACC) {
                        demp_unlock_pages(DEMP_LOCK_NAND, (uint32_t)Data, LockLength);
                    }
#endif
                    free_MTD_lock();
                    ASSERT_RET(0, MTD_STATUS_ERROR);
                    //return MTD_STATUS_PARA_ERROR;
                }
            }
        } else {
            /*! cache program case*/
            ASSERT_RET(writingLen <= MAX_MULTIPAGE_NUM, MTD_STATUS_ERROR);
            for ( i = 0; i < writingLen; i++) {
                if (Acc_Type == RAW_ACC) {
                    uint32_t ZoneLen;

                    ZoneLen = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
                    Mtd_para.pageIdx[i] = ((ZoneNo * ZoneLen) + (Bank[0] * Flash_Info.bankNum)) * Flash_Info.blockPage + PageAddr + i;
                    Mtd_para.ce = 0;
                } else {
                    Add_Cnvt.pageIdx = PageAddr + i;
                    addressMap(&Add_Cnvt);
                    Mtd_para.pageIdx[i] = Add_Cnvt.phyPage;
                    Mtd_para.ce = Add_Cnvt.ce;
                }
                if (Flash_Info.deviceInfo_CE[Add_Cnvt.ce].deviceSize * 1024 * 1024 <= Flash_Info.pageSize * Add_Cnvt.phyPage) {
                    //ASSERT_RET(0, MTD_STATUS_ERROR);
#if defined(__DEMAND_PAGING__) && !defined(__FUE__) && !defined(__UBL__)
                    if (Acc_Type == FAT_ACC) {
                        demp_unlock_pages(DEMP_LOCK_NAND, (uint32_t )Data, LockLength);
                    }
#endif
                    free_MTD_lock();
                    ASSERT_RET(0, MTD_STATUS_ERROR);
                    //return MTD_STATUS_PARA_ERROR;
                }
            }
        }
#if defined DAL_VERIFICATION
        if (Fail_Bank != 0xFF) {
            /*test case*/
            if (Fail_Bank >= PageAddr && Fail_Bank < (PageAddr + writingLen) ) {
                Mtd_Fail_Offset = Fail_Bank - PageAddr;
            }
        }
#endif

        Mtd_para.opFlag |= ONE_SPARE;
        /*call MTD function*/
        retVal = mtd_fp(&Mtd_para);
#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__) )
        if ((PFTrigger == TRUE) && (PFTestCase & PF_DAL_TRIGGER)) {
            uint8_t Index, Count;

            PFTrigger = FALSE;
            for (Index = 0; Index < writingLen; Index++) {
                Count = 0;
                while (1) {
                    if (ProgBadAddr[Count].Zone == 0xff) {
                        break;
                    }
                    if ( (ZoneNo == ProgBadAddr[Count].Zone) &&
                            ((Add_Cnvt.bankArry[(PageAddr + Index) % Flash_Info.bankNum] & (~BBM_GLOBAL_ADDR)) == ProgBadAddr[Count].Block) &&
                            ((ProgBadAddr[Count].Page == 0xffff) || (Add_Cnvt.pageIdx == ProgBadAddr[Count].Page)) ) {
                        retVal |= (0x01 << ((PageAddr + Index) % Flash_Info.bankNum));
                        if (PF_EF_AutoTest == TRUE) {
                            LOG_I(common, "Program Before PFTestCase:0x%X Block:0x%X Page:0x%X\r\n", PFTestCase, ProgBadAddr[Count].Block, Add_Cnvt.pageIdx );

                            PFTestCase &= (~PFTrigerByTestCase);
                            LOG_I(common, "Program After PFTestCase:0x%X Block:0x%X Page:0x%X\r\n", PFTestCase, ProgBadAddr[Count].Block, Add_Cnvt.pageIdx );

                        }
                    }
                    Count++;
                }

            }
        }
#endif

#if defined DAL_VERIFICATION
        if (Fail_Bank != 0xFF) {
            /*test case*/
            if (Fail_Bank >= PageAddr && Fail_Bank < (PageAddr + writingLen) ) {
                Mtd_Fail_Offset = 0xFF;    /*Reset*/
            }
        }
#endif
        if ( retVal != MTD_STATUS_NO_ERROR) {
            retVal = (retVal & 0xFFFF0000) | ((retVal & 0x0000FFFF) << (PageAddr % Flash_Info.bankNum));
            break;
        }
        /*set PageAddr & PageLength*/
        PageAddr += writingLen;
        writenLen += writingLen;
        PageLength -= writingLen;
    }
#if defined(__DEMAND_PAGING__) && !defined(__FUE__) && !defined(__UBL__)
    if (Acc_Type == FAT_ACC) {
        demp_unlock_pages(DEMP_LOCK_NAND, (uint32_t )Data, LockLength);
    }
#endif
    free_MTD_lock();
    return retVal;
}


/****************************************************************************
 * Function:
 *
 * Parameters:  Spare - spare buffer, it will put all the spare data of the page
 *
 * Returns:     Low 2 byte: error code
 *              High 2 byte: error Page
 * Description:
 *
 ****************************************************************************/
uint32_t DAL_ReadPage_internal(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint16_t PageAddr, uint16_t PageLength, uint8_t *Data, uint8_t *Spare)
{
    //uint8_t  m_idx = 0;
    //uint8_t  bk_idx_ce;
    //uint8_t  ce_idx = 0;
    uint16_t  readingLen, Done_Len, i;
    uint32_t (*mtd_fp) (mtd_if_para *);
    uint32_t retVal = MTD_STATUS_NO_ERROR, mtd_retVal;

    get_MTD_lock();
#if defined (_NAND_FLASH_BOOTING_) || defined(__NANDFDM_MULTI_INSTANCE__) || (defined(__UP_PKG_ON_NAND__) && defined(NAND_SUPPORT))
#if 0
    if (Acc_Type == FAT_ACC) {
        for (i = 0; i < Flash_Info.bankNum; i++) {
#if defined(__NANDFDM_TOTAL_BBM__)
            if (Bank[i]&BBM_GLOBAL_ADDR) {
                DAL_Bank_Array[i] = Bank[i];
            } else
#endif //#if defined(__NANDFDM_TOTAL_BBM__)
                if (ZoneNo == 0) {
#if !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset;
#else // !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset[CurDiskIndex];
#endif // !defined(__NANDFDM_MULTI_INSTANCE__)
                } else {
                    DAL_Bank_Array[i] = Bank[i];
                }
        }
        Bank = DAL_Bank_Array;
#if !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum;
#else  // !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum[CurDiskIndex];
#endif	// !defined(__NANDFDM_MULTI_INSTANCE__)
    }
#endif
#else
    if (Acc_Type != FAT_ACC) {
        //ASSERT_RET(0, MTD_STATUS_ERROR);
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }
#endif /* _NAND_FLASH_BOOTING_ || defined(__NANDFDM_MULTI_INSTANCE__) || (__UP_PKG_ON_NAND__ && NAND_SUPPORT)) */
    /*boundary check*/
    if ( (ZoneNo >= Flash_Info.zoneNum) || ( PageAddr + PageLength > Flash_Info.bankNum * Flash_Info.blockPage ) ) {
        //force_ASSERT(0,ZoneNo,(PageAddr+PageLength),0);
        //ASSERT_RET(0, MTD_STATUS_ERROR);
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }

    Done_Len = 0;
    kal_mem_set(&Mtd_para, 0x00, sizeof(Mtd_para));
    Add_Cnvt.zoneNum = ZoneNo ;
    Add_Cnvt.bankArry = Bank;
    while ( PageLength ) {
        /*Get the mtd_fp, m_idx & readingLen*/
        readingLen = 1;
        mtd_fp = MTD_Read;
        /* Fill Mtd_para*/
        Mtd_para.buff = &Data[(Done_Len * Flash_Info.pageSize)];
        if (Spare == NULL) {
            Mtd_para.spare = NULL;
        } else {
            Mtd_para.spare = &Spare[(Done_Len * Flash_Info.spareSize)];
        }
        Mtd_para.length = readingLen;
        for ( i = 0; i < readingLen; i++) {
            Add_Cnvt.pageIdx = PageAddr + i;
            addressMap(&Add_Cnvt);
            Mtd_para.pageIdx[i] = Add_Cnvt.phyPage;
            Mtd_para.ce = Add_Cnvt.ce;
#if 0 /*bootloader test*/
            LOG_I(common, "Read CE%d page %d to 0x%x\n\r", Mtd_para.ce, Mtd_para.pageIdx[i], Mtd_para.buff);
#endif
            if (Flash_Info.deviceInfo_CE[Add_Cnvt.ce].deviceSize * 1024 * 1024 <= Flash_Info.pageSize * Add_Cnvt.phyPage) {
                //ASSERT_RET(0, MTD_STATUS_ERROR);
                free_MTD_lock();
                ASSERT_RET(0, MTD_STATUS_ERROR);
                //return MTD_STATUS_PARA_ERROR;
            }
        }
#if defined DAL_VERIFICATION
        if (Fail_Bank != 0xFF) {
            /*test case*/
            if (Fail_Bank >= PageAddr && Fail_Bank < (PageAddr + readingLen) ) {
                Mtd_Fail_Offset = Fail_Bank - PageAddr;
            }
        }
#endif
        /*call MTD function*/

        mtd_retVal = mtd_fp(&Mtd_para);
#if defined DAL_VERIFICATION
        if (Fail_Bank != 0xFF) {
            /*test case*/
            if (Fail_Bank >= PageAddr && Fail_Bank < (PageAddr + readingLen) ) {
                Mtd_Fail_Offset = 0xFF;    /*Reset*/
            }
        }
#endif
        if ( mtd_retVal != MTD_STATUS_NO_ERROR) {
            retVal |= (mtd_retVal & 0xFFFF0000) | ((mtd_retVal & 0x0000FFFF) << (PageAddr % Flash_Info.bankNum));
            //break;/*Do not stop even read error*/
        }
        /*set PageAddr & PageLength*/
        PageAddr += readingLen;
        Done_Len += readingLen;
        PageLength -= readingLen;
    }
    free_MTD_lock();
    return retVal;
}

//if read fail, retry twice
uint32_t DAL_ReadPage(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint16_t PageAddr, uint16_t PageLength, uint8_t *Data, uint8_t *Spare)
{
    uint32_t retVal = MTD_STATUS_NO_ERROR;
    uint32_t trycount = 0;

    retVal = DAL_ReadPage_internal(Acc_Type, ZoneNo, Bank, PageAddr, PageLength, Data, Spare);
    if ((retVal & 0xffff0000) == MTD_STATUS_ERROR) {
DAL_ReadPage_Retry_R:
        trycount++;
#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__))
        LOG_I(common, "DAL_ReadPage Retry %d time\r\n", trycount);

#endif
        if ((DAL_ReadPage_internal(Acc_Type, ZoneNo, Bank, PageAddr, PageLength, Data, Spare) & 0xffff0000) == MTD_STATUS_ERROR) {
            if (trycount < 3) {
                goto DAL_ReadPage_Retry_R;
            }
        }
    }
    //return first time read return value, even second time read succeed.
    //when first time read fail, caller will know this page is risky.
    return retVal;
}
/****************************************************************************
 * Function:    DAL_CopyPage
 *
 * Parameters:  uint8_t ZoneNo     The Zone number
 *              uint16_t *srcBank  The copy source bank array
 *              uint16_t *dstBank  The copy source bank array
 *              uint16_t PageLength The desired copy length (unit by page)
 *              uint8_t *Data_buff   copy function Working buffer
 *              uint8_t *Spare_buff  copy function Working buffer
 *              uint32_t buff_len  Working buffer length, unit by page
 *              uint8_t  SDataNumber - The number of Upper layer sending Data in Spare_buff,
 *                                      if >= 1 means dst spare should be assigned with data in Spare_buff
 * Returns:     Low 2 byte: error code
 *              High 2 byte: error Page
 * Description: [Important Note]: If PageLength > 1 The source pages should have the same spare content!
 *
 ****************************************************************************/
uint32_t DAL_CopyPage(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *srcBank, uint16_t *dstBank, uint16_t srcPageAddr, uint16_t dstPageAddr, uint16_t PageLength, uint8_t *Data_buff, uint8_t *Spare_buff, uint32_t buff_len, uint8_t SDataNumber)
{
    uint8_t   fgNeedCheckBound = TRUE;
    uint8_t  m_idx = 0;
    uint8_t  bk_idx_ce;
    uint8_t  ce_idx = 0;
    uint8_t  bCurrZone;
    uint16_t writingLen, i;
    uint16_t *pCurrSrcBank, *pCurrDstBank;

    uint32_t (*mtd_program_fp) (mtd_if_para *);
    uint32_t retVal = MTD_STATUS_NO_ERROR, mtd_retVal;
    bCurrZone = ZoneNo;
    pCurrSrcBank = srcBank;
    pCurrDstBank = dstBank;
    writingLen = 0;
    while ( PageLength ) {
        get_MTD_lock();
#if defined (_NAND_FLASH_BOOTING_) || defined(__NANDFDM_MULTI_INSTANCE__) || (defined(__UP_PKG_ON_NAND__) && defined(NAND_SUPPORT))
        if (Acc_Type == FAT_ACC) {
            for (i = 0; i < Flash_Info.bankNum; i++) {
#if (defined(__NANDFDM_TOTAL_BBM__) && defined(__NANDFDM_MULTI_INSTANCE__))
                if (srcBank[i]&BBM_GLOBAL_ADDR) {
                    DAL_Bank_Array[i] = srcBank[i];
                } else {
                    if (ZoneNo == 0) {
                        DAL_Bank_Array[i] = srcBank[i] + FAT_Start_Section_Offset[CurDiskIndex];
                    } else {
                        DAL_Bank_Array[i] = srcBank[i];
                    }
                }
                if (dstBank[i]&BBM_GLOBAL_ADDR) {
                    DAL_Bank_Array2[i] = dstBank[i];
                } else {
                    if (ZoneNo == 0) {
                        DAL_Bank_Array2[i] = dstBank[i] + FAT_Start_Section_Offset[CurDiskIndex];
                    } else {
                        DAL_Bank_Array2[i] = dstBank[i];
                    }
                }
#else
                if (ZoneNo == 0) {
#if !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = srcBank[i] + FAT_Start_Section_Offset;
                    DAL_Bank_Array2[i] = dstBank[i] + FAT_Start_Section_Offset;
#else  // !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = srcBank[i] + FAT_Start_Section_Offset[CurDiskIndex];
                    DAL_Bank_Array2[i] = dstBank[i] + FAT_Start_Section_Offset[CurDiskIndex];
#endif // !defined(__NANDFDM_MULTI_INSTANCE__)
                } else {
                    DAL_Bank_Array[i] = srcBank[i];
                    DAL_Bank_Array2[i] = dstBank[i];
                }
#endif //#if defined(__NANDFDM_TOTAL_BBM__)
            }
            pCurrSrcBank = DAL_Bank_Array;
            pCurrDstBank = DAL_Bank_Array2;
#if !defined(__NANDFDM_MULTI_INSTANCE__)
            bCurrZone = ZoneNo + FAT_Start_ZoneNum;
#else  // !defined(__NANDFDM_MULTI_INSTANCE__)
            bCurrZone = ZoneNo +  FAT_Start_ZoneNum[CurDiskIndex];
#endif // !defined(__NANDFDM_MULTI_INSTANCE__)
        }
#else
        if (Acc_Type != FAT_ACC) {            
            free_MTD_lock();
            ASSERT_RET(0, MTD_STATUS_ERROR);
            //return MTD_STATUS_PARA_ERROR;
        }
#endif /* _NAND_FLASH_BOOTING || defined(__NANDFDM_MULTI_INSTANCE__) || (__UP_PKG_ON_NAND__ && NAND_SUPPORT)) */

        if (fgNeedCheckBound == TRUE) {
            fgNeedCheckBound = FALSE;
            /*parameter check*/
            if ( bCurrZone >= Flash_Info.zoneNum || srcPageAddr + PageLength > Flash_Info.bankNum * Flash_Info.blockPage ||
                    dstPageAddr + PageLength > Flash_Info.bankNum * Flash_Info.blockPage ||
                    (srcPageAddr % Flash_Info.bankNum) != (dstPageAddr % Flash_Info.bankNum) || SDataNumber >= Flash_Info.spareSize) {
                free_MTD_lock();
                ASSERT_RET(0, MTD_STATUS_ERROR);
                //return MTD_STATUS_PARA_ERROR;
            }
        }
        if ( SDataNumber > 0) {
            kal_mem_set(DAL_spare_buff, 0xFF, Flash_Info.spareSize );
            kal_mem_cpy(DAL_spare_buff, Spare_buff, SDataNumber );
        }

        /*read + write method*/
        Add_Cnvt.zoneNum = bCurrZone ;

        /*Get the mtd_program_fp, m_idx & writingLen*/
        mtd_program_fp = NULL;
        if ( Flash_Info.CE_interleave_Flag == TRUE ) {
            if ( (dstPageAddr % Flash_Info.bankNum) == 0) {
                if ( (PageLength < buff_len ? PageLength : buff_len) >= Flash_Info.bankNum * 2 &&
                        Flash_Info.CE_cache_interleave_Flag == TRUE) {
                    writingLen = Flash_Info.bankNum * ( (PageLength < buff_len ? PageLength : buff_len) / Flash_Info.bankNum );
                    m_idx = Flash_Info.CE_cache_interleave_mOP_Idx[0];
                    mtd_program_fp = MTD_CE_Interleave_CacheProgram;
                } else if ( (PageLength < buff_len ? PageLength : buff_len) >= Flash_Info.bankNum ) {
                    writingLen = Flash_Info.bankNum;
                    m_idx = Flash_Info.CE_interleave_mOP_Idx[0];
                    mtd_program_fp = MTD_CE_Interleave_Program;
                }
            }
        }
        if (mtd_program_fp == NULL) {
            Add_Cnvt.pageIdx = dstPageAddr;
            Add_Cnvt.bankArry = pCurrDstBank;
            addressMap(&Add_Cnvt);
            ce_idx = Add_Cnvt.ce;
            bk_idx_ce = dstPageAddr % Flash_Info.bankNum;
            if ( Flash_Info.CE_interleave_Flag == TRUE && ce_idx == 1) {
                bk_idx_ce -= Flash_Info.bank_CE[0];
            }
            for (m_idx = 0; m_idx < MAX_MULTIPAGE_FUNCTION_NUM; m_idx++) {
                if ( Flash_Info.multiProgram[ce_idx][m_idx].func_p != NULL
                        && ((bk_idx_ce % Flash_Info.multiProgram[ce_idx][m_idx].startAlign) == 0) ) {
                    /*cache program check*/
                    if ( Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum == 0xFF
                            && ( (PageLength < buff_len ? PageLength : buff_len) >= Flash_Info.multiProgram[ce_idx][m_idx].startAlign * 2)
                            && Flash_Info.CE_interleave_Flag == FALSE ) {
                        writingLen = (Flash_Info.multiProgram[ce_idx][m_idx].startAlign) * ((PageLength < buff_len ? PageLength : buff_len) / (Flash_Info.multiProgram[ce_idx][m_idx].startAlign));
                        mtd_program_fp = MTD_CacheProgram;
                        break;
                    } else if ( (PageLength < buff_len ? PageLength : buff_len) >= Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum) {
                        writingLen = (Flash_Info.multiProgram[ce_idx][m_idx].multiPageNum);
                        mtd_program_fp = MTD_Program;
                        break;
                    }
                }
            }
            /*here m_idx point to the corresponding multiProgram function */
        }
        ASSERT_RET(writingLen <= PageLength, MTD_STATUS_ERROR);
        ASSERT_RET(mtd_program_fp != NULL, MTD_STATUS_ERROR);
        /*here get all information to process multi-page write, now read data out*/
        /* Fill Mtd_para*/
        kal_mem_set(&Mtd_para, 0x00, sizeof(Mtd_para));
        Mtd_para.length = 1; /*single read only*/
        Add_Cnvt.bankArry = pCurrSrcBank;
        for ( i = 0; i < writingLen; i++) {
            Add_Cnvt.pageIdx = srcPageAddr + i;
            addressMap(&Add_Cnvt);
            Mtd_para.pageIdx[0] = Add_Cnvt.phyPage;
            Mtd_para.ce = Add_Cnvt.ce;
            Mtd_para.buff = &Data_buff[ i * Flash_Info.pageSize ];
            if (SDataNumber > 0) {
                Mtd_para.spare = &Spare_buff[0];
            } else {
                Mtd_para.spare = &Spare_buff[ i * Flash_Info.spareSize ];
            }
            if (Flash_Info.deviceInfo_CE[Add_Cnvt.ce].deviceSize * 1024 * 1024 <= Flash_Info.pageSize * Add_Cnvt.phyPage) {
                free_MTD_lock();
                ASSERT_RET(0, MTD_STATUS_ERROR);                
                //return MTD_STATUS_PARA_ERROR;
            }
            /*call MTD_Read function*/
            mtd_retVal = MTD_Read(&Mtd_para);
            if ( (mtd_retVal & 0xffff0000) == MTD_STATUS_ERROR) {
                uint8_t try_count = 0;
DAL_COPY_PAGE_RETRY:
                //re-try to read the same page when read fail.
                if (try_count < 3) {
#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__))
                    LOG_I(common, "DAL_CopyPage Retry %d time\r\n", try_count);

#endif
                    mtd_retVal = MTD_Read(&Mtd_para);
                    if ( (mtd_retVal & 0xffff0000) == MTD_STATUS_ERROR) {
                        try_count++;
                        goto DAL_COPY_PAGE_RETRY;

                    }
                }
            }
            //if ( mtd_retVal != MTD_STATUS_NO_ERROR)
            //{
            //retVal |= (mtd_retVal&0xFFFF0000)|((mtd_retVal&0x0000FFFF)<<((srcPageAddr+i)%Flash_Info.bankNum));
            //break;/*Do not stop even read error*/
            //}
        }
        /*Now process the write part*/
        /* Fill Mtd_para*/
        kal_mem_set(&Mtd_para, 0x00, sizeof(Mtd_para));
        Mtd_para.buff = Data_buff;
        Mtd_para.spare = (SDataNumber > 0 ? (uint8_t *)DAL_spare_buff : Spare_buff);
        Mtd_para.m_idx = m_idx;
        Mtd_para.length = writingLen;

        Add_Cnvt.bankArry = pCurrDstBank;
        for ( i = 0; i < writingLen; i++) {
            Add_Cnvt.pageIdx = dstPageAddr + i;
            addressMap(&Add_Cnvt);
            Mtd_para.pageIdx[i] = Add_Cnvt.phyPage;
            Mtd_para.ce = Add_Cnvt.ce;
            if (Flash_Info.deviceInfo_CE[Add_Cnvt.ce].deviceSize * 1024 * 1024 <= Flash_Info.pageSize * Add_Cnvt.phyPage) {
                free_MTD_lock();
                ASSERT_RET(0, MTD_STATUS_ERROR);
                //return MTD_STATUS_PARA_ERROR;
            }
        }
#if defined DAL_VERIFICATION
        if (Fail_Bank != 0xFF) {
            /*test case*/
            if (Fail_Bank >= srcPageAddr && Fail_Bank < (srcPageAddr + writingLen) ) {
                Mtd_Fail_Offset = Fail_Bank - srcPageAddr;
            }
        }
#endif
        /*call MTD function*/
        if (SDataNumber > 0) {
            Mtd_para.opFlag |= ONE_SPARE;
        } else {
            Mtd_para.opFlag &= (~ONE_SPARE);
        }

        mtd_retVal = mtd_program_fp(&Mtd_para);
#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__))
        if ((PFTrigger == TRUE) && (PFTestCase & PF_DAL_TRIGGER)) {
            uint8_t Index, Count;

            PFTrigger = FALSE;
            for (Index = 0; Index < writingLen; Index++) {
                Count = 0;
                while (1) {
                    if (ProgBadAddr[Count].Zone == 0xff) {
                        break;
                    }
                    if ( (bCurrZone == ProgBadAddr[Count].Zone) &&
                            ((Add_Cnvt.bankArry[(dstPageAddr + Index) % Flash_Info.bankNum] & (~BBM_GLOBAL_ADDR)) == ProgBadAddr[Count].Block) &&
                            ((ProgBadAddr[Count].Page == 0xffff) || (Add_Cnvt.pageIdx == ProgBadAddr[Count].Page)) ) {
                        retVal |= (0x01 << ((dstPageAddr + Index) % Flash_Info.bankNum));
                        if (PF_EF_AutoTest == TRUE) {
                            LOG_I(common, "Copy Before PFTestCase:0x%X Block:0x%X Page:0x%X\r\n", PFTestCase, ProgBadAddr[Count].Block, Add_Cnvt.pageIdx );

                            PFTestCase &= (~PFTrigerByTestCase);
                            LOG_I(common, "Copy After PFTestCase:0x%X Block:0x%X Page:0x%X\r\n", PFTestCase, ProgBadAddr[Count].Block, Add_Cnvt.pageIdx );


                        }
                    }
                    Count++;
                }

            }
        }
#endif
#if defined DAL_VERIFICATION
        if (Fail_Bank != 0xFF) {
            /*test case*/
            if (Fail_Bank >= srcPageAddr && Fail_Bank < (srcPageAddr + writingLen) ) {
                Mtd_Fail_Offset = 0xFF;    /*Reset*/
            }
        }
#endif
        if ( mtd_retVal != MTD_STATUS_NO_ERROR) {
            retVal |= (mtd_retVal & 0xFFFF0000) | ((mtd_retVal & 0x0000FFFF) << (dstPageAddr % Flash_Info.bankNum));
            break;
        }
        /*set PageAddr & PageLength*/
        srcPageAddr += writingLen;
        dstPageAddr += writingLen;
        PageLength -= writingLen;
        free_MTD_lock();
    }

    return retVal;
}

/****************************************************************************
 * Function:
 *
 * Parameters:
 *
 * Returns:     Low 2 byte: error code
 *              High 2 byte: error Page
 * Description:
 *
 ****************************************************************************/
uint32_t DAL_EraseBlock(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint8_t start, uint8_t length)
{
    uint8_t  m_idx = 0;
    uint8_t  bk_idx_ce;
    uint8_t  ce_idx = 0;
    uint8_t  erasingLen, i;
    uint32_t (*mtd_fp) (mtd_if_para *);
    uint32_t retVal = MTD_STATUS_NO_ERROR;

    get_MTD_lock();
#if defined (_NAND_FLASH_BOOTING_) || defined(__NANDFDM_MULTI_INSTANCE__) || (defined(__UP_PKG_ON_NAND__) && defined(NAND_SUPPORT))
    if (Acc_Type == FAT_ACC) {
        for (i = 0; i < Flash_Info.bankNum; i++) {
#if defined(__NANDFDM_TOTAL_BBM__)
            if (Bank[i]&BBM_GLOBAL_ADDR) {
                DAL_Bank_Array[i] = Bank[i];
            } else
#endif //#if defined(__NANDFDM_TOTAL_BBM__)
                if (ZoneNo == 0) {
#if !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset;
#else // !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset[CurDiskIndex];
#endif //!defined(__NANDFDM_MULTI_INSTANCE__)
                } else {
                    DAL_Bank_Array[i] = Bank[i];
                }
        }
        Bank = DAL_Bank_Array;
#if !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum;
#else // !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum[CurDiskIndex];
#endif //!defined(__NANDFDM_MULTI_INSTANCE__)
    }
#else
    if (Acc_Type != FAT_ACC) {
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }
#endif /* _NAND_FLASH_BOOTING_ || defined(__NANDFDM_MULTI_INSTANCE__) || (__UP_PKG_ON_NAND__ && NAND_SUPPORT)) */
    /*boundary check*/
    if ( ZoneNo >= Flash_Info.zoneNum || start + length > Flash_Info.bankNum ) {
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }

    kal_mem_set(&Mtd_para, 0x00, sizeof(Mtd_para));
    Add_Cnvt.zoneNum = ZoneNo ;
    Add_Cnvt.bankArry = Bank;
    while ( length ) {
        /*Get the mtd_fp, m_idx & erasingLen*/
        mtd_fp = NULL;
        erasingLen = 0;
        Add_Cnvt.pageIdx = start;
        addressMap(&Add_Cnvt);
        ce_idx = Add_Cnvt.ce;
        bk_idx_ce = start % Flash_Info.bankNum;
        if ( Flash_Info.CE_interleave_Flag == TRUE && ce_idx == 1) {
            bk_idx_ce -= Flash_Info.bank_CE[0];
        }
        for (m_idx = 0; m_idx < MAX_MULTIPAGE_FUNCTION_NUM; m_idx++) {
            if ( Flash_Info.multiErase[ce_idx][m_idx].func_p != NULL
                    && ((bk_idx_ce % Flash_Info.multiErase[ce_idx][m_idx].startAlign) == 0)
                    && length >= Flash_Info.multiErase[ce_idx][m_idx].multiPageNum ) {
                erasingLen = (Flash_Info.multiErase[ce_idx][m_idx].multiPageNum);
                mtd_fp = MTD_Erase;
                break;
            }
        }
        /*here m_idx point to the corresponding multiErase function */
        ASSERT_RET(erasingLen < MAX_MULTIPAGE_NUM, MTD_STATUS_ERROR);
        ASSERT_RET(erasingLen <= length, MTD_STATUS_ERROR);
        ASSERT_RET(mtd_fp != NULL, MTD_STATUS_ERROR);
        /* Fill Mtd_para*/
        Mtd_para.buff = NULL;
        Mtd_para.spare = NULL;
        Mtd_para.m_idx = m_idx;
        Mtd_para.length = erasingLen;
        for ( i = 0; i < erasingLen; i++) {
            if (Acc_Type == RAW_ACC) {
                uint32_t ZoneLen;
                ZoneLen = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
                Mtd_para.pageIdx[i] = ((ZoneNo * ZoneLen) + (Bank[0] * Flash_Info.bankNum) + start + i) * Flash_Info.blockPage;
                Mtd_para.ce = 0;
            } else {
                Add_Cnvt.pageIdx = start + i; /*first page of block*/
                addressMap(&Add_Cnvt);
                Mtd_para.pageIdx[i] = Add_Cnvt.phyPage;
                Mtd_para.ce = Add_Cnvt.ce;
            }
            if (Flash_Info.deviceInfo_CE[Add_Cnvt.ce].deviceSize * 1024 * 1024 <= Flash_Info.pageSize * Add_Cnvt.phyPage) {
                free_MTD_lock();
                ASSERT_RET(0, MTD_STATUS_ERROR);
                //return MTD_STATUS_PARA_ERROR;
            }
        }
#if defined DAL_VERIFICATION
        if (Fail_Bank != 0xFF) {
            /*test case*/
            if (Fail_Bank >= start && Fail_Bank < (start + erasingLen) ) {
                Mtd_Fail_Offset = Fail_Bank - start;
            }
        }
#endif
        /*call MTD function*/
        retVal = mtd_fp(&Mtd_para);
#if defined DAL_VERIFICATION
        if (Fail_Bank != 0xFF) {
            /*test case*/
            if (Fail_Bank >= start && Fail_Bank < (start + erasingLen) ) {
                Mtd_Fail_Offset = 0xFF;    /*Reset*/
            }
        }
#endif
#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__))
        if ((EFTrigger == TRUE) && (EFTestCase & EF_DAL_TRIGGER)) {
            uint8_t Index, Count;

            EFTrigger = FALSE;
            for (Index = 0; Index < erasingLen; Index++) {
                Count = 0;
                while (1) {
                    if (EraseBadAddr[Count].Zone == 0xff) {
                        break;
                    }
                    if ((ZoneNo == EraseBadAddr[Count].Zone) &&
                            ((Add_Cnvt.bankArry[start + Index] & (~BBM_GLOBAL_ADDR)) == EraseBadAddr[Count].Block) &&
                            ((start + Index) == EraseBadAddr[Count].Bank)) {
                        retVal |= (0x01 << (start + Index));
                        if (PF_EF_AutoTest == TRUE) {
                            LOG_I(common, "Before EFTestCase:0x%X \r\n", EFTestCase);
                            EFTestCase &= (~EFTrigerByTestCase);
                            LOG_I(common, "After EFTestCase:0x%X \r\n", EFTestCase);
                        }
                    }
                    Count++;
                }
            }
        }
#endif

        if ( retVal != MTD_STATUS_NO_ERROR) {
            retVal = (retVal & 0xFFFF0000) | ((retVal & 0x0000FFFF) << start);
            break;
        }
        start += erasingLen;
        length -= erasingLen;
    }
    free_MTD_lock();
    return retVal;
}

/****************************************************************************
 * Function:
 *
 * Parameters:
 *
 * Returns:
 *
 * Description:
 *
 ****************************************************************************/
uint32_t DAL_MarkBlockBad(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint8_t Block_offset, uint8_t EraseBlock, uint32_t *Data_buff, uint32_t *Spare_buff)
{
    uint8_t  m_idx = 0;
    uint8_t  ce_idx = 0;
    uint32_t ZoneLen = 0;

    get_MTD_lock();
#if defined (_NAND_FLASH_BOOTING_) || defined(__NANDFDM_MULTI_INSTANCE__) || (defined(__UP_PKG_ON_NAND__) && defined(NAND_SUPPORT))
    if (Acc_Type == FAT_ACC) {
        uint8_t  i;
        for (i = 0; i < Flash_Info.bankNum; i++) {
#if defined(__NANDFDM_TOTAL_BBM__)
            if (Bank[i]&BBM_GLOBAL_ADDR) {
                DAL_Bank_Array[i] = Bank[i];
            } else
#endif //#if defined(__NANDFDM_TOTAL_BBM__)
                if (ZoneNo == 0) {
#if !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset;
#else // !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset[CurDiskIndex];
#endif //!defined(__NANDFDM_MULTI_INSTANCE__)
                } else {
                    DAL_Bank_Array[i] = Bank[i];
                }
        }
        Bank = DAL_Bank_Array;
#if !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum;
#else // !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum[CurDiskIndex];
#endif //!defined(__NANDFDM_MULTI_INSTANCE__)
    }
#else
    if (Acc_Type != FAT_ACC) {
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }
#endif /* _NAND_FLASH_BOOTING || defined(__NANDFDM_MULTI_INSTANCE__) || (__UP_PKG_ON_NAND__ && NAND_SUPPORT)) */
    /*boundary check*/
    if ( ZoneNo >= Flash_Info.zoneNum || Block_offset >= Flash_Info.bankNum ) {
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }

    if (Acc_Type == RAW_ACC) {
        ZoneLen = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
    }
    kal_mem_set(&Mtd_para, 0x00, sizeof(Mtd_para));
    Add_Cnvt.zoneNum = ZoneNo ;
    Add_Cnvt.bankArry = Bank;
    Add_Cnvt.pageIdx = Block_offset; /*first page of block*/
    addressMap(&Add_Cnvt);
    if (Flash_Info.deviceInfo_CE[Add_Cnvt.ce].deviceSize * 1024 * 1024 <= Flash_Info.pageSize * Add_Cnvt.phyPage) {
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }
    if (EraseBlock == TRUE) {
        ce_idx = Add_Cnvt.ce;
        for (m_idx = 0; m_idx < MAX_MULTIPAGE_FUNCTION_NUM; m_idx++) {
            if ( 1 == Flash_Info.multiErase[ce_idx][m_idx].multiPageNum
                    && Flash_Info.multiErase[ce_idx][m_idx].func_p != NULL) {
                break;
            }
        }
        ASSERT_RET(m_idx != MAX_MULTIPAGE_FUNCTION_NUM, MTD_STATUS_ERROR);
        /* Fill Mtd_para*/
        Mtd_para.buff = NULL;
        Mtd_para.spare = NULL;
        Mtd_para.m_idx = m_idx;
        Mtd_para.length = 1;
        if (Acc_Type == RAW_ACC) {
            Mtd_para.pageIdx[0] = ((ZoneNo * ZoneLen) + (Bank[0] * Flash_Info.bankNum) + Block_offset) * Flash_Info.blockPage;
            Mtd_para.ce = 0;
        } else {
            Mtd_para.pageIdx[0] = Add_Cnvt.phyPage;
            Mtd_para.ce = Add_Cnvt.ce;
        }
        MTD_Erase(&Mtd_para);
    }
    /*Read the target page back*/
    /* Fill Mtd_para*/
    kal_mem_set(&Mtd_para, 0x00, sizeof(Mtd_para));
    Mtd_para.length = 1; /*single read only*/
    Mtd_para.m_idx = 0; /*No use*/
    if (Acc_Type == RAW_ACC) {
        Mtd_para.pageIdx[0] = ((ZoneNo * ZoneLen) + (Bank[0] * Flash_Info.bankNum) + Block_offset) * Flash_Info.blockPage;
        Mtd_para.ce = 0;
    } else {
        Mtd_para.pageIdx[0] = Add_Cnvt.phyPage;
        Mtd_para.ce = Add_Cnvt.ce;
    }
    Mtd_para.buff = (uint8_t *)&Data_buff[0];
    Mtd_para.spare = (uint8_t *)&Spare_buff[0];
    MTD_Read(&Mtd_para);

    //kal_mem_set(Data_buff, 0xFF, sizeof(Flash_Info.pageSize));
    //kal_mem_set(Spare_buff, 0xFF, sizeof(Flash_Info.spareSize));

    if (Flash_Info.pageSize == 512) {
        ((FDMData_512 *)Spare_buff) -> BI &= FDM_BAD_BLOCK_MARK_512; /*use &= to adapte the original mark, ex: 0x00(early bad)*/
    } else {
        ((FDMData_2k *)Spare_buff) -> BI &= FDM_BAD_BLOCK_MARK_2K; /*use &= to adapte the original mark, ex: 0x00(early bad)*/
    }

    /* Fill Mtd_para*/
    Mtd_para.buff = (uint8_t *)Data_buff;
    Mtd_para.spare = (uint8_t *) Spare_buff;
    Mtd_para.m_idx = 0; /*No use*/
    Mtd_para.length = 1; /*No use*/
    if (Acc_Type == RAW_ACC) {
        Mtd_para.pageIdx[0] = ((ZoneNo * ZoneLen) + (Bank[0] * Flash_Info.bankNum) + Block_offset) * Flash_Info.blockPage;
        Mtd_para.ce = 0;
    } else {
        Mtd_para.pageIdx[0] = Add_Cnvt.phyPage;
        Mtd_para.ce = Add_Cnvt.ce;
    }
    MTD_MarkBlockBad(&Mtd_para);

    free_MTD_lock();
    return MTD_STATUS_NO_ERROR;
}

/****************************************************************************
 * Function:
 *
 * Parameters:   uint8_t newFlash This flag make condition to check: if BI != 0xFF_FF, it's early bad
 *
 * Returns:      MTD_STATUS_NO_ERROR - The block is a good block
 *               MTD_STATUS_EARLY_BAD - The block is a bad block of early bad
 *               MTD_STATUS_FDM_BAD - The block is a bad block of FDM bad
 *               (MTD_STATUS_EARLY_BAD|MTD_STATUS_FDM_BAD)
 * Description:
 *
 ****************************************************************************/
uint32_t DAL_CheckBadBlock (DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint8_t Block_offset, uint8_t newFlash, uint32_t *Data_buff, uint32_t *Spare_buff)
{
    uint32_t mtd_retVal, retVal = MTD_STATUS_NO_ERROR;
    uint8_t FirstCheck = TRUE;

    get_MTD_lock();
#if defined (_NAND_FLASH_BOOTING_) || defined(__NANDFDM_MULTI_INSTANCE__) || (defined(__UP_PKG_ON_NAND__) && defined(NAND_SUPPORT))
    if (Acc_Type == FAT_ACC) {
        uint8_t i;
        for (i = 0; i < Flash_Info.bankNum; i++) {
#if defined(__NANDFDM_TOTAL_BBM__)
            if (Bank[i]&BBM_GLOBAL_ADDR) {
                DAL_Bank_Array[i] = Bank[i];
            } else
#endif //#if defined(__NANDFDM_TOTAL_BBM__)
                if (ZoneNo == 0) {
#if !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset;
#else  // !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset[CurDiskIndex];
#endif  // !defined(__NANDFDM_MULTI_INSTANCE__)
                } else {
                    DAL_Bank_Array[i] = Bank[i];
                }
        }
        Bank = DAL_Bank_Array;
#if !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum;
#else  // !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum[CurDiskIndex];
#endif  // !defined(__NANDFDM_MULTI_INSTANCE__)
    }
#else
    if (Acc_Type != FAT_ACC) {
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }
#endif /* _NAND_FLASH_BOOTING_ || defined(__NANDFDM_MULTI_INSTANCE__) || (__UP_PKG_ON_NAND__ && NAND_SUPPORT)) */
    /*boundary check*/
    if ( ZoneNo >= Flash_Info.zoneNum || Block_offset >= Flash_Info.bankNum ) { 
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }

    kal_mem_set(&Mtd_para, 0x00, sizeof(Mtd_para));
    Add_Cnvt.zoneNum = ZoneNo ;
    Add_Cnvt.bankArry = Bank;
    Add_Cnvt.pageIdx = Block_offset; /*first page of block*/
    addressMap(&Add_Cnvt);
    if (Flash_Info.deviceInfo_CE[Add_Cnvt.ce].deviceSize * 1024 * 1024 <= Flash_Info.pageSize * Add_Cnvt.phyPage) {
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }
    /* Fill Mtd_para*/
    Mtd_para.buff = (uint8_t *)Data_buff;
    Mtd_para.spare = (uint8_t *)Spare_buff;
    Mtd_para.m_idx = 0; /*No use*/
    Mtd_para.length = 1; /*No use*/
    if (Acc_Type == RAW_ACC) {
        uint32_t ZoneLen;
        ZoneLen = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
        Mtd_para.pageIdx[0] = ((ZoneNo * ZoneLen) + (Bank[0] * Flash_Info.bankNum) + Block_offset) * Flash_Info.blockPage;
        Mtd_para.ce = 0;
    } else {
        Mtd_para.pageIdx[0] = Add_Cnvt.phyPage; /*2k page need to check 2nd page in newFlash case*/
        Mtd_para.ce = Add_Cnvt.ce;
    }

MTD_BB_CHECK:

    mtd_retVal = MTD_CheckBlockBad(&Mtd_para);

    if (mtd_retVal != MTD_STATUS_NO_ERROR) { /*no early bad mark in MTD, check the FDM mark whether be OK*/
        retVal |= MTD_STATUS_EARLY_BAD;
    }

    if (Flash_Info.pageSize == 512) {
        if ( ( ZEROBIT [((FDMData_512 *)Spare_buff)->BI ] >= 2 ) &&
                ( ZEROBIT [((FDMData_512 *)Spare_buff)->BI ] <= (ZERO_OF_FDM_BAD_BLOCK_MARK_512 + 1) ) ) {
            retVal |= MTD_STATUS_FDM_BAD;
        }
    } else {
        uint8_t zeroCount;
        zeroCount = ZEROBIT [((FDMData_2k *)Spare_buff)->BI & 0xFF ];
        if (( zeroCount >= 2 ) &&
                ( zeroCount <= (ZERO_OF_FDM_BAD_BLOCK_MARK_512 + 1 ))) {
            retVal |= MTD_STATUS_FDM_BAD;
        }


///TODO::: delete  is ok?
#if 0
        if (Flash_Info.deviceInfo_CE[0].IOWidth == 16) {
            zeroCount = ZEROBIT [(((FDMData_2k *)Spare_buff)->BI & 0xFF00) >> 8 ] ;
            if ( ( zeroCount >= 2 ) &&
                    ( zeroCount <= (ZERO_OF_FDM_BAD_BLOCK_MARK_512 + 1) ) ) {
                retVal |= MTD_STATUS_FDM_BAD;
            }
        }
#endif
    }

    if ( FirstCheck == TRUE) {
        FirstCheck = FALSE;

        if ( newFlash == TRUE) {
            if (Flash_Info.pageSize == 512) {
#if defined(__NFI_VERSION3_1__)
                if ( ((uint8_t *)Spare_buff)[5]  != 0xFF )
#else // defined(__NFI_VERSION3_1__)
                if ( ((FDMData_512 *)Spare_buff)->BI != 0xFF )
#endif // defined(__NFI_VERSION3_1__)
                {
                    retVal |= MTD_STATUS_EARLY_BAD;
                }

                if ( (retVal & MTD_STATUS_EARLY_BAD) != MTD_STATUS_EARLY_BAD ) {
                    Mtd_para.pageIdx[0] ++; /*second page*/
                    MTD_CheckBlockBad(&Mtd_para);
#if defined(__NFI_VERSION3_1__)
                    if ( ((uint8_t *)Spare_buff)[5]  != 0xFF )
#else // defined(__NFI_VERSION3_1__)
                    if ( ((FDMData_512 *)Spare_buff)->BI != 0xFF )
#endif // defined(__NFI_VERSION3_1__)
                    {
                        retVal |= MTD_STATUS_EARLY_BAD;
                    }
                }
            } else {
                // Offset 0 and offset 2048.
                if ( Mtd_para.buff[0] != 0xff ) {
                    retVal |= MTD_STATUS_EARLY_BAD;
                }
                if ( (Flash_Info.deviceInfo_CE[Mtd_para.ce].IOWidth == 16) && (Mtd_para.buff[1] != 0xff) ) {
                    retVal |= MTD_STATUS_EARLY_BAD;
                }

#if defined(__NFI_VERSION3_1__)
                if (Flash_Info.pageSize == 2048) {
                    if ( (((uint8_t *)Data_buff)[2048 - 48] != 0xff) || (((uint8_t *)Data_buff)[2048 - 48 + 1] != 0xff) ) {
                        retVal |= MTD_STATUS_EARLY_BAD;
                    }
                } else {
                    if ( (((uint8_t *)Data_buff)[4096 - (16 * 7)] != 0xff) || (((uint8_t *)Data_buff)[4096 - (16 * 7) + 1] != 0xff) ) {
                        retVal |= MTD_STATUS_EARLY_BAD;
                    }
                }
#else // defined(__NFI_VERSION3_1__)
                if ( ((FDMData_2k *)Spare_buff)->BI != 0xFFFF ) {
                    retVal |= MTD_STATUS_EARLY_BAD;
                }
#endif // defined(__NFI_VERSION3_1__)
                /*for 2k page, the bad mark is possible to put at 2nd page*/
                if ( (retVal & MTD_STATUS_EARLY_BAD) != MTD_STATUS_EARLY_BAD ) {
                    if (Flash_Info.deviceInfo_CE[Mtd_para.ce].ID_info[0] == POWERFLASH) {
                        // last page
                        Mtd_para.pageIdx[0] += (Flash_Info.blockPage - 1);
                    } else {
                        // second page
                        Mtd_para.pageIdx[0] ++;
                    }
                    MTD_CheckBlockBad(&Mtd_para);
                    if ( Mtd_para.buff[0] != 0xff ) {
                        retVal |= MTD_STATUS_EARLY_BAD;
                    }
                    if ( (Flash_Info.deviceInfo_CE[Mtd_para.ce].IOWidth == 16) && (Mtd_para.buff[1] != 0xff) ) {
                        retVal |= MTD_STATUS_EARLY_BAD;
                    }
#if defined(__NFI_VERSION3_1__)
                    if (Flash_Info.pageSize == 2048) {
                        if ( (((uint8_t *)Data_buff)[2048 - 48] != 0xff) || (((uint8_t *)Data_buff)[2048 - 48 + 1] != 0xff) ) {
                            retVal |= MTD_STATUS_EARLY_BAD;
                        }
                    } else {
                        if ( (((uint8_t *)Data_buff)[4096 - (16 * 7)] != 0xff) || (((uint8_t *)Data_buff)[4096 - (16 * 7) + 1] != 0xff) ) {
                            retVal |= MTD_STATUS_EARLY_BAD;
                        }
                    }
#else // defined(__NFI_VERSION3_1__)
                    if ( ((FDMData_2k *)Spare_buff)->BI != 0xFFFF ) {
                        retVal |= MTD_STATUS_EARLY_BAD;
                    }
#endif // defined(__NFI_VERSION3_1__)
                }
            }

#if !defined(__NFI_VERSION3_1__)
// For code image bad block check, no new flash case.
            if (Acc_Type == CODE_ACC) {
                goto MTD_BB_CHECK;
            }
#endif //#if !(defined(__NFI_VERSION3_1__))
        } else {
            // No new flash
            if (Acc_Type == CODE_ACC) {
                if (Flash_Info.deviceInfo_CE[Mtd_para.ce].ID_info[0] == POWERFLASH) {
                    // last page
                    Mtd_para.pageIdx[0] += (Flash_Info.blockPage - 1);
                } else {
                    // second page
                    Mtd_para.pageIdx[0] ++;
                }
                goto MTD_BB_CHECK;
            }
        }
    }

    free_MTD_lock();
    return retVal;
}

// Description
//	This function is only used for FOTA for compatility.
uint8_t IsGoodBlock(void *D, void *Spare)
{
    uint8_t  good_block = 1;

    if (Flash_Info.pageSize == 512) {
        if ( ZEROBIT [((FDMData_512 *)Spare)->BI ] >= 2 ) {
            good_block = 0;
        }
    } else {
        uint8_t zeroCount;
        zeroCount = ZEROBIT [((FDMData_2k *)Spare)->BI & 0xFF ];
        if ( zeroCount >= 2 ) {
            good_block = 0;
        }

        if (Flash_Info.deviceInfo_CE[0].IOWidth == 16) {
            zeroCount = ZEROBIT [(((FDMData_2k *)Spare)->BI & 0xFF00) >> 8 ] ;
            if ( zeroCount >= 2 ) {
                good_block = 0;
            }
        }
    }

    return good_block;
}

/****************************************************************************
 * Function:   DAL_NAND_Reset
 *
 * Parameters: void
 *
 * Returns:    MTD_STATUS_NO_ERROR
 *
 * Description:
 *
 ****************************************************************************/
uint32_t DAL_NAND_Reset(void)
{
    get_MTD_lock();
    if ( Flash_Info.valid_CE[0] == TRUE ) {
        Mtd_para.ce = 0;
        MTD_NAND_Reset(&Mtd_para);
    }
    if ( Flash_Info.valid_CE[1] == TRUE ) {
        Mtd_para.ce = 1;
        MTD_NAND_Reset(&Mtd_para);
    }
    free_MTD_lock();
    return MTD_STATUS_NO_ERROR;
}

/****************************************************************************
 * Function:
 *
 * Parameters:
 *
 * Returns:
 *
 * Description:
 *
 ****************************************************************************/
uint16_t DAL_GetFDMData(uint8_t FDMType, uint32_t *Spare_buff)
{
    uint16_t Data = 0;

    if (Flash_Info.pageSize == 512) {
        switch (FDMType) {
            case FDMDATA_BM:
            case FDMDATA_PA:
                Data = ((FDMData_512 *)Spare_buff) -> BM_PA;
                break;
            case FDMDATA_LBA:
                Data = ((FDMData_512 *)Spare_buff) -> LBA;
                break;
            case FDMDATA_BI:
                Data = ((FDMData_512 *)Spare_buff) -> BI;
                break;
            case FDMDATA_4XCTL:
                Data = ((FDMData_512 *)Spare_buff) -> PBA;
                break;
            default:
                ASSERT_RET(0, (uint16_t)(MTD_STATUS_ERROR));
        }
    } else {
        switch (FDMType) {
            case FDMDATA_BM:
            case FDMDATA_PA:
            case FDMDATA_4XCTL:
                Data = ((FDMData_2k *)Spare_buff) -> BM_PA;
                break;
            case FDMDATA_LBA:
                Data = ((FDMData_2k *)Spare_buff) -> LBA;
                break;
            case FDMDATA_BI:
                Data = ((FDMData_2k *)Spare_buff) -> BI;
                break;
            default:
                ASSERT_RET(0, (uint16_t)(MTD_STATUS_ERROR));
        }
    }
    return Data;
}

/****************************************************************************
 * Function:
 *
 * Parameters:
 *
 * Returns:
 *
 * Description:
 *
 ****************************************************************************/
void DAL_SetFDMData(uint16_t BMPA, uint16_t LBA, uint8_t Option, uint32_t *Spare_buff)
{
    if (Flash_Info.pageSize == 512) {
        if (Option & FDMDATA_EMPTY) {
            kal_mem_set(Spare_buff, 0xFF, sizeof(FDMData_512) );
        }

        if (Option & (FDMDATA_BM | FDMDATA_PA)) {
            ((FDMData_512 *)Spare_buff) -> BM_PA = BMPA;
        }

        if (Option & FDMDATA_LBA) {
            ((FDMData_512 *)Spare_buff) -> LBA = LBA;
        }
    } else {
        if (Option & FDMDATA_EMPTY) {
            kal_mem_set(Spare_buff, 0xFF, sizeof(FDMData_2k) );
        }

        if (Option & (FDMDATA_BM | FDMDATA_PA)) {
            ((FDMData_2k *)Spare_buff) -> BM_PA = BMPA;
        }

        if (Option & FDMDATA_LBA) {
            ((FDMData_2k *)Spare_buff) -> LBA = LBA;
        }
    }
}

/****************************************************************************
 * Function:     DAL_GetDALVersion
 *
 * Parameters:
 *
 * Returns:      DAL_VERISON
 *
 * Description:  Return the DAL version of this load
 *
 ****************************************************************************/
uint8_t DAL_GetDALVersion(void)
{
    //return DAL_MINOR_VER2;
    ///TODO::: why not build error
    return 1;
}

/****************************************************************************
 * Function:     DAL_GetDALVersion
 *
 * Parameters:   dal_ver    The DAL version that record at File system
 *
 * Returns:      TRUE   Format it
 *               FALSE  Do not format it
 *
 * Description:  FDM uses this fucntion to query DAL if format trigger or not
 *
 ****************************************************************************/
uint8_t DAL_FormatQuery(uint8_t dal_ver)
{
    //return (dal_ver != (DAL_VERISON)) ? TRUE : FALSE ;
    ///TODO::: why not build error
    return 1;
}

uint8_t NANDsanitycheck_Factory(void)
{
    if ( DAL_is_initialized == 0 ) {
        FLASH_INFO F_info;
        DAL_init();
        DAL_FDM_Flash_Info_Init (&F_info, 0);
    }
    return (uint8_t)Flash_Info.valid_CE[0];
}

void DAL_GetDeviceInfo (uint8_t cs, flash_list *p)
{
    if (Flash_Info.valid_CE[cs] == TRUE) {
        kal_mem_cpy(p, &Flash_Info.deviceInfo_CE[cs], sizeof(flash_list));
    } else {
        kal_mem_set(p, 0xFF, sizeof(flash_list) ); /*Reset value*/
    }
}
/****************************************************************************
 * Function:
 *
 * Parameters:  Block       Logic block in code view (input)
 *              Page        The page index in the Block (input)
 *              cs_p        output of chip select
 *              page_idx_p  output of physical page index
 * Returns:     MTD_STATUS_NO_ERROR
 *
 * Description:
 *
 ****************************************************************************/
#if 0
#if defined(FLASH_DISKDRV_DEBUG) || (defined _NAND_FLASH_BOOTING_)
uint32_t DAL_NFB_AddressConvert(uint32_t Block, uint32_t Page, uint8_t *cs_p, uint32_t *page_idx_p)
{
    uint32_t zone_len;
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t k, retVal = MTD_STATUS_NO_ERROR;
    get_MTD_lock();
    zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
    /*calculate the DAL parameter*/
    zone_off = Block / zone_len;
    Block -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (Block / Flash_Info.bankNum);
    }
    Page = Page * Flash_Info.bankNum + Block % Flash_Info.bankNum ;
    Add_Cnvt.zoneNum = zone_off ;
    Add_Cnvt.bankArry = b_arr;
    Add_Cnvt.pageIdx = Page;
    retVal = addressMap(&Add_Cnvt);
    *cs_p = Add_Cnvt.ce;
    *page_idx_p = Add_Cnvt.phyPage;
    if (Flash_Info.deviceInfo_CE[Add_Cnvt.ce].deviceSize * 1024 * 1024 <= Flash_Info.pageSize * Add_Cnvt.phyPage) {
        ASSERT (0);
        free_MTD_lock();
        return MTD_STATUS_PARA_ERROR;
    }
    free_MTD_lock();
    return retVal;
}


#endif
#endif

#if defined(_NAND_FLASH_BOOTING_) || (defined(__UP_PKG_ON_NAND__) && defined(NAND_SUPPORT))

int32_t NFB_ErasePhysicalBlock(uint32_t PhyBlock, uint8_t DALRemap)
{
    //FLASH_INFO F_info;
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t NFBStatus = -1, k;
    uint32_t zone_len, BankNo;

    if ( DAL_is_initialized == 0 ) {
        NFBStatus = DAL_init();
        ASSERT( NFBStatus == FS_NO_ERROR);
    }
    //DAL_FDM_Flash_Info_Init (&F_info, 0);
    if ( PhyBlock == 0xFFFFFFFF) { /*1st access access of NFB*/
        return Flash_Info.deviceInfo_CE[0].IOWidth;
    }

    DAL_NAND_Reset();

    zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);

    /*calculate the DAL parameter*/
    zone_off = PhyBlock / zone_len;
    PhyBlock -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (PhyBlock / Flash_Info.bankNum);
    }
    BankNo = PhyBlock % Flash_Info.bankNum;
    NFBStatus = DAL_CheckBadBlock (DALRemap ? CODE_ACC : RAW_ACC, zone_off, b_arr, BankNo, FALSE, NFB_tmp_D_Buff, NFB_tmp_S_Buff);
    if ( NFBStatus != MTD_STATUS_NO_ERROR ) {
        return ERROR_NFB_BAD_BLOCK;
    }

    NFBStatus = DAL_EraseBlock(DALRemap ? CODE_ACC : RAW_ACC, zone_off, b_arr, BankNo, 1) & 0xffff;

    if (NFBStatus) {
        // Program failure.
        return ERROR_NFB_ERASE;
    }

    return ERROR_NFB_SUCCESS;

}

int32_t NFB_ProgramPhysicalSpare(uint32_t PhyBlock, uint32_t PhyPage, void *Data, uint8_t chksum, uint8_t DALRemap)
{
    //FLASH_INFO F_info;
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t NFBStatus = -1, k;
    uint32_t zone_len;
    /*	uint16_t *word_ptr = NULL;
    	uint8_t  *byte_ptr = NULL;
        uint32_t end_count = 0;
    	uint32_t idx = 0;
    	uint16_t sum_val = 0;
    	uint16_t temp_val = 0;
        uint8_t  *pack_ptr = (uint8_t *)&temp_val;*/

    if ( DAL_is_initialized == 0 ) {
        NFBStatus = DAL_init();
        ASSERT( NFBStatus == FS_NO_ERROR);
    }
    //DAL_FDM_Flash_Info_Init (&F_info, 0);
    if ( PhyBlock == 0xFFFFFFFF && PhyPage == 0xFFFFFFFF ) { /*1st access access of NFB*/
        return Flash_Info.deviceInfo_CE[0].IOWidth;
    }

    DAL_NAND_Reset();

    zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);

    /*calculate the DAL parameter*/
    zone_off = PhyBlock / zone_len;
    PhyBlock -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (PhyBlock / Flash_Info.bankNum);
    }
    PhyPage = PhyPage * Flash_Info.bankNum + PhyBlock % Flash_Info.bankNum ;

    /* check alignment */
    /*	if(((uint32_t)Data) & 0x01)
    	{
    		byte_ptr = (uint8_t *)Data;
    	}
    	else
    	{
    		word_ptr = (uint16_t *)Data;
    	}

    	if (chksum) {
    		if( Flash_Info.pageSize == 2048 )
    		{
    			end_count = 31;
    		}
    		else if ( Flash_Info.pageSize == 512 )
    		{
    			end_count = 7;
    		}
    		else
    		{
    			ASSERT(0);
    		}

    		if(word_ptr)
    		{
    			for(idx = 0 ; idx < end_count ; idx++)
    			{
    				sum_val += word_ptr[idx];
    			}
    			word_ptr[idx] = sum_val;
    		}
    		else
    		{
    			for(idx = 0 ; idx < end_count ; idx++)
    			{
    				pack_ptr[0] = byte_ptr[(idx<<1)];
    				pack_ptr[1] = byte_ptr[1+(idx<<1)];
    				sum_val += temp_val;
    			}
    			temp_val = sum_val;
    			byte_ptr[idx<<1] = pack_ptr[0];
    			byte_ptr[1+(idx<<1)] = pack_ptr[1];
    		}
    	}*/

    kal_mem_set(NFB_tmp_D_Buff, 0xFF, Flash_Info.pageSize);
    NFBStatus = DAL_ProgramPage(DALRemap ? CODE_ACC : RAW_ACC, zone_off, b_arr, PhyPage, 1, (uint8_t *)NFB_tmp_D_Buff, (uint8_t *)Data) & 0xffff;

    if (NFBStatus) {
        // Program failure.
        return ERROR_NFB_PROGRAM;
    }

    return ERROR_NFB_SUCCESS;

}

int32_t NFB_ProgramPhysicalPage(uint32_t PhyBlock, uint32_t PhyPage, void *Data, uint8_t DALRemap)
{
    //FLASH_INFO F_info;
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t NFBStatus = -1, k;
    uint32_t zone_len;

    if ( DAL_is_initialized == 0 ) {
        NFBStatus = DAL_init();
        ASSERT( NFBStatus == FS_NO_ERROR);
    }
    //DAL_FDM_Flash_Info_Init (&F_info, 0);
    if ( PhyBlock == 0xFFFFFFFF && PhyPage == 0xFFFFFFFF ) { /*1st access access of NFB*/
        return Flash_Info.deviceInfo_CE[0].IOWidth;
    }

    DAL_NAND_Reset();

    zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);

    /*calculate the DAL parameter*/
    zone_off = PhyBlock / zone_len;
    PhyBlock -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (PhyBlock / Flash_Info.bankNum);
    }
    PhyPage = PhyPage * Flash_Info.bankNum + PhyBlock % Flash_Info.bankNum ;

    NFBStatus = DAL_CheckBadBlock (DALRemap ? CODE_ACC : RAW_ACC, zone_off, b_arr, PhyBlock % Flash_Info.bankNum, FALSE, NFB_tmp_D_Buff, NFB_tmp_S_Buff);
    if ( NFBStatus != MTD_STATUS_NO_ERROR ) {
        return ERROR_NFB_BAD_BLOCK;
    }

    kal_mem_set(NFB_tmp_S_Buff, 0xFF, 64);
    NFBStatus = DAL_ProgramPage(DALRemap ? CODE_ACC : RAW_ACC, zone_off, b_arr, PhyPage, 1, Data, (uint8_t *)NFB_tmp_S_Buff) & 0xffff;

    if (NFBStatus) {
        // Program failure.
        return ERROR_NFB_PROGRAM;
    }

    return Flash_Info.pageSize;
}

int32_t	NFB_MarkBadBlock(uint32_t PhyBlock, uint8_t DALRemap)
{
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t NFBStatus = -1, k;
    uint32_t zone_len;

    if ( DAL_is_initialized == 0 ) {
        NFBStatus = DAL_init();
        ASSERT( NFBStatus == FS_NO_ERROR);
    }

    DAL_NAND_Reset();

    zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);

    /*calculate the DAL parameter*/
    zone_off = PhyBlock / zone_len;
    PhyBlock -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (PhyBlock / Flash_Info.bankNum);
    }

    NFBStatus = DAL_MarkBlockBad(DALRemap ? CODE_ACC : RAW_ACC, zone_off, b_arr, PhyBlock % Flash_Info.bankNum, TRUE, NFB_tmp_D_Buff, NFB_tmp_S_Buff);

    if (NFBStatus) {
        return ERROR_NFB_PROGRAM;
    }

    return ERROR_NFB_SUCCESS;

}

int32_t	NFB_ProgramPhysicalPageWithSpare(uint32_t PhyBlock, uint32_t PhyPage, void *Data, void *Spare, uint8_t DALRemap)
{
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t NFBStatus = -1, k;
    uint32_t zone_len;

    if ( DAL_is_initialized == 0 ) {
        NFBStatus = DAL_init();
        ASSERT(NFBStatus == FS_NO_ERROR);
    }

    DAL_NAND_Reset();

    zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);

    /*calculate the DAL parameter*/
    zone_off = PhyBlock / zone_len;
    PhyBlock -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (PhyBlock / Flash_Info.bankNum);
    }
    PhyPage = PhyPage * Flash_Info.bankNum + PhyBlock % Flash_Info.bankNum ;

    NFBStatus = DAL_CheckBadBlock (DALRemap ? CODE_ACC : RAW_ACC, zone_off, b_arr, PhyBlock % Flash_Info.bankNum, FALSE, NFB_tmp_D_Buff, NFB_tmp_S_Buff);
    if ( NFBStatus != MTD_STATUS_NO_ERROR ) {
        return ERROR_NFB_BAD_BLOCK;
    }

    NFBStatus = DAL_ProgramPage(DALRemap ? CODE_ACC : RAW_ACC, zone_off, b_arr, PhyPage, 1, (uint8_t *)Data, (uint8_t *)Spare) & 0xffff;

    if (NFBStatus) {
        // Program failure.
        return ERROR_NFB_PROGRAM;
    }

    return Flash_Info.pageSize;

}

uint32_t NFB_ReadPhysicalPage(uint32_t PhyBlock, uint32_t PhyPage, void *Data)
{
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t NFBStatus = -1, k;
    static uint32_t zone_len = DWORD_INVALID;
    static uint32_t blockStatus[3] = {DWORD_INVALID, DWORD_INVALID, DWORD_INVALID}; /*record block status*/
#if ( defined(__EMULATE_NFB_BIT_ERROE__) )
    FLASH_INFO F_info;
    uint8_t CompareError = FALSE;
#endif // ( defined(__EMULATE_NFB_BIT_ERROE__) )

    if ( DAL_is_initialized == 0 ) {
        NFBStatus = DAL_init();
        ASSERT(NFBStatus == FS_NO_ERROR);
    }
    //DAL_FDM_Flash_Info_Init (&F_info, 0);   // 2010.1.11 mtk02493 remark for useless
    if ( PhyBlock == 0xFFFFFFFF && PhyPage == 0xFFFFFFFF ) { /*1st access access of NFB*/
        return Flash_Info.deviceInfo_CE[0].IOWidth;
    }

    DAL_NAND_Reset();

    if (zone_len == DWORD_INVALID) {
        zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
    }
    /*calculate the DAL parameter*/
    zone_off = PhyBlock / zone_len;
    PhyBlock -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (PhyBlock / Flash_Info.bankNum);
    }
    PhyPage = PhyPage * Flash_Info.bankNum + PhyBlock % Flash_Info.bankNum ;
    get_MTD_lock(); /*protect blockStatus*/
    if ( (blockStatus[0] == zone_off) &&
            (blockStatus[1] == PhyBlock) ) {
        free_MTD_lock();
    } else {
        free_MTD_lock();
        NFBStatus = DAL_CheckBadBlock (CODE_ACC, zone_off, b_arr, PhyBlock % Flash_Info.bankNum, FALSE, NFB_tmp_D_Buff, NFB_tmp_S_Buff);
        get_MTD_lock();
        blockStatus[0] = zone_off;
        blockStatus[1] = PhyBlock;
        blockStatus[2] = NFBStatus;
        free_MTD_lock();
    }
    if ( blockStatus[2] != MTD_STATUS_NO_ERROR ) {
        return ERROR_NFB_BAD_BLOCK;
    }


#if ( defined(__EMULATE_NFB_BIT_ERROE__) )
    DAL_FDM_Flash_Info_Init (&F_info, 0);   // 2010.1.11 mtk02493 move to here
    EmulateBitError(CODE_ACC, zone_off, b_arr, PhyPage, F_info.SectorNumPerPage, D_Buff, S_Buff);

#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) )
#if defined(__UBL__)
    dbg_print("NFB download into address %xh\n\r", Data);
#else // defined(__UBL__)
    LOG_I(common, "NFB download into address %xh\r\n", Data);

#endif // !defined(__UBL__)
#endif // ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) )

#endif // ( defined(__EMULATE_NFB_BIT_ERROE__) )

    NFBStatus = DAL_ReadPage (CODE_ACC, zone_off, b_arr, PhyPage, 1, Data, (uint8_t *)NULL) & 0xffff0000; /*null spare buff point mean do not care spare data*/

#if ( defined(__EMULATE_NFB_BIT_ERROE__) )

    // The following needs to be tested only when secondary ROM is un-cachable.
    // Otherwise, kal_mem_cmp will corrupt the last page if the starting address is not cache line alignment.
    // That is because MTD use DMA move but kal_mem_cmp use CPU move and will have cache have the data of next oage.
    if ( kal_mem_cmp(Data, &D_Buff[_DAL_MAX_PAGE_SIZE_ / 4], F_info.SectorNumPerPage * 0x200 - 32 ) )  {
        uint32_t Offset;
        for (Offset = 0; Offset < Flash_Info.pageSize / 4; Offset++) {
            if ( ((uint32_t *)Data)[Offset] != D_Buff[(_DAL_MAX_PAGE_SIZE_ / 4) + Offset] ) {

#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) )

#if defined(__UBL__)
                dbg_print("NFB compare error in ECC test. Address %x. Wrong data=%x, Right data=%x\n\r", &(((uint32_t *)Data)[Offset]),
                          ((uint32_t *)Data)[Offset], D_Buff[(_DAL_MAX_PAGE_SIZE_ / 4) + Offset]);
#else // defined(__UBL__)
                LOG_I(common, "NFB compare error in ECC test. Address %x. Wrong data=%x, Right data=%x\r\n", &(((uint32_t *)Data)[Offset]),
                      ((uint32_t *)Data)[Offset], D_Buff[(_DAL_MAX_PAGE_SIZE_ / 4) + Offset]);

#endif // defined(__UBL__)			    

#endif // ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) )

                CompareError = TRUE;
            }
        }
    }


    if (CompareError == TRUE) {

#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) )

#if defined(__UBL__)
        dbg_print("NFB compare error in ECC test.\n\r");
#else // defined(__UBL__)
        LOG_I(common, "NFB compare error in ECC test.\r\n");
#endif // defined(__UBL__)

#endif // ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) )

        ASSERT(0);
    }

#endif // ( defined(__EMULATE_NFB_BIT_ERROE__) )


#if defined(__FUE__)
    if ( NFBStatus == MTD_STATUS_ERROR ) {
        return ERROR_NFB_READ;
    } else if ( NFBStatus != MTD_STATUS_NO_ERROR ) {
        if (NFBStatus == MTD_STATUS_CORRECTED) {
            return ERROR_NFB_ECC_CORRECTED;
        }
    }
#else // defined(__FUE__)
    if ( NFBStatus == MTD_STATUS_ERROR ) {
        return ERROR_NFB_READ;
    }
#endif // defined(__FUE__)

    return Flash_Info.pageSize;
}

int32_t NFB_ReadPhysicalSpare(uint32_t PhyBlock, uint32_t PhyPage, void *Data, uint8_t chksum)
{
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t NFBStatus = -1, k;
    uint32_t zone_len = DWORD_INVALID;
    /*	uint8_t  *byte_ptr = NULL;
    	uint16_t *word_ptr = NULL;
    	uint8_t  *pack_byte = NULL;
    	uint16_t temp_val = 0;
    	uint16_t sum_val = 0;
    	uint32_t spare_size = 0;
    	uint32_t i = 0;*/


    if ( DAL_is_initialized == 0 ) {
        NFBStatus = DAL_init();
        ASSERT(NFBStatus == FS_NO_ERROR);
    }
    if ( PhyBlock == 0xFFFFFFFF && PhyPage == 0xFFFFFFFF ) { /*1st access access of NFB*/
        return Flash_Info.deviceInfo_CE[0].IOWidth;
    }

    DAL_NAND_Reset();

    zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);

    /*calculate the DAL parameter*/
    zone_off = PhyBlock / zone_len;
    PhyBlock -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (PhyBlock / Flash_Info.bankNum);
    }
    PhyPage = PhyPage * Flash_Info.bankNum + PhyBlock % Flash_Info.bankNum ;


    NFBStatus = DAL_ReadPage (CODE_ACC, zone_off, b_arr, PhyPage, 1, (uint8_t *)NFB_tmp_D_Buff, (uint8_t *)Data) & 0xffff0000;
    // Edward : May not use NFBStatus to check because this is full page read with ECC correction..
    // In NAND FDM 4.x FOTA, read spare does not return error.

    /*	if( chksum ) {
    		if (Flash_Info.pageSize == 2048) {
    			spare_size = 64;
    		}
    		else if (Flash_Info.pageSize == 512) {
    			spare_size = 16;
    		}

    		if(((uint32_t)Data) & 0x01) // not word aligned
    		{
    			byte_ptr = (uint8_t *)Data;
    			for(i = 0 ; i < ((spare_size >> 1) - 1) ; i++)
    			{
    				pack_byte[0] = byte_ptr[i<<1];
    				pack_byte[1] = byte_ptr[1+(i<<1)];
    				sum_val += temp_val;
    			}
    			pack_byte[0] = byte_ptr[i>>1];
    			pack_byte[1] = byte_ptr[1+(i>>1)];
    			if(sum_val != temp_val)
    			{
    				NFBStatus = ERROR_NFB_CHECKSUM;
    			}
    			else
    			{
    				NFBStatus = ERROR_NFB_SUCCESS;
    			}
    		}
    		else
    		{
    			word_ptr = (uint16_t *)Data;
    			for(i = 0 ; i < ((spare_size >> 1) - 1) ; i++)
    			{
    				sum_val += word_ptr[i];
    			}
    			if(sum_val != word_ptr[i])
    			{
    				NFBStatus = ERROR_NFB_CHECKSUM;
    			}
    			else
    			{
    				NFBStatus = ERROR_NFB_SUCCESS;
    			}
    		}
       }
       else */
    {
        NFBStatus = ERROR_NFB_SUCCESS;
    }

    return NFBStatus;
}

//#if defined(__FOTA_DM__)

uint32_t NFB_ReadPhysicalPageWithSpare(uint32_t PhyBlock, uint32_t PhyPage, uint8_t *Data)
{
    //FLASH_INFO F_info;
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t NFBStatus = -1, k;
    static uint32_t zone_len = DWORD_INVALID;
    static uint32_t blockStatus[3] = {DWORD_INVALID, DWORD_INVALID, DWORD_INVALID}; /*record block status*/

    if ( DAL_is_initialized == 0 ) {
        NFBStatus = DAL_init();
        ASSERT( NFBStatus == FS_NO_ERROR);
    }
    //DAL_FDM_Flash_Info_Init (&F_info, 0);  //2010.01.12 kuohong remark
    if ( PhyBlock == 0xFFFFFFFF && PhyPage == 0xFFFFFFFF ) { /*1st access access of NFB*/
        return Flash_Info.deviceInfo_CE[0].IOWidth;
    }

    DAL_NAND_Reset();

    if (zone_len == DWORD_INVALID) {
        zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
    }
    /*calculate the DAL parameter*/
    zone_off = PhyBlock / zone_len;
    PhyBlock -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (PhyBlock / Flash_Info.bankNum);
    }
    PhyPage = PhyPage * Flash_Info.bankNum + PhyBlock % Flash_Info.bankNum ;
    get_MTD_lock(); /*protect blockStatus*/
    if ( (blockStatus[0] == zone_off) &&
            (blockStatus[1] == PhyBlock) ) {
        free_MTD_lock();
    } else {
        free_MTD_lock();
        NFBStatus = DAL_CheckBadBlock (CODE_ACC, zone_off, b_arr, PhyBlock % Flash_Info.bankNum, FALSE, NFB_tmp_D_Buff, NFB_tmp_S_Buff);
        get_MTD_lock();
        blockStatus[0] = zone_off;
        blockStatus[1] = PhyBlock;
        blockStatus[2] = NFBStatus;
        free_MTD_lock();
    }
    if ( blockStatus[2] != MTD_STATUS_NO_ERROR ) {
        return ERROR_NFB_BAD_BLOCK;
    }

    NFBStatus = DAL_ReadPage (CODE_ACC, zone_off, b_arr, PhyPage, 1, Data, (uint8_t *)&Data[Flash_Info.pageSize]) & 0xffff0000;

    // It will return fail because that flashtool use checksum instead of spare ECC.
    /*    if ( NFBStatus == MTD_STATUS_ERROR ) {
        	return ERROR_NFB_READ;
        } else if ( NFBStatus != MTD_STATUS_NO_ERROR ) {
        	if (NFBStatus == MTD_STATUS_CORRECTED) {
        		return ERROR_NFB_ECC_CORRECTED;
        	}
        }*/

    return Flash_Info.pageSize;
}

//#endif // defined(__FOTA_DM__)


uint32_t NFB_CheckGoodBlock2(uint32_t Block, uint8_t DALRemap)
{
    uint32_t zone_len;
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t k, retVal;
    zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
    /*calculate the DAL parameter*/
    zone_off = Block / zone_len;
    Block -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (Block / Flash_Info.bankNum);
    }
    DAL_NAND_Reset();
    retVal = DAL_CheckBadBlock (DALRemap ? CODE_ACC : RAW_ACC, zone_off, b_arr, Block % Flash_Info.bankNum, FALSE, NFB_tmp_D_Buff, NFB_tmp_S_Buff);
    if (retVal == MTD_STATUS_NO_ERROR) {
        return 0;
    } else {
        return -1;
    }
}

uint32_t NFB_CheckGoodBlock(uint32_t Block)
{
    return demp_CheckGoodBlock(Block);
}

uint32_t demp_CheckGoodBlock(uint32_t Block) /*MAUI_00592819*/
{
    uint32_t zone_len;
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t k, retVal;
    zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
    /*calculate the DAL parameter*/
    zone_off = Block / zone_len;
    Block -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (Block / Flash_Info.bankNum);
    }
    DAL_NAND_Reset();
    retVal = DAL_CheckBadBlock (CODE_ACC, zone_off, b_arr, Block % Flash_Info.bankNum, FALSE, NFB_tmp_D_Buff, NFB_tmp_S_Buff);
    if (retVal == MTD_STATUS_NO_ERROR) {
        return 0;
    } else {
        return -1;
    }
}
uint32_t demp_ReadPhysicalPage(uint32_t PhyBlock, uint32_t PhyPage, void *Data)  /*MAUI_00592819*/
{
    uint8_t zone_off;
    uint16_t b_arr[MAX_MULTIPAGE_NUM];
    uint32_t NFBStatus = -1, k;
    static uint32_t zone_len = DWORD_INVALID;
    uint8_t old_use_interrupt;
#if !defined(__FUE__) && !defined(__UBL__) && defined(__NFB_ON_DEMAND_PAGE_IN__)
    extern uint32_t kal_get_mytask_priority(void);
    extern NAND_MTD_LOCK nfi_mtd_lock;
    uint32_t task_index, prio;
#endif /* !__FUE__ && !__UBL__ */

    /* Confirmed by CC, demp_ReadPhysicalPage will 100% be called after NFB_ReadPhysicalPage, so dont care DAL init stage*/
    if (zone_len == DWORD_INVALID) {
        zone_len = (Flash_Info.CE_interleave_Flag == FALSE) ? zonePage(0) : zonePage(0) + zonePage(1);
    }
    /*calculate the DAL parameter*/
    zone_off = PhyBlock / zone_len;
    PhyBlock -= zone_off * zone_len;
    for (k = 0; k < Flash_Info.bankNum; k++) {
        b_arr[k] = (PhyBlock / Flash_Info.bankNum);
    }
    PhyPage = PhyPage * Flash_Info.bankNum + PhyBlock % Flash_Info.bankNum ;
    /* Confirmed by CC, demp_ReadPhysicalPage don't need to check bad block because demp_CheckGoodBlock will be called to know bad blocks position*/
#if !defined(__FUE__) && !defined(__UBL__) && defined(__NFB_ON_DEMAND_PAGE_IN__)
    if (nfi_mtd_lock.owner_id != NULL) {
        kal_trace(SYSTRACE_GROUP_DEMP, DEMP_PAGE_LOCK_TAKEN,
                  *(uint32_t *)(((NU_TASK *)(nfi_mtd_lock.owner_id))->tc_argc), nfi_mtd_lock.lock_count);
    } else {
        kal_trace(SYSTRACE_GROUP_DEMP, DEMP_PAGE_LOCK_AVAILABLE);
    }
#endif /* !__FUE__ && !__UBL__ */
    get_MTD_lock();		// Need to use lock to protect use_interrupt.
#if !defined(__FUE__) && !defined(__UBL__) && defined(__NFB_ON_DEMAND_PAGE_IN__)
    kal_get_my_task_index(&task_index);
    prio = kal_get_mytask_priority();
    kal_trace(SYSTRACE_GROUP_DEMP, DEMP_PAGE_GET_LOCK, task_index, prio);
#endif /* !__FUE__ && !__UBL__ */

    DemandPageingRead = TRUE;
    old_use_interrupt = use_interrupt;
    use_interrupt = FALSE;	// Need to set FALSE to avoid context switch. If context switch, FT task in META mode will be swtich out and L1Audio get control and assert.
    NFBStatus = DAL_ReadPage (CODE_ACC, zone_off, b_arr, PhyPage, 1, Data, (uint8_t *)NULL) & 0xffff0000; /*null spare buff point mean do not care spare data*/
    use_interrupt = old_use_interrupt;
    DemandPageingRead = FALSE;

#if !defined(__FUE__) && !defined(__UBL__) && defined(__NFB_ON_DEMAND_PAGE_IN__)
    prio = kal_get_mytask_priority();
    kal_trace(SYSTRACE_GROUP_DEMP, DEMP_PAGE_FREE_LOCK, task_index, prio);
#endif /* !__FUE__ && !__UBL__ */
    free_MTD_lock();

    if ( NFBStatus == MTD_STATUS_ERROR ) {
        return -1;
    } else {
        return Flash_Info.pageSize;
    }
}


#endif /* _NAND_FLASH_BOOTING_ || (__UP_PKG_ON_NAND__ && NAND_SUPPORT) */

#if 0
#if defined(__SECURITY_OTP__)&& defined(NAND_SUPPORT)

uint32_t DAL_OTPAccess(uint32_t accesstype, uint32_t Offset, void *BufferPtr, uint32_t Length)
{

    uint32_t Status, PageAddr, AccessLength = 0;
    uint8_t *BufAddr = (uint8_t *)BufferPtr;

    if (devDriver(0)->OTPAccess == NULL) {
        return FS_FLASH_OTP_UNKNOWERR;
    }

    if (DAL_is_initialized == 0) {
        Status = DAL_init();
        ASSERT(Status == FS_NO_ERROR);
    }

    DAL_NAND_Reset();

    get_MTD_lock();


    while (1) {
        PageAddr = Offset / Flash_Info.pageSize;
        if (accesstype == FS_OTP_READ) {
            Status = devDriver(0)->OTPAccess(FS_OTP_READ, PageAddr, NFB_tmp_D_Buff, NFB_tmp_S_Buff);
            if (Status != FS_NO_ERROR) {
                break;
            }
            AccessLength = Flash_Info.pageSize - (Offset % Flash_Info.pageSize);
            if (Length >= AccessLength) {
                kal_mem_cpy(BufAddr, ((uint8_t *)NFB_tmp_D_Buff) + (Offset % Flash_Info.pageSize), AccessLength);
            } else {
                // Last time.
                kal_mem_cpy(BufAddr, ((uint8_t *)NFB_tmp_D_Buff) + (Offset % Flash_Info.pageSize), Length);
            }
        } else if (accesstype == FS_OTP_WRITE) {
            AccessLength = Flash_Info.pageSize - (Offset % Flash_Info.pageSize);
            kal_mem_set(NFB_tmp_D_Buff, 0xff, Flash_Info.pageSize);
            kal_mem_set(NFB_tmp_S_Buff, 0xff, Flash_Info.spareSize);
            if (Length >= AccessLength) {
                kal_mem_cpy(((uint8_t *)NFB_tmp_D_Buff) + (Offset % Flash_Info.pageSize), BufAddr, AccessLength);
            } else {
                // Last time.
                kal_mem_cpy(((uint8_t *)NFB_tmp_D_Buff) + (Offset % Flash_Info.pageSize), BufAddr, Length);
            }
            Status = devDriver(0)->OTPAccess(FS_OTP_WRITE, PageAddr, NFB_tmp_D_Buff, NFB_tmp_S_Buff);
        } else {
            Status = devDriver(0)->OTPAccess(accesstype, PageAddr, NULL, NULL);
            break;
        }

        Offset += AccessLength;
        BufAddr += AccessLength;
        if (Length <= AccessLength) {
            Length = 0;
            break;
        } else {
            Length -= AccessLength;
        }
    }
    free_MTD_lock();

    return Status;

}

uint32_t DAL_OTPQueryLength(uint32_t *LengthPtr)
{
    uint32_t Status;

    if (devDriver(0)->OTPQueryLength == NULL) {
        return FS_FLASH_OTP_UNKNOWERR;
    }

    if (DAL_is_initialized == 0) {
        Status = DAL_init();
        ASSERT(Status == FS_NO_ERROR);
    }

    DAL_NAND_Reset();

    get_MTD_lock();
    Status = devDriver(0)->OTPQueryLength(LengthPtr);
    free_MTD_lock();

    return Status;
}

#else // __SECURITY_OTP__

uint32_t DAL_OTPAccess(uint32_t accesstype, uint32_t Offset, void *BufferPtr, uint32_t Length)
{
    return FS_UNSUPPORTED_DRIVER_FUNCTION;
}

uint32_t DAL_OTPQueryLength(uint32_t *LengthPtr)
{
    return FS_UNSUPPORTED_DRIVER_FUNCTION;
}

#endif // __SECURITY_OTP__
#endif

uint32_t DAL_GetDeviceIDInfo(void *pInfo)
{
    FS_DeviceInfo *DeviceInfo = (FS_DeviceInfo *)pInfo;
    uint32_t Index;
    for (Index = 0; Index < 8; Index++) {
        DeviceInfo->DeviceInfo.NAND.DeviceID[Index] = Flash_Info.deviceInfo_CE[0].ID_info[Index];
    }

    DeviceInfo->DeviceInfo.NAND.PageSize = Flash_Info.deviceInfo_CE[0].pageSize + Flash_Info.deviceInfo_CE[0].spareSize;
    DeviceInfo->DeviceInfo.NAND.BlockSize = Flash_Info.deviceInfo_CE[0].blockPage * DeviceInfo->DeviceInfo.NAND.PageSize;
    DeviceInfo->DeviceInfo.NAND.DeviceSize = Flash_Info.deviceInfo_CE[0].deviceSize + Flash_Info.deviceInfo_CE[1].deviceSize;
    return FS_NO_ERROR;
}
uint32_t DAL_GetBlockSize(void)
{
    return (Flash_Info.deviceInfo_CE[0].blockPage * Flash_Info.deviceInfo_CE[0].pageSize);
}
uint32_t DAL_GetPageSize(void)
{
    return (Flash_Info.deviceInfo_CE[0].pageSize);
}

uint32_t DAL_LPage2PPage(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint16_t PageAddr)
{
    //uint8_t  m_idx = 0;
    //uint8_t  bk_idx_ce;
    //uint8_t  ce_idx = 0;
    //uint16_t  i;
    uint32_t retVal = MTD_STATUS_NO_ERROR;

    get_MTD_lock();
#if defined (_NAND_FLASH_BOOTING_) || defined(__NANDFDM_MULTI_INSTANCE__) || (defined(__UP_PKG_ON_NAND__) && defined(NAND_SUPPORT))
    if (Acc_Type == FAT_ACC) {
        for (i = 0; i < Flash_Info.bankNum; i++) {
#if defined(__NANDFDM_TOTAL_BBM__)
            if (Bank[i]&BBM_GLOBAL_ADDR) {
                DAL_Bank_Array[i] = Bank[i];
            } else
#endif //#if defined(__NANDFDM_TOTAL_BBM__)
                if (ZoneNo == 0) {
#if !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset;
#else // !defined(__NANDFDM_MULTI_INSTANCE__)
                    DAL_Bank_Array[i] = Bank[i] + FAT_Start_Section_Offset[CurDiskIndex];
#endif // !defined(__NANDFDM_MULTI_INSTANCE__)
                } else {
                    DAL_Bank_Array[i] = Bank[i];
                }
        }
        Bank = DAL_Bank_Array;
#if !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum;
#else  // !defined(__NANDFDM_MULTI_INSTANCE__)
        ZoneNo += FAT_Start_ZoneNum[CurDiskIndex];
#endif	// !defined(__NANDFDM_MULTI_INSTANCE__)
    }
#else
    if (Acc_Type != FAT_ACC) {
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }
#endif /* _NAND_FLASH_BOOTING_ || defined(__NANDFDM_MULTI_INSTANCE__) || (__UP_PKG_ON_NAND__ && NAND_SUPPORT)) */
    /*boundary check*/
    if ( (ZoneNo >= Flash_Info.zoneNum)  ) {
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }

    kal_mem_set(&Mtd_para, 0x00, sizeof(Mtd_para));
    Add_Cnvt.zoneNum = ZoneNo ;
    Add_Cnvt.bankArry = Bank;
    Add_Cnvt.pageIdx = PageAddr;
    addressMap(&Add_Cnvt);
    retVal = Add_Cnvt.phyPage; // Add_Cnvt.ce reserved for future use
    Mtd_para.ce = Add_Cnvt.ce;
    if (Flash_Info.deviceInfo_CE[Add_Cnvt.ce].deviceSize * 1024 * 1024 <= Flash_Info.pageSize * Add_Cnvt.phyPage) {
        free_MTD_lock();
        ASSERT_RET(0, MTD_STATUS_ERROR);
        //return MTD_STATUS_PARA_ERROR;
    }
    free_MTD_lock();
    return retVal;
}
#endif /*( defined(__NAND_FDM_50__) && defined(__NFI_VERSION2__) )*/

#endif
