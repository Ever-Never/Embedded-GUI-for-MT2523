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

#ifdef MTK_CTP_ENABLE

#include "touch_panel_custom_config.h"

#ifdef CTP_GT9137_SUPPORTED
#include <string.h>
#include "ctp.h"
#include "ctp_goodix_gt9xxx.h"
#include "hal_gpt.h"
#include "gt9xxx_config.h"
#include "touch_panel_custom_goodix.h"
#include "hal_log.h"
#include "hal_nvic_internal.h"

gt9xx_config_data_t gt9xx_config_data;

ctp_custom_information_struct  ctp_goodix_gt9xx_custom_information_def = {
    "Goodix ",
    "GT9xx",
    "UNKNOWN ",
};

static bool ctp_read_one_point(uint32_t x_base, bsp_ctp_single_event_t *event)
{
    uint8_t point_info[CTP_POINT_INFO_LEN];

    CTP_I2C_read(x_base, point_info, CTP_POINT_INFO_LEN);

    event->x = point_info[1] + (point_info[2] << 8);
    event->y = point_info[3] + (point_info[4] << 8);

    return true;

}


static bool ctp_read_all_point(bsp_ctp_multiple_event_t *tpes, uint32_t points)
{
    uint32_t          i;
    bsp_ctp_single_event_t get_one_point;

    if ((points < 1) || (points > 5)) {
        return false;
    }

    for (i = 0; i < points; i++) {
        ctp_read_one_point(CTP_POINT_INFO_REG_BASE + CTP_POINT_INFO_LEN * i , &get_one_point);
        tpes->points[i].x = get_one_point.x;
        tpes->points[i].y = get_one_point.y;
    }

    return true;

}



static bool ctp_goodix_gt9xx_get_data(bsp_ctp_multiple_event_t *tpes)
{
    uint8_t  lvalue = 0;
    uint8_t  model = 0;
    uint8_t  buf_status;
    bool     ret = true;
    uint8_t  rst_char = 0;

    uint32_t temp_data;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &temp_data);
    tpes->time_stamp =  temp_data;
    tpes->padding    = CTP_PATTERN;

    /* get number of touch points */
    CTP_I2C_read(CTP_TOUCH_INFO_REG_BASE, &lvalue, 1);
    model      = (uint8_t)(lvalue & CTP_STAT_NUMBER_TOUCH);
    buf_status = lvalue & 0xF0;

    if (model > 5) { //goodix only support 5 fingers
        ret = false;
        goto exit_get_data;
    }

    tpes->model = (uint8_t)model;  //read out all touch points coordinates.

    if (model == 0) {
        ret = false;
        CTP_I2C_read(CTP_POINT_INFO_REG_BASE + 1 , &lvalue, 1);
    } else {
        /*bit7:1 indicate coordinates or key is ready , CPU can read out */
        if ((buf_status & 0x80) != 0) {
            ctp_read_all_point(tpes, model);
        }
    }

exit_get_data:

    CTP_I2C_send(CTP_TOUCH_INFO_REG_BASE , &rst_char, 1);

    return ret;

}


static ctp_pen_state_enum_t ctp_goodix_gt9xx_hisr(void)
{
    uint8_t lvalue;

    CTP_I2C_read(CTP_TOUCH_INFO_REG_BASE, &lvalue, 1);

    if (lvalue & CTP_STAT_NUMBER_TOUCH) {
        return CTP_DOWN;
    } else {
        return CTP_UP;
    }
}

static bool ctp_goodix_gt9xx_parameters(ctp_parameters_struct_t *para, uint32_t get_para, uint32_t set_para)
{
    return true;
}



void ctp_update_firmware_enable_wdt(bool enable)
{

    /*WDT_CTRL_ENABLE_T wdt_data;
    DCL_HANDLE init_dcl_wdt_handle;

    wdt_data.fgEnable=enable;
    init_dcl_wdt_handle=DclWDT_Open(DCL_WDT,0);
    DclWDT_Control(init_dcl_wdt_handle,WDT_CMD_ENABLE,(DCL_CTRL_DATA_T*)&wdt_data);
    DclWDT_Close(init_dcl_wdt_handle);*/
}

