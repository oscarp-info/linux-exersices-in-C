# PRACTICA DE C

## Enunciado

Por favor lee el [ENUNCIADO.md](https://github.com/oscarp-info/linux-exersices-in-C/blob/main/ENUNCIADO.md)

## Funciones de socket sock-lib.c

Tener en cuenta que se hicieron algunas modificaciones a la version original:

* Se cambió de nombre de la funcion Open_conection por open_conection
* Se cambió de nombre de la funcion  Aceptar_pedidos por aceptar_pedidos
* En la funcion open_conection se utiliza setsockopt() para eliminar el timeout del puerto al finalizar con Ctrl-C
* Las funciones tienen un parametro 'debug' 0 o 1, para activar mensajes en la consola
* Se puede configurar el numero de puerto y el backlog en la funcion open_conection()



## Paso a paso

* Paso (1): Aun no empezamos a desarrollar, pero probamos el cliente con dos herramientas: nc y xxd

* Paso (2): Version minimalista del servidor(socket), imprime por pantalla lo que envia el cliente

* Paso (3): Funcionalidad para guardar la informacion recibida en el array de estructuras(global)

* Paso (4): Version multithread

* Paso (5): Funcionalidad 'monitor' para hacer un dump del array y vaciarlo

* Paso (6): Version final




