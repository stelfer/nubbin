// Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt

#ifndef _KERNEL_H
#define _KERNEL_H

#define PANIC() __asm__("int $3\n")

#define HALT() __asm__("hlt\n");

#ifndef VERSION
#define VERSION "VERSION"
#endif
#endif /* _KERNEL_H */
