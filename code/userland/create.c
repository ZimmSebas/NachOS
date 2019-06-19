#include "syscall.h"

int
main(void)
{
    OpenFileId id;
    char s[10];
    
    Create("Ale");
    Create("Seba");
    // File created
    id = Open("Ale");
    Write("VAMO LOCO!",10,id);
    Close(id);
    id = Open("Ale");
    Read(s,10,id);
    Close(id);
    id = Open("Seba");
    Write(s,10,id);
    Close(id);
	Exit(0);
    
}
