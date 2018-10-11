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

#ifndef DATE_PICKER_WHEEL_HPP
#define DATE_PICKER_WHEEL_HPP

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <BitmapDatabase.hpp>
#include <texts/TextKeysAndLanguages.hpp>

#include <touchgfx/mixins/MoveAnimator.hpp>

using namespace touchgfx;

/**
 * @class WheelSelector WheelSelector.hpp gui/custom_controls_screen/WheelSelector.hpp
 *
 * @brief A wheel selector. Displays a text in a scrollable wheel.
 *
 *        A wheel selector. Displays a text in a scrollable wheel. Option to display the
 *        text in a different color near the selected element. Snaps to the nearest element
 *        after movement.
 *
 * @note This widget is part of the TouchGFX Open Repository.
 *       https://github.com/draupnergraphics/touchgfx-open-repository
 *
 * @see Container
 */
class WheelSelector : public Container
{
public:
    /**
     * @fn WheelSelector::WheelSelector();
     *
     * @brief Default constructor.
     *
     *        Default constructor.
     */
    WheelSelector();

    /**
     * @fn virtual WheelSelector::~WheelSelector();
     *
     * @brief Destructor.
     *
     *        Destructor.
     */
    virtual ~WheelSelector();

    virtual void handleDragEvent(const DragEvent &evt);

    /**
     * @fn virtual void WheelSelector::setup(int width, int height, int textXPosition, int textYPosition, int spaceBetweenTextRows, int selectedYOffset, TEXTS wheelText);
     *
     * @brief Setup method.
     *
     * @param width                The width of the container.
     * @param height               The height of the container.
     * @param textXPosition        The text x coordinate position.
     * @param textYPosition        The text y coordinate position.
     * @param wheelText            The wheel text.
     */
    virtual void setup(int width, int height, int textXPosition, int textYPosition, TEXTS wheelText);

    /**
     * @fn void WheelSelector::setTextColor(colortype normalColor, colortype selectedColor, colortype selectedBackgroundColor, int selectedYOffset = 0, int selectedHeight = 0);
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
    void setTextColor(colortype normalColor, colortype selectedColor, colortype selectedBackgroundColor, int selectedYOffset = 0, int selectedHeight = 0);

    virtual void handleClickEvent(const ClickEvent &evt);
    virtual void handleGestureEvent(const GestureEvent &evt);

    /**
     * @fn int WheelSelector::getSelectedIndex();
     *
     * @brief Gets selected index.
     *
     *        Gets selected index calculated from what is on screen.
     *
     * @return The selected index.
     */
    int getSelectedIndex();

    /**
     * @fn void WheelSelector::setSelectedIndex(int index, int duration, EasingEquation equation = EasingEquations::backEaseInOut);
     *
     * @brief Sets selected index.
     *
     *        Sets selected index by moving the wheel selector using the given easing equation.
     *
     * @param index    Zero-based index of the.
     * @param duration The duration.
     * @param equation The equation.
     */
    void setSelectedIndex(int index, int duration, EasingEquation equation = EasingEquations::backEaseInOut);

    /**
     * @fn void WheelSelector::reset();
     *
     * @brief Resets this object to show first element.
     *
     *        Resets this object to show first element.
     */
    void reset();

    void setElementSelectedCallback(touchgfx::GenericCallback<const WheelSelector &, const int &> &callback)
    {
        elementSelectedAction = &callback;
    }

protected:
    enum AnimationState {
        ANIMATING_GESTURE, ///< Animating a gesture
        ANIMATING_SNAP,    ///< Animating a 'snap' after a drag/gesture to align index
        ANIMATING_DRAG,    ///< Animating a click+drag
        NO_ANIMATION       ///< No animation
    };

    AnimationState currentAnimationState;

    Container textContainer; ///< Container for wheel text
    MoveAnimator<TextArea> text; ///< Wheel text
    Container textSelectedContainer; ///< Container for selected text when selection has a different color
    MoveAnimator<TextArea> textSelected; ///<
    Box textSelectedBackground;
    int selectedAreaYOffset;

    int selectedTextYPosition;
    int spaceBetweenTextRows;
    int topCutOff;
    int bottomCutOff;

    GenericCallback<const WheelSelector &, const int &> *elementSelectedAction;

    Callback<WheelSelector, const MoveAnimator<TextArea>& > textMoveEndedCallback;
    void textMoveEndedHandler(const MoveAnimator<TextArea> &textArea);

    virtual void moveText(int newYPosition);
    virtual void animateMoveText(int newYPosition, int duration, EasingEquation equation);

    void snap();
    void adjustForBoundries(int &newYValue);
};


#endif /* DATE_PICKER_WHEEL_HPP */
