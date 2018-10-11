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

#include "sensor_alg_interface.h"
#include "stdio.h"
#include "string.h"
// #include "motion_st.h"
// #include "motion_fet.h"
// #include "hr_fet.h"
// #include "mtk_sleep.h"
#include "algo_adaptor/algo_adaptor.h"
#include "sleep.h"

#if defined(FOTA_UBIN_DECOUPLING)
#include "sleep_tracker_wrapper.h"
#endif

#define LOGE(fmt, args...)    printf("[Sleep] ERR: "fmt, ##args)
#define LOGD(fmt, args...)    printf("[Sleep] DBG: "fmt, ##args)

#define EN_ADAPTOR_SET_MSG            0
#define EN_ADAPTOR_GET_MSG            1
#define TEST_TRACKER_STAGING_SWITCH   0
#define EN_ADAPTOR_GET_SENSOR_MSG     0
#define FORCE_STAGING_ONLY            1

#define WATCH_COLD_INIT_STATE         0
#define WATCH_WARM_INIT_STATE         1

#ifdef SLEEP_RPOFILING
#define NANO_MEASUREMENT
#define MINI_MEASUREMENT
#endif

//#define FASTX8_TEST
//#define FASTX4_TEST
//#define FASTX2_TEST

/* enable this option for pattern test */
//#define ACC_OUT_PATTERN
//#define FATFS_IO

#ifdef ACC_OUT_PATTERN
  #include "mt2511_bisi_in.h"
  #include "mt2511_motion_data_in.h"
  #include "mt2511_motion_time_in.h"
  int mpattern_idx, bipattern_idx, last_motion_timestamp, last_bisi_timestamp;
  int stop_mpattern, stop_bipattern;
  int count_pattern;
#endif


#ifdef HR_PPG_64HZ
  #define PPG_FS 64
#else
  #define PPG_FS 128
#endif

static sensor_input_list_t tracker_input_comp_acc; /* for tracker */
static sensor_input_list_t staging_input_comp_acc; /* for staging */
static sensor_input_list_t staging_input_comp_bio; /* for staging */
static sensor_input_list_t staging_input_comp_bisi; /* for staging */

int      pid;
int      watch_state = WATCH_COLD_INIT_STATE;
int      algo_state  = INVOKE_TRACKER;
int      test_counter;

sleep_fet_sp_t   tmp_sleep_fet_sp;
uint8_t          txcode; 


uint8_t          gtime_latch; 
uint32_t         first_gtime_stamp; /* 1'st g-sensor timestamp */
int32_t tmp_stfeature[9]; // declare as global variables to prevent crash

