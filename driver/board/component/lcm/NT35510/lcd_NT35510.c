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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hal_platform.h"

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED

#include "mt25x3_hdk_lcd.h"
#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
#include "hal_display_dsi_internal.h"
#endif
#include "hal_gpt.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
#define HEIGHT 240
#define WIDTH 240

extern volatile LCD_REGISTER_T *lcd_register_ptr;
extern volatile LCD_DSI_REGISTER_T *lcd_dsi_register_ptr;

static lcm_config_para_t NT35510_para = {
    .type = LCM_INTERFACE_TYPE_DSI,
    .backlight_type = BACKLIGHT_TYPE_LCM_BRIGHTNESS,
    .main_command_address = LCD_SERIAL0_A0_LOW_ADDR,
    .main_data_address = LCD_SERIAL0_A0_HIGH_ADDR,
    .main_lcd_output = LCM_8BIT_24_BPP_RGB888_1,
    .output_pixel_width = 24,
#ifdef MTK_TE_ENABLE
    .te_enable = true,
#endif
};

void LCD_Init_Interface_NT35510(void)
{
    hal_display_dsi_dphy_timing_struct_t	timing;

    timing.da_hs_trail = 0x04;
    timing.da_hs_zero = 0x07;
    timing.da_hs_prep = 0x02;
    timing.lpx = 0x03;
    timing.da_hs_exit = 0x04;
    timing.ta_get = 0xF;
    timing.ta_sure = 0x02;
    timing.ta_go = 0x0C;
    timing.clk_hs_trail = 0x03;
    timing.clk_hs_zero = 0x0C;
    timing.clk_hs_post = 0x09;
    timing.clk_hs_prep = 0x01;

    hal_display_dsi_set_dphy_timing(&timing);
} /* init_lcd_interface() */

/***********************NT35510********************************/

