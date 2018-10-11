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
#include "queue.h"
#include "sys_init.h"
#include "hal.h"

#include "wfc_stub.h"

#define SPIM_TEST_PORT          HAL_SPI_SLAVE_0
#define SPI_TX_DATA_SIZE      1600
#define SPI_RX_DATA_SIZE      256

/* Private variables ---------------------------------------------------------*/

static volatile bool spi_slave_poweron_flag;
static volatile bool spi_slave_cfg_write_flag;
static volatile bool spi_slave_write_data_flag;
static volatile bool spi_slave_cfg_read_flag;
static volatile bool spi_slave_read_data_flag;
static volatile bool spi_slave_timeout_flag;
static volatile bool spi_slave_read_error_flag;
static volatile bool spi_slave_write_error_flag;

static uint8_t g_tx_tansfer_buffer[SPI_TX_DATA_SIZE];
static uint8_t g_rx_tansfer_buffer[SPI_RX_DATA_SIZE];

static uint32_t txdata_length;
static uint32_t rxdata_length;

static void wfc_spi_slave_callback(hal_spi_slave_transaction_status_t status, void *user_data)
{
    uint16_t slave_status;
    //hal_spi_slave_config_t spi_configure;

    //printf( "<WFC> : fsm(0x%x)...%s-%d\r\n",status.fsm_status,__FUNCTION__,__LINE__);
   // printf( "F(0x%x, 0x%x)\r\n",status.fsm_status,status.interrupt_status);
    if (HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION == (status.fsm_status)) {
        /* Normal fsm behavior */
        slave_status = status.interrupt_status;
        switch (slave_status) {
            case SPIS_INT_POWER_ON_MASK:
                /* PDN is turned on, initializes spi slave controller here */
                spi_slave_poweron_flag = true;
                //printf( "---Receive POWERON command----\r\n\r\n");
                break;
            case SPIS_INT_POWER_OFF_MASK:
                hal_spi_slave_deinit(SPIM_TEST_PORT);
                spi_slave_poweron_flag = false;
                //printf( "---Receive POWEROFF command----\r\n\r\n");
                break;
            case SPIS_INT_RD_CFG_FINISH_MASK:
                /* Call SPI slave send function here to set data address and size*/
                spi_slave_cfg_read_flag = true;
                //printf( "---Receive CRD_FINISH command----\r\n\r\n");
                break;
            case SPIS_INT_WR_CFG_FINISH_MASK:
                /* call spi slave read function here to set data address and size*/
                spi_slave_cfg_write_flag = true;
                //printf( "---Receive CWR_FINISH command----\r\n\r\n");
                break;
            case SPIS_INT_RD_TRANS_FINISH_MASK:
                spi_slave_read_data_flag = true;
                //printf( "---Receive RD_FINISH command----\r\n\r\n");
                break;
            case SPIS_INT_WR_TRANS_FINISH_MASK:
                /* User can now get the data from the address set before */
                spi_slave_write_data_flag = true;
                //printf( "---Receive WR_FINISH command----\r\n\r\n");
                break;
            case SPIS_INT_RD_DATA_ERR_MASK:
                /* Data buffer may be reserved for retransmit depending on user's usage */
                spi_slave_read_error_flag = true;
                //printf( "---detect RD_ERR----\r\n\r\n");
                break;
            case SPIS_INT_WR_DATA_ERR_MASK:
                /* Data in the address set before isn't correct, user may abandon them */
                spi_slave_write_error_flag = true;
                //printf( "---detect WR_ERR----\r\n\r\n");
                break;
            case SPIS_INT_TMOUT_ERR_MASK:
                /* timeout happen */
                spi_slave_timeout_flag = true;
                //printf( "---detect TMOUT_ERR----\r\n\r\n");
                break;
            default:
                printf( "[WFC]: Error - FSM(0x%x, 0x%x)\r\n",status.fsm_status,status.interrupt_status);
                break;
        }
    } else if (HAL_SPI_SLAVE_FSM_INVALID_OPERATION != (status.fsm_status)) {
        switch (status.fsm_status) {
            case HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR, fsm is poweroff\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW, fsm is poweroff\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR, fsm is CR\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW, fsm is CR\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW, fsm is CW\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR, fsm is CW\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR, fsm is poweron\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW:
                //printf( "HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW, fsm is poweron\r\n");
                break;
            default:
            	//printf( "<WFC>...%s-%d\n",__FUNCTION__,__LINE__);
                break;
        }
    } else {
        //printf( "HAL_SPI_SLAVE_FSM_INVALID_OPERATION, fsm is poweron\r\n");
    }
}


