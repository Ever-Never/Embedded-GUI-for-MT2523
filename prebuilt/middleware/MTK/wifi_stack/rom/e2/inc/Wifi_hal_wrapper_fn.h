
#include "rt_config.h"

//#define PRTMP_ADAPTER PVOID 
//extern struct MT_RX_FILTER_CTRL_T;
#ifndef N9_HAL_PORTING_UNPATCH

typedef enum _ENUM_AUTO_RATE_NUM_T {
    AR_RATE_1 = 0,
    AR_RATE_2,
    AR_RATE_3,
    AR_RATE_4,
    AR_RATE_5,
    AR_RATE_6,
    AR_RATE_7,
    AR_RATE_8,
    AR_RATE_NUM
} ENUM_AUTO_RATE_NUM_T;


typedef struct _HAL_MEASUREMENT_QUIET_CTRL_T {
    UINT_32 u4CounterItem0;
    UINT_32 u4CounterItem1;
    UINT_32 u4CounterItem2;
    UINT_32 u4CounterItem3;
} HAL_MEASUREMENT_QUIET_CTRL_T, *P_HAL_MEASUREMENT_QUIET_CTRL_T;


typedef struct _TX_AC_PARAM_AIFS_CW_T {
    UINT_16     u2CWmin;            /*!< CWmin */
    UINT_16     u2CWmax;            /*!< CWmax */
    UINT_16     u2TxopLimit;        /*!< TXOP limit */
    UINT_16     u2Aifsn;            /*!< AIFSN */
    UINT_8       ucGuradTime;   /*!< GuardTime for STOP/FLUSH. */
} TX_AC_PARAM_AIFS_CW_T, *P_TX_AC_PARAM_AIFS_CW_T;
#endif/*N9_HAL_PORTING_UNPATCH*/

#ifndef N9_HAL_PORTING_UNPATCH

typedef struct _HAL_WTBL_PEER_CAP_T  {
    UINT_8                  ucTxPowerOffset;
    UINT_8                  ucAntnnaPriority;
    UINT_8                  ucCounterBWSelector;
    UINT_8                  ucChangeBWAfterRateN;
    UINT_8                  ucFrequencyCapability;
    BOOL                    fgSpatialExtension;
    BOOL                    fgG2;
    BOOL                    fgG4;
    BOOL                    fgG8;
    BOOL                    fgG16;

    /*
     * Posh 20131112, Merge code from MT6630 E2
     */
    UINT_8                  ucMMSS;
    UINT_8                  ucAmpduFactor;
}HAL_WTBL_PEER_CAP_T,*P_HAL_WTBL_PEER_CAP_T;

typedef struct _HAL_WTBL_PEER_RATE_INFO_T {
    UINT_8                  ucCounterMPDUFail;
    UINT_8                  ucCounterMPDUTx;
    UINT_8                  ucRateIdx;
    UINT_8                  ucReserved[1];
    UINT_16                 au2RateCode[AR_RATE_NUM];
}HAL_WTBL_PEER_RATE_INFO_T, *P_HAL_WTBL_PEER_RATE_INFO_T;

typedef struct _HAL_WTBL2_TX_COUNTER_ALL_T {
    UINT_16     u2Rate1TxCount;
    UINT_16     u2Rate1FailCount;
    UINT_8      ucRate2TxCount;
    UINT_8      ucRate3TxCount;
    UINT_8      ucRate4TxCount;
    UINT_8      ucRate5TxCount;
    UINT_16     u2CurrentBWTxCount;
    UINT_16     u2CurrentBWFailCount;
    UINT_16     u2OtherBWTxCount;
    UINT_16     u2OtherBWFailCount;
}HAL_WTBL2_TX_COUNTER_ALL_T,*P_HAL_WTBL2_TX_COUNTER_ALL_T;

