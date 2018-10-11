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

#include <stdint.h>
#include <stdbool.h>
#include "bl_common.h"
#include "bl_fota.h"
#include "bl_mtk_bb_reg.h"
#include <string.h>
#include "bl_gpt.h"
#include "bl_wdt.h"
#include "hal_wdt.h"
#include "hal_flash.h"
#include "hal_flash_disk.h"
#include "mbedtls/sha1.h"
#include "hal_lzma_decode_interface.h"

#define BL_FOTA_TRIGGERED_ALWAYS_ON 0

#define E BL_PRINT(LOG_DEBUG,"e: %s %d\n\r", __FUNCTION__, __LINE__)
#define O BL_PRINT(LOG_DEBUG, "exit: %s %d\n", __FUNCTION__, __LINE__)

typedef enum {
    INIT_PHASE = 0,
    READ_PHASE,
    VERIFY_PHASE,
    UPDATE_PHASE,
    FINISH_PHASE
} bl_fota_progress_phase_t;

#ifndef __ICCARM__
static uint32_t page_buffer[BL_FOTA_PAGE_SIZE];
#else
__no_init static uint32_t page_buffer[BL_FOTA_PAGE_SIZE];
#endif
static bl_fota_header_t fota_head_buf;

void bl_fota_update_progress(bl_fota_progress_phase_t phase, uint32_t progress);
bool bl_fota_is_updating(void);

/* LZMA porting ( decompress a bin always use 4 buffer ) */
static int lzma_alloc_count = 0;
#ifndef __ICCARM__
static uint8_t lzma_buf_0[16384]; /* 15980 */
static uint8_t lzma_buf_1[16384]; /* 16384 */
static uint8_t lzma_buf_2[4096];  /* 4096 */
static uint8_t lzma_buf_3[4096];  /* 4096 */
#else
__no_init static uint8_t lzma_buf_0[16384]; /* 15980 */
__no_init static uint8_t lzma_buf_1[16384]; /* 16384 */
__no_init static uint8_t lzma_buf_2[4096];  /* 4096 */
__no_init static uint8_t lzma_buf_3[4096];  /* 4096 */
#endif

#ifdef BL_FOTA_FS_ENABLE

#include "ff.h"
FIL fdst_update;
FIL fdst_image;
FATFS fatfs_update;
#endif

void *bl_alloc(void *p, size_t size)
{
    BL_PRINT(LOG_DEBUG,"_bl_alloc size = %d \r\n", size);
    BL_PRINT(LOG_DEBUG,"_bl_alloc lzma_alloc_count = %d \r\n", lzma_alloc_count);
    switch (lzma_alloc_count) {
        case 0:
            lzma_alloc_count++;
            return &lzma_buf_0;
        case 1:
            lzma_alloc_count++;
            return &lzma_buf_1;
        case 2:
            lzma_alloc_count++;
            return &lzma_buf_2;
        case 3:
            lzma_alloc_count = 0;
            return &lzma_buf_3;
        default:
            E;
            return NULL;
    }
}
void bl_free(void *p, void *address)
{
    BL_PRINT(LOG_DEBUG,"_bl_free  \r\n");
}
lzma_alloc_t lzma_alloc = { bl_alloc, bl_free };


bl_fota_status_t bl_fota_compress_update(uint32_t bin_number)
{
    int ret = 0;
    uint32_t lzma_source ;
    uint32_t lzma_dest_addr;
    uint32_t lzma_dest_size;
    lzma_source = bl_custom_fota_start_address() + fota_head_buf.m_bin_info[bin_number].m_bin_offset;
    lzma_dest_addr = fota_head_buf.m_bin_info[bin_number].m_bin_start_addr;
    lzma_dest_size = fota_head_buf.m_bin_info[bin_number].m_partition_length;

    BL_PRINT(LOG_DEBUG,"p addr                                    = %x \n\r",  bl_custom_fota_start_address());
    BL_PRINT(LOG_DEBUG,"fota_head_buf.m_bin_info[%d].m_bin_length     = %x \n\r", bin_number, fota_head_buf.m_bin_info[bin_number].m_bin_length);
    BL_PRINT(LOG_DEBUG,"fota_head_buf.m_bin_info[%d].m_bin_offset     = %x \n\r", bin_number, fota_head_buf.m_bin_info[bin_number].m_bin_offset);
    BL_PRINT(LOG_DEBUG,"fota_head_buf.m_bin_info[%d].m_bin_start_addr = %x \n\r", bin_number, fota_head_buf.m_bin_info[bin_number].m_bin_start_addr);
    BL_PRINT(LOG_DEBUG,"fota_head_buf.m_bin_info[%d].m_partition_len  = %x \n\r", bin_number, fota_head_buf.m_bin_info[bin_number].m_partition_length);

    BL_PRINT(LOG_DEBUG,"lzma_source             = %x \r\n", lzma_source);
    BL_PRINT(LOG_DEBUG,"lzma_dest_size          = %x \r\n", lzma_dest_size);
    BL_PRINT(LOG_DEBUG,"lzma_dest_addr          = %x \r\n", lzma_dest_addr);

    ret = lzma_decode2flash(
              (uint8_t *)(lzma_dest_addr-bl_custom_rom_baseaddr()),
              lzma_dest_size,
              (uint8_t *)(lzma_source-bl_custom_rom_baseaddr()),
              &lzma_alloc);
    if ( ret != LZMA_OK) {
        BL_PRINT(LOG_DEBUG,"fota compress status = %d \r\n",ret);
        E;
        return BL_FOTA_ERROR_WRITE_TO_PARTITON;
    }
    return BL_FOTA_ERROR_NONE;
}


