

#include <nubbin/kernel.h>

unsigned short *video = (unsigned short *)0xB8000;
unsigned char attrib = 0xF;

void gdt_install();
void init_paging();


void func1() {
}

int func2() {
    return 2;
}

void cls()
{
    int i = 0;
    for (i = 0; i < 80 * 25; i++)
        video[i] = (attrib << 8) | 0;
}

void helloworld()
{
    char msg[] = "Hello, World!";
    int i;
    for (i = 0; msg[i] != '\0'; i++)
        video[i] = (attrib << 8) | msg[i];
}

void main() {
     /* FIRST enable paging and THEN load the real GDT! */
    init_paging();
    gdt_install();

    cls();
    helloworld();
    for(;;);
}
