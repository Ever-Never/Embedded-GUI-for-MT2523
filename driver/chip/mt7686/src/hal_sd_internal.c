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
#include "assert.h"

#define CLK_CM4_FREQ_26M ((uint32_t) 26000000)

sd_information_t sd_information;
static uint32_t mmc_extended_csd[128];
uint32_t sd_csd[4];
static uint32_t sd_read_buffer[128];
static uint32_t sd_dma_interrupt_gpt_timer;
static sd_dma_interrupt_context_t sd_dma_interrupt_context;
extern msdc_config_t msdc_config;

#ifdef HAL_SLEEP_MANAGER_ENABLED
static volatile sd_backup_parameter_t sd_backup_parameter;
static volatile msdc_register_t sd_backup_register;
#endif

sd_internal_status_t sd_wait_data_ready(void)
{
    uint32_t interrupt_status;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);
    do {
        interrupt_status = MSDC_REG->MSDC_INT;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
        hal_gpt_get_duration_count(start_count, end_count, &count);
    } while ((!(interrupt_status & MSDC_DAT_INTS)) && msdc_card_is_present() && (count < MSDC_TIMEOUT_PERIOD_DATA));

    /*clear msdc interrupt status*/
    MSDC_REG->MSDC_INT =  MSDC_DAT_INTS;

    if (count >= MSDC_TIMEOUT_PERIOD_DATA) {
        log_hal_error("wait sd data ready timeout.\r\n");
        return ERROR_DATA_TIMEOUT;
    }

    if (interrupt_status & MSDC_INT_DATTMO) {
        return ERROR_DATA_TIMEOUT;
    } else if (interrupt_status & MSDC_INT_DATCRCERR) {
        log_hal_error("interrupt_status = %x\r\n", (unsigned int)interrupt_status);
        return ERROR_DATA_CRC_ERROR;
    } else {
        return NO_ERROR;
    }
}

sd_internal_status_t sd_wait_data_ready_dma(void)
{
    uint32_t interrupt_status;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);
    do {
        interrupt_status = MSDC_REG->MSDC_INT;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
        hal_gpt_get_duration_count(start_count, end_count, &count);
    } while ((!(interrupt_status & (MSDC_DAT_INTS | MSDC_INT_DXFER_DONE))) && msdc_card_is_present() && (count < MSDC_TIMEOUT_PERIOD_DATA));

    /*clear msdc interrupt status*/
    MSDC_REG->MSDC_INT = (MSDC_DAT_INTS | MSDC_INT_DXFER_DONE);

    if (count >= MSDC_TIMEOUT_PERIOD_DATA) {
        log_hal_error("wait sd data ready timeout.\r\n");
        return ERROR_DATA_TIMEOUT;
    }

    if (interrupt_status & MSDC_INT_DATTMO) {
        return ERROR_DATA_TIMEOUT;
    } else if (interrupt_status & MSDC_INT_DATCRCERR) {
        log_hal_error("interrupt_status = %x\r\n", (unsigned int)interrupt_status);
        return ERROR_DATA_CRC_ERROR;
    } else {
        return NO_ERROR;
    }
}


sd_internal_status_t sd_wait_command_ready(void)
{
    volatile uint32_t interrupt_status;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);
    do {
        interrupt_status = MSDC_REG->MSDC_INT;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
        hal_gpt_get_duration_count(start_count, end_count, &count);
    } while ((!(interrupt_status & (MSDC_CMD_INTS | MSDC_ACMD_INTS))) && msdc_card_is_present() && (count < MSDC_TIMEOUT_PERIOD_COMMAND));

    /*clear msdc interrupt status*/
    MSDC_REG->MSDC_INT =  MSDC_CMD_INTS | MSDC_ACMD_INTS;

    if (count >= MSDC_TIMEOUT_PERIOD_COMMAND) {
        log_hal_error("wait sd command ready timeout.\r\n");
        return ERROR_COMMANDD_TIMEOUT;
    }

    if (interrupt_status & (MSDC_INT_CMDRDY | MSDC_INT_ACMDRDY)) {
        return NO_ERROR;
    } else if (interrupt_status & (MSDC_INT_RSPCRCERR | MSDC_INT_ACMDCRCERR)) {
        return ERROR_COMMAND_CRC_ERROR;
    } else if (interrupt_status & (MSDC_INT_CMDTMO | MSDC_INT_ACMDTMO)) {
        return ERROR_COMMANDD_TIMEOUT;
    }

    return NO_ERROR;
}


