/*
 * gcc -Wall servidor_paso_04.c sock-lib.c -o servidor -lpthread
 *
 *  guardo la informacion recibida en el array de estructuras(global)
 *  agrego la funcionalidad de servidor multithread
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

// funcion que ejecutara cada thread
void *process(void *ptr);

// estructura a enviar a cada thread que procesa la conexion de un cliente
typedef struct
{
    int sock; // numero de socket
    int idx;  // posicion a donde escribir en el array
} connection_t;

// mutex para proteger el array 'client_status' y el indice 'idx'
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int debug = 1;

int main(int argc, char **argv)
{

    int sock = -1;
    struct sockaddr_in address;
    int port = 8000;
    int backlog = 10;

    // ya no las utilizo, utilizo la estructura connection_t
    // int sockdup;

    // ya no las utilizo, utilizo la estructura connection_t
    // variables para almacenar lo que nos envia el cliente
    // int len;
    // char *buffer;

    // variables para hacer nuestro programa multithread
    pthread_t arr_tid[STATUS_SIZE];

    // indicador de array lleno
    int full = 0;

    // puntero a estructura a enviar a los threads
    connection_t *connection;

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
        pthread_mutex_lock(&mutex);
        if (idx < STATUS_SIZE)
            full = 0;
        else
            full = 1;
        pthread_mutex_unlock(&mutex);

        if (full == 0)
        {
            // accept incoming connections
            connection = (connection_t *)malloc(sizeof(connection_t));
            connection->sock = aceptar_pedidos(sock, debug);
            if (connection->sock > 0)
            {
                pthread_mutex_lock(&mutex);
                connection->idx = idx;
                pthread_create(&arr_tid[idx], 0, process, (void *)connection);
                pthread_detach(arr_tid[idx]);
                idx++;
                pthread_mutex_unlock(&mutex);
            }
            else
            {
                free(connection);
            }
        }
        else
        {
            // tengo que esperar que el thread 'monitor' vacie el array (en el proximo paso)
            usleep(2500);
            if (debug)
                fprintf(stderr, "debug:: main() array full, esperando que se vacie....\n");
        }
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

void *process(void *ptr)
{
    char *buffer;
    int len;
    connection_t *conn;

    if (!ptr)
        pthread_exit(0);
    conn = (connection_t *)ptr;

    // read length of message
    read(conn->sock, &len, sizeof(int));
    // network byte order to host byte order
    len = ntohl(len);

    if (len > 0)
    {
        buffer = (char *)malloc((len + 1) * sizeof(char));
        buffer[len] = 0;

        // read message
        read(conn->sock, buffer, len);

        // save message to array
        sscanf(buffer, "%[^;];%d;%d", client_status[conn->idx].hostname, &client_status[conn->idx].cpu, &client_status[conn->idx].mem);

        if (debug)
            printf("debug:: process() recibido...[len=%d][message=%s]\n", len, buffer);

        free(buffer);
    }

    // close socket and clean up
    close(conn->sock);
    free(conn);
    pthread_exit(0);
}
