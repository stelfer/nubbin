/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/kdata.h>
#include <nubbin/kernel/smbios.h>

CONSOLE_TAG("SMB");

/* static rsdp_descr_t* */
/* find_rsdp_in_mem(size_t start, size_t stop) */
/* { */
/*     for (size_t i = start; i < stop; i += 16) { */
/*         const uint64_t x = *((uint64_t*)i); */
/*         if (x == RSDP_SIG) { */
/*             return (rsdp_descr_t*)i; */
/*         } */
/*     } */
/*     return NULL; */
/* } */

void
parse_entry64(uintptr_t entry)
{
    UNSUPPORTED();
}

void
parse_entry32(uintptr_t entry)
{
    console_start("Parsing SMBios 32-bit entry");

    if (entry == 0) {
        console_puts("ZERO SMBios 32-bit entry address");
        PANIC();
    }
    smbios32_entry_t* e = (smbios32_entry_t*)entry;
    kdata_t* kd         = kdata_get();
    kd->smbios.type     = SMBIOS_TYPE_32;
    kd->smbios.entry32  = e;

    console_putf("ENTRY           = 0x0000000000000000", entry, 8, 20);
    console_putf(
        "ANCHOR          = 0x00000000", *(uint32_t*)&e->anchor[0], 4, 20);
    console_putf("CHECKSUM        = 0x00", e->cksum, 1, 20);
    console_putf("LENGTH          = 0x00", e->len, 1, 20);
    console_putf("MAJ VER         = 0x00", e->maj_ver, 1, 20);
    console_putf("MIN VER         = 0x00", e->min_ver, 1, 20);
    console_putf("MAX_STRUCT_SZ   = 0x0000", e->max_struct_sz, 2, 20);
    console_putf("REV             = 0x00", e->rev, 1, 20);
    console_putf("STRUCT_TBL_LEN  = 0x0000", e->struct_tbl_len, 2, 20);
    console_putf("STRUCT_TBL_ADDR = 0x00000000", e->struct_tbl_addr, 4, 20);
    console_putf("NUM_STRUCT      = 0x0000", e->num_struct, 2, 20);

    uintptr_t off   = (uintptr_t)e->struct_tbl_addr;
    uintptr_t nread = 0;
    for (uint16_t i = 0; i < e->num_struct; ++i) {
        smbios_hdr_t* h = (smbios_hdr_t*)off;
        console_putf("i               = 0x0000", i, 2, 20);
        console_putf("HDR_TYPE        = 0x00", h->type, 1, 20);
        console_putf("HDR_LEN         = 0x00", h->len, 1, 20);
        console_putf("HDR_HANDLE      = 0x0000", h->handle, 2, 20);
        off += h->len;
        nread += h->len;
    }
    console_putq(nread);
    console_putw(e->struct_tbl_len);

    HALT();
    console_ok();
}

void
smbios_init()
{
    console_start("Initializing");
    uintptr_t entry = 0;
    uint8_t status  = smbios_find_entry(&entry);
    switch (status) {
    case SMBIOS_TYPE_UNSPEC:
        console_puts("No SMBIOS entry point");
        PANIC();
        break;
    case SMBIOS_TYPE_32:
        parse_entry32(entry);
        break;
    case SMBIOS_TYPE_64:
        parse_entry64(entry);
    default:
        PANIC();
    }
    HALT();
    console_ok();
}
