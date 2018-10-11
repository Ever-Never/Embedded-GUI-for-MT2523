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

#include "bt_sink_srv_ami.h"

#include "stdint.h"
#include "bt_sink_srv_am_med_mgr.h"
#include "bt_sink_srv_utils.h"
#include "hal_nvic_internal.h"
#ifdef __BT_AWS_SUPPORT__
#include "bt_sink_srv_audio_sync.h"
#include "bt_sink_srv_music.h"
#endif

#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
#if 0
#define BT_SINK_SRV_A2DP_BS_LEN (100 * 1024)

uint8_t bt_sink_srv_a2dp_bitstream[BT_SINK_SRV_A2DP_BS_LEN];
uint32_t bt_sink_srv_a2dp_ptr = 0;
#endif
#endif

uint32_t log_switch = 0;
bt_sink_srv_am_med_mgr_context_t g_am_med_mgr_conx;
uint8_t *bt_sink_srv_am_release_buff = NULL;
uint8_t *bt_sink_srv_am_release_buff_2nd = NULL;
uint32_t bt_sink_srv_am_node_pos = 0;
void *bt_sink_srv_node = NULL;


static bt_sink_srv_am_med_mgr_context_t *bt_sink_srv_am_med_mgr_get_context(void);

static void bt_sink_srv_am_med_mgr_write_to_dsp_duplicate(volatile uint16_t *dst,
        uint16_t *src, uint32_t length);

#ifndef MTK_DEBUG_LEVEL_INFO
extern int32_t g_test_fix_warning;
#endif

void list_insert_node(list_t *head, list_t *node, list_dir dir)
{
    list_t *tmp = head;

    if (dir == LIST_BACK) {
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
    }

    node->next = tmp->next;
    tmp->next = node;
}


/* just care insert back for re-entry */
void list_insert_back(list_t *node)
{
    bt_sink_srv_am_med_mgr_context_t *med_mgr = NULL;
    list_t *tmp = NULL;

    med_mgr = bt_sink_srv_am_med_mgr_get_context();

    tmp = med_mgr->write; /* the last node */

    node->next = NULL;
    tmp->next = node;
}


list_t *list_remove_node(list_t *head, list_dir dir)
{
    list_t *n_node = head;
    list_t *nn_node = head->next; /* #ff_node is removing node. */

    if (nn_node != NULL) {
        if (dir == LIST_BACK) {
            while (nn_node->next != NULL) {
                n_node = nn_node;
                nn_node = nn_node->next;
            }
        }
        n_node->next = nn_node->next;
        nn_node->next = NULL;
    }

    return nn_node;
}


list_t *list_delete_node(list_t *head, list_t *node)
{
    list_t *hd = head;
    list_t *m_node = NULL, *n_node = NULL;

    while (hd->next != NULL) {
        if (hd->next == node) {
            m_node = hd;
            break;
        }
        hd = hd->next;
    }

    if (m_node) {
        m_node->next = node->next;
        n_node = node->next;
        node->next = NULL;
    }

    return n_node;
}


list_t *list_get_node(list_t *head, list_dir dir)
{
    //list_t *n_node = head;
    list_t *nn_node = head->next; /* #ff_node is geting node. */

    if (nn_node != NULL) {
        if (dir == LIST_BACK) {
            while (nn_node->next != NULL) {
                //n_node = nn_node;
                nn_node = nn_node->next;
            }
        }
    }

    return nn_node;
}


bool list_node_is_exist(list_t *head, list_t *node)
{
    bt_hci_packet_t *pkt = NULL;
    bool ret = false;
    list_t *n_node = head->next;

    bt_sink_srv_am_node_pos = 0;
    while (n_node != NULL) {
        pkt = (bt_hci_packet_t *)n_node;
        if (pkt->ref_count != 1) {
            bt_sink_srv_node = (void *)n_node;
            bt_sink_srv_assert(0);
        }
        if (n_node == node) {
            ret = true;
            break;
        }
        n_node = n_node->next;
        ++bt_sink_srv_am_node_pos;
    }

    return ret;
}


int32_t list_node_is_valid(void)
{
    bt_sink_srv_am_med_mgr_context_t *med_mgr = NULL;
    list_t *head = NULL, *n_node = NULL;
    int32_t ret = 0;
    bt_hci_packet_t *pkt = NULL;

    med_mgr = bt_sink_srv_am_med_mgr_get_context();

    head = &(med_mgr->med_head);

    n_node = head;
    //nn_node = head->next;
    while (n_node != NULL) {
        pkt = (bt_hci_packet_t *)n_node;
        if (pkt->ref_count != 1) {
            bt_sink_srv_node = (void *)n_node;
            bt_sink_srv_assert(0);
        }
        n_node = n_node->next;
        ++bt_sink_srv_am_node_pos;
    }

    return ret;
}