int32_t get_sleep_data(sensor_data_t * const output)
{
  int ret = IC_STATE;
#ifdef ACC_OUT_PATTERN  
  int i;
#endif
  int j, result_count;

  if(output == NULL || output->data == NULL){
    return -1;
  }  // int tmp_stfeature1[9]; // declare as global variables to prevent crash
  // int tmp_stfeature2[9]; // declare as global variables to prevent crash
  //sensor_sleepmonitor_event_t tmp_evt; // allocate one memory for features;
  
  sensor_data_unit_t * pdata;
  pdata              = output->data; // cast to sleep unit structure
  pdata->sleep_data_t.feature = NULL;
  
  int state;
  unsigned int timestamp;
  
  /*------------------------------*/
  /*01. States Transmission       */
  /*------------------------------*/  
  /* get sleep results first */
  if(txcode == STATE_MARK) {  
    result_count             = check_sleep_results();
    output->data_exist_count = result_count;
    
    for (j=0;j<result_count;j++) {
      ret = get_one_sleep_result(&state, &timestamp);
      pdata->sensor_type = SENSOR_TYPE_SLEEP;
      pdata->sleep_data_t.state = state;
      pdata->time_stamp = timestamp;
      pdata++;
    }
    /* if complete frame, then cache the sleep efficiency */
    if(ret == C_STATE) {
      pdata->sensor_type = SENSOR_TYPE_SLEEP;
      pdata->sleep_data_t.state = get_se();
      pdata->time_stamp         = get_bedtime();
      pdata++;
      output->data_exist_count++;
    }
    if(result_count>0) rearrange_queue_for_sleep(); 
    #if (EN_ADAPTOR_GET_MSG)
      printf("get_sleep_data() - PACKET_STATE\n");
    #endif
    return ret;
  }  
  /*-------------------------------------------*/
  /*02. Features Transmission  (1'st packet)   */
  /*-------------------------------------------*/  
  if(txcode == FEATURE_MARK0) {    
    output->data_exist_count = 1; 
    //output->data = &tmp_evt;
    pdata->sensor_type          = SENSOR_TYPE_SLEEP_STAGING;    
    pdata->sleep_data_t.state   = FEATURE_MARK0;
    pdata->sleep_data_t.feature = (int32_t *)tmp_stfeature; /*default: NULL*/ 
    pdata->sleep_data_t.feature_type = FEATURE_MARK0;
    send_fet_packet(pdata->sleep_data_t.feature, &(pdata->time_stamp), &(pdata->sleep_data_t.feature_num), FEATURE_MARK0);
    // if(output->data[0].sleep_data_t.feature != NULL) for (i=0;i<pdata->sleep_data_t.feature_num;i++) printf("%d,", pdata->sleep_data_t.feature[i]);
    // else                                             printf("sleep_data_t.feature == NULL\n");
    // printf("\n");
#ifdef ACC_OUT_PATTERN
    if(output->data[0].sleep_data_t.feature != NULL) for (i=0;i<pdata->sleep_data_t.feature_num;i++) printf("%d,", pdata->sleep_data_t.feature[i]);
    else                                             printf("sleep_data_t.feature == NULL\n");
    printf("@ %d, %d-", pdata->time_stamp, last_bisi_timestamp);
#endif    

#ifdef FASTX8_TEST
    pdata->time_stamp = (pdata->time_stamp>>3);
#endif    
#ifdef FASTX4_TEST
    pdata->time_stamp = (pdata->time_stamp>>2);
#endif    
#ifdef FASTX2_TEST
    pdata->time_stamp = (pdata->time_stamp>>1);
#endif    


    #if (EN_ADAPTOR_GET_MSG)
      printf("get_sleep_data() - FEATURE_MARK0-%d\n", (int)pdata->time_stamp);
    #endif
    pdata++;
    return ret;  
  }  
  /*-------------------------------------------*/
  /*03. Features Transmission  (2'nd packet)   */
  /*-------------------------------------------*/ 
  if(txcode == FEATURE_MARK1) {
    output->data_exist_count = 1;   
    //output->data = &tmp_evt;
    pdata->sensor_type          = SENSOR_TYPE_SLEEP_STAGING;
    pdata->sleep_data_t.state   = FEATURE_MARK1;
    pdata->sleep_data_t.feature = (int32_t *)tmp_stfeature; /*default: NULL*/  
    pdata->sleep_data_t.feature_type = FEATURE_MARK1;
    send_fet_packet(pdata->sleep_data_t.feature, &(pdata->time_stamp), &(pdata->sleep_data_t.feature_num), FEATURE_MARK1);

    // if(output->data[0].sleep_data_t.feature != NULL) for (i=0;i<pdata->sleep_data_t.feature_num;i++) printf("%d,", pdata->sleep_data_t.feature[i]);
    // else                                             printf("sleep_data_t.feature == NULL\n");    
    // printf("\n");
#ifdef ACC_OUT_PATTERN    
    if(output->data[0].sleep_data_t.feature != NULL) for (i=0;i<pdata->sleep_data_t.feature_num;i++) printf("%d,", pdata->sleep_data_t.feature[i]);
    else                                             printf("sleep_data_t.feature == NULL\n");    
    printf("@ %d, %d-", pdata->time_stamp, last_bisi_timestamp);
#endif    

#ifdef FASTX8_TEST
    pdata->time_stamp = (pdata->time_stamp>>3);
#endif    
#ifdef FASTX4_TEST
    pdata->time_stamp = (pdata->time_stamp>>2);
#endif    
#ifdef FASTX2_TEST
    pdata->time_stamp = (pdata->time_stamp>>1);
#endif   

    #if (EN_ADAPTOR_GET_MSG)
      printf("get_sleep_data() - FEATURE_MARK1-%d\n", (int)pdata->time_stamp);
    #endif
    pdata++;
    return ret;  
  }  
  /*------------------------------------------------------*/
  /*04. Features Transmission  (for tracker-only feature) */
  /*------------------------------------------------------*/ 
  if(txcode == FEATURE_MARK2) {
    output->data_exist_count = 1;   
    //output->data = &tmp_evt;
    pdata->sensor_type = SENSOR_TYPE_SLEEP;
    pdata->sleep_data_t.state   = FEATURE_MARK2;
    pdata->sleep_data_t.feature = (int32_t *)tmp_stfeature; /*default: NULL*/  
    pdata->sleep_data_t.feature_type = FEATURE_MARK2;
    send_fet_packet(pdata->sleep_data_t.feature, &(pdata->time_stamp), &(pdata->sleep_data_t.feature_num), FEATURE_MARK2);        
    #if (EN_ADAPTOR_GET_MSG)
      printf("get_sleep_data() - FEATURE_MARK2-%d\n", (int)pdata->time_stamp);
    #endif
    pdata++;
    return ret;  
  }
  
  /*------------------------------------------------------*/
  /*05. Features Transmission  (for state-transition)     */
  /*------------------------------------------------------*/ 
  if(txcode == INVOKE_TRACKER) { /* software request switching to tracker */
    output->data_exist_count  = 1;   
    pdata->sensor_type        = SENSOR_TYPE_SLEEP;
    pdata->sleep_data_t.state = INVOKE_TRACKER;
    pdata++;
    return ret;  
  }  
  
  if(txcode == INVOKE_STAGING) { /* software request switching to staging */
    output->data_exist_count  = 1;   
    pdata->sensor_type        = SENSOR_TYPE_SLEEP_STAGING;
    pdata->sleep_data_t.state = INVOKE_STAGING;
    pdata++;
    return ret;  
  }  
  
  return -1;  
}


