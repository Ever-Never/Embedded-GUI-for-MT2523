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

#include "bmp280.h"
#include "sensor_alg_interface.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"

#include "mems_bus.h"
#include "memory_attribute.h"

/* syslog */
#define LOGI(fmt,arg...)   LOG_I(sensor, "BMP280: "fmt,##arg)
#define LOGE(fmt,arg...)   LOG_E(sensor, "BMP280: "fmt,##arg)
/* printf*/
//#define LOGI(fmt,arg...)   printf("BMP280:INFO: "fmt,##arg)
//#define LOGE(fmt,arg...)   printf("BMP280:ERROR: "fmt,##arg)


#define MAX_WRITE_LENGTH 4

static struct bmp280_t t_bmp280;

uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) bmp280_txbuffer[MAX_WRITE_LENGTH*2];

static int32_t bmp280_I2C_multi_write(uint8_t addr, uint8_t *value, uint16_t len)
{
    uint8_t reg_addr = addr;
    uint8_t reg_len = len;
    uint8_t *ptxbuffer = bmp280_txbuffer;
    int32_t ret;

    if (len > MAX_WRITE_LENGTH) {
        LOGE("I2C Max Write length exceeds!\n");
        return -1;
    }

    while (reg_len != 0) {
        // Register Address
        *ptxbuffer = reg_addr;
        reg_addr++;
        ptxbuffer++;

        // Register Data
        *ptxbuffer = *value;
        value++;
        ptxbuffer++;

        reg_len--;
    }

    ret = mems_i2c_write(BMP280_I2C_ADDRESS, bmp280_txbuffer, (len * 2));

    if ( HAL_I2C_STATUS_OK != ret) {
        LOGE("I2C Trasmit error(%ld)\n", ret);
        return -1;
    }
    return ret;
}

uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) bmp280_reg_addr;

static int32_t bmp280_I2C_multi_read(uint8_t addr, uint8_t *result, uint16_t len)
{
    int32_t ret;
    bmp280_reg_addr = addr;

    ret = mems_i2c_write(BMP280_I2C_ADDRESS, &bmp280_reg_addr, 1);
    if ( HAL_I2C_STATUS_OK != ret) {
        LOGE("I2C Trasmit error (%ld)\n", ret);
        return -1;
    }

    ret = mems_i2c_read(BMP280_I2C_ADDRESS, result, len);
    if ( HAL_I2C_STATUS_OK != ret) {
        LOGE("I2C Receive error (%ld)\n", ret);
        return -1;
    }

    return ret;
}

uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) bmp280_a_data_u8r[26];

/*******************************************************************************
 * Description: *//**\brief reads calibration parameters used for calculation
 *
 *
 *
 *
 *  \param  None
 *
 *
 *
 *  \return
 *
 ******************************************************************************/
