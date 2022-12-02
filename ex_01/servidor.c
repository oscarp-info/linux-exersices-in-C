/*
 * autor: Oscar Paniagua
 * fecha: 25/11/2022
 *
 * verificar:
 *      $ netstat -na | grep 8000
 *      tcp        0      0 0.0.0.0:8000            0.0.0.0:*               LISTEN
 *
 *      $ lsof -i :8000
 *      COMMAND   PID   USER   FD   TYPE DEVICE SIZE/OFF NODE NAME
 *      servidor 3598 devops    3u  IPv4  19972      0t0  TCP *:8000 (LISTEN)
 *
 *  recuerden:
 *      - NO HACER lock de mutex y despues llamar a una funcion bloqueante, ejemplo read() o aceptar_pedidos()
 *          ya que bloque todo el codigo donde tengamos look/unlook de ese mutex
 *      - PARA ENVIAR/RECIBIR integer utilizar network byte order
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

#define DEFAULT_PORT 8000 // puerto por default
#define BACKLOG 10        // tamaño de la cola de conexiones recibidas

typedef struct
{
    int port;
} config_t;

typedef struct
{
    int sock;
    int idx; // posicion a donde escribir en el array
} connection_t;

#define HOSTNAME_LEN 64
#define STATUS_SIZE 10
// mutex e indice del array 'client_status'
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int idx = 0;
typedef struct
{
    char hostname[HOSTNAME_LEN];
    int cpu;
    int mem;
} status_t;
status_t client_status[STATUS_SIZE];

void get_arg(int argc, char **argv, config_t *arg);
void *process(void *ptr);
void *monitor(void *ptr);
void dump(void);

int debug = 1;

int main(int argc, char **argv)
{
    config_t config; // estructura de configuracion de la aplicacion
    int sock = -1;   // puerto principal del servidor
    struct sockaddr_in address;
    connection_t *connection;
    pthread_t arr_tid[STATUS_SIZE];
    pthread_t monitor_tid;

    int full = 0;

    // obtiene los argumentos del main y actualiza los valores por default si corresponde
    get_arg(argc, argv, &config);

    sock = open_conection(&address, config.port, 0, debug);

    if (debug)
        fprintf(stderr, "debug:: main() ready and listening\n");

    // lanzo el thread que monitorea el array
    pthread_create(&monitor_tid, NULL, monitor, NULL);
    pthread_detach(monitor_tid);

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
                // start a new thread but do not wait for it
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
            // tengo que esperar que el thread 'monitor' vacie el array
            usleep(2500);
        }
    }

    return 0;
}

void get_arg(int argc, char **argv, config_t *arg)
{
    // set default values
    arg->port = DEFAULT_PORT;

    // $./servidor port 8001
    if (argc == 3)
    {
        // verifico que el segundo parametro sea 'port'
        if (strcmp(argv[1], "port") == 0)
        {
            // obtengo el port
            arg->port = atoi(argv[2]);
        }
    }
    if (debug)
        fprintf(stderr, "debug:: main() usando puerto: %d\n", arg->port);
}

void *process(void *ptr)
{
    char *buffer;
    int len;
    connection_t *conn;
    // long addr = 0;

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
        // ejemplo: "host9;96;41"
        // hace falta el mutex? ya tengo la posicion en la estructura conn?...???
        // pthread_mutex_lock(&mutex);
        sscanf(buffer, "%[^;];%d;%d", client_status[conn->idx].hostname, &client_status[conn->idx].cpu, &client_status[conn->idx].mem);
        // pthread_mutex_unlock(&mutex);

        if (debug)
            printf("debug:: process() recibido...[len=%d][message=%s]\n", len, buffer);

        free(buffer);
    }

    // close socket and clean up
    close(conn->sock);
    free(conn);
    pthread_exit(0);
}

void *monitor(void *ptr)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (idx == STATUS_SIZE)
        {
            dump();  // guardo el log
            idx = 0; // vacio el array
        }
        pthread_mutex_unlock(&mutex);
        usleep(2500);
    }

    return NULL;
}

void dump(void)
{
    int i = 0;
    FILE *fp;

    // genero el string con la fecha y hora, se guarda en 'buffer'
    char buffer[80];
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info = localtime(&rawtime);
    strftime(buffer, 80, "%Y-%m-%d;%H:%M", info);

    // error grave si se hace lock aca, porque ya esta lock en monitor
    // pthread_mutex_lock(&mutex);

    if ((fp = fopen("status.log", "a")) != NULL)
    {
        for (i = 0; i < STATUS_SIZE; i++)
        {
            if (client_status[i].cpu > 50 || client_status[i].mem > 50)
            {

                fprintf(fp, "%s;%s; cpu=%%%d;memory=%%%d\n",
                        buffer, client_status[i].hostname, client_status[i].cpu, client_status[i].mem);
            }
        }
        fclose(fp);
    }
    // error grave si se hace lock aca, porque ya esta lock en monitor
    // pthread_mutex_unlock(&mutex);

    return;
}