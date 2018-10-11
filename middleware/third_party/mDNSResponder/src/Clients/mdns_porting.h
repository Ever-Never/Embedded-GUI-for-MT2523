#ifndef __MDNS_PORTING_H__
#define __MDNS_PORTING_H__

#ifdef __cplusplus
extern "C" {
#endif

// For KEIL/IAR IDE
#if (defined(__CC_ARM) || defined(__ICCARM__))
#ifndef ssize_t
typedef long int ssize_t;
#endif

#ifndef ssize_t pid_t
typedef int pid_t;
#endif

#ifndef __u_char_defined
typedef unsigned char   u_char;
#define __u_char_defined
#endif
#ifndef __u_short_defined
typedef unsigned short  u_short;
#define __u_short_defined
#endif
#ifndef __u_int_defined
typedef unsigned int    u_int;
#define __u_int_defined
#endif
#ifndef __u_long_defined
typedef unsigned long   u_long;
#define __u_long_defined
#endif

#elif defined(__GNUC__)
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MDNS_PORTING_H__ */

