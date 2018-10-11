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

#include "hal_msdc.h"
#include "hal_sd.h"
#include "hal_sd_internal.h"

#ifdef HAL_SD_MODULE_ENABLED
#include "hal.h"
#include "hal_pinmux_define.h"
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_pmu_internal.h"
#include "assert.h"

#define CLK_CM4_FREQ_26M ((uint32_t) 26000000)

sd_information_t sd_information;
static uint32_t mmc_extended_csd[128];
uint32_t sd_csd[4];
static uint32_t sd_read_buffer[128];

extern const msdc_clock_t msdc_output_clock[7];

extern msdc_config_t msdc_config[];
static sd_dma_interrupt_context_t sd_dma_interrupt_context[MSDC_PORT_MAX];
static uint32_t sd_dma_interrupt_gpt_timer;
#ifdef HAL_SLEEP_MANAGER_ENABLED
static volatile sd_backup_parameter_t sd_backup_parameter;
static volatile msdc_register_t sd_backup_register;
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
extern bool dvfs_1p1v_need_sd_clock_tuning;
extern bool dvfs_1p3v_need_sd_clock_tuning;
extern uint32_t dvfs_save_sd_init_clock;
#endif
extern uint32_t SystemCoreClock;
extern void SystemCoreClockUpdate(void);

sd_internal_status_t sd_wait_data_ready(hal_sd_port_t sd_port)
{
    msdc_register_t *msdc_register_base;
    uint32_t data_status;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);
    do {
        data_status = msdc_register_base->SDC_DATSTA;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
        hal_gpt_get_duration_count(start_count, end_count, &count);
    } while ((!data_status) && msdc_card_is_present((msdc_port_t)sd_port) && (count < MSDC_TIMEOUT_PERIOD_DATA));

    /*clear msdc interrupt status*/
    msdc_register_base->MSDC_INT;

    /*record command status*/
    msdc_record_data_status((msdc_port_t)sd_port, data_status);

    if (count >= MSDC_TIMEOUT_PERIOD_DATA) {
        log_hal_error("wait sd data ready timeout.\r\n");
        return ERROR_DATA_TIMEOUT;
    }

    if (data_status & SDC_DATSTA_DATTO_MASK) {
        return ERROR_DATA_TIMEOUT;
    } else if (data_status & SDC_DATSTA_DATCRCERR_MASK) {
        return ERROR_DATA_CRC_ERROR;
    } else {
        return NO_ERROR;
    }
}

sd_internal_status_t sd_wait_command_ready(hal_sd_port_t sd_port)
{
    msdc_register_t *msdc_register_base;
    volatile uint32_t command_status;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);
    do {
        command_status = msdc_register_base->SDC_CMDSTA;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
        hal_gpt_get_duration_count(start_count, end_count, &count);
    } while (!command_status && msdc_card_is_present((msdc_port_t)sd_port) && (count < MSDC_TIMEOUT_PERIOD_COMMAND));

    /*clear msdc interrupt status*/
    msdc_register_base->MSDC_INT;

    /*record command status*/
    msdc_record_command_status((msdc_port_t)sd_port, command_status);

    if (count >= MSDC_TIMEOUT_PERIOD_COMMAND) {
        log_hal_error("wait sd command ready timeout.\r\n");
        return ERROR_COMMAND_TIMEOUT;
    }

    if (command_status & SDC_CMDSTA_CMDTO_MASK) {
        return ERROR_COMMAND_TIMEOUT;
    } else if (command_status & SDC_CMDSTA_RSPCRCERR_MASK) {
        return ERROR_COMMAND_CRC_ERROR;
    } else {
        return NO_ERROR;
    }
}


sd_internal_status_t sd_send_command(hal_sd_port_t sd_port, uint32_t msdc_command, uint32_t argument)
{
    msdc_register_t *msdc_register_base;
    sd_internal_status_t status;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    if (MSDC_COMMAND12 != msdc_command) {
        while ((msdc_register_base->SDC_STA & SDC_STA_BESDCBUSY_MASK) && msdc_card_is_present((msdc_port_t)sd_port));
    } else {
        while ((msdc_register_base->SDC_STA & SDC_STA_BECMDBUSY_MASK) && msdc_card_is_present((msdc_port_t)sd_port));
    }

    /*clear msdc interrupt status*/
    msdc_register_base->MSDC_INT;

    msdc_register_base->SDC_ARG = argument;
    msdc_register_base->SDC_CMD = msdc_command;

    status = sd_wait_command_ready(sd_port);
    if (status != NO_ERROR) {
        return status;
    }

    return NO_ERROR;
}

sd_internal_status_t sd_send_command55(hal_sd_port_t sd_port, uint32_t argument)
{
    sd_internal_status_t status;
    msdc_register_t *msdc_register_base;
    uint32_t response;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;
    status = sd_send_command(sd_port, MSDC_COMMAND55, (argument << 16));
    if (NO_ERROR != status) {
        return status;
    }

    response = msdc_register_base->SDC_RESP0;

    if (!(response & SD_CARD_STATUS_ERROR_MASK)) {
        if (!(response & SD_CARD_STATUS_APP_COMMAND_MASK)) {
            return ERROR_APPCOMMAND_FAILED;
        } else {
            return NO_ERROR;
        }
    } else if (response & SD_CARD_STATUS_LOCKED) {
        return ERROR_CARD_IS_LOCKED;
    } else {
        return ERROR_STATUS;
    }
}

sd_internal_status_t sd_reset(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;

    status = sd_send_command(sd_port, MSDC_COMMAND0, COMMAND_NO_ARGUMENT);
    sd_information.sd_state = IDLE_STA;

    return status;
}

