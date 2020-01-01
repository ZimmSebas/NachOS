#include <syscall.h>

int main(int argc, char** argv){

    char arg1[] = "h";
    char *arg[2];
    
    arg[0] = arg1;
    arg[1] = 0;
    	
	OpenFileId id = 0;
	
	Create(arg1);

	id = Open(arg1);
	
	//~ int i = 0;
	//~ for(i = 0;i<10;i++){
	//~ Write("Test\n",5,1);
	//~ }
	Close(id);


    //~ static char* fst = "Pre1Exec  \n";
    //~ static char* snd = "Pre1Join  \n";
    //~ static char* trd = "Pre2Exec  \n";
    //~ static char* fth = "Pre2Join  \n";
    //~ SpaceId newProc;
  
    //~ Write(fst,12,CONSOLE_OUTPUT); //Pre primer exec

    //~ newProc = Exec(arg,0);

    //~ Write(snd,12,CONSOLE_OUTPUT); //Post primer exec

    //~ Join(newProc);
    
    //~ Write(trd,12,CONSOLE_OUTPUT); //Pre segundo exec
      
    //~ newProc = Exec(arg,0);
    
    //~ Write(fth,12,CONSOLE_OUTPUT); //Post segundo exec

    //~ newProc = Exec(arg,0);
    //~ newProc = Exec(arg,1);
    //~ Join(newProc);	
	//~ Halt();

}
