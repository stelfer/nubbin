/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/acpi.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/kdata.h>
#include <nubbin/kernel/string.h>

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
    if (rsdp != NULL) {
        console_putf(
            "RSDP    = 0x0000000000000000", (unsigned long)rsdp, 8, 12);
        console_putf("VERSION = 0x00", rsdp->rev + 1, 1, 12);
    }
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
parse_madt_lapic(madt_lapic_t* h)
{
    console_start("Parsing MADT LAPIC entry");

    uint32_t cpu_id = cpu_id_from_apic_id(h->apic_id);
    kdata_t* kd     = kdata_get();
    if (kd->cpu.info[cpu_id].apic_id != h->apic_id) {
        console_puts("Something wrong here, apic ids don't line up");
        PANIC();
    }
    cpu_kdata_info_t* info = &kd->cpu.info[cpu_id];
    info->acpi_proc_id     = h->acpi_proc_id;
    info->status |= CPU_STAT_APIC_ENABLED;
    ++kd->cpu.num_cpus;

    console_putf("APIC_ID      = 0x00", h->apic_id, 1, 17);
    console_putf("ACPI_PROC_ID = 0x00000000", h->acpi_proc_id, 4, 17);
    console_putf("FLAGS        = 0x00", h->flags, 1, 17);
    console_ok();
}

void
parse_madt_ioapic(madt_ioapic_t* h)
{
    console_start("Parsing MADT IOAPIC entry");

    if (h->id > IOAPIC_MAX_IOAPICS) {
        console_puts("IOAPIC_MAX_IOAPICS exceeded");
        PANIC();
    }

    kdata_t* kd                        = kdata_get();
    kd->ioapic.entries[h->id].id       = h->id;
    kd->ioapic.entries[h->id].addr     = h->addr;
    kd->ioapic.entries[h->id].gsi_base = h->gsi_base;

    console_putf("IOAPIC_ID    = 0x00", h->id, 1, 17);
    console_putf("ADDR         = 0x00000000", h->addr, 4, 17);
    console_putf("GSIB         = 0x00000000", h->gsi_base, 4, 17);
    console_ok();
}

void
parse_madt(madt_tbl_t* tbl)
{
    console_start("Parsing MADT");
    const size_t esize = madt_tbl_size(tbl);
    const uint8_t* p   = &tbl->strct[0];
    const uint8_t* end = p + esize;
    while (p != end) {
        const madt_tbl_entry_hdr_t* h = (madt_tbl_entry_hdr_t*)p;
        switch (h->type) {
        case MADT_LAPIC:
            parse_madt_lapic((madt_lapic_t*)h);
            break;
        case MADT_IOAPIC:
            parse_madt_ioapic((madt_ioapic_t*)h);
            break;
        case MADT_INT_SRC_OVR:
        case MADT_LAPIC_NMI:
            break;
        default:
            console_puts("UNHANDLED MADT TYPE");
        }
        p += h->len;
    }
    console_ok();
}

void
parse_srat_lapic_affnty(srat_tbl_lapic_affnty_entry_t* h)
{
    console_start("Parsing LAPIC affinity entry");

    uint32_t domain        = (h->domain_high << 7) | h->domain_low;
    uint8_t apic_id        = h->apic_id;
    uint32_t cpu_id        = cpu_id_from_apic_id(apic_id);
    kdata_t* kd            = kdata_get();
    cpu_kdata_info_t* info = &kd->cpu.info[cpu_id];
    info->id               = cpu_id;
    info->apic_id          = apic_id;
    info->domain           = domain;

    console_putf("DOMAIN     = 0x00000000", domain, 4, 15);
    console_putf("APIC_ID    = 0x00", apic_id, 1, 15);
    console_putf("CLK_DOMAIN = 0x00000000", h->clk_domain, 4, 15);
    console_putf("SAPICID    = 0x00", h->sapic_eid, 1, 15);
    console_ok();
}

void
parse_srat_mem_affnty(srat_tbl_mem_affnty_entry_t* h)
{
    console_start("Parsing Memory affinity entry");

    uint64_t base   = (h->base_high << 31) | h->base_low;
    uint64_t length = (h->length_high) << 31 | h->length_low;

    console_putf("DOMAIN     = 0x00000000", h->domain, 4, 15);
    console_putf("BASE       = 0x0000000000000000", base, 8, 15);
    console_putf("LENGTH     = 0x0000000000000000", length, 8, 15);
    console_putf("FLAGS      = 0x00000000", h->flags, 4, 15);
    console_ok();
}

void
parse_srat(srat_tbl_t* srat)
{
    console_start("Parsing SRAT");
    const size_t esize = srat_tbl_size(srat);

    const uint8_t* p   = &srat->strct[0];
    const uint8_t* end = p + esize;
    while (p != end) {
        const srat_tbl_common_entry_t* h = (srat_tbl_common_entry_t*)p;
        switch (h->type) {
        case SRAT_LAPIC_AFFNTY:
            parse_srat_lapic_affnty((srat_tbl_lapic_affnty_entry_t*)p);
            break;
        case SRAT_MEM_AFFNTY:
            parse_srat_mem_affnty((srat_tbl_mem_affnty_entry_t*)p);
            break;
        case SRAT_LAPIC_X2_AFFNTY:
            UNSUPPORTED();
            /* TODO handle X2 stuff */
            break;
        default:
            console_puts("Bad SRAT TBL TYPE");
            PANIC();
        }
        p += h->length;
    }
    console_ok();
}

void
parse_rsdt(rsdp_descr_t* rsdp)
{
    console_start("Parsing RSDT");
    size_t addr            = rsdp->rsdt_addr;
    rsdt_t* rsdt           = (rsdt_t*)addr;
    kdata_get()->acpi.rsdt = rsdt;
    const int num_entries  = (rsdt->hdr.len - sizeof(rsdt->hdr)) / 4;
    for (int i = 0; i < num_entries; ++i) {
        addr              = rsdt->strct[i];
        acpi_tbl_hdr_t* h = (acpi_tbl_hdr_t*)addr;
        uint32_t sig      = *((uint32_t*)h->sig);
        switch (sig) {
        case RSDT_SIG_MADT:
            kdata_get()->acpi.madt = (madt_tbl_t*)h;
            break;
        case RSDT_SIG_SRAT:
            kdata_get()->acpi.srat = (srat_tbl_t*)h;
            break;
        case RSDT_SIG_SLIT:
            /* BUG, we wan't to support this */
            UNSUPPORTED();
            break;
        default:
            console_putf("Unhandled RSDT_SIG 0x00000000", sig, 4, 21);
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
    if (kd->acpi.srat != 0) {
        parse_srat(kd->acpi.srat);
    }

    if (kd->acpi.madt != 0) {
        parse_madt(kd->acpi.madt);
    }
    console_ok();
}
