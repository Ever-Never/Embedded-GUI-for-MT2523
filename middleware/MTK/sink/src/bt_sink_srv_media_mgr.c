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

#include <stdint.h>
#include "bt_a2dp.h"
#include "bt_sink_srv_media_mgr.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_utils.h"


/* Variable declare */
static bt_sink_srv_media_mgr_context_t g_bt_sink_srv_media_mgr_context = {{0}};

const static uint8_t BT_SINK_SRV_A2DP_AAC_SYNC_HEAD[BT_SINK_SRV_AAC_ADTS_LENGTH] = {0xFF, 0xF9, 0x40, 0x20, 0x00, 0x1F, 0xFC};

extern uint32_t g_a2dp_srv_rece_pkt_byte;
extern uint32_t g_a2dp_decoder_read_pkt_byte;

/* Function declare */
static uint16_t bt_sink_srv_media_mgr_update_index(uint16_t index);

static void bt_sink_srv_media_mgr_free_item(void);

static void bt_sink_srv_media_mgr_prepare_data_blcok(data_block_struct_t *db, uint16_t db_len);

static void bt_sink_srv_media_mgr_clear_data_blcok(data_block_struct_t *db, uint16_t db_len);

static void bt_sink_srv_media_mgr_access_item(uint32_t item);

static uint8_t bt_sink_srv_media_mgr_get_lbyte(uint16_t data);

static uint8_t bt_sink_srv_media_mgr_get_hbyte(uint16_t data);

static void bt_sink_srv_media_mgr_fill_aac_header(uint8_t *data, uint32_t len, bt_a2dp_codec_capability_t *codec);

static int32_t bt_sink_srv_media_mgr_write_to_dsp(volatile uint16_t *buf, uint32_t length);

static void bt_sink_srv_media_mgr_construct_copy_data(copy_data_struct_t *cp_data, data_block_struct_t *db, uint16_t db_count, uint16_t skip);

static int32_t media_mgr_copy_data(copy_data_struct_t *cp_data, volatile uint16_t *buf, uint32_t length);

static uint8_t media_mgr_get_last_byte(copy_data_struct_t *cp_data);

static void media_mgr_write_dsp_data(volatile uint16_t *dst, uint16_t *src, uint32_t length);



static uint16_t bt_sink_srv_media_mgr_update_index(uint16_t index)
{
    return (++index >= BT_SINK_SRV_MEDIA_MGR_ITEM_COUNT) ? (0) : (index);
}


static void bt_sink_srv_media_mgr_free_item(void)
{
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();
    uint16_t index = 0;
    int32_t i = 0;
    bt_hci_packet_t *pkt = NULL;

    for (i = 0; i < BT_SINK_SRV_MEDIA_MGR_ITEM_COUNT; ++i) {
        index = ctx->garbage_ac_index;
        if (ctx->iflag[index] & BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_FREE) {
            bt_a2dp_release_media_data_node((uint8_t *)(ctx->item[index]));
            /* Update hold count */
            --ctx->item_count;
            /* Update hold size */
            pkt = (bt_hci_packet_t *)(ctx->item[index]);
            ctx->size -= (pkt->packet_length - pkt->offset);
            /* Reset free access flag */
            BT_SINK_SRV_REMOVE_FLAG(ctx->iflag[index], BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_FREE);

            ctx->garbage_ac_index = bt_sink_srv_media_mgr_update_index(index);
        } else {
            break;
        }
    }
}


