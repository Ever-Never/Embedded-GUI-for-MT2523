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

#ifndef __HAL_SDIO_DEFINE_H__
#define __HAL_SDIO_DEFINE_H__

#include "hal_platform.h"
#include "hal_sd_define.h"
#include "hal_dvfs_internal.h"
#include "hal_sdio.h"

#ifdef HAL_SDIO_MODULE_ENABLED


/**********************************************************************************************************/
/******************************************CMD5 RESPONSE***************************************************/
/**********************************************************************************************************/
/*| 1 | 2 |     6    | 1 |           3            |       1        |      3     |    24   |     7    | 1 |*/
/*| S | D | Reserved | C | Numberof I/O functions | Memory Present | Stuff Bits | I/O OCR | Reserved | E |*/
/**********************************************************************************************************/
#define SDIO_COMMAND5_RESPONSE_IS_READY_BIT_SHIFT          (31)
#define SDIO_COMMAND5_RESPONSE_IO_FUNCTION_BIT_SHIFT       (28)
#define SDIO_COMMAND5_RESPONSE_MEMORY_PRESENT_BIT_SHIFT    (27)
#define SDIO_COMMAND5_RESPONSE_OCR_BIT_SHIFT               (0)


#define SDIO_COMMAND5_RESPONSE_IS_READY_BIT_MASK           ((uint32_t)0x01 << SDIO_COMMAND5_RESPONSE_IS_READY_BIT_SHIFT)
#define SDIO_COMMAND5_RESPONSE_IO_FUNCTION_BIT_MASK        (0x07 << SDIO_COMMAND5_RESPONSE_IO_FUNCTION_BIT_SHIFT)
#define SDIO_COMMAND5_RESPONSE_MEMORY_PRESENT_BIT_MASK     (1 << SDIO_COMMAND5_RESPONSE_MEMORY_PRESENT_BIT_SHIFT)
#define SDIO_COMMAND5_RESPONSE_OCR_BIT_MASK                (0xffffff << SDIO_COMMAND5_RESPONSE_OCR_BIT_SHIFT)

/********************************************************************/
/**************************CMD52 RESPONSE (R5)***********************/
/********************************************************************/
/*| 1 | 1 |       6       |  16   |       8       |   8  |  7  | 1 |*/
/*| S | D | Command index | stuff | response flag | data | CRC | E |*/
/********************************************************************/
#define SDIO_R5_RESPONSE_FLAG_BIT_SHIFT   (8)
#define SDIO_R5_RESPONSE_DATA_BIT_SHIFT   (0)

#define SDIO_R5_RESPONSE_FLAG_BIT_MASK    (0xff << SDIO_R5_RESPONSE_FLAG_BIT_SHIFT)
#define SDIO_R5_RESPONSE_DATA_BIT_MASK    (0xff << SDIO_R5_RESPONSE_DATA_BIT_SHIFT)

/*R5 response flag define*/
#define SDIO_R5_RESPONSE_FLAG_STATE_BIT_SHIFT   (4 + SDIO_R5_RESPONSE_FLAG_BIT_SHIFT)

#define SDIO_R5_RESPONSE_FLAG_STATE_BIT_MASK    (0x3 << SDIO_R5_RESPONSE_FLAG_STATE_BIT_SHIFT)
#define SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK    (0xcb)


#define SDIO_CARD_STATUS_ERROR_MASK          (0xffff0000)


/**************************************************************************************************************************/
/************************************************CMD52 ********************************************************************/
/**************************************************************************************************************************/
/*| 1 | 1 |       6       |  1  |           3            |  1  |  1    |         17       |   1   |      8     |  7  | 1 |*/
/*| S | D | Command index | R/W | Numberof I/O functions | RAW | stuff | register address | stuff | write data | CRC | E |*/
/**************************************************************************************************************************/

#define COMMAND52_DIRECTION_BIT_SHIFT         (31)
#define COMMAND52_FUNCTION_NUMBER_BIT_SHIFT   (28)
#define COMMAND52_RAW_BIT_SHIFT               (27)
#define COMMAND52_REGISTER_ADDRESS_BIT_SHIFT  (9)
#define COMMAND52_WRITE_DATA_BIT_SHIFT        (0)

#define COMMAND52_DIRECTION_BIT_MASK          ((uint32_t)1 << COMMAND52_DIRECTION_BIT_SHIFT)
#define COMMAND52_FUNCTION_NUMBER_BIT_MASK    (0x7 << COMMAND52_FUNCTION_NUMBER_BIT_SHIFT)
#define COMMAND52_RAW_BIT_MASK                (1 << COMMAND52_RAW_BIT_SHIFT)
#define COMMAND52_REGISTER_ADDRESS_BIT_MASK   (0x1ffff << COMMAND52_REGISTER_ADDRESS_BIT_SHIFT)
#define COMMAND52_WRITE_DATA_BIT_MASK         (0xff << COMMAND52_WRITE_DATA_BIT_SHIFT)


