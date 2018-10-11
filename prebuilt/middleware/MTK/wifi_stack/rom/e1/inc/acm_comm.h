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
	acm_comm.h

	Abstract:
	ACM Related Structure & Definition

	Revision History:
	Who			When			What
	----------	----------		----------------------------------------------
	Sample Lin	06-18-2007		Create first
*/

#ifndef __ACM_COMM_H__
#define __ACM_COMM_H__


/* -------------------------------------------------------------------------

	ACM			Adimission Control Mechanism
	ACM_TC		ACM TSPEC
	ACM_TS		ACM TCLASS
	ACM_SM		ACM STREAM
	ACM_RM		ACM RENAME
	EACM		Extensible ACM, means depended on different WLAN module design

   ------------------------------------------------------------------------- */




/* ========================================================================== */
/* Definition */

/* ----- General ----- */
	/*	1. maximum number of kept fail TSPECs;
		2. maximum retry number of delts frame; */
#define ACM_MAX_NUM_OF_FAIL_RSV_TSPEC	5
#define ACM_MAX_NUM_OF_DELTS_RETRY		5

#define ACM_STREAM_CHECK_BASE			100000	/* unit: 100 mini second */
#define ACM_STREAM_CHECK_OFFSET			(ACM_STREAM_CHECK_BASE/ACM_JIFFIES_BASE)

#define ACM_SECOND_BASE					((UINT32)1000000) /* unit: micro sec */
#define ACM_TIME_BASE					((UINT32)1000000) /* unit: micro sec */

#define ACM_FLG_FUNC_SUPPORT			1
#define ACM_FLG_FUNC_NOT_SUPPORT		0

#define ACM_FLG_FUNC_ENABLED			1
#define ACM_FLG_FUNC_DISABLED			0

#define ACM_ETH_DA_ADDR_LEN				6
#define ACM_ETH_SA_ADDR_LEN				6
#define ACM_ETH_TYPE_LEN				2

#define ACM_ELM_ID_LEN_SIZE				2 /* size of ID + LENGTH field */


/* ----- Rate & Time ----- */
/* for PLCP duration table access, Rate_Legacy[] & Rate_G[] */
#define ACM_RATE_ID_54M					0
#define ACM_RATE_ID_48M					1
#define ACM_RATE_ID_36M					2
#define	ACM_RATE_ID_24M					3
#define ACM_RATE_ID_18M					4
#define ACM_RATE_ID_12M					5
#define ACM_RATE_ID_9M					6
#define	ACM_RATE_ID_6M					7

#define ACM_RATE_ID_11M					0
#define ACM_RATE_ID_5_5M				1
#define ACM_RATE_ID_2M					2
#define	ACM_RATE_ID_1M					3

/* rate value (unit: 2) */
#define ACM_RATE_54M					0x6c	/* 108 */
#define ACM_RATE_48M					0x60	/* 96 */
#define ACM_RATE_36M					0x48	/* 72 */
#define ACM_RATE_24M					0x30	/* 66 */
#define ACM_RATE_18M					0x24	/* 36 */
#define ACM_RATE_12M					0x18	/* 24 */
#define ACM_RATE_9M						0x12	/* 18 */
#define ACM_RATE_6M						0x0c	/* 12 */
#define ACM_RATE_11M					0x16	/* 22 */
#define ACM_RATE_5_5M					0x0b	/* 11 */
#define ACM_RATE_2M						0x04	/* 4 */
#define ACM_RATE_1M						0x02	/* 2 */

/* for Pre-Time calculation use, ACM_TX_TimeCalPre() */
#define ACM_PRE_TIME_ID_1M				0
#define ACM_PRE_TIME_ID_2M				1
#define ACM_PRE_TIME_ID_5_5M			2
#define ACM_PRE_TIME_ID_11M				3
#define ACM_PRE_TIME_ID_6M				4
#define ACM_PRE_TIME_ID_9M				5
#define ACM_PRE_TIME_ID_12M				6
#define ACM_PRE_TIME_ID_18M				7
#define ACM_PRE_TIME_ID_24M				8
#define ACM_PRE_TIME_ID_36M				9
#define ACM_PRE_TIME_ID_48M				10
#define ACM_PRE_TIME_ID_54M				11

#define ACM_PRE_TIME_SPREAMBLE			0
#define ACM_PRE_TIME_LPREAMBLE			1

#define ACM_PRE_TIME_CTS_SELF			1
#define ACM_PRE_TIME_RTS_CTS			2
#define ACM_PRE_TIME_HEADER				3
#define ACM_PRE_TIME_ACK				4

/* in software ACM, we partition max packet size to
	0 ~ 31B, 32 ~ 63B, .....
	i.e. the tx time of 0B is same as the tx time of 31B */
#define ACM_PRE_TIME_DATA_SIZE_OFFSET	5
#define ACM_PRE_TIME_DATA_SIZE_INTERVAL	(1<<ACM_PRE_TIME_DATA_SIZE_OFFSET)
#define ACM_PRE_TIME_DATA_SIZE_NUM		(ACMR_MAX_BUF_SIZE/ACM_PRE_TIME_DATA_SIZE_OFFSET+1)

/* Time value, unit: micro second */
#define TIME_UNIT					1024
#define TIME_LONG_PREAMBLE			0xC0
#define TIME_SHORT_PREAMBLE			0x60
#define TIME_LONG_PREAMBLEx2		0x180
#define TIME_SHORT_PREAMBLEx2		0xC0
#define TIME_LONG_PREAMBLEx3		0x240
#define TIME_SHORT_PREAMBLEx3		0x120
#define TIME_PREAMBLE_11G			0x14
#define TIME_PREAMBLE_11Gx2			0x28
#define TIME_PREAMBLE_11Gx3			0x3C /* for 11g */
#define TIME_PREAMBLE_DIFF			(TIME_LONG_PREAMBLEx3 - TIME_SHORT_PREAMBLEx3)
#define TIME_SIFS					0x0A 
#define TIME_SIFSx2					0x14 
#define TIME_SIFSx3					0x1E
#define TIME_ACK_1Mbps				0x70
#define TIME_ACK_1Mbpsx2			0xE0
#define TIME_ACK_2Mbpsx2			0x70

