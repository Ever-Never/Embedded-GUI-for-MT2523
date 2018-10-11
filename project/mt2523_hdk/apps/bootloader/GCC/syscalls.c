#include <sys/stat.h>
#include "bl_common.h"

int printf(__const char *__restrict __format, ...)
{
    bl_print(LOG_DEBUG, (char *)__format);
    return 0;
}
