#include "syscall.h"

int
main(void)
{
    OpenFileId id;
	id = Open("Ale");
	int j;
	for (j = 0; j<8;j++){
		Write("fghij\n",6,id);
	}
	
	Write("Test21 \n",9,1);

    Close(id);
	Write("Test22 \n",9,1);
	Exit(0);
}
