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
 * @addtogroup mt7687_hdk_hal_examples_sleep_manager_power_mode sleep_manager_power_mode
 * @{

@par Overview
  - Example description
    - This example is a reference application to use the sleep manager APIs
      to force system to enter sleep mode for power saving operation.
    - This application requires FreeRTOS.
    - Power monitor equipment is optional to verify the system status.
  - Results
    - The system enters into the WFI mode for 3 seconds and then into Legacy
      sleep mode for another 3 seconds.

@par Hardware and software environment
  - Supported platform
    - LinkIt 7687 HDK.
  - Power monitor (optional)
  - Note:
    - Please refer to LinkIt 7687 HDK Power Consumption Measurement Guide
      about how to connect power monitor with 7687 HDK board.
  - Environment configuration
    - The output logs are communicated through a micro-USB cable to the PC
      from USB (CON5) connector on the HDK.
      - Install the mbed serial driver according to the instructions at
        https://developer.mbed.org/handbook/Windows-serial-configuration. For
        more information, please refer to section "Installing the LinkIt 7687
        HDK drivers on Microsoft Windows" on the "LinkIt 7687 HDK User Guide"
        in [sdk_root]/doc folder.
      - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
        MK20 micro-B USB connector on the LinkIt 7687 HDK. For more
        information about the connector cable, please refer to
        https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
      - Launch a terminal emulator program, such as Tera terminal on your PC
        for data logging through UART. For the installation details, please
        refer to section "Installing Tera terminal on Microsoft Windows" on
        the "LinkIt for RTOS Get Started Guide" in [sdk_root]/doc folder.
      - COM port settings. baudrate: 115200, databits: 8, stop bit: 1,
        parity: none and flow control: off.
  - Customization Option
    - \b MTK_WIFI_REPEATER_ENABLE:
      Wi-Fi repeater mode compile option, due to AP part need a exactly IP
      address when local STA communcation with 7687 in repeater mode. This is
      enabled at default. If it is defined disabled, the repeat mode function
      will not be supported.

@par Directory contents
  - Source and header files
    - \b src/main.c:              Main program.
    - \b src/system_mt7687.c:     MT76x7 clock configuration file.
    - \b inc/hal_feature_config.h:
                                  MT76x7 feature configuration file.
    - \b inc/flash_map.h:         MT76x7 memory layout symbol file.
    - \b GCC/startup_mt7687.s:    MT76x7 startup file for GCC.
    - \b GCC/syscalls.c:          MT76x7 syscalls for GCC.
    - \b MDK-ARM/startup_mt7687.s:
                                  MT76x7 startup file for Keil IDE.
    - \b EWARM/startup_mt7687.s:  MT76x7 startup file for IAR.
  - Project configuration files using GCC
    - \b GCC/feature.mk:       Feature configuration.
    - \b GCC/Makefile:         Makefile.
    - \b GCC/mt7687_flash.ld:  Linker script.
  - Project configuration files using Keil
    - \b MDK-ARM/sleep_manager_power_mode.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/sleep_manager_power_mode.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/sleep_manager_power_mode.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/sleep_manager_power_mode.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/sleep_manager_power_mode.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project with the command "./build.sh mt7687_hdk
    sleep_manager_power_mode" from the SDK root folder and download the
    binary file to the LinkIt 7687 HDK.
  - Connect HDK to the PC with a micro-USB cable.
  - Connect the power monitor to the LinkIt 7687 HDK (optional).
  - Run the example. The system status is displayed in the log and switches
    between sleep modes until it's powered off.
*/
/**
 * @}
 * @}
 * @}
 */

