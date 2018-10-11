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

//#define I_O_DEBUG

#ifdef I_O_DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#include <hal_flash.h>
#include <hal_sha.h>
#include "hal_wdt.h"

#include "fota_internal.h"
#include "fota_port.h"
#include "hal_lzma_decode_interface.h"

#ifdef BL_FOTA_DEBUG
#include "hw_uart.h"
#define FOTA_INT_PRINT_I hw_uart_printf
#define FOTA_INT_PRINT_W hw_uart_printf
#define FOTA_INT_PRINT_E hw_uart_printf
#else
#include "syslog.h"
log_create_module(fota_internal, PRINT_LEVEL_INFO);
#define FOTA_INT_PRINT_I(_message,...)    LOG_I(fota_internal, (_message), ##__VA_ARGS__)
#define FOTA_INT_PRINT_W(_message,...)    LOG_W(fota_internal, (_message), ##__VA_ARGS__)
#define FOTA_INT_PRINT_E(_message,...)    LOG_E(fota_internal, (_message), ##__VA_ARGS__)
#endif

#ifdef I_O_DEBUG
#define I   FOTA_INT_PRINT_I("enter: %s %d\n", __FUNCTION__, __LINE__)
#define O   FOTA_INT_PRINT_I("exit: %s %d\n", __FUNCTION__, __LINE__)
#define E   FOTA_INT_PRINT_E("e: %s %d\n", __FUNCTION__, __LINE__)
#else
    
#ifdef BL_FOTA_DEBUG
#define I
#define O
#define E   FOTA_INT_PRINT_I("e: %s %d\n", __FUNCTION__, __LINE__)
#else
#define I
#define O
#define E
#endif

#endif

#define FOTA_HEADER_MAGIC                   0x004D4D4D
#define FOTA_HEADER_MAGIC_END_MARK          0x45454545
#define FOTA_HEADER_GET_MAGIC(magic_ver)    ((magic_ver)&0x00FFFFFF)
#define FOTA_HEADER_GET_VER(magic_ver)      ((magic_ver)>>24)

#define FOTA_SIGNATURE_SIZE                 (20)
#define FOTA_UPDATE_INFO_RESERVE_SIZE           (512)
#define FOTA_UPDATING_MARKER                    (0x544e5546)

#define FOTA_BIN_NUMBER_MAX                     4

typedef struct {
    int32_t m_magic_ver;
    int32_t m_fota_triggered;
} fota_trigger_info_t;

typedef struct {
    int32_t m_ver;
    int32_t m_error_code;
    int32_t m_behavior;
    int32_t m_is_read;
    char  m_marker[32];
    int32_t reserved[4];
} fota_update_info_t;

typedef struct {
    uint32_t m_bin_offset;
    uint32_t m_bin_start_addr;
    uint32_t m_bin_length;
    uint32_t m_partition_length;
    uint32_t m_sig_offset;
    uint32_t m_sig_length;
    uint32_t m_is_compressed;
    uint8_t m_bin_reserved[4];
} fota_bin_info_t;

typedef struct {
    uint32_t m_magic_ver;
    uint32_t m_bin_num;
    fota_bin_info_t m_bin_info[FOTA_BIN_NUMBER_MAX];
} fota_header_info_t;

static fota_flash_t *flash_s;

/**
 * Check whether <i>number</i> is power-of-2.
 *
 * Examples: 1, 2, 4, 8, ..., and so on.
 *
 * @retval true if <i>number</i> is power-of-2.
 * @retval false if <i>number</i> is not power-of-2.
 */
static bool _fota_is_power_of_2(uint32_t number)
{
    number &= number - 1;
    return (number == 0);
}


static fota_partition_t *_fota_find_partition(uint32_t partition)
{
    size_t  i;

    for (i = 0; i < flash_s->table_entries; i++) {
        if (flash_s->table[i].id == partition) {
            return &flash_s->table[i];
        }
    }

    return NULL;
}


#if defined(MOD_CFG_FOTA_BL_RESERVED)
static fota_header_info_t fota_head;

