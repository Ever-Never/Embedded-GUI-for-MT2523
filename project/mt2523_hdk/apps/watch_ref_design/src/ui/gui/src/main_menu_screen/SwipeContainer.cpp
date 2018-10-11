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

#include <gui/main_menu_screen/SwipeContainer.hpp>
#include <touchgfx/EasingEquations.hpp>
#include "BitmapDatabase.hpp"
#include <touchgfx/Color.hpp>

using namespace touchgfx;

SwipeContainer::SwipeContainer() :
    currentState(NO_ANIMATION),
    numberOfScreens(0),
    animationCounter(0),
    swipeCutoff(80),
    dragX(0),
    currentScreen(0),
    endElasticWidth(30),
    screens(EAST),
    menuChangeInitiatedAction(0),
    menuChangeEndedAction(0)
{
    touchgfx::Application::getInstance()->registerTimerWidget(this);

    setTouchable(true);

    Container::add(screens);
    Container::add(dotIndicator);
}

SwipeContainer::~SwipeContainer()
{
    touchgfx::Application::getInstance()->unregisterTimerWidget(this);
}


void SwipeContainer::add(Drawable &screen)
{
    screens.add(screen);
    numberOfScreens++;

    dotIndicator.setNumberOfDots(numberOfScreens);

    setWidth(screen.getWidth());
    setHeight(screen.getHeight());
}

void SwipeContainer::setEndSwipeElasticWidth(uint16_t width)
{
    endElasticWidth = width;
}

void SwipeContainer::setSwipeCutoff(uint16_t cutoff)
{
    swipeCutoff = cutoff;
}

void SwipeContainer::setDotIndicatorBitmaps(const touchgfx::Bitmap &normalDot, const touchgfx::Bitmap &highlightedDot)
{
    dotIndicator.setBitmaps(normalDot, highlightedDot);
}

void SwipeContainer::setDotIndicatorXY(uint16_t x, uint16_t y)
{
    dotIndicator.setXY(x, y);
}

void SwipeContainer::setDotIndicatorXYWithCenteredX(uint16_t x, uint16_t y)
{
    dotIndicator.setXY(x - dotIndicator.getWidth() / 2, y);
}

void SwipeContainer::setSelectedScreen(uint8_t screenIndex)
{
    currentScreen = screenIndex;
    dotIndicator.setHighlightPosition(currentScreen);
    adjustScreens();
}

void SwipeContainer::handleTickEvent()
{
    if (currentState == ANIMATE_SWIPE_CANCELLED_LEFT) {
        animateSwipeCancelledLeft();
    } else if (currentState == ANIMATE_SWIPE_CANCELLED_RIGHT) {
        animateSwipeCancelledRight();
    } else if (currentState == ANIMATE_LEFT) {
        animateLeft();
    } else if (currentState == ANIMATE_RIGHT) {
        animateRight();
    }
}

void SwipeContainer::handleClickEvent(const ClickEvent &evt)
{
    // If an animation is already in progress do not
    // react to clicks
    if (currentState != NO_ANIMATION) {
        return;
    }

    if (evt.getType() == ClickEvent::RELEASED) {
        // Save current position for use during animation
        animateDistance = dragX;
        startX = screens.getX();

        if (dragX < 0) {
            if (currentScreen == getNumberOfScreens() - 1 || dragX > -swipeCutoff) {
                currentState = ANIMATE_SWIPE_CANCELLED_LEFT;
            } else {
                currentState = ANIMATE_LEFT;

                if (menuChangeInitiatedAction && menuChangeInitiatedAction->isValid()) {
                    menuChangeInitiatedAction->execute(*this);
                }
            }
        } else if (dragX > 0) {
            if (currentScreen == 0 || dragX < swipeCutoff) {
                currentState = ANIMATE_SWIPE_CANCELLED_RIGHT;
            } else {
                currentState = ANIMATE_RIGHT;

                if (menuChangeInitiatedAction && menuChangeInitiatedAction->isValid()) {
                    menuChangeInitiatedAction->execute(*this);
                }
            }
        }
    }
}

