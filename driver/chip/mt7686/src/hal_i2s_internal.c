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

#include "hal_i2s_internal.h"
#ifdef HAL_I2S_MODULE_ENABLED
#include "hal_gpt.h"


/*i2s0*/
static uint32_t i2s0_global_cg = (uint32_t)0x0;
static uint32_t i2s0_dl_cg = (uint32_t)0x0;
static uint32_t i2s0_ul_cg = (uint32_t)0x0;
/*i2s1*/
static uint32_t i2s1_global_cg = (uint32_t)0x0;
static uint32_t i2s1_dl_cg = (uint32_t)0x0;
static uint32_t i2s1_ul_cg = (uint32_t)0x0;
static volatile bool bl_xpll_open = false;


static void i2s_set_clock_source(i2s_internal_t *hal_i2s, hal_i2s_fs_base_t fs_base)
{

#if defined XPLL_ENABLE
    if (fs_base == HAL_I2S_FS_BASE_8KHZ) {
        hal_i2s->i2s_clock_source = HAL_I2S_CLOCK_XPLL_24_576M;
        log_hal_info("i2s_clock_source is HAL_I2S_CLOCK_XPLL_24_576M\r\n");
    } else {
        hal_i2s->i2s_clock_source = HAL_I2S_CLOCK_XPLL_22_5792M;
        log_hal_info("i2s_clock_source is HAL_I2S_CLOCK_XPLL_22_5792M\r\n");
    }
#else
    hal_i2s->i2s_clock_source = HAL_I2S_CLOCK_XO_26M;
    log_hal_info("i2s_clock_source is HAL_I2S_CLOCK_XO_26M\r\n");
#endif

}


void i2s_xpll_close(void)
{
    if (!bl_xpll_open) {
        return;
    }
    DRV_Reg32(XPLL_CTL0) = (uint32_t)0x1;
    //wait 1us
    hal_gpt_delay_us(1);
    DRV_Reg32(XPLL_CTL8) &= ~(uint32_t)0x8;
    //wait 1us
    hal_gpt_delay_us(1);
    DRV_Reg32(XPLL_CTL8) |= (uint32_t)0x10000;
    bl_xpll_open = false;

}

