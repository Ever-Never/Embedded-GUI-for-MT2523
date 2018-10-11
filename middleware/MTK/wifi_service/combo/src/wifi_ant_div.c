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

/**
* @file wifi_ant_div.c
*
* WIFI Antenna Diversity Feature
*
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "type_def.h"

#ifdef MTK_ANT_DIV_ENABLE
#include "FreeRTOS.h"
#include "task.h"

#include "type_def.h"
#include "mt7687_cm4_hw_memmap.h"
#include "hal_gpio_7687.h"

#include "nvdm.h"
#include "wifi_ant_div.h"
#include "wifi_inband.h"

int g_gpio_pinmux = 0;

void ant_div_update_pin()
{ 
	DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) = g_gpio_pinmux;
}

#if 0
void ant_div_init_sw(int ant)
{
    /* 2. GPIO Setting */
    /*(1) AON 0x81023020[3:0] = 8 , 0x81023020[27:24] = 8*/    
    HAL_REG_32(0x81023020) &= 0xF0FFFFF0;
    HAL_REG_32(0x81023020) |= 0x08000078;
    printf("write HAL_REG_32(0x81023020) done !\n");

    /* ant = 0 : GPIO0, ant=1: GPIO6 */
    printf("ant_div init to => ant%d\n", ant);
	
    /* set GPIO 0 = HI */
    int gpio1 = (ant == 0 ? 0: 6), gpio2 = (ant == 0 ? 6 : 0);
    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~(0xF << (gpio1 * 4));
    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) &= ~(0xF << (gpio2 * 4));
    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |= (0x8 << (gpio1 * 4));
    DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20) |=   (0x8 << (gpio2 * 4));
    g_gpio_pinmux=DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20);
    /* Output Enable */
    DRV_Reg32(IOT_GPIO_AON_BASE + 0x90) |= (1 << gpio1);
    DRV_Reg32(IOT_GPIO_AON_BASE + 0x90) |= (1 << gpio2);
    /* Set output polarity */
    DRV_Reg32(IOT_GPIO_AON_BASE + 0x60) |= (1 << gpio1);	
    DRV_Reg32(IOT_GPIO_AON_BASE + 0x60) &= ~(1 << gpio2);	

    printf("gpio(%d, %d), pinmux=0x%x, output enable=0x%x, polarity=0x%x\n", gpio1, gpio2, 
		DRV_Reg32(IOT_GPIO_PINMUX_AON_BASE + 0x20), DRV_Reg32(IOT_GPIO_AON_BASE + 0x90), DRV_Reg32(IOT_GPIO_AON_BASE + 0x60));
}
#endif

void ant_diversity_init(int ant)
{
/*
    wr 0x80000108 0x4000
    wr 0x81023020 0x80887770
    wr 0x80025100 0x01000001
    wr 0x810600e0 0x03020011
    wr 0x810600e4 0x06100704
    wr 0x60200f10 0x00000008
    wr 0x60200084 0x00100000
    wr 0x60200514 0x0010000c
*/
    
    /* 1. TOP Clock 0x80000108[14] = 1 */    
    HAL_REG_32(0x80000108) |= 0x4000;
    //HAL_REG_32(0x80000108) = 0x4000;
    printf("write HAL_REG_32(0x80000108)=0x%x done !\n", HAL_REG_32(0x80000108));

    /* 2. GPIO Setting */
#if 0
    //Mask to fix the issue: AMZN P2 board auto reboot after bootup. 
    /*(1) AON 0x81023020[3:0] = 0 , 0x81023020[27:24] = 0*/    
    HAL_REG_32(0x81023020) &= 0xF0FFFFF0;
    HAL_REG_32(0x81023020) |= 0x00000070;
    //HAL_REG_32(0x81023020) = 0x80887770;
    printf("write HAL_REG_32(0x81023020)=0x%x done !\n", HAL_REG_32(0x81023020));
#endif

    /*(2) TOP OFF 0x80025100[3:0] = 1, 0x80025100[27:24] = 1*/
    HAL_REG_32(0x80025100) &= 0xF0FFFFF0;    
    HAL_REG_32(0x80025100) |= 0x01000001;
    //HAL_REG_32(0x80025100) = 0x01000001;
    printf("write HAL_REG_32(0x80025100)=0x%x done !\n", HAL_REG_32(0x80025100));
    
    /* 3. PTA Setting */    
    /* (1) enable 0x8106000[0] = 1, (2) GPIO-0 0x810600E0[7:0] = 0x11, GPIO-6 0x810600E4[23:16] = 0x10*/    
    /*  PTA ant sel mode: 
         0
         ...
         13 = wifi_ant_id[0]	
         14 = ~wifi_ant_id[0]	
         15 = 	
         16 = (dig_wf0_ant_sel_out & phy_rf_sw_r_out) | (wifi_ant_id[0] & phy_rf_sw_t_out)
         17 = ~((dig_wf0_ant_sel_out & phy_rf_sw_r_out) | (wifi_ant_id[0] & phy_rf_sw_t_out))	
         ...
         25
    */         
    HAL_REG_32(0x810600e0) &= 0xFFFFFF00;
    //HAL_REG_32(0x810600e0) |= 0x00000011; // mode = 17 
    HAL_REG_32(0x810600e0) |= 0x0000000E; // mode = 14
    //HAL_REG_32(0x810600e0) = 0x03020011;
    printf("write HAL_REG_32(0x810600e0)=0x%x done !\n", HAL_REG_32(0x810600e0));
    
    HAL_REG_32(0x810600e4) &= 0xFF00FFFF;
    //HAL_REG_32(0x810600e4) |= 0x00100000; // mode = 16 
    HAL_REG_32(0x810600e4) |= 0x000D0000; // mode = 13
    //HAL_REG_32(0x810600e4) = 0x06100704;
    printf("write HAL_REG_32(0x810600e4)=0x%x done !\n", HAL_REG_32(0x810600e4));
#if 0
    /*Fix RX to Main 0x6020_0F10[3] =1*/
    HAL_REG_32(0x60200f10) |= 0x00000008;
    //HAL_REG_32(0x60200f10) = 0x00000008;
    printf("write HAL_REG_32(0x60200f10) done !\n");
#else

    if (ant > 1)
    {
        LOG_E(common, "Invalid antenna number: %d, bypass...", ant);
    }	
    else
        HAL_REG_32(0x81060218) = 0x80000000 | ant; 
#endif	

#if 0
//Remove this CR setting, this will cause RSSI dropped to a low value.
    /*x6020_0084[20] = 1*/    
    HAL_REG_32(0x60200084) |= 0x00100000;
    //HAL_REG_32(0x60200084) = 0x00100000;
    printf("write HAL_REG_32(0x60200084)=0x%x done !\n", HAL_REG_32(0x60200084) );
#endif
    /*0X6020_0514 [20]  : set 1 to enable (default 0) ANT Selection*/
    HAL_REG_32(0x60200514) |= 0x00100000;
    //HAL_REG_32(0x60200514) = 0x00100000;
    printf("write HAL_REG_32(0x60200514)=0x%x done !\n", HAL_REG_32(0x60200514));

}

