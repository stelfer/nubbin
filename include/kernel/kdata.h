// Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt

#ifndef _KDATA_H
#define _KDATA_H

#include <nubbin/kernel/types.h>

#define KDATA_SIZE 0x1000
#define KDATA_MAGIC 0xdeadbeef

extern size_t kdata_base;
extern size_t kdata_size;

#include <nubbin/kernel/acpi.h>
#include <nubbin/kernel/cpu.h>
#include <nubbin/kernel/memory.h>

/* The kernel data structure held at 0x200000 */
struct kdata {
    uint32_t magic;
    acpi_kdata_t acpi;
    cpu_kdata_t cpu;
    memory_kdata_t memory;
} __packed;
typedef struct kdata kdata_t;
STATIC_ASSERT(sizeof(kdata_t) <= KDATA_SIZE);

void kdata_init();

static inline kdata_t*
kdata_get()
{
    return (kdata_t*)KERNEL_SYM_VADDR(kdata_base);
}

#endif /* _KDATA_H */
