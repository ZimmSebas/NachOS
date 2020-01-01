/// Entry points into the Nachos kernel from user programs.
///
/// There are two kinds of things that can cause control to transfer back to
/// here from user code:
///
/// * System calls: the user code explicitly requests to call a procedure in
///   the Nachos kernel.  Right now, the only function we support is `Halt`.
///
/// * Exceptions: the user code does something that the CPU cannot handle.
///   For instance, accessing memory that does not exist, arithmetic errors,
///   etc.
///
/// Interrupts (which can also cause control to transfer from user code into
/// the Nachos kernel) are handled elsewhere.
///
/// For now, this only handles the `Halt` system call.  Everything else core-
/// dumps.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2019 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.

#include <cstdio>
#include "transfer.hh"
#include "syscall.h"
#include "args.hh"
#include "filesys/directory_entry.hh"
#include "threads/system.hh"

typedef struct _Args{
    AddressSpace *sp;
    char **av;
}Args;


static void
IncrementPC()
{
    unsigned pc;

    pc = machine->ReadRegister(PC_REG);
    machine->WriteRegister(PREV_PC_REG, pc);
    pc = machine->ReadRegister(NEXT_PC_REG);
    machine->WriteRegister(PC_REG, pc);
    pc += 4;
    machine->WriteRegister(NEXT_PC_REG, pc);
}

/// Do some default behavior for an unexpected exception.
///
/// * `et` is the kind of exception.  The list of possible exceptions is in
///   `machine/exception_type.hh`.

static void
PageFaultHandler(ExceptionType et)
{
  int virtaddr = machine->ReadRegister(BAD_VADDR_REG);
  int virtpage = virtaddr / PAGE_SIZE;
  static int pos = 0;

  TranslationEntry tentry = currentThread->space->GetTranslationEntry(virtpage);
  if(tentry.valid == false || (tentry.valid == true && tentry.physicalPage == -1)){ // La pagina no esta cargada en memoria
    currentThread->space->CargarPagina(virtpage);
    return;
  }
  //Conseguimos la translation entry en posicion X
#ifdef USE_TLB
  pos++;
  pos %= TLB_SIZE;
  machine->GetMMU()->tlb[pos] = tentry;
  stats->fallosTLB++;
#endif
  return;
}

static void
ReadOnlytHandler(ExceptionType et)
{
  fprintf(stderr, "Se intentó acceder a una página de solo lectura\n");
  ASSERT(false);
  return;
}

static void
DefaultHandler(ExceptionType et)
{
  int exceptionArg = machine->ReadRegister(2);
  fprintf(stderr, "Unexpected user mode exception: %s, arg %d.\n",
      ExceptionTypeToString(et), exceptionArg);
  ASSERT(false);    
}

void 
Ejecutar(void *arg_){ //Este nombre es muy poco cool


    Args *arg = (Args*) arg_;
    AddressSpace *space = (AddressSpace*) arg->sp;
    char **argv = (char**) arg->av;

    delete arg;

    DEBUG('t',"Ejecutando funcion de hijo\n");
    space->InitRegisters();  // Set the initial register values.
    
    space->RestoreState();   // Load page table register.
    WriteArgs(argv);
    
    machine->Run();
}

