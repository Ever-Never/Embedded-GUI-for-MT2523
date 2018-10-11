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

#include <string.h>
#include "gdi_internal.h"
#include "gdi_features.h"
#include "gdi_image.h"
#include "gdi_image_bmp.h"
#include "gdi_imgdec_internal.h"
#include <stdio.h>
#include "gdi_resource_internal.h"
#define GDI_TRACE(...)  

/****************************************************************************
* Local Variable
*****************************************************************************/

static const gdi_image_codecs_struct gdi_image_codecs_invalid = 
{NULL, NULL, NULL, false};

static const gdi_image_codecs_struct gdi_image_codecs_bmp = 
{
    gdi_image_bmp_draw_handler,
    gdi_image_bmp_get_dimension_handler,
    NULL, false};


static const gdi_image_codecs_struct gdi_image_codecs_dev_bmp =
{NULL, NULL, NULL, false};
    
#ifdef GDI_USING_WBMP
static const gdi_image_codecs_struct gdi_image_codecs_wbmp = 
    {
        gdi_image_wbmp_draw_handler,
        gdi_image_wbmp_get_dimension_handler,
        NULL, false};
#endif


/* should make sure sizeof(gdi_image_codecs) == GDI_IMAGE_TYPE_SUM */
const gdi_image_codecs_struct * const gdi_image_codecs[]=
    {
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_bmp,
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_BMP_SEQUENCE */
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_dev_bmp,
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_DEVICE_BITMAP_SEQUENCE */
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid,
    #ifdef GDI_USING_WBMP
        &gdi_image_codecs_wbmp,
    #else
        &gdi_image_codecs_invalid,
    #endif
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_AVI */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_AVI_FILE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_3GP */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_3GP_FILE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_MP4 */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_MP4_FILE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_JPG_SEQUENCE */
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_PNG_SEQUENCE */
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_BMP_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_GIF_FILE_OFFSET */
        &gdi_image_codecs_invalid,  /* GDI_IMAGE_TYPE_M3D */
        &gdi_image_codecs_invalid,  /* GDI_IMAGE_TYPE_M3D_FILE */
        &gdi_image_codecs_invalid,  /* GDI_IMAGE_TYPE_SVG */
        &gdi_image_codecs_invalid,  /* GDI_IMAGE_TYPE_SVG_FILE */
        &gdi_image_codecs_invalid,  /* GDI_IMAGE_TYPE_MFLASH */
        &gdi_image_codecs_invalid,  /* GDI_IMAGE_TYPE_MFLASH_FILE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_JPG_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_PNG_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_DEVICE_BMP_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_WBMP_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_M3D_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_SVG_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_SWFLASH_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_AVATAR */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_ABM_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_ABM */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_ABM_SEQUENCE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_ABM_FILE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_MPG */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_MPG_FILE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_MPG_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_3G2 */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_3G2_FILE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_3G2_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_VIS */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_VIS_FILE */
        &gdi_image_codecs_bmp, /* GDI_IMAGE_TYPE_BMP_MEM */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_AB2 */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_AB2_SEQUENCE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_AB2_FILE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_AB2_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_BMP_SEQUENCE_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_DEVICE_BITMAP_SEQUENCE_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_JPG_SEQUENCE_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_PNG_SEQUENCE_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_ABM_SEQUENCE_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_AB2_SEQUENCE_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_9SLICE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_9SLICE_FILE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_9SLICE_FILE_OFFSET */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_RM_FILE */
        &gdi_image_codecs_invalid, /* GDI_IMAGE_TYPE_RA_FILE */
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid,
        &gdi_image_codecs_invalid
    };

bool gdi_memory_output = false;
uint16_t gdi_memory_output_width;
uint16_t gdi_memory_output_height;
uint32_t gdi_memory_output_buffer_address;
uint32_t gdi_memory_output_buffer_size;

static int32_t gdi_image_update_area_x1;
static int32_t gdi_image_update_area_y1;
static int32_t gdi_image_update_area_x2;
static int32_t gdi_image_update_area_y2;


void gdi_image_set_update_area(int32_t x1,int32_t y1,int32_t x2,int32_t y2)
{
    gdi_image_update_area_x1 = x1;
    gdi_image_update_area_y1 = y1;
    gdi_image_update_area_x2 = x2;
    gdi_image_update_area_y2 = y2;
}

