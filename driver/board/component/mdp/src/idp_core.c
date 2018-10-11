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

#define THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL

#include "drv_features_mdp.h"
//#include "drv_comm.h"

#if defined(DRV_IDP_SUPPORT)

//#include "kal_general_types.h"
//#include "kal_public_defs.h"
//#include "idp_hisr.h"
#include "stdarg.h"
#include <idp_core.h>
//#include <idp_core_internal.h>

#if defined(DRV_IDP_6238_SERIES)
#include <mt6238/idp_core.h>
//#include <idp_sw_yuv_rotator.h> // for MT6268 VT
#endif

#if !defined(IDP_DVT_LOAD)
//#include <kal_release.h>
//#include "visualhisr.h"

/* for kal_get_current_thread_ID() */
//#include "rtfiles.h"
//#include "Fs_kal.h"
#endif

//#include "reg_base.h"

/* for rand() */
#include <stdlib.h>


#if defined(DRV_IDP_6235_SERIES)
#include <mt6235/idp_engines.h>
#endif

#if defined(DRV_IDP_MT6236_SERIES)
#include <mt6236/idp_engines.h>
#endif

#if defined(DRV_IDP_6252_SERIES)
#include <mt6252/idp_engines.h>
#endif

#if defined(DRV_IDP_6253_SERIES)
#include <mt6253/idp_engines.h>
#endif

#if defined(DRV_IDP_MT6276_SERIES) || defined(DRV_IDP_MT6276E2_SERIES)
#include <mt6276/idp_engines.h>
#endif

#if defined(DRV_IDP_MT6256_SERIES)
#include <mt6256_e2/idp_engines.h>
#endif

#if defined(DRV_IDP_MT6255_SERIES)
#include <mt6255/idp_engines.h>
#endif


kal_bool
idp_hw_open(
  idp_hw_owner_t * const owner)
{
  kal_bool result = KAL_TRUE;

  if (NULL == owner)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }
  //kal_take_sem(owner->sem, KAL_INFINITE_WAIT);

  if (owner->key != 0)
  {
    result = KAL_FALSE;
  }
  else
  {
    kal_uint32 local_key = 0;

    while (0 == local_key)
    {
      local_key = rand();
    }

    owner->key = local_key;
  }

  //kal_give_sem(owner->sem);

  return result;
}

kal_bool
idp_hw_close(
  idp_hw_owner_t * const owner,
  kal_uint32 const key)
{
  kal_bool result = KAL_TRUE;

  if (NULL == owner)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }

  //kal_take_sem(owner->sem, KAL_INFINITE_WAIT);

  if (owner->key != key)
  {
    result = KAL_FALSE;
  }
  else
  {
    owner->key = 0;
  }

  //kal_give_sem(owner->sem);

  return result;
}

kal_bool
idp_hw_can_be_used(
  idp_hw_owner_t * const owner,
  kal_uint32 const key)
{
  kal_bool result = KAL_TRUE;

  if (NULL == owner)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }

  /* Because this function may be called in LISR, I can not
   * call system API here.
   *
   * Because I expect users should 'open' the hardware
   * first for each data path, I can skip the semaphore API
   * here due to other codes will not touch the IDP
   * hardware (When they call xxx_open, they will fail).
   */
  /* kal_take_sem(owner->sem, KAL_INFINITE_WAIT); */

  if (owner->key != key)
  {
    result = KAL_FALSE;
  }


  /* kal_give_sem(owner->sem); */

  return result;
}

#define IDP_TRACED_BUFFER_SIZE (64)

#if !defined(L1_NOT_PRESENT)
extern kal_uint32 L1I_GetTimeStamp(void);
#endif  // #if !defined(L1_NOT_PRESENT)

#if 1//defined(__LOW_COST_SUPPORT_COMMON__)
kal_int32
idp_add_traced_begin(IdpTracedAPI const api)
{
  return 0;
}

void
idp_add_traced_end(IdpTracedAPI const api, kal_int32 const idx)
{
}

kal_int32
idp_add_traced_lisr(IdpTracedAPI const api)
{
  return 0;
}

kal_int32
idp_add_traced_custom(IdpTracedAPI const api, kal_uint32 param)
{
  return 0;
}

#else

IdpTraced g_idp_traced[IDP_TRACED_BUFFER_SIZE];
kal_int32 g_idp_traced_curr_idx = 0;

