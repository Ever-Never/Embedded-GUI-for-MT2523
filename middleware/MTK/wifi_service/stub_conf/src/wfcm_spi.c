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

#include "hal.h"
#include "memory_attribute.h"
#include "hal_spi_master.h"
#include "hal_gpio_internal.h"
#include "hal_gpio.h"

#include "lwip/pbuf.h"
//#include "hal_gdma.h"

#if (PRODUCT_VERSION == 2523)
#define SPIM_TEST_FREQUENCY     6000000
//#define SPIM_TEST_FREQUENCY     4800000
#define SPIM_TEST_PORT          HAL_SPI_MASTER_3
#define SPI_TEST_MASTER_SLAVE HAL_SPI_MASTER_SLAVE_0

#define SPIM_PIN_NUMBER_CS    HAL_GPIO_25
#define SPIM_PIN_FUNC_CS      HAL_GPIO_25_MA_SPI3_A_CS
#define SPIM_PIN_NUMBER_CLK   HAL_GPIO_26
#define SPIM_PIN_FUNC_CLK     HAL_GPIO_26_MA_SPI3_A_SCK
#define SPIM_PIN_NUMBER_SIO_0  HAL_GPIO_27
#define SPIM_PIN_FUNC_SIO_0    HAL_GPIO_27_MA_SPI3_A_MOSI
#define SPIM_PIN_NUMBER_SIO_1  HAL_GPIO_28
#define SPIM_PIN_FUNC_SIO_1    HAL_GPIO_28_MA_SPI3_A_MISO
//#define SPIM_PIN_NUMBER_MT5932_RESET  HAL_GPIO_48  /* The GPIO pin to control reset pin of MT5932 */
#define SPI_STATUS_RDWR_FINISH_MASK   SPISLV_STATUS_RDWR_FINISH_MASK
#define SPI_STATUS_TXRX_FIFO_RDY_MASK SPISLV_STATUS_TXRX_FIFO_RDY_MASK
#define SPI_STATUS_SLV_ON_MASK        SPISLV_STATUS_SLV_ON_MASK

#else
#define SPIM_TEST_FREQUENCY     10000000
//#define SPIM_TEST_FREQUENCY     4800000
#define SPIM_TEST_PORT          HAL_SPI_MASTER_0


#define SPIM_PIN_NUMBER_CS      HAL_GPIO_17
#define SPIM_PIN_FUNC_CS        2
#define SPIM_PIN_NUMBER_CLK     HAL_GPIO_16
#define SPIM_PIN_FUNC_CLK       2
#define SPIM_PIN_NUMBER_SIO_0   HAL_GPIO_15
#define SPIM_PIN_FUNC_SIO_0     2
#define SPIM_PIN_NUMBER_SIO_1   HAL_GPIO_14
#define SPIM_PIN_FUNC_SIO_1     2
#define SPIM_PIN_NUMBER_SIO_2   HAL_GPIO_13
#define SPIM_PIN_FUNC_SIO_2     2
#define SPIM_PIN_NUMBER_SIO_3   HAL_GPIO_12
#define SPIM_PIN_FUNC_SIO_3     2

#define SPI_STATUS_RDWR_FINISH_MASK   SPIS_STA_RDWR_FINISH_MASK
#define SPI_STATUS_TXRX_FIFO_RDY_MASK SPIS_STA_TXRX_FIFO_RDY_MASK
#define SPI_STATUS_SLV_ON_MASK        SPIS_STA_SLV_ON_MASK

#endif

#define SIPS_CMDSET_XBOOT       0
#define SIPS_CMDSET_FW          1

// For xBoot Command Set
#define SPIS_CFG_RD_CMD         0x02
#define SPIS_RD_CMD             0x81
#define SPIS_CFG_WR_CMD         0x04
#define SPIS_WR_CMD             0x06
#define SPIS_RS_CMD             0x0a
#define SPIS_PWON_CMD           0x0e
#define SPIS_PWOFF_CMD          0x0c
#define SPIS_CT_CMD             0x10

// For WFC Command Set
#define SPIS_FW_CFG_RD_CMD      0x0a
#define SPIS_FW_RD_CMD          0x81
#define SPIS_FW_CFG_WR_CMD      0x0c
#define SPIS_FW_WR_CMD          0x0e
#define SPIS_FW_RS_CMD          0x06
#define SPIS_FW_PWON_CMD        0x04
#define SPIS_FW_PWOFF_CMD       0x02
#define SPIS_FW_CT_CMD          0x10

#define SPIS_CFG_4BYTE_ADDR     (0x01 << 2)
#define SPIS_CFG_QUAD_MODE      (0x02 << 0)

