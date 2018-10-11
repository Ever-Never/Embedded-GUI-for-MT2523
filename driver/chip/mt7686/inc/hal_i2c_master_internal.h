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

#ifndef _HAL_I2C_MASTER_INTERNAL_H_
#define _HAL_I2C_MASTER_INTERNAL_H_

#include "hal_platform.h"
#if defined(HAL_I2C_MASTER_MODULE_ENABLED)
//#define I2C_ENABLE_DEBUG_LOG

#define I2C_FIFO_DEPTH   (8)

#define I2C_SOURCE_CLOCK_RATE_96M         (96000)     /* 96Mhz*/
#define I2C_SOURCE_CLOCK_BUS_CLOCK        (0)

#define I2C_CONTROL_REGISTER_DMA_BIT    (8)

#define I2C_COMPLETE_STATUS_BIT     (0)
#define I2C_ACKERR_STATUS_BIT       (8)
#define I2C_HS_NACKERR_STATUS_BIT   (16)

#define I2C_IO_PUSH_PULL    (0x0)
#define I2C_IO_OPEN_DRAIN   (0x3)

#define HAL_I2C_TX_DMA_CHANNEL PDMA_I2C0_TX,PDMA_I2C1_TX
#define HAL_I2C_RX_DMA_CHANNEL PDMA_I2C0_RX,PDMA_I2C1_RX

#define HAL_I2C_IRQ_TABLE I2C0_IRQn,I2C1_IRQn

#define HAL_I2C_PDN_TABLE HAL_CLOCK_CG_I2C0,HAL_CLOCK_CG_I2C1
#define HAL_I2C_SOURCE_CLOCK_TABLE I2C_SOURCE_CLOCK_RATE_96M,I2C_SOURCE_CLOCK_RATE_96M

#define I2C_DMA_FINISH_CALLBACK_COUNT  (2)

#define I2C_ENABLE_POLLING_TIMEOUT
#define I2C_POLLING_TIMEOUT_VALUE   (20000) /* 20ms */
#define I2C_INTERRUPT_TIMEOUT_VALUE (20000) /* 20ms */

#define I2C_WAIT_TRANSACTION_COMPLETE

#define I2C_TX_DMA_RUNNING_FLAG (0x01)
#define I2C_TX_DMA_FINISH_FLAG  (0x02)
#define I2C_RX_DMA_RUNNING_FLAG (0x04)
#define I2C_RX_DMA_FINISH_FLAG  (0x08)


/** @brief This enum define the I2C transaction speed.  */
typedef enum {
    I2C_FREQUENCY_50K  = 50,          /**<  Transmit data with 50kbps. */
    I2C_FREQUENCY_100K = 100,          /**<  Transmit data with 100kbps. */
    I2C_FREQUENCY_200K = 200,          /**<  Transmit data with 200kbps. */
    I2C_FREQUENCY_300K = 300,          /**<  Transmit data with 300kbps. */
    I2C_FREQUENCY_400K = 400,          /**<  Transmit data with 400kbps. */
    I2C_FREQUENCY_1M   = 1000,         /**<  Transmit data with 1mbps. */
    I2C_FREQUENCY_MAX                  /**<  Max transaction speed, don't use this value. */
} i2c_frequency_t;

typedef enum {
    I2C_TRANSACTION_PUSH_PULL_SEND_POLLING = 0,
    I2C_TRANSACTION_PUSH_PULL_SEND_DMA     = 1,
    I2C_TRANSACTION_PUSH_PULL_RECEIVE_POLLING = 2,
    I2C_TRANSACTION_PUSH_PULL_RECEIVE_DMA = 3,
    I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_POLLING = 4,
    I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_DMA = 5,

    I2C_TRANSACTION_OPEN_DRAIN_SEND_POLLING = 6,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA     = 7,
    I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_POLLING = 8,
    I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA = 9,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING = 10,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA = 11,

    I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA_EXTEND     = 12,
    I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA_EXTEND = 13,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND = 14
} i2c_transaction_type_t;

typedef enum {
    I2C_TRANSACTION_UNKNOWN_ERROR = -4,
    I2C_TRANSACTION_ACK_ERROR = -3,           /**<  an ACK error happened during transaction. */
    I2C_TRANSACTION_NACK_ERROR = -2,          /**<  an NACK error happened during transaction.*/
    I2C_TRANSACTION_TIMEOUT_ERROR = -1,       /**<  an timeout error happened during transaction.*/
    I2C_TRANSACTION_SUCCESS = 0               /**<  the transaction complete wihtout any error. */
} i2c_transaction_status_t;