kal_int32
idp_add_traced_begin(IdpTracedAPI const api)
{
  kal_uint32 savedMask;
  kal_int32 my_idx;
#if defined(ENABLE_LOGGING_FOR_IS_BUSY)
  kal_int32 my_idx_minus_1;
  kal_int32 my_idx_minus_2;
#endif

#if !defined(ENABLE_LOGGING_FOR_IS_BUSY)
  switch (api)
  {
  case IDP_TRACED_API_camera_preview_IS_BUSY:
  case IDP_TRACED_API_camera_preview_with_face_detection_IS_BUSY:
  case IDP_TRACED_API_camera_capture_to_jpeg_IS_BUSY:
  case IDP_TRACED_API_camera_capture_to_mem_IS_BUSY:
  case IDP_TRACED_API_camera_capture_to_barcode_IS_BUSY:
  case IDP_TRACED_API_jpeg_decode_IS_BUSY:
  case IDP_TRACED_API_jpeg_encode_IS_BUSY:
  case IDP_TRACED_API_jpeg_resize_IS_BUSY:
  case IDP_TRACED_API_image_resize_IS_BUSY:
  case IDP_TRACED_API_rgb2yuv_IS_BUSY:
  case IDP_TRACED_API_video_editor_decode_IS_BUSY:
  case IDP_TRACED_API_video_editor_encode_IS_BUSY:
  case IDP_TRACED_API_video_decode_IS_BUSY:
  case IDP_TRACED_API_video_encode_IS_BUSY:
  case IDP_TRACED_API_webcam_IS_BUSY:
  case IDP_TRACED_API_video_call_decode_IS_BUSY:
  case IDP_TRACED_API_video_call_encode_IS_BUSY:
  case IDP_TRACED_API_simple_display_with_rotate_IS_BUSY:
  case IDP_TRACED_API_image_effect_pixel_IS_BUSY:
    return -1;
    //break;

  default:
    break;
  }
#endif

  savedMask = SaveAndSetIRQMask();

  {
    my_idx = g_idp_traced_curr_idx;

#if defined(ENABLE_LOGGING_FOR_IS_BUSY)
    // I don't want the same xxx_IS_BUSY fill out of my
    // g_idp_traced space.
    switch (my_idx)
    {
    case 0:
      my_idx_minus_1 = (IDP_TRACED_BUFFER_SIZE - 1);
      my_idx_minus_2 = (IDP_TRACED_BUFFER_SIZE - 2);
      break;

    case 1:
      my_idx_minus_1 = 0;
      my_idx_minus_2 = (IDP_TRACED_BUFFER_SIZE - 1);
      break;

    default:
      my_idx_minus_1 = (my_idx - 1);
      my_idx_minus_2 = (my_idx - 2);
      break;
    }

    switch (api)
    {
    case IDP_TRACED_API_camera_preview_IS_BUSY:
    case IDP_TRACED_API_camera_preview_with_face_detection_IS_BUSY:
    case IDP_TRACED_API_camera_capture_to_jpeg_IS_BUSY:
    case IDP_TRACED_API_camera_capture_to_mem_IS_BUSY:
    case IDP_TRACED_API_camera_capture_to_barcode_IS_BUSY:
    case IDP_TRACED_API_camera_capture_to_ybuffer_IS_BUSY:
    case IDP_TRACED_API_jpeg_decode_IS_BUSY:
    case IDP_TRACED_API_jpeg_encode_IS_BUSY:
    case IDP_TRACED_API_jpeg_resize_IS_BUSY:
    case IDP_TRACED_API_image_resize_IS_BUSY:
    case IDP_TRACED_API_rgb2yuv_IS_BUSY:
    case IDP_TRACED_API_video_editor_decode_IS_BUSY:
    case IDP_TRACED_API_video_editor_encode_IS_BUSY:
    case IDP_TRACED_API_video_decode_IS_BUSY:
    case IDP_TRACED_API_video_encode_IS_BUSY:
    case IDP_TRACED_API_webcam_IS_BUSY:
    case IDP_TRACED_API_video_call_decode_IS_BUSY:
    case IDP_TRACED_API_video_call_encode_IS_BUSY:
    case IDP_TRACED_API_simple_display_with_rotate_IS_BUSY:
    case IDP_TRACED_API_image_effect_pixel_IS_BUSY:
      if ((api == g_idp_traced[my_idx_minus_1].m_api) &&
          (api == g_idp_traced[my_idx_minus_2].m_api))
      {
        RestoreIRQMask(savedMask);
        return my_idx_minus_1;
      }
      break;

    default:
      break;
    }
#endif

    ++g_idp_traced_curr_idx;
    g_idp_traced_curr_idx &= (IDP_TRACED_BUFFER_SIZE - 1);
  }

  RestoreIRQMask(savedMask);

  g_idp_traced[my_idx].m_api = api;
  g_idp_traced[my_idx].m_who = kal_get_current_thread_ID();
  if (kal_if_hisr())
  {
    g_idp_traced[my_idx].m_call_by_which_level = IDP_CALL_BY_HISR;
  }
  else if (kal_if_lisr())
  {
    g_idp_traced[my_idx].m_call_by_which_level = IDP_CALL_BY_LISR;
  }
  else
  {
    g_idp_traced[my_idx].m_call_by_which_level = IDP_CALL_BY_TASK;
  }
  g_idp_traced[my_idx].m_done = KAL_FALSE;
  g_idp_traced[my_idx].m_start_time_drv = drv_get_current_time();

  return my_idx;
}

void
idp_add_traced_end(IdpTracedAPI const api, kal_int32 const idx)
{
  if ((idx < 0) || (idx >= IDP_TRACED_BUFFER_SIZE))
  {
    ASSERT(0);
  }

  if (g_idp_traced[idx].m_api != api)
  {
    ASSERT(0);
  }

  g_idp_traced[idx].m_done = KAL_TRUE;
  g_idp_traced[idx].m_done_time_drv = drv_get_current_time();
}

