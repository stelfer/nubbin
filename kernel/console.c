/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>
#include <nubbin/kernel/console.h>

static unsigned short* video      = (unsigned short*)0x000b8000;
static unsigned int cursor        = 0;
static const unsigned char attrib = 0x1F;

void
console_clear()
{
    cursor = 0;
    int i = 0;
    for (i = 0; i < 80 * 25; i++)
        video[i] = (attrib << 8) | ' ';
}

void
console_puts(const char* msg)
{
    unsigned long off = cursor;
    unsigned long i = 0;
    for (; msg[i] != '\0'; i++) {
        video[cursor + i] = (attrib << 8) | msg[i];
    }
    /* Add newline */
    cursor += i;
    cursor = (cursor / 80 + 1) * 80;
}
