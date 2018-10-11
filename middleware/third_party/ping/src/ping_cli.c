
#include <os.h>
#include <toi.h>

#include "ping.h"

#include "ping_cli.h"


uint8_t ping_cli_handler(uint8_t len, char *param[])
{
    uint32_t count;
    uint32_t pktsz;
    uint8_t  type;

    if (len < 1) {
        return 1;
    } else {
        count = 3;
        pktsz = 64;
    }

    if (len > 1) {
        count = toi(param[1], &type);
        if (type == TOI_ERR) {
            return 2;
        }
    }

    if (len > 2) {
        pktsz = toi(param[2], &type);
        if (type == TOI_ERR) {
            return 2;
        }
    }

    ping_init(count, param[0], os_strlen(param[0]), pktsz);

    return 0;
}

