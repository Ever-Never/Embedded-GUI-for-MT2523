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

#include "string.h" /* memset */
#include "FreeRTOS.h"
#include "task.h"
#include "bma2x2.h"
#include "bma255_i2c_operation.h"
#include "semphr.h"
#include "syslog.h"
#include "hal_i2c_master.h"
#include "mems_bus.h"
#include "semphr.h"
#include "memory_attribute.h"
#include "task.h"



log_create_module(bma255_i2_driver, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(bma255_i2_driver, "[BMA255]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(bma255_i2_driver, "[BMA255]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(bma255_i2_driver, "[BMA255]"fmt,##arg)

#define	I2C_BUFFER_LEN 8

extern struct bma2x2_t bma2x2;
SemaphoreHandle_t irq_res;
#ifndef __ICCARM__
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) array[I2C_BUFFER_LEN];
#else
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  array[I2C_BUFFER_LEN];
#endif
s8 BMA2x2_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    s32 iError = 0, i = 0;;
    //u8 array[I2C_BUFFER_LEN];
    u8 stringpos = 0;

    memset(array, 0, sizeof(array));
    array[0] = reg_addr;
    for (stringpos = 0; stringpos < cnt; stringpos++) {
        array[stringpos + 1] = *(reg_data + stringpos);
    }
    for ( i = 0; i < 5; i ++) {
        iError = mems_i2c_write(dev_addr, array, cnt + 2);
        if (!iError) {
            break;
        }
        vTaskDelay(1);
    }

    return (s8)iError;
}

s8 BMA2x2_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    s32 iError = 0, i = 0;
    u8 stringpos = 0;

    /* Please take the below function as your reference
     * for read the data using I2C communication
     * add your I2C rad function here.
     * "IERROR = I2C_WRITE_READ_STRING(DEV_ADDR, ARRAY, ARRAY, 1, CNT)"
     * iError is an return value of SPI write function
     * Please select your valid return value
     * In the driver SUCCESS defined as 0
     * and FAILURE defined as -1
     */
    for ( i = 0; i < 5; i ++) {
        iError = mems_i2c_write_read(dev_addr, reg_addr, array, cnt);        

        if (!iError) {
            break;
        }
        vTaskDelay(1);
    }

    for (stringpos = 0; stringpos < cnt; stringpos++) {
        *(reg_data + stringpos) = array[stringpos];
    }
    return (s8)iError;
}

void BMA2x2_delay_msek(u32 msek)
{
    /*Here you can write your own delay routine*/
    vTaskDelay(msek);
}


s8 I2C_routine(void) {
/*--------------------------------------------------------------------------*
 *  By using bma2x2 the following structure parameter can be accessed
 *  Bus write function pointer: BMA2x2_WR_FUNC_PTR
 *  Bus read function pointer: BMA2x2_RD_FUNC_PTR
 *  Delay function pointer: delay_msec
 *  I2C address: dev_addr
 *--------------------------------------------------------------------------*/
    bma2x2.bus_write = BMA2x2_I2C_bus_write;
    bma2x2.bus_read = BMA2x2_I2C_bus_read;
    bma2x2.delay_msec = BMA2x2_delay_msek;
    bma2x2.dev_addr = BMA2x2_I2C_ADDR1;

    return 0;
}

