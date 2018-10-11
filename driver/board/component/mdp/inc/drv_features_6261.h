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
 
 #ifndef DRV_FEATURES_6261_H 
#define DRV_FEATURES_6261_H 

/****************************MT6261  Driver features*******************************/

//#if  defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)

#define    DRV_RTC_6261_SERIES                      

#define    DRV_F32K_6261_SERIES                     

#define    DRV_SIM_6250_SERIES                      
                                            
#define    DRV_LCD_MT6260_SERIES                    

#define    DRV_GPIO_6261_SERIES                     

#define    DRV_GPT_6261_SERIES                      

#define    DRV_I2C_6261_SERIES                      

#define    DRV_PWM_6261_SERIES                      

#define    DRV_WDT_6261_SERIES                      
                                           
#define    DRV_MSDC_6261_SERIES    

#define    DRV_MISC_6261_SERIES                  
                                           
#define    __DRV_GRAPHICS_JPEG_6261_SERIES__        

#define    DRV_ACCDET_6261_SERIES                   

#define    DRV_BMT_6261_SERIES                      

#define    DRV_ISP_6235_SERIES

#define    DRV_ISP_MT6250_HW_SUPPORT 

#define    DRV_IDP_6252_SERIES                      

#define    DRV_MM_POWER_CTRL_MT6250_SERIES                                                   

#define    DRV_USB_6261_SERIES                      

#define    DRV_ADC_6261_SERIES                      

#define   DRV_CHR_DET_6261_SERIES

#define    DRV_KBD_6261_SERIES                      

#define    DRV_TP_6261_SERIES                       

#define    DRV_GCU_6225_SERIES                      

#define    DRV_SPI_6261_SERIES                      

#define    DRV_SPI_SLAVE_6261_SERIES                

#define    DRV_UART_6261_SERIES                     

#define    __DRV_GRAPHICS_RGB_RESIZER_6250_SERIES__ 

#define    __DRV_GRAPHICS_RGB_ROTATOR_6250_SERIES__ 

#define    __DRV_GRAPHICS_YUV_ROTATOR_6252_SERIES__ 

#if defined(G2D_SUPPORT)

  #define    __DRV_GRAPHICS_G2D_6260_SERIES__         

#endif

#define	   __DRV_COLOR_MT6260_SERIES__

//#endif   /*#if  defined(MT6261)*/ 

#endif /*#ifndef DRV_FEATURES_6261_H*/ 


 