void list_clear(list_t *head)
{
    list_t *node = NULL;
    //bt_sink_srv_am_med_mgr_context_t *med_mgr = NULL;
    uint8_t *buff = NULL;

    //med_mgr = bt_sink_srv_am_med_mgr_get_context();

    while (head->next != NULL) {
        /* free node & release med node */
        node = list_get_node(head, LIST_FRONT);
        buff = (uint8_t *)node;
        list_remove_node(head, LIST_FRONT);
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
        bt_sink_srv_report("[sink][am][med]list_clear(release)--buff: 0x%x\n", buff);
#endif
        bt_sink_srv_am_release_buff = buff;
        bt_sink_srv_am_release_buff_2nd = buff;
        bt_a2dp_release_media_data_node(buff);
    }

    bt_sink_srv_am_release_buff = NULL;
    bt_sink_srv_am_release_buff_2nd = NULL;

#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
    //memset(bt_sink_srv_a2dp_bitstream, 0x00, sizeof(bt_sink_srv_a2dp_bitstream));
#endif
}


#if 0
void bt_sink_srv_am_med_report(const char *format, ...)
{
    if (log_switch & 0x0001) {
        //bt_sink_srv_report(format, ##__VA_ARGS__);
        bt_sink_srv_report(format, ...);
    } else {
        ;
    }
}
#endif


void bt_sink_srv_am_med_mgr_init(void)
{
    memset(&g_am_med_mgr_conx, 0x00, sizeof(bt_sink_srv_am_med_mgr_context_t));

    g_am_med_mgr_conx.read = NULL;
    g_am_med_mgr_conx.write = &(g_am_med_mgr_conx.med_head);

    BT_SINK_SRV_SET_FLAG(g_am_med_mgr_conx.flag, BT_SINK_SRV_AM_MED_MGR_FLAG_READY);
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
    bt_sink_srv_report("[sink][am][med]mgr_init--head: 0x%x\n", &g_am_med_mgr_conx.med_head);
#endif
}


static bt_sink_srv_am_med_mgr_context_t *bt_sink_srv_am_med_mgr_get_context(void)
{
    return &g_am_med_mgr_conx;
}


int32_t bt_sink_srv_am_med_mgr_push_data(void *ptr)
{
    //list_t *head;
    list_t *node = NULL;
    bt_sink_srv_am_med_mgr_context_t *med_mgr = NULL;
    bt_hci_packet_t *pkt = NULL;
    int32_t ret = 0;
    static uint32_t dis_cnt = 0;
    uint32_t int_mask = 0;


    med_mgr = bt_sink_srv_am_med_mgr_get_context();

    BT_SINK_SRV_SET_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_WRITING);

#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
    if (log_switch & 0x0001) {
        bt_sink_srv_report("push_data(start)--buf: 0x%x, cnt: %d, len: %d, r: 0x%x, w: 0x%x\n",
                           ptr, med_mgr->node_cnt, med_mgr->med_len, med_mgr->read, med_mgr->write);
    }
#endif

    if ((med_mgr->node_cnt > BT_SINK_SRV_AM_MED_MGR_MAX_COUNT) &&
            (med_mgr->med_len > BT_SINK_SRV_AM_MED_MGR_MAX_SIZE)) {
        if (log_switch & 0x0001) {
            bt_sink_srv_report("push_data(discard)--buf: 0x%x, cnt: %d, med_len: %d, dis_cnt: %d\n",
                               ptr, med_mgr->node_cnt, med_mgr->med_len, dis_cnt);
        }
        dis_cnt += 1;
        if (dis_cnt > 10) {
            //bt_sink_srv_assert(0);
        }
        //return ret;
    }

    if (med_mgr->med_len > BT_SINK_SRV_AM_MED_MGR_MAX_SIZE) {
        bt_sink_srv_report("med_push_data(discard)--buf: 0x%x, cnt: %d, med_len: %d\n",
                           ptr, med_mgr->node_cnt, med_mgr->med_len);
        ret = -101;
        return ret;
    }

    /* hold med node */
    bt_a2dp_hold_media_data_node((uint8_t *) ptr);

    //head = &(med_mgr->med_head);

    node = (list_t *)ptr;
    pkt = (bt_hci_packet_t *)ptr;

    bt_sink_srv_assert(node);
    node->next = NULL;

    /* disable int */
    int_mask = save_and_set_interrupt_mask();

    //list_insert_back(node);
    med_mgr->write->next = node;

    /* increase queue pkt len */
    med_mgr->med_len += (pkt->packet_length - pkt->offset);
    med_mgr->node_cnt += 1;

    /* update write node */
    med_mgr->write = node;

    /* enable int */
    restore_interrupt_mask(int_mask);

    BT_SINK_SRV_REMOVE_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_WRITING);

