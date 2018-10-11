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
#include "stdio.h"
#include "os.h"
#include "syslog.h"
#include "nvdm.h"
#include "hal_efuse.h"
#include "hal_gpt.h"
#include "wifi_host_api.h"
#include "wfc_stub.h"
#include "wfc_at_api.h"

#define RSP_BUF_SIZE 1024
extern uint8_t rsp_buf[RSP_BUF_SIZE];
extern uint32_t rsp_size;

int32_t wifi_host_config_set_mac_address(uint8_t port, uint8_t *address);

int32_t wifi_host_config_init(wifi_host_system_config_t *pwifi_host_cfg)
{
    int32_t ret = -1;
    wh_set_init_para_cmd_para_t cmd_para;
    wh_set_init_para_resp_para_t *rsp_para;

    uint8_t  wfc_is_ready = 0;
    uint8_t  rspbuf[32]={0};  /*WFC_CMD_WFC_READY, */
    uint32_t rspSize  = 0;

    cmd_para.api_id.api = WFC_API_INIT_PARA;

    if(pwifi_host_cfg == NULL) {
        ret = -1;
        return ret;
    }

    os_memcpy(&cmd_para.config, &(pwifi_host_cfg->wifi_config), sizeof(wifi_config_t));
    cmd_para.config_valid = true;

    os_memcpy(&cmd_para.config_ext, &(pwifi_host_cfg->wifi_config_ext), sizeof(wifi_config_ext_t));
    cmd_para.config_ext_valid = true;


    // Get Device WFC module Status
    wfc_is_ready = 0;
    while (!wfc_is_ready) {
        wfcm_to_slave(WFC_CMD_WFC_READY, NULL, 0, rspbuf, &rspSize, 0);
        wfc_is_ready = rspbuf[0];
        printf("-");
        hal_gpt_delay_ms(100);
    }
    printf("\r\nDevice WFC Ready(%d)\r\n", wfc_is_ready);



    /*Send the config to Device,  to do deivce initialization*/
    /*the mac address only can be set here , before doing wifi_host_config_init()*/
    ret = wifi_host_config_set_mac_address(WIFI_PORT_STA, pwifi_host_cfg->wifi_host_adv_config.sta_mac);
    if(ret<0){
        printf("set sta mac fail\n");
    }
    ret = wifi_host_config_set_mac_address(WIFI_PORT_AP, pwifi_host_cfg->wifi_host_adv_config.ap_mac);
    if(ret<0){
        printf("set ap mac fail\n");
    }
    
    // Set Device WiFi Init settings
    if (WFC_RSP_OK != wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0)){
        printf("\r\n wfcm_to_slave fail\r\n");
    } else {
        rsp_para = (wh_set_init_para_resp_para_t *)rsp_buf;
        ret = rsp_para->error_code;
    }

    return ret;
}

int32_t wifi_host_config_get_mac_address(uint8_t port, uint8_t *address)
{
    int32_t ret = 0;
    wh_get_mac_addr_cmd_para_t cmd_para;
    wh_get_mac_addr_resp_para_t *rsp_para;
    
    cmd_para.api_id.api = WFC_API_MAC_ADDR;
    cmd_para.api_id.set = false;
    cmd_para.port = port;

    if(WFC_RSP_OK != wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0)){
        printf("wfcm_to_slave fail in mac opt\r\n");
        ret = -1;
    } else {
        rsp_para = (wh_get_mac_addr_resp_para_t *)rsp_buf;
        ret = rsp_para->error_code;
        os_memcpy(address, rsp_para->mac_addr, WIFI_MAC_ADDRESS_LENGTH);
        
        printf("GET MAC: 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,\n\r",
            address[0],address[1],address[2],
            address[3],address[4],address[5]);
    }
    return ret;
}


int32_t wifi_host_config_set_mac_address(uint8_t port, uint8_t *address)
{
    int32_t ret = -1;
    wh_set_mac_addr_cmd_para_t cmd_para;
    wh_set_mac_addr_resp_para_t *rsp_para;
    
    cmd_para.api_id.api = WFC_API_MAC_ADDR;
    cmd_para.api_id.set = true;
    cmd_para.port = port;
    os_memcpy(cmd_para.mac_addr, address, WIFI_MAC_ADDRESS_LENGTH);
    if(WFC_RSP_OK != wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0)){
        printf("wfcm_to_slave fail in mac opt\r\n");
        ret = -1;
    } else {
        rsp_para = (wh_set_mac_addr_resp_para_t *)rsp_buf;
        ret = rsp_para->error_code;
    }
    
    return ret;
}

