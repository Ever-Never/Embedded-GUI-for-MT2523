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

#include "hal_des.h"

#ifdef HAL_DES_MODULE_ENABLED

#include <string.h>
#include "mt7687.h"
#include "crypt_des.h"
#include "hal_log.h"
#include "hal_crypt_internal.h"
#include "hal_nvic.h"

#define HAL_DES_MIN_SIZE (16)

/* It's about 10s at 192MHz CPU clock */
#define HAL_DES_MAX_WAIT_COUNT (0x10000000)

volatile static uint32_t g_des_op_done = false;

static void des_operation_done(hal_nvic_irq_t irq_number)
{
    hal_nvic_disable_irq((IRQn_Type)CM4_MTK_CRYPTO_IRQ);
    hal_nvic_clear_pending_irq((IRQn_Type)CM4_MTK_CRYPTO_IRQ);

    g_des_op_done = true;

    hal_nvic_enable_irq((IRQn_Type)CM4_MTK_CRYPTO_IRQ);
}

static hal_des_status_t do_des_encrypt(uint8_t *encrypt_buffer,
                                       uint32_t encrypt_buffer_length,
                                       uint8_t *plain_buffer,
                                       uint32_t plain_buffer_length,
                                       uint8_t init_vector[HAL_DES_CBC_IV_LENGTH])
{
    g_des_op_done = false;
    int32_t ret_val = des_operate(encrypt_buffer,
                                  encrypt_buffer_length,
                                  plain_buffer,
                                  plain_buffer_length,
                                  init_vector,
                                  DES_MODE_ENCRYPT);
    if (ret_val < 0) {
        log_hal_error("des_operate fail.");
        return HAL_DES_STATUS_ERROR;
    }
    uint32_t wait_count = 0;
    while (!g_des_op_done) {
        //simple wait
        wait_count++;
        if (wait_count > HAL_DES_MAX_WAIT_COUNT) {
            log_hal_error("wait for encrypt timeout.");
            return HAL_DES_STATUS_ERROR;
        }
    }
    return HAL_DES_STATUS_OK;
}

static hal_des_status_t do_des_decrypt(hal_des_buffer_t *plain_text,
                                       hal_des_buffer_t *encrypted_text,
                                       uint8_t init_vector[HAL_DES_CBC_IV_LENGTH])
{
    g_des_op_done = false;

    int32_t ret_val = des_operate(encrypted_text->buffer,
                                  encrypted_text->length,
                                  plain_text->buffer,
                                  plain_text->length,
                                  init_vector,
                                  DES_MODE_DECRYPT);
    if (ret_val < 0) {
        log_hal_error("des_operate fail.");
        return HAL_DES_STATUS_ERROR;
    }
    uint32_t wait_count = 0;
    while (!g_des_op_done) {
        //simple wait
        wait_count++;
        if (wait_count > HAL_DES_MAX_WAIT_COUNT) {
            log_hal_error("wait for decrypt timeout.");
            return HAL_DES_STATUS_ERROR;
        }
    }
    return HAL_DES_STATUS_OK;
}


hal_des_status_t hal_des_encrypt_with_padding(hal_des_buffer_t *encrypted_text,
        hal_des_buffer_t *plain_text,
        hal_des_buffer_t *key,
        uint8_t init_vector[HAL_DES_CBC_IV_LENGTH],
        uint8_t mode)
{

    if ((NULL == encrypted_text)
            || (NULL == plain_text)
            || (NULL == key)
            || (NULL == init_vector)) {
        log_hal_error("NULL input.");
        return HAL_DES_STATUS_ERROR;
    }

    uint32_t last_block_size = plain_text->length % HAL_DES_MIN_SIZE;
    uint32_t padding_size = HAL_DES_MIN_SIZE - last_block_size;
    uint32_t block_num = plain_text->length / HAL_DES_MIN_SIZE;
    uint8_t *iv;

    if (encrypted_text->length < (plain_text->length + padding_size)) {
        log_hal_error("Inadequate encrypted buffer.");
        return HAL_DES_STATUS_ERROR;
    }

    hal_crypt_lock_take();

    hal_nvic_register_isr_handler((IRQn_Type)CM4_MTK_CRYPTO_IRQ, des_operation_done);
    hal_nvic_set_priority((IRQn_Type)CM4_MTK_CRYPTO_IRQ, CM4_MTK_CRYPTO_PRI);
    hal_nvic_enable_irq((IRQn_Type)CM4_MTK_CRYPTO_IRQ);

    des_set_key(key->length, key->buffer);
    if (DES_TYPE_CBC == mode) { /* DES_TYPE_CBC */
        des_configure(1, 1);  /* 0:efuse and 1:software   0:ECB and 1:CBC */
    } else {  /* DES_TYPE_ECB */
        des_configure(1, 0);  /* 0:efuse and 1:software   0:ECB and 1:CBC */
    }

    if (block_num > 0) {
        uint32_t first_encypt_size = block_num * HAL_DES_MIN_SIZE;
        if (HAL_DES_STATUS_OK != do_des_encrypt(encrypted_text->buffer,
                                                encrypted_text->length,
                                                plain_text->buffer,
                                                first_encypt_size,
                                                init_vector)) {
            log_hal_error("do_des_encrypt fail.");
            hal_crypt_lock_give();
            return HAL_DES_STATUS_ERROR;
        }

        uint8_t data_block[HAL_DES_MIN_SIZE] = {0};
        memcpy(data_block, plain_text->buffer + first_encypt_size, last_block_size);
        memset(data_block + last_block_size, padding_size, padding_size);

        uint8_t *init_vector2;
        if (DES_TYPE_CBC == mode) { /* DES_TYPE_CBC */
            init_vector2 = encrypted_text->buffer + first_encypt_size - HAL_DES_CBC_IV_LENGTH;
            iv = init_vector2;
        } else {
            iv = init_vector;
        }

        if (HAL_DES_STATUS_OK != do_des_encrypt(encrypted_text->buffer + first_encypt_size,
                                                encrypted_text->length,
                                                data_block,
                                                HAL_DES_MIN_SIZE,
                                                iv)) {
            log_hal_error("do_des_encrypt fail.");
            hal_crypt_lock_give();
            return HAL_DES_STATUS_ERROR;
        }
    } else {
        uint8_t data_block[HAL_DES_MIN_SIZE] = {0};
        memcpy(data_block, plain_text->buffer, plain_text->length);
        memset(data_block + last_block_size, padding_size, padding_size);

        if (HAL_DES_STATUS_OK != do_des_encrypt(encrypted_text->buffer,
                                                encrypted_text->length,
                                                data_block,
                                                HAL_DES_MIN_SIZE,
                                                init_vector)) {
            log_hal_error("do_des_encrypt fail.");
            hal_crypt_lock_give();
            return HAL_DES_STATUS_ERROR;
        }
    }

    encrypted_text->length = (block_num + 1) * HAL_DES_MIN_SIZE;

    hal_crypt_lock_give();
    return HAL_DES_STATUS_OK;
}