/* LZMA porting ( decompress a bin always use 4 buffer ) */
static int lzma_alloc_count = 0;
static uint8_t lzma_buf_0[16384]; /* 15980 */
static uint8_t lzma_buf_1[16384]; /* 16384 */
static uint8_t lzma_buf_2[4096];  /* 4096 */
static uint8_t lzma_buf_3[4096];  /* 4096 */
void *_bl_alloc(void *p, size_t size)
{
    FOTA_INT_PRINT_I("_bl_alloc size = %d \r\n", size);
    FOTA_INT_PRINT_I("_bl_alloc lzma_alloc_count = %d \r\n", lzma_alloc_count);
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
void _bl_free(void *p, void *address)
{
    FOTA_INT_PRINT_I("_bl_free  \r\n");
}
lzma_alloc_t lzma_alloc = { _bl_alloc, _bl_free };


static fota_status_t _fota_check_updating_marker(uint32_t partition)
{
    fota_update_info_t fota_update_info;
    void *fota_update_info_buf;
    fota_status_t status = FOTA_STATUS_OK;
    int marker_found = 1;
    fota_partition_t *p;
    int i;

    I;
    fota_update_info_buf = &fota_update_info;
    marker_found = 1;

    /* get fota partition ptr */
    if ((p = _fota_find_partition(partition)) == NULL) {
        E;
        O;
        return FOTA_STATUS_ERROR_UNKNOWN_ID;
    }

    /* seek and read marker */
    status = fota_seek( partition, (p->length - FOTA_UPDATE_INFO_RESERVE_SIZE) );
    if (status != FOTA_STATUS_OK) {
        E;
        O;
        return status;
    }

    status = fota_read(partition, (uint8_t *)fota_update_info_buf, sizeof(fota_update_info_t));
    if (status != FOTA_STATUS_OK) {
        E;
        O;
        return FOTA_STATUS_ERROR_FLASH_OP;
    }

    /* check marker */
    for (i = 0; i < sizeof(fota_update_info.m_marker) / 4; i++) {
        if (*((uint32_t *)(fota_update_info.m_marker) + i) != FOTA_UPDATING_MARKER) {
            marker_found = 0;
        }
    }
    if (marker_found == 1) {
        O;
        return FOTA_STATUS_IS_FULL;
    }
    O;
    return FOTA_STATUS_IS_EMPTY;
}


static fota_status_t _fota_check_fota_triggered(uint32_t partition)
{
    fota_status_t status = FOTA_STATUS_OK;
    fota_trigger_info_t fota_triiger_info;
    void *fota_triiger_info_buf;
    fota_partition_t *p;

    I;
    fota_triiger_info_buf = &fota_triiger_info;

    /* get fota partition ptr */
    if ((p = _fota_find_partition(partition)) == NULL) {
        E;
        O;
        return FOTA_STATUS_ERROR_UNKNOWN_ID;
    }

    /* seek and read fota triggered */
    status = fota_seek( partition, (p->length - FOTA_UPDATE_INFO_RESERVE_SIZE) );
    if (status != FOTA_STATUS_OK) {
        E;
        O;
        return status;
    }

    status = fota_read(partition, (uint8_t *)fota_triiger_info_buf, sizeof(fota_trigger_info_t));
    if (status != FOTA_STATUS_OK) {
        E;
        O;
        return FOTA_STATUS_ERROR_FLASH_OP;
    }

    if ( FOTA_HEADER_GET_MAGIC(fota_triiger_info.m_magic_ver) != FOTA_HEADER_MAGIC ) {
        O;
        return FOTA_STATUS_IS_EMPTY;
    }
    O;
    return FOTA_STATUS_IS_FULL;
}


static fota_status_t _fota_erase_final_block(uint32_t partition)
{
    fota_partition_t *p;
    uint32_t addr;

    I;
    p = _fota_find_partition(partition);
    /* erase final block (4K) */
    addr = p->address + p->length - 0x1000;
    if ((addr % flash_s->block_size) == 0) {
        fota_port_isr_disable();
        if (hal_flash_erase(addr, HAL_FLASH_BLOCK_4K) < 0) {
            fota_port_isr_enable();
            E;
            O;
            return FOTA_STATUS_ERROR_FLASH_OP;
        }

        fota_port_isr_enable();
        O;
        return FOTA_STATUS_OK;
    }
    E;
    O;
    return FOTA_STATUS_ERROR_FLASH_OP;
}


static fota_status_t _fota_write_update_status(uint32_t partition, int32_t error_code)
{
    fota_status_t status = FOTA_STATUS_OK;
    fota_update_info_t iot_fota_update_info;
    void *iot_fota_update_info_buf;
    fota_partition_t    *p;

    I;
    /* erase final block in TMP partition */
    _fota_erase_final_block(partition);

    /* set update status in final block */
    iot_fota_update_info_buf = &iot_fota_update_info;
    iot_fota_update_info.m_ver = 0;
    iot_fota_update_info.m_error_code = error_code;
    iot_fota_update_info.m_behavior = 0;
    iot_fota_update_info.m_is_read = 0;

    p = _fota_find_partition(partition);
    status = fota_seek( partition, (p->length - FOTA_UPDATE_INFO_RESERVE_SIZE) );
    if ( status != FOTA_STATUS_OK) {
        E;
        O;
        return status;
    }

    status = fota_write( partition, (uint8_t *)iot_fota_update_info_buf, sizeof(fota_update_info_t));
    if ( status != FOTA_STATUS_OK) {
        E;
        O;
        return status;
    }

    O;
    return status;
}

static fota_status_t _fota_write_marker(uint32_t partition)
{
    fota_status_t status = FOTA_STATUS_OK;
    fota_partition_t *p;
    fota_update_info_t iot_fota_update_info;
    void *iot_fota_update_info_buf;
    int i;

    I;
    /* erase final block in TMP partition */
    _fota_erase_final_block(partition);

    /* write marker*/
    iot_fota_update_info_buf = &iot_fota_update_info;
    memset(&iot_fota_update_info, 0x0, sizeof(fota_update_info_t));
    for (i = 0; i < sizeof(iot_fota_update_info.m_marker) / 4; i++) {
        *((uint32_t *)(iot_fota_update_info.m_marker) + i) = FOTA_UPDATING_MARKER;
    }
    p = _fota_find_partition(partition);
    status = fota_seek( partition, (p->length - FOTA_UPDATE_INFO_RESERVE_SIZE) );
    if ( status != FOTA_STATUS_OK) {
        E;
        O;
        return status;
    }
    status = fota_write( partition, (uint8_t *)iot_fota_update_info_buf, sizeof(fota_update_info_t));
    if ( status != FOTA_STATUS_OK) {
        E;
    }
    O;
    return status;
}

static fota_status_t _fota_parse_header(uint32_t partition)
{
    fota_status_t  fota_status = FOTA_STATUS_OK;
    void *fota_head_buf;
    int i;
    hal_sha_status_t sha_status;
    hal_sha1_context_t sha1_context;
    uint8_t header_sha1_checksum[64] = {0};
    uint8_t header_checksum[FOTA_SIGNATURE_SIZE];

    I;
    fota_head_buf = &fota_head;
    fota_status = fota_read(partition, (uint8_t *)fota_head_buf, sizeof(fota_header_info_t));
    if (fota_status == FOTA_STATUS_OK) {
        /* calculate header checksum */
        FOTA_INT_PRINT_I("header sha1 init \r\n");
        sha_status = hal_sha1_init(&sha1_context);
        if ( sha_status != HAL_SHA_STATUS_OK) {
            E;
            fota_status = FOTA_STATUS_ERROR_INVALD_PARAMETER;
            return fota_status;
        }
        sha_status = hal_sha1_append(&sha1_context, fota_head_buf, sizeof(fota_header_info_t));
        if (sha_status != HAL_SHA_STATUS_OK) {
            fota_status = FOTA_STATUS_ERROR_INVALD_PARAMETER;
            E;
        }
        FOTA_INT_PRINT_I("header end \r\n");
        sha_status = hal_sha1_end(&sha1_context, header_sha1_checksum);
        if ( sha_status != HAL_SHA_STATUS_OK) {
            E;
            fota_status = FOTA_STATUS_ERROR_INVALD_PARAMETER;
            return fota_status;
        }
        FOTA_INT_PRINT_I("header checksum   = ");
        for (i = 0; i < HAL_SHA1_DIGEST_SIZE; i++) {
            FOTA_INT_PRINT_I("%x", header_sha1_checksum[i]);
        }
        FOTA_INT_PRINT_I("\r\n");
        /* read header checksum */
        fota_status = fota_read(partition, header_checksum, FOTA_SIGNATURE_SIZE);
        if ( fota_status != FOTA_STATUS_OK) {
            E;
            fota_status = FOTA_STATUS_ERROR_FLASH_OP;
            return fota_status;
        }
        FOTA_INT_PRINT_I("header checksum   = ");
        for (i = 0; i < HAL_SHA1_DIGEST_SIZE; i++) {
            FOTA_INT_PRINT_I("%x", header_checksum[i]);
        }
        FOTA_INT_PRINT_I("\r\n");

        /* compare checksum */
        for (i = 0; i < HAL_SHA1_DIGEST_SIZE; i++) {
            if (header_sha1_checksum[i] != header_checksum[i]) {
                FOTA_INT_PRINT_E("header integrity check fail\r\n");
                E;
                return FOTA_STATUS_ERROR_INVALD_PARAMETER;
            }
        }

        /* dump fota header */
        FOTA_INT_PRINT_I("fota_head.m_magic_ver                    = %x \n\r", fota_head.m_magic_ver);
        FOTA_INT_PRINT_I("fota_head.m_bin_num                      = %x \n\r", fota_head.m_bin_num);
        for (i = 0; i < FOTA_BIN_NUMBER_MAX; i++) {
            FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_bin_length     = %x \n\r", i, fota_head.m_bin_info[i].m_bin_length);
            FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_bin_offset     = %x \n\r", i, fota_head.m_bin_info[i].m_bin_offset);
            FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_bin_start_addr = %x \n\r", i, fota_head.m_bin_info[i].m_bin_start_addr);
            FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_partition_len  = %x \n\r", i, fota_head.m_bin_info[i].m_partition_length);
            FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_sig_length     = %x \n\r", i, fota_head.m_bin_info[i].m_sig_length);
            FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_sig_offset     = %x \n\r", i, fota_head.m_bin_info[i].m_sig_offset);
            FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_is_compressed  = %x \n\r", i, fota_head.m_bin_info[i].m_is_compressed);
        }

        if ( fota_head.m_bin_num > FOTA_BIN_NUMBER_MAX ) {
            E;
            fota_status = FOTA_STATUS_ERROR_INVALD_PARAMETER;
        }

        if ( FOTA_HEADER_GET_MAGIC(fota_head.m_magic_ver) != FOTA_HEADER_MAGIC ) {
            E;
            fota_status = FOTA_STATUS_ERROR_INVALD_PARAMETER;
        }
    } else {
        E;
    }

    O;
    return fota_status;
}


static void _fota_show_progress(uint32_t current, uint32_t max)
{
    uint32_t percentage = (current * 100) / max;

    /* feed watchdog regularly. */
    hal_wdt_feed(HAL_WDT_FEED_MAGIC);

    FOTA_INT_PRINT_I("progress = %d/100\r\n", percentage);
}


static fota_status_t _fota_integrity_check(uint32_t source, uint8_t  *buffer, uint32_t length, uint32_t bin_number)
{
    hal_sha_status_t sha_status;
    fota_status_t  status;
    fota_status_t  fota_status = FOTA_STATUS_OK;
    hal_sha1_context_t sha1_context;
    uint32_t bin_counter;
    uint8_t bin_sha1_checksum[64] = {0};
    int i;
    uint8_t fota_checksum[FOTA_SIGNATURE_SIZE];
    /* calculate os bin checksum */
    FOTA_INT_PRINT_I("sha1 init \r\n");
    sha_status = hal_sha1_init(&sha1_context);
    if ( sha_status != HAL_SHA_STATUS_OK) {
        E;
        fota_status = FOTA_STATUS_ERROR_FLASH_OP;
        return fota_status;
    }
    bin_counter = fota_head.m_bin_info[bin_number].m_bin_length;
    status = fota_seek(source, fota_head.m_bin_info[bin_number].m_bin_offset);
    if ( status != FOTA_STATUS_OK) {
        E;
        fota_status = FOTA_STATUS_ERROR_FLASH_OP;
        return fota_status;
    }
    FOTA_INT_PRINT_I("calculate os bin checksum :\r\n");
    _fota_show_progress(fota_head.m_bin_info[bin_number].m_bin_length - bin_counter, fota_head.m_bin_info[bin_number].m_bin_length);
    while (bin_counter != 0) {

        if (bin_counter >= length) {
            status = fota_read(source, buffer, length);
            if (status == FOTA_STATUS_OK) {
                sha_status = hal_sha1_append(&sha1_context, buffer, length);
            }
            bin_counter -= length;
        } else {
            status = fota_read(source, buffer, bin_counter);
            if (status == FOTA_STATUS_OK) {
                sha_status = hal_sha1_append(&sha1_context, buffer, bin_counter);
            }
            bin_counter = 0;
        }

        _fota_show_progress(fota_head.m_bin_info[bin_number].m_bin_length - bin_counter, fota_head.m_bin_info[bin_number].m_bin_length);

        if (status != FOTA_STATUS_OK) {
            fota_status = status;
            E;
            break;
        }

        if (sha_status != HAL_SHA_STATUS_OK) {
            fota_status = FOTA_STATUS_ERROR_FLASH_OP;
            E;
            break;
        }

    }
    FOTA_INT_PRINT_I("sha1 end \r\n");
    sha_status = hal_sha1_end(&sha1_context, bin_sha1_checksum);
    if ( sha_status != HAL_SHA_STATUS_OK) {
        E;
        fota_status = FOTA_STATUS_ERROR_FLASH_OP;
        return fota_status;
    }

    if (fota_status != FOTA_STATUS_OK) {
        E;
        return fota_status;
    }
    FOTA_INT_PRINT_I("os bin checksum   = ");
    for (i = 0; i < HAL_SHA1_DIGEST_SIZE; i++) {
        FOTA_INT_PRINT_I("%x", bin_sha1_checksum[i]);
    }
    FOTA_INT_PRINT_I("\r\n");

    /* read checksum from fota bin */
    status = fota_seek(source, fota_head.m_bin_info[bin_number].m_sig_offset);
    if ( status != FOTA_STATUS_OK) {
        E;
        fota_status = FOTA_STATUS_ERROR_FLASH_OP;
        return fota_status;
    }
    status = fota_read(source, fota_checksum, fota_head.m_bin_info[bin_number].m_sig_length);
    if ( status != FOTA_STATUS_OK) {
        E;
        fota_status = FOTA_STATUS_ERROR_FLASH_OP;
        return fota_status;
    }
    FOTA_INT_PRINT_I("fota bin checksum = ");
    for (i = 0; i < HAL_SHA1_DIGEST_SIZE; i++) {
        FOTA_INT_PRINT_I("%x", fota_checksum[i]);
    }
    FOTA_INT_PRINT_I("\r\n");

    /* compare checksum */
    for (i = 0; i < HAL_SHA1_DIGEST_SIZE; i++) {
        if (fota_checksum[i] != bin_sha1_checksum[i]) {
            FOTA_INT_PRINT_E("integrity check fail\r\n");
            fota_status = FOTA_STATUS_ERROR_FLASH_OP;
        }
    }

    if (fota_status == FOTA_STATUS_OK) {
        FOTA_INT_PRINT_I("integrity check pass\r\n");
    }
    return fota_status;
}


fota_status_t _fota_uncompress_update(uint32_t target, uint32_t source, uint8_t  *buffer, uint32_t length , uint32_t bin_number)
{
    fota_status_t  fota_status = FOTA_STATUS_OK;
    fota_partition_t    *p;
    uint32_t bin_counter;
    /* change os start address , use fota header */
    p = _fota_find_partition(target);
    FOTA_INT_PRINT_I("CM4 partition hard code addr = %x \r\n", p->address);
    p->address = fota_head.m_bin_info[bin_number].m_bin_start_addr;
    p->length = fota_head.m_bin_info[bin_number].m_bin_length;
    FOTA_INT_PRINT_I("CM4 partition fota head addr = %x \r\n", p->address);

    fota_seek(target, 0);
    fota_seek(source, fota_head.m_bin_info[bin_number].m_bin_offset);
    bin_counter = fota_head.m_bin_info[bin_number].m_bin_length;
    FOTA_INT_PRINT_I("start fota update : \r\n");


    /* run fota update */
    while (1) {
        _fota_show_progress(fota_head.m_bin_info[bin_number].m_bin_length - bin_counter, fota_head.m_bin_info[bin_number].m_bin_length);
        if (bin_counter >= length) {
            fota_status = fota_read(source, buffer, length);
            if (fota_status != FOTA_STATUS_OK) {
                E;
                O;
                return fota_status;
            }
            fota_status = fota_write(target, buffer, length);
            bin_counter -= length;
        } else {
            fota_status = fota_read(source, buffer, bin_counter);
            if (fota_status != FOTA_STATUS_OK) {
                E;
                O;
                return fota_status;
            }
            fota_status = fota_write(target, buffer, bin_counter);
            bin_counter = 0;
        }

        if (fota_status != FOTA_STATUS_OK) {
            E;
            O;
            return fota_status;
        }

        if (bin_counter == 0) {
            FOTA_INT_PRINT_I("fota break\r\n");
            break;
        }
    }
    return fota_status;
}

fota_status_t _fota_compress_update(uint32_t source, uint32_t bin_number)
{
    int ret = 0;
    fota_partition_t    *p;
    uint32_t lzma_source ;
    uint32_t lzma_dest_addr;
    uint32_t lzma_dest_size;
    p = _fota_find_partition(source);
    lzma_source = p->address + fota_head.m_bin_info[bin_number].m_bin_offset;
    lzma_dest_addr = fota_head.m_bin_info[bin_number].m_bin_start_addr;
    lzma_dest_size = fota_head.m_bin_info[bin_number].m_partition_length;

    FOTA_INT_PRINT_I("p addr                                    = %x \n\r", p->address);
    FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_bin_length     = %x \n\r", bin_number, fota_head.m_bin_info[bin_number].m_bin_length);
    FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_bin_offset     = %x \n\r", bin_number, fota_head.m_bin_info[bin_number].m_bin_offset);
    FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_bin_start_addr = %x \n\r", bin_number, fota_head.m_bin_info[bin_number].m_bin_start_addr);
    FOTA_INT_PRINT_I("fota_head.m_bin_info[%d].m_partition_len  = %x \n\r", bin_number, fota_head.m_bin_info[bin_number].m_partition_length);

    FOTA_INT_PRINT_I("lzma_source             = %x \r\n", lzma_source);
    FOTA_INT_PRINT_I("lzma_dest_size          = %x \r\n", lzma_dest_size);
    FOTA_INT_PRINT_I("lzma_dest_addr          = %x \r\n", lzma_dest_addr);

    ret = lzma_decode2flash(
              (uint8_t *)lzma_dest_addr,
              lzma_dest_size,
              (uint8_t *)lzma_source,
              &lzma_alloc);
    if ( ret != LZMA_OK) {
        FOTA_INT_PRINT_I("fota compress status = %d \r\n",ret);
        E;
        return FOTA_STATUS_ERROR_FLASH_OP;
    }
    return FOTA_STATUS_OK;
}

fota_status_t fota_copy(uint32_t target,
                        uint32_t source,
                        uint8_t  *buffer,
                        uint32_t length)
{
    fota_status_t  fota_status = FOTA_STATUS_OK;
    int i;

    I;

    /* Check the validity of parameters. */
    if (buffer == NULL || length == 0) {
        O;
        fota_status = FOTA_STATUS_ERROR_INVALD_PARAMETER;
    }

    /* fota parse header and header integrity check*/
    if (fota_status == FOTA_STATUS_OK) {
        fota_status = _fota_parse_header(source);
        if (fota_status != FOTA_STATUS_OK) {
            E;
        }
    }

    /* Integrity check */
    if (fota_status == FOTA_STATUS_OK) {
        for (i = 0; i < fota_head.m_bin_num; i++) {
            fota_status = _fota_integrity_check(source, buffer, length, i);
            if (fota_status != FOTA_STATUS_OK) {
                E;
                break;
            }
        }
    }

    /* error handling */
    if (fota_status != FOTA_STATUS_OK) {
        _fota_write_update_status(source, fota_status);
    }

    /** start fota update **/
    /* set updating marker ( if updating marker exist , do not write marker again )*/
    if ( _fota_check_updating_marker(source) == FOTA_STATUS_IS_FULL) {
        O;
        FOTA_INT_PRINT_I("updating marker exists\r\n");
    }else{
        if (fota_status == FOTA_STATUS_OK) {
            fota_status = _fota_write_marker(source);
            if ( fota_status != FOTA_STATUS_OK) {
                E;
            }
        }
    }

    /* update OS partition */
    if (fota_status == FOTA_STATUS_OK) {
        for (i = 0; i < fota_head.m_bin_num; i++) {
            if (fota_head.m_bin_info[i].m_is_compressed == 1) {
                fota_status = _fota_compress_update(source, i);
            } else {
                fota_status = _fota_uncompress_update(target, source, buffer, length, i);
            }
            if (fota_status != FOTA_STATUS_OK) {
                E;
                break;
            }
        }
    }

    /* update finish , clean marker and set update status */
    if (fota_status == FOTA_STATUS_OK) {
        fota_status = _fota_write_update_status(source, fota_status);
        if (fota_status != FOTA_STATUS_OK) {
            E;
        }
    }

    return fota_status;

}


fota_status_t fota_is_empty(uint32_t partition)
{
    fota_status_t status = FOTA_STATUS_OK;

    I;
    /* Check the validity of parameters. */

    if (flash_s == NULL) {
        E;
        O;
        return FOTA_STATUS_ERROR_NOT_INITIALIZED;
    }

    /* check is updating marker */

    status = _fota_check_updating_marker(partition);
    if ( status == FOTA_STATUS_IS_FULL) {
        O;
        FOTA_INT_PRINT_I("found updating marker\r\n");
        return status;
    }

    /* check fota triggered */
    status = _fota_check_fota_triggered(partition);
    if ( status == FOTA_STATUS_IS_FULL) {
        O;
        FOTA_INT_PRINT_I("found fota triggered\r\n");
        return status;
    }

    O;
    return FOTA_STATUS_IS_EMPTY;
}

#endif /* MOD_CFG_FOTA_BL_RESERVED */

fota_status_t fota_init(fota_flash_t *flash)
{
    size_t  i;

    I;

    if (!flash || !flash->table || flash->table_entries == 0 ||
            !_fota_is_power_of_2(flash->block_size)) {

        O;

        return FOTA_STATUS_ERROR_INVALD_PARAMETER;
    }

    flash_s = flash;

    for (i = 0; i < flash->table_entries; i++) {
        if (flash->table[i].address % flash->block_size ||
                flash->table[i].length  % flash->block_size) {

            O;

            return FOTA_STATUS_ERROR_BLOCK_ALIGN;
        }
    }

    hal_flash_init();

    O;

    return FOTA_STATUS_OK;
}







/**
 * Make the <i>partition</i> empty (remove first block) and make
 * fota_is_empty() detects the partition is empty.
 *
 * @retval FOTA_STATUS_ERROR_UNKNOWN_ID if the <i>partition</i> is not in
 * partition table.
 *
 * @retval FOTA_STATUS_ERROR_NOT_INITIALIZED if FOTA was not initialized.
 */
fota_status_t fota_make_empty(uint32_t partition)
{
    fota_partition_t    *p;

    I;

    /* 1. Check the validity of parameters. */

    if (flash_s == NULL) {
        O;
        return FOTA_STATUS_ERROR_NOT_INITIALIZED;
    }

    if ((p = _fota_find_partition(partition)) == NULL) {
        O;
        return FOTA_STATUS_ERROR_UNKNOWN_ID;
    }

    /*
     * 2. Erase first block.
     */

    fota_port_isr_disable();

    if (hal_flash_erase(p->address, HAL_FLASH_BLOCK_4K) < 0) {
        fota_port_isr_enable();
        O;
        return FOTA_STATUS_ERROR_FLASH_OP;
    }

    fota_port_isr_enable();

    return FOTA_STATUS_OK;
}


fota_status_t fota_seek(uint32_t partition, uint32_t offset)
{
    fota_partition_t    *p;

    I;

    /* 1. Check the validity of parameters. */

    if (flash_s == NULL) {
        O;
        return FOTA_STATUS_ERROR_NOT_INITIALIZED;
    }

    if ((p = _fota_find_partition(partition)) == NULL) {
        O;
        return FOTA_STATUS_ERROR_UNKNOWN_ID;
    }

    if (offset >= p->length) {
        O;
        return FOTA_STATUS_ERROR_OUT_OF_RANGE;
    }

    p->offset = offset;
    O;
    return FOTA_STATUS_OK;
}


fota_status_t fota_read(uint32_t partition, uint8_t *buffer, uint32_t length)
{
    fota_partition_t    *p;

    I;

    /* 1. Check the validity of parameters. */

    if (flash_s == NULL) {
        O;
        return FOTA_STATUS_ERROR_NOT_INITIALIZED;
    }

    if (buffer == 0 || length == 0) {
        O;
        return FOTA_STATUS_ERROR_INVALD_PARAMETER;
    }

    if ((p = _fota_find_partition(partition)) == NULL) {
        O;
        return FOTA_STATUS_ERROR_UNKNOWN_ID;
    }

    if ((p->offset + length) > p->length) {
        O;
        return FOTA_STATUS_ERROR_OUT_OF_RANGE;
    }

    /*
     * 2. Read from flash.
     */

    fota_port_isr_disable();

    if (hal_flash_read(p->address + p->offset, buffer, length) < 0) {
        fota_port_isr_enable();
        O;
        return FOTA_STATUS_ERROR_FLASH_OP;
    }

    fota_port_isr_enable();
    p->offset += length;

    O;
    return FOTA_STATUS_OK;
}


fota_status_t fota_write(uint32_t partition, const uint8_t *buffer, uint32_t length)
{
    fota_partition_t    *p;
    uint32_t            addr;
    uint32_t            block_idx_start;
    uint32_t            block_idx_end;
    uint32_t            erase_addr;
    uint32_t            i;

    I;

    /* 1. Check the validity of parameters. */

    if (flash_s == NULL) {
        O;
        return FOTA_STATUS_ERROR_NOT_INITIALIZED;
    }

    if (buffer == 0 || length == 0) {
        O;
        return FOTA_STATUS_ERROR_INVALD_PARAMETER;
    }

    if ((p = _fota_find_partition(partition)) == NULL) {
        O;
        return FOTA_STATUS_ERROR_UNKNOWN_ID;
    }

    if ((p->offset + length) > p->length) {
        O;
        return FOTA_STATUS_ERROR_OUT_OF_RANGE;
    }

    /*
     * 2. Erase block.
     *
     * if the write is to the block boundary, erase the block
     */

    addr = p->address + p->offset;

    block_idx_start = addr / flash_s->block_size;
    block_idx_end = (addr + length - 1) / flash_s->block_size;

    if ((addr % flash_s->block_size) == 0) {
        fota_port_isr_disable();

        if (hal_flash_erase(addr, HAL_FLASH_BLOCK_4K) < 0) {
            fota_port_isr_enable();
            O;
            return FOTA_STATUS_ERROR_FLASH_OP;
        }

        fota_port_isr_enable();
    }

    i = block_idx_start + 1;
    while (i <= block_idx_end) {
        erase_addr = i * flash_s->block_size;
        
        fota_port_isr_disable();
        if (hal_flash_erase(erase_addr, HAL_FLASH_BLOCK_4K) < 0) {
            fota_port_isr_enable();
            O;
            return FOTA_STATUS_ERROR_FLASH_OP;
        }

        fota_port_isr_enable();
        i++;
    }

    /* 3. Write data. */

    fota_port_isr_disable();

    if (hal_flash_write(addr, (uint8_t *)buffer, length) < 0) {
        fota_port_isr_enable();
        O;
        return FOTA_STATUS_ERROR_FLASH_OP;
    }

    fota_port_isr_enable();

    /* 4. Increment pointer. */

    p->offset += length;
    O;
    return FOTA_STATUS_OK;
}




