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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "memory_attribute.h"

/* hal includes */
#include "hal.h"
#include "sdio_hal.h"
#include "sdio_reg.h"


#if (PRODUCT_VERSION == 2523)
#define MT5932_RESET_PIN  HAL_GPIO_48   /* The GPIO pin to control reset pin of MT5932 */
#else
#define MT5932_RESET_PIN  HAL_GPIO_18   /* The GPIO pin to control reset pin of MT5932 */
#endif

#define SDIO_SWINT_MB0_BROMSEND   0x00020000
#define SDIO_SWINT_MB0_BROMRECV   0x00010000
#define SDIO_SWINT_MB_SENDDATA    0x00020000
#define SDIO_SWINT_MB_RECVDATA    0x00010000
#define SDIO_SWINT_ERROR_BROMSEND 0x00080000
#define SDIO_SWINT_ERROR_BROMRECV 0x00040000

#define BROM_RX_Q0_PKT_CNT        (16)
#define BROM_RX_Q1_PKT_CNT        (16)

#define BROM_BLOCK_SIZE           (256)

//#define MAX_LOAD_SIZE             (352*1024)   //E1
#define MAX_LOAD_SIZE             (378*1024)   //E2
#define MAX_BLOCK_SIZE            (2048)

#define BROM_BUFF_SIZE            (4096)


ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char sdio_tx_buff[BROM_BUFF_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t whisr_enhance_reg[512];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t sdio_rx_buffer[BROM_BUFF_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint32_t brom_tx_test[2048];

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t value[4];

#define WIFI_BLK_SIZE (256)
#define WIFI_SDIO_PORT HAL_SDIO_PORT_0


#ifndef ALIGN_TO_BLOCK_SIZE
#define ALIGN_TO_BLOCK_SIZE(_value)     (((_value) + (BROM_BLOCK_SIZE - 1)) & ~(BROM_BLOCK_SIZE - 1))
#endif

#ifndef ALIGN_4
#define ALIGN_4(_value)             (((_value) + 3) & ~3u)
#endif /* ALIGN_4 */

typedef struct {
    union {
        struct {
            uint32_t  length: 16;
            uint32_t  reserved: 13;
            uint32_t  tx_type: 3;
        } bits;
        uint32_t      asUINT32;
    } u;
} brom_sdio_tx_sdu_header_t;  // 4byte tx header

typedef struct {
    union {
        struct {
            uint32_t  tx_done_int: 1;
            uint32_t  rx0_done_int: 1;
            uint32_t  rx1_done_int: 1;
            uint32_t  rx2_done_int: 1;
            uint32_t  rx3_done_int: 1;
            uint32_t  reserved: 1;
            uint32_t  abnormal_int: 1;
            uint32_t  fw_ownback_int: 1;
            uint32_t  d2h_sw_int: 24;
        } bits;
        uint32_t      asUINT32;
    } u;
} sdio_whisr_t;

typedef struct {
    union {
        struct {
            uint32_t  tq0_cnt: 8;
            uint32_t  tq1_cnt: 8;
            uint32_t  tq2_cnt: 8;
            uint32_t  tq3_cnt: 8;
        } bits;
        uint32_t      asUINT32;
    } u;
} sdio_wtsr0_t;

typedef struct {
    union {
        struct {
            uint32_t  tq4_cnt: 8;
            uint32_t  tq5_cnt: 8;
            uint32_t  tq6_cnt: 8;
            uint32_t  tq7_cnt: 8;
        } bits;
        uint32_t      asUINT32;
    } u;
} sdio_wtsr1_t;

typedef struct {
    sdio_whisr_t  whisr ;
    sdio_wtsr0_t  whtsr0 ;
    sdio_wtsr1_t  whtsr1 ;
    uint16_t      rx0_num ;
    uint16_t      rx1_num ;
    uint16_t      rx0_pkt_len[BROM_RX_Q0_PKT_CNT];
    uint16_t      rx1_pkt_len[BROM_RX_Q1_PKT_CNT];
    uint32_t      d2hrm0r ;
    uint32_t      d2hrm1r ;
} sdio_whisr_enhance_t;


typedef enum {
    BIN_LOAD_START     = 0,
    ACK_BIN_LOAD_START = 1,
    GET_BIN            = 2,
    ACK_GET_BIN        = 3,
    BIN_LOAD_END       = 4,
    ACK_BIN_LOAD_END   = 5,
    MD_BOOT_END        = 6,
    ACK_MD_BOOT_END    = 7,
    MD_MSG_OUTPUT      = 8,
    ACK_MD_MSG_OUTPUT  = 9,
    MD_MSG_FLUSH       = 10,
    ACK_MD_MSG_FLUSH   = 11,
    XBOOT_CMD_ID_END   = 0x0FFFFFFF
} xboot_cmd_id_t;


typedef struct {
    uint32_t        m_magic;
    xboot_cmd_id_t  m_msg_id;
    uint32_t        m_status;
    uint32_t        m_reserve[5];
} xboot_cmd_status_t;

typedef struct {
    uint32_t        m_magic;
    xboot_cmd_id_t  m_msg_id;
    uint32_t        m_offset;
    uint32_t        m_len;
    uint32_t        m_reserve[4];
} xboot_cmd_getbin_t;


/*----------------------------------------------------------------------------*/
/*!
* \brief Read device I/O port
*
* \param[in] port_offset             I/O port offset
* \param[in] length              Length to be read
* \param[out] read_buffer            Pointer to read buffer
*
* \retval TRUE          operation success
* \retval FALSE         operation fail
*/
/*----------------------------------------------------------------------------*/
static bool sdio_read_device_port(uint32_t port_offset, uint16_t length, uint32_t *read_buffer, hal_sdio_function_id_t func)
{
    hal_sdio_status_t status;
    uint8_t block_number = 0;
    uint32_t  func1_block_size;
    hal_sdio_command53_config_t cmd_cfg;

    /*addr 4-byte align check*/
    //assert((((uint32_t)read_buffer) & 0x03) == 0);

    if (((uint32_t)read_buffer) & 0x03) {
        printf("Error!!!kalDevPortRead:Invalid Buffer Addr. \r\n");
        return false;
    }

    cmd_cfg.direction = HAL_SDIO_DIRECTION_READ;
    cmd_cfg.function = func;
    cmd_cfg.operation = HAL_SDIO_FIXED_ADDRESS;
    cmd_cfg.address = port_offset;
    cmd_cfg.buffer = (uint32_t)read_buffer;

    func1_block_size = WIFI_BLK_SIZE;
    while (length >= func1_block_size) {
        length -= func1_block_size;
        block_number++;
    }
    if (length > 0 && block_number > 0) {
        block_number++;
    }

    if (block_number > 0) { /* block mode*/
        cmd_cfg.block = true;
        cmd_cfg.count = block_number;
    } else {
        cmd_cfg.block = false;
        cmd_cfg.count = length;//ALIGN_32(length);
    }

    status = hal_sdio_execute_command53(WIFI_SDIO_PORT, &cmd_cfg);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio read error. \r\n");
        return false;
    } else {
        return true;
    }
}

/*----------------------------------------------------------------------------*/
/*!
* \brief Write device I/O port
*
* \param[in] port_offset             I/O port offset
* \param[in] length              Length to be write
* \param[in] write_buffer             Pointer to write buffer
*
* \retval TRUE          operation success
* \retval FALSE         operation fail
*/
/*----------------------------------------------------------------------------*/
static bool sdio_write_device_port(uint32_t  port_offset, uint16_t length, uint8_t *write_buffer, hal_sdio_function_id_t func)
{
    hal_sdio_status_t status;
    uint8_t block_number = 0;
    uint32_t func1_block_size;
    hal_sdio_command53_config_t cmd_cfg;
    uint32_t total_length;
    uint32_t length_backup;

    /*addr 4-byte align check*/
    //assert((((uint32_t)write_buffer) & 0x03) == 0);

    if (((uint32_t)write_buffer) & 0x03) {
        printf("Error!!!Invalid Buffer Addr. \r\n");
        return false;
    }

    cmd_cfg.direction = HAL_SDIO_DIRECTION_WRITE;
    cmd_cfg.function = func;
    cmd_cfg.operation = HAL_SDIO_FIXED_ADDRESS;
    cmd_cfg.address = port_offset;
    cmd_cfg.buffer = (uint32_t)write_buffer;

    length_backup = length;

    func1_block_size = WIFI_BLK_SIZE;
    while (length >= func1_block_size) {
        length -= func1_block_size;
        block_number++;
    }
    if (length > 0 && block_number > 0) {
        block_number++;
    }

    if (block_number > 0) { // block mode
        cmd_cfg.block = true;
        cmd_cfg.count = block_number;
        total_length = block_number * func1_block_size;//ALIGN_32(block_number * func1_block_size);
    } else {
        cmd_cfg.block = false;
        cmd_cfg.count = length;// ALIGN_32(length);
        total_length = length; //ALIGN_32(length);
    }

    if ((total_length - length_backup) >= sizeof(uint32_t)) {
        /*set unisgned int to Zero for 4-byte unalign address, will shift to align address*/
        uint32_t aliglen = length_backup; //ALIGN_4(length_backup);
        write_buffer[aliglen] = 0;
        write_buffer[aliglen + 1] = 0;
        write_buffer[aliglen + 2] = 0;
        write_buffer[aliglen + 3] = 0;
    }

    status = hal_sdio_execute_command53(WIFI_SDIO_PORT, &cmd_cfg);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio write error. \r\n");
        return false;
    } else {
        return true;
    }
}


