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

#ifndef __MT2523_H__
#define __MT2523_H__

//#define __CM4_REV                 0x0001  /*!< Core revision r0p1                            */
#define __MPU_PRESENT             1       /*!< MT25x3 provides an MPU                        */
#define __NVIC_PRIO_BITS          6       /*!< MT25x3 uses 6 Bits for the Priority Levels    */
//#define __Vendor_SysTickConfig    0       /*!< Set to 1 if different SysTick Config is used  */
#define __FPU_PRESENT             1       /*!< FPU present                                   */

typedef enum IRQn {
    /****  CM4 internal exceptions  **********/
    Reset_IRQn                  = -15,	/* Reset */
    NonMaskableInt_IRQn         = -14,	/* NMI */
    HardFault_IRQn              = -13,	/* HarFault */
    MemoryManagement_IRQn       = -12,	/* Memory Management */
    BusFault_IRQn               = -11,	/* Bus Fault  */
    UsageFault_IRQn             = -10,	/* Usage Fault */
    SVC_IRQn                    = -5,	/* SV Call*/
    DebugMonitor_IRQn           = -4,	/* Debug Monitor */
    PendSV_IRQn                 = -2,	/* Pend SV */
    SysTick_IRQn                = -1,	/* System Tick */

    /****  MT2523 specific external/peripheral interrupt ****/
    OSTimer_IRQn                =  0,
    Reserved1_IRQn              =  1,
    MD_TOPSM_IRQn               =  2,
    CM4_TOPSM_IRQn              =  3,
    Reserved2_IRQn              =  4,
    Reserved3_IRQn              =  5,
    Reserved4_IRQn              =  6,
    Reserved5_IRQn              =  7,
    ACCDET_IRQn                 =  8,
    RTC_IRQn                    =  9,
    KP_IRQn                     =  10,
    GPTimer_IRQn                =  11,
    EINT_IRQn                   =  12,
    LCD_IRQn                    =  13,
    LCD_AAL_IRQn                =  14,
    DSI_IRQn                    =  15,
    RESIZE_IRQn                 =  16,
    G2D_IRQn                    =  17,
    CAM_IRQn                    =  18,
    ROT_DMA_IRQn                =  19,
    SCAM_IRQn                   =  20,
    DMA_IRQn                    =  21,
    DMA_AO_IRQn                 =  22,
    I2C_DUAL_IRQn               =  23,
    I2C0_IRQn                   =  24,
    I2C1_IRQn                   =  25,
    I2C2_IRQn                   =  26,
    Reserved6_IRQn              =  27,
    GPCounter_IRQn              =  28,
    UART0_IRQn                  =  29,
    UART1_IRQn                  =  30,
    UART2_IRQn                  =  31,
    UART3_IRQn                  =  32,
    USB20_IRQn                  =  33,
    MSDC0_IRQn                  =  34,
    MSDC1_IRQn                  =  35,
    Reserved7_IRQn              =  36,
    Reserved8_IRQn              =  37,
    SF_IRQn                     =  38,
    DSP22CPU_IRQn               =  39,
    SensorDMA_IRQn              =  40,
    RGU_IRQn                    =  41,
    SPI_SLV_IRQn                =  42,
    SPI_MST0_IRQn               =  43,
    SPI_MST1_IRQn               =  44,
    SPI_MST2_IRQn               =  45,
    SPI_MST3_IRQn               =  46,
    TRNG_IRQn                   =  47,
    BT_TIMCON_IRQn              =  48,
    BTIF_IRQn                   =  49,
    Reserved9_IRQn              =  50,
    Reserved10_IRQn             =  51,
    LISR0_IRQn                  =  52,
    LISR1_IRQn                  =  53,
    LISR2_IRQn                  =  54,
    LISR3_IRQn                  =  55,
    PSI_RISE_IRQn               =  56,
    PSI_FALL_IRQn               =  57,
    Reserved11_IRQn             =  58,
    Reserved12_IRQn             =  59,
    Reserved13_IRQn             =  60,
    Reserved14_IRQn             =  61,
    Reserved15_IRQn             =  62,
    Reserved16_IRQn             =  63,
    IRQ_NUMBER_MAX              =  64
} IRQn_Type;

typedef IRQn_Type hal_nvic_irq_t;

#include "core_cm4.h"                  /* Core Peripheral Access Layer */
#include "system_mt2523.h"

/* MT2523 private config */
#define VECTBL_BASE (0x0)

/* PD APB Peripheral */
#define DMA_BASE             0xA0000000 /*DMA Controller*/
#define TRNG_BASE            0xA0010000 /*TRNG*/
#define MSDC0_BASE           0xA0020000 /*MS/SD Controller 0*/
#define MSDC1_BASE           0xA0030000 /*MS/SD Controller 1*/
#define SFC_BASE             0xA0040000 /*Serial Flash*/
#define EMI_BASE             0xA0050000 /*External Memory Interface*/
#define DBGSYS0_APB_BASE     0xA0060000 /*DebugSYS APB 0*/
#define DBGSYS1_APB_BASE     0xA0070000 /*DebugSYS APB 1*/
#define DBGSYS2_APB_BASE     0xA0080000 /*DebugSYS APB 2*/
#define DBGSYS3_APB_BASE     0xA0090000 /*DebugSYS APB 3*/
#define DBGSYS4_APB_BASE     0xA00A0000 /*DebugSYS APB 4*/
#define DBGSYS5_APB_BASE     0xA00B0000 /*DebugSYS APB 5*/
#define DBGSYS6_APB_BASE     0xA00C0000 /*DebugSYS APB 6*/
#define UART0_BASE           0xA00D0000 /*UART 0*/
#define UART1_BASE           0xA00E0000 /*UART 1*/
#define UART2_BASE           0xA00F0000 /*UART 2*/
#define UART3_BASE           0xA0100000 /*UART 3 */
#define SPI_MASTER0_BASE     0xA0110000 /*SPI_MASTER 0*/
#define SPI_MASTER1_BASE     0xA0120000 /*SPI_MASTER 1*/
#define SPI_MASTER2_BASE     0xA0130000 /*SPI_MASTER 2*/
#define SPI_MASTER3_BASE     0xA0140000 /*SPI_MASTER 3*/
#define SPI_SLAVE_BASE       0xA0150000 /*SPI_SLAVE*/
#define PWM2_BASE            0xA0160000 /*Pulse-Width Modulation Outputs 2*/
#define PWM3_BASE            0xA0170000 /*Pulse-Width Modulation Outputs 3*/
#define PWM4_BASE            0xA0180000 /*Pulse-Width Modulation Outputs 4*/
#define PWM5_BASE            0xA0190000 /*Pulse-Width Modulation Outputs 5*/
#define I2C_2_BASE           0xA01B0000 /*I2C_2*/
#define INFRA_MBIST_BASE     0xA01C0000 /*INFRA MBIST CONFIG*/
#define SEN_MEM_BASE         0xA01F0000 /*Sensor memory*/
#define I2C_0_BASE           0xA0210000 /*I2C_0*/
#define I2C_1_BASE           0xA0220000 /*I2C_1*/
#define DMA_SENSOR_BASE      0xA0230000 /*Sensor DMA controller*/
#define AUXADC_BASE          0xA0240000 /*Auxiliary ADC Unit*/

/* PD MMSYS Peripheral */
#define ROT_DMA_BASE         0xA0400000 /*ROT DMA*/
#define RESIZER_BASE         0xA0410000 /*Resizer*/
#define CAM_IF_BASE          0xA0420000 /*Camera Interface*/
#define SCAM_BASE            0xA0430000 /*SCAM*/
#define G2D_BASE             0xA0440000 /*G2D*/
#define LCD_BASE             0xA0450000 /*LCD*/
#define MMSYS_MBIST_BASE     0xA0460000 /*MMSYS_MBIST_CONFIG*/
#define MM_COLOR_BASE        0xA0470000 /*MM_Color*/
#define MMSYS_CONFG_BASE     0xA0480000 /*MMSYS_CONFIG*/
#define AAL_BASE             0xA0490000 /*AAL*/
#define DSI_BASE             0xA04A0000 /*DSI*/

/* PD CMSYS Peripheral */
#define CMSYS_CFG_BASE       0xA0800000 /*cmsys_cfg*/
#define CMSYS_MBIST_BASE     0xA0810000 /*cmsys_mbist*/
#define CM4_MCU_BASE	     0xA0830000 /*Reserved for MDM*/
#define CMSYS_L1CACHE_BASE   0xA0880000 /*cmsys_l1cache*/

/* PD L1 Peripheral */
#define USB_BASE             0xA0900000 /*USB*/
#define USB_SIFSLV_BASE      0xA0910000 /*USB SIFSLV*/
#define PD_DMA_AHB_BASE      0xA0920000 /*DMA*/

/* PD BT system */
#define BT_CONFG_BASE        0xA3300000
#define BT_CIRQ_BASE         0xA3310000
#define BT_DMA_BASE          0xA3320000
#define BT_BTIF_BASE         0xA3330000
#define PKV_BASE             0xA3340000
#define TIMCON_BASE          0xA3350000
#define RFCON_BASE           0xA3360000
#define MODEM_BASE           0xA3370000
#define DBGIF_BASE           0xA3380000
#define BT_MBIST_CONFG_BASE  0xA3390000

/* PD MDSYS */
#define IDMA_CM              0x82000000
#define IDMA_PM              0x82100000
#define IDMA_DM              0x82200000
#define IDMA_SM              0x82300000
#define AHB2DSPIO            0x82800000
#define MD2G_CONFG           0x82C00000
#define MD2G_MBIST_CONFG     0x82C10000
#define SHARE_D1             0x82CA0000
#define PATCH                0x82CC0000
#define AFE                  0x82CD0000
#define MODEM_CONFG_BASE     0x83000000
#define MODEM_MBIST_CFG_BASE 0x83008000
#define MODEM2G_TOPSM_BASE   0x83010000
#define BSI_BASE             0x83070000

/* AO APB Peripheral */
#define CONFIG_BASE          0xA2010000 /*Configuration Registers(Clock, Power Down, Version and Reset)*/
#define GPIO_BASE            0xA2020000 /*General Purpose Inputs/Outputs*/
#define CIRQ_BASE            0xA2030000 /*Interrupt Controller (eint+cirq)*/
#define MIXED_BASE           0xA2040000 /*Analog Chip Interface Controller (PLL, CLKSQ, FH, CLKSW and SIMLS)*/
#define RGU_BASE             0xA2050000 /*Reset Generation Unit*/
#define EFUSE_BASE           0xA2060000 /*EFUSE*/
#define AO_DMA_BASE          0xA2070000 /*AO DMA Controller*/
#define INFRA_BUS_CFG_BASE   0xA2080000 /*INFRA BUS Confg*/
#define MIPI_TX_CFG_BASE     0xA2090000 /*MIPI_TX_CONFIG*/
#define CM_MEMS_DCM_BASE     0xA20A0000 /*Configuration Registers(clock, 104M)*/
#define PSI_MST_BASE         0xA20C0000 /*PSI_MST*/
#define KP_BASE              0xA20D0000 /*Keypad Scanner*/
#define BTIF_BASE            0xA20E0000 /*BTIF*/
#define SYS_TOPSM_BASE       0xA20F0000 /*SYS_TOPSM*/
#define CM4_TOPSM_BASE       0xA2100000 /*CM4_TOPSM*/
#define CM4_CFG_PRI_BASE     0xA2110000 /*CM4_CFG_PRIVATE*/
#define CM4_OSTIMER_BASE     0xA2120000 /*CM4_OSTIMER*/
#define GPCOUNTER_BASE       0xA2130000 /*GP Counter*/
#define GPT_BASE             0xA2140000 /*GP Timer */
#define I2C_D2D_BASE         0xA2150000 /*I2C_D2D*/
#define PWM0_BASE            0xA2160000 /*Pulse-Width Modulation Outputs 0*/
#define PWM1_BASE            0xA2170000 /*Pulse-Width Modulation Outputs 1*/
#define DISP_PWM_BASE        0xA2180000 /*Display Pulse-Width Modulation*/
#define PMU_BASE             0xA21A0000 /*PMU mixedsys*/
#define GPDAC_BASE           0xA21B0000 /*General Purpose DAC*/
#define ABBSYS_BASE          0xA21C0000 /*Analog baseband (ABB) controller*/
#define ANA_CFGSYS_BASE      0xA21D0000 /*Analog die Configuration Registers  (Clock, Reset, etc.)*/
#define RTC_BASE             0xA21E0000 /*Real Time Clock*/
#define ACCDET_BASE          0xA21F0000 /*ACCDET*/
#define EINT_BASE            0xA2030300 /*EINT*/

/* AO L1 Peripheral */
#define AO_DMA_AHB_BASE      0xA2920000 /*AO DMA AHB*/

/* PPB External */
#define CMSYS_CFG_EXT_BASE   0xE00FE000 /*cmsys_cfg_ext*/

/* structure type to access the CACHE register
 */
typedef struct {
    __IO uint32_t CACHE_CON;
    __IO uint32_t CACHE_OP;
    __IO uint32_t CACHE_HCNT0L;
    __IO uint32_t CACHE_HCNT0U;
    __IO uint32_t CACHE_CCNT0L;
    __IO uint32_t CACHE_CCNT0U;
    __IO uint32_t CACHE_HCNT1L;
    __IO uint32_t CACHE_HCNT1U;
    __IO uint32_t CACHE_CCNT1L;
    __IO uint32_t CACHE_CCNT1U;
    uint32_t RESERVED0[1];
    __IO uint32_t CACHE_REGION_EN;
    uint32_t RESERVED1[16372];                  /**< (0x10000-12*4)/4 */
    __IO uint32_t CACHE_ENTRY_N[16];
    __IO uint32_t CACHE_END_ENTRY_N[16];
} CACHE_REGISTER_T;

/* CACHE_CON register definitions
 */
#define CACHE_CON_MCEN_OFFSET 					(0)
#define CACHE_CON_MCEN_MASK 					(0x1UL<<CACHE_CON_MCEN_OFFSET)

#define CACHE_CON_CNTEN0_OFFSET 				(2)
#define CACHE_CON_CNTEN0_MASK 					(0x1UL<<CACHE_CON_CNTEN0_OFFSET)

#define CACHE_CON_CNTEN1_OFFSET 				(3)
#define CACHE_CON_CNTEN1_MASK 					(0x1UL<<CACHE_CON_CNTEN1_OFFSET)

#define CACHE_CON_MDRF_OFFSET 				    (7)
#define CACHE_CON_MDRF_MASK 					(0x1UL<<CACHE_CON_MDRF_OFFSET)

#define CACHE_CON_CACHESIZE_OFFSET 				(8)
#define CACHE_CON_CACHESIZE_MASK 				(0x3UL<<CACHE_CON_CACHESIZE_OFFSET)

/* CACHE_OP register definitions
 */
#define CACHE_OP_EN_OFFSET 						(0)
#define CACHE_OP_EN_MASK 						(0x1UL<<CACHE_OP_EN_OFFSET)

#define CACHE_OP_OP_OFFSET 						(1)
#define CACHE_OP_OP_MASK 						(0xFUL<<CACHE_OP_OP_OFFSET)

#define CACHE_OP_TADDR_OFFSET 					(5)
#define CACHE_OP_TADDR_MASK 					(0x7FFFFFFUL<<CACHE_OP_TADDR_OFFSET)

/* CACHE_HCNT0L register definitions
 */
#define CACHE_HCNT0L_CHIT_CNT0_MASK 			(0xFFFFFFFFUL<<0)

/* CACHE_HCNT0U register definitions
 */
#define CACHE_HCNT0U_CHIT_CNT0_MASK				(0xFFFFUL<<0)

/* CACHE_CCNT0L register definitions
 */
#define CACHE_CCNT0L_CACC_CNT0_MASK				(0xFFFFFFFFUL<<0)

/* CACHE_CCNT0U register definitions
 */
#define CACHE_CCNT0U_CACC_CNT0_MASK				(0xFFFFUL<<0)

/* CACHE_HCNT1L register definitions
 */
#define CACHE_HCNT1L_CHIT_CNT1_MASK				(0xFFFFFFFFUL<<0)

/* CACHE_HCNT1U register definitions
 */
#define CACHE_HCNT1U_CHIT_CNT1_MASK				(0xFFFFUL<<0)

/* CACHE_CCNT1L register definitions
 */
#define CACHE_CCNT1L_CACC_CNT1_MASK				(0xFFFFFFFFUL<<0)

/* CACHE_CCNT1U register definitions
 */
#define CACHE_CCNT1U_CACC_CNT1_MASK				(0xFFFFUL<<0)

/* CACHE_ENTRY_N register definitions
 */
#define CACHE_ENTRY_N_C_MASK 					(0x1UL<<8)
#define CACHE_ENTRY_BASEADDR_MASK 				(0xFFFFFUL<<12)

/* CACHE_END_ENTRY_N register definitions
 */
#define CACHE_END_ENTRY_N_BASEADDR_MASK 		(0xFFFFFUL<<12)

/* memory mapping of MT7687
 */
#define CACHE  									((CACHE_REGISTER_T *)CMSYS_L1CACHE_BASE)


/* structure type to access the ADC register
 */
typedef struct {
    uint32_t RESERVED0[1];
    __IO uint32_t AUXADC_CON1;
    uint32_t RESERVED1[1];
    __IO uint32_t AUXADC_CON3;
    uint32_t RESERVED2[6];
    __I  uint32_t AUXADC_DAT6;
    __I  uint32_t AUXADC_DAT7;
    __I  uint32_t AUXADC_DAT8;
    uint32_t RESERVED3[2];
    __I  uint32_t AUXADC_DAT11;
    __I  uint32_t AUXADC_DAT12;
    __I  uint32_t AUXADC_DAT13;
    __I  uint32_t AUXADC_DAT14;
    __I  uint32_t AUXADC_DAT15;
} ADC_REGISTER_T;

/* AUXADC_CON3 register definitions
 */
#define AUXADC_CON3_AUTOSET_OFFSET 						(15)
#define AUXADC_CON3_AUTOSET_MASK						(0x1UL<<AUXADC_CON3_AUTOSET_OFFSET)

#define AUXADC_CON3_SOFT_RST_OFFSET 					(7)
#define AUXADC_CON3_SOFT_RST_MASK 						(0x1UL<<AUXADC_CON3_SOFT_RST_OFFSET)

#define AUXADC_CON3_AUXADC_STA_OFFSET 					(0)
#define AUXADC_CON3_AUXADC_STA_MASK 					(0x1UL<<AUXADC_CON3_AUXADC_STA_OFFSET)

/* memory mapping of MT2523
 */
#define ADC 									((ADC_REGISTER_T *)AUXADC_BASE)


/* structure type to access the DAC register
 */
typedef struct {
    __IO uint32_t GPDAC_TGSEL;
    __IO uint32_t GPDAC_TEST;
    __IO uint32_t GPDACLDO_CAL;
    __IO uint32_t GPDACLDO_TRIM;
    __IO uint32_t GPDAC_CAL;
    __IO uint32_t GPDAC_PDN;
    __IO uint32_t SWRST;
    __IO uint32_t OUTPUT_COMMAND;
    __IO uint32_t OUTPUT_REGION;
    __IO uint32_t WRITE_COMMAND;
    uint32_t RESERVED0[10];
    __IO uint32_t GPDAC_SRAM_PWR;
    __IO uint32_t GPDAC_DEBUG;
} DAC_REGISTER_T;

/* GPDAC_PDN register definitions
 */
#define GPDAC_PDN_GPDAC_PDN_MASK 				(0x1UL)

/* SWRST register definitions
 */
#define SWRST_SWRST_MASK 						(0x1UL)

/* OUTPUT_COMMAND register definitions
 */
#define OUTPUT_COMMAND_OUTPUT_EN_OFFSET 		(0)
#define OUTPUT_COMMAND_OUTPUT_EN_MASK 			(0x1UL<<OUTPUT_COMMAND_OUTPUT_EN_OFFSET)

#define OUTPUT_COMMAND_REPEAT_EN_OFFSET 		(4)
#define OUTPUT_COMMAND_REPEAT_EN_MASK 			(0x1UL<<OUTPUT_COMMAND_REPEAT_EN_OFFSET)

/* OUTPUT_REGION register definitions
 */
#define OUTPUT_REGION_OUTPUT_START_ADDR_OFFSET 	(0)
#define OUTPUT_REGION_OUTPUT_START_ADDR_MASK 	(0x3FUL<<OUTPUT_REGION_OUTPUT_START_ADDR_OFFSET)

#define OUTPUT_REGION_OUTPUT_END_ADDR_OFFSET 	(8)
#define OUTPUT_REGION_OUTPUT_END_ADDR_MASK 		(0x3FUL<<OUTPUT_REGION_OUTPUT_END_ADDR_OFFSET)

/* WRITE_COMMAND register definitions
 */
#define WRITE_COMMAND_SRAM_ADDR_OFFSET 			(0)
#define WRITE_COMMAND_SRAM_ADDR_MASK 			(0x3FUL<<WRITE_COMMAND_SRAM_ADDR_OFFSET)

#define WRITE_COMMAND_SRAM_DATA_OFFSET 			(8)
#define WRITE_COMMAND_SRAM_DATA_MASK 			(0x3FFUL<<WRITE_COMMAND_SRAM_DATA_OFFSET)

/* GPDAC_SRAM_PWR register definitions
 */
#define GPDAC_SRAM_PWR_GPDAC_SLEEPB_OFFSET      (3)
#define GPDAC_SRAM_PWR_GPDAC_SLEEPB_MASK        (0x1UL<<GPDAC_SRAM_PWR_GPDAC_SLEEPB_OFFSET)

#define GPDAC_SRAM_PWR_GPDAC_PD_OFFSET          (2)
#define GPDAC_SRAM_PWR_GPDAC_PD_MASK            (0x1UL<<GPDAC_SRAM_PWR_GPDAC_PD_OFFSET)

#define GPDAC_SRAM_PWR_GPDAC_ISOINTB_OFFSET     (1)
#define GPDAC_SRAM_PWR_GPDAC_ISOINTB_MASK       (0x1UL<<GPDAC_SRAM_PWR_GPDAC_ISOINTB_OFFSET)

