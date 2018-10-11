/*
 *  RSA simple data encryption program
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
int pk_encrypt_main( char *input, char *output, size_t *olen, size_t output_buf_len, char *key_buf, size_t key_len )
{
    LOG_I(mbedtls_proj, "MBEDTLS_BIGNUM_C and/or MBEDTLS_PK_PARSE_C and/or "
                   "MBEDTLS_ENTROPY_C and/or MBEDTLS_FS_IO and/or "
                   "MBEDTLS_CTR_DRBG_C not defined.\n");
    return ( -1 );
}
#else
int pk_encrypt_main( char *input, char *output, size_t *olen, size_t output_buf_len, char *key_buf, size_t key_len )
{
    int ret = 0;
    size_t i;
    mbedtls_pk_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "mbedtls_pk_encrypt";

    mbedtls_ctr_drbg_init( &ctr_drbg );
    LOG_I(mbedtls_proj, "%s\n", __func__);
    LOG_I(mbedtls_proj, "\n  . Seeding the random number generator..." );
    fflush( stdout );

    mbedtls_entropy_init( &entropy );
    if ( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                                        (const unsigned char *) pers,
                                        strlen( pers ) ) ) != 0 ) {
        LOG_E(mbedtls_proj, " failed\n  ! mbedtls_ctr_drbg_seed returned -0x%04x\n", -ret );
        goto exit;
    }

    mbedtls_pk_init( &pk );

    if ( ( ret = mbedtls_pk_parse_public_key( &pk, (const unsigned char *)key_buf, key_len ) ) != 0 ) {
        LOG_E(mbedtls_proj, " failed\n  ! mbedtls_pk_parse_public_key returned -0x%04x\n", -ret );
        goto exit;
    }

    if ( strlen( input ) > 100 ) {
        LOG_E(mbedtls_proj, " Input data larger than 100 characters.\n\n" );
        goto exit;
    }

    /*
     * Calculate the RSA encryption of the hash.
     */
    LOG_I(mbedtls_proj, "\n  . Generating the encrypted value" );
    fflush( stdout );

    if ( ( ret = mbedtls_pk_encrypt( &pk, (const unsigned char *)input, strlen( input ),
                                     (unsigned char *)output, olen, output_buf_len,
                                     mbedtls_ctr_drbg_random, &ctr_drbg ) ) != 0 ) {
        LOG_E(mbedtls_proj, " failed\n  ! mbedtls_pk_encrypt returned -0x%04x\n", -ret );
        goto exit;
    }

    /*
     * Print the signature
     */
    LOG_I(mbedtls_proj, "\n Encrypt result: \n" );
    for ( i = 0; i < *olen; i++ )
        LOG_I(mbedtls_proj, "%02X%s", output[i],
                        ( i + 1 ) % 16 == 0 ? "\r\n" : " " );

exit:
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

    return ( ret );
}
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_PK_PARSE_C && MBEDTLS_ENTROPY_C && MBEDTLS_CTR_DRBG_C */