uint32_t bl_fota_get_block_size_length(uint32_t addr )
{
    /* 4K = 0x1000 */
    uint32_t block_size;
    uint32_t block_index;

    block_index = NOR_BlockIndex(addr);
    if (block_index == INVALID_BLOCK_INDEX) {
        E;
        return 0x1000;
    }
    block_size = NOR_BlockSize(block_index);

    BL_PRINT(LOG_DEBUG, "block size =%x \n\r", block_size);

    return block_size;
}

#ifdef BL_FOTA_FS_ENABLE
bl_fota_status_t bl_fota_write_update_info(void *buffer, int32_t buffer_size)
{
    FRESULT res; /* fs status infor*/
    UINT length_written = 0;
    FSIZE_t file_length = 0;

    res = f_open(&fdst_update, _T("0:/update"), FA_OPEN_EXISTING | FA_WRITE | FA_READ);

    if (FR_OK != res) {
        bl_print(LOG_DEBUG, "Failed to open update file:%d\n\r", res);
        res = f_close(&fdst_update);
        return BL_FOTA_ERROR_FS_OP;
    }

    file_length = f_size(&fdst_update);
    bl_print(LOG_DEBUG, "bl_fota_write_update_info file length (%d)\n\r", file_length);

    res = f_write(&fdst_update, buffer, buffer_size, &length_written);
    if (FR_OK != res) {
        bl_print(LOG_DEBUG, "Failed to write the file:%d\n\r",res);
        res = f_close(&fdst_update);
        return BL_FOTA_ERROR_FS_OP;
    }

    /* Close the open file object, flush the cached information of the file back to the volume.*/
    res = f_close(&fdst_update);
    if (FR_OK != res) {
        bl_print(LOG_DEBUG, "Failed to close the file:%d\n\r",res);
        res = f_close(&fdst_update);
        return BL_FOTA_ERROR_FS_OP;
    }
    return BL_FOTA_ERROR_NONE;
}

#else
/*
 * RAW write update info
 */
bl_fota_status_t bl_fota_write_update_info(void *buffer, int32_t buffer_size)
{
    hal_flash_status_t flash_status = HAL_FLASH_STATUS_OK;
    uint32_t marker_addr = bl_custom_fota_start_address() + bl_custom_fota_size() - BL_FOTA_UPDATE_INFO_RESERVE_SIZE;
    hal_flash_block_t flash_block_size_enum;
    uint32_t flash_block_size;
    uint32_t erase_addr;

    /* subtract flash base addr */
    marker_addr = marker_addr - bl_custom_rom_baseaddr();

    /* get block size */
    flash_block_size_enum = get_block_size_from_address(marker_addr);
    switch (flash_block_size_enum) {
        case HAL_FLASH_BLOCK_4K: /* 4K */
            flash_block_size = (1 << 12);
            break;
        case HAL_FLASH_BLOCK_32K: /* 32K */
            flash_block_size = (1 << 15);
            break;
        case HAL_FLASH_BLOCK_64K: /* 64K */
            flash_block_size = (1 << 16);
            break;
        default:
            flash_block_size = (1 << 12);
    }

    erase_addr = marker_addr & (~(flash_block_size - 1));
    BL_PRINT(LOG_DEBUG, "erase addr: %x, marker addr: %x\r\n", erase_addr, marker_addr);
    flash_status = hal_flash_erase(erase_addr, flash_block_size_enum);
    if (flash_status != HAL_FLASH_STATUS_OK) {
        BL_PRINT(LOG_DEBUG, "hal_flash_erase status = %d\r\n", flash_status);
        E;
        return BL_FOTA_ERROR_ERASE_MARKER;
    }
    flash_status = hal_flash_write(marker_addr, (uint8_t *)buffer, buffer_size);
    if (flash_status != HAL_FLASH_STATUS_OK) {
        BL_PRINT(LOG_DEBUG, "hal_flash_write status = %d\r\n", flash_status);
        E;
        return BL_FOTA_ERROR_WRITE_TO_PARTITON;
    }
    BL_PRINT(LOG_DEBUG, "write update info addr %x \r\n", marker_addr);

    return BL_FOTA_ERROR_NONE;

}
#endif

