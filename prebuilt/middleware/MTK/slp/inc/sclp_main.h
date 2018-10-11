#ifndef __SCLP_MAIN_H__
#define __SCLP_MAIN_H__

#include "slp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*sclp_callback_func)(sclp_cell_location_list* result);
//-1 means failure
int sclp_init(sclp_callback_func callback);

#ifdef __cplusplus
}
#endif

#endif
