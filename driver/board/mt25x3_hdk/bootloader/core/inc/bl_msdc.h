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

#ifndef __BL_MSDC_H__
#define __BL_MSDC_H__

#include "stdint.h"
#include "stdbool.h"
#include <string.h>

/*enable sd module in bootloader*/
#define BL_SD_MODULE_ENABLED

#if defined(BL_SD_MODULE_ENABLED)
#include "mt2523.h"

#define    MSDC_FALSE      false
#define    MSDC_TRUE       true

#define MSDC0_BASE           0xA0020000 /*MS/SD Controller 0*/
#define MSDC1_BASE           0xA0030000 /*MS/SD Controller 1*/


#define MSDC0_REG  ((msdc_register_t *)MSDC0_BASE) /*defined in mt2523.h*/
#define MSDC1_REG  ((msdc_register_t *)MSDC1_BASE)

#define MSDC_CLOCK           89140 /*(kHz) 89140*/
#define MSDC_OUTPUT_CLOCK    15300
#define MSDC_INIT_CLOCK      240 /*KHz*/

#define MSDC_MAX_BLOCK_LENGTH (2048)

#define MSDC_IO_1_ALL_PULLUP_47K              (0x22222)     /*config MSDC_IOCON_1,pull up io 47K*/

#define MSDC_DATA_TIMEOUT_COUNT               (80)


#define MSDC_LATCH_THRESHOLD (25000)  /*for register CLK_REG, if output clock biger than 25000, CLKPAD_RED BIT should be set to 1, else should be set to 0*/

#if 0
/************************ MSDC register definition start line  *******************************
 */

#ifdef __cplusplus
typedef  volatile   __I;                  /*!< Defines 'read only' permissions                 */
#else
typedef  volatile const __I;            /*!< Defines 'read only' permissions                 */
#endif
typedef    volatile  __O;                  /*!< Defines 'write only' permissions                */
typedef    volatile  __IO;                 /*!< Defines 'read / write' permissions              */


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

#define SDC_CFG_DTOC_MASK                                    (0xFF << SDC_CFG_DTOC_OFFSET)
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
#endif

/*MSDC port, sd and sdio header file should defined as below*/
typedef enum {
    MSDC_PORT_0 = 0,
    MSDC_PORT_1 = 1,
    MSDC_PORT_MAX
} msdc_port_t;

typedef enum {
    MSDC_FAIL = -1,
    MSDC_OK = 0,
    MSDC_INITIALIZED = 1,
} msdc_status_t;

typedef enum {
    MSDC_OWNER_NONE = 0,
    MSDC_OWNER_SD   = 1,
    MSDC_OWNER_SDIO
} msdc_owner_t;

typedef struct {
    volatile msdc_owner_t    owner;
    uint32_t                 msdc_clock;
    uint32_t                 output_clock;
    uint32_t                 interrupt_register;
    volatile bool            is_card_present;
    volatile bool            is_initialized;
    volatile bool            is_timeout;
    volatile bool            is_card_plug_out;
    uint32_t                 command_status;
    uint32_t                 data_status;
    volatile uint32_t        dma_status;
    volatile uint32_t        dma_count;
    volatile uint32_t        dma_address;
    volatile bool            is_dma_write;
    void (*msdc_dma_callback)(void *user_data);
    //void (*msdc_card_detect_callback)(int8_t event, void *user_data);
    //void                     *card_detect_user_data;
} msdc_config_t;


typedef struct {
    uint32_t output_clock;
    uint8_t sclkf;
    uint8_t clksrc_pat;
    uint8_t clksrc;
} msdc_clock_t;