static void bt_sink_srv_media_mgr_prepare_data_blcok(data_block_struct_t *db, uint16_t db_len)
{
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();
    uint16_t index = ctx->decoder_ac_index;
    bt_a2dp_codec_type_t codec_type;
    int32_t k = 0;
    uint8_t *ptr = NULL;
    uint16_t len = 0;
    bt_hci_packet_t *pkt = NULL;
    bt_a2dp_codec_capability_t *codec = NULL;
    uint8_t *hci_buf = NULL;
    uint16_t offset = 0;

    if (ctx->iflag[index] & BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS) {
        /* get current codec type */
        codec_type = ctx->codec_type;
        codec = ctx->codec;

        k = 0;
        pkt = (bt_hci_packet_t *)ctx->item[index];
        hci_buf = (uint8_t *)pkt;

        ctx->last_check_item = (uint32_t)hci_buf;
        /* Odd byte case */
        if (k < db_len && ctx->odd_byte < DATA_BLOCK_ODD_BYTE) {
            ptr = (uint8_t *)(db[k].ptr);
            ptr[0] = (uint8_t)(ctx->odd_byte);
            db[k].size = 1;
            ++k;
        }

        /* 2: 0x3453 sync word or ADTS header And payload */
        if (k + 2 != db_len) {
            return ;
        }

        if (codec_type == BT_A2DP_CODEC_SBC) {
            /* SBC sync work */
            ptr = (uint8_t *)(db[k].ptr);
            /* DSP order byte~~little-endian */
            ptr[0] = bt_sink_srv_media_mgr_get_lbyte(BT_SINK_SRV_MEDIA_SBC_SYNC_WORD);
            ptr[1] = bt_sink_srv_media_mgr_get_hbyte(BT_SINK_SRV_MEDIA_SBC_SYNC_WORD);
            len = pkt->packet_length - pkt->offset - BT_SINK_SRV_MEDIA_PKT_HEADER_LEN;
            ptr[2] = bt_sink_srv_media_mgr_get_lbyte(len);
            ptr[3] = bt_sink_srv_media_mgr_get_hbyte(len);
            db[k].size = BT_SINK_SRV_MEDIA_SBC_SYNC_WORD_LEN;
            ++k;

            /* SBC payload */
            offset = pkt->offset + BT_SINK_SRV_MEDIA_PKT_HEADER_LEN;
            db[k].ptr = (uint32_t)(hci_buf + offset);
            len = pkt->packet_length - offset;
            db[k].size = len;
            BT_SINK_SRV_SET_FLAG(db[k].flag, DATA_BLOCK_FLAG_RELEASE);
            ++k;
        } else if (codec_type == BT_A2DP_CODEC_AAC) {
            // search the payload start pointer for AAC.
            uint8_t payload_offset = 9;
            uint8_t tempByte = 0;
            do {
                tempByte = ((uint8_t *)(hci_buf + pkt->offset))[BT_SINK_SRV_MEDIA_PKT_HEADER_LEN + payload_offset];
                payload_offset++;
            } while (tempByte == 0xFF);

            ptr = (uint8_t *)(db[k].ptr);
            len = pkt->packet_length - pkt->offset - BT_SINK_SRV_MEDIA_PKT_HEADER_LEN - payload_offset + BT_SINK_SRV_AAC_ADTS_LENGTH;
            bt_sink_srv_media_mgr_fill_aac_header(ptr, len, codec);
            db[k].size = BT_SINK_SRV_AAC_ADTS_LENGTH;
            ++k;

            /* AAC payload */
            offset =  pkt->offset + BT_SINK_SRV_MEDIA_PKT_HEADER_LEN + payload_offset;
            db[k].ptr = (uint32_t)(hci_buf + offset);
            len = pkt->packet_length - offset;
            db[k].size = len;
            BT_SINK_SRV_SET_FLAG(db[k].flag, DATA_BLOCK_FLAG_RELEASE);
            ++k;
        } else {
            ;
        }
    } else {
        ;
    }
}


static void bt_sink_srv_media_mgr_clear_data_blcok(data_block_struct_t *db, uint16_t db_len)
{
    int32_t i = 0;
    //uint8_t *hci_buf = NULL;
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();

    for (i = 0; i < db_len; ++i) {
        if (db[i].flag & DATA_BLOCK_FLAG_RELEASE) {
            BT_SINK_SRV_REMOVE_FLAG(db[i].flag, DATA_BLOCK_FLAG_RELEASE);
            bt_sink_srv_media_mgr_access_item(ctx->last_check_item);
        }
    }
}


static void bt_sink_srv_media_mgr_access_item(uint32_t item)
{
    //bt_hci_packet_t *pkt = NULL;
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();
    uint16_t index = ctx->decoder_ac_index;
    if ((item != ctx->item[index]) || (!(ctx->iflag[index] & BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS))) {
        bt_sink_srv_report("[Sink][MedMgr]acees_item(error)--");
    }

    ctx->last_check_item = 0;
    //pkt = (bt_hci_packet_t *)(ctx->item[index]);
    //ctx->size -= pkt->packet_length - pkt->offset;
    BT_SINK_SRV_SET_FLAG(ctx->iflag[index], BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_FREE);
    BT_SINK_SRV_REMOVE_FLAG(ctx->iflag[index], BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS);
    ctx->decoder_ac_index = bt_sink_srv_media_mgr_update_index(index);
}