int sdio_func1_wr(unsigned int u4Register, void *pBuffer,  unsigned int Length)
{
    int ret, i;

    //assert((pBuffer != NULL));
    if (!pBuffer) {
        return 1;
    }

    ret = sdio_write_device_port(u4Register, Length, pBuffer, 1);
    if (!ret) {
        printf("[ERR] function 1 write fail : addr : 0x%x , size : %d, err_ret: 0x%x\r\n", u4Register, Length, ret);
        printf("Error packet content = ") ;

        for (i = 0; i < Length; i++) {
            printf("%x ", *(unsigned char *)(pBuffer + i));
        }

        printf(" \r\n ");
    }

    return (ret) ? 0 : 1 ;
}

int sdio_func1_rd(unsigned int u4Register, void *pBuffer,  unsigned int Length)
{
    int ret ;

    //assert((pBuffer != NULL)) ;
    if (!pBuffer) {
        return 1;
    }

    ret = sdio_read_device_port(u4Register, Length, pBuffer, 1);
    if (!ret) {
        printf("[ERR] function 1 read fail : addr : 0x%x , size : %d, err_ret: 0x%x\r\n", u4Register, Length, ret) ;
    }

    return (ret) ? 0 : 1 ;
}



static bool sdio_hif_get_driver_own(void)
{
    bool ret ;
    uint32_t value ;
    uint32_t cnt = 50;

    printf("[sdio_hif_get_driver_own]<==========>\r\n") ;

    //Set driver own
    value = W_FW_OWN_REQ_CLR ;
    if ((ret = sdio_func1_wr(SDIO_IP_WHLPCR, &value, 4)) != 0) {
        return false;
    }

    hal_gpt_delay_ms(2);

    while (cnt--) {
        if ((ret = sdio_func1_rd(SDIO_IP_WHLPCR, &value, 4)) != 0) {
            return false ;
        }

        if (value & W_DRV_OWN_STATUS) {
            return true;
        }
        hal_gpt_delay_ms(10);
    }

    return false;
}

