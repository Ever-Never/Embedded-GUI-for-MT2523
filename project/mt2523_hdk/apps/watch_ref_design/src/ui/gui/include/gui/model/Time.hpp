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

#ifndef TIME_HPP
#define TIME_HPP

#include <touchgfx/hal/Types.hpp>

struct Time {
    Time() : hours(0), minutes(0), seconds(0), milliseconds(0) {}

    explicit Time(int ms)
    {
        hours = ms / 3600000;
        ms -= hours * 3600000;
        minutes = ms / 60000;
        ms -= minutes * 60000;
        seconds = ms / 1000;
        ms -= seconds * 1000;
        milliseconds = ms;
    }

    Time(uint8_t h, uint8_t m, uint8_t s, uint16_t millis) : hours(h), minutes(m), seconds(s), milliseconds(millis) {}

    bool operator==(const Time &other) const
    {
        return ((hours == other.hours) &&
                (minutes == other.minutes) &&
                (seconds == other.seconds) &&
                (milliseconds == other.milliseconds));
    }

    bool operator!=(const Time &other) const
    {
        return !(*this == other);
    }

    // Returns the time difference as count of milliseconds
    long operator-(const Time &other) const
    {
        int diff = (hours - other.hours) * 60 * 60 * 1000 +
                   (minutes - other.minutes) * 60 * 1000 +
                   (seconds - other.seconds) * 1000 +
                   (milliseconds - other.milliseconds);
        if (diff < 0) {
            diff += 24 * 60 * 60 * 1000;
        }
        return diff;
    }

    static float toSeconds(long milliseconds)
    {
        return milliseconds / (float) 1000.0;
    }

    bool hasValue() const
    {
        return ((hours > 0) ||
                (minutes > 0) ||
                (seconds > 0) ||
                (milliseconds > 0));
    }

    int getValueInSeconds() const
    {
        return hours * 3600 + minutes * 60 + seconds;
    }

    int getValueInMillis() const
    {
        return getValueInSeconds() * 1000 + milliseconds;
    }

    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t milliseconds;
};

#endif /* TIME_HPP */
