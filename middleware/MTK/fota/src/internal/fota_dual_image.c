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

#include "stdint.h"

#include "fota_dual_image.h"
#include "fota_download_interface.h"
#include "fota_config.h"
#include "fota_internal.h"

#include "hal_flash.h"

#ifdef BL_FOTA_DEBUG
#define LOGI(fmt, args...)               hw_uart_printf(fmt, ##args)
#define LOGW(fmt, args...)               hw_uart_printf(fmt, ##args)
#define LOGE(fmt, args...)               hw_uart_printf(fmt, ##args)

#define DEBUG_ASSERT(x)                  {if (!(x)) {hw_uart_printf("assert\n\r"); while (1); } }
#else
#include "syslog.h"
log_create_module(fota_dual, PRINT_LEVEL_INFO);

#define LOGI(fmt, args...)               LOG_I(fota_dual, "[FOTA DUAL] "fmt, ##args)
#define LOGW(fmt, args...)               LOG_W(fota_dual, "[FOTA DUAL] "fmt, ##args)
#define LOGE(fmt, args...)               LOG_E(fota_dual, "[FOTA DUAL] "fmt, ##args)

#define DEBUG_ASSERT(x)                  configASSERT(x)
#endif

#ifndef MOD_CFG_FOTA_DISABLE_OS
#include "httpclient.h"
#include "mbedtls/sha512.h"

#define IMAGE_A_FILE_POSTFIX        "_A"
#define IMAGE_B_FILE_POSTFIX        "_B"
#define IMAGE_N9_FILE_NAME            "image_n9"
#define IMAGE_CM4_FILE_NAME           "image_cm4"
#define HASH_CHECK_FILE_NAME          "hash_check"

#define URL_MAX_LEN                 (255)
#define FOTA_BUF_SIZE               ((1 << 12) + 1)
#define HASH_FILE_LEN               ((sizeof(fota_hash_check_t)) << 1)

#if defined(MTK_FOTA_DUAL_IMAGE_ONE_PACK)
#define FOTA_DUAL_HEADER_LEN        ((sizeof(fota_dual_pack_header_t)))
#define PATCH_ENTRY_LEN             ((sizeof(patch_entry_t)))
#define ENTRY_PER_TABLE (512)

typedef struct {
    uint32_t hash_offset;
    uint32_t diff_offset;
    uint32_t cm4_offset;
    uint32_t n9_offset;
}fota_dual_pack_header_t;

typedef enum {
    INVALID_PART = 0,
    DIFF_PART = 1,
    CM4_PART = 2,
    N9_PART = 3
}fota_dual_partition_t;


typedef struct {
    uint32_t        off;
    char            byte;
}__attribute__((packed)) patch_entry_t;

typedef struct _patch_table patch_table_t;
typedef struct _patch_table{
    uint32_t        count;
    patch_entry_t   entries[ENTRY_PER_TABLE];
    patch_table_t   *next;
}patch_table_t;

typedef struct{
    uint32_t count;
    char left_bytes[4];
}patch_left_bytes_t;

static uint32_t        _list_count;
static patch_table_t   *_list_head;
static patch_table_t   *_list_tail;
static patch_table_t   *_curr_table = NULL;
static uint32_t        _curr_table_index;

static fota_dual_pack_header_t _fota_dual_header = {0};
static patch_left_bytes_t _left_bytes = {0};
static bool _diff_parse_done = false;

static mbedtls_sha512_context _hash_ctx;

#endif /* MTK_FOTA_DUAL_IMAGE_ONE_PACK */

static httpclient_t _fota_dual_httpclient = {0};


static int32_t _fota_dual_image_dl_by_https(char * address,fota_image_type_t dst_image);
//static int32_t _fota_dual_http_retrieve_get(char * get_url,char * buf,uint32_t len,uint32_t partition,fota_hash_check_t * phash_calc);
static void _fota_dump_hash_code(char * hash_dl,char * hash_calc);
#endif

