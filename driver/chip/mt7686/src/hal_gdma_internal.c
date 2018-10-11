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
 
#include "hal_gdma.h"
#ifdef HAL_GDMA_MODULE_ENABLED

#include "hal_gdma_internal.h"
#include "hal_pdma_internal.h"

/*general dma base address array */
GDMA_REGISTER_T  *gdma[GDMA_NUMBER] = {GDMA1};

/*general dma global status base address array */
GDMA_REGISTER_GLOBAL_T  *gdma_global[GDMA_NUMBER] = {GDMA_GLOAL1};

volatile uint32_t gdma_status = 0;

/*general dma's  callback function array*/
gdma_user_callback_t g_gdma_callback[GDMA_NUMBER] = {
    {NULL, NULL}
};

//#define GDMA_DVT_API_FOR_INTERNAL_MODULE   // eg:EMI test
gdma_transfer_size_format_t gdma_calculate_size(
    uint32_t length,
    uint32_t *count)
{
    if (length == MAX_LENGTH_VALUE) {
        *count = MAX_COUNT_VALUE;
        return GDMA_WORD;
    }
    if (0 == (length % 4)) {
        *count = length / 4;
        return GDMA_WORD;
    } else if (0 == (length % 2)) {
        *count = length / 2;
        return GDMA_HALF_WORD;
    } else {
        *count = length;
        return GDMA_BYTE;
    }

}

void gdma_reset(uint32_t channel)

{
    /*reset gdma default setting*/
    gdma[channel]->GDMA_CON_UNION.GDMA_CON = 0x0;
    gdma[channel]->GDMA_START = 0x0;
    gdma[channel]->GDMA_INTSTA = 0x0;
    gdma[channel]->GDMA_SRC = 0x0;
    gdma[channel]->GDMA_DST = 0x0;
	gdma[channel]->GDMA_ACKINT = GDMA_ACKINT_BIT_MASK;
    gdma[channel]->GDMA_WPPT = 0x0; 
	gdma[channel]->GDMA_WPTO = 0x0;
	gdma[channel]->GDMA_LIMITER = 0x0;
}




uint32_t gdma_get_global_status(uint32_t channel)
{

    uint32_t global_status = 0 ;

    /* read gdma running  status */
    global_status = gdma_global[channel]->GDMA_GLBSTA;

    return  global_status;

}

void gdma_register_callback(uint32_t channel, hal_gdma_callback_t callback, void *user_data)

{
    if (g_gdma_callback[channel].func == NULL) {
        g_gdma_callback[channel].func = callback;
        g_gdma_callback[channel].argument	= user_data;
    }
}

void gdma_stop(uint32_t channel)
{
    gdma[channel]->GDMA_START = GDMA_STOP_BIT_MASK;
}

void gdma_start(uint32_t channel)
{
    gdma[channel]->GDMA_START = GDMA_START_BIT_MASK;
}

void gdma_set_size(uint32_t channel, gdma_transfer_size_format_t size)
{

    gdma[channel]->GDMA_CON_UNION.GDMA_CON_CELLS.GDMA_SIZE = size;
}

void gdma_set_direction(uint32_t channel, uint8_t direction)
{
    gdma[channel]->GDMA_CON_UNION.GDMA_CON_CELLS.GDMA_DIRECTION = direction;

}

void gdma_set_iten(uint32_t channel, bool enable)
{
     gdma[channel]->GDMA_CON_UNION.GDMA_CON_CELLS.GDMA_ITEN = enable;
}


void gdma_set_address(uint32_t channel, uint32_t destination, uint32_t source)
{
    gdma[channel]->GDMA_SRC = source;
    gdma[channel]->GDMA_DST = destination;

}

void gdma_set_count(uint32_t channel, uint32_t count)
{
    gdma[channel]->GDMA_COUNT = count;

}

void gdma_init(uint32_t channel)
{

  ///TODO: enable dma clock 

/* keep driver default setting */
 gdma_reset(channel);
 /*register irq handler*/
 hal_nvic_register_isr_handler(DMA_IRQn, dma_interrupt_hander);
 hal_nvic_enable_irq(DMA_IRQn);

}


