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
 * Filename    : platform.c
 * Description : linux system specific source
 *
 * + This is part of libaroma, an embedded ui toolkit.
 * + 06/04/15 - Author(s): Ahmad Amarullah
 *
 */
#ifndef __libaroma_platform_c__
#define __libaroma_platform
#include <aroma_internal.h>
#include "ff.h"
#include "message_map_queue.h"

LIBAROMA_MUTEX _libaroma_wm_log_mutex;
static GLOBAL_QUEUEP global_queue_p ;

void lock_log()
{
  libaroma_mutex_lock(_libaroma_wm_log_mutex);
}

GLOBAL_QUEUEP libaroma_get_global_queue(){
  return global_queue_p ;
}

void unlock_log()
{
  libaroma_mutex_unlock(_libaroma_wm_log_mutex);
}
void libaroma_cond_init(
    LIBAROMA_COND *cond, LIBAROMA_COND_MUTEX *mutex)
{
  *cond = xSemaphoreCreateBinary();
  *mutex = xSemaphoreCreateMutex();
}
void libaroma_cond_free(
    LIBAROMA_COND *cond, LIBAROMA_COND_MUTEX *mutex)
{
  vSemaphoreDelete(*cond);
  vSemaphoreDelete(*mutex);
}

void global_queue_init()
{
   global_queue_p = (GLOBAL_QUEUEP)calloc(sizeof(GLOBAL_QUEUE),1);
   global_queue_p->msg_queue = xQueueCreate(10,  sizeof(MESSAGE_T));

}

void mutex_unlock(SemaphoreHandle_t lock)
{
  // portBASE_TYPE xHigherPriorityTaskWoken;
  // xHigherPriorityTaskWoken = pdFALSE;
  // if (__get_IPSR() > 0)
  // {

  //   xSemaphoreGiveFromISR(lock,&xHigherPriorityTaskWoken);
  //   portEND_SWITCHING_ISR(xHigherPriorityTaskWoken );
  // }
  // else
  {
    xSemaphoreGive(lock);
  }
}

void mutex_lock(SemaphoreHandle_t lock)
{
  // portBASE_TYPE xHigherPriorityTaskWoken;
  // xHigherPriorityTaskWoken = pdFALSE;
  // if (__get_IPSR() > 0)
  // {

  //   xSemaphoreTakeFromISR(lock,&xHigherPriorityTaskWoken);
  //   portEND_SWITCHING_ISR(xHigherPriorityTaskWoken );
  // }
  // else
  {
    xSemaphoreTake(lock, portMAX_DELAY);
  }
}

/* get filesize */
int libaroma_filesize(const char *filename)
{
  FILINFO st;
  if (f_stat(filename, &st) != FR_OK)
  {
    return -1;
  }
  return st.fsize;
}

/* get filesize from fd */
int libaroma_filesize_fd(int fd)
{

  return -1;
}

/* file exists */
byte libaroma_file_exists(const char *filename)
{
  FILINFO st;
  if (f_stat(filename, &st) != FR_OK)
  {
    return 0;
  }
  return 1;
}

#endif /* __libaroma_platform */
