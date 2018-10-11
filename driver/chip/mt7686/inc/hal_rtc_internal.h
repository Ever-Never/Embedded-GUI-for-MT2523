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

#ifndef _HAL_RTC_INTERNAL_H_
#define _HAL_RTC_INTERNAL_H_

//#include "hal_pmu.h"

#define RTC_KEY_BBPU                        (0x4300)
#define RTC_KEY_BBPU_0                      (0xa200)
#define RTC_KEY_BBPU_1                      (0x3900)
#define RTC_KEY_BBPU_2                      (0xda00)
#define RTC_KEY_BBPU_3                      (0xc800)
#define RTC_KEY_BBPU_4                      (0x6400)
#define RTC_KEY_BBPU_CLEAR_POWERKEY         (0x8700)
#define RTC_KEY_BBPU_RELOAD                 (0x5800)

#define RTC_ALARM_PWREN_OFFSET              (0)
#define RTC_ALARM_PWREN_MASK                (0x1 << RTC_ALARM_PWREN_OFFSET)
/*1: ALARM or EINT occurred, BBPU[1] = 1*/
#define RTC_RTC_PU_OFFSET                   (1)
#define RTC_RTC_PU_MASK                     (0x1 << RTC_RTC_PU_OFFSET)
/*1: When an RTC tick occurs, BBPU[2] = 1*/
#define RTC_TICK_PWREN_OFFSET               (2)
#define RTC_TICK_PWREN_MASK                 (0x1 << RTC_TICK_PWREN_OFFSET)
/*1: When an RTC alarm occurs, BBPU[3]= 1*/
#define RTC_EINT_PWREN_OFFSET               (3)
#define RTC_EINT_PWREN_MASK                 (0x1 << RTC_EINT_PWREN_OFFSET)
/*1: System is in retention mode or wake up from retention mode.
Write 1 to enter retention mode, and core will power down.*/
#define RTC_RETENTION_MODE_OFFSET           (4)
#define RTC_RETENTION_MODE_MASK             (0x1 << RTC_RETENTION_MODE_OFFSET)

#define RTC_ALSTA_OFFSET                    (0)
#define RTC_ALSTA_MASK                      (0x1 << RTC_ALSTA_OFFSET)
#define RTC_TCSTA_OFFSET                    (1)
#define RTC_TCSTA_MASK                      (0x1 << RTC_TCSTA_OFFSET)
#define RTC_LPSTA_OFFSET                    (2)
#define RTC_LPSTA_MASK                      (0x1 << RTC_LPSTA_OFFSET)
#define RTC_EINTSTA_OFFSET                  (3)
#define RTC_EINTSTA_MASK                    (0x1 << RTC_EINTSTA_OFFSET)

#define RTC_AL_EN_OFFSET                    (0)
#define RTC_AL_EN_MASK                      (0x1 << RTC_AL_EN_OFFSET)
///TODO: add AL_MASK bit definition here

#define RTC_TC_EN_OFFSET                    (8)
#define RTC_TC_EN_MASK                      (0x1 << RTC_TC_EN_OFFSET)

#define RTC_TC_SECOND_OFFSET                (0)
#define RTC_TC_SECOND_MASK                  (0x3F << RTC_TC_SECOND_OFFSET)
#define RTC_TC_MINUTE_OFFSET                (8)
#define RTC_TC_MINUTE_MASK                  (0x3F << RTC_TC_MINUTE_OFFSET)

#define RTC_TC_HOUR_OFFSET                  (0)
#define RTC_TC_HOUR_MASK                    (0x1F << RTC_TC_HOUR_OFFSET)
#define RTC_TC_DOM_OFFSET                   (8)
#define RTC_TC_DOM_MASK                     (0x1F << RTC_TC_DOM_OFFSET)

#define RTC_TC_DOW_OFFSET                   (0)
#define RTC_TC_DOW_MASK                     (0x7 << RTC_TC_DOW_OFFSET)
#define RTC_TC_MONTH_OFFSET                 (8)
#define RTC_TC_MONTH_MASK                   (0xF << RTC_TC_MONTH_OFFSET)

#define RTC_TC_YEAR_OFFSET                  (0)
#define RTC_TC_YEAR_MASK                    (0x7F << RTC_TC_YEAR_OFFSET)

