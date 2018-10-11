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

#ifndef PAINTERCOLOREDBITMAP_HPP
#define PAINTERCOLOREDBITMAP_HPP

#include <stdint.h>
#if !defined(USE_BPP) || USE_BPP==16
#include <touchgfx/widgets/canvas/PainterRGB565Bitmap.hpp>
#elif USE_BPP==24
#include <touchgfx/widgets/canvas/PainterRGB888Bitmap.hpp>
#endif

using namespace touchgfx;

/**
 * @class PainterColoredBitmap PainterColoredBitmap.hpp gui/graph_screen/PainterColoredBitmap.hpp
 *
 * @brief A Painter that will paint using a bitmap with a given color.
 *
 *        PainterColoredBitmap will take the color for a given point from a bitmap. The pixel
 *        from the bitmap is 'multiplied' with the given color. This means that if the given
 *        color is red (255,0,0), the blue and green parts of the image will be masked out.
 *        Interesting effects can be obtained if the given image contains colors from the
 *        entire spectrum, but usually the bitmap will be a gray scale bitmap and in this case
 *        the painter can be used to get various shades of the selected color.
 *
 * @see AbstractPainter
 */
class PainterColoredBitmap :
#if !defined(USE_BPP) || USE_BPP==16
    public PainterRGB565Bitmap
#elif USE_BPP==24
    public PainterRGB888Bitmap
#endif
{
public:
    /**
     * @fn PainterColoredBitmap::PainterColoredBitmap();
     *
     * @brief Default constructor.
     *
     *        Default constructor.
     */
    PainterColoredBitmap();

    /**
     * @fn PainterColoredBitmap::PainterColoredBitmap(colortype color, const Bitmap& bmp = Bitmap(BITMAP_INVALID), uint8_t alpha = 255);
     *
     * @brief Constructor.
     *
     *        Constructor.
     *
     * @param color The color.
     * @param bmp   The bitmap.
     * @param alpha the alpha.
     */
    PainterColoredBitmap(colortype color, const Bitmap &bmp = Bitmap(BITMAP_INVALID), uint8_t alpha = 255);

    /**
     * @fn void PainterColoredBitmap::setColor(colortype color);
     *
     * @brief Sets color and alpha to use when drawing the CanvasWidget.
     *
     *        Sets color and alpha to use when drawing the CanvasWidget.
     *
     * @param color The color.
     *
     * ### param alpha (Optional) The alpha.
     */
    void setColor(colortype color);

protected:

    /**
     * @fn virtual bool renderNext(uint16_t& pixelRed, uint8_t& pixelGreen, uint8_t& pixelBlue, uint8_t& pixelAlpha);
     *
     * @brief Gets the color and alpha for the next pixel.
     *
     *        Gets the color and alpha for the next pixel.
     *
     * @param [out] pixelRed   The red.
     * @param [out] pixelGreen The green.
     * @param [out] pixelBlue  The blue.
     * @param [out] pixelAlpha The alpha.
     *
     * @return true if it succeeds, false if it fails.
     */
    virtual bool renderNext(uint8_t &pixelRed, uint8_t &pixelGreen, uint8_t &pixelBlue, uint8_t &pixelAlpha);

    uint8_t red;   ///< The red part of the color
    uint8_t green; ///< The green part of the color
    uint8_t blue;  ///< The blue part of the color
};

#endif /* PAINTERCOLOREDBITMAP_HPP */
