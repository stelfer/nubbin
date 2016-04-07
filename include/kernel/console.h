/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */
#ifndef _CONSOLE_H
#define _CONSOLE_H

#define CONSOLE_MAX_TAGS 10

#define CONSOLE_TAG(x) static const char* CONSOLE_TAG = x

void __console_write(const char*, unsigned long len);

#define CONSOLE_COLOR_FN(name)                  \
    static inline void console_##name()         \
    {                                           \
        static const char code[] = name##_CODE; \
        __console_write(code, sizeof(code));    \
    }

#define bold_CODE                \
    {                            \
        0x1b, '[', '0', '1', 'm' \
    }
CONSOLE_COLOR_FN(bold)

#define reset_CODE          \
    {                       \
        0x1b, '[', '0', 'm' \
    }
CONSOLE_COLOR_FN(reset)

#define teal_CODE                               \
    {                                           \
        0x1b, '[', '3', '6', ';', '0', '6', 'm' \
    }
CONSOLE_COLOR_FN(teal)

#define turquoise_CODE                          \
    {                                           \
        0x1b, '[', '3', '6', ';', '0', '1', 'm' \
    }
CONSOLE_COLOR_FN(turquoise)

#define fuscia_CODE                             \
    {                                           \
        0x1b, '[', '3', '5', ';', '0', '1', 'm' \
    }
CONSOLE_COLOR_FN(fuscia)

#define purple_CODE                             \
    {                                           \
        0x1b, '[', '3', '5', ';', '0', '6', 'm' \
    }
CONSOLE_COLOR_FN(purple)

#define blue_CODE                               \
    {                                           \
        0x1b, '[', '3', '4', ';', '0', '1', 'm' \
    }
CONSOLE_COLOR_FN(blue)

#define darkblue_CODE                           \
    {                                           \
        0x1b, '[', '3', '4', ';', '0', '6', 'm' \
    }
CONSOLE_COLOR_FN(darkblue)

#define green_CODE                              \
    {                                           \
        0x1b, '[', '3', '2', ';', '0', '1', 'm' \
    }
CONSOLE_COLOR_FN(green)

#define darkgreen_CODE                          \
    {                                           \
        0x1b, '[', '3', '2', ';', '0', '6', 'm' \
    }
CONSOLE_COLOR_FN(darkgreen)

#define yellow_CODE                             \
    {                                           \
        0x1b, '[', '3', '3', ';', '0', '1', 'm' \
    }
CONSOLE_COLOR_FN(yellow)

#define brown_CODE                              \
    {                                           \
        0x1b, '[', '3', '3', ';', '0', '6', 'm' \
    }
CONSOLE_COLOR_FN(brown)

#define red_CODE                                \
    {                                           \
        0x1b, '[', '3', '1', ';', '0', '1', 'm' \
    }
CONSOLE_COLOR_FN(red)

#define darkred_CODE                            \
    {                                           \
        0x1b, '[', '3', '1', ';', '0', '6', 'm' \
    }
CONSOLE_COLOR_FN(darkred)

#undef CONSOLE_COLOR_FN

void console_start_tagged(const char* tag, const char* msg);

void console_finish(const char* status);

#define console_start(msg) console_start_tagged(CONSOLE_TAG, msg)
static inline void
console_ok()
{
    console_green();
    console_finish("OK");
    console_reset();
}

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