#ifdef MTK_HTTPCLIENT_SSL_ENABLE
static const char _svr_ca_crt_rsa_test[] =                                \
"-----BEGIN CERTIFICATE-----\r\n"                                       \
"MIIDhzCCAm+gAwIBAgIBADANBgkqhkiG9w0BAQUFADA7MQswCQYDVQQGEwJOTDER\r\n"  \
"MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\r\n"  \
"MTEwMjEyMTQ0NDAwWhcNMjEwMjEyMTQ0NDAwWjA7MQswCQYDVQQGEwJOTDERMA8G\r\n"  \
"A1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwggEiMA0G\r\n"  \
"CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDA3zf8F7vglp0/ht6WMn1EpRagzSHx\r\n"  \
"mdTs6st8GFgIlKXsm8WL3xoemTiZhx57wI053zhdcHgH057Zk+i5clHFzqMwUqny\r\n"  \
"50BwFMtEonILwuVA+T7lpg6z+exKY8C4KQB0nFc7qKUEkHHxvYPZP9al4jwqj+8n\r\n"  \
"YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\r\n"  \
"R7EUQIN9Z/SG9jGr8XmksrUuEvmEF/Bibyc+E1ixVA0hmnM3oTDPb5Lc9un8rNsu\r\n"  \
"KNF+AksjoBXyOGVkCeoMbo4bF6BxyLObyavpw/LPh5aPgAIynplYb6LVAgMBAAGj\r\n"  \
"gZUwgZIwDAYDVR0TBAUwAwEB/zAdBgNVHQ4EFgQUtFrkpbPe0lL2udWmlQ/rPrzH\r\n"  \
"/f8wYwYDVR0jBFwwWoAUtFrkpbPe0lL2udWmlQ/rPrzH/f+hP6Q9MDsxCzAJBgNV\r\n"  \
"BAYTAk5MMREwDwYDVQQKEwhQb2xhclNTTDEZMBcGA1UEAxMQUG9sYXJTU0wgVGVz\r\n"  \
"dCBDQYIBADANBgkqhkiG9w0BAQUFAAOCAQEAuP1U2ABUkIslsCfdlc2i94QHHYeJ\r\n"  \
"SsR4EdgHtdciUI5I62J6Mom+Y0dT/7a+8S6MVMCZP6C5NyNyXw1GWY/YR82XTJ8H\r\n"  \
"DBJiCTok5DbZ6SzaONBzdWHXwWwmi5vg1dxn7YxrM9d0IjxM27WNKs4sDQhZBQkF\r\n"  \
"pjmfs2cb4oPl4Y9T9meTx/lvdkRYEug61Jfn6cA+qHpyPYdTH+UshITnmp5/Ztkf\r\n"  \
"m/UTSLBNFNHesiTZeH31NcxYGdHSme9Nc/gfidRa0FLOCfWxRlFqAI47zG9jAQCZ\r\n"  \
"7Z2mCGDNMhjQc+BYcdnl0lPXjdDK6V0qCg1dVewhUBcW5gZKzV7e9+DpVA==\r\n"      \
"-----END CERTIFICATE-----\r\n";
#endif

static void _fota_dual_image_control_block_init(void)
{
    fota_control_block_t ctrl_blk;
    hal_flash_status_t err;

    ctrl_blk.magic_num = FOTA_CONTROL_BLOCK_MAGIC;
    ctrl_blk.active_flag = FOTA_IMAGE_A_MARK;

    do {
        err = hal_flash_erase(FOTA_CONTROL_BLOCK_ADDR, HAL_FLASH_BLOCK_4K);
        if (err < 0) {
            LOGE("flash erase error, err = %d\r\n", err);
            break;
        }

        hal_flash_write(FOTA_CONTROL_BLOCK_ADDR, (uint8_t*)&ctrl_blk, sizeof(fota_control_block_t));
        if (err < 0) {
            LOGE("flash write error, err = %d\r\n", err);
            break;
        }

        LOGI("succeed to init control block info.\r\n");
        return;
    } while (0);

    /* fail to init fota control block info */
    DEBUG_ASSERT(0);
}


/* supposed only for bootloader used */
fota_status_t fota_dual_image_init(void)
{
    fota_control_block_t ctrl_blk;
    hal_flash_status_t err;
    
    err = hal_flash_read(FOTA_CONTROL_BLOCK_ADDR, (uint8_t*)&ctrl_blk, sizeof(fota_control_block_t));
    if (err < 0) {
        LOGE("flash read error, err = %d\r\n", err);
        return FOTA_STATUS_ERROR_FLASH_OP;
    }

    if (FOTA_CONTROL_BLOCK_MAGIC == ctrl_blk.magic_num) {
        LOGI("has been initialized before.\r\n");
        return FOTA_STATUS_OK;
    } else {
        /* supposed here is the first bootup */
        _fota_dual_image_control_block_init();
    }
   
    return FOTA_STATUS_OK;
}




fota_status_t fota_switch_active_image (void)
{
    fota_control_block_t ctrl_blk;
    hal_flash_status_t err;
    err = hal_flash_read(FOTA_CONTROL_BLOCK_ADDR, (uint8_t*)&ctrl_blk, sizeof(fota_control_block_t));
    if (err < 0) {
        LOGE("flash read error, err = %d\r\n", err);
        return FOTA_STATUS_ERROR_FLASH_OP;
    }

    if (FOTA_IMAGE_A_MARK == ctrl_blk.active_flag) {
        ctrl_blk.active_flag = FOTA_IMAGE_B_MARK;
        LOGI("switch to image B.\r\n");
    } else if (FOTA_IMAGE_B_MARK == ctrl_blk.active_flag) {
        ctrl_blk.active_flag = FOTA_IMAGE_A_MARK;
        LOGI("switch to image A.\r\n");
    } else {
        LOGE("fota control block corruption\r\n");
        return FOTA_STATUS_ERROR_CONTROL_BLOCK_CORRUPTION;
    }

    err = hal_flash_erase(FOTA_CONTROL_BLOCK_ADDR, HAL_FLASH_BLOCK_4K);
    if (err < 0) {
        LOGE("flash erase error, err = %d\r\n", err);
        return FOTA_STATUS_ERROR_FLASH_OP;
    }

    hal_flash_write(FOTA_CONTROL_BLOCK_ADDR, (uint8_t*)&ctrl_blk, sizeof(fota_control_block_t));
    if (err < 0) {
        LOGE("flash write error, err = %d\r\n", err);
        return FOTA_STATUS_ERROR_FLASH_OP;
    }

    LOGI("switch active image success.\r\n");
    return FOTA_STATUS_OK;

}


