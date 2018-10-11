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

#include <gui/main_menu_screen/MainMenuView.hpp>
#include <BitmapDatabase.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
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
#include "syslog.h"

log_create_module(tgfx, PRINT_LEVEL_INFO);
#endif

MainMenuView::MainMenuView() :
    clickAbort(false),
    currentSecondCounter(0),
    currentMinuteCounter(0),
    currentHourCounter(0),
    transitionDone(false),
    firstPresenterCall(true),
    graphX(0),
    pointCounter(0),
    showFirstTimeFlag(0),
    graphInterval(0),
    averageHeartRate(0),
    totalHeartRate(0),
    maxHeartRate(0),
    minHeartRate(0),
    effectiveHeatrRateNum(0)
{
}

void MainMenuView::setupScreen()
{
    backgroundLeft.setBitmap(Bitmap(BITMAP_MENU_STAGE_STRETCH_LEFT_SIDE_INT_ID));
    backgroundLeft.setXY(0, 0);
    add(backgroundLeft);

    backgroundRight.setBitmap(Bitmap(BITMAP_MENU_STAGE_STRETCH_RIGHT_SIDE_INT_ID));
    backgroundRight.setXY(HAL::DISPLAY_WIDTH - backgroundRight.getWidth(), 0);
    add(backgroundRight);

    menuContainer.setXY(0, 0);
    menuContainer.setSwipeCutoff(80);
    menuContainer.setEndSwipeElasticWidth(50);
    menuContainer.setDotIndicatorBitmaps(Bitmap(BITMAP_SCREEN_SWIPE_DOTS_INACTIVE_INT_ID), Bitmap(BITMAP_SCREEN_SWIPE_DOTS_ACTIVE_INT_ID));
    menuContainer.setDotIndicatorXYWithCenteredX(HAL::DISPLAY_WIDTH / 2 - 25, HAL::DISPLAY_HEIGHT - 15);
    menuContainer.setTouchable(false);

    demoScreens[MainMenuPresenter::CLOCK_FACE].setBitmap(Bitmap(BITMAP_MENU_CLOCK_BACKGROUND_INT_ID));
    demoScreens[MainMenuPresenter::HEART_RATE].setBitmap(Bitmap(BITMAP_MENU_HEART_RATE_BACKGROUND_INT_ID));
    demoScreens[MainMenuPresenter::SETTINGS].setBitmap(Bitmap(BITMAP_MENU_SETTING_BACKGROUND_INT_ID));

    for (int i = MainMenuPresenter::CLOCK_FACE; i < NUMBER_OF_DEMO_SCREENS; i++)
    {
        demoScreensContainer[i].setPosition(0, 0, demoScreens[i].getWidth(), demoScreens[i].getHeight());
        demoScreensContainer[i].add(demoScreens[i]);

        TEXTS titleID[NUMBER_OF_DEMO_SCREENS] = { T_CLOCK_TITLE, T_HEART_RATE_TITLE, T_SETTINGS_TITLE };
        int titleX[NUMBER_OF_DEMO_SCREENS] = { 132, 106, 119 };

        titleTxt[i].setTypedText(TypedText(titleID[i]));
        titleTxt[i].setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
        titleTxt[i].setXY(titleX[i], 5);
        demoScreensContainer[i].add(titleTxt[i]);

        battery[i].setBitmap(Bitmap(BITMAP_ICON_BATTERY_INT_ID));
        battery[i].setXY(280, 5);
        demoScreensContainer[i].add(battery[i]);

        if (i == MainMenuPresenter::CLOCK_FACE)
        {
            addClockFaceScreen();
        }
        else if (i == MainMenuPresenter::HEART_RATE)
        {
            addHeartRateScreen();
        }

        menuContainer.add(demoScreensContainer[i]);
    }

    add(menuContainer);

    demoScreenIndex[0] = MainMenuPresenter::CLOCK_FACE;
    demoScreenIndex[1] = MainMenuPresenter::HEART_RATE;
    demoScreenIndex[2] = MainMenuPresenter::SETTINGS;
}

void MainMenuView::tearDownScreen()
{
}

void MainMenuView::afterTransition()
{
    menuContainer.setTouchable(true);
    transitionDone = true;
}

