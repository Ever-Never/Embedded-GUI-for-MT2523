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

/*
    Module Name:
    connsys_bus_driver.c

    Abstract:
    Provide SDIO-GEN3  based bus driver routines

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
*/
#include <stdio.h>
#include <string.h>
#include "type_def.h"
#include "connsys_driver.h"
#include "connsys_bus.h"
#include "nvic.h"
#include "dma_sw.h"
#include "dma_hw.h"
#include "debug.h"
#include "misc.h"
#include "syslog.h"
#include "type_def.h"
#include "cos_api.h"
#include "hal_cache.h"
#include "ctype.h"
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
#include "hal_clock.h"
#endif


#ifdef MTK_WIFI_SLIM_ENABLE
/* anthony */
#define OVERRIDE_LOG
#endif

#include "connsys_log.h"

#ifdef _CTYPE_H
/* Use driver/chip/mt5932/rom/fw/inc/ctype.h, the _ctype need defined here */
unsigned char _ctype[] = {
_C,_C,_C,_C,_C,_C,_C,_C,                        /* 0-7 */
_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,         /* 8-15 */
_C,_C,_C,_C,_C,_C,_C,_C,                        /* 16-23 */
_C,_C,_C,_C,_C,_C,_C,_C,                        /* 24-31 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,                    /* 32-39 */
_P,_P,_P,_P,_P,_P,_P,_P,                        /* 40-47 */
_D,_D,_D,_D,_D,_D,_D,_D,                        /* 48-55 */
_D,_D,_P,_P,_P,_P,_P,_P,                        /* 56-63 */
_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,      /* 64-71 */
_U,_U,_U,_U,_U,_U,_U,_U,                        /* 72-79 */
_U,_U,_U,_U,_U,_U,_U,_U,                        /* 80-87 */
_U,_U,_U,_P,_P,_P,_P,_P,                        /* 88-95 */
_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,      /* 96-103 */
_L,_L,_L,_L,_L,_L,_L,_L,                        /* 104-111 */
_L,_L,_L,_L,_L,_L,_L,_L,                        /* 112-119 */
_L,_L,_L,_P,_P,_P,_P,_C,                        /* 120-127 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /* 128-143 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                /* 144-159 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,   /* 160-175 */
_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,       /* 176-191 */
_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,       /* 192-207 */
_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,       /* 208-223 */
_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,       /* 224-239 */
_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};      /* 240-255 */
#endif


log_create_module(connsys, PRINT_LEVEL_WARNING);

//#include <errno.h>
#define EIO 5     // errno.h not available for Keil compiler
#define ETIME 62  // errno.h not available for Keil compiler

// ========================== SDIO Private Routines =============================

void connsys_bus_get_bus(struct connsys_func *func)
{
}

void connsys_bus_release_bus(struct connsys_func *func)
{
}

void connsys_bus_read_port_pio(connsys_gen3_cmd53_info *info, void *dst, int32_t count)
{
    int32_t i;
    int32_t drop_count = 0;
    uint32_t flags;

    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info->word;
    /* 2. CMD53 read out */
    for (i = 0; i < count; i += 4) {
        uint32_t value = REG32(SDIO_GEN3_CMD53_DATA);
        if (i < CONNSYS_MAX_RX_PKT_SIZE && dst != NULL) {
            *(((uint8_t *)dst) + i) = (value & 0xFF);
            *(((uint8_t *)dst) + i + 1) = ((value & 0xFF00) >> 8);
            *(((uint8_t *)dst) + i + 2) = ((value & 0xFF0000) >> 16);
            *(((uint8_t *)dst) + i + 3) = ((value & 0xFF000000) >> 24);
        } else {
            drop_count += 4;
        }
    }
    local_irq_restore(flags);
}


void connsys_bus_read_port_pio_garbage(connsys_gen3_cmd53_info *info, int32_t count)
{
    int32_t i;
    uint32_t flags;

    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info->word;
    /* 2. CMD53 read out */
    for (i = 0; i < count; i += 4) {
        REG32(SDIO_GEN3_CMD53_DATA);
    }
    local_irq_restore(flags);
}




