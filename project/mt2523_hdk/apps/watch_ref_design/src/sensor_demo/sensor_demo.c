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

#include "sensor_demo.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include <string.h>

#ifdef SENSOR_DEMO
/* syslog */
#define SENSOR_DEMO_LOGI(fmt,...) LOG_I(sensor,  (fmt), ##__VA_ARGS__)
/* printf*/
//#define SENSOR_DEMO_LOGI(fmt,arg...)   printf("SENSOR_DEMO:INFO: "fmt,##arg)
#endif

#ifdef SENSOR_BTSPP
 /*bt spp include */
#include "bt_spp.h"
#include "bt_common_dispatch.h"

 /*bt spp define buffer */
#define SPPS_PROFILE_TX_BUFF_SIZE BT_SPP_MIN_TX_BUFSIZE  /*must be >= BT_SPP_MIN_TX_BUFSIZE*/
#define SPPS_PROFILE_RX_BUFF_SIZE BT_SPP_MIN_RX_BUFSIZE  /*must be >= BT_SPP_MIN_RX_BUFSIZE*/
 uint8_t spp_buf[SPPS_PROFILE_TX_BUFF_SIZE + SPPS_PROFILE_RX_BUFF_SIZE];

 bt_spp_common_callback spp_server_callback;
 bt_spp_port_id_t port_id;
 uint32_t bt_spp_is_connected;

/* bluetooth includes */
#include "bt_gap.h"
#define SENSOR_SPP_DATA_MAGIC       54321
#define SENSOR_SPP_DATA_RESERVED    12345

void BTBMDumpBdAddr(uint8_t *addr)
{
    SENSOR_DEMO_LOGI("Random Address %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\r\n",
                addr[5], addr[4], addr[3], addr[2],
                addr[1], addr[0]);
}

void bt_spp_connect_ind(bt_spp_connect_ind_t *parameter)
{
    int32_t result;
    SENSOR_DEMO_LOGI("app_spps_handle_connect_ind()\n");
    if (parameter == NULL) {
        return;
    }

    if (port_id == parameter->port) {
        result =  bt_spp_connect_response(
                      parameter->port,
                      true,
                      spp_buf,
                      SPPS_PROFILE_TX_BUFF_SIZE,
                      spp_buf + SPPS_PROFILE_TX_BUFF_SIZE,
                      SPPS_PROFILE_RX_BUFF_SIZE
                  );
        SENSOR_DEMO_LOGI("spp server responese result: %d\n", result);
    }
    return;
}


void bt_spp_connect_cnf(bt_spp_connect_cnf_t *parameter)
{

    SENSOR_DEMO_LOGI("app_spps_handle_connect_cnf()\n");
    if (parameter == NULL) {
        return;
    }

    if (parameter->result == BT_STATUS_SUCCESS) { /**< connect sucessfully*/
        bt_spp_is_connected = true;
    } else { /**< connect failed*/
        ;//error handle
    }
    return;
}


void append_data_twenty(int32_t sensor_type, int32_t x, int32_t y, int32_t z, int32_t time_stamp)
{
    int real_write_len = 0;
    int spps_data_length = 20;

    if (bt_spp_is_connected) {
        int32_t DATA[5];
        DATA[0] = sensor_type;
        DATA[1] = x;
        DATA[2] = y;
        DATA[3] = z;
        DATA[4] = time_stamp;
        real_write_len = bt_spp_write(port_id, (void *)DATA, spps_data_length);
        memset(DATA, 0, 20);
    }
    (void)real_write_len;
}

void append_data_twelve(int32_t sensor_type, int32_t value, int32_t time_stamp)
{
    int real_write_len = 0;
    int spps_data_length = 12;

    if (bt_spp_is_connected) {
        int32_t DATA[3];
        DATA[0] = sensor_type;
        DATA[1] = value;
        DATA[2] = time_stamp;
        real_write_len = bt_spp_write(port_id, (void *)DATA, spps_data_length);
        memset(DATA, 0, 12);
    }
    (void)real_write_len;
}

