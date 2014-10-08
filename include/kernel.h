#ifndef _KERNEL_H
#define _KERNEL_H

#include <kernel/types.h>
#include <kernel/compiler.h>
#include <kernel/macro.h>

#include <stdarg.h>


#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)

typedef __builtin_va_list va_list;

extern
int ksnprintf(char* buf, size_t size, const char* fmt, ...)
    __printf(3,4);
extern
int kvsnprintf(char* buf, size_t size, const char* fmt, va_list args)
    __printf(3,0);

void* __must_check kmalloc(size_t size);

void kfree(void* mem);

#endif /* _KERNEL_H */
