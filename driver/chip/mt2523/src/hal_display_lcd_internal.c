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

#include "hal_platform.h"

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED

#include "hal_display_lcd_internal.h"
#include "hal_display_lcd.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_gpt.h"
#include "stdio.h"
#include "string.h"

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
#include "hal_display_dsi_internal.h"
extern LCD_DSI_REGISTER_T *lcd_dsi_register_ptr;
#endif

#ifdef __cplusplus
extern "C" {
#endif

hal_display_lcd_interface_timing_t lcd_if_time_para;
hal_display_lcd_te_timing_para_t lcd_te_time_para;
hal_display_lcd_interface_mode_t lcd_if_mode_para;
hal_display_lcd_interface_2_data_lane_t lcd_if_2_dana_lane_para;
hal_display_lcd_layer_input_t layer_struct[4] = {{0}};
hal_display_lcd_roi_output_t roi_struct = {0};
uint32_t backup_index_table[16];

volatile LCD_REGISTER_T	*lcd_register_ptr = (LCD_REGISTER_T *)(LCD_BASE);

hal_display_lcd_status_t display_lcd_init(uint32_t main_command_address, uint32_t main_data_addressess, uint32_t main_lcd_output_format)
{
    uint32_t i;

    // reset settings
    // Clear register
    lcd_register_ptr->lcd_inten_register.value = 0;
    lcd_register_ptr->lcd_wroiofs_register.value = 0;
    lcd_register_ptr->lcd_wroisize_register.value = 0;
    lcd_register_ptr->lcd_wroicon_register.value = 0;
    lcd_register_ptr->lcd_wroicadd_register.value = 0;
    lcd_register_ptr->lcd_wroidadd_register.value = 0;

    // Reset HW layer register
    for (i = 0; i < LCD_TOTAL_LAYER; i++) {
        lcd_register_ptr->lcd_layer_register[i].lcd_lwincon_register.value = 0;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinkey_register.value = 0;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinofs_register.value = 0;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinadd_register.value = 0;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinsize_register.value = 0;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinmofs_register.value = 0;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinpitch_register.value = 0;
    }

    DISABLE_LCD_TE_DETECT;
    SET_LCD_ROI_COMMAND_ADDR(main_command_address);
    SET_LCD_ROI_DATA_ADDR(main_data_addressess);
    SET_LCD_ROI_CTRL_OUTPUT_FORMAT(main_lcd_output_format);

    *((volatile uint32_t *)(0xA2090040)) &= 0xFFFFFFFE;		// Disable MIPI TX BG Core power temp
    *((volatile uint32_t *)(0xA0480010)) &= 0xFFFFFFFE;		// data path set to DBI

    if (0 != lcd_register_ptr->lcd_wroicadd_register.value) {	// Initialize fail, the data can't be set in to register
        return HAL_DISPLAY_LCD_STATUS_INITIALIZATION_FAILED;
    } else {
        return HAL_DISPLAY_LCD_STATUS_OK;
    }
}

hal_display_lcd_status_t display_lcd_set_interface_timing(hal_display_lcd_interface_timing_t para)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    lcd_if_time_para = para;

    display_lcd_restore_if_settings();

    return ret;
}

