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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "type_def.h"
#include "nvic.h"
#include "cos_api.h"
#include "hal_gdma.h"
#include "hal_nvic.h"
#include "hal_lp.h"
#include "dma_sw.h"
#include "misc.h"

#include "connsys_driver.h"

#include "connsys_profile.h"
#include "connsys_bus.h"
#include "connsys_util.h"
#include "mt_cmd_fmt.h"
#include <stdint.h>
#include "memory_attribute.h"
#include "hal.h"
#include "hal_clock.h"
#ifdef HAL_DCXO_MODULE_ENABLED
#include "hal_dcxo.h"
#endif

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
#include "gpt.h"
#endif
#include "syslog.h"
#if (CFG_WIFI_HIF_GDMA_EN == 1)
#include "dma_hw.h"
#endif

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
#include "net_task.h"
#endif

#if defined (MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
#include "context_dump.h"
#endif

#include "connsys_adapter.h"
#ifdef MTK_WIFI_ROM_ENABLE
#include "patch_config_ram.h"
#include "hal_spm.h"
#endif
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_spm.h"
#include "hal_eint.h"
#include "hal_clock.h"
extern uint32_t MACLPSState;
extern void eint_ack_interrupt(uint32_t eint_number);

#define EINT_MASK_SET(source_pin)           *((volatile uint32_t *)0xA2100328) |= 1 << source_pin;
#define EINT_WACKUP_MASK_CLR(source_pin)    *((volatile uint32_t *)0xA2100350) |= 1 << source_pin;
#endif


#ifdef MTK_WIFI_SLIM_ENABLE
/* anthony */
#define OVERRIDE_LOG
#endif

#include "connsys_log.h"


#ifndef MTK_WIFI_SLIM_ENABLE
void (*rx_interrupt_handle)(int32_t) = NULL;
#else
ATTR_ZIDATA_IN_TCM void (*rx_interrupt_handle)(int32_t) = NULL;
#endif

#ifndef MTK_WIFI_SLIM_ENABLE
struct connsys_func g_connsys_func;
#ifdef MTK_MINICLI_ENABLE
static uint32_t g_whisr_val = 0;
#endif
#else
ATTR_ZIDATA_IN_TCM struct connsys_func g_connsys_func;
#ifdef MTK_MINICLI_ENABLE
ATTR_ZIDATA_IN_TCM static uint32_t g_whisr_val = 0;
#endif
#endif

extern bool hal_clock_fxo_is_26m(void);
#ifndef MTK_WIFI_SLIM_ENABLE
#if defined(__ICCARM__)
ATTR_4BYTE_ALIGN enhance_mode_data_struct_t g_last_enhance_mode_data_struct;
#else
enhance_mode_data_struct_t g_last_enhance_mode_data_struct __attribute__((aligned(4)));
#endif
#else
#if defined(__ICCARM__)
ATTR_ZIDATA_IN_TCM ATTR_4BYTE_ALIGN enhance_mode_data_struct_t g_last_enhance_mode_data_struct;
#else
ATTR_ZIDATA_IN_TCM enhance_mode_data_struct_t g_last_enhance_mode_data_struct __attribute__((aligned(4)));
#endif
#endif


#ifndef MTK_WIFI_SLIM_ENABLE
wifi_hif_tx_flow_control_t g_hif_tx_flow_control_stat;
uint8_t g_hif_tx_flow_ctrl_en = 0;
connsys_balance_ctr_t g_balance_ctr;
#else
ATTR_ZIDATA_IN_TCM wifi_hif_tx_flow_control_t g_hif_tx_flow_control_stat;
ATTR_ZIDATA_IN_TCM uint8_t g_hif_tx_flow_ctrl_en = 0;
ATTR_ZIDATA_IN_TCM connsys_balance_ctr_t g_balance_ctr;
#endif

#ifndef MTK_WIFI_SLIM_ENABLE
connsys_ops_t connsys_ops = NULL;
#else
ATTR_ZIDATA_IN_TCM connsys_ops_t connsys_ops = NULL;
#endif

#ifndef MTK_WIFI_SLIM_ENABLE
#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1 || defined(MTK_BSP_LOOPBACK_ENABLE))
connsys_measure_time_t g_connsys_time_current;
connsys_measure_time_t g_connsys_time_last;
connsys_measure_time_t g_connsys_time_max;
#endif
#else
#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1 || defined(MTK_BSP_LOOPBACK_ENABLE))
ATTR_ZIDATA_IN_TCM connsys_measure_time_t g_connsys_time_current;
ATTR_ZIDATA_IN_TCM connsys_measure_time_t g_connsys_time_last;
ATTR_ZIDATA_IN_TCM connsys_measure_time_t g_connsys_time_max;
#endif
#endif

#ifndef MTK_WIFI_SLIM_ENABLE
connsys_stat_t g_connsys_stat;
uint32_t g_connsys_debug_feature = 0;
#else
ATTR_ZIDATA_IN_TCM connsys_stat_t g_connsys_stat;
ATTR_ZIDATA_IN_TCM uint32_t g_connsys_debug_feature = 0;
#endif

#if defined(MTK_HAL_LOWPOWER_ENABLE)
extern int8_t (*ptr_lp_connsys_get_own_enable_int)(void);
extern int8_t (*ptr_lp_connsys_give_n9_own)(void);
extern uint8_t (*ptr_connsys_get_ownership)(void);
#endif

#ifdef MTK_MINICLI_ENABLE
void connsys_print_stat(void)
{
    connsys_stat_t stat;
    uint32_t flags;
    uint32_t port = 0;

    local_irq_save(flags);
    memcpy(&stat, &g_connsys_stat, sizeof(connsys_stat_t));
    local_irq_restore(flags);

    printf("==>connsys_print_stat\n");
    printf("number of interrupt: %u\n", (unsigned int)stat.number_of_int);
    printf("number of tx interrupt: %u\n", (unsigned int)stat.num_of_tx_int);
    printf("number of rx interrupt: %u\n", (unsigned int)stat.num_of_rx_int);
    printf("number of tx wait: %u\n", (unsigned int)stat.num_of_tx_wait);
    printf("number of abnormal interrupt: %u\n", (unsigned int)stat.number_of_abnormal_int);
    printf("number fw_own_back: %u\n", (unsigned int)stat.number_of_fw_own_back);

    for (port = 0; port < NUM_OF_WIFI_HIF_RX_PORT; port++) {
        printf("port index:%u\n", (unsigned int)port);
        printf("\trx_packet_cnt:%u\n", (unsigned int)stat.rx_port[port].rx_packet_cnt);
        printf("\trx_invalid_sz_packet_cnt:%u\n", (unsigned int)stat.rx_port[port].rx_invalid_sz_packet_cnt);
        printf("\trx_max_invalid_sz:%u\n", (unsigned int)stat.rx_port[port].rx_max_invalid_sz);
        printf("\trx_error_cnt:%u\n", (unsigned int)stat.rx_port[port].rx_error_cnt);
        printf("\trx_allocate_fail_cnt:%u\n", (unsigned int)stat.rx_port[port].rx_allocate_fail_cnt);
    }
}

