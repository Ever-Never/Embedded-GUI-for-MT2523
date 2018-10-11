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

#include "gdi_internal.h"
#include <stdlib.h>
#include <math.h>
#include "gdi_const.h"
#include "gdi.h"
#include <string.h>
#include "gdi_datatype.h"
#include "gdi_primitive.h"
#include "gd_include.h"
#include "gd_primitive_arm.h"
#include <assert.h>

#if defined(GDI_LAYER_PARGB6666_SUPPORT)
void gdi_memset24(uint8_t* address,uint32_t pixel,int size);
#endif
void gdi_memset32(uint8_t* address,uint32_t pixel,int size);

/****************************************************************************
* Global variable 
*****************************************************************************/

/****************************************************************************
* MACRO
*****************************************************************************/
#define SWAP(t,a,b) do {t tmp=a;a=b;b=tmp;} while(0)

/* check if pixel within clip, if not will return */
#define	GDI_CLIP_POINT_TEST(x, y)\
{\
	if( (x < GDI_LAYER.clipx1) || (x > GDI_LAYER.clipx2) ||\
		(y < GDI_LAYER.clipy1) || (y > GDI_LAYER.clipy2) )\
	{\
		GDI_RETURN_VOID;	\
	}\
}

#define	GDI_CLIP_TEXT_POINT_TEST(x, y)\
{\
	if( (x < GDI_LAYER.act_text_clip.x1) || (x > GDI_LAYER.act_text_clip.x2) ||\
		(y < GDI_LAYER.act_text_clip.y1) || (y > GDI_LAYER.act_text_clip.y2) )\
	{\
		GDI_RETURN_VOID;	\
	}\
}

#define GDI_CLIP_LINE_X_TEST(X1, X2)\
{\
	if(X1 < GDI_LAYER.clipx1)\
		X1 = GDI_LAYER.clipx1;\
	if(X2 > GDI_LAYER.clipx2)\
		X2 = GDI_LAYER.clipx2;\
}

#define GDI_CLIP_LINE_Y_TEST(Y1, Y2)\
{\
	if(Y1 < GDI_LAYER.clipy1)\
		Y1 = GDI_LAYER.clipy1;\
	if(Y2 > GDI_LAYER.clipy2)\
		Y2 = GDI_LAYER.clipy2;\
}

#define GDI_CLIP_LINE_XY_TEST(x1,y1,x2,y2)\
{\
	if(x1 < GDI_LAYER.clipx1)\
		x1 = GDI_LAYER.clipx1;\
	if(x2 > GDI_LAYER.clipx2)\
		x2 = GDI_LAYER.clipx2;\
	if(y1 < GDI_LAYER.clipy1)\
		y1 = GDI_LAYER.clipy1;\
	if(y2 > GDI_LAYER.clipy2)\
		y2 = GDI_LAYER.clipy2;\
}

#define AA_LINE_PRECISION_BITS 16
#define AA_LINE_FLOAT ((int32_t)((float)(x)*(float)(1 << AA_LINE_PRECISION_BITS)))


/*****************************************************************************
 * FUNCTION
 *  GAUSSIAN_INTEGRAL
 * DESCRIPTION
 *  return the value of gaussian integral
 * PARAMETERS
 *  dis         [IN]    the x (the distance from x to zero point)
 *  integral    [OUT]   the integral of gaussian from dis to infinity
 * RETURNS
 *  void
 *****************************************************************************/
#define GAUSSIAN_INTEGRAL(dis, integral)                    \
do                                                          \
{                                                           \
    int32_t idx;                                                \
    if (dis > (2 << AA_LINE_PRECISION_BITS) ||              \
        dis < -(2 << AA_LINE_PRECISION_BITS))               \
    {                                                       \
        integral = 0;                                       \
    }                                                       \
    if (dis >= 0)                                           \
    {                                                       \
        idx = (dis * 10) >> AA_LINE_PRECISION_BITS;         \
        integral = gaussian_integral[idx];                  \
    }                                                       \
    else                                                    \
    {                                                       \
        dis = -dis;                                         \
        idx = (dis * 10) >> AA_LINE_PRECISION_BITS;         \
        integral = ((1 << AA_LINE_PRECISION_BITS) -         \
                    gaussian_integral[idx]);                \
    }                                                       \
} while(0)


/*****************************************************************************
 * FUNCTION
 *  ANTIALIASING_LINE_PUT_PIXEL
 * DESCRIPTION
 *  put pixel in gdi_draw_antialiasing_line
 * PARAMETERS
 *  x           [IN]    x (position of the point to be draw)
 *  y           [IN]    y
 *  lineR       [IN]    the red color of line
 *  lineG       [IN]    the green color of line
 *  lineB       [IN]    the blue color of line
 *  dis_to_line [IN]    the distance that the pixel to the line
 * RETURNS
 *  void
 *****************************************************************************/
#define ANTIALIASING_LINE_PUT_PIXEL(x, y, lineR, lineG, lineB, dis_to_line)                         \
do                                                                                                  \
{                                                                                                   \
    uint8_t bgA, bgR, bgG, bgB;                                                                         \
    /*uint32_t lineA, lineR, lineG, lineB;*/                                                             \
    uint8_t colorR, colorG, colorB;                                                                     \
    gdi_color_t bg_color;                                                                             \
    int32_t integral, integral_inver, g1, g2, dis1, dis2;                                               \
                                                                                                    \
    dis1 = abs(dis_to_line) - (1 << (AA_LINE_PRECISION_BITS-1));                                    \
    dis2 = abs(dis_to_line) + (1 << (AA_LINE_PRECISION_BITS-1));                                    \
    GAUSSIAN_INTEGRAL(dis1, g1);                                                                    \
    GAUSSIAN_INTEGRAL(dis2, g2);                                                                    \
    integral = g1 - g2;                                                                             \
    integral_inver = (1 << AA_LINE_PRECISION_BITS) - integral;                                      \
    bg_color = gdi_act_get_pixel(x, y);                                                             \
    gdi_act_color_to_rgb(&bgA, &bgR, &bgG, &bgB, bg_color);                                         \
    /* if background color is transparent color, use color in abm_src_layer */                      \
    if (bg_color == source_key_value && source_key_enable && abm_src_layer != GDI_ERROR_HANDLE)     \
    {                                                                                               \
        bg_color = src_layer_get_pixel(src_layer_buf_ptr, src_layer_layer_width, x, y); \
        src_layer_color_to_rgb(&bgA, &bgR, &bgG, &bgB, bg_color);                               \
    }                                                                                               \
    colorR = (uint8_t)((integral_inver * bgR) + (integral * lineR)) >> AA_LINE_PRECISION_BITS;          \
    colorG = (uint8_t)((integral_inver * bgG) + (integral * lineG)) >> AA_LINE_PRECISION_BITS;          \
    colorB = (uint8_t)((integral_inver * bgB) + (integral * lineB)) >> AA_LINE_PRECISION_BITS;          \
    gdi_draw_point(x, y, gdi_act_color_from_rgb(bgA, colorR, colorG, colorB));                   \
} while(0)

/* draw horizontal line */
void GDI_DRAW_H_LINE(int32_t x1, int32_t x2, int32_t y, gdi_color_t pixel_color)
{
	GDI_ENTER_CRITICAL_SECTION(GDI_DRAW_H_LINE)
		int32_t	h_index;
		for(h_index = x1; h_index <= x2; h_index++)
		{
			GDI_SET_BUFFER_PIXEL(h_index, y, pixel_color);
		}
	GDI_EXIT_CRITICAL_SECTION(GDI_DRAW_H_LINE)
}

void GDI_DRAW_H_LINE_STYLE(int32_t x1, int32_t x2, int32_t y, gdi_color_t pixel_color, uint8_t cycle, const uint8_t *bitvalues)
{
	GDI_ENTER_CRITICAL_SECTION(GDI_DRAW_H_LINE_STYLE)
		int32_t		h_index;
		uint8_t bitcount;
		for(h_index = x1, bitcount=0; h_index <= x2; h_index++, bitcount++, bitcount %= cycle)
		{
			if (bitvalues[bitcount])
			{
				GDI_SET_BUFFER_PIXEL(h_index, y, pixel_color);
			}
		}
	GDI_EXIT_CRITICAL_SECTION(GDI_DRAW_H_LINE_STYLE)
}

/* draw vertical line */
void GDI_DRAW_V_LINE(int32_t y1, int32_t y2, int32_t x, gdi_color_t pixel_color)
{
	GDI_ENTER_CRITICAL_SECTION(GDI_DRAW_V_LINE)
		int32_t	v_index;

		for(v_index = y1; v_index <= y2; v_index++)
		{
			GDI_SET_BUFFER_PIXEL(x,v_index,pixel_color);
		}
	GDI_EXIT_CRITICAL_SECTION(GDI_DRAW_V_LINE)
}

void GDI_DRAW_V_LINE_STYLE(int32_t y1, int32_t y2, int32_t x, gdi_color_t pixel_color, uint8_t cycle, const uint8_t *bitvalues)
{
	GDI_ENTER_CRITICAL_SECTION(GDI_DRAW_V_LINE_STYLE)
		int32_t		v_index;
		uint8_t		bitcount;
		for(v_index = y1, bitcount=0; v_index <= y2; v_index++, bitcount++, bitcount %= cycle)
		{
			if (bitvalues[bitcount])
			{
				GDI_SET_BUFFER_PIXEL(x, v_index, pixel_color);
			}
		}
	GDI_EXIT_CRITICAL_SECTION(GDI_DRAW_V_LINE_STYLE)
}

static void GDI_DRAW_RECT(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t pixel_color)
{
	int32_t	r_index;
	for(r_index = y1; r_index <=y2; r_index++)
	{
		GDI_DRAW_H_LINE(x1, x2, r_index, pixel_color);
	}
}

static void GDI_DRAW_RECT16(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t pixel_color)
{
	uint8_t* dest = GDI_LAYER.buf_ptr + ((x1+y1*GDI_LAYER.width)<<1);

	y2 -=y1; 
	y2++;

	x2-=x1;
	x2++;
	x2<<=1;
	x1 = GDI_LAYER.width<<1;
	
	while(y2--)
	{
		gdi_memset16(dest,(uint16_t)pixel_color,x2);
		dest +=x1;
	}
}

#ifdef GDI_PRIMITIVE_24_BIT_SUPPORT
static void GDI_DRAW_RECT24(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t pixel_color)
{
	uint8_t* dest = GDI_LAYER.buf_ptr + ((x1+y1*GDI_LAYER.width) * 3);

	y2 -=y1; 
	y2++;

	x2-=x1;
	x2++;
	x2 *= 3;
	x1 = GDI_LAYER.width * 3;
	
	while(y2--)
	{
		gdi_memset24(dest,(uint32_t)pixel_color,x2);
		dest +=x1;
	}
}
#endif

static void GDI_DRAW_RECT32(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t pixel_color)
{
	uint8_t* dest = GDI_LAYER.buf_ptr + ((x1+y1*GDI_LAYER.width)<<2);

	y2 -=y1; 
	y2++;

	x2-=x1;
	x2++;
	x2<<=2;
	x1 = GDI_LAYER.width<<2;
	
	while(y2--)
	{
		gdi_memset32(dest,(uint32_t)pixel_color,x2);
		dest +=x1;
	}
}

