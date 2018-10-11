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

#include "hal_i2c_master.h"
#if defined(HAL_I2C_MASTER_MODULE_ENABLED)
#include "hal_i2c_master_internal.h"
#include "hal_i2c_pmic.h"
#include "hal_pdma_internal.h"
#include "hal_log.h"
#include "hal_clock.h"
#include "hal_gpt.h"
#include <assert.h>


I2C_REGISTER_T *const i2c_master_register[(uint32_t)HAL_I2C_MASTER_MAX + (uint32_t)PMIC_I2C_MASTER_MAX] = {(I2C_REGISTER_T *)(I2C_0_BASE), (I2C_REGISTER_T *)(I2C_1_BASE), (I2C_REGISTER_T *)(I2C_2_BASE), (I2C_REGISTER_T *)(I2C_D2D_BASE)};
const pdma_channel_t g_i2c_tx_dma_channel[(uint32_t)HAL_I2C_MASTER_MAX + (uint32_t)PMIC_I2C_MASTER_MAX] = {HAL_I2C_TX_DMA_CHANNEL};
const pdma_channel_t g_i2c_rx_dma_channel[(uint32_t)HAL_I2C_MASTER_MAX + (uint32_t)PMIC_I2C_MASTER_MAX] = {HAL_I2C_RX_DMA_CHANNEL};
const hal_clock_cg_id g_i2c_pdn_table[(uint32_t)HAL_I2C_MASTER_MAX + (uint32_t)PMIC_I2C_MASTER_MAX] = {HAL_I2C_PDN_TABLE};


uint32_t g_i2c_source_clock_table[(uint32_t)HAL_I2C_MASTER_MAX + (uint32_t)PMIC_I2C_MASTER_MAX] = {HAL_I2C_SOURCE_CLOCK_TABLE};
/*
 * To record the call flow for debuging.
 *
 */
#ifdef I2C_ENABLE_DEBUG_LOG
uint32_t i2c_debug_flow_array[HAL_I2C_MASTER_MAX + PMIC_I2C_MASTER_MAX][I2C_DEBUG_FLOW_ARRAY_COUNT] = {0};
static uint32_t debug_loop[HAL_I2C_MASTER_MAX + PMIC_I2C_MASTER_MAX] = {0};
void i2c_debug_record_flow(hal_i2c_port_t i2c_port, uint32_t type)
{
    I2C_DEBUG_LOG(i2c_port, type);
    i2c_debug_flow_array[i2c_port][debug_loop[i2c_port]] = type;
    debug_loop[i2c_port] = (debug_loop[i2c_port] + 1) % I2C_DEBUG_FLOW_ARRAY_COUNT;
}
#endif


/*
    Note: 1. I2C0,I2C1 source clock fixed 26Mhz.
          2. I2C2,I2C D2D source clock are bus clock:26Mhz,13Mhz,62.5M.
*/