#define GPDAC_SRAM_PWR_GPDAC_RET_OFFSET         (0)
#define GPDAC_SRAM_PWR_GPDAC_RET_MASK           (0x1UL<<GPDAC_SRAM_PWR_GPDAC_RET_OFFSET)

/* memory mapping of MT2523
 */

#define DAC 									((DAC_REGISTER_T *)GPDAC_BASE)


/* structure type to access the ACCDET register
 */
typedef struct {
    __IO uint32_t ACCDET_RSTB;
    __IO uint32_t ACCDET_CTRL;
    __IO uint32_t ACCDET_STATE_SWCTRL;
    __IO uint32_t ACCDET_PWM_WIDTH;
    __IO uint32_t ACCDET_PWM_THRESH;
    uint32_t RESERVED0[4];
    __IO uint32_t ACCDET_EN_DELAY_NUM;
    __IO uint32_t ACCDET_PWM_IDLE_VALUE;
    __IO uint32_t ACCDET_DEBOUNCE0;
    __IO uint32_t ACCDET_DEBOUNCE1;
    uint32_t RESERVED1[1];
    __IO uint32_t ACCDET_DEBOUNCE3;
    __IO uint32_t ACCDET_IRQ_STS;
    __I  uint32_t ACCDET_CURR_IN;
    __I  uint32_t ACCDET_SAMPLE_IN;
    __I  uint32_t ACCDET_MEMORIZED_IN;
    __I  uint32_t ACCDET_LAST_MEMORIZED_IN;
    __I  uint32_t ACCDET_FSM_STATE;
    __I  uint32_t ACCDET_CURR_DEBOUNCE;
    __I  uint32_t ACCDET_VERSION;
    __IO uint32_t ACCDET_IN_DEFAULT;
} ACCDET_REGISTER_T;

/* ACCDET_RSTB register definitions
 */
#define ACCDET_RSTB_RSTB_OFFSET 				(0)
#define ACCDET_RSTB_RSTB_MASK 					(0x1UL<<ACCDET_RSTB_RSTB_OFFSET)

/* ACCDET_CTRL register definitions
 */
#define ACCDET_CTRL_ACCDET_EN_OFFSET 			(0)
#define ACCDET_CTRL_ACCDET_EN_MASK 				(0x1UL<<ACCDET_CTRL_ACCDET_EN_OFFSET)

/* ACCDET_STATE_SWCTRL register definitions
 */
#define ACCDET_STATE_SWCTRL_CMP_PWM_EN_OFFSET 	(2)
#define ACCDET_STATE_SWCTRL_CMP_PWM_EN_MASK 	(0x1UL<<ACCDET_STATE_SWCTRL_CMP_PWM_EN_OFFSET)

#define ACCDET_STATE_SWCTRL_VTH_PWM_EN_OFFSET 	(3)
#define ACCDET_STATE_SWCTRL_VTH_PWM_EN_MASK 	(0x1UL<<ACCDET_STATE_SWCTRL_VTH_PWM_EN_OFFSET)

#define ACCDET_STATE_SWCTRL_MBIAS_PWM_EN_OFFSET (4)
#define ACCDET_STATE_SWCTRL_MBIAS_PWM_EN_MASK 	(0x1UL<<ACCDET_STATE_SWCTRL_MBIAS_PWM_EN_OFFSET)

/* ACCDET_PWM_WIDTH register definitions
 */
#define ACCDET_PWM_WIDTH_PWM_WIDTH_OFFSET 		(0)
#define ACCDET_PWM_WIDTH_PWM_WIDTH_MASK 		(0xFFFFUL<<ACCDET_PWM_WIDTH_PWM_WIDTH_OFFSET)

/* ACCDET_PWM_THRESH register definitions
 */
#define ACCDET_PWM_THRESH_PWM_THRESH_OFFSET 	(0)
#define ACCDET_PWM_THRESH_PWM_THRESH_MASK 		(0xFFFFUL<<ACCDET_PWM_THRESH_PWM_THRESH_OFFSET)

/* ACCDET_DEBOUNCE0 register definitions
 */
#define ACCDET_DEBOUNCE0_DEBOUNCE0_OFFSET 		(0)
#define ACCDET_DEBOUNCE0_DEBOUNCE0_MASK 		(0x1UL<<ACCDET_DEBOUNCE0_DEBOUNCE0_OFFSET)

/* ACCDET_DEBOUNCE1 register definitions
 */
#define ACCDET_DEBOUNCE1_DEBOUNCE1_OFFSET 		(0)
#define ACCDET_DEBOUNCE1_DEBOUNCE1_MASK 		(0x1UL<<ACCDET_DEBOUNCE1_DEBOUNCE1_OFFSET)

/* ACCDET_DEBOUNCE3 register definitions
 */
#define ACCDET_DEBOUNCE3_DEBOUNCE3_OFFSET 		(0)
#define ACCDET_DEBOUNCE3_DEBOUNCE3_MASK 		(0x1UL<<ACCDET_DEBOUNCE3_DEBOUNCE3_OFFSET)

/* ACCDET_IRQ_STS register definitions
 */
#define ACCDET_IRQ_STS_IRQ_OFFSET 				(0)
#define ACCDET_IRQ_STS_IRQ_MASK 				(0x1UL<<ACCDET_IRQ_STS_IRQ_OFFSET)

#define ACCDET_IRQ_STS_IRQ_CLR_OFFSET 			(8)
#define ACCDET_IRQ_STS_IRQ_CLR_MASK 			(0x1UL<<ACCDET_IRQ_STS_IRQ_CLR_OFFSET)

/* ACCDET memory map
 */
#define ACCDET 									((ACCDET_REGISTER_T *)ACCDET_BASE)

/* Register definitions for UART */
typedef struct {
    union {
        __I uint32_t RBR; /* RX buffer register */
        __O uint32_t THR; /* TX holding register */
        __IO uint32_t DLL; /* Divisor latch register */
    } RBR_THR_DLL;
    union {
        __IO uint32_t IER; /* Interrupt enable register */
        __IO uint32_t DLM; /* Divisor latch register */
    } IER_DLM;
    union {
        __I uint32_t IIR; /* Interrupt identification register */
        __O uint32_t FCR; /* FIFO control register */
        __IO uint32_t EFR; /* Enhanced feature register */
    } IIR_FCR_EFR;
    __IO uint32_t LCR; /* Line control register */
    union {
        __IO uint32_t MCR; /* Modem control register */
        __IO uint32_t XON1; /* XON1 char register */
    } MCR_XON1;
    union {
        __I uint32_t LSR; /* Line status register */
        __IO uint32_t XON2; /* XON2 char register */
    } LSR_XON2;
    __IO uint32_t XOFF1; /* XOFF1 char register */
    union {
        __IO uint32_t SCR; /* Scratch register */
        __IO uint32_t XOFF2; /* XOFF2 char register */
    } SCR_XOFF2;
    __IO uint32_t AUTOBAUD_EN; /* Autoband detect enable register */
    __IO uint32_t HIGHSPEED; /* High speed mode register */
    __IO uint32_t SAMPLE_COUNT; /* Sample counter register */
    __IO uint32_t SAMPLE_POINT; /* Sample point register */
    __I uint32_t AUTOBAUD_REG; /* Auto Baud Monitor Register */
    __IO uint32_t RATEFIX_AD; /* Clock Rate Fix Register */
    __IO uint32_t AUTOBAUDSAMPLE; /* Auto Baud Sample Register */
    __IO uint32_t GUARD; /* Guard Time Added reRegister */
    __IO uint32_t ESCAPE_DAT; /* Escape character register */
    __IO uint32_t ESCAPE_EN; /* Escape enable register */
    __IO uint32_t SLEEP_EN; /* Sleep enable register */
    __IO uint32_t DMA_EN; /* DMA enable register */
    __IO uint32_t RXTRI_AD; /* Rx Trigger Address */
    __IO uint32_t FRACDIV_L; /* Fractional Divider LSB Address */
    __IO uint32_t FRACDIV_M; /* Fractional Divider MSB Address */
    __I uint32_t FCR_RD; /* FIFO Control Register */
    __IO uint32_t TX_ACTIVE_EN; /* TX Active Enable Address */
} UART_REGISTER_T;

#define UART0   ((UART_REGISTER_T *)(UART0_BASE))
#define UART1   ((UART_REGISTER_T *)(UART1_BASE))
#define UART2   ((UART_REGISTER_T *)(UART2_BASE))
#define UART3   ((UART_REGISTER_T *)(UART3_BASE))

/* Bit definition for Interrupt Enable Register */
#define UART_IER_CTSI_OFFSET    (7)
#define UART_IER_CTSI_MASK      (0x1<<UART_IER_CTSI_OFFSET) /* generated when a rising edge is detected on the CTS modem control line */
#define UART_IER_RTSI_OFFSET    (6)
#define UART_IER_RTSI_MASK      (0x1<<UART_IER_RTSI_OFFSET) /* generated when a rising edge is detected on the RTS modem control line */
#define UART_IER_XOFFI_OFFSET   (5)
#define UART_IER_XOFFI_MASK     (0x1<<UART_IER_XOFFI_OFFSET) /* generated when an XOFF character is received */
#define UART_IER_ELSI_OFFSET    (2)
#define UART_IER_ELSI_MASK      (0x1<<UART_IER_ELSI_OFFSET) /* generated if BI, FE, PE or OE (LSR[4:1]) becomes set */
#define UART_IER_ETBEI_OFFSET   (1)
#define UART_IER_ETBEI_MASK     (0x1<<UART_IER_ETBEI_OFFSET) /* generated if the THR is empty or TX FIFO reduced to its Trigger Level */
#define UART_IER_ERBFI_OFFSET   (0)
#define UART_IER_ERBFI_MASK     (0x1<<UART_IER_ERBFI_OFFSET) /* generated if RBR has data or the RX Trigger Level is reached */

/* Bit definition for Interrupt Identification Register */
#define UART_IIR_FIFOE_OFFSET   (6)
#define UART_IIR_FIFOE_MASK     (0x3<<UART_IIR_FIFOE_OFFSET) /*  */
#define UART_IIR_ID_OFFSET      (0)
#define UART_IIR_ID_MASK        (0x3F<<UART_IIR_ID_OFFSET) /* Interrupt Source Identification */

/* Bit definition for FIFO Control Register */
#define UART_FCR_RFTL1_RFTL0_OFFSET (6)
#define UART_FCR_RFTL1_RFTL0_MASK   (0x3<<UART_FCR_RFTL1_RFTL0_OFFSET) /* RX FIFO trigger threshold */
#define UART_FCR_TFTL1_TFTL0_OFFSET (4)
#define UART_FCR_TFTL1_TFTL0_MASK   (0x3<<UART_FCR_TFTL1_TFTL0_OFFSET) /* TX FIFO trigger threshold */
#define UART_FCR_CLRT_OFFSET    (2)
#define UART_FCR_CLRT_MASK      (0x1<<UART_FCR_CLRT_OFFSET) /* control bit to clear tx fifo */
#define UART_FCR_CLRR_OFFSET    (1)
#define UART_FCR_CLRR_MASK      (0x1<<UART_FCR_CLRR_OFFSET) /* control bit to clear rx fifo */
#define UART_FCR_FIFOE_OFFSET   (0)
#define UART_FCR_FIFOE_MASK     (0x1<<UART_FCR_FIFOE_OFFSET) /* FIFO Enabled */

/* Bit definition for Enhanced Feature Register */
#define UART_EFR_AUTO_CTS_OFFSET    (7)
#define UART_EFR_AUTO_CTS_MASK      (0x1<<UART_EFR_AUTO_CTS_OFFSET) /* Enables hardware transmission flow control */
#define UART_EFR_AUTO_RTS_OFFSET    (6)
#define UART_EFR_AUTO_RTS_MASK      (0x1<<UART_EFR_AUTO_RTS_OFFSET) /* Enables hardware reception flow control */
#define UART_EFR_ENABLE_E_OFFSET    (4)
#define UART_EFR_ENABLE_E_MASK      (0x1<<UART_EFR_ENABLE_E_OFFSET) /* Enables enhancement feature */
#define UART_EFR_SW_FLOW_TX_OFFSET  (2)
#define UART_EFR_SW_FLOW_TX_MASK    (0x3<<UART_EFR_SW_FLOW_TX_OFFSET) /* Transmit XON1/XOFF1 as flow control bytes */
#define UART_EFR_SW_FLOW_RX_OFFSET  (0)
#define UART_EFR_SW_FLOW_RX_MASK    (0x3<<UART_EFR_SW_FLOW_RX_OFFSET) /* Receive XON1/XOFF1 as flow control bytes */


/* Bit definition for Line Control Register */
#define UART_LCR_DLAB_OFFSET (7)
#define UART_LCR_DLAB_MASK  (0x1<<UART_LCR_DLAB_OFFSET) /* Divisor Latch Access Bit */
#define UART_LCR_SB_OFFSET  (6)
#define UART_LCR_SB_MASK    (0x1<<UART_LCR_SB_OFFSET) /* Set Break */
#define UART_LCR_PARITY_OFFSET  (3)
#define UART_LCR_PARITY_MASK    (0x7<<UART_LCR_PARITY_OFFSET)
#define UART_LCR_SP_OFFSET      (5)
#define UART_LCR_SP_MASK    (0x1<<UART_LCR_SP_OFFSET) /* Stick Parity */
#define UART_LCR_EPS_OFFSET (4)
#define UART_LCR_EPS_MASK   (0x1<<UART_LCR_EPS_OFFSET) /* Even Parity Select */
#define UART_LCR_PEN_OFFSET (3)
#define UART_LCR_PEN_MASK   (0x1<<UART_LCR_PEN_OFFSET) /* Parity Enable */
#define UART_LCR_STB_OFFSET (2)
#define UART_LCR_STB_MASK   (0x1<<UART_LCR_STB_OFFSET) /* Number of STOP bits */
#define UART_LCR_WORD_OFFSET (0)
#define UART_LCR_WORD_MASK  (0x3<<UART_LCR_WORD_OFFSET) /* Word Length */


/* Bit definition for Modem Control Register */
#define UART_MCR_XOFF_STATUS_OFFSET (7)
#define UART_MCR_XOFF_STATUS_MASK   (0x1<<UART_MCR_XOFF_STATUS_OFFSET) /* whether an XON character is received */
#define UART_MCR_LOOP_OFFSET        (4)
#define UART_MCR_LOOP_MASK  (0x1<<UART_MCR_LOOP_OFFSET) /* Loop-back control bit */
#define UART_MCR_RTS_OFFSET (1)
#define UART_MCR_RTS_MASK   (0x1<<UART_MCR_RTS_OFFSET) /* Controls the state of the output RTS, even in loop mode */

/* Bit definition for Line Status Register */
#define UART_LSR_FIFOERR_OFFSET (7)
#define UART_LSR_FIFOERR_MASK   (0x1<<UART_LSR_FIFOERR_OFFSET) /* RX FIFO Error Indicator */
#define UART_LSR_TEMT_OFFSET    (6)
#define UART_LSR_TEMT_MASK      (0x1<<UART_LSR_TEMT_OFFSET) /* TX Holding Register (or TX FIFO) and the TX Shift Register are empty */
#define UART_LSR_THRE_OFFSET    (5)
#define UART_LSR_THRE_MASK      (0x1<<UART_LSR_THRE_OFFSET) /* Indicates if there is room for TX Holding Register or TX FIFO is reduced to its Trigger Level */
#define UART_LSR_BI_OFFSET  (4)
#define UART_LSR_BI_MASK    (0x1<<UART_LSR_BI_OFFSET) /* Break Interrupt */
#define UART_LSR_FE_OFFSET  (3)
#define UART_LSR_FE_MASK    (0x1<<UART_LSR_FE_OFFSET) /* Framing Error */
#define UART_LSR_PE_OFFSET  (2)
#define UART_LSR_PE_MASK    (0x1<<UART_LSR_PE_OFFSET) /* Parity Error */
#define UART_LSR_OE_OFFSET  (1)
#define UART_LSR_OE_MASK    (0x1<<UART_LSR_OE_OFFSET) /* Overrun Error */
#define UART_LSR_DR_OFFSET  (0)
#define UART_LSR_DR_MASK    (0x1<<UART_LSR_DR_OFFSET) /* Data Ready */

/* Bit definition for Auto Baud Detect Enable Register */
#define UART_AUTOBAUD_EN_SLEEP_ACK_SEL_OFFSET   (2)
#define UART_AUTOBAUD_EN_SLEEP_ACK_SEL_MASK     (0x1<<UART_AUTOBAUD_EN_SLEEP_ACK_SEL_OFFSET) /* Sleep ack select when autobaud_en */
#define UART_AUTOBAUD_EN_AUTOBAUD_SEL_OFFSET    (1)
#define UART_AUTOBAUD_EN_AUTOBAUD_SEL_MASK      (0x1<<UART_AUTOBAUD_EN_AUTOBAUD_SEL_OFFSET) /* Auto-baud select */
#define UART_AUTOBAUD_EN_AUTOBAUD_EN_OFFSET     (0)
#define UART_AUTOBAUD_EN_AUTOBAUD_EN_MASK       (0x1<<UART_AUTOBAUD_EN_AUTOBAUD_EN_OFFSET) /* Auto-baud enable signal */

/* Bit definition for High Speed Mode Register */
#define UART_HIGHSPEED_SPEED_OFFSET (0)
#define UART_HIGHSPEED_SPEED_MASK   (0x3<<UART_HIGHSPEED_SPEED_OFFSET) /* UART sample counter base */

/* Bit definition for Auto Baud Monitor Register */
#define UART_AUTOBAUD_REG_BAUD_STAT_OFFSET  (4)
#define UART_AUTOBAUD_REG_BAUD_STAT_MASK    (0x0F<<UART_AUTOBAUD_REG_BAUD_STAT_OFFSET) /* frame detect result */
#define UART_AUTOBAUD_REG_BAUD_RATE_OFFSET  (0)
#define UART_AUTOBAUD_REG_BAUD_RATE_MASK    (0x0F<<UART_AUTOBAUD_REG_BAUD_RATE_OFFSET) /* baudrate detect result */

/* Bit definition for Clock Rate Fix Register */
#define UART_RATEFIX_AD_AUTOBAUD_OFFSET (1)
#define UART_RATEFIX_AD_AUTOBAUD_MASK   (0x1<<UART_RATEFIX_AD_AUTOBAUD_OFFSET)
#define UART_RATEFIX_AD_RATE_FIX_OFFSET (0)
#define UART_RATEFIX_AD_RATE_FIX_MASK   (0x1<<UART_RATEFIX_AD_RATE_FIX_OFFSET)

/* Bit definition for Auto Baud Sample Register */
#define UART_AUTOBAUDSAMPLE_OFFSET          (0)
#define UART_AUTOBAUDSAMPLE_MASK            (0x3F<<UART_AUTOBAUDSAMPLE_OFFSET) /* clk division for autobaud rate detection */

/* Bit definition for Guard time added register */
#define UART_GUARD_GUARD_EN_OFFSET  (4)
#define UART_GUARD_GUARD_EN_MASK    (0x1<<UART_GUARD_GUARD_EN_OFFSET) /* Guard interval add enable signal */
#define UART_GUARD_GUARD_CNT_OFFSET (0)
#define UART_GUARD_GUARD_CNT_MASK   (0x0F<<UART_GUARD_GUARD_CNT_OFFSET) /* Guard interval count value */

/* Bit definition for Escape enable register */
#define UART_ESCAPE_EN_OFFSET   (0)
#define UART_ESCAPE_EN_MASK     (0x1<<UART_ESCAPE_EN_OFFSET) /* Add escape character when TX and remove escape character when RX */

/* Bit definition for Sleep enable register */
#define UART_SLEEP_EN_OFFSET    (0)
#define UART_SLEEP_EN_MASK      (0x1<<UART_SLEEP_EN_OFFSET) /* Sleep enable bit */

/* Bit definition for DMA enable register */
#define UART_DMA_EN_FIFO_LSR_SEL_OFFSET     (3)
#define UART_DMA_EN_FIFO_LSR_SEL_MASK       (0x1<<UART_DMA_EN_FIFO_LSR_SEL_OFFSET) /* fifo lsr mode selection */
#define UART_DMA_EN_TO_CNT_AUTORST_OFFSET   (2)
#define UART_DMA_EN_TO_CNT_AUTORST_MASK     (0x1<<UART_DMA_EN_TO_CNT_AUTORST_OFFSET) /* Timeout counter auto reset register */
#define UART_DMA_EN_TX_DMA_EN_OFFSET        (1)
#define UART_DMA_EN_TX_DMA_EN_MASK      (0x1<<UART_DMA_EN_TX_DMA_EN_OFFSET) /* TX_DMA mechanism enable signal */
#define UART_DMA_EN_RX_DMA_EN_OFFSET    (0)
#define UART_DMA_EN_RX_DMA_EN_MASK      (0x1<<UART_DMA_EN_RX_DMA_EN_OFFSET) /* RX_DMA mechanism enable signal */

/* Bit definition for Rx Trigger Address */
#define UART_RXTRI_AD_OFFSET (0)
#define UART_RXTRI_AD_MASK   (0x0F<<UART_RXTRI_AD_OFFSET) /* When {rtm,rtl}=2'b11, The Rx FIFO threshold will be Rxtrig */

/* Bit definition for Fractional Divider MSB Address */
#define UART_FRACDIV_M_OFFSET   (0)
#define UART_FRACDIV_M_MASK     (0x3<<UART_FRACDIV_M_OFFSET) /* Add sampling count when in state stop to parity */

/* Bit definition for FIFO Control Register */
#define UART_FCR_RD_RXFIFO_TRIGGER_OFFSET   (6)
#define UART_FCR_RD_RXFIFO_TRIGGER_MASK     (0x3<<UART_FCR_RD_RXFIFO_TRIGGER_OFFSET) /* RX FIFO trigger threshold bits mask */
#define UART_FCR_RD_TXFIFO_TRIGGER_OFFSET   (4)
#define UART_FCR_RD_TXFIFO_TRIGGER_MASK     (0x3<<UART_FCR_RD_TXFIFO_TRIGGER_OFFSET) /* TX FIFO trigger threshold bits mask */
#define UART_FCR_RD_CLRT_OFFSET         (2)
#define UART_FCR_RD_CLRT_MASK       (0x1<<UART_FCR_RD_CLRT_OFFSET) /* TX FIFO clear status */
#define UART_FCR_RD_CLRR_OFFSET     (1)
#define UART_FCR_RD_CLRR_MASK       (0x1<<UART_FCR_RD_CLRR_OFFSET) /* RX FIFO clear status */
#define UART_FCR_RD_FIFOE_OFFSET    (0)
#define UART_FCR_RD_FIFOE_MASK      (0x1<<UART_FCR_RD_FIFOE_OFFSET) /* FIFO Enable */