hal_sd_card_type_t sd_check_sd_card_type(hal_sd_port_t sd_port)
{
    msdc_register_t *msdc_register_base;
    sd_internal_status_t status;
    uint32_t response = 0;
    uint32_t ocr_argument = 0;
    uint32_t ocr_value = 0;
    bool is_first_command41 = MSDC_TRUE;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    status = sd_send_command(sd_port, MSDC_COMMAND8, COMMAND8_ARGUMENT);
    if (NO_ERROR != status) {
        sd_reset(sd_port);
        sd_information.command8_response = SD_COMMAND8_RESPONSE_NO_RESPONSE;
    } else {
        response = msdc_register_base->SDC_RESP0;
        if (COMMAND8_ARGUMENT == response) {
            sd_information.command8_response = SD_COMMAND8_RESPONSE_VALID;
        } else {
            sd_information.command8_response = SD_COMMAND8_RESPONSE_INVALID;
        }
    }

    if (SD_COMMAND8_RESPONSE_NO_RESPONSE == sd_information.command8_response) {
        ocr_argument = SD_OCR_VOLTAGE_ARGUMENT;
    } else if (SD_COMMAND8_RESPONSE_VALID == sd_information.command8_response) {
        ocr_argument = SD_OCR_VOLTAGE_ARGUMENT | SD_OCR_CAPACITY_STATUS;
    } else if (SD_COMMAND8_RESPONSE_INVALID == sd_information.command8_response) {
        sd_information.error_status = ERROR_COMMAND8_INVALID;
        return HAL_SD_TYPE_UNKNOWN_CARD;
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);

    while (1) {
        /*wait MSDC not busy*/
        while (msdc_register_base->MSDC_STA & MSDC_STA_BUSY_MASK);

        status = sd_send_command55(sd_port, SD_RCA_DEFAULT);
        if (NO_ERROR != status) {
            sd_information.error_status = status;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        /*wait MSDC not busy*/
        while (msdc_register_base->MSDC_STA & MSDC_STA_BUSY_MASK);

        if (is_first_command41) {
            status = sd_send_command(sd_port, MSDC_ACOMMAND41, COMMAND_NO_ARGUMENT);
            is_first_command41 = MSDC_FALSE;
        } else {
            status = sd_send_command(sd_port, MSDC_ACOMMAND41, ocr_argument);
        }

        if (NO_ERROR != status) {
            sd_information.error_status = status;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        ocr_value = msdc_register_base->SDC_RESP0;
        if (!(ocr_value & SD_OCR_VOLTAGE_ARGUMENT)) {
            sd_information.error_status = ERROR_OCR_NOT_SUPPORT;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        if (MSDC_FALSE == msdc_card_is_present((msdc_port_t)sd_port)) {
            sd_information.error_status = ERROR_CARD_NOT_PRESENT;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        if ((ocr_value & SD_OCR_BUSY_STATUS)) {
            break;
        } else {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
            hal_gpt_get_duration_count(start_count, end_count, &count);
            if (count >= MSDC_TIMEOUT_PERIOD_INIT) {
                sd_information.error_status = ERROR_R3_OCR_BUSY;
                return HAL_SD_TYPE_UNKNOWN_CARD;
            }
        }

    }

    sd_information.is_inactive = MSDC_FALSE;
    sd_information.sd_ocr = ocr_value;

    if (ocr_value & SD_OCR_CAPACITY_STATUS) {
        sd_information.card_type = HAL_SD_TYPE_SD20_HCS_CARD;
    } else if (SD_COMMAND8_RESPONSE_VALID == sd_information.command8_response) {
        sd_information.card_type = HAL_SD_TYPE_SD20_LCS_CARD;
    } else {
        sd_information.card_type = HAL_SD_TYPE_SD_CARD;
    }

    sd_information.sd_state = READY_STA;

    return (sd_information.card_type);
}


hal_sd_card_type_t sd_check_mmc_card_type(hal_sd_port_t sd_port)
{
    msdc_register_t *msdc_register_base;
    sd_internal_status_t status;
    uint32_t ocr_argument = 0;
    uint32_t ocr_value = 0;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;


    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    if (SD_COMMAND8_RESPONSE_INVALID == sd_information.command8_response) {
        sd_information.error_status = ERROR_COMMAND8_INVALID;
        return HAL_SD_TYPE_UNKNOWN_CARD;
    } else {
        ocr_argument = SD_OCR_VOLTAGE_ARGUMENT | MMC_HIGH_DESITY_CHECK_BIT;
    }

    if (ERROR_R3_OCR_BUSY == sd_information.error_status) {
        return HAL_SD_TYPE_UNKNOWN_CARD;
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);

    while (1) {
        /*wait MSDC not busy*/
        while (msdc_register_base->MSDC_STA & MSDC_STA_BUSY_MASK);

        status = sd_send_command(sd_port, MSDC_COMMAND1_MMC, ocr_argument);
        if (NO_ERROR != status) {
            sd_information.error_status = status;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        ocr_value = msdc_register_base->SDC_RESP0;

        if (!(ocr_value & SD_OCR_VOLTAGE_ARGUMENT)) {
            sd_information.error_status = ERROR_OCR_NOT_SUPPORT;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        if (MSDC_FALSE == msdc_card_is_present((msdc_port_t)sd_port)) {
            sd_information.error_status = ERROR_CARD_NOT_PRESENT;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        if ((ocr_value & SD_OCR_BUSY_STATUS)) {
            break;
        } else {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
            hal_gpt_get_duration_count(start_count, end_count, &count);
            if (count >= MSDC_TIMEOUT_PERIOD_INIT) {
                sd_information.error_status = ERROR_R3_OCR_BUSY;
                return HAL_SD_TYPE_UNKNOWN_CARD;
            }
        }
    }

    sd_information.is_inactive = MSDC_FALSE;
    sd_information.sd_ocr = ocr_value;

    if ((ocr_value & MMC_HIGH_DESITY_CHECK_MASK) == MMC_HIGH_DESITY_CHECK_BIT) {
        sd_information.card_type = HAL_SD_TYPE_MMC42_CARD;
    } else {
        sd_information.card_type = HAL_SD_TYPE_MMC_CARD;
    }

    sd_information.sd_state = READY_STA;

    return (sd_information.card_type);
}

hal_sd_card_type_t sd_check_card_type(hal_sd_port_t sd_port)
{
    hal_sd_card_type_t card_type;

    if ((card_type = sd_check_sd_card_type(sd_port)) != HAL_SD_TYPE_UNKNOWN_CARD) {
        return card_type;
    } else if ((card_type = sd_check_mmc_card_type(sd_port)) != HAL_SD_TYPE_UNKNOWN_CARD) {
        return card_type;
    } else {
        return HAL_SD_TYPE_UNKNOWN_CARD;
    }
}

sd_internal_status_t sd_get_card_id(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    status = sd_send_command(sd_port, MSDC_COMMAND2, COMMAND_NO_ARGUMENT);
    if (NO_ERROR != status) {
        return status;
    }

    sd_information.cid[0] = msdc_register_base->SDC_RESP0;
    sd_information.cid[1] = msdc_register_base->SDC_RESP1;
    sd_information.cid[2] = msdc_register_base->SDC_RESP2;
    sd_information.cid[3] = msdc_register_base->SDC_RESP3;

    sd_information.sd_state = IDENT_STA;

    return NO_ERROR;
}

sd_internal_status_t sd_get_rca(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;
    msdc_register_t *msdc_register_base;
    uint32_t response = 0;
    sd_state_t state;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    if (sd_information.card_type & (HAL_SD_TYPE_SD_CARD | HAL_SD_TYPE_SD20_HCS_CARD | HAL_SD_TYPE_SD20_LCS_CARD)) {
        /*sd card*/
        status = sd_send_command(sd_port, MSDC_COMMAND3, COMMAND_NO_ARGUMENT);
        if (NO_ERROR != status) {
            return status;
        }

        response = msdc_register_base->SDC_RESP0;
        sd_information.rca = response >> COMMAND_RCA_ARGUMENT_SHIFT;
    } else {
        /*emmc card*/
        status = sd_send_command(sd_port, MSDC_COMMAND3_MMC, COMMAND_MMC_RCA_ARGUMENT << COMMAND_RCA_ARGUMENT_SHIFT);
        if (NO_ERROR != status) {
            return status;
        }

        response = msdc_register_base->SDC_RESP0;

        status = sd_send_command(sd_port, MSDC_COMMAND13, COMMAND_MMC_RCA_ARGUMENT << COMMAND_RCA_ARGUMENT_SHIFT);
        if (NO_ERROR != status) {
            return status;
        }

        response = msdc_register_base->SDC_RESP0;
        state = (sd_state_t)((response & SD_CARD_STATUS_STATE_BIT_MASK) >> SD_CARD_STATUS_STATE_BIT_SHIFT);

        if (STBY_STA != state) {
            return ERROR_RCA_FAILED;
        }

        sd_information.rca = COMMAND_MMC_RCA_ARGUMENT;
    }

    sd_information.sd_state = STBY_STA;

    return NO_ERROR;
}

sd_internal_status_t sd_acommand42(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;

    status = sd_send_command55(sd_port, sd_information.rca);
    if (status != NO_ERROR) {
        return status;
    }

    /*disconnect the 50K Ohm pull-up resistor on CD/DAT3*/
    status = sd_send_command(sd_port, MSDC_ACOMMAND42, MSDC_FALSE);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_check_card_status(sd_port);
    if (status != NO_ERROR) {
        return status;
    }

    return status;
}


void sd_get_bit_field_value(uint8_t *destination, uint8_t *source, uint16_t start_bit, uint16_t width)
{
    uint16_t i;
    uint16_t bytes, bits;

    memset(destination, 0, (width >> 8) + (0 == (width & 0x07) ? 0 : 1));
    for (i = 0; i < width; i++) {
        bytes = (start_bit + i) >> 3;
        bits  = (start_bit + i) & 0x07;
        *(destination + (i >> 3)) |= (uint8_t)(((*(source + bytes) >> bits) & 1) << (i & 0x07));
    }
}

void sd_analysis_csd(hal_sd_port_t sd_port, uint32_t *csd)
{
    uint8_t *csd_pointer;
    uint32_t c_size_mult;

    csd_pointer = (uint8_t *)csd;

    sd_get_bit_field_value((uint8_t *)&sd_information.csd.csd_structure, csd_pointer, 126, 2);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.tacc, csd_pointer, 112, 8);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.nsac, csd_pointer, 104, 8);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.tran_speed, csd_pointer, 96, 8);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.ccc, csd_pointer, 84, 12);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.read_bl_len, csd_pointer, 80, 4);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.read_bl_partial, csd_pointer, 79, 1);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.write_blk_misalign, csd_pointer, 78, 1);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.read_blk_misalign, csd_pointer, 77, 1);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.dsr_imp, csd_pointer, 76, 1);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.write_bl_len, csd_pointer, 22, 4);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.write_bl_partial, csd_pointer, 21, 1);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.wp_grp_enable, csd_pointer, 31, 1);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.perm_write_protect, csd_pointer, 13, 1);
    sd_get_bit_field_value((uint8_t *)&sd_information.csd.tmp_write_protect, csd_pointer, 12, 1);

    sd_information.csd.read_bl_len = 1 << sd_information.csd.read_bl_len;
    sd_information.csd.write_bl_len = 1 << sd_information.csd.write_bl_len;

    if (sd_information.csd.tmp_write_protect || sd_information.csd.perm_write_protect) {
        sd_information.is_write_protection = MSDC_TRUE;
    }

    /*there is some difference of CSD between SD and eMMC */
    if (sd_information.card_type == HAL_SD_TYPE_MMC_CARD || sd_information.card_type == HAL_SD_TYPE_MMC42_CARD) {
        sd_get_bit_field_value((uint8_t *)&sd_information.csd.spec_vers, csd_pointer, 122, 4);
        sd_get_bit_field_value((uint8_t *)&sd_information.csd.erase_sec_size_mmc, csd_pointer, 42, 5);
        sd_get_bit_field_value((uint8_t *)&sd_information.csd.erase_grp_size_mmc, csd_pointer, 37, 5);
        sd_get_bit_field_value((uint8_t *)&sd_information.csd.wp_grp_size_mmc, csd_pointer, 32, 5);

        sd_information.csd.erase_sec_size_mmc = (sd_information.csd.erase_sec_size_mmc + 1) * sd_information.csd.write_bl_len;
        sd_information.csd.erase_grp_size_mmc = (sd_information.csd.erase_grp_size_mmc + 1) * sd_information.csd.erase_sec_size_mmc;
        sd_information.csd.wp_grp_size_mmc = (sd_information.csd.wp_grp_size_mmc + 1) * sd_information.csd.erase_grp_size_mmc;
    } else {
        sd_get_bit_field_value((uint8_t *)&sd_information.csd.sector_size, csd_pointer, 39, 7);
        sd_get_bit_field_value((uint8_t *)&sd_information.csd.wp_grg_size, csd_pointer, 32, 7);
        sd_get_bit_field_value((uint8_t *)&sd_information.csd.erase_blk_en, csd_pointer, 46, 1);

        sd_information.csd.sector_size = sd_information.csd.sector_size + 1;
        sd_information.csd.wp_grg_size = (sd_information.csd.wp_grg_size + 1) * sd_information.csd.sector_size;
    }

    if (sd_information.card_type == HAL_SD_TYPE_SD20_HCS_CARD && sd_information.csd.csd_structure >= CSD_VERSION_2_0) {
        sd_get_bit_field_value((uint8_t *)&sd_information.csd.c_size, csd_pointer, 48, 22);

        /*in SD2.0, capacity = (C_SIZE + 1) * 512Kbyte*/
        sd_information.csd.capacity = (uint64_t)(sd_information.csd.c_size + 1) * 512 * 1024;
    } else {
        sd_get_bit_field_value((uint8_t *)&sd_information.csd.c_size, csd_pointer, 62, 12);
        sd_get_bit_field_value((uint8_t *)&c_size_mult, csd_pointer, 47, 3);

        /*in SD1.0, capacity = (C_SIZE + 1) * (1 << C_SIZE_MULT) * (READ_BL_LEN)*/
        sd_information.csd.capacity = (uint64_t)(sd_information.csd.c_size + 1) * (1 << (c_size_mult + 2)) * sd_information.csd.read_bl_len;
    }
}