void wfc_if_init(void)
{
	hal_spi_slave_config_t spi_configure;
	    
    // Reset SPI slaver status
    spi_slave_poweron_flag     = false;
    spi_slave_cfg_write_flag   = false;
    spi_slave_write_data_flag  = false;
    spi_slave_write_error_flag = false;
    spi_slave_cfg_read_flag    = false;
    spi_slave_read_data_flag   = false;
    spi_slave_read_error_flag  = false;    
    spi_slave_timeout_flag     = false;        
    
    // Reset transfer buffer
    txdata_length = 0;
    rxdata_length = 0;

    // Set SPI Configure
    spi_configure.bit_order = HAL_SPI_SLAVE_LSB_FIRST;
    spi_configure.phase = HAL_SPI_SLAVE_CLOCK_PHASE0;
    spi_configure.polarity = HAL_SPI_SLAVE_CLOCK_POLARITY0;
    spi_configure.timeout_threshold = 0xFFFFFFFF;
    hal_spi_slave_init(SPIM_TEST_PORT, &spi_configure);

    // Register callback function to SPI slaver driver
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_register_callback(SPIM_TEST_PORT, wfc_spi_slave_callback, NULL)) {
        //printf( "SPI slave register callback failed\r\n");
    }

    // Wait SPI master to send power on command.
    //printf( "SPI Slave is waiting for PWON command\r\n");
    while (spi_slave_poweron_flag == false) {
        if (spi_slave_timeout_flag == true) {
            //printf( "Slave detect PWON command error\r\n");
            return;
        }
    }	
}

int wfc_get_opcode(T_WFC_OPCODE  *op)
{
    uint16_t *opcode = (uint16_t *)&g_rx_tansfer_buffer[0];

    // Init SPI rx transfer buffer	
//    memset(g_rx_tansfer_buffer,0,sizeof(g_rx_tansfer_buffer));        

    // Wait SPI master to send config write command.
    //printf( "SPI Slave is waiting for CW command\r\n");
    while (spi_slave_cfg_write_flag == false) hal_gpt_delay_us(0);

    // Reset CWR_FINISH flag
    spi_slave_cfg_write_flag = false; 

    // Start to receive rx data
    rxdata_length = *(volatile uint32_t *)(0xA00B0014);
    hal_spi_slave_receive(SPIM_TEST_PORT, g_rx_tansfer_buffer, rxdata_length);

    // Wait SPI master to send write data command.
    //printf( "SPI Slave is waiting for WR command\r\n");
    while (spi_slave_write_data_flag == false) {
        if (spi_slave_write_error_flag == true) {
            //printf( "Slave detect WR command error\r\n");
            op->opcode = WFC_INVALID_OPCODE;
            return -1;
        }
    }
                      
    spi_slave_write_data_flag = false;
    
    // Dump data received (Debug)
    //printf("\r\nData Length == %d\r\n",rxdata_length);         
    //for (int i = 0; i < rxdata_length; i++)
    //    printf("0x%02x ", g_rx_tansfer_buffer[i]);
                
    // Get opcode from received data
    //opcode = &g_rx_tansfer_buffer[0];
    op->opcode = *opcode;  
    op->rxptr  =  wifi_os_malloc(rxdata_length-1);
    memcpy(op->rxptr, &g_rx_tansfer_buffer[2], rxdata_length-2);
    op->rxptr[rxdata_length-2] = 0;
    
    return 0;      
}


