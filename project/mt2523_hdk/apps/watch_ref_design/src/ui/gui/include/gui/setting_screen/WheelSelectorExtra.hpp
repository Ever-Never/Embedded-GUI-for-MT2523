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

#ifndef DATE_PICKER_WHEEL_DAY_HPP
#define DATE_PICKER_WHEEL_DAY_HPP

#include <gui/setting_screen/WheelSelector.hpp>
#include <touchgfx/mixins/FadeAnimator.hpp>

using namespace touchgfx;

/**
 * @class WheelSelectorExtra WheelSelectorExtra.hpp gui/custom_controls_screen/WheelSelectorExtra.hpp
 *
 * @brief A wheel selector that introduces a number of extra text areas in the bottom of the date
 *        picker.
 *
 *        A wheel selector that introduces a number of extra text areas in the bottom of the
 *        date picker. This allows the contents of the wheel selector to be changed.
 *
 * @note This widget is part of the TouchGFX Open Repository.
 *       https://github.com/draupnergraphics/touchgfx-open-repository
 *
* @see WheelSelector
 */
class WheelSelectorExtra : public WheelSelector
{
public:
    /**
     * @fn WheelSelectorDay::WheelSelectorDay();
     *
     * @brief Default constructor.
     *
     *        Default constructor.
     */
    WheelSelectorExtra();

    /**
     * @fn virtual WheelSelectorDay::~WheelSelectorDay();
     *
     * @brief Destructor.
     *
     *        Destructor.
     */
    virtual ~WheelSelectorExtra();

    /**
     * @fn void WheelSelectorExtra::setupDatePickerWheelDay(int width, int height, int textXPosition, int textYPositionOfIndex0Selected, TEXTS wheelText, TEXTS wheelTextExtra0, TEXTS wheelTextExtra1, TEXTS wheelTextExtra2);
     *
     * @brief Sets up the date picker wheel.
     *
     *        Sets up the date picker wheel.
     *
     * @param width                         The width.
     * @param height                        The height.
     * @param textXPosition                 The text x coordinate position.
     * @param textYPositionOfIndex0Selected The text y coordinate position of index 0 selected.
     * @param wheelText                     The wheel text.
     * @param wheelTextExtra0               The wheel extra text 1.
     * @param wheelTextExtra1               The wheel extra text 2.
     * @param wheelTextExtra2               The wheel extra text 3.
     */
    void setupDatePickerWheelDay(int width, int height, int textXPosition, int textYPositionOfIndex0Selected, TEXTS wheelText, TEXTS wheelTextExtra0, TEXTS wheelTextExtra1, TEXTS wheelTextExtra2);

    /**
     * @fn virtual void WheelSelectorExtra::setTextColor(colortype normalColor, colortype selectedColor, colortype selectedBackgroundColor, int selectedYOffset = 0, int selectedHeight = 0);
     *
     * @brief Sets text color of normal/selected text.
     *
     *        Sets text color of normal/selected text. If the colors are the same, the overlays
     *        for showing selected text is hidden, otherwise it is setup with the given y
     *        coordinate offset.
     *
     * @param normalColor             The color of normal text.
     * @param selectedColor           The color of selected text.
     * @param selectedBackgroundColor The selected background color.
     * @param selectedYOffset         The selected area (where the text changes color) y coordinate
     *                                offset.
     * @param selectedHeight          The selected height.
     */
    virtual void setTextColor(colortype normalColor, colortype selectedColor, colortype selectedBackgroundColor, int selectedYOffset = 0, int selectedHeight = 0);

    /**
     * @fn void WheelSelectorExtra::fadeExtraText(int extraTextIndex, uint8_t endAlpha, uint16_t duration);
     *
     * @brief Fade extra texts.
     *
     *        Fade extra texts so the text list is smoothly extended or retracted.
     *
     * @param extraTextIndex Zero-based index of the extra text.
     * @param endAlpha       The end alpha.
     * @param duration       The duration.
     */
    void fadeExtraText(int extraTextIndex, uint8_t endAlpha, uint16_t duration);

protected:
    static const int NUMBER_OF_EXTRA_TEXTS = 3;

    FadeAnimator<MoveAnimator<TextArea> > textExtra[NUMBER_OF_EXTRA_TEXTS];
    FadeAnimator<MoveAnimator<TextArea> > textExtraSelected[NUMBER_OF_EXTRA_TEXTS];
    bool textExtraIsVisible[NUMBER_OF_EXTRA_TEXTS];

    virtual void moveText(int newYPosition);
    virtual void animateMoveText(int newYPosition, int duration, EasingEquation equation);
    void adjustBottomCutOff();
};


#endif /* DATE_PICKER_WHEEL_DAY_HPP */
