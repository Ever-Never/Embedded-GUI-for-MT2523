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

/* hal includes */
#include "memory_attribute.h"
#include "task.h"
#include "queue.h"
#include "sys_init.h"

#include "wifi_host_api.h"
#include "wifi_os_api.h"
#include "wfcm_sdio.h"

#include "lwip/pbuf.h"

#include "hal_gpt.h"
#include "hal_gpio_internal.h"
#include "hal_gpio.h"

#include "hal_eint.h"


#ifndef WFC_HRX_POLLING
#if (PRODUCT_VERSION == 2523)
#define  WFC_HRX_EINT           5 //EINT5, HAL_GPIO_6
//#define  WFC_HRX_EINT           19 //EINT19, HAL_GPIO_23
#else //For MT7686
#define  WFC_HRX_EINT           HAL_GPIO_17
#endif
#endif


#define WFC_SAMPLE_AP           "CastroAP"

#define MAX_CMD_RSP_LENGTH          256
#define MAX_PARAM_LENGTH            256

#define WFCM_STUB_TASK_NAME        "wfm"
#define WFCM_STUB_TASK_STACK_SIZE   512
#define WFCM_STUB_TASK_PRIORITY       6

#define WFCM_WIFI_CONF_TASK_NAME         "wfc"
#define WFCM_WIFI_CONF_TASK_STACK_SIZE    512
#define WFCM_WIFI_CONF_TASK_PRIORITY        6

#define WFCM_EVT_TASK_NAME         "wfv"
#define WFCM_EVT_TASK_STACK_SIZE    512
#define WFCM_EVT_TASK_PRIORITY        6

#define WFCM_MQ_TASK_NAME                "wfm"
#define WFCM_MQ_TASK_STACK_SIZE           2048
#define WFCM_MQ_TASK_PRIORITY                6
#define WFCM_MQ_SIZE                       100

#define WFCM_QUEUE_SIZE                      20


static TaskHandle_t  wfcm_stub_task_handle = NULL;
#if 0
static TaskHandle_t  wfcm_wifi_conf_handle = NULL;
#endif
static TaskHandle_t  wfcm_evt_handle        = NULL;
static QueueHandle_t wfcm_queue             = NULL;
static QueueHandle_t wfcm_evt_queue         = NULL;


#ifdef WFC_HRX_POLLING
int g_wfcm_standby_time = 6;
#endif

os_semaphore_t g_wfcm_sem_rsp = NULL;

extern const wifi_event_handler_t wifi_rx_event_handler[];
bool wfcm_sdio_check_init(void);

//true - no wait response, false - need wait response from slave
T_WFC_CONFIG g_wfcm_stub_opcode[] =
{
	//WFC System Cmd
    {WFC_CT_SYSTEM, WFC_CMD_DTH_READY,         false},   //Need wait HRX data size
    {WFC_CT_SYSTEM, WFC_CMD_DTW_DATA,          false},   //No Response Need

    //WiFi Configure Cmd
    {WFC_CT_USER, WFC_CMD_WFC_READY,          false},

    //OS Cmd
    {WFC_CT_USER, WFC_CMD_IP_READY,            false},
    {WFC_CT_USER, WFC_CMD_OS_INFO,             true},

    //Platform Cmd
    //{WFC_CT_USER, WFC_CMD_REMOTE_IPERF,        true},
    //{WFC_CT_USER, WFC_CMD_REMOTE_IPERF_CLIENT, true},

    //AT Cmd
    #ifdef MTK_ATCI_ENABLE
    {WFC_CT_USER, WFC_CMD_AT,                  false},
    #endif


    {WFC_CT_USER, WFC_CMD_API,                  false},

    {WFC_CT_USER, WFC_CMD_MAX_SUPPORT,         true},
};

int32_t wifi_station_port_secure_event_handler(uint32_t event, uint8_t *payload, uint32_t length);
int32_t wifi_station_disconnected_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);

struct netif *netif_find_by_type(uint32_t netif_type);

#if MTK_WIFI_STUB_CONF_SPI_ENABLE
    #define FLASH_MT5932_BIN_ADDRESS  (0x08210000)
    #define FLASH_MT5932_BIN_SIZE     (378*1024)
    bool xboot_over_spi_fw_download(uint8_t *address, uint32_t length);
#else
    #define FLASH_MT5932_BIN_ADDRESS  (0x08210000)
    #define FLASH_MT5932_BIN_SIZE     (378*1024)
    bool xboot_over_sdio_fw_download(uint8_t *address, uint32_t length);
