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

#include "external_dsp_application.h"
#include "hal_audio_internal_afe.h"
#include "hal_audio_internal_service.h"
#include "memory_map.h"

external_dsp_t  external_dsp;
#define FW_DL_CHECK 0  //can only enable for local debug

uint32_t error1 = 0, error2 = 0, error3 = 0;

#if FW_DL_CHECK
/* local buffer for readback check */
uint8_t read_iram0[131072] = {0};
uint8_t read_dram0[131072] = {0};
uint8_t read_dram1[122880] = {0};
#endif

uint8_t *source_iram0  = NULL;
uint8_t *source_dram0  = NULL;
uint8_t *source_dram1  = NULL;
uint32_t iram0_address = 0;  //0x10000000
uint32_t dram0_address = 0;  //0x0ffc0000
uint32_t dram1_address = 0;  //0x0ffe0000
uint32_t iram0_length  = 0;  //131072
uint32_t dram0_length  = 0;  //131072
uint32_t dram1_length  = 0;  //122880


/*reset SPI/ RTC pin to GPIO mode*/
void external_dsp_gpio_init(void)
{
    external_dsp_spi_pinmux_switch(false);
    external_dsp_rtc_pinmux_switch(false);
}

external_dsp_status_t external_dsp_init(void)
{
    /**Power on VDDIO by HW or host SW **/
    if (!external_dsp.external_dsp_on) {
        external_dsp_power_on(true);
        hal_gpt_delay_ms(5);
        external_dsp_spi_pinmux_switch(true);
        external_dsp_rtc_pinmux_switch(true);
        external_dsp.external_dsp_on = true;
        log_hal_info("[OK]");
    }
    else {
        log_hal_error("[ERR]");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_deinit(void)
{
    /**Power off VDDIO by HW or host SW **/
    if (external_dsp.external_dsp_on) {
        external_dsp_rtc_pinmux_switch(false);
        external_dsp_spi_pinmux_switch(false);
        hal_gpt_delay_ms(5);
        external_dsp_power_on(false);
        external_dsp.external_dsp_on = false;
        log_hal_info("[OK]");
    }
    else {
        log_hal_error("[ERR]");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_activate(bool active)
{
    external_dsp_status_t status = EXTERNAL_DSP_STATUS_OK;
    if (active) {
        log_hal_info("[ON+DL]");
        status = external_dsp_firmware_loaded();
    } else {
        status = external_dsp_deinit();
        log_hal_info("[OFF]");
    }
    return status;
}

/*query ext-DSP status*/
bool external_dsp_is_power_on(void)
{
    return external_dsp.external_dsp_on;
}

/*ext-DSP download firmware address parser*/
static void external_dsp_binary_information(void)
{
    uint8_t *first_segment = (uint8_t *)EXT_DSP_BASE;
    uint8_t *second_segment = NULL, *third_segment = NULL;

    /*iram0 address and size*/
    iram0_address = * first_segment << 24 | *(first_segment + 1) << 16 | *(first_segment + 2) << 8 | *(first_segment + 3);
    iram0_length = *(first_segment + 4) << 24 | *(first_segment + 5) << 16 | *(first_segment + 6) << 8 | *(first_segment + 7);
    source_iram0 = first_segment + 8;

    /*dram0 address and size*/
    second_segment = source_iram0 + iram0_length;
    dram0_address = *second_segment << 24 | *(second_segment + 1) << 16 | *(second_segment + 2) << 8 | *(second_segment + 3);
    dram0_length = *(second_segment + 4) << 24 | *(second_segment + 5) << 16 | *(second_segment + 6) << 8 | *(second_segment + 7);
    source_dram0 = second_segment + 8;

    /*dram1 address and size*/
    third_segment = source_dram0 + dram0_length;
    dram1_address = *third_segment << 24 | *(third_segment + 1) << 16 | *(third_segment + 2) << 8 | *(third_segment + 3);
    dram1_length = *(third_segment + 4) << 24 | *(third_segment + 5) << 16 | *(third_segment + 6) << 8 | *(third_segment + 7);
    source_dram1 = third_segment + 8;

}

#if FW_DL_CHECK
static void external_dsp_8byte_swap(uint8_t *rxbuf, uint32_t length)
{
    uint8_t local_buf[8];
    int i;

    for (i = 0; i < length / 8 * 8; i += 8) {
        local_buf[0] = rxbuf[i + 0];
        local_buf[1] = rxbuf[i + 1];
        local_buf[2] = rxbuf[i + 2];
        local_buf[3] = rxbuf[i + 3];
        local_buf[4] = rxbuf[i + 4];
        local_buf[5] = rxbuf[i + 5];
        local_buf[6] = rxbuf[i + 6];
        local_buf[7] = rxbuf[i + 7];

        rxbuf[i + 0] = local_buf[7];
        rxbuf[i + 1] = local_buf[6];
        rxbuf[i + 2] = local_buf[5];
        rxbuf[i + 3] = local_buf[4];
        rxbuf[i + 4] = local_buf[3];
        rxbuf[i + 5] = local_buf[2];
        rxbuf[i + 6] = local_buf[1];
        rxbuf[i + 7] = local_buf[0];
    }
}
#endif

/*ext-DSP download firmware function*/
external_dsp_status_t external_dsp_load_firmware(void)
{
#if FW_DL_CHECK
    uint32_t i;
    error1 = 0;
    error2 = 0;
    error3 = 0;
#endif
    external_dsp_binary_information();
    if (iram0_length % 8) {
        log_hal_error("firmware download iram0 size isn't the multiple of 8");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    external_dsp_download_firmware_binary(iram0_address, source_iram0, iram0_length);

#if FW_DL_CHECK
    external_dsp_burst_read(iram0_address, read_iram0, iram0_length);
    external_dsp_8byte_swap(read_iram0, iram0_length);
    for (i = 0; i < iram0_length; i++) {
        if (read_iram0[i] != source_iram0[i]) {
            //log_hal_info(" %d firmware iram0.bin download read data %x origin data is %x  address is %x  ",i, read_iram0[i], source_iram0[i], iram0_address+i);
            error1 = 1;
        }
    }
    log_hal_info("iram0 error =%d", error1);
#endif
    log_hal_info("iram0.bin download completed. address= 0x%x, length=%d", iram0_address, iram0_length);

    if (dram0_length % 8) {
        log_hal_error("firmware download dram0 size isn't the multiple of 8");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    external_dsp_download_firmware_binary(dram0_address, source_dram0, dram0_length);

#if FW_DL_CHECK
    external_dsp_burst_read(dram0_address, read_dram0, dram0_length);
    external_dsp_8byte_swap(read_dram0, dram0_length);
    for (i = 0; i < dram0_length; i++) {
        if (read_dram0[i] != source_dram0[i]) {
            //log_hal_info(" %d firmware dram0.bin download read data %x origin data is %x  address is %x  ",i, read_dram0[i], source_dram0[i], dram0_address+i);
            error2 = 1;
        }
    }
    log_hal_info("dram0 error =%d", error2);
#endif
    log_hal_info("dram0.bin download completed. address= 0x%x, length=%d ", dram0_address, dram0_length);

    if (dram1_length % 8) {
        log_hal_error("firmware download dram1 size isn't the multiple of 8");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    external_dsp_download_firmware_binary(dram1_address, source_dram1, dram1_length);

#if FW_DL_CHECK
    external_dsp_burst_read(dram1_address, read_dram1, dram1_length);
    external_dsp_8byte_swap(read_dram1, dram1_length);
    for (i = 0; i < dram1_length; i++) {
        if (read_dram1[i] != source_dram1[i]) {
            //log_hal_info(" %d firmware dram1.bin download read data %x origin data is %x  address is %x  ",i, read_dram1[i], source_dram1[i], dram1_address+i);
            error3 = 1;
        }
    }
    log_hal_info("dram1 error =%d", error3);
#endif
    log_hal_info("dram1.bin download completed. address= 0x%x, length=%d ", dram1_address, dram1_length);
#if FW_DL_CHECK
    if (error1 ||error2 || error3) {
        log_hal_error("error1/2/3 =%d/%d/%d", error1,error2,error3);
        return EXTERNAL_DSP_STATUS_ERROR;
    }
#endif
    return EXTERNAL_DSP_STATUS_OK;
}
/*switch PDM clock to provide ext-DSP system clock */
void external_dsp_switch_pdm_clock(bool enable)
{
    log_hal_info("enable=%x", enable);
    if (enable) {
        hal_audio_init();
        *HW_MISC |= 0x0013;
        *MD2GSYS_CG_CLR2 = 0x0100;
        *AFE_MCU_CON1 |= 0x0003;
        *AFE_MCU_CON0 = 0x0001;
        afe_switch_mic_bias(true);
        afe_audio_set_digital_mic(true);
    } else {
        afe_audio_set_digital_mic(false);
        afe_switch_mic_bias(false);
        *AFE_MCU_CON0 &= (~0x0001);
        *AFE_MCU_CON1 &= (~0x000F);
        *MD2GSYS_CG_SET2 = 0x0100;
        *HW_MISC &= (~0x0010);
    }
}

/*Turn off PDM clock to let ext-DSP enter power saving mode*/
external_dsp_status_t external_dsp_enter_psm_mode(void)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint8_t                 read_data = 0;

    log_hal_info("write state reg(0x01): %x", EXT_DSP_MODE_POWER_SAVING_W);
    status = external_dsp_write_register(EXT_DSP_STATE_REG, EXT_DSP_MODE_POWER_SAVING_W);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    status = external_dsp_read_register(EXT_DSP_STATE_REG, (uint8_t *)&read_data);
    if (status != EXTERNAL_DSP_STATUS_OK) {
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    log_hal_info("read state reg(0x01): readback=%x, expected=%x", read_data, EXT_DSP_MODE_POWER_SAVING_R);
    if (EXT_DSP_MODE_POWER_SAVING_R != read_data) {
        log_hal_error("enter power saving mode failed. Readback REG 0x01 is wrong. expect data=%x. readback=%x", EXT_DSP_MODE_POWER_SAVING_R, read_data);
    } else {
        log_hal_info("enter power saving mode success. Turn off PDM CLK.");
//        external_dsp_switch_pdm_clock(false);
    }

    return status;
}

/*Turn on PDM clock to let ext-DSP enter normal mode*/
external_dsp_status_t external_dsp_enter_normal_mode(void)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint8_t                 read_data = 0;

    status = external_dsp_write_register(EXT_DSP_RESET_REG, EXT_DSP_MODE_NORMAL_W);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    status = external_dsp_read_register(EXT_DSP_RESET_REG, (uint8_t *)&read_data);
    if (status != EXTERNAL_DSP_STATUS_OK) {
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    if (EXT_DSP_MODE_NORMAL_R != read_data) {
        log_hal_error("enter normal mode failed.");
    }
    return status;
}

external_dsp_status_t external_dsp_configure_data_path(uint32_t config)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                read_data = 0;

    hal_gpt_delay_ms(50);
    status = external_dsp_write_dram(0x0fffbff8 , config);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to configure data output path.");
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    external_dsp_read_dram(0x0fffbff8, &read_data);
    if (read_data != config) {
        log_hal_error("Readback dram 0x0fffbff8 is wrong. Expect data=%x, readback=%x", config, read_data);
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    read_data = 0;
    status = external_dsp_write_register(EXT_DSP_STATE_REG, 0x29);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to configure data output path.");
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    hal_gpt_delay_ms(10);
    external_dsp_read_dram(0x0fffbf00, &read_data); //read back address: 0x0fffbf02, but read dram address should be 4-byte align
    read_data = read_data >> 16;
    if (read_data != config) {
        log_hal_error("Readback dram 0x0fffbf02 is wrong. Expect data=%x, readback=%x", config, read_data);
        return EXTERNAL_DSP_STATUS_ERROR;
    } else {
        log_hal_info("Configure output source to PDM_DATO0/PDM_DATAO1 success. config=%x", config);
        external_dsp_set_output_source(config);
    }
    return EXTERNAL_DSP_STATUS_OK;
}

#if defined (MTK_NDVC_ENABLE)

external_dsp_status_t external_dsp_enable_spl(bool enable)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                read_data, write_data = 0;

    log_hal_info("\r\n [Ext-DSP]%s SPL enable=%x\r\n", __FUNCTION__, enable);
    external_dsp_read_dram(0x0fffbf00, &write_data);
    if (enable) {
        write_data |= (1 << 10);
    } else {
        write_data &= ~(1 << 10);
    }
    status = external_dsp_write_dram(0x0fffbf00, write_data);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("\r\n [Ext-DSP]failed to en/disable ext-DSP SPL .\r\n");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    external_dsp_read_dram(0x0fffbf00, &read_data);
    if (read_data != write_data) {
        log_hal_error("\r\n [Ext-DSP]Readback dram 0x0fffbf00 is wrong. Expect data=%x, readback=%x\r\n", write_data, read_data);
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    return status;
}


uint16_t external_dsp_read_dsp_spl_value(void)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                read_data = 0;

    status = external_dsp_read_dram(0x0fffbf08, &read_data);

    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("[EXT_DSP] DSP read error:%d\r\n",status);
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    log_hal_info("[EXT_DSP] SPL value:%x, %d dB\r\n",(uint16_t)read_data,(uint16_t)read_data);
    return ((uint16_t)read_data);
}

#endif /*MTK_NDVC_ENABLE*/

uint32_t external_dsp_read_dsp_config_path(void)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                read_data = 0;
    uint32_t                config_path=external_dsp_get_output_source();

    status = external_dsp_read_dram(0x0fffbf00, &read_data);
    read_data = read_data >> 16;
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("[EXT_DSP] DSP error:%d\r\n",status);
        return 0;
    }

    if (read_data != config_path) {
        log_hal_error("[Ext-DSP]Readback dram 0x0fffbf02 is wrong. Expect data=%x, readback=%x\r\n", config_path, read_data);
        return read_data;
    }
    return read_data;
}
/* MIPS modification */
external_dsp_status_t external_dsp_set_clock_rate(uint8_t mips)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;

    status = external_dsp_write_dram(0x0fffbff8 , mips);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to configure data output path.");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    hal_gpt_delay_ms(100);
    status = external_dsp_write_register(EXT_DSP_STATE_REG, 0x3D);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to configure data output path.");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_get_clock_rate(uint32_t *mips)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                read_mips = 0;
    uint8_t                 ldo = 0;

    status = external_dsp_read_dram(0x0fffbf04, &read_mips);
    log_hal_info("readback address 0x0fffbf04: %x\r\n", read_mips);

    ldo = (read_mips & 0xFF00) >> 8;
    read_mips &= 0x00FF;
    *mips = read_mips;
    log_hal_info("[Ext-DSP]LDO=%x, mips=%x\r\n", ldo, read_mips);
    return status;
}
external_dsp_status_t external_dsp_firmware_loaded(void)
{
    external_dsp_status_t status = EXTERNAL_DSP_STATUS_OK;

    /*Step 1: Enable VDD by HW */ /*200us before apply PDM_CLKI*/
    status = external_dsp_init();
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return status;
    }
#ifdef MTK_AUTOMOTIVE_SUPPORT
    external_dsp_clk_sw_config(false);  //bclk
    external_dsp_echo_ref_sw_config(false);
#else
    external_dsp_clk_sw_config(true);   //pcm_clk
#endif
    external_dsp_rtc_sw_config(true);
    hal_gpt_delay_ms(1);
    external_dsp_spi_init();

#ifdef MTK_AUTOMOTIVE_SUPPORT
    /*Step 2: Apply PDM_CLKI 512kHz by host and wait 1024 cycles at automotive product*/
    hal_gpt_delay_ms(10); //wait 10ms
#else
    /*Step 2: Apply PDM_CLKI 3.25MHz by host at bt headset produce, and wait 1024 cycles*/
    external_dsp_switch_pdm_clock(true);
    hal_gpt_delay_ms(10); //wait 10ms
#endif

    /*Step 3: Enable ext-DSP Reset Pin(optional) and reset DSP*/
    status = external_dsp_enter_download_mode();
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return status;
    }

    /*Step 4 : download ext-DSP firmare*/
    log_hal_info("external_dsp_load_firmware begins");
    external_dsp_load_firmware();
    hal_gpt_delay_ms(1);

    /*Step 5 : put DSP on run state*/
    external_dsp_exit_download_mode();

#ifndef MTK_BUILD_SMT_LOAD
    /*Step 6 : configure output source to PDM_DATO0/PDM_DATO1 */
    status = external_dsp_configure_data_path(EXP_DSP_OUTPUT_SOURCE);
#endif
#ifdef MTK_AUTOMOTIVE_SUPPORT
    external_dsp_auto_mode_t auto_mode;
    status = external_dsp_get_automotive_mode(&auto_mode);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return status;
    } else {    //update initial mode
        log_hal_info("DSP initial dspmode=%s", auto_mode == EXTERNAL_DSP_AUTO_BT_MODE ? "BT" : "VR");
        external_dsp.auto_mode = auto_mode;
    }
#endif
#ifndef MTK_BUILD_SMT_LOAD
#ifndef MTK_AUTOMOTIVE_SUPPORT
#if 0
    external_dsp_spi_deinit();
    external_dsp_spi_pinmux_switch(false);
#endif
    hal_gpt_delay_ms(10);
    external_dsp_switch_pdm_clock(false);
#endif
#endif
    return status;
}