extern int g_supplicant_ready;
void printf_ant_diversity(int test)
{
    printf("%d : *************************************************\n",test);
    UINT32  cr_value = 0;    
    cr_value = HAL_REG_32(0x80000108);
    printf("HAL_REG_32(0x80000108) : %8x\n", cr_value);

    cr_value = HAL_REG_32(0x81023020);
    printf("HAL_REG_32(0x81023020) : %8x\n", cr_value);
    
    cr_value = HAL_REG_32(0x80025100);
    printf("HAL_REG_32(0x80025100) : %8x\n", cr_value);

    cr_value = HAL_REG_32(0x810600e0);
    printf("HAL_REG_32(0x810600e0) : %8x\n", cr_value);

    cr_value = HAL_REG_32(0x810600e4);
    printf("HAL_REG_32(0x810600e4) : %8x\n", cr_value);

    cr_value = HAL_REG_32(0x60200f10);
    printf("HAL_REG_32(0x60200f10) : %8x\n", cr_value);

    cr_value = HAL_REG_32(0x60200084);
    printf("HAL_REG_32(0x60200084) : %8x\n", cr_value);

    cr_value = HAL_REG_32(0x60200514);
    printf("HAL_REG_32(0x60200514) : %8x\n", cr_value);
    printf("***************************************************\n");
}

#if 0
void ant_div_init_task(void *args)
{
    printf("ant_div_init_task start !!!\n");
    while(!g_supplicant_ready){
        vTaskDelay(50);
    }
    //vTaskDelay(3000);
    ant_diversity_init();
    printf_ant_diversity(2);
    vTaskDelete(NULL);
}
#endif

/**
* @brief send inband to trigger N9 send prob request to switch monitor.
* @param
* @return
*/
int32_t wifi_config_set_antenna_monitor(void)
{
    int32_t status = 0;
    uint8_t antenna_diversity_enable = 0;

    uint8_t buff[4];
    uint32_t len = sizeof(buff);

    nvdm_read_data_item("common", "AntDiversityEnable", buff, &len);
    antenna_diversity_enable= (uint8_t)atoi((char *)buff);

    if(antenna_diversity_enable ) {
        status = wifi_inband_antenna_monitor(1);
    } else {
        LOG_E(wifi,"Ant Diversity not Enable");
        status = -1;
    }

    return status;
}

int32_t wifi_config_get_antenna_status(wifi_antenna_status_t *ant_status)
{
    int32_t status = 0;

    if(ant_status != NULL) {
        status = wifi_inband_antenna_status(0, ant_status);
    } else {
        status = -1;
        LOG_E(wifi,"input ant_status is NULL");
    }

    return status;
}

int32_t wifi_config_set_antenna_number(uint8_t antenna_number)
{
    int32_t status = 0;
    wifi_antenna_status_t ant_status = {0};

    ant_status.antenna_number = antenna_number;

    if(ant_status.antenna_number <= 1) {
        status = wifi_inband_antenna_status(1, &ant_status);
    } else {
        status = -1;
        LOG_E(wifi,"input error");
    }

    return status;
}

int32_t wifi_profile_record_antenna_status(wifi_antenna_status_t * ant_status)
{
    int32_t status = 0;
    char nvdm_buf[10] = {0};

    //LOG_I(wifi, "ant_number:%d,pre_selected_rssi:%d\n",ant_status->antenna_number, ant_status->pre_selected_rssi);


    sprintf(nvdm_buf, "%d", ant_status->antenna_number);
    if (NVDM_STATUS_OK != nvdm_write_data_item("common", "AntennaNumber",
                                 NVDM_DATA_ITEM_TYPE_STRING,
                                 (uint8_t *)nvdm_buf, strlen(nvdm_buf))){
        LOG_E(wifi, "write AntennaNumber fail!");
        return -1;
    }

    sprintf(nvdm_buf, "%d", ant_status->pre_selected_rssi);
    if (NVDM_STATUS_OK != nvdm_write_data_item("common", "AntPreSelectedRssi",
                                 NVDM_DATA_ITEM_TYPE_STRING,
                                 (uint8_t *)nvdm_buf, strlen(nvdm_buf))){
        LOG_E(wifi, "write AntPreSelectedRssi fail!");
        return -1;
    }

    return status;
}

#endif /*MTK_ANT_DIV_ENABLE*/