fota_status_t fota_query_active_image(fota_image_type_t *type)
{
    fota_control_block_t ctrl_blk;
    hal_flash_status_t err;
    err = hal_flash_read(FOTA_CONTROL_BLOCK_ADDR, (uint8_t*)&ctrl_blk, sizeof(fota_control_block_t));
    if (err < 0) {
        LOGE("flash read error, err = %d\r\n", err);
        return FOTA_STATUS_ERROR_FLASH_OP;
    }

    if (FOTA_IMAGE_A_MARK == ctrl_blk.active_flag) {
        *type = FOTA_DUAL_IMAGE_A;
    } else if (FOTA_IMAGE_B_MARK == ctrl_blk.active_flag) {
        *type = FOTA_DUAL_IMAGE_B;
    } else {
        LOGE("fota control block corruption\r\n");
        return FOTA_STATUS_ERROR_CONTROL_BLOCK_CORRUPTION;
    }

    LOGI("current active image is  = %d\r\n", (int32_t)*type);
    return FOTA_STATUS_OK;

}


#ifndef MOD_CFG_FOTA_DISABLE_OS
fota_status_t fota_dual_image_download(char* address)
{
    fota_image_type_t curr_image;
    fota_image_type_t dst_image = FOTA_DUAL_IMAGE_A;
    int32_t ret;

    fota_query_active_image(&curr_image);

    if (FOTA_DUAL_IMAGE_A == curr_image) {
        dst_image = FOTA_DUAL_IMAGE_B;
    } else if (FOTA_DUAL_IMAGE_B == curr_image) {
        dst_image = FOTA_DUAL_IMAGE_A;
    } else {
        LOGE("control block may be corrupted.\r\n");
        configASSERT(0);
    }

    LOGI("download address: %s", address);
    ret = _fota_dual_image_dl_by_https(address, dst_image);
    if (ret) {
        LOGE("download fail, ret = %d", ret);
        return FOTA_STATUS_DOWNLOAD_FAIL;
    } else {
        return FOTA_STATUS_OK;
    }
    
}


#if !defined(MTK_FOTA_DUAL_IMAGE_ONE_PACK)
static int32_t _fota_dual_http_retrieve_get(char* get_url, char* buf, uint32_t len, uint32_t partition, fota_hash_check_t *phash_calc)
{
    int32_t ret = HTTPCLIENT_ERROR_CONN;
    fota_status_t write_ret = FOTA_STATUS_OK;
    httpclient_data_t client_data = {0};
    uint32_t count = 0;
    uint32_t recv_temp = -1;
    uint32_t data_len = 0;
    mbedtls_sha512_context ctx;
    int32_t ret_val = 0;
    char *temp = NULL;
    uint32_t hash_to_be_written = 0;

    if (partition) {
        /* for n9 or cm4 */
        client_data.response_buf = buf;
        client_data.response_buf_len = len;
        fota_seek(partition, 0);
    } else {
        /* for hash file */  
        temp = pvPortMalloc(1025);
        if (!temp) {
            LOGE("malloc temp buffer failed.\r\n");
            return -1;
        }
        client_data.response_buf = temp;
        client_data.response_buf_len = 1025;
        hash_to_be_written = HASH_FILE_LEN;
    }

    ret = httpclient_send_request(&_fota_dual_httpclient, get_url, HTTPCLIENT_GET, &client_data);
    if (ret < 0) {
        
        LOGE("http client fail to send request \r\n");
        return ret;
    }

    mbedtls_sha512_init(&ctx);
    mbedtls_sha512_starts(&ctx, 0);
    do {
        ret = httpclient_recv_response(&_fota_dual_httpclient, &client_data);
        if (ret < 0) {
            LOGE("http client recv response error, ret = %d \r\n", ret);
            return ret;
        }

        if (recv_temp == -1)
        {
            recv_temp = client_data.response_content_len;
        }

        LOGI("to be retrieved len = %d \n", client_data.retrieve_len);
        
        data_len = recv_temp - client_data.retrieve_len;
        LOGI("current pack data len = %u \n", data_len);
        
        count += data_len;
        recv_temp = client_data.retrieve_len;
        
        //vTaskDelay(100);/* Print log may block other task, so sleep some ticks */
        LOGI("total data received: %u , ret: %d \n", count, ret);

        if (partition) {
            if (data_len > 0) {
                mbedtls_sha512_update(&ctx, (const unsigned char*)client_data.response_buf, data_len);
                write_ret = fota_write(partition, (const uint8_t*)client_data.response_buf, data_len);
                if (FOTA_STATUS_OK != write_ret) {
                    LOGE("fail to write flash, write_ret = %d \n", write_ret);
                    break;
                }
                LOGI("download progrses = %u \r\n", count * 100 / client_data.response_content_len);
            }
        } else {
            LOGI("download hash file, hash_to_be_written: %u\r\n", (unsigned int)hash_to_be_written);
            if (hash_to_be_written > data_len) {
                memcpy(buf, client_data.response_buf, data_len);
                hash_to_be_written -= data_len;                
                buf += data_len;
            } else {
                if (hash_to_be_written > 0) {
                    memcpy(buf, client_data.response_buf, hash_to_be_written);
                    hash_to_be_written = 0;
                }
            }
        }
        
    } while (ret == HTTPCLIENT_RETRIEVE_MORE_DATA);

    if (write_ret != FOTA_STATUS_OK) {
        ret_val = write_ret;
    }
    else if (count != client_data.response_content_len || httpclient_get_response_code(&_fota_dual_httpclient) != 200) {
        LOGE("data received not completed, or invalid error code \r\n");
        ret_val = -2;
    }
    else if (count == 0) {
        LOGE("receive length is zero, file not found \n");
        ret_val = -3;
    }
    else {
        LOGI("download success \n");
        if (phash_calc) {
            LOGI("partition: %d , n9 addr: 0x%x , cm4_addr: 0x%x\r\n", partition, phash_calc->SHA512_hash_n9, phash_calc->SHA512_hash_cm4);
            mbedtls_sha512_finish(&ctx, partition%2 ? phash_calc->SHA512_hash_n9 : phash_calc->SHA512_hash_cm4);
        }
        ret_val = ret;
    }
        
    mbedtls_sha512_free(&ctx);
    if(temp) {
        vPortFree(temp);
    }

    return ret_val;
}