static int32_t set_sleep_data(const sensor_data_t *input_list, void *reserve)
{
    int count                      = input_list->data_exist_count;
    sensor_data_unit_t *data_start = input_list->data;
    int ret, j;
    int result_count; /*sleep efficiency, states*/
    int fet_count; /*sleep-related features*/
    int advfet_count;
    
    advfet_count    = 0;
    
#if defined(FASTX8_TEST)||defined(FASTX4_TEST)||defined(FASTX2_TEST)
    int data0;
    //int data1, data2, data3, data4, data5, data6, data7;
    int time0;
    // int time1, time2, time3, time4, time5, time6, time7; 
#endif    
    /*------------------------------------------------------*/
    /* Phase 1 Input PPG/BISI/Motion and get basic features */
    /*------------------------------------------------------*/    
    if(input_list->data->sensor_type == SENSOR_TYPE_ACCELEROMETER){ /* Motion Thread */      
      #if (EN_ADAPTOR_GET_SENSOR_MSG)
        printf("Got %d motion\n", count);      
      #endif
      
      if(gtime_latch==0) { 
        first_gtime_stamp = (data_start)->time_stamp;
        gtime_latch = 1;
        
      }
      
#ifndef ACC_OUT_PATTERN
      while (count > 0) {
          /* kernel of motion-based sleep tracker */
          
#ifdef FASTX8_TEST
        time0 = ((((data_start)->time_stamp)-first_gtime_stamp)<<3);        
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0);
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*1));
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*2));
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*3));
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*4));
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*5));
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*6));
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*7));
        //printf("g time: %d, %d-%d\n", (data_start)->time_stamp, time0, time0+(16*7));           
#endif
        
#ifdef FASTX4_TEST
        time0 = ((((data_start)->time_stamp)-first_gtime_stamp)<<2);
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0);
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*1));
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*2));
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*3));
        //printf("g time: %d, %d-%d\n", (data_start)->time_stamp, time0, time0+(16*3));
#endif

