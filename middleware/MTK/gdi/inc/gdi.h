/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifndef _GDI_INCLUDE_H_
#define _GDI_INCLUDE_H_

/**
 * @addtogroup GDI
 * @{
 * This module provides the graphic API to display strings, images and basic shapes.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b GDI                        | Graphics Device Interface. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Graphics_Device_Interface">Wikipedia</a>.|
 *
 * @section gdi_module_usage How to use this module
 * - Step 1: Create a UI project.\n
 *  Create a UI project according to the LinkIt for RTOS get started guide.\n
 *  Example:\n
 *  Copy "project\mt2523_hdk\templates\freertos_initialize_main_features" to "project\mt2523_hdk\apps".\n 
 *  Change the folder name to "basic_ui".
 * - Step 2: Prepare the UI resource.\n
 *  For more information about the resource files, please refer to "\tools\PC_tool_Win1.zip\Resgen_tool\readme.txt".\n
 *  Note: Launch the resource tool in a Windows environment. Use the resource tool to create the resource files 
 *        and copy all resource files (the output files of the resource tool) to the project folder on Linux.\n
 *  Example:
 *  - Configure the font resource.\n
 *    Enable English language (#define __MMI_LANG_ENGLISH__) in the file ".\Resgen_tool\custom_resource\font\language_config.txt".\n
 *    Add an English font file (MTK_English_22.bdf) and the MediaTek proprietary font file (MTK_Proprietary_14.bdf) 
 *    in the file ".\Resgen_tool\custom_resource\font\res_gen_font.cpp".\n
 *    @code
 *       int main(int argc, char* argv[])
 *       {
 *           InitialFontEngine(FONT_DATA_OUTPUT_PATH);
 *
 *           // Add an English font file.
 *           AddFont(
 *               ("English"), ("*#0044#"), ("en-US"),
 *               FONT_DATA_FILE_PATH(MTK_English_22.bdf),
 *               MCT_SMALL_FONT | MCT_MEDIUM_FONT | MCT_LARGE_FONT,
 *               0, 0);
 *
 *           // Add MediaTek proprietary font file.
 *           AddFont(
 *               "gMTKProprietaryFont", NULL, NULL,
 *               FONT_DATA_FILE_PATH(MTK_Proprietary_14.bdf),
 *               MCT_SMALL_FONT | MCT_MEDIUM_FONT | MCT_LARGE_FONT,
 *               0, 0);
 *
 *           GenerateFontResFile();
 *           DeinitialFontEngine();
 *           return 0;
 *       }
 *     @endcode
 *  - Configure the image resource.\n
 *    Put all image files (if any) in the folder ".\Resgen_tool\custom_resource\images".\n
 *  - Compile the resource files.\n
 *    Run the following command in resource gen folder.\n
 *     >make resgen
 *  - Copy the output resource files.\n
 *    Copy all output resource files to the project folder on Linux.
 *  <pre>
 *    basic_ui\\inc:
 *        custom_image_data_resource.h
 *        custom_resource_def.h
 *        L_English_small.h
 *        L_gMTKProprietaryFont_small.h
 *    basic_ui\\src:
 *        custom_image_data_resource.c
 *        custom_image_map.c
 *        custom_image_resource.c
 *        FontRes.c
 *  </pre>
 *  -  Add resource files in the Makefile.\n
 *     @code
 *       # Resource files
 *       APP_FILES = $(APP_PATH_SRC)/custom_image_data_resource.c \
 *                   $(APP_PATH_SRC)/custom_image_map.c \
 *                   $(APP_PATH_SRC)/custom_image_resource.c\
 *                   $(APP_PATH_SRC)/FontRes.c
 *     @endcode
 * - Step 3: Add the UI module.\n
 *  -  Add GDI module Makefile.\n
 *  @code
 *    #gdi files
 *    -include $(SOURCE_DIR)/middleware/MTK/gdi/module.mk
 *  @endcode
 *  -  Add the UI screen file.\n
 *  @code
 *    # Basic UI files
 *    APP_FILES += $(APP_PATH_SRC)/main_screen.c
 *    APP_FILES += $(APP_PATH_SRC)/ui_demo_task.c
 *  @endcode
 *  - Check the other modules.\n
 *  @code
 *    # Keypad configuration, if the keypad is required.
 *    -include $(SOURCE_DIR)/driver/board/mt25x3_hdk/keypad/module.mk
 *    # Touch panel configuration, if the touch panel is required.
 *    -include $(SOURCE_DIR)/driver/board/mt25x3_hdk/touch_panel/module.mk
 *    #Backlight
 *    CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/backlight
 *  @endcode
 * - Step 4: Create the UI task.\n
 *  -
 *  @code
 *    int main(void)
 *    {
 *        system_init();
 *        demo_app_start();// Create a UI task in your main function.
 *        vTaskStartScheduler();
 *        for (;;);
 *    }
 *
 *    // Initialize the UI task.
 *    void ui_task_main(void*arg)
 *    {
 *        ui_task_message_struct_t queue_item;
 *        ui_task_cntx.event_queue = xQueueCreate(UI_TASK_QUEUE_SIZE , sizeof( ui_task_message_struct_t ));
 *
 *        main_screen_init(); // Initialize the main screen.
 *        main_screen_show(); // Display the main screen.
 *
 *        while (1) {
 *            if (xQueueReceive(ui_task_cntx.event_queue, &queue_item, portMAX_DELAY)) {
 *                ui_task_msg_handler(&queue_item);
 *            }
 *        }
 *    }
 *
 *    // Create a UI task.
 *    TaskHandle_t demo_app_start()
 *    {
 *        TaskHandle_t task_handler;
 *        xTaskCreate((TaskFunction_t) ui_task_main,
 *                     UI_TASK_NAME,
 *                     UI_TASK_STACK_SIZE/(( uint32_t )sizeof( StackType_t )),
 *                     NULL,
 *                     UI_TASK_PRIORITY,
 *                     &task_handler );
 *        return task_handler;
 *    }
 *  @endcode
 * - Step 5: Display the main screen.\n
 *  -
 *  @code
 *    #include "gdi.h"
 *    #include "gdi_font_engine.h"
 *    #include "custom_resource_def.h"
 *    #include "main_screen.h"
 *    #include "memory_attribute.h"
 *    #include "mt25x3_hdk_lcd.h"
 *    #include "bsp_lcd.h"
 *    #include "mt25x3_hdk_backlight.h"
 *
 *    #define DEFAULT_LCD_W 320
 *    #define DEFAULT_LCD_H 320
 *    static uint32_t LCD_WIDTH = 320;
 *    static uint32_t LCD_HEIGHT = 320;
 *    ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char layer_buffer[DEFAULT_LCD_W*DEFAULT_LCD_H*2];
 *
 *    void main_screen_init(){
 *        bsp_lcd_init(0xF800);
 *        bsp_backlight_init();
 *        bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, &LCD_WIDTH); // Get the LCD size.
 *        bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, &LCD_HEIGHT);
 *        gdi_init(LCD_WIDTH, LCD_HEIGHT, GDI_COLOR_FORMAT_16, layer_buffer); // Initialize the GDI buffer.
 *    }
 *
 *    uint8_t* convert_string_to_wstring(uint8_t* string)
 *    {
 *        static uint8_t wstring[50];
 *        int32_t index = 0;
 *        if (!string) {
 *            return NULL;
 *        }
 *        while (*string) {
 *            wstring[index] = *string;
 *            wstring[index + 1] = 0;
 *            string++;
 *            index+=2;
 *        }
 *        return wstring;
 *    }
 *
 *    void main_screen_show (){
 *        gdi_font_engine_display_string_info_t param;
 *
 *        // Set the font size and color.
 *        gdi_font_engine_color_t font_color = {0, 255, 255, 255};
 *        gdi_font_engine_set_font_size(GDI_FONT_ENGINE_FONT_MEDIUM);
 *        gdi_font_engine_set_text_color(font_color);
 *
 *        // Set the screen background color.
 *        gdi_draw_filled_rectangle(0,0,LCD_WIDTH,LCD_HEIGHT,gdi_get_color_from_argb(0, 0, 0, 0));
 *
 *        // Draw an image (source ID IMAGE_ID_BT_DISCONNECT_BMP) at position of (20, 140).
 *        gdi_image_draw_by_id(20,140,IMAGE_ID_BT_DISCONNECT_BMP);
 *
 *        // Draw the string "Hello World!" at position of (100, 150).
 *        param.x = 100;
 *        param.y = 150;
 *        param.string = convert_string_to_wstring((uint8_t*) "Hello World!");
 *        param.length = strlen("Hello World!");
 *        param.baseline_height = -1;
 *        gdi_font_engine_display_string(&param);
 *
 *        // Update the screen.
 *        gdi_lcd_update_screen(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
 *    }
 *  @endcode
 * - Step 6: Check the UI output.\n
 *   Basic UI screen will be shown on the HDK's display.\n
 *   @image html Basic_UI_Screen.png
 *
 * @addtogroup Graphics
 * @{
 * This section provides APIs to display images and basic shapes, such as points, lines, rectangles, circles and polygons. 
 * Only BMP image is supported in SDK.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b GDI                        | Graphics Device Interface. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Graphics_Device_Interface">Wikipedia</a>.|
 * |\b RGB                        | An additive color model in which red, green and blue light are added together in various ways to reproduce a broad array of colors. For more information, please refer to <a href="https://en.wikipedia.org/wiki/RGB_color_model">Wikipedia</a>.|
 * |\b BLT                        | Bit block transfer, is a data operation commonly used in computer graphics in which several bitmaps are combined into one using a Boolean function. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Bit_blit">Wikipedia</a>.|
 *
 * @section gdi_api_usage How to use this module
 * - Step 1: Initialize the GDI.
 *  - Sample Code:
 *     @code
 *                #define LCD_WITH   (240)
 *                #define LCD_HEIGHT (240)
 *                static uint8_t layer_buffer[LCD_WIDTH * LCD_HEIGHT * 2];
 *                gdi_init(LCD_WIDTH, LCD_HEIGHT, GDI_COLOR_FORMAT_16, layer_buffer);
 *     @endcode
 * - Step 2: Use GDI APIs to draw. To display an image, please use the resource generator tool to add image resource with image_id and run this tool to populate raw image data. 
 *           For resource generator tool usage, please navigate to ".\tools\mtk\Resgen\" in Windows command window and type in "make ?".
 *  - Sample code:
 *     @code
 *                gdi_draw_filled_rectangle(0, 0, LCD_WIDTH, LCD_HEIGHT, gdi_get_color_from_argb(0, 0, 0, 0)); // Clear the screen to black.
 *                gdi_draw_rectangle(50, 50, 100, 100, gdi_get_color_from_argb(0, 255, 0, 0)); // Draw a red rectangle.
 *                gdi_image_draw_by_id(10, 10, image_id);
 *     @endcode
 *
 * - Step 3: Update the screen.
 *  - Sample code:
 *     @code
 *                gdi_lcd_update_screen(0, 0, LCD_WITH, LCD_HEIGHT);
 *     @endcode
 *
 */
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @defgroup GDI_define Define
 * @{
 * This section defines macros for the GDI.
 */
#define GDI_COLOR_FORMAT_1          0       /**< 1-bit, black/white color. */
#define GDI_COLOR_FORMAT_8          1       /**< 8-bit, index color. */
#define GDI_COLOR_FORMAT_16         2       /**< 16-bit, RGB 565. */
#define GDI_COLOR_FORMAT_24         3       /**< 24-bit, B,G,R 8,8,8. */
#define GDI_COLOR_FORMAT_32         4       /**< 32-bit, B,G,R,A  8,8,8,8. */

#define GDI_SUCCEED        (0)              /**< The operation completed successfully. */
#define GDI_FAILED         (-1)             /**< The operation has failed. */

/**
 * @}
 */

/**
 * @defgroup GDI_struct Struct
 * @{
 * Basic data structure for the GDI.
 */
typedef uint32_t gdi_color_t;            /**< Define the GDI color type. */
typedef uint8_t  gdi_color_format_t;     /**< Define the GDI color format type. */
typedef int32_t  gdi_result_t;           /**< Define the GDI result type returned from the APIs. */

/**
 *  @brief Point type.
 */
typedef struct {
    int16_t x;     /**< The x position of the point. */
    int16_t y;     /**< The y position of the point. */
} gdi_point_t;
/**
 * @}
 */

/**
 * @brief This function initializes the GDI.
 * @param[in] width           is the width of the LCD.
 * @param[in] height          is the height of the LCD.
 * @param[in] color_format    is the color format of the buffer.
 * @param[in] buffer          is the buffer containing the drawing content.
 * @return                    void.
 */
void gdi_init(uint32_t width, uint32_t height, gdi_color_format_t color_format, uint8_t *buffer);
/**
 * @brief This function generates a color from given alpha, red, green, blue.
 * @param[in] alpha           is the alpha component value to generate the color.
 * @param[in] red             is the red component value to generate the color.
 * @param[in] green           is the green component value to generate the color.
 * @param[in] blue            is the blue component value to generate the color.
 * @return                    the generated color.
 */
gdi_color_t gdi_get_color_from_argb(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);
/**
 * @brief This function extracts alpha, red, green, blue from the given color, the value range of the component is relative with the color format.
 * @param[out] alpha          is the alpha component value of the given color.
 * @param[out] red            is the red component value of the given color.
 * @param[out] green          is the green component value of the given color.
 * @param[out] blue           is the blue component value of the given color.
 * @param[in] color           is the given color to extract alpha, red, green, blue.
 * @return                    void.
 */
void gdi_get_argb_from_color(uint8_t *alpha, uint8_t *red, uint8_t *green, uint8_t *blue, gdi_color_t color);
/**
 * @brief This function draws a point on the screen.
 * @param[in] x               is the x position of the point.
 * @param[in] y               is the y position of the point.
 * @param[in] color           is the color of the point.
 * @return                    void.
 */
void gdi_draw_point(int32_t x, int32_t y, gdi_color_t color);
/**
 * @brief This function draws a line on the screen.
 * @param[in] x1              is the x position of the first end point of line.
 * @param[in] y1              is the y position of the first end point of line.
 * @param[in] x2              is the x position of the other end point of line.
 * @param[in] y2              is the y position of the other end point of line.
 * @param[in] color           is the color of the line.
 * @return                    void.
 */
void gdi_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t color);
/**
 * @brief This function draws a rectangle on the screen.
 * @param[in] x1              is the x position of the left-top corner of rectangle.
 * @param[in] y1              is the y position of the left-top corner of rectangle.
 * @param[in] x2              is the x position of the right-down corner of rectangle.
 * @param[in] y2              is the y position of the right-down corner of rectangle.
 * @param[in] color           is the color of the rectangle frame.
 * @return                    void.
 */