#endif

#if (PRODUCT_VERSION == 2523)
void mt5932_low_level_input(struct netif *netif, struct pbuf *p);
#else
void ethernetif_intr_enhance_mode_dispatch(struct pbuf *p, struct netif *netif);
#endif

uint16_t wfcm_evt_send(uint16_t evt_id, uint8_t *ptr, uint32_t ptrSize, uint8_t fromISR);

#if (PRODUCT_VERSION == 2523)
typedef SemaphoreHandle_t   os_semaphore_t;

os_semaphore_t wifi_os_semphr_create_binary(void)
{
    return (os_semaphore_t) xSemaphoreCreateBinary();
}

int32_t wifi_os_semphr_take(os_semaphore_t semphr, struct timeval *wait_time)
{
    assert(semphr);

    TickType_t ticks;

    if(wait_time == NULL){
        ticks = portMAX_DELAY;
    }else{
        ticks = (wait_time->tv_sec * 1000 + wait_time->tv_usec / 1000) / portTICK_PERIOD_MS;
    }

    if(pdTRUE == xSemaphoreTake((SemaphoreHandle_t)semphr, ticks))
        return 0;

    return -1;
}

int32_t wifi_os_semphr_give(os_semaphore_t semphr)
{
    assert(semphr);

    if(pdTRUE == xSemaphoreGive((SemaphoreHandle_t)semphr))
        return 0;

    return -1;
}

#endif

T_WFC_CONFIG *wfcm_get_config(uint16_t opcode)
{
    int            idx = 0;
    T_WFC_CONFIG  *pstub;

    pstub = &g_wfcm_stub_opcode[idx];

    while (pstub->op != WFC_CMD_MAX_SUPPORT)
    {
        if (pstub->op == opcode )
        {
            return pstub;
        }
        pstub = &g_wfcm_stub_opcode[++idx];
    }

    return NULL;
}

int wfcm_if_init(void)
{
	#if MTK_WIFI_STUB_CONF_SPI_ENABLE
	    return wfcm_if_spi_init();
	#else
	    return wfcm_if_sdio_init();
	#endif
}


void wfcm_if_reinit(void)
{
	#if MTK_WIFI_STUB_CONF_SPI_ENABLE
	    wfcm_if_spi_reinit();
	#else
	    wfcm_if_sdio_reinit();
	#endif
}


void wfcm_if_deinit(void)
{
	#if MTK_WIFI_STUB_CONF_SPI_ENABLE
	    wfcm_if_spi_deinit();
	#else
	    wfcm_if_sdio_deinit();
	#endif
}

void wfcm_fw_download(void)
{
	#if MTK_WIFI_STUB_CONF_SPI_ENABLE
	    xboot_over_spi_fw_download((uint8_t *)FLASH_MT5932_BIN_ADDRESS, FLASH_MT5932_BIN_SIZE);
	#else
	    xboot_over_sdio_fw_download((uint8_t *)FLASH_MT5932_BIN_ADDRESS, FLASH_MT5932_BIN_SIZE);
	#endif
}


uint8_t *wfcm_hrx_data_alloc(uint32_t bufSize)
{
    //return pbuf_alloc(PBUF_RAW, bufSize, PBUF_RAM);
    return (uint8_t*)pbuf_alloc(PBUF_RAW, bufSize, PBUF_POOL);
}


uint8_t *wfcm_hrx_data_buffer(uint8_t *pbuf)
{
    return (pbuf)?((struct pbuf *)pbuf)->payload:NULL;
}


