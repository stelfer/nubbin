/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _CPU_H
#define _CPU_H

#include <nubbin/kernel/acpi.h>
#include <nubbin/kernel/apic.h>
#include <nubbin/kernel/types.h>

#define MAXCPUS 16

typedef uint8_t cpu_status_t;

enum { CPU_STAT_BSP = (1 << 0), CPU_STAT_APIC_ENABLED = (1 << 1) };

struct cpu_info {
    cpu_status_t status;
    acpi_proc_id_t acpi_proc_id;
    uintptr_t zone;
};
typedef struct cpu_info cpu_info_t;

struct cpu_kdata_info {
    uint32_t id;
    uint32_t apic_id;
    cpu_status_t status;
    uint32_t domain;
    uint8_t socket;
    uint8_t physical_core;
    uint8_t logical_core;
    acpi_proc_id_t acpi_proc_id;
    uintptr_t zone;
} __packed;
typedef struct cpu_kdata_info cpu_kdata_info_t;

struct cpu_kdata {
    /* Indexed by APIC_ID */
    uint8_t have_x2apic;
    uint8_t num_cpus;
    uint32_t ticks_per_hz;
    cpu_kdata_info_t info[MAXCPUS];
} __packed;
typedef struct cpu_kdata cpu_kdata_t;

enum { CPU_STACK_SIZE = 1024 };

extern uintptr_t cpu_stack_size;

/* A per-cpu memory zone. The apic reg is 4k aligned so that we can
 * use page protection to give it UC semantics
 */
struct cpu_zone {
    volatile uintptr_t lapic_reg;
    uint8_t stack[CPU_STACK_SIZE];
    cpu_kdata_info_t* info;

    uintptr_t schedule[512] __attribute__((aligned(0x1000)));
    uint8_t trampoline[4906] __attribute__((aligned(0x1000)));
    uint8_t reserved[] __attribute__((aligned(0x1000)));
} __packed;
typedef struct cpu_zone cpu_zone_t;

enum { CPU_ZONE_SIZE = 0x4000 };
STATIC_ASSERT(sizeof(cpu_zone_t) == CPU_ZONE_SIZE);

void cpu_bsp_init();

void cpu_spin_here();

void cpu_move_stack(uintptr_t new_top, uintptr_t old_top);

uint32_t cpu_id_from_x2apic_id(uint32_t x2apic_id);

uint32_t cpu_id_from_apic_id(uint32_t apic_id);

uintptr_t cpu_get_zone_addr();

void cpu_prepare_trampoline(uintptr_t area);

#endif /* _CPU_H */