#define SPIS_ADDRESS_ID         0x55aa0000
#define MT5932_LOAD_SIZE          0x57FF0 /* size for the load of MT5932 */
//#define MT5932_LOAD_SIZE          1024 /* size for the load of MT5932 */

static volatile bool g_spi_transaction_finish;
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_transfer_tx_buffer[MT5932_LOAD_SIZE + 16 + 1];
//ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_transfer_rx_buffer[64];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_transfer_rx_buffer[2048];
//static uint8_t g_transfer_tx_buffer[MT5932_LOAD_SIZE + 16 + 1];
//static uint8_t g_transfer_rx_buffer[64];

//#define SPIM_THROUGHPUT_TEST


#ifdef SPIM_THROUGHPUT_TEST
#define SPI_TEST_DATA_SIZE      (1600)
#define SPI_TEST_DATA_RANGE     115
#define SPIS_CFG_LENGTH         (SPI_TEST_DATA_SIZE - 1)

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_transfer_tx_big_buffer[SPI_TEST_DATA_SIZE + 1];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_transfer_rx_big_buffer[SPI_TEST_DATA_SIZE + 1];
#endif //SPIM_THROUGHPUT_TEST

#if 0
void spim_dma_memcpy(void *src, void *dst, unsigned int len)    
{                                                                      
    hal_gdma_status_t ret;                                             
    hal_gdma_running_status_t running_status;                          
    hal_gdma_channel_t channel = HAL_GDMA_CHANNEL_0;                   
                                                             
    ret = hal_gdma_start_polling(channel, (int)dst, (int)src, len);
        if(HAL_GDMA_STATUS_OK != ret)                                  
        {                                                              
            printf("DMA unavailable...\n");                            
            os_memcpy(dst, src, len);                                     
        }                                                              
                                                                       
        hal_gdma_get_running_status(channel,&running_status);          
        hal_gdma_stop(channel); // stop GDMA.                          
}                   
#endif

static void spi_master_dma_callback(hal_spi_master_callback_event_t event, void *user_data)
{
    //printf("  spi_cb_entered, event = %d\r\n", event);
    g_spi_transaction_finish = true;
}

void spi_query_slave_status(uint16_t cmdset, uint8_t *status)
{
    uint8_t status_cmd;
    uint8_t status_receive[2];
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
                                  
    status_cmd = (cmdset?SPIS_FW_RS_CMD:SPIS_RS_CMD);
    	
    /* Note:
     * The value of receive_length is the valid number of bytes received plus the number of bytes to send.
     * For example, here the valid number of bytes received is 1 byte,
     * and the number of bytes to send also is 1 byte, so the receive_length is 2.
     */
    status_receive[1] = 0;
    spi_send_and_receive_config.receive_length = 2;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.send_data = &status_cmd;
    spi_send_and_receive_config.receive_buffer = status_receive;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(SPIM_TEST_PORT, &spi_send_and_receive_config)) {
        log_hal_error("SPI master query status of slaver failed\r\n");
        return;
    }

    //printf("  Status receive: 0x%x\r\n", status_receive[1]);
    *status = status_receive[1];
}

void spi_master_read_request(uint16_t cmdset, uint8_t *buffer, uint32_t length, uint32_t wait_ms)
{
    uint8_t cfg_rd_cmd[9];
    uint8_t status_receive = 0;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    /* SPIM configure read test */
    //printf("  SPI master starts to send CR command to slave\r\n");
//    printf("[WFCM]:CR -> Slave\r\n");
    cfg_rd_cmd[0] = (cmdset?SPIS_FW_CFG_RD_CMD:SPIS_CFG_RD_CMD);    	
    cfg_rd_cmd[1] = SPIS_ADDRESS_ID & 0xff;
    cfg_rd_cmd[2] = (SPIS_ADDRESS_ID >> 8) & 0xff;
    cfg_rd_cmd[3] = (SPIS_ADDRESS_ID >> 16) & 0xff;
    cfg_rd_cmd[4] = (SPIS_ADDRESS_ID >> 24) & 0xff;
    cfg_rd_cmd[5] = (length - 1) & 0xff;
    cfg_rd_cmd[6] = ((length - 1) >> 8) & 0xff;
    cfg_rd_cmd[7] = ((length - 1) >> 16) & 0xff;
    cfg_rd_cmd[8] = ((length - 1) >> 24) & 0xff;

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, cfg_rd_cmd, 9)) {
       log_hal_error("  SPI master send CR command failed\r\n");
       return;
    }

    while (1)
    {	
        spi_query_slave_status(cmdset, &status_receive);
        if ((status_receive & (uint8_t)SPI_STATUS_TXRX_FIFO_RDY_MASK) == (uint8_t)SPI_STATUS_TXRX_FIFO_RDY_MASK) 
           break;
       	//hal_gpt_delay_ms(wait_ms);
    }

    /* Step2: Master read 4 bytes from the slave */
    //printf("  SPI master starts to send RD command to slave using DMA mode\r\n");