static int32_t _fota_dual_image_dl_by_https(char* address, fota_image_type_t dst_image)
{
    char *postfix = NULL;
    uint32_t n9_partition, cm4_partition;
    int32_t ret = HTTPCLIENT_ERROR_CONN;
    int32_t ret_val = 0;
    uint32_t len = strlen(address);
    if (len < 1) {
        return -1;
    }
    
    LOGI("url length: %d\n", len);
    
    switch(dst_image) {
        case FOTA_DUAL_IMAGE_A:
            postfix = IMAGE_A_FILE_POSTFIX;
            n9_partition = FOTA_PARTITION_N9_A;
            cm4_partition = FOTA_PARTITION_CM4_A;
            break;
        case FOTA_DUAL_IMAGE_B:
            postfix = IMAGE_B_FILE_POSTFIX;
            n9_partition = FOTA_PARTITION_N9_B;
            cm4_partition = FOTA_PARTITION_CM4_B;
            break;
        default:
            LOGE("invalide dst image.\r\n");
            return -2;
    }

    if (fota_init(&fota_flash_default_config) != FOTA_STATUS_OK) {
        LOGE("fota init fail. \n");
        return -3;
    }
    
    char *get_url = pvPortMalloc(URL_MAX_LEN);
    char* buf = pvPortMalloc(FOTA_BUF_SIZE);
    fota_hash_check_t *phash_dl = pvPortMalloc(HASH_FILE_LEN);
    fota_hash_check_t *phash_calc = pvPortMalloc(sizeof(fota_hash_check_t));
    LOGI("phash_dl: 0x%x , phash_calc: 0x%x", phash_dl, phash_calc);
    if (buf == NULL || get_url == NULL || phash_dl == NULL || phash_calc == NULL) {
        LOGE("malloc failed.\r\n");
        return -4;
    }
    
    memset(get_url, 0, URL_MAX_LEN);
#ifdef MTK_HTTPCLIENT_SSL_ENABLE
    _fota_dual_httpclient.server_cert = _svr_ca_crt_rsa_test;
    _fota_dual_httpclient.server_cert_len = sizeof(_svr_ca_crt_rsa_test);
    LOGI("cert len: %d\r\n", _fota_dual_httpclient.server_cert_len);
#endif

    do {
        ret = httpclient_connect(&_fota_dual_httpclient, address);

        if (ret) {
            LOGE("http client connect error. \r");
            ret_val = ret;
            break;
        }

        /* download hash file */
        snprintf(get_url, URL_MAX_LEN, "%s/%s", address, HASH_CHECK_FILE_NAME);
        LOGI("hash url = %s", get_url);
        ret = _fota_dual_http_retrieve_get(get_url, (char*)phash_dl, HASH_FILE_LEN + 1, 0, NULL);      
        httpclient_close(&_fota_dual_httpclient);
        LOGI("download hash file result = %d \r\n", (int)ret);
        if (ret) {
            ret_val = ret;
            break;
        } 

        

        /* download n9 file */
        ret = httpclient_connect(&_fota_dual_httpclient, address);
        if (ret) {
            LOGE("http client connect error. \r");
            ret_val = ret;
            break;
        }
        snprintf(get_url, URL_MAX_LEN, "%s/%s%s", address, IMAGE_N9_FILE_NAME, postfix);
        LOGI("n9 url = %s", get_url);
        ret = _fota_dual_http_retrieve_get(get_url, buf, FOTA_BUF_SIZE, n9_partition, phash_calc);
        LOGI("download n9 file result = %d \r\n", (int)ret);
        httpclient_close(&_fota_dual_httpclient);
        if (ret) {
            ret_val = ret;
            break;
        }

        /* download cm4 file */
        ret = httpclient_connect(&_fota_dual_httpclient, address);
        if (ret) {
            LOGE("http client connect error. \r");
            ret_val = ret;
            break;
        }
        snprintf(get_url, URL_MAX_LEN, "%s/%s%s", address, IMAGE_CM4_FILE_NAME, postfix);
        LOGI("cm4 url = %s", get_url);
        ret = _fota_dual_http_retrieve_get(get_url, buf, FOTA_BUF_SIZE, cm4_partition, phash_calc);     
        LOGI("download cm4 file result = %d \r\n", (int)ret);
        httpclient_close(&_fota_dual_httpclient);
        if (ret) {
            ret_val = ret;
            break;
        }
        
        /* check hash code */
        char *p_dl = (char*)((phash_dl + (uint32_t)dst_image)->SHA512_hash_n9);
        char *p_calc =  (char*)(phash_calc->SHA512_hash_n9);
        LOGI("p_dl: 0x%x , p_calc: 0x%x\r\n", p_dl, p_calc);
        _fota_dump_hash_code(p_dl, p_calc);
        if (strncmp(p_dl, p_calc, 64)) {
            LOGE("n9 sha512 hash code not match! \r\n");
           ret_val = -5;
            break;
        }

        p_dl = (char*)((phash_dl + (uint32_t)dst_image)->SHA512_hash_cm4);
        p_calc =  (char*)(phash_calc->SHA512_hash_cm4);
        LOGI("p_dl: 0x%x , p_calc: 0x%x\r\n", p_dl, p_calc);
        _fota_dump_hash_code(p_dl, p_calc);
        if (strncmp(p_dl, p_calc, 64)) {
            LOGE("cm4 sha512 hash code not match! \r\n");
            ret_val = -6;
            break;
        }
        
    }while(0);

    LOGI("download process final.");

    vPortFree(buf);
    vPortFree(phash_calc);
    vPortFree(phash_dl);
    vPortFree(get_url);
    buf = NULL;
    phash_calc = NULL;
    phash_dl = NULL;
    get_url = NULL;
    
    return ret_val;

}

