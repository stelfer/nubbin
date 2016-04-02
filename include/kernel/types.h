// Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt

#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned char uint8_t;
typedef char int8_t;

typedef __UINT16_TYPE__ uint16_t;
typedef __INT16_TYPE__ int16_t;

typedef __UINT32_TYPE__ uint32_t;
typedef __INT32_TYPE__ int32_t;

typedef __UINT64_TYPE__ uint64_t;
typedef __INT64_TYPE__ int64_t;

typedef __SIZE_TYPE__ size_t;

#if __SIZEOF_SIZE_T__ == 4
typedef long ll_t;
typedef unsigned long llu_t;
#else
typedef long long ll_t;
typedef unsigned long long llu_t;
#endif

#if __SIZEOF_LONG__ == 4
#define ULONG_C __UINT32_C
#define LONG_C __INT32_C
#elif __SIZEOF_LONG__ == 8
#define ULONG_C __UINT64_C
#define LONG_C __INT64_C
#else
#error
#endif

#endif /* _TYPES_H */
