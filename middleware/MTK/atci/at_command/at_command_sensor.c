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

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include "sensor_manager.h" /* sensor manager task */
#include "sensor_alg_interface.h"

#define LOGE(fmt,arg...)   LOG_E(atcmd, "SENSOR_ATCMD: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "SENSOR_ATCMD: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd ,"SENSOR_ATCMD: "fmt,##arg)

//+++++++++++++++++++++++++++++++++++++++++ atan

#if !defined(__CC_ARM) /* ARM compiler math.h with it*/

#define __LITTLE_ENDIAN

#ifdef __LITTLE_ENDIAN
#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#else
#define __HI(x) *(int*)&x
#define __LO(x) *(1+(int*)&x)
#endif
#endif

#ifdef __STDC__
    static double sensor_fabs(double x)
#else
    static double sensor_fabs(x)
    static double x;
#endif
{
    __HI(x) &= 0x7fffffff;
        return x;
}


#ifdef __STDC__
static const double atanhi[] = {
#else
static double atanhi[] = {
#endif
  4.63647609000806093515e-01, /* atan(0.5)hi 0x3FDDAC67, 0x0561BB4F */
  7.85398163397448278999e-01, /* atan(1.0)hi 0x3FE921FB, 0x54442D18 */
  9.82793723247329054082e-01, /* atan(1.5)hi 0x3FEF730B, 0xD281F69B */
  1.57079632679489655800e+00, /* atan(inf)hi 0x3FF921FB, 0x54442D18 */
};

#ifdef __STDC__
static const double atanlo[] = {
#else
static double atanlo[] = {
#endif
  2.26987774529616870924e-17, /* atan(0.5)lo 0x3C7A2B7F, 0x222F65E2 */
  3.06161699786838301793e-17, /* atan(1.0)lo 0x3C81A626, 0x33145C07 */
  1.39033110312309984516e-17, /* atan(1.5)lo 0x3C700788, 0x7AF0CBBD */
  6.12323399573676603587e-17, /* atan(inf)lo 0x3C91A626, 0x33145C07 */
};

#ifdef __STDC__
static const double aT[] = {
#else
static double aT[] = {
#endif
  3.33333333333329318027e-01, /* 0x3FD55555, 0x5555550D */
 -1.99999999998764832476e-01, /* 0xBFC99999, 0x9998EBC4 */
  1.42857142725034663711e-01, /* 0x3FC24924, 0x920083FF */
 -1.11111104054623557880e-01, /* 0xBFBC71C6, 0xFE231671 */
  9.09088713343650656196e-02, /* 0x3FB745CD, 0xC54C206E */
 -7.69187620504482999495e-02, /* 0xBFB3B0F2, 0xAF749A6D */
  6.66107313738753120669e-02, /* 0x3FB10D66, 0xA0D03D51 */
 -5.83357013379057348645e-02, /* 0xBFADDE2D, 0x52DEFD9A */
  4.97687799461593236017e-02, /* 0x3FA97B4B, 0x24760DEB */
 -3.65315727442169155270e-02, /* 0xBFA2B444, 0x2C6A6C2F */
  1.62858201153657823623e-02, /* 0x3F90AD3A, 0xE322DA11 */
};

#ifdef __STDC__
    static const double
#else
    static double
#endif
one   = 1.0,
huge   = (double)(1.0e300L);

#ifdef __STDC__
    static double sensor_atan(double x)
#else
    static double sensor_atan(x)
    static double x;
#endif
{
    double w,s1,s2,z;
    int ix,hx,id;

    hx = __HI(x);
    ix = hx&0x7fffffff;
    if(ix>=0x44100000) {    /* if |x| >= 2^66 */
        if(ix>0x7ff00000||
        (ix==0x7ff00000&&(__LO(x)!=0)))
        return x+x;     /* NaN */
        if(hx>0) return  atanhi[3]+atanlo[3];
        else     return -atanhi[3]-atanlo[3];
    } if (ix < 0x3fdc0000) {    /* |x| < 0.4375 */
        if (ix < 0x3e200000) {  /* |x| < 2^-29 */
        if(huge+x>one) return x;    /* raise inexact */
        }
        id = -1;
    } else {
    x = sensor_fabs(x);
    if (ix < 0x3ff30000) {      /* |x| < 1.1875 */
        if (ix < 0x3fe60000) {  /* 7/16 <=|x|<11/16 */
        id = 0; x = ((double)2.0*x-one)/((double)2.0+x);
        } else {            /* 11/16<=|x|< 19/16 */
        id = 1; x  = (x-one)/(x+one);
        }
    } else {
        if (ix < 0x40038000) {  /* |x| < 2.4375 */
        id = 2; x  = (x-(double)1.5)/(one+(double)1.5*x);
        } else {            /* 2.4375 <= |x| < 2^66 */
        id = 3; x  = (double)-1.0/x;
        }
    }}
    /* end of argument reduction */
    z = x*x;
    w = z*z;
    /* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
    s1 = z*(aT[0]+w*(aT[2]+w*(aT[4]+w*(aT[6]+w*(aT[8]+w*aT[10])))));
    s2 = w*(aT[1]+w*(aT[3]+w*(aT[5]+w*(aT[7]+w*aT[9]))));
    if (id<0) return x - x*(s1+s2);
    else {
        z = atanhi[id] - ((x*(s1+s2) - atanlo[id]) - x);
        return (hx<0)? -z:z;
    }
}
//----------------------------------------

static bool g_hide_acc = false;

static int total_data_count;
static int acc_count;
static int gyro_count;
static int mag_count;
static int baro_count;
static int prox_count;
double custom_atan(double x){
    LOGI("atan(%ld/10000) = %ld/10000\r\n", (int32_t)(x*10000),(int32_t)(sensor_atan(x)*10000) );
    return sensor_atan(x);
    //return 0;
}

int32_t get_degree(int32_t X, int32_t Y)
{
    double dwDegree;
    double lx, ly;

//    //+++++ TEST
//    double xx = -10000;
//    double yy = -50500;
//    double xy = xx/yy;
//    LOGI("test2\r\n");
//    custom_atan(xy);
//    LOGI("test2\r\n");
//    //-----

    lx = (double)X;
    ly = (double)Y;

    dwDegree = 180*custom_atan(lx/ly)/3.141592L;

    if (Y<0)
        dwDegree = 180 - dwDegree;
    else
        dwDegree = 360 - dwDegree;

    if (dwDegree >=360)
        dwDegree -= 360;

    return (int32_t)dwDegree;
}



int32_t sensor_send_digest_callback(sensor_data_t *const output)
{
    if ((total_data_count++) % 100 == 0) {
        LOGI("sensor type = %lu , value = ( %ld , %ld , %ld ) (%ld) , timestamp = %lu \r\n",
                output->data[0].sensor_type,
                output->data[0].value[0],
                output->data[0].value[1],
                output->data[0].value[2],
                output->data[0].value[3],
                output->data[0].time_stamp
              );
    }


    switch(output->data[0].sensor_type){
        case SENSOR_TYPE_ACCELEROMETER:
            //TODO need down sampling rate ?
            if(g_hide_acc == false){
                if ((acc_count++) % 100 == 0) {
                    LOGI("[QA_LOG] accelerometer data = (%ld,%ld,%ld) mm/s^2 , accuracy = %ld , timestamp = %lu \r\n",
                            output->data[0].accelerometer_t.x,
                            output->data[0].accelerometer_t.y,
                            output->data[0].accelerometer_t.z,
                            output->data[0].accelerometer_t.status,
                            output->data[0].time_stamp
                            );
                }
            }
            break;
        case SENSOR_TYPE_GYROSCOPE:
            if ((gyro_count++) % 100 == 0) {
                LOGI("[QA_LOG] gyroscope data = (%ld,%ld,%ld) m-degree/s , accuracy = %ld , timestamp = %lu \r\n",
                        output->data[0].gyroscope_t.x,
                        output->data[0].gyroscope_t.y,
                        output->data[0].gyroscope_t.z,
                        output->data[0].gyroscope_t.status,
                        output->data[0].time_stamp
                        );
            }
            break;
        case SENSOR_TYPE_MAGNETIC_FIELD:
            if ((mag_count++) % 100 == 0) {
                LOGI("[QA_LOG] magnetic field degree = %ld degree , accuracy = %ld , timestamp = %lu \r\n",
                        get_degree(output->data[0].magnetic_t.x, output->data[0].magnetic_t.y),
                        output->data[0].magnetic_t.status,
                        output->data[0].time_stamp
                        );
            }
            break;
        case SENSOR_TYPE_PRESSURE:
            if ((baro_count++) % 100 == 0) {
                LOGI("[QA_LOG] pressure data = %ld Pa  , timestamp = %lu \r\n",
                        output->data[0].pressure_t.pressure,
                        output->data[0].time_stamp
                        );
            }
            break;
        case SENSOR_TYPE_PROXIMITY:
            //show near/far
            if(output->data[0].distance == 0){
                LOGI("[QA_LOG] proximity data = Near  , timestamp = %lu \r\n",output->data[0].time_stamp);
            }else{
                LOGI("[QA_LOG] proximity data = Far  , timestamp = %lu \r\n",output->data[0].time_stamp);
            }
            prox_count++;
            break;
        default :
            LOGI("SQC not inlcude this sensor type\r\n");
            break;
    }

    return 0;
}

sensor_subscriber_t sensors_subscriber_at[SENSOR_TYPE_ALL] = {
    {"ap0", 0, SENSOR_TYPE_ACCELEROMETER, 10, sensor_send_digest_callback, 200},
    {"ap1", 0, SENSOR_TYPE_MAGNETIC_FIELD, 20, sensor_send_digest_callback, 200},
    {"ap2", 0, SENSOR_TYPE_PRESSURE, 20, sensor_send_digest_callback, 200},
    {"ap3", 0, SENSOR_TYPE_GYROSCOPE, 5, sensor_send_digest_callback, 200},
    {"ap4", 0, SENSOR_TYPE_PROXIMITY, 0, sensor_send_digest_callback, 0},
};


#define BARO_AUTO_TIMES 10
#define BARO_SPEC_UP_BOUND_PER 105 // base pressure * 1.05
#define BARO_SPEC_LOW_BOUND_PER 95 // base pressure * 0.95

bool baro_auto_test(int32_t base_pressure){
    int32_t baro_up_bound = (base_pressure*BARO_SPEC_UP_BOUND_PER)/100;
    int32_t baro_low_bound = (base_pressure*BARO_SPEC_LOW_BOUND_PER)/100;
    sensor_data_unit_t output_data_unit_t;
    sensor_data_t output;
    int32_t baro_data = 0;
    int i;
    output.data = &output_data_unit_t;

    LOGI("[QA_LOG] baro up bound = %ld , low bound = %ld \r\n",baro_up_bound,baro_low_bound);

    for(i=0;i<BARO_AUTO_TIMES;i++){
        sensor_get_latest_sensor_data(SENSOR_TYPE_PRESSURE, &output);
        LOGI("[QA_LOG] pressure data = %ld Pa  , timestamp = %lu \r\n",
                output.data[0].pressure_t.pressure,
                output.data[0].time_stamp
                );
        baro_data = baro_data + output.data[0].pressure_t.pressure;
    }
    LOGI("[QA_LOG] pressure total data = %ld Pa  \r\n",baro_data);
    baro_data = baro_data/BARO_AUTO_TIMES;
    LOGI("[QA_LOG] pressure avg data = %ld Pa  \r\n",baro_data);
    if( (baro_data>baro_up_bound) || (baro_data<baro_low_bound) ){
        LOGI("[QA_LOG] auto test fail baro value out of range\r\n");
        return false;
    }
    return true;
}



#define GYRO_AUTO_TIMES 10
#define GYRO_SPEC_UP_BOUND   5000  // 5000 m-degree (  5 degree )
#define GYRO_SPEC_LOW_BOUND -5000  //-5000 m-degree ( -5 degree )
bool gyro_auto_test(){
    sensor_data_unit_t output_data_unit_t;
    sensor_data_t output;
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
    int i;
    output.data = &output_data_unit_t;

    //debug_get_user_confirm_sync("Place DUT on desk and Z+ up to sky.\r\nPress Yes to do Gyro auto test\r\n");

    for(i=0;i<GYRO_AUTO_TIMES;i++){
        sensor_get_latest_sensor_data(SENSOR_TYPE_GYROSCOPE, &output);
        LOGI("[QA_LOG] gyroscope data = (%ld,%ld,%ld) m-degree/s , accuracy = %ld , timestamp = %lu \r\n",
                output.data[0].gyroscope_t.x,
                output.data[0].gyroscope_t.y,
                output.data[0].gyroscope_t.z,
                output.data[0].gyroscope_t.status,
                output.data[0].time_stamp
                );
        x = x + output.data[0].gyroscope_t.x;
        y = y + output.data[0].gyroscope_t.y;
        z = z + output.data[0].gyroscope_t.z;
    }

    LOGI("[QA_LOG] gyroscope total = (%ld,%ld,%ld) m-degree/s \r\n",x,y,z);
    x = x/GYRO_AUTO_TIMES;
    y = y/GYRO_AUTO_TIMES;
    z = z/GYRO_AUTO_TIMES;
    LOGI("[QA_LOG] gyroscope avg   = (%ld,%ld,%ld) m-degree/s \r\n",x,y,z);

    if( (x>GYRO_SPEC_UP_BOUND) || (x<GYRO_SPEC_LOW_BOUND) ){
        LOGI("[QA_LOG] auto test fail x value out of range\r\n");
        return false;
    }

    if( (y>GYRO_SPEC_UP_BOUND) || (y<GYRO_SPEC_LOW_BOUND) ){
        LOGI("[QA_LOG] auto test fail y value out of range\r\n");
        return false;
    }

    if( (z>GYRO_SPEC_UP_BOUND) || (z<GYRO_SPEC_LOW_BOUND) ){
        LOGI("[QA_LOG] auto test fail y value out of range\r\n");
        return false;
    }

    return true;
}





sensor_subscriber_t sanity_sensor_subscriber_at[5] = {
        {"ap1", 0, SENSOR_TYPE_ACCELEROMETER, 10, NULL, 200},
        {"ap2", 0, SENSOR_TYPE_MAGNETIC_FIELD, 20, NULL, 200},
        {"ap3", 0, SENSOR_TYPE_PRESSURE, 20, NULL, 200},
        {"ap4", 0, SENSOR_TYPE_GYROSCOPE, 5, NULL, 200},
        {"ap5", 0, SENSOR_TYPE_PROXIMITY, 0, NULL, 0}
};

#define DATA_CHECK_TIMES 10

int sanity_check(int sensor_type)
{
    int i;
    int is_pass = 0;
    sensor_data_unit_t output_data_unit_t;
    sensor_data_t output;
    output.data = &output_data_unit_t;

    LOGI("Sensor(%d) ON\r\n",sensor_type);
    is_pass = 0;

    if(sensor_type == SENSOR_TYPE_GYROSCOPE){
        // enable gyro , also need ot enable acc
        sensor_subscribe_sensor(&sanity_sensor_subscriber_at[SENSOR_TYPE_ACCELEROMETER]);
    }
    sensor_subscribe_sensor(&sanity_sensor_subscriber_at[sensor_type]);
    LOGI("Sensor(%d) DATA CHECK\r\n",sensor_type);

    vTaskDelay(3000);

    if(sensor_type == SENSOR_TYPE_PROXIMITY){
        // proximity sensor check enabled
        if( sensor_driver_get_enable(SENSOR_TYPE_PROXIMITY) == 1 ){
            is_pass = 1;
        }

    }else{
        for(i=0;i<DATA_CHECK_TIMES;i++){
            sensor_get_latest_sensor_data(sensor_type, &output);
            LOGI("Sensor(%d) DATA %d. (%ld,%ld,%ld) \r\n",sensor_type,i,output.data[0].value[0],output.data[0].value[1],output.data[0].value[2]);
            if(output.data[0].value[0]!=0 || output.data[0].value[1]!=0 || output.data[0].value[2]!=0 ){
                LOGI("Sensor(%d) DATA not zero , pass\r\n",sensor_type);
                is_pass = 1;
                break;
            }
        }
    }

    sensor_unsubscribe_sensor(sanity_sensor_subscriber_at[sensor_type].handle);
    if(sensor_type == SENSOR_TYPE_GYROSCOPE){
        // enable gyro , also need ot enable acc
        sensor_unsubscribe_sensor(sanity_sensor_subscriber_at[SENSOR_TYPE_ACCELEROMETER].handle);
    }
    LOGI("Sensor(%d) OFF\r\n",sensor_type);
    if(is_pass == 0){
        switch(sensor_type){
        case SENSOR_TYPE_ACCELEROMETER:
            LOGI("[QA_LOG] Accelerometer Sensor FAIL\r\n");
            break;
        case SENSOR_TYPE_GYROSCOPE:
            LOGI("[QA_LOG] Gyroscope Sensor FAIL\r\n");
            break;
        case SENSOR_TYPE_MAGNETIC_FIELD:
            LOGI("[QA_LOG] Magnetic Field Sensor FAIL\r\n");
            break;
        case SENSOR_TYPE_PRESSURE:
            LOGI("[QA_LOG] Pressure Sensor FAIL\r\n");
            break;
        case SENSOR_TYPE_PROXIMITY:
            LOGI("[QA_LOG] Proximity Sensor FAIL\r\n");
            break;
        default:
            LOGI("[QA_LOG] FAIL\r\n");
            break;
        }
        return 0;
    }
    return 1;
}




/*--- Function ---*/
atci_status_t atci_cmd_hdlr_sensor(atci_parse_cmd_param_t *parse_cmd);

/*
AT+ESENSOR=<op>                  |   "OK"
AT+ESENSOR=?                     |   "+ESENSOR =<op>[,<param>]","OK"

URC:
+ESENSOR: <sensor mode>

*/
/* AT command handler */
atci_status_t atci_cmd_hdlr_sensor(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    bool test_result = true;
    int32_t base_pressure;

    resonse.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {

        case ATCI_CMD_MODE_TESTING:
            strcpy((char *)resonse.response_buf, "+ESENSOR =<op>[,<param>]\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION:

            if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=0,1") != NULL) {
                /* A on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER]);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=0,0") != NULL) {
                /* A off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER].handle);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=1,1") != NULL) {
                /* A+G on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER]);
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE]);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=1,0") != NULL) {
                /* A+G off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE].handle);
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER].handle);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=2,1") != NULL) {
                /* M on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_MAGNETIC_FIELD]);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=2,0") != NULL) {
                /* M off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_MAGNETIC_FIELD].handle);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=3,1") != NULL) {
                /* B on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_PRESSURE]);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=3,0") != NULL) {
                /* B off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_PRESSURE].handle);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=4,1") != NULL) {
                /* P on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_PROXIMITY]);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=4,0") != NULL) {
                /* P off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_PROXIMITY].handle);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=5,1") != NULL) {
                /* all on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER]);
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE]);
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_MAGNETIC_FIELD]);
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_PRESSURE]);
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_PROXIMITY]);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=5,0") != NULL) {
                /* all off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE].handle);
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER].handle);
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_MAGNETIC_FIELD].handle);
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_PRESSURE].handle);
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_PROXIMITY].handle);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSOR=6,0") != NULL) {
                /* cal */
                int input = 0;
                sensor_manager_set_cust_from_app(SENSOR_TYPE_GYROSCOPE, (void *)(&input), sizeof(int));

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_001,on") != NULL) {
                /* acc on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER]);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_001,off") != NULL) {
                /* acc off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER].handle);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_002,on") != NULL) {
                /* acc gyro on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER]);
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE]);
                g_hide_acc = true;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_002,off") != NULL) {
                /* acc gyro off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE].handle);
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER].handle);
                g_hide_acc = false;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_003,on") != NULL) {
                /* mag on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_MAGNETIC_FIELD]);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_003,off") != NULL) {
                /* mag off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_MAGNETIC_FIELD].handle);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_004,on") != NULL) {
                /* baro on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_PRESSURE]);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_004,off") != NULL) {
                /* baro off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_PRESSURE].handle);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_005,on") != NULL) {
                /* prox on */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_PROXIMITY]);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_005,off") != NULL) {
                /* prox off */
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_PROXIMITY].handle);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_auto_001") != NULL) {
                /* gyro auto test */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER]);
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE]);
                g_hide_acc = true;

                // delay 2 sec
                vTaskDelay(2000);
                test_result = gyro_auto_test();

                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE].handle);
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER].handle);
                g_hide_acc = false;

                if(test_result == true){
                    strcpy((char *)resonse.response_buf, "hw_auto_001 PASS\r\n");
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                }else{
                    strcpy((char *)resonse.response_buf, "hw_auto_001 FAIL\r\n");
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                }

                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=hw_auto_002") != NULL) {
                //TODO parse pressure value
                base_pressure = 101325;
                LOGI("base_pressure = %d\r\n",base_pressure);
                /* baro auto test */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_PRESSURE]);
                // delay 2 sec
                vTaskDelay(2000);
                test_result = baro_auto_test(base_pressure);
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_PRESSURE].handle);

                if(test_result == true){
                    strcpy((char *)resonse.response_buf, "hw_auto_002 PASS\r\n");
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                }else{
                    strcpy((char *)resonse.response_buf, "hw_auto_002 FAIL\r\n");
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                }

                atci_send_response(&resonse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=cal_001,on") != NULL) {
                /* cal */
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER]);
                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE]);

                int input = 0;
                sensor_manager_set_cust_from_app(SENSOR_TYPE_GYROSCOPE, (void *)(&input), sizeof(int));