void i2s_xpll_open(i2s_internal_t *hal_i2s)
{
    if ((hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XO_26M)) {
        return;
    }

    log_hal_info("i2s_xpll_open\r\n");
    if (bl_xpll_open) {
        log_hal_info("i2s_xpll is already opened\r\n");
        return;
    }

    /*Read CKSYS_XTAL_FREQ__F_F_FXO_IS_26M (0xA20202A3) for REF_CK (1: 26MHz, 0: 40MHz)*/
    uint8_t ref_clk = DRV_Reg8(CKSYS_XTAL_FREQ__F_F_FXO_IS_26M);

    if (ref_clk == 1) { //26mhz
        if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XPLL_26M) {
            log_hal_info("ref_clk 26M, HAL_I2S_CLOCK_XPLL_26M\r\n");
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x441F;
            DRV_Reg32(XPLL_CTL1) = (uint32_t)0x441F;
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0x7320;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x1E000000;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F00;
            //wait 20us
            hal_gpt_delay_us(20);
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x441E;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F008;
            while ((DRV_Reg32(XPLL_CTL4) & AD_RGS_PLL_VCOCAL_CPLT_MASK) != AD_RGS_PLL_VCOCAL_CPLT);
            DRV_Reg32(XPLL_CTL4) = (uint32_t)0x2A;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F018;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x1E000001;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F038;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0xF302;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0xF038;
            //wait 20us
            hal_gpt_delay_us(20);
        }

        if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XPLL_24_576M) {
            log_hal_info("ref_clk 26M, HAL_I2S_CLOCK_XPLL_24_576M\r\n");
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x441D;
            DRV_Reg32(XPLL_CTL1) = (uint32_t)0xD861;
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0x7302;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x1C3F549A;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x20000;
            //wait 20us
            hal_gpt_delay_us(20);
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x441C;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x20008;
            while ((DRV_Reg32(XPLL_CTL4) & AD_RGS_PLL_VCOCAL_CPLT_MASK) != AD_RGS_PLL_VCOCAL_CPLT);
            DRV_Reg32(XPLL_CTL4) = (uint32_t)0x2A;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x20018;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x1C3F549B;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x20038;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0xF302;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x38;
            //wait 20us
            hal_gpt_delay_us(20);
        }

        if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XPLL_22_5792M) {
            log_hal_info("ref_clk 26M, HAL_I2S_CLOCK_XPLL_22_5792M\r\n");
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x435;
            DRV_Reg32(XPLL_CTL1) = (uint32_t)0xC861;
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0x7302;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x19CA2F54;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F000;
            //wait 20us
            hal_gpt_delay_us(20);
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x434;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F008;
            while ((DRV_Reg32(XPLL_CTL4) & AD_RGS_PLL_VCOCAL_CPLT_MASK) != AD_RGS_PLL_VCOCAL_CPLT);
            DRV_Reg32(XPLL_CTL4) = (uint32_t)0x2A;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F018;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x19CA2F55;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F038;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0xF302;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0xF038;
            //wait 20us
            hal_gpt_delay_us(20);
        }

    } else { //40mhz
        if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XPLL_26M) {
            log_hal_info("ref_clk 40M, HAL_I2S_CLOCK_XPLL_26M\r\n");
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x4413;
            DRV_Reg32(XPLL_CTL1) = (uint32_t)0x6861;
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0x7303;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x12CCCCCC;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F00;
            //wait 20us
            hal_gpt_delay_us(20);
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x4412;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F008;
            while ((DRV_Reg32(XPLL_CTL4) & AD_RGS_PLL_VCOCAL_CPLT_MASK) != AD_RGS_PLL_VCOCAL_CPLT);
            DRV_Reg32(XPLL_CTL4) = (uint32_t)0x2A;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F008;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x12CCCCCD;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F038;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0xF303;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0xF038;
            //wait 20us
            hal_gpt_delay_us(20);
        }

        if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XPLL_24_576M) {
            log_hal_info("ref_clk 40M, HAL_I2S_CLOCK_XPLL_24_576M\r\n");
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x4411;
            DRV_Reg32(XPLL_CTL1) = (uint32_t)0x5861;
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0x7303;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x11A92A30;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F000;
            //wait 20us
            hal_gpt_delay_us(20);
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x4410;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F008;
            while ((DRV_Reg32(XPLL_CTL4) & AD_RGS_PLL_VCOCAL_CPLT_MASK) != AD_RGS_PLL_VCOCAL_CPLT);
            DRV_Reg32(XPLL_CTL4) = (uint32_t)0x2A;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F018;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x11A92A31;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F038;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0xF302;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0xF038;
            //wait 20us
            hal_gpt_delay_us(20);
        }

        if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XPLL_22_5792M) {
            log_hal_info("ref_clk 40M, HAL_I2S_CLOCK_XPLL_22_5792M\r\n");
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x423;
            DRV_Reg32(XPLL_CTL1) = (uint32_t)0x5861;
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0x7303;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x1010385C;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F000;
            //wait 20us
            hal_gpt_delay_us(20);
            DRV_Reg32(XPLL_CTL0) = (uint32_t)0x422;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F008;
            while ((DRV_Reg32(XPLL_CTL4) & AD_RGS_PLL_VCOCAL_CPLT_MASK) != AD_RGS_PLL_VCOCAL_CPLT);
            DRV_Reg32(XPLL_CTL4) = (uint32_t)0x2A;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F018;
            DRV_Reg32(XPLL_CTL3) = (uint32_t)0x1010385D;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0x2F038;
            //wait 1us
            hal_gpt_delay_us(1);
            DRV_Reg32(XPLL_CTL2) = (uint32_t)0xF303;
            DRV_Reg32(XPLL_CTL8) = (uint32_t)0xF038;
            //wait 20us
            hal_gpt_delay_us(20);
        }

    }
    bl_xpll_open = true;
}


