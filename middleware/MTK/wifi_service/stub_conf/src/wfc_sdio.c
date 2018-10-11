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
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

#include "wfc_stub.h"
#include "wfc_sdio.h"

/* hal includes */
#include "memory_attribute.h"
#include "mt7686.h"
#include "hal.h"
#include "sdio_hal.h"
#include "sdio_reg.h"
#include "lwip/pbuf.h"
#include "wifi_api.h"
#include "wifi_os_api.h"

#include "hal_gpio_internal.h"
#include "hal_gpio.h"



#ifndef WFC_HRX_POLLING
#if 0 //For MT7686
#define WFC_HRX_EINT  HAL_GPIO_17
#else //For MT5932
//#define WFC_HRX_EINT  HAL_GPIO_2
//#define WFC_HRX_EINT  HAL_GPIO_17
#define WFC_HRX_EINT  HAL_GPIO_4
#endif
#endif


#ifndef ALIGN_4
#define ALIGN_4(_value)             (((_value) + 3) & ~3u)
#endif /* ALIGN_4 */


#define MEM_QUEUE_SIZE             1 //8
#define SDIO_CMD_SIZE            256
#define SDIO_CMD_RSP_SIZE        256
#define SDIO_HRX_DATA_SIZE      1600
#define SDIO_HTX_DATA_SIZE      1600
#define SDIO_SLAVE_EVT_SIZE     1024

#define BROM_BLOCK_SIZE  (256)


T_WFC_QBUF   g_htx_free_qbuf;

T_WFC_QBUF  *g_hrx_qbuf;
T_WFC_QBUF  *plist_hrx;

