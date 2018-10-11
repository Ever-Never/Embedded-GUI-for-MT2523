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

#include "hal_pinmux_define.h"

typedef enum {
    SPI_TX = 0,
    SPI_RX = 1
} spi_direction_t;

typedef enum {
    SPI_MODE_FIFO = 0,
    SPI_MODE_DMA  = 1
} spi_mode_t;

#define HAL_SPI_IRQ_TABLE SPI_MST0_IRQn, SPI_MST1_IRQn,SPI_MST2_IRQn, SPI_MST3_IRQn
#define HAL_SPI_CG_TABLE HAL_CLOCK_CG_SPI0, HAL_CLOCK_CG_SPI1, HAL_CLOCK_CG_SPI2, HAL_CLOCK_CG_SPI3
#define HAL_SPI_PRI_TABLE SPI_MST0_IRQ_PRIORITY, SPI_MST1_IRQ_PRIORITY, SPI_MST2_IRQ_PRIORITY, SPI_MST3_IRQ_PRIORITY

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

/*clock frequency related macro*/
#define  CLOCK_SOURCE_DIVIDE_2           ((uint32_t)0x00C65D40)  /*13000000Hz*/

/* packet length and packet count macro */
#define  SPI_MAX_SIZE_FOR_NON_PAUSE      ((uint32_t)0x00040000)
#define  SPI_MAX_PACKET_LENGTH           ((uint16_t)0x0400)
#define  SPI_MAX_PACKET_COUNT            ((uint16_t)0x0100)

/* spi master controller register default value */
#define  SPI_DEFAULT_VALUE               ((uint32_t)0x00000000)

/* spi master pin related address */
#define SPI0_CLK_PAD_B_GPIO5_MODE_ADDRESS  (0xA2020C00)
#define SPI0_CLK_PAD_B_GPIO5_MODE_SHIFT    (20)
#define SPI0_CLK_PAD_B_GPIO5_MODE_MASK     (0x07 << SPI0_CLK_PAD_B_GPIO5_MODE_SHIFT)

#define SPI1_CLK_PAD_B_GPIO12_MODE_ADDRESS  (0xA2020C10)
#define SPI1_CLK_PAD_B_GPIO12_MODE_SHIFT    (16)
#define SPI1_CLK_PAD_B_GPIO12_MODE_MASK     (0x07 << SPI1_CLK_PAD_B_GPIO12_MODE_SHIFT)

#define SPI2_CLK_PAD_B_GPIO40_MODE_ADDRESS  (0xA2020C50)
#define SPI2_CLK_PAD_B_GPIO40_MODE_SHIFT    (0)
#define SPI2_CLK_PAD_B_GPIO40_MODE_MASK     (0x0f << SPI2_CLK_PAD_B_GPIO40_MODE_SHIFT)

#define SPI3_CLK_PAD_B_GPIO33_MODE_ADDRESS  (0xA2020C40)
#define SPI3_CLK_PAD_B_GPIO33_MODE_SHIFT    (4)
#define SPI3_CLK_PAD_B_GPIO33_MODE_MASK     (0x0f << SPI3_CLK_PAD_B_GPIO33_MODE_SHIFT)


extern spi_direction_t g_spi_direction[HAL_SPI_MASTER_MAX];

/* function prototype */
void spi_isr_handler(hal_spi_master_port_t master_port, hal_spi_master_callback_t user_callback, void *user_data);
void spi_master_init(hal_spi_master_port_t master_port, const hal_spi_master_config_t *spi_config);
#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
void spi_master_set_advanced_config(hal_spi_master_port_t master_port, const hal_spi_master_advanced_config_t *advanced_config);
#endif
uint32_t spi_get_status(hal_spi_master_port_t master_port);
void spi_set_rwaddr(hal_spi_master_port_t master_port, spi_direction_t type, const uint8_t *addr);
hal_spi_master_status_t spi_push_data(hal_spi_master_port_t master_port, const uint8_t *data, uint32_t size);
hal_spi_master_status_t spi_pop_data(hal_spi_master_port_t master_port, uint8_t *buffer, uint32_t size);
void spi_set_interrupt(hal_spi_master_port_t master_port, bool status);
void spi_clear_fifo(hal_spi_master_port_t master_port);
void spi_set_mode(hal_spi_master_port_t master_port, spi_direction_t type, spi_mode_t mode);
void spi_start_transfer(hal_spi_master_port_t master_port, spi_mode_t mode);
void spi_start_transfer_dma_blocking(hal_spi_master_port_t master_port);
hal_spi_master_status_t spi_analyse_transfer_size(hal_spi_master_port_t master_port, uint32_t size);
#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
void spi_master_set_chip_select_timing(hal_spi_master_port_t master_port, hal_spi_master_chip_select_timing_t chip_select_timing);
#endif
#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
void spi_master_set_deassert(hal_spi_master_port_t master_port, hal_spi_master_deassert_t deassert);
#endif

#endif   /* HAL_SPI_MASTER_MODULE_ENABLED */
#endif   /*__HAL_SPI_MASTER_INTERNAL_H__*/