#define UART_NORMAL_OFFSET  (0)
#define UART_NORMAL_MASK    (0xFF<<UART_NORMAL_OFFSET) /* mask for byte in uart register */


/* *************************dma hardware definition start line**********************************
*/


/*gdma base address definition
*/
#define GDMA1_base  ((uint32_t)0xA0000100)

/*gdma global status base address definition
*/
#define GDMA_GLOAL1_base  ((uint32_t)0xA0000000)


/*pdma base address definition
*/
#define PDMA2_base  ((uint32_t)0xA0000208)
#define PDMA3_base  ((uint32_t)0xA0000308)
#define PDMA4_base  ((uint32_t)0xA0230208)
#define PDMA5_base  ((uint32_t)0xA0230308)
#define PDMA6_base  ((uint32_t)0xA0230408)
#define PDMA7_base  ((uint32_t)0xA0230508)

/*pdma global status base address definition
*/

#define PDMA_GLOAL1_base  ((uint32_t)0xA0000000)
#define PDMA_GLOAL2_base  ((uint32_t)0xA0230000)


/*virtual fifo dma base address definition
*/
#define VDMA9_base   ((uint32_t)0xA0000910)
#define VDMA10_base  ((uint32_t)0xA0000A10)
#define VDMA11_base  ((uint32_t)0xA0000B10)
#define VDMA12_base  ((uint32_t)0xA0000C10)
#define VDMA13_base  ((uint32_t)0xA0000D10)
#define VDMA14_base  ((uint32_t)0xA0000E10)
#define VDMA15_base  ((uint32_t)0xA0000F10)
#define VDMA16_base  ((uint32_t)0xA0001010)
#define VDMA17_base  ((uint32_t)0xA0270910)
#define VDMA18_base  ((uint32_t)0xA0270A10)

/*virtual dma port address definition
*/
#define VDMA9_port    ((uint32_t)0xA0920000)
#define VDMA10_port   ((uint32_t)0xA0920100)
#define VDMA11_port   ((uint32_t)0xA0920200)
#define VDMA12_port   ((uint32_t)0xA0920300)
#define VDMA13_port   ((uint32_t)0xA0920400)
#define VDMA14_port   ((uint32_t)0xA0920500)
#define VDMA15_port   ((uint32_t)0xA0920600)
#define VDMA16_port   ((uint32_t)0xA0920700)
#define VDMA17_port   ((uint32_t)0xA2920000)
#define VDMA18_port   ((uint32_t)0xA2920100)


/*pdma global status base address definition
*/

#define VDMA_GLOAL1_base  ((uint32_t)0xA0000000)
#define VDMA_GLOAL2_base  ((uint32_t)0xA0270000)


/*general dma register definition
*/
typedef struct {
    __IO uint32_t GDMA_SRC;		   /*!<  general dma source address register */
    __IO uint32_t GDMA_DST;		   /*!<  general dma destination address register */
    __IO uint32_t GDMA_WPPT;         /*!<  general dma wrap point address register */
    __IO uint32_t GDMA_WPTO;         /*!<  general dma wrap to address register */
    __IO uint32_t GDMA_COUNT;        /*!<  general dma transfer counter  register */
    __IO uint32_t GDMA_CON;          /*!<  general dma control register */
    __IO uint32_t GDMA_START;        /*!<  general dma start register */
    __IO  uint32_t GDMA_INTSTA;       /*!<  general dma interrupt status register*/
    __O  uint32_t GDMA_ACKINT;       /*!<  generall dma interrupt acknowledge register*/
    __I  uint32_t GDMA_RLCT;         /*!<  general dma remaining length of current transfer register*/
} GDMA_REGISTER_T;

/*general dma  global  status register definition
*/
typedef struct {
    __IO uint32_t GDMA_GLBSTA;         /*!<  general dma wrap point address register */
} GDMA_REGISTER_GLOBAL_T;


/*peripheral dma register definition
*/
typedef struct {
    __IO uint32_t PDMA_WPPT;         /*!<  peripheral dma wrap point address register */
    __IO uint32_t PDMA_WPTO;         /*!<  peripheral dma wrap to address register */
    __IO uint32_t PDMA_COUNT;        /*!<  peripheral dma transfer counter  register */
    __IO uint32_t PDMA_CON;          /*!<  peripheral dma control register */
    __IO uint32_t PDMA_START;        /*!<  peripheral dma start register */
    __IO  uint32_t PDMA_INTSTA;       /*!<  peripheral dma interrupt status register*/
    __O  uint32_t PDMA_ACKINT;       /*!<  peripheral dma interrupt acknowledge register*/
    __I  uint32_t PDMA_RLCT;         /*!<  peripheral dma remaining length of current transfer register*/
    __O  uint32_t  DUMMY_OFFSET[1];   /*!< peripheral fifo dma dummy offser register*/
    __IO uint32_t PDMA_PGMADDR;      /*!<  peripheral dma programmable address register*/
} PDMA_REGISTER_T;

/*peripheral dma  global  status register definition
*/
typedef struct {
    __IO uint32_t PDMA_GLBSTA;         /*!<  peripheral dma wrap point address register */
} PDMA_REGISTER_GLOBAL_T;


/*virtual fifo dma register definition
*/
typedef struct {
    __IO uint32_t VDMA_COUNT;        /*!<  virtual fifo dma transfer counter  register */
    __IO uint32_t VDMA_CON;          /*!<  virtual fifo dma control register */
    __IO uint32_t VDMA_START;        /*!<  virtual fifo dma start register */
    __IO uint32_t VDMA_INTSTA;       /*!<  virtual fifol dma interrupt status register*/
    __O  uint32_t VDMA_ACKINT;       /*!<  virtual fifo dma interrupt acknowledge register*/
    __O  uint32_t  DUMMY_OFFSET[2];      /*!< virtual fifo dma dummy offser register*/
    __IO uint32_t VDMA_PGMADDR;      /*!<  virtual fifo dma programmable address register*/
    __I  uint32_t VDMA_WRPTR;        /*!<  virtual fifo dma write pointer register */
    __I  uint32_t VDMA_RDPTR;        /*!<  virtual fifo dma read  pointer register */
    __I uint32_t VDMA_FFCNT;        /*!<  virtual fifo dma fifo count register */
    __I uint32_t VDMA_FFSTA;        /*!<  virtual fifo dma fifo status  register */
    __IO uint32_t VDMA_ALTLEN;        /*!<  virtual fifo dma fifo alert lentgh register */
    __IO uint32_t VDMA_FFSIZE;        /*!<  virtual fifo dma fifo size  register */

} VDMA_REGISTER_T;

/*virtual fifo dma  port address definition
*/
typedef struct {
    __IO uint32_t VDMA_PORT;         /*!<  virtual fifo dma port address register */
} VDMA_REGISTER_PORT_T;


/*virtual fifo dma  global  status register definition
*/
typedef struct {
    __IO uint32_t VDMA_GLBSTA;         /*!<  virtual fifo dma global status  register */
} VDMA_REGISTER_GLOBAL_T;


/*general dma base address definition
*/
#define GDMA1                   ((GDMA_REGISTER_T *) (GDMA1_base))

/*peripheral dma global status base address definition
*/
#define GDMA_GLOAL1                   ((GDMA_REGISTER_GLOBAL_T *) (GDMA_GLOAL1_base))


/*peripheral dma base address definition
*/
#define PDMA2                   ((PDMA_REGISTER_T *) (PDMA2_base))
#define PDMA3                   ((PDMA_REGISTER_T *) (PDMA3_base))
#define PDMA4                   ((PDMA_REGISTER_T *) (PDMA4_base))
#define PDMA5                   ((PDMA_REGISTER_T *) (PDMA5_base))
#define PDMA6                   ((PDMA_REGISTER_T *) (PDMA6_base))
#define PDMA7                   ((PDMA_REGISTER_T *) (PDMA7_base))

/*peripheral dma global status base address definition
*/
#define PDMA_GLOAL1                   ((PDMA_REGISTER_GLOBAL_T *) (PDMA_GLOAL1_base))
#define PDMA_GLOAL2                   ((PDMA_REGISTER_GLOBAL_T *) (PDMA_GLOAL2_base))

/*virtual fifo dma base address definition
*/
#define VDMA9                    ((VDMA_REGISTER_T *) (VDMA9_base))
#define VDMA10                   ((VDMA_REGISTER_T *) (VDMA10_base))
#define VDMA11                   ((VDMA_REGISTER_T *) (VDMA11_base))
#define VDMA12                   ((VDMA_REGISTER_T *) (VDMA12_base))
#define VDMA13                   ((VDMA_REGISTER_T *) (VDMA13_base))
#define VDMA14                   ((VDMA_REGISTER_T *) (VDMA14_base))
#define VDMA15                   ((VDMA_REGISTER_T *) (VDMA15_base))
#define VDMA16                   ((VDMA_REGISTER_T *) (VDMA16_base))
#define VDMA17                   ((VDMA_REGISTER_T *) (VDMA17_base))
#define VDMA18                   ((VDMA_REGISTER_T *) (VDMA18_base))

/*virtual fifo dma  port address definition
*/
#define VDMA9_PORT                    ((VDMA_REGISTER_PORT_T *) (VDMA9_port))
#define VDMA10_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA10_port))
#define VDMA11_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA11_port))
#define VDMA12_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA12_port))
#define VDMA13_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA13_port))
#define VDMA14_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA14_port))
#define VDMA15_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA15_port))
#define VDMA16_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA16_port))
#define VDMA17_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA17_port))
#define VDMA18_PORT                   ((VDMA_REGISTER_PORT_T *) (VDMA18_port))


/*virtual fifo  dma global status base address definition
*/
#define VDMA_GLOAL1                   ((VDMA_REGISTER_GLOBAL_T *) (VDMA_GLOAL1_base))
#define VDMA_GLOAL2                   ((VDMA_REGISTER_GLOBAL_T *) (VDMA_GLOAL2_base))


/*the bit value in gdma control  register
*/

#define GDMA_CON_SIZE_HALF_WORD_OFFSET       (0)
#define GDMA_CON_SIZE_HALF_WORD_MASK         (0x1<< GDMA_CON_SIZE_HALF_WORD_OFFSET)

#define GDMA_CON_SIZE_WORD_OFFSET            (1)
#define GDMA_CON_SIZE_WORD_MASK              (0x1<< GDMA_CON_SIZE_WORD_OFFSET)

#define GDMA_CON_SINC_OFFSET                  (2)
#define GDMA_CON_SINC_MASK                    (0x1<< GDMA_CON_SINC_OFFSET)

#define GDMA_CON_DINC_OFFSET                  (3)
#define GDMA_CON_DINC_MASK                     (0x1<< GDMA_CON_DINC_OFFSET)

#define GDMA_CON_BURST_4BEAT_OFFSET           (9)
#define GDMA_CON_BURST_4BEAT_MASK             (0x1<< GDMA_CON_BURST_4BEAT_OFFSET)


#define GDMA_CON_ITEN_OFFSET                   (15)
#define GDMA_CON_ITEN_MASK                     (0x1<< GDMA_CON_ITEN_OFFSET)

#define GDMA_CON_WPSD_OFFSET                   (16)
#define GDMA_CON_WPSD_MASK                      (0x1<< GDMA_CON_WPSD_OFFSET)

#define GDMA_CON_WPEN_OFFSET                   (17)
#define GDMA_CON_WPEN_MASK                      (0x1<< GDMA_CON_WPEN_OFFSET)

#define GDMA_RUNNING_STATUS_OFFSET              (0)
#define GDMA_RUNNING_STATUS_MASK                (0x1<< GDMA_RUNNING_STATUS_OFFSET)

#define GDMA_INTERRUPT_STATUS_OFFSET            (1)
#define GDMA_INTERRUPT_STATUS_MASK              (0x1<< GDMA_INTERRUPT_STATUS_OFFSET)


/*the bit value in gdma start  register
*/
#define GDMA_START_BIT_OFFSET            (15)
#define GDMA_START_BIT_MASK              (0x1<< GDMA_START_BIT_OFFSET)

#define GDMA_STOP_BIT_OFFSET            (15)
#define GDMA_STOP_BIT_MASK              (0x0<< GDMA_STOP_BIT_OFFSET)


/*the bit value in gdma interrput status  register
*/
#define GDMA_INTSTA_BIT_OFFSET            (15)
#define GDMA_INTSTA_BIT_MASK              (0x1<< GDMA_INTSTA_BIT_OFFSET)

/*the bit value in gdma acknowlege  register
*/
#define GDMA_ACKINT_BIT_OFFSET            (15)
#define GDMA_ACKINT_BIT_MASK              (0x1<< GDMA_ACKINT_BIT_OFFSET)



/*the bit value in pdma control  register
*/
#define PDMA_CON_SIZE_BYTE_OFFSET           (0)
#define PDMA_CON_SIZE_BYTE_MASK             (0x0<< PDMA_CON_SIZE_BYTE_OFFSET)

#define PDMA_CON_SIZE_HALF_WORD_OFFSET       (0)
#define PDMA_CON_SIZE_HALF_WORD_MASK         (0x1<< PDMA_CON_SIZE_HALF_WORD_OFFSET)

#define PDMA_CON_SIZE_WORD_OFFSET            (1)
#define PDMA_CON_SIZE_WORD_MASK              (0x1<< PDMA_CON_SIZE_WORD_OFFSET)

#define PDMA_CON_SINC_OFFSET                 (2)
#define PDMA_CON_SINC_MASK                   (0x1<< PDMA_CON_SINC_OFFSET)

#define PDMA_CON_DINC_OFFSET                 (3)
#define PDMA_CON_DINC_MASK                   (0x1<< PDMA_CON_DINC_OFFSET)

#define PDMA_CON_DREQ_OFFSET                 (4)
#define PDMA_CON_DREQ_MASK                   (0x1<< PDMA_CON_DREQ_OFFSET) /*peripheral device should enable this bit for handshake*/

#define PDMA_CON_B2W_OFFSET                 (5)
#define PDMA_CON_B2W_MASK                    (0x1<< PDMA_CON_B2W_OFFSET)


#define PDMA_CON_BURST_4BEAT_OFFSET           (9)
#define PDMA_CON_BURST_4BEAT_MASK             (0x1<< PDMA_CON_BURST_4BEAT_OFFSET)


#define PDMA_CON_ITEN_OFFSET                   (15)
#define PDMA_CON_ITEN_MASK                     (0x1<< PDMA_CON_ITEN_OFFSET)

#define PDMA_CON_WPSD_OFFSET                   (16)
#define PDMA_CON_WPSD_MASK                     (0x1<< PDMA_CON_WPSD_OFFSET)

#define PDMA_CON_WPEN_OFFSET                   (17)
#define PDMA_CON_WPEN_MASK                     (0x1<< PDMA_CON_WPEN_OFFSET)

#define PDMA_CON_DIR_OFFSET                    (18)
#define PDMA_CON_DIR_MASK                      (0x1<< PDMA_CON_DIR_OFFSET)



/*the bit value in pdma start  register
*/
#define PDMA_START_BIT_OFFSET            (15)
#define PDMA_START_BIT_MASK              (0x1<< PDMA_START_BIT_OFFSET)

#define PDMA_STOP_BIT_OFFSET            (15)
#define PDMA_STOP_BIT_MASK              (0x0<< PDMA_STOP_BIT_OFFSET)




/*the bit value in pdma acknowlege  register
*/
#define PDMA_ACKINT_BIT_OFFSET            (15)
#define PDMA_ACKINT_BIT_MASK              (0x1<< PDMA_ACKINT_BIT_OFFSET)


/*the bit mask definition in pdma control  register
*/
#define PDMA_CON_BURST_OFFSET            (8)
#define PDMA_CON_BURST_MASK              (0x3<< PDMA_CON_BURST_OFFSET)


/*pdma channel running offset definition
*/
#define PDMA_CHANNEL2_OFFSET    (2)

#define PDMA_CHANNEL3_OFFSET    (4)

#define PDMA_CHANNEL4_OFFSET    (2)

#define PDMA_CHANNEL5_OFFSET    (4)

#define PDMA_CHANNEL6_OFFSET    (6)

#define PDMA_CHANNEL7_OFFSET    (8)


/*the bit value in virtual fifo dma control  register
*/
#define VDMA_CON_SIZE_BYTE_OFFSET           (0)
#define VDMA_CON_SIZE_BYTE_MASK                  (0x0<< VDMA_CON_SIZE_BYTE_OFFSET)

#define VDMA_CON_SIZE_HALF_WORD_OFFSET      (0)
#define VDMA_CON_SIZE_HALF_WORD_MASK        (0x1<< VDMA_CON_SIZE_HALF_WORD_OFFSET)

#define VDMA_CON_SIZE_WORD_OFFSET           (1)
#define VDMA_CON_SIZE_WORD_MASK             (0x1<< VDMA_CON_SIZE_WORD_OFFSET)


#define VDMA_CON_DREQ_OFFSET                (4)
#define VDMA_CON_DREQ_MASK                  (0x1<< VDMA_CON_DREQ_OFFSET) /*peripheral device should enable this bit for handshake*/

#define VDMA_CON_ITEN_OFFSET                (15)
#define VDMA_CON_ITEN_MASK                  (0x1<< VDMA_CON_ITEN_OFFSET)

#define VDMA_CON_DIR_OFFSET                  (18)
#define VDMA_CON_DIR_MASK                    (0x1<< VDMA_CON_DIR_OFFSET)


/*the bit value in vdma start  register
*/
#define VDMA_START_BIT_OFFSET            (15)
#define VDMA_START_BIT_MASK              (0x1<< VDMA_START_BIT_OFFSET)

#define VDMA_STOP_BIT_OFFSET            (15)
#define VDMA_STOP_BIT_MASK               (0x0<< VDMA_STOP_BIT_OFFSET)


/*the bit value in virtual fifo dma acknowlege  register
*/
#define VDMA_ACKINT_BIT_OFFSET            (15)
#define VDMA_ACKINT_BIT_MASK                   (0x1<< VDMA_ACKINT_BIT_OFFSET)


/*the bit value in virtual fifo dma  fifo count register
*/
#define VDMA_FFSTA_FULL_OFFSET            (0)
#define VDMA_FFSTA_FULL_MASK              (0x1<< VDMA_FFSTA_FULL_OFFSET)

#define VDMA_FFSTA_EMPTY_OFFSET            (1)
#define VDMA_FFSTA_EMPTY_MASK	           (0x1<< VDMA_FFSTA_EMPTY_OFFSET)

#define VDMA_FFSTA_ALERT_OFFSET            (2)
#define VDMA_FFSTA_ALERT_MASK	           (0x1<< VDMA_FFSTA_ALERT_OFFSET)

#define PDMA_CHANNEL9_OFFSET     (16)

#define PDMA_CHANNEL10_OFFSET    (18)

#define PDMA_CHANNEL11_OFFSET    (20)

#define PDMA_CHANNEL12_OFFSET    (22)

#define PDMA_CHANNEL13_OFFSET    (24)

#define PDMA_CHANNEL14_OFFSET    (26)

#define PDMA_CHANNEL15_OFFSET    (28)

#define PDMA_CHANNEL16_OFFSET    (30)

#define PDMA_CHANNEL17_OFFSET    (16)

#define PDMA_CHANNEL18_OFFSET    (18)


/* *************************dma hardware definition end line**********************************
*/

/* *************************pwm hardware definition start line**********************************
*/

/*pwm register structure definition*/
typedef struct {
    __IO uint32_t PWM_CTRL;         /*!<  PWM control register Address offset: 0x00 */
    __IO uint32_t PWM_COUNT;        /*!< PWM counter register Address offset: 0x04 */
    __IO uint32_t PWM_THRESH;      /*!<  pwm thresh registerAddress offset:   0x08 */
} PWM_REGISTER_T;

#define PWM0                  ((PWM_REGISTER_T *) (PWM0_BASE))
#define PWM1                  ((PWM_REGISTER_T *) (PWM1_BASE))
#define PWM2                  ((PWM_REGISTER_T *) (PWM2_BASE))
#define PWM3                  ((PWM_REGISTER_T *) (PWM3_BASE))
#define PWM4                  ((PWM_REGISTER_T *) (PWM4_BASE))
#define PWM5                  ((PWM_REGISTER_T *) (PWM5_BASE))

/*the bit value in PWM  control  register*/
#define PWM_CLK_SEL_OFFSET            (2)
#define PWM_CLK_SEL_MASK              (0x1<< PWM_CLK_SEL_OFFSET)


/*the bit mask definition in  PWM control  register */
#define PWM_CLK_DIV_OFFSET            (0)
#define PWM_CLK_DIV_MASK                   (0x3<< PWM_CLK_DIV_OFFSET)



/* *************************pwm hardware definition end line**********************************
*/

/* *************************trng hardware definition start line**********************************
*/

typedef struct {
    __IO uint32_t TRNG_CTRL;         /*!<  trng control register Address offset: 0x00 */
    __IO uint32_t TRNG_TIME;         /*!<  trng time register Address offset: 0x04 */
    __I  uint32_t TRNG_DATA;         /*!<  trng data registerAddress offset: 0x08 */
    __IO uint32_t TRNG_CONF;         /*!<  trng configure registerAddress offset: 0x0C */
    __I  uint32_t TRNG_INT_SET;      /*!<  trng interrupt setting registerAddress offset: 0x10 */
    __IO uint32_t TRNG_INT_CLR;      /*!<  trng interrupt clean  registerAddress offset: 0x14 */
} TRNG_REGISTER_T;


#define TRNG                  ((TRNG_REGISTER_T *) (TRNG_BASE))


/* the bit value in TRNG  CONTROL register */
#define  TRNG_START_OFFSET            (0)
#define  TRNG_START_MASK              (0x1UL<< TRNG_START_OFFSET)

#define  TRNG_STOP_OFFSET            (0)
#define  TRNG_STOP_MASK              (0x1UL<< TRNG_STOP_OFFSET)

