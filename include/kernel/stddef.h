// Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt

#ifndef _STDDEF_H
#define _STDDEF_H

#include <nubbin/kernel/types.h>

#define NULL 0

#define offsetof(st, m) __builtin_offsetof(st, m)

typedef __PTRDIFF_TYPE__ ptrdiff_t;

typedef __WCHAR_TYPE__ wchar_t;

#endif /* _STDDEF_H */