void connsys_bus_read_port_dma(connsys_gen3_cmd53_info *info, void *dst, int32_t count)
{

    uint32_t flags;
    uint32_t reg;
#if 0
    INFO(connsys, "==>connsys_bus_read_port_dma 2, dst = 0x%x, count = %d\n",
           (unsigned int)dst, (int)count);

    INFO(connsys, "block_mode = %u\n", (unsigned int)info->field.block_mode);
    INFO(connsys, "field.count = %u\n", (unsigned int)info->field.count);
#endif

    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info->word;
    /* DMA setting */
#if (PRODUCT_VERSION == 7687 || PRODUCT_VERSION == 7697)
    // MT7687
    REG32(GDMA_CH4_START) &= ~(0x1 << 15);	   /* Clear DMA channel */
    REG32(GDMA_CH4_PGMADDR) = (uint32_t)dst;			/* Setting CM4 SRAM Start Address */
    REG32(GDMA_CH4_COUNT) = (count >> 2);	   /* Setting DMA transfer length in DW */
    REG32(GDMA_CH4_CON) = 0;
    REG32(GDMA_CH4_CON) |= (0x15 << 20);	   /* select HIF(SDIO) Tx/Rx */
    REG32(GDMA_CH4_CON) |= (0x1 << 18); 	   /* DIR=1, HIF to CM4 Sysram	=> WRITE */
//	REG32(GDMA_CH4_CON) |= (0x1 << 15); 	   /* Enable DMA finish interrupt */
    REG32(GDMA_CH4_CON) |= (0x6 << 8);		   /* Burst transfer 4 beats in DW */
    REG32(GDMA_CH4_CON) |= (0x1 << 3);		   /* Increase destination address, HIF address */
    REG32(GDMA_CH4_CON) |= (0x0 << 2);		   /* Don't increase source address, CM4 Sysram address */
    REG32(GDMA_CH4_CON) |= (0x2 << 0);		   /* Double word transfer, 4 bytes */

    /* DMA kick out */
    REG32(GDMA_CH4_START) |= (0x1 << 15); /* Start DMA channel*/
    do {
        reg = REG32(GDMA_CH4_RLCT);
    } while (reg != 0x00);
#elif (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    REG32(PDN_CLR0) = 0x1;    // turn off DMA cg
    REG32(GDMA_CH6_START) = 0x0; //&= ~(0x1 << 15);     /* Clear DMA channel */
    REG32(GDMA_CH6_PGMADDR) = (unsigned int)dst;            /* Setting CM4 SRAM Start Address */
    REG32(GDMA_CH6_COUNT) = (count >> 2);      /* Setting DMA transfer length in DW */
    //REG32(GDMA_CH6_CON) = 0x01090212;
    REG32(GDMA_CH6_CON) = 0x90212; // no interrupt
    //REG32(GDMA_CH6_CON) = 0;
    //REG32(GDMA_CH6_CON) |= (0x1 << 24);        /* ITEN, 1 = enable DMA transfer completion interrupt */
    //REG32(GDMA_CH6_CON) |= (0x2 << 18);        /* Transfer type, 10 = 4-beat Incrementing burst */
    //REG32(GDMA_CH6_CON) |= (0x1 << 16);        /* DREQ, 0 = no throttle control; 1 = Hardware handshake management */
    //REG32(GDMA_CH6_CON) |= (0x2 << 8);         /* SIZE, Word transfer - 4 bytes */
    //REG32(GDMA_CH6_CON) |= (0x1 << 4);         /* DIRECTION, [4] Directions of PDMA transfer */
    //REG32(GDMA_CH6_CON) |= (0x1 << 1);         /* DIRECTION, [1] Incremental destination address */
    //REG32(GDMA_CH6_CON) |= (0x0 << 0);         /* DIRECTION, [0] Incremental sourue address */

    /* DMA kick out */
    REG32(GDMA_CH6_START) |= (0x1 << 15); /* Start DMA channel*/
    do {
        reg = REG32(GDMA_CH6_RLCT);
    } while (reg != 0x00);
#else
#endif
    local_irq_restore(flags);


}
int32_t connsys_get_rx_buffer_size(struct connsys_func *func, int32_t size)
{

    int32_t count;

    /* CMD53 port mode to write n-byte, if count >= block size => block mode, otherwise =>  byte mode  */
    count = ALIGN_4BYTE(size);


    /* 1. Setup command information */
    if (count >= func->blksize) {
        /* block  mode */
        int32_t cnt  = count / func->blksize;
        if (count % func->blksize > 0) {
            cnt++;
        }
        count = cnt * func->blksize;
        //INFO(connsys, "rx alloc: %d bytes\n", (int)count);
    } else {
        ;/* byte  mode */
    }
    return count;
}

/**
 *	connsys_readsb - read from a FIFO on a SDIO function
 *	@func: SDIO function to access
 *	@dst: buffer to store the data
 *	@addr: address of (single byte) FIFO
 *	@count: number of bytes to read
 *
 *	Reads from the specified FIFO of a given SDIO function. Return
 *	value indicates if the transfer succeeded or not.
 */
int32_t connsys_bus_read_port(struct connsys_func *func, void *dst, uint32_t addr,
                              int32_t size)
{
    connsys_gen3_cmd53_info info;
    int32_t count;

    if ((unsigned int)dst & 0x3) {
        ERR(connsys, "==>connsys_bus_read_port, align error, dst = 0x%x", (unsigned int)dst);
        connsys_bus_read_port_garbage(addr, size);
        return CONNSYS_STATUS_FAIL;
    }

    /* CMD53 port mode to write n-byte, if count >= block size => block mode, otherwise =>  byte mode  */
    count = ALIGN_4BYTE(size);

    /* 1. Setup command information */
    info.word = 0;
    info.field.rw_flag = SDIO_GEN3_READ;
    info.field.func_num = func->num; //SDIO_GEN3_FUNCTION_WIFI;
    if (count >= func->blksize) {
        info.field.block_mode = SDIO_GEN3_BLOCK_MODE; /* block  mode */
        info.field.count = count / func->blksize;
        if (count % func->blksize > 0) {
            info.field.count++;
        }
        count = info.field.count * func->blksize;
    } else {
        info.field.block_mode = SDIO_GEN3_BYTE_MODE; /* byte  mode */
        info.field.count = count;
    }
    info.field.op_mode = SDIO_GEN3_FIXED_PORT_MODE; /* fix mode */
    info.field.addr = addr;
    if (func->use_dma) {
        /*
        	uint32_t drop_count = 0;
        	if (count >  CONNSYS_MAX_RX_PKT_SIZE)
        	{
        		count = CONNSYS_MAX_RX_PKT_SIZE;
        		drop_count = CONNSYS_MAX_RX_PKT_SIZE - count;
        	}
        */
        //if (count > func->blksize)
        //	INFO(connsys, "rx: %d -> %d bytes, CMD53=0x%x\n", (int)size, (int)count, (int)info.word);

        connsys_bus_read_port_dma(&info, dst, count);
        /*
        	if (drop_count >  0)
        	{
        		// TODO: take care of info of drop count
        		ERR(connsys, "DMA mode drop count(%d) not support yet!", (int)drop_count);
        	}
        */
    } else {
        connsys_bus_read_port_pio(&info, dst, count);
    }

    return CONNSYS_STATUS_SUCCESS;
}


int32_t connsys_bus_read_port_garbage(uint32_t addr, int32_t size)
{
    connsys_gen3_cmd53_info info;
    int32_t count;
    struct connsys_func *func = &g_connsys_func;


    /* CMD53 port mode to write n-byte, if count >= block size => block mode, otherwise =>  byte mode  */
    count = ALIGN_4BYTE(size);

    /* 1. Setup command information */
    info.word = 0;
    info.field.rw_flag = SDIO_GEN3_READ;
    info.field.func_num = func->num; //SDIO_GEN3_FUNCTION_WIFI;
    if (count >= func->blksize) {
        info.field.block_mode = SDIO_GEN3_BLOCK_MODE; /* block  mode */
        info.field.count = count / func->blksize;
        if (count % func->blksize > 0) {
            info.field.count++;
        }
        count = info.field.count * func->blksize;
    } else {
        info.field.block_mode = SDIO_GEN3_BYTE_MODE; /* byte  mode */
        info.field.count = count;
    }
    info.field.op_mode = SDIO_GEN3_FIXED_PORT_MODE; /* fix mode */
    info.field.addr = addr;

    connsys_bus_read_port_pio_garbage(&info, count);

    return 0;
}

