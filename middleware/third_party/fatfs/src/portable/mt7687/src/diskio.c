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

#ifdef MTK_FATFS_ON_SERIAL_NAND
#include "diskio_snand.h"
#endif

//#if defined(MTK_FATFS_ON_SPI_SD) || defined(HAL_SD_MODULE_ENABLED)
#include "diskio_sd.h"
//#endif

DSTATUS Drive0_Stat = STA_NOINIT;	/* Physical drive status */
DSTATUS Drive1_Stat = STA_NOINIT;	/* Physical drive status */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
    BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    if (pdrv) {
        return Drive1_Stat;
    } else {
        return Drive0_Stat;
    }
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
    BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    if(!pdrv) {
        return sd_disk_initialize(pdrv);
    } else {
#ifdef MTK_FATFS_ON_SERIAL_NAND
        return serial_nand_disk_initialize(pdrv);
#else
        return RES_PARERR;
#endif
   }


}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,		/* Physical drive nmuber to identify the drive */
    BYTE *buff,		/* Data buffer to store read data */
    DWORD sector,	/* Sector address in LBA */
    UINT count		/* Number of sectors to read */
)
{
    if(!pdrv) {
        return sd_disk_read(pdrv, buff, sector, count);
    } else {
#ifdef MTK_FATFS_ON_SERIAL_NAND
        return serial_nand_disk_read(pdrv, buff, sector, count);
#else
        return RES_PARERR;
#endif
   }
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write(
    BYTE pdrv,			/* Physical drive nmuber to identify the drive */
    const BYTE *buff,	/* Data to be written */
    DWORD sector,		/* Sector address in LBA */
    UINT count			/* Number of sectors to write */
)
{
    if(!pdrv) {
        return sd_disk_write(pdrv, buff, sector, count);
    } else {
#ifdef MTK_FATFS_ON_SERIAL_NAND
        return serial_nand_disk_write(pdrv, buff, sector, count);
#else
        return RES_PARERR;
#endif
   }
}
#endif



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl(
    BYTE pdrv,		/* Physical drive nmuber (0..) */
    BYTE cmd,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    if(!pdrv) {
        return sd_disk_ioctl(pdrv, cmd, buff);
    } else {
#ifdef MTK_FATFS_ON_SERIAL_NAND
        return serial_nand_disk_ioctl(pdrv, cmd, buff);
#else
        return RES_PARERR;
#endif
   }
}
#endif




#if !_FS_READONLY && !_FS_NORTC
DWORD get_fattime(void)
{
    // get the current time

    return 0; //return the current time instead of 0 if current time is got
}
#endif


