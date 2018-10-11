#ifndef __SDIO_HAL_H__
#define __SDIO_HAL_H__

#include "sdio_dev.h"

typedef enum {

    HDRV_SDBUS_BUSWIDTH_1BIT = 0x00,

    HDRV_SDBUS_BUSWIDTH_4BIT = 0x02,

    HDRV_SDBUS_BUSWIDTH_BITMASK = 0x03,

} HDRV_SDBUS_BUSWIDTH, *PHDRV_SDBUS_BUSWIDTH ;



typedef enum {

    HDRV_SDBUS_FUNCTION_NUMBER = 0,

    HDRV_SDBUS_FUNCTION_TYPE,

    HDRV_SDBUS_BUS_DRIVER_VERSION,

    HDRV_SDBUS_BUS_WIDTH,

    HDRV_SDBUS_BUS_CLOCK,

    HDRV_SDBUS_BUS_INTERFACE_CONTROL,

    HDRV_SDBUS_HOST_BLOCK_LENGTH,

    HDRV_SDBUS_FUNCTION_BLOCK_LENGTH,

    HDRV_SDBUS_FN0_BLOCK_LENGTH,

    HDRV_SDBUS_FUNCTION_INT_ENABLE

} HDRV_SDBUS_PROPERTY, *PHDRV_SDBUS_PROPERTY;



#if 1

int sdio_func0_wr(unsigned int u4Register, unsigned char *pValue,  unsigned int Length);

int sdio_func0_rd(unsigned int u4Register, unsigned char *pValue,  unsigned int Length);



int sdio_func1_wr(unsigned int u4Register, void *pBuffer,  unsigned int Length);

int sdio_func1_rd(unsigned int u4Register, void *pBuffer,  unsigned int Length);



int sdio_func2_wr(unsigned int u4Register, void *pBuffer, unsigned int Length);

int sdio_func2_rd(unsigned int u4Register, void *pBuffer,  unsigned int Length);



int sdio_property_set(HDRV_SDBUS_PROPERTY PropFunc, unsigned char *pData,  unsigned int size);

int sdio_property_get(HDRV_SDBUS_PROPERTY PropFunc, unsigned char *pData,  unsigned int size);


hal_sdio_status_t sdio_init(void);

#if 0
int sdio_open_device(struct sdio_func *sdiofunc);

int sdio_close_device(struct sdio_func *sdiofunc);



int sdio_setup_irq(struct sdio_func *sdiofunc, sdio_irq_handler_t *handler);

int sdio_remove_irq(struct sdio_func *sdiofunc);
#endif
#endif



#endif