#define RTC_AL_SECOND_OFFSET                (0)
#define RTC_AL_SECOND_MASK                  (0x3F << RTC_AL_SECOND_OFFSET)
#define RTC_AL_MINUTE_OFFSET                (8)
#define RTC_AL_MINUTE_MASK                  (0x3F << RTC_AL_MINUTE_OFFSET)

#define RTC_AL_HOUR_OFFSET                  (0)
#define RTC_AL_HOUR_MASK                    (0x1F << RTC_AL_HOUR_OFFSET)
#define RTC_AL_DOM_OFFSET                   (8)
#define RTC_AL_DOM_MASK                     (0x1F << RTC_AL_DOM_OFFSET)

#define RTC_AL_DOW_OFFSET                   (0)
#define RTC_AL_DOW_MASK                     (0x7 << RTC_AL_DOW_OFFSET)
#define RTC_AL_MONTH_OFFSET                 (8)
#define RTC_AL_MONTH_MASK                   (0xF << RTC_AL_MONTH_OFFSET)

#define RTC_AL_YEAR_OFFSET                  (0)
#define RTC_AL_YEAR_MASK                    (0x7F << RTC_AL_YEAR_OFFSET)

#define RTC_RTC_LPD_OPT_OFFSET              (0)
#define RTC_RTC_LPD_OPT_MASK                (0x1 << RTC_RTC_LPD_OPT_OFFSET)
#define RTC_LPSTA_RAW_OFFSET                (0)
#define RTC_LPSTA_RAW_MASK                  (0x1 << RTC_LPSTA_RAW_OFFSET)
#define RTC_EOSC32_LPEN_OFFSET              (1)
#define RTC_EOSC32_LPEN_MASK                (0x1 << RTC_EOSC32_LPEN_OFFSET)
#define RTC_LPRST_OFFSET                    (3)
#define RTC_LPRST_MASK                      (0x1 << RTC_LPRST_OFFSET)

#define RTC_EINT_EINT_EN_OFFSET             (0)
#define RTC_EINT_EINT_EN_MASK               (0x1 << RTC_EINT_EINT_EN_OFFSET)
#define RTC_ENIT_DEB_OFFSET                 (1)
#define RTC_ENIT_DEB_MASK                   (0x1 << RTC_ENIT_DEB_OFFSET)
#define RTC_EINT_SYNC_EN_OFFSET             (2)
#define RTC_EINT_SYNC_EN_MASK               (0x1 << RTC_EINT_SYNC_EN_OFFSET)
#define RTC_EINT_INV_EN_OFFSET              (3)
#define RTC_EINT_INV_EN_MASK                (0x1 << RTC_EINT_INV_EN_OFFSET)

#define RTC_TIMER_CG_EN_OFFSET              (0)
#define RTC_TIMER_CG_EN_MASK                (0x1 << RTC_TIMER_CG_EN_OFFSET)

#define RTC_XOSCCALI_OFFSET                 (0)
#define RTC_XOSCCALI_MASK                   (0x1F << RTC_XOSCCALI_OFFSET)
#define RTC_AMPCTL_EN_OFFSET                (5)
#define RTC_AMPCTL_EN_MASK                  (0x1 << RTC_AMPCTL_EN_OFFSET)
#define RTC_AMP_GSEL_OFFSET                 (6)
#define RTC_AMP_GSEL_MASK                   (0x1 << RTC_AMP_GSEL_OFFSET)
#define RTC_EOSC32_STP_PWD_OFFSET           (7)
#define RTC_EOSC32_STP_PWD_MASK             (0x1 << RTC_EOSC32_STP_PWD_OFFSET)
#define RTC_EOSC32_CHOP_EN_OFFSET           (8)
#define RTC_EOSC32_CHOP_EN_MASK             (0x1 << RTC_EOSC32_CHOP_EN_OFFSET)
#define RTC_EOSC32_VCT_EN_OFFSET            (9)
#define RTC_EOSC32_VCT_EN_MASK              (0x1 << RTC_EOSC32_VCT_EN_OFFSET)
#define RTC_EOSC32_RSV_OFFSET               (10)
#define RTC_EOSC32_RSV_MASK                 (0xF << RTC_EOSC32_RSV_OFFSET)

#define RTC_XOSC_PWDB_OFFSET                (0)
#define RTC_XOSC_PWDB_MASK                  (0x1 << RTC_XOSC_PWDB_OFFSET)
#define RTC_EOSC_PWDB_OFFSET                (1)
#define RTC_EOSC_PWDB_MASK                  (0x1 << RTC_EOSC_PWDB_OFFSET)
#define RTC_SRAM_ISO_EN_OFFSET              (0)
#define RTC_SRAM_ISO_EN_MASK                (0x1 << RTC_SRAM_ISO_EN_OFFSET)


