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

#include <stdlib.h>

#define CM4
//#define RANDOM_NUMBER_DISABLE

#ifdef CM4
#define PROCESSOR_APP
#else
#define PROCESSOR_NCP
#endif

#ifdef PROCESSOR_APP
/*
 * header files from IoT SDK for CM4.
 */
#include "os.h"
#include "os_util.h"
#include "mbedtls/arc4.h"
#ifndef RANDOM_NUMBER_DISABLE
#include "hal_trng.h"
#endif
#endif

#ifdef PROCESSOR_NCP
/*
 * header files from BORA.
 */
#include "precomp_rom.h"
#include "precomp.h"
#include "crypt_arc4.h"
#include "rt_config.h"
#endif

#include "sie.h"


#ifdef PROCESSOR_APP
sie_request_t *sie_request_init(void)
{
    sie_request_t *request = os_malloc(sizeof(*request));

    if (NULL != request) {
#ifndef RANDOM_NUMBER_DISABLE
        uint32_t random;

        hal_trng_get_generated_random_number(&random);

        request->nonce[0] = random         & 0xFF;
        request->nonce[1] = (random >> 8)  & 0xFF;
        request->nonce[2] = (random >> 16) & 0xFF;
        request->nonce[3] = (random >> 24) & 0xFF;
#else
        request->nonce[0] = 0x01;
        request->nonce[1] = 0x02;
        request->nonce[2] = 0x03;
        request->nonce[3] = 0x04;
#endif
    }

    return request;
}


void sie_request_deinit(sie_request_t *request)
{
    os_memset(request, 0, sizeof(*request));
    os_free(request);
}


sie_response_t *sie_response_init(sie_request_t   *request,
                                  uint8_t         *payload,
                                  uint16_t        payload_len)
{
    sie_response_t  *response = os_malloc(sizeof(*response) + payload_len);

    if (NULL == response) {
        return NULL;
    }

    response->nonce[0] = request->nonce[0];
    response->nonce[1] = request->nonce[1];
    response->nonce[2] = request->nonce[2];
    response->nonce[3] = request->nonce[3];

    mbedtls_arc4_context ctx;

    mbedtls_arc4_init(&ctx);

    mbedtls_arc4_setup(&ctx, &request->nonce[0], sizeof(request->nonce));
    mbedtls_arc4_crypt(&ctx, payload_len, payload, &response->payload[0]);

    mbedtls_arc4_free(&ctx);

    response->payload_len = payload_len;

    return response;
}


void sie_response_deinit(sie_response_t *response, uint16_t response_len)
{
    os_memset(response, 0, (size_t)response_len);
    os_free(response);
}


sie_validate_t sie_response_decrypt(sie_response_t  *response,
                                    uint16_t        reponse_len,
                                    sie_request_t   *request)
{
    sie_response_t  *encrypt;

    encrypt = (sie_response_t *)os_malloc(reponse_len);
    if (NULL == encrypt) {
        return SIE_VALIDATE_FAILURE;
    }

    os_memcpy(encrypt, response, reponse_len);

    mbedtls_arc4_context ctx;

    mbedtls_arc4_init(&ctx);

    mbedtls_arc4_setup(&ctx, &request->nonce[0], sizeof(request->nonce));
    mbedtls_arc4_crypt(&ctx, reponse_len, &encrypt->payload[0], &response->payload[0]);

    mbedtls_arc4_free(&ctx);

    os_free(encrypt);

    if (os_memcmp(&response->nonce[0],
                  &request->nonce[0],
                  sizeof(request->nonce)) ||
            (reponse_len != (response->payload_len + sizeof(*response)))
       ) {
        return SIE_VALIDATE_FAILURE;
    }

    return SIE_VALIDATE_SUCCESS;
}
#endif


#ifdef PROCESSOR_NCP
sie_request_t *sie_request_init(void)
{
    sie_request_t *request = (sie_request_t *)iot_malloc(sizeof(*request));

    if (NULL != request) {
#ifndef RANDOM_NUMBER_DISABLE
        UINT_16    rand;

        halGetRandomNumber(&rand);
        request->nonce[0] = (char)(rand & 0xFF);
        halGetRandomNumber(&rand);
        request->nonce[1] = (char)(rand & 0xFF);
        halGetRandomNumber(&rand);
        request->nonce[2] = (char)(rand & 0xFF);
        halGetRandomNumber(&rand);
        request->nonce[3] = (char)(rand & 0xFF);
#else
        request->nonce[0] = 0x01;
        request->nonce[1] = 0x02;
        request->nonce[2] = 0x03;
        request->nonce[3] = 0x04;
#endif
    }

    return request;
}


void sie_request_deinit(sie_request_t *request)
{
    memset(request, 0, sizeof(*request));
    iot_free(request);
}


sie_response_t *sie_response_init(sie_request_t   *request,
                                  uint8_t         *payload,
                                  uint16_t        payload_len)
{
    sie_response_t  *response;
    sie_response_t  *encrypt;

    ARC4_CTX_STRUC  arc;

    response = (sie_response_t *)iot_malloc(sizeof(*response) + payload_len);
    if (NULL == response) {
        return NULL;
    }

    encrypt = (sie_response_t *)iot_malloc(sizeof(*encrypt) + payload_len);
    if (NULL == encrypt) {
        iot_free(response);
        return NULL;
    }

    response->nonce[0] = request->nonce[0];
    response->nonce[1] = request->nonce[1];
    response->nonce[2] = request->nonce[2];
    response->nonce[3] = request->nonce[3];

    response->payload_len = payload_len;

    memcpy(&response->payload[0],
           (PUINT_8)payload,
           (UINT_32)payload_len);

    ARC4_INIT(&arc, &request->nonce[0], sizeof(request->nonce));

    ARC4_Compute(&arc,
                 (UINT_8 *)response,
                 sizeof(*response) + payload_len,
                 (UINT_8 *)encrypt);

    iot_free(response);

    return encrypt;
}


void sie_response_deinit(sie_response_t *response, uint16_t response_len)
{
    memset(response, 0, (size_t)response_len);
    iot_free(response);
}


sie_validate_t sie_response_decrypt(sie_response_t  *response,
                                    uint16_t        reponse_len,
                                    sie_request_t   *request)
{
    sie_response_t  *encrypt;
    ARC4_CTX_STRUC  arc;

    encrypt = (sie_response_t *)iot_malloc(reponse_len);
    if (NULL == encrypt) {
        return SIE_VALIDATE_FAILURE;
    }

    memcpy(encrypt, response, reponse_len);

    ARC4_INIT(&arc, &request->nonce[0], sizeof(request->nonce));

    ARC4_Compute(&arc,
                 (UINT_8 *)encrypt,
                 (UINT_32)reponse_len,
                 (UINT_8 *)response);

    iot_free(encrypt);

    if (memcmp(&response->nonce[0],
               &request->nonce[0],
               sizeof(request->nonce)) ||
            (reponse_len != (response->payload_len + sizeof(*response)))
       ) {
        return SIE_VALIDATE_FAILURE;
    }

    return SIE_VALIDATE_SUCCESS;
}
#endif

