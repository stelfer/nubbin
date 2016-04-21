/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/interrupt.h>
#include <nubbin/kernel/kdata.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/stddef.h>

CONSOLE_TAG("MEM");

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
                  uintptr_t pd_selector,
                  uintptr_t pt_selector)
{
    if ((mpt->blks[MEMORY_VMEM_PML4][pml4_off] | 0x20) !=
        ((uintptr_t)&mpt->blks[pdp_selector][0] | 0x23)) {
        PANIC();
    }
    if ((mpt->blks[pdp_selector][pdp_off] | 0x20) !=
        ((uintptr_t)&mpt->blks[pd_selector][0] | 0x23)) {
        PANIC();
    }
    if ((mpt->blks[pd_selector][0] | 0x20) !=
        ((uintptr_t)&mpt->blks[pt_selector][0] | 0x23)) {
        PANIC();
    }
    if ((mpt->blks[pd_selector][1] | 0x20) !=
        ((uintptr_t)&mpt->blks[pt_selector + 1][0] | 0x23)) {
        PANIC();
    }
    if ((mpt->blks[pd_selector][2] | 0x20) !=
        ((uintptr_t)&mpt->blks[pt_selector + 2][0] | 0x23)) {
        PANIC();
    }
    for (unsigned i = 0; i < 512; ++i) {
        /* Blast off the low status bytes */
        uintptr_t mapped_addr = mpt->blks[pt_selector][i] & 0xffffffffffffff00;
        uintptr_t paddr       = i * 0x1000;
        int ok                = mapped_addr == paddr;
        mapped_addr = mpt->blks[pt_selector + 1][i] & 0xffffffffffffff00;
        paddr += 0x200000;
        ok &= mapped_addr == paddr;
        mapped_addr = mpt->blks[pt_selector + 2][i] & 0xffffffffffffff00;
        paddr += 0x200000;
        ok &= mapped_addr == paddr;
        if (!ok) {
            PANIC();
        }
    }
}

static void
check_early_mappings(memory_page_tables_t* mpt)
{
    console_start("Checking early memory maps");
    console_start("Checking lower maps");
    check_early_range(mpt,
                      0,
                      MEMORY_VMEM_LOWER_PDP,
                      0,
                      MEMORY_VMEM_LOWER_PD,
                      MEMORY_VMEM_LOWER_PT0);
    console_ok();

    console_start("Checking upper maps");
    uintptr_t kern_base      = (uintptr_t)0xffffffff00000000;
    const uintptr_t pml4_off = memory_get_pml4_off(kern_base);
    const uintptr_t pdp_off  = memory_get_pdp_off(kern_base);
    check_early_range(mpt,
                      pml4_off,
                      MEMORY_VMEM_UPPER_PDP,
                      pdp_off,
                      MEMORY_VMEM_UPPER_PD,
                      MEMORY_VMEM_UPPER_PT0);
    console_ok();

    console_ok();
}

static void
percpu_init()
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

void
memory_map_init_finish()
{
    memory_page_tables_t* mpt = get_mpt();
    check_early_mappings(mpt);
    percpu_init();
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

static void
vmem_map_not_present_page(uintptr_t addr,
                          uintptr_t mask,
                          uintptr_t pd_sel,
                          uintptr_t pdp_sel)
{
    uintptr_t pml4_off  = memory_get_pml4_off(addr | mask);
    uintptr_t pdp_off   = memory_get_pdp_off(addr | mask);
    uintptr_t pd_off    = memory_get_pd_off(addr | mask);
    uintptr_t addr_pd   = (addr & -(MEMORY_VMEM_PAGE_SIZE * 512));
    uintptr_t pd        = addr_pd == 0 ? pd_sel : addr_pd + MEMORY_VMEM_PD1 - 1;
    uintptr_t page_addr = addr - memory_get_off(addr | mask);

    /* memory_get_page_addr(addr); */
    memory_page_tables_t* mpt = get_mpt();
    mpt->blks[pd][pd_off]     = page_addr | PTE_PRES | PTE_RDWR | PTE_HUGE;

    /* Update the pdp and pml4 if necessary */
    if (mpt->blks[pdp_sel][pdp_off] == 0) {
        mpt->blks[pdp_sel][pdp_off] =
            (uintptr_t)&mpt->blks[pd] | PD_PRES | PD_RDWR;
    }
    if (mpt->blks[MEMORY_VMEM_PML4][pml4_off] == 0) {
        mpt->blks[MEMORY_VMEM_PML4][pml4_off] =
            (uintptr_t)&mpt->blks[pdp_sel] | PD_PRES | PD_RDWR;
    }
}

static inline void
vmem_map_not_present_low(uintptr_t addr)
{
    vmem_map_not_present_page(
        addr, 0, MEMORY_VMEM_LOWER_PD, MEMORY_VMEM_LOWER_PDP);
}

static inline void
vmem_map_not_present_high(uintptr_t addr)
{
    vmem_map_not_present_page(
        addr, 0xffffffff00000000, MEMORY_VMEM_UPPER_PD, MEMORY_VMEM_UPPER_PDP);
}

void
memory_isr_pf(interrupt_frame_t* frame, uintptr_t code, uintptr_t addr)
{
    /* console_putf("SS           = 0x0000000000000000", frame->ss, 8, 17); */
    /* console_putf("RSP          = 0x0000000000000000", frame->sp, 8, 17); */
    /* console_putf("FLAGS        = 0x0000000000000000", frame->flags, 8, 17);
     */
    /* console_putf("CS           = 0x0000000000000000", frame->cs, 8, 17); */
    /* console_putf("RIP          = 0x0000000000000000", frame->ip, 8, 17); */
    /* console_putf("CODE         = 0x0000000000000000", code, 8, 17); */
    /* console_putf("ADDR         = 0x0000000000000000", addr, 8, 17); */

    if (code == 0) {
        vmem_map_not_present_low(addr);
        if (addr < 0xffffffff00000000) {
            /* If the addr is < 4G, then also map into upper, even though it
             * wasn't requested */
            vmem_map_not_present_high(addr);
        }
    }
}
