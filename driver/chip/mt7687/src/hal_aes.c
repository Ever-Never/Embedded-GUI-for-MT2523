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
#include "mt7687.h"
#include "crypt_aes.h"
#include "hal_log.h"
#include "hal_crypt_internal.h"
#include "hal_nvic.h"

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

volatile static bool g_aes_op_done = false;

/* It's about 10s at 192MHz CPU clock */
#define HAL_AES_MAX_WAIT_COUNT 0x10000000

static void aes_operation_done(hal_nvic_irq_t irq_number)
{
    hal_nvic_disable_irq((IRQn_Type)CM4_MTK_CRYPTO_IRQ);
    hal_nvic_clear_pending_irq((IRQn_Type)CM4_MTK_CRYPTO_IRQ);

    g_aes_op_done = true;

    hal_nvic_enable_irq((IRQn_Type)CM4_MTK_CRYPTO_IRQ);
}

static hal_aes_status_t do_aes_encrypt(uint8_t *encrypt_buffer,
                                       uint32_t encrypt_buffer_length,
                                       uint8_t *plain_buffer,
                                       uint32_t plain_buffer_length,
                                       uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    g_aes_op_done = false;
    int32_t ret_val = aes_operate(encrypt_buffer,
                                  encrypt_buffer_length,
                                  plain_buffer,
                                  plain_buffer_length,
                                  init_vector,
                                  AES_MODE_ENCRYPT);
    if (ret_val < 0) {
        log_hal_error("aes_operate fail.");
        return HAL_AES_STATUS_ERROR;
    }
    uint32_t wait_count = 0;
    while (!g_aes_op_done) {
        //simple wait
        wait_count++;
        if (wait_count > HAL_AES_MAX_WAIT_COUNT) {
            log_hal_error("wait for encrypt timeout.");
            return HAL_AES_STATUS_ERROR;
        }
    }
    return HAL_AES_STATUS_OK;
}

static hal_aes_status_t do_aes_decrypt(hal_aes_buffer_t *plain_text,
                                       hal_aes_buffer_t *encrypted_text,
                                       uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    g_aes_op_done = false;

    int32_t ret_val = aes_operate(encrypted_text->buffer,
                                  encrypted_text->length,
                                  plain_text->buffer,
                                  plain_text->length,
                                  init_vector,
                                  AES_MODE_DECRYPT);
    if (ret_val < 0) {
        log_hal_error("aes_operate fail.");
        return HAL_AES_STATUS_ERROR;
    }
    uint32_t wait_count = 0;
    while (!g_aes_op_done) {
        //simple wait
        wait_count++;
        if (wait_count > HAL_AES_MAX_WAIT_COUNT) {
            log_hal_error("wait for decrypt timeout.");
            return HAL_AES_STATUS_ERROR;
        }
    }
    return HAL_AES_STATUS_OK;
}

