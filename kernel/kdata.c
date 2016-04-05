/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel/kdata.h>

#include <nubbin/kernel/memory.h>
#include <nubbin/kernel/string.h>

void
kdata_init()
{
    kdata_t* kd = kdata_get();
    bzero(kd, KDATA_SIZE);
    kd->magic = KDATA_MAGIC;
}