/* the bit value in TRNG INT_SET register */
#define  TRNG_RDY_OFFSET            (0)
#define  TRNG_RDY_MASK               (0x1UL<< TRNG_RDY_OFFSET)

/* the bit value in TRNG INT_SET register */
#define  TRNG_TIMEOUT_OFFSET             (1)
#define  TRNG_TIMEOUT_MASK               (0x1UL<< TRNG_TIMEOUT_OFFSET)


/* the bit value in TRNG INT_CLR register */
#define  TRNG_INT_CLR_OFFSET             (0)
#define  TRNG_INT_CLR_MASK               (0x1UL<< TRNG_INT_CLR_OFFSET)


/* the bit value in TRNG  CONF register */
#define  TRNG_H_FIRO_OFFSET         (2)
#define  TRNG_H_FIRO_EN_MASK        (0x1UL<< TRNG_H_FIRO_OFFSET)

#define  TRNG_H_RO_EN_OFFSET        (3)
#define  TRNG_H_RO_EN_MASK          (0x1UL<< TRNG_H_RO_EN_OFFSET)


#define  TRNG_H_GARO_OFFSET        (4)
#define  TRNG_H_GARO_EN_MASK       (0x1UL<< TRNG_H_GARO_OFFSET)


#define  TRNG_VON_EN_OFFSET        (5)
#define  TRNG_VON_EN_MASK          (0x1UL<< TRNG_VON_EN_OFFSET)




/* *************************trng hardware definition end line**********************************
*/

/*************************** GPT register definition start line  ******************************
 */
typedef struct {
    __I   uint32_t	GPT_IRQSTA;
    __IO  uint32_t  GPT_IRQMSK0;
    __IO  uint32_t  GPT_IRQMSK1;
} GPT_REGISTER_GLOABL_T;

typedef struct {
    __IO uint32_t GPT_CON ;					/*!< timer enable and mode config*/
    __IO uint32_t GPT_CLK ;                 /*!< clock select and prescale config*/
    __IO uint32_t GPT_IRQ_EN ;              /*!< interrupt enable*/
    __I  uint32_t GPT_IRQ_STA ;             /*!< interrupt status*/
    __O  uint32_t GPT_IRQ_ACK;             	/*!< interrupt ack*/
    __IO uint32_t GPT_COUNT ;               /*!< gpt0~4 count,gpt5 low word*/
    __IO uint32_t GPT_COMPARE ;             /*!< gpt0~4 threshold,gpt5 low word*/
    __IO uint32_t GPT_COUNTH;               /*!< count, only for gpt5 low word*/
    __IO uint32_t GPT_COMPAREH;             /*!< threshold, only for gpt5 low word*/
} GPT_REGISTER_T;


#define _GPT_BASE_                                  (GPT_BASE)
#define _GPT_BASEADDR_                              (_GPT_BASE_ + 0x000)
#define _GPT1_BASEADDR_                             (_GPT_BASE_ + 0x010)
#define _GPT2_BASEADDR_                             (_GPT_BASE_ + 0x040)
#define _GPT3_BASEADDR_                             (_GPT_BASE_ + 0x070)
#define _GPT4_BASEADDR_                             (_GPT_BASE_ + 0x0a0)
#define _GPT5_BASEADDR_                             (_GPT_BASE_ + 0x0d0)
#define _GPT6_BASEADDR_                             (_GPT_BASE_ + 0x100)

#define GPTGLB                                      ((GPT_REGISTER_GLOABL_T*)(_GPT_BASEADDR_))
#define GPT0                                        ((GPT_REGISTER_T*)(_GPT1_BASEADDR_))
#define GPT1                                        ((GPT_REGISTER_T*)(_GPT2_BASEADDR_))
#define GPT2                                        ((GPT_REGISTER_T*)(_GPT3_BASEADDR_))
#define GPT3                                        ((GPT_REGISTER_T*)(_GPT4_BASEADDR_))
#define GPT4                                        ((GPT_REGISTER_T*)(_GPT5_BASEADDR_))
#define GPT5                                        ((GPT_REGISTER_T*)(_GPT6_BASEADDR_))
/* Bit value definition for GPT register */

/* GPT_CON register definitions
 */

#define GPT_CON_EN_OFFSET                           (0)
#define GPT_CON_EN_MASK                             (0x1<<GPT_CON_EN_OFFSET)

#define GPT_CON_CLR_OFFSET                          (1)
#define GPT_CON_CLR_MASK                            (0x1<<GPT_CON_CLR_OFFSET)

#define GPT_CON_MODE_OFFSET                         (4)
#define GPT_CON_MODE_MASK                           (0x3<<GPT_CON_MODE_OFFSET)

#define GPT_CON_SW_CG_OFFSET                        (6)
#define GPT_CON_SW_CG_MASK                          (0x1<<GPT_CON_SW_CG_OFFSET)

/* GPT_CLK register definitions
 */
#define GPT_CLK_CLK_OFFSET                          (4)
#define GPT_CLK_CLK_MASK                            (0x1<<GPT_CLK_CLK_OFFSET)

#define GPT_CLK_CLKDIV_OFFSET                       (0)
#define GPT_CLK_CLKDIV_MASK                         (0xf<<GPT_CLK_CLKDIV_OFFSET)

/* GPT_IRQ_EN register definitions
 */
#define GPT_IRQ_EN_OFFSET                           (0)
#define GPT_IRQ_EN_MASK                             (0x1<<GPT_IRQ_EN_OFFSET)

/* GPT_IRQ_STA register definitions
 */
#define GPT_IRQ_STA_OFFSET                          (0)
#define GPT_IRQ_STA_MASK                            (0x1<<GPT_IRQ_STA_OFFSET)

/* GPT_IRQ_STA register definitions
 */
#define GPT_IRQ_ACK_OFFSET                          (0)
#define GPT_IRQ_ACK_MASK                            (0x1<<GPT_IRQ_ACK_OFFSET)

/*************************** GPT register definition end line  *******************************
 */

/************************ keypad register definition start line  *******************************
 */
typedef struct {
    __I  uint32_t KP_STA ;			/*!< keypad status press or release register */
    __I  uint32_t KP_MEM1 ;			/*!< keypad key position bit map register */
    __I  uint32_t KP_MEM2 ;			/*!< keypad key position bit map register */
    uint32_t reserved0[3];
    __IO uint32_t KP_DEBOUNCE ;		/*!< keypad debounce config register */
    __IO uint32_t KP_SCAN_TIMING;   /*!< keypad scan timing config register */
    __IO uint32_t KP_SEL ;			/*!< keypad column and row selection, single or double key selection */
    __IO uint32_t KP_EN ;			/*!< keypad module enable reigster */
} KEYPAD_REGISTER_T;


#define _KEYPAD_BASE_                               (KP_BASE)
#define KEYPAD_REGISTER                             ((KEYPAD_REGISTER_T*)(_KEYPAD_BASE_))

/* Bit value definition for keypad register */

/* KP_STA register definitions
 */
#define KEYPAD_KP_STA_OFFSET                        (0)
#define KEYPAD_KP_STA_MASK                          (1<<KEYPAD_KP_STA_OFFSET)

/* KP_SCAN_TIMING register definitions
 */
#define KEYPAD_KP_SCAN_TIMING_ROW_DIV_OFFSET        (0)
#define KEYPAD_KP_SCAN_TIMING_ROW_DIV_MASK          (0xf<<KEYPAD_KP_SCAN_TIMING_ROW_DIV_OFFSET)

#define KEYPAD_KP_SCAN_TIMING_COL_DIV_OFFSET        (4)
#define KEYPAD_KP_SCAN_TIMING_COL_DIV_MASK          (0xf<<KEYPAD_KP_SCAN_TIMING_COL_DIV_OFFSET)

#define KEYPAD_KP_SCAN_TIMING_ROW_HIGH_OFFSET       (8)
#define KEYPAD_KP_SCAN_TIMING_ROW_HIGH_MASK         (0xf<<KEYPAD_KP_SCAN_TIMING_ROW_HIGH_OFFSET)

#define KEYPAD_KP_SCAN_TIMING_COL_HIGH_OFFSET       (12)
#define KEYPAD_KP_SCAN_TIMING_COL_HIGH_MASK         (0xf<<KEYPAD_KP_SCAN_TIMING_COL_HIGH_OFFSET)

/* KP_SEL register definitions
 */
#define KEYPAD_KP_SEL_EN_OFFSET                     (0)
#define KEYPAD_KP_SEL_EN_MASK                       (0x1<<KEYPAD_KP_SEL_EN_OFFSET)

#define KEYPAD_KP_SEL_ROW_OFFSET                    (4)
#define KEYPAD_KP_SEL_ROW_MASK                      (0x3f<<KEYPAD_KP_SEL_ROW_OFFSET)

#define KEYPAD_KP_SEL_COL_OFFSET                    (10)
#define KEYPAD_KP_SEL_COL_MASK                      (0x3f<<KEYPAD_KP_SEL_COL_OFFSET)

/* KP_EN register definitions
 */
#define KEYPAD_KP_EN_OFFSET                         (0)
#define KEYPAD_KP_EN_MASK                           (0x1<<KEYPAD_KP_EN_OFFSET)


/************************ keypad register definition end line  *******************************
 */

/*************************** GPIO register definition start line  *******************************
 */

typedef struct {
    __IO uint32_t GPIO_DIR;        /*!< GPIO direction register */
    __IO uint32_t GPIO_DIR_SET;    /*!< GPIO direction set register */
    __IO uint32_t GPIO_DIR_CLR;    /*!< GPIO direction clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_DIR_REGISTER_T;



typedef struct {
    __IO uint32_t GPIO_PULLEN;        /*!< GPIO pullen register */
    __IO uint32_t GPIO_PULLEN_SET;    /*!< GPIO pullen set mode register */
    __IO uint32_t GPIO_PULLEN_CLR;    /*!< GPIO pullen clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_PULLEN_REGISTER_T;


typedef struct {
    __IO uint32_t GPIO_DINV;        /*!< GPIO input inverse control register */
    __IO uint32_t GPIO_DINV_SET;    /*!< GPIO input inverse control set register */
    __IO uint32_t GPIO_DINV_CLR;    /*!< GPIO input inverse control clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_DINV_REGISTER_T;



typedef struct {
    __IO uint32_t GPIO_DOUT;        /*!< GPIO output data register */
    __IO uint32_t GPIO_DOUT_SET;    /*!< GPIO output data set register */
    __IO uint32_t GPIO_DOUT_CLR;    /*!< GPIO output data clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_DOUT_REGISTER_T;


typedef struct {
    __IO uint32_t GPIO_DIN;         /*!< GPIO input data register */
    uint8_t  GPIO_OFFSET[12];
} GPIO_DIN_REGISTER_T;


typedef struct {
    __IO uint32_t GPIO_PULLSEL;        /*!< GPIO pullsel register */
    __IO uint32_t GPIO_PULLSEL_SET;    /*!< GPIO pullsel set register */
    __IO uint32_t GPIO_PULLSEL_CLR;    /*!< GPIO pullsel clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_PULLSEL_REGISTER_T;


typedef struct {
    __IO uint32_t GPIO_SMT;        /*!< GPIO schmitt trigger register */
    __IO uint32_t GPIO_SMT_SET;    /*!< GPIO schmitt trigger set register */
    __IO uint32_t GPIO_SMT_CLR;    /*!< GPIO schmitt trigger clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_SMT_REGISTER_T;

typedef struct {
    __IO uint32_t GPIO_SR;        /*!< GPIO slew rate control register */
    __IO uint32_t GPIO_SR_SET;    /*!< GPIO slew rate control set register */
    __IO uint32_t GPIO_SR_CLR;    /*!< GPIO slew rate control clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_SR_REGISTER_T;


typedef struct {
    __IO uint32_t GPIO_DRV;        /*!< GPIO driving control register */
    __IO uint32_t GPIO_DRV_SET;    /*!< GPIO driving control set register */
    __IO uint32_t GPIO_DRV_CLR;    /*!< GPIO driving control clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_DRV_REGISTER_T;


typedef struct {
    __IO uint32_t GPIO_IES;        /*!< GPIO enable input buffer control register */
    __IO uint32_t GPIO_IES_SET;    /*!< GPIO enable input buffer control set register */
    __IO uint32_t GPIO_IES_CLR;    /*!< GPIO enable input buffer control clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_IES_REGISTER_T;



typedef struct {
    __IO uint32_t GPIO_PUPD;        /*!< GPIO pupd control register */
    __IO uint32_t GPIO_PUPD_SET;    /*!< GPIO pupd control set register */
    __IO uint32_t GPIO_PUPD_CLR;    /*!< GPIO pupd control clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_PUPD_REGISTER_T;



typedef struct {
    __IO uint32_t GPIO_RESEN0;        /*!< GPIO r0 control register */
    __IO uint32_t GPIO_RESEN0_SET;    /*!< GPIO r0 control set register */
    __IO uint32_t GPIO_RESEN0_CLR;    /*!< GPIO r0 control clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_RESEN0_REGISTER_T;


typedef struct {
    __IO uint32_t GPIO_RESEN1;        /*!< GPIO r1 control register */
    __IO uint32_t GPIO_RESEN1_SET;    /*!< GPIO r1 control set register */
    __IO uint32_t GPIO_RESEN1_CLR;    /*!< GPIO r1 control clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_RESEN1_REGISTER_T;


typedef struct {
    __IO uint32_t GPIO_MODE;        /*!< GPIO mode control register */
    __IO uint32_t GPIO_MODE_SET;    /*!< GPIO mode control set register */
    __IO uint32_t GPIO_MODE_CLR;    /*!< GPIO mode control clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_MODE_REGISTER_T;


typedef struct {
    __IO uint32_t GPIO_TDSEL;        /*!< GPIO tdsel control register */
    __IO uint32_t GPIO_TDSEL_SET;    /*!< GPIO tdsel control set register */
    __IO uint32_t GPIO_TDSEL_CLR;    /*!< GPIO tdsel control clear register */
    uint8_t  GPIO_OFFSET[4];
} GPIO_TDSEL_REGISTER_T;


typedef struct {
    __IO uint32_t GPIO_CLKOUT;        /*!< GPIO clockout control register */
    uint8_t  GPIO_OFFSET[12];
} GPIO_CLKOUT_REGISTER_T;


typedef struct {
    GPIO_DIR_REGISTER_T GPIO_DIR_REGISTER[2];             /*!< GPIO direction register */
    uint8_t  GPIO_OFFSET1[224];
    GPIO_PULLEN_REGISTER_T GPIO_PULLEN_REGISTER[2];       /*!< GPIO pullen register */
    uint8_t  GPIO_OFFSET2[224];
    GPIO_DINV_REGISTER_T GPIO_DINV_REGISTER[2];           /*!< GPIO DINV mode register */
    uint8_t  GPIO_OFFSET3[224];
    GPIO_DOUT_REGISTER_T GPIO_DOUT_REGISTER[2];           /*!< GPIO output data register */
    uint8_t  GPIO_OFFSET4[224];
    GPIO_DIN_REGISTER_T GPIO_DIN_REGISTER[2];             /*!< GPIO input data register */
    uint8_t  GPIO_OFFSET5[224];
    GPIO_PULLSEL_REGISTER_T GPIO_PULLSEL_REGISTER[2];     /*!< GPIO PULLSEL register */
    uint8_t  GPIO_OFFSET6[224];
    GPIO_SMT_REGISTER_T GPIO_SMT_REGISTER[2];             /*!< GPIO SMT register */
    uint8_t  GPIO_OFFSET7[224];
    GPIO_SR_REGISTER_T GPIO_SR_REGISTER[2];               /*!< GPIO SR register */
    uint8_t  GPIO_OFFSET8[224];
    GPIO_DRV_REGISTER_T GPIO_DRV_REGISTER[4];             /*!< GPIO DRV register */
    uint8_t  GPIO_OFFSET9[192];
    GPIO_IES_REGISTER_T GPIO_IES_REGISTER[2];             /*!< GPIO IES register */
    uint8_t  GPIO_OFFSET10[224];
    GPIO_PUPD_REGISTER_T GPIO_PUPD_REGISTER[2];           /*!< GPIO PUPD register */
    uint8_t  GPIO_OFFSET11[224];
    GPIO_RESEN0_REGISTER_T GPIO_RESEN0_REGISTER[2];       /*!< GPIO R0 register */
    GPIO_RESEN1_REGISTER_T GPIO_RESEN1_REGISTER[2];       /*!< GPIO R1 register */
    uint8_t  GPIO_OFFSET12[192];
    GPIO_MODE_REGISTER_T GPIO_MODE_REGISTER[7];           /*!< GPIO mode register */
    uint8_t  GPIO_OFFSET13[144];
    GPIO_TDSEL_REGISTER_T GPIO_TDSEL_REGISTER[4];         /*!< GPIO TDSEL register */
    uint8_t  GPIO_OFFSET14[192];
    GPIO_CLKOUT_REGISTER_T GPIO_CLKOUT_REGISTER[6];       /*!< GPIO clockout register */

} GPIO_REGISTER_T;


/*************************** GPIO register definition end line  *******************************
 */

/* Register definitions for SPI Master */
typedef struct {
    __IO uint32_t SPI_CFG0;           /*!<SPI configuration 0 register.           Address offset: 0x00 */
    __IO uint32_t SPI_CFG1;           /*!<SPI configuration 1 register.           Address offset: 0x04 */
    __IO uint32_t SPI_TX_SRC;         /*!<SPI tx source address register.         Address offset: 0x08 */
    __IO uint32_t SPI_RX_DST;         /*!<SPI rx destination address register.    Address offset: 0x0C */
    __O  uint32_t SPI_TX_DATA;        /*!<SPI tx data fifo register.              Address offset: 0x10 */
    __I  uint32_t SPI_RX_DATA;        /*!<SPI rx data fifo register.              Address offset: 0x14 */
    __IO uint32_t SPI_CMD;            /*!<SPI command register.                   Address offset: 0x18 */
    __I  uint32_t SPI_STATUS0;        /*!<SPI status 0 register.                  Address offset: 0x1C */
    __I  uint32_t SPI_STATUS1;        /*!<SPI status 1 register.                  Address offset: 0x20 */
    __IO uint32_t SPI_PAD_MACRO_SEL;  /*!<SPI pad_macro selection register.       Address offset: 0x24 */
    __IO uint32_t SPI_CFG2;           /*!<SPI configuration 2 register.           Address offset: 0x28 */
} SPI_MASTER_REGISTER_T;

#define  SPI_MASTER_0                    ((SPI_MASTER_REGISTER_T *) SPI_MASTER0_BASE)
#define  SPI_MASTER_1                    ((SPI_MASTER_REGISTER_T *) SPI_MASTER1_BASE)
#define  SPI_MASTER_2                    ((SPI_MASTER_REGISTER_T *) SPI_MASTER2_BASE)
#define  SPI_MASTER_3                    ((SPI_MASTER_REGISTER_T *) SPI_MASTER3_BASE)

/* Bit value definition for SPIM register */

/* SPI_CFG0 register definitions
 */
#define SPI_CFG0_CS_HOLD_COUNT_OFFSET          (0)
#define SPI_CFG0_CS_HOLD_COUNT_MASK            (0xffffU<<SPI_CFG0_CS_HOLD_COUNT_OFFSET)

#define SPI_CFG0_CS_SETUP_COUNT_OFFSET         (16)
#define SPI_CFG0_CS_SETUP_COUNT_MASK           (0xffffU<<SPI_CFG0_CS_SETUP_COUNT_OFFSET)

/* SPI_CFG1 register definitions
 */
#define SPI_CFG1_CS_IDLE_COUNT_OFFSET          (0)
#define SPI_CFG1_CS_IDLE_COUNT_MASK            (0xffU<<SPI_CFG1_CS_IDLE_COUNT_OFFSET)

#define SPI_CFG1_PACKET_LOOP_COUNT_OFFSET      (8)
#define SPI_CFG1_PACKET_LOOP_COUNT_MASK        (0xffU<<SPI_CFG1_PACKET_LOOP_COUNT_OFFSET)

#define SPI_CFG1_PACKET_LENGTH_OFFSET          (16)
#define SPI_CFG1_PACKET_LENGTH_MASK            (0x3ffU<<SPI_CFG1_PACKET_LENGTH_OFFSET)

#define SPI_CFG1_DEVICE_SEL_OFFSET             (26)
#define SPI_CFG1_DEVICE_SEL_MASK               (0x1U<<SPI_CFG1_DEVICE_SEL_OFFSET)

#define SPI_CFG1_GET_TICK_DLY_OFFSET           (29)
#define SPI_CFG1_GET_TICK_DLY_MASK             (0x7U<<SPI_CFG1_GET_TICK_DLY_OFFSET)

/* SPI_CMD register definitions
 */
#define SPI_CMD_CMD_ACT_OFFSET                 (0)
#define SPI_CMD_CMD_ACT_MASK                   (0x1U<<SPI_CMD_CMD_ACT_OFFSET)

#define SPI_CMD_RESUME_OFFSET                  (1)
#define SPI_CMD_RESUME_MASK                    (0x1U<<SPI_CMD_RESUME_OFFSET)

#define SPI_CMD_RST_OFFSET                     (2)
#define SPI_CMD_RST_MASK                       (0x1U<<SPI_CMD_RST_OFFSET)

#define SPI_CMD_PAUSE_EN_OFFSET                (4)
#define SPI_CMD_PAUSE_EN_MASK                  (0x1U<<SPI_CMD_PAUSE_EN_OFFSET)

#define SPI_CMD_CS_DEASSERT_EN_OFFSET          (5)
#define SPI_CMD_CS_DEASSERT_EN_MASK            (0x1U<<SPI_CMD_CS_DEASSERT_EN_OFFSET)

#define SPI_CMD_SAMPLE_SEL_OFFSET              (6)
#define SPI_CMD_SAMPLE_SEL_MASK                (0x1U<<SPI_CMD_SAMPLE_SEL_OFFSET)

#define SPI_CMD_CS_POL_OFFSET                  (7)
#define SPI_CMD_CS_POL_MASK                    (0x1U<<SPI_CMD_CS_POL_OFFSET)

#define SPI_CMD_CPHA_OFFSET                    (8)
#define SPI_CMD_CPHA_MASK                      (0x1U<<SPI_CMD_CPHA_OFFSET)