typedef struct {
    uint32_t send_size;
    uint32_t receive_size;
    uint32_t transfer_length;
    uint32_t transfer_length_aux;
    uint32_t transaction_length;
} i2c_transaction_size_t;

typedef struct {
    i2c_transaction_type_t transaction_type;        /* transaction type */
    bool dma_finish;
    uint8_t i2c_port;
} i2c_dma_callback_parameter_t;

typedef struct {
    uint16_t clock_div;
    uint16_t timing;
} i2c_register_backup_t;



#define I2C_BUSY  1
#define I2C_IDLE  0
#define I2C_CHECK_AND_SET_BUSY(i2c_port,busy_status)  \
do{ \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    if(s_i2c_master_status[i2c_port] == I2C_BUSY){ \
        busy_status = HAL_I2C_STATUS_ERROR_BUSY; \
    } else { \
        s_i2c_master_status[i2c_port] = I2C_BUSY;  \
        busy_status = HAL_I2C_STATUS_OK; \
    } \
       restore_interrupt_mask(saved_mask); \
}while(0)

#define I2C_SET_IDLE(i2c_port)   \
do{  \
       s_i2c_master_status[i2c_port] = I2C_IDLE;  \
}while(0)

#ifdef I2C_ENABLE_DEBUG_LOG
#define I2C_DEBUG_FLOW_ARRAY_COUNT  (200)
#define I2C_DEBUG_LOG(i2c_port,i2c_api_type) log_hal_error("[D_I2C%d][type= %d]\r\n",i2c_port,i2c_api_type)
#define I2C_DEBUG_RECORD_FLOW(i2c_port,i2c_api_type) i2c_debug_record_flow(i2c_port,i2c_api_type)
#else
#define I2C_DEBUG_LOG(i2c_port,i2c_api_type)
#define I2C_DEBUG_RECORD_FLOW(i2c_port,i2c_api_type)
#endif

#define I2C_STOP(i2c_port) i2c_master_register[i2c_port]->START = 0x0
#define I2C_START(i2c_port) i2c_master_register[i2c_port]->START = 0x1
#define I2C_MASK_INTERRUPT(i2c_port) i2c_master_register[i2c_port]->INTR_MASK = 0x0
#define I2C_UNMASK_INTERRUPT(i2c_port) i2c_master_register[i2c_port]->INTR_MASK = 0x010101
#define I2C_QUERY_TRANSACTION_FINISH_STATUS(i2c_port) (i2c_master_register[i2c_port]->INTR_STAT)
#define I2C_IF_RECEIVE(i2c_port) ((i2c_master_register[i2c_port]->SLAVE_ADDR) & 0x1)
#define I2C_IF_DMA_ENABLE(i2c_port) (((i2c_master_register[i2c_port]->CONTROL) >> I2C_CONTROL_REGISTER_DMA_BIT) & 0x1)
#define I2C_GET_SLAVE_ADDRESS(i2c_port) ((i2c_master_register[i2c_port]->SLAVE_ADDR) >> 1)
#define I2C_CLEAR_FIFO(i2c_port) (i2c_master_register[i2c_port]->FIFO_ADDR_CLR = 0x1)
#define I2C_CLEAR_INTERRUPT_STATUS(i2c_port) (i2c_master_register[i2c_port]->INTR_STAT = 0x010101)
#define I2C_QUERY_RUNNING_STATUS(i2c_port)  (i2c_master_register[i2c_port]->START & 0x1)
#define I2C_RESET(i2c_port) (i2c_master_register[i2c_port]->SOFTRESET = 0x1)



#define I2C_GET_SEND_ADDRESS(slave_address)  ((slave_address<<1) & 0xFE)
#define I2C_GET_RECEIVE_ADDRESS(slave_address)  ((slave_address<<1) | 0x01)


#define I2C_IF_DMA_FINISH(i2c_port) (true == s_i2c_dma_parameter[i2c_port].dma_finish)



void i2c_set_frequency(uint32_t i2c_port, uint32_t transfer_frequency);
void i2c_config_hardware(uint32_t i2c_port, i2c_transaction_type_t transaction_type, uint8_t slave_address, i2c_transaction_size_t *config_size);
void i2c_write_fifo(uint32_t i2c_port, const uint8_t *data, uint32_t size);
void i2c_read_fifo(uint32_t i2c_port, uint8_t *buffer, uint32_t size);
i2c_transaction_status_t i2c_get_transaction_status(uint32_t i2c_port);
void i2c_debug_record_flow(hal_i2c_port_t i2c_port, uint32_t type);

#endif
#endif