/*set bit[10] of DRAM Address 0x0FFF_FF12 to "0".*/
external_dsp_status_t external_dsp_enable_dsp_pdm_clkO1(bool clk_O1)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                read_data, write_data = 0;

    log_hal_info("clk_O1=%x", clk_O1);
    if (clk_O1) {
        external_dsp_read_dram(0x0fffff10 , &write_data);
        write_data &= ~(1 << 26);
        status = external_dsp_write_dram(0x0fffff10 , write_data);
        if (EXTERNAL_DSP_STATUS_OK != status) {
            log_hal_error("failed to enable ext-DSP PDM_CLKO1.");
            return EXTERNAL_DSP_STATUS_ERROR;
        }
        external_dsp_read_dram(0x0fffff10, &read_data);
        if (read_data != write_data) {
            log_hal_error("Readback dram 0x0fffff10 is wrong. Expect data=%x, readback=%x", write_data, read_data);
            return EXTERNAL_DSP_STATUS_ERROR;
        }
    }
    return status;
}

external_dsp_status_t external_dsp_write_dram_word(uint32_t address, uint16_t data)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                write_data = 0, read_data = 0;
    uint32_t                write_address = address;

    log_hal_info("write dram address=%x, data=%x", address, data);

    write_address &= ~0x0003; // 4-byte align
    external_dsp_read_dram(write_address, &read_data);
    log_hal_info("[DBG]before write: address=%x, data=%x", write_address, read_data);
    if (address % 4 != 0) {
        write_data = (((uint32_t)data << 16) | (read_data & 0x0000FFFF));
    } else {
        write_data = ((read_data & 0xFFFF0000) | (data & 0xFFFF));
    }
    status = external_dsp_write_dram(write_address, write_data);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to write dram data.");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    external_dsp_read_dram(write_address, &read_data);
    log_hal_info("[DBG]after write: address=%x, data=%x", write_address, read_data);

    return status;
}