void i2c_set_frequency(uint32_t i2c_port, uint32_t transfer_frequency)
{
    uint32_t    frequency = 0;
    uint32_t    step_cnt_div;
    uint32_t    sample_cnt_div;
    uint32_t    orignal_register_value, register_value;
    uint32_t    temp;

    /* set clock div to 0 */
    i2c_master_register[i2c_port]->CLOCK_DIV = 0;
    switch (transfer_frequency) {
        case (HAL_I2C_FREQUENCY_50K) :
            /* set clock div to 1 to acheive 50KHz frequency */
            i2c_master_register[i2c_port]->CLOCK_DIV = 1;
            frequency = I2C_FREQUENCY_100K;
            break;
        case (HAL_I2C_FREQUENCY_100K) :
            frequency = I2C_FREQUENCY_100K;
            break;
        case (HAL_I2C_FREQUENCY_200K) :
            frequency = I2C_FREQUENCY_200K;
            break;
        case (HAL_I2C_FREQUENCY_300K) :
            frequency = I2C_FREQUENCY_300K;
            break;
        case (HAL_I2C_FREQUENCY_400K) :
            frequency = I2C_FREQUENCY_400K;
            break;
        case (HAL_I2C_FREQUENCY_1M) :
            frequency = I2C_FREQUENCY_1M;
            break;
        default :
            /* this branch is special for pmu */
            /* pmu driver will use real i2c frequency as transfer_frquency */
            frequency = transfer_frequency;
            break;
    }

    /***********************************************************
    * Note: according to datasheet
    *  speed = source_clock/(clock_div+1)/(2*(step_cnt_div+1)*(sample_cnt_div+1))
    ************************************************************/
    /* Fast Mode Speed */
    for (sample_cnt_div = 1; sample_cnt_div <= 8; sample_cnt_div++) {

        temp = (frequency * 2 * sample_cnt_div);
        step_cnt_div = (g_i2c_source_clock_table[i2c_port] + temp - 1) / temp;  /* cast the <1 part */

        if (step_cnt_div <= 64) {
            break;
        }
    }

    sample_cnt_div--;
    step_cnt_div--;

    /* write timing register */
    orignal_register_value = (i2c_master_register[i2c_port]->TIMING) & (~(I2C_SAMPLE_CNT_DIV_BIT_MASK | I2C_STEP_CNT_DIV_BIT_MASK));
    register_value = orignal_register_value | (I2C_SAMPLE_CNT_DIV_BIT_MASK & (sample_cnt_div << I2C_SAMPLE_CNT_DIV_BIT_SHIFT)) | (I2C_STEP_CNT_DIV_BIT_MASK & (step_cnt_div << I2C_STEP_CNT_DIV_BIT_SHIFT));
    i2c_master_register[i2c_port]->TIMING = register_value;
}

/***********************************************************
 * Note: currently, the DMA maximum length is 15Bytes.
 * To let most of the sensors fit to the transfer format, we chose to default
 * set transaction length to 1. If customer want to transaction data
 * more than 15 once, we need to add a new function or new featurn to
 * let the customer give the transfer and transaction length.
 ************************************************************/
void i2c_config_hardware(uint32_t i2c_port, i2c_transaction_type_t transaction_type, uint8_t slave_address, i2c_transaction_size_t *config_size)
{
    /* set slave address */
    i2c_master_register[i2c_port]->SLAVE_ADDR = slave_address;
    /* clear fifo */
    I2C_CLEAR_FIFO(i2c_port);
    /* clear interrupt status */
    I2C_CLEAR_INTERRUPT_STATUS(i2c_port);

    /* set control register */
    switch (transaction_type) {
        case I2C_TRANSACTION_PUSH_PULL_SEND_POLLING:
        case I2C_TRANSACTION_OPEN_DRAIN_SEND_POLLING:
            if (I2C_TRANSACTION_PUSH_PULL_SEND_POLLING == transaction_type) {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_PUSH_PULL;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_PUSH_PULL_SEND_POLLING;
            } else {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_OPEN_DRAIN;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_OPEN_DRAIN_SEND_POLLING;

            }
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->send_size;
            i2c_master_register[i2c_port]->TRANSAC_LEN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_PUSH_PULL_SEND_DMA:
        case I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA:
            if (I2C_TRANSACTION_PUSH_PULL_SEND_DMA == transaction_type) {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_PUSH_PULL;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_PUSH_PULL_SEND_DMA;
            } else {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_OPEN_DRAIN;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_OPEN_DRAIN_SEND_DMA;
            }
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->send_size;
            i2c_master_register[i2c_port]->TRANSAC_LEN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_PUSH_PULL_RECEIVE_POLLING:
        case I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_POLLING:
            if (I2C_TRANSACTION_PUSH_PULL_RECEIVE_POLLING == transaction_type) {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_PUSH_PULL;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_PUSH_PULL_RECEIVE_POLLING;
            } else {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_OPEN_DRAIN;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_OPEN_DRAIN_RECEIVE_POLLING;
            }
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->receive_size;
            i2c_master_register[i2c_port]->TRANSAC_LEN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_PUSH_PULL_RECEIVE_DMA:
        case I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA:
            if (I2C_TRANSACTION_PUSH_PULL_RECEIVE_DMA == transaction_type) {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_PUSH_PULL;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_PUSH_PULL_RECEIVE_DMA;
            } else {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_OPEN_DRAIN;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_OPEN_DRAIN_RECEIVE_DMA;
            }
            i2c_master_register[i2c_port]->TRANSFER_LEN = 1;
            i2c_master_register[i2c_port]->TRANSAC_LEN = config_size->receive_size;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_POLLING:
        case I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING:
            if (I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_POLLING == transaction_type) {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_PUSH_PULL;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_PUSH_PULL_SEND_TO_RECEIVE_POLLING;
            } else {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_OPEN_DRAIN;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING;
            }
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->send_size;
            i2c_master_register[i2c_port]->TRANSAC_LEN =  2;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = config_size->receive_size;
            break;
        case I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_DMA:
        case I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA:
            if (I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_DMA == transaction_type) {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_PUSH_PULL;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_PUSH_PULL_SEND_TO_RECEIVE_DMA;
            } else {
                i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_OPEN_DRAIN;
                i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_OPEN_DRAIN_SEND_TO_RECEIVE_DMA;
            }
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->send_size;
            i2c_master_register[i2c_port]->TRANSAC_LEN = 1 + config_size->receive_size;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;

        case I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA_EXTEND:
            i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_OPEN_DRAIN;
            i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_OPEN_DRAIN_SEND_DMA_EXTEND;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->transfer_length;
            i2c_master_register[i2c_port]->TRANSAC_LEN = config_size->transaction_length;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA_EXTEND:
            i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_OPEN_DRAIN;
            i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_OPEN_DRAIN_RECEIVE_DMA_EXTEND;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->transfer_length;
            i2c_master_register[i2c_port]->TRANSAC_LEN = config_size->transaction_length;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;

            break;
        case I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND:
            i2c_master_register[i2c_port]->IO_CONFIG = I2C_IO_OPEN_DRAIN;
            i2c_master_register[i2c_port]->CONTROL = I2C_CONTROL_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->transfer_length;
            i2c_master_register[i2c_port]->TRANSAC_LEN = config_size->transaction_length;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = config_size->transfer_length_aux;
            break;
        default :
            break;

    }

}




