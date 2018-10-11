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

#ifndef __HAL_PINMUX_DEFINE_H__

#define __HAL_PINMUX_DEFINE_H__

#include "hal_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_GPIO_MODULE_ENABLED



#define HAL_GPIO_0_GPIO0    0   
#define HAL_GPIO_0_EINT0    1   
#define HAL_GPIO_0_WIFIRF_DBGOUT0    2  
#define HAL_GPIO_0_U1RTS    3   
#define HAL_GPIO_0_SCL1    4    
#define HAL_GPIO_0_I2S_RX    5  
#define HAL_GPIO_0_JTDI    6    
#define HAL_GPIO_0_WF_JTDI    7 
#define HAL_GPIO_0_WIFI_ANT_SEL0    8   
#define HAL_GPIO_0_BT_PRI1    9 
#define HAL_GPIO_0_PWM0    10   

#define HAL_GPIO_1_GPIO1    0   
#define HAL_GPIO_1_EINT1    1   
#define HAL_GPIO_1_WIFIRF_DBGOUT1    2  
#define HAL_GPIO_1_U1CTS    3   
#define HAL_GPIO_1_SDA1    4    
#define HAL_GPIO_1_I2S_TX    5  
#define HAL_GPIO_1_JTMS    6    
#define HAL_GPIO_1_WF_JTMS    7 
#define HAL_GPIO_1_WIFI_ANT_SEL1    8   
#define HAL_GPIO_1_BT_PRI2    9 
#define HAL_GPIO_1_PWM1    10   

#define HAL_GPIO_2_GPIO2    0   
#define HAL_GPIO_2_EINT2    1   
#define HAL_GPIO_2_WIFIRF_DBGOUT2    2  
#define HAL_GPIO_2_URXD1    3   
#define HAL_GPIO_2_PWM0    4    
#define HAL_GPIO_2_I2S_WS    5  
#define HAL_GPIO_2_JTCK    6    
#define HAL_GPIO_2_CLKO0    7   
#define HAL_GPIO_2_WF_JTCK    8 
#define HAL_GPIO_2_BT_PRI0    9 
#define HAL_GPIO_2_WIFI_ANT_SEL4    10  

#define HAL_GPIO_3_GPIO3    0   
#define HAL_GPIO_3_EINT3    1   
#define HAL_GPIO_3_WIFIRF_DBGOUT3    2  
#define HAL_GPIO_3_UTXD1    3   
#define HAL_GPIO_3_PWM1    4    
#define HAL_GPIO_3_I2S_CK    5  
#define HAL_GPIO_3_JTRST_B    6 
#define HAL_GPIO_3_DEBUGMON0    7   
#define HAL_GPIO_3_WF_JTRST_B    8  
#define HAL_GPIO_3_WIFI_ANT_SEL2    9   

#define HAL_GPIO_4_GPIO4    0   
#define HAL_GPIO_4_SPISLV_A_SIO2    1   
#define HAL_GPIO_4_SPIMST_A_SIO2    2   
#define HAL_GPIO_4_EINT4    3   
#define HAL_GPIO_4_DEBUGMON15    4  
#define HAL_GPIO_4_I2S_MCLK    5    
#define HAL_GPIO_4_JTDO    6    
#define HAL_GPIO_4_DEBUGMON1    7   
#define HAL_GPIO_4_WF_JTDO    8 
#define HAL_GPIO_4_WIFI_ANT_SEL3    9   

#if (PRODUCT_VERSION == 7686)
#define HAL_GPIO_5_GPIO5    0   
#define HAL_GPIO_5_SPISLV_A_SIO3    1   
#define HAL_GPIO_5_SPIMST_A_SIO3    2   
#define HAL_GPIO_5_EINT5    3   
#define HAL_GPIO_5_URXD0    4   
#define HAL_GPIO_5_WIFI_ANT_SEL0    5   
#define HAL_GPIO_5_TDM_RX    6  
#define HAL_GPIO_5_DEBUGMON2    7   
#define HAL_GPIO_5_N9_UTXD3    8    
#define HAL_GPIO_5_SCL0    9    
#define HAL_GPIO_5_PMU_RGU_RSTB    10   

#define HAL_GPIO_6_GPIO6    0   
#define HAL_GPIO_6_SPISLV_A_CS    1 
#define HAL_GPIO_6_SPIMST_A_CS    2 
#define HAL_GPIO_6_EINT6    3   
#define HAL_GPIO_6_UTXD0    4   
#define HAL_GPIO_6_WIFI_ANT_SEL1    5   
#define HAL_GPIO_6_TDM_TX    6  
#define HAL_GPIO_6_DEBUGMON3    7   
#define HAL_GPIO_6_WF_DBGI_N    8   
#define HAL_GPIO_6_SDA0    9        

