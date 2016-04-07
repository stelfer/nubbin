/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel/cpu.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/acpi.h>
#include <nubbin/kernel/apic.h>
#include <nubbin/kernel.h>
#include <nubbin/kernel/kdata.h>

static const char* CONSOLE_TAG = "CPU";

/* Relocate the local apic memory address */
static apic_local_reg_map_t*
remap_local_apic_reg()
{
    console_start("Remaping local apic register");
    memory_percpu_init();

    uintptr_t mem = memory_percpu_alloc_phy(PERCPU_TYPE_LOCAL_APIC_REG_MAP,
                                            sizeof(apic_local_reg_map_t));
    if (mem == 0) {
        console_puts("NO ALLOC!");
        PANIC();
    }
    apic_set_base_msr(mem);

    console_ok();
    return (apic_local_reg_map_t*)KERNEL_VADDR(mem);
}

static void
enable_local_apic_timer(apic_local_reg_map_t* map)
{
    /* Enable the spurious interrupt vector */
    uint32_t sivr = map->off_00f0.dw0;
    sivr |= 0x0000010f | (LOCAL_APIC_SIVR_VEC << 4);
    map->off_00f0.dw0 = sivr;

    /* __asm__("int $32\n"); */
}

static void
update_kdata_from_local_reg_map(apic_local_reg_map_t* map)
{
    kdata_t* kdata = kdata_get();
    console_putf("Found xxxx cpus", kdata->cpu.num_cpus, 2, 6);

    /* Search for the BSP, and/or update kdata  */
    uint32_t bsp_apic_id = map->off_0020.dw0;

    for (uint32_t i = 0; i < kdata->cpu.num_cpus; ++i) {
        if (kdata->cpu.apic_id[i] == bsp_apic_id) {
            kdata->cpu.status[i] |= CPU_STAT_BSP;
            kdata->cpu.lapic_reg[i] = map;
            console_putf("cpu xxxx BSP", i, 2, 4);
        } else {
            console_putf("cpu xxxx", i, 2, 4);
        }
    }
}

static void
check_bsp_sanity()
{
    uint64_t apic_base = apic_get_base_msr();
    uint8_t is_bsp = apic_cpu_is_bsp(apic_base);
    if (!is_bsp) {
        console_puts("We booted into a non-bsp cpu");
        PANIC();
    }

    uint8_t is_enabled = apic_local_apic_enabled(apic_base);
    if (!is_enabled) {
        console_puts("We booted into a disabled cpu");
        PANIC();
    }
}

static void
bsp_init()
{
    console_start("Initializing the BSP");
    check_bsp_sanity();

    apic_local_reg_map_t* map = remap_local_apic_reg();
    update_kdata_from_local_reg_map(map);
    enable_local_apic_timer(map);

    console_ok();
}

void
cpu_init()
{
    acpi_init();

    bsp_init();
}
