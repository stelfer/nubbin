/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/serial.h>

void
memory_print_bios_mmap()
{
    const struct mem_info* p =
        (const struct mem_info*)(KERNEL_SYM_ADDR(bios_mmap));

    serial_putw(p->low_mem);
    serial_putw(p->high_mem);
    serial_putq(p->size);
    for (unsigned int i = 0; i < p->size; ++i) {
        serial_puts("ENTRY: start, length ,flags");
        serial_putq(p->entries[i].start);
        serial_putq(p->entries[i].length);
        serial_putd(p->entries[i].type);
        serial_putd(p->entries[i].ext_attr);
    }
}
