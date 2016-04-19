/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _CPU_H
#define _CPU_H

#include <nubbin/kernel/acpi.h>
#include <nubbin/kernel/apic.h>
#include <nubbin/kernel/types.h>

#define MAXCPUS 8

typedef uint8_t cpu_status_t;

enum { CPU_STAT_BSP = (1 << 0), CPU_STAT_APIC_ENABLED = (1 << 1) };

struct cpu_kdata {
    uint8_t num_cpus;
    cpu_status_t status[MAXCPUS];
    apic_id_t apic_id[MAXCPUS];
    acpi_proc_id_t acpi_proc_id[MAXCPUS];
    uintptr_t lapic_reg[MAXCPUS];
} __packed;
typedef struct cpu_kdata cpu_kdata_t;

enum { CPU_STACK_SIZE = 1024 };

extern uintptr_t cpu_stack_size;

/* A per-cpu memory zone */
struct cpu_zone {
    uint8_t stack[CPU_STACK_SIZE];
    apic_local_reg_map_t lapic_reg;
} __packed;
typedef struct cpu_zone cpu_zone_t;

void cpu_bsp_init();

void cpu_spin_here();

void cpu_move_stack(uintptr_t new_top, uintptr_t old_top);

#endif /* _CPU_H */
