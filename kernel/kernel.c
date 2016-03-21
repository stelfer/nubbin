/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/string.h>
#include <nubbin/kernel/memory.h>

void
main()
{
    const struct mem_info* p = (const struct mem_info*)0x100000;

    console_clear();

    char buf[19];

    console_puts(hexify(p->low_mem, buf, 19));
    console_puts(hexify(p->high_mem, buf, 19));

    const char* x = hexify(p->size, buf, 19);
    console_puts(x);
    unsigned int size = p->size;
    for (unsigned int i = 0; i < size; ++i) {
        console_puts("ENTRY: start, length ,flags");
        console_puts(hexify(p->entries[i].start, buf, 19));
        console_puts(hexify(p->entries[i].length, buf, 19));
        console_puts(hexify(p->entries[i].type, buf, 19));
    }

    for (;;)
        ;
}