// #ifdef FASTX2_TEST
        // time0 = ((((data_start)->time_stamp)-first_gtime_stamp)<<1);
        // motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0);
        // motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, time0+(16*1));
        // printf("g time: %d, %d-%d\n", (data_start)->time_stamp, time0, time0+(16*1));
// #endif

#if  !defined(FASTX8_TEST) && !defined(FASTX4_TEST) && !defined(FASTX2_TEST)
        motion_st((data_start)->accelerometer_t.x, (data_start)->accelerometer_t.y, (data_start)->accelerometer_t.z, ((data_start)->time_stamp)-first_gtime_stamp);
#endif
        data_start++;
        count--;
      }
#else
      //count_pattern = count*2;
      count_pattern = count;
      while (count_pattern > 0) {
        if(mpattern_idx<ACC_OUT_TIME_SIZE) {
          motion_st(motion_data_in[mpattern_idx*3], motion_data_in[mpattern_idx*3+1], motion_data_in[mpattern_idx*3+2], motion_time_in[mpattern_idx]);  
          last_motion_timestamp = motion_time_in[mpattern_idx];
          mpattern_idx++;          
        }
        count_pattern--;
      }
      
      if(mpattern_idx<ACC_OUT_TIME_SIZE) 
        //printf("mpattern_idx: %d (%2.2f%%), last timesamp: %d\n", mpattern_idx, (100.0*mpattern_idx/ACC_OUT_TIME_SIZE), last_motion_timestamp);      
        ;
      else                               {
        if(stop_mpattern==0) printf("No ACC PATTERN. Finish Test @ %d\n", (data_start)->time_stamp);       
        stop_mpattern = 1;
      }
#endif    
      
      result_count = check_sleep_results(); //get tracker results      
      fet_count    = check_sleep_motion_fet(); //get motion features      
      
      if(result_count>0) {
        #if (EN_ADAPTOR_SET_MSG)
          printf("sensor_fusion_algorithm_notify()-result, %d\n", result_count);
        #endif
        txcode = STATE_MARK; 
        sensor_fusion_algorithm_notify(SENSOR_TYPE_SLEEP); /* Directly notify sensor manager to get result - sleep efficiency, sleep states */         
      }
      
      if(fet_count>0) { 
        //printf("Feed %d motion fet\n", fet_count);
        for (j=0;j<fet_count;j++) {
          ret = get_one_motion_fet(&tmp_sleep_fet_sp.value, &tmp_sleep_fet_sp.timestamp);          
          if(ret==STATUS_OK) sleep_fet_run(tmp_sleep_fet_sp, pid, algo_state); /* process the advanced features extractions */          
        }
        rearrange_queue_for_motion_fet();        
      }
    }
    else if(input_list->data->sensor_type == SENSOR_TYPE_BIOSENSOR_PPG1){ /* PPG Thread */         
      #if (EN_ADAPTOR_GET_SENSOR_MSG)
        printf("Got %d PPG\n", count); 
      #endif
      #if 0
        while (count > 0) {  
          /* kernel of respiration feature extraction */
          resp_fet_ext((data_start)->bio_data.data, (data_start)->time_stamp); //input whenever get 1 sample for respiration
          data_start++; // move to next sample        
          count--;
        }
      #endif
    }
    else if(input_list->data->sensor_type == SENSOR_TYPE_BIOSENSOR_BISI) { /* BISI Thread */   
      #if (EN_ADAPTOR_GET_SENSOR_MSG)
        printf("Got %d BISI\n", count);
      #endif
      
#ifndef ACC_OUT_PATTERN            
      //printf("bisi@sleep_adaptor:%u,%u,@%d\n", (data_start)->bio_data.data, ((data_start)->bio_data.data)&0x00ffffff, (data_start)->time_stamp);      
      while (count > 0) { 
      
  #ifdef FASTX8_TEST
          //hr_fet_ext((data_start)->bio_data.data, (data_start)->time_stamp, 1);
          data0=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<3));
          // data1=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<3)+50*1);
          // data2=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<3)+50*2);
          // data3=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<3)+50*3);
          // data4=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<3)+50*4);
          // data5=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<3)+50*5);
          // data6=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<3)+50*6);
          // data7=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<3)+50*7);
          time0=(((data_start)->time_stamp)<<3); // for 128 Hz       
          hr_fet_ext(data0, time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*1), time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*2), time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*3), time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*4), time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*5), time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*6), time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*7), time0, 1, PPG_FS);
          printf("bisi time: %u %u; data: %u, %u-%u\n", (data_start)->time_stamp, time0, (data_start)->bio_data.data, data0, data0+(50*7));          
  #endif
  
  #ifdef FASTX4_TEST
          //hr_fet_ext((data_start)->bio_data.data, (data_start)->time_stamp, 1);
          data0=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<2));
          // data1=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<2)+50*1);
          // data2=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<2)+50*2);
          // data3=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<2)+50*3);
          time0=(((data_start)->time_stamp)<<2); // for 128 Hz
          hr_fet_ext(data0, time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*1), time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*2), time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*3), time0, 1, PPG_FS);
          //printf("bisi time: %u %u; data: %u, %u-%u\n", (data_start)->time_stamp, time0, (data_start)->bio_data.data, data0, data0+(50*3));          
  #endif
  
  #ifdef FASTX2_TEST
          //hr_fet_ext((data_start)->bio_data.data, (data_start)->time_stamp, 1);
          data0=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<1));
          // data1=(((data_start)->bio_data.data)&0xff000000) | (((((data_start)->bio_data.data)&0x00ffffff)<<1)+50*1);
          time0=(((data_start)->time_stamp)<<1); // for 128 Hz
          hr_fet_ext(data0, time0, 1, PPG_FS);
          hr_fet_ext(data0+(50*1), time0, 1, PPG_FS);
          printf("bisi time: %u %u; data: %u, %u-%u\n", (data_start)->time_stamp, time0, (data_start)->bio_data.data, data0, data0+(50*1));  
  #endif
  
  
  #if  !defined(FASTX8_TEST) && !defined(FASTX4_TEST)&& !defined(FASTX2_TEST)
          hr_fet_ext((data_start)->bio_data.data, (data_start)->time_stamp, 1, PPG_FS);
  #endif  

        data_start++;
        count--;
      }
