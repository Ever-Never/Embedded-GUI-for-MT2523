/*
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
#ifndef __CTP_ITE_IT7XXX_H__
#define __CTP_ITE_IT7XXX_H__

#ifdef MTK_CTP_ENABLE

#include "touch_panel_custom_config.h"
#ifdef CTP_IT7258_SUPPORTED

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

//Controller parameter getting/setting flag
#define IT_CTP_SLAVE_ADDR              (0x8c)

#define MAX_FINGER_NUM                  2

#define COMMAND_BUFFER_INDEX            0x20
#define QUERY_BUFFER_INDEX              0x80
#define COMMAND_RESPONSE_BUFFER_INDEX   0xA0
#define SYSTEM_COMMAND_RESPONSE_INDEX   0xC0
#define POINT_BUFFER_INDEX              0xE0

#define POINT_DATA_SIZE                 64

#define QUERY_BUFFER_NEW_POINT_INFO     0x80
#define POINT_BUFFER_FORMAT_MASK        0xF0
#define POINT_BUFFER_FORMAT_POINT       0x00
#define POINT_BUFFER_FORMAT_GESTURE     0x80

#define POINT_BUFFER_POINT_INFO_MASK    0x07
#define POINT_BUFFER_POINT_OFFSET       0x00
#define POINT_BUFFER_POINT_INFO_0       0x01
#define POINT_BUFFER_POINT_INFO_1       0x02
#define POINT_BUFFER_POINT_INFO_2       0x04

#define POINT_BUFFER_X0_LSB_OFFSET      0x02
#define POINT_BUFFER_X0_MSB_OFFSET      0x03
#define POINT_BUFFER_Y0_LSB_OFFSET      0x04
#define POINT_BUFFER_Y0_MSB_OFFSET      0x03
#define POINT_BUFFER_Z0_OFFSET          0x05

#define POINT_BUFFER_X1_LSB_OFFSET      0x06
#define POINT_BUFFER_X1_MSB_OFFSET      0x07
#define POINT_BUFFER_Y1_LSB_OFFSET      0x08
#define POINT_BUFFER_Y1_MSB_OFFSET      0x07
#define POINT_BUFFER_Z1_OFFSET          0x09

#define POINT_BUFFER_X2_LSB_OFFSET      0x0A
#define POINT_BUFFER_X2_MSB_OFFSET      0x0B
#define POINT_BUFFER_Y2_LSB_OFFSET      0x0C
#define POINT_BUFFER_Y2_MSB_OFFSET      0x0B
#define POINT_BUFFER_Z2_OFFSET          0x0D

#define POINT_BUFFER_X_MSB_MASK         0x0F
#define POINT_BUFFER_X_MSB_SHIFT        0x08 //shift << 8
#define POINT_BUFFER_Y_MSB_MASK         0xF0
#define POINT_BUFFER_Y_MSB_SHIFT        0x04 //shift << 4
#define POINT_BUFFER_Z_MASK             0xF

#define COMMAND_IDENTIFY_SENSOR                     0x0
#define COMMAND_INQUIRY_SENSOR                      0x1
#define COMMAND_INQUIRY_SENSOR_SUB_FIRMWARE         0x00
#define COMMAND_INQUIRY_SENSOR_SUB_CONFIGURATION    0x06

#define COMMAND_SET_POWER_MODE                      0x4
#define COMMAND_RESET_QUEUE                         0x7
#define COMMAND_ENTER_FW_UPGRADE                    0x0
#define COMMAND_EXIT_FW_UPGRADE                     0x80
#define COMMAND_FW_UPGRADE_MODE                     0x60
#define COMMAND_SET_FW_OFFSET                       0x9
#define COMMAND_WRITE_FLASH                         0xA
#define COMMAND_READ_FLASH                          0xB
#define COMMAND_REINIT_FW                           0xC
#define COMMAND_WRITE_MEMORY                        0xD
#define COMMAND_WRITE_REGISTER                      0xE0
#define COMMAND_READ_MEMORY                         0xE
#define COMMAND_READ_REGISTER                       0xE1
#define COMMAND_IDLE_SLEEP_MODE                     0x11
#define COMMAND_SET_TIME_INTERVAL                   0x12
#define COMMAND_MP_CALIBRATION                      0x13
#define COMMAND_SET_POINT_THRESHOLD                 0x15

#define COMMAND_DATA_TYPE_BYTE                      0x1
#define COMMAND_DATA_TYPE_WORD                      0x2
#define COMMAND_DATA_TYPE_DWORD                     0x4

#define COMMAND_POWER_IDLE_MODE                     0x1
#define COMMAND_POWER_SLEEP_MODE                    0x2

#define FW_UPDATE_START                             0xF1
#define FW_UPDATE_ERROR                             0x2F
#define FW_UPDATE_FINISH                            0x87

#define COMMAND_RESPONSE_SUCCESS                    0x0000
#define QUERY_BUFFER_COMMAND_STATUS                 0x07
#define SIGNATURE_LENGTH                            16

#define CTP_I2C_WRITE(reg, data) ctp_i2c_bottom_write(IT_CTP_SLAVE_ADDR, reg, data, 1)
#define CTP_I2C_MULTI_WRITE(data, len) ctp_i2c_bottom_write(IT_CTP_SLAVE_ADDR, *data, data+1, len-1)
#define CTP_I2C_READ(reg, data, len) ctp_i2c_bottom_read(IT_CTP_SLAVE_ADDR, reg, data, len)

/******** funtion extern **************/
extern bool ctp_i2c_bottom_read(uint8_t device_addr, uint16_t ctp_reg_addr, uint8_t *data_buffer, uint32_t data_leng);
extern bool ctp_i2c_bottom_write(uint8_t device_addr, uint16_t ctp_reg_index, uint8_t *data_buffer, uint32_t data_length);

bool ctp_ite_get_firmware_information(ctp_custom_information_struct *str);
#define CTP_I2C_read(a,b,c) ctp_i2c_bottom_read( IT_CTP_SLAVE_ADDR, a, b, c)
#define CTP_I2C_send(a,b,c) ctp_i2c_bottom_write( IT_CTP_SLAVE_ADDR, a, b, c)

#ifdef __cplusplus
}
#endif

#endif /*CTP_IT7258_SUPPORTED*/

#endif /*MTK_CTP_ENABLE*/

#endif /*__CTP_ITE_IT7XXX_H__*/