#ifdef MT5932_SINGLE_CONTEXT
#ifndef MTK_WIFI_SLIM_ENABLE
uint8_t inband_rsp1[300];
#else
ATTR_ZIDATA_IN_TCM uint8_t inband_rsp1[300];
#endif
int inband_queue_inband_handler(void *pkt_ptr, unsigned char *payload, unsigned int len);

void connsys_bus_read_port0_pio_garbage(connsys_gen3_cmd53_info *info, int32_t count, int len)
{
    int32_t i;
    uint32_t flags;
       uint32_t *data = (uint32_t *)inband_rsp1;
    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info->word;
       /* 2. CMD53 read out */
       for (i = 0; i < count; i+=4)
    {
               *data++ = REG32(SDIO_GEN3_CMD53_DATA);
    }
    local_irq_restore(flags);

       inband_queue_inband_handler(NULL, inband_rsp1, len);

}

int32_t connsys_bus_read_port0_garbage(uint32_t addr, int32_t size)
{
    connsys_gen3_cmd53_info info;
    int32_t count;
    struct connsys_func *func = &g_connsys_func;
       //INFO(connsys, "HIT\n");

    /* CMD53 port mode to write n-byte, if count >= block size => block mode, otherwise =>  byte mode  */
    count = ALIGN_4BYTE(size);

    /* 1. Setup command information */
    info.word = 0;
    info.field.rw_flag = SDIO_GEN3_READ;
    info.field.func_num = func->num; //SDIO_GEN3_FUNCTION_WIFI;
    if (count >= func->blksize)
    {
        info.field.block_mode = SDIO_GEN3_BLOCK_MODE; /* block  mode */
        info.field.count = count/func->blksize;
        if (count % func->blksize > 0)
            info.field.count++;
        count = info.field.count * func->blksize;
    }
    else
    {
        info.field.block_mode = SDIO_GEN3_BYTE_MODE; /* byte  mode */
        info.field.count = count;
    }
    info.field.op_mode = SDIO_GEN3_FIXED_PORT_MODE; /* fix mode */
    info.field.addr = addr;

       connsys_bus_read_port0_pio_garbage(&info, count, size);

    return 0;
}
#endif
void connsys_dump_cmd53_info(connsys_gen3_cmd53_info *info)
{
    INFO(connsys, "info: 0x%x", (unsigned int)info->word);
    INFO(connsys, "\tByte/Block Count: %u", info->field.count);
    INFO(connsys, "\tRegister Address: 0x%x", info->field.addr);
    INFO(connsys, "\tOP Code: 0x%x", info->field.op_mode);
    INFO(connsys, "\tBlock Mode: 0x%x", info->field.block_mode);
    INFO(connsys, "\tFunction Number: 0x%x", info->field.func_num);
    INFO(connsys, "\tR/W flag: 0x%x", info->field.rw_flag);
}



static void connsys_bus_write_port_pio(connsys_gen3_cmd53_info *info, void *src, int32_t count)
{
    int32_t i;
    uint32_t flags;
    uint32_t value;


    if (g_connsys_debug_feature & CONNSYS_DBG_PORT) {
        CONNSYS_D(CONNSYS_DBG_PORT,
                    ("==>connsys_bus_write_port_pio, src = 0x%x, count = %u",
                     (unsigned int)src, (unsigned int)count));

        connsys_dump_cmd53_info(info);
    }

    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info->word;
    for (i = 0; i < count; i += 4) {
        if (i >= count) {
            /* Padding with all "0" */
            value = 0;
        } else {
            value = *(((uint8_t *)src) + i);
            value |= ((*(((uint8_t *)src) + i + 1)) << 8);
            value |= ((*(((uint8_t *)src) + i + 2)) << 16);
            value |= ((*(((uint8_t *)src) + i + 3)) << 24);
        }
        REG32(SDIO_GEN3_CMD53_DATA) = value;
    }
    local_irq_restore(flags);
}

// TODO: Support scattered packets
static void connsys_bus_write_port_dma(connsys_gen3_cmd53_info *info, void *src, int32_t count)
{
    uint32_t reg;
    uint32_t flags;

    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info->word;
#if (PRODUCT_VERSION == 7687 || PRODUCT_VERSION == 7697)
    // MT7687
    /* DMA setting */
    REG32(GDMA_CH4_START) = 0;	   /* Clear DMA channel */
    REG32(GDMA_CH4_PGMADDR) = (uint32_t)src;			/* Setting CM4 SRAM Start Address */
    REG32(GDMA_CH4_COUNT) = (count >> 2);	   /* Setting DMA transfer length in DW */
    REG32(GDMA_CH4_CON) = 0;
    REG32(GDMA_CH4_CON) |= (0x15 << 20);	   /* select HIF(SDIO) Tx/Rx */
    REG32(GDMA_CH4_CON) |= (0x0 << 18); 	   /* DIR=0, CM4 Sysram to HIF => READ */
//	REG32(GDMA_CH4_CON) |= (0x0 << 15); 	   /* Enable DMA finish interrupt */
    REG32(GDMA_CH4_CON) |= (0x6 << 8);		   /* Burst transfer 4 beats in DW */
    REG32(GDMA_CH4_CON) |= (0x0 << 3);		   /* Don't increase destination address, HIF address */
    REG32(GDMA_CH4_CON) |= (0x1 << 2);		   /* Increase source address, CM4 Sysram address */
    REG32(GDMA_CH4_CON) |= (0x2 << 0);		   /* Double word transfer, 4 bytes */
    /* DMA kick out */
    REG32(GDMA_CH4_START) |= (0x1 << 15); /* Start DMA channel*/
    /* wait DMA finish */
    do {
        reg = REG32(GDMA_CH4_RLCT);
    } while (reg != 0x00);
#elif (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
    /* DMA setting */
    REG32(PDN_CLR0) = 0x1;    // turn off DMA cg
    REG32(GDMA_CH6_START) = 0x0;     /* Clear DMA channel */
    REG32(GDMA_CH6_PGMADDR) = (unsigned int)src;            /* Setting CM4 SRAM Start Address */
    REG32(GDMA_CH6_COUNT) = (count >> 2);      /* Setting DMA transfer length in DW */
    //REG32(GDMA_CH6_CON) = 0x01090201;
    REG32(GDMA_CH6_CON) = 0x90201; // no interrupt
    //REG32(GDMA_CH6_CON) = 0;
    //REG32(GDMA_CH6_CON) |= (0x1 << 24);        /* ITEN, 1 = enable DMA transfer completion interrupt */
    //REG32(GDMA_CH6_CON) |= (0x2 << 18);        /* Transfer type, 10 = 4-beat Incrementing burst */
    //REG32(GDMA_CH6_CON) |= (0x1 << 16);        /* DREQ, 0 = no throttle control; 1 = Hardware handshake management */
    //REG32(GDMA_CH6_CON) |= (0x2 << 8);         /* SIZE, Word transfer - 4 bytes */
    //REG32(GDMA_CH6_CON) |= (0x0 << 1);         /* DIRECTION, [1] Incremental destination address */
    //REG32(GDMA_CH6_CON) |= (0x1 << 0);         /* DIRECTION, [0] Incremental sourue address */
    /* DMA kick out */
    REG32(GDMA_CH6_START) |= (0x1 << 15); /* Start DMA channel*/
    /* wait DMA finish */
    do {
        reg = REG32(GDMA_CH6_RLCT);
    } while (reg != 0x00);
#endif
    local_irq_restore(flags);
}