#define FRM_LENGTH_ACK				0x0E
#define FRM_LENGTH_ACKx2			0x1C
#define FRM_LENGTH_RTS				0x14


/* ----- QoS ----- */
#define ACM_DELTS_TIMEOUT			1000	/* unit: mini seconds */
#define ACM_TIMEOUT_CHECK_BASE		100		/* unit: mini seconds */
#define ACM_BANDWIDTH_CHECK_BASE	900000	/* unit: 900000 us */

#define ACM_QOS_SUBTYPE_DATA		0x8		/* 1000 */

#define ACM_QOS_CTRL_EOSP			0x0010	/* bit 4 */


/* ----- Reason Code ----- */
#define ACM_REASON_CODE_DISASSOC_UNSPECIFIED_FAILURE			32
#define ACM_REASON_CODE_DISASSOC_LACK_SUFFICIENT_BANDWIDTH		33
#define ACM_REASON_CODE_DISASSOC_POOR_CHANNEL_CONDITION			34
#define ACM_REASON_CODE_DISASSOC_OUTSIDE_TXOP_LIMIT				35


/* ----- Status Code ----- */
#define ACM_STATUS_CODE_SUCCESS									0
#define ACM_STATUS_CODE_UNSPECIFIED_FAILURE						32
#define ACM_STATUS_CODE_ASSOC_DENIED_INSUFFICIENT_BANDWIDTH		33
#define ACM_STATUS_CODE_ASSOC_DENIED_POOR_CONDITIONS			34
#define ACM_STATUS_CODE_ASSOC_DENIED_NOT_QOS_STATION			35
#define ACM_STATUS_CODE_DECLINED								37
#define ACM_STATUS_CODE_INVALID_PARAMETERS						38
#define ACM_STATUS_CODE_SUGGESTED_TSPEC							39
#define ACM_STATUS_CODE_MAYBE_AFTER_TS_DELAY					40
#define ACM_STATUS_CODE_DIRECT_LINK_IS_NOT_ALLOWED				41
#define ACM_STATUS_CODE_DEST_STA_IS_NOT_PRESENT					42
#define ACM_STATUS_CODE_DEST_STA_IS_NOT_A_QSTA					43

#define ACM_STATUS_CODE_WMM_SUCCESS								0
#define ACM_STATUS_CODE_WMM_INVALID_PARAMETERS					1
#define ACM_STATUS_CODE_WMM_REFUSED								3

/* these status are defined by RALINK */
#define ACM_STATUS_CODE_PRIVATE_ACM_DISABLED					255




/* ========================================================================== */
/* MACRO */

/* ----- General ----- */
#ifndef PACKED
#define PACKED	__attribute__ ((packed))
#endif // PACKED //

#if 0 //def ACM_MEMORY_TEST
#define ACM_MEM_DEBUG(id)												\
	printk("11e_msg> (%d) Alloc_Num = %d\n", id, uiACM_MEM_Alloc_Num);	\
	printk("11e_msg> (%d) Free_Num  = %d\n", id, uiACM_MEM_Free_Num);
#else
#define ACM_MEM_DEBUG()
#endif // ACM_MEMORY_TEST //


	/*	copy a TSPEC; */
#define MR_STRUCT_COPY(dst, src, size)	\
									rom_memcpy((UINT8 *)&dst, (UINT8 *)&src, size)

	/*	check if the MAC is same */
#define MR_IS_SAME_MAC(src, dst)	((*src == *dst) && \
									(*(src+1) == *(dst+1)) && \
									(*(src+2) == *(dst+2)) && \
									(*(src+3) == *(dst+3)) && \
									(*(src+4) == *(dst+4)) && \
									(*(src+5) == *(dst+5)))

	/*	check if the pointer address is same */
#define MR_IS_SAME_POINTER(src, dst)	((UINT32)src == (UINT32)dst)

	/*	get VLAN priority field */
#define ACM_VLAN_UP_GET(field_tci)		((field_tci>>ACM_VLAN_OFFSET)&0x07)

	/*	1. lock a TSPEC semaphore;
		2. unlock a TSPEC semaphore; */
#define ACM_TSPEC_IRQ_LOCK(label_sem_err)				\
		do { local_irq_save(splflags);					\
			 if (0) goto label_sem_err; } while(0);

#define ACM_TSPEC_IRQ_UNLOCK(label_sem_err)				\
		do { local_irq_restore(splflags); } while(0);

#define ACM_TSPEC_SEM_LOCK(label_sem_err) 				\
        do { spin_lock_bh(&(ACMR_DB->tspec_spinlock));	\
			 splflags = 0;								\
             if (0) goto label_sem_err; } while(0);

#define ACM_TSPEC_SEM_UNLOCK(label_sem_err) \
        do { spin_unlock_bh(&(ACMR_DB->tspec_spinlock)); } while(0);


/* ----- QoS ----- */
#define ACM_TID_GET(ctrl)		(ctrl & 0x000F)
#define ACM_TCLAS_Len_Get(type)  ucA_TCLAS_Elm_Len[type]; /* length of TCLAS */


/* ----- TSPEC ------ */
/* A TS is identified uniquely by its TID value within the context of
	the RA and TA, dont care about direction */
/* In 11e spec. a TS is idnetified uniquely by its TID and direction */
#define ACM_IS_SAME_TSPEC(src_spec_p, dst_spec_p) \
	(src_spec_p->ts_info.TSID == dst_spec_p->ts_info.TSID)

#define ACM_IS_SAME_TS_INFO(src_info, dst_info) \
	(src_info.TSID == dst_info.TSID)

#define ACM_IS_SAME_TS_INFOP(src_info_p, dst_info_p) \
	((src_info_p)->TSID == (dst_info_p)->TSID)

/* same TS condition:
	1. if the direction of any TS is bidirectional link; or
		2.1 if the TSID is the same; or
		2.2 if the AC ID is the same;
	2. if the direction is the same; or
		2.1 if the TSID is the same; or
		2.2 if the AC ID is the same;
	3. if the TSID is the same; */