#else

static void _parse_header(const char *data_ptr)
{
    /* big end translate to little end */
    int i;
    uint32_t offset;
    uint32_t *header = (uint32_t*)&_fota_dual_header;
    char *p = (char*)data_ptr;
    for (i = 0; i < 4; i++) {
        offset = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
        *header++ = offset;
        p += 4;
    }

    LOGI("hash_off: 0x%.8x, diff_off: 0x%.8x, cm4_off: 0x%.8x, n9_off: 0x%.8x\r\n",
            _fota_dual_header.hash_offset,
            _fota_dual_header.diff_offset,
            _fota_dual_header.cm4_offset,
            _fota_dual_header.n9_offset
            );
}

static void _parse_hash(char *hash_dl, const char *p)
{
    memcpy(hash_dl, p, HASH_FILE_LEN);
}


static int32_t _patch_add(uint32_t off, char byte)
{   
    if (!_list_head) {        
        _list_head = _list_tail = pvPortMalloc(sizeof(patch_table_t));
        if (_list_head == NULL) {
            LOGE("failed to allocate heap for patchr\r\n");
            return -1;
        }
        memset(_list_head, 0, sizeof(patch_table_t));
        _list_count++;
    }else if (_list_tail->count == ENTRY_PER_TABLE) {
        _list_tail->next = pvPortMalloc(sizeof(patch_table_t));
        if (_list_tail->next == NULL) {
            LOGE("failed to allocate heap for patchr\r\n");
            return -1;
        }
        memset(_list_tail->next, 0, sizeof(patch_table_t));
        _list_tail = _list_tail->next;
        _list_count++;
    }
    _list_tail->entries[_list_tail->count  ].off  = off;
    _list_tail->entries[_list_tail->count++].byte = byte;

    return 0;
}

static void _patch_apply(uint32_t offset, char *data_ptr, uint32_t data_len)
{
    char *p = data_ptr;
    int32_t i,j,k;
    patch_table_t *tbl;
    if (!_curr_table) {
        _curr_table = _list_head;
    }

    tbl = _curr_table;
    k = _curr_table_index;

    //LOGI("offset: 0x%.8x\r\n", offset);
    for (i = 0, j = offset; i < data_len; i++, j++) {
        if (tbl && k == tbl->count) {
            tbl = tbl->next;
            k = 0;
        }

        if (tbl && tbl->entries[k].off == j) {
            //LOGI("different bytes found, k: %d, j: 0x%.8x, value: 0x%.2x\r\n", k, j, tbl->entries[k].byte);
            *(p + i) = tbl->entries[k++].byte;
        }
    }

    _curr_table = tbl;
    _curr_table_index = k;
}