void connsys_bus_write_port_transfer(struct connsys_func *func, connsys_gen3_cmd53_info *info, void *src, int32_t size)
{

    if (func->use_dma) {
        connsys_bus_write_port_dma(info, src, size);
    } else {
        connsys_bus_write_port_pio(info, src, size);

    }
}

int32_t connsys_bus_write_port(struct connsys_func *func, uint32_t addr, void *src,
                               int32_t size)
{
    connsys_gen3_cmd53_info info;
    int32_t count;

    CONNSYS_D(CONNSYS_DBG_PORT, ("==>connsys_bus_write_port, addr = 0x%x, src = 0x%x, size = %u\n",
                                   (unsigned int)addr, (unsigned int)src, (unsigned int)size));

    if (!(IS_ALIGN_4((uint32_t)src)) && (func->use_dma)) {
        CONNSYS_DUMP_E(connsys, "connsys_bus_write_port, align error, packet: ", (uint8_t *)src, (uint32_t)size);
        return CONNSYS_STATUS_FAIL;
    }

    /* CMD53 port mode to write n-byte, if count >= block size => block mode, otherwise =>  byte mode  */
    count = ALIGN_4BYTE(size);

    /* 1. Setup command information */
    info.word = 0;
    info.field.rw_flag = SDIO_GEN3_WRITE;
    info.field.func_num = func->num; //SDIO_GEN3_FUNCTION_WIFI;

    if (count >= func->blksize) {
        info.field.block_mode = SDIO_GEN3_BLOCK_MODE; /* block  mode */
        info.field.count = count / func->blksize;
        if (count % func->blksize > 0) {
            info.field.count++;
        }
        count = info.field.count * func->blksize;
    } else {
        info.field.block_mode = SDIO_GEN3_BYTE_MODE; /* byte  mode */
        info.field.count = count;
    }

    info.field.op_mode = SDIO_GEN3_FIXED_PORT_MODE; /* fix mode */
    info.field.addr = addr;

    if ((func->use_dma) &&
            (info.field.block_mode == SDIO_GEN3_BLOCK_MODE) &&
            (count < CONNSYS_MAX_RX_PKT_SIZE))

    {
        connsys_bus_write_port_dma(&info, src, count);
    } else {
        connsys_bus_write_port_pio(&info, src, count);

    }
    return CONNSYS_STATUS_SUCCESS;
}

void connsys_dump_tx_scatter_info(connsys_tx_scatter_info_t *tx_info)
{
    uint32_t i;
    INFO(connsys, "dump tx scatter info:");
    CONNSYS_DUMP_I(connsys, "txd", tx_info->txd, 12);
    INFO(connsys, "reserved_headroom_len = %u, tx_len = %u, real_tx_len = %u, buf_num = %u, fg_wmm = %d",
          (unsigned int)tx_info->reserved_headroom_len,
          (unsigned int)tx_info->tx_len,
          (unsigned int)tx_info->real_tx_len,
          (unsigned int)tx_info->buf_num,
          (int)tx_info->fg_wmm);

    for (i = 0; i < tx_info->buf_num; i++) {
        INFO(connsys, "[%u]: ", (unsigned int)i);
        CONNSYS_DUMP_I(connsys, "content", tx_info->buff[i].buf_ptr, tx_info->buff[i].buf_len);
    }
}
#define CONNYS_WSAP00041276	1//Mutiple Chaining issue Fix

int32_t connsys_write_data(connsys_tx_scatter_info_t *tx_info)
{
    connsys_gen3_cmd53_info info;
    struct connsys_func *func = &g_connsys_func;
    uint32_t addr = WTDR1;
    int32_t count;
    uint32_t i;
    uint32_t src;
    uint32_t send_len;
    int32_t remain_send_len = 0;
    int32_t remain_reserved_headroom_len = 0;
    uint32_t  n9_pad_buf;
    uint32_t send_cnt = 0;
    uint32_t  byte_idx;
    uint32_t  buf_idx;
    uint32_t flags;
    uint32_t send_buf = 0;
#if (CONNYS_WSAP00041276 == 1)
	uint8_t flag = 0;
#endif	

    /* CMD53 port mode to write n-byte, if count >= block size => block mode, otherwise =>  byte mode  */
    count = tx_info->real_tx_len;

    /* 1. Setup command information */
    info.word = 0;
    info.field.rw_flag = SDIO_GEN3_WRITE;
    info.field.func_num = func->num; //SDIO_GEN3_FUNCTION_WIFI;

    if (count >= func->blksize) {
        info.field.block_mode = SDIO_GEN3_BLOCK_MODE; /* block  mode */
        info.field.count = count / func->blksize;
        if (count % func->blksize > 0) {
            info.field.count++;
        }
        count = info.field.count * func->blksize;
    } else {
        info.field.block_mode = SDIO_GEN3_BYTE_MODE; /* byte  mode */
        info.field.count = count;
    }

    info.field.op_mode = SDIO_GEN3_FIXED_PORT_MODE; /* fix mode */
    info.field.addr = addr;


#if (CONNSYS_DEBUG_MODE_EN == 1)
    if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_PORT)) {
        CONNSYS_D(CONNSYS_DBG_PORT, ("==>connsys_write_data\n"));
        connsys_dump_cmd53_info(&info);
    }