void MainMenuView::handleClickEvent(const ClickEvent &evt)
{
    DemoView::handleClickEvent(evt);

    if (evt.getType() == ClickEvent::RELEASED) {
        if (clickAbort == false) {
            gotoSelectedDemoScreen(menuContainer.getSelectedScreen());
        } else {
            clickAbort = false;
        }
    }
}

void MainMenuView::handleDragEvent(const DragEvent &evt)
{
    DemoView::handleDragEvent(evt);

    if (abs(evt.getDeltaX()) > 0 || abs(evt.getDeltaY()) > 0) {
        clickAbort = true;
    }
}

void MainMenuView::handleKeyEvent(uint8_t key)
{
    if (CommonService::isBacklight() == true) {
        DemoView::handleKeyEvent(key);
        return;
    }

    DemoView::handleKeyEvent(key);
}

void MainMenuView::handleTimeUpdated(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    currentSecondCounter = seconds;
    currentMinuteCounter = minutes;
    //currentHourCounter counts from 0-59 due to the number of images of the hour hand
    currentHourCounter = ((hours % 12) * 5) + (minutes / 12);

    updateClock(currentHourCounter, currentMinuteCounter, currentSecondCounter);
}

void MainMenuView::updateClock(uint8_t hourCounter, uint8_t minuteCounter, uint8_t secondCounter)
{
    // Updating the clock before transition to the page is done will cause the hands to move unintended.
    // We do allow the first call from the presenter in that case where we do not animate from start.
    if (transitionDone || firstPresenterCall)
    {
        if (firstPresenterCall)
        {
            // To avoid jump in seconds when opening the view
            secondCounter++;
            firstPresenterCall = false;
        }

        secondHand.updateAngle(secondCounter * 6);

        minuteHand.updateAngle(minuteCounter * 6 + secondCounter / 10);

        hourHand.updateAngle(hourCounter * 6 + minuteCounter / 10);

        year.invalidate();
        month.invalidate();
        day.invalidate();
    }
}

void MainMenuView::gotoSelectedDemoScreen(uint8_t demoIndex)
{
    presenter->demoSelected(demoScreenIndex[demoIndex], demoIndex);
}

void MainMenuView::setSelectedMenuIndex(uint8_t demoIndex)
{
    menuContainer.setSelectedScreen(demoIndex);
}