sd_internal_status_t sd_get_csd(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    status = sd_send_command(sd_port, MSDC_COMMAND9, (uint32_t)sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
    if (NO_ERROR != status) {
        return status;
    }

    sd_csd[0] = msdc_register_base->SDC_RESP0;
    sd_csd[1] = msdc_register_base->SDC_RESP1;
    sd_csd[2] = msdc_register_base->SDC_RESP2;
    sd_csd[3] = msdc_register_base->SDC_RESP3;

    sd_analysis_csd(sd_port, sd_csd);

    return NO_ERROR;
}


sd_internal_status_t sd_set_dsr(hal_sd_port_t sd_port)
{
    return  sd_send_command(sd_port, MSDC_COMMAND4, COMMAND_DCR_ARGUMENT << 16);
}

sd_internal_status_t sd_wait_card_not_busy(hal_sd_port_t sd_port)
{
    msdc_register_t *msdc_register_base;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_count);
    while (SD_IS_R1B_BUSY && msdc_card_is_present((msdc_port_t)sd_port)) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_count);
        hal_gpt_get_duration_count(start_count,end_count,&count);
        if (count >= MSDC_TIMEOUT_PERIOD_DATA) {
             log_hal_error("wait sd r1b error!\r\n");
             return ERROR_STATUS;
        }
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_count);
    while (msdc_register_base->SDC_STA & SDC_STA_BESDCBUSY_MASK) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_count);
        hal_gpt_get_duration_count(start_count,end_count,&count);
        if (count >= MSDC_TIMEOUT_PERIOD_DATA) {
             log_hal_error("wait sd besdcbusy error!\r\n");
             return ERROR_STATUS;
        }
    }

    /*clear msdc interrupt status*/
    msdc_register_base->MSDC_INT;

    return NO_ERROR;
}