//    printf("[WFCM]: RD(DMA) -> Slave\r\n");
    g_spi_transaction_finish = false;
    g_transfer_tx_buffer[0] = (cmdset?SPIS_FW_RD_CMD:SPIS_RD_CMD);
//    memset(buffer, 0, (length + 1));
    spi_send_and_receive_config.receive_length = length + 1;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.send_data = g_transfer_tx_buffer;
    spi_send_and_receive_config.receive_buffer = g_transfer_rx_buffer;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_dma(SPIM_TEST_PORT, &spi_send_and_receive_config)) {
        printf("  SPI master send dma failed \r\n");
    }
    while (g_spi_transaction_finish == false);

    do {
        spi_query_slave_status(cmdset, &status_receive);
    } while ((status_receive & (uint8_t)SPI_STATUS_RDWR_FINISH_MASK) != (uint8_t)SPI_STATUS_RDWR_FINISH_MASK);

    memcpy(buffer, &(g_transfer_rx_buffer[1]), length);
//printf("src(%p), dst(%p)\r\n",&(g_transfer_rx_buffer[1]), buffer);    
    //spim_dma_memcpy(buffer, &(g_transfer_rx_buffer[1]), length);
}

void spi_master_write_data(uint16_t cmdset, uint8_t *buffer, uint32_t length)
{
    uint8_t cfg_wr_cmd[9];
    uint8_t status_receive = 0;

    /* SPIM configure write test */
    //printf("  SPI master starts to send cfg_wr_cmd command to slave\r\n");
//    printf("[WFCM]: cfg_wr_cmd -> Slave\r\n");
    cfg_wr_cmd[0] = (cmdset?SPIS_FW_CFG_WR_CMD:SPIS_CFG_WR_CMD);
    cfg_wr_cmd[1] = SPIS_ADDRESS_ID & 0xff;
    cfg_wr_cmd[2] = (SPIS_ADDRESS_ID >> 8) & 0xff;
    cfg_wr_cmd[3] = (SPIS_ADDRESS_ID >> 16) & 0xff;
    cfg_wr_cmd[4] = (SPIS_ADDRESS_ID >> 24) & 0xff;
    cfg_wr_cmd[5] = (length - 1) & 0xff;
    cfg_wr_cmd[6] = ((length - 1) >> 8) & 0xff;
    cfg_wr_cmd[7] = ((length - 1) >> 16) & 0xff;
    cfg_wr_cmd[8] = ((length - 1) >> 24) & 0xff; 
         
    //printf("  SPI master enter send cfg_wr_cmd \r\n");
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, cfg_wr_cmd, 9)) {
       log_hal_error("  SPI master send cfg_wr_cmd command failed\r\n");
       return;
    }

    while(1)
    {        
    	spi_query_slave_status(cmdset, &status_receive);
        if ((status_receive & (uint8_t)SPI_STATUS_TXRX_FIFO_RDY_MASK) == (uint8_t)SPI_STATUS_TXRX_FIFO_RDY_MASK) 
            break;
        //hal_gpt_delay_ms(0);
    }

    //printf("  SPI master starts to send wr_cmd command to slave using DMA mode\r\n");
//    printf("[WFCM]: wr_cmd(DMA) -> Slave\r\n");
    g_spi_transaction_finish = false;
    g_transfer_tx_buffer[0] = (cmdset?SPIS_FW_WR_CMD:SPIS_WR_CMD);
    memcpy(&(g_transfer_tx_buffer[1]), buffer, length);
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_dma(SPIM_TEST_PORT, g_transfer_tx_buffer, (length + 1))) {
        log_hal_error("\r\n SPI master send wr_cmd command failed\r\n");
    }
    while (g_spi_transaction_finish == false);

    do {
        spi_query_slave_status(cmdset, &status_receive);
    } while ((status_receive & (uint8_t)SPI_STATUS_RDWR_FINISH_MASK) != (uint8_t)SPI_STATUS_RDWR_FINISH_MASK);
}