static bool h2d_send_mailbox(uint32_t cmd)
{
    uint32_t h2d_mb;

    if (0 != sdio_func1_wr(SDIO_IP_H2DSM0R, &cmd, 4)) {
        return false;
    }

    h2d_mb = SDIO_SWINT_MB0_BROMSEND;
    if (0 != sdio_func1_wr(SDIO_IP_WSICR, &h2d_mb, 4)) {
        return false;
    }

    return true;
}

bool h2d_receive_mailbox(uint32_t *cmd)
{
    uint32_t whisr;
    uint32_t wait_count = 0;

    do {
        wait_count++;
        hal_gpt_delay_ms(1);
        sdio_func1_rd(SDIO_IP_WHISR, &whisr, 4);
    } while (0 == (whisr & SDIO_SWINT_MB0_BROMRECV));

    if (0 != sdio_func1_rd(SDIO_IP_D2HRM0R, cmd, 4)) {
        return false;
    }

    whisr = SDIO_SWINT_MB0_BROMRECV;
    sdio_func1_wr(SDIO_IP_WHISR, &whisr, 4);

    return true;
}


static bool sdio_send_pkt(unsigned char tx_que, uint8_t *tx_buf, int data_length)
{
    uint32_t whisr_reg = 0;
    unsigned char *buf;
    brom_sdio_tx_sdu_header_t *tx_pkt_header;

    memset(&sdio_tx_buff, 0, BROM_BUFF_SIZE);
    buf = sdio_tx_buff;

    tx_pkt_header = (brom_sdio_tx_sdu_header_t *)buf;
    buf += sizeof(brom_sdio_tx_sdu_header_t);

    tx_pkt_header->u.asUINT32 = 0;
    if (tx_que != 0) {
        tx_pkt_header->u.bits.tx_type = tx_que - 1;
    }

    tx_pkt_header->u.bits.length = data_length + sizeof(brom_sdio_tx_sdu_header_t);

    memcpy(buf, tx_buf , data_length);

    do {
        sdio_func1_rd(SDIO_IP_WHISR, &whisr_reg, 4);
        hal_gpt_delay_ms(1);
    } while (0 == (whisr_reg & 0x01)); /*waiting TX_DONE_INT*/

    if (sdio_func1_wr(SDIO_IP_WTDR1, &sdio_tx_buff, ALIGN_TO_BLOCK_SIZE(data_length + sizeof(brom_sdio_tx_sdu_header_t)))) {
        printf("[ERR] sdio_send_pkt => sdio_func1_wr 0x%08x len=%d error\r\n", SDIO_IP_WTDR1, (data_length + sizeof(brom_sdio_tx_sdu_header_t)));
        return false;
    }

    return true;
}

