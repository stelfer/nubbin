/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _CONSOLE_H
#define _CONSOLE_H

#define CONSOLE_MAX_TAGS 10

#define CONSOLE_TAG(x) static const char* CONSOLE_TAG = x

void console_start_tagged(const char* tag, const char* msg);

void console_finish(const char* status);

#define console_start(msg) console_start_tagged(CONSOLE_TAG, msg)
#define console_ok() console_finish("OK")

void console_putc(char a);

void console_puts(const char* buf);

void console_put(unsigned long addr, unsigned long len);
void console_putf(const char* fmt,
                  unsigned long addr,
                  unsigned long len,
                  unsigned long off);

void console_write(const char*, unsigned long len);

#define console_putb(x) console_put((x), 1)
#define console_putw(x) console_put((x), 2)
#define console_putd(x) console_put((x), 4)
#define console_putq(x) console_put((x), 8)

#endif /* _CONSOLE_H */