external_dsp_status_t external_dsp_read_dram_word(uint32_t address, uint16_t *data)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                read_data = 0;
    uint32_t                read_address = address;

    if(address % 2) {
        return EXTERNAL_DSP_STATUS_INVALID_PARAMETER;
    }

    read_address &= ~0x0003; // 4-byte align
    status = external_dsp_read_dram(read_address, &read_data);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to read dram data. ");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    if (address % 4 != 0) {
        *data = read_data >> 16;
    } else {
        *data = read_data & 0xFFFF;
    }
    log_hal_info("address=%x, read_data=%x, data=%x", address, read_data, *data);
    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_write_parameter(uint32_t index, uint16_t data)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                write_data = 0, read_data = 0;
    uint32_t                address = EXT_DSP_PARAMETER_BASE + 2*index;
    bool                    odd = (index %2) ? 1: 0;

    log_hal_info("write parameter index=%d, data=%x", (int)index, data);

    address &= ~0x0003; // 4-byte align
    external_dsp_read_dram(address, &read_data);
    if (odd) {
        write_data = (((uint32_t)data << 16) | (read_data & 0x0000FFFF));
    } else {
        write_data = ((read_data & 0xFFFF0000) | (data & 0xFFFF));
    }
    status = external_dsp_write_dram(address, write_data);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to write dram data.");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    return status;
}

