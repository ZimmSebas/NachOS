#include "syscall.h"

int
main(void)
{
    SpaceId    newProc;
    OpenFileId input  = CONSOLE_INPUT;
    OpenFileId output = CONSOLE_OUTPUT;
    char       prompt[2], ch, buffer[60];
    int        i;
    int        joineable;

    prompt[0] = '-';
    prompt[1] = '-';

    while (1)
    {
        joineable = 1; //Por defecto es Joineable
        Write(prompt, 2, output);
        i = 0;
        do
            Read(&buffer[i], 1, input);
        while (buffer[i++] != '\n');

        buffer[--i] = '\0';
                
        if (i > 0) {
            int it = 0,cantpal = 1;
            
            if(buffer[it] == '&')
              it+=2;
            
            
            for(;buffer[it] != '\0';it++){
              if(buffer[it] == ' ')
                cantpal++; //contador de argumentos
            }
            
            char *argv[cantpal+1];
            
            it = 0, cantpal = 1;
            if(buffer[it] == '&')
              it+=2;
              
            argv[0] = buffer+it;
            
            for(;buffer[it] != '\0';it++){
              if(buffer[it] == ' '){
                argv[cantpal] = buffer+it+1; //Apuntamos al primer lugar del argumento n (con n = cantpal)
                cantpal++;
                buffer[it] = '\0';
              }
            }
            
            argv[cantpal] = 0;
             
            if(buffer[0] == '&'){ //Lo hacemos no joineable
              joineable = 0;
            }
             
            newProc = Exec(argv,joineable);
            if(joineable){
              Join(newProc);
              
              }
        }
    }
}