void gdi_draw_rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t color);
/**
 * @brief This function draws a solid rectangle on the screen.
 * @param[in] x1              is the x position of the left-top corner of rectangle.
 * @param[in] y1              is the y position of the left-top corner of rectangle.
 * @param[in] x2              is the x position of the right-down corner of rectangle.
 * @param[in] y2              is the y position of the right-down corner of rectangle.
 * @param[in] color           is the color to fill the rectangle.
 * @return                    void.
 */
void gdi_draw_filled_rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t color);
/**
 * @brief This function draws a circle on the screen.
 * @param[in] x               is the x position of the center of the circle.
 * @param[in] y               is the y position of the center of the circle.
 * @param[in] radius          is the radius of the circle.
 * @param[in] color           is the color of the circle.
 * @return                    void.
 */
void gdi_draw_circle(int32_t x, int32_t y, int32_t radius, gdi_color_t color);
/**
 * @brief This function draws a solid circle on the screen.
 * @param[in] x               is the x position of the center of the circle.
 * @param[in] y               is the y position of the center of the circle.
 * @param[in] radius          is the radius of the circle.
 * @param[in] color           is the color to fill the circle.
 * @return                    void.
 */
void gdi_draw_filled_circle(int32_t x, int32_t y, int32_t radius, gdi_color_t color);
/**
 * @brief This function draws a solid polygon on current screen.
 * @param[in] point           is the point array.
 * @param[in] point_num       is the number of elements of the point array.
 * @param[in] color           is the color to fill the polygon.
 * @return                    void.
 */