#define HAL_GPIO_7_GPIO7    0   
#define HAL_GPIO_7_SPISLV_A_SCK    1    
#define HAL_GPIO_7_SPIMST_A_SCK    2    
#define HAL_GPIO_7_EINT7    3   
#define HAL_GPIO_7_CLKO1    4   
#define HAL_GPIO_7_WIFI_ANT_SEL2    5   
#define HAL_GPIO_7_TDM_WS    6  
#define HAL_GPIO_7_DEBUGMON4    7   
#define HAL_GPIO_7_WF_DBGACK_N    8 
#define HAL_GPIO_7_BT_PRI3    9     

#define HAL_GPIO_8_GPIO8    0   
#define HAL_GPIO_8_SPISLV_A_SIO0    1   
#define HAL_GPIO_8_SPIMST_A_SIO0    2   
#define HAL_GPIO_8_EINT8    3   
#define HAL_GPIO_8_SCL0    4    
#define HAL_GPIO_8_U0RTS    5   
#define HAL_GPIO_8_TDM_CK    6  
#define HAL_GPIO_8_DEBUGMON5    7   
#define HAL_GPIO_8_BT_PRI0    8         

#define HAL_GPIO_9_GPIO9    0   
#define HAL_GPIO_9_SPISLV_A_SIO1    1   
#define HAL_GPIO_9_SPIMST_A_SIO1    2   
#define HAL_GPIO_9_EINT9    3   
#define HAL_GPIO_9_SDA0    4    
#define HAL_GPIO_9_U0CTS    5   
#define HAL_GPIO_9_TDM_MCLK    6    
#define HAL_GPIO_9_DEBUGMON6    7   
#define HAL_GPIO_9_WIFI_ANT_SEL3    8   
#define HAL_GPIO_9_BT_PRI1    9     

#define HAL_GPIO_10_GPIO10    0 
#define HAL_GPIO_10_EINT10    1     
#define HAL_GPIO_10_U2CTS    3  
#define HAL_GPIO_10_PWM2    4   
#define HAL_GPIO_10_PMU_RGU_RSTB    5   
#define HAL_GPIO_10_PMU_GOTO_SLEEP    6 
#define HAL_GPIO_10_DEBUGMON7    7  
#define HAL_GPIO_10_WIFI_ANT_SEL4    8  
#define HAL_GPIO_10_BT_PRI2    9        
#endif

#define HAL_GPIO_11_GPIO11    0 
#define HAL_GPIO_11_EINT11    1 
#define HAL_GPIO_11_PWM3    2   
#define HAL_GPIO_11_URXD2    3  
#define HAL_GPIO_11_MA_MC0_CK    4  
#define HAL_GPIO_11_SLV_MC0_CK    5 
#define HAL_GPIO_11_CLKO2    6  
#define HAL_GPIO_11_DEBUGMON8    7  
#define HAL_GPIO_11_WF_AICE_TCKC    8   
#define HAL_GPIO_11_WIFI_ANT_SEL0    9  
#define HAL_GPIO_11_I2S_RX    10    

#define HAL_GPIO_12_GPIO12    0 
#define HAL_GPIO_12_SPISLV_B_SIO3    1  
#define HAL_GPIO_12_SPIMST_B_SIO3    2  
#define HAL_GPIO_12_UTXD2    3  
#define HAL_GPIO_12_MA_MC0_CM0    4 
#define HAL_GPIO_12_SLV_MC0_CM0    5    
#define HAL_GPIO_12_EINT12    6 
#define HAL_GPIO_12_DEBUGMON9    7  
#define HAL_GPIO_12_WF_AICE_TMSC    8   
#define HAL_GPIO_12_WIFI_ANT_SEL1    9  
#define HAL_GPIO_12_I2S_TX    10    

#define HAL_GPIO_13_GPIO13    0 
#define HAL_GPIO_13_SPISLV_B_SIO2    1  
#define HAL_GPIO_13_SPIMST_B_SIO2    2  
#define HAL_GPIO_13_U2RTS    3  
#define HAL_GPIO_13_MA_MC0_DA0    4 
#define HAL_GPIO_13_SLV_MC0_DA0    5    
#define HAL_GPIO_13_CLKO4    6  
#define HAL_GPIO_13_DEBUGMON10    7 
#define HAL_GPIO_13_EINT13    8 
#define HAL_GPIO_13_WF_JTDI    9    
#define HAL_GPIO_13_I2S_WS    10    

