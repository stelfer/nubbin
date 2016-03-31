

#include <nubbin/kernel/serial.h>

int
hello_user()
{
    serial_puts("hello from user");
    return 0;
}
