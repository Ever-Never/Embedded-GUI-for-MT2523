/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
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
 * @addtogroup mt2523_hdk_apps_headset_ref_design_fota fota
 * @{

@par Overview
  - Feature description
    - This feature demonstrates how to use Bluetooth notification
      services to receive FOTA packages and trigger update on the target board.

@par Hardware and software environment
  - Please refer to mt2523_hdk/apps/headset_ref_design/readme.txt.

@par Directory contents
  - Source and header files
    - \b inc/fota:                 FOTA header files.
    - \b src/fota/fota_main.c:     FOTA download manager source code.
    - \b src/fota/fota_bt_common.c:Bluetooth event handler source code.

@par Run FOTA feature
  - Build and Download
    - Please refer to mt2523_hdk/apps/headset_ref_design/readme.txt.
  - Install the application "MediaTek_SmartDevice.apk" located under
    the "sdk_root/tools/smart_device/Android/" folder on an Android
    smartphone and launch the application.
  - Power on the MT2523 HDK.
  - Click "scan" on the smartphone to discover Bluetooth enabled devices,
    then find and connect to the MT2523 HDK.
  - After Bluetooth connection is established, click "Firmware Update(FBIN)" in
    drop-down menu at the top-right corner of the screen.
  - Select the FOTA package file from the smartphone file system, then the
    application will push the FOTA package to MT2523 HDK and start the FOTA
    update process.
  - The update progress is shown in a trace window, the MT2523 HDK will reboot
    automatically after download is complete, reconnect with the smartphone and
    notify the update status to the application.
*/
/**
 * @}
 * @}
 * @}
 * @}
 */
