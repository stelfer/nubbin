// Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt
#ifndef _SERIAL_H
#define _SERIAL_H

#include <nubbin/kernel/io.h>

#define outb port_byte_out
#define inb port_byte_in

#define PORT 0x3f8 /* COM1 */

int serial_tx_empty();

void serial_write(char a);

void serial_puts(const char* buf);

void serial_init();

#endif /* _SERIAL_H */
