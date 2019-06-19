#ifndef NACHOS_PORT__HH
#define NACHOS_PORT__HH

#include "synch.hh"
#include "system.hh"
#include "synch_list.hh"



class Port {
public:

    // Constructor: indicate which lock the condition variable belongs to.
    Port (const char *debugName);

    ~Port();

    const char *GetName() const;

    /// The three operations on condition variables.
    ///
    /// The thread that invokes any of these operations must hold the
    /// corresponding lock; otherwise an error must occur.

	void Send(int message);
	void Receive(int *message);

private:

    const char *name;
	SynchList<int> *portlist; 
	Semaphore *portsem;

    // Other needed fields are to be added here.
};

#endif