static bool sdio_receive_pkt(uint8_t *rx_buf)
{
    uint32_t whisr_reg = 0;
    uint16_t  *fw_rx_pkt_num ;
    uint16_t  *fw_rx_pkt_len ;
    uint32_t total_len = 0;
    sdio_whisr_enhance_t *enhance_reg;
    uint32_t i;

    whisr_reg = 0;
    do {
        sdio_func1_rd(SDIO_IP_WHISR, &whisr_reg, 4);
        hal_gpt_delay_ms(1);
    } while (0 == (whisr_reg & 0x06)); /*wait RX0_DONE_INT & RX1_DONE_INT*/

    memset(&whisr_enhance_reg , 0, sizeof(sdio_whisr_enhance_t));
    if (sdio_func1_rd(SDIO_IP_WHISR, &whisr_enhance_reg, ALIGN_TO_BLOCK_SIZE(sizeof(sdio_whisr_enhance_t)))) {
        printf("[ERR]sdio_receive_pkt => sdio_func1_rd SDIO_IP_WHISR fail\r\n");
        return false;
    }

    enhance_reg = (sdio_whisr_enhance_t *)(&whisr_enhance_reg);
    fw_rx_pkt_num = &(enhance_reg->rx0_num) ;
    fw_rx_pkt_len = enhance_reg->rx0_pkt_len ;


    total_len = 0 ;
    for (i = 0; i < *fw_rx_pkt_num; i++) {
        total_len += ALIGN_4(fw_rx_pkt_len[i]);
    }

    if (sdio_func1_rd(SDIO_IP_WRDR0, rx_buf, ALIGN_TO_BLOCK_SIZE((total_len)))) {
        printf("[ERR],sdio_receive_pkt, sdio_func1_rd SDIO_IP_WRDR0 fail\r\n");
        return false;
    }

    return true;
}


