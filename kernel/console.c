/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel/console.h>
#include <nubbin/kernel/serial.h>
#include <nubbin/kernel/string.h>

static int
tag_pos(int incr)
{
    static int ctag = 0;
    if (incr < 0) {
        int save = ctag;
        ctag += incr;
        return save;
    }
    ctag += incr;
    return ctag;
}

static void
write_tag(const char* tag)
{
    static const char* pad = "      |";
    size_t tagsz           = strlen(tag);
    size_t padsz           = tagsz >= 6 ? 2 : 7 - tagsz;

    serial_write(tag, tagsz);
    serial_write(&pad[tagsz], padsz);
}

static void
do_indent(int i)
{
    for (int j = 0; j < i; ++j) {
        serial_putc(' ');
    }
}

static const char*
cur_tag(int i, const char* tag)
{
    static const char* tags[CONSOLE_MAX_TAGS];
    if (tag != NULL) {
        tags[i] = tag;
    }
    return tags[i];
}

static const char*
cur_msg(int i, const char* tag)
{
    static const char* msgs[CONSOLE_MAX_TAGS];
    if (tag == NULL) {
        const char* save = msgs[i];
        msgs[i]          = NULL;
        return save;
    }
    msgs[i] = tag;
    return msgs[i];
}

void
console_putc(char a)
{
    serial_putc(a);
}

void
console_puts(const char* buf)
{
    serial_puts(buf);
}

void
console_put(unsigned long addr, unsigned long len)
{
    serial_put(addr, len);
}
void
console_putf(const char* fmt,
             unsigned long addr,
             unsigned long len,
             unsigned long off)
{
    int i           = tag_pos(0);
    const char* tag = cur_tag(i, NULL);
    write_tag(tag);
    do_indent(i + 1);
    serial_putf(fmt, addr, len, off);
}

void
__console_write(const char* buf, unsigned long len)
{
    serial_write(buf, len);
}

void
console_write(const char* buf, unsigned long len)
{
    /* int i = tag_pos(0); */
    /* const char* tag = cur_tag(i, NULL); */
    /* write_tag(tag); */
    /* do_indent(i + 1); */
    serial_write(buf, len);
}

void
console_start_tagged(const char* tag, const char* msg)
{
    int i = tag_pos(1);
    cur_tag(i, tag);
    cur_msg(i, msg);
    write_tag(tag);
    do_indent(i);
    serial_write(msg, strlen(msg));
    serial_puts(" [..]");
}

void
console_finish(const char* status)
{
    int i           = tag_pos(-1);
    const char* tag = cur_tag(i, NULL);
    const char* msg = cur_msg(i, NULL);

    write_tag(tag);
    do_indent(i);
    serial_write(msg, strlen(msg));
    serial_write(" [", 2);
    serial_write(status, strlen(status));
    serial_puts("]");
}
