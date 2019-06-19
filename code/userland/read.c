#include "syscall.h"


int
main(void)
{
    char s[20];
    Read(s,20,CONSOLE_INPUT);
    Write(s,20,CONSOLE_OUTPUT);
    Halt();
}