void i2c_write_fifo(uint32_t i2c_port, const uint8_t *data, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++) {
        i2c_master_register[i2c_port]->DATA_PORT = *(data + i);
    }
}

void i2c_read_fifo(uint32_t i2c_port, uint8_t *buffer, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++) {
        *(buffer + i) = i2c_master_register[i2c_port]->DATA_PORT;
    }
}

i2c_transaction_status_t i2c_get_transaction_status(uint32_t i2c_port)
{
    uint16_t register_value;
    i2c_transaction_status_t return_status = I2C_TRANSACTION_UNKNOWN_ERROR;

    register_value = i2c_master_register[i2c_port]->INTR_STAT;

    if (0 != (register_value & (0x1 << I2C_ACKERR_STATUS_BIT))) {
        return_status = I2C_TRANSACTION_ACK_ERROR;
    } else if (0 != (register_value & (0x1 << I2C_HS_NACKERR_STATUS_BIT))) {
        return_status = I2C_TRANSACTION_NACK_ERROR;
    } else if (0 != (register_value & (0x1 << I2C_HS_TIMEOUT_STATUS_BIT))) {
        return_status = I2C_TRANSACTION_TIMEOUT_ERROR;
    } else if (0x1 == register_value) {
        return_status = I2C_TRANSACTION_SUCCESS;
    }


#ifdef I2C_WAIT_TRANSACTION_COMPLETE
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;
    if(return_status != I2C_TRANSACTION_SUCCESS){
        /*wait for transcation_complete flag is set*/
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
        while(!(i2c_master_register[i2c_port]->INTR_STAT & (0x1 << I2C_COMPLETE_STATUS_BIT)))
        {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
            hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
            if (I2C_INTERRUPT_TIMEOUT_VALUE < gpt_duration_count)
            {
                log_hal_error("[I2C%d][isr]:trans_comp timeout!\r\n", i2c_port);
                break;
            }
        }
    }
#endif

    I2C_CLEAR_INTERRUPT_STATUS(i2c_port);
    return return_status;
}

#endif

