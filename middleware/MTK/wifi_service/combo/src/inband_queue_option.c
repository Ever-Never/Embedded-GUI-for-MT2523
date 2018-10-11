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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inband_queue_option.h"


#if defined(__ICCARM__)
ATTR_4BYTE_ALIGN static unsigned char    tx_buf[SDIO_TX_OUTPUT_BUF_SIZE];
#else
static unsigned char    tx_buf[SDIO_TX_OUTPUT_BUF_SIZE] __attribute__((aligned(4)));
#endif



/* rsp_handler: NULL => use default handler */
int inband_queue_query(
    unsigned char   cmd_id,
    unsigned char   *buf,
    ssize_t         len,
    unsigned char   *ret_buf,
    ssize_t         out_len,
    unsigned char   is_from_isr,
    unsigned char   is_ext_cmd)
{
    int             err;
    uint8_t         status = 0;
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
#if defined(MTK_CM4_WIFI_TASK_ENABLE) || defined(MTK_WIFI_ROM_ENABLE)
    err = inband_enqueue_cmd_config_to_wifi_task(cmd_id, buf, len, (uint8_t *)&status,
                                    ret_buf, out_len, inband_default_rsp_handler,
                                    TRUE, TRUE, TRUE, is_ext_cmd);
#else
    err = inband_enqueue_cmd_config(cmd_id, buf, len, (uint8_t *)&status,
                                 ret_buf, out_len, inband_default_rsp_handler,
                                 TRUE, TRUE, TRUE, is_ext_cmd);
#endif
#else
	err = inband_enqueue_cmd_config(cmd_id, buf, len, (uint8_t *)&status,
								 ret_buf, out_len, inband_default_rsp_handler,
								 TRUE, TRUE, TRUE, is_ext_cmd);
#endif

    if (err < 0) {
        return err;
    }

    return status;
}


int inband_queue_exec(
    unsigned char   cmd_id,
    unsigned char   *buf,
    ssize_t         len,
    unsigned char   is_from_isr,
    unsigned char   is_ext_cmd)
{
    uint8_t         status = 0;
    int             err = 0;
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
#if defined(MTK_CM4_WIFI_TASK_ENABLE) || defined(MTK_WIFI_ROM_ENABLE)
    err = inband_enqueue_cmd_config_to_wifi_task(cmd_id, buf, len, (uint8_t *)&status, NULL,
                                    0, inband_default_rsp_handler, TRUE, TRUE,
                                    FALSE, is_ext_cmd);
#else
    err = inband_enqueue_cmd_config(cmd_id, buf, len, (uint8_t *)&status, NULL,
                                    0, inband_default_rsp_handler, TRUE, TRUE,
                                    FALSE, is_ext_cmd);
#endif
#else
	err = inband_enqueue_cmd_config(cmd_id, buf, len, (uint8_t *)&status, NULL,
									0, inband_default_rsp_handler, TRUE, TRUE,
									FALSE, is_ext_cmd);
#endif
    if (err < 0) {
        return err;
    }

    return status;
}


int inband_query_tx_frame(
    unsigned char   cmd_id,
    unsigned char   *buf,
    ssize_t         len,
    unsigned char   is_query,
    unsigned char   is_ext_cmd)
{
    int             status = 0;
    int             err    = 0;
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
#if defined(MTK_CM4_WIFI_TASK_ENABLE) || defined(MTK_WIFI_ROM_ENABLE)
    err = inband_enqueue_cmd_config_to_wifi_task(cmd_id, buf, len, (uint8_t *)&status, NULL,
                                    0, inband_default_rsp_handler, TRUE, TRUE,
                                    is_query, is_ext_cmd);
#else
    err = inband_enqueue_cmd_config(cmd_id, buf, len, (uint8_t *)&status, NULL,
                                    0, inband_default_rsp_handler, TRUE, TRUE,
                                    is_query, is_ext_cmd);
#endif
#else
	err = inband_enqueue_cmd_config(cmd_id, buf, len, (uint8_t *)&status, NULL,
									0, inband_default_rsp_handler, TRUE, TRUE,
									is_query, is_ext_cmd);
#endif
    if (err < 0) {
        return err;
    }

    return status;
}



