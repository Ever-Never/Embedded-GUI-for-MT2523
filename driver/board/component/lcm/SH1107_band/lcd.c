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

#include "mt25x3_hdk_lcd.h"
#include "hal_gpt.h"
#include "hal_g2d.h"
#include "hal_log.h"
#include <stdio.h>
#include "string.h"
#include "memory_attribute.h"
#ifdef __cplusplus
extern "C" {
#endif

#define HEIGHT 128
#define WIDTH 64

uint32_t ranesis_delay;

#define GDI_LCD_WIDTH (128)
#define GDI_LCD_HEIGHT (128)
#define D_LCD_Layer_Size    (GDI_LCD_WIDTH*GDI_LCD_HEIGHT)

uint8_t R_LCD_BWBuffer[D_LCD_Layer_Size/8];

uint8_t LCDSetArray[]  = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
uint8_t LCDReSetArray[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
uint32_t i;

void F_LCD_ColorTranslate(uint8_t * pBuf, uint16_t wX1, uint16_t wY1, uint16_t wX2, uint16_t wY2)
{  
    uint16_t wX, wY;
    uint16_t wGrayColor;
    uint32_t dwTemp;

    memset(R_LCD_BWBuffer, 0, sizeof(R_LCD_BWBuffer));
    for (wY = wY1; wY <= wY2; wY++) {
        for (wX = wX1; wX <= wX2; wX++) {
            i = (GDI_LCD_WIDTH/8-(wY/8)-1) + (GDI_LCD_WIDTH/8 * wX);
            wGrayColor = ((pBuf[i] >> (7-(wY%8)))&(0x1));
            dwTemp = wY / 8;
            dwTemp *= GDI_LCD_WIDTH;
            dwTemp += wX;

            if (wGrayColor == 1) {
                R_LCD_BWBuffer[dwTemp] |= LCDSetArray[wY % 8];
            } else {
                R_LCD_BWBuffer[dwTemp] &= LCDReSetArray[wY % 8];
            }
        }
    }
}

#ifdef __CONVERT_BW_ONE_LAYER__
#define GDI_LCD_WIDTH (128)
#define GDI_LCD_HEIGHT (128)
#define  D_LCD_Layer_Size    (GDI_LCD_WIDTH*GDI_LCD_HEIGHT)
#define LCD_DRV_RGB_TO_BW(color,bit)   ( (uint8_t)((((((color)&0xf800)>>11) + (((color)&0x07e0)>>5) + ((color)&0x001f) ) & 0x40) ? (1<<bit):0) )
#define LCD_DRV_HW_TO_RGB_R(VALUE)  ((uint8_t)(((VALUE)&0xf800)>>8))
#define LCD_DRV_HW_TO_RGB_G(VALUE)  ((uint8_t)(((VALUE)&0x07e0)>>3))
#define LCD_DRV_HW_TO_RGB_B(VALUE)   ((uint8_t)(((VALUE)&0x001f)<<3))
#define LCD_RGB_TO_HW_FORMAT(R,G,B)   (((uint16_t)(((int)(R)+(int)(G)+(int)(B))/(int)3)>0x80)?1:0)
#define LCD_SET_BUFFER_PIXEL(X,Y,VALUE)   \
          (VALUE<=0)?((*((uint8_t*)lcd_act_layer+(uint8_t)(LCD_WIDTH)*(uint8_t)(Y/8)+(uint8_t)(X%128)))&(~(0x1<<(uint8_t)(Y%8)))) \
           :((*((uint8_t*)lcd_act_layer+(uint8_t)(LCD_WIDTH)+(uint8_t)(Y/8)+(uint8_t)(X%128)))|(0x1<<(uint8_t)(Y%8)))

ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t R_LCD_BWBuffer[D_LCD_Layer_Size/8];
ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t LCD_BWBuffer[D_LCD_Layer_Size/8];
uint8_t LCDSetArray[]  = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
uint8_t LCDReSetArray[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t r_layer_buf[GDI_LCD_WIDTH * GDI_LCD_HEIGHT * 2] = {0};
#endif

#define MAIN_LCD_CMD_ADDR		LCD_SERIAL0_A0_LOW_ADDR
#define MAIN_LCD_DATA_ADDR		LCD_SERIAL0_A0_HIGH_ADDR
#define LCD_DataRead_SH1107    (*(volatile uint8_t *)MAIN_LCD_DATA_ADDR);

static lcm_config_para_t SH1107_para =
{
    .type = LCM_INTERFACE_TYPE_DBI,
    .backlight_type = BACKLIGHT_TYPE_LCM_BRIGHTNESS,
    .main_command_address = LCD_SERIAL0_A0_LOW_ADDR,
	.main_data_address = LCD_SERIAL0_A0_HIGH_ADDR,
	.main_lcd_output = LCM_16BIT_16_BPP_RGB565_1,
    .output_pixel_width = 16,
};

#define LCD_CtrlWrite_SH1107(_data) \
{\
	*((volatile uint16_t *)MAIN_LCD_CMD_ADDR) = ((_data)&(0x00FF));\
for(ranesis_delay = 20; ranesis_delay > 0; ranesis_delay--);\
}

#define LCD_DataWrite_SH1107(_data) \
{\
	*((volatile uint16_t *)MAIN_LCD_DATA_ADDR) = ((_data)&(0x00FF));\
for(ranesis_delay = 20; ranesis_delay > 0; ranesis_delay--);\
}

void Set_Column(uint8_t column)
{
    LCD_CtrlWrite_SH1107((0x00) | ((column) & (0x0F)));
    LCD_CtrlWrite_SH1107((0x10) | (column >> 4));
}


void Set_Start_Column(void)
{
    LCD_CtrlWrite_SH1107(0x00);
    LCD_CtrlWrite_SH1107(0x10);
}

void Set_Start_Page(unsigned char d)
{
    LCD_CtrlWrite_SH1107((0xB0) | (d));			// Set Page Start Address for Page Addressing Mode
}

void Set_Display_On_Off(unsigned char d)
{
    LCD_CtrlWrite_SH1107((0xAE) | (d));			// Set Display On/Off
}

void Set_Display_Offset(unsigned char d)
{
    LCD_CtrlWrite_SH1107(0xD3);			// Set Display Offset
    LCD_CtrlWrite_SH1107(d);			//   Default => 0x00
}

void LCD_Init_Interface_SH1107(void)
{
    hal_display_lcd_interface_mode_t mode_settings;
    hal_display_lcd_interface_timing_t timing_settings;

    mode_settings.port_number = HAL_DISPLAY_LCD_INTERFACE_SERIAL_0;
    mode_settings.three_wire_mode = 0;
    mode_settings.cs_stay_low_mode = 0;
    mode_settings.driving_current = HAL_DISPLAY_LCD_DRIVING_CURRENT_16MA;
    mode_settings.single_a0_mode = 0;
    mode_settings.read_from_SDI = 0;
    mode_settings.width = HAL_DISPLAY_LCD_INTERFACE_WIDTH_8;
    mode_settings.hw_cs = 1;
    mode_settings.power_domain = HAL_DISPLAY_LCD_POWER_DOMAIN_1V8;
    mode_settings.start_byte_mode = 0;

    hal_display_lcd_set_interface_mode(mode_settings);

    timing_settings.port_number = HAL_DISPLAY_LCD_INTERFACE_SERIAL_0;
    timing_settings.csh = 0;
    timing_settings.css = 0;
    timing_settings.wr_low = 0xF;
    timing_settings.wr_high = 0xF;
    timing_settings.rd_low = 0xF;
    timing_settings.rd_high = 0xF;
    timing_settings.clock_freq = HAL_DISPLAY_LCD_INTERFACE_CLOCK_124MHZ;

    hal_display_lcd_set_interface_timing(timing_settings);
    // Do error check
} /* init_lcd_interface() */

/***********************SH1107********************************/

void LCD_blockClear_SH1107(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t data)
{
    unsigned char x, y, send_data;

    if (data == 0) {
        send_data = 0;
    } else {
        send_data = 0xFF;
    }

    for (x = 0; x < 16; x++) {
        Set_Start_Page(x);
        Set_Start_Column();
        for (y = 0; y < 128; y++) {
            LCD_DataWrite_SH1107(send_data);
        }
    }
    hal_gpt_delay_ms(100);
}

void LCD_ClearAll_SH1107(uint16_t data)
{
    LCD_blockClear_SH1107(0, 0, HEIGHT - 1, WIDTH - 1, data);
}

void LCD_Init_SH1107(uint16_t bkground)
{
    hal_display_lcd_toggle_reset(10, 120);

    LCD_CtrlWrite_SH1107(0xAE);
    LCD_CtrlWrite_SH1107(0xA8);
    LCD_CtrlWrite_SH1107(0x3F);
    LCD_CtrlWrite_SH1107(0xD5);
    LCD_CtrlWrite_SH1107(0x51);

#ifndef LCD_MIRROR_DISPLAY
    LCD_CtrlWrite_SH1107(0xc8); //0xc0,0xc8// Set Common scan direction
#else
    LCD_CtrlWrite_SH1107(0xc0); //0xc0,0xc8// Set Common scan direction
#endif
    LCD_CtrlWrite_SH1107(0xd3); // Set Display Offset
#ifndef LCD_MIRROR_DISPLAY
    LCD_CtrlWrite_SH1107(0x20);//0x60
#else
    LCD_CtrlWrite_SH1107(0x60);//0x60
#endif
    LCD_CtrlWrite_SH1107(0xdc); // Set Display Start Line
    LCD_CtrlWrite_SH1107(0x00);
    LCD_CtrlWrite_SH1107(0x20); // Set Page Addressing Mode
    LCD_CtrlWrite_SH1107(0x81); // Set Contrast Control
    LCD_CtrlWrite_SH1107(0x90);
#ifndef LCD_MIRROR_DISPLAY
    LCD_CtrlWrite_SH1107(0xa1); //0xa0,0xa1// Set Segment Re-mapd
#else
    LCD_CtrlWrite_SH1107(0xa0); //0xa0,0xa1// Set Segment Re-mapd
#endif
    LCD_CtrlWrite_SH1107(0xa4); // Set Entire Display OFF/ON
    LCD_CtrlWrite_SH1107(0xa6); // Set Normal/Reverse Display
    LCD_CtrlWrite_SH1107(0xad); // Set External VPP
    LCD_CtrlWrite_SH1107(0x8a);
    LCD_CtrlWrite_SH1107(0xd9); // Set Phase Leghth
    LCD_CtrlWrite_SH1107(0x22);
    LCD_CtrlWrite_SH1107(0xdb); // Set Vcomh voltage
    LCD_CtrlWrite_SH1107(0x35);
    LCD_ClearAll_SH1107(0);
    LCD_CtrlWrite_SH1107(0xaf); //Display ON

}

void LCD_EnterSleep_SH1107(void)
{
    Set_Display_On_Off(0x00);
}

void LCD_ExitSleep_SH1107(void)
{
    LCD_Init_SH1107(0);
}


void LCD_EnterIdle_SH1107(void)
{
    LCD_CtrlWrite_SH1107(0x39);
}

void LCD_ExitIdle_SH1107(void)
{
    LCD_CtrlWrite_SH1107(0x38);
}

#ifdef __CONVERT_BW_ONE_LAYER__
static void convert_layer(uint8_t *buffer)
{
    hal_g2d_handle_t *g2dHandle;

    hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_BLOCKING);
    hal_g2d_overlay_set_window(g2dHandle, 0, 0);
    hal_g2d_set_rgb_buffer(g2dHandle, r_layer_buf, GDI_LCD_WIDTH*GDI_LCD_HEIGHT*2, GDI_LCD_WIDTH, 
                                     GDI_LCD_HEIGHT,HAL_G2D_COLOR_FORMAT_RGB565);
    hal_g2d_overlay_set_background_color(g2dHandle, 0);
    hal_g2d_overlay_set_roi_window(g2dHandle, 0, 0, GDI_LCD_WIDTH, GDI_LCD_HEIGHT);
    hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
    hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0, 0, GDI_LCD_WIDTH, GDI_LCD_HEIGHT);
    hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0);
    hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
    hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_ROTATE_ANGLE_90);
    hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, buffer, GDI_LCD_WIDTH * GDI_LCD_HEIGHT * 2, GDI_LCD_WIDTH, GDI_LCD_HEIGHT, HAL_G2D_COLOR_FORMAT_RGB565);
    hal_g2d_overlay_start(g2dHandle);

    hal_g2d_release_handle(g2dHandle);
}

