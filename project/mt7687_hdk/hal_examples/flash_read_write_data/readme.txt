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
 * @addtogroup mt7687_hdk mt7687_hdk
 * @{
 * @addtogroup mt7687_hdk_hal_examples hal_examples
 * @{
 * @addtogroup mt7687_hdk_hal_examples_flash_read_write_data flash_read_write_data
 * @{

@par Overview
  - Example description
    - This example is a reference application to erase, read and write data
      from/to the flash.
    - This example does not require FreeRTOS.
  - Results
    - The result is in the system log.

@par Hardware and software environment
  - Supported platform
    - LinkIt 7687 HDK.
  - HDK switches and pin configuration
    - N/A.
  - Environment configuration
    - The output logs are communicated through a type-A to micro-B USB cable
      to the PC from USB (CON5) connector on the HDK.
    - Install the mbed serial driver according to the instructions at
      https://developer.mbed.org/handbook/Windows-serial-configuration. For
      more information, please refer to section "Installing the LinkIt 7687
      HDK drivers on Microsoft Windows" on the "LinkIt 7687 HDK User Guide"
      in [sdk_root]/doc folder.
    - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
      CON5 micro-B USB connector on the LinkIt 7687 HDK. For more information
      about the connector cable, please refer to
      https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
    - Launch a terminal emulator program, such as Tera terminal on your PC
      for data logging through UART. For the installation details, please
      refer to section "Installing Tera terminal on Microsoft Windows" on the
      "LinkIt for RTOS Get Started Guide" in [sdk_root]/doc folder.
    - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none and flow control: off.

@par Directory contents
  - Source and header files
    - \b src/main.c:            Main program.
    - \b src/system_mt7687.c:   MT76x7 clock configuration file.
    - \b inc/hal_feature_config.h:
                                MT76x7 feature configuration file
    - \b inc/flash_map.h:       MT76x7 memory layout symbol file
    - \b GCC/startup_mt7687.s:  MT76x7 startup file for GCC.
    - \b GCC/syscalls.c:        MT76x7 syscalls for GCC.
    - \b MDK-ARM/startup_mt7687.s:
                                MT76x7 startup file for Keil IDE.
  - Project configuration files using GCC
    - \b GCC/feature.mk:       Feature configuration.
    - \b GCC/Makefile.:        Makefile.
    - \b GCC/mt7687_flash.ld:  Linker script.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/flash_read_write_data.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/flash_read_write_data.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/flash_read_write_data.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/flash_read_write_data.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/flash_read_write_data.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project with a command "./build.sh mt7687_hdk
    flash_read_write_data" from the SDK root folder and download the binary
    file to the LinkIt 7687 HDK.
  - Connect the HDK to the PC with a type-A to micro-B USB cable and specify
    the port on the serial terminal corresponding to "mbed Serial Port".
  - Run the example. The log "Flash example is successful!" indicates a
    successful operation.
*/
/**
 * @}
 * @}
 * @}
 */
