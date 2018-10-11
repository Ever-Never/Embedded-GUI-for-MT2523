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
 
#ifndef _GDI_BYTESTREAM_H_
#define _GDI_BYTESTREAM_H_

#include "gdi_const.h"
#include "gdi_datatype.h"

#define BYTESTREAM_BUFFER_SIZE      512 /* size of file buffer cache */

extern gdi_result_t gdi_bytestream_create(uint8_t *buf_ptr, uint32_t buf_size);
extern gdi_result_t gdi_bytestream_create_file(const char *file_name);
extern gdi_result_t gdi_bytestream_create_file_ext(
                        const char *file_name);
extern gdi_result_t gdi_bytestream_free(void);

extern void gdi_bytestream_buffer_load_file(int at_least);
extern bool gdi_bytestream_buffer_load_file_widthout_exception(int at_least);
extern void gdi_bytestream_seek_file(int32_t offset);

extern uint8_t(*gdi_bytestream_get_byte) (void);
extern uint8_t(*gdi_bytestream_peek_byte) (void);
extern uint8_t(*gdi_bytestream_peek_byte_n) (int index);
extern uint16_t(*gdi_bytestream_get_word) (void);
extern uint32_t(*gdi_bytestream_get_3bytes) (void);
extern uint32_t(*gdi_bytestream_get_dword) (void);
extern void (*gdi_bytestream_get_array) (uint8_t *array, uint32_t size);
extern void (*gdi_bytestream_flush) (uint32_t count);
extern void (*gdi_bytestream_seek) (int32_t offset);
extern int32_t(*gdi_bytestream_tell) (void);
extern bool(*gdi_bytestream_is_eof) (void);
extern uint32_t(*gdi_bytestream_get_size) (void);

extern int gdi_bytestream_file_size;
extern int gdi_bytestream_buffer_content_size;
extern int gdi_bytestream_file_offset;
extern uint8_t *gdi_bytestream_buffer_reader;

extern uint8_t *gdi_bytestream_buffer_ptr;
extern uint32_t gdi_bytestream_buffer_ptr_size;

#define     GDI_BYTESTREAM_PEEK_BYTE_FILE(X)       \
         {                                         \
            if(!gdi_bytestream_buffer_content_size)\
               gdi_bytestream_buffer_load_file(1); \
            X = *gdi_bytestream_buffer_reader;     \
         }

#define     GDI_BYTESTREAM_PEEK_BYTE_N_FILE(X,OFFSET)    \
         {                                               \
            if(gdi_bytestream_buffer_content_size<OFFSET)\
               gdi_bytestream_buffer_load_file(OFFSET);  \
            X = gdi_bytestream_buffer_reader[OFFSET];    \
         }

#define     GDI_BYTESTREAM_GET_BYTE_FILE(X)          \
         {                                           \
            if(!(gdi_bytestream_buffer_content_size))\
               gdi_bytestream_buffer_load_file(1);   \
            gdi_bytestream_buffer_content_size--;    \
            gdi_bytestream_file_offset++;            \
            X = *gdi_bytestream_buffer_reader++;     \
         }                                           \

#define     GDI_BYTESTREAM_GET_WORD_FILE(X)             \
         {                                              \
            if(gdi_bytestream_buffer_content_size<2)    \
               gdi_bytestream_buffer_load_file(2);      \
            gdi_bytestream_buffer_content_size-=2;      \
            gdi_bytestream_file_offset+=2;              \
            X = *gdi_bytestream_buffer_reader++;        \
            X |= (*gdi_bytestream_buffer_reader++) << 8;\
         }                                              \

#define     GDI_BYTESTREAM_GET_3BYTES_FILE(X)            \
         {                                               \
            if(gdi_bytestream_buffer_content_size<3)     \
               gdi_bytestream_buffer_load_file(3);       \
            gdi_bytestream_buffer_content_size-=3;       \
            gdi_bytestream_file_offset+=3;               \
            X = *gdi_bytestream_buffer_reader++;         \
            X |= (*gdi_bytestream_buffer_reader++) << 8; \
            X |= (*gdi_bytestream_buffer_reader++) << 16;\
         }                                               \

#define     GDI_BYTESTREAM_GET_DWORD_FILE(X)             \
         {                                               \
            if(gdi_bytestream_buffer_content_size<4)     \
               gdi_bytestream_buffer_load_file(4);       \
            gdi_bytestream_buffer_content_size-=4;       \
            gdi_bytestream_file_offset+=4;               \
            X = *gdi_bytestream_buffer_reader++;         \
            X |= (*gdi_bytestream_buffer_reader++) << 8; \
            X |= (*gdi_bytestream_buffer_reader++) << 16;\
            X |= (*gdi_bytestream_buffer_reader++) << 24;\
         }                                               \

#define GDI_BYTESTREAM_FLUSH_FILE(COUNT)                             \
         {                                                           \
            int count = COUNT;                                       \
            gdi_bytestream_file_offset+=count;                       \
            if(count >= gdi_bytestream_buffer_ptr_size)              \
               gdi_bytestream_seek_file(gdi_bytestream_file_offset); \
            else                                                     \
            {                                                        \
               while(count)                                          \
               {                                                     \
                  if(gdi_bytestream_buffer_content_size < (int)count)\
                  {                                                  \
                     count -= gdi_bytestream_buffer_content_size;    \
                     gdi_bytestream_buffer_content_size=0;           \
                     gdi_bytestream_buffer_load_file(1);             \
                  }                                                  \
                  else                                               \
                  {                                                  \
                     gdi_bytestream_buffer_content_size-=count;      \
                     gdi_bytestream_buffer_reader +=count;           \
                     count=0;                                        \
                  }                                                  \
               }                                                     \
            }                                                        \
         }                                                           \

#define GDI_BYTESTREAM_IS_EOF_FILE                 \
         (gdi_bytestream_buffer_content_size?FALSE:\
            (gdi_bytestream_buffer_load_file_widthout_exception(1))?FALSE:TRUE)

#define GDI_BYTESTREAM_TELL_FILE       gdi_bytestream_file_offset

#define GDI_BYTESTREAM_GET_ARRAY_FILE(ARRAY_POINTER,SIZE)     \
   {                                                          \
      int size = SIZE;                                        \
      while(size>0)                                           \
      {                                                       \
         int n;                                               \
         if(gdi_bytestream_buffer_content_size == 0)          \
            gdi_bytestream_buffer_load_file(1);               \
         n= GDI_MIN(size,gdi_bytestream_buffer_content_size); \
         memcpy(ARRAY_POINTER,gdi_bytestream_buffer_reader,n);\
         ARRAY_POINTER+=n;                                    \
         gdi_bytestream_buffer_content_size -=n;              \
         size -= n;                                           \
         gdi_bytestream_file_offset+=n;                       \
         gdi_bytestream_buffer_reader+=n;                     \
      }                                                       \
   }

#endif /* _GDI_BYTESTREAM_H_ */ 