bool xboot_over_sdio_fw_download(uint8_t *address, uint32_t length)
{
    uint32_t reg_value = 0;
    uint32_t i;
    uint32_t total_length, block_size;
    xboot_cmd_status_t *cmd;
    xboot_cmd_getbin_t *get_bin;

    hal_sdio_status_t status = 0;
    hal_sdio_config_t sdio_config = {HAL_SDIO_BUS_WIDTH_1, 10000};
    uint32_t blk_size;

    uint32_t checksum = 0;
    uint32_t checksum_received = 0;

    total_length = length;

    // check parameter
    if ((length == 0) || (length > MAX_LOAD_SIZE)) {
        printf("Invalid length = %d \r\n", (int)length);
        return false;
    }

#if (PRODUCT_VERSION == 2523)
    hal_pinmux_set_function(HAL_GPIO_24, HAL_GPIO_24_MC1_B_DA3);// MC1_CK
    hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_MC1_B_DA2);// MC1_CM0
    hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_MC1_B_CM0);// MC1_DA0
    hal_pinmux_set_function(HAL_GPIO_27, HAL_GPIO_27_MC1_B_CK);// MC1_DA1
    hal_pinmux_set_function(HAL_GPIO_28, HAL_GPIO_28_MC1_B_DA0);// MC1_DA2
    hal_pinmux_set_function(HAL_GPIO_29, HAL_GPIO_29_MC1_B_DA1);// MC1_DA3

    hal_pinmux_set_function(HAL_GPIO_30, HAL_GPIO_30_MC0_CK);// MC0_CK
    hal_pinmux_set_function(HAL_GPIO_31, HAL_GPIO_31_MC0_CM0);// MC0_CM0
    hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_MC0_DA0);// MC0_DA0
    hal_pinmux_set_function(HAL_GPIO_33, HAL_GPIO_33_MC0_DA1);// MC0_DA1
    hal_pinmux_set_function(HAL_GPIO_34, HAL_GPIO_34_MC0_DA2);// MC0_DA2
    hal_pinmux_set_function(HAL_GPIO_35, HAL_GPIO_35_MC0_DA3);// MC0_DA3
#else
    hal_pinmux_set_function(11, 4);
    hal_pinmux_set_function(12, 4);
    hal_pinmux_set_function(13, 4);
    hal_pinmux_set_function(14, 4);
    hal_pinmux_set_function(15, 4);
    hal_pinmux_set_function(16, 4);

    hal_gpio_set_pupd_register(11, 0, 0, 1);
    hal_gpio_set_pupd_register(12, 0, 0, 1);
    hal_gpio_set_pupd_register(13, 0, 0, 1);
    hal_gpio_set_pupd_register(14, 0, 0, 1);
    hal_gpio_set_pupd_register(15, 0, 0, 1);
    hal_gpio_set_pupd_register(16, 0, 0, 1);
