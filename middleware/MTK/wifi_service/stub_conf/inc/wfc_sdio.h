/* Copyright Statement:
 *
 * (C) 2017-2100  MediaTek Inc. All rights reserved.
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

#ifndef __WFC_SDIO_H__
#define __WFC_SDIO_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_sdio.h"
#include "wfc_stub.h"

hal_sdio_status_t wfc_if_sdio_init(void);
void wfc_sdio_send_response(T_WFC_OPCODE *pwfc, uint16_t rspcode, uint8_t *txptr, uint32_t txsize, uint32_t hsize);
void wfc_sdio_send_data(uint8_t *txptr, uint32_t txsize);

void wfc_sido_htx_cp(T_WFC_QBUF *qbuf, uint8_t *pbuf, uint32_t offset);
void wfc_sdio_hrx_cp(T_WFC_QBUF *qbuf, uint8_t *hrxptr, uint32_t hrxSize);
void wfc_sdio_hevt_cp(T_WFC_QBUF *qbuf, uint8_t *hrxptr, uint32_t hrxSize);

int wfc_sdio_set_cmd_buf(void);
int wfc_sdio_set_htx_buf(void);

void wfc_sdio_free_cmd_buf(uint32_t qidx);
uint8_t *wfc_sdio_get_cmd_buf(uint32_t qidx);

void wfc_sdio_mq_reset(uint32_t ctrl);

void wfc_mq_push_hrx(T_WFC_QBUF *qbuf);
T_WFC_QBUF  *wfc_mq_pop_hrx(void);

int32_t wfc_mq_get_buf(uint32_t ctrl_idx, uint32_t *qidx);
void wfc_mq_clear(T_WFC_QBUF *qbuf);
void wfc_mq_free(T_WFC_QBUF *qbuf);
uint8_t wfc_mq_opcmd_process(T_WFC_QBUF *qbuf, uint8_t *isDTW);

void wfc_dump_buf(uint8_t *pbuf, uint32_t bufSize);

#ifdef __cplusplus
}
#endif


#endif  /* __WFC_SDIO_H__ */
