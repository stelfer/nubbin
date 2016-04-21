/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/interrupt.h>

void
interrupt_trap(uint8_t e, interrupt_frame_t* frame)
{
    console_putf("E            = 0x00", e, 1, 17);
    console_putf("SS           = 0x0000000000000000", frame->ss, 8, 17);
    console_putf("RSP          = 0x0000000000000000", frame->sp, 8, 17);
    console_putf("FLAGS        = 0x0000000000000000", frame->flags, 8, 17);
    console_putf("CS           = 0x0000000000000000", frame->cs, 8, 17);
    console_putf("RIP          = 0x0000000000000000", frame->ip, 8, 17);
}

void
interrupt(uint8_t e, interrupt_frame_t* frame)
{
}

void
interrupt_abort(uint8_t e, interrupt_frame_t* frame)
{
}

void
interrupt_fault_code(uint8_t e, interrupt_frame_t* frame, uint64_t code)
{
    console_putf("E            = 0x0000000000000000", e, 8, 17);
    console_putf("CODE         = 0x0000000000000000", code, 8, 17);
    console_putf("SS           = 0x0000000000000000", frame->ss, 8, 17);
    console_putf("RSP          = 0x0000000000000000", frame->sp, 8, 17);
    console_putf("FLAGS        = 0x0000000000000000", frame->flags, 8, 17);
    console_putf("CS           = 0x0000000000000000", frame->cs, 8, 17);
    console_putf("RIP          = 0x0000000000000000", frame->ip, 8, 17);
    HALT();
}

void
interrupt_abort_code(uint8_t e, interrupt_frame_t* frame, uint64_t code)
{
}

void
interrupt_fault(uint8_t e, interrupt_frame_t* frame)
{
    HALT();
}

void
interrupt_irq(uint8_t irq, interrupt_frame_t* frame)
{
}
