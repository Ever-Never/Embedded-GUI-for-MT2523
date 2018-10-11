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
 * @addtogroup linkit7697_hdk linkit7697_hdk
 * @{
 * @addtogroup linkit7697_hdk_apps apps
 * @{
 * @addtogroup linkit7697_hdk_apps_mbedtls mbedtls
 * @{

@par Overview
  - Application description
    - This application consists of some demonstrations for the features
      mbedtls provides, such as AES demonstration, MD5 demonstration, SSL
      client demonstration, and so on. For more detail of each demonstration,
      please refer to the descriptions below.
  - Features of the application
    - AES: AES encryption & decryption demonstration program.
    - MD5: md5 demonstration program.
    - MPI: MPI demonstration program.
    - RSA encrypt & decrypt: RSA encryption & decryption demonstration
      program.
    - PK encrypt & decrypt: Public key-based encryption & decryption program.
    - Selftest: Self-test demonstration program.
    - SSL client: SSL client demonstration program.
  - Results
    - The output is collected in a log.

@par Hardware and software environment
  - Supported platform
    - LinkIt 7697 HDK.
  - Environment configuration
    - The output logs are communicated through a micro-USB cable to the PC
      from USB (CON5) connector on the HDK.
      - Install the mbed serial driver according to the instructions at
        https://developer.mbed.org/handbook/Windows-serial-configuration. For
        more information, please refer to section "Installing the LinkIt 7697
        HDK drivers on Microsoft Windows" on the "LinkIt 7697 HDK User Guide"
        in [sdk_root]/doc folder.
      - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
        MK20 micro-B USB connector on the LinkIt 7697 HDK. For more
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
    - \b src/aes_demo.c:          AES encryption & decryption demonstration.
                                  This file was derived from
                                  https://tls.mbed.org/, MediaTek modified it
                                  to integrate it in this example project.
    - \b src/md5_demo.c:          MD5 demonstration. This file was derived from
                                  https://tls.mbed.org/, MediaTek modified it
                                  to integrate it in this example project.
    - \b src/mpi_demo.c:          MPI demonstration. This file was derived from
                                  https://tls.mbed.org/, MediaTek modified it
                                  to integrate it in this example project.
    - \b src/pk_encrypt.c:        PK encryption demonstration. This file was
                                  derived from https://tls.mbed.org/, MediaTek
                                  modified it to integrate it in this example
                                  project.
    - \b src/pk_decrypt.c:        PK decryption demonstration. This file was
                                  derived from https://tls.mbed.org/, MediaTek
                                  modified it to integrate it in this example
                                  project.
    - \b src/rsa_encrypt.c:       RSA encryption demonstration. This file was
                                  derived from https://tls.mbed.org/, MediaTek
                                  modified it to integrate it in this example
                                  project.
    - \b src/rsa_decrypt.c:       RSA decryption demonstration. This file was
                                  derived from https://tls.mbed.org/, MediaTek
                                  modified it to integrate it in this example
                                  project.
    - \b src/selftest.c:          Self-test demonstration. This file was
                                  derived from https://tls.mbed.org/, MediaTek
                                  modified it to integrate it in this example
                                  project.
    - \b src/ssl_client.c:        SSL client demonstration. This file was
                                  derived from https://tls.mbed.org/, MediaTek
                                  modified it to integrate it in this example
                                  project.
    - \b src/wifi_lwip_helper.c:  Network helper function for Wi-Fi station
                                  mode initialization until IP network ready.
    - \b src/ept_eint_var.c:      The EINT configuration file generated by Easy
                                  Pinmux Tool(EPT). Please do not edit the
                                  file.
    - \b src/ept_gpio_var.c:      The GPIO configuration file generated by Easy
                                  Pinmux Tool(EPT). Please do not edit the
                                  file.
    - \b src/system_mt7687.c:     MT76x7 clock configuration file.
    - \b src/sys_init.c:          This file initializes the basic hardware
                                  environment for the system, including system
                                  clock, UART port for logging, Pinmux, cache
                                  and other necessary hardware.
    - \b inc/mbedtls_example.h:   Mbedtls demonstration entries.
    - \b inc/task_def.h:          Define the task stack size, queue length,
                                  project name, and priority for the
                                  application to create tasks.
    - \b inc/hal_feature_config.h:
                                  MT76x7 feature configuration file.
    - \b inc/flash_map.h:         MT76x7 memory layout symbol file.
    - \b inc/ept_eint_drv.h:      The EINT configuration file generated by Easy
                                  Pinmux Tool(EPT). Please do not edit the
                                  file.
    - \b inc/ept_gpio_drv.h:      The GPIO configuration file generated by Easy
                                  Pinmux Tool(EPT). Please do not edit the
                                  file.
    - \b inc/sys_init.h:          The header file of sys_init.c.
    - \b inc/wifi_lwip_helper.h:  The header file of sta_network.c.
    - \b inc/lwipopts.h:          The configuration file of tcpip feature.
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
    - \b MDK-ARM/mbedtls.uvprojx:  uVision5 project file. Contains the project
                                   structure in XML format.
    - \b MDK-ARM/mbedtls.uvoptx:   uVision5 project options. Contains the
                                   settings for the debugger, trace
                                   configuration, breakpoints, currently open
                                   files, etc.
    - \b MDK-ARM/flash.sct:        Linker script.
  - Project configuration files using IAR
    - \b EWARM/mbedtls.ewd:  IAR project options. Contains the settings for the
                             debugger.
    - \b EWARM/mbedtls.ewp:  IAR project file. Contains the project structure
                             in XML format.
    - \b EWARM/mbedtls.eww:  IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:    Linker script.

@par Run the application
  - Build the application with the command "./build.sh linkit7697_hdk mbedtls"
    from the SDK root folder and download the binary file to the LinkIt 7697
    evelopment board.
  - Connect your board to the PC with a micro-USB cable.
  - Set up a WiFi router which connects to the internet.
    - Router settings: name: SQA_TEST_AP, password: 77777777, auth mode:
      WPA_PSK_WPA2_PSK.
    - You can also change the default settings in main.c to adapt the device
      to your Wi-Fi Access Point by specifying authentication mode,
      encryption mode, SSID, and password.
  - Run the application. The transaction result is displayed in the log.
    "example project test success" printed in the log indicates a success.
*/
/**
 * @}
 * @}
 * @}
 */
