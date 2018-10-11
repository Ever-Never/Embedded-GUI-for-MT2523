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

#include "bl_msdc.h"
#include "bl_sd.h"
#include "bl_sd_internal.h"
#include "stdio.h"
#include "bl_common.h"


#ifdef BL_SD_MODULE_ENABLED


extern sd_information_t sd_information;
extern msdc_config_t msdc_config[];
extern uint32_t sd_csd[];


/*This API only used for SD card*/
bl_sd_status_t bl_sd_set_bus_width(bl_sd_port_t sd_port, bl_sd_bus_width_t bus_width)
{
    sd_internal_status_t status;
    uint32_t command6_argment;
    msdc_register_t *msdc_register_base;
    uint32_t argument = 0;

    msdc_register_base = (BL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    if (sd_information.card_type == BL_SD_TYPE_SD_CARD       ||
            sd_information.card_type == BL_SD_TYPE_SD20_LCS_CARD ||
            sd_information.card_type == BL_SD_TYPE_SD20_HCS_CARD) {

        /*check whether sd card support 4 bit bus*/
        if (bus_width == BL_SD_BUS_WIDTH_4 && !(sd_information.scr.bus_width & (1 << BL_SD_BUS_WIDTH_4))) {
            status = ERROR_NOT_SUPPORT_4BITS;
            goto error;
        }

        status = sd_send_command55(sd_port, sd_information.rca);
        if (status != NO_ERROR) {
            goto error;
        }

        command6_argment = (bus_width == BL_SD_BUS_WIDTH_4) ? COMMAND6_BUS_WIDTH_4 : COMMAND6_BUS_WIDTH_1;
        status = sd_send_command(sd_port, MSDC_COMMAND6, command6_argment);
        if (status != NO_ERROR) {
            goto error;
        }

        status = sd_check_card_status(sd_port);
        if (status != NO_ERROR) {
            goto error;
        }

        if (bus_width == BL_SD_BUS_WIDTH_4) {
            msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG | SDC_CFG_MDLEN_MASK;
            sd_information.bus_width = BL_SD_BUS_WIDTH_4;
        } else {
            msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG & (~SDC_CFG_MDLEN_MASK);
        }
    } else if (sd_information.card_type == BL_SD_TYPE_MMC_CARD ||
               sd_information.card_type == BL_SD_TYPE_MMC42_CARD) {

        if (bus_width == BL_SD_BUS_WIDTH_4) {
            argument = (EXT_CSD_ACCESS_MODE_WRITE_BYTE << MMC_COMMAND6_ACCESS_BIT_SHIFT) |
                       (EXT_CSD_BUS_WIDTH_INDEX << MMC_COMMAND6_INDEX_BIT_SHIFT)          |
                       (MMC_BUS_WIDTH_4 << MMC_COMMAND6_VALUE_BIT_SHIFT)                  |
                       (0);
        } else if (bus_width == BL_SD_BUS_WIDTH_1) {
            argument = (EXT_CSD_ACCESS_MODE_WRITE_BYTE << MMC_COMMAND6_ACCESS_BIT_SHIFT) |
                       (EXT_CSD_BUS_WIDTH_INDEX << MMC_COMMAND6_INDEX_BIT_SHIFT)          |
                       (MMC_BUS_WIDTH_1 << MMC_COMMAND6_VALUE_BIT_SHIFT)                  |
                       (0);
        }

        /*because need read ext_csd after switch to bus width 4 in api mmc_switch, so need change msdc bus config to 4 bit frist */
        if (bus_width == BL_SD_BUS_WIDTH_4) {
            msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG | SDC_CFG_MDLEN_MASK;
            sd_information.bus_width = BL_SD_BUS_WIDTH_4;
        } else {
            msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG & (~SDC_CFG_MDLEN_MASK);
        }

        if (sd_information.csd.ext_csd->ext_csd_rev >= MMC_EXTENDED_CSD_VERSION_441) {
            status = mmc_switch(sd_port, argument);
            if (NO_ERROR != status) {
                goto error;
            }
        }
    }

    return BL_SD_STATUS_OK;

error:

    sd_information.error_status = status;
    bl_print(LOG_DEBUG, "sd error status = %d\r\n", status);
    return BL_SD_STATUS_ERROR;
}

bl_sd_status_t bl_sd_init(bl_sd_port_t sd_port, bl_sd_config_t *sd_config)
{
    msdc_register_t *msdc_register_base;
    sd_internal_status_t status;

    if (NULL == sd_config) {
        bl_print(LOG_DEBUG, "parameter error\r\n");
        return BL_SD_STATUS_ERROR;
    }

    msdc_register_base = (BL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;


    /*cofirm whether MSDC have used by SDIO*/
    if (MSDC_INITIALIZED == msdc_init((msdc_port_t)sd_port)) {
        if (MSDC_OWNER_SDIO == msdc_get_owner((msdc_port_t)sd_port)) {
            bl_print(LOG_DEBUG, "msdc_%d used by SDIO\r\n", sd_port);
            return BL_SD_STATUS_ERROR;
        }
    }

    /*disable MSDC DMA*/
    msdc_dma_disable((msdc_port_t)sd_port);

    msdc_card_power_set((msdc_port_t)sd_port, MSDC_FALSE);

    msdc_wait(1);

    msdc_card_power_set((msdc_port_t)sd_port, MSDC_TRUE);

    /*save MSDC owner*/
    msdc_set_owner((msdc_port_t)sd_port, MSDC_OWNER_SD);

    msdc_reset((msdc_port_t)sd_port);

    msdc_clock_init((msdc_port_t)sd_port);

    /*disable 4 bit mode*/
    msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG & (~SDC_CFG_MDLEN_MASK);

    /*init global structure*/
    memset(&sd_information, 0, sizeof(sd_information_t));
    sd_information.block_length = SD_BLOCK_LENGTH;
    sd_information.rca = 0;
    sd_information.is_inactive = MSDC_FALSE;
    sd_information.sd_state = IDLE_STA;
    sd_information.bus_width = BL_SD_BUS_WIDTH_1;
    sd_information.is_initialized = MSDC_FALSE;

    /*for debugging,enable serial clock*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_SCLKON_MASK;

    msdc_wait(1);

    /*disable serial clock*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG & (~MSDC_CFG_SCLKON_MASK);

    /*send CMD0*/
    status = sd_reset(sd_port);
    if (NO_ERROR != status) {
        goto error;
    }

    /*send CMD8 and ADMD41 for SD, send CMD1 for eMMC*/
    if (sd_check_card_type(sd_port) == BL_SD_TYPE_UNKNOWN_CARD) {
        status = ERROR_INVALID_CARD;
        goto error;
    } else {
        /*need reset error status because error status changed in sd_check_card_type*/
        sd_information.error_status = BL_SD_STATUS_OK;
    }

    /*senc CMD2*/
    status = sd_get_card_id(sd_port);
    if (NO_ERROR != status) {
        goto error;
    }

    /*send CMD3*/
    status = sd_get_rca(sd_port);
    if (NO_ERROR != status) {
        goto error;
    }

    /*send CMD9*/
    status = sd_get_csd(sd_port);
    if (NO_ERROR != status) {
        goto error;
    }

    if (sd_information.csd.dsr_imp) {
        status = sd_set_dsr(sd_port);
        if (NO_ERROR != status) {
            goto error;
        }
    }

    /*send CMD7*/
    status = sd_select_card(sd_port, sd_information.rca);
    if (status == ERROR_CARD_IS_LOCKED) {
        sd_information.is_locked = MSDC_TRUE;
    } else if (NO_ERROR != status) {
        goto error;
    }

    msdc_set_output_clock((msdc_port_t)sd_port, MSDC_OUTPUT_CLOCK);

    if (sd_information.card_type == BL_SD_TYPE_SD_CARD       ||
            sd_information.card_type == BL_SD_TYPE_SD20_LCS_CARD ||
            sd_information.card_type == BL_SD_TYPE_SD20_HCS_CARD) {
        /*
           if (msdc_register_base->SDC_STA & SDC_STA_WP_MASK) {
               sd_information.is_write_protection = MSDC_TRUE;
           }*/


        status = sd_get_scr(sd_port);
        if (NO_ERROR != status) {
            goto error;
        }

        if (BL_SD_STATUS_OK != bl_sd_set_bus_width(sd_port, sd_config->bus_width)) {
            msdc_deinit((msdc_port_t)sd_port);
            return BL_SD_STATUS_ERROR;
        }

        status = sd_acommand42(sd_port);
        if (NO_ERROR != status) {
            goto error;
        }

        if (sd_information.scr.spec_ver > SD_SPECIFICATION_101) {
            status = sd_select_high_speed(sd_port);
            if (NO_ERROR != status) {
                /*if card not support high speed, the max speed is 25MHZ*/
                sd_information.is_high_speed = MSDC_FALSE;
            }
        }
    } else if (sd_information.card_type == BL_SD_TYPE_MMC_CARD ||
               sd_information.card_type == BL_SD_TYPE_MMC42_CARD) {

        status = mmc_set_high_speed(sd_port);
        if (NO_ERROR != status) {
            goto error;
        }
        if (BL_SD_STATUS_OK != bl_sd_set_bus_width(sd_port, sd_config->bus_width)) {
            msdc_deinit((msdc_port_t)sd_port);
            return BL_SD_STATUS_ERROR;
        }


        /*ERASE_GROUP_DEF,if set to 1, the erasable unit of MMC card is defined in EXT_CSD,otherwise defined in CSD*/
        /*        argument = (EXT_CSD_ACCESS_MODE_WRITE_BYTE << MMC_COMMAND6_ACCESS_BIT_SHIFT) |
                            (EXT_CSD_ERASE_GROUP_INDEX << MMC_COMMAND6_INDEX_BIT_SHIFT)         |
                            (1 << MMC_COMMAND6_VALUE_BIT_SHIFT)                                 |
                            (0);

                if (sd_information.csd.ext_csd->ext_csd_rev >= MMC_EXTENDED_CSD_VERSION_441) {
                    status = mmc_switch(sd_port, argument);
                    if (NO_ERROR != status) {
                        goto error;
                    }
                }
                */
    }

    /*hal_sd_clock_adaptation();*/


    if (sd_information.card_type == BL_SD_TYPE_MMC42_CARD) {
        sd_information.csd.capacity = (uint64_t)sd_information.csd.ext_csd->sec_count << 9; /*sec_cout * 512byte*/
    }

    status = sd_set_block_length(sd_port, SD_BLOCK_LENGTH);
    if (NO_ERROR != status) {
        return BL_SD_STATUS_ERROR;
    }

    if ((!sd_information.is_high_speed) && (sd_config->clock > SD_DEFAULT_SPEED_MAX)) {
        msdc_set_output_clock((msdc_port_t)sd_port, SD_DEFAULT_SPEED_MAX);
    } else if (sd_config->clock > SD_HIGH_SPEED_MAX) {
        msdc_set_output_clock((msdc_port_t)sd_port, SD_HIGH_SPEED_MAX);
    } else {
        msdc_set_output_clock((msdc_port_t)sd_port, sd_config->clock);
    }

    sd_information.is_initialized = MSDC_TRUE;
    return BL_SD_STATUS_OK;

error:
    memset(&sd_information, 0, sizeof(sd_information_t));
    sd_information.block_length = SD_BLOCK_LENGTH;
    sd_information.rca = 0;
    sd_information.is_inactive = MSDC_FALSE;
    sd_information.sd_state = IDLE_STA;
    sd_information.bus_width = BL_SD_BUS_WIDTH_1;
    sd_information.is_initialized = MSDC_FALSE;

    sd_information.error_status = status;
    bl_print(LOG_DEBUG, "sd error status = %d \r\n", status);
    msdc_deinit((msdc_port_t)sd_port);

    return BL_SD_STATUS_ERROR;
}


bl_sd_status_t bl_sd_deinit(bl_sd_port_t sd_port)
{
    msdc_reset((msdc_port_t)sd_port);

    memset(&sd_information, 0, sizeof(sd_information_t));

    msdc_deinit((msdc_port_t)sd_port);

    return BL_SD_STATUS_OK;
}


bl_sd_status_t bl_sd_get_capacity(bl_sd_port_t sd_port, uint64_t *capacity)
{

    if (NULL == capacity) {
        bl_print(LOG_DEBUG, "parameter error.\r\n");
        return BL_SD_STATUS_ERROR;
    }

    if (MSDC_FALSE == sd_information.is_initialized) {
        bl_print(LOG_DEBUG, "card not initialize.\r\n");
        return BL_SD_STATUS_ERROR;
    }

    if ((0 != sd_information.csd.capacity) && (sd_information.is_initialized == MSDC_TRUE)) {
        *capacity = sd_information.csd.capacity;
        return BL_SD_STATUS_OK;
    } else {
        return BL_SD_STATUS_ERROR;
    }
}

/*for SD card densities greater than 2GB, start_sector unit is write block size, otherwise the unit is byte, and should write block size align.
  for MMC card densities greater than 2GB, start_sector unit is erase group size, otherwise the unit is byte, and should erase group size align*/
bl_sd_status_t bl_sd_erase_sectors(bl_sd_port_t sd_port, uint32_t start_sector, uint32_t sector_number)
{

    sd_internal_status_t status;
    uint32_t sector_multiplier;
    uint32_t start_address_command;
    uint32_t end_address_command;

    /*data address is in byte units in a SD1.0 memory card and in block(512byte)  units in a High Capacity SD memory card.
    for eMMC, data address for densities =< 2GB is a 32bit byte address and data address densities > 2GB is a 32bit erase group size address.*/
    if ((sd_information.card_type == BL_SD_TYPE_SD20_HCS_CARD) || (sd_information.card_type == BL_SD_TYPE_MMC42_CARD)) {
        sector_multiplier = 1;
    } else if (sd_information.card_type == BL_SD_TYPE_MMC_CARD) {
        /*for emmc, erase group size is mutil blocks*/
        sector_multiplier = sd_information.csd.erase_grp_size_mmc >> 9;
    } else {
        sector_multiplier = SD_BLOCK_LENGTH;
    }

    /*set erase start address command and erase end address command adopt*/
    if ((sd_information.card_type == BL_SD_TYPE_SD20_HCS_CARD) ||
            (sd_information.card_type == BL_SD_TYPE_SD20_LCS_CARD) ||
            (sd_information.card_type == BL_SD_TYPE_SD_CARD)) {
        start_address_command = MSDC_COMMAND32;
        end_address_command = MSDC_COMMAND33;
    } else {
        start_address_command = MSDC_COMMAND35_MMC;
        end_address_command = MSDC_COMMAND36_MMC;
    }

    /*send erase start sector address*/
    status = sd_send_erase_command(sd_port, start_address_command, sector_multiplier * start_sector);
    if (NO_ERROR != status) {
        goto error;
    }

    /*send erase end sector address*/
    status = sd_send_erase_command(sd_port, end_address_command, sector_multiplier * (start_sector + sector_number - 1));
    if (NO_ERROR != status) {
        goto error;
    }

    /*start erase*/
    status = sd_send_erase_command(sd_port, MSDC_COMMAND38, COMMAND_NO_ARGUMENT);
    if (NO_ERROR != status) {
        goto error;
    }

    return BL_SD_STATUS_OK;

error:

    sd_information.error_status = status;
    msdc_power_set((msdc_port_t)sd_port, MSDC_FALSE);

    bl_print(LOG_DEBUG, "sd error status = %d \r\n", status);


    return BL_SD_STATUS_ERROR;
}


bl_sd_status_t bl_sd_read_blocks(bl_sd_port_t sd_port, uint32_t *read_buffer,  uint32_t read_address, uint32_t block_number)
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
        bl_print(LOG_DEBUG, "parameter error\r\n");
        return BL_SD_STATUS_ERROR;
    }

    msdc_register_base = (BL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;


    msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_FIFOTHD_MASK)) |
                                   (MSDC_FIFO_THRESHOLD_1  << MSDC_CFG_FIFOTHD_OFFSET);

    msdc_register_base->MSDC_IOCON = (msdc_register_base->MSDC_IOCON & (~MSDC_IOCON_DMABURST_MASK)) |
                                     (MSDC_DMA_SINGLE_MODE << MSDC_IOCON_DMABURST_OFFSET);

    /*read clear to make sure we will read latest one in the future*/
    msdc_register_base->SDC_DATSTA;


    /*data address is in byte units in a SD1.0 memory card and in block(512byte)  units in a High Capacity SD memory card.
      for eMMC, data address for densities =< 2GB is a 32bit byte address and data address densities > 2GB is a 32bit erase group size address.*/
    if ((sd_information.card_type == BL_SD_TYPE_SD20_HCS_CARD) || (sd_information.card_type == BL_SD_TYPE_MMC42_CARD)) {
        block_multiplier = 1;
    } else if (sd_information.card_type == BL_SD_TYPE_MMC_CARD) {
        /*for emmc, erase group size is mutil blocks*/
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

    read_word_count = (uint64_t)((block_number * SD_BLOCK_LENGTH) >> 2);

    while (index < read_word_count) {

        data_status = msdc_register_base->SDC_DATSTA;
        if (data_status >> 1) {
            bl_print(LOG_DEBUG, "bl_sd_read_blocks, SDC_DATSTA = %x \r\n", (unsigned int)data_status);
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

    return BL_SD_STATUS_OK;

error:

    sd_information.error_status = status;
    bl_print(LOG_DEBUG, "sd error status = %d \r\n", status);
    msdc_reset((msdc_port_t)sd_port);
    msdc_register_base->MSDC_INT;
    if (1 < block_number) {
        sd_stop_transfer(sd_port);
    }

    return BL_SD_STATUS_ERROR;
}


bl_sd_status_t bl_sd_write_blocks(bl_sd_port_t sd_port, const uint32_t *write_buffer,  uint32_t write_address, uint32_t block_number)
{
    sd_internal_status_t status;
    uint32_t write_command;
    uint32_t index = 0;
    uint64_t write_word_count = 0;
    uint32_t response;
    uint32_t block_multiplier;
    msdc_register_t *msdc_register_base;
    uint32_t data_status;

    if (NULL == write_buffer) {
        bl_print(LOG_DEBUG, "parameter error\r\n");
        return BL_SD_STATUS_ERROR;
    }

    msdc_register_base = (BL_SD_PORT_0 == sd_port) ? MSDC0_REG : MSDC1_REG;

    msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_FIFOTHD_MASK)) |
                                   (MSDC_FIFO_THRESHOLD_1  << MSDC_CFG_FIFOTHD_OFFSET);

    msdc_register_base->MSDC_IOCON = (msdc_register_base->MSDC_IOCON & (~MSDC_IOCON_DMABURST_MASK)) |
                                     (MSDC_DMA_SINGLE_MODE << MSDC_IOCON_DMABURST_OFFSET);

    msdc_reset((msdc_port_t)sd_port);

    if (sd_information.is_write_protection) {
        status = ERROR_WRITE_PROTECT;
        goto error;
    }

    /*data address is in byte units in a SD1.0 memory card and in block(512byte)  units in a High Capacity SD memory card.
      for eMMC, data address for densities =< 2GB is a 32bit byte address and data address densities > 2GB is a 32bit erase group size address.*/
    if ((sd_information.card_type == BL_SD_TYPE_SD20_HCS_CARD) || (sd_information.card_type == BL_SD_TYPE_MMC42_CARD)) {
        block_multiplier = 1;
    } else if (sd_information.card_type == BL_SD_TYPE_MMC_CARD) {
        /*for emmc, erase group size is mutil blocks*/

        block_multiplier = sd_information.csd.read_bl_len;
    } else {
        block_multiplier = SD_BLOCK_LENGTH;
    }

    write_command = (block_number > 1) ? MSDC_COMMAND25 : MSDC_COMMAND24;
    status = sd_send_command(sd_port, write_command, (write_address * block_multiplier));
    if (status != NO_ERROR) {
        goto error;
    }

    status = sd_check_card_status(sd_port);
    if (status != NO_ERROR) {
        goto error;
    }

    write_word_count = (uint64_t)((block_number * SD_BLOCK_LENGTH) >> 2);

    while (index < write_word_count) {

        data_status = msdc_register_base->SDC_DATSTA;
        if (data_status >> 1) {
            bl_print(LOG_DEBUG, "bl_sd_write_blocks, SDC_DATSTA = %x \r\n", (unsigned int)data_status);
            if (data_status & SDC_DATSTA_DATTO_MASK) {
                status = ERROR_DATA_TIMEOUT;
                goto error;
            } else if (data_status & SDC_DATSTA_DATCRCERR_MASK) {
                status = ERROR_DATA_CRC_ERROR;
                goto error;
            }
        }

        if (msdc_register_base->MSDC_STA & MSDC_STA_DRQ_MASK) {
            msdc_register_base->MSDC_DAT = *(write_buffer + index);
            index++;
        }

    }

    if (1 == block_number) {
        status = sd_wait_card_not_busy(sd_port);
        if (status != NO_ERROR) {
            goto error;
        }
    }

    status = sd_wait_data_ready(sd_port);
    if (status != NO_ERROR) {
        goto error;
    }

    if (1 < block_number) {
        /*for mutil block write, this API must be called*/
        status = sd_wait_last_block_complete(sd_port);
        if (status != NO_ERROR) {
            goto error;
        }

        status = sd_stop_transfer(sd_port);
        if (status != NO_ERROR) {
            goto error;
        }
    } else if (1 ==  block_number) {
        status = sd_wait_card_not_busy(sd_port);
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

        /*check corresponds to buffer empty singaling on the bus*/
        if ((response & SD_CARD_STATUS_READ_FOR_DATA_BIT_MASK) &&
                ((response & SD_CARD_STATUS_STATE_BIT_MASK) >> SD_CARD_STATUS_STATE_BIT_SHIFT) == TRAN_STA) {
            break;
        }
    }


    msdc_register_base->MSDC_INT;
    msdc_reset((msdc_port_t)sd_port);

    return BL_SD_STATUS_OK;

error:

    sd_information.error_status = status;
    bl_print(LOG_DEBUG, "sd error status = %d \r\n", status);
    msdc_reset((msdc_port_t)sd_port);
    msdc_register_base->MSDC_INT;
    if (1 < block_number) {
        sd_stop_transfer(sd_port);
    }

    return BL_SD_STATUS_ERROR;
}


