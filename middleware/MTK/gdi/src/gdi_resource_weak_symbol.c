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
 
#include "gdi_resource_internal.h"
#include "gdi_font_resource.h"
#include "gdi_font_engine.h"

const gdi_resource_custom_image_t gdi_resource_custom_image_names_default[] = {{0}};
uint16_t gdi_resource_current_max_search_image_id_default = 0;
const gdi_resource_custom_image_search_map_t gdi_resource_image_id_search_map_default[] = {{0}};
const gdi_resource_custom_image_map_t gdi_resource_custom_image_id_map_default[] = {{0}};
const gdi_resource_string_map_search_t gdi_resource_string_map_search_default[] = {{0}};
const uint16_t gdi_resource_translation_string_count_default = 0;
const gdi_resource_custom_string_t gdi_resource_string_resource_fixed_default[] = {0};
const gdi_resource_string_resource_list_t gdi_resource_string_list_default[2] = {{0}};

#if _MSC_VER >= 1500
    #pragma comment(linker, "/alternatename:_gdi_resource_custom_image_names=_gdi_resource_custom_image_names_default")
    #pragma comment(linker, "/alternatename:_gdi_resource_current_max_search_image_id=_gdi_resource_current_max_search_image_id_default")
    #pragma comment(linker, "/alternatename:_gdi_resource_image_id_search_map=_gdi_resource_image_id_search_map_default")
    #pragma comment(linker, "/alternatename:_gdi_resource_custom_image_id_map=_gdi_resource_custom_image_id_map_default")
    #pragma comment(linker, "/alternatename:_gdi_resource_string_map_search=_gdi_resource_string_map_search_default")
    #pragma comment(linker, "/alternatename:_gdi_resource_translation_string_count=_gdi_resource_translation_string_count_default")
    #pragma comment(linker, "/alternatename:_gdi_resource_string_resource_fixed=_gdi_resource_string_resource_fixed_default")
    #pragma comment(linker, "/alternatename:_gdi_resource_string_list=_gdi_resource_string_list_default")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
    #pragma weak gdi_resource_custom_image_names = gdi_resource_custom_image_names_default
    #pragma weak gdi_resource_current_max_search_image_id = gdi_resource_current_max_search_image_id_default 
    #pragma weak gdi_resource_image_id_search_map = gdi_resource_image_id_search_map_default
    #pragma weak gdi_resource_custom_image_id_map = gdi_resource_custom_image_id_map_default        
    #pragma weak gdi_resource_string_map_search = gdi_resource_string_map_search_default
    #pragma weak gdi_resource_translation_string_count = gdi_resource_translation_string_count_default 
    #pragma weak gdi_resource_string_resource_fixed = gdi_resource_string_resource_fixed_default
    #pragma weak gdi_resource_string_list = gdi_resource_string_list_default
#else
    #error "Unsupported Platform"
#endif

const uint8_t proprietaryfont__Width[1]= {
0xF ,};

const uint32_t proprietaryfont__Offset[2]= {
0x0000,0x001B,};

const uint16_t proprietaryfont__RangeOffset[1]= {
0,};

const uint8_t  proprietaryfont__Data[27]= {
0x0,0x0,0x0,0x0,0xFE,0x3,0x1,0x81,0x80,0x40,0x40,0x20,0x20,0x10,0x10,0x8,
0x8,0x4,0x4,0x2,0x2,0xFF,0x1,0x0,0x0,0x0,0x0,};

const RangeData proprietaryfont__RangeData[1]={
{65515,65515},};

const RangeDetails proprietaryfont__RangeInfo={
1,
proprietaryfont__RangeData
};

const sCustFontData proprietaryfont_small = {
14, 14,
11, 3,
0, 27, 500,
(uint8_t*)proprietaryfont__Width, (uint8_t*)proprietaryfont__Width, (uint32_t*)proprietaryfont__Offset, (uint8_t*)proprietaryfont__Data, (uint16_t*)proprietaryfont__RangeOffset,
&proprietaryfont__RangeInfo,
0X00000000,
};

const sCustFontData * const g_small_font_data_array_default[] = {
&proprietaryfont_small
};

const font_group_struct g_fontfamily_default[3] = {
{1, g_small_font_data_array_default},
{1, g_small_font_data_array_default},
{1, g_small_font_data_array_default},
};
const gdi_font_engine_language_t mtk_gLanguageArray_default[] =
{
{
 {(char)0x45,(char)0x0,(char)0x6E,(char)0x0,(char)0x67,(char)0x0,(char)0x6C,(char)0x0,(char)0x69,(char)0x0,(char)0x73,(char)0x0,(char)0x68,(char)0x0,(char)0x00,(char)0x00},//Language Name
  "*#0044#",//SSC string name
  "en-US","eng"
}
};
const uint16_t mtk_gMaxDeployedLangs_default = 0;

#if _MSC_VER >= 1500
    #pragma comment(linker, "/alternatename:_mtk_gLanguageArray = mtk_gLanguageArray_default")
    #pragma comment(linker, "/alternatename:_g_fontfamily = g_fontfamily_default")
    #pragma comment(linker, "/alternatename:_mtk_gMaxDeployedLangs = mtk_gMaxDeployedLangs_default")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
    #pragma weak mtk_gLanguageArray = mtk_gLanguageArray_default
    #pragma weak g_fontfamily = g_fontfamily_default
    #pragma weak mtk_gMaxDeployedLangs = mtk_gMaxDeployedLangs_default
#else
    #error "Unsupported Platform"
#endif

