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

#include <gui/graph_screen/PainterVerticalAlpha.hpp>
#include <touchgfx/Color.hpp>

using namespace touchgfx;

PainterVerticalAlpha::PainterVerticalAlpha(colortype color /*= 0*/, int alpha0atY /*= 100*/)
{
    setColor(color, alpha0atY);
}

void PainterVerticalAlpha::setColor(colortype color, int alphaZeroAt)
{
    red = Color::getRedColor(color);
    green = Color::getGreenColor(color);
    blue = Color::getBlueColor(color);
    alpha0atY = alphaZeroAt;
}

bool PainterVerticalAlpha::renderInit()
{
    int gradientAlpha = 255 - (currentY * 255 / alpha0atY);
    if (gradientAlpha < 0) {
        gradientAlpha = 0;
    }
    if (gradientAlpha > 255) {
        gradientAlpha = 255;
    }
    alpha = gradientAlpha;
    return true;
}

bool PainterVerticalAlpha::renderNext(uint8_t &pixelRed, uint8_t &pixelGreen, uint8_t &pixelBlue, uint8_t &pixelAlpha)
{
    pixelRed = this->red;
    pixelGreen = this->green;
    pixelBlue = this->blue;
    pixelAlpha = this->alpha;
    return true;
}

void PainterVerticalAlpha::renderPixel(uint16_t *p, uint8_t pixelRed, uint8_t pixelGreen, uint8_t pixelBlue)
{
#if !defined(USE_BPP) || USE_BPP==16
#define fastrand(g_seed) ((214013*(uint32_t)(g_seed)+2531011)>>16)
    uint8_t rand = fastrand((currentX + *p) * (currentY + *p));

    if (pixelRed   < 0xF8) {
        pixelRed   = pixelRed   + ((rand >> 5));    // Add bit 7-5 of rand to red to get rounding
    }
    if (pixelGreen < 0xFC) {
        pixelGreen = pixelGreen + ((rand >> 3) & 0x3);    // Add bit 4-3 of rand to green to get rounding
    }
    if (pixelBlue  < 0xF8) {
        pixelBlue  = pixelBlue  + ((rand) & 0x7);    // Add bit 2-0 of rand to blue to get rounding
    }

    AbstractPainterRGB565::renderPixel(p, pixelRed, pixelGreen, pixelBlue);
#elif USE_BPP==24
    AbstractPainterRGB888::renderPixel(p, pixelRed, pixelGreen, pixelBlue);
#endif
}