#define SPI_CMD_CPOL_OFFSET                    (9)
#define SPI_CMD_CPOL_MASK                      (0x1U<<SPI_CMD_CPOL_OFFSET)

#define SPI_CMD_RX_DMA_EN_OFFSET               (10)
#define SPI_CMD_RX_DMA_EN_MASK                 (0x1U<<SPI_CMD_RX_DMA_EN_OFFSET)

#define SPI_CMD_TX_DMA_EN_OFFSET               (11)
#define SPI_CMD_TX_DMA_EN_MASK                 (0x1U<<SPI_CMD_TX_DMA_EN_OFFSET)

#define SPI_CMD_TXMSBF_OFFSET                  (12)
#define SPI_CMD_TXMSBF_MASK                    (0x1U<<SPI_CMD_TXMSBF_OFFSET)

#define SPI_CMD_RXMSBF_OFFSET                  (13)
#define SPI_CMD_RXMSBF_MASK                    (0x1U<<SPI_CMD_RXMSBF_OFFSET)

#define SPI_CMD_RX_ENDIAN_OFFSET               (14)
#define SPI_CMD_RX_ENDIAN_MASK                 (0x1U<<SPI_CMD_RX_ENDIAN_OFFSET)

#define SPI_CMD_TX_ENDIAN_OFFSET               (15)
#define SPI_CMD_TX_ENDIAN_MASK                 (0x1U<<SPI_CMD_TX_ENDIAN_OFFSET)

#define SPI_CMD_FINISH_IE_OFFSET               (16)
#define SPI_CMD_FINISH_IE_MASK                 (0x1U<<SPI_CMD_FINISH_IE_OFFSET)

#define SPI_CMD_PAUSE_IE_OFFSET                (17)
#define SPI_CMD_PAUSE_IE_MASK                  (0x1U<<SPI_CMD_PAUSE_IE_OFFSET)

/* SPI_STATUS0 register definitions
 */
#define SPI_STATUS0_FINISH_OFFSET              (0)
#define SPI_STATUS0_FINISH_MASK                (0x1U<<SPI_STATUS0_FINISH_OFFSET)

#define SPI_STATUS0_PAUSE_OFFSET               (1)
#define SPI_STATUS0_PAUSE_MASK                 (0x1U<<SPI_STATUS0_PAUSE_OFFSET)

/* SPI_STATUS1 register definitions
 */
#define SPI_STATUS1_BUSY_OFFSET                (0)
#define SPI_STATUS1_BUSY_MASK                  (0x1U<<SPI_STATUS1_BUSY_OFFSET)

/* SPI_PAD_MACRO_SEL register definitions
 */
#define SPI_PAD_MACRO_SEL_OFFSET               (0)
#define SPI_PAD_MACRO_SEL_MASK                 (0x7U<<SPI_PAD_MACRO_SEL_OFFSET)

/* SPI_CFG2 register definitions
 */
#define SPI_CFG2_SCK_HIGH_COUNT_OFFSET         (0)
#define SPI_CFG2_SCK_HIGH_COUNT_MASK           (0xffffU<<SPI_CFG2_SCK_HIGH_COUNT_OFFSET)

#define SPI_CFG2_SCK_LOW_COUNT_OFFSET          (16)
#define SPI_CFG2_SCK_LOW_COUNT_MASK            (0xffffU<<SPI_CFG2_SCK_LOW_COUNT_OFFSET)


/* Register definitions for SPI Slave */
typedef struct {
    __I  uint32_t SPISLV_TRANS_TYPE;            /*!<  Address offset: 0x00 */
    __I  uint32_t SPISLV_TRANS_LENGTH;          /*!<  Address offset: 0x04 */
    __I  uint32_t SPISLV_TRANS_ADDR;            /*!<  Address offset: 0x08 */
    __IO uint32_t SPISLV_CTRL;                  /*!<  Address offset: 0x0C */
    __IO uint32_t SPISLV_STATUS;                /*!<  Address offset: 0x10 */
    __IO uint32_t SPISLV_TIMOUT_THR;            /*!<  Address offset: 0x14 */
    __IO uint32_t SPISLV_SW_RST;                /*!<  Address offset: 0x18 */
    __IO uint32_t SPISLV_BUFFER_BASE_ADDR;      /*!<  Address offset: 0x1C */
    __IO uint32_t SPISLV_BUFFER_SIZE;           /*!<  Address offset: 0x20 */
    __I  uint32_t SPISLV_IRQ;                   /*!<  Address offset: 0x24 */
    __IO uint32_t SPISLV_MISO_EARLY_HALF_SCK;   /*!<  Address offset: 0x28 */
    __IO uint32_t SPISLV_CMD_DEFINE0;           /*!<  Address offset: 0x2C */
    __IO uint32_t SPISLV_CMD_DEFINE1;           /*!<  Address offset: 0x30 */
} SPI_SLAVE_REGISTER_T;

#define SPI_SLAVE                   ((SPI_SLAVE_REGISTER_T *) (SPI_SLAVE_BASE))

/* SPISLV_CTRL register definitions
 */
#define SPISLV_CTRL_SIZE_OFFSET                (0)
#define SPISLV_CTRL_SIZE_MASK                  (0x1U<<SPISLV_CTRL_SIZE_OFFSET)

#define SPISLV_CTRL_CPHA_OFFSET                (1)
#define SPISLV_CTRL_CPHA_MASK                  (0x1U<<SPISLV_CTRL_CPHA_OFFSET)

#define SPISLV_CTRL_CPOL_OFFSET                (2)
#define SPISLV_CTRL_CPOL_MASK                  (0x1U<<SPISLV_CTRL_CPOL_OFFSET)

#define SPISLV_CTRL_INTERRUPT_ALL_OFFSET       (3)
#define SPISLV_CTRL_INTERRUPT_ALL_MASK         (0xffU<<SPISLV_CTRL_INTERRUPT_ALL_OFFSET)

#define SPISLV_CTRL_RXMSBF_OFFSET              (11)
#define SPISLV_CTRL_RXMSBF_MASK                (0x1U<<SPISLV_CTRL_RXMSBF_OFFSET)

#define SPISLV_CTRL_TXMSBF_OFFSET              (12)
#define SPISLV_CTRL_TXMSBF_MASK                (0x1U<<SPISLV_CTRL_TXMSBF_OFFSET)

#define SPISLV_CTRL_RX_DMA_SW_READY_OFFSET     (13)
#define SPISLV_CTRL_RX_DMA_SW_READY_MASK       (0x1U<<SPISLV_CTRL_RX_DMA_SW_READY_OFFSET)

#define SPISLV_CTRL_TX_DMA_SW_READY_OFFSET     (14)
#define SPISLV_CTRL_TX_DMA_SW_READY_MASK       (0x1U<<SPISLV_CTRL_TX_DMA_SW_READY_OFFSET)

#define SPISLV_CTRL_SW_DECODE_OFFSET           (15)
#define SPISLV_CTRL_SW_DECODE_MASK             (0x1U<<SPISLV_CTRL_SW_DECODE_OFFSET)

/* SPISLV_STATUS register definitions
 */
#define SPISLV_STATUS_SLV_ON_OFFSET            (0)
#define SPISLV_STATUS_SLV_ON_MASK              (0x1U<<SPISLV_STATUS_SLV_ON_OFFSET)

#define SPISLV_STATUS_CFG_SUCCESS_OFFSET       (1)
#define SPISLV_STATUS_CFG_SUCCESS_MASK         (0x1U<<SPISLV_STATUS_CFG_SUCCESS_OFFSET)

#define SPISLV_STATUS_TXRX_FIFO_RDY_OFFSET     (2)
#define SPISLV_STATUS_TXRX_FIFO_RDY_MASK       (0x1U<<SPISLV_STATUS_TXRX_FIFO_RDY_OFFSET)

#define SPISLV_STATUS_RD_ERR_OFFSET            (3)
#define SPISLV_STATUS_RD_ERR_MASK              (0x1U<<SPISLV_STATUS_RD_ERR_OFFSET)

#define SPISLV_STATUS_WR_ERR_OFFSET            (4)
#define SPISLV_STATUS_WR_ERR_MASK              (0x1U<<SPISLV_STATUS_WR_ERR_OFFSET)

#define SPISLV_STATUS_RDWR_FINISH_OFFSET       (5)
#define SPISLV_STATUS_RDWR_FINISH_MASK         (0x1U<<SPISLV_STATUS_RDWR_FINISH_OFFSET)

#define SPISLV_STATUS_TIMOUT_ERR_OFFSET        (6)
#define SPISLV_STATUS_TIMOUT_ERR_MASK          (0x1U<<SPISLV_STATUS_TIMOUT_ERR_OFFSET)

#define SPISLV_STATUS_CMD_ERR_OFFSET           (7)
#define SPISLV_STATUS_CMD_ERR_MASK             (0x1U<<SPISLV_STATUS_CMD_ERR_OFFSET)

/* SPISLV_SW_RST register definitions
 */
#define SPISLV_SW_RST_OFFSET                   (0)
#define SPISLV_SW_RST_MASK                     (0x1U<<SPISLV_SW_RST_OFFSET)

/* SPISLV_IRQ register definitions
 */
#define SPISLV_IRQ_OFFSET                      (0)
#define SPISLV_IRQ_MASK                        (0x1ffU<<SPISLV_IRQ_OFFSET)

#define SPISLV_IRQ_CRD_FINISH_IRQ_OFFSET       (0)
#define SPISLV_IRQ_CRD_FINISH_IRQ_MASK         (0x1U<<SPISLV_IRQ_CRD_FINISH_IRQ_OFFSET)

#define SPISLV_IRQ_CWR_FINISH_IRQ_OFFSET       (1)
#define SPISLV_IRQ_CWR_FINISH_IRQ_MASK         (0x1U<<SPISLV_IRQ_CWR_FINISH_IRQ_OFFSET)

#define SPISLV_IRQ_RD_FINISH_IRQ_OFFSET        (2)
#define SPISLV_IRQ_RD_FINISH_IRQ_MASK          (0x1U<<SPISLV_IRQ_RD_FINISH_IRQ_OFFSET)

#define SPISLV_IRQ_WR_FINISH_IRQ_OFFSET        (3)
#define SPISLV_IRQ_WR_FINISH_IRQ_MASK          (0x1U<<SPISLV_IRQ_WR_FINISH_IRQ_OFFSET)

#define SPISLV_IRQ_POWEROFF_IRQ_OFFSET         (4)
#define SPISLV_IRQ_POWEROFF_IRQ_MASK           (0x1U<<SPISLV_IRQ_POWEROFF_IRQ_OFFSET)

#define SPISLV_IRQ_POWERON_IRQ_OFFSET          (5)
#define SPISLV_IRQ_POWERON_IRQ_MASK            (0x1U<<SPISLV_IRQ_POWERON_IRQ_OFFSET)

#define SPISLV_IRQ_RD_ERR_IRQ_OFFSET           (6)
#define SPISLV_IRQ_RD_ERR_IRQ_MASK             (0x1U<<SPISLV_IRQ_RD_ERR_IRQ_OFFSET)

#define SPISLV_IRQ_WR_ERR_IRQ_OFFSET           (7)
#define SPISLV_IRQ_WR_ERR_IRQ_MASK             (0x1U<<SPISLV_IRQ_WR_ERR_IRQ_OFFSET)

#define SPISLV_IRQ_TIMEOUT_ERR_IRQ_OFFSET      (8)
#define SPISLV_IRQ_TIMEOUT_ERR_IRQ_MASK        (0x1U<<SPISLV_IRQ_TIMEOUT_ERR_IRQ_OFFSET)

/* SPISLV_MISO_EARLY_HALF_SCK register definitions
 */
#define SPISLV_MISO_EARLY_HALF_SCK_OFFSET      (0)
#define SPISLV_MISO_EARLY_HALF_SCK_MASK        (0x1U<<SPISLV_MISO_EARLY_HALF_SCK_OFFSET)

/* SPISLV_CMD_DEFINE0 register definitions
 */
#define SPISLV_CMD_RD_OFFSET                   (0)
#define SPISLV_CMD_RD_MASK                     (0xffU<<SPISLV_CMD_RD_OFFSET)

#define SPISLV_CMD_WR_OFFSET                   (8)
#define SPISLV_CMD_WR_MASK                     (0xffU<<SPISLV_CMD_WR_OFFSET)

#define SPISLV_CMD_RS_OFFSET                   (16)
#define SPISLV_CMD_RS_MASK                     (0xffU<<SPISLV_CMD_RS_OFFSET)

#define SPISLV_CMD_WS_OFFSET                   (24)
#define SPISLV_CMD_WS_MASK                     (0xffU<<SPISLV_CMD_WS_OFFSET)

/* SPISLV_CMD_DEFINE1 register definitions
 */
#define SPISLV_CMD_CR_OFFSET                   (0)
#define SPISLV_CMD_CR_MASK                     (0xffU<<SPISLV_CMD_CR_OFFSET)

#define SPISLV_CMD_CW_OFFSET                   (8)
#define SPISLV_CMD_CW_MASK                     (0xffU<<SPISLV_CMD_CW_OFFSET)

#define SPISLV_CMD_POWERON_OFFSET              (16)
#define SPISLV_CMD_POWERON_MASK                (0xffU<<SPISLV_CMD_POWERON_OFFSET)

#define SPISLV_CMD_POWEROFF_OFFSET             (24)
#define SPISLV_CMD_POWEROFF_MASK               (0xffU<<SPISLV_CMD_POWEROFF_OFFSET)


/* Register definitions for I2C Master */
typedef struct {
    __IO uint16_t DATA_PORT;         /* 00 */
    __IO uint16_t RESERVED1;         /*    */
    __IO uint16_t SLAVE_ADDR;        /* 04 */
    __IO uint16_t RESERVED2;         /*    */
    __IO uint16_t INTR_MASK;		 /* 08 */
    __IO uint16_t RESERVED3;         /*    */
    __IO uint16_t INTR_STAT;		 /* 0C */
    __IO uint16_t RESERVED4;         /*    */
    __IO uint16_t CONTROL;		     /* 10 */
    __IO uint16_t RESERVED5;         /*    */
    __IO uint16_t TRANSFER_LEN;		 /* 14 */
    __IO uint16_t RESERVED6;         /*    */
    __IO uint16_t TRANSAC_LEN;		 /* 18 */
    __IO uint16_t RESERVED7;         /*    */
    __IO uint16_t DELAY_LEN;		 /* 1C */
    __IO uint16_t RESERVED8;         /*    */
    __IO uint16_t TIMING;		     /* 20 */
    __IO uint16_t RESERVED9;         /*    */
    __IO uint16_t START;		     /* 24 */
    __IO uint16_t RESERVED10;        /*    */
    __IO uint16_t RESERVED11[2];     /*    */
    __IO uint16_t CLOCK_DIV;		 /* 2C */
    __IO uint16_t RESERVED12;        /*    */
    __IO uint16_t FIFO_STAT;		 /* 30 */
    __IO uint16_t RESERVED13;        /*    */
    __IO uint16_t FIFO_THRESH;		 /* 34 */
    __IO uint16_t RESERVED14;        /*    */
    __IO uint16_t FIFO_ADDR_CLR;	 /* 38 */
    __IO uint16_t RESERVED15;        /*    */
    __IO uint16_t RESERVED16[2];     /*    */
    __IO uint16_t IO_CONFIG;		 /* 40 */
    __IO uint16_t RESERVED17;        /*    */
    __IO uint16_t RESERVED18[2];
    __IO uint16_t HS;		         /* 48 */
    __IO uint16_t RESERVED19;        /*    */
    __IO uint16_t RESERVED20[2];
    __IO uint16_t SOFTRESET;		 /* 50 */
    __IO uint16_t RESERVED21;        /*    */
    __IO uint16_t RESERVED22[6];     /*    */
    __IO uint16_t SPARE;		     /* 60 */
    __IO uint16_t RESERVED23;        /*    */
    __IO uint16_t DEBUGSTAT;		 /* 64 */
    __IO uint16_t RESERVED24;        /*    */
    __IO uint16_t DEBUGCTRL;		 /* 68 */
    __IO uint16_t RESERVED25;        /*    */
    __IO uint16_t TRANSFER_LEN_AUX;  /* 6C */
    __IO uint16_t RESERVED26;        /*    */
    __IO uint16_t RESERVED27[2];     /*    */
    __IO uint16_t TIMEOUT;           /* 74 */
    __IO uint16_t RESERVED28;        /*    */

} I2C_REGISTER_T;

/* Register definitions for WDT */
typedef struct {
    __IO uint32_t WDT_MODE;         /* WDT MODE register */
    __IO uint32_t WDT_LENGTH;       /* WDT Length register */
    __IO uint32_t WDT_RESTART;      /* WDT RESTART register */
    __IO uint32_t WDT_STA;          /* WDT STA register */
    __IO uint32_t reserved1;        /* reserved */
    __IO uint32_t reserved2;        /* reserved */
    __IO uint32_t WDT_INTERVAL;     /* WDT INTERVAL register */
    __IO uint32_t WDT_SWRST;        /* WDT SWRST register */
    __IO uint32_t WDT_CON0;        /* WDT SWRST register */
} WDT_REGISTER_T;

#define WDT_REGISTER  ((WDT_REGISTER_T *)(RGU_BASE)) /* CM4_WDT_BASE */


/*************************** Clock register definition start line  ******************************
 */
typedef struct {
    __IO uint32_t MSDC_CFG;         /* MSDC_CFG @A002_0000 */
} CLK_USED_MSDC0_REGISTER_T;

typedef struct {
    __IO uint32_t MSDC_CFG;         /* MSDC_CFG @A003_0000 */
} CLK_USED_MSDC1_REGISTER_T;

/* MSDC_CFG used offset and mask definitions */
#define MSDC_CFG_MSDC_MUX_SEL_OFFSET           (3)
#define MSDC_CFG_MSDC_MUX_SEL_MASK             (0x100003<<MSDC_CFG_MSDC_MUX_SEL_OFFSET)
#define MSDC_CFG_MSDC_MUX_SEL_NR_INPUTS        (8)
#define MSDC_CFG_MSDC_MUX_SEL_LSB_MASK         (3)
#define MSDC_CFG_MSDC_MUX_SEL_MSB_MASK         (4)
#define MSDC_CFG_MSDC_MUX_SEL_MSB_OFFSET       (21)

typedef struct {
    __IO uint32_t CLK_CONDA;        /* CLK_CONDA @A201_0100 */
    __IO uint32_t CLK_CONDB;        /* CLK_CONDB @A201_0104 */
    __IO uint32_t CLK_CONDC;        /* CLK_CONDC @A201_0108 */
    __IO uint32_t CLK_CONDD;        /* CLK_CONDD @A201_010C */
    __IO uint32_t reserver1;        /* alignment for 4 bytes */
    __IO uint32_t CLK_CONDF;        /* CLK_CONDF @A201_0114 */
    __IO uint32_t CLK_CONDG;        /* CLK_CONDG @A201_0118 */
    __IO uint32_t CLK_CONDH;        /* CLK_CONDH @A201_011C */
    __IO uint32_t reserver2[24];    /* alignment for 96 bytes, 0x180 - 0x120*/
    __IO uint32_t CLK_SOURCE_SEL;   /* CLK_SOURCE_SEL @A201_0180 */
    __IO uint32_t reserver3[31];    /* alignment for 124 bytes, 0x200 - 0x184*/
    __IO uint32_t SLEEP_COND;       /* SLEEP_COND @A201_0200 */
    __IO uint32_t reserver4[13];    /* alignment for 52 bytes, 0x238 - 0x204 */
    __IO uint32_t TOP_DEBUG;        /* TOP_DEBUG @A2010238 */
    __IO uint32_t reserver5[49];    /* alignment for 196 bytes, 0x300 - 0x23C */
    __I  uint32_t PDN_COND0;        /* PDN_COND0 @A201_0300 */
    __I  uint32_t PDN_COND1;        /* PDN_COND1 @A201_0304 */
    __I  uint32_t PDN_COND2;        /* PDN_COND2 @A201_0308 */
    __IO uint32_t reserver6;        /* alignment for 4 bytes */
    __IO uint32_t PDN_SETD0;        /* PDN_SETD0 @A201_0310 */
    __IO uint32_t PDN_SETD1;        /* PDN_SETD1 @A201_0314 */
    __IO uint32_t PDN_SETD2;        /* PDN_SETD2 @A201_0318 */
    __IO uint32_t reserver7;        /* alignment for 4 bytes */
    __IO uint32_t PDN_CLRD0;        /* PDN_CLRD0 @A201_0320 */
    __IO uint32_t PDN_CLRD1;        /* PDN_CLRD1 @A201_0324 */
    __IO uint32_t PDN_CLRD2;        /* PDN_CLRD2 @A201_0328 */
    __IO uint32_t reserver8[117];   /* alignment for 468 bytes 0x500 - 0x32C*/
    __IO uint32_t LPM_CON;          /* LPM_CON @0xA201_0500*/
    __IO uint32_t reserver9;        /* alignment for 4 bytes */
    __I  uint32_t LPM_L2H_CNT;      /* LPM_L2H_CNT @0xA201_0508*/
} CONFIGSYS_REGISTER_T;

