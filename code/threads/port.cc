#include "port.hh"

Port::Port(const char *debugName)
{
	portlist = new SynchList<int>();
	portsem = new Semaphore("port semaphore",0);
	
	name = debugName;
}

Port::~Port()
{
	delete portlist;
	delete portsem;
}

const char *
Port::GetName() const
{
    return name;
}

void
Port::Send(int message)
{
	portlist->Append(message);
	portsem->P();
}

void
Port::Receive(int *message)
{
	*message = portlist->Pop();
	portsem->V();

}