bl_fota_status_t bl_fota_write_marker()
{
    bl_fota_status_t status = BL_FOTA_ERROR_NONE;
    bl_fota_update_info_t ui;
    uint32_t i;

    /* prepare marker struct */
    memset(&ui, 0x0, sizeof(bl_fota_update_info_t));
    ui.m_ver = BL_FOTA_UPDATE_INFO_VERSION;
    ui.m_error_code = BL_FOTA_ERROR_NONE;
    ui.m_is_read = 0;
    for (i = 0; i < sizeof(ui.m_marker) / 4; i++) {
        *((uint32_t *)(ui.m_marker) + i) = BL_FOTA_MARKER;
    }

    /* write marker to flash */
    status = bl_fota_write_update_info( &ui, sizeof(bl_fota_update_info_t));

    return status;
}

/*
 * clear the marker and write update status
 */
bl_fota_status_t bl_fota_write_update_status(bl_fota_status_t update_status)
{
    bl_fota_status_t status = BL_FOTA_ERROR_NONE;
    bl_fota_update_info_t ui;
    uint32_t i;

    /* prepare update status struct */
    memset(&ui, 0x0, sizeof(bl_fota_update_info_t));
    /* set marker clear*/
    for (i = 0; i < sizeof(ui.m_marker) / 4; i++) {
        *((uint32_t *)(ui.m_marker) + i) = 0x0;
    }
    ui.m_error_code = update_status;

    status = bl_fota_write_update_info( &ui, sizeof(bl_fota_update_info_t));

    return status;
}

void bl_fota_update_progress(bl_fota_progress_phase_t phase, uint32_t progress)
{
    /*
    phase 0: initialzation, 5%
    phase 0: read image,    15%
    phase 1: verification,  30%
    phase 2: upgrading,     50%
    phase 3: finishing,     10%
    */

    const uint32_t phase_portion[] = {5, 15 , 30, 40, 10};
    const uint32_t step = 1;

    static uint32_t last_progress = 0;
    uint32_t total_progress = 0;
    uint32_t i;

    for (i = 0; i < phase; i++) {
        total_progress += phase_portion[i];
    }
    total_progress += progress * phase_portion[i] / 100;

#ifndef __FPGA_TARGET__
    hal_wdt_feed(HAL_WDT_FEED_MAGIC);
#endif

    if (total_progress >= last_progress + step) {
        last_progress = total_progress;
    }

    BL_PRINT(LOG_DEBUG, "bl_fota_update_progress: progress = %d/100   ( phase %d , progress %d ) \n\r", total_progress, phase, progress);

}