/* CLK_CONDA used offset and mask definitions */
#define CLK_CONDA_HFOSC_POWERFUL_DIV_EN_OFFSET      (0)
#define CLK_CONDA_HFOSC_DIV1P5_CK_MASK              (0x01<<CLK_CONDA_HFOSC_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_HFOSC_DIV2_CK_MASK                (0x02<<CLK_CONDA_HFOSC_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_HFOSC_DIV4_CK_MASK                (0x02<<CLK_CONDA_HFOSC_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_HFOSC_DIV2P5_CK_MASK              (0x04<<CLK_CONDA_HFOSC_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_HFOSC_DIV3_CK_MASK                (0x08<<CLK_CONDA_HFOSC_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_HFOSC_DIV6_CK_MASK                (0x08<<CLK_CONDA_HFOSC_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_HFOSC_DIV3P5_CK_MASK              (0x10<<CLK_CONDA_HFOSC_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_HFOSC_DIV5_CK_MASK                (0x20<<CLK_CONDA_HFOSC_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_UPLL_POWERFUL_DIV_EN_OFFSET       (8)
#define CLK_CONDA_UPLL_F312M_CK_MASK                (0x00<<CLK_CONDA_UPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_UPLL_F104M_CK_MASK                (0x02<<CLK_CONDA_UPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_UPLL_F124M_CK_MASK                (0x04<<CLK_CONDA_UPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_UPLL_F62M_CK_MASK                 (0x08<<CLK_CONDA_UPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_UPLL_48M_CK_MASK                  (0x00<<CLK_CONDA_UPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET       (16)
#define CLK_CONDA_MPLL_F312M_CK_MASK                (0x001<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_F250M_CK_MASK                (0x002<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_F208M_CK_MASK                (0x004<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_F125M_CK_MASK                (0x008<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_DIV1P5_CK_MASK               (0x010<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_DIV2_CK_MASK                 (0x020<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_DIV4_CK_MASK                 (0x020<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_DIV2P5_CK_MASK               (0x040<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_DIV3_CK_MASK                 (0x080<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_DIV6_CK_MASK                 (0x080<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_DIV3P5_CK_MASK               (0x100<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_DIV5_CK_MASK                 (0x200<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_F138M_CK_MASK                (0x400<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)
#define CLK_CONDA_MPLL_F178M_CK_MASK                (0x800<<CLK_CONDA_MPLL_POWERFUL_DIV_EN_OFFSET)

/* CLK_CONDB used offset and mask definitions */
#define CLK_CONDB_BUS_MUX_SEL_OFFSET                (0)
#define CLK_CONDB_BUS_MUX_SEL_MASK                  (0x7<<CLK_CONDB_BUS_MUX_SEL_OFFSET)
#define CLK_CONDB_BUS_MUX_SEL_NR_INPUTS             (7)
#define CLK_CONDB_CM_MUX_SEL_OFFSET                 (3)
#define CLK_CONDB_CM_MUX_SEL_MASK                   (0xF<<CLK_CONDB_CM_MUX_SEL_OFFSET)
#define CLK_CONDB_CM_MUX_SEL_NR_INPUTS              (9)
#define CLK_CONDB_SFC_MUX_SEL_OFFSET                (10)
#define CLK_CONDB_SFC_MUX_SEL_MASK                  (0xF<<CLK_CONDB_SFC_MUX_SEL_OFFSET)
#define CLK_CONDB_SFC_MUX_SEL_NR_INPUTS             (9)
#define CLK_CONDB_SLCD_MUX_SEL_OFFSET               (14)
#define CLK_CONDB_SLCD_MUX_SEL_MASK                 (0x7<<CLK_CONDB_SLCD_MUX_SEL_OFFSET)
#define CLK_CONDB_SLCD_MUX_SEL_NR_INPUTS            (8)
#define CLK_CONDB_BSI_MUX_SEL_OFFSET                (17)
#define CLK_CONDB_BSI_MUX_SEL_MASK                  (0x7<<CLK_CONDB_BSI_MUX_SEL_OFFSET)
#define CLK_CONDB_BSI_MUX_SEL_NR_INPUTS             (6)
#define CLK_CONDB_LP_F26M_GFMUX_SEL_OFFSET          (20)
#define CLK_CONDB_LP_F26M_GFMUX_SEL_MASK            (0x1<<CLK_CONDB_LP_F26M_GFMUX_SEL_OFFSET)
#define CLK_CONDB_LP_F26M_GFMUX_SEL_NR_INPUTS       (2)
#define CLK_CONDB_GP_F26M_GFMUX_SEL_OFFSET          (21)
#define CLK_CONDB_GP_F26M_GFMUX_SEL_MASK            (0x1<<CLK_CONDB_GP_F26M_GFMUX_SEL_OFFSET)
#define CLK_CONDB_GP_F26M_GFMUX_SEL_NR_INPUTS       (2)
#define CLK_CONDB_CAM_MUX_SEL_OFFSET                (22)
#define CLK_CONDB_CAM_MUX_SEL_MASK                  (0x3<<CLK_CONDB_CAM_MUX_SEL_OFFSET)
#define CLK_CONDB_CAM_MUX_SEL_NR_INPUTS             (4)
#define CLK_CONDB_USB_MUX_SEL_OFFSET                (24)
#define CLK_CONDB_USB_MUX_SEL_MASK                  (0x3<<CLK_CONDB_USB_MUX_SEL_OFFSET)
#define CLK_CONDB_USB_MUX_SEL_NR_INPUTS             (4)
#define CLK_CONDB_DISP_PWM_MUX_SEL_OFFSET           (26)
#define CLK_CONDB_DISP_PWM_MUX_SEL_MASK             (0x3<<CLK_CONDB_DISP_PWM_MUX_SEL_OFFSET)
#define CLK_CONDB_DISP_PWM_MUX_SEL_NR_INPUTS        (4)
#define CLK_CONDB_DSP_MUX_SEL_OFFSET                (28)
#define CLK_CONDB_DSP_MUX_SEL_MASK                  (0x7<<CLK_CONDB_DSP_MUX_SEL_OFFSET)
#define CLK_CONDB_DSP_MUX_SEL_NR_INPUTS             (8)

/* CLK_CONDC used offset and mask definitions */
#define CLK_CONDC_MPLLCLK_REQ_OFFSET                (24)
#define CLK_CONDC_MPLLCLK_REQ_MASK                  (0x1<<CLK_CONDC_MPLLCLK_REQ_OFFSET)
#define CLK_CONDC_UPLLCLK_REQ_OFFSET                (25)
#define CLK_CONDC_UPLLCLK_REQ_MASK                  (0x1<<CLK_CONDC_UPLLCLK_REQ_OFFSET)
#define CLK_CONDC_SYSCLK_REQ_OFFSET                 (26)
#define CLK_CONDC_SYSCLK_REQ_MASK                   (0x1<<CLK_CONDC_SYSCLK_REQ_OFFSET)

/* CLK_CONDD used offset and mask definitions */
#define CLK_CONDD_BUSCSW_FORCE_ON_OFFSET            (0)
#define CLK_CONDD_BUSCSW_FORCE_ON_MASK              (0x1<<CLK_CONDD_BUSCSW_FORCE_ON_OFFSET)
#define CLK_CONDD_CMCSW_FORCE_ON_OFFSET             (1)
#define CLK_CONDD_CMCSW_FORCE_ON_MASK               (0x1<<CLK_CONDD_CMCSW_FORCE_ON_OFFSET)
#define CLK_CONDD_SFCCSW_FORCE_ON_OFFSET            (3)
#define CLK_CONDD_SFCCSW_FORCE_ON_MASK              (0x1<<CLK_CONDD_SFCCSW_FORCE_ON_OFFSET)
#define CLK_CONDD_SLCDCSW_FORCE_ON_OFFSET           (4)
#define CLK_CONDD_SLCDCSW_FORCE_ON_MASK             (0x1<<CLK_CONDD_SLCDCSW_FORCE_ON_OFFSET)
#define CLK_CONDD_BSICSW_FORCE_ON_OFFSET            (5)
#define CLK_CONDD_BSICSW_FORCE_ON_MASK              (0x1<<CLK_CONDD_BSICSW_FORCE_ON_OFFSET)
#define CLK_CONDD_DSPCSW_FORCE_ON_OFFSET            (6)
#define CLK_CONDD_DSPCSW_FORCE_ON_MASK              (0x1<<CLK_CONDD_DSPCSW_FORCE_ON_OFFSET)
#define CLK_CONDD_CAMCSW_FORCE_ON_OFFSET            (7)
#define CLK_CONDD_CAMCSW_FORCE_ON_MASK              (0x1<<CLK_CONDD_CAMCSW_FORCE_ON_OFFSET)
#define CLK_CONDD_USBCSW_FORCE_ON_OFFSET            (8)
#define CLK_CONDD_USBCSW_FORCE_ON_MASK              (0x1<<CLK_CONDD_USBCSW_FORCE_ON_OFFSET)
#define CLK_CONDD_DISPPWMCSW_FORCE_ON_OFFSET        (9)
#define CLK_CONDD_DISPPWMCSW_FORCE_ON_MASK          (0x1<<CLK_CONDD_DISPPWMCSW_FORCE_ON_OFFSET)
#define CLK_CONDD_MSDC0CSW_FORCE_ON_OFFSET          (10)
#define CLK_CONDD_MSDC0CSW_FORCE_ON_MASK            (0x1<<CLK_CONDD_MSDC0CSW_FORCE_ON_OFFSET)
#define CLK_CONDD_MSDC1CSW_FORCE_ON_OFFSET          (11)
#define CLK_CONDD_MSDC1CSW_FORCE_ON_MASK            (0x1<<CLK_CONDD_MSDC1CSW_FORCE_ON_OFFSET)
#define CLK_CONDD_RG_SLCD_CK_SEL_OFFSET             (21)
#define CLK_CONDD_RG_SLCD_CK_SEL_MASK               (0x1<<CLK_CONDD_RG_SLCD_CK_SEL_OFFSET)
#define CLK_CONDD_RG_SLCD_CK_SEL_NR_INPUTS          (2)
#define CLK_CONDD_LP_LFOSC_MUX_SEL_OFFSET           (22)
#define CLK_CONDD_LP_LFOSC_MUX_SEL_MASK             (0x3<<CLK_CONDD_LP_LFOSC_MUX_SEL_OFFSET)
#define CLK_CONDD_LP_LFOSC_MUX_SEL_NR_INPUTS        (4)
#define CLK_CONDD_LP_CLKSQ_MUX_SEL_OFFSET           (24)
#define CLK_CONDD_LP_CLKSQ_MUX_SEL_MASK             (0x3<<CLK_CONDD_LP_CLKSQ_MUX_SEL_OFFSET)
#define CLK_CONDD_LP_CLKSQ_MUX_SEL_NR_INPUTS        (4)

/* CLK_SOURCE_SEL used offset and mask definitions */
#define CLK_SOURCE_SEL_DSPCLK_SOURCE_SEL_OFFSET     (0)
#define CLK_SOURCE_SEL_DSPCLK_SOURCE_SEL_MASK       (0xF<<CLK_SOURCE_SEL_DSPCLK_SOURCE_SEL_OFFSET)
#define CLK_SOURCE_SEL_BSICLK_SOURCE_SEL_OFFSET     (4)
#define CLK_SOURCE_SEL_BSICLK_SOURCE_SEL_MASK       (0xF<<CLK_SOURCE_SEL_BSICLK_SOURCE_SEL_OFFSET)
#define CLK_SOURCE_SEL_SFCCLK_SOURCE_SEL_OFFSET     (8)
#define CLK_SOURCE_SEL_SFCCLK_SOURCE_SEL_MASK       (0xF<<CLK_SOURCE_SEL_SFCCLK_SOURCE_SEL_OFFSET)
#define CLK_SOURCE_SEL_PDN_BUSCLK_SOURCE_SEL_OFFSET (12)
#define CLK_SOURCE_SEL_PDN_BUSCLK_SOURCE_SEL_MASK   (0xF<<CLK_SOURCE_SEL_PDN_BUSCLK_SOURCE_SEL_OFFSET)
#define CLK_SOURCE_SEL_AO_BUSCLK_SOURCE_SEL_OFFSET  (16)
#define CLK_SOURCE_SEL_AO_BUSCLK_SOURCE_SEL_MASK    (0xF<<CLK_SOURCE_SEL_AO_BUSCLK_SOURCE_SEL_OFFSET)
#define CLK_SOURCE_SEL_CM4CLK_SOURCE_SEL_OFFSET     (20)
#define CLK_SOURCE_SEL_CM4CLK_SOURCE_SEL_MASK       (0xF<<CLK_SOURCE_SEL_CM4CLK_SOURCE_SEL_OFFSET)

typedef struct {
    __IO uint32_t GPIO_DIR0;            /* GPIO_DIR0 @A202_0000 */
    __IO uint32_t GPIO_DIR0_SET;        /* GPIO_DIR0_SET @A202_0004 */
    __IO uint32_t GPIO_DIR0_CLR;        /* GPIO_DIR0_CLR @A202_0008 */
    __IO uint32_t reserver1[777];       /* alignment for 3108 bytes, 0xC30 - 0x00C */
    __IO uint32_t GPIO_MODE3;           /* GPIO_MODE3 @A202_0C30 */
    __IO uint32_t GPIO_MODE3_SET;       /* GPIO_MODE3_SET @A202_0C34 */
    __IO uint32_t GPIO_MODE3_CLR;       /* GPIO_MODE3_CLR @A202_0C38 */
} FREQ_METER_GPIO_USED_T;

typedef struct {
    __IO uint32_t CLKSQ_CON0;           /* CLKSQ_CON0 @A204_0020 */
    __IO uint32_t reserver1[9];         /* alignment for 36 bytes, 0x48 - 0x24 */
    __IO uint32_t PLL_CON2;             /* PLL_CON2 @A204_0048 */
    __IO uint32_t PLL_CON3;             /* PLL_CON3 @A204_004C */
    __IO uint32_t PLL_CON4;             /* PLL_CON4 @A204_0050 */
    __IO uint32_t reserver2;            /* alignment for 4 bytes, 0x58 - 0x54 */
    __IO uint32_t PLL_CON6;             /* PLL_CON6 @A204_0058 */
    __IO uint32_t PLL_CON7;             /* PLL_CON7 @A204_005C */
    __IO uint32_t reserver3[12];        /* alignment for 48 bytes, 0x90 - 0x60 */
    __IO uint32_t DPM_CON0;             /* DPM_CON0 @A204_0090 */
    __IO uint32_t DPM_CON1;             /* DPM_CON1 @A204_0094 */
    __IO uint32_t DPM_CON2;             /* DPM_CON2 @A204_0098 */
    __IO uint32_t reserver4[25];        /* alignment for 100 bytes, 0x100 - 0x09C */
    __IO uint32_t MPLL_CON0;            /* MPLL_CON0 @A204_0100 */
    __IO uint32_t MPLL_CON1;            /* MPLL_CON1 @A204_0104 */
    __IO uint32_t MPLL_CON2;            /* MPLL_CON2 @A204_0108 */
    __IO uint32_t reserver5[13];        /* alignment for 52 bytes, 0x140 - 0x10C */
    __IO uint32_t UPLL_CON0;            /* UPLL_CON0 @A204_0140 */
    __IO uint32_t UPLL_CON1;            /* UPLL_CON1 @A204_0144 */
    __IO uint32_t UPLL_CON2;            /* UPLL_CON2 @A204_0148 */
    __IO uint32_t reserver6[133];       /* alignment for 532 bytes, 0x360 - 0x14C */
    __IO uint32_t RSV_CON0;             /* RSV_CON0 @A204_0360 */
    __IO uint32_t reserver7[39];        /* alignment for 156 bytes, 0x400 - 0x364 */
    __IO uint32_t ABIST_FQMTR_CON0;     /* ABIST_FQMTR_CON0 @A204_0400 */
    __IO uint32_t ABIST_FQMTR_CON1;     /* ABIST_FQMTR_CON1 @A204_0404, bit 15 is RO */
    __IO uint32_t ABIST_FQMTR_CON2;     /* ABIST_FQMTR_CON2 @A204_0408 */
    __IO uint32_t ABIST_FQMTR_DATA;     /* ABIST_FQMTR_DATA @A204_040C */
    __I  uint32_t ABIST_FQMTR_DATA_MSB; /* ABIST_FQMTR_DATA_MSB @A204_0410 */
    __IO uint32_t reserver8[3];         /* alignment for 12 bytes, 0x20 - 0x14 */
    __IO uint32_t SYS_ABIST_MON_CON0;   /* SYS_ABIST_MON_CON0 @A204_0420 */
    __IO uint32_t SYS_ABIST_MON_CON1;   /* SYS_ABIST_MON_CON1 @A204_0424 */
    __IO uint32_t SYS_ABIST_MON_CON2;   /* SYS_ABIST_MON_CON2 @A204_0428 */
    __IO uint32_t reserver9[5];         /* alignment for 20 bytes, 0x40 - 0x2C */
    __I  uint32_t ABIST_MON_DATA0;      /* ABIST_MON_DATA0 @A204_0440 */
    __IO uint32_t reserver10[47];       /* alignment for 188 bytes, 0x500 - 0x444 */
    __IO uint32_t FH_CON0;              /* FH_CON0 @A204_0500 */
    __I  uint32_t FH_CON1;              /* FH_CON1 @A204_0504 */
    __IO uint32_t reserver11[2];        /* alignment for 8 bytes, 0x510 - 0x508 */
    __IO uint32_t FH_CON4;              /* FH_CON4 @A204_0510 */
    __IO uint32_t FH_CON5;              /* FH_CON5 @A204_0514 */
    __IO uint32_t reserver12[123];      /* alignment for 492 bytes, 0x704 - 0x518 */
    __IO uint32_t PLLTD_CON1;           /* PLLTD_CON1 @A204_0704 */
    __IO uint32_t PLLTD_CON2;           /* PLLTD_CON2 @A204_0708 */
    __IO uint32_t PLLTD_CON3;           /* PLLTD_CON3 @A204_070C */
    __IO uint32_t PLLTD_CON4;           /* PLLTD_CON4 @A204_0710 */
    __IO uint32_t PLLTD_CON5;           /* PLLTD_CON5 @A204_0714 */
    __IO uint32_t PLLTD_CON6;           /* PLLTD_CON6 @A204_0718 */
    __IO uint32_t PLLTD_CON7;           /* PLLTD_CON7 @A204_071C */
    __IO uint32_t reserver13[120];      /* alignment for 480 bytes, 0x900 - 0x720 */
    __IO uint32_t LFOSC_CON0;           /* LFOSC_CON0 @A204_0900 */
    __IO uint32_t LFOSC_CON1;           /* LFOSC_CON1 @A204_0904 */
    __IO uint32_t LFOSC_CON2;           /* LFOSC_CON2 @A204_0908 */
    __IO uint32_t reserver14[5];        /* alignment for 20 bytes, 0x920 - 0x90C */
    __IO uint32_t HFOSC_CON0;           /* HFOSC_CON0 @A204_0920 */
    __IO uint32_t HFOSC_CON1;           /* HFOSC_CON1 @A204_0924 */
    __IO uint32_t HFOSC_CON2;           /* HFOSC_CON2 @A204_0928 */
    __IO uint32_t reserver15[9];        /* alignment for 36 bytes, 0x950 - 0x92C */
    __IO uint32_t SSC_CON0;             /* SSC_CON0 @A204_0950 */
    __I uint32_t SSC_CON1;              /* SSC_CON1 @A204_0954 */
    __IO uint32_t SSC_CON2;             /* SSC_CON2 @A204_0958 */
    __IO uint32_t SSC_CON3;             /* SSC_CON3 @A204_095C */
    __IO uint32_t SSC_CON4;             /* SSC_CON4 @A204_0960 */
} MIXEDSYSD_REGISTER_T;

#define LFOSC_CON0_RG_LFOSC_CALI_OFFSET (8)
#define LFOSC_CON0_RG_LFOSC_CALI_MASK (0x3F << LFOSC_CON0_RG_LFOSC_CALI_OFFSET)
#define LFOSC_CON0_RG_LFOSC_FT_OFFSET (4)
#define LFOSC_CON0_RG_LFOSC_FT_MASK (0xF << LFOSC_CON0_RG_LFOSC_FT_OFFSET)

typedef struct {
    __IO uint32_t EFUSE_CPU_104M;       /* EFUSE_CPU_104M @A206_0040 */
} EFUSE_REGISTER_T;

typedef struct {
    __I  uint32_t CM_PDN_COND0;     /* CM_PDN_COND0 @0xA20A_0000 */
    __IO uint32_t reserver1[3];     /* alignment for 12 bytes, 0x10 - 0x04 */
    __IO uint32_t CM_PDN_SETD0;     /* CM_PDN_SETD0 @0xA20A_0010 */
    __IO uint32_t reserver2[3];     /* alignment for 12 bytes, 0x20 - 0x14 */
    __IO uint32_t CM_PDN_CLRD0;     /* CM_PDN_CLRD0 @0xA20A_0020 */
    __IO uint32_t reserver3[75];    /* alignment for 300 bytes, 0x150 - 0x24 */
    __IO uint32_t CM4_DCM_CON;      /* CM4_DCM_CON @0xA20A_0150 */
    __IO uint32_t MEMS_DCM_CON;     /* MEMS_DCM_CON @0xA20A_0154 */
    __IO uint32_t CM_FREE_DCM_CON;  /* CM_FREE_DCM_CON @0xA20A_0158 */
} CM_MEMS_DCM_REGISTER_T;

typedef struct {
    __I  uint32_t ACFG_CLK_CG;          /* ACFG_CLK_CG @0xA21D_0008 */
    __IO uint32_t ACFG_CLK_CG_SET;      /* ACFG_CLK_CG_SET @0xA21D_000C */
    __IO uint32_t ACFG_CLK_CG_CLR;      /* ACFG_CLK_CG_CLR @0xA21D_0010 */
    __IO uint32_t reserver1[5];        /* alignment for 20 bytes, 0x28 - 0x014 */
    __IO uint32_t ACFG_TEST_CON0;        /* ACFG_TEST_CON0 @0xA21D_0028 */
    __IO uint32_t ACFG_TEST_CON1;        /* ACFG_TEST_CON1 @0xA21D_002C */
    __IO uint32_t reserver2[72];        /* alignment for 288 bytes, 0x150 - 0x030 */
    __IO uint32_t ACFG_CLK_UPDATE;      /* ACFG_CLK_UPDATE @0xA21D_0150 */
    __IO uint32_t ACFG_FREQ_SWCH;       /* ACFG_FREQ_SWCH @0xA21D_0154 */
    __IO uint32_t ACFG_FREQ_SWCH_OFF;   /* ACFG_FREQ_SWCH_OFF @0xA21D_0158 */
} ANA_CFGSYS_REGISTER_T;