hal_display_lcd_status_t display_lcd_set_interface_mode(hal_display_lcd_interface_mode_t para)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    lcd_if_mode_para = para;
    if (HAL_DISPLAY_LCD_INTERFACE_SERIAL_0 == para.port_number) {
        lcd_register_ptr->lcd_sif_con_register.field.SIF0_3WIRE = para.three_wire_mode;
        lcd_register_ptr->lcd_sif_con_register.field.SIF0_SDI = para.read_from_SDI;
        lcd_register_ptr->lcd_sif_con_register.field.SIF0_SIZE = para.width;
        lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_SINGLE_A0 = para.single_a0_mode;
        lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_CS_STAY_LOW = para.cs_stay_low_mode;
        *((volatile uint32_t *)(0xA2020824)) &= 0xFF000FFF;
        switch (para.driving_current) {
            case HAL_DISPLAY_LCD_DRIVING_CURRENT_4MA:
                *((volatile uint32_t *)(0xA2020824)) |= 0;
                break;
            case HAL_DISPLAY_LCD_DRIVING_CURRENT_8MA:
                *((volatile uint32_t *)(0xA2020824)) |= 0x555000;
                break;
            case HAL_DISPLAY_LCD_DRIVING_CURRENT_12MA:
                *((volatile uint32_t *)(0xA2020824)) |= 0xAAA000;
                break;
            case HAL_DISPLAY_LCD_DRIVING_CURRENT_16MA:
                *((volatile uint32_t *)(0xA2020824)) |= 0xFFF000;
                break;
            default:
                break;
        }
    } else {
        lcd_register_ptr->lcd_sif_con_register.field.SIF1_3WIRE = para.three_wire_mode;
        lcd_register_ptr->lcd_sif_con_register.field.SIF1_SDI = para.read_from_SDI;
        lcd_register_ptr->lcd_sif_con_register.field.SIF1_SIZE = para.width;
        lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_SINGLE_A0 = para.single_a0_mode;
        lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_CS_STAY_LOW = para.cs_stay_low_mode;
        *((volatile uint32_t *)(0xA2020814)) &= 0xF000FFFF;
        switch (para.driving_current) {
            case HAL_DISPLAY_LCD_DRIVING_CURRENT_4MA:
                *((volatile uint32_t *)(0xA2020814)) |= 0;
                break;
            case HAL_DISPLAY_LCD_DRIVING_CURRENT_8MA:
                *((volatile uint32_t *)(0xA2020814)) |= 0x5550000;
                break;
            case HAL_DISPLAY_LCD_DRIVING_CURRENT_12MA:
                *((volatile uint32_t *)(0xA2020814)) |= 0xAAA0000;
                break;
            case HAL_DISPLAY_LCD_DRIVING_CURRENT_16MA:
                *((volatile uint32_t *)(0xA2020814)) |= 0xFFF0000;
                break;
            default:
                break;
        }
    }

    if (para.hw_cs == 1) {
        ENABLE_LCD_SERIAL_IF_HW_CS;
    } else {
        DISABLE_LCD_SERIAL_IF_HW_CS;
    }

    return ret;
}

hal_display_lcd_status_t display_lcd_set_interface_2data_lane_mode(hal_display_lcd_interface_port_t port, bool flag, hal_display_lcd_interface_2pin_width_t two_data_width)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    lcd_if_2_dana_lane_para.port_number = port;
    lcd_if_2_dana_lane_para.two_data_flag = flag;
    lcd_if_2_dana_lane_para.two_data_width = two_data_width;

    if (HAL_DISPLAY_LCD_INTERFACE_SERIAL_0 == port) {
        lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_PIX_2PIN = flag;
        lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_2PIN_SIZE = two_data_width;
    } else {
        lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_PIX_2PIN = flag;
        lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_2PIN_SIZE = two_data_width;
    }

    return ret;
}

hal_display_lcd_status_t display_lcd_restore_if_settings(void)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    SET_LCD_SERIAL_IF_CSS(lcd_if_time_para.port_number, lcd_if_time_para.css);
    SET_LCD_SERIAL_IF_CSH(lcd_if_time_para.port_number, lcd_if_time_para.csh);
    SET_LCD_SERIAL_IF_RD_1ST(lcd_if_time_para.port_number, lcd_if_time_para.rd_low);
    SET_LCD_SERIAL_IF_RD_2ND(lcd_if_time_para.port_number, lcd_if_time_para.rd_high);
    SET_LCD_SERIAL_IF_WR_1ST(lcd_if_time_para.port_number, lcd_if_time_para.wr_low);
    SET_LCD_SERIAL_IF_WR_2ND(lcd_if_time_para.port_number, lcd_if_time_para.wr_high);

    display_lcd_set_interface_mode(lcd_if_mode_para);
    display_lcd_set_interface_2data_lane_mode(lcd_if_2_dana_lane_para.port_number, lcd_if_2_dana_lane_para.two_data_flag, lcd_if_2_dana_lane_para.two_data_width);
    return ret;
}

hal_display_lcd_status_t display_lcd_config_dither(void)
{
    uint32_t dither_bit_r, dither_bit_g, dither_bit_b, temp;
    hal_display_lcd_output_color_format_t lcd_to_lcm_fmt;
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    GET_LCD_ROI_CTRL_OUTPUT_FORMAT(temp);
    lcd_to_lcm_fmt = (hal_display_lcd_output_color_format_t)((temp & 0x38) >> 3);

    dither_bit_r = 1;
    dither_bit_g = 1;
    dither_bit_b = 1;
    switch (lcd_to_lcm_fmt) {
        case HAL_DISPLAY_LCD_ROI_OUTPUT_RGB666:
            dither_bit_r = 1;
            dither_bit_g = 1;
            dither_bit_b = 1;
            break;
        case HAL_DISPLAY_LCD_ROI_OUTPUT_RGB565:
            dither_bit_r = 2;
            dither_bit_g = 1;
            dither_bit_b = 2;
            break;
        default:
            break;
    }

    SET_LCD_DITHER_BIT(dither_bit_r, dither_bit_g, dither_bit_b);

    return ret;
}