void F_LCD_ColorTranslate(uint16_t * pBuf, uint16_t wX1, uint16_t wY1, uint16_t wX2, uint16_t wY2)
{  
    uint16_t wR, wG, wB, wX, wY;
    uint16_t wGrayColor;
    uint32_t dwTemp;
    uint32_t i;

    memset(R_LCD_BWBuffer, 0, sizeof(R_LCD_BWBuffer));
    for (wY = wY1; wY <= wY2; wY++) {
        for (wX = wX1; wX <= wX2; wX++) {
            i = wY * GDI_LCD_WIDTH + wX;

            wR = LCD_DRV_HW_TO_RGB_R(pBuf[i]);
            wG = LCD_DRV_HW_TO_RGB_G(pBuf[i]);
            wB = LCD_DRV_HW_TO_RGB_B(pBuf[i]);
 
            wGrayColor = (wR + wG  + wB) / 3;
            dwTemp = wY / 8;
            dwTemp *= GDI_LCD_WIDTH;
            dwTemp += wX;
	
            if (wGrayColor > 0x80) {
                R_LCD_BWBuffer[dwTemp] |= LCDSetArray[wY % 8];
            } else {
                R_LCD_BWBuffer[dwTemp] &= LCDReSetArray[wY % 8];
            }
        }
    }
}
#endif
//static int b =0;
extern uint8_t lcd_get_page_count(uint8_t charHeight, uint8_t y0_Coordinate);
void LCD_BlockWrite_SH1107(uint16_t startx, uint16_t starty, uint16_t endx, uint16_t endy)
{
    unsigned char x, y;
    uint32_t cnt = 0;
    uint8_t *buffer_addr;

#if 0
    uint16_t        i, j, k;
    uint16_t        start_page_num, page_num;
    uint16_t        start_bit_num, stop_bit_num;
    uint16_t        height, width;
    uint8_t         old_data8;
    uint8_t        *p_frame;

    buffer_addr = (uint8_t *)hal_display_lcd_get_layer_address(HAL_DISPLAY_LCD_LAYER0);
    height = endy - starty + 1;
    width  = endx - startx + 1;
    page_num = lcd_get_page_count((uint8_t) height, (uint8_t)starty);
    start_page_num = starty >> 3;
    start_bit_num  = starty & 0x07;
    stop_bit_num   = (height + start_bit_num) & 0x07;
    p_frame = buffer_addr + start_page_num * WIDTH;
    startx = WIDTH - startx - 1;
    endx = WIDTH - endx - 1;
    for (j = start_page_num; j < start_page_num + page_num; j++) {
        Set_Start_Page(j);
        Set_Column(endx);
        k = WIDTH - endx - 1;
        for (i = endx; i <= startx; i++) {
            LCD_DataWrite_SH1107(*(p_frame + k));
            k--;
        }
        p_frame += WIDTH;
    }
#else
	buffer_addr = (uint8_t *)hal_display_lcd_get_layer_address(HAL_DISPLAY_LCD_LAYER0);
    if(buffer_addr == NULL) {
        return; }
    F_LCD_ColorTranslate(buffer_addr, 0, 0, GDI_LCD_WIDTH-1, GDI_LCD_HEIGHT-1);
    
#ifdef __CONVERT_BW_ONE_LAYER__
    convert_layer(buffer_addr);
    F_LCD_ColorTranslate((uint16_t *)r_layer_buf, 0, 0, GDI_LCD_WIDTH-1, GDI_LCD_HEIGHT-1);
#endif
	for (x = 0; x < 16; x++) {
        Set_Start_Page(x);
        Set_Start_Column();
        for (y = 0; y < 128; y++) {
        #ifdef __CONVERT_BW_ONE_LAYER__
            LCD_DataWrite_SH1107(R_LCD_BWBuffer[cnt++]);
        #else
            LCD_DataWrite_SH1107(R_LCD_BWBuffer[cnt++]);
        #endif
        }
    }
#endif
    hal_gpt_delay_ms(1);
}


