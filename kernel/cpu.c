/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/acpi.h>
#include <nubbin/kernel/apic.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/cpu.h>
#include <nubbin/kernel/kdata.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/string.h>

static const char* CONSOLE_TAG = "CPU";

static cpu_zone_t*
alloc_cpu_zone()
{
    return (cpu_zone_t*)KERNEL_VADDR(
        memory_percpu_alloc_phy(PERCPU_TYPE_ZONE, sizeof(cpu_zone_t)));
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
write_console_cpu_info(const kdata_t* kdata, int i)
{
    char* sindex = "XXXX ";
    string_hexify(sindex, i, 2, 0);
    console_write(sindex, 5);
    console_putq(kdata->cpu.status[i]);
}

static void
update_kdata_from_local_reg_map(apic_local_reg_map_t* map)
{
    kdata_t* kdata = kdata_get();
    console_putf("Found xxxx cpus", kdata->cpu.num_cpus, 2, 6);

    /* Search for the BSP, and/or update kdata  */
    uint32_t bsp_apic_id = map->off_0020.dw0;

    for (int i = 0; i < kdata->cpu.num_cpus; ++i) {
        if (kdata->cpu.apic_id[i] == bsp_apic_id) {
            kdata->cpu.status[i] |= CPU_STAT_BSP;
            kdata->cpu.lapic_reg[i] = (uintptr_t)map;
        }
        write_console_cpu_info(kdata, i);
    }
}

static void
check_bsp_sanity()
{
    uint64_t apic_base = apic_get_base_msr();
    uint8_t is_bsp     = apic_cpu_is_bsp(apic_base);
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

    memory_percpu_init();

    console_start("Allocating a zone for the BSP");
    cpu_zone_t* zone = alloc_cpu_zone();
    if (zone == 0) {
        console_puts("Can't alloc!");
        PANIC();
    } else {
        console_ok();
        console_start("Remapping local apic register");
        apic_set_base_msr((uintptr_t)&zone->lapic_reg);
        update_kdata_from_local_reg_map(&zone->lapic_reg);
        enable_local_apic_timer(&zone->lapic_reg);
        console_ok();
    }
    console_ok();
}

void
cpu_init()
{
    acpi_init();

    bsp_init();
}
