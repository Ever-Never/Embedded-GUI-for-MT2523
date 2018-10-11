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

// For Register AT command handler
// System head file
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include "at_command.h"
#include "vsm_driver.h"
#include "syslog.h"
#include "hal_gpio.h"
#include "semphr.h"
#include "hal_gpt.h"
#include "task_def.h"

#ifdef MTK_BUILD_SMT_LOAD 
#define MT2511_SMT_MODE
#endif
//#define MT2511_SMT_MODE
#ifdef MT2511_SMT_MODE
#include "kiss_fft.h"
#endif

//#define BIO_TUNNING_TOOL_USE

log_create_module(atci_biogui, PRINT_LEVEL_INFO);
#if 1
#define LOGE(fmt,arg...)   LOG_E(atci_biogui, "[biogui]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci_biogui, "[biogui]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci_biogui ,"[biogui]"fmt,##arg)
#else
#define LOGE(fmt,arg...)   printf("[biogui]"fmt,##arg)
#define LOGW(fmt,arg...)   printf("[biogui]"fmt,##arg)
#define LOGI(fmt,arg...)   printf("[biogui]"fmt,##arg)
#endif

#define DATA_PAGE       100
#define EKG_DATA_PAGE   50
#define BISI_SRAM_LEN   256
#define LAST_ITEM_SLAVE1  0x23AC
#define LAST_ITEM_SLAVE2  0x33FC
#define FIRST_ITEM_SLAVE0 0x3300
#define CHIP_VERSION_ADDR 0x23AC
#define EKG_IRQ_TH_REG    0x33CC
#define PPG1_IRQ_TH_REG   0x33DC
#define PPG2_IRQ_TH_REG   0x33EC
#define BISI_IRQ_TH_REG   0x33FC

//extern gpio var by name
extern const char BSP_BIO_SENSOR_PPG_VDRV_EN;
extern const char BSP_BIO_SENSOR_32K;
extern const char BSP_BIO_SENSOR_AFE_PWD_PIN;
extern const char BSP_BIO_SENSOR_RST_PORT_PIN;
#define GPIO_MT2511_PPG_VDRV_EN     (hal_gpio_pin_t)BSP_BIO_SENSOR_PPG_VDRV_EN
#define GPIO_MT2511_32K             (hal_gpio_pin_t)BSP_BIO_SENSOR_32K
#define GPIO_MT2511_AFE_PWD_PIN     (hal_gpio_pin_t)BSP_BIO_SENSOR_AFE_PWD_PIN
#define GPIO_MT2511_RST_PORT_PIN    (hal_gpio_pin_t)BSP_BIO_SENSOR_RST_PORT_PIN

#ifdef MT2511_SMT_MODE
#define EKG_SAMPLE      (16384)
#define PPG_SAMPLE      (4096)
#define EKG_VPP         (4)       //unit: bits
#define EKG_BITS        (23)      //unit: bits
//#define EKG_ADC_LSB     (1/(1<<21))  //4/2^23
#define EKG_ADC_LSB     (2097152)  //4/2^23
#define EKG_GAIN        (6)

#define PPG_VPP         (3.2f)   //unit: volt
#define PPG_BITS        (16)    //unit: bits
#define PPG_ADC_LSB     (1<<PPG_BITS)//(PPG_VPP/(1<<PPG_BITS))
#define PPG_TIA_GAIN    (1)
#define PPG_PGA_GAIN    (1)

#define EKG_SFREQ       (512)
#define PPG_SFREQ       (124.89f)
#define EKG_BW          (150)
#define PPG_BW          (4)
#define EKG_SUM_START   (0)
#define EKG_SUM_END     (4800) //Nfout=floor(sfout/sfreq*NFFT);
#define PPG_SUM_START   (0)
#define PPG_SUM_END     (131)  //Nfout=floor(sfout/sfreq*NFFT);

//#define PPG_128HZ
//#define AC_SCAN_DUMP_RAW_DATA
//#define LED_DAC_EXPANDSION

#ifdef AC_SCAN_DUMP_RAW_DATA
#define AC_SCAN_RAW_AMB_DATA
#endif

#ifndef PPG_128HZ
#define PPG_SAMPLE_RATE             (125)
#else
#define PPG_SAMPLE_RATE             (128)
#endif

#define PPG_SAMPLE_RUN_SEC          (6)

#ifdef LED_DAC_EXPANDSION
#define PPG_SAMPLE_RUN_LED_PHASE    (16)
#else
#define PPG_SAMPLE_RUN_LED_PHASE    (8)
#endif

#define PPG_SAMPLE_RUN_AMB_PHASE    (6)

#define LED_CUR_TEST_LEN    (PPG_SAMPLE_RATE * PPG_SAMPLE_RUN_SEC * PPG_SAMPLE_RUN_LED_PHASE) 
#define AMBDAC_CUR_TEST_LEN (PPG_SAMPLE_RATE * PPG_SAMPLE_RUN_SEC * PPG_SAMPLE_RUN_LED_PHASE * PPG_SAMPLE_RUN_AMB_PHASE)

#define TIA_GAIN_100K       (1)
#define TIA_GAIN_500K       (2)
#define LED_CUR_TEST        (1)
#define LED_AMBDAC_CUR_TEST (2)
#endif


typedef struct
{
	uint16_t addr;
	uint32_t value;
} signal_data_t;

typedef enum
{
    TYPE_PPG1 = 0,
    TYPE_PPG2 = 1,
    TYPE_EKG  = 2,
    TYPE_BISI = 3
} type_t;

typedef struct
{
    int16_t type;
    int32_t sample;
} task_para_t;


bool PPG1 = false, PPG2 = false, EKG = false, BISI = false;
bool g_hw_init = false;

TaskHandle_t  atci_biogui_task_handle = NULL;
QueueHandle_t g_biogui_handler_semaphore = NULL;
task_para_t task_para;

uint32_t read_data[VSM_SRAM_LEN];
#ifdef MT2511_SMT_MODE
kiss_fft_cpx fft_data_in[EKG_SAMPLE];
static kiss_fft_cfg cfg;
kiss_fft_cpx fft_data_out[EKG_SAMPLE];
float led_data[AMBDAC_CUR_TEST_LEN];
#ifdef AC_SCAN_DUMP_RAW_DATA
#ifdef AC_SCAN_RAW_AMB_DATA
uint32_t raw_data[AMBDAC_CUR_TEST_LEN*2];
#else
uint32_t raw_data[AMBDAC_CUR_TEST_LEN];
#endif
#endif

#endif

int32_t ekg_mode = 0;
/*
 * sample code
*/


/*--- Function ---*/
extern int32_t vsm_driver_chip_version_get(void);
extern void vsm_data_drop(vsm_sram_type_t sram_type);
extern vsm_signal_t vsm_driver_reset_PPG_counter(void);
extern vsm_status_t vsm_driver_set_led_current_range(uint32_t cur_range);
extern vsm_status_t vsm_driver_set_tia_cf(vsm_led_type_t led_type, uint32_t input);
#ifdef MT2511_USE_SPI
void vsm_spi_speed_modify(int32_t speed);
#endif

void set_tia_gain(int32_t tia_gain)
{
    if (tia_gain == 1) {
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG1, VSM_TIA_GAIN_10_K);
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG2, VSM_TIA_GAIN_10_K);
    } else if (tia_gain == 2) {
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG1, VSM_TIA_GAIN_25_K);
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG2, VSM_TIA_GAIN_25_K);
    } else if (tia_gain == 3) {
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG1, VSM_TIA_GAIN_50_K);
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG2, VSM_TIA_GAIN_50_K);
    } else if (tia_gain == 4) {
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG1, VSM_TIA_GAIN_100_K);
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG2, VSM_TIA_GAIN_100_K);
    } else if (tia_gain == 5) {
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG1, VSM_TIA_GAIN_250_K);
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG2, VSM_TIA_GAIN_250_K);
    } else if (tia_gain == 6) {
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG1, VSM_TIA_GAIN_500_K);
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG2, VSM_TIA_GAIN_500_K);
    } else if (tia_gain == 7) {
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG1, VSM_TIA_GAIN_1000_K);
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG2, VSM_TIA_GAIN_1000_K);
    }
}

void set_tia_cf(uint32_t tia_cf)
{
    vsm_driver_set_tia_cf(VSM_LED_1,(tia_cf & 0x1F));
    vsm_driver_set_tia_cf(VSM_LED_2,(tia_cf & 0x1F));
}