void wfc_send_response(T_WFC_OPCODE *pwfc, uint16_t rspcode, uint8_t *txptr, uint32_t txsize, uint32_t hsize)
{
    // Init SPI tx transfer buffer
    //memset(g_tx_tansfer_buffer,0,sizeof(g_tx_tansfer_buffer));     

    // Set DTH data response code into SPI response
    rspcode = (hsize)?(rspcode|WFC_RSP_DTH_DATA):rspcode;
    	
    // Assign response code to tx transfer buffer
    pwfc->rspcode = (uint16_t *)&g_tx_tansfer_buffer[0];
    *(pwfc->rspcode) = rspcode;
    
    // Copy DTH data size info into tx transfer buffer
    pwfc->txptr = &g_tx_tansfer_buffer[sizeof(rspcode)];  
    if (hsize)
    {
    	memcpy(pwfc->txptr, &hsize, sizeof(hsize));
    	pwfc->txptr +=  sizeof(hsize);
    }
    
    // Copy response content into tx transfer buffer	  	
    if ( txptr && txsize) {	
        memcpy(pwfc->txptr, txptr, txsize);
    }    

    // Dump data received (Debug)
    //if (hsize)
    //{	   
    //  printf("rsp:\r\n");
    //  for (int i = 0; i < 8; i++)
    //    printf("0x%02x ", g_tx_tansfer_buffer[i]);
    //  printf("\r\n\r\n");  
    //}


    // Wait SPI master to send config read command.
    //printf( "SPI Slave is waiting for CR command\r\n");
    while (spi_slave_cfg_read_flag == false) hal_gpt_delay_us(0);


    // Reset CRD_FINISH flag
    spi_slave_cfg_read_flag = false;

    // Assign response length
    txdata_length = *(volatile uint32_t *)(0xA00B0014);    
    hal_spi_slave_send(SPIM_TEST_PORT, g_tx_tansfer_buffer, txdata_length);                
    
    // Wait SPI master to send read data command.
    //printf( "SPI Slave is waiting for RD command\r\n");
    while (spi_slave_read_data_flag == false) {
        if (spi_slave_read_error_flag == true) {
            log_hal_error("SPI Slave detect RD error\r\n");  
            break;
        }
    }       	
    
    // Reset SPI slaver status
    spi_slave_read_data_flag = false;
}


int wfc_get_data(T_WFC_OPCODE  *op)
{
    //uint16_t *opcode;
    
    // Wait SPI master to send config write command.
    while (spi_slave_cfg_write_flag == false) hal_gpt_delay_us(0);

    // Reset CWR_FINISH flag
    spi_slave_cfg_write_flag = false; 

    // Prepare Buffer
    rxdata_length = *(volatile uint32_t *)(0xA00B0014);
    //printf("rxdata_length:%d\r\n",rxdata_length);
        
  	op->rxsize = rxdata_length;
   	op->rxptr  = wifi_os_malloc(op->rxsize+1);
   	//memset(op->rxptr, 0, op->rxsize+1);
    
    // Start to receive rx data
    hal_spi_slave_receive(SPIM_TEST_PORT, op->rxptr, op->rxsize);

    // Wait SPI master to send write data command.
    //printf( "SPI Slave is waiting for WR command\r\n");
    while (spi_slave_write_data_flag == false) {
        if (spi_slave_write_error_flag == true) {
            //printf( "Slave detect WR command error\r\n");
            op->opcode = WFC_INVALID_OPCODE;
            return -1;
        }
    }
                         
    // Reset SPI slaver status
    spi_slave_write_data_flag = false;
  
    // Dump data received (Debug)
    //printf("\r\n[Dump]: Data Length == %d\r\n",rxdata_length);         
    //for (int i = 0; i < rxdata_length; i++)
    //    printf("0x%02x ", op->rxptr[i]);    
    //printf("\r\n\r\n\r\n");             
         
    return 0;      
}


void wfc_send_data(T_WFC_OPCODE *pwfc, uint8_t *txptr, uint32_t txsize)
{
    // Init SPI tx transfer buffer
//    memset(g_tx_tansfer_buffer,0,sizeof(g_tx_tansfer_buffer));     

    // Set DTH data response code into SPI response
    memcpy( &g_tx_tansfer_buffer[0], txptr, txsize);
        	
    // Copy response content into tx transfer buffer
    pwfc->txptr = &g_tx_tansfer_buffer[0];

    // Wait SPI master to send config read command.
    //printf( "SPI Slave is waiting for CR command\r\n");
    while (spi_slave_cfg_read_flag == false) hal_gpt_delay_us(0);

    // Reset CRD_FINISH flag
    spi_slave_cfg_read_flag = false;
   
    // Send Tx Data
    hal_spi_slave_send(SPIM_TEST_PORT, pwfc->txptr, txsize);                
    
    // Wait SPI master to send read data command.
    //printf( "SPI Slave is waiting for RD command\r\n");
    while (spi_slave_read_data_flag == false) {
        if (spi_slave_read_error_flag == true) {
            log_hal_error("SPI Slave detect RD error\r\n");  
            break;
        }
    }       	
                          
    // Reset SPI slaver status
    spi_slave_read_data_flag = false;
}

