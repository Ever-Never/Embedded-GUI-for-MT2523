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

extern void bsp_ept_gpio_setting_init(void);

#define    GPIO_PIN_NUMBER                          0x31   /* number of pins the chip supported */

#define    GPIO_MODE_OFFSET_TO_NEXT                 0x10  /* offset between two continuous mode registers */
#define    GPIO_MODE_REG_MAX_NUM                    6   /* maximam index number of mode register */
#define    GPIO_MODE_ONE_CONTROL_BITS               4   /* number of bits to control the mode of one pin */
#define    GPIO_MODE_ONE_REG_CONTROL_NUM            8   /* number of pins can be controled in one register */
#define    GPIO_MODE_REG0    GPIO_HWORD_REG_VAL(MODE, 0,  1,  2,  3,  4,  5,  6,  7, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE_REG1    GPIO_HWORD_REG_VAL(MODE,  8,  9,  10,  11,  12,  13,  14,  15, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE_REG2    GPIO_HWORD_REG_VAL(MODE,  16,  17,  18,  19,  20,  21,  22,  23, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE_REG3    GPIO_HWORD_REG_VAL(MODE,  24,  25,  26,  27,  28,  29,  30,  31, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE_REG4    GPIO_HWORD_REG_VAL(MODE, 32,  33,  34,  35,  36,  37,  38,  39, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE_REG5    GPIO_HWORD_REG_VAL(MODE,  40,  41,  42,  43,  44,  45,  46,  47, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE_REG6    GPIO_HWORD_REG_VAL(MODE,  48, NULL, NULL,  NULL, NULL, NULL,  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_MODE_ALL_VALUE     GPIO_MODE_REG0,GPIO_MODE_REG1,GPIO_MODE_REG2,GPIO_MODE_REG3,GPIO_MODE_REG4,GPIO_MODE_REG5,GPIO_MODE_REG6


#define    GPIO_DIR_OFFSET_TO_NEXT                  0x10  /* offset between two continuous direction registers */
#define    GPIO_DIR_REG_MAX_NUM                     1   /* maximam index number of direction register */
#define    GPIO_DIR_ONE_CONTROL_BITS                1   /* number of bits to control the direction of one pin */
#define    GPIO_DIR_ONE_REG_CONTROL_NUM            32   /* number of pins can be controled in one register */
#define    GPIO_DIR_REG0    GPIO_HWORD_REG_VAL(DIR, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_DIR_REG1    GPIO_HWORD_REG_VAL(DIR,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 48, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_DIR_ALL_VALUE     GPIO_DIR_REG0,GPIO_DIR_REG1


#define    GPIO_PULL_OFFSET_TO_NEXT                 0x10  /* offset between two continuous pullen registers */
#define    GPIO_PULL_REG_MAX_NUM                    1   /* maximam index number of pullen register */
#define    GPIO_PULL_ONE_CONTROL_BITS               1   /* number of bits to control the pullen of one pin */
#define    GPIO_PULL_ONE_REG_CONTROL_NUM            32   /* number of pins can be controled in one register */
#define    GPIO_PULL_REG0    GPIO_HWORD_REG_VAL(PULL, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_PULL_REG1    GPIO_HWORD_REG_VAL(PULL,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 48, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_PULL_ALL_VALUE     GPIO_PULL_REG0,GPIO_PULL_REG1



#define    GPIO_OUTPUT_LEVEL_OFFSET_TO_NEXT         0x10  /* offset between two continuous output data registers */
#define    GPIO_OUTPUT_LEVEL_REG_MAX_NUM            1   /* maximam index number of output data register */
#define    GPIO_OUTPUT_LEVEL_ONE_CONTROL_BITS       1   /* number of bits to control the output data of one pin */
#define    GPIO_OUTPUT_LEVEL_ONE_REG_CONTROL_NUM    32   /* number of pins can be controled in one register */
#define    GPIO_OUTPUT_LEVEL_REG0    GPIO_HWORD_REG_VAL(OUTPUT_LEVEL, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_OUTPUT_LEVEL_REG1    GPIO_HWORD_REG_VAL(OUTPUT_LEVEL, 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 48, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_OUTPUT_LEVEL_ALL_VALUE     GPIO_OUTPUT_LEVEL_REG0,GPIO_OUTPUT_LEVEL_REG1