typedef struct _HAL_WTBL2_RX_COUNTER_ALL_T {
    UINT_8     ucRxRcpi0;
    UINT_8     ucRxRcpi1;
    UINT_8     ucRxRcpi2;
    UINT_8     ucReserve[1];
    UINT_8     ucRxCC0;
    UINT_8     ucRxCC1;
    UINT_8     ucRxCC2;
    UINT_8     ucCeRmsd;
}HAL_WTBL2_RX_COUNTER_ALL_T,*P_HAL_WTBL2_RX_COUNTER_ALL_T;


typedef struct _HAL_WTBL_SEC_CONFIG_T  {
    BOOL                   fgRCA2;
    BOOL                   fgRV;
    BOOL                   fgIKV;
    BOOL                   fgRKV;
    BOOL                   fgRCID;
    BOOL                   fgRCA1;
    BOOL                   fgEvenPN;
    UINT_8                 ucKeyID;
    UINT_8                 ucMUARIdx;
    UINT_8                 ucCipherSuit;
}HAL_WTBL_SEC_CONFIG_T, *P_HAL_WTBL_SEC_CONFIG_T;


typedef struct _HAL_WTBL_TX_CONFIG_T  {
    UINT_8                 aucPA[6];
    BOOL                    fgIsToDS;
    BOOL                    fgIsFromDS;
    BOOL                   fgDisRxHdrTran;
    BOOL                    fgIsPwrMgt;
    BOOL                   fgRDG;
    BOOL                   fgRTS;
    BOOL                   fgCFAck;
    BOOL                   fgRdgBA;
    BOOL                   fgSMPS;
    BOOL                   fgBAFEn;
    BOOL                   fgDynBw;
    BOOL                   fgLDPC;
    BOOL                   fgTIBF;
    BOOL                   fgTEBF;
    BOOL                   fgTxopPS;
    BOOL                   fgIsQoS;
    BOOL                   fgIsHT;
    BOOL                   fgIsVHT;
    BOOL                   fgADMEn;
    BOOL                   fgCheckPER;
    BOOL                   fgIsGID63;
    BOOL                   fgDonotUpdateIPSM;
    BOOL                   fgIPSM;
    BOOL                   fgIsPSM;
    BOOL                   fgSkipTx;
    BOOL                   fgSW;
    BOOL                   fgMesh;

#if 0   //Posh 20131112, Old design on MT6630 E1
    UINT_8                 ucMMSS;
    UINT_8                 ucAmpduFactor;
//#elif (CFG_SUPPORT_VHT == 1)
#else
    BOOL                   fgVhtLDPC;
    BOOL                   fgVhtTIBF;
    BOOL                   fgVhtTEBF;
#endif
}HAL_WTBL_TX_CONFIG_T, *P_HAL_WTBL_TX_CONFIG_T;


typedef struct _HAL_WTBL_KEY_CONFIG_T  {
    UINT_8                 aucCipherKey[16];
    UINT_8                 aucRxMicKey[8];
    UINT_8                 aucTxMicKey[8];
}HAL_WTBL_KEY_CONFIG_T, *P_HAL_WTBL_KEY_CONFIG_T;


typedef struct _HAL_WTBL_FID_EID_T  {
    UINT_16                u2Tbl2Fid;
    UINT_16                u2Tbl3Fid;
    UINT_16                u2Tbl4Fid;
    UINT_16                u2PartialAID;
    UINT_8                 ucTbl2Eid;
    UINT_8                 ucTbl3Eid;
    UINT_8                 ucTbl4Eid;
}HAL_WTBL_FID_EID_T, *P_HAL_WTBL_FID_EID_T;


