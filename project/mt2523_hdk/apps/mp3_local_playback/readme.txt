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
 * @addtogroup mt2523_hdk_apps_mp3_local_playback mp3_local_playback
 * @{

@par Overview
  - Application description
    - This is a reference application to demonstrate usage of
      mp3_local_playback.

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK.
  - Environment configuration
    - Need a serial tool (such as Tera terminal) for UART logging.
    - COM port settings: baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none and flow control: off.

@par Directory contents
  - Source and header files
    - \b src/main.c:               Main program.
    - \b src/at_command_serial_port.c:
                                   at command port server file.
    - \b src/bt_init.c:            Bluetooth initialization file.
    - \b src/audio_player_main.c:  MP3 player file.
    - \b src/sys_init.c:           system init file.
    - \b src/ept_eint_var.c:       EINT assignment file with variable names.
    - \b src/ept_gpio_var.c:       GPIO pin assignment with variable names.
    - \b src/system_mt2523.c:      MT2523x system clock configuration file.
    - \b GCC/startup_mt2523.s:     MT2523x startup file for GCC.
    - \b GCC/syscalls.c:           MT2523x syscalls for GCC.
    - \b inc/task_def.h:           Define the task stack size, queue length,
                                   project name, and priority for the
                                   application to create tasks.
    - \b inc/hal_feature_config.h:
                                   MT2523x feature configuration file.
    - \b inc/memory_map.h:         MT2523x memory layout symbol file.
    - \b inc/ept_eint_drv.h:       EINT configuration header file.
    - \b inc/ept_gpio_drv.h:       GPIO pin assignment and configuration file.
    - \b inc/ept_keypad_drv.h:     Keypad configuration header file.
    - \b inc/main.h:               Main program header file.
    - \b inc/hci_log.h:            HCI log header file.
    - \b inc/audio_player.h:       MP3 player extern API header file.
    - \b inc/audio_player_main.h:  MP3 player main header file.
  - Project configuration files using GCC.
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile:    Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using Keil IDE.
    - \b MDK-ARM/mp3_local_playback.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/mp3_local_playback.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR.
    - \b EWARM/mp3_local_playback.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/mp3_local_playback.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/mp3_local_playback.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the sample project with the command "./build.sh mt2523_hdk
    mp3_local_playback" from the SDK root folder on GCC, or build the project
    mp3_local_playback.uvprojx from the MDK-ARM folder on Keil, or build the
    project mp3_local_playback.eww from the EWARM folder on IAR, then
    download the binary file to LinkIt 2523 HDK.
  - To run the application.
    - AT+EAUDPLY=1: Play CMD, send again will pause music.
    - AT+EAUDVOL=1: Volume up; AT+EAUDVOL=0 Volume down.
    - AT+EAUDSKIP=1: Change next track; AT+EAUDSKIP=0: Change previous track.
*/
/**
 * @}
 * @}
 * @}
 */

