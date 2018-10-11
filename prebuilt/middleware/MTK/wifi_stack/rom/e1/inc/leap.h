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
	leap.h

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Name		Date			Modification logs
*/
#ifndef __LEAP_H__
#define __LEAP_H__

// Messages for Associate state machine
#define LEAP_MACHINE_BASE                   30

#define LEAP_MSG_REQUEST_IDENTITY           31
#define LEAP_MSG_REQUEST_LEAP               32
#define LEAP_MSG_SUCCESS                    33
#define LEAP_MSG_FAILED                     34
#define LEAP_MSG_RESPONSE_LEAP              35
#define LEAP_MSG_EAPOLKEY                   36
#define LEAP_MSG_UNKNOWN                    37
#define LEAP_MSG                            38
//! assoc state-machine states
#define LEAP_IDLE                           0
#define LEAP_WAIT_IDENTITY_REQUEST          1
#define LEAP_WAIT_CHANLLENGE_REQUEST        2
#define LEAP_WAIT_SUCCESS                   3
#define LEAP_WAIT_CHANLLENGE_RESPONSE       4
#define LEAP_WAIT_EAPOLKEY                  5

#define LEAP_REASON_INVALID_AUTH                    0x01
#define LEAP_REASON_AUTH_TIMEOUT                    0x02
#define LEAP_REASON_CHALLENGE_FROM_AP_FAILED        0x03
#define LEAP_REASON_CHALLENGE_TO_AP_FAILED          0x04

#define CISCO_AuthModeLEAP                          0x80
#define CISCO_AuthModeLEAPNone                      0x00
#define LEAP_AUTH_TIMEOUT                           30000
#define LEAP_CHALLENGE_RESPONSE_LENGTH              24
#define LEAP_CHALLENGE_REQUEST_LENGTH               8

typedef struct _LEAP_EAPOL_HEADER_ {
    UCHAR       Version;
    UCHAR       Type;
    UCHAR       Length[2];
} LEAP_EAPOL_HEADER, *PLEAP_EAPOL_HEADER;

typedef struct _LEAP_EAPOL_PACKET_ {
    UCHAR       Code;
    UCHAR       Identifier;
    UCHAR       Length[2];
    UCHAR       Type;
} LEAP_EAPOL_PACKET, *PLEAP_EAPOL_PACKET;

typedef struct _LEAP_EAP_CONTENTS_ {
    UCHAR       Version;
    UCHAR       Reserved;
    UCHAR       Length;
} LEAP_EAP_CONTENTS, *PLEAP_EAP_CONTENTS;

/*** EAPOL key ***/
typedef struct _EAPOL_KEY_HEADER_ {
    UCHAR       Type;
    UCHAR       Length[2];
    UCHAR       Counter[8];
    UCHAR       IV[16];
    UCHAR       Index;
    UCHAR       Signature[16];
} EAPOL_KEY_HEADER, *PEAPOL_KEY_HEADER;

BOOLEAN LeapMsgTypeSubst(
    IN  UCHAR   EAPType,
    OUT ULONG   *MsgType);

VOID LeapMachinePerformAction(
    IN PRTMP_ADAPTER    pAd,
    IN STATE_MACHINE    *S,
    IN MLME_QUEUE_ELEM  *Elem);

VOID LeapMacHeaderInit(
    IN  PRTMP_ADAPTER       pAd,
    IN  OUT PHEADER_802_11  pHdr80211,
    IN  UCHAR               wep,
    IN  PUCHAR              pAddr3);

VOID LeapStartAction(
    IN PRTMP_ADAPTER    pAd, 
    IN MLME_QUEUE_ELEM  *Elem);

VOID LeapIdentityAction(
    IN PRTMP_ADAPTER    pAd, 
    IN MLME_QUEUE_ELEM  *Elem);

VOID LeapPeerChallengeAction(
    IN PRTMP_ADAPTER    pAd, 
    IN MLME_QUEUE_ELEM  *Elem);

VOID HashPwd(
    IN  PUCHAR  pwd,
    IN  INT     pwdlen,
    OUT PUCHAR  hash);

VOID PeerChallengeResponse(
    IN  PUCHAR  szChallenge,
    IN  PUCHAR  smbPasswd,
    OUT PUCHAR  szResponse);

VOID ParityKey(
    OUT PUCHAR  szOut,
    IN  PUCHAR  szIn);

VOID DesKey(
    OUT ULONG   k[16][2],
    IN  PUCHAR  key,
    IN  INT     decrypt);

VOID Des(
    IN  ULONG   ks[16][2],
    OUT UCHAR   block[8]);

VOID DesEncrypt(
    IN  PUCHAR  szClear,
    IN  PUCHAR  szKey,
    OUT PUCHAR  szOut);

VOID LeapNetworkChallengeAction(
    IN PRTMP_ADAPTER    pAd, 
    IN MLME_QUEUE_ELEM  *Elem);

VOID LeapNetworkChallengeResponse(
    IN PRTMP_ADAPTER    pAd, 
    IN MLME_QUEUE_ELEM  *Elem);

VOID HashpwdHash(
    IN  PUCHAR  hash,
    IN  PUCHAR  hashhash);

VOID ProcessSessionKey(
    OUT PUCHAR  SessionKey,
    IN  PUCHAR  hash2, 
    IN  PUCHAR  ChallengeToRadius, 
    IN  PUCHAR  ChallengeResponseFromRadius,
    IN  PUCHAR  ChallengeFromRadius, 
    IN  PUCHAR  ChallengeResponseToRadius);

VOID LeapEapolKeyAction(
    IN PRTMP_ADAPTER    pAd, 
    IN MLME_QUEUE_ELEM  *Elem);

VOID RogueApTableInit(
    IN ROGUEAP_TABLE    *Tab);

ULONG RogueApTableSearch(
    IN ROGUEAP_TABLE    *Tab,
    IN PUCHAR           pAddr);

VOID RogueApEntrySet(
    IN  PRTMP_ADAPTER   pAd, 
    OUT ROGUEAP_ENTRY   *pRogueAp, 
    IN PUCHAR           pAddr, 
    IN UCHAR            FaileCode);

ULONG RogueApTableSetEntry(
    IN  PRTMP_ADAPTER   pAd,
    OUT ROGUEAP_TABLE  *Tab,
    IN PUCHAR           pAddr,
    IN UCHAR            FaileCode);

VOID RogueApTableDeleteEntry(
    IN OUT ROGUEAP_TABLE *Tab, 
    IN PUCHAR          pAddr);

VOID LeapAuthTimeout(
    IN PVOID SystemSpecific1,
    IN PVOID FunctionContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3);

VOID LeapSendRogueAPReport(
    IN  PRTMP_ADAPTER   pAd);

BOOLEAN CCKMAssocRspSanity(
    IN PRTMP_ADAPTER    pAd,
    IN VOID             *Msg,
    IN ULONG            MsgLen);

#endif  // __LEAP_H__