void append_data(int32_t number, int32_t sensor_type, int32_t x, int32_t y, int32_t z, int32_t status, int32_t time_stamp)
{
    int real_write_len = 0;
    int spps_data_length = 28;

    if (bt_spp_is_connected) {
        int32_t DATA[7];
        DATA[0] = sensor_type;
        DATA[1] = sensor_type;
        DATA[2] = x;
        DATA[3] = y;
        DATA[4] = z;
        DATA[5] = status;
        DATA[6] = time_stamp;
        real_write_len = bt_spp_write(port_id, (void *)DATA, spps_data_length);
        memset(DATA, 0, 28);
    }
    (void)real_write_len;
}

//#define SENSOR_BT_SPP_64BYTES_FORMAT_ENABLED


int send_sensor_data_via_btspp(int32_t magic, int32_t sensor_type, int32_t x, int32_t y, int32_t z, int32_t status, int32_t time_stamp)
{
#if SENSOR_BT_SPP_64BYTES_FORMAT_ENABLED
    int real_write_len = 0;
    int spps_data_length = 64;
    int ret = 0;
    int i=0;
    if (bt_spp_is_connected) {
        int32_t DATA[16];
        DATA[0] = magic;
        DATA[1] = sensor_type;
        DATA[2] = 0;//seq
        DATA[3] = x;
        DATA[4] = y;
        DATA[5] = z;
        DATA[6] = status;
        DATA[7] = time_stamp;
        for(i=8;i<16;i++){
            DATA[i] = 12345;//reserved
        }
        real_write_len = bt_spp_write(port_id, (void *)DATA, spps_data_length);
        if(real_write_len!=64){
//            bt_spp_is_connected = false;
//            printf("buffer full\r\n");
            ret = -1;
        }
        memset(DATA, 0, 64);
    }
#else
    int real_write_len = 0;
    int spps_data_length = 28;
    int ret = 0;
    if (bt_spp_is_connected) {
        int32_t DATA[7];
        DATA[0] = magic;
        DATA[1] = sensor_type;
        DATA[2] = x;
        DATA[3] = y;
        DATA[4] = z;
        DATA[5] = status;
        DATA[6] = time_stamp;
        real_write_len = bt_spp_write(port_id, (void *)DATA, spps_data_length);
        if(real_write_len!=28){
//            bt_spp_is_connected = false;
//            printf("buffer full\r\n");
            ret = -1;
        }
        memset(DATA, 0, 28);
    }
#endif
    return ret;
}


void spps_handle_disconnect_ind(bt_spp_disconnect_ind_t *parameter)
{
    SENSOR_DEMO_LOGI("app_spps_handle_disconnect_ind()\n");
    if (parameter == NULL) {
        return;
    }

    if (parameter->result == BT_STATUS_SUCCESS) {  /**< disconnect sucessfully*/
        bt_spp_is_connected = false;
        SENSOR_DEMO_LOGI("spp server app disconnected.\n");
    } else { /**< disconnect failed*/
        ;//error handle
    }
    return;
}

void spps_event_handler(bt_event_t event_id, const void *parameter)
{
    SENSOR_DEMO_LOGI("spps_event_handler :%d\n", event_id);

    switch (event_id) {
        case BT_SPP_CONNECT_IND:
            bt_spp_connect_ind((bt_spp_connect_ind_t *)parameter);
            show_sensor_connected_screen();
            break;
        case BT_SPP_CONNECT_CNF:
            bt_spp_connect_cnf((bt_spp_connect_cnf_t *) parameter);
            show_sensor_connected_screen();
            break;
        case BT_SPP_DISCONNECT_IND:
            spps_handle_disconnect_ind((bt_spp_disconnect_ind_t *) parameter);
            show_sensor_ready_to_connect_screen();
            break;
//      case BT_SPP_READY_TO_WRITE_IND:
//            bt_spp_ready_to_write_ind((bt_spp_ready_to_write_ind_t*) parameter);
//            break;
        default:
            break;
    }
}

void enable_bt_spp_server(void)
{
    spp_server_callback = spps_event_handler;
    int32_t result;
    SENSOR_DEMO_LOGI("app_spps_enable_server()\n");
    result = bt_spp_start_service(NULL, NULL, spp_server_callback);
    if (result > 0) { //enable server port succussfully
        port_id = result;
    } else { //enable server port failed
        //error handler
        //return false;
    }
    return;
}