uint32_t display_lcd_GetFormatBpp(hal_display_lcd_layer_source_color_format_t layer_format)
{
    switch (layer_format) {
        case HAL_DISPLAY_LCD_LAYER_COLOR_8BPP_INDEX:
            return 1;

        case HAL_DISPLAY_LCD_LAYER_COLOR_RGB565:
            return 2;

        case HAL_DISPLAY_LCD_LAYER_COLOR_UYVY422:
            return 2;

        case HAL_DISPLAY_LCD_LAYER_COLOR_RGB888:
            return 3;

        case HAL_DISPLAY_LCD_LAYER_COLOR_ARGB6666:
            return 3;
        case HAL_DISPLAY_LCD_LAYER_COLOR_PARGB6666:
            return 3;

        case HAL_DISPLAY_LCD_LAYER_COLOR_ARGB8888:
            return 4;

        case HAL_DISPLAY_LCD_LAYER_COLOR_PARGB8888:
            return 4;

        default:
            //ASSERT(0);
            break;
    }

    return 2;
}


hal_display_lcd_status_t display_lcd_config_layer(hal_display_lcd_layer_input_t *layer_data)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;
    memcpy(&layer_struct[layer_data->layer_enable], layer_data, sizeof(hal_display_lcd_layer_input_t));

    return ret;
}

hal_display_lcd_status_t display_lcd_config_roi(hal_display_lcd_roi_output_t *roi_para)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;
    memcpy(&roi_struct, roi_para, sizeof(hal_display_lcd_roi_output_t));

    return ret;
}

hal_display_lcd_status_t display_lcd_config_start_byte(hal_display_lcd_interface_start_byte_mode_t *start_byte_para)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    return ret;
}

hal_display_lcd_status_t display_lcd_set_index_color_table(uint32_t *index_table)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;
    memcpy(&backup_index_table, index_table, sizeof(uint32_t)*16);

    return ret;
}