sd_internal_status_t sd_check_card_status(hal_sd_port_t sd_port)
{
    msdc_register_t *msdc_register_base;
    uint32_t response;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    response = msdc_register_base->SDC_RESP0;

    if (!(response & SD_CARD_STATUS_ERROR_MASK)) {
        return NO_ERROR;
    } else if (response & SD_CARD_STATUS_LOCKED) {
        return ERROR_CARD_IS_LOCKED;
    } else {
        return ERROR_STATUS;
    }
}

sd_internal_status_t sd_select_card(hal_sd_port_t sd_port, uint32_t rca)
{
    sd_internal_status_t status;

    status = sd_send_command(sd_port, MSDC_COMMAND7, rca << COMMAND_RCA_ARGUMENT_SHIFT);
    if (status !=  NO_ERROR) {
        return status;
    }

    status = sd_wait_card_not_busy(sd_port);
    if (status !=  NO_ERROR) {
        return status;
    }

    status = sd_check_card_status(sd_port);
    if (status !=  NO_ERROR) {
        return status;
    }

    return NO_ERROR;
}

sd_internal_status_t sd_set_block_length(hal_sd_port_t sd_port, uint32_t block_length)
{
    sd_internal_status_t status;
    msdc_register_t *msdc_register_base;


    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;
    if (block_length > MSDC_MAX_BLOCK_LENGTH) {
        return ERROR_INVALID_BLOCK_LENGTH;
    }

    status = sd_send_command(sd_port, MSDC_COMMAND16, block_length);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_check_card_status(sd_port);
    if (status != NO_ERROR) {
        return status;
    }

    sd_information.block_length = block_length;

    msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_BLKEN_MASK)) | (block_length);

    return NO_ERROR;
}

sd_internal_status_t sd_analysis_scr(hal_sd_port_t sd_port, uint32_t *scr)
{
    uint8_t *scr_pointer;

    scr_pointer = (uint8_t *)scr;

    sd_get_bit_field_value((uint8_t *)(&sd_information.scr.spec_ver), scr_pointer, 0, 4);
    sd_get_bit_field_value(&sd_information.scr.dat_after_erase, (scr_pointer + 1), 7, 1);
    sd_get_bit_field_value(&sd_information.scr.security, (scr_pointer + 1), 4, 3);
    sd_get_bit_field_value(&sd_information.scr.bus_width, (scr_pointer + 1), 0, 4);

    return NO_ERROR;
}



sd_internal_status_t mmc_switch(hal_sd_port_t sd_port, uint32_t argument)
{
    sd_internal_status_t status;
    uint32_t response;
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;
    status = sd_send_command(sd_port, MSDC_COMMAND6_MMC, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    status = sd_wait_card_not_busy(sd_port);
    if (status != NO_ERROR) {
        goto error;
    }

    /*read card status*/
    status = sd_send_command(sd_port, MSDC_COMMAND13, sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
    if (NO_ERROR != status) {
        goto error;
    }

    status = sd_check_card_status(sd_port);
    if (NO_ERROR != status) {
        goto error;
    }

    response = msdc_register_base->SDC_RESP0;
    if (response & MMC_SWITCH_ERROR_BIT_MASK) {
        status = ERROR_MMC_SWITCH_ERROR;
        goto error;
    }


    status = sd_set_block_length(sd_port, SD_BLOCK_LENGTH);
    if (NO_ERROR != status) {
        return status;
    }

    /*some ext cst bit filed has changed,read again*/
    status = mmc_get_extended_csd(sd_port);
    if (NO_ERROR != status) {
        return status;
    }

    msdc_reset((msdc_port_t)sd_port);
    return NO_ERROR;

error:
    msdc_reset((msdc_port_t)sd_port);
    return status;
}

sd_internal_status_t sd_get_scr(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;
    uint32_t i = 0;
    uint32_t scr[2];
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;
    status = sd_set_block_length(sd_port, SD_SCR_LENGTH);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_send_command55(sd_port, sd_information.rca);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_send_command(sd_port, MSDC_ACOMMAND51, COMMAND_NO_ARGUMENT);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_check_card_status(sd_port);
    if (status != NO_ERROR) {
        return status;
    }

    while (i < 2) {
        if (msdc_register_base->MSDC_STA & MSDC_STA_DRQ_MASK) {
            *(uint32_t *)(scr + i) = msdc_register_base->MSDC_DAT;
            i++;
        }
    }

    status =  sd_wait_data_ready(sd_port);
    if (status != NO_ERROR) {
        return status;
    }

    sd_analysis_scr(sd_port, scr);

    msdc_reset((msdc_port_t)sd_port);

    return status;
}

void sd_analysis_command6_switch_status(sd_command6_switch_status_t *command6_switch_status, uint8_t *buffer)
{
    command6_switch_status->max_current = (((*(uint8_t *)buffer) << 8) | (*(uint8_t *)(buffer + 1)));
    command6_switch_status->group1_info = (((*(uint8_t *)(buffer + 12)) << 8) | (*(uint8_t *)(buffer + 13)));
    command6_switch_status->group1_result = ((*(uint8_t *)(buffer + 16)) & 0xf);
    command6_switch_status->structure_version = (*(uint8_t *)(buffer + 17));
    command6_switch_status->group1_status = (((*(uint8_t *)(buffer + 28)) << 8) | (*(uint8_t *)(buffer + 29)));
}



sd_internal_status_t sd_query_switch_high_speed(hal_sd_port_t sd_port, uint32_t argument)
{
    sd_internal_status_t status;
    uint32_t command6_response[COMMAND6_RESPONSE_LENGTH >> 2];
    uint32_t index = 0;
    uint32_t read_word_count = 0;
    sd_command6_switch_status_t switch_status;
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~(SDC_CFG_BLKEN_MASK))) | COMMAND6_RESPONSE_LENGTH;

    msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_FIFOTHD_MASK)) |
                                   (MSDC_FIFO_THRESHOLD_1  << MSDC_CFG_FIFOTHD_OFFSET);

    msdc_register_base->MSDC_IOCON = (msdc_register_base->MSDC_IOCON & (~MSDC_IOCON_DMABURST_MASK)) |
                                     (MSDC_DMA_SINGLE_MODE << MSDC_IOCON_DMABURST_OFFSET);

    status = sd_send_command(sd_port, MSDC_COMMAND6, argument);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_check_card_status(sd_port);
    if (status != NO_ERROR) {
        return status;
    }

    /*set CMD6 read response length*/
    read_word_count = COMMAND6_RESPONSE_LENGTH >> 2;
    while (index < read_word_count) {
        if (msdc_register_base->MSDC_STA & MSDC_STA_DRQ_MASK) {
            *(command6_response + index) = msdc_register_base->MSDC_DAT;
            index++;
        }
    }

    /*analysis command6 switch status*/
    sd_analysis_command6_switch_status(&switch_status, (uint8_t *)command6_response);

    if (0 == switch_status.max_current) {
        return ERROR_HIGH_SPEED_CONSUMPTION;
    }
    if ((1 == switch_status.group1_result) && (0 == (switch_status.group1_status & (1 << 1)))) {
        /*the high speed function can be switched*/
        return NO_ERROR;
    }
    if ((0xF == switch_status.group1_result) || (!(switch_status.group1_info & (1 << 1)))) {
        return ERROR_HIGH_SPEED_NOT_SUPPORT;
    }
    if (switch_status.group1_status & (1 << 1)) {
        return ERROR_HIGH_SPEED_BUSY;
    }

    return ERROR_HIGH_SPEED_COMMON_ERROR;
}


