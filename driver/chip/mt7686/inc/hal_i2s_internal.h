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

#ifndef __HAL_I2S_INTERNAL_H__
#define __HAL_I2S_INTERNAL_H__

#include "hal_i2s.h"

#if defined(HAL_I2S_MODULE_ENABLED)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mt7686.h"
#include "hal_pdma_internal.h"

/*Enable XPLL or not, if not, XTAL will be used for i2s0/i2s1*/
#define     XPLL_ENABLE

#define     I2S_STATE_IDLE              0
#define     I2S_STATE_INIT              1
#define     I2S_STATE_RX_RUNNING        2
#define     I2S_STATE_TX_RUNNING        3

#define     DRV_Reg32(addr)             (*(volatile uint32_t*)(addr))
#define     DRV_Reg16(addr)             (*(volatile uint16_t *)(addr))
#define     DRV_Reg8(addr)              (*(volatile uint8_t*)(addr))
#define     I2S_CHECK_BIT(var,pos)       ((var) & (1<<(pos)))


/****************************************XPLL Control mask**********************************************/
//XPLL_CTL4
#define     AD_RGS_PLL_VCO_STATE_MASK                           0x000000FC
#define     AD_RGS_PLL_VCOCAL_CPLT_MASK                         0x00000002
#define     AD_RGS_PLL_VCOCAL_CPLT                              0x00000002
#define     AD_RGS_PLL_VCOCAL_FAIL_MASK                         0x00000001
#define     AD_RGS_PLL_VCOCAL_FAIL                              0x00000001

/****************************************REGISTER***********************************************/
#define     AUDIO_TOP_AHB_BASE                                  AUDIO_AHB_BASE//i2s 0,0xA1010000
#define     ASYS_TOP_AHB_BASE                                   ASYS_AHB_BASE//i2s 1,0xA1020000
#define     AUDIO_TOP_TX_DMA_FIFO                               (DMA_AHB_BASE+0x0000)//ch7
#define     AUDIO_TOP_RX_DMA_FIFO                               (DMA_AHB_BASE+0x0100)//ch8
#define     ASYS_TOP_TX_DMA_FIFO                                (DMA_AHB_BASE+0x0200)//ch9
#define     ASYS_TOP_RX_DMA_FIFO                                (DMA_AHB_BASE+0x0300)//ch10

#define     CKSYS_XTAL_FREQ__F_F_FXO_IS_26M                     0xA20202A3
/*clock gating clear*/
#define     FPGA_PDN_CLRD0                                      0xA21D0320