void i2s_set_initial_parameter(i2s_internal_t *hal_i2s)
{

    memset(hal_i2s, 0 , sizeof(i2s_internal_t));

    hal_i2s->i2s_initial_type = HAL_I2S_TYPE_MAX;
    hal_i2s->i2s_port = HAL_I2S_0;
    hal_i2s->i2s_clock_source = HAL_I2S_CLOCK_MAX;
    hal_i2s->i2s_vfifo.tx_vfifo_length = 0;
    hal_i2s->i2s_vfifo.tx_vfifo_threshold = 0;
    hal_i2s->i2s_vfifo.rx_vfifo_length = 0;
    hal_i2s->i2s_vfifo.rx_vfifo_threshold = 0;

    hal_i2s->i2s_user_config.clock_mode = HAL_I2S_MASTER;
    hal_i2s->i2s_user_config.tdm_channel = HAL_I2S_TDM_2_CHANNEL;
    hal_i2s->i2s_user_config.frame_sync_width = HAL_I2S_FRAME_SYNC_WIDTH_32;
    hal_i2s->i2s_user_config.sample_width = HAL_I2S_SAMPLE_WIDTH_16BIT;
    hal_i2s->i2s_user_config.tx_mode = HAL_I2S_TX_MONO_DUPLICATE_DISABLE;
    hal_i2s->i2s_user_config.rx_down_rate = HAL_I2S_RX_DOWN_RATE_DISABLE;

    hal_i2s->i2s_user_config.i2s_in.channel_number = HAL_I2S_STEREO;
    hal_i2s->i2s_user_config.i2s_in.sample_rate = HAL_I2S_SAMPLE_RATE_48K;
    hal_i2s->i2s_user_config.i2s_in.msb_offset = 0;
    hal_i2s->i2s_user_config.i2s_in.lr_swap = HAL_I2S_LR_SWAP_DISABLE;
    hal_i2s->i2s_user_config.i2s_in.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;

    hal_i2s->i2s_user_config.i2s_out.channel_number = HAL_I2S_STEREO;
    hal_i2s->i2s_user_config.i2s_out.sample_rate = HAL_I2S_SAMPLE_RATE_48K;
    hal_i2s->i2s_user_config.i2s_out.msb_offset = 0;
    hal_i2s->i2s_user_config.i2s_out.lr_swap = HAL_I2S_LR_SWAP_DISABLE;
    hal_i2s->i2s_user_config.i2s_out.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;

    /*
    log_hal_info("hal_i2s->user_tx_callback_func=%x\r\n", (unsigned int)hal_i2s->user_tx_callback_func);
    log_hal_info("hal_i2s->user_rx_callback_func=%x\r\n", (unsigned int)hal_i2s->user_rx_callback_func);
    log_hal_info("hal_i2s->user_tx_data=%x\r\n", (unsigned int)hal_i2s->user_tx_data);
    log_hal_info("hal_i2s->user_rx_data=%x\r\n", (unsigned int)hal_i2s->user_rx_data);
    log_hal_info("hal_i2s->i2s_state=%d\r\n", (unsigned int)hal_i2s->i2s_state);
    log_hal_info("hal_i2s->i2s_initial_type=%d\r\n", (unsigned int)hal_i2s->i2s_initial_type);

    log_hal_info("hal_i2s->i2s_user_config.clock_mode=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.clock_mode);
    log_hal_info("hal_i2s->i2s_user_config.tdm_channel=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.tdm_channel);
    log_hal_info("hal_i2s->i2s_user_config.frame_sync_width=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.frame_sync_width);
    log_hal_info("hal_i2s->i2s_user_config.sample_width=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.sample_width);
    log_hal_info("hal_i2s->i2s_user_config.tx_mode=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.tx_mode);
    log_hal_info("hal_i2s->i2s_user_config.rx_down_rate=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.rx_down_rate);

    log_hal_info("hal_i2s->i2s_user_config.i2s_in.channel_number=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.i2s_in.channel_number);
    log_hal_info("hal_i2s->i2s_user_config.i2s_in.sample_rate=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.i2s_in.sample_rate);
    log_hal_info("hal_i2s->i2s_user_config.i2s_in.msb_offset=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.i2s_in.msb_offset);
    log_hal_info("hal_i2s->i2s_user_config.i2s_in.lr_swap=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.i2s_in.lr_swap);
    log_hal_info("hal_i2s->i2s_user_config.i2s_in.word_select_inverse=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.i2s_in.word_select_inverse);

    log_hal_info("hal_i2s->i2s_user_config.i2s_out.channel_number=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.i2s_in.channel_number);
    log_hal_info("hal_i2s->i2s_user_config.i2s_out.sample_rate=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.i2s_in.sample_rate);
    log_hal_info("hal_i2s->i2s_user_config.i2s_out.msb_offset=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.i2s_in.msb_offset);
    log_hal_info("hal_i2s->i2s_user_config.i2s_out.lr_swap=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.i2s_in.lr_swap);
    log_hal_info("hal_i2s->i2s_user_config.i2s_out.word_select_inverse=%d\r\n", (unsigned int)hal_i2s->i2s_user_config.i2s_in.word_select_inverse);
    */
}