/*********************************************************************************************************************************/
/************************************************CMD53 ***************************************************************************/
/*********************************************************************************************************************************/
/*| 1 | 1 |       6       |  1  |           3            |      1     |    1    |         17       |         9        |  7  | 1 |*/
/*| S | D | Command index | R/W | Numberof I/O functions | Block mode | OP code | register address | byte/block count | CRC | E |*/
/*********************************************************************************************************************************/
#define COMMAND53_DIRECTION_BIT_SHIFT               (31)
#define COMMAND53_FUNCTION_NUMBER_BIT_SHIFT         (28)
#define COMMAND53_BLOCK_MODE_BIT_SHIFT              (27)
#define COMMAND53_OP_BIT_SHIFT                      (26)
#define COMMAND53_REGISTER_ADDRESS_BIT_SHIFT        (9)
#define COMMAND53_COUNT_BIT_SHIFT                   (0)

#define COMMAND53_DIRECTION_BIT_MASK                ((uint32_t)1 << COMMAND53_DIRECTION_BIT_SHIFT)
#define COMMAND53_FUNCTION_NUMBER_BIT_MASK          (0x07 << COMMAND53_FUNCTION_NUMBER_BIT_SHIFT)
#define COMMAND53_BLOCK_MODE_BIT_MASK               (1 << COMMAND53_BLOCK_MODE_BIT_SHIFT)
#define COMMAND53_OP_BIT_MASK                       (1 << COMMAND53_OP_BIT_SHIFT)
#define COMMAND53_REGISTER_ADDRESS_BIT_MASK         (0x1ffff << COMMAND53_REGISTER_ADDRESS_BIT_SHIFT)
#define COMMAND53_COUNT_BIT_MASK                    (0x1ff << COMMAND53_COUNT_BIT_SHIFT)

/*CCCR register define,card capability(Byte 19 (0x13))*/
#define SDIO_CCCR_EHS_BIT_SHIFT        (1)
#define SDIO_CCCR_SHS_BIT_SHIFT        (0)

#define SDIO_CCCR_EHS_BIT_MASK        (1 << SDIO_CCCR_EHS_BIT_SHIFT)
#define SDIO_CCCR_SHS_BIT_MASK        (1 << SDIO_CCCR_SHS_BIT_SHIFT)

/*CCCR register define,card capability(Byte 8)*/
#define SDIO_CCCR_4BLS_BIT_SHIFT       (7)
#define SDIO_CCCR_LSC_BIT_SHIFT        (6)
#define SDIO_CCCR_E4MI_BIT_SHIFT       (5)
#define SDIO_CCCR_S4MI_BIT_SHIFT       (4)

#define SDIO_CCCR_4BLS_BIT_MASK        (1 << SDIO_CCCR_4BLS_BIT_SHIFT)
#define SDIO_CCCR_LSC_BIT_MASK         (1 << SDIO_CCCR_LSC_BIT_SHIFT)
#define SDIO_CCCR_E4MI_BIT_MASK        (1 << SDIO_CCCR_E4MI_BIT_SHIFT)
#define SDIO_CCCR_S4MI_BIT_MASK        (1 << SDIO_CCCR_S4MI_BIT_SHIFT)

/*CCCR register define,bus interface control(Byte 7)*/
#define SDIO_CCCR_CD_BIT_SHIFT        (7)
#define SDIO_CCCR_BUS1_BIT_SHIFT      (1)
#define SDIO_CCCR_BUS0_BIT_SHIFT      (0)

#define SDIO_CCCR_CD_BIT_MASK         (1 << SDIO_CCCR_CD_BIT_SHIFT)
#define SDIO_CCCR_BUS1_BIT_MASK       (1 << SDIO_CCCR_BUS1_BIT_SHIFT)
#define SDIO_CCCR_BUS0_BIT_MASK       (1 << SDIO_CCCR_BUS0_BIT_SHIFT)


#define SDIO_CCCR_SIZE   (32)
#define SDIO_FBR_SIZE    (256)
#define SDIO_TUPLE_SIZE  (256)

#define CISTPL_NULL (0)
#define CISTPL_END  (0xff)
#define CISTPL_MANFID  (0x20)

#define SDIO_DEFAULT_MAX_SPEED    (25000)  /*The max speed is 25MHZ in default.*/


#define SDIO_BLOCK_SIZE   (512)