void spi_master_write_pbuf(uint16_t cmdset, struct pbuf *p, uint32_t totalSize)
{
	struct pbuf *q;
	uint8_t *pdst;
    uint8_t cfg_wr_cmd[9];
    uint8_t status_receive = 0;

    /* SPIM configure write test */
    //printf("  SPI master starts to send cfg_wr_cmd command to slave\r\n");
//    printf("[WFCM]: cfg_wr_cmd -> Slave\r\n");
    cfg_wr_cmd[0] = (cmdset?SPIS_FW_CFG_WR_CMD:SPIS_CFG_WR_CMD);
    cfg_wr_cmd[1] = SPIS_ADDRESS_ID & 0xff;
    cfg_wr_cmd[2] = (SPIS_ADDRESS_ID >> 8) & 0xff;
    cfg_wr_cmd[3] = (SPIS_ADDRESS_ID >> 16) & 0xff;
    cfg_wr_cmd[4] = (SPIS_ADDRESS_ID >> 24) & 0xff;
    cfg_wr_cmd[5] = (totalSize - 1) & 0xff;
    cfg_wr_cmd[6] = ((totalSize - 1) >> 8) & 0xff;
    cfg_wr_cmd[7] = ((totalSize - 1) >> 16) & 0xff;
    cfg_wr_cmd[8] = ((totalSize - 1) >> 24) & 0xff; 
         
    //printf("  SPI master enter send cfg_wr_cmd \r\n");
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, cfg_wr_cmd, 9)) {
       log_hal_error("  SPI master send cfg_wr_cmd command failed\r\n");
       return;
    }

    while(1)
    {        
    	spi_query_slave_status(cmdset, &status_receive);
        if ((status_receive & (uint8_t)SPI_STATUS_TXRX_FIFO_RDY_MASK) == (uint8_t)SPI_STATUS_TXRX_FIFO_RDY_MASK) 
            break;
        //hal_gpt_delay_ms(0);
    }

    //printf("  SPI master starts to send wr_cmd command to slave using DMA mode\r\n");
//    printf("[WFCM]: wr_cmd(DMA) -> Slave\r\n");
    g_spi_transaction_finish = false;
    g_transfer_tx_buffer[0] = (cmdset?SPIS_FW_WR_CMD:SPIS_WR_CMD);
                                                                
    // Send to Slave
    pdst = &(g_transfer_tx_buffer[1]);
    for(q = p; q != NULL; q = q->next) {
       memcpy(pdst, (uint8_t*)q->payload, q->len);
//printf("src(%p), dst(%p), len(%d)\r\n",q->payload, pdst, q->len);        
       //spim_dma_memcpy(pdst, (uint8_t*)q->payload, q->len);
       pdst += q->len;
    }        	
    //memcpy(&(g_transfer_tx_buffer[1]), buffer, totalSize);
    
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_dma(SPIM_TEST_PORT, g_transfer_tx_buffer, (totalSize + 1))) {
        log_hal_error("\r\n SPI master send wr_cmd command failed\r\n");
    }
    while (g_spi_transaction_finish == false);

    do {
        spi_query_slave_status(cmdset, &status_receive);
    } while ((status_receive & (uint8_t)SPI_STATUS_RDWR_FINISH_MASK) != (uint8_t)SPI_STATUS_RDWR_FINISH_MASK);
}

void spi_wait_power_on(uint16_t cmdset)
{
    uint8_t status_receive;
    uint8_t poweron_cmd;
    //uint8_t cfg_mode[2];
        	
    // Wait SPI Slave Software Reset 
    do {
    	hal_gpt_delay_ms(100);
        spi_query_slave_status(cmdset, &status_receive);        
    } while ((status_receive & (uint8_t)SPI_STATUS_SLV_ON_MASK) == (uint8_t)SPI_STATUS_SLV_ON_MASK);

    ///* Step3: Config SPI slave to use quad mode */
    //printf("SPI Master starts to send CT command to SPI slave\r\n");
    //cfg_mode[0] = SPIS_CT_CMD;
    //cfg_mode[1] = SPIS_CFG_4BYTE_ADDR | SPIS_CFG_QUAD_MODE;
    //if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, cfg_mode, 2)) {
    //    printf("  SPI master send CT command failed\r\n");
    //    return;
    //}

    /* Step1: SPI Master starts to send power_on command to SPI slave */
    //printf("SPI Master starts to send PWON command to SPI slave\r\n");
    printf("[WFCM]: PWON -> Slave\r\n");
    poweron_cmd = SPIS_FW_PWON_CMD;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, &poweron_cmd, 1)) {
        printf("SPI master sends PWON command fail\r\n");
        return;
    }    	
    do {
    	hal_gpt_delay_ms(100);
        spi_query_slave_status(cmdset, &status_receive);
    } while ((status_receive & (uint8_t)SPI_STATUS_SLV_ON_MASK) != (uint8_t)SPI_STATUS_SLV_ON_MASK);
}

