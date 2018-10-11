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

#ifndef __EXTERNAL_DSP_DRIVER_H__
#define __EXTERNAL_DSP_DRIVER_H__
#include "hal_platform.h"

/* - UseEXTERNAL_DSP to download DSP's firmware. \n
*  - step1: call external_dsp_init() to power on DSP
*  - step2: call Audio's API to trigger PDM_CLKI 3.25M and wait 1024 cycles
*  - step3: call external_dsp_set_interface() to select spi to transfer
*  - step4: call external_dsp_download_firmware_binary() to download a dediacte binary which user needs .
*  - step5: call external_dsp_set_download_done() to make sure that DSP is changed from hold state to run state.
*  - sample code:
*    @code
*       ret = external_dsp_init(); //initialize theEXTERNAL_DSP source clock
*       if(DSP_OPERATION_STATUS_OK != ret) {
*             //error handle
*       }
*        //audio_trigger_3.25M_clock
*       ret = external_dsp_enter_download_mode(); //set download mode.
*       if(DSP_OPERATION_STATUS_OK != ret) {
*             //error handle
*       }
*       ret =external_dsp_download_firmware_binary(address_1, tx_buffer_1, length_1); //load bin1
*       if(HAL_DSP_OPERATION_STATUS_OK != ret) {
*             //error handle
*       }
*       ret =external_dsp_download_firmware_binary(address_2, tx_buffer_2, length_2); //load bin2
*       if(HAL_DSP_OPERATION_STATUS_OK != ret) {
*             //error handle
*       }
*       ret =external_dsp_download_firmware_binary(address_3, tx_buffer_3, length_3); //load bin3
*       if(HAL_DSP_OPERATION_STATUS_OK != ret) {
*             //error handle
*       }
*       external_dsp_exit_download_mode();
*       //...
*       external_dsp_write_dram(address, data);
*       external_dsp_read_dram(address, &data);
*        external_dsp_deinit();                            // de-initialize theEXTERNAL_DSP.
*
*    @endcode
*/


#ifdef __cplusplus
extern "C" {
#endif

/** @brief DSP operation status */
typedef enum {
    EXTERNAL_DSP_STATUS_ERROR                   = -2,         /**< DSP operation function ERROR */
    EXTERNAL_DSP_STATUS_INVALID_PARAMETER       = -1,         /**< DSP operation error channel */
    EXTERNAL_DSP_STATUS_OK   = 0,                             /**< DSP operation function OK*/
} external_dsp_status_t;

extern const char EXT_DSP_POWER_SW_PIN;
extern const char EXT_DSP_POWER_SW_PIN_M_GPIO;

extern const char EXT_DSP_32K_CLK_PIN;
extern const char EXT_DSP_32K_CLK_PIN_M_GPIO;
extern const char EXT_DSP_32K_CLK_PIN_M_CLKO;

extern const char HAL_SPI_MASTER_3_CS_PIN;
extern const char HAL_SPI_MASTER_3_CS_PIN_M_GPIO;
extern const char HAL_SPI_MASTER_3_CS_PIN_M_MA_SPI3_B_CS;

extern const char HAL_SPI_MASTER_3_SCK_PIN;
extern const char HAL_SPI_MASTER_3_SCK_PIN_M_GPIO;
extern const char HAL_SPI_MASTER_3_SCK_PIN_M_MA_SPI3_B_SCK;

extern const char HAL_SPI_MASTER_3_MOSI_PIN;
extern const char HAL_SPI_MASTER_3_MOSI_PIN_M_GPIO;
extern const char HAL_SPI_MASTER_3_MOSI_PIN_M_MA_SPI3_B_MOSI;

extern const char HAL_SPI_MASTER_3_MISO_PIN;
extern const char HAL_SPI_MASTER_3_MISO_PIN_M_GPIO;
extern const char HAL_SPI_MASTER_3_MISO_PIN_M_MA_SPI3_B_MISO;

extern const char EXT_DSP_CLK_SW_PIN;
extern const char EXT_DSP_CLK_SW_PIN_M_GPIO;

extern const char EXT_DSP_RTC_CLK_SW_PIN;
extern const char EXT_DSP_RTC_CLK_SW_PIN_M_GPIO;


void external_dsp_spi_pinmux_switch(bool spi_mode);
void external_dsp_rtc_pinmux_switch(bool rtc_mode);

void external_dsp_power_on(bool on);
void external_dsp_clk_sw_config(bool pdm_clk);
void external_dsp_rtc_sw_config(bool rtc_clk);
void external_dsp_echo_ref_sw_config(bool bt_device);

external_dsp_status_t external_dsp_spi_init(void);
external_dsp_status_t external_dsp_spi_deinit(void);

external_dsp_status_t external_dsp_enter_download_mode(void);
external_dsp_status_t external_dsp_download_firmware_binary(uint32_t address, const uint8_t *tx_buffer, uint32_t length);
external_dsp_status_t external_dsp_exit_download_mode(void);

external_dsp_status_t external_dsp_read_register(uint8_t register, uint8_t *value);
external_dsp_status_t external_dsp_write_register(uint8_t register, uint8_t value);

external_dsp_status_t external_dsp_read_dram(uint32_t address, uint32_t *data);
external_dsp_status_t external_dsp_write_dram(uint32_t address, uint32_t data);
external_dsp_status_t external_dsp_burst_read(uint32_t address, uint8_t *rx_buffer, uint32_t length);
external_dsp_status_t external_dsp_burst_write(uint32_t address, const uint8_t *tx_buffer, uint32_t length);

#ifdef __cplusplus
}
#endif




#endif /* __EXTERNAL_DSP_DRIVER_H__ */