bl_fota_status_t bl_fota_do_update()
{
    hal_flash_status_t status = HAL_FLASH_STATUS_OK;
    uint32_t addr = fota_head_buf.m_bin_info[0].m_bin_start_addr;
    uint32_t length = fota_head_buf.m_bin_info[0].m_bin_length;
    uint32_t bin_offset = fota_head_buf.m_bin_info[0].m_bin_offset;
    uint32_t p = addr;
    uint32_t the_block_size;
    hal_flash_block_t flash_block_size_enum;
    uint32_t the_page_size;
    uint32_t page_to_write;
    int i;
    uint32_t os_bin_raw_seek;
    uint32_t os_bin_raw_end;
    void *p_buffer;
#ifdef BL_FOTA_FS_ENABLE
    FRESULT res; /* fs status infor*/
    UINT length_read = 0;
#else
    uint32_t fota_bin_raw_seek;
#endif
    p_buffer = &page_buffer;
    os_bin_raw_seek = addr - bl_custom_rom_baseaddr();
    os_bin_raw_end = addr + length - 1 - bl_custom_rom_baseaddr();
    the_page_size = BL_FOTA_PAGE_SIZE;
    flash_block_size_enum = get_block_size_from_address(os_bin_raw_seek);
    the_block_size = bl_fota_get_block_size_length(os_bin_raw_seek);

#ifdef BL_FOTA_FS_ENABLE
    res = f_lseek(&fdst_image, bin_offset);
    if (FR_OK != res) {
        E;
        res = f_close(&fdst_image);
        return BL_FOTA_ERROR_FS_OP;
    }
#else
    fota_bin_raw_seek = bl_custom_fota_start_address() + bin_offset - bl_custom_rom_baseaddr();
#endif

    for (; os_bin_raw_seek <= os_bin_raw_end; ) {

        /* erase a block */
        status = hal_flash_erase(os_bin_raw_seek,  flash_block_size_enum);
        if ( status != HAL_FLASH_STATUS_OK) {
            E;
            BL_PRINT(LOG_DEBUG, "hal_flash_erase , status = %d \r\n", status);
        }

        /* read a block from fota partition , write a block to os partition */
        page_to_write = the_block_size  / the_page_size;
        for (i = 0 ; status == HAL_FLASH_STATUS_OK && i < page_to_write; i++) {

        #ifdef BL_FOTA_FS_ENABLE
            res = f_read(&fdst_image, (uint8_t *)p_buffer, the_page_size, &length_read);
            if (res != FR_OK) {
                E;
                BL_PRINT(LOG_DEBUG, "f_read , res = %d \r\n", res);
                status = -1;
                break;
            }
            if (os_bin_raw_seek+length_read > os_bin_raw_end) {
                BL_PRINT(LOG_DEBUG, "f_read , (%d/%x/%x) \r\n", length_read, os_bin_raw_seek, os_bin_raw_end);
                memset((((uint8_t *)p_buffer)+os_bin_raw_end+1-os_bin_raw_seek), 0xff, the_page_size-(os_bin_raw_end+1-os_bin_raw_seek));
                if (os_bin_raw_seek > os_bin_raw_end) {
                    BL_PRINT(LOG_DEBUG, "bl_fota_do_update break , (%d/%d) \r\n", os_bin_raw_seek, os_bin_raw_end);
                    break;
                }
            }
        #else
            /* read a page */
            /* BL_PRINT(LOG_DEBUG,"HAL flash read %d B from %x \r\n",the_page_size,fota_bin_raw_seek); */
            status = hal_flash_read(fota_bin_raw_seek, (uint8_t *)p_buffer, the_page_size);
            if (status != HAL_FLASH_STATUS_OK) {
                E;
                BL_PRINT(LOG_DEBUG, "hal_flash_read , status = %d \r\n", status);
                break;
            }
            fota_bin_raw_seek += the_page_size;
        #endif
            /* write a page */
            /* BL_PRINT(LOG_DEBUG,"HAL flash write %d B to %x \r\n",the_page_size,os_bin_raw_seek); */
            status = hal_flash_write(os_bin_raw_seek, (uint8_t *)p_buffer, the_page_size);
            if (status != HAL_FLASH_STATUS_OK) {
                E;
                BL_PRINT(LOG_DEBUG, "hal_flash_write , status = %d \r\n", status);
                break;
            }
            os_bin_raw_seek += the_page_size;
        }

        /* offset a block */
        bin_offset += the_block_size;
        p += the_block_size;

        /* error handling */
        if (status != HAL_FLASH_STATUS_OK) {
            E;
            BL_PRINT(LOG_ERROR, "Error when processing os bin raw seek %d, status%d\n\r", os_bin_raw_seek, status);
            return BL_FOTA_ERROR_WRITE_TO_PARTITON;
        }
        bl_fota_update_progress(UPDATE_PHASE, bin_offset * 100 / length);

    }

    return BL_FOTA_ERROR_NONE;
}


