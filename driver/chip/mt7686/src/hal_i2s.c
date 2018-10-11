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

#include "hal_log.h"
#include "hal_clock.h"
#include "hal_gpio.h"
#include "hal_pdma_internal.h"
#include "hal_i2s_internal.h"

#ifdef __I2S1_MCLK_PIN__
extern const char HAL_I2S1_MCLK_PIN;
#endif
#include "memory_attribute.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif

static hal_clock_cg_id i2s_clock_cg[HAL_I2S_MAX] = {HAL_CLOCK_CG_SW_AUDIO, HAL_CLOCK_CG_SW_ASYS};
static vdma_channel_t i2s_vdma_tx_ch[HAL_I2S_MAX] = {VDMA_AUDTX, VDMA_ASYSTX};
static vdma_channel_t i2s_vdma_rx_ch[HAL_I2S_MAX] = {VDMA_AUDRX, VDMA_ASYSRX};
static i2s_internal_t i2s_internal_cfg[HAL_I2S_MAX];

#ifdef HAL_SLEEP_MANAGER_ENABLED
static bool is_register_sleep_callback[HAL_I2S_MAX] = {false};
static bool is_lock_sleep[HAL_I2S_MAX] = {false};
static uint32_t i2s_global_cfg[HAL_I2S_MAX] = {0};
static uint32_t i2s_dl_cfg[HAL_I2S_MAX] = {0};
static uint32_t i2s_ul_cfg[HAL_I2S_MAX] = {0};
static uint8_t  i2s_ul_fs_cfg[HAL_I2S_MAX] = {0};
static uint8_t  i2s_dl_fs_cfg[HAL_I2S_MAX] = {0};

static void i2s0_enter_suspend(void);
static void i2s0_enter_resume(void);
static void i2s1_enter_suspend(void);
static void i2s1_enter_resume(void);
#endif

static void i2s_rx_dma_callback_handler(vdma_event_t event, void  *user_data);
static void i2s_tx_dma_callback_handler(vdma_event_t event, void  *user_data);

void i2s_stop_rx_vfifo(hal_i2s_port_t i2s_port, bool disable_flag)
{
    if (i2s_internal_cfg[i2s_port].i2s_vfifo.rx_dma_configured != true) {
        return ;
    }

    vdma_status_t status;
    status = vdma_stop(i2s_vdma_rx_ch[i2s_port]);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] STOP RX VFIFO ERROR, vdma_stop failed\r\n", i2s_port);
        return ;
    }
    status = vdma_deinit(i2s_vdma_rx_ch[i2s_port]);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] STOP RX VFIFO ERROR, deinit vdma rx channel failed\r\n", i2s_port);
        return ;
    }

    if (disable_flag) {
        i2s_internal_cfg[i2s_port].i2s_vfifo.rx_dma_configured = false;
    }
}

void i2s_stop_tx_vfifo(hal_i2s_port_t i2s_port, bool disable_flag)
{
    if (i2s_internal_cfg[i2s_port].i2s_vfifo.tx_dma_configured != true) {
        return ;
    }
    vdma_status_t status;
    status = vdma_stop(i2s_vdma_tx_ch[i2s_port]);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] STOP TX VFIFO ERROR, vdma_stop fail\r\n", i2s_port);
        return ;
    }

    status = vdma_deinit(i2s_vdma_tx_ch[i2s_port]);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] STOP TX VFIFO ERROR, deinit vdma tx channel failed\r\n", i2s_port);
        return ;
    }

    if (disable_flag) {
        i2s_internal_cfg[i2s_port].i2s_vfifo.tx_dma_configured = false;
    }
}

void i2s_query_tx_vfifo_empty(hal_i2s_port_t i2s_port)
{
    while ((I2S_CHECK_BIT(i2s_internal_cfg[i2s_port].i2s_state, I2S_STATE_TX_RUNNING) != 0) &&
            (i2s_internal_cfg[i2s_port].i2s_vfifo.tx_dma_configured == true) &&
            (i2s_internal_cfg[i2s_port].i2s_audiotop_enabled == true)) {

        if (i2s_port == HAL_I2S_0) {
            if (DRV_Reg32(VDMA7_FFCNT) == 0) {
                break;
            }
        } else {
            if (DRV_Reg32(VDMA9_FFCNT) == 0) {
                break;
            }
        }
    }
}


hal_i2s_status_t hal_i2s_init(hal_i2s_initial_type_t i2s_initial_type)
{
    return hal_i2s_init_ex(HAL_I2S_0, i2s_initial_type);
}


hal_i2s_status_t hal_i2s_init_ex(hal_i2s_port_t i2s_port, hal_i2s_initial_type_t i2s_initial_type)
{
    log_hal_info("[I2S%d] INIT\r\n", i2s_port);

    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] INIT ERROR, invalid i2s port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].i2s_state != I2S_STATE_IDLE) {
        log_hal_error("[I2S%d] INIT ERROR, i2s_state=%d\r\n", i2s_port, i2s_internal_cfg[i2s_port].i2s_state);
        return HAL_I2S_STATUS_ERROR;
    }

    i2s_set_initial_parameter(&i2s_internal_cfg[i2s_port]);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    if (is_register_sleep_callback[i2s_port] == false) {
        if (i2s_port == HAL_I2S_0) {
            sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_AUDIO_TOP, (sleep_management_suspend_callback_t)i2s0_enter_suspend, NULL);
            sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_AUDIO_TOP, (sleep_management_resume_callback_t)i2s0_enter_resume, NULL);
        }
        if (i2s_port == HAL_I2S_1) {
            sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_ASYS_TOP, (sleep_management_suspend_callback_t)i2s1_enter_suspend, NULL);
            sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_ASYS_TOP, (sleep_management_resume_callback_t)i2s1_enter_resume, NULL);
        }
        is_register_sleep_callback[i2s_port] = true;
    }