#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
    if (log_switch & 0x0001) {
        bt_sink_srv_report("push_data(end)--buf: 0x%x, pk_len: %d, offset: %d, cnt: %d, len: %d, r: 0x%x, w: 0x%x\n",
                           pkt, pkt->packet_length, pkt->offset, med_mgr->node_cnt,
                           med_mgr->med_len, med_mgr->read, med_mgr->write);
    }
#endif

    return ret;
}


int32_t bt_sink_srv_am_med_mgr_get_data_count(void)
{
    bt_sink_srv_am_med_mgr_context_t *med_mgr = NULL;
    int32_t data_count = 0;

    med_mgr = bt_sink_srv_am_med_mgr_get_context();

    data_count = (int32_t)(med_mgr->med_len);

    return data_count;
}


#ifdef __BT_AWS_SUPPORT__
int32_t bt_sink_srv_am_med_mgr_read_data(volatile uint16_t *buf, uint32_t length /* word length */)
{
    list_t *head, *node = NULL;
    bt_sink_srv_am_med_mgr_context_t *med_mgr = NULL;
    int32_t ret = 0;
    uint32_t read_len, write_len, real_len, len;
    bt_hci_packet_t *pkt = NULL;
    uint8_t *buff = NULL;
    uint8_t tmp = 0;
    list_t *read, *write;
    bt_sink_srv_music_device_t *aws_dev = NULL;
    uint32_t retSilenceNum = 0;
    bool retDrop = false;


    med_mgr = bt_sink_srv_am_med_mgr_get_context();

    len = length << 1;
    head = &(med_mgr->med_head);
    read_len = len;
    real_len = 0;

    med_mgr->read = list_get_node(head, LIST_FRONT);

    read = med_mgr->read;
    write = med_mgr->write;

    #ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
    if (log_switch & 0x0001) {
        bt_sink_srv_report("read_data(start)--r: 0x%x, w: 0x%x, buf: 0x%x, len: %d, cnt: %d, med_len: %d\n",
                           read, write, buf, length, med_mgr->node_cnt, med_mgr->med_len);
    }
    #endif

    aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
    aws_dev = NULL;

    if (aws_dev) {
        if ((med_mgr->flag & BT_SINK_SRV_AM_MED_MGR_FLAG_READY)) {
            while ((read_len > 0) && (med_mgr->med_len > 1) && (med_mgr->read) && (med_mgr->node_cnt > 1)) {
                if (med_mgr->sill_ptr) {
                    write_len = med_mgr->sill_len - med_mgr->sill_offset;
                    if (read_len >= write_len) {
                        bt_sink_srv_am_med_mgr_write_to_dsp_duplicate((buf + (real_len >> 1)),
                            (uint16_t *)(med_mgr->sill_ptr + med_mgr->sill_offset),
                            (write_len >> 1));
                        real_len = write_len;
                        med_mgr->sill_ptr = NULL;
                        med_mgr->sill_len = 0;
                        med_mgr->sill_offset = 0;
                        read_len -= write_len;
                        continue;
                    } else {
                        write_len = read_len;
                        bt_sink_srv_am_med_mgr_write_to_dsp_duplicate((buf + (real_len >> 1)),
                            (uint16_t *)(med_mgr->sill_ptr + med_mgr->sill_offset),
                            (write_len >> 1));
                        real_len = write_len;
                        med_mgr->sill_offset += write_len;
                        break;
                    }
                }
                /* get read node */
                read = med_mgr->read;
                node = read;
    
                if (med_mgr->reading.data) {
                    ;
                } else {
                    pkt = (bt_hci_packet_t *)node;
                    buff = (uint8_t *)node;
                    med_mgr->reading.data = buff + pkt->offset; /* calc ptr */
                    med_mgr->reading.length = (pkt->packet_length - pkt->offset);
                    med_mgr->reading.offset = 0;
                }

                if (med_mgr->flag & BT_SINK_SRV_AM_MED_MGR_FLAG_ODD_BTYE) {
                    if (0 == med_mgr->reading.offset) {
                        /* first check pkt */
                        bt_a2dp_plh_check_packet((bt_hci_packet_t *)node, &retSilenceNum, &retDrop);
                        if (retDrop) {
                            /* drop odd byte */
                            BT_SINK_SRV_REMOVE_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_ODD_BTYE);
                            med_mgr->odd_byte = 0x00;
                            /* free node */
                            buff = (uint8_t *)node;
                            /* update read node */
                            med_mgr->read = list_delete_node(head, node);
                            med_mgr->node_cnt -= 1;
                            if (med_mgr->read) {
                                /* no empty */
                                ;
                            } else {
                                /* reset write pointer */
                                med_mgr->write = &(med_mgr->med_head);
                            }
                            /* release med node */
                            #ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                            if (log_switch & 0x0001) {
                                bt_sink_srv_report("read_data_0-aws-odd(release)--buff: 0x%x\n", buff);
                            }
                            #endif
                            bt_sink_srv_am_release_buff = buff;
                            bt_a2dp_release_media_data_node(buff);
                            med_mgr->reading.data = NULL;
                            med_mgr->reading.offset = 0;
                            med_mgr->reading.length = 0;
                            continue;
                        } else if  (retSilenceNum > 0) {
                            /* drop odd byte */
                            BT_SINK_SRV_REMOVE_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_ODD_BTYE);
                            med_mgr->odd_byte = 0x00;
                            med_mgr->sill_len = bt_sink_srv_fill_silence_frame(&(med_mgr->sill_ptr), retSilenceNum);
                            bt_sink_srv_assert((med_mgr->sill_ptr));
                            med_mgr->sill_offset = 0;
                            continue;
                        } else {
                            /* node packet length odd(1) case */

                            /* 1. save odd context */
                            tmp = med_mgr->odd_byte;
                            BT_SINK_SRV_REMOVE_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_ODD_BTYE);
                            med_mgr->odd_byte = 0x00;
                            #ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                            if (log_switch & 0x0001) {
                                bt_sink_srv_report("read_data_1-aws-odd(release)--buff: 0x%x\n", buff);
                            }
                            #endif
                            /* 2. save odd context */
                            node = med_mgr->read;
                            pkt = (bt_hci_packet_t *)node;
                            pkt->offset -= 1;
                            buff = (uint8_t *)node;
                            buff[pkt->offset] = tmp;
                            /* 3. update reading info */
                            med_mgr->reading.data = buff + pkt->offset; /* calc ptr */
                            med_mgr->reading.length = (pkt->packet_length - pkt->offset);
                        }
                    } else {
                        bt_sink_srv_report("read_data(odd_err)--offset: %d\n", med_mgr->reading.offset);
                    }
                } else {
                    if (0 == med_mgr->reading.offset) {
                        /* first check pkt */
                        bt_a2dp_plh_check_packet((bt_hci_packet_t *)node, &retSilenceNum, &retDrop);
                        if (retDrop) {
                            /* free node */
                            buff = (uint8_t *)node;
                            /* update read node */
                            med_mgr->read = list_delete_node(head, node);
                            med_mgr->node_cnt -= 1;
                            if (med_mgr->read) {
                                /* no empty */
                                ;
                            } else {
                                /* reset write pointer */
                                med_mgr->write = &(med_mgr->med_head);
                            }
                            /* release med node */
                            #ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                            if (log_switch & 0x0001) {
                                bt_sink_srv_report("read_data_0-aws(release)--buff: 0x%x\n", buff);
                            }
                            #endif
                            bt_sink_srv_am_release_buff = buff;
                            bt_a2dp_release_media_data_node(buff);
                            med_mgr->reading.data = NULL;
                            med_mgr->reading.offset = 0;
                            med_mgr->reading.length = 0;
                            continue;
                        } else if  (retSilenceNum > 0) {
                            med_mgr->sill_len = bt_sink_srv_fill_silence_frame(&(med_mgr->sill_ptr), retSilenceNum);
                            bt_sink_srv_assert((med_mgr->sill_ptr));
                            med_mgr->sill_offset = 0;
                            continue;
                        } else {
                            /* normal case */
                            ;
                        }
                    }
                }
                /* calc write length */
                write_len = med_mgr->reading.length - med_mgr->reading.offset;
                if (write_len < read_len) {
                    ;
                } else {
                    write_len = read_len;
                }
    
                write_len  &= ~0x1; // make it even
    
                bt_sink_srv_am_med_mgr_write_to_dsp_duplicate((buf + (real_len >> 1)),
                        (uint16_t *)(med_mgr->reading.data + med_mgr->reading.offset),
                        (write_len >> 1));
                /* calc offset */
                real_len += write_len;
                med_mgr->reading.offset += write_len;
                read_len -= write_len;
                med_mgr->med_len -= write_len;
    
                write_len = med_mgr->reading.length - med_mgr->reading.offset;
                if (write_len == 1) {
                    /* node packet length odd(1) case */

                    /* 1. save odd context */
                    med_mgr->odd_byte = *(med_mgr->reading.data + med_mgr->reading.offset);
                    BT_SINK_SRV_SET_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_ODD_BTYE);

                    /* 2. free node & release med node */
                    buff = (uint8_t *)node;

                    /* update read node */
                    med_mgr->read = list_delete_node(head, node);
                    med_mgr->node_cnt -= 1;

                    if (med_mgr->read) {
                        /* no empty */
                        ;
                    } else {
                        /* reset write pointer */
                        med_mgr->write = &(med_mgr->med_head);
                    }

                    #ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                    if (log_switch & 0x0001) {
                        bt_sink_srv_report("read_data_1-aws(release)--buff: 0x%x\n", buff);
                    }
                    #endif
                    bt_sink_srv_am_release_buff = buff;
                    bt_a2dp_release_media_data_node(buff);
                    bt_a2dp_plh_write_frame_done((bt_hci_packet_t *)buff);
                    med_mgr->reading.data = NULL;
                    med_mgr->reading.offset = 0;
                    med_mgr->reading.length = 0;
                } else if (write_len == 0) {
                    /* free node */
                    buff = (uint8_t *)node;
    
                    /* update read node */
                    med_mgr->read = list_delete_node(head, node);
                    med_mgr->node_cnt -= 1;
    
                    if (med_mgr->read) {
                        /* no empty */
                        ;
                    } else {
                        /* reset write pointer */
                        med_mgr->write = &(med_mgr->med_head);
                    }
    
                    /* release med node */
                    #ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                    if (log_switch & 0x0001) {
                        bt_sink_srv_report("read_data_0(release)--buff: 0x%x\n", buff);
                    }
                    #endif
                    bt_sink_srv_am_release_buff = buff;
                    bt_a2dp_release_media_data_node(buff);
                    bt_a2dp_plh_write_frame_done((bt_hci_packet_t *)buff);
                    med_mgr->reading.data = NULL;
                    med_mgr->reading.offset = 0;
                    med_mgr->reading.length = 0;
                } else {
                    /* will jump out while loop */
                    #ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                    if (log_switch & 0x0001) {
                        bt_sink_srv_report("read_data(jump while loop)--read_len: %d, med_len: %d, cnt: %d\n",
                                           read_len, med_mgr->med_len, med_mgr->node_cnt);
                    }
                    #endif
                }
            }
        }
    } else {
    if ((med_mgr->flag & BT_SINK_SRV_AM_MED_MGR_FLAG_READY)) {
        while ((read_len > 0) && (med_mgr->med_len > 1) && (med_mgr->read) && (med_mgr->node_cnt > 1)) {
            /* get read node */
            read = med_mgr->read;
            node = read;

            if (med_mgr->reading.data) {
                ;
            } else {
                pkt = (bt_hci_packet_t *)node;
                buff = (uint8_t *)node;
                med_mgr->reading.data = buff + pkt->offset; /* calc ptr */
                med_mgr->reading.length = (pkt->packet_length - pkt->offset);
                med_mgr->reading.offset = 0;
            }

            /* calc write length */
            write_len = med_mgr->reading.length - med_mgr->reading.offset;
            if (write_len < read_len) {
                ;
            } else {
                write_len = read_len;
            }

            write_len  &= ~0x1; // make it even

            bt_sink_srv_am_med_mgr_write_to_dsp_duplicate((buf + (real_len >> 1)),
                    (uint16_t *)(med_mgr->reading.data + med_mgr->reading.offset),
                    (write_len >> 1));
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
#if 0
            if ((write_len + bt_sink_srv_a2dp_ptr) < BT_SINK_SRV_A2DP_BS_LEN) {
                memcpy(bt_sink_srv_a2dp_bitstream + bt_sink_srv_a2dp_ptr,
                       med_mgr->reading.data + med_mgr->reading.offset, write_len);
                bt_sink_srv_a2dp_ptr += write_len;
            }
#endif
#endif
            /* calc offset */
            real_len += write_len;
            med_mgr->reading.offset += write_len;
            read_len -= write_len;
            med_mgr->med_len -= write_len;

            write_len = med_mgr->reading.length - med_mgr->reading.offset;
            if (write_len == 1) {
                if (read->next) {
                    /* node packet length odd(1) case */

                    /* 1. save odd context */
                    tmp = *(med_mgr->reading.data + med_mgr->reading.offset);

                    /* 2. free node & release med node */
                    buff = (uint8_t *)node;

                    /* update read node */
                    med_mgr->read = list_delete_node(head, node);
                    med_mgr->node_cnt -= 1;

#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                    if (log_switch & 0x0001) {
                        bt_sink_srv_report("read_data_1(release)--buff: 0x%x\n", buff);
                    }
#endif
                    bt_sink_srv_am_release_buff = buff;
                    bt_a2dp_release_media_data_node(buff);
                    med_mgr->reading.data = NULL;
                    med_mgr->reading.offset = 0;
                    med_mgr->reading.length = 0;

                    /* 3. save odd context */
                    node = med_mgr->read;
                    pkt = (bt_hci_packet_t *)node;
                    pkt->offset -= 1;
                    buff = (uint8_t *)node;
                    buff[pkt->offset] = tmp;
                } else {
                    /* wait node insert */
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                    if (log_switch & 0x0001) {
                        bt_sink_srv_report("read_data(wait insert)-cnt: %d, med_len: %d\n", med_mgr->node_cnt, med_mgr->med_len);
                    }
#endif
                    break;
                }
            } else if (write_len == 0) {
                /* free node */
                buff = (uint8_t *)node;

                /* update read node */
                med_mgr->read = list_delete_node(head, node);
                med_mgr->node_cnt -= 1;

                if (med_mgr->read) {
                    /* no empty */
                    ;
                } else {
                    /* reset write pointer */
                    med_mgr->write = &(med_mgr->med_head);
                }

                /* release med node */
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                if (log_switch & 0x0001) {
                    bt_sink_srv_report("read_data_0(release)--buff: 0x%x\n", buff);
                }
#endif
                bt_sink_srv_am_release_buff = buff;
                bt_a2dp_release_media_data_node(buff);
                med_mgr->reading.data = NULL;
                med_mgr->reading.offset = 0;
                med_mgr->reading.length = 0;
            } else {
                /* will jump out while loop */
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                if (log_switch & 0x0001) {
                    bt_sink_srv_report("read_data(jump while loop)--read_len: %d, med_len: %d, cnt: %d\n",
                                       read_len, med_mgr->med_len, med_mgr->node_cnt);
                }
#endif
            }
        }
    }
    }
    ret = real_len >> 1;
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
    if (log_switch & 0x0001) {
        bt_sink_srv_report("read_data(end)--r: 0x%x, w: 0x%x, cnt: %d, med_len: %d, ret: %d\n",
                           med_mgr->read, med_mgr->write, med_mgr->node_cnt, med_mgr->med_len, ret);
    }