#define ACM_IS_SAME_TS(src_tsid, dst_tsid, src_up, dst_up, src_dir, dst_dir) \
	((((src_dir == ACM_DIRECTION_BIDIREC_LINK) ||				\
	   (dst_dir == ACM_DIRECTION_BIDIREC_LINK) ||				\
	   (src_dir == dst_dir)) &&									\
	  ((src_tsid == dst_tsid) || 								\
	   (ACM_MR_EDCA_AC(src_up) == ACM_MR_EDCA_AC(dst_up)))) ||	\
	((src_tsid == dst_tsid)))

#define ACM_TSPEC_COPY(dst_tspec_p, src_tspec_p) \
    (rom_memcpy((UINT8 *)dst_tspec_p, (UINT8 *)src_tspec_p, sizeof(ACM_TSPEC)))

#define ACM_TCLAS_COPY(dst_tclas_p, src_tclas_p) \
    (rom_memcpy((UINT8 *)dst_tclas_p, (UINT8 *)src_tclas_p, sizeof(ACM_TCLAS)))

/* send a DELTS frame or WME teardown frame */
#ifdef CONFIG_AP_SUPPORT
#define ACM_DELTS_SEND(ad_p, req_p) \
	{ EACM_WME_TeardownSendToQSTA(ad_p, req_p->cdb_p, req_p); }
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
#define ACM_DELTS_SEND(ad_p, req_p) \
	{ EACM_WME_TeardownSendToQAP(ad_p, req_p->cdb_p, req_p); }
#endif // CONFIG_STA_SUPPORT //





/* ========================================================================== */
/* Structure */

/* ----- IP ----- */
typedef struct _ACM_IPHDR {
#if defined(ACM_LITTLE_ENDIAN)
	UINT8	ihl:4,
			version:4;
#elif defined (ACM_BIG_ENDIAN)
	UINT8	version:4,
			ihl:4;
#else
#error	"Please fix endian mode in acm_extr.h"
#endif

	UINT8	tos;
	UINT16	tot_len;
	UINT16	id;
	UINT16	frag_off;
	UINT8	ttl;
	UINT8	protocol;
	UINT8	check;
	UINT32	saddr;
	UINT32	daddr;
	/*The options start here. */
} ACM_IPHDR;


/* ----- Stream ----- */
typedef struct _ACM_STREAM {

	struct _ACM_STREAM *prev_p;	/* prev TSPEC */
	struct _ACM_STREAM *next_p;	/* next TSPEC */
	struct _ACM_STREAM *dup_p;	/* duplicate TSPEC, used for BI */

	ACM_TSPEC	*tspec_p;
	ACM_TCLAS	*tclas_p[ACM_TCLAS_MAX_NUM];

#define ACM_TS_PROCESSING_NOT_EXIST		0xFF
	UINT8  	tclas_processing;

#define ACM_SM_TYPE_11E					0x00
#define ACM_SM_TYPE_WIFI				0x01
	UINT8	stream_type;	/* only support WIFI WMM ACM currently */

	/* the up will be the user priority of TCLAS if TCLAS exists;
		or it will be the user priority of TS INFO */
	UINT8	up;				/* user priority */
	UINT8	reserved1;

    /* for QAP mode, *cdb_p is the associated client database;
       for QSTA mode, *cdb_p is the client database. */
	ACMR_STA_DB	*cdb_p;		/* client database pointer */

#define ACM_TX_QUEUE_TYPE_NOT_EXIST		0xFFFFFFFF
	UINT32	tx_queue_type;

	UINT32	timeout;		/* request timeout */
	UINT16	dialog_token;	/* TSPEC ID */
	UINT16	retry;			/* retry count */

#define ACM_TC_STATUS_REQUEST			0x0000 /* requesting */
#define ACM_TC_STATUS_ACTIVE			0x0001 /* active */
#define ACM_TC_STATUS_ACTIVE_SUSPENSION	0x0002 /* active but suspended */
#define ACM_TC_STATUS_REQ_DELETING		0x0003 /* deleting the request */
#define ACM_TC_STATUS_ACT_DELETING		0x0004 /* deleting the active */
#define ACM_TC_STATUS_RENEGOTIATING		0x0005 /* renegotiating */

#define ACM_TC_STATUS_HANDLING			0x0006 /* handle request */

#define ACM_TC_STATUS_FAIL				0x0007 /* active or request fail */
	UINT16	status;			/* current status */

#define ACM_TC_CAUSE_UNKNOWN			0x0000 /* unknown cause */
#define ACM_TC_CAUSE_REQ_TIMEOUT		0x0001 /* fail due to request timeout */
#define ACM_TC_CAUSE_SUGGESTED_TSPEC	0x0002 /* fail due to suggested TSPEC */
#define ACM_TC_CAUSE_REJECTED			0x0003 /* rejected by QAP */
#define ACM_TC_CAUSE_UNKNOWN_STATUS		0x0004 /* unknown rsp status code */
#define ACM_TC_CAUSE_INACTIVITY_TIMEOUT	0x0005 /* inactivity timeout */
#define ACM_TC_CAUSE_DELETED_BY_QAP		0x0006 /* deleted by QAP */
#define ACM_TC_CAUSE_DELETED_BY_QSTA	0x0007 /* deleted by QSTA */
#define ACM_TC_CAUSE_BANDWIDTH			0x0008 /* lower priority deletion */
#define ACM_TC_CAUSE_REJ_MANY_TS		0x0009 /* only one TS for a AC */
	UINT16  cause;			/* cause that makes the stream to be stopped */
							/* valid only when status == TSPEC_STATUS_FAIL */

#define ACM_TC_PHY_TSID_DISABLE			0xFF
	/* for uplink, phy_tsid = 0 */
	UINT8	phy_tsid;		/* physical TS ID or AC ID (ACM_MR_EDCA_AC(up)) */
	UINT8	phy_tsid2;		/* duplicate TS ID, only for bi-link */

#define ACM_TC_TIMEOUT_ACTION_DELTS			0x00
#define ACM_TC_TIMEOUT_ACTION_ADDTS_REQ		0x01
	UINT8	timeout_action;	/* action when timeout */

	UINT8  renegotiation;   /* used in renegotiation TSPEC */
	UINT8  flg_is_main_link;/* 1: in active table; 0: in client data base */
	UINT8  reserved2[3];    /* reserved field */

	UINT32	addts_timeout;	/* timeout of addts */
	UINT32	delts_timeout;	/* tiemout of delts */
	UINT32	inactivity_cur, suspension_cur;  /* variable, miscroseconds */

	/* software ACM use */
	UINT32	acm_used_time;	/* unit: micro second */
	UINT64	tx_time_first;	/* unit: mirco second */

	/* minimum physical mode & mcs */
	UINT8	min_phy_mode;
	UINT8	min_mcs;

} ACM_STREAM;