#define HAL_GPIO_14_GPIO14    0 
#define HAL_GPIO_14_SPISLV_B_SIO1    1  
#define HAL_GPIO_14_SPIMST_B_SIO1    2  
#define HAL_GPIO_14_TDM_RX    3 
#define HAL_GPIO_14_MA_MC0_DA1    4 
#define HAL_GPIO_14_SLV_MC0_DA1    5    
#define HAL_GPIO_14_PWM4    6   
#define HAL_GPIO_14_DEBUGMON11    7 
#define HAL_GPIO_14_EINT14    8 
#define HAL_GPIO_14_WF_JTMS    9    
#define HAL_GPIO_14_CLKO4    10 

#define HAL_GPIO_15_GPIO15    0 
#define HAL_GPIO_15_SPISLV_B_SIO0    1  
#define HAL_GPIO_15_SPIMST_B_SIO0    2  
#define HAL_GPIO_15_TDM_TX    3 
#define HAL_GPIO_15_MA_MC0_DA2    4 
#define HAL_GPIO_15_SLV_MC0_DA2    5    
#define HAL_GPIO_15_SCL1    6   
#define HAL_GPIO_15_DEBUGMON12    7 
#define HAL_GPIO_15_EINT15    8 
#define HAL_GPIO_15_WF_JTCK    9    
#define HAL_GPIO_15_PWM3    10  

#define HAL_GPIO_16_GPIO16    0 
#define HAL_GPIO_16_SPISLV_B_SCK    1   
#define HAL_GPIO_16_SPIMST_B_SCK    2   
#define HAL_GPIO_16_TDM_WS    3 
#define HAL_GPIO_16_MA_MC0_DA3    4 
#define HAL_GPIO_16_SLV_MC0_DA3    5    
#define HAL_GPIO_16_SDA1    6   
#define HAL_GPIO_16_DEBUGMON13    7 
#define HAL_GPIO_16_EINT16    8 
#define HAL_GPIO_16_WF_JTRST_B    9 
#define HAL_GPIO_16_N9_UTXD3    10  

#define HAL_GPIO_17_GPIO17    0 
#define HAL_GPIO_17_SPISLV_B_CS    1    
#define HAL_GPIO_17_SPIMST_B_CS    2    
#define HAL_GPIO_17_TDM_CK    3 
#define HAL_GPIO_17_PWM5    4   
#define HAL_GPIO_17_CLKO3    5  
#define HAL_GPIO_17_AUXADC0    6    
#define HAL_GPIO_17_DEBUGMON14    7 
#define HAL_GPIO_17_EINT17    8 
#define HAL_GPIO_17_WF_JTDO    9    
#define HAL_GPIO_17_BT_PRI0    10   

#if (PRODUCT_VERSION == 7686)
#define HAL_GPIO_18_GPIO18    0 
#define HAL_GPIO_18_PMU_GOTO_SLEEP    1     
#define HAL_GPIO_18_TDM_MCLK    3   
#define HAL_GPIO_18_CLKO4    4  
#define HAL_GPIO_18_SDA1    5   
#define HAL_GPIO_18_ZCV   6 
#define HAL_GPIO_18_DEBUGMON15    7 
#define HAL_GPIO_18_EINT18    8 
#define HAL_GPIO_18_CLKO3    9  
#define HAL_GPIO_18_PMU_RGU_RSTB    10  
#endif

#if ((PRODUCT_VERSION == 7686)||(PRODUCT_VERSION == 5932))
#define HAL_GPIO_19_GPIO19    0 
#define HAL_GPIO_19_URXD0    1  
#define HAL_GPIO_19_EINT19    2 
#define HAL_GPIO_19_SCL1    3       
#define HAL_GPIO_19_PWM5    5   
#define HAL_GPIO_19_AUXADC2    6    
#define HAL_GPIO_19_WIFI_EXT_CLK    7               

#define HAL_GPIO_20_GPIO20    0 
#define HAL_GPIO_20_UTXD0    1  
#define HAL_GPIO_20_EINT20    2             
#define HAL_GPIO_20_AUXADC3    6                    
#endif

#if (PRODUCT_VERSION == 7682)
#define HAL_GPIO_21_GPIO21    0 
#define HAL_GPIO_21_URXD0    1  
#define HAL_GPIO_21_EINT19    2 
#define HAL_GPIO_21_SCL1    3       
#define HAL_GPIO_21_PWM5    5                       

#define HAL_GPIO_22_GPIO22    0 
#define HAL_GPIO_22_UTXD0    1  
#define HAL_GPIO_22_EINT20    2                                 
#endif  


#ifdef __cplusplus
}
#endif

#endif /*HAL_GPIO_MODULE_ENABLED*/

#endif /*__HAL_PINMUX_DEFINE_H__*/

