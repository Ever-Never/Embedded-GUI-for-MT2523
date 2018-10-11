/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ffconf.h"
#include "diskio.h"		/* FatFs lower layer API */
#include <stdbool.h>
#include "hal.h"

extern DSTATUS Drive0_Stat;	/* Physical drive status */


#ifdef MTK_FATFS_ON_SPI_SD

#include "hal_pinmux_define.h"
#include "hal_gpio.h"
#include "hal_spi_master.h"
#include "hal_log.h"
#include "hal_gpt.h"

/* Definitions for MMC/SDC command */
#define CMD0    (0x40+0)    /* GO_IDLE_STATE */
#define CMD1    (0x40+1)    /* SEND_OP_COND */
#define CMD8    (0x40+8)    /* SEND_IF_COND */
#define CMD9    (0x40+9)    /* SEND_CSD */
#define CMD10    (0x40+10)    /* SEND_CID */
#define CMD12    (0x40+12)    /* STOP_TRANSMISSION */
#define CMD16    (0x40+16)    /* SET_BLOCKLEN */
#define CMD17    (0x40+17)    /* READ_SINGLE_BLOCK */
#define CMD18    (0x40+18)    /* READ_MULTIPLE_BLOCK */
#define CMD23    (0x40+23)    /* SET_BLOCK_COUNT */
#define CMD24    (0x40+24)    /* WRITE_BLOCK */
#define CMD25    (0x40+25)    /* WRITE_MULTIPLE_BLOCK */
#define CMD41    (0x40+41)    /* SEND_OP_COND (ACMD) */
#define CMD55    (0x40+55)    /* APP_CMD */
#define CMD58    (0x40+58)    /* READ_OCR */


/*SPIM port (Platform dependent)*/
#define SPIM_PORT HAL_SPI_MASTER_0

/* Port Controls  (Platform dependent) */
#define SPI_PINMUX_REG      (0x81023000 + 0x2c)
#define SPI_PINMUX_REG_0_3_VALUE      ((*(volatile uint32_t *)SPI_PINMUX_REG) & 0x0000000f)
#define SPI_PINMUX_REG_20_23_VALUE    (((*(volatile uint32_t *)SPI_PINMUX_REG) & 0x00f00000) >> 20)
#define SPI_USE_GPIO7_AS_CS           (HAL_GPIO_24_SPI_MOSI_M_CM4 == (SPI_PINMUX_REG_0_3_VALUE & HAL_GPIO_24_SPI_MOSI_M_CM4))
#define SPI_USE_GPIO32_AS_CS          (HAL_GPIO_29_SPI_MOSI_M_CM4 == (SPI_PINMUX_REG_20_23_VALUE & HAL_GPIO_29_SPI_MOSI_M_CM4))


static BYTE CardType;            /* b0:MMC, b1:SDC, b2:Block addressing */
static uint32_t spi_mosi_pin = 0;
static uint8_t spi_mosi_pin_func_mosi = 0;
static uint8_t spi_mosi_pin_func_gpio = 0;

void spi_pad_select(void)
{
    if (SPI_USE_GPIO7_AS_CS) {
        spi_mosi_pin = HAL_GPIO_24;
        spi_mosi_pin_func_mosi = HAL_GPIO_24_SPI_MOSI_M_CM4;
        spi_mosi_pin_func_gpio = HAL_GPIO_24_GPIO24;
    } else if (SPI_USE_GPIO32_AS_CS) {
        spi_mosi_pin = HAL_GPIO_29;
        spi_mosi_pin_func_mosi = HAL_GPIO_29_SPI_MOSI_M_CM4;
        spi_mosi_pin_func_gpio = HAL_GPIO_29_GPIO29;
    }

}

