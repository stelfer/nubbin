unsigned short *video = (unsigned short *)0xB8000; // We could also use the virtual address 0xC00B8000
unsigned char attrib = 0xF; // White text on black background

void gdt_install();
void init_paging();


void func1() {
    int a = 1;
}

int func2() {
    return 2;
}

// Clears the screen
void cls()
{
    int i = 0;
    for (i = 0; i < 80 * 25; i++)
        video[i] = (attrib << 8) | 0;
}

// Prints the welcome message ;)
void helloworld()
{
    char msg[] = "Hello, World!";
    int i;
    for (i = 0; msg[i] != '\0'; i++)
        video[i] = (attrib << 8) | msg[i];
}

void main() {
    // FIRST enable paging and THEN load the real GDT!
    init_paging();
    gdt_install();

    cls();
    helloworld();
    for(;;);
}
