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

#include <gui/setting_screen/SetTimeView.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <gui/common/CommonUI.hpp>
#include <gui/common/CommonService.hpp>

#ifdef SIMULATOR
#include <ctime>
#ifndef _MSC_VER
#include <sys/time.h>
#endif /* _MSC_VER*/
#else
#include "FreeRTOS.h"
#include "hal_rtc.h"
#endif

SetTimeView::SetTimeView() :
    DemoView<SetTimePresenter>(),
    onSaveAndGotoSettingButtonClicked(this, &SetTimeView::saveAndGotoSettingClickedHandler)
{
    // Setup background
    background.setBitmap(Bitmap(BITMAP_TIMEPICKER_MAIN_BACKGROUND_INT_ID));
    background.setXY(0, 0);
    add(background);

    // Add title
    titleTxt.setTypedText(TypedText(T_SET_TIME_TITLE));
    titleTxt.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    titleTxt.setXY(116, 5);
    add(titleTxt);

    timePicker.setPosition(0, CommonUI::TITLE_HEIGHT, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT - CommonUI::TITLE_HEIGHT);
    timePicker.setVisible(false);
    add(timePicker);

    gotoSettingButton.setBitmaps(Bitmap(BITMAP_ICON_BACK_INT_ID), Bitmap(BITMAP_ICON_BACK_INT_ID));
    gotoSettingButton.setXY(10, 5);
    add(gotoSettingButton);

    saveAndGotoSettingButton.setBitmaps(Bitmap(BITMAP_ICON_SAVE_INT_ID), Bitmap(BITMAP_ICON_SAVE_INT_ID));
    saveAndGotoSettingButton.setXY(280, 5);
    saveAndGotoSettingButton.setAction(onSaveAndGotoSettingButtonClicked);
    add(saveAndGotoSettingButton);
}

SetTimeView::~SetTimeView()
{
}

void SetTimeView::setupScreen()
{
    // Set the chosen control visible
    timePicker.setVisible(true);
    timePicker.invalidate();
    timePicker.reset();

#ifdef SIMULATOR
    time_t rawtime;
    struct tm timenow;
    time(&rawtime);
    localtime_s(&timenow, &rawtime);

    // Set start values before animation
    timePicker.setHour(timenow.tm_hour, 0, EasingEquations::backEaseInOut);
    timePicker.setMinute(timenow.tm_min, 0, EasingEquations::backEaseInOut);

    // Start animation to a specific date
    timePicker.setHour(timenow.tm_hour, 40, EasingEquations::backEaseInOut);
    timePicker.setMinute(timenow.tm_min, 42, EasingEquations::backEaseInOut);
#else
    hal_rtc_time_t rtc_time;
    hal_rtc_status_t ret = hal_rtc_get_time(&rtc_time);
    if (HAL_RTC_STATUS_OK != ret) {
        configASSERT(0);
        return;
    }

    // Set start values before animation
    timePicker.setHour(rtc_time.rtc_hour, 0, EasingEquations::backEaseInOut);
    timePicker.setMinute(rtc_time.rtc_min, 0, EasingEquations::backEaseInOut);

    // Start animation to a specific date
    timePicker.setHour(rtc_time.rtc_hour, 40, EasingEquations::backEaseInOut);
    timePicker.setMinute(rtc_time.rtc_min, 42, EasingEquations::backEaseInOut);
#endif
}

void SetTimeView::tearDownScreen()
{
}

void SetTimeView::handleKeyEvent(uint8_t key)
{
#ifdef SIMULATOR
    SYSTEMTIME sm;
    GetLocalTime(&sm);
    sm.wHour = timePicker.getHour();
    sm.wMinute = timePicker.getMinute();
    SetLocalTime(&sm);
#else
    hal_rtc_time_t rtc_time;
    hal_rtc_status_t ret = hal_rtc_get_time(&rtc_time);
    if (HAL_RTC_STATUS_OK != ret) {
        configASSERT(0);
        return;
    }
    rtc_time.rtc_hour = timePicker.getHour();
    rtc_time.rtc_min = timePicker.getMinute();
    ret = hal_rtc_set_time(&rtc_time);
    if (HAL_RTC_STATUS_OK != ret) {
        configASSERT(0);
        return;
    }
#endif

    if (CommonService::isBacklight() == true) {
        DemoView::handleKeyEvent(key);
        return;
    }

    DemoView::handleKeyEvent(key);

    presenter->backOptionSettingSelected();
}

void SetTimeView::saveAndGotoSettingClickedHandler(const AbstractButton &button)
{
    handleKeyEvent(0);
}
