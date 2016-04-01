// Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt
#ifndef _SERIAL_H
#define _SERIAL_H

#include <nubbin/kernel/io.h>

void serial_putc(char a);

void serial_puts(const char* buf);

void serial_put(unsigned long addr, unsigned long len);
void serial_putf(const char* fmt,
                 unsigned long addr,
                 unsigned long len,
                 unsigned long off);

void serial_init();

#define serial_putb(x) serial_put((x), 1)
#define serial_putw(x) serial_put((x), 2)
#define serial_putd(x) serial_put((x), 4)
#define serial_putq(x) serial_put((x), 8)

#endif /* _SERIAL_H */
