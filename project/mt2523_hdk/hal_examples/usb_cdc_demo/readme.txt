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
 * @addtogroup mt2523_hdk_hal_examples_usb_cdc_demo usb_cdc_demo
 * @{

@par Overview
  - Example description
    - This example is a reference application to demonstrate the usage of USB
      HAL module APIs. The project runs without FreeRTOS.
  - Application features
    - The application demonstrates how to use USB connectivity feature of the
      LinkIt 2523 HDK to input and output data with USB CDC class.
  - Results
    - Echo the result from the host terminal software.

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK.
  - HDK switches and pin configuration
    - Connect USB cable between PC Host and device
  - Environment configuration
    - USB host driver is the same as download tool.
    - A serial tool is required, such as hyper terminal, or TeraTerm.
    - COM port settings. Select the COM port corresponding to the connected
      MediaTek device, such as "MTK USB Port".

@par Directory contents
  - Source and header files
    - \b src/main.c:              Main program.
    - \b src/usb_demo:            USB CDC class driver files.
    - \b src/system_mt2523.c:     MT2523x clock configuration file.
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
    - \b MDK-ARM/usb_cdc_demo.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/usb_cdc_demo.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/usb_cdc_demo.ewd:  IAR project options. Contains the settings
                                  for the debugger.
    - \b EWARM/usb_cdc_demo.ewp:  IAR project file. Contains the project
                                  structure in XML format.
    - \b EWARM/usb_cdc_demo.eww:  IAR workspace file. Contains project
                                  information.
    - \b EWARM/flash.icf:         Linker script.

@par Run the example
  - Build the example project with the command, "./build.sh mt2523_hdk
    usb_cdc_demo" from the SDK root folder and download the binary file to
    the LinkIt 2523 HDK.
  - Connect the HDK to PC with micro-USB cable before powering it on. Note,
    this example project doesn't support hot plugging,  plug in the cable
    before powering the device on.
  - Power on device, wait for execution of bootloader is finished, a new com
    port will generate.
  - When the host detects the USB device, then open a serial tool on the PC.
  - Input test data into USB COM port from terminal emulator software, such
    as TeraTerm.
  - Then received data will echo to the TeraTerm.
*/
/**
 * @}
 * @}
 * @}
 */