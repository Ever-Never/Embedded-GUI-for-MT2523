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

#include "hal_aes.h"

#ifdef HAL_AES_MODULE_ENABLED

#include <string.h>
#include "hal_define.h"
#include "hal_aes_internal.h"
#include "hal_crypt_internal.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/* global lock used to protect the crypto engine */
bool g_crypt_lock = CRYPT_UNLOCK;

/* internal function for getting lock, -100 means the crypto engine is not available */
int32_t hal_crypt_lock_take(void)
{
    uint32_t irq_status;
    irq_status = save_and_set_interrupt_mask();
    if (g_crypt_lock == CRYPT_LOCK) {
        restore_interrupt_mask(irq_status);
        return -100;
    }
    g_crypt_lock = CRYPT_LOCK;
    restore_interrupt_mask(irq_status);
    return 0;
}

/* internal function for releasing lock */
void hal_crypt_lock_give(void)
{
    g_crypt_lock = CRYPT_UNLOCK;
}

hal_aes_status_t aes_operate(aes_parcel_t *parcel, bool mode, bool sw_hw_key, bool encrypt)
{
    if (HAL_AES_STATUS_ERROR == (hal_aes_status_t) AES_HW_Init(parcel, mode, 0)) {
        return HAL_AES_STATUS_ERROR;
    }

    if (true == encrypt) {  /* do encryption */
        if (HAL_AES_STATUS_ERROR == (hal_aes_status_t) AES_HW_Encrypt()) {
            return HAL_AES_STATUS_ERROR;
        }
    } else {  /* do decryption */
        if (HAL_AES_STATUS_ERROR == (hal_aes_status_t) AES_HW_Decrypt()) {
            return HAL_AES_STATUS_ERROR;
        }
    }

    if (HAL_AES_STATUS_ERROR == (hal_aes_status_t) AES_HW_DeInit()) {
        return HAL_AES_STATUS_ERROR;
    }

    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_cbc_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    if ((NULL == encrypted_text)
            || (NULL == plain_text)
            || (NULL == key)
            || (NULL == init_vector)) {
        return HAL_AES_STATUS_ERROR;
    }

    hal_aes_buffer_t aes_buf;
    hal_aes_buffer_t aes_buf_out;
    uint32_t last_block_size = plain_text->length % HAL_AES_BLOCK_SIZES;
    uint32_t block_num = plain_text->length / HAL_AES_BLOCK_SIZES;
    uint8_t padding_size = HAL_AES_BLOCK_SIZES - last_block_size;

    /* padding checking */
    if (encrypted_text->length < (plain_text->length + padding_size)) {
        log_hal_error("Inadequate encrypted buffer.");
        return HAL_AES_STATUS_ERROR;
    }

    hal_crypt_lock_take();

    aes_parcel_t parcel;

    if (block_num > 0) {
        /* block-alignment part */
        uint32_t first_encrypt_size = block_num * HAL_AES_BLOCK_SIZES;

        aes_buf.buffer = plain_text->buffer;
        aes_buf.length = first_encrypt_size;

        aes_buf_out.buffer = encrypted_text->buffer;
        aes_buf_out.length = first_encrypt_size;

        parcel.in = &aes_buf;
        parcel.out = &aes_buf_out;
        parcel.key = key;
        parcel.iv = init_vector;

        if (HAL_AES_STATUS_ERROR == aes_operate(&parcel, AES_CBC, 0, true)) {
            hal_crypt_lock_give();
            return HAL_AES_STATUS_ERROR;
        }

        /* non block-alignment part */
        uint8_t data_block[HAL_AES_BLOCK_SIZES] = {0};
        memcpy(data_block, plain_text->buffer + first_encrypt_size, last_block_size);
        memset(data_block + last_block_size, padding_size, padding_size);
        uint8_t *init_vector2 = encrypted_text->buffer + first_encrypt_size - HAL_AES_BLOCK_SIZES;

        aes_buf.buffer = data_block;
        aes_buf.length = HAL_AES_BLOCK_SIZES;

        aes_buf_out.buffer = encrypted_text->buffer + first_encrypt_size;
        aes_buf_out.length = HAL_AES_BLOCK_SIZES;

        parcel.in = &aes_buf;
        parcel.out = &aes_buf_out;
        parcel.key = key;
        parcel.iv = init_vector2;

        if (HAL_AES_STATUS_ERROR == aes_operate(&parcel, AES_CBC, 0, true)) {
            hal_crypt_lock_give();
            return HAL_AES_STATUS_ERROR;
        }

    } else {
        uint8_t data_block[HAL_AES_BLOCK_SIZES] = {0};
        memcpy(data_block, plain_text->buffer, plain_text->length);
        memset(data_block + last_block_size, padding_size, padding_size);

        aes_buf.buffer = data_block;
        aes_buf.length = HAL_AES_BLOCK_SIZES;

        parcel.in = &aes_buf;
        parcel.out = encrypted_text;
        parcel.key = key;
        parcel.iv = init_vector;

        if (HAL_AES_STATUS_ERROR == aes_operate(&parcel, AES_CBC, 0, true)) {
            hal_crypt_lock_give();
            return HAL_AES_STATUS_ERROR;
        }
    }

    hal_crypt_lock_give();
    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_cbc_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    if ((NULL == encrypted_text)
            || (NULL == plain_text)
            || (NULL == key)
            || (NULL == init_vector)) {
        return HAL_AES_STATUS_ERROR;
    }

    hal_crypt_lock_take();

    aes_parcel_t parcel;
    parcel.in = encrypted_text;
    parcel.out = plain_text;
    parcel.key = key;
    parcel.iv = init_vector;

    if (HAL_AES_STATUS_ERROR == aes_operate(&parcel, AES_CBC, 0, false)) {
        hal_crypt_lock_give();
        return HAL_AES_STATUS_ERROR;
    }

    hal_crypt_lock_give();
    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_ecb_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key)
{
    if ((NULL == encrypted_text)
            || (NULL == plain_text)
            || (NULL == key)) {
        return HAL_AES_STATUS_ERROR;
    }

    hal_aes_buffer_t aes_buf;
    hal_aes_buffer_t aes_buf_out;
    uint32_t last_block_size = plain_text->length % HAL_AES_BLOCK_SIZES;
    uint32_t block_num = plain_text->length / HAL_AES_BLOCK_SIZES;
    uint8_t padding_size = HAL_AES_BLOCK_SIZES - last_block_size;

    /* padding checking */
    if (encrypted_text->length < (plain_text->length + padding_size)) {
        log_hal_error("Inadequate encrypted buffer.");
        return HAL_AES_STATUS_ERROR;
    }

    hal_crypt_lock_take();

    aes_parcel_t parcel;

    if (block_num > 0) {
        /* block-alignment part */
        uint32_t first_encrypt_size = block_num * HAL_AES_BLOCK_SIZES;

        aes_buf.buffer = plain_text->buffer;
        aes_buf.length = first_encrypt_size;

        aes_buf_out.buffer = encrypted_text->buffer;
        aes_buf_out.length = first_encrypt_size;

        parcel.in = &aes_buf;
        parcel.out = &aes_buf_out;
        parcel.key = key;

        if (HAL_AES_STATUS_ERROR == aes_operate(&parcel, AES_ECB, 0, true)) {
            hal_crypt_lock_give();
            return HAL_AES_STATUS_ERROR;
        }

        /* non block-alignment part */
        uint8_t data_block[HAL_AES_BLOCK_SIZES] = {0};
        memcpy(data_block, plain_text->buffer + first_encrypt_size, last_block_size);
        memset(data_block + last_block_size, padding_size, padding_size);

        aes_buf.buffer = data_block;
        aes_buf.length = HAL_AES_BLOCK_SIZES;

        aes_buf_out.buffer = encrypted_text->buffer + first_encrypt_size;
        aes_buf_out.length = HAL_AES_BLOCK_SIZES;

        parcel.in = &aes_buf;
        parcel.out = &aes_buf_out;
        parcel.key = key;

        if (HAL_AES_STATUS_ERROR == aes_operate(&parcel, AES_ECB, 0, true)) {
            hal_crypt_lock_give();
            return HAL_AES_STATUS_ERROR;
        }

    } else {
        uint8_t data_block[HAL_AES_BLOCK_SIZES] = {0};
        memcpy(data_block, plain_text->buffer, plain_text->length);
        memset(data_block + last_block_size, padding_size, padding_size);

        aes_buf.buffer = data_block;
        aes_buf.length = HAL_AES_BLOCK_SIZES;

        parcel.in = &aes_buf;
        parcel.out = encrypted_text;
        parcel.key = key;

        if (HAL_AES_STATUS_ERROR == aes_operate(&parcel, AES_ECB, 0, true)) {
            hal_crypt_lock_give();
            return HAL_AES_STATUS_ERROR;
        }
    }

    hal_crypt_lock_give();
    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_ecb_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key)
{
    if ((NULL == encrypted_text)
            || (NULL == plain_text)
            || (NULL == key)) {
        return HAL_AES_STATUS_ERROR;
    }

    hal_crypt_lock_take();

    aes_parcel_t parcel;
    parcel.in = encrypted_text;
    parcel.out = plain_text;
    parcel.key = key;

    if (HAL_AES_STATUS_ERROR == aes_operate(&parcel, AES_ECB, 0, false)) {
        hal_crypt_lock_give();
        return HAL_AES_STATUS_ERROR;
    }

    hal_crypt_lock_give();
    return HAL_AES_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_AES_MODULE_ENABLED */

