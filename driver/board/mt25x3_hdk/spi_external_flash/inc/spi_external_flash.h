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

/* Includes ------------------------------------------------------------------*/
#ifndef _SERIAL_FLASH_H_
#define _SERIAL_FLASH_H_ 


/* 4 bytes Command + 256 byte data */
#define WRITE_BUFFER_SIZE     (0x104)
#define FLASH_DADA_MAX_LENGTH (0x100)

#define COMMAND_SIZE    (0x5)
#define FLASH_BUSY      (0x1)
#define FLASH_INIT      (2)
#define FLASH_NOT_INIT  (-1)


typedef enum {
    FLASH_STATUS_IDLE = 0,
    FLASH_STATUS_BUSY = 1,
} flash_status_t;

typedef enum {
	  SPI_FLASH_STATUS_NOT_INIT = -2,
    SPI_FLASH_STATUS_ERROR = -1,
    SPI_FLASH_STATUS_OK,
} spi_flash_status_t;

typedef enum {
    FLASH_SECTOR_4K,
   	FLASH_SECTOR_32K,
   	FLASH_SECTOR_64K,
   	FLASH_SECTOR_ALL,
} block_size_type_t;	


flash_status_t get_flash_status(void);

flash_status_t wait_flash_ready(uint32_t sleep_ms);

int32_t spi_external_flash_get_rdid(uint8_t *buffer);

int32_t spi_external_flash_init(uint32_t frequency);

int32_t spi_external_flash_deinit();

int32_t spi_external_flash_read(uint32_t address, uint8_t *buffer, uint32_t length);

int32_t spi_external_flash_write(uint32_t address, uint8_t *data, int32_t length);

int32_t spi_external_flash_erase(uint32_t address, block_size_type_t block_size);

/*-----------------------------------------------------------*/
#endif


