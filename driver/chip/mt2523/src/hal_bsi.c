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

#include "hal_bsi.h"
#include "hal_platform.h"
#include "hal_sleep_manager_platform.h"
#include "hal_log.h"
#include "hal_gpt.h"

void bsi_write(uint8_t address , uint32_t data)
{
//	log_hal_info("bsi_write address:%d  data:%#x \n",address,data);
//	log_hal_info("[BSI init MODEM_CG_SET4:%d  CG_chck2:%#x] \n",*MODEM_CG_SET4,*PDN_COND0);
    uint32_t check;
    uint32_t rw_data;
    *BSI_WRDAT_CON = 0x1d00;
    data = data & 0x0007FFFFF;
    rw_data = address << 20;
    rw_data = rw_data | data;
    *BSI_WRDAT = rw_data;
    *BSI_CON = 0x80401;
    while (1) {
        check = *BSI_CON;
        if ((check & 0x80000000) == 0x80000000) {
            break;
        }
    }
    log_hal_info("[BSI_WRDAT : %x]\n", (unsigned int)*BSI_WRDAT);
}

void bsi_read(uint8_t address, uint32_t *get_data)
{
    uint32_t read_data;
    uint32_t rw_data;
    *BSI_RDCON = 0x9f8b;
    *BSI_RDADDR_CON = 0x0902;
    rw_data = 0x100;
    rw_data = rw_data + address;
    *BSI_RDADDR = rw_data;
    *BSI_RDCS_CON = 0x0;
    *BSI_RDCON = 0x89f8b;
    *get_data = *BSI_RDDAT;
    while (1) {
        read_data = *BSI_CON;
        if ((read_data & 0x80000000) == 0x80000000) {
            break;
        }
    }
    log_hal_info("[BSI_RDDAT : %x]\n", (unsigned int)*get_data);
}

void bsi_init(void)
{
    *MODEM_CG_CLR4 = *MODEM_CG_CLR4 | 0x4;
    *PDN_CLRD0 = *PDN_CLRD0 | (1 << 23);
}

