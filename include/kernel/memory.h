/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _MEMORY_H
#define _MEMORY_H

struct mem_info_entry {
    unsigned long start;
    unsigned long length;
    unsigned int type;
    unsigned int ext_attr;
} __attribute__((packed));

struct mem_info {
    unsigned short low_mem;
    unsigned short high_mem;
    unsigned short size;
    struct mem_info_entry entries[];
} __attribute__((packed));

extern unsigned long kernel_vaddr_off;
extern unsigned long bios_mmap;
#define KERNEL_SYM_ADDR(x) \
    ((unsigned long)(&x) + (unsigned long)&kernel_vaddr_off)

#endif /* _MEMORY_H */