os_semaphore_t g_sem_qbuf;

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_cmd_rsp_buf[SDIO_CMD_RSP_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_host_rx_buf[MEM_QUEUE_SIZE][SDIO_HRX_DATA_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_host_tx_buf[MEM_QUEUE_SIZE][SDIO_HTX_DATA_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_slave_rx_buf[MEM_QUEUE_SIZE][SDIO_SLAVE_EVT_SIZE];

volatile hal_sdio_slave_callback_event_t test_event = HAL_SDIO_SLAVE_EVENT_NONE;
//uint32_t   g_rx0_done_event = 0;

uint8_t g_memq_list[MEMQ_CTRL_MAX_NUM][MEM_QUEUE_SIZE];

uint8_t wfc_mq_send_queue(T_WFC_QBUF *qbuf, uint32_t fromISR);
uint8_t wfc_send_queue(uint16_t op, uint16_t cmd, uint8_t *ptr, uint32_t ptrSize, uint8_t fromISR);


void wfc_mq_push_hrx(T_WFC_QBUF *qbuf)
{
    T_WFC_QBUF   *pcur;
    T_WFC_QBUF   *pnext;

    wifi_os_semphr_take(g_sem_qbuf, NULL);
    qbuf->next = NULL;

    if (!plist_hrx) {
        plist_hrx = qbuf;
    } else {
        pcur  = plist_hrx;
        pnext = plist_hrx->next;
        while (pnext) {
            pcur = pnext;
            pnext = pcur->next;
        }
        pcur->next = qbuf;
    }
    wifi_os_semphr_give(g_sem_qbuf);
}


T_WFC_QBUF  *wfc_mq_pop_hrx(void)
{
    T_WFC_QBUF *pdst;

    wifi_os_semphr_take(g_sem_qbuf, NULL);
    pdst = plist_hrx;
    if (pdst) {
        plist_hrx = pdst->next;
    }
    wifi_os_semphr_give(g_sem_qbuf);

    return pdst;
}


int32_t wfc_mq_get_buf(uint32_t ctrl_idx, uint32_t *qidx)
{
    uint32_t idx;

    if (ctrl_idx == MEMQ_CTRL_HOST_RX) {
        wifi_os_semphr_take(g_sem_qbuf, NULL);
    }
    for (idx = 0; idx < MEM_QUEUE_SIZE; idx++) {
        if (g_memq_list[ctrl_idx][idx] == 0) {
            g_memq_list[ctrl_idx][idx] = 1; //allocated
            *qidx = idx;
            if (ctrl_idx == MEMQ_CTRL_HOST_RX) {
                wifi_os_semphr_give(g_sem_qbuf);
            }
            return idx;
        }
    }
    *qidx = WFC_MEMQ_INVALID;
    if (ctrl_idx == MEMQ_CTRL_HOST_RX) {
        wifi_os_semphr_give(g_sem_qbuf);
    }
    return -1;
}


void wfc_mq_clear(T_WFC_QBUF *qbuf)
{
    qbuf->qidx = WFC_MEMQ_INVALID;
    qbuf->size = 0;
}


void wfc_mq_free(T_WFC_QBUF *qbuf)
{
    if (qbuf->ctrl == MEMQ_CTRL_HOST_RX) {
        wifi_os_semphr_take(g_sem_qbuf, NULL);
    }
    if((qbuf->ctrl != MEMQ_CTRL_HOST_RX) && (qbuf->ctrl != MEMQ_CTRL_SLAVE_EVT) &&
        (qbuf->ctrl < MEMQ_CTRL_MAX_NUM) && (qbuf->qidx < MEM_QUEUE_SIZE)) { //memory leakage protection
        g_memq_list[qbuf->ctrl][qbuf->qidx] = 0; //free
        qbuf->qidx = WFC_MEMQ_INVALID;
    }
    qbuf->size = 0;
    if (qbuf->ctrl == MEMQ_CTRL_HOST_RX) {
        wifi_os_semphr_give(g_sem_qbuf);
    }
}


void wfc_sdio_mq_reset(uint32_t ctrl)
{
    if (ctrl == MEMQ_CTRL_HOST_TX) {
        g_htx_free_qbuf.ctrl = MEMQ_CTRL_HOST_TX;
        g_htx_free_qbuf.qidx = WFC_MEMQ_INVALID;
    }
}


void wfc_dump_buf(uint8_t *pbuf, uint32_t bufSize)
{
    // Dump data received (Debug)
    printf("\r\n[Dump]: Data Length == %lu\r\n", bufSize);
    for (int i = 0; i < bufSize; i++) {
        printf("0x%02x ", pbuf[i]);
    }
    printf("\r\n\r\n\r\n");
}


#ifndef WFC_HRX_POLLING
void wfc_rx_notify(void)
{
    hal_gpio_set_output(WFC_HRX_EINT, HAL_GPIO_DATA_HIGH);
    hal_gpio_set_output(WFC_HRX_EINT, HAL_GPIO_DATA_LOW);
}
#endif


void wfc_dma_memcpy(uint8_t *dst, uint8_t *src, unsigned int len)
{
    hal_gdma_status_t ret;
    hal_gdma_running_status_t running_status;
    hal_gdma_channel_t channel = HAL_GDMA_CHANNEL_0;

    hal_gdma_init(HAL_GDMA_CHANNEL_0);

    ret = hal_gdma_start_polling(channel, (uint32_t)dst, (uint32_t)src, len);
    if (HAL_GDMA_STATUS_OK != ret) {
        //printf("DMA unavailable...\n");
        memcpy(dst, src, len);
    }
    hal_gdma_get_running_status(channel, &running_status);
    hal_gdma_stop(HAL_GDMA_CHANNEL_0); // Stop the GDMA.
    hal_gdma_deinit(HAL_GDMA_CHANNEL_0);   //De-initialize the GDMA.
}


/*
void sdio_rx0_done_add(uint8_t fromIsr)
{
	if (fromIsr)
		wifi_os_semphr_take_from_isr(g_sem_rx0);
	else
		wifi_os_semphr_take(g_sem_rx0, NULL);

	g_rx0_done_event++;

	if (fromIsr)
		wifi_os_semphr_give_from_isr(g_sem_rx0);
	else
		wifi_os_semphr_give(g_sem_rx0);
}

void sdio_rx0_done_free(uint8_t fromIsr)
{
	if (fromIsr)
		wifi_os_semphr_take_from_isr(g_sem_rx0);
	else
		wifi_os_semphr_take(g_sem_rx0, NULL);

	g_rx0_done_event--;

	if (fromIsr)
		wifi_os_semphr_give_from_isr(g_sem_rx0);
	else
		wifi_os_semphr_give(g_sem_rx0);
}
*/

uint8_t wfc_mq_opcmd_process(T_WFC_QBUF *qbuf, uint8_t *isDTW)
{
    uint8_t   *ptr;
    uint32_t   ptrSize;
    uint16_t  *opcmd;

    opcmd   = (uint16_t *)g_host_tx_buf[qbuf->qidx];
    ptrSize = qbuf->size - 2;

    //printf("[JCB]%s,%d, opcmd,=0x%x,  qbuf->qidx=%d \n",__FUNCTION__,__LINE__,opcmd, qbuf->qidx);
    if ((*opcmd) == WFC_CMD_DTW_DATA)
    {
        *isDTW = 1;
        return true;
    }
    *isDTW = 0;

    if (ptrSize <= 0)  //OpCmd Only
    {
        wfc_send_queue(WFC_OP_HOST_CMD, *opcmd, NULL, 0, 1);    //Queue Modify Later
    }
    else
    {
        ptr = (uint8_t*)wifi_os_malloc(ptrSize+1);
        if (!ptr)
            return false;

        memcpy(ptr, &(g_host_tx_buf[qbuf->qidx][2]), ptrSize);
        ptr[ptrSize] = 0;
        //printf("[JCB]%s,%d, malloc  ptr=0x%x\n",__FUNCTION__,__LINE__, ptr);

        //Send Host Cmd and Parameter to WFC queue
        if(wfc_send_queue(WFC_OP_HOST_CMD, *opcmd, (uint8_t *)ptr, qbuf->size - 2, 1) == false)
        {
           /*if send queue fail,  the ptr need to be free here*/
           if (ptr != NULL)
           {
                printf("%s,%d,  wfc send queue fail !\n",__FUNCTION__,__LINE__);
                wifi_os_free(ptr);
                ptr = NULL;
           }
        }
    }

    return true;
}

void sdio_isr_callback(hal_sdio_slave_callback_event_t event,  void *parameter, void *user_data)
{
    test_event = event;

    if (event == HAL_SDIO_SLAVE_EVENT_TX1_DONE) { //Host_TX
        //uint16_t *opcmd;
        uint32_t  rcvSize;
        hal_sdio_slave_callback_tx_length_parameter_t *data;

        data = (hal_sdio_slave_callback_tx_length_parameter_t *)parameter;
        g_htx_free_qbuf.size = rcvSize = data->hal_sdio_slave_tx_length;

        //opcmd = (uint16_t *)g_host_tx_buf[g_htx_free_qbuf.qidx];
        //printf("[HTX]: opcmd(%u), rcvSize(%u),Q(%d,%d)\r\n",*opcmd,rcvSize,g_htx_free_qbuf.ctrl, g_htx_free_qbuf.qidx);

        wfc_mq_send_queue(&g_htx_free_qbuf, 1);
        wfc_mq_clear(&g_htx_free_qbuf);
    }

    if (event == HAL_SDIO_SLAVE_EVENT_ERROR) { // Host RX
        //sdio_rx0_done_add(1);
        printf("sdio_send_error\r\n");
    }
}


hal_sdio_status_t wfc_if_sdio_init(void)
{
    hal_pinmux_set_function(11, 5);
    hal_pinmux_set_function(12, 5);
    hal_pinmux_set_function(13, 5);
    hal_pinmux_set_function(14, 5);
    hal_pinmux_set_function(15, 5);
    hal_pinmux_set_function(16, 5);

#ifndef WFC_HRX_POLLING
    hal_gpio_init(WFC_HRX_EINT);
    hal_pinmux_set_function(WFC_HRX_EINT, 0);
    hal_gpio_set_direction(WFC_HRX_EINT, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_pull_down(WFC_HRX_EINT);
#endif

    /*step1: init SDIO slave.*/
    if (0 > hal_sdio_slave_init(HAL_SDIO_SLAVE_PORT_0)) {
        printf("sdio_slave init error \r\n");
        return HAL_SDIO_STATUS_ERROR;
    }

    hal_sdio_slave_register_callback(HAL_SDIO_SLAVE_PORT_0, sdio_isr_callback, NULL);

    memset(g_cmd_rsp_buf, 0, SDIO_CMD_RSP_SIZE);
    memset(g_host_rx_buf, 0, MEM_QUEUE_SIZE * SDIO_HRX_DATA_SIZE);
    memset(g_host_tx_buf, 0, MEM_QUEUE_SIZE * SDIO_HTX_DATA_SIZE);
    memset(g_slave_rx_buf, 0, MEM_QUEUE_SIZE *SDIO_SLAVE_EVT_SIZE);
    memset(g_memq_list,   0, MEMQ_CTRL_MAX_NUM*MEM_QUEUE_SIZE);

    g_hrx_qbuf = plist_hrx = NULL;

    g_sem_qbuf  = wifi_os_semphr_create_binary();
    wifi_os_semphr_give(g_sem_qbuf);

    //g_sem_rx0   = wifi_os_semphr_create_binary();
    //wifi_os_semphr_give(g_sem_rx0);

    // Reset Global Memq Information
    wfc_sdio_mq_reset(MEMQ_CTRL_HOST_TX);

    //Prepare HTX Buffer
    wfc_sdio_set_htx_buf();

    return HAL_SDIO_STATUS_OK;
}



int wfc_sdio_set_htx_buf(void)
{
    g_htx_free_qbuf.ctrl = MEMQ_CTRL_HOST_TX;

    // Get free index from TX receive queue buffer
    if (g_htx_free_qbuf.qidx == WFC_MEMQ_INVALID) {
        if (wfc_mq_get_buf(g_htx_free_qbuf.ctrl, &(g_htx_free_qbuf.qidx)) >= 0) {
            hal_sdio_slave_receive_dma(HAL_SDIO_SLAVE_PORT_0, HAL_SDIO_SLAVE_TX_QUEUE_1, (uint32_t *)&g_host_tx_buf[g_htx_free_qbuf.qidx][0], SDIO_HTX_DATA_SIZE);
        }
    }

    return 0;
}


void wfc_sido_htx_cp(T_WFC_QBUF *qbuf, uint8_t *pbuf, uint32_t offset)
{
    if (((uint32_t)pbuf) % 4) {
        memcpy(pbuf, &(g_host_tx_buf[qbuf->qidx][offset]), qbuf->size);
    } else {
        wfc_dma_memcpy(pbuf, &(g_host_tx_buf[qbuf->qidx][offset]), qbuf->size);
    }
}


void wfc_sdio_hevt_cp(T_WFC_QBUF *qbuf, uint8_t *hrxptr, uint32_t hrxSize)
{
    uint8_t     *pdst;
    uint32_t    *pmagic;

    qbuf->ctrl = MEMQ_CTRL_SLAVE_EVT;
    qbuf->qidx = 0;// it is not used for MEMQ_CTRL_SLAVE_EVT, just give a initial setting.
    qbuf->size = hrxSize+WFC_MAGIC_LEN*2;

    qbuf->ptr = (uint8_t*)wifi_os_malloc(qbuf->size+1);
    qbuf->ptr[qbuf->size] = 0;

    // Get free index from Host RX receive queue buffer
//    while (wfc_mq_get_buf(qbuf->ctrl, &(qbuf->qidx)) < 0) {
//        hal_gpt_delay_ms(1);
//    }

    //Add Magic Number at the head of Event Notify
    pdst = qbuf->ptr;
    pmagic = (uint32_t *)pdst;
    *pmagic = WFC_MAGIC_NUM;
    pdst += WFC_MAGIC_LEN;

    //Copy Notify Event and Parameter
    memcpy(pdst, (uint8_t *)hrxptr, hrxSize);
    pdst += hrxSize;

    //Append Magic Number at the end of Event Notify
    pmagic = (uint32_t *)pdst;
    *pmagic = WFC_MAGIC_NUM;

    //wfc_dump_buf(g_host_rx_buf[qbuf->qidx], qbuf->size);
}


void wfc_sdio_hrx_cp(T_WFC_QBUF *qbuf, uint8_t *hrxptr, uint32_t hrxSize)
{
    struct pbuf *q;
    uint8_t     *pdst;

    qbuf->ctrl = MEMQ_CTRL_HOST_RX;
    qbuf->size = hrxSize;

    // Get free index from Host RX receive queue buffer
    while (wfc_mq_get_buf(qbuf->ctrl, &(qbuf->qidx)) < 0) {
        hal_gpt_delay_ms(1);
    }

    pdst = g_host_rx_buf[qbuf->qidx];
    for (q = (struct pbuf *)hrxptr; q != NULL; q = q->next) {
        if ((((uint32_t)q->payload) % 4) || (((uint32_t)pdst) % 4)) {
            memcpy(pdst, (uint8_t *)q->payload, q->len);
        } else {
            wfc_dma_memcpy(pdst, (uint8_t *)q->payload, q->len);
        }
        pdst += q->len;
    }
}


void wfc_sdio_send_response(T_WFC_OPCODE *pwfc, uint16_t rspcode, uint8_t *txptr, uint32_t txsize, uint32_t hsize)
{
    uint32_t  slen = sizeof(rspcode);
	uint32_t  cnt = 0;

    // Set DTH data response code into SPI response
    rspcode = (hsize) ? (rspcode | WFC_RSP_DTH_DATA) : rspcode;

    // Assign response code to tx transfer buffer
    pwfc->rspcode = (uint16_t *)&g_cmd_rsp_buf[0];
    *(pwfc->rspcode) = rspcode;

    // Copy DTH data size info into tx transfer buffer
    pwfc->txptr = &g_cmd_rsp_buf[sizeof(rspcode)];
    if (hsize) {
        memcpy(pwfc->txptr, &hsize, sizeof(hsize));
        pwfc->txptr +=  sizeof(hsize);
        slen += sizeof(hsize);
    }

    // Copy response content into tx transfer buffer
    if (txptr && txsize) {
        memcpy(pwfc->txptr, txptr, txsize);
        slen += txsize;
    }

    // Dump data received (Debug)
    // wfc_dump_buf(&g_cmd_rsp_buf[0], slen);

    // Wait SPI master to send config read command.
    if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_send_dma(HAL_SDIO_SLAVE_PORT_0, HAL_SDIO_SLAVE_RX_QUEUE_0, (uint32_t *)&g_cmd_rsp_buf[0], slen)) {
        printf("slt sdio slave send error.\r\n");
        return;
    }

    //Wait Send Response Done (Option)
    while ((test_event != HAL_SDIO_SLAVE_EVENT_RX0_DONE) && (cnt++ < 20));
    test_event = HAL_SDIO_SLAVE_EVENT_NONE;

    //while (!g_rx0_done_event);
    //sdio_rx0_done_free(0);
}


void wfc_sdio_send_data(uint8_t *txptr, uint32_t txsize)
{
    uint32_t   cnt = 0;

    if (g_hrx_qbuf == NULL) {
        printf("[HRX]: No HRX Data in Queue\r\n");
        goto Exit;
    }

    // Send Host RX Data
    if ( g_hrx_qbuf->ctrl == MEMQ_CTRL_SLAVE_EVT )
    {
        memcpy(&g_slave_rx_buf[0][0], g_hrx_qbuf->ptr, g_hrx_qbuf->size);
        wifi_os_free(g_hrx_qbuf->ptr);
        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_send_dma(HAL_SDIO_SLAVE_PORT_0, HAL_SDIO_SLAVE_RX_QUEUE_0, (uint32_t *) & (g_slave_rx_buf[0][0]), g_hrx_qbuf->size)) {
            printf("Prepare Host RX Buffer Error\r\n");
            goto Exit;
        }
    } else {
        struct pbuf *q;
        uint8_t     *pdst;

        pdst = g_host_rx_buf[0];
        for (q = (struct pbuf *)g_hrx_qbuf->ptr; q != NULL; q = q->next) {
            if ((((uint32_t)q->payload) % 4) || (((uint32_t)pdst) % 4)) {
                memcpy(pdst, (uint8_t *)q->payload, q->len);
            } else {
                wfc_dma_memcpy(pdst, (uint8_t *)q->payload, q->len);
            }
            pdst += q->len;
        }
        pbuf_free((struct pbuf *)(g_hrx_qbuf->ptr));

        if (HAL_SDIO_SLAVE_STATUS_OK != hal_sdio_slave_send_dma(HAL_SDIO_SLAVE_PORT_0, HAL_SDIO_SLAVE_RX_QUEUE_0, (uint32_t *) & (g_host_rx_buf[0][0]), g_hrx_qbuf->size)) {
            printf("Prepare Host RX Buffer Error\r\n");
            goto Exit;
        }
    }

    // Wait HRX Done
    while ((test_event != HAL_SDIO_SLAVE_EVENT_RX0_DONE) && (cnt++ < 20));
    test_event = HAL_SDIO_SLAVE_EVENT_NONE;
    goto Exit;

    //while (!g_rx0_done_event);
    //sdio_rx0_done_free(0);

Exit:
    if(g_hrx_qbuf != NULL){
        //Free HRX Buffer
        //printf("[JCB]%s,%d, free g_hrx_qbuf = 0x%x \r\n",__FUNCTION__,__LINE__);

        wfc_mq_free(g_hrx_qbuf);
        wifi_os_free(g_hrx_qbuf);
        g_hrx_qbuf = NULL;
    }
}


