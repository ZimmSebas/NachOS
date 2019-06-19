/// Simple test case for the threads assignment.
///
/// Create several threads, and have them context switch back and forth
/// between themselves by calling `Thread::Yield`, to illustrate the inner
/// workings of the thread system.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2007-2009 Universidad de Las Palmas de Gran Canaria.
///               2016-2017 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.

#include "synch.hh"
#include "lib/utility.hh"
#include "system.hh"
#include "port.hh"

Port puerto("Parana");

/// * `name` points to a string with a thread name, just for debugging
///   purposes.
void
SimpleThreadSend(void *name_)
{
    // Reinterpret arg `name` as a string.
    int msg = 701;
    int msg2 = 705;
    int msg3 = 723;
	puerto.Send(msg);
	puerto.Send(msg2);
	puerto.Send(msg3);

}
void
SimpleThreadReceive(void *name_)
{
    // Reinterpret arg `name` as a string.
	int valor = 6;
	int valor2 = 21;
	int valor3 = 33;
	puerto.Receive(&valor);
	puerto.Receive(&valor2);
	puerto.Receive(&valor3);
	printf("El valor es %d\n",valor);
	printf("El valor es %d\n",valor2);
	printf("El valor es %d\n",valor3);
}

void
ThreadTest()
{
    DEBUG('t', "Entering thread test\n");

    char *name = new char [64];

    strncpy(name, "2nd", 64);

    Thread *newThread = new Thread(name,true);

    newThread->Fork(SimpleThreadSend, (void *) name);

    SimpleThreadReceive((void *) "1st");
}
