#include <syscall.h>

int main(int argc, char** argv){

    char *arg1 = argv[1];
	char *arg2 = argv[2];
	
	char s[10];
	int leido = 0;
	
	OpenFileId id = 0, id2 = 0;
	
	//~ Create(arg1);
	Create(arg2);

	id = Open(arg1);
	id2 = Open(arg2);
	
	do{
	leido = Read(s,10,id);
	Write(s,leido,id2);
	}while(leido != 0);
	Close(id);
	Close(id2);
	
	Exit(0);
	return 0;	

}
