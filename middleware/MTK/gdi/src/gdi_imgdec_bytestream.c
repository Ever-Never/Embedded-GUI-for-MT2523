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

/***************************************************************************** 
 * Include
 *****************************************************************************/
#include "gdi_internal.h"
#include "gdi_imgdec_bytestream.h"
#include "gdi_imgdec_internal.h"
#include "gdi_const.h"
#include "gdi_datatype.h"
#include <string.h>
#define GDI_IMGDEC_BYTESTREAM_RAISE(h, ret_code) \
do \
{ \
    if (h->jmpbuf) \
    { \
        /*longjmp(h->jmpbuf, ret_code);*/ \
    } \
} \
while(0)

/*****************************************************************************
 * FUNCTION
 *  gdi_imgdec_bytestream_init
 * DESCRIPTION
 *  Inits IMGDEC.
 * PARAMETERS
 *  handle        [IN]  bitstream handle
 *  src           [IN]  source pointer
 *  size          [IN]  bytestream size
 *  is_file       [IN]  is file
 * RETURNS
 *  Error code.
 *****************************************************************************/
gdi_result_t gdi_imgdec_bytestream_init(gdi_imgdec_bytestream_struct *handle, uint8_t* src, uint32_t size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(handle, 0, sizeof(gdi_imgdec_bytestream_struct));

    if (size == 0)
    {
        //TODO: not support file.
    }
    else
    {
        handle->buffer = src;
        handle->buffer_ptr = src;
        handle->buffer_ptr_size = size;
        handle->is_from_memory = true;
    }

    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_bytestream_free
 * DESCRIPTION
 *  free a bytestream
 * PARAMETERS
 *  void
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
gdi_result_t gdi_imgdec_bytestream_deinit(gdi_imgdec_bytestream_struct *handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_imgdec_bytestream_read
 * DESCRIPTION
 *  Reads bytestream.
 * PARAMETERS
 *  handle       [IN]   bitstream handle
 *  ptr          [OUT]  read buffer
 *  size         [IN]   read size
 * RETURNS
 *  Error code.
 *****************************************************************************/
gdi_result_t gdi_imgdec_bytestream_read(gdi_imgdec_bytestream_struct *handle, void *ptr, uint32_t size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t result = GDI_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if (handle->is_from_memory == true)
    {
        // TODO: overflow check
        memcpy(ptr, handle->buffer_ptr, size);
        handle->buffer_ptr += size;

        result = GDI_SUCCEED;
    }
    
    return result;  
}

/*****************************************************************************
 * FUNCTION
 *  gdi_imgdec_bytestream_seek
 * DESCRIPTION
 *  Seeks bitstream.
 * PARAMETERS
 *  handle       [IN]   bitstream handle
 *  offset       [IN]   seek offset
 * RETURNS
 *  Error code.
 *****************************************************************************/
gdi_result_t gdi_imgdec_bytestream_seek(gdi_imgdec_bytestream_struct *handle, int32_t offset)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t result;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    result = GDI_FAILED;
    if (handle->is_from_memory == true)
    {
        handle->buffer_ptr += offset;
    
        result = GDI_SUCCEED;
    }
    
    return result;
}





