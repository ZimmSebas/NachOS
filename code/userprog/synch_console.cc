#include "synch_console.hh"

static void
readHandler(void *arg)
{
    ASSERT(arg != nullptr);
    SynchConsole *console = (SynchConsole *) arg;
    console -> ReadAvail();
}

static void
writeHandler(void *arg)
{
    ASSERT(arg != nullptr);
    SynchConsole *console = (SynchConsole *) arg;
    console -> WriteDone();
}

void
SynchConsole::ReadAvail(){
    readAvail->V();
}

void
SynchConsole::WriteDone(){
    writeDone->V();
}

const char*
SynchConsole::GetName(){
    return name;
}

SynchConsole::SynchConsole(const char *n){
    
    readAvail = new Semaphore("read synch consola",0);
    writeDone = new Semaphore("write synch consola",0);
    lock = new Lock("synch consola");
    consola = new Console(nullptr,nullptr,readHandler,writeHandler,this);
    name = n;
}

SynchConsole::~SynchConsole(){
	delete readAvail;
	delete writeDone;
	delete lock;
	delete consola;
}


void
SynchConsole::ReadChar(char *data)
{
    ASSERT(data != nullptr);
    readAvail->P();
    lock->Acquire();  // Only one SynchConsole I/O at a time.
    *data = consola->GetChar();
    lock->Release();
}
void
SynchConsole::WriteChar(const char *data)
{
    ASSERT(data != nullptr);
    lock->Acquire();  // only one disk I/O at a time
    consola->PutChar(*data);
    writeDone->P();
    lock->Release();
}

void
SynchConsole::ReadBuffer(char *buffer, unsigned size){
	for (unsigned i = 0;i < size;i++) {
        this->ReadChar(buffer+i);
	}
}

void
SynchConsole::WriteBuffer(char *buffer, unsigned size){
	for (unsigned i = 0;i < size;i++) {
        this->WriteChar(buffer+i);
	}
}
