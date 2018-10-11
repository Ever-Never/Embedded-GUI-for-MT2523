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

#include "hal_gpio.h"


#ifdef HAL_GPIO_MODULE_ENABLED
#include "hal_gpio_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t save_and_set_interrupt_mask(void);
extern void restore_interrupt_mask(uint32_t mask);

/* check if the pin has only one pull-up resister and one pull-down resister*/
/* pin with pullsel means the pin has only one pull-up resister and one pull-down resister*/
uint32_t is_pin_with_pullsel(hal_gpio_pin_t gpio_pin)
{
    uint32_t i;
    uint32_t pin_number_with_pullsel[5] = {0, 1, 2, 3, 10}; /* pin number which has only one pull-up resister and one pull-down resister, it is different on different chips */

    for (i = 0; i < 5; i++) {
        if (pin_number_with_pullsel[i] == gpio_pin) {
            return 1;
        }
    }
    return 0;
}


GPIO_REGISTER_T *gpio_register_base = (GPIO_REGISTER_T *)(GPIO_BASE);

hal_gpio_status_t hal_gpio_init(hal_gpio_pin_t gpio_pin)
{
    return HAL_GPIO_STATUS_OK;
}


hal_gpio_status_t hal_gpio_deinit(hal_gpio_pin_t gpio_pin)
{
    return HAL_GPIO_STATUS_OK;
}



hal_gpio_status_t hal_gpio_set_direction(hal_gpio_pin_t gpio_pin, hal_gpio_direction_t gpio_direction)
{
    uint32_t no;
    uint32_t remainder;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_DIR_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_DIR_REG_CTRL_PIN_NUM;

    if (gpio_direction == HAL_GPIO_DIRECTION_INPUT) {
        gpio_register_base->GPIO_DIR_REGISTER[no].GPIO_DIR_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    } else {
        gpio_register_base->GPIO_DIR_REGISTER[no].GPIO_DIR_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    }
    return HAL_GPIO_STATUS_OK;

}



hal_gpio_status_t hal_gpio_get_direction(hal_gpio_pin_t gpio_pin, hal_gpio_direction_t *gpio_direction)
{
    uint32_t no;
    uint32_t remainder;
    uint32_t temp;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    if (NULL == gpio_direction) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_DIR_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_DIR_REG_CTRL_PIN_NUM;

    temp = gpio_register_base->GPIO_DIR_REGISTER[no].GPIO_DIR;
    temp = temp & (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    *gpio_direction = (hal_gpio_direction_t)(temp >> remainder);
    return HAL_GPIO_STATUS_OK;

}



hal_pinmux_status_t hal_pinmux_set_function(hal_gpio_pin_t gpio_pin, uint8_t function_index)
{
    uint32_t no;
    uint32_t remainder;
    uint32_t irq_status;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_PINMUX_STATUS_ERROR_PORT;
    }

    /* check whether the function index is right as one function is corresponding to 4 bits of oen pin */
    if (function_index >= GPIO_MODE_MAX_NUMBER) {
        return HAL_PINMUX_STATUS_INVALID_FUNCTION;
    }

    /* get the register number corresponding to the pin as one register can control 8 pins*/
    no = gpio_pin / GPIO_MODE_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 8 pins*/
    remainder = gpio_pin % GPIO_MODE_REG_CTRL_PIN_NUM;

    /* protect the configuration to prevent possible interrupt */
    irq_status = save_and_set_interrupt_mask();
    gpio_register_base->GPIO_MODE_REGISTER[no].GPIO_MODE_CLR = (GPIO_REG_FOUR_BIT_SET_CLR << (remainder * GPIO_MODE_FUNCTION_CTRL_BITS));
    gpio_register_base->GPIO_MODE_REGISTER[no].GPIO_MODE_SET = (function_index << (remainder * GPIO_MODE_FUNCTION_CTRL_BITS));
    restore_interrupt_mask(irq_status);
    return HAL_PINMUX_STATUS_OK;

}



