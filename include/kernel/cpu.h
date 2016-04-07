/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _CPU_H
#define _CPU_H

#include <nubbin/kernel/types.h>
#include <nubbin/kernel/acpi.h>

#define MAXCPUS 8

typedef unsigned char cpu_status_t;

enum { CPU_STAT_BSP = (1 << 0), CPU_STAT_APIC_ENABLED = (1 << 1) };

struct cpu_kdata {
    unsigned char num_cpus;
    cpu_status_t status[MAXCPUS];
    apic_id_t apic_id[MAXCPUS];
    acpi_proc_id_t acpi_proc_id[MAXCPUS];
    void* lapic_reg[MAXCPUS];
} __packed;
typedef struct cpu_kdata cpu_kdata_t;

void cpu_init();

void cpu_spin_here();

#endif /* _CPU_H */
