/// Routines to manage address spaces (executing user programs).
///
/// In order to run a user program, you must:
///
/// 1. Link with the `-N -T 0` option.
/// 2. Run `coff2noff` to convert the object file to Nachos format (Nachos
///    object code format is essentially just a simpler version of the UNIX
///    executable object code format).
/// 3. Load the NOFF file into the Nachos file system (if you have not
///    implemented the file system yet, you do not need to do this last
///    step).
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2017 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.

#include <cstdio>
#include <sstream>
using namespace std;

#include "address_space.hh"
#include "bin/noff.h"
#include "machine/endianness.hh"
#include "threads/system.hh"
#include "userprog/syscall.h"

#define min(a,b) (a)<(b)?(a):(b)


/// Do little endian to big endian conversion on the bytes in the object file
/// header, in case the file was generated on a little endian machine, and we
/// are re now running on a big endian machine.
static void
SwapHeader(noffHeader *noffH)
{
    ASSERT(noffH != nullptr);

    noffH->noffMagic              = WordToHost(noffH->noffMagic);
    noffH->code.size              = WordToHost(noffH->code.size);
    noffH->code.virtualAddr       = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr        = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size          = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr   = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr    = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size        = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr =
      WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr  = WordToHost(noffH->uninitData.inFileAddr);
}

/// Create an address space to run a user program.
///
/// Load the program from a file `executable`, and set everything up so that
/// we can start executing user instructions.
///
/// Assumes that the object code file is in NOFF format.
///
/// First, set up the translation from program memory to physical memory.
/// For now, this is really simple (1:1), since we are only uniprogramming,
/// and we have a single unsegmented page table.
///
/// * `executable` is the file containing the object code to load into
///   memory.
AddressSpace::AddressSpace(OpenFile *executable)
{    
    ASSERT(executable != nullptr);
    exec = executable;
    noffHeader noffH;
    executable->ReadAt((char *) &noffH, sizeof noffH, 0);
    if (noffH.noffMagic != NOFF_MAGIC &&
          WordToHost(noffH.noffMagic) == NOFF_MAGIC)
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFF_MAGIC);

    // How big is address space?
    unsigned size = noffH.code.size + noffH.initData.size
                    + noffH.uninitData.size + USER_STACK_SIZE;
      // We need to increase the size to leave room for the stack.
    numPages = DivRoundUp(size, PAGE_SIZE);
    size = numPages * PAGE_SIZE;
    
    DEBUG('a', "Initializing address space, num pages %u, size %u\n",
          numPages, size);

    // First, set up the translation.

    pageTable = new TranslationEntry[numPages];
    for (unsigned i = 0; i < numPages; i++) {
        pageTable[i].virtualPage  = i;
        pageTable[i].physicalPage = -1;
        pageTable[i].valid        = false;
    }

	#ifdef VMEM
	nohayswap = true;
	#endif

    DEBUG('a', "Finish initializing address space\n");
}

/// Deallocate an address space.
///
/// Nothing for now!
AddressSpace::~AddressSpace()
{
    
    
    for (unsigned i = 0; i < numPages; i++){ //Liberamos las paginas utilizadas
        if(pageTable[i].physicalPage != -1)
            paginas->Clear(pageTable[i].physicalPage);
            //~ Borrar(pageTable[i].physicalPage);
    }
    delete [] pageTable;
    delete exec;
    
    //~ #ifdef VMEM
	//~ if (!fileSystem->Remove(nswap))
		//~ DEBUG('a', "Error: llamada a Remove fallo.\n");
		
	//~ delete [] nswap;
    //~ #endif
    
}

/// Set the initial values for the user-level register set.
///
/// We write these directly into the “machine” registers, so that we cans
/// immediately jump to user code.  Note that these will be saved/restored
/// into the `currentThread->userRegisters` when this thread is context
/// switched out.
void
AddressSpace::InitRegisters()
{
    for (unsigned i = 0; i < NUM_TOTAL_REGS; i++)
        machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of `Start`.
    machine->WriteRegister(PC_REG, 0);

    // Need to also tell MIPS where next instruction is, because of branch
    // delay possibility.
    machine->WriteRegister(NEXT_PC_REG, 4);
    
    
    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we do not
    // accidentally reference off the end!
    
    
    machine->WriteRegister(STACK_REG, numPages * PAGE_SIZE - 16);
    DEBUG('a', "Initializing stack register to %u\n",
          numPages * PAGE_SIZE - 16);
}

/// On a context switch, save any machine state, specific to this address
/// space, that needs saving.
///
/// For now, nothing!
void
AddressSpace::SaveState()
{}

