//Cliente
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 3535
#define MSGSIZE 32
#define STRUCTSIZE sizeof(struct DogType)

//Definimos constantes para cuadrar el numero de clientes/hilos que se utilizaran al tiempo
#define NUM_CLIENTES 5 


//-----------------------------------------------------------------------------------------------------------------------------------------
//  Definición de la función de los hilos	
//-----------------------------------------------------------------------------------------------------------------------------------------

void *connection_handler(void *threadid){
	int threadnum = (intptr_t)threadid;
	int clientfd, r;
	char msg[32];

	//Esta estructura gurada la información concerniente al servidor con el cual el cliente quiere conectarse
	struct sockaddr_in serv_addr;


//-----------------------------------------------------------------------------------------------------------------------------------------
//  Creacion del socket() clientfd	
//-----------------------------------------------------------------------------------------------------------------------------------------

	clientfd = socket(AF_INET,SOCK_STREAM,0);
	//validar
	if(clientfd==-1){
		perror("Error en socket del clientfd\n");
		exit(-1);
	}	
	serv_addr.sin_family= AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr= inet_addr("127.0.0.1");
	bzero(serv_addr.sin_zero,8);



//-----------------------------------------------------------------------------------------------------------------------------------------
// Se conectan el cliente y el servidor
//-----------------------------------------------------------------------------------------------------------------------------------------


	r=connect(clientfd,(struct sockaddr*)&serv_addr, sizeof(serv_addr));
	//validar
	if(r<0){
		perror("Error en connect de r\n");
		exit(-1);
	}


	printf("Conexión exitosa cliente número:%d\n ", threadnum);
	while(1){
		printf("Conexion con el cliente: %d\n", threadnum);
		printf("Escribir mensaje");
		scanf("%s",msg);	

//-----------------------------------------------------------------------------------------------------------------------------------------
// Envio de informacion
//-----------------------------------------------------------------------------------------------------------------------------------------
		r = send(clientfd, msg, MSGSIZE,0);
		if(r == -1){
			perror("Error en send");
			exit(-1);	
		}
	
	sleep(2);
	}

}




int main(){
	int clientfd, new_socket, c, *new_sock, i;
	pthread_t cliente_hilo;
	for(i = 1; i <= NUM_CLIENTES; i++){
		if(pthread_create(&cliente_hilo, NULL, connection_handler, (void*)&i)<0){
		perror("No se pudo crear el hilo");
		exit(-1);	
		}
	}
	pthread_exit(NULL);


/*
	int clientfd,r;
	char msg[32];

	struct sockaddr_in client;
	socklen_t tama=sizeof(struct sockaddr);


//-----------------------------------------------------------------------------------------------------------------------------------------
//  Creacion del socket() clientfd	
//-----------------------------------------------------------------------------------------------------------------------------------------

	clientfd = socket(AF_INET,SOCK_STREAM,0);
	//validar
	if(clientfd==-1){
		perror("Error en socket del clientfd\n");
		exit(-1);
	}	
	client.sin_family= AF_INET;
	client.sin_port = htons(PORT);
	client.sin_addr.s_addr= inet_addr("127.0.0.1");
	bzero(client.sin_zero,8);



//-----------------------------------------------------------------------------------------------------------------------------------------
// Se conectan el cliente y el servidor
//-----------------------------------------------------------------------------------------------------------------------------------------


	r=connect(clientfd,(struct sockaddr*)&client, tama);
	//validar
	if(r==-1){
		perror("Error en connect de r\n");
		exit(-1);
	}




//-----------------------------------------------------------------------------------------------------------------------------------------
// Envio de informacion
//-----------------------------------------------------------------------------------------------------------------------------------------


	r = send(clientfd, "Conexion Exitosa", MSGSIZE,0);
	if(r == -1){
		perror("Error en send");
		exit(-1);
	}	


	while(true){
		printf("Su mensaje es:");
		scanf("%s",msg);	

		r = send(clientfd, msg, MSGSIZE,0);
		if(r == -1){
			perror("Error en send");
			exit(-1);
		}
	}

*/
	close(clientfd);
	return 0;
}
