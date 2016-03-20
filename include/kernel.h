#ifndef _KERNEL_H
#define _KERNEL_H

#include <nubbin/kernel/types.h>
#include <nubbin/kernel/compiler.h>
#include <nubbin/kernel/macro.h>

#include <stdarg.h>

extern unsigned short* video;

#define va_start(v, l) __builtin_va_start(v, l)
#define va_arg(v, l) __builtin_va_arg(v, l)
#define va_end(v) __builtin_va_end(v)
#define va_copy(d, s) __builtin_va_copy(d, s)

typedef __builtin_va_list va_list;

void* __must_check kmalloc(size_t size);

void kfree(void* mem);

#endif /* _KERNEL_H */