#endif

    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info.word;
#if (defined MTK_CM4_WIFI_TASK_ENABLE) && (defined DATA_PATH_87)
//#ifdef DATA_PATH_87
 uint8_t Cid = *((uint8_t*)&tx_info->txd + 4);
 if(Cid != 0xff)
 {
    /* Send HIF TXD */
    src = (uint32_t) &tx_info->txd;
    for (i = 0; i < 12; i+=4) {
        REG32(SDIO_GEN3_CMD53_DATA) = *(uint32_t *)(src + i);
    }
    send_cnt += 12;
  }
  else
  {
    //INFO(connsys, "Skip HIF p_txd->ucCID != 0xff\n");

  }
 #else

    /* Send HIF TXD */
    src = (uint32_t) &tx_info->txd;
    for (i = 0; i < 12; i += 4) {
        REG32(SDIO_GEN3_CMD53_DATA) = *(uint32_t *)(src + i);
    }

    send_cnt += 12;

#endif /* MT7686 && DATA_PATH_87 */

    /* Send N9 padding */
    send_len = (tx_info->reserved_headroom_len / 4) * 4;
    for (i = 0; i < send_len; i += 4) {
        REG32(SDIO_GEN3_CMD53_DATA) = 0;
    }
    send_cnt += send_len;

    remain_reserved_headroom_len = tx_info->reserved_headroom_len - send_len;

    if (remain_reserved_headroom_len && (!func->use_dma)) {
        /* PIO sent out 1st DW of payload */
        n9_pad_buf = 0;
        memcpy((void *)((uint32_t)((uint8_t *)&n9_pad_buf) + remain_reserved_headroom_len),
               tx_info->buff[0].buf_ptr,
               (4 - remain_reserved_headroom_len));

        REG32(SDIO_GEN3_CMD53_DATA) = n9_pad_buf;
        send_cnt += 4;
    }

    /* Send packet payload */
    byte_idx = 0;
    for (buf_idx = 0; buf_idx < tx_info->buf_num; buf_idx++) {
        if ((buf_idx == 0) && (remain_reserved_headroom_len)) {
            if (func->use_dma) {
                //ASSERT(remain_reserved_headroom_len == RESERVED_HLEN);
                /* DMA not sent out 1st DW of payload  */
                src = (uint32_t)tx_info->buff[0].buf_ptr - remain_reserved_headroom_len;
                remain_send_len = tx_info->buff[0].buf_len + remain_reserved_headroom_len;
            } else {
                /* PIO has sent out 1st DW of payload  */
                src = (uint32_t)tx_info->buff[0].buf_ptr + remain_reserved_headroom_len;
                remain_send_len = tx_info->buff[0].buf_len - (4 - remain_reserved_headroom_len);
            }
        } else {
            src = (uint32_t)tx_info->buff[buf_idx].buf_ptr;
            remain_send_len = tx_info->buff[buf_idx].buf_len;
#if (CONNYS_WSAP00041276 == 1)
					if(flag == 1)
					{
						while (byte_idx < 4)
						{
							*(((uint8_t *)&send_buf) + byte_idx) = *(uint8_t *)src;
							byte_idx ++;
							src ++;
							remain_send_len--;
							
						}
						send_cnt+=4;
						REG32(SDIO_GEN3_CMD53_DATA) = send_buf;
						send_buf = 0;
						flag = 0;
					}
#endif

#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
#ifdef HAL_CACHE_MODULE_ENABLED
	     if (true == hal_cache_is_cacheable(src))
	     {
	     	   //hex_dump("cacheable", (unsigned char *)src, remain_send_len);
	         goto pure_pio;
	     }
#endif
#endif
        }
        /* 2. CMD53 write out */
        if (func->use_dma) {

            unsigned int reg;
            unsigned int len_moved_by_dma;
            if (!IS_ALIGN_4((uint32_t)src)) {
                /* Sometimes, LWIP use RX buffer for TX directly, w/o pbuf_free and allocation again, in this case, PBUF_RAW type will lead to unaligned 4-byte */
                INFO(connsys, "[DMA] 4-align addr(%d), len(%d), src:0x%x, tx_info->reserved_headroom_len(%d), remain_reserved_headroom_len(%d), src offset 2 bytes...\n",
                      IS_ALIGN_4((uint32_t)src), IS_ALIGN_4(remain_send_len), src,
                      tx_info->reserved_headroom_len, remain_reserved_headroom_len);
                goto pure_pio;
            }
#if (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
            hal_clock_enable(HAL_CLOCK_CG_DMA);
#else
            REG32(GDMA_CHANNEL_ENABLE) = 0xFFFFFFFF; // enable all DMA channel
#endif
            //REG32(SDIO_GEN3_CMD_SETUP) = info.word; // DMA mode same as PIO
#if (PRODUCT_VERSION == 7687 || PRODUCT_VERSION == 7697)
            // MT7687
            /* DMA setting */
            REG32(GDMA_CH4_START) = 0;     /* Clear DMA channel */
            REG32(GDMA_CH4_PGMADDR) = (unsigned int)src;            /* Setting CM4 SRAM Start Address */
            REG32(GDMA_CH4_COUNT) = (remain_send_len >> 2);    /* Setting DMA transfer length in DW */
            REG32(GDMA_CH4_CON) = 0;
            REG32(GDMA_CH4_CON) |= (0x15 << 20);       /* select HIF(SDIO) Tx/Rx */
            REG32(GDMA_CH4_CON) |= (0x0 << 18);        /* DIR=0, CM4 Sysram to HIF => READ */
            //REG32(GDMA_CH4_CON) |= (0x0 << 15);      /* Enable DMA finish interrupt */
            REG32(GDMA_CH4_CON) |= (0x6 << 8);         /* Burst transfer 4 beats in DW */
            REG32(GDMA_CH4_CON) |= (0x0 << 3);         /* Don't increase destination address, HIF address */
            REG32(GDMA_CH4_CON) |= (0x1 << 2);         /* Increase source address, CM4 Sysram address */
            REG32(GDMA_CH4_CON) |= (0x2 << 0);         /* Double word transfer, 4 bytes */
#elif (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
		REG32(PDN_CLR0) = 0x1;    // turn off DMA cg
		REG32(GDMA_CH6_START) = 0x0;     /* Clear DMA channel */
		REG32(GDMA_CH6_PGMADDR) = (unsigned int)src;            /* Setting CM4 SRAM Start Address */
		REG32(GDMA_CH6_COUNT) = (remain_send_len >> 2);      /* Setting DMA transfer length in DW */
		//REG32(GDMA_CH6_CON) = 0x01090201;
		REG32(GDMA_CH6_CON) = 0x90201; // no interrupt
		//REG32(GDMA_CH6_CON) = 0;
		//REG32(GDMA_CH6_CON) |= (0x0 << 24);        /* ITEN, 1 = enable DMA transfer completion interrupt */
		//REG32(GDMA_CH6_CON) |= (0x2 << 18);        /* Transfer type, 10 = 4-beat Incrementing burst */
		//REG32(GDMA_CH6_CON) |= (0x1 << 16);        /* DREQ, 0 = no throttle control; 1 = Hardware handshake management */
		//REG32(GDMA_CH6_CON) |= (0x2 << 8);         /* SIZE, Word transfer - 4 bytes */
		//REG32(GDMA_CH6_CON) |= (0x0 << 1);         /* DIRECTION, [1] Incremental destination address */
		//REG32(GDMA_CH6_CON) |= (0x1 << 0);         /* DIRECTION, [0] Incremental sourue address */

#endif
            byte_idx = 0;   //byte_idx indicate remaing bytes for residual (div by 4)
            len_moved_by_dma = (remain_send_len >> 2) << 2;
            src +=  len_moved_by_dma;
            send_cnt += len_moved_by_dma;
            remain_send_len -= len_moved_by_dma;
            //INFO(connsys, "byte_idx=%d\n", byte_idx);

#if 0 //(CONSYS_BUG_DBG==1)
            INFO(connsys, "GDMA_CON(0x%x)=0x%x\n", GDMA_CH4_CON, REG32(GDMA_CH4_CON));

            // wait DMA finish
            INFO(connsys, "Wait GDMA finishing...\n");
#endif

            /* DMA kick out */
#if (PRODUCT_VERSION == 7687 || PRODUCT_VERSION == 7697)
            REG32(GDMA_CH4_START) |= (0x1 << 15); /* Start DMA channel*/
#elif (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
            REG32(GDMA_CH6_START) |= (0x1 << 15); /* Start DMA channel*/
#endif
            //INFO(connsys, "DMA kick done\n");
            do {
#if (PRODUCT_VERSION == 7687 || PRODUCT_VERSION == 7697)
                reg = REG32(GDMA_CH4_RLCT);
#elif (PRODUCT_VERSION == 7686 || PRODUCT_VERSION == 7682 || PRODUCT_VERSION == 5932)
                reg = REG32(GDMA_CH6_RLCT);
#endif
            } while (reg != 0x00);
            //INFO(connsys, "GDMA done! byte_idx(%d), len_moved_by_dma(%d), remain_send_len(%d), count(%d)\n", (int)byte_idx, (int)len_moved_by_dma, (int)remain_send_len, (int)info.field.count);

        }

pure_pio:
        /* Pure PIO mode */
	 //if (remain_send_len > 1024) 	INFO(connsys, "send by PIO %d bytes\n", (int)remain_send_len);
#if 0
        for (i = 0; i < remain_send_len; i++) {
            *(((uint8_t *)&send_buf) + byte_idx) = *(uint8_t *)src;
            byte_idx ++;
            src ++;
            if (byte_idx == 4) {
                REG32(SDIO_GEN3_CMD53_DATA) = send_buf;
                byte_idx = 0;
                send_cnt += 4;
            }
        }
#else
      byte_idx = 0;

	  while (remain_send_len >= 4)
	  {
#if defined(__CC_ARM)
        //REG(..) = addr;  KEIL build it to "LDM R5!,{R0}". this "addr" must be 4 byte aline. otherwise CM4 will assert.
	    *(((uint8_t *)&send_buf) + byte_idx) = *(uint8_t *)src;
        byte_idx++;
        src ++;
        if(byte_idx == 4) {
            REG32(SDIO_GEN3_CMD53_DATA) = send_buf;
            byte_idx = 0;
            send_cnt += 4;
            remain_send_len -= 4;
        }
#else
            REG32(SDIO_GEN3_CMD53_DATA) = 	*((uint32_t *)src);
            send_cnt += 4;
 	     src += 4;
	     remain_send_len -= 4;
#endif
 	  }

	  if (remain_send_len > 0)
	  {
		  byte_idx = 0;
	        for (i = 0; i < remain_send_len; i++) {
	            *(((uint8_t *)&send_buf) + byte_idx) = *(uint8_t *)src;
	            byte_idx ++;
	            src ++;
	        }
#if (CONNYS_WSAP00041276 == 1)
			if (tx_info->buf_num > 2 && (buf_idx < (tx_info->buf_num -1)))
			{
			  flag = 1;
			  continue;
			}
			flag =0;
#endif						
  	        send_cnt+=4;
              REG32(SDIO_GEN3_CMD53_DATA) = send_buf;
              //INFO(connsys, "rest PIO %d bytes, send_cnt(%d), count(%d)\n", (int)remain_send_len, (int)send_cnt, (int)count);
	  }
#endif
    }
