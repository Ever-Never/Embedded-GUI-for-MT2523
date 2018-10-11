/*
 *  md5 demonstration program
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
#include "mbedtls/md5.h"


int md5_main( void )
{
    int i;
    unsigned char digest[16];
    const char str[] = "Hello, world!";

    LOG_I(mbedtls_proj, "\n  MD5('%s') = ", str );

    mbedtls_md5( (unsigned char *) str, 13, digest );

    for ( i = 0; i < 16; i++ ) {
        LOG_I(mbedtls_proj, "%02x", digest[i] );
    }

    LOG_I(mbedtls_proj, "\n\n" );

    return ( 0 );
}
