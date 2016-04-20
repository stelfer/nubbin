/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/stddef.h>

const char* CONSOLE_TAG = "MEM";

static inline memory_page_tables_t*
get_mpt()
{
    return (memory_page_tables_t*)&page_table_paddr;
}

static inline memory_percpu_tbl_t*
get_percpu_tbl()
{
    return (memory_percpu_tbl_t*)&percpu_tbl_paddr;
}

static inline const struct mem_info*
get_bios_mem_info()
{
    return (const struct mem_info*)(KERNEL_SYM_VADDR(bios_mmap));
}

void
memory_print_bios_mmap()
{
    const struct mem_info* p = get_bios_mem_info();
    console_putq((uintptr_t)&p->entries);
    console_putw(p->low_mem);
    console_putw(p->high_mem);
    console_putq(p->size);
    for (unsigned int i = 0; i < p->size; ++i) {
        console_puts("ENTRY: start, length ,flags");
        console_putq(p->entries[i].base);
        console_putq(p->entries[i].len);
        console_putd(p->entries[i].type);
        console_putd(p->entries[i].ext_attr);
    }
}

static void
check_early_range(memory_page_tables_t* mpt,
                  uintptr_t pml4_off,
                  uintptr_t pdp_selector,
                  uintptr_t pdp_off,
                  uintptr_t pd_selector)
{
    if ((mpt->blks[MEMORY_VMEM_PML4][pml4_off] | 0x20) !=
        ((uintptr_t)&mpt->blks[pdp_selector][0] | 0x23)) {
        PANIC();
    }
    if ((mpt->blks[pdp_selector][pdp_off] | 0x20) !=
        ((uintptr_t)&mpt->blks[pd_selector][0] | 0x23)) {
        PANIC();
    }

    for (unsigned i = 0; i < 512; ++i) {
        /* Blast off the low status bytes */
        if ((mpt->blks[pd_selector][i] & 0xffffffffffffff00) !=
            (i * 0x200000)) {
            PANIC();
        }
    }
}

static void
check_early_mappings(memory_page_tables_t* mpt)
{
    console_start("Checking early memory maps");
    console_start("Checking lower maps");
    check_early_range(mpt, 0, MEMORY_VMEM_LOWER_PDP, 0, MEMORY_VMEM_LOWER_PD);
    console_ok();

    console_start("Checking upper maps");
    uintptr_t kern_base      = (uintptr_t)0xffffffff00000000;
    const uintptr_t pml4_off = memory_get_pml4_off(kern_base);
    const uintptr_t pdp_off  = memory_get_pdp_off(kern_base);
    check_early_range(
        mpt, pml4_off, MEMORY_VMEM_UPPER_PDP, pdp_off, MEMORY_VMEM_UPPER_PD);
    console_ok();

    console_ok();
}

void
memory_map_init_finish()
{
    memory_page_tables_t* mpt = get_mpt();
    check_early_mappings(mpt);
    /* Fix user space here? */
}

void
memory_percpu_init()
{
    /* The GDT gets pre-placed here during jump to long mode */
    memory_percpu_tbl_t* mio = get_percpu_tbl();
    mio->num_entries         = 1;
    mio->entries[0].size     = MEMORY_GDT_SIZE;
    mio->entries[0].state    = PERCPU_STATE_VALID;
    mio->entries[0].type     = PERCPU_TYPE_GDT;
    mio->entries[0].base     = (uintptr_t)&gdt_paddr;
    mio->size = sizeof(memory_percpu_tbl_t) + mio->entries[0].size;
}

uintptr_t
memory_percpu_alloc_phy(percpu_type_t type, percpu_size_t size)
{
    memory_percpu_tbl_t* mio = get_percpu_tbl();
    const size_t miosz       = (size_t)((char*)mio + mio->size);
    uintptr_t base           = align_to(miosz, MEMORY_PERCPU_ALLOC_ALIGNTO);

    if ((base + size) > KERNEL_SYM_PADDR(page_table_paddr)) {
        return 0;
    }
    memory_percpu_entry_t* men = &mio->entries[mio->num_entries];
    men->size                  = size;
    men->state                 = PERCPU_STATE_VALID;
    men->type                  = type;
    men->base                  = (uintptr_t)base;
    mio->size                  = (char*)base + size - (char*)mio;
    mio->num_entries           = mio->num_entries + 1;

    return base;
}