hal_display_lcd_status_t display_lcd_init_te(uint32_t frame_rate, uint32_t back_porch, uint32_t front_porch, uint32_t width, uint32_t height, uint32_t main_lcd_output)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;
    float ClkWidth, TransferCntPerPixel;
    uint32_t if_cs_no, IfWidth, lcmFmtBitCnt, transactionCycle, css, csh, wr_1st, wr_2nd, if_period;
    uint32_t div2 = 0;
    uint8_t b3wire, single_a0_mode, start_byte_mode, cs_stay_low_mode, b2_data_lane_mode;

    lcd_te_time_para.port_number = lcd_if_time_para.port_number;

    // Fill te parameter first
    if (lcd_te_time_para.port_number == HAL_DISPLAY_LCD_INTERFACE_SERIAL_0) {	// serial 0
        // Get clock count

        lcd_te_time_para.three_wire_mode = lcd_register_ptr->lcd_sif_con_register.field.SIF0_3WIRE;
        lcd_te_time_para.two_data_lane_mode = lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_PIX_2PIN;
        lcd_te_time_para.single_a0_mode = lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_SINGLE_A0;
        lcd_te_time_para.cs_stay_low_mode = lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_CS_STAY_LOW;
        lcd_te_time_para.start_byte_mode = lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF0_STR_BYTE_MOD;

        switch (lcd_register_ptr->lcd_sif_con_register.field.SIF0_SIZE) {
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_8:
                lcd_te_time_para.each_trans_length = 8;
                break;
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_9:
                lcd_te_time_para.each_trans_length = 9;
                break;
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_16:
                lcd_te_time_para.each_trans_length = 16;
                break;
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_18:
                lcd_te_time_para.each_trans_length = 18;
                break;
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_24:
                lcd_te_time_para.each_trans_length = 24;
                break;
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_32:
                lcd_te_time_para.each_trans_length = 32;
                break;
            default:
                //ASSERT(0);
                break;
        }

        if (lcd_te_time_para.two_data_lane_mode) {
            switch (lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_2PIN_SIZE) {
                case HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_12:
                    lcd_te_time_para.two_data_lane_each_trans_length = 12;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_16:
                    lcd_te_time_para.two_data_lane_each_trans_length = 16;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_18:
                    lcd_te_time_para.two_data_lane_each_trans_length = 18;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_24:
                    lcd_te_time_para.two_data_lane_each_trans_length = 24;
                    break;
                default:
                    //ASSERT(0);
                    break;
            }
        }

        if (lcd_te_time_para.start_byte_mode) {
            switch (lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF0_STR_DATA_SIZE) {
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_8:
                    lcd_te_time_para.str_byte_each_trans_length = 8;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_9:
                    lcd_te_time_para.str_byte_each_trans_length = 9;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_16:
                    lcd_te_time_para.str_byte_each_trans_length = 16;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_18:
                    lcd_te_time_para.str_byte_each_trans_length = 18;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_24:
                    lcd_te_time_para.str_byte_each_trans_length = 24;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_32:
                    lcd_te_time_para.str_byte_each_trans_length = 32;
                    break;
                default:
                    //ASSERT(0);
                    break;
            }

        }
    } else {	// serial 1
        /* Get clock count */

        lcd_te_time_para.three_wire_mode = lcd_register_ptr->lcd_sif_con_register.field.SIF1_3WIRE;
        lcd_te_time_para.two_data_lane_mode = lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_PIX_2PIN;
        lcd_te_time_para.single_a0_mode = lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_SINGLE_A0;
        lcd_te_time_para.cs_stay_low_mode = lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_CS_STAY_LOW;
        lcd_te_time_para.start_byte_mode = lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF1_STR_BYTE_MOD;

        switch (lcd_register_ptr->lcd_sif_con_register.field.SIF1_SIZE) {
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_8:
                lcd_te_time_para.each_trans_length = 8;
                break;
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_9:
                lcd_te_time_para.each_trans_length = 9;
                break;
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_16:
                lcd_te_time_para.each_trans_length = 16;
                break;
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_18:
                lcd_te_time_para.each_trans_length = 18;
                break;
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_24:
                lcd_te_time_para.each_trans_length = 24;
                break;
            case HAL_DISPLAY_LCD_INTERFACE_WIDTH_32:
                lcd_te_time_para.each_trans_length = 32;
                break;
            default:
                //ASSERT(0);
                break;
        }

        if (lcd_te_time_para.two_data_lane_mode) {
            switch (lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_2PIN_SIZE) {
                case HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_12:
                    lcd_te_time_para.two_data_lane_each_trans_length = 12;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_16:
                    lcd_te_time_para.two_data_lane_each_trans_length = 16;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_18:
                    lcd_te_time_para.two_data_lane_each_trans_length = 18;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_24:
                    lcd_te_time_para.two_data_lane_each_trans_length = 24;
                    break;
                default:
                    //ASSERT(0);
                    break;
            }
        }

        if (lcd_te_time_para.start_byte_mode) {
            switch (lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF1_STR_DATA_SIZE) {
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_8:
                    lcd_te_time_para.str_byte_each_trans_length = 8;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_9:
                    lcd_te_time_para.str_byte_each_trans_length = 9;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_16:
                    lcd_te_time_para.str_byte_each_trans_length = 16;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_18:
                    lcd_te_time_para.str_byte_each_trans_length = 18;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_24:
                    lcd_te_time_para.str_byte_each_trans_length = 24;
                    break;
                case HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_32:
                    lcd_te_time_para.str_byte_each_trans_length = 32;
                    break;
                default:
                    //ASSERT(0);
                    break;
            }

        }
    }

    b3wire = lcd_te_time_para.three_wire_mode;
    single_a0_mode = lcd_te_time_para.single_a0_mode;
    start_byte_mode = lcd_te_time_para.start_byte_mode;
    cs_stay_low_mode = lcd_te_time_para.cs_stay_low_mode;
    b2_data_lane_mode = lcd_te_time_para.two_data_lane_mode;
    if_cs_no = lcd_te_time_para.port_number;

    if (if_cs_no == HAL_DISPLAY_LCD_INTERFACE_SERIAL_0) {
        div2 = lcd_register_ptr->lcd_sif_con_register.field.SIF0_DIV2;
    } else if (if_cs_no == HAL_DISPLAY_LCD_INTERFACE_SERIAL_1) {
        div2 = lcd_register_ptr->lcd_sif_con_register.field.SIF1_DIV2;
    } else {
    }
    css = lcd_if_time_para.css * (div2 + 1) + 1;
    csh = lcd_if_time_para.csh * (div2 + 1) + 1;
    wr_1st = lcd_if_time_para.wr_low * (div2 + 1) + 1;
    wr_2nd = lcd_if_time_para.wr_high  * (div2 + 1) + 1;

    if_period = 2;
    if (b3wire) {
        if (b2_data_lane_mode) {
            IfWidth = lcd_te_time_para.two_data_lane_each_trans_length;
            if (single_a0_mode && !cs_stay_low_mode) { //when in single A0 mode, ignore the 1bit in fisrt byte tranfer
                transactionCycle = css + (wr_1st + wr_2nd) * (IfWidth / 2) + csh + if_period;
            } else if (!single_a0_mode && cs_stay_low_mode) {
                transactionCycle = (wr_1st + wr_2nd) * (IfWidth / 2 + 1) + if_period;
            } else if (single_a0_mode && cs_stay_low_mode) {
                transactionCycle = (wr_1st + wr_2nd) * (IfWidth / 2) + if_period;
            } else if (!single_a0_mode && !cs_stay_low_mode) {
                transactionCycle = css + (wr_1st + wr_2nd) * (IfWidth / 2 + 1) + csh + if_period;
            } else {
                //ASSERT(0); //unknown mode combination
            }
        } else {
            IfWidth = lcd_te_time_para.each_trans_length;
            if (single_a0_mode && !cs_stay_low_mode) {
                transactionCycle = css + (wr_1st + wr_2nd) * (IfWidth) + csh + if_period;
            } else if (!single_a0_mode && cs_stay_low_mode) {
                transactionCycle = (wr_1st + wr_2nd) * (IfWidth + 1) + if_period;
            } else if (single_a0_mode && cs_stay_low_mode) {
                transactionCycle = (wr_1st + wr_2nd) * (IfWidth) + if_period;
            } else if (!single_a0_mode && !cs_stay_low_mode) {
                transactionCycle = css + (wr_1st + wr_2nd) * (IfWidth + 1) + csh + if_period;
            } else {
                //ASSERT(0); //unknown mode combination
            }
        }
    } else {	// 4wire or start byte mode
        if (start_byte_mode) {
            IfWidth = lcd_te_time_para.str_byte_each_trans_length;
            if (cs_stay_low_mode) {
                transactionCycle = css + (wr_1st + wr_2nd) * (IfWidth) + csh;
            } else {
                transactionCycle = css + (wr_1st + wr_2nd) * (IfWidth) + csh + if_period + lcd_te_time_para.str_byte_each_trans_length;
            }
        } else {
            IfWidth = lcd_te_time_para.each_trans_length;
            if (cs_stay_low_mode) {
                transactionCycle = (wr_1st + wr_2nd) * (IfWidth) + if_period;
            } else {
                transactionCycle = css + (wr_1st + wr_2nd) * (IfWidth) + csh + if_period;
            }
            //TODO: for pixel trasfer pixel data IfWidth=8/9/16/18bits,note 24/32bits case
        }

    }

    lcd_te_time_para.transactionCycle = transactionCycle;

    ClkWidth = 1000000000 / (double)lcd_if_time_para.clock_freq;		//ns
    lcd_te_time_para.ClkWidth = ClkWidth;
    lcmFmtBitCnt = (main_lcd_output & 0x38) >> 3;

    switch (lcmFmtBitCnt) {
        case 0:
            lcmFmtBitCnt = 3 + 3 + 2;
            break;//RGB332
        case 1:
            lcmFmtBitCnt = 4 + 4 + 4;
            break;//RGB444
        case 2:
            lcmFmtBitCnt = 5 + 6 + 5;
            break;//RGB565
        case 3:
            lcmFmtBitCnt = 6 + 6 + 6;
            break;//RGB666
        case 4:
            lcmFmtBitCnt = 8 + 8 + 8;
            break;//RGB888
        default: //ASSERT(0);
            break;
    }

    TransferCntPerPixel = (float)lcmFmtBitCnt / (float)IfWidth;

    if (TransferCntPerPixel > 0.5f && TransferCntPerPixel <= 1.0f) {
        TransferCntPerPixel = 1.0f;
    } else if (TransferCntPerPixel > 1.0f && TransferCntPerPixel <= 1.5f) {
        TransferCntPerPixel = 1.5f;
    } else if (TransferCntPerPixel > 1.5f && TransferCntPerPixel <= 2.0f) {
        TransferCntPerPixel = 2.0f;
    } else if (TransferCntPerPixel > 2.0f && TransferCntPerPixel <= 3.0f) {
        TransferCntPerPixel = 3.0f;
    } else if (TransferCntPerPixel > 0.0f && TransferCntPerPixel <= 0.5f) {
        TransferCntPerPixel = 0.5f;
    } else
        //ASSERT(0);
    {
        lcd_te_time_para.TransferCntPerPixel = TransferCntPerPixel;
    }
    lcd_te_time_para.lcdPixelTransitionTime = TransferCntPerPixel * transactionCycle * ClkWidth;
    lcd_te_time_para.lcmScanTimePerLine = (1000000000.0 / (frame_rate * (height + back_porch + front_porch)));

    return ret;
}