//                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE].handle);
//                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER].handle);
//
//                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER]);
//                sensor_subscribe_sensor(&sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE]);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=cal_001,off") != NULL) {
                /* cal , off sensor*/

                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_GYROSCOPE].handle);
                sensor_unsubscribe_sensor(sensors_subscriber_at[SENSOR_TYPE_ACCELEROMETER].handle);

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }else if (strstr((char *)parse_cmd->string_ptr, "AT+ESENSORTEST=sanity") != NULL) {
                /* cal , off sensor*/

                int ret = 0;

                ret = sanity_check(SENSOR_TYPE_ACCELEROMETER);
                if(ret == 0){
                    strcpy((char *)resonse.response_buf, "Acc FAIL\r\n");
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }
                ret = sanity_check(SENSOR_TYPE_GYROSCOPE);
                if(ret == 0){
                    strcpy((char *)resonse.response_buf, "Gyro FAIL\r\n");
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }
                ret = sanity_check(SENSOR_TYPE_MAGNETIC_FIELD);
                if(ret == 0){
                    strcpy((char *)resonse.response_buf, "Mag FAIL\r\n");
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }
                ret = sanity_check(SENSOR_TYPE_PRESSURE);
                if(ret == 0){
                    strcpy((char *)resonse.response_buf, "Baro FAIL\r\n");
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }
                ret = sanity_check(SENSOR_TYPE_PROXIMITY);
                if(ret == 0){
                    strcpy((char *)resonse.response_buf, "Prox FAIL\r\n");
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }

                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }

            break;

        default :
            /* others are invalid command format */
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}
