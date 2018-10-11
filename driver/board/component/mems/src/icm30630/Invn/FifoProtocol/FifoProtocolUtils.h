/*
* ________________________________________________________________________________________________________
* Copyright (c) 2014-2015 InvenSense Inc. Portions Copyright (c) 2014-2015 Movea. All rights reserved.
*
* This software, related documentation and any modifications thereto (collectively "Software") is subject
* to InvenSense and its licensors' intellectual property rights under U.S. and international copyright and
* other intellectual property rights laws.
*
* InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
* and any use, reproduction, disclosure or distribution of the Software without an express license
* agreement from InvenSense is strictly prohibited.
*
* EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE SOFTWARE IS
* PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
* EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
* INVENSENSE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY
* DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
* NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
* OF THE SOFTWARE.
* ________________________________________________________________________________________________________
*/
 
/** @defgroup 	FifoProtocolUtils	 FifoProtocolUtils
	@brief 		Helper macros to perform basic stuff
	@ingroup 	FifoProtocolAdapter
    @{
*/
 
#ifndef _FIFO_PROTOCOL_UTILS_H
#define _FIFO_PROTOCOL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>



// fixed-point helper conversion functions
#define 	FIFOPROTOCOL_UFIX_TO_FLT(value, shift)	( (float) (uint32_t)(value) / (float)(1ULL << (shift)) )

#define 	FIFOPROTOCOL_FLT_TO_SFIX(value, shift)	( (int32_t)  ((float)(value)*(1ULL << (shift)) + ( (value>=0)-0.5f )) )
#define 	FIFOPROTOCOL_SFIX_TO_FLT(value, shift)	( (float)  (int32_t)(value) / (float)(1ULL << (shift)) )

#define 	FIFOPROTOCOL_FLT_TO_SFIX16(value, shift)	( (int16_t)  ((float)(value)*(1ULL << (shift)) + ( (value>=0)-0.5f )) )
#define 	FIFOPROTOCOL_SFIX16_TO_FLT(value, shift)	( (float)  (int16_t)(value) / (float)(1ULL << (shift)) )

#define 	FIFOPROTOCOL_FLT_TO_UFIX16(value, shift)	( (uint16_t) ((float)(value)*(1ULL << (shift)) + 0.5f) )
#define 	FIFOPROTOCOL_UFIX16_TO_FLT(value, shift)	( (float) (uint16_t)(value) / (float)(1ULL << (shift)) )



// endianness conversion functions
#ifndef FIFOPROTOCOL_LITTLE_ENDIAN_MACHINE
#define FIFOPROTOCOL_LITTLE_ENDIAN_MACHINE		1		// defines the endianness of the target machine (defaults to 1, currently)
#endif

#if(FIFOPROTOCOL_LITTLE_ENDIAN_MACHINE == 1)
// little endian implementation (LE/LE: no-op)
#define FIFOPROTOCOL_TO_LITTLE_ENDIAN_16(x)		x
#define FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(x)		x
#define FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(x)	x
#define FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(x)	x
#else // FIFOPROTOCOL_LITTLE_ENDIAN_MACHINE
// big endian implementation (BE/LE)
#define FIFOPROTOCOL_TO_LITTLE_ENDIAN_16(x)		((((((x) & 0xFFFF)) & 0x00FF) << 8) | ((((x) & 0xFFFF)) >> 8))
#define FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(x)		((((x) & 0x000000FF) << 24) | (((x) & 0x0000FF00) << 8) | (((x) & 0x00FF0000) >> 8) | (((x) & 0xFF000000) >> 24))
#define FIFOPROTOCOL_FROM_LITTLE_ENDIAN_16(x)	FIFOPROTOCOL_TO_LITTLE_ENDIAN_16(x)
#define FIFOPROTOCOL_FROM_LITTLE_ENDIAN_32(x)	FIFOPROTOCOL_TO_LITTLE_ENDIAN_32(x)
#endif // FIFOPROTOCOL_LITTLE_ENDIAN_MACHINE

#ifndef FIFOPROTOCOL_MEMCPY
#define FIFOPROTOCOL_MEMCPY(dest, src, len) FifoProtocol_memcpy((dest), (src), (len))
#endif

static inline void *FifoProtocol_memcpy(void *dest, const void *src, size_t n)
{
	size_t i;
	const uint8_t *s = (const uint8_t *)src;
	uint8_t *d = (uint8_t *)dest;

	for (i = 0; i < n; ++i) {
		d[i] = s[i];
	}

	return dest;
}

#ifdef __cplusplus
}
#endif

#endif /* _FIFO_PROTOCOL_UTILS_H */

/** @} */
