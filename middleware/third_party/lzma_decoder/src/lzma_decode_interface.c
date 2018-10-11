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
 
#include <stdio.h>
#include "LZMA_decoder.h"
#include "lzma_decode_interface.h"
#include "hal_flash.h"

#if defined(BL_FOTA_ENABLE) || (MOD_CFG_FOTA_BL_RESERVED)
#include "hal_wdt.h"
#endif

#if defined(__ICCARM__) || defined(__CC_ARM)
#define LZMA_LOG(...)
#else
#ifdef MTK_FOTA_ON_7687
#define LZMA_LOG(...)    printf(__VA_ARGS__)
#else
#if defined(BL_FOTA_ENABLE)
#include "bl_common.h"
#define LZMA_LOG(...)    BL_PRINT(LOG_DEBUG, __VA_ARGS__)
#else
#include "syslog.h"
#define LZMA_LOG(...)    LOG_I(common, __VA_ARGS__)
#endif /* BL_FOTA_ENABLE */
#endif
#endif

#define IN_BUF_SIZE (1 << 12)
#define OUT_BUF_SIZE (1 << 12)
#define FLASH_BLOCK_SIZE (1 << 12)

#define CHIP_MT2523 0x2523
#define VERSION_CTL_BASE 0xA2000000
#define RAM_ADDRESS_BASE 0x08000000

typedef struct
{
    uint32_t HW_VERSION;
    uint32_t FW_VERSION;
    uint32_t HW_CODE;
    uint32_t HW_SUBCODE;
    uint32_t RESERVED0[2];
    uint32_t FAB_CODE;
}VERSION_CTL_T;

#define VERSION_CTL ((VERSION_CTL_T*)VERSION_CTL_BASE)
#define GET_CHIP_ID()  (VERSION_CTL->HW_CODE)

LZMA_ret lzma_decode (
    uint8_t *destination, 
    uint32_t *destination_length, 
    const uint8_t *source, 
    uint32_t *source_length,
    const uint8_t *compression_property, 
    uint32_t compression_property_size, 
    lzma_alloc_t *lzma_alloc)
{
    SRes SRes_ret = SZ_OK;
    LZMA_ret LZMA_ret = LZMA_OK;
    SizeT destination_length_sizeT = (SizeT)*destination_length;
    SizeT source_length_sizeT = (SizeT)*source_length;
    ISzAlloc *g_alloc;
    ELzmaStatus status;

    g_alloc = (ISzAlloc *)lzma_alloc;
    
    
    SRes_ret = LzmaDecode(destination, &destination_length_sizeT, source, &source_length_sizeT, compression_property, (unsigned)compression_property_size, LZMA_FINISH_ANY, &status, g_alloc);
    
    *destination_length = (unsigned int)destination_length_sizeT;
    *source_length = (unsigned int)source_length_sizeT;

    LZMA_ret = SRes_ret;
    return LZMA_ret;
}