external_dsp_status_t external_dsp_read_parameter(uint32_t index, uint16_t *data)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                read_data = 0;
    uint32_t                address = EXT_DSP_PARAMETER_BASE + 2*index;

    address &= ~0x0003; // 4-byte align
    status = external_dsp_read_dram(address, &read_data);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to read dram data. ");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    if (index%2) {
        *data = read_data >> 16;
    } else {
        *data = read_data & 0xFFFF;
    }
    log_hal_info("index=%d(%x), read_data=%x, data=%x", index, address, read_data, *data);
    return EXTERNAL_DSP_STATUS_OK;
}

void external_dsp_set_output_source(uint32_t config)
{
    external_dsp.output_source = config;
}

uint32_t external_dsp_get_output_source(void)
{
    return external_dsp.output_source;
}

#ifdef MTK_AUTOMOTIVE_SUPPORT
void external_dsp_set_auto_mode(external_dsp_auto_mode_t mode)
{
    external_dsp.auto_mode = mode;
}

external_dsp_auto_mode_t external_dsp_get_auto_mode(void)
{
    return external_dsp.auto_mode;
}

external_dsp_status_t external_dsp_switch_automotive_mode(external_dsp_auto_mode_t mode)
{
    external_dsp_status_t       status = EXTERNAL_DSP_STATUS_OK;
    uint32_t                    read_data = 0;
    external_dsp_auto_mode_t    prev_mode;

    log_hal_info("dspmode=%s", mode==EXTERNAL_DSP_AUTO_BT_MODE?"BT":"VR");

    status = external_dsp_get_automotive_mode(&prev_mode);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to get automotive mode.");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    if (mode == prev_mode) {
        /*mode has applied, no need to switch again. */
        log_hal_error("DSP has already been in %s mode. Don't need to change again.", mode == EXTERNAL_DSP_AUTO_BT_MODE ? "BT" : "VR");
        return EXTERNAL_DSP_STATUS_INVALID_PARAMETER;
    }

    status = external_dsp_write_dram(0x0fffbff8, mode);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to switch mode.");
        return EXTERNAL_DSP_STATUS_ERROR;
    }
    hal_gpt_delay_ms(50);
    status = external_dsp_write_register(EXT_DSP_STATE_REG, 0x41);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to write register 0x41");
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    hal_gpt_delay_ms(50);
    status =external_dsp_read_dram(0x0fffbf04, &read_data);  //read back address: 0x0fffbf06, but read dram address should be 4-byte align
    if (EXTERNAL_DSP_STATUS_OK != status) {
        log_hal_error("failed to read mode back.");
        return EXTERNAL_DSP_STATUS_ERROR;
    }

    read_data = read_data >> 16;
    if (read_data != mode) {
        log_hal_error("Readback dram 0x0fffbf06 is wrong. Expect data=%x, Readback=%x", mode, read_data);
        return EXTERNAL_DSP_STATUS_ERROR;
    } else {
        log_hal_info("Switch Automotive Mode to %x complete.", mode);
        external_dsp.auto_mode = mode;
    }

    external_dsp_configure_data_path(EXP_DSP_OUTPUT_SOURCE);
    if (mode == EXTERNAL_DSP_AUTO_VR_MODE) {

#ifndef MTK_AUDIO_TUNING_ENABLED
        car_dsp_tunning_nvdm_setting();
#endif
    }
    return EXTERNAL_DSP_STATUS_OK;
}

