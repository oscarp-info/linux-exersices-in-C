/*
 * gcc -Wall servidor_paso_02.c sock-lib.c -o servidor
 *
 *
 *
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
    int debug = 1;
    int sock = -1;
    struct sockaddr_in address;
    int port = 8000;
    int backlog = 10;
    int sockdup;

    // variables para almacenar lo que nos envia el cliente
    int len;
    char *buffer;

    sock = open_conection(&address, port, backlog, debug);

    if (sock == -1)
    {
        fprintf(stderr, "debug:: main() open_conection error!\n");
        exit(-1);
    }

    if (debug)
        fprintf(stderr, "debug:: main() servidor ready y listening\n");

    while (1)
    {
        sockdup = aceptar_pedidos(sock, debug);
        if (sockdup > 0)
        {
            // leer el entero
            read(sockdup, &len, sizeof(int));
            if (len > 0)
            {

                // recordar: network to host byte order
                len = ntohl(len);

                buffer = (char *)malloc((len + 1) * sizeof(char));
                buffer[len] = 0;

                // leer el string
                read(sockdup, buffer, len);

                // lo muestro por pantalla
                if (debug)
                    printf("debug:: mensaje recibido...[len=%d][message=%s]\n", len, buffer);

                // cleanup
                free(buffer);
                close(sockdup);
            }
        }
        usleep(2500);
    }
    return 0;
}
