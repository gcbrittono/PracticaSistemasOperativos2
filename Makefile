all: ejecutableServ ejecutableClient exeServidor clear

ejecutableServ:servidor.c
	gcc servidor.c -o servidor -lpthread

ejecutableClient:cliente.c
	gcc cliente.c -o cliente
exeServidor:servidor
	./servidor
clear:
	rm servidor
	rm cliente
