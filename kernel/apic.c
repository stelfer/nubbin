/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/apic.h>
#include <nubbin/kernel/console.h>

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

    /* apic_pit_prepare_sleep(10000); */

    /* APIC_REG_TMRINITCNT(reg) = -1; */

    /* apic_pit_lseep(); */

    uint32_t ticks = apic_calibrate_timer(reg);

    /* 0xffffffff - APIC_REG_TMRCURRCNT(reg); */
    console_putd(ticks);
    HALT();

    APIC_REG_TMRDIV(reg)     = 0x03;
    APIC_REG_LVT_TMR(reg)    = 32 | APIC_TMR_PERIODIC;
    APIC_REG_TMRINITCNT(reg) = ticks;
}
