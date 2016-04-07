/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _MEMORY_H
#define _MEMORY_H

#include <nubbin/kernel/types.h>
#include <nubbin/kernel/intrinsics.h>

extern size_t kernel_vaddr_off;
extern size_t bios_mmap;
extern size_t page_table_paddr;
extern size_t percpu_tbl_paddr;
extern size_t gdt_paddr;

#define KERNEL_VADDR(x) (uintptr_t)((char*)x + (uintptr_t)&kernel_vaddr_off)
#define KERNEL_SYM_VADDR(x) KERNEL_VADDR(&(x))
#define KERNEL_SYM_PADDR(x) ((uintptr_t) & (x))

// Our page tables sit on a single physical 2MiB page residing at
// page_table_paddr.  We set aside 1 4k block for the PML4, one
// for a PDP to handle the lower half, one PDP for the upper half,
// one PDP for system percpu mapping, then the rest for PDs.  The top four
// PDs are reserved for the the kernel to do auxilliary mappings (percpu, etc).

typedef u64 memory_vmem_blk_t[512];

enum {
    MEMORY_NUM_USER_PDPS = 1,
    MEMORY_NUM_KERN_PDPS = 1,
    MEMORY_NUM_MMAP_PDPS = 1,
    MEMORY_NUM_KERN_PDS  = 4,
    MEMORY_NUM_USER_PDS  = 512 - 1 - MEMORY_NUM_USER_PDPS -
                          MEMORY_NUM_KERN_PDPS -
                          MEMORY_NUM_MMAP_PDPS -
                          MEMORY_NUM_KERN_PDS
};

struct memory_page_tables {
    memory_vmem_blk_t pml4;
    memory_vmem_blk_t user_pdps[MEMORY_NUM_USER_PDPS];
    memory_vmem_blk_t kern_pdps[MEMORY_NUM_KERN_PDPS];
    memory_vmem_blk_t mmap_pdps[MEMORY_NUM_MMAP_PDPS];
    memory_vmem_blk_t user_pds[MEMORY_NUM_USER_PDS];
    memory_vmem_blk_t kern_pds[MEMORY_NUM_KERN_PDS];
} __packed;
typedef struct memory_page_tables memory_page_tables_t;
STATIC_ASSERT(sizeof(memory_page_tables_t) == 0x200000);

enum { KERN_PDP_USER = 0, KERN_PDP_PERCPU = 1 };

/*
 * The first page of the Remmaped PERCPU region contains a table of all entries.
 * This allows us to dynamically use the area to provide physical mappings.
 */

enum {
    PERCPU_TYPE_UNSPEC             = 0,
    PERCPU_TYPE_LOCAL_APIC_REG_MAP = 1,
    PERCPU_TYPE_GDT                = 2
};

enum { PERCPU_STATE_UNSPEC = 0, PERCPU_STATE_VALID = 1 };

typedef u16 percpu_type_t;
typedef u16 percpu_state_t;
typedef u32 percpu_size_t;

struct memory_percpu_entry {
    percpu_size_t size;
    percpu_state_t state;
    percpu_type_t type;
    uintptr_t base;
} __packed;
typedef struct memory_percpu_entry memory_percpu_entry_t;

enum { MEMORY_PERCPU_MAX_ENTRIES = 255 };

enum { MEMORY_GDT_SIZE = 0x400 };

struct memory_percpu_tbl {
    percpu_size_t size;
    u32 num_entries;
    u64 pad;
    memory_percpu_entry_t entries[MEMORY_PERCPU_MAX_ENTRIES];
} __packed;
typedef struct memory_percpu_tbl memory_percpu_tbl_t;
STATIC_ASSERT(sizeof(memory_percpu_tbl_t) == 0x1000);

enum { MEMORY_PERCPU_ALLOC_ALIGNTO = 0x400 };

static inline uintptr_t
memory_get_off(uintptr_t virt_addr)
{
    return virt_addr & 0x00000000001fffff;
}

static inline u32
memory_get_pd_off(uintptr_t virt_addr)
{
    return ((u64)virt_addr & 0x000000003fe00000) >> 21;
}

static inline u32
memory_get_pdp_off(uintptr_t virt_addr)
{
    return ((u64)virt_addr & 0x0000007fc0000000) >> 30;
}

static inline u32
memory_get_pml4_off(uintptr_t virt_addr)
{
    return ((u64)virt_addr & 0x0000ff8000000000) >> 39;
}

static u32
memory_get_pd(uintptr_t virt_addr)
{
    return (u64)virt_addr / 0x40000000;
}

struct mem_info_entry {
    u64 base;
    u64 len;
    u32 type;
    u32 ext_attr;
} __attribute__((packed));

struct mem_info {
    u16 low_mem;
    u16 high_mem;
    u32 size;
    struct mem_info_entry entries[];
} __attribute__((packed));

#define EBDA_ADDR \
    (size_t)(((*((uint16_t*)(0x40E))) << 4))  // *16 because it's a RM address

enum {
    PD_PRES = (1 << 0),
    PD_RDWR = (1 << 1),
    PD_USER = (1 << 2),
    PD_WRTH = (1 << 3),
    PD_CDSL = (1 << 4),
    PD_ACCD = (1 << 5)
};

enum {
    PTE_PRES = (1 << 0),
    PTE_RDWR = (1 << 1),
    PTE_USER = (1 << 2),
    PTE_WRTH = (1 << 3),
    PTE_CDSL = (1 << 4),
    PTE_ACCD = (1 << 5),
    PTE_DRTY = (1 << 6),
    PTE_HUGE = (1 << 7)
};

void memory_print_bios_mmap();

uintptr_t memory_map_user(uintptr_t phy_addr, uintptr_t virt_addr);

uintptr_t memory_map_kern(uintptr_t phy_addr, uintptr_t virt_addr);

void memory_map_init_finish();

void memory_flush_tlb();

void memory_percpu_init();

uintptr_t memory_percpu_alloc_phy(percpu_type_t type, percpu_size_t size);

#endif /* _MEMORY_H */
