#include "transfer.hh"
#include "lib/utility.hh"
#include "threads/system.hh"


bool ReadStringFromUser(int userAddress, char *outString,
                        unsigned maxByteCount)
{
    ASSERT(userAddress != 0);
    ASSERT(outString != nullptr);
    ASSERT(maxByteCount != 0);
    
    bool pudeleer = false;
    unsigned count = 0;
    
    do {
        int temp;
        count++;
        for(int i = 0;i<10;i++){ //intentamos 10 veces leer memoria
          pudeleer = machine->ReadMem(userAddress, 1, &temp);
          if(pudeleer)
            break;
        }
        userAddress++;
        ASSERT(pudeleer);
        *outString = (unsigned char) temp;
    } while (*outString++ != '\0' && count < maxByteCount);

    return *(outString - 1) == '\0';
}

void ReadBufferFromUser(int userAddress, char *outBuffer,
                        unsigned byteCount)
{
    ASSERT(userAddress != 0);
    ASSERT(outBuffer != nullptr);
    ASSERT(byteCount != 0);
    
    bool pudeleer = false;
    unsigned count = 0;
    do {
        int temp;
        count++;
        for(int i = 0;i<10;i++){ //intentamos 10 veces leer memoria
          pudeleer = machine->ReadMem(userAddress, 1, &temp);
          if(pudeleer)
            break;
        }
        
        userAddress++;
        ASSERT(pudeleer);
        *outBuffer = (unsigned char) temp;
        outBuffer++;
    } while (count < byteCount);

}

void WriteBufferToUser(const char *buffer, int userAddress, unsigned byteCount){
	
	ASSERT(byteCount != 0);
	ASSERT(userAddress != 0);
	ASSERT(buffer != nullptr);
	
  bool pudeesc = false;
	unsigned count = 0;
	
	do{
		count++;
		 for(int i = 0;i<10;i++){ //intentamos 10 veces leer memoria
			pudeesc = machine->WriteMem(userAddress, 1, *buffer);
			if(pudeesc){
				userAddress++;
				buffer++;
				break;
			}
		}
    ASSERT(pudeesc);
	} while (count < byteCount);
}


void WriteStringToUser(const char *string, int userAddress){
	
	ASSERT(userAddress != 0);
	ASSERT(string != nullptr);
	
	bool pudeesc = false;
  
	do{
  
  for(int i = 0;i<10;i++){ //intentamos 10 veces leer memoria
      pudeesc = machine->WriteMem(userAddress, 1, *string);
      if(pudeesc)
        break;
  }
    userAddress++;
    ASSERT(pudeesc);
	} while (*string++ != '\0');
}


// TODO: complete...