hal_display_lcd_status_t display_lcd_calculate_te(uint32_t lcm_width, uint32_t lcm_height)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;
    uint32_t roi_width, roi_height, final_hsync_cnt = 0;

    GET_LCD_ROI_WIDTH(roi_width);
    GET_LCD_ROI_HEIGHT(roi_height);

    if (lcm_width >= roi_width) {
        uint32_t	temp = (uint32_t)((roi_height * roi_width * lcd_te_time_para.lcdPixelTransitionTime) / lcd_te_time_para.lcmScanTimePerLine);
        uint32_t	delay_line_count = 0;
        if (temp <= roi_height) {
            delay_line_count = roi_height - temp;
        } else {
            delay_line_count = 0;
        }
        final_hsync_cnt += delay_line_count;
    }

    SET_TE_SYNC_COUNTER_WAITLINE(final_hsync_cnt);

    return ret;
}

uint32_t display_lcd_get_roi_width(void)
{
    int width;

    GET_LCD_ROI_WIDTH(width);

    return width;
}

hal_display_lcd_status_t display_lcd_dbgInfoPrint(display_lcd_debug_info_e dbg_index)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    switch (dbg_index) {
        case DISPLAY_LCD_PRINT_IF_TIME_PARA:
            break;
        case DISPLAY_LCD_PRINT_TE_TIME_PARA:
            break;
        default:
            break;
    }

    return ret;
}

