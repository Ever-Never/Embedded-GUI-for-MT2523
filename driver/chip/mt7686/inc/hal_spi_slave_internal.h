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

#ifndef __HAL_SPI_SLAVE_INTERNAL_H__
#define __HAL_SPI_SLAVE_INTERNAL_H__

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED

/*define the command value for slave and master communication*/
#define  SLV_CMD_DEFINE0            ((uint32_t)0x08060402)
#define  SLV_CMD_DEFINE1            ((uint32_t)0x0E810C0A)

#define HAL_SPI_SLAVE_IRQ_TABLE SPI_SLV_IRQn
#define HAL_SPI_SLAVE_PRI_TABLE SPI_SLV_IRQ_PRIORITY
#define HAL_SPI_SLAVE_CG_TABLE HAL_CLOCK_CG_SPISLV

#define SPI_SLAVE_BUSY 1
#define SPI_SLAVE_IDLE 0
#define SPI_SLAVE_CHECK_AND_SET_BUSY(spi_port, busy_status)  \
do{ \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    if(g_spi_slave_status[spi_port] == SPI_SLAVE_BUSY){ \
        busy_status = HAL_SPI_SLAVE_STATUS_ERROR_BUSY; \
    } else { \
        g_spi_slave_status[spi_port] = SPI_SLAVE_BUSY;  \
        busy_status = HAL_SPI_SLAVE_STATUS_OK; \
    } \
       restore_interrupt_mask(saved_mask); \
}while(0)

#define SPI_SLAVE_SET_IDLE(spi_port)   \
do{  \
       g_spi_slave_status[spi_port] = SPI_SLAVE_IDLE;  \
}while(0)

/* SPI SLAVE FSM STATUS */
typedef enum {
    PWROFF_STA,
    PWRON_STA,
    CR_STA,
    CW_STA,
    MAX_STATUS
} spi_slave_fsm_status_t;

/* SPI SLAVE OPERATION COMMAND*/
typedef enum {
    POWER_OFF_CMD,
    POWER_ON_CMD,
    CONFIG_READ_CMD,
    READ_CMD,
    CONFIG_WRITE_CMD,
    WRITE_CMD,
    MAX_OPERATION_CMD
} spi_slave_operation_cmd_t;

/* SPI SLAVE FSM STATUS UPDATE */
#define spi_slave_update_status(now_status)      \
    do {                                         \
        g_last2now_status[0] = g_last2now_status[1]; \
        g_last2now_status[1] = now_status;         \
    } while(0)

/* function prototype */
void spi_slave_lisr(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data);
void spi_slave_init(hal_spi_slave_port_t spi_port, const hal_spi_slave_config_t *spi_config);
hal_spi_slave_status_t spi_slave_send(hal_spi_slave_port_t spi_port, const uint8_t *data, uint32_t size);
hal_spi_slave_status_t spi_slave_receive(hal_spi_slave_port_t spi_port, uint8_t *buffer, uint32_t size);
hal_spi_slave_status_t spi_slave_query_config_info(hal_spi_slave_port_t spi_port, uint32_t *address, uint32_t *length);
void spi_slave_set_early_miso(hal_spi_slave_port_t spi_port, hal_spi_slave_early_miso_t early_miso);
void spi_slave_set_command(hal_spi_slave_port_t spi_port, hal_spi_slave_command_type_t command, uint8_t value);
void spi_slave_reset_default(hal_spi_slave_port_t spi_port);
#ifdef HAL_SLEEP_MANAGER_ENABLED
void spi_slave_backup_register_callback(void *data);
void spi_slave_restore_register_callback(void *data);
#endif

#endif   /* HAL_SPI_SLAVE_MODULE_ENABLED */

#endif   /*__HAL_SPI_SLAVE_INTERNAL_H__*/