/* internal common function */
hal_aes_status_t hal_aes_encrypt_with_padding(hal_aes_buffer_t *encrypted_text,
        hal_aes_buffer_t *plain_text,
        hal_aes_buffer_t *key,
        uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
        uint8_t mode)
{
    if ((NULL == encrypted_text)
            || (NULL == plain_text)
            || (NULL == key)
            || (NULL == init_vector)) {
        log_hal_error("NULL input.");
        return HAL_AES_STATUS_ERROR;
    }
    uint32_t last_block_size = plain_text->length % HAL_AES_BLOCK_SIZES;
    uint32_t block_num = plain_text->length / HAL_AES_BLOCK_SIZES;
    uint8_t padding_size = HAL_AES_BLOCK_SIZES - last_block_size;
    uint8_t *iv;

    if (encrypted_text->length < (plain_text->length + padding_size)) {
        log_hal_error("Inadequate encrypted buffer.");
        return HAL_AES_STATUS_ERROR;
    }

    hal_crypt_lock_take();

    hal_nvic_register_isr_handler((IRQn_Type)CM4_MTK_CRYPTO_IRQ, aes_operation_done);
    hal_nvic_set_priority((IRQn_Type)CM4_MTK_CRYPTO_IRQ, CM4_MTK_CRYPTO_PRI);
    hal_nvic_enable_irq((IRQn_Type)CM4_MTK_CRYPTO_IRQ);

    aes_set_key((uint8_t)key->length, key->buffer);

    if (AES_TYPE_CBC == mode) { /* AES_TYPE_CBC */
        aes_configure(1, 1); /* 1:software 0:ECB and 1:CBC */
    } else { /* AES_TYPE_CBC */
        aes_configure(1, 0); /* 1:software 0:ECB and 1:CBC */
    }

    if (block_num > 0) {
        uint32_t first_encypt_size = block_num * HAL_AES_BLOCK_SIZES;
        if (HAL_AES_STATUS_OK != do_aes_encrypt(encrypted_text->buffer,
                                                encrypted_text->length,
                                                plain_text->buffer,
                                                first_encypt_size,
                                                init_vector)) {
            log_hal_error("do_aes_encrypt fail.");
            hal_crypt_lock_give();
            return HAL_AES_STATUS_ERROR;
        }

        uint8_t data_block[HAL_AES_BLOCK_SIZES] = {0};
        memcpy(data_block, plain_text->buffer + first_encypt_size, last_block_size);
        memset(data_block + last_block_size, padding_size, padding_size);

        uint8_t *init_vector2;
        if (AES_TYPE_CBC == mode) { /* do 2nd aes cbc operation need to input newer iv */
            init_vector2 = encrypted_text->buffer + first_encypt_size - HAL_AES_BLOCK_SIZES;
            iv = init_vector2;
        } else {
            iv = init_vector;
        }

        if (HAL_AES_STATUS_OK != do_aes_encrypt(encrypted_text->buffer + first_encypt_size,
                                                encrypted_text->length,
                                                data_block,
                                                HAL_AES_BLOCK_SIZES,
                                                iv)) {
            log_hal_error("do_aes_encrypt fail.");
            hal_crypt_lock_give();
            return HAL_AES_STATUS_ERROR;
        }
    } else {
        uint8_t data_block[HAL_AES_BLOCK_SIZES] = {0};
        memcpy(data_block, plain_text->buffer, plain_text->length);
        memset(data_block + last_block_size, padding_size, padding_size);
        if (HAL_AES_STATUS_OK != do_aes_encrypt(encrypted_text->buffer,
                                                encrypted_text->length,
                                                data_block,
                                                HAL_AES_BLOCK_SIZES,
                                                init_vector)) {
            log_hal_error("do_aes_encrypt fail.");
            hal_crypt_lock_give();
            return HAL_AES_STATUS_ERROR;
        }
    }

    encrypted_text->length = (block_num + 1) * HAL_AES_BLOCK_SIZES;

    hal_crypt_lock_give();
    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_decrypt(hal_aes_buffer_t *plain_text,
                                 hal_aes_buffer_t *encrypted_text,
                                 hal_aes_buffer_t *key,
                                 uint8_t init_vector[HAL_AES_CBC_IV_LENGTH],
                                 uint8_t mode)
{
    if ((NULL == plain_text)
            || (NULL == encrypted_text)
            || (NULL == key)
            || (NULL == init_vector)) {
        log_hal_error("NULL input.");
        return HAL_AES_STATUS_ERROR;
    }

    if ((encrypted_text->length % HAL_AES_BLOCK_SIZES) != 0) {
        log_hal_error("Invalid encrypted text length: %lu.", encrypted_text->length);
        return HAL_AES_STATUS_ERROR;
    }
    if (plain_text->length < (encrypted_text->length - HAL_AES_BLOCK_SIZES)) {
        log_hal_error("Plain text buffer lengthL %lu is too small, encrypted length is: %lu",
                      encrypted_text->length, encrypted_text->length);
        return HAL_AES_STATUS_ERROR;
    }
    if ((key->length != HAL_AES_KEY_LENGTH_128)
            && (key->length != HAL_AES_KEY_LENGTH_192)
            && (key->length != HAL_AES_KEY_LENGTH_256)) {
        log_hal_error("key length is %lu, invalid. It has to be 16, 24 or 32.", key->length);
        return HAL_AES_STATUS_ERROR;
    }

    hal_crypt_lock_take();

    hal_nvic_register_isr_handler((IRQn_Type)CM4_MTK_CRYPTO_IRQ, aes_operation_done);
    hal_nvic_set_priority((IRQn_Type)CM4_MTK_CRYPTO_IRQ, CM4_MTK_CRYPTO_PRI);
    hal_nvic_enable_irq((IRQn_Type)CM4_MTK_CRYPTO_IRQ);

    aes_set_key(key->length, key->buffer);
    if (AES_TYPE_CBC == mode) { /* AES_TYPE_CBC */
        aes_configure(1, 1); /* 1:software 0:ECB and 1:CBC */
    } else { /* AES_TYPE_CBC */
        aes_configure(1, 0); /* 1:software 0:ECB and 1:CBC */
    }


    if (HAL_AES_STATUS_OK != do_aes_decrypt(plain_text, encrypted_text, init_vector)) {
        log_hal_error("do_aes_decrypt fail");
        hal_crypt_lock_give();
        return HAL_AES_STATUS_ERROR;
    }

    uint8_t padding_size = plain_text->buffer[encrypted_text->length - 1];
    log_hal_dump("Decrypted raw data(AES %s): ", plain_text->buffer, encrypted_text->length, (mode == AES_TYPE_CBC) ? "CBC" : "ECB");
    plain_text->length = encrypted_text->length - padding_size;

    hal_crypt_lock_give();
    return HAL_AES_STATUS_OK;
}

hal_aes_status_t hal_aes_cbc_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    return hal_aes_encrypt_with_padding(encrypted_text, plain_text, key, init_vector, AES_TYPE_CBC);
}

hal_aes_status_t hal_aes_cbc_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key,
                                     uint8_t init_vector[HAL_AES_CBC_IV_LENGTH])
{
    return hal_aes_decrypt(plain_text, encrypted_text, key, init_vector, AES_TYPE_CBC);
}

hal_aes_status_t hal_aes_ecb_encrypt(hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *key)
{
    uint8_t init_vector[16] = {0};

    return hal_aes_encrypt_with_padding(encrypted_text, plain_text, key, init_vector, AES_TYPE_ECB);
}

hal_aes_status_t hal_aes_ecb_decrypt(hal_aes_buffer_t *plain_text,
                                     hal_aes_buffer_t *encrypted_text,
                                     hal_aes_buffer_t *key)
{
    uint8_t init_vector[HAL_AES_CBC_IV_LENGTH] = {0};

    return hal_aes_decrypt(plain_text, encrypted_text, key, init_vector, AES_TYPE_ECB);
}

#endif /* HAL_AES_MODULE_ENABLED */