void i2s_set_parameter(i2s_internal_t *hal_i2s)
{

    switch (hal_i2s->i2s_initial_type) {

        case HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE: {
            if (hal_i2s->i2s_port == HAL_I2S_0) {
                i2s0_global_cg |= (uint32_t)0x80000000;
                i2s0_dl_cg = (uint32_t)0x08008009;
                i2s0_ul_cg = (uint32_t)0x08008009;

            } else {
                i2s1_global_cg |= (uint32_t)0x80000000;
            }
        }
        break;

        case HAL_I2S_TYPE_INTERNAL_TDM_LOOPBACK_MODE: {
            if (hal_i2s->i2s_port == HAL_I2S_0) {
                i2s0_global_cg |= (uint32_t)0x80000000;
                i2s0_dl_cg = (uint32_t)0x08008021;//TDM32_CH2, master
                i2s0_ul_cg = (uint32_t)0x08008021;//TDM32_CH2, master
            }
        }
        break;

        case HAL_I2S_TYPE_EXTERNAL_MODE: {
            if (hal_i2s->i2s_port == HAL_I2S_0) {
                i2s0_dl_cg = (uint32_t)0x08008009;
                i2s0_ul_cg = (uint32_t)0x08008009;
            }
        }
        break;

        case HAL_I2S_TYPE_EXTERNAL_TDM_MODE: {
            if (hal_i2s->i2s_port == HAL_I2S_0) {
                i2s0_dl_cg = (uint32_t)0x08008021; //TDM32_CH2, master
                i2s0_ul_cg = (uint32_t)0x08008021; //TDM32_CH2, master
            }
        }
        break;

        default:
            break;
    }

    if (hal_i2s->i2s_port == HAL_I2S_0) {

        if (hal_i2s->i2s_user_config.clock_mode == HAL_I2S_MASTER) {
            i2s0_dl_cg &= ~((uint32_t)0x4); //bit2 ; 0:master, 1:slave
            i2s0_ul_cg &= ~((uint32_t)0x4); //bit2 ; 0:master, 1:slave
        } else {
            i2s0_global_cg |= (uint32_t)0xF100000;
            i2s0_dl_cg |= ((uint32_t)0x4); //bit2 ; 0:master, 1:slave
            i2s0_ul_cg |= ((uint32_t)0x4); //bit2 ; 0:master, 1:slave
        }

        //mono mode
        if (hal_i2s->i2s_user_config.i2s_out.channel_number == HAL_I2S_MONO) {
            i2s0_global_cg |= (uint32_t)0x100;
        }

        //mono duplication
        if (hal_i2s->i2s_user_config.tx_mode == HAL_I2S_TX_MONO_DUPLICATE_ENABLE) {
            i2s0_global_cg |= (uint32_t)0x200;
        }

        //tx lr swap
        if (hal_i2s->i2s_user_config.i2s_out.lr_swap == 1) {
            i2s0_global_cg |= (uint32_t)0x80;
        }

        //rx lr swap
        if (hal_i2s->i2s_user_config.i2s_in.lr_swap == 1) {
            i2s0_ul_cg |= (uint32_t)0x80000000;
        }


        if ((hal_i2s->i2s_initial_type == HAL_I2S_TYPE_EXTERNAL_TDM_MODE) ||
                (hal_i2s->i2s_initial_type == HAL_I2S_TYPE_INTERNAL_TDM_LOOPBACK_MODE)) {

            /*channel per frame sync, bit[30:29]  00 = 2ch, 01 =4 ch*/
            if (hal_i2s->i2s_user_config.tdm_channel == HAL_I2S_TDM_2_CHANNEL) {
                ;//do nothing
            } else {
                i2s0_dl_cg |= (uint32_t)0x20000000;
                i2s0_ul_cg |= (uint32_t)0x20000000;
            }

            /*bits per frame sync, bit[14:13]   00 = 32bits, 01 = 64bits, 10 =128bits*/
            if (hal_i2s->i2s_user_config.frame_sync_width == HAL_I2S_FRAME_SYNC_WIDTH_32) {
                ;//do nothing
            } else if (hal_i2s->i2s_user_config.frame_sync_width == HAL_I2S_FRAME_SYNC_WIDTH_64) {
                i2s0_dl_cg |= (uint32_t)0x2000;
                i2s0_ul_cg |= (uint32_t)0x2000;
                if (hal_i2s->i2s_user_config.tdm_channel == HAL_I2S_TDM_4_CHANNEL) {
                    i2s0_dl_cg |= (uint32_t)0x80;
                }
            } else {//HAL_I2S_FRAME_SYNC_WIDTH_128
                i2s0_dl_cg |= (uint32_t)0x4000;
                i2s0_ul_cg |= (uint32_t)0x4000;
                if (hal_i2s->i2s_user_config.tdm_channel == HAL_I2S_TDM_4_CHANNEL) {
                    i2s0_dl_cg |= (uint32_t)0x80;
                }
            }

            /*bit[23:17] MSB_OFFSET*/
            uint32_t MSB_OFFSET = hal_i2s->i2s_user_config.i2s_in.msb_offset;
            //log_hal_info("MSB_OFFSET=%x\r\n", (unsigned int)MSB_OFFSET);
            MSB_OFFSET = MSB_OFFSET << 17;
            //log_hal_info("MSB_OFFSET << 17=%x\r\n", (unsigned int)MSB_OFFSET);
            i2s0_dl_cg |= MSB_OFFSET;
            i2s0_ul_cg |= MSB_OFFSET;
        }

        DRV_Reg32(I2S_GLOBAL_CONTROL) = i2s0_global_cg;
        DRV_Reg32(I2S_DL_CONTROL) = i2s0_dl_cg;
        DRV_Reg32(I2S_UL_CONTROL) = i2s0_ul_cg;

        log_hal_info("AUDIO TOP REGISTER DUMP\r\n");
        log_hal_info("I2S_GLOBAL_CONTROL=%x\r\n", (unsigned int)DRV_Reg32(I2S_GLOBAL_CONTROL));
        log_hal_info("I2S_DL_CONTROL=%x\r\n", (unsigned int)DRV_Reg32(I2S_DL_CONTROL));
        log_hal_info("I2S_UL_CONTROL=%x\r\n", (unsigned int)DRV_Reg32(I2S_UL_CONTROL));
        log_hal_info("I2S_DL_SR_EN_CONTROL__F_CR_I2S_OUT_SR=%x\r\n", DRV_Reg8(I2S_DL_SR_EN_CONTROL__F_CR_I2S_OUT_SR));
        log_hal_info("I2S_UL_SR_EN_CONTROL__F_CR_I2S_IN_SR=%x\r\n", DRV_Reg8(I2S_UL_SR_EN_CONTROL__F_CR_I2S_IN_SR));

    } else { //hal_i2s->i2s_port == HAL_I2S_1

        //mono mode
        if (hal_i2s->i2s_user_config.i2s_out.channel_number == HAL_I2S_MONO) {
            i2s1_global_cg |= (uint32_t)0x100;
        }

        //mono duplication
        if (hal_i2s->i2s_user_config.tx_mode == HAL_I2S_TX_MONO_DUPLICATE_ENABLE) {
            i2s1_global_cg |= (uint32_t)0x200;
        }

        //tx lr swap
        if (hal_i2s->i2s_user_config.i2s_out.lr_swap == 1) {
            i2s1_global_cg |= (uint32_t)0x80;
        }

        //rx lr swap
        //only for tx use, rx is always disable
        if (hal_i2s->i2s_user_config.i2s_in.lr_swap == 1) {
            i2s1_ul_cg |= (uint32_t)0x80000000;
        }


        /*bits per frame sync, bit[14:13]   00 = 32bits, 01 = 64bits, 10 =128bits*/
        if (hal_i2s->i2s_user_config.frame_sync_width == HAL_I2S_FRAME_SYNC_WIDTH_32) {
            ;//do nothing
        }
        if (hal_i2s->i2s_user_config.frame_sync_width == HAL_I2S_FRAME_SYNC_WIDTH_64) {
            i2s1_dl_cg |= (uint32_t)0x2;
            i2s1_ul_cg |= (uint32_t)0x2;
        }

        //sample width, bit18 and bit1: 1 for 24bit mode, 0 for 16bit mode
        if (hal_i2s->i2s_user_config.sample_width == HAL_I2S_SAMPLE_WIDTH_24BIT) {
            if (hal_i2s->i2s_user_config.clock_mode == HAL_I2S_MASTER) {
                i2s1_dl_cg |= (uint32_t)0x0006008B;//32bit  input master i2s mode
                i2s1_ul_cg |= (uint32_t)0x0006800B;
            } else {
                i2s1_dl_cg |= (uint32_t)0x0006008F;//32bit  input slave i2s mode
                i2s1_ul_cg |= (uint32_t)0x0006800F;
            }

        } else {//16bit
            if (hal_i2s->i2s_user_config.clock_mode == HAL_I2S_MASTER) {
                i2s1_dl_cg |= (uint32_t)0x00020009;//16bit  input master i2s mode
                i2s1_ul_cg |= (uint32_t)0x00028009;
            } else {
                i2s1_dl_cg |= (uint32_t)0x0002000D;//16bit  input slave i2s mode
                i2s1_ul_cg |= (uint32_t)0x0002800D;
            }
        }
        DRV_Reg32(I2S1_GLOBAL_CONTROL) = i2s1_global_cg;
        DRV_Reg32(I2S1_DL_CONTROL) = i2s1_dl_cg;
        DRV_Reg32(I2S1_UL_CONTROL) = i2s1_ul_cg;

        log_hal_info("ASYS TOP REGISTER DUMP\r\n");
        log_hal_info("I2S1_GLOBAL_CONTROL=%x\r\n", (unsigned int)DRV_Reg32(I2S1_GLOBAL_CONTROL));
        log_hal_info("I2S1_UL_CONTROL=%x\r\n", (unsigned int)DRV_Reg32(I2S1_UL_CONTROL));
        log_hal_info("I2S1_DL_CONTROL=%x\r\n", (unsigned int)DRV_Reg32(I2S1_DL_CONTROL));
        log_hal_info("I2S1_DL_SR_EN_CONTROL__F_CR_I2S1_OUT_SR=%x\r\n", DRV_Reg8(I2S1_DL_SR_EN_CONTROL__F_CR_I2S1_OUT_SR));
        log_hal_info("I2S1_UL_SR_EN_CONTROL__F_CR_I2S1_IN_SR=%x\r\n", DRV_Reg8(I2S1_UL_SR_EN_CONTROL__F_CR_I2S1_IN_SR));
    }

}