void MainMenuView::addClockFaceScreen()
{
    int clockIndex = MainMenuPresenter::CLOCK_FACE;

#ifdef SIMULATOR
    SYSTEMTIME sm;
    GetLocalTime(&sm);
#else
    log_config_print_switch(hal, DEBUG_LOG_OFF);
    hal_rtc_time_t rtc_time;
    hal_rtc_status_t ret = hal_rtc_get_time(&rtc_time);
    if (HAL_RTC_STATUS_OK != ret) {
        LOG_E(tgfx, "hal_rtc_get_time = %d", ret);
        configASSERT(0);
        return;
    }
    LOG_I(tgfx, "rtc_year = %d, rtc_mon = %d, rtc_day = %d", rtc_time.rtc_year, rtc_time.rtc_mon, rtc_time.rtc_day);
    LOG_I(tgfx, "rtc_hour = %d, rtc_min = %d, rtc_sec = %d", rtc_time.rtc_hour, rtc_time.rtc_min, rtc_time.rtc_sec);
#endif

#ifdef SIMULATOR
    Unicode::snprintf(yearValueBuffer, 5, "%d", sm.wYear);
#else
    Unicode::snprintf(yearValueBuffer, 5, "%d", rtc_time.rtc_year + 2012);
#endif
    year.setWildcard(yearValueBuffer);
    year.setTypedText(TypedText(T_CLOCK_YEARS));
    year.setColor(Color::getColorFrom24BitRGB(0x99, 0x9A, 0x94));
    year.setXY(140, 110);
    demoScreensContainer[clockIndex].add(year);

    TEXTS stringID[12] = {
        T_CLOCK_MONTHS_1, T_CLOCK_MONTHS_2, T_CLOCK_MONTHS_3, T_CLOCK_MONTHS_4, T_CLOCK_MONTHS_5, T_CLOCK_MONTHS_6,
        T_CLOCK_MONTHS_7, T_CLOCK_MONTHS_8, T_CLOCK_MONTHS_9, T_CLOCK_MONTHS_10, T_CLOCK_MONTHS_11, T_CLOCK_MONTHS_12 };
#ifdef SIMULATOR
    month.setTypedText(TypedText(stringID[sm.wMonth - 1]));
#else
    month.setTypedText(TypedText(stringID[rtc_time.rtc_mon - 1]));
#endif
    month.setColor(Color::getColorFrom24BitRGB(0xEE, 0xEC, 0xEA));
    month.setXY(90, 152);
    demoScreensContainer[clockIndex].add(month);

#ifdef SIMULATOR
    Unicode::snprintf(dayValueBuffer, 3, "%02d", sm.wDay);
#else
    Unicode::snprintf(dayValueBuffer, 3, "%02d", rtc_time.rtc_day);
#endif
    day.setWildcard(dayValueBuffer);
    day.setTypedText(TypedText(T_CLOCK_DAYS));
    day.setColor(Color::getColorFrom24BitRGB(0xEE, 0xEC, 0xEA));
    day.setXY(206, 154);
    demoScreensContainer[clockIndex].add(day);

    handColorRed.setColor(Color::getColorFrom24BitRGB(0xFD, 0x5E, 0x44));
    handColorWhite.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));

    secondHand.setPosition(0, 40, 320, 320);
    secondHand.setOrigin(160, 129);
    secondHand.setPainter(handColorRed);
    demoScreensContainer[clockIndex].add(secondHand);

    minuteHand.setPosition(0, 40, 320, 320);
    minuteHand.setOrigin(160, 129);
    minuteHand.setPainter(handColorWhite);
    demoScreensContainer[clockIndex].add(minuteHand);

    hourHand.setPosition(0, 40, 320, 320);
    hourHand.setOrigin(160, 129);
    hourHand.setPainter(handColorWhite);
    demoScreensContainer[clockIndex].add(hourHand);

    centerDotBG.setBitmap(Bitmap(BITMAP_CLOCK_CENTER_BACKGROUND_INT_ID));
    centerDotBG.setXY(154, 163);
    demoScreensContainer[clockIndex].add(centerDotBG);

    centerDot.setBitmap(Bitmap(BITMAP_CLOCK_CENTER_DOT_INT_ID));
    centerDot.setXY(158, 167);
    demoScreensContainer[clockIndex].add(centerDot);
}

