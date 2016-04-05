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
apic_local_reg_map_t*
remap_local_apic_reg()
{
    console_start("Remaping local apic register");
    memory_mmio_init();

    void* mem = memory_mmio_alloc_phy(MMIO_TYPE_LOCAL_APIC_REG_MAP,
                                      sizeof(apic_local_reg_map_t));
    if (mem == NULL) {
        console_puts("NO ALLOC!");
    }
    apic_set_base_msr(mem);

    console_ok();
    apic_local_reg_map_t* rv = KERNEL_ADDR(mem);
    return rv;
}

void
bsp_init()
{
    console_start("Initializing the BSP");
    u64 apic_base = apic_get_base_msr();
    u8 is_bsp = apic_cpu_is_bsp(apic_base);
    if (!is_bsp) {
        console_puts("We booted into a non-bsp cpu");
        PANIC();
    }

    u8 is_enabled = apic_local_apic_enabled(apic_base);
    if (!is_enabled) {
        console_puts("We booted into a disabled cpu");
        PANIC();
    }

    kdata_t* kdata = kdata_get();
    console_putf("Found xxxx cpus", kdata->cpu.num_cpus, 2, 6);

    /* Search for the BSP  */
    apic_local_reg_map_t* mem = remap_local_apic_reg();
    u32 bsp_apic_id           = mem->off_0020.dw0;

    for (u32 i = 0; i < kdata->cpu.num_cpus; ++i) {
        if (kdata->cpu.apic_id[i] == bsp_apic_id) {
            kdata->cpu.status[i] |= CPU_STAT_BSP;
            kdata->cpu.lapic_reg[i] = mem;
            console_putf("cpu xxxx BSP", i, 2, 4);

        } else {
            console_putf("cpu xxxx", i, 2, 4);
        }
    }
    console_ok();
}

void
cpu_init()
{
    acpi_init();

    bsp_init();
}
