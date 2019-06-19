#ifndef NACHOS_VMEM_COREMAP__HH
#define NACHOS_VMEM_COREMAP__HH

#include "machine/mmu.hh"
#include <utility>

using namespace std;

class CoreMap {
public:

	CoreMap();
	
	~CoreMap();
	
	void Agregar(int pid, int vpn, int ppn);
	
	void Borrar(int ppn);
	
private:
	pair<int,int> mapa[NUM_PHYS_PAGES];
};

#endif
