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

#ifndef NAND_DAL_VERSION_H
#define NAND_DAL_VERSION_H

// Define Max satck size for demand paging
#define MAX_DEMAND_STACK_USED_BY_NAND_DRIVER		500


#if 0
#if defined __NAND_FDM_50__

#define FDM_MARK 'F'
/* FDM version information */
#if defined(__NANDFDM_MULTI_INSTANCE__)
#if defined(__NANDFDM_TOTAL_BBM__)
#define FDM_VERSION     0x52
#define FDM_MAJOR_VER   (5)
#define FDM_MINOR_VER1  (2)
#define FDM_MINOR_VER2  (0)
#else
#define FDM_VERSION     0x51
#define FDM_MAJOR_VER   (5)
#define FDM_MINOR_VER1  (1)
#define FDM_MINOR_VER2  (0)
#endif //defined(__NANDFDM_TOTAL_BBM__)
#else
#define FDM_VERSION     0x50
#define FDM_MAJOR_VER   (5)
#define FDM_MINOR_VER1  (0)
#define FDM_MINOR_VER2  (0)
#define FDM_MINOR_VER2_EN_REPLACECNT (1)
#endif // defined(__NANDFDM_MULTI_INSTANCE__)
#define DAL_MARK 'D'
#if defined(__NFI_VERSION3_1__)
#define DAL_MAJOR_VER   (0)
#define DAL_MINOR_VER1  (0)
#if defined(MT6256)
#define DAL_VERISON     0x03	// ECC 12 bits
#define DAL_MINOR_VER2  (3)
#elif defined(MT6255)
#define DAL_VERISON     0x05	// ECC 8 bits
#define DAL_MINOR_VER2  (5)
#else
#define DAL_VERISON     0x02  // ECC 4 bit for MT6276
#define DAL_MINOR_VER2  (2)
#endif
#else
#define DAL_VERISON     0x01
#define DAL_MAJOR_VER   (0)
#define DAL_MINOR_VER1  (0)
#define DAL_MINOR_VER2  (1)
#endif


#endif /*__NAND_FDM_50__*/
#endif
#define FDM_MARK 'F'

#define FDM_VERSION     0x50
#define FDM_MAJOR_VER   (5)
#define FDM_MINOR_VER1  (0)
#define FDM_MINOR_VER2  (0)
#define FDM_MINOR_VER2_EN_REPLACECNT (1)

#define DAL_MARK 'D'

#define DAL_VERISON     0x01
#define DAL_MAJOR_VER   (0)
#define DAL_MINOR_VER1  (0)
#define DAL_MINOR_VER2  (1)

#endif /*NAND_DAL_VERSION_H*/

