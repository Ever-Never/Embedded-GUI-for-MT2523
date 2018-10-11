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
#include "hal_flash_opt.h"
#include "hal_flash_combo_init.h"
#include "hal_flash_combo_defs.h"
#include "hal_flash_cfi.h"
#include "hal_flash_cfi_internal.h"
#include "hal_flash_mtd.h"
#if defined(__SERIAL_FLASH__)
#include "hal_flash_sf.h"
#endif


//#include "br_GFH_cmem_id_info.h"
typedef struct {
    GFH_Header            m_gfh_hdr;
    CMEMEntryIDList       m_data;
} GFH_CMEM_ID_INFO_v1;


//-----------------------------------------------------------------------------
// MCP ID Table
//-----------------------------------------------------------------------------
#define  COMBO_MEM_TYPE_MODIFIER  const
#define  COMBO_MEM_INST_NAME    combo_mem_id_list

#define  COMBO_MEM_TYPE_NAME  GFH_CMEM_ID_INFO_v1

#define COMBO_MEM_STRUCT_HEAD  COMBO_MEM_ID_GFH_HEADER, { COMBO_MEM_ID_VER, COMBO_MEM_DEVICE_COUNT, {
#define COMBO_MEM_STRUCT_FOOT  } }

#include "hal_flash_combo_id.h"       // ==== Instantiate ID table

//-----------------------------------------------------------------------------
// External Functions
//-----------------------------------------------------------------------------
extern uint32_t custom_get_fat_addr(void);
extern uint32_t INT_RetrieveFlashBaseAddr(void);

//-----------------------------------------------------------------------------
// Internal Functions
//-----------------------------------------------------------------------------
// ===[EMI/SFI Initialization]===
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
int32_t CMEM_EMIINIT_Index(const uint16_t CS);
#else
int32_t CMEM_EMIINIT_Index(void);
#endif
void CMEM_EMIINIT_ReadID(const uint16_t CS, void *BaseAddr, uint16_t *flashid);

//-----------------------------------------------------------------------------
// Internal Variables
//-----------------------------------------------------------------------------
#define CMEM_INVALID_INDEX -1

#if defined(__COMBO_MEMORY_SUPPORT__)

static signed short int cmem_emiinit_index = CMEM_INVALID_INDEX; // Init stage only

static signed short int cmem_index = CMEM_INVALID_INDEX;

#endif //__COMBO_MEMORY_SUPPORT__


//-----------------------------------------------------------------------------
/*!
  @brief
    Determine whether SF ID is valid.
    Apply for MT6250 MT6260/61 because after command issue(ex: Read ID), Data pins are in floating, may read trasient value instead of 0x00 or 0xFF.
  @retval
    true: the device ID0 is valid.
    false: the device ID0 is not valid.
*/
bool CMEM_CheckValidDeviceID(uint8_t *id)
{
    // Serial Flash
#if defined(__SERIAL_FLASH__)
    int32_t i, j;
    const CMEMEntryID *id_list = NULL;

    id_list = combo_mem_id_list.m_data.List;
    // seach CMEM list for specific Flash ID
    for (i = 0; i < COMBO_MEM_DEVICE_COUNT; i++)	{
        // Check 1: Compare ID
        for (j = 0; j < id_list[i].IDLength; j++) {
            if (id_list[i].ID[j] != id[j]) {
                break;
            }
        }
        // Check 2: Compare RegionInfo
        if (j == id_list[i].IDLength)   {
            // TBD: Compare RegionInfo
            return true;
        }
    }
#endif //defined(__SERIAL_FLASH__)
    return false;
}

//-----------------------------------------------------------------------------
/*!
  @brief
  Seach device in the combo MCP list by Flash ID.
    1. Read NOR flash ID
    2. Lookup ID table and return the index to the found entry.

  @retval
    The index to the found entry.
    -1 : ID not found
  @remars
    If combo MCP was not turned on, the returned index is always 0.
*/
#if defined(__COMBO_MEMORY_SUPPORT__)
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
int32_t CMEM_Index(const uint16_t CS)
#else //ADMUX or SF without Dual SF enable
int32_t CMEM_Index()
#endif
{
    int32_t i, j;
    CMEMFlashID id;
    const CMEMEntryID *id_list = NULL;

#if !defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    if (cmem_index >= 0) {
        return cmem_index;
    }
#endif

// Read Flash ID
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    if (CS != 2) { //Skip Read ID for dummy SF2
        NOR_ReadID(CS, (void *)(INT_RetrieveFlashBaseAddr()), id.NOR);
    }
#else //ADMUX and SF without Dual SF enable
    NOR_ReadID(0, (void *)(SFC_GENERIC_FLASH_BANK_MASK), id.NOR);
#endif //defined(__DUAL_SERIAL_FLASH_SUPPORT__)

    id_list = combo_mem_id_list.m_data.List;

    // seach CMEM list for specific Flash ID
    for (i = 0; i < COMBO_MEM_DEVICE_COUNT; i++)    {
        // Check 1: Compare ID
        for (j = 0; j < id_list[i].IDLength; j++) {
            if (id_list[i].ID[j] != id.NOR[j]) {
                break;
            }
        }
        // Check 2: Compare RegionInfo
        if (j == id_list[i].IDLength)   {
            // TBD: Compare RegionInfo
            cmem_index = i;
            break;  // entry found, break the loop
        }
    }
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    /*Get Dummy SF entry ID*/
    if (CS == 2) {
        // seach CMEM list for specific Flash ID
        for (i = 0; i < COMBO_MEM_DEVICE_COUNT; i++)    {
            // Check 1: Compare ID
            for (j = 0; j < id_list[i].IDLength; j++) {
                if (j == 0)
                    if (id_list[i].ID[j] != 0x00) {
                        break;
                    }
                if (j == 1)
                    if (id_list[i].ID[j] != 0x34) {
                        break;
                    }
                if (j == 2)
                    if (id_list[i].ID[j] != 0x56) {
                        break;
                    }
            }

            if (j == id_list[i].IDLength) {
                // TBD: Compare RegionInfo
                cmem_index = i;
                break;  // entry found, break the loop
            }
        }
    }
