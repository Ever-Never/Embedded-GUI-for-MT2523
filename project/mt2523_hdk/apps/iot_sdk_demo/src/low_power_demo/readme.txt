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
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo iot_sdk_demo
 * @{
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo_low_power_demo low_power_demo
 * @{

@par Overview
  - This demo project that can be customized to measure the current
    consumption of different application scenario and shows how to configure
    for each application scenario.

@par Hardware and software environment
  - Supported HDK
    - LinkIt 2523 HDK.
  - Enviroment configuration
    - Use power meter to verify the system current from measure VBAT.

@par Directory contents
  - Source and header files
    - \b src/low_power_demo/sleep_at_cmd.c:
      The source file of the sleep at command.
    - \b src/low_power_demo/bt_at_cmd.c:
      The source file of the BT at command.
    - \b src/low_power_demo/gnss_st_cmd.c:
      The source file of the GNSS at command.

@par Run the demo
  - Build the example project with the command, "./build.sh mt2523_hdk
    iot_sdk_demo" from the SDK root folder and download the binary file to
    LinkIt 2523 development board.
  - Connect the board to PC with serial port cable.
  - Connect external power source, please refer to
    <sdk_root>/doc/board/mt2523_hdk/LinkIt_2523_HDK_Power_Measurement_Guide.pdf .
  - By default, the application uses USB-COM1 for AT command input.
  - Then enter "AT+EPORT=2,syslog,0\0d\0a", the result "OK" indicates the
    syslog automatically changed at runtime to use UART0.
  - Then enter "AT+EPORT=2,atci,1\0d\0a", the result "OK" indicates the AT
    command automatically changed at runtime to use UART1.
  - plug out USB cable.
  - This at command force the system to enter deep sleep mode for power
    saving operations
    - The steps are shown below:
      - Step1: input at command "AT+SM_DEMO=8,0\0d\0a" to close BT MTCMOS.
      - Step2: input at command "AT+SM_DEMO=8,3\0d\0a" disable battery
        manager.
      - Step3: input at command "AT+SM_DEMO=8,4\0d\0a" close VA28 LDO.
      - Step4: input at command "AT+SM_DEMO=6,2\0d\0a" to close all task.
      - Step5: input at command "AT+SM_DEMO=6,1\0d\0a" to unlock sleep
        handle.
      - Follow the above steps, you can verify the system low power status.
  - This at command force the system to enter sleep mode for GNSS power
    saving operations
    - The steps are shown below:
      - Step1: input at command "AT+SM_DEMO=8,0\0d\0a" to close BT MTCMOS.
      - Step2: input at command "AT+SM_DEMO=8,3\0d\0a" disable battery
        manager.
      - Step3: input at command "AT+SM_DEMO=8,4\0d\0a" close VA28 LDO.
      - Step4: input at command "AT+SM_DEMO=6,2\0d\0a" to close all task.
      - Step5: input at command "AT+EGPSC_DEMO=1\0d\0a" to open GNSS.
      - Step6: input at command "AT+SM_DEMO=6,1\0d\0a" to unlock sleep handle
      - Ensure to test it in an open area where the line of sight GNSS signal
        can be received.
      - Waiting about 15 minute for GNSS chip download the ephemeris, you can
        verify the
      - low power on GNSS tracking mode.

@par Bluetooth low power test
  - There are 6 cases for Bluetooth low power, the detailed info is shown
    below:
    - Case 1.
      - Description:
        - BLE 3T3R 1.28s Advertising(9 bytes Data"Mediatek")
      - The steps are shown below:
        - steps1: Power on device.
        - steps2: AT+SM_DEMO=8,3\0d\0a.
        - steps3: AT+SM_DEMO=8,4\0d\0a.
        - steps4: AT+SM_DEMO=6,2\0d\0a.
        - steps5: AT+EBTTEST_DEMO=1\0d\0a.
        - steps6: AT+EBLESA_DEMO=0\0d\0a.
        - steps7: AT+EBLESAD_DEMO=C3:06:BB:02:01:C0\0d\0a.
        - steps8: AT+EBLESEA_DEMO=1,2048,2048,0,1,09094D6564696174656B\0d\0a.
        - steps9: AT+EBTES_DEMO=0\0d\0a after bt swithed on (step 6) 90s
          later.
        - steps10: AT+SM_DEMO=6,1\0d\0a.
    - Case 2.
      - Description:
        - BLE 1T1R 1.28s Connection
      - The steps are shown below:
        - steps1: Power on device.
        - steps2: AT+SM_DEMO=8,3\0d\0a.
        - steps3: AT+SM_DEMO=8,4\0d\0a.
        - steps4: AT+SM_DEMO=6,2\0d\0a.
        - steps5: AT+EBTTEST_DEMO=1\0d\0a.
        - steps6: AT+EBLESA_DEMO=0\0d\0a.
        - steps7: AT+EBLESAD_DEMO=C3:06:BB:02:01:C0\0d\0a.
        - steps8: AT+EBLESEA_DEMO=1,1024,1024,0,1,09094D6564696174656B\0d\0a.
        - steps9: Wait for connection (From Device B).
        - steps10: AT+EBTES_DEMO=0\0d\0a after bt switched on (step 6) 90s
          later.
        - steps11: AT+SM_DEMO=6,1\0d\0a.
        - [Device_B_for_BLE_Connection_Test (Device_B is same as deivce under
          of test, not SP)].
        - 1.AT+EBLECON_DEMO=C3:06:BB:02:01:C0,1,1024,1024,0,1000\0d\0a.
    - Case 3.
      - Description:
        - BT Classic Paging 1.28s
      - The steps are shown below:
        - steps1: Power on device.
        - steps2: AT+SM_DEMO=8,3\0d\0a.
        - steps3: AT+SM_DEMO=8,4\0d\0a.
        - steps4: AT+SM_DEMO=6,2\0d\0a.
        - steps5: AT+EBTTEST_DEMO=1\0d\0a.
        - steps6: AT+EBTES_DEMO=0\0d\0a.
        - steps7: AT+EBTES_DEMO=2\0d\0a.
        - steps8: AT+SM_DEMO=6,1\0d\0a after bt is switched on (step 6) 90s
          later.
    - Case 4.
      - Description:
        - BT Classic Sniff 0.5s, 4 attempts
      - The steps are shown below:
        - steps1: Power on device.
        - steps2: AT+SM_DEMO=8,3\0d\0a.
        - steps3: AT+SM_DEMO=8,4\0d\0a.
        - steps4: AT+SM_DEMO=6,2\0d\0a.
        - steps5: AT+EBTTEST_DEMO=1\0d\0a.
        - steps6: AT+EBTES_DEMO=3\0d\0a.
        - steps7: Smart phone(made from Mediatek) connects to device.
        - steps8: AT+EBTES_DEMO=0\0d\0a after BT switched on (step 6) 90s
          later.
        - steps9: AT+EBTSI_DEMO=2048,2048,4,1\0d\0a.
        - steps10: Wait for 60 secs to ensure device entering sniff state.
        - steps11: AT+SM_DEMO=6,1\0d\0a.
    - Case 5.
      - Description:
        - BT Voice
      - The steps are shown below:
        - steps1: Power on device.
        - steps2: AT+SM_DEMO=8,3\0d\0a.
        - steps3: AT+SM_DEMO=8,4\0d\0a.
        - steps4: AT+SM_DEMO=6,2\0d\0a.
        - steps5: AT+EBTTEST_DEMO=1\0d\0a.
        - steps6: AT+EBTES_DEMO=3\0d\0a.
        - steps7: pair device (device name: BT_Audio_Demo).
        - steps8: AT+EBTES_DEMO=0\0d\0a after bt is switched on (step 6) 90s
          later.
        - steps9: make a call to smart phone.
    - Case 6.
      - Description:
        - BT A2DP
      - The steps are shown below:
        - steps1: Power on device.
        - steps2: AT+SM_DEMO=8,3\0d\0a.
        - steps3: AT+SM_DEMO=8,4\0d\0a.
        - steps4: AT+SM_DEMO=6,2\0d\0a.
        - steps5: AT+EBTTEST_DEMO=1\0d\0a.
        - steps6: AT+EBTES_DEMO=3\0d\0a.
        - steps7: pair device (device name: BT_Audio_Demo).
        - steps8: AT+EBTES_DEMO=0\0d\0a after bt is switched on (step 6) 90s
          later.
        - steps9: playback from host device.
*/
/**
 * @}
 * @}
 * @}
 */
*/