bl_fota_status_t bl_fota_integrity_check()
{
    uint8_t signature_in_fota_pkg[BL_FOTA_SIGNATURE_SIZE];
    uint32_t sig_len  = fota_head_buf.m_bin_info[0].m_sig_length;
    uint32_t sig_size = sig_len;
    uint32_t bin_counter = fota_head_buf.m_bin_info[0].m_bin_length;
    uint32_t bin_offset = fota_head_buf.m_bin_info[0].m_bin_offset;
    uint32_t fota_bin_raw_seek;
    mbedtls_sha1_context ctx;
    unsigned char bin_sha1_sum[20];
    int i;
    void *buffer;
    void *p_buffer;
#ifdef BL_FOTA_FS_ENABLE
    FRESULT res; /* fs status infor*/
    UINT length_read = 0;
#else
    hal_flash_status_t hal_status   = HAL_FLASH_STATUS_OK;
    uint32_t sig_addr = ( bl_custom_fota_start_address() + fota_head_buf.m_bin_info[0].m_sig_offset ) - bl_custom_rom_baseaddr();
#endif

    p_buffer = &page_buffer;
    buffer = &signature_in_fota_pkg;
    mbedtls_sha1_init( &ctx );

#ifdef BL_FOTA_FS_ENABLE
    fota_bin_raw_seek = bin_offset;
    res = f_lseek(&fdst_image, fota_bin_raw_seek);
    if (FR_OK != res) {
        E;
        res = f_close(&fdst_image);
        return BL_FOTA_ERROR_LOADING_CHECKSUM;
    }
#else
    fota_bin_raw_seek = bl_custom_fota_start_address() + bin_offset - bl_custom_rom_baseaddr();
#endif

    /* calculate checksum */
    BL_PRINT(LOG_DEBUG, "sha1 start \r\n");
    mbedtls_sha1_starts( &ctx );

    while (bin_counter != 0) {

        BL_PRINT(LOG_DEBUG, "sha1 update %x (%d/%d) \r\n", fota_bin_raw_seek, bin_counter, fota_head_buf.m_bin_info[0].m_bin_length);

        if (bin_counter >= BL_FOTA_PAGE_SIZE) {
        #ifdef BL_FOTA_FS_ENABLE
            res = f_read(&fdst_image, (uint8_t *)p_buffer, BL_FOTA_PAGE_SIZE, &length_read);
            if (FR_OK == res) {
                mbedtls_sha1_update(&ctx, p_buffer, BL_FOTA_PAGE_SIZE);
            }
        #else
            hal_status = hal_flash_read(fota_bin_raw_seek, (uint8_t *)p_buffer, BL_FOTA_PAGE_SIZE);
            if (hal_status == HAL_FLASH_STATUS_OK) {
                mbedtls_sha1_update(&ctx, p_buffer, BL_FOTA_PAGE_SIZE);
            }
        #endif
            bin_counter -= BL_FOTA_PAGE_SIZE;
            fota_bin_raw_seek += BL_FOTA_PAGE_SIZE;
        } else {
        #ifdef BL_FOTA_FS_ENABLE
            res = f_read(&fdst_image, (uint8_t *)p_buffer, bin_counter, &length_read);
            if (FR_OK == res) {
                mbedtls_sha1_update(&ctx, p_buffer, bin_counter);
            }
        #else
            hal_status = hal_flash_read(fota_bin_raw_seek, (uint8_t *)p_buffer, bin_counter);
            if (hal_status == HAL_FLASH_STATUS_OK) {
                mbedtls_sha1_update(&ctx, p_buffer, bin_counter);
            }
        #endif
            bin_counter = 0;
        }

    #ifdef BL_FOTA_FS_ENABLE
        if (res != FR_OK) {
            bl_print(LOG_DEBUG, "Failed to read the file:%d\n\r", res);
            res = f_close(&fdst_image);
            E;
            break;
        }
    #else
        if (hal_status != HAL_FLASH_STATUS_OK) {
            E;
            break;
        }
    #endif
    }

    BL_PRINT(LOG_DEBUG, "sha1 finish \r\n");
    mbedtls_sha1_finish( &ctx, bin_sha1_sum );

#ifdef BL_FOTA_FS_ENABLE
    if (res != FR_OK) {
        E;
        return BL_FOTA_ERROR_LOADING_CHECKSUM;
    }
#else
    if (hal_status != HAL_FLASH_STATUS_OK) {
        E;
        return BL_FOTA_ERROR_LOADING_CHECKSUM;
    }
#endif

    /* dump calculate sig */
    BL_PRINT(LOG_DEBUG, "dump sig = ");
    for (i = 0; i < BL_FOTA_SIGNATURE_SIZE; i++) {
        BL_PRINT(LOG_DEBUG, "%x ", bin_sha1_sum[i]);
    }
    BL_PRINT(LOG_DEBUG, "\r\n");

    /* check fota bin sig length == BL_FOTA_SIGNATURE_SIZE(stack sig array size) */
    if (sig_len != BL_FOTA_SIGNATURE_SIZE) {
        BL_PRINT(LOG_DEBUG, "sig_len = %d\r\n", sig_len);
        E;
        return BL_FOTA_ERROR_LOADING_CHECKSUM;
    }

#ifdef BL_FOTA_FS_ENABLE
    res = f_lseek(&fdst_image, fota_head_buf.m_bin_info[0].m_sig_offset);
    if (FR_OK != res) {
        E;
        res = f_close(&fdst_image);
        return BL_FOTA_ERROR_LOADING_CHECKSUM;
    }
    res = f_read(&fdst_image, (uint8_t *)buffer, sig_size, &length_read);
    if (FR_OK != res) {
        E;
        res = f_close(&fdst_image);
        return BL_FOTA_ERROR_LOADING_CHECKSUM;
    }
#else
    /* read sig */
    hal_status = hal_flash_read(sig_addr, (uint8_t *)buffer, sig_size);
    if (hal_status != HAL_FLASH_STATUS_OK) {
        BL_PRINT(LOG_DEBUG, "hal_flash_read status = %d \r\n", hal_status);
        E;
        return BL_FOTA_ERROR_LOADING_CHECKSUM;
    }
#endif
    /* dump fota bin sig */
    BL_PRINT(LOG_DEBUG, "dump sig = ");
    for (i = 0; i < BL_FOTA_SIGNATURE_SIZE; i++) {
        BL_PRINT(LOG_DEBUG, "%x ", signature_in_fota_pkg[i]);
    }
    BL_PRINT(LOG_DEBUG, "\r\n");

    /* compare checksum */
    for (i = 0; i < BL_FOTA_SIGNATURE_SIZE; i++) {
        if (signature_in_fota_pkg[i] != bin_sha1_sum[i]) {
            BL_PRINT(LOG_DEBUG, "integrity check fail\r\n");
            E;
            return BL_FOTA_ERROR_CHECKSUM_VERIFY;
        }
    }

    BL_PRINT(LOG_DEBUG, "integrity check pass\r\n");

    return BL_FOTA_ERROR_NONE;
}