sd_internal_status_t sd_select_high_speed(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;
    uint32_t i = 1000;

    do {
        status = sd_query_switch_high_speed(sd_port, SD_COMMAND6_QUERY_HIGH_SPEED);
        if (NO_ERROR == status) {
            status = sd_query_switch_high_speed(sd_port, SD_COMMAND6_SELECT_HIGH_SPEED);
            if (NO_ERROR == status) {
                sd_information.is_high_speed = MSDC_TRUE;
                break;
            } else if (ERROR_HIGH_SPEED_BUSY == status) {
                break;
            }
        } else if (ERROR_HIGH_SPEED_BUSY == status) {
            break;
        }
    } while (i--);

    return status;
}


sd_internal_status_t mmc_get_extended_csd(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;
    uint32_t index = 0;
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;
    status = sd_send_command(sd_port, MSDC_COMMAND8_MMC, COMMAND_NO_ARGUMENT);
    if (NO_ERROR != status) {
        goto error;
    }

    status = sd_check_card_status(sd_port);
    if (NO_ERROR != status) {
        goto error;
    }

    while (index < 128) {
        if (msdc_register_base->MSDC_STA & MSDC_STA_FIFICNT_MASK) {
            *(mmc_extended_csd + index) = msdc_register_base->MSDC_DAT;
            index ++;
        }
    }

    status = sd_wait_data_ready(sd_port);
    if (NO_ERROR != status) {
        goto error;
    }

    msdc_reset((msdc_port_t)sd_port);

    sd_information.csd.ext_csd = (extended_csd_information_t *)mmc_extended_csd;

    return NO_ERROR;

error:
    msdc_reset((msdc_port_t)sd_port);
    return status;
}


void mmc_is_version_44(void)
{
    if (sd_information.csd.ext_csd->ext_csd_rev > MMC_EXTENDED_CSD_VERSION_43) {
        if (0 != sd_information.csd.ext_csd->boot_size_mul) {
            sd_information.emmc_information.is_mmc_version_44 = MSDC_TRUE;
        } else {
            sd_information.emmc_information.is_mmc_version_44 = MSDC_FALSE;
        }
    } else {
        sd_information.emmc_information.is_mmc_version_44 = MSDC_FALSE;
    }
}


sd_internal_status_t mmc_set_high_speed(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;

    status = sd_set_block_length(sd_port, SD_BLOCK_LENGTH);
    if (NO_ERROR != status) {
        goto error;
    }

    status = mmc_get_extended_csd(sd_port);
    if (NO_ERROR != status) {
        goto error;
    }

    if (sd_information.card_type == HAL_SD_TYPE_MMC42_CARD) {
        sd_information.csd.capacity = SD_BLOCK_LENGTH * sd_information.csd.ext_csd->sec_count;
    }

    mmc_is_version_44();

    /*set bus clock*/
    if (sd_information.csd.ext_csd->card_type & MMC_HS_52M) {
        sd_information.is_high_speed = MSDC_TRUE;
    } else if (sd_information.csd.ext_csd->card_type & MMC_HS_26M) {
        sd_information.is_high_speed = MSDC_FALSE;
    }

    return NO_ERROR;

error:
    return status;
}


