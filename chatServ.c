#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>


#define HASH_SIZE 1000
#define BACKLOG 8
#define MSGSIZE 32
#define STRUCTSIZE sizeof(struct DogType)
#define NUMTHREADS 8


/*struct sockaddr_in{
	short sin_family;
	unsigned short sin_port;
	struct in_addr sind_addr;
	char sin_zero[8] ;
};
struct in_addr{
	unsigned long s_addr;
};
*/

int cantidad_clientes;

//Funcion para cada uno de los hilos para el control de las conexiones con los clientes
void *function(void *sock_id){
//intercambio de información entre cliente y serviidor
	
	//Extrae información del descriptor del socket
	int sock = *(int*)sock_id;
	int read_size;
	char *message, client_message[2000];

	message = "Prueba para ver si existe conexión exitosa";
	write(sock, message, strlen(message));
//	send(sock, message, strlen(message),0);

	while((read_size = recv(sock, client_message,2000,0))>0){
		write(sock, client_message, strlen(client_message));
		if(client_message[0] == '*'){
			break;
		}		
		puts(client_message);

	}
	close(sock);
	cantidad_clientes--;
	
}



int main(){
	
	//creacion de hilos relacionados con el numero de clientes
	pthread_t hilo[NUMTHREADS];
	int i,data[NUMTHREADS];

	
	socklen_t tama;
	struct sockaddr_in server, client;
	int servfd, clientfd, r; 
	char msg[32];

	tama = sizeof(struct sockaddr);
//-----------------------------------------------------------------------------------------------------------------------------------------
//  Creacion del socket() servfd	
//-----------------------------------------------------------------------------------------------------------------------------------------
	servfd = socket(AF_INET, SOCK_STREAM, 0);
	if(servfd == -1){
		perror("Error en socket");
		exit(-1);
	}

	puts("Creacion del socket");

//-----------------------------------------------------------------------------------------------------------------------------------------
// configuracion para poder utilizar el sockect repetidas veces
//-----------------------------------------------------------------------------------------------------------------------------------------
	
	if ((setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int))) < 0){
    		perror("setsockopt(SO_REUSEADDR) failed");
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(3535);
	server.sin_addr.s_addr = INADDR_ANY;
	bzero(server.sin_zero,8);
	tama = sizeof(struct sockaddr_in);

//-----------------------------------------------------------------------------------------------------------------------------------------
//creacion bind
//-----------------------------------------------------------------------------------------------------------------------------------------

	r = bind(servfd, (struct sockaddr *)&server, tama);
	
	if(r == -1){
		perror("Error en bind");
		exit(-1);
	}
	puts("Bind del socket creado");
//-----------------------------------------------------------------------------------------------------------------------------------------
// El socket entra en modo escucha hasta aceptar la conexion
//-----------------------------------------------------------------------------------------------------------------------------------------

	r = listen(servfd, BACKLOG);
	if(r == -1){
		perror("Error en listen");
		exit(-1);
	}
	puts("Esperando conexion...");
//-----------------------------------------------------------------------------------------------------------------------------------------
// Se conectan el cliente y el servidor
//-----------------------------------------------------------------------------------------------------------------------------------------


	while(true){
		clientfd = accept(servfd, (struct sockaddr *)&client, &tama);
		cantidad_clientes++;		
		if(clientfd == -1){
			perror("Error en accept");
			exit(-1);
		}
		printf("Conexión aceptada\nCantidad clientes:%i",cantidad_clientes);
		

	//	for ( i = 0 ; i < NUMTHREADS ; i ++){
	//		data[i] = i;
			if(pthread_create(&hilo[1], NULL , function , (void*) &clientfd)<0){
				perror("Error al crear los hilos");
				exit(-1);
			}

		//}
	}

//-----------------------------------------------------------------------------------------------------------------------------------------
// Envio de informacion
//-----------------------------------------------------------------------------------------------------------------------------------------


/*
	r=recv(clientfd,msg,MSGSIZE,0);
	if(r==-1){
		perror("Error en recv de r\n");
		exit(-1);
	}



	printf("%s\n",msg);
	while(true){
		r=recv(clientfd,msg,MSGSIZE,0);
		if(r==-1){
			perror("Error en recv de r\n");
			exit(-1);
		}
		printf("%s\n",msg);
	}
*/
	//close(clientfd);
	//close(servfd);

	return 0;
}
