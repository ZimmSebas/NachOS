#include "core_map.hh"

#define mp(p,q) make_pair(p,q)
#define fst first
#define snd second

CoreMap::CoreMap(){
	for(unsigned i = 0 ; i < NUM_PHYS_PAGES ;i++)
		mapa[i] = mp(-1,-1);
}

CoreMap::~CoreMap(){
}

void
CoreMap::Agregar(int pid, int vpn, int ppn){
	mapa[ppn] = mp(pid,vpn);
}

void
CoreMap::Borrar(int ppn){
	mapa[ppn] = mp(-1,-1);
}