/*  I2S0  REG */
#define     AUDIO_TOP_BASE                                      AUDIO_BASE//0xA0070000
#define     I2S_GLOBAL_CONTROL                                  (AUDIO_TOP_BASE+0x0000)  // apb32
#define     I2S_GLOBAL_CONTROL__F_CR_I2S_GLOBAL_CONTROL         (AUDIO_TOP_BASE+0x0000)  // apb32:32
#define     I2S_DL_CONTROL                                      (AUDIO_TOP_BASE+0x0004) // apb32
#define     I2S_DL_CONTROL__F_CR_I2S_DL_CONTROL                 (AUDIO_TOP_BASE+0x0004) // apb32:32
#define     I2S_UL_CONTROL                                      (AUDIO_TOP_BASE+0x0008) // apb32
#define     I2S_UL_CONTROL__F_CR_I2S_UL_CONTROL                 (AUDIO_TOP_BASE+0x0008) // apb32:32
#define     I2S_SOFT_RESET                                      (AUDIO_TOP_BASE+0x000C) // apb32
#define     I2S_SOFT_RESET__F_CR_SOFT_RSTB                      (AUDIO_TOP_BASE+0x000C) // apb8:1
#define     I2S_DL_FIFO                                         (AUDIO_TOP_BASE+0x0010) // apb32
#define     I2S_DL_FIFO__F_CR_FIFO_DL_W_THRESHOLD               (AUDIO_TOP_BASE+0x0013) // apb8:4
#define     I2S_DL_FIFO__F_CR_FIFO_DL_WCLEAR                    (AUDIO_TOP_BASE+0x0012) // apb8:1
#define     I2S_DL_FIFO__F_CR_FIFO_DL_R_THRESHOLD               (AUDIO_TOP_BASE+0x0011) // apb8:4
#define     I2S_DL_FIFO__F_CR_FIFO_DL_RCLEAR                    (AUDIO_TOP_BASE+0x0010) // apb8:1
#define     I2S_UL_FIFO                                         (AUDIO_TOP_BASE+0x0014) // apb32
#define     I2S_UL_FIFO__F_CR_FIFO_UL_W_THRESHOLD               (AUDIO_TOP_BASE+0x0017) // apb8:4
#define     I2S_UL_FIFO__F_CR_FIFO_UL_WCLEAR                    (AUDIO_TOP_BASE+0x0016) // apb8:1
#define     I2S_UL_FIFO__F_CR_FIFO_UL_R_THRESHOLD               (AUDIO_TOP_BASE+0x0015) // apb8:4
#define     I2S_UL_FIFO__F_CR_FIFO_UL_RCLEAR                    (AUDIO_TOP_BASE+0x0014) // apb8:1
#define     I2S_DL_FIFO_STATUS                                  (AUDIO_TOP_BASE+0x0018) // apb32
#define     I2S_DL_FIFO_STATUS__F_CR_FIFO_DL_WFIFO_CNT          (AUDIO_TOP_BASE+0x001A) // apb8:4
#define     I2S_DL_FIFO_STATUS__F_CR_FIFO_DL_RFIFO_CNT          (AUDIO_TOP_BASE+0x0019) // apb8:4
#define     I2S_DL_FIFO_STATUS__F_CR_FIFO_DL_STATUS             (AUDIO_TOP_BASE+0x0018) // apb8:6
#define     I2S_UL_FIFO_STATUS                                  (AUDIO_TOP_BASE+0x001C) // apb32
#define     I2S_UL_FIFO_STATUS__F_CR_FIFO_UL_WFIFO_CNT          (AUDIO_TOP_BASE+0x001E) // apb8:4
#define     I2S_UL_FIFO_STATUS__F_CR_FIFO_UL_RFIFO_CNT          (AUDIO_TOP_BASE+0x001D) // apb8:4
#define     I2S_UL_FIFO_STATUS__F_CR_FIFO_UL_STATUS             (AUDIO_TOP_BASE+0x001C) // apb8:6
#define     I2S_SCAN_RSV                                        (AUDIO_TOP_BASE+0x0020) // apb32
#define     I2S_SCAN_RSV__F_CR_RESERVED                         (AUDIO_TOP_BASE+0x0020) // apb32:32
#define     I2S_GLOBAL_EN_CONTROL                               (AUDIO_TOP_BASE+0x0030) // apb32
#define     I2S_GLOBAL_EN_CONTROL__F_CR_PDN_AUD_26M             (AUDIO_TOP_BASE+0x0033) // apb8:1
#define     I2S_GLOBAL_EN_CONTROL__F_CR_UL_FIFO_EN              (AUDIO_TOP_BASE+0x0032) // apb8:1
#define     I2S_GLOBAL_EN_CONTROL__F_CR_DL_FIFO_EN              (AUDIO_TOP_BASE+0x0031) // apb8:1
#define     I2S_GLOBAL_EN_CONTROL__F_CR_ENABLE                  (AUDIO_TOP_BASE+0x0030) // apb8:1
#define     I2S_DL_SR_EN_CONTROL                                (AUDIO_TOP_BASE+0x0034) // apb32
#define     I2S_DL_SR_EN_CONTROL__F_CR_PDN_I2SO                 (AUDIO_TOP_BASE+0x0036) // apb8:1
#define     I2S_DL_SR_EN_CONTROL__F_CR_I2S_OUT_SR               (AUDIO_TOP_BASE+0x0035) // apb8:5
#define     I2S_DL_SR_EN_CONTROL__F_CR_I2S_OUT_EN               (AUDIO_TOP_BASE+0x0034) // apb8:1
#define     I2S_UL_SR_EN_CONTROL                                (AUDIO_TOP_BASE+0x0038) // apb32
#define     I2S_UL_SR_EN_CONTROL__F_CR_PDN_I2SIN                (AUDIO_TOP_BASE+0x003a) // apb8:1
#define     I2S_UL_SR_EN_CONTROL__F_CR_I2S_IN_SR                (AUDIO_TOP_BASE+0x0039) // apb8:5
#define     I2S_UL_SR_EN_CONTROL__F_CR_I2S_IN_EN                (AUDIO_TOP_BASE+0x0038) // apb8:1
#define     I2S_DL_INT_CONTROL                                  (AUDIO_TOP_BASE+0x0040) // apb32
#define     I2S_DL_INT_CONTROL__F_CR_DL_INTSTS_INT              (AUDIO_TOP_BASE+0x0041) // apb8:1
#define     I2S_DL_INT_CONTROL__F_CR_DL_CONTRL_ITEN             (AUDIO_TOP_BASE+0x0040) // apb8:1
#define     I2S_UL_INT_CONTROL                                  (AUDIO_TOP_BASE+0x0044) // apb32
#define     I2S_UL_INT_CONTROL__F_CR_UL_INTSTS_INT              (AUDIO_TOP_BASE+0x0045) // apb8:1
#define     I2S_UL_INT_CONTROL__F_CR_UL_CONTRL_ITEN             (AUDIO_TOP_BASE+0x0044) // apb8:1
#define     I2S_INT_ACK_CONTROL                                 (AUDIO_TOP_BASE+0x0048) // apb32
#define     I2S_INT_ACK_CONTROL__F_CR_DL_ACKINT_ACK             (AUDIO_TOP_BASE+0x0049) // apb8:1
#define     I2S_INT_ACK_CONTROL__F_CR_UL_ACKINT_ACK             (AUDIO_TOP_BASE+0x0048) // apb8:1