void wfcm_if_init(void)
{
    hal_spi_master_config_t          spi_config;
    hal_spi_master_advanced_config_t advanced_config;	
/*
    hal_gdma_running_status_t dma_status; 

    dma_status = hal_gdma_init(HAL_GDMA_CHANNEL_0);
*/
//Temp
#if 1
    /* Step1: GPIO configuration for SPI master */
    hal_gpio_init(SPIM_PIN_NUMBER_CS);
    hal_gpio_init(SPIM_PIN_NUMBER_CLK);
    hal_gpio_init(SPIM_PIN_NUMBER_SIO_0);
    hal_gpio_init(SPIM_PIN_NUMBER_SIO_1);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_CS, SPIM_PIN_FUNC_CS);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_CLK, SPIM_PIN_FUNC_CLK);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_SIO_0, SPIM_PIN_FUNC_SIO_0);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_SIO_1, SPIM_PIN_FUNC_SIO_1);
#if (PRODUCT_VERSION != 2523)
    hal_gpio_set_driving(SPIM_PIN_NUMBER_CLK, HAL_GPIO_DRIVING_8MA); /* for 48MHz, must adjust the driving of CLK */
#endif
#else
    hal_gpio_init(SPIM_PIN_NUMBER_CS);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_CS, SPIM_PIN_FUNC_CS);
#endif
    
    /* Step2: Initializes SPI master with normal mode */
    spi_config.bit_order       = HAL_SPI_MASTER_LSB_FIRST;
#if (PRODUCT_VERSION == 2523)
    spi_config.slave_port      = SPI_TEST_MASTER_SLAVE;
#endif
    spi_config.clock_frequency = SPIM_TEST_FREQUENCY;
    spi_config.phase           = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity        = HAL_SPI_MASTER_CLOCK_POLARITY0;
    
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(SPIM_TEST_PORT, &spi_config)) {
        printf("SPI master init failed\r\n");
        return;
    }
    
    
    advanced_config.byte_order    = HAL_SPI_MASTER_LITTLE_ENDIAN;
    advanced_config.chip_polarity = HAL_SPI_MASTER_CHIP_SELECT_LOW;
    advanced_config.get_tick      = HAL_SPI_MASTER_GET_TICK_DELAY2; /* for 48MHz, must adjust delay_tick */
    advanced_config.sample_select = HAL_SPI_MASTER_SAMPLE_POSITIVE;
    
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_set_advanced_config(SPIM_TEST_PORT, &advanced_config)) {
        printf("SPI master set advanced config failed\r\n");
        return;
    }
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_register_callback(SPIM_TEST_PORT, spi_master_dma_callback, NULL)) {
        printf("SPI master register callback failed\r\n");
        return;
    }    
}

void wfcm_if_reinit(void)
{
    hal_spi_master_config_t          spi_config;
    hal_spi_master_advanced_config_t advanced_config;	
    
    /* Step2: Initializes SPI master with normal mode */
    spi_config.bit_order       = HAL_SPI_MASTER_LSB_FIRST;
    spi_config.clock_frequency = SPIM_TEST_FREQUENCY;
    spi_config.phase           = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity        = HAL_SPI_MASTER_CLOCK_POLARITY0;
    
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(SPIM_TEST_PORT, &spi_config)) {
        printf("SPI master init failed\r\n");
        return;
    }
       
    advanced_config.byte_order    = HAL_SPI_MASTER_LITTLE_ENDIAN;
    advanced_config.chip_polarity = HAL_SPI_MASTER_CHIP_SELECT_LOW;
    advanced_config.get_tick      = HAL_SPI_MASTER_GET_TICK_DELAY2; /* for 48MHz, must adjust delay_tick */
    advanced_config.sample_select = HAL_SPI_MASTER_SAMPLE_POSITIVE;
    
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_set_advanced_config(SPIM_TEST_PORT, &advanced_config)) {
        printf("SPI master set advanced config failed\r\n");
        return;
    }
}

void wfcm_if_deinit(void)
{
    /* Step11: De-initialize SPI master and the GPIO. */
    printf("SPI master deinit\r\n\r\n");
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(SPIM_TEST_PORT)) {
        printf("SPI master deinit failed\r\n");
        return;
    }
    hal_gpio_deinit(SPIM_PIN_NUMBER_CS);
    hal_gpio_deinit(SPIM_PIN_NUMBER_CLK);
    hal_gpio_deinit(SPIM_PIN_NUMBER_SIO_0);
    hal_gpio_deinit(SPIM_PIN_NUMBER_SIO_1);
    //hal_gpio_deinit(SPIM_PIN_NUMBER_SIO_2);
    //hal_gpio_deinit(SPIM_PIN_NUMBER_SIO_3);
}