static void gdi_color_interpolate(gdi_color_t color0, gdi_color_t color1, uint32_t orig_ratio, gdi_color_t *result)
{
    uint8_t a0,r0,g0,b0;
    uint8_t a1,r1,g1,b1;
    uint32_t _100_RATIO = 100 - orig_ratio;
    gdi_act_color_to_rgb(&a0, &r0, &g0, &b0, color0);
    gdi_act_color_to_rgb(&a1, &r1, &g1, &b1, color1);
    *result = gdi_act_color_from_rgb(
                    (a0 * orig_ratio + a1 * _100_RATIO) / 100,
                    (r0 * orig_ratio + r1 * _100_RATIO) / 100,
                    (g0 * orig_ratio + g1 * _100_RATIO) / 100,
                    (b0 * orig_ratio + b1 * _100_RATIO) / 100);
}
/**************************************************************

	FUNCTION NAME		: gdi_draw_point()

  	PURPOSE				: draw a point

	INPUT PARAMETERS	: int32_t			x
							  int32_t			y
							  gdi_color_t pixel_color

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_point(int32_t x, int32_t y, gdi_color_t pixel_color)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_draw_point)
		GDI_CLIP_POINT_TEST(x,y);
		GDI_SET_BUFFER_PIXEL(x,y,pixel_color);
	GDI_EXIT_CRITICAL_SECTION(gdi_draw_point)
}

void gdi_draw_line_ext(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t line_color, bool is_dotted)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_draw_line_ext)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool steep;                                             /* y-scan or x-scan */
    int32_t slope, dx, dy, d, incrE, incrNE, x, y;              /* slope is the direction that y goes */
    int32_t rate, tmp;                                          /* used in cliping */
    int32_t clipx1, clipx2, clipy1, clipy2;                     /* the clip area */
    bool draw_dot = true;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //assert((GDI_LAYER.clipy1 <= GDI_LAYER.clipy2) && (GDI_LAYER.clipx1 <= GDI_LAYER.clipx2));

    {
        clipx1 = x1;    /* clipx1,y1,x2,y2 is only used as tmp variables here */
        clipy1 = y1;
        clipx2 = x2;
        clipy2 = y2;
        if (x1 > x2)
        {
            clipx1 = x2;
            clipx2 = x1;
        }
        if (y1 > y2)
        {
            clipy1 = y2;
            clipy2 = y1;
        }
        /* return if the line is not in the clip region */
        GDI_CLIP_RECT_TEST(clipx1, clipy1, clipx2, clipy2, GDI_RETURN_VOID);
    }
    
    if ((!is_dotted) && (y1 == y2) && (y1 >= GDI_LAYER.clipy1) && (y1 <= GDI_LAYER.clipy2)) // Horizontal Line /
	{
		if(x1 > x2)     // exchange x /
		{
		    tmp = x1;
            x1 = x2;
            x2 = tmp;
		}
        if((x1 <= GDI_LAYER.clipx2) && (x2 >= GDI_LAYER.clipx1))
        {
            GDI_CLIP_LINE_X_TEST(x1,x2);
            GDI_DRAW_H_LINE(x1, x2, y1, line_color);
    	}
	}
	else if ((!is_dotted) && (x1 == x2) && (x1 >= GDI_LAYER.clipx1) && (x1 <= GDI_LAYER.clipx2)) // Vertical Line /
    {
		if(y1 > y2)     // exchange y /
		{
			tmp = y1;
            y1 = y2;
            y2 = tmp;
		}
        if((y1 <= GDI_LAYER.clipy2) && (y2 >= GDI_LAYER.clipy1))
        {
            GDI_CLIP_LINE_Y_TEST(y1,y2);
            GDI_DRAW_V_LINE(y1, y2, x1, line_color);
	    }
	}
    else
    {
        /* clip */
        gdi_layer_get_clip(&clipx1, &clipy1, &clipx2, &clipy2);

        if (x1 > x2)    
        {
            /* exchange two points: (x1,y1) and (x2,y2)*/
            tmp = x1;
            x1 = x2;
            x2 = tmp;

            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        dx = x2 - x1;
        dy = y2 - y1;
        
        if (dx == 0)
        {
            rate = 0;
        }
        else
        {
            rate = (dy << 20) / dx;
        }
        
        if (y1 <= y2) 
    	{
    		if (y1 < clipy1) 
    		{
    			int orgy = y1;
    			y1 = clipy1;
    			if (y1 > y2)
    				GDI_RETURN_VOID;
    			x1 = x1 - ((orgy - y1) * dx) / dy;
    		}

    		if (y2 > clipy2) 
    		{
    			int orgy = y2;
    			y2 = clipy2;
    			if (y1 > y2)
    				GDI_RETURN_VOID;
    			x2 = x2 - ((orgy - y2) * dx) / dy;
    		}
    	} 
    	else 
    	{ /* y1 > y2 */
    		if (y2 < clipy1) 
    		{
    			int orgy = y2;
    			y2 = clipy1;
    			if (y2 > y1)
    				GDI_RETURN_VOID;
    			x2 = x2 - ((orgy - y2) * dx) / dy;
    		}

    		if (y1 > clipy2) 
    		{
    			int orgy = y1;
    			y1 = clipy2;
    			if (y2 > y1)
    				GDI_RETURN_VOID;
    			x1 = x1 - ((orgy - y1) * dx) / dy;
    		}
    	}

    	if (x1 < clipx1) 
    	{
    		int orgx = x1;
    		x1 = clipx1;
    		if (x1 > x2)
    			GDI_RETURN_VOID;
    		y1 = y1 - (((orgx - x1) * rate) >> 20);
    	}

    	if (x2 > clipx2) 
    	{
    		int orgx = x2;
    		x2 = clipx2;
    		if (x1 > x2)
    			GDI_RETURN_VOID;
    		y2 = y2 - (((orgx - x2) * rate) >> 20);
    	}
        
        /* if the slope>1, need to exchange x-axis and y-axis */
        steep = (abs(y2 - y1) > abs(x2 - x1));  
        if (steep)
        {
            /* exchange x1 and y1, x2 and y2 */
            tmp = x1;
            x1 = y1;
            y1 = tmp;

            tmp = x2;
            x2= y2;
            y2 = tmp;
        }
        
        if (x1 > x2)    
        {
            /* exchange two points: (x1,y1) and (x2,y2) */
            tmp = x1;
            x1 = x2;
            x2 = tmp;

            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        
        dx = x2 - x1;
        dy = y2 - y1;
        
        if (dy < 0)
        {
            slope = -1;
            dy = -dy;
        }
        else
        {
            slope = 1;
        }

        /* variables for mid-point draw line algorithm */
        d = (2 * dy) - dx;
        incrE = 2 * dy;
        incrNE = 2 * (dy - dx);

        for (x = x1, y = y1; x <= x2; x++)
        {
            int32_t outx, outy;
            if (steep)
            {
                outx = y;
                outy = x;
            }
            else
            {
                outx = x;
                outy = y;
            }

            if (is_dotted)
            {
                if (draw_dot == false)
                {
                    draw_dot = true;
                }
                else
                {
                    GDI_CLIP_POINT_TEST(outx,outy);
                    gdi_act_put_pixel(outx, outy, line_color);
                    draw_dot = false;
                }
            }
            else
            {
                GDI_CLIP_POINT_TEST(outx,outy);
                gdi_act_put_pixel(outx, outy, line_color);
            }
 
            if (d < 0)
            {
                d += incrE;
            }
            else
            {
                d += incrNE;
                y += slope;
            }
        }
    }
 
    GDI_EXIT_CRITICAL_SECTION(gdi_draw_line_ext)
}

/**************************************************************

	FUNCTION NAME		: gdi_draw_line()

  	PURPOSE				: draw a line

	INPUT PARAMETERS	: int32_t			x1
							  int32_t			y1
							  int32_t			x2
							  int32_t			y2
							  gdi_color_t line_color

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t line_color)
{
	gdi_draw_line_ext(x1,y1,x2,y2, line_color,false);
}

/**************************************************************

	FUNCTION NAME		: gdi_draw_line_style()

  	PURPOSE				: draw a style line

	INPUT PARAMETERS	: int32_t			x1
							  int32_t			y1
							  int32_t			x2
							  int32_t			y2
							  gdi_color_t line_color
							  uint8_t			cycle
							  const uint8_t  *bitvalues
	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_line_style(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t line_color, uint8_t cycle, const uint8_t *bitvalues)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_draw_line_style)
		if(y1 - y2 == 0) /* Horizontal Line */
		{
			/* within valid y clip */
			if((y1 >= GDI_LAYER.clipy1) && (y1 <= GDI_LAYER.clipy2))
			{
				if((x1 < x2) && (x1 <= GDI_LAYER.clipx2) && (x2 >= GDI_LAYER.clipx1))
				{
					GDI_CLIP_LINE_X_TEST(x1,x2);
					GDI_DRAW_H_LINE_STYLE(x1, x2, y1, line_color, cycle, bitvalues);
				}
				else if((x1 >= x2) && (x2 <= GDI_LAYER.clipx2) && (x1 >= GDI_LAYER.clipx1))
				{
					GDI_CLIP_LINE_X_TEST(x2,x1);
					GDI_DRAW_H_LINE_STYLE(x2, x1, y1, line_color, cycle, bitvalues);
				}
			}
		}
		else if(x1 - x2 == 0) /* Vertical Line */
		{
			/* within valid x clip */
			if((x1 >= GDI_LAYER.clipx1) && (x1 <= GDI_LAYER.clipx2))
			{
				if((y1 < y2) && (y1 <= GDI_LAYER.clipy2) && (y2 >= GDI_LAYER.clipy1))
				{
					GDI_CLIP_LINE_Y_TEST(y1,y2);
					GDI_DRAW_V_LINE_STYLE(y1, y2, x1, line_color, cycle, bitvalues);
				}
				else if((y1 >= y2) && (y2 <= GDI_LAYER.clipy2) && (y1 >= GDI_LAYER.clipy1))
				{
					GDI_CLIP_LINE_Y_TEST(y2,y1);
					GDI_DRAW_V_LINE_STYLE(y2, y1, x1, line_color, cycle, bitvalues);
				}
			}
		}
		else
		{
			gdi_draw_line_ext(x1,y1,x2,y2, line_color,true); //FIXME: should support real dotted & cycle
		}
	GDI_EXIT_CRITICAL_SECTION(gdi_draw_line_style)
}

/**************************************************************

	FUNCTION NAME		: gdi_draw_rectangle()

  	PURPOSE				: draw a rect (border only)

	INPUT PARAMETERS	: int32_t			x1
							  int32_t			y1
							  int32_t			x2
							  int32_t			y2
							  gdi_color_t frame_color

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t frame_color)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_draw_rectangle)
		int32_t nx1,ny1,nx2,ny2;

		nx1 = x1;
		ny1 = y1;
		nx2 = x2;
		ny2 = y2;

		GDI_CLIP_RECT_TEST(nx1,ny1,nx2,ny2,GDI_RETURN_VOID);

		if(GDI_LAYER.clipy1<= y1 && y1 <= GDI_LAYER.clipy2)
			GDI_DRAW_H_LINE(nx1, nx2, y1, frame_color);	/* up horizontal */

		if(GDI_LAYER.clipy1<= y2 && y2 <= GDI_LAYER.clipy2)
			GDI_DRAW_H_LINE(nx1, nx2, y2, frame_color);	/* down horizontal */

		if(GDI_LAYER.clipx1<= x1 && x1 <= GDI_LAYER.clipx2)
			GDI_DRAW_V_LINE(ny1, ny2, x1, frame_color);	/* left vertical */

		if(GDI_LAYER.clipx1<= x2 && x2 <= GDI_LAYER.clipx2)
			GDI_DRAW_V_LINE(ny1, ny2, x2, frame_color);	/* right vertical */
	GDI_EXIT_CRITICAL_SECTION(gdi_draw_rectangle)
}

/**************************************************************

	FUNCTION NAME		: gdi_draw_filled_rectangle()

  	PURPOSE				: draw a solid rect

	INPUT PARAMETERS	: int32_t			x1
							  int32_t			y1
							  int32_t			x2
							  int32_t			y2
							  gdi_color_t rect_color

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_filled_rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t rect_color)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_draw_filled_rectangle)
    do
    {
        GDI_CLIP_RECT_TEST(x1,y1,x2,y2,break);
    
        if(GDI_LAYER.bits_per_pixel == 16)
        {
            if (GDI_LAYER.cf == GDI_COLOR_FORMAT_UYVY422)
            {
                rect_color = 0x0080; /* Only support clear to black */
            }
        
            GDI_DRAW_RECT16(x1, y1, x2, y2, rect_color);
        }
        else if ((GDI_LAYER.bits_per_pixel == 32) && (rect_color == GDI_COLOR_TRANSPARENT))
        {   /* speed up 32-bit performance by memset(), it is safe to replace GDI_COLOR_TRANSPARENT as 0 since both alpha = 0 */
            uint32_t *p;
            int32_t line_width, pitch; 
            int32_t y;

            line_width = x2 - x1 + 1;
            pitch = GDI_LAYER.width;
            p = (uint32_t*)GDI_LAYER.buf_ptr + x1 + y1 * pitch;

            for (y = y2 - y1; y >= 0; y--)
            {
                memset(p, 0, line_width * 4);
                p += pitch; 
            }               
        }
    #ifdef GDI_PRIMITIVE_24_BIT_SUPPORT
        else if (GDI_LAYER.bits_per_pixel == 24)
        {
            uint32_t cbyte = (rect_color & 0xff);
            if (cbyte == (rect_color << 16 >> 24) &&
                cbyte == (rect_color << 8 >> 24))
            {
                uint32_t pitch;
                int32_t line_width;
                int32_t line_count;
                uint8_t *line_ptr;

                line_width = (x2 - x1 + 1) * 3;
                pitch = GDI_LAYER.width * 3;
                line_ptr = (uint8_t*)GDI_LAYER.buf_ptr + y1 * pitch + x1 * 3;
                for (line_count = y2 - y1; line_count >= 0; line_count--)
                {
                    memset(line_ptr, cbyte, line_width);
                    line_ptr += pitch;
                }
            }
            else
            {
                GDI_DRAW_RECT24(x1, y1, x2, y2, rect_color);
            }
        }
    #endif
        else if(GDI_LAYER.bits_per_pixel == 32)
        {
            GDI_DRAW_RECT32(x1, y1, x2, y2, rect_color);                
        }
        else
        {
            GDI_DRAW_RECT(x1, y1, x2, y2, rect_color);
        }
    } while(0);
    GDI_EXIT_CRITICAL_SECTION(gdi_draw_filled_rectangle)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_draw_greyscale_rect
 * DESCRIPTION
 *  draw a greyscale rectangle
 *
 *  The greyscale value of a pixel is decided by the following described.
 *  1. Get the average of r, g, b color of the pixel, called "ave"
 *  2. tmp = ave + (256-ave) * white_value / 256
 *     tmp will be a value between ave and 255(white color).
 *     256:white_value = (256-ave):(t01-ave)
 *
 *     256-----------|----------|------0
 *                  tmp        ave     
 *
 *  3. grey = tmp * black_value / 256
 *     256:black_value = tmp:grey
 *
 *     tmp-----------|-----------------0
 *                  grey             
 * PARAMETERS
 *  x1                : [IN]
 *  y1                : [IN]
 *  x2                : [IN]
 *  y2                : [IN]
 *  white_value       : [IN]
 *  black_value       : [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_draw_greyscale_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t white_value, int32_t black_value)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_draw_greyscale_rect)	
	int32_t x, y;
	gdi_color_t color;
	uint8_t a,r, g, b;//, max, min;
	gd_get_pixel_func get_ptr = gd_get_pixel[gdi_act_layer->cf];
	gd_put_pixel_func put_ptr = gd_put_pixel[gdi_act_layer->cf];
	int32_t BW256,WB_B;

	GDI_CLIP_RECT_TEST(x1,y1,x2,y2,GDI_RETURN_VOID);
	
		{
			black_value = 256 - black_value;
		
			BW256 = (white_value*black_value)<<8;
		
			WB_B = black_value*(256 - white_value);
		
			for (y = y1; y <= y2; y++)
			{
				for (x = x1; x <= x2; x++)
				{
					color = get_ptr(x,y);
					gdi_act_color_to_rgb(&a,&r,&g,&b,color);
					r = (r + g + b)/3;
					//r = r + (256-r) * w / 256
					//r = r * b / 256
					r = (BW256 + r*WB_B) >> 16;
					//if (r > 255)
						//r = 255;
					put_ptr(x,y,gdi_act_color_from_rgb(a,r,r,r));
				}
			}
		}
	GDI_EXIT_CRITICAL_SECTION(gdi_draw_greyscale_rect)	
}

