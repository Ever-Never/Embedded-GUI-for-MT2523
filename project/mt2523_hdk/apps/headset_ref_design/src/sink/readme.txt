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
 * @addtogroup mt2523_hdk_apps_headset_ref_design_sink sink
 * @{
 
@par Overview
  - Feature description
    - The Bluetooth Sink operates as a Bluetooth headset that
      supports Bluetooth Hands-Free, Advanced Audio Distribution and
      Audio/Video Remote Control Profiles.
  - The sink feature provides:
    - 1. Connection management, such as manage Bluetooth EDR connection, 
         auto-reconnect to a remote device after the Bluetooth is powered
         on or the Bluetooth link is lost. 
    - 2. Call management, such as answer or reject an incoming call, 
         redial on the smartphone, hang up or hold
         an active call, swap duo SCO calls, switch audio path, handle the 
         three way call.
    - 3. Bluetooth music. Streaming music from an audio device, streaming 
         control includes play or pause the music, move to previous or next track,
         Advanced Wireless Stereo (AWS) feature for high quality streaming between
         two headsets. For more details on AWS, please contact MediaTek customer
         support.

@par Hardware and software environment
  - Please refer to mt2523_hdkapps/headset_ref_design/readme.txt.

@par Directory contents
  - \b src/sink/bt_sink_app_keypad.c:
                                 Sink keypad file.
  - \b src/sink/bt_sink_app_cmd.c:
                                 Sink command file.
  - \b src/sink/bt_sink_app_event.c:
                                 Sink event file.
  - \b src/sink/bt_sink_app_main.c:
                                 Sink main program file.
  - \b src/sink/bt_sink_app_config.c:
                                 Sink action configuartion file.
  - \b inc/sink/bt_sink_app_keypad.h:
                                 Sink keypad header file.
  - \b inc/sink/bt_sink_app_cmd.h:
                                 Sink command header file.
  - \b inc/sink/bt_sink_app_event.h:
                                 Sink event header file.
  - \b inc/sink/bt_sink_app_main.h:
                                 Sink main program header file.

@par Run the feature
  - Build and Download
    - Please refer to mt2523_hdk/apps/headset_ref_design/readme.txt.
  - Perpare two MT2523 HDKs and download, one is a master headset, 
    the other is a slave headset.
  - Power on two MT2523 HDKs. Input AT command "AT+BTROLE=SET,Master\0d\0a" on 
    the one of them to set the slave. Input AT command "AT+BTROLE=SET,Master\0d\0a"
    on the other to set the master. Reboot the master and slave.
  - Respectively double press the function key on both master and slave,  
    waiting for the AWS connection established. 
  - After the AWS connection established, double press the function key to make 
    the master discoverable. 
  - use smartphone to search, bond and connect to the master with the name
    "BT_Headset_xxxxxxxxxxxx", where xxxxxxxxxxxx is the master Bluetooth
    EDR address.
  - After connection is established with the smartphone, the following
    operations will be available. For more details on how to run, please contact 
    MediaTek customer support.
      \code
      -------------------------------------------------------------------------------------------------------------------------------------------
      |                        |                                                    State                                                       |
      |                        |----------------------------------------------------------------------------------------------------------------|
      | Function Key Operation |Local music|    Not    |Connected  |Incoming |Outgoing |Active   | Held    |3-way         |Held and   |Bluetooth|
      |                        |playback   | Connected |           |call     |call     |call     | call    |incoming      |active call|music    |
      |                        |           |           |           |         |         |         |         |              |           |playback |
      |------------------------|-----------|-----------|-----------|---------|---------|---------|---------|--------------|-----------|---------|
      |        |               |           |           |Play       |         |         |         |         |Hold Active   |Swap active|         |
      |        |  Short press  |   Pause   |           |Bluetooth  | Answer  |         |Hold     |Active   |and accept    |and hlod   | Pause   |
      |        |               |           |           |music      |         |         |         |         |new incoming  |           |         |
      |        |---------------|-----------|-----------|-----------|---------|---------|---------|---------|--------------|-----------|---------|
      |        |               |           |           |           |         |         |Volume   |         |Hung up Active|           |         |
      |        |  Double press | Volume up |Pairing    |Pairing    |         |         |up       |         |and accept    | Volume up |Volume up|
      |        |               |           |           |           |         |         |         |         |new incoming  |           |         |
      | Master |---------------|-----------|-----------|-----------|---------|---------|---------|---------|--------------|-----------|---------|
      |        |               |           |Reset the  |Redial the |         |         |Switch   |         |              |Switch     |         |
      |        | Triple press  |           |paired     |last dialed|         |         |audio    |         |              |audio      |         |
      |        |               |           |list       |number     |         |         |paths    |         |              |path       |         |
      |        |---------------|-----------|-----------|-----------|---------|---------|---------|---------|--------------|-----------|---------|
      |        |  Long press   |Next track |           |Redial the | Reject  |Hang up  |Hang up  |Hang up  |Reject new    |Hung up    |Next     |
      |        |               |           |           |missed call|         |         |         |         |incoming      |Active     |track    |
      |        |---------------|-----------|-----------|-----------|---------|---------|---------|---------|--------------|-----------|---------|
      |        |Very long press| Power off |Power off  | Power off |Power off|Power off|Power off|Power off|  Power off   | Power off |Power off|
      -------------------------------------------------------------------------------------------------------------------------------------------
      \endcode

      \code
      -------------------------------------------------------------------------------------------------------------------------------------------
      |                        |                                                    State                                                       |
      |                        |----------------------------------------------------------------------------------------------------------------|
      | Function Key Operation |Local music|    Not    |Connected  |Incoming |Outgoing |Active   | Held    |3-way         |Held and   |Bluetooth|
      |                        |playback   | Connected |           |call     |call     |call     | call    |incoming      |active call|music    |
      |                        |           |           |           |         |         |         |         |              |           |playback |
      |------------------------|-----------|-----------|-----------|---------|---------|---------|---------|--------------|-----------|---------|
      |        |               |           |Play       |Play       |         |         |         |         |Hold Active   |Swap active|         |
      |        |  Short press  |   Pause   |local      |Bluetooth  | Answer  |         |Hold     |Active   |and accept    |and hlod   | Pause   |
      |        |               |           |music      |music      |         |         |         |         |new incoming  |           |         |
      |        |---------------|-----------|-----------|-----------|---------|---------|---------|---------|--------------|-----------|---------|
      |        |               |           |           |           |         |         |Volume   |         |Hung up Active|           |Volume   |
      |        | Double press  |Volume down|Pairing    |           |         |         |down     |         |and accept    |Volume down|down     |
      |        |               |           |           |           |         |         |         |         |new incoming  |           |         |
      | Slave  |---------------|-----------|-----------|-----------|---------|---------|---------|---------|--------------|-----------|---------|
      |        |               |           |Reset the  |Redial the |         |         |Switch   |         |              |Switch     |         |
      |        | Triple press  |           |paired     |last dialed|         |         |audio    |         |              |audio      |         |
      |        |               |           |list       |number     |         |         |paths    |         |              |path       |         |
      |        |---------------|-----------|-----------|-----------|---------|---------|---------|---------|--------------|-----------|---------|
      |        |  Long press   |Previous   |           |Redial the | Reject  |Hang up  |Hang up  |Hang up  |Reject new    |Hung up    |Previous |
      |        |               |track      |           |missed call|         |         |         |         |incoming      |Active     |track    |
      |        |---------------|-----------|-----------|-----------|---------|---------|---------|---------|--------------|-----------|---------|
      |        |Very long press| Power off |Power off  | Power off |Power off|Power off|Power off|Power off|  Power off   | Power off |Power off|
      -------------------------------------------------------------------------------------------------------------------------------------------
      \endcode
*/
/**
 * @}
 * @}
 * @}
 * @}
 */
