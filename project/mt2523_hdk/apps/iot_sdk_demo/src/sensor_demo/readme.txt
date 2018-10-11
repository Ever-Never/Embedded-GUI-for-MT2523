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
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo_sensor_demo sensor_demo
 * @{

@par Overview
  - This project is a reference application on how to obtain heart rate (HR)
    and blood pressre (BP) data from MediaTek MT2511 biosensor daughterboard.
    The HR and BP data are displayed on the UI.
  - Enable the sensor
    - Call the function #hal_pinmux_set_function() or use the EPT tool to set
      the GPIO pinmux to configure the I2C digital interface and external
      interrupt pinmux settings. Please refer to bsp_sensor_peripheral_init()
      in [sdk_root]/driver/board/mt25x3_hdk/mems_init/mems_init.c.
    - Call the fucntion #sensor_subscribe_sensor() to get the HR and BP data.
    - Sensor subsystem task (sensor_mgr_task() in
      [sdk_root]/middleware/MTK/sensor_subsys/src/sensor_manager.c) feeds
      accelerometer and biosensor data into fusion algorithm to get heart
      rate and blood pressure measurements.
  - Results
    - UI displays the HR and BP data. Please note that HR data update is
      paused while BP is being measured.

@par Hardware and software environment
  - Supported HDK
    - LinkIt 2523 HDK.
    - MediaTek MT2511 biosensor daughterboard (model number: MTK0067).
    - Optical module sub-board (model number: WS3524).
    - An electrocardiography (EKG) cable.
  - Environment configuration
    - Connect MediaTek MT2511 biosensor daughterboard to the LinkIt 2523 HDK
      using the Sensor interface on the development board.
    - Connect an EKG cable to the daughterboard.
    - Connect an optical module sub-board to the daughterboard. The optical
      module sub-board uses the light-based technology to sense the rate of
      blood flow controlled by the heart's pumping action.
  - Customization Option
    - \b SENSOR_DEMO determines whether the sensor features such as heart
      rate monitor and blood pressure monitor are contained in this project.
    - \b MTK_SENSOR_BIO_USE_MT2511 selects and enables the driver for
      MediaTek MT2511 biosensor. Please make sure MediaTek MT2511 biosensor
      daughterboard is connected to the LinkIt 2523 HDK.
    - \b FUSION_HEART_RATE_MONITOR_USE enables MediaTek heart rate monitor
      fusion algorithm library under
      middleware/MTK/sensor_subsys/lib/fusion_algo folder.
    - \b FUSION_HEART_RATE_VARIABILITY_USE enables MediaTek heart rate
      variability monitor fusion algorithm library under
      middleware/MTK/sensor_subsys/lib/fusion_algo folder. Please note that
      this definition and FUSION_BLOOD_PRESSURE_USE should be used
      exclusively.
    - \b FUSION_BLOOD_PRESSURE_USE enables MediaTek blood pressure monitor
      fusion algorithm library under
      middleware/MTK/sensor_subsys/lib/fusion_algo folder. Please note that
      this definition and FUSION_HEART_RATE_VARIABILITY_USE should be used
      exclusively.
    - \b FUSION_PEDOMETER_USE is for pedometer and not used in this project.
    - \b FUSION_SLEEP_TRACKER_USE is for sleep tracker and not used in this
      project.

@par Directory contents
  - Source and header files
    - \b src/sensor_demo/sensor_demo.c:
      The source file of the sensor initialization functions.
    - \b src/sensor_demo/sensor_demo_screen.c:
      The display configuration file of the sensor demo.
    - \b inc/sensor_demo/sensor_demo.h:
      The header file of the sensor initialization functions.
    - \b \<sdk_root\>/driver/board/component/biosensor/mt2511:
      The driver of MT2511 (MediaTek biosensing analog front-end chip).
    - \b \<sdk_root\>/driver/board/component/mems:
      The motion sensor driver (including BMA255 driver).
    - \b \<sdk_root\>/middleware/MTK/sensor_subsys:
      The sensor manager implementation to control drivers, and report data to
      application.

@par Run the application
  - Press "Sensor Subsystem" on the main display.
  - The EKG cable has three leads with electrode patch. Stick the green and
    black electrode patches to the left arm, stick the red electrode patch to
    the right arm.
  - Place your hand on the optical module sub-board.
  - Observe the HR and BP data change on the display.
    - Sensor subsystem display will show the HR data changes.
    - Press "BP" on the sensor subsystem display.
      - Sensor subsystem display will show the BP data.
      - Wait a few seconds for the BP data measurement. Please note that HR
        data update is paused.
      - Sensor subsystem display will show the BP measurements.
*/
/**
 * @}
 * @}
 * @}
 * @}
 */
