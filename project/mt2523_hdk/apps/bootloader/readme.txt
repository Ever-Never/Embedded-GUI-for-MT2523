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
 * @addtogroup mt2523_hdk_apps_bootloader bootloader
 * @{

@par Overview
  - This is not a example project. This project is a bootloader for LinkIt
    2523 HDK. The developer can refer this project to understand how to build
    a bootloader.

@par Hardware and software environment
  - Environment configuration
    - The output logs are communicated through a micro USB cable to the PC
      from MK20 USB connector on the HDK.
    - Install the mbed serial driver according to the instructions at
      https://developer.mbed.org/handbook/Windows-serial-configuration. For
      more information, please refer to section "Installing the LinkIt 2523
      HDK drivers on Microsoft Windows" on the "LinkIt 2523 HDK User Guide"
      in [sdk_root]/doc folder.
    - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
      MK20 micro-B USB connector on the LinkIt 2523 HDK. For more information
      about the connector cable, please refer to
      https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
    - Launch a terminal emulator program, such as Tera terminal on your PC
      for data logging through UART. For the installation details, please
      refer to section "Installing Tera terminal on Microsoft Windows" on the
      "LinkIt for RTOS Get Started Guide" in [sdk_root]/doc folder.
    - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none and flow control: off.
  - Customization Option
    - \b MTK_FOTA_FS_ENABLE: Selects and enables the FOTA update via file
      (FOTA.bin) in SD card. Default FOTA update is via content in flash.
      Please note that this option needs to be enabled in the
      fota_download_manager project.

@par Directory contents
  - Source and header files
    - \b \<sdk_root\>/driver/board/mt25x3_hdk/bootloader/core/src/bl_main.c:
                                  Main program
    - \b inc/hal_feature_config.h:
                                  MT2523x's feature configuration file.
    - \b MDK-ARM/hal_log.h:       Delcalre HAL log function in bootloader for
                                  Keil.
    - \b src/custom_fota.c:       Fota update function customization.
    - \b GCC/startup_bootloader.s:
                                  MT2523x startup file for GCC.
    - \b GCC/syscalls.c:          The minimal implementation of the system
                                  calls.
    - \b MDK-ARM/startup_bootloader.s:
                                  MT2523x startup file for Keil IDE.
    - \b EWARM/startup_mt2523.s:  MT2523x startup file for IAR IDE.
  - Project configuration files using GCC
    - \b GCC/feature.mk:     Feature configuration.
    - \b GCC/Makefile:       Makefile.
    - \b GCC/bootloader.ld:  Linker script.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/bootloader.uvprojx:
                           uVision5 project File. Contains the project
                           structure in XML format.
    - \b MDK-ARM/bootloader.uvoptx:
                           uVision5 project options. Contains the settings for
                           the debugger, trace configuration, breakpoints,
                           currently open files, etc.
    - \b MDK-ARM/ram.sct:  Scatter file.
  - Project configuration files using IAR IDE
    - \b EWARM/bootloader.ewd:     IAR project options. Contains the settings
                                   for the debugger.
    - \b EWARM/bootloader.ewp:     IAR project file. Contains the project
                                   structure in XML format
    - \b EWARM/bootloader.eww:     IAR workspace file. Contains project
                                   information.
    - \b EWARM/mtk2523_flash.icf:  Linker script.

@par Run the example
  - Connect board to the PC with serial port cable.
  - Build bootloader and an example project with the command, "./build.sh
    mt2523_hdk example_project_name bl" under the SDK root folder and
    download the binary file to LinkIt MT2523 HDK.
  - Power on LinkIt 2523 HDK and bootloader log are showen on serial tool.
 * @}
 * @}
 * @}
*/
