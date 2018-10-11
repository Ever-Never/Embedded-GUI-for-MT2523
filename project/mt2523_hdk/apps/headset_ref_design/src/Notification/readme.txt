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
 * @addtogroup mt2523_hdk_apps_headset_ref_design headset_ref_design
 * @{
 * @addtogroup mt2523_hdk_apps_headset_ref_design_notification notification
 * @{

@par Overview
  - Feature description
    - This feature demonstrates how to receive notifications from a remote
      device.

@par Hardware and software environment
  - Please refer to mt2523_hdk/apps/headset_ref_design/readme.txt.

@par Directory contents
  - Source and header files
    - \b inc/notification:                 Notification header files.
    - \b src/notification/bt_notify_app.c: Source code to receive notifications
                                           from a smartphone.

@par Run the feature
  - Build and Download
    - Please refer to mt2523_hdk/apps/headset_ref_design/readme.txt.
  - Install the application "MediaTek_SmartDevice.apk" located under the
    "sdk_root/tools/smart_device/Android/" folder on an Android smartphone
    and launch the application.
  - Power on the MT2523 HDK.
  - Click "scan" to enable scanning for Bluetooth enabled devices, discover
    and tap on the MT2523 HDK.
  - After the Bluetooth connection is established, a log "Alert: New
    Notification" indicates a successful operation.
*/
/**
 * @}
 * @}
 * @}
 * @}
 */
