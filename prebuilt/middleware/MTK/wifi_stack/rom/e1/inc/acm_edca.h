/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	acm_edca.h

	Abstract:
	EDCA Related Structure & Definition

	Revision History:
	Who			When			What
	----------	----------		----------------------------------------------
	Sample Lin	06-18-2007		Create first
*/

#ifndef __ACM_EDCA_H__
#define __ACM_EDCA_H__


/* ========================== Definition ==================================== */

#define ACM_IS_EDCA_STREAM(tid)			(tid <= 7)
#define ACM_MR_EDCA_AC(tid)				(ucA_EDCA_UP_AC[tid&0x07])
#define ACM_MR_EDCA_UP(tid)				(tid & 0x0f)
#define ACM_EDCA_MAX_UP					7 /* 0 ~ 7 */

/* get User Priority for the DSCP value */
#define ACM_MR_DSCP_MAPPING(dscp)		(ucA_EDCA_UP_DSCP[dscp])

#define ACM_EDCA_BE_AC_QUE_ID			0 /* UP = 0 or 3 */
#define ACM_EDCA_BK_AC_QUE_ID			1 /* UP = 1 or 2 */
#define ACM_EDCA_VI_AC_QUE_ID			2 /* UP = 4 or 5 */
#define ACM_EDCA_VO_AC_QUE_ID			3 /* UP = 6 or 7 */

#define ACM_EDCA_MAX_QSTA_DN_LINK		4 /* WMM requirement */
#define ACM_EDCA_MAX_QSTA_UP_LINK		4 /* WMM requirement */
#define ACM_EDCA_MAX_QSTA_TS_FOR_A_AC	1 /* WMM requirement */


/* ========================== Structure ===================================== */

typedef struct _ACM_ACI_AIFSN {

	UINT8 AIFSN:4; /* bit 0 */
	UINT8 ACM:1;
	UINT8 ACI:2;
	UINT8 reserved:1;
} PACKED ACM_ACI_AIFSN;

typedef struct _ACM_AC_PARAM_RECORD {

	ACM_ACI_AIFSN aci_aifsn; /* 1B */

	/* CWmax = 2^ECWmax - 1 */
	/* CWmin = 2^ECWmin - 1 */
	UINT8 ECWmin:4;
	UINT8 ECWmax:4;

	UINT16 txop_limit; /* in units of 32 microseconds */
} PACKED ACM_AC_PARAM_RECORD;

typedef struct _ACM_ELM_EDCA_PARAM {

	UINT8 element_id;
	UINT8 length;

	ACM_QOS_INFO qos_info; /* 1B */
	UINT8 reserved;

	ACM_AC_PARAM_RECORD ac_be;		/* best effort */
	ACM_AC_PARAM_RECORD ac_bk;		/* background */
	ACM_AC_PARAM_RECORD ac_vi;		/* video */
	ACM_AC_PARAM_RECORD ac_vo;		/* voice */
} PACKED ACM_ELM_EDCA_PARAM;


/* ----- WMM v1.1 ----- */
#ifdef ACM_CC_FUNC_WMM
typedef struct _ACM_WME_AC_INFO {

#ifndef APPS_INCLUDE_WLAN_11E_FINAL_EDCA_WMM_UAPSD
	UINT8 param_set_count:4;
	UINT8 reserved:4;
#else

	union {
		UINT8 info;

		struct _ap {
			UINT8 param_set_count:4;
			UINT8 reserved:3;
			UINT8 flag_uapsd:1;     /* 1: support U-APSD */
		} ap; /* QAP */

		struct _sta {
#define WME_UAPSD_STA_SUP_MASK	0x0F
			UINT8 flag_uapsd_vo:1;
			UINT8 flag_uapsd_vi:1;
			UINT8 flag_uapsd_bk:1;
			UINT8 flag_uapsd_be:1;
			UINT8 flag_qack:1;
			UINT8 max_sp_len:2;
			UINT8 flag_more_data_ack:1;
		} sta; /* QSTA */
	} op;

#define WME_UAPSD_OP_AP	op.ap
#define WME_UAPSD_OP_STA	op.sta
#endif
} PACKED ACM_WME_AC_INFO;

#define ACM_WME_OUI_HDR_LEN			6
#define ACM_WME_OUI_0				0x00
#define ACM_WME_OUI_1				0x50
#define ACM_WME_OUI_2				0xf2

#define ACM_WME_OUI_TYPE			0x02
#define ACM_WME_OUI_VERSION			0x01

