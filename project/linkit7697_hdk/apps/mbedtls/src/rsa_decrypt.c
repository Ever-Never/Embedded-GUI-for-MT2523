/*
 *  RSA simple decryption program
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

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_RSA_C) \
    && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
#include "mbedtls/rsa.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include <string.h>
#endif


#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_RSA_C) ||  \
    !defined(MBEDTLS_ENTROPY_C) || !defined(MBEDTLS_CTR_DRBG_C)
int rsa_decrypt_main( char *input )
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

#define RSA_D   "24BF6185468786FDD303083D25E64EFC" \
                "66CA472BC44D253102F8B4A9D3BFA750" \
                "91386C0077937FE33FA3252D28855837" \
                "AE1B484A8A9A45F7EE8C0C634F99E8CD" \
                "DF79C5CE07EE72C7F123142198164234" \
                "CABB724CF78B8173B9F880FC86322407" \
                "AF1FEDFDDE2BEB674CA15F3E81A1521E" \
                "071513A1E85B5DFA031F21ECAE91A34D"

#define RSA_P   "C36D0EB7FCD285223CFB5AABA5BDA3D8" \
                "2C01CAD19EA484A87EA4377637E75500" \
                "FCB2005C5C7DD6EC4AC023CDA285D796" \
                "C3D9E75E1EFC42488BB4F1D13AC30A57"

#define RSA_Q   "C000DF51A7C77AE8D7C7370C1FF55B69" \
                "E211C2B9E5DB1ED0BF61D0D9899620F4" \
                "910E4168387E3C30AA1E00C339A79508" \
                "8452DD96A9A5EA5D9DCA68DA636032AF"

#define RSA_DP  "C1ACF567564274FB07A0BBAD5D26E298" \
                "3C94D22288ACD763FD8E5600ED4A702D" \
                "F84198A5F06C2E72236AE490C93F07F8" \
                "3CC559CD27BC2D1CA488811730BB5725"

#define RSA_DQ  "4959CBF6F8FEF750AEE6977C155579C7" \
                "D8AAEA56749EA28623272E4F7D0592AF" \
                "7C1F1313CAC9471B5C523BFE592F517B" \
                "407A1BD76C164B93DA2D32A383E58357"

#define RSA_QP  "9AE7FBC99546432DF71896FC239EADAE" \
                "F38D18D2B2F0E2DD275AA977E2BF4411" \
                "F5A3B2A5D33605AEBBCCBA7FEB9F2D2F" \
                "A74206CEC169D74BF5A8C50D6F48EA08"

int rsa_decrypt_main( char *input )
{
    int ret = 0;
    size_t i;
    mbedtls_rsa_context rsa;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    unsigned char result[1024];
    const char *pers = "rsa_decrypt";

    memset(result, 0, sizeof( result ) );
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

    LOG_I(mbedtls_proj, "\n  . Reading private key from string" );
    fflush( stdout );

    mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );

    if ( ( ret = mbedtls_mpi_read_string( &rsa.N , 16, RSA_N ) ) != 0 ||
            ( ret = mbedtls_mpi_read_string( &rsa.E , 16, RSA_E ) ) != 0 ||
            ( ret = mbedtls_mpi_read_string( &rsa.D , 16, RSA_D ) ) != 0 ||
            ( ret = mbedtls_mpi_read_string( &rsa.P , 16, RSA_P ) ) != 0 ||
            ( ret = mbedtls_mpi_read_string( &rsa.Q , 16, RSA_Q ) ) != 0 ||
            ( ret = mbedtls_mpi_read_string( &rsa.DP, 16, RSA_DP ) ) != 0 ||
            ( ret = mbedtls_mpi_read_string( &rsa.DQ, 16, RSA_DQ ) ) != 0 ||
            ( ret = mbedtls_mpi_read_string( &rsa.QP, 16, RSA_QP ) ) != 0 ) {
        LOG_E(mbedtls_proj, " failed\n  ! mbedtls_mpi_read_string returned %d\n\n", ret );
        goto exit;
    }

    rsa.len = ( mbedtls_mpi_bitlen( &rsa.N ) + 7 ) >> 3;

    /*
     * Decrypt the encrypted RSA data and print the result.
     */
    LOG_I(mbedtls_proj, "\n  . Decrypting the encrypted data" );
    fflush( stdout );

    if ( ( ret = mbedtls_rsa_pkcs1_decrypt( &rsa, mbedtls_ctr_drbg_random, &ctr_drbg,
                                            MBEDTLS_RSA_PRIVATE, &i, (const unsigned char *)input, result,
                                            1024 ) ) != 0 ) {
        LOG_E(mbedtls_proj, " failed\n  ! mbedtls_rsa_pkcs1_decrypt returned %d\n\n", ret );
        goto exit;
    }

    LOG_I(mbedtls_proj, "\n  . OK\n\n" );

    LOG_I(mbedtls_proj, "The decrypted result is: '%s'\n\n", result );

exit:
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

    return ( ret );
}
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_RSA_C && MBEDTLS_FS_IO */
