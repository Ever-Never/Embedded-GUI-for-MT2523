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
#include "semphr.h"

/* hal includes */
#include "memory_attribute.h"
#include "hal.h"
#include "sdio_hal.h"
#include "sdio_reg.h"

#include "lwip/pbuf.h"
#include "wfc_stub.h"


#define WIFI_SDIO_PORT            HAL_SDIO_PORT_0
#define WFCM_SDIO_BUS_WIDTH       HAL_SDIO_BUS_WIDTH_1
#define WFCM_SDIO_CLOCK           26000   //26M
#define WIFI_BLK_SIZE             (256)
#define MEM_QUEUE_SIZE                8   //8
#define SDIO_HRX_DATA_SIZE        1600
#define SDIO_HTX_DATA_SIZE        1600

#ifndef ALIGN_TO_BLOCK_SIZE
#define ALIGN_TO_BLOCK_SIZE(_value)     (((_value) + (WIFI_BLK_SIZE - 1)) & ~(WIFI_BLK_SIZE - 1))
#endif

#ifndef ALIGN_32
#define ALIGN_32(_value)             (((_value) + 31) & ~31u)
#endif /* ALIGN_32*/

#ifndef ALIGN_4
#define ALIGN_4(_value)             (((_value) + 3) & ~3u)
#endif /* ALIGN_4 */

typedef struct {
    union {
        struct {
            uint32_t  length: 16;
            uint32_t  reserved: 13;
            uint32_t  tx_type: 3;
        } bits;
        uint32_t         asUINT32;
    } u;
} brom_sdio_tx_sdu_header_t;  // 4byte tx header


