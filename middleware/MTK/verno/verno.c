#include "verno.h"

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)

#ifndef __ICCARM__
__attribute__((section(".tcmRO"))) __attribute__((used)) const 
#else
_Pragma("location=\".tcmRO\"") __root const
#endif

char build_date_time_str[] = __DATE__ " " __TIME__;
char sw_verno_str[] = MTK_FW_VERSION;

#endif

#if (PRODUCT_VERSION == 2523) || (PRODUCT_VERSION == 2533)

#ifndef __ICCARM__
__attribute__((section(".ram_rodata"))) __attribute__((used)) const
#else
_Pragma("location=\".ram_rodata\"") __root const
#endif

char build_date_time_str[] = __DATE__ " " __TIME__;
char sw_verno_str[] = MTK_FW_VERSION;

#endif