sd_internal_status_t sd_stop_transfer(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;
    uint32_t retry = 0;

    while (30 > retry) {
        status = sd_send_command(sd_port, MSDC_COMMAND12, COMMAND_NO_ARGUMENT);
        if (status != NO_ERROR) {
            retry++;
        } else {
            break;
        }
    }

    if (30 <= retry) {
        return status;
    }

    status = sd_wait_card_not_busy(sd_port);
    if (NO_ERROR != status) {
        return status;
    }

    return NO_ERROR;
}
sd_internal_status_t sd_get_card_status(hal_sd_port_t sd_port, uint32_t *card_status)
{
    sd_internal_status_t status;
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;
    status = sd_send_command(sd_port, MSDC_COMMAND13, sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
    if (NO_ERROR != status) {
        return status;
    }

    *card_status = msdc_register_base->SDC_RESP0;

    return NO_ERROR;
}


sd_internal_status_t sd_send_erase_command(hal_sd_port_t sd_port, uint32_t command, uint32_t address)
{
    sd_internal_status_t status;
    uint32_t card_status;

    if (MSDC_COMMAND38 != command) {
        status = sd_send_command(sd_port, command, address);
        if (NO_ERROR != status) {
            return status;
        }
    } else {
        status = sd_send_command(sd_port, command, COMMAND_NO_ARGUMENT);
        if (NO_ERROR != status) {
            return status;
        }
    }

    status = sd_check_card_status(sd_port);
    if (NO_ERROR != status) {
        return status;
    }

    if (MSDC_COMMAND38 == command) {
        status = sd_wait_card_not_busy(sd_port);
        if (NO_ERROR != status) {
            return status;
        }

        do {
            status = sd_get_card_status(sd_port, &card_status);
            if (NO_ERROR != status) {
                return status;
            }
        } while (((card_status & SD_CARD_STATUS_STATE_BIT_MASK) >> SD_CARD_STATUS_STATE_BIT_SHIFT) != TRAN_STA);
    }

    return NO_ERROR;
}

sd_internal_status_t sd_wait_last_block_complete(hal_sd_port_t sd_port)
{
    uint32_t value;

    if (sd_port == HAL_SD_PORT_0) {
        *(volatile uint32_t *)(MSDC0_BASE + 0x90) = 3;
    } else {
        *(volatile uint32_t *)(MSDC1_BASE + 0x90) = 3;
    }

    do {
        if (sd_port == HAL_SD_PORT_0) {
            value = *(volatile uint32_t *)(MSDC0_BASE + 0X94);
        } else {
            value = *(volatile uint32_t *)(MSDC1_BASE + 0X94);
        }
        if (0x01800000 == (value & 0x01f00000)) {
            break;
        }

    } while (1);

    return NO_ERROR;
}


sd_internal_status_t sd_latch_read_blocks(hal_sd_port_t sd_port, uint32_t *read_buffer,  uint32_t read_address, uint32_t block_number)
{
    sd_internal_status_t status;
    uint32_t read_command;
    uint32_t index = 0;
    uint64_t read_word_count = 0;
    uint32_t response;
    uint32_t block_multiplier;
    msdc_register_t *msdc_register_base;
    uint32_t data_status;

    if (NULL == read_buffer) {
        log_hal_error("parameter error\r\n");
        return ERROR_ERRORS;
    }

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_FIFOTHD_MASK)) |
                                   (MSDC_FIFO_THRESHOLD_1  << MSDC_CFG_FIFOTHD_OFFSET);

    msdc_register_base->MSDC_IOCON = (msdc_register_base->MSDC_IOCON & (~MSDC_IOCON_DMABURST_MASK)) |
                                     (MSDC_DMA_SINGLE_MODE << MSDC_IOCON_DMABURST_OFFSET);

    /*read clear to make sure we will read latest one in the future*/
    msdc_register_base->SDC_DATSTA;


    /*data address is in byte units in a SD1.0 memory card and in block(512byte)  units in a High Capacity SD memory card.
      for eMMC, data address for densities =< 2GB is a 32bit byte address and data address densities > 2GB is a 32bit erase group size address.*/
    if ((sd_information.card_type == HAL_SD_TYPE_SD20_HCS_CARD) || (sd_information.card_type == HAL_SD_TYPE_MMC42_CARD)) {
        block_multiplier = 1;
    } else if (sd_information.card_type == HAL_SD_TYPE_MMC_CARD) {
        /*for emmc densities =< 2GB, access mode is byte mode*/
        block_multiplier = sd_information.csd.read_bl_len;
    } else {
        block_multiplier = SD_BLOCK_LENGTH;
    }

    read_command = (block_number > 1) ? MSDC_COMMAND18 : MSDC_COMMAND17;
    status = sd_send_command(sd_port, read_command, (read_address * block_multiplier));
    if (status != NO_ERROR) {
        goto error;
    }

    status = sd_check_card_status(sd_port);
    if (status != NO_ERROR) {
        goto error;
    }

    read_word_count = (uint64_t)(((uint64_t)block_number * SD_BLOCK_LENGTH) >> 2);

    while (index < read_word_count) {

        data_status = msdc_register_base->SDC_DATSTA;
        if (data_status >> 1) {
            log_hal_error("sd_latch_read_blocks, SDC_DATSTA = %x \r\n", (unsigned int)data_status);
            if (data_status & SDC_DATSTA_DATTO_MASK) {
                status = ERROR_DATA_TIMEOUT;
                goto error;
            } else if (data_status & SDC_DATSTA_DATCRCERR_MASK) {
                status = ERROR_DATA_CRC_ERROR;
                goto error;
            }
        }

        if (msdc_register_base->MSDC_STA & MSDC_STA_DRQ_MASK) {
            *(read_buffer + index) = msdc_register_base->MSDC_DAT;
            index++;
        }
    }

    msdc_reset((msdc_port_t)sd_port);

    msdc_register_base->MSDC_INT;

    if (1 == block_number) {
        status = sd_wait_card_not_busy(sd_port);
        if (status != NO_ERROR) {
            goto error;
        }
    } else {
        status = sd_stop_transfer(sd_port);
        if (status != NO_ERROR) {
            goto error;
        }
    }

    while (msdc_card_is_present((msdc_port_t)sd_port)) {
        /*read card status*/
        status = sd_send_command(sd_port, MSDC_COMMAND13, sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
        if (NO_ERROR != status) {
            goto error;
        }

        response = msdc_register_base->SDC_RESP0;

        if ((response & SD_CARD_STATUS_READ_FOR_DATA_BIT_MASK) &&
                ((response & SD_CARD_STATUS_STATE_BIT_MASK) >> SD_CARD_STATUS_STATE_BIT_SHIFT) == TRAN_STA) {
            break;
        }
    }

    msdc_reset((msdc_port_t)sd_port);

    return NO_ERROR;

error:

    sd_information.error_status = status;
    msdc_reset((msdc_port_t)sd_port);
    msdc_register_base->MSDC_INT;
    if (1 < block_number) {
        sd_stop_transfer(sd_port);
    }

    return status;
}

bool sd_latch_command_test(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;
    uint32_t card_status;

    status = sd_get_card_status(sd_port, &card_status);

    if (NO_ERROR == status) {
        return MSDC_TRUE;
    } else {
        return MSDC_FALSE;
    }
}

bool sd_latch_data_test(hal_sd_port_t sd_port)
{
    sd_internal_status_t status;
    uint32_t index = 0;

    status = sd_set_block_length(sd_port, SD_BLOCK_LENGTH);

    if (NO_ERROR != status) {
        return MSDC_FALSE;
    }

    for (index = 0; index < 6; index++) {
        if (NO_ERROR != (status = sd_latch_read_blocks(sd_port, sd_read_buffer, index, 1))) {
            log_hal_error("sd_latch_read_blocks error, status = %d \r\n", status);
            return MSDC_FALSE;
        }
    }
    return MSDC_TRUE;
}