hal_display_lcd_status_t display_lcd_toggle_reset(uint32_t low_time, uint32_t high_time)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    lcd_register_ptr->lcd_rstb_register.field.RSTB = 1;
    hal_gpt_delay_ms(1);
    lcd_register_ptr->lcd_rstb_register.field.RSTB = 0;
    hal_gpt_delay_ms(low_time);
    lcd_register_ptr->lcd_rstb_register.field.RSTB = 1;
    hal_gpt_delay_ms(high_time);

    return ret;
}

hal_display_lcd_status_t display_lcd_start_dma(bool wait_te)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    lcd_dsi_register_ptr->LCD_DSI_MEM_CONTI_REGISTER.field.DSI_RWMEM_CONTI = 0x3C;
    lcd_dsi_register_ptr->LCD_DSI_INTEN_REGISTER.field.FRAME_DONE_INT_EN = 1;
#endif
    if (wait_te) {
        ENABLE_LCD_TE_DETECT;
        lcd_register_ptr->lcd_inten_register.field.SYNC = 1;
    }
    ENABLE_LCD_TRANSFER_COMPLETE_INT;
    DISABLE_LCD_ROI_CTRL_CMD_FIRST;
    START_LCD_TRANSFER;

    return ret;

}

uint32_t display_lcd_get_layer_address(hal_display_lcd_layer_t layer)
{
    return lcd_register_ptr->lcd_layer_register[layer].lcd_lwinadd_register.value;
}

hal_display_lcd_status_t display_lcd_init_para(void)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    memset(&lcd_if_time_para, 0, sizeof(hal_display_lcd_interface_timing_t));
    memset(&lcd_if_mode_para, 0, sizeof(hal_display_lcd_interface_mode_t));
    memset(&lcd_if_2_dana_lane_para, 0, sizeof(hal_display_lcd_interface_2_data_lane_t));

    return ret;
}

backup_register_t backup_reg;

hal_display_lcd_status_t display_lcd_backup_register(void)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;
    uint32_t i;

    backup_reg.lcd_pal_add_register.value = lcd_register_ptr->lcd_pal_add_register.value;
    backup_reg.lcd_wroicon_register.value = lcd_register_ptr->lcd_wroicon_register.value;
    backup_reg.lcd_wroiofs_register.value = lcd_register_ptr->lcd_wroiofs_register.value;
    backup_reg.lcd_wroicadd_register.value = lcd_register_ptr->lcd_wroicadd_register.value;
    backup_reg.lcd_wroidadd_register.value = lcd_register_ptr->lcd_wroidadd_register.value;
    backup_reg.lcd_wroisize_register.value = lcd_register_ptr->lcd_wroisize_register.value;
    backup_reg.lcd_wroi_bgclr_register.value = lcd_register_ptr->lcd_wroi_bgclr_register.value;

    for (i = 0; i < 4; i++) {
        backup_reg.lcd_layer_register[i].lcd_lwincon_register.value = lcd_register_ptr->lcd_layer_register[i].lcd_lwincon_register.value;
        backup_reg.lcd_layer_register[i].lcd_lwinkey_register.value = lcd_register_ptr->lcd_layer_register[i].lcd_lwinkey_register.value;
        backup_reg.lcd_layer_register[i].lcd_lwinofs_register.value = lcd_register_ptr->lcd_layer_register[i].lcd_lwinofs_register.value;
        backup_reg.lcd_layer_register[i].lcd_lwinadd_register.value = lcd_register_ptr->lcd_layer_register[i].lcd_lwinadd_register.value;
        backup_reg.lcd_layer_register[i].lcd_lwinsize_register.value = lcd_register_ptr->lcd_layer_register[i].lcd_lwinsize_register.value;
        backup_reg.lcd_layer_register[i].lcd_lwinmofs_register.value = lcd_register_ptr->lcd_layer_register[i].lcd_lwinmofs_register.value;
        backup_reg.lcd_layer_register[i].lcd_lwinpitch_register.value = lcd_register_ptr->lcd_layer_register[i].lcd_lwinpitch_register.value;
    }

    return ret;
}

