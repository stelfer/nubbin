#ifndef _KERNEL_H
#define _KERNEL_H

#include <stdarg.h>


#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;


#include <types.h>



#define STRINGIFY(s) __STR(s)
#define __STR(s) #s

#define __must_check __attribute__((warn_unused_result))

int __must_check vsnprintf(char* buf, size_t size, const char* fmt, va_list args);

void* __must_check kmalloc(size_t size);

void kfree(void* mem);

#endif /* _KERNEL_H */