#define ACM_WME_OUI_ID_OFFSET		6 /* sub ID field offset from element ID */

typedef struct _ACM_ELM_WME_INFO {

	UINT8 element_id;		/* IFX_WQOS_ELM_WME_ID */
	UINT8 length;			/* IFX_WQOS_ELM_WME_INFO_LEN */

	UINT8 oui[3];			/* 00:50:f2 (hex) */
	UINT8 oui_type;			/* ACM_WME_OUI_TYPE */
	UINT8 oui_subtype;		/* WS_WME_OUI_SUBTYPE_INFO */
	UINT8 version;			/* ACM_WME_OUI_VERSION */

	ACM_WME_AC_INFO ac_info;
} PACKED ACM_ELM_WME_INFO;

typedef struct _ACM_WME_ACI_AIFSN {

	UINT8 AIFSN:4;		/* bit 0 ~ 3 */
	UINT8 ACM:1;		/* bit 4 */
	/* -------------------------------------	*/
	/* ACI      AC      Access Category 		*/
	/* -------------------------------------	*/
	/* 00       AC_BE   Best Effort				*/
	/* 01       AC_BK   Background				*/
	/* 10       AC_VI   Video					*/
	/* 11       AC_VO   Voice					*/
	/* -------------------------------------	*/
	UINT8 ACI:2;		/* bit 5 ~ 6 */
	UINT8 reserved:1;	/* bit 7 */
} PACKED ACM_WME_ACI_AIFSN;

typedef struct _ACM_WME_AC_PARAM {

	ACM_WME_ACI_AIFSN aci_aifsn;

	UINT8 ECWmin:4;
	UINT8 ECWmax:4;

	UINT16 txop_limit;
} PACKED ACM_WME_AC_PARAM;

typedef struct _ACM_ELM_WME_PARAM {

	UINT8 element_id;		/* IFX_WQOS_ELM_WME_ID */
	UINT8 length;			/* IFX_WQOS_ELM_WME_PARAM_LEN */

	UINT8 oui[3];			/* 00:50:f2 (hex) */
	UINT8 oui_type;			/* ACM_WME_OUI_TYPE */
	UINT8 oui_subtype;		/* WS_WME_OUI_SUBTYPE_PARAM */
	UINT8 version;			/* ACM_WME_OUI_VERSION */

	ACM_WME_AC_INFO ac_info;
	UINT8 reserved;
	ACM_WME_AC_PARAM ac_be;
	ACM_WME_AC_PARAM ac_bk;
	ACM_WME_AC_PARAM ac_vi;
	ACM_WME_AC_PARAM ac_vo;
} PACKED ACM_ELM_WME_PARAM;

typedef struct _ACM_WME_TS_INFO {

	UINT8 zero2:1;			/* always 0 */
	UINT8 tid:4;
    /* -------------------------------	*/
    /* Direction	Meaning				*/
    /* -------------------------------	*/
    /* 00			Uplink				*/
    /* 01			Downlink			*/
    /* 10			Reserved			*/
    /* 11			Bi-directional		*/
    /* -------------------------------	*/
	/*	ACM_DIRECTION_UP_LINK
		ACM_DIRECTION_DOWN_LINK
		ACM_DIRECTION_BIDIREC_LINK		*/
	UINT16 direction:2;
	UINT8 one:1;				/* always 1 */

	UINT8 zero1:1;			    /* always 0 */
	UINT8 reserved3:1;
	UINT8 psb:1;
	UINT8 up:3;
	UINT8 reserved2:2;

	UINT8 reserved1;
} PACKED ACM_WME_TS_INFO;

typedef struct _ACM_WME_TSPEC {

	ACM_WME_TS_INFO ts_info;		/* 3B */

	UINT16  nominal_msdu_size;
	UINT16  max_msdu_size;      	/* dont care */

	UINT32  min_serv_int;			/* dont care */
	UINT32  max_serv_int;			/* dont care */
#define ACM_WME_TSPEC_INACTIVITY_DEFAULT		((UINT32)60000000)
	UINT32  inactivity_int;			/* must be assigned, if no, default 1 minute */
	UINT32  suspension_int;			/* dont care */
	UINT32  service_start_time;		/* dont care */
	UINT32  min_data_rate;			/* dont care */
	UINT32  mean_data_rate;
	UINT32  peak_data_rate;			/* dont care */
	UINT32  max_burst_size;			/* dont care */
	UINT32  delay_bound;			/* dont care */
	UINT32  min_phy_rate;

	UINT16  surplus_bandwidth_allowance;
	UINT16  medium_time;
} PACKED ACM_WME_TSPEC;