/* Non-main activated links */
typedef struct _ACM_IN_TS_DEV_LIST {

    struct _ACM_IN_TS_DEV_LIST	*prev_p;
    struct _ACM_IN_TS_DEV_LIST	*next_p;

	UINT8	mac[6]; 		/* the MAC of the connected device */

} ACM_IN_TS_DEV_LIST;


/* ----- TSPEC request & active list ----- */
typedef struct _ACM_TSPEC_REQ_LIST {

	ACM_STREAM	*head_p;	/* points to head of requested TSPEC list */
	ACM_STREAM	*tail_p;	/* points to tail of requested TSPEC list */

	UINT32	tspec_num;		/* the number of requested TSPEC */

} ACM_TSPEC_REQ_LIST;

typedef struct _ACM_TSPEC_ACT_LIST {

	ACM_STREAM	*head_p;	/* points to head of activated TSPEC list */
	ACM_STREAM	*tail_p;	/* points to tail of activated TSPEC list */

#define ACM_NUM_OF_ACTIVE_LINK_FOR_A_AC		65535	/* for EDCA */
#define ACM_NUM_OF_ACTIVE_LINK_FOR_A_TS		1		/* for HCCA */
	UINT16 tspec_num;  /* number of TSPEC, 1 for TS, 65535 for AC */

    /* valid only for EDCA streams */
#define ACM_DIR_UP_OR_DL		0 /* uplink or direct link */
#define ACM_DIR_DL_OR_BI		1 /* dnlink or bidirectional link */
#define ACM_DIR_DL_OR_UP		2 /* direct link or uplink */
#define ACM_DIR_BI_OR_DN		3 /* bidirectional link or dnlink */
	UINT8  direction;

} ACM_TSPEC_ACT_LIST;


/* ----- QoS Element ----- */
typedef struct _ACM_ELM_TSPEC {

	UINT8 element_id;
	UINT8 length;

	ACM_TSPEC tspec;

} PACKED ACM_ELM_TSPEC;

typedef struct _ACM_ELM_TCLAS {

	UINT8 element_id;
	UINT8 length;

	ACM_TCLAS tclas;
} __attribute__ ((packed)) ACM_ELM_TCLAS;

typedef struct _ACM_ELM_TCLAS_PROCESSING {

	UINT8 element_id;
	UINT8 length;

#define ACM_TCLAS_PROCESSING_ALL		0
#define ACM_TCLAS_PROCESSING_ONE		1
	UINT8 processing;
} __attribute__ ((packed)) ACM_ELM_TCLAS_PROCESSING;

typedef struct _ACM_ELM_TS_DELAY {

	UINT8 element_id;
	UINT8 length;

#define ACM_TS_DELAY_DISABLE	0xFFFFFFFF
#define ACM_TS_DELAY_NORMAL		1000
#define ACM_TS_DELAY_NEVER		0	/*	never to attempt setting up of a
										traffic stream */
	UINT32 delay; /* unit: TUs, >= 0 */

} PACKED ACM_ELM_TS_DELAY;

typedef struct _ACM_ELM_QBSS_LOAD {

	UINT8 element_id;
	UINT8 length;

	/* the total number of STAs currently associated with this QBSS */
	UINT16 station_count;

	/*	defined as the percentage of time, nomalized to 255, the QAP sensed the
		medium busy, as indicated by either the physical or virtual carrier sense
		mechanism.
		This percentage is computed using the formula:
			((channel busy time / (dot11ChannelUtilizationBeaconIntervals *
			dot11BeaconPeriod * 1024)) * 255) */
	UINT8 chan_util;

	/*	specifies the remaining amount of medium time available via explicit
		admission control, in units of 32 microsecond periods per 1 second. */
	UINT16 aval_adm_cap;

} PACKED ACM_ELM_QBSS_LOAD;


/* ----- ACM Frame ----- */
typedef struct _ACM_ADDTS_REQ_FRAME {

	/* 1: representing QoS */
	UINT8 category;

	/*	0: ADDTS request;
		1: ADDTS response;
		2: DELTS;
		3: Schedule */
#define ACM_ACTION_REQUEST			0
#define ACM_ACTION_RESPONSE			1
#define ACM_ACTION_DELTS			2
#define ACM_ACTION_SCHEDULE			3
	UINT8 action;

	/* TSPEC ID */
	UINT8 dialog_token;

	/* TSPEC */
	ACM_ELM_TSPEC elm_tspec;

	/* max 5 TCLASS & 1 TCLASS Processing or none */
    UINT8 tclasp[0];

} PACKED ACM_ADDTS_REQ_FRAME;

typedef struct _ACM_ADDTS_RSP_FRAME {

	/* 1: representing QoS */
	UINT8 category;

	/* 1: ADDTS response */
	UINT8 action;

	/* TSPEC ID */
	UINT8 dialog_token;

	/* status */
	UINT8 status_code;

	/* TS Delay, TSPEC, TCLASS, TCLASS Processing, Schedule elements */
	UINT8 elmp[0];
} PACKED ACM_ADDTS_RSP_FRAME;

typedef struct _ACM_DELTS_FRAME {

	/* 1: representing QoS (WS_CATEGORY_QOS) */
	UINT8 category;

	/* 2: DELTS (WS_ACTION_QOS_DELTS) */
	UINT8 action;

	ACM_TS_INFO ts_info;
} PACKED ACM_DELTS_FRAME;

