all: ejecutableServ ejecutableClient exeServidor clear

ejecutableServ:proyecto.c
	gcc proyecto.c -o servidor -lpthread

ejecutableClient:cliente.c
	gcc cliente.c -o cliente
exeServidor:servidor
	./servidor
clear:
	rm servidor
	rm cliente
