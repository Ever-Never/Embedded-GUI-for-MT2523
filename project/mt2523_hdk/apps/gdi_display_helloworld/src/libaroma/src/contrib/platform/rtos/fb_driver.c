/********************************************************************[libaroma]*
 * Copyright (C) 2011-2015 Ahmad Amarullah (http://amarullz.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *______________________________________________________________________________
 *
 * Filename    : fb_driver.c
 * Description : QNX Framebuffer driver
 *
 * + This is part of libaroma, an embedded ui toolkit.
 * + 21/01/15 - Author(s): Ahmad Amarullah
 *
 */
#ifndef __libaroma_rtos_fb_driver_c__
#define __libaroma_rtos_fb_driver_c__
/*
 * Headers Includes
 *
 */
#include <aroma_internal.h>
#include "bsp_lcd.h"
#include "gdi.h"
#include "memory_attribute.h"


typedef struct _RTOS_INTERNAL RTOS_INTERNAL;
typedef struct _RTOS_INTERNAL * RTOS_INTERNALP;

						
/*
 * structure : internal framebuffer data
 */
struct _RTOS_INTERNAL{

  int       fb_sz;                      /* framebuffer memory size */
  voidp     buffer;                     /* direct buffer */
  int       stride;                     /* stride size */
  int       line;                       /* line size */
  byte      depth;                      /* color depth */
  byte      pixsz;                      /* memory size per pixel */
  byte      rgb_pos[6];                 /* framebuffer 32bit rgb position */
  LIBAROMA_MUTEX  mutex;
  
};


LIBAROMA_MUTEX ___rtosfbmutex;

/*
 * Forward Functions
 *
 */
void RTOS_release(LIBAROMA_FBP me);
void RTOS_dump(RTOS_INTERNALP mi);
byte RTOS_start_post(LIBAROMA_FBP me);
byte RTOS_post(
    LIBAROMA_FBP me, wordp __restrict src,
    int dx, int dy, int dw, int dh,
    int sx, int sy, int sw, int sh);
byte RTOS_end_post(LIBAROMA_FBP me);

static uint32_t LCD_WIDTH = 320;
static uint32_t LCD_HEIGHT = 320;

ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char layer_buffers[320 * 320 * 2];

void rtos_screen_init()
{
  bsp_lcd_init(0xF800);
  bsp_backlight_init();
  bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, &LCD_WIDTH); // Get the LCD size.
  bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, &LCD_HEIGHT);
  gdi_init(LCD_WIDTH, LCD_HEIGHT, GDI_COLOR_FORMAT_16, layer_buffers); // Initialize the GDI buffer.
}

/*
 * Function : Framebuffer Driver Initializer
 *
 */
byte RTOS_init(LIBAROMA_FBP me)
{
  /* Allocating Internal Data */
  ALOGV("RTOS Initialized Internal Data");
  RTOS_INTERNALP mi = (RTOS_INTERNALP)calloc(sizeof(RTOS_INTERNAL), 1);
  

  if (!mi)
  {
    ALOGE("RTOS malloc internal data - Memory Error");
    return 0;
  }

  rtos_screen_init() ;

  
    /* set internal address */
  me->internal = (voidp) mi;
  me->w        = LCD_WIDTH;  /* width */
  me->h        = LCD_HEIGHT;  /* height */
  me->sz       = me->w*me->h;   /* width x height */
  me->double_buffer =0 ;
  me->snapshoot   = NULL;
  mi->fb_sz = 320*320 ;
  mi->buffer = layer_buffers ;
  mi->line = 320*2 ;
  mi->pixsz = 2 ;



  /* set sync callbacks */
  me->start_post = &RTOS_start_post;
  me->end_post = &RTOS_end_post;
  me->post = &RTOS_post;
  me->release=&RTOS_release ;
  libaroma_mutex_init(___rtosfbmutex);

  return 1;
}

/*
 * Function : Release Framebuffer Instance (Same For All Colorspace)
 *
 */
void RTOS_release(LIBAROMA_FBP me)
{
  /* Is Framebuffer Initialized ? */
  if (me == NULL)
  {
    return;
  }

  libaroma_mutex_free(___rtosfbmutex);
}

/*
 * Function : Dump Framebuffer Informations
 *
 */
void RTOS_dump(RTOS_INTERNALP mi)
{
  ALOGV("DISPLAY INFORMATIONS:");
}

/*
 * Function    : QNXGF_start_post
 * Return Value: byte
 * Descriptions: start post
 */
byte RTOS_start_post(LIBAROMA_FBP me)
{
  if (me == NULL)
  {
    return 0;
  }
  return 1;
}

/*
 * Function    : QNXGF_end_post
 * Return Value: byte
 * Descriptions: end post
 */
byte RTOS_end_post(LIBAROMA_FBP me)
{
  if (me == NULL)
  {
    return 0;
  }

  return 1;
}

/*
 * Function : Save display canvas into framebuffer
 *
 */
byte RTOS_post(
    LIBAROMA_FBP me, wordp __restrict src,
    int dx, int dy, int dw, int dh,
    int sx, int sy, int sw, int sh)
{
  if (me == NULL)
  {
    return 0;
  }

  libaroma_mutex_lock(___rtosfbmutex);
  // RTOS_INTERNALP mi = (RTOS_INTERNALP) me->internal;
  // int sstride = (sw - dw) * 2;
  // int dstride = (mi->line - (dw * mi->pixsz));
  // wordp copy_dst =
  //   (wordp) (((bytep) mi->buffer)+(mi->line * dy)+(dx * mi->pixsz));
  // wordp copy_src = 
  //   (wordp) (src + (sw * sy) + sx);
  // libaroma_blt_align16(
  //   copy_dst,
  //   copy_src,
  //   dw, dh,
  //   dstride,
  //   sstride
  // );

  gdi_lcd_update_screen(dx, dy, dw, dh);
  libaroma_mutex_unlock(___rtosfbmutex);
  return 1;
}

/*
 * Function : libaroma init fb driver
 */
byte libaroma_fb_driver_init(LIBAROMA_FBP me)
{
  return RTOS_init(me);
}

#endif /* __libaroma_rtos_fb_driver_c__ */
