/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#if !defined(__EMI_BUS_MONITOR_MT6261_H__)
#define __EMI_BUS_MONITOR_MT6261_H__


/* RHR Add */
#include "emi_hw_internal.h"
#include <stdint.h>
#include <stdbool.h>
#if 1   //[CM's question 20120106] wait to implement
/*******************************************************************************
 * Define HW register.
 *******************************************************************************/



/*******************************************************************************
 * Define constants.
 *******************************************************************************/

/*
 * M0 : MCU
 * M1 : DMA
 * M2 : GMC
 */
enum { MCU = 0x1, DMA = 0x2, GMC = 0x4, DUMMY_READ = 0x80 };

enum { Read_Write = 0x0, Read_Only = 0x1, Write_Only = 0x2 };

// different from ARM11:  INC8_4B, WR8_4B
enum { SINGLE_1B = 0x00, SINGLE_2B = 0x01, SINGLE_4B = 0x02,
       WR4_1B = 0x08, WR4_2B = 0x09, WR4_4B = 0x0A,
       INC4_1B = 0x0C, INC4_2B = 0x0D, INC4_4B = 0x0E,
       WR8_1B = 0x10, WR8_2B = 0x11, WR8_4B = 0x12,
       INC8_1B = 0x14, INC8_2B = 0x15, INC8_4B = 0x16,
       WR16_1B = 0x18, WR16_2B = 0x19, WR16_4B = 0x1A,
       INC16_1B = 0x1C, INC16_2B = 0x1D, INC816_4B = 0x1E
     };

/*******************************************************************************
 * Define data structures.
 *******************************************************************************/

/* data structure for EMI transaction counts */
typedef struct _EBM_TCNT_LOG_T {
    uint32_t bus_ccnt;
    uint32_t trans_all_cnt;
    uint32_t trans_cnt;
    uint32_t word_all_cnt;
    uint32_t word_cnt;
    uint32_t busy_all_cnt;
    uint32_t busy_cnt;
} EBM_TCNT_LOG_T;

/* data structure for EMI transaction type */
typedef struct _EBM_TTYPE_LOG_T {
    uint32_t ttype1, ttype2, ttype3;
} EBM_TTYPE_LOG_T;

/* data structure for EMI transcation log */
typedef struct _EBM_LOG_T {
    EBM_TCNT_LOG_T tcnt;
    EBM_TTYPE_LOG_T ttype;
} EBM_LOG_T;


/*******************************************************************************
 * Define macros.
 *******************************************************************************/

#define EBM_SEL_MASTER(c, m)    \
        do {    \
            if ((c) == 1) { \
                *EMI_BMEN = (*EMI_BMEN & 0xff00ffff) | ((m) << 16); \
            } else {    \
                ASSERT(0);  \
            }   \
        } while (0)

#define EBM_SEL_TTYPE(c, t) \
        do {    \
            if ((c) == 1) { \
                *EMI_BMEN = (*EMI_BMEN & 0x00ffffff) | ((uint32_t)(t) << 24); \
            } else {    \
                ASSERT(0);  \
            }   \
        } while (0)

#define EBM_SET_RW_REQ(v) \
        {\
        	*EMI_BMEN = (*EMI_BMEN & 0xffffffcf) | ((v) << 4);\
        }

#define EBM_Start() \
        {\
        	*EMI_BMEN |= 0x001;\
        }

#define EBM_Pause_LOG() \
        {\
        	*EMI_BMEN |= 2;\
        }

#define EBM_Check_Overrun() ((*EMI_BMEN&0x100)==0?KAL_FALSE:KAL_TRUE)

#define EBM_GET_TCNT_LOG(c, l)   \
        do {    \
        	(l)->bus_ccnt = *EMI_BCNT;  \
        	(l)->trans_all_cnt = *EMI_TACT; \
        	(l)->word_all_cnt = *EMI_WACT;  \
        	(l)->busy_all_cnt = *EMI_BACT;  \
            if ((c) == 1) { \
                (l)->trans_cnt = *EMI_TSCT; \
                (l)->word_cnt = *EMI_WSCT;  \
                (l)->busy_cnt = *EMI_BSCT0;  \
            } else {    \
                ASSERT(0);  \
            }   \
        } while (0)

#define EBM_GET_TTYPE_LOG(l)    \
        do {    \
            (l)->ttype1 = *(EMI_TTYPE1);    \
        } while (0)

#define EBM_Clear_Counter() \
        {\
        	*EMI_BMEN &= 0xfffffffc;\
        }

#define EBM_Restart_Counter() \
        {\
        	*EMI_BMEN &= 0xfffffffd;\
        }


/*******************************************************************************
 * Implement abstration interface.
 *******************************************************************************/


/* To Marvin: Standard setting, for SWLA EBM-addon;
							will be used in sst_concurrent_utility.c:Enable_Normal_EMI_Monitor()
 */
#define EBM_INIT()  \
        do {    \
            EBM_Clear_Counter();    \
            EBM_SEL_MASTER(1, MCU);    \
            EBM_SEL_TTYPE(1, 0);    \
            EBM_SET_RW_REQ(Read_Write);     \
        } while (0)

#define EBM_START() \
        do {    \
            EBM_Start();    \
        } while (0)

#define EBM_PAUSE() \
        do {    \
            EBM_Pause_LOG();    \
        } while (0)

#define EBM_GET_MCU_IBUS_LOG(l)  \
        do {    \
            bool ret;   \
            ret = EBM_Check_Overrun();  \
            ASSERT(ret == KAL_FALSE);   \
            EBM_GET_TCNT_LOG(1, &((l)->tcnt)); \
            EBM_GET_TTYPE_LOG(&((l)->ttype));   \
        } while (0)

#define EBM_GET_MCU_DBUS_LOG(l)  \
        do {    \
            bool ret;   \
            ret = EBM_Check_Overrun();  \
            ASSERT(ret == KAL_FALSE);   \
            EBM_GET_TCNT_LOG(1, &((l)->tcnt)); \
            EBM_GET_TTYPE_LOG(&((l)->ttype));   \
        } while (0)

#define EBM_GET_MCU_BUS_LOG(l)  \
        do {    \
            bool ret;   \
            ret = EBM_Check_Overrun();  \
            ASSERT(ret == KAL_FALSE);   \
            EBM_GET_TCNT_LOG(1, &((l)->tcnt)); \
            EBM_GET_TTYPE_LOG(&((l)->ttype));   \
        } while (0)

#define EBM_RESUME()    \
        do {    \
            EBM_Restart_Counter();  \
        } while (0)

#define EBM_STOP()  \
        do {    \
            EBM_Clear_Counter() \
        } while (0)

#endif
#endif  /* !__EMI_BUS_MONITOR_MT6261_H__ */

