#include "lockFileSys.hh"
#include "threads/system.hh"

lockFS::lockFS(const char *debugName){
	cont = 0;
  unsigned nameLen = strlen(debugName);
  name = new char[nameLen+5];
  strcpy(name,debugName);
  char tmp1[] = "atomizar operaciones lockFS: ";
  atomName = new char[nameLen+strlen(tmp1)+5];
	strcpy(atomName, tmp1);
	strcat(atomName, name);
  atom = new Lock(atomName);
  
  char tmp2[] = "escritura lectura concurrente: ";
  readwriteName = new char[nameLen+strlen(tmp2)+5];
	strcpy(readwriteName, tmp2);
	strcat(readwriteName, name);
	readwrite = new Lock(readwriteName);
}

lockFS::~lockFS(){
  delete atom;
  delete readwrite;
  delete[] name;
  delete[] atomName;
  delete[] readwriteName;
}

void
lockFS::AcquireWrite(){
#ifdef FILESYS
  currentThread->usandoFS++; // marco que voy a usar el fs
#endif
	readwrite->Acquire();
}

void
lockFS::ReleaseWrite(){
	readwrite->Release();
#ifdef FILESYS
  currentThread->usandoFS--; // marco que deje de usar el fs
#endif
}

void
lockFS::AcquireRead(bool escribiendo){
#ifdef FILESYS
  if(!escribiendo)
    currentThread->usandoFS++; // marco que voy a usar el fs
#endif
	atom->Acquire();
	if(cont == 0 && !escribiendo)
		readwrite->Acquire();
	cont++;
	atom->Release();
}

void
lockFS::ReleaseRead(bool escribiendo){
	atom->Acquire();
	if(cont > 0){
		cont--;
		if(!escribiendo && cont == 0 )
			readwrite->Release();
	}
	atom->Release();
#ifdef FILESYS
  if(!escribiendo)
    currentThread->usandoFS--; // marco que deje de usar el fs
#endif
}
