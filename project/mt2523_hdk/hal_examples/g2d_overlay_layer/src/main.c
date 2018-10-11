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

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* hal includes */
#include "hal.h"
#include "hal_g2d.h"
#include "hal_log.h"
#include "hal_uart.h"

#include "bsp_gpio_ept_config.h"
#include "bsp_lcd.h"
#include "mt25x3_hdk_lcd.h"
#include "mt25x3_hdk_backlight.h"
#include "memory_attribute.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern unsigned char wallpaper[];
extern unsigned char brandmark[];

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char font_layer[16 * 6];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char frame_buffer[320 * 320 * 3];

const unsigned char c_4C[16] = {
0x00, 0x00, 0x00, 0x07, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x42, 0x7F, 0x00, 0x00
}; /* L */
const unsigned char c_69[16] = {
0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00
}; /* i */
const unsigned char c_6E[16] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x46, 0x42, 0x42, 0x42, 0x42, 0xE7, 0x00, 0x00
}; /* n */
const unsigned char c_6B[16] = {
0x00, 0x00, 0x00, 0x03, 0x02, 0x02, 0x02, 0x72, 0x12, 0x0A, 0x16, 0x12, 0x22, 0x77, 0x00, 0x00
}; /* k */
const unsigned char c_49[16] = {
0x00, 0x00, 0x00, 0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00
}; /* I */
const unsigned char c_74[16] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x30, 0x00, 0x00
}; /* t */

/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware( void );

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_0 one at a time */
    hal_uart_put_char(HAL_UART_2, ch);
    return ch;
}

/* UART hardware initialization for log output */
static void uart_log_init(void)
{
    hal_uart_config_t uart_config;

    /* COM port settings */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.parity = HAL_UART_PARITY_NONE;
    hal_uart_init(HAL_UART_2, &uart_config);
}

static void SystemClock_Config(void)
{
    hal_clock_init();
}

static void prvSetupHardware( void )
{
    /* System HW initialization */

    /* Peripherals initialization */
    uart_log_init();
    hal_flash_init();
    hal_nvic_init();

    /* Board HW initialization */
    bsp_ept_gpio_setting_init();
}

bool g2d_callback(void *data)
{
    printf("g2d_callback\r\n");

    return true;
}

