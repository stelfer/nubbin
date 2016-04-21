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

uintptr_t cpu_trampoline_get_zone_stack_addr();

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

void isr_int020h();
void isr_int027h();

void
cpu_isr_apic_timer(uint8_t irq, interrupt_frame_t* frame)
{
    console_puts("HERE!");
    HALT();
}

void cpu_enable_apic();

uint32_t cpu_has_apic();

/* uint32_t */
/* cpuHasAPIC() */
/* { */
/*     uint32_t eax, edx; */
/*     eax                    = 0x1; */
/*     uint32_t CPU_FLAG_APIC = 0x200; */
/*     asm volatile("cpuid" : "=d"(edx) : "a"(eax)); */
/*     return edx & CPU_FLAG_APIC; */
/* } */

static void
enable_local_apic_timer(cpu_zone_t* zone)
{
    console_start("Enabling local apic timer");

    apic_local_reg_map_t* reg = &zone->lapic_reg;

    /* Enable the spurious interrupt vector */
    interrupt_write_gate(
        32, (uintptr_t)isr_int020h, IDT_PRESENT | IDT_TYPE_INTR_GATE);

    interrupt_write_gate(
        37, (uintptr_t)isr_int027h, IDT_PRESENT | IDT_TYPE_INTR_GATE);

    uint32_t sivr =
        APIC_REG_SPURIOUS(reg) | 0x0000010f | (LOCAL_APIC_SIVR_VEC << 4);
    APIC_REG_SPURIOUS(reg) = sivr;

    APIC_REG_DFR(reg)       = 0xffffffff;
    APIC_REG_LDR(reg)       = (APIC_REG_LDR(reg) & 0x0ffffff) | 1;
    APIC_REG_LVT_TMR(reg)   = APIC_DISABLE;
    APIC_REG_LVT_PERF(reg)  = APIC_NMI;
    APIC_REG_LVT_LINT0(reg) = APIC_DISABLE;
    APIC_REG_LVT_LINT1(reg) = APIC_DISABLE;
    APIC_REG_TASKPRIO(reg)  = 0;

    cpu_enable_apic();

    APIC_REG_SPURIOUS(reg) = 39 + APIC_SW_ENABLE;
    APIC_REG_LVT_TMR(reg)  = 32 + APIC_SW_ENABLE;
    APIC_REG_TMRDIV(reg)   = 0x03;

    console_putd(APIC_REG_SPURIOUS(reg));

    __asm__("int $1\n");
    console_ok();
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

static void
remap_lapic(cpu_zone_t* zone)
{
    console_start("Remapping local apic register");

    uintptr_t apic_base = (uintptr_t)&zone->lapic_reg;
    apic_set_base_msr(apic_base);
    apic_base = apic_get_base_msr();

    kdata_t* kdata   = kdata_get();
    uint32_t apic_id = cpu_zone_get_apic_id(zone);
    if (apic_id > kdata->cpu.num_cpus) {
        console_puts("Bad APIC ID");
        PANIC();
    }
    uint32_t cpu_id              = cpu_id_from_apic_id(apic_id);
    kdata->cpu.info[cpu_id].zone = (uintptr_t)zone;
    console_ok();
}

/*
 * Main entry-point for every cpu
 */
void
cpu_trampoline()
{
    cpu_zone_t* zone =
        (cpu_zone_t*)(cpu_trampoline_get_zone_stack_addr() - CPU_STACK_SIZE);
    if (zone == 0) {
        console_puts("Bad ZONE address");
        PANIC();
    }
    console_start("Trampoline");

    uintptr_t apic_base = apic_get_base_msr();
    const int is_bsp    = apic_cpu_is_bsp(apic_base);
    if (!is_bsp) {
        /* The bsp lapic is already remapped */
        remap_lapic(zone);
    }

    uint32_t apic_id = cpu_zone_get_apic_id(zone);
    uint32_t cpu_id  = cpu_id_from_apic_id(apic_id);

    console_putf("ZONE         = 0x0000000000000000", (uintptr_t)zone, 8, 17);
    console_putf("APIC_BASE    = 0x0000000000000000", apic_base, 8, 17);
    console_putf("APIC_ID      = 0x00000000", apic_id, 4, 17);
    console_putf("CPU_ID       = 0x00000000", cpu_id, 4, 17);
    console_putf("IS_BSP       = 0x00", is_bsp, 1, 17);
    console_putf(
        "INFO         = 0x0000000000000000", (uintptr_t)zone->info, 8, 17);

    if (is_bsp) {
        kdata_get()->cpu.info[cpu_id].status |= CPU_STAT_BSP;
    }

    enable_local_apic_timer(zone);

    for (;;)
        ;
}

static void
rewrite_stack(cpu_zone_t* zone)
{
    /* Finish fixing up the stack */
    /* The second stack entry points to the first, this prepares for pop rbp */
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
    cpu_move_stack((uintptr_t)&zone->stack[CPU_STACK_SIZE - 1],
                   (uintptr_t)&boot_paddr);
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
        uintptr_t apic_base = (uintptr_t)&zone->lapic_reg;
        /* The apic_base needs to have UC semantics see 3a.1 10.4.1 */
        memory_set_uc(apic_base);

        if (!found_bsp) {
            apic_set_base_msr(apic_base);
            uint32_t apic_id = cpu_zone_get_apic_id(zone);
            if (apic_id == kd->cpu.info[i].apic_id) {
                found_bsp = 1;
                move_stack(zone);
            }
        }
        zone->info = &kd->cpu.info[i];
        /*  */
        rewrite_stack(zone);
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
