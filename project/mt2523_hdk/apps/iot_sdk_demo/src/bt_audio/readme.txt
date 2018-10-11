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
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo_BT_Audio BT_Audio
 * @{

@par Overview
  - Application description
    - The Bluetooth Sink application works as a Bluetooth headset that
      supports Bluetooth Hands-Free, Advanced Audio Distribution and
      Audio/Video Remote Control Profiles. The Bluetooth Sink application
      also supports Bluetooth dual mode to enable both BR/EDR and Bluetooth
      Low Energy (LE) Proximity Profile (PXP) connections at the same time.
  - Features of the application
    - 1. Handle incoming calls, for example, answer or reject an incoming
      call. 2. Dial last dialed number on the device. 3. Stream music from a
      music-playing device. 4. Streaming control, such as play or pause
      music, move to previous or next song. 5. Reconnect the last connected
      device. 6. Connect Bluetooth LE PXP.

@par Hardware and software environment
  - Refer to mt2523_hdk/apps/iot_sdk_demo/readme.txt.

@par Directory contents
  - Source and header files
    - \b src/bt_audio/bt_audio.c:  Bluetooth UI display file.
    - \b src/bt_audio/sink/bt_sink_app_cmd.c:
                                   Bluetooth application command file.
    - \b src/bt_audio/sink/bt_sink_app_event.c:
                                   Bluetooth Event file.
    - \b src/bt_audio/sink/bt_sink_app_main.c:
                                   Bluetooth main program file.
    - \b src/bt_audio/sink/bt_sink_app_mapping_table.c:
                                   Bluetooth action mapping file.
    - \b inc/bt_audio/bt_audio.h:  Bluetooth UI display header file.
    - \b inc/bt_audio/sink/bt_sink_app_cmd.h:
                                   Bluetooth application command header file.
    - \b inc/bt_audio/sink/bt_sink_app_event.h:
                                   Bluetooth Event header file.
    - \b inc/bt_audio/sink/bt_sink_app_main.h:
                                   Bluetooth main program header file.

@par Run the application
  - Build source code of this demo application.
    - Build the example project with the command "./build.sh mt2523_hdk
      iot_sdk_demo" from the SDK root folder.
  - Download the binary file of the application to Linkit 2523  HDK using
    Flash Tool.
  - Pair the Bluetooth between smartphone and Linkit 2523 HDK.
  - Touch BT Audio app on the display.
  - Bluetooth Audio app shows "Connecting" status and "reconnect" option.
  - Touch "reconnect", Linkit 2523 HDK will reconnect with the last connected
    device.
  - Use smartphone search, bond and connect to the Linkit 2523 with the name
    "BT_Audio_Demo".
  - Touch "play music" to play music on a smartphone.
  - Touch "Next" or "Previous" to switch to the next or the previous song.
  - Make an incoming call to the smartphone and Linkit 2523 HDK will show the
    caller's number.
  - Touch "Answer call" or "Reject call" to accept or reject the incoming
    call.
  - After the call is answered, touch "Hang up" to terminate the active call.
  - To connect with Bluetooth LE PXP, please refer to
    mt2523_hdk/apps/iot_sdk_demo/src/pxp_screen/readme.txt, step 2 and step
    3.
*/
/**
 * @}
 * @}
 * @}
 * @}
 */

