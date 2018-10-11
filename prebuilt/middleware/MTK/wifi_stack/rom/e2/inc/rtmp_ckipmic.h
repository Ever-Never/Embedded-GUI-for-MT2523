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
	rtmp_ckipmic.h

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Name		Date			Modification logs
*/
#ifndef	__RTMP_CKIPMIC_H__
#define	__RTMP_CKIPMIC_H__

typedef	struct	_MIC_CONTEXT	{
	/* --- MMH context                            */
	UCHAR		CK[16];				/* the key                                    */
	UCHAR		coefficient[16];	/* current aes counter mode coefficients      */
	ULONGLONG	accum;				/* accumulated mic, reduced to u32 in final() */
	UINT		position;			/* current position (byte offset) in message  */
	UCHAR		part[4];			/* for conversion of message to u32 for mmh   */
}	MIC_CONTEXT, *PMIC_CONTEXT;
#if 0
VOID	CKIP_key_permute(
	OUT	UCHAR	*PK,			/* output permuted key */
	IN	UCHAR	*CK,			/* input CKIP key */
	IN	UCHAR	toDsFromDs,		/* input toDs/FromDs bits */
	IN	UCHAR	*piv);			/* input pointer to IV */

VOID	RTMPCkipMicInit(    
	IN	PMIC_CONTEXT		pContext,
	IN	PUCHAR				CK);

VOID RTMPMicUpdate(
    IN  PMIC_CONTEXT        pContext, 
    IN  PUCHAR              pOctets, 
    IN  INT                 len);

ULONG RTMPMicGetCoefficient(
    IN  PMIC_CONTEXT         pContext);

VOID xor_128(
    IN  PUCHAR              a,
    IN  PUCHAR              b,
    OUT PUCHAR              out);

UCHAR RTMPCkipSbox(
    IN  UCHAR               a);

VOID xor_32(
    IN  PUCHAR              a,
    IN  PUCHAR              b,
    OUT PUCHAR              out);

VOID next_key(
    IN  PUCHAR              key,
    IN  INT                 round);

VOID byte_sub(
    IN  PUCHAR              in,
    OUT PUCHAR              out);

VOID shift_row(
    IN  PUCHAR              in,
    OUT PUCHAR              out);

VOID mix_column(
    IN  PUCHAR              in,
    OUT PUCHAR              out);

VOID RTMPAesEncrypt(
    IN  PUCHAR              key,
    IN  PUCHAR              data,
    IN  PUCHAR              ciphertext);

VOID RTMPMicFinal(
    IN  PMIC_CONTEXT        pContext,
    OUT UCHAR               digest[4]);

VOID RTMPCkipInsertCMIC(
    IN  PRTMP_ADAPTER   pAd,
    OUT PUCHAR          pMIC,
    IN  PUCHAR          p80211hdr,
    IN  PNDIS_PACKET    pPacket,
    IN  PCIPHER_KEY     pKey,
    IN  PUCHAR          mic_snap);
#endif
#endif //__RTMP_CKIPMIC_H__