int inband_default_rsp_handler(P_CONNSYS_INBAND_REQ_T req, unsigned int arg2, unsigned int arg3)
{
    unsigned char           *payload = (unsigned char *)arg2;
    P_INIT_WIFI_EVENT_T     p_rxd    = (P_INIT_WIFI_EVENT_T) payload;
    P_INIT_EVENT_CMD_RESULT p_result = (P_INIT_EVENT_CMD_RESULT)(p_rxd->aucBuffer);
    INIT_HIF_TX_HEADER_T    *p_txd   = (INIT_HIF_TX_HEADER_T *)payload;
    int                     len      = p_rxd->u2RxByteCount - sizeof(INIT_WIFI_EVENT_T) - sizeof(INIT_EVENT_CMD_RESULT);
    unsigned char           id_match = 0;
    ENUM_INBNAD_RSP_STATUS_T ret = INBAND_RSP_OK;

#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
	if (req->is_ext_cmd)
	{
		if (p_rxd->ucEID == 0xDF)
		{
			if (p_txd->rInitWifiCmd.ucExtenCID == req->cmd_id)
				id_match = 1;
		}
		else if (p_rxd->ucEID == 0xED)
		{
			if (p_txd->rInitWifiCmd.aucBuffer[0] == req->cmd_id)
				id_match = 1;
		}
	}
	else
	{
		if (req->cmd_id == p_rxd->ucEID) {
            id_match = 1;
        }
	}
#else
    if (req->is_ext_cmd) {
        if (p_rxd->ucEID == 0xDF)
            if (p_txd->rInitWifiCmd.ucExtenCID == req->cmd_id) {
                id_match = 1;
            }
    } else {
        if (req->cmd_id == p_rxd->ucEID) {
            id_match = 1;
        }
    }
#endif

    if (id_match && (req->seq == p_rxd->ucSeqNum)) {

        if (req->out_len > 0) {
            unsigned short min_len = req->out_len;
            if (req->out_buf == NULL) {
                ret = INBAND_RSP_NO_BUF;
            } else {
                if (len < req->out_len) {
                    min_len = len;
                    ret = INBAND_RSP_SHORT;
                } else if (len > req->out_len) {
                    ret = INBAND_RSP_LONG;
                }
                //DBG_INBAND(INBAND_DBG_MAIN, ("Get CMD[0x%x], results %d bytes\n", req->cmd_id, min_len));
                memcpy(req->out_buf, &(p_rxd->aucBuffer[sizeof(INIT_EVENT_CMD_RESULT)]), min_len);
            }
        }
        //DBG_INBAND(INBAND_DBG_MAIN, ("Get status=0x%x\n", p_result->ucStatus));
        req->ret_status = p_result->ucStatus;

        return (int)ret;
    }
    return (int)INBAND_NOT_RSP;
}

#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)

extern void wifi_generic_inband_callback(uint32_t data);

