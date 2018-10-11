/******************************************************************************
 *
 * @brief     This file is part of the TouchGFX 4.6.0 evaluation distribution.
 *
 * @author    Draupner Graphics A/S <http://www.touchgfx.com>
 *
 ******************************************************************************
 *
 * @section Copyright
 *
 * Copyright (C) 2014-2016 Draupner Graphics A/S <http://www.touchgfx.com>.
 * All rights reserved.
 *
 * TouchGFX is protected by international copyright laws and the knowledge of
 * this source code may not be used to write a similar product. This file may
 * only be used in accordance with a license and should not be re-
 * distributed in any way without the prior permission of Draupner Graphics.
 *
 * This is licensed software for evaluation use, any use must strictly comply
 * with the evaluation license agreement provided with delivery of the
 * TouchGFX software.
 *
 * The evaluation license agreement can be seen on www.touchgfx.com
 *
 * @section Disclaimer
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Draupner Graphics A/S has
 * no obligation to support this software. Draupner Graphics A/S is providing
 * the software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Draupner Graphics A/S can not be held liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this software.
 *
 *****************************************************************************/
#ifndef PAINTERGRAY2_HPP
#define PAINTERGRAY2_HPP

#include <stdint.h>
#include <touchgfx/widgets/canvas/AbstractPainterGRAY2.hpp>
#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * @class PainterGRAY2 PainterGRAY2.hpp touchgfx/widgets/canvas/PainterGRAY2.hpp
 *
 * @brief A Painter that will paint using a color and an alpha value.
 *
 *        The PainterGRAY2 class allows a shape to be filled with a given color and alpha
 *        value. This allows transparent, anti-aliased elements to be drawn.
 *
 * @see AbstractPainter
 */
class PainterGRAY2 : public AbstractPainterGRAY2
{
public:

    /**
     * @fn PainterGRAY2::PainterGRAY2(colortype color = 0, uint8_t alpha = 255);
     *
     * @brief Constructor.
     *
     *        Constructor.
     *
     * @param color the color.
     * @param alpha the alpha.
     */
    PainterGRAY2(colortype color = 0, uint8_t alpha = 255);

    /**
     * @fn void PainterGRAY2::setColor(colortype color, uint8_t alpha = 255);
     *
     * @brief Sets color and alpha to use when drawing the CanvasWidget.
     *
     *        Sets color and alpha to use when drawing the CanvasWidget.
     *
     * @param color The color.
     * @param alpha The alpha.
     */
    void setColor(colortype color, uint8_t alpha = 255);

protected:
    virtual bool renderNext(uint8_t& gray, uint8_t& alpha);

    uint8_t painterGray;  ///< The grey color
    uint8_t painterAlpha; ///< The alpha value

}; // class PainterGRAY2

} // namespace touchgfx

#endif // PAINTERGRAY2_HPP