#endif
    return ret;
}
#endif /* __BT_AWS_SUPPORT__ */


#ifndef __BT_AWS_SUPPORT__
int32_t bt_sink_srv_am_med_mgr_read_data(volatile uint16_t *buf, uint32_t length /* word length */)
{
    list_t *head, *node = NULL;
    bt_sink_srv_am_med_mgr_context_t *med_mgr = NULL;
    int32_t ret = 0;
    uint32_t read_len, write_len, real_len, len;
    bt_hci_packet_t *pkt = NULL;
    uint8_t *buff = NULL;
    uint8_t tmp = 0;
    list_t *read, *write;

    med_mgr = bt_sink_srv_am_med_mgr_get_context();

    len = length << 1;
    head = &(med_mgr->med_head);
    read_len = len;
    real_len = 0;

    med_mgr->read = list_get_node(head, LIST_FRONT);

    read = med_mgr->read;
    write = med_mgr->write;

#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
    if (log_switch & 0x0001) {
        bt_sink_srv_report("read_data(start)--r: 0x%x, w: 0x%x, buf: 0x%x, len: %d, cnt: %d, med_len: %d\n",
                           read, write, buf, length, med_mgr->node_cnt, med_mgr->med_len);
    }
#endif

    if ((med_mgr->flag & BT_SINK_SRV_AM_MED_MGR_FLAG_READY)) {
        while ((read_len > 0) && (med_mgr->med_len > 1) && (med_mgr->read) && (med_mgr->node_cnt > 1)) {
            /* get read node */
            read = med_mgr->read;
            node = read;

            if (med_mgr->reading.data) {
                ;
            } else {
                pkt = (bt_hci_packet_t *)node;
                buff = (uint8_t *)node;
                med_mgr->reading.data = buff + pkt->offset; /* calc ptr */
                med_mgr->reading.length = (pkt->packet_length - pkt->offset);
                med_mgr->reading.offset = 0;
            }

            /* calc write length */
            write_len = med_mgr->reading.length - med_mgr->reading.offset;
            if (write_len < read_len) {
                ;
            } else {
                write_len = read_len;
            }

            write_len  &= ~0x1; // make it even

            bt_sink_srv_am_med_mgr_write_to_dsp_duplicate((buf + (real_len >> 1)),
                    (uint16_t *)(med_mgr->reading.data + med_mgr->reading.offset),
                    (write_len >> 1));
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
#if 0
            if ((write_len + bt_sink_srv_a2dp_ptr) < BT_SINK_SRV_A2DP_BS_LEN) {
                memcpy(bt_sink_srv_a2dp_bitstream + bt_sink_srv_a2dp_ptr,
                       med_mgr->reading.data + med_mgr->reading.offset, write_len);
                bt_sink_srv_a2dp_ptr += write_len;
            }
#endif
#endif
            /* calc offset */
            real_len += write_len;
            med_mgr->reading.offset += write_len;
            read_len -= write_len;
            med_mgr->med_len -= write_len;

            write_len = med_mgr->reading.length - med_mgr->reading.offset;
            if (write_len == 1) {
                if (read->next) {
                    /* node packet length odd(1) case */

                    /* 1. save odd context */
                    tmp = *(med_mgr->reading.data + med_mgr->reading.offset);

                    /* 2. free node & release med node */
                    buff = (uint8_t *)node;

                    /* update read node */
                    med_mgr->read = list_delete_node(head, node);
                    med_mgr->node_cnt -= 1;

#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                    if (log_switch & 0x0001) {
                        bt_sink_srv_report("read_data_1(release)--buff: 0x%x\n", buff);
                    }
#endif
                    bt_sink_srv_am_release_buff = buff;
                    bt_a2dp_release_media_data_node(buff);
                    med_mgr->reading.data = NULL;
                    med_mgr->reading.offset = 0;
                    med_mgr->reading.length = 0;

                    /* 3. save odd context */
                    node = med_mgr->read;
                    pkt = (bt_hci_packet_t *)node;
                    pkt->offset -= 1;
                    buff = (uint8_t *)node;
                    buff[pkt->offset] = tmp;
                } else {
                    /* wait node insert */
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                    if (log_switch & 0x0001) {
                        bt_sink_srv_report("read_data(wait insert)-cnt: %d, med_len: %d\n", med_mgr->node_cnt, med_mgr->med_len);
                    }
#endif
                    break;
                }
            } else if (write_len == 0) {
                /* free node */
                buff = (uint8_t *)node;

                /* update read node */
                med_mgr->read = list_delete_node(head, node);
                med_mgr->node_cnt -= 1;

                if (med_mgr->read) {
                    /* no empty */
                    ;
                } else {
                    /* reset write pointer */
                    med_mgr->write = &(med_mgr->med_head);
                }

                /* release med node */
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                if (log_switch & 0x0001) {
                    bt_sink_srv_report("read_data_0(release)--buff: 0x%x\n", buff);
                }
#endif
                bt_sink_srv_am_release_buff = buff;
                bt_a2dp_release_media_data_node(buff);
                med_mgr->reading.data = NULL;
                med_mgr->reading.offset = 0;
                med_mgr->reading.length = 0;
            } else {
                /* will jump out while loop */
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
                if (log_switch & 0x0001) {
                    bt_sink_srv_report("read_data(jump while loop)--read_len: %d, med_len: %d, cnt: %d\n",
                                       read_len, med_mgr->med_len, med_mgr->node_cnt);
                }
#endif
            }
        }
    }

    ret = real_len >> 1;
