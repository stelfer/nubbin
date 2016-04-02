/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _MEMORY_H
#define _MEMORY_H

#include <nubbin/kernel/types.h>

struct mem_info_entry {
    uint64_t start;
    uint64_t length;
    uint32_t type;
    uint32_t ext_attr;
} __attribute__((packed));

struct mem_info {
    uint16_t low_mem;
    uint16_t high_mem;
    uint16_t size;
    struct mem_info_entry entries[];
} __attribute__((packed));

extern size_t kernel_vaddr_off;
extern size_t bios_mmap;
#define KERNEL_SYM_ADDR(x) ((size_t)(&x) + (size_t)&kernel_vaddr_off)

#define EBDA_ADDR \
    (size_t)(((*((uint16_t*)(0x40E))) << 4))  // *16 because it's a RM address

// #define EBDA_PTR_ADDR 0x40e
// #define EBDA_ADDR (unsigned short)()

void memory_print_bios_mmap();

#endif /* _MEMORY_H */
