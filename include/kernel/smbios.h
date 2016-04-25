/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _SMBIOS_H
#define _SMBIOS_H

#include <nubbin/kernel/intrinsics.h>
#include <nubbin/kernel/stddef.h>

enum { SMBIOS32_ENTRY_SIZE = 0x1F };

enum { SMBIOS_PROC_INFO_SIZE = 0x30 };

enum { SMBIOS_TYPE_UNSPEC = 0, SMBIOS_TYPE_32 = 1, SMBIOS_TYPE_64 = 2 };

enum {
    SMBIOS_INFO_BIOS         = 0,
    SMBIOS_INFO_SYSTEM       = 1,
    SMBIOS_INFO_ENCLOSURE    = 2,
    SMBIOS_INFO_PROC         = 4,
    SMBIOS_INFO_CACHE        = 7,
    SMBIOS_INFO_SYSTEM_SLOTS = 9,
    SMBIOS_INFO_MEM_PHY      = 16,
    SMBIOS_INFO_MEM_DEV      = 17,
    SMBIOS_INFO_MEM_MAP      = 19,
    SMBIOS_INFO_BOOT         = 32,
    SMBIOS_INFO_END          = 127
};

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

struct smbios_proc_info {
    uint8_t type;
    uint8_t len;
    uint16_t handle;
    uint8_t socket;
    uint8_t proc_type;
    uint8_t proc_family;
    uint8_t proc_manufacturer;
    uint64_t proc_id;
    uint8_t proc_ver;
    uint8_t voltage;
    uint16_t clock_mhz;
    uint16_t max_speed;
    uint16_t cur_speed;
    uint8_t status;
    uint8_t upgrade;
    uint16_t l1_cache_handle;
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;
    uint8_t core_count;
    uint8_t core_enabled;
    uint8_t thread_count;
    uint16_t proc_char;
    uint16_t proc_family2;
    uint16_t proc_count2;
    uint16_t core_enabled2;
    uint16_t thread_count2;
} __packed;
typedef struct smbios_proc_info smbios_proc_info_t;
STATIC_ASSERT(sizeof(smbios_proc_info_t) == SMBIOS_PROC_INFO_SIZE);

void smbios_init();

uintptr_t smbios_find_entry(uintptr_t* entry_point);

#endif /* _SMBIOS_H */
