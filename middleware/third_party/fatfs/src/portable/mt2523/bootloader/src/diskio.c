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
#include "bl_sd.h"
#include "stdint.h"


/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define USB		2	/* Example: Map USB MSD to physical drive 2 */

static volatile DSTATUS Stat = STA_NOINIT;	/* Physical drive status */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{

    if (pdrv) {
        return STA_NOINIT;    /* Supports only drive 0 */
    }

    return Stat;	/* Return disk status */
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    bl_sd_status_t result;
    bl_sd_config_t msdc0_cfg = { BL_SD_BUS_WIDTH_4, 13000};

    if (pdrv) {
        return STA_NOINIT;    /* Supports only drive 0 */
    }

    result = bl_sd_init(BL_SD_PORT_0, &(msdc0_cfg));

    if (BL_SD_STATUS_OK == result) {
        Stat &= ~STA_NOINIT;	/* Clear STA_NOINIT flag */
    }

    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,		/* Physical drive nmuber to identify the drive */
    BYTE *buff,		/* Data buffer to store read data */
    DWORD sector,	        /* Sector address in LBA */
    UINT count		/* Number of sectors to read */
)
{
    bl_sd_status_t status;
    if (pdrv || !count) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    status = bl_sd_read_blocks(BL_SD_PORT_0, (uint32_t *)buff, sector, count);
    if (BL_SD_STATUS_OK == status) {
        return RES_OK;
    } else {
        return RES_ERROR;
    }

}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
    BYTE pdrv,			/* Physical drive nmuber to identify the drive */
    const BYTE *buff,	        /* Data to be written */
    DWORD sector,		        /* Sector address in LBA */
    UINT count			/* Number of sectors to write */
)
{
    bl_sd_status_t status;

    if (pdrv || !count) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    status = bl_sd_write_blocks(BL_SD_PORT_0, (uint32_t *)buff, sector, count);
    if (BL_SD_STATUS_OK == status) {
        return RES_OK;
    } else {
        return RES_ERROR;
    }


}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
    BYTE pdrv,		/* Physical drive nmuber (0..) */
    BYTE cmd,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT result;
    uint64_t *temp_erase_pointer, erase_start, erase_end, sector_count;
    bl_sd_status_t status;

    if (pdrv) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    result = RES_ERROR;


    switch (cmd) {
        case CTRL_SYNC :		/* write process has been completed */
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT :          /* Get drive capacity in unit of sector (DWORD) */
            status = bl_sd_get_capacity(BL_SD_PORT_0, &sector_count);
            if (status == BL_SD_STATUS_OK) {
                *(uint32_t *)buff = (uint32_t)(sector_count / 512);   /* unit is byte */
                result = RES_OK;
            }

            break;

        case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
            status = bl_sd_get_erase_sector_size(BL_SD_PORT_0, (uint32_t *)buff);
            if (status == BL_SD_STATUS_OK) {
                result = RES_OK;
            }

            break;

        case CTRL_TRIM :	/* Erase a block of sectors (used when _USE_TRIM == 1) */
            temp_erase_pointer = buff;
            erase_start = temp_erase_pointer[0];
            erase_end = temp_erase_pointer[1];
            status = bl_sd_erase_sectors(BL_SD_PORT_0, erase_start, (erase_end - erase_start));
            if (status == BL_SD_STATUS_OK) {
                result = RES_OK;
            }

            break;


        /* Following commands are not used by FatFs module */
        case MMC_GET_TYPE : 	/* Get MMC/SDC type (BYTE) */
            status = bl_sd_get_card_type(BL_SD_PORT_0, buff);
            if (status == BL_SD_STATUS_OK) {
                result = RES_OK;
            }

            break;

        case MMC_GET_CSD :		/* Read CSD (16 bytes) */
            status = bl_sd_get_csd(BL_SD_PORT_0, buff);
            if (status == BL_SD_STATUS_OK) {
                result = RES_OK;
            }
            break;


        default:
            result = RES_PARERR;
    }
    return result;
}


#endif