hal_display_lcd_status_t display_lcd_restore_layer_settings(void)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;
    uint32_t i;

    lcd_register_ptr->lcd_pal_add_register.value = backup_reg.lcd_pal_add_register.value;
    lcd_register_ptr->lcd_wroicon_register.value = backup_reg.lcd_wroicon_register.value;
    lcd_register_ptr->lcd_wroiofs_register.value = backup_reg.lcd_wroiofs_register.value;
    lcd_register_ptr->lcd_wroicadd_register.value = backup_reg.lcd_wroicadd_register.value;
    lcd_register_ptr->lcd_wroidadd_register.value = backup_reg.lcd_wroidadd_register.value;
    lcd_register_ptr->lcd_wroisize_register.value = backup_reg.lcd_wroisize_register.value;
    lcd_register_ptr->lcd_wroi_bgclr_register.value = backup_reg.lcd_wroi_bgclr_register.value;

    for (i = 0; i < LCD_TOTAL_LAYER; i++) {
        lcd_register_ptr->lcd_layer_register[i].lcd_lwincon_register.value = backup_reg.lcd_layer_register[i].lcd_lwincon_register.value;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinkey_register.value = backup_reg.lcd_layer_register[i].lcd_lwinkey_register.value;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinofs_register.value = backup_reg.lcd_layer_register[i].lcd_lwinofs_register.value;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinadd_register.value = backup_reg.lcd_layer_register[i].lcd_lwinadd_register.value;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinsize_register.value = backup_reg.lcd_layer_register[i].lcd_lwinsize_register.value;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinmofs_register.value = backup_reg.lcd_layer_register[i].lcd_lwinmofs_register.value;
        lcd_register_ptr->lcd_layer_register[i].lcd_lwinpitch_register.value = backup_reg.lcd_layer_register[i].lcd_lwinpitch_register.value;
    }

    return ret;
}

hal_display_lcd_status_t display_lcd_power_on(void)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;
    hal_clock_status_t clock_ret;

    clock_ret = hal_clock_enable(HAL_CLOCK_CG_LCD);
    clock_ret = hal_clock_enable(HAL_CLOCK_CG_LCD_APB);

    if (clock_ret != HAL_CLOCK_STATUS_OK) {
        ret = HAL_DISPLAY_LCD_STATUS_POWER_FAILED;
    }

    return ret;
}

hal_display_lcd_status_t display_lcd_power_off(void)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;
    hal_clock_status_t clock_ret;

    clock_ret = hal_clock_disable(HAL_CLOCK_CG_LCD);
    clock_ret = hal_clock_disable(HAL_CLOCK_CG_LCD_APB);

    if (clock_ret != HAL_CLOCK_STATUS_OK) {
        ret = HAL_DISPLAY_LCD_STATUS_POWER_FAILED;
    }

    return ret;
}

hal_display_lcd_status_t display_lcd_apply_setting(void)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    // apply roi
    uint32_t roi_width, roi_height, layer_index, i;
#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    uint32_t temp, bpp = 1;
    LCD_DSI_PSCON_REGISTER_T pscon;

    hal_display_lcd_output_color_format_t lcd_to_lcm_fmt;