static void _patch_free_buf(patch_table_t *head)
{
    if (head) {
        if (head->next) {
            _patch_free_buf(head->next);
        }
        vPortFree(head);
    }
}

static int32_t _get_target_partition(fota_dual_partition_t *part, uint32_t index, int32_t length, bool *is_part_end)
{
    LOGI("index: 0x%x\r\n", (unsigned int)index);
    if (index >= _fota_dual_header.diff_offset && index < _fota_dual_header.cm4_offset) {
        if (index + length >= _fota_dual_header.cm4_offset) {
            length = _fota_dual_header.cm4_offset - index;
            *is_part_end = true;
        }
        *part = DIFF_PART;
        return length;
    } else if (index >= _fota_dual_header.cm4_offset && index < _fota_dual_header.n9_offset) {
        if (index + length >= _fota_dual_header.n9_offset) {
            length = _fota_dual_header.n9_offset - index;
            *is_part_end = true;
        }
        *part = CM4_PART;
        return length;
    } else if (index >= _fota_dual_header.n9_offset) {
        *part = N9_PART;
        return length;
    } else {
        LOGE("Invalide index\r\n");
        return -1;
    }
}


static int32_t _fota_dual_data_packet_hdlr(fota_image_type_t type, fota_dual_partition_t part, uint32_t index, char *data_ptr, uint32_t data_len)
{
    char *p = NULL;
    patch_entry_t left_entry;
    patch_entry_t *tmp;
    int32_t left;
    int32_t gap = 0;
    int32_t i,j;   
    uint32_t n9_partition, cm4_partition;
    fota_status_t ret;

    switch(type) {
        case FOTA_DUAL_IMAGE_A:
            n9_partition = FOTA_PARTITION_N9_A;
            cm4_partition = FOTA_PARTITION_CM4_A;
            break;
        case FOTA_DUAL_IMAGE_B:
            n9_partition = FOTA_PARTITION_N9_B;
            cm4_partition = FOTA_PARTITION_CM4_B;
            break;
        default:
            LOGE("invalide dst image.\r\n");
            return -1;
    }
    
    switch(part)
    {
        case DIFF_PART:
            if (_left_bytes.count > 0) {
                gap = sizeof(patch_entry_t) - _left_bytes.count;
                
                if (gap > data_len) {
                    LOGI("should meet the end mark\r\n");
                    p = _left_bytes.left_bytes;
                    memcpy(p+_left_bytes.count, data_ptr, data_len);
                    gap = data_len;
                } else {
                    LOGI("construct patch with the previous legacy bytes\r\n");
                    p = (char*)&left_entry;
                    memcpy(p, _left_bytes.left_bytes, _left_bytes.count);
                    memcpy(p+_left_bytes.count, data_ptr, gap);
                    _patch_add(left_entry.off, left_entry.byte);
                }
            }

            data_len -= gap;
            p = data_ptr + gap;
            for (i = 0, j = sizeof(patch_entry_t)-1; j < data_len; i += sizeof(patch_entry_t), j += sizeof(patch_entry_t)) {
                tmp = (patch_entry_t*)(p + i);
                _patch_add(tmp->off, tmp->byte);
            }

            //LOGI("data_len: %u, i: %d, j: %d, gap: %d\r\n", data_len, i, j, gap);
            //LOGI("data_ptr: 0x%.8x, p: 0x%.8x\r\n", data_ptr, p);
            left = data_len - i;
            if (left > 4 || left < 0) {
                LOGE("Wrong data, left: %d\r\n", left);
                return -2;
            } else {
                LOGI("p+i: 0x%.8x\r\n", (char*)(p+i));
                memcpy(_left_bytes.left_bytes, (char*)(p+i), left);
                _left_bytes.count = left;
            }

            if (_left_bytes.count == 4) {
                LOGI("_left_bytes value: 0x%.8x", (*(uint32_t*)&_left_bytes.left_bytes[0]));
                if ((*(uint32_t*)&_left_bytes.left_bytes[0]) == 0xffffffff) {
                    LOGI("end mark found\r\n");
                    _diff_parse_done = true;
                }
            }

            LOGI("left bytes count: %d\r\n", _left_bytes.count);
            break;
        case CM4_PART:
            if (!_diff_parse_done) {
                return -3;
            }

            if (type == FOTA_DUAL_IMAGE_B) {
                _patch_apply(index - _fota_dual_header.cm4_offset, data_ptr, data_len);    
            }

            mbedtls_sha512_update(&_hash_ctx, (const unsigned char*)data_ptr, data_len);
            ret = fota_write(cm4_partition, (const uint8_t*)data_ptr, data_len);
            if (ret != FOTA_STATUS_OK) {
                LOGE("Failed to write CM4 to flash, ret: %d\r\n", ret);
                return -4;
            }

            break;
        case N9_PART:
            if (!_diff_parse_done) {
                return -5;
            }

            mbedtls_sha512_update(&_hash_ctx, (const unsigned char*)data_ptr, data_len);
            ret = fota_write(n9_partition, (const uint8_t*)data_ptr, data_len);
            if (ret != FOTA_STATUS_OK) {
                LOGE("Failed to write N9 to flash, ret: %d\r\n", ret);
                return -6;
            }
            break;
        default:
            LOGE("Invalid part, part: %d\r\n", part);
            return -7;
    }

    return 0;
}


