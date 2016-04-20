/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include <nubbin/kernel/stddef.h>

enum {
    IDT_PRESENT        = (1 << 15),
    IDT_DPL_RING0      = (0 << 13),
    IDT_DPL_RING1      = (1 << 13),
    IDT_DPL_RING2      = (2 << 13),
    IDT_DPL_RING3      = (3 << 13),
    IDT_TYPE_CALL_GATE = (0xc << 8),
    IDT_TYPE_INTR_GATE = (0xe << 8),
    IDT_TYPE_TRAP_GATE = (0xf << 8),
};

void interrupt_write_gate(uint8_t id, uintptr_t gate, uint16_t type);

extern void isr_timer();

struct interrupt_frame {
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t sp;
    uint64_t ss;
} __packed;
typedef struct interrupt_frame interrupt_frame_t;

#endif /* _INTERRUPT_H */