void gdi_effect_alpha_blending_rect_16(
        gdi_layer_handle_t src_layer_handle,
        int32_t x1,
        int32_t y1,
        int32_t x2,
        int32_t y2,
        uint32_t transparent_color_a,
        uint32_t transparent_color_r,
        uint32_t transparent_color_g,
        uint32_t transparent_color_b)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_effect_alpha_blending_rect_16)
    
    do {
        gdi_layer_struct_t *src_layer, *dest_layer;
        uint32_t src_pitch, dest_pitch;
        uint32_t src_delta, dest_delta;
        uint8_t *src_buffer, *dest_buffer;
        int32_t pixel_count, line_count;
        int32_t inv_a;
        uint32_t color_prgb888;
        
        src_layer = (gdi_layer_struct_t*)src_layer_handle;
        dest_layer = gdi_act_layer;

        GDI_ASSERT(src_layer->bits_per_pixel == 16 && dest_layer->bits_per_pixel == 16);

        GDI_CLIP_RECT_TEST(x1, y1, x2, y2, break);

        src_pitch = src_layer->width * 2;
        dest_pitch = dest_layer->width * 2;
        src_buffer = src_layer->buf_ptr + src_pitch * y1 + x1 * 2;
        dest_buffer = dest_layer->buf_ptr + dest_pitch * y1 + x1 * 2;
        src_delta = src_pitch - (x2 - x1 + 1) * 2;
        dest_delta = dest_pitch - (x2 - x1 + 1) * 2;
        color_prgb888 = GDI_PARGB8888(
            0,
            GDI_DIV_255(transparent_color_r * transparent_color_a),
            GDI_DIV_255(transparent_color_g * transparent_color_a),
            GDI_DIV_255(transparent_color_b * transparent_color_a));
        inv_a = 255 - transparent_color_a;
        for (line_count = (y2 - y1 + 1); line_count > 0; line_count--)
        {
            for (pixel_count = (x2 - x1 + 1); pixel_count > 0; pixel_count--)
            {
                register uint32_t rgb565, rgb888;
                uint32_t r, g, b;
                
                rgb565 = GDI_MEM_GET_16(src_buffer);
                rgb888 = GDI_RGB888_FROM_RGB565(rgb565);
                r = GDI_DIV_255(GDI_R_OF_RGB888(rgb888) * inv_a);
                g = GDI_DIV_255(GDI_G_OF_RGB888(rgb888) * inv_a);
                b = GDI_DIV_255(GDI_B_OF_RGB888(rgb888) * inv_a);
                rgb888 = GDI_RGB888(r, g, b);
                rgb888 += color_prgb888;
                rgb565 = GDI_RGB565_FROM_RGB888(rgb888);

                GDI_MEM_PUT_16(dest_buffer, rgb565);

                src_buffer += 2;
                dest_buffer += 2;
            }

            src_buffer += src_delta;
            dest_buffer += dest_delta;
        }
    } while (0);
    
    GDI_EXIT_CRITICAL_SECTION(gdi_effect_alpha_blending_rect_16)
}

void gdi_effect_alpha_blending_rect_ex(
                                gdi_layer_handle_t src_layer_handle,
                                int32_t x1,int32_t y1,int32_t x2,int32_t y2, 
                                uint32_t transparent_color_a,
                                uint32_t transparent_color_r,
                                uint32_t transparent_color_g,
                                uint32_t transparent_color_b)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_effect_alpha_blending_rect_ex)	
    {
		int32_t x, y;
		gdi_color_t color;
		uint8_t a, r, g, b;
        gdi_layer_struct_t *src_layer = (gdi_layer_struct_t*) src_layer_handle;
		uint8_t* src_buf = src_layer->buf_ptr;
		uint32_t src_layer_width = src_layer->width;
		gd_get_buf_pixel_func get_ptr = gd_get_buf_pixel[src_layer->cf];
        gd_color_to_rgb_func src_color_to_rgb = gd_color_to_rgb[src_layer->cf];
        
		int32_t opacity_value = transparent_color_a * 256 / 255;
		int32_t inverse_opacity_value = 256 - transparent_color_a * 256 / 255;
		int32_t offset_x = gdi_act_layer->offset_x - src_layer->offset_x;
		int32_t offset_y = gdi_act_layer->offset_y - src_layer->offset_y;

        transparent_color_a *= 256;
		transparent_color_r *= opacity_value;
		transparent_color_g *= opacity_value;
		transparent_color_b *= opacity_value;

		GDI_CLIP_RECT_TEST(x1,y1,x2,y2,break);

		for (y = y1; y <= y2; y++)
		{
			for (x = x1; x <= x2; x++)
			{
				color = get_ptr(src_buf,src_layer_width,x + offset_x,y + offset_y);
				src_color_to_rgb(&a,&r,&g,&b,color);
				a = ((transparent_color_a) +  (a * inverse_opacity_value)) >> 8;
				r = ((transparent_color_r) +  (r * inverse_opacity_value)) >> 8;
				g = ((transparent_color_g) +  (g * inverse_opacity_value)) >> 8;
				b = ((transparent_color_b) +  (b * inverse_opacity_value)) >> 8;
                gdi_act_put_pixel(x,y,gdi_act_color_from_rgb(a,r,g,b));
			}
		}
    }
	GDI_EXIT_CRITICAL_SECTION(gdi_effect_alpha_blending_rect_ex)
}

void gdi_effect_alpha_blending_rect(gdi_layer_handle_t src_layer_handle,int32_t x1,int32_t y1,int32_t x2,int32_t y2, uint8_t transparent_color_a,uint8_t transparent_color_r,uint8_t transparent_color_g,uint8_t transparent_color_b)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_effect_alpha_blending_rect_ex)
	{
        gdi_layer_struct_t *src = ((gdi_layer_struct_t*)src_layer_handle);
        
        GDI_DEBUG_ASSERT(src_layer_handle);

        if(src->bits_per_pixel == 16 && gdi_act_layer->bits_per_pixel == 16 
        && gdi_act_color_from_rgb(0xff,0,255,0) == 0x07e0 ) // standard rgb565
            gdi_effect_alpha_blending_rect_16(src_layer_handle, x1, y1, x2, y2, transparent_color_a, transparent_color_r, transparent_color_g, transparent_color_b);
        else
            gdi_effect_alpha_blending_rect_ex(src_layer_handle, x1, y1, x2, y2, transparent_color_a, transparent_color_r, transparent_color_g, transparent_color_b);
	}
	GDI_EXIT_CRITICAL_SECTION(gdi_effect_alpha_blending_rect_ex)
}

/**************************************************************

	FUNCTION NAME		: gdi_draw_frame_rect()

  	PURPOSE				: draw a rect with frame and filling.

	INPUT PARAMETERS	: int32_t			x1
							  int32_t			y1
							  int32_t			x2
							  int32_t			y2
							  gdi_color_t rect_color
							  gdi_color_t	frame_color
							  int32_t			frame_width

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_frame_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t rect_color, gdi_color_t frame_color, uint16_t frame_width)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_draw_frame_rect)
		int32_t i;

		GDI_CLIP_RECT_TEST(x1,y1,x2,y2,GDI_RETURN_VOID);

		/* draw interial color */
		GDI_DRAW_RECT(x1+frame_width, y1+frame_width, x2-frame_width, y2-frame_width, rect_color);

		/* draw frame */
		for( i = 0 ; i < frame_width; i++)
		{
			GDI_DRAW_H_LINE(x1+i, x2-i, y1+i, frame_color);	/* up horizontal */
			GDI_DRAW_H_LINE(x1+i, x2-i, y2-i, frame_color);	/* down horizontal */
			GDI_DRAW_V_LINE(y1+i, y2-i, x1+i, frame_color);	/* left vertical */
			GDI_DRAW_V_LINE(y1+i, y2-i, x2-i, frame_color);	/* right vertical */
		}
	GDI_EXIT_CRITICAL_SECTION(gdi_draw_frame_rect)
}

/**************************************************************

	FUNCTION NAME		: gdi_draw_round_rect()

  	PURPOSE				: draw a rect with round frame

	INPUT PARAMETERS	: int32_t			x1
							  int32_t			y1
							  int32_t			x2
							  int32_t			y2
							  gdi_color_t rect_color
							  gdi_color_t	frame_color
							  int32_t			frame_width

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_round_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t rect_color, gdi_color_t frame_color, uint16_t frame_width)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_draw_round_rect)
		int32_t			i;
		int32_t			shift;
		gdi_color_t	result;

		GDI_CLIP_RECT_TEST(x1,y1,x2,y2,GDI_RETURN_VOID);

		/* draw interial color */
		GDI_DRAW_RECT(x1+frame_width, y1+frame_width, x2-frame_width, y2-frame_width, rect_color);


		/* draw frame */
		for( i = 0 ; i < frame_width; i++)
		{
			shift = frame_width - i;
			GDI_DRAW_H_LINE(x1+ shift, x2-shift, y1+i, frame_color);	/* up horizontal */
			GDI_DRAW_H_LINE(x1+ shift, x2-shift, y2-i, frame_color);	/* down horizontal */
			GDI_DRAW_V_LINE(y1+shift, y2-shift, x1+i, frame_color);	/* left vertical */
			GDI_DRAW_V_LINE(y1+shift, y2-shift, x2-i, frame_color);	/* right vertical */
		}

		/* draw anti_aliase pixels, make it smooth */
		gdi_color_interpolate(frame_color, rect_color, 10, &result);

		GDI_SET_BUFFER_PIXEL(x1+frame_width+1, y1+frame_width, result);
		GDI_SET_BUFFER_PIXEL(x1+frame_width,	y1+frame_width+1, result);

		GDI_SET_BUFFER_PIXEL(x1+frame_width+1, y2-frame_width, result);
		GDI_SET_BUFFER_PIXEL(x1+frame_width,	y2-frame_width-1, result);

		GDI_SET_BUFFER_PIXEL(x2-frame_width-1, y1+frame_width, result);
		GDI_SET_BUFFER_PIXEL(x2-frame_width,	y1+frame_width+1, result);

		GDI_SET_BUFFER_PIXEL(x2-frame_width-1, y2-frame_width, result);
		GDI_SET_BUFFER_PIXEL(x2-frame_width,	y2-frame_width-1, result);

		gdi_color_interpolate(frame_color, rect_color, 30, &result);

		GDI_SET_BUFFER_PIXEL(x1+frame_width	, y1+frame_width, result);
		GDI_SET_BUFFER_PIXEL(x1+frame_width	, y2-frame_width, result);
		GDI_SET_BUFFER_PIXEL(x2-frame_width	, y1+frame_width, result);
		GDI_SET_BUFFER_PIXEL(x2-frame_width	, y2-frame_width, result);
	GDI_EXIT_CRITICAL_SECTION(gdi_draw_round_rect)
}

/**************************************************************

	FUNCTION NAME		: gdi_draw_button_rect()

  	PURPOSE				: draw a rect with button style

	INPUT PARAMETERS	: int32_t			x1
							  int32_t			y1
							  int32_t			x2
							  int32_t			y2
							  gdi_color_t rect_color
							  gdi_color_t	frame_color
							  int32_t			frame_width
							  uint16_t			button_height
							  bool		invert

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_button_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t rect_color, gdi_color_t frame_color, uint16_t frame_width, uint16_t button_height, bool invert)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_draw_button_rect)
		int32_t		i;
		uint16_t		shift;
		uint16_t		ratio_light;
		uint16_t		ratio_dark;

		gdi_color_t result_light;
		gdi_color_t retult_dark;
		gdi_color_t black = gdi_act_color_from_rgb(0xff,0,0,0);
		gdi_color_t white = gdi_act_color_from_rgb(0xff,255,255,255);

		/* draw rect and frame */
		GDI_CLIP_RECT_TEST(x1,y1,x2,y2,GDI_RETURN_VOID);
		gdi_draw_frame_rect(x1, y1, x2, y2, rect_color, frame_color, frame_width);


		/* add button style */
		ratio_light = 70;
		ratio_dark	= 80;
		for(i = 0; i < button_height; i++)
		{
			shift = button_height+frame_width-i-1;
            gdi_color_interpolate(rect_color, white, ratio_light, &result_light);
            gdi_color_interpolate(rect_color, black, ratio_dark, &retult_dark);
            if(invert == true)
            {
                gdi_color_t tmp;
                tmp = result_light;
                result_light = retult_dark;
                retult_dark = tmp;
            }

			GDI_DRAW_H_LINE(x1+shift, x2-shift-1, y1+shift, result_light);	/* up horizontal */
			GDI_DRAW_V_LINE(y1+shift, y2-shift, x1+shift, result_light);		/* left vertical */

			GDI_DRAW_H_LINE(x1+shift, x2-shift, y2-shift, retult_dark);		/* down vertical */
			GDI_DRAW_V_LINE(y1+shift, y2-shift, x2-shift, retult_dark);		/* left vertical */
			ratio_light = ratio_light - 10;
			ratio_dark	= ratio_dark  - 10;
		}
	GDI_EXIT_CRITICAL_SECTION(gdi_draw_button_rect)
}

