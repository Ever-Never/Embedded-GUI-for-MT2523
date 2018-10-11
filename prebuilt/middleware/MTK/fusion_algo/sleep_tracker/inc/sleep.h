/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
 
#ifndef __SLEEP_H__
#define __SLEEP_H__

#ifdef _cpluscplus
extern "C"{
#endif

/**
 * @addtogroup API
 * @{
 * This document include APIs for sleep tracker and staging.
 * @addtogroup Sleep
 * @{
 * This document mainly introduce details about the sleep APIs including terms and acronym,
 * supported features, software achitechture, how to use this APIs, function groups, all enum, structure and functions.
 *
 *
 * @section Terms_Chapter Terms and acronyms
 *
 * |Terms              |Details                                                                 |
 * |-------------------|------------------------------------------------------------------------|
 * |\b ST              | Sleep Tracker                                                          |
 * |\b SS              | Sleep Staging                                                          |
 * |\b SE              | Sleep Efficiency (%) is the rate of time spent asleep (total sleep time) to the amount of time spent on bed. |
 * |\b HR              | Heart Rate                                                             |
 * |\b PPG             | Photoplethysmography                                                   |
 * |\b BISI            | Beat Interval and Signal Integrity. This is a proprietary signal in MediaTek Clover. |
 * |\b REM             | Rapid Eye Movement. One of phase of sleep in human.                    |
 *
 * @section Features_Chapter Supported features 
 * - Users can determine to run ST mode or SS mode by different sensor configuration of platform. 
 * - ST mode can detect sleep by continuously feeding in 3-axis accelerometers data. \n
 * - SE (%) and sleep states with corresponding timestamp (#SS_AWAKE, #SS_REM, #SS_LIGHT, and #SS_DEEP) are reported while a complete sleep duration is detected. \n
 * - SS mode can elaborate advanced sleep states by feeding in continuous 3-axis accelerometers data, PPG, and BISI. HR features and intermidiate feature will be generated for final sleep state decision. \n
 * - For SS, sleep features with corresponding timestamp should be stored and uploaded to MediaTek Cloud for final sleep states (ex: #SS_AWAKE, #SS_REM, #SS_LIGHT, and #SS_DEEP ...etc.) \n
 *
 * @section Usage_Chapter How to use this API 
 *
 * - Tracker mode \n
 *  - step1: Call #motion_st_init & #sleep_fet_init to intialize sleep tracking algorithm.
 *  - step2: Call #motion_st to process motion data.
 *  - step3: Call #check_sleep_results to check if any sleep event is vailable. The return number means total available events.
 *  - step4: Allocate appropriate buffer to #get_one_sleep_result. Buffer size can be determined by #check_sleep_results.
 *           In the sample code, 100 elements are preallocated for sleep events and corresponding timestamps.
 *  - step5: Call #rearrange_queue_for_sleep to manage internal buffer in sleep tracker APIs. It must be called after full events are accessed. 
 *  - step6: Call #check_sleep_motion_fet to check if any motion feature is available. The return number means total available motion features. 
 *  - step7: If motion features are available, then put motion feature into #sleep_fet_run for sleep feature caculation. Call #rearrange_queue_for_motion_fet after getting all motion features.
 *  - step8: Use #check_fet_ready to see if any sleep feature is ready to store or upload. #send_fet_packet is designed to get sleep features.
 *  - sample code:
 *    @code
 *       int ret;
 *       int result_count  = 0;
 *       int fet_count     = 0;
 *       int advfet_count  = 0;
 *       int *feature;
 *       unsigned int *feature_time;
 *       int *fet_num; 
 *       sensor_data_t output;
 *       sleep_fet_sp_t   tmp_sleep_fet_sp;
 *       int          sleep_evt[100];
 *       unsigned int sleep_evt_time[100]; 
 *       int se;
 *       unsigned int onbed_time; 
 *       ret = ret | motion_st_init(); 
 *       ret = ret | sleep_fet_init();
 *       if (ret == STATUS_OK) {
 *         // infinite-loop to set data   
 *         while (input != NULL) {
 *           motion_st( input->data->accelerometer_t.x, input->data->accelerometer_t.y, input->data->accelerometer_t.z, input->data->accelerometer_t.time_stamp); 
 *           result_count = check_sleep_results();
 *           if(result_count > 0) { // loop to get sleep events
 *             for (j=0;j<result_count;j++) ret = get_one_sleep_result(&sleep_evt[j], &sleep_evt_time[j]);
 *             if(ret == C_STATE) { 
 *               se         = get_se();
 *               onbed_time = get_bedtime();
 *             }
 *             rearrange_queue_for_sleep();
 *           } 
 *           fet_count    = check_sleep_motion_fet(); 
 *           if(fet_count>0) {   
 *             for (j=0;j<fet_count;j++) {
 *               ret = get_one_motion_fet(&tmp_sleep_fet_sp.value, &tmp_sleep_fet_sp.timestamp);
 *               sleep_fet_run(tmp_sleep_fet_sp, pid, algo_state); // process the advanced features extractions
 *             }
 *             rearrange_queue_for_motion_fet(); 
 *             advfet_count = check_fet_ready(); // check the features for classifier 
 *             if(advfet_count > 0) {
 *                send_fet_packet(feature, feature_time, fet_num, FEATURE_MARK2);
 *                pdata++;
 *             }
 *           }
 *         }
 *       }
 *    @endcode 
 *
 * - Staging mode \n
 *  - step1: Call #motion_st_init, #hr_fet_init, and #sleep_fet_init to intialize sleep staging algorithm. 
 *  - step2: Call #motion_st to process motion data. #hr_fet_ext is designed to process PPG and BISI data for HR features.
 *  - step3: Call #check_sleep_results to check if any sleep event is available. The return number means total available events.
 *  - step4: Allocate appropriate buffer to #get_one_sleep_result. Buffer size can be determined by #check_sleep_results.
 *           In the sample code, 100 elements are preallocated for sleep events and corresponding timestamps.
 *  - step5: Call #rearrange_queue_for_sleep to manage internal buffer. It must be called after full events are accessed. 
 *  - step6: Call #check_sleep_motion_fet to check if any motion feature is available. The return number means total available motion features. 
 *  - step7: If motion features are available, then put them into #sleep_fet_run for sleep feature caculation. Call #rearrange_queue_for_motion_fet after getting all motion features.
 *  - step8: Call #check_sleep_hr_fet to check if any HR feature is available. The return number means total available HR features. 
 *  - step9: If HR features are available, then put them into #sleep_fet_run for sleep feature caculation.  
 *  - step10: Use #check_fet_ready to see if any sleep feature is ready to store or upload to MediaTek cloud. #send_fet_packet is designed to get sleep features.
 
 *  - sample code:
 *    @code
 *       int ret;
 *       int result_count  = 0;
 *       int fet_count     = 0;
 *       int advfet_count  = 0;
 *       int *feature1, *feature2;
 *       unsigned int *feature1_time, *feature2_time;
 *       int *fet1_num, *fet2_num;; 
 *       sensor_data_t output;
 *       sleep_fet_sp_t   tmp_sleep_fet_sp; 
 *       int          sleep_evt[100];
 *       unsigned int sleep_evt_time[100];  
 *       int se;
 *       unsigned int onbed_time; 
 *       ret = ret | motion_st_init(); 
 *       ret = ret | sleep_fet_init();
 *       if (ret == STATUS_OK) {
 *         // infinite-loop to set data   
 *         while (input != NULL) {
 *           motion_st(input->data->accelerometer_t.x, input->data->accelerometer_t.y, input->data->accelerometer_t.z, input->data->accelerometer_t.time_stamp); 
 *           hr_fet_ext(input->data->bisi, input->data->bisi.time_stamp, BISI_TYPE, 64);
 *           hr_fet_ext(input->data->ppg, input->data->ppg.time_stamp, PPG_TYPE, 64);
 *           result_count = check_sleep_results();
 *           if(result_count > 0) { // loop to get sleep events
 *             for (j=0;j<result_count;j++) ret = get_one_sleep_result(&sleep_evt[j], &sleep_evt_time[j]);            *             
 *             if(ret == C_STATE) { 
 *               se         = get_se();
 *               onbed_time = get_bedtime();
 *             }
 *             rearrange_queue_for_sleep();
 *           } 
 *           fet_count    = check_sleep_motion_fet(); 
 *           if(fet_count>0) {   
 *             for (j=0;j<fet_count;j++) {
 *               ret = get_one_motion_fet(&tmp_sleep_fet_sp.value, &tmp_sleep_fet_sp.timestamp);          
 *               sleep_fet_run(tmp_sleep_fet_sp, pid, algo_state); // process the advanced features extractions
 *             }
 *             rearrange_queue_for_motion_fet(); 
 *           }
 *          fet_count = check_sleep_hr_fet();   // get heart rate features
 *           if(fet_count>0) {   
 *             for (j=0;j<fet_count;j++) {
 *               ret = get_one_hr_fet(&tmp_sleep_fet_sp.value, &tmp_sleep_fet_sp.timestamp);          
 *               sleep_fet_run(tmp_sleep_fet_sp, pid, algo_state); // process the advanced features extractions
 *             }
 *             advfet_count = check_fet_ready(); // check the features for classifier 
 *             if(advfet_count > 0) {
 *                send_fet_packet(feature1, feature1_time, fet1_num, FEATURE_MARK1);       
 *                send_fet_packet(feature2, feature2_time, fet2_num, FEATURE_MARK2);       
 *             }
 *           }
 *         }
 *       }    
 *    @endcode 
 *
 * @section Function_Group_Chapter ST function groups description 
 *
 * For easily using, we group the APIs by its functionality.\n
 * You can refer to the details of each function by clicking the function name.\n
 * - ST initialization \n 
 *   #motion_st_init and #sleep_fet_init functions initialize parameters for ST. Before running ST mode, you should call these functions firstly. \n
 *
 * - SS initialization \n 
 *   #motion_st_init, #hr_fet_init, and #sleep_fet_init initialize parameters for SS. Before running SS mode, you should call these functions firstly.\n
 *
 * - Run ST \n 
 *   #motion_st function will run algorithm by assigning a set of motion value and corresponding timestamp;\n
 *
 * - Check results \n 
 *   #check_sleep_results can return the available number of data which can be accessed. \n
 *   Users should get data immediately OR the results may be overwritten.\n
 * 
 * - Get results \n 
 *   #get_one_sleep_result can get one sleep states and its corresponding timestamp. System software should allocate apporiate results buffer and results_timestamp buffer before calling #get_one_sleep_result. #check_sleep_results can return the available number of states which should be accessed. \n
 *   #get_se and #get_bedtime returns SE and bedtime while a sleep period is complete. \n *   
 *   #rearrange_queue_for_sleep must call after #get_one_sleep_result. \n
 *
 * - Check basic features \n 
 *   #check_sleep_motion_fet can return the available number of motion features to be accessed. \n
 *   #check_sleep_hr_fet can return the available number of HR features to be accessed. \n
 *
 * - Get basic features \n 
 *   Call #get_one_motion_fet for ST mode. Call both #get_one_motion_fet and #get_one_hr_fet for SS mode. \n
 *   #check_sleep_motion_fet can return the available number of final sleep features which can be accessed. \n
 *   #check_sleep_hr_fet can return the available number of final sleep features which can be accessed. \n
 *
 * - Process basic features for advanced features \n 
 *   #sleep_fet_run function will process basic features from motion and HR to generate final sleep features; \n
 *
 * - Check advanced features \n 
 *   #check_fet_ready  can return the available number of final sleep features to be accessed. \n
 *   Users should get data immediately OR the results may be overwritten. \n
 *
 * - Get advanced features \n 
 *   Call #send_fet_packet to get advanced features. \n
 */
//---------------------------------------------------------
// definitions of sleep states
//---------------------------------------------------------
/** @brief  This macro defines numeric value for sleep state. \n AWAKE: Active or not sleep
  */
#define SS_AWAKE       0 

/** @brief  This macro defines numeric value for sleep state. \n RESTLESS: Significant movement or turn over during sleep, but not AWAKE
  */
#define SS_RESTLESS    -12

/** @brief  This macro defines numeric value for sleep state.\n ASLEEP: Inactive state
  */
#define SS_ASLEEP      -13

/** @brief  This macro defines numeric value for sleep state. \n REM: Rapid eye movement sleep
  */
#define SS_REM         -1

/** @brief  This macro defines numeric value for sleep state. \n LIGHT: Light sleep
  */
#define SS_LIGHT       -2

/** @brief  This macro defines numeric value for sleep state. \n DEEP: Deep sleep
  */
#define SS_DEEP        -3

/** @brief  This macro defines numeric value for sleep state. \n UNKNOWN: Unknown states
  */
#define SS_UNKNOWN     -7

//---------------------------------------------------------
// definitions of indicating feature type
//---------------------------------------------------------
/** @brief  This macro defines numeric value for packet. \n FEATURE_MARK0: Feature set for staging
  */
#define FEATURE_MARK0   20

/** @brief  This macro defines numeric value for sleep state. \n FEATURE_MARK1: Feature set for staging
  */
#define FEATURE_MARK1   21

/** @brief  This macro defines numeric value for sleep state. \n FEATURE_MARK2: Feature set for tracker
  */
#define FEATURE_MARK2   22

/** @brief  This macro defines numeric value for sleep state. \n STATE_MARK: Sleep states
  */
#define STATE_MARK      30

//---------------------------------------------------------
// definitions of algorithm modes
//---------------------------------------------------------

/** @brief  This macro defines numeric value for sleep state. \n INVOKE_STAGING: The system is requested to invoke staging mode. Motion, PPG, and BISI are relevant.
  */
#define INVOKE_STAGING  31

/** @brief  This macro defines numeric value for sleep state. \n INVOKE_TRACKER: The system is requested to invoke tracker mode. Motion sensor are relevant.
  */
#define INVOKE_TRACKER  32

//---------------------------------------------------------
// signal type definition
//---------------------------------------------------------
/** @brief  This macro defines numeric value for signal type. \n PPG_TYPE: PPG
  */
#define PPG_TYPE                       0
/** @brief  This macro defines numeric value for signal type. \n BISI_TYPE: BISI
  */
#define BISI_TYPE                      1
//---------------------------------------------------------
// function status definition
//---------------------------------------------------------
/** @brief  This macro defines numeric value for error indication. STATUS_OK: Run normally during function call
  */
#define STATUS_OK               0
/** @brief  This macro defines numeric value for error indication. SD_STATUS_ERROR: Error happen during function call
  */
#define STATUS_ERROR           -1
//---------------------------------------------------------
// state for frame control
//---------------------------------------------------------
/** @brief  This macro defines numeric value for frame control. IC_STATE: Incomplete sleep duration but the internal buffer in library is almost full. Request the system to get sleep states to prevent buffer overflow.
  */
#define IC_STATE                  -99 
/** @brief  This macro defines numeric value for frame control. C_STATE: Complete sleep duration
  */
#define C_STATE                    0

//---------------------------------------------------------
//  structure for sleep staging                   
//---------------------------------------------------------
/** @brief This structure defines the data input structure for sleep feature extraction.
*/
typedef struct {
  int           value;      /**< intermediate output by motion/heart rate feature extraction functions */
  unsigned int  timestamp;  /**< the corresponding timestamp of each intermediate output */
} sleep_fet_sp_t;

/**
 * @brief Initialize parameters for sleep staging.
 * @return  Return #STATUS_OK for success and #STATUS_ERROR for failure indication.
 */
int  sleep_fet_init(void);

/**
* @brief This function is main function to extract advanced sleep features. Call #sleep_fet_run with one motion or HR feature.
* @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication. 
* @param[in] in is the preliminary features for sleep feature extraction.
* @param[in] pid is the system process ID for sleep feature extraction. This parameter is designed for scheduling. Users can increment a number as pid while you call this function.
* @param[in] algo_state indicate the current algorithm state (#INVOKE_STAGING or #INVOKE_TRACKER) for internal process. If in ST mode, then input #INVOKE_TRACKER; else in SS mode, input #INVOKE_STAGING.
*/  
int  sleep_fet_run(sleep_fet_sp_t in, int pid, int algo_state);

/**
* @brief This function is used to check if any available sleep feature can be stored. \n
      Call #check_fet_ready once #sleep_fet_run is processed. \n
* @return    Return number is the available advanced sleep features. Return 0 if no available data. 
*/    
int  check_fet_ready(void);

/**
* @brief This function is used to get sleep features. \n
        Call #send_fet_packet if any avaliable feature is ready . \n
* @return    Return number is the available features. Return 0 if no available data. 
* @param[in] fet is the pointer for features
* @param[in] fet_time is the pointer for corresponding timestamp of feature
* @param[in] fet_num indicate how many features in current packet
* @param[in] fet_type indicate the current feature type (#FEATURE_MARK0/#FEATURE_MARK1/#FEATURE_MARK2). If in ST mode, then input #FEATURE_MARK2; else in SS mode, input #FEATURE_MARK0 and #FEATURE_MARK1 for full features.
*/  
//void send_fet_packet(int32_t *fet, unsigned int *fet_time, int *fet_num, int fet_type);
void send_fet_packet(int32_t *src_fet, uint32_t *src_fet_time, int32_t *fet_num, int fet_type);
//---------------------------------------------------------
// functions for sleep tracker
//---------------------------------------------------------
/**
 * @brief Initialize parameters for sleep tracker.
 * @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication.
 * Please refer to #motion_st for sample code.
 */
int  motion_st_init(void);

/**
 * @brief This function is main function to detect sleep. Call #motion_st with one set of motion sample.
 * @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication. 
 * @param[in] ax is the x-axis value from accelerometer (unit: 0.001 m/s^2).
 * @param[in] ay is the y-axis value from accelerometer (unit: 0.001 m/s^2).
 * @param[in] az is the z-axis value from accelerometer (unit: 0.001 m/s^2).
 * @param[in] timestamp is the corresponding time (unit: milli-second) to accelerometer.
 *  - sample code:
 *    @code
 *       int ret;
 *       int result_count = 0;
 *       int          sleep_evt[100];
 *       unsigned int sleep_evt_time[100]; 
 *       int se;
 *       unsigned int onbed_time; 
 *       ret = motion_st_init();//initialize ST hardware, which power on ST.
 *       if (ret == STATUS_OK) {
 *         // infinite-loop to set data   
 *         while (input != NULL) {
 *           motion_st( input->data->accelerometer_t.x,
 *                      input->data->accelerometer_t.y,
 *                      input->data->accelerometer_t.z, 
 *                      input->data->time_stamp); 
 *           result_count = check_sleep_results();
 *           if(result_count > 0) {
 *             // loop to get sleep events
 *             for (j=0;j<result_count;j++) {            
 *               ret = get_one_sleep_result(&sleep_evt[j], &sleep_evt_time[j]);           
 *             }
 *             if(ret == C_STATE) {
 *               se         = get_se();
 *               onbed_time = get_bedtime();
 *             }
 *             rearrange_queue_for_sleep();
 *           }
 *         }
 *       }        
 *    @endcode
 */  
int  motion_st(int ax, int ay, int az, unsigned int timestamp);

/**
 * @brief This function is used to check if any available sleep state can be accessed. \n
          Call #check_sleep_results once system get one set of motion data. \n
 * @return    Return number is the available data. Return 0 if no available data. 
 * Please refer to #motion_st for sample code.
 */    
int  check_sleep_results();

/**
 * @brief This function is used to get one sleep evnet and its corresponding timestamp if available\n
 * @param[in] results is the pointer for event label.
 *            Please call #check_sleep_results to allocate appropriate buffer size before calling #get_one_sleep_result.
 *            Loop N times to get full results and N can be obtained by calling #check_sleep_results. 
 * @param[in] results_timestamp is the pointer for event timestamp.
 * @return    Return #C_STATE as complete sleep duration and #IC_STATE for incomplete sleep duration. 
 *            If #C_STATE is returned, calling #get_se and #get_bedtime to get sleep efficiency (%) and bedtime timestamp, respectively.
 *            If #IC_STATE is returned, it typically means internal buffers are full but the sleep duration is incomplete.
 calling #get_se and #get_bedtime do not provide meaningful infomation. This typically means buffers are out of 
 * Sample code please refer to #motion_st
 */  
int   get_one_sleep_result(int *results, unsigned int *results_timestamp);
/**
 * @brief This function is used to get sleep efficiency(%). \n
 * @return    Return sleep efficiency(%) if the sleep duration is complete.
 * Please refer to #motion_st for sample code.
 */  
int   get_se(void);
/**
 * @brief This function is used to get bedtime timestamp. \n
 * @return    Return bedtime timestamp if the sleep duration is complete.
 * Sample code please refer to #motion_st
 */  
unsigned int get_bedtime(void);
/**
 * @brief This function is used to manage internal buffer in sleep tracker APIs. It must be called after full events are accessed.\n
 * @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication.
 * Please refer to #motion_st for sample code.
 */  
int  rearrange_queue_for_sleep(void);
//---------------------------------------------------------
// functions for sleep tracker
//---------------------------------------------------------
/**
 * @brief This function is used to check if available features can be accessed. \n
 * @return    Return number is the available data. Return 0 if no available data. 
 *  - sample code:
 *    @code
 *       int ret;
 *       int fet_count = 0;
 *       int          motion_fet[100];
 *       unsigned int motion_fet_time[100]; 
 *       ret = motion_st_init();//initialize ST hardware, which power on ST.
 *       if (ret == STATUS_OK) {
 *         // infinite-loop to set data   
 *         while (input != NULL) {
 *           motion_st( input->data->accelerometer_t.x,
 *                      input->data->accelerometer_t.y,
 *                      input->data->accelerometer_t.z, 
 *                      input->data->time_stamp); 
 *           fet_count = check_sleep_motion_fet();
 *           if(fet_count > 0) {
 *             // loop to get motion features
 *             for (j=0;j<fet_count;j++) {            
 *               ret = get_one_motion_fet(&motion_fet[j], &motion_fet_time[j]);           
 *             }
 *             rearrange_queue_for_motion_fet();
 *           }
 *         }
 *       }        
 *    @endcode
 */    
int  check_sleep_motion_fet(void);

/**
 * @brief This function is used to get one set of motion features and its corresponding timestamp if available\n
 * @param[in] results is the pointer for features.
 *            Please call #check_sleep_motion_fet to allocate appropriate buffer size before calling #get_one_motion_fet.
 *            Loop N times to get full results and N can be obtained by calling #check_sleep_motion_fet. 
 * @param[in] results_timestamp is the pointer for features timestamp.
 * @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication.
 * Sample code please refer to #check_sleep_motion_fet
 */  
int  get_one_motion_fet(int *results, unsigned int *results_timestamp);

/**
 * @brief This function is used to manage internal buffer in sleep tracker APIs. It must be called after full features are accessed.\n
 * @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication.
 * Please refer to #check_sleep_motion_fet for sample code.
 */  
int  rearrange_queue_for_motion_fet(void);

//---------------------------------------------------------
// functions for heart rate features extraction
//---------------------------------------------------------
/**
 * @brief Initialize parameters for heart rate feature extraction.
 * @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication.
 * Please refer to #hr_fet_ext for sample code.
 */
int  hr_fet_init(void);
/**
 * @brief This function is main function to extract heart rate features for sleep staging. Call #hr_fet_ext with one set of PPG/BISI sample\n
 * @param[in] ppg_bisi is the PPG/BISI data inputs.
 * @param[in] timestamp is the corresponding timestamp to PPG/BISI data.
 * @param[in] sensor_type is the type definition for event timestamp.
 * @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication.
 *  - sample code:
 *    @code
 *       int ret;
 *       int fet_count = 0;
 *       int          hr_fet[100];
 *       unsigned int hr_fet_time[100]; 
 *       ret = hr_fet_init();
 *       if (ret == STATUS_OK) {
 *         while (input != NULL) {
 *           if(input_list->sensor_type == PPG_TYPE) {
 *             hr_fet_ext(ppg_bisi_data, ppg_bisi_time, PPG_TYPE, 64);
 *           }
 *           else if (input_list->sensor_type == BISI_TYPE) {
 *             hr_fet_ext(ppg_bisi_data, ppg_bisi_time, BISI_TYPE, 64);
 *           }
 *           fet_count = check_sleep_hr_fet();             *
 *           if(fet_count > 0) {
 *             // loop to get heart rate features
 *             for (j=0;j<fet_count;j++) {            
 *               ret = get_one_hr_fet(&hr_fet[j], &hr_fet_time[j]);           
 *             }
 *             rearrange_queue_for_hr_fet();
 *           }
 *         }
 *       }        
 *    @endcode
 */  
int  hr_fet_ext(int ppg_bisi, int timestamp, uint8_t sensor_type, uint8_t fs);

/**
 * @brief This function is used to check if available features can be accessed. \n
 * @return    Return number is the available data. Return 0 if no available data. 
 * Sample code please refer to #hr_fet_ext
 */    
int  check_sleep_hr_fet(void);

/**
 * @brief This function is used to get one set of motion features and its corresponding timestamp if available\n
 * @param[in] results is the pointer for event label.
 *            Please call #check_sleep_hr_fet to allocate appropriate buffer size before calling #get_one_hr_fet.
 *            Loop N times to get full results and N can be obtained by calling #check_sleep_hr_fet. 
 * @param[in] results_timestamp is the pointer for event timestamp.
 * @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication.
 * Sample code please refer to #hr_fet_ext
 */  
int  get_one_hr_fet(int *results, unsigned int *results_timestamp);


/**
 * @brief This function is used to manage internal buffer in HR features extraction APIs. It must be called after full events are accessed.\n
 * @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication.
 * Please refer to #hr_fet_ext for sample code.
 */  
int  rearrange_queue_for_hr_fet(void);

/**
 * @brief This function is main function to extract respiration features for sleep staging. Call #resp_fet_ext with one set of PPG sample\n
 * @param[in] ppg is the PPG data inputs.
 * @param[in] timestamp is the corresponding timestamp to PPG data.
 * @return    Return #STATUS_OK for success and #STATUS_ERROR for failure indication.
 * Please refer to #hr_fet_ext for sample code.
 */  
int resp_fet_ext(int ppg, int timestamp);


#ifdef _cpluscplus
}
#endif

#endif /* __SLEEP_H__ */