kal_int32
idp_add_traced_lisr(IdpTracedAPI const api)
{
  kal_uint32 savedMask;
  kal_uint32 register my_idx;

  savedMask = SaveAndSetIRQMask();
  my_idx = g_idp_traced_curr_idx;
  ++g_idp_traced_curr_idx;
  g_idp_traced_curr_idx &= (IDP_TRACED_BUFFER_SIZE - 1);
  RestoreIRQMask(savedMask);

  g_idp_traced[my_idx].m_api = api;
  g_idp_traced[my_idx].m_start_time_drv = drv_get_current_time();

  return my_idx;
}

kal_int32
idp_add_traced_custom(IdpTracedAPI const api, kal_uint32 param)
{
  kal_uint32 register my_idx;

  my_idx = idp_add_traced_lisr(api);

  g_idp_traced[my_idx].m_param = param;

  return my_idx;
}
#endif

static kal_semid idp_owners_sem;
static idp_owner_t owners[MAX_CONCURRENT_DP];

#if defined(__MTK_TARGET__) && defined(__DCM_WITH_COMPRESSION_MAUI_INIT__)
#pragma push
#pragma arm section code="DYNAMIC_COMP_MAUIINIT_SECTION"
#endif 
void
idp_init(void)
{
  kal_uint32 i;

  /* All operations based on this buffer will assume that the size of the buffer
   * is power of 2. This is to say that I have to ensure that the binary form of
   * buffer_size must contain only one one.
   */
  //ASSERT(1 == count_one_bit_number(IDP_TRACED_BUFFER_SIZE));

  /* Because I need to call semaphore API, I have to ensure
   * this function will not be called from HISR & LISR.
   */
   #if 0
  if (kal_if_hisr() || kal_if_lisr())
  {
    ASSERT(0);
  }
   #endif

  //idp_owners_sem = kal_create_sem("IDP", 1);

  for (i = 0; i < MAX_CONCURRENT_DP; ++i)
  {
    kal_mem_set(&(owners[i]), 0, sizeof(owners[i]));
  }

  idp_hw_init();

#if defined(IDP_HISR_SUPPORT)
  idp_hisr_init();
#endif

#if defined(DRV_IDP_6238_SERIES) || defined(DRV_IDP_MT6236_SERIES)
  // For MT6268 VT video frame rotation
  //idp_sw_yuv_rotator_init(); // for MT6268 VT
#endif  // #if defined(DRV_IDP_6238_SERIES) || defined(DRV_IDP_MT6236_SERIES)
}

#if defined(__MTK_TARGET__) && defined(__DCM_WITH_COMPRESSION_MAUI_INIT__)
#pragma arm section code
#pragma pop
#endif

kal_bool
idp_find_empty_and_register(
  kal_uint32 * const key,
  idp_owner_t ** const owner)
{
  kal_uint32 i;
  kal_bool result = KAL_FALSE;

  if (NULL == owner)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }

  /* Because I need to call semaphore API, I have to ensure
   * this function will not be called from HISR & LISR.
   */
#if 0
  if (kal_if_hisr())
  {
    //ASSERT(0);
    return KAL_FALSE;
  }
  if (kal_if_lisr())
  {
    //ASSERT(0);
    return KAL_FALSE;
  }
#endif

  //kal_take_sem(idp_owners_sem, KAL_INFINITE_WAIT);

  for (i = 0; i < MAX_CONCURRENT_DP; ++i)
  {
    //kal_bool find_one = KAL_FALSE;

    if (0 == owners[i].key)
    {
      kal_uint32 local_key;

      while (1)
      {
        kal_bool cal_again = KAL_FALSE;

        local_key = 0;

        while (0 == local_key)
        {
          local_key = rand();
        }

        {
          int j;

          for (j = 0; j < MAX_CONCURRENT_DP; ++j)
          {
            if (j != i)
            {
              if (owners[j].key == local_key)
              {
                cal_again = KAL_TRUE;
              }

              if (KAL_TRUE == cal_again)
              {
                break;
              }
            }
          }
        }

        if (KAL_FALSE == cal_again)
        {
          break;
        }
      }

      owners[i].key = local_key;
      (*key) = local_key;

      owners[i].task = 0x0; //kal_get_current_thread_ID();

      (*owner) = &(owners[i]);

      result = KAL_TRUE;

      //find_one = KAL_TRUE;
      break;
    }

    //if (KAL_TRUE == find_one)
    //{
    //  break;
    //}
  }

  //kal_give_sem(idp_owners_sem);

  return result;
}

kal_bool
idp_find(
  kal_uint32 const key,
  idp_owner_t ** const owner)
{
  kal_uint32 i;
  kal_bool result = KAL_FALSE;

  if (NULL == owner)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }

  /* Because this function may be called in LISR, I can not
   * call system API here.
   *
   * Because I expect users should 'open' the data path
   * first for each data path, I can skip the semaphore API
   * here due to other codes will not touch the IDP
   * data path (When they call xxx_open, they will fail).
   */
  /* kal_take_sem(idp_owners_sem, KAL_INFINITE_WAIT); */

  for (i = 0; i < MAX_CONCURRENT_DP; ++i)
  {
    if (key == owners[i].key)
    {
      (*owner) = &(owners[i]);

      result = KAL_TRUE;
    }
  }

  /* kal_give_sem(idp_owners_sem); */

  return result;
}

