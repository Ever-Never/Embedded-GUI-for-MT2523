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

 

#include "bt_notify_list.h"
#include <FreeRTOS.h>

/**
 * @brief          This function is for create a list.
 * @param[in]  void.
 * @return       the head of the list.
 */

Lnode *createList(void)
{
    Lnode *head;
    head = (Lnode *)pvPortMalloc(sizeof(Lnode));
    
    head->packet = NULL;
    head->packet_length = 0;
    head->next = NULL;
 
    return head;
}

/**
 * @brief          This function is for add a node into list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       void.
 */

void addNode(Lnode *head, uint8_t *packet, uint16_t packet_length) 
{//insert start
    Lnode *p = (Lnode*)pvPortMalloc(sizeof(Lnode));
    p->packet = packet;
    p->packet_length = packet_length;
    p->next = head->next;
    head->next = p;
}

/**
 * @brief          This function is for get the list length.
 * @param[in]  head      is the head of the list.
 * @return       the length of the list.
 */

uint32_t Length(Lnode *head)//get list length
{
    uint32_t n=0;
    Lnode *p;
    p=head->next;
    while(p)
    {
       n++;
       p=p->next;
    }
    return n;
}

/**
 * @brief          This function is for delete a node into list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       delete success or not.
 */

uint32_t deleteElem(Lnode * head, uint8_t *packet, uint16_t packet_length) 
{
    Lnode * p;
    Lnode * q;
    for(p = head; p != NULL; p = p->next) {
        if(p->next->packet == packet) {
            q = p->next;
            p->next = q->next;
            vPortFree(q);
            return 1; 
        }
    }
    return 0; 
}

/**
 * @brief          This function is for find a node into list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       the exit node or not.
 */

uint32_t FindNode(Lnode *head, uint8_t *packet, uint16_t packet_length) 
{
    Lnode *p;
    for(p = head->next; p != NULL; p = p->next) {
        if(p->packet == packet) 
            return 1; 
    }
    return 0; 
}

/**
 * @brief          This function is for add a node by the index of the list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       the node.
 */

Lnode *FindNodeByIndex(Lnode * head, int index)
{
	int i;
    Lnode * p;
    p = head->next;
	for(i = 0;i < index - 1;i ++)
	{
		
		if(NULL == p)
		{
			break;
		}
                 p = p->next;
	}

	if(NULL == p)
	{
		return NULL;
	}

	return p;
}