bl_sd_status_t bl_sd_read_blocks_dma(bl_sd_port_t sd_port, uint32_t *read_buffer, uint32_t read_address, uint32_t block_number)
{
    return BL_SD_STATUS_OK;
}


bl_sd_status_t bl_sd_write_blocks_dma(bl_sd_port_t sd_port, const uint32_t *write_buffer,  uint32_t write_address, uint32_t block_number)
{
    return BL_SD_STATUS_OK;
}



bl_sd_status_t bl_sd_set_clock(bl_sd_port_t sd_port, uint32_t clock)
{
    return BL_SD_STATUS_OK;
}



bl_sd_status_t bl_sd_get_clock(bl_sd_port_t sd_port, uint32_t *clock)
{
    return BL_SD_STATUS_OK;
}


bl_sd_status_t bl_sd_get_card_type(bl_sd_port_t sd_port, bl_sd_card_type_t *card_type)
{
    if (NULL == card_type) {
        bl_print(LOG_DEBUG, "parameter error\r\n");
        return BL_SD_STATUS_ERROR;
    }

    if (MSDC_FALSE == sd_information.is_initialized) {
        bl_print(LOG_DEBUG, "card not initialize.\r\n");
        return BL_SD_STATUS_ERROR;
    }

    *card_type = sd_information.card_type;

    return BL_SD_STATUS_OK;
}