void disable_bt_spp_server(void)
{
    SENSOR_DEMO_LOGI("app_spps_disable_server()\n");
    bt_spp_stop_service(port_id);
}

#else
void enable_bt_spp_server(void)
{

}

void disable_bt_spp_server(void)
{

}
#endif /*SENSOR_BTSPP */

#ifdef SENSOR_DEMO

#include "hal_eint.h"
#include "hal_i2c_master.h"
#include "hal_spi_master.h"

/* sensor subsys includes */
#include "mems_bus.h"
#include "sensor_manager.h" /* sensor manager task */
#include "sensor_alg_interface.h"

#ifdef MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160
#include "bmi160.h"
#endif

#ifdef MTK_SENSOR_ACCELEROMETER_USE_BMA255
#include "bma255_sensor_adaptor.h"
#endif

#ifdef MTK_SENSOR_PROXIMITY_USE_CM36672
#include "cm36672.h"
#endif

//#define SENSORS_ON_EVB

/* pin mux setting of EVB */
#ifdef SENSORS_ON_EVB
#include "hal_gpio.h"

void sensor_peripheral_init()
{
    SENSOR_DEMO_LOGI("\n\r sensorhub_peripheral_init ++\n\r");

    /* 1. Config SPI3 */
    hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_MA_SPI3_B_CS);
    hal_pinmux_set_function(HAL_GPIO_33, HAL_GPIO_33_MA_SPI3_B_SCK);
    hal_pinmux_set_function(HAL_GPIO_34, HAL_GPIO_34_MA_SPI3_B_MOSI);
    hal_pinmux_set_function(HAL_GPIO_35, HAL_GPIO_35_MA_SPI3_B_MISO);

    hal_spi_master_config_t spi_config;
    spi_config.clock_frequency = 0x400000;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;

    mems_spi_init(SPI_SELECT_BMI160, HAL_SPI_MASTER_3, &spi_config, HAL_SPI_MASTER_MACRO_GROUP_B);

    /* 2. Config I2C0 */
    mems_i2c_init(HAL_I2C_MASTER_0, HAL_I2C_FREQUENCY_400K);

    /*3. Config EINT*/
  #if  defined(MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160) || defined(MTK_SENSOR_ACCELEROMETER_USE_BMA255)
    /* GPIO[14] function1 for EINT12, BMI160 */
    hal_pinmux_set_function(HAL_GPIO_14, HAL_GPIO_14_EINT12);
    hal_eint_config_t eint1_config;
    eint1_config.trigger_mode = HAL_EINT_EDGE_RISING;
    eint1_config.debounce_time = 1;
    hal_eint_init(HAL_EINT_NUMBER_12, &eint1_config);

#ifdef MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160
    bmi160_eint_set(HAL_EINT_NUMBER_12);
    hal_eint_register_callback(HAL_EINT_NUMBER_12, bmi160_eint_handler, NULL);
#endif

#ifdef MTK_SENSOR_ACCELEROMETER_USE_BMA255
    bma255_eint_set(HAL_EINT_NUMBER_12);
    hal_eint_register_callback(HAL_EINT_NUMBER_12, bma255_eint_handler, NULL);
#endif

#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_unmask(HAL_EINT_NUMBER_12);
#endif
  #endif

  #ifdef MTK_SENSOR_PROXIMITY_USE_CM36672
    /* GPIO[11] function1 for EINT9, PS_EINT */
    hal_pinmux_set_function(HAL_GPIO_11, HAL_GPIO_11_EINT9);
    hal_eint_config_t eint3_config;
    eint3_config.trigger_mode = HAL_EINT_EDGE_FALLING;
    eint3_config.debounce_time = 1;
    cm36672_eint_set(HAL_EINT_NUMBER_9);
    hal_eint_init(HAL_EINT_NUMBER_9, &eint3_config);
    hal_eint_register_callback(HAL_EINT_NUMBER_9, cm36672_eint_handler, NULL);