bool sd_output_clock_tuning(hal_sd_port_t sd_port, uint32_t clock)
{
    uint32_t vcore_voltage;
    uint32_t i = 0;

    vcore_voltage = msdc_get_vcore_voltage();

    /*update CM4 clock frequency.*/
    SystemCoreClockUpdate();

    if (PMIC_VOL_ERROR == vcore_voltage) {
        assert(0);
    }
    /*Check Vcore Voltage*/
    if (PMIC_VOL_0P9V == vcore_voltage) {
        while (i < 3) {
            if (0 == i) {
                msdc_set_output_clock((msdc_port_t)sd_port, msdc_output_clock[6].output_clock);
            } else {
                msdc_set_output_clock((msdc_port_t)sd_port, msdc_output_clock[6].output_clock / (i * 2));
            }
            if (sd_latch_command_test(sd_port) && sd_latch_data_test(sd_port)) {
#ifdef HAL_DVFS_MODULE_ENABLED
                dvfs_1p1v_need_sd_clock_tuning = MSDC_TRUE;
                dvfs_save_sd_init_clock = clock;
#endif
                goto end;
            }
            i++;
        }
        return MSDC_FALSE;
    } else if ((PMIC_VOL_1P1V == vcore_voltage) && (SystemCoreClock > CLK_CM4_FREQ_26M)) {
        while (i < 5) {
            msdc_set_output_clock((msdc_port_t)sd_port, msdc_output_clock[2 + i].output_clock);
            if (sd_latch_command_test(sd_port) && sd_latch_data_test(sd_port)) {
#ifdef HAL_DVFS_MODULE_ENABLED
                dvfs_1p3v_need_sd_clock_tuning = MSDC_TRUE;
                dvfs_save_sd_init_clock = clock;
#endif
                goto end;
            }
            i++;
        }
        return MSDC_FALSE;
    } else if ((PMIC_VOL_1P3V == vcore_voltage) && (SystemCoreClock > CLK_CM4_FREQ_26M)) {
        while (i < 7) {
            msdc_set_output_clock((msdc_port_t)sd_port, msdc_output_clock[0 + i].output_clock);
            if (sd_latch_command_test(sd_port) && sd_latch_data_test(sd_port)) {
                goto end;
            }
            i++;
        }
        return MSDC_FALSE;
    } else {
        while (i < 3) {
            if (0 == i) {
                msdc_set_output_clock((msdc_port_t)sd_port, msdc_output_clock[6].output_clock);
            } else {
                msdc_set_output_clock((msdc_port_t)sd_port, msdc_output_clock[6].output_clock / (i * 2));
            }
            if (sd_latch_command_test(sd_port) && sd_latch_data_test(sd_port)) {
#ifdef HAL_DVFS_MODULE_ENABLED
                dvfs_1p1v_need_sd_clock_tuning = MSDC_TRUE;
                dvfs_save_sd_init_clock = clock;
#endif
                goto end;
            }
            i++;
        }
        return MSDC_FALSE;
    }

end:
    if (msdc_get_output_clock((msdc_port_t)sd_port) > clock) {
        msdc_set_output_clock((msdc_port_t)sd_port, clock);
    }

    return MSDC_TRUE;
}

void sd_save_dma_interrupt_context(hal_sd_port_t sd_port,const sd_dma_interrupt_context_t *context)
{
    sd_dma_interrupt_context[sd_port] = *context;
}

static void sd_dma_interrupt_error_handle(hal_sd_port_t sd_port,sd_internal_status_t status)
{
    msdc_register_t *msdc_register_base;
    pdma_channel_t dma_channel;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;
    dma_channel = (sd_port == HAL_SD_PORT_0) ? PDMA_MSDC1 : PDMA_MSDC2;

    sd_information.error_status = status;
    msdc_dma_disable((msdc_port_t)sd_port);
    msdc_reset((msdc_port_t)sd_port);
    msdc_register_base->MSDC_INT;
    if (1 < sd_dma_interrupt_context[sd_port].sd_current_write_read_block_num) {
        sd_stop_transfer(sd_port);
    }

    pdma_stop(dma_channel);
    pdma_deinit(dma_channel);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs.*/
    dvfs_unlock(&sd_dma_interrupt_context[sd_port].sd_dvfs_lock);
#endif

    sd_dma_interrupt_context[sd_port].sd_current_write_read_block_num = 0;

    log_hal_error("msdc:sd_wait_dma_interrupt_transfer_ready error!\r\n");
    /*call user callback*/
    msdc_config[sd_port].msdc_sd_dma_interrupt_callback(HAL_SD_EVENT_TRANSFER_ERROR ,(void *)0);

    msdc_set_irqmask0((msdc_port_t)sd_port,0xFFFFFFFF);

    sd_information.is_busy = MSDC_FALSE;
}

