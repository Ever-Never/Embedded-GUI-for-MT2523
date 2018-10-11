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

#ifndef _CTP_GOODIX_GT9XXX_H_
#define _CTP_GOODIX_GT9XXX_H_

#ifdef MTK_CTP_ENABLE

#include "touch_panel_custom_config.h"

#ifdef CTP_GT9137_SUPPORTED

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif


#define CTP_SLAVE_ADDR            		  	0xBA
#define CTP_POWER_MODE_REG        			0x8040

#define CTP_FW_HEADER_SIZE                	0

#define CTP_VERSION_INFO_REG             	0x8140
//#define CTP_MASTER_RESET_REG              0x4180
//#define CTP_MASTER_STATUS_REG             0x6088
#define CTP_MASTER_CODE_REG               	0xC000
//#define CTP_DSP_ISP_BOOT_CODE_REG         0x9000
//#define CTP_SRAM_BAND_REG                 0x4048
#define CTP_FIRMWARE_SIZE                 	(0x8000+CTP_FW_HEADER_SIZE)
//#define CTP_DSP_FIRMWARE_SIZE             (0x1000+CTP_FW_HEADER_SIZE)
//#define CTP_BAND_SIZE                     0x4000
//#define CTP_SS51_DOWNLOAD_SIZE            0x2000
//#define CTP_DSP_ISP_DOWNLOAD_SIZE         0x1000
//#define CTP_DSP_DOWNLOAD_SIZE             0x1000
//#define CTP_BOOTCODE_DOWNLOAD_SIZE        0x800
#define CTP_TOUCH_INFO_REG_BASE           	0x814E
#define CTP_POINT_INFO_REG_BASE           	0x814F
//#define CTP_DIFF_DATA_REG                 0xBB10
//#define CTP_RAW_DATA_REG                  0x8B98
//#define CTP_FLASH_RUN_REG                 0x4283
#define CTP_CONFIG_REG_BASE               	0x8047
//#define CTP_BUFFER_STATUS_SHIFT           7
#define CTP_FIRMWARE_VERSION              	0x8147
#define CTP_NVRAM_I2C_ADDRESS             	0x5094
#define CTP_NVRAM_OPERATION_SIZE          	0x5096
#define CTP_NVRAM_FILE_OFFSET             	0x4196
#define CTP_NVRAM_OPERATION_CHECKSUM      	0x4194
#define CTP_FLASH_POWER_OFF               	0x4284

#define CTP_SRAM_BANK                     	0x4048
#define CTP_MEM_CD_EN                     	0x4049
#define CTP_CACHE_EN                      	0x404B
#define CTP_TMR0_EN                       	0x40B0
#define CTP_SWRST_B0_                     	0x4180
#define CTP_CPU_SWRST_PULSE               	0x4184
#define CTP_BOOTCTL_B0_                   	0x4190
#define CTP_BOOT_OPT_B0_                  	0x4218
#define CTP_BOOT_CTL_                     	0x5094//0x4283
#define CTP_ANA_RXADC_B0_                 	0x4250
#define CTP_RG_LDO_A18_PWD                	0x426f
#define CTP_RG_BG_PWD                     	0x426a
#define CTP_RG_CLKGEN_PWD                 	0x4269
#define CTP_RG_DMY                        	0x4282
#define CTP_RG_OSC_CALIB                  	0x4268
#define CTP_OSC_CK_SEL                    	0x4030


#define CTP_STAT_NUMBER_TOUCH     			0xF

#define CTP_POINT_INFO_LEN        			8

typedef struct {
    uint8_t vendor_id_1;
    uint8_t vendor_id_2;
    uint8_t product_id_1;
    uint8_t product_id_2;
    uint8_t version_1;
    uint8_t version_2;
} ctp_info_t;

typedef struct {
    uint8_t *data;
    uint8_t  length;
} gt9xx_config_data_t;

/********* varible extern *************/
extern gt9xx_config_data_t	gt9xx_config_data;



/******** funtion extern **************/
extern bool ctp_i2c_bottom_read(uint8_t device_addr, uint16_t ctp_reg_addr, uint8_t *data_buffer, uint32_t data_leng);
extern bool ctp_i2c_bottom_write(uint8_t device_addr, uint16_t ctp_reg_index, uint8_t *data_buffer, uint32_t data_length);

#define CTP_I2C_read(a,b,c) ctp_i2c_bottom_read( CTP_SLAVE_ADDR, a, b, c)
#define CTP_I2C_send(a,b,c) ctp_i2c_bottom_write( CTP_SLAVE_ADDR, a, b, c)

bool ctp_goodix_gt9xx_set_configuration(void);
bool ctp_goodix_gt9xx_get_information(ctp_info_t *ctp_info);

uint32_t save_and_set_interrupt_mask(void);
void restore_interrupt_mask(uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif /*CTP_GT9137_SUPPORTED*/
#endif /*MTK_CTP_ENABLE*/
#endif /*_CTP_GOODIX_GT9XXX_H_*/