void wfcm_hrx_handler(uint8_t *pbuf, uint32_t bufSize)
{
    struct pbuf  *p;
    struct netif *sta_if;

    uint32_t    *pmagic_s;
    uint32_t    *pmagic_e;

    if (!pbuf)
    {
        printf("[HRX]: Null Packet Received!\r\n");
        return;
    }

    p = (struct pbuf *)pbuf;

    //Check if Event Packet or Data Packet
    pmagic_s = (uint32_t*)p->payload;
    if ( (*pmagic_s) == WFC_MAGIC_NUM )
    {
        pmagic_e = (uint32_t*)(p->payload + bufSize - WFC_MAGIC_LEN);
        if ((*pmagic_e) == WFC_MAGIC_NUM)
        {
            uint8_t  *ptr, *param=NULL;
            uint16_t *peid;
            uint32_t  ptrSize;

            ptr = (uint8_t *)(p->payload+WFC_MAGIC_LEN);
            peid = (uint16_t*)ptr;
            ptr += sizeof(uint16_t);
            ptrSize = bufSize - WFC_MAGIC_LEN*2 - sizeof(uint16_t);
            if (ptrSize)
            {
                //Allocate Event Parameter, Need to be free by event handler
                param = (uint8_t*)wifi_os_malloc(ptrSize+1);
                if (param == NULL)
                {
                    //printf("malloc fail!\r\n");
                }
                //printf("Event alloc buf:0x%x, size=%d\n",  param,  ptrSize+1);
                memcpy(param, ptr, ptrSize);
                param[ptrSize] = 0;
            }
            //printf("Eid(%u), param(%p), size(%u)...%s-%d\r\n",*peid, param, ptrSize,__FUNCTION__,__LINE__);
            //wfcm_dump_buf(p->payload+WFC_MAGIC_LEN, bufSize-(WFC_MAGIC_LEN*2));

            // Send Event to Event Queue
            /* The param will be freed in wfcm_evt_task() while deal with the peid*/
            if(wfcm_evt_send(*peid, param, ptrSize, 0) != WFC_RSP_OK){
                if(param != NULL){
                    wifi_os_free(param);
                    param = NULL;
                }
            }

            //Free SDIO received Buffer
            pbuf_free(p);
            return;
        }
    }

    sta_if = netif_find_by_type(2);  //NETIF_TYPE_STA = 2

    //printf("lwip <- S(%d)\r\n", bufSize);
    //wfcm_dump_buf(p->payload, bufSize);

    mt5932_low_level_input(sta_if, p);
    //pbuf_free(p);
}


uint16_t wfcm_send_cmd_to_slave(T_WFC_OPCODE *pwfc, uint8_t qbuf_release)
{
    uint16_t      *op;
    uint16_t       rsp;
    uint32_t       rspSize;
    uint8_t        rspbuf[MAX_CMD_RSP_LENGTH];
    T_WFC_CONFIG  *pwfg;

    pwfg    = pwfc->pwfg;
    op      = (uint16_t*)&rspbuf[0];
    rspSize = MAX_CMD_RSP_LENGTH;

    memset(rspbuf, 0, MAX_CMD_RSP_LENGTH);

    // Send Host Command
    wfcm_sdio_send_data(&(pwfc->qbuf), qbuf_release);

    // Receive Command Response
    wfcm_sdio_receive_cmdrsp(rspbuf, &rspSize);

    // Get Response Code
    rsp = *op;

    if ( !pwfg->auto_rsp )
    {
    	// Copy Parameter Value
        memcpy(pwfc->rxptr, &rspbuf[2], rspSize-2);
        *(pwfc->rspsize) = rspSize-2;
        //wfcm_dump_buf(rspbuf, rspSize);
    }

    return rsp;
}


uint16_t wfcm_send_data_to_slave(T_WFC_QBUF *qbuf)
{
    wfcm_sdio_send_data(qbuf, true);

    return WFC_RSP_OK;
}


uint16_t wfcm_to_slave(uint16_t op, uint8_t *ptr, uint32_t ptrSize, uint8_t *rsp, uint32_t *rspSize, uint8_t fromISR)
{
    uint16_t      rspcode = WFC_RSP_OK;
    T_WFC_OPCODE  wfcm;
    T_WFC_CONFIG *pwfg;

    if (!wfcm_queue || !g_wfcm_sem_rsp)
    {
    	printf("[WFCM]: To Slave Fail - %u, %u\r\n", (unsigned int)wfcm_queue, (unsigned int)g_wfcm_sem_rsp);
    	return WFC_RSP_QUEUE_ERR;
    }

    // Get wifi configure opcode information
    pwfg = wfcm_get_config(op);
    if (!pwfg)
    {
    	return WFC_RSP_INVALID_OPCODE;
    }

    memset(&wfcm, 0, sizeof(T_WFC_OPCODE));

    wfcm.opcode  = op;
    wfcm.txptr   = ptr;
    wfcm.txsize  = ptrSize;
    wfcm.rxptr   = rsp;
    wfcm.rspsize = rspSize;
    wfcm.pwfg    = pwfg;
    wfcm.rspcode = &rspcode;

    // Get Free HTX Mem Queue Buffer (Token - Allow To Send)
    wfcm.qbuf.ctrl = MEMQ_CTRL_HOST_TX;
    if (fromISR)
    {
        wfcm.qbuf.qidx = WFC_MEMQ_INVALID;
        if ( xQueueSendFromISR(wfcm_queue, &wfcm, 0) != pdPASS )
        {
            return WFC_RSP_QUEUE_ERR;
        }
    }
    else
    {
        wfcm_sdio_memq_get_qbuf(&wfcm.qbuf);
        if ( op == WFC_CMD_DTW_DATA)
        {
            // Copy Host TX data into HTX buffer
            wfcm_sdio_htx_cp(WFC_CMD_DTW_DATA, &(wfcm.qbuf), ptr, ptrSize); //Copy Host TX Data to SDIO TX Buffer,
        }
        else
        {
            // Copy Cmd Parameter into HTX buffer
            wfcm_sdio_cmd_cp(op, &wfcm.qbuf, ptr, ptrSize, 0);
        }

        // Send to WFC Queue
        if ( xQueueSend(wfcm_queue, &wfcm, 0) != pdPASS )
        {
            return WFC_RSP_QUEUE_ERR;
        }

        // Send WiFi Configure Response Code (Response No Wait)
        if ( !pwfg->auto_rsp && (op!=WFC_CMD_DTH_READY) && (op!=WFC_CMD_DTW_DATA) )
        {
            wifi_os_semphr_take(g_wfcm_sem_rsp, NULL);
            return rspcode;
        }
    }
    return WFC_RSP_OK;
}


