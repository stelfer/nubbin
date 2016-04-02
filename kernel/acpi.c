/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/acpi.h>
#include <nubbin/kernel/serial.h>
#include <nubbin/kernel/string.h>

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
    const char* found_rsdp_msg = "ACPI: RSDP = 0x0000000000000000";
    serial_putf(found_rsdp_msg, (unsigned long)rsdp, 8, 15);
    const char* acpi_ver_msg = "ACPI: version = 0x00";
    serial_putf(acpi_ver_msg, rsdp->rev + 1, 1, 18);
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
        if (!memcmp(h->sig, name, 4)) {
            return h;
        }
    }
    return NULL;
}

void
parse_madt_local_apic(apic_local_apic_t* h)
{
    serial_putb(h->acpi_proc_id);
    serial_putb(h->apic_id);
    serial_putd(h->flags);
}

void
parse_apic(rsdp_descr_t* rsdp)
{
    apic_tbl_t* apic = (apic_tbl_t*)find_tbl(rsdp, "APIC");
    if (apic == NULL) {
        PANIC();
    }
    const size_t esize = (apic->hdr.len - sizeof(apic->hdr) -
                          sizeof(apic->lca) - sizeof(apic->flags));
    const char* p   = &apic->strct[0];
    const char* end = p + esize;
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
            serial_puts("UNHANDLED MADT TYPE");
        }
        p += h->len;
    }
    serial_puts("H");
}

void
acpi_init()
{
    rsdp_descr_t* rsdp = find_rsdp();
    if (rsdp == NULL) {
        /* We can't proceed, this isn't good */
        PANIC();
    }

    validate_rsdp_cksum(rsdp);
    print_rsdp_info(rsdp);
    parse_apic(rsdp);
}
