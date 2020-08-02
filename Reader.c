/**
 * @file    Reader.c
 * @author  Jose Mauricio Lara Tapia (torneosmau@gmail.com)
 * @brief   Comunicacion de procesos por FIFO
 * @version 0.1
 * @date    2020-08-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#define FIFO_NAME "myfifo" //Nombre de la fifo
#define BUFFER_SIZE 300    //Tamano del buffer
#define Header_Data "DATA" //Cabezera del mensaje tipo dato
#define Header_Sign "SIGN" //Cabezera del mensaje tipo senal

char msg_header[20]; //buffer para el cuerpo del mensaje

uint8_t inputBuffer[BUFFER_SIZE]; //buffer para recepcion del mensaje completo
int32_t bytesRead;
int32_t returnCode;
int32_t fd; //File descriptor

FILE *Log;     //Creador del archivo log.txt
FILE *Signals; //Creador del archivo signals.txt

int main(void)
{
    Log = fopen("log.txt", "a");
    if (Log == NULL)
    {
        printf("Error en la creacion de archivo log.txt\n\n");
        exit(1);
    }

    Signals = fopen("Signals.txt", "a");
    if (Signals == NULL)
    {
        printf("Error en la creacion de archivo signals.txt\n\n");
        exit(1);
    }
    /* Create named fifo. -1 means already exists so no action if already exists */
    if ((returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0)) < -1)
    {
        printf("Error creating named fifo: %d\n", returnCode);
        exit(1);
    }

    /* Open named fifo. Blocks until other process opens it */
    printf("waiting for writers...\n");
    if ((fd = open(FIFO_NAME, O_RDONLY)) < 0)
    {
        printf("Error opening named fifo file: %d\n", fd);
        exit(1);
    }

    /* open syscalls returned without error -> other process attached to named fifo */
    printf("got a writer\n");

    /* Loop until read syscall returns a value <= 0 */
    do
    {
        /* read data into local buffer */
        if ((bytesRead = read(fd, inputBuffer, BUFFER_SIZE)) == -1)
        {
            perror("read");
        }
        else
        {
            inputBuffer[bytesRead] = '\0';
            sscanf(inputBuffer, "%[^:]:", msg_header);

            if (strcmp(msg_header, Header_Data) == 0)
            {
                fprintf(Log, "%s \n", inputBuffer);
                printf("se recibio data %s\n", inputBuffer);
            }

            else if (strcmp(msg_header, Header_Sign) == 0)
            {

                fprintf(Signals, "%s \n", inputBuffer);
                printf("se recibio signal %s\n", inputBuffer);
            }
            else
            {
                perror("mensje no valido");
            }

            //	printf("reader: read %d bytes: palabra1: \"%s\" palabra2 \"%s\" \n", bytesRead, dato1,dato2);
        }
        fflush(Log);
        fflush(Signals);
        fclose(Log);
        fclose(Signals);
        close(fd);

    } while (bytesRead > 0);

    return 0;
}