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
 * @addtogroup mt7687_hdk_hal_examples_des_encrypt_decrypt_data des_encrypt_decrypt_data
 * @{

@par Overview
  - Example project description
    - This project is a reference application to use the DES APIs for
      encryption
    /decryption, including these modes.
    - CBC (Cipher Block Chaining)
    - ECB (Electronic CodeBook)
    - The application does not require FreeRTOS.
  - Results
    - The calculation output is collected in a log.

@par Hardware and software environment
  - Supported platform
    - LinkIt 7687 HDK.
  - Environment configuration
    - This example project will output logs via HDK USB (CON5) to PC. Please
      connect a micro USB cable from a PC to CON5 port, install the mbed
      serial driver, and launch Tera terminal in PC for logging as below.
      - Require a type-A to micro-B USB cable to connect type-A USB of PC and
        micro-B USB of the LinkIt 7687 HDK development board. For more
        information about type-A plug and micro-B plug, please refer to
        https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
      - Install the mbed serial driver as instructed at
        https://developer.mbed.org/handbook/Windows-serial-configuration. For
        more information, please refere to the installing the LinkIt 7687 HDK
        drivers on Microsoft Windows section of the LinkIt 7687 HDK user
        guides under [sdk_root]/doc folder.
      - A serial tool for UART logging is required, such as Tera terminal.
        Please refer to the installing Tera terminal on Microsoft Windows
        section of get startedguide under [sdk_root]/doc folder for more
        information.
      - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1,
        parity: none and flow control: off.

@par Directory contents
  - Source and header files
    - \b src/main.c:              Main program.
    - \b src/system_mt7687.c:     MT76x7 clock configuration file.
    - \b inc/hal_feature_config.h:
                                  MT76x7 feature configuration file.
    - \b inc/flash_map.h:         MT76x7 memory layout symbol file.
    - \b GCC/syscalls.c:          MT76x7 syscalls for GCC.
    - \b GCC/startup_mt7687.s:    MT76x7 startup file for GCC.
    - \b MDK-ARM/startup_mt7687.s:
                                  MT76x7 startup file for Keil IDE.
    - \b EWARM/startup_mt7687.s:  MT76x7 startup file for IAR.
  - Project configuration files using GCC
    - \b GCC/feature.mk:       Feature configuration.
    - \b GCC/Makefile:         Makefile.
    - \b GCC/mt7687_flash.ld:  Linker script.
  - Project configuration files using Keil
    - \b MDK-ARM/des_encrypt_decrypt_data.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/des_encrypt_decrypt_data.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/des_encrypt_decrypt_data.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/des_encrypt_decrypt_data.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/des_encrypt_decrypt_data.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project with the command, "./build.sh mt7687_hdk
    des_encrypt_decrypt_data" from the SDK root folder and download the bin
    file to LinkIt 7687 development board.
  - Connect your board to the PC with a micro USB cable.
  - Run the example. The result of its CBC/ECB encryption/decryption is
    displayed in the log.
*/
/**
 * @}
 * @}
 * @}
 */