#ifdef __BT_SINK_SRV_AM_MED_LIST_DEBUG__
    if (log_switch & 0x0001) {
        bt_sink_srv_report("read_data(end)--r: 0x%x, w: 0x%x, cnt: %d, med_len: %d, ret: %d\n",
                           med_mgr->read, med_mgr->write, med_mgr->node_cnt, med_mgr->med_len, ret);
    }
#endif
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = (int32_t) write;
    #endif
    return ret;
}
#endif /* __BT_AWS_SUPPORT__ */


int32_t bt_sink_srv_am_med_mgr_clear_node(void)
{
    int32_t ret = 0;
    list_t *head = NULL;
    bt_sink_srv_am_med_mgr_context_t *med_mgr = NULL;

    med_mgr = bt_sink_srv_am_med_mgr_get_context();

    head = &(med_mgr->med_head);
    //med_mgr.flag &= ~(BT_SINK_SRV_AM_MED_MGR_FLAG_READY);
    //med_mgr.flag |= (BT_SINK_SRV_AM_MED_MGR_FLAG_CLEAR);

    BT_SINK_SRV_REMOVE_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_READY);
    BT_SINK_SRV_SET_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_CLEAR);

    list_clear(head);

    med_mgr->reading.data = NULL;
    med_mgr->reading.offset = 0;
    med_mgr->reading.length = 0;
    med_mgr->node_cnt = 0;
    med_mgr->med_len = 0;
    med_mgr->read = NULL;
    med_mgr->med_head.next = NULL;
    med_mgr->write = &(med_mgr->med_head);
    /* AWS */
    BT_SINK_SRV_REMOVE_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_ODD_BTYE);
    med_mgr->odd_byte = 0x00;
    med_mgr->sill_ptr = NULL;
    med_mgr->sill_len = 0;
    med_mgr->sill_offset = 0;

    BT_SINK_SRV_REMOVE_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_CLEAR);
    BT_SINK_SRV_SET_FLAG(med_mgr->flag, BT_SINK_SRV_AM_MED_MGR_FLAG_READY);

    return ret;
}