#else      
      while((bisi_data_time_in[bipattern_idx*2+1]<last_motion_timestamp) && (bipattern_idx<((BISI_OUT_VALUE_TIME_SIZE>>1)-1))) { /* update until the alignment to motion timestamp */      
          hr_fet_ext(bisi_data_time_in[bipattern_idx*2], bisi_data_time_in[bipattern_idx*2+1], 1, PPG_FS);
          last_bisi_timestamp = bisi_data_time_in[bipattern_idx*2+1];
          bipattern_idx++;
      }
      
      if(bipattern_idx<((BISI_OUT_VALUE_TIME_SIZE>>1)-1)) 
        //printf("bipattern_idx: %d(%2.2f%%), last timesamp: %d\n", bipattern_idx, (100.0*bipattern_idx/(BISI_OUT_VALUE_TIME_SIZE>>1)), last_bisi_timestamp);
        ;
      else                               {
        if(stop_bipattern==0) printf("No BISI PATTERN. Finish Test @ %d\n", (data_start)->time_stamp);       
        stop_bipattern = 1;
      }
#endif    

    }
    
    /*-----------------------------------------------------------*/
    /* Phase 2 Input HR basic features to get advanced features  */
    /*-----------------------------------------------------------*/  
    if((input_list->data->sensor_type == SENSOR_TYPE_BIOSENSOR_BISI) || input_list->data->sensor_type == SENSOR_TYPE_BIOSENSOR_PPG1) {        
      fet_count = check_sleep_hr_fet();   // get heart rate features  
      if(fet_count > 0) {
        #if (EN_ADAPTOR_GET_SENSOR_MSG)
          printf("Got %d PPG features\n", fet_count);
        #endif
        //printf("Feed %d PPG fet\n", fet_count);
        for (j=0;j<fet_count;j++) {
          ret = get_one_hr_fet(&tmp_sleep_fet_sp.value, &tmp_sleep_fet_sp.timestamp);
          if(ret==STATUS_OK) sleep_fet_run(tmp_sleep_fet_sp, pid, algo_state); /* process the advanced features extractions */           
        }
        rearrange_queue_for_hr_fet();
      } 
    }    
    
    /*check the features for classifier*/
    advfet_count    = check_fet_ready();
    
    if(advfet_count) { 
      if(algo_state == INVOKE_STAGING) {
        #if (EN_ADAPTOR_SET_MSG)
          printf("sensor_fusion_algorithm_notify()-advanced fet, %d\n", advfet_count);
        #endif
        txcode = FEATURE_MARK0; // 1'st feature packet
        sensor_fusion_algorithm_notify(SENSOR_TYPE_SLEEP_STAGING); /* notify sensor manager to get sleep staging features */
        txcode = FEATURE_MARK1; // 2'nd feature packet
        sensor_fusion_algorithm_notify(SENSOR_TYPE_SLEEP_STAGING); /* notify sensor manager to get sleep staging features */
      }
      else if(algo_state == INVOKE_TRACKER) {
        #if (EN_ADAPTOR_SET_MSG)
          printf("sensor_fusion_algorithm_notify()-simple fet, %d\n", advfet_count);
        #endif
        txcode = FEATURE_MARK2; // 1'st feature packet
        sensor_fusion_algorithm_notify(SENSOR_TYPE_SLEEP); /* notify sensor manager to get motion features */
      }
    }
    /*-----------------------------------------------------------*/
    /* Test block for sleep tracker/staging switching            */
    /*-----------------------------------------------------------*/  