/**************************************************************

	FUNCTION NAME		: gdi_draw_shadow_rect()

  	PURPOSE				: draw a rect with shadow style

	INPUT PARAMETERS	: int32_t			x1
							  int32_t			y1
							  int32_t			x2
							  int32_t			y2
							  gdi_color_t rect_color
							  gdi_color_t	frame_color
							  int32_t			frame_width
							  gdi_color_t	shadow_color
							  int32_t		shadow_height

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_shadow_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t rect_color, gdi_color_t frame_color, uint16_t frame_width, gdi_color_t	shadow_color, uint16_t shadow_height)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_draw_shadow_rect)
		int32_t		 i;
		/* draw rect and frame */
		GDI_CLIP_RECT_TEST(x1,y1,x2,y2,GDI_RETURN_VOID);
		gdi_draw_frame_rect(x1, y1, x2, y2, rect_color, frame_color, frame_width);

		for( i = 0 ; i < shadow_height; i++)
		{
			GDI_DRAW_H_LINE(x1+i+2, x2+i+1, y2+i+1, shadow_color);	/* horizontal */
			GDI_DRAW_V_LINE(y1+i+2, y2+i+1, x2+i+1, shadow_color);	/* vertical */
		}
	GDI_EXIT_CRITICAL_SECTION(gdi_draw_shadow_rect)
}

/**************************************************************

	FUNCTION NAME		: gdi_draw_gradient_rect()

  	PURPOSE				: draw a rect with gradient filling

	INPUT PARAMETERS	: int32_t			x1
							  int32_t			y1
							  int32_t			x2
							  int32_t			y2
							  gdi_color_t rect_color
							  gdi_color_t	frame_color
							  int32_t			frame_width
							  gdi_gradient_rect_style_enum style

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_gradient_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t rect_color_start, gdi_color_t rect_color_end, gdi_color_t frame_color, uint16_t frame_width, gdi_gradient_rect_style_t style)
{
	GDI_ENTER_CRITICAL_SECTION(gdi_draw_gradient_rect)
		int32_t		 i;
		int32_t		 ratio;
		int32_t		 size;
		gdi_color_t result;

		GDI_CLIP_RECT_TEST(x1,y1,x2,y2,GDI_RETURN_VOID);

		switch(style)
		{
		case GDI_GRADIENT_RECT_STYLE_HOR:
			{
				size = x2 - x1;
				if(size ==0) size = 1;

				for( i = x1 ; i <= x2; i++)
				{
					ratio = (i-x1)*100/size;
					gdi_color_interpolate(rect_color_end, rect_color_start, (uint32_t)ratio, &result);	/* light */
					GDI_DRAW_V_LINE(y1, y2, i, result);	/* left vertical */
				}
				break;
			}

		case GDI_GRADIENT_RECT_STYLE_VER:
			{
				size = y2 - y1;
				if(size ==0) size = 1;

				for( i = y1 ; i <= y2; i++)
				{
					ratio = (i-y1)*100/size;
					gdi_color_interpolate(rect_color_end, rect_color_start, (uint32_t)ratio, &result);	/* light */
					GDI_DRAW_H_LINE(x1, x2, i, result);	/* left vertical */
				}
				break;
			}
    #ifdef __MORE_GRADIENT__
		case GDI_GRADIENT_RECT_DIA:
			{
				size = (x2-x1) + (y2-y1);

				for( i = y1 ; i <= y2; i++)
				{
					for(j = x1 ; j <= x2; j++)
					{
						ratio = ((i-y1)+(j-x1))*100/size;
						gdi_color_interpolate(rect_color_end, rect_color_start, (uint32_t)ratio, &result);	/* light */
						GDI_SET_BUFFER_PIXEL(j, i, result);
					}
				}
				break;
			}

		case GDI_GRADIENT_RECT_DIA_INV:
			{
				size = (x2-x1) + (y2-y1);

				for( i = x1 ; i <= x2; i++)
				{
					for(j = y1 ; j <= y2; j++)
					{
						ratio = ((i-x1)+(j-y1))*100/size;
						gdi_color_interpolate(rect_color_start, rect_color_end, (uint32_t)ratio, &result);	/* light */
						GDI_SET_BUFFER_PIXEL(i, j, result);
					}
				}
				break;
			}
		case GDI_GRADIENT_RECT_FLIP:
			{
				size = y2 - y1;
				if(size ==0) size = 1;

				for( i = y1 ; i <= y1+((y2-y1)>>1)-1; i++)
				{
					ratio = (i-y1)*2*100/size;
					if (ratio>100) ratio=100;
					gdi_color_interpolate(rect_color_start, rect_color_end, (uint32_t)ratio, &result);	/* light */
					GDI_DRAW_H_LINE(x1, x2, i, result);	/* left vertical */
				}
				for( i = y1+((y2-y1)>>1) ; i <= y2; i++)
				{
					ratio = (y2-i)*2*100/size;
					if (ratio>100) ratio=100;
					gdi_color_interpolate(rect_color_start, rect_color_end, (uint32_t)ratio, &result);	/* light */
					GDI_DRAW_H_LINE(x1, x2, i, result);	/* left vertical */
				}
				break;
			}
        #endif /* __DISABLE_SANDBOX_LIB__ */
		}
		/* draw frame */
		for( i = 0 ; i < frame_width; i++)
		{
			GDI_DRAW_H_LINE(x1+i, x2-i, y1+i, frame_color);	/* up horizontal */
			GDI_DRAW_H_LINE(x1+i, x2-i, y2-i, frame_color);	/* down horizontal */
			GDI_DRAW_V_LINE(y1+i, y2-i, x1+i, frame_color);	/* left vertical */
			GDI_DRAW_V_LINE(y1+i, y2-i, x2-i, frame_color);	/* right vertical */
		}
	GDI_EXIT_CRITICAL_SECTION(gdi_draw_gradient_rect)
}

void gdi_memset16(uint8_t* address,uint16_t pixel,int size)
{
    register int add = (int)address;
    int sizeR = size;
    int postcount;
    if(add & 0x3)
	{
		*((unsigned short*)add) = pixel;
		add += 2;
		sizeR -= 2;
	}
	postcount = sizeR & 0x3;
	sizeR -= postcount;
	if(sizeR > 0)
	{
		register unsigned int a;
		register int sizeReg = sizeR;
		register int endAdd = add + sizeReg;
		a = pixel + (pixel << 16);
		do
		{
			((unsigned int*)add)[0] = a;
			add += 4;
		}while(add < endAdd);
	}
	if(postcount != 0)
	{
		*((unsigned short*)add) = pixel;
	}
}

void gdi_memset24(uint8_t* address,uint32_t pixel,int size)
{
    register int add = (int)address;
    int sizeR = size;
    int postcount;
    unsigned int p3 = (pixel << 8) | (pixel >> 16); /* RGBR */
    unsigned int p2 = (pixel << 16) | (pixel >> 8); /* GBRG */
    unsigned int p1 = pixel | (pixel << 24); /* BRGB */
    unsigned char R = (unsigned char)(p3 & 0xFF);
    unsigned char G = (unsigned char)(p2 & 0xFF);
    unsigned char B = (unsigned char)(p1 & 0xFF);

    if (sizeR <= 2*3)
    {
        postcount = sizeR;
    }
    else
    {
        if(add & 0x1)
	    {
		    ((unsigned char*)add)[0] = B;
		    ((unsigned char*)add)[1] = G;
		    ((unsigned char*)add)[2] = R;
		    add += 3;
		    sizeR -= 3;        
	    }

        if(add & 0x2)
	    {
		    ((unsigned short*)add)[0] = (unsigned short)(p1 & 0xFFFF);
		    ((unsigned short*)add)[1] = (unsigned short)(p1 >> 16);
            ((unsigned short*)add)[2] = (unsigned short)(p2 & 0xFFFF);
		    add += 6;
		    sizeR -= 6;
	    }    
        
	    postcount = sizeR % 12;
	    sizeR -= postcount;
	    if(sizeR > 0)
	    {		
		    register int sizeReg = sizeR;
		    register int endAdd = add + sizeReg;
    		
		    do
		    {
			    ((unsigned int*)add)[0] = p1;
                ((unsigned int*)add)[1] = p2;
                ((unsigned int*)add)[2] = p3;
			    add += 12;
		    }while(add < endAdd);
	    }
    }

	if(postcount > 0)
	{
        do
		{
			((unsigned char*)add)[0] = B;
            ((unsigned char*)add)[1] = G;
            ((unsigned char*)add)[2] = R;
			add += 3;
            postcount -= 3;
		}while(postcount > 0);
	}
}

void gdi_memset32(uint8_t* address,uint32_t pixel,int size)
{
    register int add = (int)address;
    int sizeR = size;
    int postcount;
        
	postcount = sizeR & 0x3;
	sizeR -= postcount;
	if(sizeR > 0)
	{
		register unsigned int a;
		register int sizeReg = sizeR;
		register int endAdd = add + sizeReg;
		a = pixel;
		do
		{
			((unsigned int*)add)[0] = a;
			add += 4;
		}while(add < endAdd);
	}
	if(postcount != 0)
	{
		*((unsigned int*)add) = pixel;
	}
}

static const uint8_t bitsperpixels[]   = { 1, 8, 16, 24, 32, 32, 16, 8, 24 };
static const uint8_t reserve_padding[] = { 7, 0,  0,  0,  0,  0,  0, 0,  0 };
uint32_t gdi_sizeof_pixels(gdi_color_format_t cf, uint32_t width, uint32_t height)
{
    if (cf <GDI_COLOR_FORMAT_STANDARD_END)
    {
        return ((width * gdi_bits_per_pixel(cf) + reserve_padding[cf]) >> 3) * height;
    }
	
    width  = (width+7) & (~0x7);
    height = (height+7) & (~0x7); 
    
	if(cf == GDI_COLOR_FORMAT_MAINLCD)
        return ((width*GDI_MAINLCD_BIT_PER_PIXEL)>>3)*height;

	GDI_ASSERT(0);
	return 0;
}

gdi_color_format_t gdi_get_color_format(uint32_t bits)
{
    /*
     * WARNING!! This API won't return GDI_COLOR_FORMAT_32_PARGB & GDI_COLOR_FORMAT_UYVY422
     */
	if(bits == 1)
		return GDI_COLOR_FORMAT_1;
#ifdef GDI_COLORFORMAT8_PALETTE
	else if(bits == 8)
		return GDI_COLOR_FORMAT_8;
#endif /* GDI_COLORFORMAT8_PALETTE */
	else if(bits == 16)
		return GDI_COLOR_FORMAT_16;
	else if(bits == 18)
		return GDI_COLOR_FORMAT_24;
	else if(bits == 24)
		return GDI_COLOR_FORMAT_24;
	else if(bits == 32)
		return GDI_COLOR_FORMAT_32;
	else
		assert(0);
	return GDI_COLOR_FORMAT_32; // default we use the maximum data type
}

uint32_t gdi_bits_per_pixel(gdi_color_format_t cf)
{
	if(cf <GDI_COLOR_FORMAT_STANDARD_END)
		return bitsperpixels[cf];
	
	if(cf == GDI_COLOR_FORMAT_MAINLCD)
		return GDI_MAINLCD_BIT_PER_PIXEL;

	assert(0);
	return 0;
}

/**************************************************************

	GDI Software Resizer
	
***************************************************************/
#if defined(GDI_USING_PBM)
gdi_resizer_struct GDI_RESIZER;
gdi_resizer_put_func gdi_resizer_put;

