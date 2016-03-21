/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include "nubbin/kernel/string.h"

void*
memcpy(void* dst, void const* src, unsigned long len)
{
    char* ds       = (char*)dst;
    char const* sr = (char* const)src;
    while (len--) {
        *ds++ = *sr++;
    }
    return dst;
}

#if 0
void*
X_aligned_memcpy_sse2(void* dest, const void* src, const unsigned long size)
{
    __asm {
    mov esi, src;  // src pointer
    mov edi, dest;  // dest pointer

    mov ebx, size;  // ebx is our counter 
    shr ebx, 7;  // divide by 128 (8 * 128bit registers)


    loop_copy:
      prefetchnta 128[ESI];  // SSE2 prefetch
      prefetchnta 160[ESI];
      prefetchnta 192[ESI];
      prefetchnta 224[ESI];

      movdqa xmm0, 0[ESI];  // move data from src to registers
      movdqa xmm1, 16[ESI];
      movdqa xmm2, 32[ESI];
      movdqa xmm3, 48[ESI];
      movdqa xmm4, 64[ESI];
      movdqa xmm5, 80[ESI];
      movdqa xmm6, 96[ESI];
      movdqa xmm7, 112[ESI];

      movntdq 0[EDI], xmm0;  // move data from registers to dest
      movntdq 16[EDI], xmm1;
      movntdq 32[EDI], xmm2;
      movntdq 48[EDI], xmm3;
      movntdq 64[EDI], xmm4;
      movntdq 80[EDI], xmm5;
      movntdq 96[EDI], xmm6;
      movntdq 112[EDI], xmm7;

      add esi, 128;
      add edi, 128;
      dec ebx;

      jnz loop_copy;  // loop please
    loop_copy_end:
    }
    return dst;
}
#endif
const char*
hexify(unsigned long x, char* fmt, unsigned long len)
{
    if (len < 19) {
        return (const char*)0;
    }
    memcpy(fmt, "0x0000000000000000", 18);
    unsigned long i = 0;
    for (; i < 16; ++i) {
        unsigned char y = 0x0f & x;
        y &= 0xf;
        if (y <= 0x09) {
            y |= 0x30;
        } else {
            y -= 0x09;
            y |= 0x60;
        }
        fmt[17 - i] = y;
        x           = x >> 4;
    }
    fmt[18] = '\0';
    return fmt;
}