static int32_t _fota_dual_http_retrieve_get(char* get_url, char* buf, uint32_t len, fota_image_type_t type, fota_hash_check_t *phash_dl, fota_hash_check_t *phash_calc)
{
    int32_t ret = HTTPCLIENT_ERROR_CONN;
    fota_status_t write_ret = FOTA_STATUS_OK;
    httpclient_data_t client_data = {0};
    uint32_t count = 0;
    uint32_t recv_temp = -1;
    uint32_t packet_data_len = 0;
    int32_t left_data_len = 0;
    int32_t real_data_len = 0;
    bool is_first_packet = false;
    bool is_part_end = false;
    //mbedtls_sha512_context ctx;
    int32_t ret_val = 0;
    char *data_ptr = NULL;
    uint32_t index = 0;
    fota_dual_partition_t part;

    /* for n9 or cm4 */
    client_data.response_buf = buf;
    client_data.response_buf_len = len;
    //fota_seek(partition, 0);

    ret = httpclient_send_request(&_fota_dual_httpclient, get_url, HTTPCLIENT_GET, &client_data);
    if (ret < 0) {
        
        LOGE("http client fail to send request \r\n");
        return ret;
    }

    mbedtls_sha512_init(&_hash_ctx);
    mbedtls_sha512_starts(&_hash_ctx, 0);
    do {
        ret = httpclient_recv_response(&_fota_dual_httpclient, &client_data);
        if (ret < 0) {
            LOGE("http client recv response error, ret = %d \r\n", ret);
            return ret;
        }

        if (recv_temp == -1)
        {
            /* the first received packet */
            recv_temp = client_data.response_content_len;
            is_first_packet = true;
            _diff_parse_done = false;
        }

        LOGI("to be retrieved len = %d \n", client_data.retrieve_len);
        
        packet_data_len = recv_temp - client_data.retrieve_len;
        LOGI("current pack data len = %u \n", packet_data_len);
        
        count += packet_data_len;
        recv_temp = client_data.retrieve_len;
        
        //vTaskDelay(100);/* Print log may block other task, so sleep some ticks */
        LOGI("total data received: %u , ret: %d \n", count, ret);

        data_ptr = client_data.response_buf;
        left_data_len = packet_data_len;
        
        if (is_first_packet) {
            if (packet_data_len < HASH_FILE_LEN + FOTA_DUAL_HEADER_LEN) {
                LOGE("the first packet isn't enough to parse\r\n");
                return -1;
            }

            LOGI("handle the first packet\r\n");

            _parse_header(data_ptr);
            data_ptr += FOTA_DUAL_HEADER_LEN;
            index += FOTA_DUAL_HEADER_LEN;
            _parse_hash((char*)phash_dl, data_ptr);
            data_ptr += HASH_FILE_LEN;
            index += HASH_FILE_LEN;
            left_data_len = packet_data_len - FOTA_DUAL_HEADER_LEN - HASH_FILE_LEN;

            is_first_packet = false;
        }
        
        do {
            is_part_end = false;
            part = INVALID_PART;
            real_data_len = _get_target_partition(&part, index, left_data_len, &is_part_end);
            LOGI("data packet hldr before, part: %d, index: %u, real_data_len: %u\r\n", part, index, real_data_len);

            ret_val = _fota_dual_data_packet_hdlr(type, part, index, data_ptr, real_data_len);
            if (ret_val) {
                LOGE("failed to handle data packet, ret_val: %d\r\n", ret_val);
                return -1;
            }

            if (is_part_end && part == CM4_PART) {
                LOGI("finalize CM4 hash code\r\n");
                mbedtls_sha512_finish(&_hash_ctx, phash_calc->SHA512_hash_cm4);
                mbedtls_sha512_starts(&_hash_ctx, 0);
            }

            index += real_data_len;
            data_ptr += real_data_len;
            left_data_len -= real_data_len;
            LOGI("left_data_len: %u\r\n", left_data_len);
        }while(left_data_len > 0);
    
        
        
    } while (ret == HTTPCLIENT_RETRIEVE_MORE_DATA);

    if (write_ret != FOTA_STATUS_OK) {
        ret_val = write_ret;
    }
    else if (count != client_data.response_content_len || httpclient_get_response_code(&_fota_dual_httpclient) != 200) {
        LOGE("data received not completed, or invalid error code\r\n");
        ret_val = -2;
    }
    else if (count == 0) {
        LOGE("receive length is zero, file not found\n");
        ret_val = -3;
    }
    else {
        LOGI("download success\r\n");
        if (phash_calc) {
            LOGI("finalize N9 hash code\r\n");
            mbedtls_sha512_finish(&_hash_ctx, phash_calc->SHA512_hash_n9);
        }

        _patch_free_buf(_list_head);
        _list_head = NULL;
        _list_tail = NULL;
        _curr_table = NULL;
        _curr_table_index = 0;
        ret_val = ret;
    }
        
    mbedtls_sha512_free(&_hash_ctx);

    return ret_val;
}


