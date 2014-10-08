#ifndef _MACRO_H
#define _MACRO_H

#include <kernel/types.h>

#define STRINGIFY(s) __STR(s)
#define __STR(s) #s

#define DIM(array_)         (sizeof(array_)/sizeof(*(array_)))
#define MIN(a, b) 			(((a) < (b)) ? (a) : (b))
#define MAX(x,y) 			(((x) > (y)) ? (x) : (y))

#ifndef CHAR_BIT
#define CHAR_BIT 			8
#endif

#define GETBIT(a, n) 		(a[n/CHAR_BIT]  & (1 << (n%CHAR_BIT)))
#define SETBIT(a, n) 		(a[n/CHAR_BIT] |= (1 << (n%CHAR_BIT)))
#define SETBIT_BE(a, n) 	(a[n/CHAR_BIT] |= (1 << (CHAR_BIT - 1 - n%CHAR_BIT)))
#define GETBIT_BE(a, n) 	(a[n/CHAR_BIT]  & (1 << (CHAR_BIT - 1 - n%CHAR_BIT)))

#if defined(__i386) || defined(__amd64)
static inline uint32_t log2(const uint32_t x) {
  uint32_t y;
  asm ( "\tbsr %1, %0\n" : "=r"(y) : "r" (x) );
  return y;
}
#else
#error UNRECOGNIZED ARCHITECTURE
#endif

#define IS_POWEROF_2(n) !(n & (n-1))
#define B2(x)   (   (x) | (   (x) >> 1) )
#define B4(x)   ( B2(x) | ( B2(x) >> 2) )
#define B8(x)   ( B4(x) | ( B4(x) >> 4) )
#define B16(x)  ( B8(x) | ( B8(x) >> 8) )  
#define B32(x)  (B16(x) | (B16(x) >>16) )
#define NEXT_POWEROF_2(x) (B32(x-1) + 1)
#define NEAREST_MULTIPLE_OF_8(x) (( (x) + 7 ) & ~7)

#define __SPLIT_BITMAP_A(y,w) ((y) > 0) * (((y) - 1) % (w) + 1)
#define __SPLIT_BITMAP_L(y,w) (((y) - 1)/(w) * __SPLIT_BITMAP_A(y,w))
#define __SPLIT_BITMAP_R(y,w) (MAX(__SPLIT_BITMAP_A(y,w), ((y) - 1)/(w) * (w)))
#define SPLIT_BITMAP_HIGH(x,y,w) ((__SPLIT_BITMAP_L(y,w) != (w)) * ((x) << __SPLIT_BITMAP_L(y,w)))
#define SPLIT_BITMAP_LOW(x,y,w) ((__SPLIT_BITMAP_R(y,w) != (w)) * ((x) << __SPLIT_BITMAP_R(y,w)))

#endif  /* _MACRO_H */
