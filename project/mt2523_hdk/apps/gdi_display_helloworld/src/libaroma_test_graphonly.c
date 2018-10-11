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
 * Filename    : libaroma_test_graphonly.c
 * Description : libaroma test file
 *
 * + This is part of libaroma, an embedded ui toolkit.
 * + 04/02/15 - Author(s): Ahmad Amarullah
 *
 */
#ifndef __libaroma_test_graphonly_c__
#define __libaroma_test_graphonly_c__
extern const unsigned char LiberationSansNarrow_Regular[401540];
/* libaroma header */
#include <aroma.h>
#include <stdio.h>
#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "syslog.h"

#define MYPI 3.14159265358979323846

static LIBAROMA_CANVASP dc; /* display canvas */

log_create_module(LIBAROMA, PRINT_LEVEL_INFO);
/* init */
byte init_aroma()
{






  return 1;
}
/* cleanup */
void release_aroma()
{
  libaroma_fb_release();
}

void getLoacationOnCircle(int cx, int cy, int radius, int angle, int *outx,
                          int *outy)
{
  float tmp = angle * (2 * MYPI / 360);
  if (angle >= 0 && angle <= 90)
  {
    *outx = cx + radius * sin(tmp);
    *outy = cy - radius * cos(tmp);
  }
  else if (angle > 90 && angle <= 180)
  {
    tmp = tmp - MYPI / 2;
    *outx = cx + radius * cos(tmp);
    *outy = cy + radius * sin(tmp);
  }
  else if (angle > 180 && angle <= 270)
  {
    tmp = tmp - MYPI;
    *outx = cx - radius * sin(tmp);
    *outy = cy + radius * cos(tmp);
  }
  else if (angle > 270 && angle <= 360)
  {
    tmp = tmp - (MYPI + MYPI / 2);
    *outx = cx - radius * cos(tmp);
    *outy = cy - radius * sin(tmp);
  }
  //printf("loacation on x:%d  y:%d  \n", *outx, *outy);
}

void button_draw(const char *text, int x, int y, int w, int h)
{
  word colorAccent = RGB(ff8800);
  /* draw text */
  LIBAROMA_TEXT textp = libaroma_text(
      text,
      colorAccent,
      w - libaroma_dp(16),
      LIBAROMA_FONT(1, 4) |
          LIBAROMA_TEXT_SINGLELINE |
          LIBAROMA_TEXT_CENTER |
          LIBAROMA_TEXT_FIXED_INDENT |
          LIBAROMA_TEXT_FIXED_COLOR |
          LIBAROMA_TEXT_NOHR,
      100);
  int ty = y + (h >> 1) - ((libaroma_text_height(textp) >> 1));
  libaroma_text_draw(dc, textp, x + libaroma_dp(8), ty);
  libaroma_text_free(textp);
}
int button_width(const char *text)
{
  /* draw text */
  LIBAROMA_TEXT textp = libaroma_text(
      text,
      0,
      dc->w,
      LIBAROMA_FONT(1, 4) |
          LIBAROMA_TEXT_SINGLELINE |
          LIBAROMA_TEXT_CENTER |
          LIBAROMA_TEXT_FIXED_INDENT |
          LIBAROMA_TEXT_FIXED_COLOR |
          LIBAROMA_TEXT_NOHR,
      100);
  int w = libaroma_dp(8) + libaroma_text_width(textp) + libaroma_dp(8);
  libaroma_text_free(textp);
  return w;
}

int testFb()
{
  if (init_aroma())
  {

    int sec = 1;
    int sz = dc->w >> 1;
    int outx, outy;
    while (1)
    {
      if (sec > 60)
      {
        sec = 0;
      }
      else
      {
        sec++;
      }
      libaroma_canvas_blank(dc);

      libaroma_draw_circle(dc, RGB(ff0000), sz, sz, sz - 130, 0xff);
      libaroma_draw_circle(dc, RGB(00ff00), sz, sz, sz - 150, 0xff);
      libaroma_draw_arc(dc, sz, sz, sz, sz, 15, 0, 360, RGB(0000ff),
                        0x80, 0, 0.8);

      getLoacationOnCircle(sz, sz, sz, 12 * 1.0 / 12 * 360,
                           &outx, &outy);
      libaroma_draw_line(dc, sz, sz, outx, outy, 13, RGB(342756),
                         0x7f, 0);
      getLoacationOnCircle(sz, sz, sz, 10 * 1.0 / 60 * 360,
                           &outx, &outy);
      libaroma_draw_line(dc, sz, sz, outx, outy, 8, RGB(348856),
                         0x7f, 0);
      getLoacationOnCircle(sz, sz, sz - 15,
                           sec * 1.0 / 60 * 360, &outx, &outy);
      libaroma_draw_line(dc, sz, sz, outx, outy, 5, RGB(349999),
                         0x7f, 0);

      libaroma_draw_circle(dc, RGB(ff00ff), sz, sz, 20, 0xff);

      libaroma_draw_text(
          dc,
          "AROMA Bootloader",
          0, libaroma_dp(2), RGB(ffffff), dc->w,
          LIBAROMA_FONT(0, 3) | LIBAROMA_TEXT_CENTER,
          100);

      libaroma_sync();

      vTaskDelay(pdMS_TO_TICKS(1));
    }

    release_aroma();
  }

  return 0;
} /* End of main */

#endif /* __libaroma_test_graphonly_c__ */
