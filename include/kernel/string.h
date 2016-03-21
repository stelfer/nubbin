/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _STRING_H
#define _STRING_H

const char* hexify(unsigned long x, char* fmt, unsigned long len);

void* memcpy(void* dst, void const* src, unsigned long len);

#endif /* _STRING_H */