int32_t bmp280_get_calib_param()
{
    int32_t comres = 0;

    comres += bmp280_I2C_multi_read(BMP280_DIG_T1_LSB_REG, bmp280_a_data_u8r, 24);

    t_bmp280.cal_param.dig_T1 = (uint16_t)(((\
                                            (uint16_t)((unsigned char)bmp280_a_data_u8r[1])) << 8) | bmp280_a_data_u8r[0]);
    t_bmp280.cal_param.dig_T2 = (int16_t)(((\
                                            (int16_t)((signed char)bmp280_a_data_u8r[3])) << 8) | bmp280_a_data_u8r[2]);
    t_bmp280.cal_param.dig_T3 = (int16_t)(((\
                                            (int16_t)((signed char)bmp280_a_data_u8r[5])) << 8) | bmp280_a_data_u8r[4]);
    t_bmp280.cal_param.dig_P1 = (uint16_t)(((\
                                            (uint16_t)((unsigned char)bmp280_a_data_u8r[7])) << 8) | bmp280_a_data_u8r[6]);
    t_bmp280.cal_param.dig_P2 = (int16_t)(((\
                                            (int16_t)((signed char)bmp280_a_data_u8r[9])) << 8) | bmp280_a_data_u8r[8]);
    t_bmp280.cal_param.dig_P3 = (int16_t)(((\
                                            (int16_t)((signed char)bmp280_a_data_u8r[11])) << 	8) | bmp280_a_data_u8r[10]);
    t_bmp280.cal_param.dig_P4 = (int16_t)(((\
                                            (int16_t)((signed char)bmp280_a_data_u8r[13])) << 	8) | bmp280_a_data_u8r[12]);
    t_bmp280.cal_param.dig_P5 = (int16_t)(((\
                                            (int16_t)((signed char)bmp280_a_data_u8r[15])) << 	8) | bmp280_a_data_u8r[14]);
    t_bmp280.cal_param.dig_P6 = (int16_t)(((\
                                            (int16_t)((signed char)bmp280_a_data_u8r[17])) << 	8) | bmp280_a_data_u8r[16]);
    t_bmp280.cal_param.dig_P7 = (int16_t)(((\
                                            (int16_t)((signed char)bmp280_a_data_u8r[19])) << 	8) | bmp280_a_data_u8r[18]);
    t_bmp280.cal_param.dig_P8 = (int16_t)(((\
                                            (int16_t)((signed char)bmp280_a_data_u8r[21])) << 	8) | bmp280_a_data_u8r[20]);
    t_bmp280.cal_param.dig_P9 = (int16_t)(((\
                                            (int16_t)((signed char)bmp280_a_data_u8r[23])) << 	8) | bmp280_a_data_u8r[22]);

    LOGI("Trimming parameter: T1(%u) \n\t T2(%d) \n\t T3(%d)\
        \n\t P1(%u) \n\t P2(%d) \n\t P3(%d) \n\t P4(%d) \n\t P5(%d)\
        \n\t P6(%d) \n\t P7(%d) \n\t P8(%d) \n\t P9(%d)\n", t_bmp280.cal_param.dig_T1,
         t_bmp280.cal_param.dig_T2, t_bmp280.cal_param.dig_T3,
         t_bmp280.cal_param.dig_P1, t_bmp280.cal_param.dig_P2,
         t_bmp280.cal_param.dig_P3, t_bmp280.cal_param.dig_P4,
         t_bmp280.cal_param.dig_P5, t_bmp280.cal_param.dig_P6,
         t_bmp280.cal_param.dig_P7, t_bmp280.cal_param.dig_P8,
         t_bmp280.cal_param.dig_P9);

    return comres;
}

/*******************************************************************************
 * Description: *//**\brief reads uncompensated temperature
 *
 *
 *
 * \param signed long utemperature : Pointer holding
 *                            the uncompensated temperature.
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
int32_t bmp280_read_ut(int32_t *utemperature)
{
    int32_t comres = 0;

    comres += bmp280_I2C_multi_read(BMP280_TEMPERATURE_MSB_REG, bmp280_a_data_u8r, 3);

    *utemperature = (int32_t)((( \
                                 (int32_t) (bmp280_a_data_u8r[0])) << 12) | \
                              (((int32_t)(bmp280_a_data_u8r[1])) << 4) \
                              | ((int32_t)bmp280_a_data_u8r[2] >> 4));

    return comres;
}

/*******************************************************************************
 * Description: *//**\brief Reads actual temperature from uncompensated temperature
 *                    and returns the value in 0.01 degree Centigrade
 *                    Output value of "5123" equals 51.23 DegC.
 *
 *
 *
 *  \param signed long : value of uncompensated temperature.
 *
 *
 *
 *  \return
 *   signed long : actual temperature
 *
 ******************************************************************************/
int32_t bmp280_compensate_T_int32(int32_t adc_T)
{
    int32_t v_x1_u32r = 0;
    int32_t v_x2_u32r = 0;
    int32_t temperature = 0;

    v_x1_u32r  = ((((adc_T >> 3) - ((int32_t)
                                    t_bmp280.cal_param.dig_T1 << 1))) * \
                  ((int32_t)t_bmp280.cal_param.dig_T2)) >> 11;
    v_x2_u32r  = (((((adc_T >> 4) - \
                     ((int32_t)t_bmp280.cal_param.dig_T1)) * ((adc_T >> 4) - \
                             ((int32_t)t_bmp280.cal_param.dig_T1))) >> 12) * \
                  ((int32_t)t_bmp280.cal_param.dig_T3)) >> 14;
    t_bmp280.cal_param.t_fine = v_x1_u32r + v_x2_u32r;
    temperature  = (t_bmp280.cal_param.t_fine * 5 + 128) >> 8;

    return temperature;
}