void wfcm_data_free(struct pbuf *p)
{
	pbuf_free(p);
}


void wfcm_queue_handler(void)
{
    uint8_t       bExit = false;
    uint8_t       rspbuf[MAX_CMD_RSP_LENGTH];
    uint32_t      rspSize;

    T_WFC_OPCODE  wfcq;
    T_WFC_CONFIG *pwfg;

    wfcm_queue = xQueueCreate(WFCM_QUEUE_SIZE, sizeof(T_WFC_OPCODE));
    if (wfcm_queue==NULL)
    {
        //printf( "<<ERROR>> :: wfcm_queue create fail!\r\n");
        vTaskDelete(NULL);
        return;
    }

    g_wfcm_sem_rsp = wifi_os_semphr_create_binary();

    memset(rspbuf, 0, sizeof(rspbuf));

    while (!bExit)
    {
    #ifdef WFC_HRX_POLLING
        if (xQueueReceive(wfcm_queue, &wfcq, g_wfcm_standby_time/portTICK_PERIOD_MS))
    #else
        if (xQueueReceive(wfcm_queue, &wfcq, portMAX_DELAY))
    #endif
        {
            //printf("<WFCM>: opcode(%d)\r\n", wfcq.opcode);
            pwfg = wfcq.pwfg;

            if (pwfg->type == WFC_CT_SYSTEM)
            {
                // OpCode Process
                switch (wfcq.opcode)
                {
                	//WFC System Cmd
                    case WFC_CMD_DTW_DATA:
                    {
                        // Send Host TX Data to Slave
                        wfcm_send_data_to_slave(&(wfcq.qbuf));
                        break;
                    }
                    case WFC_CMD_DTH_READY:
                    {
                    	uint8_t    *p;
                    	uint16_t    rsp;
                    	uint32_t    hrxSize;

                        wfcm_sdio_cmd_cp(WFC_CMD_DTH_READY, &wfcq.qbuf, NULL, 0, 1);
                        //Assign Response Buffer Size
                        wfcq.rxptr   = rspbuf;
                        wfcq.rspsize = &rspSize;

                 #ifdef WFC_HRX_POLLING
                        rsp = wfcm_send_cmd_to_slave(&wfcq, false);
                 #else
                        rsp = wfcm_send_cmd_to_slave(&wfcq, true);
                 #endif

                        if (rsp == WFC_RSP_NO_HRX)   // No HRX Data for Host
                        {
                            //Free HTX Buffer
                            #ifdef WFC_HRX_POLLING
                            wfcm_mq_free(&(wfcq.qbuf));
                            #endif
                            //printf("No HRX Data\r\n");
                            break;
                        }

                        #ifdef WFC_HRX_POLLING
                        // Request for next HRX data until receive WFC_RSP_NO_HRX response. Use original HRX qbuf
                        xQueueSend(wfcm_queue, &wfcq, 0); //wfcm_to_slave(WFC_CMD_DTH_READY, NULL, 0, NULL, NULL, 1);   //Rx Interrupt
                        #endif

                        //Get HRX Data Size
                        hrxSize = *((uint32_t *)&rspbuf[0]);
                        //printf("[HRX]:Size(%u)\r\n", hrxSize);
                        if (!hrxSize)
                        {
                            printf("[Error]: Get HRX Size Fail\r\n");
                            break;
                        }

                        //Allocate Data Buffer (pbuf/mem)
                        p = wfcm_hrx_data_alloc(hrxSize);
                        if (!p)
                        {
                            printf("[WFCM]: Data Buffer Allocate Fail!\r\n");
                            break;
                        }

                        //Get HRX Data
                        wfcm_sdio_receive_data((uint8_t*)wfcm_hrx_data_buffer(p), hrxSize);
                        //HRX Receiver
                        wfcm_hrx_handler(p, hrxSize);
                        break;
                    }
                    default:
                    	break;
                }
            }
            else
            {
            	// Send User Command to Slave
            	*(wfcq.rspcode) = wfcm_send_cmd_to_slave(&wfcq, true);
            	if (!pwfg->auto_rsp)
            	{
            		wifi_os_semphr_give(g_wfcm_sem_rsp);
            	}
            }
        }
        #ifdef WFC_HRX_POLLING
        else
        {
            wfcm_to_slave(WFC_CMD_DTH_READY, NULL, 0, NULL, NULL, 0);
        }
        #endif
    }

    vTaskDelete(NULL);
}