void set_led_cur_range(int32_t current_range)
{
    if (current_range == 1) {
        vsm_driver_set_led_current_range(VSM_LED_CUR_RANGE_9_DOT_5_MA);
    } else if (current_range == 2) {
        vsm_driver_set_led_current_range(VSM_LED_CUR_RANGE_22_DOT_9_MA);
    } else if (current_range == 3) {
        vsm_driver_set_led_current_range(VSM_LED_CUR_RANGE_36_DOT_3_MA);
    } else if (current_range == 4) {
        vsm_driver_set_led_current_range(VSM_LED_CUR_RANGE_49_DOT_7_MA);
    } else if (current_range == 5) {
        vsm_driver_set_led_current_range(VSM_LED_CUR_RANGE_63_DOT_1_MA);
    } else if (current_range == 6) {
        vsm_driver_set_led_current_range(VSM_LED_CUR_RANGE_76_DOT_5_MA);
    } else if (current_range == 7) {
        vsm_driver_set_led_current_range(VSM_LED_CUR_RANGE_89_DOT_9_MA);
    } else if (current_range == 8) {
        vsm_driver_set_led_current_range(VSM_LED_CUR_RANGE_103_DOT_3_MA);
    }
}

void set_ambdac(uint32_t ambdac)
{
    if (ambdac == 1) {  
        vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_02_MA);
        vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_02_MA);
    } else if (ambdac == 2) {
        vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_03_MA);
        vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_03_MA);
    } else if (ambdac == 3) {
        vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_04_MA);
        vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_04_MA);
    } else if (ambdac == 4) {
        vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_05_MA);
        vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_05_MA);
    } else if (ambdac == 5) {
        vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
        vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_06_MA);
    } 

}

#ifdef MT2511_SMT_MODE
void mean(kiss_fft_cpx *input, double *output, int32_t len)
{
    int i = 0;
    double sum = 0;
    if(!input || !output)
        return;
    for(i = 0; i < len; i++){
        sum += input->r;
        input++;
    }
    LOGI("mean():sum/len %.12f \r\n", sum/len);
    *output =(float) sum/len;
}

void atci_clover_task_dump(void *pvParameters)
{
    int i = 0, data_len = 0, type;
    atci_response_t response = {{0}};
    type = *(int *)pvParameters;
#ifdef AC_SCAN_DUMP_RAW_DATA
    int seq_num = 0;
#endif

    if (type == 7) {
        data_len = LED_CUR_TEST_LEN;
    } else if (type == 8) {
        data_len = AMBDAC_CUR_TEST_LEN;
    }
    #ifdef AC_SCAN_RAW_AMB_DATA
    data_len *=2;
    #endif

    LOGI("%s():type %d, data_len %ld\r\n", __func__, type, data_len);
    for (i = 0;i < data_len;i ++) {
        //catch led data
        if (i != 0 && (i % 12) == 11) {
#ifndef AC_SCAN_DUMP_RAW_DATA
            snprintf((char *)response.response_buf, sizeof(response.response_buf), "led_data[%d] %.12f\r\n", i, (double)led_data[i]);
#else
            snprintf((char *)response.response_buf, sizeof(response.response_buf), 
                    //sensor_type, seq_number, data1~data12
                    #ifndef AC_SCAN_RAW_AMB_DATA
                    "9478,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,12345\r\n", seq_num++,
                    #else
                    "9,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,12345\r\n", seq_num++, 
                    #endif
                    (uint32_t)raw_data[i - 11],(uint32_t)raw_data[i - 10],(uint32_t)raw_data[i - 9],(uint32_t)raw_data[i - 8],
                    (uint32_t)raw_data[i - 7], (uint32_t)raw_data[i - 6], (uint32_t)raw_data[i - 5],(uint32_t)raw_data[i - 4],
                    (uint32_t)raw_data[i - 3], (uint32_t)raw_data[i - 2], (uint32_t)raw_data[i - 1],(uint32_t)raw_data[i]);
#endif
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            vTaskDelay(15);
        }
    }

    vTaskDelete(NULL);
}
extern void vsm_revert_ppg_data(uint32_t *data_buf, int32_t len);
/**
  * @ recording PPG data while switching led current
  * @ test_type: select tia gain, 1: led current test, 2: led current test on different ambdac
  * @ tia_gain:  select tia gain, 1: set to 100K, 2: set to 500K.
  */
int32_t current_test(int32_t test_type, int32_t tia_gain, int32_t tia_cf, uint32_t current_range)
{
    int32_t data_count = 0, i = 0, output_len = 0;
    int32_t led_count = 0;
    uint32_t ambdac_count = 1;
    int32_t err = -1;
    int32_t data_len = 0;
    int32_t output_zero_cnt = 0, ambdac_change = 0;
#ifdef AC_SCAN_DUMP_RAW_DATA
    int32_t full_idx =0
#endif

    if (test_type == LED_CUR_TEST) {
        data_len = LED_CUR_TEST_LEN; /* collecting 125 * 4 sec * 8 led current round data */
    } else if (test_type == LED_AMBDAC_CUR_TEST) {
        data_len = AMBDAC_CUR_TEST_LEN; /* collecting 125 * 4 sec * 8 led current round data under 6 ambdac phase */
    }

    /* initialize vsm */
    vsm_driver_init();

    vsm_driver_set_signal(VSM_SIGNAL_PPG1);

    #ifdef BIO_TUNNING_TOOL_USE
    vsm_driver_enable_signal(VSM_SIGNAL_PPG1);
    #endif
    vsm_driver_disable_signal(VSM_SIGNAL_PPG1);

    /* step 1 set tia_gain */
    set_tia_gain(tia_gain);

    /* step 2: pga gain default: 6*/
    vsm_driver_set_pga_gain(VSM_PGA_GAIN_6);

    /* step 3: ambdac 1/2 set default 0*/
    if (vsm_driver_chip_version_get() == 1) {
        vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_00_MA);
    } else if (vsm_driver_chip_version_get() == 2) {
        vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_01_MA);
        vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_01_MA);
    }

    /* step 4: start from LED1/LED2 = 0/0 */
    vsm_driver_set_led_current(VSM_LED_1, led_count);
    vsm_driver_set_led_current(VSM_LED_2, led_count);

    /* step 5: set tia cf*/
    set_tia_cf(tia_cf);

    /* step 6: set full-scale range of led current*/
    vsm_driver_set_led_current_range(current_range);

    vsm_driver_enable_signal(VSM_SIGNAL_PPG1);
    err = vsm_driver_read_sram(VSM_SRAM_PPG1, (uint32_t *)read_data, &output_len);

    while (data_count < data_len) {
        vTaskDelay(50);
        err = vsm_driver_read_sram(VSM_SRAM_PPG1, (uint32_t *)read_data, &output_len);
        //vsm_revert_ppg_data(read_data, output_len);
        if (output_len == 0) {
            output_zero_cnt++;
            if (output_zero_cnt > 5) {
                LOGI("%s():output_zero_cnt break! err %ld\r\n", __func__, err);
                break;
            }
        } else {
            output_zero_cnt = 0;
        }
        //LOGI("%s():data_count %d,data_len %d , output_len %d\r\n", __func__, data_count, data_len, output_len);
        for (i = 0; i < output_len; i ++) {
            if (ambdac_change != 0 && ambdac_change-- != 0) {
                break;
            }

            if(read_data[i] > 4194304) {
                read_data[i] -= 8388608;
            }

            if (test_type == LED_CUR_TEST) {
                //keep led current setting and add ambdac current 1
                //if (i % 2 == 0 && read_data[i] > 30240) {
                if (i % 2 == 0 && (int)(read_data[i]) > 29000) {
                    ambdac_count++;
                    LOGI("%s():adjust ambdac data_count %d,ambdac_count %d \r\n", __func__, data_count, ambdac_count);
                    if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_01_MA) {
                        ambdac_change = 2;
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_01_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_01_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_01_MA);
                        }
                    } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_02_MA) {
                        ambdac_change = 2;
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_02_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_02_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_02_MA);
                        }
                    } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_03_MA) {
                        ambdac_change = 2;
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_03_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_03_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_03_MA);

                        }
                    } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_04_MA) {
                        ambdac_change = 2;
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_04_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_04_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_04_MA);

                        }
                    } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_05_MA) {
                        ambdac_change = 2;
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_05_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_05_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_05_MA);

                        }
                    } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_06_MA) {
                        ambdac_change = 2;
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_06_MA);
                        }
                    } 

                    if (vsm_driver_chip_version_get() == 1) {
                        if (VSM_AMBDAC_CURR_00_MA + ambdac_count > VSM_AMBDAC_CURR_06_MA) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                        }
                    } else if (vsm_driver_chip_version_get() == 2) {
                        if (VSM_AMBDAC_CURR_01_MA + ambdac_count > VSM_AMBDAC_CURR_06_MA) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_06_MA);
                        } 
                    }
                }

                /* collectiing led data*/
                if (i % 2 == 0) {
                    #ifdef AC_SCAN_DUMP_RAW_DATA
                    raw_data[full_idx++] = read_data[i];
                #ifdef AC_SCAN_RAW_AMB_DATA
                    raw_data[full_idx++] = read_data[i+1];
                #endif
                    #endif
                    led_data[data_count++] = (float)(((float)(int)read_data[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
                    /* collectiing engough data then break */
                    if (data_count == (data_len)) {
                        break;
                    }
                    /* adjust current while end of run*/
                    if (data_count != 0 && (data_count % (PPG_SAMPLE_RATE * PPG_SAMPLE_RUN_SEC)) == 0 ) {
                        /* 125(sample_rate) * 5 sec * 2 (led/ambient) */
                        /* switch led current setting once collecting 5 sec data */
                    #ifdef LED_DAC_EXPANDSION
                        led_count += 0x10;
                    #else
                        led_count += 0x20;
                    #endif
                        LOGI("%s():adjust led data_count %d, led_count 0x%x \r\n", __func__, data_count, led_count);
                        vsm_driver_set_led_current(VSM_LED_1,led_count);
                        vsm_driver_set_led_current(VSM_LED_2,led_count);
                        break;
                    }
                }
                if ( ambdac_change ) {
                    //ambdac_change = 0;
                    break;
                }
            } else if (test_type == LED_AMBDAC_CUR_TEST) {
                /* collectiing led data*/
                if (i % 2 == 0) {
                    #ifdef AC_SCAN_DUMP_RAW_DATA
                    raw_data[full_idx++] = read_data[i];
                #ifdef AC_SCAN_RAW_AMB_DATA
                    raw_data[full_idx++] = read_data[i+1];
                #endif
                    #endif
                    led_data[data_count++] = (float)(((float)(int)read_data[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);

                    /* collectiing engough data then break */
                    if (data_count == (data_len)) {
                        break;
                    }
                    /* adjust current while end of run*/
                    if (data_count != 0 && (data_count % (PPG_SAMPLE_RATE * PPG_SAMPLE_RUN_SEC)) == 0 ) {
                        /* 125(sample_rate) * 5 sec * 2 (led/ambient) */
                        /* switch led current setting once collecting 5 sec data */
                    #ifdef LED_DAC_EXPANDSION
                        led_count += 0x10;
                    #else
                        led_count += 0x20;
                    #endif
                        if (led_count <= 0xf0) {
                            LOGI("%s():adjust led data_count %d, led_count 0x%x \r\n", __func__, data_count, led_count);
                            vsm_driver_set_led_current(VSM_LED_1,led_count);
                            vsm_driver_set_led_current(VSM_LED_2,led_count);
                        } else {
                            /* every led current phase collecting done, change to next ambdac */
                            LOGI("data_count %ld, ambdac level %lu \r\n", data_count, ambdac_count);
                            led_count = 0;
                            ambdac_count++;
                            vsm_driver_set_led_current(VSM_LED_1,led_count);
                            vsm_driver_set_led_current(VSM_LED_2,led_count);
                            if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_01_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_01_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_01_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_01_MA);
                                }
                            } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_02_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_02_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_02_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_02_MA);
                                }
                            } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_03_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_03_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_03_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_03_MA);

                                }
                            } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_04_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_04_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_04_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_04_MA);

                                }
                            } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_05_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_05_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_05_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_05_MA);

                                }
                            } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_06_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_06_MA);
                                }
                            } 

                            if (vsm_driver_chip_version_get() == 1) {
                                if (VSM_AMBDAC_CURR_00_MA + ambdac_count > VSM_AMBDAC_CURR_06_MA) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                                }
                            } else if (vsm_driver_chip_version_get() == 2) {
                                if (VSM_AMBDAC_CURR_01_MA + ambdac_count > VSM_AMBDAC_CURR_06_MA) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_06_MA);
                                } 
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    return err;
}
/**
  * @ measure signal noise
  * @ signal: signal type.
  * @ signal_noise: pointer for the signal noise reuslt.
  * @ is_odd: measure ppg/led if true, else measue ppg/ambient.
  * @ is_amplitude: measure amplitude error if true, else measue signal noise.
  */