/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/
static void xmit_spi(BYTE dat)
{
    BYTE send_data;
    hal_spi_master_status_t status;

    send_data = dat;
    hal_pinmux_set_function((hal_gpio_pin_t)spi_mosi_pin, spi_mosi_pin_func_mosi);
    if (HAL_SPI_MASTER_STATUS_OK != (status = hal_spi_master_send_polling(SPIM_PORT, (uint8_t *)(&send_data), 1))) {
        log_hal_info("xmit_spi,SPI master send failed,status = %d \r\n", status);
    }
    hal_pinmux_set_function((hal_gpio_pin_t)spi_mosi_pin, spi_mosi_pin_func_gpio);
    hal_gpio_set_direction((hal_gpio_pin_t)spi_mosi_pin, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output((hal_gpio_pin_t)spi_mosi_pin, HAL_GPIO_DATA_HIGH);
}


/* Alternative macro to receive data fast */
static void xmit_spi_m(const BYTE *src, DWORD data_len)
{
    hal_spi_master_status_t status;

    hal_pinmux_set_function((hal_gpio_pin_t)spi_mosi_pin, spi_mosi_pin_func_mosi);
    if (HAL_SPI_MASTER_STATUS_OK != (status = hal_spi_master_send_polling(SPIM_PORT, (uint8_t *)src, data_len))) {
        log_hal_info("xmit_spi_m,SPI master send failed,status = %d \r\n", status);
    }
    hal_pinmux_set_function((hal_gpio_pin_t)spi_mosi_pin, spi_mosi_pin_func_gpio);
    hal_gpio_set_direction((hal_gpio_pin_t)spi_mosi_pin, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output((hal_gpio_pin_t)spi_mosi_pin, HAL_GPIO_DATA_HIGH);
}

/*Recive one byte once.*/
static BYTE rcvr_spi(void)
{
    BYTE send_data[2];
    BYTE receive_data;
    hal_spi_master_send_and_receive_config_t config;
    hal_spi_master_status_t status;

    config.send_data = send_data;
    config.send_length = 0;
    config.receive_buffer = &receive_data;
    config.receive_length = 1;

    if (HAL_SPI_MASTER_STATUS_OK != (status = hal_spi_master_send_and_receive_polling(SPIM_PORT, &config))) {
        log_hal_info("rcvr_spi,SPI master send and receive failed, status = %d \r\n", status);
    }

    return receive_data;
}

/*Recive mutil bytes once.*/
static void rcvr_spi_m(BYTE *dst, DWORD data_len)
{
    BYTE send_data;
    hal_spi_master_send_and_receive_config_t config;
    hal_spi_master_status_t status;

    config.send_data = &send_data;
    config.send_length = 0;
    config.receive_buffer = dst;
    config.receive_length = data_len;

    if (HAL_SPI_MASTER_STATUS_OK != (status = hal_spi_master_send_and_receive_polling(SPIM_PORT, &config))) {
        log_hal_info("rcvr_spi,SPI master send and receive failed,status = %d \r\n", status);
    }
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/
BYTE wait_ready(void)
{
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;
    BYTE ret = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &start_count);
    while ((0xFF != (ret = rcvr_spi())) && (count < 32768)) { /*wait max 1s.*/
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &end_count);
        hal_gpt_get_duration_count(start_count, end_count, &count);
    }

    if (count >= 32768) {
        log_hal_error("wait bus not busy timeout. \r\n");
    }

    return ret;
}

/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */
void power_on(void)
{
    /*LDO, GPIO, SPI INIT,CS */
    uint32_t frequency = 240000;
    hal_spi_master_config_t spi_config;
    hal_spi_master_status_t status;

    spi_pad_select();

    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = frequency;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE1;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY1;
    if (HAL_SPI_MASTER_STATUS_OK != (status = hal_spi_master_init(SPIM_PORT, &spi_config))) {
        log_hal_info("hal_spi_master_init fail, status = %d\n", status);
    }

}

bool set_frequency(DWORD frequency)
{
    hal_spi_master_config_t spi_config;
    hal_spi_master_status_t status;

    hal_spi_master_deinit(SPIM_PORT);
    hal_gpt_delay_ms(1);

    hal_pinmux_set_function((hal_gpio_pin_t)spi_mosi_pin, spi_mosi_pin_func_mosi);

    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = frequency;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE1;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY1;
    if (HAL_SPI_MASTER_STATUS_OK != (status = hal_spi_master_init(SPIM_PORT, &spi_config))) {
        log_hal_info("hal_spi_master_init fail, status = %d.\n", status);
        return FALSE;
    }

    /*Config MOSI pin default high.*/
    hal_pinmux_set_function((hal_gpio_pin_t)spi_mosi_pin, spi_mosi_pin_func_gpio);
    hal_gpio_set_direction((hal_gpio_pin_t)spi_mosi_pin, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output((hal_gpio_pin_t)spi_mosi_pin, HAL_GPIO_DATA_HIGH);

    return TRUE;
}


void power_off(void)
{
    Drive0_Stat |= STA_NOINIT;        /* Set STA_NOINIT */
}



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd(
    BYTE cmd,        /* Command byte */
    DWORD arg        /* Argument */
)
{
    BYTE res;
    BYTE send_data[6];
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;
    hal_spi_master_status_t status;

    if (wait_ready() != 0xFF) {
        return 0xFF;
    }

    send_data[0] = cmd;
    send_data[1] = (BYTE)(arg >> 24);
    send_data[2] = (BYTE)(arg >> 16);
    send_data[3] = (BYTE)(arg >> 8);
    send_data[4] = (BYTE)arg;
    send_data[5] = 0;
    if (cmd == CMD0) {
        send_data[5] = 0x95;    /* CRC for CMD0(0) */
    }
    if (cmd == CMD8) {
        send_data[5] = 0x87;    /* CRC for CMD8(0x1AA) */
    }

    hal_pinmux_set_function((hal_gpio_pin_t)spi_mosi_pin, spi_mosi_pin_func_mosi);

    if (HAL_SPI_MASTER_STATUS_OK != (status = hal_spi_master_send_polling(SPIM_PORT, send_data, 6))) {
        log_hal_info("xmit_spi,SPI master send failed,status = %d \r\n", status);
    }

    hal_pinmux_set_function((hal_gpio_pin_t)spi_mosi_pin, spi_mosi_pin_func_gpio);
    hal_gpio_set_direction((hal_gpio_pin_t)spi_mosi_pin, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output((hal_gpio_pin_t)spi_mosi_pin, HAL_GPIO_DATA_HIGH);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &start_count);
    do {
        res = rcvr_spi();
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &end_count);
        hal_gpt_get_duration_count(start_count, end_count, &count);
    } while ((res & 0x80) && (count < 3276)); /*max wait 100ms*/

    //log_hal_info("cmd is %x,cmd response is %x. \r\n", (cmd & 0x3f), res);

    if (count >= 3276) {
        log_hal_error("wait cmd response timeout. \r\n");
    }

    return res;            /* Return with the response value */
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS sd_disk_initialize(
    BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    BYTE n, ty, ocr[4];
    BYTE buff[20];
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    if (DRV_CARD != pdrv) {
        return STA_NOINIT;    /* Supports only single drive */
    }
    if (Drive0_Stat & STA_NODISK) {
        return Drive0_Stat;    /* No card in the socket */
    }

    power_on(); 						   /* Force socket power on */

    hal_gpt_delay_ms(5);

    /*Switch to GPIO mode and output high, because need config MOSI pin to high, but maybe MOSI output is low.*/
    hal_pinmux_set_function((hal_gpio_pin_t)spi_mosi_pin, spi_mosi_pin_func_gpio);
    hal_gpio_set_direction((hal_gpio_pin_t)spi_mosi_pin, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output((hal_gpio_pin_t)spi_mosi_pin, HAL_GPIO_DATA_HIGH);
    hal_gpt_delay_ms(5);

    rcvr_spi_m(buff, 20);	/* dummy read for generate more than 74 dummy clocks */

    ty = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &start_count);
    if (send_cmd(CMD0, 0) == 1) {			 /* Enter Idle state */
        if (send_cmd(CMD8, 0x1AA) == 1) {	 /* SDC Ver2+ */
            for (n = 0; n < 4; n++) {
                ocr[n] = rcvr_spi();
            }
            if (ocr[2] == 0x01 && ocr[3] == 0xAA) {    /* The card can work at vdd range of 2.7-3.6V */
                do {
                    if (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD41, 1UL << 30) == 0) {
                        break;    /* ACMD41 with HCS bit */
                    }
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &end_count);
                    hal_gpt_get_duration_count(start_count, end_count, &count);
                } while (count < 32768); /* Setting timeout to 1s.*/
                if ((count < 32768) && send_cmd(CMD58, 0) == 0) {	/* Check CCS bit */
                    for (n = 0; n < 4; n++) {
                        ocr[n] = rcvr_spi();
                    }
                    ty = (ocr[0] & 0x40) ? 6 : 2;
                }
            }
        } else {							/* SDC Ver1 or MMC */
            ty = (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD41, 0) <= 1) ? 2 : 1;	  /* SDC : MMC */
            do {
                if (ty == 2) {
                    if (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD41, 0) == 0) {
                        break;    /* ACMD41 */
                    }
                } else {
                    if (send_cmd(CMD1, 0) == 0) {
                        break;    /* CMD1 */
                    }
                }
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &end_count);
                hal_gpt_get_duration_count(start_count, end_count, &count);
            } while (count < 32768);
            if ((count < 32768) || send_cmd(CMD16, 512) != 0) { /* Select R/W block length */
                ty = 0;
            }
        }
    }
    CardType = ty;

    rcvr_spi(); 		   /* Idle (Release DO) */

    if (ty) {			 /* Initialization succeded */
        Drive0_Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */
        set_frequency(20000000); /*the max freqency is 60MHz*/
    } else {			/* Initialization failed */
        power_off();
    }

    return Drive0_Stat;
}