static uint8_t bt_sink_srv_media_mgr_get_lbyte(uint16_t data)
{
    return (data & 0x00ff);
}


static uint8_t bt_sink_srv_media_mgr_get_hbyte(uint16_t data)
{
    return ((data & 0xff00) >> 8);
}


static void bt_sink_srv_media_mgr_fill_aac_header(uint8_t *data, uint32_t len, bt_a2dp_codec_capability_t *codec)
{
    uint8_t freqIndex = 0;
    uint8_t channelNum = 2; // Assume mono
    uint8_t temp = 0;

    temp = codec->codec.aac.freq_h << 4 | (codec->codec.aac.freq_l & 0x0F);

    while (temp ^ 1) {
        freqIndex++;
        temp >>= 1;
    }

    temp = codec->codec.aac.channels;
    if (temp == 1) {
        channelNum = 2;
    } else if (temp == 2) {
        channelNum = 1;
    }

    memcpy(data, BT_SINK_SRV_A2DP_AAC_SYNC_HEAD, BT_SINK_SRV_AAC_ADTS_LENGTH);

    data[2] |= (freqIndex << 2);
    data[3] |= (channelNum << 6) | (len >> 11);
    data[4] = (len >> 3) & 0xFF;
    data[5] |= (len & 0x07) << 5;
}


static int32_t bt_sink_srv_media_mgr_write_to_dsp(volatile uint16_t *buf, uint32_t length)
{
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();
    uint8_t odd_byte = 0;
    uint8_t payload_header[7] = {0};
    data_block_struct_t dblock[3] = {{0}};
    uint16_t db_len = 0;
    int32_t read_len = 0, write_len = 0;
    uint16_t index = 0;
    copy_data_struct_t cp_data = {0};
    static uint32_t loop_idx = 0;

    /* Why need??? */
    if (length == 0) {
        return 0;
    }

    write_len = 0;
    while (1) {
        index = ctx->decoder_ac_index;
        db_len = 0;
        if (!(ctx->flag & BT_SINK_SRV_MEDIA_MGR_FLAG_READY)) {
            /* Media mgr is not ready(handle clear all case) */
            loop_idx = 0;
            break;
        }

        if (!(ctx->iflag[index] & BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS)) {
            /* Empty data item */
            loop_idx = 0;
            break;
        }

        /* Prepare stack memory for odd byte and Sync word or ADTS header */
        if (ctx->odd_byte < DATA_BLOCK_ODD_BYTE) {
            dblock[db_len].ptr = (uint32_t)(&odd_byte);
            ++db_len;
        }

        dblock[db_len].ptr = (uint32_t)(&payload_header);
        ++db_len;

        /* For payload */
        ++db_len;

        /* Prepare data block */
        bt_sink_srv_media_mgr_prepare_data_blcok(dblock, db_len);
        /* Construct copy data struct */
        bt_sink_srv_media_mgr_construct_copy_data(&cp_data, dblock, db_len, ctx->skip);
        /* Write DSP data format little-endian */

        read_len = media_mgr_copy_data(&cp_data, buf + (write_len >> 1), (length - (write_len >> 1) ));

        /* Calc skip */
        if ((read_len + ctx->skip) == cp_data.size) {
            /* Access data done need free it */
            /* Update skip position */
            ctx->skip = 0;
            /* Update odd byte */
            ctx->odd_byte = DATA_BLOCK_ODD_BYTE;
            bt_sink_srv_media_mgr_clear_data_blcok(cp_data.db , cp_data.db_count);
        } else if ((cp_data.size - read_len - ctx->skip) > 1) {
            /* Update skip position, DSP buf full */
            ctx->skip += read_len;
        } else if ((cp_data.size - read_len - ctx->skip) == 1) {
            /* Odd byte case */
            /* Update skip position */
            ctx->skip = 0;
            /* Update odd byte */
            ctx->odd_byte = media_mgr_get_last_byte(&cp_data);
            bt_sink_srv_media_mgr_clear_data_blcok(cp_data.db , cp_data.db_count);
        } else {
            /* Exception handle */
            ;
        }

        write_len += read_len;

        if ((write_len >> 1) == length) {
            /* DSP buf full */
            loop_idx = 0;
            break;
        } else {
            ++loop_idx;
        }

        if (loop_idx >= 30) {
            printf("cpy: 0x%x, read_len: %d, write_len: %d, len: %d", (unsigned int)(&cp_data), (int)read_len, (int)write_len, (int)(length << 1));
            bt_sink_srv_assert(0 && "write_to_dsp\n");
        }
    }

    g_a2dp_decoder_read_pkt_byte += write_len;
    /* Word length */
    return (write_len >> 1);
}


