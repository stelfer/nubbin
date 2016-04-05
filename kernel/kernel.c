/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/string.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/serial.h>
#include <nubbin/kernel/cpu.h>
#include <nubbin/kernel/kdata.h>

int hello_user();
int task_init();
void
main()
{
    serial_init();
    serial_puts("Starting kernel...");

    kdata_init();

    memory_map_init_finish();

    cpu_init();

    task_init();

    hello_user();

    for (;;)
        ;
}
