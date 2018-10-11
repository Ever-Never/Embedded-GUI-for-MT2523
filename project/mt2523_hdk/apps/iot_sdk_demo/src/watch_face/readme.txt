/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its
 * licensors. Without the prior written permission of MediaTek and/or its
 * licensors, any reproduction, modification, use or disclosure of MediaTek
 * Software, and information contained herein, in whole or in part, shall be
 * strictly prohibited. You may only use, reproduce, modify, or distribute
 * (as applicable) MediaTek Software if you have agreed to and been bound by
 * the applicable license agreement with MediaTek ("License Agreement") and
 * been granted explicit permission to do so within the License Agreement
 * ("Permitted User"). If you are not a Permitted User, please cease any
 * access or use of MediaTek Software immediately.
 */

/**
 * @addtogroup mt2523_hdk mt2523_hdk
 * @{
 * @addtogroup mt2523_hdk_apps apps
 * @{
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo iot_sdk_demo
 * @{
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo_watch_face watch_face
 * @{

@par Overview
  - Application description
    - The application implements a watch screen that updates the time in low
      power mode. The time update happens every second by RTC notification.
      The LCM partially updates the time context part. Between the interval
      of the time update, the system enters the low power mode. The system
      wakes up by the RTC notification to enter the normal mode. After
      updating the current time, system enters the low power mode again.

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK

@par Directory contents
  - Source and header files
    - src/watch_face/watch_face_app.c     Main program
    - src/watch_face/wf_image.c           Digital image arrays of index color
    - inc/watch_face/wf_image.h           Main program header file

@par Run the application
  - Touch "Watch Face" on LCM screen.
  - The power can be measured on this screen.
 * @}
 * @}
 * @}
 * @}
 */