#endif

    switch (i2s_initial_type) {

        case HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE:
            log_hal_info("[I2S%d] HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE\r\n", i2s_port);
            i2s_internal_cfg[i2s_port].i2s_initial_type = HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE;
            break;

        case HAL_I2S_TYPE_EXTERNAL_MODE:
            log_hal_info("[I2S%d] HAL_I2S_TYPE_EXTERNAL_MODE\r\n", i2s_port);
            i2s_internal_cfg[i2s_port].i2s_initial_type = HAL_I2S_TYPE_EXTERNAL_MODE;
            break;

        case HAL_I2S_TYPE_INTERNAL_MODE:
            log_hal_error("[I2S%d] HAL_I2S_TYPE_INTERNAL_MODE is not supported\r\n", i2s_port);
            return HAL_I2S_STATUS_INVALID_PARAMETER;

        case HAL_I2S_TYPE_INTERNAL_TDM_LOOPBACK_MODE:
            if (i2s_port == HAL_I2S_0) {
                log_hal_info("[I2S%d] HAL_I2S_TYPE_INTERNAL_TDM_LOOPBACK_MODE\r\n", i2s_port);
                i2s_internal_cfg[i2s_port].i2s_initial_type = HAL_I2S_TYPE_INTERNAL_TDM_LOOPBACK_MODE;
                break;
            } else {
                log_hal_error("[I2S%d] HAL_I2S_TYPE_INTERNAL_TDM_LOOPBACK_MODE is not supported\n\r", i2s_port);
                return HAL_I2S_STATUS_INVALID_PARAMETER;
            }

        case HAL_I2S_TYPE_EXTERNAL_TDM_MODE:
            if (i2s_port == HAL_I2S_0) {
                log_hal_info("[I2S%d] HAL_I2S_TYPE_EXTERNAL_TDM_MODE\r\n", i2s_port);
                i2s_internal_cfg[i2s_port].i2s_initial_type = HAL_I2S_TYPE_EXTERNAL_TDM_MODE;
                break;
            } else {
                log_hal_error("[I2S%d] HAL_I2S_TYPE_EXTERNAL_TDM_MODE is not supported\n\r", i2s_port);
                return HAL_I2S_STATUS_INVALID_PARAMETER;
            }

        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    i2s_internal_cfg[i2s_port].i2s_state = I2S_STATE_INIT;
    i2s_internal_cfg[i2s_port].i2s_port = i2s_port;

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t hal_i2s_deinit(void)
{
    return hal_i2s_deinit_ex(HAL_I2S_0);
}


hal_i2s_status_t hal_i2s_deinit_ex(hal_i2s_port_t i2s_port)
{
    log_hal_info("[I2S%d] DEINIT\r\n", i2s_port);

    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] I2S DEINIT ERROR, invalid i2s port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].i2s_state != I2S_STATE_INIT) {
        log_hal_error("[I2S%d] DEINIT ERROR, i2s_state=%d\r\n", i2s_port, i2s_internal_cfg[i2s_port].i2s_state);
        return HAL_I2S_STATUS_ERROR;
    }
    i2s_internal_cfg[i2s_port].i2s_state = I2S_STATE_IDLE;
    i2s_internal_cfg[i2s_port].i2s_configured = false;
    //Stop DMA
    i2s_stop_rx_vfifo(i2s_port, true);
    i2s_stop_tx_vfifo(i2s_port, true);
    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t hal_i2s_set_config(const hal_i2s_config_t *config)
{
    return hal_i2s_set_config_ex(HAL_I2S_0, config);
}