/*  I2S1 REG */
#define     ASYS_TOP_BASE                                       ASYS_BASE//0xA0080000
#define     I2S1_GLOBAL_CONTROL                                 (ASYS_TOP_BASE+0x0000) // apb32
#define     I2S1_GLOBAL_CONTROL__F_CR_I2S1_GLOBAL_CONTROL       (ASYS_TOP_BASE+0x0000) // apb32:32
#define     I2S1_DL_CONTROL                                     (ASYS_TOP_BASE+0x0004) // apb32
#define     I2S1_DL_CONTROL__F_CR_I2S1_DL_CONTROL               (ASYS_TOP_BASE+0x0004) // apb32:32
#define     I2S1_UL_CONTROL                                     (ASYS_TOP_BASE+0x0008) // apb32
#define     I2S1_UL_CONTROL__F_CR_I2S1_UL_CONTROL               (ASYS_TOP_BASE+0x0008) // apb32:32
#define     I2S1_SOFT_RESET                                     (ASYS_TOP_BASE+0x000C) // apb32
#define     I2S1_SOFT_RESET__F_CR_I2S1_SOFT_RSTB                (ASYS_TOP_BASE+0x000C) // apb8:1
#define     I2S1_DL_FIFO                                        (ASYS_TOP_BASE+0x0010) // apb32
#define     I2S1_DL_FIFO__F_CR_I2S1_FIFO_DL_W_THRESHOLD         (ASYS_TOP_BASE+0x0013) // apb8:4
#define     I2S1_DL_FIFO__F_CR_I2S1_FIFO_DL_WCLEAR              (ASYS_TOP_BASE+0x0012) // apb8:1
#define     I2S1_DL_FIFO__F_CR_I2S1_FIFO_DL_R_THRESHOLD         (ASYS_TOP_BASE+0x0011) // apb8:4
#define     I2S1_DL_FIFO__F_CR_I2S1_FIFO_DL_RCLEAR              (ASYS_TOP_BASE+0x0010) // apb8:1
#define     I2S1_UL_FIFO                                        (ASYS_TOP_BASE+0x0014) // apb32
#define     I2S1_UL_FIFO__F_CR_I2S1_FIFO_UL_W_THRESHOLD         (ASYS_TOP_BASE+0x0017) // apb8:4
#define     I2S1_UL_FIFO__F_CR_I2S1_FIFO_UL_WCLEAR              (ASYS_TOP_BASE+0x0016) // apb8:1
#define     I2S1_UL_FIFO__F_CR_I2S1_FIFO_UL_R_THRESHOLD         (ASYS_TOP_BASE+0x0015) // apb8:4
#define     I2S1_UL_FIFO__F_CR_I2S1_FIFO_UL_RCLEAR              (ASYS_TOP_BASE+0x0014) // apb8:1
#define     I2S1_DL_FIFO_STATUS                                 (ASYS_TOP_BASE+0x0018) // apb32
#define     I2S1_DL_FIFO_STATUS__F_CR_I2S1_FIFO_DL_WFIFO_CNT    (ASYS_TOP_BASE+0x001A) // apb8:4
#define     I2S1_DL_FIFO_STATUS__F_CR_I2S1_FIFO_DL_RFIFO_CNT    (ASYS_TOP_BASE+0x0019) // apb8:4
#define     I2S1_DL_FIFO_STATUS__F_CR_I2S1_FIFO_DL_STATUS       (ASYS_TOP_BASE+0x0018) // apb8:6
#define     I2S1_UL_FIFO_STATUS                                 (ASYS_TOP_BASE+0x001C) // apb32
#define     I2S1_UL_FIFO_STATUS__F_CR_I2S1_FIFO_UL_WFIFO_CNT    (ASYS_TOP_BASE+0x001E) // apb8:4
#define     I2S1_UL_FIFO_STATUS__F_CR_I2S1_FIFO_UL_RFIFO_CNT    (ASYS_TOP_BASE+0x001D) // apb8:4
#define     I2S1_UL_FIFO_STATUS__F_CR_I2S1_FIFO_UL_STATUS       (ASYS_TOP_BASE+0x001C) // apb8:6
#define     I2S1_SCAN_RSV                                       (ASYS_TOP_BASE+0x0020) // apb32
#define     I2S1_SCAN_RSV__F_CR_I2S1_RESERVED                   (ASYS_TOP_BASE+0x0020) // apb32:32
#define     I2S1_GLOBAL_EN_CONTROL                              (ASYS_TOP_BASE+0x0030) // apb32
#define     I2S1_GLOBAL_EN_CONTROL__F_CR_PDN_AUD_26M            (ASYS_TOP_BASE+0x0033) // apb8:1
#define     I2S1_GLOBAL_EN_CONTROL__F_CR_I2S1_UL_FIFO_EN        (ASYS_TOP_BASE+0x0032) // apb8:1
#define     I2S1_GLOBAL_EN_CONTROL__F_CR_I2S1_DL_FIFO_EN        (ASYS_TOP_BASE+0x0031) // apb8:1
#define     I2S1_GLOBAL_EN_CONTROL__F_CR_I2S1_ENABLE            (ASYS_TOP_BASE+0x0030) // apb8:1
#define     I2S1_DL_SR_EN_CONTROL                               (ASYS_TOP_BASE+0x0034) // apb32
#define     I2S1_DL_SR_EN_CONTROL__F_CR_PDN_I2SO1               (ASYS_TOP_BASE+0x0036) // apb8:1
#define     I2S1_DL_SR_EN_CONTROL__F_CR_I2S1_OUT_SR             (ASYS_TOP_BASE+0x0035) // apb8:5
#define     I2S1_DL_SR_EN_CONTROL__F_CR_I2S1_OUT_EN             (ASYS_TOP_BASE+0x0034) // apb8:1
#define     I2S1_UL_SR_EN_CONTROL                               (ASYS_TOP_BASE+0x0038) // apb32
#define     I2S1_UL_SR_EN_CONTROL__F_CR_PDN_I2SIN1              (ASYS_TOP_BASE+0x003A) // apb8:1
#define     I2S1_UL_SR_EN_CONTROL__F_CR_I2S1_IN_SR              (ASYS_TOP_BASE+0x0039) // apb8:5
#define     I2S1_UL_SR_EN_CONTROL__F_CR_I2S1_IN_EN              (ASYS_TOP_BASE+0x0038) // apb8:1
#define     I2S_MONITOR                                         (ASYS_TOP_BASE+0x003C) // apb32
#define     I2S_MONITOR__F_CR_I2S_MONITOR                       (ASYS_TOP_BASE+0x003C) // apb32:32

