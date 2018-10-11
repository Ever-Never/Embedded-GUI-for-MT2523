#include "resource.h"

unsigned int
res_get_offset(unsigned int res_index)
{
    unsigned int *header = (unsigned int *)RES_BASE_ADDR;
    return header[res_index];
}

unsigned int
res_get_length(unsigned int res_index)
{
    return (res_get_offset(res_index + 1) - res_get_offset(res_index));
}

const unsigned char *
res_get(unsigned int res_index)
{
    const unsigned char *addr = (const unsigned char *)RES_BASE_ADDR;
    addr += res_get_offset(res_index);
    return addr;
}


