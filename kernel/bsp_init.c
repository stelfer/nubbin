/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/cpu.h>
#include <nubbin/kernel/kdata.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/serial.h>
#include <nubbin/kernel/string.h>

int hello_user();
void
bsp_init()
{
    serial_init();
    console_bold();
    console_puts("Starting nubbin " VERSION);
    console_reset();

    kdata_init();

    memory_map_init_finish();

    acpi_init();

    cpu_bsp_init();

    hello_user();

    /* Bounces to cpu_trampoline() on return */
}
