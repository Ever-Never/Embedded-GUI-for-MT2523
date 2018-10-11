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
 * @addtogroup mt2523_hdk_hal_examples_ept_configure_pins_by_ept ept_configure_pins_by_ept
 * @{

@par Overview
  - Example description
    - This example shows how to use the EPT (Easy Pinmux Tool) to configure
      and initialize the I/O pins on the target board.
    - This example does not require FreeRTOS.
  - Results
    - The output is printed in a log.

@par Hardware and software environment
  - Supported platform
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
    - COM port settings. baudrate: 115200, data: 8bits, stop bit: 1, parity:
      none and flow control: off.

@par Directory contents
  - Source and header files
    - \b src/main.c:           Main program.
    - \b src/ept_gpio_var.c:   GPIO pin assignment with variable names.
    - \b src/ept_eint_var.c:   EINT assignment file with variable names.
    - \b src/system_mt2523.c:  MT2523x system clock configuration file.
    - \b inc/hal_feature_config.h:
                               MT2523x feature configuration file.
    - \b inc/memory_map.h:     MT2523x memory layout symbol file.
    - \b inc/ept_eint_drv.h:   EINT basic configuration header file.
    - \b ept_gpio_drv.h.              GPIO pin assignment and basic
      configuration file.
    - \b inc/ept_keypad_drv.h:  Keypad basic configuration header file.
    - \b GCC/startup_mt2523.s:  MT2523x startup file for GCC.
    - \b GCC/syscalls.c:        MT2523x syscalls for GCC.
    - \b MDK-ARM/startup_mt2523.s:
                                MT2523x startup file for Keil IDE.
  - Project configuration files using GCC
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile:    Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/ept_configure_pins_by_ept.uvprojx:
                             uVision5 project File. Contains the project
                             structure in XML format.
    - \b MDK-ARM/ept_configure_pins_by_ept.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/ept_configure_pins_by_ept.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/ept_configure_pins_by_ept.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/ept_configure_pins_by_ept.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Copy EPT generated files to the target project folder.
    - Copy ept_gpio_var.c and ept_eint_var.c to
      /project/mt2523_hdk/hal_examples/ept_configure_pins_by_ept/src/.
    - Copy ept_eint_drv.h, ept_gpio_drv.h and ept_keypad_drv.h to
      /project/mt2523_hdk/hal_examples/ept_configure_pins_by_ept/inc/.
    - Call the API bsp_ept_gpio_setting_init() to configure the EPT and
      on-board pins.
  - Build the example project with a command "./build.sh
    mt2523_hdk ept_configure_pins_by_ept bl" from the SDK root
    folder and download the binary file to LinkIt 2523 HDK.
  - Connect the HDK to the PC with a type-A to micro-B USB cable
    and specify the port on Tera terminal corresponding to "mbed Serial Port".
  - Run the example.The result is displayed in the log and
    "Pins have been initialized according to the configuration of EPT"
    indicates a successful operation.

@par Configuration
  - There are several variable names that can be assigned to different pins
    according to hardware and software design with help of EPT. All the GPIO
    and EINT variable names and the role assignment are provided in the table
    below. The software can use variable names to configure the corresponding
    pin without the need to know the pin number in the low level. This
    simplifies the work of porting the software to different platforms.
  - As for the GPIO variable names, it is necessary to assign the GPIO
    variable name to the correct pin according to the current pinmux setting
    or the corresponding peripheral may not work properly or the platform
    will not boot. For example, "HAL_KEYPAD_COL2_PIN", if one pin works in
    KCOL1 mode for keypad, the variable name of "HAL_KEYPAD_COL2_PIN" should
    be assigned to this pin, or something will be wrong with keypad module.
    The role assignment of all the GPIO variable names can be obtained from
    the table below.
  - As for the EINT variable name, it has the same effect as GPIO variable
    name other than that it is assigned to a different EINTx on the EINT page
    of EPT if the EINT has been assigned to a pin. Taking EINT variable name
    of "HAL_ACCDET_EINT" for example, if one pin is configured to work in
    EINT mode for ACCDET, variable name of "HAL_ACCDET_EINT" should be
    assigned to this pin, or something will be wrong with ACCDET module or
    the platform cannot boot.
  - The variable names and role assignment can be obtained from the table
    below, please pay extra attention on the comment field.
    | Module|Role of variable names assigned to pins   |  GPIO variable name           | EINT variable name |  Comment                                     |
    |-------|------------------------------------------|-------------------------------|--------------------|----------------------------------------------|
    |keypad |Chosen as KCOL2 for keypad                |HAL_KEYPAD_COL2_PIN            |                    |                                              |
    |keypad |Chosen as KCOL1 for keypad                |HAL_KEYPAD_COL1_PIN            |                    |                                              |
    |keypad |Chosen as KCOL0 for keypad                |HAL_KEYPAD_COL0_PIN            |                    |                                              |
    |keypad |Chosen as KROW2 for keypad                |HAL_KEYPAD_ROW2_PIN            |                    |                                              |
    |keypad |Chosen as KROW1 for keypad                |HAL_KEYPAD_ROW1_PIN            |                    |                                              |
    |keypad |Chosen as KROW0 for keypad                |HAL_KEYPAD_ROW0_PIN            |                    |                                              |
    |CTP    |Chosen as GPIOx for TP_RST                |BSP_CTP_RST_PIN                |                    |                                              |
    |CTP    |Chosen as EINTx for TP_EINT               |BSP_CTP_EINT_PIN               |BSP_CTP_EINT        |Pull up the pin corresponding to this EINTx.  |
    |CTP    |Chosen as SCLx for TP_I2C_SCL             |BSP_CTP_SCL_PIN                |                    |                                              |
    |CTP    |Chosen as SDAx for TP_I2C_SDA             |BSP_CTP_SDA_PIN                |                    |                                              |
    |ACCDET |Chosen as EINTx for ACCDET                |                               |HAL_ACCDET_EINT     |Disable the pin corresponding to this EINTx.  |
    |AUDIO  |Chosen as "SPEAKER" for audio             |BSP_SPEAKER_EBABLE_PIN         |                    |                                              |
    |AUDIO  |Chosen as "AUXADC" for audio              |BSP_AUXADC_ENABLE_PIN          |                    |                                              |
    |MSDC   |Chosen as EINTx for MSDC                  |                               |HAL_MSDC_EINT       |Pull up the pin corresponding to this EINTx.  |
    |GNSS   |Chosen as GPIOx for GNSS_LDO_EN           |BSP_GNSS_POWER_PIN             |                    |                                              |
    |GNSS   |Chosen as EINTx for GNSS_EINT             |                               |BSP_GNSS_EINT       |                                              |
    |Senser |Chosen as GPIOx for PPG_VDRV_EN           |BSP_BIO_SENSOR_PPG_VDRV_EN     |                    |                                              |
    |Senser |Chosen as CLKOx of 32KHz for bio sensor   |BSP_BIO_SENSOR_32K             |                    |                                              |
    |Senser |Chosen as GPIOx for reset control         |BSP_BIO_SENSOR_RST_PORT_PIN    |                    |                                              |
    |Senser |Chosen as GPIOx for power down control    |BSP_BIO_SENSOR_AFE_PWD_PIN     |                    |                                              |
    |WIFI   |Chosen as CLKOx of 32KHz for MT5931 WIFI  |BSP_WIFI_32K_PIN               |                    |                                              |
    |WIFI   |Chosen as CLKOx of 26MHz for MT5931 WIFI  |BSP_WIFI_26M_PIN               |                    |                                              |
    |WIFI   |Chosen as EINTx for MT5931 WIFI           |                               |BSP_WIFI_EINT       |                                              |
    |WIFI   |Chosen as GPIOx for MT5931 WIFI to Reset  |BSP_WIFI_RESET_PIN             |                    |                                              |
    |WIFI   |Chosen as GPIOx for MT5931 WIFI Enable    |BSP_WIFI_ENABLE_PIN            |                    |                                              |
    |modem  |Chosen as GPIOx to update status to modem |UPDATE_HOST_STATUS_PIN         |                    |Communication with MT6280 modem               |
    |modem  |Chosen as GPIOx to query modem status     |QUERY_MODEM_STATUS_PIN         |                    |Communication with MT6280 modem               |
    |modem  |Chosen as EINTx for modem notify exception|NOTIFY_MODEM_EXCEPTION_PIN     |MODEM_EXCEPTION_EINT|Communication with MT6280 modem               |
    |modem  |Chosen as GPIOx to trigger modem reset    |TRIGGER_MODEM_RESET_PIN        |                    |Communication with MT6280 modem               |
    |modem  |Chosen as EINTx for modem notify wakeup   |NOTIFY_MODEM_WAKEUP_PIN        |MODEM_WAKEUP_EINT   |Communication with MT6280 modem               |
    |modem  |Chosen as GPIOx to trigger modem start    |TRIGGER_MODEM_START_PIN        |                    |Communication with MT6280 modem               |
    |modem  |Chosen as GPIOx to trigger modem wakeup   |TRIGGER_MODEM_WAKEUP_PIN       |                    |Communication with MT6280 modem               |
*/
/**
 * @}
 * @}
 * @}
 */
