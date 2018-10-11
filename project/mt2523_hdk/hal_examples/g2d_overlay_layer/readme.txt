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
 * @addtogroup mt2523_hdk_hal_examples hal_examples
 * @{
 * @addtogroup mt2523_hdk_hal_examples_g2d_overlay_layer g2d_overlay_layer
 * @{

@par Overview
  - Example description
    - This example is a reference to use the G2D APIs to render an image.
      This example does not require FreeRTOS.
  - Results
    - The rendered image is shown on the display module and the output logs
      are sent to the COM port.

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK with MIPI 320*320 AMOLED display module.
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
    - \b src/system_mt2523.c:     MT2523x clock configuration file.
    - \b src/wallpaper.c:         Mediatek's wallpaper test image in
                                  hexadecimal format.
    - \b src/brandmark.c:         Mediatek's brandmark test image in
                                  hexadecimal format.
    - \b inc/hal_feature_config.h:
                                  MT2523x feature configuration file.
    - \b inc/memory_map.h:        MT2523x memory layout symbol file.
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
    - \b MDK-ARM/g2d_overlay_layer.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/g2d_overlay_layer.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/g2d_overlay_layer.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/g2d_overlay_layer.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/g2d_overlay_layer.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project with the command "./build.sh mt2523_hdk
    g2d_overlay_layer" from the SDK root folder and download the binary file
    to the LinkIt 2523 HDK.
  - Connect the HDK to the PC with a micro USB-cable.
  - Run the example. The log "G2D test done." indicates the destination image
    is rendered and the rendered image is shown on display module.
*/
/**
 * @}
 * @}
 * @}
 */

