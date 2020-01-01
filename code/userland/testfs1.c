#include "syscall.h"

int
main(void)
{
    OpenFileId id;
    Create("Ale");

	char pepe[]="testfs2";
	char *argv[2];
	argv[0] = pepe;
	argv[1] = 0;
    SpaceId ids = Exec(argv,1);
	
	int i;
    id = Open("Ale");
	for (i = 0;i<100;i++){
		Write("abcde\n",6,id);
	}
	for (i = 0;i<141;i++){
		Write("2",1,id);
	}
	Write("Test11 \n",9,1);
    Close(id);
	Write("Test12 \n",9,1);
	Join(ids);
	Exit(0);    
}