/*****************************************************************************
 * FUNCTION
 *  gdi_resizer_put_null
 * DESCRIPTION
 *  dummy put pixel function
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_resizer_put_null(gdi_color_t c, bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	// keep empty
}

#if defined(GDI_USE_RGB565_TO_SHOW_BW)
/*****************************************************************************
 * FUNCTION
 *  gdi_resizer_put_mainlcd
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_resizer_put_1(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* 
     * W06.26 Fix the wrong position GDI_RESIZER.want_sx and GDI_RESIZER.want_sy
     * Use GDI_RESIZER.next_want_sx - GDI_RESIZER.want_sx_table instead of GDI_RESIZER.want_sx
     * as jumping to the next pixel
     * GDI_RESIZER.next_want_dy - 1 is the current y position
     */
	if(want_draw)
		GDI_SET_BUFFER_PIXEL_1(
			(GDI_RESIZER.want_dx1 + (int)(GDI_RESIZER.next_want_sx - GDI_RESIZER.want_sx_table) - 1), 
			(GDI_RESIZER.next_want_dy - 1),
			(c));

	// generate next_position
	if(GDI_RESIZER.next_want_sx >= GDI_RESIZER.want_sx_table_end)
	{
		GDI_RESIZER.next_want_sx = GDI_RESIZER.want_sx_table;

		//GDI_RESIZER.want_sy = (((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.y_scale_factor) >> 16);
		GDI_RESIZER.want_sy = ((((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.src_height_range << 1) +  GDI_RESIZER.dest_height_range) / (GDI_RESIZER.dest_height_range << 1));

		if(GDI_RESIZER.next_want_dy < GDI_RESIZER.want_dy1 || GDI_RESIZER.next_want_dy > GDI_RESIZER.want_dy2)
		{
			gdi_resizer_put=gdi_resizer_put_null;
			return;
		}
		GDI_RESIZER.next_want_dy += GDI_RESIZER.dir_y;
	}
	GDI_RESIZER.want_sx = *GDI_RESIZER.next_want_sx++;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_non_resizer_put_mainlcd
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_non_resizer_put_1(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	#define GDI_RESIZER_PUT_PIXEL(COLOR) GDI_SET_BUFFER_PIXEL_1((GDI_RESIZER.offset_dx + GDI_RESIZER.want_sx),(GDI_RESIZER.offset_dy + GDI_RESIZER.want_sy),COLOR)
	#define GDI_RESIZER_INCREASE_DEST				
	GDI_NON_RESIZER_PUT_X(c,want_draw);
	#undef GDI_RESIZER_PUT_PIXEL
	#undef GDI_RESIZER_INCREASE_DEST
}

#endif

/*****************************************************************************
 * FUNCTION
 *  gdi_resizer_put_8
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_resizer_put_8(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	if(want_draw)
		*((uint8_t*)GDI_RESIZER.dest) = (uint8_t)c;

	GDI_RESIZER.dest+=1;
	// generate next_position
	if(GDI_RESIZER.next_want_sx >= GDI_RESIZER.want_sx_table_end)
	{
		GDI_RESIZER.next_want_sx = GDI_RESIZER.want_sx_table;

		if(GDI_RESIZER.dir_x>0)
			GDI_RESIZER.dest = gdi_act_layer->buf_ptr + (GDI_RESIZER.want_dx1+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )*1;
		else
			GDI_RESIZER.dest = gdi_act_layer->buf_ptr + (GDI_RESIZER.want_dx2+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )*1;
		//GDI_RESIZER.want_sy = (((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.y_scale_factor) >> 16);
		GDI_RESIZER.want_sy = ((((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.src_height_range << 1) +  GDI_RESIZER.dest_height_range) / (GDI_RESIZER.dest_height_range << 1));

		if(GDI_RESIZER.next_want_dy < GDI_RESIZER.want_dy1 || GDI_RESIZER.next_want_dy > GDI_RESIZER.want_dy2)
		{
			gdi_resizer_put=gdi_resizer_put_null;
			return;
		}
		GDI_RESIZER.next_want_dy += GDI_RESIZER.dir_y;
	}
	GDI_RESIZER.want_sx = *GDI_RESIZER.next_want_sx++;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_resizer_put_16
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_resizer_put_16(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	if(want_draw)
		*((uint16_t*)GDI_RESIZER.dest) = (uint16_t)c;

	GDI_RESIZER.dest+=2;
	// generate next_position
	if(GDI_RESIZER.next_want_sx >= GDI_RESIZER.want_sx_table_end)
	{
		GDI_RESIZER.next_want_sx = GDI_RESIZER.want_sx_table;

		if(GDI_RESIZER.dir_x>0)
			GDI_RESIZER.dest = gdi_act_layer->buf_ptr + (GDI_RESIZER.want_dx1+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )*2;
		else
			GDI_RESIZER.dest = gdi_act_layer->buf_ptr + (GDI_RESIZER.want_dx2+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )*2;
		//GDI_RESIZER.want_sy = (((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.y_scale_factor) >> 16);
		GDI_RESIZER.want_sy = ((((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.src_height_range << 1) +  GDI_RESIZER.dest_height_range) / (GDI_RESIZER.dest_height_range << 1));

		if(GDI_RESIZER.next_want_dy < GDI_RESIZER.want_dy1 || GDI_RESIZER.next_want_dy > GDI_RESIZER.want_dy2)
		{
			gdi_resizer_put=gdi_resizer_put_null;
			return;
		}
		GDI_RESIZER.next_want_dy += GDI_RESIZER.dir_y;
	}
	GDI_RESIZER.want_sx = *GDI_RESIZER.next_want_sx++;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_resizer_put_24
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_resizer_put_24(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	if(want_draw)
	{
		uint32_t *ptr = (uint32_t*)(GDI_RESIZER.dest);
		switch(((uint32_t)ptr)&0x3)
		{
		case 0:
			*ptr = (*ptr&0xff000000)|(c&0x00ffffff);
			break;
		case 1:
			ptr= (uint32_t*)(((uint32_t)ptr)&(~0x3));
			*ptr = (*ptr&0x000000ff)|(c<<8);
			break;
		case 2:
			ptr= (uint32_t*)(((uint32_t)ptr)&(~0x3));
			*ptr = (*ptr&0x0000ffff)|(c<<16); ptr++;
			*ptr = (*ptr&0xffffff00)|((c&0x00ff0000)>>16);
			break;
		default:
			ptr= (uint32_t*)(((uint32_t)ptr)&(~0x3));
			*ptr = (*ptr&0x00ffffff)|(c<<24); ptr++;
			*ptr = (*ptr&0xffff0000)|((c&0x00ffff00)>>8);
			break;
		}
	}

	GDI_RESIZER.dest+=3;
	// generate next_position
	if(GDI_RESIZER.next_want_sx >= GDI_RESIZER.want_sx_table_end)
	{
		GDI_RESIZER.next_want_sx = GDI_RESIZER.want_sx_table;

		if(GDI_RESIZER.dir_x>0)
			GDI_RESIZER.dest = gdi_act_layer->buf_ptr + (GDI_RESIZER.want_dx1+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )*3;
		else
			GDI_RESIZER.dest = gdi_act_layer->buf_ptr + (GDI_RESIZER.want_dx2+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )*3;
		//GDI_RESIZER.want_sy = (((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.y_scale_factor) >> 16);
		GDI_RESIZER.want_sy = ((((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.src_height_range << 1) +  GDI_RESIZER.dest_height_range) / (GDI_RESIZER.dest_height_range << 1));

		if(GDI_RESIZER.next_want_dy < GDI_RESIZER.want_dy1 || GDI_RESIZER.next_want_dy > GDI_RESIZER.want_dy2)
		{
			gdi_resizer_put=gdi_resizer_put_null;
			return;
		}
		GDI_RESIZER.next_want_dy += GDI_RESIZER.dir_y;
	}
	GDI_RESIZER.want_sx = *GDI_RESIZER.next_want_sx++;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_resizer_put_32
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_resizer_put_32(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	if(want_draw)
	{
		 *(uint32_t*)(GDI_RESIZER.dest) = (uint32_t)c;
	}

	GDI_RESIZER.dest+=4;
	// generate next_position
	if(GDI_RESIZER.next_want_sx >= GDI_RESIZER.want_sx_table_end)
	{
		GDI_RESIZER.next_want_sx = GDI_RESIZER.want_sx_table;

		if(GDI_RESIZER.dir_x>0)
			GDI_RESIZER.dest = gdi_act_layer->buf_ptr + (GDI_RESIZER.want_dx1+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )*4;
		else
			GDI_RESIZER.dest = gdi_act_layer->buf_ptr + (GDI_RESIZER.want_dx2+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )*4;
		//GDI_RESIZER.want_sy = (((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.y_scale_factor) >> 16);
		GDI_RESIZER.want_sy = ((((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.src_height_range << 1) +  GDI_RESIZER.dest_height_range) / (GDI_RESIZER.dest_height_range << 1));

		if(GDI_RESIZER.next_want_dy < GDI_RESIZER.want_dy1 || GDI_RESIZER.next_want_dy > GDI_RESIZER.want_dy2)
		{
			gdi_resizer_put=gdi_resizer_put_null;
			return;
		}
		GDI_RESIZER.next_want_dy += GDI_RESIZER.dir_y;
	}
	GDI_RESIZER.want_sx = *GDI_RESIZER.next_want_sx++;
}

#if  GDI_MAINLCD_BIT_PER_PIXEL ==  1 || !defined(GDI_USING_LAYER)

/*****************************************************************************
 * FUNCTION
 *  gdi_resizer_put_mainlcd
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_resizer_put_mainlcd(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* 
     * W06.26 Fix the wrong position GDI_RESIZER.want_sx and GDI_RESIZER.want_sy
     * Use GDI_RESIZER.next_want_sx - GDI_RESIZER.want_sx_table instead of GDI_RESIZER.want_sx
     * as jumping to the next pixel
     * GDI_RESIZER.next_want_dy - 1 is the current y position
     */
	if(want_draw)
		DRV_MAINLCD_SET_BUFFER_PIXEL(
			(GDI_RESIZER.want_dx1 + (int)(GDI_RESIZER.next_want_sx - GDI_RESIZER.want_sx_table) - 1), 
			(GDI_RESIZER.next_want_dy - 1),
			(c));

	// generate next_position
	if(GDI_RESIZER.next_want_sx >= GDI_RESIZER.want_sx_table_end)
	{
		GDI_RESIZER.next_want_sx = GDI_RESIZER.want_sx_table;

		//GDI_RESIZER.want_sy = (((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.y_scale_factor) >> 16);
		GDI_RESIZER.want_sy = ((((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * GDI_RESIZER.src_height_range << 1) +  GDI_RESIZER.dest_height_range) / (GDI_RESIZER.dest_height_range << 1));

		if(GDI_RESIZER.next_want_dy < GDI_RESIZER.want_dy1 || GDI_RESIZER.next_want_dy > GDI_RESIZER.want_dy2)
		{
			gdi_resizer_put=gdi_resizer_put_null;
			return;
		}
		GDI_RESIZER.next_want_dy += GDI_RESIZER.dir_y;
	}
	GDI_RESIZER.want_sx = *GDI_RESIZER.next_want_sx++;
}
#elif GDI_MAINLCD_BIT_PER_PIXEL == 16
	#define gdi_resizer_put_mainlcd 	gdi_resizer_put_16
#elif GDI_MAINLCD_BIT_PER_PIXEL == 24
	#define gdi_resizer_put_mainlcd 	gdi_resizer_put_24
#elif GDI_MAINLCD_BIT_PER_PIXEL == 32
	#define gdi_resizer_put_mainlcd 	gdi_resizer_put_32
#else
	#error "Unknown MAINLCD_BIT_PER_PIXEL"
#endif

/*****************************************************************************
 * FUNCTION
 *  gdi_non_resizer_put_8
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_non_resizer_put_8(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	#define GDI_RESIZER_PUT_PIXEL(COLOR)			*((uint8_t*)GDI_RESIZER.dest) = (uint8_t)COLOR
	#define GDI_RESIZER_INCREASE_DEST				GDI_RESIZER.dest ++
	GDI_NON_RESIZER_PUT_X(c,want_draw);
	#undef GDI_RESIZER_PUT_PIXEL
	#undef GDI_RESIZER_INCREASE_DEST
}

/*****************************************************************************
 * FUNCTION
 *  gdi_non_resizer_put_16
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_non_resizer_put_16(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	#define GDI_RESIZER_PUT_PIXEL(COLOR)		*((uint16_t*)GDI_RESIZER.dest) = (uint16_t)COLOR
	#define GDI_RESIZER_INCREASE_DEST			GDI_RESIZER.dest +=2
	GDI_NON_RESIZER_PUT_X(c,want_draw);
	#undef GDI_RESIZER_PUT_PIXEL
	#undef GDI_RESIZER_INCREASE_DEST
}

/*****************************************************************************
 * FUNCTION
 *  gdi_non_resizer_put_24
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_non_resizer_put_24(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	#define GDI_RESIZER_PUT_PIXEL(COLOR)			\
			do\
			{\
				uint32_t *ptr = (uint32_t*)(GDI_RESIZER.dest);\
				switch(((uint32_t)ptr)&0x3)\
				{\
				case 0:\
					*ptr = (*ptr&0xff000000)|(COLOR&0x00ffffff);\
					break;\
				case 1:\
					ptr= (uint32_t*)(((uint32_t)ptr)&(~0x3));\
					*ptr = (*ptr&0x000000ff)|(COLOR<<8);\
					break;\
				case 2:\
					ptr= (uint32_t*)(((uint32_t)ptr)&(~0x3));\
					*ptr = (*ptr&0x0000ffff)|(COLOR<<16); ptr++;\
					*ptr = (*ptr&0xffffff00)|((COLOR&0x00ff0000)>>16);\
					break;\
				default:\
					ptr= (uint32_t*)(((uint32_t)ptr)&(~0x3));\
					*ptr = (*ptr&0x00ffffff)|(COLOR<<24); ptr++;\
					*ptr = (*ptr&0xffff0000)|((COLOR&0x00ffff00)>>8);\
				}\
			} while(0)

	#define GDI_RESIZER_INCREASE_DEST				GDI_RESIZER.dest +=3
	GDI_NON_RESIZER_PUT_X(c,want_draw);
	#undef GDI_RESIZER_PUT_PIXEL
	#undef GDI_RESIZER_INCREASE_DEST
}

/*****************************************************************************
 * FUNCTION
 *  gdi_non_resizer_put_32
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_non_resizer_put_32(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	#define GDI_RESIZER_PUT_PIXEL(COLOR)			*((uint32_t*)GDI_RESIZER.dest) = (uint32_t)COLOR
	#define GDI_RESIZER_INCREASE_DEST				GDI_RESIZER.dest +=4
	GDI_NON_RESIZER_PUT_X(c,want_draw);
	#undef GDI_RESIZER_PUT_PIXEL
	#undef GDI_RESIZER_INCREASE_DEST
}

#if  GDI_MAINLCD_BIT_PER_PIXEL ==  1 || !defined(GDI_USING_LAYER)
/*****************************************************************************
 * FUNCTION
 *  gdi_non_resizer_put_mainlcd
 * DESCRIPTION
 *  put pixel
 * PARAMETERS
 *  c               [IN]    color want to output
 *  want_draw       [IN]    if false, it will not modify color of this pixel
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_non_resizer_put_mainlcd(gdi_color_t c,bool want_draw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	#define GDI_RESIZER_PUT_PIXEL(COLOR)			DRV_MAINLCD_SET_BUFFER_PIXEL((GDI_RESIZER.offset_dx + GDI_RESIZER.want_sx),(GDI_RESIZER.offset_dy + GDI_RESIZER.want_sy),COLOR)
	#define GDI_RESIZER_INCREASE_DEST				
	GDI_NON_RESIZER_PUT_X(c,want_draw);
	#undef GDI_RESIZER_PUT_PIXEL
	#undef GDI_RESIZER_INCREASE_DEST
}
#elif GDI_MAINLCD_BIT_PER_PIXEL == 16
	#define gdi_non_resizer_put_mainlcd 	gdi_non_resizer_put_16
#elif GDI_MAINLCD_BIT_PER_PIXEL == 24
	#define gdi_non_resizer_put_mainlcd 	gdi_non_resizer_put_24
#elif GDI_MAINLCD_BIT_PER_PIXEL == 32
	#define gdi_non_resizer_put_mainlcd 	gdi_non_resizer_put_32
#else
	#error "Unknown MAINLCD_BIT_PER_PIXEL"
#endif

/*****************************************************************************
 * FUNCTION
 *  gdi_resizer_set_want_sy
 * DESCRIPTION
 *  update resizer context depends on want_sy
 * PARAMETERS
 *  want_sy       [IN]  the next line resizer want to draw
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_resizer_set_want_sy(int32_t want_sy)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	int32_t dy;

    // out of clipping should not update want_sx, want_sy
    if( GDI_RESIZER.want_sx <0 || GDI_RESIZER.want_sy < 0) return;
    
	// sometime we need to stop auto increase dy, and manual set the position
	GDI_RESIZER.dir_y = 0; 
	// default : the pixel line (want_sy) will not be accept !!
	GDI_RESIZER.want_sy = want_sy+1; 

	// non resize handler
	if(GDI_RESIZER.want_sx_table==NULL)
	{
		if ((want_sy) >= GDI_RESIZER.want_start_sy && (want_sy) <= GDI_RESIZER.want_end_sy)
		{
			GDI_RESIZER.want_sx = GDI_RESIZER.want_start_sx;
			GDI_RESIZER.want_sy = want_sy;
			//W05.40 Compute GDI_RESIZER.dest by GDI_RESIZER.want_dx1 and GDI_RESIZER.want_dy1
			//GDI_RESIZER.dest = gdi_act_layer->buf_ptr + ((GDI_RESIZER.want_sx+GDI_RESIZER.want_sy*gdi_act_layer->width* bitsperpixels[gdi_act_layer->vcf])>>3);
			GDI_RESIZER.dest = gdi_act_layer->buf_ptr 
								+ (((GDI_RESIZER.want_dx1
								+(GDI_RESIZER.want_dy1+GDI_RESIZER.want_sy-GDI_RESIZER.want_start_sy)*gdi_act_layer->width)
								* gdi_bits_per_pixel(gdi_act_layer->cf))>>3);
		}
		return ;
	}

	for(;;)
	{
		// want_sy project to dy
		dy = ((want_sy * GDI_RESIZER.dest_height_range << 1) +  GDI_RESIZER.src_height_range) / (GDI_RESIZER.src_height_range << 1);
		
		//check this want_dy will also project to want_sy
		if( want_sy ==  ((((dy) * GDI_RESIZER.src_height_range << 1) +  GDI_RESIZER.dest_height_range) / (GDI_RESIZER.dest_height_range << 1)))
			break;

		dy ++;
		// check dy + 1 will project to want_sy
		if( want_sy == ((((dy) * GDI_RESIZER.src_height_range << 1) +  GDI_RESIZER.dest_height_range) / (GDI_RESIZER.dest_height_range << 1)))
			break;

		
		return;// this line will not be accept !!
	}

	dy += GDI_RESIZER.offset_dy;

	// check this destination is out of clipping area ?
	if(dy < GDI_RESIZER.want_dy1 || dy > GDI_RESIZER.want_dy2)
		return;// this line will not be accept !!

	GDI_RESIZER.want_sy = want_sy;
	GDI_RESIZER.next_want_dy = dy;
	GDI_RESIZER.next_want_sx = GDI_RESIZER.want_sx_table;
	GDI_RESIZER.want_sx = *GDI_RESIZER.next_want_sx++;
	
	if(GDI_RESIZER.dir_x>0)
		GDI_RESIZER.dest = gdi_act_layer->buf_ptr 
							+ ((	(	GDI_RESIZER.want_dx1
									+GDI_RESIZER.next_want_dy* gdi_act_layer->width )
									* gdi_bits_per_pixel(gdi_act_layer->cf))>>3);
	else
		GDI_RESIZER.dest = gdi_act_layer->buf_ptr 
							+ ((		(GDI_RESIZER.want_dx2
									+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )
									* gdi_bits_per_pixel(gdi_act_layer->cf))>>3);
	
}
#endif

#if defined(GDI_USING_PBM)
typedef void (*gdi_resizer_put_func)(gdi_color_t c,bool want_draw);
const static gdi_resizer_put_func gdi_resizer_put_table[]=
{
#if defined(GDI_USE_RGB565_TO_SHOW_BW)
	gdi_resizer_put_1, //(gdi_resizer_put_func)gd_null_pointer_function, // 1
#else
    (gdi_resizer_put_func)gd_null_pointer_function, // 1
#endif
	gdi_resizer_put_8, // 8
	gdi_resizer_put_16, // 16
	gdi_resizer_put_24, // 24
	gdi_resizer_put_32, // 32
	gdi_resizer_put_32, // 32PARGB
	(gdi_resizer_put_func)gd_null_pointer_function, // UYVY422
	gdi_resizer_put_8,  /* A8 */
	gdi_resizer_put_24, /* PARGB6666 */
	gdi_resizer_put_mainlcd, // main
	(gdi_resizer_put_func)gd_null_pointer_function, // sub
};
const static gdi_resizer_put_func gdi_non_resizer_put_table[]=
{
#if defined(GDI_USE_RGB565_TO_SHOW_BW)
    gdi_non_resizer_put_1,
#else
	(gdi_resizer_put_func)gd_null_pointer_function, // 1
#endif
	gdi_non_resizer_put_8, // 8
	gdi_non_resizer_put_16, // 16
	gdi_non_resizer_put_24, // 24
	gdi_non_resizer_put_32, // 32
	gdi_non_resizer_put_32, // 32PARGB
	(gdi_resizer_put_func)gd_null_pointer_function, // UYVY422
	gdi_non_resizer_put_8,  /* A8 */
	gdi_non_resizer_put_24,  /* PARGB6666 */
	gdi_non_resizer_put_mainlcd, // main
	(gdi_resizer_put_func)gd_null_pointer_function // sub
};