bl_fota_status_t bl_fota_load_and_check_fota_header()
{
    uint32_t fota_header_size      = sizeof(bl_fota_header_t);
    void *buffer;
#ifdef BL_FOTA_FS_ENABLE
    FRESULT res; /* fs status infor*/
    FSIZE_t file_length = 0;
    UINT length_read = 0;
#else
    hal_flash_status_t flash_status = HAL_FLASH_STATUS_OK;
    uint32_t fota_header_startaddr = bl_custom_fota_start_address() - bl_custom_rom_baseaddr();
#endif

    buffer = &fota_head_buf;

#ifdef BL_FOTA_FS_ENABLE
    res = f_open(&fdst_image, _T("0:/FOTA.bin"), FA_OPEN_EXISTING | FA_READ);

    file_length = f_size(&fdst_image);
    bl_print(LOG_DEBUG, "image file length (%d)\n\r", file_length);

    if (FR_OK != res) {
        bl_print(LOG_DEBUG, "Failed to open the file:%d\n\r",res);
        res = f_close(&fdst_image);
        return BL_FOTA_ERROR_LOADING_HEADER;
    }

    res = f_read(&fdst_image, (uint8_t *)buffer, fota_header_size, &length_read);
    if (FR_OK != res) {
        bl_print(LOG_DEBUG, "Failed to read the file:%d\n\r",res);
        res = f_close(&fdst_image);
        return BL_FOTA_ERROR_LOADING_HEADER;
    }
    bl_print(LOG_DEBUG, "length_read (%d)\n\r", length_read);
#else
    /* load header */
    BL_PRINT(LOG_DEBUG, "hal_flash_read \n\r");
    flash_status = hal_flash_read(fota_header_startaddr, (uint8_t *)buffer, fota_header_size);
    if (flash_status != HAL_FLASH_STATUS_OK) {
        BL_PRINT(LOG_DEBUG, "hal_flash_read status = %d\r\n", flash_status);
        E;
        return BL_FOTA_ERROR_LOADING_HEADER;
    }
#endif

    /* dump header */
    BL_PRINT(LOG_DEBUG, "fota_head_buf.m_magic_ver = %x \n\r", fota_head_buf.m_magic_ver);
    BL_PRINT(LOG_DEBUG, "fota_head_buf.m_bin_info[0].m_bin_length = %x \n\r", fota_head_buf.m_bin_info[0].m_bin_length);
    BL_PRINT(LOG_DEBUG, "fota_head_buf.m_bin_info[0].m_bin_offset = %x \n\r", fota_head_buf.m_bin_info[0].m_bin_offset);
    BL_PRINT(LOG_DEBUG, "fota_head_buf.m_bin_info[0].m_bin_start_addr = %x \n\r", fota_head_buf.m_bin_info[0].m_bin_start_addr);
    BL_PRINT(LOG_DEBUG, "fota_head_buf.m_bin_info[0].m_sig_length = %x \n\r", fota_head_buf.m_bin_info[0].m_sig_length);
    BL_PRINT(LOG_DEBUG, "fota_head_buf.m_bin_info[0].m_sig_offset = %x \n\r", fota_head_buf.m_bin_info[0].m_sig_offset);
    BL_PRINT(LOG_DEBUG, "fota_head_buf.m_bin_info[0].m_is_compressed = %x \n\r", fota_head_buf.m_bin_info[0].m_is_compressed);

    /* Check Magic Number */
    if ( BL_FOTA_HEADER_GET_MAGIC(fota_head_buf.m_magic_ver) != BL_FOTA_HEADER_MAGIC ) {
        BL_PRINT(LOG_DEBUG, "BL_FOTA_HEADER_GET_MAGIC fail\n\r");
        E;
        return BL_FOTA_ERROR_HEADER_FORMAT;
    }

    return BL_FOTA_ERROR_NONE;
}

