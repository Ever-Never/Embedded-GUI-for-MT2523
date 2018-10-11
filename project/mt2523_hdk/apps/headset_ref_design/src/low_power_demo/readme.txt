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
 * @addtogroup mt2523_hdk mt2533_hdk 
 * @{
 * @addtogroup mt2523_hdk_apps apps
 * @{
 * @addtogroup mt2523_hdk_apps_headset_ref_design headset_ref_design
 * @{
 * @addtogroup mt2523_hdk_apps_headset_ref_design_low_power_demo low_power_demo
 * @{

@par Overview
  - This feature is designed to measure the current consumption
    of the MT2523 HDK in different use cases. 
    The feature can be customized based on the application requirements.

@par Hardware and software environment
  - Supported platform
    - MT2523 HDK.
  - Enviroment configuration
    - Use power meter to verify the system current from VBAT power rail.

@par Directory contents
  - Source and header files
    - \b src/low_power_demo/sleep_at_cmd.c:
      AT commands for low power mode.
    - \b src/low_power_demo/bt_at_cmd.c:
      AT commands for Bluetooth support.

@par Run the feature
  - Build and Download
    - Please refer to mt2523_hdk/apps/headset_ref_design/readme.txt.
  - Apply the following AT commands to power off the system.
      - Step 1. Input AT command "AT+SM_DEMO=5\0d\0a" to power off the system.
      - Step 2. Remove the USB cable once the system is powered off.
  - Apply the following AT commands to set the system into deep sleep mode
    for power saving operations.
      - Step 1. Input AT command "AT+SM_DEMO=8,0\0d\0a" to close the Bluetooth MTCMOS.
      - Step 2. Input AT command "AT+SM_DEMO=8,3\0d\0a" to disable the battery manager.
      - Step 3. Input AT command "AT+SM_DEMO=8,4\0d\0a" to close the VA28 LDO.
      - Step 4. Input AT command "AT+SM_DEMO=6,2\0d\0a" to close all tasks.
      - Step 5. Input AT command "AT+EPMICREG=1,36,a,ff\0d\0a" to close the gauge
        leakage.
      - Step 6. Input AT command "AT+EPMICREG=1,36,b,ff\0d\0a " to close the gauge
        leakage.
      - Step 7. Input AT command "AT+SM_DEMO=6,1\0d\0a" to unlock the sleep
        handle.
  - Apply the following steps and AT commands for MP3 local audio playback in power
    saving mode.
      -[Environment]
        - Step 1. Create a folder on SD card.
        - Step 2. Copy at least one MP3 file into the folder on SD card.
        - Step 3. Insert the SD card on MT2533 EVB.
      -[Use AT commands]
        - Step 4. AT+SM_DEMO=8,0\0d\0a.
        - Step 5. AT+SM_DEMO=8,3\0d\0a.
        - Step 6. AT+SM_DEMO=6,2\0d\0a.
        - Step 7. Wait for 2 to 5 seconds.
        - Step 8. AT+EAUD=LOCAL-LP\0d\0a.
        - Step 9. AT+EPMICREG=1,36,a,ff\0d\0a.
        - Step 10.AT+EPMICREG=1,36,b,ff\0d\0a.
        - Step 11.AT+EAUD=LOCAL-PLAY\0d\0a.
        - Step 12.AT+SM_DEMO=6,1\0d\0a.

@par Bluetooth low power test
  - There are 8 cases for Bluetooth low power operation, as shown below:
    - Case 1.
      - Description: Bluetooth LE 3T3R 1.28s Advertising (9 bytes Data"Mediatek")
        - Step 1. Power on the device.
        - Step 2. AT+SM_DEMO=8,3\0d\0a.
        - Step 3. AT+SM_DEMO=8,4\0d\0a.
        - Step 4. AT+SM_DEMO=6,2\0d\0a.
        - Step 5. Wait for about 2 seconds.
        - Step 6. AT+EBTTEST_DEMO=1\0d\0a.
        - Step 7. Wait for at least 5 seconds, or until "POWER ON" is displayed.
        - Step 8. AT+EBTES_DEMO=0\0d\0a.
        - Step 9. AT+EBLESA_DEMO=0\0d\0a.
        - Step 10.AT+EBLESAD_DEMO=C3:06:BB:02:01:C0\0d\0a.
        - Step 11.AT+EBLESEA_DEMO=1,2048,2048,0,1,09094D6564696174656B\0d\0a.
        - Step 12.AT+EPMICREG=1,36,a,ff\0d\0a.
        - Step 13.AT+EPMICREG=1,36,b,ff\0d\0a.
        - Step 14.AT+SM_DEMO=6,1\0d\0a.
    - Case 2.
      - Description: Bluetooth LE 1T1R 1.28s Connection.
          - Step 1. Power on device A.
          - Step 2. AT+SM_DEMO=8,3\0d\0a.
          - Step 3. AT+SM_DEMO=8,4\0d\0a.
          - Step 4. AT+SM_DEMO=6,2\0d\0a.
          - Step 5. Wait for 2 seconds.
          - Step 6. AT+EBTTEST_DEMO=1\0d\0a.
          - Step 7. Wait for at least 5 seconds, or until 
            "POWER ON" is displayed.
          - Step 8. AT+EBTES_DEMO=0\0d\0a.
          - Step 9. AT+EBLESA_DEMO=0\0d\0a.
          - Step 10.AT+EBLESAD_DEMO=C3:06:BB:02:01:C0\0d\0a.
          - Step 11.AT+EBLESEA_DEMO=1,1024,1024,0,1,09094D6564696174656B\0d\0a.
          - Step 12.Wait for connection (from Device B).
        - [Device_B_for_BLE_Connection_Test (Device_B is the same as device under
          test, not smartphone)].
          - Step 14. Power on the device B.
          - Step 15. AT+SM_DEMO=8,3\0d\0a.
          - Step 16. AT+SM_DEMO=8,4\0d\0a.
          - Step 17. AT+SM_DEMO=6,2\0d\0a.
          - Step 18. Wait for about 2 seconds.
          - Step 19. AT+EBTTEST_DEMO=1\0d\0a.
          - Step 20. Wait for least 4 seconds, or until "POWER ON" is displayed.
          - Steps 21.AT+EBLECON_DEMO=C3:06:BB:02:01:C0,1,1024,1024,0,1000\0d\0a.
        - [Device_A_for_BLE_Connection_Test].
          - Step 22. AT+EPMICREG=1,36,a,ff\0d\0a.
          - Step 23. AT+EPMICREG=1,36,b,ff\0d\0a.
          - Step 24. AT+SM_DEMO=6,1\0d\0a.
    - Case 3.
      - Description: Bluetooth Classic Paging 1.28s
        - Step 1. Power on the device.
        - Step 2. AT+SM_DEMO=8,3\0d\0a.
        - Step 3. AT+SM_DEMO=8,4\0d\0a.
        - Step 4. AT+SM_DEMO=6,2\0d\0a.
        - Step 5. Wait for about 2 seconds.
        - Step 6. AT+EBTTEST_DEMO=1\0d\0a.
        - Step 7. Wait for at least 5 seconds, or until "POWER ON" is displayed.
        - Step 8. AT+EBLESA_DEMO=0\0d\0a.
        - Step 9. AT+EBTES_DEMO=0\0d\0a.
        - Step 10.AT+EBTES_DEMO=2\0d\0a.
        - Step 11.AT+EPMICREG=1,36,a,ff\0d\0a.
        - Step 12.AT+EPMICREG=1,36,b,ff\0d\0a.
        - Step 13.AT+SM_DEMO=6,1\0d\0a.
    - Case 4.
      - Description: Bluetooth Classic Sniff 0.5s, 4 attempts
        - Step 1. Power on the device.
        - Step 2. AT+SM_DEMO=8,3\0d\0a.
        - Step 3. AT+SM_DEMO=8,4\0d\0a.
        - Step 4. AT+SM_DEMO=6,2\0d\0a.
        - Step 5. Wait for about 2 seconds.
        - Step 6. AT+EBTTEST_DEMO=1\0d\0a.
        - Step 7. Wait for at least 5 seconds, or until "POWER ON" is displayed.
        - Step 8. AT+EBLESA_DEMO=0\0d\0a.
        - Step 9. AT+EBTES_DEMO=3\0d\0a.
        - Step 10.Smartphone (powered by MediaTek chipset) connects to a device.
        - Step 11.AT+EBTES_DEMO=0\0d\0a.
        - Step 12.AT+EBTSI_DEMO=2048,2048,4,1\0d\0a.
        - Step 13.AT+EPMICREG=1,36,a,ff\0d\0a.
        - Step 14.AT+EPMICREG=1,36,b,ff\0d\0a.
        - Step 15.AT+SM_DEMO=6,1\0d\0a.
    - Case 5.
      - Description: Bluetooth Voice MSBC(16kHZ)
        - Step 1. Power on the device.
        - Step 2. AT+SM_DEMO=8,3\0d\0a.
        - Step 3. AT+SM_DEMO=8,4\0d\0a.
        - Step 4. AT+SM_DEMO=6,2\0d\0a.
        - Step 5. Wait for about 2 seconds.
        - Step 6. AT+EBTTEST_DEMO=1\0d\0a.
        - Step 7. Wait for at least 5 seconds, or until "POWER ON" is displayed.
        - Step 8. AT+EBLESA_DEMO=0\0d\0a.
        - Step 9. AT+EBTES_DEMO=3\0d\0a.
        - Step 10.Smartphone(powered by MediaTek chipset) connects to a device.
        - Step 11.Pair device (device name: BT_Head_XXX).
        - Step 12.AT+EBTES_DEMO=0\0d\0a.
        - Step 13.AT+EPMICREG=1,36,a,ff\0d\0a.
        - Step 14.AT+EPMICREG=1,36,b,ff\0d\0a.
        - Step 15.Make a call to a smartphone.
    - Case 6.
      - Description: Bluetooth A2DP(AAC)
        - Step 1. Power on the device.
        - Step 2. AT+SM_DEMO=8,3\0d\0a.
        - Step 3. AT+SM_DEMO=8,4\0d\0a.
        - Step 4. AT+SM_DEMO=6,2\0d\0a.
        - Step 5. Wait for about 2 seconds.
        - Step 6. AT+EBTTEST_DEMO=1\0d\0a.
        - Step 7. Wait for at least 5 seconds, or until "POWER ON" is displayed.
        - Step 8. AT+EBLESA_DEMO=0\0d\0a.
        - Step 9. AT+EBTES_DEMO=3\0d\0a.
        - Step 10.Pairing the smartphone with the device (device name:
          BT_Head_XXX)<XXX is bt address>.
        - Step 11.Pairing the smartphone with the device (device name: BT_Head_XXX).
        - Step 12.AT+EBTES_DEMO=0\0d\0a.
        - Step 13.AT+EPMICREG=1,36,a,ff\0d\0a.
        - Step 14.AT+EPMICREG=1,36,b,ff\0d\0a.
        - Step 15.Playback from host device.
    - Case 7.
      - Description: Bluetooth A2DP (SBC)
        - Step 1.Power on the device.
        - Step 2.AT+SM_DEMO=8,3\0d\0a.
        - Step 3.AT+SM_DEMO=8,4\0d\0a.
        - Step 3.1. AT+EBTA2DP_DEMO=0\0d\0a.
        - Step 4. AT+SM_DEMO=6,2\0d\0a.
        - Step 5. Wait for about 2 seconds.
        - Step 6. AT+EBTTEST_DEMO=1\0d\0a.
        - Step 7. Wait for at least 5 seconds, or until "POWER ON" is displayed.
        - Step 8. AT+EBLESA_DEMO=0\0d\0a.
        - Step 9. AT+EBTES_DEMO=3\0d\0a.
        - Step 10. Smartphone pair and connects to pair device (device name:
          BT_Head_XXX).
        - Step 11. Pair the device (device name: BT_Head_XXX).
        - Step 12. AT+EBTES_DEMO=0\0d\0a.
        - Step 13. AT+EPMICREG=1,36,a,ff\0d\0a
        - Step 14. AT+EPMICREG=1,36,b,ff\0d\0a
        - Step 15. Playback from the host device.
    - Case 8.
      - Description:
        - AWS
        - [Device_A as SLAVE speaker2, not smartphone].
          - Step 1. Power on the device A.
          - Step 2. AT+SM_DEMO=8,3\0d\0a.
          - Step 3. AT+SM_DEMO=8,4\0d\0a.
          - Step 4. AT+SM_DEMO=6,2\0d\0a.
          - Step 5. Wait for about 2 seconds. 
          - Step 6. AT+EBTTEST_DEMO=1\0d\0a.
          - Step 7. Wait for at least 5 seconds, or until "POWER ON" is displayed.
          - Step 8. AT+EBLESA_DEMO=0\0d\0a.
          - Step 9. AT+BTSINK=FUNC,LONGPRESSUP\0d\0a.
          - Step 10. Wait for speaker1 to connect.
        - [Device_B as MASTER speaker1, not smartphone].
          - Step 11. Power on the device B.
          - Step 12. AT+SM_DEMO=8,3\0d\0a.
          - Step 13. AT+SM_DEMO=8,4\0d\0a.
          - Step 14. AT+SM_DEMO=6,2\0d\0a.
          - Step 15. Wait for about 2 seconds. 
          - Step 16. AT+EBTTEST_DEMO=1\0d\0a.
          - Step 17. Wait for at least 5 seconds, or until "POWER ON" is displayed.
          - Step 18. AT+EBLESA_DEMO=0\0d\0a.
          - Step 19. AT+BTSINK=PREV,PRESSUP\0d\0a for inquiry and connect to
            speaker2.
          - Step 20. Smartphone queries and connects to speaker1 within 90 seconds after speaker1 and
          speaker2 are connected.
        - [Switch to Device_A]
          - Step 21. Input AT command "AT+EPMICREG=1,36,a,ff\0d\0a" to close the
            gauge leakage.
          - Step 22. Input AT command "AT+EPMICREG=1,36,b,ff\0d\0a" to close the
            gauge leakage.
          - Step 23. AT+EBTES_DEMO=0\0d\0a. [Switch to Device_B]
          - Step 24. AT+EBTES_DEMO=0\0d\0a.
          - Step 25. Input AT command "AT+EPMICREG=1,36,a,ff\0d\0a" to close the
            gauge leakage.
          - Step 26. Input AT command "AT+EPMICREG=1,36,b,ff\0d\0a" to close the
            gauge leakage.
          - Step 27. Play music on a smartphone.


*/
/**
 * @}
 * @}
 * @}
 * @}
 */
*/
