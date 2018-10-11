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

#ifndef DEMO_VIEW_HPP
#define DEMO_VIEW_HPP

#include <mvp/View.hpp>
#include <gui/common/DemoViewInterface.hpp>
#include <touchgfx/widgets/Button.hpp>
#include <BitmapDatabase.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <gui/common/CommonService.hpp>

using namespace touchgfx;

template <class T>
class DemoView : public View<T>, public DemoViewInterface
{
public:
    DemoView() :
        onGotoMenuButtonClicked(this, &DemoView::gotoMenuClickedHandler),
        onGotoSettingButtonClicked(this, &DemoView::gotoSettingClickedHandler)
    {
        gotoMenuButton.setAction(onGotoMenuButtonClicked);
        gotoSettingButton.setAction(onGotoSettingButtonClicked);
    }

    virtual void handleClickEvent(const ClickEvent &evt)
    {
        View<T>::handleClickEvent(evt);

        CommonService::turnOnBacklight();
    }

    virtual void handleKeyEvent(uint8_t key)
    {
        View<T>::handleKeyEvent(key);

        CommonService::turnOnBacklight();
    }

protected:
    Button gotoMenuButton;
    Button gotoSettingButton;

    Callback<DemoView, const AbstractButton &> onGotoMenuButtonClicked;
    Callback<DemoView, const AbstractButton &> onGotoSettingButtonClicked;

    void gotoMenuClickedHandler(const AbstractButton &button)
    {
        View<T>::presenter->backOptionMenuSelected();
    }

    void gotoSettingClickedHandler(const AbstractButton &button)
    {
        View<T>::presenter->backOptionSettingSelected();
    }
};

#endif // DEMO_VIEW_HPP
