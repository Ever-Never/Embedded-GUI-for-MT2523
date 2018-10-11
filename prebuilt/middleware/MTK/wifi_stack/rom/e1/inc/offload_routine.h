#ifndef _OFFLOAD_ROUTINE_H
#define _OFFLOAD_ROUTINE_H

#include "typedef_iot_rom.h"
#define SIFS_TIME_24G          10
#define SIFS_TIME_5G           16

#define RIFS_TIME              2 // 802.11n
#define EIFS_TIME              360


/* Power Management Level */
#define PM2         2
#define PM4         4
#define PM5         5
#define PM6         6
#define PM7         7
#define ENTER_PM_STATE 1
#define EXIT_PM_STATE 2

typedef struct _PM_STATE_CTRL_T
{
    UINT_8 PmNumber;
    UINT_8 PmState;
    UINT_8 Bssid[6];
    UINT_8 DtimPeriod;
    UINT_8 WlanIdx;
    UINT_16 u2BcnInterval;
    UINT_32 u4Aid;
    UINT_32 u4RxFilter;
    UINT_8 aucReserved[4];
    UINT_32 u4Feature;
    UINT_8 OwnMacIdx;
    UINT_8 WmmIdx;
    UINT_8 BcnLossCount;
    UINT_8 BcnSpDuration;
} PM_STATE_CTRL_T, *P_PM_STATE_CTRL_T;

typedef struct _PWR_MGT_BIT_T
{
    UINT_8 WlanIdx;
    UINT_8 PwrMgtBit;
    UINT_8 aucReserved[2];
}PWR_MGT_BIT_T, *P_PWR_MGT_BIT_T;

#define Q_IDX_AC0	0x00
#define Q_IDX_AC1	0x01
#define Q_IDX_AC2	0x02
#define Q_IDX_AC3	0x03

typedef struct _CHAN_SWITCH_T
{
    UINT_8 PrimCh;
    UINT_8 CentralCh;
    UINT_8 BW;
    UINT_8 TxStreamNum;
    UINT_8 RxStreamNum;
    UINT_8 SwitchReason;
    UINT_8 BandNum;
    UINT_8 CentralCh2;
    UINT_8 Reserved0[4];
    UINT_8 TxPowerSKU[21];
    UINT_8 Reserved1[3];
} CHAN_SWITCH_T, *P_CHAN_SWITCH_T;

typedef struct _RDG_CTRL_T
{
    UINT_32 u4Txop;
    UINT_8 LongNav;
    UINT_8 Role;  //0: RDG disable  1: initiator  2: responder  3: Both
    UINT_8 ucWlanIdx;
    UINT_8 Band;
} RDG_CTRL_T, *P_RDG_CTRL_T;

typedef struct _TX_AC_PARAM
{
    UINT_8 ucAcNum;
    UINT_8 ucVaildBit;
    UINT_8 ucAifs;
    UINT_8 ucWinMin;
    UINT_16 u2WinMax;
    UINT_16 u2Txop;
}TX_AC_PARAM,*P_TX_AC_PARAM;

typedef struct _EDCA_SET_T
{
    UINT_8 TotalNum;
    UINT_8 aucReserve[3];
    TX_AC_PARAM rAcParam[HW_TX_QUE_NUM];
} EDCA_SET_T, *P_EDCA_SET_T;

typedef struct _SLOT_TIME_SET_T
{
    UINT_8 ucSlotTime;
    UINT_8 ucSifs;
    UINT_8 ucRifs;
    UINT_8 ucOldEifs;
    UINT_16 u2Eifs;
    UINT_8 ucBandNum;
    UINT_8 ucReserved;
}SLOT_TIME_SET_T,*P_SLOT_TIME_SET_T;

#endif /* _OFFLOAD_ROUTINE_H */