static int32_t _fota_dual_image_dl_by_https(char* address, fota_image_type_t dst_image)
{
    int32_t ret = HTTPCLIENT_ERROR_CONN;
    int32_t ret_val = 0;
    uint32_t len = strlen(address);
    if (len < 1) {
        return -1;
    }

    LOGI("url: %s\r\n", address);
    LOGI("url length: %d\r\n", len);
    
    if (fota_init(&fota_flash_default_config) != FOTA_STATUS_OK) {
        LOGE("fota init fail. \n");
        return -3;
    }
    
    char* buf = pvPortMalloc(FOTA_BUF_SIZE);
    fota_hash_check_t *phash_dl = pvPortMalloc(HASH_FILE_LEN);
    fota_hash_check_t *phash_calc = pvPortMalloc(sizeof(fota_hash_check_t));
    LOGI("phash_dl: 0x%x , phash_calc: 0x%x", phash_dl, phash_calc);
    if (buf == NULL || phash_dl == NULL || phash_calc == NULL) {
        LOGE("malloc failed.\r\n");
        return -4;
    }
    
#ifdef MTK_HTTPCLIENT_SSL_ENABLE
    _fota_dual_httpclient.server_cert = _svr_ca_crt_rsa_test;
    _fota_dual_httpclient.server_cert_len = sizeof(_svr_ca_crt_rsa_test);
    LOGI("cert len: %d\r\n", _fota_dual_httpclient.server_cert_len);
#endif

    do {    

        /* download the whole package file */
        ret = httpclient_connect(&_fota_dual_httpclient, address);
        if (ret) {
            LOGE("http client connect error. \r");
            ret_val = ret;
            break;
        }
        ret = _fota_dual_http_retrieve_get(address, buf, FOTA_BUF_SIZE, dst_image, phash_dl, phash_calc);
        LOGI("download n9 file result = %d \r\n", (int)ret);
        httpclient_close(&_fota_dual_httpclient);
        if (ret) {
            ret_val = ret;
            break;
        }
        
        /* check hash code */
        char *p_dl = (char*)((phash_dl + (uint32_t)dst_image)->SHA512_hash_n9);
        char *p_calc =  (char*)(phash_calc->SHA512_hash_n9);
        LOGI("p_dl: 0x%x , p_calc: 0x%x\r\n", p_dl, p_calc);
        _fota_dump_hash_code(p_dl, p_calc);
        if (strncmp(p_dl, p_calc, 64)) {
            LOGE("n9 sha512 hash code not match! \r\n");
           ret_val = -5;
            break;
        }

        p_dl = (char*)((phash_dl + (uint32_t)dst_image)->SHA512_hash_cm4);
        p_calc =  (char*)(phash_calc->SHA512_hash_cm4);
        LOGI("p_dl: 0x%x , p_calc: 0x%x\r\n", p_dl, p_calc);
        _fota_dump_hash_code(p_dl, p_calc);
        if (strncmp(p_dl, p_calc, 64)) {
            LOGE("cm4 sha512 hash code not match! \r\n");
            ret_val = -6;
            break;
        }
        
    }while(0);

    LOGI("download process final.");

    vPortFree(buf);
    vPortFree(phash_calc);
    vPortFree(phash_dl);
    buf = NULL;
    phash_calc = NULL;
    phash_dl = NULL;
    
    return ret_val;

}

#endif /* MTK_FOTA_DUAL_IMAGE_ONE_PACK */


static void _fota_dump_hash_code(char *hash_dl, char *hash_calc)
{
    int32_t i;
    printf("==========================================\r\n");
    if (hash_dl) {        
        printf("[FOTA DUAL] print hash dl:\r\n");
        for (i = 0; i < 64 ; i++) {
            printf("0x%.2x%s", hash_dl[i], (i + 1)%8 ? " " : "\r\n");
        }
    }

    if (hash_calc) {
        printf("[FOTA_DUAL] print hash calc:\r\n");
        for (i = 0; i < 64 ; i++) {
            printf("0x%.2x%s", hash_calc[i], (i + 1)%8 ? " " : "\r\n");
        }
    }
    printf("==========================================\r\n");
}
#endif /* MOD_CFG_FOTA_DISABLE_OS */