/***************************** Take Notice *******************************
*************************************************************************/

void LCD_IOCTRL_SH1107(LCM_IOCTRL_ID_ENUM ID, void *Parameters)
{
    switch (ID) {
        case LCM_IOCTRL_QUERY__FRAME_RATE:
            *((uint32_t *)(Parameters)) = 60;
            break;

        case LCM_IOCTRL_QUERY__BACK_PORCH:
            *(uint32_t *)(Parameters) = 0;
            break;

        case LCM_IOCTRL_QUERY__FRONT_PORCH:
            *(uint32_t *)(Parameters) = 0;
            break;

        case LCM_IOCTRL_QUERY__IF_CS_NUMBER:
            *(uint32_t *)(Parameters) = HAL_DISPLAY_LCD_INTERFACE_SERIAL_0;
            break;

        case LCM_IOCTRL_QUERY__LCM_WIDTH:
            *((unsigned long *)(Parameters)) = 128; // physical, not use app's LCD_WIDTH
            break;

        case LCM_IOCTRL_QUERY__LCM_HEIGHT:
            *((unsigned long *)(Parameters)) = 64; //320;// physical, not use app's LCD_HEIGHT
            break;

        case LCM_IOCTRL_QUERY__LCM_SETTING_PARAM:
            *((lcm_config_para_t *)(Parameters)) = SH1107_para; // return LCM_para
            break;

#if 0
            //#ifdef __LCD_ESD_RECOVERY__
            /** LCM ESD feature control, Check LCD ESD is happend or not **/
        case LCM_IOCTRL_ESD_CHECK: {
            LCM_ESD_INFO_STRUCT *pESDInfo = (LCM_ESD_INFO_STRUCT *)Parameters;
            kal_bool lcm_esd_test = KAL_FALSE;

            lcm_esd_test = LCD_Read_ESD_SH1107();

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
            LCD_Init_SH1107(0, 0);

            break;
#endif

        default:
            break;
    }
}

void LCD_ClearScreen_SH1107(uint16_t color)
{
    LCD_ClearAll_SH1107(color);
}

void LCD_ClearScreenBW_SH1107(void)
{
    unsigned char x, y, send_data;

    for (x = 0; x < 16; x++) {
        if (x < 8) {
            send_data = 0xFF;
        } else {
            send_data = 0;
        }
        Set_Start_Page(x);
        Set_Start_Column();
        for (y = 0; y < 128; y++) {
            LCD_DataWrite_SH1107(send_data);
        }
    }
    hal_gpt_delay_ms(100);
}

bool LCD_CheckID_SH1107(void)
{
    return true;
}

LCD_Funcs LCD_func_SH1107 = {
    LCD_Init_SH1107,
    LCD_Init_Interface_SH1107,
    LCD_BlockWrite_SH1107,
    LCD_EnterSleep_SH1107,
    LCD_ExitSleep_SH1107,
    LCD_EnterIdle_SH1107,
    LCD_ExitIdle_SH1107,
    LCD_ClearScreen_SH1107,
    LCD_ClearScreenBW_SH1107,
    LCD_IOCTRL_SH1107,
    LCD_CheckID_SH1107
};

#ifdef __cplusplus
}
#endif

