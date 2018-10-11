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
 * @addtogroup mt2523_hdk_apps_audio_mp3_play audio_mp3_play
 * @{

@par Overview
  - Application description
    - This project demonstrates how to setup MP3 play environment.
  - Application features
    - The project creates a task to demonstrate how to use the MP3 codec
      driver to play MP3 files.
  - Application process
    - Place three MP3 files in the SD card's temporary folder (temp/) and
      name them as "1.mp3", "2.mp3" and "3.mp3". Then boot the device.
  - Results
    - You can hear the MP3 sound. First, it'll play "1.mp3", then pause and
      resume again. After a while it will change file to "2.mp3", then pause
      and resume again. Finally, it'll play "3.mp3", pause and resume and
      then play to the end of the file.

@par Hardware and software environment
  - Supported HDK
    - LinkIt 2523 HDK.
  - Hardware environment
    - Requires an SD card to store the MP3 files.
    - Set J1001 and J1006 jumpers to pin 2 and 3.
  - Software environment
    - Modify GPIO configuration file (ept_gpio_drv.h) under "inc" folder to
      provide the SD card settings.
      \code
      #define GPIO_PORT30_MODE   MODE_5 // 5:MC0_CK
      #define GPIO_PORT31_MODE   MODE_5 // 5:MC0_CM0
      #define GPIO_PORT32_MODE   MODE_5 // 5:MC0_DA0
      #define GPIO_PORT33_MODE   MODE_5 // 5:MC0_DA1
      #define GPIO_PORT34_MODE   MODE_5 // 5:MC0_DA2
      #define GPIO_PORT35_MODE   MODE_5 // 5:MC0_DA3
      \endcode
  - Environment configuration
    - The output logs are communicated through a micro-USB cable to the PC
      from MK20 USB connector on the HDK.
      - Install the mbed serial driver according to the instructions at
        https://developer.mbed.org/handbook/Windows-serial-configuration. For
        more information, please refer to section "Installing the LinkIt 2523
        HDK drivers on Microsoft Windows" on the "LinkIt 2523 HDK User Guide"
        in [sdk_root]/doc folder.
      - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
        MK20 micro-B USB connector on the LinkIt 2523 HDK. For more
        information about the connector cable, please refer to
        https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
      - Launch a terminal emulator program, such as Tera terminal on your PC
        for data logging through UART. For the installation details, please
        refer to section "Installing Tera terminal on Microsoft Windows" on
        the "LinkIt for RTOS Get Started Guide" in [sdk_root]/doc folder.
      - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1,
        parity: none and flow control: off.

@par Directory contents
  - Source and header files
    - \b src/main.c:              Main program.
    - \b src/mp3_codec_ut.c:      The MP3 demo unit test code.
    - \b src/system_mt2523.c:     MT2523x clock configuration file.
    - \b src/sys_init.c:          Aggregated initialization routines.
    - \b src/ept_gpio_var.c:      GPIO pin assignment with variable names.
    - \b src/ept_eint_var.c:      EINT assignment file with variable names.
    - \b inc/FreeRTOSConfig.h:    FreeRTOS feature configuration file.
    - \b inc/task_def.h:          Define the task stack size, queue length,
                                  project name, and priority for the
                                  application to create tasks.
    - \b inc/hal_feature_config.h:
                                  MT2523x feature configuration file.
    - \b inc/memory_map.h:        MT2523x memory layout symbol file.
    - \b inc/ept_eint_drv.h:      EINT configuration header file.
    - \b inc/ept_gpio_drv.h:      GPIO pin assignment and configuration file.
    - \b inc/ept_keypad_drv.h:    Keypad configuration header file.
    - \b inc/cust_battery_meter.h:
                                  The customization file to configure the
                                  battery level parameters.
    - \b inc/cust_battery_meter_table.h:
                                  The customization file to configure the
                                  battery meter parameters.
    - \b inc/cust_battery_temperature_table.h:
                                  The customization file to configure the
                                  battery temperature parameters.
    - \b inc/cust_charging.h:     The customization file to configure the
                                  charging parameters.
    - \b inc/sys_init.h:          The header file of sys_init.c.
    - \b GCC/startup_mt2523.s:    MT2523x startup file for GCC.
    - \b GCC/syscalls.c:          MT2523x syscalls for GCC.
    - \b MDK-ARM/startup_mt2523.s:
                                  MT2523x startup file for Keil IDE.
    - \b EWARM/startup_mt2523.s:  MT2523x startup file for IAR.
  - Project configuration files using GCC
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile:    Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/audio_mp3_play.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/audio_mp3_play.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/audio_mp3_play.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/audio_mp3_play.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/audio_mp3_play.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project with the command "./build.sh mt2523_hdk
    audio_mp3_play" from the SDK root folder and download the binary file to
    the LinkIt 2523 HDK.
  - Connect the HDK to the PC with a micro-USB cable.
  - Set J1001 and J1006 jumpers to pin 2 and 3.
  - Run the example. The processing states are displayed in the log and the
    sound is audible.
 * @}
 * @}
 * @}
 **/