static bool ctp_goodix_gt9xx_set_device_mode(ctp_device_mode_enum_t mode)
{
    uint8_t     suspend_command = 0x05;

    uint8_t     ctp_buffer[2] = {0};

    if (mode == CTP_ACTIVE_MODE) {
        //gup_update_proc(NULL);
        //gtp_reset_guitar(NULL,20);
        //kal_prompt_trace(MOD_TP_TASK,"ctp_sleep_in");
        hal_pinmux_set_function(ctp_config_info.ctp_eint.eint_pin, 0);
        hal_gpio_set_direction(ctp_config_info.ctp_eint.eint_pin, HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_set_output(ctp_config_info.ctp_eint.eint_pin, HAL_GPIO_DATA_HIGH);
        ctp_delay_ms(15);

        /* set enit pin to eint mode */
        hal_pinmux_set_function(ctp_config_info.ctp_eint.eint_pin, 1);
    } else if ((mode == CTP_SLEEP_MODE)) {
        hal_pinmux_set_function(ctp_config_info.ctp_eint.eint_pin, 0);
        hal_gpio_set_direction(ctp_config_info.ctp_eint.eint_pin, HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_set_output(ctp_config_info.ctp_eint.eint_pin, HAL_GPIO_DATA_LOW);
        ctp_delay_ms(5);

        /* close screen */
        CTP_I2C_send(CTP_POWER_MODE_REG, &suspend_command, 1);
        CTP_I2C_read(CTP_POWER_MODE_REG, ctp_buffer, 1);
        log_hal_info("[CTP]CTP_POWER_MODE_REG = 0x%x\r\n", (int)ctp_buffer[0]);
    } else if (mode == CTP_FIRMWARE_UPDATE) {
        ctp_update_firmware_enable_wdt(false);
        //gup_update_proc(NULL); // need to do by lei dou 2015.11.10 20:50
        ctp_update_firmware_enable_wdt(true);
        //gtp_reset_guitar(NULL,5);
        //gtp_reset_guitar(NULL, 5);
        //ctp_goodix_gt9xx_init();

    }

    return false;

}


static void ctp_goodix_gt9xx_power(bool on)
{
    if (on == true) {
        ctp_goodix_gt9xx_set_device_mode(CTP_ACTIVE_MODE);
    } else {
        ctp_goodix_gt9xx_set_device_mode(CTP_SLEEP_MODE);
    }
}


//If controller has the register store these informations
//Read out the informations from controller through I2C

bool ctp_goodix_gt9xx_get_information(ctp_info_t *ctp_info)
{
    uint8_t cfg;

    /* get product ID and firmware version */
    CTP_I2C_read(CTP_VERSION_INFO_REG, (uint8_t *)ctp_info, sizeof(ctp_info_t));

    ctp_goodix_gt9xx_custom_information_def.vendor[0]           = ctp_info->vendor_id_1;
    ctp_goodix_gt9xx_custom_information_def.vendor[1]           = ctp_info->vendor_id_2;
    ctp_goodix_gt9xx_custom_information_def.product[0]          = ctp_info->product_id_1;
    ctp_goodix_gt9xx_custom_information_def.product[1]          = ctp_info->product_id_2;
    ctp_goodix_gt9xx_custom_information_def.firmware_version[0] = ctp_info->version_1;
    ctp_goodix_gt9xx_custom_information_def.firmware_version[1] = ctp_info->version_2;

    /* get config file version */
    CTP_I2C_read(CTP_CONFIG_REG_BASE, &cfg, 1);

    log_hal_info("[CTP]verdorID  verndor_1 = 0x%x, verndor_0 = 0x%x\r\n", ctp_goodix_gt9xx_custom_information_def.vendor[1], ctp_goodix_gt9xx_custom_information_def.vendor[0]);
    log_hal_info("[CTP]productID product_1 = 0x%x, product_0 = 0x%x\r\n", ctp_goodix_gt9xx_custom_information_def.product[1], ctp_goodix_gt9xx_custom_information_def.product[0]);
    log_hal_info("[CTP]firmware  version_1 = 0x%x, version_0 = 0x%x\r\n", ctp_goodix_gt9xx_custom_information_def.firmware_version[1], ctp_goodix_gt9xx_custom_information_def.firmware_version[0]);

    if ((ctp_info->vendor_id_1  != '9')  || \
            (ctp_info->vendor_id_2  != '1')  || \
            (ctp_info->product_id_1 != '3') || \
            (ctp_info->product_id_2 != '7')) {
        log_hal_info("[CTP]this touch is not goodix gt9137\r\n");
        return false;
    } else {
        log_hal_info("[CTP]this touch is goodix gt9137\r\n");
        return true;
    }
}

bool ctp_goodix_gt9xx_set_configuration(void)
{
    bool        ret;
    uint32_t    mask = 0;
#ifdef CTP_DEBUG_LOG
    uint32_t    i;
    char        buf[200];
#endif


    mask = save_and_set_interrupt_mask();
    ret = CTP_I2C_send(CTP_CONFIG_REG_BASE, gt9xx_config , sizeof(gt9xx_config));
    restore_interrupt_mask(mask);

    if (ret != true) {
        log_hal_info("[CTP]i2c send error\r\n");
        return false;
    }


#ifdef CTP_DEBUG_LOG
    ctp_delay_ms(10);
    memset(&buf, 0, sizeof(buf));
    ret = CTP_I2C_read(CTP_CONFIG_REG_BASE, (uint8_t *)buf, sizeof(gt9xx_config));
    if (ret != true) {
        log_hal_info("[CTP]i2c receive error\r\n");
        return false;
    }

    log_hal_info("[CTP]read configuration data\r\n");

    for (i = 0; i < sizeof(gt9xx_config); i++) {
        if (buf[i] != gt9xx_config[i]) {
            log_hal_info("[W]0x%.2x:[R]0x%.2x [ADDR]0x%x  [ERR]\r\n", gt9xx_config[i], buf[i], CTP_CONFIG_REG_BASE + i);
        } else {
            log_hal_info("[W]0x%.2x:[R]0x%.2x [ADDR]0x%x [OK]\r\n", gt9xx_config[i], buf[i], CTP_CONFIG_REG_BASE + i);
        }
    }
#endif


    return ret;
}


static uint32_t ctp_goodix_gt9xx_command(ctp_control_command_enum_t cmd, void *p1, void *p2) // p1: input p2: output
{
#define TPD_SYNC_REG                  0x711
#define TPD_DIFF_DATA_REG             0xD80
#define TPD_CHANNEL_CONFIG_REG        0x6D5
#define TPD_RAW_DATA_1_REG            0x880
#define TPD_RAW_DATA_2_REG            0x9C0

#define MAX_DRIVING_CHANNEL           16
#define MAX_SENSING_CHANNLE           10
#define MAX_DIFF_DATA_SIZE            (MAX_DRIVING_CHANNEL*MAX_SENSING_CHANNLE*2)
#define MAX_RAW_DATA_SIZE             MAX_DIFF_DATA_SIZE


    ctp_info_t  ctp_info;
    bool        ret = true;

    switch (cmd) {
        case CTP_COMMAND_GET_VERSION: // get firmware version
            if (ctp_goodix_gt9xx_get_information(&ctp_info)) {
                memcpy(p2, (uint8_t *)&ctp_info, sizeof(ctp_info_t));
            } else {
                ret = false;
            }
            break;

        case CTP_COMMAND_GET_CONFIG:
            if (CTP_I2C_read(CTP_CONFIG_REG_BASE, p2, GOODIX_CTP_CONFIG_LENGTH)) {
            } else {
                ret = false;
            }
            break;

        case CTP_COMMAND_LOAD_INT_CONFIG:
            ctp_goodix_gt9xx_set_configuration();
            break;

        case CTP_COMMAND_LOAD_EXT_CONFIG:
            CTP_I2C_send(CTP_CONFIG_REG_BASE, p1, GOODIX_CTP_CONFIG_LENGTH);
            break;

        case CTP_COMMAND_GET_DIFF_DATA:
            break;

        case CTP_COMMAND_GET_FW_BUFFER:
            break;

        case CTP_COMMAND_DO_FW_UPDATE:
            break;

        default:
            ret = false;
    }

    return ret;

}
static bool ctp_goodix_gt9xx_init(void)
{
    bool        ret;
    ctp_info_t  ctp_info;
    uint32_t    flag = 0;
    uint32_t    time[10];
    uint32_t    timeout_count = 0;


    gt9xx_config_data.data   = (uint8_t *)gt9xx_config;
    gt9xx_config_data.length = sizeof(gt9xx_config);

    time[0] = ctp_get_time_us();

    log_hal_info("[CTP][init]start reset sequence\r\n");

    /* CTP reset flow start , need to control pinmux **/
    /***************************  reset flow **************************************/
    /* set eint  pin to gpio mode and output low */
    hal_gpio_init(ctp_config_info.ctp_eint.eint_pin);
    hal_gpio_init(ctp_config_info.reset_pin);
    hal_pinmux_set_function(ctp_config_info.ctp_eint.eint_pin, ctp_config_info.ctp_eint.gpio_mode);
    hal_gpio_set_direction(ctp_config_info.ctp_eint.eint_pin, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(ctp_config_info.ctp_eint.eint_pin, HAL_GPIO_DATA_LOW);

    /* set reset pin to gpio mode and output low */
    hal_pinmux_set_function(ctp_config_info.reset_pin, 0);
    hal_gpio_set_direction(ctp_config_info.reset_pin, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(ctp_config_info.reset_pin, HAL_GPIO_DATA_LOW);
    ctp_delay_ms(5);

    hal_gpio_set_output(ctp_config_info.reset_pin, HAL_GPIO_DATA_HIGH);
    ctp_delay_ms(60);

    /***************************  eint flow **************************************/
    /* set enit pin to eint mode */
    hal_pinmux_set_function(ctp_config_info.ctp_eint.eint_pin, ctp_config_info.ctp_eint.eint_mode);          //set eint mode
    hal_gpio_set_direction(ctp_config_info.ctp_eint.eint_pin, HAL_GPIO_DIRECTION_INPUT);
    ctp_delay_ms(10);

    log_hal_info("[CTP][init]end reset sequence\r\n");
    /* CTP reset fllow end **/

    while (ctp_hw_i2c_init(&ctp_config_info.ctp_i2c) == false) {
        timeout_count++;
        log_hal_info("[CTP][init]ctp_hw_i2c_init, cannot get i2c %d time\r\n", timeout_count);
        ctp_delay_ms(10);
        if (timeout_count >= 30) {
            log_hal_info("[CTP][init]ctp_hw_i2c_init, get i2c fail\r\n");
            return false;
        }
    }

    ret = ctp_goodix_gt9xx_get_information(&ctp_info);
    if (ret != true) {
        log_hal_info("[CTP]read information fail!!!\r\n");
        flag++;
    }

    ret = ctp_goodix_gt9xx_set_configuration();
    if (ret != true) {
        log_hal_info("[CTP]ctp_goodix_gt9xx_set_configuration fail!!!\r\n");
        flag++;
    } else {
        log_hal_info("[CTP]ctp_goodix_gt9xx_set_configuration OK\r\n");
    }



    ctp_hw_i2c_deinit();

    time[1] = ctp_get_duration_time_us_to_ms(time[0]);

    log_hal_info("[CTP]ctp_goodix_gt9xx init period time = %d ms\r\n", time[1]);

    if (flag != 0) {
        log_hal_info("[CTP]ctp_goodix_gt9xx init flow [Error], flag = %d!\r\n", flag);
        return false;
    } else {
        log_hal_info("[CTP]ctp_goodix_gt9xx init flow [OK]!\r\n ");
        return true;
    }


}

bool ctp_goodix_gt9xx_valid(void)
{
    uint8_t illegal_int = 0;
    bool ret;

    /* if invalid data, skip */
    ret = CTP_I2C_read(CTP_TOUCH_INFO_REG_BASE, &illegal_int, 1);
    
    #ifdef CTP_DEBUG_POINT_BUFFER
    log_hal_info("[CTP]CTP_TOUCH_INFO_REG_BASE(0x814E) = %x\r\n",illegal_int);
    #endif
    if ((ret == false) || ((illegal_int & 0x80) == 0)) {

        return false;
    } else {
        return true;
    }
}

static ctp_customize_function_struct ctp_goodix_gt9xx_custom_function = {
    ctp_goodix_gt9xx_init,
    ctp_goodix_gt9xx_set_device_mode,
    ctp_goodix_gt9xx_hisr,
    ctp_goodix_gt9xx_get_data,
    ctp_goodix_gt9xx_parameters,
    ctp_goodix_gt9xx_power,
    ctp_goodix_gt9xx_command,
    ctp_goodix_gt9xx_valid
};

ctp_customize_function_struct *ctp_goodix_gt9xx_get_custom_function(void)
{
    return (&ctp_goodix_gt9xx_custom_function);
}

#endif /*CTP_GT9137_SUPPORTED*/

#endif /*MTK_CTP_ENABLE*/