int32_t signal_noise_compute(vsm_signal_t signal, double *signal_noise, bool is_odd, bool is_amplitude)
{
    uint32_t i = 0, data_count = 0;
    int32_t sum = 0, output_len;
    int32_t sample_count = 0, sum_start = 0, sum_end = 0, sram_type = 0;
    int32_t drop_point = 0;
    double mean_data = 0, signal_result = 0;

    if (!signal_noise) {
        return -1;
    }

    if(signal == VSM_SIGNAL_EKG || signal == VSM_SIGNAL_PPG1) {

        if (signal == VSM_SIGNAL_EKG) {

            sum_start = EKG_SUM_START;
            sum_end = EKG_SUM_END;
            #ifndef GOLDEN_TEST
            sample_count = EKG_SAMPLE;
            sram_type = VSM_SRAM_EKG;
            #else
            sample_count = sizeof(data_ekg_input)/sizeof(data_ekg_input[0]);
            #endif
            drop_point = 512;
        } else if (signal == VSM_SIGNAL_PPG1) {
            sum_start = PPG_SUM_START;
            sum_end = PPG_SUM_END;
            #ifndef GOLDEN_TEST
            sram_type = VSM_SRAM_PPG1;
            sample_count = PPG_SAMPLE;
            #else
            sample_count = sizeof(data_ppg_even_input)/sizeof(data_ppg_even_input[0]);
            #endif
            drop_point = 128 * 2 * 30;
        }

        LOGI("prepare init \r\n");

#ifndef GOLDEN_TEST
        /* initialize vsm */
        vsm_driver_init();
        LOGI("prepare set signal %d \r\n", signal);
        /* enable ekg mode or get ppg data */
        vsm_driver_set_signal(signal);
        vsm_driver_disable_signal(signal);
        /* add other parameter here*/
        if (signal == VSM_SIGNAL_EKG) {
            bus_data_t data;
            uint32_t ekg_data;
            data.addr = 0x33;
            data.reg = 0x08;
            data.data_buf = (uint8_t *)&ekg_data;
            data.length = sizeof(uint32_t);
            if (ekg_mode == 1) {
                /*RLD mode*/
                ekg_data = 0x0001D042;
            } else if (ekg_mode == 2) {
                /*2E mode*/                
                ekg_data = 0x0001D442;
            }
            vsm_driver_write_register(&data);
            vsm_driver_update_register();
        }
        /* enable signal*/
        vsm_driver_enable_signal(signal);
#endif
        memset(fft_data_in, 0, sizeof(fft_data_in));
        memset(fft_data_out, 0, sizeof(fft_data_out));
        memset(led_data, 0, sizeof(led_data));
        //LOGI("prepare get data and convert to mV, %f\r\n", 1.2f * 1.2f);
#ifndef GOLDEN_TEST
        /* drop sample first */
        while (data_count < drop_point) {
            vsm_driver_read_sram((vsm_sram_type_t)sram_type, (uint32_t *)read_data, (int32_t *)&output_len);
            data_count += output_len;
        }
        data_count = 0;
        /*1. collect ekg 16384 point sram data to fft_data[EKG_SAMPLE] */
        while(data_count < sample_count) {
            //LOGI("data_count %d, sample_count %d\r\n", data_count, sample_count);
            vsm_driver_read_sram((vsm_sram_type_t)sram_type, (uint32_t *)read_data, (int32_t *)&output_len);
            for (i = 0; i < output_len; i ++) {
                if (data_count == sample_count) {
                    break;
                }

                //filter data by data_ppg1_buf(find(data_ppg1_buf >2^22)) = data_ppg1_buf(find(data_ppg1_buf >2^22)) - 2^23; % 2's complement
                if(read_data[i] > 4194304) {
                    read_data[i] -= 8388608;
                }
                // get sram data and convert to mV
                if (signal == VSM_SIGNAL_EKG) {
                    if ((data_count == 0 && i > 50) || (data_count > 0)) {
                        fft_data_in[data_count].i = 0.0;
                        fft_data_in[data_count].r = (double)(((double)(int)read_data[i]) * 1000 / EKG_GAIN / EKG_ADC_LSB);
                        data_count++;
                    }
                    #if 0
                    if(i < 10) {
                        printf("read_data[%d] %d\r\n", i, read_data[i]);
                    }
                    #endif
                } else if (signal == VSM_SIGNAL_PPG1 && is_odd && (i%2 == 0)) {
                    /*input ppg1 led1 raw data*/
                    if ((data_count == 0 && i > 10) || (data_count > 0)) {
                        fft_data_in[data_count].i = 0.0;
                        fft_data_in[data_count].r = (float)(((float)(int)read_data[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
                        data_count++;
                    }
                    #if 0
                    if(i < 10) {
                        printf("read_data[%d] %d\r\n", i, read_data[i]);
                    }
                    #endif
                } else if (signal == VSM_SIGNAL_PPG1 && !is_odd && (i%2 == 1)) {
                    /*input ppg1 ambient1 raw data*/
                    if ((data_count == 0 && i > 10) || (data_count > 0)) {
                        fft_data_in[data_count].i = 0.0;
                        fft_data_in[data_count].r = (float)(((float)(int)read_data[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
                        data_count++;
                    }
                    #if 0
                    if(i < 10) {
                        printf("read_data[%d] %d\r\n", i, read_data[i]);
                    }
                    #endif
                }
                #if 0
                if(i < 10) {
                    LOGI("read_data[%d] %lu\r\n", i, read_data[i]);
                }
                #endif

            }
            sum += output_len;

            // wait data if not collecting enough data
            if (data_count < sample_count) {
                vTaskDelay(100);
            }
        }

        /* amplitude computation, mV data minus reference data*/
        if (is_amplitude) {

        }
#else
        for (i = 0; i < sample_count; i ++) {
            /*1. get sram data and convert to mV */
            if (signal == VSM_SIGNAL_EKG) {
                if(data_ekg_input[i] > 4194304) {
                    data_ekg_input[i] -= 8388608;
                }
                fft_data_in[i].i = 0.0;
                fft_data_in[i].r = (double)(((double)data_ekg_input[i]) * 1000 / EKG_GAIN / EKG_ADC_LSB);
            } else if (signal == VSM_SIGNAL_PPG1 && is_odd) {
                /*input ppg1 led1 raw data*/
                if(data_ppg_odd_input[i] > 4194304) {
                    data_ppg_odd_input[i] -= 8388608;
                }
                fft_data_in[i].i = 0.0;
                fft_data_in[i].r = (float)(((float)data_ppg_odd_input[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
            }else if (signal == VSM_SIGNAL_PPG1 && !is_odd) {
                /*input ppg1 ambient1 raw data*/
                if(data_ppg_even_input[i] > 4194304) {
                    data_ppg_even_input[i] -= 8388608;
                }
                fft_data_in[i].i = 0.0;
                fft_data_in[i].r = (float)(((float)data_ppg_even_input[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
            }
            #if 0
            if (i < 10) {
                LOGI("%.12f\r\n", fft_data_in[i].r);
            }
            #endif
        }
#endif

        /*2. convert mV to noise*/
        /*2-1. sub mean*/
        mean(fft_data_in, &mean_data, sample_count);
        LOGI("prepare convert to mV, mean_data %f,%e\r\n", mean_data, mean_data);
        for( i = 0; i < sample_count; i ++) {
            #if 0
            LOGI("fft_data_in[%d].r %.12f \r\n", i, fft_data_in[i].r);
            #endif
            fft_data_in[i].r = fft_data_in[i].r - (mean_data);
            //fft_data_in[i].r = (fft_data_in[i].r - (mean_data)) * 1000;
        }

        if ((cfg = kiss_fft_alloc(sample_count, 0/*is_inverse_fft*/, NULL, NULL)) != NULL)
        {
            int i;
            //compute fft
            kiss_fft(cfg, fft_data_in, fft_data_out);
            KISS_FFT_FREE(cfg);

            #if 0
            //print part of fft results
            for (i = 0; i < 10; i++)
              LOGI(" in[%d] = %e , %e    "
                     "out[%d] = %e , %e\r\n",
                     i, fft_data_in[i].r, fft_data_in[i].i,
                     i, fft_data_out[i].r, fft_data_out[i].i);
            #endif

            //Fsn_data = fft(sn_data' .* h_hamming, NFFT) / record_size;(optional)
            //FPsn_data = Fsn_data .* conj(Fsn_data) ; %magnitude , real square + i square
            for (i = 0; i < sample_count; i ++) {
                fft_data_out[i].r /= sample_count;
                fft_data_out[i].i /= sample_count;
                led_data[i] = fft_data_out[i].r * fft_data_out[i].r + fft_data_out[i].i * fft_data_out[i].i;
                #if 0
                if (i < 10) {
                    LOGI("fft_data_out[%d] = %e , %e, square out: noise[%d]:%e\r\n",
                        i, (double)fft_data_out[i].r, (double)fft_data_out[i].i, i, (double)noise[i]);
                }
                #endif
            }
            //Nfin=floor(sfin/sfreq*NFFT);
            //Nfout=floor(sfout/sfreq*NFFT);
            //nn=sum(FPsn_data(Nfin:Nfout))^0.5*2*1000;
            //compute noise result
            *signal_noise = 0;
            for (i = sum_start; i < sum_end; i ++) {
                *signal_noise += (double)led_data[i];
                signal_result += (double)led_data[i];
            }
            LOGI("sum noise: signal_result:%.12f\r\n", signal_result);
            *signal_noise = (float)sqrt(*signal_noise)*2*1000;
            signal_result = (float)sqrt(signal_result)*2*1000;
            //signal_result = (float)sqrt(signal_result < 0?signal_result*(-1):signal_result)*2*1000;

            LOGI("noise_result %.12f , signal_result %.12f\r\n", *signal_noise, signal_result);
        } else {
            return -1;
        }
    }else {
        return -1;
    }
    return 0;
}
#endif

#ifdef BIO_TUNNING_TOOL_USE

bool atci_biogui_check_task_life(int32_t type)
{
    LOGI("atci_biogui_check_task_life type %d \r\n", type);

    switch(type){
        case TYPE_PPG1:
            LOGI("atci_biogui_check_task_life PPG1 %d \r\n", PPG1);
            if(PPG1) {
                return true;
            } else {
                return false;
            }
        case TYPE_PPG2:
            LOGI("atci_biogui_check_task_life PPG2 %d \r\n", PPG2);
            if(PPG2) {
                return true;
            } else {
                return false;
            }            
        case TYPE_EKG:
            LOGI("atci_biogui_check_task_life EKG %d \r\n", EKG);
            if(EKG) {
                return true;
            } else {
                return false;
            }            
        case TYPE_BISI:
            LOGI("atci_biogui_check_task_life BISI %d \r\n", BISI);
            if(BISI) {
                return true;
            } else {
                return false;
            }            
        default:
            break;
    }
    return false;
}

void atci_biogui_task_main(void *pvParameters)
{
    //task_para_t *p_task_para = (task_para_t *)pvParameters;
    task_para_t *p_task_para;
    int32_t signal_type;
    int32_t output_len, sram_type, sample;
    int32_t  i, j, page, sum, zero_cnt = 0;
    atci_response_t response = {{0}};
    uint32_t *temp_buffer;
    uint8_t str[20];

    while(1) {
        xSemaphoreTake(g_biogui_handler_semaphore,portMAX_DELAY);
        p_task_para = &task_para;
        signal_type = p_task_para->type;
        sample = p_task_para->sample;
        zero_cnt = 0;
        LOGI("collecting task start ... signal_type %d, sample %d\r\n", signal_type, sample);
        sample = (signal_type == TYPE_BISI)?BISI_SRAM_LEN:sample;
        while(1)
        {
            if(!atci_biogui_check_task_life(signal_type)) {
                break;
            }
            LOGI("collecting task pass block condition ... \r\n");
            if(signal_type == TYPE_PPG1) {
                sram_type = VSM_SRAM_PPG1;
            } else if(signal_type == TYPE_PPG2) {
                sram_type = VSM_SRAM_PPG2;
            } else if(signal_type == TYPE_EKG) {
                sram_type = VSM_SRAM_EKG;
            } else if(signal_type == TYPE_BISI) {
                sram_type = VSM_SRAM_BISI;
            } 
            temp_buffer = read_data;

            vsm_driver_read_sram((vsm_sram_type_t)sram_type, (uint32_t *)temp_buffer, &output_len);
            
            LOGI("1.output_len %d, sample %d \r\n", output_len, sample);
            if (output_len > 1) {            
                output_len = (output_len >= sample)?sample:output_len;
                LOGI("2.output_len %d, sample %d \r\n", output_len, sample);
                if (signal_type == TYPE_EKG || signal_type == TYPE_BISI) {
                    i = 0, j = 0, page = (output_len%EKG_DATA_PAGE==0)?(output_len/EKG_DATA_PAGE):(output_len/EKG_DATA_PAGE) + 1;
                    for(j = 0; j < page; j ++) {
                        //define last item
                        if (j == (page -1)) {
                            //sum = (j * DATA_PAGE) + ((output_len % DATA_PAGE)==0? DATA_PAGE:(output_len % DATA_PAGE));
                            sum = output_len;
                        } else {
                            sum = (j + 1) * EKG_DATA_PAGE;
                        }
                        //memset(&response, 0, sizeof(response));
                        for (i = j * EKG_DATA_PAGE; i < sum; i ++) {
                            if (i == j * EKG_DATA_PAGE) {
                                //[+EBIO:<type>,<channel>,<length>]
                                sprintf((char *)response.response_buf, "+EBIO:%ld,%d,%ld", 
                                        signal_type, 0, (sum - (j * EKG_DATA_PAGE)));
                            }
                            
                            //normal case
                            sprintf((char *)str, ",%lx", temp_buffer[i]);
                            strcat((char *)response.response_buf, (char *)str);
                            
                            if(i == (sum - 1)) {
                                strcat((char *)response.response_buf,"\r\n");
                            }
                        }
                        response.response_len = strlen((char *)response.response_buf);
                        //response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        atci_send_response(&response);
                        vTaskDelay(30);
                    }
                } else { // PPG1/PPG2 case
                    i = 0, j = 0, page = (output_len%DATA_PAGE==0)?(output_len/DATA_PAGE):(output_len/DATA_PAGE) + 1;
                    //first loop for led
                    for(j = 0; j < page; j ++) {
                        //define last item
                        if (j == (page -1)) {
                            //sum = (j * DATA_PAGE) + ((output_len % DATA_PAGE)==0? DATA_PAGE:(output_len % DATA_PAGE));
                            sum = output_len;
                        } else {
                            sum = (j + 1) * DATA_PAGE;
                        }
                        
                        //memset(&response, 0, sizeof(response));
                        //deal with led case first
                        for (i = j * DATA_PAGE; i < sum; i ++) {
                            if (i == j * DATA_PAGE) {
                                //[+EBIO:<type>,<channel>,<length>]
                                snprintf((char *)response.response_buf, sizeof(response.response_buf),"+EBIO:%ld,%d,%ld", 
                                        signal_type, 0, (sum - (j * DATA_PAGE))/2);
                            }
                            if ((i%2 == 0)) {
                                //normal case
                                snprintf((char *)str, sizeof(str), ",%lx", temp_buffer[i]);
                                strcat((char *)response.response_buf, (char *)str);
                            } 
                            if(i == (sum - 1)) {
                                strcat((char *)response.response_buf,"\r\n");
                            }
                        }
                        response.response_len = strlen((char *)response.response_buf);
                        //response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        atci_send_response(&response);
                        vTaskDelay(30);
                        //memset(&response, 0, sizeof(atci_response_t));
                        //second loop for led ambient
                        for (i = j * DATA_PAGE; i < sum; i ++) {
                                                       
                            if (i == j * DATA_PAGE) {
                                //[+EBIO:<type>,<channel>,<length>]
                                snprintf((char *)response.response_buf, sizeof(response.response_buf), "+EBIO:%ld,%d,%ld", 
                                        signal_type, 1, (sum - (j * DATA_PAGE))/2);
                            }
                            
                            if ((i%2 == 1)) {
                                //normal case
                                snprintf((char *)str, sizeof(str), ",%lx", temp_buffer[i]);
                                strcat((char *)response.response_buf, (char *)str);
                            } 
                            
                            if(i == (sum - 1)) {
                                strcat((char *)response.response_buf,"\r\n");
                            }
                        }

                        response.response_len = strlen((char *)response.response_buf);
                        //response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        atci_send_response(&response);
                        vTaskDelay(30);
                    }
                }
                
                //break if collect enough data
                if(output_len == sample) {
                    break;
                } else {
                    sample -= output_len;
                }
                LOGI("output_len %ld, sample %ld \r\n", output_len, sample);
                //vTaskDelay(10);
            } else {
                zero_cnt ++;
                if(zero_cnt > 100) {
                    break;
                }
            }        
        }

        if (signal_type == TYPE_EKG) {
            snprintf((char *)str, sizeof(str), "%s", "EKG");
            EKG = false;
        } else if (signal_type == TYPE_PPG1) {
            snprintf((char *)str, sizeof(str), "%s", "PPG1");
            PPG1 = false;
        } else if (signal_type == TYPE_PPG2) {
            snprintf((char *)str, sizeof(str), "%s", "PPG2");
            PPG2 = false;
        } else if (signal_type == TYPE_BISI) {
            snprintf((char *)str, sizeof(str), "%s", "BISI");
            BISI = false;
        }
        snprintf((char *)response.response_buf, sizeof(response.response_buf), "OK,%s\r\n", str);
        response.response_len = strlen((char *)response.response_buf);
        atci_send_response(&response);    
        
        LOGI("Leaving task... \r\n");
    }
    //vTaskDelete(NULL);
}
#endif

int32_t check_non_exist_item(uint32_t addr)
{
    int32_t non_exist_item = 0;
    if (addr == 0x33B8) {
        non_exist_item = 1;
    } else if (addr >= 0x33BC) {
        non_exist_item = 2;
    }
    return non_exist_item;
}

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_biogui(atci_parse_cmd_param_t *parse_cmd);


void modify_sram_irq_th(vsm_signal_t signal, int32_t signal_th)
{
    bus_data_t data;
	uint32_t addr = 0;
    int err = VSM_STATUS_OK;
    if (signal == VSM_SIGNAL_EKG) {
        addr = EKG_IRQ_TH_REG;
    } else if (signal == VSM_SIGNAL_PPG1) {
        addr = PPG1_IRQ_TH_REG;
    } else if (signal == VSM_SIGNAL_PPG2) {
        addr = PPG2_IRQ_TH_REG;
    } else if (signal == VSM_SIGNAL_BISI) {
        addr = BISI_IRQ_TH_REG;
    }

    data.addr = (addr & 0xFF00) >> 8;
    data.reg = (addr & 0xFF);
    data.data_buf = (uint8_t *)&signal_th;
    data.length = sizeof(signal_th);
    err = vsm_driver_write_register(&data);
    if (err == VSM_STATUS_OK) {
        vsm_driver_update_register();
    }
}

// AT command handler
atci_status_t atci_cmd_hdlr_biogui(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    char *type  = NULL;
    
    LOGI("atci_cmd_hdlr_biogui \r\n");
    
    response.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+EKEYURC=?
            strcpy((char *)response.response_buf, "+EBIO=(0,1)\r\nOK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EKEYURC=<op>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            strtok(parse_cmd->string_ptr, "=");
            type = strtok(NULL, ",");
            LOGI("BIO type %s , len %d, %c\r\n", type, strlen(type), *type);
            
            if (*type == '0') {                
                /* read register */
                uint32_t addr = 0, reg_value =10;
                char *s1;
                bus_data_t data;
                LOGI("read register type %s \r\n", type);
                s1 = strtok(NULL, ",");
                if (s1) {
                    sscanf(s1, "%lx", &addr);
                    data.addr = (addr & 0xFF00) >> 8;
	                data.reg = (addr & 0xFF);
                    data.data_buf = (uint8_t *)&reg_value;
                    data.length = sizeof(reg_value);
                    LOGI("data.addr 0x%x, data.reg 0x%x\r\n", data.addr, data.reg);
                    if (vsm_driver_read_register(&data) == VSM_STATUS_OK) {
                        snprintf((char *)response.response_buf, sizeof(response.response_buf), "+EBIO:%lx,%lx\r\n", 
                            addr, *(uint32_t *)data.data_buf);
                        response.response_len = strlen((char *)response.response_buf);
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        atci_send_response(&response);
                    } else {
                        LOGI("I2C error... \r\n");
                        strcpy((char *)response.response_buf, "I2C ERROR\r\n");
                        response.response_len = strlen((char *)response.response_buf);
                        atci_send_response(&response);
                    }
                } else {
                    LOGI("no addr value... \r\n");
                    strcpy((char *)response.response_buf, "ERROR\r\n");
                    response.response_len = strlen((char *)response.response_buf);
                    atci_send_response(&response);
                }
            } else if (*type == '1') {
                /* write register */
                uint32_t addr, reg_value;
                char *s1 = NULL, *s2 = NULL;
                bus_data_t data;
                LOGI("write register type %s \r\n", type);
                s1 = strtok(NULL, ",");
                s2 = strtok(NULL, ",");
                if(s1 && s2) {
                    sscanf(s1, "%lx", &addr);
                    sscanf(s2, "%lx", &reg_value);
                    data.addr = (addr & 0xFF00) >> 8;
	                data.reg = (addr & 0xFF);
                    data.data_buf = (uint8_t *)&reg_value;
                    data.length = sizeof(reg_value);
                    LOGI("data.addr 0x%x, data.reg 0x%x, reg_value 0x%x\r\n", data.addr, data.reg, reg_value);
                    if(addr == 0x3360 && reg_value != 0x00000000) { 
                        vsm_driver_reset_PPG_counter();
                    }
                    if(vsm_driver_write_register(&data) == VSM_STATUS_OK) {
                        response.response_len = 0;
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        atci_send_response(&response);
                        if(addr == 0x3360 && reg_value == 0x00000000) { 
                            vsm_data_drop(VSM_SRAM_EKG);
                            vsm_data_drop(VSM_SRAM_PPG1);
                            vsm_data_drop(VSM_SRAM_PPG2);
                            vsm_data_drop(VSM_SRAM_BISI);
                        }
                    } else {
                        LOGI("I2C error... \r\n");
                        strcpy((char *)response.response_buf, "I2C ERROR\r\n");
                        response.response_len = strlen((char *)response.response_buf);
                        atci_send_response(&response);
                    }
                } else {
                    LOGI("no addr value or register value... \r\n");
                    strcpy((char *)response.response_buf, "ERROR\r\n");
                    response.response_len = strlen((char *)response.response_buf);
                    atci_send_response(&response);
                }
                
            } else if (*type == '2') {
                /* batch read operation */
                uint32_t addr1, addr2, temp_addr2 = 0, reg_value;
                char *s1 = NULL, *s2 = NULL;
                bus_data_t data;
                int i = 0, count = 0, count2 = 0;
                int non_exist_item = 0;
                uint8_t str[20];
                
                s1 = strtok(NULL, ",");
                s2 = strtok(NULL, ",");
                
                if(s1 && s2) {
                    sscanf(s1, "%lx", &addr1);
                    sscanf(s2, "%lx", &addr2);
                    //if the range is legal or not
                    if ( (addr2 > addr1) && (addr2 % 4 == 0) && (addr1 % 4 == 0)) {
                        // if addr2 is another slave id, then store it and 
                        // assign it to the destination of the same slave id
                        if(((addr1 & 0xFF00) >> 8) != (addr2 & 0xFF00) >> 8) {
                            if (addr2 > LAST_ITEM_SLAVE2) {
                                addr2 = LAST_ITEM_SLAVE2;
                            }
                            temp_addr2 = addr2;
                            addr2 = LAST_ITEM_SLAVE1;
                            count2 =((temp_addr2 - FIRST_ITEM_SLAVE0)/4 + 1);
                            //compute how non_exist_item
                            non_exist_item = check_non_exist_item(temp_addr2);
                        } else {
                            //compute how non_exist_item
                            non_exist_item = check_non_exist_item(addr2);
                        }
                        count = (addr2 - addr1) / 4 + 1;
                        LOGI("addr1 0x%x, addr2 0x%x, temp_addr2 0x%x, non_exist_item %d\r\n",
                            addr1, addr2, temp_addr2, non_exist_item);
                        for (i = 0; i < count; i ++) {
                            if (addr1 == 0x33B8 || addr1 == 0x33BC) {
                                addr1 +=4;
                                continue;
                            }
                            data.addr = (addr1 & 0xFF00) >> 8;
        	                data.reg = (addr1 & 0xFF);
                            data.data_buf = (uint8_t *)&reg_value;
                            data.length = sizeof(reg_value);
                            reg_value = 0;
                            //LOGI("data.addr 0x%x, data.reg 0x%x\r\n", data.addr, data.reg);
                            memset(str, 0, sizeof(str));
                            if (vsm_driver_read_register(&data) == VSM_STATUS_OK) {
                                //LOGI("reg_value 0x%x, i %d\r\n", reg_value, i);
                                /* deal with first item*/
                                if (i == 0) {                                    
                                    if (i == (count - 1)) {
                                        //accumulate another slave id length
                                        if (temp_addr2 != 0) {
                                            snprintf((char *)response.response_buf, sizeof(response.response_buf), "+EBIO:%d,%lx\r\n", 
                                                count + count2 - non_exist_item, *(uint32_t *)data.data_buf);
                                        } else {
                                            snprintf((char *)response.response_buf, sizeof(response.response_buf), "+EBIO:%d,%lx\r\n", 
                                                count - non_exist_item, *(uint32_t *)data.data_buf);
                                        }
                                        response.response_len = strlen((char *)response.response_buf);
                                    } else {  
                                        if (temp_addr2 != 0) {
                                            snprintf((char *)response.response_buf, sizeof(response.response_buf), "+EBIO:%d,%lx,", 
                                                count + count2 - non_exist_item, *(uint32_t *)data.data_buf);
                                        } else {
                                            snprintf((char *)response.response_buf, sizeof(response.response_buf), "+EBIO:%d,%lx,", 
                                                count - non_exist_item, *(uint32_t *)data.data_buf);
                                        } 
                                        response.response_len = strlen((char *)response.response_buf);
                                    }
                                } else if(i == (count - 1)) {/* deal with other item*/
                                    if (temp_addr2 == 0) {
                                        snprintf((char *)str, sizeof(str), "%lx\r\n", *(uint32_t *)data.data_buf);
                                    } else {
                                        sprintf((char *)str, "%lx", *(uint32_t *)data.data_buf);
                                    }
                                    strcat((char *)response.response_buf,(char *)str);
                                } else {
                                    snprintf((char *)str, sizeof(str), "%lx,", *(uint32_t *)data.data_buf);
                                    strcat((char *)response.response_buf,(char *)str);
                                }                                
                            } else {
                                LOGI("I2C error... \r\n");
                                strcpy((char *)response.response_buf, "I2C ERROR\r\n");
                                response.response_len = strlen((char *)response.response_buf);
                                atci_send_response(&response);
                                break;
                            }
                            addr1 += 4;
                        }

                        LOGI("for loop i %d \r\n", i);
                        if ( i == count ){
                            response.response_len = strlen((char *)response.response_buf);                                

                            if (temp_addr2 == 0) {
                                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                            }
                            atci_send_response(&response);
                        }
                        
                        if (temp_addr2 != 0) {
                            addr1 = FIRST_ITEM_SLAVE0;
                            addr2 = temp_addr2;
                            //count = count2;
                            LOGI("slave 0x33 addr1 0x%x, addr2 0x%x\r\n", addr1, addr2);
                            memset(&response, 0, sizeof(response));
                            for (i = 0; i < count2; i ++) {
                                if (addr1 == 0x33B8 || addr1 == 0x33BC) {
                                    addr1 +=4;
                                    continue;
                                }
                                data.addr = (addr1 & 0xFF00) >> 8;
            	                data.reg = (addr1 & 0xFF);
                                data.data_buf = (uint8_t *)&reg_value;
                                data.length = sizeof(reg_value);
                                reg_value = 0;
                                //LOGI("data.addr 0x%x, data.reg 0x%x\r\n", data.addr, data.reg);
                                memset(str, 0, sizeof(str));
                                if (vsm_driver_read_register(&data) == VSM_STATUS_OK) {
                                    //LOGI("reg_value 0x%x, i %d\r\n", reg_value, i);
                                    if(i == (count2 - 1)) {
                                        snprintf((char *)str, sizeof(str), ",%lx\r\n", *(uint32_t *)data.data_buf);
                                        strcat((char *)response.response_buf, (char *)str);
                                    } else {
                                        snprintf((char *)str, sizeof(str), ",%lx", *(uint32_t *)data.data_buf);
                                        strcat((char *)response.response_buf, (char *)str);
                                    }                                
                                } else {
                                    LOGI("I2C error... \r\n");
                                    strcpy((char *)response.response_buf, "I2C ERROR\r\n");
                                    response.response_len = strlen((char *)response.response_buf);
                                    atci_send_response(&response);
                                    break;
                                }
                                addr1 += 4;
                            }

                            LOGI("for loop i %d \r\n", i);
                            if ( i == count2 ){
                                response.response_len = strlen((char *)response.response_buf);                                
                                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                                atci_send_response(&response);
                            }
                        }
                        
                    } else {
                        LOGI("wrong address range... \r\n");
                        strcpy((char *)response.response_buf, "ADDRESS ERROR\r\n");
                        response.response_len = strlen((char *)response.response_buf);
                        atci_send_response(&response);
                    }
                } else {
                    LOGI("no addr value... \r\n");
                    strcpy((char *)response.response_buf, "ERROR\r\n");
                    response.response_len = strlen((char *)response.response_buf);
                    atci_send_response(&response);
                }
                
            } else if (*type == '3') {
                /* start collecting PPG/EKG/BISI data */
                if(!g_hw_init) {
                    vsm_driver_init();
                    vsm_driver_set_signal(VSM_SIGNAL_EKG);
                    //vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                    vsm_driver_set_signal(VSM_SIGNAL_BISI);
                    vsm_driver_set_signal(VSM_SIGNAL_PPG2);
                    vsm_driver_set_signal(VSM_SIGNAL_PPG1_512HZ);
                    g_hw_init = true;
                }
#ifdef BIO_TUNNING_TOOL_USE
                bool enable_get_sram = false;
                char *s1 = NULL, *s2 = NULL;
                s1 = strtok(NULL, ",");
                s2 = strtok(NULL, ",");
                int32_t signal_type, samples;
                if(s1 && s2) {                    
                    sscanf(s1, "%ld", &signal_type);
                    sscanf(s2, "%ld", &samples);
                    LOGI("start collect type %d ,sample %d\r\n", signal_type, samples);
                    //enable signal, type: 0:ppg1 1:ppg2 2:ekg
                    LOGI("PPG1 %d, PPG2 %d, EKG %d, BISI %d\r\n", PPG1, PPG2, EKG, BISI);
                    switch(signal_type){
                        //start collecting PPG1
                        case TYPE_PPG1:
                            if(!PPG1) {
                                PPG1 = true;
                                enable_get_sram = true;
                            }
                            break;
                        //start collecting PPG2
                        case TYPE_PPG2:
                            if(!PPG2) {
                                PPG2 = true;
                                enable_get_sram = true;
                            }
                            break;
                        //start collecting EKG
                        case TYPE_EKG:
                            if(!EKG) {
                                EKG = true;
                                enable_get_sram = true;
                            }
                            break;
                        case TYPE_BISI:
                            if(!BISI) {
                                BISI = true;
                                enable_get_sram = true;
                            }
                            break;
                        default:
                            break;
                    }
                    if (enable_get_sram) {
                        task_para.type = signal_type;
                        task_para.sample = samples;
                        LOGI("task_para.type %d ,task_para.sample %d\r\n", task_para.type, task_para.sample);
                        //if(!atci_keypad_queue_handle) {
                        if(!g_biogui_handler_semaphore) {
                            vSemaphoreCreateBinary(g_biogui_handler_semaphore);
                            /* call Task */
                            xTaskCreate( atci_biogui_task_main, ATCI_BIO_TASK_NAME, ATCI_BIO_TASK_STACKSIZE, &task_para, ATCI_BIO_TASK_PRIO, atci_biogui_task_handle);
                        } else {
                            xSemaphoreGive(g_biogui_handler_semaphore);
                        }
                        
                        #if 1
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        atci_send_response(&response);
                        #endif
                    } else {
                        strcpy((char *)response.response_buf, "IN_USE ERROR\r\n");
                        response.response_len = strlen((char *)response.response_buf);
                        atci_send_response(&response);
                    }
                    #if 0
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    atci_send_response(&response);
                    #endif
                } else {
                    LOGI("no type or sample value... \r\n");
                    strcpy((char *)response.response_buf, "ERROR\r\n");
                    response.response_len = strlen((char *)response.response_buf);
                }
#else
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&response);
#endif
            } else if (*type == '4') {
                /* stop collecting PPG/EKG data */
                int32_t signal_type;
                char *s1 = NULL;
                s1 = strtok(NULL, ",");
                sscanf(s1, "%ld", &signal_type);
                LOGI("stop collect type %s %d\r\n", s1,signal_type);
                switch(signal_type){
                    //PPG1
                    case 0:
                        if(PPG1) {
                            //disable PPG1
                            PPG1 = false;
                        }
                        break;
                    //PPG2
                    case 1:
                        if(PPG2) {
                            //disable PPG2
                            PPG2 = false;
                        }
                        break;
                    //EKG
                    case 2:
                        if(EKG) {
                            //disable EKG
                            EKG = false;
                        }
                        break;
                    default:
                        break;
                }
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&response);                
            } else if (*type == '5') {
                uint32_t reg_value = 0x01c30243;
                bus_data_t data;
                LOGI("reset type %s \r\n", type);
                /* reset to EVM default */
                vsm_driver_init();
                
                vsm_driver_set_signal(VSM_SIGNAL_EKG);
                //vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                vsm_driver_set_signal(VSM_SIGNAL_BISI);
                vsm_driver_set_signal(VSM_SIGNAL_PPG2);
                vsm_driver_set_signal(VSM_SIGNAL_PPG1_512HZ);

                
                data.addr = 0x23;
                data.reg = 0x34;
                data.data_buf = (uint8_t *)&reg_value;
                data.length = sizeof(reg_value);
                vsm_driver_write_register(&data);

                g_hw_init = true;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&response);
            }  
            /* at cmd for smt case of instrument vendor */
            else if (*type == '6') {
                char *s1 = NULL;
                uint32_t test_item = 0;
                s1 = strtok(NULL, ",");
                LOGI("SMT test item %s \r\n", s1);
                g_hw_init = true;
                if(s1) {
                    sscanf(s1, "%lx", &test_item);
                    if (test_item == 1) {
                        char *s2 = NULL;
                        bus_data_t data;
                    	uint32_t enable_data = 0;
                        uint32_t ambdac = 0;
                        s2 = strtok(NULL, ",");
                        /*PD TEST, LED off*/
                        vsm_driver_init();
                        vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                        hal_gpio_set_output(GPIO_MT2511_PPG_VDRV_EN, HAL_GPIO_DATA_LOW);
                        vsm_driver_disable_signal(VSM_SIGNAL_PPG1);
                        data.addr = 0x33;
                        data.reg = 0x2C;
                        data.data_buf = (uint8_t *)&enable_data;
                        data.length = 4;
                        vsm_driver_write_register(&data);
                        if (s2) {
                            sscanf(s2, "%lx", &ambdac);
                            set_ambdac(ambdac);
                        }
                        vsm_driver_enable_signal(VSM_SIGNAL_PPG1);
                    } else if(test_item == 2) {
                        char *s2 = NULL, *s3 = NULL, *s4 = NULL;
                        uint32_t cur_setting = 0;
                        uint32_t cur_range = 0;
                        uint32_t ambdac = 0;
                        /*LED TEST, LED ALL ON*/
                        vsm_driver_init();
                        vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                        vsm_driver_disable_signal(VSM_SIGNAL_PPG1);
                        //set dac1/2 code
                        s2 = strtok(NULL, ",");
                        if(s2) {
                            bus_data_t data;
                            sscanf(s2, "%lx", &cur_setting);
                            data.addr = 0x33;
                            data.reg = 0x2C;
                            data.data_buf = (uint8_t *)&cur_setting;
                            data.length = 4;
                            LOGI("cur_setting 0x%x\r\n", cur_setting);
                            vsm_driver_write_register(&data);
                        }
                        //set full-scale range of led current
                        s3 = strtok(NULL, ",");
                        if (s3) {
                            sscanf(s3, "%lx", &cur_range);
                            vsm_driver_set_led_current_range(cur_range);
                        }
                        //set ambdac current                        
                        s4 = strtok(NULL, ",");
                        if (s4) {
                            sscanf(s4, "%lx", &ambdac);
                            set_ambdac(ambdac);
                        }
                        vsm_driver_enable_signal(VSM_SIGNAL_PPG1);
                    } else if(test_item == 3) {
                        /*Only tunr on tx2 led*/
                        char *s2 = NULL, *s3 = NULL, *s4 = NULL;
                        uint32_t cur_setting = 0;
                        bus_data_t data;
                    	uint32_t enable_data;
                        uint32_t cur_range = 0;
                        uint32_t ambdac = 0;

                        s2 = strtok(NULL, ",");                                               
                        vsm_driver_init();
                        vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                        vsm_driver_disable_signal(VSM_SIGNAL_PPG1);
                        //disable right side
                        data.addr = 0x33;
                        data.reg = 0x2C;
                        data.data_buf = (uint8_t *)&enable_data;
                        data.length = 4;
                        vsm_driver_read_register(&data);
                        if(s2) {
                            sscanf(s2, "%lx", &cur_setting);
                            enable_data = (0xFF00 & cur_setting);
                        } else {
                            enable_data &= 0xFF00;
                        }                        
                        //vsm_driver_write_register(&data);
                        enable_data = (enable_data >> 8) & 0xFF;
                        vsm_driver_set_led_current(VSM_LED_1, 0);
                        vsm_driver_set_led_current(VSM_LED_2, enable_data);

                        //set full-scale range of led current
                        s3 = strtok(NULL, ",");
                        if (s3) {
                            sscanf(s3, "%lx", &cur_range);
                            vsm_driver_set_led_current_range(cur_range);
                        }
                        
                        //set ambdac
                        s4 = strtok(NULL, ",");
                        if (s4) {
                            sscanf(s4, "%lx", &ambdac);
                            set_ambdac(ambdac);
                        }
                        vsm_driver_enable_signal(VSM_SIGNAL_PPG1);
                    } else if(test_item == 4) {
                        /*Only tunr on tx1 led*/
                        char *s2 = NULL, *s3 = NULL, *s4 = NULL;
                        uint32_t cur_setting = 0;
                        bus_data_t data;
                    	uint32_t enable_data;
                        uint32_t cur_range = 0;
                        uint32_t ambdac = 0;

                        s2 = strtok(NULL, ",");
                        vsm_driver_init();
                        vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                        vsm_driver_disable_signal(VSM_SIGNAL_PPG1);
                        //disable left side
                        data.addr = 0x33;
                        data.reg = 0x2C;
                        data.data_buf = (uint8_t *)&enable_data;
                        data.length = 4;
                        vsm_driver_read_register(&data);
                        if(s2) {
                            sscanf(s2, "%lx", &cur_setting);
                            enable_data = (0xFF & cur_setting);
                        } else {
                            enable_data &= 0xFF;
                        }
                        //vsm_driver_write_register(&data);
                        enable_data &= 0xFF;
                        vsm_driver_set_led_current(VSM_LED_1, enable_data);
                        vsm_driver_set_led_current(VSM_LED_2, 0);

                        //set full-scale range of led current
                        s3 = strtok(NULL, ",");
                        if (s3) {
                            sscanf(s3, "%lx", &cur_range);
                            vsm_driver_set_led_current_range(cur_range);
                        }

                        //set ambdac
                        s4 = strtok(NULL, ",");
                        if (s4) {
                            sscanf(s4, "%lx", &ambdac);
                            set_ambdac(ambdac);
                        }
                        vsm_driver_enable_signal(VSM_SIGNAL_PPG1);
                    } else if(test_item == 5) {
                        char *s2 = NULL;
                        s2 = strtok(NULL, ",");
                        /*EKG mode*/
                        vsm_driver_init();
                        vsm_driver_set_signal(VSM_SIGNAL_EKG);
                        vsm_driver_disable_signal(VSM_SIGNAL_EKG);                        
                        if (s2) {                            
                            uint32_t ekg_setting = 0;
                            bus_data_t data;
                            uint32_t ekg_data = 0;
                            data.addr = 0x33;
                            data.reg = 0x08;
                            data.data_buf = (uint8_t *)&ekg_data;
                            data.length = sizeof(uint32_t);
                            sscanf(s2, "%lx", &ekg_setting);
                            if ( ekg_setting == 1) {
                                /*RLD mode*/
                                ekg_data = 0x0001D042;
                            } else if ( ekg_setting == 0) {
                                /*2E mode*/                
                                ekg_data = 0x0001D442;
                            }
                            if (ekg_data != 0) {
                                vsm_driver_write_register(&data);
                                vsm_driver_update_register();
                            }
                        }
                        vsm_driver_enable_signal(VSM_SIGNAL_EKG);
                    } 
                }
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&response);
            } 
            /* This is original smt test case*/
            else if (*type == '7') {
                char *s1 = NULL;
                uint32_t test_item = 0;
                s1 = strtok(NULL, ",");
                LOGI("SMT test item %s \r\n", s1);
                if(s1) {
                    sscanf(s1, "%lx", &test_item);
                    if (test_item == 1) {                        
                        /* enable PPG for 1 sec and get one ppg led data*/
                        int32_t output_len;
                        vsm_driver_init();
                        vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                        #ifdef BIO_TUNNING_TOOL_USE
                        vsm_driver_enable_signal(VSM_SIGNAL_PPG1);
                        #endif
                        vTaskDelay(1000);
                        /* read back ppg data */
                        memset(read_data, 0, sizeof(read_data));
                        vsm_driver_read_sram(VSM_SRAM_PPG1, (uint32_t *)read_data, &output_len);
                        if (output_len > 1) {
                            if ((output_len/2)%2 == 1){
                                output_len+=2;
                            }
                            snprintf((char *)response.response_buf, sizeof(response.response_buf), "+EBIO: 0x%lx\r\n", read_data[output_len/2]);
                            response.response_len = strlen((char *)response.response_buf);
                            //response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                            atci_send_response(&response);
                        }

                        hal_gpio_set_output(GPIO_MT2511_PPG_VDRV_EN, HAL_GPIO_DATA_LOW);
                        LOGI("BIO read sram successfully... \r\n");
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        				response.response_len   = 0;
                    } else if (test_item == 2) {                        
                        /* turn off */
                        vsm_driver_deinit();
                        LOGI("disable MT2511 successfully \r\n");
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        response.response_len   = 0;
                    } 
                    #ifdef MT2511_SMT_MODE
                    else if (test_item == 4) { /*ekg noise test*/
                        char *s2 = NULL;
                        s2 = strtok(NULL, ",");
                        
                        if (s2) {                            
                            double signal_nosie;
                            uint32_t ekg_setting = 0;
                            sscanf(s2, "%lx", &ekg_setting);
                            if ( ekg_setting == 0) {
                                /*2E mode*/
                                ekg_mode = 2;
                            } else if ( ekg_setting == 1) {
                                /*RLD mode*/
                                ekg_mode = 1;
                            }
                            signal_noise_compute(VSM_SIGNAL_EKG, &signal_nosie, false, false);
                            snprintf((char *)response.response_buf, sizeof(response.response_buf), "+EBIO:0,%.12f\r\n", signal_nosie);
                            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                            //response.response_len   = 0;
                            response.response_len = strlen((char *)response.response_buf);
                        } else {
                            strcpy((char *)response.response_buf, "ERROR\r\n");
                            response.response_len = strlen((char *)response.response_buf);
                        }

                    } else if (test_item == 5) { /*ppg led1 noise test*/
                        double signal_nosie;
                        signal_noise_compute(VSM_SIGNAL_PPG1, &signal_nosie, true, false);
                        snprintf((char *)response.response_buf, sizeof(response.response_buf), "+EBIO:1,%.12f\r\n", signal_nosie);
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        //response.response_len   = 0;
                        response.response_len = strlen((char *)response.response_buf);
                    } else if (test_item == 6) { /*ppg ambient1 noise test*/
                        double signal_nosie;
                        signal_noise_compute(VSM_SIGNAL_PPG1, &signal_nosie, false, false);
                        snprintf((char *)response.response_buf, sizeof(response.response_buf), "+EBIO:2,%.12f\r\n", signal_nosie);
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        //response.response_len   = 0;
                        response.response_len = strlen((char *)response.response_buf);
                    } else if (test_item == 7) { /*Quick PPG ac data scan test*/
                        char *s2 = NULL, *s3 = NULL, *s4 = NULL;
                        uint32_t tia_gain = 0, tia_cf = 0, led_cur_range = 0;
                        s2 = strtok(NULL, ",");
                        if(s2) {
                            sscanf(s2, "%lx", &tia_gain);
                            s3 = strtok(NULL, ",");
                            if (s3) {
                                sscanf(s3, "%lx", &tia_cf);
                                s4 = strtok(NULL, ",");
                                if (s4) {
                                    sscanf(s4, "%lx", &led_cur_range);
                                    current_test(LED_CUR_TEST, tia_gain, tia_cf, led_cur_range);
                                    xTaskCreate( atci_clover_task_dump, ATCI_BIO_SMT_TASK_NAME, ATCI_BIO_SMT_TASK_STACKSIZE, &test_item, TASK_PRIORITY_NORMAL, NULL);
                                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                                    response.response_len = 0;

                                } else {
                                    /*invalide parameter, return "ERROR"*/
                                    strcpy((char *)response.response_buf, "ERROR\r\n");
                                    response.response_len = strlen((char *)response.response_buf);
                                }
                            } else {
                                /*invalide parameter, return "ERROR"*/
                                strcpy((char *)response.response_buf, "ERROR\r\n");
                                response.response_len = strlen((char *)response.response_buf);
                            }
                        } else {
                            /*invalide parameter, return "ERROR"*/
                            strcpy((char *)response.response_buf, "ERROR\r\n");
                            response.response_len = strlen((char *)response.response_buf);
                        }
                    } else if (test_item == 8) { /*Detail PPG ac data scan test*/
                        char *s2 = NULL, *s3 = NULL, *s4 = NULL;
                        uint32_t tia_gain = 0, tia_cf = 0, led_cur_range = 0;
                        s2 = strtok(NULL, ",");
                        if(s2) {
                            sscanf(s2, "%lx", &tia_gain);
                            s3 = strtok(NULL, ",");
                            if (s3) {
                                sscanf(s3, "%lx", &tia_cf);
                                s4 = strtok(NULL, ",");
                                if (s4) {
                                    sscanf(s4, "%lx", &led_cur_range);
                                    current_test(LED_AMBDAC_CUR_TEST, tia_gain, tia_cf, led_cur_range);
                                    xTaskCreate( atci_clover_task_dump, ATCI_BIO_SMT_TASK_NAME, ATCI_BIO_SMT_TASK_STACKSIZE, &test_item, TASK_PRIORITY_NORMAL, NULL);
                                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                                    response.response_len = 0;

                                } else {
                                    /*invalide parameter, return "ERROR"*/
                                    strcpy((char *)response.response_buf, "ERROR\r\n");
                                    response.response_len = strlen((char *)response.response_buf);
                                }
                            } else {
                                /*invalide parameter, return "ERROR"*/
                                strcpy((char *)response.response_buf, "ERROR\r\n");
                                response.response_len = strlen((char *)response.response_buf);
                            }
                        } else {
                            /*invalide parameter, return "ERROR"*/
                            strcpy((char *)response.response_buf, "ERROR\r\n");
                            response.response_len = strlen((char *)response.response_buf);
                        }
                    }
                    #endif
                    else {
                        /*invalide parameter, return "ERROR"*/
                        strcpy((char *)response.response_buf, "ERROR\r\n");
                        response.response_len = strlen((char *)response.response_buf);
                    }

                    atci_send_response(&response);
                    break;
                }
            } else {
                /* others are invalid command format */ 
                strcpy((char *)response.response_buf, "ERROR\r\n");
                response.response_len = strlen((char *)response.response_buf);
                atci_send_response(&response);
            }
            
            break;
        default :
            /* others are invalid command format */ 
            strcpy((char *)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}



