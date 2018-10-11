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

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_RSA_C) && \
    defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
#include "mbedtls/rsa.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include <string.h>
#endif


#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_RSA_C) ||  \
    !defined(MBEDTLS_ENTROPY_C) || !defined(MBEDTLS_CTR_DRBG_C)
int rsa_encrypt_main( char *input, char *output )
{
    LOG_I(mbedtls_proj, "MBEDTLS_BIGNUM_C and/or MBEDTLS_RSA_C and/or "
                   "MBEDTLS_ENTROPY_C and/or "
                   "MBEDTLS_CTR_DRBG_C not defined.\n");
    return ( -1 );
}
#else
#define RSA_N   "9292758453063D803DD603D5E777D788" \
                "8ED1D5BF35786190FA2F23EBC0848AEA" \
                "DDA92CA6C3D80B32C4D109BE0F36D6AE" \
                "7130B9CED7ACDF54CFC7555AC14EEBAB" \
                "93A89813FBF3C4F8066D2D800F7C38A8" \
                "1AE31942917403FF4946B0A83D3D3E05" \
                "EE57C6F5F5606FB5D4BC6CD34EE0801A" \
                "5E94BB77B07507233A0BC7BAC8F90F79"

#define RSA_E   "10001"

int rsa_encrypt_main( char *input, char *output )
{
    int ret = 0;
    size_t i;
    mbedtls_rsa_context rsa;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "rsa_encrypt";

    mbedtls_ctr_drbg_init( &ctr_drbg );
    LOG_I(mbedtls_proj, "%s\n", __func__);
    LOG_I(mbedtls_proj, "\n  . Seeding the random number generator..." );
    fflush( stdout );

    mbedtls_entropy_init( &entropy );
    if ( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                                        (const unsigned char *) pers,
                                        strlen( pers ) ) ) != 0 ) {
        LOG_E(mbedtls_proj, " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit;
    }

    mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );

    if ( ( ret = mbedtls_mpi_read_string( &rsa.N, 16, RSA_N ) ) != 0 ||
            ( ret = mbedtls_mpi_read_string( &rsa.E, 16, RSA_E ) ) != 0 ) {
        LOG_E(mbedtls_proj, " failed\n  ! mbedtls_mpi_read_file returned %d\n\n", ret );
        goto exit;
    }

    rsa.len = ( mbedtls_mpi_bitlen( &rsa.N ) + 7 ) >> 3;


    /*
     * Calculate the RSA encryption of the hash.
     */
    LOG_I(mbedtls_proj, "\n  . Generating the RSA encrypted value" );
    fflush( stdout );

    if ( ( ret = mbedtls_rsa_pkcs1_encrypt( &rsa, mbedtls_ctr_drbg_random, &ctr_drbg,
                                            MBEDTLS_RSA_PUBLIC, strlen( input ),
                                            (const unsigned char *)input, (unsigned char *)output ) ) != 0 ) {
        LOG_E(mbedtls_proj, " failed\n  ! mbedtls_rsa_pkcs1_encrypt returned %d\n\n", ret );
        goto exit;
    }

    /*
     * Print the signature
     */
    LOG_I(mbedtls_proj, "Encrypt result: \n");
    for ( i = 0; i < rsa.len; i++ )
        LOG_I(mbedtls_proj, "%02X%s", output[i],
                        ( i + 1 ) % 16 == 0 ? "\r\n" : " " );

exit:
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

    return ( ret );
}
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_RSA_C && MBEDTLS_ENTROPY_C && MBEDTLS_CTR_DRBG_C */