void MainMenuView::addHeartRateScreen()
{
    int heartRateIndex = MainMenuPresenter::HEART_RATE;

    // heart rate icon
    HeartRateIcon.setBitmap(Bitmap(BITMAP_ICON_LARGE_HEART_INT_ID));
    HeartRateIcon.setXY(120, 64);
    HeartRateIcon.setAlpha(0x3B);
    demoScreensContainer[heartRateIndex].add(HeartRateIcon);

    // average heartRate
    Unicode::snprintf(averageHRDatabuf, 5, "%d", averageHeartRate);
    averageHRDataTxt.setTypedText(TypedText(T_HEARTRATE_AVERAGE_VALUE));
    averageHRDataTxt.setWildcard(averageHRDatabuf);
    averageHRDataTxt.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    averageHRDataTxt.resizeToCurrentText();
    averageHRDataTxt.setXY(20, 40);
    demoScreensContainer[heartRateIndex].add(averageHRDataTxt);

    // Max Heart rate
    strMaxHeart.setTypedText(TypedText(T_HEARTRATE_MAX_STRING));
    strMaxHeart.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    strMaxHeart.setXY(206, 60);
    demoScreensContainer[heartRateIndex].add(strMaxHeart);

    // Min Heart rate
    strMinHeart.setTypedText(TypedText(T_HEARTRATE_MIN_STRING));
    strMinHeart.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    strMinHeart.setXY(206, 82);
    demoScreensContainer[heartRateIndex].add(strMinHeart);

    // Max Heart rate val
    Unicode::snprintf(maxHRDatabuf, 5, "%d", maxHeartRate);
    maxHRDataTxt.setTypedText(TypedText(T_HEARTRATE_MAX_STRING_VALUE));
    maxHRDataTxt.setWildcard(maxHRDatabuf);
    maxHRDataTxt.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    maxHRDataTxt.resizeToCurrentText();
    maxHRDataTxt.setXY(strMaxHeart.getRect().right() + 10, strMaxHeart.getRect().bottom() - strMaxHeart.getRect().height);
    demoScreensContainer[heartRateIndex].add(maxHRDataTxt);

    // Min Heart rate val
    Unicode::snprintf(minHRDatabuf, 5, "%d", minHeartRate);
    minHRDataTxt.setTypedText(TypedText(T_HEARTRATE_MIN_STRING_VALUE));
    minHRDataTxt.setWildcard(minHRDatabuf);
    minHRDataTxt.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    minHRDataTxt.resizeToCurrentText();
    minHRDataTxt.setXY(strMinHeart.getRect().right() + 12, strMinHeart.getRect().bottom() - strMinHeart.getRect().height);
    demoScreensContainer[heartRateIndex].add(minHRDataTxt);

    // graphic background
    int graphXOffset = 20;
    int graphYOffset = 10;
    graphArea.setPosition(0, averageHRDataTxt.getRect().bottom(), HAL::DISPLAY_WIDTH - graphXOffset, HAL::DISPLAY_HEIGHT - averageHRDataTxt.getRect().bottom() - 40);

    int graphWidth = graphArea.getRect().width;
    int graphHeight = graphArea.getRect().height;

    int eachRowData = graphHeight / NUMBER_OF_GRID_LINES;
    int logicalREachRowData = (MainMenuPresenter::HeartRateUpper - MainMenuPresenter::HeartRateLower) / (NUMBER_OF_GRID_LINES - 1);

    for (int i = 0; i < NUMBER_OF_GRID_LINES; i++)
    {
        graphGridLines[i].setColor(Color::getColorFrom24BitRGB(0xE1, 0xE4, 0xE6));
        // show interval line
        graphGridLines[i].setPosition(graphXOffset + 10, eachRowData * (i + 1), graphWidth, 1);
        graphArea.add(graphGridLines[i]);
        // column value
        Unicode::snprintf(graphYValuesbuf[i], 5, "%d", MainMenuPresenter::HeartRateUpper -  i * logicalREachRowData);
        graphYValues[i].setTypedText(TypedText(T_GRAPH_Y_VALUE));
        graphYValues[i].setWildcard(graphYValuesbuf[i]);
        graphYValues[i].setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
        graphYValues[i].resizeToCurrentText();
        graphYValues[i].setXY(graphGridLines[i].getX() - 25, graphGridLines[i].getY() - 15);
        graphArea.add(graphYValues[i]);
    }

    primaryGraph.setXY(graphXOffset, graphYOffset);
    primaryGraph.setup(graphWidth, graphHeight, Color::getColorFrom24BitRGB(0xFF, 0xFF, 0x10), graphBackground.getColor());
    primaryGraph.setDotShape(0, 20, 3);
    primaryGraph.setDotBackgroundShape(0, 20, 5);
    graphArea.add(primaryGraph);

    // graphArea background
    demoScreensContainer[heartRateIndex].add(graphArea);
    graphX = graphXOffset;
    graphInterval = primaryGraph.getWidth() / Data_Save_Num;

    initHeartRateChracterastic();
    initPrimaryGraph();
}

void MainMenuView::initHeartRateChracterastic()
{
    // show max data
    Unicode::snprintf(maxHRDatabuf, 5, "%d", maxHeartRate);
    maxHRDataTxt.setTypedText(TypedText(T_HEARTRATE_MAX_STRING_VALUE));
    maxHRDataTxt.setWildcard(maxHRDatabuf);
    maxHRDataTxt.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    maxHRDataTxt.resizeToCurrentText();

    // show min data
    Unicode::snprintf(minHRDatabuf, 5, "%d", minHeartRate);
    minHRDataTxt.setTypedText(TypedText(T_HEARTRATE_MIN_STRING_VALUE));
    minHRDataTxt.setWildcard(minHRDatabuf);
    minHRDataTxt.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));
    minHRDataTxt.resizeToCurrentText();

    // show average data
    if (0 != effectiveHeatrRateNum)
    {
        averageHeartRate = totalHeartRate / effectiveHeatrRateNum;
    }
    Unicode::snprintf(averageHRDatabuf, 5, "%d", averageHeartRate);
    averageHRDataTxt.setTypedText(TypedText(T_HEARTRATE_AVERAGE_VALUE));
    averageHRDataTxt.setWildcard(averageHRDatabuf);
    averageHRDataTxt.resizeToCurrentText();
}

