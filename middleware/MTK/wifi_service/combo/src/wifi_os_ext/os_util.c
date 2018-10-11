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

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "os.h" // FIXME: in libutil

#include "os_util.h"


static TickType_t gSysBootTick = 0;


void os_sleep(os_time_t sec, os_time_t usec)
{
    TickType_t xDelay;

    xDelay = sec * 1000 * portTICK_PERIOD_MS + usec * portTICK_PERIOD_MS / 1000;

    vTaskDelay(xDelay);
}


int os_get_time(struct os_time *t)
{
    TickType_t  cur;

    if (!gSysBootTick) {
        gSysBootTick = xTaskGetTickCount();
    }

    cur = xTaskGetTickCount();

    t->sec = (cur - gSysBootTick) / 1000 / portTICK_PERIOD_MS;
    t->usec = ((cur - gSysBootTick) % 1000) / portTICK_PERIOD_MS * 1000;

    return (cur - gSysBootTick);
}


unsigned long os_random(void)
{
    TickType_t  cur;

    cur = xTaskGetTickCount();

    cur = cur * 1103515245 + 12345 ;

    /* for return type "unsigned long" */
    return (unsigned long)(cur / 131072);

    //return (unsigned long)(cur/131072) % 65536 ;
}


int os_get_random(unsigned char *buf, size_t len)
{
    int            idx = 0;
    unsigned long  rd = 0;
    unsigned char *pc = (unsigned char *)&rd;

    while (len) {
        rd = os_random();
        os_memcpy(buf + idx, pc, (len > sizeof(unsigned long)) ? sizeof(unsigned long) : len);
        idx += (len > sizeof(unsigned long)) ? sizeof(unsigned long) : len;
        len = (len > sizeof(unsigned long)) ? len - sizeof(unsigned long) : 0;
    }

    return 0;
}


void *os_zalloc(size_t size)
{
    return pvPortCalloc(1, size);
}


void *os_malloc(size_t size)
{
    return pvPortMalloc(size);
}


void *os_realloc(void *ptr, size_t size)
{
    return pvPortRealloc(ptr, size);
}


void os_free(void *ptr)
{
    vPortFree(ptr);
}


/**
* os_strdup dulicates the incoming string and return the cloned string to
* caller.
*
* @param s incoming source string.
* @return NULL if memory allocation failed. The duplicated memory buffer
*         if succeeded.
* @note The caller is responsible to call free() to return the buffer.
*/
char *os_strdup(const char *s)
{
    int  l;
    char *r;

    if (s == NULL) {
        return NULL;
    }

    l = strlen(s);
    r = os_malloc(l + 1);

    if (r != NULL) {
        os_memcpy(r, s, l);
        r[l] = '\0';
    }

    return r;
}


int os_mktime(int year, int month, int day, int hour, int min, int sec,
              os_time_t *t)
{
    return -1;
}


int os_gmtime(os_time_t t, struct os_tm *tm)
{
    return -1;
}