kal_bool
idp_close(
  kal_uint32 const key,
  idp_close_hook_t const hook)
{
  kal_bool result;
  idp_owner_t *owner = NULL;

  result = idp_find(key, &owner);
  if (KAL_FALSE == result)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }

  result = hook(owner);

  owner->key = 0;

  /* Because I need to call semaphore API, I have to ensure
   * this function will not be called from HISR & LISR.
   */
#if 0
  if (kal_if_hisr())
  {
    //ASSERT(0);
    return KAL_FALSE;
  }

  if (kal_if_lisr())
  {
    //ASSERT(0);
    return KAL_FALSE;
  }
#endif

#if ((!defined(DRV_IDP_6252_SERIES)) && (!defined(DRV_IDP_6253_SERIES)))
  // To see if there is no one use IDP.
  {
    kal_uint32 i;
    kal_bool can_poweroff = KAL_TRUE;

    // lock all possible data paths.
    kal_take_sem(idp_owners_sem, KAL_INFINITE_WAIT);

    for (i = 0; i < MAX_CONCURRENT_DP; ++i)
    {
      if (owners[i].key != 0)
      {
        // There at least one uses IDP now, so I can not
        // power off IDP.
        can_poweroff = KAL_FALSE;
      }
    }

    if (KAL_TRUE == can_poweroff)
    {
#if defined(IDP_DRVPDN_SUPPORT)
      turn_off_idp_power();
#endif
    }

    // unlock all possible data paths.
    kal_give_sem(idp_owners_sem);
  }
#endif // ((!defined(DRV_IDP_6252_SERIES))...

  return KAL_TRUE;
}

kal_bool
idp_stop(
  kal_uint32 const key,
  idp_stop_hook_t const hook)
{
  kal_bool result;
  idp_owner_t *owner = NULL;

  result = idp_find(key, &owner);
  if (KAL_FALSE == result)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }
  result = hook(owner);
  if (KAL_FALSE == result)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }

  return KAL_TRUE;
}

kal_bool
idp_open(
  kal_uint32 * const key,
  idp_scenario_t const scenario,
  idp_open_hook_t const open_hook,
  idp_close_hook_t const close_hook,
  idp_init_config_hook_t const init_config_hook)
{
  kal_bool result;
  idp_owner_t *owner = NULL;

  result = idp_find_empty_and_register(key, &owner);
  if (KAL_FALSE == result)
  {
    return KAL_FALSE; // [MAUI_01656816] Per Bear's request, remove ASSERTion but return error code here
    //ASSERT(0);
  }

  switch (scenario)
  {
  case IDP_SCENARIO_camera_preview:
  case IDP_SCENARIO_camera_preview_with_face_detection:
  case IDP_SCENARIO_camera_capture_to_jpeg:
  case IDP_SCENARIO_camera_capture_to_mem:
  case IDP_SCENARIO_camera_capture_to_barcode:
  case IDP_SCENARIO_camera_capture_to_ybuffer:
  case IDP_SCENARIO_video_decode:
  case IDP_SCENARIO_video_encode:
#if defined(DRV_IDP_MT6256_E2_SERIES)
  case IDP_SCENARIO_mjpeg_encode:
#endif
  case IDP_SCENARIO_video_call_encode:
  case IDP_SCENARIO_video_call_decode:
  case IDP_SCENARIO_video_editor_encode:
  case IDP_SCENARIO_video_editor_decode:
  case IDP_SCENARIO_rgb2yuv:
  case IDP_SCENARIO_jpeg_decode:
  case IDP_SCENARIO_jpeg_encode:
  case IDP_SCENARIO_jpeg_resize:
  case IDP_SCENARIO_image_effect_pixel:
  case IDP_SCENARIO_image_resize:
  case IDP_SCENARIO_webcam:
  case IDP_SCENARIO_simple_display_with_rotate:
    break;

  default:
    return KAL_FALSE;
    //ASSERT(0);
    //break;
  }

  owner->scenario = scenario;


#if defined(DRV_IDP_6252_SERIES)
    owner->irt0_key = 0;
    owner->crz_key = 0;
#elif defined(DRV_IDP_6253_SERIES) || defined(DRV_IDP_6235_SERIES)
    owner->crz_key = 0;
#else
     {
       kal_uint32 key = owner->key;
       kal_taskid task = owner->task;
       idp_scenario_t scenario = owner->scenario;
  
       kal_mem_set(owner, 0, sizeof(idp_owner_t));
  
       owner->key = key;
       owner->task = task;
       owner->scenario = scenario;
     }
#endif

  result = open_hook(owner);

  if (KAL_TRUE == result)
  {
    init_config_hook(owner);

    //return KAL_TRUE;
  }
  else
  {
    idp_close(*key, close_hook);

    (*key) = 0; // [MAUI_01366535] error handling instead of ASSERTion

    //return KAL_FALSE;
  }

  return result;
}

