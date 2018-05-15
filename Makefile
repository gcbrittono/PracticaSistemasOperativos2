all: ejecutableServ ejecutableClient exeServidor clear

ejecutableServ:servidor2.c
	gcc servidor2.c -o servidor

ejecutableClient:cliente2.c
	gcc cliente2.c -o cliente
exeServidor:servidor
	./servidor
clear:
	rm servidor
	rm cliente
