#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#define FIFO_NAME "myfifo"          //Nombre de la fifo
#define BUFFER_SIZE 300             //Tamano del buffer
#define Header_Data "DATA"          //Cabezera del mensaje tipo dato
#define Header_Sign "SIGN"          //Cabezera del mensaje tipo senal
#define Size_Header_signal 6
char msg_header[20];                //buffer para la cabezera del mensaje
char outputBuffer[BUFFER_SIZE];    //buffer para envio del mensaje completo
uint32_t bytesWrote;
int32_t returnCode;
int32_t fd;                         //File Descriptor

void MySignalHandler1(int sig){

    if ((bytesWrote = write(fd, "SIGN:1",Size_Header_signal )) == -1)
            {
			    perror("write");
            }
            else
            {
			    printf("writer: wrote %d bytes\n", bytesWrote);
        }

}


void MySignalHandler2(int sig){

   if ((bytesWrote = write(fd, "SIGN:2", Size_Header_signal)) == -1)
            {
			    perror("write");
            }
            else
            {
			    printf("writer: wrote %d bytes\n", bytesWrote);
        }

}

int main(void)
{
   
   
  
    /* Create named fifo. -1 means already exists so no action if already exists */
    if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1 )
    {
        printf("Error creating named fifo: %d\n", returnCode);
        exit(1);
    }

    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for readers...\n");
	if ( (fd = open(FIFO_NAME, O_WRONLY) ) < 0 )
    {
        printf("Error opening named fifo file: %d\n", fd);
        exit(1);
    }
    
    /* open syscalls returned without error -> other process attached to named fifo */
	printf("got a reader--type some stuff\n");

    /* Loop forever */

    signal(SIGUSR1,&MySignalHandler1);
    signal(SIGUSR2,&MySignalHandler2);
    
	while (1)
	{
        /* Get some text from console */
		fgets(outputBuffer, BUFFER_SIZE, stdin);

        sscanf(outputBuffer,"%[^:]:",msg_header);

        if(strcmp(msg_header,Header_Data)!=0){

            perror("mensaje incorrecto\n\n");

        }
        else{
            /* Write buffer to named fifo. Strlen - 1 to avoid sending \n char */
		    if ((bytesWrote = write(fd, outputBuffer, strlen(outputBuffer)-1)) == -1)
            {
			    perror("write");
            }
            else
            {
			    printf("writer: wrote %d bytes\n", bytesWrote);
        }

        }
        
        	}
	return 0;
}