kal_bool
idp_is_in_use(idp_is_in_use_hook_t const hook)
{
  return hook();
}

kal_bool
idp_config_fast(
  kal_uint32 const key,
  idp_read_user_config_fast_hook_t const read_user_config_fast_hook,
  kal_uint32 const para_type,
  kal_uint32 const para_value)
{
  kal_bool result;
  idp_owner_t *owner = NULL;

  result = idp_find(key, &owner);
  if (KAL_FALSE == result)
  {
    return KAL_FALSE;
    //ASSERT(0);
  }
  if (KAL_FALSE == read_user_config_fast_hook(owner, para_type, para_value))
  {
    return KAL_FALSE;
  }
  return KAL_TRUE;
}

kal_bool
idp_config(
  kal_uint32 const key,
  idp_read_user_config_hook_t const read_user_config_hook,
  idp_finish_read_user_config_hook_t const finish_read_user_config_hook,
  idp_hardware_setting_have_been_changed_hook_t const hardware_setting_have_been_changed_hook,
  idp_config_hook_t const config_hook,
  idp_clear_setting_diff_hook_t const clear_setting_diff_hook,
  idp_is_setting_diff_is_ok_when_busy_hook_t const is_setting_diff_is_ok_when_busy_hook,
  idp_is_busy_hook_t const is_busy_hook,
  idp_stop_hook_t const stop_hook,
  va_list ap)
{
  kal_bool result;
  idp_owner_t *owner = NULL;

  result = idp_find(key, &owner);
  if (KAL_FALSE == result)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }
  if (KAL_FALSE == read_user_config_hook(owner, ap))
  {
    return KAL_FALSE;
  }

  if ((KAL_TRUE == finish_read_user_config_hook()) &&
      (KAL_TRUE == hardware_setting_have_been_changed_hook(owner)))
  {

#if ((!defined(DRV_IDP_6252_SERIES)) && (!defined(DRV_IDP_6253_SERIES)))
    if (KAL_FALSE == is_setting_diff_is_ok_when_busy_hook(owner))
    {
      kal_bool busy;

      result = is_busy_hook(owner, &busy);
      if (KAL_TRUE == busy)
      {
        //ASSERT(0);

        return KAL_FALSE;
      }
#if 1
      //result = stop_for_config_hook(owner);
      result = stop_hook(owner);
#endif
    }
#endif // ((!defined(DRV_IDP_6252_SERIES))...


    result = config_hook(owner, KAL_TRUE);
    if (KAL_FALSE == result)
    {
      return result;
    }
  }

#if ((!defined(DRV_IDP_6252_SERIES)) && (!defined(DRV_IDP_6253_SERIES)))
  clear_setting_diff_hook(owner);
#endif

  return KAL_TRUE;
}

kal_bool
idp_configurable(
    kal_uint32 const key,
    idp_read_user_config_hook_t const read_user_config_hook,
    idp_finish_read_user_config_hook_t const finish_read_user_config_hook,
    idp_config_hook_t const config_hook,
    va_list ap)
{
  kal_bool result;
  idp_owner_t *owner = NULL;

  result = idp_find(key, &owner);
  if (KAL_FALSE == result)
  {
    //SSERT(0);
    return KAL_FALSE;
  }

  if (KAL_FALSE == read_user_config_hook(owner, ap))
  {
    return KAL_FALSE;
  }

  if (KAL_TRUE == finish_read_user_config_hook())
  {
    result = config_hook(owner, KAL_FALSE);
    if (KAL_FALSE == result)
    {
      return result;
    }
  }

  return KAL_TRUE;
}

kal_bool
idp_query(
  kal_uint32 const key,
  idp_query_hook_t const query_hook,
  va_list ap)
{
  kal_bool result;
  idp_owner_t *owner = NULL;

  result = idp_find(key, &owner);
  if (KAL_FALSE == result)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }
  query_hook(owner, ap);

  return KAL_TRUE;
}

kal_bool
idp_start(
  kal_uint32 const key,
  idp_finish_read_user_config_hook_t const finish_read_user_config_hook,
  idp_start_hook_t const start_hook)
{
  kal_bool result;
  idp_owner_t *owner = NULL;

  result = idp_find(key, &owner);
  if (KAL_FALSE == result)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }

  if (KAL_TRUE == finish_read_user_config_hook())
  {
    result = start_hook(owner);
    if (KAL_FALSE == result)
    {
      //ASSERT(0);
      return KAL_FALSE;
    }

    return KAL_TRUE;
  }
  else
  {
    //ASSERT(0);

    return KAL_FALSE;
  }
}

kal_bool
idp_is_busy(
  kal_uint32 const key,
  kal_bool * const busy,
  idp_is_busy_hook_t const is_busy_hook)
{
  kal_bool result;
  idp_owner_t *owner = NULL;

  if (NULL == busy)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }

  result = idp_find(key, &owner);
  if (KAL_FALSE == result)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }
  
  result = is_busy_hook(owner, busy);

  return KAL_TRUE;
}

void
idp_calc_config_attr_bit_pos(
  kal_uint32 const attr_idx,
  kal_uint32 * const word_pos,
  kal_uint32 * const bit_pos)
{
  (*word_pos) = (attr_idx / 32);
  (*bit_pos) = (attr_idx % 32);
}

