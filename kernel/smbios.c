/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>
#include <nubbin/kernel/kdata.h>
#include <nubbin/kernel/smbios.h>

CONSOLE_TAG("SMB");

static void
parse_entry64(uintptr_t entry)
{
    UNSUPPORTED();
}

static void
handle_string(const char* string)
{
    /* console_puts(strings); */
}

static uintptr_t
extract_strings(uintptr_t off, uintptr_t off_max)
{
    const char* strings = (const char*)((char*)off);
    while (off < off_max) {
        const char* c = (const char*)(off);
        if (*c == 0) {
            handle_string(strings);
            strings = c + 1;
            if (*strings == 0) {
                off += 1;
                break;
            }
        }
        off += 1;
    }
    if (off == off_max) {
        console_puts("ZOMETHING BAD HERE");
        PANIC();
    }
    return off + 1;
}

static uintptr_t
handle_bios_info(smbios_hdr_t* h)
{
    return h->len;
}

static uintptr_t
handle_proc_info(smbios_hdr_t* h)
{
    smbios_proc_info_t* i = (smbios_proc_info_t*)h;
    console_putf("SOCKET          = 0x00", i->socket, 1, 20);
    console_putf("CLOCK_MHZ       = 0x0000", i->clock_mhz, 2, 20);
    console_putf("CLOCK_MHZ       = 0x0000", i->clock_mhz, 2, 20);
    console_putf("MAX_SPEED       = 0x0000", i->max_speed, 2, 20);
    console_putf("CUR_SPEED       = 0x0000", i->cur_speed, 2, 20);
    console_putf("STATUS          = 0x00", i->status, 1, 20);
    console_putf("CORES           = 0x0000", i->core_count, 1, 20);
    console_putf("CORES_ENABLED   = 0x0000", i->core_enabled, 1, 20);
    console_putf("THREAD_COUNT    = 0x0000", i->thread_count, 1, 20);
    return h->len;
}

static void
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

    uintptr_t off           = (uintptr_t)e->struct_tbl_addr;
    const uintptr_t off_max = (uintptr_t)(off + e->struct_tbl_len);
    int found_end           = 0;
    for (uint16_t i = 0; i < e->num_struct; ++i) {
        smbios_hdr_t* h = (smbios_hdr_t*)off;
        console_putf("OFF             = 0x0000000000000000", off, 8, 20);
        console_putf("TYPE            = 0x00", h->type, 1, 20);
        console_putf("LEN             = 0x00", h->len, 1, 20);
        console_putf("HANDLE          = 0x0000", h->handle, 2, 20);
        switch (h->type) {
        case SMBIOS_INFO_BIOS:
            off += handle_bios_info(h);
            break;
        case SMBIOS_INFO_PROC:
            off += handle_proc_info(h);
            break;
        case SMBIOS_INFO_END:
            found_end = 1;
        default:
            off += h->len;
            break;
        }
        off = extract_strings(off, off_max);
    }
    if (!found_end) {
        console_puts("Error Parsing, SMBIOS Table, no table end");
        PANIC();
    }
    if (off != off_max) {
        console_puts("Error Parsing, bytes mismatched");
        console_putq(off);
        console_putq(off_max);
        PANIC();
    }
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
    console_ok();
}