#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_unmask(HAL_EINT_NUMBER_9);
#endif
  #endif
    SENSOR_DEMO_LOGI("\n\r sensorhub_peripheral_init --\n\r");
}

/* pin mux setting of HDK */
#else
void sensor_peripheral_init()
{
    SENSOR_DEMO_LOGI("\n\r sensorhub_peripheral_init ++\n\r");

    /* 1. Config SPI3 */
    hal_spi_master_config_t spi_config;
    spi_config.clock_frequency = 0x400000;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;

    mems_spi_init(SPI_SELECT_BMI160, HAL_SPI_MASTER_3, &spi_config, HAL_SPI_MASTER_MACRO_GROUP_A);

    /* 2. Config I2C2 */
    mems_i2c_init(HAL_I2C_MASTER_0, HAL_I2C_FREQUENCY_400K);

    /*3. Config EINT*/
  #if defined(MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160)  || defined(MTK_SENSOR_ACCELEROMETER_USE_BMA255)
    /* GPIO[5] function1 for EINT4, BMI160 */
    hal_eint_config_t eint1_config;
    eint1_config.trigger_mode = HAL_EINT_EDGE_RISING;
    eint1_config.debounce_time = 1;
    hal_eint_init(HAL_EINT_NUMBER_4, &eint1_config);

#ifdef MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160
    bmi160_eint_set(HAL_EINT_NUMBER_4);
    hal_eint_register_callback(HAL_EINT_NUMBER_4, bmi160_eint_handler, NULL);
#endif

#ifdef MTK_SENSOR_ACCELEROMETER_USE_BMA255
    bma255_eint_set(HAL_EINT_NUMBER_4);
    hal_eint_register_callback(HAL_EINT_NUMBER_4, bma255_eint_handler, NULL);
#endif

#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_unmask(HAL_EINT_NUMBER_4);
#endif
  #endif

  #ifdef MTK_SENSOR_PROXIMITY_USE_CM36672
    /* GPIO[6] function1 for EINT5, PS_EINT */
    hal_eint_config_t eint3_config;
    eint3_config.trigger_mode = HAL_EINT_EDGE_FALLING;
    eint3_config.debounce_time = 1;
    cm36672_eint_set(HAL_EINT_NUMBER_5);
    hal_eint_init(HAL_EINT_NUMBER_5, &eint3_config);
    hal_eint_register_callback(HAL_EINT_NUMBER_5, cm36672_eint_handler, NULL);
#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_unmask(HAL_EINT_NUMBER_5);
#endif
  #endif
    SENSOR_DEMO_LOGI("\n\r sensorhub_peripheral_init --\n\r");
}
#endif

int32_t all_sensor_send_digest_callback(sensor_data_t *const output);
#ifdef FUSION_PEDOMETER_USE
sensor_subscriber_t pedometer_subscriber = {
    "ap12", 0, SENSOR_TYPE_PEDOMETER, 0, all_sensor_send_digest_callback, 0
};
#endif

#ifdef FUSION_SLEEP_TRACKER_USE
sensor_subscriber_t sleep_tracker_subscriber = {
    "ap13", 0, SENSOR_TYPE_SLEEP, 0, all_sensor_send_digest_callback, 0
};
#endif

#ifdef FUSION_HEART_RATE_MONITOR_USE
sensor_subscriber_t heart_rate_monitor_subscriber = {
    "ap21", 0, SENSOR_TYPE_HEART_RATE_MONITOR, 0, all_sensor_send_digest_callback, 0
};
uint32_t hr_triggered;
#endif

#ifdef FUSION_HEART_RATE_VARIABILITY_USE
sensor_subscriber_t heart_rate_variability_subscriber = {
    "ap22", 0, SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR, 0, all_sensor_send_digest_callback, 0
};
uint32_t hrv_triggered;
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
sensor_subscriber_t blood_pressure_subscriber = {
    "ap23", 0, SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, 0, all_sensor_send_digest_callback, 0
};
uint32_t bp_triggered;
#endif