#endif

    DISABLE_LCD_TE_DETECT;

    roi_width = roi_struct.target_end_x - roi_struct.target_start_x + 1;
    roi_height = roi_struct.target_end_y - roi_struct.target_start_y + 1;


    display_lcd_config_dither();
    //ENABLE_LCD_TRANSFER_COMPLETE_INT;
    SET_LCD_ROI_BG_COLOR(roi_struct.roi_background_color);
    SET_LCD_ROI_CTRL_OUTPUT_FORMAT(roi_struct.main_lcd_output);
    SET_LCD_ROI_WINDOW_OFFSET((roi_struct.roi_offset_x), (roi_struct.roi_offset_y));
    SET_LCD_ROI_WINDOW_SIZE(roi_width, roi_height);

    // apply layer setting

    /// Basic check
    //ASSERT(layer_index < LCD_TOTAL_LAYER);

    /*
    if (layer_data->alpha_en)
    {
    	//ASSERT(layer_data->alpha <= LCD_MAX_OPACITY);
    }
    */
    //ASSERT(layer_data->clr_fmt <= LCD_LAYER_MONO);
    //ASSERT(layer_data->rotate  <= LAYER_ROTATE_270_DEGREE_FLIP);

    //ASSERT(0==(layer_data->buffer_address& 0x3));

    if (layer_struct[0].layer_enable == HAL_DISPLAY_LCD_LAYER0) {
        ENABLE_LCD_LAYER0;
    }
    if (layer_struct[1].layer_enable == HAL_DISPLAY_LCD_LAYER1) {
        ENABLE_LCD_LAYER1;
    }
    if (layer_struct[2].layer_enable == HAL_DISPLAY_LCD_LAYER2) {
        ENABLE_LCD_LAYER2;
    }
    if (layer_struct[3].layer_enable == HAL_DISPLAY_LCD_LAYER3) {
        ENABLE_LCD_LAYER3;
    }

    //display_lcd_restore_if_settings();

    for (layer_index = 0; layer_index < 4; layer_index++) {
        lcd_register_ptr->lcd_layer_register[layer_index].lcd_lwincon_register.value = 0;

        SET_LCD_LAYER_SOURCE_KEY_EN(layer_index, layer_struct[layer_index].source_key_flag);
        SET_LCD_LAYER_DST_KEY_EN(layer_index, layer_struct[layer_index].destination_key_flag);
        SET_LCD_LAYER_ALPHA_EN(layer_index, layer_struct[layer_index].alpha_flag);
        SET_LCD_LAYER_ALPHA(layer_index, layer_struct[layer_index].alpha);
        SET_LCD_LAYER_ROTATE(layer_index,  layer_struct[layer_index].rotate);
        SET_LCD_LAYER_COLOR_FORMAT(layer_index,  layer_struct[layer_index].color_format);
        SET_LCD_LAYER_DITHER_EN(layer_index, layer_struct[layer_index].dither_flag);

        SET_LCD_LAYER_SOURCE_KEY(layer_index, layer_struct[layer_index].color_key);
        SET_LCD_LAYER_WINDOW_WMEM_OFFSET(layer_index, (layer_struct[layer_index].window_x_offset), (layer_struct[layer_index].window_y_offset));
        SET_LCD_LAYER_WINDOW_START_ADDR(layer_index, layer_struct[layer_index].buffer_address);
        SET_LCD_LAYER_WINDOW_SIZE(layer_index, (layer_struct[layer_index].column_size), (layer_struct[layer_index].row_size));
        SET_LCD_LAYER_WINDOW_WMEM_PITCH(layer_index, layer_struct[layer_index].pitch);
    }

    for (i = 0; i < 16; i += 2) {
        lcd_register_ptr->lcd_table_index_color_register[i / 2].value = (backup_index_table[i + 1] << 16 | backup_index_table[i]);
    }

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    GET_LCD_ROI_CTRL_OUTPUT_FORMAT(temp);
    lcd_to_lcm_fmt = (hal_display_lcd_output_color_format_t)((temp & 0x38) >> 3);

    switch (lcd_to_lcm_fmt) {
        case HAL_DISPLAY_LCD_ROI_OUTPUT_RGB565:
            bpp = 2;
            pscon = lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER;
            pscon.field.DSI_PS_SEL = 0x0;
            lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER.value = pscon.value;
            break;
        case HAL_DISPLAY_LCD_ROI_OUTPUT_RGB888:
            bpp = 3;
            pscon = lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER;
            pscon.field.DSI_PS_SEL = 0x3;
            lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER.value = pscon.value;
            break;
        default:
            return HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER;
    }

    lcd_dsi_register_ptr->LCD_DSI_VACT_NL_REGISTER.field.VACT_NL = roi_height;
    pscon = lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER;
    pscon.field.DSI_PS_WC = roi_width * bpp;
    lcd_dsi_register_ptr->LCD_DSI_PSCON_REGISTER.value = pscon.value;
#endif

    return ret;
}

hal_display_lcd_status_t display_lcd_set_layer_to_default(void)
{
    uint32_t i;
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    for (i = 0; i < 4; i++) {
        memset(&layer_struct[i], 0, sizeof(hal_display_lcd_layer_input_t));
    }

    DISABLE_LCD_LAYER0;
    DISABLE_LCD_LAYER1;
    DISABLE_LCD_LAYER2;
    DISABLE_LCD_LAYER3;

    lcd_register_ptr->lcd_start_register.value = 1;
    lcd_register_ptr->lcd_start_register.value = 0;
    lcd_register_ptr->lcd_inten_register.value = 0;

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    lcd_dsi_register_ptr->LCD_DSI_INTEN_REGISTER.field.FRAME_DONE_INT_EN = 0;

    lcd_dsi_register_ptr->LCD_DSI_COM_CON_REGISTER.field.DSI_RESET = 1;
    lcd_dsi_register_ptr->LCD_DSI_COM_CON_REGISTER.field.DSI_RESET = 0;
#endif

    return ret;
}

#ifdef __cplusplus
}
#endif

#endif
