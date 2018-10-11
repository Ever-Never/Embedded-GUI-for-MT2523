/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    rtmp_type.h

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Paul Lin    1-2-2004
*/
#ifndef __RTMP_TYPE_H__
#define __RTMP_TYPE_H__

//#include "gl_typedef.h"
//#include "gl_typedef_ram.h"
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#include "rom/type_def.h"
#include "rom/string.h"
#if 0
// Ralink timer control block
typedef struct  _RALINK_TIMER_STRUCT    {
	BOOLEAN				Valid;			// Set to True when call RTMPInitTimer
    NDIS_MINIPORT_TIMER TimerObj;       // Ndis Timer object
    ULONG               TimerValue;     // Timer value in milliseconds
    BOOLEAN             State;          // True if timer cancelled
    BOOLEAN             Repeat;         // True if periodic timer
}   RALINK_TIMER_STRUCT, *PRALINK_TIMER_STRUCT;
#endif			

#ifdef IAR_COMPILE
PACKED typedef struct	 _RSN_IE_HEADER_STRUCT	{
#else
typedef struct	PACKED _RSN_IE_HEADER_STRUCT	{
#endif
	UCHAR		Eid;
	UCHAR		Length;
	USHORT		Version;	// Little endian format
}	RSN_IE_HEADER_STRUCT, *PRSN_IE_HEADER_STRUCT;

// Cipher suite selector types
#ifdef IAR_COMPILE
PACKED typedef struct	 _CIPHER_SUITE_STRUCT		{
#else
typedef struct	PACKED _CIPHER_SUITE_STRUCT		{
#endif
	UCHAR		Oui[3];
	UCHAR		Type;
}	CIPHER_SUITE_STRUCT, *PCIPHER_SUITE_STRUCT;

// Authentication and Key Management suite selector
#ifdef IAR_COMPILE
PACKED typedef struct	 _AKM_SUITE_STRUCT		{
#else
typedef struct	PACKED _AKM_SUITE_STRUCT		{
#endif
	UCHAR		Oui[3];
	UCHAR		Type;
}	AKM_SUITE_STRUCT, *PAKM_SUITE_STRUCT;

// RSN capability
#ifdef IAR_COMPILE
PACKED typedef struct	 _RSN_CAPABILITY		{
#else
typedef struct	PACKED _RSN_CAPABILITY		{
#endif
	USHORT		Rsv:10;
	USHORT		GTKSAReplayCnt:2;
	USHORT		PTKSAReplayCnt:2;
	USHORT		NoPairwise:1;
	USHORT		PreAuth:1;
}	RSN_CAPABILITY, *PRSN_CAPABILITY;

#endif  // __RTMP_TYPE_H__