#ifdef SENSOR_BTSPP
void append_data_twenty(int32_t sensor_type, int32_t x, int32_t y, int32_t z, int32_t time_stamp);
void append_data_twelve(int32_t sensor_type, int32_t value, int32_t time_stamp);
void append_data(int32_t number, int32_t sensor_type, int32_t x, int32_t y, int32_t z, int32_t status, int32_t time_stamp);
#else
#define append_data_twenty(fmt, args...)
#define append_data_twelve(fmt, args...)
#define append_data(fmt, args...)
#endif /* SENSOR_BTSPP */

static uint32_t acc_count, gyro_count, mag_count, baro_count;

int32_t all_sensor_send_digest_callback(sensor_data_t *const output)
{
    switch (output->data[0].sensor_type) {
        case SENSOR_TYPE_ACCELEROMETER:
#if 1
        if ((acc_count++) % 400 == 0) {
            SENSOR_DEMO_LOGI("type = %lu , value = ( %ld , %ld , %ld )(mm/s^2) , timestampe = %lu \r\n",
                   output->data[0].sensor_type,
                   output->data[0].accelerometer_t.x,
                   output->data[0].accelerometer_t.y,
                   output->data[0].accelerometer_t.z,
                   output->data[0].time_stamp);
        }
#endif
        append_data(0,
                    output->data[0].sensor_type,
                    output->data[0].accelerometer_t.x,
                    output->data[0].accelerometer_t.y,
                    output->data[0].accelerometer_t.z,
                    output->data[0].time_stamp,
                    9);
            break;
        case SENSOR_TYPE_MAGNETIC_FIELD:

            if ((mag_count++) % 200 == 0) {
                SENSOR_DEMO_LOGI("type = %lu , value = (%ld.%03ld, %ld.%03ld, %ld.%03ld) uT Status(%ld), timestampe = %lu \r\n",
                       output->data[0].sensor_type,
                       output->data[0].magnetic_t.x/1000, output->data[0].magnetic_t.x%1000,
                       output->data[0].magnetic_t.y/1000, output->data[0].magnetic_t.y%1000,
                       output->data[0].magnetic_t.y/1000, output->data[0].magnetic_t.y%1000,
                       output->data[0].magnetic_t.status,
                       output->data[0].time_stamp
                       );
            }
            append_data(1,
            output->data[0].sensor_type,
            output->data[0].magnetic_t.x,
            output->data[0].magnetic_t.y,
            output->data[0].magnetic_t.z,
            output->data[0].magnetic_t.status,
            output->data[0].time_stamp);
            break;
        case SENSOR_TYPE_PRESSURE:
#if 1
            if ((baro_count++) % 200 == 0) {
                SENSOR_DEMO_LOGI("type = %lu , value = (%ld.%02ld) hPa , timestampe = %lu \r\n",
                       output->data[0].sensor_type,
                       output->data[0].pressure_t.pressure / 100,
                       output->data[0].pressure_t.pressure % 100,
                       output->data[0].time_stamp);
            }
#endif
            append_data(       2,
                               output->data[0].sensor_type,
                               output->data[0].pressure_t.pressure,
                               output->data[0].time_stamp,
                               9,
                               9,
                               9);
            break;
        case SENSOR_TYPE_GYROSCOPE:
#if 1
        if ((gyro_count++) % 400 == 0) {
            SENSOR_DEMO_LOGI("type = %lu , value = ( %ld , %ld , %ld )(degree/s) , timestampe = %lu \r\n",
                   output->data[0].sensor_type,
                   output->data[0].accelerometer_t.x,
                   output->data[0].accelerometer_t.y,
                   output->data[0].accelerometer_t.z,
                   output->data[0].time_stamp);
        }
#endif

        append_data(       3,
                           output->data[0].sensor_type,
                           output->data[0].gyroscope_t.x,
                           output->data[0].gyroscope_t.y,
                           output->data[0].gyroscope_t.z,
                           output->data[0].time_stamp,
                           9);
            break;

        case SENSOR_TYPE_PROXIMITY:
#if 1
            SENSOR_DEMO_LOGI("type = %lu , value = (%ld) , timestampe = %lu \r\n",
               output->data[0].sensor_type,
               output->data[0].distance,
               output->data[0].time_stamp);
#endif
            append_data(4,
                               output->data[0].sensor_type,
                               output->data[0].distance,
                               output->data[0].time_stamp,
                               9,
                               9,
                               9);
            break;

        case SENSOR_TYPE_HEART_RATE_MONITOR:
            #if 1
                SENSOR_DEMO_LOGI("Heart rate , bpm = %ld , timestampe = %lu \r\n",
                   output->data[0].heart_rate_t.bpm,
                   output->data[0].time_stamp);

            #ifdef SENSOR_BTSPP
                send_sensor_data_via_btspp(
                                   SENSOR_SPP_DATA_MAGIC,
                                   SENSOR_TYPE_HEART_RATE_MONITOR-1,
                                   output->data[0].heart_rate_t.bpm,
                                   output->data[0].heart_rate_t.status,
                                   output->data[0].time_stamp,
                                   SENSOR_SPP_DATA_RESERVED,
                                   SENSOR_SPP_DATA_RESERVED);
            #endif
                //SENSOR_DEMO_LOGI("Out update_hr_data, value = (%ld)\r\n", output->data[0].heart_rate_t.bpm);
                update_hr_data(output->data[0].heart_rate_t.bpm);

            #endif
            break;

        case SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR:
        {
            SENSOR_DEMO_LOGI("Heart rate variability: T(%ld), SDNN(%ld), LF(%ld), HF(%ld), LF_HF(%ld)\r\n", output->data[0].time_stamp, output->data[0].heart_rate_variability_t.SDNN,
                output->data[0].heart_rate_variability_t.LF, output->data[0].heart_rate_variability_t.HF, output->data[0].heart_rate_variability_t.LF_HF);

            #ifdef SENSOR_BTSPP
            send_sensor_data_via_btspp(
                               SENSOR_SPP_DATA_MAGIC,
                               SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR-1,
                               output->data[0].heart_rate_variability_t.SDNN,
                               output->data[0].heart_rate_variability_t.LF,
                               output->data[0].heart_rate_variability_t.HF,
                               output->data[0].heart_rate_variability_t.LF_HF,
                               output->data[0].time_stamp
                               );
            #endif

            update_hrv_data(
                    output->data[0].heart_rate_variability_t.SDNN,
                    output->data[0].heart_rate_variability_t.LF,
                    output->data[0].heart_rate_variability_t.HF,
                    output->data[0].heart_rate_variability_t.LF_HF);

            /* To unsubscribe here? */
#ifdef FUSION_HEART_RATE_VARIABILITY_USE
            int32_t unsubscription;

            unsubscription = sensor_unsubscribe_sensor(heart_rate_variability_subscriber.handle);
            if (unsubscription < 0) {
                SENSOR_DEMO_LOGI("HRV unsubscription fail\r\n");
            } else {
                hrv_triggered = 0;
                SENSOR_DEMO_LOGI("HRV unsubscribed\r\n");
            }
#endif
            break;
        }

        case SENSOR_TYPE_BLOOD_PRESSURE_MONITOR:
        {
            SENSOR_DEMO_LOGI("B.P: T(%ld), sdp(%ld), dbp(%ld), status(%ld) \r\n", output->data[0].time_stamp, output->data[0].blood_pressure_t.sbp,
                output->data[0].blood_pressure_t.dbp, output->data[0].blood_pressure_t.status);

            update_bp_data(
                    output->data[0].blood_pressure_t.sbp,
                    output->data[0].blood_pressure_t.dbp);

            /* To unsubscribe here? */
#ifdef FUSION_BLOOD_PRESSURE_USE
            int32_t unsubscription;

            unsubscription = sensor_unsubscribe_sensor(blood_pressure_subscriber.handle);
            if (unsubscription < 0) {
                SENSOR_DEMO_LOGI("B.P. unsubscription fail\r\n");
            } else {
                bp_triggered = 0;
                SENSOR_DEMO_LOGI("B.P. unsubscribed\r\n");
            }
#endif

#ifdef FUSION_HEART_RATE_MONITOR_USE
            int32_t subscription;

            if (hr_triggered == 2) {
                subscription = sensor_subscribe_sensor(&heart_rate_monitor_subscriber);
                if (subscription < 0) {
                    SENSOR_DEMO_LOGI("HR subscription fail\r\n");
                } else {
                    hr_triggered = 1;
                    SENSOR_DEMO_LOGI("HR subscribed\r\n");
                }
            }
#endif

            break;
        }

        default:
            SENSOR_DEMO_LOGI("type = %lu , value = ( %ld %ld %ld ) , timestamp = %lu \r\n",
                   output->data[0].sensor_type,
                   output->data[0].value[0],
                   output->data[0].value[1],
                   output->data[0].value[2],
                   output->data[0].time_stamp);
            break;
    }

    return 0;
}