typedef struct _ACM_QOS_INFO { /* 1B */

	UINT8 qack:1;
	UINT8 queue_req:1;
	UINT8 txop_req:1;
	UINT8 more_data_ack:1;
	UINT8 edca_update_count:4;
} PACKED ACM_QOS_INFO;


/* ----- Multiple AP ACM ----- */
/* Record neighbor AP whatever ACM */
typedef struct _ACM_MULTIPLE_AP_LIST {

	struct _ACM_MULTIPLE_AP_LIST *next_p;

	ULONG timestamp_latest; /* latest update timestamp */
	UINT32 acm_time;		/* used ACM time for the AP */
	UINT8 ap_mac[6];		/* AP BSSID */

	BOOLEAN flg_is_acm_used;/* 1: ACM is used for the AP */

} ACM_MULTIPLE_AP_LIST;


/* ----- ACM Control Block ----- */
/* ACM control parameters */
typedef struct _ACM_CTRL_PARAM {

	UINT16  chan_util;          	/* QBSS Load, 0 ~ 255 */
	UINT32  chan_busy_time;     	/* unit: 1 micro second */

	/* current EDCA Parameter Set for AC0 ~ AC3 */
	UINT8   flg_ACM[ACM_DEV_NUM_OF_AC];

	/* Downgrade function, shall not downgrade to ACM AC */
#define ACM_DOWNGRADE_DISABLE		0xFF
	UINT8   downgrade_ac_num[ACM_DEV_NUM_OF_AC]; /* 0 ~ 3 */

	/* minimum contention period Tcp in a service interval */
	/* set these values by minimum service interval, do not change them on the
	   fly */
#define ACM_MIN_CP_NU_DEFAULT		90
#define ACM_MIN_CP_DE_DEFAULT		100
	UINT32  min_CP_nu, min_CP_de;   /* % = nu/de */

	/* minimum AC0/AC1 bandwidth in a second */
	/* set these values by minimum service interval, do not change them on the
	   fly */
#define ACM_MIN_BEK_NU_DEFAULT		20
#define ACM_MIN_BEK_DE_DEFAULT		100
	UINT32  min_BEK_nu, min_BEK_de; /* % = nu/de */

	/* total used ACM time, unit: microsecond */
	UINT32  acm_total_time; /* includes dnlink + uplink medium time */

	/* ACM time from other APs, unit: microsecond */
	/* We need to include the ACM time from other AP or our ACM algorithm will
		not sure the allocated bandwidth */
	BOOLEAN flg_are_multiple_AP_checked;
	UINT32	acm_time_from_other_AP;

	/* for each AC, used to set ACM CSR register, so uplink medium time is
		NOT included. unit: microsecond */
	UINT32  acm_main_time[ACM_DEV_NUM_OF_AC];

	/* for each AC, include dnlink + uplink. unit: microsecond */
	UINT32  acm_ac_time[ACM_DEV_NUM_OF_AC];

	/* current link number for uplink, dnlink, bidirectional link, &
	   direct link */
	UINT32  link_num_up, link_num_dn, link_num_bi, link_num_di;

	/* channel utilization */
	UINT8	flg_chan_util;	/* 1: enable channel utilization */

	/* dynamic ATL */
	/* 1. the sum of all MAX BW must be equal to 100 */
	/* 2. MIN must <= MAX */
	UINT8	flg_datl; 		/* 1: enable DATL */

	/* Note: all MAX sum must be 100, unit: 1/100 */
#define ACM_DATL_BW_MAX_SUM			100
#define ACM_DATL_NO_BORROW			0xFF
#define ACM_DATL_BW_MIN_VO			30
#define ACM_DATL_BW_MAX_VO			40
#define ACM_DATL_BW_MIN_VI			30
#define ACM_DATL_BW_MAX_VI			40
#define ACM_DATL_BW_MIN_BE			10
#define ACM_DATL_BW_MAX_BE			10
#define ACM_DATL_BW_MIN_BK			10
#define ACM_DATL_BW_MAX_BK			10
	UINT8	datl_bw_min[ACM_DEV_NUM_OF_AC];	/* unit: 1/100 */
	UINT8	datl_bw_max[ACM_DEV_NUM_OF_AC];	/* unit: 1/100 */

	/* record which AC borrows bandwidth from which AC */
	/* ex: [0][1] + [0][2] + [0][3] = all borrowed bandwidth for AC0 */
	UINT32	datl_bor_ac_bw[ACM_DEV_NUM_OF_AC][ACM_DEV_NUM_OF_AC];

} ACM_CTRL_PARAM;


/* ACM Control block */
typedef struct _ACM_CTRL_BLOCK {

/* EDCA parameters */
	ACM_CTRL_PARAM  edca_ctrl_param; 		/* EDCA control parameters */

/* Global Variable (used in acm_edca.c & acm_comm.c) */
	spinlock_t tspec_spinlock;				/* TSPEC management spinlock */

	/* controlled by other module to enable/disable TSPEC requests */
#define ACM_MR_TSPEC_ALLOW()			{ ACMR_DB->flg_is_tspec_allowed = 1; }
#define ACM_MR_TSPEC_DISALLOW()			{ ACMR_DB->flg_is_tspec_allowed = 0; }
#define ACM_MR_TSPEC_IS_ALLOWED()		((ACMR_DB->flg_is_tspec_allowed) == 1)
	BOOLEAN flg_is_tspec_allowed; 			/* 1: allow TSPEC request */

/* TSPEC request/deletion management */
	struct timer_list tspec_req_check_timer;
	struct tasklet_struct tspec_req_check_tasklet;
	UINT8 flg_tspec_req_check_enable;

/* STREAM activity/suspend management */
	struct timer_list stream_alive_check_timer;
	struct tasklet_struct stream_alive_check_tasklet;
	UINT8 flg_stream_alive_check_enable;

#define TSPEC_DIALOG_TOKEN_GET(pAd)		(ACMR_DB->dialog_token++)
	UINT16 dialog_token;					/* unique TSPEC dialog token */

	/* output TSPEC for QAP & QSTA */
	ACM_TSPEC_ACT_LIST  tspec_output_list_p[ACM_DEV_NUM_OF_AC];

	ACM_TSPEC_REQ_LIST  tspec_req_list;		/* all req TSPECs */
	ACM_TSPEC_REQ_LIST  tspec_fail_list;	/* all fail TSPECs, max 5 */

	/* we use the parameter to get device list to check all input links */
	/* it is a single linked list */
	ACM_IN_TS_DEV_LIST  *input_ts_dev_list_p;

/* Multiple AP management */
	ACM_MULTIPLE_AP_LIST *multiple_ap_list;

/* Power save control */
#ifdef CONFIG_STA_SUPPORT
	BOOLEAN flg_ps_is_addts_req_sent; /* 1: waiting for ADDTS response */
	UINT32	ps_mode_backup;	/* backup old PS mode */
#endif // CONFIG_STA_SUPPORT //

} ACM_CTRL_BLOCK;




