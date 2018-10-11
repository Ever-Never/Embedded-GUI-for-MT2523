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
 * @addtogroup mt2523_hdk_apps_headset_ref_design_AudioPlayer AudioPlayer
 * @{
 
@par Overview
  - Feature description
    - This feature demonstrates the local music control, such as play, pause or
      change track MP3 file of the headset device based on MediaTek MT2523 SOC.

@par Hardware and software environment
  - Please refer to mt2523_hdk/apps/headset_ref_design/readme.txt.

@par Directory contents
  - \b src/AudioPlayer/audio_player_main.c:    MP3 player file.
  - \b src/AudioPlayer/audio_player_atci_cmd.c:MP3 player AT command file.
  - \b inc/AudioPlayer/audio_player.h:         MP3 player extern API header file.
  - \b inc/AudioPlayer/audio_player_main.h:    MP3 player main header file.

@par Run the feature
  - Build and Download
    - Please refer to mt2523_hdk/apps/headset_ref_design/readme.txt.
  - Power on the MT2523 HDK.
  - The following options are available for the Audio Player.
    - FUNC key, Short press, play or pause the music;
    - Next/Prev key, Short press, volume up/down; 
    - Next/Prev key, Long press, change track.
*/
/**
 * @}
 * @}
 * @}
 * @}
 */