#ifdef SPIM_THROUGHPUT_TEST
void spim_througput_test(void)
{
#define SPI_TEST_LOOP_COUNT   10

extern SPIM_REGISTER_T *const g_spi_master_register[HAL_SPI_MASTER_MAX];

uint32_t i;
	uint8_t status;
	uint32_t temp, loop_cnt, throughtput;
	uint32_t time_wr_begin, time_wr_end, time_wr_total;
	uint32_t time_rd_begin, time_rd_end, time_rd_total;

	time_wr_total = 0;
	time_rd_total = 0;
	loop_cnt = 0;
	
	printf("\r\n\r\nspi slave throughput test begin with single mode\r\n\r\n");

	/* write operation */
	for (i = 1; i < (SPI_TEST_DATA_SIZE + 1); i++) {
		g_transfer_tx_big_buffer[i] = (i % SPI_TEST_DATA_RANGE);
	}

	while(1) {
		hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_wr_begin);

		//printf("1: send cfg_wr_cmd command\r\n\r\n");

		g_spi_master_register[SPIM_TEST_PORT]->CTRL1_UNION.CTRL1_CELLS.TXDMA_EN = 0;
		g_spi_master_register[SPIM_TEST_PORT]->CTRL1_UNION.CTRL1_CELLS.RXDMA_EN = 0;

		g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.RST = 1; /* clear fifo */
		g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
		g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = 8;
		g_spi_master_register[SPIM_TEST_PORT]->TX_DATA = SPIS_FW_CFG_WR_CMD | ((SPIS_ADDRESS_ID & 0xff) << 8) | ((SPIS_ADDRESS_ID & 0xff00) << 8) | ((SPIS_ADDRESS_ID & 0xff0000) << 8);
		g_spi_master_register[SPIM_TEST_PORT]->TX_DATA = ((SPIS_ADDRESS_ID >> 24) & 0xff) | ((SPIS_CFG_LENGTH & 0xff) << 8) | ((SPIS_CFG_LENGTH & 0xff00) << 8) | ((SPIS_CFG_LENGTH & 0xff0000) << 8);
		g_spi_master_register[SPIM_TEST_PORT]->TX_DATA = ((SPIS_CFG_LENGTH >> 24) & 0xff);
		g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
		while ((g_spi_master_register[SPIM_TEST_PORT]->STA) == SPIM_STATUS_BUSY_MASK);
		status = (g_spi_master_register[SPIM_TEST_PORT]->INT);

		//printf("2: send cfg_wr_cmd command ok\r\n\r\n");

		do {
			g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.RST = 1; /* clear fifo */
			g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
			g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = 1;
			g_spi_master_register[SPIM_TEST_PORT]->TX_DATA = SPIS_FW_RS_CMD;
			g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
			while ((g_spi_master_register[SPIM_TEST_PORT]->STA) == SPIM_STATUS_BUSY_MASK);
			status = (g_spi_master_register[SPIM_TEST_PORT]->INT);
			temp = g_spi_master_register[SPIM_TEST_PORT]->RX_DATA;
			status = (temp >> 8) & 0xff;
		} while((status & (uint8_t)SPI_STATUS_TXRX_FIFO_RDY_MASK) != (uint8_t)SPI_STATUS_TXRX_FIFO_RDY_MASK);

		//printf("3: read respond of cfg_wr_cmd command ok\r\n\r\n");

		//printf("4: send wr_cmd command\r\n\r\n");

		//g_spi_master_register[SPIM_TEST_PORT]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= 0x02 << SPIM_CTRL0_TYPE_OFFSET;
		g_spi_master_register[SPIM_TEST_PORT]->CFG3_UNION.CFG3_CELLS.COMMAND_CNT = 1;

		g_transfer_tx_big_buffer[0] = SPIS_FW_WR_CMD;
		g_spi_master_register[SPIM_TEST_PORT]->TX_SRC = (uint32_t)g_transfer_tx_big_buffer;
		g_spi_master_register[SPIM_TEST_PORT]->CTRL1_UNION.CTRL1_CELLS.TXDMA_EN = 1;
		g_spi_master_register[SPIM_TEST_PORT]->CTRL1_UNION.CTRL1_CELLS.RXDMA_EN = 0;
		g_spi_master_register[SPIM_TEST_PORT]->IE = (SPIM_IE_FINISH_IE_MASK | SPIM_IE_PAUSE_IE_MASK);
		g_spi_master_register[SPIM_TEST_PORT]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= (1<<SPIM_CTRL0_RW_MODE_OFFSET);
		g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
		g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = SPI_TEST_DATA_SIZE;
		g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
		while ((g_spi_master_register[SPIM_TEST_PORT]->STA) == SPIM_STATUS_BUSY_MASK);
		while (((g_spi_master_register[SPIM_TEST_PORT]->INT) & SPIM_INT_FINISH_INT_MASK) != SPIM_INT_FINISH_INT_MASK);

		//printf("5: send wr_cmd command ok\r\n\r\n");

		//g_spi_master_register[SPIM_TEST_PORT]->CTRL0_UNION.CTRL0_CELLS.CTRL0 &= ~SPIM_CTRL0_TYPE_MASK;
		g_spi_master_register[SPIM_TEST_PORT]->CTRL1_UNION.CTRL1_CELLS.TXDMA_EN = 0;
		g_spi_master_register[SPIM_TEST_PORT]->CTRL1_UNION.CTRL1_CELLS.RXDMA_EN = 0;

		do {
			g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.RST = 1; /* clear fifo */
			g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
			g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = 1;
			g_spi_master_register[SPIM_TEST_PORT]->TX_DATA = SPIS_FW_RS_CMD;
			g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
			while ((g_spi_master_register[SPIM_TEST_PORT]->STA) == SPIM_STATUS_BUSY_MASK);
			status = (g_spi_master_register[SPIM_TEST_PORT]->INT);
			temp = g_spi_master_register[SPIM_TEST_PORT]->RX_DATA;
			status = (temp >> 8) & 0xff;
		} while((status & (uint8_t)SPI_STATUS_RDWR_FINISH_MASK) != (uint8_t)SPI_STATUS_RDWR_FINISH_MASK);

		//printf("6: read respond of wr_cmd command ok\r\n\r\n");

		hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_wr_end);
		time_wr_total += time_wr_end - time_wr_begin;