sd_internal_status_t sd_send_command(uint32_t msdc_command, uint32_t argument)
{
    sd_internal_status_t status;

    if (!(SDC_CMD_STOP & msdc_command)) {
        while (MSDC_REG->SDC_STS & SDC_STS_CMDBUSY);
    } else {
        while (MSDC_REG->SDC_STS & SDC_STS_SDCBUSY);
    }

    /*clear msdc command interrupt*/
    MSDC_REG->MSDC_INT = MSDC_CMD_INTS | MSDC_ACMD_INTS;
    MSDC_REG->SDC_ARG = argument;
    MSDC_REG->SDC_CMD = msdc_command;

    status = sd_wait_command_ready();
    if (status != NO_ERROR) {
        log_hal_error("send command error, command = %x, arg = %x \r\n", (unsigned int)msdc_command, (unsigned int)argument);
        return status;
    }

    return NO_ERROR;
}

sd_internal_status_t sd_send_command55(uint32_t argument)
{
    sd_internal_status_t status;
    uint32_t response;

    status = sd_send_command(MSDC_COMMAND55, (argument << 16));
    if (NO_ERROR != status) {
        return status;
    }

    response = MSDC_REG->SDC_RESP0;

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

sd_internal_status_t sd_reset(void)
{
    sd_internal_status_t status;

    status = sd_send_command(MSDC_COMMAND0, COMMAND_NO_ARGUMENT);
    sd_information.sd_state = IDLE_STA;

    return status;
}

hal_sd_card_type_t sd_check_sd_card_type(void)
{
    sd_internal_status_t status;
    uint32_t response = 0;
    uint32_t ocr_argument = 0;
    uint32_t ocr_value = 0;
    bool is_first_command41 = true;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    status = sd_send_command(MSDC_COMMAND8, COMMAND8_ARGUMENT);
    if (NO_ERROR != status) {
        sd_reset();
        sd_information.command8_response = SD_COMMAND8_RESPONSE_NO_RESPONSE;
    } else {
        response = MSDC_REG->SDC_RESP0;
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
        while (MSDC_REG->SDC_STS & SDC_STS_SDCBUSY);

        status = sd_send_command55(SD_RCA_DEFAULT);
        if (NO_ERROR != status) {
            sd_information.error_status = status;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        /*wait MSDC not busy*/
        while (MSDC_REG->SDC_STS & SDC_STS_SDCBUSY);

        if (is_first_command41) {
            status = sd_send_command(MSDC_ACOMMAND41, COMMAND_NO_ARGUMENT);
            is_first_command41 = false;
        } else {
            status = sd_send_command(MSDC_ACOMMAND41, ocr_argument);
        }

        if (NO_ERROR != status) {
            sd_information.error_status = status;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        ocr_value = MSDC_REG->SDC_RESP0;
        if (!(ocr_value & SD_OCR_VOLTAGE_ARGUMENT)) {
            sd_information.error_status = ERROR_OCR_NOT_SUPPORT;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        if (false == msdc_card_is_present()) {
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

    sd_information.is_inactive = false;
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


hal_sd_card_type_t sd_check_mmc_card_type(void)
{
    sd_internal_status_t status;
    uint32_t ocr_argument = 0;
    uint32_t ocr_value = 0;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

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
        while (MSDC_REG->SDC_STS & SDC_STS_SDCBUSY);

        status = sd_send_command(MSDC_COMMAND1_MMC, ocr_argument);
        if (NO_ERROR != status) {
            sd_information.error_status = status;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        ocr_value = MSDC_REG->SDC_RESP0;

        if (!(ocr_value & SD_OCR_VOLTAGE_ARGUMENT)) {
            sd_information.error_status = ERROR_OCR_NOT_SUPPORT;
            return HAL_SD_TYPE_UNKNOWN_CARD;
        }

        if (false == msdc_card_is_present()) {
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

    sd_information.is_inactive = false;
    sd_information.sd_ocr = ocr_value;

    if ((ocr_value & MMC_HIGH_DESITY_CHECK_MASK) == MMC_HIGH_DESITY_CHECK_BIT) {
        sd_information.card_type = HAL_SD_TYPE_MMC42_CARD;
    } else {
        sd_information.card_type = HAL_SD_TYPE_MMC_CARD;
    }

    sd_information.sd_state = READY_STA;

    return (sd_information.card_type);
}

hal_sd_card_type_t sd_check_card_type(void)
{
    hal_sd_card_type_t card_type;

    if ((card_type = sd_check_sd_card_type()) != HAL_SD_TYPE_UNKNOWN_CARD) {
        return card_type;
    } else if ((card_type = sd_check_mmc_card_type()) != HAL_SD_TYPE_UNKNOWN_CARD) {
        return card_type;
    } else {
        return HAL_SD_TYPE_UNKNOWN_CARD;
    }
}

sd_internal_status_t sd_get_card_id(void)
{
    sd_internal_status_t status;

    status = sd_send_command(MSDC_COMMAND2, COMMAND_NO_ARGUMENT);
    if (NO_ERROR != status) {
        return status;
    }

    sd_information.cid[0] = MSDC_REG->SDC_RESP0;
    sd_information.cid[1] = MSDC_REG->SDC_RESP1;
    sd_information.cid[2] = MSDC_REG->SDC_RESP2;
    sd_information.cid[3] = MSDC_REG->SDC_RESP3;

    sd_information.sd_state = IDENT_STA;

    return NO_ERROR;
}

sd_internal_status_t sd_get_rca(void)
{
    sd_internal_status_t status;
    uint32_t response = 0;
    sd_state_t state;

    if (sd_information.card_type & (HAL_SD_TYPE_SD_CARD | HAL_SD_TYPE_SD20_HCS_CARD | HAL_SD_TYPE_SD20_LCS_CARD)) {
        /*sd card*/
        status = sd_send_command(MSDC_COMMAND3, COMMAND_NO_ARGUMENT);
        if (NO_ERROR != status) {
            return status;
        }

        response = MSDC_REG->SDC_RESP0;
        sd_information.rca = response >> COMMAND_RCA_ARGUMENT_SHIFT;
    } else {
        /*emmc card*/
        status = sd_send_command(MSDC_COMMAND3_MMC, COMMAND_MMC_RCA_ARGUMENT << COMMAND_RCA_ARGUMENT_SHIFT);
        if (NO_ERROR != status) {
            return status;
        }

        response = MSDC_REG->SDC_RESP0;

        status = sd_send_command(MSDC_COMMAND13, COMMAND_MMC_RCA_ARGUMENT << COMMAND_RCA_ARGUMENT_SHIFT);
        if (NO_ERROR != status) {
            return status;
        }

        response = MSDC_REG->SDC_RESP0;
        state = (sd_state_t)((response & SD_CARD_STATUS_STATE_BIT_MASK) >> SD_CARD_STATUS_STATE_BIT_SHIFT);

        if (STBY_STA != state) {
            return ERROR_RCA_FAILED;
        }

        sd_information.rca = COMMAND_MMC_RCA_ARGUMENT;
    }

    sd_information.sd_state = STBY_STA;

    return NO_ERROR;
}

sd_internal_status_t sd_acommand42(void)
{
    sd_internal_status_t status;

    status = sd_send_command55(sd_information.rca);
    if (status != NO_ERROR) {
        return status;
    }

    /*disconnect the 50K Ohm pull-up resistor on CD/DAT3*/
    status = sd_send_command(MSDC_ACOMMAND42, COMMAND_NO_ARGUMENT);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_check_card_status();
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

void sd_analysis_csd(uint32_t *csd)
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
        sd_information.is_write_protection = true;
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


sd_internal_status_t sd_get_csd(void)
{
    sd_internal_status_t status;

    status = sd_send_command(MSDC_COMMAND9, (uint32_t)sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
    if (NO_ERROR != status) {
        return status;
    }

    sd_csd[0] = MSDC_REG->SDC_RESP0;
    sd_csd[1] = MSDC_REG->SDC_RESP1;
    sd_csd[2] = MSDC_REG->SDC_RESP2;
    sd_csd[3] = MSDC_REG->SDC_RESP3;

    sd_analysis_csd(sd_csd);

    return NO_ERROR;
}


sd_internal_status_t sd_set_dsr(void)
{
    return  sd_send_command(MSDC_COMMAND4, COMMAND_DCR_ARGUMENT << 16);
}

sd_internal_status_t sd_wait_card_not_busy(void)
{
    while ((MSDC_IS_CMD_BUSY() || MSDC_IS_SDC_BUSY()) && msdc_card_is_present());

    return NO_ERROR;
}

sd_internal_status_t sd_check_card_status(void)
{
    uint32_t response;

    response = MSDC_REG->SDC_RESP0;

    if (!(response & SD_CARD_STATUS_ERROR_MASK)) {
        return NO_ERROR;
    } else if (response & SD_CARD_STATUS_LOCKED) {
        return ERROR_CARD_IS_LOCKED;
    } else {
        return ERROR_STATUS;
    }
}

sd_internal_status_t sd_select_card(uint32_t rca)
{
    sd_internal_status_t status;

    status = sd_send_command(MSDC_COMMAND7, rca << COMMAND_RCA_ARGUMENT_SHIFT);
    if (status !=  NO_ERROR) {
        return status;
    }

    status = sd_wait_card_not_busy();
    if (status !=  NO_ERROR) {
        return status;
    }

    status = sd_check_card_status();
    if (status !=  NO_ERROR) {
        return status;
    }

    return NO_ERROR;
}

sd_internal_status_t sd_set_block_length(uint32_t block_length)
{
    sd_internal_status_t status;

    if (block_length > MSDC_MAX_BLOCK_LENGTH) {
        return ERROR_INVALID_BLOCK_LENGTH;
    }

    status = sd_send_command(MSDC_COMMAND16, block_length);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_check_card_status();
    if (status != NO_ERROR) {
        return status;
    }

    sd_information.block_length = block_length;

    return NO_ERROR;
}

sd_internal_status_t sd_analysis_scr(uint32_t *scr)
{
    uint8_t *scr_pointer;

    scr_pointer = (uint8_t *)scr;

    sd_get_bit_field_value((uint8_t *)(&sd_information.scr.spec_ver), scr_pointer, 0, 4);
    sd_get_bit_field_value(&sd_information.scr.dat_after_erase, (scr_pointer + 1), 7, 1);
    sd_get_bit_field_value(&sd_information.scr.security, (scr_pointer + 1), 4, 3);
    sd_get_bit_field_value(&sd_information.scr.bus_width, (scr_pointer + 1), 0, 4);

    return NO_ERROR;
}



sd_internal_status_t mmc_switch(uint32_t argument)
{
    sd_internal_status_t status;
    uint32_t response;

    status = sd_send_command(MSDC_COMMAND6_MMC, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    status = sd_wait_card_not_busy();
    if (status != NO_ERROR) {
        goto error;
    }

    /*read card status*/
    status = sd_send_command(MSDC_COMMAND13, sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
    if (NO_ERROR != status) {
        goto error;
    }

    status = sd_check_card_status();
    if (NO_ERROR != status) {
        goto error;
    }

    response = MSDC_REG->SDC_RESP0;
    if (response & MMC_SWITCH_ERROR_BIT_MASK) {
        status = ERROR_MMC_SWITCH_ERROR;
        goto error;
    }


    status = sd_set_block_length(SD_BLOCK_LENGTH);
    if (NO_ERROR != status) {
        return status;
    }

    /*some ext cst bit filed has changed,read again*/
    status = mmc_get_extended_csd();
    if (NO_ERROR != status) {
        return status;
    }

    msdc_reset();
    return NO_ERROR;

error:
    msdc_reset();
    return status;
}

sd_internal_status_t sd_get_scr(void)
{
    sd_internal_status_t status;
    uint32_t i = 0;
    uint32_t scr[2];
    uint32_t command = 0;

    status = sd_set_block_length(SD_SCR_LENGTH);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_send_command55(sd_information.rca);
    if (status != NO_ERROR) {
        return status;
    }

    command = MSDC_ACOMMAND51 | (SD_SCR_LENGTH << SDC_CMD_LEN_OFFSET);
    status = sd_send_command(command, COMMAND_NO_ARGUMENT);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_check_card_status();
    if (status != NO_ERROR) {
        return status;
    }

    while (i < 2) {
        if (4 <= (MSDC_REG->MSDC_FIFOCS & MSDC_FIFOCS_RXCNT)) {
            *(uint32_t *)(scr + i) = MSDC_REG->MSDC_RXDATA;
            i++;
        }
    }
    
    /*set block number.*/
    MSDC_REG->SDC_BLK_NUM = 1;
    
    status =  sd_wait_data_ready();
    if (status != NO_ERROR) {
        return status;
    }

    sd_analysis_scr(scr);

    msdc_reset();

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



sd_internal_status_t sd_query_switch_high_speed(uint32_t argument)
{
    sd_internal_status_t status;
    uint32_t command6_response[COMMAND6_RESPONSE_LENGTH >> 2];
    uint32_t index = 0;
    uint32_t read_word_count = 0;
    sd_command6_switch_status_t switch_status;
    uint32_t command = 0;

    status = sd_set_block_length(COMMAND6_RESPONSE_LENGTH);
    if (status != NO_ERROR) {
        return status;
    }

    command = MSDC_COMMAND6 | (COMMAND6_RESPONSE_LENGTH << SDC_CMD_LEN_OFFSET);
    status = sd_send_command(command, argument);
    if (status != NO_ERROR) {
        return status;
    }

    status = sd_check_card_status();
    if (status != NO_ERROR) {
        return status;
    }

    /*set CMD6 read response length*/
    read_word_count = COMMAND6_RESPONSE_LENGTH >> 2;
    while (index < read_word_count) {
        if (4 <= (MSDC_REG->MSDC_FIFOCS & MSDC_FIFOCS_RXCNT)) {
            *(command6_response + index) = MSDC_REG->MSDC_RXDATA;
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


sd_internal_status_t sd_select_high_speed(void)
{
    sd_internal_status_t status;
    uint32_t i = 1000;

    do {
        status = sd_query_switch_high_speed(SD_COMMAND6_QUERY_HIGH_SPEED);
        if (NO_ERROR == status) {
            status = sd_query_switch_high_speed(SD_COMMAND6_SELECT_HIGH_SPEED);
            if (NO_ERROR == status) {
                sd_information.is_high_speed = true;
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


sd_internal_status_t mmc_get_extended_csd(void)
{
    sd_internal_status_t status;
    uint32_t index = 0;

    status = sd_send_command(MSDC_COMMAND8_MMC, COMMAND_NO_ARGUMENT);
    if (NO_ERROR != status) {
        goto error;
    }

    status = sd_check_card_status();
    if (NO_ERROR != status) {
        goto error;
    }

    while (index < 128) {
        if (4 <= (MSDC_REG->MSDC_FIFOCS & MSDC_FIFOCS_RXCNT)) {
            *(mmc_extended_csd + index) = MSDC_REG->MSDC_RXDATA;
            index ++;
        }
    }

    status = sd_wait_data_ready();
    if (NO_ERROR != status) {
        goto error;
    }

    msdc_reset();

    sd_information.csd.ext_csd = (extended_csd_information_t *)mmc_extended_csd;

    return NO_ERROR;

error:
    msdc_reset();
    return status;
}


void mmc_is_version_44(void)
{
    if (sd_information.csd.ext_csd->ext_csd_rev > MMC_EXTENDED_CSD_VERSION_43) {
        if (0 != sd_information.csd.ext_csd->boot_size_mul) {
            sd_information.emmc_information.is_mmc_version_44 = true;
        } else {
            sd_information.emmc_information.is_mmc_version_44 = false;
        }
    } else {
        sd_information.emmc_information.is_mmc_version_44 = false;
    }
}

sd_internal_status_t mmc_set_high_speed(void)
{
    sd_internal_status_t status;

    status = sd_set_block_length(SD_BLOCK_LENGTH);
    if (NO_ERROR != status) {
        goto error;
    }

    status = mmc_get_extended_csd();
    if (NO_ERROR != status) {
        goto error;
    }

    if (sd_information.card_type == HAL_SD_TYPE_MMC42_CARD) {
        sd_information.csd.capacity = SD_BLOCK_LENGTH * sd_information.csd.ext_csd->sec_count;
    }

    mmc_is_version_44();

    /*set bus clock*/
    if (sd_information.csd.ext_csd->card_type & MMC_HS_52M) {
        sd_information.is_high_speed = true;
    } else if (sd_information.csd.ext_csd->card_type & MMC_HS_26M) {
        sd_information.is_high_speed = false;
    }

    return NO_ERROR;

error:
    return status;
}


sd_internal_status_t sd_stop_transfer(void)
{
    sd_internal_status_t status;
    uint32_t retry = 0;

    while (30 > retry) {
        status = sd_send_command(MSDC_COMMAND12, COMMAND_NO_ARGUMENT);
        if (status != NO_ERROR) {
            retry++;
        } else {
            break;
        }
    }

    if (30 <= retry) {
        return status;
    }

    status = sd_wait_card_not_busy();
    if (NO_ERROR != status) {
        return status;
    }

    return NO_ERROR;
}

sd_internal_status_t sd_get_card_status(uint32_t *card_status)
{
    sd_internal_status_t status;

    status = sd_send_command(MSDC_COMMAND13, sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
    if (NO_ERROR != status) {
        return status;
    }

    *card_status = MSDC_REG->SDC_RESP0;

    return NO_ERROR;
}

sd_internal_status_t sd_send_erase_command(uint32_t command, uint32_t address)
{
    sd_internal_status_t status;
    uint32_t card_status;

    if (MSDC_COMMAND38 != command) {
        status = sd_send_command(command, address);
        if (NO_ERROR != status) {
            return status;
        }
    } else {
        status = sd_send_command(command, COMMAND_NO_ARGUMENT);
        if (NO_ERROR != status) {
            return status;
        }
    }

    status = sd_check_card_status();
    if (NO_ERROR != status) {
        return status;
    }

    if (MSDC_COMMAND38 == command) {
        status = sd_wait_card_not_busy();
        if (NO_ERROR != status) {
            return status;
        }

        do {
            status = sd_get_card_status(&card_status);
            if (NO_ERROR != status) {
                return status;
            }
        } while (((card_status & SD_CARD_STATUS_STATE_BIT_MASK) >> SD_CARD_STATUS_STATE_BIT_SHIFT) != TRAN_STA);
    }

    return NO_ERROR;
}


sd_internal_status_t sd_latch_read_blocks(uint32_t *read_buffer,  uint32_t read_address, uint32_t block_number)
{
    sd_internal_status_t status;
    uint32_t read_command;
    uint64_t read_word_count = 0;
    uint32_t response;
    uint32_t block_multiplier;
    uint32_t count;
    uint32_t fifo_count;

    if (NULL == read_buffer) {
        log_hal_error("parameter error\r\n");
        return ERROR_ERRORS;
    }

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

    read_command = ((block_number > 1) ? MSDC_COMMAND18 : MSDC_COMMAND17) | (SD_BLOCK_LENGTH << SDC_CMD_LEN_OFFSET);
    status = sd_send_command(read_command, (read_address * block_multiplier));
    if (status != NO_ERROR) {
        goto error;
    }

    status = sd_check_card_status();
    if (status != NO_ERROR) {
        goto error;
    }

    read_word_count = (uint64_t)(((uint64_t)block_number * SD_BLOCK_LENGTH) >> 2);

    while (read_word_count) {
        fifo_count = ((MSDC_REG->MSDC_FIFOCS & MSDC_FIFOCS_RXCNT) >> 0);
        log_hal_info("fifo_count = %x \r\n", (unsigned int)fifo_count);

        if ((read_word_count < (MSDC_FIFO_SIZE >> 2)) && (read_word_count <= (fifo_count >> 2))) {
            do {
                *read_buffer++ = MSDC_REG->MSDC_RXDATA;
            } while (--read_word_count);
        } else if (read_word_count >= (MSDC_FIFO_SIZE >> 2) && ((MSDC_FIFO_SIZE >> 2) <= MSDC_RXFIFOCNT())) {
            count = MSDC_FIFO_SIZE >> 2;
            do {
                *read_buffer++ = MSDC_REG->MSDC_RXDATA;
            } while (--count);
            read_word_count = read_word_count - (MSDC_FIFO_SIZE >> 2);
        }
    }

    msdc_reset();

    /*clear msdc interrupt status*/
    MSDC_REG->MSDC_INT =  MSDC_DAT_INTS;

    if (1 == block_number) {
        status = sd_wait_card_not_busy();
        if (status != NO_ERROR) {
            goto error;
        }
    } else {
        status = sd_stop_transfer();
        if (status != NO_ERROR) {
            goto error;
        }
    }

    while (msdc_card_is_present()) {
        /*read card status*/
        status = sd_send_command(MSDC_COMMAND13, sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
        if (NO_ERROR != status) {
            goto error;
        }

        response = MSDC_REG->SDC_RESP0;

        if ((response & SD_CARD_STATUS_READ_FOR_DATA_BIT_MASK) &&
                ((response & SD_CARD_STATUS_STATE_BIT_MASK) >> SD_CARD_STATUS_STATE_BIT_SHIFT) == TRAN_STA) {
            break;
        }
    }

    msdc_reset();

    return NO_ERROR;

error:

    sd_information.error_status = status;
    log_hal_error("sd error status = %d \r\n", status);
    msdc_reset();
    MSDC_REG->MSDC_INT = MSDC_REG->MSDC_INT;
    if (1 < block_number) {
        sd_stop_transfer();
    }

    return status;
}

bool sd_latch_command_test(void)
{
    sd_internal_status_t status;
    uint32_t card_status;

    status = sd_get_card_status(&card_status);

    if (NO_ERROR == status) {
        return true;
    } else {
        return false;
    }
}

bool sd_latch_data_test(void)
{
    sd_internal_status_t status;
    uint32_t index = 0;

    status = sd_set_block_length(SD_BLOCK_LENGTH);

    if (NO_ERROR != status) {
        return false;
    }

    for (index = 0; index < 6; index++) {
        if (NO_ERROR != sd_latch_read_blocks(sd_read_buffer, index, 1)) {
            return false;
        }
    }
    return true;
}


bool sd_output_clock_tuning(uint32_t clock)
{
    msdc_set_output_clock(clock);
    return true;
}

void sd_save_dma_interrupt_context(const sd_dma_interrupt_context_t *context)
{
    sd_dma_interrupt_context = *context;
}

static void sd_dma_interrupt_error_handle(sd_internal_status_t status)
{
    sd_information.error_status = status;
    msdc_dma_disable();
    MSDC_REG->MSDC_INTEN = 0;
    msdc_reset();
    MSDC_REG->MSDC_INT = MSDC_REG->MSDC_INT;
    if (1 < sd_dma_interrupt_context.sd_current_write_read_block_num) {
        sd_stop_transfer();
    }

    sd_dma_interrupt_context.sd_current_write_read_block_num = 0;
    sd_information.is_busy = false;

    /*call user callback*/
    msdc_config.msdc_sd_callback(HAL_SD_EVENT_TRANSFER_ERROR,NULL);

	msdc_nvic_set(false);

    log_hal_error("sd error status = %d \r\n", status);
}

static void sd_dma_interrput_gpt_callback(void *user_data)
{
    sd_internal_status_t status = NO_ERROR;
    uint32_t response = 0;
    static uint32_t count = 0;

    /*1500ms gpt timeout*/
    if (count < 1500) {
        count++;
    }
    else {
        count = 0;
        /*free gpt timer*/
        hal_gpt_sw_free_timer(sd_dma_interrupt_gpt_timer);
        sd_dma_interrupt_error_handle(ERROR_STATUS);
        return;
    }

    if (!((MSDC_IS_CMD_BUSY() || MSDC_IS_SDC_BUSY()) && msdc_card_is_present())) {    
        status = sd_send_command(MSDC_COMMAND13, sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
        if (NO_ERROR != status) {
            sd_dma_interrupt_error_handle(status);
        }
        response = MSDC_REG->SDC_RESP0;

        /*check corresponds to buffer empty singaling on the bus*/
        if ((response & SD_CARD_STATUS_READ_FOR_DATA_BIT_MASK)) {
            count = 0;
            /*free gpt timer*/
            hal_gpt_sw_free_timer(sd_dma_interrupt_gpt_timer);
            
            msdc_reset();
            MSDC_REG->MSDC_INTEN = 0;
            msdc_dma_disable();
            MSDC_REG->MSDC_INT = MSDC_REG->MSDC_INT;
            /*call user callback*/
            msdc_config.msdc_sd_callback(HAL_SD_EVENT_SUCCESS,NULL);

            sd_dma_interrupt_context.sd_current_write_read_block_num = 0;

            msdc_nvic_set(false);
            sd_information.is_busy = false;
            return;
        }           
    }
    /*restart gpt timer*/
    hal_gpt_sw_start_timer_ms(sd_dma_interrupt_gpt_timer,1,sd_dma_interrput_gpt_callback,&sd_dma_interrupt_context);
}

hal_sd_status_t sd_wait_dma_interrupt_transfer_ready(void)
{
    sd_internal_status_t status = NO_ERROR;
    uint32_t response = 0;
    uint32_t retry = 0;

    /*diable dma and msdc interrupt*/
    msdc_dma_disable();
    MSDC_REG->MSDC_INTEN = 0;

    if (1 < sd_dma_interrupt_context.sd_current_write_read_block_num) {
        while (30 > retry) {
            status = sd_send_command(MSDC_COMMAND12, COMMAND_NO_ARGUMENT);
            if (status != NO_ERROR) {
                retry++;
            } else {
                break;
            }
        }
        
        if (30 <= retry) {
            if (status != NO_ERROR) {
                goto error;
            }            
        }       
    } 

    if ((MSDC_IS_CMD_BUSY() || MSDC_IS_SDC_BUSY()) && msdc_card_is_present()) {
         hal_gpt_sw_get_timer(&sd_dma_interrupt_gpt_timer);
         hal_gpt_sw_start_timer_ms(sd_dma_interrupt_gpt_timer,1,sd_dma_interrput_gpt_callback,&sd_dma_interrupt_context);
         return HAL_SD_STATUS_OK;
    }
    
    while (msdc_card_is_present()) {
        /*read card status*/
        status = sd_send_command(MSDC_COMMAND13, sd_information.rca << COMMAND_RCA_ARGUMENT_SHIFT);
        if (NO_ERROR != status) {
            goto error;
        }

        response = MSDC_REG->SDC_RESP0;

        /*check corresponds to buffer empty singaling on the bus*/
        if ((response & SD_CARD_STATUS_READ_FOR_DATA_BIT_MASK)) {
            msdc_reset();
            MSDC_REG->MSDC_INTEN = 0;
            msdc_dma_disable();
            MSDC_REG->MSDC_INT = MSDC_REG->MSDC_INT;
            /*call user callback*/
            msdc_config.msdc_sd_callback(HAL_SD_EVENT_SUCCESS,NULL);

            sd_dma_interrupt_context.sd_current_write_read_block_num = 0;
            sd_information.is_busy = false;

            msdc_nvic_set(false);

            return HAL_SD_STATUS_OK;
        }
    }

        /*card removed*/
        goto error;
error:

    sd_dma_interrupt_error_handle(status);
    return HAL_SD_STATUS_ERROR;
    
}
#ifdef HAL_SLEEP_MANAGER_ENABLED
void sd_backup_all_register(hal_sd_port_t sd_port)
{
    sd_backup_register.MSDC_CFG     = MSDC_REG->MSDC_CFG;
    sd_backup_register.MSDC_IOCON   = MSDC_REG->MSDC_IOCON;
    sd_backup_register.MSDC_INTEN  = MSDC_REG->MSDC_INTEN;
    sd_backup_register.SDC_CFG      = MSDC_REG->SDC_CFG;
}


void sd_restore_all_register(hal_sd_port_t sd_port)
{
    /*must be enable SD first*/
    MSDC_REG->MSDC_CFG = MSDC_REG->MSDC_CFG | MSDC_CFG_MODE;

    MSDC_REG->MSDC_CFG     = sd_backup_register.MSDC_CFG;
    MSDC_REG->MSDC_IOCON   = sd_backup_register.MSDC_IOCON;
    MSDC_REG->MSDC_INTEN   = sd_backup_register.MSDC_INTEN;
    MSDC_REG->SDC_CFG      = sd_backup_register.SDC_CFG;
}


void sd_backup_all(void *data)
{
    if (!sd_information.is_initialized) {
        sd_backup_parameter.is_initialized = false;
        return;
    } else {
        sd_backup_parameter.is_initialized = true;
    }

    sd_backup_all_register(sd_backup_parameter.sd_port);
}

void sd_restore_all(void *data)
{
    if (!sd_backup_parameter.is_initialized) {
        return;
    }

    sd_restore_all_register(sd_backup_parameter.sd_port);
    sd_backup_parameter.is_initialized = false;

    /*call once read make MSDC enter transfer state*/
    hal_sd_read_blocks(sd_backup_parameter.sd_port, sd_read_buffer, 0, 1);
}

#endif /*HAL_SLEEP_MANAGER_ENABLED*/
#endif /*HAL_SD_MODULE_ENABLED*/

