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
 * @addtogroup mt7687_hdk_hal_examples_i2c_control_EEPROM_with_dma i2c_control_EEPROM_with_dma
 * @{

@par Overview
  - Example description
    - This example is a reference application to use the I2C APIs to
      communicate with an I2C M24C01 EEPROM device in the I2C dma_extended
      mode.
    - This example does not require FreeRTOS.
  - Results
    - The transaction output is printed in the log.

@par Hardware and software environment
  - Supported platform
    - LinkIt 7687 HDK.
  - HDK switches and pin configuration
    - I2C0 pin mapping is listed below.
      |GPIOx    |PINx   |
      |---------|-------|
      |  27     |J34.1  |
      |  28     |J34.2  |
  - Note:
    - The pull-up resistors are needed for the I2C interface to provide the
      sufficient driving power to the I2C clock and data lines. On LinkIt
      7687 HDK, the two pull-up resistors R1003 and R1002 are already
      installed for the I2C0 interface.When using a different I2C interface,
      please make sure the pull-up resistors (normally 4.7kohm) are installed
      for that particular I2C interface.
  - Environment configuration
    - The output logs are communicated through a type-A to micro-B USB cable
      to the PC from USB (CON5) connector on the HDK.
    - Install the mbed serial driver according to the instructions at
      https://developer.mbed.org/handbook/Windows-serial-configuration. For
      more information, please refer to section "Installing the LinkIt 7687
      HDK drivers on Microsoft Windows" on the "LinkIt 7687HDK User Guide" in
      [sdk_root]/doc folder.
    - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
      CON5 micro-B USB connector on the LinkIt 7687 HDK. For more information
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
    - \b GCC/Makefile.:        Makefile.
    - \b GCC/mt7687_flash.ld:  Linker script.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/i2c_control_EEPROM_with_dma.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/i2c_control_EEPROM_with_dma.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/i2c_control_EEPROM_with_dma.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/i2c_control_EEPROM_with_dma.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/i2c_control_EEPROM_with_dma.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project with a command "./build.sh mt7687_hdk
    i2c_control_EEPROM_with_dma" from the SDK root folder and download the
    binary file to the LinkIt 7687 HDK.
  - Connect J34.1 and J34.2 to the SCL (clock) and SDA (data) pins of the
    M24C01 EEPROM device, respectively. Connect VCC of EEPROM to J25.2 and
    GND of EEPROM to J34.4 on LinkIt 7687 HDK.
  - Connect the HDK to the PC with a type-A to micro-B USB cable and specify
    the port on Tera terminal corresponding to "mbed Serial Port".
  - Run the example. The transaction result is displayed in the log and "I2C
    test is successful" indicates a successful data communication.
*/
/**
 * @}
 * @}
 * @}
 */