kal_bool
idp_finish_read_user_config(
  kal_uint32 const last_attr_idx,
  kal_uint32 const * const read_user_config)
{
  kal_uint32 word_pos, bit_pos;
  kal_uint32 i;
  kal_uint32 remainder_value = 0;

  if (NULL == read_user_config)
  {
    //ASSERT(0);
    return KAL_FALSE;
  }

  word_pos = (last_attr_idx / 32);
  bit_pos = (last_attr_idx % 32);

  for (i = 0; i < word_pos; ++i)
  {
    if (read_user_config[i] != 0xFFFFFFFF)
    {
      return KAL_FALSE;
    }
  }

  while (bit_pos != 0)
  {
    remainder_value |= (1 << (bit_pos - 1));

    --bit_pos;
  }

  if ((read_user_config[word_pos] & remainder_value) != remainder_value)
  {
    return KAL_FALSE;
  }

  return KAL_TRUE;
}

void
idp_line_count_ratio_init(idp_line_count_ratio_t * const data,
                          kal_uint32 const size)
{
  if (0 == data)
  {
    ASSERT(0);
  }

  data->ratio_size = size;
  data->victim_idx = (size - 1);
}

void
idp_line_count_ratio_minus_one(idp_line_count_ratio_t * const data)
{
  if (0 == data)
  {
    ASSERT(0);
  }

  --(data->ratio[data->victim_idx]);
  if (0 == data->ratio[data->victim_idx])
  {
    ASSERT(0);
  }

  if (data->victim_idx > 0)
  {
    --(data->victim_idx);

    if (0 == data->victim_idx)
    {
      data->victim_idx = (data->ratio_size - 1);
    }
  }
}

kal_bool
IS_BIT_SET(kal_uint32 const * const location,
           kal_uint32 const para_type)
{
  kal_uint32 word_pos, bit_pos;

  idp_calc_config_attr_bit_pos(para_type, &word_pos, &bit_pos);

  if ((location[word_pos] & (1 << bit_pos)) != 0)
  {
    return KAL_TRUE;
  }
  else
  {
    return KAL_FALSE;
  }
}

void
_idp_set_bit(kal_uint32* location,kal_uint32 len ,kal_uint32 para_type)
{
  kal_uint32 word_pos, bit_pos;

  idp_calc_config_attr_bit_pos(para_type, &word_pos, &bit_pos);

  if (word_pos < (len / sizeof(kal_uint32)))
  {
    location[word_pos] |= (1 << bit_pos);
  }else
  {
    ASSERT(0);
  }
}

void
_idp_clear_bit(kal_uint32* location,kal_uint32 len ,kal_uint32 para_type)
{
    kal_uint32 word_pos, bit_pos;

    idp_calc_config_attr_bit_pos(para_type, &word_pos, &bit_pos);

    if (word_pos < (len / sizeof(kal_uint32)))
    {
      location[word_pos] &= ~(1 << bit_pos);
    }else
    {
      ASSERT(0);
    }
}


#if ((!defined(DRV_IDP_6252_SERIES)) && (!defined(DRV_IDP_6253_SERIES)))
volatile kal_uint32 _video_call_decode_idp_key = 0;
volatile PFN_TRIGGER_VIDEO_CALL_DECODE_PATH pf_trigger_video_call_decode_path = NULL;
volatile PFN_TRIGGER_VIDEO_CALL_DECODE_PATH pf_trigger_video_call_encode_display_path = NULL;


void idp_video_call_query_key(kal_uint32* key)
{
    *key = _video_call_decode_idp_key;
}

void idp_set_video_call_decode_cb(PFN_TRIGGER_VIDEO_CALL_DECODE_PATH video_call_decode_trigger_fn)
{
    pf_trigger_video_call_decode_path = video_call_decode_trigger_fn;
}
void idp_trigger_video_call_encode_display_cb()
{
  if (pf_trigger_video_call_encode_display_path!=NULL)
  {
    (*pf_trigger_video_call_encode_display_path)(NULL);
  }
}
#endif // ((!defined(DRV_IDP_6252_SERIES))...

void idp_common_check(IdpCheckReentrant* idp_check_reentrant)
{
  //kal_uint32 savedMask;
  //kal_bool result;
  //kal_int32 idp_traced_idx;
  
  //savedMask = SaveAndSetIRQMask();
  if (KAL_TRUE == idp_check_reentrant->idp_check_reentrant_flag)
  {
    ASSERT(0);
  }
  idp_check_reentrant->idp_check_reentrant_flag = KAL_TRUE;
  //RestoreIRQMask(savedMask);
}