/*******************************************************************************
 * Description: *//**\brief reads uncompensated pressure.
 *
 *
 *
 *
 *  \param signed long upressure : Pointer holding the uncompensated pressure.
 *
 *
 *
 *  \return
 *
 ******************************************************************************/
int32_t bmp280_read_up(int32_t *upressure)
{
    int32_t comres = 0;

    comres += bmp280_I2C_multi_read(BMP280_PRESSURE_MSB_REG, bmp280_a_data_u8r, 3);
    *upressure = (int32_t)((((int32_t)(bmp280_a_data_u8r[0])) \
                            << 12) | (((int32_t)(bmp280_a_data_u8r[1])) \
                                      << 4) | ((int32_t)bmp280_a_data_u8r[2] >> \
                                              4));

    return comres;
}

/*******************************************************************************
 * Description: *//**\brief Reads actual pressure from uncompensated pressure
 *                          and returns the value in Pascal(Pa)
 *                          Output value of "96386" equals 96386 Pa =
 *                          963.86 hPa = 963.86 millibar

 *
 *
 *
 *  \param signed long : value of uncompensated pressure
 *
 *
 *
 *  \return
 *   unsigned long : actual pressure
 *
 ******************************************************************************/
uint32_t bmp280_compensate_P_int32(int32_t adc_P)
{
    int32_t v_x1_u32r = 0;
    int32_t v_x2_u32r = 0;
    uint32_t pressure = 0;

    v_x1_u32r = (((int32_t)t_bmp280.cal_param.t_fine) >> 1) - \
                (int32_t)64000;
    v_x2_u32r = (((v_x1_u32r >> 2) * (v_x1_u32r >> 2)) >> 11) * \
                ((int32_t)t_bmp280.cal_param.dig_P6);
    v_x2_u32r = v_x2_u32r + ((v_x1_u32r * \
                              ((int32_t)t_bmp280.cal_param.dig_P5)) << 1);
    v_x2_u32r = (v_x2_u32r >> 2) + \
                (((int32_t)t_bmp280.cal_param.dig_P4) << 16);
    v_x1_u32r = (((t_bmp280.cal_param.dig_P3 * (((v_x1_u32r >> 2) * \
                   (v_x1_u32r >> 2)) >> 13)) >> 3) + \
                 ((((int32_t)t_bmp280.cal_param.dig_P2) * \
                   v_x1_u32r) >> 1)) >> 18;
    v_x1_u32r = ((((32768 + v_x1_u32r)) * \
                  ((int32_t)t_bmp280.cal_param.dig_P1))	>> 15);
    if (v_x1_u32r == 0) {
        return 0;    /* Avoid exception caused by division by zero */
    }
    pressure = (((uint32_t)(((int32_t)1048576) - adc_P) - \
                 (v_x2_u32r >> 12))) * 3125;
    if (pressure < 0x80000000) {
        pressure = (pressure << 1) / ((uint32_t)v_x1_u32r);
    } else {
        pressure = (pressure / (uint32_t)v_x1_u32r) * 2;
    }
    v_x1_u32r = (((int32_t)t_bmp280.cal_param.dig_P9) * \
                 ((int32_t)(((pressure >> 3) * (pressure >> 3)) >> 13))) \
                >> 12;
    v_x2_u32r = (((int32_t)(pressure >> 2)) * \
                 ((int32_t)t_bmp280.cal_param.dig_P8)) >> 13;
    pressure = (uint32_t)((int32_t)pressure + \
                          ((v_x1_u32r + v_x2_u32r + t_bmp280.cal_param.dig_P7) >> 4));

    return pressure;
}

uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) bmp280_reg_value;