/*****************************************************************************
 * FUNCTION
 *  gdi_resizer_init
 * DESCRIPTION
 *  init the resizer
 * PARAMETERS
 *  src_width              [IN] srouce width
 *  src_height             [IN] source height
 *  src_clipx1             [IN] source clip x1
 *  src_clipy1             [IN] source clip y1
 *  src_clipx2             [IN] source clip x2
 *  src_clipy2             [IN] source clip y2
 *  dest_x1                [IN] destination x1
 *  dest_y1                [IN] destination y1
 *  dest_x2                [IN] destination x2
 *  dest_y2                [IN] destination y2
 * RETURNS
 *  int32_t, return 0 if no pixel to output, 1 if decoder need to continue to decode, -1 if error.
 *****************************************************************************/
int32_t gdi_resizer_init(
				int32_t src_width,int32_t src_height, 
				int32_t src_clipx1, int32_t src_clipy1,int32_t src_clipx2,int32_t src_clipy2,
				int32_t dest_x1,int32_t dest_y1,int32_t dest_x2,int32_t dest_y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t dest_width;
	int32_t dest_height;
	int32_t dir_x;
	int32_t dir_y;
	int32_t src_width_range;
	int32_t src_height_range;
	int32_t dest_width_range;
	int32_t dest_height_range;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_RESIZER.want_sx = GDI_RESIZER.want_sy = -1;
	
	// we don't support 1, 8 bites color format in this resizer
	if(src_width  <=0) return 0;
	if(src_height <=0) return 0;

	dir_x=dir_y=1;

	if(dest_x1>dest_x2)
	{
		GDI_DEBUG_ASSERT(0); // current, we only support dir_x >0
		SWAP(int32_t,dest_x1,dest_x2);
		dir_x = -1;
	}
	if(dest_y1>dest_y2)
	{
		SWAP(int32_t,dest_y1,dest_y2);
		dir_y = -1;
	}

	GDI_RESIZER.dest = gdi_act_layer->buf_ptr;
	GDI_RESIZER.offset_dx = dest_x1;
	GDI_RESIZER.offset_dy = dest_y1;
	dest_width = dest_x2-dest_x1+1;
	dest_height = dest_y2-dest_y1+1;
	
    gdi_image_set_update_area(dest_x1,dest_y1,dest_x2,dest_y2);
	
	GDI_RESIZER.want_dx1 = dest_x1;
	GDI_RESIZER.want_dy1 = dest_y1;
	GDI_RESIZER.want_dx2 = dest_x2;
	GDI_RESIZER.want_dy2 = dest_y2;

	GDI_RESIZER.dir_x = dir_x;
	GDI_RESIZER.dir_y = dir_y;
	
	// check the clipping area
	GDI_CLIP_RECT_TEST(GDI_RESIZER.want_dx1,GDI_RESIZER.want_dy1,GDI_RESIZER.want_dx2,GDI_RESIZER.want_dy2,return 0); // no pixel will be render !!

	/* W06.16 Use Coordinate Range instead of width and height */
	/* Use round off to compute want_sx and want_sy */
	if (1 == src_width)
	{
		src_width = 2;
	}
	if (1 == dest_width)
	{
		dest_width = 2;
	}
	if (1 == src_height)
	{
		src_height = 2;
	}
	if (1 == dest_height)
	{
		dest_height = 2;
	}
	src_width_range = src_width - 1;
	src_height_range = src_height - 1;
	dest_width_range  = dest_width - 1;
	dest_height_range  = dest_height - 1;
	//multiply_factor = ((src_width_range) <<16)/ (dest_width_range);
	// make y_scale_factor
	//GDI_RESIZER.y_scale_factor 		= ((src_height_range)<<16)  / (dest_height_range);
	//GDI_RESIZER.y_inv_scale_factory	= ((dest_height_range)<<16 )  / (src_height_range);
	GDI_RESIZER.src_height_range 		= src_height_range;
	GDI_RESIZER.dest_height_range	= dest_height_range;

	{
		int dx1,dy1,dx2,dy2;
		// check the source clipping area
		dx1 = dest_x1 + (src_clipx1*(dest_width_range<<1) + src_width_range)/(src_width_range<<1);
		dx2 = dest_x1 + (src_clipx2*(dest_width_range<<1) + src_width_range)/(src_width_range<<1);		
		
		dy1 = dest_y1 + (src_clipy1*(dest_height_range<<1) + src_height_range)/(src_height_range<<1);
		dy2 = dest_y1 + (src_clipy2*(dest_height_range)<<1) + src_height_range/(src_height_range<<1);		

		GDI_CLIP_TWO_RECT(
			GDI_RESIZER.want_dx1, GDI_RESIZER.want_dy1,
			GDI_RESIZER.want_dx2, GDI_RESIZER.want_dy2,
			dx1,dy1,
			dx2,dy2,
			return 0);

		//W05.39 Check points may be out of clipping area because of dividing
		while ( (((GDI_RESIZER.want_dx1 - GDI_RESIZER.offset_dx) * src_width_range << 1) + dest_width_range) / (dest_width_range << 1) < src_clipx1 )
			GDI_RESIZER.want_dx1++;

		while((((GDI_RESIZER.want_dy1 - GDI_RESIZER.offset_dy) * src_height_range << 1) + dest_height_range) / (dest_height_range << 1) < src_clipy1)
			GDI_RESIZER.want_dy1++;

		while ( (((GDI_RESIZER.want_dx2 - GDI_RESIZER.offset_dx) * src_width_range << 1) + dest_width_range) / (dest_width_range << 1) > src_clipx2 )
			GDI_RESIZER.want_dx2--;

		while((((GDI_RESIZER.want_dy2 - GDI_RESIZER.offset_dy) * src_height_range << 1) + dest_height_range) / (dest_height_range << 1)> src_clipy2)
			GDI_RESIZER.want_dy2--;
	}

	// non resize handler
	if(src_width == dest_width && src_height == dest_height)
	{
		uint32_t bytes_per_pixel = gdi_bits_per_pixel(gdi_act_layer->cf) >> 3;
		GDI_RESIZER.want_sx_table = NULL;
		GDI_RESIZER.dest = gdi_act_layer->buf_ptr;
		if(dir_x >0)
		{
			GDI_RESIZER.want_sx 		=
			GDI_RESIZER.want_start_sx = GDI_RESIZER.want_dx1 - dest_x1;
			GDI_RESIZER.want_end_sx 	= GDI_RESIZER.want_dx2 - dest_x1;
			
			GDI_RESIZER.dest 		+= GDI_RESIZER.want_dx1*bytes_per_pixel;
		}
		else
		{
			GDI_RESIZER.want_sx 		=
			GDI_RESIZER.want_start_sx = GDI_RESIZER.want_dx2 - dest_x1;
			GDI_RESIZER.want_end_sx  = GDI_RESIZER.want_dx1 - dest_x1;
			GDI_RESIZER.dest 		+= GDI_RESIZER.want_dx2*bytes_per_pixel;			
		}
		if(dir_y >0)
		{
			GDI_RESIZER.want_sy 		=
			GDI_RESIZER.want_start_sy = GDI_RESIZER.want_dy1 - dest_y1;
			GDI_RESIZER.want_end_sy 	= GDI_RESIZER.want_dy2 - dest_y1;
			
			GDI_RESIZER.dest 		+= GDI_RESIZER.want_dy1*gdi_act_layer->width*bytes_per_pixel;
			GDI_RESIZER.dest_pitch_jump = (gdi_act_layer->width - (GDI_RESIZER.want_dx2-GDI_RESIZER.want_dx1+1)) * bytes_per_pixel; ///TODO: only support dir_x>0
		}
		else
		{
			GDI_RESIZER.want_sy 		=
			GDI_RESIZER.want_start_sy = GDI_RESIZER.want_dy2 - dest_y1;
			GDI_RESIZER.want_end_sy  = GDI_RESIZER.want_dy1 - dest_y1;
			
			GDI_RESIZER.dest 		+= GDI_RESIZER.want_dy2*gdi_act_layer->width*bytes_per_pixel;
			GDI_RESIZER.dest_pitch_jump = -(gdi_act_layer->width + (GDI_RESIZER.want_dx2-GDI_RESIZER.want_dx1+1)) * bytes_per_pixel; ///TODO: only support dir_x>0
		}
		gdi_resizer_put = gdi_non_resizer_put_table[gdi_act_layer->cf];

		return 1;		
	}

	// return error when want_sx_table memory is not enough
	if ( GDI_RESIZER.want_dx2-GDI_RESIZER.want_dx1+1 > GDI_MAX_RESIZE_WIDTH)
    {
        return -1;
    }
    GDI_RESIZER.want_sx_table = (S16*)gdi_alloc_ext_mem(GDI_MAX_RESIZE_WIDTH * sizeof(S16));
    if (GDI_RESIZER.want_sx_table == NULL)
    {
        return 0;
    }

	// make want_x table
	{
		int32_t x;
		int32_t i;
		int32_t start_dx,end_dx;

		if(dir_x>0)
		{
			start_dx = GDI_RESIZER.want_dx1;
			end_dx = GDI_RESIZER.want_dx2;
		}
		else
		{
			start_dx = GDI_RESIZER.want_dx2;
			end_dx = GDI_RESIZER.want_dx1;
		}
			
		for(i=0,x=start_dx;x<=end_dx;x+=dir_x,i++)
		{
			int32_t d = x - dest_x1;
			GDI_RESIZER.want_sx_table[i] = (S16)((((d) * src_width_range << 1) + dest_width_range) / (dest_width_range << 1));
		}
		GDI_RESIZER.want_sx_table_end = GDI_RESIZER.want_sx_table+i;
		GDI_RESIZER.next_want_sx = GDI_RESIZER.want_sx_table;
	}

	if(dir_y>0)
		GDI_RESIZER.next_want_dy = GDI_RESIZER.want_dy1;
	else
		GDI_RESIZER.next_want_dy = GDI_RESIZER.want_dy2;

	// set the put pixel function pointer
	gdi_resizer_put = gdi_resizer_put_table[gdi_act_layer->cf];
	// init want position
	if(dir_x>0)
		GDI_RESIZER.dest = gdi_act_layer->buf_ptr + (((GDI_RESIZER.want_dx1+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )* bitsperpixels[gdi_act_layer->vcf])>>3);
	else
		GDI_RESIZER.dest = gdi_act_layer->buf_ptr + (((GDI_RESIZER.want_dx2+ GDI_RESIZER.next_want_dy* gdi_act_layer->width )* bitsperpixels[gdi_act_layer->vcf])>>3);
	
	GDI_RESIZER.want_sx = *GDI_RESIZER.next_want_sx++;
	GDI_RESIZER.want_sy = ((((GDI_RESIZER.next_want_dy  - GDI_RESIZER.offset_dy) * src_height_range << 1) +  dest_height_range) / (dest_height_range << 1));
	GDI_RESIZER.next_want_dy +=dir_y;
	return 1;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_image_resizer_deinit
 * DESCRIPTION
 *  Deinit resizer.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_resizer_deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    // TODO: use memory allocator
    if (GDI_RESIZER.want_sx_table)
    {
        gdi_free_ext_mem((void*)&GDI_RESIZER.want_sx_table);
        GDI_RESIZER.want_sx_table = NULL;
    }
}
#endif

