/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/interrupt.h>

void
interrupt_trap(interrupt_frame_t* frame)
{
    console_putf("SS           = 0x0000000000000000", frame->ss, 8, 17);
    console_putf("RSP          = 0x0000000000000000", frame->sp, 8, 17);
    console_putf("FLAGS        = 0x0000000000000000", frame->flags, 8, 17);
    console_putf("CS           = 0x0000000000000000", frame->cs, 8, 17);
    console_putf("RIP          = 0x0000000000000000", frame->ip, 8, 17);
}

void
interrupt(interrupt_frame_t* frame)
{
}

void
interrupt_abort(interrupt_frame_t* frame)
{
}

void
interrupt_fault_code(interrupt_frame_t* frame, uint64_t code)
{
}

void
interrupt_fault(interrupt_frame_t* frame)
{
}