void idp_common_check_end(IdpCheckReentrant* idp_check_reentrant,IdpTracedAPI const api, kal_int32 idx)
{
  //kal_uint32 savedMask;

  //savedMask = SaveAndSetIRQMask();
  idp_check_reentrant->idp_check_reentrant_flag = KAL_FALSE;
  //RestoreIRQMask(savedMask);

  if (idx != -1)
  {
    idp_add_traced_end(api, idx);
  }

}
#if defined(THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL)
#if defined(DRV_IDP_MT6236_SERIES) || defined(DRV_IDP_6238_SERIES) || defined(DRV_IDP_6235_SERIES) || defined(DRV_IDP_MT6276E2_SERIES) || defined(DRV_IDP_MT6256_E2_SERIES) || defined(DRV_IDP_MT6255_SERIES)
#if defined(__MTK_TARGET__)

  extern kal_bool INT_QueryExceptionStatus(void);


void idp_internal_crz_usel_dsel(kal_uint32 scenario, kal_uint32 source_w, kal_uint32 source_h, kal_uint32 target_w, kal_uint32 target_h, kal_uint32* usel, kal_uint32* dsel)
{
  idp_custom_crz_usel_dsel(((CUSTOM_SCENARIO_ID)scenario), (source_w), (source_h), (target_w), (target_h), (usel), (dsel));
  if ((INT_QueryExceptionStatus() == KAL_FALSE) && (kal_if_hisr() == KAL_FALSE) && (kal_if_lisr() == KAL_FALSE))
  {kal_trace(TRACE_INFO, IDP_CUSTOM_CRZ_U_D, (scenario), (source_w), (source_h), (target_w), (target_h), *(usel), *(dsel));}
}
#endif  // #if defined(__MTK_TARGET__)
#endif  // #if defined(DRV_IDP_MT6236_SERIES) || defined(DRV_IDP_6238_SERIES)
#endif  // #if defined(THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL)