#if 0
		/* read operation */
		memset(g_transfer_rx_big_buffer, 0x00, sizeof(g_transfer_rx_big_buffer));
#endif
		//printf("7: send cfg_rd_cmd command\r\n\r\n");

        /* SPI slave callback need some time */
		hal_gpt_delay_us(20);

		hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_rd_begin);

		/* Step4: Master starts to send cfg_wr_cmd command to slave */
		g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.RST = 1; /* clear fifo */
		g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
		g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = 8;
		g_spi_master_register[SPIM_TEST_PORT]->TX_DATA = SPIS_FW_CFG_RD_CMD | ((SPIS_ADDRESS_ID & 0xff) << 8) | ((SPIS_ADDRESS_ID & 0xff00) << 8) | ((SPIS_ADDRESS_ID & 0xff0000) << 8);
		g_spi_master_register[SPIM_TEST_PORT]->TX_DATA = ((SPIS_ADDRESS_ID >> 24) & 0xff) | ((SPIS_CFG_LENGTH & 0xff) << 8) | ((SPIS_CFG_LENGTH & 0xff00) << 8) | ((SPIS_CFG_LENGTH & 0xff0000) << 8);
		g_spi_master_register[SPIM_TEST_PORT]->TX_DATA = ((SPIS_CFG_LENGTH >> 24) & 0xff);
		g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
		while ((g_spi_master_register[SPIM_TEST_PORT]->STA) == SPIM_STATUS_BUSY_MASK);
		status = (g_spi_master_register[SPIM_TEST_PORT]->INT);

		//printf("8: send cfg_rd_cmd command ok\r\n\r\n");

		do {
			g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.RST = 1; /* clear fifo */
			g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
			g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = 1;
			g_spi_master_register[SPIM_TEST_PORT]->TX_DATA = SPIS_FW_RS_CMD;
			g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
			while ((g_spi_master_register[SPIM_TEST_PORT]->STA) == SPIM_STATUS_BUSY_MASK);
			status = (g_spi_master_register[SPIM_TEST_PORT]->INT);
			temp = g_spi_master_register[SPIM_TEST_PORT]->RX_DATA;
			status = (temp >> 8) & 0xff;
		} while((status & (uint8_t)SPI_STATUS_TXRX_FIFO_RDY_MASK) != (uint8_t)SPI_STATUS_TXRX_FIFO_RDY_MASK);

		//printf("9: read respond of cfg_rd_cmd command ok\r\n\r\n");

		//g_spi_master_register[SPIM_TEST_PORT]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= 0x02 << SPIM_CTRL0_TYPE_OFFSET;
		g_spi_master_register[SPIM_TEST_PORT]->CFG3_UNION.CFG3_CELLS.COMMAND_CNT = 1;

		//printf("10: send rd_cmd command\r\n\r\n");

		g_transfer_tx_big_buffer[0] = SPIS_FW_RD_CMD;
		g_spi_master_register[SPIM_TEST_PORT]->TX_SRC = (uint32_t)g_transfer_tx_big_buffer;
		g_spi_master_register[SPIM_TEST_PORT]->RX_DST = (uint32_t)g_transfer_rx_big_buffer;
		g_spi_master_register[SPIM_TEST_PORT]->CTRL1_UNION.CTRL1_CELLS.TXDMA_EN = 1;
		g_spi_master_register[SPIM_TEST_PORT]->CTRL1_UNION.CTRL1_CELLS.RXDMA_EN = 1;
		g_spi_master_register[SPIM_TEST_PORT]->IE = (SPIM_IE_FINISH_IE_MASK | SPIM_IE_PAUSE_IE_MASK);
		g_spi_master_register[SPIM_TEST_PORT]->CTRL0_UNION.CTRL0_CELLS.CTRL0 &= ~(1<<SPIM_CTRL0_RW_MODE_OFFSET);
		g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
		g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = SPI_TEST_DATA_SIZE;
		g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
		while ((g_spi_master_register[SPIM_TEST_PORT]->STA) == SPIM_STATUS_BUSY_MASK);
		while (((g_spi_master_register[SPIM_TEST_PORT]->INT) & SPIM_INT_FINISH_INT_MASK) != SPIM_INT_FINISH_INT_MASK);

		//printf("11: send rd_cmd command ok\r\n\r\n");

		//g_spi_master_register[SPIM_TEST_PORT]->CTRL0_UNION.CTRL0_CELLS.CTRL0 &= ~SPIM_CTRL0_TYPE_MASK;
        g_spi_master_register[SPIM_TEST_PORT]->CTRL1_UNION.CTRL1_CELLS.TXDMA_EN = 0;
		g_spi_master_register[SPIM_TEST_PORT]->CTRL1_UNION.CTRL1_CELLS.RXDMA_EN = 0;

		do {
			g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.RST = 1; /* clear fifo */
			g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
			g_spi_master_register[SPIM_TEST_PORT]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = 1;
			g_spi_master_register[SPIM_TEST_PORT]->TX_DATA = SPIS_FW_RS_CMD;
			g_spi_master_register[SPIM_TEST_PORT]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
			while ((g_spi_master_register[SPIM_TEST_PORT]->STA) == SPIM_STATUS_BUSY_MASK);
			status = (g_spi_master_register[SPIM_TEST_PORT]->INT);
			temp = g_spi_master_register[SPIM_TEST_PORT]->RX_DATA;
			status = (temp >> 8) & 0xff;
		} while((status & (uint8_t)SPI_STATUS_RDWR_FINISH_MASK) != (uint8_t)SPI_STATUS_RDWR_FINISH_MASK);

		hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_rd_end);

		//printf("12: read respond of rd_cmd command ok\r\n\r\n");

		time_rd_total += time_rd_end - time_rd_begin;