static void inband_send_cmd_to_wifi_task(inband_queue_tcb_t *iq_tcb,
	P_CONNSYS_INBAND_REQ_T req, unsigned char *in_buffer)
{
    unsigned char           cmd_id      = req->cmd_id;
  //  unsigned char           *buf        = req->in_buf;
    ssize_t                 in_len      = (ssize_t) req->in_len;
    ssize_t                 out_len     = (ssize_t) req->out_len;
    unsigned int            tx_len;
    P_INIT_HIF_TX_HEADER_T  p_txd;
 	unsigned char temp_buf[500] = {0};

    tx_len       = (out_len > in_len) ? out_len : in_len;
    tx_len      += sizeof(INIT_HIF_TX_HEADER_T);


	//inband_tx_buf_lock(iq_tcb);

    /*  buf = INIT_HIF_TX_HEADER + packet_data */
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
	//memset(tx_buf, 0, SDIO_TX_OUTPUT_BUF_SIZE);
#else
	memset(tx_buf, 0, SDIO_TX_OUTPUT_BUF_SIZE);
#endif
	//p_txd                               = (P_INIT_HIF_TX_HEADER_T)tx_buf;
	p_txd                               = (P_INIT_HIF_TX_HEADER_T)temp_buf;
    p_txd->u2TxByteCount                = tx_len;
    p_txd->u2PQ_ID                      = P1_Q0;
    p_txd->rInitWifiCmd.ucCID           = cmd_id;
	inband_tx_buf_lock(iq_tcb);

	req->seq = iq_tcb->inband_seq;
    p_txd->rInitWifiCmd.ucSeqNum        = iq_tcb->inband_seq++;
	inband_tx_buf_unlock(iq_tcb);

    p_txd->rInitWifiCmd.ucPktTypeID     = PKT_ID_CMD;

    if (req->is_ext_cmd) {
        p_txd->rInitWifiCmd.ucCID       = 0xDF; // for extend cmd
        p_txd->rInitWifiCmd.ucExtenCID  = cmd_id;
    }

    if (req->is_query) {
        p_txd->rInitWifiCmd.ucReserved  = 1;
    }

    if (req->is_query == CFG_INBAND_CMD_IS_QUERY_NEED_STATUS) {
        p_txd->rInitWifiCmd.ucReserved = CFG_INBAND_CMD_IS_QUERY_NEED_STATUS;
    }

    // fill in packet data
    if (in_buffer) {
        memmove(p_txd->rInitWifiCmd.aucBuffer, in_buffer, in_len + 1);
    }
	memmove(req->in_buf, temp_buf, in_len + 1 + sizeof(INIT_HIF_TX_HEADER_T));
	//inband_tx_buf_unlock(iq_tcb);

	// hex_dump("inband Supp", req->in_buf, in_len + 1 + sizeof (INIT_HIF_TX_HEADER_T));


    /* Note: enqueue wait req before send out inband cmd, otherwise,
     * we'll get resp too fast to enqueue wait req
     */
#if 0
    if (req->need_rsp) {
        inband_add_to_wait_list(iq_tcb, req);
    }
#endif
    DBG_INBAND(INBAND_DBG_MAIN, ("Send in-band (%d bytes), cmd(0x%x), extend cmd(0x%x), seq(%d), need_rsp(%u), wait_sema(0x%x)\n",
                                 tx_len,
                                 p_txd->rInitWifiCmd.ucCID,
                                 p_txd->rInitWifiCmd.ucExtenCID,
                                 p_txd->rInitWifiCmd.ucSeqNum,
                                 (unsigned int)req->need_rsp,
                                 (unsigned int)req->wait_sema));

	wifi_generic_inband_callback((uint32_t) req);

	return;
}
#endif


#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
extern unsigned char g_inband_87_enable;
#endif