#if 0
    if (byte_idx) {
        REG32(SDIO_GEN3_CMD53_DATA) = send_buf;
        byte_idx = 0;
        send_cnt += 4;
    }
#endif
    /* Send all zero */
    for (i = send_cnt; i < count; i += 4) {
        REG32(SDIO_GEN3_CMD53_DATA) = 0;
    }

    local_irq_restore(flags);
    return CONNSYS_STATUS_SUCCESS;
}

/**
 *	connsys_bus_readl - read a 32 bit integer from a SDIO function
 *	@func: SDIO function to access
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *
 *	Reads a 32 bit integer from the address space of a given SDIO
 *	function. If there is a problem reading the address,
 *	0xffffffff is returned and @err_ret will contain the error
 *	code.
 */
uint32_t connsys_bus_readl(struct connsys_func *func, uint32_t addr, int32_t *err_ret)
{
    uint32_t value;
    connsys_gen3_cmd53_info info;
    uint32_t flags;

    if (err_ret) {
        *err_ret = 0;
    }

    /* CMD53 incremental mode to read 4-byte */
    /* 1. Setup command information */
    info.word = 0;
    info.field.rw_flag = SDIO_GEN3_READ;
    info.field.func_num = func->num; //SDIO_GEN3_FUNCTION_WIFI;
    info.field.block_mode = SDIO_GEN3_BYTE_MODE; /* byte  mode */
    info.field.op_mode = SDIO_GEN3_FIXED_PORT_MODE; // SDIO-GEN3 only apply to fix port, forget  SDIO_GEN3_INCREMENT_MODE; /* increment mode */
    info.field.addr = addr;
    info.field.count = 4;

    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info.word;
    /* 2. CMD53 read out */
    value = REG32(SDIO_GEN3_CMD53_DATA);
    local_irq_restore(flags);

    return value;
}