#if(TEST_TRACKER_STAGING_SWITCH)
    test_counter++; /* for sleep tracker/staging test */
    //printf("test_counter: %d\n", test_counter);
    if(test_counter > 200) {
      
      if(algo_state==INVOKE_TRACKER) {
        printf("switch to STAGING\n");
        txcode = INVOKE_STAGING; 
        sensor_fusion_algorithm_notify(SENSOR_TYPE_SLEEP); /* Directly notify sensor manager to get result - sleep efficiency, sleep states */                  
      }
      else if (algo_state==INVOKE_STAGING) {
        printf("switch to TRACKER\n");
        txcode = INVOKE_TRACKER; 
        sensor_fusion_algorithm_notify(SENSOR_TYPE_SLEEP_STAGING); /* Directly notify sensor manager to get result - sleep efficiency, sleep states */          
      }
    }
#endif    


#if(FORCE_STAGING_ONLY)  
    if(algo_state==INVOKE_TRACKER) {
      printf("switch to STAGING\n");
      txcode = INVOKE_STAGING; 
      sensor_fusion_algorithm_notify(SENSOR_TYPE_SLEEP); /* Directly notify sensor manager to get result - sleep efficiency, sleep states */                  
    }
#endif


    txcode = 0;
    pid++;
    
    return 1;
}


static int32_t sleep_operate(int32_t command, void* buffer_out, int32_t size_out, void* buffer_in, int32_t size_in)
{
  return 0;
}

const sensor_descriptor_t sleep_desp = {
    SENSOR_TYPE_SLEEP,    /* output_type */
    2,                    /* version */
    SENSOR_REPORT_MODE_ON_CHANGE,            /* report_mode */
    &tracker_input_comp_acc,      /* sensor_input_list_t */
    sleep_operate,
    get_sleep_data,
    set_sleep_data,
    0                     /* accumulate */
};

const sensor_descriptor_t sleep_staging_desp = {
    SENSOR_TYPE_SLEEP_STAGING,    /* output_type */
    2,                    /* version */
    SENSOR_REPORT_MODE_ON_CHANGE,            /* report_mode */
    &staging_input_comp_acc,      /* sensor_input_list_t */
    sleep_operate,
    get_sleep_data,
    set_sleep_data,
    0                     /* accumulate */
};

