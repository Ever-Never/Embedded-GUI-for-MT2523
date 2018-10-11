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
 * Filename    : aroma_platform.h
 * Description : platform header
 *
 * + This is part of libaroma, an embedded ui toolkit.
 * + 06/04/15 - Author(s): Ahmad Amarullah
 *
 */
#ifndef __libaroma_aroma_internal_h__
#error "Include <aroma_internal.h> instead."
#endif
#ifndef __libaroma_platform_h__
#define __libaroma_platform_h__
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"  
#include "semphr.h" 

#define LIBAROMA_CONFIG_OS "RTOS"

struct _GLOBAL_QUEUE{
   QueueHandle_t msg_queue;
   QueueHandle_t input_queue;
} ;

typedef struct _GLOBAL_QUEUE GLOBAL_QUEUE;
typedef struct _GLOBAL_QUEUE * GLOBAL_QUEUEP;
GLOBAL_QUEUEP libaroma_get_global_queue();
void mutex_unlock( SemaphoreHandle_t lock);
void mutex_lock( SemaphoreHandle_t lock);

void global_queue_init() ;

/*
 * common platform wrapper
 */
#define libaroma_unlink(filename) f_unlink(filename) 
#define libaroma_sleep(ms) vTaskDelay(pdMS_TO_TICKS(ms))

/*
 * get tick count
 */
static inline long libaroma_tick()
{

  long tick = -1;

  // if (__get_IPSR() > 0)
  // {
  //   tick = xTaskGetTickCountFromISR() / pdMS_TO_TICKS(1);
  // }
  // else
  // {
    tick = xTaskGetTickCount() / pdMS_TO_TICKS(1); 
    //  tick = xTaskGetTickCount() ;
  // }
  return tick;
}

#ifndef LIBAROMA_PFLINUX_DUMMY_PTHREAD

  #define LIBAROMA_MUTEX SemaphoreHandle_t
  #define libaroma_mutex_init(x)  x = xSemaphoreCreateMutex() 
  #define libaroma_mutex_free(x) vSemaphoreDelete(x)
  #define libaroma_mutex_lock(x) mutex_lock(x) 
  #define libaroma_mutex_unlock(x) mutex_unlock(x) 
  typedef TaskHandle_t LIBAROMA_THREAD;
  #define libaroma_thread_create_rtos(th, cb, cookie, name)  xTaskCreate( cb, name,512,cookie, 3, th)
  #define libaroma_thread_create(th, cb, cookie)  xTaskCreate( cb, "default", 512,cookie, tskIDLE_PRIORITY, th)
  #define libaroma_thread_join(th) vTaskSuspend(th)
  #define libaroma_thread_detach(th)
  #define libaroma_thread_kill(th) vTaskDelete(th) 
  static inline void libaroma_thread_set_hiprio(LIBAROMA_THREAD t)
  {
    // vTaskPrioritySet(t,tskIDLE_PRIORITY + 1) ;
  }
  /*
    * CONDITION & MUTEX CONDITION - NOT NEED MULTICORE THREADSAFE
    */
  #define LIBAROMA_COND_MUTEX SemaphoreHandle_t
  #define LIBAROMA_COND SemaphoreHandle_t
  #define libaroma_cond_wait(c, m) mutex_lock(*c)
  #define libaroma_cond_signal(c) mutex_unlock(*c) 
  #define libaroma_cond_lock(m) mutex_lock(*m)
  #define libaroma_cond_unlock(m) mutex_unlock(*m) 
 //#define libaroma_cond_lock(m)
 //#define libaroma_cond_unlock(m)
  void libaroma_cond_init(LIBAROMA_COND *cond, LIBAROMA_COND_MUTEX *mutex);
  void libaroma_cond_free(LIBAROMA_COND *cond, LIBAROMA_COND_MUTEX *mutex);


#else
  #define LIBAROMA_MUTEX voidp
  #define libaroma_mutex_init(x)
  #define libaroma_mutex_free(x)
  #define libaroma_mutex_lock(x)
  #define libaroma_mutex_unlock(x)
  typedef voidp LIBAROMA_THREAD;
  #define libaroma_thread_create(th,cb,cookie)
  #define libaroma_thread_join(th)
  #define libaroma_thread_detach(th)
  #define libaroma_thread_kill(th)
  static inline void libaroma_thread_set_hiprio(LIBAROMA_THREAD t){
  }
  /*
   * CONDITION & MUTEX CONDITION - NOT NEED MULTICORE THREADSAFE
   */
  #define LIBAROMA_COND_MUTEX voidp
  #define LIBAROMA_COND voidp
  #define libaroma_cond_wait(c,m)
  #define libaroma_cond_signal(c)
  #define libaroma_cond_lock(m)
  #define libaroma_cond_unlock(m)
  void libaroma_cond_init(LIBAROMA_COND * cond, LIBAROMA_COND_MUTEX * mutex);
  void libaroma_cond_free(LIBAROMA_COND * cond, LIBAROMA_COND_MUTEX * mutex);
#endif
/*
 * File Management
 */
int libaroma_filesize(const char *filename);
int libaroma_filesize_fd(int fd);
 void lock_log() ;
 void unlock_log() ;
byte libaroma_file_exists(const char *filename);

#endif /* __libaroma_platform_h__ */
