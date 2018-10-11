/*
 *  Declaration of mbedtls releated APIs in MBEDTLS project
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
 */

#ifndef __MBEDTLS_EXAMPLE_H__
#define __MBEDTLS_EXAMPLE_H__


/**
* @brief       This function tests AES encryption and decryption.
* @return     0, if OK.\n
*                 Error code, if errors occurred.\n
*/
int aes_main( void );

/**
* @brief       This function tests MD5.
* @return     0, if OK.\n
*                 Error code, if errors occurred.\n
*/
int md5_main( void );

/**
* @brief       This function tests MPI.
* @return     0, if OK.\n
*                 Error code, if errors occurred.\n
*/
int mpi_demo_main( void );

/**
* @brief          This function tests private key decryption.
* @param[in]   enc_buf: Encrypted data.
* @param[in]   enc_len: The size of enc_buf.
* @param[in]   key_buf: Private key.
* @param[in]   key_len: The size of key_buf.
* @return        0, if OK.\n
*                    Error code, if errors occurred.\n
*/
int pk_decrypt_main(char *enc_buf, size_t enc_len, char *key_buf, size_t key_len);

/**
* @brief           This function tests public key encryption.
* @param[in]    input: Message to encrypt.
* @param[out]  output: Encrypted output.
* @param[out]  olen: Encrypted output length.
* @param[in]    output_buf_len: Size of the output buffer.
* @param[in]    key_buf: Public key.
* @param[in]    key_len: The size of key_buf.
* @return         0, if OK.\n
*                     Error code, if errors occurred.\n
*/
int pk_encrypt_main( char *input, char *output, size_t *olen, size_t output_buf_len, char *key_buf, size_t key_len );

/**
* @brief          This function tests RSA decryption.
* @param[in]   input: Encrypted data.
* @return        0, if OK.\n
*                    Error code, if errors occurred.\n
*/
int rsa_decrypt_main( char *input );

/**
* @brief           This function tests RSA encryption.
* @param[in]    input: Message to encrypt.
* @param[out]  output: Encrypted output.
* @return         0, if OK.\n
*                     Error code, if errors occurred.\n
*/
int rsa_encrypt_main( char *input, char *output );

/**
* @brief           This function calls mbedtls' selftest functions to test MD5, SHA256, SHA512, AES, DES, GCM, and so on.
* @param[in]    argc: Argument counter.
* @param[in]    argv: Argument vector.
* @return         0, if OK.\n
*                     Error code, if errors occurred.\n
*/
int tls_selftest_main( int argc, char *argv[] );

/**
* @brief           This function is a demonstration of ssl client.
* @return         0, if OK.\n
*                     Error code, if errors occurred.\n
*/
int ssl_client_main(void);

#endif /* __MBEDTLS_EXAMPLE_H__ */