typedef enum {
    MSDC_FIFO_THRESHOLD_0     = 0,
    MSDC_FIFO_THRESHOLD_1     = 1,
    MSDC_FIFO_THRESHOLD_2     = 2,
    MSDC_FIFO_THRESHOLD_3     = 3,
    MSDC_FIFO_THRESHOLD_4     = 4,
    MSDC_FIFO_THRESHOLD_5     = 5,
    MSDC_FIFO_THRESHOLD_6     = 6,
    MSDC_FIFO_THRESHOLD_7     = 7,
    MSDC_FIFO_THRESHOLD_8     = 8,
} msdc_fifo_threshold_t;

typedef enum {
    MSDC_SOURCE_CLOCK_94_5M = 0,
    MSDC_SOURCE_CLOCK_74_3M = 1,
    MSDC_SOURCE_CLOCK_65M   = 2,
    MSDC_SOURCE_CLOCK_60_6M = 3
} msdc_source_clock_t;



typedef enum {
    MSDC_IO_4MA  = 0,
    MSDC_IO_8MA  = 1,
    MSDC_IO_12MA = 2,
    MSDC_IO_16MA = 3,
} msdc_driving_capability_t;

typedef enum {
    MSDC_IO_SLEW_FAST    = 0,
    MSDC_IO_SLEW_SLOW    = 1
} msdc_slew_rate_t;

typedef enum {
    MSDC_DMA_SINGLE_MODE = 0,
    MSDC_DMA_4_BEAT_INCREMENT_BUSRT = 1,
    MSDC_DMA_8_BEAT_INCREMENT_BUSRT = 2,
    MSDC_DMA_BURST_MODE_MAX
} msdc_burst_mode_t;


/** @brief   SD/MMC IO driving capability structure */
typedef struct {
    msdc_driving_capability_t control_line_output_driving_capability;                /**< SD/MMC control line output driving capability */
    msdc_driving_capability_t data_line_output_driving_capability;                   /**< SD/MMC data line output driving capability */
    msdc_slew_rate_t          control_line_slew_rate;                                /**< SD/MMC control line output slew rate */
    msdc_slew_rate_t          data_line_slew_rate;                                   /**< SD/MMC data line output slew rate */
} msdc_io_parameter_t;


void msdc_nvic_set(msdc_port_t msdc_port, bool is_enable);
void msdc_wait(uint32_t wait_ms);
void msdc_reset(msdc_port_t msdc_port);
void msdc_record_command_status(msdc_port_t msdc_port, uint32_t command_status);
void msdc_record_data_status(msdc_port_t msdc_port, uint32_t data_status);
void msdc_set_output_clock(msdc_port_t msdc_port, uint32_t msdc_clock);
void msdc_clock_init(msdc_port_t msdc_port);
uint32_t msdc_get_output_clock(msdc_port_t msdc_port);
void msdc_power_set(msdc_port_t msdc_port, bool is_power_on);
void msdc_set_io_parameter(msdc_port_t msdc_port, msdc_io_parameter_t *io_config);
void msdc_interrupt_init(msdc_port_t msdc_port);
bool msdc_card_is_present(msdc_port_t msdc_port);
void msdc_dma_wait_transfer_complete(msdc_port_t msdc_port);
void msdc_dma_init(msdc_port_t msdc_port);
void msdc_dma_config(msdc_port_t msdc_port, uint32_t address, uint32_t count, bool is_write);
void msdc_dma_enable(msdc_port_t msdc_port);
void msdc_dma_disable(msdc_port_t msdc_port);
msdc_owner_t msdc_get_owner(msdc_port_t msdc_port);
void msdc_set_owner(msdc_port_t msdc_port, msdc_owner_t owner);
void msdc_card_power_set(msdc_port_t msdc_port, bool is_power_on);
msdc_status_t msdc_init(msdc_port_t msdc_port);
void msdc_deinit(msdc_port_t msdc_port);
uint32_t msdc_read_fifo(msdc_port_t msdc_port);
void msdc_write_fifo(msdc_port_t msdc_port, uint32_t data);


#endif /*BL_SD_MODULE_ENABLED || HAL_SDIO_MODULE_ENABLED*/
#endif