void SwipeContainer::handleDragEvent(const DragEvent &evt)
{
    // If an animation is already in progress do not
    // react to drags
    if (currentState != NO_ANIMATION) {
        return;
    }

    dragX += evt.getDeltaX();

    // Do not show too much background next to end screens
    if (currentScreen == 0 && dragX > endElasticWidth) {
        dragX = endElasticWidth;
    } else if (currentScreen == getNumberOfScreens() - 1 && dragX < -endElasticWidth) {
        dragX = -endElasticWidth;
    }

    adjustScreens();
}

void SwipeContainer::handleGestureEvent(const GestureEvent &evt)
{
    // Do not accept gestures while animating
    if (currentState != NO_ANIMATION) {
        return;
    }

    if (evt.getType() == evt.SWIPE_HORIZONTAL) {
        // Save current position for use during animation
        animateDistance = dragX;
        startX = screens.getX();

        if (evt.getVelocity() < 0  && currentScreen < getNumberOfScreens() - 1) {
            currentState = ANIMATE_LEFT;

            if (menuChangeInitiatedAction && menuChangeInitiatedAction->isValid()) {
                menuChangeInitiatedAction->execute(*this);
            }
        } else if (evt.getVelocity() > 0  && currentScreen > 0) {
            currentState = ANIMATE_RIGHT;

            if (menuChangeInitiatedAction && menuChangeInitiatedAction->isValid()) {
                menuChangeInitiatedAction->execute(*this);
            }
        }
    }
}

void SwipeContainer::adjustScreens()
{
    screens.moveTo(-(currentScreen * getWidth()) + dragX, 0);
}

void SwipeContainer::animateSwipeCancelledLeft()
{
    uint8_t duration = 14;

    if (animationCounter <= duration) {
        int16_t delta = EasingEquations::backEaseOut(animationCounter, 0, -animateDistance, duration);
        dragX = animateDistance + delta;

        adjustScreens();
    } else {
        // Final step: stop the animation
        currentState = NO_ANIMATION;
        animationCounter = 0;
        dragX = 0;
        adjustScreens();
    }
    animationCounter++;
}

void SwipeContainer::animateSwipeCancelledRight()
{
    uint8_t duration = 14;

    if (animationCounter <= duration) {
        int16_t delta = EasingEquations::backEaseOut(animationCounter, 0, animateDistance, duration);
        dragX = animateDistance - delta;

        adjustScreens();
    } else {
        // Final step: stop the animation
        currentState = NO_ANIMATION;
        animationCounter = 0;
        dragX = 0;
        adjustScreens();
    }
    animationCounter++;
}

void SwipeContainer::animateLeft()
{
    uint8_t duration = 10;

    if (animationCounter <= duration) {
        int16_t delta = EasingEquations::cubicEaseOut(animationCounter, 0, getWidth() + animateDistance, duration);
        dragX = animateDistance - delta;

        adjustScreens();
    } else {
        // Final step: stop the animation
        currentState = NO_ANIMATION;

        animationCounter = 0;
        currentScreen++;
        dragX = 0;
        adjustScreens();
        dotIndicator.goRight();

        if (menuChangeEndedAction && menuChangeEndedAction->isValid()) {
            menuChangeEndedAction->execute(*this);
        }
    }
    animationCounter++;
}

void SwipeContainer::animateRight()
{
    uint8_t duration = 10;

    if (animationCounter <= duration) {
        int16_t delta = EasingEquations::cubicEaseOut(animationCounter, 0, getWidth() - animateDistance, duration);
        dragX = animateDistance + delta;

        adjustScreens();
    } else {
        // Final step: stop the animation
        currentState = NO_ANIMATION;

        animationCounter = 0;
        currentScreen--;
        dragX = 0;
        adjustScreens();
        dotIndicator.goLeft();

        if (menuChangeEndedAction && menuChangeEndedAction->isValid()) {
            menuChangeEndedAction->execute(*this);
        }
    }
    animationCounter++;
}
