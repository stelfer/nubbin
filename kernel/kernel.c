/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/string.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/serial.h>

extern unsigned long bios_mmap;

void
main()
{
    const struct mem_info* p = (const struct mem_info*)&bios_mmap;

    serial_puts("hi");

    /* console_clear(); */

    char buf[19];

    serial_puts(hexify(p->low_mem, buf, 19));
    serial_puts(hexify(p->high_mem, buf, 19));
    serial_puts(hexify(p->size, buf, 19));
    for (unsigned int i = 0; i < p->size; ++i) {
        serial_puts("ENTRY: start, length ,flags");
        serial_puts(hexify(p->entries[i].start, buf, 19));
        serial_puts(hexify(p->entries[i].length, buf, 19));
        serial_puts(hexify(p->entries[i].type, buf, 19));
        serial_puts(hexify(p->entries[i].ext_attr, buf, 19));
    }

    for (;;)
        ;
}
