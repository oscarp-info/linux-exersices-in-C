/*
 * gcc -Wall servidor_paso_03.c sock-lib.c -o servidor
 *
 *  guardo la informacion recibida en el array de estructuras(global)
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

// indice para saber la proxima posicion donde escribir en el array
int idx = 0;

// muestra el contenido del array en pantalla
void dump(void);

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

                // guardo el mensage en el array de estructuras en la posicion indicada por 'idx'
                // por ahora solo verifico que el array no este lleno y si ocurre lo descarto
                if (idx < STATUS_SIZE)
                {
                    // parsear el mensaje y ponerlo en cada campo de la estructura
                    // opcion (1)
                    sscanf(buffer, "%[^;];%d;%d", client_status[idx].hostname, &client_status[idx].cpu, &client_status[idx].mem);
                    // TODO:: implementa tu opcion alternativa
                    // ...
                    idx++;
                }
                else
                {
                    if (debug)
                        printf("debug:: mensaje recibido...descartado (array full)\n");
                    dump();
                }

                // cleanup
                free(buffer);
                close(sockdup);
            }
        }
        usleep(2500);
    }
    return 0;
}

void dump(void)
{
    int i = 0;

    fprintf(stderr, "=== dump del array ===\n");
    for (i = 0; i < STATUS_SIZE; i++)
    {
        fprintf(stderr, "%s; cpu=%%%d;memory=%%%d\n",
                client_status[i].hostname, client_status[i].cpu, client_status[i].mem);
    }
    fprintf(stderr, "======================\n");

    return;
}