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

#ifndef MAIN_MENU_VIEW_HPP
#define MAIN_MENU_VIEW_HPP

#include <gui/common/DemoView.hpp>
#include <gui/main_menu_screen/MainMenuPresenter.hpp>
#include <gui/main_menu_screen/SwipeContainer.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <touchgfx/widgets/canvas/Shape.hpp>
#include <gui/graph_screen/Graph.hpp>

#if !defined(USE_BPP) || USE_BPP==16
#include <touchgfx/widgets/canvas/PainterRGB565.hpp>
#elif USE_BPP==24
#include <touchgfx/widgets/canvas/PainterRGB888.hpp>
#endif

class MainMenuView : public DemoView<MainMenuPresenter>
{
public:
    MainMenuView();
    virtual ~MainMenuView() {};

    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void afterTransition();
    virtual void handleClickEvent(const ClickEvent &evt);
    virtual void handleDragEvent(const DragEvent &evt);
    virtual void handleKeyEvent(uint8_t key);

    void gotoSelectedDemoScreen(uint8_t demoIndex);
    void setSelectedMenuIndex(uint8_t demoIndex);

    /**
     * Updates the hour, minutes and second counters
     * hours: 0-23
     * minutes: 0-59
     * seconds: 0-59
     */
    void handleTimeUpdated(uint8_t hours, uint8_t minutes, uint8_t seconds);

    // Updates the HeatrRate data
    void handleHeartRateDataUpdated(int physicalData, int logicalShowData);

    // Updates the clock view
    void updateClock(uint8_t hourCounter, uint8_t minuteCounter, uint8_t secondCounter);

    class SecondClockHand : public Shape<4>
    {
    public:
        SecondClockHand()
        {
            ShapePoint<float> points[4] = { { -1.5f, 20 },{ -1.5f, -126 },{ 1.5f, -126 },{ 1.5f, 20 } };
            setShape(points);
        }
    };

    class MinuteClockHand : public Shape<10>
    {
    public:
        MinuteClockHand()
        {
            ShapePoint<float> points[10] = { { 0, -117.5f },{ -5, -112.5f },{ -5, -22.5f },{ -2.5f, -20 },{ -2.5f, -2.5f },{ 0, 0 },{ 2.5f, -2.5f },{ 2.5f, -20 },{ 5, -22.5f },{ 5, -112.5f } };
            setShape(points);
        }
    };

    class HourClockHand : public Shape<10>
    {
    public:
        HourClockHand()
        {
            ShapePoint<float> points[10] = { { 0, -69.5f },{ -5, -64.5f },{ -5, -22.5f },{ -2.5f, -20 },{ -2.5f, -2.5f },{ 0, 0 },{ 2.5f, -2.5f },{ 2.5f, -20 },{ 5, -22.5f },{ 5, -64.5f } };
            setShape(points);
        }
    };

    int getGraphRowNum()
    {
        return NUMBER_OF_GRID_LINES;
    }

protected:
    static const int NUMBER_OF_DEMO_SCREENS = MainMenuPresenter::MAX_DEMO_ID;

    MainMenuPresenter::DemoID demoScreenIndex[NUMBER_OF_DEMO_SCREENS];
    bool clickAbort;

    Container demoScreensContainer[NUMBER_OF_DEMO_SCREENS];
    SwipeContainer menuContainer;
    Image backgroundLeft;
    Image backgroundRight;
    Image demoScreens[NUMBER_OF_DEMO_SCREENS];
    TextArea titleTxt[NUMBER_OF_DEMO_SCREENS];
    Image battery[NUMBER_OF_DEMO_SCREENS];

    // Clock members
    Image centerDotBG;
    Image centerDot;

    TextAreaWithOneWildcard year; Unicode::UnicodeChar yearValueBuffer[5];
    TextArea month;
    TextAreaWithOneWildcard day; Unicode::UnicodeChar dayValueBuffer[3];

    uint8_t currentSecondCounter;
    uint8_t currentMinuteCounter;
    uint8_t currentHourCounter;

    bool transitionDone;
    bool firstPresenterCall;

    SecondClockHand secondHand;
    MinuteClockHand minuteHand;
    HourClockHand hourHand;

#if !defined(USE_BPP) || USE_BPP==16
    PainterRGB565 handColorRed;
    PainterRGB565 handColorWhite;
#elif USE_BPP==24
    PainterRGB888 handColorRed;
    PainterRGB888 handColorWhite;
#endif

    // HeartRate members
    static const int NUMBER_OF_GRID_LINES = 7;
    Image HeartRateIcon;
    Container graphArea;
    Box graphBackground;
    Box graphGridLines[NUMBER_OF_GRID_LINES];
    TextAreaWithOneWildcard graphYValues[NUMBER_OF_GRID_LINES];
    Unicode::UnicodeChar graphYValuesbuf[NUMBER_OF_GRID_LINES][5];
    Graph primaryGraph;
    int tickCount;
    int graphX;
    int graphType;
    int leftX;
    int pointCounter;
    int showFirstTimeFlag;
    int graphInterval;
    static const int VALIDATE_HR_DATA_Y_VALUE = 25;
    static const int Data_Save_Num = 60;
    static const int HEART_RATE_ICON_SHALLOW_COLOR = 0xAB;
    static const int HEART_RATE_ICON_DEEP_COLOR = 0xFF;
    int data[Data_Save_Num][3];
    int averageHeartRate;
    int totalHeartRate;
    int maxHeartRate;
    int minHeartRate;
    int effectiveHeatrRateNum;

    TextAreaWithOneWildcard averageHRDataTxt;
    Unicode::UnicodeChar averageHRDatabuf[5];
    TextArea strMaxHeart;
    TextArea strMinHeart;
    TextAreaWithOneWildcard maxHRDataTxt;
    Unicode::UnicodeChar maxHRDatabuf[5];
    TextAreaWithOneWildcard minHRDataTxt;
    Unicode::UnicodeChar minHRDatabuf[5];

    void addClockFaceScreen();
    void addHeartRateScreen();
    void refreshGraphs(int y, int physicalData);
    void addHeartRateData(int y, int physicalData);
    void initPrimaryGraph();
    void refreshHeartRateChracterastic();
    void refreshHeartRateIcon();
    void initHeartRateChracterastic();
    void resetPrimaryGraph();
};

#endif // MAIN_MENU_VIEW_HPP
