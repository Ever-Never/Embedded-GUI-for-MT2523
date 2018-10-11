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

#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <mvp/View.hpp>
#include <mvp/MVPApplication.hpp>
#include <gui/model/Model.hpp>
#include <gui/model/Time.hpp>

using namespace touchgfx;

class FrontendHeap;

/**
 * The FrontendApplication is the concrete implementation of an MVPApplication
 * for a specific application. Its primary job is to provide functions for
 * switching between screens (presenter/view pairs). By convention these functions
 * are usually called gotoXXScreen().
 *
 * Note that the base class Application is a singleton. Switching screen can be done
 * from anywhere (usually presenters) by e.g. the following:
 * static_cast<FrontendApplication*>(Application::getInstance())->gotoXXScreen();
 */
class FrontendApplication : public MVPApplication
{
public:
    FrontendApplication(Model &m, FrontendHeap &heap);
    virtual ~FrontendApplication() { }

    /**
     * Called automatically every frame. Will call tick on the model and then delegate
     * the tick event to the framework for further processing.
     */
    virtual void handleTickEvent()
    {
        model.tick();
        MVPApplication::handleTickEvent();
    }

    /**
    * Request a transition to the "Template" screen. The actual switching will
    * be done at next tick (by gotoTemplateScreenImpl).
    *
    * @note The reason for not switching immediately is that the new presenter and view
    * reuse the same memory as the current ones, so the switch cannot occur instantaneously
    * since that would cause memory corruption at the point of calling this function.
    */
    void gotoMainMenuScreenNoTransition();
    void gotoMainMenuScreen();
    void backMainMenuScreen();
    void gotoSettingScreen();
    void backSettingScreen();
    void gotoSetTimeScreen();
    void gotoSetDateScreen();
    void gotoSetHeartRateScreen();

private:
    Callback<FrontendApplication> transitionCallback;
    FrontendHeap &frontendHeap;
    Model &model;

    void gotoMainMenuScreenNoTransitionImpl();
    void gotoMainMenuScreenImpl();
    void backMainMenuScreenImpl();
    void gotoSettingScreenImpl();
    void backSettingScreenImpl();
    void gotoSetTimeScreenImpl();
    void gotoSetDateScreenImpl();
    void gotoSetHeartRateScreenImpl();
};

#endif /* FRONTENDAPPLICATION_HPP */
