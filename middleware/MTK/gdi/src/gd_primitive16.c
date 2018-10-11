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

#include "gd_include.h"
#include "gdi_internal.h"
#include "gdi_datatype.h"
#include "gdi_primitive.h"
#include "gdi.h"
#include "gd_primitive_arm.h"
#define BYTES_PER_PIXEL    2

/*****************************************************************************
 * FUNCTION
 *  gd_draw_line16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  imgp            [?]         
 *  destWidth       [IN]        
 *  x1              [IN]        
 *  y1              [IN]        
 *  x2              [IN]        
 *  y2              [IN]        
 *  clipx1          [IN]        
 *  clipy1          [IN]        
 *  clipx2          [IN]        
 *  clipy2          [IN]        
 *  color           [IN]        
 *  dotted          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_draw_line16(
        uint16_t *imgp,
        int destWidth,
        int x1,
        int y1,
        int x2,
        int y2,
        int clipx1,
        int clipy1,
        int clipx2,
        int clipy2,
        gdi_color_t color,
        bool dotted)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int xscan;
    int rate;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((x1 == x2) && (y1 == y2))
    {
        if ((x1 < clipx1) || (x1 > clipx2))
        {
            return;
        }
        if ((y1 < clipy1) || (y1 > clipy2))
        {
            return;
        }
        imgp[x1 + y1 * destWidth] = color;
        return;
    }

    {
        int dx = x2 - x1;
        int dy = y2 - y1;

        /* calculate slope rate */
        {
            int absdx;
            int absdy;

            if (dx < 0)
            {
                dx --;
                x1 ++;
				absdx = -dx;
            }
            else
            {
                if(dx != 0)
                {
                    dx ++;
                    x2 ++;
                }
				absdx = dx;
            }

            if (dy < 0)
            {
                dy --;
                y1 ++;
				absdy = -dy;
            }
            else
            {
                if(dy != 0)
                {
                    dy ++;
                    y2 ++;
                }
				absdy = dy;
            }

            if (absdx >= absdy)
            {
                xscan = 1;
                if (x1 > x2)
                {   /* exchange */
                    int temp = x1;

                    x1 = x2;
                    x2 = temp;
                    temp = y1;
                    y1 = y2;
                    y2 = temp;
                }
                rate = (dy << 20) / dx;
            }
            else
            {
                xscan = 0;
                if (y1 > y2)
                {   /* exchange */
                    int temp = x1;

                    x1 = x2;
                    x2 = temp;
                    temp = y1;
                    y1 = y2;
                    y2 = temp;
                }
                rate = (dx << 20) / dy;
            }
        }

        /* clipping */
        if (xscan)
        {
            if (y1 <= y2)
            {
                if (y1 < clipy1)
                {
                    int orgy = y1;

                    y1 = clipy1;
                    if (y1 > y2)
                    {
                        return;
                    }
                    x1 = x1 - ((orgy - y1) * dx) / dy;
                }

                if (y2 > clipy2)
                {
                    int orgy = y2;

                    y2 = clipy2;
                    if (y1 > y2)
                    {
                        return;
                    }
                    x2 = x2 - ((orgy - y2) * dx) / dy;
                }
            }
            else
            {   /* y1 > y2 */
                if (y2 < clipy1)
                {
                    int orgy = y2;

                    y2 = clipy1;
                    if (y2 > y1)
                    {
                        return;
                    }
                    x2 = x2 - ((orgy - y2) * dx) / dy;
                }

                if (y1 > clipy2)
                {
                    int orgy = y1;

                    y1 = clipy2;
                    if (y2 > y1)
                    {
                        return;
                    }
                    x1 = x1 - ((orgy - y1) * dx) / dy;
                }
            }

            if (x1 < clipx1)
            {
                int orgx = x1;

                x1 = clipx1;
                if (x1 > x2)
                {
                    return;
                }
                y1 = y1 - (((orgx - x1) * rate) >> 20);
            }

            if (x2 > clipx2)
            {
                int orgx = x2;

                x2 = clipx2;
                if (x1 > x2)
                {
                    return;
                }
                y2 = y2 - (((orgx - x2) * rate) >> 20);
            }

        }
        else
        {   /* Yscan */
            if (x1 <= x2)
            {
                if (x1 < clipx1)
                {
                    int orgx = x1;

                    x1 = clipx1;
                    if (x1 > x2)
                    {
                        return;
                    }
                    y1 = y1 - ((orgx - x1) * dy) / dx;  /* dx will not be zero */
                }

                if (x2 > clipx2)
                {
                    int orgx = x2;

                    x2 = clipx2;
                    if (x1 > x2)
                    {
                        return;
                    }
                    y2 = y2 - ((orgx - x2) * dy) / dx;  /* dx will not be zero */
                }
            }
            else
            {   /* x1 > x2 */
                if (x2 < clipx1)
                {
                    int orgx = x2;

                    x2 = clipx1;
                    if (x2 > x1)
                    {
                        return;
                    }
                    y2 = y2 - ((orgx - x2) * dy) / dx;  /* dx will not be zero */
                }

                if (x1 > clipx2)
                {
                    int orgx = x1;

                    x1 = clipx2;
                    if (x2 > x1)
                    {
                        return;
                    }
                    y1 = y1 - ((orgx - x1) * dy) / dx;  /* dx will not be zero */
                }
            }

            if (y1 < clipy1)
            {
                int orgy = y1;

                y1 = clipy1;
                if (y1 > y2)
                {
                    return;
                }
                x1 = x1 - (((orgy - y1) * rate) >> 20);
            }

            if (y2 > clipy2)
            {
                int orgy = y2;

                y2 = clipy2;
                if (y1 > y2)
                {
                    return;
                }
                x2 = x2 - (((orgy - y2) * rate) >> 20);
            }
        }
    }

    if (dotted == 0)
    {
        if (xscan == 0)
        {
            int tempWidth = destWidth;
            int tempx = 0;

            /* unsigned short * dstImageReg = &dst_->data[x1];//+ y1 * destWidth; */
            unsigned short *dstImageReg = &imgp[x1];    /* + y1 * destWidth; */
            unsigned short *endAddrReg = dstImageReg + y2 * tempWidth;

            dstImageReg += y1 * tempWidth;
            if (dstImageReg >= endAddrReg)
            {
                return;
            }
            do
            {
                dstImageReg[tempx >> 20] = color;
                dstImageReg += tempWidth;
                tempx += rate;
            } while (dstImageReg < endAddrReg);
        }
        else
        {
            int tempWidth = destWidth;
            int temprate = 0;

            /* unsigned short * dstImageReg = &dst_->data[y1 * tempWidth]; */
            unsigned short *dstImageReg = &imgp[y1 * tempWidth];
            unsigned short *endAddrReg = dstImageReg + x2;

            dstImageReg += x1;
            if (dstImageReg >= endAddrReg)
            {
                return;
            }
            do
            {
                dstImageReg[(temprate >> 20) * tempWidth] = color;
                dstImageReg++;
                temprate += rate;
            } while (dstImageReg < endAddrReg);
        }
    }
    else
    {   /* dotted */
        if (xscan == 0)
        {
            int tempWidth = destWidth;
            int tempx = 0;

            /* unsigned short * dstImageReg = &dst_->data[x1];//+ y1 * destWidth; */
            unsigned short *dstImageReg = &imgp[x1];    /* + y1 * destWidth; */
            unsigned short *endAddrReg = dstImageReg + y2 * tempWidth;

            dstImageReg += y1 * tempWidth;
            tempWidth += tempWidth;
            rate += rate;
            if (dstImageReg >= endAddrReg)
            {
                return;
            }
            do
            {
                dstImageReg[tempx >> 20] = color;
                dstImageReg += tempWidth;
                tempx += rate;
            } while (dstImageReg < endAddrReg);
        }
        else
        {
            int tempWidth = destWidth;
            int temprate = 0;

            /* unsigned short * dstImageReg = &dst_->data[y1 * tempWidth]; */
            unsigned short *dstImageReg = &imgp[y1 * tempWidth];
            unsigned short *endAddrReg = dstImageReg + x2;

            dstImageReg += x1;
            rate += rate;
            if (dstImageReg >= endAddrReg)
            {
                return;
            }
            do
            {
                dstImageReg[(temprate >> 20) * tempWidth] = color;
                dstImageReg += 2;
                temprate += rate;
            } while (dstImageReg < endAddrReg);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  gd_draw_rect16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  imgp            [?]         
 *  destWidth       [IN]        
 *  x1              [IN]        
 *  y1              [IN]        
 *  x2              [IN]        
 *  y2              [IN]        
 *  clipx1          [IN]        
 *  clipy1          [IN]        
 *  clipx2          [IN]        
 *  clipy2          [IN]        
 *  color           [IN]        
 *  dotted          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_draw_rect16(
        uint16_t *imgp,
        int destWidth,
        int x1,
        int y1,
        int x2,
        int y2,
        int clipx1,
        int clipy1,
        int clipx2,
        int clipy2,
        gdi_color_t color,
        bool dotted)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int width, height;
    int clipsides = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    width = x2 - x1 + 1;
    height = y2 - y1 + 1;

    if (x1 < clipx1)
    {
        width -= clipx1 - x1;
        x1 = clipx1;
        clipsides |= 0x110;
    }
    if (y1 < clipy1)
    {
        height -= clipy1 - y1;
        y1 = clipy1;
        clipsides |= 0x101;
    }

    if (width + x1 > clipx2)
    {
        width = clipx2 - x1;
        clipsides |= 0x120;
    }

    if (height + y1 > clipy2)
    {
        height = clipy2 - y1;
        clipsides |= 0x102;
    }

    if ((width <= 1) || (height <= 1))
    {
        if ((width == 1) && (height == 1))
        {
            /* dst_->data[y * destWidth + x] = color; */
            imgp[y1 * destWidth + x1] = color;
            return;
        }
        if ((width <= 0) || (height <= 0))
        {
            return;
        }
    }

    if (dotted == 0)
    {
        int widthTemp = width - 1;

        /* unsigned short  * dstImageReg    = (unsigned short *)(dst_->data + x + y * destWidth); */
        unsigned short *dstImageReg = (unsigned short*)(imgp + x1 + y1 * destWidth);
        unsigned short *dstImageEndReg = (unsigned short*)(dstImageReg + (height - 1) * destWidth);

        if (widthTemp < 0)
        {
            widthTemp = 0;
        }
        width <<= 1;

        if (clipsides == 0)
        {
            gdi_memset16((uint8_t*) dstImageReg, (uint16_t) color, width);
            dstImageReg += destWidth;
            while (dstImageReg < dstImageEndReg)
            {
                dstImageReg[0] = color;
                dstImageReg[widthTemp] = color;
                dstImageReg += destWidth;
            }
            gdi_memset16((uint8_t*) dstImageEndReg, (uint16_t) color, width);
        }
        else
        {

            if ((clipsides & 1) == 0)
            {
                gdi_memset16((uint8_t*) dstImageReg, (uint16_t) color, width);
            }

            /* width -= 2; */
            if ((clipsides & 0x30) == 0)
            {
                /* width += 2; */
                while (dstImageReg < dstImageEndReg)
                {
                    dstImageReg[0] = color;
                    dstImageReg[widthTemp] = color;
                    dstImageReg += destWidth;
                }
            }
            else if (((clipsides >> 4) & 1) == 0)
            {
                while (dstImageReg < dstImageEndReg)
                {
                    dstImageReg[0] = color;
                    dstImageReg += destWidth;
                }
            }
            else if (((clipsides >> 5) & 1) == 0)
            {
                /* width += 2; */
                while (dstImageReg < dstImageEndReg)
                {
                    dstImageReg[widthTemp] = color;
                    dstImageReg += destWidth;
                }
            }

            if ((clipsides & 2) == 0)
            {
                gdi_memset16((uint8_t*) dstImageEndReg, (uint16_t) color, width);
            }

        }
    }
    else
    {
        int tempx;
        int widthTemp = width - 1;

        /*  unsigned short * dstImageReg    = (unsigned short *)(dst_->data + x + y * destWidth); */
        unsigned short *dstImageReg = (unsigned short*)(imgp + x1 + y1 * destWidth);
        unsigned short *dstImageEndReg = (unsigned short*)(dstImageReg + (height - 1) * destWidth);

        if (widthTemp < 0)
        {
           // widthTemp = 0; remove for fix coverity deadcode issue, widthTemp can not be 0
        }

        if (clipsides == 0)
        {
            for (tempx = 0; tempx <= width; tempx += 2)
            {
                dstImageReg[tempx] = color;
            }
            dstImageReg[width] = color;
            while (dstImageReg <= dstImageEndReg)
            {
                dstImageReg[0] = color;
                dstImageReg[widthTemp] = color;
                dstImageReg += destWidth << 1;
            }
            for (tempx = 0; tempx <= width; tempx += 2)
            {
                dstImageEndReg[tempx] = color;
            }
            dstImageEndReg[width] = color;
        }
        else
        {
            if ((clipsides & 1) == 0)
            {
                for (tempx = 0; tempx <= width; tempx += 2)
                {
                    dstImageReg[tempx] = color;
                }
                dstImageReg[width] = color;
            }
            if ((clipsides & 0x30) == 0)
            {
                while (dstImageReg <= dstImageEndReg)
                {
                    dstImageReg[0] = color;
                    dstImageReg[widthTemp] = color;
                    dstImageReg += destWidth << 1;
                }
            }
            else if (((clipsides >> 4) & 1) == 0)
            {
                while (dstImageReg <= dstImageEndReg)
                {
                    dstImageReg[0] = color;
                    dstImageReg += destWidth << 1;
                }
            }
            else if (((clipsides >> 5) & 1) == 0)
            {
                while (dstImageReg <= dstImageEndReg)
                {
                    dstImageReg[widthTemp] = color;
                    dstImageReg += destWidth << 1;
                }
            }
            if ((clipsides & 2) == 0)
            {
                for (tempx = 0; tempx <= width; tempx += 2)
                {
                    dstImageEndReg[tempx] = color;
                }
                dstImageEndReg[width] = color;
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  gd_fill_rect16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  imgp            [?]         
 *  destWidth       [IN]        
 *  x1              [IN]        
 *  y1              [IN]        
 *  x2              [IN]        
 *  y2              [IN]        
 *  clipx1          [IN]        
 *  clipy1          [IN]        
 *  clipx2          [IN]        
 *  clipy2          [IN]        
 *  color           [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_fill_rect16(
        uint16_t *imgp,
        int destWidth,
        int x1,
        int y1,
        int x2,
        int y2,
        int clipx1,
        int clipy1,
        int clipx2,
        int clipy2,
        gdi_color_t color)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GD_CLIP_RECT_TEST(x1, y1, x2, y2, clipx1, clipy1, clipx2, clipy2);
    {
        uint8_t *dest = ((uint8_t*) imgp) + ((x1 + y1 * destWidth) * BYTES_PER_PIXEL);

        y2 -= y1;
        y2++;

        x2 -= x1;
        x2++;
        x2 <<= 1;
        x1 = destWidth * BYTES_PER_PIXEL;

        while (y2--)
        {
            gdi_memset16(dest, (uint16_t) color, x2);
            dest += x1;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  gd_draw_round_rect16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dstPtr          [?]         
 *  destWidth       [IN]        
 *  x               [IN]        
 *  y               [IN]        
 *  width           [IN]        
 *  height          [IN]        
 *  arcWidth        [IN]        
 *  arcHeight       [IN]        
 *  clipx1          [IN]        
 *  clipy1          [IN]        
 *  clipx2          [IN]        
 *  clipy2          [IN]        
 *  color           [IN]        
 *  dotted          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_draw_round_rect16(
        uint16_t *dstPtr,
        int destWidth,
        int x,
        int y,
        int width,
        int height,
        int arcWidth,
        int arcHeight,
        int clipx1,
        int clipy1,
        int clipx2,
        int clipy2,
        gdi_color_t color,
        bool dotted)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int xo = 0, yo = 0, width_ = 0, height_ = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    width_ = width >> 1;
    height_ = height >> 1;
    arcWidth = (arcWidth > width) ? width_ : arcWidth >> 1;
    arcHeight = (arcHeight > height) ? height_ : arcHeight >> 1;
    xo = x + width_;
    yo = y + height_;

    if (arcWidth < 2 || arcHeight < 2 || ((width < 5) && (height < 5)))
    {
        gd_draw_rect16(
            dstPtr,
            destWidth,
            xo - width,
            yo - height,
            xo + width,
            yo + height,
            clipx1,
            clipy1,
            clipx2,
            clipy2,
            color,
            dotted);
        return;
    }
    width = width_;
    height = height_;
    {

        int xo_ = xo + width - arcWidth;
        int yo_ = yo - height + arcHeight;
        int xo2_ = xo - width + arcWidth;
        int yo2_ = yo + height - arcHeight;

        int i, sqr;
        int x_, y_, x2_, y2_;
        int pre_x1, pre_x2;
        int mid_x1, mid_x2, dotted_flag = 0;

        int index, d, Di;

        /* 2003 0920 Hsu */
        if (arcHeight == 0)
        {
            arcHeight = 1;
        }
        d = 1048576 / arcHeight;    /* (1024*1024/arcHeight) */

        if (dotted)
        {
            dotted_flag = 1;
        }
        x_ = (xo_ + arcWidth - 1 < clipx1) ? clipx1 : ((xo_ + arcWidth - 1 > clipx2) ? clipx2 : xo_ + arcWidth - 1);
        x2_ = (xo2_ - arcWidth + 1 < clipx1) ? clipx1 : ((xo2_ - arcWidth + 1 > clipx2) ? clipx2 : xo2_ - arcWidth + 1);
        y_ = (yo_ < clipy1) ? clipy1 : ((yo_ > clipy2) ? clipy2 : yo_);
        y2_ = (yo2_ < clipy1) ? clipy1 : ((yo2_ > clipy2) ? clipy2 : yo2_);

        Di = 0; /* c*x_start;// start_point_shift_value */
        for (i = 1; i <= arcHeight; i++)
        {
            Di += d;
            if (dotted_flag && (i != arcHeight))
            {
                dotted_flag = 0;
                continue;
            }

            index = Di >> 10;
            sqr = (arcWidth * gd_sqrt[index]) >> 10;
            pre_x1 = x_;
            pre_x2 = x2_;

            x_ = (xo_ + sqr - 1 < clipx1) ? clipx1 : ((xo_ + sqr - 1 > clipx2) ? clipx2 : xo_ + sqr - 1);
            x2_ = (xo2_ - sqr + 1 < clipx1) ? clipx1 : ((xo2_ - sqr + 1 > clipx2) ? clipx2 : xo2_ - sqr + 1);
            y_ = (yo_ - i < clipy1) ? clipy1 : ((yo_ - i > clipy2) ? clipy2 : yo_ - i);
            y2_ = (yo2_ + i < clipy1) ? clipy1 : ((yo2_ + i > clipy2) ? clipy2 : yo2_ + i);

            /* 2003 1127 MaoSheng */
            if ((x_ != clipx1) && (x_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
            {
                *(dstPtr + y_ * destWidth + x_) = color;
            }
            if ((x_ != clipx1) && (x_ != clipx2) && (y2_ != clipy1) && (y2_ != clipy2))
            {
                *(dstPtr + y2_ * destWidth + x_) = color;
            }
            if ((x2_ != clipx1) && (x2_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
            {
                *(dstPtr + y_ * destWidth + x2_) = color;
            }
            if ((x2_ != clipx1) && (x2_ != clipx2) && (y2_ != clipy1) && (y2_ != clipy2))
            {
                *(dstPtr + y2_ * destWidth + x2_) = color;
            }

            if (dotted)
            {

                /* 2003 1127 MaoSheng */
                if (((pre_x1 - x_) > 3) || ((x2_ - pre_x2) > 3))
                {
                    if (pre_x1 - x_ > 3)
                    {
                        dotted_flag = (pre_x1 - x_) >> 1;
                    }
                    else if ((x2_ - pre_x2) > 3)
                    {
                        dotted_flag = (x2_ - pre_x2) >> 1;
                    }

                    mid_x1 = x_ + dotted_flag;
                    mid_x1 = (mid_x1 < clipx1) ? clipx1 : ((mid_x1 > clipx2) ? clipx2 : mid_x1);
                    mid_x2 = pre_x2 + dotted_flag;
                    mid_x2 = (mid_x2 < clipx1) ? clipx1 : ((mid_x2 > clipx2) ? clipx2 : mid_x2);

                    if ((mid_x1 != clipx1) && (mid_x1 != clipx2) && ((y_ + 1) != clipy1) && ((y_ + 1) != clipy2))
                        if ((y_ > clipy1) && (x_ < clipx2))
                        {
                            *(dstPtr + (y_ + 1) * destWidth + mid_x1) = color;  /* right_top_arc */
                        }
                    if ((mid_x1 != clipx1) && (mid_x1 != clipx2) && ((y2_ - 1) != clipy1) && ((y2_ - 1) != clipy2))
                        if ((y2_ < clipy2) && (x_ < clipx2))
                        {
                            *(dstPtr + (y2_ - 1) * destWidth + mid_x1) = color; /* right_bottom_arc */
                        }
                    if ((mid_x2 != clipx1) && (mid_x2 != clipx2) && ((y_ + 1) != clipy1) && ((y_ + 1) != clipy2))
                        if ((y_ > clipy1) && (x2_ > clipx1))
                        {
                            *(dstPtr + (y_ + 1) * destWidth + mid_x2) = color;  /* left_top_arc */
                        }

                    if ((mid_x2 != clipx1) && (mid_x2 != clipx2) && ((y2_ - 1) != clipy1) && ((y2_ - 1) != clipy2))
                        if ((y2_ < clipy2) && (x2_ > clipx1))
                        {
                            *(dstPtr + (y2_ - 1) * destWidth + mid_x2) = color; /* left_bottom_arc */
                        }
                }
                dotted_flag = 1;

            }
            else
            {

                /* 2003 1127 MaoSheng */
                if ((((pre_x1 - x_) > 1) || ((x2_ - pre_x2) > 1)) && (y_ != clipy1) && (y_ != clipy2))
                {
                    gdi_memset16((uint8_t*) (dstPtr + x_ + y_ * destWidth), (uint16_t) color, (pre_x1 - x_) << 1);
                    if (x2_ != pre_x2)
                    {
                        gdi_memset16(
                            (uint8_t*) (dstPtr + 1 + (pre_x2) + (y_) * destWidth),
                            (uint16_t) color,
                            (x2_ - pre_x2) << 1);
                    }
                }

                if ((((pre_x1 - x_) > 1) || ((x2_ - pre_x2) > 1)) && (y2_ != clipy1) && (y2_ != clipy2))
                {
                    if (x2_ != pre_x2)
                    {
                        gdi_memset16(
                            (uint8_t*) (dstPtr + 1 + (pre_x2) + (y2_) * destWidth),
                            (uint16_t) color,
                            (x2_ - pre_x2) << 1);
                    }

                    gdi_memset16((uint8_t*) (dstPtr + x_ + y2_ * destWidth), (uint16_t) color, (pre_x1 - x_) << 1);
                }
            }   /* end if(dotted) */

        }   /* end for(i=1;i<=arcHeight;i++) */

        /* i = arcHeight, drawing the lines of top and bottom  */
        if (dotted)
        {
            x_ = (xo < clipx1) ? clipx1 : ((xo > clipx2) ? clipx2 : xo);
            y_ = (yo - height < clipy1) ? clipy1 : ((yo - height > clipy2) ? clipy2 : yo - height);
            y2_ = (yo + height < clipy1) ? clipy1 : ((yo + height > clipy2) ? clipy2 : yo + height);

            /* 2003 1128 MaoSheng */
            if ((x_ != clipx1) && (x_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
            {
                *(dstPtr + (y_ * destWidth) + x_) = color;  /* draw top_center point */
            }
            if ((y2_ != clipy1) && (y2_ != clipy2))
            {
                *(dstPtr + y2_ * destWidth + x_) = color;   /* draw bottom_center point */
            }

            if (dotted)
            {
                dotted_flag = 1;
            }
            for (i = 1; i < (width - arcWidth); i++)
            {

                if (dotted_flag)
                {
                    dotted_flag = 0;
                    continue;
                }
                x_ = (xo - i < clipx1) ? clipx1 : ((xo - i > clipx2) ? clipx2 : xo - i);
                x2_ = (xo + i < clipx1) ? clipx1 : ((xo + i > clipx2) ? clipx2 : xo + i);

                if ((x_ != clipx1) && (x_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
                {
                    *(dstPtr + y_ * destWidth + x_) = color;    /* top_left_half_line */
                }
                if ((x_ != clipx1) && (x_ != clipx2) && (y2_ != clipy1) && (y2_ != clipy2))
                {
                    *(dstPtr + y2_ * destWidth + x_) = color;   /* bottom_left_half_line */
                }
                if ((x2_ != clipx1) && (x2_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
                {
                    *(dstPtr + y_ * destWidth + x2_) = color;   /* top_right_half_line */
                }
                if ((x2_ != clipx1) && (x2_ != clipx2) && (y2_ != clipy1) && (y2_ != clipy2))
                {
                    *(dstPtr + y2_ * destWidth + x2_) = color;  /* bottom_right_half_line */
                }

                if (dotted)
                {
                    dotted_flag = 1;
                }

            }

        }
        else
        {

            if ((x_-x2_)>0 ) 
            {
                gdi_memset16((uint8_t*) (dstPtr + x2_ + y_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
            }
            if ((x_-x2_)>0 ) 
            {
                gdi_memset16((uint8_t*) (dstPtr + x2_ + y2_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
            }
        }

        /* drawing the lines of left and right */
        x_ = (xo + width - 2 < clipx1) ? clipx1 : ((xo + width - 2 > clipx2) ? clipx2 : xo + width - 2);
        x2_ = (xo - width + 2 < clipx1) ? clipx1 : ((xo - width + 2 > clipx2) ? clipx2 : xo - width + 2);
        y_ = (yo < clipy1) ? clipy1 : ((yo > clipy2) ? clipy2 : yo);

        /* 2003 1127 MaoSheng */
        if ((x_ != clipx1) && (x_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
        {
            *(dstPtr + y_ * destWidth + x_) = color;
        }
        if ((x2_ != clipx1) && (x2_ != clipx2))
        {
            *(dstPtr + y_ * destWidth + x2_) = color;
        }

        if (dotted)
        {
            dotted_flag = 1;
        }
        for (i = 1; i <= (height - arcHeight); i++)
        {

            if (dotted_flag && (i != (height - arcHeight)))
            {
                dotted_flag = 0;
                continue;
            }
            y_ = (yo - i < clipy1) ? clipy1 : ((yo - i > clipy2) ? clipy2 : yo - i);
            y2_ = (yo + i < clipy1) ? clipy1 : ((yo + i > clipy2) ? clipy2 : yo + i);

            /* 2003 1127 MaoSheng */
            if ((x_ != clipx1) && (x_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
            {
                *(dstPtr + y_ * destWidth + x_) = color;    /* draw right_top_half_line */
            }
            if ((x_ != clipx1) && (x_ != clipx2) && (y2_ != clipy1) && (y2_ != clipy2))
            {
                *(dstPtr + y2_ * destWidth + x_) = color;   /* draw right_bottom_half_line */
            }
            if ((x2_ != clipx1) && (x2_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
            {
                *(dstPtr + y_ * destWidth + x2_) = color;   /* draw left_top_half_line */
            }
            if ((x2_ != clipx1) && (x2_ != clipx2) && (y2_ != clipy1) && (y2_ != clipy2))
            {
                *(dstPtr + y2_ * destWidth + x2_) = color;  /* draw left_bottom_half_line */
            }

            if (dotted)
            {
                dotted_flag = 1;
            }

        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  gd_fill_round_rect16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dstPtr          [?]         
 *  destWidth       [IN]        
 *  x               [IN]        
 *  y               [IN]        
 *  width           [IN]        
 *  height          [IN]        
 *  arcWidth        [IN]        
 *  arcHeight       [IN]        
 *  clipx1          [IN]        
 *  clipy1          [IN]        
 *  clipx2          [IN]        
 *  clipy2          [IN]        
 *  color           [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_fill_round_rect16(
        uint16_t *dstPtr,
        int destWidth,
        int x,
        int y,
        int width,
        int height,
        int arcWidth,
        int arcHeight,
        int clipx1,
        int clipy1,
        int clipx2,
        int clipy2,
        gdi_color_t color)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int xo = 0, yo = 0, width_ = 0, height_ = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    width_ = width >> 1;
    height_ = height >> 1;
    arcWidth = (arcWidth > width) ? width_ : arcWidth >> 1;
    arcHeight = (arcHeight > height) ? height_ : arcHeight >> 1;
    xo = x + width_;
    yo = y + height_;

    if (arcWidth < 2 || arcHeight < 2 || ((arcWidth<4) && (arcHeight<4)) || ((width < 5) && (height < 5)))
    {
        width >>= 1;
        height >>= 1;
        gd_fill_rect16(
            dstPtr,
            destWidth,
            xo - width,
            yo - height,
            xo + width,
            yo + height,
            clipx1,
            clipy1,
            clipx2,
            clipy2,
            color);
        return;
    }
    width = width_;
    height = height_;
    {
        int xo_ = xo + width - arcWidth;
        int yo_ = yo - height + arcHeight;
        int xo2_ = xo - width + arcWidth;
        int yo2_ = yo + height - arcHeight;

        int i, sqr;
        int x_, y_, x2_, y2_;

        int index, d, Di;

        /* 2003 0920 Hsu */
        if (arcHeight == 0)
        {
            arcHeight = 1;
        }
        d = 1048576 / arcHeight;    /* (1024*1024/arcHeight) */
        if ((clipy2--) < 0)
        {
            clipy2 = 0;
        }
        x_ = (xo_ + arcWidth - 1 < clipx1) ? clipx1 : ((xo_ + arcWidth - 1 > clipx2) ? clipx2 : xo_ + arcWidth - 1);
        x2_ = (xo_ - arcWidth + 1 < clipx1) ? clipx1 : ((xo_ - arcWidth + 1 > clipx2) ? clipx2 : xo_ - arcWidth + 1);
        y_ = (yo_ < clipy1) ? clipy1 : ((yo_ > clipy2) ? clipy2 : yo_);
        y2_ = (yo2_ < clipy1) ? clipy1 : ((yo2_ > clipy2) ? clipy2 : yo2_);
        /* 2003 0825 Hsu */
        if ((y_ >= clipy1) && (y_ <= clipy2) && ((x_-x2_) > 0))
        {
            gdi_memset16((uint8_t*) (dstPtr + x2_ + y_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
        }
        if ((y2_ >= clipy1) && (y2_ <= clipy2) && ((x_-x2_) > 0))
        {
            gdi_memset16((uint8_t*) (dstPtr + x2_ + y2_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
        }
        Di = 0;

        for (i = 1; i <= arcHeight; i++)
        {
            Di += d;
            index = Di >> 10;
            sqr = (arcWidth * gd_sqrt[index]) >> 10;

            x_ = (xo_ + sqr - 1 < clipx1) ? clipx1 : ((xo_ + sqr - 1 > clipx2) ? clipx2 : xo_ + sqr - 1);
            x2_ = (xo2_ - sqr + 1 < clipx1) ? clipx1 : ((xo2_ - sqr + 1 > clipx2) ? clipx2 : xo2_ - sqr + 1);
            y_ = (yo_ - i < clipy1) ? clipy1 : ((yo_ - i > clipy2) ? clipy2 : yo_ - i);
            y2_ = (yo2_ + i < clipy1) ? clipy1 : ((yo2_ + i > clipy2) ? clipy2 : yo2_ + i);

            if ((y_ >= clipy1) && (y_ <= clipy2) && ((x_ - x2_) > 0))
            {
                gdi_memset16((uint8_t*) (dstPtr + x2_ + y_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
            }

            if ((y2_ >= clipy1) && (y2_ <= clipy2) && ((x_ - x2_) > 0))
            {
                gdi_memset16((uint8_t*) (dstPtr + x2_ + y2_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
            }
        }

        /* fill the Rectangle  */
        x_ = (xo + width - 1 < clipx1) ? clipx1 : ((xo + width - 1 > clipx2) ? clipx2 : xo + width - 1);
        x2_ = (xo - width + 1 < clipx1) ? clipx1 : ((xo - width + 1 > clipx2) ? clipx2 : xo - width + 1);
        y_ = (yo < clipy1) ? clipy1 : ((yo > clipy2) ? clipy2 : yo);

        if ((y_ >= clipy1) && (y_ <= clipy2))
        {
            gdi_memset16((uint8_t*) (dstPtr + x2_ + y_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
        }

        for (i = 1; i <= (height - arcHeight); i++)
        {
            y_ = (yo - i < clipy1) ? clipy1 : ((yo - i > clipy2) ? clipy2 : yo - i);
            y2_ = (yo + i < clipy1) ? clipy1 : ((yo + i > clipy2) ? clipy2 : yo + i);

            if ((y_ >= clipy1) && (y_ <= clipy2) && ((x_ - x2_) > 0))
            {
                gdi_memset16((uint8_t*) (dstPtr + x2_ + y_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
            }

            if ((y2_ >= clipy1) && (y2_ <= clipy2) && ((x_ - x2_) > 0))
            {
                gdi_memset16((uint8_t*) (dstPtr + x2_ + y2_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  gd_draw_ellipse16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dstPtr          [?]         
 *  destWidth       [IN]        
 *  x               [IN]        
 *  y               [IN]        
 *  arcWidth        [IN]        
 *  arcHeight       [IN]        
 *  clipx1          [IN]        
 *  clipy1          [IN]        
 *  clipx2          [IN]        
 *  clipy2          [IN]        
 *  color           [IN]        
 *  dotted          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_draw_ellipse16(
        uint16_t *dstPtr,
        int destWidth,
        int x,
        int y,
        int arcWidth,
        int arcHeight,
        int clipx1,
        int clipy1,
        int clipx2,
        int clipy2,
        gdi_color_t color,
        bool dotted)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i, sqr;
    int x_, y_, x2_, y2_;
    int pre_x1, pre_x2;
    int mid_x1, mid_x2, dotted_flag = 0;

    int index, d, Di;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* 2003 0920 Hsu */
    if ((clipy2--) < 0)
    {
        clipy2 = 0;
    }
    if (arcHeight == 0)
    {
        arcHeight = 1;
    }
    d = 1048576 / arcHeight;    /* (1024*1024/arcHeight) */

    if (dotted)
    {
        dotted_flag = 1;
    }
    x_ = (x + arcWidth - 1 < clipx1) ? clipx1 : ((x + arcWidth - 1 > clipx2) ? clipx2 : x + arcWidth - 1);
    x2_ = (x - arcWidth + 1 < clipx1) ? clipx1 : ((x - arcWidth + 1 > clipx2) ? clipx2 : x - arcWidth + 1);
    y_ = (y < clipy1) ? clipy1 : ((y > clipy2) ? clipy2 : y);

    if ((x_ != clipx1) && (x_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
    {
        *(dstPtr + y_ * destWidth + x_ - 1) = color;
    }
    if ((x2_ != clipx1) && (x2_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
    {
        *(dstPtr + y_ * destWidth + x2_ + 1) = color;
    }

    Di = 0; /* c*x_start; // start_point_shift_value */
    for (i = 1; i <= arcHeight; i++)
    {
        Di += d;

        if (dotted_flag && (i != arcHeight))
        {
            dotted_flag = 0;
            continue;
        }

        index = Di >> 10;
        sqr = (arcWidth * gd_sqrt[index]) >> 10;
        pre_x1 = x_;
        pre_x2 = x2_;

        x_ = (x + sqr - 1 < clipx1) ? clipx1 : ((x + sqr - 1 > clipx2) ? clipx2 : x + sqr - 1);
        x2_ = (x - sqr + 1 < clipx1) ? clipx1 : ((x - sqr + 1 > clipx2) ? clipx2 : x - sqr + 1);
        y_ = (y - i < clipy1) ? clipy1 : ((y - i > clipy2) ? clipy2 : y - i);
        y2_ = (y + i < clipy1) ? clipy1 : ((y + i > clipy2) ? clipy2 : y + i);

        /* 2003 1127 MaoSheng */
        if ((x_ != clipx1) && (x_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
        {
            *(dstPtr + y_ * destWidth + x_) = color;
        }
        if ((x_ != clipx1) && (x_ != clipx2) && (y2_ != clipy1) && (y2_ != clipy2))
        {
            *(dstPtr + y2_ * destWidth + x_) = color;
        }
        if ((x2_ != clipx1) && (x2_ != clipx2) && (y_ != clipy1) && (y_ != clipy2))
        {
            *(dstPtr + y_ * destWidth + x2_) = color;
        }
        if ((x2_ != clipx1) && (x2_ != clipx2) && (y2_ != clipy1) && (y2_ != clipy2))
        {
            *(dstPtr + y2_ * destWidth + x2_) = color;
        }

        if (dotted)
        {

            if (((pre_x1 - x_) > 3) || ((x2_ - pre_x2) > 3))
            {
                if (pre_x1 - x_ > 3)
                {
                    dotted_flag = (pre_x1 - x_) >> 1;
                }
                else if ((x2_ - pre_x2) > 3)
                {
                    dotted_flag = (x2_ - pre_x2) >> 1;
                }

                mid_x1 = x_ + dotted_flag;
                mid_x1 = (mid_x1 < clipx1) ? clipx1 : ((mid_x1 > clipx2) ? clipx2 : mid_x1);
                mid_x2 = pre_x2 + dotted_flag;
                mid_x2 = (mid_x2 < clipx1) ? clipx1 : ((mid_x2 > clipx2) ? clipx2 : mid_x2);

                if ((mid_x1 != clipx1) && (mid_x1 != clipx2) && ((y_ + 1) != clipy1) && ((y_ + 1) != clipy2))
                    if ((y_ > clipy1) && (x_ < clipx2))
                    {
                        *(dstPtr + (y_ + 1) * destWidth + mid_x1) = color;
                    }
                if ((mid_x1 != clipx1) && (mid_x1 != clipx2) && ((y2_ - 1) != clipy1) && ((y2_ - 1) != clipy2))
                    if ((y2_ < clipy2) && (x_ < clipx2))
                    {
                        *(dstPtr + (y2_ - 1) * destWidth + mid_x1) = color;
                    }
                if ((mid_x2 != clipx1) && (mid_x2 != clipx2) && ((y_ + 1) != clipy1) && ((y_ + 1) != clipy2))
                    if ((y_ > clipy1) && (x2_ > clipx1))
                    {
                        *(dstPtr + (y_ + 1) * destWidth + mid_x2) = color;
                    }
                if ((mid_x2 != clipx1) && (mid_x2 != clipx2) && ((y2_ - 1) != clipy1) && ((y2_ - 1) != clipy2))
                    if ((y2_ < clipy2) && (x2_ > clipx1))
                    {
                        *(dstPtr + (y2_ - 1) * destWidth + mid_x2) = color;
                    }

            }
            dotted_flag = 1;

        }
        else
        {
            /* 2003 1201 MaoSheng */
            if ((((pre_x1 - x_) > 1) || ((x2_ - pre_x2) > 1)) && (y_ != clipy1) && (y_ != clipy2))
            {
                if (i < arcHeight)
                {
                    gdi_memset16((uint8_t*) (dstPtr + x_ + y_ * destWidth), (uint16_t) color, (pre_x1 - x_) << 1);
                }
                else
                {
                    gdi_memset16(
                        (uint8_t*) (dstPtr + x_ - (x_ - x2_) + y_ * destWidth),
                        (uint16_t) color,
                        (x_ - x2_) * 2 + ((pre_x1 - x_) << 1));
                }
                if (x2_ != pre_x2)
                {
                    if ((i < arcHeight))
                    {
                        gdi_memset16((uint8_t*) (dstPtr + 1 + pre_x2 + y_ * destWidth), (uint16_t) color, (x2_ - pre_x2) << 1);
                    }
                    else
                    {
                        if ((1 + x2_) <= clipx2)
                        {
                            gdi_memset16(
                                (uint8_t*) (dstPtr + 1 + pre_x2 + y_ * destWidth),
                                (uint16_t) color,
                                ((x2_ - pre_x2) << 1));
                        }
                        else
                        {
                            gdi_memset16(
                                (uint8_t*) (dstPtr + 1 + pre_x2 + y_ * destWidth),
                                (uint16_t) color,
                                (clipx2 - pre_x2 - 1) << 1);
                        }
                    }
                }
            }
            if ((((pre_x1 - x_) > 1) || ((x2_ - pre_x2) > 1)) && (y2_ != clipy1) && (y2_ != clipy2))
            {
                if (x2_ != pre_x2)
                {
                    if (i < arcHeight)
                    {
                        gdi_memset16((uint8_t*) (dstPtr + 1 + pre_x2 + y2_ * destWidth), (uint16_t) color, (x2_ - pre_x2) << 1);
                    }
                    else
                    {
                        if ((1 + x2_) <= clipx2)
                        {
                            gdi_memset16(
                                (uint8_t*) (dstPtr + 1 + pre_x2 + y2_ * destWidth),
                                (uint16_t) color,
                                ((x2_ - pre_x2) << 1));
                        }
                        else
                        {
                            gdi_memset16(
                                (uint8_t*) (dstPtr + 1 + pre_x2 + y2_ * destWidth),
                                (uint16_t) color,
                                (clipx2 - pre_x2 - 1) << 1);
                        }
                    }
                }
                if (i < arcHeight)
                {
                    gdi_memset16((uint8_t*) (dstPtr + x_ + y2_ * destWidth), (uint16_t) color, (pre_x1 - x_) << 1);
                }
                else
                {
                    gdi_memset16(
                        (uint8_t*) (dstPtr + x_ - (x_ - x2_) + y2_ * destWidth),
                        (uint16_t) color,
                        (x_ - x2_) * 2 + ((pre_x1 - x_) << 1));
                }
            }

        }

    }

}

/*****************************************************************************
 * FUNCTION
 *  gd_fill_ellipse16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dstPtr          [?]         
 *  destWidth       [IN]        
 *  x               [IN]        
 *  y               [IN]        
 *  arcWidth        [IN]        
 *  arcHeight       [IN]        
 *  clipx1          [IN]        
 *  clipy1          [IN]        
 *  clipx2          [IN]        
 *  clipy2          [IN]        
 *  color           [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_fill_ellipse16(
        uint16_t *dstPtr,
        int destWidth,
        int x,
        int y,
        int arcWidth,
        int arcHeight,
        int clipx1,
        int clipy1,
        int clipx2,
        int clipy2,
        gdi_color_t color)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i, sqr;
    int x_, y_, x2_, y2_;

    int index, d, Di;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* 2003 0920 Hsu */
    if (arcHeight == 0)
    {
        arcHeight = 1;
    }
    d = 1048576 / arcHeight;    /* (1024*1024/arcHeight) */
    if ((clipy2--) < 0)
    {
        clipy2 = 0;
    }
    y_ = (y < clipy1) ? clipy1 : ((y > clipy2) ? clipy2 : y);
    if ((y_ != clipy1) && (y_ != clipy2))
    {
        x_ = (x + arcWidth - 1 < clipx1) ? clipx1 : ((x + arcWidth - 1 > clipx2) ? clipx2 : x + arcWidth - 1);
        x2_ = (x - arcWidth + 1 < clipx1) ? clipx1 : ((x - arcWidth + 1 > clipx2) ? clipx2 : x - arcWidth + 1);
        gdi_memset16((uint8_t*) (dstPtr + x2_ + y_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
    }

    Di = 0; /* start_point_shift_value */
    for (i = 1; i < arcHeight; i++)
    {
        Di += d;
        index = Di >> 10;
        sqr = (arcWidth * gd_sqrt[index]) >> 10;

        x_ = (x + sqr < clipx1) ? clipx1 : ((x + sqr > clipx2) ? clipx2 : x + sqr);
        x2_ = (x - sqr < clipx1) ? clipx1 : ((x - sqr > clipx2) ? clipx2 : x - sqr);
        y_ = (y - i < clipy1) ? clipy1 : ((y - i > clipy2) ? clipy2 : y - i);
        y2_ = (y + i < clipy1) ? clipy1 : ((y + i > clipy2) ? clipy2 : y + i);

        if ((y_ >= clipy1) && (y_ <= clipy2) && ((x_ - x2_) > 0))
        {
            gdi_memset16((uint8_t*) (dstPtr + x2_ + y_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
        }

        if ((y2_ >= clipy1) && (y2_ <= clipy2) && ((x_ - x2_) > 0))
        {
            gdi_memset16((uint8_t*) (dstPtr + x2_ + y2_ * destWidth), (uint16_t) color, (x_ - x2_) << 1);
        }

    }

}

/* Maximum width for this function is only 512 */
/*****************************************************************************
 * FUNCTION
 *  gd_fill_triangle16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  imgp            [?]         
 *  destWidth       [IN]        
 *  x1              [IN]        
 *  y1              [IN]        
 *  x2              [IN]        
 *  y2              [IN]        
 *  x3              [IN]        
 *  y3              [IN]        
 *  clipx1          [IN]        
 *  clipy1          [IN]        
 *  clipx2          [IN]        
 *  clipy2          [IN]        
 *  color           [IN]        
 *  dotted          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_fill_triangle16(
        uint16_t *imgp,
        int destWidth,
        int x1,
        int y1,
        int x2,
        int y2,
        int x3,
        int y3,
        int clipx1,
        int clipy1,
        int clipx2,
        int clipy2,
        gdi_color_t color,
        bool dotted)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int x[3], y[3];
    int yStart = 0, yEnd = 0;
    int i = 0, yStop = 0, yStart2 = 0;

    // int LB=0;    /* xB<xC  */
    int Drawing = 0;    /* drawing second part of triangle   
                           0->normal, 1->only first, 2->only second  */
    int slope[3];
    int constant[3];
    int p1 = 0, p2 = 0;
    int xf1 = 0, xf2 = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* arrange the vertics in sequence from top to bottom  */
    if (y1 < y2)
    {
        y[0] = y1;
        x[0] = x1;
        y[1] = y2;
        x[1] = x2;
        y[2] = y3;
        x[2] = x3;
    }
    else
    {
        y[0] = y2;
        x[0] = x2;
        y[1] = y1;
        x[1] = x1;
        y[2] = y3;
        x[2] = x3;
    }

    if (y3 < y[0])
    {
        y[2] = y[1];
        x[2] = x[1];
        y[1] = y[0];
        x[1] = x[0];
        y[0] = y3;
        x[0] = x3;
    }
    else if (y3 < y[1])
    {
        y[2] = y[1];
        x[2] = x[1];
        y[1] = y3;
        x[1] = x3;
    }

    /* check the position of pB and pC */
    /* if (x[1] > x[2]) LB=1; */

    /* check if the desired triangle out of clipping area */
    if ((y[0] > clipy2) || (y[2] < clipy1))
    {
        return;
    }

    {
        int xMax = 0, xMin = 9999;

        for (i = 0; i < 3; i++)
        {
            if (x[i] > xMax)
            {
                xMax = x[i];
            }
            if (x[i] < xMin)
            {
                xMin = x[i];
            }
        }
        if ((xMin > clipx2) || (xMax < clipx1))
        {
            return;
        }
    }

    /* decide the start and finish y-position */
    yStart = (y[0] < clipy1) ? clipy1 : y[0];
    yEnd = (y[2] > clipy2) ? clipy2 : y[2];
    yStart2 = yStop = y[1];

    if (y[1] < yStart)
    {
        Drawing = 2;
        yStart2 = yStart;
    }
    else if (y[1] > clipy2)
    {
        yEnd = clipy2;
        yStop = yEnd;
        Drawing = 1;
    }

    /* start to draw triangle... */
    {
        int dx, dy;

        for (i = 0; i < 3; i++)
        {
            if (i == 2)
            {
                dx = x[i] - x[0];
                dy = y[i] - y[0];
            }
            else
            {
                dx = x[i + 1] - x[i];
                dy = y[i + 1] - y[i];
            }

            if (dy == 0)
            {
                slope[i] = 0;
            }
            /* 13 + 9 + 9 = 31  max width will be 512 */
            else
            {
                slope[i] = (dx << 13) / dy;
            }
            constant[i] = x[i] - ((slope[i] * y[i]) >> 13);
        }
    }

    if ((Drawing == 0) || (Drawing == 1))
    {
        /* 2006 0323 Hsu */
        for (i = yStart; i < yStop; i++)
        {
            p1 = constant[0] + ((slope[0] * i) >> 13);
            p2 = constant[2] + ((slope[2] * i) >> 13);
            xf1 = (p1 > clipx2) ? clipx2 : ((p1 < clipx1) ? clipx1 : p1);
            xf2 = (p2 > clipx2) ? clipx2 : ((p2 < clipx1) ? clipx1 : p2);
            if (xf2 > xf1)
            {
                /* int len = (xf2-xf1)<<1; */
                gdi_memset16((uint8_t*) (imgp + xf1 + i * destWidth), (uint16_t) color, (xf2 - xf1) << 1);
                /*          MEMSET_16_((dst_->data + xf1 + j * destWidth)  ,color, (xf2-xf1)); */
            }
            else if (xf2 < xf1)
            {
                gdi_memset16((uint8_t*) (imgp + xf2 + i * destWidth), (uint16_t) color, (xf1 - xf2) << 1);
            }
        }

    }

    if ((Drawing == 0) || (Drawing == 2))
    {
        /* 2006 0323 Hsu */
        for (i = yStart2; i < yEnd; i++)
        {
            p1 = constant[1] + ((slope[1] * i) >> 13);
            p2 = constant[2] + ((slope[2] * i) >> 13);
            xf1 = (p1 > clipx2) ? clipx2 : ((p1 < clipx1) ? clipx1 : p1);
            xf2 = (p2 > clipx2) ? clipx2 : ((p2 < clipx1) ? clipx1 : p2);

            if (xf2 > xf1)
            {
                /* int len = (xf2-xf1)<<1; */
                gdi_memset16((uint8_t*) (imgp + xf1 + i * destWidth), (uint16_t) color, (xf2 - xf1) << 1);
                /*          MEMSET_16_( (dst_->data + xf1 + j * destWidth)  ,color, (xf2-xf1)); */
            }
            else if (xf2 < xf1)
            {
                gdi_memset16((uint8_t*) (imgp + xf2 + i * destWidth), (uint16_t) color, (xf1 - xf2) << 1);
            }
        }

    }
}

/*****************************************************************************
 * FUNCTION
 *  gd_color_from_rgb_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  A       [IN]        
 *  R       [IN]        
 *  G       [IN]        
 *  B       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_color_t gd_color_from_rgb_16(uint8_t A, uint8_t R, uint8_t G, uint8_t B)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return (gdi_color_t) ((((B) & 0xf8) >> 3) | (((G) & 0xfc) << 3) | (((R) & 0xf8) << 8));
}

/*****************************************************************************
 * FUNCTION
 *  gd_color_to_rgb_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  A       [?]         
 *  R       [?]         
 *  G       [?]         
 *  B       [?]         
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_color_to_rgb_16(uint8_t *A, uint8_t *R, uint8_t *G, uint8_t *B, gdi_color_t c)
{
    *A = 0xFF;
    *R = (uint8_t)(((((c) & 0xf800) >> 8)) >> 5);   
    *G = (uint8_t)(((((c) & 0x07e0) >> 3)) >> 6);
    *B = (uint8_t)(((((c) & 0x001f) << 3)) >> 5);
}

/*****************************************************************************
 * FUNCTION
 *  gd_get_pixel_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_color_t gd_get_pixel_16(int x, int y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return *(uint16_t*) (gdi_act_layer->buf_ptr + ((y * gdi_act_layer->width + x) << 1));
}

/*****************************************************************************
 * FUNCTION
 *  gd_put_pixel_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_pixel_16(int x, int y, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *(uint16_t*) (gdi_act_layer->buf_ptr + ((y * gdi_act_layer->width + x) << 1)) = (uint16_t) c;
}


/*****************************************************************************
 * FUNCTION
 *  gd_put_pixel_to_layer_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_pixel_to_layer_16(gdi_layer_handle_t layer_handle, int x, int y, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *(uint16_t*) (layer->buf_ptr + ((y * layer->width + x) << 1)) = (uint16_t) c;
}

/*****************************************************************************
 * FUNCTION
 *  gd_get_buf_pixel_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  buf_ptr         [?]         
 *  buf_width       [IN]        
 *  x               [IN]        
 *  y               [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_color_t gd_get_buf_pixel_16(uint8_t *buf_ptr, uint32_t buf_width, int x, int y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return *(uint16_t*) (buf_ptr + ((y * buf_width + x) << 1));
}

/*****************************************************************************
 * FUNCTION
 *  gd_put_buf_pixel_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  buf_ptr         [?]         
 *  buf_width       [IN]        
 *  x               [IN]        
 *  y               [IN]        
 *  c               [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_buf_pixel_16(uint8_t *buf_ptr, uint32_t buf_width, int x, int y, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *(uint16_t*) (buf_ptr + ((y * buf_width + x) << 1)) = (uint16_t) c;
}

/*****************************************************************************
 * FUNCTION
 *  gd_replace_src_key_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest_ptr            [?]         
 *  dest_pitch          [IN]        
 *  dest_offset_x       [IN]        
 *  dest_offset_y       [IN]        
 *  clipx1              [IN]        
 *  clipy1              [IN]        
 *  clipx2              [IN]        
 *  clipy2              [IN]        
 *  src_key             [IN]        
 *  width               [IN]        
 *  height              [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_replace_src_key_16(
        uint8_t *dest_ptr,
        int32_t dest_pitch,
        int32_t dest_offset_x,
        int32_t dest_offset_y,
        int32_t clipx1,
        int32_t clipy1,
        int32_t clipx2,
        int32_t clipy2,
        gdi_color_t src_key,
        int32_t width,
        int32_t height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    uint8_t *dest_line_start_ptr;

    int32_t jump_size;
    int32_t start_x, start_y, end;
    int32_t x_draw_count, y_draw_count;

    /* if not totally out of clip region */
    if (!((dest_offset_x > clipx2) ||
          (dest_offset_y > clipy2) || (dest_offset_x + width < clipx1) || (dest_offset_y + height < clipy1)))
    {
        /* find x_draw_count and y_draw_count */
        start_x = (dest_offset_x < clipx1) ? clipx1 : dest_offset_x;
        end = (dest_offset_x + (int32_t) width - 1 > clipx2) ? clipx2 : dest_offset_x + (int32_t) width - 1;
        x_draw_count = end - start_x + 1;

        start_y = (dest_offset_y < clipy1) ? clipy1 : dest_offset_y;
        end = (dest_offset_y + (int32_t) height - 1 > clipy2) ? clipy2 : dest_offset_y + (int32_t) height - 1;
        y_draw_count = end - start_y + 1;

        /* adjust dest ptr to the start of drawing point */
        /* find start ptr for actual drawing */
        jump_size = ((start_y * dest_pitch + start_x) * 16) >> 3;
        dest_line_start_ptr = dest_ptr + jump_size;
        
        {
            uint16_t *dest;
            int32_t x, y;

            for (y = 0; y < y_draw_count; y++)
            {
                dest = (uint16_t*) (dest_line_start_ptr + ((dest_pitch * y * 16) >> 3));

                for (x = 0; x < x_draw_count; x++)
                {
                    uint16_t c = *dest;

                    if (c == src_key)
                    {
                        c ^= 1;
                        *dest = c;
                    }
                    dest++;
                }
            }
        }
    }
}

#define PIXEL_TYPE                  uint16_t
#define PIXEL_BYTES                 2
#define GET_PIXEL(C,PTR)            C=*((PIXEL_TYPE*)(PTR))
#define PUT_PIXEL(C,PTR)            *(PIXEL_TYPE*)(PTR) = (PIXEL_TYPE) C
#define JUMP_PIXEL(PTR,N)           PTR = (((uint8_t*)PTR)+((N)* PIXEL_BYTES))

#define GD_RESIZE_BITBLT            gd_resize_bitblt_16
#define GD_BITBLT                   gd_bitblt_16
#define GD_MEMSET                   gd_memset_16
#define GD_FILL_DOT_RECTANGLE       gd_fill_dot_rect_16
#define GD_IMAGE_BITS_DRAW          gd_image_bits_draw_16
#include "gd_primitive_internal.h"
