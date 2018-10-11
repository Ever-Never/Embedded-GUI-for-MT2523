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
 * @addtogroup mt2523_hdk_apps_sensor_subsys_accelerometer sensor_subsys_accelerometer
 * @{

@par Overview
  - This project is a reference application on getting the accelerometer
    sensor data using the sensor subsystem API.
  - Enable the sensor
    - Call the function #hal_pinmux_set_function() or use the EPT tool to set
      the GPIO pinmux to configure the I2C digital interface and external
      interrupt pinmux settings. Please refer to bsp_sensor_peripheral_init()
      in [sdk_root]/driver/board/mt25x3_hdk/mems_init/mems_init.c.
    - Sensor subsystem task (sensor_mgr_task() in
      [sdk_root]/middleware/MTK/sensor_subsys/src/sensor_manager.c) gets
      accelerometer sensor data through an interrupt.
  - Results
    - The received accelerometer data is displayed in the log.

@par Hardware and software environment
  - Supported HDK
    - MediaTek LinkIt 2523 HDK.
    - MediaTek MT2511 biosensor daughterboard (model number: MTK0067).
      Alternatively, a Bosch BMA255 shuttle board can be used as a
      substitute. For more information on BMA255 shuttle board, please visit
      https://www.bosch-sensortec.com/bst/home/home_overview.
      - The connection of the digital interface between BMA255 shuttle board
        and LinkIt 2523 HDK is shown below.
        - Find the <b>Sensor</b> connector on LinkIt 2523 HDK.
        - Connection configuration
          |LinkIt 2523 HDK PIN Name |Sensor connector(Con6301) PIN # |GPIOx  |BMA255 shuttle board PIN # |
          |-------                  |-----                           |-------|-------                    |
          |VIO28                    |3                               | -     |1, 2, 9                    |
          |SDA0                     |7                               |GPIO37 |5                          |
          |SCL0                     |9                               |GPIO36 |6                          |
          |U304_EINT_CONN           |17                              |GPIO5  |20                         |
          |GND                      |23, 24                          | -     |3, 4                       |
        - Please note, the pull-up resistors are needed for the I2C interface
          to provide the sufficient driving power to the I2C clock and data
          lines. On LinkIt 2523 HDK, the two pull-up resistors R1003 and
          R1002 are already installed for the I2C0 interface. When using a
          different I2C interface, please make sure the pull-up resistors
          (normally 4.7 kohm) are installed for that particular I2C
          interface.
      - The BMI160 sensor component integrated with triaxial accelerometer
        and gyroscope is on the MTK0067 daughterboard. To use BMI160, a 0 ohm
        resistor is to be mounted on R6403 (default no connection), which is
        near BMI160 (U6302) on the MTK0067 daughterboard. The connection
        configuration
        |LinkIt 2523 HDK PIN Name |Sensor connector(Con6301) PIN # |GPIOx  |
        |-------                  |-----                           |-------|
        |VIO28                    |3                               | -     |
        |MA_SPI3_B_MISO           |8                               |GPIO28 |
        |MA_SPI3_B_MOSI           |10                              |GPIO27 |
        |MA_SPI3_B_SCK            |12                              |GPIO26 |
        |MA_SPI3_B_CS1            |13                              |GPIO48 |
        |U302_EINT_CONN           |19                              |GPIO6  |
        |GND                      |23, 24                          | -     |

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
    - \b MTK_SENSOR_ACCELEROMETER_USE_BMA255: Selects and enables the driver
      for BMA255 component. Please note that
      MTK_SENSOR_ACCELEROMETER_USE_BMA255 and
      MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160 should be exclusively
      defined.
    - \b MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160: Selects and enables
      the driver for BMI160 component. Please note that
      MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160 and
      MTK_SENSOR_ACCELEROMETER_USE_BMA255 should be exclusively defined.

@par Directory contents
  - Source and header files
    - \b src/main.c:              Main program.
    - \b src/system_mt2523.c:     The configuration file of the Cortex-M4 with
                                  floating point core registers and system
                                  clock.
    - \b src/sys_init.c:          This file initializes the basic hardware
                                  environment for the system, including system
                                  clock, UART port for logging, pinmux, cache
                                  and other necessary hardware.
    - \b src/ept_eint_var.c:      The EINT configuration file generated by Easy
                                  Pinmux Tool(EPT). Please do not edit the
                                  file.
    - \b src/ept_gpio_var.c:      The GPIO configuration file generated by Easy
                                  Pinmux Tool(EPT). Please do not edit the
                                  file.
    - \b inc/FreeRTOSConfig.h:    FreeRTOS feature configuration file.
    - \b inc/task_def.h:          Define the task stack size, queue length,
                                  project name, and priority for the
                                  application to create tasks.
    - \b inc/hal_feature_config.h:
                                  MT2523x feature configuration file.
    - \b inc/memory_map.h:        MT2523x memory layout symbol file.
    - \b inc/ept_eint_drv.h:      The EINT configuration file generated by Easy
                                  Pinmux Tool(EPT). Please do not edit the
                                  file.
    - \b inc/ept_gpio_drv.h:      The GPIO configuration file generated by Easy
                                  Pinmux Tool(EPT). Please do not edit the
                                  file.
    - \b inc/ept_keypad_drv.h:    The KEYPAD configuration file generated by
                                  Easy Pinmux Tool(EPT). Please do not edit the
                                  file.
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
    - \b GCC/syscalls.c:          The minimal implementation of the system
                                  calls.
    - \b MDK-ARM/startup_mt2523.s:
                                  MT2523x startup file for Keil IDE.
    - \b EWARM/startup_mt2523.s:  MT2523x startup file for IAR IDE.
    - \b \<sdk_root\>/driver/board/component/mems:
                                  The motion sensor driver.
    - \b \<sdk_root\>/middleware/MTK/sensor_subsys:
                                  The sensor manager implementation to control
                                  drivers and report data to application.
  - Project configuration files using GCC
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile:    Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/sensor_subsys_accelerometer.uvprojx:
                             uVision5 Project File. Contains the project
                             structure in XML format.
    - \b MDK-ARM/sensor_subsys_accelerometer.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently opened files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR IDE
    - \b EWARM/sensor_subsys_accelerometer.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/sensor_subsys_accelerometer.ewp:
                           IAR project file. Contains the project structure in
                           XML format
    - \b EWARM/sensor_subsys_accelerometer.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project with the command "./build.sh mt2523_hdk
    sensor_subsys_accelerometer" from the SDK root folder and download the
    binary file to LinkIt 2523 HDK.
    - This example project enables BMA255 by default. To change the
      configuration for enabling BMI160 using
      - GCC: modify GCC/feature.mk. Mark MTK_SENSOR_ACCELEROMETER_USE =
        BMA255 and unmark MTK_SENSOR_ACCELEROMETER_USE = BMI160.
      - Keil IDE: remove the definitions "MTK_SENSOR_ACCELEROMETER_USE" and
        "MTK_SENSOR_ACCELEROMETER_USE_BMA255". Add the definitions
        "MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE" and
        "MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160".
      - IAR IDE: remove the definitions "MTK_SENSOR_ACCELEROMETER_USE" and
        "MTK_SENSOR_ACCELEROMETER_USE_BMA255". Add the definitions
        "MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE" and
        "MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160".
  - Connect the HDK to the PC with a micro USB cable.
  - Run the example. Move MediaTek LinkIt 2523 HDK to trigger accelerometer
    data change. The log displays the update of the accelerometer data.
 * @}
 * @}
 * @}
 */