SemaphoreHandle_t g_sem_qbuf = NULL;
uint8_t g_memq_list[MEMQ_CTRL_MAX_NUM][MEM_QUEUE_SIZE];

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_host_rx_buf[MEM_QUEUE_SIZE][SDIO_HRX_DATA_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  uint8_t g_host_tx_buf[MEM_QUEUE_SIZE][SDIO_HTX_DATA_SIZE];


int32_t wfcm_mq_get_buf(uint32_t ctrl_idx, uint32_t *qidx)
{
	uint32_t idx;
    xSemaphoreTake(g_sem_qbuf, portMAX_DELAY);

    for (idx=0; idx<(MEM_QUEUE_SIZE-1); idx++)
    {
    	if (g_memq_list[ctrl_idx][idx] == 0)
    	{
    		g_memq_list[ctrl_idx][idx] = 1; //allocated
    		*qidx = idx;

            xSemaphoreGive(g_sem_qbuf);
    		return idx;
    	}
    }	
    *qidx = WFC_MEMQ_INVALID;

    xSemaphoreGive(g_sem_qbuf);
    return -1;
}

void wfcm_mq_clear(T_WFC_QBUF *qbuf)
{	          
    qbuf->qidx = WFC_MEMQ_INVALID;
    qbuf->size = 0;
}

void wfcm_mq_free(T_WFC_QBUF *qbuf)
{
    xSemaphoreTake(g_sem_qbuf, portMAX_DELAY);
    g_memq_list[qbuf->ctrl][qbuf->qidx] = 0; //free
    qbuf->qidx = WFC_MEMQ_INVALID;
    qbuf->size = 0;
    xSemaphoreGive(g_sem_qbuf);
}


void wfcm_dump_buf(uint8_t *pbuf, uint32_t bufSize)
{
    // Dump data received (Debug)
    printf("\r\n[Dump]: Data Length == %lu\r\n",bufSize);         
    for (int i = 0; i < bufSize; i++)
        printf("0x%02x ", pbuf[i]);    
    printf("\r\n\r\n\r\n");             
}   

void wfcm_mem_align_clr(uint8_t *pbuf, uint32_t bufSize)
{
	uint32_t sidx = bufSize;
	uint32_t eidx;

	//sidx = g_hrx_free_qbuf.size;
	eidx = ALIGN_TO_BLOCK_SIZE(bufSize);
	if (eidx > sidx)
	{	
	   memset(pbuf+sidx, 0, eidx-sidx);
    }
}

    
void wfcm_dma_memcpy(void *dst, void *src, unsigned int len)    
{                                                                      
    hal_gdma_status_t ret;                                             
    hal_gdma_running_status_t running_status;                          
    hal_gdma_channel_t channel = HAL_GDMA_CHANNEL_0;                   

    hal_gdma_init(HAL_GDMA_CHANNEL_0);

    ret = hal_gdma_start_polling(channel, (uint32_t)dst, (uint32_t)src, len);
    if(HAL_GDMA_STATUS_OK != ret)                                  
    {                                                              
        //printf("DMA unavailable...\n");                            
        memcpy(dst, src, len);                                     
    }                                                                                                                                     
    hal_gdma_get_running_status(channel,&running_status);          
    hal_gdma_stop(HAL_GDMA_CHANNEL_0); // Stop the GDMA.
    hal_gdma_deinit(HAL_GDMA_CHANNEL_0);   //De-initialize the GDMA.	  	
}                   


bool wfcm_sdio_write(uint32_t  port_offset, uint8_t *write_buffer, uint16_t length, uint8_t usingDMA)
{
    hal_sdio_status_t status;
    uint8_t block_number = 0;
    uint32_t func1_block_size;
    hal_sdio_command53_config_t cmd_cfg;
    uint32_t total_length;
    uint32_t length_backup;

    /*addr 4-byte align check*/
    //assert((((uint32_t)write_buffer) & 0x03) == 0);

    if (((uint32_t)write_buffer) & 0x03) {
        printf("Error!!!Invalid Buffer Addr. \r\n");
        return false;
    }
    memset(&cmd_cfg, 0, sizeof(hal_sdio_command53_config_t));

    cmd_cfg.direction = HAL_SDIO_DIRECTION_WRITE;
    cmd_cfg.function = 1;
    cmd_cfg.operation = HAL_SDIO_FIXED_ADDRESS;
    cmd_cfg.address = port_offset;
    cmd_cfg.buffer = (uint32_t)write_buffer;

    length_backup = length;

    func1_block_size = WIFI_BLK_SIZE;
    while (length >= func1_block_size) {
        length -= func1_block_size;
        block_number++;
    }
    if (length > 0 && block_number > 0) {
        block_number++;
    }

    if (block_number > 0) { // block mode
        cmd_cfg.block = true;
        cmd_cfg.count = block_number;            
        total_length = (usingDMA)?ALIGN_32(block_number * func1_block_size):block_number * func1_block_size;  //ALIGN_32(block_number * func1_block_size);
    } else {
        cmd_cfg.block = false;
        cmd_cfg.count = (usingDMA)?ALIGN_32(length):length;// ALIGN_32(length);
        total_length = (usingDMA)?ALIGN_32(length):length; //ALIGN_32(length);
    }

    if ((total_length - length_backup) >= sizeof(uint32_t)) {
        /*set unisgned int to Zero for 4-byte unalign address, will shift to align address*/
        uint32_t aliglen = (usingDMA)?ALIGN_4(length_backup):length_backup; //ALIGN_4(length_backup);
        write_buffer[aliglen] = 0;
        write_buffer[aliglen + 1] = 0;
        write_buffer[aliglen + 2] = 0;
        write_buffer[aliglen + 3] = 0;
    }

    if (usingDMA)
        status = hal_sdio_execute_command53_dma_blocking(WIFI_SDIO_PORT, &cmd_cfg);
    else
        status = hal_sdio_execute_command53(WIFI_SDIO_PORT, &cmd_cfg);

    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio write error. \r\n");
        return false;
    } else {
        return true;
    }
}


bool wfcm_sdio_read(uint32_t port_offset,  uint32_t *read_buffer, uint16_t length, uint8_t usingDMA)
{
    hal_sdio_status_t status;
    uint8_t block_number = 0;
    uint32_t  func1_block_size;
    hal_sdio_command53_config_t cmd_cfg;

    /*addr 4-byte align check*/
    assert((((uint32_t)read_buffer) & 0x03) == 0);

    if (((uint32_t)read_buffer) & 0x03) {
        printf("Error!!!kalDevPortRead:Invalid Buffer Addr. \r\n");
        return false;
    }

    cmd_cfg.direction = HAL_SDIO_DIRECTION_READ;
    cmd_cfg.function = 1;
    cmd_cfg.operation = HAL_SDIO_FIXED_ADDRESS;
    cmd_cfg.address = port_offset;
    cmd_cfg.buffer = (uint32_t)read_buffer;

    func1_block_size = WIFI_BLK_SIZE;
    while (length >= func1_block_size) {
        length -= func1_block_size;
        block_number++;
    }
    if (length > 0 && block_number > 0) {
        block_number++;
    }

    if (block_number > 0) { /* block mode*/
        cmd_cfg.block = true;
        cmd_cfg.count = block_number;
    } else {
        cmd_cfg.block = false;
        cmd_cfg.count = (usingDMA)?ALIGN_32(length):length; //length;//ALIGN_32(length);
    }

    if (usingDMA)
        status = hal_sdio_execute_command53_dma_blocking(WIFI_SDIO_PORT, &cmd_cfg);
    else
        status = hal_sdio_execute_command53(WIFI_SDIO_PORT, &cmd_cfg);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio read error. \r\n");
        return false;
    } else {
        return true;
    }
}


