#ifndef NACHOS_FILESYS_LOCKFILESYS__HH
#define NACHOS_FILESYS_LOCKFILESYS__HH

#include "threads/synch.hh"

/*
 * La clase lockFS maneja que pueda haber varias lecturas en
 * simultáneo o solo una escritura a la vez.
 * 
 * cont lleva la cuenta de cuantas lecturas se están haciendo en un
 * momento determinado.
 * 
 * Para poder escribir este cont debe estar en 0 y quien quiere escribir
 * debe poseer el lock readwrite, impidiendo así la lectura y otras
 * escrituras.
 * 
 * Entre los lectores solo uno tendrá el lock readwrite.
 * 
 * */

class lockFS{
public:
	
	lockFS(const char *debugName);
	~lockFS();
	void AcquireWrite();
	void ReleaseWrite();
	void AcquireRead(bool escribiendo=false);
	void ReleaseRead(bool escribiendo=false);
	
private:
	char *name;
	unsigned cont;
  
	Lock *atom;
	char *atomName;
	
  Lock *readwrite;
	char *readwriteName;
};

#endif
