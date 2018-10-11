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

#include <gui/setting_screen/SetDateView.hpp>
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

SetDateView::SetDateView() :
    DemoView<SetDatePresenter>(),
    onSaveAndGotoSettingButtonClicked(this, &SetDateView::saveAndGotoSettingClickedHandler)
{
    // Setup background
    background.setBitmap(Bitmap(BITMAP_DATEPICKER_MAIN_BACKGROUND_INT_ID));
    background.setXY(0, 0);
    add(background);

    // Add title
    titleTxt.setTypedText(TypedText(T_SET_DATE_TITLE));
    titleTxt.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    titleTxt.setXY(118, 5);
    add(titleTxt);

    datePicker.setPosition(0, CommonUI::TITLE_HEIGHT, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT - CommonUI::TITLE_HEIGHT);
    datePicker.setVisible(false);
    add(datePicker);

    gotoSettingButton.setBitmaps(Bitmap(BITMAP_ICON_BACK_INT_ID), Bitmap(BITMAP_ICON_BACK_INT_ID));
    gotoSettingButton.setXY(10, 5);
    add(gotoSettingButton);

    saveAndGotoSettingButton.setBitmaps(Bitmap(BITMAP_ICON_SAVE_INT_ID), Bitmap(BITMAP_ICON_SAVE_INT_ID));
    saveAndGotoSettingButton.setXY(280, 5);
    saveAndGotoSettingButton.setAction(onSaveAndGotoSettingButtonClicked);
    add(saveAndGotoSettingButton);
}

SetDateView::~SetDateView()
{
}

void SetDateView::setupScreen()
{
    // Set the chosen control visible
    datePicker.setVisible(true);
    datePicker.invalidate();
    datePicker.reset();

#ifdef SIMULATOR
    time_t rawtime;
    struct tm timenow;
    time(&rawtime);
    localtime_s(&timenow, &rawtime);

    // Set start values before animation
    datePicker.setDay(timenow.tm_mday - 1, 0, EasingEquations::backEaseInOut);
    datePicker.setMonth(timenow.tm_mon, 0, EasingEquations::backEaseInOut);
    datePicker.setYear(timenow.tm_year - 112, 0, EasingEquations::backEaseInOut);

    // Start animation to a specific date
    datePicker.setDay(timenow.tm_mday - 1, 40, EasingEquations::backEaseInOut);
    datePicker.setMonth(timenow.tm_mon, 42, EasingEquations::backEaseInOut);
    datePicker.setYear(timenow.tm_year - 112, 38, EasingEquations::backEaseInOut);
#else
    hal_rtc_time_t rtc_time;
    hal_rtc_status_t ret = hal_rtc_get_time(&rtc_time);
    if (HAL_RTC_STATUS_OK != ret) {
        configASSERT(0);
        return;
    }

    // Set start values before animation
    datePicker.setDay(rtc_time.rtc_day - 1, 0, EasingEquations::backEaseInOut);
    datePicker.setMonth(rtc_time.rtc_mon - 1, 0, EasingEquations::backEaseInOut);
    datePicker.setYear(rtc_time.rtc_year, 0, EasingEquations::backEaseInOut);

    // Start animation to a specific date
    datePicker.setDay(rtc_time.rtc_day - 1, 40, EasingEquations::backEaseInOut);
    datePicker.setMonth(rtc_time.rtc_mon - 1, 42, EasingEquations::backEaseInOut);
    datePicker.setYear(rtc_time.rtc_year, 38, EasingEquations::backEaseInOut);
#endif
}

void SetDateView::tearDownScreen()
{
}

void SetDateView::handleKeyEvent(uint8_t key)
{
#ifdef SIMULATOR
    SYSTEMTIME sm;
    GetLocalTime(&sm);
    sm.wYear = 2012 + datePicker.getYear();
    sm.wMonth = 1 + datePicker.getMonth();
    sm.wDay = 1 + datePicker.getDay();
    SetLocalTime(&sm);
#else
    hal_rtc_time_t rtc_time;
    hal_rtc_status_t ret = hal_rtc_get_time(&rtc_time);
    if (HAL_RTC_STATUS_OK != ret) {
        configASSERT(0);
        return;
    }
    rtc_time.rtc_year = datePicker.getYear();
    rtc_time.rtc_mon = datePicker.getMonth() + 1;
    rtc_time.rtc_day = datePicker.getDay() + 1;
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

void SetDateView::saveAndGotoSettingClickedHandler(const AbstractButton &button)
{
    handleKeyEvent(0);
}
