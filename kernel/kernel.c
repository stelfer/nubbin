/* Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt */

#include <nubbin/kernel.h>

unsigned char attrib = 0x1F;

void
cls()
{
    int i = 0;
    for (i = 0; i < 80 * 25; i++)
        video[i] = (attrib << 8) | 0;
}

void
helloworld()
{
    char msg[] = "Hello, World!";
    int i;
    for (i = 0; msg[i] != '\0'; i++)
        video[i] = (attrib << 8) | msg[i];
}

void
main()
{
    cls();

    helloworld();

    for (;;)
        ;
}