typedef enum {
    SDIO_STATE_DIS,     /*disabled, initalize,standby and inactive states -- card not selected*/
    SDIO_STATE_CMD,     /*data lines free, command waiting*/
    SDIO_STATE_TRN,     /*transfer*/
    SDIO_STATE_RFU      /*Reserved for Future Use*/
} sdio_state_t;


typedef struct {
    uint16_t vendor;
    uint16_t device;
} sdio_cis_t;


typedef struct {
    uint8_t number_function;
    uint8_t response;
    hal_sdio_bus_width_t bus_width;
    uint8_t power_control;
    uint8_t high_speed;
    bool    is_memory_present;
    bool    is_io_ready;
    bool    is_ocr_valid;
    bool    is_stop;
    bool    is_initialized;
    sdio_state_t state;
    uint8_t capability;
    uint32_t rca;
    uint32_t ocr;
    uint32_t block_size[HAL_SDIO_FUNCTION_1 + 1];
    sdio_internal_status_t status;
    bool     is_busy;
    sdio_cis_t cis;
} sdio_information_t;

#ifdef HAL_SLEEP_MANAGER_ENABLED
typedef struct {
    hal_sdio_config_t config;
    hal_sdio_port_t   sdio_port;
    bool            is_initialized;
} sdio_backup_parameter_t;
#endif

typedef struct {
    hal_sdio_command53_config_t sdio_command53_config;
#ifdef HAL_DVFS_MODULE_ENABLED
    dvfs_lock_t sd_dvfs_lock;
#endif
}sdio_dma_interrupt_context_t;
sdio_internal_status_t sdio_command52(hal_sdio_port_t sdio_port, hal_sdio_command52_config_t *command52_config);
sdio_internal_status_t sdio_wait_command_ready(hal_sdio_port_t sdio_port);
sdio_internal_status_t sdio_send_command(hal_sdio_port_t sdio_port, uint32_t msdc_command, uint32_t argument);
sdio_internal_status_t sdio_send_command5(hal_sdio_port_t sdio_port, uint32_t ocr);
sdio_internal_status_t sdio_get_rca(hal_sdio_port_t sdio_port);
sdio_internal_status_t sdio_wait_card_not_busy(hal_sdio_port_t sdio_port);
sdio_internal_status_t sdio_wait_data_ready(hal_sdio_port_t sdio_port);
sdio_internal_status_t sdio_check_card_status(hal_sdio_port_t sdio_port);
sdio_internal_status_t sdio_select_card(hal_sdio_port_t sdio_port, uint32_t rca);
sdio_internal_status_t sdio_get_cccr(hal_sdio_port_t sdio_port);
void sdio_get_capacity(hal_sdio_port_t sdio_port);
void sdio_get_power_control(hal_sdio_port_t sdio_port);
void sdio_get_high_speed(hal_sdio_port_t sdio_port);
bool sdio_is_support_lsc(hal_sdio_port_t sdio_port);
bool sdio_is_support_s4mi(hal_sdio_port_t sdio_port);
bool sdio_is_support_4bls(hal_sdio_port_t sdio_port);
bool sdio_is_support_mps(hal_sdio_port_t sdio_port);
bool sdio_is_support_shs(hal_sdio_port_t sdio_port);
sdio_internal_status_t sdio_set_ehs(hal_sdio_port_t sdio_port, bool is_enable);
sdio_internal_status_t sdio_set_mps(hal_sdio_port_t sdio_port, bool is_enable);
sdio_internal_status_t sdio_set_e4mi(hal_sdio_port_t sdio_port, bool is_enable);
sdio_internal_status_t sdio_set_io(hal_sdio_port_t sdio_port,  hal_sdio_function_id_t function, bool is_enable);
sdio_internal_status_t sdio_check_io_ready(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function, bool *is_ready);
sdio_internal_status_t sdio_get_fbr(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function);
sdio_internal_status_t sdio_get_cis(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function);
sdio_internal_status_t sdio_wait_last_block_complete(hal_sdio_port_t sdio_port);
sdio_internal_status_t sdio_stop(hal_sdio_port_t sdio_port);
sdio_internal_status_t sdio_command12_stop(hal_sdio_port_t sdio_port);
void sdio_record_error_status(sdio_internal_status_t sdio_status);
void sdio_isr(void);
void sdio_save_dma_interrupt_context(hal_sdio_port_t sd_port,const sdio_dma_interrupt_context_t *context);
hal_sdio_status_t sdio_wait_dma_interrupt_transfer_ready(hal_sdio_port_t sd_port);

#endif /*HAL_SDIO_MODULE_ENABLED*/

#endif