void inband_send_cmd(inband_queue_tcb_t *iq_tcb, P_CONNSYS_INBAND_REQ_T req)
{
    unsigned char           cmd_id      = req->cmd_id;
    unsigned char           *buf        = req->in_buf;
    ssize_t                 in_len      = (ssize_t) req->in_len;
    ssize_t                 out_len     = (ssize_t) req->out_len;
    unsigned int            real_tx_len, tx_len, clear_len;
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    P_WIFI_CMD_FW_T  p_txd;
#else
	P_INIT_HIF_TX_HEADER_T  p_txd;
#endif
    signed int              flow_control_status;

    inband_tx_buf_lock(iq_tcb);

    tx_len       = (out_len > in_len) ? out_len : in_len;
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    tx_len      += sizeof(WIFI_CMD_FW_T);
#else
    tx_len      += sizeof(INIT_HIF_TX_HEADER_T);
#endif
    real_tx_len  = ALIGN_4BYTE(tx_len);

    if (real_tx_len > SDIO_TX_OUTPUT_BUF_SIZE) {
        LOG_E(inband, "inband_send_cmd: real_tx_len(%u) too long",
              real_tx_len);
        inband_tx_buf_unlock(iq_tcb);
        return;
    }

    clear_len = real_tx_len + WIFI_HIF_TX_BYTE_CNT_LEN;

    if (clear_len > SDIO_TX_OUTPUT_BUF_SIZE) {
        clear_len = SDIO_TX_OUTPUT_BUF_SIZE;
    }

    memset(tx_buf, 0, SDIO_TX_OUTPUT_BUF_SIZE);
    req->seq = iq_tcb->inband_seq;

    /*
     * tx_buf = INIT_HIF_TX_HEADER + packet_data
     */
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    p_txd                               = (P_WIFI_CMD_FW_T)tx_buf;
#else
    p_txd                               = (P_INIT_HIF_TX_HEADER_T)tx_buf;
#endif
    p_txd->u2TxByteCount                = tx_len;
    p_txd->u2PQ_ID                      = P1_Q0;
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    p_txd->ucCID				        = cmd_id;
    p_txd->ucSeqNum				        = iq_tcb->inband_seq++;
    p_txd->ucPktTypeID				    = PKT_ID_CMD;

		p_txd->ucExtCmdOption 				= EXT_CMD_OPT_BIT_0_ACK;
#else
    p_txd->rInitWifiCmd.ucCID           = cmd_id;
    p_txd->rInitWifiCmd.ucSeqNum        = iq_tcb->inband_seq++;
    p_txd->rInitWifiCmd.ucPktTypeID     = PKT_ID_CMD;
#endif
    if (req->is_ext_cmd) {
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
		if(g_inband_87_enable)
		{
			p_txd->ucCID       = 0xDF; // for extend cmd
		}
		else
		{
        	p_txd->ucCID       = 0xED; // for extend cmd
		}
#else
		p_txd->rInitWifiCmd.ucCID		= 0xDF; // for extend cmd
#endif
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
        p_txd->ucExtenCID  = cmd_id;
#else
        p_txd->rInitWifiCmd.ucExtenCID  = cmd_id;
#endif
    }

    if (req->is_query) {
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
        p_txd->ucReserved  = 1;
#else
        p_txd->rInitWifiCmd.ucReserved  = 1;
#endif
    }

    if (req->is_query == CFG_INBAND_CMD_IS_QUERY_NEED_STATUS) {
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
        p_txd->ucReserved = CFG_INBAND_CMD_IS_QUERY_NEED_STATUS;
#else
        p_txd->rInitWifiCmd.ucReserved = CFG_INBAND_CMD_IS_QUERY_NEED_STATUS;
#endif
    }

    // fill in packet data
    if (buf) {
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
        memmove(p_txd->aucBuffer, buf, in_len);
#else
        memmove(p_txd->rInitWifiCmd.aucBuffer, buf, in_len);
#endif
    }

    memset((uint8_t *)tx_buf + tx_len, 0, clear_len - tx_len);

    /* Note: enqueue wait req before send out inband cmd, otherwise,
     * we'll get resp too fast to enqueue wait req
     */

    if (req->need_rsp) {
        inband_add_to_wait_list(iq_tcb, req);
    }

#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    DBG_INBAND(INBAND_DBG_MAIN, ("Send in-band (%d bytes), cmd(0x%x), extend cmd(0x%x), seq(%d), need_rsp(%u), wait_sema(0x%x)\n",
                                 tx_len,
                                 p_txd->ucCID,
                                 p_txd->ucExtenCID,
                                 p_txd->ucSeqNum,
                                 (unsigned int)req->need_rsp,
                                 (unsigned int)req->wait_sema));
