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

#include <gui/setting_screen/SetHeartRateView.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <gui/common/CommonUI.hpp>
#include <gui/common/CommonService.hpp>

SetHeartRateView::SetHeartRateView() :
    DemoView<SetHeartRatePresenter>(),
    onState(CommonService::getHeartRateStatus()),
    onToggleButtonClicked(this, &SetHeartRateView::toggleButtonClickedHandler)
{
    // Setup background
    background.setColor(Color::getColorFrom24BitRGB(0x35, 0x36, 0x30));
    background.setPosition(0, 0, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
    add(background);

    // Add title
    titleTxt.setTypedText(TypedText(T_HEART_RATE_TITLE));
    titleTxt.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    titleTxt.setXY(106, 5);
    add(titleTxt);

    if (onState) {
        buttonTxt.setTypedText(TypedText(T_ON_TEXT));
    } else {
        buttonTxt.setTypedText(TypedText(T_OFF_TEXT));
    }
    buttonTxt.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    buttonTxt.setXY(28, 65);
    add(buttonTxt);

    if (onState) {
        button.setBitmaps(Bitmap(BITMAP_SWITCH_ON_INT_ID), Bitmap(BITMAP_SWITCH_OFF_INT_ID));
    } else {
        button.setBitmaps(Bitmap(BITMAP_SWITCH_OFF_INT_ID), Bitmap(BITMAP_SWITCH_ON_INT_ID));
    }
    button.setAction(onToggleButtonClicked);
    button.setXY(172, 60);
    add(button);

    gotoSettingButton.setBitmaps(Bitmap(BITMAP_ICON_BACK_INT_ID), Bitmap(BITMAP_ICON_BACK_INT_ID));
    gotoSettingButton.setXY(10, 5);
    add(gotoSettingButton);
}

SetHeartRateView::~SetHeartRateView()
{
}

void SetHeartRateView::setupScreen()
{
}

void SetHeartRateView::tearDownScreen()
{
}

void SetHeartRateView::handleKeyEvent(uint8_t key)
{
    if (CommonService::isBacklight() == true) {
        DemoView::handleKeyEvent(key);
        return;
    }

    DemoView::handleKeyEvent(key);

    presenter->backOptionSettingSelected();
}

void SetHeartRateView::toggleButtonClickedHandler(const AbstractButton &button)
{
    onState = !onState;
    CommonService::setHeartRateStatus(onState);
    if (onState) {
        buttonTxt.setTypedText(TypedText(T_ON_TEXT));
    } else {
        buttonTxt.setTypedText(TypedText(T_OFF_TEXT));
    }
    buttonTxt.invalidate();
}
