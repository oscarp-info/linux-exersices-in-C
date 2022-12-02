/*
    Realizar una prueba del 'cliente' que nos proporcionaron para verificar su funcionamiento y el
    protocolo contra el servidor que tenemos que implementar.
    Vamos a utilizar dos herramientas: netcat y xxd.

    ```console
    sudo apt-get update
    sudo apt-get install netcat
    sudo apt-get install xxd
    ```

    Con la aplicacion netcat ("nc") en una terminal, simulamos nuestro servidor que recibirá la informacion
    del cliente y la muestrará en pantalla; la salida se la emviamos a otra applicacion para que la muestra
    en hexadecimal asi podemos entender bien lo que nos envia el cliente (segun el enunciado un integer
    en network byte order que indica la longitud del mensaje y luego el mensaje como string sin el '\0').
    Abrimos dos consolas, una para el servidor y otra para el cliente:

    ```console
    $ nc -l 8000 | xxd -g 1
    00000000: 00 00 00 0b 68 6f 73 74 30 3b 31 35 3b 37 38     ....host0;15;78
    ```
    ```console
    $ ./cliente localhost 8000 1
    ```
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include "sock-lib.h"

#define HOSTNAME_LEN 64
#define STATUS_SIZE 10

typedef struct
{
    char hostname[HOSTNAME_LEN];
    int cpu;
    int mem;
} status_t;

// array de estructuras status_t;
status_t client_status[STATUS_SIZE];

int main(int argc, char **argv)
{
    return 0;
}