#if 0 /* 6630 */
typedef struct _HAL_WTBL_TBL2_DW11_T  {
    UINT_8                ucTxPowerOffset;
    UINT_8                ucAntPri;
    UINT_8                ucCcBwSel;
    UINT_8                ucCBRN;
    UINT_8                ucFCap;
    UINT_8                ucRateNSel;
    UINT_8                ucMpduFailCnt;
    UINT_8                ucMpduTxCnt;
    UINT_8                ucRateIdx;
    BOOL                  fgSpeEn;
    BOOL                  fgSGI160;
    BOOL                  fgSGI80;
    BOOL                  fgSGI40;
    BOOL                  fgSGI20;
}HAL_WTBL_TBL2_DW11_T, *P_HAL_WTBL_TBL2_DW11_T;
#endif

typedef struct _HAL_WTBL_TBL2_DW15_T  {
    UINT_8                aucBaWinSize[8];
    UINT_8                ucBaEnBitmap;
}HAL_WTBL_TBL2_DW15_T, *P_HAL_WTBL_TBL2_DW15_T;


typedef enum _ENUM_WTBL2_RX_COUNTER_SEL_TYPE_T {
    SEL_TYPE_CHANNELCAPACITY_CNT=0,
    SEL_TYPE_NOISE_CNT,
} ENUM_WTBL2_RX_COUNTER_SEL_TYPE_T;


typedef struct _HAL_WTBL2_RATE_CONFIG_T  {
    UINT_16                u2Rate[AR_RATE_NUM];
}HAL_WTBL2_RATE_CONFIG_T, *P_HAL_WTBL2_RATE_CONFIG_T;

typedef struct _AC_ADM_INFO_T {
    UINT_32                u4ACTxTime;
    UINT_32                u4ACTxBytes;
} AC_ADM_INFO_T, *P_AC_ADM_INFO_T;


typedef struct _HAL_WTBL4_TX_ADM_INFO_T  {
    AC_ADM_INFO_T        arTxAdmInfo[4];
}HAL_WTBL4_TX_ADM_INFO_T , *P_HAL_WTBL4_TX_ADM_INFO_T ;

#endif/*N9_HAL_PORTING_UNPATCH*/


#if 0 /* 6630 */
typedef struct _HAL_WTBL_RATE_CONFIG_T  {
    UINT_8                 ucRate[AR_RATE_NUM];
}HAL_WTBL_RATE_CONFIG_T, *P_HAL_WTBL_RATE_CONFIG_T;
#endif
#ifndef N9_HAL_PORTING_UNPATCH
#if CFG_NEW_AUTO_RATE // NEW Auto Rate
typedef struct _HAL_RATE_TX_CNT_CONFIG_T  {
    UINT_8                 ucRateUpLimit[AR_RATE_NUM];
    UINT_8                 ucRateDownLimit[AR_RATE_NUM];
}HAL_RATE_TX_CNT_CONFIG_T, *P_HAL_RATE_TX_CNT_CONFIG_T;
#else // OLD Auto Rate
typedef struct _HAL_RATE_TX_CNT_CONFIG_T  {
    UINT_8                 ucRateCntLimit[AR_RATE_NUM];
}HAL_RATE_TX_CNT_CONFIG_T, *P_HAL_RATE_TX_CNT_CONFIG_T;
#endif
#endif/*N9_HAL_PORTING_UNPATCH*/

#define RCPI_MOVING_AVERAGE_ONE          0x3
#define RCPI_MOVING_AVERAGE_TWO         0x2
#define RCPI_MOVING_AVERAGE_FOUR       0x1
#define RCPI_MOVING_AVERAGE_EIGHT       0x0

#ifndef N9_HAL_PORTING_UNPATCH

typedef enum _ENUM_RESP_RCPI_LQ_TYPE_T {
    RESP_RCPI_LQ_BW20_1M = 0,
    RESP_RCPI_LQ_BW20_2M,
    RESP_RCPI_LQ_BW20_55M,
    RESP_RCPI_LQ_BW20_11M,
    RESP_RCPI_LQ_BW20_6M,
    RESP_RCPI_LQ_BW20_12M,
    RESP_RCPI_LQ_BW20_24M,
    RESP_RCPI_LQ_BW40_6M,
    RESP_RCPI_LQ_BW40_12M,
    RESP_RCPI_LQ_BW40_24M,
    RESP_RCPI_LQ_NUM
} ENUM_RESP_RCPI_LQ_TYPE_T;


