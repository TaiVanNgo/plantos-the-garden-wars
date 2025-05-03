#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _VA_LIST_DEFINED
#define _VA_LIST_DEFINED
typedef __builtin_va_list va_list;
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)
#endif

void delay_ms(unsigned int ms);
int strcmp(const char *s1, const char *s2);
int str_to_int(const char *str);
int strncmp(const char *s1, const char *s2, unsigned int n);
unsigned int strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, unsigned int n);
char *strchr(const char *s, int c);
int sprintf(char *str, const char *format, ...);
int str_to_int(const char *str);