#define    GPIO_DIN_OFFSET_TO_NEXT                  0x10  /* offset between two continuous input data registers */
#define    GPIO_DIN_REG_MAX_NUM                     1   /* maximam index number of input data register */
#define    GPIO_DIN_ONE_CONTROL_BITS                1   /* number of bits to control the input data of one pin */
#define    GPIO_DIN_ONE_REG_CONTROL_NUM             32   /* number of pins can be controled in one register */
#define    GPIO_DIN_REG0    GPIO_HWORD_REG_VAL(DIN, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_DIN_REG1    GPIO_HWORD_REG_VAL(DIN,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 48, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_DIN_ALL_VALUE     GPIO_DIN_REG0,GPIO_DIN_REG1


#define    GPIO_PULL_SEL_OFFSET_TO_NEXT             0x10  /* offset between two continuous pullsel registers */
#define    GPIO_PULL_SEL_REG_MAX_NUM                1   /* maximam index number of pullsel register */
#define    GPIO_PULL_SEL_ONE_CONTROL_BITS           1   /* number of bits to control the pullsel of one pin */
#define    GPIO_PULL_SEL_ONE_REG_CONTROL_NUM        32   /* number of pins can be controled in one register */
#define    GPIO_PULL_SEL_REG0    GPIO_HWORD_REG_VAL(PULL_SEL, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_PULL_SEL_REG1    GPIO_HWORD_REG_VAL(PULL_SEL,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47, 48, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_PULL_SEL_ALL_VALUE     GPIO_PULL_SEL_REG0,GPIO_PULL_SEL_REG1




#define    GPIO_PUPD_OFFSET_TO_NEXT                 0x10  /* offset between two continuous pupd registers */
#define    GPIO_PUPD_REG_MAX_NUM                    1   /* maximam index number of pupd register */
#define    GPIO_PUPD_ONE_CONTROL_BITS               1   /* number of bits to control the pupd of one pin */
#define    GPIO_PUPD_ONE_REG_CONTROL_NUM            32   /* number of pins can be controled in one register */
#define    GPIO_PUPD_REG0    GPIO_HWORD_REG_VAL(PUPD, NULL, NULL, NULL, NULL, 4, 5, 6, 7, 8, 9, NULL, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_PUPD_REG1    GPIO_HWORD_REG_VAL(PUPD, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_PUPD_ALL_VALUE     GPIO_PUPD_REG0,GPIO_PUPD_REG1


#define    GPIO_R0_OFFSET_TO_NEXT                   0x10  /* offset between two continuous R0 registers */
#define    GPIO_R0_REG_MAX_NUM                      1   /* maximam index number of R0 register */
#define    GPIO_R0_ONE_CONTROL_BITS                 1   /* number of bits to control the R0 of one pin */
#define    GPIO_R0_ONE_REG_CONTROL_NUM              32   /* number of pins can be controled in one register */
#define    GPIO_R0_REG0    GPIO_HWORD_REG_VAL(R0, NULL, NULL, NULL, NULL, 4, 5, 6, 7, 8, 9, NULL, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_R0_REG1    GPIO_HWORD_REG_VAL(R0, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_R0_ALL_VALUE     GPIO_R0_REG0,GPIO_R0_REG1


#define    GPIO_R1_OFFSET_TO_NEXT                   0x10  /* offset between two continuous  R1 registers */
#define    GPIO_R1_REG_MAX_NUM                      1   /* maximam index number of R1 register */
#define    GPIO_R1_ONE_CONTROL_BITS                 1   /* number of bits to control the R1 of one pin */
#define    GPIO_R1_ONE_REG_CONTROL_NUM              32   /* number of pins can be controled in one register */
#define    GPIO_R1_REG0    GPIO_HWORD_REG_VAL(R1, NULL, NULL, NULL, NULL, 4, 5, 6, 7, 8, 9, NULL, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31)
#define    GPIO_R1_REG1    GPIO_HWORD_REG_VAL(R1, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
#define    GPIO_R1_ALL_VALUE     GPIO_R1_REG0,GPIO_R1_REG1