void enable_all_sensors(void)
{
    int32_t subscription = 0;

    (void)subscription;

#ifdef FUSION_PEDOMETER_USE
    subscription = sensor_subscribe_sensor(&pedometer_subscriber);
    if (subscription < 0) {
        SENSOR_DEMO_LOGI("Pedometer subscription fail\r\b");
    }
#endif

#ifdef FUSION_SLEEP_TRACKER_USE
    subscription = sensor_subscribe_sensor(&sleep_tracker_subscriber);
    if (subscription < 0) {
        SENSOR_DEMO_LOGI("Sleep tracker subscription fail\r\b");
    }
#endif

#ifdef FUSION_HEART_RATE_MONITOR_USE
    if (hr_triggered == 0) {
        subscription = sensor_subscribe_sensor(&heart_rate_monitor_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("HR subscription fail\r\n");
        }
        else {
            SENSOR_DEMO_LOGI("HR triggered\r\n");
            hr_triggered = 1;
        }
    }
#endif
}

void disable_all_sensors(void)
{
    int32_t unsubscription = 0;

    (void)unsubscription;

#ifdef FUSION_HEART_RATE_MONITOR_USE
    if (hr_triggered == 1) {
        unsubscription = sensor_unsubscribe_sensor(heart_rate_monitor_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("HR unsubscription fail\r\b");
        } else {
            hr_triggered = 0;
            SENSOR_DEMO_LOGI("HR unsubscribed\r\n");
        }
    }
#endif

#ifdef FUSION_HEART_RATE_VARIABILITY_USE
    if (hrv_triggered == 1) {
        unsubscription = sensor_unsubscribe_sensor(heart_rate_variability_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("HRV unsubscription fail\r\n");
        } else {
            hrv_triggered = 0;
            SENSOR_DEMO_LOGI("HRV unsubscribed\r\n");
        }
    }
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
    if (bp_triggered == 1) {
        unsubscription = sensor_unsubscribe_sensor(blood_pressure_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("B.P. unsubscription fail\r\n");
        } else {
            bp_triggered = 0;
            SENSOR_DEMO_LOGI("B.P. unsubscribed\r\n");
        }
    }
#endif
}