typedef struct _HAL_RESP_RCPI_LQ_T {
    UINT_8      aucRespRcpiLq[RESP_RCPI_LQ_NUM];
} HAL_RESP_RCPI_LQ_T, *P_HAL_RESP_RCPI_LQ_T;


typedef enum _ENUM_RESP_RCPI_TYPE_T {
    RESP_RCPI0 = 0,
    RESP_RCPI1,
    RESP_RCPI2,
    RESP_RCPI_NUM
} ENUM_RESP_RCPI_TYPE_T;


typedef struct _HAL_RESP_RCPI_T {
    UINT_8      aucRespRcpi[RESP_RCPI_NUM];
} HAL_RESP_RCPI_T, *P_HAL_RESP_RCPI_T;

typedef enum _ENUM_FRAME_POWER_GROUP_T {
    FRAME_POWER_0 = 0,
    //FRAME_POWER_1,    //Posh 20140101, Merge code from MT6630 E2
    FRAME_POWER_NUM
} ENUM_FRAME_POWER_GROUP_T;


typedef struct _HAL_FRAME_POWER_SET_T {
    UINT_8 ucFramePowerDbm;
} HAL_FRAME_POWER_SET_T, *P_HAL_FRAME_POWER_SET_T;


typedef struct _HAL_FRAME_POWER_CONFIG_T {
    HAL_FRAME_POWER_SET_T aucFramePowerConfig[MODULATION_SYSTEM_NUM][MODULATION_TYPE_NUM];
} HAL_FRAME_POWER_CONFIG_T, *P_HAL_FRAME_POWER_CONFIG_T;

typedef struct _HAL_VHT_FRAME_POWER_CONFIG_T {
    HAL_FRAME_POWER_SET_T aucVhtFramePowerConfig[MODULATION_TYPE_VHT_NUM];
} HAL_VHT_FRAME_POWER_CONFIG_T, *P_HAL_VHT_FRAME_POWER_CONFIG_T;
#endif/*N9_HAL_PORTING_UNPATCH*/

#if 0 /* Move to RAM Code */
typedef enum _ENUM_MEASUREMENT_COUNTER_T {
    MEASUREMENT_DISABLE = 0,
    MEASUREMENT_COUNTER_0,
    MEASUREMENT_COUNTER_1,
    MEASUREMENT_COUNTER_2,
    MEASUREMENT_COUNTER_3,
    MEASUREMENT_COUNTER_ALWAYS_ON = 7
} ENUM_MEASUREMENT_COUNTER_T;

typedef enum _ENUM_MEASUREMENT_BYPASS_TYPE_T {
    MEASUREMENT_BYPASS_DISABLE = 0,
    MEASUREMENT_BYPASS_RXP_DISABLE,
    MEASUREMENT_BYPASS_RXP_DISABLE_OR_RW0,
    MEASUREMENT_BYPASS_RESERVED
} ENUM_MEASUREMENT_BYPASS_TYPE_T;

typedef enum _ENUM_MEASUREMENT_ITEM_T {
    MEASUREMENT_BSSRXSTS_REQ = 0,
    MEASUREMENT_RADAR_REQ,
    MEASUREMENT_CCA_REQ,
    MEASUREMENT_RPI_HISTOGRAM_REQ,
    MEASUREMENT_IPI_HISTOGRAM_REQ,
    MEASUREMENT_BSS_REQUEST_REQ,
    MEASUREMENT_CCANAVTX_REQ,
    MEASUREMENT_P_ED_REQ,
    MEASUREMENT_RX_OFDM_CCK_MDRDY_REQ,
    MEASUREMENT_ITEM_NUM
} ENUM_MEASUREMENT_ITEM_T;
#endif

/*************************************************************/

#endif