#endif

    hal_gpio_init(MT5932_RESET_PIN);
    hal_pinmux_set_function(MT5932_RESET_PIN, 0);
    hal_gpio_set_direction(MT5932_RESET_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(MT5932_RESET_PIN, HAL_GPIO_DATA_LOW);
    hal_gpt_delay_ms(10);
    hal_gpio_set_output(MT5932_RESET_PIN, HAL_GPIO_DATA_HIGH);

    /* wait MT5932 finish straping */
    hal_gpt_delay_ms(10);


    //sdio_init();
    /* Excute SDIO SW hook flow */
    do {
        status = hal_sdio_init(WIFI_SDIO_PORT, &sdio_config);
        printf("hal_sdio_init status = %d \r\n", status);
        if (HAL_SDIO_STATUS_OK != status) {
            printf("sdio init error. \r\n");
        }
        hal_gpt_delay_ms(10);
    } while (HAL_SDIO_STATUS_OK != status);

    status = hal_sdio_set_block_size(WIFI_SDIO_PORT, HAL_SDIO_FUNCTION_1, WIFI_BLK_SIZE);
    printf("hal_sdio_set_block_size status = %d \r\n", status);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio set block size error. \r\n");
    }

    status = hal_sdio_get_block_size(WIFI_SDIO_PORT, HAL_SDIO_FUNCTION_1, &blk_size);
    printf("hal_sdio_get_block_size status = %d \r\n", status);
    if (HAL_SDIO_STATUS_OK != status) {
        printf("sdio get block size error. \r\n");
    } else {
        printf("sdio get block size, block size is %d. \r\n", (int)blk_size);
    }

    if (false == sdio_hif_get_driver_own()) {
        printf("1 get driver own fail. \r\n");
        return false;
    } else {
        printf("1 get driver own success. \r\n");
    }


    reg_value = 0;
    h2d_receive_mailbox(&reg_value);
    if (0x53444254 != reg_value) {
        printf("2 error, device response is not 0x87654321!,reg_value = %x \r\n", (unsigned int)reg_value);
        return false;
    } else {
        printf("2 receive mailbox success \r\n");
    }

    reg_value = 0x534254FF;
    h2d_send_mailbox(reg_value);
    printf("3 send mailbox success \r\n");

    /*host receive data from slave test. Get Bin */
    sdio_receive_pkt((uint8_t *)(&sdio_rx_buffer));
    get_bin = (xboot_cmd_getbin_t *)(&sdio_rx_buffer);
    if ((get_bin->m_magic == 0x444D434D) && (get_bin->m_msg_id == GET_BIN)) {
        printf("6_1 device req GET_BIN success! \r\n");

        block_size = get_bin->m_reserve[0];
        if (total_length > get_bin->m_len) {
            printf("6_2 size mismatch. Allowable size = %d \r\n", (int)get_bin->m_len);
            return false;
        }

        memset(&brom_tx_test, 0, 2048);
        cmd = (xboot_cmd_status_t *)(&brom_tx_test);
        cmd->m_magic = 0x4D4B4341;
        cmd->m_msg_id = ACK_GET_BIN;
        cmd->m_status = 0;
        cmd->m_reserve[0] = total_length;
        cmd->m_reserve[1] = MAX_BLOCK_SIZE;
        sdio_send_pkt(0, (uint8_t *)(&brom_tx_test), sizeof(xboot_cmd_status_t));

        printf("6_2 allowable size = %d, block size = %d \r\n", (int)get_bin->m_len, (int)block_size);

        printf("6_3 device req ACK_GET_BIN! \r\n");
    } else {
        printf("6 device req GET_BIN error! magic = %x, ID = %x \r\n", (unsigned int)get_bin->m_magic, get_bin->m_msg_id);
        return false;
    }

    // Send binary
    {
        uint32_t len_segment;
        uint32_t *pBuf32 = (uint32_t *)brom_tx_test;
        uint8_t *pBuf8 = address;

        for (; total_length > 0;) {
            memcpy(brom_tx_test, pBuf8, MAX_BLOCK_SIZE);
            pBuf8 += MAX_BLOCK_SIZE;

            pBuf32 = (uint32_t *)brom_tx_test;

            for (i = 0; i < MAX_BLOCK_SIZE / 4; i++) {
                checksum ^= *pBuf32;
                pBuf32++;
            }

            if (total_length >= MAX_BLOCK_SIZE) {
                len_segment = MAX_BLOCK_SIZE;
            } else {
                len_segment = total_length;
            }

            sdio_send_pkt(0, (uint8_t *)(&brom_tx_test), len_segment);

            total_length -= len_segment;
            printf("7 Send bin! Remain %d \r\n", (unsigned int)total_length);
        }
        printf("7 Send bin complete! checksum = %x \r\n", (unsigned int)checksum);
    }

    /*host receive data from slave test. Load END */
    sdio_receive_pkt((uint8_t *)(&sdio_rx_buffer));
    cmd = (xboot_cmd_status_t *)(&sdio_rx_buffer);
    if ((cmd->m_magic == 0x444D434D) && (cmd->m_msg_id == BIN_LOAD_END)) {
        printf("8_1 device req BIN_LOAD_BIN success! \r\n");
        printf("8_1 Received Checksum = %x \r\n", (unsigned int)cmd->m_reserve[0]);
        checksum_received = (unsigned int)cmd->m_reserve[0];
        memset(&brom_tx_test, 0, 2048);
        cmd = (xboot_cmd_status_t *)(&brom_tx_test);
        cmd->m_magic = 0x4D4B4341;
        cmd->m_msg_id = ACK_BIN_LOAD_END;
        cmd->m_status = 0;
        cmd->m_reserve[0] = 0;
        sdio_send_pkt(0, (uint8_t *)(&brom_tx_test), sizeof(xboot_cmd_status_t));
        printf("8_2 device req ACK_BIN_LOAD_END! \r\n");
    } else {
        printf("8 device req BIN_LOAD_END error! %d \r\n", cmd->m_msg_id);
        return false;
    }

    // Compare the checksum
    if (checksum == checksum_received) {
        return true;
    } else {
        return false;
    }
}