/* decode from flash to flash */
static hal_flash_status_t Write2Flash(uint8_t *des, uint8_t *buf, uint32_t length)
{
    uint32_t current_block;
    uint8_t *current_block_start_addr;
    uint8_t *end_addr;
    uint32_t end_block;
    uint32_t block_idx;
    uint32_t erase_address;
    hal_flash_status_t flash_op_ret;

    //LZMA_LOG("LZAM_write, des = %x\r\n", des);
    end_addr = des + length - 1;
    end_block = (uint32_t)end_addr / FLASH_BLOCK_SIZE;

    current_block = (uint32_t)des / FLASH_BLOCK_SIZE;
    current_block_start_addr = (uint8_t *)(current_block * 0x1000);

    if (current_block_start_addr != des)
    {
        current_block++;
    }

#if 0
    LZMA_LOG("LZMA_write: length = %d\r\n", length);
    LZMA_LOG("LZMA_write: current_block = %d\r\n", current_block);
    LZMA_LOG("LZMA_write: end_block = %d\r\n", end_block);
    LZMA_LOG("LZMA_write: current_block_start_addr = %x\r\n", current_block_start_addr);
#endif
    for(block_idx = current_block; block_idx <= end_block; block_idx++)
    {
        //LZMA_LOG("LZMA_erase start\r\n");
        erase_address = block_idx * 0x1000;
        if (GET_CHIP_ID() == CHIP_MT2523 && erase_address > RAM_ADDRESS_BASE)
        {
            //LZMA_LOG("LZMA_erase_1, addr = %x\r\n", erase_address - RAM_ADDRESS_BASE);
            flash_op_ret = hal_flash_erase(erase_address - RAM_ADDRESS_BASE, HAL_FLASH_BLOCK_4K);
        }
        else
        {
            //LZMA_LOG("LZMA_erase_2, addr = %x\r\n", erase_address);
            flash_op_ret = hal_flash_erase(erase_address, HAL_FLASH_BLOCK_4K);
        }
        if (flash_op_ret < 0) 
        {
            //LZMA_LOG("LZAM_erase fail\r\n");
            return HAL_FLASH_STATUS_ERROR_ERASE_FAIL;
        }
    }

    if (GET_CHIP_ID() == CHIP_MT2523 && (int)des > RAM_ADDRESS_BASE)
    {
        //LZMA_LOG("LZMA_write_1, addr = %x\r\n", (uint32_t)des - RAM_ADDRESS_BASE);
        flash_op_ret = hal_flash_write((uint32_t)des - RAM_ADDRESS_BASE, buf, length);
    }
    else
    {
        //LZMA_LOG("LZMA_write_2, addr = %x\r\n", des);
        flash_op_ret = hal_flash_write((uint32_t)des, buf, length);
    }
    if (flash_op_ret < 0)
    {
        LZMA_LOG("LZMA_write fail\r\n");
        return HAL_FLASH_STATUS_ERROR;
    }
    return HAL_FLASH_STATUS_OK;
}

static SRes Decode2Ram(CLzmaDec *state, uint8_t *des, uint8_t *src, uint32_t unpackSize, ISzAlloc *alloc)
{
    int thereIsSize = (unpackSize != (uint32_t)(int32_t)-1);
    Byte *inBuf;
    Byte *outBuf;
    size_t inPos = 0, inSize = 0, outPos = 0;
    hal_flash_status_t ret;
	  SRes res;
	  SizeT inProcessed;
	  SizeT outProcessed;
	  ELzmaStatus status;
	  ELzmaFinishMode finishMode;

    /* variable for read and write flash */
    uint8_t *read_ptr = src;
    uint8_t *write_ptr = des;
    int flash_wait_count = 0;

    if ((inBuf = (Byte *)alloc->Alloc(alloc, IN_BUF_SIZE)) == 0) 
    {
        LZMA_LOG("inbuf malloc failed!\r\n");
        return LZMA_ERROR_MEM;
    }

    if ((outBuf = (Byte *)alloc->Alloc(alloc, OUT_BUF_SIZE)) == 0) 
    {
        LZMA_LOG("outbuf malloc failed!\r\n");
        return LZMA_ERROR_MEM;
    }

    LzmaDec_Init(state);
    ret = hal_flash_init();
    if (ret < HAL_FLASH_STATUS_OK)
    {
        LZMA_LOG("LZMA_flash_init fail\r\n");
        return LZMA_ERROR_MEM;
    }

    for (;;)
    {
        if (inPos == inSize)
        {
            inSize = IN_BUF_SIZE;
            LZMA_LOG("LZMA_read_data, addr = %x \r\n", read_ptr);
            for(flash_wait_count=0;flash_wait_count<10000;flash_wait_count++){
              //tmp solution for flash write fail
            }
            if (GET_CHIP_ID() == CHIP_MT2523 && (int)read_ptr > RAM_ADDRESS_BASE)
            {
                ret = hal_flash_read((uint32_t)read_ptr - RAM_ADDRESS_BASE, inBuf, inSize);
            }
            else
            {
                ret = hal_flash_read((uint32_t)read_ptr, inBuf, inSize);
            }
                
            if (ret != HAL_FLASH_STATUS_OK)
            {
                LZMA_LOG("read data fail \r\n");
                return LZMA_ERROR_READ;
            }
            read_ptr = read_ptr + inSize;
            inPos = 0;
        }      

        inProcessed = inSize - inPos;
        outProcessed = OUT_BUF_SIZE - outPos;
        finishMode = LZMA_FINISH_ANY;
        if (thereIsSize && outProcessed > unpackSize)
        {
            outProcessed = (SizeT)unpackSize;
            finishMode = LZMA_FINISH_END;
        }

        //LZMA_LOG("LZAM_decode_begin: inPos = %d \r\n", inPos);
        res = LzmaDec_DecodeToBuf(state, outBuf + outPos, &outProcessed,
          inBuf + inPos, &inProcessed, finishMode, &status);
        //LZMA_LOG("LZAM_decode_end: inProcessed = %d, outProcessed = %d \r\n", inProcessed, outProcessed);
        inPos += inProcessed;
        outPos += outProcessed;
        unpackSize -= outProcessed;

        if (Write2Flash(write_ptr, outBuf, outPos) != HAL_FLASH_STATUS_OK)
        {            
            alloc->Free(alloc, inBuf);
            alloc->Free(alloc, outBuf);
            return LZMA_ERROR_WRITE;
        }
        write_ptr = write_ptr + outPos;
        outPos = 0;

        if (res != SZ_OK || (thereIsSize && unpackSize == 0))
        {
            alloc->Free(alloc, inBuf);
            alloc->Free(alloc, outBuf);
            return res;
        }
      
        if (inProcessed == 0 && outProcessed == 0)
        {
            if (thereIsSize || status != LZMA_STATUS_FINISHED_WITH_MARK)
            {
                alloc->Free(alloc, inBuf);
                alloc->Free(alloc, outBuf);
                return SZ_ERROR_DATA;
            }
            alloc->Free(alloc, inBuf);
            alloc->Free(alloc, outBuf);
            return res;
        }
        if (unpackSize == 0)
        {
            alloc->Free(alloc, inBuf);
            alloc->Free(alloc, outBuf);
            return LZMA_OK;
        }

    #if defined(BL_FOTA_ENABLE) || (MOD_CFG_FOTA_BL_RESERVED)
        hal_wdt_feed(HAL_WDT_FEED_MAGIC);
    #endif
    }
}


