#include <syscall.h>

int main(int argc, char** argv){
	char *arg1 = argv[1];
	
	OpenFileId id;
	
	char s[10];
	int leido = 0;
	
	
	id = Open(arg1);
	do {
		leido = Read(s,10,id);
		Write(s,leido,1);
	} while(leido != 0);
	Close(id);
	Exit(0);
	return 0;
}
