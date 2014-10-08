#ifndef _STRTOX_H
#define _STRTOX_H

extern int __attribute__((warn_unused_result)) strtoul(const char *s, unsigned int base, unsigned long *res);
extern int __attribute__((warn_unused_result)) strtol(const char *s, unsigned int base, long *res);

#define STRTOX_OVERFLOW (1 << (sizeof(unsigned long) - 1))

#endif /* _STRTOX_H */
