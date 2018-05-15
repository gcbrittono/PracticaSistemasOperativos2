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

#define HASH_SIZE 1000
#define BACKLOG 8
#define MSGSIZE 32
#define STRUCTSIZE sizeof(struct DogType)

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



//Definimos constantes para cuadrar el numero de clientes/hilos que se utilizaran al tiempo
#define NUM_CLIENTES 5 


//-----------------------------------------------------------------------------------------------------------------------------------------
//  Definición de la función de los hilos	
//-----------------------------------------------------------------------------------------------------------------------------------------

void *connection_handler(void *sock_id){
	//adquiere el identificador para el socket	
	int clientfd = (intptr_t)sock_id;
	int r;
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

//-----------------------------------------------------------------------------------------------------------------------------------------
// configuracion para poder utilizar el sockect repetidas veces
//-----------------------------------------------------------------------------------------------------------------------------------------
	
	if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0){
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

//-----------------------------------------------------------------------------------------------------------------------------------------
// El socket entra en modo escucha hasta aceptar la conexion
//-----------------------------------------------------------------------------------------------------------------------------------------

	r = listen(servfd, BACKLOG);
	if(r == -1){
		perror("Error en listen");
		exit(-1);
	}

//-----------------------------------------------------------------------------------------------------------------------------------------
// Se conectan el cliente y el servidor
//-----------------------------------------------------------------------------------------------------------------------------------------


	clientfd = accept(servfd, (struct sockaddr *)&client, &tama);
	if(clientfd == -1){
		perror("Error en accept");
		exit(-1);
	}



//-----------------------------------------------------------------------------------------------------------------------------------------
// Envio de informacion
//-----------------------------------------------------------------------------------------------------------------------------------------



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

	close(clientfd);
	close(servfd);

	return 0;
}
