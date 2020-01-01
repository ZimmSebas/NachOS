#ifndef NACHOS_VMEM_COREMAP__HH
#define NACHOS_VMEM_COREMAP__HH

#include "machine/mmu.hh"
#include "threads/synch.hh"
#include "threads/thread.hh"
#include <utility>

using namespace std;

class CoreMap {
public:

	CoreMap();
	
	~CoreMap();
	
	void Agregar(int pid, int vpn, int ppn);
	
	void Borrar(int ppn);
	
	pair<int,int> PidoInfo(int ppn);
	
	int Reloj();

	
private:
	pair<int,int> mapa[NUM_PHYS_PAGES];
	
	unsigned lastout;	
};

#endif