/*XPLL REG*/
#define     XPLL_DBG_PROB                                       (XPLL_CTRL_BASE+0x0400) // apb32
#define     XPLL_DBG_PROB__F_XPLL_FQMTR_EN                      (XPLL_CTRL_BASE+0x0403) // apb8:1
#define     XPLL_DBG_PROB__F_XPLL_DBG_PROB                      (XPLL_CTRL_BASE+0x0402) // apb8:4
#define     XPLL_DBG_PROB__F_XPLL_DBG_SEL                       (XPLL_CTRL_BASE+0x0400) // apb8:4
#define     XPLL_CTL0                                           (XPLL_CTRL_BASE+0x0A80) // apb32
#define     XPLL_CTL0__F_XPLL_CTL0                              (XPLL_CTRL_BASE+0x0A80) // apb16:16
#define     XPLL_CTL1                                           (XPLL_CTRL_BASE+0x0A84) // apb32
#define     XPLL_CTL1__F_XPLL_CTL1                              (XPLL_CTRL_BASE+0x0A84) // apb16:16
#define     XPLL_CTL2                                           (XPLL_CTRL_BASE+0x0A88) // apb32
#define     XPLL_CTL2__F_XPLL_CTL2                              (XPLL_CTRL_BASE+0x0A88) // apb16:16
#define     XPLL_CTL3                                           (XPLL_CTRL_BASE+0x0A8c) // apb32
#define     XPLL_CTL3__F_XPLL_CTL3                              (XPLL_CTRL_BASE+0x0A8c) // apb32:32
#define     XPLL_CTL4                                           (XPLL_CTRL_BASE+0x0A90) // apb32
#define     XPLL_CTL4__F_XPLL_CTL4                              (XPLL_CTRL_BASE+0x0A90) // apb8:8
#define     XPLL_CTL8                                           (XPLL_CTRL_BASE+0x0AA0) // apb32
#define     XPLL_CTL8__F_XPLL_CTL8                              (XPLL_CTRL_BASE+0x0AA0) // apb32:19

