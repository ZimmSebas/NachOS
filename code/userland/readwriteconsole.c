#include <syscall.h>

int main(){
	char buffer[10];
	Read(buffer,10,0);
	Write(buffer,10,1);
	Halt(0);
}