static void bt_sink_srv_media_mgr_construct_copy_data(copy_data_struct_t *cp_data, data_block_struct_t *db, uint16_t db_count, uint16_t skip)
{
    uint16_t size = 0;
    int32_t i = 0;

    for (i = 0, size = 0; i < db_count; ++i) {
        size += db[i].size;
    }

    cp_data->db = db;
    cp_data->skip = skip;
    cp_data->db_count = db_count;
    cp_data->size = size;
}


static int32_t media_mgr_copy_data(copy_data_struct_t *cp_data, volatile uint16_t *buf, uint32_t length)
{
    int16_t offset = cp_data->skip;
    uint16_t dsp_word = 0;
    uint8_t odd_data = 0;
    uint8_t odd_flag = false;
    int32_t write_len = 0, total_len = 0, len = 0;
    int32_t i = 0, k = 0;
    volatile uint16_t *dst = buf;
    uint8_t *src = NULL;
    uint8_t *data = NULL;

    k = cp_data->db_count;
    total_len = length << 1;
    for (i = 0, len = 0; (i < k) && (total_len); ++i) {
        if (cp_data->db[i].size > offset) {
            if (odd_flag) {
                odd_flag = false;
                data = (uint8_t *)cp_data->db[i].ptr;
                dsp_word = (data[offset] << 8) | (odd_data);
        #ifdef __MEDIA_DUMP_DEBUG__
                sink_srv_dump_dsp_data((uint8_t *)(&dsp_word), 2);
        #endif
                media_mgr_write_dsp_data(dst + (len >> 1), &dsp_word, 1);
                ++offset;
                total_len -= 2;
                len += 2;
                /* Reback i */
                --i;
                continue;
            }
            /* Calc min write len */
            write_len = MINIMUM(total_len, cp_data->db[i].size - offset);
            /* Make it even */
            write_len &= ~0x01;
            /* Write DSP buffer by word */
            src = (uint8_t *)cp_data->db[i].ptr;
        #ifdef __MEDIA_DUMP_DEBUG__
            sink_srv_dump_dsp_data(src + offset, write_len);
        #endif
            media_mgr_write_dsp_data(dst + (len >> 1), (uint16_t *)(src + offset), (write_len >> 1));
            /* Calc offset */
            offset += write_len;
            total_len -= write_len;
            len += write_len;
            /* This block access done, reset offset 0 */
            if (cp_data->db[i].size == offset) {
                offset = 0;
            } else if (cp_data->db[i].size - 1 == offset) {
                /* Odd case */                
                data = (uint8_t *)cp_data->db[i].ptr;
                odd_flag = true;
                /* Save Odd content */
                odd_data = data[offset];
                /* Trans next db */
                offset = 0;
            } else {
                /* DSP buffer full */
            }
        } else {
            /* Skip */
            offset -= cp_data->db[i].size;
        }
    }

    return len;
}


static uint8_t media_mgr_get_last_byte(copy_data_struct_t *cp_data)
{
    uint8_t *data = (uint8_t *)(cp_data->db[cp_data->db_count - 1].ptr);
    return data[cp_data->db[cp_data->db_count - 1].size - 1];
}


static void media_mgr_write_dsp_data(volatile uint16_t *dst, uint16_t *src, uint32_t length)
{
    uint32_t i;
    for (i = length; i > 0; i--) {
        *dst++ = *src++;
    }
}


void bt_sink_srv_media_mgr_init(void)
{
    g_bt_sink_srv_media_mgr_context.odd_byte = DATA_BLOCK_ODD_BYTE;
    BT_SINK_SRV_SET_FLAG(g_bt_sink_srv_media_mgr_context.flag, BT_SINK_SRV_MEDIA_MGR_FLAG_READY);
}


bt_sink_srv_media_mgr_context_t *bt_sink_srv_media_mgr_get_context(void)
{
    return &g_bt_sink_srv_media_mgr_context;
}


void bt_sink_srv_media_mgr_set_codec_type(uint32_t type, bt_a2dp_codec_capability_t *codec)
{
    bt_sink_srv_report("[sink][media]set_codec_type-type: %d\n", type);
    g_bt_sink_srv_media_mgr_context.codec_type = type;
    g_bt_sink_srv_media_mgr_context.codec = codec;
}


