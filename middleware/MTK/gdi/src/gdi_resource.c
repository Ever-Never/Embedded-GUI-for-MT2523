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
#include "gdi_font_engine_internal.h"

extern const gdi_resource_custom_image_t gdi_resource_custom_image_names[1];
extern uint16_t gdi_resource_current_max_search_image_id;
extern const gdi_resource_custom_image_search_map_t gdi_resource_image_id_search_map[1];
extern const gdi_resource_custom_image_map_t gdi_resource_custom_image_id_map[1];
static const uint8_t empty_wstr[] = {0x00, 0x00};
extern const gdi_resource_string_map_search_t gdi_resource_string_map_search[1];
extern const uint16_t gdi_resource_translation_string_count;
extern const gdi_resource_custom_string_t gdi_resource_string_resource_fixed[1];
extern const gdi_resource_string_resource_list_t gdi_resource_string_list[2];


uint8_t *gdi_resource_get_image_data(uint16_t image_id)
{
    int32_t first = 0;
    int32_t last = gdi_resource_current_max_search_image_id - 1;
    int32_t middle;

    if (image_id == 0) {
        return (uint8_t *)(gdi_resource_custom_image_names[0].image);
    }

    while (last >= first) {
        middle = (first + last) / 2;
        if ((gdi_resource_image_id_search_map[middle].minimum_image_id <= image_id) &&
                (gdi_resource_image_id_search_map[middle].maximum_image_id >= image_id)) {
            uint16_t img_index;

            img_index = (uint16_t)gdi_resource_custom_image_id_map[(gdi_resource_image_id_search_map[middle].index) + (image_id) - (gdi_resource_image_id_search_map[middle].minimum_image_id)].image_number;
            return (uint8_t *)(gdi_resource_custom_image_names[img_index].image);
        }

        if (image_id > gdi_resource_image_id_search_map[middle].minimum_image_id) {
            first = middle + 1;
        } else {
            last = middle - 1;
        }
    }
    return (uint8_t *)(gdi_resource_custom_image_names[0].image);
}

uint8_t *gdi_resource_get_string_data(uint16_t string_id)
{
    uint8_t *ret_str = NULL;
    int32_t first = 0;
    int32_t last = 0;
    int32_t middle;
    uint32_t offset;
    uint8_t language_index = 0;
    uint8_t language_number = 1;
    const gdi_font_engine_language_t *language_list = NULL;
    uint16_t gdi_resource_current_max_string_id_search;
    bool gdi_resource_string_resource_is_16_bit;
    gdi_resource_custom_string_map_16_t *gdi_resource_fixed_string_map_resource_16 = NULL;
    gdi_resource_custom_string_map_t *gdi_resource_fixed_string_map_resource = NULL;
    gdi_resource_custom_string_t *gdi_resource_string_resource = NULL;
    gdi_resource_custom_string_map_16_t *gdi_resource_string_map_resource_16 = NULL;
    gdi_resource_custom_string_map_t *gdi_resource_string_map_resource = NULL;
    gdi_resource_custom_string_t *gdi_resource_string_resource_duplicate = NULL;
    uint32_t gdi_resource_maximum_non_duplicate_string_offset;

    gdi_font_engine_get_current_language_index(&language_index);
    gdi_font_engine_get_language_list(&language_list, &language_number);
    gdi_resource_current_max_string_id_search = gdi_resource_string_list[language_index].total_string_map_search;
    last = gdi_resource_current_max_string_id_search;

    /*initlize fixed string map*/
    gdi_resource_string_resource_is_16_bit = gdi_resource_string_list[language_number + 1].is16bit;
    if (gdi_resource_string_resource_is_16_bit) {
        gdi_resource_fixed_string_map_resource_16 = (gdi_resource_custom_string_map_16_t *)gdi_resource_string_list[language_number + 1].string_map;
    } else {
        gdi_resource_fixed_string_map_resource = (gdi_resource_custom_string_map_t *)gdi_resource_string_list[language_number + 1].string_map;
    }
    /*initlize string map*/
    gdi_resource_string_resource_is_16_bit = gdi_resource_string_list[language_index].is16bit;
    if (gdi_resource_string_resource_is_16_bit) {
        gdi_resource_string_map_resource_16 = (gdi_resource_custom_string_map_16_t *)gdi_resource_string_list[language_index].string_map;
    } else {
        gdi_resource_string_map_resource = (gdi_resource_custom_string_map_t *)gdi_resource_string_list[language_index].string_map;
    }

    /*initlize dup string offset*/
    gdi_resource_string_resource_duplicate = gdi_resource_string_list[language_number].string_resource;
    if (gdi_resource_string_resource_duplicate) {
        gdi_resource_maximum_non_duplicate_string_offset = (uint32_t)gdi_resource_string_list[language_number].total_string_map;
        gdi_resource_maximum_non_duplicate_string_offset = gdi_resource_maximum_non_duplicate_string_offset << 16;
        gdi_resource_maximum_non_duplicate_string_offset += gdi_resource_string_list[language_number].total_string_map_search;
    } else {
        gdi_resource_maximum_non_duplicate_string_offset = 0xFFFFFFFF;
    }

    /*initlize string resource*/
    gdi_resource_string_resource = gdi_resource_string_list[language_index].string_resource;

    if (string_id == 0) {
        return (uint8_t *) empty_wstr;
    }

    while (last >= first) {
        middle = (first + last) / 2;
        if (middle >= gdi_resource_current_max_string_id_search) {
            break;
        }
        if ((gdi_resource_string_map_search[middle].minimum_string_id <= string_id) && (gdi_resource_string_map_search[middle].maximum_string_id >= string_id)) {
            /* fixed string */
            if (gdi_resource_string_map_search[middle].map_index >= gdi_resource_translation_string_count) {
                assert(gdi_resource_string_resource_fixed != NULL);
                if (gdi_resource_string_resource_is_16_bit) {
                    offset = gdi_resource_fixed_string_map_resource_16[gdi_resource_string_map_search[middle].map_index + string_id - gdi_resource_string_map_search[middle].minimum_string_id - gdi_resource_translation_string_count] << 1;
                } else {
                    offset = gdi_resource_fixed_string_map_resource[gdi_resource_string_map_search[middle].map_index + string_id - gdi_resource_string_map_search[middle].minimum_string_id - gdi_resource_translation_string_count] << 1;
                }
                ret_str = (uint8_t *)&gdi_resource_string_resource_fixed[offset];
            } else {
                if (gdi_resource_string_resource_is_16_bit) {
                    offset = gdi_resource_string_map_resource_16[((gdi_resource_string_map_search[middle].map_index) + (string_id) - (gdi_resource_string_map_search[middle].minimum_string_id))] << 1;
                } else {
                    offset = gdi_resource_string_map_resource[((gdi_resource_string_map_search[middle].map_index) + (string_id) - (gdi_resource_string_map_search[middle].minimum_string_id))] << 1;
                }
                if (offset > gdi_resource_maximum_non_duplicate_string_offset) {
                    assert(gdi_resource_string_resource_duplicate != NULL);
                    ret_str = (uint8_t *) &gdi_resource_string_resource_duplicate[offset - gdi_resource_maximum_non_duplicate_string_offset];
                } else {
                    ret_str = (uint8_t *) &gdi_resource_string_resource[offset];
                }
            }
            break;
        }

        if (string_id > gdi_resource_string_map_search[middle].minimum_string_id) {
            first = middle + 1;
        } else {
            last = middle - 1;
        }

    }

    if (ret_str != NULL) {
        return ret_str;
    }
    return (uint8_t *) empty_wstr;
}
