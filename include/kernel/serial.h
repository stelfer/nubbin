// Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt
#ifndef _SERIAL_H
#define _SERIAL_H

#include <nubbin/kernel/io.h>

void serial_putc(char a);

void serial_puts(const char* buf);

void serial_putaddr(unsigned long addr, unsigned long len);

#endif /* _SERIAL_H */
