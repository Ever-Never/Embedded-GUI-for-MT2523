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
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo_gnss_screen gnss_screen
 * @{

@par Overview
  - Application description
    - This is a simple GNSS application with UI display. It demonstrates the
      usage of GNSS module APIs.
  - Application features
    - This application shows how to send commands to GNSS module and recieve
      and parse NMEA data from the module.
    - UI display shows GNSS positioning information, such as longitude,
      latitude, etc.
  - Result
    - Positioning information on UI display.

@par Hardware and software environment
  - Please refer to the readme document on
    project/mt2523_hdk/apps/iot_sdk_demo.

@par Directory contents
  - Source and header files
    - \b src/gnss_screen/epo_demo.c:
      EPO sample code.
    - \b src/gnss_screen/gnss_app.c:
      GNSS app task.
    - \b src/gnss_screen/gnss_demo_screen.c:
      GNSS UI and NMEA data parser.
    - \b src/gnss_screen/gnss_ring_buffer.c:
      Ring buffer for further usage.
    - \b src/gnss_screen/gnss_timer.c:
      RTOS timer wrapper.
  - Project configuration files using GCC
    - \b GCC/gnss_screen.mk:  Makefile.

@par Run the GNSS application
  - Build this application with the command "./build.sh mt2523_hdk
    iot_sdk_demo bl" from the SDK root folder.
  - Download the binary file to LinkIt 2523 HDK.
  - Power on LinkIt 2523 HDK.
  - Press "GNSS demo" menu item to enter GNSS screen.
  - GNSS screen will show "No fix" at the beginning when GNSS receiver just
    starts working.
  - Wait for a few minutes under open sky, the latitude and longitude will be
    shown after the GNSS acquires the positioning information successfully.
  - Press "Exit" button to exit the GNSS Screen.
 * @}
 * @}
 * @}
 * @}
 */
