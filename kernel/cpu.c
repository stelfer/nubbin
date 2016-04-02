/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel/cpu.h>
#include <nubbin/kernel/serial.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/acpi.h>

void
cpu_init()
{
    const char* stepping = "CPU : Stepping = 0x00";
    serial_putf(stepping, 0x12, 1, 17);

    acpi_init();
}