bl_fota_status_t bl_fota_update_main(void)
{
    bl_fota_status_t status = BL_FOTA_ERROR_NONE;
    lzma_alloc_count = 0;

    /* set updating marker */
    if (status == BL_FOTA_ERROR_NONE) {
        if(bl_fota_is_updating() == FALSE){
            status = bl_fota_write_marker();
        }else{
            BL_PRINT(LOG_DEBUG, "updating marker exist");
        }
    }
    /* check 1: update file is cleared to 0. update marker existed at flash. */

    /* parse header */
    if (status == BL_FOTA_ERROR_NONE) {
        BL_PRINT(LOG_DEBUG, "bl_fota_load_and_check_fota_header\n\r");
        status = bl_fota_load_and_check_fota_header();
    }

    /* check 2: fota_head_buf is correct. */

    /* integrity check */
    if (status == BL_FOTA_ERROR_NONE) {
        bl_fota_update_progress(VERIFY_PHASE, 0);
        status = bl_fota_integrity_check();
    }

    /* check 3: sha1 signature checked. */

    /* error handling */
    if (status != BL_FOTA_ERROR_NONE) {
        bl_fota_write_update_status(status);
    }

    /* run fota update */
    if (status == BL_FOTA_ERROR_NONE) {
        bl_fota_update_progress(UPDATE_PHASE, 0);
        if (fota_head_buf.m_bin_info[0].m_is_compressed == 1) {
        #ifdef BL_FOTA_FS_ENABLE
            /* Compression not support in file system! */
            status = -1;
        #else
            status = bl_fota_compress_update(0);
        #endif
        } else {
            status = bl_fota_do_update();
        }
        if (status != BL_FOTA_ERROR_NONE) {
            BL_PRINT(LOG_DEBUG, "wrong is_compressed value = %x\n\r",fota_head_buf.m_bin_info[0].m_is_compressed);
        }
    }

    /* clean marker and set update status */
    if (status == BL_FOTA_ERROR_NONE) {
        bl_fota_update_progress(FINISH_PHASE, 50);
    }

    /* write update result */
    bl_fota_write_update_status(status);

    /* clear retention register flag */
    if (status == BL_FOTA_ERROR_NONE) {
        bl_fota_update_progress(FINISH_PHASE, 100);
    } else {
        BL_PRINT(LOG_DEBUG, "fota update fail \r\n");
    }

#ifndef __FPGA_TARGET__
    /* watch dog reset */
    hal_wdt_software_reset();
#endif
    return status;
}

#ifdef BL_FOTA_FS_ENABLE
bl_fota_status_t bl_fota_read_update_info(void *buffer, int32_t buffer_size )
{
    FRESULT res; /* fs status infor*/
    UINT length_read = 0;
    FSIZE_t file_length = 0;

    res = f_open(&fdst_update, _T("0:/update"), FA_OPEN_EXISTING | FA_READ);

    if (FR_OK != res) {
        bl_print(LOG_DEBUG, "Failed to open update file:%d\n\r", res);
        res = f_close(&fdst_update);
        return BL_FOTA_ERROR_LOADING_MARKER;
    }

    file_length = f_size(&fdst_update);
    bl_print(LOG_DEBUG, "bl_fota_read_update_info file length (%d)\n\r", file_length);

    res = f_read(&fdst_update, buffer, buffer_size, &length_read);
    if (FR_OK != res) {
        bl_print(LOG_DEBUG, "Failed to read the file:%d\n\r",res);
        res = f_close(&fdst_update);
        return BL_FOTA_ERROR_LOADING_MARKER;
    }
    bl_print(LOG_DEBUG, "length_read (%d)\n\r", length_read);

    res = f_close(&fdst_update);
    return BL_FOTA_ERROR_NONE;
}