/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
BYTE rcvr_datablock(
    BYTE *buff,            /* Data buffer to store received data */
    UINT btr            /* Byte count (must be even number) */
)
{
    BYTE token;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &start_count);
    do {                            /* Wait for data packet in timeout of 100ms */
        token = rcvr_spi();
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &end_count);
        hal_gpt_get_duration_count(start_count, end_count, &count);
    } while ((token == 0xFF) && (count < 3276));

    if (count >= 3276) {
        log_hal_error("wait recive data token timeout. \r\n");
    }

    if (token != 0xFE) {
        log_hal_error("recive data token error.token = %x \r\n", token);
        return FALSE;    /* If not valid data token, retutn with error */
    }

    rcvr_spi_m(buff, btr); /* Receive the data block into buffer */

    rcvr_spi();                        /* Discard CRC */
    rcvr_spi();

    return TRUE;                    /* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT sd_disk_read(
    BYTE pdrv,		/* Physical drive nmuber to identify the drive */
    BYTE *buff,		/* Data buffer to store read data */
    DWORD sector,	/* Sector address in LBA */
    UINT count		/* Number of sectors to read */
)
{
    if ((DRV_CARD != pdrv) || !count) {
        return RES_PARERR;
    }
    if (Drive0_Stat & STA_NOINIT) {
        return RES_NOTRDY;
    }

    if (!(CardType & 4)) {
        sector *= 512;    /* Convert to byte address if needed */
    }

    if (count == 1) {    /* Single block read */
        if ((send_cmd(CMD17, sector) == 0)    /* READ_SINGLE_BLOCK */
                && rcvr_datablock(buff, 512)) {
            count = 0;
        }
    } else {              /* Multiple block read */
        if (send_cmd(CMD18, sector) == 0) {    /* READ_MULTIPLE_BLOCK */
            do {
                if (!rcvr_datablock(buff, 512)) {
                    break;
                }
                buff += 512;
            } while (--count);
            send_cmd(CMD12, 0);                /* STOP_TRANSMISSION */
        }
    }

    rcvr_spi();            /* Idle (Release DO) */

    return count ? RES_ERROR : RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
static
bool xmit_datablock(
    const BYTE *buff,    /* 512 byte data block to be transmitted */
    BYTE token            /* Data/Stop token */
)
{
    BYTE resp;


    if (wait_ready() != 0xFF) {
        return FALSE;
    }

    xmit_spi(token);                    /* Xmit data token */
    if (token != 0xFD) {    /* Is data token */
        xmit_spi_m(buff, 512);  /* Xmit the 512 byte data block to MMC */

        xmit_spi(0xFF);                    /* CRC (Dummy) */
        xmit_spi(0xFF);
        resp = rcvr_spi();                /* Reveive data response */
        if ((resp & 0x1F) != 0x05) {      /* If not accepted, return with error */
            return FALSE;
        }
    }

    return TRUE;
}
#endif /* _READONLY */


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT sd_disk_write(
    BYTE pdrv,			/* Physical drive nmuber to identify the drive */
    const BYTE *buff,	/* Data to be written */
    DWORD sector,		/* Sector address in LBA */
    UINT count			/* Number of sectors to write */
)
{


    if (DRV_CARD != pdrv || !count) {
        return RES_PARERR;
    }
    if (Drive0_Stat & STA_NOINIT) {
        return RES_NOTRDY;
    }
    if (Drive0_Stat & STA_PROTECT) {
        return RES_WRPRT;
    }

    if (!(CardType & 4)) {
        sector *= 512;    /* Convert to byte address if needed */
    }

    if (count == 1) {	 /* Single block write */
        if ((send_cmd(CMD24, sector) == 0)	  /* WRITE_BLOCK */
                && xmit_datablock(buff, 0xFE)) {
            count = 0;
        }
    } else {				 /* Multiple block write */
        if (CardType & 2) {
            send_cmd(CMD55, 0);
            send_cmd(CMD23, count);    /* ACMD23 */
        }
        if (send_cmd(CMD25, sector) == 0) {    /* WRITE_MULTIPLE_BLOCK */
            do {
                if (!xmit_datablock(buff, 0xFC)) {
                    break;
                }
                buff += 512;
            } while (--count);
            if (!xmit_datablock(0, 0xFD)) { /* STOP_TRAN token */
                count = 1;
            }
        }
    }

    rcvr_spi(); 		   /* Idle (Release DO) */

    return count ? RES_ERROR : RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT sd_disk_ioctl(
    BYTE pdrv,		/* Physical drive nmuber (0..) */
    BYTE cmd,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res;
    BYTE n, csd[16], *ptr = buff;
    WORD csize;


    if (DRV_CARD != pdrv) {
        return RES_PARERR;
    }

    res = RES_ERROR;

    if (cmd == CTRL_POWER) {
        switch (*ptr) {
            case 0: 	   /* Sub control code == 0 (POWER_OFF) */
                //if (chk_power())
                //	power_off();		/* Power off */
                res = RES_OK;
                break;
            case 1: 	   /* Sub control code == 1 (POWER_ON) */
                power_on(); 			   /* Power on */
                res = RES_OK;
                break;
            case 2: 	   /* Sub control code == 2 (POWER_GET) */
                //*(ptr+1) = (BYTE)chk_power();
                res = RES_OK;
                break;
            default :
                res = RES_PARERR;
        }
    } else {
        if (Drive0_Stat & STA_NOINIT) {
            return RES_NOTRDY;
        }

        switch (cmd) {
            case GET_SECTOR_COUNT :    /* Get number of sectors on the disk (DWORD) */
                if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
                    if ((csd[0] >> 6) == 1) {	 /* SDC ver 2.00 */
                        csize = csd[9] + ((WORD)csd[8] << 8) + 1;
                        *(DWORD *)buff = (DWORD)csize << 10;
                    } else {					/* MMC or SDC ver 1.XX */
                        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                        csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                        *(DWORD *)buff = (DWORD)csize << (n - 9);
                    }
                    res = RES_OK;
                }
                break;

            case GET_SECTOR_SIZE :	  /* Get sectors on the disk (WORD) */
                *(WORD *)buff = 512;
                res = RES_OK;
                break;

            case CTRL_SYNC :	/* Make sure that data has been written */
                if (wait_ready() == 0xFF) {
                    res = RES_OK;
                }
                break;

            case MMC_GET_CSD :	  /* Receive CSD as a data block (16 bytes) */
                if (send_cmd(CMD9, 0) == 0		  /* READ_CSD */
                        && rcvr_datablock(ptr, 16)) {
                    res = RES_OK;
                }
                break;

            case MMC_GET_CID :	  /* Receive CID as a data block (16 bytes) */
                if (send_cmd(CMD10, 0) == 0 	   /* READ_CID */
                        && rcvr_datablock(ptr, 16)) {
                    res = RES_OK;
                }
                break;

            case MMC_GET_OCR :	  /* Receive OCR as an R3 resp (4 bytes) */
                if (send_cmd(CMD58, 0) == 0) {	  /* READ_OCR */
                    for (n = 0; n < 4; n++) {
                        *ptr++ = rcvr_spi();
                    }
                    res = RES_OK;
                }

            case MMC_GET_TYPE :    /* Get card type flags (1 byte) */
                *ptr = CardType;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
        }

        rcvr_spi(); 		   /* Idle (Release DO) */
    }

    return res;
}

#endif

#elif defined(HAL_SD_MODULE_ENABLED)
#include "hal_msdc.h"
#include "hal_sd.h"
#include "hal_sd_define.h"
#include "msdc_custom_config.h"


hal_sd_config_t fatfs_sd_config = {HAL_SD_BUS_WIDTH_4, 45000};
hal_sd_port_t fatfs_sd_port = HAL_SD_PORT_0;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS sd_disk_initialize(
    BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    uint32_t retry = 0;
    hal_sd_status_t result;

    if (DRV_CARD != pdrv) {
        return RES_PARERR;    /* Supports only drive 0 */
    }

    fatfs_sd_port = (hal_sd_port_t)FS_USE_MSDC_PORT_NUMBER;
    fatfs_sd_config.bus_width = (hal_sd_bus_width_t)FS_MSDC_BUS_WIDTH;
    fatfs_sd_config.clock = FS_MSDC_BUS_CLOCK;

    while (retry < 3) {
        result = hal_sd_init(fatfs_sd_port, &fatfs_sd_config);

        if (HAL_SD_STATUS_OK == result) {
            Drive0_Stat &= ~STA_NOINIT;	/* Clear STA_NOINIT flag */
            return Drive0_Stat;
        } else {
            retry++;
        }
    }

    return Drive0_Stat;


}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT sd_disk_read(
    BYTE pdrv,		/* Physical drive nmuber to identify the drive */
    BYTE *buff,		/* Data buffer to store read data */
    DWORD sector,	/* Sector address in LBA */
    UINT count		/* Number of sectors to read */
)
{
    uint32_t retry = 0;
    hal_sd_status_t status;
    if (DRV_CARD != pdrv || !count) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Drive0_Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    while (retry < 3) {
        status = hal_sd_read_blocks(fatfs_sd_port, (uint32_t *)buff, sector, count);
        if (HAL_SD_STATUS_OK == status) {
            return RES_OK;
        } else {
            status = hal_sd_init(fatfs_sd_port, &fatfs_sd_config);
            if (HAL_SD_STATUS_OK == status) {
                retry++;
            } else {
                return RES_ERROR;
            }
        }
    }

    return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT sd_disk_write(
    BYTE pdrv,			/* Physical drive nmuber to identify the drive */
    const BYTE *buff,	/* Data to be written */
    DWORD sector,		/* Sector address in LBA */
    UINT count			/* Number of sectors to write */
)
{

    uint32_t retry = 0;
    hal_sd_status_t status;

    if ((DRV_CARD != pdrv) || !count) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Drive0_Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    while (retry < 3) {
        status = hal_sd_write_blocks(fatfs_sd_port, (uint32_t *)buff, sector, count);
        if (HAL_SD_STATUS_OK == status) {
            return RES_OK;
        } else {
            status = hal_sd_init(fatfs_sd_port, &fatfs_sd_config);
            if (HAL_SD_STATUS_OK == status) {
                retry++;
            } else {
                return RES_ERROR;
            }
        }
    }

    return RES_ERROR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT sd_disk_ioctl(
    BYTE pdrv,		/* Physical drive nmuber (0..) */
    BYTE cmd,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT result;
    DWORD *temp_erase_pointer, erase_start, erase_end;
    hal_sd_status_t status;
    uint64_t sector_count;

    if (DRV_CARD != pdrv) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Drive0_Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    result = RES_ERROR;


    switch (cmd) {
        case CTRL_SYNC :		/* write process has been completed */
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT :          /* Get drive capacity in unit of sector (DWORD) */
            status = hal_sd_get_capacity(fatfs_sd_port, &sector_count);
            if (status == HAL_SD_STATUS_OK) {
                *(uint32_t *)buff = (uint32_t)(sector_count / 512);   /* unit is byte */
                result = RES_OK;
            }

            break;

        case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
            status = hal_sd_get_erase_sector_size(fatfs_sd_port, (uint32_t *)buff);
            if (status == HAL_SD_STATUS_OK) {
                result = RES_OK;
            }

            break;

        case CTRL_TRIM :	/* Erase a block of sectors (used when _USE_TRIM == 1) */
            temp_erase_pointer = buff;
            erase_start = temp_erase_pointer[0];
            erase_end = temp_erase_pointer[1];
            status = hal_sd_erase_sectors(fatfs_sd_port, erase_start, (erase_end - erase_start));
            if (status == HAL_SD_STATUS_OK) {
                result = RES_OK;
            }

            break;


        /* Following commands are not used by FatFs module */

        case MMC_GET_TYPE : 	/* Get MMC/SDC type (BYTE) */
            status = hal_sd_get_card_type(fatfs_sd_port, buff);
            if (status == HAL_SD_STATUS_OK) {
                result = RES_OK;
            }

            break;

        case MMC_GET_CSD :		/* Read CSD (16 bytes) */
            status = hal_sd_get_csd(fatfs_sd_port, buff);
            if (status == HAL_SD_STATUS_OK) {
                result = RES_OK;
            }
            break;

        default:
            result = RES_PARERR;
    }
    return result;
}
#endif
#endif

