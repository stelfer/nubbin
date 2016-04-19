/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#ifndef _ACPI_H
#define _ACPI_H

#include <nubbin/kernel/intrinsics.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/stddef.h>

#define RSDP_SIG 0x2052545020445352

struct rsdp_descr {
    unsigned char sig[8];
    unsigned char cksum;
    unsigned char oem_id[6];
    unsigned char rev;
    uint32_t rsdt_addr;
} __packed;
typedef struct rsdp_descr rsdp_descr_t;

struct acpi_tbl_hdr {
    unsigned char sig[4];
    uint32_t len;
    unsigned char rev;
    unsigned char cksum;
    unsigned char oem_id[6];
    unsigned char oem_tbl_id[8];
    uint32_t oem_rev;
    uint32_t creator_id;
    uint32_t creator_rev;
} __packed;
typedef struct acpi_tbl_hdr acpi_tbl_hdr_t;

struct rsdt {
    acpi_tbl_hdr_t hdr;
    uint32_t strct[];
} __packed;
typedef struct rsdt rsdt_t;

struct apic_tbl {
    acpi_tbl_hdr_t hdr;
    uint32_t lca;
    uint32_t flags;
    unsigned char strct[];
} __packed;
typedef struct apic_tbl apic_tbl_t;

static inline size_t
apic_tbl_size(apic_tbl_t* apic)
{
    return (apic->hdr.len - sizeof(apic->hdr) - sizeof(apic->lca) -
            sizeof(apic->flags));
}

struct srat_tbl {
    acpi_tbl_hdr_t hdr;
    uint32_t res1;
    uint64_t res2;
    unsigned char strct[];
} __packed;
typedef struct srat_tbl srat_tbl_t;

static inline size_t
srat_tbl_size(srat_tbl_t* srat)
{
    return (srat->hdr.len - sizeof(srat->hdr) - sizeof(srat->res1) -
            sizeof(srat->res2));
}

struct srat_tbl_lapic_affnty_entry {
    uint8_t type;
    uint8_t length;
    uint8_t domain_low;
    uint8_t apic_id;
    uint32_t flags;
    uint8_t sapic_eid;
    uint32_t domain_high : 24;
    uint32_t clk_domain;
} __packed;
typedef struct srat_tbl_lapic_affnty_entry srat_tbl_lapic_affnty_entry_t;

struct srat_tbl_mem_affnty_entry {
    uint8_t type;
    uint8_t length;
    uint32_t domain;
    uint16_t res1;
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t res2;
    uint32_t flags;
    uint64_t res3;
} __packed;
typedef struct srat_tbl_mem_affnty_entry srat_tbl_mem_affnty_entry_t;

enum {
    SRAT_MEM_ENABLED       = (1 << 0),
    SRAT_MEM_HOT_PLUGGABLE = (1 << 1),
    SRAT_MEM_NVM           = (1 << 2)
};

struct srat_tbl_common_entry {
    uint8_t type;
    uint8_t length;
} __packed;
typedef struct srat_tbl_common_entry srat_tbl_common_entry_t;

enum { SRAT_LAPIC_AFFNTY = 0, SRAT_MEM_AFFNTY = 1, SRAT_LAPIC_X2_AFFNTY = 2 };

struct apic_tbl_entry_hdr {
    unsigned char type;
    unsigned char len;
} __packed;
typedef struct apic_tbl_entry_hdr apic_tbl_entry_hdr_t;

typedef unsigned char apic_id_t;
typedef unsigned char acpi_proc_id_t;

struct apic_local_apic {
    apic_tbl_entry_hdr_t hdr;
    acpi_proc_id_t acpi_proc_id;
    apic_id_t apic_id;
    uint32_t flags;
} __packed;
typedef struct apic_local_apic apic_local_apic_t;

enum { ACPI_LOCAL_APIC_ENABLED = 1 };

struct apic_io_apic {
    apic_tbl_entry_hdr_t hdr;
    unsigned char io_apic_id;
    unsigned char reserved;
    uint32_t io_apic_addr;
    uint32_t gsi_base;
} __packed;
typedef struct apic_io_apic apic_io_apic_t;

struct apic_iso {
    apic_tbl_entry_hdr_t hdr;
    unsigned char bus_src;
    unsigned char irq_src;
    uint32_t gsi;
    uint16_t flags;
} __packed;
typedef struct apic_iso apic_iso_t;

typedef enum {
    MADT_LOCAL_APIC,
    MADT_IO_APIC,
    MADT_INT_SRC_OVR,
    MADT_NMI_SRC,
    MADT_LOCAL_APIC_NMI,
    MADT_LOCAL_APIC_ADDR_OVR,
    MADT_IO_SAPIC,
    MADT_LOCAL_SAPIC,
    MADT_PLAT_INTR_SRCS,
    MADT_X2APIC_NMI
} madt_apic_type_t;

struct acpi_kdata {
    rsdt_t* rsdt;
    apic_tbl_t* apic;
    srat_tbl_t* srat;
} __packed;
typedef struct acpi_kdata acpi_kdata_t;

void acpi_init();

/* hex(reduce(lambda x,y: (x << 8) + y, map(lambda x: ord(x), "HPET")[::-1])) */
#define RSDT_SIG_APIC 0x43495041
#define RSDT_SIG_SSDT 0x54445353
#define RSDT_SIG_HPET 0x54455048
#define RSDT_SIG_FACP 0x50434146
#define RSDT_SIG_SRAT 0x54415253
#define RSDT_SIG_SLIT 0x54494c53
#endif /* _ACPI_H */