void gdi_draw_filled_polygon(gdi_point_t point[], uint32_t point_num, gdi_color_t color);
/**
 * @brief This function draws a polygon on current screen.
 * @param[in] point           is the point array.
 * @param[in] point_num       is the number of elements of the point array.
 * @param[in] color           is the color of polygon.
 * @return                    void.
 */
void gdi_draw_polygon(gdi_point_t point[], uint32_t point_num, gdi_color_t color);
/**
 * @brief This function draws an image from a resource with original image dimension.
 * @param[in] x               is the x position to draw image.
 * @param[in] y               is the y position to draw image.
 * @param[in] image_id        is the image resource ID.
 * @return                    whether the operation is successful.
 */
gdi_result_t gdi_image_draw_by_id(int32_t x, int32_t y, uint16_t image_id);
/**
 * @brief This function draws an image from a resource with specified dimensions.
 * @param[in] x               is the x position to draw image.
 * @param[in] y               is the y position to draw image.
 * @param[in] width           is the image width to draw.
 * @param[in] height          is the image height to draw.
 * @param[in] image_id        is the image resource ID.
 * @return                    whether the operation is successful.
 */
gdi_result_t gdi_image_draw_resized_by_id(int32_t x, int32_t y, uint32_t width, uint32_t height, uint16_t image_id);
/**
 * @brief This function gets the image dimension by the image resource ID.
 * @param[in] image_id     is the image resource ID.
 * @param[out] width       is the image width.
 * @param[out] height      is the image height.
 * @return                 whether the operation is successful.
 */
gdi_result_t gdi_image_get_dimension_by_id(uint16_t image_id, uint32_t *width, uint32_t *height);
/**
 * @brief This function updates the LCD screen. The four parameters are the region to update the LCD screen.
 * @param[in] x1              is x of left-top corner of update region.
 * @param[in] y1              is y of left-top corner of update region.
 * @param[in] x2              is x of right-down corner of update region.
 * @param[in] y2              is y of right-down corner of update region.
 * @return                    whether the operation is successful.
 */
gdi_result_t gdi_lcd_update_screen(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 * @}
 */
#endif /* _GDI_INCLUDE_H_ */