hal_i2s_status_t hal_i2s_set_config_ex(hal_i2s_port_t i2s_port, const hal_i2s_config_t *config)
{
    log_hal_info("[I2S%d] CONFIG\r\n", i2s_port);

    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] CONFIG ERROR, invalid i2s port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (NULL == config) {
        log_hal_error("[I2S%d] CONFIG ERROR, config is null\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].i2s_state != I2S_STATE_INIT) {
        log_hal_error("[I2S%d] CONFIG ERROR, i2s_state=%d\r\n", i2s_port, i2s_internal_cfg[i2s_port].i2s_state);
        return HAL_I2S_STATUS_ERROR;
    }

    /*Check both sample rates are same value */
    if ((config->i2s_out.sample_rate) != (config->i2s_in.sample_rate)) {
        log_hal_error("[I2S%d] CONFIG ERROR, tx sampling rate != rx sampling rate\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    //---Check whether both tx channerl number and tx mode are set to enable at same time
    if ((config->i2s_out.channel_number == HAL_I2S_STEREO) && (config->tx_mode == HAL_I2S_TX_MONO_DUPLICATE_ENABLE)) {
        log_hal_error("[I2S%d] CONFIG ERROR, tx is stereo and tx mode is duplicate\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    /*Check msb_offset are same value */
    if ((config->i2s_out.msb_offset) != (config->i2s_in.msb_offset)) {
        log_hal_error("[I2S%d] CONFIG ERROR, tx msb_offset != rx msb_offset\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    /*Set TX channel number*/
    switch (config->i2s_out.channel_number) {
        case HAL_I2S_MONO:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_out.channel_number = HAL_I2S_MONO;
            break;
        case HAL_I2S_STEREO:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_out.channel_number = HAL_I2S_STEREO;
            break;
        default:
            log_hal_error("[I2S%d] CONFIG ERROR, invalid i2s_out.channel_number\r\n", i2s_port);
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    /*Set RX channel number*/
    switch (config->i2s_in.channel_number) {
        case HAL_I2S_MONO:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_in.channel_number = HAL_I2S_MONO;
            break;
        case HAL_I2S_STEREO:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_in.channel_number = HAL_I2S_STEREO;
            break;
        default:
            log_hal_error("[I2S%d] CONFIG ERROR, invalid i2s_in.channel_number\r\n", i2s_port);
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    /*Set clock mode*/
    switch (config->clock_mode) {
        case HAL_I2S_MASTER:
            i2s_internal_cfg[i2s_port].i2s_user_config.clock_mode = HAL_I2S_MASTER;
            break;
        case HAL_I2S_SLAVE:
            i2s_internal_cfg[i2s_port].i2s_user_config.clock_mode = HAL_I2S_SLAVE;
            break;
        default:
            log_hal_error("[I2S%d] CONFIG ERROR, invalid clock_mode\r\n", i2s_port);
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    /*Set tx mode*/
    switch (config->tx_mode) {
        case HAL_I2S_TX_MONO_DUPLICATE_ENABLE:
            i2s_internal_cfg[i2s_port].i2s_user_config.tx_mode = HAL_I2S_TX_MONO_DUPLICATE_ENABLE;
            break;
        default:
            i2s_internal_cfg[i2s_port].i2s_user_config.tx_mode = HAL_I2S_TX_MONO_DUPLICATE_DISABLE;
            break;
    }


    /*Set TX lr swap*/
    switch (config->i2s_out.lr_swap) {
        case HAL_I2S_LR_SWAP_ENABLE:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_out.lr_swap = HAL_I2S_LR_SWAP_ENABLE;
            break;
        default:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_out.lr_swap = HAL_I2S_LR_SWAP_DISABLE;
            break;
    }

    /*Set RX lr swap*/
    switch (config->i2s_in.lr_swap) {
        case HAL_I2S_LR_SWAP_ENABLE:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_in.lr_swap = HAL_I2S_LR_SWAP_ENABLE;
            break;
        default:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_in.lr_swap = HAL_I2S_LR_SWAP_DISABLE;
            break;
    }

    /*Sample width*/
    switch (config->sample_width) {
        case HAL_I2S_SAMPLE_WIDTH_8BIT:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
        case HAL_I2S_SAMPLE_WIDTH_16BIT:
            i2s_internal_cfg[i2s_port].i2s_user_config.sample_width = HAL_I2S_SAMPLE_WIDTH_16BIT;
            break;
        case HAL_I2S_SAMPLE_WIDTH_24BIT:
            i2s_internal_cfg[i2s_port].i2s_user_config.sample_width = HAL_I2S_SAMPLE_WIDTH_24BIT;
            break;
        default:
            i2s_internal_cfg[i2s_port].i2s_user_config.sample_width = HAL_I2S_SAMPLE_WIDTH_16BIT;
            break;
    }

    /*Polarity of BCLK*/
    //todo

    /*Set msb_offset*/
    i2s_internal_cfg[i2s_port].i2s_user_config.i2s_in.msb_offset = config->i2s_in.msb_offset;
    i2s_internal_cfg[i2s_port].i2s_user_config.i2s_out.msb_offset = config->i2s_out.msb_offset;

    //Set TX word_select_inverse----
    switch (config->i2s_out.word_select_inverse) {
        case HAL_I2S_WORD_SELECT_INVERSE_ENABLE:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_out.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_ENABLE;
            break;
        default:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_out.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
            break;
    }

    //Set RX word_select_inverse----
    switch (config->i2s_in.word_select_inverse) {
        case HAL_I2S_WORD_SELECT_INVERSE_ENABLE:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_in.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_ENABLE;
            break;
        default:
            i2s_internal_cfg[i2s_port].i2s_user_config.i2s_in.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
            break;
    }


    /*Data size per frame, channel per frame*/
    if (i2s_port == HAL_I2S_0) {
        if ((i2s_internal_cfg[i2s_port].i2s_initial_type == HAL_I2S_TYPE_EXTERNAL_TDM_MODE) || (i2s_internal_cfg[i2s_port].i2s_initial_type == HAL_I2S_TYPE_INTERNAL_TDM_LOOPBACK_MODE)) {

            /*TDM CH*/
            if ((config->tdm_channel != HAL_I2S_TDM_2_CHANNEL) && (config->tdm_channel != HAL_I2S_TDM_4_CHANNEL)) {
                log_hal_error("[I2S%d] CONFIG ERROR, invalid tdm_channel\r\n", i2s_port);
                return HAL_I2S_STATUS_INVALID_PARAMETER;
            }
            i2s_internal_cfg[i2s_port].i2s_user_config.tdm_channel = config->tdm_channel;

            /*frame sync size*/
            if ((config->frame_sync_width != HAL_I2S_FRAME_SYNC_WIDTH_32) && (config->frame_sync_width != HAL_I2S_FRAME_SYNC_WIDTH_64) && (config->frame_sync_width != HAL_I2S_FRAME_SYNC_WIDTH_128)) {
                log_hal_error("[I2S%d] CONFIG ERROR, invalid frame_sync_width\r\n", i2s_port);
                return HAL_I2S_STATUS_INVALID_PARAMETER;
            }
            if ((config->frame_sync_width == HAL_I2S_FRAME_SYNC_WIDTH_128) && (config->clock_mode == HAL_I2S_MASTER)) {
                log_hal_error("I2S%d] CONFIG ERROR, HAL_I2S_FRAME_SYNC_WIDTH_128 is not supported on TDM master mode\r\n");
                return HAL_I2S_STATUS_INVALID_PARAMETER;

            }
            i2s_internal_cfg[i2s_port].i2s_user_config.frame_sync_width = config->frame_sync_width;
        }
    } else {
        /*frame sync size*/
        switch (config->frame_sync_width) {
            case HAL_I2S_FRAME_SYNC_WIDTH_64:
                i2s_internal_cfg[i2s_port].i2s_user_config.frame_sync_width = HAL_I2S_FRAME_SYNC_WIDTH_64;
                break;
            default:
                i2s_internal_cfg[i2s_port].i2s_user_config.frame_sync_width = HAL_I2S_FRAME_SYNC_WIDTH_32;
                break;
        }
    }

    /*Set sample rate, must put this function after "i2s_set_parameter" to prevent overwrite FS setting*/
    if (i2s_set_clock(&i2s_internal_cfg[i2s_port], config->i2s_out.sample_rate) != HAL_I2S_STATUS_OK) {
        log_hal_error("[I2S%d] CONFIG ERROR, invalid sample_rate\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    i2s_internal_cfg[i2s_port].i2s_configured = true;

    if ((i2s_internal_cfg[HAL_I2S_0].i2s_configured == true) && (i2s_internal_cfg[HAL_I2S_1].i2s_configured == true)) {
        if (i2s_internal_cfg[HAL_I2S_0].i2s_clock_source != i2s_internal_cfg[HAL_I2S_1].i2s_clock_source) {
            log_hal_error("[I2S%d] CONFIG ERROR,  clock source is different between i2s0 and i2s1\r\n", i2s_port);
            return HAL_I2S_STATUS_ERROR;
        }
    }

    /*Configure I2S according to user`s settings*/
    i2s_set_parameter(&i2s_internal_cfg[i2s_port]);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    //Backup SR reg
    if (i2s_port == HAL_I2S_0) {
        //I2S
        i2s_global_cfg[HAL_I2S_0] = (uint32_t)DRV_Reg32(I2S_GLOBAL_CONTROL);
        i2s_dl_cfg[HAL_I2S_0] = (uint32_t)DRV_Reg32(I2S_DL_CONTROL);
        i2s_ul_cfg[HAL_I2S_0] = (uint32_t)DRV_Reg32(I2S_UL_CONTROL);
        i2s_dl_fs_cfg[HAL_I2S_0] = (uint8_t)DRV_Reg8(I2S_DL_SR_EN_CONTROL__F_CR_I2S_OUT_SR);
        i2s_ul_fs_cfg[HAL_I2S_0] = (uint8_t)DRV_Reg8(I2S_UL_SR_EN_CONTROL__F_CR_I2S_IN_SR);
    } else {
        //I2S
        i2s_global_cfg[HAL_I2S_1] = (uint32_t)DRV_Reg32(I2S1_GLOBAL_CONTROL);
        i2s_dl_cfg[HAL_I2S_1] = (uint32_t)DRV_Reg32(I2S1_DL_CONTROL);
        i2s_ul_cfg[HAL_I2S_1] = (uint32_t)DRV_Reg32(I2S1_UL_CONTROL);
        i2s_dl_fs_cfg[HAL_I2S_1] = (uint8_t)DRV_Reg8(I2S1_DL_SR_EN_CONTROL__F_CR_I2S1_OUT_SR);
        i2s_ul_fs_cfg[HAL_I2S_1] = (uint8_t)DRV_Reg8(I2S1_UL_SR_EN_CONTROL__F_CR_I2S1_IN_SR);
    }
#endif
    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t hal_i2s_setup_tx_vfifo(uint32_t *buffer, uint32_t threshold, uint32_t buffer_length)
{
    return hal_i2s_setup_tx_vfifo_ex(HAL_I2S_0, buffer, threshold, buffer_length);
}


hal_i2s_status_t hal_i2s_setup_tx_vfifo_ex(hal_i2s_port_t i2s_port, uint32_t *buffer, uint32_t threshold, uint32_t buffer_length)
{

    if (NULL == buffer) {
        log_hal_error("[I2S%d] SET TX VFIFO ERROR, buffer is NULL\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    /*address should be 4 bytes aligned*/
    if ((((uint32_t)buffer) & 0x3) != 0) {
        log_hal_error("[I2S%d] SET TX VFIFO ERROR, buffer should be 4 bytes aligned\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (buffer_length == 0 || (buffer_length & 0x1) != 0) {
        log_hal_error("[I2S%d] SET TX VFIFO ERROR, buffer_length == 0 || buffer_length is odd\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] SET TX VFIFO ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].i2s_state != I2S_STATE_INIT) {
        log_hal_error("[I2S%d] SET TX VFIFO ERROR ERROR, i2s_state=%d\r\n", i2s_port, i2s_internal_cfg[i2s_port].i2s_state);
        return HAL_I2S_STATUS_ERROR;
    }

    i2s_stop_tx_vfifo(i2s_port, true);

    vdma_status_t i2s_vdma_status;
    vdma_config_t i2s_vdma_config;
    uint32_t i2s_threshold;

    i2s_vdma_config.base_address = (uint32_t)buffer;
    i2s_vdma_config.size = buffer_length;
    i2s_threshold = threshold;

    i2s_vdma_status = vdma_init(i2s_vdma_tx_ch[i2s_port]);
    if (i2s_vdma_status != VDMA_OK) {
        log_hal_error("[I2S%d] SET TX VFIFO ERROR, vdma_init fail\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    i2s_vdma_status = vdma_configure(i2s_vdma_tx_ch[i2s_port], &i2s_vdma_config);
    if (i2s_vdma_status != VDMA_OK) {
        log_hal_error("[I2S%d] SET TX VFIFO ERROR, vdma_configure fail\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    i2s_vdma_status = vdma_set_threshold(i2s_vdma_tx_ch[i2s_port], i2s_threshold);
    if (i2s_vdma_status != VDMA_OK) {
        log_hal_error("[I2S%d] SET TX VFIFO ERROR, vdma_set_threshold fail\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    i2s_vdma_status = vdma_start(i2s_vdma_tx_ch[i2s_port]);
    if (i2s_vdma_status != VDMA_OK) {
        log_hal_error("[I2S%d] SET TX VFIFO ERROR, vdma_start fail\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }
    i2s_internal_cfg[i2s_port].i2s_vfifo.tx_vfifo_base = (uint32_t *)i2s_vdma_config.base_address;
    i2s_internal_cfg[i2s_port].i2s_vfifo.tx_vfifo_length = i2s_vdma_config.size;
    i2s_internal_cfg[i2s_port].i2s_vfifo.tx_vfifo_threshold = i2s_threshold;
    i2s_internal_cfg[i2s_port].i2s_vfifo.tx_dma_configured = true;

    return HAL_I2S_STATUS_OK;

}

hal_i2s_status_t hal_i2s_setup_rx_vfifo(uint32_t *buffer, uint32_t threshold, uint32_t buffer_length)
{
    return hal_i2s_setup_rx_vfifo_ex(HAL_I2S_0, buffer, threshold, buffer_length);
}


hal_i2s_status_t hal_i2s_setup_rx_vfifo_ex(hal_i2s_port_t i2s_port, uint32_t *buffer, uint32_t threshold, uint32_t buffer_length)
{
    if (NULL == buffer) {
        log_hal_error("[I2S%d] SET RX VFIFO ERROR, buffer is NULL\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    /*address should be 4 bytes aligned*/
    if ((((uint32_t)buffer) & 0x3) != 0) {
        log_hal_error("[I2S%d] SET RX VFIFO ERROR, buffer should be 4 bytes aligned\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (buffer_length == 0 || (buffer_length & 0x1) != 0) {
        log_hal_error("[I2S%d] SET RX VFIFO ERROR, buffer_length == 0 || buffer_length is odd\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] SET RX VFIFO ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].i2s_state != I2S_STATE_INIT) {
        log_hal_error("[I2S%d] SET RX VFIFO ERROR ERROR, i2s_state=%d\r\n", i2s_port, i2s_internal_cfg[i2s_port].i2s_state);
        return HAL_I2S_STATUS_ERROR;
    }

    i2s_stop_rx_vfifo(i2s_port, true);

    vdma_status_t i2s_vdma_status;
    vdma_config_t i2s_vdma_config;
    uint32_t i2s_threshold;

    i2s_vdma_config.base_address = (uint32_t)buffer;
    i2s_vdma_config.size = buffer_length;
    i2s_threshold = threshold;

    i2s_vdma_status = vdma_init(i2s_vdma_rx_ch[i2s_port]);
    if (i2s_vdma_status != VDMA_OK) {
        log_hal_error("[I2S%d] SET RX VFIFO ERROR, vdma_init fail\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    i2s_vdma_status = vdma_configure(i2s_vdma_rx_ch[i2s_port], &i2s_vdma_config);
    if (i2s_vdma_status != VDMA_OK) {
        log_hal_error("[I2S%d] SET RX VFIFO ERROR, vdma_configure fail\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    i2s_vdma_status = vdma_set_threshold(i2s_vdma_rx_ch[i2s_port], i2s_threshold);
    if (i2s_vdma_status != VDMA_OK) {
        log_hal_error("[I2S%d] SET RX VFIFO ERROR, vdma_set_threshold fail\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    i2s_vdma_status = vdma_start(i2s_vdma_rx_ch[i2s_port]);
    if (i2s_vdma_status != VDMA_OK) {
        log_hal_error("[I2S%d] SET RX VFIFO ERROR, vdma_start fail\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }
    i2s_internal_cfg[i2s_port].i2s_vfifo.rx_vfifo_base = (uint32_t *)i2s_vdma_config.base_address;
    i2s_internal_cfg[i2s_port].i2s_vfifo.rx_vfifo_length = i2s_vdma_config.size;
    i2s_internal_cfg[i2s_port].i2s_vfifo.rx_vfifo_threshold = i2s_threshold;
    i2s_internal_cfg[i2s_port].i2s_vfifo.rx_dma_configured = true;

    return HAL_I2S_STATUS_OK;


}

//Deprecate SDK API
hal_i2s_status_t hal_i2s_stop_tx_vfifo(void)
{
    return HAL_I2S_STATUS_OK;
}

//Deprecate SDK API
hal_i2s_status_t hal_i2s_stop_tx_vfifo_ex(hal_i2s_port_t i2s_port)
{
    return HAL_I2S_STATUS_OK;
}

//Deprecate SDK API
hal_i2s_status_t hal_i2s_stop_rx_vfifo(void)
{
    return HAL_I2S_STATUS_OK;
}

//Deprecate SDK API
hal_i2s_status_t hal_i2s_stop_rx_vfifo_ex(hal_i2s_port_t i2s_port)
{
    return HAL_I2S_STATUS_OK;
}


ATTR_TEXT_IN_TCM hal_i2s_status_t hal_i2s_tx_write(uint32_t data)
{
    DRV_Reg32(AUDIO_TOP_TX_DMA_FIFO) = data;

    return HAL_I2S_STATUS_OK;

}


ATTR_TEXT_IN_TCM hal_i2s_status_t hal_i2s_tx_write_ex(hal_i2s_port_t i2s_port, uint32_t data)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] WRITE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_port == HAL_I2S_0) {
        DRV_Reg32(AUDIO_TOP_TX_DMA_FIFO) = data;
    } else {
        DRV_Reg32(ASYS_TOP_TX_DMA_FIFO) = data;
    }
    return HAL_I2S_STATUS_OK;
}


ATTR_TEXT_IN_TCM hal_i2s_status_t hal_i2s_rx_read(uint32_t *data)
{
    *data = DRV_Reg32(AUDIO_TOP_RX_DMA_FIFO);
    return HAL_I2S_STATUS_OK;
}


ATTR_TEXT_IN_TCM hal_i2s_status_t hal_i2s_rx_read_ex(hal_i2s_port_t i2s_port, uint32_t *data)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] READ ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_port == HAL_I2S_0) {
        *data = DRV_Reg32(AUDIO_TOP_RX_DMA_FIFO);
    } else {
        *data = DRV_Reg32(ASYS_TOP_RX_DMA_FIFO);
        //Use software LR SWAP, because LR SWAP doesn`t work on i2s1 hw
        if ((i2s_internal_cfg[HAL_I2S_1].i2s_user_config.i2s_in.lr_swap == HAL_I2S_LR_SWAP_ENABLE)
                && (i2s_internal_cfg[HAL_I2S_1].i2s_user_config.sample_width == HAL_I2S_SAMPLE_WIDTH_16BIT)) {
            *data = ((*data) << 16) | ((*data) >> 16);
        }
    }
    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t hal_i2s_get_config(hal_i2s_config_t *config)
{
    *config = i2s_internal_cfg[HAL_I2S_0].i2s_user_config;

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_get_config_ex(hal_i2s_port_t i2s_port, hal_i2s_config_t *config)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] GET CONFIG ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    *config = i2s_internal_cfg[i2s_port].i2s_user_config;
    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_get_tx_sample_count(uint32_t *sample_count)
{

    return hal_i2s_get_tx_sample_count_ex(HAL_I2S_0, sample_count);
}


hal_i2s_status_t hal_i2s_get_tx_sample_count_ex(hal_i2s_port_t i2s_port, uint32_t *sample_count)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] GET TX SAMPLE COUNT ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].i2s_vfifo.tx_vfifo_length == 0) {
        log_hal_error("[I2S%d] GET TX SAMPLE COUNT ERROR, tx vfifo length == 0\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    vdma_status_t status;

    status = vdma_get_available_send_space(i2s_vdma_tx_ch[i2s_port], sample_count);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] GET TX SAMPLE COUNT ERROR,  vdma_get_available_send_space fail\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    return HAL_I2S_STATUS_OK;

}

hal_i2s_status_t hal_i2s_get_rx_sample_count(uint32_t *sample_count)
{

    return hal_i2s_get_rx_sample_count_ex(HAL_I2S_0, sample_count);
}


hal_i2s_status_t hal_i2s_get_rx_sample_count_ex(hal_i2s_port_t i2s_port, uint32_t *sample_count)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] GET RX SAMPLE COUNT ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].i2s_vfifo.rx_vfifo_length == 0) {
        log_hal_error("[I2S%d] GET RX SAMPLE COUNT ERROR, rx vfifo length == 0\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    vdma_status_t status;

    status = vdma_get_available_receive_bytes(i2s_vdma_rx_ch[i2s_port], sample_count);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] GET RX SAMPLE COUNT ERROR, vdma_get_available_receive_bytes failed\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t  hal_i2s_enable_tx(void)
{
    return hal_i2s_enable_tx_ex(HAL_I2S_0);
}


hal_i2s_status_t  hal_i2s_enable_tx_ex(hal_i2s_port_t i2s_port)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] TX ENABLE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_port == HAL_I2S_0) {
        DRV_Reg8(I2S_DL_SR_EN_CONTROL__F_CR_I2S_OUT_EN) = (uint8_t)0x1;
    } else {
        DRV_Reg8(I2S1_DL_SR_EN_CONTROL__F_CR_I2S1_OUT_EN) = (uint8_t)0x1;
    }

    i2s_internal_cfg[i2s_port].i2s_state |= (uint8_t)(1 << I2S_STATE_TX_RUNNING);

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t  hal_i2s_enable_rx(void)
{
    return hal_i2s_enable_rx_ex(HAL_I2S_0);
}


hal_i2s_status_t  hal_i2s_enable_rx_ex(hal_i2s_port_t i2s_port)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] RX ENABLE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_port == HAL_I2S_0) {
        DRV_Reg8(I2S_UL_SR_EN_CONTROL__F_CR_I2S_IN_EN) = (uint8_t)0x1;
    } else {
        DRV_Reg8(I2S1_UL_SR_EN_CONTROL__F_CR_I2S1_IN_EN) = (uint8_t)0x1;
    }

    i2s_internal_cfg[i2s_port].i2s_state |= (uint8_t)(1 << I2S_STATE_RX_RUNNING);

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t  hal_i2s_disable_tx(void)
{
    return hal_i2s_disable_tx_ex(HAL_I2S_0);
}


hal_i2s_status_t  hal_i2s_disable_tx_ex(hal_i2s_port_t i2s_port)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] TX DISABLE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_port == HAL_I2S_0) {
        //flush tx vfifo
        i2s_query_tx_vfifo_empty(i2s_port);
        DRV_Reg8(I2S_DL_SR_EN_CONTROL__F_CR_I2S_OUT_EN) = (uint8_t)0x0;

    } else {//HAL_I2S_1
        //flush tx vfifo
        i2s_query_tx_vfifo_empty(i2s_port);
        DRV_Reg8(I2S1_DL_SR_EN_CONTROL__F_CR_I2S1_OUT_EN) = (uint8_t)0x0;
    }

    i2s_internal_cfg[i2s_port].i2s_state &= (uint8_t)(~(1 << I2S_STATE_TX_RUNNING));

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t hal_i2s_disable_rx(void)
{
    return hal_i2s_disable_rx_ex(HAL_I2S_0);
}


hal_i2s_status_t hal_i2s_disable_rx_ex(hal_i2s_port_t i2s_port)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] RX DISABLE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_port == HAL_I2S_0) {
        DRV_Reg8(I2S_UL_SR_EN_CONTROL__F_CR_I2S_IN_EN) = (uint8_t)0x0;
    } else {//HAL_I2S_1
        DRV_Reg8(I2S1_UL_SR_EN_CONTROL__F_CR_I2S1_IN_EN) = (uint8_t)0x0;
    }

    i2s_internal_cfg[i2s_port].i2s_state &= (uint8_t)(~(1 << I2S_STATE_RX_RUNNING));

    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t  hal_i2s_enable_audio_top(void)
{
    return hal_i2s_enable_audio_top_ex(HAL_I2S_0);
}

hal_i2s_status_t  hal_i2s_enable_audio_top_ex(hal_i2s_port_t i2s_port)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] ADUIOTOP ENABLE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    i2s_xpll_open(&(i2s_internal_cfg[i2s_port]));

    if (i2s_port == HAL_I2S_0) {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        if (is_lock_sleep[i2s_port] == false) {
            hal_sleep_manager_lock_sleep(SLEEP_LOCK_AUDIO_TOP_AHB);/*lock sleep mode*/
            is_lock_sleep[HAL_I2S_0] = true;
            log_hal_info("[I2S0] lock sleep successfully\r\n");
        }
#endif
        /*Audiotop AHB clock*/
        hal_clock_enable(i2s_clock_cg[i2s_port]);
        //Enable I2S 26M CLOCK CG
        DRV_Reg8(I2S_GLOBAL_EN_CONTROL__F_CR_PDN_AUD_26M) = (uint8_t)0x0;
        DRV_Reg8(I2S_DL_SR_EN_CONTROL__F_CR_PDN_I2SO) = (uint8_t)0x0;
        DRV_Reg8(I2S_UL_SR_EN_CONTROL__F_CR_PDN_I2SIN) = (uint8_t)0x0;
        /*Soft reset I2S FIFO and in/out control*/
        DRV_Reg8(I2S_SOFT_RESET__F_CR_SOFT_RSTB) = (uint8_t)0x1;
        DRV_Reg8(I2S_SOFT_RESET__F_CR_SOFT_RSTB) = (uint8_t)0x0;
        //Enable I2S FIFO clock
        DRV_Reg8(I2S_GLOBAL_EN_CONTROL__F_CR_DL_FIFO_EN) = (uint8_t)0x1;
        DRV_Reg8(I2S_GLOBAL_EN_CONTROL__F_CR_UL_FIFO_EN) = (uint8_t)0x1;
        DRV_Reg8(I2S_GLOBAL_EN_CONTROL__F_CR_ENABLE) = (uint8_t)0x1;

    } else {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        if (is_lock_sleep[i2s_port] == false) {
            hal_sleep_manager_lock_sleep(SLEEP_LOCK_ASYS_TOP_AHB);/*lock sleep mode*/
            is_lock_sleep[HAL_I2S_1] = true;
            log_hal_info("[I2S1] lock sleep successfully\r\n");
        }
#endif
        /*Asystop AHB clock*/
        hal_clock_enable(i2s_clock_cg[i2s_port]);
        //Enable I2S 26M CLOCK CG
        DRV_Reg8(I2S1_GLOBAL_EN_CONTROL__F_CR_PDN_AUD_26M) = (uint8_t)0x0;
        DRV_Reg8(I2S1_UL_SR_EN_CONTROL__F_CR_PDN_I2SIN1) = (uint8_t)0x0;
        DRV_Reg8(I2S1_DL_SR_EN_CONTROL__F_CR_PDN_I2SO1) = (uint8_t)0x0;
        /*Soft reset I2S FIFO and in/out control*/
        DRV_Reg8(I2S1_SOFT_RESET__F_CR_I2S1_SOFT_RSTB) = (uint8_t)0x1;
        DRV_Reg8(I2S1_SOFT_RESET__F_CR_I2S1_SOFT_RSTB) = (uint8_t)0x0;
        //Enable I2S FIFO clock
        DRV_Reg8(I2S1_GLOBAL_EN_CONTROL__F_CR_I2S1_UL_FIFO_EN) = (uint8_t)0x1;
        DRV_Reg8(I2S1_GLOBAL_EN_CONTROL__F_CR_I2S1_DL_FIFO_EN) = (uint8_t)0x1;
        DRV_Reg8(I2S1_GLOBAL_EN_CONTROL__F_CR_I2S1_ENABLE) = (uint8_t)0x1;

    }

    i2s_internal_cfg[i2s_port].i2s_audiotop_enabled = true;

    return HAL_I2S_STATUS_OK;

}

hal_i2s_status_t  hal_i2s_disable_audio_top(void)
{
    return hal_i2s_disable_audio_top_ex(HAL_I2S_0);
}


hal_i2s_status_t  hal_i2s_disable_audio_top_ex(hal_i2s_port_t i2s_port)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] ADUIOTOP DISABLE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].i2s_state != I2S_STATE_INIT) {
        log_hal_error("[I2S%d] ADUIOTOP DISABLE , i2s_state=%d\r\n", i2s_port, i2s_internal_cfg[i2s_port].i2s_state);
        return HAL_I2S_STATUS_ERROR;
    }

    if (i2s_internal_cfg[i2s_port].i2s_audiotop_enabled != true) {
        return HAL_I2S_STATUS_ERROR;
    }

    if (i2s_port == HAL_I2S_0) {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        if (is_lock_sleep[i2s_port] == true) {
            hal_sleep_manager_unlock_sleep(SLEEP_LOCK_AUDIO_TOP_AHB);/*lock sleep mode*/
            is_lock_sleep[i2s_port] = false;
            log_hal_info("[I2S0] unlock sleep successfully");
        }
#endif
        i2s_internal_cfg[i2s_port].i2s_audiotop_enabled = false;
        //Disable I2S FIFO clock
        DRV_Reg8(I2S_GLOBAL_EN_CONTROL__F_CR_ENABLE) = (uint8_t)0x0;
        DRV_Reg8(I2S_GLOBAL_EN_CONTROL__F_CR_UL_FIFO_EN) = (uint8_t)0x0;
        DRV_Reg8(I2S_GLOBAL_EN_CONTROL__F_CR_DL_FIFO_EN) = (uint8_t)0x0;
        /*Soft reset I2S FIFO and in/out control*/
        DRV_Reg8(I2S_SOFT_RESET__F_CR_SOFT_RSTB) = (uint8_t)0x1;
        DRV_Reg8(I2S_SOFT_RESET__F_CR_SOFT_RSTB) = (uint8_t)0x0;
        //Disable I2S 26M CLOCK CG
        DRV_Reg8(I2S_DL_SR_EN_CONTROL__F_CR_PDN_I2SO) = (uint8_t)0x1;
        DRV_Reg8(I2S_UL_SR_EN_CONTROL__F_CR_PDN_I2SIN) = (uint8_t)0x1;
        DRV_Reg8(I2S_GLOBAL_EN_CONTROL__F_CR_PDN_AUD_26M) = (uint8_t)0x1;
        /*Audiotop AHB clock*/
        hal_clock_disable(i2s_clock_cg[i2s_port]);

    } else {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        if (is_lock_sleep[i2s_port] == true) {
            hal_sleep_manager_unlock_sleep(SLEEP_LOCK_ASYS_TOP_AHB);/*lock sleep mode*/
            is_lock_sleep[i2s_port] = false;
            log_hal_info("[I2S1] unlock sleep successfully");
        }
#endif
        i2s_internal_cfg[i2s_port].i2s_audiotop_enabled = false;
        //Disable I2S FIFO clock
        DRV_Reg8(I2S1_GLOBAL_EN_CONTROL__F_CR_I2S1_ENABLE) = (uint8_t)0x0;
        DRV_Reg8(I2S1_GLOBAL_EN_CONTROL__F_CR_I2S1_UL_FIFO_EN) = (uint8_t)0x0;
        DRV_Reg8(I2S1_GLOBAL_EN_CONTROL__F_CR_I2S1_DL_FIFO_EN) = (uint8_t)0x0;
        /*Soft reset I2S FIFO and in/out control*/
        DRV_Reg8(I2S1_SOFT_RESET__F_CR_I2S1_SOFT_RSTB) = (uint8_t)0x1;
        DRV_Reg8(I2S1_SOFT_RESET__F_CR_I2S1_SOFT_RSTB) = (uint8_t)0x0;
        //Disable I2S 26M CLOCK CG
        DRV_Reg8(I2S1_UL_SR_EN_CONTROL__F_CR_PDN_I2SIN1) = (uint8_t)0x1;
        DRV_Reg8(I2S1_DL_SR_EN_CONTROL__F_CR_PDN_I2SO1) = (uint8_t)0x1;
        DRV_Reg8(I2S1_GLOBAL_EN_CONTROL__F_CR_PDN_AUD_26M) = (uint8_t)0x1;
        /*Audiotop AHB clock*/
        hal_clock_disable(i2s_clock_cg[i2s_port]);
    }

    if ((i2s_internal_cfg[HAL_I2S_0].i2s_audiotop_enabled == false) && (i2s_internal_cfg[HAL_I2S_1].i2s_audiotop_enabled == false)) {
        i2s_xpll_close();
    }

    return HAL_I2S_STATUS_OK;

}

hal_i2s_status_t  hal_i2s_enable_tx_dma_interrupt(void)
{
    return hal_i2s_enable_tx_dma_interrupt_ex(HAL_I2S_0);
}

hal_i2s_status_t  hal_i2s_enable_tx_dma_interrupt_ex(hal_i2s_port_t i2s_port)
{

    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] TX DMA INTERRUPT ENABLE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].user_tx_callback_func == NULL) {
        log_hal_error("[I2S%d] TX DMA INTERRUPT ENABLE ERROR, user_tx_callback_func undefined\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    vdma_status_t status;

    status = vdma_enable_interrupt(i2s_vdma_tx_ch[i2s_port]);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] TX DMA INTERRUPT ENABLE ERROR, vdma_enable_interrupt failed\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    i2s_internal_cfg[i2s_port].i2s_vfifo.tx_dma_interrupt = true;

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t  hal_i2s_disable_tx_dma_interrupt(void)
{
    return hal_i2s_disable_tx_dma_interrupt_ex(HAL_I2S_0);
}


hal_i2s_status_t  hal_i2s_disable_tx_dma_interrupt_ex(hal_i2s_port_t i2s_port)
{

    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] TX DMA INTERRUPT DISABLE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].user_tx_callback_func == NULL) {
        log_hal_error("[I2S%d] TX DMA INTERRUPT DISABLE ERROR, user_tx_callback_func undefined\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    vdma_status_t status;

    status = vdma_disable_interrupt(i2s_vdma_tx_ch[i2s_port]);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] TX DMA INTERRUPT DISABLE ERROR, vdma_disable_interrupt failed\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }
    i2s_internal_cfg[i2s_port].i2s_vfifo.tx_dma_interrupt = false;

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t  hal_i2s_enable_rx_dma_interrupt(void)
{
    return hal_i2s_enable_rx_dma_interrupt_ex(HAL_I2S_0);
}


hal_i2s_status_t  hal_i2s_enable_rx_dma_interrupt_ex(hal_i2s_port_t i2s_port)
{

    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] RX DMA INTERRUPT ENABLE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].user_rx_callback_func == NULL) {
        log_hal_error("[I2S%d] RX DMA INTERRUPT ENABLE ERROR, user_rx_callback_func undefined\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    vdma_status_t status;

    status = vdma_enable_interrupt(i2s_vdma_rx_ch[i2s_port]);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] RX DMA INTERRUPT ENABLE ERROR, vdma_enable_interrupt failed\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }
    i2s_internal_cfg[i2s_port].i2s_vfifo.rx_dma_interrupt = true;

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t  hal_i2s_disable_rx_dma_interrupt(void)
{
    return hal_i2s_disable_rx_dma_interrupt_ex(HAL_I2S_0);
}


hal_i2s_status_t  hal_i2s_disable_rx_dma_interrupt_ex(hal_i2s_port_t i2s_port)
{

    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] RX DMA INTERRUPT DISABLE ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (i2s_internal_cfg[i2s_port].user_rx_callback_func == NULL) {
        log_hal_error("[I2S%d] RX DMA INTERRUPT DISABLE ERROR, user_rx_callback_func undefined\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    vdma_status_t status;

    status = vdma_disable_interrupt(i2s_vdma_rx_ch[i2s_port]);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] RX DMA INTERRUPT DISABLE ERROR, vdma_disable_interrupt failed\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }
    i2s_internal_cfg[i2s_port].i2s_vfifo.rx_dma_interrupt = false;

    return HAL_I2S_STATUS_OK;

}


hal_i2s_status_t hal_i2s_register_tx_callback(hal_i2s_tx_callback_t tx_callback, void *user_data)
{

    return hal_i2s_register_tx_vfifo_callback_ex(HAL_I2S_0, tx_callback, user_data);

}

hal_i2s_status_t hal_i2s_register_tx_callback_ex(hal_i2s_tx_callback_t tx_callback, void *user_data)
{

    return hal_i2s_register_tx_vfifo_callback_ex(HAL_I2S_0, tx_callback, user_data);

}

//Deprecate SDK API
hal_i2s_status_t hal_i2s_register_tx_vfifo_callback(hal_i2s_tx_callback_t tx_callback, void *user_data)
{

    return hal_i2s_register_tx_vfifo_callback_ex(HAL_I2S_0, tx_callback, user_data);

}

//Deprecate SDK API
hal_i2s_status_t hal_i2s_register_tx_vfifo_callback_ex(hal_i2s_port_t i2s_port, hal_i2s_tx_callback_t tx_callback, void *user_data)
{
    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] REGISTER TX CALLBACK ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (NULL == tx_callback) {
        log_hal_error("[I2S%d] REGISTER TX CALLBACK ERROR, tx_callback is null\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    i2s_internal_cfg[i2s_port].user_tx_callback_func = tx_callback;
    i2s_internal_cfg[i2s_port].user_tx_data = user_data;

    vdma_status_t status;
    status = vdma_register_callback(i2s_vdma_tx_ch[i2s_port], i2s_tx_dma_callback_handler, &i2s_internal_cfg[i2s_port]);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] REGISTER TX CALLBACK ERROR, vdma_register_callback failed\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    return HAL_I2S_STATUS_OK;

}

hal_i2s_status_t hal_i2s_register_rx_callback(hal_i2s_rx_callback_t rx_callback, void *user_data)
{
    return hal_i2s_register_rx_vfifo_callback_ex(HAL_I2S_0, rx_callback, user_data);
}

hal_i2s_status_t hal_i2s_register_rx_callback_ex(hal_i2s_rx_callback_t rx_callback, void *user_data)
{
    return hal_i2s_register_rx_vfifo_callback_ex(HAL_I2S_0, rx_callback, user_data);
}

//Deprecate SDK API
hal_i2s_status_t hal_i2s_register_rx_vfifo_callback(hal_i2s_rx_callback_t rx_callback, void *user_data)
{
    return hal_i2s_register_rx_vfifo_callback_ex(HAL_I2S_0, rx_callback, user_data);
}

//Deprecate SDK API
hal_i2s_status_t hal_i2s_register_rx_vfifo_callback_ex(hal_i2s_port_t i2s_port, hal_i2s_rx_callback_t rx_callback, void *user_data)
{

    if (i2s_port != HAL_I2S_0 && i2s_port != HAL_I2S_1) {
        log_hal_error("[I2S%d] REGISTER RX CALLBACK ERROR, invalid i2s_port\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    if (NULL == rx_callback) {
        log_hal_error("[I2S%d] REGISTER RX CALLBACK ERROR, rx_callback is null\r\n", i2s_port);
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    i2s_internal_cfg[i2s_port].user_rx_callback_func = rx_callback;
    i2s_internal_cfg[i2s_port].user_rx_data = user_data;

    vdma_status_t status;
    status = vdma_register_callback(i2s_vdma_rx_ch[i2s_port], i2s_rx_dma_callback_handler, &i2s_internal_cfg[i2s_port]);
    if (status != VDMA_OK) {
        log_hal_error("[I2S%d] REGISTER RX CALLBACK ERROR, vdma_register_callback failed\r\n", i2s_port);
        return HAL_I2S_STATUS_ERROR;
    }

    return HAL_I2S_STATUS_OK;
}


static void i2s_rx_dma_callback_handler(vdma_event_t event, void  *user_data)
{
    i2s_internal_t *i2s_internal = (i2s_internal_t *)user_data;
    i2s_internal->user_rx_callback_func(HAL_I2S_EVENT_DATA_NOTIFICATION, i2s_internal->user_rx_data);
}

static void i2s_tx_dma_callback_handler(vdma_event_t event, void  *user_data)
{
    i2s_internal_t *i2s_internal = (i2s_internal_t *)user_data;
    i2s_internal->user_tx_callback_func(HAL_I2S_EVENT_DATA_REQUEST, i2s_internal->user_tx_data);
}

#ifdef HAL_SLEEP_MANAGER_ENABLED

void i2s_suspend(hal_i2s_port_t i2s_port)
{
    if (i2s_internal_cfg[i2s_port].i2s_state == I2S_STATE_IDLE) {
        return;
    }
}

void i2s_resume(hal_i2s_port_t i2s_port)
{
    if (i2s_internal_cfg[i2s_port].i2s_state == I2S_STATE_IDLE) {
        return;
    }

    if (i2s_port == HAL_I2S_0) {
        //I2S
        DRV_Reg32(I2S_GLOBAL_CONTROL) = (uint32_t)i2s_global_cfg[HAL_I2S_0];
        DRV_Reg32(I2S_DL_CONTROL) = (uint32_t)i2s_dl_cfg[HAL_I2S_0];
        DRV_Reg32(I2S_UL_CONTROL) = (uint32_t)i2s_ul_cfg[HAL_I2S_0];
        DRV_Reg8(I2S_DL_SR_EN_CONTROL__F_CR_I2S_OUT_SR) = (uint8_t)i2s_dl_fs_cfg[HAL_I2S_0];
        DRV_Reg8(I2S_UL_SR_EN_CONTROL__F_CR_I2S_IN_SR) = (uint8_t)i2s_ul_fs_cfg[HAL_I2S_0];
    } else {
        //I2S
        DRV_Reg32(I2S1_GLOBAL_CONTROL) = (uint32_t)i2s_global_cfg[HAL_I2S_1];
        DRV_Reg32(I2S1_DL_CONTROL) = (uint32_t)i2s_dl_cfg[HAL_I2S_1];
        DRV_Reg32(I2S1_UL_CONTROL) = (uint32_t)i2s_ul_cfg[HAL_I2S_1];
        DRV_Reg8(I2S1_DL_SR_EN_CONTROL__F_CR_I2S1_OUT_SR) = (uint8_t)i2s_dl_fs_cfg[HAL_I2S_1];
        DRV_Reg8(I2S1_UL_SR_EN_CONTROL__F_CR_I2S1_IN_SR) = (uint8_t)i2s_ul_fs_cfg[HAL_I2S_1];
    }

    //VDMA
    if (i2s_internal_cfg[i2s_port].i2s_vfifo.tx_dma_configured) {
        hal_i2s_setup_tx_vfifo_ex(i2s_port,
                                  i2s_internal_cfg[i2s_port].i2s_vfifo.tx_vfifo_base,
                                  i2s_internal_cfg[i2s_port].i2s_vfifo.tx_vfifo_threshold,
                                  i2s_internal_cfg[i2s_port].i2s_vfifo.tx_vfifo_length);
    }
    if (i2s_internal_cfg[i2s_port].i2s_vfifo.rx_dma_configured) {
        hal_i2s_setup_rx_vfifo_ex(i2s_port,
                                  i2s_internal_cfg[i2s_port].i2s_vfifo.rx_vfifo_base,
                                  i2s_internal_cfg[i2s_port].i2s_vfifo.rx_vfifo_threshold,
                                  i2s_internal_cfg[i2s_port].i2s_vfifo.rx_vfifo_length);
    }

}

void i2s0_enter_suspend(void)
{
    i2s_suspend(HAL_I2S_0);
}

void i2s1_enter_suspend(void)
{
    i2s_suspend(HAL_I2S_1);

    //SWITCH MCLK PIN TO GPIO mode and PULL UP, avoid current leakage
#ifdef __I2S1_MCLK_PIN__
    hal_gpio_init(HAL_I2S1_MCLK_PIN);
    hal_pinmux_set_function(HAL_I2S1_MCLK_PIN, 0);
    hal_gpio_pull_up(HAL_I2S1_MCLK_PIN);
#endif

}

void i2s0_enter_resume(void)
{
    i2s_resume(HAL_I2S_0);
}

void i2s1_enter_resume(void)
{
    //SWITCH BACK TO MCLK MODE
#ifdef __I2S1_MCLK_PIN__
    hal_gpio_init(HAL_I2S1_MCLK_PIN);
    hal_pinmux_set_function(HAL_I2S1_MCLK_PIN, 5);
#endif
    i2s_resume(HAL_I2S_1);
}
#endif

#endif//#ifdef HAL_I2S_MODULE_ENABLED