#ifndef WFC_HRX_POLLING
void eint17_handler(void *parameter)
{
#ifdef HAL_EINT_FEATURE_MASK
    /* Mask EINT */
    hal_eint_mask(WFC_HRX_EINT);
#endif

    wfcm_to_slave(WFC_CMD_DTH_READY, NULL, 0, NULL, NULL, 1);   //Rx Interrupt

#ifdef HAL_EINT_FEATURE_MASK
    /* Please unmask the EINT if you want to recieve EINT interrupt  */
    hal_eint_unmask(WFC_HRX_EINT);
#endif
}
#endif


static void wfcm_stub_task(void)
{
    int  ret;

    /*Download Slave/Device FW*/
    wfcm_fw_download();

    wfcm_if_deinit();

    /*waiting Boot ROM release*/
    wfcm_sdio_check_init();

    ret = wfcm_if_init();
    if (ret)
    {
    	printf("WFCm Interface Init Fail!\r\n");
    	vTaskDelete(NULL);
    	return;
    }

#ifndef WFC_HRX_POLLING
    hal_eint_config_t eint_config;

    //eint_config.trigger_mode = HAL_EINT_EDGE_FALLING;
    eint_config.trigger_mode = HAL_EINT_EDGE_RISING;
    eint_config.debounce_time = 0;

    hal_eint_mask(WFC_HRX_EINT);
    if (HAL_EINT_STATUS_OK !=  hal_eint_init(WFC_HRX_EINT, &eint_config)) {
        // error handle
    }
    if (HAL_EINT_STATUS_OK !=  hal_eint_register_callback(WFC_HRX_EINT, eint17_handler, NULL)) {
        // error handle
    }
    hal_eint_unmask(WFC_HRX_EINT);
#endif

    wfcm_queue_handler();


    vTaskDelete(NULL);
}


#if 0
static void wfcm_wifi_config(void)
{
    uint8_t  is_ready = 0;
    uint8_t  rspbuf[MAX_CMD_RSP_LENGTH];
    uint32_t rspSize  = 0;

    while ((!wfcm_queue || !g_wfcm_sem_rsp ||!wfcm_evt_queue))
    {
        hal_gpt_delay_ms(10);
    }
    memset(rspbuf, 0, MAX_CMD_RSP_LENGTH);

#if !MTK_WFC_WITH_LWIP_NO_WIFI_ENABLE
    // Get Device WFC module Status
    is_ready = 0;
    while (!is_ready)
    {
        wfcm_to_slave(WFC_CMD_WFC_READY, NULL, 0, rspbuf, &rspSize, 0);
        is_ready = rspbuf[0];
        printf("-");
        hal_gpt_delay_ms(100);
    }
    printf("\r\nDevice1 WFC Ready(%d)\r\n", is_ready);

    // Print OS Info
    //printf("SPI Master Print OS Info....\r\n");
    //wfcm_to_slave(WFC_CMD_OS_INFO, NULL, 0, NULL, NULL, 0);
    //hal_gpt_delay_ms(100);
#endif

/*  remove for - it will change to wait slave send event notify
    // Get IP Status
    is_ready = 0;
    while (!is_ready) //IP Connected
    {
   	    wfcm_to_slave(WFC_CMD_IP_READY, NULL, 0, rspbuf, &rspSize, 0);
        is_ready = rspbuf[0];
        printf("|");
        hal_gpt_delay_ms(1000);
    }
    printf("\r\nWiFi Connected\r\n");

    wifi_station_port_secure_event_handler(0, NULL, 0);
*/
    vTaskDelete(NULL);
}
#endif