void LCD_Init_NT35510(uint16_t bkground)
{
    uint32_t data_array[16];

    // Do HW Reset
    hal_display_lcd_toggle_reset(10, 120);

    data_array[0] = 0x00053902;
    data_array[1] = 0x2555AAFF;
    data_array[2] = 0x00000001;
    hal_display_dsi_set_command_queue(data_array, 3, true);

    data_array[0] = 0x00093902;
    data_array[1] = 0x000201F8;
    data_array[2] = 0x00133320;
    data_array[3] = 0x00000048;
    hal_display_dsi_set_command_queue(data_array, 4, true);

    data_array[0] = 0x00063902;
    data_array[1] = 0x52AA55F0;
    data_array[2] = 0x00000108;
    hal_display_dsi_set_command_queue(data_array, 3, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x0D0D0DB0;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x343434B6;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x0D0D0DB1;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x343434B7;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x000000B2;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x242424B8;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00023902;
    data_array[1] = 0x000001BF;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x0F0F0FB3;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x343434B9;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x080808B5;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00023902;
    data_array[1] = 0x000003C2;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x242424BA;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x007800BC;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x007800BD;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00033902;
    data_array[1] = 0x006400BE;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00353902;
    data_array[1] = 0x003300D1;
    data_array[2] = 0x003A0034;
    data_array[3] = 0x005C004A;
    data_array[4] = 0x00A60081;
    data_array[5] = 0x011301E5;
    data_array[6] = 0x01820154;
    data_array[7] = 0x020002CA;
    data_array[8] = 0x02340201;
    data_array[9] = 0x02840267;
    data_array[10] = 0x02B702A4;
    data_array[11] = 0x02DE02CF;
    data_array[12] = 0x03FE02F2;
    data_array[13] = 0x03330310;
    data_array[14] = 0x0000006D;
    hal_display_dsi_set_command_queue(data_array, 15, true);

    data_array[0] = 0x00353902;
    data_array[1] = 0x003300D2;
    data_array[2] = 0x003A0034;
    data_array[3] = 0x005C004A;
    data_array[4] = 0x00A60081;
    data_array[5] = 0x011301E5;
    data_array[6] = 0x01820154;
    data_array[7] = 0x020002CA;
    data_array[8] = 0x02340201;
    data_array[9] = 0x02840267;
    data_array[10] = 0x02B702A4;
    data_array[11] = 0x02DE02CF;
    data_array[12] = 0x03FE02F2;
    data_array[13] = 0x03330310;
    data_array[14] = 0x0000006D;
    hal_display_dsi_set_command_queue(data_array, 15, true);

    data_array[0] = 0x00353902;
    data_array[1] = 0x003300D3;
    data_array[2] = 0x003A0034;
    data_array[3] = 0x005C004A;
    data_array[4] = 0x00A60081;
    data_array[5] = 0x011301E5;
    data_array[6] = 0x01820154;
    data_array[7] = 0x020002CA;
    data_array[8] = 0x02340201;
    data_array[9] = 0x02840267;
    data_array[10] = 0x02B702A4;
    data_array[11] = 0x02DE02CF;
    data_array[12] = 0x03FE02F2;
    data_array[13] = 0x03330310;
    data_array[14] = 0x0000006D;
    hal_display_dsi_set_command_queue(data_array, 15, true);

    data_array[0] = 0x00353902;
    data_array[1] = 0x003300D4;
    data_array[2] = 0x003A0034;
    data_array[3] = 0x005C004A;
    data_array[4] = 0x00A60081;
    data_array[5] = 0x011301E5;
    data_array[6] = 0x01820154;
    data_array[7] = 0x020002CA;
    data_array[8] = 0x02340201;
    data_array[9] = 0x02840267;
    data_array[10] = 0x02B702A4;
    data_array[11] = 0x02DE02CF;
    data_array[12] = 0x03FE02F2;
    data_array[13] = 0x03330310;
    data_array[14] = 0x0000006D;
    hal_display_dsi_set_command_queue(data_array, 15, true);

    data_array[0] = 0x00353902;
    data_array[1] = 0x003300D5;
    data_array[2] = 0x003A0034;
    data_array[3] = 0x005C004A;
    data_array[4] = 0x00A60081;
    data_array[5] = 0x011301E5;
    data_array[6] = 0x01820154;
    data_array[7] = 0x020002CA;
    data_array[8] = 0x02340201;
    data_array[9] = 0x02840267;
    data_array[10] = 0x02B702A4;
    data_array[11] = 0x02DE02CF;
    data_array[12] = 0x03FE02F2;
    data_array[13] = 0x03330310;
    data_array[14] = 0x0000006D;
    hal_display_dsi_set_command_queue(data_array, 15, true);

    data_array[0] = 0x00353902;
    data_array[1] = 0x003300D6;
    data_array[2] = 0x003A0034;
    data_array[3] = 0x005C004A;
    data_array[4] = 0x00A60081;
    data_array[5] = 0x011301E5;
    data_array[6] = 0x01820154;
    data_array[7] = 0x020002CA;
    data_array[8] = 0x02340201;
    data_array[9] = 0x02840267;
    data_array[10] = 0x02B702A4;
    data_array[11] = 0x02DE02CF;
    data_array[12] = 0x03FE02F2;
    data_array[13] = 0x03330310;
    data_array[14] = 0x0000006D;
    hal_display_dsi_set_command_queue(data_array, 15, true);
    hal_gpt_delay_ms(10);

    data_array[0] = 0x00063902;
    data_array[1] = 0x52AA55F0;
    data_array[2] = 0x00000008;
    hal_display_dsi_set_command_queue(data_array, 3, true);

    data_array[0] = 0x00033902;
    data_array[1] = 0x00007CB1;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00023902;
    data_array[1] = 0x000005B6;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00033902;
    data_array[1] = 0x007070B7;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00053902;
    data_array[1] = 0x030301B8;
    data_array[2] = 0x00000003;
    hal_display_dsi_set_command_queue(data_array, 3, true);

    data_array[0] = 0x00043902;
    data_array[1] = 0x000002BC;
    hal_display_dsi_set_command_queue(data_array, 2, true);

    data_array[0] = 0x00063902;
    data_array[1] = 0x5002D0C9;
    data_array[2] = 0x00005050;
    hal_display_dsi_set_command_queue(data_array, 3, true);

    data_array[0] = 0x00351500;
    hal_display_dsi_set_command_queue(data_array, 1, true);

    data_array[0] = 0x773A1500;
    hal_display_dsi_set_command_queue(data_array, 1, true);

    data_array[0] = 0x00110500;
    hal_display_dsi_set_command_queue(data_array, 1, true);
    hal_gpt_delay_ms(120);

    data_array[0] = 0x00290500;
    hal_display_dsi_set_command_queue(data_array, 1, true);
    hal_gpt_delay_ms(20);
}

void LCD_EnterSleep_NT35510(void)
{
    uint32_t data_array[16];

    data_array[0] = 0x00100500;
    hal_display_dsi_set_command_queue(data_array, 1, true);
    hal_gpt_delay_ms(120);

    data_array[0] = 0x00280500;
    hal_display_dsi_set_command_queue(data_array, 1, true);
    hal_gpt_delay_ms(10);

    data_array[0] = 0x014F1500;
    hal_display_dsi_set_command_queue(data_array, 1, true);
    hal_gpt_delay_ms(40);
}

void LCD_ExitSleep_NT35510(void)
{
    LCD_Init_NT35510(0);
}

void LCD_blockClear_NT35510(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t data)
{
}

void LCD_ClearAll_NT35510(uint16_t data)
{
    LCD_blockClear_NT35510(0, 0, HEIGHT - 1, WIDTH - 1, data);
}

void LCD_EnterIdle_NT35510(void)
{
}

void LCD_ExitIdle_NT35510(void)
{
}

void LCD_BlockWrite_NT35510(uint16_t startx, uint16_t starty, uint16_t endx, uint16_t endy)
{
    uint32_t data_array[16];

    unsigned char x0_MSB = (startx >> 8) & 0xFF;
    unsigned char x0_LSB = startx & 0xFF;
    unsigned char x1_MSB = (endx >> 8) & 0xFF;
    unsigned char x1_LSB = endx & 0xFF;
    unsigned char y0_MSB = (starty >> 8) & 0xFF;
    unsigned char y0_LSB = starty & 0xFF;
    unsigned char y1_MSB = (endy >> 8) & 0xFF;
    unsigned char y1_LSB = endy & 0xFF;

    data_array[0] = 0x00053902;
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2A;
    data_array[2] = x1_LSB;
    hal_display_dsi_set_command_queue(data_array, 3, true);

    data_array[0] = 0x00053902;
    data_array[1] = (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2B;
    data_array[2] = y1_LSB;
    hal_display_dsi_set_command_queue(data_array, 3, true);

    hal_display_dsi_set_transfer_mode(HAL_DISPLAY_DSI_TRANSFER_MODE_HS);

    hal_display_lcd_start_dma(0);

#ifdef MTK_TE_ENABLE
    if(NT35510_para.te_enable) {

        hal_display_dsi_start_bta_transfer(0x2C3909);
    }
    else
#endif
    {
        data_array[0] = 0x2C3909;
        hal_display_dsi_set_command_queue(data_array, 1, 0);
    }
}


/***************************** Take Notice *******************************
*************************************************************************/

void LCD_IOCTRL_NT35510(LCM_IOCTRL_ID_ENUM ID, void *Parameters)
{
    switch (ID) {
        case LCM_IOCTRL_QUERY__FRAME_RATE:
            *((uint32_t *)(Parameters)) = 60;
            break;

        case LCM_IOCTRL_QUERY__BACK_PORCH:
            *(uint32_t *)(Parameters) = 12;
            break;

        case LCM_IOCTRL_QUERY__FRONT_PORCH:
            *(uint32_t *)(Parameters) = 12;
            break;

        case LCM_IOCTRL_QUERY__IF_CS_NUMBER:
            *(uint32_t *)(Parameters) = HAL_DISPLAY_LCD_INTERFACE_SERIAL_0;
            break;

        case LCM_IOCTRL_QUERY__LCM_WIDTH:
            *((unsigned long *)(Parameters)) = 240; // physical, not use app's LCD_WIDTH
            break;

        case LCM_IOCTRL_QUERY__LCM_HEIGHT:
            *((unsigned long *)(Parameters)) = 240; //320;// physical, not use app's LCD_HEIGHT
            break;

        case LCM_IOCTRL_QUERY__LCM_SETTING_PARAM:
            *((lcm_config_para_t *)(Parameters)) = NT35510_para; // return LCM_para
            break;

#if 0
        //#ifdef __LCD_ESD_RECOVERY__
        /** LCM ESD feature control, Check LCD ESD is happend or not **/
        case LCM_IOCTRL_ESD_CHECK: {
            LCM_ESD_INFO_STRUCT *pESDInfo = (LCM_ESD_INFO_STRUCT *)Parameters;
            kal_bool lcm_esd_test = KAL_FALSE;

            lcm_esd_test = LCD_Read_ESD_NT35510();

            if (pESDInfo) {
                //TODO: add ESD real check function here
                // If happend, must take lcm_esd_test = KAL_TURE;
                if (lcm_esd_test) {
                    lcm_esd_test = KAL_FALSE;
                    pESDInfo->bEsdHappened = KAL_TRUE;
                } else {
                    pESDInfo->bEsdHappened = KAL_FALSE;
                }
            }

            return LCM_IOCTRL_OK;
            break;
        }

        /** LCM ESD feature control, If LCD ESD is happend, We should do something here like resetLCM. **/
        case LCM_IOCTRL_ESD_RESET:
            //TODO: add LCM reset function here
            //Example
            LCD_Init_NT35510(0, 0);

            break;
#endif
#ifdef MTK_TE_ENABLE
        case LCM_IOCTRL_SET_TE:
            NT35510_para.te_enable = *((bool *)(Parameters));
            break;
#endif

        default:
            break;
    }
}

void LCD_ClearScreen_NT35510(uint16_t color)
{
    LCD_ClearAll_NT35510(color);
}

void LCD_ClearScreenBW_NT35510(void)
{
}
#endif

LCD_Funcs LCD_func_NT35510 = {
    LCD_Init_NT35510,
    LCD_Init_Interface_NT35510,
    LCD_BlockWrite_NT35510,
    LCD_EnterSleep_NT35510,
    LCD_ExitSleep_NT35510,
    LCD_EnterIdle_NT35510,
    LCD_ExitIdle_NT35510,
    LCD_ClearScreen_NT35510,
    LCD_ClearScreenBW_NT35510,
    LCD_IOCTRL_NT35510
};

#ifdef __cplusplus
}
#endif

#endif