void connsys_dump_cr(void)
{
    uint32_t value;

    printf("==>connsys_dump_cr\n");

    connsys_cr_read(WHLPCR, &value);
    printf("WHLPCR(0x%x): 0x%x\n", WHLPCR, (unsigned int)value);
    printf("\tW_INT_EN_SET:%u\n", (value & W_INT_EN_SET) ? 1 : 0);
    printf("\tW_INT_EN_CLR:%u\n", (value & W_INT_EN_CLR) ? 1 : 0);

    connsys_cr_read(WHCR, &value);
    printf("WHCR(0x%x): 0x%x\n", WHCR, (unsigned int)value);
    printf("\tRX_ENHANCE_MODE:%u\n", (value & RX_ENHANCE_MODE) ? 1 : 0);
    printf("\tMAX_HIF_RX_LEN_NUM:%u\n", (unsigned int)((value & MAX_HIF_RX_LEN_NUM_MASK) >> MAX_HIF_RX_LEN_NUM_OFFSET));

    printf("dump last intr enhance mode whisr\n");
    value = g_last_enhance_mode_data_struct.WHISR_reg_val;
    printf("WHISR(0x%x): 0x%x\n", WHISR, (unsigned int)value);
    printf("\tTX_DONE_INT:%u\n", (value & TX_DONE_INT) ? 1 : 0);
    printf("\tRX0_DONE_INT:%u\n", (value & RX0_DONE_INT) ? 1 : 0);
    printf("\tRX1_DONE_INT:%u\n", (value & RX1_DONE_INT) ? 1 : 0);
    printf("\tABNORMAL_INT:%u\n", (value & ABNORMAL_INT) ? 1 : 0);
    printf("\tFW_OWN_BACK_INT:%u\n", (value & FW_OWN_BACK_INT) ? 1 : 0);

    printf("dump last read whisr by cmd\n");
    value = g_whisr_val;
    printf("WHISR(0x%x): 0x%x\n", WHISR, (unsigned int)value);
    printf("\tTX_DONE_INT:%u\n", (value & TX_DONE_INT) ? 1 : 0);
    printf("\tRX0_DONE_INT:%u\n", (value & RX0_DONE_INT) ? 1 : 0);
    printf("\tRX1_DONE_INT:%u\n", (value & RX1_DONE_INT) ? 1 : 0);
    printf("\tABNORMAL_INT:%u\n", (value & ABNORMAL_INT) ? 1 : 0);
    printf("\tFW_OWN_BACK_INT:%u\n", (value & FW_OWN_BACK_INT) ? 1 : 0);


    connsys_cr_read(WHIER, &value);
    printf("WHIER(0x%x): 0x%x\n", WHIER, (unsigned int)value);
    printf("\tTX_DONE_INT_EN :%u\n", (value & TX_DONE_INT_EN) ? 1 : 0);
    printf("\tRX0_DONE_INT_EN :%u\n", (value & RX0_DONE_INT_EN) ? 1 : 0);
    printf("\tRX1_DONE_INT_EN :%u\n", (value & RX1_DONE_INT_EN) ? 1 : 0);
    printf("\tABNORMAL_INT_EN :%u\n", (value & ABNORMAL_INT_EN) ? 1 : 0);
    printf("\tFW_OWN_BACK_INT_EN :%u\n", (value & FW_OWN_BACK_INT_EN) ? 1 : 0);

    connsys_cr_read(WASR, &value);
    printf("WASR(0x%x): 0x%x\n", WASR, (unsigned int)value);
    printf("\tTX1_OVERFLOW :%u\n", (value & TX1_OVERFLOW) ? 1 : 0);
    printf("\tRX0_UNDERFLOW :%u\n", (value & RX0_UNDERFLOW) ? 1 : 0);
    printf("\tRX1_UNDERFLOW :%u\n", (value & RX1_UNDERFLOW) ? 1 : 0);
    printf("\tWASR2 :%u\n", (value & WASR_WASR2) ? 1 : 0);

    connsys_cr_read(WASR2, &value);
    printf("WASR2(0x%x): 0x%x (Note: all fields are read clear)\n", WASR2, (unsigned int)value);
    printf("\tWASR2_CMD53_ERR_CNT :%u\n",
           (unsigned int)((value & WASR2_CMD53_ERR_CNT_MASK) >> WASR2_CMD53_ERR_CNT_OFFSET));
    printf("\tWASR2_CMD53_ERR :%u\n", (value & WASR2_CMD53_ERR) ? 1 : 0);
    printf("\tWASR2_CMD53_RD_TIMEOUT :%u\n", (value & WASR2_CMD53_RD_TIMEOUT) ? 1 : 0);
    printf("\tWASR2_CMD53_WR_TIMEOUT :%u\n", (value & WASR2_CMD53_WR_TIMEOUT) ? 1 : 0);
    printf("\tWASR2_FW_OWN_INVALID_ACCESS :%u\n", (value & WASR2_FW_OWN_INVALID_ACCESS) ? 1 : 0);
}

void connsys_dump_whisr(void)
{
    uint32_t value;

    printf("==>connsys_dump_whisr\n");
    connsys_cr_read(WHISR, &value);
    g_whisr_val = value;
    printf("WHISR(0x%x): 0x%x\n", WHISR, (unsigned int)value);
    printf("\tTX_DONE_INT:%u\n", (value & TX_DONE_INT) ? 1 : 0);
    printf("\tRX0_DONE_INT:%u\n", (value & RX0_DONE_INT) ? 1 : 0);
    printf("\tRX1_DONE_INT:%u\n", (value & RX1_DONE_INT) ? 1 : 0);
    printf("\tABNORMAL_INT:%u\n", (value & ABNORMAL_INT) ? 1 : 0);
    printf("\tFW_OWN_BACK_INT:%u\n", (value & FW_OWN_BACK_INT) ? 1 : 0);
}
#endif


#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1 || defined(MTK_BSP_LOOPBACK_ENABLE))
uint32_t connsys_measure_convert2ms(uint32_t count)
{
    uint32_t ret;
    if (count) {
        ret = count / (192 * 1000);
    } else {
        ret = 0;
    }
    return ret;
}

void connsys_measure_time_init(void)
{
    CM4_GPT4Init();
    GPT_Stop(4);
    GPT_Start(4);
}

void connsys_measure_time_finish(void)
{
    uint32_t flags;

    local_irq_save(flags);
    memcpy(&g_connsys_time_last,
           &g_connsys_time_current,
           sizeof(connsys_measure_time_t));

#if 0
    for (i = 0; i < CFG_CONNSYS_MEASURE_TIME_NUM; i++) {
        if (g_connsys_time_max.time[i] < g_connsys_time_last.time[i]) {
            g_connsys_time_max.time[i] = g_connsys_time_last.time[i];
        }
    }

    for (i = 0; i < NUM_OF_WIFI_HIF_RX_PORT; i++) {
        if (g_connsys_time_max.pkt_cnt[i] < g_connsys_time_last.pkt_cnt[i]) {
            g_connsys_time_max.pkt_cnt[i] = g_connsys_time_last.pkt_cnt[i];
        }
    }
#endif
    if (g_connsys_time_max.time[3] < g_connsys_time_last.time[3]) {
        memcpy(&g_connsys_time_max,
               &g_connsys_time_last,
               sizeof(connsys_measure_time_t));
    }
    local_irq_restore(flags);
}


void connsys_measure_time_set(uint8_t idx)
{
    uint32_t flags;
    local_irq_save(flags);
    g_connsys_time_current.time[idx] = GPT_return_current_count(4);
    local_irq_restore(flags);
}

void connsys_measure_time_set_pkt_time(uint8_t port, uint8_t idx)
{
    uint32_t flags;
    local_irq_save(flags);
    g_connsys_time_current.pkt_time[port][idx] = GPT_return_current_count(4);
    local_irq_restore(flags);
}

void connsys_measure_time_set_pkt_num(uint8_t port, uint8_t num)
{
    uint32_t flags;
    local_irq_save(flags);
    g_connsys_time_current.pkt_cnt[port] = num;
    local_irq_restore(flags);
}

#ifdef MTK_MINICLI_ENABLE
void connsys_measure_time_show(void)
{
    connsys_measure_time_t tmp;
    uint32_t flags;
    uint32_t i;
    uint32_t port;

    printf("==>connsys_measure_time_show\n");
    local_irq_save(flags);
    memcpy(&tmp, &g_connsys_time_last, sizeof(connsys_measure_time_t));
    local_irq_restore(flags);
    printf("last measure time:\n");
    for (i = 0; i < CFG_CONNSYS_MEASURE_TIME_NUM; i++) {
        printf("%u: %u count, %u ms\n",
               (unsigned int)i,
               (unsigned int)tmp.time[i],
               (unsigned int)connsys_measure_convert2ms(tmp.time[i]));
    }
    printf("detail pkt time:\n");

    for (port = 0; port < NUM_OF_WIFI_HIF_RX_PORT; port ++) {
        printf("p[%u]:\n", (unsigned int)port);
        printf("\tmax pkt num: %u\n", (unsigned int)tmp.pkt_cnt[port]);
        for (i = 0; i < tmp.pkt_cnt[port]; i++) {
            printf("\tidx %u, %u count, %u ms\n",
                   (unsigned int)i,
                   (unsigned int)tmp.pkt_time[port][i],
                   (unsigned int)connsys_measure_convert2ms(tmp.pkt_time[port][i]));
        }
    }

    local_irq_save(flags);
    memcpy(&tmp, &g_connsys_time_max, sizeof(connsys_measure_time_t));
    local_irq_restore(flags);
    printf("max measure time:\n");
    for (i = 0; i < CFG_CONNSYS_MEASURE_TIME_NUM; i++) {
        printf("%u: %u count, %u ms\n",
               (unsigned int)i,
               (unsigned int)tmp.time[i],
               (unsigned int)connsys_measure_convert2ms(tmp.time[i]));
    }
    printf("detail pkt time:\n");

    for (port = 0; port < NUM_OF_WIFI_HIF_RX_PORT; port ++) {
        printf("p[%u]:\n", (unsigned int)port);
        printf("\tmax pkt num: %u\n", (unsigned int)tmp.pkt_cnt[port]);
        for (i = 0; i < tmp.pkt_cnt[port]; i++) {
            printf("\t%u: %u count, %u ms\n",
                   (unsigned int)i,
                   (unsigned int)tmp.pkt_time[port][i],
                   (unsigned int)connsys_measure_convert2ms(tmp.pkt_time[port][i]));
        }
    }
}
#endif // end of MTK_MINICLI_ENABLE

#endif
#ifdef MT5932_SINGLE_CONTEXT
void wifi_crit_enter(u_int32 *flags)
{
    local_irq_save(*flags);
    //INFO(connsys, "En\n");
#if 0
    if (net_pkt_semaphore == NULL) {
        ERR(connsys, "ERROR! inband not init yet, lock fail\n");
        return;
    }

    if (xSemaphoreTake(net_pkt_semaphore, 10) == 0)  {
 //  if (xSemaphoreTakeFromISR(net_pkt_semaphore, 0) == 0) {
        ERR(connsys, "ERROR! semaphore take fail, unlock fail\n");
        return;
    }
#endif
    //INFO(connsys, "Ex\n");
}
void wifi_crit_exit(u_int32 *flags)
{
    local_irq_restore(*flags);
//    INFO(connsys, "Ex1\n");
#if 0
    if (net_pkt_semaphore == NULL) {
        ERR(connsys, "ERROR! inband not init yet, unlock fail\n");
        return;
    }
    xSemaphoreGive(net_pkt_semaphore);
#endif
    //INFO(connsys, "Ex2\n");
}
#endif

uint32_t connsys_get_stat_int_count(void)
{
    uint32_t flags;
    uint32_t number_of_int;

    local_irq_save(flags);
    number_of_int = g_connsys_stat.number_of_int;
    local_irq_restore(flags);

    return number_of_int;
}