#else
    DBG_INBAND(INBAND_DBG_MAIN, ("Send in-band (%d bytes), cmd(0x%x), extend cmd(0x%x), seq(%d), need_rsp(%u), wait_sema(0x%x)\n",
                                 tx_len,
                                 p_txd->rInitWifiCmd.ucCID,
                                 p_txd->rInitWifiCmd.ucExtenCID,
                                 p_txd->rInitWifiCmd.ucSeqNum,
                                 (unsigned int)req->need_rsp,
                                 (unsigned int)req->wait_sema));
#endif
    flow_control_status = connsys_tx_flow_control_check_and_update_tx(WIFI_HIF_RX_PORT0_IDX, real_tx_len);

    if (flow_control_status == CONNSYS_STATUS_SUCCESS) {
        connsys_util_tx_data((unsigned char *)tx_buf, real_tx_len);
    }

    inband_tx_buf_unlock(iq_tcb);
}

#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)

/* rsp_handler: NULL => use default handler */
int inband_enqueue_cmd_config_to_wifi_task(
    unsigned char           cmd_id,
    unsigned char           *buf,
    ssize_t                 len,
    unsigned char           *status,
    unsigned char           *ret_buf,
    ssize_t                 out_len,
    P_INBAND_RSP_HANDLER    rsp_handler,
    unsigned char           need_rsp,
    unsigned char           need_wait,
    unsigned char           is_query,
    unsigned char           is_ext_cmd)
{
	unsigned int			tx_len;
	P_CONNSYS_INBAND_REQ_T	req = NULL;
	int 					ret = 0;

	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
		return -1;
	}

	tx_len		= len + sizeof(INIT_HIF_TX_HEADER_T);
	if (need_wait) {
		need_rsp = TRUE;
	}
	//inband_tx_buf_lock(iq_tcb);
	req = inband_malloc(iq_tcb);
#if 0
	inband_crit_enter();
	DBG_INBAND(INBAND_DBG_MAIN, ("inband_enqueue_cmd_config ==> rsp len = %u, free len = %u\n",
								 DlListLen(&iq_tcb->wait_list), DlListLen(&iq_tcb->idle_list)));
	inband_crit_exit();
#endif
	if (req == NULL) {
		// IOT_DBGPRINT_ERR("inband_enq_cmd: Req buf null\n");
		//inband_tx_buf_unlock(iq_tcb);
		return -1;
	}

	req->cmd_id = cmd_id;
	req->in_len = len;
	req->in_buf = (uint8_t *)malloc(tx_len + 1);
#if 0
	if (buf != NULL) {
		// req->in_buf = malloc(tx_len + 1);
		// memcpy(req->in_buf, buf, len + 1); Anil modify this op
	} else {
		req->in_buf = NULL;
	}
