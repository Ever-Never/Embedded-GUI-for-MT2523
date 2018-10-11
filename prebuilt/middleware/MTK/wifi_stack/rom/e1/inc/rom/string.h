#ifndef __STRING_H__
#define __STRING_H__

extern void *(*rom_memcpy)(void *dest, const void *src, size_t n);
extern void *(*rom_memmove)(void *dest, const void *src, size_t n);
extern void *(*rom_memset)(void *s, int c, size_t n);

extern char *rom_strcat(char *dest, const char *src);
//extern char *strncat(char *dest, const char *src, size_t n);

//extern int strcasecmp(const char *s1, const char *s2);
//extern int strncasecmp(const char *s1, const char *s2, size_t n);

extern int (*rom_strcmp)(const char *s1, const char *s2);
extern int rom_strncmp(const char *s1, const char *s2, size_t n);

extern char *rom_strcpy(char *dest, const char *src);
extern char *rom_strncpy(char *dest, const char *src, size_t n);

//extern char *strdup(const char *s);
//extern char *strndup(const char *s, size_t n);

extern size_t (*rom_strlen)(const char *s);
//extern size_t strnlen(const char *s, size_t maxlen);

extern char *rom_strstr(const char *haystack, const char *needle);
//extern char *strupr(char *str);

//extern size_t strspn(const char *s, const char *accept);
extern char * rom_strpbrk(const char * cs,const char * ct);
extern int (*rom_memcmp)(const void * cs, const void * ct, size_t count);

#endif /* __STRING_H__ */
