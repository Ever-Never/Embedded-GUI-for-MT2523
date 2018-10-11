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

#ifndef __HAL_DSP_TOPSM_H__
#define __HAL_DSP_TOPSM_H__

#include "hal_platform.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    /* Resource on MCU RM */
    RM_MCU_BASE = 0,

    /* Resource on DSP RM */
    RM_MODEM_BASE = 10,
    RM_MODEM_DSP_1,
    RM_MODEM_DSP_2,
    RM_MODEM_DSP_3,
    RM_MODEM_L2_COPRO,
    NUMBER_OF_RM_MODEM
} hal_dsp_topsm_Resource_Module_t;

#define RM_SLV_MD2G_TOPSM   0
#define MD_AUDIO_CPD_PROTECT_ACK_MASK 0x1  /* Mask MD2G */


#define MODEM_CFG_RM_SLV_SM_MODEM_TOPSM   0
//#define MODEM_CONFG_BASE                        (0x83000000) /* MODEM_CONFG_BASE*/
//#define MODEM2G_TOPSM_BASE                       (0x83010000) /* MODEM2G_TOPSM_BASE: 0x83010000 is used for Larkspur */

#define MODEM_CFG_SLV_SM_WAKUP_REQ           ((volatile uint32_t*)(MODEM_CONFG_BASE+0x0500))
#define MODEM_CFG_SLV_SM_WAKUP_RDY           ((volatile uint32_t*)(MODEM_CONFG_BASE+0x0504))

#define DSP_TOPSM_SM_SLV_REQ_IRQ             ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x08C0)) /* Sleep Manager Request Interrupt */
#define DSP_TOPSM_SM_MAS_RDY_IRQ             ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x08D0)) /* Sleep Manager Ready Interrupt */
#define DSP_TOPSM_SM_TRIG_SETTLE0            ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0900)) /* Sleep Manager Ready Interrupt */
#define DSP_TOPSM_DBG_RM_SM_MASK             ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0880)) /* Sleep Manager Debug Control Mask */
#define DSP_TOPSM_TOPSM_DBG                  ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0050)) /* Sleep Manager Debug Control */
#define DSP_TOPSM_RM_PWR_CON0                ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0800)) /* Power Control Register0: Specify MTCMOS SUBSYS Power Control Sequence, and it is controlled by Software mode or hardware mode */
#define DSP_TOPSM_RM_PWR_STA                 ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0820)) /* Power Status Register */
#define DSP_TOPSM_RM_PWR_PER0                ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0824)) /* Power COntrol for peripheral */
#define DSP_TOPSM_RM_PLL_MASK0               ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0830)) /* Resource Manager Timer PLL Enable Control 0 */
#define DSP_TOPSM_RM_SM_PWR                  ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0850)) /* SLeep Manager  Power Enable Control */
#define DSP_TOPSM_RM_TMR_PWR0                ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0018)) /* Resource Manager Timer Power Enable Control 0 */
#define DSP_TOPSM_RM_CLK_SETTLE              ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0000)) /* Resource Manager Clock Settling Setting */
#define DSP_TOPSM_PROTECT_ACK_MASK           ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0830)) /* Resource Manager Timer PLL Enable Control 0 */
#define DSP_TOPSM_CCF_CLK_CON                ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0200)) /* Clock Control Register */
#define DSP_TOPSM_SM_REQ_MASK                ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x08B0)) /* Sleep Manager Request Source Mask */
#define DSP_TOPSM_RM_SM_MASK                 ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0890)) /*Sleep Manager Enable Control*/
#define DSP_TOPSM_RM_SM_PWR                  ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0850)) /*SLeep Manager  Power Enable Control*/
#define DSP_TOPSM_RM_SM_PLL_MASK0            ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0840)) /*  Sleep Manager PLL Enable Control  */
#define DSP_TOPSM_SM_SW_WAKEUP	             ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x08A0))   /*Sleep Manager Software Wake Up When set those bits, hardware will assert resource request to other topsm.*/
#define DSP_TOPSM_SM_MAS_RDY_STA             ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x08F0))   /*Sleep Manager Ready Status Specify subsys topsm receive ready from other topsm */
#define DSP_TOPSM_RM_SM_TRG                  ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x0860))   /*Sleep Manager cross trigger control */
#define DSP_TOPSM_RM_TMR_SSTA                ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0x40))  /*Resource Manager Timer Sleep Control State*/
#define DSP_TOPSM_SW_CLK_FORCE_ON_SET        ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0xA04))  /*Software Clock Force On Set*/
#define DSP_TOPSM_SW_CLK_FORCE_ON_CLR        ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0xA08))  /*Software Clock Force On Clear*/
#define DSP_TOPSM_INDIV_CLK_PROTECT_ACK_MASK ((volatile uint32_t*)(MODEM2G_TOPSM_BASE+0xA34))

void hal_dsp_topsm_get_resource(hal_dsp_topsm_Resource_Module_t module , bool config);
void dsp_topsm_init(void);

#endif /* HAL_SLEEP_MANAGER_ENABLED */
#endif /* __HAL_DSP_TOPSM_H__ */
