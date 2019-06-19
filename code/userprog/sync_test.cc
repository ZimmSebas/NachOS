/// Test routines for demonstrating that Nachos can load a user program and
/// execute it.
///
/// Also, routines for testing the SynchConsole hardware device.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2017 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "address_space.hh"
#include "threads/synch.hh"
#include "threads/system.hh"
#include "synch_console.hh"


/// Run a user program.
///
/// Open the executable, load it into memory, and jump to it.
void
SynchStartProcess(const char *filename)
{
    ASSERT(filename != nullptr);

    OpenFile *executable = fileSystem->Open(filename);
    if (executable == nullptr) {
        printf("Unable to open file %s\n", filename);
        return;
    }

    AddressSpace *space = new AddressSpace(executable);
    currentThread->space = space;

    delete executable;

    space->InitRegisters();  // Set the initial register values.
    space->RestoreState();   // Load page table register.

    machine->Run();  // Jump to the user progam.
    ASSERT(false);   // `machine->Run` never returns; the address space
                     // exits by doing the system call `Exit`.
}

/// Data structures needed for the console test.
///
/// Threads making I/O requests wait on a `Semaphore` to delay until the I/O
/// completes.

static SynchConsole *console;


/// Test the console by echoing characters typed at the input onto the
/// output.
///
/// Stop when the user types a `q`.
void
SynchConsoleTest()
{
    console   = new SynchConsole("Holis");

    for (;;) {
        char ch;
        console->ReadChar(&ch);
        console->WriteChar(&ch);  // Echo it!

        if (ch == 'q')
            return;  // If `q`, then quit.
    }
}
