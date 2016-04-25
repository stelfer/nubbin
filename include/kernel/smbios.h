/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _SMBIOS_H
#define _SMBIOS_H

#include <nubbin/kernel/intrinsics.h>
#include <nubbin/kernel/stddef.h>

enum { SMBIOS32_ENTRY_SIZE = 0x1F };

enum { SMBIOS_TYPE_UNSPEC = 0, SMBIOS_TYPE_32 = 1, SMBIOS_TYPE_64 = 2 };

struct smbios32_entry {
    char anchor[4];
    uint8_t cksum;
    uint8_t len;
    uint8_t maj_ver;
    uint8_t min_ver;
    uint16_t max_struct_sz;
    uint8_t rev;
    char fmt_area[5];
    char intrm_anchor[5];
    uint8_t intrm_cksum;
    uint16_t struct_tbl_len;
    uint32_t struct_tbl_addr;
    uint16_t num_struct;
    uint8_t bcd_rev;
} __packed;
typedef struct smbios32_entry smbios32_entry_t;
STATIC_ASSERT(sizeof(smbios32_entry_t) == SMBIOS32_ENTRY_SIZE);

struct smbios_hdr {
    uint8_t type;
    uint8_t len;
    uint16_t handle;
} __packed;
typedef struct smbios_hdr smbios_hdr_t;

struct smbios_kdata {
    uint8_t type;
    smbios32_entry_t* entry32;
};
typedef struct smbios_kdata smbios_kdata_t;

void smbios_init();

uintptr_t smbios_find_entry(uintptr_t* entry_point);

#endif /* _SMBIOS_H */