/* ACFG_CLK_UPDATE used offset and mask definitions */
#define ACFG_CLK_UPDATE_CHG_BUS_OFFSET          (0)
#define ACFG_CLK_UPDATE_CHG_BUS_MASK            (0x1<<ACFG_CLK_UPDATE_CHG_BUS_OFFSET)
#define ACFG_CLK_UPDATE_CHG_CM_OFFSET           (1)
#define ACFG_CLK_UPDATE_CHG_CM_MASK             (0x1<<ACFG_CLK_UPDATE_CHG_CM_OFFSET)
#define ACFG_CLK_UPDATE_CHG_SFC_OFFSET          (3)
#define ACFG_CLK_UPDATE_CHG_SFC_MASK            (0x1<<ACFG_CLK_UPDATE_CHG_SFC_OFFSET)
#define ACFG_CLK_UPDATE_CHG_SLCD_OFFSET         (4)
#define ACFG_CLK_UPDATE_CHG_SLCD_MASK           (0x1<<ACFG_CLK_UPDATE_CHG_SLCD_OFFSET)
#define ACFG_CLK_UPDATE_CHG_BSI_OFFSET          (5)
#define ACFG_CLK_UPDATE_CHG_BSI_MASK            (0x1<<ACFG_CLK_UPDATE_CHG_BSI_OFFSET)
#define ACFG_CLK_UPDATE_CHG_DSP_OFFSET          (6)
#define ACFG_CLK_UPDATE_CHG_DSP_MASK            (0x1<<ACFG_CLK_UPDATE_CHG_DSP_OFFSET)
#define ACFG_CLK_UPDATE_CHG_CAM_OFFSET          (7)
#define ACFG_CLK_UPDATE_CHG_CAM_MASK            (0x1<<ACFG_CLK_UPDATE_CHG_CAM_OFFSET)
#define ACFG_CLK_UPDATE_CHG_USB_OFFSET          (8)
#define ACFG_CLK_UPDATE_CHG_USB_MASK            (0x1<<ACFG_CLK_UPDATE_CHG_USB_OFFSET)
#define ACFG_CLK_UPDATE_CHG_DISP_PWM_OFFSET     (9)
#define ACFG_CLK_UPDATE_CHG_DISP_PWM_MASK       (0x1<<ACFG_CLK_UPDATE_CHG_DISP_PWM_OFFSET)
#define ACFG_CLK_UPDATE_CHG_MSDC0_OFFSET        (10)
#define ACFG_CLK_UPDATE_CHG_MSDC0_MASK          (0x1<<ACFG_CLK_UPDATE_CHG_MSDC0_OFFSET)
#define ACFG_CLK_UPDATE_CHG_MSDC1_OFFSET        (11)
#define ACFG_CLK_UPDATE_CHG_MSDC1_MASK          (0x1<<ACFG_CLK_UPDATE_CHG_MSDC1_OFFSET)
#define ACFG_CLK_UPDATE_CHG_LP_CLKSQ_OFFSET     (12)
#define ACFG_CLK_UPDATE_CHG_LP_CLKSQ_MASK       (0x1<<ACFG_CLK_UPDATE_CHG_LP_CLKSQ_OFFSET)
#define ACFG_CLK_UPDATE_CHG_LP_LFOSC_OFFSET     (13)
#define ACFG_CLK_UPDATE_CHG_LP_LFOSC_MASK       (0x1<<ACFG_CLK_UPDATE_CHG_LP_LFOSC_OFFSET)



#define _CONFIGSYS_BASE_            (CONFIG_BASE)
#define _FREQ_METER_GPIO_USED_BASE_ (GPIO_BASE)
#define _MIXEDSYSD_BASE_            (MIXED_BASE)
#define _EFUSE_BASE_                (EFUSE_BASE)
#define _CM_MEMS_DCM_BASE_          (CM_MEMS_DCM_BASE)
#define _ANA_CFGSYS_BASE_           (ANA_CFGSYS_BASE)
#define _CLK_USED_MSDC0_BASE_       (MSDC0_BASE)
#define _CLK_USED_MSDC1_BASE_       (MSDC1_BASE)

#define _CONFIGSYS_BASE_ADDR_       (_CONFIGSYS_BASE_ + 0x0100)
#define _FREQ_METER_GPIO_USED_BASE_ADDR_ (GPIO_BASE + 0x0000)
#define _MIXEDSYSD_BASE_ADDR_       (_MIXEDSYSD_BASE_ + 0x0020)
#define _EFUSE_BASE_ADDR_           (_EFUSE_BASE_ + 0x0040)
#define _CM_MEMS_DCM_BASE_ADDR_     (_CM_MEMS_DCM_BASE_ + 0x0000)
#define _ANA_CFGSYS_BASE_ADDR_      (_ANA_CFGSYS_BASE_ + 0x0008)
#define _CLK_USED_MSDC0_BASE_ADDR_  (_CLK_USED_MSDC0_BASE_ + 0x0000)
#define _CLK_USED_MSDC1_BASE_ADDR_  (_CLK_USED_MSDC1_BASE_ + 0x0000)

#define CONFIGSYS                   ((CONFIGSYS_REGISTER_T *)(_CONFIGSYS_BASE_ADDR_))
#define FREQ_METER_GPIO_USED        ((FREQ_METER_GPIO_USED_T *)(_FREQ_METER_GPIO_USED_BASE_ADDR_))
#define MIXEDSYSD                   ((MIXEDSYSD_REGISTER_T *)(_MIXEDSYSD_BASE_ADDR_))
#define CLK_USED_EFUSE              ((EFUSE_REGISTER_T *)(_EFUSE_BASE_ADDR_))
#define CM_MEMS_DCM                 ((CM_MEMS_DCM_REGISTER_T *)(_CM_MEMS_DCM_BASE_ADDR_))
#define ANA_CFGSYS                  ((ANA_CFGSYS_REGISTER_T *)(_ANA_CFGSYS_BASE_ADDR_))
#define CLK_USED_MSDC0              ((CLK_USED_MSDC0_REGISTER_T *)(_CLK_USED_MSDC0_BASE_ADDR_))
#define CLK_USED_MSDC1              ((CLK_USED_MSDC1_REGISTER_T *)(_CLK_USED_MSDC1_BASE_ADDR_))
/*************************** Clock register definition end line  *******************************
 */

/*************************** EINT definition start line  ******************************
 */
typedef union {
    struct {
        uint32_t EINT_CNT: 10;       /* bit 0-10: CNT - debounce duration in terms of the number of 32768Hz clock cycles,
                                                                  cycle length is determinded by PRESCALER*/
        uint32_t EINT_POL: 1;        /* bit 11  : POL - polarity, activation type of the EINT source*/
        uint32_t EINT_PRESCALER: 4;  /* bit 12 -14 : PRESCALER */
        uint32_t EINT_EN: 1;         /* bit 15: EN - enable debounce */
        uint32_t reserved: 15;
        uint32_t RSTDBC: 1;          /* bit 32: RSTDBC - reset the de-bounce counter */
    } EINT_CON_B;
    uint32_t EINT_CON_W;
} EINT_CON_T;

typedef struct {
    EINT_CON_T CON_REGISTER;
} EINT_CON_REGISTER_T;


typedef struct {
    __I uint32_t EINT_STA;               /* EINT interrupt status register   */
    uint32_t RESERVED0;
    __O uint32_t EITN_INTACK;            /* EINT interrupt acknowledge register    */
    uint32_t RESERVED1;
    __I uint32_t EINT_EEVT;              /* EINT wake-up event, only for debugging */
    uint32_t RESERVED2[3];
    __I uint32_t EINT_MASK;              /* EINT interrupt mask register */
    uint32_t RESERVED3;
    __O uint32_t EINT_MASK_SET;          /* EINT interrupt mask set register, W1S: write 1 set */
    uint32_t RESERVED4;
    __O uint32_t EINT_MASK_CLR;          /* EINT interrupt mask clear register, W1C: write 1 clear  */
    uint32_t RESERVED5[3];
    __I uint32_t EINT_WAKEUP_MASK;       /*EINT wakeup event mask register */
    uint32_t RESERVED6;

    __O uint32_t EINT_WAKEUP_MASK_SET;   /*EINT wakeup event mask set register */
    uint32_t RESERVED7;
    __O uint32_t EINT_WAKEUP_MASK_CLR;   /*EINT wakeup event mask clear register */
    uint32_t RESERVED8[3];
    __I uint32_t EINT_SENS;              /* EINT interrupt sensitive register  */
    uint32_t RESERVED9;
    __O uint32_t EINT_SENS_SET;          /* EINT interrupt sensitivity set register, W1S */
    uint32_t RESERVED10;
    __O uint32_t EINT_SENS_CLR;          /* EINT interrupt sensitivity clear register, W1C */
    uint32_t RESERVED11[3];
    __I uint32_t EINT_DUALEDGE_SENS;     /* EINT interrupt dual edge sensitive register  */
    uint32_t RESERVED12;
    __O uint32_t EINT_DUALEDGE_SENS_SET; /* EINT interrupt dual edge sensitivity set register, W1S */
    uint32_t RESERVED13;
    __O uint32_t EINT_DUALEDGE_SENS_CLR; /* EINT interrupt dual edge sensitivity clear register, W1C */
    uint32_t RESERVED14[3];
    __I uint32_t EINT_SOFT;              /* EINT software interrupt register */
    uint32_t RESERVED15;
    __O uint32_t EINT_SOFT_SET;          /* EINT software interrupt set register W1S */
    uint32_t RESERVED16;
    __O uint32_t EINT_SOFT_CLR;          /* EINT software interrupt clear register W1C */
    uint32_t RESERVED17[3];
    __IO uint32_t EINT_DOMEN;
    uint32_t RESERVED18[15];
    __IO EINT_CON_REGISTER_T EINT_CON[IRQ_NUMBER_MAX];
} EINT_REGISTER_T;

#define EINT_CON_DBC_CNT_OFFSET    (0)
#define EINT_CON_DBC_CNT_MASK      (0x7FFUL << EINT_CON_DBC_CNT_OFFSET)

#define EINT_CON_POL_OFFSET        (11)
#define EINT_CON_POL_MASK          (0x1UL << EINT_CON_POL_OFFSET)

#define EINT_CON_PRESCALER_OFFSET  (12)
#define EINT_CON_PRESCALER_MASK    (0x7UL << EINT_CON_PRESCALER_OFFSET)

#define EINT_CON_DBC_EN_OFFSET     (15)
#define EINT_CON_DBC_EN_MASK       (0x1UL << EINT_CON_DBC_EN_OFFSET)

#define EINT_CON_RSTD_OFFSET       (31)
#define EINT_CON_RSTD_MASK         (0x1UL << EINT_CON_RSTD_OFFSET)


/*************************** EINT definition end line  *******************************
 */

/************************ RTC register definition start line  *******************************
 */
typedef struct {
    __IO uint16_t RTC_BBPU;		/* Baseband power up,							Address offset: 0x00 */
    __IO uint16_t RESERVED1;
    __IO uint16_t RTC_IRQ_STA;	/* RTC IRQ status,								Address offset: 0x04 */
    __IO uint16_t RESERVED2;
    __IO uint16_t RTC_IRQ_EN;		/* RTC IRQ enable,								Address offset: 0x08 */
    __IO uint16_t RESERVED3;
    __IO uint16_t RTC_CII_EN;		/* Counter increment IRQ enable,					Address offset: 0x0C */
    __IO uint16_t RESERVED4;
    __IO uint16_t RTC_AL_MASK;	/* RTC alarm mask,								Address offset: 0x10 */
    __IO uint16_t RESERVED5;
    __IO uint16_t RTC_TC_SEC;		/* RTC seconds time counter register,				Address offset: 0x14 */
    __IO uint16_t RESERVED6;
    __IO uint16_t RTC_TC_MIN;		/* RTC minutes time counter register,				Address offset: 0x18 */
    __IO uint16_t RESERVED7;
    __IO uint16_t RTC_TC_HOU;		/* RTC hours time counter register,				Address offset: 0x1C */
    __IO uint16_t RESERVED8;
    __IO uint16_t RTC_TC_DOM;		/* RTC day-of-month time counter register,			Address offset: 0x20 */
    __IO uint16_t RESERVED9;
    __IO uint16_t RTC_TC_DOW;		/* RTC day-of-week time counter register,			Address offset: 0x24 */
    __IO uint16_t RESERVED10;
    __IO uint16_t RTC_TC_MTH;		/* RTC month time counter register,				Address offset: 0x28 */
    __IO uint16_t RESERVED11;
    __IO uint16_t RTC_TC_YEA;		/* RTC year time counter register,					Address offset: 0x2C */
    __IO uint16_t RESERVED12;
    __IO uint16_t RTC_AL_SEC;		/* RTC second alarm setting register,				Address offset: 0x30 */
    __IO uint16_t RESERVED13;
    __IO uint16_t RTC_AL_MIN;		/* RTC minutes alarm setting register,				Address offset: 0x34 */
    __IO uint16_t RESERVED14;
    __IO uint16_t RTC_AL_HOU;		/* RTC hour alarm setting register,				Address offset: 0x38 */
    __IO uint16_t RESERVED15;
    __IO uint16_t RTC_AL_DOM;		/* RTC day-of-month alarm setting register,			Address offset: 0x3C */
    __IO uint16_t RESERVED16;
    __IO uint16_t RTC_AL_DOW;		/* RTC day-of-week alarm setting register,			Address offset: 0x40 */
    __IO uint16_t RESERVED17;
    __IO uint16_t RTC_AL_MTH;		/* RTC month alarm setting register,				Address offset: 0x44 */
    __IO uint16_t RESERVED18;
    __IO uint16_t RTC_AL_YEA;		/* RTC year alarm setting register,				Address offset: 0x48 */
    __IO uint16_t RESERVED19;
    __IO uint16_t RTC_OSC32CON;	/* OSC32 control,								Address offset: 0x4C */
    __IO uint16_t RESERVED20;
    __IO uint16_t RTC_POWERKEY1;	/* RTC_POWERKEY1 register,						Address offset: 0x50 */
    __IO uint16_t RESERVED21;
    __IO uint16_t RTC_POWERKEY2;	/* RTC_POWERKEY2 register,						Address offset: 0x54 */
    __IO uint16_t RESERVED22;
    __IO uint16_t RTC_PDN1;		/* PDN1,										Address offset: 0x58 */
    __IO uint16_t RESERVED23;
    __IO uint16_t RTC_PDN2;		/* PDN2,										Address offset: 0x5C */
    __IO uint16_t RESERVED24;
    __IO uint16_t RTC_SPAR0;		/* Spare register for specific purpose,				Address offset: 0x60 */
    __IO uint16_t RESERVED25;
    __IO uint16_t RTC_SPAR1;		/* Spare register for specific purpose,				Address offset: 0x64 */
    __IO uint16_t RESERVED26;
    __IO uint16_t RTC_PROT;		/* Lock/unlock scheme to prevent RTC miswriting,		Address offset: 0x68 */
    __IO uint16_t RESERVED27;
    __IO uint16_t RTC_DIFF;		/* One-time calibration offset,					Address offset: 0x6C */
    __IO uint16_t RESERVED28;
    __IO uint16_t RTC_CALI;		/* Repeat calibration offset,						Address offset: 0x70 */
    __IO uint16_t RESERVED29;
    __IO uint16_t RTC_WRTGR;		/* Enable the transfers from core to RTC in the queue,	Address offset: 0x74 */
    __IO uint16_t RESERVED30;
    __IO uint16_t RTC_CON;		/* Other RTC control register,					Address offset: 0x78 */
    __IO uint16_t RESERVED31;
} RTC_REGISTER_T;

/************************ RTC register definition end line  *******************************
 */

#define EMI_MR_DATA_MASK 0xFF
#define EMI_MREG_RDATA_OFFSET (8)
#define EMI_MREG_RDATA_MASK (0xFF << EMI_MREG_RDATA_OFFSET)
#define EMI_MREG_BANK_OFFSET (20)
#define EMI_MREG_BANK_MASK (0x7 << EMI_MREG_BANK_MASK)
#define EMI_MRGE_EDGE_TRIGGER_OFFSET (16)
#define EMI_MRGE_EDGE_TRIGGER_MASK (0x1 << EMI_MRGE_EDGE_TRIGGER_OFFSET)
#define EMI_MRGE_ACC_IDLE_OFFSET (0)
#define EMI_MRGE_ACC_IDLE_MASK (0x1 << EMI_MRGE_ACC_IDLE_OFFSET)
#define EMI_MRGE_WDATA_OFFSET (24)
#define EMI_MRGE_WDATA_MASK (0xFF << EMI_MRGE_WDATA_OFFSET)
#define EMI_MRGE_W_OFFSET (17)
#define EMI_MRGE_W_MASK (0x1 << EMI_MRGE_W_OFFSET)
#define EMI_HFSLP_EXIT_REQ_OFFSET (0)
#define EMI_HFSLP_EXIT_REQ_MASK (0x1 << EMI_HFSLP_EXIT_REQ_OFFSET)
#define EMI_HFSLP_ENT_STA_OFFSET (4)
#define EMI_HFSLP_ENT_STA_MASK (0x1 << EMI_HFSLP_ENT_STA_OFFSET)
#define EMI_HFSLP_EXIT_ACK_OFFSET (1)
#define EMI_HFSLP_EXIT_ACK_MASK (0x1 << EMI_HFSLP_EXIT_ACK_OFFSET)
#define EMI_BIST_STR_ADDR_OFFSET (16)
#define EMI_BIST_STR_ADDR_MASK (0xFFFF << EMI_BIST_STR_ADDR_OFFSET)
#define EMI_BIST_BG_DATA_OFFSET (16)
#define EMI_BIST_BG_DATA_MASK (0xFFFF << EMI_BIST_BG_DATA_OFFSET)
#define EMI_BIST_DATA_INV_OFFSET (12)
#define EMI_BIST_DATA_INV_MASK (0x1 << EMI_BIST_DATA_INV_OFFSET)
#define EMI_BIST_DATA_RANDOM_OFFSET (13)
#define EMI_BIST_DATA_RANDOM_MASK (0x1 << EMI_BIST_DATA_RANDOM_OFFSET)
#define EMI_BIST_END_OFFSET (1)
#define EMI_BIST_END_MASK (0x1 << EMI_BIST_END_OFFSET)
#define EMI_BIST_FAIL_OFFSET (0)
#define EMI_BIST_FAIL_MASK (0x1 << EMI_BIST_FAIL_OFFSET)
#define EMI_DQ7_IN_DEL_OFFSET (24)
#define EMI_DQ6_IN_DEL_OFFSET (16)
#define EMI_DQ5_IN_DEL_OFFSET (8)
#define EMI_DQ4_IN_DEL_OFFSET (0)
#define EMI_DQ3_IN_DEL_OFFSET (24)
#define EMI_DQ2_IN_DEL_OFFSET (16)
#define EMI_DQ1_IN_DEL_OFFSET (8)
#define EMI_DQ0_IN_DEL_OFFSET (0)
#define EMI_DQS0_IN_DEL_OFFSET (16)
#define EMI_CAL_DONE_OFFSET (7)
#define EMI_CAL_DONE_MASK (0x1 << EMI_CAL_DONE_OFFSET)
#define EMI_IDLE_OFFSET (0)
#define EMI_IDLE_MASK (0x1 << EMI_IDLE_OFFSET)
#define EMI_SRAM_IDLE_OFFSET (4)
#define EMI_SRAM_IDLE_MASK (0x1 << EMI_SRAM_IDLE_OFFSET)
#define EMI_REQ_MASK_OFFSET (8)
#define EMI_REQ_MASK_MASK (0x7 << EMI_REQ_MASK_OFFSET)


/*EMI register definition start*/
typedef struct {
    __IO uint8_t  RESERVED1[96];
    __IO uint32_t EMI_CONM;     //0x60
    __IO uint8_t  RESERVED2[12];
    __IO uint32_t EMI_GENA;    //0x70
    __IO uint8_t  RESERVED3[20];
    __IO uint32_t EMI_RDCT;    //0x88
    __IO uint8_t  RESERVED4[4];
    __IO uint32_t EMI_DLLV;   //0x90
    __IO uint8_t  RESERVED5[60];
    __IO uint32_t EMI_IDLC;
    __IO uint8_t  RESERVED6[4];
    __IO uint32_t EMI_IDLD;
    __IO uint8_t  RESERVED7[4];
    __IO uint32_t EMI_IDLE;
    __IO uint8_t  RESERVED8[20];
    __IO uint32_t EMI_ODLC;
    __IO uint8_t  RESERVED9[4];
    __IO uint32_t EMI_ODLD;
    __IO uint8_t  RESERVED10[4];
    __IO uint32_t EMI_ODLE;
    __IO uint8_t  RESERVED11[4];
    __IO uint32_t EMI_ODLF;
    __IO uint8_t  RESERVED12[28];
    __IO uint32_t EMI_IOA;
    __IO uint8_t  RESERVED13[4];
    __IO uint32_t EMI_IOB;
    __IO uint8_t  RESERVED14[12];
    __IO uint32_t EMI_HFSLP;
    __IO uint8_t  RESERVED15[4];
    __IO uint32_t EMI_DSRAM;
    __IO uint8_t  RESERVED16[4];
    __IO uint32_t EMI_MSRAM;
    __IO uint8_t  RESERVED17[4];
    __IO uint32_t EMI_MREG_RW;
    __IO uint8_t  RESERVED18[12];
    __IO uint32_t EMI_ARBA;
    __IO uint8_t  RESERVED19[4];
    __IO uint32_t EMI_ARBB;
    __IO uint8_t  RESERVED20[4];
    __IO uint32_t EMI_ARBC;
    __IO uint8_t  RESERVED21[20];
    __IO uint32_t EMI_SLCT;
    __IO uint8_t  RESERVED22[4];
    __IO uint32_t EMI_ABCT;
    __IO uint8_t  RESERVED23[92];
    __IO uint32_t EMI_BMEN;
    __IO uint8_t  RESERVED24[4];
    __IO uint32_t EMI_BCNT;
    __IO uint8_t  RESERVED25[4];
    __IO uint32_t EMI_TACT;
    __IO uint8_t  RESERVED26[4];
    __IO uint32_t EMI_TSCT;
    __IO uint8_t  RESERVED27[4];
    __IO uint32_t EMI_WACT;
    __IO uint8_t  RESERVED28[4];
    __IO uint32_t EMI_WSCT;
    __IO uint8_t  RESERVED29[4];
    __IO uint32_t EMI_BACT;
    __IO uint8_t  RESERVED30[4];
    __IO uint32_t EMI_BSCT0;
    __IO uint8_t  RESERVED31[4];
    __IO uint32_t EMI_BSCT1;
    __IO uint8_t  RESERVED32[60];
    __IO uint32_t EMI_TTYPE1;
    __IO uint8_t  RESERVED33[124];
    __IO uint32_t EMI_MBISTA;
    __IO uint8_t  RESERVED34[4];
    __IO uint32_t EMI_MBISTB;
    __IO uint8_t  RESERVED35[4];
    __IO uint32_t EMI_MBISTC;
    __IO uint8_t  RESERVED36[4];
    __IO uint32_t EMI_MBISTD;
    __IO uint8_t  RESERVED37[20];
    __IO uint32_t EMI_TEST;
    __IO uint8_t  RESERVED38[20];
    __IO uint32_t EMI_VENDER_INFO;
} EMI_REGISTER_T;
/*EMI register definition end*/

