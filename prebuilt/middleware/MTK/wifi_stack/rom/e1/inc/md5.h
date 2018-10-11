/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************
	Module Name:
	md5.h

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Name		Date			Modification logs
	jan			10-28-03		Initial
	Rita    	11-23-04		Modify MD5 and SHA-1
*/
#ifndef	uint8
#define	uint8  unsigned	char
#endif

#ifndef	uint32
#define	uint32 unsigned	long int
#endif


#ifndef	__MD5_H__
#define	__MD5_H__

#define MD5_DIGEST_SIZE   16

#define MD5_MAC_LEN 16

typedef struct _MD5_CTX {
    ULONG   Buf[4];             // buffers of four states
	UCHAR   Input[64];          // input message
	ULONG   LenInBitCount[2];   // length counter for input message, 0 up to 64 bits	                            
}   MD5_CTX;



void md5_mac(u8	*key, size_t key_len, u8 *data,	size_t data_len, u8	*mac);
//void hmac_md5(u8 *key,	size_t key_len,	u8 *data, size_t data_len, u8 *mac);

//
// SHA context
//
typedef	struct _SHA_CTX
{
	ULONG   Buf[5];             // buffers of five states
	UCHAR   Input[80];          // input message
	ULONG   LenInBitCount[2];   // length counter for input message, 0 up to 64 bits
	
}	SHA_CTX;


#define SHA_DIGEST_LEN 20
#endif // __MD5_H__

/******************************************************************************/
#ifndef	_AES_H
#define	_AES_H

typedef	struct
{
	uint32 erk[64];		/* encryption round	keys */
	uint32 drk[64];		/* decryption round	keys */
	int	nr;				/* number of rounds	*/
}
aes_context;

void F(char *password, unsigned char *ssid, int ssidlength, int iterations, int count, unsigned char *output);

#endif /* aes.h	*/