bl_sd_status_t bl_sd_get_ocr(bl_sd_port_t sd_port, uint32_t *ocr)
{
    if (NULL == ocr) {
        bl_print(LOG_DEBUG, "parameter error\r\n");
        return BL_SD_STATUS_ERROR;
    }

    if (MSDC_FALSE == sd_information.is_initialized) {
        bl_print(LOG_DEBUG, "card not initialize.\r\n");
        return BL_SD_STATUS_ERROR;
    }

    *ocr = sd_information.sd_ocr;

    return BL_SD_STATUS_OK;
}


bl_sd_status_t bl_sd_get_card_status(bl_sd_port_t sd_port, uint32_t *card_status)
{
    return BL_SD_STATUS_OK;
}

bl_sd_status_t bl_sd_get_erase_sector_size(bl_sd_port_t sd_port, uint32_t *erase_sector_size)
{
    if (NULL == erase_sector_size) {
        bl_print(LOG_DEBUG, "parameter error\r\n");
        return BL_SD_STATUS_ERROR;
    }

    if (MSDC_FALSE == sd_information.is_initialized) {
        bl_print(LOG_DEBUG, "card not initialize.\r\n");
        return BL_SD_STATUS_ERROR;
    }

    if ((sd_information.card_type == BL_SD_TYPE_MMC_CARD) || (sd_information.card_type == BL_SD_TYPE_MMC42_CARD)) {
        *erase_sector_size = sd_information.csd.erase_grp_size_mmc;
    } else if ((sd_information.card_type == BL_SD_TYPE_SD20_HCS_CARD) ||
               (sd_information.card_type == BL_SD_TYPE_SD20_LCS_CARD) ||
               (sd_information.card_type == BL_SD_TYPE_SD_CARD)) {
        *erase_sector_size = sd_information.csd.write_bl_len;
    }

    return BL_SD_STATUS_OK;
}