hal_gpio_status_t hal_gpio_get_input(hal_gpio_pin_t gpio_pin, hal_gpio_data_t *gpio_data)
{
    uint32_t no;
    uint32_t remainder;
    uint32_t temp;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    if (NULL == gpio_data) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_DIN_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_DIN_REG_CTRL_PIN_NUM;

    temp = gpio_register_base->GPIO_DIN_REGISTER[no].GPIO_DIN;
    temp = temp & (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    *gpio_data = (hal_gpio_data_t)(temp >> remainder);
    return HAL_GPIO_STATUS_OK;

}



hal_gpio_status_t hal_gpio_set_output(hal_gpio_pin_t gpio_pin, hal_gpio_data_t gpio_data)
{
    uint32_t no;
    uint32_t remainder;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_DOUT_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_DOUT_REG_CTRL_PIN_NUM;

    if (gpio_data) {
        gpio_register_base->GPIO_DOUT_REGISTER[no].GPIO_DOUT_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    } else {
        gpio_register_base->GPIO_DOUT_REGISTER[no].GPIO_DOUT_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    }
    return HAL_GPIO_STATUS_OK;
}



hal_gpio_status_t hal_gpio_get_output(hal_gpio_pin_t gpio_pin, hal_gpio_data_t *gpio_data)
{
    uint32_t no;
    uint32_t remainder;
    uint32_t temp;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    if (NULL == gpio_data) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_DOUT_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_DOUT_REG_CTRL_PIN_NUM;

    temp = gpio_register_base->GPIO_DOUT_REGISTER[no].GPIO_DOUT;
    temp = temp & (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    *gpio_data = (hal_gpio_data_t)(temp >> remainder);
    return HAL_GPIO_STATUS_OK;

}



hal_gpio_status_t hal_gpio_toggle_pin(hal_gpio_pin_t gpio_pin)
{
    uint32_t no;
    uint32_t remainder;
    uint32_t temp;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_DOUT_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_DOUT_REG_CTRL_PIN_NUM;

    temp = gpio_register_base->GPIO_DOUT_REGISTER[no].GPIO_DOUT;
    temp = temp & (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    if ((temp >> remainder)) {
        gpio_register_base->GPIO_DOUT_REGISTER[no].GPIO_DOUT_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    } else {
        gpio_register_base->GPIO_DOUT_REGISTER[no].GPIO_DOUT_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    }
    return HAL_GPIO_STATUS_OK;
}



#ifdef HAL_GPIO_FEATURE_INVERSE
hal_gpio_status_t hal_gpio_enable_inversion(hal_gpio_pin_t gpio_pin)
{
    uint32_t no;
    uint32_t remainder;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_DINV_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_DINV_REG_CTRL_PIN_NUM;

    gpio_register_base->GPIO_DINV_REGISTER[no].GPIO_DINV_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    return HAL_GPIO_STATUS_OK;
}



hal_gpio_status_t hal_gpio_disable_inversion(hal_gpio_pin_t gpio_pin)
{
    uint32_t no;
    uint32_t remainder;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_DINV_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_DINV_REG_CTRL_PIN_NUM;

    gpio_register_base->GPIO_DINV_REGISTER[no].GPIO_DINV_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    return HAL_GPIO_STATUS_OK;
}
#endif



hal_gpio_status_t hal_gpio_pull_up(hal_gpio_pin_t gpio_pin)
{
    uint32_t no;
    uint32_t remainder;

    if ((gpio_pin >= HAL_GPIO_MAX) || (!is_pin_with_pullsel(gpio_pin))) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_PULL_CTRL_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_PULL_CTRL_REG_CTRL_PIN_NUM;

    gpio_register_base->GPIO_PULLEN_REGISTER[no].GPIO_PULLEN_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    gpio_register_base->GPIO_PULLSEL_REGISTER[no].GPIO_PULLSEL_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    return HAL_GPIO_STATUS_OK;
}



hal_gpio_status_t hal_gpio_pull_down(hal_gpio_pin_t gpio_pin)
{
    uint32_t no;
    uint32_t remainder;

    if ((gpio_pin >= HAL_GPIO_MAX) || (!is_pin_with_pullsel(gpio_pin))) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_PULL_CTRL_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_PULL_CTRL_REG_CTRL_PIN_NUM;

    gpio_register_base->GPIO_PULLEN_REGISTER[no].GPIO_PULLEN_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    gpio_register_base->GPIO_PULLSEL_REGISTER[no].GPIO_PULLSEL_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    return HAL_GPIO_STATUS_OK;
}



hal_gpio_status_t hal_gpio_disable_pull(hal_gpio_pin_t gpio_pin)
{
    uint32_t no;
    uint32_t remainder;

    if ((gpio_pin >= HAL_GPIO_MAX) || (!is_pin_with_pullsel(gpio_pin))) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_PULL_CTRL_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_PULL_CTRL_REG_CTRL_PIN_NUM;

    gpio_register_base->GPIO_PULLEN_REGISTER[no].GPIO_PULLEN_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    return HAL_GPIO_STATUS_OK;
}



#ifdef HAL_GPIO_FEATURE_CLOCKOUT
hal_gpio_status_t hal_gpio_set_clockout(hal_gpio_clock_t gpio_clock_num, hal_gpio_clock_mode_t clock_mode)
{

    if (gpio_clock_num >= HAL_GPIO_CLOCK_MAX) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    gpio_register_base->GPIO_CLKOUT_REGISTER[gpio_clock_num].GPIO_CLKOUT = clock_mode;
    return HAL_GPIO_STATUS_OK;
}

#endif



#ifdef HAL_GPIO_FEATURE_PUPD
hal_gpio_status_t hal_gpio_set_pupd_register(hal_gpio_pin_t gpio_pin, uint8_t gpio_pupd, uint8_t gpio_r0, uint8_t gpio_r1)
{
    uint32_t no;
    uint32_t remainder;

    if ((gpio_pin >= HAL_GPIO_MAX) || (is_pin_with_pullsel(gpio_pin))) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no = gpio_pin / GPIO_PUPD_CTRL_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder = gpio_pin % GPIO_PUPD_CTRL_REG_CTRL_PIN_NUM;

    if (gpio_pupd) {
        gpio_register_base->GPIO_PUPD_REGISTER[no].GPIO_PUPD_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    } else {
        gpio_register_base->GPIO_PUPD_REGISTER[no].GPIO_PUPD_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    }

    if (gpio_r0) {
        gpio_register_base->GPIO_RESEN0_REGISTER[no].GPIO_RESEN0_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    } else {
        gpio_register_base->GPIO_RESEN0_REGISTER[no].GPIO_RESEN0_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    }

    if (gpio_r1) {
        gpio_register_base->GPIO_RESEN1_REGISTER[no].GPIO_RESEN1_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    } else {
        gpio_register_base->GPIO_RESEN1_REGISTER[no].GPIO_RESEN1_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder);
    }
    return HAL_GPIO_STATUS_OK;
}
#endif


#ifdef HAL_GPIO_FEATURE_HIGH_Z
hal_gpio_status_t hal_gpio_set_high_impedance(hal_gpio_pin_t gpio_pin)
{
    uint32_t no1, no2, no3;
    uint32_t remainder1, remainder2, remainder3;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no1 = gpio_pin / GPIO_REG_CTRL_PIN_NUM_OF_32;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder1 = gpio_pin % GPIO_REG_CTRL_PIN_NUM_OF_32;

    /* get the register number corresponding to the pin as one register can control 16 pins*/
    no2 = gpio_pin / GPIO_TDSEL_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 16 pins*/
    remainder2 = gpio_pin % GPIO_TDSEL_REG_CTRL_PIN_NUM;

    /* get the register number corresponding to the pin as one register can control 8 pins*/
    no3 = gpio_pin / GPIO_MODE_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 8 pins*/
    remainder3 = gpio_pin % GPIO_MODE_REG_CTRL_PIN_NUM;

    /* set GPIO mode of pin */
    gpio_register_base->GPIO_MODE_REGISTER[no3].GPIO_MODE_CLR = (GPIO_REG_FOUR_BIT_SET_CLR << (remainder3 * GPIO_MODE_FUNCTION_CTRL_BITS));

    /* set input direction of pin */
    gpio_register_base->GPIO_DIR_REGISTER[no1].GPIO_DIR_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);

    /* disable input buffer enable function of pin */
    gpio_register_base->GPIO_IES_REGISTER[no1].GPIO_IES_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);

    /* disable pull function of pin */
    if (is_pin_with_pullsel(gpio_pin)) {
        gpio_register_base->GPIO_PULLEN_REGISTER[no1].GPIO_PULLEN_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);
    }
