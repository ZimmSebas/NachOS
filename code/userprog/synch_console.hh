#ifndef NACHOS_MACHINE_SYNCH_CONSOLE__HH
#define NACHOS_MACHINE_SYNCH_CONSOLE__HH

#include "machine/console.hh"
#include "threads/synch.hh"


class SynchConsole{

public:	
	SynchConsole(const char *n);
	
	~SynchConsole();
    
    void ReadChar(char *data);

    void WriteChar(const char *data);
    
    void ReadAvail();

    void WriteDone();
    
    void ReadBuffer(char *buffer, unsigned size);
    
    void WriteBuffer(char *buffer, unsigned size);

    const char* GetName();

private:
	Console *consola;
	Lock *lock;
	Semaphore *readAvail;
	Semaphore *writeDone;

  const char *name;
	
};
#endif