bl_sd_status_t bl_sd_get_cid(bl_sd_port_t sd_port, uint32_t *cid)
{
    if (NULL == cid) {
        bl_print(LOG_DEBUG, "parameter error\r\n");
        return BL_SD_STATUS_ERROR;
    }

    if (MSDC_FALSE == sd_information.is_initialized) {
        bl_print(LOG_DEBUG, "card not initialize.\r\n");
        return BL_SD_STATUS_ERROR;
    }

    *(cid + 0) = sd_information.cid[0];
    *(cid + 1) = sd_information.cid[1];
    *(cid + 2) = sd_information.cid[2];
    *(cid + 3) = sd_information.cid[3];

    return BL_SD_STATUS_OK;
}

bl_sd_status_t bl_sd_get_csd(bl_sd_port_t sd_port, uint32_t *csd)
{
    if (NULL == csd) {
        bl_print(LOG_DEBUG, "parameter error\r\n");
        return BL_SD_STATUS_ERROR;
    }

    if (MSDC_FALSE == sd_information.is_initialized) {
        bl_print(LOG_DEBUG, "card not initialize.\r\n");
        return BL_SD_STATUS_ERROR;
    }

    *(csd + 0) = sd_csd[0];
    *(csd + 1) = sd_csd[1];
    *(csd + 2) = sd_csd[2];
    *(csd + 3) = sd_csd[3];

    return BL_SD_STATUS_OK;
}

#endif /*BL_SD_MODULE_ENABLED*/