#ifdef HAL_GPIO_FEATURE_PUPD
    else {
        gpio_register_base->GPIO_PUPD_REGISTER[no1].GPIO_PUPD_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);
        gpio_register_base->GPIO_RESEN0_REGISTER[no1].GPIO_RESEN0_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);
        gpio_register_base->GPIO_RESEN1_REGISTER[no1].GPIO_RESEN1_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);
    }
#endif

    /* disable schmitt trigger of pin */
    gpio_register_base->GPIO_SMT_REGISTER[no1].GPIO_SMT_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);

    /* disable slew rate of pin*/
    gpio_register_base->GPIO_SR_REGISTER[no1].GPIO_SR_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);

    /* disable TDsel of pin as one register can control 16 pins */
    gpio_register_base->GPIO_TDSEL_REGISTER[no2].GPIO_TDSEL_CLR = (GPIO_REG_TWO_BIT_SET_CLR << (remainder2 * GPIO_TDSEL_FUNCTION_CTRL_BITS));

    return HAL_GPIO_STATUS_OK;
}


hal_gpio_status_t hal_gpio_clear_high_impedance(hal_gpio_pin_t gpio_pin)
{
    uint32_t no1, no2;
    uint32_t remainder1, remainder2;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 32 pins*/
    no1 = gpio_pin / GPIO_REG_CTRL_PIN_NUM_OF_32;

    /* get the bit offset within the register as one register can control 32 pins*/
    remainder1 = gpio_pin % GPIO_REG_CTRL_PIN_NUM_OF_32;

    /* get the register number corresponding to the pin as one register can control 8 pins*/
    no2 = gpio_pin / GPIO_MODE_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 8 pins*/
    remainder2 = gpio_pin % GPIO_MODE_REG_CTRL_PIN_NUM;

    /* set GPIO mode of pin. */
    gpio_register_base->GPIO_MODE_REGISTER[no2].GPIO_MODE_CLR = (GPIO_REG_FOUR_BIT_SET_CLR << (remainder2 * GPIO_MODE_FUNCTION_CTRL_BITS));

    /* set input direction of pin. */
    gpio_register_base->GPIO_DIR_REGISTER[no1].GPIO_DIR_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);

    /* enable pull down of pin. */
    if (is_pin_with_pullsel(gpio_pin)) {
        gpio_register_base->GPIO_PULLEN_REGISTER[no1].GPIO_PULLEN_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);
        gpio_register_base->GPIO_PULLSEL_REGISTER[no1].GPIO_PULLSEL_CLR = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);
    }
