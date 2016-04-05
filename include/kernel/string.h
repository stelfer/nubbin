/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _STRING_H
#define _STRING_H

#include <nubbin/kernel/stddef.h>

const char* hexify(unsigned long x, char* fmt, unsigned long len);

void* memcpy(void* dst, void const* src, unsigned long len);

int memcmp(const void* s1, const void* s2, size_t len);

void bzero(void* dst, size_t len);

size_t strlen(const char* p);

#endif /* _STRING_H */