#ifdef GDMA_DVT_API_FOR_INTERNAL_MODULE

void gdma_set_control(uint32_t channel, uint32_t control)
{
     gdma[channel]->GDMA_CON_UNION.GDMA_CON = control;

}


void gdma_set_wrap(uint32_t channel, uint32_t wppt, uint32_t wpto)
{
    gdma[channel]->GDMA_WPPT = wppt; 
	gdma[channel]->GDMA_WPTO = wpto;

}

void gdma_set_limiter(uint32_t channel, uint32_t limiter)
{
    gdma[channel]->GDMA_LIMITER = limiter;

}


void  gdma_advanced_config_and_start(uint32_t channel, gdma_config_t *cfg ) 

{
    //int size;
    uint32_t con = 0;

    /* Stop DMA first */
    gdma_stop(channel);
    /*Set DMA  source and dest address*/
    gdma_set_address(channel,cfg->dest_address, cfg->source_address);
	/*Set DMA count*/
    gdma_set_count(channel,cfg->count);
    /*Set DMA wrap*/
    if(cfg->wrap)
    {
      gdma_set_wrap(channel,cfg->wppt,cfg->wpto);
      gdma_set_count(channel,cfg->count);
    }
	/*Set DMA wrap*/
	gdma_set_limiter(channel,cfg->limiter);
    /* WRAP */  
    if (cfg->wrap == 0) {
        /* no wrap */
        con = con & ~GDMA_CON_WPEN_MASK;
    } else if (cfg->wrap == 1) {
        /* wrap on src */
        con = (con | GDMA_CON_WPEN_MASK);
        con = (con & ~GDMA_CON_WPSD_MASK);
    } else if (cfg->wrap == 2) {
        /* wrap on dst */
        con = (con | GDMA_CON_WPEN_MASK);
        con = (con | GDMA_CON_WPSD_MASK);
    } else {
        /* default: no wrap */
        con = con & ~GDMA_CON_WPEN_MASK;
    }

    /* INC_SRC */
    if (cfg->inc_src) {
        con = (con | GDMA_CON_SINC_MASK);
    } else {
        con = (con & ~GDMA_CON_SINC_MASK);
    }

    /* INC_DST */
    if (cfg->inc_dst) {
        con = (con | GDMA_CON_DINC_MASK);
    } else {
        con = (con & ~GDMA_CON_DINC_MASK);
    }

    /* BURST */

    if (cfg->burst == DMA_BURST_SINGLE) {
        con = (con & ~GDMA_CON_BURST_MASK);
    } else if (cfg->burst == DMA_BURST_4BEAT) {
        con = (con & ~GDMA_CON_BURST_MASK);
		con = (con | GDMA_CON_BURST_4BEAT_MASK);
    } else {
		 //dbg_print("unsupport DMA burst!\n");
		 while(1);
	}

	/* SIZE */

    if (cfg->size == DMA_SIZE_1B) {
        con = (con & ~GDMA_CON_SIZE_MASK) | GDMA_CON_SIZE_BYTE_MASK;
    } else if (cfg->size == DMA_SIZE_2B) {
        con = (con & ~GDMA_CON_SIZE_MASK) | GDMA_CON_SIZE_HALF_WORD_MASK;
    } else if (cfg->size == DMA_SIZE_4B) {
        con = (con & ~GDMA_CON_SIZE_MASK) | GDMA_CON_SIZE_WORD_MASK;
    } else {
    	//dbg_print("unsupport DMA size!\n");
        while(1);
    }
    /*gdma set control*/
	gdma_set_control(channel, con);
   
    /* ITEN */
	gdma_set_iten(channel, cfg->iten);
 
    /* To make sure the dma_status is reset */
    gdma_status = 0;

    /* START */
    if(cfg->start == true) {
          gdma_start(channel);
    	}
}


void gdma_deinit(uint32_t channel)
{

/* stop gdma */
 gdma_stop(channel);

///TODO: disable dma clock 

}
#endif
#endif //#ifdef HAL_GDMA_MODULE_ENABLED