/*****************************************************************************
 * FUNCTION
 *  gdi_fill_dot_rect
 * DESCRIPTION
 *  
 * PARAMETERS
 * RETURNS
 *  
 *****************************************************************************/
void gdi_fill_dot_rect(int32_t x1,int32_t y1,int32_t x2,int32_t y2,gdi_color_t c)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_fill_dot_rect)
    {
        gd_fill_dot_rect[gdi_act_layer->cf](
                                            gdi_act_layer->buf_ptr,
                                            gdi_act_layer->width,
                                            gdi_act_layer->height,
                                            x1,y1,
                                            x2,y2,
                                            gdi_act_layer->clipx1,
                                            gdi_act_layer->clipy1,
                                            gdi_act_layer->clipx2,
                                            gdi_act_layer->clipy2,
                                            c);
    }
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_fill_dot_rect)
}

#define R_OF_RGB565(p)      ((p) >> 11)
#define G_OF_RGB565(p)      (((p) << 21) >> 26)
#define B_OF_RGB565(p)      (((p) << 27) >> 27)

/*****************************************************************************
 * FUNCTION
 *  gdi_rgb565_to_rgb888
 * DESCRIPTION
 *  convert RGB565 to RGB888
 * PARAMETERS
 *  p       [IN]    RGB565 pixel
 * RETURNS
 *  24-bit RGB888 pixel
 *****************************************************************************/
uint32_t gdi_rgb565_to_rgb888(uint32_t p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/   
    uint32_t     result;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* extend p's B[4:0] to result B[7:0] */
    result = B_OF_RGB565(p) << 3;

    /* extend p's G[10:5] to result G[15:8] */
    result += G_OF_RGB565(p) << (8 + 2);

    /* extend p's R[15:11] to result B[23:16] */
    result += R_OF_RGB565(p) << (16 + 3);

    return result;
}

void gdi_draw_circle(int32_t x, int32_t y, int32_t r, gdi_color_t circle_color)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_draw_circle)
    {
        float delta;
        int32_t m, n;

        for (delta = 5.0 / 4 - r, m = 0, n = r; m <= n; m ++)
        {
            gdi_draw_point(x + m, y + n, circle_color);
            gdi_draw_point(x + m, y - n, circle_color);
            gdi_draw_point(x - m, y + n, circle_color);
            gdi_draw_point(x - m, y - n, circle_color);
            gdi_draw_point(x + n, y + m, circle_color);
            gdi_draw_point(x + n, y - m, circle_color);
            gdi_draw_point(x - n, y + m, circle_color);
            gdi_draw_point(x - n, y - m, circle_color);

            if (delta >= 0)
            {
                delta += (float)(2.0 * (m - n) + 5);
                n --;
            }
            else
            {
                delta += (float)(m * 2.0 + 3);
            }
        }
    }
    GDI_EXIT_CRITICAL_SECTION(gdi_draw_circle)
}

void gdi_draw_filled_circle(int32_t x, int32_t y, int32_t r, gdi_color_t circle_color)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_draw_filled_circle)
    {
        float delta;
        int32_t m, n;

        for (delta = 5.0 / 4 - r, m = 0, n = r; m <= n; m ++)
        {
            gdi_draw_line(x + m, y + n, x + m, y - n, circle_color);
            gdi_draw_line(x - m, y + n, x - m, y - n, circle_color);
            gdi_draw_line(x + n, y + m, x + n, y - m, circle_color);
            gdi_draw_line(x - n, y + m, x - n, y - m, circle_color);

            if (delta >= 0)
            {
                delta += (float)(2.0 * (m - n) + 5);
                n --;
            }
            else
            {
                delta += (float)(m * 2.0 + 3);
            }
        }
    }
    GDI_EXIT_CRITICAL_SECTION(gdi_draw_filled_circle)
}

void gdi_draw_arc(int32_t circle_x, int32_t circle_y, int32_t circle_r, int startAngle, int angleExtent, gdi_color_t color)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_draw_arc)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /* 
     * Bresenham Algorithm: Draw 1/8 circle
     */
    int x, y, d;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    x = 0;
    y = circle_r;
    d = 1 - circle_r;

    while (x < y)
    {
        if ((startAngle >= 0 || startAngle <= 45 / 2) && (angleExtent > 45 / 2))
        {   /* first 1/8 circle, angle:(0~45), coord.:(x,y) */
            gdi_draw_point(circle_x + x, circle_y + y, color);
        }
        if ((startAngle > 45 - 45 / 2 || startAngle <= 45 + 45 / 2) && (angleExtent > 90 - 45 / 2))
        {   /* second 1/8 circle, angle:(45~90), coord.:(y,x) */
            gdi_draw_point(circle_x + y, circle_y + x, color);
        }
        if ((startAngle > 90 - 45 / 2 || startAngle <= 90 + 45 / 2) && (angleExtent > 135 - 45 / 2))
        {   /* third 1/8 circle, angle:(90~135), coord.:(y,-x) */
            gdi_draw_point(circle_x + y, circle_y - x, color);
        }
        if ((startAngle > 135 - 45 / 2 || startAngle <= 135 + 45 / 2) && (angleExtent > 180 - 45 / 2))
        {   /* fourth 1/8 circle, angle:(135~180), coord.:(x,-y) */
            gdi_draw_point(circle_x + x, circle_y - y, color);
        }
        if ((startAngle > 180 - 45 / 2 || startAngle <= 180 + 45 / 2) && (angleExtent > 225 - 45 / 2))
        {   /* fifth 1/8 circle, angle:(180~225), coord.:(-x,-y) */
            gdi_draw_point(circle_x - x, circle_y - y, color);
        }
        if ((startAngle > 225 - 45 / 2 || startAngle <= 225 + 45 / 2) && (angleExtent > 270 - 45 / 2))
        {   /* sixth 1/8 circle, angle:(225~270), coord.:(-y,-x) */
            gdi_draw_point(circle_x - y, circle_y - x, color);
        }
        if ((startAngle > 270 - 45 / 2 || startAngle <= 270 + 45 / 2) && (angleExtent > 315 - 45 / 2))
        {   /* fourth 1/8 circle, angle:(270~315), coord.:(-y,x) */
            gdi_draw_point(circle_x - y, circle_y + x, color);
        }
        if ((startAngle > 315 - 45 / 2 || startAngle <= 315 + 45 / 2) && (angleExtent > 360 - 45 / 2))
        {   /* fourth 1/8 circle, angle:(315~360), coord.:(-x,y) */
            gdi_draw_point(circle_x - x, circle_y + y, color);
        }

        if (d > 0)
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        else
        {
            d += 2 * x + 3;
        }
        x++;
    }
    GDI_EXIT_CRITICAL_SECTION(gdi_draw_arc)
}

void gdi_draw_solid_arc(int32_t circle_x, int32_t circle_y, int32_t circle_r, int startAngle, int angleExtent, gdi_color_t color)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_draw_solid_arc)
        
    gdi_draw_arc(circle_x, circle_y, circle_r, startAngle, angleExtent, color);
    circle_r--;
    while (circle_r > 0)
    {
        gdi_draw_arc(circle_x, circle_y, circle_r, startAngle, angleExtent, color);
        gdi_draw_point(circle_x + circle_r, circle_y + circle_r, color);
        gdi_draw_point(circle_x + circle_r, circle_y - circle_r, color);
        gdi_draw_point(circle_x - circle_r, circle_y - circle_r, color);
        gdi_draw_point(circle_x - circle_r, circle_y + circle_r, color);
        circle_r--;
    }
    gdi_draw_point(circle_x, circle_y, color);
    
    GDI_EXIT_CRITICAL_SECTION(gdi_draw_solid_arc)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_draw_antialiasing_line
 * DESCRIPTION
 *  draw a anti-aliasing line
 * PARAMETERS
 *  x1          [IN]    the start point x1
 *  y1          [IN]    the start point y1
 *  x2          [IN]    the end point x2
 *  y2          [IN]    the end point y2
 *  line_color  [IN]    the color of line
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_draw_antialiasing_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t line_color)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_draw_antialiasing_line)
    const uint32_t gaussian_integral[21] =   {   32767,      26958,      21431,      16431,      12127,
                                            8603,       5859,       3825,       2393,       1432,
                                            820,        449,        234,        117,         55,
                                            25,         11,          4,          1,          0,
                                            0};
