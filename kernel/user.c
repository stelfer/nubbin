

#include <nubbin/kernel/console.h>

CONSOLE_TAG("USER");

int
hello_user()
{
    console_start("Starting");
    console_ok();
    return 0;
}