uint16_t wfcm_evt_send(uint16_t evt_id, uint8_t *ptr, uint32_t ptrSize, uint8_t fromISR)
{
    T_WFC_EVT  evt;

    if (!wfcm_evt_queue)
    {
    	printf("[WFCM]: Event Send Fail - %u\r\n", (unsigned int)wfcm_evt_queue);
    	return WFC_RSP_QUEUE_ERR;
    }

    memset(&evt, 0, sizeof(T_WFC_EVT));

    evt.eid    = evt_id;
    evt.param  = ptr;
    evt.size   = ptrSize;

    // Send to WFC Queue
    if (fromISR)
    {
    	if ( xQueueSendFromISR(wfcm_evt_queue, &evt, 0) != pdPASS )
    		return WFC_RSP_QUEUE_ERR;
    }
    else if ( xQueueSend(wfcm_evt_queue, &evt, 0) != pdPASS )
    {
        return WFC_RSP_QUEUE_ERR;
    }

    return WFC_RSP_OK;
}


static void wfcm_evt_task(void)
{
    uint8_t       bExit = false;
    T_WFC_EVT     evt;

    while ((!wfcm_queue || !g_wfcm_sem_rsp)) {
        hal_gpt_delay_ms(10);
    }

    wfcm_evt_queue = xQueueCreate(WFCM_QUEUE_SIZE, sizeof(T_WFC_EVT));
    if (wfcm_evt_queue == NULL)
    {
        vTaskDelete(NULL);
        return;
    }

    while (!bExit)
    {
        if (xQueueReceive(wfcm_evt_queue, &evt, portMAX_DELAY))
        {
            // Event Process
            switch (evt.eid)
            {
            	//WFC System Cmd
                case WFC_EVT_WIFI_CONNECTED:
                {
                    wifi_station_port_secure_event_handler(WIFI_EVENT_IOT_CONNECTED, NULL, 0);
                    break;
                }
                case WFC_EVT_WIFI_DISCONNECT:
                {
                    wifi_station_disconnected_event_handler(WIFI_EVENT_IOT_DISCONNECTED, NULL, 0);
                    break;
                }
                case WFC_EVT_SCAN_RESULTS:
                {
                    wifi_rx_event_handler[WIFI_EVENT_IOT_SCAN_COMPLETE](WIFI_EVENT_IOT_SCAN_COMPLETE, evt.param, evt.size);
                    break;
                }
                default:
                	break;
            }

            /*JCB0613, free event buffer which alloc in wfcm_hrx_handler()*/
            if(evt.param != NULL){
                //printf("Event free buf:0x%x \n",  evt.param);
                wifi_os_free(evt.param);
                evt.param = NULL;
            }
        }
    }

    vTaskDelete(NULL);
}


void wfcm_stub_init(void)
{
	// WFC Host Task
    if ( xTaskCreate((TaskFunction_t)wfcm_stub_task,
                    WFCM_STUB_TASK_NAME,
                    WFCM_STUB_TASK_STACK_SIZE,
                    NULL,
                    WFCM_STUB_TASK_PRIORITY,
                    &wfcm_stub_task_handle) != pdPASS )
    {
    	//printf( "WFC Task Create Fail!\r\n");
    	return;
    }

#if 0
    if ( xTaskCreate((TaskFunction_t)wfcm_wifi_config,
                    WFCM_WIFI_CONF_TASK_NAME,
                    WFCM_WIFI_CONF_TASK_STACK_SIZE,
                    NULL,
                    WFCM_WIFI_CONF_TASK_PRIORITY,
                    &wfcm_wifi_conf_handle) != pdPASS )
    {
    	//printf( "WFC Task Create Fail!\r\n");
    	return;
    }
#endif

    if ( xTaskCreate((TaskFunction_t)wfcm_evt_task,
                    WFCM_EVT_TASK_NAME,
                    WFCM_EVT_TASK_STACK_SIZE,
                    NULL,
                    WFCM_EVT_TASK_PRIORITY,
                    &wfcm_evt_handle) != pdPASS )
    {
    	//printf( "WFC Task Create Fail!\r\n");
    	return;
    }

	return;
}
