/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel.h>
#include <nubbin/kernel/stddef.h>

const char* CONSOLE_TAG = "MEM";

static inline memory_page_tables_t*
get_mpt()
{
    return (memory_page_tables_t*)&page_table_paddr;
}

static inline memory_mmio_tbl_t*
get_mmio_tbl()
{
    return (memory_mmio_tbl_t*)&mmio_paddr;
}

static inline const struct mem_info*
get_bios_mem_info()
{
    return (const struct mem_info*)(KERNEL_SYM_ADDR(bios_mmap));
}

void
memory_print_bios_mmap()
{
    const struct mem_info* p = get_bios_mem_info();
    console_putq((u64)&p->entries);
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
check_early_mappings(memory_page_tables_t* mpt)
{
    console_start("Checking early memory maps");
    /* memory_map_init_early() did what we expected */
    /* The user mpl entries should map to lower values, also the are marked with
     * present and accessed (probably), so we compare
     */
    if ((mpt->pml4[0] | 0x20) != ((u64)&mpt->user_pdps[0] | 0x23)) {
        PANIC();
    }
    if ((mpt->user_pdps[0][0] | 0x20) != ((u64)&mpt->user_pds[0] | 0x23)) {
        PANIC();
    }
    for (u16 i = 0; i < 512; ++i) {
        /* Blast off the low status bytes */
        if ((mpt->user_pds[0][i] & 0xffffffffffffff00) != (i * 0x200000)) {
            PANIC();
        }
    }

    void* kern_base    = (void*)0xffffffff00000000;
    const u64 pml4_off = memory_get_pml4_off(kern_base);
    if ((mpt->pml4[pml4_off] | 0x20) != ((u64)&mpt->kern_pdps[0] | 0x23)) {
        PANIC();
    }
    /* The 508 pdps map the same linear range as the user 0 pdps, so they
     * use the same pds */
    const u64 pdp_off = memory_get_pdp_off(kern_base);
    if ((mpt->kern_pdps[0][pdp_off] | 0x20) !=
        ((u64)&mpt->user_pds[0] | 0x23)) {
        PANIC();
    }
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
memory_mmio_init()
{
    memory_mmio_tbl_t* mio = (memory_mmio_tbl_t*)&mmio_paddr;
    mio->size              = sizeof(memory_mmio_tbl_t);
    mio->num_entries       = 0;
}

void*
memory_mmio_alloc_phy(mmio_type_t type, mmio_size_t size)
{
    memory_mmio_tbl_t* mio = get_mmio_tbl();
    const size_t miosz     = (size_t)((char*)mio + mio->size);
    u64 base               = align_to(miosz, MIO_ALLOC_ALIGNTO);

    if ((base + size) > (u64)((char*)mio + 0x400000)) {
        return NULL;
    }

    memory_mmio_entry_t* men = &mio->entries[mio->num_entries];
    men->size                = size;
    men->state               = MMIO_STATE_VALID;
    men->type                = type;
    men->base                = (void*)base;
    mio->size                = (char*)base + size - (char*)mio;
    mio->num_entries         = mio->num_entries + 1;

    return (void*)base;
}