#define RTC_SRAM_PD_OFFSET                  (0)
#define RTC_SRAM_PD_MASK                    (0xFF << RTC_SRAM_PD_OFFSET)
#define RTC_SRAM_SLEEPB_OFFSET              (8)
#define RTC_SRAM_SLEEPB_MASK                (0xFF << RTC_SRAM_SLEEPB_OFFSET)

#define RTC_RTC_DIFF_OFFSET                 (0)
#define RTC_RTC_DIFF_MASK                   (0xFFF << RTC_RTC_DIFF_OFFSET)

#define RTC_RTC_CALI_OFFSET                 (0)
#define RTC_RTC_CALI_MASK                   (0x3FFF << RTC_RTC_CALI_OFFSET)
#define RTC_CALI_RW_SEL_OFFSET              (14)
#define RTC_CALI_RW_SEL_MASK                (0x1 << RTC_CALI_RW_SEL_OFFSET)

#define RTC_WRTGR_OFFSET                    (0)
#define RTC_WRTGR_MASK                      (0x1 << RTC_WRTGR_OFFSET)
#define RTC_CBUSY_OFFSET                    (0)
#define RTC_CBUSY_MASK                      (0x1 << RTC_CBUSY_OFFSET)
#define RTC_POWER_DETECTED_OFFSET           (1)
#define RTC_POWER_DETECTED_MASK             (0x1 << RTC_POWER_DETECTED_OFFSET)
#define RTC_STA_RETENTION_MODE_OFFSET       (2)
#define RTC_STA_RETENTION_MODE_MASK         (0x1 << RTC_STA_RETENTION_MODE_OFFSET)

#define RTC_LOW_BYTE_OPERATION              (0)
#define RTC_HIGH_BYTE_OPERATION             (1)
#define RTC_WORD_OPERATION                  (2)

#define RTC_POWERKEY0_KEY                   0xa357
#define RTC_POWERKEY1_KEY                   0x67d2

#define RTC_PROTECT1                        0x586a
#define RTC_PROTECT2                        0x9136

#define RTC_OSC32CON0_MAGIC_KEY_1           0x1a57
#define RTC_OSC32CON0_MAGIC_KEY_2           0x2b68

#define RTC_OSC32CON1_MAGIC_KEY_1           0x1a85
#define RTC_OSC32CON1_MAGIC_KEY_2           0xe7cf

#define RTC_OSC32CON2_MAGIC_KEY_1           0x1653
#define RTC_OSC32CON2_MAGIC_KEY_2           0x8918

#define RTC_SRAMCON_MAGIC_KEY_1             0x5D4A
#define RTC_SRAMCON_MAGIC_KEY_2             0x943F


#define ABIST_FQMTR_BASE                    (0xa2020000 + 0x400)

#define CKSYS_XTAL_FREQ_BASE                ((volatile uint32_t *)0xA20202A0)
#define CKSYS_TST_SEL_1_BASE                ((volatile uint32_t *)0xA2020224)
#define RTC_FXO_IS_26M                      ((volatile uint8_t*)(0xA20202A3))
#define RTC_XO_DIV_32K_EN                   ((volatile uint32_t*)(0xA202028C))
#define RTC_SLOW_SRC_B                      ((volatile uint8_t*)(0xA2010040))

/* frequency meter start */
typedef struct {
    __IO uint32_t ABIST_FQMTR_CON0;     /* Frequency Meter Control Register 0 */
    __IO uint32_t ABIST_FQMTR_CON1;     /* Frequency Meter Control Register 1 */
    __IO uint32_t ABIST_FQMTR_CON2;     /* Frequency Meter Control Register 2 */
    __IO uint32_t ABIST_FQMTR_DATA;     /* Frequency Meter Data */
} ABIST_FQMTR_REGISTER_T;
/* frequency meter end */

bool rtc_is_back_from_retention(void);

/* For internal user request, return frequency meter count with specific test. */
uint32_t f32k_measure_count(uint16_t fixed_clock, uint16_t tested_clock, uint16_t window_setting);

#endif /* #ifndef _HAL_RTC_INTERNAL_H_ */