int sleep_register(void)
{
  int ret = 0;
  tracker_input_comp_acc.input_type     = SENSOR_TYPE_ACCELEROMETER;
  tracker_input_comp_acc.sampling_delay = 40; /* 40 ms, default: 25Hz */
  tracker_input_comp_acc.timeout        = 400; /* 400 ms for power reduction */
  tracker_input_comp_acc.next_input     = NULL;

  ret = sensor_fusion_algorithm_register_type(&sleep_desp);
  if(ret < 0) printf("%s():fail to register sleep\r\n", __func__);

  ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_SLEEP, 100);
  if(ret<0) LOGE("fail to register value \r\n");

  printf("%s():success to register sleep\r\n", __func__);  
  return ret;
}


int sleep_staging_register(void)
{
  int ret = 0;
  staging_input_comp_acc.input_type     = SENSOR_TYPE_ACCELEROMETER;
  staging_input_comp_acc.sampling_delay = 40; /* 40 ms, default: 25Hz */
  staging_input_comp_acc.timeout        = 400; /* 400 ms for power reduction */
  staging_input_comp_acc.next_input     = &staging_input_comp_bio;

  staging_input_comp_bio.input_type     = SENSOR_TYPE_BIOSENSOR_PPG1;
  staging_input_comp_bio.sampling_delay = 8; /* 8 ms, default: 125Hz */ //modify to 8 when MT2511 E1 is applied.
  staging_input_comp_bio.timeout        = 400; /* 400 ms for power reduction */
  staging_input_comp_bio.next_input     = &staging_input_comp_bisi;

  staging_input_comp_bisi.input_type     = SENSOR_TYPE_BIOSENSOR_BISI;
  staging_input_comp_bisi.sampling_delay = 100; /* 100 ms, default: 10Hz */
  staging_input_comp_bisi.timeout        = 400; /* 400 ms for power reduction */
  staging_input_comp_bisi.next_input     = NULL;

  ret = sensor_fusion_algorithm_register_type(&sleep_staging_desp);
  if(ret < 0) printf("%s():fail to register sleep staging\r\n", __func__);

  ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_SLEEP_STAGING, 100);
  if(ret<0) LOGE("fail to register value \r\n");

  printf("%s():success to register sleep staging\r\n", __func__);
  return ret;
}


int sleep_init(void)
{
  printf("sleep_init(void)\n");
  int ret = 0; 
  algo_state = INVOKE_TRACKER;
  test_counter = 0;
  gtime_latch = 0;
  if(watch_state==WATCH_COLD_INIT_STATE) {
    txcode   = 0;
    pid      = 0;    
    #if defined(NANO_MEASUREMENT) || defined(MINI_MEASUREMENT)
    timestamp_init();
    #endif
    motion_st_init();
    hr_fet_init();
    sleep_fet_init();
    //watch_state = WATCH_WARM_INIT_STATE;
  }
  else if(watch_state==WATCH_WARM_INIT_STATE) {
    /* TBD */    
  }
#ifdef ACC_OUT_PATTERN
  mpattern_idx = 0;
  bipattern_idx = 0;
  last_motion_timestamp = 0;
  last_bisi_timestamp = 0;
  stop_mpattern = 0;
  stop_bipattern = 0;
  
#endif
  return ret;
}

int sleep_staging_init(void)
{
  printf("sleep_staging_init(void)\n");
  int ret = 0;
  algo_state = INVOKE_STAGING;
  test_counter = 0;
  gtime_latch = 0;
#ifdef ACC_OUT_PATTERN
  watch_state = WATCH_COLD_INIT_STATE; // Force to reset all variables for pattern comparison
#endif  
  
  if(watch_state==WATCH_COLD_INIT_STATE) {
    txcode   = 0;
    pid      = 0;    
    #if defined(NANO_MEASUREMENT) || defined(MINI_MEASUREMENT)
    timestamp_init();
    #endif
    motion_st_init();
    hr_fet_init();
    sleep_fet_init();
    //watch_state = WATCH_WARM_INIT_STATE;
  }
  else if(watch_state==WATCH_WARM_INIT_STATE) {
    /* TBD */
  }
#ifdef ACC_OUT_PATTERN
  mpattern_idx = 0;
  bipattern_idx = 0;
  last_motion_timestamp = 0;
  last_bisi_timestamp = 0;
  stop_mpattern = 0;
  stop_bipattern = 0;  
#endif  

  return ret;
}

