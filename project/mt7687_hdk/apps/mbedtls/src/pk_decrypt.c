/*
 *  Public key-based simple decryption program
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdio.h>

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#endif

#include "syslog.h"

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_PK_PARSE_C) && \
    defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
#include "mbedtls/error.h"
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include <string.h>
#endif


#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_PK_PARSE_C) ||  \
    !defined(MBEDTLS_ENTROPY_C) || !defined(MBEDTLS_CTR_DRBG_C)
int pk_decrypt_main(char *enc_buf, size_t enc_len, char *key_buf, size_t key_len)
{
    LOG_I(mbedtls_proj, "MBEDTLS_BIGNUM_C and/or MBEDTLS_PK_PARSE_C and/or "
                   "and/or MBEDTLS_ENTROPY_C and/or "
                   "MBEDTLS_CTR_DRBG_C not defined.\n");
    return ( -1 );
}
#else
int pk_decrypt_main(char *enc_buf, size_t enc_len, char *key_buf, size_t key_len)
{
    int ret = 0;
    size_t olen = 0;
    mbedtls_pk_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    unsigned char result[1024];
    const char *pers = "mbedtls_pk_decrypt";

    mbedtls_ctr_drbg_init( &ctr_drbg );
    memset(result, 0, sizeof( result ) );

    LOG_I(mbedtls_proj, "%s\n", __func__);
    LOG_I(mbedtls_proj, "\n  . Seeding the random number generator..." );
    fflush( stdout );

    mbedtls_entropy_init( &entropy );
    if ( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                                        (const unsigned char *) pers,
                                        strlen( pers ) ) ) != 0 ) {
        LOG_I(mbedtls_proj, " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit;
    }

    mbedtls_pk_init( &pk );

    if ( ( ret = mbedtls_pk_parse_key( &pk, (const unsigned char *)key_buf, key_len, (const unsigned char *)"", 0 ) ) != 0 ) {
        LOG_I(mbedtls_proj, " failed\n  ! mbedtls_pk_parse_key returned -0x%04x\n", -ret );
        goto exit;
    }


    /*
     * Decrypt the encrypted RSA data and print the result.
     */
    LOG_I(mbedtls_proj, "\n  . Decrypting the encrypted data" );
    fflush( stdout );

    if ( ( ret = mbedtls_pk_decrypt( &pk, (const unsigned char *)enc_buf, enc_len, result, &olen, sizeof(result),
                                     mbedtls_ctr_drbg_random, &ctr_drbg ) ) != 0 ) {
        LOG_I(mbedtls_proj, " failed\n  ! mbedtls_pk_decrypt returned -0x%04x\n", -ret );
        goto exit;
    }

    LOG_I(mbedtls_proj, "\n  . OK\n\n" );

    LOG_I(mbedtls_proj, "The decrypted result is: '%s'\n\n", result );

exit:
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );


    return ( ret );
}
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_PK_PARSE_C && MBEDTLS_ENTROPY_C && MBEDTLS_CTR_DRBG_C */
