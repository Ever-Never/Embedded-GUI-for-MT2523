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
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo_main_screen main_screen
 * @{

@par Overview
  - Application description
    - This application provide UI task, process pen event assignment, and
      provide entry point for sub features.
  - Output from the application
    - Show a list of demo items.

@par Hardware and software environment
  - Please refer to the readme document in
    project/mt2523_hdk/apps/iot_sdk_demo

@par Directory contents
  - Source and header files
    - \b src/main_screen/main_screen.c:
      Draw list menu.
    - \b src/main_screen/ui_demo_task.c:
      UI task
  - Project configuration files using GCC
    - \b GCC/main_screen.mk:  Makefile.

@par Run this application
  - Build the application with the command, "./build.sh mt2523_hdk
    iot_sdk_demo bl" from the SDK root folder.
  - Download the bin file to LinkIt 2523 HDK.
  - Power on the LinkIt 2523 HDK, main screen will be shown.
 * @}
 * @}
 * @}
 * @}
**/