/// Handle a system call exception.
///
/// * `et` is the kind of exception.  The list of possible exceptions is in
///   `machine/exception_type.hh`.
///
/// The calling convention is the following:
///
/// * system call identifier in `r2`;
/// * 1st argument in `r4`;
/// * 2nd argument in `r5`;
/// * 3rd argument in `r6`;
/// * 4th argument in `r7`;
/// * the result of the system call, if any, must be put back into `r2`.
///
/// And do not forget to increment the program counter before returning. (Or
/// else you will loop making the same system call forever!)
static void
SyscallHandler(ExceptionType _et)
{
    int scid = machine->ReadRegister(2);

    switch (scid) {

        case SC_HALT:{
            DEBUG('a', "Shutdown, initiated by user program.\n");
            interrupt->Halt();
            break;
        }

        case SC_EXIT: {
            int status = machine->ReadRegister(4);
            if(status != 0){
                DEBUG('t', "Thread %s termina con un retorno distinto a cero \n",
                currentThread->GetName());
            } else {
                DEBUG('t', "Thread %s termina todo cool loco ;D \n",
                currentThread->GetName());
            }
            currentThread->Finish(status);

            break;
        }

        case SC_EXEC: {
            DEBUG('a', "Inicio syscall Exec\n");
            int argv       = machine->ReadRegister(4);
            int joineable  = machine->ReadRegister(5);
            char **argvKernel;
            char *filename;

            if (argv == 0){
                DEBUG('a', "Error: address to argv array is null.\n");
                break;
            }

            argvKernel = SaveArgs(argv);

            filename = argvKernel[0];

            OpenFile *executable = fileSystem->Open(filename);
            if (executable == nullptr) {
                DEBUG('a', "Error: Unable to open file %s\n", filename);
                break;
            }

            AddressSpace *space = new AddressSpace(executable);
            
            
            Args *argumento = new Args;
            argumento->sp = space;
            argumento->av = argvKernel;
            
            char *name = new char [64];
            strncpy(name, filename, 64);
            
            Thread *hijoThread = new Thread(name,((bool) joineable) );
            
            hijoThread->space = space;
            hijoThread->Fork(Ejecutar,argumento);
            
            machine->WriteRegister(2,hijoThread->myid);
            break;
        }

        case SC_CREATE: {
            DEBUG('a', "Inicio syscall Create\n");
            int filenameAddr = machine->ReadRegister(4);
            if (filenameAddr == 0){
                DEBUG('a', "Error: address to filename string is null.\n");
                break;
            }

            char filename[FILE_NAME_MAX_LEN + 1];
            if (!ReadStringFromUser(filenameAddr, filename, sizeof filename)){
                DEBUG('a', "Error: filename string too long (maximum is %u bytes).\n",
                      FILE_NAME_MAX_LEN);
                break;
            }
                        
            if (!fileSystem->Create(filename,741))
                DEBUG('a', "Error: llamada a Create fallo.\n");

            break;
        }
        
        case SC_JOIN: {
			DEBUG('a', "Inicio syscall Join\n");
            SpaceId id = machine->ReadRegister(4);
            int ret = -1;
            Thread *hijo;
            if(!corriendo->HasKey(id)){
				DEBUG('t', "Error: id inexistente");
				machine->WriteRegister(2,ret);
				break;
			}
			
			hijo = corriendo->Get(id);
			
			ret = hijo->Join();
          
			machine->WriteRegister(2,ret);
			
			break;
			// :(
          
        }
        
        case SC_OPEN: {
            DEBUG('a', "Inicio syscall Open\n");
            OpenFileId id;
            OpenFile *archivo;
            
            int filenameAddr = machine->ReadRegister(4);
            if (filenameAddr == 0){
                DEBUG('a', "Error: address to filename string is null.\n");
				machine->WriteRegister(2,-1);	
				break;
			}

            char filename[FILE_NAME_MAX_LEN + 1];
            if (!ReadStringFromUser(filenameAddr, filename, sizeof filename)){
                DEBUG('a', "Error: filename string too long (maximum is %u bytes).\n",
                      FILE_NAME_MAX_LEN);
				machine->WriteRegister(2,-1);	
				break;
			}
            
            archivo = fileSystem->Open(filename);
            if(archivo != nullptr){
				id = currentThread->AddArchivo(archivo);				
				machine->WriteRegister(2,id);
			}
			else{
				DEBUG('a', "Error: Archivo a puntero nulo\n");
				machine->WriteRegister(2,-1);	
			}
            break;
        }
        
        case SC_CLOSE: {
            OpenFileId id = machine->ReadRegister(4);
            DEBUG('a', "Close requested for id %u.\n", id);
            currentThread->DeleteArchivo(id);
            break;
        }

        case SC_READ: {
			DEBUG('a',"empiezo a leer\n");
            int bufferUsuario = machine->ReadRegister(4);
            unsigned size = (unsigned)machine->ReadRegister(5);
            OpenFileId id = machine->ReadRegister(6);
			char *buffer = new char[size];
            int leido = 0;
            
			if (id == CONSOLE_INPUT){
				consola->ReadBuffer(buffer,size);
				leido = size;
			} else if(id == CONSOLE_OUTPUT){
				DEBUG('a', "Error: intenta leer de stdout\n");
				break;
			} else{
				OpenFile *archivo;
				archivo = currentThread->GetArchivo(id);
				
				if (!archivo){
					DEBUG('a', "Error: llamada a Read fallo.\n");
					machine->WriteRegister(2,leido);
					break;
				}
				
				leido = archivo->Read(buffer,size);
				
				DEBUG('a', "Cantidad leida %d\n", leido);
			}
            if(leido != 0)
				WriteBufferToUser(buffer,bufferUsuario,leido);
				
			machine->WriteRegister(2,leido);
            
            delete [] buffer;
            break;
        }
        
        case SC_WRITE: {
            int bufferUsuario = machine->ReadRegister(4);
            unsigned size = machine->ReadRegister(5);
            OpenFileId id = machine->ReadRegister(6);
            unsigned escrito = 0;
			char *buffer = new char[size];
            
			if(bufferUsuario == 0){
				DEBUG('a',"Error, bufferUsuario nulo\n");
				break;
			}
			
				if(size == 0){
					DEBUG('a',"Error, tamaño de lectura nulo\n");
					break;
				}
			 
            ReadBufferFromUser(bufferUsuario,buffer,size);
            
            if (id == CONSOLE_OUTPUT){
				consola->WriteBuffer(buffer,size);
				escrito = size;
			} else if(id == CONSOLE_INPUT){
				DEBUG('a', "Error: intenta escribir en stdin\n");
				break;
			} else{
            
				OpenFile *archivo = currentThread->GetArchivo(id);
				
				if (!archivo){
					DEBUG('a', "Error: llamada a GetArchivo fallo.\n");
					break;
				}
				
				escrito = archivo->Write(buffer,size);
			}
            
			
			if(escrito != size)
				DEBUG('a', "Error: La cantidad escrita es distinta de la esperada\n");
            
            delete[] buffer;
            
            break;
        }

        default:
            fprintf(stderr, "Unexpected system call: id %d.\n", scid);
            ASSERT(false);

    }

    IncrementPC();
}


/// By default, only system calls have their own handler.  All other
/// exception types are assigned the default handler.
void
SetExceptionHandlers()
{
    machine->SetHandler(NO_EXCEPTION,            &DefaultHandler);
    machine->SetHandler(SYSCALL_EXCEPTION,       &SyscallHandler);
    machine->SetHandler(PAGE_FAULT_EXCEPTION,    &PageFaultHandler);
    machine->SetHandler(READ_ONLY_EXCEPTION,     &ReadOnlytHandler);
    machine->SetHandler(BUS_ERROR_EXCEPTION,     &DefaultHandler);
    machine->SetHandler(ADDRESS_ERROR_EXCEPTION, &DefaultHandler);
    machine->SetHandler(OVERFLOW_EXCEPTION,      &DefaultHandler);
    machine->SetHandler(ILLEGAL_INSTR_EXCEPTION, &DefaultHandler);
}
