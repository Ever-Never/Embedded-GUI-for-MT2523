/*
 ---------------------------------------------------------------------------
 Copyright (c) 2002, Dr Brian Gladman, Worcester, UK.   All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products
      built using this software without specific written permission.

 ALTERNATIVELY, provided that this notice is retained in full, this product
 may be distributed under the terms of the GNU General Public License (GPL),
 in which case the provisions of the GPL apply INSTEAD OF those given above.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue Date: 01/08/2005
*/

#ifndef _SHA2_H
#define _SHA2_H

#define EXIT_SUCCESS    0
#define EXIT_FAILURE    1

// modify : define in "tdefs.h"
typedef unsigned char       uint_8t;
typedef unsigned short      uint_16t;
typedef unsigned int        uint_32t;
//typedef unsigned long       uint_32t;
typedef unsigned long long  uint_64t;


//#define void_ret void __romtext
//#define int_ret  int __romtext



#if 1   /* add for WSC CryptoLib ++ */

/* define the hash functions that you need  */
#define SHA_2   /* for dynamic hash length  */
#define SHA_224
#define SHA_256

#else
#include "tdefs.h"

#define SHA_64BIT

/* define the hash functions that you need  */
#define SHA_2   /* for dynamic hash length  */
#define SHA_224
#define SHA_256
#ifdef SHA_64BIT
#  define SHA_384
#  define SHA_512
#endif

#endif


/* Note that the following function prototypes are the same */
/* for both the bit and byte oriented implementations.  But */
/* the length fields are in bytes or bits as is appropriate */
/* for the version used.  Bit sequences are arrays of bytes */
/* in which bit sequence indexes increase from the most to  */
/* the least significant end of each byte                   */

#define SHA224_DIGEST_SIZE  28
#define SHA224_BLOCK_SIZE   64
#define SHA256_DIGEST_SIZE  32
#define SHA256_BLOCK_SIZE   64

#define SHA1_DIGEST_SIZE 20

/* type to hold the SHA256 (and SHA224) context */

typedef struct
{   uint_32t count[2];
    uint_32t hash[8];
    uint_32t wbuf[16];
} sha256_ctx;

typedef sha256_ctx  sha224_ctx;
#define sha224_hash sha256_hash

#if 0
void_ret sha256_compile(sha256_ctx ctx[1]);

void_ret sha224_begin(sha224_ctx ctx[1]);

void_ret sha224_end(unsigned char hval[], sha224_ctx ctx[1]);
void_ret sha224(unsigned char hval[], const unsigned char data[], unsigned int len);

void_ret sha256_begin(sha256_ctx ctx[1]);
void_ret sha256_hash(const unsigned char data[], unsigned int len, sha256_ctx ctx[1]);
void_ret sha256_end(unsigned char hval[], sha256_ctx ctx[1]);
void_ret sha256(unsigned char hval[], const unsigned char data[], unsigned int len);
#endif

#if 1 //ndef SHA_64BIT

typedef struct
{   union
    { sha256_ctx  ctx256[1];
    } uu[1];
    uint_32t    sha2_len;
} sha2_ctx;

#define SHA2_MAX_DIGEST_SIZE    SHA256_DIGEST_SIZE

#else

#define SHA384_DIGEST_SIZE  48
#define SHA384_BLOCK_SIZE  128
#define SHA512_DIGEST_SIZE  64
#define SHA512_BLOCK_SIZE  128
#define SHA2_MAX_DIGEST_SIZE    SHA512_DIGEST_SIZE

/* type to hold the SHA384 (and SHA512) context */

typedef struct
{   uint_64t count[2];
    uint_64t hash[8];
    uint_64t wbuf[16];
} sha512_ctx;

typedef sha512_ctx  sha384_ctx;

typedef struct
{   union
    { sha256_ctx  ctx256[1];
      sha512_ctx  ctx512[1];
    } uu[1];
    uint_32t    sha2_len;
} sha2_ctx;

void_ret sha512_compile(sha512_ctx ctx[1]);

void_ret sha384_begin(sha384_ctx ctx[1]);
#define sha384_hash sha512_hash
void_ret sha384_end(unsigned char hval[], sha384_ctx ctx[1]);
void_ret sha384(unsigned char hval[], const unsigned char data[], unsigned int len);

void_ret sha512_begin(sha512_ctx ctx[1]);
void_ret sha512_hash(const unsigned char data[], unsigned int len, sha512_ctx ctx[1]);
void_ret sha512_end(unsigned char hval[], sha512_ctx ctx[1]);
void_ret sha512(unsigned char hval[], const unsigned char data[], unsigned int len);

int_ret  sha2_begin(unsigned int size, sha2_ctx ctx[1]);
void_ret sha2_hash(const unsigned char data[], unsigned int len, sha2_ctx ctx[1]);
void_ret sha2_end(unsigned char hval[], sha2_ctx ctx[1]);
int_ret  sha2(unsigned char hval[], unsigned int size, const unsigned char data[], unsigned int len);

#endif


#endif