/**
 *	connsys_bus_writel - write a 32 bit integer to a SDIO function
 *	@func: SDIO function to access
 *	@b: integer to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Writes a 32 bit integer to the address space of a given SDIO
 *	function. @err_ret will contain the status of the actual
 *	transfer.
 */
void connsys_bus_writel(struct connsys_func *func, uint32_t b, uint32_t addr, int32_t *err_ret)
{
    connsys_gen3_cmd53_info info;
    uint32_t flags;

    if (err_ret) {
        *err_ret = 0;
    }

    /* CMD53 incremental mode to read 4-byte */
    /* 1. Setup command information */
    info.word = 0;
    info.field.rw_flag = SDIO_GEN3_WRITE;
    info.field.func_num = func->num; //SDIO_GEN3_FUNCTION_WIFI;
    info.field.block_mode = SDIO_GEN3_BYTE_MODE; /* byte  mode */
    info.field.op_mode = SDIO_GEN3_FIXED_PORT_MODE; // SDIO-GEN3 only apply to fix port, forget  SDIO_GEN3_INCREMENT_MODE; /* increment mode */
    info.field.addr = addr;
    info.field.count = 4;
    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info.word;
    /* 2. CMD53 write data  */
    REG32(SDIO_GEN3_CMD53_DATA) = b;
    local_irq_restore(flags);


}

/**
 *	connsys_bus_fn0_read_byte - read a single byte from SDIO function 0
 *	@func: an SDIO function of the card
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *
 *	Reads a single byte from the address space of SDIO function 0.
 *	If there is a problem reading the address, 0xff is returned
 *	and @err_ret will contain the error code.
 */
uint8_t connsys_bus_fn0_read_byte(struct connsys_func *func, uint32_t addr,
                                  int32_t *err_ret)
{
    //int32_t ret;
    uint8_t val;
    connsys_gen3_cmd52_info info;
    info.word = 0;
    uint32_t flags;
    /* CMD52 read 1-byte of func0 */

    if (err_ret) {
        *err_ret = 0;
    }

    /* 1. Setup command information */
    info.field.rw_flag = SDIO_GEN3_READ;
    info.field.func_num = 0; //func->num;
    info.field.addr = addr;

    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info.word;
    /* 2. CMD52 read  data  */
    val = REG32(SDIO_GEN3_CMD52_DATA);
    local_irq_restore(flags);
    //ret = mmc_io_rw_direct(func->card, 0, 0, addr, 0, &val);

    return val;
}

/**
 *	connsys_bus_fn0_write_byte - write a single byte to SDIO function 0
 *	@func: an SDIO function of the card
 *	@b: byte to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Writes a single byte to the address space of SDIO function 0.
 *	@err_ret will contain the status of the actual transfer.
 *
 *	Only writes to the vendor specific CCCR registers (0xF0 -
 *	0xFF) are permiited; @err_ret will be set to -EINVAL for *
 *	writes outside this range.
 */
void connsys_bus_fn0_write_byte(struct connsys_func *func, uint8_t b, uint32_t addr,
                                int32_t *err_ret)
{
    //int32_t ret;
    connsys_gen3_cmd52_info info;
    info.word = 0;
    uint32_t flags;
    /* CMD52 write 1-byte of func0 */

    if (err_ret) {
        *err_ret = 0;
    }

    /* 1. Setup command information */
    info.field.rw_flag = SDIO_GEN3_WRITE;
    info.field.func_num = 0; //func->num;
    info.field.addr = addr;
    info.field.data = b;
    INFO(connsys, "=== write f0, setup=0x%x\n", (unsigned int)info.word);

    local_irq_save(flags);
    REG32(SDIO_GEN3_CMD_SETUP) = info.word;
    /* 2. CMD52 write dummy 0 to trigger write  data  */
#if 1  //def MTK_MT7686_ENABLE
    REG32(SDIO_GEN3_CMD52_DATA) = 0;
#else
    REG32(SDIO_GEN3_CMD52_DATA) = b;
#endif

    local_irq_restore(flags);
}


/**
 *	connsys_bus_enable_func - enables a SDIO function for usage
 *	@func: SDIO function to enable
 *
 *	Powers up and activates a SDIO function so that register
 *	access is possible.
 */
