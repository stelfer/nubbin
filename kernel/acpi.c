/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/acpi.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/string.h>
#include <nubbin/kernel/kdata.h>

CONSOLE_TAG("ACPI");

static rsdp_descr_t*
find_rsdp_in_mem(size_t start, size_t stop)
{
    for (size_t i = start; i < stop; i += 16) {
        const uint64_t x = *((uint64_t*)i);
        if (x == RSDP_SIG) {
            return (rsdp_descr_t*)i;
        }
    }
    return NULL;
}

static rsdp_descr_t*
find_rsdp()
{
    /* We are looking for "RSD PTR " inside of the first 1KiB of EBDA area, at
     * 16 byte offsets, or inside 0xe0000-0xfffff */
    rsdp_descr_t* rsdp = find_rsdp_in_mem(EBDA_ADDR, EBDA_ADDR + 0x400);
    if (rsdp == NULL) {
        rsdp = find_rsdp_in_mem(0xe0000, 0xfffff);
    }
    return rsdp;
}

static void
validate_rsdp_cksum(rsdp_descr_t* rsdp)
{
    const char* p     = (const char*)rsdp;
    unsigned long acc = 0;
    for (size_t i = 0; i < sizeof(rsdp_descr_t); ++i) {
        acc += p[i];
    }
    if ((acc & 0xff) != 0) {
        PANIC();
    }
}

static void
print_rsdp_info(rsdp_descr_t* rsdp)
{
    const char* found_rsdp_msg = "RSDP = 0x0000000000000000";
    console_putf(found_rsdp_msg, (unsigned long)rsdp, 8, 9);
    const char* acpi_ver_msg = "version = 0x00";
    console_putf(acpi_ver_msg, rsdp->rev + 1, 1, 12);
}

static acpi_tbl_hdr_t*
find_tbl(rsdp_descr_t* rsdp, const char* name)
{
    size_t addr           = rsdp->rsdt_addr;
    rsdt_t* rsdt          = (rsdt_t*)addr;
    const int num_entries = (rsdt->hdr.len - sizeof(rsdt->hdr)) / 4;
    for (int i = 0; i < num_entries; ++i) {
        addr              = rsdt->strct[i];
        acpi_tbl_hdr_t* h = (acpi_tbl_hdr_t*)addr;

        char buf[5];
        memcpy(buf, h->sig, 4);
        buf[4] = 0;
        console_puts(buf);

        if (!memcmp(h->sig, name, 4)) {
            return h;
        }
    }
    return NULL;
}

void
parse_madt_local_apic(apic_local_apic_t* h)
{
    kdata_t* kd        = kdata_get();
    const u32 i        = kd->cpu.num_cpus;
    kd->cpu.apic_id[i] = h->apic_id;
    if (h->flags == ACPI_LOCAL_APIC_ENABLED) {
        kd->cpu.status[i] |= CPU_STAT_APIC_ENABLED;
    }
    kd->cpu.acpi_proc_id[0] = h->acpi_proc_id;
    ++kd->cpu.num_cpus;
}

void
parse_apic(apic_tbl_t* apic)
{
    const size_t esize = (apic->hdr.len - sizeof(apic->hdr) -
                          sizeof(apic->lca) - sizeof(apic->flags));
    const u8* p   = &apic->strct[0];
    const u8* end = p + esize;
    while (p != end) {
        const apic_tbl_entry_hdr_t* h = (apic_tbl_entry_hdr_t*)p;
        switch (h->type) {
        case MADT_LOCAL_APIC:
            parse_madt_local_apic((apic_local_apic_t*)h);
            break;
        case MADT_IO_APIC:
        case MADT_INT_SRC_OVR:
        case MADT_LOCAL_APIC_NMI:
            break;
        default:
            console_puts("UNHANDLED MADT TYPE");
        }
        p += h->len;
    }
}

void
parse_rsdt(rsdp_descr_t* rsdp)
{
    console_start("Parsing RSDT");
    size_t addr  = rsdp->rsdt_addr;
    rsdt_t* rsdt = (rsdt_t*)addr;
    kdata_get()->acpi.rsdt = rsdt;
    const int num_entries = (rsdt->hdr.len - sizeof(rsdt->hdr)) / 4;
    for (int i = 0; i < num_entries; ++i) {
        addr              = rsdt->strct[i];
        acpi_tbl_hdr_t* h = (acpi_tbl_hdr_t*)addr;
        u32 sig = *((u32*)h->sig);
        switch (sig) {
        case RSDT_SIG_APIC:
            kdata_get()->acpi.apic = (apic_tbl_t*)h;
            break;
        }
    }
    console_ok();
}

void
acpi_init()
{
    console_start("Initializing");
    rsdp_descr_t* rsdp = find_rsdp();
    if (rsdp == NULL) {
        /* We can't proceed, this isn't good */
        PANIC();
    }

    validate_rsdp_cksum(rsdp);
    print_rsdp_info(rsdp);

    parse_rsdt(rsdp);
    kdata_t* kd = kdata_get();
    if (kd->acpi.apic != 0) {
        parse_apic(kd->acpi.apic);
    }
    console_ok();
}