LZMA_ret lzma_decode2flash(
	uint8_t *destination,
	uint32_t reserved_size,
	const uint8_t *source,
	lzma_alloc_t *lzma_alloc)
{
    uint32_t unpackSize;
    int i;
    SRes res = 0;
    ISzAlloc *g_alloc;
    CLzmaDec state;
    unsigned char lzma_header[LZMA_PROPS_SIZE + 8];
    hal_flash_status_t read_ret;
    int flash_wait_count = 0;
    
    g_alloc = (ISzAlloc *)lzma_alloc;

    if ((uint32_t)destination % FLASH_BLOCK_SIZE != 0)
    {
        LZMA_LOG("destination error: must at begin address of one block");
        return LZMA_ERROR_MEM;
    }
    
    if (GET_CHIP_ID() == CHIP_MT2523 && (int)source > RAM_ADDRESS_BASE)
    {
        read_ret = hal_flash_read((uint32_t)source - RAM_ADDRESS_BASE, lzma_header, sizeof(lzma_header));
    }
    else
    {
        read_ret = hal_flash_read((uint32_t)source, lzma_header, sizeof(lzma_header));
    }
    if (read_ret != HAL_FLASH_STATUS_OK)
    {
        LZMA_LOG("read header fail\r\n");
        return LZMA_ERROR_READ;
    }

    unpackSize = 0;
    for (i = 0; i < 8; i++)
        unpackSize += (uint32_t)lzma_header[LZMA_PROPS_SIZE + i] << (i * 8);

    if(unpackSize > reserved_size)
    {
        LZMA_LOG("ERROR: decompressed size over reserverd size!!!\r\n");
        return LZMA_ERROR_MEM;
    }

    LzmaDec_Construct(&state);
    LzmaDec_Allocate(&state, lzma_header, LZMA_PROPS_SIZE, g_alloc);
    LZMA_LOG("LZMA decode begin: \r\n");
    LZMA_LOG("destination = %x\r\n", destination);
    LZMA_LOG("source = %x\r\n", source + sizeof(lzma_header));
    LZMA_LOG("unpacked_size = %d\r\n", unpackSize);

    for(flash_wait_count=0;flash_wait_count<10000;flash_wait_count++){
      //tmp solution for flash write fail
    }
    
    res = Decode2Ram(&state, destination, (uint8_t*)source + sizeof(lzma_header), unpackSize, g_alloc);
    LzmaDec_Free(&state, g_alloc);
    return res;
}