void MainMenuView::initPrimaryGraph()
{
    int index = 0;
    showFirstTimeFlag = 0;
    for (index = 0; index < Data_Save_Num; ++index)
    {
        data[index][0] = graphX;
        data[index][1] = VALIDATE_HR_DATA_Y_VALUE;
        data[index][2] = 0;
        graphX += graphInterval;
        //primaryGraph.addValue(data[index][0], data[index][1]);
        //pointCounter++;
    }
    graphX = 20;
    showFirstTimeFlag = 1;
}

void MainMenuView::refreshHeartRateChracterastic()
{
    // show max data
    Unicode::snprintf(maxHRDatabuf, 5, "%d", maxHeartRate);
    maxHRDataTxt.resizeToCurrentText();
    maxHRDataTxt.invalidate();

    // show min data
    Unicode::snprintf(minHRDatabuf, 5, "%d", minHeartRate);
    minHRDataTxt.resizeToCurrentText();
    minHRDataTxt.invalidate();

    // show average data
    if (0 != effectiveHeatrRateNum)
    {
        averageHeartRate = totalHeartRate / effectiveHeatrRateNum;
    }
    Unicode::snprintf(averageHRDatabuf, 5, "%d", averageHeartRate);
    averageHRDataTxt.resizeToCurrentText();
    averageHRDataTxt.invalidate();
}

void MainMenuView::resetPrimaryGraph()
{
    initPrimaryGraph();
    //primaryGraph.removeAll();
    primaryGraph.clear();
    primaryGraph.invalidate();
    pointCounter = 0;
}

void MainMenuView::addHeartRateData(int y, int physicalData)
{
    int index = 0;
    int firstPhysicalHeartRateData = 0;
    int firstLogicalHeartRateData = 0;
    // move all data from ahead by one element
    if (pointCounter == Data_Save_Num)
    {
        resetPrimaryGraph();
    }

    data[pointCounter][1] = y;
    data[pointCounter][2] = physicalData;
    primaryGraph.addValue(data[pointCounter][0], data[pointCounter][1]);

    index = pointCounter;
    ++pointCounter;

    // caculate charateristic
    firstPhysicalHeartRateData = data[index][2];
    firstLogicalHeartRateData = data[index][1];
    if (VALIDATE_HR_DATA_Y_VALUE != firstLogicalHeartRateData)
    {
        ++effectiveHeatrRateNum;
        totalHeartRate += firstPhysicalHeartRateData;
        maxHeartRate = maxHeartRate < firstPhysicalHeartRateData ? firstPhysicalHeartRateData : maxHeartRate;
        if (0 != minHeartRate)
        {
            minHeartRate = minHeartRate > firstPhysicalHeartRateData ? firstPhysicalHeartRateData : minHeartRate;
        }
        else
        {
            minHeartRate = firstPhysicalHeartRateData;
        }
    }
}

void MainMenuView::refreshGraphs(int y, int physicalData)
{
    addHeartRateData(y, physicalData);
}

void MainMenuView::refreshHeartRateIcon()
{
    static int hrateFlag = 0;
    int hrateAlphaVal = HEART_RATE_ICON_SHALLOW_COLOR;
    hrateFlag = (hrateFlag == 0) ? \
                (hrateAlphaVal = HEART_RATE_ICON_SHALLOW_COLOR, HeartRateIcon.setBitmap(Bitmap(BITMAP_ICON_SMALL_HEART_INT_ID)) , 1) : \
                (hrateAlphaVal = HEART_RATE_ICON_DEEP_COLOR, HeartRateIcon.setBitmap(Bitmap(BITMAP_ICON_LARGE_HEART_INT_ID)), 0);
    HeartRateIcon.setAlpha(hrateAlphaVal);
    //HeartRateIcon.invalidate();
}

void MainMenuView::handleHeartRateDataUpdated(int physicalData, int logicalRateData)
{
    if (1 == showFirstTimeFlag)
    {
        refreshGraphs(logicalRateData, physicalData);
        refreshHeartRateChracterastic();

        if ((MainMenuPresenter::HEART_RATE == menuContainer.getSelectedScreen()) && (menuContainer.getCurrentState() == SwipeContainer::NO_ANIMATION))
        {
            refreshHeartRateIcon();
        }
    }
}
