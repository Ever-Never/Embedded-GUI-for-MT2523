/*
 *  AES-256 encryption program
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "syslog.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#endif

#include "mbedtls/aes.h"
#include "mbedtls/md.h"

#define KEY "E76B2413958B00E193"

static const char plaintext[] = "===Hello! This is plain text!===";  /* the length must be the multiple of 16 */

int aes_main( void )
{
    int ret = 0;
    int i, n;
    int lastn;
    size_t keylen;

    char *p;
    unsigned char IV[16];
    unsigned char key[512];
    unsigned char digest[32];
    unsigned char buffer[256];
    unsigned char output_buffer[256];
    unsigned char ciphertext[256];
    unsigned char diff;

    mbedtls_aes_context aes_ctx;
    mbedtls_md_context_t sha_ctx;
    size_t plaintext_size, offset, output_offset, ciphertext_size;
    unsigned char tmp[16];

    LOG_I(mbedtls_proj, "Plaintext:\n");
    LOG_I(mbedtls_proj, "%s\n", plaintext);
    LOG_I(mbedtls_proj, "Start to encrypt\n");
    mbedtls_aes_init( &aes_ctx );
    mbedtls_md_init( &sha_ctx );
    ret = mbedtls_md_setup( &sha_ctx, mbedtls_md_info_from_type( MBEDTLS_MD_SHA256 ), 1 );
    if ( ret != 0 ) {
        LOG_E(mbedtls_proj, "! mbedtls_md_setup() returned -0x%04x\n", -ret );
        goto exit;
    }

    memset(IV, 0, sizeof(IV));
    memset(digest, 0, sizeof(digest));
    memset(buffer, 0, sizeof(buffer));
    strcpy((char*)buffer, KEY);
    p = (char *)buffer;
    keylen = 0;

    while ( sscanf( p, "%02x", &n ) > 0 &&
            keylen < (int) sizeof( key ) ) {
        key[keylen++] = (unsigned char) n;
        p += 2;
    }

    memset(buffer, 0, sizeof(buffer));
    plaintext_size = strlen(plaintext);

    /*
     * Generate the initialization vector as:
     * IV = SHA-256( plaintext_size || plaintext )[0..15]
     */
    for ( i = 0; i < 8; i++ ) {
        buffer[i] = (unsigned char)( plaintext_size >> ( i << 3 ) );
    }

    p = (char *)plaintext;

    mbedtls_md_starts( &sha_ctx );
    mbedtls_md_update( &sha_ctx, buffer, 8 );
    mbedtls_md_update( &sha_ctx, (unsigned char *) p, plaintext_size );
    mbedtls_md_finish( &sha_ctx, digest );

    memcpy( IV, digest, 16 );

    /*
     * The last four bits in the IV are actually used
     * to store the file size module the AES block size.
     */
    lastn = (int)( plaintext_size & 0x0F );

    IV[15] = (unsigned char)
             ( ( IV[15] & 0xF0 ) | lastn );

    /*
     * Append the IV at the beginning of the output.
     */
    memcpy(output_buffer, IV, 16);
    output_offset = 16;
    /*
     * Hash the IV and the secret key together 8192 times
     * using the result to setup the AES context and HMAC.
     */
    memset( digest, 0,  32 );
    memcpy( digest, IV, 16 );

    for ( i = 0; i < 8192; i++ ) {
        mbedtls_md_starts( &sha_ctx );
        mbedtls_md_update( &sha_ctx, digest, 32 );
        mbedtls_md_update( &sha_ctx, key, keylen );
        mbedtls_md_finish( &sha_ctx, digest );
    }

    mbedtls_aes_setkey_enc( &aes_ctx, digest, 256 );
    mbedtls_md_hmac_starts( &sha_ctx, digest, 32 );

    /*
     * Encrypt and write the ciphertext.
     */
    for ( offset = 0; offset < plaintext_size; offset += 16 ) {
        n = ( plaintext_size - offset > 16 ) ? 16 : (int)
            ( plaintext_size - offset );
        memcpy(buffer, plaintext + offset, n);

        for ( i = 0; i < 16; i++ ) {
            buffer[i] = (unsigned char)( buffer[i] ^ IV[i] );
        }

        mbedtls_aes_crypt_ecb( &aes_ctx, MBEDTLS_AES_ENCRYPT, buffer, buffer );
        mbedtls_md_hmac_update( &sha_ctx, buffer, 16 );

        memcpy(output_buffer + output_offset, buffer, n);
        output_offset += n;

        memcpy( IV, buffer, 16 );
    }

    /*
     * Finally write the HMAC.
     */
    mbedtls_md_hmac_finish( &sha_ctx, digest );

    memcpy(output_buffer + output_offset, digest, 32);
    output_offset += 32;
    ciphertext_size = output_offset;
    memcpy(ciphertext, output_buffer, output_offset);

    LOG_I(mbedtls_proj, "Finish encrypting\n");
    LOG_I(mbedtls_proj, "Ciphertext:");
    for ( offset = 0; offset < ciphertext_size; offset++) {
        if (offset % 16 == 0) {
            LOG_I(mbedtls_proj, "\n");
        }
        LOG_I(mbedtls_proj, "%02X", ciphertext[offset]);
    }
    LOG_I(mbedtls_proj, "\n");

    LOG_I(mbedtls_proj, "Start to decrpt\n");
    /*
     *  The ciphertext is structured as follows:
     *
     *        00 .. 15              Initialization Vector
     *        16 .. 31              AES Encrypted Block #1
     *           ..
     *      N*16 .. (N+1)*16 - 1    AES Encrypted Block #N
     *  (N+1)*16 .. (N+1)*16 + 32   HMAC-SHA-256(ciphertext)
     */


    /*
     * Read the IV and original plaintext_size modulo 16.
     */
    memcpy( IV, ciphertext, 16 );
    lastn = IV[15] & 0x0F;

    /*
     * Hash the IV and the secret key together 8192 times
     * using the result to setup the AES context and HMAC.
     */
    memset( digest, 0,  32 );
    memcpy( digest, IV, 16 );

    for ( i = 0; i < 8192; i++ ) {
        mbedtls_md_starts( &sha_ctx );
        mbedtls_md_update( &sha_ctx, digest, 32 );
        mbedtls_md_update( &sha_ctx, key, keylen );
        mbedtls_md_finish( &sha_ctx, digest );
    }

    mbedtls_aes_setkey_dec( &aes_ctx, digest, 256 );
    mbedtls_md_hmac_starts( &sha_ctx, digest, 32 );

    /*
     * Subtract the HMAC length.
     */
    ciphertext_size -= 32;

    memset(output_buffer, 0, sizeof(output_buffer));
    output_offset = 0;
    /*
     * Decrypt and write the plaintext.
     */
    for ( offset = 16; offset < ciphertext_size; offset += 16 ) {
        memcpy(buffer, ciphertext + offset, 16);
        memcpy( tmp, buffer, 16 );

        mbedtls_md_hmac_update( &sha_ctx, buffer, 16 );
        mbedtls_aes_crypt_ecb( &aes_ctx, MBEDTLS_AES_DECRYPT, buffer, buffer );

        for ( i = 0; i < 16; i++ ) {
            buffer[i] = (unsigned char)( buffer[i] ^ IV[i] );
        }

        memcpy( IV, tmp, 16 );

        n = ( lastn > 0 && offset == ciphertext_size - 16 )
            ? lastn : 16;

        memcpy(output_buffer + output_offset, buffer, n);
        output_offset += n;
    }

    /*
     * Verify the message authentication code.
     */
    mbedtls_md_hmac_finish( &sha_ctx, digest );

    memcpy(buffer, ciphertext + ciphertext_size, 32);
    /* Use constant-time buffer comparison */
    diff = 0;
    for ( i = 0; i < 32; i++ ) {
        diff |= digest[i] ^ buffer[i];
    }

    if ( diff != 0 ) {
        LOG_I(mbedtls_proj, "HMAC check failed: wrong key, "
               "or file corrupted.\n" );
        ret = -1;
        goto exit;
    }
    LOG_I(mbedtls_proj, "Finish decrypting\n");
    LOG_I(mbedtls_proj, "The plainttext:\n");
    LOG_I(mbedtls_proj, "%s\n", output_buffer);

exit:

    mbedtls_aes_free( &aes_ctx );
    mbedtls_md_free( &sha_ctx );

    return ( ret );
}
