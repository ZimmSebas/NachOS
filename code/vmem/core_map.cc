#include "core_map.hh"
#include "threads/system.hh"


#define mp(p,q) make_pair(p,q)
#define fst first
#define snd second

CoreMap::CoreMap(){
	for(unsigned i = 0 ; i < NUM_PHYS_PAGES ;i++)
		mapa[i] = mp(-1,-1);
	lastout = 0;
}

CoreMap::~CoreMap(){
}


// pid : Process ID
// vpn : Virtual Page Number.
// ppn : Physical Page Number.
void
CoreMap::Agregar(int pid, int vpn, int ppn){ 
	mapa[ppn] = mp(pid,vpn);
}

pair<int,int>
CoreMap::PidoInfo(int ppn){
	return mapa[ppn];
}

void
CoreMap::Borrar(int ppn){
	mapa[ppn] = mp(-1,-1);
}

int
CoreMap::Reloj(){
		
	unsigned temp = 0;
	
	for (unsigned j = 0; j < 2; j++){ //Una sin cambiar nada, otra poniendo bits de usado en cero
		
		for(unsigned i = 0; i < NUM_PHYS_PAGES; i++){ //for que busca (0,0)
			
			temp = (lastout + i) % NUM_PHYS_PAGES;
					
			pair <int,int> data = mapa[temp];

			int pid = data.first;
			unsigned vpn = data.second;
			
			Thread *thswapeando = corriendo->Get(pid);

			bool dirty = thswapeando->space->pageTable[vpn].dirty;
			bool use   = thswapeando->space->pageTable[vpn].use;
			
			if(!use && !dirty){
				lastout = (temp + 1) % NUM_PHYS_PAGES;
				return temp;
			}
		}
		
		for(unsigned i = 0; i < NUM_PHYS_PAGES; i++){ //for que busca (0,1) y cambia (1,n) a (0,n)
			
			temp = (lastout + i) % NUM_PHYS_PAGES;
			
			pair <int,int> data = mapa[temp];

			int pid = data.first;
			unsigned vpn = data.second;
			
			
			Thread *thswapeando = corriendo->Get(pid);

			bool dirty = thswapeando->space->pageTable[vpn].dirty;
			bool use   = thswapeando->space->pageTable[vpn].use;
			
			if(!use && dirty){
				lastout = (temp + 1) % NUM_PHYS_PAGES;
				return temp;
			} else if (use){
				thswapeando->space->pageTable[vpn].use = false;
			}
		}
	}
	

	return -1;
}
