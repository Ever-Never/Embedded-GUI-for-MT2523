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

#ifndef __HAL_SPI_MASTER_INTERNAL_H__
#define __HAL_SPI_MASTER_INTERNAL_H__

#ifdef HAL_SPI_MASTER_MODULE_ENABLED

typedef enum {
    SPI_MASTER_STATUS_UNINITIALIZED,
    SPI_MASTER_STATUS_POLLING_MODE,
    SPI_MASTER_STATUS_DMA_MODE
} spi_master_status_t;

typedef enum {
    SPI_MASTER_TX = 0,
    SPI_MASTER_RX = 1
} spi_master_direction_t;

typedef enum {
    SPI_MASTER_MODE_FIFO = 0,
    SPI_MASTER_MODE_DMA  = 1
} spi_master_mode_t;

#define SPI_MASTER_IRQ_TABLE SPI_MST_IRQn
#define SPI_MASTER_CG_TABLE HAL_CLOCK_CG_SPIMST
#define SPI_MASTER_PRI_TABLE SPI_MST_IRQ_PRIORITY

#define SPI_BUSY 1
#define SPI_IDLE 0
#define SPI_CHECK_AND_SET_BUSY(spi_port,busy_status)  \
do{ \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    if(g_spi_master_status[spi_port] == SPI_BUSY){ \
        busy_status = HAL_SPI_MASTER_STATUS_ERROR_BUSY; \
    } else { \
        g_spi_master_status[spi_port] = SPI_BUSY;  \
        busy_status = HAL_SPI_MASTER_STATUS_OK; \
    } \
       restore_interrupt_mask(saved_mask); \
}while(0)

#define SPI_SET_IDLE(spi_port)   \
do{  \
       g_spi_master_status[spi_port] = SPI_IDLE;  \
}while(0)

/* clock frequency related macro */
#define  SPI_MASTER_INPUT_CLOCK_FREQUENCY 96000000

/* packet length and packet count macro */
#define  SPI_MASTER_MAX_SIZE_FOR_NON_PAUSE      0x1000000
#define  SPI_MASTER_MAX_PACKET_LENGTH           0x10000
#define  SPI_MASTER_MAX_PACKET_LENGTH_MASK      0xFFFF
#define  SPI_MASTER_MAX_PACKET_COUNT            0x100
#define  SPI_MASTER_MAX_PACKET_COUNT_MASK       0xFF

/* function prototype */
void spi_master_isr_handler(hal_spi_master_port_t master_port, hal_spi_master_callback_t user_callback, void *user_data);
void spi_master_init(hal_spi_master_port_t master_port, const hal_spi_master_config_t *spi_config);
#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
void spi_master_set_advanced_config(hal_spi_master_port_t master_port, const hal_spi_master_advanced_config_t *advanced_config);
#endif
uint32_t spi_master_get_status(hal_spi_master_port_t master_port);
void spi_master_set_rwaddr(hal_spi_master_port_t master_port, spi_master_direction_t type, const uint8_t *addr);
hal_spi_master_status_t spi_master_push_data(hal_spi_master_port_t master_port, const uint8_t *data, uint32_t size, uint32_t total_size);
hal_spi_master_status_t spi_master_pop_data(hal_spi_master_port_t master_port, uint8_t *buffer, uint32_t size);
void spi_master_set_interrupt(hal_spi_master_port_t master_port, bool status);
void spi_master_clear_fifo(hal_spi_master_port_t master_port);
void spi_master_set_mode(hal_spi_master_port_t master_port, spi_master_direction_t type, spi_master_mode_t mode);
void spi_master_start_transfer_fifo(hal_spi_master_port_t master_port, bool is_write);
void spi_master_start_transfer_dma(hal_spi_master_port_t master_port, bool is_continue, bool is_write);
void spi_master_start_transfer_dma_blocking(hal_spi_master_port_t master_port, bool is_write);
hal_spi_master_status_t spi_master_analyse_transfer_size(hal_spi_master_port_t master_port, uint32_t size);
#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
void spi_master_set_chip_select_timing(hal_spi_master_port_t master_port, hal_spi_master_chip_select_timing_t chip_select_timing);
#endif
#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
void spi_master_set_deassert(hal_spi_master_port_t master_port, hal_spi_master_deassert_t deassert);
#endif
#ifdef HAL_SLEEP_MANAGER_ENABLED
void spi_master_backup_register_callback(void *data);
void spi_master_restore_register_callback(void *data);
#endif
#ifdef HAL_SPI_MASTER_FEATURE_DUAL_QUAD_MODE
void spi_master_set_type(hal_spi_master_port_t master_port, hal_spi_master_mode_t mode);
void spi_master_set_command_bytes(hal_spi_master_port_t master_port, uint8_t command_bytes);
void spi_master_set_dummy_bits(hal_spi_master_port_t master_port, uint8_t dummy_bits);
#endif
void spi_master_reset_default_value(hal_spi_master_port_t master_port);

#endif   /* HAL_SPI_MASTER_MODULE_ENABLED */

#endif   /*__HAL_SPI_MASTER_INTERNAL_H__*/