bool wfcm_sdio_check_init(void)
{
    uint32_t      wcir_reg = 0;
  
    do {
    } while (wfcm_sdio_read(SDIO_IP_WCIR, &wcir_reg, 4, false)==true); /*waiting Boot ROM release*/

    return true;
}

bool wfcm_sdio_hif_get_driver_own(void)
{
    bool ret ;
    uint32_t value ;
    uint32_t cnt = 50;

    printf("[wfcm_sdio_hif_get_driver_own]<==========>\r\n") ;

    //Set driver own
    value = W_FW_OWN_REQ_CLR ;
    if ((ret = wfcm_sdio_write(SDIO_IP_WHLPCR, (uint8_t *)&value, 4, false)) == false) {
        return false;
    }

    while (cnt--) {
        if ((ret = wfcm_sdio_read(SDIO_IP_WHLPCR, (uint32_t *)&value, 4, false)) == false) {
            return false ;
        }

        if (value & W_DRV_OWN_STATUS) {
            return true;
        }
        hal_gpt_delay_ms(10);
    }

    return false;
}


int wfcm_if_sdio_init(void)
{
    hal_sdio_status_t status = 0;
    hal_sdio_config_t sdio_config = {WFCM_SDIO_BUS_WIDTH,   WFCM_SDIO_CLOCK};
    uint32_t blk_size;
    uint32_t count = 0;

#if (PRODUCT_VERSION == 2523)
     /* Step1: Call hal_pinmux_set_function() to set GPIO pinmux, if EPT tool was not used to configure the related pinmux.*/
    hal_pinmux_set_function(HAL_GPIO_30, HAL_GPIO_30_MC0_CK);// MC0_CK
    hal_pinmux_set_function(HAL_GPIO_31, HAL_GPIO_31_MC0_CM0);// MC0_CM0
    hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_MC0_DA0);// MC0_DA0
    hal_pinmux_set_function(HAL_GPIO_33, HAL_GPIO_33_MC0_DA1);// MC0_DA1
    hal_pinmux_set_function(HAL_GPIO_34, HAL_GPIO_34_MC0_DA2);// MC0_DA2
    hal_pinmux_set_function(HAL_GPIO_35, HAL_GPIO_35_MC0_DA3);// MC0_DA3
    
    #ifndef WFC_HRX_POLLING
    hal_gpio_init(HAL_GPIO_6);
    hal_pinmux_set_function(HAL_GPIO_6, 1);
    hal_gpio_set_direction(HAL_GPIO_6, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_disable_pull(HAL_GPIO_6);                   
    #endif
#else
    hal_pinmux_set_function(11, 4);
    hal_pinmux_set_function(12, 4);
    hal_pinmux_set_function(13, 4);
    hal_pinmux_set_function(14, 4);
    hal_pinmux_set_function(15, 4);
    hal_pinmux_set_function(16, 4);
    
    hal_gpio_set_pupd_register(11, 0, 0, 1);
    hal_gpio_set_pupd_register(12, 0, 0, 1);
    hal_gpio_set_pupd_register(13, 0, 0, 1);
    hal_gpio_set_pupd_register(14, 0, 0, 1);
    hal_gpio_set_pupd_register(15, 0, 0, 1);
    hal_gpio_set_pupd_register(16, 0, 0, 1);

    #ifndef WFC_HRX_POLLING
    hal_gpio_init(HAL_GPIO_17);
    hal_pinmux_set_function(HAL_GPIO_17, 8);
    hal_gpio_set_direction(HAL_GPIO_17, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_disable_pull(HAL_GPIO_17);               
    #endif
#endif

    if(g_sem_qbuf == NULL) {
        g_sem_qbuf  = xSemaphoreCreateBinary();
    }
    xSemaphoreGive(g_sem_qbuf);

    /* Excute SDIO SW hook flow */
    do {
        status = hal_sdio_init(WIFI_SDIO_PORT, &sdio_config);
        printf("hal_sdio_init status = %d \r\n", status);
        if (HAL_SDIO_STATUS_OK != status) {
            printf("sdio init error. \r\n");
            //return HAL_SDIO_STATUS_ERROR;
        }
        hal_gpt_delay_ms(500);
        count++;
    } while ((HAL_SDIO_STATUS_OK != status) && (count < 10));

    if (count >= 10) {
        printf("sdio init timeout!\r\n");
        return HAL_SDIO_STATUS_ERROR;
    }

    status = hal_sdio_set_block_size(WIFI_SDIO_PORT, HAL_SDIO_FUNCTION_1, WIFI_BLK_SIZE);
    printf("hal_sdio_set_block_size status = %d \r\n", status);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio set block size error. \r\n");
        return status;
    }

    status = hal_sdio_get_block_size(WIFI_SDIO_PORT, HAL_SDIO_FUNCTION_1, &blk_size);
    printf("hal_sdio_get_block_size status = %d \r\n", status);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio get block size error. \r\n");
        return status;
    } else {
        printf("sdio get block size, block size is %lu. \r\n", blk_size);
    }

    if (false == wfcm_sdio_hif_get_driver_own()) {
        printf("1 get driver own fail. \r\n");
        return HAL_SDIO_STATUS_ERROR;
    } else {
        printf("1 get driver own success. \r\n");
    }
    
    memset(g_host_rx_buf, 0, MEM_QUEUE_SIZE*SDIO_HRX_DATA_SIZE);
    memset(g_host_tx_buf, 0, MEM_QUEUE_SIZE*SDIO_HTX_DATA_SIZE);
    
    return HAL_SDIO_STATUS_OK;
}