/*        __|__                         */
/*      _/  |  \_                       */
/*     /    |    \                      */
/* ___/     |     \___                  */
/* --------------------------->x-axis   */
/*          0    1     2    3           */
/* the gaussian_integral[x*10] is the integral of gaussian from x to infinity and shift left 16 */

    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool steep;                                             /* y-scan or x-scan */
    int32_t slope, dx, dy, d, incrE, incrNE, x, y, two_v_dx;    /* slope is the direction that y goes */
    uint8_t lineA, lineR, lineG, lineB;                         /* color of line */
    double sqrt_result;                                     /* used to store result of sqrt() */
    int32_t invDenom_i, two_dx_invDemon_i;                      /* used to compute distance between pixel and line */
    int32_t tmp;                                                /* used in cliping */
    gdi_layer_handle_t abm_src_layer;                               /* abm source layer */
    bool source_key_enable;                                 /* if the source layer enable the source key */
    gdi_color_t source_key_value;                             /* the source key value of abm source layer */
    int32_t clipx1, clipx2, clipy1, clipy2;                     /* the clip area */
    uint8_t* src_layer_buf_ptr = NULL;                           /* buf_ptr of abm_src_layer*/
    int32_t src_layer_layer_width = 0;                          /* layer_width of abm_src_layer*/
    gd_get_buf_pixel_func src_layer_get_pixel = NULL;       /* get_buf_pixel() function pointer of abm_src_layer*/
    gd_color_to_rgb_func src_layer_color_to_rgb = NULL;     /* color_to_rgb() functio pointer of abm_src_layer*/
    
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ASSERT((GDI_LAYER.clipy1 <= GDI_LAYER.clipy2) && (GDI_LAYER.clipx1 <= GDI_LAYER.clipx2));
    
    if ((y1 == y2) && (y1 >= GDI_LAYER.clipy1) && (y1 <= GDI_LAYER.clipy2)) // Horizontal Line /
	{
		if(x1 > x2)     // exchange x /
		{
		    tmp = x1;
            x1 = x2;
            x2 = tmp;
		}
        if((x1 <= GDI_LAYER.clipx2) && (x2 >= GDI_LAYER.clipx1))
        {
            GDI_CLIP_LINE_X_TEST(x1,x2);
            GDI_DRAW_H_LINE(x1, x2, y1, line_color);
    	}
	}
	else if ((x1 == x2) && (x1 >= GDI_LAYER.clipx1) && (x1 <= GDI_LAYER.clipx2)) // Vertical Line /
    {
		if(y1 > y2)     // exchange y /
		{
			tmp = y1;
            y1 = y2;
            y2 = tmp;
		}
        if((y1 <= GDI_LAYER.clipy2) && (y2 >= GDI_LAYER.clipy1))
        {
            GDI_CLIP_LINE_Y_TEST(y1,y2);
            GDI_DRAW_V_LINE(y1, y2, x1, line_color);
	    }
	}
    else
    {
        /* get abm source layer */
        gdi_layer_get_source_key(&source_key_enable, &source_key_value);
        gdi_get_alpha_blending_source_layer(&abm_src_layer);
        if (abm_src_layer != GDI_ERROR_HANDLE)
        {   
            gdi_layer_struct_t *tmpstruc = (gdi_layer_struct_t*)abm_src_layer;
            src_layer_buf_ptr = tmpstruc->buf_ptr;
            src_layer_layer_width = tmpstruc->width;
            src_layer_get_pixel = gd_get_buf_pixel[tmpstruc->cf];
            src_layer_color_to_rgb = gd_color_to_rgb[tmpstruc->cf];
        }
        
        /* clip */
        gdi_layer_get_clip(&clipx1, &clipy1, &clipx2, &clipy2);

        if (x1 > x2)    
        {
            /* exchange two points: (x1,y1) and (x2,y2)*/
            tmp = x1;
            x1 = x2;
            x2 = tmp;

            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        dx = x2 - x1;
        dy = y2 - y1;
        
        /* if the slope>1, need to exchange x-axis and y-axis */
        steep = (abs(y2 - y1) > abs(x2 - x1));  
        if (steep)
        {
            /* exchange x1 and y1, x2 and y2 */
            tmp = x1;
            x1 = y1;
            y1 = tmp;

            tmp = x2;
            x2= y2;
            y2 = tmp;
        }
        
        if (x1 > x2)    
        {
            /* exchange two points: (x1,y1) and (x2,y2) */
            tmp = x1;
            x1 = x2;
            x2 = tmp;

            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        
        dx = x2 - x1;
        dy = y2 - y1;
        
        if (dy < 0)
        {
            slope = -1;
            dy = -dy;
        }
        else
        {
            slope = 1;
        }

        /* variables for mid-point draw line algorithm */
        d = (2 * dy) - dx;
        incrE = 2 * dy;
        incrNE = 2 * (dy - dx);

        /* variables for anti-aliasing */
        two_v_dx = 0;
        sqrt_result = sqrt((double)(dx * dx + dy * dy));
        invDenom_i = (int32_t)((1.0f / (2.0f * (float)sqrt_result)) * (1 << AA_LINE_PRECISION_BITS)); /* shift AA_LINE_PRECISION_BITS to make float to int32_t */
        two_dx_invDemon_i = 2 * dx * invDenom_i;

        /* get line color rgb */
        gdi_act_color_to_rgb(&lineA, &lineR, &lineG, &lineB, line_color);

        for (x = x1, y = y1; x <= x2; x++)
        {
            int32_t slope1, slope2, outx, outy, clip1, clip2;
            if (steep)
            {
                outx = y;
                outy = x;
                slope1 = slope;
                slope2 = 0;
                clip1 = clipx1;
                clip2 = clipx2;
            }
            else
            {
                outx = x;
                outy = y;
                slope1 = 0;
                slope2 = slope;
                clip1 = clipy1;
                clip2 = clipy2;
            }
            do
            {
                if ((outx < clipx1) || (outx > clipx2) ||
            	    (outy < clipy1) || (outy > clipy2))
            	{
            	    break;	
            	}
                ANTIALIASING_LINE_PUT_PIXEL(outx, outy, lineR, lineG, lineB, two_v_dx * invDenom_i);
                tmp = y + slope;
                if (tmp >= clip1 && tmp <= clip2)
                {
                    ANTIALIASING_LINE_PUT_PIXEL(outx + slope1, outy + slope2, lineR, lineG, lineB, two_dx_invDemon_i - two_v_dx * invDenom_i);
                }
                tmp = y - slope;
                if (tmp >= clip1 && tmp <= clip2)
                {
                    ANTIALIASING_LINE_PUT_PIXEL(outx - slope1, outy - slope2, lineR, lineG, lineB, two_dx_invDemon_i + two_v_dx * invDenom_i);
                }
            }while(0);
            if (d < 0)
            {
                two_v_dx = d + dx;
                d += incrE;
            }
            else
            {
                two_v_dx = d - dx;
                d += incrNE;
                y += slope;
            }
        }
    }
 
    GDI_EXIT_CRITICAL_SECTION(gdi_draw_antialiasing_line)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_draw_polygon
 * DESCRIPTION
 *  draw a polygon
 * PARAMETERS
 *  point_array [IN]    point array
 *  point_num   [IN]    the number of elements of point array
 *  color       [IN]    the color of polygon
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_draw_polygon(gdi_point_t point[], uint32_t point_num, gdi_color_t color)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_draw_polygon)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    uint32_t i;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (point_num == 0)
    {
        GDI_RETURN_VOID;
    }
    if (point_num == 1)
    {
        gdi_draw_point(point[0].x, point[0].y, color);
        GDI_RETURN_VOID;
    }
    for (i = 0 ; i < point_num ; i++)
    {
        uint32_t idx = (i + 1);
        if (i == (point_num - 1))
        {
            idx = 0;
        }
        gdi_draw_line_ext(point[i].x, point[i].y, point[idx].x, point[idx].y, color, false);
    }
    
    GDI_EXIT_CRITICAL_SECTION(gdi_draw_polygon)
}

static void gdi_draw_solid_polygon_AddNode(
                gdi_polygon_edge_struct **head, 
                gdi_polygon_edge_struct *node)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (*head == NULL)
    {
        *head = node;
        node->next = NULL;
    }
    else
    {
        gdi_polygon_edge_struct *Etmp = *head;
        if (((node->Xfra == 0) && (Etmp->Xint == node->Xint)) || 
            (Etmp->Xint > node->Xint))
        {
            node->next = Etmp;
            *head = node;
            return;
        }
        while (Etmp->next != NULL)
        {
            /* insert node with ordering by Xcur */
            if (((node->Xfra == 0) && (Etmp->next->Xint == node->Xint)) || 
                (Etmp->next->Xint > node->Xint))
            {
                node->next = Etmp->next;
                Etmp->next = node;
                break;
            }
            Etmp = Etmp->next;
        }
        if (Etmp->next == NULL)
        {
            /* insert to tail */
            Etmp->next = node;
            node->next = NULL;
        }
    }
}

static void gdi_draw_solid_polygon_RemoveNextNode(
                gdi_polygon_edge_struct **head, 
                gdi_polygon_edge_struct *node)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (node == NULL)
    {
        *head = (*head)->next;
    }
    else if (node->next !=  NULL)
    {
        node->next = node->next->next;
    }
}

static void gdi_draw_solid_polygon_UpdateX(
                gdi_polygon_edge_struct *head)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_polygon_edge_struct *Etmp = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    Etmp = head;
    while (Etmp != NULL)
    {
        if (Etmp->slope_y == 0)
        {
            Etmp = Etmp->next;
            continue;
        }
        Etmp->Xfra += Etmp->slope_x;
        if (Etmp->Xfra > 0)
        {
            while (Etmp->Xfra > Etmp->slope_y)
            {
                Etmp->Xint++;
                Etmp->Xfra -= Etmp->slope_y;
            }
        }
        else if (Etmp->Xfra < 0)
        {
            while ((0 - Etmp->Xfra) > Etmp->slope_y)
            {
                Etmp->Xint--;
                Etmp->Xfra += Etmp->slope_y;
            }
        }
        Etmp = Etmp->next;
    }
}

/*****************************************************************************
 * FUNCTION
 *  gdi_draw_filled_polygon
 * DESCRIPTION
 *  draw a solid polygon
 * PARAMETERS
 *  point_array [IN]    point array
 *  point_num   [IN]    the number of elements of point array
 *  color       [IN]    the color of polygon
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_draw_filled_polygon(
            gdi_point_t point[], 
            uint32_t point_num, 
            gdi_color_t color)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_draw_filled_polygon)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    int32_t i;
    int32_t Ycur;   /* Current Y */
    gdi_polygon_edge_struct *AET_head = NULL, *AET_head_sort = NULL;    /* Active Edge Table */
    gdi_polygon_edge_struct *EYmin = NULL, *EYmax = NULL;  /* Edge with the smallest Y & largest Y */
    gdi_polygon_edge_struct *ET = NULL;     /* Edge Table*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (point_num == 0)
    {
        GDI_RETURN_VOID;
    }
    else if (point_num == 1)
    {
        gdi_draw_point(point[0].x, point[0].y, color);
        GDI_RETURN_VOID;
    }
    else if (point_num == 2)
    {
        gdi_draw_line_ext(point[0].x, point[0].y, point[1].x, point[1].y, color, false);
        GDI_RETURN_VOID;
    }

    ET = (gdi_polygon_edge_struct*)gdi_alloc_ext_mem(sizeof(gdi_polygon_edge_struct)*point_num);
    if (ET == NULL)
    {
        GDI_RETURN_VOID;
    }
    
    /* init Edge Table (ET) */
    for (i = 0 ; i < point_num ; i++)
    {
        gdi_point_t *p1, *p2;
        uint32_t idx = (i + 1);

        if (i == (point_num - 1))
        {
            idx = 0;
        }
        /* p1 is the point with smaller y */
        /* If p1.y==p2.y, p1 is the point with larger x. */
        if ((point[i].y < point[idx].y) ||
            ((point[i].y == point[idx].y) && (point[i].x > point[idx].x)))
        {
            p1 = &point[i];
            p2 = &point[idx];
        }
        else
        {
            p1 = &point[idx];
            p2 = &point[i];
        }
        ET[i].Ymax = p2->y;
        ET[i].Ymin = p1->y;
        ET[i].slope_x = p2->x - p1->x;
        ET[i].slope_y = p2->y - p1->y;
        ET[i].Xint = p1->x;
        ET[i].Xfra = ET[i].slope_y;
        if (ET[i].slope_x < 0)
        {
            ET[i].Xfra = 0 - ET[i].Xfra;
        }
        ET[i].next = NULL;

        /* Find EYmin and EYmax */
        if ((EYmin == NULL) || (ET[i].Ymin < EYmin->Ymin))
        {
            EYmin = &ET[i];
        }
        if ((EYmax == NULL) || (ET[i].Ymax > EYmax->Ymax))
        {
            EYmax = &ET[i];
        }
    }

    for (Ycur = EYmin->Ymin ; Ycur <= EYmax->Ymax ; )
    {
        gdi_polygon_edge_struct *Etmp = NULL, *Epre = NULL;

        /* remove edges with Ymax==Ycur from AET */
        Etmp = AET_head;
        while (Etmp != NULL)
        {
            if ((Etmp->Ymax <= Ycur) && (Ycur != EYmax->Ymax))
            {
                gdi_draw_solid_polygon_RemoveNextNode(&AET_head, Epre);
            }
            else
            {
                Epre = Etmp;
            }
            Etmp = Etmp->next;
        }
        
        /* resort by X */
        AET_head_sort = NULL;
        while (AET_head != NULL)
        {
            Etmp = AET_head;
            AET_head = AET_head->next;
            gdi_draw_solid_polygon_AddNode(&AET_head_sort, Etmp);
        }
        AET_head = AET_head_sort;
        
        /* move edges with Y==Ycur in ET to AET */
        for (i = 0 ; i < point_num ; i++)
        {
            if (ET[i].Ymin == Ycur)
            {
                gdi_draw_solid_polygon_AddNode(&AET_head, &ET[i]);
            }
        }
        /* draw scan line */
        Etmp = AET_head;
        while (Etmp != NULL)
        {
            gdi_polygon_edge_struct *Enxt = Etmp->next;
            while (Enxt != NULL)
            {
                if (Enxt->slope_y != 0)
                {
                    break;
                }
                Enxt = Enxt->next;
            }
            if (Enxt == NULL)
            {
                break;
            }
            if (Etmp->Xint == Enxt->Xint)
            {
                gdi_draw_point(Etmp->Xint, Ycur, color);
            }
            else
            {
                gdi_draw_line_ext(Etmp->Xint, Ycur, Enxt->Xint, Ycur, color, false);
            }
            
            /* go to next next edge */
            Etmp = Enxt;
            Etmp = Etmp->next;
        }

        Ycur++;
        if (Ycur > gdi_act_layer->clipy2)   /* clip y */
        {
            break;
        }
        
        /* update Xcur of edges in AET */
        gdi_draw_solid_polygon_UpdateX(AET_head);

    }
    if (ET != NULL)
    {
        gdi_free_ext_mem((void**)&ET);
    }
    GDI_EXIT_CRITICAL_SECTION(gdi_draw_filled_polygon)
}