hal_des_status_t hal_des_decrypt(hal_des_buffer_t *plain_text,
                                 hal_des_buffer_t *encrypted_text,
                                 hal_des_buffer_t *key,
                                 uint8_t init_vector[HAL_DES_CBC_IV_LENGTH],
                                 uint8_t mode)
{
    if ((NULL == plain_text)
            || (NULL == encrypted_text)
            || (NULL == key)
            || (NULL == init_vector)) {
        log_hal_error("NULL input.");
        return HAL_DES_STATUS_ERROR;
    }
    if ((encrypted_text->length % HAL_DES_BLOCK_SIZES) != 0) {
        log_hal_error("Invalid encrypted text length: %lu.", encrypted_text->length);
        return HAL_DES_STATUS_ERROR;
    }
    if (plain_text->length < (encrypted_text->length - HAL_DES_BLOCK_SIZES)) {
        log_hal_error("Plain text buffer lengthL %lu is too small, encrypted length is: %lu",
                      plain_text->length, encrypted_text->length);
        return HAL_DES_STATUS_ERROR;
    }
    if ((key->length != HAL_DES_KEY_LENGTH_128)
            && (key->length != HAL_DES_KEY_LENGTH_192)
            && (key->length != HAL_DES_KEY_LENGTH_64)) {
        log_hal_error("Invalid key length: %lu", key->length);
        return HAL_DES_STATUS_ERROR;
    }

    hal_crypt_lock_take();

    hal_nvic_register_isr_handler((IRQn_Type)CM4_MTK_CRYPTO_IRQ, des_operation_done);
    hal_nvic_set_priority((IRQn_Type)CM4_MTK_CRYPTO_IRQ, CM4_MTK_CRYPTO_PRI);
    hal_nvic_enable_irq((IRQn_Type)CM4_MTK_CRYPTO_IRQ);

    des_set_key(key->length, key->buffer);
    if (DES_TYPE_CBC == mode) { /* DES_TYPE_CBC */
        des_configure(1, 1);  /* 0:efuse and 1:software   0:ECB and 1:CBC */
    } else {  /* DES_TYPE_ECB */
        des_configure(1, 0);  /* 0:efuse and 1:software   0:ECB and 1:CBC */
    }

    if (HAL_DES_STATUS_OK != do_des_decrypt(plain_text, encrypted_text, init_vector)) {
        log_hal_error("do_des_decrypt fail.");
        hal_crypt_lock_give();
        return HAL_DES_STATUS_ERROR;
    }

    uint32_t padding_size = plain_text->buffer[encrypted_text->length - 1];
    plain_text->length = encrypted_text->length - padding_size;

    hal_crypt_lock_give();
    return HAL_DES_STATUS_OK;
}

hal_des_status_t hal_des_cbc_encrypt(hal_des_buffer_t *encrypted_text,
                                     hal_des_buffer_t *plain_text,
                                     hal_des_buffer_t *key,
                                     uint8_t init_vector[HAL_DES_CBC_IV_LENGTH])
{
    return hal_des_encrypt_with_padding(encrypted_text, plain_text, key, init_vector, DES_TYPE_CBC);
}

hal_des_status_t hal_des_cbc_decrypt(hal_des_buffer_t *plain_text,
                                     hal_des_buffer_t *encrypted_text,
                                     hal_des_buffer_t *key,
                                     uint8_t init_vector[HAL_DES_CBC_IV_LENGTH])
{
    return hal_des_decrypt(plain_text, encrypted_text, key, init_vector, DES_TYPE_CBC);
}

hal_des_status_t hal_des_ecb_encrypt(hal_des_buffer_t *encrypted_text,
                                     hal_des_buffer_t *plain_text,
                                     hal_des_buffer_t *key)
{
    uint8_t init_vector[8] = {0};

    return hal_des_encrypt_with_padding(encrypted_text, plain_text, key, init_vector, DES_TYPE_ECB);
}

hal_des_status_t hal_des_ecb_decrypt(hal_des_buffer_t *plain_text,
                                     hal_des_buffer_t *encrypted_text,
                                     hal_des_buffer_t *key)
{
    uint8_t init_vector[8] = {0};

    return hal_des_decrypt(plain_text, encrypted_text, key, init_vector, DES_TYPE_ECB);
}

#endif /* HAL_DES_MODULE_ENABLED */