bool bt_sink_srv_am_med_mgr_node_is_exist(void *ptr)
{
    list_t *head, *node = NULL;
    bt_sink_srv_am_med_mgr_context_t *med_mgr = NULL;
    //bt_hci_packet_t *pkt = NULL;
    //int32_t ret = 0;
    bool is_exist = false;
    uint32_t int_mask = 0;

    med_mgr = bt_sink_srv_am_med_mgr_get_context();

    head = &(med_mgr->med_head);

    node = (list_t *)ptr;
    //pkt = (bt_hci_packet_t *)ptr;

    /* disable int */
    int_mask = save_and_set_interrupt_mask();

    is_exist = list_node_is_exist(head, node);

    /* enable int */
    restore_interrupt_mask(int_mask);

    return is_exist;
}


static void bt_sink_srv_am_med_mgr_write_to_dsp_duplicate(volatile uint16_t *dst,
        uint16_t *src, uint32_t length)
{
    uint32_t i;
    for (i = length; i > 0; i--) {
        *dst++ = *src++;
    }
}


void BT_A2DP_MAKE_SBC_CODEC(bt_a2dp_codec_capability_t *codec,
                            bt_a2dp_role_t role,
                            uint8_t min_bit_pool, uint8_t max_bit_pool,
                            uint8_t block_length, uint8_t subband_num,
                            uint8_t alloc_method, uint8_t sample_rate,
                            uint8_t channel_mode)
{
    do {
        codec->type = BT_A2DP_CODEC_SBC;
        codec->sep_type = role;
        codec->length = sizeof(bt_a2dp_sbc_codec_t);
        codec->codec.sbc.channel_mode = (channel_mode & 0x0F);
        codec->codec.sbc.sample_freq = (sample_rate & 0x0F);
        codec->codec.sbc.alloc_method = (alloc_method & 0x03);
        codec->codec.sbc.subbands = (subband_num & 0x03);
        codec->codec.sbc.block_len = (block_length & 0x0F);
        codec->codec.sbc.min_bitpool = (min_bit_pool & 0xFF);
        codec->codec.sbc.max_bitpool = (max_bit_pool & 0xFF);
    } while (0);
}


