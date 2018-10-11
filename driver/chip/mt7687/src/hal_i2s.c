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

#include "hal_i2s.h"
#ifdef HAL_I2S_MODULE_ENABLED
#include "hal_i2s_internal.h"
#include "hal_log.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"
#endif

#define     I2S_CHECK_BIT(var,pos)       ((var) & (1<<(pos)))

static i2s_internal_t      i2s_internal_cfg;
static hal_i2s_status_t    i2s_is_sample_rate(hal_i2s_sample_rate_t sample_rate, bool blinput);
static void                i2s_dma_callback(bool is_rx);
static void                i2x_tx_handler(void);
static void                i2x_rx_handler(void);

#ifdef HAL_SLEEP_MANAGER_ENABLED
static uint8_t             sleep_handler = 0;
void                       i2s_enter_suspend(void);
void                       i2s_enter_resume(void);
#endif

static void i2s_query_tx_vfifo_empty(void)
{
    //log_hal_info("i2s_query_tx_vfifo_empty\r\n");
    while (1) {
        if ((I2S_CHECK_BIT(i2s_internal_cfg.i2s_state, I2S_STATE_TX_RUNNING) != 0) &&
                (i2s_internal_cfg.i2s_vfifo.tx_dma_configured == true) &&
                (i2s_internal_cfg.i2s_audiotop_enabled == true)) {
            if (I2sGetVdmaTxFifoCnt() == 0) {
                break;
            }
        } else {
            break;
        }
    }
}

hal_i2s_status_t hal_i2s_init(hal_i2s_initial_type_t i2s_initial_type)
{

    if (i2s_internal_cfg.i2s_state != I2S_STATE_IDLE) {
        return HAL_I2S_STATUS_ERROR;
    }

    switch (i2s_initial_type) {

        //---only for test case use----
        case HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE:
            i2s_internal_cfg.I2S_INITIAL_TYPE = I2S_INTERNAL_LOOPBACK_MODE;
            i2s_internal_cfg.i2s_initial_type = HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE;
            break;
        //external codec , i2s slave mode
        case HAL_I2S_TYPE_EXTERNAL_MODE:
            i2s_internal_cfg.I2S_INITIAL_TYPE = I2S_EXTERNAL_MODE;
            i2s_internal_cfg.i2s_initial_type = HAL_I2S_TYPE_EXTERNAL_MODE;
            break;
        case HAL_I2S_TYPE_INTERNAL_MODE:
        case HAL_I2S_TYPE_EXTERNAL_TDM_MODE:
            return HAL_I2S_STATUS_INVALID_PARAMETER;

        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }


    if (I2sCfgInitialSetting(
                i2s_internal_cfg.I2S_INITIAL_TYPE,
                &i2s_internal_cfg.I2SGblCfg,
                &i2s_internal_cfg.I2SDLCfg,
                &i2s_internal_cfg.I2SULCfg)) {
        i2s_internal_cfg.i2s_state = I2S_STATE_INIT;
        i2s_internal_cfg.i2s_configured = false;
        i2s_internal_cfg.i2s_audiotop_enabled = false;
        return HAL_I2S_STATUS_OK;
    } else {
        return HAL_I2S_STATUS_ERROR;
    }

}


hal_i2s_status_t hal_i2s_deinit(void)
{
    if (i2s_internal_cfg.i2s_state != I2S_STATE_INIT) {
        return HAL_I2S_STATUS_ERROR;
    }

    //xpllClose();
    i2s_internal_cfg.i2s_state =  I2S_STATE_IDLE;
    i2s_internal_cfg.i2s_configured = false;
    i2s_internal_cfg.i2s_audiotop_enabled = false;
    if (I2sReset()) {
        return HAL_I2S_STATUS_OK;
    } else {
        return HAL_I2S_STATUS_ERROR;
    }
}


