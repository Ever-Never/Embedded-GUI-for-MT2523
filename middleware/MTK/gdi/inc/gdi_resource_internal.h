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

#ifndef __GDI_RESOURCE_INTERNAL_H__
#define __GDI_RESOURCE_INTERNAL_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "memory_attribute.h"

#define GDI_RESOURCE_4_BYTE_ALIGN ATTR_4BYTE_ALIGN

typedef struct {
    uint8_t *image; /* Customizable Image filenames. */
} gdi_resource_custom_image_t;
#if 0
typedef struct ImageNameList {
    int8_t filename[MAX_FILENAME_LEN];
} IMAGENAME_LIST;
#endif
/* Custom_ImageMap is the mapping array btw image_id & ImageNum. */
typedef struct {
    uint16_t image_number;  /* Index of str in Custom_Image. */
} gdi_resource_custom_image_map_t;

typedef struct {
    uint16_t minimum_image_id; /* Minimum image_id defined by the developer. */
    uint16_t maximum_image_id; /* Maximum image_id defined by the developer. */
    uint16_t index; 	 /* Index of str in Custom_Image. */
} gdi_resource_custom_image_search_map_t;

/* Custom_String is the typedef array of customizable strings. */
typedef uint8_t gdi_resource_custom_string_t;

/* Custom_StringMap is the mapping array btw string_id & StringNum. */

typedef uint32_t gdi_resource_custom_string_map_t;
typedef uint16_t gdi_resource_custom_string_map_16_t;

typedef struct {
    uint16_t minimum_string_id;  /* Minimum StringID defined by the developer. */
    uint16_t maximum_string_id;  /* Maximum StringID defined by the developer. */
    uint16_t map_index;  /* Index of str in Custom_String_MAP. */
} gdi_resource_string_map_search_t;


typedef struct {
    gdi_resource_custom_string_t *string_resource;
    uint16_t total_string_resource;
    void *string_map;
    uint16_t total_string_map; /* for dup str res entry, total_string_map stores the high 16 bits of max offset to non dup strings*/
    gdi_resource_string_map_search_t *string_map_search;
    uint16_t total_string_map_search; /* for dup str res entry, total_string_map_search stores the low 16 bits of max offset to non dup strings */
    bool is16bit;
} gdi_resource_string_resource_list_t;


/**
 * @brief	   This function is used to get image raw data by image id.
 * @param[in] image_id    is the image id which add by resource generator tool flow.
 * @return    uint8_t*        is the image raw data.
 */
uint8_t *gdi_resource_get_image_data(uint16_t image_id);

/**
 * @brief 	This function is used to get string raw data by string id.
 * @param[in] string_id    is the string id which add by resource generator tool flow.
 * @return	uint8_t*	   is the string raw data.
 */
uint8_t *gdi_resource_get_string_data(uint16_t string_id);

#endif /* __GDI_RESOURCE_INTERNAL_H__ */