/************************ MSDC register definition start line  *******************************
 */

typedef struct {
    __IO uint32_t MSDC_CFG; 			   /*MSDC_CFG, SD memory card controller configuration register*/
    __IO uint32_t MSDC_STA; 			   /*MSDC_STA, SD memory card controller status register*/
    __I  uint32_t MSDC_INT; 			   /*MSDC_INT, SD memory card controller interrupt register*/
    __IO uint32_t MSDC_PS;				   /*MSDC_PS, SD memory card pin status register*/
    __IO uint32_t MSDC_DAT; 			   /*MSDC_DAT, SD memory card controller data register*/
    __IO uint32_t MSDC_IOCON;			   /*MSDC_IOCON, SD memory card controller IO control register*/
    __IO uint32_t MSDC_IOCON1;			   /*MSDC_IOCON1, SD memory card controller IO control register 1*/
    __IO uint32_t RESERVE1;
    __IO uint32_t SDC_CFG;				   /*SDC_CFG, SD memory card controller configuration register*/
    __IO uint32_t SDC_CMD;				   /*SDC_CMD, SD memory card controller command register*/
    __IO uint32_t SDC_ARG;				   /*SDC_ARG, SD memory card controller argument register*/
    __I  uint32_t SDC_STA;				   /*SDC_STA, SD memory card controller status register*/
    __I  uint32_t SDC_RESP0;			   /*SDC_RESP0, SD memory card controller response register 0*/
    __I  uint32_t SDC_RESP1;			   /*SDC_RESP1, SD memory card controller response register 1*/
    __I  uint32_t SDC_RESP2;			   /*SDC_RESP2, SD memory card controller response register 2*/
    __I  uint32_t SDC_RESP3;			   /*SDC_RESP3, SD memory card controller response register 3*/
    __I  uint32_t SDC_CMDSTA;			   /*SDC_CMDSTA, SD memory card controller command status register*/
    __I  uint32_t SDC_DATSTA;			   /*SDC_DATSTA, SD memory card controller data status register*/
    __I  uint32_t SDC_CSTA; 			   /*SDC_CSTA, SD memory card  status register*/
    __IO uint32_t SDC_IRQMASK0; 		   /*SDC_IRQMASK0, SD memory card IRQ mask register 0*/
    __IO uint32_t SDC_IRQMASK1; 		   /*SDC_IRQMASK1, SD memory card IRQ mask register 1*/
    __IO uint32_t SDIO_CFG; 			   /*SDIO_CFG, SDIO configuration register */
    __I  uint32_t SDIO_STA; 			   /*SDIO_STA, SDIO status register */
    __IO uint32_t RESERVE2[9];
    __IO uint32_t CLK_RED;				   /*CLK_RED, clock latch configuration register*/
} msdc_register_t;


/*********************MSDC_CFG*****************************/
#define MSDC_CFG_FIFOTHD_OFFSET                 (24)
#define MSDC_CFG_CLKSRC_PAT_OFFSET              (23)
#define MSDC_CFG_DIRQEN_OFFSET                  (19)
#define MSDC_CFG_PINEN_OFFSET                   (18)
#define MSDC_CFG_DMAEN_OFFSET                   (17)
#define MSDC_CFG_INTEN_OFFSET                   (16)
#define MSDC_CFG_SCLKF_OFFSET                   (8)
#define MSDC_CFG_SCLKON_OFFSET                  (7)
#define MSDC_CFG_CRED_OFFSET                    (6)
#define MSDC_CFG_CLKSRC_OFFSET                  (3)
#define MSDC_CFG_RST_OFFSET                     (1)
#define MSDC_CFG_MSDC_OFFSET                    (0)

#define MSDC_CFG_FIFOTHD_MASK                  (0x0F << MSDC_CFG_FIFOTHD_OFFSET)
#define MSDC_CFG_CLKSRC_PAT_MASK               (1 << MSDC_CFG_CLKSRC_PAT_OFFSET)
#define MSDC_CFG_DIRQEN_MASK                   (1 << MSDC_CFG_DIRQEN_OFFSET)
#define MSDC_CFG_PINEN_MASK                    (1 << MSDC_CFG_PINEN_OFFSET)
#define MSDC_CFG_DMAEN_MASK                    (1 << MSDC_CFG_DMAEN_OFFSET)
#define MSDC_CFG_INTEN_MASK                    (1 << MSDC_CFG_INTEN_OFFSET)
#define MSDC_CFG_SCLKON_MASK                   (1 << MSDC_CFG_SCLKON_OFFSET)
#define MSDC_CFG_CRED_MASK                     (1 << MSDC_CFG_CRED_OFFSET)
#define MSDC_CFG_RST_MASK                      (1 << MSDC_CFG_RST_OFFSET)
#define MSDC_CFG_MSDC_MASK                     (1 << MSDC_CFG_MSDC_OFFSET)

#define MSDC_CFG_SCLKF_MASK                    (0xFF << MSDC_CFG_SCLKF_OFFSET)
#define MSDC_CFG_CLKSRC_MASK                   (0x03 << MSDC_CFG_CLKSRC_OFFSET)

/*********************MSDC_STA*****************************/

#define MSDC_STA_BUSY_OFFSET                       (15)
#define MSDC_STA_FIFOCLR_OFFSET                    (14)
#define MSDC_STA_FIFICNT_OFFSET                    (4)
#define MSDC_STA_INT_OFFSET                        (3)
#define MSDC_STA_DRQ_OFFSET                        (2)
#define MSDC_STA_BE_OFFSET                         (1)
#define MSDC_STA_BF_OFFSET                         (0)

#define MSDC_STA_BUSY_MASK                         (1 << MSDC_STA_BUSY_OFFSET)
#define MSDC_STA_FIFOCLR_MASK                      (1 << MSDC_STA_FIFOCLR_OFFSET)
#define MSDC_STA_FIFICNT_MASK                      (0x0F << MSDC_STA_FIFICNT_OFFSET)
#define MSDC_STA_INT_MASK                          (1 << MSDC_STA_INT_OFFSET)
#define MSDC_STA_DRQ_MASK                          (1 << MSDC_STA_DRQ_OFFSET)
#define MSDC_STA_BE_MASK                           (1 << MSDC_STA_BE_OFFSET)
#define MSDC_STA_BF_MASK                           (1 << MSDC_STA_BF_OFFSET)


/*********************MSDC_INT*****************************/
#define MSDC_INT_SDIOIRQ_OFFSET                    (7)
#define MSDC_INT_SDR1BIRQ_OFFSET                   (6)
#define MSDC_INT_SDMCIRQ_OFFSET                    (4)
#define MSDC_INT_SDDATIRQ_OFFSET                   (3)
#define MSDC_INT_SDCMDIRQ_OFFSET                   (2)
#define MSDC_INT_PINIRQ_OFFSET                     (1)
#define MSDC_INT_DIRQ_OFFSET                       (0)

#define MSDC_INT_SDIOIRQ_MASK                      (1 << MSDC_INT_SDIOIRQ_OFFSET)
#define MSDC_INT_SDR1BIRQ_MASK                     (1 << MSDC_INT_SDR1BIRQ_OFFSET)
#define MSDC_INT_SDMCIRQ_MASK                      (1 << MSDC_INT_SDMCIRQ_OFFSET)
#define MSDC_INT_SDDATIRQ_MASK                     (1 << MSDC_INT_SDDATIRQ_OFFSET)
#define MSDC_INT_SDCMDIRQ_MASK                     (1 << MSDC_INT_SDCMDIRQ_OFFSET)
#define MSDC_INT_PINIRQ_MASK                       (1 << MSDC_INT_PINIRQ_OFFSET)
#define MSDC_INT_DIRQ_MASK                         (1 << MSDC_INT_DIRQ_OFFSET)


/*********************MSDC_IOCON*****************************/
#define MSDC_IOCON_SAMPON_OFFSET                             (21)
#define MSDC_IOCON_CMDSEL_OFFSET                             (19)
#define MSDC_IOCON_DMABURST_OFFSET                           (8)
#define MSDC_IOCON_SRCFG1_OFFSET                             (7)
#define MSDC_IOCON_SRCFG0_OFFSET                             (6)
#define MSDC_IOCON_ODCCFG1_OFFSET                            (3)
#define MSDC_IOCON_ODCCFG0_OFFSET                            (0)

#define MSDC_IOCON_SAMPON_MASK           (1 << MSDC_IOCON_SAMPON_OFFSET)
#define MSDC_IOCON_CMDSEL_MASK           (1 << MSDC_IOCON_CMDSEL_OFFSET)
#define MSDC_IOCON_DMABURST_MASK         (0x03 << MSDC_IOCON_DMABURST_OFFSET)
#define MSDC_IOCON_ODCCFG0_MASK          (0xFF << MSDC_IOCON_ODCCFG0_OFFSET)

/*********************SDC_CFG*****************************/
#define SDC_CFG_DTOC_OFFSET                                  (24)
#define SDC_CFG_WDOD_OFFSET                                  (20)
#define SDC_CFG_SDIO_OFFSET                                  (19)
#define SDC_CFG_MDLEN_OFFSET                                 (17)
#define SDC_CFG_SIEN_OFFSET                                  (16)
#define SDC_CFG_BSYDLY_OFFSET                                (12)
#define SDC_CFG_BLKEN_OFFSET                                 (0)

#define SDC_CFG_SDIO_MASK                                    (1 << SDC_CFG_SDIO_OFFSET)
#define SDC_CFG_MDLEN_MASK                                   (1 << SDC_CFG_MDLEN_OFFSET)
#define SDC_CFG_SIEN_MASK                                    (1 << SDC_CFG_SIEN_OFFSET)

#define SDC_CFG_DTOC_MASK                                    ((uint32_t)0xFF << SDC_CFG_DTOC_OFFSET)
#define SDC_CFG_WDOD_MASK                                    (0x0F << SDC_CFG_WDOD_OFFSET)
#define SDC_CFG_BSYDLY_MASK                                  (0x0F << SDC_CFG_BSYDLY_OFFSET)
#define SDC_CFG_BLKEN_MASK                                   (0xFFF << SDC_CFG_BLKEN_OFFSET)

/*********************SDC_CMD*****************************/
#define SDC_CMD_STOP_OFFSET                                  (14)


#define SDC_CMD_STOP_MASK                                    (1 << SDC_CMD_STOP_OFFSET)

/*********************SDC_STA*****************************/
#define SDC_STA_WP_OFFSET                                    (15)
#define SDC_STA_FEDATBUSY_OFFSET                             (4)
#define SDC_STA_FECMDBUSY_OFFSET                             (3)
#define SDC_STA_BEDATBUSY_OFFSET                             (2)
#define SDC_STA_BECMDBUSY_OFFSET                              (1)
#define SDC_STA_BESDCBUSY_OFFSET                             (0)

#define SDC_STA_WP_MASK                                      (1 << SDC_STA_WP_OFFSET)
#define SDC_STA_FEDATBUSY_MASK                               (1 << SDC_STA_FEDATBUSY_OFFSET)
#define SDC_STA_FECMDBUSY_MASK                               (1 << SDC_STA_FECMDBUSY_OFFSET)
#define SDC_STA_BEDATBUSY_MASK                               (1 << SDC_STA_BEDATBUSY_OFFSET)
#define SDC_STA_BECMDBUSY_MASK                                (1 << SDC_STA_BECMDBUSY_OFFSET)
#define SDC_STA_BESDCBUSY_MASK                               (1 << SDC_STA_BESDCBUSY_OFFSET)


/*********************SDC_CMDSTA*****************************/
#define SDC_CMDSTA_RSPCRCERR_OFFSET                          (2)
#define SDC_CMDSTA_CMDTO_OFFSET                              (1)
#define SDC_CMDSTA_CMDRDY_OFFSET                             (0)

#define SDC_CMDSTA_RSPCRCERR_MASK                            (1 << SDC_CMDSTA_RSPCRCERR_OFFSET)
#define SDC_CMDSTA_CMDTO_MASK                                (1 << SDC_CMDSTA_CMDTO_OFFSET)
#define SDC_CMDSTA_CMDRDY_MASK                               (1 << SDC_CMDSTA_CMDRDY_OFFSET)


/*********************SDC_DATSTA*****************************/
#define SDC_DATSTA_DATCRCERR_OFFSET                       (2)
#define SDC_DATSTA_DATTO_OFFSET                           (1)
#define SDC_DATSTA_BLKDONE_OFFSET                         (0)

#define SDC_DATSTA_DATCRCERR_MASK                         (0xFF << SDC_DATSTA_DATCRCERR_OFFSET)
#define SDC_DATSTA_DATTO_MASK                             (1 << SDC_DATSTA_DATTO_OFFSET)
#define SDC_DATSTA_BLKDONE_MASK                           (1 << SDC_DATSTA_BLKDONE_OFFSET)

/*********************CLK_RED*****************************/
#define CLK_RED_CMD_RED_OFFSET                            (29)
#define CLK_RED_DAT_RED_OFFSET                            (13)
#define CLK_RED_CLKPAD_RED_OFFSET                         (7)
#define CLK_RED_CLK_LATCH_OFFSET                          (6)

#define CLK_RED_CMD_RED_MASK                              (1 << CLK_RED_CMD_RED_OFFSET)
#define CLK_RED_DAT_RED_MASK                              (1 << CLK_RED_DAT_RED_OFFSET)
#define CLK_RED_CLKPAD_RED_MASK                           (1 << CLK_RED_CLKPAD_RED_OFFSET)
#define CLK_RED_CLK_LATCH_MASK                            (1 << CLK_RED_CLK_LATCH_OFFSET)

/*********************SDIO_CFG*****************************/
#define SDIO_CFG_INTEN_OFFSET                             (0)

#define SDIO_CFG_INTEN_MASK                               (1 << SDIO_CFG_INTEN_OFFSET)


/************************ MSDC register definition end line  *******************************
 */

/* structure type of CMSYS_CFG
 */
typedef struct {
    __IO uint32_t MISC1;
    __IO uint32_t STCALIB;
    __IO uint32_t AHB_SLAVE_WAY_EN;
    __IO uint32_t AHB_DEC_ERR_EN;
    __IO uint32_t AHB_SIDEBAND;
    __IO uint32_t AHB_BUFFERALBE;
    __IO uint32_t AHB_FIFO_TH;
    __IO uint32_t FORCE_IDLE_OFF;
    uint32_t RESERVED0[1];
    __IO uint32_t CG_FREERUN_EN;
    __IO uint32_t CG_GATE_EN;
    uint32_t RESERVED1[1];
    __IO uint32_t TCM_BUS_CTRL;
    __IO uint32_t INT_ACTIVE_HL0;
    __IO uint32_t INT_ACTIVE_HL1;
    uint32_t RESERVED2[1];
    __IO uint32_t DCM_CTRL_REG;
} CMSYS_CFG_REGISTER_T;

#define CMSYS_CFG  	((CMSYS_CFG_REGISTER_T *)CMSYS_CFG_BASE)

/* structure type of CMSYS_CFG_EXT
 */
typedef struct {
    __IO uint32_t CG_EN;
    __IO uint32_t DCM_EN;
} CMSYS_CFG_EXT_REGISTER_T;

#define CMSYS_CFG_EXT  	((CMSYS_CFG_EXT_REGISTER_T *)CMSYS_CFG_EXT_BASE)

/************************ CMSYS_CFG register definition end line  *******************************
 */


/* *************************flash hardware definition start line**********************************
*/

/*flash register structure definition*/
typedef struct {
    __IO uint32_t RW_SF_MAC_CTL;         /*!<  SFC control register Address offset: 0x00 */
    __IO uint32_t RW_SF_DIRECT_CTL;        /*!< SFC control register Address offset: 0x04 */
    __IO uint32_t RW_SF_MISC_CTL;      /*!<  SFC control register Address offset:   0x08 */
    __IO uint32_t RW_SF_MISC_CTL2;      /*!<  SFC control register Address offset:   0x0C */
    __IO uint32_t RW_SF_MAC_OUTL;      /*!<  SFC control register Address offset:   0x10 */
    __IO uint32_t RW_SF_MAC_INL;      /*!<  SFC thresh registerAddress offset:   0x14 */
    __IO uint32_t RW_SF_RESET_CTL;      /*!<  SFC thresh registerAddress offset:   0x18 */
    __IO uint32_t RW_SF_STA2_CTL;      /*!<  SFC thresh registerAddress offset:   0x1C */
    __IO uint32_t RW_SF_DLY_CTL1;      /*!<  SFC thresh registerAddress offset:   0x20*/
    __IO uint32_t RW_SF_DLY_CTL2;      /*!<  SFC thresh registerAddress offset:   0x24 */
    __IO uint32_t RW_SF_DLY_CTL3;      /*!<  SFC thresh registerAddress offset:   0x28 */
    __IO uint32_t RW_SF_DUMMY1;         /*!<  SFC thresh registerAddress offset:   0x2C */
    __IO uint32_t RW_SF_DLY_CTL4;      /*!<  SFC thresh registerAddress offset:   0x30 */
    __IO uint32_t RW_SF_DLY_CTL5;      /*!<  SFC thresh registerAddress offset:   0x34 */
    __IO uint32_t RW_SF_DLY_CTL6;      /*!<  SFC thresh registerAddress offset:   0x38 */
    __IO uint32_t RW_SF_DUMMY2;         /*!<  SFC thresh registerAddress offset:   0x3C */
    __IO uint32_t RW_SF_DIRECT_CTL2;      /*!<  SFC thresh registerAddress offset:   0x40 */
    __IO uint32_t RW_SF_MISC_CTL3;      /*!<  SFC thresh registerAddress offset:   0x44 */
} SFC_REGISTER_T;


// Performance Monitor
typedef struct {
    __IO uint32_t SF_PERF_MON1;         /*!<  SFC control register Address offset: 0x80 */
    __IO uint32_t SF_PERF_MON2;        /*!< SFC counter register Address offset: 0x84 */
    __IO uint32_t SF_PERF_MON3;      /*!<  SFC thresh registerAddress offset:   0x88 */
    __IO uint32_t SF_PERF_MON4;      /*!<  SFC thresh registerAddress offset:   0x8C */
    __IO uint32_t SF_PERF_MON5;      /*!<  SFC thresh registerAddress offset:   0x90 */
    __IO uint32_t SF_PERF_MON6;      /*!<  SFC thresh registerAddress offset:   0x94 */
    __IO uint32_t SF_PERF_MON7;      /*!<  SFC thresh registerAddress offset:   0x98 */
    __IO uint32_t SF_PERF_MON8;      /*!<  SFC thresh registerAddress offset:   0x9C */
    __IO uint32_t SF_PERF_MON9;      /*!<  SFC thresh registerAddress offset:   0xA0*/
    __IO uint32_t SF_PERF_MON10;      /*!<  SFC thresh registerAddress offset:   0xA4 */
    __IO uint32_t SF_PERF_MON11;      /*!<  SFC thresh registerAddress offset:   0xA8 */
    __IO uint32_t SF_PERF_MON12;         /*!<  SFC thresh registerAddress offset:   0xAC */
    __IO uint32_t SF_PERF_MON13;      /*!<  SFC thresh registerAddress offset:   0xB0 */
} SFC_PM_REGISTER_T;


typedef struct {
    __IO uint32_t RW_SF_GPRAM_DATA;      /*!<  SFC thresh registerAddress offset:   0x800 */
    __IO uint32_t RW_SF_GPRAM_DATA_OF_4; /*!<  SFC thresh registerAddress offset:   0x804 */
} SFC_GPRAM_REGISTER_T;

#define SFC_PM_BASE      	(SFC_BASE + 0x80)
#define SFC_GPRAMADDR     	(SFC_BASE + 0x800)
#define SFC                 ((SFC_REGISTER_T *) (SFC_BASE))
#define SFC_PM              ((SFC_PM_REGISTER_T *) (SFC_PM_BASE))
#define SFC_GPRAM           ((SFC_GPRAM_REGISTER_T *) (SFC_GPRAMADDR))

/* SFC generic offset definition */
#define SFC_GENERIC_1_BIT_OFFSET      (1)
#define SFC_GENERIC_2_BIT_OFFSET      (2)
#define SFC_GENERIC_4_BIT_OFFSET      (4)
#define SFC_GENERIC_8_BIT_OFFSET      (8)
#define SFC_GENERIC_10_BIT_OFFSET    (10)
#define SFC_GENERIC_16_BIT_OFFSET    (16)
#define SFC_GENERIC_24_BIT_OFFSET    (24)
#define SFC_GENERIC_31_BIT_OFFSET    (31)

/* SFC generic mask definition */
#define SFC_GENERIC_0x1_MASK         (0x1)
#define SFC_GENERIC_0x0F_MASK        (0x0F)
#define SFC_GENERIC_0xF0_MASK        (0xF0)
#define SFC_GENERIC_0xFF_MASK        (0xFF)
#define SFC_GENERIC_0xF000_MASK      (0xF000)
#define SFC_GENERIC_0x00FF_MASK      (0x00FF)
#define SFC_GENERIC_0x0FFFFFFF_MASK  (0x0FFFFFFF)
#define SFC_GENERIC_0x000000FF_MASK  (0x000000FF)
#define SFC_GENERIC_0x0000FF00_MASK  (0x0000FF00)
#define SFC_GENERIC_0x00FF0000_MASK  (0x00FF0000)
#define SFC_GENERIC_0xFF000000_MASK  (0xFF000000)
#define SFC_GENERIC_0xFFFFFF00_MASK  (0xFFFFFF00)
#define SFC_GENERIC_FLASH_BANK_MASK  (0x8000000)
#define SFC_GENERIC_DPD_SW_MASK      (0x000F0F00)
#define SFC_GENERIC_DPD_SW_IO_MASK   (0x0F0F0F0F)

/* *************************flash hardware definition end line**********************************
*/


#endif
