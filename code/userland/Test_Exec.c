#include "syscall.h"


int main(){
  char pepe[]="cp";
  char pepe2[]="TestCopy";
  char pepe3[]="AA";
  char *argv[4];
  //~ char *argv[3];
  argv[0] = pepe;
  argv[1] = pepe2;
  argv[2] = pepe3;
  argv[3] = 0;
  //~ argv[2] = 0;
  SpaceId id = Exec(argv,0);
  Write("pepe\n",6,1);
  Exit(0);
}