int g2d_test(void)
{
    hal_g2d_handle_t *g2dHandle;
    static hal_display_lcd_roi_output_t lcd_parameters;
    static hal_display_lcd_layer_input_t lcm_parameters;
    int i, j, screen_width, screen_height;

    printf("Address:\r\n");
    printf("wallpaper = 0x%08X\r\n", (unsigned int)wallpaper);
    printf("brandmark = 0x%08X\r\n", (unsigned int)brandmark);
    printf("font_layer = 0x%08X\r\n", (unsigned int)font_layer);
    printf("frame_buffer = 0x%08X\r\n", (unsigned int)frame_buffer);

    for (i = 0, j = 0; i < 16; i++, j += 6) {
        font_layer[j] = c_4C[i];
        font_layer[j + 1] = c_69[i];
        font_layer[j + 2] = c_6E[i];
        font_layer[j + 3] = c_6B[i];
        font_layer[j + 4] = c_49[i];
        font_layer[j + 5] = c_74[i];
    }

    for (i = 0; i < 320 * 320 * 3; i += 3) {
        frame_buffer[i] = 0xFF;
        frame_buffer[i + 1] = 0x0;
        frame_buffer[i + 2] = 0x0;
    }

    bsp_lcd_init(0x0000);
    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, &screen_width);
    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, &screen_height);
    bsp_backlight_init();

    printf("LCD width = %d, LCD height = %d\r\n", screen_width, screen_height);

    lcd_parameters.target_start_x = 0;
    lcd_parameters.target_start_y = 0;    
    lcd_parameters.target_end_x = screen_width - 1;
    lcd_parameters.target_end_y = screen_height - 1;
    lcd_parameters.roi_offset_x = 0;
    lcd_parameters.roi_offset_y = 0;
    lcd_parameters.roi_background_color = 0;
    lcd_parameters.main_lcd_output = LCM_8BIT_24_BPP_RGB888_1;

    bsp_lcd_config_roi(&lcd_parameters);

    lcm_parameters.layer_enable = HAL_DISPLAY_LCD_LAYER0;
    lcm_parameters.alpha = 0;
    lcm_parameters.alpha_flag = 0;
    lcm_parameters.rotate = HAL_DISPLAY_LCD_LAYER_ROTATE_0;
    lcm_parameters.source_key_flag = 0;
    lcm_parameters.rgb_swap_flag = 1;
    lcm_parameters.byte_swap_flag = 0;
    lcm_parameters.dither_flag = 0;
    lcm_parameters.color_format = HAL_DISPLAY_LCD_LAYER_COLOR_RGB888;
    lcm_parameters.destination_key_flag = 0;
    lcm_parameters.color_key = 0;
    lcm_parameters.window_x_offset = 0;
    lcm_parameters.window_y_offset = 0;
    lcm_parameters.buffer_address = (uint32_t)&frame_buffer;
    lcm_parameters.row_size = screen_height;
    lcm_parameters.column_size = screen_width;
    lcm_parameters.pitch = screen_width * 3;

    bsp_lcd_config_layer(&lcm_parameters);

    printf("G2D test start.\r\n");

    if (hal_g2d_init() != HAL_G2D_STATUS_OK) {
        printf("hal_g2d_init failed\r\n");
        return 0;
    }

    if (hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_NON_BLOCKING) != HAL_G2D_STATUS_OK) {
       printf("hal_g2d_get_handle failed\r\n");
       return 0;
    }

    hal_g2d_register_callback(g2dHandle, g2d_callback);

    hal_g2d_overlay_set_alpha(g2dHandle, 0xFF);
    hal_g2d_set_rgb_buffer(g2dHandle, frame_buffer, 320*320*3, 320, 320, HAL_G2D_COLOR_FORMAT_RGB888);
    hal_g2d_overlay_set_roi_window(g2dHandle, 0, 0, 320, 320);
    hal_g2d_overlay_set_window(g2dHandle, 0, 0);

    hal_g2d_set_clipping_window(g2dHandle, 0, 0, 320, 320);
    hal_g2d_replace_color(g2dHandle, 0xFF, 0xD7, 0x1F, 0x85, 0xFF, 0x69, 0xBE, 0x28);
    hal_g2d_overlay_set_background_color(g2dHandle, 0x00A1DE);

    hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
    hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0xFF);
    hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, wallpaper, 320*320*3, 320, 320, HAL_G2D_COLOR_FORMAT_RGB888);
    hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0, 0, 320, 320);
    hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0x000000);
    hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_ROTATE_ANGLE_0);
    hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);

    hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER1, HAL_G2D_OVERLAY_LAYER_FUNCTION_RECTFILL);
    hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER1, 0, 240, 320, 80);
    hal_g2d_overlay_set_layer_rectfill_color(g2dHandle, HAL_G2D_OVERLAY_LAYER1, 0xFFD71F85);
    hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER1, HAL_G2D_ROTATE_ANGLE_0);
    hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER1, true);

    hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER2, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
    hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER2, brandmark, 240*120*3, 240, 120, HAL_G2D_COLOR_FORMAT_RGB888);
    hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER2, 40, 100, 240, 120);
    hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER2, 0xFFFFFF);
    hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER2, HAL_G2D_ROTATE_ANGLE_0);
    hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER2, true);

    hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER3, HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT);
    hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER3, font_layer, 48*16/8, 48, 16, HAL_G2D_COLOR_FORMAT_ARGB8888);
    hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER3, 0, 300, 48, 16);
    hal_g2d_overlay_set_layer_font_format(g2dHandle, HAL_G2D_OVERLAY_LAYER3, HAL_G2D_FONT_FORMAT_1_BIT);
    hal_g2d_overlay_set_layer_font_color(g2dHandle, HAL_G2D_OVERLAY_LAYER3, 0xFFF39A1E);
    hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER3, HAL_G2D_ROTATE_ANGLE_0);
    hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER3, true);

    hal_g2d_overlay_start(g2dHandle);

    while (hal_g2d_get_status(g2dHandle) == HAL_G2D_STATUS_BUSY)
        ;

    hal_g2d_release_handle(g2dHandle);

    printf("G2D test done.\r\n");

    bsp_lcd_update_screen(lcd_parameters.target_start_x , lcd_parameters.target_start_y ,lcd_parameters.target_end_x, lcd_parameters.target_end_y);

    return 0;
}

int main(void)
{
    /* Configure system clock */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware */
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
    printf("\r\n\r\n");/* The output UART used by printf is set by log_uart_init() */
    printf("welcome to main()\r\n");
    printf("pls add your own code from here\r\n");
    printf("\r\n\r\n");

    g2d_test();

    while (1);
}

