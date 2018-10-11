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

#include <gui/setting_screen/WheelSelectorExtra.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/EasingEquations.hpp>

WheelSelectorExtra::WheelSelectorExtra() :
    WheelSelector()
{
}

WheelSelectorExtra::~WheelSelectorExtra()
{
}

void WheelSelectorExtra::setupDatePickerWheelDay(int width, int height, int textXPosition, int textYPositionOfIndex0Selected, TEXTS wheelText, TEXTS wheelTextExtra0, TEXTS wheelTextExtra1, TEXTS wheelTextExtra2)
{
    WheelSelector::setup(width, height, textXPosition, textYPositionOfIndex0Selected, wheelText);

    textExtra[0].setTypedText(TypedText(wheelTextExtra0));
    textExtra[1].setTypedText(TypedText(wheelTextExtra1));
    textExtra[2].setTypedText(TypedText(wheelTextExtra2));

    textExtra[0].setXY(text.getX(), text.getRect().bottom());
    textExtra[1].setXY(text.getX(), textExtra[0].getRect().bottom());
    textExtra[2].setXY(text.getX(), textExtra[1].getRect().bottom());

    for (int i = 0; i < NUMBER_OF_EXTRA_TEXTS; i++) {
        textExtraIsVisible[i] = true;
        textExtra[i].setMoveAnimationEndedAction(textMoveEndedCallback);
        textContainer.add(textExtra[i]);

        textExtraSelected[i].setTypedText(textExtra[i].getTypedText());
        textExtraSelected[i].setX(textExtra[i].getX());
        textExtraSelected[i].setMoveAnimationEndedAction(textMoveEndedCallback);
        textSelectedContainer.add(textExtraSelected[i]);
    }

    adjustBottomCutOff();
    reset();
}

void WheelSelectorExtra::setTextColor(colortype normalColor, colortype selectedColor, colortype selectedBackgroundColor, int selectedYOffset /*= 0*/, int selectedHeight /*= 0*/)
{
    WheelSelector::setTextColor(normalColor, selectedColor, selectedBackgroundColor, selectedYOffset, selectedHeight);
    for (int i = 0; i < NUMBER_OF_EXTRA_TEXTS; i++) {
        textExtra[i].setColor(text.getColor());
        textExtraSelected[i].setY(textExtra[i].getY() - selectedYOffset);
        textExtraSelected[i].setColor(textSelected.getColor());
    }
}

void WheelSelectorExtra::moveText(int newYPosition)
{
    adjustForBoundries(newYPosition);

    text.moveTo(text.getX(), newYPosition);
    textExtra[0].moveTo(text.getX(), newYPosition + text.getHeight());
    textExtra[1].moveTo(text.getX(), newYPosition + text.getHeight() + textExtra[0].getHeight());
    textExtra[2].moveTo(text.getX(), newYPosition + text.getHeight() + textExtra[0].getHeight() + textExtra[1].getHeight());

    textSelected.moveTo(text.getX(), newYPosition - selectedAreaYOffset);
    textExtraSelected[0].moveTo(text.getX(), textExtra[0].getY() - selectedAreaYOffset);
    textExtraSelected[1].moveTo(text.getX(), textExtra[1].getY() - selectedAreaYOffset);
    textExtraSelected[2].moveTo(text.getX(), textExtra[2].getY() - selectedAreaYOffset);
}

void WheelSelectorExtra::animateMoveText(int newYPosition, int duration, EasingEquation equation)
{
    text.startMoveAnimation(text.getX(), newYPosition, duration, EasingEquations::linearEaseNone, equation);
    textExtra[0].startMoveAnimation(text.getX(), newYPosition + text.getHeight(), duration, EasingEquations::linearEaseNone, equation);
    textExtra[1].startMoveAnimation(text.getX(), newYPosition + text.getHeight() + textExtra[0].getHeight(), duration, EasingEquations::linearEaseNone, equation);
    textExtra[2].startMoveAnimation(text.getX(), newYPosition + text.getHeight() + textExtra[0].getHeight() + textExtra[1].getHeight(), duration, EasingEquations::linearEaseNone, equation);

    textSelected.startMoveAnimation(text.getX(), newYPosition - selectedAreaYOffset, duration, EasingEquations::linearEaseNone, equation);
    textExtraSelected[0].startMoveAnimation(text.getX(), newYPosition + text.getHeight() - selectedAreaYOffset, duration, EasingEquations::linearEaseNone, equation);
    textExtraSelected[1].startMoveAnimation(text.getX(), newYPosition + text.getHeight() + textExtra[0].getHeight() - selectedAreaYOffset, duration, EasingEquations::linearEaseNone, equation);
    textExtraSelected[2].startMoveAnimation(text.getX(), newYPosition + text.getHeight() + textExtra[0].getHeight() + textExtra[1].getHeight() - selectedAreaYOffset, duration, EasingEquations::linearEaseNone, equation);
}

void WheelSelectorExtra::fadeExtraText(int extraTextIndex, uint8_t endAlpha, uint16_t duration)
{
    assert(extraTextIndex < NUMBER_OF_EXTRA_TEXTS);

    textExtra[extraTextIndex].startFadeAnimation(endAlpha, duration);
    textExtraSelected[extraTextIndex].startFadeAnimation(endAlpha, duration);
    textExtraIsVisible[extraTextIndex] = (endAlpha != 0);

    adjustBottomCutOff();
}

void WheelSelectorExtra::adjustBottomCutOff()
{
    bottomCutOff = topCutOff - (textExtra[2].getRect().bottom() - text.getY()) - (spaceBetweenTextRows / 3);
    for (int i = 0; i < NUMBER_OF_EXTRA_TEXTS; i++) {
        if (!textExtraIsVisible[i]) {
            bottomCutOff = topCutOff - (textExtra[i].getY() - text.getY()) - (spaceBetweenTextRows / 3);
            break;
        }
    }
}