static void sd_dma_interrput_gpt_callback(void *user_data)
{
    sd_dma_interrupt_context_t *context = 0;
    uint32_t response = 0;
    sd_internal_status_t status;
    pdma_channel_t dma_channel;
    msdc_register_t *msdc_register_base;
    static uint8_t count = 0;

    context = (sd_dma_interrupt_context_t *)user_data;
    
    msdc_register_base = (HAL_SD_PORT_0 == context->sd_port) ? MSDC0_REG : MSDC1_REG;
    dma_channel = (context->sd_port == HAL_SD_PORT_0) ? PDMA_MSDC1 : PDMA_MSDC2;

    /*100ms gpt timeout*/
    if (count < 100) {
        count++;
    }
    else {
        count = 0;
        /*free gpt timer*/
        hal_gpt_sw_free_timer(sd_dma_interrupt_gpt_timer);
        sd_dma_interrupt_error_handle((hal_sd_port_t)context->sd_port,ERROR_STATUS);
        return;
    }

    if (msdc_card_is_present((msdc_port_t)context->sd_port)) {
        /*read card status*/
        status = sd_send_command(context->sd_port, MSDC_COMMAND13, sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
        if (NO_ERROR != status) {
            count = 0;
            /*free gpt timer*/
            hal_gpt_sw_free_timer(sd_dma_interrupt_gpt_timer);
            sd_dma_interrupt_error_handle((hal_sd_port_t)context->sd_port,status);
            return;
        }

        response = msdc_register_base->SDC_RESP0;

        /*check corresponds to buffer empty singaling on the bus*/
        if ((response & SD_CARD_STATUS_READ_FOR_DATA_BIT_MASK)) {
             count = 0;
             /*free gpt timer*/
             hal_gpt_sw_free_timer(sd_dma_interrupt_gpt_timer);
             msdc_register_base->MSDC_INT;
             msdc_reset((msdc_port_t)context->sd_port);
             msdc_dma_disable((msdc_port_t)context->sd_port);
             pdma_stop(dma_channel);
             pdma_deinit(dma_channel);

             #ifdef HAL_DVFS_MODULE_ENABLED
             /*unlock dvfs after dma transfer done.*/
                dvfs_unlock(&sd_dma_interrupt_context[context->sd_port].sd_dvfs_lock);
             #endif

             msdc_set_irqmask0((msdc_port_t)context->sd_port,0xFFFFFFFF);

             /*call user callback*/
             msdc_config[context->sd_port].msdc_sd_dma_interrupt_callback(HAL_SD_EVENT_SUCCESS ,(void *)0);

             sd_information.is_busy = MSDC_FALSE;
             sd_dma_interrupt_context[context->sd_port].sd_current_write_read_block_num = 0;

             return;
        }
        /*restart gpt timer*/
        hal_gpt_sw_start_timer_ms(sd_dma_interrupt_gpt_timer,1,sd_dma_interrput_gpt_callback,&sd_dma_interrupt_context[context->sd_port]);
    }
    else {
        count = 0;
        status = ERROR_CARD_NOT_PRESENT;
        /*free gpt timer*/
        hal_gpt_sw_free_timer(sd_dma_interrupt_gpt_timer);
        sd_dma_interrupt_error_handle((hal_sd_port_t)context->sd_port,status);
        return;
    }
}

hal_sd_status_t sd_wait_dma_interrupt_transfer_ready(hal_sd_port_t sd_port)
{
    uint32_t response = 0;
    sd_internal_status_t status;
    pdma_channel_t dma_channel;
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;
    dma_channel = (sd_port == HAL_SD_PORT_0) ? PDMA_MSDC1 : PDMA_MSDC2;

    status = sd_wait_data_ready(sd_port);
    if (status != NO_ERROR) {
        goto error;
    }

    msdc_reset((msdc_port_t)sd_port);
    msdc_register_base->MSDC_INT;
    
    if (1 < sd_dma_interrupt_context[sd_port].sd_current_write_read_block_num) {
        
        if (MSDC_COMMAND25 == sd_dma_interrupt_context[sd_port].sd_cmd) {
            /*for mutil block write, this API must be called for check data write to card done*/        
            status = sd_wait_last_block_complete(sd_port);
            if (status != NO_ERROR) {
                goto error;
            }
        }
        
        status = sd_stop_transfer(sd_port);    
        
        if (status != NO_ERROR) {
            goto error;
        }

    } else if (1 ==  sd_dma_interrupt_context[sd_port].sd_current_write_read_block_num) {
        status = sd_wait_card_not_busy(sd_port);
        if (status != NO_ERROR) {
            goto error;
        }
    }
    
    if (msdc_card_is_present((msdc_port_t)sd_port)) {
        /*read card status*/
        status = sd_send_command(sd_port, MSDC_COMMAND13, sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
        if (NO_ERROR != status) {
            goto error;
        }

        response = msdc_register_base->SDC_RESP0;

        /*check corresponds to buffer empty singaling on the bus*/
        if ((response & SD_CARD_STATUS_READ_FOR_DATA_BIT_MASK)) {
             msdc_register_base->MSDC_INT;
             msdc_reset((msdc_port_t)sd_port);
             msdc_dma_disable((msdc_port_t)sd_port);
             pdma_stop(dma_channel);
             pdma_deinit(dma_channel);

             #ifdef HAL_DVFS_MODULE_ENABLED
             /*unlock dvfs after dma transfer done.*/
                dvfs_unlock(&sd_dma_interrupt_context[sd_port].sd_dvfs_lock);
             #endif

             msdc_set_irqmask0((msdc_port_t)sd_port,0xFFFFFFFF);

             /*call user callback*/
             msdc_config[sd_port].msdc_sd_dma_interrupt_callback(HAL_SD_EVENT_SUCCESS,(void *)0);

             sd_dma_interrupt_context[sd_port].sd_current_write_read_block_num = 0;
             sd_information.is_busy = MSDC_FALSE;

             return HAL_SD_STATUS_OK;
        }
        else {
            hal_gpt_sw_get_timer(&sd_dma_interrupt_gpt_timer);
            hal_gpt_sw_start_timer_ms(sd_dma_interrupt_gpt_timer,1,sd_dma_interrput_gpt_callback,&sd_dma_interrupt_context[sd_port]);
            return HAL_SD_STATUS_OK;
        }
    }
    else {
        status = ERROR_INVALID_CARD;
        goto error;
    }

error:

    sd_dma_interrupt_error_handle(sd_port,status);
    return HAL_SD_STATUS_ERROR;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void sd_backup_all_register(hal_sd_port_t sd_port)
{
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    sd_backup_register.MSDC_CFG     = msdc_register_base->MSDC_CFG;
    sd_backup_register.MSDC_IOCON   = msdc_register_base->MSDC_IOCON;
    sd_backup_register.MSDC_IOCON1  = msdc_register_base->MSDC_IOCON1;
    sd_backup_register.SDC_CFG      = msdc_register_base->SDC_CFG;
    sd_backup_register.SDC_IRQMASK0 = msdc_register_base->SDC_IRQMASK0;
    sd_backup_register.SDC_IRQMASK1 = msdc_register_base->SDC_IRQMASK1;
    sd_backup_register.CLK_RED      = msdc_register_base->CLK_RED;
}


void sd_restore_all_register(hal_sd_port_t sd_port)
{
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    /*must be enable SD first*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_MSDC_MASK;

    msdc_register_base->MSDC_CFG     = sd_backup_register.MSDC_CFG;
    msdc_register_base->MSDC_IOCON   = sd_backup_register.MSDC_IOCON;
    msdc_register_base->MSDC_IOCON1  = sd_backup_register.MSDC_IOCON1;
    msdc_register_base->SDC_CFG      = sd_backup_register.SDC_CFG;
    msdc_register_base->SDC_IRQMASK0 = sd_backup_register.SDC_IRQMASK0;
    msdc_register_base->SDC_IRQMASK1 = sd_backup_register.SDC_IRQMASK1;
    msdc_register_base->CLK_RED      = sd_backup_register.CLK_RED;
}

void sd_backup_all(void)
{
    /*sdcard msdc to gpio mode*/
    msdc_switch_pinmux_to_gpio_mode();

    if (!sd_information.is_initialized) {
        sd_backup_parameter.is_initialized = MSDC_FALSE;
        return;
    } else {
        sd_backup_parameter.is_initialized = MSDC_TRUE;
    }

    if (MSDC_OWNER_SD == msdc_get_owner(MSDC_PORT_0)) {
        sd_backup_parameter.sd_port = HAL_SD_PORT_0;
    } else if (MSDC_OWNER_SD == msdc_get_owner(MSDC_PORT_1)) {
        sd_backup_parameter.sd_port = HAL_SD_PORT_1;
    }

    sd_backup_all_register(sd_backup_parameter.sd_port);
}

void sd_restore_all(void)
{
    if (!sd_backup_parameter.is_initialized) {
        return;
    }

    /* sdcard GPIO to MSDC mode*/
    msdc_switch_pinmux_to_msdc_mode();

    sd_restore_all_register(sd_backup_parameter.sd_port);
    sd_backup_parameter.is_initialized = MSDC_FALSE;

    /*call once read make MSDC enter transfer state*/
    sd_latch_read_blocks(sd_backup_parameter.sd_port, sd_read_buffer, 0, 1);
}

#endif /*HAL_SLEEP_MANAGER_ENABLED*/
#endif /*HAL_SD_MODULE_ENABLED*/