hal_i2s_status_t i2s_set_clock(i2s_internal_t *hal_i2s, hal_i2s_sample_rate_t sample_rate)
{
    uint8_t fs_value = 0;
    hal_i2s_fs_base_t fs_base;

    switch (sample_rate) {
        case HAL_I2S_SAMPLE_RATE_8K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_8K\r\n");
            fs_value = (uint8_t)0x0;
            fs_base = HAL_I2S_FS_BASE_8KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_11_025K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_11_025K\r\n");
            fs_value = (uint8_t)0x1;
            fs_base = HAL_I2S_FS_BASE_11_025KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_12K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_12K\r\n");
            fs_value = (uint8_t)0x2;
            fs_base = HAL_I2S_FS_BASE_8KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_16K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_16K\r\n");
            fs_value = (uint8_t)0x4;
            fs_base = HAL_I2S_FS_BASE_8KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_22_05K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_22_05K\r\n");
            fs_value = (uint8_t)0x5;
            fs_base = HAL_I2S_FS_BASE_11_025KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_24K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_24K\r\n");
            fs_value = (uint8_t)0x6;
            fs_base = HAL_I2S_FS_BASE_8KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_32K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_32K\r\n");
            fs_value = (uint8_t)0x8;
            fs_base = HAL_I2S_FS_BASE_8KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_44_1K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_44_1K\r\n");
            fs_value = (uint8_t)0x9;
            fs_base = HAL_I2S_FS_BASE_11_025KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_48K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_48K\r\n");
            fs_value = (uint8_t)0xa;
            fs_base = HAL_I2S_FS_BASE_8KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_88_2K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_88_2K\r\n");
            fs_value = (uint8_t)0xb;
            fs_base = HAL_I2S_FS_BASE_11_025KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_96K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_96K\r\n");
            fs_value = (uint8_t)0xc;
            fs_base = HAL_I2S_FS_BASE_8KHZ;
            break;

        case HAL_I2S_SAMPLE_RATE_176_4K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_176_4K\r\n");
            if (hal_i2s->i2s_user_config.frame_sync_width == HAL_I2S_FRAME_SYNC_WIDTH_128) {
                log_hal_error("192kHz and 176.4kHz only supports TDM32 and TDM64\r\n");
                return HAL_I2S_STATUS_INVALID_PARAMETER;
            } else {
                fs_value = (uint8_t)0xd;
                fs_base = HAL_I2S_FS_BASE_11_025KHZ;
            }
            break;

        case HAL_I2S_SAMPLE_RATE_192K:
            log_hal_info("HAL_I2S_SAMPLE_RATE_192K\r\n");
            if (hal_i2s->i2s_user_config.frame_sync_width == HAL_I2S_FRAME_SYNC_WIDTH_128) {
                log_hal_error("192kHz and 176.4kHz only supports TDM32 and TDM64\r\n");
                return HAL_I2S_STATUS_INVALID_PARAMETER;
            } else {
                fs_value = (uint8_t)0xe;
                fs_base = HAL_I2S_FS_BASE_8KHZ;
            }
            break;

        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    hal_i2s->i2s_user_config.i2s_in.sample_rate = sample_rate;
    hal_i2s->i2s_user_config.i2s_out.sample_rate = sample_rate;

    //set sampling rate base
    i2s_set_clock_source(hal_i2s, fs_base);
    //hd mode for xpll
    if (hal_i2s->i2s_clock_source != HAL_I2S_CLOCK_XO_26M) {
        fs_value |= (uint8_t)0x10;
    } else {
        fs_value &= (~(uint8_t)0x10);
    }

    //set sampling rate
    if (hal_i2s->i2s_port == HAL_I2S_0) {
        DRV_Reg8(I2S_DL_SR_EN_CONTROL__F_CR_I2S_OUT_SR) = fs_value;
        DRV_Reg8(I2S_UL_SR_EN_CONTROL__F_CR_I2S_IN_SR) = fs_value;
    } else {
        DRV_Reg8(I2S1_DL_SR_EN_CONTROL__F_CR_I2S1_OUT_SR) = fs_value;
        DRV_Reg8(I2S1_UL_SR_EN_CONTROL__F_CR_I2S1_IN_SR) = fs_value;
    }


    //set xpll or xo
    if (hal_i2s->i2s_port == HAL_I2S_0) {
        if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XO_26M) {
            i2s0_global_cg = (uint32_t)0x2A080028;
        } else if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XPLL_26M) {
            i2s0_global_cg = (uint32_t)0x2A0C0028;
        } else if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XPLL_24_576M) {
            i2s0_global_cg = (uint32_t)0x000C0028;
        } else { //HAL_I2S_CLOCK_XPLL_22_5792M
            i2s0_global_cg = (uint32_t)0x150C0028;
        }
    } else {
        if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XO_26M) {
            i2s1_global_cg = (uint32_t)0x20000028;
        } else if ((hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XPLL_26M)) {
            i2s1_global_cg = (uint32_t)0x20040028;
        } else if (hal_i2s->i2s_clock_source == HAL_I2S_CLOCK_XPLL_24_576M) {
            i2s1_global_cg = (uint32_t)0x00040028;;
        } else {//HAL_I2S_CLOCK_XPLL_22_5792M
            i2s1_global_cg = (uint32_t)0x10040028;;
        }
    }

    return HAL_I2S_STATUS_OK;
}


#endif /* HAL_I2S_MODULE_ENABLED */