#if 0 //!defined(__MTK_TARGET__)
kal_bool
idp_scenario_open(
  kal_uint32 * const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  idp_scenario_t const scenario,
  hook_collect_t* hook_collect)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_is_in_use(
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_close(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_stop(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_start_intermedia_pipe(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_start_input(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_start_output(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  return KAL_TRUE;
}


kal_bool
idp_scenario_start_all(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  return KAL_TRUE;
}


kal_bool
idp_scenario_is_busy(
  kal_uint32 const key,
  kal_bool * const busy,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_config_fast(
  kal_uint32 const key,
  kal_uint32 const para_type,
  kal_uint32 const para_value,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_config(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_configurable(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_query(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap)
{
  return KAL_TRUE;
}

kal_bool
idp_scenario_config_and_start(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap)
{
  return KAL_TRUE;
}

#else
kal_bool
idp_scenario_open(
  kal_uint32 * const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  idp_scenario_t const scenario,
  hook_collect_t* hook_collect)
{
  kal_bool result;
  kal_int32 idp_traced_idx;
  
  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_OPEN;
  
  idp_traced_idx = idp_add_traced_begin(api);
  
  result = idp_open(key,
                    scenario,
                    hook_collect->idp_open_hook,
                    hook_collect->idp_close_hook,
                    hook_collect->idp_init_config_hook);
  
  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}

kal_bool
idp_scenario_is_in_use(
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  kal_bool result;
  kal_int32 idp_traced_idx;
  
  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_IS_IN_USE;
  
  idp_traced_idx = idp_add_traced_begin(api);
  
  result = idp_is_in_use(hook_collect->idp_is_in_use_hook);
  
  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
  }

kal_bool
idp_scenario_close(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  kal_bool result = KAL_TRUE;
  kal_int32 idp_traced_idx;

  if (0 == key)
  {
    return KAL_FALSE;
  }

  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_CLOSE;

  idp_traced_idx = idp_add_traced_begin(api);

  result &= idp_stop(key, hook_collect->idp_stop_hook);
  result &= idp_close(key, hook_collect->idp_close_hook);

  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}

kal_bool
idp_scenario_stop(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  kal_bool result;
  kal_int32 idp_traced_idx;

  if (0 == key)
  {
    return KAL_FALSE;
  }

  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_STOP;

  idp_traced_idx = idp_add_traced_begin(api);

  result = idp_stop(key, hook_collect->idp_stop_hook);

  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}

kal_bool
idp_scenario_start_intermedia_pipe(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  kal_bool result;
  kal_int32 idp_traced_idx;

  if (0 == key)
  {
    return KAL_FALSE;
  }

  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_START_INTER;

  idp_traced_idx = idp_add_traced_begin(api);

  result = idp_start(key,
                     hook_collect->idp_finish_read_user_config_hook,
                     hook_collect->idp_start_intermedia_pipe_hook);

  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}

kal_bool
idp_scenario_start_input(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  kal_bool result;
  kal_int32 idp_traced_idx;

  if (0 == key)
  {
    return KAL_FALSE;
  }

  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_START_INPUT;

  idp_traced_idx = idp_add_traced_begin(api);

  result = idp_start(key,
                     hook_collect->idp_finish_read_user_config_hook,
                     hook_collect->idp_start_input_hook);

  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}

kal_bool
idp_scenario_start_output(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{
  kal_bool result;
  kal_int32 idp_traced_idx;

  if (0 == key)
  {
    return KAL_FALSE;
  }

  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_START_OUTPUT;

  idp_traced_idx = idp_add_traced_begin(api);

  result = idp_start(key,
                     hook_collect->idp_finish_read_user_config_hook,
                     hook_collect->idp_start_output_hook);

  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}


kal_bool
idp_scenario_start_all(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{

  kal_bool result = KAL_TRUE;
  kal_int32 idp_traced_idx;

  if (0 == key)
  {
    return KAL_FALSE;
  }

  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_START_ALL;

  idp_traced_idx = idp_add_traced_begin(api);


  result &= idp_start(key,
                      hook_collect->idp_finish_read_user_config_hook,
                      hook_collect->idp_start_output_hook);

  result &= idp_start(key,
                      hook_collect->idp_finish_read_user_config_hook,
                      hook_collect->idp_start_intermedia_pipe_hook);

  result &= idp_start(key,
                      hook_collect->idp_finish_read_user_config_hook,
                      hook_collect->idp_start_input_hook);

  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}


kal_bool
idp_scenario_is_busy(
  kal_uint32 const key,
  kal_bool * const busy,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{

  kal_bool result;
  kal_int32 idp_traced_idx;

  if (0 == key)
  {
    return KAL_FALSE;
  }

  idp_traced_idx = idp_add_traced_begin(api);

  result = idp_is_busy(key,
                       busy,
                       hook_collect->idp_is_busy_hook);

  if (idp_traced_idx != -1)
  {
    idp_add_traced_end(api, idp_traced_idx);

  }
  return result;
}

kal_bool
idp_scenario_config_fast(
  kal_uint32 const key,
  kal_uint32 const para_type,
  kal_uint32 const para_value,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect)
{

  kal_bool result;
  kal_int32 idp_traced_idx;

  if (0 == key)
  {
    return KAL_FALSE;
  }

  idp_traced_idx = idp_add_traced_begin(api);

  result = idp_config_fast(key,
                           hook_collect->idp_read_user_config_fast_hook,
                           para_type,
                           para_value);

  if (idp_traced_idx != -1)
  {
    idp_add_traced_end(api, idp_traced_idx);
  }
  return result;
}

kal_bool
idp_scenario_config(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap)
{

  kal_bool result;
  kal_int32 idp_traced_idx;

  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_CONFIG;

  idp_traced_idx = idp_add_traced_begin(api);

  result = idp_config(key,
                      hook_collect->idp_read_user_config_hook,
                      hook_collect->idp_finish_read_user_config_hook,
                      hook_collect->idp_hardware_setting_have_been_changed_hook,
                      hook_collect->idp_config_hook,
                      hook_collect->idp_clear_setting_diff_hook,
                      hook_collect->idp_is_setting_diff_is_ok_when_busy_hook,
                      hook_collect->idp_is_busy_hook,
                      hook_collect->idp_stop_hook,
                      ap);

  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}

kal_bool
idp_scenario_configurable(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap)
{

  kal_bool result;
  kal_int32 idp_traced_idx;

  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = (IdpCheckReentrantPrevAPI) IDP_TRACED_API_camera_preview_CONFIGURABLE;

  idp_traced_idx = idp_add_traced_begin(api);

  result = idp_configurable(key,
           hook_collect->idp_read_user_config_hook,
           hook_collect->idp_finish_read_user_config_hook,
           hook_collect->idp_config_hook,
           ap);

  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}

kal_bool
idp_scenario_query(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap)
{

  kal_bool result;
  kal_int32 idp_traced_idx;

  //No need to check reentrant since query function does not affect scenario settings
  //idp_common_check(idp_check_reentrant);
  //idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_QUERY;

  idp_traced_idx = idp_add_traced_begin(api);

  result = idp_query(key,
                     hook_collect->idp_query_hook,
                     ap);

  //idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}

kal_bool
idp_scenario_config_and_start(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap)
{

  kal_bool result = KAL_TRUE;
  kal_int32 idp_traced_idx;

  idp_common_check(idp_check_reentrant);
  idp_check_reentrant->idp_check_reentrant_prev_api = IDP_CHECK_REENTRANT_PREV_CONFIG_AND_START;

  idp_traced_idx = idp_add_traced_begin(api);

  result &= idp_config(key,
                      hook_collect->idp_read_user_config_hook,
                      hook_collect->idp_finish_read_user_config_hook,
                      hook_collect->idp_hardware_setting_have_been_changed_hook,
                      hook_collect->idp_config_hook,
                      hook_collect->idp_clear_setting_diff_hook,
                      hook_collect->idp_is_setting_diff_is_ok_when_busy_hook,
                      hook_collect->idp_is_busy_hook,
                      hook_collect->idp_stop_hook,
                      ap);

  result &= idp_start(key,
                      hook_collect->idp_finish_read_user_config_hook,
                      hook_collect->idp_start_output_hook);

  result &= idp_start(key,
                      hook_collect->idp_finish_read_user_config_hook,
                      hook_collect->idp_start_intermedia_pipe_hook);

  result &= idp_start(key,
                      hook_collect->idp_finish_read_user_config_hook,
                      hook_collect->idp_start_input_hook);

  idp_common_check_end(idp_check_reentrant,api,idp_traced_idx);
  return result;
}


#endif //#if !defined(__MTK_TARGET__)
#endif // #if defined(DRV_IDP_SUPPORT)