void BT_A2DP_MAKE_AAC_CODEC(bt_a2dp_codec_capability_t *codec,
                            bt_a2dp_role_t role, bool vbr, uint8_t object_type,
                            uint8_t channels, uint16_t sample_rate,
                            uint32_t bit_rate)
{
    do {
        codec->type = BT_A2DP_CODEC_AAC;
        codec->sep_type = role;
        codec->length = sizeof(bt_a2dp_aac_codec_t);
        codec->codec.aac.object_type = object_type;
        codec->codec.aac.freq_h = ((sample_rate >> 4) & 0xFF);
        codec->codec.aac.reserved = 0x03;
        codec->codec.aac.channels = channels;
        codec->codec.aac.freq_l = (sample_rate & 0x0F);
        codec->codec.aac.br_h = ((bit_rate >> 16) & 0x7F);
        codec->codec.aac.vbr = vbr;
        codec->codec.aac.br_m = ((bit_rate >> 8) & 0xFF);
        codec->codec.aac.br_l = (bit_rate & 0xFF);
    } while (0);
}


void BT_A2DP_CONVERT_SBC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec)
{
    dst_codec->type = BT_A2DP_CODEC_SBC;
    dst_codec->codec.sbc.min_bit_pool = src_codec->codec.sbc.min_bitpool;
    dst_codec->codec.sbc.max_bit_pool = src_codec->codec.sbc.max_bitpool;
    dst_codec->codec.sbc.block_length = src_codec->codec.sbc.block_len;
    dst_codec->codec.sbc.subband_num = src_codec->codec.sbc.subbands;
    dst_codec->codec.sbc.alloc_method = src_codec->codec.sbc.alloc_method;
    dst_codec->codec.sbc.sample_rate = src_codec->codec.sbc.sample_freq;
    dst_codec->codec.sbc.channel_mode = src_codec->codec.sbc.channel_mode;
    bt_sink_srv_report("[sink][am][med]CONVERT_SBC--min_pool: %d, max_pool: %d, b_len: %d, sub_num: %d, all_met: %d, samp_rate: %d, ch_m: %d\n",
                       dst_codec->codec.sbc.min_bit_pool, dst_codec->codec.sbc.max_bit_pool,
                       dst_codec->codec.sbc.block_length, dst_codec->codec.sbc.subband_num,
                       dst_codec->codec.sbc.alloc_method, dst_codec->codec.sbc.sample_rate,
                       dst_codec->codec.sbc.channel_mode);
}


void BT_A2DP_CONVERT_AAC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec)
{
    dst_codec->type = BT_A2DP_CODEC_AAC;
    dst_codec->codec.aac.vbr = src_codec->codec.aac.vbr;
    dst_codec->codec.aac.object_type = src_codec->codec.aac.object_type;
    dst_codec->codec.aac.channels = src_codec->codec.aac.channels;
    dst_codec->codec.aac.sample_rate = (src_codec->codec.aac.freq_h << 4) | (src_codec->codec.aac.freq_l);
    dst_codec->codec.aac.bit_rate = (src_codec->codec.aac.br_h << 16) | (src_codec->codec.aac.br_m << 8) | (src_codec->codec.aac.br_l);
    bt_sink_srv_report("[sink][am][med]CONVERT_AAC--vbr: %d, object_type: %d, channels: %d, sample_rate: %d, bit_rate: %d\n",
                       dst_codec->codec.aac.vbr, dst_codec->codec.aac.object_type,
                       dst_codec->codec.aac.channels, dst_codec->codec.aac.sample_rate,
                       dst_codec->codec.aac.bit_rate);
}

