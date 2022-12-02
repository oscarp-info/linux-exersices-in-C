/*
 * autor: Oscar Paniagua
 * fecha: 25/11/2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// cantidad por default de threads a lanzar (cantidad de pruebas)
#define TESTS 100

// estructura para pasar a cada thread
typedef struct
{
    char host[100];
    int port;
    int len;
    char message[100];
} unit_test_t;

void *work(void *args);

int main(int argc, char **argv)
{
    int port;
    time_t t;
    unit_test_t arr_test[TESTS];
    int i;
    int cpu, mem;
    pthread_t arr_tid[TESTS];
    int quantity = TESTS;

    srand((unsigned)time(&t));

    if (argc <= 3)
    {
        fprintf(stderr, "error:: use %s hostname port quantity\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (sscanf(argv[2], "%d", &port) <= 0)
    {
        fprintf(stderr, "error: wrong parameter: port %d\n", atoi(argv[2]));
        return EXIT_FAILURE;
    }

    // quantity opcional
    if (argc == 4)
    {
        quantity = atoi(argv[3]);

        if (quantity <= 0)
        {
            fprintf(stderr, "error: wrong parameter: quantity %d ...using default %d\n", atoi(argv[3]), TESTS);
            quantity = TESTS;
        }
        else if (quantity > TESTS)
        {
            fprintf(stderr, "error: wrong parameter: quantity %d ...max value %d\n", atoi(argv[3]), TESTS);
            fprintf(stderr, "error: wrong parameter: quantity %d ...using default %d\n", atoi(argv[3]), TESTS);
            quantity = TESTS;
        }
    }

    for (i = 0; i < quantity; i++)
    {
        // create a message to send
        arr_test[i].port = port;
        strcpy(arr_test[i].host, argv[1]);
        cpu = rand() % 100 + 1;
        mem = rand() % 100 + 1;
        memset(arr_test[i].message, 0, 100);
        sprintf(arr_test[i].message, "host%d;%d;%d", i, cpu, mem);
        arr_test[i].len = strlen(arr_test[i].message); // TODO: network order

        // create and join thread
        pthread_create(&arr_tid[i], NULL, work, (void *)&arr_test[i]);
        pthread_join(arr_tid[i], NULL);
    }

    return 0;
}

void *work(void *args)
{
    int sock;
    struct sockaddr_in address;
    struct hostent *host;

    unit_test_t *test = (unit_test_t *)args;

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock <= 0)
    {
        fprintf(stderr, "%s: error: cannot create socket\n", test->host);
        pthread_exit(0);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(test->port);
    host = gethostbyname(test->host);
    if (!host)
    {
        fprintf(stderr, "error: unknown host %s\n", test->host);
        pthread_exit(0);
    }

    memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
    if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
    {
        fprintf(stderr, "error: cannot connect to host %s\n", test->host);
        pthread_exit(0);
    }

    // send len(integer)
    // convert to network byte order
    // test->len = htonl(test->len);

    // ojo que aca lo tengo que mandar como network byte order
    int len_tmp = htonl(test->len);
    write(sock, &len_tmp, sizeof(len_tmp));
    // send message (string), aca la longitud para la funcion no tiene que ser network byte order
    // por eso utilice antes una variable temporal
    write(sock, test->message, test->len);
    close(sock);
    pthread_exit(0);
}