int32_t connsys_cccr_read(uint32_t addr, uint8_t *value)
{
    int32_t ret = 0;
    struct connsys_func *dev_func = &g_connsys_func;

    connsys_bus_get_bus(dev_func);
    *value = connsys_bus_fn0_read_byte(dev_func, addr, &ret);
    connsys_bus_release_bus(dev_func);
    if (ret) {
        ERR(connsys, "<<%s>> Read CCCR 0x%02x failed. Error = %d\n",
              __FUNCTION__,
              (unsigned int)addr,
              (int)ret);
    }
    return ret;
}

int32_t connsys_cccr_write(uint32_t addr, uint8_t value)
{
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    struct connsys_func *dev_func = &g_connsys_func;

    connsys_bus_get_bus(dev_func);
    connsys_bus_fn0_write_byte(dev_func, value, addr, &ret);
    connsys_bus_release_bus(dev_func);

    if (ret) {
        ERR(connsys, "<<%s>> Write register 0x%02x failed. Error = %d\n",
              __FUNCTION__,
              (unsigned int)addr,
              (int)ret);
    }

    return ret;
}

int32_t connsys_cr_read(uint32_t addr, uint32_t *value)
{

    int32_t ret = CONNSYS_STATUS_SUCCESS;
    struct connsys_func *dev_func = &g_connsys_func;

    connsys_bus_get_bus(dev_func);
    *value = connsys_bus_readl(dev_func, addr, &ret);
    connsys_bus_release_bus(dev_func);

    /* ret never be non-zero
        if (ret)
        {
            ERR(connsys, "<<%s>> Read register 0x%08x failed. Error = %d\n",
                __FUNCTION__,
                (unsigned int)addr,
                (int)ret);
        }
        */

    return ret;
}

int32_t connsys_cr_write(uint32_t addr, uint32_t value)
{
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    struct connsys_func *dev_func = &g_connsys_func;

    connsys_bus_get_bus(dev_func);
    connsys_bus_writel(dev_func, value, addr, &ret);
    connsys_bus_release_bus(dev_func);

    /* comment this statement because ret never be non-zero value
        if (ret)
        {
            ERR(connsys, "<<%s>> Write register 0x%08x failed. Error = %d\n",
                __FUNCTION__,
                (unsigned int)addr,
                (int)ret);
        }
    */
    return ret;
}

/*
   use to read data
   @ addr:  WRDR0 / WRDR1/ WHISR
*/
int32_t connsys_fifo_read(uint32_t addr, uint8_t *buf, size_t size)
{
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    struct connsys_func *dev_func = &g_connsys_func;

    ret = connsys_bus_read_port(dev_func, buf, addr, size);
    if (ret) {
        ERR(connsys, "<<%s>> SDIO read data failed. Error = %d \n", __FUNCTION__, (int)ret);
        ret = CONNSYS_STATUS_FAIL;
    }
    return ret;
}

/*use to write data*/
int32_t connsys_fifo_write(uint8_t *buf, size_t size)
{
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    struct connsys_func *dev_func = &g_connsys_func;
    ret = connsys_bus_write_port(dev_func, WTDR1, buf, size);
    return ret;
}

void connsys_driver_interrupt(struct connsys_func *func)
{
    (void) func; // avoid compiler warning
    if (connsys_ops && connsys_ops->enqueue_bottom_half) {
        // 0. Disable HIF TX/RX Interrupt
        connsys_disable_interrupt();       //connsys_cr_write(WHLPCR, W_INT_EN_CLR)
        connsys_disable_whier_trx_int();   //WHIER, val &= ~(TX_DONE_INT_EN | RX0_DONE_INT_EN | RX1_DONE_INT_EN);
        connsys_ops->enqueue_bottom_half(0, (PNETFUNC)connsys_util_intr_enhance_mode_receive_data);
    }

    return;
}

#if (CFG_CONNSYS_POLLING_IRQ_EN == 1)
uint32_t connsys_polling_irq(int32_t max_tries)
{
    int32_t count = 0;

    INFO(connsys, "Waiting for IRQ...\n");

    while (!g_connsys_func.irq_info.irq_assert) {

        //g_connsys_func.irq_handler(&g_connsys_func);
        if (count++ > max_tries) {
            break;
        }
        cos_delay_time(CONNSYS_POLLING_DELAY_TIME);; //delay 50us
    }

    if (g_connsys_func.irq_info.irq_assert) {
        g_connsys_func.irq_info.irq_assert = 0;
        INFO(connsys, "WHISR: 0x%x\n", (unsigned int)g_connsys_func.irq_info.irq_data.WHISR_reg_val);
        return 1; //g_connsys_func.irq_info.irq_data.WHISR_reg_val;
    }
    INFO(connsys, "==> No interrupt for %d loops.FUN[%d](0x%p) irq_asset=%d\n",
          (int)count,
          (int)g_connsys_func.num,
          (void *)&g_connsys_func,
          (int)g_connsys_func.irq_info.irq_assert);
    return 0;
}
#endif

uint8_t connsys_giveup_ownership(void)
{
    uint32_t value, counter = 0, addr;
    int32_t ret;
    uint8_t status = TRUE;

    addr = WHLPCR;

    // 1. check if it is already driver own
    ret = connsys_cr_read(addr, &value);
    if (ret) {
        ERR(connsys, "Ownership read failed.\n");
        goto err;
    }

    if (GET_W_FW_OWN_REQ_SET(value)) {
        // give up owner ship
        value |= W_FW_OWN_REQ_SET;
        ret = connsys_cr_write(addr, value);
        if (ret) {
            LOG_E(connsys, "request owner ship write fail\n");
            goto err;
        }

        // check if the ownership in FW
        counter = 0;
        while (GET_W_FW_OWN_REQ_SET(value)) {
            if (counter > 200000) { // wait for at least 1 second
                status = FALSE;
                break;
            }
            //udelay(50); //delay 50us

            ret = connsys_cr_read(addr, &value);
            if (ret) {
                goto err;
            }
            counter++;
        }
    }

err:
    /*io read/write fail*/
    if (ret) {
        status = FALSE;
    }
    LOG_I(connsys, "Give up Own Bit, succ(%d), value(0x%x)\n", (int)status, (int)value);
    return status;
}