#endif
	req->out_len	= out_len;
	req->out_buf	= ret_buf;
	req->need_rsp	= need_rsp;
	req->wait_sema	= NULL;
	req->rsp_handler = NULL; // inband_default_rsp_handler Anil chk
	req->ret_status = 0;
	req->is_query	= is_query;
	req->completed	= 0;
	req->is_ext_cmd = is_ext_cmd;
	LOG_I(inband, "START(%s):is_ext %d cmd_id %d in_len %d out_len %d\n",pcTaskGetTaskName(xTaskGetCurrentTaskHandle()),
		req->is_ext_cmd, req->cmd_id, len, out_len);
	//LOG_E(inband, "is_ext_cmd %d cmd_id 0x%x\n", is_ext_cmd, cmd_id);
	//LOG_E(inband, "buf %p in_buf %p out_buf %p len %d\n", buf, req->in_buf, req->out_buf, req->out_len);
	if (need_wait) {
		req->rsp_handler = inband_default_rsp_handler;
		/* init semaphore */
		req->wait_sema = xSemaphoreCreateBinary();

		if (req->wait_sema == NULL) {
			// IOT_DBGPRINT_ERR("inband_enq_cmd: insuff openRTOS heap available\n");
		}
	}
	inband_send_cmd_to_wifi_task(iq_tcb,req, buf);
	if (req->wait_sema) {
		DBG_INBAND(INBAND_DBG_MAIN, ("wait_sema handle = 0x%x, name = %s\n",
									 (unsigned int)xTaskGetCurrentTaskHandle(),
									 pcTaskGetTaskName(xTaskGetCurrentTaskHandle())));

		// See if we can obtain the semaphore.	If the semaphore is not available
		// wait 1000  ticks to see if it becomes free.
		//LOG_I(inband, "Wait for RSP on sema...\n");

		if (xSemaphoreTake((SemaphoreHandle_t)req->wait_sema,
						   (TickType_t)CFG_INBAND_CMD_RSP_WAIT_TIME_VAL) == pdTRUE) {
			// We were able to obtain the semaphore and can now access the
			// shared resource.
			// ...
			*status = req->ret_status;
				LOG_I(inband, "inband success\n");
		} else {
			LOG_E(inband, "timeout: fail to get response. cmd:[0x%0x], task:[%s]", cmd_id, pcTaskGetTaskName(NULL));
			ret = -2;
		}
	} else if (need_rsp) {
		volatile int loop = 0;

		/* busy waiting for rsp */
		DBG_INBAND(INBAND_DBG_MAIN, ("Busy looping for rsp handle = 0x%x, name = %s\n", (unsigned int)xTaskGetCurrentTaskHandle(),
									 pcTaskGetTaskName(xTaskGetCurrentTaskHandle())));
		while (!req->completed) {
			vTaskDelay(CFG_INBAND_CMD_RSP_WAIT_PERIOD_VAL);
			if (loop++ > (CFG_INBAND_CMD_RSP_WAIT_TIME_VAL / CFG_INBAND_CMD_RSP_WAIT_PERIOD_VAL)) {
				LOG_W(inband, "WARN! wait rsp timeout for %u seconds... ",
					  CFG_INBAND_CMD_RSP_WAIT_TIME_VAL / 1000);
				break;
			}
		}
		DBG_INBAND(INBAND_DBG_MAIN, ("Completed.\n"));
	}
	inband_mfree(iq_tcb, req);
	// inband_tx_buf_unlock(iq_tcb);
	return ret;
}
#endif