#endif

    return cmem_index;
}


#else  // __COMBO_MEMORY_SUPPORT__

#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
int32_t CMEM_Index(const uint16_t CS)
#else //ADMUX flash
int32_t CMEM_Index()
#endif

{
    return 0;
}

#endif // __COMBO_MEMORY_SUPPORT__

const CMEMEntryID *CMEM_GetIDEntry(uint32_t index)
{
    return &combo_mem_id_list.m_data.List[index];
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Read Flash ID
  @param[in] BaseAddr Base address to the Flash
  @param[out] flashid Flash ID
  @remarks
    This function is only allowed in EMI/SFI init stage.
*/
ATTR_TEXT_IN_RAM void CMEM_EMIINIT_ReadID(const uint16_t CS, void *BaseAddr, uint16_t *flashid)
{
    // Serial Flash
    {
        uint8_t cmd, id[SF_FLASH_ID_LENGTH], i;

        cmd = SF_CMD_READ_ID_QPI;
        SFI_Dev_Command_Ext(CS, &cmd, id, 1, SF_FLASH_ID_LENGTH);

        if (id[0] == 0x00 || id[0] == 0xFF || (CMEM_EMIINIT_CheckValidDeviceID(id) == false)) {
            cmd = SF_CMD_READ_ID;
            SFI_Dev_Command_Ext(CS, &cmd, id, 1, SF_FLASH_ID_LENGTH);
        }

        for (i = 0; i < SF_FLASH_ID_LENGTH; i++) {
            flashid[i] = id[i];
        }
    }
    return;
}


//-----------------------------------------------------------------------------
/*!
  @brief
    Determine whether SF ID is valid.
    Apply for MT6250 MT6260/61 because after command issue(ex: Read ID), Data pins are in floating, may read trasient value instead of 0x00 or 0xFF.
  @retval
    true: the device ID0 is valid.
    false: the device ID0 is not valid.
*/
ATTR_TEXT_IN_RAM bool CMEM_EMIINIT_CheckValidDeviceID(uint8_t *id)
{
    // Serial Flash
#if defined(__SERIAL_FLASH__)
    int32_t i, j;
    const CMEMEntryID *id_list = NULL;
    // seach CMEM list for specific Flash ID
    id_list = combo_mem_id_list.m_data.List;

    for (i = 0; i < COMBO_MEM_DEVICE_COUNT; i++)    {
        // Check 1: Compare ID
        for (j = 0; j < id_list[i].IDLength; j++) {
            if (id_list[i].ID[j] != id[j]) {
                break;
            }
        }
        // Check 2: Compare RegionInfo
        if (j == id_list[i].IDLength)   {
            // TBD: Compare RegionInfo
            return true;
        }
    }

#endif //defined(__SERIAL_FLASH__)
    return false;
}



//-----------------------------------------------------------------------------
/*!
  @brief
    Search device ID list
  @retval
    The index to the found device ID.
    -1: device not found
  @remarks
    This function is only allowed in EMI/SFI init stage.
*/
#if defined(__COMBO_MEMORY_SUPPORT__)
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
ATTR_TEXT_IN_RAM int32_t CMEM_EMIINIT_Index(const uint16_t CS)
#else
ATTR_TEXT_IN_RAM int32_t CMEM_EMIINIT_Index()
#endif
{
    int32_t i, j;
    CMEMFlashID id;
    const CMEMEntryID *id_list = NULL;

#if !defined(__DUAL_SERIAL_FLASH_SUPPORT__)// Dual SF will enter this function two times
    if (cmem_emiinit_index >= 0) {
        return cmem_emiinit_index;
    }
#endif
    // Read Flash ID
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    CMEM_EMIINIT_ReadID(CS, (void *)NOR_FLASH_BASE_ADDRESS, id.NOR);
#else //ADMUX, SF without Dual SF enable
    CMEM_EMIINIT_ReadID(0, (void *)NOR_FLASH_BASE_ADDRESS, id.NOR);
#endif

// seach CMEM list for specific Flash ID
    id_list = combo_mem_id_list.m_data.List;

    for (i = 0; i < COMBO_MEM_DEVICE_COUNT; i++)    {
        // Check 1: Compare ID
        for (j = 0; j < id_list[i].IDLength; j++) {
            if (id_list[i].ID[j] != id.NOR[j]) {
                break;
            }
        }
        // Check 2: Compare RegionInfo
        if (j == id_list[i].IDLength)   {
            // TBD: Compare RegionInfo
            cmem_emiinit_index = i;
            break;
        }
    }
    return cmem_emiinit_index;  // entry not found
}

#else  // __COMBO_MEMORY_SUPPORT__
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
ATTR_TEXT_IN_RAM int32_t CMEM_EMIINIT_Index(const uint16_t CS)
#else
ATTR_TEXT_IN_RAM int32_t CMEM_EMIINIT_Index()
#endif
{
    return 0;
}

#endif // __COMBO_MEMORY_SUPPORT__


#endif // HAL_FLASH_MODULE_ENABLED

