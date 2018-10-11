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

#ifndef __E_CORE_H__
#define __E_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


#define ESUCCESS    0x0000
#define ESTATE      0x3000
#define ELOCK       0x3001
#define EMAC        0x3002
#define EPARAM      0x3003
#define ETIMEOUTRST 0x3004
#define ELENGTH     0x3005
#define ENOTREADY   0x3006
#define ENOSUCHTYPE 0x3007
#define EDROPPED    0x3008
#define EIDXOOR     0x3009

enum eevent_id {
    EVT_ID_SYNFAIL = 0x0,
    EVT_ID_SYNCSUC,
    EVT_ID_INFOGET,
    EVT_ID_TIMEOUT,
    EVT_ID_MAX
};

enum etype_id {
    TYPE_ID_BEGIN = 0x0,
    TYPE_ID_AM,
    TYPE_ID_SSID,
    TYPE_ID_PWD,
    TYPE_ID_USER,
    TYPE_ID_PMK,
    TYPE_ID_CUST = 0x7F,
    TYPE_ID_MAX = 0xFF
};

struct etimer {
    void (*func)(uint32_t);
    uint32_t data;
    uint32_t expires;
};

typedef void (*event_cb)(enum eevent_id evt);
typedef void (*aes128_decrypt_cb)(uint8_t *cipher_blk, uint32_t cipher_blk_size,
                                  uint8_t *key, uint8_t key_len,
                                  uint8_t *plain_blk, uint32_t *plain_blk_size);
typedef void (*start_timer_cb)(struct etimer *);
typedef int32_t (*stop_timer_cb)(struct etimer *);


struct efunc_table {
    /* Func pointer used to indicate events from elian. */
    event_cb report_evt;

    start_timer_cb start_timer;
    stop_timer_cb stop_timer;

    /* AES128 decrypt func. */
    aes128_decrypt_cb aes128_decrypt;
    void (*os_get_time)(uint32_t * ptime);
};



/* This function is used to initialize ELIAN, this function
 * must be called first before call elian_input().
 * Parameters:
 *   la: Local MAC Address.
 *   tbl: call back functions needed by Elian.
 *   key: AES128 decrypt key, The length must be 16 bytes, if the
 *   key is NULL, ELIAN will use the default key.
 * Return value:
 *   0: success
 *   others: please refer to error code. */
int32_t elian_init(char *la, const struct efunc_table *tbl, const uint8_t *key);


/* This function is used to reset ELIAN, after reset, Elian
 * Will start to accept and parse data again. NOTES: the Local
 * MAC Address, function table and key will NOT be reseted.
 * Return value:
 *   0: success
 *   others: please refer to error code. */
int32_t elian_reset(void);


/* This function is to accept and parse 802.11 data package.
 * Parameters:
 *   p: the start address of 802.11 header.
 *   len: the length of package.
 * Return value:
 *   0: success
 *   others: please refer to error code. */
int32_t elian_input(char *p, int32_t len);


/* This function is to get the result of ELIAN. like SSID, PASSWORD.
 * Parameters:
 *    id: please refer to the enum etype_id.
 *    buf: buffer use to store the result.
 *    len: the length of the buffer, after this function returned,
 *         this len is the actual length of the info
 * Return value:
 *   0: success
 *   others: please refer to error code. */
int32_t elian_get(enum etype_id id, char *buf, int32_t *len);


/* This function is used to reset ELIAN, except stop the timer. After reset, Elian
 * Will start to accept and parse data again. NOTES: the Local
 * MAC Address, function table and key will NOT be reseted.
 * Return value:
 *   0: success
 *   others: please refer to error code. */
int32_t sc_rst(void);

/* This function is used to stop the timer.
 * Return value:
 *   0: success
 *   others: please refer to error code. */
int32_t elian_stop(void);

/* This function is for customer to get the customize-TLV buf and length.
 * Parameters:
 *      buf:(output) point to the customize-TLV
 *      len:(output) return the length of customize-TLV
 * Return value:
 *   0: success
 *   others: please refer to error code. */
int32_t build_cust_tlv_buf(char **buf, int32_t *len);


/* This function is used to get the code version.
 * Parameters:
 *      ver:(output) buffer to store the version string
 *      len:(input): the buffer length, cannot be shorter than the length of version string
 *
 * Return value:
 *   0: success
 *   others: please refer to error code. */
int32_t get_ver_code(char *ver, int32_t len);


/* This function is used to get the smart connection protocol version.
 * Parameters:
 *      ver:(output) buffer to store the version string
 *      len:(input): the buffer length, cannot be shorter than the length of version string
 *
 * Return value:
 *   0: success
 *   others: please refer to error code. */
int32_t get_ver_proto(char *ver, int32_t len);

struct etimer *get_etimer(void);

void bsmtcn_register_evt_func_table(struct efunc_table *tbl);

#ifdef __cplusplus
}
#endif

#endif