/*VDMA*/
/*
*DMA7	Virtual FIFO    AUDTOP TX
*DMA8	Virtual FIFO	AUDTOP RX
*DMA9	Virtual FIFO	ASYSTOP TX
*DMA10	Virtual FIFO	ASYSTOP RX
*/

#define     VDMA7_COUNT                                         (DMA_BASE+0x0710) // apb32
#define     VDMA7_CON                                           (DMA_BASE+0x0714) // apb32
#define     VDMA7_START                                         (DMA_BASE+0x0718) // apb32
#define     VDMA7_PGMADDR                                       (DMA_BASE+0x072c) // apb32
#define     VDMA7_FFCNT                                         (DMA_BASE+0x0738) // apb32
#define     VDMA7_FFSIZE                                        (DMA_BASE+0x0744) // apb32
#define     VDMA8_COUNT                                         (DMA_BASE+0x0810) // apb32
#define     VDMA8_CON                                           (DMA_BASE+0x0814) // apb32
#define     VDMA8_START                                         (DMA_BASE+0x0818) // apb32
#define     VDMA8_PGMADDR                                       (DMA_BASE+0x082c) // apb32
#define     VDMA8_FFCNT                                         (DMA_BASE+0x0838) // apb32
#define     VDMA8_FFSIZE                                        (DMA_BASE+0x0844) // apb32
#define     VDMA9_COUNT                                         (DMA_BASE+0x0910) // apb32
#define     VDMA9_CON                                           (DMA_BASE+0x0914) // apb32
#define     VDMA9_START                                         (DMA_BASE+0x0918) // apb32
#define     VDMA9_PGMADDR                                       (DMA_BASE+0x092c) // apb32
#define     VDMA9_FFCNT                                         (DMA_BASE+0x0938) // apb32
#define     VDMA9_FFSIZE                                        (DMA_BASE+0x0944) // apb32
#define     VDMA10_COUNT                                        (DMA_BASE+0x0a10) // apb32
#define     VDMA10_CON                                          (DMA_BASE+0x0a14) // apb32
#define     VDMA10_START                                        (DMA_BASE+0x0a18) // apb32
#define     VDMA10_PGMADDR                                      (DMA_BASE+0x0a2c) // apb32
#define     VDMA10_FFCNT                                        (DMA_BASE+0x0a38) // apb32
#define     VDMA10_FFSIZE                                       (DMA_BASE+0x0a44) // apb32



