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

#include <gui/graph_screen/GraphBelow.hpp>

using namespace touchgfx;

bool GraphBelow::drawCanvasWidget(const Rect &invalidatedArea) const
{
    if (numPoints < 2) {
        // A graph line with a single (or not even a single) point is invisible
        return true;
    }

    int firstIndex = 0;
    int lastIndex = numPoints - 1;

    // We can skip the line from index if the left part of the line at index+1 is to the left of the invalidated area
    while (firstIndex < lastIndex &&
            invalidatedArea.x > (widgetXindex(firstIndex + 1) + lineWidth / 2).to<int>()) {
        firstIndex++;
    }
    // We can skip the line from index if the left part of the line at index-1 is to the right of the invalidated area
    while (lastIndex > firstIndex &&
            invalidatedArea.right() < (widgetXindex(lastIndex - 1) - lineWidth / 2).to<int>()) {
        lastIndex--;
    }
    if (firstIndex < lastIndex) {
        Canvas canvas(this, invalidatedArea);
        canvas.moveTo(widgetXindex(firstIndex), widgetYindex(firstIndex));
        for (int index = firstIndex + 1; index <= lastIndex; index++) {
            canvas.lineTo(widgetXindex(index), widgetYindex(index));
        }
        canvas.lineTo(widgetXindex(lastIndex), widgetY(0));
        canvas.lineTo(widgetXindex(firstIndex), widgetY(0));
        //            canvas.lineTo(widgetXindex(lastIndex), widgetY(bottom));
        //            canvas.lineTo(widgetXindex(firstIndex), widgetY(bottom));
        return canvas.render();
    }
    return true;
}

Rect GraphBelow::getMinimalRectContainingIndices(int firstIndex, int lastIndex) const
{
    int minX = widgetXindex(firstIndex).to<int>();
    int maxX = widgetXindex(lastIndex).to<int>();
    int zeroY = widgetY(0).to<int>();
    int firstY = widgetYindex(firstIndex).to<int>();
    int minY = MIN(zeroY, firstY);
    int maxY = MAX(zeroY, firstY);
    for (int index = firstIndex + 1; index <= lastIndex; index++) {
        int y = widgetYindex(index).to<int>();
        minY = MIN(minY, y);
        maxY = MAX(maxY, y);
    }

    return Rect(minX, minY, maxX - minX + 1, maxY - minY + 1);
}
