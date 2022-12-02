#
# Oscar Paniagua
#
# Makefile sockets cliente - servidor
#

CC = gcc
LIBS =  -lpthread
CFLAGS = -Wall
OBJS_SRV =  servidor.o sock-lib.o
OBJS_CLT =  cliente.o sock-lib.o

all : servidor cliente

%.o: %.c %.h
	gcc $(CFLAGS) -o $@ -c $<

servidor: ${OBJS_SRV}
	${CC} ${CFLAGS} -o $@ ${OBJS_SRV} ${LIBS}

cliente: ${OBJS_CLT}
	${CC} ${CFLAGS} -o $@ ${OBJS_CLT} ${LIBS}

run_servidor:
	./servidor

run_cliente:
	./cliente localhost 8000

clean:
	rm -f servidor cliente *.o *.d status.log