/* ========================================================================== */
/* Function Prototype */

/* ----- ASIC settings Function ----- */
/* reset ACM setting in CSR */
STATIC void ACM_ASIC_ACM_Reset(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT32				ac_id,
	ACM_PARAM_IN	UINT32				medium_time);

/* enable channel busy time calculation */
STATIC void ACM_ASIC_ChanBusyEnable(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				is_enable);

/* get the channel busy time in last TBTT */
STATIC UINT32 ACM_ASIC_ChanBusyGet(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);

/* reset EDCA Parameters in CSR */
STATIC void ACM_ASIC_EDCA_Reset(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				flg_deltc);

/* get true time value, unit: microseconds */
STATIC UINT32 ACM_ASIC_TU_Translate(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT32				TU);


/* ----- Other Function ----- */
/* reset UAPSD state */
STATIC void ACM_APSD_Ctrl(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB			*cdb_p,
	ACM_PARAM_IN	UINT8				ac_id,
	ACM_PARAM_IN	UINT8				flg_ts_add,
	ACM_PARAM_IN	UINT8				flg_apsd_enable);

/* check enough bandwidth */
STATIC ACM_FUNC_STATUS ACM_BandwidthCheck(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT32				ac_id,
	ACM_PARAM_IN	UINT32				SI,
	ACM_PARAM_IN	UINT32				policy,
	ACM_PARAM_IN	UINT32				direction,
	ACM_PARAM_IN	UINT32				time_old,
	ACM_PARAM_IN	UINT32				time_new,
	ACM_PARAM_OUT	UINT32				*time_off_p,
	ACM_PARAM_OUT	UINT32				*datl_ac_p,
	ACM_PARAM_OUT	UINT32				*datl_bw_p);

/* get the MAC address of next client data base */
STATIC ACM_FUNC_STATUS ACM_CDB_MacGet(
	ACM_PARAM_IN		ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN_OUT	UINT32				*index_p,
	ACM_PARAM_IN		UINT8				*mac_p);

/* check DATL bandwidth */
STATIC ACM_FUNC_STATUS ACM_DATL_Handle(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT32				ac_id,
	ACM_PARAM_IN	UINT32				time_old,
	ACM_PARAM_IN	UINT32				time_new,
	ACM_PARAM_OUT	UINT32				*datl_ac_p,
	ACM_PARAM_OUT	UINT32				*datl_bw_p);

/* update DATL information */
STATIC void ACM_DATL_Update(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT32				ac_id,
	ACM_PARAM_IN	UINT32				time_old,
	ACM_PARAM_IN	UINT32				time_new,
	ACM_PARAM_OUT	UINT32				datl_ac_id,
	ACM_PARAM_OUT	UINT32				datl_bandwidth);

/* get extra data length for different entrypt mode */
STATIC UINT32 ACM_EncryptExtraLenGet(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB		    *cdb_p);

/* increase or decrease the link number counter for any stream */
STATIC void ACM_LinkNumCtrl(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT32				access_policy,
	ACM_PARAM_IN	UINT32				dir,
	ACM_PARAM_IN	UINT32				is_add);

/* get the MAC of the client using the TSPEC */
STATIC ACM_FUNC_STATUS ACM_MAC_Get(
	ACM_PARAM_IN	ACM_STREAM			*stream_p,
	ACM_PARAM_IN	UINT8				*mac_p);

/* check if the current tx PHY Mode and MCS > minimum PHY Mode and MCS */
STATIC ACM_FUNC_STATUS ACM_PacketPhyModeMCSCheck(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_MBUF			*mbuf_p,
	ACM_PARAM_IN	UINT8				phy_mode,
	ACM_PARAM_IN	UINT8				mcs);

/* set the minimum PHY Mode and MCS to the packet */
STATIC void ACM_PacketPhyModeMCSSet(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*stream_p);

#ifdef CONFIG_STA_SUPPORT
/* change PS mode to ACTIVE mode */
STATIC void ACM_PS_ActiveOn(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);

/* Return power save right to system */
STATIC void ACM_PS_CtrlRightReturn(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);
#endif // CONFIG_STA_SUPPORT //

/* mapping current station rate to ACM rate */
STATIC UINT8 ACM_Rate_Mapping(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB			*cdb_p);

/* get remaining amount of medium time available */
STATIC UINT32 ACM_RemainingBandwidthGet(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);

/* get TSID from a packet */
STATIC UINT8 ACM_TSID_Get(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_MBUF			*mbuf_p);

/* get physical transmit queue */
STATIC UINT32 ACM_TX_QueueTypeGet(
	ACM_PARAM_IN	UINT32				access_policy,
	ACM_PARAM_IN	UINT8				phy_tid);


/* ----- Stream Management Function ----- */
/* check whether a stream is timeout due to inactivity or suspendsion */
STATIC void ACM_STM_IdleCheck(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*stream_p);

/* copy the stream information */
STATIC void ACM_STM_InfoCopy(
	ACM_PARAM_OUT	ACM_STREAM_INFO		*dst_p,
	ACM_PARAM_IN	ACM_STREAM			*src_p);