external_dsp_status_t external_dsp_get_automotive_mode(external_dsp_auto_mode_t *mode)
{
    uint32_t                    read_data = 0;

    hal_gpt_delay_ms(10);
    external_dsp_read_dram(0x0fffbf04,&read_data);  //read back address: 0x0fffbf06, but read dram address should be 4-byte align
    read_data = read_data >> 16;
    //log_hal_info(" automotive dspmode=%s", read_data==EXTERNAL_DSP_AUTO_BT_MODE?"BT":"VR");
    if (EXTERNAL_DSP_AUTO_BT_MODE != read_data && EXTERNAL_DSP_AUTO_VR_MODE != read_data) {
        *mode = 0;
        return EXTERNAL_DSP_STATUS_ERROR;
    } else {
        *mode = read_data;
        return EXTERNAL_DSP_STATUS_OK;
    }
}
#endif

#ifdef MTK_BUILD_SMT_LOAD
/*Ext-DSP download firmware flow*/
external_dsp_status_t external_dsp_i2s_firmware_loaded(void)
{
    external_dsp_status_t   status = EXTERNAL_DSP_STATUS_OK;

    /*Step 1: Enable VDD by HW */
    status = external_dsp_init();
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return status;
    }
    external_dsp_clk_sw_config(true);
    external_dsp_rtc_sw_config(true);
    hal_gpt_delay_ms(1);
    external_dsp_spi_init();

    /*Step 2: Apply PDM_CLKI 3.25M by MT2523 host and wait 1024 cycles*/
    external_dsp_switch_pdm_clock(true);
    hal_gpt_delay_ms(10); //wait 10ms

    /*Step 3: Enable ext-DSP Reset Pin(optional) and reset DSP*/
    status = external_dsp_enter_download_mode();
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return status;
    }

    /*Step 4 : download ext-DSP firmare*/
    log_hal_info("external_dsp_load_firmware begins");
    external_dsp_load_firmware();
    hal_gpt_delay_ms(1);

    /*Step 5 : put DSP on run state*/
    external_dsp_exit_download_mode();

    /*Step 6.1 : software reset again, only in SMT temp load */
    status = external_dsp_enter_download_mode();
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return status;
    }

    /*Step 6.2 : put DSP on run state again, only in SMT temp load */
    external_dsp_exit_download_mode();

    external_dsp_spi_deinit();

    external_dsp_spi_pinmux_switch(false);
    hal_gpt_delay_ms(10);

