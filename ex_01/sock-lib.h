/*
 * cambió de nombre de la funcion Open_conection por open_conection
 * cambió de nombre de la funcion  Aceptar_pedidos por aceptar_pedidos
 * en la funcion open_conection se utiliza setsockopt() para eliminar el timeout del puerto al finalizar con Ctrl-C
 * las funciones tienen un parametro 'debug' 0 o 1, para activar mensajes en la consola
 * se puede configurar el numero de puerto y el backlog en la funcion open_conection()
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 8000  /* El puerto donde se conectará, servidor */
#define BACKLOG 10 /* Tamaño de la cola de conexiones recibidas */

int conectar(int, char **);

int open_conection(struct sockaddr_in *, int port, int backlog, int debug); // función que crea la conexión

int aceptar_pedidos(int, int debug); // función que acepta una conexión entrante
