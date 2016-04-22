/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/apic.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/kdata.h>

void
apic_timer_init(uintptr_t reg)
{
    APIC_REG_SPURIOUS(reg)  = 39 | APIC_SW_ENABLE;
    APIC_REG_LDR(reg)       = (APIC_REG_LDR(reg) & 0x0ffffff) | 1;
    APIC_REG_LVT_TMR(reg)   = APIC_DISABLE;
    APIC_REG_LVT_PERF(reg)  = APIC_NMI;
    APIC_REG_LVT_LINT0(reg) = APIC_DISABLE;
    APIC_REG_LVT_LINT1(reg) = APIC_DISABLE;
    APIC_REG_DFR(reg)       = 0xffffffff;
    APIC_REG_TASKPRIO(reg)  = 0;
    APIC_REG_TMRDIV(reg)    = 0x03;
    APIC_REG_LVT_TMR(reg)   = 32;

    kdata_t* kd = kdata_get();
    if (kd->cpu.ticks_per_hz == 0) {
        kd->cpu.ticks_per_hz = apic_calibrate_timer(reg, CLOCK_HZ);
    }
    APIC_REG_TMRDIV(reg)     = 0x03;
    APIC_REG_LVT_TMR(reg)    = 32 | APIC_TMR_PERIODIC;
    APIC_REG_TMRINITCNT(reg) = kd->cpu.ticks_per_hz;
}