#else
/*
 * RAW read update/trigger info
 */
bl_fota_status_t bl_fota_read_update_info(void *buffer, int32_t buffer_size )
{
    hal_flash_status_t flash_status = HAL_FLASH_STATUS_OK;
    uint32_t marker_addr = bl_custom_fota_start_address() + bl_custom_fota_size() - BL_FOTA_UPDATE_INFO_RESERVE_SIZE;
    marker_addr = marker_addr - bl_custom_rom_baseaddr();
    flash_status = hal_flash_read(marker_addr, (uint8_t *)buffer, buffer_size);
    if (flash_status != HAL_FLASH_STATUS_OK) {
        E;
        BL_PRINT(LOG_DEBUG, "hal_flash_read , status = %d \r\n", flash_status);
        return BL_FOTA_ERROR_LOADING_MARKER;
    }
    BL_PRINT(LOG_DEBUG, "read update info from %x \r\n", marker_addr);
    return BL_FOTA_ERROR_NONE;
}
#endif

/*
 * Assume marker exist in last block at updated image partition
*/
static bool bl_fota_check_updating_marker()
{
    bl_fota_status_t status = BL_FOTA_ERROR_NONE;
    uint32_t i;
    bool marker_found = TRUE;
    bl_fota_update_info_t ui;

    status = bl_fota_read_update_info(&ui, sizeof(bl_fota_update_info_t));
    if (status != BL_FOTA_ERROR_NONE) {
        E;
        return FALSE;
    }

    for (i = 0; i < sizeof(ui.m_marker) / 4; i++) {
        if (*((uint32_t *)(ui.m_marker) + i) != BL_FOTA_MARKER) {
            marker_found = FALSE;
        }
    }

    return marker_found;
}

bool bl_fota_is_updating()
{
    if (bl_fota_check_updating_marker()) {
        BL_PRINT(LOG_DEBUG, "fota is updating\n\r");
        return TRUE;
    }
    return FALSE;
}

bool bl_fota_is_triggered()
{
    /* Check the FOTA update trigger information based on the tail of the FOTA buffer*/
    bl_fota_status_t status = BL_FOTA_ERROR_NONE;
    fota_trigger_info_t ti;

#if BL_FOTA_TRIGGERED_ALWAYS_ON
    BL_PRINT(LOG_DEBUG, "fota always triggered\n\r");
    return TRUE;
#endif

#ifdef BL_FOTA_FS_ENABLE
    FRESULT res; /* fs status infor*/
    bl_print(LOG_DEBUG, "\r\nbegins to mount fs!\n\r");
    /* Mount the partition before it is accessed, and need only do it once. */
    res = f_mount(&fatfs_update, _T("0:"), 1);
    if (FR_OK != res) {
        bl_print(LOG_DEBUG, "\r\nFailed to mount the partition:%d\n\r",res);
        return FALSE;
    }
    bl_print(LOG_DEBUG, "\r\nend to mount fs\n\r");
#endif

    status = bl_fota_read_update_info(&ti, sizeof(fota_trigger_info_t));
    if (status != BL_FOTA_ERROR_NONE) {
        E;
        return FALSE;
    }

    if ( (ti.m_magic_ver&0x00FFFFFF) != FOTA_HEADER_MAGIC ) {
        O;
        return FALSE;
    }

    return TRUE;
}

void bl_fota_process(void)
{
#if defined(__CC_ARM)
    *((volatile uint32_t *)0xA0880004) = 3;
    *((volatile uint32_t *)0xA0880000) = 0;
#endif

    if ( bl_fota_is_triggered() || bl_fota_is_updating() ) {

        bl_fota_status_t status;

#ifndef __FPGA_TARGET__
        /* watch dog feed */
        hal_wdt_feed(HAL_WDT_FEED_MAGIC);
#endif

        BL_PRINT(LOG_DEBUG, "start fota update \n\r");
        status = bl_fota_update_main();

        if (status == BL_FOTA_ERROR_NONE) {
            /* Impossible to get here. */
            BL_PRINT(LOG_DEBUG, "FOTA update success\n\r");
        } else {
            BL_PRINT(LOG_DEBUG, "FOTA error%d\n\r", status);
            while (1);
        }
    }

}