void gdi_image_get_update_area(int32_t *x1,int32_t *y1,int32_t *x2,int32_t *y2)
{
    *x1 = gdi_image_update_area_x1;
    *y1 = gdi_image_update_area_y1;
    *x2 = gdi_image_update_area_x2;
    *y2 = gdi_image_update_area_y2;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_image_parse_resource_internal
 * DESCRIPTION
 *  
 * PARAMETERS
 *  res_src         [?]         
 *  data_ptr        [IN]        
 *  img_type        [?]         
 *  img_size        [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_image_parse_resource_internal(const uint8_t *res_src, uint8_t **data_ptr, uint8_t *img_type, uint32_t *img_size, uint32_t *frame_number, uint32_t *width, uint32_t *height)
{
    const uint8_t *imgBuffer = res_src;
	GDI_DEBUG_ASSERT(imgBuffer);
	*img_type = imgBuffer[0];
	*frame_number = (int32_t)(uint32_t) imgBuffer[1];

    switch (*img_type)
    {
        case GDI_IMAGE_TYPE_BMP_FILE_OFFSET:
        case GDI_IMAGE_TYPE_GIF_FILE_OFFSET:
        case GDI_IMAGE_TYPE_JPG_FILE_OFFSET:
        case GDI_IMAGE_TYPE_PNG_FILE_OFFSET:
        case GDI_IMAGE_TYPE_DEVICE_BMP_FILE_OFFSET:
        case GDI_IMAGE_TYPE_WBMP_FILE_OFFSET:
        case GDI_IMAGE_TYPE_M3D_FILE_OFFSET:
        case GDI_IMAGE_TYPE_SVG_FILE_OFFSET:
        case GDI_IMAGE_TYPE_SWFLASH_FILE_OFFSET:
        case GDI_IMAGE_TYPE_ABM_FILE_OFFSET:
        case GDI_IMAGE_TYPE_AB2_FILE_OFFSET:
        case GDI_IMAGE_TYPE_9SLICE_FILE_OFFSET:
		*width 		  = (int32_t)(( ((uint32_t)imgBuffer[7])     <<4) | (((uint32_t)imgBuffer[6]&0xF0)>>4));
		*height		  = (int32_t)(((((uint32_t)imgBuffer[6])&0xF)<<8) |  ((uint32_t)imgBuffer[5]         ));
		*img_size = (int32_t)(((uint32_t)imgBuffer[4])<<16)|(((uint32_t)imgBuffer[3])<<8)|(((uint32_t)imgBuffer[2]));
		*data_ptr = (uint8_t *)imgBuffer;
		    break;
            
        case GDI_IMAGE_TYPE_BMP_SEQUENCE:
        case GDI_IMAGE_TYPE_DEVICE_BITMAP_SEQUENCE:
        case GDI_IMAGE_TYPE_JPG_SEQUENCE:
        case GDI_IMAGE_TYPE_PNG_SEQUENCE:
        case GDI_IMAGE_TYPE_ABM_SEQUENCE:
        case GDI_IMAGE_TYPE_AB2_SEQUENCE:
        case GDI_IMAGE_TYPE_BMP_SEQUENCE_FILE_OFFSET:
        case GDI_IMAGE_TYPE_DEVICE_BITMAP_SEQUENCE_FILE_OFFSET:
        case GDI_IMAGE_TYPE_JPG_SEQUENCE_FILE_OFFSET:
        case GDI_IMAGE_TYPE_PNG_SEQUENCE_FILE_OFFSET:
        case GDI_IMAGE_TYPE_ABM_SEQUENCE_FILE_OFFSET:
        case GDI_IMAGE_TYPE_AB2_SEQUENCE_FILE_OFFSET:
		*width 		  = (int32_t)(( ((uint32_t)imgBuffer[7])     <<4) | (((uint32_t)imgBuffer[6]&0xF0)>>4));
		*height		  = (int32_t)(((((uint32_t)imgBuffer[6])&0xF)<<8) |  ((uint32_t)imgBuffer[5]         ));
		*img_size = (int32_t) (imgBuffer[2]) | (int32_t) ((int32_t) imgBuffer[3] << 8) | (int32_t) ((int32_t) imgBuffer[4] << 16);
			// Here img_size is 0. To get img sequence data size, use gdi_image_sequence_get_data_len.
			// See gdi_image_get_buf_len.
            break;
        case GDI_IMAGE_TYPE_DEV_BMP:
            *width 		  = (int32_t)((uint32_t)res_src[6]);
            *height		  = (int32_t)((uint32_t)res_src[8]);
            *img_size = (*width) * (*height) * ((uint8_t)res_src[4]) + 12;
            *data_ptr = (uint8_t *)res_src;
            break;           
        default:
		*width 		  = (int32_t)(( ((uint32_t)imgBuffer[7])     <<4) | (((uint32_t)imgBuffer[6]&0xF0)>>4));
		*height		  = (int32_t)(((((uint32_t)imgBuffer[6])&0xF)<<8) |  ((uint32_t)imgBuffer[5]         ));
		*img_size = (int32_t) (imgBuffer[2]) | (int32_t) ((int32_t) imgBuffer[3] << 8) | (int32_t) ((int32_t) imgBuffer[4] << 16);
		*data_ptr = (uint8_t *)imgBuffer + 8;
            break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  gdi_image_get_dimension_by_id
 * DESCRIPTION
 *  stop a animation by handle
 * PARAMETERS
 *  image_id        [IN]        
 *  width           [OUT]       
 *  height          [OUT]       
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_image_get_dimension_by_id(uint16_t image_id, uint32_t *width, uint32_t *height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return gdi_image_codec_get_dimension(
            GDI_IMAGE_SOURCE_RESOURCE,
            (const uint8_t*) gdi_resource_get_image_data(image_id),
            GDI_IMAGE_TYPE_INVALID,
            0,
            width,
            height,
            0);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_image_get_dimension
 * DESCRIPTION
 *  get image dimesion from resource
 * PARAMETERS
 *  image_ptr       [IN]        
 *  width           [OUT]       
 *  height          [OUT]       
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_image_get_dimension(const uint8_t *image_ptr, uint32_t *width, uint32_t *height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return gdi_image_codec_get_dimension(
            GDI_IMAGE_SOURCE_RESOURCE,
            image_ptr,
            GDI_IMAGE_TYPE_INVALID,
            0,
            width,
            height,
            0);
}


/*****************************************************************************
 * FUNCTION
 *  gdi_image_get_dimension_mem
 * DESCRIPTION
 *  get image dimesion from memory
 * PARAMETERS
 *  image_ptr       [IN]        
 *  width           [OUT]       
 *  height          [OUT]       
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_image_get_dimension_mem(uint8_t img_type,const uint8_t *img_ptr,uint32_t img_size,uint32_t *width, uint32_t *height)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return gdi_image_codec_get_dimension(
            GDI_IMAGE_SOURCE_MEMORY,
            img_ptr,
            img_type,
            img_size,
            width,
            height,
            0);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_image_get_type_from_mem
 * DESCRIPTION
 *  check gdi decoder's capability
 * PARAMETERS
 *  ptr        [IN]        
 * RETURNS
 *  uint16_t
 *****************************************************************************/
uint16_t gdi_image_get_type_from_mem_internal(uint8_t *p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (p[0] == 'B' && p[1] == 'M')
        return GDI_IMAGE_TYPE_BMP;

    if (p[0] == 'G' && p[1] == 'I' && p[2] == 'F')
        return GDI_IMAGE_TYPE_GIF;

    if (p[0] == 0xff && p[1] == 0xd8 && p[2] == 0xff)
        return GDI_IMAGE_TYPE_JPG;

    if (memcmp((void*)p, "\x89PNG\x0d\x0a\x1a\x0a", 8) == 0)
        return GDI_IMAGE_TYPE_PNG;

    if (p[0] == 'P' && p[1] == 'B' && p[2] == 'M')
        return GDI_IMAGE_TYPE_DEVICE_BITMAP;

    return (GDI_IMAGE_TYPE_INVALID);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_image_get_type_from_mem
 * DESCRIPTION
 *  check gdi decoder's capability
 * PARAMETERS
 *  ptr        [IN]        
 * RETURNS
 *  uint16_t
 *****************************************************************************/
gdi_image_type_t gdi_image_get_type_from_mem(const uint8_t *ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return gdi_image_get_type_from_mem_internal((uint8_t*)ptr);
}

gdi_result_t gdi_image_get_info_from_res_data(
    const uint8_t *res_data,
    gdi_image_type_t *image_type,
    uint32_t *width,
    uint32_t *height,
    bool *with_alpha)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t t_image_type;
    uint8_t *t_data_ptr;
    uint32_t t_img_size;
    uint32_t t_frame_number;
    uint32_t t_width, t_height;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    gdi_image_parse_resource_internal(
        (uint8_t*)res_data,
        &t_data_ptr,
        &t_image_type,
        &t_img_size,
        &t_frame_number,
        &t_width,
        &t_height);

    if (image_type != NULL)
    {
        *image_type = t_image_type;
    }
    if (width != NULL)
    {
        *width = t_width;
    }
    if (height != NULL)
    {
        *height = t_height;
    }

    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_image_parse_mem_internal
 * DESCRIPTION
 *  
 * PARAMETERS
 *  res_src         [?]         
 *  data_ptr        [IN]        
 *  img_type        [?]         
 *  img_size        [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void gdi_image_parse_mem_internal(const uint8_t *res_src, uint8_t **data_ptr, uint8_t *img_type, uint32_t *img_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	const uint8_t *imgBuffer = res_src;
	*img_type = imgBuffer[0];

    switch (*img_type)
    {
        case GDI_IMAGE_TYPE_AVATAR:
		*data_ptr = (uint8_t *)imgBuffer;
            *img_size = 
			(int32_t) (imgBuffer[4]) 
			| (int32_t) ((int32_t) imgBuffer[5] << 8) 
			| (int32_t) ((int32_t) imgBuffer[6] << 16) 
			| (int32_t) ((int32_t) imgBuffer[7] << 24);
            break;
        case GDI_IMAGE_TYPE_GIF:
        case GDI_IMAGE_TYPE_M3D:
        case GDI_IMAGE_TYPE_PNG:
        case GDI_IMAGE_TYPE_JPG_SEQUENCE:
        case GDI_IMAGE_TYPE_PNG_SEQUENCE:
        case GDI_IMAGE_TYPE_BMP_SEQUENCE:
		*data_ptr = (uint8_t *)imgBuffer + 8;
            break;
	    case GDI_IMAGE_TYPE_BMP_FILE_OFFSET:
	    case GDI_IMAGE_TYPE_GIF_FILE_OFFSET:
        case GDI_IMAGE_TYPE_JPG_FILE_OFFSET:
        case GDI_IMAGE_TYPE_PNG_FILE_OFFSET:
        case GDI_IMAGE_TYPE_DEVICE_BMP_FILE_OFFSET:
        case GDI_IMAGE_TYPE_WBMP_FILE_OFFSET:
        case GDI_IMAGE_TYPE_M3D_FILE_OFFSET:
        case GDI_IMAGE_TYPE_SVG_FILE_OFFSET:
        case GDI_IMAGE_TYPE_SWFLASH_FILE_OFFSET:
		*img_size = (((int32_t)imgBuffer[2])<<16)  |  (((int32_t)imgBuffer[3])<<8)  |  (((int32_t)imgBuffer[4]));
		*data_ptr = (uint8_t *)imgBuffer;
		    break;
        default:
		*data_ptr = (uint8_t *)imgBuffer + 6;
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  gdi_image_parse_info
 * DESCRIPTION
 *  
 * PARAMETERS
 *  src_type        [IN]        
 *  img_src         [?]         
 *  data_ptr        [IN]        
 *  img_type        [?]         
 *  img_size        [?]         
 * RETURNS
 *  
 *****************************************************************************/
gdi_result_t gdi_image_parse_info(
            gdi_image_source_t src_type,
            const uint8_t *img_src,
            uint8_t **data_ptr,
            uint8_t *img_type,
            uint32_t *img_size,
            uint32_t *frame_number,
            uint32_t *width,
            uint32_t *height)
{
	const uint8_t *imgBuffer = img_src;
    *width = *height = *frame_number = 0;

    if (imgBuffer == NULL)
        return (GDI_IMAGE_ERR_NULL_IMG_PTR);

    /* Detect image type if unknown */
    *data_ptr = (uint8_t *)img_src;    /* default data is from begin of img_src */
    switch (src_type)
    {
        case GDI_IMAGE_SOURCE_RESOURCE:
            gdi_image_parse_resource_internal(imgBuffer, data_ptr, img_type, img_size,frame_number,width,height);
            break;
        case GDI_IMAGE_SOURCE_RESOURCE_WITHOUT_HEADER:
        case GDI_IMAGE_SOURCE_MEMORY:
            if (*img_type == GDI_IMAGE_TYPE_INVALID)
            {
                return (GDI_IMAGE_ERR_INVALID_IMG_TYPE);
            }
            else if (*img_type == GDI_IMAGE_TYPE_AVATAR)
            {
                gdi_image_parse_mem_internal(imgBuffer, data_ptr, img_type, img_size);
            } 
            break;
        default:
            GDI_ASSERT(0);  /* unknown src type */
    }
    return (GDI_SUCCEED);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_image_codec_get_dimension
 * DESCRIPTION
 *  
 * PARAMETERS
 *  src_type        [IN]        
 *  img_src         [?]         
 *  img_type        [IN]        
 *  img_size        [IN]        
 *  width           [?]         
 *  height          [?]         
 *  flag            [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_result_t gdi_image_codec_get_dimension(
            gdi_image_source_t src_type,
            const uint8_t *img_src,
            uint8_t img_type,
            uint32_t img_size,
            uint32_t *width,
            uint32_t *height,
            uint32_t flag)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t *data_ptr;
    gdi_result_t ret;
    uint32_t tmp;
    const gdi_image_codecs_struct *codec;
    const uint8_t  *imgBuffer = img_src;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ret = gdi_image_parse_info(src_type, imgBuffer, &data_ptr, &img_type, &img_size, &tmp, width, height);
    
    if (ret != GDI_SUCCEED) return ret;
    
    if(*width == 0 && *height == 0)
    {
	    if (GDI_IMAGE_TYPE_INVALID == img_type || img_type >= GDI_IMAGE_TYPE_SUM)
	        return GDI_IMAGE_ERR_INVALID_IMG_TYPE;

        codec = gdi_image_codecs[img_type];
	
        if(src_type == GDI_IMAGE_SOURCE_MEMORY)
            flag |= GDI_IMAGE_CODEC_FLAG_IS_MEM;
	    ret = GDI_IMAGE_ERR_NO_DECODER_SUPPORT;
	    *width = *height = 0;
            if (codec->get_dimension)
    	        ret = codec->get_dimension(flag, data_ptr, img_size, width, height);
	}
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_image_create_temp_layer
 * DESCRIPTION
 *  The layer created by this function must freed by gdi_image_free_temp_layer.
 *  alloc a buffer from med buffer, and then use the buffer to create layer
 * PARAMETERS
 *  temp_layer_handle  [OUT]  the created layer
 *  cf                 [IN]   color format of created layer
 *  x                  [IN]   x position of created layer
 *  y                  [IN]   y position of created layer
 *  width              [IN]   width of created layer
 *  height             [IN]   height of created layer
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_image_create_temp_layer(
            gdi_layer_handle_t *temp_layer_handle,
            gdi_color_format_t cf,
            int32_t x,
            int32_t y,
            int32_t width,
            int32_t height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t bytes_per_pixel;
    uint32_t buf_size;
    uint8_t *buf;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ASSERT(temp_layer_handle != NULL);
    GDI_ASSERT(width > 0 && height > 0);

    *temp_layer_handle = GDI_NULL_HANDLE;

    bytes_per_pixel = gdi_bits_per_pixel(cf) >> 3;
    buf_size = width * height * bytes_per_pixel;
    
    buf = (uint8_t *)gdi_alloc_ext_mem_framebuffer(buf_size);
    if (buf == NULL)
    {
        /* not enough memory */
        return GDI_IMAGE_ERR_MEMORY_NOT_AVAILABLE;
    }
        
    gdi_layer_create_cf_using_outside_memory(
        cf, 
        x, 
        y,
        width, 
        height,
        buf, 
        buf_size,
        temp_layer_handle);
    if (*temp_layer_handle == GDI_NULL_HANDLE)
    {
        gdi_free_ext_mem((void**)&buf);
        return GDI_FAILED;
    }

    return GDI_SUCCEED;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_image_free_temp_layer
 * DESCRIPTION
 *  free the layer that created by gdi_image_create_temp_layer.
 * PARAMETERS
 *  temp_layer_handle  [IN]  the layer to be freed
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_image_free_temp_layer(gdi_layer_handle_t temp_layer_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t *buf;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (temp_layer_handle == GDI_NULL_HANDLE)
    {
        /* Do nothing */
        return;
    }

    buf = ((gdi_layer_struct_t *)temp_layer_handle)->buf_ptr;
    
    gdi_layer_free(temp_layer_handle);

    if (buf != NULL)
    {
        gdi_free_ext_mem((void**)&buf);
    }
}


/*****************************************************************************
 * FUNCTION
 *  gdi_image_codec_draw
 * DESCRIPTION
 *  
 * PARAMETERS
 *  src_type        [IN]        
 *  img_src         [?]         
 *  img_type        [IN]        
 *  img_size        [IN]        
 *  x               [IN]        
 *  y               [IN]        
 *  w               [IN]        
 *  h               [IN]        
 *  flag            [IN]        
 *  frame_pos       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_result_t gdi_image_codec_draw(
            gdi_image_source_t src_type,
            const uint8_t *img_src, 
            gdi_image_type_t img_type,                       
            uint32_t img_size,
            int32_t x,
            int32_t y,
            uint32_t w,
            uint32_t h)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t *data_ptr;
    gdi_result_t ret;
    const gdi_image_codecs_struct *codec;
	uint32_t tmp;
	const uint8_t *imgBuffer = img_src;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_image_codec_draw)

GDI_LOG("[GDI][IMG]enter gdi_image_codec_draw x:%d y:%d w:%d h:%d\n", x, y, w, h);
    if ((x > gdi_act_layer->clipx2) || (y > gdi_act_layer->clipy2))
    {
        GDI_RETURN(GDI_SUCCEED);
    }

	ret = gdi_image_parse_info(src_type, imgBuffer, &data_ptr, &img_type, &img_size,&tmp,&tmp,&tmp);
    if (ret != GDI_SUCCEED)
    {
        GDI_RETURN(ret);
    }
GDI_LOG("[GDI][IMG]parse_info end ret:%d, src:%d, type:%d, size:%d\n", ret, src_type, img_type, img_size);
    
    if (GDI_IMAGE_TYPE_DEV_BMP == img_type)
    {
        img_type = GDI_IMAGE_TYPE_DEVICE_BITMAP;
    }
    
    if (GDI_IMAGE_TYPE_INVALID == img_type || img_type >= GDI_IMAGE_TYPE_SUM)
    {
        GDI_RETURN(GDI_IMAGE_ERR_INVALID_IMG_TYPE);
    }

    codec = gdi_image_codecs[img_type];

    ret = GDI_SUCCEED;
    
    gdi_image_set_update_area(x,y,x+w,y+h);
    GDI_LOG("[GDI][IMG]update area:(%d, %d, %d, %d)\n", x, y, w, h);

    if (codec->draw)
    {
        ret = codec->draw(x, y, w, h, data_ptr, img_size);
        if(ret < GDI_SUCCEED) 
            ret = GDI_ERR_NB_ABORT;
    }
    GDI_EXIT_CRITICAL_SECTION(gdi_image_codec_draw)

    return ret;
}

gdi_result_t gdi_image_draw_by_id(
             int32_t x,
             int32_t y,
             uint16_t image_id)
{    return gdi_image_codec_draw(
             GDI_IMAGE_SOURCE_RESOURCE,
             (const uint8_t*) gdi_resource_get_image_data(image_id),
             GDI_IMAGE_TYPE_INVALID,
             0,
             x,
             y,
             0,
             0);
}

gdi_result_t gdi_image_draw_resized_by_id(
                    int32_t x,
                    int32_t y,
                    uint32_t w,
                    uint32_t h,
                    uint16_t image_id)
{
    return gdi_image_codec_draw(
             GDI_IMAGE_SOURCE_RESOURCE,
             (const uint8_t*) gdi_resource_get_image_data(image_id),
             GDI_IMAGE_TYPE_INVALID,
             0,
             x,
             y,
             w,
             h);
}