typedef struct _ACM_ELM_WME_TSPEC {

	UINT8 element_id;		/* IFX_WQOS_ELM_WME_ID */
	UINT8 length;			/* IFX_WQOS_ELM_WME_TSPEC_LEN */

	UINT8 oui[3];			/* 00:50:f2 (hex) */
	UINT8 oui_type;			/* ACM_WME_OUI_TYPE */
	UINT8 oui_subtype;		/* ACM_WME_OUI_SUBTYPE_TSPEC */
	UINT8 version;			/* ACM_WME_OUI_VERSION */

	ACM_WME_TSPEC tspec;
} PACKED ACM_ELM_WME_TSPEC;

typedef struct _ACM_ELM_WME_TCLAS {

	UINT8 element_id;		/* IFX_WQOS_ELM_WME_ID */
	UINT8 length;			/* IFX_WQOS_ELM_WME_TSPEC_LEN */

	UINT8 oui[3];			/* 00:50:f2 (hex) */
	UINT8 oui_type;		    /* ACM_WME_OUI_TYPE */
	UINT8 oui_subtype;	    /* ACM_WSM_OUI_SUBTYPE_TCLAS */
	UINT8 version;			/* ACM_WME_OUI_VERSION */

	ACM_TCLAS tclas;
} PACKED ACM_ELM_WME_TCLAS;

typedef struct _ACM_ELM_WME_TCLAS_PROCESSING {

	UINT8 element_id;		/* IFX_WQOS_ELM_WME_ID */
	UINT8 length;			/* IFX_WQOS_ELM_WME_TSPEC_LEN */

	UINT8 oui[3];			/* 00:50:f2 (hex) */
	UINT8 oui_type;	    	/* ACM_WME_OUI_TYPE */
	UINT8 oui_subtype;	    /* ACM_WSM_OUI_SUBTYPE_TCLAS_PROCESSING */
	UINT8 version;			/* ACM_WME_OUI_VERSION */

#define ACM_WME_TCLAS_PROCESSING_ALL		0
#define ACM_WME_TCLAS_PROCESSING_ONE		1
	UINT8 processing;
} PACKED ACM_ELM_WME_TCLAS_PROCESSING;

/* WME notification Frame */
typedef struct _ACM_WME_NOT_FRAME {

	/* 17: WME notification frame (ACM_CATEGORY_WME) */
	UINT8 category;

	/* 0: setup request (ACM_ACTION_WME_SETUP_REQ);
	   1: setup response (WS_ACTION_WME_SETUP_RSP);
	   2: teardown (ACM_ACTION_WME_TEAR_DOWN) */
	UINT8 action;

	/* For teardown frame, the field must be 0 */
	UINT8 dialog_token;

#define WLAN_STATUS_CODE_WME_ACM_ACCEPTED			0x00
#define WLAN_STATUS_CODE_WME_INVALID_PARAM		    0x01
#define WLAN_STATUS_CODE_WME_REFUSED				0x03
	/* For setup request or teardown frame, the field must be 0 */
	UINT8 status_code;

	ACM_ELM_WME_TSPEC elm_tspec;

	/* max 5 TCLASS & 1 TCLASS Processing or none */
	UINT8 tclasp[sizeof(ACM_ELM_WME_TCLAS) * ACM_TSPEC_TCLAS_MAX_NUM +
				sizeof(ACM_ELM_WME_TCLAS_PROCESSING)];

} PACKED ACM_WME_NOT_FRAME;
#endif // ACM_CC_FUNC_WMM //


/* ======================= EDCA Function definition ======================= */

#ifdef CONFIG_AP_SUPPORT
/* delete a lower ACM AC stream */
STATIC ACM_FUNC_STATUS ACM_EDCA_Lower_UP_Del(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT32				time_off);

/* handle a EDCA stream TSPEC */
STATIC UINT8 ACM_EDCA_ReqHandle(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB			*cdb_p,
	ACM_PARAM_IN	ACM_STREAM			*new_stream_p,
	ACM_PARAM_IN	ACM_STREAM			*old_stream_in_p,
	ACM_PARAM_IN	ACM_STREAM			*old_stream_out_p);
#endif // CONFIG_AP_SUPPORT //

