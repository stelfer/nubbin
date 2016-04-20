// Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt

#ifndef _IOAPIC_H
#define _IOAPIC_H

#include <nubbin/kernel/stddef.h>

enum { IOAPIC_MAX_IOAPICS = 2 };

struct ioapic_kdata_entry {
    uint8_t id;
    uint8_t reserved;
    uint32_t addr;
    uint32_t gsi_base;
} __packed;
typedef struct ioapic_kdata_entry ioapic_kdata_entry_t;

struct ioapic_kdata {
    ioapic_kdata_entry_t entries[IOAPIC_MAX_IOAPICS];
};
typedef struct ioapic_kdata ioapic_kdata_t;

#endif /* _IOAPIC_H */
