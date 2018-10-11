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
 * @addtogroup mt2523_hdk_hal_examples_i2s_internal_loopback i2s_internal_loopback
 * @{

@par Overview
  - Example description
    - This project is a reference application that demonstrates how to use
      the I2S module APIs to apply an internal loopback. The data sent from
      the I2S TX link is looped back to the I2S RX link.
    - This project does not require FreeRTOS.
  - Results
    - The input and output of transmitted data are described below.
      - Input to the example.
        - Data in the i2s_tone2k_32ksr array: 2kHz sine wave audio frequency
          tone under 32kHz sampling rate. 0xFFF4  0x30F0  0x5A79  0x763C
          0x7FFF  0x7646  0x5A8B  0x3107 0x000C  0xCF10  0xA587  0x89C4
          0x8001  0x89BB  0xA576  0xCEF9 0xFFF3  0x30F0  0x5A79  0x763C
          0x7FFF  0x7645  0x5A8A  0x3106 0x000C  0xCF10  0xA587  0x89C4
          0x8001  0x89BB  0xA576  0xCEF9
        - This example project outputs data continuously.
      - Output from the example.
        - A transmission delay from the TX to the RX link is unavoidable when
          the internal loopback mode is set. The first few samples of the RX
          link are expected to be zero before receiving valid samples.
          Because of this, the example records samples after several
          transmissions. Since the recorded data is extracted from a series
          of transmissions, it would not start from the beginning of
          i2s_tone2k_32ksr, but follows the pattern below. 0x89bb  0xa576
          0xcef9  0xfff3  0x30ef  0x5a78  0x763b  0x7ffe 0x7644  0x5a89
          0x3105  0x000b  0xcf10  0xa587  0x89c4  0x8001 0x89bb  0xa576
          0xcef9  0xfff4  0x30ef  0x5a78  0x763b  0x7ffe 0x7645  0x5a8a
          0x3106  0x000b  0xcf10  0xa587  0x89c4  0x8001
        - The output through I2S interface would not be exactly the same (not
          bit-true) as the input owing to the slight gain adjustment from the
          hardware. However, the audio performance would not be affected at
          all.

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK
  - HDK switches and pin configuration
    - N/A
  - Note:
    - To simplify the example project, raw data received from the RX link is
      shown on the output log.
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
      - COM port settings. baudrate: 115200, data: 8 bits, stop bit: 1,
        parity: none and flow control: off.

@par Directory contents
  - Source and header files
    - \b src/main.c:            Main program.
    - \b src/system_mt2523.c:   MT2523x clock configuration file.
    - \b src/ept_gpio_var.c:    GPIO pin assignment with variable names.
    - \b inc/hal_feature_config.h:
                                MT2523x feature configuration file.
    - \b inc/memory_map.h:      MT2523x memory layout symbol file.
    - \b GCC/startup_mt2523.s:  MT2523x startup file for GCC.
    - \b GCC/syscalls.c:        MT2523x syscalls for GCC.
    - \b MDK-ARM/startup_mt2523.s:
                                MT2523x startup file for Keil IDE.
  - Project configuration files using GCC
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile:    Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/i2s_internal_loopback.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/i2s_internal_loopback.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/i2s_internal_loopback.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/i2s_internal_loopback.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/i2s_internal_loopback.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project with a command "./build.sh mt2523_hdk
    i2s_internal_loopback" from the SDK root folder and download the binary
    file to LinkIt 2523 HDK.
  - Connect the HDK to the PC with a micro-USB cable.
  - Run the example. The transaction result is displayed in the log. Please
    refer to "Input to the example" and "Output from the example" sections.
*/
/**
 * @}
 * @}
 * @}
 */