hal_i2s_status_t hal_i2s_get_config(hal_i2s_config_t *config)
{

    *config = i2s_internal_cfg.i2s_user_config;
    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_set_config(const hal_i2s_config_t *config)
{

    if (i2s_internal_cfg.i2s_state != I2S_STATE_INIT) {
        log_hal_error("i2s_state != I2S_STATE_INIT , i2s_state =%d\r\n", (unsigned char)i2s_internal_cfg.i2s_state);
        return HAL_I2S_STATUS_ERROR;
    }

    if (NULL == config) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    //enable DMA for i2s
    DMA_Init();
    DMA_Vfifo_init();

    //---check Both sample rates are same value
    if ((config->i2s_out.sample_rate) != (config->i2s_in.sample_rate)) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    //----set tx sample rate----
    if (i2s_is_sample_rate(config->i2s_out.sample_rate, false) != HAL_I2S_STATUS_OK) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    //----set rx sample rate----
    if (i2s_is_sample_rate(config->i2s_in.sample_rate, true) != HAL_I2S_STATUS_OK) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    //---check whether both tx channerl number and tx mode are set to enable at same time
    if ((config->i2s_out.channel_number == HAL_I2S_STEREO) && (config->tx_mode == HAL_I2S_TX_MONO_DUPLICATE_ENABLE)) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    //----set tx channel number----
    switch (config->i2s_out.channel_number) {
        case HAL_I2S_MONO:
            i2s_internal_cfg.I2SGblCfg.ucI2SDLMonoStereoSel = I2S_DL_MONO_MODE;
            break;
        case HAL_I2S_STEREO:
            i2s_internal_cfg.I2SGblCfg.ucI2SDLMonoStereoSel = I2S_DL_STEREO_MODE;
            break;
        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    //----set rx down rate mode----(disable this function)
    switch (config->rx_down_rate) {
        case HAL_I2S_RX_DOWN_RATE_ENABLE:
            //i2s_internal_cfg.I2SULCfg.bI2SDownRateEn = I2S_UL_DOWN_RATE_EN;
            i2s_internal_cfg.I2SULCfg.bI2SDownRateEn = I2S_UL_DOWN_RATE_DIS;
            break;
        default:
            i2s_internal_cfg.I2SULCfg.bI2SDownRateEn = I2S_UL_DOWN_RATE_DIS;
            break;
    }

    //----set tx mode----
    switch (config->tx_mode) {
        case HAL_I2S_TX_MONO_DUPLICATE_ENABLE:
            i2s_internal_cfg.I2SGblCfg.bI2SDLMonoDupEn = I2S_DL_MONO_DUP_EN;
            break;
        default:
            i2s_internal_cfg.I2SGblCfg.bI2SDLMonoDupEn = I2S_DL_MONO_DUP_DIS;
            break;
    }

    //----set clock mode----
    //----default: slave mode
    switch (config->clock_mode) {
        case HAL_I2S_MASTER:
            //reserve for internal loopback mode test
            i2s_internal_cfg.I2SDLCfg.ucI2SSRC = I2S_DL_SRC_MASTER;
            i2s_internal_cfg.I2SULCfg.ucI2SSRC = I2S_UL_SRC_MASTER;
            break;
        case HAL_I2S_SLAVE:
            i2s_internal_cfg.I2SDLCfg.ucI2SSRC = I2S_DL_SRC_SLAVE;
            i2s_internal_cfg.I2SULCfg.ucI2SSRC = I2S_UL_SRC_SLAVE;
            break;
        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    //sample width
    //fix for 7687 i2s
    i2s_internal_cfg.i2s_user_config.sample_width = HAL_I2S_SAMPLE_WIDTH_16BIT;
    //frame sync width
    i2s_internal_cfg.i2s_user_config.frame_sync_width = HAL_I2S_FRAME_SYNC_WIDTH_32;

    //---set TX LR swap----
    //---default: disable
    switch (config->i2s_out.lr_swap) {
        case HAL_I2S_LR_SWAP_ENABLE:
            i2s_internal_cfg.I2SGblCfg.ucI2SDLSwapLR = 1;
            break;
        default:
            i2s_internal_cfg.I2SGblCfg.ucI2SDLSwapLR = 0;
            break;
    }

    //---set RX LR swap----
    //---default: disable
    switch (config->i2s_in.lr_swap) {
        case HAL_I2S_LR_SWAP_ENABLE:
            i2s_internal_cfg.I2SULCfg.u4I2SLRSwap = 1;
            break;
        default:
            i2s_internal_cfg.I2SULCfg.u4I2SLRSwap = 0;
            break;
    }

    //---set BCLK clock inverse---
    //Enable or disable BCLK clock inverting
    //i2s_internal_cfg.I2SGblCfg.bI2SClkInvEn=config->bclk_inv;

    //---set Negative edge capture RX data---
    //Enable or disable the read data can be captured either on the positive or negative edge of BCLK
    //i2s_internal_cfg.I2SGblCfg.bI2SNegCapEn=config->neg_cap;

    //---set BCLK clock inverse---

    //---set TX msb_offset----
    i2s_internal_cfg.I2SDLCfg.u4I2SMsbOffset = config->i2s_out.msb_offset;

    //---set RX msb_offset----
    i2s_internal_cfg.I2SULCfg.u4I2SMsbOffset = config->i2s_in.msb_offset;

    //---set TX word_select_inverse----
    switch (config->i2s_out.word_select_inverse) {
        case HAL_I2S_WORD_SELECT_INVERSE_ENABLE:
            i2s_internal_cfg.I2SDLCfg.ucI2SWordSelInv = 1;
            break;
        default:
            i2s_internal_cfg.I2SDLCfg.ucI2SWordSelInv = 0;
            break;
    }

    //---set RX word_select_inverse----
    switch (config->i2s_in.word_select_inverse) {
        case HAL_I2S_WORD_SELECT_INVERSE_ENABLE:
            i2s_internal_cfg.I2SULCfg.ucI2SWordSelInv = 1;
            break;
        default:
            i2s_internal_cfg.I2SULCfg.ucI2SWordSelInv = 0;
            break;
    }

    I2sSetGblCfg(&i2s_internal_cfg.I2SGblCfg);
    I2sSetDlCfg(&i2s_internal_cfg.I2SDLCfg);
    I2sSetUlCfg(&i2s_internal_cfg.I2SULCfg);

    i2s_internal_cfg.i2s_user_config = *config;
    /*
     //dump register and setting
     printf("i2s_internal_cfg.i2s_user_config.clock_mode=%d\r\n",i2s_internal_cfg.i2s_user_config.clock_mode);
     printf("i2s_internal_cfg.i2s_user_config.rx_down_rate=%d\r\n",i2s_internal_cfg.i2s_user_config.rx_down_rate);
     printf("i2s_internal_cfg.i2s_user_config.tx_mode=%d\r\n",i2s_internal_cfg.i2s_user_config.tx_mode);
     printf("i2s_internal_cfg.i2s_user_config.i2s_in.channel_number=%d\r\n",i2s_internal_cfg.i2s_user_config.i2s_in.channel_number);
     printf("i2s_internal_cfg.i2s_user_config.i2s_in.lr_swap=%d\r\n",i2s_internal_cfg.i2s_user_config.i2s_in.lr_swap);
     printf("i2s_internal_cfg.i2s_user_config.i2s_in.msb_offset=%d\r\n",i2s_internal_cfg.i2s_user_config.i2s_in.msb_offset);
     printf("i2s_internal_cfg.i2s_user_config.i2s_in.sample_rate=%d\r\n",i2s_internal_cfg.i2s_user_config.i2s_in.sample_rate);
     printf("i2s_internal_cfg.i2s_user_config.i2s_in.word_select_inverse=%d\r\n",i2s_internal_cfg.i2s_user_config.i2s_in.word_select_inverse);
     printf("i2s_internal_cfg.i2s_user_config.i2s_out.channel_number=%d\r\n",i2s_internal_cfg.i2s_user_config.i2s_out.channel_number);
     printf("i2s_internal_cfg.i2s_user_config.i2s_out.lr_swap=%d\r\n",i2s_internal_cfg.i2s_user_config.i2s_out.lr_swap);
     printf("i2s_internal_cfg.i2s_user_config.i2s_out.msb_offset=%d\r\n",i2s_internal_cfg.i2s_user_config.i2s_out.msb_offset);
     printf("i2s_internal_cfg.i2s_user_config.i2s_out.sample_rate=%d\r\n",i2s_internal_cfg.i2s_user_config.i2s_out.sample_rate);
     printf("i2s_internal_cfg.i2s_user_config.i2s_out.word_select_inverse=%d\r\n",i2s_internal_cfg.i2s_user_config.i2s_out.word_select_inverse);

     printf("I2S_GLB_CONTROL_ADDR=%x\r\n",DRV_Reg32(I2S_GLB_CONTROL_ADDR));
     printf("I2S_DL_CONTROL_ADDR=%x\r\n",DRV_Reg32(I2S_DL_CONTROL_ADDR));
     printf("I2S_UL_CONTROL_ADDR=%x\r\n",DRV_Reg32(I2S_UL_CONTROL_ADDR));
     */
    i2s_internal_cfg.i2s_configured = true;

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t  hal_i2s_enable_audio_top(void)
{
    //----Enable  audio_top----
    xpllOpen();
    I2sClkFifoEn(I2S_TRUE);
    i2s_internal_cfg.i2s_audiotop_enabled = true;

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t  hal_i2s_disable_audio_top(void)
{
    //----Disable audio_top----
    I2sClkFifoEn(I2S_FALSE);
    xpllClose();
    i2s_internal_cfg.i2s_audiotop_enabled = false;

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t  hal_i2s_enable_tx_dma_interrupt(void)
{
    if (i2s_internal_cfg.user_tx_callback_func == NULL) {
        return HAL_I2S_STATUS_ERROR;
    }

    //----Enable DMA interrupt for TX----
    I2sSetVdmaTxIntrEn(I2S_DMA_INTR_EN);
    i2s_internal_cfg.i2s_vfifo.tx_dma_interrupt = true;

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t  hal_i2s_disable_tx_dma_interrupt(void)
{

    //----Disable DMA interrupt for TX----
    I2sSetVdmaTxIntrEn(I2S_DMA_INTR_DIS);
    i2s_internal_cfg.i2s_vfifo.tx_dma_interrupt = false;

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t  hal_i2s_enable_rx_dma_interrupt(void)
{

    if (i2s_internal_cfg.user_rx_callback_func == NULL) {
        return HAL_I2S_STATUS_ERROR;
    }

    //----Enable  DMA interrupt for RX----
    I2sSetVdmaRxIntrEn(I2S_DMA_INTR_EN);
    i2s_internal_cfg.i2s_vfifo.rx_dma_interrupt = true;

    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t  hal_i2s_disable_rx_dma_interrupt(void)
{

    //----Disable DMA interrupt for RX----
    I2sSetVdmaRxIntrEn(I2S_DMA_INTR_DIS);
    i2s_internal_cfg.i2s_vfifo.rx_dma_interrupt = false;

    return HAL_I2S_STATUS_OK;
}



hal_i2s_status_t hal_i2s_get_tx_sample_count(uint32_t *sample_count)
{
    if (i2s_internal_cfg.i2s_vfifo.tx_vfifo_length == 0) {
        return HAL_I2S_STATUS_ERROR;
    }

    //---get free space in tx vfifo
    *sample_count = i2s_internal_cfg.i2s_vfifo.tx_vfifo_length - I2sGetVdmaTxFifoCnt();

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_get_rx_sample_count(uint32_t *sample_count)
{
    //---get the length to the received data in rx vfifo
    *sample_count = I2sGetVdmaRxFifoCnt();

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_setup_tx_vfifo(uint32_t *buffer, uint32_t threshold, uint32_t buffer_length)
{

    if (NULL == buffer) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg.i2s_state != I2S_STATE_INIT) {
        log_hal_error("i2s_state != I2S_STATE_INIT , i2s_state =%d\r\n", (unsigned char)i2s_internal_cfg.i2s_state);
        return HAL_I2S_STATUS_ERROR;
    }

    //----Initialize I2S hardware with Virtual FIFO DMA Tx----
    I2sVdmaTxStart((UINT32)buffer, buffer_length, (DMA_VFIFO_CHANNEL) 0, I2S_DMA_CVFF_DIS);

    //----Set the threshold  for Tx VDMA----
    I2sSetVdmaTxThreshold(threshold);

    i2s_internal_cfg.i2s_vfifo.tx_vfifo_base = (uint32_t *)buffer;
    i2s_internal_cfg.i2s_vfifo.tx_vfifo_length = buffer_length;
    i2s_internal_cfg.i2s_vfifo.tx_vfifo_threshold = threshold;
    i2s_internal_cfg.i2s_vfifo.tx_dma_configured = true;

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_setup_rx_vfifo(uint32_t *buffer, uint32_t threshold, uint32_t buffer_length)
{

    if (NULL == buffer) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg.i2s_state != I2S_STATE_INIT) {
        log_hal_error("i2s_state != I2S_STATE_INIT , i2s_state =%d\r\n", (unsigned char)i2s_internal_cfg.i2s_state);
        return HAL_I2S_STATUS_ERROR;
    }

    //----Initialize I2S hardware with Virtual FIFO DMA Rx----
    I2sVdmaRxStart((UINT32)buffer, buffer_length, (DMA_VFIFO_CHANNEL)0, I2S_DMA_CVFF_DIS);

    //----Set the threshold  for Rx VDMA----
    I2sSetVdmaRxThreshold(threshold);

    i2s_internal_cfg.i2s_vfifo.rx_vfifo_base = (uint32_t *)buffer;
    i2s_internal_cfg.i2s_vfifo.rx_vfifo_length = buffer_length;
    i2s_internal_cfg.i2s_vfifo.rx_vfifo_threshold = threshold;
    i2s_internal_cfg.i2s_vfifo.rx_dma_configured = true;

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t hal_i2s_stop_tx_vfifo(void)
{

    if (i2s_internal_cfg.i2s_vfifo.tx_dma_configured) {
        DMA_Stop((UINT8)VDMA_I2S_TX_CH);
    }

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_stop_rx_vfifo(void)
{
    if (i2s_internal_cfg.i2s_vfifo.rx_dma_configured) {
        DMA_Stop((UINT8)VDMA_I2S_RX_CH);
    }

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_register_tx_vfifo_callback(hal_i2s_tx_callback_t tx_callback, void *user_data)
{

    if (NULL == tx_callback) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    i2s_internal_cfg.user_tx_callback_func = tx_callback;
    i2s_internal_cfg.user_tx_data = user_data;

    DMA_Vfifo_Register_Callback(VDMA_I2S_TX_CH, i2x_tx_handler);

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t hal_i2s_register_rx_vfifo_callback(hal_i2s_rx_callback_t rx_callback, void *user_data)
{

    if (NULL == rx_callback) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    i2s_internal_cfg.user_rx_callback_func = rx_callback;
    i2s_internal_cfg.user_rx_data = user_data;

    DMA_Vfifo_Register_Callback(VDMA_I2S_RX_CH, i2x_rx_handler);

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t  hal_i2s_enable_tx(void)
{

    i2s_internal_cfg.i2s_state |= (1 << I2S_STATE_TX_RUNNING);

    I2sDlEn(KAL_TRUE);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    /*get sleep handler, just lock once*/
    if (sleep_handler == 0) {
        sleep_handler = hal_sleep_manager_set_sleep_handle("I2S");
        if (sleep_handler == INVALID_SLEEP_HANDLE) {
            log_hal_info("there's no available handle when I2S get sleep handle");
            return HAL_I2S_STATUS_ERROR;
        } else {
            hal_sleep_manager_lock_sleep(sleep_handler);/*lock sleep mode*/
            log_hal_info("[I2S] lock sleep successfully");
        }
    }
#endif
    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t  hal_i2s_enable_rx(void)
{
    i2s_internal_cfg.i2s_state |= (1 << I2S_STATE_RX_RUNNING);

    I2sUlEn(KAL_TRUE);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    /*get sleep handler, just lock once*/
    if (sleep_handler == 0) {
        sleep_handler = hal_sleep_manager_set_sleep_handle("I2S");
        if (sleep_handler == INVALID_SLEEP_HANDLE) {
            log_hal_info("there's no available handle when I2S get sleep handle");
            return HAL_I2S_STATUS_ERROR;
        } else {
            hal_sleep_manager_lock_sleep(sleep_handler);/*lock sleep mode*/
            log_hal_info("[I2S] lock sleep successfully");
        }
    }
#endif

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t  hal_i2s_disable_tx(void)
{
    //wait until data counts in tx vfifo is empty
    i2s_query_tx_vfifo_empty();

    i2s_internal_cfg.i2s_state &=  ~(1 << I2S_STATE_TX_RUNNING);

    I2sDlEn(KAL_FALSE);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    if ((I2S_CHECK_BIT(i2s_internal_cfg.i2s_state, I2S_STATE_TX_RUNNING) == 0) && (I2S_CHECK_BIT(i2s_internal_cfg.i2s_state, I2S_STATE_RX_RUNNING) == 0)) {
        /*unlock sleep mode*/
        hal_sleep_manager_unlock_sleep(sleep_handler);
        /*release sleep hander*/
        hal_sleep_manager_release_sleep_handle(sleep_handler);
        sleep_handler = 0;
        log_hal_info("[I2S] unlock sleep successfully");
    }
#endif

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_disable_rx(void)
{

    i2s_internal_cfg.i2s_state &= ~(1 << I2S_STATE_RX_RUNNING);

    I2sUlEn(KAL_FALSE);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    if ((I2S_CHECK_BIT(i2s_internal_cfg.i2s_state, I2S_STATE_TX_RUNNING) == 0) && (I2S_CHECK_BIT(i2s_internal_cfg.i2s_state, I2S_STATE_RX_RUNNING) == 0)) {
        /*unlock sleep mode*/
        hal_sleep_manager_unlock_sleep(sleep_handler);
        /*release sleep hander*/
        hal_sleep_manager_release_sleep_handle(sleep_handler);
        sleep_handler = 0;
        log_hal_info("[I2S] unlock sleep successfully");
    }
#endif

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t hal_i2s_tx_write(uint32_t data)
{

    DRV_Reg32(I2S_DMA_TX_FIFO) = data;

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_rx_read(uint32_t *data)
{

    *data = DRV_Reg32(I2S_DMA_RX_FIFO);

    return HAL_I2S_STATUS_OK;

}


static void i2s_dma_callback(bool is_rx)
{
    if (is_rx) {
        //----notify user to get data from RX VFIFO----
        i2s_internal_cfg.user_rx_callback_func(HAL_I2S_EVENT_DATA_NOTIFICATION,
                                               i2s_internal_cfg.user_rx_data);
    } else {
        //notify user to refill data to TX VFIFO
        i2s_internal_cfg.user_tx_callback_func(HAL_I2S_EVENT_DATA_REQUEST,
                                               i2s_internal_cfg.user_tx_data);
    }
}


static void i2x_tx_handler(void)
{
    i2s_dma_callback(0);
}


static void i2x_rx_handler(void)
{
    i2s_dma_callback(1);
}


static hal_i2s_status_t i2s_is_sample_rate(hal_i2s_sample_rate_t sample_rate, bool IsRx)
{
    switch (sample_rate) {
        case HAL_I2S_SAMPLE_RATE_8K:
            if (IsRx) {
                i2s_internal_cfg.I2SULCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_8K;
            } else {
                i2s_internal_cfg.I2SDLCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_8K;
            }
            break;
        case HAL_I2S_SAMPLE_RATE_12K:
            if (IsRx) {
                i2s_internal_cfg.I2SULCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_12K;
            } else {
                i2s_internal_cfg.I2SDLCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_12K;
            }
            break;
        case HAL_I2S_SAMPLE_RATE_16K:
            if (IsRx) {
                i2s_internal_cfg.I2SULCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_16K;
            } else {
                i2s_internal_cfg.I2SDLCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_16K;
            }
            break;
        case HAL_I2S_SAMPLE_RATE_24K:
            if (IsRx) {
                i2s_internal_cfg.I2SULCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_24K;
            } else {
                i2s_internal_cfg.I2SDLCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_24K;
            }
            break;
        case HAL_I2S_SAMPLE_RATE_32K:
            if (IsRx) {
                i2s_internal_cfg.I2SULCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_32K;
            } else {
                i2s_internal_cfg.I2SDLCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_32K;
            }
            break;
        case HAL_I2S_SAMPLE_RATE_48K:
            if (IsRx) {
                i2s_internal_cfg.I2SULCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_48K;
            } else {
                i2s_internal_cfg.I2SDLCfg.u4I2SSampleRate = I2S_SAMPLE_RATE_48K;
            }
            break;
        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    return HAL_I2S_STATUS_OK;
}


#ifdef HAL_SLEEP_MANAGER_ENABLED
void i2s_enter_suspend(void)
{
    //we do not need to anything(backup setting, disable i2s and clock) now,
    //because i2s can not allow to enter sleep when i2s tx or rx is enabled
    //and all settings are already stored by static global structure
    return;

}

void i2s_enter_resume(void)
{
    if ((i2s_internal_cfg.i2s_state == I2S_STATE_INIT) && (i2s_internal_cfg.i2s_configured)) {
        /*reinit and reconfig when i2s resume back*/
        hal_i2s_set_config(&i2s_internal_cfg.i2s_user_config);

        /*reconfig vfifo*/
        if (i2s_internal_cfg.i2s_vfifo.tx_dma_configured) {
            hal_i2s_setup_tx_vfifo(
                i2s_internal_cfg.i2s_vfifo.tx_vfifo_base,
                i2s_internal_cfg.i2s_vfifo.tx_vfifo_threshold,
                i2s_internal_cfg.i2s_vfifo.tx_vfifo_length);
        }

        if (i2s_internal_cfg.i2s_vfifo.rx_dma_configured) {
            hal_i2s_setup_rx_vfifo(
                i2s_internal_cfg.i2s_vfifo.rx_vfifo_base,
                i2s_internal_cfg.i2s_vfifo.rx_vfifo_threshold,
                i2s_internal_cfg.i2s_vfifo.rx_vfifo_length);
        }

    }
}
#endif

#endif//#ifdef HAL_I2S_MODULE_ENABLED