void wfcm_if_sdio_reinit(void)
{
    hal_pinmux_set_function(11, 4);
    hal_pinmux_set_function(12, 4);
    hal_pinmux_set_function(13, 4);
    hal_pinmux_set_function(14, 4);
    hal_pinmux_set_function(15, 4);
    hal_pinmux_set_function(16, 4);

    hal_gpio_set_pupd_register(11, 0, 0, 1);
    hal_gpio_set_pupd_register(12, 0, 0, 1);
    hal_gpio_set_pupd_register(13, 0, 0, 1);
    hal_gpio_set_pupd_register(14, 0, 0, 1);
    hal_gpio_set_pupd_register(15, 0, 0, 1);
    hal_gpio_set_pupd_register(16, 0, 0, 1);	
}


void wfcm_if_sdio_deinit(void)
{
    hal_sdio_deinit(WIFI_SDIO_PORT);
}


uint8_t *wfcm_sdio_get_htxbuf(T_WFC_QBUF *qbuf)
{
    return (&(g_host_tx_buf[qbuf->qidx][0])+sizeof(brom_sdio_tx_sdu_header_t));   
}


void wfcm_sdio_memq_get_qbuf(T_WFC_QBUF *qbuf)
{
    // Get free index from Host TX receive queue buffer
    while ( wfcm_mq_get_buf(qbuf->ctrl, &(qbuf->qidx)) < 0)
    {
        hal_gpt_delay_us(10);
    }
}

void wfcm_sdio_cmd_cp(uint16_t opcode, T_WFC_QBUF *qbuf, uint8_t *param, uint32_t paramSize, uint8_t forISR)
{
    uint16_t *op;
    uint8_t  *pdst;

    qbuf->size = paramSize + sizeof(opcode); //opcode(2)

    if (forISR)
    {
        qbuf->qidx = MEM_QUEUE_SIZE-1;
    }
    // Clear HTX Buffer (After Buffer Size)
    wfcm_mem_align_clr(&(g_host_tx_buf[qbuf->qidx][0]), qbuf->size + sizeof(brom_sdio_tx_sdu_header_t));

    // Assign OpCode    
    op = (uint16_t *)(&(g_host_tx_buf[qbuf->qidx][0])+ sizeof(brom_sdio_tx_sdu_header_t));
    *op  = opcode;	

    // Copy Cmd Parameter
    if (param && paramSize)
    {
        pdst = (uint8_t *)(&(g_host_tx_buf[qbuf->qidx][0])+ sizeof(brom_sdio_tx_sdu_header_t));        
        pdst = pdst + sizeof(opcode);        //opcode(2)
        memcpy(pdst, param, paramSize); 
    }
}