#if FW_DL_CHECK
    if (error1 | error2 | error3) {
        status = EXTERNAL_DSP_STATUS_ERROR;
    }
#endif
    return status;
}

external_dsp_status_t external_dsp_power_clk_on(void)
{
    external_dsp_status_t status = EXTERNAL_DSP_STATUS_OK;

    /*Step 1: Enable VDD by HW */
    status = external_dsp_init();
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return status;
    }
    external_dsp_clk_sw_config(true);
    external_dsp_rtc_sw_config(true);
    hal_gpt_delay_ms(1);
    external_dsp_spi_init();

    /*Step 2: Apply PDM_CLKI 3.25M by MT2523 host and wait 1024 cycles*/
    external_dsp_switch_pdm_clock(true);
    hal_gpt_delay_ms(10); //wait 10ms

    /*Step 3: Enable ext-DSP Reset Pin(optional) and reset DSP*/
    status = external_dsp_enter_download_mode();
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return status;
    }

    /*Step 4 : download ext-DSP firmare*/
    log_hal_info("turn on PDM_CLKO1");
    status = external_dsp_enable_dsp_pdm_clkO1(true);
    hal_gpt_delay_ms(1);
    if (EXTERNAL_DSP_STATUS_OK != status) {
        return status;
    }
    external_dsp_switch_pdm_clock(false);
    external_dsp_spi_deinit();
    external_dsp_spi_pinmux_switch(false);
    hal_gpt_delay_ms(10);

    return status;
}
#endif /*MTK_BUILD_SMT_LOAD*/
