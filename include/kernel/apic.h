/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _APIC_H
#define _APIC_H

#include <nubbin/kernel/types.h>

enum { LOCAL_APIC_SIVR_VEC = 0xf };

// #define APIC_CPU_IS_BSP(AB)

#define LOCAL_REG_MAP(name) \
    struct {                \
        uint32_t dw0;       \
        uint32_t dw1;       \
        uint32_t dw2;       \
        uint32_t dw3;       \
    } name
struct apic_local_reg_map {
    LOCAL_REG_MAP(off_0000);
    LOCAL_REG_MAP(off_0010);
    LOCAL_REG_MAP(off_0020);
    LOCAL_REG_MAP(off_0030);
    LOCAL_REG_MAP(off_0040);
    LOCAL_REG_MAP(off_0050);
    LOCAL_REG_MAP(off_0060);
    LOCAL_REG_MAP(off_0070);
    LOCAL_REG_MAP(off_0080);
    LOCAL_REG_MAP(off_0090);
    LOCAL_REG_MAP(off_00a0);
    LOCAL_REG_MAP(off_00b0);
    LOCAL_REG_MAP(off_00c0);
    LOCAL_REG_MAP(off_00d0);
    LOCAL_REG_MAP(off_00e0);
    LOCAL_REG_MAP(off_00f0);
    LOCAL_REG_MAP(off_0100);
    LOCAL_REG_MAP(off_0110);
    LOCAL_REG_MAP(off_0120);
    LOCAL_REG_MAP(off_0130);
    LOCAL_REG_MAP(off_0140);
    LOCAL_REG_MAP(off_0150);
    LOCAL_REG_MAP(off_0160);
    LOCAL_REG_MAP(off_0170);
    LOCAL_REG_MAP(off_0180);
    LOCAL_REG_MAP(off_0190);
    LOCAL_REG_MAP(off_01a0);
    LOCAL_REG_MAP(off_01b0);
    LOCAL_REG_MAP(off_01c0);
    LOCAL_REG_MAP(off_01d0);
    LOCAL_REG_MAP(off_01e0);
    LOCAL_REG_MAP(off_01f0);
    LOCAL_REG_MAP(off_0200);
    LOCAL_REG_MAP(off_0210);
    LOCAL_REG_MAP(off_0220);
    LOCAL_REG_MAP(off_0230);
    LOCAL_REG_MAP(off_0240);
    LOCAL_REG_MAP(off_0250);
    LOCAL_REG_MAP(off_0260);
    LOCAL_REG_MAP(off_0270);
    LOCAL_REG_MAP(off_0280);
    LOCAL_REG_MAP(off_0290);
    LOCAL_REG_MAP(off_02a0);
    LOCAL_REG_MAP(off_02b0);
    LOCAL_REG_MAP(off_02c0);
    LOCAL_REG_MAP(off_02d0);
    LOCAL_REG_MAP(off_02e0);
    LOCAL_REG_MAP(off_02f0);
    LOCAL_REG_MAP(off_0300);
    LOCAL_REG_MAP(off_0310);
    LOCAL_REG_MAP(off_0320);
    LOCAL_REG_MAP(off_0330);
    LOCAL_REG_MAP(off_0340);
    LOCAL_REG_MAP(off_0350);
    LOCAL_REG_MAP(off_0360);
    LOCAL_REG_MAP(off_0370);
    LOCAL_REG_MAP(off_0380);
    LOCAL_REG_MAP(off_0390);
    LOCAL_REG_MAP(off_03a0);
    LOCAL_REG_MAP(off_03b0);
    LOCAL_REG_MAP(off_03c0);
    LOCAL_REG_MAP(off_03d0);
    LOCAL_REG_MAP(off_03e0);
    LOCAL_REG_MAP(off_03f0);
};
#undef LOCAL_REG_MAP
typedef struct apic_local_reg_map apic_local_reg_map_t;

enum {
    APIC_DISABLE   = 0x10000,
    APIC_SW_ENABLE = 0x100,
    APIC_CPUFOCUS  = 0x200,
    APIC_NMI       = (4 << 8),
    TMR_PERIODIC   = 0x20000,
    TMR_BASEDIV    = (1 << 20)
};

#define APIC_REG_APIC_ID(r) (r)->off_0020.dw0
#define APIC_REG_APIC_VER(r) (r)->off_0030.dw0
#define APIC_REG_TASKPRIO(r) (r)->off_0080.dw0
#define APIC_REG_EOI(r) (r)->off_00b0.dw0
#define APIC_REG_LDR(r) (r)->off_00d0.dw0
#define APIC_REG_DFR(r) (r)->off_00e0.dw0
#define APIC_REG_SPURIOUS(r) (r)->off_00f0.dw0
#define APIC_REG_ESR(r) (r)->off_0280.dw0
#define APIC_REG_ICRL(r) (r)->off_0300.dw0
#define APIC_REG_ICRH(r) (r)->off_0310.dw0
#define APIC_REG_LVT_TMR(r) (r)->off_0320.dw0
#define APIC_REG_LVT_PERF(r) (r)->off_0340.dw0
#define APIC_REG_LVT_LINT0(r) (r)->off_0350.dw0
#define APIC_REG_LVT_LINT1(r) (r)->off_0360.dw0
#define APIC_REG_LVT_ERR(r) (r)->off_0370.dw0
#define APIC_REG_TMRINITCNT(r) (r)->off_0380.dw0
#define APIC_REG_TMRCURRCNT(r) (r)->off_0390.dw0
#define APIC_REG_TMRDIV(r) (r)->off_03f0.dw0

static inline uint8_t
apic_cpu_is_bsp(uintptr_t apic_base)
{
    return (apic_base & (1 << 8)) != 0;
}

static inline uint8_t
apic_local_apic_enabled(uintptr_t apic_base)
{
    return (apic_base & (1 << 11)) != 0;
}

static inline size_t
apic_base_addr(uintptr_t apic_base)
{
    return apic_base & 0xffffff000;
}

uint32_t apic_reg_read32(uintptr_t apic_base);

static inline uint8_t
apic_reg_apic_id(uintptr_t apic_base)
{
    return apic_reg_read32(apic_base_addr(apic_base) + 0x20);
}

void apic_set_base_msr(uintptr_t addr);

void apic_set_eoi();

void apic_spurious_isr();

uint64_t apic_get_base_msr();

#endif /* _APIC_H */
