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
 * @addtogroup mt2523_hdk_hal_examples_spi_master_to_slave spi_master_to_slave
 * @{

@par Overview
  - Example description
    - This example is a reference application to demonstrate how to use SPI
      master API to communicate with an SPI slave with two LinkIt 2523 HDKs.
    - This example does not require FreeRTOS.
  - Results
    - The system result is in the log.
  - Features of the example project
    - The example project has two parts: 1. LinkIt 2523 HDK operates as SPI
      Master device (spi_master_to_slave). 2. LinkIt 2523 HDK operates as SPI
      Slave device (spi_slave_from_master). The communication between master
      and slave devices is shown below.
      \code
      _________________________                       __________________________
      |           ______________|                     |______________            |
      |         |SPI Master     |                     |     SPI Slave|           |
      |         |               |                     |              |           |
      |         CON6301 Left.6  |_____________________|CON6301 Left.6            |
      |         |               |                     |              |           |
      |         CON6301 Left.7  |_____________________|CON6301 Left.7            |
      |         |               |                     |              |           |
      |         CON6301 Left.8  |_____________________|CON6301 Left.8            |
      |         |               |                     |              |           |
      |         CON6301 Left.9  |_____________________|CON6301 Left.9            |
      |         |_______________|                     |______________|           |
      |                         |                     |                          |
      |                         |                     |                          |
      |                      GND|_____________________|GND                       |
      |                         |                     |                          |
      |_MT2523__________________|                     |_MT2523___________________|
      \endcode

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK.
  - HDK switches and pin configuration
    - Connect J1007.2 to J1007.3
    - Connect J1009.2 to J1009.3
    - Connect J1010.2 to J1010.3
    - Connect J1011.2 to J1011.3
    - SPI master module pin mapping table is shown below.
      | SPI Pin | GPIOx     |    PINx          |
      |-------  |---------  |----------------  |
      |  CS_N   | GPIO_25   | CON6301 Left.6   |
      |  SCK    | GPIO_26   | CON6301 Left.7   |
      |  MOSI   | GPIO_27   | CON6301 Left.8   |
      |  MISO   | GPIO_28   | CON6301 Left.9   |
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
    - \b src/main.c:            Main program.
    - \b src/system_mt2523.c:   MT2523x system clock configuration file.
    - \b inc/hal_feature_config.h:
                                MT2523x feature configuration file.
    - \b inc/memory_map.h:      MT2523x memory layout symbol file.
    - \b GCC/startup_mt2523.s:  MT2523x startup file for GCC.
    - \b GCC/syscalls.c:        MT2523x syscalls for GCC.
    - \b MDK-ARM/startup_mt2523.s:
                                MT2523x startup file for Keil IDE.
  - Project configuration files using GCC
    - \b GCC/feature.mk:  Feature configuration file.
    - \b GCC/Makefile.:   Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/spi_master_to_slave.uvprojx:
                             uVision5 Project File. Contains the project
                             structure in XML format.
    - \b MDK-ARM/spi_master_to_slave.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/spi_master_to_slave.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/spi_master_to_slave.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/spi_master_to_slave.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project for Master with a command "./build.sh
    mt2523_hdk spi_master_to_slave bl" from the SDK root folder and download
    the binary file to the LinkIt 2523 HDK that operates as SPI Master.
  - Build the example project for Slave with a command "./build.sh mt2523_hdk
    spi_slave_from_master bl" from the SDK root folder and download the
    binary file to the 2523 HDK that operates as SPI Slave.
  - Connect the related pins of the two LinkIt 2523 HDKs as shown in the
    "Features of the example project" and "HDK switches and pin
    configuration" sections.
  - Connect the HDK to the PC with a type-A to micro-B USB cable and specify
    the port on Tera terminal corresponding to "mbed Serial Port".
  - Run the example. The log will show the communication result with the SPI
    Slave. The log of "spi_master_send_data_two_boards_example ends" for
    master board and "spi_slave_receive_data_two_boards_example ends" for
    slave board indicates a successful communication. Please make sure the
    two LinkIt 2523 HDK boards (Master and Slave) are powered up at the same
    time, no more than 2 seconds apart.
*/
/**
 * @}
 * @}
 * @}
 */