#if 1
		//printf("Start to check data integrity\r\n");
		for (i = 1; i < (SPI_TEST_DATA_SIZE + 1); i++) {
			if (g_transfer_rx_big_buffer[i] != (i % SPI_TEST_DATA_RANGE)) {
				printf("  Data check fail\r\n");
				return;
			}
		}
		//printf("  Data check pass\r\n");
#endif

        hal_gpt_delay_us(20);
        
		//printf("time_rd_end - time_rd_begin = %dus\r\n", (time_rd_end - time_rd_begin));
		//printf("time_wr_end - time_wr_begin = %dus\r\n", (time_wr_end - time_wr_begin));
		
		loop_cnt++;
		if (loop_cnt > SPI_TEST_LOOP_COUNT) {
			throughtput = SPI_TEST_DATA_SIZE * 8;
			throughtput /= (time_wr_total / loop_cnt);
			printf("\r\n\r\nSPI slave write throughtput = %d Mbps\r\n", throughtput);
			throughtput = SPI_TEST_DATA_SIZE * 8;
			throughtput /= (time_rd_total / loop_cnt);
			printf("SPI slave read throughtput = %d Mbps\r\n\r\n", throughtput);
			loop_cnt = 0;
			time_rd_total = 0;
			time_wr_total = 0;
		}
	}

}
#endif