#ifdef HAL_GPIO_FEATURE_PUPD
    else {
        gpio_register_base->GPIO_PUPD_REGISTER[no1].GPIO_PUPD_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);
        gpio_register_base->GPIO_RESEN0_REGISTER[no1].GPIO_RESEN0_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);
        gpio_register_base->GPIO_RESEN1_REGISTER[no1].GPIO_RESEN1_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);
    }
#endif

    /* enable input buffer enable function of pin. */
    gpio_register_base->GPIO_IES_REGISTER[no1].GPIO_IES_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);

    /* disable slew rate of pin. */
    gpio_register_base->GPIO_SR_REGISTER[no1].GPIO_SR_SET = (GPIO_REG_ONE_BIT_SET_CLR << remainder1);


    return HAL_GPIO_STATUS_OK;
}
#endif

#ifdef HAL_GPIO_FEATURE_SET_DRIVING
hal_gpio_status_t hal_gpio_set_driving_current(hal_gpio_pin_t gpio_pin, hal_gpio_driving_current_t driving)
{
    uint32_t no;
    uint32_t remainder;

    if ((gpio_pin >= HAL_GPIO_MAX)) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* get the register number corresponding to the pin as one register can control 16 pins*/
    no = gpio_pin / GPIO_DRV_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 16 pins*/
    remainder = gpio_pin % GPIO_DRV_REG_CTRL_PIN_NUM;

    gpio_register_base->GPIO_DRV_REGISTER[no].GPIO_DRV_CLR = (GPIO_REG_TWO_BIT_SET_CLR << (remainder * GPIO_DRV_FUNCTION_CTRL_BITS));
    gpio_register_base->GPIO_DRV_REGISTER[no].GPIO_DRV_SET = (driving << (remainder * GPIO_DRV_FUNCTION_CTRL_BITS));
    return HAL_GPIO_STATUS_OK;
}


hal_gpio_status_t hal_gpio_get_driving_current(hal_gpio_pin_t gpio_pin, hal_gpio_driving_current_t *driving)
{
    uint32_t no;
    uint32_t remainder;
    uint32_t temp;

    if ((gpio_pin >= HAL_GPIO_MAX)) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    if (NULL == driving) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    /* get the register number corresponding to the pin as one register can control 16 pins*/
    no = gpio_pin / GPIO_DRV_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 16 pins*/
    remainder = gpio_pin % GPIO_DRV_REG_CTRL_PIN_NUM;

    temp = gpio_register_base->GPIO_DRV_REGISTER[no].GPIO_DRV;
    temp = temp & (GPIO_REG_TWO_BIT_SET_CLR << (remainder * GPIO_DRV_FUNCTION_CTRL_BITS));
    temp = (temp >> (remainder * GPIO_DRV_FUNCTION_CTRL_BITS));

    /* translate the bit value to driving strength data */
    *driving = (hal_gpio_driving_current_t)((temp + 1) * 4);

    return HAL_GPIO_STATUS_OK;
}
#endif

#ifdef __cplusplus
}
#endif

#endif  /* HAL_GPIO_MODULE_ENABLED */