/**************************Global variable***************************************************/

/** @MCLK SOURCE*/
typedef enum {
    HAL_I2S_CLOCK_XO_26M         = 0,   /**< XTAL 26MHZ. */
    HAL_I2S_CLOCK_XPLL_26M       = 1,   /**< XPLL 26MHZ. */
    HAL_I2S_CLOCK_XPLL_24_576M   = 2,   /**< XPLL 24.576MHZ. */
    HAL_I2S_CLOCK_XPLL_22_5792M  = 3,    /**< XPLL 22.5792MHZ. */
    HAL_I2S_CLOCK_MAX            = 4
} hal_i2s_clock_source_t;

typedef enum {
    HAL_I2S_FS_BASE_8KHZ         = 0,  /**< FS  is a multiple of 8khz. */
    HAL_I2S_FS_BASE_11_025KHZ    = 1   /**< FS  is a multiple of 11.025khz. */
} hal_i2s_fs_base_t;


typedef struct {
    uint32_t           *tx_vfifo_base;  //< Tx buffer- internal use
    uint32_t            tx_vfifo_length;
    uint32_t            tx_vfifo_threshold;
    bool                tx_dma_interrupt;
    bool                tx_dma_configured;

    uint32_t           *rx_vfifo_base;  //< Rx buffer- internal use
    uint32_t            rx_vfifo_length;
    uint32_t            rx_vfifo_threshold;
    bool                rx_dma_interrupt;
    bool                rx_dma_configured;
} i2s_vfifo_t;

typedef struct {
    hal_i2s_port_t                     i2s_port;
    hal_i2s_config_t                   i2s_user_config;
    hal_i2s_initial_type_t             i2s_initial_type;
    hal_i2s_clock_source_t             i2s_clock_source;
    /* state control */
    volatile uint8_t                   i2s_state;
    i2s_vfifo_t                        i2s_vfifo;
    bool                               i2s_configured;
    bool                               i2s_audiotop_enabled;
    /* user defined callback functions */
    hal_i2s_tx_callback_t              user_tx_callback_func;
    hal_i2s_rx_callback_t              user_rx_callback_func;
    void                               *user_tx_data;
    void                               *user_rx_data;
} i2s_internal_t;

void i2s_xpll_open(i2s_internal_t *hal_i2s);
void i2s_xpll_close(void);
void i2s_set_initial_parameter(i2s_internal_t *hal_i2s);
void i2s_set_parameter(i2s_internal_t *hal_i2s);
hal_i2s_status_t i2s_set_clock(i2s_internal_t *hal_i2s, hal_i2s_sample_rate_t sample_rate);

#ifdef __cplusplus
}
#endif

#endif  /* defined(HAL_I2S_MODULE_ENABLED)*/
#endif /* __HAL_I2S_INTERNAL_H__ */