uint8_t connsys_get_ownership(void)
{
    uint32_t value = 0, counter = 0, addr = 0;
    int32_t ret = 0;
    uint8_t status = TRUE;

    addr = WHLPCR;
    // 1. check if it is already driver own
    ret = connsys_cr_read(addr, &value);
    if (ret) {
        ERR(connsys, "Read WHLPCR org value fail.\n");
        goto err;
    }

    if (!GET_W_FW_OWN_REQ_SET(value)) {
        // request owner ship
        value |= W_FW_OWN_REQ_CLR;
        ret = connsys_cr_write(addr, value);
        if (ret) {
            ERR(connsys, "Write WHLPCR own_req fail\n");
            goto err;
        }

        // check if the ownership back
        counter = 0;
        while (!GET_W_FW_OWN_REQ_SET(value)) {
#if (MT7682_WSAP00041423 == 1)			
            if (counter > 2000000) { // wait for at least 1 second
#else
			if (counter > 200000) { // wait for at least 1 second
#endif
                status = FALSE;
                break;
            }
            //udelay(50); //delay 50us

            ret = connsys_cr_read(addr, &value);
            if (ret) {
                LOG_E(connsys, "Read WHLPCR end value fail.\n");
                goto err;
            }
            counter++;
        }
    }

err:
    /*io read/write fail*/
    if (ret) {
        status = FALSE;
    }
    LOG_I(connsys, "Get Own Bit, succ(%d), value(0x%x)\n", (int)status, (int)value);
    return status;
}

int32_t connsys_abnormal_interrupt_check(uint32_t value)
{
    uint32_t reason;
    int32_t err = CONNSYS_STATUS_SUCCESS;

    if (value & ABNORMAL_INT) {
        err = connsys_cr_read(WASR, &reason);
        if (err) {
            ERR(connsys, "<<%s>> Read WASR failed. Error = %d.\n", __FUNCTION__, (int)err);
        } else {
            INFO(connsys, "<<%s>> Read WASR = %08x.\n", __FUNCTION__, (unsigned int)reason);
        }
    }
    return err;
}


/*
void connsys_irq_handler(hal_nvic_irq_t irq_number)
{
    uint8_t input[LEN_INT_ENHANCE_MODE] __attribute__ ((aligned (4)));
    uint32_t flags;
    enhance_mode_data_struct_t *p_int_enhance;
#if (CONNSYS_DEBUG_MODE_EN == 1)
    g_connsys_stat.number_of_int ++;
#endif
    connsys_disable_interrupt();

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
    connsys_measure_time_init();
#endif
    connsys_disable_whier_trx_int();

    memset(input, 0, sizeof(input));
    connsys_fifo_read(WHISR, (uint8_t *)input, LEN_INT_ENHANCE_MODE);
    p_int_enhance = (enhance_mode_data_struct_t *)input;

    if (p_int_enhance->WHISR_reg_val == 0)
    {
        ERR(connsys, "WHISR=0x%x, abort...\n", (unsigned int)p_int_enhance->WHISR_reg_val);
        connsys_cr_write(WHLPCR, W_INT_EN_SET);
        connsys_enable_interrupt();
        return;
    }

#if (CONNSYS_DEBUG_MODE_EN == 1)
    if ((p_int_enhance->WHISR_reg_val) & ABNORMAL_INT)
    {
        uint32_t reg1, reg2;
        connsys_cr_read(WASR, &reg1);
        connsys_cr_read(0xE0, &reg2);
        local_irq_save(flags);
        g_connsys_stat.number_of_abnormal_int ++;
        local_irq_restore(flags);
        //ERR(connsys, "Abnormal(0x%x), 0xE0(0x%x)\n", (unsigned int)reg1, (unsigned int)reg2);
    }

    if ((p_int_enhance->WHISR_reg_val) & FW_OWN_BACK_INT)
    {
        uint32_t reg1;
        connsys_cr_read(WASR, &reg1);
        local_irq_save(flags);
        g_connsys_stat.number_of_fw_own_back ++;
        local_irq_restore(flags);
        //ERR(connsys, "FW_OWN_BACK_INT: WASR(0x%x)\n", (unsigned int)reg1);
    }
#endif

    if ((p_int_enhance->WHISR_reg_val) & TX_DONE_INT)
    {
#if (CONNSYS_DEBUG_MODE_EN == 1)
        g_connsys_stat.num_of_tx_int ++;
#endif
        connsys_tx_flow_control_update_free_page_cnt(p_int_enhance);
    }

    if ((p_int_enhance->WHISR_reg_val) & (RX0_DONE_INT | RX1_DONE_INT))
    {
        if (((p_int_enhance->WHISR_reg_val) & TX_DONE_INT) == 0)
        {
            connsys_tx_flow_control_update_free_page_cnt(p_int_enhance);
        }
#if (CONNSYS_DEBUG_MODE_EN == 1)
        g_connsys_stat.num_of_rx_int ++;
#endif
        local_irq_save(flags);
        memcpy(&g_last_enhance_mode_data_struct, input, LEN_INT_ENHANCE_MODE);
#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
        g_balance_ctr.fg_rx_wait = TRUE;
#endif
        local_irq_restore(flags);
        g_connsys_func.irq_callback(&g_connsys_func);
    }
    else
    {
        connsys_enalbe_whier_rx_int();
    }
    connsys_enalbe_whier_tx_int();
    connsys_enable_interrupt();
#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
    connsys_measure_time_set(1);
#endif
    return;
}
*/


void connsys_irq_handler(hal_nvic_irq_t irq_number)
{
    // 1. Record Debug Information if need
#if (CONNSYS_DEBUG_MODE_EN == 1)
    g_connsys_stat.number_of_int ++;
#endif

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
    connsys_measure_time_init();
#endif

    // 2. Notify HIF Packet Handler
    g_connsys_func.irq_callback(&g_connsys_func);  //Add to netjob task

    // 3. Enable Interrupt - It will be implemented by netjob callback (TX/RX Packet Handler)

#if (CFG_CONNSYS_MEASURE_INT_TIME_EN == 1)
    connsys_measure_time_set(1);
#endif

    return;
}

void connsys_set_register_value(uint32_t address, short int mask, short int shift, short int value) {
    uint32_t mask_buffer;
    mask_buffer = (~(mask << shift));
    *((volatile uint32_t *)(address)) &=mask_buffer;
    *((volatile uint32_t *)(address)) |=(value << shift);
}

uint32_t connsys_get_register_value(uint32_t address, short int mask, short int shift) {
    uint32_t get_value, mask_buffer;
    mask_buffer = (mask << shift);
    get_value = *((volatile uint32_t *)(address));
    get_value &=mask_buffer;
    get_value = (get_value>> shift);
    return get_value;
}

int32_t connsys_enable_enhance_mode(void)
{
    uint32_t reg_value = 0;

    /* enable RX enhance mode */

    INFO(connsys, "enable connsys enhance mode.\n");

    if (connsys_cr_read(WHCR, &reg_value)) {
        ERR(connsys, "FAIL. read WHCR.\n");
        return -1;
    }
//    reg_value |= W_INT_CLR_CTRL;
    reg_value &= ~MAX_HIF_RX_LEN_NUM_MASK;
    reg_value |= MAX_HIF_RX_LEN_NUM(CFG_MAX_HIF_RX_LEN_NUM);
//    reg_value |= RX_ENHANCE_MODE;

#if (CFG_RPT_OWN_RX_PACKET_LEN_EN == 1)
    reg_value |= RPT_OWN_RX_PACKET_LEN;
#endif

    INFO(connsys, "write 0x%x to WHCR\n", (unsigned int)reg_value);

    if (connsys_cr_write(WHCR, reg_value)) {
        ERR(connsys, "FAIL. write WHCR.\n");
        return -1;
    }
    reg_value = 0;
    if (connsys_cr_read(WHCR, &reg_value)) {
        ERR(connsys, "FAIL. read WHCR.\n");
        return -1;
    } else {
#if 0
        if (!(reg_value & RX_ENHANCE_MODE)) {
            ERR(connsys, "FAIL. write RX_ENHANCE_MODE fail. WHCR = 0x%08x.\n", (unsigned int)reg_value);
            return -1;
        }
#endif
    }
//    INFO(connsys, "Enable enhance mode, WHCR=0x%x\n", (unsigned int)reg_value);

#if (CFG_RPT_OWN_RX_PACKET_LEN_EN == 1)
    reg_value = (RX0_RPT_PKT_LEN(CFG_RX0_RPT_PKT_LEN) |
                 (RX1_RPT_PKT_LEN(CFG_RX1_RPT_PKT_LEN);
    if (connsys_cr_write(WPLRCR, reg_value)) {
        ERR(connsys, "FAIL. write WPLRCR.\n");
        return -1;
    }
#endif
    return 0;
}

#if (CFG_WIFI_HIF_GDMA_EN == 1)
void connsys_enable_dma(void)
{
    INFO(connsys, "==>connsys_enable_dma\n");
    g_connsys_func.use_dma = 1;
}
#endif
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
#include "hal_gdma.h"
void connsys_dma_callback(hal_gdma_event_t event, void  *user_data)
{
    INFO(connsys, "DMA callback\n");
}
// mt7686
#define EINT_SENS_SET0 0xA2100368
#define EINT_SENS_CLR0 0xA2100370
#define EINT_MASK_SET0 0xA2100328
#define EINT_MASK_CLR0 0xA2100330

void SET_EINT_SENS(int num, int value)
{
    unsigned int * pReg;
    if (value == 0) {
        pReg = (unsigned int *)((unsigned int) EINT_SENS_CLR0);
    } else {
        pReg = (unsigned int *)((unsigned int) EINT_SENS_SET0);
    }
    *pReg = (1 << num);
}

void SET_EINT_MASK(int num, int value)
{
    unsigned int * pReg;
    if (value == 0) {
        pReg = (unsigned int *)((unsigned int) EINT_MASK_CLR0);
    } else {
        pReg = (unsigned int *)((unsigned int) EINT_MASK_SET0);
    }
    *pReg = (1 << num);
}
#else
    // MT7687
void connsys_dma_callback()
{
    INFO(connsys, "DMA callback\n");
}
#endif

#if (INTERRUPT_BASED_SUPPORT == 1)
void send_msg_to_wifi_task();

#define LP_HWIS0R            0xc0b1003c//(WIFI_INT_BASE + 0x003C)

void wifi_irq_handler(hal_nvic_irq_t irq_number)
{
    NVIC_DisableIRQ(CONNSYS1_IRQn);
    //INFO(connsys, "0x%x\n", HAL_REG_32(LP_HWIS0R));
    send_msg_to_wifi_task();
    return;
}
void wifi_enable_irq()
{
    NVIC_EnableIRQ(CONNSYS1_IRQn);
}

#include "hal_eint.h"
#if (MT7682_WSAP00023897 == 1)
void wifi_eint_handler(void *parameter)
{
    eint_ack_interrupt(HAL_EINT_MAC);
    hal_eint_unmask(HAL_EINT_MAC);
    send_msg_to_wifi_task();
}
#endif
void register_wifi_intr_handler()
{
    //INFO(connsys, "++++++++++++++\n\n Raghav: Register isr: %d\n+++++++++\n",CONNSYS1_IRQn);
    hal_nvic_register_isr_handler(CONNSYS1_IRQn, wifi_irq_handler);
    NVIC_SetPriority(CONNSYS1_IRQn, CM4_HIF_PRI);
    NVIC_EnableIRQ(CONNSYS1_IRQn);
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
#if (MT7682_WSAP00023897 == 0)
    SET_EINT_SENS(CONNSYS1_IRQn, 1);
    SET_EINT_MASK(CONNSYS1_IRQn, 0);
    //hal_eint_set_trigger_mode(CONNSYS1_IRQn,HAL_EINT_EDGE_RISING);
#else
    hal_eint_config_t config_set;
    config_set.trigger_mode = HAL_EINT_EDGE_FALLING_AND_RISING;
    config_set.debounce_time = 0;
    hal_eint_init(HAL_EINT_MAC,&config_set);
    hal_eint_register_callback(HAL_EINT_MAC,wifi_eint_handler,NULL);
    hal_eint_unmask(HAL_EINT_MAC);   
    
#endif

#endif

}



void connsys_register_wifi_irq()
{
    register_wifi_intr_handler();
}
#endif

#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
void connsys_eint_handler(void *parameter)
{
    hal_eint_unmask(HAL_EINT_HIF);
}

void connsys_suspend_callback(void)
{ 
    if (MACLPSState == 1) {
        /* Enable FW_OWN_BACK_INT interrupt */
        hal_lp_connsys_get_own_enable_int();
        /* Give connsys ownership to N9 */
        hal_lp_connsys_give_n9_own();
    } 
    eint_ack_interrupt(HAL_EINT_MAC);
    hal_eint_unmask(HAL_EINT_MAC);    
    
}

void connsys_resume_callback(void)
{    
    if (MACLPSState == 1) {
        /* re-init connsys for handling inband-cmd response */
        if (FALSE == connsys_get_ownership()) {
            sleep_management_dump_debug_log(SLEEP_MANAGEMENT_DEBUG_LOG_OWNERSHIP_FAIL);
        }
    } 
}
#endif

int32_t connsys_open()
{
    struct connsys_func *func = &g_connsys_func;
    int32_t             ret   = CONNSYS_STATUS_SUCCESS;

#if defined(MTK_HAL_LOWPOWER_ENABLE)
    ptr_lp_connsys_get_own_enable_int   = lp_connsys_get_own_enable_int;
    ptr_lp_connsys_give_n9_own          = lp_connsys_give_n9_own;
    ptr_connsys_get_ownership           = connsys_get_ownership;
#endif

    g_connsys_func.blksize = MY_CONNSYS_BLOCK_SIZE;
    g_connsys_func.num = SDIO_GEN3_FUNCTION_WIFI;
    g_connsys_func.irq_callback = NULL;
#if (CFG_WIFI_HIF_GDMA_EN == 1)
    g_connsys_func.use_dma = 1;
#else
    g_connsys_func.use_dma = 0;
#endif

#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    // Hard code use dma to do MT7686 FW download - need discuss with Michael
    g_connsys_func.use_dma = 1;
#endif
    // Enable GDMA
    if (g_connsys_func.use_dma) {
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
	    hal_gdma_init((hal_gdma_channel_t)CONNSYS_GDMA_CH);
	    hal_gdma_register_callback((hal_gdma_channel_t)CONNSYS_GDMA_CH, connsys_dma_callback, NULL);
	    // Enable GDMA
        hal_clock_enable(HAL_CLOCK_CG_DMA);
#else
        // MT7687
        DMA_Init();
        DMA_Register(CONNSYS_GDMA_CH, connsys_dma_callback);

#endif
    }



    INFO(connsys, "============> SDIO open, (0x%p) use DMA(%d)\n",
          &g_connsys_func, (int)g_connsys_func.use_dma);

    // function enable
    connsys_bus_get_bus(func);
    ret = connsys_bus_enable_func(func);
    connsys_bus_release_bus(func);
    if (ret) {
        ERR(connsys, "<<%s>> Enable function failed. Error = %d.\n", __FUNCTION__, (int)ret);
        goto err;
    }

    // set block size
    connsys_bus_get_bus(func);
    ret = connsys_bus_set_block_size(func, func->blksize);
    connsys_bus_release_bus(func);

    if (ret) {
        ERR(connsys, "<<%s>> Set block size failed. Error = %d.\n", __FUNCTION__, (int)ret);
        goto err;
    }

    // register sdio irq
    connsys_bus_get_bus(func);
    ret = connsys_bus_get_irq(func, &connsys_driver_interrupt); /* Interrupt IRQ handler */
    connsys_bus_release_bus(func);
    if (ret) {
        ERR(connsys, "<<%s>> Claim irq failed. Error = %d.\n", __FUNCTION__, (int)ret);
        goto err;
    }

    // register to MCU IRQ
    hal_nvic_register_isr_handler((hal_nvic_irq_t)CM4_HIF_IRQ, connsys_irq_handler);
    NVIC_SetPriority((IRQn_Type)CM4_HIF_IRQ, CM4_HIF_PRI);
    NVIC_EnableIRQ((IRQn_Type)CM4_HIF_IRQ);
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    // Register HAL_EINT_HIF as wakeup source for CM4 tickless mode.
    hal_eint_config_t config1;
    config1.trigger_mode = HAL_EINT_LEVEL_LOW;
    config1.debounce_time = 0;
    hal_eint_init(HAL_EINT_HIF, &config1); //set EINT trigger mode and debounce time.
    hal_eint_register_callback(HAL_EINT_HIF, connsys_eint_handler, NULL); // register a user callback.
    hal_eint_unmask(HAL_EINT_HIF);  
    
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_CONNNSYS, (sleep_management_suspend_callback_t)connsys_suspend_callback, NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_CONNNSYS, (sleep_management_resume_callback_t)connsys_resume_callback, NULL);
#endif

    // Bug fixed for ownbit initial value (1) error
    connsys_giveup_ownership();

    if (connsys_get_ownership() == FALSE) {
        ERR(connsys, "connsys_get_ownership failed.\n");
        ret = -1;
        goto err;
    }
    if (connsys_enable_enhance_mode() != 0) {
        ERR(connsys, "connsys_enable_enhance_mode failed.\n");
        ret = -1;
        goto err;
    }
    connsys_tx_flow_control_init();

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
    connnsys_balance_init();
#endif

    if (connsys_cr_write(WHIER, (RX0_DONE_INT_EN | RX1_DONE_INT_EN))) {
        ERR(connsys, "FAIL. write WHIER failed (1).\n");
        ret = CONNSYS_STATUS_FAIL;
        goto err;
    } else {
        uint32_t reg_value = 0;
        connsys_cr_read(WHISR, &reg_value);
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
        if ((reg_value & (RX0_DONE_INT_EN))) {
            ERR(connsys, "FAIL. WHISR.RX0_DONE interrupt should be cleared first. (2). WHISR = 0x%08x.\n", (unsigned int)reg_value);
            ret = CONNSYS_STATUS_FAIL;
            goto err;
        }
#else
        if ((reg_value & (RX0_DONE_INT_EN | RX1_DONE_INT_EN))) {
            ERR(connsys, "FAIL. WHISR.RX0/1_DONE interrupt should be cleared first. (2). WHISR = 0x%08x.\n", (unsigned int)reg_value);
            ret = CONNSYS_STATUS_FAIL;
            goto err;
        }
#endif
        reg_value = 0;
        connsys_cr_read(WHIER, &reg_value);
        if (!(reg_value & (RX0_DONE_INT_EN | RX1_DONE_INT_EN))) {
            ERR(connsys, "FAIL. write WHIER failed (2). WHIER = 0x%08x.\n", (unsigned int)reg_value);
            ret = CONNSYS_STATUS_FAIL;
            goto err;
        }
    }
err:
    return ret;
}

int32_t connsys_close()
{
    struct connsys_func *func = &g_connsys_func;
    int32_t             ret = CONNSYS_STATUS_SUCCESS;


    if (g_connsys_func.use_dma) {
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    hal_gdma_deinit((hal_gdma_channel_t)CONNSYS_GDMA_CH);
#else
    DMA_UnRegister(CONNSYS_GDMA_CH);
#endif
    }

    // release sdio irq
    connsys_bus_get_bus(func);
    ret = connsys_bus_release_irq(func);
    connsys_bus_release_bus(func);
    if (ret) {
        ERR(connsys, "<<%s>> Release irq failed. Error = %d.\n", __FUNCTION__, (int)ret);
    }

    // disable function
    connsys_bus_get_bus(func);
    ret =  connsys_bus_disable_func(func);
    connsys_bus_release_bus(func);
    if (ret) {
        ERR(connsys, "<<%s>> Disable function failed. Error = %d.\n", __FUNCTION__, (int)ret);
    }

    INFO(connsys, "<<%s>> Yes. Release Done.\n", __FUNCTION__);

    return ret;
}

int32_t connsys_disable_interrupt(void)
{
    int32_t ret;
    ret = connsys_cr_write(WHLPCR, W_INT_EN_CLR);
    return ret;
}

#if defined (MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
int32_t connsys_disable_dump_n9_clr(void)
{
    int32_t ret;
    ret = connsys_cr_write(WSICR, NUMP_N9_TRIGGER_INT_CLR);
    return ret;
}

int32_t connsys_enable_dump_n9_set(void)
{
    connsys_cr_write(WSICR, NUMP_N9_TRIGGER_INT_SET);
    return CONNSYS_STATUS_SUCCESS;
}
#endif

int32_t connsys_enable_interrupt(void)
{
    connsys_cr_write(WHLPCR, W_INT_EN_SET);
    return CONNSYS_STATUS_SUCCESS;
}

void connsys_disable_whier_trx_int(void)
{
    uint32_t flags;
    uint32_t val;
    local_irq_save(flags);
    connsys_cr_read(WHIER, &val);
    val &= ~(TX_DONE_INT_EN | RX0_DONE_INT_EN | RX1_DONE_INT_EN);
    connsys_cr_write(WHIER, val);
    local_irq_restore(flags);
}

void connsys_enalbe_whier_rx_int(void)
{
    uint32_t flags;
    uint32_t val;
    local_irq_save(flags);
    connsys_cr_read(WHIER, &val);
    val |= (RX0_DONE_INT_EN | RX1_DONE_INT_EN);
    connsys_cr_write(WHIER, val);
    local_irq_restore(flags);
}

void connsys_enalbe_whier_tx_int(void)
{
    uint32_t flags;
    uint32_t val;
    local_irq_save(flags);
    connsys_cr_read(WHIER, &val);
    val |= (TX_DONE_INT_EN);
    connsys_cr_write(WHIER, val);
    local_irq_restore(flags);
}
#if defined (MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
void connsys_enable_whier_dump_n9_int(void)
{
    uint32_t flags;
    uint32_t val;
    local_irq_save(flags);
    connsys_cr_read(WHIER, &val);
    val |= (DUMP_N9_INT_EN);
    connsys_cr_write(WHIER, val);
    local_irq_restore(flags);
}

void connsys_disable_whier_dump_n9_int(void)
{
    uint32_t flags;
    uint32_t val;
    local_irq_save(flags);
    connsys_cr_read(WHIER, &val);
    val &= ~(DUMP_N9_INT_EN);
    connsys_cr_write(WHIER, val);
    local_irq_restore(flags);
}
#endif

void connsys_tx_flow_control_init(void)
{
    uint32_t flags;
    local_irq_save(flags);
    memset(&g_hif_tx_flow_control_stat, 0, sizeof(wifi_hif_tx_flow_control_t));
    g_hif_tx_flow_control_stat.reserve_quota_page_cnt = DEFAULT_N9_PSE_PAGE_QUOTA;
    g_hif_tx_flow_control_stat.page_sz = DEFAULT_N9_PSE_PAGE_SIZE;
    g_hif_tx_flow_control_stat.available_page_cnt = DEFAULT_N9_PSE_PAGE_QUOTA;
    g_hif_tx_flow_ctrl_en = 1;
    local_irq_restore(flags);
}

uint32_t connsys_tx_flow_control_get_page_size(void)
{
    uint32_t flags;
    uint32_t page_size;
    local_irq_save(flags);
    page_size = g_hif_tx_flow_control_stat.page_sz;
    local_irq_restore(flags);
    return page_size;
}

#ifdef MTK_CM4_WIFI_TASK_ENABLE
extern int Send_Directly_To_lmac;
#endif
uint32_t connsys_tx_flow_control_update_free_page_cnt(void)
{
    uint32_t  flags;
    uint32_t  cr_wtqcr7 = 0;
    wifi_hif_tx_flow_control_t *ctrl = &g_hif_tx_flow_control_stat;

    // Disable Interrupt
    local_irq_save(flags);

    // Read WLAN TXQ Count Register 7 (For N9 only use this now)

#ifdef MTK_CM4_WIFI_TASK_ENABLE
#ifdef SEND_DIRECLY_TO_LMAC
    if(Send_Directly_To_lmac == 0)
    {
        connsys_cr_read(WTQCR7, &cr_wtqcr7);
    }
    else
#endif
    {
#if 0
        uint32_t  check_val,i;
        for(i=0x0130;i<=0x014C;i+=4)
        {
            connsys_cr_read(i,&check_val);

            if(i==0x014c)
            cr_wtqcr7 = check_val;

            if(i==0x0130)
            cr_wtqcr0 = check_val;
        }

        if(cr_wtqcr7 == 65537)
            cr_wtqcr7 = 0;

        if(cr_wtqcr0!=0 && cr_wtqcr7 == 0)
        {
            cr_wtqcr7 = cr_wtqcr0;
        }

#else
            /*Need to read Status from WTQCR0 - sending to LMAC ,WTQCR1 - Beacon,WTQCR7- will have total info*/
            uint32_t read_val,i;
            for(i = WTQCR0; i <= WTQCR7; i+=0x4)
            {
                connsys_cr_read(i,&read_val);
            }
            cr_wtqcr7 = read_val;//Need to write WTQCR7 val
#endif


    }
#else
#if (PRODUCT_VERSION == 5932 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 7686)
    /*Need to read Status from WTQCR0 - sending to LMAC ,WTQCR1 - Beacon,WTQCR7- will have total info*/
    uint32_t read_val,i;
    for(i = WTQCR0; i <= WTQCR7; i+=0x4)
    {
        connsys_cr_read(i,&read_val);
    }
    cr_wtqcr7 = read_val;//Need to write WTQCR7 val
#else
        // MT7687
    connsys_cr_read(WTQCR7, &cr_wtqcr7);
#endif
#endif

#if (CONNSYS_DEBUG_MODE_EN == 1)
    ctrl->free_page_cnt_by_wifi_txq[WIFI_TXQ_CNT_IDX_14_TXCFFA] += (cr_wtqcr7 & 0xffff);
    ctrl->free_page_cnt_by_wifi_txq[WIFI_TXQ_CNT_IDX_15_TXCCPU] += ((cr_wtqcr7 & (0xffffU << 16U)) >> 16U);
    ctrl->total_free_page_cnt += (cr_wtqcr7 & 0xffff);
#endif

    // Update Current Page Count
    ctrl->current_page_cnt -= (cr_wtqcr7 & 0xffff);

    // Update Available Page Count
    ctrl->available_page_cnt = ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;

    // Enable Interrupt
    local_irq_restore(flags);

    return (cr_wtqcr7 & 0xffff);
}

int32_t connsys_tx_flow_control_check_and_update_tx(int32_t port, uint32_t pkt_len)
{
    uint32_t flags;
    uint32_t send_page = 0;
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    wifi_hif_tx_flow_control_t *ctrl = &g_hif_tx_flow_control_stat;

    //added by guofu
    if (0 == ctrl->page_sz) {
        return CONNSYS_STATUS_FAIL;
    }

    local_irq_save(flags);
    send_page = pkt_len / ctrl->page_sz;

    if ((pkt_len % ctrl->page_sz) > 0) {
        send_page ++;
    }

#if (CONNSYS_DEBUG_MODE_EN == 1)
    if (g_hif_tx_flow_ctrl_en) {
        if (send_page <= ctrl->available_page_cnt) {
            ctrl->send_page_cnt_by_tx_port[port] += send_page;
            ctrl->total_send_page_cnt += send_page;
            ctrl->send_pkt_cnt_by_tx_port[port] ++;
            ctrl->total_send_pkt_cnt ++;

            ctrl->current_page_cnt += send_page;

            if (ctrl->max_page_cnt < ctrl->current_page_cnt) {
                ctrl->max_page_cnt = ctrl->current_page_cnt;
            }

            ctrl->available_page_cnt =
                ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;

            ret = CONNSYS_STATUS_SUCCESS;
        } else {
            ctrl->total_drop_pkt_cnt ++;
            ret = CONNSYS_STATUS_FAIL;
        }
    } else {
        ctrl->send_page_cnt_by_tx_port[port] += send_page;
        ctrl->total_send_page_cnt += send_page;
        ctrl->send_pkt_cnt_by_tx_port[port] ++;
        ctrl->total_send_pkt_cnt ++;

        ctrl->current_page_cnt += send_page;

        if (ctrl->max_page_cnt < ctrl->current_page_cnt) {
            ctrl->max_page_cnt = ctrl->current_page_cnt;
        }

        ctrl->available_page_cnt =
            ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;
        ret = CONNSYS_STATUS_SUCCESS;
    }
#else
    if (send_page <= ctrl->available_page_cnt) {
        ctrl->current_page_cnt += send_page;
        ctrl->available_page_cnt =
            ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;
        ret = CONNSYS_STATUS_SUCCESS;
    } else {
        ctrl->total_drop_pkt_cnt ++;
        ret = CONNSYS_STATUS_FAIL;
    }
#endif /* (CONNSYS_DEBUG_MODE_EN == 1) */

    local_irq_restore(flags);

    return ret;
}

void connsys_tx_flow_control_config(uint8_t hif_tx_flow_ctrl_en)
{
    uint32_t flags;
    INFO(connsys, "==>connsys_tx_flow_control_config, hif_tx_flow_ctrl_en = %u\n",
          (unsigned int)hif_tx_flow_ctrl_en);
    local_irq_save(flags);
    g_hif_tx_flow_ctrl_en = hif_tx_flow_ctrl_en;
    local_irq_restore(flags);
}

void connsys_tx_flow_control_set_reserve_page(uint32_t reserve_page)
{
    uint32_t flags;
    wifi_hif_tx_flow_control_t *ctrl = &g_hif_tx_flow_control_stat;
    INFO(connsys, "==>connsys_tx_flow_control_set_reserve_page, reserve_page = %u\n",
          (unsigned int)reserve_page);
    local_irq_save(flags);
    ctrl->reserve_quota_page_cnt = reserve_page;
    ctrl->available_page_cnt =
        ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;
    local_irq_restore(flags);
}


void connsys_tx_flow_control_set_reserve_page_by_cr(void)
{
    uint32_t flags;
    wifi_hif_tx_flow_control_t *ctrl = &g_hif_tx_flow_control_stat;
    local_irq_save(flags);
    ctrl->reserve_quota_page_cnt = connsys_util_pse_get_p0_min_resv();
    ctrl->available_page_cnt =
        ctrl->reserve_quota_page_cnt - ctrl->current_page_cnt;
    local_irq_restore(flags);
}


#ifdef MTK_MINICLI_ENABLE
void connsys_tx_flow_control_show_info(void)
{
    uint32_t flags;
    uint32_t port;
    uint32_t txq_idx;
    wifi_hif_tx_flow_control_t wifi_hif_tx_flow_control;
    local_irq_save(flags);
    memcpy(&wifi_hif_tx_flow_control,
           &g_hif_tx_flow_control_stat,
           sizeof(wifi_hif_tx_flow_control_t));
    local_irq_restore(flags);

    printf("==>connsys_tx_flow_control_show_info\n");


    printf("total_send_pkt_cnt: %u\n", (unsigned int)wifi_hif_tx_flow_control.total_send_pkt_cnt);
    printf("send_pkt_cnt_by_tx_port: \n");
    for (port = 0; port < NUM_OF_WIFI_HIF_TX_PORT; port++) {
        printf("\t[%u]: %u\n",
               (unsigned int)port ,
               (unsigned int)wifi_hif_tx_flow_control.send_pkt_cnt_by_tx_port[port]);
    }

    printf("total_send_page_cnt: %u\n", (unsigned int)wifi_hif_tx_flow_control.total_send_page_cnt);
    printf("send_page_cnt_by_tx_port: \n");
    for (port = 0; port < NUM_OF_WIFI_HIF_TX_PORT; port++) {
        printf("\t[%u]: %u\n",
               (unsigned int)port,
               (unsigned int)wifi_hif_tx_flow_control.send_page_cnt_by_tx_port[port]);
    }


    printf("total_free_page_cnt: %u\n", (unsigned int)wifi_hif_tx_flow_control.total_free_page_cnt);
    printf("free_page_cnt_by_wifi_txq: \n");
    for (txq_idx = 0; txq_idx < NUM_OF_WIFI_TXQ; txq_idx ++) {
        printf("[%2u]:%8u\t",
               (unsigned int)txq_idx,
               (unsigned int)wifi_hif_tx_flow_control.free_page_cnt_by_wifi_txq[txq_idx]);
        if ((txq_idx % 4) == 3) {
            printf("\n");
        }
    }
    printf("available_page_cnt: %u\n", (int)wifi_hif_tx_flow_control.available_page_cnt);
    printf("current_page_cnt: %d\n", (int)wifi_hif_tx_flow_control.current_page_cnt);
    printf("max_page_cnt: %d\n", (int)wifi_hif_tx_flow_control.max_page_cnt);
    printf("reserve_quota_page_cnt: %u\n", (unsigned int)wifi_hif_tx_flow_control.reserve_quota_page_cnt);
    printf("page_sz: %u\n", (unsigned int)wifi_hif_tx_flow_control.page_sz);
    printf("total_drop_pkt_cnt: %u\n", (unsigned int)wifi_hif_tx_flow_control.total_drop_pkt_cnt);
    printf("g_hif_tx_flow_ctrl_en: %u\n", (unsigned int)g_hif_tx_flow_ctrl_en);
}

void connsys_show_config_option(void)
{
    printf("==>connsys_show_config_option\n");
    printf("CONNSYS_MAX_RX_PKT_SIZE = %u\n", CONNSYS_MAX_RX_PKT_SIZE);
    printf("MY_CONNSYS_BLOCK_SIZE = %u\n", MY_CONNSYS_BLOCK_SIZE);
    printf("CFG_WIFI_HIF_GDMA_EN = %u\n", CFG_WIFI_HIF_GDMA_EN);
    printf("CFG_CONNSYS_IOT_RX_ZERO_COPY_EN = %u\n", CFG_CONNSYS_IOT_RX_ZERO_COPY_EN);
    printf("CFG_CONNSYS_IOT_TX_ZERO_COPY_EN = %u\n", CFG_CONNSYS_IOT_TX_ZERO_COPY_EN);
}
#endif

/* Extra headroom lenght when N9 zero copy is enabled. */
static uint8_t EXTRA_HEADROOM_LEN_FOR_NON_QOS      = 38;
static uint8_t EXTRA_HEADROOM_LEN_FOR_QOS_ENABLE   = 42;

uint32_t connsys_get_headroom_offset(uint8_t qos_enable)
{
    if (qos_enable) {
        return EXTRA_HEADROOM_LEN_FOR_QOS_ENABLE;
    }

    return EXTRA_HEADROOM_LEN_FOR_NON_QOS;
}

void connsys_set_headroom_offset(uint8_t qos_enable, uint8_t offset)
{
    if (qos_enable) {
        EXTRA_HEADROOM_LEN_FOR_QOS_ENABLE = offset;
    } else {
        EXTRA_HEADROOM_LEN_FOR_NON_QOS = offset;
    }

}
/*
*Fot tickless
*Must open the FW_OWN_BACK_INT_EN before CM4 give own to N9
* return 0 succeed
*/
int8_t lp_connsys_get_own_enable_int()
{
    uint32_t flags = 0;
    uint32_t reg_value = 0;
    uint32_t counter = 0;

    if (connsys_get_ownership() == FALSE) {
        ERR(connsys, "FAIL get connsys ownership.\n");
        return -1;
    }

    /* enable FW_OWN_BACK_INT interrupt */
    local_irq_save(flags);
    connsys_cr_read(WHIER, &reg_value);
    reg_value |= FW_OWN_BACK_INT_EN;
    connsys_cr_write(WHIER, reg_value);
    local_irq_restore(flags);

    /*check int alrady open*/
    reg_value = 0;
    connsys_cr_read(WHIER, &reg_value);
    while(!(reg_value & FW_OWN_BACK_INT_EN)) {
        if(counter > 2000) {
            ERR(connsys, "en own_back_int timeout.WHIER = 0x%08x \n", (unsigned int)reg_value);
            return -1;
        }
        connsys_cr_read(WHIER, &reg_value);
        counter ++;
    }

    /* enable W_INT_EN_SET */
    if(connsys_enable_interrupt()) {
        ERR(connsys, "Enable interrupt fail.\n");
        return -1;
    }

    return 0;
}

/*
*For tickless
*Must make sure own in N9 before CM4 sleep
*return 0 succeed
*/
int8_t lp_connsys_give_n9_own()
{
    uint32_t reg_value = 0, counter = 0;

    if (connsys_get_ownership() == FALSE) {
        ERR(connsys, "FAIL. get connsys ownership.\n");
        return -1;
    }

    /* set ownership to fw */
    if (connsys_cr_write(WHLPCR, W_FW_OWN_REQ_SET)) {
        ERR(connsys, "FAIL. write WHLPCR failed.\n");
        return -1;
    }

    /* enable connsys interrupt */
    if(connsys_enable_interrupt()) {
        ERR(connsys, "Enable interrupt fail.\n");
        return -1;
    }

    /* check own in fw side */
    counter = 0;
    if(connsys_cr_read(WHLPCR, &reg_value)) {
        ERR(connsys, "FAIL. read WHLPCR failed.\n");
        return -1;
    }
    while (GET_W_FW_OWN_REQ_SET(reg_value)) {
        //loop until ownership not in driver or timeout
        if (counter > 2000) {
            LOG_E(connsys, "give own timeout.\n");
            return -1;
        }

        if (connsys_cr_read(WHLPCR, &reg_value)) {
            LOG_E(connsys, "Read WHLPCR end value fail.\n");
            return -1;
        }
        counter++;
    }

    return 0;

}

#if (MT7682_WSAP00020527 == 1)
#define     AON_TOP_AON_RSV 0xC00C1138
#define N9_HIF_RDY        BIT(15)
extern VOID (*setOpMode)(UINT32 OpMode);
extern int g_iot_init_done;
void radio_on_off(UCHAR Radio)
{
    if (Radio == 1)
    {
        HAL_REG_32(AON_TOP_AON_RSV) &= ~N9_HIF_RDY; //Need to reset this bit when doing radio off or fw reload.
        spm_control_mtcmos(SPM_MTCMOS_CONN,SPM_MTCMOS_PWR_DISABLE);
    }
    else
    {
        g_iot_init_done = 1;
        setOpMode(0);
    }
}

uint32_t connsys_reload_fw()
{
    HAL_REG_32(AON_TOP_AON_RSV) &= ~N9_HIF_RDY; //Need to reset this bit when doing radio off or fw reload.
    spm_control_mtcmos(SPM_MTCMOS_CONN, SPM_MTCMOS_PWR_DISABLE);
    spm_control_mtcmos(SPM_MTCMOS_CONN, SPM_MTCMOS_PWR_ENABLE);

    while(1)
    {
        vTaskDelay(1 / portTICK_RATE_MS);//wifi_os_task_sleep(1);
        if (HAL_REG_32(N9ROM_INIT_DONE) == 1)
           break;
    }
    taskENTER_CRITICAL();

    connsys_open();
    connsys_util_firmware_download();

#if (MT7682_WSAP00020997 == 0)
    g_connsys_func.use_dma = 0;
#endif

#if 0
    while(1)
    {
    if (HAL_REG_32(AON_TOP_AON_RSV) & N9_HIF_RDY)
        break;
    }
#endif
    connsys_tx_flow_control_set_reserve_page_by_cr();

    connsys_enable_interrupt();
    taskEXIT_CRITICAL();
    return TRUE;
}

#endif


#if (PRODUCT_VERSION == 7687 || PRODUCT_VERSION == 7697)
int32_t connsys_init(wifi_sys_cfg_t *sys_config)
{
#if defined(MTK_HAL_LOWPOWER_ENABLE)
    if ((hal_lp_get_wic_status()) || (1 == hal_lp_get_wic_wakeup())) {
        /* N9 MUST be active for clock switch and pinmux config*/
        /* Wakeup N9 by connsys ownership */
        connsys_open();
        connsys_close();
    }

    ptr_lp_connsys_get_own_enable_int   = lp_connsys_get_own_enable_int;
    ptr_lp_connsys_give_n9_own          = lp_connsys_give_n9_own;
    ptr_connsys_get_ownership           = connsys_get_ownership;
#endif

    connsys_set_headroom_offset(CONNSYS_HEADROOM_OFFSET_QOS, 58);
    connsys_set_headroom_offset(CONNSYS_HEADROOM_OFFSET_NON_QOS, 54);

    connsys_set_wifi_profile(sys_config);

    connsys_open();

#if defined(MTK_HAL_LOWPOWER_ENABLE)
    if ((hal_lp_get_wic_status() == 0) && (0 == hal_lp_get_wic_wakeup()))
    {
        /* NO need to reload N9 patch and FW after wakeup from sleep */
#endif
        connsys_util_firmware_download();
#if defined(MTK_HAL_LOWPOWER_ENABLE)
    }
#endif

#if (CFG_WIFI_HIF_GDMA_EN == 1)
    connsys_enable_dma();
    /* Enable LWIP DMA copy */
    if (hal_gdma_init(HAL_GDMA_CHANNEL_0) != HAL_GDMA_STATUS_OK)
        LOG_W(connsys, "WARN! LWIP DMA data copy disabled..\n");

    //hal_gdma_deinit();   // TODO?
#endif /* CFG_WIFI_HIF_GDMA_EN */

    connsys_tx_flow_control_set_reserve_page_by_cr();
    //connsys_enable_interrupt();
    connsys_disable_interrupt();

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
    g_balance_ctr.rx_handle = NetJobGetTaskId();
#endif

    return 0;
}

#endif /* (PRODUCT_VERSION == 7687 || PRODUCT_VERSION == 7697) */

void connsys_disable_dma(void)
{
    g_connsys_func.use_dma = 0;
}


#if (PRODUCT_VERSION == 5932 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 7686)
typedef enum _connsys_state_t
{
    CONNSYS_STATE_UNINITIALIZED,
    CONNSYS_STATE_READY,
} connsys_state_t;

#ifdef MTK_USER_FAST_TX_ENABLE
PUINT_8 g_PktForSend = NULL;
UINT_32 g_PktLen = 0;
extern void PktSendbyPSE(PUINT_8 pPktContent, UINT_32 PktLen);
#endif

/**
 * MCU reset release procedure.
 */
static void _connsys_init_activate_mcu(void)
{
    //Enable N9 MTCMOS Power
    spm_control_mtcmos(SPM_MTCMOS_CONN, SPM_MTCMOS_PWR_ENABLE);

    hal_gpt_delay_ms(1);

    // Release SW reset of connsys
    HAL_REG_32(CONNSYS_SW_RST) = 0x18;

    while(1)
    {
        hal_gpt_delay_ms(1);
        if (HAL_REG_32(N9ROM_INIT_DONE) == 1)
           break;
    }
}


static int32_t _connsys_early_open(void)
{
    struct connsys_func *func = &g_connsys_func;
    int32_t             ret   = CONNSYS_STATUS_SUCCESS;

    func->blksize       = MY_CONNSYS_BLOCK_SIZE;
    func->num           = SDIO_GEN3_FUNCTION_WIFI;
    func->irq_callback  = NULL;
    func->use_dma       = 0;

#if defined(MTK_HAL_LOWPOWER_ENABLE)
    ptr_lp_connsys_get_own_enable_int   = lp_connsys_get_own_enable_int;
    ptr_lp_connsys_give_n9_own          = lp_connsys_give_n9_own;
    ptr_connsys_get_ownership           = connsys_get_ownership;
#endif

    // function enable
    connsys_bus_get_bus(func);
    ret = connsys_bus_enable_func(func);
    connsys_bus_release_bus(func);
    if (ret) {
        ERR(connsys, "enable function failed (%d)\n", (int)ret);
        goto err;
    }

    // set block size
    connsys_bus_get_bus(func);
    ret = connsys_bus_set_block_size(func, func->blksize);
    connsys_bus_release_bus(func);

    if (ret) {
        ERR(connsys, "set block size failed (%d)\n", (int)ret);
        goto err;
    }

    // register sdio irq
    connsys_bus_get_bus(func);
    ret = connsys_bus_get_irq(func, &connsys_driver_interrupt); /* Interrupt IRQ handler */
    connsys_bus_release_bus(func);
    if (ret) {
        ERR(connsys, "claim irq failed (%d)\n", (int)ret);
        goto err;
    }

    // Bug fixed for ownbit initial value (1) error
    connsys_giveup_ownership();

    if (connsys_get_ownership() == FALSE) {
        ERR(connsys, "get ownership failed\n");
        ret = -1;
        goto err;
    }

    connsys_tx_flow_control_init();

err:
    return ret;
}


static int32_t _connsys_early_close(void)
{
    struct connsys_func *func = &g_connsys_func;
    int32_t             ret   = CONNSYS_STATUS_SUCCESS;

    // release sdio irq
    connsys_bus_get_bus(func);
    ret = connsys_bus_release_irq(func);
    connsys_bus_release_bus(func);
    if (ret) {
        ERR(connsys, "release irq failed (%d)\n", (int)ret);
    }

    // disable function
    connsys_bus_get_bus(func);
    ret = connsys_bus_disable_func(func);
    connsys_bus_release_bus(func);
    if (ret) {
        ERR(connsys, "disable function failed (%d)\n", (int)ret);
    }

    return ret;
}


/**
 * The connsys_init() performs initilization and basic settings of connsys.
 * subsystem.
 *
 * connsys_init() can be used (invoked) in two different ways:
 *
 * 1. invoke two times: early init + late init
 *
 *    connsys_init(NULL);
 *    connsys_init(&sysconfig);
 *
 * 2. invoke one time:  normal init (run both early init and late init)
 *
 *    connsys_init(&sysconfig);
 *
 * In both cases, PLL needs to be configured to full speed to allow baseband
 * and RF to be initialized correctly.
 */
int32_t connsys_init(wifi_sys_cfg_t *sys_config)
{
    ATTR_ZIDATA_IN_TCM static connsys_state_t   _g_connsys_state = CONNSYS_STATE_UNINITIALIZED;
#if (MT7682_WSAP00033453==1)
    INFO(connsys, "connsys state %u, sys_config 0x%08x\n",
                    _g_connsys_state,
                    sys_config);
#else
	ERR(connsys, "connsys state %u, sys_config 0x%08x\n",
				  _g_connsys_state,
				  sys_config);
#endif 

    /* early init */

    if (_g_connsys_state == CONNSYS_STATE_UNINITIALIZED) {
        // release SW reset of connsys
        _connsys_init_activate_mcu();
        // setting xtal option to n9
#ifdef HAL_DCXO_MODULE_ENABLED
        bool connsys_xtal_type;
        connsys_xtal_type = hal_clock_fxo_is_26m(); //26M or 40M [false:40M ; true:26M]
        connsys_set_register_value((uint32_t)AON_TOP_AON_RSV2, XTAL_COMPILER_OPTION_TRAPPING_MASK, XTAL_COMPILER_OPTION_TRAPPING_SHIFT, !connsys_xtal_type);
#ifdef XO_3225
        connsys_set_register_value((uint32_t)AON_TOP_AON_RSV2, XTAL_COMPILER_OPTION_XO_MASK, XTAL_COMPILER_OPTION_XO_SHIFT,0);
#endif
#ifdef XO_2016
        connsys_set_register_value((uint32_t)AON_TOP_AON_RSV2, XTAL_COMPILER_OPTION_XO_MASK, XTAL_COMPILER_OPTION_XO_SHIFT,0x10);
#endif
#ifdef XO_2520
        connsys_set_register_value((uint32_t)AON_TOP_AON_RSV2, XTAL_COMPILER_OPTION_XO_MASK, XTAL_COMPILER_OPTION_XO_SHIFT,0x01);
#endif
        if (IS_External_32K)
        {
        	connsys_set_register_value((uint32_t)AON_TOP_AON_RSV2, XTAL_COMPILER_OPTION_CK_MASK, XTAL_COMPILER_OPTION_CK_SHIFT,0x0);
        }
        else
        {
        	connsys_set_register_value((uint32_t)AON_TOP_AON_RSV2, XTAL_COMPILER_OPTION_CK_MASK, XTAL_COMPILER_OPTION_CK_SHIFT,0x1);
        }
#endif
        connsys_set_headroom_offset(CONNSYS_HEADROOM_OFFSET_QOS,     58);
        connsys_set_headroom_offset(CONNSYS_HEADROOM_OFFSET_NON_QOS, 54);

        _connsys_early_open();

        connsys_util_firmware_download();

        _connsys_early_close();

#ifndef MTK_WIFI_SLIM_ENABLE
        _g_connsys_state = CONNSYS_STATE_READY;
#endif
    }

    connsys_set_wifi_profile(sys_config);

    /* late init */

    if (_g_connsys_state == CONNSYS_STATE_READY || sys_config) {

        connsys_open();

#if (CFG_WIFI_HIF_GDMA_EN == 1)
        connsys_enable_dma();

        /* Enable LWIP DMA copy */
        if (hal_gdma_init(HAL_GDMA_CHANNEL_0) != HAL_GDMA_STATUS_OK)
            LOG_W(connsys, "WARN! LWIP DMA data copy disabled..\n");

        hal_gdma_deinit(HAL_GDMA_CHANNEL_0);   // TODO?
#endif

        connsys_tx_flow_control_set_reserve_page_by_cr();

        //int should be disable before connsys_ops initialized
        connsys_disable_interrupt();

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
        g_balance_ctr.rx_handle = NetJobGetTaskId();
#endif
    }

#ifdef MTK_WIFI_SLIM_ENABLE
    /* should stop SLIM driver when normal uart driver is initialized */
    connsys_util_uart_slim_enable(0);
#endif

    _g_connsys_state = CONNSYS_STATE_READY;

#ifdef MTK_USER_FAST_TX_ENABLE
    if ((g_PktForSend != NULL) && (g_PktLen != 0))
        PktSendbyPSE(g_PktForSend, g_PktLen);
#endif

#if defined (MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
        context_dump_init();
#endif

    return 0;
}
#endif
