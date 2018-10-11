/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ffconf.h"
#include "diskio.h"    /* FatFs lower layer API */
#include <stdbool.h>



#ifdef MTK_FATFS_ON_SERIAL_NAND
#include "spi_nand_fdm.h"
extern DSTATUS Drive1_Stat;	/* Physical drive status */


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS serial_nand_disk_initialize(
    BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    uint32_t retry = 0;
    int32_t result;
    if (pdrv != DRV_CFC) {
        return RES_PARERR;    /* Supports only drive 0 */
    }
    
    while (retry < 3) {
        result = spi_nand_fdm_DAL_init();   //return sectore size

        if (0 == result) {
            Drive1_Stat &= ~STA_NOINIT;  /* Clear STA_NOINIT flag */
            return Drive1_Stat;
        } else {
            retry++;
        }
    }

    return Drive1_Stat;


}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT serial_nand_disk_read(
    BYTE pdrv,		/* Physical drive nmuber to identify the drive */
    BYTE *buff,		/* Data buffer to store read data */
    DWORD sector,	/* Sector address in LBA */
    UINT count		/* Number of sectors to read */
)
{
    uint32_t retry = 0;
    int32_t status;
    if ((pdrv != DRV_CFC) || !count) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Drive1_Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    while (retry < 3) {
        status = spi_nand_fdm_ReadSectors(sector, count, (void *)buff);
        if (0 == status) {
            return RES_OK;
        } else {
            status = spi_nand_fdm_DAL_init();
            if (0 == status) {
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

DRESULT serial_nand_disk_write(
    BYTE pdrv,			/* Physical drive nmuber to identify the drive */
    const BYTE *buff,	/* Data to be written */
    DWORD sector,		/* Sector address in LBA */
    UINT count			/* Number of sectors to write */
)
{
    uint32_t retry = 0;
    int32_t status;

    if ((pdrv != DRV_CFC) || !count) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Drive1_Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    while (retry < 3) {
        status = spi_nand_fdm_WriteSectors(sector, count, (void *)buff);
        if (0 == status) {
            return RES_OK;
        } else {
            status = spi_nand_fdm_DAL_init();
            if (0 == status) {
                retry++;
            } else {
                return RES_ERROR;
            }
        }
    }

    return RES_ERROR;
}


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/


DRESULT serial_nand_disk_ioctl(
    BYTE pdrv,		/* Physical drive nmuber (0..) */
    BYTE cmd,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT result;
    int32_t status;
    DEVICE_INFO_T device_info;

    if (pdrv != DRV_CFC) {
        return RES_PARERR;    /* Check parameter */
    }
    if (Drive1_Stat & STA_NOINIT) {
        return RES_NOTRDY;    /* Check if drive is ready */
    }

    result = RES_ERROR;


    switch (cmd) {
        case CTRL_SYNC :		/* write process has been completed */
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT :
            status = spi_nand_GetDiskGeometry(&device_info);
            if (status == 0) {
                *(uint32_t *)buff = device_info.sectors;   /* unit is byte */
                result = RES_OK;
            }

            break;

        case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
        	//erase 256k
        	status = spi_nand_GetDiskGeometry(&device_info);
        	 
            
            //status = hal_sd_get_erase_sector_size(fatfs_sd_port, (uint32_t *)buff);
            if (status == 0) {
                *(uint32_t *)buff = device_info.block_size;
                result = RES_OK;
            }
			*(uint32_t *)buff = 256 * 1024;  //default 256K block size
            result = RES_OK;

            break;

        case CTRL_TRIM :	/* Erase a block of sectors (used when _USE_TRIM == 1) */
#if 0
            temp_erase_pointer = buff;
            erase_start = temp_erase_pointer[0];
            erase_end = temp_erase_pointer[1];
            ///TODO::: 
            //status = hal_sd_erase_sectors(fatfs_sd_port, erase_start, (erase_end - erase_start));
            if (status == 0) {
                result = RES_OK;
            }
#endif
            //has no this api, do nothing
            result = RES_OK;
            break;

        default:
            result = RES_PARERR;
    }
    return result;
}

#endif /*MTK_FATFS_ON_SPI_SERIAL_NAND*/


