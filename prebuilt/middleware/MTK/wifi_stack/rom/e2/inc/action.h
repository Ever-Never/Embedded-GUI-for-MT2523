/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	aironet.h

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Name		Date			Modification logs
	Paul Lin	04-06-15		Initial
*/

#ifndef	__ACTION_H__
#define	__ACTION_H__
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#ifdef IAR_COMPILE
PACKED typedef struct  __HT_INFO_OCTET {
#else
typedef struct PACKED __HT_INFO_OCTET {
#endif
#ifdef BIG_ENDIAN
	UCHAR	Reserved:5;
	UCHAR 	STA_Channel_Width:1;
	UCHAR	Forty_MHz_Intolerant:1;
	UCHAR	Request:1;
#else
	UCHAR	Request:1;
	UCHAR	Forty_MHz_Intolerant:1;
	UCHAR 	STA_Channel_Width:1;
	UCHAR	Reserved:5;
#endif
} HT_INFORMATION_OCTET;

#ifdef IAR_COMPILE
PACKED typedef struct  __FRAME_HT_INFO {
#else
typedef struct PACKED __FRAME_HT_INFO {
#endif
	HEADER_802_11   		Hdr;
	UCHAR					Category;
	UCHAR					Action;
	HT_INFORMATION_OCTET	HT_Info;
}   FRAME_HT_INFO, *PFRAME_HT_INFO;

#ifdef IAR_COMPILE
PACKED typedef struct  _CH_SW_ANN_INFO {
#else
typedef struct PACKED _CH_SW_ANN_INFO {
#endif
	UINT8 ChSwMode;
	UINT8 Channel;
	UINT8 ChSwCnt;
} CH_SW_ANN_INFO, *PCH_SW_ANN_INFO;

#endif /* __ACTION_H__ */


