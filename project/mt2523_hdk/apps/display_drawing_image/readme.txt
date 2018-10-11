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
 * @addtogroup mt2523_hdk_apps_display_drawing_image display_drawing_image
 * @{

@par Overview
  - Application description
    - This project is a reference application to setup LCM and display images
      on the LCM.
  - Application features
    - The application automatically detects two different output interfaces
      MediaTek serial interface (240x240) and MIPI interface (320x320), then
      displays the images with different resolutions based on the interface
      selection.
  - Results
    - The LCM displays and switches between MediaTek logo and LinkIt logo
      every 5 seconds.

@par Hardware and software environment
  - Supported HDK
    - LinkIt 2523 HDK.
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
    - \b src/display_demo.c:      Main task to display the image.
    - \b src/linkit_240X240.c:    Raw data of LinikIt logo in RGB565 format
                                  with resolution of 240x240.
    - \b src/linkit_320X320.c:    Raw data of LinikIt logo in RGB565 format
                                  with resolution of 320x320.
    - \b src/logo_240X240.c:      Raw data of MediaTek logo in RGB565 format
                                  with resolution of 240x240.
    - \b src/logo_320X320.c:      Raw data of MediaTek logo in RGB565 format
                                  with resolution of 320x320.
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
  - Project configuration files using Keil
    - \b MDK-ARM/display_drawing_image.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/display_drawing_image.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/display_drawing_image.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/display_drawing_image.ewp:
                           IAR project file. Contains the project structure in
                           XML format
    - \b EWARM/display_drawing_image.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the application
  - Build the example project with the command "./build.sh mt2523_hdk
    display_drawing_image" from the SDK root folder and download the binary
    file to the LinkIt 2523 HDK.
  - Connect HDK to the PC with a micro-USB cable.
  - Run the application and observe the result. The LCM switches between
    MediaTek logo and LinkIt logo every 5 seconds.
*/
/**
 * @}
 * @}
 * @}
*/
