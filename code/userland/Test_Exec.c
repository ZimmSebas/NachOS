#include "syscall.h"


int main(){
  char pepe[]="cp";
  char pepe2[]="TestCopy";
  char pepe3[]="AAxpepe";
  char *argv[4];
  argv[0] = pepe;
  argv[1] = pepe2;
  argv[2] = pepe3;
  argv[3] = 0;
  SpaceId id = Exec(argv,0);
  //~ Write("pepe",5,1);
  Exit(0);
  return 0;
}