/*--------------------------------------------------------------------------------*/
int32_t baros_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                      void *buff_in, int32_t size_in)
{
    int err = 0;
    int value = 0;
    uint8_t reg_value = 0;
    int32_t ret;

    switch (command) {
        case SENSOR_DELAY:
            // Need it?
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("ACC Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int *)buff_in;
                LOGI("Baro SENSOR_DELAY ms (%d) \r\n", value);
            }
            break;
        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("Enable sensor parameter error! \r\n");
                err = -1;
            } else {
                value = *(int *)buff_in;
                LOGI("BMP280 SENSOR_ENABLE (%d) \r\n", value);
                if (value) {
                    // CTRL_MEAS, High resolution
                    reg_value = (t_bmp280.osrs_t << BMP280_CTRLMEAS_REG_OSRST__POS) |
                                (t_bmp280.osrs_p << BMP280_CTRLMEAS_REG_OSRSP__POS) |
                                POWER_NORMAL_MODE;

                    ret = bmp280_I2C_multi_write(BMP280_CTRLMEAS_REG, &reg_value, 1);
                    if (ret < 0) {
                        LOGE("I2C write error\n");
                    }

                    reg_value = (t_bmp280.t_standby << BMP280_CONFIG_REG_TSB__POS) |
                                (t_bmp280.filter_coef << BMP280_CONFIG_REG_FILTER__POS);

                    ret = bmp280_I2C_multi_write(BMP280_CONFIG_REG, &reg_value, 1);
                    if (ret < 0) {
                        LOGE("I2C write error\n");
                    }

                    bmp280_get_calib_param(); /* readout bmp280 calibparam structure */

                } else {
                    ret = bmp280_I2C_multi_read(BMP280_CTRLMEAS_REG, &bmp280_reg_value, 1);
                    if (ret < 0) {
                        LOGE("I2C write error\n");
                    }
                    reg_value = bmp280_reg_value;
                    reg_value = (reg_value & (~BMP280_CTRLMEAS_REG_MODE__MSK)) | POWER_SLEEP_MODE;
                    ret = bmp280_I2C_multi_write(BMP280_CTRLMEAS_REG, &reg_value, 1);
                    if (ret < 0) {
                        LOGE("I2C write error\n");
                    }
                }
            }

            break;
        case SENSOR_GET_DATA:
            if ((buff_out == NULL) || size_out != sizeof(sensor_data_unit_t)) {
                LOGE("SENSOR_GET_DATA parameter error \r\n");
            } else {
                int32_t utemperature;
                int32_t upressure;

                *actualout = sizeof(sensor_data_unit_t);
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                err = bmp280_read_ut(&utemperature);
                if (err < 0) {
                    *actualout = 0;
                    break;
                }
                start->pressure_t.temperature = bmp280_compensate_T_int32(utemperature);
                err = bmp280_read_up(&upressure);
                if (err < 0) {
                    *actualout = 0;
                    break;
                }
                start->pressure_t.pressure = bmp280_compensate_P_int32(upressure);
            }
            break;

        case SENSOR_CUST:
            break;
        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

/*----------------------------------------------------------------------------*/
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) bmp280_chip_id;

void baros_init(void)
{
    sensor_driver_object_t obj_baros;

    t_bmp280.chip_id = 0x58;
    t_bmp280.osrs_p = P_OVERSAMPLING_X8;
    t_bmp280.osrs_t = T_OVERSAMPLING_X1;
    t_bmp280.t_standby = 0; //0.5 ms
    t_bmp280.filter_coef = CONF_FILTER_COEF4;

    int fail_cout = 0;
    while (bmp280_chip_id != t_bmp280.chip_id) {
        /* read Chip Id */
        bmp280_I2C_multi_read(BMP280_CHIPID_REG, &bmp280_chip_id, 1);
        LOGI("chip id == 0x%X\n", bmp280_chip_id);
        if (bmp280_chip_id == t_bmp280.chip_id) {
            break;
        }
        fail_cout++;
        if(fail_cout > 5) {
            return;
        }
        vTaskDelay(1000);
    }

    obj_baros.self = (void *)&obj_baros;
    obj_baros.polling = 1;
    obj_baros.sensor_operate = baros_operate;

    sensor_driver_attach(SENSOR_TYPE_PRESSURE, &obj_baros);
}