int32_t bt_sink_srv_media_mgr_push_item(uint32_t item)
{
    bt_hci_packet_t *pkt = (bt_hci_packet_t *)item;
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();
    uint16_t index = ctx->bt_ac_index;

    if ((pkt) && (ctx->size < BT_SINK_SRV_MEDIA_MGR_HOLD_SIZE)) {
        if (!(ctx->iflag[index])) {
            bt_a2dp_hold_media_data_node((uint8_t *)pkt);
            ctx->item[index] = item;
            ctx->size += (pkt->packet_length - pkt->offset);
            ++ctx->item_count;

            /* Set decoder access flag */
            BT_SINK_SRV_SET_FLAG(ctx->iflag[index], BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS);

            ctx->bt_ac_index = bt_sink_srv_media_mgr_update_index(index);

            return BT_SINK_SRV_MEDIA_MGR_ERR_SUCCESS;
        } else {
            /* Reach max hold item count */
            return BT_SINK_SRV_MEDIA_MGR_ERR_FAIL_2ND;
        }
    } else {
        /* reach max hold size */
        return BT_SINK_SRV_MEDIA_MGR_ERR_FAIL_1ST;
    }
}


int32_t bt_sink_srv_media_mgr_read_data(volatile uint16_t *buf, uint32_t length /* word length */)
{
    return bt_sink_srv_media_mgr_write_to_dsp(buf, length);
}


void bt_sink_srv_media_mgr_clear(void)
{
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();
    int32_t i = 0;
    uint16_t index = 0;

    /* Remove ready flag */
    BT_SINK_SRV_REMOVE_FLAG(ctx->flag, BT_SINK_SRV_MEDIA_MGR_FLAG_READY);
    ctx->odd_byte = DATA_BLOCK_ODD_BYTE;
    ctx->skip = 0;

    for (i = 0; i < BT_SINK_SRV_MEDIA_MGR_ITEM_COUNT; ++i) {
        index = ctx->decoder_ac_index;
        if (ctx->iflag[index] & BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS) {
            /* Set free flag */
            BT_SINK_SRV_SET_FLAG(ctx->iflag[index], BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_FREE);
            /* Reset access flag */
            BT_SINK_SRV_REMOVE_FLAG(ctx->iflag[index], BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS);
            ctx->decoder_ac_index = bt_sink_srv_media_mgr_update_index(index);
        } else {
            break;
        }
    }

    /* Set Ready flag */
    BT_SINK_SRV_SET_FLAG(ctx->flag, BT_SINK_SRV_MEDIA_MGR_FLAG_READY);
}


void bt_sink_srv_media_mgr_garbage_free(void)
{
    bt_sink_srv_media_mgr_free_item();
}


uint32_t bt_sink_srv_media_mgr_get_last_item(void)
{
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();
    uint16_t index = 0;
    uint32_t last_item = 0;

    index = ctx->bt_ac_index;
    if (index == 0) {
        index = BT_SINK_SRV_MEDIA_MGR_ITEM_COUNT - 1;
    } else {
        --index;
    }

    if ((ctx->iflag[index] & BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS)) {
        last_item = ctx->item[index];
    }

    return last_item;
}


uint32_t bt_sink_srv_media_mgr_get_first_item(void)
{
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();
    uint16_t index = 0;
    uint32_t first_item = 0;

    index = ctx->decoder_ac_index;

    if ((ctx->iflag[index] & BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS)) {
        first_item = ctx->item[index];
    }

    return first_item;
}


int32_t bt_sink_srv_media_mgr_get_data_count(void)
{
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();

    return ctx->size;
}


uint32_t bt_sink_srv_media_mgr_pop_first_item(void)
{
    bt_sink_srv_media_mgr_context_t *ctx = bt_sink_srv_media_mgr_get_context();
    uint16_t index = 0;
    uint32_t first_item = 0;

    index = ctx->decoder_ac_index;

    if ((ctx->iflag[index] & BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS)) {
        first_item = ctx->item[index];
        /* Set free flag */
        BT_SINK_SRV_SET_FLAG(ctx->iflag[index], BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_FREE);
        /* Reset access flag */
        BT_SINK_SRV_REMOVE_FLAG(ctx->iflag[index], BT_SINK_SRV_MEDIA_MGR_ITEM_FLAG_ACCESS);
        ctx->decoder_ac_index = bt_sink_srv_media_mgr_update_index(index);
    }

    return first_item;
}

