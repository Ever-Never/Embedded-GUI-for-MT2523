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
 * @addtogroup mt2523_hdk_apps_gdi_display_helloworld gdi_display_helloworld
 * @{

@par Overview
  - Application description
    - This is a reference example to design a UI based application.
  - Application features
    - This example demonstrates how to use GDI interface. Note: since GDI
      middleware interface only supports single instance, please use only one task to show
      all the UI content.
    - The example will guide you to:
      - Create a UI task and display the content.
      - Display a string.
      - Display an image.
    - For more information, please refer to GDI in LinkIt 2523 API Reference Manual.

@par Hardware and software environment
  - Supported HDK
    - LinkIt 2523 HDK.
  - HDK switches and pin configuration
    - N/A.
  - Environment configuration
    - The output logs are communicated through a type-A to micro-B USB cable
      to the PC from MK20 USB connector on the HDK.
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
    - COM port settings. baudrate: 115200, data: 8 bits, stop bit: 1, parity:
      none and flow control: off.

@par Directory contents
  - Source and header files
    - \b src/main.c:            Main program. Start the UI task in main function.
    - \b src/ui_demo_task.c:    Create and control the UI task.
    - \b src/main_screen.c:     Display the main screen. Draw an image and a string.
    - \b src/custom_image_data_resource.c:
                                An image resource file. Includes raw image data.
    - \b src/custom_image_map.c:
                                An image resource file. Includes the list of all image data.
    - \b src/custom_image_resource.c:
                                An image resource file. Includes the list of all image file names.
    - \b src/FontRes.c:         Font and language resource file.
    - \b src/system_mt2523.c:   The configuration file of the Cortex-M4 with
                                floating point core registers and system clock.
    - \b src/sys_init.c:        This file initializes the hardware environment
                                for the system, including system clock, UART
                                port for logging, Pinmux, cache and other
                                necessary hardware.
    - \b inc/main_screen.h:     Define the main screen function.
    - \b inc/custom_image_data_resource.h:
                                Image resource file. Extern the list of image data.
    - \b inc/custom_resource_def.h:
                                Image resource file. Includes the list of all image IDs used in 
                                the UI project.
    - \b inc/L_English_small.h: Font resource file. For English characters.
    - \b inc/L_gMTKProprietaryFont_small.h:
                                Font resource file. For MediaTek proprietary characters.
    - \b inc/FreeRTOSConfig.h:  FreeRTOS feature configuration file.
    - \b inc/task_def.h:        Define the task stack size, queue length,
                                project name, and priority for the application
                                to create tasks.
    - \b inc/hal_feature_config.h:
                                MT2523x feature configuration file.
    - \b inc/memory_map.h:      MT2523x memory layout symbol file.
    - \b inc/sys_init.h:        The header file of sys_init.c.
    - \b GCC/startup_mt2523.s:  MT2523x startup file for GCC.
    - \b GCC/syscalls.c:        The minimal implementation of the system calls.
    - \b MDK-ARM/startup_mt2523.s:
                                MT2523x startup file for Keil IDE.
  - Project configuration files using GCC
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile.:   Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/gdi_display_helloworld.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/gdi_display_helloworld.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently opened files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.

@par Run the example
  - Build the example project with the command "./build.sh mt2523_hdk
    gdi_display_helloworld bl" from the SDK root folder and download
    the binary file to LinkIt 2523 HDK.
  - Connect the HDK to the PC with a type-A to micro-B USB cable and specify
    the port on Tera terminal corresponding to "mbed Serial Port".
  - Run the example. The display will show "Hello World!" and a Bluetooth icon.
*/
/**
 * @}
 * @}
 * @}
 */
*/