void wfcm_sdio_htx_cp(uint16_t opcode, T_WFC_QBUF *qbuf, uint8_t *htxptr, uint32_t htxSize)
{
    uint16_t *op;
    struct pbuf *q;
    uint8_t  *pdst;

    qbuf->size = htxSize+4; //opcode(2)+2, 4 bytes alignment

    
    // Clear HTX Buffer (After Buffer Size 4 Bytes)
    wfcm_mem_align_clr(&(g_host_tx_buf[qbuf->qidx][0]), qbuf->size + sizeof(brom_sdio_tx_sdu_header_t));

    // Assign OpCode    
    op = (uint16_t *)(&(g_host_tx_buf[qbuf->qidx][0])+ sizeof(brom_sdio_tx_sdu_header_t));
    *op  = opcode;	

    // Copy HTX Data
    pdst = ((uint8_t*)op)+4; //opcode(2)+2, 4 bytes alignment 
        
    for(q = (struct pbuf *)htxptr; q != NULL; q = q->next) {
      if ( (((uint32_t)q->payload)%4) || (((uint32_t)pdst)%4) )
      {
      	  memcpy(pdst, (uint8_t*)q->payload, q->len);          
      }		
      else
      {	
          wfcm_dma_memcpy(pdst, (uint8_t*)q->payload, q->len);   
      }
      pdst += q->len;
    } 
}


void wfcm_sido_hrx_cp(T_WFC_QBUF *qbuf, uint8_t *pbuf)
{
    wfcm_dma_memcpy(pbuf, &(g_host_rx_buf[qbuf->qidx][0]), qbuf->size);	
}


bool wfcm_sdio_send_data(T_WFC_QBUF *qbuf, uint8_t qbuf_release)
{

    uint32_t      whisr_reg = 0;
    bool          ret       = true;

    brom_sdio_tx_sdu_header_t *tx_header = (brom_sdio_tx_sdu_header_t *)&(g_host_tx_buf[qbuf->qidx][0]);
	
    tx_header->u.asUINT32 = 0;

    tx_header->u.bits.length = qbuf->size + sizeof(brom_sdio_tx_sdu_header_t);  

    //Waiting TX_DONE_INT
    do {
        wfcm_sdio_read(SDIO_IP_WHISR, &whisr_reg, 4, false);
    } while (0 == (whisr_reg & 0x01)); /*waiting TX_DONE_INT*/
    
    //Clear TX_DONE
    wfcm_sdio_read(SDIO_IP_WTSR0, &whisr_reg, 4, false);
    if (wfcm_sdio_write(SDIO_IP_WTDR1, &(g_host_tx_buf[qbuf->qidx][0]), (qbuf->size + sizeof(brom_sdio_tx_sdu_header_t)), false) == false) {
        printf("[ERR] sdio_send_pkt => wfcm_sdio_write 0x%08x len=%lu error\r\n", SDIO_IP_WTDR1, (qbuf->size + sizeof(brom_sdio_tx_sdu_header_t)));
        ret = false;
    }

    //Free HTX Buffer
    if (qbuf_release){
        wfcm_mq_free(qbuf);
    }
    return ret;
}


bool wfcm_sdio_receive_cmdrsp(uint8_t *rx_buf, uint32_t *bufSize)
{
	uint32_t whisr_reg = 0;
	uint32_t wrplr_reg = 0;
			
	do {
	    wfcm_sdio_read(SDIO_IP_WHISR, &whisr_reg, 4, false);
	} while (0 == (whisr_reg & 0x02)); /*wait RX0_DONE_INT */
	
    wfcm_sdio_read(SDIO_IP_WRPLR, &wrplr_reg, 4, false);
    *bufSize = (wrplr_reg&0xffff);

    if (wfcm_sdio_read(SDIO_IP_WRDR0, (uint32_t *)rx_buf, (wrplr_reg&0xffff), false) == false) {
        printf("[ERR],sdio_receive_pkt, wfcm_sdio_read SDIO_IP_WRDR0 fail\r\n");
    	return false;
    }

    return true;
}


bool wfcm_sdio_receive_data(uint8_t *pbuf, uint32_t bufSize)
{
	uint32_t whisr_reg = 0;
	uint32_t wrplr_reg = 0;	
	
	do {
	    wfcm_sdio_read(SDIO_IP_WHISR, &whisr_reg, 4, false);
	} while (0 == (whisr_reg & 0x02)); /*wait RX0_DONE_INT*/

    wfcm_sdio_read(SDIO_IP_WRPLR, &wrplr_reg, 4, false);
    if (wfcm_sdio_read(SDIO_IP_WRDR0, (uint32_t*)pbuf, bufSize, false) == false) {
        printf("[ERR],sdio_receive_pkt, wfcm_sdio_read SDIO_IP_WRDR0 fail\r\n");
    	return false;
    }

    return true;
}

