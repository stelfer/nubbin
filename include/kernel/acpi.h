/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#ifndef _ACPI_H
#define _ACPI_H

#include <nubbin/kernel/stddef.h>
#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/intrinsics.h>

#define RSDP_SIG 0x2052545020445352

struct rsdp_descr {
    u8 sig[8];
    u8 cksum;
    u8 oem_id[6];
    u8 rev;
    u32 rsdt_addr;
} __packed;
typedef struct rsdp_descr rsdp_descr_t;

struct acpi_tbl_hdr {
    u8 sig[4];
    u32 len;
    u8 rev;
    u8 cksum;
    u8 oem_id[6];
    u8 oem_tbl_id[8];
    u32 oem_rev;
    u32 creator_id;
    u32 creator_rev;
} __packed;
typedef struct acpi_tbl_hdr acpi_tbl_hdr_t;

struct rsdt {
    acpi_tbl_hdr_t hdr;
    u32 strct[];
} __packed;
typedef struct rsdt rsdt_t;

struct apic_tbl {
    acpi_tbl_hdr_t hdr;
    u32 lca;
    u32 flags;
    u8 strct[];
} __packed;
typedef struct apic_tbl apic_tbl_t;

struct apic_tbl_entry_hdr {
    u8 type;
    u8 len;
} __packed;
typedef struct apic_tbl_entry_hdr apic_tbl_entry_hdr_t;

typedef u8 apic_id_t;
typedef u8 acpi_proc_id_t;

struct apic_local_apic {
    apic_tbl_entry_hdr_t hdr;
    acpi_proc_id_t acpi_proc_id;
    apic_id_t apic_id;
    u32 flags;
} __packed;
typedef struct apic_local_apic apic_local_apic_t;

enum { ACPI_LOCAL_APIC_ENABLED = 1 };

struct apic_io_apic {
    apic_tbl_entry_hdr_t hdr;
    u8 io_apic_id;
    u8 reserved;
    u32 io_apic_addr;
    u32 gsi_base;
} __packed;
typedef struct apic_io_apic apic_io_apic_t;

struct apic_iso {
    apic_tbl_entry_hdr_t hdr;
    u8 bus_src;
    u8 irq_src;
    u32 gsi;
    u16 flags;
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
} __packed;
typedef struct acpi_kdata acpi_kdata_t;

void acpi_init();

/* hex(reduce(lambda x,y: (x << 8) + y, map(lambda x: ord(x), "HPET")[::-1])) */
#define RSDT_SIG_APIC 0x43495041
#define RSDT_SIG_SSDT 0x54445353
#define RSDT_SIG_HPET 0x54455048
#define RSDT_SIG_FACP 0x50434146

#endif /* _ACPI_H */
