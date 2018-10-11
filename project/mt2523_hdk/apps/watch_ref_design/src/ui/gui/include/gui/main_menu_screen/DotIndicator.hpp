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

#ifndef DOTINDICATOR_HPP_
#define DOTINDICATOR_HPP_

#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/containers/ListLayout.hpp>

using namespace touchgfx;

/**
 * @class DotIndicator
 *
 * DotIndicator shows a number of small dots where one of the dots is highlighted.
 *
 * This widget is part of the TouchGFX Open Widget Repository.
 * https://github.com/draupnergraphics/touchgfx-widgets
 *
 * @see DotIndicator
 */
class DotIndicator : public Container
{
public:
    DotIndicator();
    virtual ~DotIndicator();

    /**
     * @fn void DotIndicator::setNumberOfDots(uint8_t size);
     *
     * @brief Sets the number of dots that should be displayed.
     *
     * @param size The size.
     */
    void setNumberOfDots(uint8_t size);

    /**
     * @fn void DotIndicator::setBitmaps(const Bitmap& normalDot, const Bitmap& highlightedDot);
     *
     * @brief Sets the bitmaps to be used for normal and highlighted dots. Note that the bitmaps should
     *        include the spacing between the dots and that they must have the same width and height.
     *
     * @param normalDot      The normal dot bitmap.
     * @param highlightedDot The highlighted dot bitmap.
     */
    void setBitmaps(const Bitmap &normalDot, const Bitmap &highlightedDot);

    /**
     * @fn void DotIndicator::goRight();
     *
     * @brief Highlight the dot to the right of the current highlighted dot.
     */
    void goRight();

    /**
     * @fn void DotIndicator::goLeft();
     *
     * @brief Highlight the dot to the left of the current highlighted dot
     */
    void goLeft();

    /**
     * @fn void DotIndicator::setHighlightPosition(uint8_t index);
     *
     * @brief Sets highlight position.
     *
     * @param index Zero-based index of the dot to be highlighted.
     */
    void setHighlightPosition(uint8_t index);

private:
    static const uint8_t MAX_SIZE = 10;

    ListLayout unselectedDots;
    Image dotHighlighted;
    Image dotNormal[MAX_SIZE];

    uint8_t numberOfDots;
    uint8_t currentDot;
};

#endif /* DOTINDICATOR_HPP_ */