/// On a context switch, restore the machine state so that this address space
/// can run.
///
/// For now, tell the machine where to find the page table.
void
AddressSpace::RestoreState()
{
    #ifndef USE_TLB
    machine->GetMMU()->pageTable     = pageTable;
    machine->GetMMU()->pageTableSize = numPages;
    #endif
}

TranslationEntry
AddressSpace::GetTranslationEntry(int vpn){
    return pageTable[vpn];
}

bool
AddressSpace::CargarPagina(int vpn){
	
	
	//~ #ifdef VMEM
	//~ if(nohayswap){
		//~ CrearSwap();
		//~ nohayswap = false;
	//~ }
	//~ #endif
	
    noffHeader noffH;
    exec->ReadAt((char *) &noffH, sizeof noffH, 0);
    if (noffH.noffMagic != NOFF_MAGIC &&
          WordToHost(noffH.noffMagic) == NOFF_MAGIC)
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFF_MAGIC);
    
    if(!paginas->CountClear())
        return false; // no hay paginas disponibles
    pageTable[vpn].physicalPage = paginas->Find();
    pageTable[vpn].valid        = true;
    pageTable[vpn].use          = false;
    pageTable[vpn].dirty        = false;
    pageTable[vpn].readOnly     = false;    

    #ifdef VMEM
        coremap->Agregar(currentThread->myid,vpn,pageTable[vpn].physicalPage);
    #endif
    
    char *mainMemory = machine->GetMMU()->mainMemory;

    // Zero out the entire page to zero
    memset(mainMemory+pageTable[vpn].physicalPage*PAGE_SIZE, 0, PAGE_SIZE);

    // Then, copy the page into memory.
    unsigned leido = PAGE_SIZE * vpn;
    if (noffH.code.size + noffH.code.virtualAddr > PAGE_SIZE * vpn) {
        //~ DEBUG('a', "Initializing code segment, at 0x%X, size %u\n",
                  //~ noffH.code.virtualAddr, noffH.code.size);
                  
        unsigned paginavirt = 0, posicion;
        /* paginavirt representa la pagina en la que estamos de virtual address
           posicion representa donde estamos en relacion a la memoria fisica
           leido representa la cantidad de bytes leidos */
        paginavirt = vpn;
        
        posicion = pageTable[paginavirt].physicalPage*PAGE_SIZE;
        
        // resto code es todo lo que necesitamos copiar
        unsigned restocode = noffH.code.size + noffH.code.virtualAddr - PAGE_SIZE * vpn;
        
        //copiarcode es cuanto necesitamos copiar (entre una página y lo que resta)
        unsigned copiarcode = min(restocode,PAGE_SIZE);
        for(unsigned i = 0; i < copiarcode;i++){
            exec->ReadAt(mainMemory+posicion,
                               1, noffH.code.inFileAddr+leido);
            leido++;
            posicion++;
        }
    }

    if( noffH.initData.size > 0 && noffH.initData.virtualAddr < PAGE_SIZE * (vpn+1) ){ 
        unsigned paginavirt = 0, offset = 0, posicion;
        /* paginavirt representa la pagina en la que estamos de virtual address
           offset representa la posicion relativa a la pagina actual
           posicion representa donde estamos en relacion a la memoria fisica
           leido representa la cantidad de bytes leidos */
        //~ paginavirt = noffH.initData.virtualAddr / PAGE_SIZE;
        paginavirt = vpn;
        if(leido % PAGE_SIZE)
            offset = noffH.initData.virtualAddr % PAGE_SIZE;
        
        posicion = pageTable[paginavirt].physicalPage*PAGE_SIZE+offset;
        
        // restodata es todo lo que necesitamos copiar
        unsigned restodata = noffH.initData.size + noffH.initData.virtualAddr - leido;
        //copiardata es cuanto necesitamos copiar (entre una página y lo que resta)
        unsigned copiardata = min(restodata,PAGE_SIZE-(leido % PAGE_SIZE));
        leido = noffH.initData.size - restodata;
        for(unsigned i = 0; i < copiardata;i++){
            exec->ReadAt(mainMemory+posicion,
                               1, noffH.initData.inFileAddr+leido);
            leido++;
            posicion++;
        }
    }
    
    return true;
}

 
#ifdef VMEM
void AddressSpace::CrearSwap(){

	string nombrearchivo;
	stringstream sst;
	sst << currentThread->myid;

	nombrearchivo = "SWAP." + sst.str();

	nswap = new char[nombrearchivo.length()];

	strcpy(nswap,nombrearchivo.c_str());

	if (!fileSystem->Create(nswap,0))
		DEBUG('a', "Error: llamada a Create fallo.\n");
}
#endif
    
   

