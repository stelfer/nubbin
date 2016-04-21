/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _MEMORY_H
#define _MEMORY_H

#include <nubbin/kernel/intrinsics.h>
#include <nubbin/kernel/types.h>

#ifndef MAX_NUMA_DOMAINS
#define MAX_NUMA_DOMAINS 4
#endif

extern size_t kernel_vaddr_off;
extern size_t bios_mmap;
extern size_t page_table_paddr;
extern size_t percpu_tbl_paddr;
extern size_t gdt_paddr;

#define KERNEL_VADDR(x) (uintptr_t)((char*)x + (uintptr_t)&kernel_vaddr_off)
#define KERNEL_SYM_VADDR(x) KERNEL_VADDR(&(x))
#define KERNEL_SYM_PADDR(x) ((uintptr_t) & (x))

enum { MEMORY_VMEM_PAGE_SIZE = 0x200000 };

typedef uintptr_t memory_vmem_blk_t[512];

struct memory_page_tables {
    memory_vmem_blk_t blks[512];
} __packed;
typedef struct memory_page_tables memory_page_tables_t;
STATIC_ASSERT(sizeof(memory_page_tables_t) == 0x200000);

/*
 * The first MEMORY_VMEM_DYNAMIC blocks are used for the initial 6MiB mapping.
 * The rest of the memory is demand-mapped using the 512-MEMORY_VMEM_DYNAMIC
 * pages
 */
enum {
    MEMORY_VMEM_PML4,
    MEMORY_VMEM_LOWER_PDP,
    MEMORY_VMEM_LOWER_PD,
    MEMORY_VMEM_LOWER_PT0,
    MEMORY_VMEM_LOWER_PT1,
    MEMORY_VMEM_LOWER_PT2,
    MEMORY_VMEM_UPPER_PDP,
    MEMORY_VMEM_UPPER_PD,
    MEMORY_VMEM_UPPER_PT0,
    MEMORY_VMEM_UPPER_PT1,
    MEMORY_VMEM_UPPER_PT2,
    MEMORY_VMEM_PD1
};

static inline int
memory_vmem_check_blk(uintptr_t pdp)
{
    if (pdp == 0) {
        return 1;
    }
    return (pdp + MEMORY_VMEM_PD1 - 1) < 512;
}

static inline uintptr_t
memory_vmem_get_lower_blk(uintptr_t pdp)
{
    if (pdp == 0) {
        return MEMORY_VMEM_LOWER_PD;
    }
    return pdp + MEMORY_VMEM_PD1 - 1;
}

static inline uintptr_t
memory_vmem_get_upper_blk(uintptr_t pdp)
{
    if (pdp == 0) {
        return MEMORY_VMEM_UPPER_PD;
    }
    return pdp + MEMORY_VMEM_PD1 - 1;
}

enum { KERN_PDP_USER = 0, KERN_PDP_PERCPU = 1 };

/*
 * The first page of the Remmaped PERCPU region contains a table of all entries.
 * This allows us to dynamically use the area to provide physical mappings.
 */

enum { PERCPU_TYPE_UNSPEC = 0, PERCPU_TYPE_ZONE = 1, PERCPU_TYPE_GDT = 2 };

enum { PERCPU_STATE_UNSPEC = 0, PERCPU_STATE_VALID = 1 };

typedef uint16_t percpu_type_t;
typedef uint16_t percpu_state_t;
typedef uint32_t percpu_size_t;

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
    uint32_t num_entries;
    uint64_t pad;
    memory_percpu_entry_t entries[MEMORY_PERCPU_MAX_ENTRIES];
} __packed;
typedef struct memory_percpu_tbl memory_percpu_tbl_t;
STATIC_ASSERT(sizeof(memory_percpu_tbl_t) == 0x1000);

enum { MEMORY_PERCPU_ALLOC_ALIGNTO = 0x1000 };

static inline uintptr_t
memory_get_off(uintptr_t virt_addr)
{
    return virt_addr & 0x00000000001fffff;
}

static inline uint32_t
memory_get_pd_off(uintptr_t virt_addr)
{
    return ((uint64_t)virt_addr & 0x000000003fe00000) >> 21;
}

static inline uint32_t
memory_get_pdp_off(uintptr_t virt_addr)
{
    return ((uint64_t)virt_addr & 0x0000007fc0000000) >> 30;
}

static inline uint32_t
memory_get_pml4_off(uintptr_t virt_addr)
{
    return ((uint64_t)virt_addr & 0x0000ff8000000000) >> 39;
}

static uint32_t
memory_get_pd(uintptr_t virt_addr)
{
    return (uint64_t)virt_addr / 0x40000000;
}

struct mem_info_entry {
    uint64_t base;
    uint64_t len;
    uint32_t type;
    uint32_t ext_attr;
} __attribute__((packed));

struct mem_info {
    uint16_t low_mem;
    uint16_t high_mem;
    uint32_t size;
    struct mem_info_entry entries[];
} __attribute__((packed));
typedef struct mem_info mem_info_t;

#define EBDA_ADDR \
    (size_t)(((*((uint16_t*)(0x40E))) << 4))  // *16 because it's a RM address

enum {
    PD_PRES = (1 << 0),
    PD_RDWR = (1 << 1),
    PD_USER = (1 << 2),
    PD_PWD  = (1 << 3),
    PD_PCD  = (1 << 4),
    PD_ACCD = (1 << 5)
};

enum {
    PTE_PRES = (1 << 0),
    PTE_RDWR = (1 << 1),
    PTE_USER = (1 << 2),
    PTE_PWT  = (1 << 3),
    PTE_PCD  = (1 << 4),
    PTE_ACCD = (1 << 5),
    PTE_DRTY = (1 << 6),
    PTE_HUGE = (1 << 7)
};

struct memory_kdata_domain {
    uint32_t id;
    uint32_t domain;
    uint64_t base;
    uint64_t length;
    uint32_t flags;
} __aligned;
typedef struct memory_kdata_domain memory_kdata_domain_t;

struct memory_kdata {
    mem_info_t* bios_mem_info;
    memory_kdata_domain_t domains[MAX_NUMA_DOMAINS];
} __packed;
typedef struct memory_kdata memory_kdata_t;

void memory_print_bios_mmap();

uintptr_t memory_map_user(uintptr_t phy_addr, uintptr_t virt_addr);

uintptr_t memory_map_kern(uintptr_t phy_addr, uintptr_t virt_addr);

void memory_map_init_finish();

void memory_flush_tlb();

uintptr_t memory_percpu_alloc_phy(percpu_type_t type, percpu_size_t size);

static inline uintptr_t
memory_get_page_addr(uintptr_t addr)
{
    return addr & -MEMORY_VMEM_PAGE_SIZE;
}

void memory_invlpg(uintptr_t m);

void memory_set_uc(uintptr_t m);

#endif /* _MEMORY_H */