/* STREAM inactivity & suspension timeout check task */
STATIC void ACM_STM_TASK_Check(
	ACM_PARAM_IN	ULONG				data);

/* (timer) periodical check inactivity and suspension for activated streams */
STATIC void ACM_TR_STM_Check(
	ACM_PARAM_IN	ULONG				data);

/* (timer) disable the stream check timer */
STATIC void ACM_TR_STM_CheckDisable(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);

/* (timer) enable the stream check timer */
STATIC void ACM_TR_STM_CheckEnable(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);


/* ----- 11e TSPEC Function ----- */
/* get the EDCA TSPECs in the client database */
STATIC UINT8 **ACM_CDB_TSPEC_Get(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				*mac_p,
	ACM_PARAM_IN	UINT8				policy);

/* active a requested TSPEC */
STATIC ACM_FUNC_STATUS ACM_TC_Active(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*req_p);

/* insert a TSPEC to the active table */
STATIC ACM_FUNC_STATUS ACM_TC_ActInsert(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM 		*req_p);

/* insert a main link to the client database */
STATIC ACM_FUNC_STATUS ACM_TC_ActInsertToCDB(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*req_p);

/* remove a TSPEC from the active table */
STATIC void ACM_TC_Act_Remove_From_ACT(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*stream_p);

/* remove a main link from the client database */
STATIC void ACM_TC_Act_Remove_From_CDB(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*req_p);

/* insert a new TSPEC into QSTA database */
STATIC ACM_FUNC_STATUS ACM_TC_CB_ActInsert(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*req_p);

/* delete a actived TSPEC and send out a DELTS frame */
STATIC void ACM_TC_Delete(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*stream_p);

/* free a stream and move the failed TSPEC to the fail list */
STATIC void ACM_TC_Destroy(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*req_p);

/* move the failed TSPEC to the fail list or free it */
STATIC ACM_FUNC_STATUS ACM_TC_DestroyBy_TS_Info(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				*mac_p,
	ACM_PARAM_IN	ACM_TS_INFO			*ts_info_p,
	ACM_PARAM_IN	UINT8				is_frm_sta);

/* free a stream and dont move the failed TSPEC to the fail list */
STATIC void ACM_TC_Discard(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*req_p);

/* duplicate a TSPEC */
STATIC ACM_STREAM *ACM_TC_Duplicate(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*stream_p);

/* check whether two TS Info are similar */
STATIC ACM_FUNC_STATUS ACM_TC_InfoDuplicateCheck(
	ACM_PARAM_IN	ACM_TS_INFO			*infos_p,
	ACM_PARAM_IN	ACM_TS_INFO			*infod_p);

/* find a stream */
STATIC ACM_STREAM *ACM_TC_Find(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				*mac_p,
	ACM_PARAM_IN	ACM_TS_INFO			*ts_info_p);

/* find a stream in the active table */
STATIC ACM_STREAM *ACM_TC_FindInAct(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				*mac_p,
	ACM_PARAM_IN	ACM_TS_INFO			*ts_info_p);

/* find a stream in the client database */
STATIC ACM_STREAM *ACM_TC_FindInCDB(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				*mac_p,
	ACM_PARAM_IN	ACM_TS_INFO			*ts_info_p);

/* find a stream in the requested list */
STATIC ACM_STREAM *ACM_TC_FindInReq(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				*mac_p,
	ACM_PARAM_IN	ACM_TS_INFO			*ts_info_p);

/* free a TSPEC */
STATIC ACM_FUNC_STATUS ACM_TC_Free(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			**head_p,
	ACM_PARAM_IN	ACM_STREAM			**tail_p,
	ACM_PARAM_IN	UINT16				*num_p,
	ACM_PARAM_IN	ACM_STREAM			*req_p);

/* get physical TSID, such as out AC ID or input array index */
STATIC ACM_FUNC_STATUS ACM_TC_PHYTSID_Get(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				is_qap,
	ACM_PARAM_IN	UINT8				*mac_p,
	ACM_PARAM_IN	ACM_TS_INFO			*ts_info_p,
	ACM_PARAM_IN	UINT8				up,
	ACM_PARAM_OUT	UINT8				*free_id1_p,
	ACM_PARAM_OUT	UINT8				*free_id2_p);

/* get the user priority */
STATIC UINT8 ACM_TC_UP_Get(
	ACM_PARAM_IN	ACM_TS_INFO				*ts_info_p,
	ACM_PARAM_IN	UINT32					tclas_num,
	ACM_PARAM_IN	ACM_TCLAS				*tclas_p);

/* rearrange the requested TSPEC in the requested list */
STATIC void ACM_TC_Rearrange(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*new_req_p,
	ACM_PARAM_IN	UINT16				retry);

/* release all activated TSPECs without DELTS */
STATIC void ACM_TC_ReleaseAll(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);

/* check same TSPEC */
STATIC ACM_FUNC_STATUS ACM_TC_RenegotiationCheck(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	UINT8				*mac_p,
	ACM_PARAM_IN	UINT8				up,
	ACM_PARAM_IN	ACM_TS_INFO			*ts_info_p,
	ACM_PARAM_OUT	ACM_STREAM			**stream_in_p,
	ACM_PARAM_OUT	ACM_STREAM			**stream_out_p);

/* change ADDTS state to DELTS state */
STATIC void ACM_TC_Req_ADDTS2DELTS(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*req_p);

/* check whether another link or same link exists in the requested list */
STATIC ACM_FUNC_STATUS ACM_TC_ReqCheck(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*stream_p);

/* insert a outgoing requested TSPEC to the requested list */
STATIC ACM_FUNC_STATUS ACM_TC_ReqInsert(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*new_req_p);

/* remove a outgoing requested TSPEC from the requested list */
STATIC void ACM_TC_ReqRemove(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACM_STREAM			*req_p);

/* TSPEC request check task */
STATIC void ACM_TC_TASK_ReqCheck(
	ACM_PARAM_IN	ULONG				data);

/* get IP information from the frame */
STATIC ACM_FUNC_STATUS ACM_TCLAS_IP_INFO_Get(
	ACM_PARAM_IN	UINT8				*pkt_p,
    ACM_PARAM_OUT	ACM_TCLAS			*tclas_p);

