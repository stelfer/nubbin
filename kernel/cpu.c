/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/acpi.h>
#include <nubbin/kernel/apic.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/cpu.h>
#include <nubbin/kernel/interrupt.h>
#include <nubbin/kernel/kdata.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/string.h>

extern uintptr_t kernel_stack_paddr;

static const char* CONSOLE_TAG = "CPU";

uint32_t
cpu_id_from_x2apic_id(uint32_t x2apic_id)
{
    console_putd(x2apic_id);
    UNSUPPORTED();
    return 0;
}

uint32_t
cpu_id_from_apic_id(uint32_t apic_id)
{
    if (__unlikely(kdata_get()->cpu.have_x2apic)) {
        return cpu_id_from_x2apic_id(apic_id);
    }
    return apic_id;
}

static cpu_zone_t*
alloc_cpu_zone()
{
    return (cpu_zone_t*)KERNEL_VADDR(
        memory_percpu_alloc_phy(PERCPU_TYPE_ZONE, sizeof(cpu_zone_t)));
}

static int
lapic_init(cpu_zone_t* zone)
{
    console_start("Enabling LAPIC");

    volatile uintptr_t base = apic_get_base_msr();
    zone->lapic_reg         = base & ~0xfff;
    apic_timer_init(zone->lapic_reg);
    apic_enable();

    console_putf("REG_PTR      = 0x0000000000000000", base & ~0xfff, 8, 17);
    console_putf("REG          = 0x0000000000000000", zone->lapic_reg, 8, 17);
    console_putf("APIC_BASE    = 0x0000000000000000", base, 8, 17);
    console_putf("APIC_REG     = 0x0000000000000000", zone->lapic_reg, 8, 17);
    console_ok();
    return apic_cpu_is_bsp(base);
}

static void
write_console_cpu_info(const kdata_t* kdata, int i)
{
    char* sindex = "XXXX ";
    string_hexify(sindex, i, 2, 0);
    console_write(sindex, 5);
    console_putq(kdata->cpu.info[i].status);
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

void
boot_asps(uintptr_t reg)
{
    kdata_t* kd = kdata_get();
    for (uint8_t i = 0; i < kd->cpu.num_cpus; ++i) {
        if ((kd->cpu.info[i].status & CPU_STAT_BSP)) {
            continue;
        }
        console_putq(kd->cpu.info[i].zone);
        console_putq(kd->cpu.info[i].apic_id);
        apic_boot_asp(reg, kd->cpu.info[i].apic_id, kd->cpu.info[i].zone);
    }
}

/*
 * Main entry-point for every cpu
 */
void
cpu_trampoline()
{
    cpu_zone_t* zone = (cpu_zone_t*)cpu_get_zone_addr();
    if (zone == 0) {
        console_puts("Bad ZONE address");
        PANIC();
    }

    console_start("Trampoline");
    int is_bsp       = lapic_init(zone);
    uint32_t apic_id = APIC_REG_APIC_ID(zone->lapic_reg);
    uint32_t cpu_id  = cpu_id_from_apic_id(apic_id);
    console_putf("IS_BSP       = 0x00", is_bsp, 1, 17);
    console_putf("APIC_ID      = 0x00000000", apic_id, 4, 17);
    console_putf("CPU_ID       = 0x00000000", cpu_id, 4, 17);
    console_putf("ZONE         = 0x0000000000000000", (uintptr_t)zone, 8, 17);
    console_putf(
        "INFO         = 0x0000000000000000", (uintptr_t)zone->info, 8, 17);

    if (is_bsp) {
        kdata_get()->cpu.info[cpu_id].status |= CPU_STAT_BSP;
        boot_asps(zone->lapic_reg);
    }

    for (;;) {
        HALT();
    }
}

static void
rewrite_stack(cpu_zone_t* zone)
{
    /* Finish fixing up the stack */
    /* The second stack entry points to the first, this prepares for pop rbp
     */
    *((uintptr_t*)&zone->stack[CPU_STACK_SIZE - 2 * 8]) =
        (uintptr_t)&zone->stack[CPU_STACK_SIZE - 1 * 8];

    /* The first will get exectuted when we return from main */
    *((uintptr_t*)&zone->stack[CPU_STACK_SIZE - 1 * 8]) =
        (uintptr_t)cpu_trampoline;
}

extern uintptr_t boot_paddr;

static void
move_stack(cpu_zone_t* zone)
{
    /* This is a little unsafe... Any pushed RBP's wont' be modified to hit
     * the new addresses. But this is why we are doing it early, and it
     * won't matter anyway since we are going to replace the old values here
     * with the trampoline anyway */
    console_start("Moving stack");

    const uintptr_t old = (uintptr_t)&boot_paddr;
    const uintptr_t new = (uintptr_t)&zone->stack[CPU_STACK_SIZE - 1];
    console_putf("OLD          = 0x0000000000000000", old, 8, 17);
    console_putf("NEW          = 0x0000000000000000", new, 8, 17);
    cpu_move_stack(new, old);
    console_ok();
}

static void
alloc_cpu_zones()
{
    console_start("Allocating CPU zones");

    /* So, we are definitely running on the bsp here, but we don't know the
     * apic_id until we remap the apic base msr. So, we will try remapping
     * here as we allocate to check to see.
     */
    kdata_t* kd   = kdata_get();
    int found_bsp = 0;
    for (uint8_t i = 0; i < kd->cpu.num_cpus; ++i) {
        cpu_zone_t* zone = alloc_cpu_zone();
        if (zone == 0) {
            console_puts("Can't alloc!");
            PANIC();
        }
        if (!found_bsp) {
            volatile uintptr_t reg = apic_get_base_msr() & ~0xfff;
            if (reg == 0) {
                console_puts("Got NULL apic base");
                PANIC();
            }
            uint32_t apic_id = APIC_REG_APIC_ID(reg);
            if (apic_id == kd->cpu.info[i].apic_id) {
                found_bsp = 1;
                move_stack(zone);
                rewrite_stack(zone);
            }
        }
        zone->info       = &kd->cpu.info[i];
        zone->info->zone = (uintptr_t)zone;
    }
    console_ok();
}

void
cpu_bsp_init()
{
    console_start("Initializing the BSP");
    check_bsp_sanity();
    alloc_cpu_zones();
    console_ok();
}
