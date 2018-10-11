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
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo_ble_pxp ble_pxp
 * @{

@par Overview
  - Application description
    - This example project demonstrates the basic functions of Proximity
      profile (PXP).
  - Application features
    - The screen will display "Unconnected" when the HDK disconnects from the
      peer device.
    - The screen will display "Connected" when the HDK connects with the peer
      device.
    - The screen will display "Out of range" when the HDK alert level becomes
      value "1" or "2".

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK. For detailed configuration, please refer to readme.txt
      under mt2523_hdk/apps/iot_sdk_demo/.
  - Environment configuration
    - Need a serial tool (such as Tera terminal) for UART logging.
    - COM port settings: baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none and flow control: off.
  - Peer test device
    - Download and install an app, such as "LightBlue", to connect to devices
      that support Bluetooth 4.0 Low Energy, on iPhone 6S Plus or later
      version device.

@par Directory contents
  - Source and header files
    - \b src/pxp_screen/ble_gatt_common.c:
      GATT server common function handle file.
    - \b src/pxp_screen/ble_gatt_connect.c:
      Bluetooth LE connection file.
    - \b src/pxp_screen/ble_gatt_service.c:
      GATT and GAP service file.
    - \b src/pxp_screen/ble_gatt_srv.c:
      GATT extern API file.
    - \b src/pxp_screen/ble_gatt_utils.c:
      GATT utilities file.
    - \b src/pxp_screen/ble_lls.c:
      Link loss service file.
    - \b src/pxp_screen/ble_pxp_main.c:
      Main program for PXP.
    - \b src/pxp_screen/ble_pxp_app.c:
      UI program file for PXP.
    - \b inc/pxp_screen/ble_gatt_common.h:
      GATT server common function handle file header file.
    - \b inc/pxp_screen/ble_gatt_connect.h:
      Bluetooth LE connection header file.
    - \b inc/pxp_screen/ble_gatt_service.h:
      GATT and GAP service header file.
    - \b inc/pxp_screen/ble_gatt_srv.h:
      GATT extern API header file.
    - \b inc/pxp_screen/ble_gatt_srv_internal.h:
      GATT service header file.
    - \b inc/pxp_screen/ble_gatt_utils.h:
      GATT utilities header file.
    - \b inc/pxp_screen/ble_pxp_main.h:
      Main program header file for PXP.
    - \b inc/pxp_screen/ble_pxp_app.h:
      UI program header file for PXP.
    - \b inc/pxp_screen/ble_lls.h:
      GATT service LLS header file.

@par Run the example
  - To run the application.
    - 1. Enter PXP's screen on LinkIt 2523 HDK - tap on "BLE PXP" on the main
      screen, it will display "Unconnected".
    - 2. Run the "LightBlue" app on the iPhone, and establish Bluetooth LE
      connection with the HDK. Find LinkIt 2523 HDK's BLE address or name,
      such as "MediaTek_PXP", tap to connect. The iPhone will enter the
      connection page, and will show all attributes (services,
      characteristics...), meanwhile the HDK will display a message
      "Connected".
    - 3. Send an alert and get a response on LinkIt 2523 HDK. On iPhone
      connection page, tap on "immediate alert" characteristic, then write
      value "1" or "2", to send an intermediate or high level alert to the
      HDK. In this example the LinkIt 2523 HDK will display a message "out of
      range" because the audio is not supported. Write value "0", to stop
      sending alerts to LinkIt 2523 HDK, then LinkIt 2523 HDK will be back to
      "connected" screen.
    - 4. Exit the app. The LinkIt 2523 HDK will disconnect and display a
      message "Unconnected".
*/
/**
 * @}
 * @}
 * @}
 * @}
 */

