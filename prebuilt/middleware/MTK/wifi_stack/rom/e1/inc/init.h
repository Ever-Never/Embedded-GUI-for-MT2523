#ifndef _INIT_H
#define _INIT_H
#include "sys_hal_types_ext.h"
#include "gl_typedef_rom.h"
#ifndef WIFI_BUILD_RAM
#define DOT11_N_SUPPORT 1 // Don't defined in makefile, otherwise, other FW files included ra_ctrl.h will conflict!
#endif

#define PACKED  __attribute__ ((packed))
#define GNU_PACKED PACKED
#define __init		__attribute__ ((__section__(".init.text")))
#define __initdata	__attribute__ ((__section__(".init.data")))
#define __exitdata	__attribute__ ((__section__(".exit.data")))
#define __exit_call	__attribute_used__ __attribute__ ((__section__(".exitcall.exit")))

#define __romtext	__attribute__ ((__section__(".romsection.text")))
#define __romdata	__attribute__ ((__section__(".romsection.data"))) const
#define __rom_x_data  __attribute__ ((__section__(".romaccess.data")))
#define __rom_bss_data  __attribute__ ((__section__(".romaccess.bss")))
#define __rom_ex_api __attribute__ ((__section__(".romsection.ex.api, \"a\"!")))

typedef unsigned long long UINT64;

struct UINT64 { UINT64 x __attribute__((packed)); };
struct UINT32 { UINT32 x __attribute__((packed)); };
struct UINT16 { UINT16 x __attribute__((packed)); };

static inline UINT64 __uldq(const UINT64 *addr)
{
	const struct UINT64 *ptr = (const struct UINT64 *) addr;
	return ptr->x;
}
static inline UINT32 __uldl(const UINT32 *addr)
{
	const struct UINT32 *ptr = (const struct UINT32 *) addr;
	return ptr->x;
}
static inline void __ustq(UINT64 val, UINT64 *addr)
{
	struct UINT64 *ptr = (struct UINT64 *) addr;
	ptr->x = val;
}
static inline void __ustl(UINT32 val, UINT32 *addr)
{
	struct UINT32 *ptr = (struct UINT32 *) addr;
	ptr->x = val;
}

static inline void __ustw(UINT16 val, UINT16 *addr)
{
	struct UINT16 *ptr = (struct UINT16 *) addr;
	ptr->x = val;
}

static inline USHORT __uldw(const UINT16 *addr)
{
	const struct UINT16 *ptr = (const struct UINT16 *) addr;
	return ptr->x;
}
#define __get_unaligned(ptr, size) ({		\
	const void *__gu_p = ptr;		\
	__typeof__(*(ptr)) val;			\
	switch (size) {				\
	case 1:					\
		val = *(const UINT8 *)__gu_p;	\
		break;				\
	case 2:					\
		val = __uldw(__gu_p);		\
		break;				\
	case 4:					\
		val = __uldl(__gu_p);		\
		break;				\
	case 8:					\
		val = __uldq(__gu_p);		\
		break;				\
	default:				\
		ASSERT(0);	\
	};					\
	val;					\
})
/* We don't want strings.h stuff being user by user stuff by accident */
#define BUG() { iot_rom_printf("kernel BUG at %s:%d!\n", __FILE__, __LINE__); }
#define BUG_ON(condition) do { if ((condition)!=0) BUG(); } while(0)
#define htons(x) OS_HTONS(x) 
#define ntohs(x) OS_NTOHS(x)
#define htonl(x) OS_HTONL(x)
#define ntohl(x) OS_NTOHL(x)

#define cpu_to_le64(x) (x)
#define le64_to_cpu(x) (x)
#define cpu_to_le32(x) (x)
#define le32_to_cpu(x) (x)
#define cpu_to_le16(x) (x)
#define le16_to_cpu(x) (x)

#define get_unaligned(ptr) \
__get_unaligned((ptr), sizeof(*(ptr)))
#define put_unaligned(x,ptr) (*(ptr) = x)

#define KSEG1ADDR(a)    (a)
#define CPHYSADDR(a)    (a)

//#define DLM_BASE     0x100000    
//#define DLM_MAX      0x1217FF
#define PROFILE_SIZE    0x800 // 2KB
//#define PROFILE_START (DLM_MAX - PROFILE_SIZE) 	

#endif /* _LINUX_INIT_H */
