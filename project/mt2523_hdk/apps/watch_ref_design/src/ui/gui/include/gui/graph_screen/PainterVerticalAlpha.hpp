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

#ifndef PAINTERVERTICALALPHA_HPP
#define PAINTERVERTICALALPHA_HPP

#include <touchgfx/hal/Types.hpp>
#include <stdint.h>
#if !defined(USE_BPP) || USE_BPP==16
#include <touchgfx/widgets/canvas/AbstractPainterRGB565.hpp>
#elif USE_BPP==24
#include <touchgfx/widgets/canvas/AbstractPainterRGB888.hpp>
#endif

using namespace touchgfx;

/**
 * @class PainterVerticalAlpha PainterVerticalAlpha.hpp gui/common/PainterVerticalAlpha.hpp
 *
 * @brief A Painter that will paint using a fading color.
 *
 *        PainterVerticalAlpha will paint using the given color at y coordinate 0 fading
 *        the color to invisible at the given y coordinate.
 *
 * @see AbstractPainter
 */
class PainterVerticalAlpha :
#if !defined(USE_BPP) || USE_BPP==16
    public AbstractPainterRGB565
#elif USE_BPP==24
    public AbstractPainterRGB888
#endif
{
public:

    /**
     * @fn PainterVerticalAlpha::PainterVerticalAlpha(colortype color = 0, int alpha0atY = 100);
     *
     * @brief Constructor.
     *
     *        Constructor.
     *
     * @param color     The color.
     * @param alpha0atY The y coordinate where alpha has faded to 0.
     */
    PainterVerticalAlpha(colortype color = 0, int alpha0atY = 100);

    /**
     * @fn void PainterVerticalAlpha::setColor(colortype color, int alphaZeroAt);
     *
     * @brief Sets color and alpha to use when drawing the CanvasWidget.
     *
     *        Sets color and alpha to use when drawing the CanvasWidget.
     *
     * @param color       The color.
     * @param alphaZeroAt The y coordinate where alpha has faded to 0.
     */
    void setColor(colortype color, int alphaZeroAt);

protected:
    /**
     * @fn virtual bool PainterVerticalAlpha::renderInit();
     *
     * @brief Get ready to render (part of) a scanline.
     *
     *        Get ready to render (part of) a scanline. Since the scanline will have the same y
     *        coordinate, the alpha for the scanline can be calculated here and used for all
     *        pixels in the scanline.
     *
     * @return true if it succeeds, false if it fails.
     */
    virtual bool renderInit();

    /**
     * @fn virtual bool PainterVerticalAlpha::renderNext(uint8_t &pixelRed, uint8_t &pixelGreen, uint8_t &pixelBlue, uint8_t &pixelAlpha);
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
    uint8_t alpha; ///< Current alpha for the scan line
    int alpha0atY; ///< The Y coordinate where alpha has faded to 0

private:
    /**
     * @fn virtual void renderPixel(uint16_t* p, uint8_t pixelRed, uint8_t pixelGreen, uint8_t pixelBlue);
     *
     * @brief Renders the pixel.
     *
     *        Renders the pixel, applying random noise to avoid banding.
     *
     * @param [in] p     Pointer to the frame buffer where the pixel should be put.
     * @param pixelRed   The red.
     * @param pixelGreen The green.
     * @param pixelBlue  The blue.
     */
    virtual void renderPixel(uint16_t *p, uint8_t pixelRed, uint8_t pixelGreen, uint8_t pixelBlue);
};

#endif /* PAINTERVERTICALALPHA_HPP */
