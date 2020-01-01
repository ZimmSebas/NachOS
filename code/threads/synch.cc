/// Routines for synchronizing threads.
///
/// Three kinds of synchronization routines are defined here: semaphores,
/// locks and condition variables (the implementation of the last two are
/// left to the reader).
///
/// Any implementation of a synchronization routine needs some primitive
/// atomic operation.  We assume Nachos is running on a uniprocessor, and
/// thus atomicity can be provided by turning off interrupts.  While
/// interrupts are disabled, no context switch can occur, and thus the
/// current thread is guaranteed to hold the CPU throughout, until interrupts
/// are reenabled.
///
/// Because some of these routines might be called with interrupts already
/// disabled (`Semaphore::V` for one), instead of turning on interrupts at
/// the end of the atomic operation, we always simply re-set the interrupt
/// state back to its original value (whether that be disabled or enabled).
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2018 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "synch.hh"
#include "system.hh"
#include "synch_list.hh"
#include "port.hh"


/// Initialize a semaphore, so that it can be used for synchronization.
///
/// * `debugName` is an arbitrary name, useful for debugging.
/// * `initialValue` is the initial value of the semaphore.
Semaphore::Semaphore(const char *debugName, int initialValue)
{
    name  = debugName;
    value = initialValue;
    queue = new List<Thread *>;
}

/// De-allocate semaphore, when no longer needed.
///
/// Assume no one is still waiting on the semaphore!
Semaphore::~Semaphore()
{
    delete queue;
}

const char *
Semaphore::GetName() const
{
    return name;
}

/// Wait until semaphore `value > 0`, then decrement.
///
/// Checking the value and decrementing must be done atomically, so we need
/// to disable interrupts before checking the value.
///
/// Note that `Thread::Sleep` assumes that interrupts are disabled when it is
/// called.
void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(INT_OFF);

      // Disable interrupts.

    while (value == 0) {  // Semaphore not available.
        queue->Append(currentThread);  // So go to sleep.
        currentThread->Sleep();
    }
    value--;  // Semaphore available, consume its value.
    interrupt->SetLevel(oldLevel);  // Re-enable interrupts.
}
/// Increment semaphore value, waking up a waiter if necessary.
///
/// As with `P`, this operation must be atomic, so we need to disable
/// interrupts.  `Scheduler::ReadyToRun` assumes that threads are disabled
/// when it is called.
void
Semaphore::V()
{
    IntStatus oldLevel = interrupt->SetLevel(INT_OFF);

    Thread *thread = queue->Pop();
    if (thread != nullptr)
        // Make thread ready, consuming the `V` immediately.
        scheduler->ReadyToRun(thread);
    value++;

    interrupt->SetLevel(oldLevel);
}

/// Dummy functions -- so we can compile our later assignments.
///
/// Note -- without a correct implementation of `Condition::Wait`, the test
/// case in the network assignment will not work!

Lock::Lock(const char *debugName)
{
    name = debugName;
    monosem = new Semaphore (debugName, 1); 
    thread = nullptr;
}

Lock::~Lock()
{
    delete monosem;
}

const char *
Lock::GetName() const
{
    return name;
}

void
Lock::Acquire()
{
    ASSERT(!IsHeldByCurrentThread());
    DEBUG('s',"Prioridad %u guardada de %s\n",prioactual,currentThread->GetName() );
    
    if(thread != nullptr && thread->GetPrio() < currentThread->GetPrio() ){
        thread->SetPrio(currentThread->GetPrio() );
    }
	
    monosem->P();
    
    prioactual = currentThread->GetPrio();
   
    DEBUG('s',"Hilo %s haciendo acquire de lock %s\n",currentThread->GetName(),name);
    thread = currentThread;
}

void
Lock::Release()
{
    DEBUG('s',"Hilo %s intentando release lock %s\n",currentThread->GetName(),name);
    ASSERT(IsHeldByCurrentThread());
    DEBUG('s',"Prioridad %u a setear a %s\n",prioactual,currentThread->GetName() );
    currentThread->SetPrio(prioactual);
    thread = nullptr;
    
    monosem->V();    
    
    DEBUG('s',"Hilo %s hizo release de lock %s\n",currentThread->GetName(),name);
}

bool
Lock::IsHeldByCurrentThread() const
{
    return (thread == currentThread);
}

Condition::Condition(const char *debugName, Lock *conditionLock)
{
	name = debugName;
	lockcon = conditionLock;
	queue = new List<Semaphore *>;
}

Condition::~Condition()
{
	delete queue;
}

const char *
Condition::GetName() const
{
    return name;
}

void
Condition::Wait()
{
	DEBUG('s',"Hilo %s se fue a dormir con condicional %s\n",currentThread->GetName(),name);
	ASSERT(lockcon->IsHeldByCurrentThread());
	Semaphore *wait = new Semaphore(currentThread->GetName(),0);
	queue->Append(wait);
	lockcon->Release();
	wait->P();
    delete wait;
	lockcon->Acquire();
}

void
Condition::Signal()
{
	if( !queue->IsEmpty() ){
		Semaphore *temp = queue->Pop();
		temp->V();
		DEBUG('s',"Hilo %s se despierta por condicional %s\n",temp->GetName(),name);
	}
}

void
Condition::Broadcast()
{
	while( !queue->IsEmpty() ){
		Semaphore *temp = queue->Pop();
		temp->V();
		DEBUG('s',"Hilo %s se despierta por condicional %s\n",temp->GetName(),name);

	}
}
