#ifndef _STRTOX_H
#define _STRTOX_H

#include <kernel/compiler.h>


extern int __must_check strtoul(const char *s, unsigned int base, unsigned long *res);
extern int __must_check strtol(const char *s, unsigned int base, long *res);

#define STRTOX_OVERFLOW (1 << (sizeof(unsigned long) - 1))

#endif /* _STRTOX_H */
