/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/acpi.h>
#include <nubbin/kernel/apic.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/cpu.h>
#include <nubbin/kernel/kdata.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/string.h>

extern uintptr_t kernel_stack_paddr;

static const char* CONSOLE_TAG = "CPU";

uintptr_t cpu_trampoline_get_zone_stack_addr();

void
cpu_trampoline()
{
    uintptr_t zone = cpu_trampoline_get_zone_stack_addr() - CPU_STACK_SIZE;
    console_start("Trampoline");
    console_write("ZONE: ", 6);
    console_putq(zone);

    kdata_t* kdata = kdata_get();
    for (;;)
        ;
}

static cpu_zone_t*
alloc_cpu_zone()
{
    return (cpu_zone_t*)KERNEL_VADDR(
        memory_percpu_alloc_phy(PERCPU_TYPE_ZONE, sizeof(cpu_zone_t)));
}

static void
enable_local_apic_timer(cpu_zone_t* zone)
{
    console_start("Enabling local apic timer");

    apic_local_reg_map_t* map = &zone->lapic_reg;
    /* Enable the spurious interrupt vector */
    uint32_t sivr = map->off_00f0.dw0;
    sivr |= 0x0000010f | (LOCAL_APIC_SIVR_VEC << 4);
    map->off_00f0.dw0 = sivr;

    /* __asm__("int $32\n"); */
    console_ok();
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
remap_lapic(cpu_zone_t* zone)
{
    console_start("Remapping local apic register");

    uint64_t apic_base = apic_get_base_msr();
    console_putq(apic_base);
    apic_set_base_msr((uintptr_t)&zone->lapic_reg);
    apic_base = apic_get_base_msr();
    console_putq(apic_base);
    console_putq(apic_cpu_is_bsp(apic_base));
    kdata_t* kdata   = kdata_get();
    uint32_t apic_id = cpu_zone_get_apic_id(zone);
    if (apic_id > kdata->cpu.num_cpus) {
        console_puts("Bad APIC ID");
        PANIC();
    }
    kdata->cpu.zone[apic_id] = (uintptr_t)zone;
    console_ok();
}

static void
fixup_stack(cpu_zone_t* zone)
{
    /* This is a little unsafe... Any pushed RBP's wont' be modified to hit
     * the new addresses. But this is why we are doing it early, and it
     * won't matter anyway since we are going to replace the old values here
     * with the trampoline anyway */
    console_start("Moving stack");
    cpu_move_stack((uintptr_t)&zone->stack[CPU_STACK_SIZE - 1],
                   (uintptr_t)&kernel_stack_paddr);
    console_ok();

    console_start("Rewriting stack");
    /* Finish fixing up the stack */
    /* The second stack entry points to the first */
    *((uintptr_t*)&zone->stack[CPU_STACK_SIZE - 2 * 8]) =
        (uintptr_t)&zone->stack[CPU_STACK_SIZE - 1 * 8];

    /* The first will get exectuted when we return from main */
    *((uintptr_t*)&zone->stack[CPU_STACK_SIZE - 1 * 8]) =
        (uintptr_t)cpu_trampoline;

    console_ok();
}

void
cpu_zone_init(cpu_zone_t* zone)
{
    remap_lapic(zone);
    enable_local_apic_timer(zone);
    fixup_stack(zone);
}

void
cpu_bsp_init()
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
        cpu_zone_init(zone);

        kdata_t* kdata   = kdata_get();
        uint32_t apic_id = cpu_zone_get_apic_id(zone);
        kdata->cpu.status[apic_id] |= CPU_STAT_BSP;
    }
    console_ok();
}