int32_t connsys_bus_enable_func(struct connsys_func *func)
{
    int32_t ret;
    uint8_t reg;

    INFO(connsys, "SDIO: Enabling Function %d...\n", (int)func->num);

    reg = connsys_bus_fn0_read_byte(func, SDIO_CCCR_IOEx, &ret);
    if (ret) {
        goto err;
    }
    INFO(connsys, "Origin Func enable=0x%x\n", reg);
    reg |= 1 << func->num;

    connsys_bus_fn0_write_byte(func, reg, SDIO_CCCR_IOEx, &ret);
    if (ret) {
        goto err;
    }

    reg = connsys_bus_fn0_read_byte(func, SDIO_CCCR_IORx, &ret);
    if (ret) {
        goto err;
    }
    INFO(connsys, "Read CCCR_IORx=0x%x\n", reg);
    if (!(reg & (1 << func->num))) {
        ret = -ETIME;
        goto err;
    }
    INFO(connsys, "SDIO: Enabled Function %d\n", (int)func->num);

    return 0;

err:
    INFO(connsys, "SDIO: Failed to enable Function %d\n", (int)func->num);
    return ret;
}
/**
 *	connsys_bus_disable_func - disable a SDIO function
 *	@func: SDIO function to disable
 *
 *	Powers down and deactivates a SDIO function. Register access
 *	to this function will fail until the function is reenabled.
 */
int32_t connsys_bus_disable_func(struct connsys_func *func)
{
    int32_t ret;
    uint8_t reg;

    INFO(connsys, "SDIO: Disabling Function %d...\n", (int)func->num);

    reg = connsys_bus_fn0_read_byte(func, SDIO_CCCR_IOEx, &ret);
    if (ret) {
        goto err;
    }

    reg &= ~(1 << func->num);

    connsys_bus_fn0_write_byte(func, reg, SDIO_CCCR_IOEx, &ret);
    if (ret) {
        goto err;
    }

    INFO(connsys, "SDIO: Disabled Function %d\n", (int)func->num);

    return 0;

err:
    ret = -EIO;
    INFO(connsys, "SDIO: Failed to Disable Function %d\n", (int)func->num);
    return ret;

}

/**
 *	connsys_bus_set_block_size - set the block size of an SDIO function
 *	@func: SDIO function to change
 *	@blksz: new block size or 0 to use the default.
 *
 *	The default block size is the largest supported by both the function
 *	and the host, with a maximum of 512 to ensure that arbitrarily sized
 *	data transfer use the optimal (least) number of commands.
 *
 *	A driver may call this to override the default block size set by the
 *	core. This can be used to set a block size greater than the maximum
 *	that reported by the card; it is the driver's responsibility to ensure
 *	it uses a value that the card supports.
 *
 *	Returns 0 on success, -EINVAL if the host does not support the
 *	requested block size, or -EIO (etc.) if one of the resultant FBR block
 *	size register writes failed.
 *
 */
int32_t connsys_bus_set_block_size(struct connsys_func *func, uint32_t blksz)
{
    int32_t ret;

    //ret = mmc_io_rw_direct(func->card, 1, 0,
    //	SDIO_FBR_BASE(func->num) + SDIO_FBR_BLKSIZE,
    //	blksz & 0xff, NULL);

    connsys_bus_fn0_write_byte(func, (blksz & 0xff),
                               SDIO_FBR_BASE(func->num) + SDIO_FBR_BLKSIZE, &ret);

    if (ret) {
        return ret;
    }

    connsys_bus_fn0_write_byte(func, ((blksz >> 8) & 0xff),
                               SDIO_FBR_BASE(func->num) + SDIO_FBR_BLKSIZE + 1, &ret);

    if (ret) {
        return ret;
    }
    func->blksize = blksz;
    return 0;
}

/**
 *	connsys_claim_irq - claim the IRQ for a SDIO function
 *	@func: SDIO function
 *	@handler: IRQ handler callback
 *
 *	Claim and activate the IRQ for the given SDIO function. The provided
 *	handler will be called when that IRQ is asserted.  The host is always
 *	claimed already when the handler is called so the handler must not
 *	call connsys_claim_host() nor connsys_release_host().
 */
int32_t connsys_bus_get_irq(struct connsys_func *func, connsys_irq_handler_t handler)
{
    int32_t ret;
    uint8_t reg = 0;

    INFO(connsys, "SDIO: Enabling IRQ for func%d...\n", (int)func->num);

    if (func->irq_callback) {
        ERR(connsys, "SDIO: IRQ for func%d already in use.\n", (int)func->num);
        return -2; //-EBUSY;
    }

    reg = connsys_bus_fn0_read_byte(func, SDIO_CCCR_IENx, &ret);
    if (ret) {
        return ret;
    }

    reg |= 1 << func->num;

    reg |= 1; /* Master interrupt enable */
    INFO(connsys, "Write IENx=0x%x\n", reg);
    connsys_bus_fn0_write_byte(func, reg, SDIO_CCCR_IENx, &ret);
    if (ret) {
        return ret;
    }

    func->irq_callback = handler;

    reg = connsys_bus_fn0_read_byte(func, SDIO_CCCR_IENx, &ret);
    if (ret) {
        return ret;
    }
    INFO(connsys, "===> IENx=0x%x\n", reg);
    return ret;
}

/**
 *	connsys_bus_release_irq - release the IRQ for a SDIO function
 *	@func: SDIO function
 *
 *	Disable and release the IRQ for the given SDIO function.
 */
int32_t connsys_bus_release_irq(struct connsys_func *func)
{
    int32_t ret;
    uint8_t reg = 0;

    INFO(connsys, "SDIO: Disabling IRQ for func%d...\n", (int)func->num);

    if (func->irq_callback) {
        func->irq_callback = NULL;
    }

    connsys_bus_fn0_read_byte(func, SDIO_CCCR_IENx, &ret);
    if (ret) {
        return ret;
    }

    reg &= ~(1 << func->num);

    /* Disable master interrupt with the last function interrupt */
    if (!(reg & 0xFE)) {
        reg = 0;
    }

    connsys_bus_fn0_write_byte(func, reg, SDIO_CCCR_IENx, &ret);
    if (ret) {
        return ret;
    }

    return 0;
}

