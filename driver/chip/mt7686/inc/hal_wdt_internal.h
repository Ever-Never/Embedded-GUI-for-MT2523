
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

#ifndef _HAL_WDT_INTERNAL_H_
#define _HAL_WDT_INTERNAL_H_

#define WDT_EN_KEY                          (0x11)
#define WDT_LENGTH_KEY                      (0x12)
#define WDT_INTERVAL_KEY                    (0x13)
#define WDT_SW_RESTART_KEY                  (0x1456789a)
#define WDT_SW_RST_KEY                      (0x156789ab)
#define WDT_AUTO_RESTART_EN_KEY             (0x16)
#define WDT_IE_KEY                          (0x17)
#define WDT_CONNSYS_SW_RST_KEY              (0x18)
#define WDT_CONNSYS_CPU_SW_RST_KEY          (0x19)
#define WDT_SDCTL_SW_RST_KEY                (0x1a)
#define WDT_RETENTION_KEY                   (0x1b)
#define WDT_JTAG_RST_MASK_KEY               (0x1c)
#define WDT_AIRCR_RST_MASK_KEY              (0x1d)
#define WDT_PCM_RST_KEY                     (0x1e)
#define WDT_PMU_RST_KEY                     (0x1f)
#define WDT_AIRCR_RST_INTERVAL_KEY          (0x21)
#define WDT_RETENTION_FALG0_KEY             (0x22)
#define WDT_RETENTION_FALG1_KEY             (0x23)
#define WDT_RETENTION_FALG2_KEY             (0x24)
#define WDT_RETENTION_FALG3_KEY             (0x25)
#define WDT_RETENTION_FALG4_KEY             (0x26)
#define WDT_RETENTION_FALG5_KEY             (0x27)


#define WDT_STANDARD_1_OFFSET               (8)
#define WDT_STANDARD_1_MASK                 (0x1 << WDT_STANDARD_1_OFFSET)

#define WDT_STANDARD_8_OFFSET               (8)
#define WDT_STANDARD_8_MASK                 (0xff << WDT_STANDARD_8_OFFSET)

#define WDT_STANDARD_16_OFFSET              (16)
#define WDT_STANDARD_16_MASK                (0xffff << WDT_STANDARD_16_OFFSET)

#define WDT_STANDARD_32_OFFSET              (0)
#define WDT_STANDARD_32_MASK                (0xffffffff << WDT_STANDARD_32_OFFSET)

#define WDT_STA_SW_WDT_OFFSET               (0)
#define WDT_STA_SW_WDT_MASK                 (0x1 << WDT_STA_SW_WDT_OFFSET)
#define WDT_STA_HW_WDT_OFFSET               (1)
#define WDT_STA_HW_WDT_MASK                 (0x1 << WDT_STA_HW_WDT_OFFSET)


#define WDT_MAX_TIMEOUT_VALUE               (1000)  /* 1000s */
#define WDT_1_TICK_LENGTH                   (156)   /* 15.6ms */


void wdt_set_length(uint32_t seconds);
void wdt_set_mode(uint32_t value);
void wdt_set_restart(void);
void wdt_set_sw_rst(void);
void wdt_set_enable(uint32_t enable);
uint32_t wdt_get_reset_status(void);
uint32_t wdt_get_enable_status(void);
uint32_t wdt_get_mode_status(void);
void wdt_clear_irq(void);


#endif /* #ifndef _HAL_WDT_INTERNAL_H_ */

