/* Copyright Statement:
 *
 * @2015 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek Inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE.
 */

/**
 * @file New_Prototype.h
 *
 *  TYPE definition.
 *
 */

#ifndef __NEW_PROTOTYPE_H__
#define __NEW_PROTOTYPE_H__
#define __KAL_INLINE__          static inline
#define IN
#define OUT
#define INOUT

#include "link_list_iot.h"
#include "link_rom.h"
#include "cm4_type.h"

#if 0
#define LOG_WIFI(_message,...) \
	do { \
		extern log_control_block_t log_control_block_wifi_fw; \
		log_control_block_wifi_fw.print_handle(&log_control_block_wifi_fw, \
												 __FUNCTION__, \
												 __LINE__, \
												 log_control_block_wifi_fw.print_level, \
												 _message, \
                                             ##__VA_ARGS__); \
	} while (0)

#define LOG_WIFI_I(_message,...) \
  do { \
	  log_control_block_t log_control_block_wifi_fw; \
	  log_control_block_wifi_fw.print_handle(&log_control_block_wifi_fw, \
											   __FUNCTION__, \
											   __LINE__, \
											   log_control_block_wifi_fw.print_level, \
											   _message, \
 ##__VA_ARGS__); \
  } while (0)

#define LOG_WIFI_W(_message,...) \
  do { \
	  log_control_block_t log_control_block_wifi_fw; \
	  log_control_block_wifi_fw.print_handle(&log_control_block_wifi_fw, \
											   __FUNCTION__, \
											   __LINE__, \
											   log_control_block_wifi_fw.print_level, \
											   _message, \
 ##__VA_ARGS__); \
  } while (0)
#endif
#if 0
#define DBGLOG(_Module, _Class, _Fmt) \
	{ \
		if (aucDebugModule[DBG_##_Module##_IDX] & DBG_CLASS_##_Class) { \
			g_fgDbgAppend = TRUE; \
			LOG_FUNC("(%11u) ", SYSTEM_TRANS(kalGetTimeTick())); \
			g_fgDbgAppend = FALSE; \
			LOG_FUNC _Fmt; \
		} \
	}
#else
#define LOUD  5
#define INFO  4
#define WARN 2
#define ERROR  1

#define DBGLOG(_Module, _Class, _Fmt) 	IOT_DBGPRINT_RAW (_Class,_Fmt);

#endif
#define PACKED  __attribute__ ((packed))
#ifndef __packed
#define __packed
#endif
#define kalBreakPoint() {IOT_DBGPRINT_RAW(RT_DEBUG_OFF,("cos core dump: Assert @ %s:%u\n", __FILE__, __LINE__)); while (1);}
#if 0
#define ASSERT(_exp) \
            { \
                if (!(_exp)) { \
                    /* LOG_FUNC(acDbgAssertString, __FILE__, __LINE__); */ \
                    kalBreakPoint(); \
                } \
            }

#define LITE_ASSERT ASSERT
#endif

#define get_unaligned(p)					\
({								\
	struct packed_dummy_struct {				\
		typeof(*(p)) __val;				\
	} __attribute__((packed)) *__ptr = (void *) (p);	\
								\
	__ptr->__val;						\
})

#define put_unaligned(x,ptr) (*(ptr) = x)

#define le16_to_cpu(x) (x)

#if 0
#define SWAP32(_x)   \
        ((UINT_32)( (((UINT_32)(_x) & 0x000000FF) << 24) | \
                    (((UINT_32)(_x) & 0x0000FF00) << 8) | \
                    (((UINT_32)(_x) & 0x00FF0000) >> 8) | \
                    (((UINT_32)(_x) & 0xFF000000) >> 24)))
#endif

#define OS_NTOHL(_Val) SWAP32(_Val)

#define ntohl(x) OS_NTOHL(x)


//void cos_delay_clock_ticks(uint32_t ms);

#if defined(kalUdelay)
#undef kalUdelay
#define kalUdelay(_usec) \
		{ \
			/* The assertion check if USEC_TO_SYSTIME() may be overflow. */ \
			/*assert((_usec) < (((UINT_32)0x80000000 - USEC_PER_SEC) / KAL_HZ)); */\
			cos_delay_clock_ticks(_usec); \
		}
#endif


#define __KAL_ATTRIB_PACKED__
#define PROFILE_SIZE    0x800 // 2KB
#define MAX_IE_LENGTH       (600)
#define INIT_EVENT_PACKET_TYPE              (0xE000)

typedef UINT_32         OS_SYSTIME, *POS_SYSTIME;

typedef VOID (*PFN_MGMT_TIMEOUT_FUNC)(UINT_32,UINT_32,UINT_32,UINT_32);
#define BUG() { IOT_DBGPRINT_RAW(RT_DEBUG_ERROR,("Kernel Bug @ %s:%u\n", __FILE__, __LINE__));}
#define BUG_ON(condition) do { if ((condition)!=0) BUG(); } while(0)

#ifndef DWORD_TO_BYTE
    #define DWORD_TO_BYTE(_value)       ((_value) << 2)
#endif /* DWORD_TO_BYTE */

#ifndef BIT
#define BIT(n)                          ((UINT_32) 1 << (n))
#define BITS2(m,n)                      (BIT(m) | BIT(n) )
#define BITS3(m,n,o)                    (BIT(m) | BIT (n) | BIT (o))
#define BITS4(m,n,o,p)                  (BIT(m) | BIT (n) | BIT (o) | BIT(p))

/* bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */


typedef struct wifi_timer_token_t {
		PFN_MGMT_TIMEOUT_FUNC	pfMgmtTimeOutFunc;
		uint32_t 	 u4Data;
}wifi_timer_token_t,*p_wifi_timer_token;

//void wifi_generic_timer_callback (TimerHandle_t tmr);
//void wifi_generic_inband_callback(uint32_t data);

//#define init_timer(x,y,d1) cnmTimerInitTimer(x,y,d1)
#define add_timer(x,y)        RTMPSetTimer(x,y)
#define mod_timer(x,y)        RTMPSetTimer(x,y)
#define del_timer_sync(x)     RTMPCancelTimer(x,NULL)
#define del_timer             del_timer_sync

#define PSE_FRAME_FAULT                                 0xfff
typedef UINT_32                 HAL_STATUS, *P_HAL_STATUS;
#define HAL_STATUS_SUCCESS                     ((HAL_STATUS) 0x00000000L)

#define BIT0		(1 << 0)
#define BIT1		(1 << 1)
#define BIT2		(1 << 2)
#define BIT3		(1 << 3)
#define BIT4		(1 << 4)
#define BIT5		(1 << 5)
#define BIT6		(1 << 6)
#define BIT7		(1 << 7)
#define BIT8		(1 << 8)
#define BIT9		(1 << 9)
#define BIT10	(1 << 10)
#define BIT11	(1 << 11)
#define BIT12	(1 << 12)
#define BIT13	(1 << 13)
#define BIT14	(1 << 14)
#define BIT15	(1 << 15)
#define BIT16	(1 << 16)
#define BIT17	(1 << 17)
#define BIT18	(1 << 18)
#define BIT19	(1 << 19)
#define BIT20	(1 << 20)
#define BIT21	(1 << 21)
#define BIT22	(1 << 22)
#define BIT23	(1 << 23)
#define BIT24	(1 << 24)
#define BIT25	(1 << 25)
#define BIT26	(1 << 26)
#define BIT27	(1 << 27)
#define BIT28	(1 << 28)
#define BIT29	(1 << 29)
#define BIT30	(1 << 30)
#define BIT31	(1 << 31)

typedef enum _ENUM_BAND_T
{
    BAND_NULL   = 0000000,
    BAND_2G4    = 2407000,
    BAND_5G     = 5000000,
    BAND_4G9375 = 4937500,
    BAND_4G89   = 4890000,
    BAND_4G85   = 4850000,
    BAND_4G     = 4000000,
    BAND_5G0025 = 5002500,
    BAND_4G0025 = 4002500
} ENUM_BAND_T, *P_ENUM_BAND_T;

typedef int bufsize;

#define LEN_KEY_DESC_NONCE			32
#define LEN_KEY_DESC_IV				16
#define LEN_KEY_DESC_RSC			8
#define LEN_KEY_DESC_ID				8
#define LEN_KEY_DESC_REPLAY			8
#define LEN_KEY_DESC_MIC			16

typedef	struct _KEY_INFO
{
	UCHAR	KeyMic:1;
	UCHAR	Secure:1;
	UCHAR	Error:1;
	UCHAR	Request:1;
	UCHAR	EKD_DL:1;       // EKD for AP; DL for STA
	UCHAR	Rsvd:3;
	UCHAR	KeyDescVer:3;
	UCHAR	KeyType:1;
	UCHAR	KeyIndex:2;
	UCHAR	Install:1;
	UCHAR	KeyAck:1;
}__attribute__((__packed__)) KEY_INFO, *PKEY_INFO;

typedef	struct __packed _KEY_DESCRIPTER
{
	UCHAR		Type;
	KEY_INFO	KeyInfo;
	UCHAR		KeyLength[2];
	UCHAR		ReplayCounter[LEN_KEY_DESC_REPLAY];
	UCHAR		KeyNonce[LEN_KEY_DESC_NONCE];
	UCHAR		KeyIv[LEN_KEY_DESC_IV];
	UCHAR		KeyRsc[LEN_KEY_DESC_RSC];
	UCHAR		KeyId[LEN_KEY_DESC_ID];
	UCHAR		KeyMic[LEN_KEY_DESC_MIC];
	UCHAR		KeyDataLen[2];
	UCHAR		KeyData[0];
}	KEY_DESCRIPTER, *PKEY_DESCRIPTER;


typedef	struct __packed _EAPOL_PACKET
{
	UCHAR	 			ProVer;
	UCHAR	 			ProType;
	UCHAR	 			Body_Len[2];
	KEY_DESCRIPTER		KeyDesc;
}	EAPOL_PACKET, *PEAPOL_PACKET;
#ifndef N9_HAL_PORTING_UNPATCH
typedef enum _ENUM_HW_TX_QUE_T {
    TX_Q_AC0 = 0,
    TX_Q_AC1,
    TX_Q_AC2,
    TX_Q_AC3,
    TX_Q_AC4,
    TX_Q_AC5,
    TX_Q_AC6,
    TX_Q_BCN,
    TX_Q_BMC,
    TX_Q_AC10,
    TX_Q_AC11,
    TX_Q_AC12,
    TX_Q_AC13,
    TX_Q_AC14,
    HW_TX_QUE_NUM
} ENUM_HW_TX_QUE_T;

typedef enum _ENUM_CIPHER_SUIT_T
{
    CIPHER_SUIT_NONE = 0,
    CIPHER_SUIT_WEP_40,
    CIPHER_SUIT_TKIP_W_MIC,
    CIPHER_SUIT_TKIP_WO_MIC,
    CIPHER_SUIT_CCMP_W_MIC, //for DFP or 802.11w MFP
    CIPHER_SUIT_WEP_104,
    CIPHER_SUIT_BIP,
    CIPHER_SUIT_WEP_128 = 7,
    CIPHER_SUIT_WPI,
    CIPHER_SUIT_CCMP_W_MIC_FOR_CCX = 9,  //for DFP or CCX MFP
    CIPHER_SUIT_CCMP_256,
    CIPHER_SUIT_GCMP_128,
    CIPHER_SUIT_GCMP_256
} ENUM_CIPHER_SUIT_T256, *P_ENUM_CIPHER_SUIT_T;

enum ENUM_RX_FILTER_T {
    RXFILTER_DROP_STBC_BCN_BC_MC,
    RXFILTER_DROP_FCS_ERR,
    RXFILTER_PROMISCUOUS_MODE,
    RXFILTER_DROP_VER_NOT_0,
    RXFILTER_DROP_PROBE_REQ,
    RXFILTER_DROP_MC_FRAME,
    RXFILTER_DROP_BC_FRAME,
    RXFILTER_DROP_NOT_IN_MC_TABLE,
    RXFILTER_DROP_ADDR3_OWN_MAC,
    RXFILTER_DROP_DIFF_BSSID_A3,
    RXFILTER_DROP_DIFF_BSSID_A2,
    RXFILTER_DROP_DIFF_BSSID_BCN,
    RXFILTER_RM_FRAME_REPORT_EN,    //Bit12
    RXFILTER_DROP_CTRL_RSV,
    RXFILTER_DROP_CTS,
    RXFILTER_DROP_RTS,
    RXFILTER_DROP_DUPLICATE,
    RXFILTER_DROP_NOT_MY_BSSID,
    RXFILTER_DROP_NOT_UC2ME,  //Bit 18
    RXFILTER_DROP_DIFF_BSSID_BTIM,
    RXFILTER_NUM
};

typedef enum _ENUM_HW_TX_PIDX_T {
    PORT_LMAC = 0,
    PORT_MCU = 1,
    PORT_NUM
} ENUM_HW_TX_PIDX_T;

#endif/*N9_HAL_PORTING_UNPATCH*/

//#define     PSE_BASE_ADDR                   0xa0000000      /* provided from designer */

/*
#define WIFI_CONFG_BASE         0x60000000
#define WIFI_MAC_BASE           0x60100000
#define WIFI_PHY_BASE           0x60200000
#define WIFI_LP_BASE            0x60300000
#define WIFI_LPAON_BASE         0x60400000
#define WIFI_INT_BASE           0x60310000
#define WIFI_WTBL_BASE          0x60320000
#define WIFI_MIB_BASE           0x60330000
#define WIFI_AON_BASE           0x60400000
#define TOP_SADC_BASE           0x80023000
#define TOP_THERM_BASE          0x80024000
#define WIFI_THERM_BASE         TOP_THERM_BASE
#define WIFI_PDA_BASE           0x81090000
#define MCU_CONFIG_BASE         0x80000000
#define TOP_RGU_BASE            0x81020000
#define TOP_CFG_AON_BASE        0x81021000
#define WIFI_CMDBT_ON_BASE      0x81022000
#define WIFI_CMDBT_BASE         0x80022000
#define TOP_GPIO_BASE           0x81023000
*/

/* the base address of the Wi-Fi modules */
/*
#define WIFI_TRB_BASE           (WIFI_MAC_BASE + 0x00000000)
#define WIFI_AGG_BASE           (WIFI_MAC_BASE + 0x00010000)
#define WIFI_ARB_BASE           (WIFI_MAC_BASE + 0x00020000)
#define WIFI_TMAC_BASE          (WIFI_MAC_BASE + 0x00030000)
#define WIFI_RMAC_BASE          (WIFI_MAC_BASE + 0x00040000)
#define WIFI_SEC_BASE           (WIFI_MAC_BASE + 0x00050000)
#define WIFI_DMA_BASE           (WIFI_MAC_BASE + 0x00060000)
#define WIFI_CONFG_OFF_BASE     (WIFI_MAC_BASE + 0x00070000)
#define WIFI_PF_BASE            (WIFI_MAC_BASE + 0x00080000)
//#define WIFI_DBG_BASE           (WIFI_MAC_BASE + 0x00090000)
#define WIFI_WTBLOFF_TOP        (WIFI_MAC_BASE + 0x00090000)
#define WIFI_ETBF_BASS          (WIFI_MAC_BASE + 0x000A0000)
*/

#define PSE_QUEUE_NO_EXIST          0xff

#define     PSE_INT_STS_LMAC_ENQ    (0x1 << 12)
#define     PSE_INT_STS_HIF_ENQ     (0x1 << 11)

#define     PSE_INT_STS_LEN_OVF     (0x1 << 10)
#define     PSE_INT_STS_PAGE_UDF    (0x1 << 9)
#define     PSE_INT_STS_WD_TO       (0x1 << 8)
#define     PSE_INT_STS_Q_F         (0x1 << 7)

#define     PSE_INT_STS_L_F         (0x1 << 6)

#define     PSE_INT_STS_F_F         (0x1 << 5)
#define     PSE_INT_STS_P_F         (0x1 << 4)
#define     PSE_INT_STS_P1Q3_NE     (0x1 << 3)
#define     PSE_INT_STS_P1Q2_NE     (0x1 << 2)
#define     PSE_INT_STS_P1Q1_NE     (0x1 << 1)
#define     PSE_INT_STS_P1Q0_NE     (0x1 << 0)

#define     PSE_INT_STS_P1_QALL_NE  (PSE_INT_STS_P1Q0_NE | PSE_INT_STS_P1Q1_NE | PSE_INT_STS_P1Q2_NE | PSE_INT_STS_P1Q3_NE)

/* For INT_STS[10:4], they will be record to field ERR_INT_ID of register PSE_INT_ERR_REASON. So, mask related status for handlering in once */
#define     PSE_ERR_INT_ID_MASK     (PSE_INT_STS_P_F|PSE_INT_STS_F_F|PSE_INT_STS_L_F|PSE_INT_STS_Q_F|PSE_INT_STS_WD_TO|PSE_INT_STS_PAGE_UDF|PSE_INT_STS_LEN_OVF)

#define     PSE_INT_EN_MASK_ALL     0xffff

#define     PSE_INT_SHIFT(x)        (0x1 << x)

#define     PSE_IDX_STS_LEN_OVF     10
#define     PSE_IDX_STS_PAGE_UDF    9
#define     PSE_IDX_STS_WD_TO       8
#define     PSE_IDX_STS_Q_F         7

#define     PSE_IDX_STS_L_F         6

#define     PSE_IDX_STS_F_F         5
#define     PSE_IDX_STS_P_F         4
#define     PSE_IDX_STS_P1Q3_NE     3
#define     PSE_IDX_STS_P1Q2_NE     2
#define     PSE_IDX_STS_P1Q1_NE     1
#define     PSE_IDX_STS_P1Q0_NE     0

#define ARB_BCNQCR1              (WIFI_ARB_BASE + 0x011C)
#define ARB_BCNQCR0              (WIFI_ARB_BASE + 0x0118)
#define  LP_PISR            (WIFI_LP_BASE + 0x0030)
#define  ARB_SCR                (WIFI_ARB_BASE + 0x0080)
#define ARB_TQCR0                (WIFI_ARB_BASE + 0x0100)
#define  ARB_RQCR               (WIFI_ARB_BASE + 0x0070)


//#define  RMAC_MORE        (WIFI_RMAC_BASE + 0x0084)
#ifndef N9_HAL_PORTING_UNPATCH
#define  LP_BEIR            (/*WIFI_LP_BASE*/0x60300000 + 0x0020)
#endif
#define  LP_PISR            (WIFI_LP_BASE + 0x0030)
#define  DMA_DCR0            (WIFI_DMA_BASE + 0x0000)
#define  TMAC_TCR               (WIFI_TMAC_BASE + 0x0000)
//#define  RMAC_RMCR        (WIFI_RMAC_BASE + 0x0080)
#define WTBL_WIUCR              (WIFI_WTBLOFF_TOP + 0x0000)
#define AGG_BWCR                 (WIFI_AGG_BASE + 0x00EC)
//#define RMAC_MAR0        (WIFI_RMAC_BASE + 0x0060)
//#define  RMAC_MAR1        (WIFI_RMAC_BASE + 0x0064)
//#define  RMAC_RFCR        (WIFI_RMAC_BASE + 0x0000)
//#define  RMAC_RFCR1       (WIFI_RMAC_BASE + 0x00A4)
//#define     PSE_INT_STS      (PSE_BASE_ADDR + 0x00000008)
#define AGG_DSCR0                (WIFI_AGG_BASE + 0x00B0)   //Posh 20131212, Add DMA scheduler
#define AGG_DSCR1                (WIFI_AGG_BASE + 0x00B4)   //Posh 20131212, Add DMA scheduler

#define AGG_PCR                  (WIFI_AGG_BASE + 0x0050)

#define AGG_PCR1                 (WIFI_AGG_BASE + 0x0054)

/* DSCR1 */ //Posh 20131212, Add DMA scheduler
#define DSCR1_DISABLE_DMA_SHDL              BIT(31)
#define DSCR1_DISABLE_DMA_SHDL_OFFSET       31

/* BSCR1 */
#define BSCR1_START_RST_BA_SB           BIT(22)
#define BSCR1_RST_BA_SEL_MASK           BITS(20,21)
#define BSCR1_RST_BA_SEL_OFFSET         20
#define BSCR1_RST_BA_TID_MASK           BITS(16,19)
#define BSCR1_RST_BA_TID_OFFSET         16
#define BSCR1_BA_MAC_ADDR               BITS(0,15)
/* SCR */
#define SCR_BCN_Q_OPERTION_MODE_0           BITS(0,1)
#define SCR_BCN_Q_OPERTION_MODE_0_OFFSET    0
#define SCR_BCN_Q_OPERTION_MODE_1           BITS(2,3)
#define SCR_BCN_Q_OPERTION_MODE_1_OFFSET    2
#define SCR_BCN_Q_OPERTION_MODE_2           BITS(4,5)
#define SCR_BCN_Q_OPERTION_MODE_2_OFFSET    4
#define SCR_BCN_Q_OPERTION_MODE_3           BITS(6,7)
#define SCR_BCN_Q_OPERTION_MODE_3_OFFSET    6

#define SCR_TX_DISABLE               BIT(8)
#define SCR_RX_DISABLE               BIT(9)
#define SCR_PLD_EN_BF_TH               BITS(16,19)
#define SCR_PLD_EN_NAV_TH              BITS(20,23)
#define SCR_BTIM_MTK_PPTARY            BIT(24)
#define SCR_BTIM_CONTROL               BIT(29)
#define SCR_BCN_CONTROL           BIT(30)
#define SCR_BM_CONTROL                 BIT(31)
/* PSE PORT LIST */
#ifndef N9_HAL_PORTING_UNPATCH
/* CPU(CTX) QUEUE LIST */
typedef enum _ENUM_PSE_CPU_P_QUEUE_T {
    ENUM_CTX_Q_0 = 0,
    ENUM_CTX_Q_1 = 1,
    ENUM_CTX_Q_2 = 2,
    ENUM_CTX_Q_3 = 3,
    ENUM_CRX     = 0,               /* Virtual source queue, keep an ID for software programming */
    ENUM_CIF_QUEUE_TOTAL_NUM = 4    /* 4 tx queue to CPU */
} ENUM_PSE_CPU_P_QUEUE_T, *P_ENUM_PSE_CPU_P_QUEUE_T;



typedef enum _ENUM_PSE_PORT_T {
    ENUM_HIF_PORT_0         = 0,
    ENUM_CPU_PORT_1         = 1,
    ENUM_LMAC_PORT_2        = 2,
    ENUM_SWITCH_PORT_3      = 3,
    ENUM_PSE_PORT_TOTAL_NUM = 4
} ENUM_PSE_PORT_T, *P_ENUM_PSE_PORT_T;
/* HIF QUEUE LIST */
typedef enum _ENUM_PSE_HIF_P_QUEUE_T {
    ENUM_HIF_HTX_LO_Q_0      = 0,
    ENUM_HIF_HTX_HI_Q_1      = 1,
    ENUM_HIF_PAUSE_Q_P0_Q_2  = 2,   /* Pause Queue ID == ( last output queue ID + 1 ) */
    ENUM_HIF_HRX             = 0,   /* Virtual source queue, keep an ID for software programming */
    ENUM_HIF_RX_QUEUE_NUM    = 1,
    ENUM_HIF_TX_QUEUE_NUM    = 2,
    ENUM_HIF_QUEUE_TOTAL_NUM = 3    /* two rx queue, and one pause queue */
} ENUM_PSE_HIF_P_QUEUE_T, *P_ENUM_PSE_HIF_P_QUEUE_T;


/* LMAC TX QUEUE LIST */
typedef enum _ENUM_PSE_LMAC_TX_P_QUEUE_T {
    ENUM_LMAC_TX_AC0_Q_0        = 0,
    ENUM_LMAC_TX_AC1_Q_1        = 1,
    ENUM_LMAC_TX_AC2_Q_2        = 2,
    ENUM_LMAC_TX_AC3_Q_3        = 3,
    ENUM_LMAC_TX_AC4_Q_4        = 4,
    ENUM_LMAC_TX_AC5_Q_5        = 5,
    ENUM_LMAC_TX_AC6_Q_6        = 6,
    ENUM_LMAC_TX_BCMC7_Q_7      = 7,
    ENUM_LMAC_TX_BEACON8_Q_8    = 8,
    ENUM_LMAC_TX_AC10_Q_9       = 9,
    ENUM_LMAC_TX_AC11_Q_10      = 10,
    ENUM_LMAC_TX_AC12_Q_11      = 11,
    ENUM_LMAC_TX_AC13_Q_12      = 12,
    ENUM_LMAC_TX_AC14_Q_13      = 13,
    ENUM_LMAC_TX_TOTAL_NUM      = 14
} ENUM_PSE_LMAC_TX_P_QUEUE_T, *P_ENUM_PSE_LMAC_TX_P_QUEUE_T;



/* LMAC RX QUEUE LIST */
typedef enum _ENUM_PSE_LMAC_RX_P_QUEUE_T {
    ENUM_LMAC_RX_DATA_Q_0       = 0,    /* virtual queue */
    ENUM_LMAC_RX_VECTOR_Q_1     = 1,    /* virtual queue */
    ENUM_LMAC_RX_TX_STS_Q_2     = 2,    /* virtual queue */
//    ENUM_LMAC_PAUSE_Q_P2_Q_14   = 14,   /* puase queue */
//    ENUM_LMAC_WRX_QS              = 4,    /* Virtual source queue, keep an ID for software programming,  */
                                        /*
                                         * Due to Source Rsv Cnt support for LMAC Virtual Port, So, we assign ENUM_LMAC_WRX_QS = 4
                                         * Otherwise, ENUM_LMAC_WRX_QS was assigned to 0,
                                         */
    ENUM_LMAC_RX_TOTAL_NUM      = 3
} ENUM_PSE_LMAC_RX_P_QUEUE_T, *P_ENUM_PSE_LMAC_RX_P_QUEUE_T;


/* PSE SWITCH QUEUE LIST, INCLUDING FREE QUEUE */
typedef enum _ENUM_PSE_SWITCH_P_QUEUE_T {
    ENUM_SWITCH_SW_Q_0          = 0,
    ENUM_SWITCH_SW_Q_1          = 1,
    ENUM_SWITCH_SW_Q_2          = 2,
    ENUM_SWITCH_SW_Q_3          = 3,
    ENUM_SWITCH_SW_Q_4          = 4,
    ENUM_SWITCH_SW_Q_5          = 5,
    ENUM_SWITCH_SW_Q_6          = 6,
    ENUM_SWITCH_SW_Q_7          = 7,
    ENUM_SWITCH_FREE_Q_8        = 8,
    ENUM_SWITCH_TOTAL_NUM       = 9
} ENUM_PSE_SWITCH_P_QUEUE_T, *P_ENUM_PSE_SWITCH_P_QUEUE_T;
#endif/*N9_HAL_PORTING_UNPATCH*/
#ifndef N9_HAL_PORTING_UNPATCH
typedef enum _ENUM_WTBL2_CLEAR_COUNTER_TYPE_T {
    RX_CNT = 0,
    TX_CNT ,
    ADM_CNT,
    TOTAL_CLEAR_CNT
}ENUM_WTBL2_CLEAR_COUNTER_TYPE_T;

typedef enum _ENUM_WTBL2_SN_SOURCE_TYPE_T {
    TID0_AC0_SN = 0,
    TID1_AC1_SN,
    TID2_AC2_SN,
    TID3_AC3_SN,
    TID4_SN,
    TID5_SN,
    TID6_SN,
    TID7_SN,
    COMMON_SN,

    MAX_NUM_SN_SOURCE
} ENUM_WTBL2_SN_SOURCE_TYPE_T;
#endif/*N9_HAL_PORTING_UNPATCH*/
#define DOT11_N_SUPPORT 1






#if 0

void * iot_malloc(size_t size);
void iot_free(void *ptr);

void * rom_memcpy (void *dest, const void *src, size_t n);
void * rom_memmove(void *dest, const void *src, size_t n);
void * rom_memset(void *s, int c, size_t n);
int rom_memcmp (const void *s1, const void *s2, size_t n);
void * rom_memset(void *s, int c, size_t n);

UINT32 cos_get_clock_ticks();
#endif
#define os_move_mem(Destination, Source, Length) rom_memmove(Destination, Source, Length)
#define os_zero_mem(Destination, Length)         rom_memset(Destination,'\0',Length)
#define os_equal_mem(Source1, Source2, Length)   (!rom_memcmp(Source1, Source2, Length))


#endif