#ifdef MT5932_SINGLE_CONTEXT
int inband_queue_inband_handler(void *pkt_ptr, unsigned char *payload, unsigned int len)
{
    P_CONNSYS_INBAND_REQ_T  req     = NULL;
    P_CONNSYS_INBAND_REQ_T  tmp     = NULL;
    unsigned char           handled = INBAND_HANDLE_NON_HANDLE;
    P_INIT_WIFI_EVENT_T     p_rxd   = (P_INIT_WIFI_EVENT_T) payload;
    ENUM_INBNAD_RSP_STATUS_T ret    = INBAND_NOT_RSP;

#if (CONNSYS_DEBUG_MODE_EN == 1)
#ifndef MTK_WIFI_DIRECT_ENABLE
    LOG_I(inband, "=> *RX inband evt, ID(0x%x), len(%d), seq(%d)", p_rxd->ucEID, p_rxd->u2RxByteCount, p_rxd->ucSeqNum);
#endif

    if (p_rxd->ucEID == 0 && p_rxd->u2RxByteCount == 0) {
        LOG_E(inband, "ERROR! PSE overflow !!");
    }

    if (p_rxd->u2PacketType != PKT_ID_EVENT) {
        LOG_HEXDUMP_W(inband, "u2PacketType not equal to 0xe000, pkt:", payload, len);
    }
#endif

    /* 1. Check for CMD response */
    wifi_os_task_enter_critical();

    DlListForEachSafe(req, tmp, &iq_tcb->wait_list, CONNSYS_INBAND_REQ_T, list) {
        ret = (ENUM_INBNAD_RSP_STATUS_T)req->rsp_handler(req, (unsigned int)payload, (unsigned int)len);
        if (ret !=  INBAND_NOT_RSP) {
            /* match and rsp data handled */

            /* remove this request from queue list */
            DlListDel(&req->list);
            req->completed = 1;
            if (req->wait_sema) {
                /* Trigger blocked task to run and free item */
                xSemaphoreGive(req->wait_sema);
                taskYIELD();
            }
            handled = INBAND_HANDLE_RSP;
            break;
        }
    }
     wifi_os_task_exit_critical();

    //Move inband default handler response log out of task critical section.
     if (ret !=  INBAND_NOT_RSP) {
        switch (ret) {
            case INBAND_RSP_NO_BUF:
                LOG_E(inband, "No expected return buffer,expected return len=%d", req->out_len);
                inband_show_wifi_event_format(payload);
                break;
            case INBAND_RSP_SHORT:
                LOG_W(inband, "No enough data returned,expected return len=%d", req->out_len);
                inband_show_wifi_event_format(payload);
                break;
            case INBAND_RSP_LONG:
                LOG_W(inband, "Too long data returned,expected return len=%d", req->out_len);
                inband_show_wifi_event_format(payload);
                break;
            default:
                break;
        }
    }

    if ((p_rxd->aucReserved[0] & 0xF) == 1) { // if PSE_RESET, may not APstart cmd rsp, but set in MacResponse due to N9 response flow
        connsys_tx_flow_control_init();
        LOG_W(inband, "PSE is reseted, cmd(0x%x), seq(%d)", p_rxd->ucEID, p_rxd->ucSeqNum);
    }

    return handled;
}
#endif

#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)

void inband_cm4_evt_handle(void* pkt_ptr, unsigned char *payload, unsigned int len)
{
	inband_queue_handler(pkt_ptr, payload, len);
	net_pkt_free_evt(pkt_ptr);
	return;
}

int inband_evt_handle_from_CM4(void *skb, P_INIT_WIFI_EVENT_T  frame)
{
		const inband_evt_item_t   *item;

		item = inband_evt_get_handler(frame->ucEID);

		if (item) // for events
		{
			unsigned int len = frame->u2RxByteCount - sizeof(INIT_WIFI_EVENT_T) - sizeof(INIT_EVENT_CMD_RESULT);
			inband_job_add(inband_cm4_evt_handle,
						   skb,
						   (unsigned char *)frame,
						   len);
			return 1;
		}
	return 0;
}
#endif

#ifndef MTK_WIFI_CONFIGURE_FREE_ENABLE
/*
*Temp modification for wifi_config_free remove to src
*Need to add source for KEIL/IAR and remove this.
*/
int wifi_cf_credential_handler(unsigned int inf, unsigned char *body, int len)
{
    LOG_W(inband,"Not support config free handler");
    return 0;
}
#endif