void enable_hrv(void)
{
    int32_t subscription = 0;

    (void)subscription;
#ifdef FUSION_HEART_RATE_VARIABILITY_USE
    if (hrv_triggered == 0) {
        subscription = sensor_subscribe_sensor(&heart_rate_variability_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("Heart rate variability subscription fail\r\n");
        }
        else {
            SENSOR_DEMO_LOGI("Heart rate variability triggered\r\n");
            hrv_triggered = 1;
        }
    }
#endif
}

void enable_bp(void)
{
    int32_t subscription = 0;

    (void)subscription;

    /* B.P. usage is exclusive with Heart rate*/

#ifdef FUSION_HEART_RATE_MONITOR_USE
    if (hr_triggered == 1) {
        subscription = sensor_unsubscribe_sensor(heart_rate_monitor_subscriber.handle);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("HR unsubscription fail\r\b");
        } else {
            hr_triggered = 2;
            SENSOR_DEMO_LOGI("HR unsubscribed\r\n");
        }
    }
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
    if (bp_triggered == 0) {
        subscription = sensor_subscribe_sensor(&blood_pressure_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("B.P. subscription fail\r\n");
        }
        else {
            SENSOR_DEMO_LOGI("B.P. triggered\r\n");
            bp_triggered = 1;
        }
    }
#endif
}

#else
void enable_all_sensors(void)
{

}
void disable_all_sensors(void)
{

}
#endif /*SENSOR_DEMO*/