/* get VLAN information from the frame */
STATIC ACM_FUNC_STATUS ACM_TCLAS_VLAN_INFO_Get(
	ACM_PARAM_IN	UINT8				*pkt_p,
    ACM_PARAM_OUT	UINT16				*vlan_tag_p);

/* (timer) periodical check for outgoing TSPECs in the requested list */
STATIC void ACM_TR_TC_ReqCheck(
	ACM_PARAM_IN	ULONG				data);

/* (timer) disable the request check timer */
STATIC void ACM_TR_TC_ReqCheckDisable(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);

/* (timer) enable the request check timer */
STATIC void ACM_TR_TC_ReqCheckEnable(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);


/* ----- QSTA Non-main activated links management function ----- */
/* insert the QSTA to the input activated TSPEC link list */
STATIC ACM_FUNC_STATUS ACM_IN_STM_QSTA_Add(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p,
	ACM_PARAM_IN	UINT8					*mac_p);

/* delete the QSTA from the input activated TSPEC link list */
STATIC void ACM_IN_STM_QSTA_Del(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p,
	ACM_PARAM_IN	UINT8					*mac_p);

/* get a QSTA from the input activated TSPEC link list */
STATIC ACM_FUNC_STATUS ACM_IN_STM_QSTA_Get(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p,
	ACM_PARAM_IN	ACM_IN_TS_DEV_LIST		**sta_p,
	ACM_PARAM_IN	UINT8					*mac_p);

/* maintain the input activated TSPEC link list */
STATIC void ACM_IN_STM_QSTA_Maintain(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p,
	ACM_PARAM_IN	UINT8					*mac_p);


/* ----- TX time function ----- */
/* calculate the QoS packet transmission time */
UINT32 ACM_TX_TimeCal(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB         *cdb_p,
	ACM_PARAM_IN	UINT32				body_len,
	ACM_PARAM_IN	UINT8				signal,
	ACM_PARAM_IN	UINT8				flag_gmode,
	ACM_PARAM_IN	UINT8				flag_cts,
	ACM_PARAM_IN	UINT8				flag_rts,
	ACM_PARAM_IN	UINT8				flag_spreamble,
	ACM_PARAM_IN	UINT8				flag_no_ack,
	ACM_PARAM_IN	UINT32				txop_limit,
	ACM_PARAM_OUT	UINT32				*time_no_data_p,
	ACM_PARAM_OUT	UINT32				*time_header_p,
	ACM_PARAM_OUT	UINT32				*time_cts_self_p,
	ACM_PARAM_OUT	UINT32				*time_rts_cts_p,
	ACM_PARAM_OUT	UINT32				*time_ack_p);

/* calculate the QoS packet transmission time on the fly */
UINT32 ACM_TX_TimeCalOnFly(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB         *cdb_p,
	ACM_PARAM_IN	UINT32				body_len,
	ACM_PARAM_IN	UINT8				signal,
	ACM_PARAM_IN	UINT8				flag_cts,
	ACM_PARAM_IN	UINT8				flag_rts,
	ACM_PARAM_IN	UINT8				flag_spreamble,
	ACM_PARAM_IN	UINT8				flag_no_ack);

/* pre-Calculate the QoS packet transmission time */
void ACM_TX_TimeCalPre(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p);

/* calculate the frame body transmission time */
STATIC UINT16 ACM_TX_TimePlcpCal(
	ACM_PARAM_IN	UINT32	body_len,
	ACM_PARAM_IN	UINT32	rate_id,
	ACM_PARAM_IN	UINT8	flg_gmode);


#ifdef CONFIG_AP_SUPPORT
/* ======================= AP Function definition ======================= */

/* handle QoS action frame by QAP */
STATIC void ACM_ActionHandleByQAP(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB         *cdb_p,
	ACM_PARAM_IN	UINT8				*mblk_p,
	ACM_PARAM_IN	UINT32				pkt_len,
	ACM_PARAM_IN	UINT32				phy_rate);

/* handle a EDCA or HCCA stream request from a QSTA */
STATIC ACM_FUNC_STATUS ACM_TC_ReqHandle(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC	ad_p,
	ACM_PARAM_IN	ACMR_STA_DB			*cdb_p,
	ACM_PARAM_IN	UINT8				stream_type,
	ACM_PARAM_IN	UINT16				dialog_token,
	ACM_PARAM_IN	ACM_TSPEC			*tspec_p,
	ACM_PARAM_IN	UINT32				tclas_num,
	ACM_PARAM_IN	ACM_TCLAS			*tclas_p[],
	ACM_PARAM_IN	UINT8				tclas_processing,
	ACM_PARAM_IN	UINT32				phy_rate);

#endif // CONFIG_AP_SUPPORT //


#ifdef CONFIG_STA_SUPPORT
/* ======================= STA Function definition ======================= */

/* handle QoS action frame by QSTA */
STATIC void ACM_ActionHandleByQSTA(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p,
	ACM_PARAM_IN	ACMR_STA_DB             *cdb_p,
	ACM_PARAM_IN	UINT8					*mblk_p,
	ACM_PARAM_IN	UINT32					pkt_len);

/* handle a EDCA or HCCA stream response from the QAP */
STATIC ACM_FUNC_STATUS ACM_TC_RspHandle(
 	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p,
	ACM_PARAM_IN	ACMR_STA_DB				*cdb_p,
	ACM_PARAM_IN	UINT8					dialog_token,
	ACM_PARAM_IN	UINT8					status_code,
	ACM_PARAM_IN	ACM_TSPEC				*tspec_p,
	ACM_PARAM_IN	ACM_ELM_TS_DELAY		*ts_delay_p);

#endif // CONFIG_STA_SUPPORT //


/* ======================= CMD Function definition ======================= */
void ACM_CMD_Init(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p);

void ACM_CMD_Release(
	ACM_PARAM_IN	ACMR_PWLAN_STRUC		ad_p);

#endif // __ACM_COMM_H__

/* End of acm_comm.h */