typedef enum
{
	WIFI_EXT_ID_PS,
	WIFI_EXT_ID_TEMP_CALIB,
} wifi_ext_id_t;
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
extern void send_wifi_temp_calib_trigger_to_wifi_task(wifi_ext_id_t id);
#endif
int inband_queue_evt_handler(void *pkt_ptr, unsigned char *payload, unsigned int len)
{
    P_CONNSYS_INBAND_REQ_T  req     = NULL;
    P_CONNSYS_INBAND_REQ_T  tmp     = NULL;
    unsigned char           handled = INBAND_HANDLE_NON_HANDLE;
    P_INIT_WIFI_EVENT_T     p_rxd   = (P_INIT_WIFI_EVENT_T) payload;
    ENUM_INBNAD_RSP_STATUS_T ret    = INBAND_NOT_RSP;
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    INIT_HIF_TX_HEADER_T    *p_txd   = (INIT_HIF_TX_HEADER_T *)payload;
#endif

#if (CONNSYS_DEBUG_MODE_EN == 1)
#ifndef MTK_WIFI_DIRECT_ENABLE
    LOG_I(inband, "=> *RX inband evt, ID(0x%x), len(%d), seq(%d)", p_rxd->ucEID, p_rxd->u2RxByteCount, p_rxd->ucSeqNum);
#endif

    if (p_rxd->ucEID == 0 && p_rxd->u2RxByteCount == 0) {
        LOG_E(inband, "ERROR! PSE overflow !!");
    }

    if (p_rxd->u2PacketType != PKT_ID_EVENT) {
        LOG_HEXDUMP_W(inband, "u2PacketType not equal to 0xe000, pkt:", payload, len);
    }
#endif

#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    /* 1. Check for CMD response */
	if (((p_txd->rInitWifiCmd.aucBuffer[0] == 0x01) || (p_txd->rInitWifiCmd.aucBuffer[0] == 0x00)) && (p_rxd->ucEID == 0xED))
	{
		send_wifi_temp_calib_trigger_to_wifi_task(WIFI_EXT_ID_TEMP_CALIB);
		return handled;
	}
#endif

    wifi_os_task_enter_critical();

    DlListForEachSafe(req, tmp, &iq_tcb->wait_list, CONNSYS_INBAND_REQ_T, list) {
        ret = (ENUM_INBNAD_RSP_STATUS_T) req->rsp_handler(req, (unsigned int)payload, (unsigned int)len);
        if (ret != INBAND_NOT_RSP) {
            /* match and rsp data handled */

            /* remove this request from queue list */
            DlListDel(&req->list);
            req->completed = 1;
            if (req->wait_sema) {
                /* Trigger blocked task to run and free item */
                wifi_os_semphr_give(req->wait_sema);
                switch_context();
            }
            handled = INBAND_HANDLE_RSP;
            break;
        }
    }
    wifi_os_task_exit_critical();

    //Move inband default handler response log out of task critical section.
     if (ret !=  INBAND_NOT_RSP) {
        switch (ret) {
            case INBAND_RSP_NO_BUF:
                LOG_E(inband, "No expected return buffer,expected return len=%d", req->out_len);
                inband_show_wifi_event_format(payload);
                break;
            case INBAND_RSP_SHORT:
                LOG_W(inband, "No enough data returned,expected return len=%d", req->out_len);
                inband_show_wifi_event_format(payload);
                break;
            case INBAND_RSP_LONG:
                LOG_W(inband, "Too long data returned,expected return len=%d", req->out_len);
                inband_show_wifi_event_format(payload);
                break;
            default:
                break;
        }
    }

    if ((p_rxd->aucReserved[0] & 0xF) == 1) { // if PSE_RESET, may not APstart cmd rsp, but set in MacResponse due to N9 response flow
        connsys_tx_flow_control_init();
        LOG_W(inband, "PSE is reseted, cmd(0x%x), seq(%d)", p_rxd->ucEID, p_rxd->ucSeqNum);
    }

    /* 2. Check for registered inband evt handler */
    if (INBAND_HANDLE_NON_HANDLE == handled) {
        const inband_evt_item_t   *item;

        item = inband_evt_get_handler(p_rxd->ucEID);

        if (item) {
            int job_len = p_rxd->u2RxByteCount - sizeof(INIT_WIFI_EVENT_T) - sizeof(INIT_EVENT_CMD_RESULT);
            inband_job_add(iq_tcb->handler,
                           pkt_ptr,
                           payload,
                           job_len);
            switch_context();
            handled = INBAND_HANDLE_EVENT_HANDLER;
        }
    }

    if (INBAND_HANDLE_NON_HANDLE == handled) {
        LOG_W(inband, "u2PacketType(0x%x), ucEID(0x%x), ucSeqNum(0x%x) not handled!",
              p_rxd->u2PacketType,
              p_rxd->ucEID,
              p_rxd->ucSeqNum);
    }

    return handled;
}