/* update ACM used time of EDCA */
STATIC void ACM_EDCA_Param_ACM_Update(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT32				phy_tsid,
	ACM_PARAM_IN	UINT8				direction,
	ACM_PARAM_IN	UINT8				up,
	ACM_PARAM_IN	UINT32				new_time,
	ACM_PARAM_IN	UINT32				old_time,
	ACM_PARAM_IN	UINT32				datl_ac_id,
	ACM_PARAM_IN	UINT32				datl_bandwidth);

/* reclaim EDCA used time */
STATIC void ACM_EDCA_STM_Del(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*stream_p);

/* translate factor decimal part binary to decimal */
STATIC UINT32 ACM_SurplusFactorDecimalBin2Dec(
	ACM_PARAM_IN	UINT32				bin);

/* translate factor decimal part decimal to binary */
STATIC UINT32 ACM_SurplusFactorDecimalDec2Bin(
	ACM_PARAM_IN	UINT32				dec);


/* ======================= WMM  Function definition ======================= */
#ifdef ACM_CC_FUNC_WMM /* -------------------------------------------------------- */

/* ----- COMMON ----- */
/* translate 11e status code to WME status code */
STATIC UINT8 ACM_11E_WMM_StatusTranslate(
	ACM_PARAM_IN	UINT8				status_code);

/* translate WME TSPEC & TCLAS to 11e TSPEC & TCLAS */
STATIC ACM_FUNC_STATUS ACM_WME_11E_TSPEC_TCLAS_Translate(
	ACM_PARAM_IN	ACM_WME_NOT_FRAME	*not_p,
	ACM_PARAM_IN	UINT32				body_len,
	ACM_PARAM_IN	ACM_TSPEC			*etspec_p,
	ACM_PARAM_IN	ACM_TCLAS			**tclas_p,
	ACM_PARAM_IN	UINT32				*tclas_num_p,
	ACM_PARAM_IN	UINT8				*tclas_processing_p);

/* translate WME TSPEC to 11e TSPEC */
STATIC ACM_FUNC_STATUS ACM_WME_11E_TSPEC_Translate(
	ACM_PARAM_IN	ACM_WME_TSPEC		*wtspec_p,
	ACM_PARAM_IN	ACM_TSPEC			*etspec_p);

/* make a WME action frame body */
STATIC UINT32 ACM_WME_ActionFrameBodyMake(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*stream_p,
	ACM_PARAM_IN	UINT8				*pkt_p,
	ACM_PARAM_IN	UINT8				action,
	ACM_PARAM_IN	UINT8				status_code);

/* handle a WME action frame */
STATIC void ACM_WME_ActionHandle(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB		    *cdb_p,
	ACM_PARAM_IN	UINT8				*body_p,
	ACM_PARAM_IN	UINT32				body_len,
	ACM_PARAM_IN	UINT32				phy_rate,
	ACM_PARAM_IN	UINT8				action);

/* ----- QAP ----- */
#ifdef CONFIG_AP_SUPPORT
/* send a WME Setup Response frame to the QSTA */
STATIC ACM_FUNC_STATUS EACM_WME_SETUP_RspSend(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*new_req_p,
	ACM_PARAM_IN	UINT8				status_code);

/* send a WME Teardown frame to the QSTA */
STATIC ACM_FUNC_STATUS EACM_WME_TeardownSendToQSTA(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB		    *cdb_p,
	ACM_PARAM_IN	ACM_STREAM			*stream_p);
#endif // CONFIG_AP_SUPPORT //


/* ----- QSTA ----- */
#if defined(CONFIG_STA_SUPPORT_SIM) || defined(CONFIG_STA_SUPPORT)

/* send a WME Setup Request frame to the QAP */
STATIC ACM_FUNC_STATUS EACM_WME_SETUP_ReqSend(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB		    *cdb_p,
	ACM_PARAM_IN	ACM_STREAM			*new_req_p);

#endif // CONFIG_STA_SUPPORT //


#ifdef CONFIG_STA_SUPPORT

/* send a WME Teardown frame to the QAP */
STATIC ACM_FUNC_STATUS EACM_WME_TeardownSendToQAP(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB		    *cdb_p,
	ACM_PARAM_IN	ACM_STREAM			*stream_p);

#endif // CONFIG_STA_SUPPORT //

#endif /* ACM_CC_FUNC_WMM */ /* =============================================== */

#endif // __ACM_EDCA_H__

/* End of acm_edca.h */

