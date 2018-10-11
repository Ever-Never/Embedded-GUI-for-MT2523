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

#ifndef _HALWDT_INTERNAL_H_
#define _HALWDT_INTERNAL_H_

#define WDT_TRUE   (1)
#define WDT_FALSE  (0)


#define WDT_MODE_KEY           (0x22)
#define WDT_MODE_KEY_OFFSET    (8)       /*8 bits*/
#define WDT_LENGTH_KEY         (0x08)
#define WDT_LENGTH_KEY_OFFSET    (5)       /*5 bits*/
#define WDT_RESTART_KEY        (0x1971)
#define WDT_SWRST_KEY          (0x1209)

#define WDT_MODE_ENABLE              (0)
#define WDT_MODE_EXTEN               (2)
#define WDT_MODE_IRQ                 (3)
#define WDT_MODE_HW_AUTO_RESTART_EN  (5)

#define WDT_ENABLE                   (1)
#define WDT_DISABLE                  (0)

#define WDT_CON0_IRQ_CLR             (4)
#define WDT_CON0_PMIC_WDT_MASK       (0)



#define WDT_STATUS_TIME_OUT_MASK     (0x00008000)
#define WDT_STATUS_SWRST_MASK        (0x00004000)




#define WDT_MAX_TIMEOUT_VALUE  (30)      /* 30s */
#define WDT_1_TICK_LENGTH      (156)     /* 15.6ms */




#define WDT_CLEAR_INTERRUPT()  \
do{ \
    WDT_REGISTER->WDT_CON0 |= (0x1 << WDT_CON0_IRQ_CLR); \
    WDT_REGISTER->WDT_CON0 &= ~(0x1 << WDT_CON0_IRQ_CLR); \
}while(0)


#define WDT_SET_SOFTWARE_RESET()  (WDT_REGISTER->WDT_SWRST = WDT_SWRST_KEY)
#define WDT_SET_RESTART()         (WDT_REGISTER->WDT_RESTART = WDT_RESTART_KEY)
#define WDT_GET_MODE_BIT_VALUE(bit_offset) (((WDT_REGISTER->WDT_MODE) >> bit_offset) & 0x1)
#define WDT_MASK_ADIE_SIGNAL()     (WDT_REGISTER->WDT_CON0 |= 0x1)

void wdt_set_length(uint32_t seconds);
void wdt_set_mode_register_by_bit(uint32_t bit_offset, uint32_t value);
uint32_t wdt_get_reset_status(void);
#endif

